//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.35
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.9.7
// ----------------------------------------------------------------------------
// ImageCalibrationInstance.cpp - Released 2022-11-21T14:47:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
//
// Copyright (c) 2003-2022 Pleiades Astrophoto S.L. All Rights Reserved.
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
#include <pcl/AutoStatusCallbackRestorer.h>
#include <pcl/ErrorHandler.h>
#include <pcl/GlobalSettings.h>
#include <pcl/IntegerResample.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/MuteStatus.h>
#include <pcl/PSFSignalEstimator.h>
#include <pcl/RobustChauvenetRejection.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/SpinStatus.h>
#include <pcl/Version.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * 5x5 B3-spline wavelet scaling function used for the noise evaluation.
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

// ----------------------------------------------------------------------------

/*
 * PSF signal evaluation keyword prefixes.
 */
static const char* s_psfPrefixes[] = { "PSFFLX", "PSFFLP", "PSFMFL", "PSFMFP", "PSFMST", "PSFNST", "PSFSGN", "PSFSGTYP" };

// ----------------------------------------------------------------------------

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

/*
 * The largest automatic pedestal allowed for sanity.
 */
#define HUGE_PEDESTAL         1000

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
   , p_evaluateSignal( TheICEvaluateSignalParameter->DefaultValue() )
   , p_structureLayers( TheICStructureLayersParameter->DefaultValue() )
   , p_saturationThreshold( TheICSaturationThresholdParameter->DefaultValue() )
   , p_saturationRelative( TheICSaturationRelativeParameter->DefaultValue() )
   , p_noiseLayers( TheICNoiseLayersParameter->DefaultValue() )
   , p_hotPixelFilterRadius( TheICHotPixelFilterRadiusParameter->DefaultValue() )
   , p_noiseReductionFilterRadius( TheICNoiseReductionFilterRadiusParameter->DefaultValue() )
   , p_minStructureSize( TheICMinStructureSizeParameter->DefaultValue() )
   , p_psfType( ICPSFType::Default )
   , p_psfGrowth( TheICPSFGrowthParameter->DefaultValue() )
   , p_maxStars( TheICMaxStarsParameter->DefaultValue() )
   , p_outputDirectory( TheICOutputDirectoryParameter->DefaultValue() )
   , p_outputExtension( TheICOutputExtensionParameter->DefaultValue() )
   , p_outputPrefix( TheICOutputPrefixParameter->DefaultValue() )
   , p_outputPostfix( TheICOutputPostfixParameter->DefaultValue() )
   , p_outputSampleFormat( ICOutputSampleFormat::Default )
   , p_outputPedestal( TheICOutputPedestalParameter->DefaultValue() )
   , p_outputPedestalMode( ICOutputPedestalMode::Default )
   , p_autoPedestalLimit( TheICAutoPedestalLimitParameter->DefaultValue() )
   , p_overwriteExistingFiles( TheICOverwriteExistingFilesParameter->DefaultValue() )
   , p_onError( ICOnError::Default )
   , p_noGUIMessages( TheICNoGUIMessagesParameter->DefaultValue() ) // ### DEPRECATED
   , p_useFileThreads( TheICUseFileThreadsParameter->DefaultValue() )
   , p_fileThreadOverload( TheICFileThreadOverloadParameter->DefaultValue() )
   , p_maxFileReadThreads( int32( TheICMaxFileReadThreadsParameter->DefaultValue() ) )
   , p_maxFileWriteThreads( int32( TheICMaxFileWriteThreadsParameter->DefaultValue() ) )
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
      p_evaluateSignal                = x->p_evaluateSignal;
      p_structureLayers               = x->p_structureLayers;
      p_saturationThreshold           = x->p_saturationThreshold;
      p_saturationRelative            = x->p_saturationRelative;
      p_noiseLayers                   = x->p_noiseLayers;
      p_hotPixelFilterRadius          = x->p_hotPixelFilterRadius;
      p_noiseReductionFilterRadius    = x->p_noiseReductionFilterRadius;
      p_minStructureSize              = x->p_minStructureSize;
      p_psfType                       = x->p_psfType;
      p_psfGrowth                     = x->p_psfGrowth;
      p_maxStars                      = x->p_maxStars;
      p_outputDirectory               = x->p_outputDirectory;
      p_outputExtension               = x->p_outputExtension;
      p_outputPrefix                  = x->p_outputPrefix;
      p_outputPostfix                 = x->p_outputPostfix;
      p_outputSampleFormat            = x->p_outputSampleFormat;
      p_outputPedestal                = x->p_outputPedestal;
      p_outputPedestalMode            = x->p_outputPedestalMode;
      p_autoPedestalLimit         = x->p_autoPedestalLimit;
      p_overwriteExistingFiles        = x->p_overwriteExistingFiles;
      p_onError                       = x->p_onError;
      p_noGUIMessages                 = x->p_noGUIMessages;
      p_useFileThreads                = x->p_useFileThreads;
      p_fileThreadOverload            = x->p_fileThreadOverload;
      p_maxFileReadThreads            = x->p_maxFileReadThreads;
      p_maxFileWriteThreads           = x->p_maxFileWriteThreads;

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
            float* __restrict__ t  = target.PixelData( c );
            float* __restrict__ t1 = t + target.NumberOfPixels();
      const float* __restrict__ b  = bias.PixelData( Min( c, bias.NumberOfChannels()-1 ) );
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
            float* __restrict__ t  = target.PixelData( c );
            float* __restrict__ t1 = t + target.NumberOfPixels();
      const float* __restrict__ d  = dark.PixelData( Min( c, dark.NumberOfChannels()-1 ) );
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
         float* __restrict__ t  = target.PixelData( tCh );
         float* __restrict__ t1 = t + target.NumberOfPixels();
   const float* __restrict__ d  = dark.PixelData( dCh );
   LOOP *t++ -= SCALED_DARK;
}

// ----------------------------------------------------------------------------

/*
 * Full calibration routine
 */
static void Calibrate( Image& target,
                       const Image* bias,
                       const Image* dark, const FVector& dScale,
                       const Image* flat )
{
   static const float TINY = 1.0e-15F; // to prevent divisions by zero flat pixels

   int nt = target.NumberOfChannels();
   int nb = (bias != nullptr) ? bias->NumberOfChannels() : 0;
   int nd = (dark != nullptr) ? dark->NumberOfChannels() : 0;
   int nf = (flat != nullptr) ? flat->NumberOfChannels() : 0;

   for ( int c = 0; c < nt; ++c )
   {
      Module->ProcessEvents();

            float* __restrict__ t  = target.PixelData( c );
            float* __restrict__ t1 = t + target.NumberOfPixels();
      const float* __restrict__ b  = (bias != nullptr) ? bias->PixelData( Min( c, nb-1 ) ) : nullptr;
      const float* __restrict__ d  = (dark != nullptr) ? dark->PixelData( Min( c, nd-1 ) ) : nullptr;
      const float* __restrict__ f  = (flat != nullptr) ? flat->PixelData( Min( c, nf-1 ) ) : nullptr;
            float  k  = (dark != nullptr) ? dScale[c] : .0F;

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
 * Quick estimation of standard deviation after dark subtraction.
 */
static double TestDarkOptimization( float k, const Image& target, const Image& dark )
{
   Image t( target );
   t.Status().DisableInitialization();
   SubtractOneChannelDark( t, 0, dark, Min( target.SelectedChannel(), dark.NumberOfChannels()-1 ), k );
   return t.StdDev();
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

   static const float GOLD = 1.618034F;
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

   CalibrationThread( const CalibrationThreadData& data, size_type index, int maxSubThreads )
      : m_data( data )
      , m_index( index )
      , m_maxSubThreads( maxSubThreads )
      , m_targetPath( m_data.instance->p_targetFrames[m_index].path )
   {
   }

   void Run() override
   {
      try
      {
         Module->ProcessEvents();

         ReadInputData();
         Perform();
         WriteOutputData();

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
         Console().Write( text );
      }
   }

   size_type Index() const
   {
      return m_index;
   }

   void GetOutputData( ImageCalibrationInstance::OutputData& o ) const
   {
      o.outputFilePath = m_outputFilePath;
      if ( m_data.instance->p_masterDark.enabled )
         if ( m_data.instance->p_optimizeDarks )
            for ( int i = 0; i < Min( 3, m_K.Length() ); ++i )
               o.darkScalingFactors[i] = m_K[i];
      if ( m_data.instance->p_evaluateSignal )
         for ( int i = 0; i < Min( 3, m_noiseEstimates.Length() ); ++i )
         {
            o.psfTotalFluxEstimates[i] = m_psfTotalFluxEstimates[i];
            o.psfTotalPowerFluxEstimates[i] = m_psfTotalPowerFluxEstimates[i];
            o.psfTotalMeanFluxEstimates[i] = m_psfTotalMeanFluxEstimates[i];
            o.psfTotalMeanPowerFluxEstimates[i] = m_psfTotalMeanPowerFluxEstimates[i];
            o.psfMStarEstimates[i] = m_psfMStarEstimates[i];
            o.psfNStarEstimates[i] = m_psfNStarEstimates[i];
            o.psfCounts[i] = m_psfCounts[i];
         }
      if ( m_data.instance->p_evaluateNoise )
         for ( int i = 0; i < Min( 3, m_noiseEstimates.Length() ); ++i )
         {
            o.noiseEstimates[i] = m_noiseEstimates[i];
            o.noiseFractions[i] = m_noiseFractions[i];
            o.noiseScaleLow[i] = m_noiseScaleLow[i];
            o.noiseScaleHigh[i] = m_noiseScaleHigh[i];
            o.noiseAlgorithms[i] = String( m_noiseAlgorithms[i] );
         }
   }

   const String& TargetFilePath() const
   {
      return m_targetPath;
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

   const CalibrationThreadData& m_data;            // calibration master frames and general parameters
         size_type              m_index;           // position of this thread in the instance's target list
         int                    m_maxSubThreads;   // maximum number of sub-threads for this thread
         String                 m_targetPath;      // file path of this target image
         ImageVariant           m_target;          // the image being calibrated
         OutputFileData         m_outputData;      // target image parameters and embedded data
         String                 m_errorInfo;       // last error message
         bool                   m_success = false; // true if the thread completed execution successfully

   const Image& TargetImage() const
   {
      return static_cast<const Image&>( *m_target );
   }

   Image& TargetImage()
   {
      return static_cast<Image&>( *m_target );
   }

   /*
    * Output data
    */

   // Full path to the generated output file
   String     m_outputFilePath;

   // Optimized dark scaling factors
   FVector    m_K;

   // Automatic or prescribed pedestal, in 16-bit DN.
   int        m_outputPedestal = 0;

   // Signal estimates
   Vector     m_psfTotalFluxEstimates;
   Vector     m_psfTotalPowerFluxEstimates;
   Vector     m_psfTotalMeanFluxEstimates;
   Vector     m_psfTotalMeanPowerFluxEstimates;
   Vector     m_psfMStarEstimates;
   Vector     m_psfNStarEstimates;
   IVector    m_psfCounts;

   // Noise estimates
   Vector     m_noiseEstimates;
   Vector     m_noiseFractions;
   Vector     m_noiseScaleLow;
   Vector     m_noiseScaleHigh;
   StringList m_noiseAlgorithms;

   //

   void ReadInputData()
   {
      Console console;
      console.WriteLn( "<end><cbr>* Loading target calibration frame: <raw>" + m_targetPath + "</raw>" );

      /*
       * Find out an installed file format that can read image files with the
       * specified extension ...
       */
      FileFormat format( File::ExtractExtension( m_targetPath ), true/*read*/, false/*write*/ );

      /*
       * ... and create a format instance (usually a disk file) to access this
       * target image.
       */
      FileFormatInstance file( format );

      /*
       * Open the image file.
       */
      ImageDescriptionArray images;
      if ( !file.Open( images, m_targetPath, m_data.instance->p_inputHints ) )
         throw CaughtException();

      if ( images.IsEmpty() )
         throw Error( m_targetPath + ": Empty image file." );

      /*
       * Multiple-image files not supported for calibration.
       */
      if ( images.Length() > 1 )
         console.NoteLn( String().Format( "* Ignoring %u additional image(s) in target file.", images.Length()-1 ) );

      /*
       * Create a shared image, 32-bit floating point format.
       */
      m_target.CreateSharedFloatImage( 32/*bitsPerSample*/ );
      m_target.SetMaxProcessors( m_maxSubThreads );

      /*
       * Read the image file.
       */
      {
         static Mutex mutex;
         static AtomicInt count;
         volatile AutoLockCounter lock( mutex, count, m_data.instance->m_maxFileReadThreads );

         /*
          * Read the image.
          */
         if ( !file.ReadImage( m_target ) )
            throw CaughtException();

         /*
          * Enforce consistency of image dimensions.
          */
         m_data.instance->ValidateImageGeometry( TargetImage() );

         /*
          * Optional pedestal subtraction.
          */
         m_data.instance->SubtractPedestal( TargetImage(), file );

         /*
          * Retrieve metadata and auxiliary data structures that must be
          * preserved in the output image (properties, header keywords, etc).
          */
         m_outputData = OutputFileData( file, images[0].options );

         /*
          * Close the input stream.
          */
         if ( !file.Close() )
            throw CaughtException();
      }
   }

   void Perform()
   {
      m_target->Status().DisableInitialization();

      Console console;

      /*
       * Overscan correction.
       */
      if ( m_data.instance->p_overscan.enabled )
      {
         console.WriteLn( "<end><cbr>* Applying overscan correction." );
         SubtractOverscan( TargetImage(), m_data.overscan, m_data.instance->p_overscan.imageRect );
      }

      /*
       * Dark frame optimization.
       */
      if ( m_data.dark != nullptr && m_data.instance->p_optimizeDarks && m_data.optimizingDark != nullptr )
      {
         console.WriteLn( "<end><cbr>* Computing dark frame optimization factors." );
         m_K = OptimizeDark( TargetImage(), *m_data.optimizingDark, m_data.darkCFAPattern );
      }
      else
         m_K = FVector( 1.0F, m_target->NumberOfChannels() );

      /*
       * Target frame calibration.
       */
      console.WriteLn( "<end><cbr>* Performing image calibration." );
      Calibrate( TargetImage(), m_data.bias, m_data.dark, m_K, m_data.flat );

      /*
       * Optional pedestal to enforce positivity.
       */
      switch ( m_data.instance->p_outputPedestalMode )
      {
      default: // ?!
      case ICOutputPedestalMode::Literal:
         m_outputPedestal = m_data.instance->p_outputPedestal;
         break;
      case ICOutputPedestalMode::Auto:
         {
            float m = TargetImage().MinimumSampleValue( Rect( 0 ), 0, TargetImage().NumberOfChannels()-1 );
            if ( m < 2*std::numeric_limits<float>::epsilon() )
            {
               Array<float> A;
               for ( int c = 0; c < TargetImage().NumberOfChannels(); ++c )
                  for ( Image::const_sample_iterator i( TargetImage(), c ); i; ++i )
                     if ( *i < 2*std::numeric_limits<float>::epsilon() )
                        A << *i;
               size_type n = size_type( m_data.instance->p_autoPedestalLimit * double( TargetImage().NumberOfPixels() ) );
               if ( A.Length() >= n )
               {
                  A.Sort();
                  m_outputPedestal = Max( 1, RoundInt( 65535*Abs( A[n-1] ) ) );
                  console.WarningLn( String().Format( "<end><cbr>** Warning: negative or insignificant "
                     "pixel sample values detected after calibration.<br>"
                     "Applying automatic pedestal: %d DN%s",
                     Min( m_outputPedestal, HUGE_PEDESTAL ), (m_outputPedestal > HUGE_PEDESTAL) ? " (huge!)" : "" ) );
                  if ( m_outputPedestal > HUGE_PEDESTAL )
                     m_outputPedestal = HUGE_PEDESTAL;
               }
            }
         }
         break;
      }
      if ( m_outputPedestal != 0 )
         TargetImage() += m_outputPedestal/65535.0;

      {
         double mn, mx;
         TargetImage().GetExtremeSampleValues( mn, mx, Rect( 0 ), 0, TargetImage().NumberOfChannels()-1 );
         console.WriteLn( String().Format( "<end><cbr>Calibration range: [%.6e,%.6e]%s", mn, mx, (mx >= 1) ? " (truncated)" : "" ) );
      }

      /*
       * Constrain the calibrated target image to the [0,1] range.
       */
      TargetImage().Truncate();

      /*
       * Signal and noise evaluation.
       */
      if ( m_data.instance->p_evaluateSignal || m_data.instance->p_evaluateNoise )
      {
         console.WriteLn( "<end><cbr>* Performing signal and noise evaluation." );
         EvaluateSignalAndNoise( m_psfTotalFluxEstimates, m_psfTotalPowerFluxEstimates,
                                 m_psfTotalMeanFluxEstimates, m_psfTotalMeanPowerFluxEstimates,
                                 m_psfMStarEstimates, m_psfNStarEstimates, m_psfCounts,
                                 m_noiseEstimates, m_noiseFractions, m_noiseScaleLow, m_noiseScaleHigh, m_noiseAlgorithms,
                                 m_target, m_data.darkCFAPattern );
      }
   }

   void WriteOutputData()
   {
      Console console;

      /*
       * Output directory.
       */
      String fileDir = m_data.instance->p_outputDirectory.Trimmed();
      if ( fileDir.IsEmpty() )
         fileDir = File::ExtractDrive( m_targetPath ) + File::ExtractDirectory( m_targetPath );
      if ( fileDir.IsEmpty() )
         throw Error( m_targetPath + ": Unable to determine an output directory." );
      if ( !fileDir.EndsWith( '/' ) )
         fileDir.Append( '/' );

      /*
       * Output file extension, which defines the output file format.
       */
      String fileExtension = m_data.instance->p_outputExtension.Trimmed();
      if ( fileExtension.IsEmpty() )
         fileExtension = File::ExtractExtension( m_targetPath );
      if ( fileExtension.IsEmpty() )
         throw Error( m_targetPath + ": Unable to determine an output file extension." );
      if ( !fileExtension.StartsWith( '.' ) )
         fileExtension.Prepend( '.' );

      /*
       * Output file name.
       */
      String fileName = File::ExtractName( m_targetPath ).Trimmed();
      if ( !m_data.instance->p_outputPrefix.IsEmpty() )
         fileName.Prepend( m_data.instance->p_outputPrefix );
      if ( !m_data.instance->p_outputPostfix.IsEmpty() )
         fileName.Append( m_data.instance->p_outputPostfix );
      if ( fileName.IsEmpty() )
         throw Error( m_targetPath + ": Unable to determine an output file name." );

      /*
       * Output file path.
       */
      m_outputFilePath = fileDir + fileName + fileExtension;

      /*
       * Write dark optimization factors to the console.
       */
      if ( m_data.instance->p_optimizeDarks )
      {
         console.WriteLn( "Dark scaling factors:" );
         for ( int i = 0; i < m_K.Length(); ++i )
         {
            console.WriteLn( String().Format( "ch %d : k = %.3f", i, m_K[i] ) );
            if ( m_K[i] <= NO_DARK_CORRELATION )
               console.WarningLn( String().Format( "** Warning: No correlation "
                                  "between the master dark and target frames (channel %d).", i ) );
         }
      }

      /*
       * Write signal evaluation information to the console.
       */
      if ( m_data.instance->p_evaluateSignal )
      {
         if ( m_psfCounts.Sum() > 0 )
         {
            console.WriteLn( "<end><cbr>PSF signal estimates:" );
            for ( int i = 0; i < m_psfCounts.Length(); ++i )
               if ( m_psfCounts[i] > 0 )
                  console.WriteLn( String().Format( "ch %d : TFlux = %.4e, TMeanFlux = %.4e, M* = %.4e, N* = %.4e, %d PSF fits",
                                                    i,
                                                    m_psfTotalFluxEstimates[i], m_psfTotalMeanFluxEstimates[i],
                                                    m_psfMStarEstimates[i], m_psfNStarEstimates[i], m_psfCounts[i] ) );
               else
                  console.WarningLn( String().Format( "** Warning: No valid PSF signal samples (channel %d).", i ) );
         }
         else
            console.WarningLn( "** Warning: No valid PSF signal samples." );
      }

      /*
       * Write noise evaluation information to the console.
       */
      if ( m_data.instance->p_evaluateNoise )
      {
         console.WriteLn( "Noise estimates:" );
         for ( int i = 0; i < m_noiseEstimates.Length(); ++i )
            console.WriteLn( String().Format( "ch %d : sigma_n = %.4e, %.2f%% pixels ", i, m_noiseEstimates[i], m_noiseFractions[i]*100 )
                             + '(' + m_noiseAlgorithms[i] + ')' );

         console.WriteLn( "Noise scaling factors:" );
         for ( int i = 0; i < m_noiseEstimates.Length(); ++i )
            console.WriteLn( String().Format( "ch %d : sigma_low = %.6e, sigma_high = %.6e", i, m_noiseScaleLow[i], m_noiseScaleHigh[i] ) );
      }

      console.WriteLn( "<end><cbr>* Writing output file: <raw>" + m_outputFilePath + "</raw>" );

      /*
       * Check for an already existing file, and either overwrite it (but show
       * a warning message if that happens), or find a unique file name,
       * depending on the overwriteExistingFiles parameter.
       */
      UniqueFileChecks checks = File::EnsureNewUniqueFile( m_outputFilePath, m_data.instance->p_overwriteExistingFiles );
      if ( checks.overwrite )
         console.WarningLn( "** Warning: Overwriting existing file." );
      else if ( checks.exists )
         console.NoteLn( "* File already exists, writing to: <raw>" + m_outputFilePath + "</raw>" );

      /*
       * Find an installed file format able to write files with the specified
       * extension ...
       */
      FileFormat outputFormat( fileExtension, false/*read*/, true/*write*/ );

      if ( outputFormat.IsDeprecated() )
         console.WarningLn( "** Warning: Deprecated file format: " + outputFormat.Name() );

      /*
       * ... and build a format instance (usually a disk file).
       */
      FileFormatInstance outputFile( outputFormat );

      /*
       * Create the output stream.
       */
      if ( !outputFile.Create( m_outputFilePath, m_data.instance->p_outputHints ) )
         throw CaughtException();

      /*
       * Gather relevant image options, including the output sample format
       * (bits per pixel sample and numeric type).
       */
      ImageOptions options = m_outputData.options; // get resolution, sample format, etc.

      /*
       * Determine the output sample format: bits per sample and whether
       * integer or real samples.
       */
      switch ( m_data.instance->p_outputSampleFormat )
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
       * determined) sample format. If it doesn't, then the user should know
       * what he/she is doing, but we always inform anyway.
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
       * File formats often use format-specific data. Keep track of private
       * data structures.
       */
      if ( m_outputData.fsData != nullptr )
         if ( outputFormat.UsesFormatSpecificData() )
            if ( outputFormat.ValidateFormatSpecificData( m_outputData.fsData ) )
               outputFile.SetFormatSpecificData( m_outputData.fsData );

      /*
       * Set image properties.
       */
      if ( !m_outputData.properties.IsEmpty() )
         if ( outputFormat.CanStoreImageProperties() && outputFormat.SupportsViewProperties() )
            outputFile.WriteImageProperties( m_outputData.properties );
         else
            console.WarningLn( "** Warning: The output format cannot store image properties - existing properties not embedded" );

      /*
       * Add FITS header keywords and preserve existing ones, if possible.
       * N.B.: A COMMENT or HISTORY keyword cannot have a value; these keywords
       * only have the name and comment components.
       */
      if ( outputFormat.CanStoreKeywords() )
      {
         FITSKeywordArray keywords = m_outputData.keywords;

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

         if ( !m_data.instance->p_inputHints.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       "ImageCalibration.inputHints: " + IsoString( m_data.instance->p_inputHints ) );
         if ( !m_data.instance->p_outputHints.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       "ImageCalibration.outputHints: " + IsoString( m_data.instance->p_outputHints ) );

         keywords << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       "ImageCalibration.overscan.enabled: " + IsoString( bool( m_data.instance->p_overscan.enabled ) ) );
         if ( m_data.instance->p_overscan.enabled )
         {
            const Rect& r = m_data.instance->p_overscan.imageRect;
            keywords << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       IsoString().Format( "ImageCalibration.overscan.imageRect: {%d,%d,%d,%d}",
                                                           r.x0, r.y0, r.x1, r.y1 ) );
            for ( int i = 0; i < 4; ++i )
               if ( m_data.instance->p_overscan.overscan[i].enabled )
               {
                  const Rect& s = m_data.instance->p_overscan.overscan[i].sourceRect;
                  const Rect& t = m_data.instance->p_overscan.overscan[i].targetRect;
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
                                       "ImageCalibration.masterBias.enabled: " + IsoString( bool( m_data.instance->p_masterBias.enabled ) ) );
         if ( m_data.instance->p_masterBias.enabled )
            keywords << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       "ImageCalibration.masterBias.fileName: " +
                                       File::ExtractNameAndExtension( m_data.instance->p_masterBias.path ).To7BitASCII() )
                     << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       "ImageCalibration.masterBias.calibrate: " + IsoString( bool( m_data.instance->p_calibrateBias ) ) );

         keywords << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       "ImageCalibration.masterDark.enabled: " + IsoString( bool( m_data.instance->p_masterDark.enabled ) ) );
         if ( m_data.instance->p_masterDark.enabled )
         {
            keywords << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       "ImageCalibration.masterDark.fileName: " +
                                       File::ExtractNameAndExtension( m_data.instance->p_masterDark.path ).To7BitASCII() )
                     << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       "ImageCalibration.masterDark.calibrate: " + IsoString( bool( m_data.instance->p_calibrateDark ) ) );
            if ( m_data.instance->p_optimizeDarks )
            {
               IsoString darkScalingFactors = "ImageCalibration.masterDark.scalingFactors:";
               for ( int i = 0; i < m_K.Length(); ++i )
                  darkScalingFactors.AppendFormat( " %.3f", m_K[i] );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), darkScalingFactors );

               if ( m_data.instance->p_darkOptimizationThreshold > 0 )
                  keywords << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       IsoString().Format( "ImageCalibration.masterDark.optimizationThreshold: %.5f", m_data.instance->p_darkOptimizationThreshold ) )
                           << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       IsoString().Format( "ImageCalibration.masterDark.optimizationLow: %.4f", m_data.instance->p_darkOptimizationLow ) );

               if ( m_data.instance->p_darkOptimizationWindow > 0 )
                  keywords << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       IsoString().Format( "ImageCalibration.masterDark.optimizationWindow: %d px", m_data.instance->p_darkOptimizationWindow ) );
            }
         }

         keywords << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       "ImageCalibration.masterFlat.enabled: " + IsoString( bool( m_data.instance->p_masterFlat.enabled ) ) );
         if ( m_data.instance->p_masterFlat.enabled )
         {
            keywords << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       "ImageCalibration.masterFlat.fileName: " +
                                       File::ExtractNameAndExtension( m_data.instance->p_masterFlat.path ).To7BitASCII() )
                     << FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       "ImageCalibration.masterFlat.calibrate: " + IsoString( bool( m_data.instance->p_calibrateFlat ) ) );

            IsoString flatScalingFactors = "ImageCalibration.masterFlat.scalingFactors:";
            for ( int i = 0; i < m_data.flatScalingFactors.Length(); ++i )
               flatScalingFactors.AppendFormat( " %.6f", m_data.flatScalingFactors[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), flatScalingFactors );
         }

         if ( m_data.instance->p_evaluateSignal )
         {
            /*
             * N.B.: If we already have computed signal estimates, remove any
             * previously existing PSF signal keywords. Only our newly created
             * keywords must be present in the header.
             */
            for ( size_type i = 0; i < keywords.Length(); )
            {
               if ( keywords[i].name.StartsWithIC( "PSF" ) )
                  for ( size_type j = 0; j < ItemsInArray( s_psfPrefixes ); ++j )
                     if ( keywords[i].name.StartsWithIC( s_psfPrefixes[j] ) )
                     {
                        keywords.Remove( keywords.At( i ) );
                        continue;
                     }
               ++i;
            }

            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "PSF signal evaluation with " + Module->ReadableVersion() );

            IsoString psfTotalFluxEstimates = "ImageCalibration.psfTotalFluxEstimates:";
            for ( int i = 0; i < m_psfTotalFluxEstimates.Length(); ++i )
               psfTotalFluxEstimates.AppendFormat( " %.4e", m_psfTotalFluxEstimates[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), psfTotalFluxEstimates );

            IsoString psfTotalPowerFluxEstimates = "ImageCalibration.psfTotalPowerFluxEstimates:";
            for ( int i = 0; i < m_psfTotalPowerFluxEstimates.Length(); ++i )
               psfTotalPowerFluxEstimates.AppendFormat( " %.4e", m_psfTotalPowerFluxEstimates[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), psfTotalPowerFluxEstimates );

            IsoString psfTotalMeanFluxEstimates = "ImageCalibration.psfTotalMeanFluxEstimates:";
            for ( int i = 0; i < m_psfTotalMeanFluxEstimates.Length(); ++i )
               psfTotalMeanFluxEstimates.AppendFormat( " %.4e", m_psfTotalMeanFluxEstimates[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), psfTotalMeanFluxEstimates );

            IsoString psfTotalMeanPowerFluxEstimates = "ImageCalibration.psfTotalMeanPowerFluxEstimates:";
            for ( int i = 0; i < m_psfTotalMeanPowerFluxEstimates.Length(); ++i )
               psfTotalMeanPowerFluxEstimates.AppendFormat( " %.4e", m_psfTotalMeanPowerFluxEstimates[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), psfTotalMeanPowerFluxEstimates );

            IsoString psfMStarEstimates = "ImageCalibration.psfMStarEstimates:";
            for ( int i = 0; i < m_psfMStarEstimates.Length(); ++i )
               psfMStarEstimates.AppendFormat( " %.4e", m_psfMStarEstimates[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), psfMStarEstimates );

            IsoString psfNStarEstimates = "ImageCalibration.psfNStarEstimates:";
            for ( int i = 0; i < m_psfNStarEstimates.Length(); ++i )
               psfNStarEstimates.AppendFormat( " %.4e", m_psfNStarEstimates[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), psfNStarEstimates );

            IsoString psfCounts = "ImageCalibration.psfCounts:";
            for ( int i = 0; i < m_psfCounts.Length(); ++i )
               psfCounts.AppendFormat( " %d", m_psfCounts[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), psfCounts );

            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageCalibration.psfType: "
                                             + ICPSFType::FunctionName( m_data.instance->p_psfType ) );

            for ( int i = 0; i < m_psfCounts.Length(); ++i )
               keywords << FITSHeaderKeyword( IsoString().Format( "PSFFLX%02d", i ),
                                              IsoString().Format( "%.4e", m_psfTotalFluxEstimates[i] ),
                                              IsoString().Format( "Sum of PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFFLP%02d", i ),
                                              IsoString().Format( "%.4e", m_psfTotalPowerFluxEstimates[i] ),
                                              IsoString().Format( "Sum of squared PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMFL%02d", i ),
                                              IsoString().Format( "%.4e", m_psfTotalMeanFluxEstimates[i] ),
                                              IsoString().Format( "Sum of mean PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMFP%02d", i ),
                                              IsoString().Format( "%.4e", m_psfTotalMeanPowerFluxEstimates[i] ),
                                              IsoString().Format( "Sum of mean squared PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMST%02d", i ),
                                              IsoString().Format( "%.4e", m_psfMStarEstimates[i] ),
                                              IsoString().Format( "M* mean background estimate, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFNST%02d", i ),
                                              IsoString().Format( "%.4e", m_psfNStarEstimates[i] ),
                                              IsoString().Format( "N* noise estimate, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFSGN%02d", i ),
                                              IsoString().Format( "%d", m_psfCounts[i] ),
                                              IsoString().Format( "Number of valid PSF flux estimates, channel #%d", i ) );

            keywords << FITSHeaderKeyword( "PSFSGTYP",
                                           ICPSFType::FunctionName( m_data.instance->p_psfType ).SingleQuoted(),
                                           "PSF type used for signal estimation" );
         }

         if ( m_data.instance->p_evaluateNoise )
         {
            /*
             * N.B.: If we already have computed noise estimates, remove any
             * previously existing NOISExx keywords. Only our newly created
             * keywords must be present in the header.
             */
            for ( size_type i = 0; i < keywords.Length(); )
               if ( keywords[i].name.StartsWithIC( "NOISE" ) )
                  keywords.Remove( keywords.At( i ) );
               else
                  ++i;

            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "Noise evaluation with " + Module->ReadableVersion() );

            IsoString noiseEstimates = "ImageCalibration.noiseEstimates:";
            for ( int i = 0; i < m_noiseEstimates.Length(); ++i )
               noiseEstimates.AppendFormat( " %.4e", m_noiseEstimates[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), noiseEstimates );

            IsoString noiseScaleLow = "ImageCalibration.noiseScaleLow:";
            for ( int i = 0; i < m_noiseScaleLow.Length(); ++i )
               noiseScaleLow.AppendFormat( " %.6e", m_noiseScaleLow[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), noiseScaleLow );

            IsoString noiseScaleHigh = "ImageCalibration.noiseScaleHigh:";
            for ( int i = 0; i < m_noiseScaleHigh.Length(); ++i )
               noiseScaleHigh.AppendFormat( " %.6e", m_noiseScaleHigh[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), noiseScaleHigh );

            for ( int i = 0; i < m_noiseEstimates.Length(); ++i )
               keywords << FITSHeaderKeyword( IsoString().Format( "NOISE%02d", i ),
                                              IsoString().Format( "%.4e", m_noiseEstimates[i] ),
                                              IsoString().Format( "Noise estimate, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEF%02d", i ),
                                              IsoString().Format( "%.3f", m_noiseFractions[i] ),
                                              IsoString().Format( "Fraction of noise pixels, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEL%02d", i ),
                                              IsoString().Format( "%.6e", m_noiseScaleLow[i] ),
                                              IsoString().Format( "Noise scaling factor, low pixels, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEH%02d", i ),
                                              IsoString().Format( "%.6e", m_noiseScaleHigh[i] ),
                                              IsoString().Format( "Noise scaling factor, high pixels, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEA%02d", i ),
                                              IsoString( m_noiseAlgorithms[i] ),
                                              IsoString().Format( "Noise evaluation algorithm, channel #%d", i ) );
         }

         if ( m_outputPedestal != 0 )
         {
            keywords << FITSHeaderKeyword( "HISTORY",
                                           IsoString(),
                                           IsoString().Format( "ImageCalibration.outputPedestal: %d", m_outputPedestal ) )
                     << FITSHeaderKeyword( "PEDESTAL",
                                           IsoString( m_outputPedestal ),
                                           "Value in DN added to enforce positivity" );
         }

         outputFile.WriteFITSKeywords( keywords );
      }
      else
      {
         console.WarningLn( "** Warning: The output format cannot store FITS header keywords - calibration metadata not embedded" );
      }

      /*
       * Preserve an existing ICC profile if possible.
       */
      if ( m_outputData.profile.IsProfile() )
         if ( outputFormat.CanStoreICCProfiles() )
            outputFile.WriteICCProfile( m_outputData.profile );
         else
            console.WarningLn( "** Warning: The output format cannot store color profiles - original ICC profile not embedded" );

      Module->ProcessEvents();

      /*
       * Write the output image and close the output stream.
       */
      {
         static Mutex mutex;
         static AtomicInt count;
         volatile AutoLockCounter lock( mutex, count, m_data.instance->m_maxFileWriteThreads );
         m_target->ResetSelections();
         if ( !outputFile.WriteImage( m_target ) || !outputFile.Close() )
            throw CaughtException();
      }
   }

   /*
    * * Signal evaluation by PSF photometry.
    *
    * * Estimation of the standard deviation of the noise, assuming a Gaussian
    *   noise distribution:
    *
    * - Use MRS noise evaluation when the algorithm converges for 4 >= J >= 2
    *
    * - Use k-sigma noise evaluation when either MRS does not converge, or when
    *   the set of noise pixels represents less than a 1% of the image area.
    *
    * - Automatically iterate to find the highest wavelet layer where noise can
    *   be successfully evaluated, in the [1,3] range.
    */
   void EvaluateSignalAndNoise( Vector& psfTotalFluxEstimates, Vector& psfTotalPowerFluxEstimates,
                                Vector& psfTotalMeanFluxEstimates, Vector& psfTotalMeanPowerFluxEstimates,
                                Vector& psfMStarEstimates, Vector& psfNStarEstimates, IVector& psfCounts,
                                Vector& noiseEstimates, Vector& noiseFractions,
                                Vector& noiseScaleLow, Vector& noiseScaleHigh, StringList& noiseAlgorithms,
                                const ImageVariant& target, const IsoString& cfaPattern = IsoString() ) const
   {
      volatile AutoStatusCallbackRestorer saveStatus( target.Status() );
      MuteStatus status;
      target.SetStatusCallback( &status );
      target.Status().DisableInitialization();

      target.ResetSelections();

      if ( !cfaPattern.IsEmpty() )
      {
         ImageVariant evaluationTarget;
         evaluationTarget.CopyImage( target );
         IntegerResample( -DownsamplingFactorForCFAPattern( cfaPattern ) ) >> evaluationTarget;
         EvaluateSignalAndNoise( psfTotalFluxEstimates, psfTotalPowerFluxEstimates,
                                 psfTotalMeanFluxEstimates, psfTotalMeanPowerFluxEstimates,
                                 psfMStarEstimates, psfNStarEstimates, psfCounts,
                                 noiseEstimates, noiseFractions,
                                 noiseScaleLow, noiseScaleHigh, noiseAlgorithms,
                                 evaluationTarget );
         return;
      }

      /*
       * Signal estimation by PSF photometry
       */
      if ( m_data.instance->p_evaluateSignal )
      {
         psfTotalFluxEstimates = Vector( 0.0, target.NumberOfChannels() );
         psfTotalPowerFluxEstimates = Vector( 0.0, target.NumberOfChannels() );
         psfTotalMeanFluxEstimates = Vector( 0.0, target.NumberOfChannels() );
         psfTotalMeanPowerFluxEstimates = Vector( 0.0, target.NumberOfChannels() );
         psfMStarEstimates = Vector( 0.0, target.NumberOfChannels() );
         psfNStarEstimates = Vector( 0.0, target.NumberOfChannels() );
         psfCounts = IVector( 0, target.NumberOfChannels() );

         PSFSignalEstimator E;
         E.Detector().SetStructureLayers( m_data.instance->p_structureLayers );
         E.Detector().SetNoiseLayers( m_data.instance->p_noiseLayers );
         E.Detector().SetHotPixelFilterRadius( m_data.instance->p_hotPixelFilterRadius );
         E.Detector().SetNoiseReductionFilterRadius( m_data.instance->p_noiseReductionFilterRadius );
         E.Detector().SetMinStructureSize( m_data.instance->p_minStructureSize );
         E.Detector().EnableClusteredSources();
         E.Detector().DisableLocalMaximaDetection();
         E.SetPSFType( ICPSFType::ToPSFFunction( m_data.instance->p_psfType ) );
         E.SetSaturationThreshold( m_data.instance->p_saturationThreshold );
         E.EnableRelativeSaturation( m_data.instance->p_saturationRelative );
         E.SetGrowthFactorForFluxMeasurement( m_data.instance->p_psfGrowth );
         E.SetMaxStars( m_data.instance->p_maxStars );
         E.EnableParallelProcessing( m_maxSubThreads > 1, m_maxSubThreads );

         for ( int c = 0; c < target.NumberOfChannels(); ++c )
         {
            target.SelectChannel( c );

            PSFSignalEstimator::Estimates e = E( target );

            psfTotalFluxEstimates[c] = e.totalFlux;
            psfTotalPowerFluxEstimates[c] = e.totalPowerFlux;
            psfTotalMeanFluxEstimates[c] = e.totalMeanFlux;
            psfTotalMeanPowerFluxEstimates[c] = e.totalMeanPowerFlux;
            psfMStarEstimates[c] = e.MStar;
            psfNStarEstimates[c] = e.NStar;
            psfCounts[c] = e.count;
         }
      }

      /*
       * Noise evaluation
       */
      if ( m_data.instance->p_evaluateNoise )
      {
         noiseEstimates = Vector( 0.0, target.NumberOfChannels() );
         noiseFractions = Vector( 0.0, target.NumberOfChannels() );
         noiseScaleLow = Vector( 0.0, target.NumberOfChannels() );
         noiseScaleHigh = Vector( 0.0, target.NumberOfChannels() );
         noiseAlgorithms = StringList( target.NumberOfChannels(), String() );

         ATrousWaveletTransform::WaveletScalingFunction H;
         if ( m_data.instance->p_noiseEvaluationAlgorithm == ICNoiseEvaluationAlgorithm::KSigma ||
              m_data.instance->p_noiseEvaluationAlgorithm == ICNoiseEvaluationAlgorithm::MRS )
         {
            if ( SeparableConvolution::FasterThanNonseparableFilterSize( m_maxSubThreads ) > 5 )
               H.Set( KernelFilter( g_5x5B3Spline, 5 ) );
            else
               H.Set( SeparableFilter( g_5x5B3Spline_hv, g_5x5B3Spline_hv, 5 ) );
         }

         for ( int c = 0; c < target.NumberOfChannels(); ++c )
         {
            target.SelectChannel( c );

            /*
             * Noise scaling factors
             */
            {
               const double clipLow = 2.0/65535;
               const double clipHigh = 1.0 - 2.0/65535;

               target.SetRangeClipping( clipLow, clipHigh );
               double center = target.Median();

               target.SetRangeClipping( clipLow, center );
               target.SetRangeClipping( Max( clipLow, center - 4*target.StdDev() ), center );
               target.SetRangeClipping( Max( clipLow, center - 4*target.StdDev() ), center );
               noiseScaleLow[c] = target.StdDev();

               target.SetRangeClipping( center, clipHigh );
               target.SetRangeClipping( center, Min( center + 4*target.StdDev(), clipHigh ) );
               target.SetRangeClipping( center, Min( center + 4*target.StdDev(), clipHigh ) );
               noiseScaleHigh[c] = target.StdDev();

               target.ResetRangeClipping();
            }

            /*
             * Noise estimates
             */
            switch ( m_data.instance->p_noiseEvaluationAlgorithm )
            {
            case ICNoiseEvaluationAlgorithm::KSigma:
               {
                  ATrousWaveletTransform W( H, 1 );
                  W.EnableParallelProcessing( m_maxSubThreads > 1, m_maxSubThreads );
                  W << target;
                  size_type N;
                  noiseEstimates[c] = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/g_5x5B3Spline_kj[0];
                  noiseFractions[c] = float( double( N )/target.NumberOfPixels() );
                  noiseAlgorithms[c] = "K-Sigma";
               }
               break;

            case ICNoiseEvaluationAlgorithm::MRS:
               {
                  double s0 = 0, f0 = 0;
                  for ( int n = 4;; )
                  {
                     Module->ProcessEvents();

                     ATrousWaveletTransform W( H, n );
                     W.EnableParallelProcessing( m_maxSubThreads > 1, m_maxSubThreads );
                     W << target;

                     size_type N;
                     if ( n == 4 )
                     {
                        s0 = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/g_5x5B3Spline_kj[0];
                        f0 = double( N )/target.NumberOfPixels();
                     }
                     noiseEstimates[c] = W.NoiseMRS( target, g_5x5B3Spline_kj, s0, 3, &N );
                     noiseFractions[c] = double( N )/target.NumberOfPixels();

                     if ( noiseEstimates[c] > 0 && noiseFractions[c] >= 0.01 )
                     {
                        noiseAlgorithms[c] = "MRS";
                        break;
                     }

                     if ( --n == 1 )
                     {
                        noiseEstimates[c] = s0;
                        noiseFractions[c] = f0;
                        noiseAlgorithms[c] = "K-Sigma";
                        break;
                     }
                  }
               }
               break;

            default:
            case ICNoiseEvaluationAlgorithm::NStar:
               if ( m_data.instance->p_evaluateSignal )
                  noiseEstimates[c] = psfNStarEstimates[c];
               else
               {
                  Array<float> R = PSFSignalEstimator::LocalBackgroundResidual( target );
                  noiseEstimates[c] = PSFSignalEstimator::NStar( R );
               }
               noiseFractions[c] = 1.0;
               noiseAlgorithms[c] = "N-star";
               break;
            }
         }
      }

      target.ResetSelections();
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Routine to enforce validity of image geometries.
 */
void ImageCalibrationInstance::ValidateImageGeometry( const Image& image, bool uncalibrated )
{
   if ( uncalibrated )
   {
      if ( m_geometry.IsRect() )
      {
         if ( image.Bounds() != m_geometry )
            throw Error( "Incompatible image geometry" );
      }
      else
      {
         m_geometry = image.Bounds();

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
         if ( image.Bounds() != m_calibratedGeometry )
            throw Error( "Incompatible image geometry" );
      }
      else
      {
         m_calibratedGeometry = image.Bounds();

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
void ImageCalibrationInstance::SubtractPedestal( Image& image, FileFormatInstance& file )
{
   Console console;
   switch ( p_pedestalMode )
   {
   case ICPedestalMode::Literal:
      if ( p_pedestal != 0 )
      {
         console.NoteLn( String().Format( "* Subtracting pedestal: %d DN", p_pedestal ) );
         image.Apply( p_pedestal/65535.0, ImageOp::Sub );
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
            image.Apply( d/65535.0, ImageOp::Sub );
         }
      }
      break;
   }
}

// ----------------------------------------------------------------------------

/*
 * Read a source calibration frame.
 */
Image* ImageCalibrationInstance::LoadMasterCalibrationFrame( const String& filePath, bool willCalibrate, IsoString* cfaPattern )
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
    * bias, dark or flat frame. Show a warning only if we don't recognize some
    * of the additional images.
    */
   if ( images.Length() > 1 )
   {
      int unknown = 0;
      for ( size_type i = 1; i < images.Length(); ++i )
         if ( images[i].id != "rejection_low" && images[i].id != "rejection_high" && images[i].id != "slope" )
            ++unknown;
      if ( unknown )
         console.WriteLn( String().Format( "<end><cbr>* Ignoring %d unknown additional image(s) in master calibration frame.", unknown ) );
   }

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
   Image* image = new Image( (void*)0, 0, 0 ); // new shared image, 32-bit floating point.
   if ( !file.ReadImage( *image ) )
      throw CaughtException();

   /*
    * Enforce consistency of frame dimensions.
    */
   ValidateImageGeometry( *image, willCalibrate );

   /*
    * Optional pedestal subtraction.
    */
   SubtractPedestal( *image, file );

   /*
    * Close the input stream.
    */
   if ( !file.Close() )
      throw CaughtException();

   return image;
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInstance::ExecuteGlobal()
{
   Console console;

   /*
    * Reset output data.
    */
   o_output = Array<OutputData>( p_targetFrames.Length() );

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

      StringList fileNames;
      for ( const auto& target : p_targetFrames )
         if ( target.enabled )
         {
            if ( !File::Exists( target.path ) )
               throw Error( "No such file exists on the local filesystem: " + target.path );
            fileNames << File::ExtractNameAndSuffix( target.path );
         }
      fileNames.Sort();
      for ( size_type i = 1; i < fileNames.Length(); ++i )
         if ( fileNames[i].CompareIC( fileNames[i-1] ) == 0 )
         {
            if ( p_overwriteExistingFiles )
               throw Error( "The target frames list contains duplicate file names (case-insensitive). "
                            "This is not allowed when the 'Overwrite existing files' option is enabled." );
            console.WarningLn( "<end><cbr><br>** Warning: The target frames list contains duplicate file names (case-insensitive)." );
            break;
         }
   }

   /*
    * Setup high-level parallelism.
    */
   m_maxFileReadThreads = p_maxFileReadThreads;
   if ( m_maxFileReadThreads < 1 )
      m_maxFileReadThreads = Max( 1, PixInsightSettings::GlobalInteger( "Process/MaxFileReadThreads" ) );
   m_maxFileWriteThreads = p_maxFileWriteThreads;
   if ( m_maxFileWriteThreads < 1 )
      m_maxFileWriteThreads = Max( 1, PixInsightSettings::GlobalInteger( "Process/MaxFileWriteThreads" ) );

   /*
    * Allow the user to abort the calibration process.
    */
   console.EnableAbort();

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
         bias = LoadMasterCalibrationFrame( p_masterBias.path, p_calibrateBias );
      }

      if ( p_masterDark.enabled )
      {
         console.NoteLn( "<end><cbr><br>* Loading master dark frame: <raw>" + p_masterDark.path + "</raw>" );
         dark = LoadMasterCalibrationFrame( p_masterDark.path, p_calibrateDark, p_enableCFA ? &darkCFAPattern : nullptr );
      }

      if ( p_masterFlat.enabled )
      {
         console.NoteLn( "<end><cbr><br>* Loading master flat frame: <raw>" + p_masterFlat.path + "</raw>" );
         flat = LoadMasterCalibrationFrame( p_masterFlat.path, p_calibrateFlat, p_enableCFA ? &flatCFAPattern : nullptr );
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
               console.WriteLn( String().Format( "<end><cbr>ch %d : Td = %.8f (%llu px = %.3f%%)",
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
                        console.WriteLn( String().Format( "<end><cbr>ch %d : k = %.3f", c, K[c] ) );
                        if ( K[c] <= NO_DARK_CORRELATION )
                           console.WarningLn( String().Format( "** Warning: No correlation between "
                                             "the master dark and master flat frames (channel %d).", c ) );
                     }
                  }
                  else
                     K = FVector( 1.0F, flat->NumberOfChannels() );

               Module->ProcessEvents();
               Calibrate( *flat, bias, dark, K, nullptr/*flat*/ );
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
               console.WriteLn( String().Format( "ch %d : f = %.6f", c, s ) );
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
               console.WriteLn( String().Format( "ch %d : f = %.6f", c, s ) );
               flatScalingFactors[c] = s;
            }
         }
      }
   } // if bias or dark or flat

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

   int succeeded = 0;
   int failed = 0;
   int skipped = 0;

   Array<size_type> pendingItems;
   for ( size_type i = 0; i < p_targetFrames.Length(); ++i )
      if ( p_targetFrames[i].enabled )
         pendingItems << i;
      else
      {
         console.NoteLn( "* Skipping disabled target: <raw>" + p_targetFrames[i].path + "</raw>" );
         ++skipped;
      }

   /*
    * Begin light frame calibration process.
    */
   console.NoteLn( String().Format( "<end><cbr><br>* Calibration of %u target frames.", pendingItems.Length() ) );

   Module->ProcessEvents();

   if ( p_useFileThreads && pendingItems.Length() > 1 )
   {
      /*
       * Running threads list. Note that IndirectArray<> initializes all of its
       * contained pointers to nullptr.
       */
      int numberOfThreadsAvailable = RoundInt( Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 ) * p_fileThreadOverload );
      int numberOfThreads = Min( numberOfThreadsAvailable, int( pendingItems.Length() ) );
      int numberOfSubthreads = Max( 1, RoundInt( double( numberOfThreadsAvailable - numberOfThreads )/numberOfThreads ) );
      thread_list runningThreads( numberOfThreads );

      console.NoteLn( String().Format( "<end><br>* Using %d worker threads.", numberOfThreads ) );
      console.Flush();

      try
      {
         /*
          * Thread execution loop.
          */
         for ( ;; )
         {
            try
            {
               int running = 0;
               for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
               {
                  Module->ProcessEvents();
                  if ( console.AbortRequested() )
                     throw ProcessAborted();

                  if ( *i != nullptr )
                  {
                     /*
                      * Check for a running thread
                      */
                     if ( !(*i)->Wait( 150 ) )
                     {
                        ++running;
                        continue;
                     }

                     /*
                      * A thread has finished execution
                      */
                     if ( succeeded+failed == 0 )
                        console.WriteLn();
                     (*i)->FlushConsoleOutputText();
                     console.WriteLn();
                     String errorInfo;
                     if ( (*i)->Succeeded() )
                        (*i)->GetOutputData( o_output[(*i)->Index()] );
                     else
                        errorInfo = (*i)->ErrorInfo();

                     /*
                      * N.B.: IndirectArray<>::Delete() sets to zero the
                      * pointer pointed to by the iterator, but does not remove
                      * the array element.
                      */
                     runningThreads.Delete( i );

                     if ( !errorInfo.IsEmpty() )
                        throw Error( errorInfo );

                     ++succeeded;
                  }

                  /*
                   * If there are pending items, try to create a new worker
                   * thread and fire it.
                   */
                  if ( !pendingItems.IsEmpty() )
                  {
                     *i = new CalibrationThread( threadData, *pendingItems, numberOfSubthreads );
                     pendingItems.Remove( pendingItems.Begin() );
                     size_type threadIndex = i - runningThreads.Begin();
                     console.NoteLn( String().Format( "<end><cbr>[%03u] <raw>", threadIndex ) + (*i)->TargetFilePath() + "</raw>" );
                     (*i)->Start( ThreadPriority::DefaultMax, threadIndex );
                     ++running;
                     if ( pendingItems.IsEmpty() )
                        console.NoteLn( "<br>* Waiting for running tasks to terminate...<br>" );
                     else if ( succeeded+failed > 0 )
                        console.WriteLn();
                  }
               }

               if ( !running )
                  break;
            }
            catch ( ProcessAborted& )
            {
               throw;
            }
            catch ( ... )
            {
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               ++failed;
               try
               {
                  throw;
               }
               ERROR_HANDLER

               ApplyErrorPolicy();
            }
         }
      }
      catch ( ... )
      {
         console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate..." );
         for ( CalibrationThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Abort();
         for ( CalibrationThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Wait();
         runningThreads.Destroy();
         throw;
      }
   }
   else // !p_useFileThreads || pendingItems.Length() < 2
   {
      for ( size_type itemIndex : pendingItems )
      {
         try
         {
            CalibrationThread thread( threadData, itemIndex, PCL_MAX_PROCESSORS );
            thread.Run();
            thread.GetOutputData( o_output[itemIndex] );
            ++succeeded;
         }
         catch ( ProcessAborted& )
         {
            throw;
         }
         catch ( ... )
         {
            if ( console.AbortRequested() )
               throw ProcessAborted();

            ++failed;
            try
            {
               throw;
            }
            ERROR_HANDLER

            ApplyErrorPolicy();
         }
      }
   }

   Module->ProcessEvents();

   if ( succeeded == 0 )
   {
      if ( failed == 0 )
         throw Error( "No images were calibrated: Empty target frames list? No enabled target frames?" );
      throw Error( "No image could be calibrated." );
   }

   console.NoteLn( String().Format( "<end><cbr><br>===== ImageCalibration: %u succeeded, %u failed, %u skipped =====",
                                    succeeded, failed, skipped ) );
   return true;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInstance::ApplyErrorPolicy()
{
   Console console;
   console.ResetStatus();
   console.EnableAbort();

   console.Note( "<end><cbr><br>* Applying error policy: " );

   switch ( p_onError )
   {
   default: // ?
   case ICOnError::Continue:
      console.NoteLn( "Continue on error." );
      break;

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

         console.NoteLn( "* Error ignored as per user request." );
      }
      break;
   }
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

   if ( p == TheICEvaluateSignalParameter )
      return &p_evaluateSignal;
   if ( p == TheICStructureLayersParameter )
      return &p_structureLayers;
   if ( p == TheICSaturationThresholdParameter )
      return &p_saturationThreshold;
   if ( p == TheICSaturationRelativeParameter )
      return &p_saturationRelative;
   if ( p == TheICNoiseLayersParameter )
      return &p_noiseLayers;
   if ( p == TheICHotPixelFilterRadiusParameter )
      return &p_hotPixelFilterRadius;
   if ( p == TheICNoiseReductionFilterRadiusParameter )
      return &p_noiseReductionFilterRadius;
   if ( p == TheICMinStructureSizeParameter )
      return &p_minStructureSize;
   if ( p == TheICPSFTypeParameter )
      return &p_psfType;
   if ( p == TheICPSFGrowthParameter )
      return &p_psfGrowth;
   if ( p == TheICMaxStarsParameter )
      return &p_maxStars;

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
   if ( p == TheICOutputPedestalModeParameter )
      return &p_outputPedestalMode;
   if ( p == TheICAutoPedestalLimitParameter )
      return &p_autoPedestalLimit;
   if ( p == TheICOverwriteExistingFilesParameter )
      return &p_overwriteExistingFiles;
   if ( p == TheICOnErrorParameter )
      return &p_onError;
   if ( p == TheICNoGUIMessagesParameter )
      return &p_noGUIMessages;

   if ( p == TheICUseFileThreadsParameter )
      return &p_useFileThreads;
   if ( p == TheICFileThreadOverloadParameter )
      return &p_fileThreadOverload;
   if ( p == TheICMaxFileReadThreadsParameter )
      return &p_maxFileReadThreads;
   if ( p == TheICMaxFileWriteThreadsParameter )
      return &p_maxFileWriteThreads;

   if ( p == TheICOutputFilePathParameter )
      return o_output[tableRow].outputFilePath.Begin();

   if ( p == TheICDarkScalingFactorRKParameter )
      return o_output[tableRow].darkScalingFactors.At( 0 );
   if ( p == TheICDarkScalingFactorGParameter )
      return o_output[tableRow].darkScalingFactors.At( 1 );
   if ( p == TheICDarkScalingFactorBParameter )
      return o_output[tableRow].darkScalingFactors.At( 2 );

   if ( p == TheICPSFTotalFluxEstimateRKParameter )
      return o_output[tableRow].psfTotalFluxEstimates.At( 0 );
   if ( p == TheICPSFTotalFluxEstimateGParameter )
      return o_output[tableRow].psfTotalFluxEstimates.At( 1 );
   if ( p == TheICPSFTotalFluxEstimateBParameter )
      return o_output[tableRow].psfTotalFluxEstimates.At( 2 );

   if ( p == TheICPSFTotalPowerFluxEstimateRKParameter )
      return o_output[tableRow].psfTotalPowerFluxEstimates.At( 0 );
   if ( p == TheICPSFTotalPowerFluxEstimateGParameter )
      return o_output[tableRow].psfTotalPowerFluxEstimates.At( 1 );
   if ( p == TheICPSFTotalPowerFluxEstimateBParameter )
      return o_output[tableRow].psfTotalPowerFluxEstimates.At( 2 );

   if ( p == TheICPSFTotalMeanFluxEstimateRKParameter )
      return o_output[tableRow].psfTotalMeanFluxEstimates.At( 0 );
   if ( p == TheICPSFTotalMeanFluxEstimateGParameter )
      return o_output[tableRow].psfTotalMeanFluxEstimates.At( 1 );
   if ( p == TheICPSFTotalMeanFluxEstimateBParameter )
      return o_output[tableRow].psfTotalMeanFluxEstimates.At( 2 );

   if ( p == TheICPSFTotalMeanPowerFluxEstimateRKParameter )
      return o_output[tableRow].psfTotalMeanPowerFluxEstimates.At( 0 );
   if ( p == TheICPSFTotalMeanPowerFluxEstimateGParameter )
      return o_output[tableRow].psfTotalMeanPowerFluxEstimates.At( 1 );
   if ( p == TheICPSFTotalMeanPowerFluxEstimateBParameter )
      return o_output[tableRow].psfTotalMeanPowerFluxEstimates.At( 2 );

   if ( p == TheICPSFMStarEstimateRKParameter )
      return o_output[tableRow].psfMStarEstimates.At( 0 );
   if ( p == TheICPSFMStarEstimateGParameter )
      return o_output[tableRow].psfMStarEstimates.At( 1 );
   if ( p == TheICPSFMStarEstimateBParameter )
      return o_output[tableRow].psfMStarEstimates.At( 2 );

   if ( p == TheICPSFNStarEstimateRKParameter )
      return o_output[tableRow].psfNStarEstimates.At( 0 );
   if ( p == TheICPSFNStarEstimateGParameter )
      return o_output[tableRow].psfNStarEstimates.At( 1 );
   if ( p == TheICPSFNStarEstimateBParameter )
      return o_output[tableRow].psfNStarEstimates.At( 2 );

   if ( p == TheICPSFCountRKParameter )
      return o_output[tableRow].psfCounts.At( 0 );
   if ( p == TheICPSFCountGParameter )
      return o_output[tableRow].psfCounts.At( 1 );
   if ( p == TheICPSFCountBParameter )
      return o_output[tableRow].psfCounts.At( 2 );

   if ( p == TheICNoiseEstimateRKParameter )
      return o_output[tableRow].noiseEstimates.At( 0 );
   if ( p == TheICNoiseEstimateGParameter )
      return o_output[tableRow].noiseEstimates.At( 1 );
   if ( p == TheICNoiseEstimateBParameter )
      return o_output[tableRow].noiseEstimates.At( 2 );

   if ( p == TheICNoiseScaleLowRKParameter )
      return o_output[tableRow].noiseScaleLow.At( 0 );
   if ( p == TheICNoiseScaleLowGParameter )
      return o_output[tableRow].noiseScaleLow.At( 1 );
   if ( p == TheICNoiseScaleLowBParameter )
      return o_output[tableRow].noiseScaleLow.At( 2 );

   if ( p == TheICNoiseScaleHighRKParameter )
      return o_output[tableRow].noiseScaleHigh.At( 0 );
   if ( p == TheICNoiseScaleHighGParameter )
      return o_output[tableRow].noiseScaleHigh.At( 1 );
   if ( p == TheICNoiseScaleHighBParameter )
      return o_output[tableRow].noiseScaleHigh.At( 2 );

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
// EOF ImageCalibrationInstance.cpp - Released 2022-11-21T14:47:17Z
