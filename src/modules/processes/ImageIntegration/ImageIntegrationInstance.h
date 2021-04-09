//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// ImageIntegrationInstance.h - Released 2021-04-09T19:41:48Z
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

#ifndef __ImageIntegrationInstance_h
#define __ImageIntegrationInstance_h

#include <pcl/Matrix.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/Vector.h>

#include "ImageIntegrationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ImageIntegrationInstance : public ProcessImplementation
{
public:

   ImageIntegrationInstance( const MetaProcess* );
   ImageIntegrationInstance( const ImageIntegrationInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool IsHistoryUpdater( const View& ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter*, size_type tableRow ) const override;

private:

   struct ImageItem
   {
      pcl_bool enabled = true;
      String   path;    // image file
      String   nmlPath; // local normalization data file
      String   drzPath; // drizzle data file

      ImageItem() = default;
      ImageItem( const ImageItem& ) = default;

      ImageItem( const String& a_path ) : path( a_path )
      {
      }

      bool IsDefined() const
      {
         return !path.IsEmpty();
      }
   };

   typedef Array<ImageItem>  image_list;

   image_list  p_images;

   String      p_inputHints;    // input format hints

   pcl_enum    p_combination;   // combination operation: average, median, minimum, maximum

   pcl_enum    p_normalization; // none | additive | multiplicative | additive+scaling | mult.+scaling | local

   pcl_enum    p_weightMode;    // don't care | exposure time | noise | signal | median | mean | keyword
   String      p_weightKeyword;
   pcl_enum    p_weightScale;   // scale estimator used for image weighting

   int32       p_adaptiveGridSize;  // adaptive normalization, matrix size
   pcl_bool    p_adaptiveNoScale;   // adaptive normalization, use only adaptive location estimates

   pcl_bool    p_ignoreNoiseKeywords; // always evaluate noise of input images; ignore NOISExx keywords

   pcl_enum    p_rejection;     // pixel rejection: None|MinMax|PClip|SigmaClip|AvgSigmaClip|CCDClip
   pcl_enum    p_rejectionNormalization;

   int32       p_minMaxLow;     // low rejected pixels for min/max
   int32       p_minMaxHigh;    // high rejected pixels for min/max

   float       p_pcClipLow;     // low percentile clipping point
   float       p_pcClipHigh;    // high percentile clipping point

   float       p_sigmaLow;      // low sigma clipping, in sigma units
   float       p_sigmaHigh;     // high sigma clipping, in sigma units

   float       p_winsorizationCutoff; // cutoff point for Winsorized sigma clipping

   float       p_linearFitLow;  // low linear fit tolerance, in sigma units
   float       p_linearFitHigh; // high linear fit tolerance, in sigma units

   float       p_esdOutliersFraction;  // maximum fraction of outliers for ESD rejection
   float       p_esdAlpha;             // type I error probability for ESD rejection
   float       p_esdLowRelaxation;     // relaxation factor for ESD rejection of low pixels

   float       p_ccdGain;       // CCD gain in e-/ADU
   float       p_ccdReadNoise;  // CCD read noise in e-
   float       p_ccdScaleNoise; // CCD scale noise (or sensitivity noise), dimensionless

   pcl_bool    p_clipLow;       // reject low pixels?
   pcl_bool    p_clipHigh;      // reject high pixels?

   pcl_bool    p_rangeClipLow;  // perform low range rejection?
   float       p_rangeLow;      // low range rejection point
   pcl_bool    p_rangeClipHigh; // perform high range rejection?
   float       p_rangeHigh;     // high range rejection point
   pcl_bool    p_reportRangeRejection; // count range rejected pixels in rejection summaries
   pcl_bool    p_mapRangeRejection;    // include range rejected pixels in rejection maps

   pcl_bool    p_largeScaleClipLow;       // perform low large-scale rejection?
   int32       p_largeScaleClipLowProtectedLayers; // low large-scale rejection, protected small-scale layers
   int32       p_largeScaleClipLowGrowth; // low large-scale rejection, growth of large-scale structures
   pcl_bool    p_largeScaleClipHigh;      // perform high large-scale rejection?
   int32       p_largeScaleClipHighProtectedLayers;// high large-scale rejection, protected small-scale layers
   int32       p_largeScaleClipHighGrowth;// high large-scale rejection, growth of large-scale structures

   pcl_bool    p_generate64BitResult;     // generate a 64-bit floating point result instead of 32-bit
   pcl_bool    p_generateRejectionMaps;   // generate pixel rejection map images
   pcl_bool    p_generateIntegratedImage; // generate integrated image, or only rejection maps
   pcl_bool    p_generateDrizzleData;     // append rejection and weight data to existing .drz files
   pcl_bool    p_closePreviousImages;     // close existing integration and map images before running

   int32       p_bufferSizeMB;         // size of a row buffer in megabytes
   int32       p_stackSizeMB;          // size of the pixel integration stack in megabytes

   pcl_bool    p_autoMemorySize;       // compute buffer and stack sizes automatically from available physical memory
   float       p_autoMemoryLimit;      // maximum fraction of available physical memory we can use

   pcl_bool    p_useROI;               // use a region of interest; entire image otherwise
   Rect        p_roi = Rect( 0 );      // region of interest

   pcl_bool    p_useCache;             // use the dynamic file cache

   pcl_bool    p_evaluateNoise;        // perform a MRS Gaussian noise estimation for the resulting image
   float       p_mrsMinDataFraction;   // minimum fraction of data for a valid MRS noise evaluation

   pcl_bool    p_subtractPedestals;    // subtract PEDESTAL keyword values from input images
   pcl_bool    p_truncateOnOutOfRange; // if the output image is out of [0,1], truncate instead of rescaling

   pcl_bool    p_noGUIMessages;        // ### DEPRECATED
   pcl_bool    p_showImages;           // since 1.8.8-6: can be set to false by scripts (e.g. WBPP)

   // High-level parallelism
   pcl_bool    p_useFileThreads;
   float       p_fileThreadOverload;

   // Buffer input threads
   pcl_bool    p_useBufferThreads;
   int32       p_maxBufferThreads;
   int         m_maxBufferThreads;

   /*
    * Read-only output properties
    */

   struct OutputData
   {
      // General integration data

      String     integrationImageId;       // identifier of the output integrated image
      String     lowRejectionMapImageId;   // identifier of the output low rejection map image
      String     highRejectionMapImageId;  // identifier of the output high rejection map image
      String     slopeMapImageId;          // identifier of the output slope map image
      int32      numberOfChannels          = 0; // number of nominal channels (1 or 3)
      uint64     numberOfPixels            = 0u; // area of the integrated image in pixels
      uint64     totalPixels               = 0u; // total integrated pixels (area*numberOfFiles)

      // Per-channel data for the final integrated image

      double     outputRangeLow            = 0; // output range, lower bound
      double     outputRangeHigh           = 0; // output range, upper bound
      UI64Vector totalRejectedLow          = UI64Vector( 0, 3 ); // low rejected pixels
      UI64Vector totalRejectedHigh         = UI64Vector( 0, 3 ); // high rejected pixels
      DVector    finalNoiseEstimates       = DVector( 0, 3 );    // noise estimates for the integrated image
      DVector    finalScaleEstimates       = DVector( 0, 3 );    // scale estimates for the integrated image
      DVector    finalLocationEstimates    = DVector( 0, 3 );    // location estimates for the integrated image
      FVector    referenceNoiseReductions  = FVector( 0, 3 );    // noise reduction w.r.t. the reference image
      FVector    medianNoiseReductions     = FVector( 0, 3 );    // median noise reduction
      FVector    referenceSNRIncrements    = FVector( 0, 3 );    // SNR increment w.r.t. the reference image
      FVector    averageSNRIncrements      = FVector( 0, 3 );    // average SNR increment

      // Per-channel data for each integrated image

      struct ImageData
      {
         FVector    weights      = FVector( 0, 3 );    // image weight (only if p_generateIntegratedImage)
         UI64Vector rejectedLow  = UI64Vector( 0, 3 ); // number of low rejected pixels
         UI64Vector rejectedHigh = UI64Vector( 0, 3 ); // number of high rejected pixels
      };

      Array<ImageData> imageData;
   };

   OutputData o_output;

   /*
    * Rejection data structures
    */

   struct RejectionDataItem
   {
      float value;                // scaled value
      float raw;                  // raw value
      int   index           : 28; // file index
      bool  rejectLow       :  1; // statistically rejected low pixel?
      bool  rejectHigh      :  1; // statistically rejected high pixel?
      bool  rejectRangeLow  :  1; // range rejected low pixel?
      bool  rejectRangeHigh :  1; // range rejected high pixel?

      void Set( float v, int i )
      {
         value = raw = IsFinite( v ) ? v : .0F;
         index = i;
         rejectLow = rejectHigh = rejectRangeLow = rejectRangeHigh = false;
      }

      bool IsStatisticallyRejected() const
      {
         return rejectLow || rejectHigh;
      }

      bool IsRangeRejected() const
      {
         return rejectRangeLow || rejectRangeHigh;
      }

      bool IsRejected() const
      {
         return IsStatisticallyRejected() || IsRangeRejected();
      }

      bool operator ==( const RejectionDataItem& x ) const
      {
         return index == x.index;
      }

      bool operator <( const RejectionDataItem& x ) const
      {
         return (IsRejected() == x.IsRejected()) ? value < x.value : !IsRejected();
      }

      static bool CompareRaw( const RejectionDataItem& a, const RejectionDataItem& b )
      {
         return a.raw < b.raw;
      }
   };

   typedef GenericMatrix<RejectionDataItem>  RejectionMatrix;
   typedef GenericVector<RejectionMatrix>    RejectionStacks;
   typedef GenericVector<IVector>            RejectionCounts;
   typedef GenericVector<FVector>            RejectionSlopes;

   /*
    * Auxiliary integration data and functions
    */

   typedef GenericVector<TwoSidedEstimate>   scale_estimates;

   DVector EvaluateNoise( const ImageVariant& ) const;
   DVector EvaluateSNR( const ImageVariant&, const DVector& noise ) const;

   ImageWindow CreateImageWindow( const IsoString& id, int bitsPerSample ) const;

   struct IntegrationDescriptionItems
   {
      String pixelCombination;
      String outputNormalization;
      String weightMode;
      String scaleEstimator;
      String rangeRejection;
      String pixelRejection;
      String rejectionNormalization;
      String rejectionClippings;
      String rejectionParameters;
      String largeScaleRejectionClippings;
      String largeScaleRejectionParameters;
      String regionOfInterest;

      IntegrationDescriptionItems( const ImageIntegrationInstance& );
   };

   String IntegrationDescription() const;

   friend class FileItem;
   friend class IntegrationFile;
   friend class IntegrationDataLoaderEngine;
   friend class IntegrationRejectionEngine;
   friend class IntegrationEngine;
   friend class MapIntegrationEngine;
   friend class RejectionMapGenerationEngine;
   friend class LargeScaleRejectionMapGenerationEngine;
   friend class ImageIntegrationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ImageIntegrationInstance_h

// ----------------------------------------------------------------------------
// EOF ImageIntegrationInstance.h - Released 2021-04-09T19:41:48Z
