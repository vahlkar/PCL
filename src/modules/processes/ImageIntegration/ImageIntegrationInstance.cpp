//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.5.3
// ----------------------------------------------------------------------------
// ImageIntegrationInstance.cpp - Released 2023-12-01T19:16:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#include "ImageIntegrationInstance.h"
#include "IntegrationCache.h"
#include "IntegrationDataLoaderEngine.h"
#include "IntegrationEngine.h"
#include "IntegrationEngineBase.h"
#include "IntegrationFile.h"
#include "IntegrationRejectionEngine.h"
#include "LargeScaleRejectionMapGenerationEngine.h"
#include "MapIntegrationEngine.h"
#include "RejectionMapGenerationEngine.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/DrizzleData.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ImageWindow.h>
#include <pcl/MetaModule.h>
#include <pcl/SpinStatus.h>
#include <pcl/Version.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ImageIntegrationInstance::ImageIntegrationInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_inputHints( TheIIInputHintsParameter->DefaultValue() )
   , p_combination( IICombination::Default )
   , p_normalization( IINormalization::Default )
   , p_weightMode( IIWeightMode::Default )
   , p_weightKeyword( TheIIWeightKeywordParameter->DefaultValue() )
   , p_weightScale( IIWeightScale::Default )
   , p_minWeight( TheIIMinWeightParameter->DefaultValue() )
   , p_adaptiveGridSize( TheIIAdaptiveGridSizeParameter->DefaultValue() )
   , p_adaptiveNoScale( TheIIAdaptiveNoScaleParameter->DefaultValue() )
   , p_ignoreNoiseKeywords( TheIIIgnoreNoiseKeywordsParameter->DefaultValue() )
   , p_rejection( IIRejection::Default )
   , p_rejectionNormalization( IIRejectionNormalization::Default )
   , p_minMaxLow( TheIIMinMaxLowParameter->DefaultValue() )
   , p_minMaxHigh( TheIIMinMaxHighParameter->DefaultValue() )
   , p_pcClipLow( TheIIPercentileLowParameter->DefaultValue() )
   , p_pcClipHigh( TheIIPercentileHighParameter->DefaultValue() )
   , p_sigmaLow( TheIISigmaLowParameter->DefaultValue() )
   , p_sigmaHigh( TheIISigmaHighParameter->DefaultValue() )
   , p_winsorizationCutoff( TheIIWinsorizationCutoffParameter->DefaultValue() )
   , p_linearFitLow( TheIILinearFitLowParameter->DefaultValue() )
   , p_linearFitHigh( TheIILinearFitHighParameter->DefaultValue() )
   , p_esdOutliersFraction( TheIIESDOutliersFractionParameter->DefaultValue() )
   , p_esdAlpha( TheIIESDAlphaParameter->DefaultValue() )
   , p_esdLowRelaxation( TheIIESDLowRelaxationParameter->DefaultValue() )
   , p_rcrLimit( TheIIRCRLimitParameter->DefaultValue() )
   , p_ccdGain( TheIICCDGainParameter->DefaultValue() )
   , p_ccdReadNoise( TheIICCDReadNoiseParameter->DefaultValue() )
   , p_ccdScaleNoise( TheIICCDScaleNoiseParameter->DefaultValue() )
   , p_clipLow( TheIIClipLowParameter->DefaultValue() )
   , p_clipHigh( TheIIClipHighParameter->DefaultValue() )
   , p_rangeClipLow( TheIIRangeClipLowParameter->DefaultValue() )
   , p_rangeLow( TheIIRangeLowParameter->DefaultValue() )
   , p_rangeClipHigh( TheIIRangeClipHighParameter->DefaultValue() )
   , p_rangeHigh( TheIIRangeHighParameter->DefaultValue() )
   , p_reportRangeRejection( TheIIReportRangeRejectionParameter->DefaultValue() )
   , p_mapRangeRejection( TheIIMapRangeRejectionParameter->DefaultValue() )
   , p_largeScaleClipLow( TheIILargeScaleClipLowParameter->DefaultValue() )
   , p_largeScaleClipLowProtectedLayers( TheIILargeScaleClipLowProtectedLayersParameter->DefaultValue() )
   , p_largeScaleClipLowGrowth( TheIILargeScaleClipLowGrowthParameter->DefaultValue() )
   , p_largeScaleClipHigh( TheIILargeScaleClipHighParameter->DefaultValue() )
   , p_largeScaleClipHighProtectedLayers( TheIILargeScaleClipHighProtectedLayersParameter->DefaultValue() )
   , p_largeScaleClipHighGrowth( TheIILargeScaleClipHighGrowthParameter->DefaultValue() )
   , p_generate64BitResult( TheIIGenerate64BitResultParameter->DefaultValue() )
   , p_generateRejectionMaps( TheIIGenerateRejectionMapsParameter->DefaultValue() )
   , p_generateIntegratedImage( TheIIGenerateIntegratedImageParameter->DefaultValue() )
   , p_generateDrizzleData( TheIIGenerateDrizzleDataParameter->DefaultValue() )
   , p_closePreviousImages( TheIIClosePreviousImagesParameter->DefaultValue() )
   , p_bufferSizeMB( TheIIBufferSizeParameter->DefaultValue() )
   , p_stackSizeMB( TheIIStackSizeParameter->DefaultValue() )
   , p_autoMemorySize( TheIIAutoMemorySizeParameter->DefaultValue() )
   , p_autoMemoryLimit( TheIIAutoMemoryLimitParameter->DefaultValue() )
   , p_useROI( TheIIUseROIParameter->DefaultValue() )
   , p_useCache( TheIIUseCacheParameter->DefaultValue() )
   , p_evaluateSNR( TheIIEvaluateSNRParameter->DefaultValue() )
   , p_noiseEvaluationAlgorithm( IINoiseEvaluationAlgorithm::Default )
   , p_mrsMinDataFraction( TheIIMRSMinDataFractionParameter->DefaultValue() )
   , p_psfStructureLayers( TheIIPSFStructureLayersParameter->DefaultValue() )
   , p_psfType( IIPSFType::Default )
   , p_subtractPedestals( TheIISubtractPedestalsParameter->DefaultValue() )
   , p_truncateOnOutOfRange( TheIITruncateOnOutOfRangeParameter->DefaultValue() )
   , p_noGUIMessages( TheIINoGUIMessagesParameter->DefaultValue() ) // ### DEPRECATED
   , p_showImages( TheIIShowImagesParameter->DefaultValue() )
   , p_useFileThreads( TheIIUseFileThreadsParameter->DefaultValue() )
   , p_fileThreadOverload( TheIIFileThreadOverloadParameter->DefaultValue() )
   , p_useBufferThreads( TheIIUseBufferThreadsParameter->DefaultValue() )
   , p_maxBufferThreads( TheIIMaxBufferThreadsParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

ImageIntegrationInstance::ImageIntegrationInstance( const ImageIntegrationInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ImageIntegrationInstance::Assign( const ProcessImplementation& p )
{
   const ImageIntegrationInstance* x = dynamic_cast<const ImageIntegrationInstance*>( &p );
   if ( x != nullptr )
   {
      p_images                            = x->p_images;
      p_inputHints                        = x->p_inputHints;
      p_combination                       = x->p_combination;
      p_normalization                     = x->p_normalization;
      p_weightMode                        = x->p_weightMode;
      p_weightKeyword                     = x->p_weightKeyword;
      p_weightScale                       = x->p_weightScale;
      p_minWeight                         = x->p_minWeight;
      p_adaptiveGridSize                  = x->p_adaptiveGridSize;
      p_adaptiveNoScale                   = x->p_adaptiveNoScale;
      p_ignoreNoiseKeywords               = x->p_ignoreNoiseKeywords;
      p_rejection                         = x->p_rejection;
      p_rejectionNormalization            = x->p_rejectionNormalization;
      p_minMaxLow                         = x->p_minMaxLow;
      p_minMaxHigh                        = x->p_minMaxHigh;
      p_pcClipLow                         = x->p_pcClipLow;
      p_pcClipHigh                        = x->p_pcClipHigh;
      p_sigmaLow                          = x->p_sigmaLow;
      p_sigmaHigh                         = x->p_sigmaHigh;
      p_winsorizationCutoff               = x->p_winsorizationCutoff;
      p_linearFitLow                      = x->p_linearFitLow;
      p_linearFitHigh                     = x->p_linearFitHigh;
      p_esdOutliersFraction               = x->p_esdOutliersFraction;
      p_esdAlpha                          = x->p_esdAlpha;
      p_esdLowRelaxation                  = x->p_esdLowRelaxation;
      p_rcrLimit                          = x->p_rcrLimit;
      p_ccdGain                           = x->p_ccdGain;
      p_ccdReadNoise                      = x->p_ccdReadNoise;
      p_ccdScaleNoise                     = x->p_ccdScaleNoise;
      p_clipLow                           = x->p_clipLow;
      p_clipHigh                          = x->p_clipHigh;
      p_rangeClipLow                      = x->p_rangeClipLow;
      p_rangeLow                          = x->p_rangeLow;
      p_rangeClipHigh                     = x->p_rangeClipHigh;
      p_rangeHigh                         = x->p_rangeHigh;
      p_reportRangeRejection              = x->p_reportRangeRejection;
      p_mapRangeRejection                 = x->p_mapRangeRejection;
      p_largeScaleClipLow                 = x->p_largeScaleClipLow;
      p_largeScaleClipLowProtectedLayers  = x->p_largeScaleClipLowProtectedLayers;
      p_largeScaleClipLowGrowth           = x->p_largeScaleClipLowGrowth;
      p_largeScaleClipHigh                = x->p_largeScaleClipHigh;
      p_largeScaleClipHighProtectedLayers = x->p_largeScaleClipHighProtectedLayers;
      p_largeScaleClipHighGrowth          = x->p_largeScaleClipHighGrowth;
      p_generate64BitResult               = x->p_generate64BitResult;
      p_generateRejectionMaps             = x->p_generateRejectionMaps;
      p_generateIntegratedImage           = x->p_generateIntegratedImage;
      p_generateDrizzleData               = x->p_generateDrizzleData;
      p_closePreviousImages               = x->p_closePreviousImages;
      p_bufferSizeMB                      = x->p_bufferSizeMB;
      p_stackSizeMB                       = x->p_stackSizeMB;
      p_autoMemorySize                    = x->p_autoMemorySize;
      p_autoMemoryLimit                   = x->p_autoMemoryLimit;
      p_useROI                            = x->p_useROI;
      p_roi                               = x->p_roi;
      p_useCache                          = x->p_useCache;
      p_evaluateSNR                       = x->p_evaluateSNR;
      p_noiseEvaluationAlgorithm          = x->p_noiseEvaluationAlgorithm;
      p_mrsMinDataFraction                = x->p_mrsMinDataFraction;
      p_psfStructureLayers                = x->p_psfStructureLayers;
      p_psfType                           = x->p_psfType;
      p_subtractPedestals                 = x->p_subtractPedestals;
      p_truncateOnOutOfRange              = x->p_truncateOnOutOfRange;
      p_noGUIMessages                     = x->p_noGUIMessages;
      p_showImages                        = x->p_showImages;
      p_useFileThreads                    = x->p_useFileThreads;
      p_fileThreadOverload                = x->p_fileThreadOverload;
      p_useBufferThreads                  = x->p_useBufferThreads;
      p_maxBufferThreads                  = x->p_maxBufferThreads;
      o_output                            = x->o_output;

      // Be compatible with old versions defining deprecated scale estimators.
      switch ( p_weightScale )
      {
      case IIWeightScale::AvgDev:
      case IIWeightScale::MAD:
      case IIWeightScale::BWMV:
         break;
      default:
         p_weightScale = IIWeightScale::BWMV;
         break;
      }
   }
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "ImageIntegration can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_images.Length() < 3 )
   {
      whyNot = "This instance of ImageIntegration defines less than three source images.";
      return false;
   }

   if ( !p_csvWeights.IsEmpty() )
      if ( !Module->HasEntitlement( "com.pixinsight.security.pcl.experimental-features" ) )
      {
         whyNot = "The caller process lacks entitlement to perform operation.";
         return false;
      }

   return true;
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInstance::ExecuteGlobal()
{
   o_output = OutputData();

   m_maxBufferThreads = p_maxBufferThreads;
   if ( m_maxBufferThreads < 1 )
      m_maxBufferThreads = Max( 1, PixInsightSettings::GlobalInteger( "Process/MaxFileReadThreads" ) );

   ImageWindow resultWindow, lowRejectionMapWindow, highRejectionMapWindow, slopeMapWindow;

   try
   {
      {
         String why;
         if ( !CanExecuteGlobal( why ) )
            throw Error( why );
      }

      if ( p_useROI )
      {
         p_roi.Order();
         if ( !p_roi.IsRect() )
            throw Error( "Empty ROI defined" );
      }

      Console console;
      console.EnableAbort();

      if ( TheIntegrationCache == nullptr )
      {
         new IntegrationCache;

         try
         {
            if ( TheIntegrationCache->IsPersistent() )
            {
               TheIntegrationCache->Load();

               if ( TheIntegrationCache->IsEmpty() )
                  console.NoteLn( "<end><cbr><br>* Empty file cache" );
               else
                  console.NoteLn( "<end><cbr><br>* Loaded cache: " + String( TheIntegrationCache->NumberOfItems() ) + " item(s)" );
            }
         }
         catch ( ... )
         {
            TheIntegrationCache->Purge();
            try
            {
               throw;
            }
            ERROR_HANDLER;
         }
      }

      IntegrationFile::OpenFiles( *this );

      console.WriteLn( String().Format( "<end><cbr><br>Integration of %d images:", IntegrationFile::NumberOfFiles() ) );
      console.WriteLn( IntegrationDescription() );
      console.Flush();

      ImageVariant result;
      if ( p_generateIntegratedImage )
      {
         resultWindow = CreateImageWindow( "integration", p_generate64BitResult ? 64 : 32 );
         result = resultWindow.MainView().Image();
      }

      ImageVariant lowRejectionMap;
      bool generateLowRejectionMap = p_generateRejectionMaps && (p_rejection != IIRejection::NoRejection && p_clipLow || p_rangeClipLow);
      if ( generateLowRejectionMap )
      {
         lowRejectionMapWindow = CreateImageWindow( "rejection_low", 32 );
         lowRejectionMap = lowRejectionMapWindow.MainView().Image();
      }

      ImageVariant highRejectionMap;
      bool generateHighRejectionMap = p_generateRejectionMaps && (p_rejection != IIRejection::NoRejection && p_clipHigh || p_rangeClipHigh);
      if ( generateHighRejectionMap )
      {
         highRejectionMapWindow = CreateImageWindow( "rejection_high", 32 );
         highRejectionMap = highRejectionMapWindow.MainView().Image();
      }

      ImageVariant slopeMap;
      bool generateSlopeMap = p_generateRejectionMaps && p_rejection == IIRejection::LinearFit;
      if ( generateSlopeMap )
      {
         slopeMapWindow = CreateImageWindow( "slope", 32 );
         slopeMap = slopeMapWindow.MainView().Image();
      }

      int totalStacks;
      {
         size_type totalAvailable = Module->AvailablePhysicalMemory();

         size_type stackSize = size_type( IntegrationFile::Width() ) *
                                 (IntegrationFile::NumberOfFiles()*sizeof( RejectionDataItem )
                                  + sizeof( int )
                                  + ((p_rejection == IIRejection::LinearFit) ? sizeof( float ) : 0));

         size_type stackBufferSize = 0;
         if ( p_autoMemorySize )
         {
            size_type maxAvailable = size_type( double( p_autoMemoryLimit ) * totalAvailable );
            if ( maxAvailable > IntegrationFile::TotalBufferSize() )
               stackBufferSize = maxAvailable - IntegrationFile::TotalBufferSize();
         }
         else
            stackBufferSize = size_type( p_stackSizeMB )*1024*1024;

         totalStacks = int( Min( Max( size_type( Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 ) ),
                                      stackBufferSize/stackSize ),
                                 size_type( IntegrationFile::BufferRows() ) ) );

         if ( totalAvailable == 0 )
            console.WarningLn( "<end><cbr><br>** Warning: Unable to estimate the available physical memory." );
         else
            console.NoteLn( String().Format( "<end><cbr><br>* Available physical memory: %.3f GiB", totalAvailable/1024/1024/1024.0 ) );

         console.NoteLn( String().Format( "* Allocated pixel buffer: %d rows, %.3f GiB",
                                          IntegrationFile::BufferRows(),
                                          IntegrationFile::TotalBufferSize()/1024/1024/1024.0 ) );
         console.NoteLn( String().Format( "* Using %d concurrent pixel stack(s), %.3f GiB<br>",
                                          totalStacks, totalStacks*stackSize/1024/1024/1024.0 ) );
      }

      o_output.imageData.Add( OutputData::ImageData(), IntegrationFile::NumberOfFiles() );

      // For each channel
      for ( int c = 0; c < IntegrationFile::NumberOfChannels(); ++c )
      {
         if ( IntegrationFile::NumberOfChannels() > 1 )
            console.WriteLn( String().Format( "<br>* Integrating channel %d of %d:", c+1, IntegrationFile::NumberOfChannels() ) );

         // Keep the GUI responsive
         Module->ProcessEvents();
         if ( console.AbortRequested() )
            throw ProcessAborted();

         /*
          * Output data pointers
          */
         float* resultData32 = nullptr;
         double* resultData64 = nullptr;
         if ( p_generateIntegratedImage )
            if ( p_generate64BitResult )
               resultData64 = static_cast<DImage&>( *result )[c];
            else
               resultData32 = static_cast<Image&>( *result )[c];

         /*
          * Rejection map data pointers
          */
         float* rejectionLowData = nullptr;
         if ( generateLowRejectionMap )
            rejectionLowData = static_cast<Image&>( *lowRejectionMap )[c];
         float* rejectionHighData = nullptr;
         if ( generateHighRejectionMap )
            rejectionHighData = static_cast<Image&>( *highRejectionMap )[c];
         float* slopeData = nullptr;
         if ( generateSlopeMap )
            slopeData = static_cast<Image&>( *slopeMap )[c];

         /*
          * Normalization and scaling vectors
          */
         DVector d, m, q;
         scale_estimates s;

         if ( p_generateIntegratedImage &&
                  (p_normalization == IINormalization::Additive ||
                   p_normalization == IINormalization::Multiplicative) )
         {
            d = DVector( IntegrationFile::NumberOfFiles() );
            double m0 = IntegrationFile::FileByIndex( 0 ).LocationEstimate( c );
            for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
               switch ( p_normalization )
               {
               default:
               case IINormalization::Additive:
                  d[i] = m0 - IntegrationFile::FileByIndex( i ).LocationEstimate( c );
                  break;
               case IINormalization::Multiplicative:
                  d[i] = m0 / IntegrationFile::FileByIndex( i ).LocationEstimate( c );
                  break;
               }
         }

         if ( p_generateDrizzleData ||
              p_generateIntegratedImage &&
                  (p_normalization == IINormalization::AdditiveWithScaling ||
                   p_normalization == IINormalization::MultiplicativeWithScaling ||
                   p_normalization == IINormalization::AdaptiveNormalization && p_adaptiveNoScale) ||
              p_rejection != IIRejection::NoRejection &&
                  (p_rejectionNormalization == IIRejectionNormalization::Scale ||
                   p_rejectionNormalization == IIRejectionNormalization::AdaptiveRejectionNormalization && p_adaptiveNoScale) )
         {
            m = DVector( IntegrationFile::NumberOfFiles() );
            s = scale_estimates( IntegrationFile::NumberOfFiles() );
            for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
            {
               m[i] = IntegrationFile::FileByIndex( i ).LocationEstimate( c );
               s[i] = IntegrationFile::FileByIndex( i ).ScaleFactor( c );
            }
         }

         if ( p_rejection != IIRejection::NoRejection &&
              p_rejectionNormalization == IIRejectionNormalization::EqualizeFluxes )
         {
            q = DVector( IntegrationFile::NumberOfFiles() );
            double m0 = IntegrationFile::FileByIndex( 0 ).LocationEstimate( c );
            for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
               q[i] = m0 / IntegrationFile::FileByIndex( i ).LocationEstimate( c );
         }

         /*
          * Rejected pixel counters
          */
         GenericVector<size_type> NRL( size_type( 0 ), IntegrationFile::NumberOfFiles() );
         GenericVector<size_type> NRH( size_type( 0 ), IntegrationFile::NumberOfFiles() );
         GenericVector<size_type> NRLR( size_type( 0 ), IntegrationFile::NumberOfFiles() );
         GenericVector<size_type> NRHR( size_type( 0 ), IntegrationFile::NumberOfFiles() );

         bool doReject = p_rejection != IIRejection::NoRejection && (p_clipLow || p_clipHigh) || p_rangeClipLow || p_rangeClipHigh;
         bool doLargeScaleRejectLow = p_rejection != IIRejection::NoRejection && p_clipLow && p_largeScaleClipLow;
         bool doLargeScaleRejectHigh = p_rejection != IIRejection::NoRejection && p_clipHigh && p_largeScaleClipHigh;
         bool doLargeScaleReject = doLargeScaleRejectLow || doLargeScaleRejectHigh;
         bool doIntegrateAndReject = p_generateIntegratedImage && !doLargeScaleReject;

         /*
          * Integrate pixels
          */
         for ( int y0 = 0; y0 < IntegrationFile::Height(); y0 += IntegrationFile::BufferRows() )
         {
            // Number of rows in this strip
            int numberOfRows = Min( IntegrationFile::BufferRows(), IntegrationFile::Height() - y0 );

            StandardStatus status;
            StatusMonitor monitor;
            monitor.SetCallback( &status );
            monitor.Initialize( String( doIntegrateAndReject ? "Integrating" : "Analyzing" ).AppendFormat(
                                                 " pixel rows: %5d -> %5d", y0, y0+numberOfRows-1 ), 5*numberOfRows );

            IntegrationFile::UpdateBuffers( y0, c, p_useBufferThreads ? m_maxBufferThreads : 1 );

            /*
             * Integrate pixels in the current strip of pixel rows.
             */
            for ( int r = 0, numberOfStacks; r < numberOfRows; r += numberOfStacks )
            {
               // Number of pixel stacks for rejection/integration in this strip
               numberOfStacks = Min( numberOfRows-r, totalStacks );

               // Keep the GUI responsive
               Module->ProcessEvents();
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               RejectionStacks stacks( numberOfStacks );
               RejectionCounts counts( numberOfStacks );
               RejectionSlopes slopes;
               if ( p_rejection == IIRejection::LinearFit )
                  slopes = RejectionSlopes( numberOfStacks );

               IntegrationDataLoaderEngine( *this, monitor, r, stacks, counts, slopes ).LoadData();

               /*
                * Perform pixel rejection.
                */
               if ( doReject )
               {
                  IntegrationRejectionEngine rejector( *this, monitor, stacks, counts, slopes, m, s, q, y0+r, c );

                  /*
                   * Range rejection must be done in first place to exclude all
                   * out-of-range pixels.
                   *
                   * ### N.B.: The order of pixels in each stack must NOT be
                   * changed because rejection normalization depends on file
                   * order.
                   */
                  if ( p_rangeClipLow || p_rangeClipHigh )
                     rejector.RejectRange();
                  else
                     monitor += numberOfStacks;

                  /*
                   * Rejection normalization.
                   */
                  if ( p_rejection != IIRejection::NoRejection )
                     rejector.Normalize();
                  else
                     monitor += numberOfStacks;

                  /*
                   * In case we have performed range rejection, each stack with
                   * out-of-range pixels must be sorted to send rejected pixels
                   * to the bottom of the stack.
                   */
                  if ( p_rangeClipLow || p_rangeClipHigh )
                     for ( int k = 0; k < numberOfStacks; ++k )
                     {
                        RejectionMatrix& R = stacks[k];
                        IVector& N = counts[k];
                        for ( int x = 0; x < IntegrationFile::Width(); ++x )
                           if ( N[x] < IntegrationFile::NumberOfFiles() )
                              Sort( R[x], R[x] + IntegrationFile::NumberOfFiles() );
                     }

                  /*
                   * Reject pixels.
                   */
                  if ( p_rejection != IIRejection::NoRejection )
                     rejector.Reject();
                  else
                     monitor += numberOfStacks;
               }
               else
                  monitor += 3*numberOfStacks;

               /*
                * Integrate pixel stacks.
                */
               if ( doIntegrateAndReject )
               {
                  IntegrationEngine( *this, monitor, stacks, counts,
                                     d, m, s, y0+r, c, resultData32, resultData64 ).Integrate();
                  size_type delta = size_type( numberOfStacks )*size_type( IntegrationFile::Width() );
                  if ( resultData32 != nullptr )
                     resultData32 += delta;
                  else
                     resultData64 += delta;
               }
               else
                  monitor += numberOfStacks;

               /*
                * Update rejection counts and maps.
                */
               if ( doReject )
                  for ( int k = 0, y = y0+r; k < numberOfStacks; ++k, ++y )
                  {
                     const RejectionMatrix& R = stacks[k];

                     for ( int x = 0; x < IntegrationFile::Width(); ++x )
                     {
                        if ( p_clipLow || p_rangeClipLow )
                        {
                           int n = 0, nr = 0;
                           const RejectionDataItem* r = R[x];
                           for ( int i = 0; i < R.Cols(); ++i, ++r )
                           {
                              if ( r->rejectLow )
                              {
                                 ++n;
                                 ++NRL[r->index];
                              }
                              if ( r->rejectRangeLow )
                              {
                                 ++nr;
                                 ++NRLR[r->index];
                              }

                              if ( doLargeScaleReject || p_generateDrizzleData && !p_useROI )
                              {
                                 uint8 mask = r->rejectLow ? 0x02 : 0x00;
                                 if ( r->rejectRangeLow )
                                    mask |= 0x08;
                                 if ( mask != 0 )
                                    IntegrationFile::FileByIndex( r->index ).UpdateRejectionMap( x, y, c, mask );
                              }
                           }

                           if ( !doLargeScaleRejectLow )
                              if ( generateLowRejectionMap )
                              {
                                 if ( p_mapRangeRejection )
                                    n += nr;
                                 *rejectionLowData++ = float( n )/R.Cols();
                              }
                        }

                        if ( p_clipHigh || p_rangeClipHigh )
                        {
                           int n = 0, nr = 0;
                           const RejectionDataItem* r = R[x];
                           for ( int i = 0; i < R.Cols(); ++i, ++r )
                           {
                              if ( r->rejectHigh )
                              {
                                 ++n;
                                 ++NRH[r->index];
                              }
                              if ( r->rejectRangeHigh )
                              {
                                 ++nr;
                                 ++NRHR[r->index];
                              }

                              if ( doLargeScaleReject || p_generateDrizzleData && !p_useROI )
                              {
                                 uint8 mask = r->rejectHigh ? 0x01 : 0x00;
                                 if ( r->rejectRangeHigh )
                                    mask |= 0x04;
                                 if ( mask != 0 )
                                    IntegrationFile::FileByIndex( r->index ).UpdateRejectionMap( x, y, c, mask );
                              }
                           }

                           if ( !doLargeScaleRejectHigh )
                              if ( generateHighRejectionMap )
                              {
                                 if ( p_mapRangeRejection )
                                    n += nr;
                                 *rejectionHighData++ = float( n )/R.Cols();
                              }
                        }

                        if ( generateSlopeMap )
                           *slopeData++ = slopes[k][x];
                     } // for each column
                  } // for each stack
                 // if doReject
            } // for each row strip

            monitor.Complete();
         } // for each row

         /*
          * Large-scale pixel rejection.
          */
         if ( doLargeScaleReject )
         {
            StandardStatus status;
            StatusMonitor monitor;
            monitor.SetCallback( &status );

            if ( doLargeScaleRejectLow )
            {
               monitor.Initialize( "Generating large-scale low rejection maps", 4*IntegrationFile::NumberOfFiles() );
               LargeScaleRejectionMapGenerationEngine( *this, monitor, NRL, false/*high*/, c ).Generate();
            }

            if ( doLargeScaleRejectHigh )
            {
               monitor.Initialize( "Generating large-scale high rejection maps", 4*IntegrationFile::NumberOfFiles() );
               LargeScaleRejectionMapGenerationEngine( *this, monitor, NRH, true/*high*/, c ).Generate();
            }

            /*
             * Integrate with map-based rejection.
             */
            if ( p_generateIntegratedImage )
               for ( int y0 = 0, numberOfRows; y0 < IntegrationFile::Height(); y0 += numberOfRows )
               {
                  Module->ProcessEvents();
                  if ( console.AbortRequested() )
                     throw ProcessAborted();

                  // Number of rows in this strip
                  numberOfRows = Min( IntegrationFile::BufferRows(), IntegrationFile::Height() - y0 );

                  IntegrationFile::UpdateBuffers( y0, c, p_useBufferThreads ? m_maxBufferThreads : 1 );

                  monitor.Initialize( String().Format( "Integrating pixel rows: %5d -> %5d", y0, y0+numberOfRows-1 ), numberOfRows );
                  MapIntegrationEngine( *this, monitor, d, m, s, c, y0, numberOfRows, resultData32, resultData64 ).Integrate();

                  size_type delta = size_type( numberOfRows )*size_type( IntegrationFile::Width() );
                  if ( resultData32 != nullptr )
                     resultData32 += delta;
                  else
                     resultData64 += delta;
               }

            if ( doLargeScaleRejectLow )
               if ( generateLowRejectionMap )
               {
                  monitor.Initialize( "Generating low pixel rejection map", IntegrationFile::Height() );
                  RejectionMapGenerationEngine( *this, monitor, static_cast<Image&>( *lowRejectionMap ), false/*high*/, c ).Generate();
               }

            if ( doLargeScaleRejectHigh )
               if ( generateHighRejectionMap )
               {
                  monitor.Initialize( "Generating high pixel rejection map", IntegrationFile::Height() );
                  RejectionMapGenerationEngine( *this, monitor, static_cast<Image&>( *highRejectionMap ), true/*high*/, c ).Generate();
               }
         } // if doLargeScaleReject

         if ( doReject )
         {
            bool showReject = p_rejection != IIRejection::NoRejection || (p_rangeClipLow || p_rangeClipHigh) && p_reportRangeRejection;

            /*
             * Show rejection stats and update output rejection data.
             */
            Module->ProcessEvents();

            if ( showReject )
               console.WriteLn( "<end><cbr><br>Pixel rejection counts:" );

            size_type N = IntegrationFile::NumberOfPixels();
            size_type NL = 0, NH = 0, NLR = 0, NHR = 0;

            for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
            {
               size_type nl = NRL[i];
               size_type nlr = NRLR[i];
               NL += nl;
               NLR += nlr;
               size_type nh = NRH[i];
               size_type nhr = NRHR[i];
               NH += nh;
               NHR += nhr;

               if ( showReject )
               {
                  console.WriteLn( IntegrationFile::FileByIndex( i ).Path() );
                  if ( p_reportRangeRejection )
                     console.WriteLn( String().Format( "%5d : %9u %7.3f%% (%9u + %9u = %7.3f%% + %7.3f%%)",
                                                       i+1, nl+nh+nlr+nhr, 100.0*(nl+nh+nlr+nhr)/N,
                                                       nl+nlr, nh+nhr, 100.0*(nl+nlr)/N, 100.0*(nh+nhr)/N ) );
                  else
                     console.WriteLn( String().Format( "%5d : %9u %7.3f%% (%9u + %9u = %7.3f%% + %7.3f%%)",
                                                       i+1, nl+nh, 100.0*(nl+nh)/N,
                                                       nl, nh, 100.0*nl/N, 100.0*nh/N ) );
               }

               if ( c < 3 )
               {
                  o_output.imageData[i].rejectedLow[c] = nl + nlr;
                  o_output.imageData[i].rejectedHigh[c] = nh + nhr;
               }
            }

            if ( showReject )
            {
               N *= IntegrationFile::NumberOfFiles();
               if ( p_reportRangeRejection )
                  console.WriteLn( String().Format( "<end><cbr><br><b>Total</b> : %9u %7.3f%% (%9u + %9u = %7.3f%% + %7.3f%%)",
                                                   NL+NH+NLR+NHR, 100.0*(NL+NH+NLR+NHR)/N,
                                                   NL+NLR, NH+NHR, 100.0*(NL+NLR)/N, 100.0*(NH+NHR)/N ) );
               else
                  console.WriteLn( String().Format( "<end><cbr><br><b>Total</b> : %9u %7.3f%% (%9u + %9u = %7.3f%% + %7.3f%%)",
                                                   NL+NH, 100.0*(NL+NH)/N,
                                                   NL, NH, 100.0*NL/N, 100.0*NH/N ) );
            }

            if ( c < 3 )
            {
               o_output.totalRejectedLow[c] = NL + NLR;
               o_output.totalRejectedHigh[c] = NH + NHR;
            }

            Module->ProcessEvents();
         } // if doReject
      } // for each channel

      o_output.numberOfChannels = Min( IntegrationFile::NumberOfChannels(), 3 );
      o_output.numberOfPixels = IntegrationFile::NumberOfPixels();
      o_output.totalPixels = o_output.numberOfPixels * IntegrationFile::NumberOfFiles();

      if ( p_generateIntegratedImage )
      {
         /*
          * Normalize/rescale or truncate the result if out-of-bounds.
          */
         result.GetExtremeSampleValues( o_output.outputRangeLow, o_output.outputRangeHigh );
         if ( o_output.outputRangeLow < 0 || o_output.outputRangeHigh > 1 ) // low < 0 check for completeness/future; should be impossible.
         {
            console.NoteLn( "<end><cbr><br>* "
               + String( p_truncateOnOutOfRange ? "Truncating" : ((o_output.outputRangeLow < 0) ? "Rescaling" : "Normalizing") )
               + " output image. Integration range: "
               + String().Format( "[%.8e,%.8e]", o_output.outputRangeLow, o_output.outputRangeHigh ) );

            if ( p_truncateOnOutOfRange )
            {
               /*
                * Truncate to [0,1]
                *
                * x' = max( 0, min( x, 1 ) )
                */
               result.Truncate();
            }
            else if ( o_output.outputRangeLow < 0 )
            {
               /*
                * Rescale to [0,1], including an additive term:
                *
                * x' = (x - low)/(high - low)
                */
               result.Rescale();
            }
            else
            {
               /*
                * Normalize to [0,1], purely multiplicative:
                *
                * x' = x/high
                */
               result /= o_output.outputRangeHigh;
            }
         }

         if ( p_evaluateSNR )
         {
            /*
             * Show noise and signal evaluation info.
             */
            console.WriteLn( "<end><cbr>" );

            SpinStatus spin;
            result->SetStatusCallback( &spin );
            result->Status().Initialize( "Computing scale and location parameters" );
            result->Status().DisableInitialization();

            DVector location( result->NumberOfNominalChannels() );
            scale_estimates scale( result->NumberOfNominalChannels() );

            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               result->SelectChannel( c );
               location[c] = result.Median();
               TwoSidedEstimate mad = result.TwoSidedMAD( location[c] );
               if ( 1 + mad.low == 1 || 1 + mad.high == 1 )
                  mad.low = mad.high = result.MAD( location[c] );
               scale[c] = Sqrt( result.TwoSidedBiweightMidvariance( location[c], mad ) );

               o_output.finalScaleEstimates[c] = 0.991*double( scale[c] );
               o_output.finalLocationEstimates[c] = location[c];
            }

            result->Status().Complete();

            // N.B. Signal evaluation must be done *before* noise evaluation in
            // order to support the N* algorithm efficiently.
            signal_estimates psfSignal = EvaluatePSFSignal( result, true/*final*/ );
            DVector noise = EvaluateNoise( result, true/*final*/ );
            scale_estimates noiseScale = EvaluateNoiseScale( result, true/*final*/ );

            result->Status().Complete();
            result->Status().EnableInitialization();
            result->SetStatusCallback( nullptr );
            result->ResetSelections();

            console.Write( "<end><cbr>"
                           "<br>Scale estimates    :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
               console.Write( String().Format( " %.6e", 0.991*double( scale[c] ) ) );

            console.Write( "<br>Location estimates :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
               console.Write( String().Format( " %.6e", location[c] ) );

            console.Write( "<br>Noise estimates    :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
               console.Write( String().Format( " %.4e", noise[c] ) );

            console.Write( "<br>SNR estimates      :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
               console.Write( String().Format( " %.4e", double( noiseScale[c] )*double( noiseScale[c] )/noise[c]/noise[c] ) );

            console.Write( "<br>PSF signal weights :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
               console.Write( String().Format( " %.4e", PSFSignalEstimator::PSFSignalWeight( psfSignal[c], noise[c] ) ) );

            console.Write( "<br>PSF SNR estimates  :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
               console.Write( String().Format( " %.4e", PSFSignalEstimator::PSFSNR( psfSignal[c], noise[c] ) ) );

            console.Write( "<br>PSF fit counts     :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
               console.Write( String().Format( " %d", psfSignal[c].count ) );

            console.WriteLn();
            console.WriteLn();

         } // if p_evaluateSNR
      } // if p_generateIntegratedImage

      Module->ProcessEvents();

      if ( p_generateIntegratedImage )
      {
         /*
          * Update image properties and FITS keywords.
          */
         PropertyArray properties;
         FITSKeywordArray keywords = resultWindow.Keywords();

         /*
          * Consistently defined metadata.
          */
         IntegrationMetadata metadata = IntegrationFile::SummaryMetadata();
         if ( p_subtractPedestals )
            metadata.pedestal.Undefine();
         metadata.UpdatePropertiesAndKeywords( properties, keywords );

         /*
          * Integration HISTORY keywords.
          */
         keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Integration with " + PixInsightVersion::AsString() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Integration with " + Module->ReadableVersion() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Integration with ImageIntegration process" );

         const IntegrationDescriptionItems items( *this );
         if ( !items.pixelCombination.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.pixelCombination: " + items.pixelCombination );
         if ( !items.outputNormalization.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.outputNormalization: " + items.outputNormalization );
         if ( !items.weightMode.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.weightMode: " + items.weightMode );
         if ( !items.scaleEstimator.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.scaleEstimator: " + items.scaleEstimator );
         if ( !items.rangeRejection.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.rangeRejection: " + items.rangeRejection );
         if ( !items.pixelRejection.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.pixelRejection: " + items.pixelRejection );
         if ( !items.rejectionNormalization.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.rejectionNormalization: " + items.rejectionNormalization );
         if ( !items.rejectionClippings.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.rejectionClippings: " + items.rejectionClippings );
         if ( !items.rejectionParameters.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.rejectionParameters: " + items.rejectionParameters );
         if ( !items.largeScaleRejectionClippings.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.largeScaleRejectionClippings: " + items.largeScaleRejectionClippings );
         if ( !items.largeScaleRejectionParameters.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.largeScaleRejectionParameters: " + items.largeScaleRejectionParameters );
         if ( !items.regionOfInterest.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.regionOfInterest: " + items.regionOfInterest );

         keywords << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "ImageIntegration.numberOfImages: %d", IntegrationFile::NumberOfFiles() ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "ImageIntegration.totalPixels: %lu", o_output.totalPixels ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "ImageIntegration.outputRangeLow: %.8e", o_output.outputRangeLow ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "ImageIntegration.outputRangeHigh: %.8e", o_output.outputRangeHigh ) );

         if ( o_output.outputRangeLow < 0 || o_output.outputRangeHigh > 1 )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString( "ImageIntegration.outputRangeOperation: " )
                                       + (p_truncateOnOutOfRange ? "truncate" : ((o_output.outputRangeLow < 0) ? "rescale" : "normalize")) );

         IsoString totalRejectedLow = "ImageIntegration.totalRejectedLow:";
         IsoString totalRejectedHigh = "ImageIntegration.totalRejectedHigh:";
         for ( int j = 0; j < o_output.numberOfChannels; ++j )
         {
            totalRejectedLow.AppendFormat( " %lu(%.3f%%)",
                                 o_output.totalRejectedLow[j], 100.0*o_output.totalRejectedLow[j]/o_output.totalPixels );
            totalRejectedHigh.AppendFormat( " %lu(%.3f%%)",
                                 o_output.totalRejectedHigh[j], 100.0*o_output.totalRejectedHigh[j]/o_output.totalPixels );
         }
         keywords << FITSHeaderKeyword( "HISTORY", IsoString(), totalRejectedLow )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), totalRejectedHigh );

         if ( p_evaluateSNR )
         {
            IsoString finalLocationEstimates = "ImageIntegration.location:";
            IsoString finalScaleEstimates = "ImageIntegration.scale:";
            IsoString finalNoiseEstimates = "ImageIntegration.noise:";
            IsoString finalNoiseScaleEstimates = "ImageIntegration.noiseScale:";
            IsoString finalPSFTotalFluxEstimates = "ImageIntegration.psfTotalFlux:";
            IsoString finalPSFTotalPowerFluxEstimates = "ImageIntegration.psfTotalPowerFlux:";
            IsoString finalPSFTotalMeanFluxEstimates = "ImageIntegration.psfTotalMeanFlux:";
            IsoString finalPSFTotalMeanPowerFluxEstimates = "ImageIntegration.psfTotalMeanPowerFlux:";
            IsoString finalPSFMStarEstimates = "ImageIntegration.psfMStar:";
            IsoString finalPSFNStarEstimates = "ImageIntegration.psfNStar:";
            IsoString finalPSFCounts = "ImageIntegration.psfCounts:";

            for ( int j = 0; j < o_output.numberOfChannels; ++j )
            {
               finalLocationEstimates.AppendFormat( " %.6e", o_output.finalLocationEstimates[j] );
               finalScaleEstimates.AppendFormat( " %.6e", o_output.finalScaleEstimates[j] );
               finalNoiseEstimates.AppendFormat( " %.4e", o_output.finalNoiseEstimates[j] );
               finalNoiseScaleEstimates.AppendFormat( " %.6e", double( o_output.finalNoiseScaleEstimates[j] ) );
               finalPSFTotalFluxEstimates.AppendFormat( " %.4e", o_output.finalPSFTotalFluxEstimates[j] );
               finalPSFTotalPowerFluxEstimates.AppendFormat( " %.4e", o_output.finalPSFTotalPowerFluxEstimates[j] );
               finalPSFTotalMeanFluxEstimates.AppendFormat( " %.4e", o_output.finalPSFTotalMeanFluxEstimates[j] );
               finalPSFTotalMeanPowerFluxEstimates.AppendFormat( " %.4e", o_output.finalPSFTotalMeanPowerFluxEstimates[j] );
               finalPSFMStarEstimates.AppendFormat( " %.4e", o_output.finalPSFMStarEstimates[j] );
               finalPSFNStarEstimates.AppendFormat( " %.4e", o_output.finalPSFNStarEstimates[j] );
               finalPSFCounts.AppendFormat( " %u", o_output.finalPSFCounts[j] );
            }
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), finalLocationEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalScaleEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalNoiseEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalNoiseScaleEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalPSFTotalFluxEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalPSFTotalPowerFluxEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalPSFTotalMeanFluxEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalPSFTotalMeanPowerFluxEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalPSFMStarEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalPSFNStarEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalPSFCounts );

            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.psfType: " + IIPSFType::FunctionName( p_psfType ) );

            for ( int j = 0; j < o_output.numberOfChannels; ++j )
               keywords << FITSHeaderKeyword( IsoString().Format( "PSFFLX%02d", j ),
                                              IsoString().Format( "%.4e", o_output.finalPSFTotalFluxEstimates[j] ),
                                              IsoString().Format( "Sum of PSF flux estimates, channel #%d", j ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFFLP%02d", j ),
                                              IsoString().Format( "%.4e", o_output.finalPSFTotalPowerFluxEstimates[j] ),
                                              IsoString().Format( "Sum of squared PSF flux estimates, channel #%d", j ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMFL%02d", j ),
                                              IsoString().Format( "%.4e", o_output.finalPSFTotalMeanFluxEstimates[j] ),
                                              IsoString().Format( "Sum of mean PSF flux estimates, channel #%d", j ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMFP%02d", j ),
                                              IsoString().Format( "%.4e", o_output.finalPSFTotalMeanPowerFluxEstimates[j] ),
                                              IsoString().Format( "Sum of mean squared PSF flux estimates, channel #%d", j ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFMST%02d", j ),
                                              IsoString().Format( "%.4e", o_output.finalPSFMStarEstimates[j] ),
                                              IsoString().Format( "M* mean background estimate, channel #%d", j ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFNST%02d", j ),
                                              IsoString().Format( "%.4e", o_output.finalPSFNStarEstimates[j] ),
                                              IsoString().Format( "N* noise estimate, channel #%d", j ) )
                        << FITSHeaderKeyword( IsoString().Format( "PSFSGN%02d", j ),
                                              IsoString().Format( "%u", o_output.finalPSFCounts[j] ),
                                              IsoString().Format( "Number of valid PSF flux estimates, channel #%d", j ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISE%02d", j ),
                                              IsoString().Format( "%.4e", o_output.finalNoiseEstimates[j] ),
                                              IsoString().Format( "Noise estimate, channel #%d", j ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEL%02d", j ),
                                              IsoString().Format( "%.6e", o_output.finalNoiseScaleEstimates[j].low ),
                                              IsoString().Format( "Noise scaling factor, low pixels, channel #%d", j ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEH%02d", j ),
                                              IsoString().Format( "%.6e", o_output.finalNoiseScaleEstimates[j].high ),
                                              IsoString().Format( "Noise scaling factor, high pixels, channel #%d", j ) )
                        << FITSHeaderKeyword( IsoString().Format( "NOISEA%02d", j ),
                                              IsoString( o_output.finalNoiseAlgorithms[j] ),
                                              IsoString().Format( "Noise evaluation algorithm, channel #%d", j ) );

            keywords << FITSHeaderKeyword( "PSFSGTYP",
                                           IIPSFType::FunctionName( p_psfType ).SingleQuoted(),
                                           "PSF type used for signal estimation" );
         } // if p_evaluateSNR

         if ( !p_subtractPedestals )
            if ( metadata.IsValid() )
               if ( metadata.pedestal.IsConsistentlyDefined() )
               {
                  keywords << FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString().Format( "ImageIntegration.outputPedestal: %.4f DN", metadata.pedestal() ) );
                  console.NoteLn( String().Format( "<end><cbr>* PEDESTAL keyword created with value = %.4f DN", metadata.pedestal() ) );
               }

         /*
          * Per-image HISTORY keywords.
          */
         for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
         {
            const IntegrationFile& file = IntegrationFile::FileByIndex( i );

            if ( file.HasScaleEstimates() )
            {
               IsoString scaleEstimates = IsoString().Format( "ImageIntegration.scaleEstimates_%d:", i );
               for ( int j = 0; j < IntegrationFile::NumberOfChannels(); ++j )
                  scaleEstimates.AppendFormat( " %.6e", double( file.ScaleEstimates( j ) ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), scaleEstimates );
            }

            if ( file.HasLocationEstimates() )
            {
               IsoString locationEstimates = IsoString().Format( "ImageIntegration.locationEstimates_%d:", i );
               for ( int j = 0; j < IntegrationFile::NumberOfChannels(); ++j )
                  locationEstimates.AppendFormat( " %.6e", file.LocationEstimate( j ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), locationEstimates );
            }

            if ( file.HasNoiseEstimates() )
            {
               IsoString noiseEstimates = IsoString().Format( "ImageIntegration.noiseEstimates_%d:", i );
               for ( int j = 0; j < IntegrationFile::NumberOfChannels(); ++j )
                  noiseEstimates.AppendFormat( " %.4e", file.NoiseEstimate( j ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), noiseEstimates );
            }

            if ( file.HasNoiseScaleEstimates() )
            {
               IsoString noiseScaleEstimates = IsoString().Format( "ImageIntegration.noiseScaleEstimates_%d:", i );
               for ( int j = 0; j < IntegrationFile::NumberOfChannels(); ++j )
                  noiseScaleEstimates.AppendFormat( " %.6e", double( file.NoiseScaleEstimates( j ) ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), noiseScaleEstimates );
            }

            if ( file.HasImageWeights() )
            {
               IsoString imageWeights = IsoString().Format( "ImageIntegration.imageWeights_%d:", i );
               for ( int j = 0; j < IntegrationFile::NumberOfChannels(); ++j )
                  imageWeights.AppendFormat( " %.5e", file.ImageWeight( j ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), imageWeights );
            }

            if ( file.HasScaleFactors() )
            {
               IsoString scaleFactors = IsoString().Format( "ImageIntegration.scaleFactors_%d:", i );
               for ( int j = 0; j < IntegrationFile::NumberOfChannels(); ++j )
                  scaleFactors.AppendFormat( " %.6e", double( file.ScaleFactor( j ) ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), scaleFactors );
            }

            if ( file.HasZeroOffsets() )
            {
               IsoString zeroOffsets = IsoString().Format( "ImageIntegration.zeroOffsets_%d:", i );
               for ( int j = 0; j < IntegrationFile::NumberOfChannels(); ++j )
                  zeroOffsets.AppendFormat( " %+.6e", file.ZeroOffset( j ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), zeroOffsets );
            }

            IsoString rejectedLow = IsoString().Format( "ImageIntegration.rejectedLow_%d:", i );
            for ( int j = 0; j < o_output.numberOfChannels; ++j )
               rejectedLow.AppendFormat( " %lu", o_output.imageData[i].rejectedLow[j] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), rejectedLow );

            IsoString rejectedHigh = IsoString().Format( "ImageIntegration.rejectedHigh_%d:", i );
            for ( int j = 0; j < o_output.numberOfChannels; ++j )
               rejectedHigh.AppendFormat( " %lu", o_output.imageData[i].rejectedHigh[j] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), rejectedHigh );
         } // for each input file

         resultWindow.SetKeywords( keywords );
         resultWindow.MainView().SetStorablePermanentProperties( properties, false/*notify*/ );
         if ( p_showImages )
            resultWindow.Show();

         /*
          * Update instance output properties.
          */
         o_output.integrationImageId = resultWindow.MainView().Id();
         for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
            for ( int j = 0; j < o_output.numberOfChannels; ++j )
               o_output.imageData[i].weights[j] = IntegrationFile::FileByIndex( i ).Weight( j );
      } // if p_generateIntegratedImage

      if ( p_generateDrizzleData )
         if ( p_useROI )
         {
            console.WarningLn( "** Warning: Drizzle data files cannot be updated with an active ROI. "
                               "Please integrate the entire image to update drizzle data files." );
         }
         else
         {
            /*
             * Generate Drizzle integration data.
             */
            console.WriteLn( "* Updating drizzle data files:" );

            int succeeded = 0;
            for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
            {
               const IntegrationFile& F = IntegrationFile::FileByIndex( i );
               if ( F.DrizzleDataPath().IsEmpty() )
               {
                  console.WarningLn( "** Warning: Drizzle data file not defined: " + F.Path() );
                  continue;
               }
               if ( !File::Exists( F.DrizzleDataPath() ) )
               {
                  console.WarningLn( "** Warning: Drizzle data file not found: " + F.DrizzleDataPath() );
                  continue;
               }

               Module->ProcessEvents();

               try
               {
                  DrizzleData drz( F.DrizzleDataPath(), DrizzleParserOption::IgnoreIntegrationData );
                  F.ToDrizzleData( drz );
                  String newDrzDataPath = File::ChangeExtension( F.DrizzleDataPath(), ".xdrz" ); // don't overwrite old .drz files
                  drz.SerializeToFile( newDrzDataPath );
                  console.WriteLn( newDrzDataPath );
                  ++succeeded;
               }
               catch ( ProcessAborted& )
               {
                  throw;
               }
               catch ( ... )
               {
                  try
                  {
                     throw;
                  }
                  ERROR_HANDLER
               }

               Module->ProcessEvents();
            }

            if ( succeeded < IntegrationFile::NumberOfFiles() )
               console.WarningLn( String().Format( "<end><cbr>** Warning: Drizzle data file generation: %d succeeded, %d errors.",
                                                   succeeded, IntegrationFile::NumberOfFiles()-succeeded ) );
            else
               console.NoteLn( String().Format( "<end><cbr>* %d drizzle data files have been updated successfully.", succeeded ) );
         }

      IntegrationFile::CloseAll();

      if ( generateLowRejectionMap )
      {
         if ( p_showImages )
            lowRejectionMapWindow.Show();
         o_output.lowRejectionMapImageId = lowRejectionMapWindow.MainView().Id();
      }

      if ( generateHighRejectionMap )
      {
         if ( p_showImages )
            highRejectionMapWindow.Show();
         o_output.highRejectionMapImageId = highRejectionMapWindow.MainView().Id();
      }

      if ( generateSlopeMap )
      {
         if ( p_showImages )
            slopeMapWindow.Show();
         o_output.slopeMapImageId = slopeMapWindow.MainView().Id();
      }

      return true;
   }
   catch ( ... )
   {
      IntegrationFile::CloseAll();

      if ( !resultWindow.IsNull() )
         resultWindow.Close();

      if ( !lowRejectionMapWindow.IsNull() )
         lowRejectionMapWindow.Close();

      if ( !highRejectionMapWindow.IsNull() )
         highRejectionMapWindow.Close();

      if ( !slopeMapWindow.IsNull() )
         slopeMapWindow.Close();

      throw;
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void* ImageIntegrationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheIIImageEnabledParameter )
      return &p_images[tableRow].enabled;
   if ( p == TheIIImagePathParameter )
      return p_images[tableRow].path.Begin();
   if ( p == TheIILocalNormalizationDataPathParameter )
      return p_images[tableRow].nmlPath.Begin();
   if ( p == TheIIDrizzlePathParameter )
      return p_images[tableRow].drzPath.Begin();
   if ( p == TheIIInputHintsParameter )
      return p_inputHints.Begin();
   if ( p == TheIICombinationParameter )
      return &p_combination;
   if ( p == TheIINormalizationParameter )
      return &p_normalization;
   if ( p == TheIIWeightModeParameter )
      return &p_weightMode;
   if ( p == TheIIWeightKeywordParameter )
      return p_weightKeyword.Begin();
   else if ( p == TheIICSVWeightsParameter )
      return p_csvWeights.Begin();
   if ( p == TheIIWeightScaleParameter )
      return &p_weightScale;
   if ( p == TheIIMinWeightParameter )
      return &p_minWeight;
   if ( p == TheIIAdaptiveGridSizeParameter )
      return &p_adaptiveGridSize;
   if ( p == TheIIAdaptiveNoScaleParameter )
      return &p_adaptiveNoScale;
   if ( p == TheIIIgnoreNoiseKeywordsParameter )
      return &p_ignoreNoiseKeywords;
   if ( p == TheIIRejectionParameter )
      return &p_rejection;
   if ( p == TheIIRejectionNormalizationParameter )
      return &p_rejectionNormalization;
   if ( p == TheIIMinMaxLowParameter )
      return &p_minMaxLow;
   if ( p == TheIIMinMaxHighParameter )
      return &p_minMaxHigh;
   if ( p == TheIIPercentileLowParameter )
      return &p_pcClipLow;
   if ( p == TheIIPercentileHighParameter )
      return &p_pcClipHigh;
   if ( p == TheIISigmaLowParameter )
      return &p_sigmaLow;
   if ( p == TheIISigmaHighParameter )
      return &p_sigmaHigh;
   if ( p == TheIIWinsorizationCutoffParameter )
      return &p_winsorizationCutoff;
   if ( p == TheIILinearFitLowParameter )
      return &p_linearFitLow;
   if ( p == TheIILinearFitHighParameter )
      return &p_linearFitHigh;
   if ( p == TheIIESDOutliersFractionParameter )
      return &p_esdOutliersFraction;
   if ( p == TheIIESDAlphaParameter )
      return &p_esdAlpha;
   if ( p == TheIIESDLowRelaxationParameter )
      return &p_esdLowRelaxation;
   if ( p == TheIIRCRLimitParameter )
      return &p_rcrLimit;
   if ( p == TheIICCDGainParameter )
      return &p_ccdGain;
   if ( p == TheIICCDReadNoiseParameter )
      return &p_ccdReadNoise;
   if ( p == TheIICCDScaleNoiseParameter )
      return &p_ccdScaleNoise;
   if ( p == TheIIClipLowParameter )
      return &p_clipLow;
   if ( p == TheIIClipHighParameter )
      return &p_clipHigh;
   if ( p == TheIIRangeClipLowParameter )
      return &p_rangeClipLow;
   if ( p == TheIIRangeLowParameter )
      return &p_rangeLow;
   if ( p == TheIIRangeClipHighParameter )
      return &p_rangeClipHigh;
   if ( p == TheIIRangeHighParameter )
      return &p_rangeHigh;
   if ( p == TheIIReportRangeRejectionParameter )
      return &p_reportRangeRejection;
   if ( p == TheIIMapRangeRejectionParameter )
      return &p_mapRangeRejection;
   if ( p == TheIILargeScaleClipLowParameter )
      return &p_largeScaleClipLow;
   if ( p == TheIILargeScaleClipLowProtectedLayersParameter )
      return &p_largeScaleClipLowProtectedLayers;
   if ( p == TheIILargeScaleClipLowGrowthParameter )
      return &p_largeScaleClipLowGrowth;
   if ( p == TheIILargeScaleClipHighParameter )
      return &p_largeScaleClipHigh;
   if ( p == TheIILargeScaleClipHighProtectedLayersParameter )
      return &p_largeScaleClipHighProtectedLayers;
   if ( p == TheIILargeScaleClipHighGrowthParameter )
      return &p_largeScaleClipHighGrowth;
   if ( p == TheIIGenerate64BitResultParameter )
      return &p_generate64BitResult;
   if ( p == TheIIGenerateRejectionMapsParameter )
      return &p_generateRejectionMaps;
   if ( p == TheIIGenerateIntegratedImageParameter )
      return &p_generateIntegratedImage;
   if ( p == TheIIGenerateDrizzleDataParameter )
      return &p_generateDrizzleData;
   if ( p == TheIIClosePreviousImagesParameter )
      return &p_closePreviousImages;
   if ( p == TheIIBufferSizeParameter )
      return &p_bufferSizeMB;
   if ( p == TheIIStackSizeParameter )
      return &p_stackSizeMB;
   if ( p == TheIIAutoMemorySizeParameter )
      return &p_autoMemorySize;
   if ( p == TheIIAutoMemoryLimitParameter )
      return &p_autoMemoryLimit;
   if ( p == TheIIUseROIParameter )
      return &p_useROI;
   if ( p == TheIIROIX0Parameter )
      return &p_roi.x0;
   if ( p == TheIIROIY0Parameter )
      return &p_roi.y0;
   if ( p == TheIIROIX1Parameter )
      return &p_roi.x1;
   if ( p == TheIIROIY1Parameter )
      return &p_roi.y1;
   if ( p == TheIIUseCacheParameter )
      return &p_useCache;
   if ( p == TheIIEvaluateSNRParameter )
      return &p_evaluateSNR;
   if ( p == TheIINoiseEvaluationAlgorithmParameter )
      return &p_noiseEvaluationAlgorithm;
   if ( p == TheIIMRSMinDataFractionParameter )
      return &p_mrsMinDataFraction;
   if ( p == TheIIPSFStructureLayersParameter )
      return &p_psfStructureLayers;
   if ( p == TheIIPSFTypeParameter )
      return &p_psfType;
   if ( p == TheIISubtractPedestalsParameter )
      return &p_subtractPedestals;
   if ( p == TheIITruncateOnOutOfRangeParameter )
      return &p_truncateOnOutOfRange;
   if ( p == TheIINoGUIMessagesParameter )
      return &p_noGUIMessages;
   if ( p == TheIIShowImagesParameter )
      return &p_showImages;
   if ( p == TheIIUseFileThreadsParameter )
      return &p_useFileThreads;
   if ( p == TheIIFileThreadOverloadParameter )
      return &p_fileThreadOverload;
   if ( p == TheIIUseBufferThreadsParameter )
      return &p_useBufferThreads;
   if ( p == TheIIMaxBufferThreadsParameter )
      return &p_maxBufferThreads;

   if ( p == TheIIIntegrationImageIdParameter )
      return o_output.integrationImageId.Begin();
   if ( p == TheIILowRejectionMapImageIdParameter )
      return o_output.lowRejectionMapImageId.Begin();
   if ( p == TheIIHighRejectionMapImageIdParameter )
      return o_output.highRejectionMapImageId.Begin();
   if ( p == TheIISlopeMapImageIdParameter )
      return o_output.slopeMapImageId.Begin();
   if ( p == TheIINumberOfChannelsParameter )
      return &o_output.numberOfChannels;
   if ( p == TheIINumberOfPixelsParameter )
      return &o_output.numberOfPixels;
   if ( p == TheIITotalPixelsParameter )
      return &o_output.totalPixels;

   if ( p == TheIIOutputRangeLowParameter )
      return &o_output.outputRangeLow;
   if ( p == TheIIOutputRangeHighParameter )
      return &o_output.outputRangeHigh;

   if ( p == TheIITotalRejectedLowRKParameter )
      return o_output.totalRejectedLow.At( 0 );
   if ( p == TheIITotalRejectedLowGParameter )
      return o_output.totalRejectedLow.At( 1 );
   if ( p == TheIITotalRejectedLowBParameter )
      return o_output.totalRejectedLow.At( 2 );

   if ( p == TheIITotalRejectedHighRKParameter )
      return o_output.totalRejectedHigh.At( 0 );
   if ( p == TheIITotalRejectedHighGParameter )
      return o_output.totalRejectedHigh.At( 1 );
   if ( p == TheIITotalRejectedHighBParameter )
      return o_output.totalRejectedHigh.At( 2 );

   if ( p == TheIIFinalNoiseEstimateRKParameter )
      return o_output.finalNoiseEstimates.At( 0 );
   if ( p == TheIIFinalNoiseEstimateGParameter )
      return o_output.finalNoiseEstimates.At( 1 );
   if ( p == TheIIFinalNoiseEstimateBParameter )
      return o_output.finalNoiseEstimates.At( 2 );

   if ( p == TheIIFinalNoiseScaleEstimateLowRKParameter )
      return &o_output.finalNoiseScaleEstimates[0].low;
   if ( p == TheIIFinalNoiseScaleEstimateLowGParameter )
      return &o_output.finalNoiseScaleEstimates[1].low;
   if ( p == TheIIFinalNoiseScaleEstimateLowBParameter )
      return &o_output.finalNoiseScaleEstimates[2].low;

   if ( p == TheIIFinalNoiseScaleEstimateHighRKParameter )
      return &o_output.finalNoiseScaleEstimates[0].high;
   if ( p == TheIIFinalNoiseScaleEstimateHighGParameter )
      return &o_output.finalNoiseScaleEstimates[1].high;
   if ( p == TheIIFinalNoiseScaleEstimateHighBParameter )
      return &o_output.finalNoiseScaleEstimates[2].high;

   if ( p == TheIIFinalNoiseAlgorithmRKParameter )
      return o_output.finalNoiseAlgorithms[0].Begin();
   if ( p == TheIIFinalNoiseAlgorithmGParameter )
      return o_output.finalNoiseAlgorithms[1].Begin();
   if ( p == TheIIFinalNoiseAlgorithmBParameter )
      return o_output.finalNoiseAlgorithms[2].Begin();

   if ( p == TheIIFinalScaleEstimateRKParameter )
      return o_output.finalScaleEstimates.At( 0 );
   if ( p == TheIIFinalScaleEstimateGParameter )
      return o_output.finalScaleEstimates.At( 1 );
   if ( p == TheIIFinalScaleEstimateBParameter )
      return o_output.finalScaleEstimates.At( 2 );

   if ( p == TheIIFinalLocationEstimateRKParameter )
      return o_output.finalLocationEstimates.At( 0 );
   if ( p == TheIIFinalLocationEstimateGParameter )
      return o_output.finalLocationEstimates.At( 1 );
   if ( p == TheIIFinalLocationEstimateBParameter )
      return o_output.finalLocationEstimates.At( 2 );

   if ( p == TheIIFinalPSFTotalFluxEstimateRKParameter )
      return o_output.finalPSFTotalFluxEstimates.At( 0 );
   if ( p == TheIIFinalPSFTotalFluxEstimateGParameter )
      return o_output.finalPSFTotalFluxEstimates.At( 1 );
   if ( p == TheIIFinalPSFTotalFluxEstimateBParameter )
      return o_output.finalPSFTotalFluxEstimates.At( 2 );

   if ( p == TheIIFinalPSFTotalPowerFluxEstimateRKParameter )
      return o_output.finalPSFTotalPowerFluxEstimates.At( 0 );
   if ( p == TheIIFinalPSFTotalPowerFluxEstimateGParameter )
      return o_output.finalPSFTotalPowerFluxEstimates.At( 1 );
   if ( p == TheIIFinalPSFTotalPowerFluxEstimateBParameter )
      return o_output.finalPSFTotalPowerFluxEstimates.At( 2 );

   if ( p == TheIIFinalPSFTotalMeanFluxEstimateRKParameter )
      return o_output.finalPSFTotalMeanFluxEstimates.At( 0 );
   if ( p == TheIIFinalPSFTotalMeanFluxEstimateGParameter )
      return o_output.finalPSFTotalMeanFluxEstimates.At( 1 );
   if ( p == TheIIFinalPSFTotalMeanFluxEstimateBParameter )
      return o_output.finalPSFTotalMeanFluxEstimates.At( 2 );

   if ( p == TheIIFinalPSFTotalMeanPowerFluxEstimateRKParameter )
      return o_output.finalPSFTotalMeanPowerFluxEstimates.At( 0 );
   if ( p == TheIIFinalPSFTotalMeanPowerFluxEstimateGParameter )
      return o_output.finalPSFTotalMeanPowerFluxEstimates.At( 1 );
   if ( p == TheIIFinalPSFTotalMeanPowerFluxEstimateBParameter )
      return o_output.finalPSFTotalMeanPowerFluxEstimates.At( 2 );

   if ( p == TheIIFinalPSFMStarEstimateRKParameter )
      return o_output.finalPSFMStarEstimates.At( 0 );
   if ( p == TheIIFinalPSFMStarEstimateGParameter )
      return o_output.finalPSFMStarEstimates.At( 1 );
   if ( p == TheIIFinalPSFMStarEstimateBParameter )
      return o_output.finalPSFMStarEstimates.At( 2 );

   if ( p == TheIIFinalPSFNStarEstimateRKParameter )
      return o_output.finalPSFNStarEstimates.At( 0 );
   if ( p == TheIIFinalPSFNStarEstimateGParameter )
      return o_output.finalPSFNStarEstimates.At( 1 );
   if ( p == TheIIFinalPSFNStarEstimateBParameter )
      return o_output.finalPSFNStarEstimates.At( 2 );

   if ( p == TheIIFinalPSFCountRKParameter )
      return o_output.finalPSFCounts.At( 0 );
   if ( p == TheIIFinalPSFCountGParameter )
      return o_output.finalPSFCounts.At( 1 );
   if ( p == TheIIFinalPSFCountBParameter )
      return o_output.finalPSFCounts.At( 2 );

   if ( p == TheIIReferenceNoiseReductionRKParameter )
      return o_output.referenceNoiseReductions.At( 0 );
   if ( p == TheIIReferenceNoiseReductionGParameter )
      return o_output.referenceNoiseReductions.At( 1 );
   if ( p == TheIIReferenceNoiseReductionBParameter )
      return o_output.referenceNoiseReductions.At( 2 );

   if ( p == TheIIMedianNoiseReductionRKParameter )
      return o_output.medianNoiseReductions.At( 0 );
   if ( p == TheIIMedianNoiseReductionGParameter )
      return o_output.medianNoiseReductions.At( 1 );
   if ( p == TheIIMedianNoiseReductionBParameter )
      return o_output.medianNoiseReductions.At( 2 );

   if ( p == TheIIReferenceSNRIncrementRKParameter )
      return o_output.referenceSNRIncrements.At( 0 );
   if ( p == TheIIReferenceSNRIncrementGParameter )
      return o_output.referenceSNRIncrements.At( 1 );
   if ( p == TheIIReferenceSNRIncrementBParameter )
      return o_output.referenceSNRIncrements.At( 2 );

   if ( p == TheIIAverageSNRIncrementRKParameter )
      return o_output.averageSNRIncrements.At( 0 );
   if ( p == TheIIAverageSNRIncrementGParameter )
      return o_output.averageSNRIncrements.At( 1 );
   if ( p == TheIIAverageSNRIncrementBParameter )
      return o_output.averageSNRIncrements.At( 2 );

   if ( p == TheIIImageWeightRKParameter )
      return o_output.imageData[tableRow].weights.At( 0 );
   if ( p == TheIIImageWeightGParameter )
      return o_output.imageData[tableRow].weights.At( 1 );
   if ( p == TheIIImageWeightBParameter )
      return o_output.imageData[tableRow].weights.At( 2 );

   if ( p == TheIIImageRejectedLowRKParameter )
      return o_output.imageData[tableRow].rejectedLow.At( 0 );
   if ( p == TheIIImageRejectedLowGParameter )
      return o_output.imageData[tableRow].rejectedLow.At( 1 );
   if ( p == TheIIImageRejectedLowBParameter )
      return o_output.imageData[tableRow].rejectedLow.At( 2 );

   if ( p == TheIIImageRejectedHighRKParameter )
      return o_output.imageData[tableRow].rejectedHigh.At( 0 );
   if ( p == TheIIImageRejectedHighGParameter )
      return o_output.imageData[tableRow].rejectedHigh.At( 1 );
   if ( p == TheIIImageRejectedHighBParameter )
      return o_output.imageData[tableRow].rejectedHigh.At( 2 );

   return nullptr;
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheIIImagesParameter )
   {
      p_images.Clear();
      if ( sizeOrLength > 0 )
         p_images.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheIIImagePathParameter )
   {
      p_images[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_images[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheIILocalNormalizationDataPathParameter )
   {
      p_images[tableRow].nmlPath.Clear();
      if ( sizeOrLength > 0 )
         p_images[tableRow].nmlPath.SetLength( sizeOrLength );
   }
   else if ( p == TheIIDrizzlePathParameter )
   {
      p_images[tableRow].drzPath.Clear();
      if ( sizeOrLength > 0 )
         p_images[tableRow].drzPath.SetLength( sizeOrLength );
   }
   else if ( p == TheIIInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheIIWeightKeywordParameter )
   {
      p_weightKeyword.Clear();
      if ( sizeOrLength > 0 )
         p_weightKeyword.SetLength( sizeOrLength );
   }
   else if ( p == TheIICSVWeightsParameter )
   {
      p_csvWeights.Clear();
      if ( sizeOrLength > 0 )
         p_csvWeights.SetLength( sizeOrLength );
   }
   else if ( p == TheIIIntegrationImageIdParameter )
   {
      o_output.integrationImageId.Clear();
      if ( sizeOrLength > 0 )
         o_output.integrationImageId.SetLength( sizeOrLength );
   }
   else if ( p == TheIILowRejectionMapImageIdParameter )
   {
      o_output.lowRejectionMapImageId.Clear();
      if ( sizeOrLength > 0 )
         o_output.lowRejectionMapImageId.SetLength( sizeOrLength );
   }
   else if ( p == TheIIHighRejectionMapImageIdParameter )
   {
      o_output.highRejectionMapImageId.Clear();
      if ( sizeOrLength > 0 )
         o_output.highRejectionMapImageId.SetLength( sizeOrLength );
   }
   else if ( p == TheIISlopeMapImageIdParameter )
   {
      o_output.slopeMapImageId.Clear();
      if ( sizeOrLength > 0 )
         o_output.slopeMapImageId.SetLength( sizeOrLength );
   }
   else if ( p == TheIIFinalNoiseAlgorithmRKParameter )
   {
      o_output.finalNoiseAlgorithms[0].Clear();
      if ( sizeOrLength > 0 )
         o_output.finalNoiseAlgorithms[0].SetLength( sizeOrLength );
   }
   else if ( p == TheIIFinalNoiseAlgorithmGParameter )
   {
      o_output.finalNoiseAlgorithms[1].Clear();
      if ( sizeOrLength > 0 )
         o_output.finalNoiseAlgorithms[1].SetLength( sizeOrLength );
   }
   else if ( p == TheIIFinalNoiseAlgorithmBParameter )
   {
      o_output.finalNoiseAlgorithms[2].Clear();
      if ( sizeOrLength > 0 )
         o_output.finalNoiseAlgorithms[2].SetLength( sizeOrLength );
   }
   else if ( p == TheIIImageDataParameter )
   {
      o_output.imageData.Clear();
      if ( sizeOrLength > 0 )
         o_output.imageData.Append( OutputData::ImageData(), sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type ImageIntegrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheIIImagesParameter )
      return p_images.Length();
   if ( p == TheIIImagePathParameter )
      return p_images[tableRow].path.Length();
   if ( p == TheIILocalNormalizationDataPathParameter )
      return p_images[tableRow].nmlPath.Length();
   if ( p == TheIIDrizzlePathParameter )
      return p_images[tableRow].drzPath.Length();
   if ( p == TheIIInputHintsParameter )
      return p_inputHints.Length();
   if ( p == TheIIWeightKeywordParameter )
      return p_weightKeyword.Length();
   if ( p == TheIICSVWeightsParameter )
      return p_csvWeights.Length();
   if ( p == TheIIIntegrationImageIdParameter )
      return o_output.integrationImageId.Length();
   if ( p == TheIILowRejectionMapImageIdParameter )
      return o_output.lowRejectionMapImageId.Length();
   if ( p == TheIIHighRejectionMapImageIdParameter )
      return o_output.highRejectionMapImageId.Length();
   if ( p == TheIISlopeMapImageIdParameter )
      return o_output.slopeMapImageId.Length();
   if ( p == TheIIFinalNoiseAlgorithmRKParameter )
      return o_output.finalNoiseAlgorithms[0].Length();
   if ( p == TheIIFinalNoiseAlgorithmGParameter )
      return o_output.finalNoiseAlgorithms[1].Length();
   if ( p == TheIIFinalNoiseAlgorithmBParameter )
      return o_output.finalNoiseAlgorithms[2].Length();
   if ( p == TheIIImageDataParameter )
      return o_output.imageData.Length();

   return 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Estimation of the standard deviation of the noise, assuming a Gaussian
 * noise distribution.
 *
 * - Use MRS noise evaluation when the algorithm converges for 4 >= J >= 2
 *
 * - Use k-sigma noise evaluation when either MRS doesn't converge or when the
 *   length of the set of noise pixels is less than the specified fraction of
 *   the image area (p_mrsMinDataFraction).
 *
 * - Automatically iterate to find the highest wavelet layer where noise can be
 *   successfully evaluated, in the [1,4] range.
 */
DVector ImageIntegrationInstance::EvaluateNoise( const ImageVariant& image, bool final ) const
{
   /*
    * 5x5 B3-spline wavelet scaling function. Used by the noise estimation
    * routines.
    *
    * Kernel filter coefficients:
    *
    *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256,
    *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
    *   3.0/128, 3.0/32, 9.0/64,  3.0/32, 3.0/128,
    *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
    *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256
    *
    * Note that we use this scaling function as a separable filter (row and
    * column vectors) for performance reasons.
    */
   // Separable filter coefficients
   static const float B3S_hv[] = { 0.0625F, 0.25F, 0.375F, 0.25F, 0.0625F };
   // Gaussian noise scaling factors
   static const float B3S_kj[] = { 0.8907F, 0.2007F, 0.0856F, 0.0413F, 0.0205F, 0.0103F, 0.0052F, 0.0026F, 0.0013F, 0.0007F };

   Console console;

   String algName;
   switch ( p_noiseEvaluationAlgorithm )
   {
   case IINoiseEvaluationAlgorithm::KSigma: algName = "k-sigma"; break;
   case IINoiseEvaluationAlgorithm::MRS:    algName = "MRS"; break;
   default: // ?!
   case IINoiseEvaluationAlgorithm::NStar:  algName = "N*"; break;
   }

   SpinStatus spin;
   image->SetStatusCallback( &spin );
   image->Status().Initialize( "Noise evaluation (" + algName + ")" );
   image->Status().DisableInitialization();

   image->ResetSelections();

   DVector noise( image->NumberOfNominalChannels() );

   for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
   {
      Module->ProcessEvents();
      if ( console.AbortRequested() )
         throw ProcessAborted();

      image->SelectChannel( c );

      switch ( p_noiseEvaluationAlgorithm )
      {
      case IINoiseEvaluationAlgorithm::KSigma:
         {
            ATrousWaveletTransform::WaveletScalingFunction H( SeparableFilter( B3S_hv, B3S_hv, 5 ) );
            ATrousWaveletTransform W( H, 1 );
            W << image;
            size_type N;
            noise[c] = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/B3S_kj[0];
            if ( final )
               o_output.finalNoiseAlgorithms[c] = "K-Sigma";
         }
         break;

      case IINoiseEvaluationAlgorithm::MRS:
         {
            ATrousWaveletTransform::WaveletScalingFunction H( SeparableFilter( B3S_hv, B3S_hv, 5 ) );
            double s0 = 0;
            for ( int n = 4;; )
            {
               Module->ProcessEvents();

               ATrousWaveletTransform W( H, n );
               W << image;

               size_type N;
               if ( n == 4 )
                  s0 = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/B3S_kj[0];
               noise[c] = W.NoiseMRS( image, B3S_kj, s0, 3, &N );

               if ( noise[c] > 0 )
                  if ( N >= size_type( p_mrsMinDataFraction * image->NumberOfPixels() ) )
                  {
                     if ( final )
                        o_output.finalNoiseAlgorithms[c] = "MRS";
                     break;
                  }

               if ( --n == 1 )
               {
                  console.WarningLn( "<end><cbr>** Warning: No convergence in MRS noise evaluation routine"
                                     " - using K-sigma noise estimate." );
                  noise[c] = s0;
                  if ( final )
                     o_output.finalNoiseAlgorithms[c] = "K-Sigma";
                  break;
               }
            }
         }
         break;

      default: // ?!
      case IINoiseEvaluationAlgorithm::NStar:
         if ( final )
         {
            noise[c] = o_output.finalPSFNStarEstimates[c];
            o_output.finalNoiseAlgorithms[c] = "N-star";
         }
         else
         {
            Array<float> R = PSFSignalEstimator::LocalBackgroundResidual( image );
            noise[c] = PSFSignalEstimator::NStar( R );
         }
         break;
      }

      if ( final )
         o_output.finalNoiseEstimates[c] = noise[c];
   }

   image->Status().Complete();
   image->Status().EnableInitialization();
   image->SetStatusCallback( nullptr );
   image->ResetSelections();

   return noise;
}

// ----------------------------------------------------------------------------

ImageIntegrationInstance::scale_estimates ImageIntegrationInstance::EvaluateNoiseScale( const ImageVariant& image, bool final ) const
{
   Console console;

   SpinStatus spin;
   image->SetStatusCallback( &spin );
   image->Status().Initialize( "Computing noise scaling factors" );
   image->Status().DisableInitialization();

   Module->ProcessEvents();

   scale_estimates noiseScale( image->NumberOfChannels() );

   image->ResetSelections();

   const int k = 4;

   for ( int c = 0; c < image->NumberOfChannels(); ++c )
   {
      image->SelectChannel( c );

      Module->ProcessEvents();

      const double clipLow = 2.0/65535;
      const double clipHigh = 1.0 - 2.0/65535;

      image->SetRangeClipping( clipLow, clipHigh );
      double center = image.Median();

      image->SetRangeClipping( clipLow, center );
      image->SetRangeClipping( Max( clipLow, center - k*image.StdDev() ), center );
      image->SetRangeClipping( Max( clipLow, center - k*image.StdDev() ), center );
      noiseScale[c].low = image.StdDev();

      image->SetRangeClipping( center, clipHigh );
      image->SetRangeClipping( center, Min( center + k*image.StdDev(), clipHigh ) );
      image->SetRangeClipping( center, Min( center + k*image.StdDev(), clipHigh ) );
      noiseScale[c].high = image.StdDev();

      if ( final )
         o_output.finalNoiseScaleEstimates[c] = noiseScale[c];
   }

   image->Status().Complete();
   image->Status().EnableInitialization();
   image->SetStatusCallback( nullptr );
   image->ResetSelections();
   image->ResetRangeClipping();

   return noiseScale;
}

// ----------------------------------------------------------------------------

ImageIntegrationInstance::signal_estimates ImageIntegrationInstance::EvaluatePSFSignal( const ImageVariant& image, bool final ) const
{
   Console console;

   SpinStatus spin;
   image->SetStatusCallback( &spin );
   image->Status().EnableInitialization();

   Module->ProcessEvents();

   signal_estimates psfSignal( image->NumberOfChannels() );

   image->ResetSelections();

   PSFSignalEstimator E;
   E.Detector().SetStructureLayers( p_psfStructureLayers );
   E.Detector().EnableClusteredSources();
   E.Detector().DisableLocalMaximaDetection();
   E.SetPSFType( IIPSFType::ToPSFFunction( p_psfType ) );
   for ( int c = 0; c < image.NumberOfChannels(); ++c )
   {
      console.WriteLn( String().Format( "<end><cbr>PSF signal evaluation (ch %d):", c ) );
      Module->ProcessEvents();

      image.SelectChannel( c );
      psfSignal[c] = E( image );

      image->Status().Complete();
      Module->ProcessEvents();

      if ( final )
      {
         o_output.finalPSFTotalFluxEstimates[c] = psfSignal[c].totalFlux;
         o_output.finalPSFTotalPowerFluxEstimates[c] = psfSignal[c].totalPowerFlux;
         o_output.finalPSFTotalMeanFluxEstimates[c] = psfSignal[c].totalMeanFlux;
         o_output.finalPSFTotalMeanPowerFluxEstimates[c] = psfSignal[c].totalMeanPowerFlux;
         o_output.finalPSFMStarEstimates[c] = psfSignal[c].MStar;
         o_output.finalPSFNStarEstimates[c] = psfSignal[c].NStar;
         o_output.finalPSFCounts[c] = psfSignal[c].count;
      }
   }

   image->SetStatusCallback( nullptr );
   image->ResetSelections();

   return psfSignal;
}

// ----------------------------------------------------------------------------

ImageWindow ImageIntegrationInstance::CreateImageWindow( const IsoString& id, int bitsPerSample ) const
{
   if ( p_closePreviousImages )
   {
      ImageWindow window = ImageWindow::WindowById( id );
      if ( !window.IsNull() )
         window.Close();
   }

   ImageWindow window( IntegrationFile::Width(), IntegrationFile::Height(), IntegrationFile::NumberOfChannels(),
                       bitsPerSample, true, IntegrationFile::IsColor(), true, id );
   if ( window.IsNull() )
      throw Error( "Unable to create image window: " + id );

   return window;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageIntegrationInstance.cpp - Released 2023-12-01T19:16:18Z
