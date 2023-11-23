//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.3
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.8.9
// ----------------------------------------------------------------------------
// SubframeSelectorInstance.cpp - Released 2023-11-23T18:45:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2021 Cameron Leger
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

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/Console.h>
#include <pcl/Cryptography.h>
#include <pcl/DrizzleData.h>
#include <pcl/ElapsedTime.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/Graphics.h>
#include <pcl/ICCProfile.h>
#include <pcl/LocalNormalizationData.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/PSFFit.h>
#include <pcl/PSFSignalEstimator.h>
#include <pcl/Position.h>
#include <pcl/ProcessInstance.h>
#include <pcl/RobustChauvenetRejection.h>
#include <pcl/Version.h>
#include <pcl/WCSKeywords.h>

#include "SubframeSelectorCache.h"
#include "SubframeSelectorInstance.h"
#include "SubframeSelectorInterface.h"
#include "SubframeSelectorMeasurementsInterface.h"

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

SubframeSelectorInstance::SubframeSelectorInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_routine( SSRoutine::Default )
   , p_nonInteractive( TheSSNonInteractiveParameter->DefaultValue() )
   , p_fileCache( TheSSFileCacheParameter->DefaultValue() )
   , p_subframeScale( TheSSSubframeScaleParameter->DefaultValue() )
   , p_cameraGain( TheSSCameraGainParameter->DefaultValue() )
   , p_cameraResolution( SSCameraResolution::Default )
   , p_siteLocalMidnight( TheSSSiteLocalMidnightParameter->DefaultValue() )
   , p_scaleUnit( SSScaleUnit::Default )
   , p_dataUnit( SSDataUnit::Default )
   , p_trimmingFactor( TheSSTrimmingFactorParameter->DefaultValue() )
   , p_structureLayers( TheSSStructureLayersParameter->DefaultValue() )
   , p_noiseLayers( TheSSNoiseLayersParameter->DefaultValue() )
   , p_hotPixelFilterRadius( TheSSHotPixelFilterRadiusParameter->DefaultValue() )
   , p_noiseReductionFilterRadius( TheSSNoiseReductionFilterRadiusParameter->DefaultValue() )
   , p_minStructureSize( TheSSMinStructureSizeParameter->DefaultValue() )
   , p_sensitivity( TheSSSensitivityParameter->DefaultValue() )
   , p_peakResponse( TheSSPeakResponseParameter->DefaultValue() )
   , p_brightThreshold( TheSSBrightThresholdParameter->DefaultValue() )
   , p_maxDistortion( TheSSMaxDistortionParameter->DefaultValue() )
   , p_allowClusteredSources( TheSSAllowClusteredSourcesParameter->DefaultValue() )
   , p_upperLimit( TheSSUpperLimitParameter->DefaultValue() )
   , p_psfFit( SSPSFFit::Default )
   , p_psfFitCircular( TheSSPSFFitCircularParameter->DefaultValue() )
   , p_maxPSFFits( int32( TheSSMaxPSFFitsParameter->DefaultValue() ) )
   , p_pedestal( TheSSPedestalParameter->DefaultValue() )
   , p_pedestalMode( SSPedestalMode::Default )
   , p_inputHints( TheSSInputHintsParameter->DefaultValue() )
   , p_outputHints( TheSSOutputHintsParameter->DefaultValue() )
   , p_outputDirectory( TheSSOutputDirectoryParameter->DefaultValue() )
   , p_outputExtension( TheSSOutputExtensionParameter->DefaultValue() )
   , p_outputPrefix( TheSSOutputPrefixParameter->DefaultValue() )
   , p_outputPostfix( TheSSOutputPostfixParameter->DefaultValue() )
   , p_outputKeyword( TheSSOutputKeywordParameter->DefaultValue() )
   , p_overwriteExistingFiles( TheSSOverwriteExistingFilesParameter->DefaultValue() )
   , p_onError( SSOnError::Default )
   , p_sortingProperty( SSSortingProperty::Default )
   , p_graphProperty( SSGraphProperty::Default )
   , p_auxGraphProperty( SSGraphProperty::Weight )
   , p_useFileThreads( TheSSUseFileThreadsParameter->DefaultValue() )
   , p_fileThreadOverload( TheSSFileThreadOverloadParameter->DefaultValue() )
   , p_maxFileReadThreads( int32( TheSSMaxFileReadThreadsParameter->DefaultValue() ) )
   , p_maxFileWriteThreads( int32( TheSSMaxFileWriteThreadsParameter->DefaultValue() ) )
{
}

// ----------------------------------------------------------------------------

SubframeSelectorInstance::SubframeSelectorInstance( const SubframeSelectorInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInstance::Assign( const ProcessImplementation& p )
{
   const SubframeSelectorInstance* x = dynamic_cast<const SubframeSelectorInstance*>( &p );
   if ( x != nullptr )
   {
      p_routine                    = x->p_routine;
      p_nonInteractive             = x->p_nonInteractive;
      p_fileCache                  = x->p_fileCache;
      p_subframes                  = x->p_subframes;
      p_subframeScale              = x->p_subframeScale;
      p_cameraGain                 = x->p_cameraGain;
      p_cameraResolution           = x->p_cameraResolution;
      p_siteLocalMidnight          = x->p_siteLocalMidnight;
      p_scaleUnit                  = x->p_scaleUnit;
      p_dataUnit                   = x->p_dataUnit;
      p_trimmingFactor             = x->p_trimmingFactor;
      p_structureLayers            = x->p_structureLayers;
      p_noiseLayers                = x->p_noiseLayers;
      p_hotPixelFilterRadius       = x->p_hotPixelFilterRadius;
      p_noiseReductionFilterRadius = x->p_noiseReductionFilterRadius;
      p_minStructureSize           = x->p_minStructureSize;

      /*
       * N.B. Some star detector parameters have changed their meaning and/or
       * valid ranges since core version 1.8.9-1, when the StarDetector class
       * was redesigned. Replace these parameters with default values for
       * existing instances created by old versions.
       */
      if ( p.Version() < 2 )
      {
         p_sensitivity = TheSSSensitivityParameter->DefaultValue();
         p_peakResponse = TheSSPeakResponseParameter->DefaultValue();
         p_maxDistortion = TheSSMaxDistortionParameter->DefaultValue();
      }
      else
      {
         p_sensitivity = x->p_sensitivity;
         p_peakResponse = x->p_peakResponse;
         p_maxDistortion = x->p_maxDistortion;
      }

      p_brightThreshold            = x->p_brightThreshold;
      p_allowClusteredSources      = x->p_allowClusteredSources;
      p_upperLimit                 = x->p_upperLimit;
      p_psfFit                     = x->p_psfFit;
      p_psfFitCircular             = x->p_psfFitCircular;
      p_maxPSFFits                 = x->p_maxPSFFits;
      p_roi                        = x->p_roi;
      p_pedestal                   = x->p_pedestal;
      p_pedestalMode               = x->p_pedestalMode;
      p_pedestalKeyword            = x->p_pedestalKeyword;
      p_inputHints                 = x->p_inputHints;
      p_outputHints                = x->p_outputHints;
      p_outputDirectory            = x->p_outputDirectory;
      p_outputExtension            = x->p_outputExtension;
      p_outputPrefix               = x->p_outputPrefix;
      p_outputPostfix              = x->p_outputPostfix;
      p_outputKeyword              = x->p_outputKeyword;
      p_overwriteExistingFiles     = x->p_overwriteExistingFiles;
      p_onError                    = x->p_onError;
      p_approvalExpression         = x->p_approvalExpression;
      p_weightingExpression        = x->p_weightingExpression;
      p_sortingProperty            = x->p_sortingProperty;
      p_graphProperty              = x->p_graphProperty;
      p_auxGraphProperty           = x->p_auxGraphProperty;
      p_useFileThreads             = x->p_useFileThreads;
      p_fileThreadOverload         = x->p_fileThreadOverload;
      p_maxFileReadThreads         = x->p_maxFileReadThreads;
      p_maxFileWriteThreads        = x->p_maxFileWriteThreads;

      o_measures                   = x->o_measures;
   }
}

// ----------------------------------------------------------------------------

typedef Array<PSFData> psf_list;

class SubframeSelectorMeasureThread : public Thread
{
public:

   SubframeSelectorMeasureThread( const SubframeSelectorInstance& instance,
                                  size_type itemIndex,
                                  bool throwsOnMeasurementError = true )
      : m_instance( instance )
      , m_index( itemIndex )
      , m_filePath( m_instance.p_subframes[m_index].path )
      , m_nmlPath( m_instance.p_subframes[m_index].nmlPath )
      , m_throwsOnMeasurementError( throwsOnMeasurementError )
   {
   }

   void Run() override
   {
      try
      {
         Module->ProcessEvents();

         if ( m_instance.p_routine == SSRoutine::MeasureSubframes )
            if ( m_instance.p_fileCache )
            {
               MeasureData cacheData( m_filePath );
               if ( cacheData.GetFromCache( m_instance ) )
               {
                  Console().NoteLn( "<end><cbr>* Retrieved data from file cache: <raw>" + m_filePath + "</raw>" );
                  m_outputData = cacheData;
                  m_success = true;
                  return;
               }
            }

         ReadInputData();
         Perform();

         if ( m_instance.p_routine == SSRoutine::MeasureSubframes )
            if ( m_instance.p_fileCache )
               m_outputData.AddToCache( m_instance );

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

   const String& TargetFilePath() const
   {
      return m_filePath;
   }

   const MeasureData& OutputData() const
   {
      return m_outputData;
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

   const SubframeSelectorInstance& m_instance;
         size_type                 m_index;
         String                    m_filePath;
         String                    m_nmlPath;
         ImageVariant              m_subframe;
         double                    m_pedestal = 0; // 16-bit DN
         double                    m_noise = 0;
         double                    m_noiseRatio = 0;
         double                    m_psfSignalWeight = 0;
         double                    m_psfSNR = 0;
         double                    m_psfScale = 0;
         double                    m_psfScaleSNR = 0;
         double                    m_psfFlux = 0;
         double                    m_psfFluxPower = 0;
         double                    m_psfTotalMeanFlux = 0;
         double                    m_psfTotalMeanPowerFlux = 0;
         unsigned                  m_psfCount = 0;
         double                    m_MStar;
         double                    m_NStar;
         double                    m_snrWeight = 0;
         MeasureData               m_outputData;
         bool                      m_success = false;
         String                    m_errorInfo;
         bool                      m_throwsOnMeasurementError = true;

   void ReadInputData()
   {
      Console console;
      console.WriteLn( "<end><cbr>* Loading subframe file: <raw>" + m_filePath + "</raw>" );

      FileFormat format( File::ExtractExtension( m_filePath ), true/*read*/, false/*write*/ );
      FileFormatInstance file( format );

      ImageDescriptionArray images;
      if ( !file.Open( images, m_filePath, m_instance.p_inputHints ) )
         throw CaughtException();

      if ( images.IsEmpty() )
         throw Error( m_filePath + ": Empty subframe image." );

      if ( images.Length() > 1 )
         console.NoteLn( String().Format( "* Ignoring %u additional image(s) in target file.", images.Length()-1 ) );

      FITSKeywordArray keywords;
      if ( format.CanStoreKeywords() )
         file.ReadFITSKeywords( keywords );

      PropertyArray properties;
      if ( format.CanStoreImageProperties() )
         properties = file.ReadImageProperties();

      int cfaSourceChannel = 0;
      if ( format.CanStoreImageProperties() )
         if ( file.HasImageProperty( "PCL:CFASourceChannel" ) )
         {
            Variant v = file.ReadImageProperty( "PCL:CFASourceChannel" );
            if ( v.IsValid() )
               cfaSourceChannel = v.ToInt();
         }

      m_subframe.CreateSharedFloatImage( 32/*bitsPerSample*/ );

      {
         static Mutex mutex;
         static AtomicInt count;
         volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileReadThreads );

         if ( !file.ReadImage( m_subframe ) || !file.Close() )
            throw CaughtException();
      }

      /*
       * Optional pedestal
       */
      switch ( m_instance.p_pedestalMode )
      {
      case SSPedestalMode::Literal:
         m_pedestal = m_instance.p_pedestal;
         break;
      case SSPedestalMode::Keyword:
      case SSPedestalMode::CustomKeyword:
         if ( !keywords.IsEmpty() )
         {
            IsoString keyName( (m_instance.p_pedestalMode == SSPedestalMode::Keyword) ?
                                    "PEDESTAL" : m_instance.p_pedestalKeyword );
            for ( const FITSHeaderKeyword& keyword : keywords )
               if ( !keyword.name.CompareIC( keyName ) )
                  if ( keyword.IsNumeric() )
                     if ( keyword.GetNumericValue( m_pedestal ) ) // GetNumericValue() sets m_pedestal=0 if keyword cannot be converted
                        break;
            if ( m_pedestal != 0 )
            {
               /*
                * Silently be compatible with acquisition applications that
                * write negative PEDESTAL keyword values.
                */
               if ( m_instance.p_pedestalMode == SSPedestalMode::Keyword )
                  if ( m_pedestal < 0 )
                     m_pedestal = -m_pedestal;

               console.NoteLn( String().Format( "* Retrieved pedestal keyword '%s': %.4f DN", keyName.c_str(), m_pedestal ) );
            }
         }
         break;
      }

      /*
       * Get noise and signal estimates from available metadata.
       */
      DVector noiseEstimates( 0.0, m_subframe.NumberOfNominalChannels() );
      DVector noiseRatios( 0.0, m_subframe.NumberOfNominalChannels() );
      DVector noiseScaleLows( 0.0, m_subframe.NumberOfNominalChannels() );
      DVector noiseScaleHighs( 0.0, m_subframe.NumberOfNominalChannels() );
      DVector psfTotalFluxEstimates( 0.0, m_subframe.NumberOfNominalChannels() );
      DVector psfTotalPowerFluxEstimates( 0.0, m_subframe.NumberOfNominalChannels() );
      DVector psfTotalMeanFluxEstimates( 0.0, m_subframe.NumberOfNominalChannels() );
      DVector psfTotalMeanPowerFluxEstimates( 0.0, m_subframe.NumberOfNominalChannels() );
      UIVector psfCounts( 0, m_subframe.NumberOfNominalChannels() );
      DVector MStarEstimates( 0.0, m_subframe.NumberOfNominalChannels() );
      DVector NStarEstimates( 0.0, m_subframe.NumberOfNominalChannels() );

      for ( int c = 0; c < m_subframe.NumberOfNominalChannels(); ++c )
      {
         IsoString keyName = IsoString().Format( "NOISE%02d", c + cfaSourceChannel );
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
            {
               if ( keyword.IsNumeric() )
                  keyword.GetNumericValue( noiseEstimates[c] ); // GetNumericValue() sets d=0 if keyword cannot be converted
               break;
            }

         keyName = IsoString().Format( "NOISEF%02d", c + cfaSourceChannel );
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
            {
               if ( keyword.IsNumeric() )
                  keyword.GetNumericValue( noiseRatios[c] );
               break;
            }

         keyName = IsoString().Format( "NOISEL%02d", c + cfaSourceChannel );
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
            {
               if ( keyword.IsNumeric() )
                  keyword.GetNumericValue( noiseScaleLows[c] );
               break;
            }

         keyName = IsoString().Format( "NOISEH%02d", c + cfaSourceChannel );
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
            {
               if ( keyword.IsNumeric() )
                  keyword.GetNumericValue( noiseScaleHighs[c] );
               break;
            }

         keyName = IsoString().Format( "PSFFLX%02d", c + cfaSourceChannel );
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
            {
               if ( keyword.IsNumeric() )
                  keyword.GetNumericValue( psfTotalFluxEstimates[c] );
               break;
            }

         keyName = IsoString().Format( "PSFFLP%02d", c + cfaSourceChannel );
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
            {
               if ( keyword.IsNumeric() )
                  keyword.GetNumericValue( psfTotalPowerFluxEstimates[c] );
               break;
            }

         keyName = IsoString().Format( "PSFMFL%02d", c + cfaSourceChannel );
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
            {
               if ( keyword.IsNumeric() )
                  keyword.GetNumericValue( psfTotalMeanFluxEstimates[c] );
               break;
            }

         keyName = IsoString().Format( "PSFMFP%02d", c + cfaSourceChannel );
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
            {
               if ( keyword.IsNumeric() )
                  keyword.GetNumericValue( psfTotalMeanPowerFluxEstimates[c] );
               break;
            }

         keyName = IsoString().Format( "PSFSGN%02d", c + cfaSourceChannel );
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
            {
               if ( keyword.IsNumeric() )
               {
                  double x;
                  keyword.GetNumericValue( x );
                  psfCounts[c] = unsigned( x );
               }
               break;
            }

         keyName = IsoString().Format( "PSFMST%02d", c + cfaSourceChannel );
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
            {
               if ( keyword.IsNumeric() )
                  keyword.GetNumericValue( MStarEstimates[c] );
               break;
            }

         keyName = IsoString().Format( "PSFNST%02d", c + cfaSourceChannel );
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
            {
               if ( keyword.IsNumeric() )
                  keyword.GetNumericValue( NStarEstimates[c] );
               break;
            }
      }

      /*
       * For grayscale images, return the image just read and its signal and
       * noise estimates if available. For color images, extract the HSI
       * intensity component and provide coherent estimates if available.
       */
      double psfTotalFlux, psfTotalPowerFlux, psfTotalMeanFlux, psfTotalMeanPowerFlux,
             MStar, NStar, noiseScaleLow, noiseScaleHigh;
      unsigned psfCount;
      if ( m_subframe.NumberOfNominalChannels() == 1 )
      {
         m_noise = noiseEstimates[0];
         m_noiseRatio = noiseRatios[0];
         noiseScaleLow = noiseScaleLows[0];
         noiseScaleHigh = noiseScaleHighs[0];
         psfTotalFlux = psfTotalFluxEstimates[0];
         psfTotalPowerFlux = psfTotalPowerFluxEstimates[0];
         psfTotalMeanFlux = psfTotalMeanFluxEstimates[0];
         psfTotalMeanPowerFlux = psfTotalMeanPowerFluxEstimates[0];
         psfCount = psfCounts[0];
         MStar = MStarEstimates[0];
         NStar = NStarEstimates[0];
      }
      else
      {
         ImageVariant I;
         I.CreateSharedFloatImage( 32/*bitsPerSample*/ );
         m_subframe.GetIntensity( I );
         m_subframe.Transfer( I );
         m_noise = 0.5*(noiseEstimates.MinComponent() + noiseEstimates.MaxComponent());
         m_noiseRatio = 0.5*(noiseRatios.MinComponent() + noiseRatios.MaxComponent());
         noiseScaleLow = 0.5*(noiseScaleLows.MinComponent() + noiseScaleLows.MaxComponent());
         noiseScaleHigh = 0.5*(noiseScaleHighs.MinComponent() + noiseScaleHighs.MaxComponent());
         psfTotalFlux = 0.5*(psfTotalFluxEstimates.MinComponent() + psfTotalFluxEstimates.MaxComponent());
         psfTotalPowerFlux = 0.5*(psfTotalPowerFluxEstimates.MinComponent() + psfTotalPowerFluxEstimates.MaxComponent());
         psfTotalMeanFlux = 0.5*(psfTotalMeanFluxEstimates.MinComponent() + psfTotalMeanFluxEstimates.MaxComponent());
         psfTotalMeanPowerFlux = 0.5*(psfTotalMeanPowerFluxEstimates.MinComponent() + psfTotalMeanPowerFluxEstimates.MaxComponent());
         psfCount = RoundInt( 0.5*(psfCounts.MinComponent() + psfCounts.MaxComponent()) );
         MStar = 0.5*(MStarEstimates.MinComponent() + MStarEstimates.MaxComponent());
         NStar = 0.5*(NStarEstimates.MinComponent() + NStarEstimates.MaxComponent());
      }

      if ( 1 + m_noise != 1 )
      {
         /*
          * Compute PSF signal estimates if not available.
          */
         if ( psfTotalFlux <= 0 || psfTotalPowerFlux <= 0 ||
              psfTotalMeanFlux <= 0 || psfTotalMeanPowerFlux <= 0 ||
              MStar <= 0 || NStar <= 0 || psfCount < 1 )
         {
            Console().WarningLn( "<end><cbr>** Warning: PSF signal estimates are not available in the image metadata and are being "
                                 "calculated from possibly non-raw or uncalibrated data. Image weights can be inaccurate." );
            PSFSignalEstimator E;
            E.Detector().EnableClusteredSources();
            E.Detector().DisableLocalMaximaDetection();
            PSFSignalEstimator::Estimates e = E( m_subframe );
            psfTotalFlux = e.totalFlux;
            psfTotalPowerFlux = e.totalPowerFlux;
            psfTotalMeanFlux = e.totalMeanFlux;
            psfTotalMeanPowerFlux = e.totalMeanPowerFlux;
            psfCount = e.count;
            MStar = e.MStar;
            NStar = e.NStar;
         }

         PSFSignalEstimator::Estimates E{ psfTotalFlux, psfTotalPowerFlux,
                                          psfTotalMeanFlux, psfTotalMeanPowerFlux,
                                          MStar, NStar, int( psfCount ) };
         m_psfSignalWeight = PSFSignalEstimator::PSFSignalWeight( E, m_noise );
         m_psfSNR = PSFSignalEstimator::PSFSNR( E, m_noise );
         m_psfFlux = psfTotalFlux;
         m_psfFluxPower = psfTotalPowerFlux;
         m_psfTotalMeanFlux = psfTotalMeanFlux;
         m_psfTotalMeanPowerFlux = psfTotalMeanPowerFlux;
         m_psfCount = psfCount;
         m_MStar = MStar;
         m_NStar = NStar;

         /*
          * Compute noise scaling factors if not available.
          */
         if ( noiseScaleLow == 0 || noiseScaleHigh == 0 )
         {
            Console().WarningLn( "<end><cbr>** Warning: Noise scaling factors are not available in the image metadata and are being "
                                 "calculated from possibly non-raw or uncalibrated data. Image weights can be inaccurate." );

            const double clipLow = 2.0/65535;
            const double clipHigh = 1.0 - 2.0/65535;

            m_subframe.SetRangeClipping( clipLow, clipHigh );
            double center = m_subframe.Median();

            m_subframe.SetRangeClipping( clipLow, center );
            m_subframe.SetRangeClipping( Max( clipLow, center - 4*m_subframe.StdDev() ), center );
            m_subframe.SetRangeClipping( Max( clipLow, center - 4*m_subframe.StdDev() ), center );
            noiseScaleLow = m_subframe.StdDev();

            m_subframe.SetRangeClipping( center, clipHigh );
            m_subframe.SetRangeClipping( center, Min( center + 4*m_subframe.StdDev(), clipHigh ) );
            m_subframe.SetRangeClipping( center, Min( center + 4*m_subframe.StdDev(), clipHigh ) );
            noiseScaleHigh = m_subframe.StdDev();

            m_subframe.ResetRangeClipping();
         }

         /*
          * SNR
          */
         double e = double( TwoSidedEstimate( noiseScaleLow, noiseScaleHigh ) )/m_noise;
         if ( IsFinite( e ) && 1 + e*e != 1 )
            m_snrWeight = e*e;

         /*
          * PSF Scale SNR
          */
         if ( !m_nmlPath.IsEmpty() )
         {
            LocalNormalizationData nml;
            {
               static Mutex mutex;
               static AtomicInt count;
               volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileReadThreads );
               nml.Parse( m_nmlPath );
            }

            if ( !nml.IsTaggedAsInvalid() )
            {
               const Vector& s = nml.RelativeScaleFactors();
               if ( !s.IsEmpty() )
               {
                  if ( s.Length() == 1 )
                     m_psfScale = s[0];
                  else
                     m_psfScale = 0.5*(s.MinComponent() + s.MaxComponent());

                  m_psfScaleSNR = 1/m_psfScale/m_psfScale/m_noise/m_noise;
               }
            }
         }
      }

      /*
       * Get horizontal coordinates from image metadata
       */
      WCSKeywords wcs( properties, keywords );
      if (  wcs.objctra.IsDefined()
         && wcs.objctdec.IsDefined()
         && wcs.dateobs.IsDefined()
         && wcs.longobs.IsDefined()
         && wcs.latobs.IsDefined() )
      {
         /*
          * If possible, compute horizontal coordinates from the equatorial
          * coordinates of the center of the image and the geodetic coordinates
          * of the observation location.
          */
         Position P( wcs.dateobs(), "UTC" );
         double sd, cd;
         SinCos( Rad( wcs.objctdec() ), sd, cd );
         double sh, ch;
         SinCos( P.GAST() - Rad( wcs.objctra() ) + Rad( wcs.longobs() ), sh, ch );
         double sp, cp;
         SinCos( Rad( wcs.latobs() ), sp, cp );
         m_outputData.altitude = Deg( ArcSin( sd*sp + cd*ch*cp ) );
         m_outputData.azimuth = Deg( ArcTan2Pi( -cd*sh, sd*cp - cd*ch*sp ) );
      }
      else
      {
         /*
          * Otherwise try to read the nonstandard FITS keywords OBJCTALT and
          * OBJCTAZ (SBFITSEXT convention).
          */
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( "OBJCTALT" ) )
            {
               if ( keyword.IsNumeric() )
               {
                  double deg = 0;
                  keyword.GetNumericValue( deg );
                  if ( deg >= 0 && deg <= 90 )
                     m_outputData.altitude = deg;
               }
               break;
            }
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( "OBJCTAZ" ) )
            {
               if ( keyword.IsNumeric() )
               {
                  double deg = 0;
                  keyword.GetNumericValue( deg );
                  if ( deg >= 0 && deg < 360 )
                     m_outputData.altitude = deg;
               }
               break;
            }
      }

      m_outputData.path = m_filePath;
   }

   void Perform()
   {
      Console console;
      console.NoteLn( "<end><cbr><br>Measuring subframe: <raw>" + m_outputData.path + "</raw>" );

      if ( IsAborted() )
         throw ProcessAborted();

      if ( m_instance.p_routine == SSRoutine::MeasureSubframes )
      {
         MeasureImage();
         if ( IsAborted() )
            throw ProcessAborted();
      }

      // Crop if the ROI was set
      if ( m_instance.p_roi.IsRect() )
         m_subframe.CropTo( m_instance.p_roi );

      // Run the star detector
      StarDetector::star_list stars = DetectStars();
      if ( stars.IsEmpty() )
         if ( m_throwsOnMeasurementError )
            throw Error( "No stars detected" );

      if ( IsAborted() )
         throw ProcessAborted();

      // Run the PSF Fitter
      psf_list fits = FitPSFs( stars.Begin(),
                        (m_instance.p_maxPSFFits > 0 && size_type( m_instance.p_maxPSFFits ) < stars.Length()) ?
                        stars.At( m_instance.p_maxPSFFits ) : stars.End() );
      if ( fits.IsEmpty() )
         if ( m_throwsOnMeasurementError )
            throw Error( "No PSF could be fitted" );

      if ( IsAborted() )
         throw ProcessAborted();

      // Measure PSF data
      MeasurePSFs( fits );

      /*
       * N.B. Since core version 1.8.9-1: The Stars property is now the number
       * of detected stars instead of the number of fitted PSFs.
       */
      m_outputData.stars = stars.Length();

      console.WriteLn( String().Format( "Stars detected: %6u", stars.Length() ) );
      console.WriteLn( String().Format( "Valid PSF fits: %6u (%6.2f%%)", fits.Length(),
                                        100.0*fits.Length()/Min( stars.Length(), size_type( m_instance.p_maxPSFFits ) ) ) );
   }

   void EvaluateSignalAndNoise()
   {
      if ( m_noise > 0 && m_noiseRatio > 0 )
      {
         m_outputData.noise = m_noise;
         m_outputData.noiseRatio = m_noiseRatio;
      }
      else
      {
         Console().WarningLn( "<end><cbr>** Warning: Noise estimates are not available in the image metadata and are being "
                              "calculated from possibly non-raw or uncalibrated data. Image weights can be inaccurate." );
         double noiseEstimate = 0;
         double noiseFraction = 0;
         double noiseEstimateKS = 0;
         double noiseFractionKS = 0;
         SeparableFilter H( s_5x5B3Spline_hv, s_5x5B3Spline_hv, 5 );
         for ( int n = 4;; )
         {
            ATrousWaveletTransform W( H, n );
            W << m_subframe;

            size_type N;
            if ( n == 4 )
            {
               noiseEstimateKS = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/s_5x5B3Spline_kj[0];
               noiseFractionKS = double( N )/m_subframe->NumberOfPixels();
            }
            noiseEstimate = W.NoiseMRS( m_subframe, s_5x5B3Spline_kj, noiseEstimateKS, 3, &N );
            noiseFraction = double( N )/m_subframe->NumberOfPixels();

            if ( noiseEstimate > 0 )
               if ( noiseFraction >= 0.01 )
                  break;

            if ( --n == 1 )
            {
               noiseEstimate = noiseEstimateKS;
               noiseFraction = noiseFractionKS;
               break;
            }
         }

         m_outputData.noise = noiseEstimate;
         m_outputData.noiseRatio = noiseFraction;
      }

      if ( m_psfSignalWeight > 0 && m_psfSNR > 0 && m_psfFlux > 0 && m_psfFluxPower > 0 &&
           m_psfTotalMeanFlux > 0 && m_psfTotalMeanPowerFlux > 0 && m_psfCount > 0 )
      {
         m_outputData.psfSignalWeight = m_psfSignalWeight;
         m_outputData.psfSNR = m_psfSNR;
         m_outputData.psfFlux = m_psfFlux;
         m_outputData.psfFluxPower = m_psfFluxPower;
         m_outputData.psfTotalMeanFlux = m_psfTotalMeanFlux;
         m_outputData.psfTotalMeanPowerFlux = m_psfTotalMeanPowerFlux;
         m_outputData.psfCount = m_psfCount;
         m_outputData.MStar = m_MStar;
         m_outputData.NStar = m_NStar;
      }
      else
      {
         Console().WarningLn( "<end><cbr>** Warning: PSF signal estimates are not available in the image metadata and are being "
                              "calculated from possibly non-raw or uncalibrated data. Image weights can be inaccurate." );
         PSFSignalEstimator E;
         E.Detector().EnableClusteredSources();
         E.Detector().DisableLocalMaximaDetection();
         PSFSignalEstimator::Estimates e = E( m_subframe );
         m_outputData.psfSignalWeight = PSFSignalEstimator::PSFSignalWeight( e, m_outputData.noise );
         m_outputData.psfSNR = PSFSignalEstimator::PSFSNR( e, m_outputData.noise );
         m_outputData.psfFlux = e.totalFlux;
         m_outputData.psfFluxPower = e.totalPowerFlux;
         m_outputData.psfTotalMeanFlux = e.totalMeanFlux;
         m_outputData.psfTotalMeanPowerFlux = e.totalMeanPowerFlux;
         m_outputData.psfCount = e.count;
         m_outputData.MStar = e.MStar;
         m_outputData.NStar = e.NStar;
      }

      if ( m_snrWeight > 0 )
      {
         m_outputData.snrWeight = m_snrWeight;
      }
      else
      {
         /*
          * Noise scaling factors
          */
         Console().WarningLn( "<end><cbr>** Warning: Noise scaling factors are not available in the image metadata and are being "
                              "calculated from possibly non-raw or uncalibrated data. Image weights can be inaccurate." );

         const double clipLow = 2.0/65535;
         const double clipHigh = 1 - 2.0/65535;

         m_subframe.SetRangeClipping( clipLow, clipHigh );
         double center = m_subframe.Median();

         m_subframe.SetRangeClipping( clipLow, center );
         m_subframe.SetRangeClipping( Max( clipLow, center - 4*m_subframe.StdDev() ), center );
         m_subframe.SetRangeClipping( Max( clipLow, center - 4*m_subframe.StdDev() ), center );
         double noiseScaleLow = m_subframe.StdDev();

         m_subframe.SetRangeClipping( center, clipHigh );
         m_subframe.SetRangeClipping( center, Min( center + 4*m_subframe.StdDev(), clipHigh ) );
         m_subframe.SetRangeClipping( center, Min( center + 4*m_subframe.StdDev(), clipHigh ) );
         double noiseScaleHigh = m_subframe.StdDev();

         m_subframe.ResetRangeClipping();

         /*
          * SNR estimate
          */
         double e = (noiseScaleLow + noiseScaleHigh)/m_outputData.noise;
         if ( IsFinite( e ) && 1 + e*e != 1 )
            m_outputData.snrWeight = e*e;
      }

      if ( m_psfScale > 0 && m_psfScaleSNR > 0 )
      {
         m_outputData.psfScale = m_psfScale;
         m_outputData.psfScaleSNR = m_psfScaleSNR;
      }
      else
      {
         /*
          * PSF Scale SNR
          */
         if ( !m_nmlPath.IsEmpty() )
         {
            LocalNormalizationData nml;
            {
               static Mutex mutex;
               static AtomicInt count;
               volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileReadThreads );
               nml.Parse( m_nmlPath );
            }

            if ( !nml.IsTaggedAsInvalid() )
            {
               const Vector& s = nml.RelativeScaleFactors();
               if ( !s.IsEmpty() )
               {
                  if ( s.Length() == 1 )
                     m_outputData.psfScale = s[0];
                  else
                     m_outputData.psfScale = 0.5*(s.MinComponent() + s.MaxComponent());

                  m_outputData.psfScaleSNR = 1/m_outputData.psfScale/m_outputData.psfScale
                                              /m_outputData.noise/m_outputData.noise;
               }
            }
         }
      }
   }

   void MeasureImage()
   {
      /*
       * Reject saturated areas for median calculation. Clipping points are
       * relative to extreme sample values.
       */
      double min, max;
      m_subframe.GetExtremeSampleValues( min, max );
      m_subframe.SetRangeClipping( min + 1.0/65535, max - 1.0/65535 );

      /*
       * Robust estimate of location: median.
       */
      m_outputData.median = m_subframe.Median();

      /*
       * Robust estimate of scale: trimmed mean deviation from the median.
       */
      m_subframe.SetRangeClipping( m_outputData.median - (1 - m_instance.p_trimmingFactor)*(m_outputData.median - min),
                                   m_outputData.median + (1 - m_instance.p_trimmingFactor)*(max - m_outputData.median) );
      m_outputData.medianMeanDev = m_subframe.AvgDev( m_outputData.median );

      m_subframe.ResetRangeClipping();

      /*
       * Subtract a possibly nonzero additive pedestal from the location
       * estimate. The medianMeanDev property is location-invariant.
       */
      m_outputData.median -= m_pedestal/65535.0;

      /*
       * Robust signal and noise estimates.
       */
      EvaluateSignalAndNoise();
   }

   StarDetector::star_list DetectStars() const
   {
      // Setup StarDetector parameters and find the list of stars
      StarDetector S;
      S.SetStructureLayers( m_instance.p_structureLayers );
      S.SetNoiseLayers( m_instance.p_noiseLayers );
      S.SetHotPixelFilterRadius( m_instance.p_hotPixelFilterRadius );
      S.SetNoiseReductionFilterRadius( m_instance.p_noiseReductionFilterRadius );
      S.SetMinStructureSize( m_instance.p_minStructureSize );
      S.SetSensitivity( m_instance.p_sensitivity );
      S.SetPeakResponse( m_instance.p_peakResponse );
      S.SetBrightThreshold( m_instance.p_brightThreshold );
      S.SetMaxDistortion( m_instance.p_maxDistortion );
      S.EnableClusteredSources( m_instance.p_allowClusteredSources );
      S.DisableLocalMaximaDetection( m_instance.p_allowClusteredSources );
      S.SetUpperLimit( m_instance.p_upperLimit );

      StarDetector::star_list stars = S.DetectStars( m_subframe );

      if ( m_instance.p_routine == SSRoutine::StarDetectionPreview )
         if ( IsRootThread() )
            {
               ImageWindow window( 1, 1, 1, 8/*bitsPerSample*/, false/*floatSample*/,
                                   false/*color*/, true/*history*/, "structure_map" );
               if ( window.IsNull() )
                  throw Error( "Unable to create image window: structure_map" );
               window.MainView().Image().CopyImage( S.StructureMap( m_subframe ) );
               window.Show();
            }

      return stars;
   }

   psf_list FitPSFs( StarDetector::star_list::const_iterator begin, StarDetector::star_list::const_iterator end )
   {
      psf_list PSFs;
      for ( StarDetector::star_list::const_iterator i = begin; i != end; ++i )
      {
         // Adaptive PSF sampling region
         int size = Max( i->rect.Width(), i->rect.Height() );
         Rect rect = DRect( i->pos + (0.5-size), i->pos + (0.5+size) ).TruncatedToInt();
         for ( double m0 = 1; ; )
         {
            double m = FMatrix::FromImage( m_subframe, rect ).Median();
            if ( m0 <= m || (m0 - m)/m0 < 0.01 )
               break;
            m0 = m;
            rect.InflateBy( 1, 1 );
         }

         PSFFit fit( m_subframe, i->pos, rect, PSFFunction(), m_instance.p_psfFitCircular );
         if ( fit )
            PSFs << fit.psf;
      }
      return PSFs;
   }

   PSFFit::psf_function PSFFunction()
   {
      switch ( m_instance.p_psfFit )
      {
      case SSPSFFit::Gaussian:      return PSFunction::Gaussian;
      case SSPSFFit::Moffat10:      return PSFunction::MoffatA;
      case SSPSFFit::Moffat8:       return PSFunction::Moffat8;
      case SSPSFFit::Moffat6:       return PSFunction::Moffat6;
      default: // ?!
      case SSPSFFit::Moffat4:       return PSFunction::Moffat4;
      case SSPSFFit::Moffat25:      return PSFunction::Moffat25;
      case SSPSFFit::Moffat15:      return PSFunction::Moffat15;
      case SSPSFFit::Lorentzian:    return PSFunction::Lorentzian;
//       case SSPSFFit::VariableShape: return PSFunction::VariableShape;
      }
   }

   void MeasurePSFs( const psf_list& fits )
   {
      if ( m_instance.p_routine == SSRoutine::StarDetectionPreview )
      {
         if ( IsRootThread() )
            if ( !fits.IsEmpty() )
            {
               Bitmap bmp( m_subframe.Width(), m_subframe.Height() );
               bmp.Fill( 0 ); // transparent

               VectorGraphics G( bmp );
               G.EnableAntialiasing();
               G.SetPen( 0xff000000 ); // solid black
               for ( const PSFData& psf : fits )
                  if ( psf )
                  {
                     G.DrawLine( psf.c0.x, psf.c0.y-5, psf.c0.x, psf.c0.y-1 );
                     G.DrawLine( psf.c0.x, psf.c0.y+1, psf.c0.x, psf.c0.y+5 );
                     G.DrawLine( psf.c0.x-5, psf.c0.y, psf.c0.x-1, psf.c0.y );
                     G.DrawLine( psf.c0.x+1, psf.c0.y, psf.c0.x+5, psf.c0.y );
                     G.DrawEllipse( psf.Bounds().InflatedBy( 2 ) );
                  }
               G.EndPaint();

               ImageWindow window( 1, 1, 1, 8/*bitsPerSample*/, false/*floatSample*/,
                                 false/*color*/, true/*history*/, "stars" );
               if ( window.IsNull() )
                  throw Error( "Unable to create image window: stars" );
               ImageVariant image = window.MainView().Image();
               image.AllocateData( m_subframe.Width(), m_subframe.Height() );
               image.White();
               image.Blend( bmp );
               window.Show();
            }

         return;
      }

      /*
       * N.B. Since core version 1.8.9-1: The Stars property is now the number
       * of detected stars instead of the number of fitted PSFs.
       */
//       m_outputData.stars = fits.Length();

      Array<double> fwhms, eccentricities, residuals;
      for ( const PSFData& fit : fits )
      {
         fwhms << PSFData::FWHM( PSFFunction(), Sqrt( fit.sx * fit.sy ) );
         eccentricities << Sqrt( 1 - fit.sy*fit.sy/fit.sx/fit.sx );
         residuals << fit.mad;
      }

      /*
       * ### TODO: Replace the following xxxMeanDev properties with their
       *           corresponding RCR standard deviations (and hence the calls
       *           to AvgDev() will be removed). The properties should then be
       *           renamed to xxxSigma.
       */
      int i, j;
      RobustChauvenetRejection()( i, j, m_outputData.fwhm, m_outputData.fwhmMeanDev, fwhms );
      m_outputData.fwhmMeanDev = AvgDev( fwhms.At( i ), fwhms.At( j ), m_outputData.fwhm );

      RobustChauvenetRejection()( i, j, m_outputData.eccentricity, m_outputData.eccentricityMeanDev, eccentricities );
      m_outputData.eccentricityMeanDev = AvgDev( eccentricities.At( i ), eccentricities.At( j ), m_outputData.eccentricity );

      RobustChauvenetRejection()( i, j, m_outputData.starResidual, m_outputData.starResidualMeanDev, residuals );
      m_outputData.starResidualMeanDev = AvgDev( residuals.At( i ), residuals.At( j ), m_outputData.starResidual );
   }
};

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
    whyNot = "SubframeSelector can only be executed in the global context.";
    return false;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::IsHistoryUpdater( const View& view ) const
{
    return false;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::CanTestStarDetector( String &whyNot ) const
{
   if ( p_subframes.IsEmpty() )
   {
      whyNot = "No subframes have been specified.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInstance::TestStarDetector()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanTestStarDetector( why ) )
         throw Error( why );

      for ( const SubframeItem& subframe : p_subframes )
         if ( subframe.enabled )
            if ( !File::Exists( subframe.path ) )
               throw Error( "No such file exists on the local filesystem: " + subframe.path );
   }

   Console console;
   console.EnableAbort();
   Module->ProcessEvents();

   try
   {
      /*
       * Load and measure the first target frame from the targets list.
       */
      SubframeSelectorMeasureThread thread( *this, 0/*index*/ );

      // Keep the GUI responsive, last chance to abort
      Module->ProcessEvents();
      if ( console.AbortRequested() )
         throw ProcessAborted();

      thread.Run();
   }
   catch ( ProcessAborted& )
   {
      /*
       * The user has requested to abort the process.
       */
      throw;
   }
   catch ( ... )
   {
      if ( console.AbortRequested() )
         throw ProcessAborted();

      try
      {
         throw;
      }
      ERROR_HANDLER

      console.ResetStatus();
      console.EnableAbort();

      throw ProcessAborted();
   }
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::CanMeasure( String &whyNot ) const
{
   if ( p_subframes.IsEmpty() )
   {
      whyNot = "No subframes have been specified.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInstance::Measure()
{
   /*
    * Reset measured values.
    */
   o_measures.Clear();

   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanMeasure( why ) )
         throw Error( why );

      for ( const SubframeItem& subframe : p_subframes )
         if ( subframe.enabled )
            if ( !File::Exists( subframe.path ) )
               throw Error( "No such file exists on the local filesystem: " + subframe.path );
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
    * Generate a cache key with cache-sensitive parameters.
    */
   m_cacheKey = IsoString::ToHex( MD5().Hash(   IsoString().Format( "%.2f", p_trimmingFactor )
                                             << IsoString( p_structureLayers )
                                             << IsoString( p_noiseLayers )
                                             << IsoString( p_hotPixelFilterRadius )
                                             << IsoString( p_noiseReductionFilterRadius )
                                             << IsoString( p_minStructureSize )
                                             << IsoString().Format( "%.4f", p_sensitivity )
                                             << IsoString().Format( "%.4f", p_peakResponse )
                                             << IsoString().Format( "%.4f", p_brightThreshold )
                                             << IsoString().Format( "%.4f", p_maxDistortion )
                                             << IsoString( bool( p_allowClusteredSources ) )
                                             << IsoString().Format( "%.4f", p_upperLimit )
                                             << IsoString( p_psfFit )
                                             << IsoString( bool( p_psfFitCircular ) )
                                             << IsoString( p_maxPSFFits )
                                             << IsoString( p_roi.x0 )
                                             << IsoString( p_roi.y0 )
                                             << IsoString( p_roi.x1 )
                                             << IsoString( p_roi.y1 )
                                             << IsoString( p_pedestal )
                                             << IsoString( p_pedestalMode )
                                             << IsoString( p_pedestalKeyword )
                                             << IsoString( p_inputHints ) ) );

   /*
    * Allow the user to abort the process.
    */
   Console console;
   console.EnableAbort();

   /*
    * Setup the cache.
    */
   if ( TheSubframeSelectorCache == nullptr )
   {
      new SubframeSelectorCache;

      try
      {
         if ( TheSubframeSelectorCache->IsPersistent() )
         {
            TheSubframeSelectorCache->Load();

            if ( TheSubframeSelectorCache->IsEmpty() )
               console.NoteLn( "<end><cbr><br>* Empty file cache" );
            else
               console.NoteLn( "<end><cbr><br>* Loaded cache: " + String( TheSubframeSelectorCache->NumberOfItems() ) + " item(s)" );
         }
      }
      catch ( ... )
      {
         TheSubframeSelectorCache->Purge();
         try
         {
            throw;
         }
         ERROR_HANDLER;
      }
   }

   /*
    * Begin subframe measuring process.
    */
   int succeeded = 0;
   int failed = 0;
   int skipped = 0;

   /*
    * Pending subframes list. We use this list for temporary storage of indices
    * that need measuring.
    */
   Array<size_type> pendingItems;
   for ( size_type i = 0; i < p_subframes.Length(); ++i )
      if ( p_subframes[i].enabled )
         pendingItems << i;
      else
      {
         console.NoteLn( "* Skipping disabled target: <raw>" + p_subframes[i].path + "</raw>" );
         ++skipped;
      }
   size_type pendingItemsTotal = pendingItems.Length();

   console.WriteLn( String().Format( "<end><cbr><br>Measuring %u subframes.", pendingItemsTotal ) );

   Module->ProcessEvents();

   if ( p_useFileThreads && pendingItems.Length() > 1 )
   {
      /*
       * Running threads list. Note that IndirectArray<> initializes all of its
       * contained pointers to nullptr.
       */
      int numberOfThreadsAvailable = RoundInt( Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 ) * p_fileThreadOverload );
      int numberOfThreads = Min( numberOfThreadsAvailable, int( pendingItems.Length() ) );
      measure_thread_list runningThreads( numberOfThreads );

      console.NoteLn( String().Format( "<end><br>* Using %d worker threads.", numberOfThreads ) );
      console.NoteLn( String().Format(          "* Using %d file reading threads.<br/>", m_maxFileReadThreads ) );
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
               for ( measure_thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
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
                     {
                        /*
                         * Store output data.
                         */
                        MeasureItem m( (*i)->Index() );
                        m.Input( (*i)->OutputData() );
                        o_measures << m;
                     }
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
                     *i = new SubframeSelectorMeasureThread( *this,
                                                             *pendingItems/*itemIndex*/,
                                                             !p_nonInteractive/*throwsOnMeasurementError*/ );
                     int threadIndex = i - runningThreads.Begin();
                     console.NoteLn( String().Format( "<end><cbr>[%03d] <raw>", threadIndex ) + (*i)->TargetFilePath() + "</raw>" );
                     (*i)->Start( ThreadPriority::DefaultMax, threadIndex );
                     ++running;
                     if ( pendingItems.Length() == 1 )
                        console.NoteLn( "<br>* Waiting for running tasks to terminate...<br>" );
                     else if ( succeeded+failed > 0 )
                        console.WriteLn();

                     pendingItems.Remove( pendingItems.Begin() );
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
         for ( SubframeSelectorMeasureThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Abort();
         for ( SubframeSelectorMeasureThread* thread : runningThreads )
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
            SubframeSelectorMeasureThread thread( *this,
                                                  itemIndex,
                                                  !p_nonInteractive/*throwsOnMeasurementError*/ );
            thread.Run();
            MeasureItem m( itemIndex );
            m.Input( thread.OutputData() );
            o_measures << m;
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
         throw Error( "No images were measured: Empty target frames list? No enabled target frames?" );
      throw Error( "No image could be measured." );
   }

   console.NoteLn( String().Format( "<end><cbr><br>===== SubframeSelector: %u succeeded, %u failed, %u skipped =====",
                                    succeeded, failed, skipped ) );

   o_measures.Sort( SubframeSortingBinaryPredicate( SSSortingProperty::Index, 0/*ascending*/ ) );

   if ( !p_nonInteractive )
      if ( TheSubframeSelectorInterface != nullptr )
      {
         TheSubframeSelectorInterface->m_instance.Assign( *this );
         TheSubframeSelectorInterface->UpdateControls();
         TheSubframeSelectorInterface->ShowExpressionsInterface();
         TheSubframeSelectorInterface->ShowMeasurementsInterface();
      }

   m_cacheKey.Clear();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInstance::ApproveMeasurements()
{
   if ( !o_measures.IsEmpty() )
      if ( p_approvalExpression.IsEmpty() )
      {
         o_measures.Apply( []( MeasureItem& item )
                           {
                              if ( !item.locked )
                                 item.enabled = true;
                           } );
      }
      else
      {
         // First, get all Medians and Mean Deviation from Medians for Sigma units
         MeasureProperties properties = MeasureProperties();
         MeasureUtils::MeasureProperties( o_measures, p_subframeScale, p_scaleUnit,
                                          p_cameraGain, p_cameraResolution, p_dataUnit,
                                          properties );

         for ( MeasureItem& item : o_measures )
         {
            if ( item.locked )
               continue;

            // The standard parameters for the MeasureItem
            String scriptSource = item.JavaScriptParameters( p_subframeScale, p_scaleUnit, p_cameraGain,
                                                            TheSSCameraResolutionParameter->ElementData( p_cameraResolution ),
                                                            p_dataUnit, properties );

            // The final expression that evaluates to a return value
            scriptSource += p_approvalExpression;

            // Try to get the final result and update the MeasureItem
            Variant result = Module->EvaluateScript( scriptSource.DecodedHTMLSpecialChars(), "JavaScript" );
            if ( !result.IsValid() )
               throw Error( "SubframeSelector: Approval error: Invalid script execution." );
            String resultText = result.ToString();
            if ( resultText.Contains( "Error" ) )
               throw Error( resultText );

            item.enabled = result.ToBool();
         }
      }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInstance::WeightMeasurements()
{
   if ( !o_measures.IsEmpty() )
      if ( p_weightingExpression.IsEmpty() )
      {
         o_measures.Apply( []( MeasureItem& item )
                           {
                              item.weight = 0;
                           } );
      }
      else
      {
         // First, get all Medians and Mean Deviation from Medians for Sigma units
         MeasureProperties properties = MeasureProperties();
         MeasureUtils::MeasureProperties( o_measures, p_subframeScale, p_scaleUnit,
                                          p_cameraGain, p_cameraResolution, p_dataUnit,
                                          properties );

         for ( MeasureItem& item : o_measures )
         {
            // The standard parameters for the MeasureItem
            String scriptSource = item.JavaScriptParameters( p_subframeScale, p_scaleUnit, p_cameraGain,
                                                            TheSSCameraResolutionParameter->ElementData( p_cameraResolution ),
                                                            p_dataUnit, properties );

            // The final expression that evaluates to a return value
            scriptSource += p_weightingExpression;

            // Try to get the final result and update the MeasureItem
            Variant result = Module->EvaluateScript( scriptSource.DecodedHTMLSpecialChars(), "JavaScript" );
            if ( !result.IsValid() )
               throw Error( "SubframeSelector: Weighting error: Invalid script execution." );
            String resultText = result.ToString();
            if ( resultText.Contains( "Error" ) )
               throw Error( resultText );

            item.weight = result.ToFloat();
         }
      }
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::CanOutput( String &whyNot ) const
{
   if ( o_measures.IsEmpty() )
   {
      whyNot = "No measurements have been made.";
      return false;
   }

   if ( p_outputKeyword.IsEmpty() )
   {
      whyNot = "The specified output keyword is blank.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

class SubframeSelectorOutputThread : public Thread
{
public:

   SubframeSelectorOutputThread( const SubframeSelectorInstance& instance, size_type itemIndex )
      : m_instance( instance )
      , m_item( m_instance.p_subframes[m_instance.o_measures[itemIndex].index] )
      , m_weight( m_instance.o_measures[itemIndex].weight )
   {
   }

   void Run() override
   {
      try
      {
         Module->ProcessEvents();
         Perform();
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

   const String& TargetFilePath() const
   {
      return m_item.path;
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

   const SubframeSelectorInstance&               m_instance;
   const SubframeSelectorInstance::SubframeItem& m_item;
         double                                  m_weight;
         String                                  m_errorInfo;
         bool                                    m_success = false;

   void Perform()
   {
      Console console;

      /*
       * Output directory.
       */
      String fileDir = m_instance.p_outputDirectory.Trimmed();
      if ( fileDir.IsEmpty() )
         fileDir = File::ExtractDrive( m_item.path ) + File::ExtractDirectory( m_item.path );
      if ( fileDir.IsEmpty() )
         throw Error( m_item.path + ": Unable to determine an output directory." );
      if ( !fileDir.EndsWith( '/' ) )
         fileDir.Append( '/' );

      /*
       * Input file extension, which defines the input file format.
       */
      String inputFileExtension = File::ExtractExtension( m_item.path ).Trimmed();
      if ( inputFileExtension.IsEmpty() )
         throw Error( m_item.path + ": Unable to determine an input file extension." );

      /*
       * Output file extension, which defines the output file format.
       */
      String outputFileExtension = m_instance.p_outputExtension.Trimmed();
      if ( outputFileExtension.IsEmpty() )
         outputFileExtension = inputFileExtension;
      else if ( !outputFileExtension.StartsWith( '.' ) )
         outputFileExtension.Prepend( '.' );

      /*
       * Output file name.
       */
      String fileName = File::ExtractName( m_item.path ).Trimmed();
      if ( !m_instance.p_outputPrefix.IsEmpty() )
         fileName.Prepend( m_instance.p_outputPrefix );
      if ( !m_instance.p_outputPostfix.IsEmpty() )
         fileName.Append( m_instance.p_outputPostfix );
      if ( fileName.IsEmpty() )
         throw Error( m_item.path + ": Unable to determine an output file name." );

      /*
       * Output file paths.
       */
      String outputFilePath = fileDir + fileName + outputFileExtension;

      String outputFilePathXNML;
      if ( !m_item.nmlPath.IsEmpty() )
         outputFilePathXNML = fileDir + fileName + ".xnml";

      String outputFilePathXDRZ;
      if ( !m_item.drzPath.IsEmpty() )
         outputFilePathXDRZ = fileDir + fileName + ".xdrz";

      console.WriteLn( "<end><cbr><br>Input  : <raw>" + m_item.path + "</raw>" );
      console.WriteLn(               "Output : <raw>" + outputFilePath + "</raw>" );
      if ( !outputFilePathXNML.IsEmpty() )
      {
         console.WriteLn(            "Input  : <raw>" + m_item.nmlPath + "</raw>" );
         console.WriteLn(            "Output : <raw>" + outputFilePathXNML + "</raw>" );
      }
      if ( !outputFilePathXDRZ.IsEmpty() )
      {
         console.WriteLn(            "Input  : <raw>" + m_item.drzPath + "</raw>" );
         console.WriteLn(            "Output : <raw>" + outputFilePathXDRZ + "</raw>" );
      }

      /*
       * Check for already existing files, and either overwrite them (but show
       * warning messages if that happens), or find unique file names,
       * depending on the overwriteExistingFiles parameter.
       */
      UniqueFileChecks checks = File::EnsureNewUniqueFile( outputFilePath, m_instance.p_overwriteExistingFiles );
      if ( checks.overwrite )
      {
         /*
          * Prevent losing input files with copy/overwrite or move operations.
          */
         if ( File::SameFile( m_item.path, outputFilePath ) )
            throw Error( "Blocked attempt to overwrite an input file: " + m_item.path );

         console.WarningLn( "** Warning: Overwriting existing file." );
      }
      else if ( checks.exists )
         console.NoteLn( "* File already exists, writing to: <raw>" + outputFilePath + "</raw>" );

      if ( !outputFilePathXNML.IsEmpty() )
      {
         checks = File::EnsureNewUniqueFile( outputFilePathXNML, m_instance.p_overwriteExistingFiles );
         if ( checks.overwrite )
         {
            if ( File::SameFile( m_item.nmlPath, outputFilePathXNML ) )
               throw Error( "Blocked attempt to overwrite an input file: " + m_item.nmlPath );
            console.WarningLn( "** Warning: Overwriting existing file." );
         }
         else if ( checks.exists )
            console.NoteLn( "* File already exists, writing to: <raw>" + outputFilePathXNML + "</raw>" );
      }

      if ( !outputFilePathXDRZ.IsEmpty() )
      {
         checks = File::EnsureNewUniqueFile( outputFilePathXDRZ, m_instance.p_overwriteExistingFiles );
         if ( checks.overwrite )
         {
            if ( File::SameFile( m_item.drzPath, outputFilePathXDRZ ) )
               throw Error( "Blocked attempt to overwrite an input file: " + m_item.drzPath );
            console.WarningLn( "** Warning: Overwriting existing file." );
         }
         else if ( checks.exists )
            console.NoteLn( "* File already exists, writing to: <raw>" + outputFilePathXDRZ + "</raw>" );
      }

      /*
       * Find installed file formats able to perform the requested read/write
       * operations ...
       */
      FileFormat inputFormat( inputFileExtension, true/*read*/, false/*write*/ );
      FileFormat outputFormat( outputFileExtension, false/*read*/, true/*write*/ );

      if ( outputFormat.IsDeprecated() )
         console.WarningLn( "** Warning: Deprecated file format: " + outputFormat.Name() );

      /*
       * ... and create format instances (usually disk files).
       */
      FileFormatInstance inputFile( inputFormat );
      FileFormatInstance outputFile( outputFormat );

      /*
       * Open the input stream.
       */
      ImageDescriptionArray images;
      if ( !inputFile.Open( images, m_item.path, m_instance.p_inputHints ) )
         throw CaughtException();

      /*
       * Check for an empty subframe.
       */
      if ( images.IsEmpty() )
         throw Error( m_item.path + ": Empty subframe image." );

      /*
       * Multiple-image files not supported for calibration.
       */
      if ( images.Length() > 1 )
         console.NoteLn( String().Format( "* Ignoring %u additional image(s) in subframe file.", images.Length()-1 ) );

      /*
       * Create a shared image with the same pixel sample format as the
       * input image.
       */
      ImageVariant image;
      image.CreateSharedImage( images[0].options.ieeefpSampleFormat,
                               images[0].options.complexSample,
                               images[0].options.bitsPerSample );
      {
         static Mutex mutex;
         static AtomicInt count;
         volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileReadThreads );

         /*
          * Read the input image.
          */
         if ( !inputFile.ReadImage( image ) )
            throw CaughtException();
      }

      /*
       * Create the output stream.
       */
      if ( !outputFile.Create( outputFilePath, m_instance.p_outputHints ) )
         throw CaughtException();

      /*
       * Determine the output sample format: bits per sample and whether
       * integer or real samples.
       */
      outputFile.SetOptions( images[0].options );

      /*
       * File formats often use format-specific data. Try to keep track of
       * private data structures.
       */
      if ( inputFormat.UsesFormatSpecificData() )
         if ( outputFormat.ValidateFormatSpecificData( inputFile.FormatSpecificData() ) )
            outputFile.SetFormatSpecificData( inputFile.FormatSpecificData() );

      /*
       * Set image properties.
       */
      if ( inputFormat.CanStoreImageProperties() )
         if ( !inputFile.ReadImageProperties().IsEmpty() )
            if ( outputFormat.CanStoreImageProperties() && outputFormat.SupportsViewProperties() )
               outputFile.WriteImageProperties( inputFile.ReadImageProperties() );
            else
               console.WarningLn( "** Warning: The output format cannot store image properties; existing properties will be lost." );

      /*
       * Add FITS header keywords and preserve existing ones, if possible.
       * N.B.: A COMMENT or HISTORY keyword cannot have a value; these keywords
       * only have the name and comment components.
       */
      if ( outputFormat.CanStoreKeywords() )
      {
         FITSKeywordArray keywords;
         if ( inputFormat.CanStoreKeywords() )
            inputFile.ReadFITSKeywords( keywords );

         // Remove old weight keywords
         FITSKeywordArray newKeywords;
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( keyword.name != IsoString( m_instance.p_outputKeyword ) )
               newKeywords << keyword;

         newKeywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Measured with " + PixInsightVersion::AsString() )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), "Measured with " + Module->ReadableVersion() )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), "Measured with SubframeSelector process" );

         if ( !m_instance.p_outputKeyword.IsEmpty() )
            newKeywords << FITSHeaderKeyword( m_instance.p_outputKeyword,
                                             String().Format( "%.6f", m_weight ),
                                             "SubframeSelector.weight" );

         outputFile.WriteFITSKeywords( newKeywords );
      }
      else
         console.WarningLn( "** Warning: The output format cannot store FITS header keywords - subframe weight metadata not embedded" );

      /*
       * Preserve an existing ICC profile if possible.
       */
      if ( inputFormat.CanStoreICCProfiles() )
      {
         ICCProfile inputProfile;
         inputFile.ReadICCProfile( inputProfile );
         if ( inputProfile.IsProfile() )
            if ( outputFormat.CanStoreICCProfiles() )
               outputFile.WriteICCProfile( inputProfile );
            else
               console.WarningLn( "** Warning: The output format cannot store color profiles - original ICC profile not embedded" );
      }

      /*
       * Close the input stream.
       */
      (void)inputFile.Close();

      Module->ProcessEvents();

      {
         static Mutex mutex;
         static AtomicInt count;
         volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileWriteThreads );

         /*
          * Write the output image and close the output stream.
          */
         image.ResetSelections();
         if ( !outputFile.WriteImage( image ) || !outputFile.Close() )
            throw CaughtException();
      }

      if ( !outputFilePathXNML.IsEmpty() )
      {
         LocalNormalizationData nml;
         {
            static Mutex mutex;
            static AtomicInt count;
            volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileReadThreads );
            nml.Parse( m_item.nmlPath );
         }

         nml.SetTargetFilePath( outputFilePath );

         {
            static Mutex mutex;
            static AtomicInt count;
            volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileWriteThreads );
            nml.SerializeToFile( outputFilePathXNML );
         }

         if ( nml.IsTaggedAsInvalid() )
            console.WarningLn( "** Warning: Writing a local normalization data file tagged as invalid." );
      }

      if ( !outputFilePathXDRZ.IsEmpty() )
      {
         DrizzleData drz;
         {
            static Mutex mutex;
            static AtomicInt count;
            volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileReadThreads );
            drz.Parse( m_item.drzPath );
         }

         drz.SetAlignmentTargetFilePath( outputFilePath );

         {
            static Mutex mutex;
            static AtomicInt count;
            volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileWriteThreads );
            drz.SerializeToFile( outputFilePathXDRZ );
         }
      }
   }
};

void SubframeSelectorInstance::Output()
{
   Console console;

   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanOutput( why ) )
         throw Error( why );

      if ( !p_outputDirectory.IsEmpty() )
         if ( !File::DirectoryExists( p_outputDirectory ) )
            throw Error( "The specified output directory does not exist: " + p_outputDirectory );

      StringList fileNames;
      for ( const auto& item : o_measures )
         if ( item.enabled )
         {
            if ( !File::Exists( item.path ) )
               throw Error( "No such file exists on the local filesystem: " + item.path );
            fileNames << File::ExtractNameAndSuffix( item.path );
            if ( !p_subframes[item.index].nmlPath.IsEmpty() )
            {
               if ( !File::Exists( p_subframes[item.index].nmlPath ) )
                  throw Error( "No such file exists on the local filesystem: " + p_subframes[item.index].nmlPath );
               fileNames << File::ExtractNameAndSuffix( p_subframes[item.index].nmlPath );
            }
            if ( !p_subframes[item.index].drzPath.IsEmpty() )
            {
               if ( !File::Exists( p_subframes[item.index].drzPath ) )
                  throw Error( "No such file exists on the local filesystem: " + p_subframes[item.index].drzPath );
               fileNames << File::ExtractNameAndSuffix( p_subframes[item.index].drzPath );
            }
         }
      fileNames.Sort();
      for ( size_type i = 1; i < fileNames.Length(); ++i )
         if ( fileNames[i].CompareIC( fileNames[i-1] ) == 0 )
         {
            if ( p_overwriteExistingFiles )
               throw Error( "The subframes list contains duplicate file names (case-insensitive). "
                            "This is not allowed when the 'Overwrite existing files' option is enabled." );
            console.WarningLn( "<end><cbr><br>** Warning: The subframes list contains duplicate file names (case-insensitive)." );
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

   int succeeded = 0;
   int failed = 0;
   int skipped = 0;

   Array<size_type> pendingItems;
   for ( size_type i = 0; i < o_measures.Length(); ++i )
      if ( o_measures[i].enabled )
         pendingItems << i;
      else
      {
         console.NoteLn( "* Skipping disabled target: <raw>" + o_measures[i].path + "</raw>" );
         ++skipped;
      }

   /*
    * Begin light frame calibration process.
    */
   console.NoteLn( String().Format( "<end><cbr><br>* Generation of %u output images.", pendingItems.Length() ) );

   Module->ProcessEvents();

   if ( p_useFileThreads && pendingItems.Length() > 1 )
   {
      /*
       * Running threads list. Note that IndirectArray<> initializes all of its
       * contained pointers to nullptr.
       */
      int numberOfThreadsAvailable = RoundInt( Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 ) * p_fileThreadOverload );
      int numberOfThreads = Min( numberOfThreadsAvailable, int( pendingItems.Length() ) );
      output_thread_list runningThreads( numberOfThreads );

      console.NoteLn( String().Format( "<end><br>* Using %d worker threads.", numberOfThreads ) );
      console.NoteLn( String().Format(          "* Using %d file reading threads.", m_maxFileReadThreads ) );
      console.NoteLn( String().Format(          "* Using %d file writing threads.<br/>", m_maxFileWriteThreads ) );
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
               for ( output_thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
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
                     (*i)->FlushConsoleOutputText();
                     String errorInfo;
                     if ( !(*i)->Succeeded() )
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
                     *i = new SubframeSelectorOutputThread( *this, *pendingItems );
                     size_type threadIndex = i - runningThreads.Begin();
                     console.NoteLn( String().Format( "<end><cbr>[%03u] <raw>", threadIndex ) + (*i)->TargetFilePath() + "</raw>" );
                     (*i)->Start( ThreadPriority::DefaultMax, threadIndex );
                     ++running;
                     pendingItems.Remove( pendingItems.Begin() );
                     if ( pendingItems.IsEmpty() )
                        console.NoteLn( "<br>* Waiting for running tasks to terminate..." );
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
         for ( SubframeSelectorOutputThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Abort();
         for ( SubframeSelectorOutputThread* thread : runningThreads )
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
            SubframeSelectorOutputThread thread( *this, itemIndex );
            thread.Run();
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
         throw Error( "No output images were generated: Empty target frames list? No enabled target frames?" );
      throw Error( "No output image could be generated." );
   }

   console.NoteLn( String().Format( "<end><cbr><br>===== SubframeSelector: %u succeeded, %u failed, %u skipped =====",
                                    succeeded, failed, skipped ) );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInstance::ApplyErrorPolicy()
{
   Console console;
   console.ResetStatus();
   console.EnableAbort();

   console.Note( "<end><cbr><br>* Applying error policy: " );

   switch ( p_onError )
   {
   default: // ?
   case SSOnError::Continue:
      console.NoteLn( "Continue on error." );
      break;

   case SSOnError::Abort:
      console.NoteLn( "Abort on error." );
      throw ProcessAborted();

   case SSOnError::AskUser:
      {
         console.NoteLn( "Ask on error..." );

         int r = MessageBox( "<p style=\"white-space:pre;\">"
                             "An error occurred during SubframeSelector execution. What do you want to do?</p>",
                             "SubframeSelector",
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

bool SubframeSelectorInstance::CanExecuteGlobal( String &whyNot ) const
{
   switch ( p_routine )
   {
   case SSRoutine::MeasureSubframes:
   case SSRoutine::StarDetectionPreview:
      if ( p_subframes.IsEmpty() )
      {
         whyNot = "No subframes have been specified.";
         return false;
      }
      break;
   case SSRoutine::OutputSubframes:
      if ( o_measures.IsEmpty() )
      {
         whyNot = "No measurements have been made.";
         return false;
      }
      if ( p_outputKeyword.IsEmpty() )
      {
         whyNot = "The specified output keyword is blank.";
         return false;
      }
      if ( !p_outputDirectory.IsEmpty() )
         if ( !File::DirectoryExists( p_outputDirectory ) )
         {
            whyNot = "The specified output directory does not exist.";
            return false;
         }
      break;
   default:
      whyNot = String().Format( "Internal error: Unknown routine with code '%d'", p_routine );
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::ExecuteGlobal()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanExecuteGlobal( why ) )
         throw Error( why );

      for ( const SubframeItem& subframe : p_subframes )
         if ( subframe.enabled )
            if ( !File::Exists( subframe.path ) )
               throw Error( "No such file exists on the local filesystem: " + subframe.path );
   }

   Console().ResetStatus();

   /*
    * Perform the selected routine.
    */
   switch ( p_routine )
   {
   case SSRoutine::StarDetectionPreview:
      TestStarDetector();
      break;
   case SSRoutine::MeasureSubframes:
      Measure();
      break;
   case SSRoutine::OutputSubframes:
      Output();
      break;
   default:
      throw Error( String().Format( "Internal error: Unknown routine with code '%d'", p_routine ) );
   }

   return true;
}

// ----------------------------------------------------------------------------

void* SubframeSelectorInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSRoutineParameter )
      return &p_routine;
   if ( p == TheSSNonInteractiveParameter )
      return &p_nonInteractive;

   if ( p == TheSSSubframeEnabledParameter )
      return &p_subframes[tableRow].enabled;
   if ( p == TheSSSubframePathParameter )
      return p_subframes[tableRow].path.Begin();
   if ( p == TheSSLocalNormalizationDataPathParameter )
      return p_subframes[tableRow].nmlPath.Begin();
   if ( p == TheSSDrizzlePathParameter )
      return p_subframes[tableRow].drzPath.Begin();

   if ( p == TheSSFileCacheParameter )
      return &p_fileCache;

   if ( p == TheSSSubframeScaleParameter )
      return &p_subframeScale;
   if ( p == TheSSCameraGainParameter )
      return &p_cameraGain;
   if ( p == TheSSCameraResolutionParameter )
      return &p_cameraResolution;
   if ( p == TheSSSiteLocalMidnightParameter )
      return &p_siteLocalMidnight;
   if ( p == TheSSScaleUnitParameter )
      return &p_scaleUnit;
   if ( p == TheSSDataUnitParameter )
      return &p_dataUnit;
   if ( p == TheSSTrimmingFactorParameter )
      return &p_trimmingFactor;

   if ( p == TheSSStructureLayersParameter )
      return &p_structureLayers;
   if ( p == TheSSNoiseLayersParameter )
      return &p_noiseLayers;
   if ( p == TheSSHotPixelFilterRadiusParameter )
      return &p_hotPixelFilterRadius;
   if ( p == TheSSNoiseReductionFilterRadiusParameter )
      return &p_noiseReductionFilterRadius;
   if ( p == TheSSMinStructureSizeParameter )
      return &p_minStructureSize;
   if ( p == TheSSSensitivityParameter )
      return &p_sensitivity;
   if ( p == TheSSPeakResponseParameter )
      return &p_peakResponse;
   if ( p == TheSSBrightThresholdParameter )
      return &p_brightThreshold;
   if ( p == TheSSMaxDistortionParameter )
      return &p_maxDistortion;
   if ( p == TheSSAllowClusteredSourcesParameter )
      return &p_allowClusteredSources;
   if ( p == TheSSUpperLimitParameter )
      return &p_upperLimit;
   if ( p == TheSSPSFFitParameter )
      return &p_psfFit;
   if ( p == TheSSPSFFitCircularParameter )
      return &p_psfFitCircular;
   if ( p == TheSSMaxPSFFitsParameter )
      return &p_maxPSFFits;
   if ( p == TheSSROIX0Parameter )
      return &p_roi.x0;
   if ( p == TheSSROIY0Parameter )
      return &p_roi.y0;
   if ( p == TheSSROIX1Parameter )
      return &p_roi.x1;
   if ( p == TheSSROIY1Parameter )
      return &p_roi.y1;

   if ( p == TheSSPedestalParameter )
      return &p_pedestal;
   if ( p == TheSSPedestalModeParameter )
      return &p_pedestalMode;
   if ( p == TheSSPedestalKeywordParameter )
      return p_pedestalKeyword.Begin();

   if ( p == TheSSInputHintsParameter )
      return p_inputHints.Begin();
   if ( p == TheSSOutputHintsParameter )
      return p_outputHints.Begin();

   if ( p == TheSSOutputDirectoryParameter )
      return p_outputDirectory.Begin();
   if ( p == TheSSOutputExtensionParameter )
      return p_outputExtension.Begin();
   if ( p == TheSSOutputPrefixParameter )
      return p_outputPrefix.Begin();
   if ( p == TheSSOutputPostfixParameter )
      return p_outputPostfix.Begin();
   if ( p == TheSSOutputKeywordParameter )
      return p_outputKeyword.Begin();
   if ( p == TheSSOverwriteExistingFilesParameter )
      return &p_overwriteExistingFiles;
   if ( p == TheSSOnErrorParameter )
      return &p_onError;

   if ( p == TheSSApprovalExpressionParameter )
      return p_approvalExpression.Begin();
   if ( p == TheSSWeightingExpressionParameter )
      return p_weightingExpression.Begin();

   if ( p == TheSSSortingPropertyParameter )
      return &p_sortingProperty;
   if ( p == TheSSGraphPropertyParameter )
      return &p_graphProperty;
   if ( p == TheSSAuxGraphPropertyParameter )
      return &p_auxGraphProperty;

   if ( p == TheSSUseFileThreadsParameter )
      return &p_useFileThreads;
   if ( p == TheSSFileThreadOverloadParameter )
      return &p_fileThreadOverload;
   if ( p == TheSSMaxFileReadThreadsParameter )
      return &p_maxFileReadThreads;
   if ( p == TheSSMaxFileWriteThreadsParameter )
      return &p_maxFileWriteThreads;

   if ( p == TheSSMeasurementIndexParameter )
      return &o_measures[tableRow].index;
   if ( p == TheSSMeasurementEnabledParameter )
      return &o_measures[tableRow].enabled;
   if ( p == TheSSMeasurementLockedParameter )
      return &o_measures[tableRow].locked;
   if ( p == TheSSMeasurementPathParameter )
      return o_measures[tableRow].path.Begin();
   if ( p == TheSSMeasurementWeightParameter )
      return &o_measures[tableRow].weight;
   if ( p == TheSSMeasurementFWHMParameter )
      return &o_measures[tableRow].fwhm;
   if ( p == TheSSMeasurementEccentricityParameter )
      return &o_measures[tableRow].eccentricity;
   if ( p == TheSSMeasurementPSFSignalWeightParameter )
      return &o_measures[tableRow].psfSignalWeight;
   if ( p == TheSSMeasurementPSFSNRParameter )
      return &o_measures[tableRow].psfSNR;
   if ( p == TheSSMeasurementPSFScaleParameter )
      return &o_measures[tableRow].psfScale;
   if ( p == TheSSMeasurementPSFScaleSNRParameter )
      return &o_measures[tableRow].psfScaleSNR;
   if ( p == TheSSMeasurementPSFFluxParameter )
      return &o_measures[tableRow].psfFlux;
   if ( p == TheSSMeasurementPSFFluxPowerParameter )
      return &o_measures[tableRow].psfFluxPower;
   if ( p == TheSSMeasurementPSFTotalMeanFluxParameter )
      return &o_measures[tableRow].psfTotalMeanFlux;
   if ( p == TheSSMeasurementPSFTotalMeanPowerFluxParameter )
      return &o_measures[tableRow].psfTotalMeanPowerFlux;
   if ( p == TheSSMeasurementPSFCountParameter )
      return &o_measures[tableRow].psfCount;
   if ( p == TheSSMeasurementMStarParameter )
      return &o_measures[tableRow].MStar;
   if ( p == TheSSMeasurementNStarParameter )
      return &o_measures[tableRow].NStar;
   if ( p == TheSSMeasurementSNRWeightParameter )
      return &o_measures[tableRow].snrWeight;
   if ( p == TheSSMeasurementMedianParameter )
      return &o_measures[tableRow].median;
   if ( p == TheSSMeasurementMedianMeanDevParameter )
      return &o_measures[tableRow].medianMeanDev;
   if ( p == TheSSMeasurementNoiseParameter )
      return &o_measures[tableRow].noise;
   if ( p == TheSSMeasurementNoiseRatioParameter )
      return &o_measures[tableRow].noiseRatio;
   if ( p == TheSSMeasurementStarsParameter )
      return &o_measures[tableRow].stars;
   if ( p == TheSSMeasurementStarResidualParameter )
      return &o_measures[tableRow].starResidual;
   if ( p == TheSSMeasurementFWHMMeanDevParameter )
      return &o_measures[tableRow].fwhmMeanDev;
   if ( p == TheSSMeasurementEccentricityMeanDevParameter )
      return &o_measures[tableRow].eccentricityMeanDev;
   if ( p == TheSSMeasurementStarResidualMeanDevParameter )
      return &o_measures[tableRow].starResidualMeanDev;
   if ( p == TheSSMeasurementAzimuthParameter )
      return &o_measures[tableRow].azimuth;
   if ( p == TheSSMeasurementAltitudeParameter )
      return &o_measures[tableRow].altitude;
   if ( p == TheSSMeasurementUnused01Parameter )
      return &o_measures[tableRow].unused01;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSSubframesParameter )
   {
      p_subframes.Clear();
      if ( sizeOrLength > 0 )
         p_subframes.Add( SubframeItem(), sizeOrLength );
   }
   else if ( p == TheSSSubframePathParameter )
   {
      p_subframes[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_subframes[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheSSLocalNormalizationDataPathParameter )
   {
      p_subframes[tableRow].nmlPath.Clear();
      if ( sizeOrLength > 0 )
         p_subframes[tableRow].nmlPath.SetLength( sizeOrLength );
   }
   else if ( p == TheSSDrizzlePathParameter )
   {
      p_subframes[tableRow].drzPath.Clear();
      if ( sizeOrLength > 0 )
         p_subframes[tableRow].drzPath.SetLength( sizeOrLength );
   }
   else if ( p == TheSSPedestalKeywordParameter )
   {
      p_pedestalKeyword.Clear();
      if ( sizeOrLength > 0 )
         p_pedestalKeyword.SetLength( sizeOrLength );
   }
   else if ( p == TheSSInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputHintsParameter )
   {
      p_outputHints.Clear();
      if ( sizeOrLength > 0 )
         p_outputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputDirectoryParameter )
   {
      p_outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_outputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputExtensionParameter )
   {
      p_outputExtension.Clear();
      if ( sizeOrLength > 0 )
         p_outputExtension.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputPrefixParameter )
   {
      p_outputPrefix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPrefix.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputPostfixParameter )
   {
      p_outputPostfix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPostfix.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputKeywordParameter )
   {
      p_outputKeyword.Clear();
      if ( sizeOrLength > 0 )
         p_outputKeyword.SetLength( sizeOrLength );
   }
   else if ( p == TheSSApprovalExpressionParameter )
   {
      p_approvalExpression.Clear();
      if ( sizeOrLength > 0 )
         p_approvalExpression.SetLength( sizeOrLength );
   }
   else if ( p == TheSSWeightingExpressionParameter )
   {
      p_weightingExpression.Clear();
      if ( sizeOrLength > 0 )
         p_weightingExpression.SetLength( sizeOrLength );
   }
   else if ( p == TheSSMeasurementsParameter )
   {
      o_measures.Clear();
      if ( sizeOrLength > 0 )
         for ( size_type i = 0; i < sizeOrLength; ++i )
            o_measures.Add( MeasureItem( i ) );
   }
   else if ( p == TheSSMeasurementPathParameter )
   {
      o_measures[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         o_measures[tableRow].path.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type SubframeSelectorInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheSSSubframesParameter )
      return p_subframes.Length();
   if ( p == TheSSSubframePathParameter )
      return p_subframes[tableRow].path.Length();
   if ( p == TheSSLocalNormalizationDataPathParameter )
      return p_subframes[tableRow].nmlPath.Length();
   if ( p == TheSSDrizzlePathParameter )
      return p_subframes[tableRow].drzPath.Length();

   if ( p == TheSSPedestalKeywordParameter )
      return p_pedestalKeyword.Length();

   if ( p == TheSSInputHintsParameter )
      return p_inputHints.Length();
   if ( p == TheSSOutputHintsParameter )
      return p_outputHints.Length();

   if ( p == TheSSOutputDirectoryParameter )
      return p_outputDirectory.Length();
   if ( p == TheSSOutputExtensionParameter )
      return p_outputExtension.Length();
   if ( p == TheSSOutputPrefixParameter )
      return p_outputPrefix.Length();
   if ( p == TheSSOutputPostfixParameter )
      return p_outputPostfix.Length();
   if ( p == TheSSOutputKeywordParameter )
      return p_outputKeyword.Length();

   if ( p == TheSSApprovalExpressionParameter )
      return p_approvalExpression.Length();
   if ( p == TheSSWeightingExpressionParameter )
      return p_weightingExpression.Length();

   if ( p == TheSSMeasurementsParameter )
      return o_measures.Length();
   if ( p == TheSSMeasurementPathParameter )
      return o_measures[tableRow].path.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInstance.cpp - Released 2023-11-23T18:45:25Z
