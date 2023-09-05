//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.9.8
// ----------------------------------------------------------------------------
// ImageCalibrationProcess.cpp - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "ImageCalibrationProcess.h"
#include "ImageCalibrationParameters.h"
#include "ImageCalibrationInstance.h"
#include "ImageCalibrationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ImageCalibrationProcess* TheImageCalibrationProcess = nullptr;

// ----------------------------------------------------------------------------

ImageCalibrationProcess::ImageCalibrationProcess()
{
   TheImageCalibrationProcess = this;

   // Instantiate process parameters
   new ICTargetFrames( this );
   new ICTargetFrameEnabled( TheICTargetFramesParameter );
   new ICTargetFramePath( TheICTargetFramesParameter );
   new ICEnableCFA( this );
   new ICCFAPattern( this );
   new ICInputHints( this );
   new ICOutputHints( this );
   new ICPedestal( this );
   new ICPedestalMode( this );
   new ICPedestalKeyword( this );
   new ICOverscanEnabled( this );
   new ICOverscanImageX0( this );
   new ICOverscanImageY0( this );
   new ICOverscanImageX1( this );
   new ICOverscanImageY1( this );
   new ICOverscanRegions( this );
   new ICOverscanRegionEnabled( TheICOverscanRegionsParameter );
   new ICOverscanSourceX0( TheICOverscanRegionsParameter );
   new ICOverscanSourceY0( TheICOverscanRegionsParameter );
   new ICOverscanSourceX1( TheICOverscanRegionsParameter );
   new ICOverscanSourceY1( TheICOverscanRegionsParameter );
   new ICOverscanTargetX0( TheICOverscanRegionsParameter );
   new ICOverscanTargetY0( TheICOverscanRegionsParameter );
   new ICOverscanTargetX1( TheICOverscanRegionsParameter );
   new ICOverscanTargetY1( TheICOverscanRegionsParameter );
   new ICMasterBiasEnabled( this );
   new ICMasterBiasPath( this );
   new ICMasterDarkEnabled( this );
   new ICMasterDarkPath( this );
   new ICMasterFlatEnabled( this );
   new ICMasterFlatPath( this );
   new ICCalibrateBias( this );
   new ICCalibrateDark( this );
   new ICCalibrateFlat( this );
   new ICOptimizeDarks( this );
   new ICDarkOptimizationThreshold( this );
   new ICDarkOptimizationLow( this );
   new ICDarkOptimizationWindow( this );
   new ICDarkCFADetectionMode( this );
   new ICSeparateCFAFlatScalingFactors( this );
   new ICFlatScaleClippingFactor( this );
   new ICEvaluateNoise( this );
   new ICNoiseEvaluationAlgorithm( this );
   new ICEvaluateSignal( this );
   new ICStructureLayers( this );
   new ICSaturationThreshold( this );
   new ICSaturationRelative( this );
   new ICNoiseLayers( this );
   new ICHotPixelFilterRadius( this );
   new ICNoiseReductionFilterRadius( this );
   new ICMinStructureSize( this );
   new ICPSFType( this );
   new ICPSFGrowth( this );
   new ICMaxStars( this );
   new ICOutputDirectory( this );
   new ICOutputExtension( this );
   new ICOutputPrefix( this );
   new ICOutputPostfix( this );
   new ICOutputSampleFormat( this );
   new ICOutputPedestal( this );
   new ICOutputPedestalMode( this );
   new ICAutoPedestalLimit( this );
   new ICOverwriteExistingFiles( this );
   new ICOnError( this );
   new ICNoGUIMessages( this );
   new ICUseFileThreads( this );
   new ICFileThreadOverload( this );
   new ICMaxFileReadThreads( this );
   new ICMaxFileWriteThreads( this );

   new ICOutputData( this );
   new ICOutputFilePath( TheICOutputDataParameter );
   new ICDarkScalingFactorRK( TheICOutputDataParameter );
   new ICDarkScalingFactorG( TheICOutputDataParameter );
   new ICDarkScalingFactorB( TheICOutputDataParameter );
   new ICPSFTotalFluxEstimateRK( TheICOutputDataParameter );
   new ICPSFTotalFluxEstimateG( TheICOutputDataParameter );
   new ICPSFTotalFluxEstimateB( TheICOutputDataParameter );
   new ICPSFTotalPowerFluxEstimateRK( TheICOutputDataParameter );
   new ICPSFTotalPowerFluxEstimateG( TheICOutputDataParameter );
   new ICPSFTotalPowerFluxEstimateB( TheICOutputDataParameter );
   new ICPSFTotalMeanFluxEstimateRK( TheICOutputDataParameter );
   new ICPSFTotalMeanFluxEstimateG( TheICOutputDataParameter );
   new ICPSFTotalMeanFluxEstimateB( TheICOutputDataParameter );
   new ICPSFTotalMeanPowerFluxEstimateRK( TheICOutputDataParameter );
   new ICPSFTotalMeanPowerFluxEstimateG( TheICOutputDataParameter );
   new ICPSFTotalMeanPowerFluxEstimateB( TheICOutputDataParameter );
   new ICPSFMStarEstimateRK( TheICOutputDataParameter );
   new ICPSFMStarEstimateG( TheICOutputDataParameter );
   new ICPSFMStarEstimateB( TheICOutputDataParameter );
   new ICPSFNStarEstimateRK( TheICOutputDataParameter );
   new ICPSFNStarEstimateG( TheICOutputDataParameter );
   new ICPSFNStarEstimateB( TheICOutputDataParameter );
   new ICPSFCountRK( TheICOutputDataParameter );
   new ICPSFCountG( TheICOutputDataParameter );
   new ICPSFCountB( TheICOutputDataParameter );
   new ICNoiseEstimateRK( TheICOutputDataParameter );
   new ICNoiseEstimateG( TheICOutputDataParameter );
   new ICNoiseEstimateB( TheICOutputDataParameter );
   new ICNoiseFractionRK( TheICOutputDataParameter );
   new ICNoiseFractionG( TheICOutputDataParameter );
   new ICNoiseFractionB( TheICOutputDataParameter );
   new ICNoiseScaleLowRK( TheICOutputDataParameter );
   new ICNoiseScaleLowG( TheICOutputDataParameter );
   new ICNoiseScaleLowB( TheICOutputDataParameter );
   new ICNoiseScaleHighRK( TheICOutputDataParameter );
   new ICNoiseScaleHighG( TheICOutputDataParameter );
   new ICNoiseScaleHighB( TheICOutputDataParameter );
   new ICNoiseAlgorithmRK( TheICOutputDataParameter );
   new ICNoiseAlgorithmG( TheICOutputDataParameter );
   new ICNoiseAlgorithmB( TheICOutputDataParameter );
}

// ----------------------------------------------------------------------------

IsoString ImageCalibrationProcess::Id() const
{
   return "ImageCalibration";
}

// ----------------------------------------------------------------------------

IsoString ImageCalibrationProcess::Category() const
{
   return "ImageCalibration,Preprocessing";
}

// ----------------------------------------------------------------------------

uint32 ImageCalibrationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ImageCalibrationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ImageCalibration.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* ImageCalibrationProcess::DefaultInterface() const
{
   return TheImageCalibrationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ImageCalibrationProcess::Create() const
{
   return new ImageCalibrationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ImageCalibrationProcess::Clone( const ProcessImplementation& p ) const
{
   const ImageCalibrationInstance* instPtr = dynamic_cast<const ImageCalibrationInstance*>( &p );
   return (instPtr != nullptr) ? new ImageCalibrationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageCalibrationProcess.cpp - Released 2023-08-28T15:23:41Z
