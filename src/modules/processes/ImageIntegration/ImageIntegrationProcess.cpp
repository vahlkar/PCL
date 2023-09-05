//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.5.2
// ----------------------------------------------------------------------------
// ImageIntegrationProcess.cpp - Released 2023-08-28T15:23:41Z
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
#include "ImageIntegrationInterface.h"
#include "ImageIntegrationParameters.h"
#include "ImageIntegrationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ImageIntegrationProcess* TheImageIntegrationProcess = nullptr;

// ----------------------------------------------------------------------------

ImageIntegrationProcess::ImageIntegrationProcess()
{
   TheImageIntegrationProcess = this;

   new IIImages( this );
   new IIImageEnabled( TheIIImagesParameter );
   new IIImagePath( TheIIImagesParameter );
   new IIDrizzlePath( TheIIImagesParameter );
   new IILocalNormalizationDataPath( TheIIImagesParameter );
   new IIInputHints( this );
   new IICombination( this );
   new IIWeightMode( this );
   new IIWeightKeyword( this );
   new IIWeightScale( this );
   new IIMinWeight( this );
   new IICSVWeights( this );
   new IIAdaptiveGridSize( this );
   new IIAdaptiveNoScale( this );
   new IIIgnoreNoiseKeywords( this );
   new IINormalization( this );
   new IIRejection( this );
   new IIRejectionNormalization( this );
   new IIMinMaxLow( this );
   new IIMinMaxHigh( this );
   new IIPercentileLow( this );
   new IIPercentileHigh( this );
   new IISigmaLow( this );
   new IISigmaHigh( this );
   new IIWinsorizationCutoff( this );
   new IILinearFitLow( this );
   new IILinearFitHigh( this );
   new IIESDOutliersFraction( this );
   new IIESDAlpha( this );
   new IIESDLowRelaxation( this );
   new IIRCRLimit( this );
   new IICCDGain( this );
   new IICCDReadNoise( this );
   new IICCDScaleNoise( this );
   new IIClipLow( this );
   new IIClipHigh( this );
   new IIRangeClipLow( this );
   new IIRangeLow( this );
   new IIRangeClipHigh( this );
   new IIRangeHigh( this );
   new IIMapRangeRejection( this );
   new IIReportRangeRejection( this );
   new IILargeScaleClipLow( this );
   new IILargeScaleClipLowProtectedLayers( this );
   new IILargeScaleClipLowGrowth( this );
   new IILargeScaleClipHigh( this );
   new IILargeScaleClipHighProtectedLayers( this );
   new IILargeScaleClipHighGrowth( this );
   new IIGenerate64BitResult( this );
   new IIGenerateRejectionMaps( this );
   new IIGenerateIntegratedImage( this );
   new IIGenerateDrizzleData( this );
   new IIClosePreviousImages( this );
   new IIBufferSize( this );
   new IIStackSize( this );
   new IIAutoMemorySize( this );
   new IIAutoMemoryLimit( this );
   new IIUseROI( this );
   new IIROIX0( this );
   new IIROIY0( this );
   new IIROIX1( this );
   new IIROIY1( this );
   new IIUseCache( this );
   new IIEvaluateSNR( this );
   new IINoiseEvaluationAlgorithm( this );
   new IIMRSMinDataFraction( this );
   new IIPSFStructureLayers( this );
   new IIPSFType( this );
   new IISubtractPedestals( this );
   new IITruncateOnOutOfRange( this );
   new IINoGUIMessages( this );
   new IIShowImages( this );
   new IIUseFileThreads( this );
   new IIFileThreadOverload( this );
   new IIUseBufferThreads( this );
   new IIMaxBufferThreads( this );

   new IIIntegrationImageId( this );

   new IILowRejectionMapImageId( this );
   new IIHighRejectionMapImageId( this );
   new IISlopeMapImageId( this );

   new IINumberOfChannels( this );
   new IINumberOfPixels( this );
   new IITotalPixels( this );

   new IIOutputRangeLow( this );
   new IIOutputRangeHigh( this );

   new IITotalRejectedLowRK( this );
   new IITotalRejectedLowG( this );
   new IITotalRejectedLowB( this );
   new IITotalRejectedHighRK( this );
   new IITotalRejectedHighG( this );
   new IITotalRejectedHighB( this );

   new IIFinalNoiseEstimateRK( this );
   new IIFinalNoiseEstimateG( this );
   new IIFinalNoiseEstimateB( this );
   new IIFinalNoiseScaleEstimateLowRK( this );
   new IIFinalNoiseScaleEstimateLowG( this );
   new IIFinalNoiseScaleEstimateLowB( this );
   new IIFinalNoiseScaleEstimateHighRK( this );
   new IIFinalNoiseScaleEstimateHighG( this );
   new IIFinalNoiseScaleEstimateHighB( this );
   new IIFinalNoiseAlgorithmRK( this );
   new IIFinalNoiseAlgorithmG( this );
   new IIFinalNoiseAlgorithmB( this );

   new IIFinalScaleEstimateRK( this );
   new IIFinalScaleEstimateG( this );
   new IIFinalScaleEstimateB( this );

   new IIFinalLocationEstimateRK( this );
   new IIFinalLocationEstimateG( this );
   new IIFinalLocationEstimateB( this );

   new IIFinalPSFTotalFluxEstimateRK( this );
   new IIFinalPSFTotalFluxEstimateG( this );
   new IIFinalPSFTotalFluxEstimateB( this );
   new IIFinalPSFTotalPowerFluxEstimateRK( this );
   new IIFinalPSFTotalPowerFluxEstimateG( this );
   new IIFinalPSFTotalPowerFluxEstimateB( this );
   new IIFinalPSFTotalMeanFluxEstimateRK( this );
   new IIFinalPSFTotalMeanFluxEstimateG( this );
   new IIFinalPSFTotalMeanFluxEstimateB( this );
   new IIFinalPSFTotalMeanPowerFluxEstimateRK( this );
   new IIFinalPSFTotalMeanPowerFluxEstimateG( this );
   new IIFinalPSFTotalMeanPowerFluxEstimateB( this );
   new IIFinalPSFMStarEstimateRK( this );
   new IIFinalPSFMStarEstimateG( this );
   new IIFinalPSFMStarEstimateB( this );
   new IIFinalPSFNStarEstimateRK( this );
   new IIFinalPSFNStarEstimateG( this );
   new IIFinalPSFNStarEstimateB( this );
   new IIFinalPSFCountRK( this );
   new IIFinalPSFCountG( this );
   new IIFinalPSFCountB( this );

   new IIReferenceNoiseReductionRK( this );
   new IIReferenceNoiseReductionG( this );
   new IIReferenceNoiseReductionB( this );
   new IIMedianNoiseReductionRK( this );
   new IIMedianNoiseReductionG( this );
   new IIMedianNoiseReductionB( this );
   new IIReferenceSNRIncrementRK( this );
   new IIReferenceSNRIncrementG( this );
   new IIReferenceSNRIncrementB( this );
   new IIAverageSNRIncrementRK( this );
   new IIAverageSNRIncrementG( this );
   new IIAverageSNRIncrementB( this );

   new IIImageData( this );
   new IIImageWeightRK( TheIIImageDataParameter );
   new IIImageWeightG( TheIIImageDataParameter );
   new IIImageWeightB( TheIIImageDataParameter );
   new IIImageRejectedLowRK( TheIIImageDataParameter );
   new IIImageRejectedLowG( TheIIImageDataParameter );
   new IIImageRejectedLowB( TheIIImageDataParameter );
   new IIImageRejectedHighRK( TheIIImageDataParameter );
   new IIImageRejectedHighG( TheIIImageDataParameter );
   new IIImageRejectedHighB( TheIIImageDataParameter );
}

// ----------------------------------------------------------------------------

IsoString ImageIntegrationProcess::Id() const
{
   return "ImageIntegration";
}

// ----------------------------------------------------------------------------

IsoString ImageIntegrationProcess::Category() const
{
   return "ImageIntegration,Preprocessing";
}

// ----------------------------------------------------------------------------

uint32 ImageIntegrationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ImageIntegrationProcess::Description() const
{
   return

   "";
}

// ----------------------------------------------------------------------------

String ImageIntegrationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ImageIntegration.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* ImageIntegrationProcess::DefaultInterface() const
{
   return TheImageIntegrationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ImageIntegrationProcess::Create() const
{
   return new ImageIntegrationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ImageIntegrationProcess::Clone( const ProcessImplementation& p ) const
{
   const ImageIntegrationInstance* instPtr = dynamic_cast<const ImageIntegrationInstance*>( &p );
   return (instPtr != nullptr) ? new ImageIntegrationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageIntegrationProcess.cpp - Released 2023-08-28T15:23:41Z
