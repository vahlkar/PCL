//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.11
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 2.1.0
// ----------------------------------------------------------------------------
// SpectrophotometricFluxCalibrationParameters.cpp - Released 2024-05-07T15:28:00Z
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

#include "SpectrophotometricFluxCalibrationParameters.h"
#include "SpectrophotometricFluxCalibrationProcess.h"

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SPFCNarrowbandMode*                TheSPFCNarrowbandModeParameter = nullptr;

SPFCGrayFilterTrCurve*             TheSPFCGrayFilterTrCurveParameter = nullptr;
SPFCGrayFilterName*                TheSPFCGrayFilterNameParameter = nullptr;
SPFCRedFilterTrCurve*              TheSPFCRedFilterTrCurveParameter = nullptr;
SPFCRedFilterName*                 TheSPFCRedFilterNameParameter = nullptr;
SPFCGreenFilterTrCurve*            TheSPFCGreenFilterTrCurveParameter = nullptr;
SPFCGreenFilterName*               TheSPFCGreenFilterNameParameter = nullptr;
SPFCBlueFilterTrCurve*             TheSPFCBlueFilterTrCurveParameter = nullptr;
SPFCBlueFilterName*                TheSPFCBlueFilterNameParameter = nullptr;

SPFCGrayFilterWavelength*          TheSPFCGrayFilterWavelengthParameter = nullptr;
SPFCGrayFilterBandwidth*           TheSPFCGrayFilterBandwidthParameter = nullptr;
SPFCRedFilterWavelength*           TheSPFCRedFilterWavelengthParameter = nullptr;
SPFCRedFilterBandwidth*            TheSPFCRedFilterBandwidthParameter = nullptr;
SPFCGreenFilterWavelength*         TheSPFCGreenFilterWavelengthParameter = nullptr;
SPFCGreenFilterBandwidth*          TheSPFCGreenFilterBandwidthParameter = nullptr;
SPFCBlueFilterWavelength*          TheSPFCBlueFilterWavelengthParameter = nullptr;
SPFCBlueFilterBandwidth*           TheSPFCBlueFilterBandwidthParameter = nullptr;

SPFCDeviceQECurve*                 TheSPFCDeviceQECurveParameter = nullptr;
SPFCDeviceQECurveName*             TheSPFCDeviceQECurveNameParameter = nullptr;

SPFCBroadbandIntegrationStepSize*  TheSPFCBroadbandIntegrationStepSizeParameter = nullptr;
SPFCNarrowbandIntegrationSteps*    TheSPFCNarrowbandIntegrationStepsParameter = nullptr;
SPFCTrimmingFraction*              TheSPFCTrimmingFractionParameter = nullptr;

SPFCCatalogId*                     TheSPFCCatalogIdParameter = nullptr;
SPFCLimitMagnitude*                TheSPFCLimitMagnitudeParameter = nullptr;
SPFCAutoLimitMagnitude*            TheSPFCAutoLimitMagnitudeParameter = nullptr;

SPFCStructureLayers*               TheSPFCStructureLayersParameter = nullptr;
SPFCSaturationThreshold*           TheSPFCSaturationThresholdParameter = nullptr;
SPFCSaturationRelative*            TheSPFCSaturationRelativeParameter = nullptr;
SPFCSaturationShrinkFactor*        TheSPFCSaturationShrinkFactorParameter = nullptr;
SPFCPSFNoiseLayers*                TheSPFCPSFNoiseLayersParameter = nullptr;
SPFCPSFHotPixelFilterRadius*       TheSPFCPSFHotPixelFilterRadiusParameter = nullptr;
SPFCPSFNoiseReductionFilterRadius* TheSPFCPSFNoiseReductionFilterRadiusParameter = nullptr;
SPFCPSFMinStructureSize*           TheSPFCPSFMinStructureSizeParameter = nullptr;
SPFCPSFMinSNR*                     TheSPFCPSFMinSNRParameter = nullptr;
SPFCPSFAllowClusteredSources*      TheSPFCPSFAllowClusteredSourcesParameter = nullptr;
SPFCPSFType*                       TheSPFCPSFTypeParameter = nullptr;
SPFCPSFGrowth*                     TheSPFCPSFGrowthParameter = nullptr;
SPFCPSFMaxStars*                   TheSPFCPSFMaxStarsParameter = nullptr;
SPFCPSFSearchTolerance*            TheSPFCPSFSearchToleranceParameter = nullptr;
SPFCPSFChannelSearchTolerance*     TheSPFCPSFChannelSearchToleranceParameter = nullptr;

// ----------------------------------------------------------------------------

SPFCNarrowbandMode::SPFCNarrowbandMode( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPFCNarrowbandModeParameter = this;
}

IsoString SPFCNarrowbandMode::Id() const
{
   return "narrowbandMode";
}

bool SPFCNarrowbandMode::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SPFCGrayFilterTrCurve::SPFCGrayFilterTrCurve( MetaProcess* P )
   : MetaString( P )
{
   TheSPFCGrayFilterTrCurveParameter = this;
}

IsoString SPFCGrayFilterTrCurve::Id() const
{
   return "grayFilterTrCurve";
}

String SPFCGrayFilterTrCurve::DefaultValue() const
{
   if ( API )
      if ( TheSpectraDatabase.IsValid() )
         return TheSpectraDatabase.DefaultLuminanceFilter().data;
   return String();
}

// ----------------------------------------------------------------------------

SPFCGrayFilterName::SPFCGrayFilterName( MetaProcess* P )
   : MetaString( P )
{
   TheSPFCGrayFilterNameParameter = this;
}

IsoString SPFCGrayFilterName::Id() const
{
   return "grayFilterName";
}

String SPFCGrayFilterName::DefaultValue() const
{
   if ( API )
      if ( TheSpectraDatabase.IsValid() )
         return TheSpectraDatabase.DefaultLuminanceFilter().name;
   return String();
}

// ----------------------------------------------------------------------------

SPFCRedFilterTrCurve::SPFCRedFilterTrCurve( MetaProcess* P )
   : MetaString( P )
{
   TheSPFCRedFilterTrCurveParameter = this;
}

IsoString SPFCRedFilterTrCurve::Id() const
{
   return "redFilterTrCurve";
}

String SPFCRedFilterTrCurve::DefaultValue() const
{
   if ( API )
      if ( TheSpectraDatabase.IsValid() )
         return TheSpectraDatabase.DefaultRedFilter().data;
   return String();
}

// ----------------------------------------------------------------------------

SPFCRedFilterName::SPFCRedFilterName( MetaProcess* P )
   : MetaString( P )
{
   TheSPFCRedFilterNameParameter = this;
}

IsoString SPFCRedFilterName::Id() const
{
   return "redFilterName";
}

String SPFCRedFilterName::DefaultValue() const
{
   if ( API )
      if ( TheSpectraDatabase.IsValid() )
         return TheSpectraDatabase.DefaultRedFilter().name;
   return String();
}

// ----------------------------------------------------------------------------

SPFCGreenFilterTrCurve::SPFCGreenFilterTrCurve( MetaProcess* P )
   : MetaString( P )
{
   TheSPFCGreenFilterTrCurveParameter = this;
}

IsoString SPFCGreenFilterTrCurve::Id() const
{
   return "greenFilterTrCurve";
}

String SPFCGreenFilterTrCurve::DefaultValue() const
{
   if ( API )
      if ( TheSpectraDatabase.IsValid() )
         return TheSpectraDatabase.DefaultGreenFilter().data;
   return String();
}

// ----------------------------------------------------------------------------

SPFCGreenFilterName::SPFCGreenFilterName( MetaProcess* P )
   : MetaString( P )
{
   TheSPFCGreenFilterNameParameter = this;
}

IsoString SPFCGreenFilterName::Id() const
{
   return "greenFilterName";
}

String SPFCGreenFilterName::DefaultValue() const
{
   if ( API )
      if ( TheSpectraDatabase.IsValid() )
         return TheSpectraDatabase.DefaultGreenFilter().name;
   return String();
}

// ----------------------------------------------------------------------------

SPFCBlueFilterTrCurve::SPFCBlueFilterTrCurve( MetaProcess* P )
   : MetaString( P )
{
   TheSPFCBlueFilterTrCurveParameter = this;
}

IsoString SPFCBlueFilterTrCurve::Id() const
{
   return "blueFilterTrCurve";
}

String SPFCBlueFilterTrCurve::DefaultValue() const
{
   if ( API )
      if ( TheSpectraDatabase.IsValid() )
         return TheSpectraDatabase.DefaultBlueFilter().data;
   return String();
}

// ----------------------------------------------------------------------------

SPFCBlueFilterName::SPFCBlueFilterName( MetaProcess* P )
   : MetaString( P )
{
   TheSPFCBlueFilterNameParameter = this;
}

IsoString SPFCBlueFilterName::Id() const
{
   return "blueFilterName";
}

String SPFCBlueFilterName::DefaultValue() const
{
   if ( API )
      if ( TheSpectraDatabase.IsValid() )
         return TheSpectraDatabase.DefaultBlueFilter().name;
   return String();
}

// ----------------------------------------------------------------------------

SPFCGrayFilterWavelength::SPFCGrayFilterWavelength( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCGrayFilterWavelengthParameter = this;
}

IsoString SPFCGrayFilterWavelength::Id() const
{
   return "grayFilterWavelength";
}

int SPFCGrayFilterWavelength::Precision() const
{
   return 1;
}

double SPFCGrayFilterWavelength::DefaultValue() const
{
   return 656.3; // nm
}

// ----------------------------------------------------------------------------

SPFCGrayFilterBandwidth::SPFCGrayFilterBandwidth( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCGrayFilterBandwidthParameter = this;
}

IsoString SPFCGrayFilterBandwidth::Id() const
{
   return "grayFilterBandwidth";
}

int SPFCGrayFilterBandwidth::Precision() const
{
   return 1;
}

double SPFCGrayFilterBandwidth::DefaultValue() const
{
   return 3.0; // nm
}

// ----------------------------------------------------------------------------

SPFCRedFilterWavelength::SPFCRedFilterWavelength( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCRedFilterWavelengthParameter = this;
}

IsoString SPFCRedFilterWavelength::Id() const
{
   return "redFilterWavelength";
}

int SPFCRedFilterWavelength::Precision() const
{
   return 1;
}

double SPFCRedFilterWavelength::DefaultValue() const
{
   return 656.3; // nm
}

// ----------------------------------------------------------------------------

SPFCRedFilterBandwidth::SPFCRedFilterBandwidth( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCRedFilterBandwidthParameter = this;
}

IsoString SPFCRedFilterBandwidth::Id() const
{
   return "redFilterBandwidth";
}

int SPFCRedFilterBandwidth::Precision() const
{
   return 1;
}

double SPFCRedFilterBandwidth::DefaultValue() const
{
   return 3.0; // nm
}

// ----------------------------------------------------------------------------

SPFCGreenFilterWavelength::SPFCGreenFilterWavelength( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCGreenFilterWavelengthParameter = this;
}

IsoString SPFCGreenFilterWavelength::Id() const
{
   return "greenFilterWavelength";
}

int SPFCGreenFilterWavelength::Precision() const
{
   return 1;
}

double SPFCGreenFilterWavelength::DefaultValue() const
{
   return 500.7; // nm
}

// ----------------------------------------------------------------------------

SPFCGreenFilterBandwidth::SPFCGreenFilterBandwidth( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCGreenFilterBandwidthParameter = this;
}

IsoString SPFCGreenFilterBandwidth::Id() const
{
   return "greenFilterBandwidth";
}

int SPFCGreenFilterBandwidth::Precision() const
{
   return 1;
}

double SPFCGreenFilterBandwidth::DefaultValue() const
{
   return 3.0; // nm
}

// ----------------------------------------------------------------------------

SPFCBlueFilterWavelength::SPFCBlueFilterWavelength( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCBlueFilterWavelengthParameter = this;
}

IsoString SPFCBlueFilterWavelength::Id() const
{
   return "blueFilterWavelength";
}

int SPFCBlueFilterWavelength::Precision() const
{
   return 1;
}

double SPFCBlueFilterWavelength::DefaultValue() const
{
   return 500.7; // nm
}

// ----------------------------------------------------------------------------

SPFCBlueFilterBandwidth::SPFCBlueFilterBandwidth( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCBlueFilterBandwidthParameter = this;
}

IsoString SPFCBlueFilterBandwidth::Id() const
{
   return "blueFilterBandwidth";
}

int SPFCBlueFilterBandwidth::Precision() const
{
   return 1;
}

double SPFCBlueFilterBandwidth::DefaultValue() const
{
   return 3.0; // nm
}

// ----------------------------------------------------------------------------

SPFCDeviceQECurve::SPFCDeviceQECurve( MetaProcess* P )
   : MetaString( P )
{
   TheSPFCDeviceQECurveParameter = this;
}

IsoString SPFCDeviceQECurve::Id() const
{
   return "deviceQECurve";
}

String SPFCDeviceQECurve::DefaultValue() const
{
   if ( API )
      if ( TheSpectraDatabase.IsValid() )
         return TheSpectraDatabase.DefaultDeviceQE().data;
   return String();
}

// ----------------------------------------------------------------------------

SPFCDeviceQECurveName::SPFCDeviceQECurveName( MetaProcess* P )
   : MetaString( P )
{
   TheSPFCDeviceQECurveNameParameter = this;
}

IsoString SPFCDeviceQECurveName::Id() const
{
   return "deviceQECurveName";
}

String SPFCDeviceQECurveName::DefaultValue() const
{
   if ( API )
      if ( TheSpectraDatabase.IsValid() )
         return TheSpectraDatabase.DefaultDeviceQE().name;
   return String();
}

// ----------------------------------------------------------------------------

SPFCBroadbandIntegrationStepSize::SPFCBroadbandIntegrationStepSize( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCBroadbandIntegrationStepSizeParameter = this;
}

IsoString SPFCBroadbandIntegrationStepSize::Id() const
{
   return "broadbandIntegrationStepSize";
}

int SPFCBroadbandIntegrationStepSize::Precision() const
{
   return 2;
}

double SPFCBroadbandIntegrationStepSize::DefaultValue() const
{
   return 0.5; // nm
}

double SPFCBroadbandIntegrationStepSize::MinimumValue() const
{
   return 0.1;
}

double SPFCBroadbandIntegrationStepSize::MaximumValue() const
{
   return 5.0;
}

// ----------------------------------------------------------------------------

SPFCNarrowbandIntegrationSteps::SPFCNarrowbandIntegrationSteps( MetaProcess* P )
   : MetaInt32( P )
{
   TheSPFCNarrowbandIntegrationStepsParameter = this;
}

IsoString SPFCNarrowbandIntegrationSteps::Id() const
{
   return "narrowbandIntegrationSteps";
}

double SPFCNarrowbandIntegrationSteps::DefaultValue() const
{
   return 10;
}

double SPFCNarrowbandIntegrationSteps::MinimumValue() const
{
   return 5;
}

double SPFCNarrowbandIntegrationSteps::MaximumValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

SPFCTrimmingFraction::SPFCTrimmingFraction( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCTrimmingFractionParameter = this;
}

IsoString SPFCTrimmingFraction::Id() const
{
   return "trimmingFraction";
}

int SPFCTrimmingFraction::Precision() const
{
   return 2;
}

double SPFCTrimmingFraction::DefaultValue() const
{
   return 0.20;
}

double SPFCTrimmingFraction::MinimumValue() const
{
   return 0.01;
}

double SPFCTrimmingFraction::MaximumValue() const
{
   return 0.50;
}

// ----------------------------------------------------------------------------

SPFCCatalogId::SPFCCatalogId( MetaProcess* P )
   : MetaString( P )
{
   TheSPFCCatalogIdParameter = this;
}

IsoString SPFCCatalogId::Id() const
{
   return "catalogId";
}

String SPFCCatalogId::DefaultValue() const
{
   return "GaiaDR3SP";
}

// ----------------------------------------------------------------------------

SPFCLimitMagnitude::SPFCLimitMagnitude( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCLimitMagnitudeParameter = this;
}

IsoString SPFCLimitMagnitude::Id() const
{
   return "limitMagnitude";
}

int SPFCLimitMagnitude::Precision() const
{
   return 2;
}

double SPFCLimitMagnitude::DefaultValue() const
{
   return 12;
}

double SPFCLimitMagnitude::MinimumValue() const
{
   return 0;
}

double SPFCLimitMagnitude::MaximumValue() const
{
   return 30;
}

// ----------------------------------------------------------------------------

SPFCAutoLimitMagnitude::SPFCAutoLimitMagnitude( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPFCAutoLimitMagnitudeParameter = this;
}

IsoString SPFCAutoLimitMagnitude::Id() const
{
   return "autoLimitMagnitude";
}

bool SPFCAutoLimitMagnitude::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SPFCStructureLayers::SPFCStructureLayers( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPFCStructureLayersParameter = this;
}

IsoString SPFCStructureLayers::Id() const
{
   return "psfStructureLayers";
}

double SPFCStructureLayers::DefaultValue() const
{
   return 5;
}

double SPFCStructureLayers::MinimumValue() const
{
   return 1;
}

double SPFCStructureLayers::MaximumValue() const
{
   return 8;
}

// ----------------------------------------------------------------------------

SPFCSaturationThreshold::SPFCSaturationThreshold( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCSaturationThresholdParameter = this;
}

IsoString SPFCSaturationThreshold::Id() const
{
   return "saturationThreshold";
}

int SPFCSaturationThreshold::Precision() const
{
   return 2;
}

double SPFCSaturationThreshold::DefaultValue() const
{
   return 0.75;
}

double SPFCSaturationThreshold::MinimumValue() const
{
   return 0.10;
}

double SPFCSaturationThreshold::MaximumValue() const
{
   return 1.00;
}

// ----------------------------------------------------------------------------

SPFCSaturationRelative::SPFCSaturationRelative( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSPFCSaturationRelativeParameter = this;
}

IsoString SPFCSaturationRelative::Id() const
{
   return "saturationRelative";
}

bool SPFCSaturationRelative::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SPFCSaturationShrinkFactor::SPFCSaturationShrinkFactor( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCSaturationShrinkFactorParameter = this;
}

IsoString SPFCSaturationShrinkFactor::Id() const
{
   return "saturationShrinkFactor";
}

int SPFCSaturationShrinkFactor::Precision() const
{
   return 2;
}

double SPFCSaturationShrinkFactor::DefaultValue() const
{
   return 0.1;
}

double SPFCSaturationShrinkFactor::MinimumValue() const
{
   return 0;
}

double SPFCSaturationShrinkFactor::MaximumValue() const
{
   return 0.25;
}

// ----------------------------------------------------------------------------

SPFCPSFNoiseLayers::SPFCPSFNoiseLayers( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPFCPSFNoiseLayersParameter = this;
}

IsoString SPFCPSFNoiseLayers::Id() const
{
   return "psfNoiseLayers";
}

double SPFCPSFNoiseLayers::DefaultValue() const
{
   return 1;
}

double SPFCPSFNoiseLayers::MinimumValue() const
{
   return 0;
}

double SPFCPSFNoiseLayers::MaximumValue() const
{
   return 4;
}

// ----------------------------------------------------------------------------

SPFCPSFHotPixelFilterRadius::SPFCPSFHotPixelFilterRadius( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPFCPSFHotPixelFilterRadiusParameter = this;
}

IsoString SPFCPSFHotPixelFilterRadius::Id() const
{
   return "psfHotPixelFilterRadius";
}

double SPFCPSFHotPixelFilterRadius::DefaultValue() const
{
   return 1;
}

double SPFCPSFHotPixelFilterRadius::MinimumValue() const
{
   return 0;
}

double SPFCPSFHotPixelFilterRadius::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

SPFCPSFNoiseReductionFilterRadius::SPFCPSFNoiseReductionFilterRadius( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPFCPSFNoiseReductionFilterRadiusParameter = this;
}

IsoString SPFCPSFNoiseReductionFilterRadius::Id() const
{
   return "psfNoiseReductionFilterRadius";
}

double SPFCPSFNoiseReductionFilterRadius::DefaultValue() const
{
   return 0;
}

double SPFCPSFNoiseReductionFilterRadius::MinimumValue() const
{
   return 0;
}

double SPFCPSFNoiseReductionFilterRadius::MaximumValue() const
{
   return 50;
}

// ----------------------------------------------------------------------------

SPFCPSFMinStructureSize::SPFCPSFMinStructureSize( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPFCPSFMinStructureSizeParameter = this;
}

IsoString SPFCPSFMinStructureSize::Id() const
{
   return "psfMinStructureSize";
}

double SPFCPSFMinStructureSize::DefaultValue() const
{
   return 0;
}

double SPFCPSFMinStructureSize::MinimumValue() const
{
   return 0;
}

double SPFCPSFMinStructureSize::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

SPFCPSFMinSNR::SPFCPSFMinSNR( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCPSFMinSNRParameter = this;
}

IsoString SPFCPSFMinSNR::Id() const
{
   return "psfMinSNR";
}

int SPFCPSFMinSNR::Precision() const
{
   return 2;
}

double SPFCPSFMinSNR::DefaultValue() const
{
   return 40.0;
}

double SPFCPSFMinSNR::MinimumValue() const
{
   return 0;
}

double SPFCPSFMinSNR::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

SPFCPSFAllowClusteredSources::SPFCPSFAllowClusteredSources( MetaProcess* p )
   : MetaBoolean( p )
{
   TheSPFCPSFAllowClusteredSourcesParameter = this;
}

IsoString SPFCPSFAllowClusteredSources::Id() const
{
   return "psfAllowClusteredSources";
}

bool SPFCPSFAllowClusteredSources::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SPFCPSFType::SPFCPSFType( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheSPFCPSFTypeParameter = this;
}

IsoString SPFCPSFType::Id() const
{
   return "psfType";
}

size_type SPFCPSFType::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SPFCPSFType::ElementId( size_type i ) const
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

IsoString SPFCPSFType::FunctionName( pcl_enum i )
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

int SPFCPSFType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type SPFCPSFType::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

SPFCPSFGrowth::SPFCPSFGrowth( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCPSFGrowthParameter = this;
}

IsoString SPFCPSFGrowth::Id() const
{
   return "psfGrowth";
}

int SPFCPSFGrowth::Precision() const
{
   return 2;
}

double SPFCPSFGrowth::DefaultValue() const
{
   return 1.25;
}

double SPFCPSFGrowth::MinimumValue() const
{
   return 0.5;
}

double SPFCPSFGrowth::MaximumValue() const
{
   return 4.0;
}

// ----------------------------------------------------------------------------

SPFCPSFMaxStars::SPFCPSFMaxStars( MetaProcess* p )
   : MetaInt32( p )
{
   TheSPFCPSFMaxStarsParameter = this;
}

IsoString SPFCPSFMaxStars::Id() const
{
   return "psfMaxStars";
}

double SPFCPSFMaxStars::DefaultValue() const
{
   return 24*1024;
}

double SPFCPSFMaxStars::MinimumValue() const
{
   return 256;
}

double SPFCPSFMaxStars::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

SPFCPSFSearchTolerance::SPFCPSFSearchTolerance( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCPSFSearchToleranceParameter = this;
}

IsoString SPFCPSFSearchTolerance::Id() const
{
   return "psfSearchTolerance";
}

int SPFCPSFSearchTolerance::Precision() const
{
   return 2;
}

double SPFCPSFSearchTolerance::DefaultValue() const
{
   return 4;
}

double SPFCPSFSearchTolerance::MinimumValue() const
{
   return 0.1;
}

double SPFCPSFSearchTolerance::MaximumValue() const
{
   return 16;
}

// ----------------------------------------------------------------------------

SPFCPSFChannelSearchTolerance::SPFCPSFChannelSearchTolerance( MetaProcess* P )
   : MetaFloat( P )
{
   TheSPFCPSFChannelSearchToleranceParameter = this;
}

IsoString SPFCPSFChannelSearchTolerance::Id() const
{
   return "psfChannelSearchTolerance";
}

int SPFCPSFChannelSearchTolerance::Precision() const
{
   return 2;
}

double SPFCPSFChannelSearchTolerance::DefaultValue() const
{
   return 2;
}

double SPFCPSFChannelSearchTolerance::MinimumValue() const
{
   return 0.1;
}

double SPFCPSFChannelSearchTolerance::MaximumValue() const
{
   return 16;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SpectrophotometricFluxCalibrationParameters.cpp - Released 2024-05-07T15:28:00Z
