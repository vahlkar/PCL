//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 1.11.2
// ----------------------------------------------------------------------------
// DebayerInstance.cpp - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "DebayerInstance.h"
#include "DebayerParameters.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/AutoLock.h>
#include <pcl/AutoStatusCallbackRestorer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ICCProfile.h>
#include <pcl/IntegerResample.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/MuteStatus.h>
#include <pcl/SpinStatus.h>
#include <pcl/StandardStatus.h>
#include <pcl/Thread.h>
#include <pcl/Version.h>
#include <pcl/WordArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * 5x5 B3-spline wavelet scaling function used by the noise estimation routine.
 *
 * Kernel filter coefficients:
 *
 *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256,
 *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
 *   3.0/128, 3.0/32, 9.0/64,  3.0/32, 3.0/128,
 *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
 *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256
 *
 * Note that we use this scaling function as a separable filter (row and column
 * vectors) for performance reasons.
 */
// Separable filter coefficients
const float s_5x5B3Spline_hv[] = { 0.0625F, 0.25F, 0.375F, 0.25F, 0.0625F };
// Gaussian noise scaling factors
const float s_5x5B3Spline_kj[] =
{ 0.8907F, 0.2007F, 0.0856F, 0.0413F, 0.0205F, 0.0103F, 0.0052F, 0.0026F, 0.0013F, 0.0007F };

// ----------------------------------------------------------------------------

/*
 * PSF signal evaluation keyword prefixes.
 */
static const char* s_psfPrefixes[] = { "PSFFLX", "PSFFLP", "PSFMFL", "PSFMFP", "PSFMST", "PSFNST", "PSFSGN", "PSFSGTYP" };

// ----------------------------------------------------------------------------

static IsoString ValidFullId( const IsoString& id )
{
   IsoString validId( id );
   validId.ReplaceString( "->", "_" );
   return validId;
}

static IsoString ValidMethodId( const IsoString& id )
{
   IsoString validId( id );
   validId.DeleteChar( '-' );
   return validId;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DebayerInstance::DebayerInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_bayerPattern( DebayerBayerPatternParameter::Default )
   , p_debayerMethod( DebayerMethodParameter::Default )
   , p_fbddNoiseReduction( int32( TheDebayerFBDDNoiseReductionParameter->DefaultValue() ) )
   , p_showImages( TheDebayerShowImagesParameter->DefaultValue() )
   , p_cfaSourceFilePath( TheDebayerCFASourceFilePathParameter->DefaultValue() )
   , p_noGUIMessages( TheDebayerNoGUIMessagesParameter->DefaultValue() ) // ### DEPRECATED
   , p_evaluateNoise( TheDebayerEvaluateNoiseParameter->DefaultValue() )
   , p_noiseEvaluationAlgorithm( DebayerNoiseEvaluationAlgorithm::Default )
   , p_evaluateSignal( TheDebayerEvaluateSignalParameter->DefaultValue() )
   , p_structureLayers( TheDebayerStructureLayersParameter->DefaultValue() )
   , p_saturationThreshold( TheDebayerSaturationThresholdParameter->DefaultValue() )
   , p_saturationRelative( TheDebayerSaturationRelativeParameter->DefaultValue() )
   , p_noiseLayers( TheDebayerNoiseLayersParameter->DefaultValue() )
   , p_hotPixelFilterRadius( TheDebayerHotPixelFilterRadiusParameter->DefaultValue() )
   , p_noiseReductionFilterRadius( TheDebayerNoiseReductionFilterRadiusParameter->DefaultValue() )
   , p_minStructureSize( TheDebayerMinStructureSizeParameter->DefaultValue() )
   , p_psfType( DebayerPSFType::Default )
   , p_psfGrowth( TheDebayerPSFGrowthParameter->DefaultValue() )
   , p_maxStars( TheDebayerMaxStarsParameter->DefaultValue() )
   , p_inputHints( TheDebayerInputHintsParameter->DefaultValue() )
   , p_outputHints( TheDebayerOutputHintsParameter->DefaultValue() )
   , p_outputRGBImages( TheDebayerOutputRGBImagesParameter->DefaultValue() )
   , p_outputSeparateChannels( TheDebayerOutputSeparateChannelsParameter->DefaultValue() )
   , p_outputDirectory( TheDebayerOutputDirectoryParameter->DefaultValue() )
   , p_outputExtension( TheDebayerOutputExtensionParameter->DefaultValue() )
   , p_outputPrefix( TheDebayerOutputPrefixParameter->DefaultValue() )
   , p_outputPostfix( TheDebayerOutputPostfixParameter->DefaultValue() )
   , p_overwriteExistingFiles( TheDebayerOverwriteExistingFilesParameter->DefaultValue() )
   , p_onError( DebayerOnError::Default )
   , p_useFileThreads( TheDebayerUseFileThreadsParameter->DefaultValue() )
   , p_fileThreadOverload( TheDebayerFileThreadOverloadParameter->DefaultValue() )
   , p_maxFileReadThreads( int32( TheDebayerMaxFileReadThreadsParameter->DefaultValue() ) )
   , p_maxFileWriteThreads( int32( TheDebayerMaxFileWriteThreadsParameter->DefaultValue() ) )
   , p_memoryLoadControl( TheDebayerMemoryLoadControlParameter->DefaultValue() )
   , p_memoryLoadLimit( TheDebayerMemoryLoadLimitParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

DebayerInstance::DebayerInstance( const DebayerInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void DebayerInstance::Assign( const ProcessImplementation& p )
{
   const DebayerInstance* x = dynamic_cast<const DebayerInstance*>( &p );
   if ( x != nullptr )
   {
      p_bayerPattern                   = x->p_bayerPattern;
      p_debayerMethod                  = x->p_debayerMethod;
      p_fbddNoiseReduction             = x->p_fbddNoiseReduction;
      p_showImages                     = x->p_showImages;
      p_cfaSourceFilePath              = x->p_cfaSourceFilePath;
      p_targets                        = x->p_targets;
      p_noGUIMessages                  = x->p_noGUIMessages;
      p_evaluateNoise                  = x->p_evaluateNoise;
      p_noiseEvaluationAlgorithm       = x->p_noiseEvaluationAlgorithm;
      p_evaluateSignal                 = x->p_evaluateSignal;
      p_structureLayers                = x->p_structureLayers;
      p_saturationThreshold            = x->p_saturationThreshold;
      p_saturationRelative             = x->p_saturationRelative;
      p_noiseLayers                    = x->p_noiseLayers;
      p_hotPixelFilterRadius           = x->p_hotPixelFilterRadius;
      p_noiseReductionFilterRadius     = x->p_noiseReductionFilterRadius;
      p_minStructureSize               = x->p_minStructureSize;
      p_psfType                        = x->p_psfType;
      p_psfGrowth                      = x->p_psfGrowth;
      p_maxStars                       = x->p_maxStars;
      p_inputHints                     = x->p_inputHints;
      p_outputHints                    = x->p_outputHints;
      p_outputRGBImages                = x->p_outputRGBImages;
      p_outputSeparateChannels         = x->p_outputSeparateChannels;
      p_outputDirectory                = x->p_outputDirectory;
      p_outputExtension                = x->p_outputExtension;
      p_outputPrefix                   = x->p_outputPrefix;
      p_outputPostfix                  = x->p_outputPostfix;
      p_overwriteExistingFiles         = x->p_overwriteExistingFiles;
      p_onError                        = x->p_onError;
      p_useFileThreads                 = x->p_useFileThreads;
      p_fileThreadOverload             = x->p_fileThreadOverload;
      p_maxFileReadThreads             = x->p_maxFileReadThreads;
      p_maxFileWriteThreads            = x->p_maxFileWriteThreads;
      p_memoryLoadControl              = x->p_memoryLoadControl;
      p_memoryLoadLimit                = x->p_memoryLoadLimit;

      o_imageId                        = x->o_imageId;
      o_channelIds                     = x->o_channelIds;
      o_psfTotalFluxEstimates          = x->o_psfTotalFluxEstimates;
      o_psfTotalPowerFluxEstimates     = x->o_psfTotalPowerFluxEstimates;
      o_psfTotalMeanFluxEstimates      = x->o_psfTotalMeanFluxEstimates;
      o_psfTotalMeanPowerFluxEstimates = x->o_psfTotalMeanPowerFluxEstimates;
      o_psfMStarEstimates              = x->o_psfMStarEstimates;
      o_psfNStarEstimates              = x->o_psfNStarEstimates;
      o_psfCounts                      = x->o_psfCounts;
      o_noiseEstimates                 = x->o_noiseEstimates;
      o_noiseFractions                 = x->o_noiseFractions;
      o_noiseScaleLow                  = x->o_noiseScaleLow;
      o_noiseScaleHigh                 = x->o_noiseScaleHigh;
      o_noiseAlgorithms                = x->o_noiseAlgorithms;

      o_outputFileData                 = x->o_outputFileData;
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Fast color filter array (CFA) pixel sample selection.
 */
class CFAIndex
{
public:

   typedef GenericMatrix<bool>   cfa_channel_index;

   CFAIndex() = default;
   CFAIndex( const CFAIndex& ) = default;

   CFAIndex( const IsoString& pattern )
   {
      if ( pattern.IsEmpty() )
         throw Error( "Internal error: Empty CFA pattern." );
      m_size = int( Sqrt( pattern.Length() ) );
      if ( m_size < 2 )
         throw Error( "Internal error: Invalid CFA pattern '" + pattern + '\'' );
      if ( m_size*m_size != pattern.Length() )
         throw Error( "Internal error: Non-square CFA patterns are not supported: '" + pattern + '\'' );
      for ( int c = 0; c < 3; ++c )
      {
         m_index[c] = cfa_channel_index( m_size, m_size );
         IsoString::const_iterator p = pattern.Begin();
         for ( int i = 0; i < m_size; ++i )
            for ( int j = 0; j < m_size; ++j )
               m_index[c][i][j] = *p++ == "RGB"[c];
      }
   }

   int Size() const
   {
      return m_size;
   }

   bool operator ()( int x, int y, int c ) const
   {
      return m_index[c][y % m_size][x % m_size];
   }

private:

   int               m_size = 0;
   cfa_channel_index m_index[ 3 ]; // RGB
};

class SignalAndNoiseEvaluationThread : public Thread
{
public:

   double    psfTotalFluxEstimate = 0;
   double    psfTotalPowerFluxEstimate = 0;
   double    psfTotalMeanFluxEstimate = 0;
   double    psfTotalMeanPowerFluxEstimate = 0;
   double    psfMStarEstimate = 0;
   double    psfNStarEstimate = 0;
   int       psfCount = 0;

   double    noiseEstimate = 0;
   double    noiseFraction = 0;
   double    noiseScaleLow = 0;
   double    noiseScaleHigh = 0;
   IsoString noiseAlgorithm;

   SignalAndNoiseEvaluationThread( const ImageVariant& image, int channel,
                                   const DebayerInstance& instance,
                                   const IsoString& cfaPattern, int numberOfSubthreads )
      : m_instance( instance )
      , m_numberOfSubthreads( numberOfSubthreads )
   {
      /*
       * Extract one CFA component as a single-channel image.
       */
      CFAIndex index( cfaPattern );
      int n = index.Size();
      int w = image.Width();
      int h = image.Height();
      int wn = w / n;
      int hn = h / n;
      m_image.AllocateData( wn, hn, 1 );
      Image::sample_iterator it( m_image );
      for ( int y = 0; y < hn; ++y )
         for ( int x = 0; x < wn; ++x, ++it )
         {
            double f = 0;
            int m = 0;
            for ( int i = y*n, i1 = i + n; i < i1; ++i )
               for ( int j = x*n, j1 = j + n; j < j1; ++j )
                  if ( index( i, j, channel ) )
                  {
                     f += image( j, i );
                     ++m;
                  }
            *it = f/m;
         }
   }

   void Run() override
   {
      volatile AutoStatusCallbackRestorer saveStatus( m_image.Status() );
      MuteStatus status;
      m_image.SetStatusCallback( &status );
      m_image.Status().DisableInitialization();

      /*
       * Signal estimation
       */
      if ( m_instance.p_evaluateSignal )
      {
         PSFSignalEstimator E;
         E.Detector().SetStructureLayers( m_instance.p_structureLayers );
         E.Detector().SetNoiseLayers( m_instance.p_noiseLayers );
         E.Detector().SetHotPixelFilterRadius( m_instance.p_hotPixelFilterRadius );
         E.Detector().SetNoiseReductionFilterRadius( m_instance.p_noiseReductionFilterRadius );
         E.Detector().SetMinStructureSize( m_instance.p_minStructureSize );
         E.Detector().EnableClusteredSources();
         E.Detector().DisableLocalMaximaDetection();
         E.SetPSFType( DebayerPSFType::ToPSFFunction( m_instance.p_psfType ) );
         E.SetSaturationThreshold( m_instance.p_saturationThreshold );
         E.EnableRelativeSaturation( m_instance.p_saturationRelative );
         E.SetGrowthFactorForFluxMeasurement( m_instance.p_psfGrowth );
         E.SetMaxStars( m_instance.p_maxStars );
         E.EnableParallelProcessing( m_numberOfSubthreads > 1, m_numberOfSubthreads );

         PSFSignalEstimator::Estimates e = E( ImageVariant( &m_image ) );
         psfTotalFluxEstimate = e.totalFlux;
         psfTotalPowerFluxEstimate = e.totalPowerFlux;
         psfTotalMeanFluxEstimate = e.totalMeanFlux;
         psfTotalMeanPowerFluxEstimate = e.totalMeanPowerFlux;
         psfMStarEstimate = e.MStar;
         psfNStarEstimate = e.NStar;
         psfCount = e.count;
      }

      if ( m_instance.p_evaluateNoise )
      {
         /*
          * Noise scaling factors
          */
         {
            const double clipLow = 2.0/65535;
            const double clipHigh = 1.0 - 2.0/65535;

            m_image.SetRangeClipping( clipLow, clipHigh );
            double center = m_image.Median();

            m_image.SetRangeClipping( clipLow, center );
            m_image.SetRangeClipping( Max( clipLow, center - 4*m_image.StdDev() ), center );
            m_image.SetRangeClipping( Max( clipLow, center - 4*m_image.StdDev() ), center );
            noiseScaleLow = m_image.StdDev();

            m_image.SetRangeClipping( center, clipHigh );
            m_image.SetRangeClipping( center, Min( center + 4*m_image.StdDev(), clipHigh ) );
            m_image.SetRangeClipping( center, Min( center + 4*m_image.StdDev(), clipHigh ) );
            noiseScaleHigh = m_image.StdDev();

            m_image.ResetRangeClipping();
         }

         /*
          * Noise estimates
          */
         SeparableFilter H( s_5x5B3Spline_hv, s_5x5B3Spline_hv, 5 );

         switch ( m_instance.p_noiseEvaluationAlgorithm )
         {
         case DebayerNoiseEvaluationAlgorithm::KSigma:
            {
               ATrousWaveletTransform W( H, 1 );
               W.EnableParallelProcessing( m_numberOfSubthreads > 1, m_numberOfSubthreads );
               W << m_image;
               size_type N;
               noiseEstimate = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/s_5x5B3Spline_kj[0];
               noiseFraction = double( N )/m_image.NumberOfPixels();
               noiseAlgorithm = "K-Sigma";
            }
            break;

         case DebayerNoiseEvaluationAlgorithm::MRS:
            {
               double s0 = 0, f0 = 0;
               for ( int n = 4; ; )
               {
                  ATrousWaveletTransform W( H, n );
                  W.EnableParallelProcessing( m_numberOfSubthreads > 1, m_numberOfSubthreads );
                  W << m_image;

                  size_type N;
                  if ( n == 4 )
                  {
                     s0 = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/s_5x5B3Spline_kj[0];
                     f0 = double( N )/m_image.NumberOfPixels();
                  }
                  noiseEstimate = W.NoiseMRS( ImageVariant( &m_image ), s_5x5B3Spline_kj, s0, 3, &N );
                  noiseFraction = double( N )/m_image.NumberOfPixels();

                  if ( noiseEstimate > 0 && noiseFraction >= 0.01F )
                  {
                     noiseAlgorithm = "MRS";
                     break;
                  }

                  if ( --n == 1 )
                  {
                     noiseEstimate = s0;
                     noiseFraction = f0;
                     noiseAlgorithm = "K-Sigma";
                     break;
                  }
               }
            }
            break;

         default:
         case DebayerNoiseEvaluationAlgorithm::NStar:
            if ( m_instance.p_evaluateSignal )
               noiseEstimate = psfMStarEstimate;
            else
            {
               Array<float> R = PSFSignalEstimator::LocalBackgroundResidual( ImageVariant( &m_image ) );
               noiseEstimate = PSFSignalEstimator::NStar( R );
            }
            noiseFraction = 1.0;
            noiseAlgorithm = "N-star";
            break;
         }
      }
   }

private:

         Image            m_image;
   const DebayerInstance& m_instance;
         int              m_numberOfSubthreads;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct OutputFileData
{
   FileFormat*      format = nullptr;  // the file format of retrieved data
   const void*      fsData = nullptr;  // format-specific data
   ImageOptions     options;           // currently used for resolution only
   PropertyArray    properties;        // image properties
   FITSKeywordArray keywords;          // FITS keywords
   ICCProfile       profile;           // ICC profile

   OutputFileData() = default;

   OutputFileData( FileFormatInstance& file, const ImageOptions& o )
      : options( o )
   {
      format = new FileFormat( file.Format() );

      if ( format->UsesFormatSpecificData() )
         fsData = file.FormatSpecificData();

      if ( format->CanStoreImageProperties() )
         properties = file.ReadImageProperties();

      if ( format->CanStoreKeywords() )
         file.ReadFITSKeywords( keywords );

      if ( format->CanStoreICCProfiles() )
         file.ReadICCProfile( profile );
   }

   OutputFileData( const OutputFileData& ) = delete;

   OutputFileData( OutputFileData&& x )
      : format( x.format )
      , fsData( x.fsData )
      , options( std::move( x.options ) )
      , properties( std::move( x.properties ) )
      , keywords( std::move( x.keywords ) )
      , profile( std::move( x.profile ) )
   {
      x.format = nullptr;
      x.fsData = nullptr;
   }

   ~OutputFileData()
   {
      if ( format != nullptr )
      {
         if ( fsData != nullptr )
            format->DisposeFormatSpecificData( const_cast<void*>( fsData ) ), fsData = nullptr;
         delete format, format = nullptr;
      }
   }

   OutputFileData& operator =( const OutputFileData& ) = delete;

   OutputFileData& operator =( OutputFileData&& x )
   {
      format = x.format; x.format = nullptr;
      fsData = x.fsData; x.fsData = nullptr;
      options = std::move( x.options );
      properties = std::move( x.properties );
      keywords = std::move( x.keywords );
      profile = std::move( x.profile );
      return *this;
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static void BayerPatternToColorIndices( int colors[ 2 ][ 2 ], int bayerPattern )
{
   switch ( bayerPattern )
   {
   default:
   case DebayerBayerPatternParameter::RGGB:
      colors[0][0] = 0;
      colors[0][1] = 1;
      colors[1][0] = 1;
      colors[1][1] = 2;
      break;
   case DebayerBayerPatternParameter::BGGR:
      colors[0][0] = 2;
      colors[0][1] = 1;
      colors[1][0] = 1;
      colors[1][1] = 0;
      break;
   case DebayerBayerPatternParameter::GBRG:
      colors[0][0] = 1;
      colors[0][1] = 2;
      colors[1][0] = 0;
      colors[1][1] = 1;
      break;
   case DebayerBayerPatternParameter::GRBG:
      colors[0][0] = 1;
      colors[0][1] = 0;
      colors[1][0] = 2;
      colors[1][1] = 1;
      break;
   case DebayerBayerPatternParameter::GRGB:
      colors[0][0] = 1;
      colors[0][1] = 0;
      colors[1][0] = 1;
      colors[1][1] = 2;
      break;
   case DebayerBayerPatternParameter::GBGR:
      colors[0][0] = 1;
      colors[0][1] = 2;
      colors[1][0] = 1;
      colors[1][1] = 0;
      break;
   case DebayerBayerPatternParameter::RGBG:
      colors[0][0] = 0;
      colors[0][1] = 1;
      colors[1][0] = 2;
      colors[1][1] = 1;
      break;
   case DebayerBayerPatternParameter::BGRG:
      colors[0][0] = 2;
      colors[0][1] = 1;
      colors[1][0] = 0;
      colors[1][1] = 1;
      break;
   }
}

// ----------------------------------------------------------------------------

class DebayerEngine
{
public:

   DebayerEngine( Image& output, const DebayerInstance& instance, pcl_enum bayerPattern )
      : m_output( output )
      , m_instance( instance )
      , m_bayerPattern( bayerPattern )
   {
   }

   void Debayer( const ImageVariant& source )
   {
      volatile AutoStatusCallbackRestorer saveStatus( m_output.Status() );
      StandardStatus status;
      m_output.SetStatusCallback( &status );

      switch ( m_instance.p_debayerMethod )
      {
      case DebayerMethodParameter::SuperPixel:
         DebayerSuperPixel( source );
         break;
      case DebayerMethodParameter::Bilinear:
         DebayerBilinear( source );
         break;
      case DebayerMethodParameter::VNG:
         DebayerVNG( source );
         break;
      }
   }

   static IsoString MethodId( pcl_enum debayerMethod )
   {
      switch( debayerMethod )
      {
      case DebayerMethodParameter::SuperPixel: return "SuperPixel";
      case DebayerMethodParameter::Bilinear:   return "Bilinear";
      case DebayerMethodParameter::VNG:        return "VNG";
      default:
         throw Error( String().Format( "Internal error: Invalid demosaicing algorithm 0x%x", debayerMethod ) );
      }
   }

   static IsoString PatternId( pcl_enum bayerPattern )
   {
      switch ( bayerPattern )
      {
      case DebayerBayerPatternParameter::RGGB: return "RGGB";
      case DebayerBayerPatternParameter::BGGR: return "BGGR";
      case DebayerBayerPatternParameter::GBRG: return "GBRG";
      case DebayerBayerPatternParameter::GRBG: return "GRBG";
      case DebayerBayerPatternParameter::GRGB: return "GRGB";
      case DebayerBayerPatternParameter::GBGR: return "GBGR";
      case DebayerBayerPatternParameter::RGBG: return "RGBG";
      case DebayerBayerPatternParameter::BGRG: return "BGRG";
      default:
         throw Error( String().Format( "Internal error: Invalid CFA pattern 0x%x", bayerPattern ) );
      }
   }

private:

         Image&           m_output;
   const DebayerInstance& m_instance;
         pcl_enum         m_bayerPattern;

   // -------------------------------------------------------------------------

   template <class P>
   class DebayerThreadBase : public Thread
   {
   public:

      DebayerThreadBase( const AbstractImage::ThreadData& data,
                         Image& output, const GenericImage<P>& source, pcl_enum bayerPattern, int start, int end )
         : m_data( data )
         , m_output( output )
         , m_source( source )
         , m_bayerPattern( bayerPattern )
         , m_start( start )
         , m_end( end )
      {
      }

   protected:

      const AbstractImage::ThreadData& m_data;
            Image&                     m_output;
      const GenericImage<P>&           m_source;
            pcl_enum                   m_bayerPattern;
            int                        m_start, m_end;
   };

   // -------------------------------------------------------------------------

#define m_output       this->m_output
#define m_source       this->m_source
#define m_bayerPattern this->m_bayerPattern
#define m_start        this->m_start
#define m_end          this->m_end
#define SRC_CHANNEL(c) (m_source.IsColor() ? c : 0)

   // -------------------------------------------------------------------------

   template <class P>
   class SuperPixelThread : public DebayerThreadBase<P>
   {
   public:

      SuperPixelThread( const AbstractImage::ThreadData& data,
                        Image& output, const GenericImage<P>& source, pcl_enum bayerPattern, int start, int end )
         : DebayerThreadBase<P>( data, output, source, bayerPattern, start, end )
      {
      }

      void Run() override
      {
         INIT_THREAD_MONITOR()

         const int src_w2 = m_source.Width() >> 1;

         for ( int row = m_start; row < m_end; row++ )
         {
            for ( int col = 0; col < src_w2; col++ )
            {
               int red_col, red_row, green_col1, green_col2, green_row1, green_row2, blue_row, blue_col;
               int col2 = col << 1;
               int row2 = row << 1;
               switch( m_bayerPattern )
               {
               default:
               case DebayerBayerPatternParameter::RGGB:
                  red_col    = col2;
                  red_row    = row2;
                  green_col1 = col2 + 1;
                  green_row1 = row2;
                  green_col2 = col2;
                  green_row2 = row2 + 1;
                  blue_col   = col2 + 1;
                  blue_row   = row2 + 1;
                  break;
               case DebayerBayerPatternParameter::BGGR:
                  red_col    = col2 + 1;
                  red_row    = row2 + 1;
                  green_col1 = col2 + 1;
                  green_row1 = row2;
                  green_col2 = col2;
                  green_row2 = row2 + 1;
                  blue_col   = col2;
                  blue_row   = row2;
                  break;
               case DebayerBayerPatternParameter::GBRG:
                  red_col    = col2;
                  red_row    = row2 + 1;
                  green_col1 = col2;
                  green_row1 = row2;
                  green_col2 = col2 + 1;
                  green_row2 = row2 + 1;
                  blue_col   = col2 + 1;
                  blue_row   = row2;
                  break;
               case DebayerBayerPatternParameter::GRBG:
                  red_col    = col2 + 1;
                  red_row    = row2;
                  green_col1 = col2;
                  green_row1 = row2;
                  green_col2 = col2 + 1;
                  green_row2 = row2 + 1;
                  blue_col   = col2;
                  blue_row   = row2 + 1;
                  break;
               case DebayerBayerPatternParameter::GRGB:
                  red_col    = col2 + 1;
                  red_row    = row2;
                  green_col1 = col2;
                  green_row1 = row2;
                  green_col2 = col2;
                  green_row2 = row2 + 1;
                  blue_col   = col2 + 1;
                  blue_row   = row2 + 1;
                  break;
               case DebayerBayerPatternParameter::GBGR:
                  red_col    = col2 + 1;
                  red_row    = row2 + 1;
                  green_col1 = col2;
                  green_row1 = row2;
                  green_col2 = col2;
                  green_row2 = row2 + 1;
                  blue_col   = col2 + 1;
                  blue_row   = row2;
                  break;
               case DebayerBayerPatternParameter::RGBG:
                  red_col    = col2;
                  red_row    = row2;
                  green_col1 = col2 + 1;
                  green_row1 = row2;
                  green_col2 = col2 + 1;
                  green_row2 = row2 + 1;
                  blue_col   = col2;
                  blue_row   = row2 + 1;
                  break;
               case DebayerBayerPatternParameter::BGRG:
                  red_col    = col2;
                  red_row    = row2 + 1;
                  green_col1 = col2 + 1;
                  green_row1 = row2;
                  green_col2 = col2 + 1;
                  green_row2 = row2 + 1;
                  blue_col   = col2;
                  blue_row   = row2;
                  break;
               }

               // red
               P::FromSample( m_output( col, row, 0 ), m_source( red_col, red_row, SRC_CHANNEL( 0 ) ) );
               //green
               double v1, v2;
               P::FromSample( v1, m_source( green_col1, green_row1, SRC_CHANNEL( 1 ) ) );
               P::FromSample( v2, m_source( green_col2, green_row2, SRC_CHANNEL( 1 ) ) );
               m_output( col, row, 1 ) = (v1 + v2)/2;
               // blue
               P::FromSample( m_output( col, row, 2 ), m_source( blue_col, blue_row, SRC_CHANNEL( 2 ) ) );
            }

            UPDATE_THREAD_MONITOR( 16 )
         }
      }
   }; // SuperPixelThread

   // -------------------------------------------------------------------------

   template <class P>
   class BilinearThread : public DebayerThreadBase<P>
   {
   public:

      BilinearThread( const AbstractImage::ThreadData& data,
                      Image& output, const GenericImage<P>& source, pcl_enum bayerPattern, int start, int end )
         : DebayerThreadBase<P>( data, output, source, bayerPattern, start, end )
      {
      }

      void Run() override
      {
         /*
          * http://winfij.homeip.net/maximdl/bilineardebayer.html
          */

         INIT_THREAD_MONITOR()

         const int src_w = m_source.Width();
         int colors[ 2 ][ 2 ]; // [row][col]
         BayerPatternToColorIndices( colors, m_bayerPattern );

         for ( int row = m_start; row < m_end; ++row )
         {
            // skip the first and last column
            for ( int col = 1; col < src_w-1; ++col )
            {
               //color_index = (row % 2) + (col % 2); // modulo division
               int current_color = colors[(row % 2)][(col % 2)];
               int next_color = colors[(row % 2)][((col+1) % 2)];
               double c1, c2, v1, v2, v3, v4, target_colors[ 3 ];
               //straight copy of the current color
               P::FromSample( target_colors[current_color], m_source( col, row, SRC_CHANNEL( current_color ) ) );
               switch ( current_color )
               {
               case 0: // red done
               case 2: // blue done
                  // get green samples
                  P::FromSample( v2, m_source( col, row + 1, SRC_CHANNEL( 1 ) ) );
                  P::FromSample( v1, m_source( col, row - 1, SRC_CHANNEL( 1 ) ) );
                  P::FromSample( v3, m_source( col - 1, row, SRC_CHANNEL( 1 ) ) );
                  P::FromSample( v4, m_source( col + 1, row, SRC_CHANNEL( 1 ) ) );
                  c1 = (v1 + v2 + v3 + v4)/4;
                  target_colors[1] = c1;
                  // get blue or red samples
                  P::FromSample( v1, m_source( col - 1, row - 1, SRC_CHANNEL( 2-current_color ) ) );
                  P::FromSample( v2, m_source( col + 1, row + 1, SRC_CHANNEL( 2-current_color ) ) );
                  P::FromSample( v3, m_source( col - 1, row + 1, SRC_CHANNEL( 2-current_color ) ) );
                  P::FromSample( v4, m_source( col + 1, row - 1, SRC_CHANNEL( 2-current_color ) ) );
                  c1 = (v1 + v2 + v3 + v4)/4;
                  // if the current color is red then we just grabbed blue and vise versa
                  target_colors[2-current_color] = c1;
                  break;

               case 1: // green already done
                  P::FromSample( v1, m_source( col, row - 1, SRC_CHANNEL( 2-next_color ) ) );
                  P::FromSample( v2, m_source( col, row + 1, SRC_CHANNEL( 2-next_color ) ) );
                  P::FromSample( v3, m_source( col - 1, row, SRC_CHANNEL( next_color ) ) ); // next color
                  P::FromSample( v4, m_source( col + 1, row, SRC_CHANNEL( next_color ) ) );
                  c1 = (v1 + v2)/2;
                  c2 = (v3 + v4)/2;
                  // if the current color is red then we just grabbed blue and vise versa
                  target_colors[next_color] = c2;
                  target_colors[2-next_color] = c1;
                  break;
               }

               for ( int i = 0; i < 3; i++ )
                  m_output( col, row, i ) = target_colors[i];
            }

            // get colors for the inner and outer column
            for ( int i = 0; i < 3; i++ )
            {
               m_output( 0, row, i ) = m_output( 1, row, i );
               m_output( src_w - 1, row, i ) = m_output( src_w-2, row, i );
            }

            UPDATE_THREAD_MONITOR( 16 )
         }
      }
   }; // BilinearThread

   // -------------------------------------------------------------------------

   template <class P>
   class VNGThread : public DebayerThreadBase<P>
   {
   public:

      VNGThread( const AbstractImage::ThreadData& data,
                 Image& output, const GenericImage<P>& source, pcl_enum bayerPattern, int start, int end )
         : DebayerThreadBase<P>( data, output, source, bayerPattern, start, end )
      {
      }

      void Run() override
      {
         // http://openfmi.net/plugins/scmsvn/cgi-bin/viewcvs.cgi/*checkout*/books/Chang.pdf?content-type=text%2Fplain&rev=15&root=interpol

         INIT_THREAD_MONITOR()

         const int src_w = m_source.Width();
         int colors[ 2 ][ 2 ]; // [row][col]
         BayerPatternToColorIndices( colors, m_bayerPattern );

         // iterate over all rows assigned to this thread
         for ( int row = m_start; row < m_end; row++ )
         {
            // skip two first and last columns
            for ( int col = 2; col < src_w-2; col++ )
            {
               // cache matrix of 5x5 pixels around current pixel
               double v[ 25 ];     // values of 5x5 pixels of bayered image, centered on current pixel
               int channels[ 25 ]; // channel indices for 5x5 pixels
               if ( col == 2 )
               {
                  // for first column, read whole matrix
                  for ( int y = 0; y < 5; y++ )
                     for ( int x = 0; x < 5; x++ )
                     {
                        int c = col+x-2;
                        int r = row+y-2;
                        channels[y*5+x] = colors[r%2][c%2];
                        P::FromSample( v[y*5+x], m_source( c, r, SRC_CHANNEL( channels[y*5+x] ) ) );
                     }
               }
               else
               {
                  // for other columns, shift the matrix to the left and read just last column
                  for ( int y = 0; y < 5; y++ )
                  {
                     for (int x = 0; x < 4; x++)
                     {
                        channels[y*5+x] = channels[y*5+x+1];
                        v[y*5+x] = v[y*5+x+1];
                     }
                     channels[y*5+4] = colors[(row+y-2)%2][(col+2)%2];
                     P::FromSample( v[y*5+4], m_source( col+2, row+y-2, SRC_CHANNEL( channels[y*5+4] ) ) );
                  }
               }

               double gradients[ 8 ];     // gradients in N, E, S, W, NE, SW, NW and SE directions
               int valid_gradients[ 8 ];  // list of gradients below computed threshold (valid for color computation)
               int valid_gradients_count; // number of valid gradients
               double value_sums[ 3 ];    // sums of color coefficients for all valid gradients

               // get channel for actual pixel from bayer pattern
               int current_channel = colors[row % 2][col % 2];

               // compute gradients in eight directions
               ComputeGradients( v, current_channel, gradients );

               // compute threshold and list of gradients below the threshold
               ThresholdGradients( gradients, valid_gradients, valid_gradients_count );

               // compute sums of color coefficients for each of valid gradients
               ComputeSums( v, channels, gradients, valid_gradients, valid_gradients_count, value_sums );

               // current_channel hold index of the channel of current pixels
               // get indices of two remaining channels
               int other_channel1 = 0, other_channel2 = 0;
               switch ( current_channel )
               {
               case 0:
                  other_channel1 = 1;
                  other_channel2 = 2;
                  break;
               case 1:
                  other_channel1 = 0;
                  other_channel2 = 2;
                  break;
               case 2:
                  other_channel1 = 0;
                  other_channel2 = 1;
                  break;
               }

               // compute normalized color differences for remaining channels
               double diff1 = (value_sums[other_channel1] - value_sums[current_channel])/double( valid_gradients_count );
               double diff2 = (value_sums[other_channel2] - value_sums[current_channel])/double( valid_gradients_count );

               // compute current pixel color
               // current channel is directly known from bayered image (take the center of the cached matrix)
               m_output( col, row, current_channel ) = v[12];
               // two remaining channels are computed using normalized differences
               m_output( col, row, other_channel1 ) = Range( v[12]+diff1, 0.0, 1.0 );
               m_output( col, row, other_channel2 ) = Range( v[12]+diff2, 0.0, 1.0 );
            }

            // get colors for the inner and outer two columns
            for ( int i = 0; i < 3; i++ )
            {
               m_output( 0, row, i ) = m_output( 2, row, i );
               m_output( 1, row, i ) = m_output( 2, row, i );
               m_output( src_w - 1, row, i ) = m_output( src_w - 3, row, i );
               m_output( src_w - 2, row, i ) = m_output( src_w - 3, row, i );
            }

            UPDATE_THREAD_MONITOR( 16 )
         }
      }

   private:

      // compute gradients in eight directions around current pixel
      void ComputeGradients( const double* v, int current_channel, double* gradients )
      {
         // compute gradients in eight directions
         // formulas taken directly from VNG method paper
         const int G_N = 0;
         const int G_E = 1;
         const int G_S = 2;
         const int G_W = 3;
         const int G_NE = 4;
         const int G_SE = 5;
         const int G_NW = 6;
         const int G_SW = 7;

         // compute vertical and horizontal gradients
         gradients[G_N]  = Abs( v[7]  - v[17] )   + Abs( v[2]  - v[12] )   + Abs( v[6]  - v[16] )/2 +
                           Abs( v[8]  - v[18] )/2 + Abs( v[1]  - v[11] )/2 + Abs( v[3]  - v[13] )/2;
         gradients[G_E]  = Abs( v[13] - v[11] )   + Abs( v[14] - v[12] )   + Abs( v[8]  - v[6]  )/2 +
                           Abs( v[18] - v[16] )/2 + Abs( v[9]  - v[7]  )/2 + Abs( v[19] - v[17] )/2;
         gradients[G_S]  = Abs( v[17] - v[7]  )   + Abs( v[22] - v[12] )   + Abs( v[16] - v[6]  )/2 +
                           Abs( v[18] - v[8]  )/2 + Abs( v[21] - v[11] )/2 + Abs( v[23] - v[13] )/2;
         gradients[G_W]  = Abs( v[11] - v[13] )   + Abs( v[10] - v[12] )   + Abs( v[6]  - v[8]  )/2 +
                           Abs( v[16] - v[18] )/2 + Abs( v[5]  - v[7]  )/2 + Abs( v[15] - v[17] )/2;

         // diagonal gradients differ for green channel and other channels
         switch ( current_channel )
         {
         // green center
         default: // just to shut down '-Wmaybe-uninitialized' warnings
         case 1:
            gradients[G_NE] = Abs( v[8]  - v[16] ) + Abs( v[4]  - v[12] ) +
                              Abs( v[3]  - v[11] ) + Abs( v[9]  - v[17] );
            gradients[G_SE] = Abs( v[18] - v[6]  ) + Abs( v[24] - v[12] ) +
                              Abs( v[23] - v[11] ) + Abs( v[19] - v[7]  );
            gradients[G_NW] = Abs( v[6]  - v[18] ) + Abs( v[0]  - v[12] ) +
                              Abs( v[1]  - v[13] ) + Abs( v[5]  - v[17] );
            gradients[G_SW] = Abs( v[16] - v[8]  ) + Abs( v[20] - v[12] ) +
                              Abs( v[21] - v[13] ) + Abs( v[15] - v[7]  );
            break;
         // red or blue center
         case 0:
         case 2:
            gradients[G_NE] = Abs( v[8]  - v[16] )   + Abs( v[4]  - v[12] )   + Abs( v[7]  - v[11] )/2 +
                              Abs( v[13] - v[17] )/2 + Abs( v[3]  - v[7]  )/2 + Abs( v[9]  - v[13] )/2;
            gradients[G_SE] = Abs( v[18] - v[6]  )   + Abs( v[24] - v[12] )   + Abs( v[13] - v[7]  )/2 +
                              Abs( v[17] - v[11] )/2 + Abs( v[19] - v[13] )   + Abs( v[23] - v[17] )/2;
            gradients[G_NW] = Abs( v[6]  - v[18] )   + Abs( v[0]  - v[12] )   + Abs( v[7]  - v[13] )/2 +
                              Abs( v[11] - v[17] )/2 + Abs( v[1]  - v[7]  )/2 + Abs( v[5]  - v[11] )/2;
            gradients[G_SW] = Abs( v[16] - v[8]  )   + Abs( v[20] - v[12] )   + Abs( v[11] - v[7]  )/2 +
                              Abs( v[17] - v[13] )/2 + Abs( v[15] - v[11] )   + Abs( v[21] - v[17] )/2;
            break;
         }
      }

      // compute threshold and list of gradients below the threshold
      void ThresholdGradients( const double* gradients, int* valid_gradients, int& count )
      {
         // get minimal and maximal gradient
         double min = gradients[0], max = gradients[0];
         for ( int i = 1; i < 8; i++ )
         {
            if ( gradients[i] < min )
               min = gradients[i];
            if ( gradients[i] > max )
               max = gradients[i];
         }

         // compute threshold
         // k1 and k2 coefficient values are taken from VNG method paper (empirically found to produce best results)
         const double k1 = 1.5;
         const double k2 = 0.5;
         double threshold = k1*min + k2*(max - min);

         // find valid gradients (below threshold)
         count = 0;
         for ( int i = 0; i < 8; i++ )
            if ( gradients[i] <= threshold+1e-10 )
               valid_gradients[count++] = i;
      }

      // compute sums of averaged color coefficients form bayered image for each of valid gradients
      void ComputeSums( const double* v, const int* channels,
                        const double* gradients, const int* valid_gradients, int count,
                        double* sums )
      {
         // list of indices to 5x5 matrix to compute summed colors for respective gradients
         // for green center pixel
         static int green_center_indices[ 8 ][ 8 ] =
         {
            {  1,  2,  3,  7, 11, 12, 13, -1 },
            {  7,  9, 12, 13, 14, 17, 19, -1 },
            { 11, 12, 13, 17, 21, 22, 23, -1 },
            {  5,  7, 10, 11, 12, 15, 17, -1 },
            {  3,  7,  8,  9, 13, -1, -1, -1 },
            { 13, 17, 18, 19, 23, -1, -1, -1 },
            {  1,  5,  6,  7, 11, -1, -1, -1 },
            { 11, 15, 16, 17, 21, -1, -1, -1 }
         };

         // list of indices to 5x5 matrix to compute summed colors for respective gradients
         // for red or blue center pixel
         static int other_center_indices[ 8 ][ 8 ] =
         {
            {  2,  6,  7,  8, 12, -1, -1, -1 },
            {  8, 12, 13, 14, 18, -1, -1, -1 },
            { 12, 16, 17, 18, 22, -1, -1, -1 },
            {  6, 10, 11, 12, 16, -1, -1, -1 },
            {  3,  4,  7,  8,  9, 12, 13, -1 },
            { 12, 13, 17, 18, 19, 23, 24, -1 },
            {  0,  1,  5,  6,  7, 11, 12, -1 },
            { 11, 12, 15, 16, 17, 20, 21, -1 }
         };

         // init sums to zero
         for ( int channel = 0; channel < 3; channel++ )
            sums[channel] = 0;

         // for all valid gradients
         for ( int i = 0; i < count; i++ )
         {
            // get index of the valid gradient
            int grad = valid_gradients[i];
            int j = 0;
            // init partial sums over channels
            double partial_sums[ 3 ] = { 0.0, 0.0, 0.0 };
            int partial_counts[ 3 ] = { 0, 0, 0 };
            int* indices = (channels[ 12 ] == 1) ? green_center_indices[grad] : other_center_indices[grad];

            // compute partial sums for current gradient
            while ( indices[j] >= 0 )
            {
               // get index to 5x5 matrix
               int index = indices[j];
               // get bayer channel for this index
               int channel = channels[index];
               // add to sum for this channel
               partial_sums[channel] += v[index];
               partial_counts[channel]++;
               j++;
            }

            // add averaged partial sums to total sums per each channel
            for ( int channel = 0; channel < 3; channel++ )
               if ( partial_counts[channel] > 0 )
                  sums[channel] += partial_sums[channel]/double( partial_counts[channel] );
         }
      }
   }; // VNGThread

   // -------------------------------------------------------------------------

#undef m_output
#undef m_source
#undef m_bayerPattern
#undef m_start
#undef m_end

   // -------------------------------------------------------------------------

   template <class P>
   void DebayerSuperPixel( const GenericImage<P>& source )
   {
      int target_w = source.Width() >> 1;
      int target_h = source.Height() >> 1;

      m_output.AllocateData( target_w, target_h, 3, ColorSpace::RGB );
      m_output.Status().Initialize( "SuperPixel demosaicing", target_h );

      Array<size_type> L = Thread::OptimalThreadLoads( target_h );
      ReferenceArray<SuperPixelThread<P> > threads;
      AbstractImage::ThreadData data( m_output, target_h );
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new SuperPixelThread<P>( data, m_output, source, m_bayerPattern, n, n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      m_output.Status() = data.status;
   }

   void DebayerSuperPixel( const ImageVariant& source )
   {
      if ( source.IsFloatSample() )
         switch ( source.BitsPerSample() )
         {
         case 32: DebayerSuperPixel( static_cast<const Image&>( *source ) ); break;
         case 64: DebayerSuperPixel( static_cast<const DImage&>( *source ) ); break;
         }
      else
         switch ( source.BitsPerSample() )
         {
         case  8: DebayerSuperPixel( static_cast<const UInt8Image&>( *source ) ); break;
         case 16: DebayerSuperPixel( static_cast<const UInt16Image&>( *source ) ); break;
         case 32: DebayerSuperPixel( static_cast<const UInt32Image&>( *source ) ); break;
         }
   }

   // -------------------------------------------------------------------------

   template <class P>
   void DebayerBilinear( const GenericImage<P>& source )
   {
      int target_w = source.Width();
      int target_h = source.Height();

      m_output.AllocateData( target_w, target_h, 3, ColorSpace::RGB );
      m_output.Status().Initialize( "Bilinear demosaicing", target_h-2 );

      Array<size_type> L = Thread::OptimalThreadLoads( target_h-2 );
      ReferenceArray<BilinearThread<P> > threads;
      AbstractImage::ThreadData data( m_output, target_h-2 );
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new BilinearThread<P>( data, m_output, source, m_bayerPattern,
                                             1 + n,
                                             1 + n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      // Copy top and bottom rows from the adjecent ones.
      for ( int col = 0; col < target_w; col++ )
         for ( int i = 0; i < 3; i++ )
         {
            m_output( col, 0, i ) = m_output( col, 1, i );
            m_output( col, target_h-1, i ) = m_output( col, target_h-2, i );
         }

      m_output.Status() = data.status;
   }

   void DebayerBilinear( const ImageVariant& source )
   {
      if ( source.IsFloatSample() )
         switch ( source.BitsPerSample() )
         {
         case 32: DebayerBilinear( static_cast<const Image&>( *source ) ); break;
         case 64: DebayerBilinear( static_cast<const DImage&>( *source ) ); break;
         }
      else
         switch ( source.BitsPerSample() )
         {
         case  8: DebayerBilinear( static_cast<const UInt8Image&>( *source ) ); break;
         case 16: DebayerBilinear( static_cast<const UInt16Image&>( *source ) ); break;
         case 32: DebayerBilinear( static_cast<const UInt32Image&>( *source ) ); break;
         }
   }

   // -------------------------------------------------------------------------

   template <class P>
   void DebayerVNG( const GenericImage<P>& source )
   {
      int target_w = source.Width();
      int target_h = source.Height();

      m_output.AllocateData( target_w, target_h, 3, ColorSpace::RGB );
      m_output.Status().Initialize( "VNG demosaicing", target_h-4 );

      Array<size_type> L = Thread::OptimalThreadLoads( target_h-4 );
      ReferenceArray<VNGThread<P> > threads;
      AbstractImage::ThreadData data( m_output, target_h-4 );
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new VNGThread<P>( data, m_output, source, m_bayerPattern,
                                        2 + n,
                                        2 + n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      // Copy top and bottom two rows from the adjecent ones.
      for ( int col = 0; col < target_w; col++ )
         for ( int i = 0; i < 3; i++ )
         {
            m_output( col, 0, i ) = m_output( col, 1, i ) = m_output( col, 2, i );
            m_output( col, target_h-1, i ) = m_output( col, target_h-2, i ) = m_output( col, target_h-3, i );
         }

      m_output.Status() = data.status;
   }

   void DebayerVNG( const ImageVariant& source )
   {
      if ( source.IsFloatSample() )
         switch ( source.BitsPerSample() )
         {
         case 32: DebayerVNG( static_cast<const Image&>( *source ) ); break;
         case 64: DebayerVNG( static_cast<const DImage&>( *source ) ); break;
         }
      else
         switch ( source.BitsPerSample() )
         {
         case  8: DebayerVNG( static_cast<const UInt8Image&>( *source ) ); break;
         case 16: DebayerVNG( static_cast<const UInt16Image&>( *source ) ); break;
         case 32: DebayerVNG( static_cast<const UInt32Image&>( *source ) ); break;
         }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * FBDD - Fake Before Demosaicing Denoising.
 * By Jacek Gozdz and Luis Sanz Rodríguez.
 *
 * *** ### EXPERIMENTAL - DO NOT ENABLE BY DEFAULT ### ***
 */
class FBDDEngine
{
public:

   FBDDEngine( int bayerPattern, bool full )
      : m_full( full )
   {
      BayerPatternToColorIndices( m_color, bayerPattern );
   }

   void Denoise( ImageVariant& image )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: Denoise( static_cast<Image&>( *image ) ); break;
         case 64: Denoise( static_cast<DImage&>( *image ) ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: Denoise( static_cast<UInt8Image&>( *image ) ); break;
         case 16: Denoise( static_cast<UInt16Image&>( *image ) ); break;
         case 32: Denoise( static_cast<UInt32Image&>( *image ) ); break;
         }
   }

private:

   uint16        (*m_image)[3] = nullptr;
   int           m_width = 0;
   int           m_height = 0;
   bool          m_full = true;
   int           m_color[ 2 ][ 2 ]; // row, col
   StatusMonitor m_monitor;

   template <typename T>
   static T Clip( T x )
   {
      return Range( x, T( 0 ), T( 65535 ) );
   }

   static uint16 Limit( uint16 x, uint16 y, uint16 z )
   {
      if ( z < y )
         Swap( y, z );
      return Range( x, y, z );
   }

   int FilterColor( int row, int col ) const
   {
      return m_color[row%2][col%2];
   }

   template <class P>
   void Denoise( GenericImage<P>& image )
   {
      m_width = image.Width();
      m_height = image.Height();

      int N = m_height + (m_height-10) + 2*(m_height-6) + 2*(m_height-2) + (m_height-12);
      if ( m_full )
         N += 2*(m_height-2) + 2*m_height*m_width + 2*(m_height-12);
      StandardStatus status;
      m_monitor.SetCallback( &status );
      m_monitor.Initialize( "FBDD denoising", N );

      Array<uint16> data( size_type( m_width ) * size_type( m_height ) * 3, uint16( 0 ) );
      m_image = reinterpret_cast<uint16 (*)[3]>( data.Begin() );
      if ( image.NumberOfNominalChannels() == 1 )
      {
         typename GenericImage<P>::const_sample_iterator i( image );
         uint16 (*p)[3] = m_image;
         for ( int row = 0; row < m_height; ++row )
            for ( int col = 0; col < m_width; ++col, ++i, ++p )
               p[0][FilterColor( row, col )] = UInt16PixelTraits::ToSample( *i );
      }
      else
      {
         typename GenericImage<P>::const_pixel_iterator i( image );
         uint16 (*p)[3] = m_image;
         for ( int row = 0; row < m_height; ++row )
            for ( int col = 0; col < m_width; ++col, ++i, ++p )
            {
               int c = FilterColor( row, col );
               p[0][c] = UInt16PixelTraits::ToSample( i[c] );
            }
      }

      FBDD();

      if ( image.NumberOfNominalChannels() == 1 )
      {
         typename GenericImage<P>::sample_iterator i( image );
         const uint16 (*p)[3] = m_image;
         for ( int row = 0; row < m_height; ++row )
            for ( int col = 0; col < m_width; ++col, ++i, ++p )
               UInt16PixelTraits::FromSample( *i, p[0][FilterColor( row, col )] );
      }
      else
      {
         typename GenericImage<P>::pixel_iterator i( image );
         const uint16 (*p)[3] = m_image;
         for ( int row = 0; row < m_height; ++row )
            for ( int col = 0; col < m_width; ++col, ++i, ++p )
            {
               int c = FilterColor( row, col );
               UInt16PixelTraits::FromSample( i[c], p[0][c] );
            }
      }

      m_monitor.Complete();
   }

   void InterpolateBorder( int border )
   {
      for ( int row = 0; row < m_height; ++row, ++m_monitor )
         for ( int col = 0; col < m_width; ++col )
         {
            if ( col == border && row >= border && row < m_height - border )
               col = m_width - border;
            double sum[ 3 ] = { 0 };
            int count[ 3 ] = { 0 };
            for ( int y = row - 1; y != row + 2; ++y )
               for ( int x = col - 1; x != col + 2; ++x )
                  if ( y >= 0 && x >= 0 && y < m_height && x < m_width )
                  {
                     int f = FilterColor( y, x );
                     sum[f] += m_image[y*m_width + x][f];
                     ++count[f];
                  }
            int f = FilterColor( row, col );
            for ( int c = 0; c < 3; ++c )
               if ( c != f )
                  if ( count[c] > 0 )
                     m_image[row*m_width + col][c] = RoundInt( sum[c]/count[c] );
         }
   }

   void FBDD()
   {
      InterpolateBorder( 4 ); // h
      FBDDGreen();      // h-10
      DCBColorFull();   // 2*(h-6) + h-12
      FBDDCorrection(); // 2*(h-2)

      if ( m_full )
      {
         Array<double> data( size_type( m_width ) * size_type( m_height ) * 3, 0.0 );
         double (*image2)[3] = reinterpret_cast<double (*)[3]>( data.Begin() );
         DCBColor();                // 2*(h-2)
         RGBToLCH( image2 );        // h*w
         FBDDCorrection2( image2 ); // h-12
         FBDDCorrection2( image2 ); // h-12
         LCHToRGB( image2 );        // h*w
      }
   }

   void FBDDGreen()
   {
      const int u = m_width;
      const int v = 2*u;
      const int w = 3*u;
      const int x = 4*u;
      const int y = 5*u;

      for ( int row = 5; row < m_height-5; ++row, ++m_monitor )
         for ( int col = 5 + (FilterColor( row, 1 ) & 1),
                   indx = row*m_width + col,
                   c = FilterColor( row, col ); col < u-5; col += 2, indx += 2 )
         {
            double f[ 4 ];
            f[0] = 1.0/(1 + Abs( int( m_image[indx-u][1] ) - int( m_image[indx-w][1] ) )
                          + Abs( int( m_image[indx-w][1] ) - int( m_image[indx+y][1] ) ));
            f[1] = 1.0/(1 + Abs( int( m_image[indx+1][1] ) - int( m_image[indx+3][1] ) )
                          + Abs( int( m_image[indx+3][1] ) - int( m_image[indx-5][1] ) ));
            f[2] = 1.0/(1 + Abs( int( m_image[indx-1][1] ) - int( m_image[indx-3][1] ) )
                          + Abs( int( m_image[indx-3][1] ) - int( m_image[indx+5][1] ) ));
            f[3] = 1.0/(1 + Abs( int( m_image[indx+u][1] ) - int( m_image[indx+w][1] ) )
                          + Abs( int( m_image[indx+w][1] ) - int( m_image[indx-y][1] ) ));

            double g[ 4 ];
            g[0] = Clip( (23.0*m_image[indx-u][1]
                        + 23.0*m_image[indx-w][1]
                        +  2.0*m_image[indx-y][1]
                        +  8.0*(int( m_image[indx-v][c] ) - int( m_image[indx-x][c] ))
                        + 40.0*(int( m_image[indx][c] ) - int( m_image[indx-v][c] )))/48 );
            g[1] = Clip( (23.0*m_image[indx+1][1]
                        + 23.0*m_image[indx+3][1]
                        +  2.0*m_image[indx+5][1]
                        +  8.0*(int( m_image[indx+2][c] ) - int( m_image[indx+4][c] ))
                        + 40.0*(int( m_image[indx][c] ) - int( m_image[indx+2][c] )))/48 );
            g[2] = Clip( (23.0*m_image[indx-1][1]
                        + 23.0*m_image[indx-3][1]
                        +  2.0*m_image[indx-5][1]
                        +  8.0*(int( m_image[indx-2][c] ) - int( m_image[indx-4][c] ))
                        + 40.0*(int( m_image[indx][c] ) - int( m_image[indx-2][c] )))/48 );
            g[3] = Clip( (23.0*m_image[indx+u][1]
                        + 23.0*m_image[indx+w][1]
                        +  2.0*m_image[indx+y][1]
                        +  8.0*(int( m_image[indx+v][c] ) - int( m_image[indx+x][c] ))
                        + 40.0*(int( m_image[indx][c] ) - int( m_image[indx+v][c] )))/48 );

            m_image[indx][1] = RoundInt( Clip( (f[0]*g[0] + f[1]*g[1] + f[2]*g[2] + f[3]*g[3])/(f[0] + f[1] + f[2] + f[3]) ) );

            uint16 min = Min( m_image[indx+1+u][1],
                              Min( m_image[indx+1-u][1],
                                   Min( m_image[indx-1+u][1],
                                        Min( m_image[indx-1-u][1],
                                             Min( m_image[indx-1][1],
                                                  Min( m_image[indx+1][1],
                                                       Min( m_image[indx-u][1],
                                                            m_image[indx+u][1] ) ) ) ) ) ) );
            uint16 max = Max( m_image[indx+1+u][1],
                              Max( m_image[indx+1-u][1],
                                   Max( m_image[indx-1+u][1],
                                        Max( m_image[indx-1-u][1],
                                             Max( m_image[indx-1][1],
                                                  Max( m_image[indx+1][1],
                                                       Max( m_image[indx-u][1],
                                                            m_image[indx+u][1] ) ) ) ) ) ) );

            m_image[indx][1] = Limit( m_image[indx][1], min, max );
         }
   }

   void DCBColorFull()
   {
      const int u = m_width;
      const int w = 3*u;

      Array<double> data( m_width * m_height * 2 );
      double (*chroma)[ 2 ] = reinterpret_cast<double (*)[2]>( data.Begin() );

      for ( int row = 1; row < m_height-1; row++ )
         for ( int col = 1 + (FilterColor( row, 1 ) & 1 ),
                   indx = row*m_width + col,
                   c = FilterColor( row, col ),
                   d = c/2; col < u-1; col += 2, indx +=2 )
            chroma[indx][d] = int( m_image[indx][c] ) - int( m_image[indx][1] );

      for ( int row = 3; row < m_height-3; ++row, ++m_monitor )
         for ( int col = 3 + (FilterColor( row, 1 ) & 1),
                   indx = row*m_width + col,
                   c = 1 - FilterColor( row, col )/2; col < u-3; col += 2, indx += 2 )
         {
            double f[ 4 ];
            f[0] = 1/(1 + Abs( chroma[indx-u-1][c] - chroma[indx+u+1][c] )
                        + Abs( chroma[indx-u-1][c] - chroma[indx-w-3][c] )
                        + Abs( chroma[indx+u+1][c] - chroma[indx-w-3][c] ));
            f[1] = 1/(1 + Abs( chroma[indx-u+1][c] - chroma[indx+u-1][c] )
                        + Abs( chroma[indx-u+1][c] - chroma[indx-w+3][c] )
                        + Abs( chroma[indx+u-1][c] - chroma[indx-w+3][c] ));
            f[2] = 1/(1 + Abs( chroma[indx+u-1][c] - chroma[indx-u+1][c] )
                        + Abs( chroma[indx+u-1][c] - chroma[indx+w+3][c] )
                        + Abs( chroma[indx-u+1][c] - chroma[indx+w-3][c] ));
            f[3] = 1/(1 + Abs( chroma[indx+u+1][c] - chroma[indx-u-1][c] )
                        + Abs( chroma[indx+u+1][c] - chroma[indx+w-3][c] )
                        + Abs( chroma[indx-u-1][c] - chroma[indx+w+3][c] ));

            double g[ 4 ];
            g[0] =  1.325*chroma[indx-u-1][c]
                  - 0.175*chroma[indx-w-3][c]
                  - 0.075*chroma[indx-w-1][c]
                  - 0.075*chroma[indx-u-3][c];
            g[1] =  1.325*chroma[indx-u+1][c]
                  - 0.175*chroma[indx-w+3][c]
                  - 0.075*chroma[indx-w+1][c]
                  - 0.075*chroma[indx-u+3][c];
            g[2] =  1.325*chroma[indx+u-1][c]
                  - 0.175*chroma[indx+w-3][c]
                  - 0.075*chroma[indx+w-1][c]
                  - 0.075*chroma[indx+u-3][c];
            g[3] =  1.325*chroma[indx+u+1][c]
                  - 0.175*chroma[indx+w+3][c]
                  - 0.075*chroma[indx+w+1][c]
                  - 0.075*chroma[indx+u+3][c];

            chroma[indx][c] = (f[0]*g[0] + f[1]*g[1] + f[2]*g[2] + f[3]*g[3])/(f[0] + f[1] + f[2] + f[3]);
         }

      for ( int row = 3; row < m_height-3; ++row, ++m_monitor )
         for ( int col = 3 + (FilterColor( row, 2 ) & 1),
                   indx = row*m_width + col,
                   c = FilterColor( row, col+1 )/2; col < u-3; col += 2, indx += 2 )
            for ( int d = 0; d <= 1; c = 1-c, d++ )
            {
               double f[ 4 ];
               f[0] = 1/(1 + Abs( chroma[indx-u][c] - chroma[indx+u][c] )
                           + Abs( chroma[indx-u][c] - chroma[indx-w][c] )
                           + Abs( chroma[indx+u][c] - chroma[indx-w][c] ));
               f[1] = 1/(1 + Abs( chroma[indx+1][c] - chroma[indx-1][c] )
                           + Abs( chroma[indx+1][c] - chroma[indx+3][c] )
                           + Abs( chroma[indx-1][c] - chroma[indx+3][c] ));
               f[2] = 1/(1 + Abs( chroma[indx-1][c] - chroma[indx+1][c] )
                           + Abs( chroma[indx-1][c] - chroma[indx-3][c] )
                           + Abs( chroma[indx+1][c] - chroma[indx-3][c] ));
               f[3] = 1/(1 + Abs( chroma[indx+u][c] - chroma[indx-u][c] )
                           + Abs( chroma[indx+u][c] - chroma[indx+w][c] )
                           + Abs( chroma[indx-u][c] - chroma[indx+w][c] ));

               double g[ 4 ];
               g[0] = 0.875*chroma[indx-u][c] + 0.125*chroma[indx-w][c];
               g[1] = 0.875*chroma[indx+1][c] + 0.125*chroma[indx+3][c];
               g[2] = 0.875*chroma[indx-1][c] + 0.125*chroma[indx-3][c];
               g[3] = 0.875*chroma[indx+u][c] + 0.125*chroma[indx+w][c];

               chroma[indx][c] = (f[0]*g[0] + f[1]*g[1] + f[2]*g[2] + f[3]*g[3])/(f[0] + f[1] + f[2] + f[3]);
            }

      for ( int row = 6; row < m_height-6; ++row, ++m_monitor )
         for ( int col = 6, indx = row*m_width + col; col < m_width-6; col++, indx++ )
         {
            m_image[indx][0] = RoundInt( Clip( chroma[indx][0] + m_image[indx][1] ) );
            m_image[indx][2] = RoundInt( Clip( chroma[indx][1] + m_image[indx][1] ) );

            uint16
            g1 = Min( m_image[indx+1+u][0],
                      Min( m_image[indx+1-u][0],
                           Min( m_image[indx-1+u][0],
                                Min( m_image[indx-1-u][0],
                                     Min( m_image[indx-1][0],
                                          Min( m_image[indx+1][0],
                                               Min( m_image[indx-u][0],
                                                    m_image[indx+u][0] ) ) ) ) ) ) );
            uint16
            g2 = Max( m_image[indx+1+u][0],
                      Max( m_image[indx+1-u][0],
                           Max( m_image[indx-1+u][0],
                                Max( m_image[indx-1-u][0],
                                     Max( m_image[indx-1][0],
                                          Max( m_image[indx+1][0],
                                               Max( m_image[indx-u][0],
                                                    m_image[indx+u][0] ) ) ) ) ) ) );
            m_image[indx][0] = Limit( m_image[indx][0], g2, g1 );

            g1 = Min( m_image[indx+1+u][2],
                      Min( m_image[indx+1-u][2],
                           Min( m_image[indx-1+u][2],
                                Min( m_image[indx-1-u][2],
                                     Min( m_image[indx-1][2],
                                          Min( m_image[indx+1][2],
                                               Min( m_image[indx-u][2],
                                                    m_image[indx+u][2] ) ) ) ) ) ) );
            g2 = Max( m_image[indx+1+u][2],
                      Max( m_image[indx+1-u][2],
                           Max( m_image[indx-1+u][2],
                                Max( m_image[indx-1-u][2],
                                     Max( m_image[indx-1][2],
                                          Max( m_image[indx+1][2],
                                               Max( m_image[indx-u][2],
                                                    m_image[indx+u][2] ) ) ) ) ) ) );
            m_image[indx][2] = Limit( m_image[indx][2], g2, g1 );
         }
   }

   void DCBColor()
   {
      const int u = m_width;

      for ( int row = 1; row < m_height-1; ++row, ++m_monitor )
         for ( int col = 1 + (FilterColor( row, 1 ) & 1),
                   indx = row*m_width + col,
                   c = 2 - FilterColor( row, col ); col < u-1; col += 2, indx += 2 )
         {
            m_image[indx][c] = RoundInt( Clip( (4*int( m_image[indx    ][1] )
                                                - int( m_image[indx+u+1][1] )
                                                - int( m_image[indx+u-1][1] )
                                                - int( m_image[indx-u+1][1] )
                                                - int( m_image[indx-u-1][1] )
                                                + int( m_image[indx+u+1][c] )
                                                + int( m_image[indx+u-1][c] )
                                                + int( m_image[indx-u+1][c] )
                                                + int( m_image[indx-u-1][c] ))/4.0 ) );
         }

      for ( int row = 1; row < m_height-1; ++row, ++m_monitor )
         for ( int col = 1 + (FilterColor( row, 2 ) & 1),
                   indx = row*m_width + col,
                   c = FilterColor( row, col+1 ),
                   d = 2-c; col < m_width-1; col += 2, indx += 2 )
         {
            m_image[indx][c] = RoundInt( Clip( (2*int( m_image[indx  ][1] )
                                                - int( m_image[indx+1][1] )
                                                - int( m_image[indx-1][1] )
                                                + int( m_image[indx+1][c] )
                                                + int( m_image[indx-1][c] ))/2.0 ) );
            m_image[indx][d] = RoundInt( Clip( (2*int( m_image[indx  ][1] )
                                                - int( m_image[indx+u][1] )
                                                - int( m_image[indx-u][1] )
                                                + int( m_image[indx+u][d] )
                                                + int( m_image[indx-u][d] ))/2.0 ) );
         }
   }

   void FBDDCorrection()
   {
      const int u = m_width;
      for ( int row = 2; row < m_height-2; ++row, ++m_monitor )
         for ( int col = 2, indx = row*m_width + col; col < m_width-2; col++, indx++ )
         {
            int c = FilterColor( row, col );
            m_image[indx][c] = Limit( m_image[indx][c],
                                      Max( m_image[indx-1][c],
                                           Max( m_image[indx+1][c],
                                                Max( m_image[indx-u][c],
                                                     m_image[indx+u][c] ) ) ),
                                      Min( m_image[indx-1][c],
                                           Min( m_image[indx+1][c],
                                                Min( m_image[indx-u][c],
                                                     m_image[indx+u][c] ) ) ) );
         }
   }

   void RGBToLCH( double (*image2)[3] )
   {
      for ( int indx = 0; indx < m_height*m_width; ++indx, ++m_monitor )
      {
         image2[indx][0] = m_image[indx][0] + m_image[indx][1] + m_image[indx][2];     // L
         image2[indx][1] = 1.732050808*(m_image[indx][0] - m_image[indx][1]);          // C
         image2[indx][2] = 2.0*m_image[indx][2] - m_image[indx][0] - m_image[indx][1]; // H
      }
   }

   void LCHToRGB( double (*image2)[3] )
   {
      for ( int indx = 0; indx < m_height*m_width; ++indx, ++m_monitor )
      {
         m_image[indx][0] = RoundInt( Clip( image2[indx][0]/3 - image2[indx][2]/6 + image2[indx][1]/3.464101615 ) );
         m_image[indx][1] = RoundInt( Clip( image2[indx][0]/3 - image2[indx][2]/6 - image2[indx][1]/3.464101615 ) );
         m_image[indx][2] = RoundInt( Clip( image2[indx][0]/3 + image2[indx][2]/3 ) );
      }
   }

   void FBDDCorrection2( double (*image2)[3] )
   {
      const int v = 2*m_width;
      for ( int row = 6; row < m_height-6; ++row, ++m_monitor )
      {
         for ( int col = 6; col < m_width-6; ++col )
         {
            int indx = row*m_width + col;
            if ( image2[indx][1]*image2[indx][2] != 0 )
            {
               double Co = (image2[indx+v][1] + image2[indx-v][1] + image2[indx-2][1] + image2[indx+2][1] -
                              Max( image2[indx-2][1],
                                   Max( image2[indx+2][1],
                                        Max( image2[indx-v][1],
                                             image2[indx+v][1] ) ) ) -
                              Min( image2[indx-2][1],
                                   Min( image2[indx+2][1],
                                        Min( image2[indx-v][1],
                                             image2[indx+v][1] ) ) ))/2;
               double Ho = (image2[indx+v][2] + image2[indx-v][2] + image2[indx-2][2] + image2[indx+2][2] -
                              Max( image2[indx-2][2],
                                   Max( image2[indx+2][2],
                                        Max( image2[indx-v][2],
                                             image2[indx+v][2] ) ) ) -
                              Min( image2[indx-2][2],
                                   Min( image2[indx+2][2],
                                        Min( image2[indx-v][2],
                                             image2[indx+v][2] ) ) ))/2;
               double ratio = Sqrt( (Co*Co + Ho*Ho)/(image2[indx][1]*image2[indx][1] + image2[indx][2]*image2[indx][2]) );
               if ( ratio < 0.85 )
               {
                  image2[indx][0] = -(image2[indx][1] + image2[indx][2] - Co - Ho) + image2[indx][0];
                  image2[indx][1] = Co;
                  image2[indx][2] = Ho;
               }
            }
         }
      }
   }
};

// ----------------------------------------------------------------------------

/*
 * X-Trans demosaicing engine.
 * Based on code from LibRaw version 0.19.0 - Copyright 2008-2018 LibRaw LLC.
 * Based on code from dcraw.c version 9.28 - Copyright 1997-2018 Dave Coffin.
 */
class XTransInterpolationEngine
{
public:

   XTransInterpolationEngine( const FMatrix& cameraTosRGB, const IMatrix& filters )
      : m_filters( filters )
   {
      // Validate X-Trans CFA pattern.
      int n[ 4 ] = { 0 };
      for ( int row = 0; row < 6; ++row )
         for ( int col = 0; col < 6; ++col )
            ++n[FilterColor( row, col )];
      if ( n[0] <  6 || n[0] > 10 ||
           n[1] < 16 || n[1] > 24 ||
           n[2] <  6 || n[2] > 10 ||
           n[3] >  0 )
         throw Error( "X-Trans interpolation: Invalid X-Trans CFA pattern" );

      InitXYZLUT();

      m_cameraToXYZ = FMatrix( .0F, 3, cameraTosRGB.Columns() );
      for ( int i = 0; i < 3; ++i )
         for ( int j = 0; j < cameraTosRGB.Columns(); ++j )
            for ( int k = 0; k < 3; ++k )
               m_cameraToXYZ[i][j] += xyz_rgb[i][k] * cameraTosRGB[k][j]/d65_white[i];
   }

   void Interpolate( Image& output, const ImageVariant& source, int passes )
   {
      if ( source.IsFloatSample() )
         switch ( source.BitsPerSample() )
         {
         case 32: Interpolate( output, static_cast<const Image&>( *source ), passes ); break;
         case 64: Interpolate( output, static_cast<const DImage&>( *source ), passes ); break;
         }
      else
         switch ( source.BitsPerSample() )
         {
         case  8: Interpolate( output, static_cast<const UInt8Image&>( *source ), passes ); break;
         case 16: Interpolate( output, static_cast<const UInt16Image&>( *source ), passes ); break;
         case 32: Interpolate( output, static_cast<const UInt32Image&>( *source ), passes ); break;
         }
   }

private:

          uint16       (*m_image)[ 4 ] = nullptr;
          int            m_width = 0;
          int            m_height = 0;
          int            m_passes = 0;
          int            m_ndir = 0;
          StatusMonitor  m_monitor;
          IMatrix        m_filters;
          short          m_allhex[ 3 ][ 3 ][ 2 ][ 8 ];
          uint16         m_sgrow;
          uint16         m_sgcol;
          FMatrix        m_cameraToXYZ;
   static FVector        m_xyzLUT;

   template <class P>
   void Interpolate( Image& output, const GenericImage<P>& source, int passes )
   {
      m_width = source.Width();
      m_height = source.Height();
      m_passes = passes;
      m_ndir = 4 << (m_passes > 1);

      if ( m_width < TS || m_height < TS )
         throw Error( String().Format( "X-Trans interpolation: Too small image (%dx%d px minimum required, the image is %dx%d)",
                                       TS, TS, m_width, m_height ) );

      StandardStatus status;
      m_monitor.SetCallback( &status );
      m_monitor.Initialize( "X-Trans demosaicing", size_type( m_height-3-19 )/(TS - 16)
                                                 * size_type( m_width-3-19 )/(TS - 16)
                                                 * (4 + 4*m_passes) );

      WordArray data( size_type( m_width ) * size_type( m_height ) * 4, uint16( 0 ) );
      m_image = reinterpret_cast<uint16 (*)[4]>( data.Begin() );
      if ( source.NumberOfNominalChannels() == 1 )
      {
         typename GenericImage<P>::const_sample_iterator i( source );
         uint16 (*p)[4] = m_image;
         for ( int row = 0; row < m_height; ++row )
            for ( int col = 0; col < m_width; ++col, ++i, ++p )
               p[0][FilterColor( row, col )] = UInt16PixelTraits::ToSample( *i );
      }
      else
      {
         typename GenericImage<P>::const_pixel_iterator i( source );
         uint16 (*p)[4] = m_image;
         for ( int row = 0; row < m_height; ++row )
            for ( int col = 0; col < m_width; ++col, ++i, ++p )
            {
               int c = FilterColor( row, col );
               p[0][c] = UInt16PixelTraits::ToSample( i[c] );
            }
      }

      InterpolateXTrans();

      output.AllocateData( m_width, m_height, 3, ColorSpace::RGB );
      {
         Image::pixel_iterator i( output );
         const uint16* p = m_image[0];
         for ( int row = 0; row < m_height; ++row )
            for ( int col = 0; col < m_width; ++col, ++i, ++p )
               for ( int c = 0; c < 3; ++c, ++p )
                  UInt16PixelTraits::FromSample( i[c], *p );
      }

      m_monitor.Complete();
   }

   static constexpr int TS = 512; // tile size

   static constexpr int Clip( int x )
   {
      return Range( x, 0, 65535 );
   }

   static const double xyz_rgb[ 3 ][ 3 ];
   static const double d65_white[ 3 ];

   void InitXYZLUT() const
   {
      static AtomicInt xyzLUTInitialized;
      if ( !xyzLUTInitialized )
      {
         static Mutex mutex;
         volatile AutoLock lock( mutex );
         if ( xyzLUTInitialized.Load() == 0 )
         {
            m_xyzLUT = FVector( 0x10000 );
            for ( int i = 0; i < 0x10000; ++i )
            {
               double r = i/65535.0;
               m_xyzLUT[i] = float( (r > 0.008856) ? Pow( r, 1.0/3 ) : 7.787*r + 16.0/116 );
            }
            xyzLUTInitialized.Store( 1 );
         }
      }
   }

   void RGBToCIELab( short lab[ 3 ], const uint16 rgb[ 3 ] ) const
   {
      FVector xyz( 0.5F, 3 );
      for ( int c = 0; c < m_cameraToXYZ.Columns(); ++c )
      {
         xyz[0] += m_cameraToXYZ[0][c] * rgb[c];
         xyz[1] += m_cameraToXYZ[1][c] * rgb[c];
         xyz[2] += m_cameraToXYZ[2][c] * rgb[c];
      }
      xyz[0] = m_xyzLUT[Clip( (int)xyz[0] )];
      xyz[1] = m_xyzLUT[Clip( (int)xyz[1] )];
      xyz[2] = m_xyzLUT[Clip( (int)xyz[2] )];
      lab[0] = 64 * (116 * xyz[1] - 16);
      lab[1] = 64 * 500 * (xyz[0] - xyz[1]);
      lab[2] = 64 * 200 * (xyz[1] - xyz[2]);
   }

   int FilterColor( int row, int col ) const
   {
      return m_filters[(row+6) % 6][(col+6) % 6];
   }

   void InterpolateBorder( int border )
   {
      for ( int row = 0; row < m_height; ++row )
         for ( int col = 0; col < m_width; ++col )
         {
            if ( col == border && row >= border && row < m_height - border )
               col = m_width - border;
            double sum[ 4 ] = { 0 };
            int count[ 4 ] = { 0 };
            for ( int y = row - 1; y != row + 2; ++y )
               for ( int x = col - 1; x != col + 2; ++x )
                  if ( y >= 0 && x >= 0 && y < m_height && x < m_width )
                  {
                     int f = FilterColor( y, x );
                     sum[f] += m_image[y*m_width + x][f];
                     ++count[f];
                  }
            int f = FilterColor( row, col );
            for ( int c = 0; c < 3; ++c )
               if ( c != f )
                  if ( count[c] > 0 )
                     m_image[row*m_width + col][c] = RoundInt( sum[c]/count[c] );
         }
   }

   /*
    * Frank Markesteijn's algorithm for Fuji X-Trans sensors.
    * Adapted from dcraw.c version 9.28 - Copyright 1997-2018 by Dave Coffin.
    */
   void InterpolateXTrans()
   {
      static const short
      orth[ 12 ]       = { 1, 0, 0, 1, -1, 0, 0, -1, 1, 0, 0, 1 },
      patt[  2 ][ 16 ] = { { 0, 1, 0, -1, 2, 0, -1, 0, 1, 1,  1, -1, 0,  0,  0, 0 },
                           { 0, 1, 0, -2, 1, 0, -2, 0, 1, 1, -2, -2, 1, -1, -1, 1 } };

      /*
       * Map a green hexagon around each non-green pixel and vice versa
       */
      // Init m_allhex table to unreasonable values
      for ( int i = 0; i < 3; i++ )
         for ( int j = 0; j < 3; j++ )
            for ( int k = 0; k < 2; k++ )
               for ( int l = 0; l < 8; l++ )
                  m_allhex[i][j][k][l] = 32700;
      m_sgrow = m_sgcol = 0;
      {
         int minv = 0, maxv = 0, minh = 0, maxh = 0;
         for ( int row = 0; row < 3; row++ )
            for ( int col = 0; col < 3; col++ )
               for ( int d = 0, ng = 0; d < 10; d += 2 )
               {
                  int g = FilterColor( row, col ) == 1;
                  if ( FilterColor( row + orth[d], col + orth[d + 2] ) == 1 )
                     ng = 0;
                  else
                     ng++;
                  if ( ng == 4 )
                  {
                     m_sgrow = row;
                     m_sgcol = col;
                  }
                  if ( ng == g + 1 )
                     for ( int c = 0; c < 8; ++c )
                     {
                        int v = orth[d] * patt[g][c * 2] + orth[d + 1] * patt[g][c * 2 + 1];
                        int h = orth[d + 2] * patt[g][c * 2] + orth[d + 3] * patt[g][c * 2 + 1];
                        minv = Min( v, minv );
                        maxv = Max( v, maxv );
                        minh = Min( v, minh );
                        maxh = Max( v, maxh );
                        m_allhex[row][col][0][c ^ (g * 2 & d)] = h + v * m_width;
                        m_allhex[row][col][1][c ^ (g * 2 & d)] = h + v * TS;
                     }
               }

         // Check m_allhex table initialization
         for ( int i = 0; i < 3; i++ )
            for ( int j = 0; j < 3; j++ )
               for ( int k = 0; k < 2; k++ )
                  for ( int l = 0; l < 8; l++ )
                     if ( m_allhex[i][j][k][l] > maxh + maxv*m_width + 1 || m_allhex[i][j][k][l] < minh + minv*m_width - 1 )
                        throw Error( "X-Trans interpolation: Invalid CFA data" );
      }

      /*
       * Set green1 and green3 to the minimum and maximum allowed values.
       */
      for ( int row = 2, retrycount = 0; row < m_height - 2; row++ )
      {
         uint16 min = ~0, max = 0;
         for ( int col = 2; col < m_width - 2; col++ )
         {
            if ( FilterColor( row, col ) == 1 && (min = ~(max = 0)) )
               continue;
            uint16 (*pix)[4] = m_image + row * m_width + col;
            const short* hex = m_allhex[row % 3][col % 3][0];
            if ( !max )
               for ( int c = 0; c < 6; ++c )
               {
                  uint16 val = pix[hex[c]][1];
                  if ( min > val )
                     min = val;
                  if ( max < val )
                     max = val;
               }
            pix[0][1] = min;
            pix[0][3] = max;
            switch ( (row - m_sgrow) % 3 )
            {
            case 1:
               if ( row < m_height - 3 )
               {
                  row++;
                  col--;
               }
               break;
            case 2:
               if ( (min = ~(max = 0)) && (col += 2) < m_width - 3 && row > 2 )
               {
                  row--;
                  if ( retrycount++ > m_width*m_height )
                     throw Error( "X-Trans interpolation: Invalid CFA data" );
               }
               break;
            }
         }
      }

      int numberOfTiles = ((m_height - 3)/(TS - 16) + ((m_height - 3)%(TS - 16) != 0))
                        * ((m_width - 3)/(TS - 16) + ((m_width - 3)%(TS - 16) != 0));
      Array<size_type> L = Thread::OptimalThreadLoads( numberOfTiles );
      ReferenceArray<XTransThread> threads;
      AbstractImage::ThreadData data( m_monitor, numberOfTiles * (4 + 4*m_passes) );
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new XTransThread( *this, data, n, n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      InterpolateBorder( 8 );
   }

   class XTransThread : public Thread
   {
   public:

      XTransThread( XTransInterpolationEngine& engine,
                    const AbstractImage::ThreadData& data, int startTile, int endTile )
         : m_engine( engine )
         , m_data( data )
         , m_startTile( startTile )
         , m_endTile( endTile )
      {
      }


#ifdef SQR
#undef SQR
#endif
      template <typename T> static constexpr T SQR( const T& x )
      {
         return x*x;
      }

      void Run() override
      {
#define m_image  m_engine.m_image
#define m_width  m_engine.m_width
#define m_height m_engine.m_height
#define m_passes m_engine.m_passes
#define m_ndir   m_engine.m_ndir
#define m_allhex m_engine.m_allhex
#define m_sgrow  m_engine.m_sgrow
#define m_sgcol  m_engine.m_sgcol

         static const short dir[ 4 ] = { 1, TS, TS + 1, TS - 1 };

         INIT_THREAD_MONITOR()

         ByteArray buffer( TS * TS * (m_ndir * 11 + 6) );

         uint16 (*rgb)[ TS ][ TS ][ 3 ] = (uint16(*)[ TS ][ TS ][ 3 ])buffer.Begin();
         short (*lab)[ TS ][ 3 ] = (short(*)[ TS ][ 3 ])buffer.At( TS * TS * (m_ndir * 6) );
         float (*drv)[ TS ][ TS ] = (float(*)[ TS ][ TS ])buffer.At( TS * TS * (m_ndir * 6 + 6) );
         char (*homo)[ TS ][ TS ] = (char(*)[ TS ][ TS ])buffer.At( TS * TS * (m_ndir * 10 + 6) );

         int tilesPerRow = (m_width - 3)/(TS - 16) + ((m_width - 3)%(TS - 16) != 0);
         for ( int tile = m_startTile; tile < m_endTile; ++tile )
         {
            int top = 3 + (TS - 16)*(tile/tilesPerRow);
            if ( top >= m_height - 19 )
               continue;
            int left = 3 + (TS - 16)*(tile%tilesPerRow);
            if ( left >= m_width - 19 )
               continue;

            int mrow = Min( top + TS, m_height - 3 );
            int mcol = Min( left + TS, m_width - 3 );
            for ( int row = top; row < mrow; row++ )
               for ( int col = left; col < mcol; col++ )
                  memcpy( rgb[0][row - top][col - left], m_image[row * m_width + col], 6 );
            for ( int c = 0; c < 3; ++c )
               memcpy( rgb[c+1], rgb[0], sizeof( *rgb ) );
            int color[3][8];

            /*
             * Interpolate green horizontally, vertically, and along both diagonals.
             */
            for ( int row = top; row < mrow; row++ )
               for ( int col = left; col < mcol; col++ )
               {
                  int f = m_engine.FilterColor( row, col );
                  if ( f == 1 )
                     continue;
                  const uint16 (*pix)[4] = m_image + row * m_width + col;
                  const short* hex = m_allhex[row % 3][col % 3][0];
                  color[1][0] = 174 * (pix[hex[1]][1] + pix[hex[0]][1]) - 46 * (pix[2 * hex[1]][1] + pix[2 * hex[0]][1]);
                  color[1][1] = 223 * pix[hex[3]][1] + pix[hex[2]][1] * 33 + 92 * (pix[0][f] - pix[-hex[2]][f]);
                  for ( int c = 0; c < 2; ++c )
                     color[1][2+c] = 164 * pix[hex[4+c]][1] + 92 * pix[-2 * hex[4+c]][1] +
                                    33 * (2 * pix[0][f] - pix[3 * hex[4+c]][f] - pix[-3 * hex[4+c]][f]);
                  for ( int c = 0; c < 4; ++c )
                     rgb[c ^ !((row - m_sgrow) % 3)][row - top][col - left][1] = Range( uint16( color[1][c] >> 8 ), pix[0][1], pix[0][3] );
               }

            UPDATE_THREAD_MONITOR( 1 )

            for ( int pass = 0; pass < m_passes; pass++ )
            {
               if ( pass == 1 )
                  memcpy( rgb += 4, buffer.Begin(), 4 * sizeof( *rgb ) );

               /*
                * Recalculate green from interpolated values of closer pixels.
                */
               if ( pass > 0 )
               {
                  for ( int row = top + 2; row < mrow - 2; row++ )
                     for ( int col = left + 2; col < mcol - 2; col++ )
                     {
                        int f = m_engine.FilterColor( row, col );
                        if ( f == 1 )
                           continue;
                        const uint16 (*pix)[4] = m_image + row * m_width + col;
                        const short* hex = m_allhex[row % 3][col % 3][1];
                        for ( int d = 3; d < 6; d++ )
                        {
                           uint16 (*rix)[3] = &rgb[(d - 2) ^ !((row - m_sgrow) % 3)][row - top][col - left];
                           uint16 val = rix[-2 * hex[d]][1] + 2 * rix[hex[d]][1] - rix[-2 * hex[d]][f] - 2 * rix[hex[d]][f] + 3 * rix[0][f];
                           rix[0][1] = Range( uint16( val/3 ), pix[0][1], pix[0][3] );
                        }
                     }
               }

               UPDATE_THREAD_MONITOR( 1 )

               /*
                * Interpolate red and blue values for solitary green pixels.
                */
               for ( int row = (top - m_sgrow + 4) / 3 * 3 + m_sgrow; row < mrow - 2; row += 3 )
                  for ( int col = (left - m_sgcol + 4) / 3 * 3 + m_sgcol; col < mcol - 2; col += 3 )
                  {
                     uint16 (*rix)[3] = &rgb[0][row - top][col - left];
                     int h = m_engine.FilterColor( row, col + 1 );
                     float diff[ 6 ];
                     memset( diff, 0, sizeof( diff ) );
                     for ( int i = 1, d = 0; d < 6; d++, i ^= TS ^ 1, h ^= 2 )
                     {
                        for ( int c = 0; c < 2; c++, h ^= 2 )
                        {
                           int g = 2 * rix[0][1] - rix[i << c][1] - rix[-i << c][1];
                           color[h][d] = g + rix[i << c][h] + rix[-i << c][h];
                           if ( d > 1 )
                              diff[d] += SQR( rix[i << c][1] - rix[-i << c][1] - rix[i << c][h] + rix[-i << c][h] ) + SQR( g );
                        }
                        if ( d > 1 && (d & 1) )
                           if ( diff[d - 1] < diff[d] )
                              for ( int c = 0; c < 2; ++c )
                                 color[c*2][d] = color[c*2][d-1];
                        if ( d < 2 || (d & 1) )
                        {
                           for ( int c = 0; c < 2; ++c )
                              rix[0][c*2] = Clip( color[c*2][d] / 2 );
                           rix += TS * TS;
                        }
                     }
                  }

               UPDATE_THREAD_MONITOR( 1 )

               /*
                * Interpolate red for blue pixels and vice versa.
                */
               for ( int row = top + 3; row < mrow - 3; row++ )
                  for ( int col = left + 3; col < mcol - 3; col++ )
                  {
                     int f = 2 - m_engine.FilterColor( row, col );
                     if ( f == 1 )
                        continue;
                     uint16 (*rix)[3] = &rgb[0][row - top][col - left];
                     int c = (row - m_sgrow) % 3 ? TS : 1;
                     int h = 3 * (c ^ TS ^ 1);
                     for ( int d = 0; d < 4; d++, rix += TS * TS )
                     {
                        int i = d > 1 || ((d ^ c) & 1) ||
                                    ((Abs( rix[0][1] - rix[c][1] ) + Abs( rix[0][1] - rix[-c][1] )) <
                                       2 * (Abs( rix[0][1] - rix[h][1] ) + Abs( rix[0][1] - rix[-h][1] )))
                                 ? c
                                 : h;
                        rix[0][f] = Clip( (rix[i][f] + rix[-i][f] + 2 * rix[0][1] - rix[i][1] - rix[-i][1]) / 2 );
                     }
                  }

               UPDATE_THREAD_MONITOR( 1 )

               /*
                * Fill in red and blue for 2x2 blocks of green.
                */
               for ( int row = top + 2; row < mrow - 2; row++ )
                  if ( (row - m_sgrow) % 3 )
                     for ( int col = left + 2; col < mcol - 2; col++ )
                        if ( (col - m_sgcol) % 3 )
                        {
                           uint16 (*rix)[3] = &rgb[0][row - top][col - left];
                           const short* hex = m_allhex[row % 3][col % 3][1];
                           for ( int d = 0; d < m_ndir; d += 2, rix += TS * TS )
                              if ( hex[d] + hex[d + 1] )
                              {
                                 int g = 3 * rix[0][1] - 2 * rix[hex[d]][1] - rix[hex[d + 1]][1];
                                 for ( int c = 0; c < 4; c += 2 )
                                    rix[0][c] = Clip( (g + 2 * rix[hex[d]][c] + rix[hex[d + 1]][c]) / 3 );
                              }
                              else
                              {
                                 int g = 2 * rix[0][1] - rix[hex[d]][1] - rix[hex[d + 1]][1];
                                 for ( int c = 0; c < 4; c += 2 )
                                    rix[0][c] = Clip( (g + rix[hex[d]][c] + rix[hex[d + 1]][c]) / 2 );
                              }
                        }

               UPDATE_THREAD_MONITOR( 1 )
            }
            rgb = (uint16(*)[ TS ][ TS ][ 3 ])buffer.Begin();
            mrow -= top;
            mcol -= left;

            /*
             * Convert to CIELab and differentiate in all directions.
             */
            for ( int d = 0; d < m_ndir; d++ )
            {
               for ( int row = 2; row < mrow - 2; row++ )
                  for ( int col = 2; col < mcol - 2; col++ )
                     m_engine.RGBToCIELab( lab[row][col], rgb[d][row][col] );
               for ( int f = dir[d & 3], row = 3; row < mrow - 3; row++ )
                  for ( int col = 3; col < mcol - 3; col++ )
                  {
                     short (*lix)[ 3 ] = &lab[row][col];
                     int g = 2 * lix[0][0] - lix[f][0] - lix[-f][0];
                     drv[d][row][col] = SQR( g ) + SQR( (2 * lix[0][1] - lix[f][1] - lix[-f][1] + g * 500 / 232) ) +
                                       SQR( (2 * lix[0][2] - lix[f][2] - lix[-f][2] - g * 500 / 580) );
                  }

            }

            UPDATE_THREAD_MONITOR( 1 )

            /*
             * Build homogeneity maps from the derivatives.
             */
            memset( homo, 0, m_ndir * TS * TS );
            for ( int row = 4; row < mrow - 4; row++ )
               for ( int col = 4; col < mcol - 4; col++ )
               {
                  float tr = FLT_MAX;
                  for ( int d = 0; d < m_ndir; d++ )
                     if ( tr > drv[d][row][col] )
                        tr = drv[d][row][col];
                  tr *= 8;
                  for ( int d = 0; d < m_ndir; d++ )
                     for ( int v = -1; v <= 1; v++ )
                        for ( int h = -1; h <= 1; h++ )
                           if ( drv[d][row + v][col + h] <= tr )
                              homo[d][row][col]++;
               }

            UPDATE_THREAD_MONITOR( 1 )

            /*
             * Average the most homogenous pixels for the final result.
             */
            if ( m_height - top < TS + 4 )
               mrow = m_height - top + 2;
            if ( m_width - left < TS + 4 )
               mcol = m_width - left + 2;
            for ( int row = Min( top, 8 ); row < mrow - 8; row++ )
               for ( int col = Min( left, 8 ); col < mcol - 8; col++ )
               {
                  int hm[ 8 ];
                  for ( int d = 0; d < m_ndir; d++ )
                  {
                     hm[d] = 0;
                     for ( int v = -2; v <= 2; v++ )
                        for ( int h = -2; h <= 2; h++ )
                           hm[d] += homo[d][row + v][col + h];
                  }
                  for ( int d = 0; d < m_ndir - 4; d++ )
                     if ( hm[d] < hm[d + 4] )
                        hm[d] = 0;
                     else if ( hm[d] > hm[d + 4] )
                        hm[d + 4] = 0;
                  int max = hm[0];
                  for ( int d = 1; d < m_ndir; d++ )
                     if ( max < hm[d] )
                        max = hm[d];
                  max -= max >> 3;
                  int avg[ 4 ] = { 0 };
                  for ( int d = 0; d < m_ndir; d++ )
                     if ( hm[d] >= max )
                     {
                        for ( int c = 0; c < 3; ++c )
                           avg[c] += rgb[d][row][col][c];
                        avg[3]++;
                     }
                  for ( int c = 0; c < 3; ++c )
                     m_image[(row + top)*m_width + col + left][c] = avg[c]/avg[3];
               }

            UPDATE_THREAD_MONITOR( 1 )
         }
#undef m_image
#undef m_width
#undef m_height
#undef m_passes
#undef m_ndir
#undef m_allhex
#undef m_sgrow
#undef m_sgcol
      }

   private:

            XTransInterpolationEngine& m_engine;
      const AbstractImage::ThreadData& m_data;
            int                        m_startTile, m_endTile;
   };
};

const double XTransInterpolationEngine::xyz_rgb[ 3 ][ 3 ] =
{
   { 0.4124564, 0.3575761, 0.1804375 },
   { 0.2126729, 0.7151522, 0.0721750 },
   { 0.0193339, 0.1191920, 0.9503041 }
};

const double XTransInterpolationEngine::d65_white[ 3 ] =
{
   0.95047, 1.0, 1.08883
};

FVector XTransInterpolationEngine::m_xyzLUT;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DebayerFileThread : public Thread
{
public:

   DebayerFileThread( const DebayerInstance& instance, size_type index )
      : m_instance( instance )
      , m_index( index )
      , m_targetFilePath( m_instance.p_targets[m_index].path )
   {
   }

   void Run() override
   {
      try
      {
         Module->ProcessEvents();

         if ( !ReadInputData() )
            return;

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

   void GetOutputData( DebayerInstance::OutputFileData& o ) const
   {
      o.filePath = m_outputFilePath;
      o.channelFilePaths = m_outputChannelFilePaths;
      if ( m_instance.p_evaluateSignal )
      {
         o.psfTotalFluxEstimates = m_psfTotalFluxEstimates;
         o.psfTotalPowerFluxEstimates = m_psfTotalPowerFluxEstimates;
         o.psfTotalMeanFluxEstimates = m_psfTotalMeanFluxEstimates;
         o.psfTotalMeanPowerFluxEstimates = m_psfTotalMeanPowerFluxEstimates;
         o.psfMStarEstimates = m_psfMStarEstimates;
         o.psfNStarEstimates = m_psfNStarEstimates;
         o.psfCounts = m_psfCounts;
      }
      if ( m_instance.p_evaluateNoise )
      {
         o.noiseEstimates = m_noiseEstimates;
         o.noiseFractions = m_noiseFractions;
         o.noiseScaleLow = m_noiseScaleLow;
         o.noiseScaleHigh = m_noiseScaleHigh;
         o.noiseAlgorithms = m_noiseAlgorithms;
      }
   }

   size_type Index() const
   {
      return m_index;
   }

   String TargetFilePath() const
   {
      return m_targetFilePath;
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

   const DebayerInstance& m_instance;
         size_type        m_index;
         String           m_targetFilePath;
         OutputFileData   m_fileData;
         ImageVariant     m_targetImage;
         bool             m_xtrans;
         IsoString        m_patternId;
         pcl_enum         m_bayerPattern;
         FMatrix          m_sRGBConversionMatrix;
         IMatrix          m_xtransPatternFilters;
         Vector           m_psfTotalFluxEstimates = Vector( 0.0, 3 );
         Vector           m_psfTotalPowerFluxEstimates = Vector( 0.0, 3 );
         Vector           m_psfTotalMeanFluxEstimates = Vector( 0.0, 3 );
         Vector           m_psfTotalMeanPowerFluxEstimates = Vector( 0.0, 3 );
         Vector           m_psfMStarEstimates = Vector( 0.0, 3 );
         Vector           m_psfNStarEstimates = Vector( 0.0, 3 );
         IVector          m_psfCounts = IVector( 0, 3 );
         Vector           m_noiseEstimates = Vector( 0.0, 3 );
         Vector           m_noiseFractions = Vector( 0.0, 3 );
         Vector           m_noiseScaleLow = Vector( 0.0, 3 );
         Vector           m_noiseScaleHigh = Vector( 0.0, 3 );
         StringList       m_noiseAlgorithms = StringList( 3 );
         Image            m_outputImage;
         String           m_outputFilePath;
         StringList       m_outputChannelFilePaths = StringList( 3 );;
         String           m_errorInfo;
         bool             m_success = false;

   // -------------------------------------------------------------------------

   bool ReadInputData()
   {
      Console console;
      console.WriteLn( "<end><cbr>* Loading target file: <raw>" + m_targetFilePath + "</raw>" );

      FileFormat format( File::ExtractExtension( m_targetFilePath ), true/*read*/, false/*write*/ );
      FileFormatInstance file( format );

      ImageDescriptionArray images;
      if ( !file.Open( images, m_targetFilePath, m_instance.p_inputHints ) )
         throw CaughtException();

      if ( images.IsEmpty() )
      {
         console.NoteLn( "* Skipping empty image file." );
         if ( !file.Close() )
            throw CaughtException();
         return false;
      }

      if ( images.Length() > 1 )
         console.NoteLn( String().Format( "* Ignoring %u additional image(s) in target file.", images.Length()-1 ) );

      m_xtrans = DebayerInstance::IsXTransCFAFromTarget( file );
      m_patternId = m_instance.CFAPatternIdFromTarget( file, m_xtrans );

      if ( m_xtrans )
      {
         m_sRGBConversionMatrix = DebayerInstance::sRGBConversionMatrixFromTarget( file );
         m_xtransPatternFilters = DebayerInstance::XTransPatternFiltersFromTarget( file );
      }
      else
         m_bayerPattern = m_instance.BayerPatternFromTarget( file );

      console.Write( "<end><cbr>CFA pattern" );
      if ( m_xtrans || m_instance.p_bayerPattern == DebayerBayerPatternParameter::Auto )
         console.Write( " (detected)" );
      console.WriteLn( ": " + m_patternId );

      m_targetImage.CreateSharedImage( images[0].options.ieeefpSampleFormat,
                                       false/*isComplex*/,
                                       images[0].options.bitsPerSample );
      {
         static Mutex mutex;
         static AtomicInt count;
         volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileReadThreads );
         if ( !file.ReadImage( m_targetImage ) )
            throw CaughtException();
         m_fileData = OutputFileData( file, images[0].options );
         if ( !file.Close() )
            throw CaughtException();
      }

      return true;
   }

   // -------------------------------------------------------------------------

   void Perform()
   {
      if ( m_xtrans )
         XTransInterpolationEngine( m_sRGBConversionMatrix, m_xtransPatternFilters ).Interpolate( m_outputImage, m_targetImage, 2/*passes*/ );
      else
         DebayerEngine( m_outputImage, m_instance, m_bayerPattern ).Debayer( m_targetImage );

      if ( m_instance.p_evaluateNoise || m_instance.p_evaluateSignal )
         m_instance.EvaluateSignalAndNoise( m_psfTotalFluxEstimates, m_psfTotalPowerFluxEstimates,
                                            m_psfTotalMeanFluxEstimates, m_psfTotalMeanPowerFluxEstimates,
                                            m_psfMStarEstimates, m_psfNStarEstimates, m_psfCounts,
                                            m_noiseEstimates, m_noiseFractions, m_noiseScaleLow, m_noiseScaleHigh,
                                            m_noiseAlgorithms, m_targetImage, m_patternId );
   }

   // -------------------------------------------------------------------------

   void WriteOutputData()
   {
      String fileDir = m_instance.p_outputDirectory.Trimmed();
      if ( fileDir.IsEmpty() )
         fileDir = File::ExtractDrive( m_targetFilePath ) + File::ExtractDirectory( m_targetFilePath );
      if ( fileDir.IsEmpty() )
         throw Error( "Unable to determine an output directory: " + m_targetFilePath );
      if ( !fileDir.EndsWith( '/' ) )
         fileDir.Append( '/' );

      String fileName = File::ExtractName( m_targetFilePath ).Trimmed();
      if ( fileName.IsEmpty() )
         throw Error( "Unable to determine an output file name: " + m_targetFilePath );

      Console console;

      String fileExtension = m_instance.p_outputExtension.Trimmed();
      if ( fileExtension.IsEmpty() )
         fileExtension = File::ExtractExtension( m_targetFilePath ).Trimmed();
      if ( fileExtension.IsEmpty() )
         throw Error( "Unable to determine an output file extension: " + m_targetFilePath );
      if ( !fileExtension.StartsWith( '.' ) )
         fileExtension.Prepend( '.' );

      String outputFilePath = fileDir + m_instance.p_outputPrefix + fileName + m_instance.p_outputPostfix + fileExtension;

      FileFormat outputFormat( fileExtension, false/*read*/, true/*write*/ );

      if ( !outputFormat.CanStoreFloat() )
         console.WarningLn( "** Warning: The output format does not support the required sample data format." );

      if ( !outputFormat.CanStoreImageProperties() || !outputFormat.SupportsViewProperties() )
         console.WarningLn( "** Warning: The output format cannot store image properties - required properties not embedded" );

      if ( !outputFormat.CanStoreKeywords() )
         console.WarningLn( "** Warning: The output format cannot store FITS keywords - keywords not embedded" );

      if ( m_fileData.profile.IsProfile() )
         if ( !outputFormat.CanStoreICCProfiles() )
            console.WarningLn( "** Warning: The output format cannot store ICC profiles - existing profile not embedded" );

      ImageOptions options = m_fileData.options; // get resolution, etc.
      options.bitsPerSample = 32;
      options.ieeefpSampleFormat = true;

      IsoString methodId = m_xtrans ? "X-Trans" : DebayerEngine::MethodId( m_instance.p_debayerMethod );

      FITSKeywordArray keywords = m_fileData.keywords;

      /*
       * N.B.: If we already have computed signal or noise estimates (e.g.,
       * with the ImageCalibration process), remove any previously existing
       * related keywords. Only our newly created keywords must be present in
       * the header.
       */
      for ( size_type i = 0; i < keywords.Length(); )
      {
         bool remove = false;
         if ( keywords[i].name.StartsWithIC( "NOISE" ) )
            remove = true;
         else if ( keywords[i].name.StartsWithIC( "PSF" ) )
            for ( size_type j = 0; j < ItemsInArray( s_psfPrefixes ); ++j )
               if ( keywords[i].name.StartsWithIC( s_psfPrefixes[j] ) )
               {
                  remove = true;
                  break;
               }

         if ( remove )
            keywords.Remove( keywords.At( i ) );
         else
            ++i;
      }

      keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Demosaicing with "  + PixInsightVersion::AsString() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Demosaicing with "  + Module->ReadableVersion() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Demosaicing with Debayer process" )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Demosaic.pattern: " + m_patternId )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Demosaic.method: " + methodId );

      PropertyArray properties = m_fileData.properties;
      properties << Property( "PCL:CFASourceFilePath", m_targetFilePath )
                 << Property( "PCL:CFASourcePattern", m_patternId )
                 << Property( "PCL:CFASourceInterpolation", methodId );

      if ( m_instance.p_outputRGBImages )
      {
         m_outputFilePath = outputFilePath;

         console.WriteLn( "<end><cbr>Writing output file: <raw>" + m_outputFilePath + "</raw>" );

         UniqueFileChecks checks = File::EnsureNewUniqueFile( m_outputFilePath, m_instance.p_overwriteExistingFiles );
         if ( checks.overwrite )
            console.WarningLn( "** Warning: Overwriting existing file." );
         else if ( checks.exists )
            console.NoteLn( "* File already exists, writing to: <raw>" + m_outputFilePath + "</raw>" );

         FileFormatInstance outputFile( outputFormat );

         if ( !outputFile.Create( m_outputFilePath, m_instance.p_outputHints ) )
            throw CaughtException();

         outputFile.SetOptions( options );

         if ( m_fileData.fsData != nullptr )
            if ( outputFormat.UsesFormatSpecificData() )
               if ( outputFormat.ValidateFormatSpecificData( m_fileData.fsData ) )
                  outputFile.SetFormatSpecificData( m_fileData.fsData );

         if ( outputFormat.CanStoreImageProperties() )
            if ( outputFormat.SupportsViewProperties() )
               outputFile.WriteImageProperties( properties );

         if ( outputFormat.CanStoreKeywords() )
         {
            FITSKeywordArray keywordsRGB = keywords;

            if ( m_instance.p_evaluateSignal )
            {
               keywordsRGB << FITSHeaderKeyword( "HISTORY", IsoString(), "PSF signal evaluation with " + Module->ReadableVersion() )
                           << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalFluxEstimates: %.4e %.4e %.4e",
                                                            m_psfTotalFluxEstimates[0], m_psfTotalFluxEstimates[1], m_psfTotalFluxEstimates[2] ) )
                           << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalPowerFluxEstimates: %.4e %.4e %.4e",
                                                            m_psfTotalPowerFluxEstimates[0], m_psfTotalPowerFluxEstimates[1], m_psfTotalPowerFluxEstimates[2] ) )
                           << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalMeanFluxEstimates: %.4e %.4e %.4e",
                                                            m_psfTotalMeanFluxEstimates[0], m_psfTotalMeanFluxEstimates[1], m_psfTotalMeanFluxEstimates[2] ) )
                           << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalMeanPowerFluxEstimates: %.4e %.4e %.4e",
                                                            m_psfTotalMeanPowerFluxEstimates[0], m_psfTotalMeanPowerFluxEstimates[1], m_psfTotalMeanPowerFluxEstimates[2] ) )
                           << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfMStarEstimates: %.4e %.4e %.4e",
                                                            m_psfMStarEstimates[0], m_psfMStarEstimates[1], m_psfMStarEstimates[2] ) )
                           << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfNStarEstimates: %.4e %.4e %.4e",
                                                            m_psfNStarEstimates[0], m_psfNStarEstimates[1], m_psfNStarEstimates[2] ) )
                           << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfCounts: %d %d %d",
                                                            m_psfCounts[0], m_psfCounts[1], m_psfCounts[2] ) )
                           << FITSHeaderKeyword( "HISTORY", IsoString(), "Demosaic.psfType: "
                                                            + DebayerPSFType::FunctionName( m_instance.p_psfType ) );
               for ( int i = 0; i < 3; ++i )
                  keywordsRGB << FITSHeaderKeyword( IsoString().Format( "PSFFLX%02d", i ),
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

               keywordsRGB << FITSHeaderKeyword( "PSFSGTYP",
                                                 DebayerPSFType::FunctionName( m_instance.p_psfType ).SingleQuoted(),
                                                 "PSF type used for signal estimation" );
            }

            if ( m_instance.p_evaluateNoise )
            {
               keywordsRGB << FITSHeaderKeyword( "HISTORY", IsoString(), "Noise evaluation with " + Module->ReadableVersion() )
                           << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.noiseEstimates: %.4e %.4e %.4e",
                                                            m_noiseEstimates[0], m_noiseEstimates[1], m_noiseEstimates[2] ) )
                           << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.noiseScaleLow: %.6e %.6e %.6e",
                                                            m_noiseScaleLow[0], m_noiseScaleLow[1], m_noiseScaleLow[2] ) )
                           << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.noiseScaleHigh: %.6e %.6e %.6e",
                                                            m_noiseScaleHigh[0], m_noiseScaleHigh[1], m_noiseScaleHigh[2] ) );
               for ( int i = 0; i < 3; ++i )
                  keywordsRGB << FITSHeaderKeyword( IsoString().Format( "NOISE%02d", i ),
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
                                                    m_noiseAlgorithms[i],
                                                    IsoString().Format( "Noise evaluation algorithm, channel #%d", i ) );
            }

            outputFile.WriteFITSKeywords( keywordsRGB );
         }

         if ( m_fileData.profile.IsProfile() )
            if ( outputFormat.CanStoreICCProfiles() )
               outputFile.WriteICCProfile( m_fileData.profile );

         Module->ProcessEvents();

         {
            static Mutex mutex;
            static AtomicInt count;
            volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileWriteThreads );
            if ( !outputFile.WriteImage( m_outputImage ) || !outputFile.Close() )
               throw CaughtException();
         }
      }

      if ( m_instance.p_outputSeparateChannels )
         for ( int i = 0; i < 3; ++i )
         {
            m_outputChannelFilePaths[i] = File::AppendToName( outputFilePath, String( '_' ) + "RGB"[i] );

            console.WriteLn( "<end><cbr>Writing output file: <raw>" + m_outputChannelFilePaths[i] + "</raw>" );

            UniqueFileChecks checks = File::EnsureNewUniqueFile( m_outputChannelFilePaths[i], m_instance.p_overwriteExistingFiles );
            if ( checks.overwrite )
               console.WarningLn( "** Warning: Overwriting existing file." );
            else if ( checks.exists )
               console.NoteLn( "* File already exists, writing to: <raw>" + m_outputChannelFilePaths[i] + "</raw>" );

            FileFormatInstance outputFile( outputFormat );

            if ( !outputFile.Create( m_outputChannelFilePaths[i], m_instance.p_outputHints ) )
               throw CaughtException();

            outputFile.SetOptions( options );

            if ( m_fileData.fsData != nullptr )
               if ( outputFormat.UsesFormatSpecificData() )
                  if ( outputFormat.ValidateFormatSpecificData( m_fileData.fsData ) )
                     outputFile.SetFormatSpecificData( m_fileData.fsData );

            if ( outputFormat.CanStoreImageProperties() )
               if ( outputFormat.SupportsViewProperties() )
               {
                  outputFile.WriteImageProperties( properties );
                  outputFile.WriteImageProperty( "PCL:CFASourceChannel", i );
               }

            if ( outputFormat.CanStoreKeywords() )
            {
               FITSKeywordArray keywordsChn = keywords;

               if ( m_instance.p_evaluateSignal )
                  keywordsChn << FITSHeaderKeyword( "HISTORY", IsoString(), "PSF signal evaluation with " + Module->ReadableVersion() )
                              << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalFluxEstimates: %.4e",
                                                                                                m_psfTotalFluxEstimates[i] ) )
                              << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalPowerFluxEstimates: %.4e",
                                                                                                m_psfTotalPowerFluxEstimates[i] ) )
                              << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalMeanFluxEstimates: %.4e",
                                                                                                m_psfTotalMeanFluxEstimates[i] ) )
                              << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalMeanPowerFluxEstimates: %.4e",
                                                                                                m_psfTotalMeanPowerFluxEstimates[i] ) )
                              << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfMStarEstimates: %.4e",
                                                                                                m_psfMStarEstimates[i] ) )
                              << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfNStarEstimates: %.4e",
                                                                                                m_psfNStarEstimates[i] ) )
                              << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfCounts: %d",
                                                                                                m_psfCounts[i] ) )
                              << FITSHeaderKeyword( "HISTORY", IsoString(), "Demosaic.psfType: "
                                                               + DebayerPSFType::FunctionName( m_instance.p_psfType ) )
                              << FITSHeaderKeyword( IsoString().Format( "PSFFLX%02d", i ),
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
                                                    IsoString().Format( "Number of valid PSF flux estimates, channel #%d", i ) )
                              << FITSHeaderKeyword( "PSFSGTYP",
                                                    DebayerPSFType::FunctionName( m_instance.p_psfType ).SingleQuoted(),
                                                    "PSF type used for signal estimation" );

               if ( m_instance.p_evaluateNoise )
                  keywordsChn << FITSHeaderKeyword( "HISTORY", IsoString(), "Noise evaluation with " + Module->ReadableVersion() )
                              << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.noiseEstimates: %.4e",
                                                                                                m_noiseEstimates[i] ) )
                              << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.noiseScaleLow: %.6e",
                                                                                                m_noiseScaleLow[i] ) )
                              << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.noiseScaleHigh: %.6e",
                                                                                                m_noiseScaleHigh[i] ) )
                              << FITSHeaderKeyword( IsoString().Format( "NOISE%02d", i ),
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
                                                    m_noiseAlgorithms[i],
                                                    IsoString().Format( "Noise evaluation algorithm, channel #%d", i ) );

               outputFile.WriteFITSKeywords( keywordsChn );
            }

            Module->ProcessEvents();

            {
               static Mutex mutex;
               static AtomicInt count;
               volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileWriteThreads );
               m_outputImage.SelectChannel( i );
               if ( !outputFile.WriteImage( m_outputImage ) || !outputFile.Close() )
                  throw CaughtException();
               m_outputImage.ResetSelections();
            }
         }

      m_outputImage.FreeData();
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool DebayerInstance::IsHistoryUpdater( const View& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool DebayerInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "Debayer cannot be executed on complex images.";
      return false;
   }

   if ( view.Image().Width() < 6 || view.Image().Height() < 6 )
   {
      whyNot = "Debayer needs an image of at least 6 by 6 pixels.";
      return false;
   }

   if ( !p_outputRGBImages )
      if ( !p_outputSeparateChannels )
      {
         whyNot = "The instance does not define a valid output mode.";
         return false;
      }

   return true;
}

// ----------------------------------------------------------------------------

bool DebayerInstance::ExecuteOn( View& view )
{
   o_imageId.Clear();
   o_channelIds = StringList( 3 );
   o_psfTotalFluxEstimates = Vector( 0.0, 3 );
   o_psfTotalPowerFluxEstimates = Vector( 0.0, 3 );
   o_psfTotalMeanFluxEstimates = Vector( 0.0, 3 );
   o_psfTotalMeanPowerFluxEstimates = Vector( 0.0, 3 );
   o_psfMStarEstimates = Vector( 0.0, 3 );
   o_psfNStarEstimates = Vector( 0.0, 3 );
   o_psfCounts = IVector( 0, 3 );
   o_noiseEstimates = Vector( 0.0, 3 );
   o_noiseFractions = Vector( 0.0, 3 );
   o_noiseScaleLow = Vector( 0.0, 3 );
   o_noiseScaleHigh = Vector( 0.0, 3 );
   o_noiseAlgorithms = StringList( 3 );

   {
      String why;
      if ( !CanExecuteOn( view, why ) )
         throw Error( why );
   }

   m_maxFileReadThreads = p_maxFileReadThreads;
   if ( m_maxFileReadThreads < 1 )
      m_maxFileReadThreads = Max( 1, PixInsightSettings::GlobalInteger( "Process/MaxFileReadThreads" ) );

   m_maxFileWriteThreads = p_maxFileWriteThreads;
   if ( m_maxFileWriteThreads < 1 )
      m_maxFileWriteThreads = Max( 1, PixInsightSettings::GlobalInteger( "Process/MaxFileWriteThreads" ) );

   AutoViewLock lock( view, false/*lock*/ );
   lock.LockForWrite();

   bool xtrans = IsXTransCFAFromTarget( view );
   IsoString patternId = CFAPatternIdFromTarget( view, xtrans );
   IsoString methodId = xtrans ? "X-Trans" : DebayerEngine::MethodId( p_debayerMethod );

   Console console;
   console.EnableAbort();

   console.Write( "<end><cbr>CFA pattern" );
   if ( xtrans || p_bayerPattern == DebayerBayerPatternParameter::Auto )
      console.Write( " (detected)" );
   console.WriteLn( ": " + patternId );

   ImageVariant source = view.Image();
   Image output;

   if ( xtrans )
   {
      XTransInterpolationEngine( sRGBConversionMatrixFromTarget( view ),
                                 XTransPatternFiltersFromTarget( view ) ).Interpolate( output, source, 2/*passes*/ );
   }
   else
   {
      int bayerPattern = BayerPatternFromTarget( view );
      // ### WARNING ### Experimental FBDD support - do not enable by default.
      if ( p_fbddNoiseReduction > 0 )
         FBDDEngine( bayerPattern, p_fbddNoiseReduction > 1 ).Denoise( source );
      DebayerEngine( output, *this, bayerPattern ).Debayer( source );
   }

   FITSKeywordArray keywords;
   view.Window().GetKeywords( keywords );

   /*
    * N.B.: If we already have computed signal or noise estimates (e.g., with
    * the ImageCalibration process), remove any previously existing related
    * keywords. Only our newly created keywords must be present in the header.
    */
   for ( size_type i = 0; i < keywords.Length(); )
   {
      bool remove = false;
      if ( keywords[i].name.StartsWithIC( "NOISE" ) )
         remove = true;
      else if ( keywords[i].name.StartsWithIC( "PSF" ) )
         for ( size_type j = 0; j < ItemsInArray( s_psfPrefixes ); ++j )
            if ( keywords[i].name.StartsWithIC( s_psfPrefixes[j] ) )
            {
               remove = true;
               break;
            }

      if ( remove )
         keywords.Remove( keywords.At( i ) );
      else
         ++i;
   }

   keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Demosaicing with "  + PixInsightVersion::AsString() )
            << FITSHeaderKeyword( "HISTORY", IsoString(), "Demosaicing with "  + Module->ReadableVersion() )
            << FITSHeaderKeyword( "HISTORY", IsoString(), "Demosaicing with Debayer process" )
            << FITSHeaderKeyword( "HISTORY", IsoString(), "Demosaic.pattern: " + patternId )
            << FITSHeaderKeyword( "HISTORY", IsoString(), "Demosaic.method: "  + methodId );

   PropertyArray properties = view.StorablePermanentProperties();

   String cfaSourceFilePath = p_cfaSourceFilePath.Trimmed();
   if ( cfaSourceFilePath.IsEmpty() )
      cfaSourceFilePath = view.Window().FilePath();

   if ( p_evaluateNoise || p_evaluateSignal )
   {
      EvaluateSignalAndNoise( o_psfTotalFluxEstimates, o_psfTotalPowerFluxEstimates,
                              o_psfTotalMeanFluxEstimates, o_psfTotalMeanPowerFluxEstimates,
                              o_psfMStarEstimates, o_psfNStarEstimates, o_psfCounts,
                              o_noiseEstimates, o_noiseFractions, o_noiseScaleLow, o_noiseScaleHigh, o_noiseAlgorithms,
                              source, patternId );

      if ( p_evaluateSignal )
         keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "PSF signal evaluation with " + Module->ReadableVersion() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalFluxEstimates: %.4e %.4e %.4e",
                                                      o_psfTotalFluxEstimates[0], o_psfTotalFluxEstimates[1], o_psfTotalFluxEstimates[2] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalPowerFluxEstimates: %.4e %.4e %.4e",
                                                      o_psfTotalPowerFluxEstimates[0], o_psfTotalPowerFluxEstimates[1], o_psfTotalPowerFluxEstimates[2] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalMeanFluxEstimates: %.4e %.4e %.4e",
                                                      o_psfTotalMeanFluxEstimates[0], o_psfTotalMeanFluxEstimates[1], o_psfTotalMeanFluxEstimates[2] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfTotalMeanPowerFluxEstimates: %.4e %.4e %.4e",
                                                      o_psfTotalMeanPowerFluxEstimates[0], o_psfTotalMeanPowerFluxEstimates[1], o_psfTotalMeanPowerFluxEstimates[2] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfMStarEstimates: %.4e %.4e %.4e",
                                                      o_psfMStarEstimates[0], o_psfMStarEstimates[1], o_psfMStarEstimates[2] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfNStarEstimates: %.4e %.4e %.4e",
                                                      o_psfNStarEstimates[0], o_psfNStarEstimates[1], o_psfNStarEstimates[2] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.psfCounts: %d %d %d",
                                                      o_psfCounts[0], o_psfCounts[1], o_psfCounts[2] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Demosaic.psfType: "
                                                      + DebayerPSFType::FunctionName( p_psfType ) );

      if ( p_evaluateNoise )
         keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "Noise evaluation with " + Module->ReadableVersion() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.noiseEstimates: %.4e %.4e %.4e",
                                                      o_noiseEstimates[0], o_noiseEstimates[1], o_noiseEstimates[2] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.noiseScaleLow: %.6e %.6e %.6e",
                                                      o_noiseScaleLow[0], o_noiseScaleLow[1], o_noiseScaleLow[2] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Demosaic.noiseScaleHigh: %.6e %.6e %.6e",
                                                      o_noiseScaleHigh[0], o_noiseScaleHigh[1], o_noiseScaleHigh[2] ) );
   }

   if ( p_outputRGBImages )
   {
      ImageWindow outputWindow(  1, // width
                                 1, // height
                                 3, // numberOfChannels
                                32, // bitsPerSample
                              true, // floatSample
                              true, // color
                              true, // initialProcessing
                              ValidFullId( view.FullId() ) + "_RGB_" + ValidMethodId( methodId ) ); // imageId

      outputWindow.MainView().Image().CopyImage( output );

      outputWindow.MainView().SetStorablePermanentProperties( properties, false/*notify*/ );

      if ( !cfaSourceFilePath.IsEmpty() )
         outputWindow.MainView().SetStorablePermanentPropertyValue( "PCL:CFASourceFilePath", cfaSourceFilePath, false/*notify*/ );
      outputWindow.MainView().SetStorablePermanentPropertyValue( "PCL:CFASourcePattern", patternId, false/*notify*/ );
      outputWindow.MainView().SetStorablePermanentPropertyValue( "PCL:CFASourceInterpolation", methodId, false/*notify*/ );

      FITSKeywordArray keywordsRGB = keywords;

      if ( p_evaluateSignal )
         for ( int i = 0; i < 3; ++i )
            keywordsRGB << FITSHeaderKeyword( IsoString().Format( "PSFFLX%02d", i ),
                                              IsoString().Format( "%.4e", o_psfTotalFluxEstimates[i] ),
                                              IsoString().Format( "Sum of PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFFLP%02d", i ),
                                              IsoString().Format( "%.4e", o_psfTotalPowerFluxEstimates[i] ),
                                              IsoString().Format( "Sum of squared PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMFL%02d", i ),
                                              IsoString().Format( "%.4e", o_psfTotalMeanFluxEstimates[i] ),
                                              IsoString().Format( "Sum of mean PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMFP%02d", i ),
                                              IsoString().Format( "%.4e", o_psfTotalMeanPowerFluxEstimates[i] ),
                                              IsoString().Format( "Sum of mean squared PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMST%02d", i ),
                                              IsoString().Format( "%.4e", o_psfMStarEstimates[i] ),
                                              IsoString().Format( "M* mean background estimate, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFNST%02d", i ),
                                              IsoString().Format( "%.4e", o_psfNStarEstimates[i] ),
                                              IsoString().Format( "N* noise estimate, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFSGN%02d", i ),
                                              IsoString().Format( "%d", o_psfCounts[i] ),
                                              IsoString().Format( "Number of valid PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( "PSFSGTYP",
                                              DebayerPSFType::FunctionName( p_psfType ).SingleQuoted(),
                                              "PSF type used for signal estimation" );

      if ( p_evaluateNoise )
         for ( int i = 0; i < 3; ++i )
            keywordsRGB << FITSHeaderKeyword( IsoString().Format( "NOISE%02d", i ),
                                              IsoString().Format( "%.4e", o_noiseEstimates[i] ),
                                              IsoString().Format( "Noise estimate, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEF%02d", i ),
                                              IsoString().Format( "%.3f", o_noiseFractions[i] ),
                                              IsoString().Format( "Fraction of noise pixels, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEL%02d", i ),
                                              IsoString().Format( "%.6e", o_noiseScaleLow[i] ),
                                              IsoString().Format( "Noise scaling factor, low pixels, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEH%02d", i ),
                                              IsoString().Format( "%.6e", o_noiseScaleHigh[i] ),
                                              IsoString().Format( "Noise scaling factor, high pixels, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEA%02d", i ),
                                              IsoString( o_noiseAlgorithms[i] ),
                                              IsoString().Format( "Noise evaluation algorithm, channel #%d", i ) );

      outputWindow.SetKeywords( keywordsRGB );

      if ( p_showImages )
         outputWindow.Show();

      o_imageId = outputWindow.MainView().Id();
   }

   if ( p_outputSeparateChannels )
      for ( int i = 0; i < 3; ++i )
      {
         ImageWindow outputWindow( 1, // width
                                   1, // height
                                   1, // numberOfChannels
                                  32, // bitsPerSample
                                true, // floatSample
                               false, // color
                                true, // initialProcessing
                               ValidFullId( view.FullId() ) + '_' + "RGB"[i] + '_' + ValidMethodId( methodId ) ); // imageId

         output.SelectChannel( i );
         outputWindow.MainView().Image().CopyImage( output );

         outputWindow.MainView().SetStorablePermanentProperties( properties, false/*notify*/ );

         if ( !cfaSourceFilePath.IsEmpty() )
            outputWindow.MainView().SetStorablePermanentPropertyValue( "PCL:CFASourceFilePath", cfaSourceFilePath, false/*notify*/ );
         outputWindow.MainView().SetStorablePermanentPropertyValue( "PCL:CFASourcePattern", patternId, false/*notify*/ );
         outputWindow.MainView().SetStorablePermanentPropertyValue( "PCL:CFASourceInterpolation", methodId, false/*notify*/ );
         outputWindow.MainView().SetStorablePermanentPropertyValue( "PCL:CFASourceChannel", i, false/*notify*/ );

         FITSKeywordArray keywordsChn = keywords;

         if ( p_evaluateSignal )
            keywordsChn << FITSHeaderKeyword( IsoString().Format( "PSFFLX%02d", i ),
                                              IsoString().Format( "%.4e", o_psfTotalFluxEstimates[i] ),
                                              IsoString().Format( "Sum of PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFFLP%02d", i ),
                                              IsoString().Format( "%.4e", o_psfTotalPowerFluxEstimates[i] ),
                                              IsoString().Format( "Sum of squared PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMFL%02d", i ),
                                              IsoString().Format( "%.4e", o_psfTotalMeanFluxEstimates[i] ),
                                              IsoString().Format( "Sum of mean PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMFP%02d", i ),
                                              IsoString().Format( "%.4e", o_psfTotalMeanPowerFluxEstimates[i] ),
                                              IsoString().Format( "Sum of mean squared PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMST%02d", i ),
                                              IsoString().Format( "%.4e", o_psfMStarEstimates[i] ),
                                              IsoString().Format( "M* mean background estimate, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFNST%02d", i ),
                                              IsoString().Format( "%.4e", o_psfNStarEstimates[i] ),
                                              IsoString().Format( "N* noise estimate, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFSGN%02d", i ),
                                              IsoString().Format( "%d", o_psfCounts[i] ),
                                              IsoString().Format( "Number of valid PSF flux estimates, channel #%d", i ) )
                        << FITSHeaderKeyword( "PSFSGTYP",
                                              DebayerPSFType::FunctionName( p_psfType ).SingleQuoted(),
                                              "PSF type used for signal estimation" );
         if ( p_evaluateNoise )
            keywordsChn << FITSHeaderKeyword( IsoString().Format( "NOISE%02d", i ),
                                              IsoString().Format( "%.4e", o_noiseEstimates[i] ),
                                              IsoString().Format( "Noise estimate, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEF%02d", i ),
                                              IsoString().Format( "%.3f", o_noiseFractions[i] ),
                                              IsoString().Format( "Fraction of noise pixels, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEL%02d", i ),
                                              IsoString().Format( "%.6e", o_noiseScaleLow[i] ),
                                              IsoString().Format( "Noise scaling factor, low pixels, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEH%02d", i ),
                                              IsoString().Format( "%.6e", o_noiseScaleHigh[i] ),
                                              IsoString().Format( "Noise scaling factor, high pixels, channel #%d", i ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEA%02d", i ),
                                              IsoString( o_noiseAlgorithms[i] ),
                                              IsoString().Format( "Noise evaluation algorithm, channel #%d", i ) );

         outputWindow.SetKeywords( keywordsChn );

         if ( p_showImages )
            outputWindow.Show();

         o_channelIds[i] = outputWindow.MainView().Id();
      }

   return true;
}

// ----------------------------------------------------------------------------

bool DebayerInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_targets.IsEmpty() )
   {
      whyNot = "No target images have been defined.";
      return false;
   }

   if ( !p_outputRGBImages )
      if ( !p_outputSeparateChannels )
      {
         whyNot = "The instance does not define a valid output mode.";
         return false;
      }

   return true;
}

// ----------------------------------------------------------------------------

typedef IndirectArray<DebayerFileThread> thread_list;

bool DebayerInstance::ExecuteGlobal()
{
   o_outputFileData = Array<OutputFileData>( p_targets.Length() );

   Console console;

   {
      String why;
      if ( !CanExecuteGlobal( why ) )
         throw Error( why );

      if ( !p_outputDirectory.IsEmpty() )
         if ( !File::DirectoryExists( p_outputDirectory ) )
            throw Error( "The specified output directory does not exist: " + p_outputDirectory );

      StringList fileNames;
      for ( const auto& target : p_targets )
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
               throw Error( "The target images list contains duplicate file names (case-insensitive). "
                            "This is not allowed when the 'Overwrite existing files' option is enabled." );
            console.WarningLn( "<end><cbr><br>** Warning: The target images list contains duplicate file names (case-insensitive)." );
            break;
         }
   }

   m_maxFileReadThreads = p_maxFileReadThreads;
   if ( m_maxFileReadThreads < 1 )
      m_maxFileReadThreads = Max( 1, PixInsightSettings::GlobalInteger( "Process/MaxFileReadThreads" ) );

   m_maxFileWriteThreads = p_maxFileWriteThreads;
   if ( m_maxFileWriteThreads < 1 )
      m_maxFileWriteThreads = Max( 1, PixInsightSettings::GlobalInteger( "Process/MaxFileWriteThreads" ) );

   console.EnableAbort();
   console.WriteLn( String().Format( "<end><cbr><br>Demosaicing of %u target files.", p_targets.Length() ) );

   int succeeded = 0;
   int failed = 0;
   int skipped = 0;

   Array<size_type> pendingItems;
   for ( size_type i = 0; i < p_targets.Length(); ++i )
      if ( p_targets[i].enabled )
         pendingItems << i;
      else
      {
         console.NoteLn( "* Skipping disabled target: " + p_targets[i].path );
         ++skipped;
      }

   if ( p_useFileThreads && pendingItems.Length() > 1 )
   {
      if ( p_memoryLoadControl )
      {
         float memLoad = Module->PhysicalMemoryLoad();
         if ( memLoad > p_memoryLoadLimit )
            console.WarningLn( String().Format( "<end><cbr><br>** Warning: physical memory load is above %.0f%%", 100*memLoad ) );
      }

      size_type bytesAvailable = Module->AvailablePhysicalMemory();
      if ( bytesAvailable == 0 )
         console.WarningLn( "<end><cbr><br>** Warning: Unable to estimate the available physical memory." );
      else
         console.NoteLn( String().Format( "<end><cbr><br>* Available physical memory: %.3f GiB", bytesAvailable/1024/1024/1024.0 ) );

      int numberOfThreadsAvailable = RoundInt( Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 ) * p_fileThreadOverload );
      int numberOfThreads = Min( numberOfThreadsAvailable, int( pendingItems.Length() ) );
      bool memoryLimited = false;

      if ( p_memoryLoadControl && bytesAvailable > 0 )
      {
         console.WriteLn( "<br>Estimating task memory requirements..." );
         console.Flush();

         Array<size_type> bytesForThreads;
         for ( size_type i : pendingItems )
         {
            Module->ProcessEvents();
            if ( console.AbortRequested() )
               throw ProcessAborted();

            FileFormat format( File::ExtractExtension( p_targets[i].path ), true/*read*/, false/*write*/ );
            FileFormatInstance file( format );
            ImageDescriptionArray images;
            if ( !file.Open( images, p_targets[i].path, p_inputHints + " verbosity 0" ) )
               throw CaughtException();
            bool xtrans = IsXTransCFAFromTarget( file );
            if ( !file.Close() )
               throw CaughtException();
            if ( images.IsEmpty() )
               continue;

            size_type bytesForTargetImage = images[0].info.NumberOfSamples() * (images[0].options.bitsPerSample >> 3);

            if ( xtrans )
               bytesForTargetImage += images[0].info.NumberOfPixels() * sizeof( uint16 );

            size_type bytesForSignalAndNoiseEvaluation = 0;
            if ( p_evaluateNoise || p_evaluateSignal )
            {
               int n = xtrans ? 6 : 2;
               bytesForSignalAndNoiseEvaluation = 6 * images[0].info.NumberOfPixels()/n/n * sizeof( float );
            }

            size_type bytesForOutputImage = images[0].info.NumberOfPixels() * 3 * sizeof( float );
            if ( p_debayerMethod == DebayerMethodParameter::SuperPixel )
               bytesForOutputImage >>= 2;

            bytesForThreads << bytesForTargetImage + bytesForSignalAndNoiseEvaluation + bytesForOutputImage;
         }

         if ( bytesForThreads.IsEmpty() )
            console.WarningLn( "<end><cbr>** Warning: Unable to estimate memory requirements: No valid image has been found." );
         else
         {
            size_type bytesPerThread = Median( bytesForThreads.Begin(), bytesForThreads.End() );
            console.NoteLn( String().Format( "<end><cbr>* Estimated per-thread memory allocation: %.3f GiB", bytesPerThread/1024/1024/1024.0 ) );

            bytesAvailable = size_type( double( p_memoryLoadLimit ) * Module->AvailablePhysicalMemory() );
            if ( bytesAvailable == 0 )
               console.WarningLn( "<end><cbr><br>** Warning: Unable to estimate the available physical memory." );
            else
            {
               int limitedNumberOfThreads = Min( numberOfThreads, Max( 1, int( bytesAvailable / bytesPerThread ) ) );
               memoryLimited = limitedNumberOfThreads < numberOfThreads;
               numberOfThreads = limitedNumberOfThreads;
            }
         }
      }

      thread_list runningThreads( numberOfThreads ); // N.B.: all pointers are set to nullptr by IndirectArray's ctor.
      console.NoteLn( String().Format( "<end><br>* Using %d worker threads%s.",
                                       numberOfThreads, memoryLimited ? " (memory-limited)" : "" ) );
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
                        (*i)->GetOutputData( o_outputFileData[(*i)->Index()] );
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
                     *i = new DebayerFileThread( *this, *pendingItems );
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
         for ( DebayerFileThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Abort();
         for ( DebayerFileThread* thread : runningThreads )
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
            DebayerFileThread thread( *this, itemIndex );
            thread.Run();
            thread.GetOutputData( o_outputFileData[itemIndex] );
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
         throw Error( "No images were demosaiced: Empty target frames list? No enabled target frames?" );
      throw Error( "No image could be demosaiced." );
   }

   console.NoteLn( String().Format( "<end><cbr><br>===== Debayer: %u succeeded, %u failed, %u skipped =====",
                                    succeeded, failed, skipped ) );
   return true;
}

// ----------------------------------------------------------------------------

void DebayerInstance::ApplyErrorPolicy()
{
   Console console;
   console.ResetStatus();
   console.EnableAbort();

   console.Note( "<end><cbr><br>* Applying error policy: " );

   switch ( p_onError )
   {
   default: // ?
   case DebayerOnError::Continue:
      console.NoteLn( "Continue on error." );
      break;

   case DebayerOnError::Abort:
      console.NoteLn( "Abort on error." );
      throw ProcessAborted();

   case DebayerOnError::AskUser:
      {
         console.NoteLn( "Ask on error..." );

         int r = MessageBox( "<p style=\"white-space:pre;\">"
                             "An error occurred during Debayer execution. What do you want to do?</p>",
                             "Debayer",
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

pcl_enum DebayerInstance::BayerPatternFromTarget( const View& view ) const
{
   if ( p_bayerPattern == DebayerBayerPatternParameter::Auto )
   {
      if ( view.HasProperty( "PCL:CFASourcePattern" ) )
         return BayerPatternFromTargetProperty( view.PropertyValue( "PCL:CFASourcePattern" ) );

      Console console;
      FITSKeywordArray keywords = view.Window().Keywords();
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
         return BayerPatternFromTargetPropertyValue( bayerPattern, TruncInt( xOffset ), TruncInt( yOffset ) );

      throw Error( "Unable to acquire CFA pattern information: Required image metadata not available." );
   }

   return p_bayerPattern;
}

pcl_enum DebayerInstance::BayerPatternFromTarget( FileFormatInstance& file ) const
{
   if ( p_bayerPattern == DebayerBayerPatternParameter::Auto )
   {
      if ( file.Format().CanStoreImageProperties() )
         if ( file.HasImageProperty( "PCL:CFASourcePattern" ) )
            return BayerPatternFromTargetProperty( file.ReadImageProperty( "PCL:CFASourcePattern" ) );

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
               return BayerPatternFromTargetPropertyValue( bayerPattern, TruncInt( xOffset ), TruncInt( yOffset ) );
         }
      }

      throw Error( "Unable to acquire CFA pattern information: Required image metadata not available." );
   }

   return p_bayerPattern;
}

pcl_enum DebayerInstance::BayerPatternFromTargetProperty( const Variant& cfaSourcePattern )
{
   if ( cfaSourcePattern.IsValid() )
      if ( cfaSourcePattern.IsString() )
         return BayerPatternFromTargetPropertyValue( cfaSourcePattern.ToIsoString() );

   throw Error( "Unable to acquire CFA pattern information: Invalid image property value." );
}

pcl_enum DebayerInstance::BayerPatternFromTargetPropertyValue( const IsoString& patternId, int dx, int dy )
{
   if ( patternId.Length() == 4 )
   {
      IsoString bayerPattern = patternId;
      if ( dx % 2 )
      {
         // Swap pattern columns
         Swap( bayerPattern[0], bayerPattern[1] );
         Swap( bayerPattern[2], bayerPattern[3] );
      }
      if ( dy % 2 )
      {
         // Swap pattern rows
         Swap( bayerPattern[0], bayerPattern[2] );
         Swap( bayerPattern[1], bayerPattern[3] );
      }

      if ( bayerPattern == "RGGB" )
         return DebayerBayerPatternParameter::RGGB;
      if ( bayerPattern == "BGGR" )
         return DebayerBayerPatternParameter::BGGR;
      if ( bayerPattern == "GBRG" )
         return DebayerBayerPatternParameter::GBRG;
      if ( bayerPattern == "GRBG" )
         return DebayerBayerPatternParameter::GRBG;
      if ( bayerPattern == "GRGB" )
         return DebayerBayerPatternParameter::GRGB;
      if ( bayerPattern == "GBGR" )
         return DebayerBayerPatternParameter::GBGR;
      if ( bayerPattern == "RGBG" )
         return DebayerBayerPatternParameter::RGBG;
      if ( bayerPattern == "BGRG" )
         return DebayerBayerPatternParameter::BGRG;
   }

   throw Error( "Unsupported or invalid CFA Bayer pattern '" + patternId + '\'' );
}

// ----------------------------------------------------------------------------

IsoString DebayerInstance::CFAPatternIdFromTarget( const View& view, bool xtrans ) const
{
   if ( xtrans || p_bayerPattern == DebayerBayerPatternParameter::Auto )
   {
      if ( view.HasProperty( "PCL:CFASourcePattern" ) )
         return CFAPatternIdFromTargetProperty( view.PropertyValue( "PCL:CFASourcePattern" ) );

      if ( !xtrans )
      {
         FITSKeywordArray keywords = view.Window().Keywords();
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
            return CFAPatternIdFromTargetProperty( bayerPattern, TruncInt( xOffset ), TruncInt( yOffset ) );
      }

      throw Error( "Unable to acquire CFA pattern information: Required image metadata not available." );
   }

   return TheDebayerBayerPatternParameter->ElementId( p_bayerPattern );
}

IsoString DebayerInstance::CFAPatternIdFromTarget( FileFormatInstance& file, bool xtrans ) const
{
   if ( xtrans || p_bayerPattern == DebayerBayerPatternParameter::Auto )
   {
      if ( file.Format().CanStoreImageProperties() )
         if ( file.HasImageProperty( "PCL:CFASourcePattern" ) )
            return CFAPatternIdFromTargetProperty( file.ReadImageProperty( "PCL:CFASourcePattern" ) );

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
                  return CFAPatternIdFromTargetProperty( bayerPattern, TruncInt( xOffset ), TruncInt( yOffset ) );
            }
         }

      throw Error( "Unable to acquire CFA pattern information: Required image metadata not available." );
   }

   return TheDebayerBayerPatternParameter->ElementId( p_bayerPattern );
}

IsoString DebayerInstance::CFAPatternIdFromTargetProperty( const Variant& cfaSourcePattern, int dx, int dy )
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

   throw Error( "Unable to acquire CFA pattern information: Invalid image property or keyword value." );
}

// ----------------------------------------------------------------------------

bool DebayerInstance::IsXTransCFAFromTarget( const View& view )
{
   return IsXTransCFAFromTargetProperty( view.PropertyValue( "PCL:CFASourcePatternName" ) );
}

bool DebayerInstance::IsXTransCFAFromTarget( FileFormatInstance& file )
{
   return file.Format().CanStoreImageProperties()
            && IsXTransCFAFromTargetProperty( file.ReadImageProperty( "PCL:CFASourcePatternName" ) );
}

bool DebayerInstance::IsXTransCFAFromTargetProperty( const Variant& cfaSourcePatternName )
{
   if ( cfaSourcePatternName.IsValid() )
      if ( cfaSourcePatternName.IsString() )
      {
         IsoString name = cfaSourcePatternName.ToIsoString().Trimmed().CaseFolded();
         return name == "x-trans" || name == "xtrans";
      }
   return false;
}

// ----------------------------------------------------------------------------

IMatrix DebayerInstance::XTransPatternFiltersFromTarget( const View& view )
{
   return XTransPatternFiltersFromTargetProperty( view.PropertyValue( "PCL:CFASourcePattern" ) );
}

IMatrix DebayerInstance::XTransPatternFiltersFromTarget( FileFormatInstance& file )
{
   return XTransPatternFiltersFromTargetProperty( file.ReadImageProperty( "PCL:CFASourcePattern" ) );
}

IMatrix DebayerInstance::XTransPatternFiltersFromTargetProperty( const Variant& cfaSourcePattern )
{
   if ( cfaSourcePattern.IsValid() )
      if ( cfaSourcePattern.IsString() )
      {
         IsoString pattern = cfaSourcePattern.ToIsoString().Trimmed();
         if ( pattern.Length() != 36 )
            throw Error( String().Format( "Invalid X-Trans CFA pattern length (expected 36 elements, got %u).", pattern.Length() ) );

         IMatrix F( 6, 6 );
         IsoString::const_iterator s = pattern.Begin();
         for ( int i = 0; i < 6; ++i )
            for ( int j = 0; j < 6; ++j, ++s )
               switch ( *s )
               {
               case 'R': F[i][j] = 0; break;
               case 'G': F[i][j] = 1; break;
               case 'B': F[i][j] = 2; break;
               default:
                  throw Error( String().Format( "Invalid X-Trans CFA pattern element '%%%02X'", int( *s ) ) );
               }
         return F;
      }
   throw Error( "Missing or invalid X-Trans CFA pattern description property." );
}

// ----------------------------------------------------------------------------

FMatrix DebayerInstance::sRGBConversionMatrixFromTarget( const View& view )
{
   return sRGBConversionMatrixFromTargetProperty( view.PropertyValue( "PCL:sRGBConversionMatrix" ) );
}

FMatrix DebayerInstance::sRGBConversionMatrixFromTarget( FileFormatInstance& file )
{
   return sRGBConversionMatrixFromTargetProperty( file.ReadImageProperty( "PCL:sRGBConversionMatrix" ) );
}

FMatrix DebayerInstance::sRGBConversionMatrixFromTargetProperty( const Variant& sRGBConversionMatrix )
{
   if ( sRGBConversionMatrix.IsValid() )
      if ( sRGBConversionMatrix.IsMatrix() )
         return sRGBConversionMatrix.ToFMatrix();
   return FMatrix::UnitMatrix( 3 );
}

// ----------------------------------------------------------------------------

void DebayerInstance::EvaluateSignalAndNoise( Vector& psfTotalFluxEstimates, Vector& psfTotalPowerFluxEstimates,
                                              Vector& psfTotalMeanFluxEstimates, Vector& psfTotalMeanPowerFluxEstimates,
                                              Vector& psfMStarEstimates, Vector& psfNStarEstimates, IVector& psfCounts,
                                              Vector& noiseEstimates, Vector& noiseFractions,
                                              Vector& noiseScaleLow, Vector& noiseScaleHigh, StringList& noiseAlgorithms,
                                              const ImageVariant& image, const IsoString& cfaPattern ) const
{
   volatile AutoStatusCallbackRestorer saveStatus( image.Status() );
   SpinStatus spin;
   image.SetStatusCallback( &spin );
   image.Status().Initialize( "Noise evaluation" );
   image.Status().DisableInitialization();

   int numberOfThreads = Thread::NumberOfThreads( image.NumberOfPixels(), 1 );
   if ( numberOfThreads >= 3 )
   {
      int numberOfSubthreads = RoundInt( numberOfThreads/3.0 );
      ReferenceArray<SignalAndNoiseEvaluationThread> threads;
      threads << new SignalAndNoiseEvaluationThread( image, 0, *this, cfaPattern, numberOfSubthreads )
              << new SignalAndNoiseEvaluationThread( image, 1, *this, cfaPattern, numberOfSubthreads )
              << new SignalAndNoiseEvaluationThread( image, 2, *this, cfaPattern, numberOfThreads - 2*numberOfSubthreads );

      AbstractImage::ThreadData data( image.Status(), 0 ); // unbounded
      AbstractImage::RunThreads( threads, data );

      for ( int i = 0; i < 3; ++i )
      {
         psfTotalFluxEstimates[i] = threads[i].psfTotalFluxEstimate;
         psfTotalPowerFluxEstimates[i] = threads[i].psfTotalPowerFluxEstimate;
         psfTotalMeanFluxEstimates[i] = threads[i].psfTotalMeanFluxEstimate;
         psfTotalMeanPowerFluxEstimates[i] = threads[i].psfTotalMeanPowerFluxEstimate;
         psfMStarEstimates[i] = threads[i].psfMStarEstimate;
         psfNStarEstimates[i] = threads[i].psfNStarEstimate;
         psfCounts[i] = threads[i].psfCount;
         noiseEstimates[i] = threads[i].noiseEstimate;
         noiseFractions[i] = threads[i].noiseFraction;
         noiseAlgorithms[i] = threads[i].noiseAlgorithm;
         noiseScaleLow[i] = threads[i].noiseScaleLow;
         noiseScaleHigh[i] = threads[i].noiseScaleHigh;
      }

      threads.Destroy();
   }
   else
   {
      for ( int i = 0; i < 3; ++i )
      {
         SignalAndNoiseEvaluationThread thread( image, i, *this, cfaPattern, 1/*numberOfSubthreads*/ );
         thread.Run();
         psfTotalFluxEstimates[i] = thread.psfTotalFluxEstimate;
         psfTotalPowerFluxEstimates[i] = thread.psfTotalPowerFluxEstimate;
         psfTotalMeanFluxEstimates[i] = thread.psfTotalMeanFluxEstimate;
         psfTotalMeanPowerFluxEstimates[i] = thread.psfTotalMeanPowerFluxEstimate;
         psfMStarEstimates[i] = thread.psfMStarEstimate;
         psfNStarEstimates[i] = thread.psfNStarEstimate;
         psfCounts[i] = thread.psfCount;
         noiseEstimates[i] = thread.noiseEstimate;
         noiseFractions[i] = thread.noiseFraction;
         noiseAlgorithms[i] = thread.noiseAlgorithm;
         noiseScaleLow[i] = thread.noiseScaleLow;
         noiseScaleHigh[i] = thread.noiseScaleHigh;
      }
   }

   image.ResetSelections();
   image.Status().Complete();

   Console console;

   if ( p_evaluateSignal )
      if ( psfCounts.Sum() > 0 )
      {
         console.WriteLn( "<end><cbr>PSF signal estimates:" );
         for ( int i = 0; i < 3; ++i )
            if ( psfCounts[i] > 0 )
               console.WriteLn( String().Format( "ch %d : TFlux = %.4e, TMeanFlux = %.4e, M* = %.4e, N* = %.4e, %d PSF fits",
                                                i,
                                                psfTotalFluxEstimates[i], psfTotalMeanFluxEstimates[i],
                                                psfMStarEstimates[i], psfNStarEstimates[i], psfCounts[i] ) );
            else
               console.WarningLn( String().Format( "** Warning: No valid PSF signal samples (channel %d).", i ) );
      }
      else
         console.WarningLn( "** Warning: No valid PSF signal samples." );

   if ( p_evaluateNoise )
   {
      console.WriteLn( "Noise estimates:" );
      for ( int i = 0; i < 3; ++i )
         console.WriteLn( String().Format( "ch %d : sigma_n = %.4e, %.2f%% pixels ", i, noiseEstimates[i], noiseFractions[i]*100 )
                           + '(' + noiseAlgorithms[i] + ')' );
      console.WriteLn( "Noise scaling factors:" );
      for ( int i = 0; i < 3; ++i )
         console.WriteLn( String().Format( "ch %d : sigma_low = %.6e, sigma_high = %.6e", i, noiseScaleLow[i], noiseScaleHigh[i] ) );
   }
}

// ----------------------------------------------------------------------------

void* DebayerInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheDebayerBayerPatternParameter )
      return &p_bayerPattern;
   if ( p == TheDebayerMethodParameter )
      return &p_debayerMethod;
   if ( p == TheDebayerFBDDNoiseReductionParameter )
      return &p_fbddNoiseReduction;
   if ( p == TheDebayerShowImagesParameter )
      return &p_showImages;
   if ( p == TheDebayerCFASourceFilePathParameter )
      return p_cfaSourceFilePath.Begin();
   if ( p == TheDebayerTargetEnabledParameter )
      return &p_targets[tableRow].enabled;
   if ( p == TheDebayerTargetImageParameter )
      return p_targets[tableRow].path.Begin();
   if ( p == TheDebayerNoGUIMessagesParameter )
      return &p_noGUIMessages;

   if ( p == TheDebayerEvaluateNoiseParameter )
      return &p_evaluateNoise;
   if ( p == TheDebayerNoiseEvaluationAlgorithmParameter )
      return &p_noiseEvaluationAlgorithm;

   if ( p == TheDebayerEvaluateSignalParameter )
      return &p_evaluateSignal;
   if ( p == TheDebayerStructureLayersParameter )
      return &p_structureLayers;
   if ( p == TheDebayerSaturationThresholdParameter )
      return &p_saturationThreshold;
   if ( p == TheDebayerSaturationRelativeParameter )
      return &p_saturationRelative;
   if ( p == TheDebayerNoiseLayersParameter )
      return &p_noiseLayers;
   if ( p == TheDebayerHotPixelFilterRadiusParameter )
      return &p_hotPixelFilterRadius;
   if ( p == TheDebayerNoiseReductionFilterRadiusParameter )
      return &p_noiseReductionFilterRadius;
   if ( p == TheDebayerMinStructureSizeParameter )
      return &p_minStructureSize;
   if ( p == TheDebayerPSFTypeParameter )
      return &p_psfType;
   if ( p == TheDebayerPSFGrowthParameter )
      return &p_psfGrowth;
   if ( p == TheDebayerMaxStarsParameter )
      return &p_maxStars;

   if ( p == TheDebayerInputHintsParameter )
      return p_inputHints.Begin();
   if ( p == TheDebayerOutputHintsParameter )
      return p_outputHints.Begin();
   if ( p == TheDebayerOutputRGBImagesParameter )
      return &p_outputRGBImages;
   if ( p == TheDebayerOutputSeparateChannelsParameter )
      return &p_outputSeparateChannels;
   if ( p == TheDebayerOutputDirectoryParameter )
      return p_outputDirectory.Begin();
   if ( p == TheDebayerOutputExtensionParameter )
      return p_outputExtension.Begin();
   if ( p == TheDebayerOutputPrefixParameter )
      return p_outputPrefix.Begin();
   if ( p == TheDebayerOutputPostfixParameter )
      return p_outputPostfix.begin();
   if ( p == TheDebayerOverwriteExistingFilesParameter )
      return &p_overwriteExistingFiles;
   if ( p == TheDebayerOnErrorParameter )
      return &p_onError;
   if ( p == TheDebayerUseFileThreadsParameter )
      return &p_useFileThreads;
   if ( p == TheDebayerFileThreadOverloadParameter )
      return &p_fileThreadOverload;
   if ( p == TheDebayerMaxFileReadThreadsParameter )
      return &p_maxFileReadThreads;
   if ( p == TheDebayerMaxFileWriteThreadsParameter )
      return &p_maxFileWriteThreads;
   if ( p == TheDebayerMemoryLoadControlParameter )
      return &p_memoryLoadControl;
   if ( p == TheDebayerMemoryLoadLimitParameter )
      return &p_memoryLoadLimit;

   if ( p == TheDebayerOutputImageParameter )
      return o_imageId.Begin();

   if ( p == TheDebayerOutputChannelImageRParameter )
      return o_channelIds[0].Begin();
   if ( p == TheDebayerOutputChannelImageGParameter )
      return o_channelIds[1].Begin();
   if ( p == TheDebayerOutputChannelImageBParameter )
      return o_channelIds[2].Begin();

   if ( p == TheDebayerPSFTotalFluxEstimateRParameter )
      return o_psfTotalFluxEstimates.At( 0 );
   if ( p == TheDebayerPSFTotalFluxEstimateGParameter )
      return o_psfTotalFluxEstimates.At( 1 );
   if ( p == TheDebayerPSFTotalFluxEstimateBParameter )
      return o_psfTotalFluxEstimates.At( 2 );

   if ( p == TheDebayerPSFTotalPowerFluxEstimateRParameter )
      return o_psfTotalPowerFluxEstimates.At( 0 );
   if ( p == TheDebayerPSFTotalPowerFluxEstimateGParameter )
      return o_psfTotalPowerFluxEstimates.At( 1 );
   if ( p == TheDebayerPSFTotalPowerFluxEstimateBParameter )
      return o_psfTotalPowerFluxEstimates.At( 2 );

   if ( p == TheDebayerPSFTotalMeanFluxEstimateRParameter )
      return o_psfTotalMeanFluxEstimates.At( 0 );
   if ( p == TheDebayerPSFTotalMeanFluxEstimateGParameter )
      return o_psfTotalMeanFluxEstimates.At( 1 );
   if ( p == TheDebayerPSFTotalMeanFluxEstimateBParameter )
      return o_psfTotalMeanFluxEstimates.At( 2 );

   if ( p == TheDebayerPSFTotalMeanPowerFluxEstimateRParameter )
      return o_psfTotalMeanPowerFluxEstimates.At( 0 );
   if ( p == TheDebayerPSFTotalMeanPowerFluxEstimateGParameter )
      return o_psfTotalMeanPowerFluxEstimates.At( 1 );
   if ( p == TheDebayerPSFTotalMeanPowerFluxEstimateBParameter )
      return o_psfTotalMeanPowerFluxEstimates.At( 2 );

   if ( p == TheDebayerPSFMStarEstimateRParameter )
      return o_psfMStarEstimates.At( 0 );
   if ( p == TheDebayerPSFMStarEstimateGParameter )
      return o_psfMStarEstimates.At( 1 );
   if ( p == TheDebayerPSFMStarEstimateBParameter )
      return o_psfMStarEstimates.At( 2 );

   if ( p == TheDebayerPSFNStarEstimateRParameter )
      return o_psfNStarEstimates.At( 0 );
   if ( p == TheDebayerPSFNStarEstimateGParameter )
      return o_psfNStarEstimates.At( 1 );
   if ( p == TheDebayerPSFNStarEstimateBParameter )
      return o_psfNStarEstimates.At( 2 );

   if ( p == TheDebayerPSFCountRParameter )
      return o_psfCounts.At( 0 );
   if ( p == TheDebayerPSFCountGParameter )
      return o_psfCounts.At( 1 );
   if ( p == TheDebayerPSFCountBParameter )
      return o_psfCounts.At( 2 );

   if ( p == TheDebayerNoiseEstimateRParameter )
      return o_noiseEstimates.At( 0 );
   if ( p == TheDebayerNoiseEstimateGParameter )
      return o_noiseEstimates.At( 1 );
   if ( p == TheDebayerNoiseEstimateBParameter )
      return o_noiseEstimates.At( 2 );

   if ( p == TheDebayerNoiseFractionRParameter )
      return o_noiseFractions.At( 0 );
   if ( p == TheDebayerNoiseFractionGParameter )
      return o_noiseFractions.At( 1 );
   if ( p == TheDebayerNoiseFractionBParameter )
      return o_noiseFractions.At( 2 );

   if ( p == TheDebayerNoiseScaleLowRParameter )
      return o_noiseScaleLow.At( 0 );
   if ( p == TheDebayerNoiseScaleLowGParameter )
      return o_noiseScaleLow.At( 1 );
   if ( p == TheDebayerNoiseScaleLowBParameter )
      return o_noiseScaleLow.At( 2 );

   if ( p == TheDebayerNoiseScaleHighRParameter )
      return o_noiseScaleHigh.At( 0 );
   if ( p == TheDebayerNoiseScaleHighGParameter )
      return o_noiseScaleHigh.At( 1 );
   if ( p == TheDebayerNoiseScaleHighBParameter )
      return o_noiseScaleHigh.At( 2 );

   if ( p == TheDebayerNoiseAlgorithmRParameter )
      return o_noiseAlgorithms[0].Begin();
   if ( p == TheDebayerNoiseAlgorithmGParameter )
      return o_noiseAlgorithms[1].Begin();
   if ( p == TheDebayerNoiseAlgorithmBParameter )
      return o_noiseAlgorithms[2].Begin();

   if ( p == TheDebayerOutputFilePathParameter )
      return o_outputFileData[tableRow].filePath.Begin();

   if ( p == TheDebayerOutputChannelFilePathRParameter )
      return o_outputFileData[tableRow].channelFilePaths[0].Begin();
   if ( p == TheDebayerOutputChannelFilePathGParameter )
      return o_outputFileData[tableRow].channelFilePaths[1].Begin();
   if ( p == TheDebayerOutputChannelFilePathBParameter )
      return o_outputFileData[tableRow].channelFilePaths[2].Begin();

   if ( p == TheDebayerOutputFilePSFTotalFluxEstimateRParameter )
      return o_outputFileData[tableRow].psfTotalFluxEstimates.At( 0 );
   if ( p == TheDebayerOutputFilePSFTotalFluxEstimateGParameter )
      return o_outputFileData[tableRow].psfTotalFluxEstimates.At( 1 );
   if ( p == TheDebayerOutputFilePSFTotalFluxEstimateBParameter )
      return o_outputFileData[tableRow].psfTotalFluxEstimates.At( 2 );

   if ( p == TheDebayerOutputFilePSFTotalPowerFluxEstimateRParameter )
      return o_outputFileData[tableRow].psfTotalPowerFluxEstimates.At( 0 );
   if ( p == TheDebayerOutputFilePSFTotalPowerFluxEstimateGParameter )
      return o_outputFileData[tableRow].psfTotalPowerFluxEstimates.At( 1 );
   if ( p == TheDebayerOutputFilePSFTotalPowerFluxEstimateBParameter )
      return o_outputFileData[tableRow].psfTotalPowerFluxEstimates.At( 2 );

   if ( p == TheDebayerOutputFilePSFTotalMeanFluxEstimateRParameter )
      return o_outputFileData[tableRow].psfTotalMeanFluxEstimates.At( 0 );
   if ( p == TheDebayerOutputFilePSFTotalMeanFluxEstimateGParameter )
      return o_outputFileData[tableRow].psfTotalMeanFluxEstimates.At( 1 );
   if ( p == TheDebayerOutputFilePSFTotalMeanFluxEstimateBParameter )
      return o_outputFileData[tableRow].psfTotalMeanFluxEstimates.At( 2 );

   if ( p == TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateRParameter )
      return o_outputFileData[tableRow].psfTotalMeanPowerFluxEstimates.At( 0 );
   if ( p == TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateGParameter )
      return o_outputFileData[tableRow].psfTotalMeanPowerFluxEstimates.At( 1 );
   if ( p == TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateBParameter )
      return o_outputFileData[tableRow].psfTotalMeanPowerFluxEstimates.At( 2 );

   if ( p == TheDebayerOutputFilePSFMStarEstimateRParameter )
      return o_outputFileData[tableRow].psfMStarEstimates.At( 0 );
   if ( p == TheDebayerOutputFilePSFMStarEstimateGParameter )
      return o_outputFileData[tableRow].psfMStarEstimates.At( 1 );
   if ( p == TheDebayerOutputFilePSFMStarEstimateBParameter )
      return o_outputFileData[tableRow].psfMStarEstimates.At( 2 );

   if ( p == TheDebayerOutputFilePSFNStarEstimateRParameter )
      return o_outputFileData[tableRow].psfNStarEstimates.At( 0 );
   if ( p == TheDebayerOutputFilePSFNStarEstimateGParameter )
      return o_outputFileData[tableRow].psfNStarEstimates.At( 1 );
   if ( p == TheDebayerOutputFilePSFNStarEstimateBParameter )
      return o_outputFileData[tableRow].psfNStarEstimates.At( 2 );

   if ( p == TheDebayerOutputFilePSFCountRParameter )
      return o_outputFileData[tableRow].psfCounts.At( 0 );
   if ( p == TheDebayerOutputFilePSFCountGParameter )
      return o_outputFileData[tableRow].psfCounts.At( 1 );
   if ( p == TheDebayerOutputFilePSFCountBParameter )
      return o_outputFileData[tableRow].psfCounts.At( 2 );

   if ( p == TheDebayerOutputFileNoiseEstimateRParameter )
      return o_outputFileData[tableRow].noiseEstimates.At( 0 );
   if ( p == TheDebayerOutputFileNoiseEstimateGParameter )
      return o_outputFileData[tableRow].noiseEstimates.At( 1 );
   if ( p == TheDebayerOutputFileNoiseEstimateBParameter )
      return o_outputFileData[tableRow].noiseEstimates.At( 2 );

   if ( p == TheDebayerOutputFileNoiseFractionRParameter )
      return o_outputFileData[tableRow].noiseFractions.At( 0 );
   if ( p == TheDebayerOutputFileNoiseFractionGParameter )
      return o_outputFileData[tableRow].noiseFractions.At( 1 );
   if ( p == TheDebayerOutputFileNoiseFractionBParameter )
      return o_outputFileData[tableRow].noiseFractions.At( 2 );

   if ( p == TheDebayerOutputFileNoiseScaleLowRParameter )
      return o_outputFileData[tableRow].noiseScaleLow.At( 0 );
   if ( p == TheDebayerOutputFileNoiseScaleLowGParameter )
      return o_outputFileData[tableRow].noiseScaleLow.At( 1 );
   if ( p == TheDebayerOutputFileNoiseScaleLowBParameter )
      return o_outputFileData[tableRow].noiseScaleLow.At( 2 );

   if ( p == TheDebayerOutputFileNoiseScaleHighRParameter )
      return o_outputFileData[tableRow].noiseScaleHigh.At( 0 );
   if ( p == TheDebayerOutputFileNoiseScaleHighGParameter )
      return o_outputFileData[tableRow].noiseScaleHigh.At( 1 );
   if ( p == TheDebayerOutputFileNoiseScaleHighBParameter )
      return o_outputFileData[tableRow].noiseScaleHigh.At( 2 );

   if ( p == TheDebayerOutputFileNoiseAlgorithmRParameter )
      return o_outputFileData[tableRow].noiseAlgorithms[0].Begin();
   if ( p == TheDebayerOutputFileNoiseAlgorithmGParameter )
      return o_outputFileData[tableRow].noiseAlgorithms[1].Begin();
   if ( p == TheDebayerOutputFileNoiseAlgorithmBParameter )
      return o_outputFileData[tableRow].noiseAlgorithms[2].Begin();

   return nullptr;
}

// ----------------------------------------------------------------------------

bool DebayerInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheDebayerTargetItemsParameter )
   {
      p_targets.Clear();
      if ( sizeOrLength > 0 )
         p_targets.Add( Item(), sizeOrLength );
   }
   else if ( p == TheDebayerTargetImageParameter )
   {
      p_targets[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_targets[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerCFASourceFilePathParameter )
   {
      p_cfaSourceFilePath.Clear();
      if ( sizeOrLength > 0 )
         p_cfaSourceFilePath.SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputHintsParameter )
   {
      p_outputHints.Clear();
      if ( sizeOrLength > 0 )
         p_outputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputDirectoryParameter )
   {
      p_outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_outputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputExtensionParameter )
   {
      p_outputExtension.Clear();
      if ( sizeOrLength > 0 )
         p_outputExtension.SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputPrefixParameter )
   {
      p_outputPrefix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPrefix.SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputPostfixParameter )
   {
      p_outputPostfix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPostfix.SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputImageParameter )
   {
      o_imageId.Clear();
      if ( sizeOrLength > 0 )
         o_imageId.SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputChannelImageRParameter )
   {
      o_channelIds[0].Clear();
      if ( sizeOrLength > 0 )
         o_channelIds[0].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputChannelImageGParameter )
   {
      o_channelIds[1].Clear();
      if ( sizeOrLength > 0 )
         o_channelIds[1].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputChannelImageBParameter )
   {
      o_channelIds[2].Clear();
      if ( sizeOrLength > 0 )
         o_channelIds[2].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerNoiseAlgorithmRParameter )
   {
      o_noiseAlgorithms[0].Clear();
      if ( sizeOrLength > 0 )
         o_noiseAlgorithms[0].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerNoiseAlgorithmGParameter )
   {
      o_noiseAlgorithms[1].Clear();
      if ( sizeOrLength > 0 )
         o_noiseAlgorithms[1].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerNoiseAlgorithmBParameter )
   {
      o_noiseAlgorithms[2].Clear();
      if ( sizeOrLength > 0 )
         o_noiseAlgorithms[2].SetLength( sizeOrLength );
   }

   else if ( p == TheDebayerOutputFileDataParameter )
   {
      o_outputFileData.Clear();
      if ( sizeOrLength > 0 )
         o_outputFileData.Add( OutputFileData(), sizeOrLength );
   }
   else if ( p == TheDebayerOutputFilePathParameter )
   {
      o_outputFileData[tableRow].filePath.Clear();
      if ( sizeOrLength > 0 )
         o_outputFileData[tableRow].filePath.SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputChannelFilePathRParameter )
   {
      o_outputFileData[tableRow].channelFilePaths[0].Clear();
      if ( sizeOrLength > 0 )
         o_outputFileData[tableRow].channelFilePaths[0].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputChannelFilePathGParameter )
   {
      o_outputFileData[tableRow].channelFilePaths[1].Clear();
      if ( sizeOrLength > 0 )
         o_outputFileData[tableRow].channelFilePaths[1].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputChannelFilePathBParameter )
   {
      o_outputFileData[tableRow].channelFilePaths[2].Clear();
      if ( sizeOrLength > 0 )
         o_outputFileData[tableRow].channelFilePaths[2].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputFileNoiseAlgorithmRParameter )
   {
      o_outputFileData[tableRow].noiseAlgorithms[0].Clear();
      if ( sizeOrLength > 0 )
         o_outputFileData[tableRow].noiseAlgorithms[0].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputFileNoiseAlgorithmGParameter )
   {
      o_outputFileData[tableRow].noiseAlgorithms[1].Clear();
      if ( sizeOrLength > 0 )
         o_outputFileData[tableRow].noiseAlgorithms[1].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerOutputFileNoiseAlgorithmBParameter )
   {
      o_outputFileData[tableRow].noiseAlgorithms[2].Clear();
      if ( sizeOrLength > 0 )
         o_outputFileData[tableRow].noiseAlgorithms[2].SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type DebayerInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheDebayerTargetItemsParameter )
      return p_targets.Length();
   if ( p == TheDebayerTargetImageParameter )
      return p_targets[tableRow].path.Length();
   if ( p == TheDebayerCFASourceFilePathParameter )
      return p_cfaSourceFilePath.Length();
   if ( p == TheDebayerInputHintsParameter )
      return p_inputHints.Length();
   if ( p == TheDebayerOutputHintsParameter )
      return p_outputHints.Length();
   if ( p == TheDebayerOutputDirectoryParameter )
      return p_outputDirectory.Length();
   if ( p == TheDebayerOutputExtensionParameter )
      return p_outputExtension.Length();
   if ( p == TheDebayerOutputPrefixParameter )
      return p_outputPrefix.Length();
   if ( p == TheDebayerOutputPostfixParameter )
      return p_outputPostfix.Length();

   if ( p == TheDebayerOutputImageParameter )
      return o_imageId.Length();
   if ( p == TheDebayerOutputChannelImageRParameter )
      return o_channelIds[0].Length();
   if ( p == TheDebayerOutputChannelImageGParameter )
      return o_channelIds[1].Length();
   if ( p == TheDebayerOutputChannelImageBParameter )
      return o_channelIds[2].Length();
   if ( p == TheDebayerNoiseAlgorithmRParameter )
      return o_noiseAlgorithms[0].Length();
   if ( p == TheDebayerNoiseAlgorithmGParameter )
      return o_noiseAlgorithms[1].Length();
   if ( p == TheDebayerNoiseAlgorithmBParameter )
      return o_noiseAlgorithms[2].Length();

   if ( p == TheDebayerOutputFileDataParameter )
      return o_outputFileData.Length();
   if ( p == TheDebayerOutputFilePathParameter )
      return o_outputFileData[tableRow].filePath.Length();
   if ( p == TheDebayerOutputChannelFilePathRParameter )
      return o_outputFileData[tableRow].channelFilePaths[0].Length();
   if ( p == TheDebayerOutputChannelFilePathGParameter )
      return o_outputFileData[tableRow].channelFilePaths[1].Length();
   if ( p == TheDebayerOutputChannelFilePathBParameter )
      return o_outputFileData[tableRow].channelFilePaths[2].Length();
   if ( p == TheDebayerOutputFileNoiseAlgorithmRParameter )
      return o_outputFileData[tableRow].noiseAlgorithms[0].Length();
   if ( p == TheDebayerOutputFileNoiseAlgorithmGParameter )
      return o_outputFileData[tableRow].noiseAlgorithms[1].Length();
   if ( p == TheDebayerOutputFileNoiseAlgorithmBParameter )
      return o_outputFileData[tableRow].noiseAlgorithms[2].Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DebayerInstance.cpp - Released 2023-08-28T15:23:41Z
