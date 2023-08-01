//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.7
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationParameters.cpp - Released 2023-08-01T16:30:17Z
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

#include "PhotometricColorCalibrationParameters.h"
#include "PhotometricColorCalibrationProcess.h"

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PCCApplyCalibration*              ThePCCApplyCalibrationParameter = nullptr;

PCCWhiteReferenceId*              ThePCCWhiteReferenceIdParameter = nullptr;
PCCWhiteReferenceName*            ThePCCWhiteReferenceNameParameter = nullptr;
PCCWhiteReferenceSr_JV*           ThePCCWhiteReferenceSr_JVParameter = nullptr;
PCCWhiteReferenceJB_JV*           ThePCCWhiteReferenceJB_JVParameter = nullptr;
PCCZeroPointSr_JV*                ThePCCZeroPointSr_JVParameter = nullptr;
PCCZeroPointJB_JV*                ThePCCZeroPointJB_JVParameter = nullptr;

PCCAutoCatalog*                   ThePCCAutoCatalogParameter = nullptr;
PCCCatalogId*                     ThePCCCatalogIdParameter = nullptr;
PCCLimitMagnitude*                ThePCCLimitMagnitudeParameter = nullptr;
PCCAutoLimitMagnitude*            ThePCCAutoLimitMagnitudeParameter = nullptr;
PCCTargetSourceCount*             ThePCCTargetSourceCountParameter = nullptr;
PCCServerURL*                     ThePCCServerURLParameter = nullptr;

PCCStructureLayers*               ThePCCStructureLayersParameter = nullptr;
PCCSaturationThreshold*           ThePCCSaturationThresholdParameter = nullptr;
PCCSaturationRelative*            ThePCCSaturationRelativeParameter = nullptr;
PCCSaturationShrinkFactor*        ThePCCSaturationShrinkFactorParameter = nullptr;
PCCPSFNoiseLayers*                ThePCCPSFNoiseLayersParameter = nullptr;
PCCPSFHotPixelFilterRadius*       ThePCCPSFHotPixelFilterRadiusParameter = nullptr;
PCCPSFNoiseReductionFilterRadius* ThePCCPSFNoiseReductionFilterRadiusParameter = nullptr;
PCCPSFMinStructureSize*           ThePCCPSFMinStructureSizeParameter = nullptr;
PCCPSFMinSNR*                     ThePCCPSFMinSNRParameter = nullptr;
PCCPSFAllowClusteredSources*      ThePCCPSFAllowClusteredSourcesParameter = nullptr;
PCCPSFType*                       ThePCCPSFTypeParameter = nullptr;
PCCPSFGrowth*                     ThePCCPSFGrowthParameter = nullptr;
PCCPSFMaxStars*                   ThePCCPSFMaxStarsParameter = nullptr;
PCCPSFSearchTolerance*            ThePCCPSFSearchToleranceParameter = nullptr;
PCCPSFChannelSearchTolerance*     ThePCCPSFChannelSearchToleranceParameter = nullptr;

PCCNeutralizeBackground*          ThePCCNeutralizeBackgroundParameter = nullptr;
PCCBackgroundReferenceViewId*     ThePCCBackgroundReferenceViewIdParameter = nullptr;
PCCBackgroundLow*                 ThePCCBackgroundLowParameter = nullptr;
PCCBackgroundHigh*                ThePCCBackgroundHighParameter = nullptr;
PCCBackgroundUseROI*              ThePCCBackgroundUseROIParameter = nullptr;
PCCBackgroundROIX0*               ThePCCBackgroundROIX0Parameter = nullptr;
PCCBackgroundROIY0*               ThePCCBackgroundROIY0Parameter = nullptr;
PCCBackgroundROIX1*               ThePCCBackgroundROIX1Parameter = nullptr;
PCCBackgroundROIY1*               ThePCCBackgroundROIY1Parameter = nullptr;

PCCGenerateGraphs*                ThePCCGenerateGraphsParameter = nullptr;
PCCGenerateStarMaps*              ThePCCGenerateStarMapsParameter = nullptr;
PCCGenerateTextFiles*             ThePCCGenerateTextFilesParameter = nullptr;
PCCOutputDirectory*               ThePCCOutputDirectoryParameter = nullptr;

// ----------------------------------------------------------------------------

PCCApplyCalibration::PCCApplyCalibration( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCApplyCalibrationParameter = this;
}

IsoString PCCApplyCalibration::Id() const
{
   return "applyCalibration";
}

bool PCCApplyCalibration::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCWhiteReferenceId::PCCWhiteReferenceId( MetaProcess* P ) : MetaString( P )
{
   ThePCCWhiteReferenceIdParameter = this;
}

IsoString PCCWhiteReferenceId::Id() const
{
   return "whiteReferenceId";
}

String PCCWhiteReferenceId::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences();
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::DefaultWhiteReference().id;
   }
   return "S";
}

// ----------------------------------------------------------------------------

PCCWhiteReferenceName::PCCWhiteReferenceName( MetaProcess* P ) : MetaString( P )
{
   ThePCCWhiteReferenceNameParameter = this;
}

IsoString PCCWhiteReferenceName::Id() const
{
   return "whiteReferenceName";
}

String PCCWhiteReferenceName::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences();
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::DefaultWhiteReference().name;
   }
   return "Average Spiral Galaxy";
}

// ----------------------------------------------------------------------------

PCCWhiteReferenceSr_JV::PCCWhiteReferenceSr_JV( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCWhiteReferenceSr_JVParameter = this;
}

IsoString PCCWhiteReferenceSr_JV::Id() const
{
   return "whiteReferenceSr_JV";
}

int PCCWhiteReferenceSr_JV::Precision() const
{
   return 5;
}

double PCCWhiteReferenceSr_JV::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences();
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::DefaultWhiteReference().Sr_JV;
   }
   return -0.37433; // average spiral
}

// ----------------------------------------------------------------------------

PCCWhiteReferenceJB_JV::PCCWhiteReferenceJB_JV( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCWhiteReferenceJB_JVParameter = this;
}

IsoString PCCWhiteReferenceJB_JV::Id() const
{
   return "whiteReferenceJB_JV";
}

int PCCWhiteReferenceJB_JV::Precision() const
{
   return 5;
}

double PCCWhiteReferenceJB_JV::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences();
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::DefaultWhiteReference().JB_JV;
   }
   return +0.28594; // average spiral
}

// ----------------------------------------------------------------------------

PCCZeroPointSr_JV::PCCZeroPointSr_JV( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCZeroPointSr_JVParameter = this;
}

IsoString PCCZeroPointSr_JV::Id() const
{
   return "zeroPointSr_JV";
}

int PCCZeroPointSr_JV::Precision() const
{
   return 5;
}

double PCCZeroPointSr_JV::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences();
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::ZeroPoint().Sr_JV;
   }
   return -0.14656;
}

// ----------------------------------------------------------------------------

PCCZeroPointJB_JV::PCCZeroPointJB_JV( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCZeroPointJB_JVParameter = this;
}

IsoString PCCZeroPointJB_JV::Id() const
{
   return "zeroPointJB_JV";
}

int PCCZeroPointJB_JV::Precision() const
{
   return 5;
}

double PCCZeroPointJB_JV::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences();
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::ZeroPoint().JB_JV;
   }
   return -0.42432;
}

// ----------------------------------------------------------------------------

PCCAutoCatalog::PCCAutoCatalog( MetaProcess* P )
   : MetaBoolean( P )
{
   ThePCCAutoCatalogParameter = this;
}

IsoString PCCAutoCatalog::Id() const
{
   return "autoCatalog";
}

bool PCCAutoCatalog::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCCatalogId::PCCCatalogId( MetaProcess* P )
   : MetaString( P )
{
   ThePCCCatalogIdParameter = this;
}

IsoString PCCCatalogId::Id() const
{
   return "catalogId";
}

String PCCCatalogId::DefaultValue() const
{
   return "APASS"; // online VizieR catalog
}

// ----------------------------------------------------------------------------

PCCLimitMagnitude::PCCLimitMagnitude( MetaProcess* P )
   : MetaFloat( P )
{
   ThePCCLimitMagnitudeParameter = this;
}

IsoString PCCLimitMagnitude::Id() const
{
   return "limitMagnitude";
}

int PCCLimitMagnitude::Precision() const
{
   return 2;
}

double PCCLimitMagnitude::DefaultValue() const
{
   return 12;
}

double PCCLimitMagnitude::MinimumValue() const
{
   return 0;
}

double PCCLimitMagnitude::MaximumValue() const
{
   return 30;
}

// ----------------------------------------------------------------------------

PCCAutoLimitMagnitude::PCCAutoLimitMagnitude( MetaProcess* P )
   : MetaBoolean( P )
{
   ThePCCAutoLimitMagnitudeParameter = this;
}

IsoString PCCAutoLimitMagnitude::Id() const
{
   return "autoLimitMagnitude";
}

bool PCCAutoLimitMagnitude::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCTargetSourceCount::PCCTargetSourceCount( MetaProcess* p )
   : MetaInt32( p )
{
   ThePCCTargetSourceCountParameter = this;
}

IsoString PCCTargetSourceCount::Id() const
{
   return "targetSourceCount";
}

double PCCTargetSourceCount::DefaultValue() const
{
   return 8000;
}

double PCCTargetSourceCount::MinimumValue() const
{
   return 1000;
}

double PCCTargetSourceCount::MaximumValue() const
{
   return 100000;
}

// ----------------------------------------------------------------------------

PCCServerURL::PCCServerURL( MetaProcess* P ) : MetaString( P )
{
   ThePCCServerURLParameter = this;
}

IsoString PCCServerURL::Id() const
{
   return "serverURL";
}

String PCCServerURL::DefaultValue() const
{
   return "https://vizier.cds.unistra.fr/";
}

// ----------------------------------------------------------------------------

PCCStructureLayers::PCCStructureLayers( MetaProcess* p )
   : MetaInt32( p )
{
   ThePCCStructureLayersParameter = this;
}

IsoString PCCStructureLayers::Id() const
{
   return "psfStructureLayers";
}

double PCCStructureLayers::DefaultValue() const
{
   return 5;
}

double PCCStructureLayers::MinimumValue() const
{
   return 1;
}

double PCCStructureLayers::MaximumValue() const
{
   return 8;
}

// ----------------------------------------------------------------------------

PCCSaturationThreshold::PCCSaturationThreshold( MetaProcess* P )
   : MetaFloat( P )
{
   ThePCCSaturationThresholdParameter = this;
}

IsoString PCCSaturationThreshold::Id() const
{
   return "saturationThreshold";
}

int PCCSaturationThreshold::Precision() const
{
   return 2;
}

double PCCSaturationThreshold::DefaultValue() const
{
   return 0.75;
}

double PCCSaturationThreshold::MinimumValue() const
{
   return 0.10;
}

double PCCSaturationThreshold::MaximumValue() const
{
   return 1.00;
}

// ----------------------------------------------------------------------------

PCCSaturationRelative::PCCSaturationRelative( MetaProcess* P )
   : MetaBoolean( P )
{
   ThePCCSaturationRelativeParameter = this;
}

IsoString PCCSaturationRelative::Id() const
{
   return "saturationRelative";
}

bool PCCSaturationRelative::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCSaturationShrinkFactor::PCCSaturationShrinkFactor( MetaProcess* P )
   : MetaFloat( P )
{
   ThePCCSaturationShrinkFactorParameter = this;
}

IsoString PCCSaturationShrinkFactor::Id() const
{
   return "saturationShrinkFactor";
}

int PCCSaturationShrinkFactor::Precision() const
{
   return 2;
}

double PCCSaturationShrinkFactor::DefaultValue() const
{
   return 0.1;
}

double PCCSaturationShrinkFactor::MinimumValue() const
{
   return 0;
}

double PCCSaturationShrinkFactor::MaximumValue() const
{
   return 0.25;
}

// ----------------------------------------------------------------------------

PCCPSFNoiseLayers::PCCPSFNoiseLayers( MetaProcess* p )
   : MetaInt32( p )
{
   ThePCCPSFNoiseLayersParameter = this;
}

IsoString PCCPSFNoiseLayers::Id() const
{
   return "psfNoiseLayers";
}

double PCCPSFNoiseLayers::DefaultValue() const
{
   return 1;
}

double PCCPSFNoiseLayers::MinimumValue() const
{
   return 0;
}

double PCCPSFNoiseLayers::MaximumValue() const
{
   return 4;
}

// ----------------------------------------------------------------------------

PCCPSFHotPixelFilterRadius::PCCPSFHotPixelFilterRadius( MetaProcess* p )
   : MetaInt32( p )
{
   ThePCCPSFHotPixelFilterRadiusParameter = this;
}

IsoString PCCPSFHotPixelFilterRadius::Id() const
{
   return "psfHotPixelFilterRadius";
}

double PCCPSFHotPixelFilterRadius::DefaultValue() const
{
   return 1;
}

double PCCPSFHotPixelFilterRadius::MinimumValue() const
{
   return 0;
}

double PCCPSFHotPixelFilterRadius::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

PCCPSFNoiseReductionFilterRadius::PCCPSFNoiseReductionFilterRadius( MetaProcess* p )
   : MetaInt32( p )
{
   ThePCCPSFNoiseReductionFilterRadiusParameter = this;
}

IsoString PCCPSFNoiseReductionFilterRadius::Id() const
{
   return "psfNoiseReductionFilterRadius";
}

double PCCPSFNoiseReductionFilterRadius::DefaultValue() const
{
   return 0;
}

double PCCPSFNoiseReductionFilterRadius::MinimumValue() const
{
   return 0;
}

double PCCPSFNoiseReductionFilterRadius::MaximumValue() const
{
   return 50;
}

// ----------------------------------------------------------------------------

PCCPSFMinStructureSize::PCCPSFMinStructureSize( MetaProcess* p )
   : MetaInt32( p )
{
   ThePCCPSFMinStructureSizeParameter = this;
}

IsoString PCCPSFMinStructureSize::Id() const
{
   return "psfMinStructureSize";
}

double PCCPSFMinStructureSize::DefaultValue() const
{
   return 0;
}

double PCCPSFMinStructureSize::MinimumValue() const
{
   return 0;
}

double PCCPSFMinStructureSize::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

PCCPSFMinSNR::PCCPSFMinSNR( MetaProcess* P )
   : MetaFloat( P )
{
   ThePCCPSFMinSNRParameter = this;
}

IsoString PCCPSFMinSNR::Id() const
{
   return "psfMinSNR";
}

int PCCPSFMinSNR::Precision() const
{
   return 2;
}

double PCCPSFMinSNR::DefaultValue() const
{
   return 40.0;
}

double PCCPSFMinSNR::MinimumValue() const
{
   return 0;
}

double PCCPSFMinSNR::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

PCCPSFAllowClusteredSources::PCCPSFAllowClusteredSources( MetaProcess* p )
   : MetaBoolean( p )
{
   ThePCCPSFAllowClusteredSourcesParameter = this;
}

IsoString PCCPSFAllowClusteredSources::Id() const
{
   return "psfAllowClusteredSources";
}

bool PCCPSFAllowClusteredSources::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCPSFType::PCCPSFType( MetaProcess* P )
   : MetaEnumeration( P )
{
   ThePCCPSFTypeParameter = this;
}

IsoString PCCPSFType::Id() const
{
   return "psfType";
}

size_type PCCPSFType::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString PCCPSFType::ElementId( size_type i ) const
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

IsoString PCCPSFType::FunctionName( pcl_enum i )
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

int PCCPSFType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type PCCPSFType::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

PCCPSFGrowth::PCCPSFGrowth( MetaProcess* P )
   : MetaFloat( P )
{
   ThePCCPSFGrowthParameter = this;
}

IsoString PCCPSFGrowth::Id() const
{
   return "psfGrowth";
}

int PCCPSFGrowth::Precision() const
{
   return 2;
}

double PCCPSFGrowth::DefaultValue() const
{
   return 1.25;
}

double PCCPSFGrowth::MinimumValue() const
{
   return 0.5;
}

double PCCPSFGrowth::MaximumValue() const
{
   return 4.0;
}

// ----------------------------------------------------------------------------

PCCPSFMaxStars::PCCPSFMaxStars( MetaProcess* p )
   : MetaInt32( p )
{
   ThePCCPSFMaxStarsParameter = this;
}

IsoString PCCPSFMaxStars::Id() const
{
   return "psfMaxStars";
}

double PCCPSFMaxStars::DefaultValue() const
{
   return 24*1024;
}

double PCCPSFMaxStars::MinimumValue() const
{
   return 256;
}

double PCCPSFMaxStars::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

PCCPSFSearchTolerance::PCCPSFSearchTolerance( MetaProcess* P )
   : MetaFloat( P )
{
   ThePCCPSFSearchToleranceParameter = this;
}

IsoString PCCPSFSearchTolerance::Id() const
{
   return "psfSearchTolerance";
}

int PCCPSFSearchTolerance::Precision() const
{
   return 2;
}

double PCCPSFSearchTolerance::DefaultValue() const
{
   return 4;
}

double PCCPSFSearchTolerance::MinimumValue() const
{
   return 0.1;
}

double PCCPSFSearchTolerance::MaximumValue() const
{
   return 16;
}

// ----------------------------------------------------------------------------

PCCPSFChannelSearchTolerance::PCCPSFChannelSearchTolerance( MetaProcess* P )
   : MetaFloat( P )
{
   ThePCCPSFChannelSearchToleranceParameter = this;
}

IsoString PCCPSFChannelSearchTolerance::Id() const
{
   return "psfChannelSearchTolerance";
}

int PCCPSFChannelSearchTolerance::Precision() const
{
   return 2;
}

double PCCPSFChannelSearchTolerance::DefaultValue() const
{
   return 2;
}

double PCCPSFChannelSearchTolerance::MinimumValue() const
{
   return 0.1;
}

double PCCPSFChannelSearchTolerance::MaximumValue() const
{
   return 16;
}

// ----------------------------------------------------------------------------

PCCNeutralizeBackground::PCCNeutralizeBackground( MetaProcess* P )
   : MetaBoolean( P )
{
   ThePCCNeutralizeBackgroundParameter = this;
}

IsoString PCCNeutralizeBackground::Id() const
{
   return "neutralizeBackground";
}

bool PCCNeutralizeBackground::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCBackgroundReferenceViewId::PCCBackgroundReferenceViewId( MetaProcess* P )
   : MetaString( P )
{
   ThePCCBackgroundReferenceViewIdParameter = this;
}

IsoString PCCBackgroundReferenceViewId::Id() const
{
   return "backgroundReferenceViewId";
}

// ----------------------------------------------------------------------------

PCCBackgroundLow::PCCBackgroundLow( MetaProcess* P )
   : MetaFloat( P )
{
   ThePCCBackgroundLowParameter = this;
}

IsoString PCCBackgroundLow::Id() const
{
   return "backgroundLow";
}

int PCCBackgroundLow::Precision() const
{
   return 2;
}

double PCCBackgroundLow::MinimumValue() const
{
   return -10;
}

double PCCBackgroundLow::MaximumValue() const
{
   return +10;
}

double PCCBackgroundLow::DefaultValue() const
{
   return -2.8;
}

// ----------------------------------------------------------------------------

PCCBackgroundHigh::PCCBackgroundHigh( MetaProcess* P )
   : MetaFloat( P )
{
   ThePCCBackgroundHighParameter = this;
}

IsoString PCCBackgroundHigh::Id() const
{
   return "backgroundHigh";
}

int PCCBackgroundHigh::Precision() const
{
   return 2;
}

double PCCBackgroundHigh::MinimumValue() const
{
   return -10;
}

double PCCBackgroundHigh::MaximumValue() const
{
   return +10;
}

double PCCBackgroundHigh::DefaultValue() const
{
   return +2.0;
}

// ----------------------------------------------------------------------------

PCCBackgroundUseROI::PCCBackgroundUseROI( MetaProcess* P )
   : MetaBoolean( P )
{
   ThePCCBackgroundUseROIParameter = this;
}

IsoString PCCBackgroundUseROI::Id() const
{
   return "backgroundUseROI";
}

bool PCCBackgroundUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PCCBackgroundROIX0::PCCBackgroundROIX0( MetaProcess* P )
   : MetaInt32( P )
{
   ThePCCBackgroundROIX0Parameter = this;
}

IsoString PCCBackgroundROIX0::Id() const
{
   return "backgroundROIX0";
}

double PCCBackgroundROIX0::DefaultValue() const
{
   return 0;
}

double PCCBackgroundROIX0::MinimumValue() const
{
   return 0;
}

double PCCBackgroundROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

PCCBackgroundROIY0::PCCBackgroundROIY0( MetaProcess* P )
   : MetaInt32( P )
{
   ThePCCBackgroundROIY0Parameter = this;
}

IsoString PCCBackgroundROIY0::Id() const
{
   return "backgroundROIY0";
}

double PCCBackgroundROIY0::DefaultValue() const
{
   return 0;
}

double PCCBackgroundROIY0::MinimumValue() const
{
   return 0;
}

double PCCBackgroundROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

PCCBackgroundROIX1::PCCBackgroundROIX1( MetaProcess* P )
   : MetaInt32( P )
{
   ThePCCBackgroundROIX1Parameter = this;
}

IsoString PCCBackgroundROIX1::Id() const
{
   return "backgroundROIX1";
}

double PCCBackgroundROIX1::DefaultValue() const
{
   return 0;
}

double PCCBackgroundROIX1::MinimumValue() const
{
   return 0;
}

double PCCBackgroundROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

PCCBackgroundROIY1::PCCBackgroundROIY1( MetaProcess* P )
   : MetaInt32( P )
{
   ThePCCBackgroundROIY1Parameter = this;
}

IsoString PCCBackgroundROIY1::Id() const
{
   return "backgroundROIY1";
}

double PCCBackgroundROIY1::DefaultValue() const
{
   return 0;
}

double PCCBackgroundROIY1::MinimumValue() const
{
   return 0;
}

double PCCBackgroundROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

PCCGenerateGraphs::PCCGenerateGraphs( MetaProcess* P )
   : MetaBoolean( P )
{
   ThePCCGenerateGraphsParameter = this;
}

IsoString PCCGenerateGraphs::Id() const
{
   return "generateGraphs";
}

bool PCCGenerateGraphs::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCGenerateStarMaps::PCCGenerateStarMaps( MetaProcess* P )
   : MetaBoolean( P )
{
   ThePCCGenerateStarMapsParameter = this;
}

IsoString PCCGenerateStarMaps::Id() const
{
   return "generateStarMaps";
}

bool PCCGenerateStarMaps::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PCCGenerateTextFiles::PCCGenerateTextFiles( MetaProcess* P )
   : MetaBoolean( P )
{
   ThePCCGenerateTextFilesParameter = this;
}

IsoString PCCGenerateTextFiles::Id() const
{
   return "generateTextFiles";
}

bool PCCGenerateTextFiles::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PCCOutputDirectory::PCCOutputDirectory( MetaProcess* P )
   : MetaString( P )
{
   ThePCCOutputDirectoryParameter = this;
}

IsoString PCCOutputDirectory::Id() const
{
   return "outputDirectory";
}

String PCCOutputDirectory::DefaultValue() const
{
   return String(); // = File::SystemTempDirectory()
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationParameters.cpp - Released 2023-08-01T16:30:17Z
