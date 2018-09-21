//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.03.01.0003
// ----------------------------------------------------------------------------
// SubframeSelectorInstance.cpp - Released 2017-11-05T16:00:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
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

#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/ErrorHandler.h>
#include <pcl/ElapsedTime.h>
#include <pcl/ProcessInstance.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/ATrousWaveletTransform.h>
#include <pcl/Version.h>
#include <pcl/ICCProfile.h>
#include <pcl/MessageBox.h>

#include "PSF.h"
#include "SubframeSelectorInstance.h"
#include "SubframeSelectorStarDetector.h"
#include "SubframeSelectorInterface.h"
#include "SubframeSelectorCache.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// From Debayer
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
const float __5x5B3Spline_hv[] = { 0.0625F, 0.25F, 0.375F, 0.25F, 0.0625F };
// Gaussian noise scaling factors
const float __5x5B3Spline_kj[] =
        { 0.8907F, 0.2007F, 0.0856F, 0.0413F, 0.0205F, 0.0103F, 0.0052F, 0.0026F, 0.0013F, 0.0007F };

// ----------------------------------------------------------------------------

SubframeSelectorInstance::SubframeSelectorInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   routine( SSRoutine::Default ),
   fileCache( TheSSFileCacheParameter->DefaultValue() ),
   subframes(),
   subframeScale( TheSSSubframeScaleParameter->DefaultValue() ),
   cameraGain( TheSSCameraGainParameter->DefaultValue() ),
   cameraResolution( SSCameraResolution::Default ),
   siteLocalMidnight( TheSSSiteLocalMidnightParameter->DefaultValue() ),
   scaleUnit( SSScaleUnit::Default ),
   dataUnit( SSDataUnit::Default ),
   structureLayers( TheSSStructureLayersParameter->DefaultValue() ),
   noiseLayers( TheSSNoiseLayersParameter->DefaultValue() ),
   hotPixelFilterRadius( TheSSHotPixelFilterRadiusParameter->DefaultValue() ),
   applyHotPixelFilterToDetectionImage( TheSSApplyHotPixelFilterParameter->DefaultValue() ),
   noiseReductionFilterRadius( TheSSNoiseReductionFilterRadiusParameter->DefaultValue() ),
   sensitivity( TheSSSensitivityParameter->DefaultValue() ),
   peakResponse( TheSSPeakResponseParameter->DefaultValue() ),
   maxDistortion( TheSSMaxDistortionParameter->DefaultValue() ),
   upperLimit( TheSSUpperLimitParameter->DefaultValue() ),
   backgroundExpansion( TheSSBackgroundExpansionParameter->DefaultValue() ),
   xyStretch( TheSSXYStretchParameter->DefaultValue() ),
   psfFit( SSPSFFit::Default ),
   psfFitCircular( TheSSPSFFitCircularParameter->DefaultValue() ),
   pedestal( TheSSPedestalParameter->DefaultValue() ),
   roi( 0 ),
   outputDirectory( TheSSOutputDirectoryParameter->DefaultValue() ),
   outputPrefix( TheSSOutputPrefixParameter->DefaultValue() ),
   outputPostfix( TheSSOutputPostfixParameter->DefaultValue() ),
   outputKeyword( TheSSOutputKeywordParameter->DefaultValue() ),
   overwriteExistingFiles( TheSSOverwriteExistingFilesParameter->DefaultValue() ),
   onError( SSOnError::Default ),
   approvalExpression( "" ),
   weightingExpression( "" ),
   sortingProperty( SSSortingProperty::Default ),
   graphProperty( SSGraphProperty::Default ),
   measures()
{
}

SubframeSelectorInstance::SubframeSelectorInstance( const SubframeSelectorInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void SubframeSelectorInstance::Assign( const ProcessImplementation& p )
{
   const SubframeSelectorInstance* x = dynamic_cast<const SubframeSelectorInstance*>( &p );
   if ( x != nullptr )
   {
      routine                                = x->routine;
      subframes                              = x->subframes;
      fileCache                              = x->fileCache;
      subframeScale                          = x->subframeScale;
      cameraGain                             = x->cameraGain;
      cameraResolution                       = x->cameraResolution;
      siteLocalMidnight                      = x->siteLocalMidnight;
      scaleUnit                              = x->scaleUnit;
      dataUnit                               = x->dataUnit;
      structureLayers                        = x->structureLayers;
      noiseLayers                            = x->noiseLayers;
      hotPixelFilterRadius                   = x->hotPixelFilterRadius;
      applyHotPixelFilterToDetectionImage    = x->applyHotPixelFilterToDetectionImage;
      noiseReductionFilterRadius             = x->noiseReductionFilterRadius;
      sensitivity                            = x->sensitivity;
      peakResponse                           = x->peakResponse;
      maxDistortion                          = x->maxDistortion;
      upperLimit                             = x->upperLimit;
      backgroundExpansion                    = x->backgroundExpansion;
      xyStretch                              = x->xyStretch;
      psfFit                                 = x->psfFit;
      psfFitCircular                         = x->psfFitCircular;
      pedestal                               = x->pedestal;
      roi                                    = x->roi;
      outputDirectory                        = x->outputDirectory;
      outputPrefix                           = x->outputPrefix;
      outputPostfix                          = x->outputPostfix;
      outputKeyword                          = x->outputKeyword;
      overwriteExistingFiles                 = x->overwriteExistingFiles;
      onError                                = x->onError;
      approvalExpression                     = x->approvalExpression;
      weightingExpression                    = x->weightingExpression;
      sortingProperty                        = x->sortingProperty;
      graphProperty                          = x->graphProperty;
      measures                               = x->measures;
   }
}

// ----------------------------------------------------------------------------

struct MeasureThreadInputData
{
   // The static settings
   pcl_bool                   showStarDetectionMaps = false;
   SubframeSelectorInstance*  instance = nullptr;
};

// ----------------------------------------------------------------------------

typedef Array<Star> star_list;
typedef Array<PSFData> psf_list;

class SubframeSelectorMeasureThread : public Thread
{
public:

   SubframeSelectorMeasureThread( int index,
                                  ImageVariant* subframe,
                                  const String& subframePath,
                                  MeasureThreadInputData* data ) :
           m_index( index ),
           m_subframe( subframe ),
           m_outputData( subframePath ),
           m_success( false ),
           m_data( data )
   {
   }

   virtual void Run()
   {

      try
      {
         m_success = false;
         ElapsedTime T;

         if ( IsAborted() )
            throw Error( "Aborted" );

         Console console;
         console.NoteLn( "<end><cbr><br>Measuring: " + m_outputData.path );
         MeasureImage();
         console.WriteLn( "Image Calculations: " + T.ToIsoString() );
         T.Reset();

         if ( IsAborted() )
            throw Error( "Aborted" );

         // Crop if the ROI was set
         if ( m_data->instance->roi.IsRect() )
            m_subframe->CropTo( m_data->instance->roi );

         // Run the Star Detector
         star_list stars = StarDetector();

         // Stop if just showing the maps
         if ( m_data->showStarDetectionMaps )
         {
            m_success = true;
            return;
         }

         // Stop if no stars found
         if ( stars.IsEmpty() )
         {
            m_success = false;
            return;
         }

         if ( IsAborted() )
            throw Error( "Aborted" );

         // Run the PSF Fitter
         psf_list fits = FitPSFs( stars.Begin(), stars.End() );

         if ( fits.IsEmpty() )
         {
            m_success = false;
            return;
         }

         if ( IsAborted() )
            throw Error( "Aborted" );

         // Measure Data
         MeasurePSFs( fits );
         console.WriteLn( "     Star Detector: " + T.ToIsoString() );

         console.WriteLn( String().Format( "%i Star(s) detected", stars.Length() ) );
         console.WriteLn( String().Format( "%i PSF(s) fitted", fits.Length() ) );

         m_success = true;
      }
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER
      }
   }

   int Index() const
   {
      return m_index;
   }

   const MeasureData& OutputData() const
   {
      return m_outputData;
   }

   bool Success() const
   {
      return m_success;
   }

private:

   void EvaluateNoise()
   {
      double noiseEstimate = 0;
      double noiseFraction = 0;
      double noiseEstimateKS = 0;
      double noiseFractionKS = 0;
      SeparableFilter H( __5x5B3Spline_hv, __5x5B3Spline_hv, 5 );
      for ( int n = 4; ; )
      {
         ATrousWaveletTransform W( H, n );
         W << *m_subframe;

         size_type N;
         if ( n == 4 )
         {
            noiseEstimateKS = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/__5x5B3Spline_kj[0];
            noiseFractionKS = double( N )/m_subframe->NumberOfPixels();
         }
         noiseEstimate = W.NoiseMRS( ImageVariant( *m_subframe ), __5x5B3Spline_kj, noiseEstimateKS, 3, &N );
         noiseFraction = double( N )/m_subframe->NumberOfPixels();

         if ( noiseEstimate > 0 && noiseFraction >= 0.01 )
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

   void MeasureImage()
   {
      // Most parameters here are defaults, but set it to use 1 CPU at the end
      m_outputData.median = m_subframe->Median( 0, -1, -1, 1 );
      m_outputData.medianMeanDev = m_subframe->AvgDev( m_outputData.median, 0, -1, -1, 1 );
      EvaluateNoise();
      m_outputData.snrWeight = m_outputData.noise != 0 ?
                               pcl::Pow( m_outputData.medianMeanDev, 2.0 ) / pcl::Pow( m_outputData.noise, 2.0 ) : 0;
   }

   star_list StarDetector()
   {
      // Setup StarDetector parameters and find the list of stars
      SubframeSelectorStarDetector starDetector;
      starDetector.showStarDetectionMaps                 = m_data->showStarDetectionMaps;
      starDetector.structureLayers                       = m_data->instance->structureLayers;
      starDetector.noiseLayers                           = m_data->instance->noiseLayers;
      starDetector.hotPixelFilterRadius                  = m_data->instance->hotPixelFilterRadius;
      starDetector.noiseReductionFilterRadius            = m_data->instance->noiseReductionFilterRadius;
      starDetector.applyHotPixelFilterToDetectionImage   = m_data->instance->applyHotPixelFilterToDetectionImage;
      starDetector.sensitivity                           = m_data->instance->sensitivity;
      starDetector.peakResponse                          = m_data->instance->peakResponse;
      starDetector.maxDistortion                         = m_data->instance->maxDistortion;
      starDetector.upperLimit                            = m_data->instance->upperLimit;
      starDetector.backgroundExpansion                   = m_data->instance->backgroundExpansion;
      starDetector.xyStretch                             = m_data->instance->xyStretch;
      return starDetector.GetStars( m_subframe );
   }

   psf_list FitPSFs( star_list::const_iterator begin, star_list::const_iterator end )
   {
      psf_list PSFs;
      for ( star_list::const_iterator i = begin; i != end; ++i )
      {
         int radius = pcl::Max(3, pcl::Ceil(pcl::Sqrt(i->size)));
         Rect rect( Point( i->position.x - radius, i->position.y - radius ),
                    Point( i->position.x + radius, i->position.y + radius ) );

         PSFFit psfFit( *m_subframe, i->position, rect, PSFFunction( m_data->instance->psfFit ),
                        m_data->instance->psfFitCircular );
         PSFData psf = psfFit.psf;

         if ( psf.status == PSFFit::FittedOk )
            PSFs.Append( psf );
      }
      return PSFs;
   }

   PSFFit::Function PSFFunction( const pcl_enum& fit )
   {
      switch ( m_data->instance->psfFit )
      {
      case SSPSFFit::Gaussian: return PSFFit::Function::Gaussian;
      case SSPSFFit::Moffat10: return PSFFit::Function::MoffatA;
      case SSPSFFit::Moffat8: return PSFFit::Function::Moffat8;
      case SSPSFFit::Moffat6: return PSFFit::Function::Moffat6;
      case SSPSFFit::Moffat4: return PSFFit::Function::Moffat4;
      case SSPSFFit::Moffat25: return PSFFit::Function::Moffat25;
      case SSPSFFit::Moffat15: return PSFFit::Function::Moffat15;
      case SSPSFFit::Lorentzian: return PSFFit::Function::Lorentzian;
      default: return PSFFit::Function::Gaussian;
      }
   }

   void MeasurePSFs( psf_list fits )
   {
      m_outputData.stars = fits.Length();

      // Determine the best fit to weight the others against
      double minMAD = DBL_MAX;
      for ( psf_list::const_iterator i = fits.Begin(); i != fits.End(); ++i )
         minMAD = pcl::Min( minMAD, i->mad );

      // Analyze each star parameter against the best residual
      double fwhmSumSigma = 0;
      double eccentricitySumSigma = 0;
      double residualSumSigma = 0;
      double sumWeight = 0;
      Array<double> fwhms( fits.Length(), 0 );
      Array<double> eccentricities( fits.Length(), 0 );
      Array<double> residuals( fits.Length(), 0 );
      for ( size_type i = 0; i < fits.Length(); ++i )
      {
         PSFData* fit = &fits[i];

         double MAD = fit->mad;
         double weight = minMAD / MAD;
         sumWeight += weight;

         fwhms[i] = pcl::Sqrt( fit->sx * fit->sy );
         eccentricities[i] = pcl::Sqrt(1.0 - pcl::Pow(fit->sy / fit->sx, 2.0));
         residuals[i] = MAD;

         fwhmSumSigma += weight * fwhms[i];
         eccentricitySumSigma += weight * eccentricities[i];
         residualSumSigma += weight * MAD;
      }

      // Average each star parameter against the total weight
      double FWHM = fwhmSumSigma / sumWeight;
      m_outputData.fwhm = PSFData::FWHM( PSFFunction( m_data->instance->psfFit ), FWHM, 0 ); // beta is unused here
      m_outputData.eccentricity = eccentricitySumSigma / sumWeight;
      m_outputData.starResidual = residualSumSigma / sumWeight;

      // Determine Mean Deviation for each star parameter
      m_outputData.fwhmMeanDev = pcl::AvgDev( fwhms.Begin(), fwhms.End(),
                                              pcl::Median( fwhms.Begin(), fwhms.End() ) );
      m_outputData.fwhmMeanDev = PSFData::FWHM( PSFFunction( m_data->instance->psfFit ),
                                                m_outputData.fwhmMeanDev, 0 ); // beta is unused here
      m_outputData.eccentricityMeanDev = pcl::AvgDev( eccentricities.Begin(), eccentricities.End(),
                                                      pcl::Median( eccentricities.Begin(), eccentricities.End() ) );
      m_outputData.starResidualMeanDev = pcl::AvgDev( residuals.Begin(), residuals.End(),
                                                      pcl::Median( residuals.Begin(), residuals.End() ) );
   }

   int                        m_index;
   AutoPointer<ImageVariant>  m_subframe;
   MeasureData                m_outputData;
   bool                       m_success : 1;

   MeasureThreadInputData* m_data;
};

// ----------------------------------------------------------------------------

ImageVariant* SubframeSelectorInstance::LoadSubframe( const String& filePath )
{
   Console console;

   /*
    * Find out an installed file format that can read image files with the
    * specified extension ...
    */
   FileFormat format( File::ExtractExtension( filePath ), true, false );

   /*
    * ... and create a format instance (usually a disk file) to access this
    * subframe image.
    */
   FileFormatInstance file( format );

   /*
    * Open input stream.
    */
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath ) )
      throw CaughtException();

   /*
    * Check for an empty subframe.
    */
   if ( images.IsEmpty() )
      throw Error( filePath + ": Empty subframe image." );

   /*
    * Subframe files can store multiple images -
    * this is not supported.
    */
   if ( images.Length() > 1 )
      throw Error( filePath + ": Has multiple images; unsupported " );

   // Create a shared image, 32-bit floating point
   Image* image = new Image( (void*)0, 0, 0 );

   // Read the image
   if ( !file.ReadImage( *image ) )
      throw CaughtException();


   /*
    * Optional pedestal subtraction.
    */
   if ( pedestal > 0 )
   {
      Console().WriteLn( String().Format( "Subtracting pedestal: %d DN", pedestal ) );
      image->Apply( (double) pedestal / (double) TheSSCameraResolutionParameter->ElementData(cameraResolution), ImageOp::Sub );
   }

   /*
    * Convert to grayscale
    */
   ImageVariant* imageVariant = new ImageVariant();
   image->GetIntensity( *imageVariant );
   image->FreeData();

   /*
    * Close the input stream.
    */
   if ( !file.Close() )
      throw CaughtException();

   return imageVariant;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
    whyNot = "SubframeSelector can only be executed in the global context.";
    return false;
}

bool SubframeSelectorInstance::IsHistoryUpdater( const View& view ) const
{
    return false;
}

bool SubframeSelectorInstance::CanTestStarDetector( String &whyNot ) const {
   if ( subframes.IsEmpty()) {
      whyNot = "No subframes have been specified.";
      return false;
   }

   return true;
}

bool SubframeSelectorInstance::TestStarDetector() {
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanTestStarDetector( why ))
         throw Error( why );

      for ( subframe_list::const_iterator i = subframes.Begin(); i != subframes.End(); ++i )
         if ( i->enabled && !File::Exists( i->path ))
            throw ("No such file exists on the local filesystem: " + i->path);
   }
   Console console;

   // Setup common data for each thread
   MeasureThreadInputData inputThreadData;
   inputThreadData.showStarDetectionMaps  = true;
   inputThreadData.instance               = this;

   try {
      /*
       * For all errors generated, we want a report on the console. This is
       * customary in PixInsight for all batch processes.
       */
      Exception::EnableConsoleOutput();
      Exception::DisableGUIOutput();

      console.EnableAbort();
      Module->ProcessEvents();

      try
      {
         /*
          * Extract the first target
          * frame from the targets list,
          * load and calibrate it.
          */
         SubframeItem item = *subframes;

         console.WriteLn( String().Format( "<end><cbr><br>Measuring subframe %u of %u", 1, subframes.Length() ) );
         Module->ProcessEvents();

         SubframeSelectorMeasureThread* thread = new SubframeSelectorMeasureThread( 1, LoadSubframe( item.path ),
                                                                                    item.path, &inputThreadData );

         // Keep the GUI responsive, last chance to abort
         Module->ProcessEvents();
         if ( console.AbortRequested() )
            throw ProcessAborted();

         thread->Run();

         Module->ProcessEvents();

         return true;

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

         try
         {
            throw;
         }
         ERROR_HANDLER

         console.ResetStatus();
         console.EnableAbort();

         console.NoteLn( "Abort on error." );
         throw ProcessAborted();
      }
   } // try

   catch ( ... ) {
      /*
       * All breaking errors are caught here.
       */
      Exception::EnableGUIOutput( true );
      throw;
   }
}

bool SubframeSelectorInstance::CanMeasure( String &whyNot ) const {
   if ( subframes.IsEmpty()) {
      whyNot = "No subframes have been specified.";
      return false;
   }

   return true;
}

bool SubframeSelectorInstance::Measure() {
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanMeasure( why ))
         throw Error( why );

      for ( subframe_list::const_iterator i = subframes.Begin(); i != subframes.End(); ++i )
         if ( i->enabled && !File::Exists( i->path ))
            throw ("No such file exists on the local filesystem: " + i->path);
   }

   // Reset measured values
   measures.Clear();

   Console console;

   // Setup common data for each thread
   MeasureThreadInputData inputThreadData;
   inputThreadData.instance = this;

   // Setup the cache
   if ( TheSubframeSelectorCache == nullptr )
   {
      new SubframeSelectorCache; // loads cache upon construction
      if ( TheSubframeSelectorCache->IsEnabled() )
         if ( TheSubframeSelectorCache->IsEmpty() )
            console.NoteLn( "<end><cbr><br>* Empty file cache" );
         else
            console.NoteLn( "<end><cbr><br>* Loaded cache: " + String( TheSubframeSelectorCache->NumberOfItems() ) + " item(s)" );
   }

   try {

      /*
       * For all errors generated, we want a report on the console. This is
       * customary in PixInsight for all batch processes.
       */
      Exception::EnableConsoleOutput();
      Exception::DisableGUIOutput();

      console.EnableAbort();
      Module->ProcessEvents();

      /*
       * Begin subframe measuring process.
       */

      int succeeded = 0;
      int failed = 0;
      int skipped = 0;

      /*
       * Running threads list. Note that IndirectArray<> initializes all item
       * pointers to zero.
       */
      int numberOfThreads = Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 );
      thread_list runningThreads( Min( int( subframes.Length()), numberOfThreads ));

      /*
       * Pending subframes list. We use this list for temporary storage of
       * indices that need measuring.
       */
      Array<size_type> pendingItems;
      for ( size_type i = 0; i < subframes.Length(); ++i )
         if ( subframes[i].enabled )
            pendingItems << i;
         else
         {
            console.NoteLn( "* Skipping disabled target: " + subframes[i].path );
            ++skipped;
         }
      size_type pendingItemsTotal = pendingItems.Length();

      console.WriteLn( String().Format( "<end><cbr><br>Measuring of %u subframes:", pendingItemsTotal ) );
      console.WriteLn( String().Format( "* Using %u worker threads", runningThreads.Length() ) );

      try
      {
         /*
          * Thread watching loop.
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
                     if ( !(*i)->Wait( 150 ) )
                     {
                        ++running;
                        continue;
                     }

                     /*
                      * A thread has just finished.
                      */
                     try
                     {
                        if ( !(*i)->Success() )
                           throw Error( (*i)->ConsoleOutputText() );

                        (*i)->FlushConsoleOutputText();
                        Module->ProcessEvents();

                        // Store output data
                        MeasureItem m( (*i)->Index() );
                        m.Input( (*i)->OutputData() );
                        measures.Append( m );
                        (*i)->OutputData().AddToCache();

                        // Dispose this calibration thread, since we are done with
                        // it. NB: IndirectArray<T>::Delete() sets to zero the
                        // pointer pointed to by the iterator, but does not remove
                        // the array element.
                        runningThreads.Delete( i );
                     }
                     catch ( ... )
                     {
                        // Ensure the thread is also destroyed in the event of
                        // error; we'd enter an infinite loop otherwise!
                        runningThreads.Delete( i );
                        throw;
                     }

                     ++succeeded;
                  }

                  /*
                   * If there are items pending, create a new thread and
                   * fire the next one.
                   */
                  if ( !pendingItems.IsEmpty() )
                  {
                     SubframeItem item = subframes[*pendingItems];

                     size_type threadIndex = i - runningThreads.Begin();
                     console.NoteLn( String().Format( "<end><cbr><br>[%03u] Measuring subframe %u of %u",
                                                      threadIndex,
                                                      pendingItemsTotal-pendingItems.Length()+1,
                                                      pendingItemsTotal ) );

                     /*
                      * Check for the subframe in the cache, and use that if possible
                      */
                     MeasureData cacheData( item.path );
                     if ( cacheData.GetFromCache() && fileCache )
                     {
                        console.NoteLn( "<end><cbr>* Retrieved data from file cache." );
                        MeasureItem m( pendingItemsTotal-pendingItems.Length()+1, item.path );
                        m.Input( cacheData );
                        measures.Append( m );
                        ++succeeded;
                        ++running;
                     }

                     /*
                      * Create a new thread for this subframe image
                      */
                     else
                     {
                        *i = new SubframeSelectorMeasureThread( pendingItemsTotal-pendingItems.Length()+1,
                                                                LoadSubframe( item.path ),
                                                                item.path,
                                                                &inputThreadData );
                        (*i)->Start( ThreadPriority::DefaultMax );
                        ++running;
                     }

                     pendingItems.Remove( pendingItems.Begin() );

                     if ( pendingItems.IsEmpty() )
                        console.NoteLn( "<br>* Waiting for running tasks to terminate...<br>" );
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

               console.ResetStatus();
               console.EnableAbort();

               console.NoteLn( "Abort on error." );
               throw ProcessAborted();
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

      /*
       * Fail if no images have been measured.
       */
      if ( succeeded == 0 ) {
         if ( failed == 0 )
            throw Error( "No images were measured: Empty subframes list? No enabled subframes?" );
         throw Error( "No image could be measured." );
      }

      /*
       * Write the final report to the console.
       */
      console.NoteLn(
              String().Format( "<end><cbr><br>===== SubframeSelector: %u succeeded, %u failed, %u skipped =====",
                               succeeded, failed, skipped ));

      measures.Sort( SubframeSortingBinaryPredicate( SSSortingProperty::Index, 0 ) );

      if ( TheSubframeSelectorInterface != nullptr )
      {
         TheSubframeSelectorInterface->ClearMeasurements();
         TheSubframeSelectorInterface->AddMeasurements( measures.Begin(), measures.End() );
         TheSubframeSelectorInterface->UpdateMeasurementImagesList();
         TheSubframeSelectorInterface->UpdateMeasurementGraph();
      }
      return true;
   } // try

   catch ( ... ) {
      /*
       * All breaking errors are caught here.
       */
      Exception::EnableGUIOutput( true );
      throw;
   }
}

void SubframeSelectorInstance::ApproveMeasurements()
{
   if ( approvalExpression.IsEmpty() )
   {
      for ( Array<MeasureItem>::iterator i = measures.Begin(); i != measures.End(); ++i )
         if ( !i->locked )
            i->enabled = true;
   }
   else
   {
      // First, get all Medians and Mean Deviation from Medians for Sigma units
      MeasureProperties properties = MeasureProperties();
      MeasureUtils::MeasureProperties( measures, subframeScale, scaleUnit,
                                       cameraGain, cameraResolution, dataUnit,
                                       properties );

      for ( Array<MeasureItem>::iterator i = measures.Begin(); i != measures.End(); ++i )
      {
         if ( i->locked )
            continue;

         // The standard parameters for the MeasureItem
         String JSEvaluator = i->JavaScriptParameters( subframeScale, scaleUnit, cameraGain,
                                                       TheSSCameraResolutionParameter->ElementData( cameraResolution ),
                                                       dataUnit, properties );

         // The final expression that evaluates to a return value
         JSEvaluator += approvalExpression;

         // Try to get the final result and update the MeasureItem
         Variant result = Module->EvaluateScript( JSEvaluator.DecodedHTMLSpecialChars(), "JavaScript" );
         if ( !result.IsValid() )
            throw Error( "Approval Error: Invalid script execution" );
         String resultText = result.ToString();
         if ( resultText.Contains( "Error" ) )
            throw Error( resultText );

         i->enabled = result.ToBool();
      }
   }
}

void SubframeSelectorInstance::WeightMeasurements()
{
   if ( weightingExpression.IsEmpty() )
   {
      for ( Array<MeasureItem>::iterator i = measures.Begin(); i != measures.End(); ++i )
         i->weight = 0;
   }
   else
   {
      // First, get all Medians and Mean Deviation from Medians for Sigma units
      MeasureProperties properties = MeasureProperties();
      MeasureUtils::MeasureProperties( measures, subframeScale, scaleUnit,
                                       cameraGain, cameraResolution, dataUnit,
                                       properties );

      for ( Array<MeasureItem>::iterator i = measures.Begin(); i != measures.End(); ++i )
      {
         // The standard parameters for the MeasureItem
         String JSEvaluator = i->JavaScriptParameters( subframeScale, scaleUnit, cameraGain,
                                                       TheSSCameraResolutionParameter->ElementData( cameraResolution ),
                                                       dataUnit, properties );

         // The final expression that evaluates to a return value
         JSEvaluator += weightingExpression;

         // Try to get the final result and update the MeasureItem
         Variant result = Module->EvaluateScript( JSEvaluator.DecodedHTMLSpecialChars(), "JavaScript" );
         if ( !result.IsValid() )
            throw Error( "Approval Error: Invalid script execution" );
         String resultText = result.ToString();
         if ( resultText.Contains( "Error" ) )
            throw Error( resultText );

         i->weight = result.ToFloat();
      }
   }
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

void SubframeSelectorInstance::WriteMeasuredImage( MeasureItem* item )
{
   Console console;

   /*
    * Output directory.
    */
   String fileDir = outputDirectory.Trimmed();
   if ( fileDir.IsEmpty() )
      fileDir = File::ExtractDrive( item->path ) + File::ExtractDirectory( item->path );
   if ( fileDir.IsEmpty() )
      throw Error( item->path + ": Unable to determine an output directory." );
   if ( !fileDir.EndsWith( '/' ) )
      fileDir.Append( '/' );

   /*
    * Output file extension, which defines the output file format.
    */
   String fileExtension = File::ExtractExtension( item->path );
   if ( fileExtension.IsEmpty() )
      throw Error( item->path + ": Unable to determine an output file extension." );
   if ( !fileExtension.StartsWith( '.' ) )
      fileExtension.Prepend( '.' );

   /*
    * Output file name.
    */
   String fileName = File::ExtractName( item->path ).Trimmed();
   if ( !outputPrefix.IsEmpty() )
      fileName.Prepend( outputPrefix );
   if ( !outputPostfix.IsEmpty() )
      fileName.Append( outputPostfix );
   if ( fileName.IsEmpty() )
      throw Error( item->path + ": Unable to determine an output file name." );

   /*
    * Output file path.
    */
   String outputFilePath = fileDir + fileName + fileExtension;

   console.WriteLn( "<end><cbr><br>Writing output file: " + outputFilePath );


   /*
    * Check for an already existing file, and either overwrite it (but show a
    * warning message if that happens), or find a unique file name, depending
    * on the overwriteExistingFiles parameter.
    */
   if ( File::Exists( outputFilePath ) )
      if ( overwriteExistingFiles )
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
   FileFormat format( fileExtension, false/*read*/, true/*write*/ );

   if ( format.IsDeprecated() )
      console.WarningLn( "** Warning: Deprecated file format: " + format.Name() );

   FileFormatInstance outputFile( format );

   /*
    * ... and create a format instance (usually a disk file).
    */
   FileFormatInstance inputFile( format );
   if ( !outputFile.Create( outputFilePath ) )
      throw CaughtException();

   /*
    * Open input stream.
    */
   ImageDescriptionArray images;
   if ( !inputFile.Open( images, item->path ) )
      throw CaughtException();

   /*
    * Check for an empty subframe.
    */
   if ( images.IsEmpty() )
      throw Error( item->path + ": Empty subframe image." );

   /*
    * Subframe files can store multiple images - only the first image
    * will be used in such case, and the rest will be ignored.
    */
   if ( images.Length() > 1 )
      throw Error ( item->path + ": Has multiple images; unsupported " );

   // Create a shared image, 32-bit floating point
   Image* image = new Image( (void*)0, 0, 0 );

   // Read the image
   if ( !inputFile.ReadImage( *image ) )
      throw CaughtException();

   /*
    * Determine the output sample format: bits per sample and whether integer
    * or real samples.
    */
   outputFile.SetOptions( images[0].options );

   /*
    * File formats often use format-specific data.
    * Keep track of private data structures.
    */
   if ( format.UsesFormatSpecificData() && format.ValidateFormatSpecificData( inputFile.FormatSpecificData() ) )
      outputFile.SetFormatSpecificData( inputFile.FormatSpecificData() );

   /*
    * Set image properties.
    */
   if ( !inputFile.ReadImageProperties().IsEmpty() )
      if ( format.CanStoreImageProperties() )
      {
         outputFile.WriteImageProperties( inputFile.ReadImageProperties() );
         if ( !format.SupportsViewProperties() )
            console.WarningLn( "** Warning: The output format cannot store view properties; existing properties have been stored as BLOB data." );
      }
      else
         console.WarningLn( "** Warning: The output format cannot store image properties; existing properties will be lost." );

   /*
    * Add FITS header keywords and preserve existing ones, if possible.
    * N.B.: A COMMENT or HISTORY keyword cannot have a value; these keywords
    * have only the name and comment components.
    */
   if ( format.CanStoreKeywords() )
   {
      FITSKeywordArray keywords;
      inputFile.ReadFITSKeywords( keywords );

      // Remove old weight keywords
      FITSKeywordArray newKeywords = FITSKeywordArray();
      for ( FITSKeywordArray::const_iterator k = keywords.Begin(); k != keywords.End(); ++k )
         if ( k->name != IsoString( outputKeyword ) )
            newKeywords.Append( *k );

      newKeywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Measured with " + PixInsightVersion::AsString() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Measured with " + Module->ReadableVersion() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Measured with SubframeSelector process" );

      if ( !outputKeyword.IsEmpty() )
         newKeywords << FITSHeaderKeyword( outputKeyword,
                                           String().Format( "%.6f", item->weight ),
                                           "SubframeSelector.weight" );

      outputFile.WriteFITSKeywords( newKeywords );
   }
   else
   {
      console.WarningLn( "** Warning: The output format cannot store FITS header keywords - subframe weight metadata not embedded." );
   }

   /*
    * Preserve ICC profile, if possible.
    */
   ICCProfile inputProfile;
   inputFile.ReadICCProfile( inputProfile );
   if ( inputProfile.IsProfile() )
      if ( format.CanStoreICCProfiles() )
         outputFile.WriteICCProfile( inputProfile );
      else
         console.WarningLn( "** Warning: The output format cannot store color profiles - original ICC profile not embedded." );

   /*
    * Write the output image and close the output stream.
    */
   if ( !outputFile.WriteImage( image ) || !outputFile.Close() )
   {
      image->FreeData();
      throw CaughtException();
   }
   image->FreeData();
}

bool SubframeSelectorInstance::CanOutput( String &whyNot ) const {
   if ( measures.IsEmpty())
   {
      whyNot = "No measurements have been made.";
      return false;
   }

   if ( outputKeyword.IsEmpty() )
   {
      whyNot = "The specified output keyword is blank.";
      return false;
   }

   if ( !outputDirectory.IsEmpty() && !File::DirectoryExists( outputDirectory ) )
   {
      whyNot = "The specified output directory does not exist.";
      return false;
   }

   return true;
}

bool SubframeSelectorInstance::Output()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanOutput( why ) )
         throw Error( why );

      for ( Array<MeasureItem>::const_iterator i = measures.Begin(); i != measures.End(); ++i )
         if ( i->enabled && !File::Exists( i->path ) )
            throw ("No such file exists on the local filesystem: " + i->path);

      size_type o = 0;
      size_type r = 0;

      for ( size_type i = 0; i < measures.Length(); ++i )
      {
         try
         {
            if ( measures[i].enabled )
            {
               Console().NoteLn( String().Format( "<end><cbr><br>Outputting subframe %u of %u",
                                                  i+1, measures.Length() ) );
               WriteMeasuredImage( measures.At( i ) );
               ++o;
            } else {
               Console().NoteLn( String().Format( "<end><cbr><br>Skipping subframe %u of %u",
                                                  i+1, measures.Length() ) );
               ++r;
            }
         }
         catch ( ... )
         {
            Console console;
            console.Note( "<end><cbr><br>* Applying error policy: " );

            switch ( onError )
            {
            default: // ?
            case SSOnError::Continue:
               console.NoteLn( "Continue on error." );
               continue;

            case SSOnError::Abort:
               console.NoteLn( "Abort on error." );
               throw;

            case SSOnError::AskUser:
            {
               console.NoteLn( "Ask on error..." );

               int r = MessageBox( "<p style=\"white-space:pre;\">"
                                           "An error occurred during SubframeSelector Output. What do you want to do?</p>",
                                   "SubframeSelector",
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
      }
      Console().NoteLn( String().Format( "<end><cbr><br>%u Output subframes, %u Rejected subframes, %u total",
                                         o, r, measures.Length() ) );

      return true;
   }
}

bool SubframeSelectorInstance::CanExecuteGlobal( String &whyNot ) const
{
   if ( routine == SSRoutine::StarDetectionPreview )
   {
      if ( subframes.IsEmpty())
      {
         whyNot = "No subframes have been specified.";
         return false;
      }

      return true;
   }
   else if ( routine == SSRoutine::MeasureSubframes )
   {
      if ( subframes.IsEmpty())
      {
         whyNot = "No subframes have been specified.";
         return false;
      }

      return true;
   }
   else if ( routine == SSRoutine::OutputSubframes )
   {
      if ( measures.IsEmpty())
      {
         whyNot = "No measurements have been made.";
         return false;
      }

      if ( outputKeyword.IsEmpty() )
      {
         whyNot = "The specified output keyword is blank.";
         return false;
      }

      if ( !outputDirectory.IsEmpty() && !File::DirectoryExists( outputDirectory ) )
      {
         whyNot = "The specified output directory does not exist.";
         return false;
      }

      return true;
   }

   whyNot = "Unknown routine.";
   return false;
}

bool SubframeSelectorInstance::ExecuteGlobal()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanExecuteGlobal( why ))
         throw Error( why );

      for ( subframe_list::const_iterator i = subframes.Begin(); i != subframes.End(); ++i )
         if ( i->enabled && !File::Exists( i->path ))
            throw ("No such file exists on the local filesystem: " + i->path);

      if ( routine == SSRoutine::StarDetectionPreview )
         return TestStarDetector();

      if ( routine == SSRoutine::MeasureSubframes )
         return Measure();

      if ( routine == SSRoutine::OutputSubframes )
         return Output();

      throw Error("Unimplemented Routine");
      return false;
   }
}

void* SubframeSelectorInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSRoutineParameter )
      return &routine;

   else if ( p == TheSSSubframeEnabledParameter )
      return &subframes[tableRow].enabled;
   else if ( p == TheSSSubframePathParameter )
      return subframes[tableRow].path.Begin();

   else if ( p == TheSSFileCacheParameter )
      return &fileCache;

   else if ( p == TheSSSubframeScaleParameter )
      return &subframeScale;
   else if ( p == TheSSCameraGainParameter )
      return &cameraGain;
   else if ( p == TheSSCameraResolutionParameter )
      return &cameraResolution;
   else if ( p == TheSSSiteLocalMidnightParameter )
      return &siteLocalMidnight;
   else if ( p == TheSSScaleUnitParameter )
      return &scaleUnit;
   else if ( p == TheSSDataUnitParameter )
      return &dataUnit;

   else if ( p == TheSSStructureLayersParameter )
      return &structureLayers;
   else if ( p == TheSSNoiseLayersParameter )
      return &noiseLayers;
   else if ( p == TheSSHotPixelFilterRadiusParameter )
      return &hotPixelFilterRadius;
   else if ( p == TheSSApplyHotPixelFilterParameter )
      return &applyHotPixelFilterToDetectionImage;
   else if ( p == TheSSNoiseReductionFilterRadiusParameter )
      return &noiseReductionFilterRadius;
   else if ( p == TheSSSensitivityParameter )
      return &sensitivity;
   else if ( p == TheSSPeakResponseParameter )
      return &peakResponse;
   else if ( p == TheSSMaxDistortionParameter )
      return &maxDistortion;
   else if ( p == TheSSUpperLimitParameter )
      return &upperLimit;
   else if ( p == TheSSBackgroundExpansionParameter )
      return &backgroundExpansion;
   else if ( p == TheSSXYStretchParameter )
      return &xyStretch;
   else if ( p == TheSSPSFFitParameter )
      return &psfFit;
   else if ( p == TheSSPSFFitCircularParameter )
      return &psfFitCircular;
   else if ( p == TheSSPedestalParameter )
      return &pedestal;
   else if ( p == TheSSROIX0Parameter )
      return &roi.x0;
   else if ( p == TheSSROIY0Parameter )
      return &roi.y0;
   else if ( p == TheSSROIX1Parameter )
      return &roi.x1;
   else if ( p == TheSSROIY1Parameter )
      return &roi.y1;

   else if ( p == TheSSOutputDirectoryParameter )
      return outputDirectory.Begin();
   else if ( p == TheSSOutputPrefixParameter )
      return outputPrefix.Begin();
   else if ( p == TheSSOutputPostfixParameter )
      return outputPostfix.Begin();
   else if ( p == TheSSOutputKeywordParameter )
      return outputKeyword.Begin();
   else if ( p == TheSSOverwriteExistingFilesParameter )
      return &overwriteExistingFiles;
   else if ( p == TheSSOnErrorParameter )
      return &onError;

   else if ( p == TheSSApprovalExpressionParameter )
      return approvalExpression.Begin();
   else if ( p == TheSSWeightingExpressionParameter )
      return weightingExpression.Begin();

   else if ( p == TheSSSortingPropertyParameter )
      return &sortingProperty;
   else if ( p == TheSSGraphPropertyParameter )
      return &graphProperty;

   else if ( p == TheSSMeasurementIndexParameter )
      return &measures[tableRow].index;
   else if ( p == TheSSMeasurementEnabledParameter )
      return &measures[tableRow].enabled;
   else if ( p == TheSSMeasurementLockedParameter )
      return &measures[tableRow].locked;
   else if ( p == TheSSMeasurementPathParameter )
      return measures[tableRow].path.Begin();
   else if ( p == TheSSMeasurementWeightParameter )
      return &measures[tableRow].weight;
   else if ( p == TheSSMeasurementFWHMParameter )
      return &measures[tableRow].fwhm;
   else if ( p == TheSSMeasurementEccentricityParameter )
      return &measures[tableRow].eccentricity;
   else if ( p == TheSSMeasurementSNRWeightParameter )
      return &measures[tableRow].snrWeight;
   else if ( p == TheSSMeasurementMedianParameter )
      return &measures[tableRow].median;
   else if ( p == TheSSMeasurementMedianMeanDevParameter )
      return &measures[tableRow].medianMeanDev;
   else if ( p == TheSSMeasurementNoiseParameter )
      return &measures[tableRow].noise;
   else if ( p == TheSSMeasurementNoiseRatioParameter )
      return &measures[tableRow].noiseRatio;
   else if ( p == TheSSMeasurementStarsParameter )
      return &measures[tableRow].stars;
   else if ( p == TheSSMeasurementStarResidualParameter )
      return &measures[tableRow].starResidual;
   else if ( p == TheSSMeasurementFWHMMeanDevParameter )
      return &measures[tableRow].fwhmMeanDev;
   else if ( p == TheSSMeasurementEccentricityMeanDevParameter )
      return &measures[tableRow].eccentricityMeanDev;
   else if ( p == TheSSMeasurementStarResidualMeanDevParameter )
      return &measures[tableRow].starResidualMeanDev;

   return nullptr;
}

bool SubframeSelectorInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSSubframesParameter )
   {
      subframes.Clear();
      if ( sizeOrLength > 0 )
         subframes.Add( SubframeItem(), sizeOrLength );
   }
   else if ( p == TheSSSubframePathParameter )
   {
      subframes[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         subframes[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputDirectoryParameter )
   {
      outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         outputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputPrefixParameter )
   {
      outputPrefix.Clear();
      if ( sizeOrLength > 0 )
         outputPrefix.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputPostfixParameter )
   {
      outputPostfix.Clear();
      if ( sizeOrLength > 0 )
         outputPostfix.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputKeywordParameter )
   {
      outputKeyword.Clear();
      if ( sizeOrLength > 0 )
         outputKeyword.SetLength( sizeOrLength );
   }
   else if ( p == TheSSApprovalExpressionParameter )
   {
      approvalExpression.Clear();
      if ( sizeOrLength > 0 )
         approvalExpression.SetLength( sizeOrLength );
   }
   else if ( p == TheSSWeightingExpressionParameter )
   {
      weightingExpression.Clear();
      if ( sizeOrLength > 0 )
         weightingExpression.SetLength( sizeOrLength );
   }
   else if ( p == TheSSMeasurementsParameter )
   {
      measures.Clear();
      if ( sizeOrLength > 0 )
         for ( size_type i = 0; i < sizeOrLength; ++i )
            measures.Add( MeasureItem( i ) );
   }
   else if ( p == TheSSMeasurementPathParameter )
   {
      measures[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         measures[tableRow].path.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type SubframeSelectorInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheSSSubframesParameter )
      return subframes.Length();
   else if ( p == TheSSSubframePathParameter )
      return subframes[tableRow].path.Length();

   else if ( p == TheSSOutputDirectoryParameter )
      return outputDirectory.Length();
   else if ( p == TheSSOutputPrefixParameter )
      return outputPrefix.Length();
   else if ( p == TheSSOutputPostfixParameter )
      return outputPostfix.Length();
   else if ( p == TheSSOutputKeywordParameter )
      return outputKeyword.Length();

   else if ( p == TheSSApprovalExpressionParameter )
      return approvalExpression.Length();
   else if ( p == TheSSWeightingExpressionParameter )
      return weightingExpression.Length();

   else if ( p == TheSSMeasurementsParameter )
      return measures.Length();
   else if ( p == TheSSMeasurementPathParameter )
      return measures[tableRow].path.Length();

   return 0;
}


// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInstance.cpp - Released 2017-11-05T16:00:00Z
