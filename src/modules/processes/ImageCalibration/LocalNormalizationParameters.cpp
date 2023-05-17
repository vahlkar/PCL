//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.3
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.9.8
// ----------------------------------------------------------------------------
// LocalNormalizationParameters.cpp - Released 2023-05-17T17:06:42Z
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

#include "LocalNormalizationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LNScale*                         TheLNScaleParameter = nullptr;
LNNoScale*                       TheLNNoScaleParameter = nullptr;
LNGlobalLocationNormalization*   TheLNGlobalLocationNormalizationParameter = nullptr;
LNTruncate*                      TheLNTruncateParameter = nullptr;
LNBackgroundSamplingDelta*       TheLNBackgroundSamplingDeltaParameter = nullptr;
LNRejection*                     TheLNRejectionParameter = nullptr;
LNReferenceRejection*            TheLNReferenceRejectionParameter = nullptr;
LNLowClippingLevel*              TheLNLowClippingLevelParameter = nullptr;
LNHighClippingLevel*             TheLNHighClippingLevelParameter = nullptr;
LNReferenceRejectionThreshold*   TheLNReferenceRejectionThresholdParameter = nullptr;
LNTargetRejectionThreshold*      TheLNTargetRejectionThresholdParameter = nullptr;
LNHotPixelFilterRadius*          TheLNHotPixelFilterRadiusParameter = nullptr;
LNNoiseReductionFilterRadius*    TheLNNoiseReductionFilterRadiusParameter = nullptr;
LNModelScalingFactor*            TheLNModelScalingFactorParameter = nullptr;
LNScaleEvaluationMethod*         TheLNScaleEvaluationMethodParameter = nullptr;
LNLocalScaleCorrections*         TheLNLocalScaleCorrectionsParameter = nullptr;
LNStructureLayers*               TheLNStructureLayersParameter = nullptr;
LNSaturationThreshold*           TheLNSaturationThresholdParameter = nullptr;
LNSaturationRelative*            TheLNSaturationRelativeParameter = nullptr;
LNRejectionLimit*                TheLNRejectionLimitParameter = nullptr;
LNPSFNoiseLayers*                TheLNPSFNoiseLayersParameter = nullptr;
LNPSFHotPixelFilterRadius*       TheLNPSFHotPixelFilterRadiusParameter = nullptr;
LNPSFNoiseReductionFilterRadius* TheLNPSFNoiseReductionFilterRadiusParameter = nullptr;
LNPSFMinStructureSize*           TheLNPSFMinStructureSizeParameter = nullptr;
LNPSFMinSNR*                     TheLNPSFMinSNRParameter = nullptr;
LNPSFAllowClusteredSources*      TheLNPSFAllowClusteredSourcesParameter = nullptr;
LNPSFType*                       TheLNPSFTypeParameter = nullptr;
LNPSFGrowth*                     TheLNPSFGrowthParameter = nullptr;
LNPSFMaxStars*                   TheLNPSFMaxStarsParameter = nullptr;
LNReferencePathOrViewId*         TheLNReferencePathOrViewIdParameter = nullptr;
LNReferenceIsView*               TheLNReferenceIsViewParameter = nullptr;
LNTargetItems*                   TheLNTargetItemsParameter = nullptr;
LNTargetEnabled*                 TheLNTargetEnabledParameter = nullptr;
LNTargetImage*                   TheLNTargetImageParameter = nullptr;
LNInputHints*                    TheLNInputHintsParameter = nullptr;
LNOutputHints*                   TheLNOutputHintsParameter = nullptr;
LNGenerateNormalizedImages*      TheLNGenerateNormalizedImagesParameter = nullptr;
LNGenerateNormalizationData*     TheLNGenerateNormalizationDataParameter = nullptr;
LNGenerateInvalidData*           TheLNGenerateInvalidDataParameter = nullptr;
LNShowBackgroundModels*          TheLNShowBackgroundModelsParameter = nullptr;
LNShowLocalScaleModels*          TheLNShowLocalScaleModelsParameter = nullptr;
LNShowRejectionMaps*             TheLNShowRejectionMapsParameter = nullptr;
LNShowStructureMaps*             TheLNShowStructureMapsParameter = nullptr;
LNPlotNormalizationFunctions*    TheLNPlotNormalizationFunctionsParameter = nullptr;
LNNoGUIMessages*                 TheLNNoGUIMessagesParameter = nullptr;
LNAutoMemoryLimit*               TheLNAutoMemoryLimitParameter = nullptr;
LNOutputDirectory*               TheLNOutputDirectoryParameter = nullptr;
LNOutputExtension*               TheLNOutputExtensionParameter = nullptr;
LNOutputPrefix*                  TheLNOutputPrefixParameter = nullptr;
LNOutputPostfix*                 TheLNOutputPostfixParameter = nullptr;
LNOverwriteExistingFiles*        TheLNOverwriteExistingFilesParameter = nullptr;
LNOnError*                       TheLNOnErrorParameter = nullptr;
LNUseFileThreads*                TheLNUseFileThreadsParameter = nullptr;
LNFileThreadOverload*            TheLNFileThreadOverloadParameter = nullptr;
LNMaxFileReadThreads*            TheLNMaxFileReadThreadsParameter = nullptr;
LNMaxFileWriteThreads*           TheLNMaxFileWriteThreadsParameter = nullptr;
LNGraphSize*                     TheLNGraphSizeParameter = nullptr;
LNGraphTextSize*                 TheLNGraphTextSizeParameter = nullptr;
LNGraphTitleSize*                TheLNGraphTitleSizeParameter = nullptr;
LNGraphTransparent*              TheLNGraphTransparentParameter = nullptr;
LNGraphOutputDirectory*          TheLNGraphOutputDirectoryParameter = nullptr;

LNOutputData*                    TheLNOutputDataParameter = nullptr;
LNOutputFilePathXNML*            TheLNOutputFilePathXNMLParameter = nullptr;
LNOutputFilePath*                TheLNOutputFilePathParameter = nullptr;
LNScaleFactorRK*                 TheLNScaleFactorRKParameter = nullptr;
LNScaleFactorG*                  TheLNScaleFactorGParameter = nullptr;
LNScaleFactorB*                  TheLNScaleFactorBParameter = nullptr;
LNValid*                         TheLNValidParameter = nullptr;

// ----------------------------------------------------------------------------

LNScale::LNScale( MetaProcess* P ) : MetaInt32( P )
{
   TheLNScaleParameter = this;
}

IsoString LNScale::Id() const
{
   return "scale";
}

double LNScale::MinimumValue() const
{
   return 32;
}

double LNScale::MaximumValue() const
{
   return 8192;
}

double LNScale::DefaultValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

LNNoScale::LNNoScale( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNNoScaleParameter = this;
}

IsoString LNNoScale::Id() const
{
   return "noScale";
}

bool LNNoScale::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNGlobalLocationNormalization::LNGlobalLocationNormalization( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNGlobalLocationNormalizationParameter = this;
}

IsoString LNGlobalLocationNormalization::Id() const
{
   return "globalLocationNormalization";
}

bool LNGlobalLocationNormalization::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNTruncate::LNTruncate( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNTruncateParameter = this;
}

IsoString LNTruncate::Id() const
{
   return "truncate";
}

bool LNTruncate::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNBackgroundSamplingDelta::LNBackgroundSamplingDelta( MetaProcess* P ) : MetaInt32( P )
{
   TheLNBackgroundSamplingDeltaParameter = this;
}

IsoString LNBackgroundSamplingDelta::Id() const
{
   return "backgroundSamplingDelta";
}

double LNBackgroundSamplingDelta::MinimumValue() const
{
   return 8;
}

double LNBackgroundSamplingDelta::MaximumValue() const
{
   return 64;
}

double LNBackgroundSamplingDelta::DefaultValue() const
{
   return 32;
}

// ----------------------------------------------------------------------------

LNRejection::LNRejection( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNRejectionParameter = this;
}

IsoString LNRejection::Id() const
{
   return "rejection";
}

bool LNRejection::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNReferenceRejection::LNReferenceRejection( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNReferenceRejectionParameter = this;
}

IsoString LNReferenceRejection::Id() const
{
   return "referenceRejection";
}

bool LNReferenceRejection::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNLowClippingLevel::LNLowClippingLevel( MetaProcess* P ) : MetaDouble( P )
{
   TheLNLowClippingLevelParameter = this;
}

IsoString LNLowClippingLevel::Id() const
{
   return "lowClippingLevel";
}

int LNLowClippingLevel::Precision() const
{
   return 6;
}

double LNLowClippingLevel::DefaultValue() const
{
   return 4.5e-5; // approx. 3/65535
}

double LNLowClippingLevel::MinimumValue() const
{
   return 0.0;
}

double LNLowClippingLevel::MaximumValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

LNHighClippingLevel::LNHighClippingLevel( MetaProcess* P ) : MetaFloat( P )
{
   TheLNHighClippingLevelParameter = this;
}

IsoString LNHighClippingLevel::Id() const
{
   return "highClippingLevel";
}

int LNHighClippingLevel::Precision() const
{
   return 2;
}

double LNHighClippingLevel::DefaultValue() const
{
   return 0.85;
}

double LNHighClippingLevel::MinimumValue() const
{
   return 0.5;
}

double LNHighClippingLevel::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

LNReferenceRejectionThreshold::LNReferenceRejectionThreshold( MetaProcess* P ) : MetaFloat( P )
{
   TheLNReferenceRejectionThresholdParameter = this;
}

IsoString LNReferenceRejectionThreshold::Id() const
{
   return "referenceRejectionThreshold";
}

int LNReferenceRejectionThreshold::Precision() const
{
   return 2;
}

double LNReferenceRejectionThreshold::DefaultValue() const
{
   return 3;
}

double LNReferenceRejectionThreshold::MinimumValue() const
{
   return 0;
}

double LNReferenceRejectionThreshold::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

LNTargetRejectionThreshold::LNTargetRejectionThreshold( MetaProcess* P ) : MetaFloat( P )
{
   TheLNTargetRejectionThresholdParameter = this;
}

IsoString LNTargetRejectionThreshold::Id() const
{
   return "targetRejectionThreshold";
}

int LNTargetRejectionThreshold::Precision() const
{
   return 2;
}

double LNTargetRejectionThreshold::DefaultValue() const
{
   return 3.2;
}

double LNTargetRejectionThreshold::MinimumValue() const
{
   return 0;
}

double LNTargetRejectionThreshold::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

LNHotPixelFilterRadius::LNHotPixelFilterRadius( MetaProcess* p ) : MetaInt32( p )
{
   TheLNHotPixelFilterRadiusParameter = this;
}

IsoString LNHotPixelFilterRadius::Id() const
{
   return "hotPixelFilterRadius";
}

double LNHotPixelFilterRadius::DefaultValue() const
{
   return 2;
}

double LNHotPixelFilterRadius::MinimumValue() const
{
   return 0;
}

double LNHotPixelFilterRadius::MaximumValue() const
{
   return 4;
}

// ----------------------------------------------------------------------------

LNNoiseReductionFilterRadius::LNNoiseReductionFilterRadius( MetaProcess* p ) : MetaInt32( p )
{
   TheLNNoiseReductionFilterRadiusParameter = this;
}

IsoString LNNoiseReductionFilterRadius::Id() const
{
   return "noiseReductionFilterRadius";
}

double LNNoiseReductionFilterRadius::DefaultValue() const
{
   return 0;
}

double LNNoiseReductionFilterRadius::MinimumValue() const
{
   return 0;
}

double LNNoiseReductionFilterRadius::MaximumValue() const
{
   return 24;
}

// ----------------------------------------------------------------------------

LNModelScalingFactor::LNModelScalingFactor( MetaProcess* P ) : MetaFloat( P )
{
   TheLNModelScalingFactorParameter = this;
}

IsoString LNModelScalingFactor::Id() const
{
   return "modelScalingFactor";
}

int LNModelScalingFactor::Precision() const
{
   return 2;
}

double LNModelScalingFactor::DefaultValue() const
{
   return 8;
}

double LNModelScalingFactor::MinimumValue() const
{
   return 1;
}

double LNModelScalingFactor::MaximumValue() const
{
   return 32; // same as minimum scale
}

// ----------------------------------------------------------------------------

LNScaleEvaluationMethod::LNScaleEvaluationMethod( MetaProcess* p ) : MetaEnumeration( p )
{
   TheLNScaleEvaluationMethodParameter = this;
}

IsoString LNScaleEvaluationMethod::Id() const
{
   return "scaleEvaluationMethod";
}

size_type LNScaleEvaluationMethod::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString LNScaleEvaluationMethod::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case PSFSignal:          return "ScaleEvaluationMethod_PSFSignal";
   case MultiscaleAnalysis: return "ScaleEvaluationMethod_MultiscaleAnalysis";
   }
}

int LNScaleEvaluationMethod::ElementValue( size_type i ) const
{
   return int( i );
}

size_type LNScaleEvaluationMethod::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

LNLocalScaleCorrections::LNLocalScaleCorrections( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNLocalScaleCorrectionsParameter = this;
}

IsoString LNLocalScaleCorrections::Id() const
{
   return "localScaleCorrections";
}

bool LNLocalScaleCorrections::DefaultValue() const
{
   return false; // N.B. Local scale corrections are an experimental feature
}

// ----------------------------------------------------------------------------

LNStructureLayers::LNStructureLayers( MetaProcess* p ) : MetaInt32( p )
{
   TheLNStructureLayersParameter = this;
}

IsoString LNStructureLayers::Id() const
{
   return "psfStructureLayers";
}

double LNStructureLayers::DefaultValue() const
{
   return 5;
}

double LNStructureLayers::MinimumValue() const
{
   return 1;
}

double LNStructureLayers::MaximumValue() const
{
   return 8;
}

// ----------------------------------------------------------------------------

LNSaturationThreshold::LNSaturationThreshold( MetaProcess* P ) : MetaFloat( P )
{
   TheLNSaturationThresholdParameter = this;
}

IsoString LNSaturationThreshold::Id() const
{
   return "saturationThreshold";
}

int LNSaturationThreshold::Precision() const
{
   return 2;
}

double LNSaturationThreshold::DefaultValue() const
{
   return 0.75;
}

double LNSaturationThreshold::MinimumValue() const
{
   return 0.10;
}

double LNSaturationThreshold::MaximumValue() const
{
   return 1.00;
}

// ----------------------------------------------------------------------------

LNSaturationRelative::LNSaturationRelative( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNSaturationRelativeParameter = this;
}

IsoString LNSaturationRelative::Id() const
{
   return "saturationRelative";
}

bool LNSaturationRelative::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNPSFNoiseLayers::LNPSFNoiseLayers( MetaProcess* p ) : MetaInt32( p )
{
   TheLNPSFNoiseLayersParameter = this;
}

IsoString LNPSFNoiseLayers::Id() const
{
   return "psfNoiseLayers";
}

double LNPSFNoiseLayers::DefaultValue() const
{
   return 1;
}

double LNPSFNoiseLayers::MinimumValue() const
{
   return 0;
}

double LNPSFNoiseLayers::MaximumValue() const
{
   return 4;
}

// ----------------------------------------------------------------------------

LNPSFHotPixelFilterRadius::LNPSFHotPixelFilterRadius( MetaProcess* p ) : MetaInt32( p )
{
   TheLNPSFHotPixelFilterRadiusParameter = this;
}

IsoString LNPSFHotPixelFilterRadius::Id() const
{
   return "psfHotPixelFilterRadius";
}

double LNPSFHotPixelFilterRadius::DefaultValue() const
{
   return 1;
}

double LNPSFHotPixelFilterRadius::MinimumValue() const
{
   return 0;
}

double LNPSFHotPixelFilterRadius::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

LNPSFNoiseReductionFilterRadius::LNPSFNoiseReductionFilterRadius( MetaProcess* p ) : MetaInt32( p )
{
   TheLNPSFNoiseReductionFilterRadiusParameter = this;
}

IsoString LNPSFNoiseReductionFilterRadius::Id() const
{
   return "psfNoiseReductionFilterRadius";
}

double LNPSFNoiseReductionFilterRadius::DefaultValue() const
{
   return 0;
}

double LNPSFNoiseReductionFilterRadius::MinimumValue() const
{
   return 0;
}

double LNPSFNoiseReductionFilterRadius::MaximumValue() const
{
   return 50;
}

// ----------------------------------------------------------------------------

LNPSFMinStructureSize::LNPSFMinStructureSize( MetaProcess* p ) : MetaInt32( p )
{
   TheLNPSFMinStructureSizeParameter = this;
}

IsoString LNPSFMinStructureSize::Id() const
{
   return "psfMinStructureSize";
}

double LNPSFMinStructureSize::DefaultValue() const
{
   return 0;
}

double LNPSFMinStructureSize::MinimumValue() const
{
   return 0;
}

double LNPSFMinStructureSize::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

LNPSFMinSNR::LNPSFMinSNR( MetaProcess* P ) : MetaFloat( P )
{
   TheLNPSFMinSNRParameter = this;
}

IsoString LNPSFMinSNR::Id() const
{
   return "psfMinSNR";
}

int LNPSFMinSNR::Precision() const
{
   return 2;
}

double LNPSFMinSNR::DefaultValue() const
{
   return 40.0;
}

double LNPSFMinSNR::MinimumValue() const
{
   return 0;
}

double LNPSFMinSNR::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

LNPSFAllowClusteredSources::LNPSFAllowClusteredSources( MetaProcess* p ) : MetaBoolean( p )
{
   TheLNPSFAllowClusteredSourcesParameter = this;
}

IsoString LNPSFAllowClusteredSources::Id() const
{
   return "psfAllowClusteredSources";
}

bool LNPSFAllowClusteredSources::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNRejectionLimit::LNRejectionLimit( MetaProcess* P ) : MetaFloat( P )
{
   TheLNRejectionLimitParameter = this;
}

IsoString LNRejectionLimit::Id() const
{
   return "rejectionLimit";
}

IsoString LNRejectionLimit::Aliases() const
{
   return "psfRejectionLimit";
}

int LNRejectionLimit::Precision() const
{
   return 2;
}

double LNRejectionLimit::DefaultValue() const
{
   return 0.3;
}

double LNRejectionLimit::MinimumValue() const
{
   return 0;
}

double LNRejectionLimit::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

LNPSFType::LNPSFType( MetaProcess* P ) : MetaEnumeration( P )
{
   TheLNPSFTypeParameter = this;
}

IsoString LNPSFType::Id() const
{
   return "psfType";
}

size_type LNPSFType::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString LNPSFType::ElementId( size_type i ) const
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

IsoString LNPSFType::FunctionName( pcl_enum i )
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

int LNPSFType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type LNPSFType::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

LNPSFGrowth::LNPSFGrowth( MetaProcess* P ) : MetaFloat( P )
{
   TheLNPSFGrowthParameter = this;
}

IsoString LNPSFGrowth::Id() const
{
   return "psfGrowth";
}

int LNPSFGrowth::Precision() const
{
   return 2;
}

double LNPSFGrowth::DefaultValue() const
{
   return 1.0;
}

double LNPSFGrowth::MinimumValue() const
{
   return 0.5;
}

double LNPSFGrowth::MaximumValue() const
{
   return 4.0;
}

// ----------------------------------------------------------------------------

LNPSFMaxStars::LNPSFMaxStars( MetaProcess* p ) : MetaInt32( p )
{
   TheLNPSFMaxStarsParameter = this;
}

IsoString LNPSFMaxStars::Id() const
{
   return "psfMaxStars";
}

double LNPSFMaxStars::DefaultValue() const
{
   return 24*1024;
}

double LNPSFMaxStars::MinimumValue() const
{
   return 256;
}

double LNPSFMaxStars::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

LNReferencePathOrViewId::LNReferencePathOrViewId( MetaProcess* P ) : MetaString( P )
{
   TheLNReferencePathOrViewIdParameter = this;
}

IsoString LNReferencePathOrViewId::Id() const
{
   return "referencePathOrViewId";
}

// ----------------------------------------------------------------------------

LNReferenceIsView::LNReferenceIsView( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNReferenceIsViewParameter = this;
}

IsoString LNReferenceIsView::Id() const
{
   return "referenceIsView";
}

bool LNReferenceIsView::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNTargetItems::LNTargetItems( MetaProcess* P ) : MetaTable( P )
{
   TheLNTargetItemsParameter = this;
}

IsoString LNTargetItems::Id() const
{
   return "targetItems";
}

// ----------------------------------------------------------------------------

LNTargetEnabled::LNTargetEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheLNTargetEnabledParameter = this;
}

IsoString LNTargetEnabled::Id() const
{
   return "enabled";
}

bool LNTargetEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNTargetImage::LNTargetImage( MetaTable* T ) : MetaString( T )
{
   TheLNTargetImageParameter = this;
}

IsoString LNTargetImage::Id() const
{
   return "image";
}

// ----------------------------------------------------------------------------

LNInputHints::LNInputHints( MetaProcess* P ) : MetaString( P )
{
   TheLNInputHintsParameter = this;
}

IsoString LNInputHints::Id() const
{
   return "inputHints";
}

// ----------------------------------------------------------------------------

LNOutputHints::LNOutputHints( MetaProcess* P ) : MetaString( P )
{
   TheLNOutputHintsParameter = this;
}

IsoString LNOutputHints::Id() const
{
   return "outputHints";
}

// ----------------------------------------------------------------------------

LNGenerateNormalizedImages::LNGenerateNormalizedImages( MetaProcess* p ) : MetaEnumeration( p )
{
   TheLNGenerateNormalizedImagesParameter = this;
}

IsoString LNGenerateNormalizedImages::Id() const
{
   return "generateNormalizedImages";
}

size_type LNGenerateNormalizedImages::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString LNGenerateNormalizedImages::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Never:               return "GenerateNormalizedImages_Never";
   case Always:              return "GenerateNormalizedImages_Always";
   default:
   case ViewExecutionOnly:   return "GenerateNormalizedImages_ViewExecutionOnly";
   case GlobalExecutionOnly: return "GenerateNormalizedImages_GlobalExecutionOnly";
   }
}

int LNGenerateNormalizedImages::ElementValue( size_type i ) const
{
   return int( i );
}

size_type LNGenerateNormalizedImages::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

LNGenerateNormalizationData::LNGenerateNormalizationData( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNGenerateNormalizationDataParameter = this;
}

IsoString LNGenerateNormalizationData::Id() const
{
   return "generateNormalizationData";
}

bool LNGenerateNormalizationData::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNGenerateInvalidData::LNGenerateInvalidData( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNGenerateInvalidDataParameter = this;
}

IsoString LNGenerateInvalidData::Id() const
{
   return "generateInvalidData";
}

bool LNGenerateInvalidData::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNShowBackgroundModels::LNShowBackgroundModels( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNShowBackgroundModelsParameter = this;
}

IsoString LNShowBackgroundModels::Id() const
{
   return "showBackgroundModels";
}

bool LNShowBackgroundModels::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNShowLocalScaleModels::LNShowLocalScaleModels( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNShowLocalScaleModelsParameter = this;
}

IsoString LNShowLocalScaleModels::Id() const
{
   return "showLocalScaleModels";
}

bool LNShowLocalScaleModels::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNShowRejectionMaps::LNShowRejectionMaps( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNShowRejectionMapsParameter = this;
}

IsoString LNShowRejectionMaps::Id() const
{
   return "showRejectionMaps";
}

bool LNShowRejectionMaps::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNShowStructureMaps::LNShowStructureMaps( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNShowStructureMapsParameter = this;
}

IsoString LNShowStructureMaps::Id() const
{
   return "showStructureMaps";
}

bool LNShowStructureMaps::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNPlotNormalizationFunctions::LNPlotNormalizationFunctions( MetaProcess* p ) : MetaEnumeration( p )
{
   TheLNPlotNormalizationFunctionsParameter = this;
}

IsoString LNPlotNormalizationFunctions::Id() const
{
   return "plotNormalizationFunctions";
}

size_type LNPlotNormalizationFunctions::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString LNPlotNormalizationFunctions::ElementId( size_type i ) const
{
   switch ( i )
   {
   case DontPlot:  return "PlotNormalizationFunctions_DontPlot";
   case Line3D:    return "PlotNormalizationFunctions_Line3D";
   case Palette3D: return "PlotNormalizationFunctions_Palette3D";
   default:
   case Map3D:     return "PlotNormalizationFunctions_Map3D";
   }
}

int LNPlotNormalizationFunctions::ElementValue( size_type i ) const
{
   return int( i );
}

size_type LNPlotNormalizationFunctions::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

// ### DEPRECATED
LNNoGUIMessages::LNNoGUIMessages( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNNoGUIMessagesParameter = this;
}

IsoString LNNoGUIMessages::Id() const
{
   return "noGUIMessages";
}

bool LNNoGUIMessages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNAutoMemoryLimit::LNAutoMemoryLimit( MetaProcess* P ) : MetaFloat( P )
{
   TheLNAutoMemoryLimitParameter = this;
}

IsoString LNAutoMemoryLimit::Id() const
{
   return "autoMemoryLimit";
}

int LNAutoMemoryLimit::Precision() const
{
   return 2;
}

double LNAutoMemoryLimit::DefaultValue() const
{
   return 0.85;
}

double LNAutoMemoryLimit::MinimumValue() const
{
   return 0.10;
}

double LNAutoMemoryLimit::MaximumValue() const
{
   return 1.00;
}

// ----------------------------------------------------------------------------

LNOutputDirectory::LNOutputDirectory( MetaProcess* P ) : MetaString( P )
{
   TheLNOutputDirectoryParameter = this;
}

IsoString LNOutputDirectory::Id() const
{
   return "outputDirectory";
}

// ----------------------------------------------------------------------------

LNOutputExtension::LNOutputExtension( MetaProcess* P ) : MetaString( P )
{
   TheLNOutputExtensionParameter = this;
}

IsoString LNOutputExtension::Id() const
{
   return "outputExtension";
}

String LNOutputExtension::DefaultValue() const
{
   return ".xisf";
}

// ----------------------------------------------------------------------------

LNOutputPrefix::LNOutputPrefix( MetaProcess* P ) : MetaString( P )
{
   TheLNOutputPrefixParameter = this;
}

IsoString LNOutputPrefix::Id() const
{
   return "outputPrefix";
}

String LNOutputPrefix::DefaultValue() const
{
   return String();
}

// ----------------------------------------------------------------------------

LNOutputPostfix::LNOutputPostfix( MetaProcess* P ) : MetaString( P )
{
   TheLNOutputPostfixParameter = this;
}

IsoString LNOutputPostfix::Id() const
{
   return "outputPostfix";
}

String LNOutputPostfix::DefaultValue() const
{
   return "_n";
}

// ----------------------------------------------------------------------------

LNOverwriteExistingFiles::LNOverwriteExistingFiles( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNOverwriteExistingFilesParameter = this;
}

IsoString LNOverwriteExistingFiles::Id() const
{
   return "overwriteExistingFiles";
}

bool LNOverwriteExistingFiles::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNOnError::LNOnError( MetaProcess* p ) : MetaEnumeration( p )
{
   TheLNOnErrorParameter = this;
}

IsoString LNOnError::Id() const
{
   return "onError";
}

size_type LNOnError::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString LNOnError::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Continue: return "OnError_Continue";
   case Abort:    return "OnError_Abort";
   case AskUser:  return "OnError_AskUser";
   }
}

int LNOnError::ElementValue( size_type i ) const
{
   return int( i );
}

size_type LNOnError::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

LNUseFileThreads::LNUseFileThreads( MetaProcess* p ) : MetaBoolean( p )
{
   TheLNUseFileThreadsParameter = this;
}

IsoString LNUseFileThreads::Id() const
{
   return "useFileThreads";
}

bool LNUseFileThreads::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNFileThreadOverload::LNFileThreadOverload( MetaProcess* p ) : MetaFloat( p )
{
   TheLNFileThreadOverloadParameter = this;
}

IsoString LNFileThreadOverload::Id() const
{
   return "fileThreadOverload";
}

int LNFileThreadOverload::Precision() const
{
   return 2;
}

double LNFileThreadOverload::DefaultValue() const
{
   return 1.0;
}

double LNFileThreadOverload::MinimumValue() const
{
   return 1;
}

double LNFileThreadOverload::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

LNMaxFileReadThreads::LNMaxFileReadThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheLNMaxFileReadThreadsParameter = this;
}

IsoString LNMaxFileReadThreads::Id() const
{
   return "maxFileReadThreads";
}

double LNMaxFileReadThreads::DefaultValue() const
{
   return 0;
}

double LNMaxFileReadThreads::MinimumValue() const
{
   return 0;
}

double LNMaxFileReadThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

LNMaxFileWriteThreads::LNMaxFileWriteThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheLNMaxFileWriteThreadsParameter = this;
}

IsoString LNMaxFileWriteThreads::Id() const
{
   return "maxFileWriteThreads";
}

double LNMaxFileWriteThreads::DefaultValue() const
{
   return 0;
}

double LNMaxFileWriteThreads::MinimumValue() const
{
   return 0;
}

double LNMaxFileWriteThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

LNGraphSize::LNGraphSize( MetaProcess* p ) : MetaInt32( p )
{
   TheLNGraphSizeParameter = this;
}

IsoString LNGraphSize::Id() const
{
   return "graphSize";
}

double LNGraphSize::DefaultValue() const
{
   return 1024;
}

double LNGraphSize::MinimumValue() const
{
   return 256;
}

double LNGraphSize::MaximumValue() const
{
   return 8192;
}

// ----------------------------------------------------------------------------

LNGraphTextSize::LNGraphTextSize( MetaProcess* p ) : MetaInt32( p )
{
   TheLNGraphTextSizeParameter = this;
}

IsoString LNGraphTextSize::Id() const
{
   return "graphTextSize";
}

double LNGraphTextSize::DefaultValue() const
{
   return 12;
}

double LNGraphTextSize::MinimumValue() const
{
   return 8;
}

double LNGraphTextSize::MaximumValue() const
{
   return 64;
}

// ----------------------------------------------------------------------------

LNGraphTitleSize::LNGraphTitleSize( MetaProcess* p ) : MetaInt32( p )
{
   TheLNGraphTitleSizeParameter = this;
}

IsoString LNGraphTitleSize::Id() const
{
   return "graphTitleSize";
}

double LNGraphTitleSize::DefaultValue() const
{
   return 18;
}

double LNGraphTitleSize::MinimumValue() const
{
   return 8;
}

double LNGraphTitleSize::MaximumValue() const
{
   return 64;
}

// ----------------------------------------------------------------------------

LNGraphTransparent::LNGraphTransparent( MetaProcess* p ) : MetaBoolean( p )
{
   TheLNGraphTransparentParameter = this;
}

IsoString LNGraphTransparent::Id() const
{
   return "graphTransparent";
}

bool LNGraphTransparent::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNGraphOutputDirectory::LNGraphOutputDirectory( MetaProcess* P ) : MetaString( P )
{
   TheLNGraphOutputDirectoryParameter = this;
}

IsoString LNGraphOutputDirectory::Id() const
{
   return "graphOutputDirectory";
}

String LNGraphOutputDirectory::DefaultValue() const
{
   return String(); // use TMPDIR by default
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

LNOutputData::LNOutputData( MetaProcess* P ) : MetaTable( P )
{
   TheLNOutputDataParameter = this;
}

IsoString LNOutputData::Id() const
{
   return "outputData";
}

bool LNOutputData::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNOutputFilePathXNML::LNOutputFilePathXNML( MetaTable* T ) : MetaString( T )
{
   TheLNOutputFilePathXNMLParameter = this;
}

IsoString LNOutputFilePathXNML::Id() const
{
   return "outputFilePathXNML";
}

bool LNOutputFilePathXNML::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNOutputFilePath::LNOutputFilePath( MetaTable* T ) : MetaString( T )
{
   TheLNOutputFilePathParameter = this;
}

IsoString LNOutputFilePath::Id() const
{
   return "outputFilePath";
}

bool LNOutputFilePath::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNScaleFactorRK::LNScaleFactorRK( MetaTable* T ) : MetaFloat( T )
{
   TheLNScaleFactorRKParameter = this;
}

IsoString LNScaleFactorRK::Id() const
{
   return "scaleFactorRK";
}

int LNScaleFactorRK::Precision() const
{
   return 4;
}

bool LNScaleFactorRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNScaleFactorG::LNScaleFactorG( MetaTable* T ) : MetaFloat( T )
{
   TheLNScaleFactorGParameter = this;
}

IsoString LNScaleFactorG::Id() const
{
   return "scaleFactorG";
}

int LNScaleFactorG::Precision() const
{
   return 4;
}

bool LNScaleFactorG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNScaleFactorB::LNScaleFactorB( MetaTable* T ) : MetaFloat( T )
{
   TheLNScaleFactorBParameter = this;
}

IsoString LNScaleFactorB::Id() const
{
   return "scaleFactorB";
}

int LNScaleFactorB::Precision() const
{
   return 4;
}

bool LNScaleFactorB::IsReadOnly() const
{
   return true;
}


// ----------------------------------------------------------------------------

LNValid::LNValid( MetaTable* T ) : MetaBoolean( T )
{
   TheLNValidParameter = this;
}

IsoString LNValid::Id() const
{
   return "valid";
}

bool LNValid::DefaultValue() const
{
   return true;
}

bool LNValid::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalNormalizationParameters.cpp - Released 2023-05-17T17:06:42Z
