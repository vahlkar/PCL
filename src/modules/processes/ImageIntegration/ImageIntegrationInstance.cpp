//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// ImageIntegrationInstance.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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
#include <pcl/Settings.h>
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
   , p_evaluateNoise( TheIIEvaluateNoiseParameter->DefaultValue() )
   , p_mrsMinDataFraction( TheIIMRSMinDataFractionParameter->DefaultValue() )
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
      p_evaluateNoise                     = x->p_evaluateNoise;
      p_mrsMinDataFraction                = x->p_mrsMinDataFraction;
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
            TheIntegrationCache->Load();

            if ( TheIntegrationCache->IsEnabled() )
               if ( TheIntegrationCache->IsEmpty() )
                  console.NoteLn( "<end><cbr><br>* Empty file cache" );
               else
                  console.NoteLn( "<end><cbr><br>* Loaded cache: " + String( TheIntegrationCache->NumberOfItems() ) + " item(s)" );
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
         size_type stackSize = size_type( IntegrationFile::Width() ) *
                                 (IntegrationFile::NumberOfFiles()*sizeof( RejectionDataItem )
                                  + sizeof( int )
                                  + ((p_rejection == IIRejection::LinearFit) ? sizeof( float ) : 0));

         size_type stackBufferSize = 0;
         if ( p_autoMemorySize )
         {
            size_type totalAvailable = size_type( double( p_autoMemoryLimit ) * IntegrationFile::AvailableMemory() );
            if ( totalAvailable > IntegrationFile::TotalBufferSize() )
               stackBufferSize = totalAvailable - IntegrationFile::TotalBufferSize();
         }
         else
            stackBufferSize = size_type( p_stackSizeMB )*1024*1024;

         totalStacks = int( Min( Max( size_type( Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 ) ),
                                      stackBufferSize/stackSize ),
                                 size_type( IntegrationFile::BufferRows() ) ) );

         if ( IntegrationFile::AvailableMemory() == 0 )
            console.WarningLn( "<end><cbr><br>** Warning: Unable to estimate the available physical memory." );
         else
            console.NoteLn( String().Format( "<end><cbr><br>* Available physical memory: %.3f GiB",
                                             IntegrationFile::AvailableMemory()/1024/1024/1024.0 ) );

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

      o_output.numberOfChannels = IntegrationFile::NumberOfChannels();
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

         if ( p_evaluateNoise )
         {
            /*
             * Show noise evaluation info.
             */
            console.WriteLn( "<end><cbr>" );

            DVector noise = EvaluateNoise( result );
            DVector snr = EvaluateSNR( result, noise );

            SpinStatus spin;
            result->SetStatusCallback( &spin );
            result->Status().Initialize( "Computing noise scaling factors" );
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
            }

            result->Status().Complete();
            result->Status().EnableInitialization();
            result->SetStatusCallback( nullptr );
            result->ResetSelections();

            console.Write( "<end><cbr>"
                           "<br>Scale estimates           :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               console.Write( String().Format( " (%.6e,%.6e)", 0.991*scale[c].low, 0.991*scale[c].high ) );
               o_output.finalScaleEstimates[c] = 0.991*double( scale[c] );
            }

            console.Write( "<br>Location estimates        :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               console.Write( String().Format( " %.6e", location[c] ) );
               o_output.finalLocationEstimates[c] = location[c];
            }

            console.Write( "<br>Scaled noise estimates    :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               console.Write( String().Format( " %.4e", noise[c]/double( scale[c] ) ) );
               o_output.finalNoiseEstimates[c] = noise[c];
            }

            console.Write( "<br>SNR estimates             :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               console.Write( String().Format( " %.4e", snr[c] ) );
               //o_output.finalSNREstimates[c] = snr[c];
            }

            console.Write( "<br>Reference noise reduction :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               double s = noise[c] * IntegrationFile::FileByIndex( 0 ).BWMV( c ).low/scale[c].low;
               double d = IntegrationFile::FileByIndex( 0 ).NoiseEstimate( c ) / s;
               console.Write( String().Format( " %.4f", d ) );
               o_output.referenceNoiseReductions[c] = d;
            }

            console.Write( "<br>Median noise reduction    :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               DVector m( IntegrationFile::NumberOfFiles() );
               for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
               {
                  double s = noise[c] * IntegrationFile::FileByIndex( i ).BWMV( c ).low/scale[c].low;
                  m[i] = IntegrationFile::FileByIndex( i ).NoiseEstimate( c ) / s;
               }
               double d = m.Median();
               console.Write( String().Format( " %.4f", d ) );
               o_output.medianNoiseReductions[c] = d;
            }

            console.WriteLn();
            console.WriteLn();

         } // if p_evaluateNoise
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

         IsoString totalRejectedLow = IsoString().Format( "ImageIntegration.totalRejectedLow: %lu(%.3f%%)",
                                 o_output.totalRejectedLow[0], 100.0*o_output.totalRejectedLow[0]/o_output.totalPixels );
         IsoString totalRejectedHigh = IsoString().Format( "ImageIntegration.totalRejectedHigh: %lu(%.3f%%)",
                                 o_output.totalRejectedHigh[0], 100.0*o_output.totalRejectedHigh[0]/o_output.totalPixels );
         if ( o_output.numberOfChannels > 1 )
         {
            totalRejectedLow.AppendFormat( " %lu(%.3f%%) %lu(%.3f%%)",
                                 o_output.totalRejectedLow[1], 100.0*o_output.totalRejectedLow[1]/o_output.totalPixels,
                                 o_output.totalRejectedLow[2], 100.0*o_output.totalRejectedLow[2]/o_output.totalPixels );
            totalRejectedHigh.AppendFormat( " %lu(%.3f%%) %lu(%.3f%%)",
                                 o_output.totalRejectedHigh[1], 100.0*o_output.totalRejectedHigh[1]/o_output.totalPixels,
                                 o_output.totalRejectedHigh[2], 100.0*o_output.totalRejectedHigh[2]/o_output.totalPixels );
         }
         keywords << FITSHeaderKeyword( "HISTORY", IsoString(), totalRejectedLow )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), totalRejectedHigh );

         if ( p_evaluateNoise )
         {
            IsoString finalNoiseEstimates = IsoString().Format( "ImageIntegration.finalNoiseEstimates: %.4e",
                                                               o_output.finalNoiseEstimates[0] );
            IsoString finalScaleEstimates = IsoString().Format( "ImageIntegration.finalScaleEstimates: %.6e",
                                                               o_output.finalScaleEstimates[0] );
            IsoString finalLocationEstimates = IsoString().Format( "ImageIntegration.finalLocationEstimates: %.6e",
                                                               o_output.finalLocationEstimates[0] );
            IsoString referenceNoiseReductions = IsoString().Format( "ImageIntegration.referenceNoiseReductions: %.4f",
                                                               o_output.referenceNoiseReductions[0] );
            IsoString medianNoiseReductions = IsoString().Format( "ImageIntegration.medianNoiseReductions: %.4f",
                                                               o_output.medianNoiseReductions[0] );
            if ( o_output.numberOfChannels > 1 )
            {
               finalNoiseEstimates.AppendFormat( " %.4e %.4e", o_output.finalNoiseEstimates[1], o_output.finalNoiseEstimates[2] );
               finalScaleEstimates.AppendFormat( " %.6e %.6e", o_output.finalScaleEstimates[1], o_output.finalScaleEstimates[2] );
               finalLocationEstimates.AppendFormat( " %.6e %.6e", o_output.finalLocationEstimates[1], o_output.finalLocationEstimates[2] );
               referenceNoiseReductions.AppendFormat( " %.4f %.4f", o_output.referenceNoiseReductions[1], o_output.referenceNoiseReductions[2] );
               medianNoiseReductions.AppendFormat( " %.4f %.4f", o_output.medianNoiseReductions[1], o_output.medianNoiseReductions[2] );
            }
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), finalNoiseEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalScaleEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalLocationEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), referenceNoiseReductions )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), medianNoiseReductions );

            for ( int i = 0; i < o_output.numberOfChannels && i < 3; ++i )
               keywords << FITSHeaderKeyword( IsoString().Format( "NOISE%02d", i ),
                                              IsoString().Format( "%.4e", o_output.finalNoiseEstimates[i] ),
                                              IsoString().Format( "Gaussian noise estimate for channel #%d", i ) );
         } // if p_evaluateNoise

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
               IsoString scaleEstimates = IsoString().Format( "ImageIntegration.scaleEstimates_%d: %.6e", i, double( file.ScaleEstimates( 0 ) ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  scaleEstimates.AppendFormat( " %.6e %.6e", double( file.ScaleEstimates( 1 ) ), double( file.ScaleEstimates( 2 ) ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), scaleEstimates );
            }

            if ( file.HasLocationEstimates() )
            {
               IsoString locationEstimates = IsoString().Format( "ImageIntegration.locationEstimates_%d: %.6e", i, file.LocationEstimate( 0 ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  locationEstimates.AppendFormat( " %.6e %.6e", file.LocationEstimate( 1 ), file.LocationEstimate( 2 ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), locationEstimates );
            }

            if ( file.HasNoiseEstimates() )
            {
               IsoString noiseEstimates = IsoString().Format( "ImageIntegration.noiseEstimates_%d: %.4e", i, file.NoiseEstimate( 0 ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  noiseEstimates.AppendFormat( " %.4e %.4e", file.NoiseEstimate( 1 ), file.NoiseEstimate( 2 ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), noiseEstimates );
            }

            if ( file.HasImageWeights() )
            {
               IsoString imageWeights = IsoString().Format( "ImageIntegration.imageWeights_%d: %.5e", i, file.ImageWeight( 0 ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  imageWeights.AppendFormat( " %.5e %.5e", file.ImageWeight( 1 ), file.ImageWeight( 2 ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), imageWeights );
            }

            if ( file.HasScaleFactors() )
            {
               IsoString scaleFactors = IsoString().Format( "ImageIntegration.scaleFactors_%d: %.6e", i, double( file.ScaleFactor( 0 ) ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  scaleFactors.AppendFormat( " %.6e %.6e", double( file.ScaleFactor( 1 ) ), double( file.ScaleFactor( 2 ) ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), scaleFactors );
            }

            if ( file.HasZeroOffsets() )
            {
               IsoString zeroOffsets = IsoString().Format( "ImageIntegration.zeroOffsets_%d: %+.6e", i, file.ZeroOffset( 0 ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  zeroOffsets.AppendFormat( " %+.6e %+.6e", file.ZeroOffset( 1 ), file.ZeroOffset( 2 ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), zeroOffsets );
            }

            IsoString rejectedLow = IsoString().Format( "ImageIntegration.rejectedLow_%d: %lu", i, o_output.imageData[i].rejectedLow[0] );
            if ( IntegrationFile::NumberOfChannels() > 1 )
               rejectedLow.AppendFormat( " %lu %lu", o_output.imageData[i].rejectedLow[1], o_output.imageData[i].rejectedLow[2] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), rejectedLow );

            IsoString rejectedHigh = IsoString().Format( "ImageIntegration.rejectedHigh_%d: %lu", i, o_output.imageData[i].rejectedHigh[0] );
            if ( IntegrationFile::NumberOfChannels() > 1 )
               rejectedHigh.AppendFormat( " %lu %lu", o_output.imageData[i].rejectedHigh[1], o_output.imageData[i].rejectedHigh[2] );
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
            for ( int c = 0; c < IntegrationFile::NumberOfChannels() && c < 3; ++c )
               o_output.imageData[i].weights[c] = IntegrationFile::FileByIndex( i ).Weight( c );
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
                  DrizzleData drz( F.DrizzleDataPath(), true/*ignoreIntegrationData*/ );
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
   if ( p == TheIIWeightScaleParameter )
      return &p_weightScale;
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
   if ( p == TheIIEvaluateNoiseParameter )
      return &p_evaluateNoise;
   if ( p == TheIIMRSMinDataFractionParameter )
      return &p_mrsMinDataFraction;
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
   if ( p == TheIIIntegrationImageIdParameter )
      return o_output.integrationImageId.Length();
   if ( p == TheIILowRejectionMapImageIdParameter )
      return o_output.lowRejectionMapImageId.Length();
   if ( p == TheIIHighRejectionMapImageIdParameter )
      return o_output.highRejectionMapImageId.Length();
   if ( p == TheIISlopeMapImageIdParameter )
      return o_output.slopeMapImageId.Length();
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
DVector ImageIntegrationInstance::EvaluateNoise( const ImageVariant& image ) const
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

   SpinStatus spin;
   image->SetStatusCallback( &spin );
   image->Status().Initialize( "MRS noise evaluation" );
   image->Status().DisableInitialization();

   image->ResetSelections();

   ATrousWaveletTransform::WaveletScalingFunction B3S( SeparableFilter( B3S_hv, B3S_hv, 5 ) );

   DVector noise( image->NumberOfNominalChannels() );

   for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
   {
      Module->ProcessEvents();
      if ( console.AbortRequested() )
         throw ProcessAborted();

      image->SelectChannel( c );

      double s0 = 0;
      for ( int n = 4;; )
      {
         ATrousWaveletTransform W( B3S, n );
         W << image;
         if ( n == 4 )
            s0 = W.NoiseKSigma( 0, image, 0.00002F, 0.99998F, 3 )/B3S_kj[0];

         size_type N;
         noise[c] = W.NoiseMRS( image, B3S_kj, s0, 3, &N, 0.00002F, 0.99998F );

         if ( noise[c] > 0 )
            if ( N >= size_type( p_mrsMinDataFraction * image->NumberOfPixels() ) )
               break;

         if ( --n == 1 )
         {
            console.WarningLn( "<end><cbr>** Warning: No convergence in MRS noise evaluation routine"
                               " - using K-sigma noise estimate." );
            noise[c] = s0;
            break;
         }
      }
   }

   image->Status().Complete();
   image->Status().EnableInitialization();
   image->SetStatusCallback( nullptr );
   image->ResetSelections();

   return noise;
}

// ----------------------------------------------------------------------------

/*
 * Estimation of the signal-to-noise ratio function:
 *
 * SNR(f,g) = E(f^2) / E((f - g)^2)
 *
 * where f is a distribution function, g is a sample, and E(.) is the expected
 * value of the argument. The denominator is the mean square error, which we
 * approximate with the variance of the noise.
 */

template <class P>
double OrderStatistic( const GenericImage<P>& image, int c, distance_type k )
{
   return pcl::OrderStatistic( image[c], image[c] + image.NumberOfPixels(), k );
}

double OrderStatistic( const ImageVariant& image, int c, distance_type k )
{
   switch ( image.BitsPerSample() )
   {
   case 32: return OrderStatistic( static_cast<const Image&>( *image ), c, k ); break;
   case 64: return OrderStatistic( static_cast<const DImage&>( *image ), c, k ); break;
   }
   return 0; // ?!
}

DVector ImageIntegrationInstance::EvaluateSNR( const ImageVariant& image, const DVector& noise ) const
{
   size_type N = image.NumberOfPixels();
   size_type l( 0.1 * N );
   size_type h( 0.2 * N );

   image->PushSelections();

   DVector snr( image->NumberOfNominalChannels() );
   for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
   {
      image->SelectChannel( c );
      image->SetRangeClipping( OrderStatistic( image, c, l ), OrderStatistic( image, c, N-h-1 ) );
      snr[c] = image.MeanOfSquares()/noise[c]/noise[c];
   }

   image->PopSelections();
   return snr;
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
// EOF ImageIntegrationInstance.cpp - Released 2021-04-09T19:41:48Z
