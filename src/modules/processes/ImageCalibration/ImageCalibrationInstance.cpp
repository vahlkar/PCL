//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.5.1
// ----------------------------------------------------------------------------
// ImageCalibrationInstance.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
//
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (https://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#include "ImageCalibrationInstance.h"
#include "OutputFileData.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/AutoPointer.h>
#include <pcl/ErrorHandler.h>
#include <pcl/IntegerResample.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/MuteStatus.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/SpinStatus.h>
#include <pcl/Version.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * 5x5 B3-spline wavelet scaling function. Used by the noise estimation routine
 * for dark frame optimization.
 *
 * Kernel filter coefficients:
 *
 *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256,
 *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
 *   3.0/128, 3.0/32, 9.0/64,  3.0/32, 3.0/128,
 *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
 *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256
 */
static const float g_5x5B3Spline[] =
{
   0.003906F, 0.015625F, 0.023438F, 0.015625F, 0.003906F,
   0.015625F, 0.062500F, 0.093750F, 0.062500F, 0.015625F,
   0.023438F, 0.093750F, 0.140625F, 0.093750F, 0.023438F,
   0.015625F, 0.062500F, 0.093750F, 0.062500F, 0.015625F,
   0.003906F, 0.015625F, 0.023438F, 0.015625F, 0.003906F
};
static const float g_5x5B3Spline_hv[] = { 0.0625F, 0.25F, 0.375F, 0.25F, 0.0625F };
static const float g_5x5B3Spline_kj[] =
{ 0.8907F, 0.2007F, 0.0856F, 0.0413F, 0.0205F, 0.0103F, 0.0052F, 0.0026F, 0.0013F, 0.0007F };

/*
 * Maximum dark scaling factor to issue a 'no correlation' warning.
 */
#define NO_DARK_CORRELATION   0.005F

/*
 * Maximum fraction of surviving pixels after optimization thresholding to
 * issue an 'optimization threshold too high' warning.
 */
#define DARK_COUNT_LOW        0.001

/*
 * Minimum cardinality of a usable dark frame optimization set. Smaller sets
 * disable dark frame optimization.
 */
#define DARK_COUNT_SMALL      16

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ImageCalibrationInstance::ImageCalibrationInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_enableCFA( TheICEnableCFAParameter->DefaultValue() )
   , p_cfaPattern( ICCFAPattern::Default )
   , p_inputHints( TheICInputHintsParameter->DefaultValue() )
   , p_outputHints( TheICOutputHintsParameter->DefaultValue() )
   , p_pedestal( TheICPedestalParameter->DefaultValue() )
   , p_pedestalMode( ICPedestalMode::Default )
   , p_calibrateBias( TheICCalibrateBiasParameter->DefaultValue() )
   , p_calibrateDark( TheICCalibrateDarkParameter->DefaultValue() )
   , p_calibrateFlat( TheICCalibrateFlatParameter->DefaultValue() )
   , p_optimizeDarks( TheICOptimizeDarksParameter->DefaultValue() )
   , p_darkOptimizationThreshold( TheICDarkOptimizationThresholdParameter->DefaultValue() ) // ### DEPRECATED
   , p_darkOptimizationLow( TheICDarkOptimizationLowParameter->DefaultValue() )
   , p_darkOptimizationWindow( TheICDarkOptimizationWindowParameter->DefaultValue() )
   , p_darkCFADetectionMode( ICDarkCFADetectionMode::Default ) // ### DEPRECATED
   , p_separateCFAFlatScalingFactors( TheICSeparateCFAFlatScalingFactorsParameter->DefaultValue() )
   , p_flatScaleClippingFactor( TheICFlatScaleClippingFactorParameter->DefaultValue() )
   , p_evaluateNoise( TheICEvaluateNoiseParameter->DefaultValue() )
   , p_noiseEvaluationAlgorithm( ICNoiseEvaluationAlgorithm::Default )
   , p_outputDirectory( TheICOutputDirectoryParameter->DefaultValue() )
   , p_outputExtension( TheICOutputExtensionParameter->DefaultValue() )
   , p_outputPrefix( TheICOutputPrefixParameter->DefaultValue() )
   , p_outputPostfix( TheICOutputPostfixParameter->DefaultValue() )
   , p_outputSampleFormat( ICOutputSampleFormat::Default )
   , p_outputPedestal( TheICOutputPedestalParameter->DefaultValue() )
   , p_overwriteExistingFiles( TheICOverwriteExistingFilesParameter->DefaultValue() )
   , p_onError( ICOnError::Default )
   , p_noGUIMessages( TheICNoGUIMessagesParameter->DefaultValue() ) // ### DEPRECATED
{
}

// ----------------------------------------------------------------------------

ImageCalibrationInstance::ImageCalibrationInstance( const ImageCalibrationInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInstance::Assign( const ProcessImplementation& p )
{
   const ImageCalibrationInstance* x = dynamic_cast<const ImageCalibrationInstance*>( &p );
   if ( x != nullptr )
   {
      p_targetFrames                  = x->p_targetFrames;
      p_enableCFA                     = x->p_enableCFA;
      p_cfaPattern                    = x->p_cfaPattern;
      p_inputHints                    = x->p_inputHints;
      p_outputHints                   = x->p_outputHints;
      p_pedestal                      = x->p_pedestal;
      p_pedestalMode                  = x->p_pedestalMode;
      p_pedestalKeyword               = x->p_pedestalKeyword;
      p_overscan                      = x->p_overscan;
      p_masterBias                    = x->p_masterBias;
      p_masterDark                    = x->p_masterDark;
      p_masterFlat                    = x->p_masterFlat;
      p_calibrateBias                 = x->p_calibrateBias;
      p_calibrateDark                 = x->p_calibrateDark;
      p_calibrateFlat                 = x->p_calibrateFlat;
      p_optimizeDarks                 = x->p_optimizeDarks;
      p_darkOptimizationThreshold     = x->p_darkOptimizationThreshold;
      p_darkOptimizationLow           = x->p_darkOptimizationLow;
      p_darkOptimizationWindow        = x->p_darkOptimizationWindow;
      p_darkCFADetectionMode          = x->p_darkCFADetectionMode;
      p_separateCFAFlatScalingFactors = x->p_separateCFAFlatScalingFactors;
      p_flatScaleClippingFactor       = x->p_flatScaleClippingFactor;
      p_evaluateNoise                 = x->p_evaluateNoise;
      p_noiseEvaluationAlgorithm      = x->p_noiseEvaluationAlgorithm;
      p_outputDirectory               = x->p_outputDirectory;
      p_outputExtension               = x->p_outputExtension;
      p_outputPrefix                  = x->p_outputPrefix;
      p_outputPostfix                 = x->p_outputPostfix;
      p_outputSampleFormat            = x->p_outputSampleFormat;
      p_outputPedestal                = x->p_outputPedestal;
      p_overwriteExistingFiles        = x->p_overwriteExistingFiles;
      p_onError                       = x->p_onError;
      p_noGUIMessages                 = x->p_noGUIMessages;
      o_output                        = x->o_output;
   }
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "ImageCalibration can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_targetFrames.IsEmpty() )
   {
      whyNot = "No target frames have been specified.";
      return false;
   }

   if ( !p_overscan.IsValid() )
   {
      whyNot = "Invalid overscan region(s) defined.";
      return false;
   }

   if ( !p_masterBias.IsValid() )
   {
      whyNot = "Missing or invalid master bias frame.";
      return false;
   }

   if ( !p_masterDark.IsValid() )
   {
      whyNot = "Missing or invalid master dark frame.";
      return false;
   }

   if ( !p_masterFlat.IsValid() )
   {
      whyNot = "Missing or invalid master flat frame.";
      return false;
   }

   if ( !p_overscan.enabled && !p_masterBias.enabled && !p_masterDark.enabled && !p_masterFlat.enabled )
   {
      whyNot = "No master calibration frames or overscan regions have been specified.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Calibration Primitives
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Overscan subtraction
 */
static void SubtractOverscan( Image& target, const ImageCalibrationInstance::overscan_table& O, const Rect& r )
{
   target.SetRangeClipping( 0.00002, 0.99998 ); // ignore black and saturated pixels

   for ( ImageCalibrationInstance::overscan_table::const_iterator o = O.Begin(); o != O.End(); ++o )
   {
      FVector v( 0.0F, target.NumberOfChannels() );
      for ( ImageCalibrationInstance::overscan_list::const_iterator r = o->Begin(); r != o->End(); ++r )
         for ( int c = 0; c < target.NumberOfChannels(); ++c )
            v[c] += target.Median( r->sourceRect, c, c );
      v /= o->Length();

      target.SelectRectangle( o->Begin()->targetRect );
      for ( int c = 0; c < target.NumberOfChannels(); ++c )
         if ( 1 + v[c] != 1 )
         {
            target.SelectChannel( c );
            target -= v[c];
         }
   }

   target.ResetSelections();

   if ( r != target.Bounds() )
   {
      target.SelectRectangle( r );
      target.Crop();
   }
}

// ----------------------------------------------------------------------------

#define LOOP         while ( t < t1 )
#define BIAS         *b++
#define DARK         *d++
#define SCALED_DARK  k * *d++
#define FLAT         Max( *f++, TINY )

// ----------------------------------------------------------------------------

/*
 * Bias subtraction
 */
static void SubtractBias( Image& target, const Image& bias )
{
   for ( int c = 0; c < target.NumberOfChannels(); ++c )
   {
            float* t  = target.PixelData( c );
            float* t1 = t + target.NumberOfPixels();
      const float* b  = bias.PixelData( Min( c, bias.NumberOfChannels()-1 ) );
      LOOP *t++ -= BIAS;
   }
}

// ----------------------------------------------------------------------------

/*
 * Dark subtraction
 */
/* -- Currently this routine is not used
static void SubtractDark( Image& target, const Image& dark, const FVector& dScale )
{
   for ( int c = 0; c < target.NumberOfChannels(); ++c )
   {
            float* t  = target.PixelData( c );
            float* t1 = t + target.NumberOfPixels();
      const float* d  = dark.PixelData( Min( c, dark.NumberOfChannels()-1 ) );
            float  k  = dScale[c];
      if ( k != 1 )
         LOOP *t++ -= SCALED_DARK;
      else
         LOOP *t++ -= DARK;
   }
}
*/

// ----------------------------------------------------------------------------

/*
 * One-channel dark subtraction with scaling
 */
static void SubtractOneChannelDark( Image& target, int tCh, const Image& dark, int dCh, float k )
{
         float* t  = target.PixelData( tCh );
         float* t1 = t + target.NumberOfPixels();
   const float* d  = dark.PixelData( dCh );
   LOOP *t++ -= SCALED_DARK;
}

// ----------------------------------------------------------------------------

/*
 * Full calibration routine
 */
static void Calibrate( Image& target,
                       const Image* bias,
                       const Image* dark, const FVector& dScale,
                       const Image* flat,
                       float        pedestal = 0 )
{
   static const float TINY = 1.0e-15F; // to prevent divisions by zero flat pixels

   int nt = target.NumberOfChannels();
   int nb = (bias != nullptr) ? bias->NumberOfChannels() : 0;
   int nd = (dark != nullptr) ? dark->NumberOfChannels() : 0;
   int nf = (flat != nullptr) ? flat->NumberOfChannels() : 0;

   for ( int c = 0; c < nt; ++c )
   {
      Module->ProcessEvents();

            float* t  = target.PixelData( c );
            float* t1 = t + target.NumberOfPixels();
      const float* b  = (bias != nullptr) ? bias->PixelData( Min( c, nb-1 ) ) : nullptr;
      const float* d  = (dark != nullptr) ? dark->PixelData( Min( c, nd-1 ) ) : nullptr;
      const float* f  = (flat != nullptr) ? flat->PixelData( Min( c, nf-1 ) ) : nullptr;
            float  k  = (dark != nullptr) ? dScale[c] : 0;

      if ( b != nullptr )
      {
         if ( d != nullptr )
         {
            if ( k != 1 )
            {
               if ( f != nullptr )
                  LOOP *t = (*t - BIAS - SCALED_DARK)/FLAT, ++t;
               else
                  LOOP *t++ -= BIAS + SCALED_DARK;
            }
            else
            {
               if ( f != nullptr )
                  LOOP *t = (*t - BIAS - DARK)/FLAT, ++t;
               else
                  LOOP *t++ -= BIAS + DARK;
            }
         }
         else
         {
            if ( f != nullptr )
               LOOP *t = (*t - BIAS)/FLAT, ++t;
            else
               LOOP *t++ -= BIAS;
         }
      }
      else
      {
         if ( d != nullptr )
         {
            if ( k != 1 )
            {
               if ( f != nullptr )
                  LOOP *t = (*t - SCALED_DARK)/FLAT, ++t;
               else
                  LOOP *t++ -= SCALED_DARK;
            }
            else
            {
               if ( f != nullptr )
                  LOOP *t = (*t - DARK)/FLAT, ++t;
               else
                  LOOP *t++ -= DARK;
            }
         }
         else
         {
            if ( f != nullptr )
               LOOP *t++ /= FLAT;
         }
      }
   } // for each channel

   /*
    * Optional pedestal to enforce positivity.
    */
   if ( pedestal != 0 )
      target += pedestal;

   /*
    * Constrain the calibrated target image to the [0,1] range.
    */
   target.Truncate();
}

#undef LOOP
#undef BIAS
#undef DARK
#undef SCALED_DARK
#undef FLAT

// ----------------------------------------------------------------------------

/*
 * Fast color filter array (CFA) pixel sample selection.
 */
class CFAIndex
{
public:

   CFAIndex() = default;
   CFAIndex( const CFAIndex& ) = default;

   CFAIndex( const IsoString& pattern )
   {
      if ( pattern.IsEmpty() )
         throw Error( "Empty CFA pattern." );
      m_size = int( Sqrt( pattern.Length() ) );
      if ( m_size < 2 )
         throw Error( "Invalid CFA pattern '" + pattern + '\'' );
      if ( m_size*m_size != pattern.Length() )
         throw Error( "Non-square CFA patterns are not supported: '" + pattern + '\'' );

      m_index = IMatrix( m_size, m_size );
      IsoString::const_iterator p = pattern.Begin();
      for ( int i = 0; i < m_size; ++i )
         for ( int j = 0; j < m_size; ++j )
         {
            int k;
            switch ( *p++ )
            {
            case 'R': k = 0; break;
            case 'G': k = 1; break;
            case 'B': k = 2; break;
            default:
               throw Error( "Invalid character(s) in CFA pattern '" + pattern + '\'' );
            }
            m_index[i][j] = k;
         }
   }

   int Size() const
   {
      return m_size;
   }

   int operator ()( int x, int y ) const
   {
      return m_index[y % m_size][x % m_size];
   }

private:

   int     m_size = 0;
   IMatrix m_index;
};

// ----------------------------------------------------------------------------

static bool IsXTransCFAFromTargetProperty( const Variant& cfaSourcePatternName )
{
   if ( cfaSourcePatternName.IsValid() )
      if ( cfaSourcePatternName.IsString() )
      {
         IsoString name = cfaSourcePatternName.ToIsoString().Trimmed().CaseFolded();
         return name == "x-trans" || name == "xtrans";
      }
   return false;
}

static IsoString CFAPatternFromTargetProperty( const Variant& cfaSourcePattern, int dx = 0, int dy = 0 )
{
   if ( cfaSourcePattern.IsValid() )
      if ( cfaSourcePattern.IsString() )
      {
         IsoString cfaPattern = cfaSourcePattern.ToIsoString().Trimmed();
         if ( dx || dy )
            if ( cfaPattern.Length() == 4 )
            {
               if ( dx % 2 )
               {
                  // Swap pattern columns
                  Swap( cfaPattern[0], cfaPattern[1] );
                  Swap( cfaPattern[2], cfaPattern[3] );
               }
               if ( dy % 2 )
               {
                  // Swap pattern rows
                  Swap( cfaPattern[0], cfaPattern[2] );
                  Swap( cfaPattern[1], cfaPattern[3] );
               }
            }

         return cfaPattern;
      }

   return IsoString();
}

/*
 * Returns an integer downsampling factor corresponding to the specified CFA
 * pattern, to be applied for noise evaluation:
 *
 * 2 for 2x2 Bayer CFA patterns
 * 3 for 6x6 X-Trans CFA patterns
 */
static int DownsamplingFactorForCFAPattern( const IsoString& cfaPattern )
{
   return (cfaPattern.Length() == 4) ? 2 : 3;
}

IsoString ImageCalibrationInstance::CFAPatternFromTarget( FileFormatInstance& file ) const
{
   bool xtrans = IsXTransCFAFromTargetProperty( file.ReadImageProperty( "PCL:CFASourcePatternName" ) );
   if ( xtrans || p_cfaPattern == ICCFAPattern::Auto )
   {
      if ( file.HasImageProperty( "PCL:CFASourcePattern" ) )
         return CFAPatternFromTargetProperty( file.ReadImageProperty( "PCL:CFASourcePattern" ) );

      if ( !xtrans )
         if ( file.Format().CanStoreKeywords() )
         {
            FITSKeywordArray keywords;
            if ( file.ReadFITSKeywords( keywords ) )
            {
               IsoString bayerPattern;
               double xOffset = 0, yOffset = 0;
               for ( const FITSHeaderKeyword& keyword : keywords )
               {
                  try
                  {
                     if ( keyword.name == "BAYERPAT" )
                        bayerPattern = keyword.StripValueDelimiters().Uppercase();
                     else if ( keyword.name == "XBAYROFF" )
                        xOffset = keyword.value.ToDouble();
                     else if ( keyword.name == "YBAYROFF" )
                        yOffset = keyword.value.ToDouble();
                  }
                  catch ( Exception& x )
                  {
                     Console().CriticalLn( "<end><cbr>*** Error: Parsing " + keyword.name + " FITS keyword: " + x.Message() );
                  }
                  catch ( ... )
                  {
                     throw;
                  }
               }

               if ( !bayerPattern.IsEmpty() )
                  return CFAPatternFromTargetProperty( bayerPattern, TruncInt( xOffset ), TruncInt( yOffset ) );
            }
         }

      return IsoString();
   }

   return TheICCFAPatternParameter->ElementId( p_cfaPattern );
}

// ----------------------------------------------------------------------------

/*
 * Quick estimation of noise sigma after dark subtraction by the iterative
 * k-sigma method.
 *
 * J.L. Stark F. Murtagh, Astronomical Image and Data Analysis, pp. 37-38.
 *
 * J.L. Stark et al., Automatic Noise Estimation from the Multiresolution
 *    Support Publications of the Royal Astronomical Society of the Pacific,
 *    vol. 110, February 1998, pp. 193-199
 *
 * Returns a noise estimate for (target - k*dark).
 */
static double TestDarkOptimization( float k, const Image& target, const Image& dark )
{
   Image t( target );
   t.Status().DisableInitialization();

   SubtractOneChannelDark( t, 0, dark, Min( target.SelectedChannel(), dark.NumberOfChannels()-1 ), k );

   ATrousWaveletTransform::WaveletScalingFunction H;
   if ( SeparableConvolution::FasterThanNonseparableFilterSize( Thread::NumberOfThreads( PCL_MAX_PROCESSORS ) ) > 5 )
      H.Set( KernelFilter( g_5x5B3Spline, 5 ) );
   else
      H.Set( SeparableFilter( g_5x5B3Spline_hv, g_5x5B3Spline_hv, 5 ) );

   ATrousWaveletTransform W( H );
   W.DisableLayer( 1 );
   W << t;
   return W.NoiseKSigma( 0 ) /*/g_5x5B3Spline_kj[0]*/; // we can work with unscaled noise estimates here.
}

// ----------------------------------------------------------------------------

/*
 * Initial bracketing of the dark optimization factor.
 *
 * Minimization routines based on Numerical Recipes in C, 2nd Edition
 * (section 10.1, pp. 397-402).
 */

template <typename T>
inline T SameSign( const T& x, const T& sameAs )
{
   return (sameAs < 0) ? ((x < 0) ? x : -x) : ((x < 0) ? -x : x);
}

#define TEST_DARK( x )  TestDarkOptimization( x, target, dark )

static void BracketDarkOptimization( float& ax, float& bx, float& cx, const Image& target, const Image& dark )
{
   Console console;
   SpinStatus spin;
   StatusMonitor monitor;
   monitor.SetCallback( &spin );
   monitor.Initialize( "Bracketing" );

   static const float GOLD = 1.618034;
   static const int GLIMIT = 10;
   static const float TINY = 1.0e-20F;

   // Start with the [1/2,2] interval
   ax = 0.5F;
   bx = 2.0F;
   float fa = TEST_DARK( ax );
   float fb = TEST_DARK( bx );
   if ( fb > fa )
   {
      Swap( ax, bx );
      Swap( fb, fa );
   }

   cx = bx + GOLD*(bx - ax);
   float fc = TEST_DARK( cx );
   while ( fb > fc )
   {
      ++monitor;
      Module->ProcessEvents();
      if ( console.AbortRequested() )
         throw ProcessAborted();

      float r = (bx - ax)*(fb - fc);
      float q = (bx - cx)*(fb - fa);
      float u = bx - ((bx - cx)*q - (bx - ax)*r)/2/SameSign( Max( Abs( q-r ), TINY ), q-r );
      float ulim = bx + GLIMIT*(cx - bx);
      float fu;
      if ( (bx - u)*(u - cx) > 0 )
      {
         fu = TEST_DARK( u );
         if ( fu < fc )
         {
            ax = bx;
            bx = u;
            break;
         }
         else if ( fu > fb )
         {
            cx = u;
            break;
         }
         u = cx + GOLD*(cx - bx);
         fu = TEST_DARK( u );
      }
      else if ( (cx - u)*(u - ulim) > 0 )
      {
         fu = TEST_DARK( u );
         if ( fu < fc )
         {
            float du = GOLD*(u - cx);
            bx = cx;
            cx = u;
            u += du;
            fb = fc;
            fc = fu;
            fu = TEST_DARK( u );
         }
      }
      else if ( (u - ulim)*(ulim - cx) >= 0 )
      {
         u = ulim;
         fu = TEST_DARK( u );
      }
      else
      {
         u = cx + GOLD*(cx - bx);
         fu = TEST_DARK( u );
      }

      ax = bx;
      bx = cx;
      cx = u;
      fa = fb;
      fb = fc;
      fc = fu;
   }

   monitor.Complete();
}

// ----------------------------------------------------------------------------

/*
 * Optimal dark scaling factor by multiscale noise evaluation and golden
 * section search minimization.
 *
 * Noise evaluation algorithms described in:
 *
 * J.L. Stark F. Murtagh, Astronomical Image and Data Analysis, pp. 37-38.
 *
 * J.L. Stark et al., Automatic Noise Estimation from the Multiresolution
 *    Support Publications of the Royal Astronomical Society of the Pacific,
 *    vol. 110, February 1998, pp. 193-199
 *
 * Function minimization routines based on:
 *
 * W.H. Press et al, Numerical Recipes in C, 2nd Edition
 * (section 10.1, pp. 397-402).
 */
static float DarkOptimization( const Image& target, const Image& dark, int c )
{
   /*
    * The golden ratios
    */
   const float R = 0.61803399;
   const float C = 1 - R;

   target.ResetSelections();
   target.SelectChannel( c );

   /*
    * Find an initial triplet (ax,bx,cx) that brackets the minimum.
    */
   float ax, bx, cx;
   BracketDarkOptimization( ax, bx, cx, target, dark );

   Console console;
   SpinStatus spin;
   StatusMonitor monitor;
   monitor.SetCallback( &spin );
   monitor.Initialize( "Optimizing" );

   /*
    * [x0,x3] is the total search interval.
    */
   float x0 = ax;
   float x3 = cx;

   /*
    * [x1,x2] is the inner search interval.
    * Start by sectioning the largest segment: either (ax,bx) or (bx,cx).
    */
   float x1, x2;
   if ( Abs( bx - ax ) < Abs( cx - bx ) )
   {
      x1 = bx;
      x2 = bx + C*(cx - bx);
   }
   else
   {
      x1 = bx - C*(bx - ax);
      x2 = bx;
   }

   /*
    * Start with noise estimates at the inner interval boundaries.
    */
   float f1 = TEST_DARK( x1 );
   float f2 = TEST_DARK( x2 );

   /*
    * Perform a golden section search for the minimum dark-induced noise.
    * We stop when we find a minimum to 1/1000 fractional accuracy.
    * Actually, trying to achieve very high accuracy is useless here, because
    * when we get close to the minimum, the function approximates a parabola so
    * we are trying to minimize something very similar to a horizontal straight
    * line. Note that this limitation is common to any minimization (or
    * maximization) routine.
    *
    * We perhaps could achieve 1/10000 at the cost of much more iterations, but
    * that would be a marginal improvement to dark optimization that doesn't
    * justify the increase in computation time.
    */
   while ( Abs( x3 - x0 ) > 0.0005F )
   {
      ++monitor;
      Module->ProcessEvents();
      if ( console.AbortRequested() )
         throw ProcessAborted();

      if ( f2 < f1 )
      {
         x0 = x1;
         x1 = x2;
         x2 = R*x2 + C*x3;
         f1 = f2;
         f2 = TEST_DARK( x2 );
      }
      else
      {
         x3 = x2;
         x2 = x1;
         x1 = R*x1 + C*x0;
         f2 = f1;
         f1 = TEST_DARK( x1 );
      }
   }

   monitor.Complete();

   /*
    * Return the dark scaling factor that minimizes noise in the target image.
    */
   return Max( .0F, (f1 < f2) ? x1 : x2 );
}

#undef TEST_DARK

static Rect OptimizingRect( const Image& target, int windowSize )
{
   return
   Rect( Min( windowSize, target.Width() ),
         Min( windowSize, target.Height() ) ).MovedTo( Max( 0, (target.Width() - windowSize) >> 1 ),
                                                       Max( 0, (target.Height() - windowSize) >> 1 ) );
}

static FVector OptimizeDark( const Image& target, const Image& optimizingDark, const IsoString& darkCFAPattern )
{
   FVector K( target.NumberOfChannels() );

   Image optimizingTarget( target );
   optimizingTarget.Status().DisableInitialization();

   if ( !darkCFAPattern.IsEmpty() )
      IntegerResample( -DownsamplingFactorForCFAPattern( darkCFAPattern ) ) >> optimizingTarget;

   if ( optimizingTarget.Bounds() != optimizingDark.Bounds() )
   {
      int windowSize = Max( optimizingDark.Width(), optimizingDark.Height() );
      optimizingTarget.SelectRectangle( OptimizingRect( optimizingTarget, windowSize ) );
      optimizingTarget.Crop();
      optimizingTarget.ResetSelections();
   }

   for ( int c = 0; c < optimizingTarget.NumberOfChannels(); ++c )
      K[c] = DarkOptimization( optimizingTarget, optimizingDark, c );

   return K;
}

// ----------------------------------------------------------------------------

/*
 * Estimation of the standard deviation of the noise, assuming a Gaussian
 * noise distribution.
 *
 * - Use MRS noise evaluation when the algorithm converges for 4 >= J >= 2
 *
 * - Use k-sigma noise evaluation when either MRS doesn't converge, or when the
 *   length of the set of noise pixels is less than a 1% of the image area.
 *
 * - Automatically iterate to find the highest wavelet layer where noise can be
 *   successfully evaluated, in the [1,3] range.
 */
static void EvaluateNoise( FVector& noiseEstimates, FVector& noiseFractions, StringList& noiseAlgorithms,
                           const Image& target,
                           int algorithm,
                           const IsoString& cfaPattern = IsoString() )
{
   MuteStatus status;
   target.SetStatusCallback( &status );
   target.Status().DisableInitialization();

   if ( !cfaPattern.IsEmpty() )
   {
      Image evaluationTarget( target );
      IntegerResample( -DownsamplingFactorForCFAPattern( cfaPattern ) ) >> evaluationTarget;
      EvaluateNoise( noiseEstimates, noiseFractions, noiseAlgorithms, evaluationTarget, algorithm );
      return;
   }

   noiseEstimates = FVector( 0.0F, target.NumberOfChannels() );
   noiseFractions = FVector( 0.0F, target.NumberOfChannels() );
   noiseAlgorithms = StringList( target.NumberOfChannels(), String() );

   ATrousWaveletTransform::WaveletScalingFunction H;
   if ( SeparableConvolution::FasterThanNonseparableFilterSize( Thread::NumberOfThreads( PCL_MAX_PROCESSORS ) ) > 5 )
      H.Set( KernelFilter( g_5x5B3Spline, 5 ) );
   else
      H.Set( SeparableFilter( g_5x5B3Spline_hv, g_5x5B3Spline_hv, 5 ) );

   for ( int c = 0; c < target.NumberOfChannels(); ++c )
   {
      target.SelectChannel( c );

      switch ( algorithm )
      {
      case ICNoiseEvaluationAlgorithm::KSigma:
         {
            ATrousWaveletTransform W( H, 1 );
            W << target;
            size_type N;
            noiseEstimates[c] = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/g_5x5B3Spline_kj[0];
            noiseFractions[c] = float( double( N )/target.NumberOfPixels() );
            noiseAlgorithms[c] = String( "K-Sigma" );
         }
         break;
      default:
      case ICNoiseEvaluationAlgorithm::MRS:
         {
            double s0 = 0;
            float f0 = 0;
            for ( int n = 4; ; )
            {
               Module->ProcessEvents();

               ATrousWaveletTransform W( H, n );
               W << target;

               size_type N;
               if ( n == 4 )
               {
                  s0 = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/g_5x5B3Spline_kj[0];
                  f0 = float( double( N )/target.NumberOfPixels() );
               }
               noiseEstimates[c] = W.NoiseMRS( ImageVariant( const_cast<Image*>( &target ) ), g_5x5B3Spline_kj, s0, 3, &N );
               noiseFractions[c] = float( double( N )/target.NumberOfPixels() );

               if ( noiseEstimates[c] > 0 && noiseFractions[c] >= 0.01F )
               {
                  noiseAlgorithms[c] = String( "MRS" );
                  break;
               }

               if ( --n == 1 )
               {
                  noiseEstimates[c] = s0;
                  noiseFractions[c] = f0;
                  noiseAlgorithms[c] = String( "K-Sigma" );
                  break;
               }
            }
         }
         break;
      }
   }

   target.ResetSelections();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// File Management Routines
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static Image* LoadImageFile( FileFormatInstance& file, int index = 0 )
{
   // Select the image at the specified index.
   if ( !file.SelectImage( index ) )
      throw CaughtException();

   // Create a shared image, 32-bit floating point.
   Image* image = new Image( (void*)0, 0, 0 );

   // Read the image.
   if ( !file.ReadImage( *image ) )
      throw CaughtException();

   return image;
}

static String UniqueFilePath( const String& filePath )
{
   for ( unsigned u = 1; ; ++u )
   {
      String tryFilePath = File::AppendToName( filePath, '_' + String( u ) );
      if ( !File::Exists( tryFilePath ) )
         return tryFilePath;
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Image Calibration Thread
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct CalibrationThreadData
{
   /*
    * The instance being executed.
    */
   ImageCalibrationInstance* instance;

   /*
    * Overscan regions grouped by target regions.
    */
   ImageCalibrationInstance::overscan_table overscan;

   /*
    * Master bias frame, overscan corrected.
    */
   Image*    bias;

   /*
    * Master dark frame, overscan+bias corrected.
    */
   Image*    dark;

   /*
    * Calibrated dark frame, central window for dark optimization, maybe
    * thresholded, maybe binned 2x2 (Bayer) or 3x3 (X-Trans).
    */
   Image*    optimizingDark;

   /*
    * If nonempty, the master dark frame is a CFA and we must apply it after
    * binning targets 2x2 (Bayer) or 3x3 (X-Trans) for optimization.
    */
   IsoString darkCFAPattern;

   /*
    * Master flat frame, overscan+bias+dark corrected and scaled.
    */
   Image*    flat;

   /*
    * Per-channel master flat scaling factors.
    */
   FVector   flatScalingFactors;
};

class CalibrationThread : public Thread
{
public:

   /*
    * Optimized dark scaling factors
    */
   FVector K;

   /*
    * Gaussian noise evaluation
    */
   FVector    noiseEstimates;
   FVector    noiseFractions;
   StringList noiseAlgorithms;

   CalibrationThread( Image* target, OutputFileData* outputData, const String& targetPath, int subimageIndex,
                      const CalibrationThreadData& data )
      : m_target( target )
      , m_outputData( outputData )
      , m_targetPath( targetPath )
      , m_subimageIndex( subimageIndex )
      , m_success( false )
      , m_data( data )
   {
   }

   void Run() override
   {
      Console console;
      String subimageText = (m_subimageIndex > 0) ? '(' + String( m_subimageIndex ) + ") " : String();
      console.WriteLn( "<end><cbr><br>* Processing target frame: " + subimageText + "<raw>" + m_targetPath + "</raw>" );

      try
      {
         Module->ProcessEvents();

         m_success = false;

         m_target->Status().DisableInitialization();

         /*
          * Overscan correction.
          */
         if ( m_data.instance->p_overscan.enabled )
         {
            console.WriteLn( "<end><cbr>* Applying overscan correction." );
            SubtractOverscan( *m_target, m_data.overscan, m_data.instance->p_overscan.imageRect );
         }

         /*
          * Dark frame optimization.
          */
         if ( m_data.dark != nullptr && m_data.instance->p_optimizeDarks && m_data.optimizingDark != nullptr )
         {
            console.WriteLn( "<end><cbr>* Computing dark frame optimization factors." );
            K = OptimizeDark( *m_target,
                              *m_data.optimizingDark,
                               m_data.darkCFAPattern );
         }
         else
            K = FVector( 1.0F, m_target->NumberOfChannels() );

         /*
          * Target frame calibration.
          */
         console.WriteLn( "<end><cbr>* Performing image calibration." );
         Calibrate( *m_target,
                     m_data.bias,
                     m_data.dark, K,
                     m_data.flat,
                     m_data.instance->p_outputPedestal/65535.0 );

         /*
          * Noise evaluation.
          *
          * ### TODO: Implement a different way to tell the noise evaluation
          *           routine that we have CFA images (not tied to dark frame
          *           optimization).
          */
         if ( m_data.instance->p_evaluateNoise )
         {
            console.WriteLn( "<end><cbr>* Performing noise evaluation." );
            EvaluateNoise( noiseEstimates, noiseFractions, noiseAlgorithms,
                          *m_target,
                           m_data.instance->p_noiseEvaluationAlgorithm,
                           m_data.darkCFAPattern );
         }

         m_success = true;
      }
      catch ( ... )
      {
         if ( IsRootThread() )
            throw;

         String text = ConsoleOutputText();
         ClearConsoleOutputText();
         try
         {
            throw;
         }
         ERROR_HANDLER
         m_errorInfo = ConsoleOutputText();
         ClearConsoleOutputText();
         console.Write( text );
      }
   }

   const CalibrationThreadData& CalibrationData() const
   {
      return m_data;
   }

   const Image* TargetImage() const
   {
      return m_target.Pointer();
   }

   String TargetFilePath() const
   {
      return m_targetPath;
   }

   const OutputFileData& OutputData() const
   {
      return *m_outputData;
   }

   int SubimageIndex() const
   {
      return m_subimageIndex;
   }

   bool Succeeded() const
   {
      return m_success;
   }

   String ErrorInfo() const
   {
      return m_errorInfo;
   }

private:

   AutoPointer<Image>           m_target;        // The image being calibrated. Owned by this thread.
   AutoPointer<OutputFileData>  m_outputData;    // Target image parameters and embedded data. Owned by this thread.
   String                       m_targetPath;    // File path of this target image.
   int                          m_subimageIndex; // >= 0 in case of a multiple image; = 0 otherwise.
   String                       m_errorInfo;     // Last error message.
   bool                         m_success;       // True if the thread completed execution successfully.
   const CalibrationThreadData& m_data;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Routine to enforce validity of image geometries.
 */
void ImageCalibrationInstance::ValidateImageGeometry( const Image* image, bool uncalibrated )
{
   if ( uncalibrated )
   {
      if ( m_geometry.IsRect() )
      {
         if ( image->Bounds() != m_geometry )
            throw Error( "Incompatible image geometry" );
      }
      else
      {
         m_geometry = image->Bounds();

         if ( p_overscan.enabled )
         {
            if ( !m_geometry.Includes( p_overscan.imageRect ) )
               throw Error( "Inconsistent overscan geometry" );

            for ( int i = 0; i < 4; ++i )
               if ( p_overscan.overscan[i].enabled )
                  if ( !m_geometry.Includes( p_overscan.overscan[i].sourceRect ) ||
                       !m_geometry.Includes( p_overscan.overscan[i].targetRect ) )
                     throw Error( "Inconsistent overscan geometry" );
         }
         else
         {
            if ( m_calibratedGeometry.IsRect() )
            {
               if ( m_geometry != m_calibratedGeometry )
                  throw Error( "Incompatible image geometry" );
            }
            else
               m_calibratedGeometry = m_geometry;
         }
      }
   }
   else
   {
      if ( m_calibratedGeometry.IsRect() )
      {
         if ( image->Bounds() != m_calibratedGeometry )
            throw Error( "Incompatible image geometry" );
      }
      else
      {
         m_calibratedGeometry = image->Bounds();

         if ( p_overscan.enabled )
         {
            if ( m_calibratedGeometry.Width() != p_overscan.imageRect.Width() ||
                 m_calibratedGeometry.Height() != p_overscan.imageRect.Height() )
               throw Error( "Inconsistent overscan geometry" );
         }
         else
         {
            if ( m_geometry.IsRect() )
            {
               if ( m_calibratedGeometry != m_geometry )
                  throw Error( "Incompatible image geometry" );
            }
            else
               m_geometry = m_calibratedGeometry;
         }
      }
   }
}

// ----------------------------------------------------------------------------

/*
 * Routine to group overscan source regions with the same target regions.
 */
ImageCalibrationInstance::overscan_table
ImageCalibrationInstance::BuildOverscanTable() const
{
   overscan_table O;

   OverscanRegions r[ 4 ];
   for ( int i = 0; i < 4; ++i )
      r[i] = p_overscan.overscan[i];

   for ( int i = 0; i < 4; ++i )
      if ( r[i].enabled )
      {
         overscan_list o;
         o.Add( r[i] );
         for ( int j = i; ++j < 4; )
            if ( r[j].enabled && r[j].targetRect == r[i].targetRect )
            {
               o.Add( r[j] );
               r[j].enabled = false;
            }
         O.Add( o );
      }

   return O;
}

// ----------------------------------------------------------------------------

/*
 * Subtraction of an input pedestal to get zero-based pixel values.
 */
void ImageCalibrationInstance::SubtractPedestal( Image* image, FileFormatInstance& file )
{
   Console console;
   switch ( p_pedestalMode )
   {
   case ICPedestalMode::Literal:
      if ( p_pedestal != 0 )
      {
         console.NoteLn( String().Format( "* Subtracting pedestal: %d DN", p_pedestal ) );
         image->Apply( p_pedestal/65535.0, ImageOp::Sub );
      }
      break;
   case ICPedestalMode::Keyword:
   case ICPedestalMode::CustomKeyword:
      if ( file.Format().CanStoreKeywords() )
      {
         IsoString keyName( (p_pedestalMode == ICPedestalMode::Keyword) ? "PEDESTAL" : p_pedestalKeyword );
         FITSKeywordArray keywords;
         if ( !file.ReadFITSKeywords( keywords ) )
            break;
         double d = 0;
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
               if ( keyword.IsNumeric() )
                  if ( keyword.GetNumericValue( d ) ) // GetNumericValue() sets d=0 if keyword cannot be converted
                     break;
         if ( d != 0 )
         {
            /*
             * Silently be compatible with acquisition applications that write
             * negative PEDESTAL keyword values.
             */
            if ( p_pedestalMode == ICPedestalMode::Keyword )
               if ( d < 0 )
                  d = -d;

            console.NoteLn( String().Format( "* Subtracting pedestal keyword '%s': %.4f DN", keyName.c_str(), d ) );
            image->Apply( d/65535.0, ImageOp::Sub );
         }
      }
      break;
   }
}

// ----------------------------------------------------------------------------

/*
 * Read a source calibration frame.
 */
Image* ImageCalibrationInstance::LoadCalibrationFrame( const String& filePath, bool willCalibrate, IsoString* cfaPattern )
{
   Console console;

   /*
    * Find out an installed file format that can read image files with the
    * specified extension ...
    */
   FileFormat format( File::ExtractExtension( filePath ), true/*read*/, false/*write*/ );

   /*
    * ... and create a format instance (usually a disk file) to access this
    * master calibration frame image.
    */
   FileFormatInstance file( format );

   /*
    * Open input stream.
    */
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath, p_inputHints ) )
      throw CaughtException();

   /*
    * Check for an empty calibration frame.
    */
   if ( images.IsEmpty() )
      throw Error( filePath + ": Empty master calibration frame image." );

   /*
    * Master calibration files can store multiple images - only the first image
    * will be used in such case, and the rest will be ignored. For example, a
    * master file can store integration rejection maps along with a master
    * bias, dark or flat frame.
    */
   if ( images.Length() > 1 )
      console.WriteLn( String().Format( "<end><cbr>* Ignoring %u additional image(s) in master calibration frame.", images.Length()-1 ) );

   /*
    * Optional CFA pattern retrieval.
    */
   if ( cfaPattern != nullptr )
      if ( images[0].info.colorSpace == ColorSpace::Gray ) // CFA frames are monochrome
      {
         *cfaPattern = CFAPatternFromTarget( file );
         if ( !cfaPattern->IsEmpty() )
         {
            console.Write( "<end><cbr>CFA pattern" );
            if ( cfaPattern->Length() != 4/*X-Trans*/ || p_cfaPattern == ICCFAPattern::Auto )
               console.Write( " (detected)" );
            console.WriteLn( ": " + *cfaPattern );
         }
      }
      else
         cfaPattern->Clear();

   /*
    * Load the master calibration frame.
    */
   Image* image = LoadImageFile( file );

   /*
    * Enforce consistency of frame dimensions.
    */
   ValidateImageGeometry( image, willCalibrate );

   /*
    * Optional pedestal subtraction.
    */
   SubtractPedestal( image, file );

   /*
    * Close the input stream.
    */
   if ( !file.Close() )
      throw CaughtException();

   return image;
}

// ----------------------------------------------------------------------------

/*
 * Read a target frame file. Returns a list of calibration threads ready to
 * calibrate all subimages loaded from the file.
 */
thread_list
ImageCalibrationInstance::LoadTargetFrame( const String& filePath, const CalibrationThreadData& threadData )
{
   Console console;
   console.WriteLn( "<end><cbr><br>* Loading target calibration frame(s): <raw>" + filePath + "</raw>" );

   /*
    * Find out an installed file format that can read image files with the
    * specified extension ...
    */
   FileFormat format( File::ExtractExtension( filePath ), true/*read*/, false/*write*/ );

   /*
    * ... and create a format instance (usually a disk file) to access this
    * target image.
    */
   FileFormatInstance file( format );

   /*
    * Open the image file.
    */
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath, p_inputHints ) )
      throw CaughtException();

   if ( images.IsEmpty() )
      throw Error( filePath + ": Empty image file." );

   /*
    * Multiple-image file formats are supported and implemented in PixInsight
    * (e.g.: XISF, FITS), so when we open a file, what we get is an array of
    * images, usually consisting of a single image, but we must provide for a
    * set of subimages.
    */
   thread_list threads;
   try
   {
      for ( size_type j = 0; j < images.Length(); ++j )
      {
         if ( images.Length() > 1 )
            console.WriteLn( String().Format( "* Subimage %u of %u", j+1, images.Length() ) );

         AutoPointer<Image> target( LoadImageFile( file, j ) );

         Module->ProcessEvents();

         /*
          * NB: At this point, LoadImageFile() has already called
          * file.SelectImage().
          */

         /*
          * Enforce consistency of image dimensions.
          */
         ValidateImageGeometry( target );

         /*
          * Optional pedestal subtraction.
          */
         SubtractPedestal( target, file );

         /*
          * Retrieve metadata and auxiliary data structures that must be
          * preserved in the output image (properties, header keywords, etc).
          */
         OutputFileData* outputData = new OutputFileData( file, images[j].options );

         /*
          * Create a new calibration thread and add it to the thread list.
          */
         threads.Add( new CalibrationThread( target,
                                             outputData,
                                             filePath,
                                             (images.Length() > 1) ? j+1 : 0,
                                             threadData ) );
         // The thread owns the target image.
         target.Release();
      }

      /*
       * Close the input stream.
       */
      if ( !file.Close() )
         throw CaughtException();

      return threads;
   }
   catch ( ... )
   {
      threads.Destroy();
      throw;
   }
}

// ----------------------------------------------------------------------------

void ImageCalibrationInstance::WriteCalibratedImage( const CalibrationThread* t )
{
   Console console;

   /*
    * Output directory.
    */
   String fileDir = p_outputDirectory.Trimmed();
   if ( fileDir.IsEmpty() )
      fileDir = File::ExtractDrive( t->TargetFilePath() ) + File::ExtractDirectory( t->TargetFilePath() );
   if ( fileDir.IsEmpty() )
      throw Error( t->TargetFilePath() + ": Unable to determine an output directory." );
   if ( !fileDir.EndsWith( '/' ) )
      fileDir.Append( '/' );

   /*
    * Output file extension, which defines the output file format.
    */
   String fileExtension = p_outputExtension.Trimmed();
   if ( fileExtension.IsEmpty() )
      fileExtension = File::ExtractExtension( t->TargetFilePath() );
   if ( fileExtension.IsEmpty() )
      throw Error( t->TargetFilePath() + ": Unable to determine an output file extension." );
   if ( !fileExtension.StartsWith( '.' ) )
      fileExtension.Prepend( '.' );

   /*
    * Output file name.
    */
   String fileName = File::ExtractName( t->TargetFilePath() ).Trimmed();
   if ( t->SubimageIndex() > 0 )
      fileName.Append( String().Format( "_%02d", t->SubimageIndex() ) );
   if ( !p_outputPrefix.IsEmpty() )
      fileName.Prepend( p_outputPrefix );
   if ( !p_outputPostfix.IsEmpty() )
      fileName.Append( p_outputPostfix );
   if ( fileName.IsEmpty() )
      throw Error( t->TargetFilePath() + ": Unable to determine an output file name." );

   /*
    * Output file path.
    */
   String outputFilePath = fileDir + fileName + fileExtension;

   console.WriteLn( "<end><cbr><br>* Writing output file: <raw>" + outputFilePath + "</raw>" );

   /*
    * Write dark optimization factors to the console.
    */
   if ( p_optimizeDarks )
   {
      console.WriteLn( "Dark scaling factors:" );
      for ( int i = 0; i < t->K.Length(); ++i )
      {
         console.WriteLn( String().Format( "k%d = %.3f", i, t->K[i] ) );
         if ( t->K[i] <= NO_DARK_CORRELATION )
            console.WarningLn( String().Format( "** Warning: No correlation "
                               "between the master dark and target frames (channel %d).", i ) );
      }
   }

   /*
    * Write noise evaluation information to the console.
    */
   if ( p_evaluateNoise )
   {
      console.WriteLn( "Gaussian noise estimates:" );
      for ( int i = 0; i < t->noiseEstimates.Length(); ++i )
         console.WriteLn( String().Format( "s%d = %.3e, n%d = %.3f ",
                              i, t->noiseEstimates[i], i, t->noiseFractions[i] ) + '(' + t->noiseAlgorithms[i] + ')' );
   }

   /*
    * Check for an already existing file, and either overwrite it (but show a
    * warning message if that happens), or find a unique file name, depending
    * on the overwriteExistingFiles parameter.
    */
   if ( File::Exists( outputFilePath ) )
      if ( p_overwriteExistingFiles )
         console.WarningLn( "** Warning: Overwriting already existing file." );
      else
      {
         outputFilePath = UniqueFilePath( outputFilePath );
         console.NoteLn( "* File already exists, writing to: " + outputFilePath );
      }

   /*
    * Find an installed file format able to write files with the
    * specified extension ...
    */
   FileFormat outputFormat( fileExtension, false/*read*/, true/*write*/ );

   if ( outputFormat.IsDeprecated() )
      console.WarningLn( "** Warning: Deprecated file format: " + outputFormat.Name() );

   FileFormatInstance outputFile( outputFormat );

   /*
    * ... and create a format instance (usually a disk file).
    */
   if ( !outputFile.Create( outputFilePath, p_outputHints ) )
      throw CaughtException();

   /*
    * Gather relevant image options, including the output sample format (bits
    * per pixel sample and numeric type).
    */
   const OutputFileData& data = t->OutputData();

   ImageOptions options = data.options; // get resolution, sample format, etc.

   /*
    * Determine the output sample format: bits per sample and whether integer
    * or real samples.
    */
   switch ( p_outputSampleFormat )
   {
   case ICOutputSampleFormat::I16: options.bitsPerSample = 16; options.ieeefpSampleFormat = false; break;
   case ICOutputSampleFormat::I32: options.bitsPerSample = 32; options.ieeefpSampleFormat = false; break;
   default:
   case ICOutputSampleFormat::F32: options.bitsPerSample = 32; options.ieeefpSampleFormat = true;  break;
   case ICOutputSampleFormat::F64: options.bitsPerSample = 64; options.ieeefpSampleFormat = true;  break;
   }

   outputFile.SetOptions( options );

   /*
    * We want to be sure that the selected (or automatically found) output
    * format is able to store an image with the selected (or automatically
    * determined) sample format. If it doesn't, then the user should know what
    * he/she is doing, but we always inform anyway.
    */
   bool canStore = true;
   if ( options.ieeefpSampleFormat )
      switch ( options.bitsPerSample )
      {
      case 32: canStore = outputFormat.CanStoreFloat(); break;
      case 64: canStore = outputFormat.CanStoreDouble(); break;
      }
   else
      switch ( options.bitsPerSample )
      {
      case 16: canStore = outputFormat.CanStore16Bit(); break;
      case 32: canStore = outputFormat.CanStore32Bit(); break;
      case 64: canStore = outputFormat.CanStore64Bit(); break;
      }

   if ( !canStore )
      console.WarningLn( "** Warning: The output format does not support the required sample data format." );

   /*
    * File formats often use format-specific data.
    * Keep track of private data structures.
    */
   if ( data.fsData != nullptr )
      if ( outputFormat.UsesFormatSpecificData() )
         if ( outputFormat.ValidateFormatSpecificData( data.fsData ) )
            outputFile.SetFormatSpecificData( data.fsData );

   /*
    * Set image properties.
    */
   if ( !data.properties.IsEmpty() )
      if ( outputFormat.CanStoreImageProperties() && outputFormat.SupportsViewProperties() )
         outputFile.WriteImageProperties( data.properties );
      else
         console.WarningLn( "** Warning: The output format cannot store image properties - existing properties not embedded." );

   /*
    * Add FITS header keywords and preserve existing ones, if possible.
    * N.B.: A COMMENT or HISTORY keyword cannot have a value; these keywords
    * only have the name and comment components.
    */
   if ( outputFormat.CanStoreKeywords() )
   {
      FITSKeywordArray keywords = data.keywords;

      /*
       * Remove previously existing PEDESTAL keywords, since we already have
       * subtracted the appropriate pedestal value before calibration.
       */
      for ( size_type i = 0; i < keywords.Length(); )
         if ( !keywords[i].name.CompareIC( "PEDESTAL" ) )
            keywords.Remove( keywords.At( i ) );
         else
            ++i;

      keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Calibration with " + PixInsightVersion::AsString() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Calibration with " + Module->ReadableVersion() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Calibration with ImageCalibration process" );

      if ( !p_inputHints.IsEmpty() )
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.inputHints: " + IsoString( p_inputHints ) );
      if ( !p_outputHints.IsEmpty() )
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.outputHints: " + IsoString( p_outputHints ) );

      keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.overscan.enabled: " + IsoString( bool( p_overscan.enabled ) ) );
      if ( p_overscan.enabled )
      {
         const Rect& r = p_overscan.imageRect;
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.overscan.imageRect: {%d,%d,%d,%d}",
                                                        r.x0, r.y0, r.x1, r.y1 ) );
         for ( int i = 0; i < 4; ++i )
            if ( p_overscan.overscan[i].enabled )
            {
               const Rect& s = p_overscan.overscan[i].sourceRect;
               const Rect& t = p_overscan.overscan[i].targetRect;
               keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.overscan[%d].sourceRect: {%d,%d,%d,%d}",
                                                        i, s.x0, s.y0, s.x1, s.y1 ) )
                        << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.overscan[%d].targetRect: {%d,%d,%d,%d}",
                                                        i, t.x0, t.y0, t.x1, t.y1 ) );
            }
      }

      keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterBias.enabled: " + IsoString( bool( p_masterBias.enabled ) ) );
      if ( p_masterBias.enabled )
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterBias.fileName: " +
                                    File::ExtractNameAndExtension( p_masterBias.path ).To7BitASCII() )
                  << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterBias.calibrate: " + IsoString( bool( p_calibrateBias ) ) );

      keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterDark.enabled: " + IsoString( bool( p_masterDark.enabled ) ) );
      if ( p_masterDark.enabled )
      {
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterDark.fileName: " +
                                    File::ExtractNameAndExtension( p_masterDark.path ).To7BitASCII() )
                  << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterDark.calibrate: " + IsoString( bool( p_calibrateDark ) ) );
         if ( p_optimizeDarks )
         {
            IsoString darkScalingFactors = IsoString().Format( "ImageCalibration.masterDark.scalingFactors: %.3f", t->K[0] );
            for ( int i = 1; i < t->K.Length(); ++i )
               darkScalingFactors.AppendFormat( " %.3f", t->K[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), darkScalingFactors );

            if ( p_darkOptimizationThreshold > 0 )
               keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.masterDark.optimizationThreshold: %.5f", p_darkOptimizationThreshold ) )
                        << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.masterDark.optimizationLow: %.4f", p_darkOptimizationLow ) );

            if ( p_darkOptimizationWindow > 0 )
               keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.masterDark.optimizationWindow: %d px", p_darkOptimizationWindow ) );
         }
      }

      keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterFlat.enabled: " + IsoString( bool( p_masterFlat.enabled ) ) );
      if ( p_masterFlat.enabled )
      {
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterFlat.fileName: " +
                                    File::ExtractNameAndExtension( p_masterFlat.path ).To7BitASCII() )
                  << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterFlat.calibrate: " + IsoString( bool( p_calibrateFlat ) ) );

         IsoString flatScalingFactors = IsoString().Format( "ImageCalibration.masterFlat.scalingFactors: %.6f", t->CalibrationData().flatScalingFactors[0] );
         for ( int i = 1; i < t->CalibrationData().flatScalingFactors.Length(); ++i )
            flatScalingFactors.AppendFormat( " %.6f", t->CalibrationData().flatScalingFactors[i] );
         keywords << FITSHeaderKeyword( "HISTORY", IsoString(), flatScalingFactors );
      }

      if ( p_evaluateNoise )
      {
         /*
          * N.B.: If we have computed noise estimates, remove any previously
          * existing NOISExxx keywords. Only our new NOISExxx keywords must be
          * present in the header.
          */
         for ( size_type i = 0; i < keywords.Length(); )
            if ( keywords[i].name.StartsWithIC( "NOISE" ) )
               keywords.Remove( keywords.At( i ) );
            else
               ++i;

         keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "Noise evaluation with " + Module->ReadableVersion() );

         IsoString noiseEstimates = IsoString().Format( "ImageCalibration.noiseEstimates: %.3e", t->noiseEstimates[0] );
         for ( int i = 1; i < t->noiseEstimates.Length(); ++i )
            noiseEstimates.AppendFormat( " %.3e", t->noiseEstimates[i] );
         keywords << FITSHeaderKeyword( "HISTORY", IsoString(), noiseEstimates );

         for ( int i = 0; i < t->noiseEstimates.Length(); ++i )
            keywords << FITSHeaderKeyword( IsoString().Format( "NOISE%02d", i ),
                                           IsoString().Format( "%.3e", t->noiseEstimates[i] ),
                                           IsoString().Format( "Gaussian noise estimate, channel #%d", i ) )
                     << FITSHeaderKeyword( IsoString().Format( "NOISEF%02d", i ),
                                           IsoString().Format( "%.3f", t->noiseFractions[i] ),
                                           IsoString().Format( "Fraction of noise pixels, channel #%d", i ) )
                     << FITSHeaderKeyword( IsoString().Format( "NOISEA%02d", i ),
                                           IsoString( t->noiseAlgorithms[i] ),
                                           IsoString().Format( "Noise evaluation algorithm, channel #%d", i ) );
      }

      if ( p_outputPedestal != 0 )
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.outputPedestal: %d", p_outputPedestal ) )
                  << FITSHeaderKeyword( "PEDESTAL",
                                    IsoString( p_outputPedestal ),
                                    "Value in DN added to enforce positivity" );

      outputFile.WriteFITSKeywords( keywords );
   }
   else
   {
      console.WarningLn( "** Warning: The output format cannot store FITS header keywords - calibration metadata not embedded." );
   }

   /*
    * Preserve ICC profile, if possible.
    */
   if ( data.profile.IsProfile() )
      if ( outputFormat.CanStoreICCProfiles() )
         outputFile.WriteICCProfile( data.profile );
      else
         console.WarningLn( "** Warning: The output format cannot store color profiles - original ICC profile not embedded." );

   /*
    * Write the output image and close the output stream.
    */
   t->TargetImage()->ResetSelections();
   if ( !outputFile.WriteImage( *t->TargetImage() ) || !outputFile.Close() )
      throw CaughtException();

   /*
    * Store output data
    */
   OutputData o;
   o.outputFilePath = outputFilePath;
   if ( p_masterDark.enabled )
      if ( p_optimizeDarks )
         for ( int i = 0; i < Min( 3, t->K.Length() ); ++i )
            o.darkScalingFactors[i] = t->K[i];
   if ( p_evaluateNoise )
      for ( int i = 0; i < Min( 3, t->noiseEstimates.Length() ); ++i )
      {
         o.noiseEstimates[i] = t->noiseEstimates[i];
         o.noiseFractions[i] = t->noiseFractions[i];
         o.noiseAlgorithms[i] = String( t->noiseAlgorithms[i] );
      }
   o_output.Add( o );
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInstance::ExecuteGlobal()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanExecuteGlobal( why ) )
         throw Error( why );

      if ( !p_outputDirectory.IsEmpty() )
         if ( !File::DirectoryExists( p_outputDirectory ) )
            throw Error( "The specified output directory does not exist: " + p_outputDirectory );

      for ( auto frame : p_targetFrames )
         if ( frame.enabled )
            if ( !File::Exists( frame.path ) )
               throw Error( "No such file exists on the local filesystem: " + frame.path );
   }

   /*
    * Allow the user to abort the calibration process.
    */
   Console console;
   console.EnableAbort();

   /*
    * Reset output data.
    */
   o_output.Clear();

   /*
    * Initialize validation frame geometries.
    */
   m_geometry = m_calibratedGeometry = 0;

   /*
    * Overscan regions grouped by target regions.
    */
   overscan_table O;
   if ( p_overscan.enabled )
      O = BuildOverscanTable();

   /*
    * Master calibration frames in use.
    */
   AutoPointer<Image> bias, dark, optimizingDark, flat;

   /*
    * Per-channel master flat scaling factors.
    */
   FVector flatScalingFactors;

   /*
    * CFA patterns for the master dark and flat frames.
    */
   IsoString darkCFAPattern, flatCFAPattern;

   /*
    * Load and calibrate, as appropriate, all master calibration frames.
    */
   if ( p_masterBias.enabled || p_masterDark.enabled || p_masterFlat.enabled )
   {
      /*
       * Load master bias, dark and flat frames, and validate their geometries.
       */
      if ( p_masterBias.enabled )
      {
         console.NoteLn( "<end><cbr><br>* Loading master bias frame: <raw>" + p_masterBias.path + "</raw>" );
         bias = LoadCalibrationFrame( p_masterBias.path, p_calibrateBias );
      }

      if ( p_masterDark.enabled )
      {
         console.NoteLn( "<end><cbr><br>* Loading master dark frame: <raw>" + p_masterDark.path + "</raw>" );
         dark = LoadCalibrationFrame( p_masterDark.path, p_calibrateDark, p_enableCFA ? &darkCFAPattern : nullptr );
      }

      if ( p_masterFlat.enabled )
      {
         console.NoteLn( "<end><cbr><br>* Loading master flat frame: <raw>" + p_masterFlat.path + "</raw>" );
         flat = LoadCalibrationFrame( p_masterFlat.path, p_calibrateFlat, p_enableCFA ? &flatCFAPattern : nullptr );
         if ( p_enableCFA )
            if ( p_masterDark.enabled )
               if ( darkCFAPattern != flatCFAPattern )
                  throw Error( "Mismatched CFA patterns between the master dark and flat frames: '" + darkCFAPattern + "', '" + flatCFAPattern + "'" );
      }

      Module->ProcessEvents();

      /*
       * Apply overscan corrections.
       */
      if ( p_overscan.enabled )
      {
         if ( p_calibrateBias )
            if ( p_masterBias.enabled )
            {
               console.NoteLn( "<end><cbr><br>* Applying overscan correction: master bias frame." );
               SubtractOverscan( *bias, O, p_overscan.imageRect );
            }
         if ( p_calibrateDark )
            if ( p_masterDark.enabled )
            {
               console.NoteLn( "<end><cbr><br>* Applying overscan correction: master dark frame." );
               SubtractOverscan( *dark, O, p_overscan.imageRect );
            }
         if ( p_calibrateFlat )
            if ( p_masterFlat.enabled )
            {
               console.NoteLn( "<end><cbr><br>* Applying overscan correction: master flat frame." );
               SubtractOverscan( *flat, O, p_overscan.imageRect );
            }
      }

      Module->ProcessEvents();

      /*
       * Calibrate the master dark frame by subtracting the master bias frame.
       */
      if ( p_masterDark.enabled )
      {
         if ( p_calibrateDark )
            if ( p_masterBias.enabled )
            {
               console.NoteLn( "<end><cbr><br>* Applying bias correction: master dark frame." );
               SubtractBias( *dark, *bias );
            }

         /*
          * If necessary, create the dark optimization window image.
          * NB: This must be done after calibrating the master dark frame.
          */
         if ( p_optimizeDarks )
         {
            dark->ResetSelections();
            optimizingDark = new Image( *dark );
            if ( !darkCFAPattern.IsEmpty() )
               IntegerResample( -DownsamplingFactorForCFAPattern( darkCFAPattern ) ) >> *optimizingDark;

            console.NoteLn( "<end><cbr><br>* Computing dark frame optimization thresholds." );
            for ( int c = 0; c < optimizingDark->NumberOfChannels(); ++c )
            {
               optimizingDark->SelectChannel( c );
               double location = optimizingDark->Median();
               double scale = 1.4826*optimizingDark->MAD( location );
               double t;
               if ( p_darkOptimizationThreshold > 0 ) // ### be backwards-compatible
               {
                  t = Range( p_darkOptimizationThreshold, 0.0F, 1.0F );
                  p_darkOptimizationLow = (t - location)/scale;
                  p_darkOptimizationThreshold = 0;
               }
               else
               {
                  t = Range( location + p_darkOptimizationLow*scale, 0.0, 1.0 );
               }
               size_type N = optimizingDark->NumberOfPixels();
               for ( Image::sample_iterator i( *optimizingDark, c ); i; ++i )
                  if ( *i < t )
                  {
                     *i = 0;
                     --N;
                  }
               console.WriteLn( String().Format( "<end><cbr>Td%d = %.8f (%llu px = %.3f%%)",
                                                   c, t, N, 100.0*N/optimizingDark->NumberOfPixels() ) );
               if ( N < DARK_COUNT_LOW*optimizingDark->NumberOfPixels() )
                  console.WarningLn( "** Warning: The dark frame optimization threshold is probably too high (channel " + String( c ) + "), "
                                     "or the master dark frame is not valid or is not being calibrated correctly." );
               if ( N < DARK_COUNT_SMALL )
               {
                  console.WarningLn( "** Warning: The selected pixel set for dark frame optimization is too small "
                                     "- disabling dark frame optimization for channel " + String( c ) + "." );
                  optimizingDark.Destroy();
                  break;
               }
            }

            if ( optimizingDark )
            {
               optimizingDark->ResetSelections();

               if ( p_darkOptimizationWindow > 0 )
                  if ( p_darkOptimizationWindow < dark->Width() || p_darkOptimizationWindow < dark->Height() )
                  {
                     optimizingDark->SelectRectangle( OptimizingRect( *optimizingDark, p_darkOptimizationWindow ) );
                     optimizingDark->Crop();
                     optimizingDark->ResetSelections();
                  }
            }
         }
      }

      Module->ProcessEvents();

      /*
       * Calibrate and scale the master flat frame.
       */
      if ( p_masterFlat.enabled )
      {
         /*
          * Calibrate the master flat frame by subtracting the bias and dark
          * master frames. If selected, perform a dark frame optimization for
          * the master flat.
          */
         if ( p_calibrateFlat )
            if ( p_masterBias.enabled || p_masterDark.enabled )
            {
               console.NoteLn( "<end><cbr><br>* Calibrating master flat frame." );

               FVector K;
               if ( p_masterDark.enabled )
                  if ( p_optimizeDarks && optimizingDark )
                  {
                     console.NoteLn( "<end><cbr><br>* Computing dark frame optimization factors: master flat frame." );
                     K = OptimizeDark( *flat, *optimizingDark, darkCFAPattern );
                     for ( int c = 0; c < flat->NumberOfChannels(); ++c )
                     {
                        console.WriteLn( String().Format( "<end><cbr>k%d = %.3f", c, K[c] ) );
                        if ( K[c] <= NO_DARK_CORRELATION )
                           console.WarningLn( String().Format( "** Warning: No correlation between "
                                             "the master dark and master flat frames (channel %d).", c ) );
                     }
                  }
                  else
                     K = FVector( 1.0F, flat->NumberOfChannels() );

               Module->ProcessEvents();
               Calibrate( *flat, bias, dark, K, 0, FVector() );
            }

         Module->ProcessEvents();

         /*
          * - Compute master flat scaling factors as per-channel robust mean
          *   pixel values.
          * - Apply scaling factors to the master flat by pixelwise division.
          */
         console.NoteLn( "<end><cbr><br>* Computing master flat scaling factors." );

         if ( flatCFAPattern.IsEmpty() || !p_separateCFAFlatScalingFactors )
         {
            flatScalingFactors = FVector( flat->NumberOfChannels() );
            for ( int c = 0; c < flat->NumberOfChannels(); ++c )
            {
               int t = Max( 1, TruncInt( p_flatScaleClippingFactor * flat->NumberOfPixels() ) );
               double s = TrimmedMean( flat->PixelData( c ), flat->PixelData( c )+flat->NumberOfPixels(), t, t );
               if ( 1 + s == 1 )
                  throw Error( String().Format( "Zero or insignificant scaling factor for flat frame channel %d. "
                                                "(empty or marginal master flat frame data?)", c ) );
               flat->Apply( s, ImageOp::Div, Rect( 0 ), c/*firstChannel*/, c/*lastChannel*/ );
               console.WriteLn( String().Format( "s%d = %.6f", c, s ) );
               flatScalingFactors[c] = s;
            }
         }
         else
         {
            CFAIndex cfa( flatCFAPattern );
            Array<float> F[ 3 ];
            {
               Image::const_sample_iterator i( *flat );
               for ( int y = 0; y < flat->Height(); ++y )
                  for ( int x = 0; x < flat->Width(); ++x, ++i )
                     F[cfa( x, y )] << *i;
            }

            flatScalingFactors = FVector( 3 );
            for ( int c = 0; c < 3; ++c )
            {
               int t = Max( 1, TruncInt( p_flatScaleClippingFactor * F[c].Length() ) );
               double s = TrimmedMean( F[c].Begin(), F[c].End(), t, t );
               if ( 1 + s == 1 )
                  throw Error( String().Format( "Zero or insignificant scaling factor for flat frame CFA component %d. "
                                                "(empty or marginal master flat frame data?)", c ) );
               Image::sample_iterator i( *flat );
               for ( int y = 0; y < flat->Height(); ++y )
                  for ( int x = 0; x < flat->Width(); ++x, ++i )
                     if ( cfa( x, y ) == c )
                        *i /= s;
               console.WriteLn( String().Format( "s%d = %.6f", c, s ) );
               flatScalingFactors[c] = s;
            }
         }
      }
   } // if bias or dark or flat

   Module->ProcessEvents();

   /*
    * Begin light frame calibration process.
    */
   console.NoteLn( String().Format( "<end><cbr><br>* Calibration of %u target frames.", p_targetFrames.Length() ) );

   /*
    * Prepare working thread data.
    */
   CalibrationThreadData threadData;
   threadData.instance = this;
   threadData.overscan = O;
   threadData.bias = bias;
   threadData.dark = dark;
   threadData.optimizingDark = optimizingDark;
   threadData.darkCFAPattern = darkCFAPattern;
   threadData.flat = flat;
   threadData.flatScalingFactors = flatScalingFactors;

   /*
    * Running threads list. Note that IndirectArray<> initializes all of its
    * contained pointers to nullptr.
    */
   int numberOfThreads = Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 );
   thread_list runningThreads( Min( int( p_targetFrames.Length() ), numberOfThreads ) );
   console.WriteLn( String().Format( "* Using %u worker threads.", runningThreads.Length() ) );

   /*
    * Waiting threads list. We use this list for temporary storage of
    * calibration threads for multiple image files.
    */
   thread_list waitingThreads;

   /*
    * Flag to signal the beginning of the final waiting period, when there are
    * no more pending images but there are still running threads.
    */
   bool waitingForFinished = false;

   /*
    * We'll work on a temporary duplicate of the target frames list. This
    * allows us to modify the working list without altering the instance.
    */
   image_list targets( p_targetFrames );

   int succeeded = 0;
   int failed = 0;
   int skipped = 0;

   try
   {
      for ( ;; )
      {
         try
         {
            /*
             * Thread watching loop.
             */
            thread_list::iterator i = nullptr;
            size_type unused = 0;
            for ( thread_list::iterator j = runningThreads.Begin(); j != runningThreads.End(); ++j )
            {
               // Keep the GUI responsive
               Module->ProcessEvents();
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               if ( *j == nullptr )
               {
                  /*
                   * This is a free thread slot. Ignore it if we don't have
                   * more pending images to feed in.
                   */
                  if ( targets.IsEmpty() )
                     if ( waitingThreads.IsEmpty() )
                     {
                        ++unused;
                        continue;
                     }
               }
               else
               {
                  /*
                   * This is an existing thread. If this thread is still alive,
                   * wait for a while and then continue watching.
                   */
                  if ( (*j)->IsActive() )
                  {
                     pcl::Sleep( 150 );
                     continue;
                  }
               }

               /*
                * If we have a useful free thread slot, or if a thread has just
                * finished, exit the watching loop and work it out.
                */
               i = j;
               break;
            }

            /*
             * Keep watching while there are no useful free thread slots or a
             * finished thread.
             */
            if ( i == nullptr )
               if ( unused == runningThreads.Length() )
                  break;
               else
                  continue;

            /*
             * At this point we have found either a unused thread slot that we
             * can reuse, or a thread that has just finished execution.
             */
            if ( *i != nullptr )
            {
               /*
                * This is a just-finished thread.
                */
               (*i)->FlushConsoleOutputText();
               String errorInfo;
               if ( (*i)->Succeeded() )
                  WriteCalibratedImage( *i );
               else
                  errorInfo = (*i)->ErrorInfo();

               /*
                * N.B.: IndirectArray<>::Delete() sets to zero the pointer
                * pointed to by the iterator, but does not remove the array
                * element.
                */
               runningThreads.Delete( i );

               if ( !errorInfo.IsEmpty() )
                  throw Error( errorInfo );

               ++succeeded;
            }

            // Keep the GUI responsive.
            Module->ProcessEvents();
            if ( console.AbortRequested() )
               throw ProcessAborted();

            if ( !waitingThreads.IsEmpty() )
            {
               /*
                * If there are waiting threads, pop the first one from the
                * waiting threads list and put it in the free thread slot for
                * immediate execution.
                */
               *i = *waitingThreads;
               waitingThreads.Remove( waitingThreads.Begin() );
            }
            else
            {
               /*
                * If there are no more target frames to calibrate, simply wait
                * until all running threads terminate execution.
                */
               if ( targets.IsEmpty() )
               {
                  bool someRunning = false;
                  for ( thread_list::iterator j = runningThreads.Begin(); j != runningThreads.End(); ++j )
                     if ( *j != nullptr )
                     {
                        someRunning = true;
                        break;
                     }

                  /*
                   * If there are still running threads, continue waiting.
                   */
                  if ( someRunning )
                  {
                     if ( !waitingForFinished )
                     {
                        console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate..." );
                        waitingForFinished = true;
                     }
                     continue;
                  }

                  /*
                   * If there are no running threads, no waiting threads, and
                   * no pending target frames, then we are done.
                   */
                  break;
               }

               /*
                * We still have pending work to do: Extract the next target
                * frame from the targets list, load and calibrate it.
                */
               ImageItem item = *targets;
               targets.Remove( targets.Begin() );

               /*
                * Skip disabled targets.
                */
               if ( !item.enabled )
               {
                  ++skipped;
                  continue;
               }

               /*
                * Create a new thread for this target frame image, or if this
                * is a multiple-image file, a set of threads for all subimages
                * in this file.
                */
               thread_list threads = LoadTargetFrame( item.path, threadData );

               /*
                * Put the new thread, or the first thread if this is a
                * multiple-image file, in the free slot.
                */
               *i = *threads;
               threads.Remove( threads.Begin() );

               /*
                * Add the rest of subimages in a multiple-image file to the
                * list of waiting threads.
                */
               if ( !threads.IsEmpty() )
                  waitingThreads.Add( threads );
            }

            /*
             * If we have produced a new thread, run it.
             */
            if ( *i != nullptr )
            {
               size_type threadIndex = i - runningThreads.Begin();
               console.NoteLn( String().Format( "<end><cbr><br>[%03u] ", threadIndex ) + (*i)->TargetFilePath() );
               (*i)->Start( ThreadPriority::DefaultMax/*, i - runningThreads.Begin()*/ );
            }
         } // try
         catch ( ProcessAborted& )
         {
            /*
             * The user has requested to abort the process.
             */
            throw;
         }
         catch ( ... )
         {
            /*
             * The user has requested to abort the process.
             */
            if ( console.AbortRequested() )
               throw ProcessAborted();

            /*
             * Other errors handled according to the selected error policy.
             */
            ++failed;

            try
            {
               throw;
            }
            ERROR_HANDLER

            console.ResetStatus();
            console.EnableAbort();

            console.Note( "<end><cbr><br>* Applying error policy: " );

            switch ( p_onError )
            {
            default: // ?
            case ICOnError::Continue:
               console.NoteLn( "Continue on error." );
               continue;

            case ICOnError::Abort:
               console.NoteLn( "Abort on error." );
               throw ProcessAborted();

            case ICOnError::AskUser:
               {
                  console.NoteLn( "Ask on error..." );

                  int r = MessageBox( "<p style=\"white-space:pre;\">"
                     "An error occurred during ImageCalibration execution. What do you want to do?</p>",
                     "ImageCalibration",
                     StdIcon::Error,
                     StdButton::Ignore, StdButton::Abort ).Execute();

                  if ( r == StdButton::Abort )
                  {
                     console.NoteLn( "* Aborting as per user request." );
                     throw ProcessAborted();
                  }

                  console.NoteLn( "* Ignoring error as per user request." );
                  continue;
               }
            }
         }
      } // for ( ;; )
   } // try
   catch ( ... )
   {
      console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate..." );
      for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
         if ( *i != nullptr ) (*i)->Abort();
      for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
         if ( *i != nullptr ) (*i)->Wait();
      runningThreads.Destroy();
      waitingThreads.Destroy();
      throw;
   }

   /*
    * Fail if no images have been calibrated.
    */
   if ( succeeded == 0 )
   {
      if ( failed == 0 )
         throw Error( "No images were calibrated: Empty target frames list? No enabled target frames?" );
      throw Error( "No image could be calibrated." );
   }

   /*
    * Write the final report to the console.
    */
   console.NoteLn( String().Format( "<end><cbr><br>===== ImageCalibration: %u succeeded, %u failed, %u skipped =====",
                                    succeeded, failed, skipped ) );
   return true;
}

// ----------------------------------------------------------------------------

void* ImageCalibrationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheICTargetFrameEnabledParameter )
      return &p_targetFrames[tableRow].enabled;
   if ( p == TheICTargetFramePathParameter )
      return p_targetFrames[tableRow].path.Begin();

   if ( p == TheICEnableCFAParameter )
      return &p_enableCFA;
   if ( p == TheICCFAPatternParameter )
      return &p_cfaPattern;

   if ( p == TheICInputHintsParameter )
      return p_inputHints.Begin();
   if ( p == TheICOutputHintsParameter )
      return p_outputHints.Begin();

   if ( p == TheICPedestalParameter )
      return &p_pedestal;
   if ( p == TheICPedestalModeParameter )
      return &p_pedestalMode;
   if ( p == TheICPedestalKeywordParameter )
      return p_pedestalKeyword.Begin();

   if ( p == TheICOverscanEnabledParameter )
      return &p_overscan.enabled;

   if ( p == TheICOverscanImageX0Parameter )
      return &p_overscan.imageRect.x0;
   if ( p == TheICOverscanImageY0Parameter )
      return &p_overscan.imageRect.y0;
   if ( p == TheICOverscanImageX1Parameter )
      return &p_overscan.imageRect.x1;
   if ( p == TheICOverscanImageY1Parameter )
      return &p_overscan.imageRect.y1;

   if ( p == TheICOverscanRegionEnabledParameter )
      return &p_overscan.overscan[tableRow].enabled;

   if ( p == TheICOverscanSourceX0Parameter )
      return &p_overscan.overscan[tableRow].sourceRect.x0;
   if ( p == TheICOverscanSourceY0Parameter )
      return &p_overscan.overscan[tableRow].sourceRect.y0;
   if ( p == TheICOverscanSourceX1Parameter )
      return &p_overscan.overscan[tableRow].sourceRect.x1;
   if ( p == TheICOverscanSourceY1Parameter )
      return &p_overscan.overscan[tableRow].sourceRect.y1;

   if ( p == TheICOverscanTargetX0Parameter )
      return &p_overscan.overscan[tableRow].targetRect.x0;
   if ( p == TheICOverscanTargetY0Parameter )
      return &p_overscan.overscan[tableRow].targetRect.y0;
   if ( p == TheICOverscanTargetX1Parameter )
      return &p_overscan.overscan[tableRow].targetRect.x1;
   if ( p == TheICOverscanTargetY1Parameter )
      return &p_overscan.overscan[tableRow].targetRect.y1;

   if ( p == TheICMasterBiasEnabledParameter )
      return &p_masterBias.enabled;
   if ( p == TheICMasterBiasPathParameter )
      return p_masterBias.path.Begin();

   if ( p == TheICMasterDarkEnabledParameter )
      return &p_masterDark.enabled;
   if ( p == TheICMasterDarkPathParameter )
      return p_masterDark.path.Begin();

   if ( p == TheICMasterFlatEnabledParameter )
      return &p_masterFlat.enabled;
   if ( p == TheICMasterFlatPathParameter )
      return p_masterFlat.path.Begin();

   if ( p == TheICCalibrateBiasParameter )
      return &p_calibrateBias;
   if ( p == TheICCalibrateDarkParameter )
      return &p_calibrateDark;
   if ( p == TheICCalibrateFlatParameter )
      return &p_calibrateFlat;
   if ( p == TheICOptimizeDarksParameter )
      return &p_optimizeDarks;
   if ( p == TheICDarkOptimizationThresholdParameter )
      return &p_darkOptimizationThreshold;
   if ( p == TheICDarkOptimizationLowParameter )
      return &p_darkOptimizationLow;
   if ( p == TheICDarkOptimizationWindowParameter )
      return &p_darkOptimizationWindow;
   if ( p == TheICDarkCFADetectionModeParameter )
      return &p_darkCFADetectionMode;

   if ( p == TheICSeparateCFAFlatScalingFactorsParameter )
      return &p_separateCFAFlatScalingFactors;
   if ( p == TheICFlatScaleClippingFactorParameter )
      return &p_flatScaleClippingFactor;

   if ( p == TheICEvaluateNoiseParameter )
      return &p_evaluateNoise;
   if ( p == TheICNoiseEvaluationAlgorithmParameter )
      return &p_noiseEvaluationAlgorithm;

   if ( p == TheICOutputDirectoryParameter )
      return p_outputDirectory.Begin();
   if ( p == TheICOutputExtensionParameter )
      return p_outputExtension.Begin();
   if ( p == TheICOutputPrefixParameter )
      return p_outputPrefix.Begin();
   if ( p == TheICOutputPostfixParameter )
      return p_outputPostfix.Begin();
   if ( p == TheICOutputSampleFormatParameter )
      return &p_outputSampleFormat;
   if ( p == TheICOutputPedestalParameter )
      return &p_outputPedestal;
   if ( p == TheICOverwriteExistingFilesParameter )
      return &p_overwriteExistingFiles;
   if ( p == TheICOnErrorParameter )
      return &p_onError;
   if ( p == TheICNoGUIMessagesParameter )
      return &p_noGUIMessages;

   if ( p == TheICOutputFilePathParameter )
      return o_output[tableRow].outputFilePath.Begin();
   if ( p == TheICDarkScalingFactorRKParameter )
      return o_output[tableRow].darkScalingFactors.At( 0 );
   if ( p == TheICDarkScalingFactorGParameter )
      return o_output[tableRow].darkScalingFactors.At( 1 );
   if ( p == TheICDarkScalingFactorBParameter )
      return o_output[tableRow].darkScalingFactors.At( 2 );
   if ( p == TheICNoiseEstimateRKParameter )
      return o_output[tableRow].noiseEstimates.At( 0 );
   if ( p == TheICNoiseEstimateGParameter )
      return o_output[tableRow].noiseEstimates.At( 1 );
   if ( p == TheICNoiseEstimateBParameter )
      return o_output[tableRow].noiseEstimates.At( 2 );
   if ( p == TheICNoiseFractionRKParameter )
      return o_output[tableRow].noiseFractions.At( 0 );
   if ( p == TheICNoiseFractionGParameter )
      return o_output[tableRow].noiseFractions.At( 1 );
   if ( p == TheICNoiseFractionBParameter )
      return o_output[tableRow].noiseFractions.At( 2 );
   if ( p == TheICNoiseAlgorithmRKParameter )
      return o_output[tableRow].noiseAlgorithms[0].Begin();
   if ( p == TheICNoiseAlgorithmGParameter )
      return o_output[tableRow].noiseAlgorithms[1].Begin();
   if ( p == TheICNoiseAlgorithmBParameter )
      return o_output[tableRow].noiseAlgorithms[2].Begin();

   return nullptr;
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheICTargetFramesParameter )
   {
      p_targetFrames.Clear();
      if ( sizeOrLength > 0 )
         p_targetFrames.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheICTargetFramePathParameter )
   {
      p_targetFrames[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_targetFrames[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheICInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputHintsParameter )
   {
      p_outputHints.Clear();
      if ( sizeOrLength > 0 )
         p_outputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheICPedestalKeywordParameter )
   {
      p_pedestalKeyword.Clear();
      if ( sizeOrLength > 0 )
         p_pedestalKeyword.SetLength( sizeOrLength );
   }
   else if ( p == TheICOverscanRegionsParameter )
   {
      // Fixed-length table, so nothing to do.
   }
   else if ( p == TheICMasterBiasPathParameter )
   {
      p_masterBias.path.Clear();
      if ( sizeOrLength > 0 )
         p_masterBias.path.SetLength( sizeOrLength );
   }
   else if ( p == TheICMasterDarkPathParameter )
   {
      p_masterDark.path.Clear();
      if ( sizeOrLength > 0 )
         p_masterDark.path.SetLength( sizeOrLength );
   }
   else if ( p == TheICMasterFlatPathParameter )
   {
      p_masterFlat.path.Clear();
      if ( sizeOrLength > 0 )
         p_masterFlat.path.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputDirectoryParameter )
   {
      p_outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_outputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputExtensionParameter )
   {
      p_outputExtension.Clear();
      if ( sizeOrLength > 0 )
         p_outputExtension.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputPrefixParameter )
   {
      p_outputPrefix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPrefix.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputPostfixParameter )
   {
      p_outputPostfix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPostfix.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputDataParameter )
   {
      o_output.Clear();
      if ( sizeOrLength > 0 )
         o_output.Add( OutputData(), sizeOrLength );
   }
   else if ( p == TheICOutputFilePathParameter )
   {
      o_output[tableRow].outputFilePath.Clear();
      if ( sizeOrLength > 0 )
         o_output[tableRow].outputFilePath.SetLength( sizeOrLength );
   }
   else if ( p == TheICNoiseAlgorithmRKParameter )
   {
      o_output[tableRow].noiseAlgorithms[0].Clear();
      if ( sizeOrLength > 0 )
         o_output[tableRow].noiseAlgorithms[0].SetLength( sizeOrLength );
   }
   else if ( p == TheICNoiseAlgorithmGParameter )
   {
      o_output[tableRow].noiseAlgorithms[1].Clear();
      if ( sizeOrLength > 0 )
         o_output[tableRow].noiseAlgorithms[1].SetLength( sizeOrLength );
   }
   else if ( p == TheICNoiseAlgorithmBParameter )
   {
      o_output[tableRow].noiseAlgorithms[2].Clear();
      if ( sizeOrLength > 0 )
         o_output[tableRow].noiseAlgorithms[2].SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type ImageCalibrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheICTargetFramesParameter )
      return p_targetFrames.Length();
   if ( p == TheICTargetFramePathParameter )
      return p_targetFrames[tableRow].path.Length();
   if ( p == TheICInputHintsParameter )
      return p_inputHints.Length();
   if ( p == TheICOutputHintsParameter )
      return p_outputHints.Length();
   if ( p == TheICPedestalKeywordParameter )
      return p_pedestalKeyword.Length();
   if ( p == TheICOverscanRegionsParameter )
      return 4;
   if ( p == TheICMasterBiasPathParameter )
      return p_masterBias.path.Length();
   if ( p == TheICMasterDarkPathParameter )
      return p_masterDark.path.Length();
   if ( p == TheICMasterFlatPathParameter )
      return p_masterFlat.path.Length();
   if ( p == TheICOutputDirectoryParameter )
      return p_outputDirectory.Length();
   if ( p == TheICOutputExtensionParameter )
      return p_outputExtension.Length();
   if ( p == TheICOutputPrefixParameter )
      return p_outputPrefix.Length();
   if ( p == TheICOutputPostfixParameter )
      return p_outputPostfix.Length();
   if ( p == TheICOutputDataParameter )
      return o_output.Length();
   if ( p == TheICOutputFilePathParameter )
      return o_output[tableRow].outputFilePath.Length();
   if ( p == TheICNoiseAlgorithmRKParameter )
      return o_output[tableRow].noiseAlgorithms[0].Length();
   if ( p == TheICNoiseAlgorithmGParameter )
      return o_output[tableRow].noiseAlgorithms[1].Length();
   if ( p == TheICNoiseAlgorithmBParameter )
      return o_output[tableRow].noiseAlgorithms[2].Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageCalibrationInstance.cpp - Released 2021-04-09T19:41:48Z
