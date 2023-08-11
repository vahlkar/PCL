//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.7
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 1.11.0
// ----------------------------------------------------------------------------
// DebayerParameters.cpp - Released 2023-08-10T11:44:14Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

#include "DebayerParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DebayerBayerPatternParameter*	                   TheDebayerBayerPatternParameter = nullptr;
DebayerMethodParameter*                          TheDebayerMethodParameter = nullptr;
DebayerFBDDNoiseReduction*                       TheDebayerFBDDNoiseReductionParameter = nullptr;
DebayerShowImages*                               TheDebayerShowImagesParameter = nullptr;
DebayerCFASourceFilePath*                        TheDebayerCFASourceFilePathParameter = nullptr;
DebayerTargetItems*                              TheDebayerTargetItemsParameter = nullptr;
DebayerTargetEnabled*                            TheDebayerTargetEnabledParameter = nullptr;
DebayerTargetImage*                              TheDebayerTargetImageParameter = nullptr;
DebayerNoGUIMessages*                            TheDebayerNoGUIMessagesParameter = nullptr;

DebayerEvaluateNoise*                            TheDebayerEvaluateNoiseParameter = nullptr;
DebayerNoiseEvaluationAlgorithm*                 TheDebayerNoiseEvaluationAlgorithmParameter = nullptr;

DebayerEvaluateSignal*                           TheDebayerEvaluateSignalParameter = nullptr;
DebayerStructureLayers*                          TheDebayerStructureLayersParameter = nullptr;
DebayerSaturationThreshold*                      TheDebayerSaturationThresholdParameter = nullptr;
DebayerSaturationRelative*                       TheDebayerSaturationRelativeParameter = nullptr;
DebayerNoiseLayers*                              TheDebayerNoiseLayersParameter = nullptr;
DebayerHotPixelFilterRadius*                     TheDebayerHotPixelFilterRadiusParameter = nullptr;
DebayerNoiseReductionFilterRadius*               TheDebayerNoiseReductionFilterRadiusParameter = nullptr;
DebayerMinStructureSize*                         TheDebayerMinStructureSizeParameter = nullptr;
DebayerPSFType*                                  TheDebayerPSFTypeParameter = nullptr;
DebayerPSFGrowth*                                TheDebayerPSFGrowthParameter = nullptr;
DebayerMaxStars*                                 TheDebayerMaxStarsParameter = nullptr;

DebayerInputHints*                               TheDebayerInputHintsParameter = nullptr;
DebayerOutputHints*                              TheDebayerOutputHintsParameter = nullptr;
DebayerOutputRGBImages*                          TheDebayerOutputRGBImagesParameter = nullptr;
DebayerOutputSeparateChannels*                   TheDebayerOutputSeparateChannelsParameter = nullptr;
DebayerOutputDirectory*                          TheDebayerOutputDirectoryParameter = nullptr;
DebayerOutputExtension*                          TheDebayerOutputExtensionParameter = nullptr;
DebayerOutputPrefix*                             TheDebayerOutputPrefixParameter = nullptr;
DebayerOutputPostfix*                            TheDebayerOutputPostfixParameter = nullptr;
DebayerOverwriteExistingFiles*                   TheDebayerOverwriteExistingFilesParameter = nullptr;
DebayerOnError*                                  TheDebayerOnErrorParameter = nullptr;
DebayerUseFileThreads*                           TheDebayerUseFileThreadsParameter = nullptr;
DebayerFileThreadOverload*                       TheDebayerFileThreadOverloadParameter = nullptr;
DebayerMaxFileReadThreads*                       TheDebayerMaxFileReadThreadsParameter = nullptr;
DebayerMaxFileWriteThreads*                      TheDebayerMaxFileWriteThreadsParameter = nullptr;
DebayerMemoryLoadControl*                        TheDebayerMemoryLoadControlParameter = nullptr;
DebayerMemoryLoadLimit*                          TheDebayerMemoryLoadLimitParameter = nullptr;

DebayerOutputImage*                              TheDebayerOutputImageParameter = nullptr;
DebayerOutputChannelImageR*                      TheDebayerOutputChannelImageRParameter = nullptr;
DebayerOutputChannelImageG*                      TheDebayerOutputChannelImageGParameter = nullptr;
DebayerOutputChannelImageB*                      TheDebayerOutputChannelImageBParameter = nullptr;
DebayerPSFTotalFluxEstimateR*                    TheDebayerPSFTotalFluxEstimateRParameter = nullptr;
DebayerPSFTotalFluxEstimateG*                    TheDebayerPSFTotalFluxEstimateGParameter = nullptr;
DebayerPSFTotalFluxEstimateB*                    TheDebayerPSFTotalFluxEstimateBParameter = nullptr;
DebayerPSFTotalPowerFluxEstimateR*               TheDebayerPSFTotalPowerFluxEstimateRParameter = nullptr;
DebayerPSFTotalPowerFluxEstimateG*               TheDebayerPSFTotalPowerFluxEstimateGParameter = nullptr;
DebayerPSFTotalPowerFluxEstimateB*               TheDebayerPSFTotalPowerFluxEstimateBParameter = nullptr;
DebayerPSFTotalMeanFluxEstimateR*                TheDebayerPSFTotalMeanFluxEstimateRParameter = nullptr;
DebayerPSFTotalMeanFluxEstimateG*                TheDebayerPSFTotalMeanFluxEstimateGParameter = nullptr;
DebayerPSFTotalMeanFluxEstimateB*                TheDebayerPSFTotalMeanFluxEstimateBParameter = nullptr;
DebayerPSFTotalMeanPowerFluxEstimateR*           TheDebayerPSFTotalMeanPowerFluxEstimateRParameter = nullptr;
DebayerPSFTotalMeanPowerFluxEstimateG*           TheDebayerPSFTotalMeanPowerFluxEstimateGParameter = nullptr;
DebayerPSFTotalMeanPowerFluxEstimateB*           TheDebayerPSFTotalMeanPowerFluxEstimateBParameter = nullptr;
DebayerPSFMStarEstimateR*                        TheDebayerPSFMStarEstimateRParameter = nullptr;
DebayerPSFMStarEstimateG*                        TheDebayerPSFMStarEstimateGParameter = nullptr;
DebayerPSFMStarEstimateB*                        TheDebayerPSFMStarEstimateBParameter = nullptr;
DebayerPSFNStarEstimateR*                        TheDebayerPSFNStarEstimateRParameter = nullptr;
DebayerPSFNStarEstimateG*                        TheDebayerPSFNStarEstimateGParameter = nullptr;
DebayerPSFNStarEstimateB*                        TheDebayerPSFNStarEstimateBParameter = nullptr;
DebayerPSFCountR*                                TheDebayerPSFCountRParameter = nullptr;
DebayerPSFCountG*                                TheDebayerPSFCountGParameter = nullptr;
DebayerPSFCountB*                                TheDebayerPSFCountBParameter = nullptr;
DebayerNoiseEstimateR*                           TheDebayerNoiseEstimateRParameter = nullptr;
DebayerNoiseEstimateG*                           TheDebayerNoiseEstimateGParameter = nullptr;
DebayerNoiseEstimateB*                           TheDebayerNoiseEstimateBParameter = nullptr;
DebayerNoiseFractionR*                           TheDebayerNoiseFractionRParameter = nullptr;
DebayerNoiseFractionG*                           TheDebayerNoiseFractionGParameter = nullptr;
DebayerNoiseFractionB*                           TheDebayerNoiseFractionBParameter = nullptr;
DebayerNoiseScaleLowR*                           TheDebayerNoiseScaleLowRParameter = nullptr;
DebayerNoiseScaleLowG*                           TheDebayerNoiseScaleLowGParameter = nullptr;
DebayerNoiseScaleLowB*                           TheDebayerNoiseScaleLowBParameter = nullptr;
DebayerNoiseScaleHighR*                          TheDebayerNoiseScaleHighRParameter = nullptr;
DebayerNoiseScaleHighG*                          TheDebayerNoiseScaleHighGParameter = nullptr;
DebayerNoiseScaleHighB*                          TheDebayerNoiseScaleHighBParameter = nullptr;
DebayerNoiseAlgorithmR*                          TheDebayerNoiseAlgorithmRParameter = nullptr;
DebayerNoiseAlgorithmG*                          TheDebayerNoiseAlgorithmGParameter = nullptr;
DebayerNoiseAlgorithmB*                          TheDebayerNoiseAlgorithmBParameter = nullptr;

DebayerOutputFileData*                           TheDebayerOutputFileDataParameter = nullptr;
DebayerOutputFilePath*                           TheDebayerOutputFilePathParameter = nullptr;
DebayerOutputChannelFilePathR*                   TheDebayerOutputChannelFilePathRParameter = nullptr;
DebayerOutputChannelFilePathG*                   TheDebayerOutputChannelFilePathGParameter = nullptr;
DebayerOutputChannelFilePathB*                   TheDebayerOutputChannelFilePathBParameter = nullptr;
DebayerOutputFilePSFTotalFluxEstimateR*          TheDebayerOutputFilePSFTotalFluxEstimateRParameter = nullptr;
DebayerOutputFilePSFTotalFluxEstimateG*          TheDebayerOutputFilePSFTotalFluxEstimateGParameter = nullptr;
DebayerOutputFilePSFTotalFluxEstimateB*          TheDebayerOutputFilePSFTotalFluxEstimateBParameter = nullptr;
DebayerOutputFilePSFTotalPowerFluxEstimateR*     TheDebayerOutputFilePSFTotalPowerFluxEstimateRParameter = nullptr;
DebayerOutputFilePSFTotalPowerFluxEstimateG*     TheDebayerOutputFilePSFTotalPowerFluxEstimateGParameter = nullptr;
DebayerOutputFilePSFTotalPowerFluxEstimateB*     TheDebayerOutputFilePSFTotalPowerFluxEstimateBParameter = nullptr;
DebayerOutputFilePSFTotalMeanFluxEstimateR*      TheDebayerOutputFilePSFTotalMeanFluxEstimateRParameter = nullptr;
DebayerOutputFilePSFTotalMeanFluxEstimateG*      TheDebayerOutputFilePSFTotalMeanFluxEstimateGParameter = nullptr;
DebayerOutputFilePSFTotalMeanFluxEstimateB*      TheDebayerOutputFilePSFTotalMeanFluxEstimateBParameter = nullptr;
DebayerOutputFilePSFTotalMeanPowerFluxEstimateR* TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateRParameter = nullptr;
DebayerOutputFilePSFTotalMeanPowerFluxEstimateG* TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateGParameter = nullptr;
DebayerOutputFilePSFTotalMeanPowerFluxEstimateB* TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateBParameter = nullptr;
DebayerOutputFilePSFMStarEstimateR*              TheDebayerOutputFilePSFMStarEstimateRParameter = nullptr;
DebayerOutputFilePSFMStarEstimateG*              TheDebayerOutputFilePSFMStarEstimateGParameter = nullptr;
DebayerOutputFilePSFMStarEstimateB*              TheDebayerOutputFilePSFMStarEstimateBParameter = nullptr;
DebayerOutputFilePSFNStarEstimateR*              TheDebayerOutputFilePSFNStarEstimateRParameter = nullptr;
DebayerOutputFilePSFNStarEstimateG*              TheDebayerOutputFilePSFNStarEstimateGParameter = nullptr;
DebayerOutputFilePSFNStarEstimateB*              TheDebayerOutputFilePSFNStarEstimateBParameter = nullptr;
DebayerOutputFilePSFCountR*                      TheDebayerOutputFilePSFCountRParameter = nullptr;
DebayerOutputFilePSFCountG*                      TheDebayerOutputFilePSFCountGParameter = nullptr;
DebayerOutputFilePSFCountB*                      TheDebayerOutputFilePSFCountBParameter = nullptr;
DebayerOutputFileNoiseEstimateR*                 TheDebayerOutputFileNoiseEstimateRParameter = nullptr;
DebayerOutputFileNoiseEstimateG*                 TheDebayerOutputFileNoiseEstimateGParameter = nullptr;
DebayerOutputFileNoiseEstimateB*                 TheDebayerOutputFileNoiseEstimateBParameter = nullptr;
DebayerOutputFileNoiseFractionR*                 TheDebayerOutputFileNoiseFractionRParameter = nullptr;
DebayerOutputFileNoiseFractionG*                 TheDebayerOutputFileNoiseFractionGParameter = nullptr;
DebayerOutputFileNoiseFractionB*                 TheDebayerOutputFileNoiseFractionBParameter = nullptr;
DebayerOutputFileNoiseScaleLowR*                 TheDebayerOutputFileNoiseScaleLowRParameter = nullptr;
DebayerOutputFileNoiseScaleLowG*                 TheDebayerOutputFileNoiseScaleLowGParameter = nullptr;
DebayerOutputFileNoiseScaleLowB*                 TheDebayerOutputFileNoiseScaleLowBParameter = nullptr;
DebayerOutputFileNoiseScaleHighR*                TheDebayerOutputFileNoiseScaleHighRParameter = nullptr;
DebayerOutputFileNoiseScaleHighG*                TheDebayerOutputFileNoiseScaleHighGParameter = nullptr;
DebayerOutputFileNoiseScaleHighB*                TheDebayerOutputFileNoiseScaleHighBParameter = nullptr;
DebayerOutputFileNoiseAlgorithmR*                TheDebayerOutputFileNoiseAlgorithmRParameter = nullptr;
DebayerOutputFileNoiseAlgorithmG*                TheDebayerOutputFileNoiseAlgorithmGParameter = nullptr;
DebayerOutputFileNoiseAlgorithmB*                TheDebayerOutputFileNoiseAlgorithmBParameter = nullptr;

// ----------------------------------------------------------------------------

DebayerBayerPatternParameter::DebayerBayerPatternParameter( MetaProcess* P ) : MetaEnumeration( P )
{
   TheDebayerBayerPatternParameter = this;
}

IsoString DebayerBayerPatternParameter::Id() const
{
   return "cfaPattern";
}

IsoString DebayerBayerPatternParameter::Aliases() const
{
   return "BayerPattern,bayerPattern";
}

size_type DebayerBayerPatternParameter::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DebayerBayerPatternParameter::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Auto: return "Auto";
   case RGGB: return "RGGB";
   case BGGR: return "BGGR";
   case GBRG: return "GBRG";
   case GRBG: return "GRBG";
   case GRGB: return "GRGB";
   case GBGR: return "GBGR";
   case RGBG: return "RGBG";
   case BGRG: return "BGRG";
   }
}

int DebayerBayerPatternParameter::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerBayerPatternParameter::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

DebayerMethodParameter::DebayerMethodParameter( MetaProcess* P ) : MetaEnumeration( P )
{
   TheDebayerMethodParameter = this;
}

IsoString DebayerMethodParameter::Id() const
{
   return "debayerMethod";
}

IsoString DebayerMethodParameter::Aliases() const
{
   return "DebayerMethod";
}

size_type DebayerMethodParameter::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DebayerMethodParameter::ElementId( size_type i ) const
{
   switch ( i )
   {
   case SuperPixel: return "SuperPixel";
   case Bilinear:   return "Bilinear";
   default:
   case VNG:        return "VNG";
   // case AHD:        return "AHD"; ### Uncomment when implemented
   }
}

int DebayerMethodParameter::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerMethodParameter::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

DebayerFBDDNoiseReduction::DebayerFBDDNoiseReduction( MetaProcess* P ) : MetaInt32( P )
{
   TheDebayerFBDDNoiseReductionParameter = this;
}

IsoString DebayerFBDDNoiseReduction::Id() const
{
   return "fbddNoiseReduction";
}

double DebayerFBDDNoiseReduction::DefaultValue() const
{
   return 0;
}

double DebayerFBDDNoiseReduction::MinimumValue() const
{
   return 0;
}

double DebayerFBDDNoiseReduction::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

DebayerShowImages::DebayerShowImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerShowImagesParameter = this;
}

IsoString DebayerShowImages::Id() const
{
   return "showImages";
}

bool DebayerShowImages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerCFASourceFilePath::DebayerCFASourceFilePath( MetaProcess* P ) : MetaString( P )
{
   TheDebayerCFASourceFilePathParameter = this;
}

IsoString DebayerCFASourceFilePath::Id() const
{
   return "cfaSourceFilePath";
}

// ----------------------------------------------------------------------------

DebayerTargetItems::DebayerTargetItems( MetaProcess* P ) : MetaTable( P )
{
   TheDebayerTargetItemsParameter = this;
}

IsoString DebayerTargetItems::Id() const
{
   return "targetItems";
}

// ----------------------------------------------------------------------------

DebayerTargetEnabled::DebayerTargetEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheDebayerTargetEnabledParameter = this;
}

IsoString DebayerTargetEnabled::Id() const
{
   return "enabled";
}

bool DebayerTargetEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerTargetImage::DebayerTargetImage( MetaTable* T ) : MetaString( T )
{
   TheDebayerTargetImageParameter = this;
}

IsoString DebayerTargetImage::Id() const
{
   return "image";
}

// ----------------------------------------------------------------------------

// ### DEPRECATED
DebayerNoGUIMessages::DebayerNoGUIMessages( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerNoGUIMessagesParameter = this;
}

IsoString DebayerNoGUIMessages::Id() const
{
   return "noGUIMessages";
}

bool DebayerNoGUIMessages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerEvaluateNoise::DebayerEvaluateNoise( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerEvaluateNoiseParameter = this;
}

IsoString DebayerEvaluateNoise::Id() const
{
   return "evaluateNoise";
}

bool DebayerEvaluateNoise::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEvaluationAlgorithm::DebayerNoiseEvaluationAlgorithm( MetaProcess* P ) : MetaEnumeration( P )
{
   TheDebayerNoiseEvaluationAlgorithmParameter = this;
}

IsoString DebayerNoiseEvaluationAlgorithm::Id() const
{
   return "noiseEvaluationAlgorithm";
}

size_type DebayerNoiseEvaluationAlgorithm::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DebayerNoiseEvaluationAlgorithm::ElementId( size_type i ) const
{
   switch ( i )
   {
   case KSigma: return "NoiseEvaluation_KSigma";
   default:
   case MRS:    return "NoiseEvaluation_MRS";
   case NStar:  return "NoiseEvaluation_NStar";
   }
}

int DebayerNoiseEvaluationAlgorithm::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerNoiseEvaluationAlgorithm::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DebayerEvaluateSignal::DebayerEvaluateSignal( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerEvaluateSignalParameter = this;
}

IsoString DebayerEvaluateSignal::Id() const
{
   return "evaluateSignal";
}

bool DebayerEvaluateSignal::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerStructureLayers::DebayerStructureLayers( MetaProcess* p ) : MetaInt32( p )
{
   TheDebayerStructureLayersParameter = this;
}

IsoString DebayerStructureLayers::Id() const
{
   return "structureLayers";
}

double DebayerStructureLayers::DefaultValue() const
{
   return 5;
}

double DebayerStructureLayers::MinimumValue() const
{
   return 1;
}

double DebayerStructureLayers::MaximumValue() const
{
   return 8;
}

// ----------------------------------------------------------------------------

DebayerSaturationThreshold::DebayerSaturationThreshold( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerSaturationThresholdParameter = this;
}

IsoString DebayerSaturationThreshold::Id() const
{
   return "saturationThreshold";
}

int DebayerSaturationThreshold::Precision() const
{
   return 2;
}

double DebayerSaturationThreshold::DefaultValue() const
{
   return 1.00;
}

double DebayerSaturationThreshold::MinimumValue() const
{
   return 0.10;
}

double DebayerSaturationThreshold::MaximumValue() const
{
   return 1.00;
}

// ----------------------------------------------------------------------------

DebayerSaturationRelative::DebayerSaturationRelative( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerSaturationRelativeParameter = this;
}

IsoString DebayerSaturationRelative::Id() const
{
   return "saturationRelative";
}

bool DebayerSaturationRelative::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DebayerNoiseLayers::DebayerNoiseLayers( MetaProcess* p ) : MetaInt32( p )
{
   TheDebayerNoiseLayersParameter = this;
}

IsoString DebayerNoiseLayers::Id() const
{
   return "noiseLayers";
}

double DebayerNoiseLayers::DefaultValue() const
{
   return 1;
}

double DebayerNoiseLayers::MinimumValue() const
{
   return 0;
}

double DebayerNoiseLayers::MaximumValue() const
{
   return 4;
}

// ----------------------------------------------------------------------------

DebayerHotPixelFilterRadius::DebayerHotPixelFilterRadius( MetaProcess* p ) : MetaInt32( p )
{
   TheDebayerHotPixelFilterRadiusParameter = this;
}

IsoString DebayerHotPixelFilterRadius::Id() const
{
   return "hotPixelFilterRadius";
}

double DebayerHotPixelFilterRadius::DefaultValue() const
{
   return 1;
}

double DebayerHotPixelFilterRadius::MinimumValue() const
{
   return 0;
}

double DebayerHotPixelFilterRadius::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

DebayerNoiseReductionFilterRadius::DebayerNoiseReductionFilterRadius( MetaProcess* p ) : MetaInt32( p )
{
   TheDebayerNoiseReductionFilterRadiusParameter = this;
}

IsoString DebayerNoiseReductionFilterRadius::Id() const
{
   return "noiseReductionFilterRadius";
}

double DebayerNoiseReductionFilterRadius::DefaultValue() const
{
   return 0;
}

double DebayerNoiseReductionFilterRadius::MinimumValue() const
{
   return 0;
}

double DebayerNoiseReductionFilterRadius::MaximumValue() const
{
   return 50;
}

// ----------------------------------------------------------------------------

DebayerMinStructureSize::DebayerMinStructureSize( MetaProcess* p ) : MetaInt32( p )
{
   TheDebayerMinStructureSizeParameter = this;
}

IsoString DebayerMinStructureSize::Id() const
{
   return "minStructureSize";
}

double DebayerMinStructureSize::DefaultValue() const
{
   return 0;
}

double DebayerMinStructureSize::MinimumValue() const
{
   return 0;
}

double DebayerMinStructureSize::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

DebayerPSFType::DebayerPSFType( MetaProcess* P ) : MetaEnumeration( P )
{
   TheDebayerPSFTypeParameter = this;
}

IsoString DebayerPSFType::Id() const
{
   return "psfType";
}

size_type DebayerPSFType::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DebayerPSFType::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Gaussian: return "PSFType_Gaussian";
   case Moffat15: return "PSFType_Moffat15";
   default:
   case Moffat4:  return "PSFType_Moffat4";
   case Moffat6:  return "PSFType_Moffat6";
   case Moffat8:  return "PSFType_Moffat8";
   case MoffatA:  return "PSFType_MoffatA";
   case Auto:     return "PSFType_Auto";
   }
}

IsoString DebayerPSFType::FunctionName( pcl_enum i )
{
   switch ( i )
   {
   case Gaussian: return "Gaussian";
   case Moffat15: return "Moffat15";
   default:
   case Moffat4:  return "Moffat4";
   case Moffat6:  return "Moffat6";
   case Moffat8:  return "Moffat8";
   case MoffatA:  return "MoffatA";
   case Auto:     return "Auto";
   }
}

int DebayerPSFType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerPSFType::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DebayerPSFGrowth::DebayerPSFGrowth( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerPSFGrowthParameter = this;
}

IsoString DebayerPSFGrowth::Id() const
{
   return "psfGrowth";
}

int DebayerPSFGrowth::Precision() const
{
   return 2;
}

double DebayerPSFGrowth::DefaultValue() const
{
   return 1.0;
}

double DebayerPSFGrowth::MinimumValue() const
{
   return 0.5;
}

double DebayerPSFGrowth::MaximumValue() const
{
   return 4.0;
}

// ----------------------------------------------------------------------------

DebayerMaxStars::DebayerMaxStars( MetaProcess* p ) : MetaInt32( p )
{
   TheDebayerMaxStarsParameter = this;
}

IsoString DebayerMaxStars::Id() const
{
   return "maxStars";
}

double DebayerMaxStars::DefaultValue() const
{
   return 24*1024;
}

double DebayerMaxStars::MinimumValue() const
{
   return 256;
}

double DebayerMaxStars::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

DebayerInputHints::DebayerInputHints( MetaProcess* P ) : MetaString( P )
{
   TheDebayerInputHintsParameter = this;
}

IsoString DebayerInputHints::Id() const
{
   return "inputHints";
}

String DebayerInputHints::DefaultValue() const
{
   return "raw cfa";
}

// ----------------------------------------------------------------------------

DebayerOutputHints::DebayerOutputHints( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputHintsParameter = this;
}

IsoString DebayerOutputHints::Id() const
{
   return "outputHints";
}

// ----------------------------------------------------------------------------

DebayerOutputRGBImages::DebayerOutputRGBImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerOutputRGBImagesParameter = this;
}

IsoString DebayerOutputRGBImages::Id() const
{
   return "outputRGBImages";
}

bool DebayerOutputRGBImages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputSeparateChannels::DebayerOutputSeparateChannels( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerOutputSeparateChannelsParameter = this;
}

IsoString DebayerOutputSeparateChannels::Id() const
{
   return "outputSeparateChannels";
}

bool DebayerOutputSeparateChannels::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DebayerOutputDirectory::DebayerOutputDirectory( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputDirectoryParameter = this;
}

IsoString DebayerOutputDirectory::Id() const
{
   return "outputDirectory";
}

// ----------------------------------------------------------------------------

DebayerOutputExtension::DebayerOutputExtension( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputExtensionParameter = this;
}

IsoString DebayerOutputExtension::Id() const
{
   return "outputExtension";
}

String DebayerOutputExtension::DefaultValue() const
{
   return ".xisf";
}

// ----------------------------------------------------------------------------

DebayerOutputPrefix::DebayerOutputPrefix( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputPrefixParameter = this;
}

IsoString DebayerOutputPrefix::Id() const
{
   return "outputPrefix";
}

String DebayerOutputPrefix::DefaultValue() const
{
   return String();
}

// ----------------------------------------------------------------------------

DebayerOutputPostfix::DebayerOutputPostfix( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputPostfixParameter = this;
}

IsoString DebayerOutputPostfix::Id() const
{
   return "outputPostfix";
}

String DebayerOutputPostfix::DefaultValue() const
{
   return "_d";
}

// ----------------------------------------------------------------------------

DebayerOverwriteExistingFiles::DebayerOverwriteExistingFiles( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerOverwriteExistingFilesParameter = this;
}

IsoString DebayerOverwriteExistingFiles::Id() const
{
   return "overwriteExistingFiles";
}

bool DebayerOverwriteExistingFiles::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DebayerOnError::DebayerOnError( MetaProcess* p ) : MetaEnumeration( p )
{
   TheDebayerOnErrorParameter = this;
}

IsoString DebayerOnError::Id() const
{
   return "onError";
}

size_type DebayerOnError::NumberOfElements() const
{
   return NumberOfErrorPolicies;
}

IsoString DebayerOnError::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Continue: return "OnError_Continue";
   case Abort:    return "OnError_Abort";
   case AskUser:  return "OnError_AskUser";
   }
}

int DebayerOnError::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerOnError::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DebayerUseFileThreads::DebayerUseFileThreads( MetaProcess* p ) : MetaBoolean( p )
{
   TheDebayerUseFileThreadsParameter = this;
}

IsoString DebayerUseFileThreads::Id() const
{
   return "useFileThreads";
}

bool DebayerUseFileThreads::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerFileThreadOverload::DebayerFileThreadOverload( MetaProcess* p ) : MetaFloat( p )
{
   TheDebayerFileThreadOverloadParameter = this;
}

IsoString DebayerFileThreadOverload::Id() const
{
   return "fileThreadOverload";
}

int DebayerFileThreadOverload::Precision() const
{
   return 2;
}

double DebayerFileThreadOverload::DefaultValue() const
{
   return 1.0;
}

double DebayerFileThreadOverload::MinimumValue() const
{
   return 1;
}

double DebayerFileThreadOverload::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

DebayerMaxFileReadThreads::DebayerMaxFileReadThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheDebayerMaxFileReadThreadsParameter = this;
}

IsoString DebayerMaxFileReadThreads::Id() const
{
   return "maxFileReadThreads";
}

double DebayerMaxFileReadThreads::DefaultValue() const
{
   return 0;
}

double DebayerMaxFileReadThreads::MinimumValue() const
{
   return 0;
}

double DebayerMaxFileReadThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

DebayerMaxFileWriteThreads::DebayerMaxFileWriteThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheDebayerMaxFileWriteThreadsParameter = this;
}

IsoString DebayerMaxFileWriteThreads::Id() const
{
   return "maxFileWriteThreads";
}

double DebayerMaxFileWriteThreads::DefaultValue() const
{
   return 0;
}

double DebayerMaxFileWriteThreads::MinimumValue() const
{
   return 0;
}

double DebayerMaxFileWriteThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

DebayerMemoryLoadControl::DebayerMemoryLoadControl( MetaProcess* p ) : MetaBoolean( p )
{
   TheDebayerMemoryLoadControlParameter = this;
}

IsoString DebayerMemoryLoadControl::Id() const
{
   return "memoryLoadControl";
}

bool DebayerMemoryLoadControl::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerMemoryLoadLimit::DebayerMemoryLoadLimit( MetaProcess* p ) : MetaFloat( p )
{
   TheDebayerMemoryLoadLimitParameter = this;
}

IsoString DebayerMemoryLoadLimit::Id() const
{
   return "memoryLoadLimit";
}

int DebayerMemoryLoadLimit::Precision() const
{
   return 2;
}

double DebayerMemoryLoadLimit::DefaultValue() const
{
   return 0.85;
}

double DebayerMemoryLoadLimit::MinimumValue() const
{
   return 0.10;
}

double DebayerMemoryLoadLimit::MaximumValue() const
{
   return 1.00;
}

// ----------------------------------------------------------------------------

DebayerOutputImage::DebayerOutputImage( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputImageParameter = this;
}

IsoString DebayerOutputImage::Id() const
{
   return "outputImage";
}

bool DebayerOutputImage::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputChannelImageR::DebayerOutputChannelImageR( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputChannelImageRParameter = this;
}

IsoString DebayerOutputChannelImageR::Id() const
{
   return "outputChannelImageR";
}

bool DebayerOutputChannelImageR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputChannelImageG::DebayerOutputChannelImageG( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputChannelImageGParameter = this;
}

IsoString DebayerOutputChannelImageG::Id() const
{
   return "outputChannelImageG";
}

bool DebayerOutputChannelImageG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputChannelImageB::DebayerOutputChannelImageB( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputChannelImageBParameter = this;
}

IsoString DebayerOutputChannelImageB::Id() const
{
   return "outputChannelImageB";
}

bool DebayerOutputChannelImageB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalFluxEstimateR::DebayerPSFTotalFluxEstimateR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalFluxEstimateRParameter = this;
}

IsoString DebayerPSFTotalFluxEstimateR::Id() const
{
   return "psfTotalFluxEstimateR";
}

int DebayerPSFTotalFluxEstimateR::Precision() const
{
   return 4;
}

bool DebayerPSFTotalFluxEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalFluxEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalFluxEstimateG::DebayerPSFTotalFluxEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalFluxEstimateGParameter = this;
}

IsoString DebayerPSFTotalFluxEstimateG::Id() const
{
   return "psfTotalFluxEstimateG";
}

int DebayerPSFTotalFluxEstimateG::Precision() const
{
   return 4;
}

bool DebayerPSFTotalFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalFluxEstimateB::DebayerPSFTotalFluxEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalFluxEstimateBParameter = this;
}

IsoString DebayerPSFTotalFluxEstimateB::Id() const
{
   return "psfTotalFluxEstimateB";
}

int DebayerPSFTotalFluxEstimateB::Precision() const
{
   return 4;
}

bool DebayerPSFTotalFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalPowerFluxEstimateR::DebayerPSFTotalPowerFluxEstimateR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalPowerFluxEstimateRParameter = this;
}

IsoString DebayerPSFTotalPowerFluxEstimateR::Id() const
{
   return "psfTotalPowerFluxEstimateR";
}

int DebayerPSFTotalPowerFluxEstimateR::Precision() const
{
   return 4;
}

bool DebayerPSFTotalPowerFluxEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalPowerFluxEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalPowerFluxEstimateG::DebayerPSFTotalPowerFluxEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalPowerFluxEstimateGParameter = this;
}

IsoString DebayerPSFTotalPowerFluxEstimateG::Id() const
{
   return "psfTotalPowerFluxEstimateG";
}

int DebayerPSFTotalPowerFluxEstimateG::Precision() const
{
   return 4;
}

bool DebayerPSFTotalPowerFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalPowerFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalPowerFluxEstimateB::DebayerPSFTotalPowerFluxEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalPowerFluxEstimateBParameter = this;
}

IsoString DebayerPSFTotalPowerFluxEstimateB::Id() const
{
   return "psfTotalPowerFluxEstimateB";
}

int DebayerPSFTotalPowerFluxEstimateB::Precision() const
{
   return 4;
}

bool DebayerPSFTotalPowerFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalPowerFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalMeanFluxEstimateR::DebayerPSFTotalMeanFluxEstimateR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalMeanFluxEstimateRParameter = this;
}

IsoString DebayerPSFTotalMeanFluxEstimateR::Id() const
{
   return "psfTotalMeanFluxEstimateR";
}

int DebayerPSFTotalMeanFluxEstimateR::Precision() const
{
   return 4;
}

bool DebayerPSFTotalMeanFluxEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalMeanFluxEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalMeanFluxEstimateG::DebayerPSFTotalMeanFluxEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalMeanFluxEstimateGParameter = this;
}

IsoString DebayerPSFTotalMeanFluxEstimateG::Id() const
{
   return "psfTotalMeanFluxEstimateG";
}

int DebayerPSFTotalMeanFluxEstimateG::Precision() const
{
   return 4;
}

bool DebayerPSFTotalMeanFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalMeanFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalMeanFluxEstimateB::DebayerPSFTotalMeanFluxEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalMeanFluxEstimateBParameter = this;
}

IsoString DebayerPSFTotalMeanFluxEstimateB::Id() const
{
   return "psfTotalMeanFluxEstimateB";
}

int DebayerPSFTotalMeanFluxEstimateB::Precision() const
{
   return 4;
}

bool DebayerPSFTotalMeanFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalMeanFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalMeanPowerFluxEstimateR::DebayerPSFTotalMeanPowerFluxEstimateR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalMeanPowerFluxEstimateRParameter = this;
}

IsoString DebayerPSFTotalMeanPowerFluxEstimateR::Id() const
{
   return "psfTotalMeanPowerFluxEstimateR";
}

int DebayerPSFTotalMeanPowerFluxEstimateR::Precision() const
{
   return 4;
}

bool DebayerPSFTotalMeanPowerFluxEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalMeanPowerFluxEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalMeanPowerFluxEstimateG::DebayerPSFTotalMeanPowerFluxEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalMeanPowerFluxEstimateGParameter = this;
}

IsoString DebayerPSFTotalMeanPowerFluxEstimateG::Id() const
{
   return "psfTotalMeanPowerFluxEstimateG";
}

int DebayerPSFTotalMeanPowerFluxEstimateG::Precision() const
{
   return 4;
}

bool DebayerPSFTotalMeanPowerFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalMeanPowerFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFTotalMeanPowerFluxEstimateB::DebayerPSFTotalMeanPowerFluxEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFTotalMeanPowerFluxEstimateBParameter = this;
}

IsoString DebayerPSFTotalMeanPowerFluxEstimateB::Id() const
{
   return "psfTotalMeanPowerFluxEstimateB";
}

int DebayerPSFTotalMeanPowerFluxEstimateB::Precision() const
{
   return 4;
}

bool DebayerPSFTotalMeanPowerFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerPSFTotalMeanPowerFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFMStarEstimateR::DebayerPSFMStarEstimateR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFMStarEstimateRParameter = this;
}

IsoString DebayerPSFMStarEstimateR::Id() const
{
   return "psfMStarEstimateR";
}

int DebayerPSFMStarEstimateR::Precision() const
{
   return 4;
}

bool DebayerPSFMStarEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerPSFMStarEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFMStarEstimateG::DebayerPSFMStarEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFMStarEstimateGParameter = this;
}

IsoString DebayerPSFMStarEstimateG::Id() const
{
   return "psfMStarEstimateG";
}

int DebayerPSFMStarEstimateG::Precision() const
{
   return 4;
}

bool DebayerPSFMStarEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerPSFMStarEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFMStarEstimateB::DebayerPSFMStarEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFMStarEstimateBParameter = this;
}

IsoString DebayerPSFMStarEstimateB::Id() const
{
   return "psfMStarEstimateB";
}

int DebayerPSFMStarEstimateB::Precision() const
{
   return 4;
}

bool DebayerPSFMStarEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerPSFMStarEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFNStarEstimateR::DebayerPSFNStarEstimateR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFNStarEstimateRParameter = this;
}

IsoString DebayerPSFNStarEstimateR::Id() const
{
   return "psfNStarEstimateR";
}

int DebayerPSFNStarEstimateR::Precision() const
{
   return 4;
}

bool DebayerPSFNStarEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerPSFNStarEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFNStarEstimateG::DebayerPSFNStarEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFNStarEstimateGParameter = this;
}

IsoString DebayerPSFNStarEstimateG::Id() const
{
   return "psfNStarEstimateG";
}

int DebayerPSFNStarEstimateG::Precision() const
{
   return 4;
}

bool DebayerPSFNStarEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerPSFNStarEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFNStarEstimateB::DebayerPSFNStarEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFNStarEstimateBParameter = this;
}

IsoString DebayerPSFNStarEstimateB::Id() const
{
   return "psfNStarEstimateB";
}

int DebayerPSFNStarEstimateB::Precision() const
{
   return 4;
}

bool DebayerPSFNStarEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerPSFNStarEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFCountR::DebayerPSFCountR( MetaProcess* P ) : MetaInt32( P )
{
   TheDebayerPSFCountRParameter = this;
}

IsoString DebayerPSFCountR::Id() const
{
   return "psfCountR";
}

bool DebayerPSFCountR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFCountG::DebayerPSFCountG( MetaProcess* P ) : MetaInt32( P )
{
   TheDebayerPSFCountGParameter = this;
}

IsoString DebayerPSFCountG::Id() const
{
   return "psfCountG";
}

bool DebayerPSFCountG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFCountB::DebayerPSFCountB( MetaProcess* P ) : MetaInt32( P )
{
   TheDebayerPSFCountBParameter = this;
}

IsoString DebayerPSFCountB::Id() const
{
   return "psfCountB";
}

bool DebayerPSFCountB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEstimateR::DebayerNoiseEstimateR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseEstimateRParameter = this;
}

IsoString DebayerNoiseEstimateR::Id() const
{
   return "noiseEstimateR";
}

int DebayerNoiseEstimateR::Precision() const
{
   return 3;
}

bool DebayerNoiseEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEstimateG::DebayerNoiseEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseEstimateGParameter = this;
}

IsoString DebayerNoiseEstimateG::Id() const
{
   return "noiseEstimateG";
}

int DebayerNoiseEstimateG::Precision() const
{
   return 3;
}

bool DebayerNoiseEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEstimateB::DebayerNoiseEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseEstimateBParameter = this;
}

IsoString DebayerNoiseEstimateB::Id() const
{
   return "noiseEstimateB";
}

int DebayerNoiseEstimateB::Precision() const
{
   return 3;
}

bool DebayerNoiseEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseFractionR::DebayerNoiseFractionR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseFractionRParameter = this;
}

IsoString DebayerNoiseFractionR::Id() const
{
   return "noiseFractionR";
}

int DebayerNoiseFractionR::Precision() const
{
   return 3;
}

bool DebayerNoiseFractionR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseFractionG::DebayerNoiseFractionG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseFractionGParameter = this;
}

IsoString DebayerNoiseFractionG::Id() const
{
   return "noiseFractionG";
}

int DebayerNoiseFractionG::Precision() const
{
   return 3;
}

bool DebayerNoiseFractionG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseFractionB::DebayerNoiseFractionB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseFractionBParameter = this;
}

IsoString DebayerNoiseFractionB::Id() const
{
   return "noiseFractionB";
}

int DebayerNoiseFractionB::Precision() const
{
   return 3;
}

bool DebayerNoiseFractionB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseScaleLowR::DebayerNoiseScaleLowR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseScaleLowRParameter = this;
}

IsoString DebayerNoiseScaleLowR::Id() const
{
   return "noiseScaleLowR";
}

int DebayerNoiseScaleLowR::Precision() const
{
   return 6;
}

bool DebayerNoiseScaleLowR::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseScaleLowR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseScaleLowG::DebayerNoiseScaleLowG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseScaleLowGParameter = this;
}

IsoString DebayerNoiseScaleLowG::Id() const
{
   return "noiseScaleLowG";
}

int DebayerNoiseScaleLowG::Precision() const
{
   return 6;
}

bool DebayerNoiseScaleLowG::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseScaleLowG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseScaleLowB::DebayerNoiseScaleLowB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseScaleLowBParameter = this;
}

IsoString DebayerNoiseScaleLowB::Id() const
{
   return "noiseScaleLowB";
}

int DebayerNoiseScaleLowB::Precision() const
{
   return 6;
}

bool DebayerNoiseScaleLowB::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseScaleLowB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseScaleHighR::DebayerNoiseScaleHighR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseScaleHighRParameter = this;
}

IsoString DebayerNoiseScaleHighR::Id() const
{
   return "noiseScaleHighR";
}

int DebayerNoiseScaleHighR::Precision() const
{
   return 6;
}

bool DebayerNoiseScaleHighR::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseScaleHighR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseScaleHighG::DebayerNoiseScaleHighG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseScaleHighGParameter = this;
}

IsoString DebayerNoiseScaleHighG::Id() const
{
   return "noiseScaleHighG";
}

int DebayerNoiseScaleHighG::Precision() const
{
   return 6;
}

bool DebayerNoiseScaleHighG::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseScaleHighG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseScaleHighB::DebayerNoiseScaleHighB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerNoiseScaleHighBParameter = this;
}

IsoString DebayerNoiseScaleHighB::Id() const
{
   return "noiseScaleHighB";
}

int DebayerNoiseScaleHighB::Precision() const
{
   return 6;
}

bool DebayerNoiseScaleHighB::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseScaleHighB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseAlgorithmR::DebayerNoiseAlgorithmR( MetaProcess* P ) : MetaString( P )
{
   TheDebayerNoiseAlgorithmRParameter = this;
}

IsoString DebayerNoiseAlgorithmR::Id() const
{
   return "noiseAlgorithmR";
}

bool DebayerNoiseAlgorithmR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseAlgorithmG::DebayerNoiseAlgorithmG( MetaProcess* P ) : MetaString( P )
{
   TheDebayerNoiseAlgorithmGParameter = this;
}

IsoString DebayerNoiseAlgorithmG::Id() const
{
   return "noiseAlgorithmG";
}

bool DebayerNoiseAlgorithmG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseAlgorithmB::DebayerNoiseAlgorithmB( MetaProcess* P ) : MetaString( P )
{
   TheDebayerNoiseAlgorithmBParameter = this;
}

IsoString DebayerNoiseAlgorithmB::Id() const
{
   return "noiseAlgorithmB";
}

bool DebayerNoiseAlgorithmB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileData::DebayerOutputFileData( MetaProcess* P ) : MetaTable( P )
{
   TheDebayerOutputFileDataParameter = this;
}

IsoString DebayerOutputFileData::Id() const
{
   return "outputFileData";
}

bool DebayerOutputFileData::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePath::DebayerOutputFilePath( MetaTable* T ) : MetaString( T )
{
   TheDebayerOutputFilePathParameter = this;
}

IsoString DebayerOutputFilePath::Id() const
{
   return "filePath";
}

bool DebayerOutputFilePath::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputChannelFilePathR::DebayerOutputChannelFilePathR( MetaTable* T ) : MetaString( T )
{
   TheDebayerOutputChannelFilePathRParameter = this;
}

IsoString DebayerOutputChannelFilePathR::Id() const
{
   return "channelFilePathR";
}

bool DebayerOutputChannelFilePathR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputChannelFilePathG::DebayerOutputChannelFilePathG( MetaTable* T ) : MetaString( T )
{
   TheDebayerOutputChannelFilePathGParameter = this;
}

IsoString DebayerOutputChannelFilePathG::Id() const
{
   return "channelFilePathG";
}

bool DebayerOutputChannelFilePathG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputChannelFilePathB::DebayerOutputChannelFilePathB( MetaTable* T ) : MetaString( T )
{
   TheDebayerOutputChannelFilePathBParameter = this;
}

IsoString DebayerOutputChannelFilePathB::Id() const
{
   return "channelFilePathB";
}

bool DebayerOutputChannelFilePathB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalFluxEstimateR::DebayerOutputFilePSFTotalFluxEstimateR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalFluxEstimateRParameter = this;
}

IsoString DebayerOutputFilePSFTotalFluxEstimateR::Id() const
{
   return "psfTotalFluxEstimateR";
}

int DebayerOutputFilePSFTotalFluxEstimateR::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalFluxEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalFluxEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalFluxEstimateG::DebayerOutputFilePSFTotalFluxEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalFluxEstimateGParameter = this;
}

IsoString DebayerOutputFilePSFTotalFluxEstimateG::Id() const
{
   return "psfTotalFluxEstimateG";
}

int DebayerOutputFilePSFTotalFluxEstimateG::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalFluxEstimateB::DebayerOutputFilePSFTotalFluxEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalFluxEstimateBParameter = this;
}

IsoString DebayerOutputFilePSFTotalFluxEstimateB::Id() const
{
   return "psfTotalFluxEstimateB";
}

int DebayerOutputFilePSFTotalFluxEstimateB::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalPowerFluxEstimateR::DebayerOutputFilePSFTotalPowerFluxEstimateR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalPowerFluxEstimateRParameter = this;
}

IsoString DebayerOutputFilePSFTotalPowerFluxEstimateR::Id() const
{
   return "psfTotalPowerFluxEstimateR";
}

int DebayerOutputFilePSFTotalPowerFluxEstimateR::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalPowerFluxEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalPowerFluxEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalPowerFluxEstimateG::DebayerOutputFilePSFTotalPowerFluxEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalPowerFluxEstimateGParameter = this;
}

IsoString DebayerOutputFilePSFTotalPowerFluxEstimateG::Id() const
{
   return "psfTotalPowerFluxEstimateG";
}

int DebayerOutputFilePSFTotalPowerFluxEstimateG::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalPowerFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalPowerFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalPowerFluxEstimateB::DebayerOutputFilePSFTotalPowerFluxEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalPowerFluxEstimateBParameter = this;
}

IsoString DebayerOutputFilePSFTotalPowerFluxEstimateB::Id() const
{
   return "psfTotalPowerFluxEstimateB";
}

int DebayerOutputFilePSFTotalPowerFluxEstimateB::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalPowerFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalPowerFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalMeanFluxEstimateR::DebayerOutputFilePSFTotalMeanFluxEstimateR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalMeanFluxEstimateRParameter = this;
}

IsoString DebayerOutputFilePSFTotalMeanFluxEstimateR::Id() const
{
   return "psfTotalMeanFluxEstimateR";
}

int DebayerOutputFilePSFTotalMeanFluxEstimateR::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalMeanFluxEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalMeanFluxEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalMeanFluxEstimateG::DebayerOutputFilePSFTotalMeanFluxEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalMeanFluxEstimateGParameter = this;
}

IsoString DebayerOutputFilePSFTotalMeanFluxEstimateG::Id() const
{
   return "psfTotalMeanFluxEstimateG";
}

int DebayerOutputFilePSFTotalMeanFluxEstimateG::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalMeanFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalMeanFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalMeanFluxEstimateB::DebayerOutputFilePSFTotalMeanFluxEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalMeanFluxEstimateBParameter = this;
}

IsoString DebayerOutputFilePSFTotalMeanFluxEstimateB::Id() const
{
   return "psfTotalMeanFluxEstimateB";
}

int DebayerOutputFilePSFTotalMeanFluxEstimateB::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalMeanFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalMeanFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalMeanPowerFluxEstimateR::DebayerOutputFilePSFTotalMeanPowerFluxEstimateR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateRParameter = this;
}

IsoString DebayerOutputFilePSFTotalMeanPowerFluxEstimateR::Id() const
{
   return "psfTotalMeanPowerFluxEstimateR";
}

int DebayerOutputFilePSFTotalMeanPowerFluxEstimateR::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalMeanPowerFluxEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalMeanPowerFluxEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalMeanPowerFluxEstimateG::DebayerOutputFilePSFTotalMeanPowerFluxEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateGParameter = this;
}

IsoString DebayerOutputFilePSFTotalMeanPowerFluxEstimateG::Id() const
{
   return "psfTotalMeanPowerFluxEstimateG";
}

int DebayerOutputFilePSFTotalMeanPowerFluxEstimateG::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalMeanPowerFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalMeanPowerFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFTotalMeanPowerFluxEstimateB::DebayerOutputFilePSFTotalMeanPowerFluxEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateBParameter = this;
}

IsoString DebayerOutputFilePSFTotalMeanPowerFluxEstimateB::Id() const
{
   return "psfTotalMeanPowerFluxEstimateB";
}

int DebayerOutputFilePSFTotalMeanPowerFluxEstimateB::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFTotalMeanPowerFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFTotalMeanPowerFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFMStarEstimateR::DebayerOutputFilePSFMStarEstimateR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFMStarEstimateRParameter = this;
}

IsoString DebayerOutputFilePSFMStarEstimateR::Id() const
{
   return "psfMStarEstimateR";
}

int DebayerOutputFilePSFMStarEstimateR::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFMStarEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFMStarEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFMStarEstimateG::DebayerOutputFilePSFMStarEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFMStarEstimateGParameter = this;
}

IsoString DebayerOutputFilePSFMStarEstimateG::Id() const
{
   return "psfMStarEstimateG";
}

int DebayerOutputFilePSFMStarEstimateG::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFMStarEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFMStarEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFMStarEstimateB::DebayerOutputFilePSFMStarEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFMStarEstimateBParameter = this;
}

IsoString DebayerOutputFilePSFMStarEstimateB::Id() const
{
   return "psfMStarEstimateB";
}

int DebayerOutputFilePSFMStarEstimateB::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFMStarEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFMStarEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFNStarEstimateR::DebayerOutputFilePSFNStarEstimateR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFNStarEstimateRParameter = this;
}

IsoString DebayerOutputFilePSFNStarEstimateR::Id() const
{
   return "psfNStarEstimateR";
}

int DebayerOutputFilePSFNStarEstimateR::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFNStarEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFNStarEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFNStarEstimateG::DebayerOutputFilePSFNStarEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFNStarEstimateGParameter = this;
}

IsoString DebayerOutputFilePSFNStarEstimateG::Id() const
{
   return "psfNStarEstimateG";
}

int DebayerOutputFilePSFNStarEstimateG::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFNStarEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFNStarEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFNStarEstimateB::DebayerOutputFilePSFNStarEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFNStarEstimateBParameter = this;
}

IsoString DebayerOutputFilePSFNStarEstimateB::Id() const
{
   return "psfNStarEstimateB";
}

int DebayerOutputFilePSFNStarEstimateB::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFNStarEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFNStarEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFCountR::DebayerOutputFilePSFCountR( MetaTable* T ) : MetaInt32( T )
{
   TheDebayerOutputFilePSFCountRParameter = this;
}

IsoString DebayerOutputFilePSFCountR::Id() const
{
   return "psfCountR";
}

bool DebayerOutputFilePSFCountR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFCountG::DebayerOutputFilePSFCountG( MetaTable* T ) : MetaInt32( T )
{
   TheDebayerOutputFilePSFCountGParameter = this;
}

IsoString DebayerOutputFilePSFCountG::Id() const
{
   return "psfCountG";
}

bool DebayerOutputFilePSFCountG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFCountB::DebayerOutputFilePSFCountB( MetaTable* T ) : MetaInt32( T )
{
   TheDebayerOutputFilePSFCountBParameter = this;
}

IsoString DebayerOutputFilePSFCountB::Id() const
{
   return "psfCountB";
}

bool DebayerOutputFilePSFCountB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseEstimateR::DebayerOutputFileNoiseEstimateR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseEstimateRParameter = this;
}

IsoString DebayerOutputFileNoiseEstimateR::Id() const
{
   return "noiseEstimateR";
}

int DebayerOutputFileNoiseEstimateR::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseEstimateG::DebayerOutputFileNoiseEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseEstimateGParameter = this;
}

IsoString DebayerOutputFileNoiseEstimateG::Id() const
{
   return "noiseEstimateG";
}

int DebayerOutputFileNoiseEstimateG::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseEstimateB::DebayerOutputFileNoiseEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseEstimateBParameter = this;
}

IsoString DebayerOutputFileNoiseEstimateB::Id() const
{
   return "noiseEstimateB";
}

int DebayerOutputFileNoiseEstimateB::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseFractionR::DebayerOutputFileNoiseFractionR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseFractionRParameter = this;
}

IsoString DebayerOutputFileNoiseFractionR::Id() const
{
   return "noiseFractionR";
}

int DebayerOutputFileNoiseFractionR::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseFractionR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseFractionG::DebayerOutputFileNoiseFractionG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseFractionGParameter = this;
}

IsoString DebayerOutputFileNoiseFractionG::Id() const
{
   return "noiseFractionG";
}

int DebayerOutputFileNoiseFractionG::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseFractionG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseFractionB::DebayerOutputFileNoiseFractionB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseFractionBParameter = this;
}

IsoString DebayerOutputFileNoiseFractionB::Id() const
{
   return "noiseFractionB";
}

int DebayerOutputFileNoiseFractionB::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseFractionB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseScaleLowR::DebayerOutputFileNoiseScaleLowR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseScaleLowRParameter = this;
}

IsoString DebayerOutputFileNoiseScaleLowR::Id() const
{
   return "noiseScaleLowR";
}

int DebayerOutputFileNoiseScaleLowR::Precision() const
{
   return 6;
}

bool DebayerOutputFileNoiseScaleLowR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseScaleLowR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseScaleLowG::DebayerOutputFileNoiseScaleLowG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseScaleLowGParameter = this;
}

IsoString DebayerOutputFileNoiseScaleLowG::Id() const
{
   return "noiseScaleLowG";
}

int DebayerOutputFileNoiseScaleLowG::Precision() const
{
   return 6;
}

bool DebayerOutputFileNoiseScaleLowG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseScaleLowG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseScaleLowB::DebayerOutputFileNoiseScaleLowB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseScaleLowBParameter = this;
}

IsoString DebayerOutputFileNoiseScaleLowB::Id() const
{
   return "noiseScaleLowB";
}

int DebayerOutputFileNoiseScaleLowB::Precision() const
{
   return 6;
}

bool DebayerOutputFileNoiseScaleLowB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseScaleLowB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseScaleHighR::DebayerOutputFileNoiseScaleHighR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseScaleHighRParameter = this;
}

IsoString DebayerOutputFileNoiseScaleHighR::Id() const
{
   return "noiseScaleHighR";
}

int DebayerOutputFileNoiseScaleHighR::Precision() const
{
   return 6;
}

bool DebayerOutputFileNoiseScaleHighR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseScaleHighR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseScaleHighG::DebayerOutputFileNoiseScaleHighG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseScaleHighGParameter = this;
}

IsoString DebayerOutputFileNoiseScaleHighG::Id() const
{
   return "noiseScaleHighG";
}

int DebayerOutputFileNoiseScaleHighG::Precision() const
{
   return 6;
}

bool DebayerOutputFileNoiseScaleHighG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseScaleHighG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseScaleHighB::DebayerOutputFileNoiseScaleHighB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFileNoiseScaleHighBParameter = this;
}

IsoString DebayerOutputFileNoiseScaleHighB::Id() const
{
   return "noiseScaleHighB";
}

int DebayerOutputFileNoiseScaleHighB::Precision() const
{
   return 6;
}

bool DebayerOutputFileNoiseScaleHighB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseScaleHighB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseAlgorithmR::DebayerOutputFileNoiseAlgorithmR( MetaTable* T ) : MetaString( T )
{
   TheDebayerOutputFileNoiseAlgorithmRParameter = this;
}

IsoString DebayerOutputFileNoiseAlgorithmR::Id() const
{
   return "noiseAlgorithmR";
}

bool DebayerOutputFileNoiseAlgorithmR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseAlgorithmG::DebayerOutputFileNoiseAlgorithmG( MetaTable* T ) : MetaString( T )
{
   TheDebayerOutputFileNoiseAlgorithmGParameter = this;
}

IsoString DebayerOutputFileNoiseAlgorithmG::Id() const
{
   return "noiseAlgorithmG";
}

bool DebayerOutputFileNoiseAlgorithmG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseAlgorithmB::DebayerOutputFileNoiseAlgorithmB( MetaTable* T ) : MetaString( T )
{
   TheDebayerOutputFileNoiseAlgorithmBParameter = this;
}

IsoString DebayerOutputFileNoiseAlgorithmB::Id() const
{
   return "noiseAlgorithmB";
}

bool DebayerOutputFileNoiseAlgorithmB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DebayerParameters.cpp - Released 2023-08-10T11:44:14Z
