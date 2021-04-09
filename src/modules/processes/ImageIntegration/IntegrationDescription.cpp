//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// IntegrationDescription.cpp - Released 2021-04-09T19:41:48Z
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

namespace pcl
{

// ----------------------------------------------------------------------------

inline static String YesNo( bool b )
{
   return String( b ? "yes" : "no" );
}

ImageIntegrationInstance::IntegrationDescriptionItems::IntegrationDescriptionItems( const ImageIntegrationInstance& instance )
{
   if ( instance.p_generateIntegratedImage )
   {
      switch ( instance.p_combination )
      {
      default:
      case IICombination::Average:
         pixelCombination = "Average";
         break;
      case IICombination::Median:
         pixelCombination = "Median";
         break;
      case IICombination::Minimum:
         pixelCombination = "Minimum";
         break;
      case IICombination::Maximum:
         pixelCombination = "Maximum";
         break;
      }

      switch ( instance.p_normalization )
      {
      case IINormalization::NoNormalization:
         outputNormalization = "None";
         break;
      default:
      case IINormalization::Additive:
         outputNormalization = "Additive";
         break;
      case IINormalization::Multiplicative:
         outputNormalization = "Multiplicative";
         break;
      case IINormalization::AdditiveWithScaling:
         outputNormalization = "Additive + scaling";
         break;
      case IINormalization::MultiplicativeWithScaling:
         outputNormalization = "Multiplicative + scaling";
         break;
      case IINormalization::LocalNormalization:
         outputNormalization = "Local";
         break;
      case IINormalization::AdaptiveNormalization:
         outputNormalization = String().Format( "Adaptive: grid_size=%d scale=",
                                                instance.p_adaptiveGridSize ) + YesNo( !instance.p_adaptiveNoScale );
         break;
      }
   }
   else
   {
      pixelCombination = "disabled";
   }

   if ( instance.p_combination == IICombination::Average )
      switch ( instance.p_weightMode )
      {
      case IIWeightMode::DontCare:
         weightMode = "Don't care";
         break;
      case IIWeightMode::ExposureTimeWeight:
         weightMode = "Exposure time";
         break;
      default:
      case IIWeightMode::NoiseEvaluationWeight:
         weightMode = "Noise evaluation";
         break;
      case IIWeightMode::SignalWeight:
         weightMode = "Average absolute deviation";
         break;
      case IIWeightMode::MedianWeight:
         weightMode = "Median sample value";
         break;
      case IIWeightMode::AverageWeight:
         weightMode = "Average sample value";
         break;
      case IIWeightMode::KeywordWeight:
         weightMode = "Custom keyword: " + instance.p_weightKeyword;
         break;
      }

   switch ( instance.p_weightScale )
   {
   case IIWeightScale::AvgDev:
      scaleEstimator = "Average absolute deviation from the median";
      break;
   case IIWeightScale::MAD:
      scaleEstimator = "Median absolute deviation from the median (MAD)";
      break;
   default:
   case IIWeightScale::BWMV:
      scaleEstimator = "Biweight midvariance";
      break;
   }

   if ( instance.p_rangeClipLow || instance.p_rangeClipHigh )
   {
      if ( instance.p_rangeClipLow )
         rangeRejection.Format( "range_low=%.6f", instance.p_rangeLow );
      if ( instance.p_rangeClipHigh )
      {
         if ( instance.p_rangeClipLow )
            rangeRejection << ' ';
         rangeRejection.AppendFormat( "range_high=%.6f", instance.p_rangeHigh );
      }
   }

   switch ( instance.p_rejection )
   {
   default:
   case IIRejection::NoRejection:
      pixelRejection = "None";
      break;
   case IIRejection::MinMax:
      pixelRejection = "Min/Max clipping";
      break;
   case IIRejection::PercentileClip:
      pixelRejection = "Percentile clipping";
      break;
   case IIRejection::SigmaClip:
      pixelRejection = "Sigma clipping";
      break;
   case IIRejection::WinsorizedSigmaClip:
      pixelRejection = "Winsorized sigma clipping";
      break;
   case IIRejection::AveragedSigmaClip:
      pixelRejection = "Averaged sigma clipping";
      break;
   case IIRejection::LinearFit:
      pixelRejection = "Linear fit clipping";
      break;
   case IIRejection::ESD:
      pixelRejection = "Generalized extreme Studentized deviate";
      break;
   case IIRejection::CCDClip:
      pixelRejection = "CCD noise model";
      break;
   }

   if ( instance.p_rejection != IIRejection::NoRejection )
   {
      switch ( instance.p_rejectionNormalization )
      {
      default: // ?!
      case IIRejectionNormalization::NoRejectionNormalization:
         rejectionNormalization = "None";
         break;
      case IIRejectionNormalization::Scale:
         rejectionNormalization = "Scale + zero offset";
         break;
      case IIRejectionNormalization::EqualizeFluxes:
         rejectionNormalization = "Equalize fluxes";
         break;
      case IIRejectionNormalization::LocalRejectionNormalization:
         rejectionNormalization = "Local";
         break;
      case IIRejectionNormalization::AdaptiveRejectionNormalization:
         rejectionNormalization = String().Format( "Adaptive: grid_size=%d scale=",
                                                   instance.p_adaptiveGridSize ) + YesNo( !instance.p_adaptiveNoScale );
         break;
      }

      rejectionClippings = "low=" + YesNo( instance.p_clipLow ) + " high=" + YesNo( instance.p_clipHigh );

      switch ( instance.p_rejection )
      {
      case IIRejection::MinMax:
         rejectionParameters.Format( "clip_low=%d clip_high=%d", instance.p_minMaxLow, instance.p_minMaxHigh );
         break;
      case IIRejection::PercentileClip:
         rejectionParameters.Format( "pc_low=%.3f pc_high=%.3f", instance.p_pcClipLow, instance.p_pcClipHigh );
         break;
      case IIRejection::SigmaClip:
      case IIRejection::WinsorizedSigmaClip:
      case IIRejection::AveragedSigmaClip:
         rejectionParameters.Format( "sigma_low=%.3f sigma_high=%.3f", instance.p_sigmaLow, instance.p_sigmaHigh );
         if ( instance.p_rejection == IIRejection::WinsorizedSigmaClip )
            rejectionParameters.AppendFormat( " cutoff=%.3f", instance.p_winsorizationCutoff );
         break;
      case IIRejection::LinearFit:
         rejectionParameters.Format( "lfit_low=%.3f lfit_high=%.3f", instance.p_linearFitLow, instance.p_linearFitHigh );
         break;
      case IIRejection::ESD:
         rejectionParameters.Format( "esd_outliers=%.2f esd_alpha=%.2f esd_low=%.2f",
                                     instance.p_esdOutliersFraction, instance.p_esdAlpha, instance.p_esdLowRelaxation );
         break;
      case IIRejection::CCDClip:
         rejectionParameters.Format( "gain=%.2f read_noise=%.2f scale_noise=%.2f",
                                     instance.p_ccdGain, instance.p_ccdReadNoise, instance.p_ccdScaleNoise );
         break;
      default: // ?!
         break;
      }
   }

   if ( instance.p_largeScaleClipLow || instance.p_largeScaleClipHigh )
   {
      largeScaleRejectionClippings = "low=" + YesNo( instance.p_largeScaleClipLow ) + " high=" + YesNo( instance.p_largeScaleClipHigh );
      largeScaleRejectionParameters.Format( "lsr_layers_low=%d lsr_grow_low=%d lsr_layers_high=%d lsr_grow_high=%d",
                                            instance.p_largeScaleClipLowProtectedLayers, instance.p_largeScaleClipLowGrowth,
                                            instance.p_largeScaleClipHighProtectedLayers, instance.p_largeScaleClipHighGrowth );
   }

   if ( instance.p_useROI )
      regionOfInterest.Format( "left=%d, top=%d, width=%d, height=%d",
                               instance.p_roi.x0, instance.p_roi.y0, instance.p_roi.Width(), instance.p_roi.Height() );
}

// ----------------------------------------------------------------------------

String ImageIntegrationInstance::IntegrationDescription() const
{
   IntegrationDescriptionItems items( *this );
   String description;
   if ( !items.pixelCombination.IsEmpty() )
      description <<   "Pixel combination .................. " << items.pixelCombination;
   if ( !items.outputNormalization.IsEmpty() )
      description << "\nOutput normalization ............... " << items.outputNormalization;
   if ( !items.weightMode.IsEmpty() )
      description << "\nWeighting mode ..................... " << items.weightMode;
   if ( !items.scaleEstimator.IsEmpty() )
      description << "\nScale estimator .................... " << items.scaleEstimator;
   if ( !items.rangeRejection.IsEmpty() )
      description << "\nRange rejection .................... " << items.rangeRejection;
   if ( !items.pixelRejection.IsEmpty() )
      description << "\nPixel rejection .................... " << items.pixelRejection;
   if ( !items.rejectionNormalization.IsEmpty() )
      description << "\nRejection normalization ............ " << items.rejectionNormalization;
   if ( !items.rejectionClippings.IsEmpty() )
      description << "\nRejection clippings ................ " << items.rejectionClippings;
   if ( !items.rejectionParameters.IsEmpty() )
      description << "\nRejection parameters ............... " << items.rejectionParameters;
   if ( !items.largeScaleRejectionClippings.IsEmpty() )
      description << "\nLarge-scale rejection clippings .... " << items.largeScaleRejectionClippings;
   if ( !items.largeScaleRejectionParameters.IsEmpty() )
      description << "\nLarge-scale rejection parameters ... " << items.largeScaleRejectionParameters;
   if ( !items.regionOfInterest.IsEmpty() )
      description << "\nRegion of interest ................. " << items.regionOfInterest;
   return description;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegrationDescription.cpp - Released 2021-04-09T19:41:48Z
