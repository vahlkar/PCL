//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 2.2.4
// ----------------------------------------------------------------------------
// SpectrophotometricFluxCalibrationProcess.cpp - Released 2024-08-02T18:17:27Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "SpectrophotometricFluxCalibrationInterface.h"
#include "SpectrophotometricFluxCalibrationParameters.h"
#include "SpectrophotometricFluxCalibrationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SpectrophotometricFluxCalibrationProcess* TheSpectrophotometricFluxCalibrationProcess = nullptr;

// ----------------------------------------------------------------------------

SpectrophotometricFluxCalibrationProcess::SpectrophotometricFluxCalibrationProcess()
{
   TheSpectrophotometricFluxCalibrationProcess = this;

   new SPFCNarrowbandMode( this );
   new SPFCGrayFilterTrCurve( this );
   new SPFCGrayFilterName( this );
   new SPFCRedFilterTrCurve( this );
   new SPFCRedFilterName( this );
   new SPFCGreenFilterTrCurve( this );
   new SPFCGreenFilterName( this );
   new SPFCBlueFilterTrCurve( this );
   new SPFCBlueFilterName( this );
   new SPFCGrayFilterWavelength( this );
   new SPFCGrayFilterBandwidth( this );
   new SPFCRedFilterWavelength( this );
   new SPFCRedFilterBandwidth( this );
   new SPFCGreenFilterWavelength( this );
   new SPFCGreenFilterBandwidth( this );
   new SPFCBlueFilterWavelength( this );
   new SPFCBlueFilterBandwidth( this );
   new SPFCDeviceQECurve( this );
   new SPFCDeviceQECurveName( this );
   new SPFCBroadbandIntegrationStepSize( this );
   new SPFCNarrowbandIntegrationSteps( this );
   new SPFCRejectionLimit( this );
   new SPFCCatalogId( this );
   new SPFCMinMagnitude( this );
   new SPFCLimitMagnitude( this );
   new SPFCAutoLimitMagnitude( this );
   new SPFCStructureLayers( this );
   new SPFCSaturationThreshold( this );
   new SPFCSaturationRelative( this );
   new SPFCSaturationShrinkFactor( this );
   new SPFCPSFNoiseLayers( this );
   new SPFCPSFHotPixelFilterRadius( this );
   new SPFCPSFNoiseReductionFilterRadius( this );
   new SPFCPSFMinStructureSize( this );
   new SPFCPSFMinSNR( this );
   new SPFCPSFAllowClusteredSources( this );
   new SPFCPSFType( this );
   new SPFCPSFGrowth( this );
   new SPFCPSFMaxStars( this );
   new SPFCPSFSearchTolerance( this );
   new SPFCPSFChannelSearchTolerance( this );
   new SPFCGenerateGraphs( this );
   new SPFCGenerateStarMaps( this );
   new SPFCGenerateTextFiles( this );
   new SPFCOutputDirectory( this );
}

// ----------------------------------------------------------------------------

IsoString SpectrophotometricFluxCalibrationProcess::Id() const
{
   return "SpectrophotometricFluxCalibration";
}

// ----------------------------------------------------------------------------

IsoString SpectrophotometricFluxCalibrationProcess::Category() const
{
   return "ImageCalibration,Preprocessing,Photometry,Spectrophotometry";
}

// ----------------------------------------------------------------------------

uint32 SpectrophotometricFluxCalibrationProcess::Version() const
{
   return 1;
}

// ----------------------------------------------------------------------------

String SpectrophotometricFluxCalibrationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String SpectrophotometricFluxCalibrationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/SpectrophotometricFluxCalibration.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* SpectrophotometricFluxCalibrationProcess::DefaultInterface() const
{
   return TheSpectrophotometricFluxCalibrationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* SpectrophotometricFluxCalibrationProcess::Create() const
{
   return new SpectrophotometricFluxCalibrationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SpectrophotometricFluxCalibrationProcess::Clone( const ProcessImplementation& p ) const
{
   const SpectrophotometricFluxCalibrationInstance* instPtr = dynamic_cast<const SpectrophotometricFluxCalibrationInstance*>( &p );
   return (instPtr != nullptr) ? new SpectrophotometricFluxCalibrationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SpectrophotometricFluxCalibrationProcess.cpp - Released 2024-08-02T18:17:27Z
