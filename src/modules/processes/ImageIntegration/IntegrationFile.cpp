//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.6
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.5.1
// ----------------------------------------------------------------------------
// IntegrationFile.cpp - Released 2023-07-06T16:53:46Z
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

#include "IntegrationCache.h"
#include "IntegrationFile.h"

#include <pcl/DrizzleData.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileFormat.h>
#include <pcl/MetaModule.h>
#include <pcl/SpinStatus.h>

/*
 * Minimum supported local normalization data version.
 */
#define LN_MIN_VERSION  1

namespace pcl
{

// ----------------------------------------------------------------------------

IntegrationFile::file_list IntegrationFile::s_files;
Rect IntegrationFile::s_roi = 0;
int IntegrationFile::s_width = 0;
int IntegrationFile::s_height = 0;
int IntegrationFile::s_numberOfChannels = 0;
bool IntegrationFile::s_isColor = false;
bool IntegrationFile::s_incremental = false;
size_type IntegrationFile::s_availableMemory = 0;
int IntegrationFile::s_bufferRows = 0;

// ----------------------------------------------------------------------------

void IntegrationFile::UpdateBuffers( int startRow, int channel, int maxThreads )
{
   if ( maxThreads > 1 && s_incremental )
   {
      Array<size_type> L = Thread::OptimalThreadLoads( s_files.Length(), 1, maxThreads );
      ReferenceArray<UpdateBuffersThread> threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads << new UpdateBuffersThread( n, n + int( L[i] ), startRow, channel );

      if ( threads.Length() > 1 )
      {
         for ( auto& thread : threads )
            thread.Start( ThreadPriority::DefaultMax );
         for ( auto& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      threads.Destroy();
   }
   else
   {
      for ( IntegrationFile* file : s_files )
         file->Read( startRow, channel );
   }
}

// ----------------------------------------------------------------------------

void IntegrationFile::CloseAll()
{
   s_files.Destroy();
}

// ----------------------------------------------------------------------------

void IntegrationFile::OpenFiles( const ImageIntegrationInstance& instance )
{
   CloseAll();

   Console console;
   console.WriteLn( "<end><cbr><br>Opening files:<br>" );

   Array<ThreadIndex> pendingItems;
   for ( size_type i = 0, f = 0; i < instance.p_images.Length(); ++i )
      if ( instance.p_images[i].enabled )
         pendingItems << ThreadIndex( i, f++ );
      else
         console.WriteLn( "* Skipping disabled image: " + instance.p_images[i].path );

   if ( pendingItems.IsEmpty() )
      throw Error( "No images have been selected." );

   if ( pendingItems.Length() < 3 )
      throw Error( "ImageIntegration requires at least three images; this instance defines only " +
                     String( pendingItems.Length() ) + " image(s)." );

   if ( instance.p_rejection == IIRejection::LinearFit )
      if ( pendingItems.Length() < 5 )
         throw Error( "Linear fit rejection requires at least five images; only " +
                        String( pendingItems.Length() ) + " images have been selected." );


   for ( size_type i = 0; i < pendingItems.Length(); ++i )
      s_files << new IntegrationFile;

   s_availableMemory = Module->AvailablePhysicalMemory();
   if ( s_availableMemory == 0 )
      console.WarningLn( "<end><cbr><br>** Warning: Unable to estimate the available physical memory." );
   else
      console.NoteLn( String().Format( "<end><cbr><br>* Available physical memory: %.3f GiB", s_availableMemory/1024/1024/1024.0 ) );

   OpenFileThread( *pendingItems, instance, true/*isReference*/ ).Run();
   pendingItems.Remove( pendingItems.Begin() );

   if ( instance.p_useFileThreads )
   {
      size_type worstCaseBytesPerThread =
               sizeof( float ) * size_type( s_bufferRows ) * size_type( s_width ) // pixel buffer
         + 4 * sizeof( float ) * size_type( s_width ) * size_type( s_height );    // noise evaluation
      int maxThreads = Max( 1, TruncInt( double( instance.p_autoMemoryLimit )*s_availableMemory / worstCaseBytesPerThread ) );
      int numberOfThreadsAvailable = RoundInt( Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 ) * instance.p_fileThreadOverload );
      int numberOfThreadsShouldUse = Min( numberOfThreadsAvailable, int( pendingItems.Length() ) );
      int numberOfThreads = Min( maxThreads, numberOfThreadsShouldUse );
      file_thread_list runningThreads( numberOfThreads ); // N.B.: all pointers are set to nullptr by IndirectArray's ctor.
      console.NoteLn( String().Format( "<end><br>* Using %d worker threads%s.",
                                       numberOfThreads,
                                       (numberOfThreads < numberOfThreadsShouldUse) ? " (memory-limited)" : "" ) );
      try
      {
         /*
          * Thread execution loop.
          */
         for ( int count = 0; ; )
         {
            int running = 0;
            for ( file_thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
            {
               Module->ProcessEvents();
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               if ( *i != nullptr )
               {
                  if ( !(*i)->Wait( 150 ) )
                  {
                     ++running;
                     continue;
                  }

                  /*
                   * A thread has just finished.
                   */
                  (*i)->FlushConsoleOutputText();
                  console.WriteLn();
                  String errorInfo;
                  if ( !(*i)->Succeeded() )
                     errorInfo = (*i)->ErrorInfo();

                  /*
                   * N.B.: IndirectArray<>::Delete() sets to nullptr the
                   * pointer pointed to by the iterator, but does not remove
                   * the array element.
                   */
                  runningThreads.Delete( i );

                  if ( !errorInfo.IsEmpty() )
                     throw Error( errorInfo );

                  ++count;
               }

               /*
                * If there are one or more items pending, create a new thread
                * and fire it immediately.
                *
                * Try to keep memory consumption under control. This is
                * necessary on many-core systems with limited memory resources.
                */
               if ( !pendingItems.IsEmpty() )
               {
                  *i = new OpenFileThread( *pendingItems, instance );
                  pendingItems.Remove( pendingItems.Begin() );
                  size_type threadIndex = i - runningThreads.Begin();
                  console.NoteLn( String().Format( "<end><cbr>[%03u] ", threadIndex ) + (*i)->FilePath() );
                  (*i)->Start( ThreadPriority::DefaultMax, threadIndex );
                  ++running;
                  if ( pendingItems.IsEmpty() )
                     console.NoteLn( "<br>* Waiting for running tasks to terminate...<br>" );
                  else if ( count > 0 )
                     console.WriteLn();
               }
            }

            if ( !running )
               break;
         }
      }
      catch ( ... )
      {
         console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate..." );
         for ( OpenFileThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Abort();
         for ( OpenFileThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Wait();
         runningThreads.Destroy();
         throw;
      }
   }
   else // !p_useFileThreads
   {
      for ( auto item : pendingItems )
      {
         console.WriteLn( "<end><cbr><br>" );
         OpenFileThread( item, instance ).Run();
      }
   }

   /*
    * Exclude invalid files
    */
   {
      size_type count = s_files.Length();
      s_files.Destroy( IntegrationFile(),
         []( const IntegrationFile& f, const IntegrationFile& )
         {
            return !f.IsValid();
         } );
      size_type excluded = count - s_files.Length();
      if ( excluded )
      {
         if ( s_files.Length() < 3 )
            throw Error( "Excluded " + String( excluded ) + " invalid file(s) - less than 3 images available for integration, aborting process." );
         console.WarningLn( "<end><cbr>** Warning: Excluded " + String( excluded ) + " invalid file(s) - "
                           + String( s_files.Length() ) + " files available for integration." );
      }
   }

   /*
    * Normalized image weights
    */
   if ( instance.p_weightMode != IIWeightMode::DontCare || !instance.p_csvWeights.IsEmpty() )
      if ( instance.p_generateIntegratedImage && instance.p_combination == IICombination::Average
        || instance.p_generateDrizzleData )
      {
         if ( !instance.p_csvWeights.IsEmpty() )
         {
            StringList tokens;
            instance.p_csvWeights.Break( tokens, ',' );
            if ( tokens.Length() != s_files.Length() * s_numberOfChannels )
               throw Error( String().Format( "Invalid CSV weights list: expected %u items; got %u",
                                             s_files.Length() * s_numberOfChannels, tokens.Length() ) );
            for ( size_type i = 0, k = 0; i < s_files.Length(); ++i )
               for ( int j = 0; j < s_numberOfChannels; ++j, ++k )
                  s_files[i]->m_weights[j] = tokens[k].ToDouble();
         }

         NormalizeImageWeights();

         /*
          * Exclude low-weight images
          */
         if ( instance.p_minWeight > 0 )
         {
            size_type count = s_files.Length();
            s_files.Destroy( IntegrationFile(),
               [&instance]( const IntegrationFile& f, const IntegrationFile& )
               {
                  return f.m_weights.L1Norm() < instance.p_minWeight;
               } );
            size_type excluded = count - s_files.Length();
            if ( excluded )
            {
               if ( s_files.Length() < 3 )
                  throw Error( String().Format( "Excluded %u image(s) with weight < %.8g - "
                                                "less than 3 images available for integration, aborting process.",
                                                excluded, instance.p_minWeight ) );
               console.WarningLn( String().Format( "<end><cbr>** Warning: Excluded %u image(s) with weight < %.8g - "
                                                "%u images available for integration.",
                                                excluded, instance.p_minWeight, s_files.Length() ) );
            }
         }

         console.NoteLn( "<end><cbr>Normalized image weights:" );
         int i = 0;
         for ( const IntegrationFile* file : s_files )
         {
            console.WriteLn( String().Format( "[%5d] <raw>", ++i ) + file->Path() + "</raw>" );
            String s = String().Format( "%8.6f", file->Weight( 0 ) );
            for ( int i = 1; i < s_numberOfChannels; ++i )
               s.AppendFormat( " %8.6f", file->Weight( i ) );
            console.WriteLn( s );
         }
      }
}

// ----------------------------------------------------------------------------

void IntegrationFile::Open( const String& path, const String& nmlPath, const String& drzPath,
                            const ImageIntegrationInstance& instance, bool isReference )
{
   Console console;
   console.WriteLn( "<end><cbr><raw>" + path + "</raw>" );

   m_instance = &instance;

   if ( !nmlPath.IsEmpty() )
   {
      console.WriteLn( "<raw>" + nmlPath + "</raw>" );
      if ( instance.p_normalization != IINormalization::LocalNormalization )
         if ( instance.p_rejectionNormalization != IIRejectionNormalization::LocalRejectionNormalization )
            console.WarningLn( "** Warning: Local normalization data will not be used." );
   }

   if ( !drzPath.IsEmpty() )
   {
      console.WriteLn( "<raw>" + drzPath + "</raw>" );
      if ( !instance.p_generateDrizzleData )
         console.WarningLn( "** Warning: No drizzle integration data will be generated." );
   }

   FileFormat format( File::ExtractExtension( path ), true/*read*/, false/*write*/ );

   m_file = new FileFormatInstance( format );

   ImageDescriptionArray images;

   if ( !m_file->Open( images, path, instance.p_inputHints ) )
      throw CaughtException();

   if ( images.IsEmpty() )
      throw Error( m_file->FilePath() + ": Empty image file." );

   if ( images.Length() > 1 )
      console.NoteLn( String().Format( "<end><cbr>* Ignoring %u additional image(s) in input file.", images.Length()-1 ) );

   if ( !images[0].info.supported || images[0].info.NumberOfSamples() == 0 )
      throw Error( m_file->FilePath() + ": Invalid or unsupported image." );

   if ( isReference )
   {
      s_width = images[0].info.width;
      s_height = images[0].info.height;

      if ( instance.p_useROI )
      {
         s_roi = Rect( s_width, s_height ).Intersection( instance.p_roi );
         if ( !s_roi.IsRect() )
            throw Error( "Invalid ROI coordinates" );
      }
      else
         s_roi = Rect( s_width, s_height );

      s_numberOfChannels = images[0].info.numberOfChannels;
      s_isColor = images[0].info.colorSpace != ColorSpace::Gray;

      s_incremental = false;
      if ( !format.CanReadIncrementally() )
      {
         console.WarningLn( "<end><cbr><br>** Warning: Incremental file reads disabled because of lack of file format support: "
                            + format.Name() + "<br>" );
      }
      else if ( !m_file->CanReadIncrementally() )
      {
         console.WarningLn( "<end><cbr><br>** Warning: Incremental file reads are supported by the selected format, "
                            "but have been disabled because they cannot be performed on this file: "
                            "<raw>" + m_file->FilePath() + "</raw><br>" );
      }
      else
      {
         s_incremental = true;
         size_type bufferSize = 0;
         if ( instance.p_autoMemorySize )
            bufferSize = size_type( double( instance.p_autoMemoryLimit ) * s_availableMemory / s_files.Length() );
         if ( bufferSize == 0 )
            bufferSize = size_type( instance.p_bufferSizeMB )*1024*1024;
         s_bufferRows = Range( int( bufferSize/(s_width * sizeof( float )) ), 1, s_roi.Height() );
      }

      if ( !s_incremental )
         s_bufferRows = s_roi.Height();
   }
   else
   {
      if ( s_incremental )
         if ( !format.CanReadIncrementally() )
            throw Error( "Invalid combination of file formats with and without incremental file read capabilities: " + format.Name() );
         else if ( !m_file->CanReadIncrementally() )
            throw Error( "Incremental file reads have been enabled, but they cannot be performed on this file: "
                         "<raw>" + m_file->FilePath() + "</raw>" );

      if ( s_width != images[0].info.width ||
           s_height != images[0].info.height ||
           s_numberOfChannels != images[0].info.numberOfChannels )
         throw Error( m_file->FilePath() + ": Incompatible image geometry." );
   }

   m_nmlPath               = String();
   m_drzPath               = String();
   m_rejectionMap          = UInt8Image();
   m_mean                  = DVector();
   m_median                = DVector();
   m_avgDev                = scale_estimates();
   m_mad                   = scale_estimates();
   m_bwmv                  = scale_estimates();
   m_locationEstimates     = DVector();
   m_scaleEstimates        = scale_estimates();
   m_noiseEstimates        = DVector();
   m_noiseScaleEstimates   = scale_estimates();
   m_adaptiveNormalization = AdaptiveNormalizationData();
   m_keywords              = FITSKeywordArray();
   m_metadata              = IntegrationMetadata();

   if ( instance.p_useCache )
   {
      int n = GetFromCache( path );
      if ( n > 0 )
         console.NoteLn( String().Format( "<end><cbr>* Retrieved %d data item(s) from file cache.", n ) );
   }

   if ( !m_metadata.IsValid() )
   {
      PropertyArray properties;
      if ( format.CanStoreImageProperties() )
         properties = m_file->ReadImageProperties();

      if ( format.CanStoreKeywords() )
         if ( !m_file->ReadFITSKeywords( m_keywords ) )
            throw CaughtException();

      m_metadata = IntegrationMetadata( properties, m_keywords );
   }

   if ( s_incremental )
      m_buffer = FMatrix( s_bufferRows, s_width );
   else
   {
      m_image = new Image( (void*)0, 0, 0 ); // shared image
      if ( !m_file->ReadImage( *m_image ) )
         throw CaughtException();
   }

   bool generateOutput = instance.p_generateIntegratedImage || instance.p_generateDrizzleData;
   bool evaluateSNR = instance.p_generateIntegratedImage && instance.p_evaluateSNR;

   bool hasWeightsList = !instance.p_csvWeights.IsEmpty();

   bool needNoise =               instance.p_generateIntegratedImage &&
                                 (instance.p_evaluateSNR ||
                                  instance.p_combination == IICombination::Average &&
                                  !hasWeightsList &&
                                  (instance.p_weightMode == IIWeightMode::SNREstimate ||
                                   instance.p_weightMode == IIWeightMode::PSFSignalWeight ||
                                   instance.p_weightMode == IIWeightMode::PSFSNR ||
                                   instance.p_weightMode == IIWeightMode::PSFScaleSNR))
                               || instance.p_generateDrizzleData &&
                                  !hasWeightsList &&
                                  (instance.p_weightMode == IIWeightMode::SNREstimate ||
                                   instance.p_weightMode == IIWeightMode::PSFSignalWeight ||
                                   instance.p_weightMode == IIWeightMode::PSFSNR ||
                                   instance.p_weightMode == IIWeightMode::PSFScaleSNR);

   bool needScale = needNoise  || instance.p_generateDrizzleData
                               || instance.p_generateIntegratedImage &&
                                 (instance.p_normalization == IINormalization::AdditiveWithScaling ||
                                  instance.p_normalization == IINormalization::MultiplicativeWithScaling)
                               || instance.p_rejection != IIRejection::NoRejection &&
                                  instance.p_rejectionNormalization == IIRejectionNormalization::Scale;

   bool needMedian = needScale || generateOutput &&
                                 (instance.p_normalization != IINormalization::NoNormalization ||
                                  !hasWeightsList &&
                                  (instance.p_weightMode == IIWeightMode::SignalWeight ||
                                   instance.p_weightMode == IIWeightMode::MedianWeight))
                               || instance.p_rejection != IIRejection::NoRejection &&
                                  instance.p_rejectionNormalization == IIRejectionNormalization::EqualizeFluxes;

   bool needAvgDev =              instance.p_weightScale == IIWeightScale::AvgDev
                               || generateOutput &&
                                  !hasWeightsList &&
                                  instance.p_weightMode == IIWeightMode::SignalWeight;

   bool needMean =                generateOutput &&
                                  !hasWeightsList &&
                                  instance.p_weightMode == IIWeightMode::AverageWeight;

   bool needAdaptive =            instance.p_generateIntegratedImage &&
                                  instance.p_normalization == IINormalization::AdaptiveNormalization
                               || instance.p_rejection != IIRejection::NoRejection &&
                                  instance.p_rejectionNormalization == IIRejectionNormalization::AdaptiveRejectionNormalization;

   bool doMean     = m_mean.IsEmpty()           && needMean;
   bool doAvgDev   = m_avgDev.IsEmpty()         && needAvgDev;
   bool doBWMV     = m_bwmv.IsEmpty()           && (instance.p_weightScale == IIWeightScale::BWMV || evaluateSNR);
   bool doMAD      = m_mad.IsEmpty()            && (instance.p_weightScale == IIWeightScale::MAD || doBWMV);
   bool doMedian   = m_median.IsEmpty()         && (needMedian || doAvgDev || doMAD || doBWMV) ;
   bool doNoise    = (m_noiseEstimates.IsEmpty() ||
                      m_noiseScaleEstimates.IsEmpty() ||
                      m_psfSignalEstimates.IsEmpty()) && needNoise;
   bool doAdaptive = needAdaptive && !m_adaptiveNormalization.IsValid();

   SpinStatus spin;

   if ( doMean || doMedian || doAvgDev || doMAD || doBWMV || doNoise || doAdaptive )
   {
      if ( s_incremental )
      {
         m_image = new Image( (void*)0, 0, 0 ); // shared image
         if ( !m_file->ReadImage( *m_image ) )
            throw CaughtException();
      }

      m_image->SetStatusCallback( &spin );
      m_image->Status().Initialize( "<end><cbr>Computing image statistics" );
      m_image->Status().DisableInitialization();
   }

   if ( doMean || doMedian || doAvgDev || doMAD || doBWMV )
   {
      if ( doMean )
         m_mean   = DVector( s_numberOfChannels );
      if ( doMedian )
         m_median = DVector( s_numberOfChannels );
      if ( doAvgDev )
         m_avgDev = scale_estimates( s_numberOfChannels );
      if ( doMAD )
         m_mad    = scale_estimates( s_numberOfChannels );
      if ( doBWMV )
         m_bwmv   = scale_estimates( s_numberOfChannels );

      m_image->ResetSelections();
      m_image->SetRangeClipping( 1.0/65535, 1 - 1.0/65535 );

      for ( int c = 0; c < s_numberOfChannels; ++c )
      {
         m_image->SelectChannel( c );
         if ( doMean )
            m_mean[c] = m_image->Mean();
         if ( doMedian )
            m_median[c] = m_image->Median();
         if ( doAvgDev )
            m_avgDev[c] = m_image->TwoSidedAvgDev( m_median[c] );
         if ( doMAD )
         {
            m_mad[c] = m_image->TwoSidedMAD( m_median[c] );
            if ( 1 + m_mad[c].low == 1 || 1 + m_mad[c].high == 1 )
               m_mad[c].low = m_mad[c].high = m_image->MAD( m_median[c] );
         }
         if ( doBWMV )
            m_bwmv[c] = Sqrt( m_image->TwoSidedBiweightMidvariance( m_median[c], m_mad[c] ) );

         if ( instance.p_subtractPedestals )
            if ( m_metadata.IsValid() )
               if ( m_metadata.pedestal.IsDefined() )
               {
                  if ( doMean )
                     m_mean[c] -= m_metadata.pedestal()/65535.0;
                  if ( doMedian )
                     m_median[c] -= m_metadata.pedestal()/65535.0;
               }
      }

      m_image->ResetSelections();
   }

   if ( doAdaptive )
   {
      try
      {
         m_adaptiveNormalization = AdaptiveNormalizationData( *m_image, instance.p_weightScale, instance.p_adaptiveGridSize );
      }
      catch ( Error& x )
      {
         throw Error( path + ": " + x.Message() );
      }
      catch ( ... )
      {
         throw;
      }
   }

   if ( m_image.IsValid() )
   {
      m_image->Status().Complete();
      m_image->SetStatusCallback( nullptr );
   }

   m_locationEstimates = m_median;

   if ( needScale )
   {
      switch ( instance.p_weightScale )
      {
      case IIWeightScale::AvgDev: m_scaleEstimates = m_avgDev; break;
      case IIWeightScale::MAD:    m_scaleEstimates = m_mad; break;
      default:
      case IIWeightScale::BWMV:   m_scaleEstimates = m_bwmv; break;
      }
      for ( int c = 0; c < s_numberOfChannels; ++c )
         if ( !m_scaleEstimates[c].IsValid() )
            throw Error( path + ": Zero or insignificant signal detected (channel " + String( c ) + ")" );
   }

   if ( doNoise )
   {
      m_psfSignalEstimates = signal_estimates( s_numberOfChannels );
      m_noiseEstimates = DVector( s_numberOfChannels );
      m_noiseScaleEstimates = scale_estimates( s_numberOfChannels );
      bool signalOk = false, noiseOk = false, noiseScaleOk = false;

      if ( !instance.p_ignoreNoiseKeywords )
      {
         if ( format.CanStoreKeywords() )
         {
            int c0 = 0;
            if ( format.CanStoreImageProperties() )
               if ( m_file->HasImageProperty( "PCL:CFASourceChannel" ) )
               {
                  Variant v = m_file->ReadImageProperty( "PCL:CFASourceChannel" );
                  if ( v.IsValid() )
                     c0 = v.ToInt();
               }

            signalOk = true;
            for ( int c = 0; c < s_numberOfChannels; ++c )
            {
               m_psfSignalEstimates[c].totalFlux = KeywordValue( IsoString().Format( "PSFFLX%02d", c0 + c ) );
               if ( m_psfSignalEstimates[c].totalFlux <= 0 )
               {
                  m_psfSignalEstimates[c].totalFlux = KeywordValue( "PSFFLX" );
                  if ( m_psfSignalEstimates[c].totalFlux <= 0 )
                  {
                     signalOk = false;
                     break;
                  }
               }
               m_psfSignalEstimates[c].totalPowerFlux = KeywordValue( IsoString().Format( "PSFFLP%02d", c0 + c ) );
               if ( m_psfSignalEstimates[c].totalPowerFlux <= 0 )
               {
                  m_psfSignalEstimates[c].totalPowerFlux = KeywordValue( "PSFFLP" );
                  if ( m_psfSignalEstimates[c].totalPowerFlux <= 0 )
                  {
                     signalOk = false;
                     break;
                  }
               }

               m_psfSignalEstimates[c].totalMeanFlux = KeywordValue( IsoString().Format( "PSFMFL%02d", c0 + c ) );
               if ( m_psfSignalEstimates[c].totalMeanFlux <= 0 )
               {
                  m_psfSignalEstimates[c].totalMeanFlux = KeywordValue( "PSFMFL" );
                  if ( m_psfSignalEstimates[c].totalMeanFlux <= 0 )
                  {
                     signalOk = false;
                     break;
                  }
               }
               m_psfSignalEstimates[c].totalMeanPowerFlux = KeywordValue( IsoString().Format( "PSFMFP%02d", c0 + c ) );
               if ( m_psfSignalEstimates[c].totalMeanPowerFlux <= 0 )
               {
                  m_psfSignalEstimates[c].totalMeanPowerFlux = KeywordValue( "PSFMFP" );
                  if ( m_psfSignalEstimates[c].totalMeanPowerFlux <= 0 )
                  {
                     signalOk = false;
                     break;
                  }
               }

               m_psfSignalEstimates[c].MStar = KeywordValue( IsoString().Format( "PSFMST%02d", c0 + c ) );
               if ( m_psfSignalEstimates[c].MStar <= 0 )
               {
                  m_psfSignalEstimates[c].MStar = KeywordValue( "PSFMST" );
                  if ( m_psfSignalEstimates[c].MStar <= 0 )
                  {
                     signalOk = false;
                     break;
                  }
               }
               m_psfSignalEstimates[c].NStar = KeywordValue( IsoString().Format( "PSFNST%02d", c0 + c ) );
               if ( m_psfSignalEstimates[c].NStar <= 0 )
               {
                  m_psfSignalEstimates[c].NStar = KeywordValue( "PSFNST" );
                  if ( m_psfSignalEstimates[c].NStar <= 0 )
                  {
                     signalOk = false;
                     break;
                  }
               }

               m_psfSignalEstimates[c].count = int( KeywordValue( IsoString().Format( "PSFSGN%02d", c0 + c ) ) );
               if ( m_psfSignalEstimates[c].count <= 0 )
               {
                  m_psfSignalEstimates[c].count = int( KeywordValue( "PSFSGN" ) );
                  if ( m_psfSignalEstimates[c].count <= 0 )
                  {
                     signalOk = false;
                     break;
                  }
               }
            }

            noiseOk = true;
            for ( int c = 0; c < s_numberOfChannels; ++c )
            {
               m_noiseEstimates[c] = KeywordValue( IsoString().Format( "NOISE%02d", c0 + c ) );
               if ( m_noiseEstimates[c] <= 0 )
               {
                  m_noiseEstimates[c] = KeywordValue( "NOISE" );
                  if ( m_noiseEstimates[c] <= 0 )
                  {
                     noiseOk = false;
                     break;
                  }
               }
            }

            if ( noiseOk )
               if ( !noiseScaleOk )
               {
                  noiseScaleOk = true;
                  for ( int c = 0; c < s_numberOfChannels; ++c )
                  {
                     m_noiseScaleEstimates[c].low = KeywordValue( IsoString().Format( "NOISEL%02d", c0 + c ) );
                     if ( m_noiseScaleEstimates[c].low <= 0 )
                     {
                        m_noiseScaleEstimates[c].low = KeywordValue( "NOISEL" );
                        if ( m_noiseScaleEstimates[c].low <= 0 )
                        {
                           noiseScaleOk = false;
                           break;
                        }
                     }
                  }

                  if ( noiseScaleOk )
                     for ( int c = 0; c < s_numberOfChannels; ++c )
                     {
                        m_noiseScaleEstimates[c].high = KeywordValue( IsoString().Format( "NOISEH%02d", c0 + c ) );
                        if ( m_noiseScaleEstimates[c].high <= 0 )
                        {
                           m_noiseScaleEstimates[c].high = KeywordValue( "NOISEH" );
                           if ( m_noiseScaleEstimates[c].high <= 0 )
                           {
                              noiseScaleOk = false;
                              break;
                           }
                        }
                     }
               }
         }
      } // if ( !instance.p_ignoreNoiseKeywords )

      if ( !signalOk )
      {
         console.WarningLn( "<end><cbr>** Warning: PSF signal estimates are being calculated from non-raw data. "
                            "Image weights can be inaccurate." );
         m_psfSignalEstimates = instance.EvaluatePSFSignal( ImageVariant( m_image.Ptr() ) );
      }

      if ( !noiseOk )
      {
         console.WarningLn( "<end><cbr>** Warning: Noise estimates and scaling factors are being calculated from non-raw data. "
                            "Image weights can be inaccurate." );
         m_noiseEstimates = instance.EvaluateNoise( ImageVariant( m_image.Ptr() ) );
         m_noiseScaleEstimates = instance.EvaluateNoiseScale( ImageVariant( m_image.Ptr() ) );
      }
      else if ( !noiseScaleOk )
      {
         console.WarningLn( "<end><cbr>** Warning: Noise scaling factors are being calculated from non-raw data. "
                            "Image weights can be inaccurate." );
         m_noiseScaleEstimates = instance.EvaluateNoiseScale( ImageVariant( m_image.Ptr() ) );
      }
   } // if doNoise

   if ( s_incremental )
      m_image.Destroy();
   else
   {
      m_image->ResetSelections();

      if ( instance.p_subtractPedestals )
         if ( m_metadata.IsValid() )
            if ( m_metadata.pedestal.IsDefined() )
               *m_image -= m_metadata.pedestal()/65535.0;
   }

   if ( instance.p_useCache )
      AddToCache( path );

   if ( needNoise )
   {
      console.WriteLn( "<end><cbr>PSF signal estimates:" );
      for ( int c = 0; c < s_numberOfChannels; ++c )
         if ( m_psfSignalEstimates[c].count > 0 )
            console.WriteLn( String().Format( "ch %d : TFlux = %.4e, TMeanFlux = %.4e, M* = %.4e, N* = %.4e, %d PSF fits",
                                       c, m_psfSignalEstimates[c].totalFlux, m_psfSignalEstimates[c].totalMeanFlux,
                                       m_psfSignalEstimates[c].MStar, m_psfSignalEstimates[c].NStar, m_psfSignalEstimates[c].count ) );
            else
               console.WarningLn( String().Format( "** Warning: No valid PSF signal samples (channel %d).", c ) );

      console.WriteLn( "Noise estimates:" );
      for ( int c = 0; c < m_noiseEstimates.Length(); ++c )
         console.WriteLn( String().Format( "ch %d : sigma_n = %.4e", c, m_noiseEstimates[c] ) );

      console.WriteLn( "Noise scaling factors:" );
      for ( int c = 0; c < m_noiseEstimates.Length(); ++c )
         console.WriteLn( String().Format( "ch %d : sigma_low = %.6e, sigma_high = %.6e",
                                       c, m_noiseScaleEstimates[c].low, m_noiseScaleEstimates[c].high ) );
   }

   if ( needScale )
   {
      console.WriteLn( "<end><cbr>Relative location and scale:" );
      for ( int c = 0; c < s_numberOfChannels; ++c )
         console.WriteLn( String().Format( "ch %d : zero_offset = %+.6e, scale_low = %.6e, scale_high = %.6e",
                                       c, ZeroOffset( c ), ScaleFactor( c ).low, ScaleFactor( c ).high ) );
   }

   if ( instance.p_generateIntegratedImage
     && instance.p_normalization == IINormalization::LocalNormalization ||
        instance.p_rejection != IIRejection::NoRejection
     && instance.p_rejectionNormalization == IIRejectionNormalization::LocalRejectionNormalization ||
        generateOutput
     && !hasWeightsList
     && (instance.p_generateDrizzleData || instance.p_combination == IICombination::Average)
     && instance.p_weightMode == IIWeightMode::PSFScaleSNR )
   {
      m_nmlPath = nmlPath.Trimmed();
      if ( m_nmlPath.IsEmpty() )
      {
         console.WarningLn( "<end><cbr>** Warning: Missing required local normalization data - file will be excluded from the integration." );
         m_valid = false;
         return;
      }

      if ( !File::Exists( m_nmlPath ) )
         throw Error( "No such file: " + m_nmlPath );

      m_localNormalization.Parse( m_nmlPath );

      if ( m_localNormalization.IsTaggedAsInvalid() )
      {
         console.WarningLn( "<end><cbr>** Warning: Invalid local normalization data - file will be excluded from the integration." );
         m_valid = false;
         return;
      }

      if ( m_localNormalization.Version() < LN_MIN_VERSION )
         throw Error( String().Format( "Incompatible local normalization data version. Expected >= %d, got %d: ",
                                       LN_MIN_VERSION, m_localNormalization.Version() ) + m_nmlPath );

      if ( m_localNormalization.ReferenceWidth() != s_width ||
           m_localNormalization.ReferenceHeight() != s_height ||
           m_localNormalization.NumberOfChannels() != s_numberOfChannels )
         throw Error( "Inconsistent image geometry: " + m_nmlPath );
   }

   m_hasLocalNormalization = m_localNormalization.HasInterpolations();
   if ( !m_hasLocalNormalization )
      if ( instance.p_generateIntegratedImage
        && instance.p_normalization == IINormalization::LocalNormalization ||
           instance.p_rejection != IIRejection::NoRejection
        && instance.p_rejectionNormalization == IIRejectionNormalization::LocalRejectionNormalization )
      {
         throw Error( "Required local normalization data not available: " + m_nmlPath );
      }

   if ( instance.p_generateDrizzleData )
   {
      m_drzPath = drzPath.Trimmed();
      if ( m_drzPath.IsEmpty() )
         throw Error( m_file->FilePath() + ": Missing drizzle data file path." );
      if ( !File::Exists( m_drzPath ) )
         throw Error( "No such file: " + m_drzPath );
   }

   if ( generateOutput )
   {
      m_weights = DVector( s_numberOfChannels );

      /*
       * Image weighting only makes sense for average combination. It is also
       * required for generation of drizzle integration data files.
       */
      if ( instance.p_weightMode != IIWeightMode::DontCare
        && !hasWeightsList
        && (instance.p_generateDrizzleData || instance.p_combination == IICombination::Average) )
      {
         switch ( instance.p_weightMode )
         {
         default:
         case IIWeightMode::PSFSignalWeight:
            for ( int c = 0; c < s_numberOfChannels; ++c )
            {
               /*
                * PSF Signal Weight (PSFSW) image quality estimator
                */
               double w_psf = PSFSignalEstimator::PSFSignalWeight( PSFSignalEstimate( c ), NoiseEstimate( c ) );
               if ( !IsFinite( w_psf ) || 1 + w_psf == 1 )
                  throw Error( m_file->FilePath() + String().Format( " (channel #%d): Zero or insignificant PSF Signal Weight estimate.", c ) );
               m_weights[c] = w_psf;
            }
            break;
         case IIWeightMode::PSFSNR:
            for ( int c = 0; c < s_numberOfChannels; ++c )
            {
               /*
                * PSF Signal to Noise Ratio (PSFSNR) estimator
                */
               double snr_psf = PSFSignalEstimator::PSFSNR( PSFSignalEstimate( c ), NoiseEstimate( c ) );
               if ( !IsFinite( snr_psf ) || 1 + snr_psf == 1 )
                  throw Error( m_file->FilePath() + String().Format( " (channel #%d): Zero or insignificant PSF SNR ratio estimate.", c ) );
               m_weights[c] = snr_psf;
            }
            break;
         case IIWeightMode::PSFScaleSNR:
            if ( !HasLocalNormalization() )
               throw Error( m_file->FilePath() + ": The PSF Scale SNR weighting method requires local normalization data." );
            {
               const Vector& s = LocalNormalization().RelativeScaleFactors();
               if ( s.IsEmpty() )
                  throw Error( LocalNormalizationDataPath() + ": The local normalization data file does not provide relative scale factors, "
                                    "which are required by the PSF Scale SNR weighting method." );
               if ( s.Length() < s_numberOfChannels )
                  throw Error( LocalNormalizationDataPath() + ": The local normalization data file does not provide enough relative scale factors, "
                                    "which are required by the PSF Scale SNR weighting method." );
               for ( int c = 0; c < s_numberOfChannels; ++c )
               {
                  /*
                   * PSF Scale SNR estimator
                   */
                  double snr_scale = 1/s[c]/s[c]/NoiseEstimate( c )/NoiseEstimate( c );
                  if ( !IsFinite( snr_scale ) || 1 + snr_scale == 1 )
                     throw Error( m_file->FilePath() + String().Format( " (channel #%d): Zero or insignificant PSF Scale SNR ratio estimate.", c ) );
                  m_weights[c] = snr_scale;
               }
            }
            break;
         case IIWeightMode::SNREstimate:
            for ( int c = 0; c < s_numberOfChannels; ++c )
            {
               /*
                * Weighting by scaled inverse noise variance
                */
               double w_snr = double( NoiseScaleEstimates( c ) )/NoiseEstimate( c );
               if ( !IsFinite( w_snr ) || 1 + w_snr*w_snr == 1 )
                  throw Error( m_file->FilePath() + String().Format( " (channel #%d): Zero or insignificant scaled noise estimate.", c ) );
               m_weights[c] = w_snr*w_snr;
            }
            break;
         case IIWeightMode::ExposureTimeWeight:
            m_weights[0] = images[0].options.exposure;
            if ( m_weights[0] <= 0 )
               if ( format.CanStoreKeywords() )
               {
                  m_weights[0] = KeywordValue( "EXPTIME" );
                  if ( m_weights[0] <= 0 )
                     m_weights[0] = KeywordValue( "EXPOSURE" );
               }
            if ( m_weights[0] <= 0 )
               throw Error( m_file->FilePath() + ": No way to know the exposure time." );
            m_weights = m_weights[0];
            break;
         case IIWeightMode::KeywordWeight:
            if ( !format.CanStoreKeywords() )
               throw Error( m_file->FilePath() + ": The " + format.Name() + " format cannot store keywords." );
            m_weights[0] = KeywordValue( instance.p_weightKeyword );
            if ( m_weights[0] <= 0 )
               throw Error( m_file->FilePath() + ": Nonexistent or invalid weight keyword '" + instance.p_weightKeyword + "'." );
            m_weights = m_weights[0];
            break;
         case IIWeightMode::SignalWeight:
            for ( int c = 0; c < s_numberOfChannels; ++c )
               m_weights[c] = double( m_avgDev[c] );
            break;
         case IIWeightMode::MedianWeight:
            for ( int c = 0; c < s_numberOfChannels; ++c )
               m_weights[c] = m_median[c];
            break;
         case IIWeightMode::AverageWeight:
            for ( int c = 0; c < s_numberOfChannels; ++c )
               m_weights[c] = m_mean[c];
            break;
         }

         for ( int c = 0; c < s_numberOfChannels; ++c )
            if ( !IsFinite( m_weights[c] ) || 1 + m_weights[c] == 1 )
               throw Error( m_file->FilePath() + ": Zero or insignificant signal detected (empty image?)." );
         console.WriteLn( "<end><cbr>Relative weights:" );
         for ( int c = 0; c < s_numberOfChannels; ++c )
            console.WriteLn( String().Format( "ch %d : weight = %.5e", c, ImageWeight( c ) ) );
      }
      else
      {
         /*
          * Set all weights equal to one for median, minimum and maximum
          * combinations, or when 'don't care' weighting has been selected.
          */
         m_weights = 1.0;
      }
   } // if ( generateOutput )

   if ( !s_incremental )
      if ( s_roi != m_image->Bounds() )
         m_image->CropTo( s_roi );
}

// ----------------------------------------------------------------------------

void IntegrationFile::Read( int startRow, int channel )
{
   if ( s_incremental )
   {
      startRow += s_roi.y0;
      int totalRows = Min( s_bufferRows, s_roi.y1 - startRow );
      if ( !m_file->ReadSamples( *m_buffer, startRow, totalRows, channel ) )
         throw CaughtException();

      if ( m_instance->p_subtractPedestals )
         if ( m_metadata.IsValid() )
            if ( m_metadata.pedestal.IsDefined() )
            {
               double p = m_metadata.pedestal()/65535.0;
               for ( int i = 0; i < totalRows; ++i )
                  for ( int j = 0; j < s_width; ++j )
                     m_buffer[i][j] -= p;
            }
   }
   else
      m_currentChannel = channel;
}

// ----------------------------------------------------------------------------

void IntegrationFile::ToDrizzleData( DrizzleData& drz ) const
{
   if ( m_metadata.IsValid() )
   {
      IntegrationMetadata metadata( m_metadata );
      if ( m_instance->p_subtractPedestals )
         if ( metadata.pedestal.IsDefined() )
         {
            drz.SetPedestal( metadata.pedestal()/65535.0 );
            metadata.pedestal.Undefine();
         }
      drz.SetMetadata( metadata.Serialize() );
   }

   drz.SetLocation( m_locationEstimates );
   drz.SetReferenceLocation( s_files[0]->m_locationEstimates );
   drz.SetScale( ScalarScaleFactors() );
   drz.SetWeight( m_weights/s_files[0]->m_weights );

   if ( !m_rejectionMap.IsEmpty() )
      drz.SetRejectionMap( m_rejectionMap );

   if ( m_adaptiveNormalization.IsValid() )
   {
      DrizzleData::point_list points;
      for ( int i = 0; i < m_adaptiveNormalization.m_x.Length(); ++i )
         points << DPoint( m_adaptiveNormalization.m_x[i], m_adaptiveNormalization.m_y[i] );
      drz.SetAdaptiveNormalizationCoordinates( points );

      drz.SetAdaptiveNormalizationLocationVectors( m_adaptiveNormalization.m_m );

      DMultiVector s0, s1;
      for ( int i = 0; i < s_numberOfChannels; ++i )
      {
         s0 << s_files[0]->m_adaptiveNormalization.m_s0[i]/m_adaptiveNormalization.m_s0[i];
         s1 << s_files[0]->m_adaptiveNormalization.m_s1[i]/m_adaptiveNormalization.m_s1[i];
      }
      drz.SetAdaptiveNormalizationScaleVectors( s0, s1 );

      DMultiVector z0, z1;
      for ( int i = 0; i < s_numberOfChannels; ++i )
      {
         /*
          * ### N.B. We need component wise vector multiplication here. If we
          * use the following:
          *
          * z0 << s_files[0]->m_adaptiveNormalization.m_m[i] - m_adaptiveNormalization.m_m[i] * s0[i];
          * z1 << s_files[0]->m_adaptiveNormalization.m_m[i] - m_adaptiveNormalization.m_m[i] * s1[i];
          *
          * we'd be applying dot vector multiplication, which would yield
          * completely wrong results. See the definition of:
          *
          * T operator *( const GenericVector<T>&, const GenericVector<T>& )
          */
         s0[i] *= m_adaptiveNormalization.m_m[i];
         s1[i] *= m_adaptiveNormalization.m_m[i];
         z0 << s_files[0]->m_adaptiveNormalization.m_m[i] - s0[i];
         z1 << s_files[0]->m_adaptiveNormalization.m_m[i] - s1[i];
      }
      drz.SetAdaptiveNormalizationZeroOffsetVectors( z0, z1 );
   }
}

// ----------------------------------------------------------------------------

void IntegrationFile::NormalizeImageWeights()
{
   for ( int i = 0; i < s_numberOfChannels; ++i )
   {
      double maxWeight = 0;
      for ( const IntegrationFile* file : s_files )
         if ( file->m_weights[i] > maxWeight )
            maxWeight = file->m_weights[i];
      for ( IntegrationFile* file : s_files )
         file->m_weights[i] /= maxWeight;
   }
}

// ----------------------------------------------------------------------------

IntegrationMetadata IntegrationFile::SummaryMetadata()
{
   Array<IntegrationMetadata> items;
   for ( const IntegrationFile* file : s_files )
      items << file->Metadata();
   return IntegrationMetadata::Summary( items );
}

// ----------------------------------------------------------------------------

double IntegrationFile::KeywordValue( const IsoString& keyName )
{
   if ( m_keywords.IsEmpty() )
      if ( m_file->Format().CanStoreKeywords() )
         if ( !m_file->ReadFITSKeywords( m_keywords ) )
            throw CaughtException();

   for ( const FITSHeaderKeyword& keyword : m_keywords )
      if ( !keyword.name.CompareIC( keyName ) )
      {
         if ( keyword.IsNumeric() )
         {
            double v;
            if ( keyword.GetNumericValue( v ) )
               return v;
         }
         break;
      }

   return -1;
}

// ----------------------------------------------------------------------------

void IntegrationFile::ResetCacheableData()
{
   m_mean                  = DVector();
   m_median                = DVector();
   m_avgDev                = scale_estimates();
   m_mad                   = scale_estimates();
   m_bwmv                  = scale_estimates();
   m_noiseEstimates        = DVector();
   m_noiseScaleEstimates   = scale_estimates();
   m_adaptiveNormalization = AdaptiveNormalizationData();
   m_metadata              = IntegrationMetadata();
}

// ----------------------------------------------------------------------------

void IntegrationFile::AddToCache( const String& path ) const
{
   if ( TheIntegrationCache != nullptr )
   {
      IntegrationCacheItem item( path );

      item.mean       = m_mean;
      item.median     = m_median;
      item.avgDev     = m_avgDev;
      item.mad        = m_mad;
      item.bwmv       = m_bwmv;
      item.psfSignal  = m_psfSignalEstimates;
      item.noise      = m_noiseEstimates;
      item.noiseScale = m_noiseScaleEstimates;

      if ( m_adaptiveNormalization.IsValid() )
      {
         item.ax = m_adaptiveNormalization.m_x;
         item.ay = m_adaptiveNormalization.m_y;
         item.am = m_adaptiveNormalization.m_m;

         switch ( m_instance->p_weightScale )
         {
         case IIWeightScale::AvgDev:
            item.as0_avgDev = m_adaptiveNormalization.m_s0;
            item.as1_avgDev = m_adaptiveNormalization.m_s1;
            break;
         case IIWeightScale::MAD:
            item.as0_mad = m_adaptiveNormalization.m_s0;
            item.as1_mad = m_adaptiveNormalization.m_s1;
            break;
         default:
         case IIWeightScale::BWMV:
            item.as0_bwmv = m_adaptiveNormalization.m_s0;
            item.as1_bwmv = m_adaptiveNormalization.m_s1;
            break;
         }
      }

      item.metadata = m_metadata.Serialize();

      TheIntegrationCache->Add( item );
   }
}

// ----------------------------------------------------------------------------

int IntegrationFile::GetFromCache( const String& path )
{
   ResetCacheableData();
   int n = 0;

   if ( TheIntegrationCache != nullptr )
   {
      IntegrationCacheItem item;
      if ( TheIntegrationCache->Get( item, path ) )
      {
         if ( item.mean.Length() == s_numberOfChannels )
            m_mean = item.mean, ++n;

         if ( item.median.Length() == s_numberOfChannels )
            m_median = item.median, ++n;

         if ( item.avgDev.Length() == s_numberOfChannels )
            m_avgDev = item.avgDev, ++n;

         if ( item.mad.Length() == s_numberOfChannels )
            m_mad = item.mad, ++n;

         if ( item.bwmv.Length() == s_numberOfChannels )
            m_bwmv = item.bwmv, ++n;

         if ( item.psfSignal.Length() == s_numberOfChannels )
            m_psfSignalEstimates = item.psfSignal, ++n;

         if ( item.noise.Length() == s_numberOfChannels && item.noiseScale.Length() == s_numberOfChannels )
         {
            m_noiseEstimates = item.noise, ++n;
            m_noiseScaleEstimates = item.noiseScale, ++n;
         }

         if ( !item.metadata.IsEmpty() )
            m_metadata = IntegrationMetadata( item.metadata ), ++n;

         int na = AdaptiveNormalizationData::NumberOfGridElements( s_width, s_height, m_instance->p_adaptiveGridSize );

         if ( item.ax.Length() == na )
            if ( item.ay.Length() == na )
               if ( int( item.am.Length() ) == s_numberOfChannels )
               {
                  DMultiVector as0, as1;
                  switch ( m_instance->p_weightScale )
                  {
                  case IIWeightScale::AvgDev:
                     as0 = item.as0_avgDev;
                     as1 = item.as1_avgDev;
                     break;
                  case IIWeightScale::MAD:
                     as0 = item.as0_mad;
                     as1 = item.as1_mad;
                     break;
                  default:
                  case IIWeightScale::BWMV:
                     as0 = item.as0_bwmv;
                     as1 = item.as1_bwmv;
                     break;
                  }

                  if ( int( as0.Length() ) == s_numberOfChannels )
                     if ( int( as1.Length() ) == s_numberOfChannels )
                     {
                        try
                        {
                           m_adaptiveNormalization = AdaptiveNormalizationData( s_width, s_height, item.ax, item.ay, item.am, as0, as1 );
                           ++n;
                        }
                        catch ( Error& x )
                        {
                           throw Error( path + ": " + x.Message() );
                        }
                        catch ( ... )
                        {
                           throw;
                        }
                     }
               }
      }
   }

   return n;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void IntegrationFile::OpenFileThread::Run()
{
   try
   {
      m_file->Open( m_item.path, m_item.nmlPath, m_item.drzPath, m_instance, m_isReference );
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

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegrationFile.cpp - Released 2023-07-06T16:53:46Z
