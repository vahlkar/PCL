//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.7
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// SpectrophotometricColorCalibrationParameters.cpp - Released 2023-08-01T16:30:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "SpectrophotometricColorCalibrationParameters.h"
#include "SpectrophotometricColorCalibrationProcess.h"

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SPCCApplyCalibration*              TheSPCCApplyCalibrationParameter = nullptr;
SPCCNarrowbandMode*                TheSPCCNarrowbandModeParameter = nullptr;
SPCCNarrowbandOptimizeStars*       TheSPCCNarrowbandOptimizeStarsParameter = nullptr;

SPCCWhiteReferenceSpectrum*        TheSPCCWhiteReferenceSpectrumParameter = nullptr;
SPCCWhiteReferenceName*            TheSPCCWhiteReferenceNameParameter = nullptr;
SPCCRedFilterTrCurve*              TheSPCCRedFilterTrCurveParameter = nullptr;
SPCCRedFilterName*                 TheSPCCRedFilterNameParameter = nullptr;
SPCCGreenFilterTrCurve*            TheSPCCGreenFilterTrCurveParameter = nullptr;
SPCCGreenFilterName*               TheSPCCGreenFilterNameParameter = nullptr;
SPCCBlueFilterTrCurve*             TheSPCCBlueFilterTrCurveParameter = nullptr;
SPCCBlueFilterName*                TheSPCCBlueFilterNameParameter = nullptr;

SPCCRedFilterWavelength*           TheSPCCRedFilterWavelengthParameter = nullptr;
SPCCRedFilterBandwidth*            TheSPCCRedFilterBandwidthParameter = nullptr;
SPCCGreenFilterWavelength*         TheSPCCGreenFilterWavelengthParameter = nullptr;
SPCCGreenFilterBandwidth*          TheSPCCGreenFilterBandwidthParameter = nullptr;
SPCCBlueFilterWavelength*          TheSPCCBlueFilterWavelengthParameter = nullptr;
SPCCBlueFilterBandwidth*           TheSPCCBlueFilterBandwidthParameter = nullptr;

SPCCDeviceQECurve*                 TheSPCCDeviceQECurveParameter = nullptr;
SPCCDeviceQECurveName*             TheSPCCDeviceQECurveNameParameter = nullptr;

SPCCBroadbandIntegrationStepSize*  TheSPCCBroadbandIntegrationStepSizeParameter = nullptr;
SPCCNarrowbandIntegrationSteps*    TheSPCCNarrowbandIntegrationStepsParameter = nullptr;

SPCCCatalogId*                     TheSPCCCatalogIdParameter = nullptr;
SPCCLimitMagnitude*                TheSPCCLimitMagnitudeParameter = nullptr;
SPCCAutoLimitMagnitude*            TheSPCCAutoLimitMagnitudeParameter = nullptr;
SPCCTargetSourceCount*             TheSPCCTargetSourceCountParameter = nullptr;

SPCCStructureLayers*               TheSPCCStructureLayersParameter = nullptr;
SPCCSaturationThreshold*           TheSPCCSaturationThresholdParameter = nullptr;
SPCCSaturationRelative*            TheSPCCSaturationRelativeParameter = nullptr;
SPCCSaturationShrinkFactor*        TheSPCCSaturationShrinkFactorParameter = nullptr;
SPCCPSFNoiseLayers*                TheSPCCPSFNoiseLayersParameter = nullptr;
SPCCPSFHotPixelFilterRadius*       TheSPCCPSFHotPixelFilterRadiusParameter = nullptr;
SPCCPSFNoiseReductionFilterRadius* TheSPCCPSFNoiseReductionFilterRadiusParameter = nullptr;
SPCCPSFMinStructureSize*           TheSPCCPSFMinStructureSizeParameter = nullptr;
SPCCPSFMinSNR*                     TheSPCCPSFMinSNRParameter = nullptr;
SPCCPSFAllowClusteredSources*      TheSPCCPSFAllowClusteredSourcesParameter = nullptr;
SPCCPSFType*                       TheSPCCPSFTypeParameter = nullptr;
SPCCPSFGrowth*                     TheSPCCPSFGrowthParameter = nullptr;
SPCCPSFMaxStars*                   TheSPCCPSFMaxStarsParameter = nullptr;
SPCCPSFSearchTolerance*            TheSPCCPSFSearchToleranceParameter = nullptr;
SPCCPSFChannelSearchTolerance*     TheSPCCPSFChannelSearchToleranceParameter = nullptr;

SPCCNeutralizeBackground*          TheSPCCNeutralizeBackgroundParameter = nullptr;
SPCCBackgroundReferenceViewId*     TheSPCCBackgroundReferenceViewIdParameter = nullptr;
SPCCBackgroundLow*                 TheSPCCBackgroundLowParameter = nullptr;
SPCCBackgroundHigh*                TheSPCCBackgroundHighParameter = nullptr;
SPCCBackgroundUseROI*              TheSPCCBackgroundUseROIParameter = nullptr;
SPCCBackgroundROIX0*               TheSPCCBackgroundROIX0Parameter = nullptr;
SPCCBackgroundROIY0*               TheSPCCBackgroundROIY0Parameter = nullptr;
SPCCBackgroundROIX1*               TheSPCCBackgroundROIX1Parameter = nullptr;
SPCCBackgroundROIY1*               TheSPCCBackgroundROIY1Parameter = nullptr;

SPCCGenerateGraphs*                TheSPCCGenerateGraphsParameter = nullptr;
SPCCGenerateStarMaps*              TheSPCCGenerateStarMapsParameter = nullptr;
SPCCGenerateTextFiles*             TheSPCCGenerateTextFilesParameter = nullptr;
SPCCOutputDirectory*               TheSPCCOutputDirectoryParameter = nullptr;

// ----------------------------------------------------------------------------

SPCCApplyCalibration::SPCCApplyCalibration( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPCCApplyCalibrationParameter = this;
}

IsoString SPCCApplyCalibration::Id() const
{
   return "applyCalibration";
}

bool SPCCApplyCalibration::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SPCCNarrowbandMode::SPCCNarrowbandMode( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPCCNarrowbandModeParameter = this;
}

IsoString SPCCNarrowbandMode::Id() const
{
   return "narrowbandMode";
}

bool SPCCNarrowbandMode::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SPCCNarrowbandOptimizeStars::SPCCNarrowbandOptimizeStars( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPCCNarrowbandOptimizeStarsParameter = this;
}

IsoString SPCCNarrowbandOptimizeStars::Id() const
{
   return "narrowbandOptimizeStars";
}

bool SPCCNarrowbandOptimizeStars::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SPCCWhiteReferenceSpectrum::SPCCWhiteReferenceSpectrum( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCWhiteReferenceSpectrumParameter = this;
}

IsoString SPCCWhiteReferenceSpectrum::Id() const
{
   return "whiteReferenceSpectrum";
}

String SPCCWhiteReferenceSpectrum::DefaultValue() const
{
   if ( API )
      if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
         return SpectrophotometricColorCalibrationProcess::DefaultWhiteReference().data;
   return String();
}

// ----------------------------------------------------------------------------

SPCCWhiteReferenceName::SPCCWhiteReferenceName( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCWhiteReferenceNameParameter = this;
}

IsoString SPCCWhiteReferenceName::Id() const
{
   return "whiteReferenceName";
}

String SPCCWhiteReferenceName::DefaultValue() const
{
   if ( API )
      if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
         return SpectrophotometricColorCalibrationProcess::DefaultWhiteReference().name;
   return String();
}

// ----------------------------------------------------------------------------

SPCCRedFilterTrCurve::SPCCRedFilterTrCurve( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCRedFilterTrCurveParameter = this;
}

IsoString SPCCRedFilterTrCurve::Id() const
{
   return "redFilterTrCurve";
}

String SPCCRedFilterTrCurve::DefaultValue() const
{
   if ( API )
      if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
         return SpectrophotometricColorCalibrationProcess::DefaultRedFilter().data;
   return String();
}

// ----------------------------------------------------------------------------

SPCCRedFilterName::SPCCRedFilterName( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCRedFilterNameParameter = this;
}

IsoString SPCCRedFilterName::Id() const
{
   return "redFilterName";
}

String SPCCRedFilterName::DefaultValue() const
{
   if ( API )
      if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
         return SpectrophotometricColorCalibrationProcess::DefaultRedFilter().name;
   return String();
}

// ----------------------------------------------------------------------------

SPCCGreenFilterTrCurve::SPCCGreenFilterTrCurve( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCGreenFilterTrCurveParameter = this;
}

IsoString SPCCGreenFilterTrCurve::Id() const
{
   return "greenFilterTrCurve";
}

String SPCCGreenFilterTrCurve::DefaultValue() const
{
   if ( API )
      if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
         return SpectrophotometricColorCalibrationProcess::DefaultGreenFilter().data;
   return String();
}

// ----------------------------------------------------------------------------

SPCCGreenFilterName::SPCCGreenFilterName( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCGreenFilterNameParameter = this;
}

IsoString SPCCGreenFilterName::Id() const
{
   return "greenFilterName";
}

String SPCCGreenFilterName::DefaultValue() const
{
   if ( API )
      if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
         return SpectrophotometricColorCalibrationProcess::DefaultGreenFilter().name;
   return String();
}

// ----------------------------------------------------------------------------

SPCCBlueFilterTrCurve::SPCCBlueFilterTrCurve( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCBlueFilterTrCurveParameter = this;
}

IsoString SPCCBlueFilterTrCurve::Id() const
{
   return "blueFilterTrCurve";
}

String SPCCBlueFilterTrCurve::DefaultValue() const
{
   if ( API )
      if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
         return SpectrophotometricColorCalibrationProcess::DefaultBlueFilter().data;
   return String();
}

// ----------------------------------------------------------------------------

SPCCBlueFilterName::SPCCBlueFilterName( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCBlueFilterNameParameter = this;
}

IsoString SPCCBlueFilterName::Id() const
{
   return "blueFilterName";
}

String SPCCBlueFilterName::DefaultValue() const
{
   if ( API )
      if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
         return SpectrophotometricColorCalibrationProcess::DefaultBlueFilter().name;
   return String();
}

// ----------------------------------------------------------------------------

SPCCRedFilterWavelength::SPCCRedFilterWavelength( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCRedFilterWavelengthParameter = this;
}

IsoString SPCCRedFilterWavelength::Id() const
{
   return "redFilterWavelength";
}

int SPCCRedFilterWavelength::Precision() const
{
   return 1;
}

double SPCCRedFilterWavelength::DefaultValue() const
{
   return 656.3; // nm
}

// ----------------------------------------------------------------------------

SPCCRedFilterBandwidth::SPCCRedFilterBandwidth( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCRedFilterBandwidthParameter = this;
}

IsoString SPCCRedFilterBandwidth::Id() const
{
   return "redFilterBandwidth";
}

int SPCCRedFilterBandwidth::Precision() const
{
   return 1;
}

double SPCCRedFilterBandwidth::DefaultValue() const
{
   return 3.0; // nm
}

// ----------------------------------------------------------------------------

SPCCGreenFilterWavelength::SPCCGreenFilterWavelength( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCGreenFilterWavelengthParameter = this;
}

IsoString SPCCGreenFilterWavelength::Id() const
{
   return "greenFilterWavelength";
}

int SPCCGreenFilterWavelength::Precision() const
{
   return 1;
}

double SPCCGreenFilterWavelength::DefaultValue() const
{
   return 500.7; // nm
}

// ----------------------------------------------------------------------------

SPCCGreenFilterBandwidth::SPCCGreenFilterBandwidth( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCGreenFilterBandwidthParameter = this;
}

IsoString SPCCGreenFilterBandwidth::Id() const
{
   return "greenFilterBandwidth";
}

int SPCCGreenFilterBandwidth::Precision() const
{
   return 1;
}

double SPCCGreenFilterBandwidth::DefaultValue() const
{
   return 3.0; // nm
}

// ----------------------------------------------------------------------------

SPCCBlueFilterWavelength::SPCCBlueFilterWavelength( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCBlueFilterWavelengthParameter = this;
}

IsoString SPCCBlueFilterWavelength::Id() const
{
   return "blueFilterWavelength";
}

int SPCCBlueFilterWavelength::Precision() const
{
   return 1;
}

double SPCCBlueFilterWavelength::DefaultValue() const
{
   return 500.7; // nm
}

// ----------------------------------------------------------------------------

SPCCBlueFilterBandwidth::SPCCBlueFilterBandwidth( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCBlueFilterBandwidthParameter = this;
}

IsoString SPCCBlueFilterBandwidth::Id() const
{
   return "blueFilterBandwidth";
}

int SPCCBlueFilterBandwidth::Precision() const
{
   return 1;
}

double SPCCBlueFilterBandwidth::DefaultValue() const
{
   return 3.0; // nm
}

// ----------------------------------------------------------------------------

SPCCDeviceQECurve::SPCCDeviceQECurve( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCDeviceQECurveParameter = this;
}

IsoString SPCCDeviceQECurve::Id() const
{
   return "deviceQECurve";
}

String SPCCDeviceQECurve::DefaultValue() const
{
   if ( API )
      if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
         return SpectrophotometricColorCalibrationProcess::DefaultDeviceQE().data;
   return String();
}

// ----------------------------------------------------------------------------

SPCCDeviceQECurveName::SPCCDeviceQECurveName( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCDeviceQECurveNameParameter = this;
}

IsoString SPCCDeviceQECurveName::Id() const
{
   return "deviceQECurveName";
}

String SPCCDeviceQECurveName::DefaultValue() const
{
   if ( API )
      if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
         return SpectrophotometricColorCalibrationProcess::DefaultDeviceQE().name;
   return String();
}

// ----------------------------------------------------------------------------

SPCCBroadbandIntegrationStepSize::SPCCBroadbandIntegrationStepSize( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCBroadbandIntegrationStepSizeParameter = this;
}

IsoString SPCCBroadbandIntegrationStepSize::Id() const
{
   return "broadbandIntegrationStepSize";
}

int SPCCBroadbandIntegrationStepSize::Precision() const
{
   return 2;
}

double SPCCBroadbandIntegrationStepSize::DefaultValue() const
{
   return 0.5; // nm
}

double SPCCBroadbandIntegrationStepSize::MinimumValue() const
{
   return 0.1;
}

double SPCCBroadbandIntegrationStepSize::MaximumValue() const
{
   return 5.0;
}

// ----------------------------------------------------------------------------

SPCCNarrowbandIntegrationSteps::SPCCNarrowbandIntegrationSteps( MetaProcess* P )
   : MetaInt32( P )
{
   TheSPCCNarrowbandIntegrationStepsParameter = this;
}

IsoString SPCCNarrowbandIntegrationSteps::Id() const
{
   return "narrowbandIntegrationSteps";
}

double SPCCNarrowbandIntegrationSteps::DefaultValue() const
{
   return 10;
}

double SPCCNarrowbandIntegrationSteps::MinimumValue() const
{
   return 5;
}

double SPCCNarrowbandIntegrationSteps::MaximumValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

SPCCCatalogId::SPCCCatalogId( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCCatalogIdParameter = this;
}

IsoString SPCCCatalogId::Id() const
{
   return "catalogId";
}

String SPCCCatalogId::DefaultValue() const
{
   return "GaiaDR3SP";
}

// ----------------------------------------------------------------------------

SPCCLimitMagnitude::SPCCLimitMagnitude( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCLimitMagnitudeParameter = this;
}

IsoString SPCCLimitMagnitude::Id() const
{
   return "limitMagnitude";
}

int SPCCLimitMagnitude::Precision() const
{
   return 2;
}

double SPCCLimitMagnitude::DefaultValue() const
{
   return 12;
}

double SPCCLimitMagnitude::MinimumValue() const
{
   return 0;
}

double SPCCLimitMagnitude::MaximumValue() const
{
   return 30;
}

// ----------------------------------------------------------------------------

SPCCAutoLimitMagnitude::SPCCAutoLimitMagnitude( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPCCAutoLimitMagnitudeParameter = this;
}

IsoString SPCCAutoLimitMagnitude::Id() const
{
   return "autoLimitMagnitude";
}

bool SPCCAutoLimitMagnitude::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SPCCTargetSourceCount::SPCCTargetSourceCount( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPCCTargetSourceCountParameter = this;
}

IsoString SPCCTargetSourceCount::Id() const
{
   return "targetSourceCount";
}

double SPCCTargetSourceCount::DefaultValue() const
{
   return 8000;
}

double SPCCTargetSourceCount::MinimumValue() const
{
   return 1000;
}

double SPCCTargetSourceCount::MaximumValue() const
{
   return 100000;
}

// ----------------------------------------------------------------------------

SPCCStructureLayers::SPCCStructureLayers( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPCCStructureLayersParameter = this;
}

IsoString SPCCStructureLayers::Id() const
{
   return "psfStructureLayers";
}

double SPCCStructureLayers::DefaultValue() const
{
   return 5;
}

double SPCCStructureLayers::MinimumValue() const
{
   return 1;
}

double SPCCStructureLayers::MaximumValue() const
{
   return 8;
}

// ----------------------------------------------------------------------------

SPCCSaturationThreshold::SPCCSaturationThreshold( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCSaturationThresholdParameter = this;
}

IsoString SPCCSaturationThreshold::Id() const
{
   return "saturationThreshold";
}

int SPCCSaturationThreshold::Precision() const
{
   return 2;
}

double SPCCSaturationThreshold::DefaultValue() const
{
   return 0.75;
}

double SPCCSaturationThreshold::MinimumValue() const
{
   return 0.10;
}

double SPCCSaturationThreshold::MaximumValue() const
{
   return 1.00;
}

// ----------------------------------------------------------------------------

SPCCSaturationRelative::SPCCSaturationRelative( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPCCSaturationRelativeParameter = this;
}

IsoString SPCCSaturationRelative::Id() const
{
   return "saturationRelative";
}

bool SPCCSaturationRelative::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SPCCSaturationShrinkFactor::SPCCSaturationShrinkFactor( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCSaturationShrinkFactorParameter = this;
}

IsoString SPCCSaturationShrinkFactor::Id() const
{
   return "saturationShrinkFactor";
}

int SPCCSaturationShrinkFactor::Precision() const
{
   return 2;
}

double SPCCSaturationShrinkFactor::DefaultValue() const
{
   return 0.1;
}

double SPCCSaturationShrinkFactor::MinimumValue() const
{
   return 0;
}

double SPCCSaturationShrinkFactor::MaximumValue() const
{
   return 0.25;
}

// ----------------------------------------------------------------------------

SPCCPSFNoiseLayers::SPCCPSFNoiseLayers( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPCCPSFNoiseLayersParameter = this;
}

IsoString SPCCPSFNoiseLayers::Id() const
{
   return "psfNoiseLayers";
}

double SPCCPSFNoiseLayers::DefaultValue() const
{
   return 1;
}

double SPCCPSFNoiseLayers::MinimumValue() const
{
   return 0;
}

double SPCCPSFNoiseLayers::MaximumValue() const
{
   return 4;
}

// ----------------------------------------------------------------------------

SPCCPSFHotPixelFilterRadius::SPCCPSFHotPixelFilterRadius( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPCCPSFHotPixelFilterRadiusParameter = this;
}

IsoString SPCCPSFHotPixelFilterRadius::Id() const
{
   return "psfHotPixelFilterRadius";
}

double SPCCPSFHotPixelFilterRadius::DefaultValue() const
{
   return 1;
}

double SPCCPSFHotPixelFilterRadius::MinimumValue() const
{
   return 0;
}

double SPCCPSFHotPixelFilterRadius::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

SPCCPSFNoiseReductionFilterRadius::SPCCPSFNoiseReductionFilterRadius( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPCCPSFNoiseReductionFilterRadiusParameter = this;
}

IsoString SPCCPSFNoiseReductionFilterRadius::Id() const
{
   return "psfNoiseReductionFilterRadius";
}

double SPCCPSFNoiseReductionFilterRadius::DefaultValue() const
{
   return 0;
}

double SPCCPSFNoiseReductionFilterRadius::MinimumValue() const
{
   return 0;
}

double SPCCPSFNoiseReductionFilterRadius::MaximumValue() const
{
   return 50;
}

// ----------------------------------------------------------------------------

SPCCPSFMinStructureSize::SPCCPSFMinStructureSize( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPCCPSFMinStructureSizeParameter = this;
}

IsoString SPCCPSFMinStructureSize::Id() const
{
   return "psfMinStructureSize";
}

double SPCCPSFMinStructureSize::DefaultValue() const
{
   return 0;
}

double SPCCPSFMinStructureSize::MinimumValue() const
{
   return 0;
}

double SPCCPSFMinStructureSize::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

SPCCPSFMinSNR::SPCCPSFMinSNR( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCPSFMinSNRParameter = this;
}

IsoString SPCCPSFMinSNR::Id() const
{
   return "psfMinSNR";
}

int SPCCPSFMinSNR::Precision() const
{
   return 2;
}

double SPCCPSFMinSNR::DefaultValue() const
{
   return 40.0;
}

double SPCCPSFMinSNR::MinimumValue() const
{
   return 0;
}

double SPCCPSFMinSNR::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

SPCCPSFAllowClusteredSources::SPCCPSFAllowClusteredSources( MetaProcess* p )
   : MetaBoolean( p )
{
   TheSPCCPSFAllowClusteredSourcesParameter = this;
}

IsoString SPCCPSFAllowClusteredSources::Id() const
{
   return "psfAllowClusteredSources";
}

bool SPCCPSFAllowClusteredSources::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SPCCPSFType::SPCCPSFType( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheSPCCPSFTypeParameter = this;
}

IsoString SPCCPSFType::Id() const
{
   return "psfType";
}

size_type SPCCPSFType::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SPCCPSFType::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Gaussian: return "PSFType_Gaussian";
   case Moffat15: return "PSFType_Moffat15";
   case Moffat4:  return "PSFType_Moffat4";
   case Moffat6:  return "PSFType_Moffat6";
   case Moffat8:  return "PSFType_Moffat8";
   case MoffatA:  return "PSFType_MoffatA";
   default:
   case Auto:     return "PSFType_Auto";
   }
}

IsoString SPCCPSFType::FunctionName( pcl_enum i )
{
   switch ( i )
   {
   case Gaussian: return "Gaussian";
   case Moffat15: return "Moffat15";
   case Moffat4:  return "Moffat4";
   case Moffat6:  return "Moffat6";
   case Moffat8:  return "Moffat8";
   case MoffatA:  return "MoffatA";
   default:
   case Auto:     return "Auto";
   }
}

int SPCCPSFType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type SPCCPSFType::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

SPCCPSFGrowth::SPCCPSFGrowth( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCPSFGrowthParameter = this;
}

IsoString SPCCPSFGrowth::Id() const
{
   return "psfGrowth";
}

int SPCCPSFGrowth::Precision() const
{
   return 2;
}

double SPCCPSFGrowth::DefaultValue() const
{
   return 1.25;
}

double SPCCPSFGrowth::MinimumValue() const
{
   return 0.5;
}

double SPCCPSFGrowth::MaximumValue() const
{
   return 4.0;
}

// ----------------------------------------------------------------------------

SPCCPSFMaxStars::SPCCPSFMaxStars( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPCCPSFMaxStarsParameter = this;
}

IsoString SPCCPSFMaxStars::Id() const
{
   return "psfMaxStars";
}

double SPCCPSFMaxStars::DefaultValue() const
{
   return 24*1024;
}

double SPCCPSFMaxStars::MinimumValue() const
{
   return 256;
}

double SPCCPSFMaxStars::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

SPCCPSFSearchTolerance::SPCCPSFSearchTolerance( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCPSFSearchToleranceParameter = this;
}

IsoString SPCCPSFSearchTolerance::Id() const
{
   return "psfSearchTolerance";
}

int SPCCPSFSearchTolerance::Precision() const
{
   return 2;
}

double SPCCPSFSearchTolerance::DefaultValue() const
{
   return 4;
}

double SPCCPSFSearchTolerance::MinimumValue() const
{
   return 0.1;
}

double SPCCPSFSearchTolerance::MaximumValue() const
{
   return 16;
}

// ----------------------------------------------------------------------------

SPCCPSFChannelSearchTolerance::SPCCPSFChannelSearchTolerance( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCPSFChannelSearchToleranceParameter = this;
}

IsoString SPCCPSFChannelSearchTolerance::Id() const
{
   return "psfChannelSearchTolerance";
}

int SPCCPSFChannelSearchTolerance::Precision() const
{
   return 2;
}

double SPCCPSFChannelSearchTolerance::DefaultValue() const
{
   return 2;
}

double SPCCPSFChannelSearchTolerance::MinimumValue() const
{
   return 0.1;
}

double SPCCPSFChannelSearchTolerance::MaximumValue() const
{
   return 16;
}

// ----------------------------------------------------------------------------

SPCCNeutralizeBackground::SPCCNeutralizeBackground( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPCCNeutralizeBackgroundParameter = this;
}

IsoString SPCCNeutralizeBackground::Id() const
{
   return "neutralizeBackground";
}

bool SPCCNeutralizeBackground::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SPCCBackgroundReferenceViewId::SPCCBackgroundReferenceViewId( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCBackgroundReferenceViewIdParameter = this;
}

IsoString SPCCBackgroundReferenceViewId::Id() const
{
   return "backgroundReferenceViewId";
}

// ----------------------------------------------------------------------------

SPCCBackgroundLow::SPCCBackgroundLow( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCBackgroundLowParameter = this;
}

IsoString SPCCBackgroundLow::Id() const
{
   return "backgroundLow";
}

int SPCCBackgroundLow::Precision() const
{
   return 2;
}

double SPCCBackgroundLow::MinimumValue() const
{
   return -10;
}

double SPCCBackgroundLow::MaximumValue() const
{
   return +10;
}

double SPCCBackgroundLow::DefaultValue() const
{
   return -2.8;
}

// ----------------------------------------------------------------------------

SPCCBackgroundHigh::SPCCBackgroundHigh( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPCCBackgroundHighParameter = this;
}

IsoString SPCCBackgroundHigh::Id() const
{
   return "backgroundHigh";
}

int SPCCBackgroundHigh::Precision() const
{
   return 2;
}

double SPCCBackgroundHigh::MinimumValue() const
{
   return -10;
}

double SPCCBackgroundHigh::MaximumValue() const
{
   return +10;
}

double SPCCBackgroundHigh::DefaultValue() const
{
   return +2.0;
}

// ----------------------------------------------------------------------------

SPCCBackgroundUseROI::SPCCBackgroundUseROI( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPCCBackgroundUseROIParameter = this;
}

IsoString SPCCBackgroundUseROI::Id() const
{
   return "backgroundUseROI";
}

bool SPCCBackgroundUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SPCCBackgroundROIX0::SPCCBackgroundROIX0( MetaProcess* P )
   : MetaInt32( P )
{
   TheSPCCBackgroundROIX0Parameter = this;
}

IsoString SPCCBackgroundROIX0::Id() const
{
   return "backgroundROIX0";
}

double SPCCBackgroundROIX0::DefaultValue() const
{
   return 0;
}

double SPCCBackgroundROIX0::MinimumValue() const
{
   return 0;
}

double SPCCBackgroundROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

SPCCBackgroundROIY0::SPCCBackgroundROIY0( MetaProcess* P )
   : MetaInt32( P )
{
   TheSPCCBackgroundROIY0Parameter = this;
}

IsoString SPCCBackgroundROIY0::Id() const
{
   return "backgroundROIY0";
}

double SPCCBackgroundROIY0::DefaultValue() const
{
   return 0;
}

double SPCCBackgroundROIY0::MinimumValue() const
{
   return 0;
}

double SPCCBackgroundROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

SPCCBackgroundROIX1::SPCCBackgroundROIX1( MetaProcess* P )
   : MetaInt32( P )
{
   TheSPCCBackgroundROIX1Parameter = this;
}

IsoString SPCCBackgroundROIX1::Id() const
{
   return "backgroundROIX1";
}

double SPCCBackgroundROIX1::DefaultValue() const
{
   return 0;
}

double SPCCBackgroundROIX1::MinimumValue() const
{
   return 0;
}

double SPCCBackgroundROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

SPCCBackgroundROIY1::SPCCBackgroundROIY1( MetaProcess* P )
   : MetaInt32( P )
{
   TheSPCCBackgroundROIY1Parameter = this;
}

IsoString SPCCBackgroundROIY1::Id() const
{
   return "backgroundROIY1";
}

double SPCCBackgroundROIY1::DefaultValue() const
{
   return 0;
}

double SPCCBackgroundROIY1::MinimumValue() const
{
   return 0;
}

double SPCCBackgroundROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

SPCCGenerateGraphs::SPCCGenerateGraphs( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPCCGenerateGraphsParameter = this;
}

IsoString SPCCGenerateGraphs::Id() const
{
   return "generateGraphs";
}

bool SPCCGenerateGraphs::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SPCCGenerateStarMaps::SPCCGenerateStarMaps( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPCCGenerateStarMapsParameter = this;
}

IsoString SPCCGenerateStarMaps::Id() const
{
   return "generateStarMaps";
}

bool SPCCGenerateStarMaps::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SPCCGenerateTextFiles::SPCCGenerateTextFiles( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPCCGenerateTextFilesParameter = this;
}

IsoString SPCCGenerateTextFiles::Id() const
{
   return "generateTextFiles";
}

bool SPCCGenerateTextFiles::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SPCCOutputDirectory::SPCCOutputDirectory( MetaProcess* P )
   : MetaString( P )
{
   TheSPCCOutputDirectoryParameter = this;
}

IsoString SPCCOutputDirectory::Id() const
{
   return "outputDirectory";
}

String SPCCOutputDirectory::DefaultValue() const
{
   return String(); // = File::SystemTempDirectory()
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SpectrophotometricColorCalibrationParameters.cpp - Released 2023-08-01T16:30:17Z
