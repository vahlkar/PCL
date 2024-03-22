//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.9
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.5
// ----------------------------------------------------------------------------
// SpectrophotometricColorCalibrationProcess.cpp - Released 2024-03-20T10:42:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2024 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "SpectrophotometricColorCalibrationProcess.h"
#include "SpectrophotometricColorCalibrationParameters.h"
#include "SpectrophotometricColorCalibrationInstance.h"
#include "SpectrophotometricColorCalibrationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SpectrophotometricColorCalibrationProcess* TheSpectrophotometricColorCalibrationProcess = nullptr;

// ----------------------------------------------------------------------------

SpectrophotometricColorCalibrationProcess::SpectrophotometricColorCalibrationProcess()
{
   TheSpectrophotometricColorCalibrationProcess = this;

   new SPCCApplyCalibration( this );
   new SPCCNarrowbandMode( this );
   new SPCCNarrowbandOptimizeStars( this );
   new SPCCWhiteReferenceSpectrum( this );
   new SPCCWhiteReferenceName( this );
   new SPCCRedFilterTrCurve( this );
   new SPCCRedFilterName( this );
   new SPCCGreenFilterTrCurve( this );
   new SPCCGreenFilterName( this );
   new SPCCBlueFilterTrCurve( this );
   new SPCCBlueFilterName( this );
   new SPCCRedFilterWavelength( this );
   new SPCCRedFilterBandwidth( this );
   new SPCCGreenFilterWavelength( this );
   new SPCCGreenFilterBandwidth( this );
   new SPCCBlueFilterWavelength( this );
   new SPCCBlueFilterBandwidth( this );
   new SPCCDeviceQECurve( this );
   new SPCCDeviceQECurveName( this );
   new SPCCBroadbandIntegrationStepSize( this );
   new SPCCNarrowbandIntegrationSteps( this );
   new SPCCCatalogId( this );
   new SPCCLimitMagnitude( this );
   new SPCCAutoLimitMagnitude( this );
   new SPCCTargetSourceCount( this );
   new SPCCStructureLayers( this );
   new SPCCSaturationThreshold( this );
   new SPCCSaturationRelative( this );
   new SPCCSaturationShrinkFactor( this );
   new SPCCPSFNoiseLayers( this );
   new SPCCPSFHotPixelFilterRadius( this );
   new SPCCPSFNoiseReductionFilterRadius( this );
   new SPCCPSFMinStructureSize( this );
   new SPCCPSFMinSNR( this );
   new SPCCPSFAllowClusteredSources( this );
   new SPCCPSFType( this );
   new SPCCPSFGrowth( this );
   new SPCCPSFMaxStars( this );
   new SPCCPSFSearchTolerance( this );
   new SPCCPSFChannelSearchTolerance( this );
   new SPCCNeutralizeBackground( this );
   new SPCCBackgroundReferenceViewId( this );
   new SPCCBackgroundLow( this );
   new SPCCBackgroundHigh( this );
   new SPCCBackgroundUseROI( this );
   new SPCCBackgroundROIX0( this );
   new SPCCBackgroundROIY0( this );
   new SPCCBackgroundROIX1( this );
   new SPCCBackgroundROIY1( this );
   new SPCCGenerateGraphs( this );
   new SPCCGenerateStarMaps( this );
   new SPCCGenerateTextFiles( this );
   new SPCCOutputDirectory( this );
}

// ----------------------------------------------------------------------------

IsoString SpectrophotometricColorCalibrationProcess::Id() const
{
   return "SpectrophotometricColorCalibration";
}

// ----------------------------------------------------------------------------

IsoString SpectrophotometricColorCalibrationProcess::Category() const
{
   return "ColorCalibration,Photometry,Spectrophotometry";
}

// ----------------------------------------------------------------------------

uint32 SpectrophotometricColorCalibrationProcess::Version() const
{
   return 1; // November 2022
}

// ----------------------------------------------------------------------------

String SpectrophotometricColorCalibrationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String SpectrophotometricColorCalibrationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/SpectrophotometricColorCalibration.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* SpectrophotometricColorCalibrationProcess::DefaultInterface() const
{
   return TheSpectrophotometricColorCalibrationInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* SpectrophotometricColorCalibrationProcess::Create() const
{
   return new SpectrophotometricColorCalibrationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SpectrophotometricColorCalibrationProcess::Clone( const ProcessImplementation& p ) const
{
   const SpectrophotometricColorCalibrationInstance* instPtr = dynamic_cast<const SpectrophotometricColorCalibrationInstance*>( &p );
   return (instPtr != nullptr) ? new SpectrophotometricColorCalibrationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SpectrophotometricColorCalibrationProcess.cpp - Released 2024-03-20T10:42:12Z
