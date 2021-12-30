//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.17
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 1.10.2
// ----------------------------------------------------------------------------
// DebayerParameters.cpp - Released 2021-12-29T20:37:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

#include "DebayerParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DebayerBayerPatternParameter*	            TheDebayerBayerPatternParameter = nullptr;
DebayerMethodParameter*                   TheDebayerMethodParameter = nullptr;
DebayerFBDDNoiseReduction*                TheDebayerFBDDNoiseReductionParameter = nullptr;
DebayerShowImages*                        TheDebayerShowImagesParameter = nullptr;
DebayerCFASourceFilePath*                 TheDebayerCFASourceFilePathParameter = nullptr;
DebayerTargetItems*                       TheDebayerTargetItemsParameter = nullptr;
DebayerTargetEnabled*                     TheDebayerTargetEnabledParameter = nullptr;
DebayerTargetImage*                       TheDebayerTargetImageParameter = nullptr;
DebayerNoGUIMessages*                     TheDebayerNoGUIMessagesParameter = nullptr;

DebayerEvaluateNoise*                     TheDebayerEvaluateNoiseParameter = nullptr;
DebayerNoiseEvaluationAlgorithm*          TheDebayerNoiseEvaluationAlgorithmParameter = nullptr;

DebayerEvaluateSignal*                    TheDebayerEvaluateSignalParameter = nullptr;
DebayerStructureLayers*                   TheDebayerStructureLayersParameter = nullptr;
DebayerNoiseLayers*                       TheDebayerNoiseLayersParameter = nullptr;
DebayerHotPixelFilterRadius*              TheDebayerHotPixelFilterRadiusParameter = nullptr;
DebayerNoiseReductionFilterRadius*        TheDebayerNoiseReductionFilterRadiusParameter = nullptr;
DebayerMinStructureSize*                  TheDebayerMinStructureSizeParameter = nullptr;
DebayerPSFType*                           TheDebayerPSFTypeParameter = nullptr;
DebayerPSFRejectionLimit*                 TheDebayerPSFRejectionLimitParameter = nullptr;
DebayerPSFHighClippingPoint*              TheDebayerPSFHighClippingPointParameter = nullptr;
DebayerMaxStars*                          TheDebayerMaxStarsParameter = nullptr;

DebayerInputHints*                        TheDebayerInputHintsParameter = nullptr;
DebayerOutputHints*                       TheDebayerOutputHintsParameter = nullptr;
DebayerOutputRGBImages*                   TheDebayerOutputRGBImagesParameter = nullptr;
DebayerOutputSeparateChannels*            TheDebayerOutputSeparateChannelsParameter = nullptr;
DebayerOutputDirectory*                   TheDebayerOutputDirectoryParameter = nullptr;
DebayerOutputExtension*                   TheDebayerOutputExtensionParameter = nullptr;
DebayerOutputPrefix*                      TheDebayerOutputPrefixParameter = nullptr;
DebayerOutputPostfix*                     TheDebayerOutputPostfixParameter = nullptr;
DebayerOverwriteExistingFiles*            TheDebayerOverwriteExistingFilesParameter = nullptr;
DebayerOnError*                           TheDebayerOnErrorParameter = nullptr;
DebayerUseFileThreads*                    TheDebayerUseFileThreadsParameter = nullptr;
DebayerFileThreadOverload*                TheDebayerFileThreadOverloadParameter = nullptr;
DebayerMaxFileReadThreads*                TheDebayerMaxFileReadThreadsParameter = nullptr;
DebayerMaxFileWriteThreads*               TheDebayerMaxFileWriteThreadsParameter = nullptr;
DebayerMemoryLoadControl*                 TheDebayerMemoryLoadControlParameter = nullptr;
DebayerMemoryLoadLimit*                   TheDebayerMemoryLoadLimitParameter = nullptr;

DebayerOutputImage*                       TheDebayerOutputImageParameter = nullptr;
DebayerOutputChannelImageR*               TheDebayerOutputChannelImageRParameter = nullptr;
DebayerOutputChannelImageG*               TheDebayerOutputChannelImageGParameter = nullptr;
DebayerOutputChannelImageB*               TheDebayerOutputChannelImageBParameter = nullptr;
DebayerPSFSignalEstimateR*                TheDebayerPSFSignalEstimateRParameter = nullptr;
DebayerPSFSignalEstimateG*                TheDebayerPSFSignalEstimateGParameter = nullptr;
DebayerPSFSignalEstimateB*                TheDebayerPSFSignalEstimateBParameter = nullptr;
DebayerPSFSignalPowerEstimateR*           TheDebayerPSFSignalPowerEstimateRParameter = nullptr;
DebayerPSFSignalPowerEstimateG*           TheDebayerPSFSignalPowerEstimateGParameter = nullptr;
DebayerPSFSignalPowerEstimateB*           TheDebayerPSFSignalPowerEstimateBParameter = nullptr;
DebayerPSFFluxEstimateR*                  TheDebayerPSFFluxEstimateRParameter = nullptr;
DebayerPSFFluxEstimateG*                  TheDebayerPSFFluxEstimateGParameter = nullptr;
DebayerPSFFluxEstimateB*                  TheDebayerPSFFluxEstimateBParameter = nullptr;
DebayerPSFFluxPowerEstimateR*             TheDebayerPSFFluxPowerEstimateRParameter = nullptr;
DebayerPSFFluxPowerEstimateG*             TheDebayerPSFFluxPowerEstimateGParameter = nullptr;
DebayerPSFFluxPowerEstimateB*             TheDebayerPSFFluxPowerEstimateBParameter = nullptr;
DebayerPSFCountR*                         TheDebayerPSFCountRParameter = nullptr;
DebayerPSFCountG*                         TheDebayerPSFCountGParameter = nullptr;
DebayerPSFCountB*                         TheDebayerPSFCountBParameter = nullptr;
DebayerNoiseEstimateR*                    TheDebayerNoiseEstimateRParameter = nullptr;
DebayerNoiseEstimateG*                    TheDebayerNoiseEstimateGParameter = nullptr;
DebayerNoiseEstimateB*                    TheDebayerNoiseEstimateBParameter = nullptr;
DebayerNoiseFractionR*                    TheDebayerNoiseFractionRParameter = nullptr;
DebayerNoiseFractionG*                    TheDebayerNoiseFractionGParameter = nullptr;
DebayerNoiseFractionB*                    TheDebayerNoiseFractionBParameter = nullptr;
DebayerNoiseScaleLowR*                    TheDebayerNoiseScaleLowRParameter = nullptr;
DebayerNoiseScaleLowG*                    TheDebayerNoiseScaleLowGParameter = nullptr;
DebayerNoiseScaleLowB*                    TheDebayerNoiseScaleLowBParameter = nullptr;
DebayerNoiseScaleHighR*                   TheDebayerNoiseScaleHighRParameter = nullptr;
DebayerNoiseScaleHighG*                   TheDebayerNoiseScaleHighGParameter = nullptr;
DebayerNoiseScaleHighB*                   TheDebayerNoiseScaleHighBParameter = nullptr;
DebayerNoiseAlgorithmR*                   TheDebayerNoiseAlgorithmRParameter = nullptr;
DebayerNoiseAlgorithmG*                   TheDebayerNoiseAlgorithmGParameter = nullptr;
DebayerNoiseAlgorithmB*                   TheDebayerNoiseAlgorithmBParameter = nullptr;

DebayerOutputFileData*                    TheDebayerOutputFileDataParameter = nullptr;
DebayerOutputFilePath*                    TheDebayerOutputFilePathParameter = nullptr;
DebayerOutputChannelFilePathR*            TheDebayerOutputChannelFilePathRParameter = nullptr;
DebayerOutputChannelFilePathG*            TheDebayerOutputChannelFilePathGParameter = nullptr;
DebayerOutputChannelFilePathB*            TheDebayerOutputChannelFilePathBParameter = nullptr;
DebayerOutputFilePSFSignalEstimateR*      TheDebayerOutputFilePSFSignalEstimateRParameter = nullptr;
DebayerOutputFilePSFSignalEstimateG*      TheDebayerOutputFilePSFSignalEstimateGParameter = nullptr;
DebayerOutputFilePSFSignalEstimateB*      TheDebayerOutputFilePSFSignalEstimateBParameter = nullptr;
DebayerOutputFilePSFSignalPowerEstimateR* TheDebayerOutputFilePSFSignalPowerEstimateRParameter = nullptr;
DebayerOutputFilePSFSignalPowerEstimateG* TheDebayerOutputFilePSFSignalPowerEstimateGParameter = nullptr;
DebayerOutputFilePSFSignalPowerEstimateB* TheDebayerOutputFilePSFSignalPowerEstimateBParameter = nullptr;
DebayerOutputFilePSFFluxEstimateR*        TheDebayerOutputFilePSFFluxEstimateRParameter = nullptr;
DebayerOutputFilePSFFluxEstimateG*        TheDebayerOutputFilePSFFluxEstimateGParameter = nullptr;
DebayerOutputFilePSFFluxEstimateB*        TheDebayerOutputFilePSFFluxEstimateBParameter = nullptr;
DebayerOutputFilePSFFluxPowerEstimateR*   TheDebayerOutputFilePSFFluxPowerEstimateRParameter = nullptr;
DebayerOutputFilePSFFluxPowerEstimateG*   TheDebayerOutputFilePSFFluxPowerEstimateGParameter = nullptr;
DebayerOutputFilePSFFluxPowerEstimateB*   TheDebayerOutputFilePSFFluxPowerEstimateBParameter = nullptr;
DebayerOutputFilePSFCountR*               TheDebayerOutputFilePSFCountRParameter = nullptr;
DebayerOutputFilePSFCountG*               TheDebayerOutputFilePSFCountGParameter = nullptr;
DebayerOutputFilePSFCountB*               TheDebayerOutputFilePSFCountBParameter = nullptr;
DebayerOutputFileNoiseEstimateR*          TheDebayerOutputFileNoiseEstimateRParameter = nullptr;
DebayerOutputFileNoiseEstimateG*          TheDebayerOutputFileNoiseEstimateGParameter = nullptr;
DebayerOutputFileNoiseEstimateB*          TheDebayerOutputFileNoiseEstimateBParameter = nullptr;
DebayerOutputFileNoiseFractionR*          TheDebayerOutputFileNoiseFractionRParameter = nullptr;
DebayerOutputFileNoiseFractionG*          TheDebayerOutputFileNoiseFractionGParameter = nullptr;
DebayerOutputFileNoiseFractionB*          TheDebayerOutputFileNoiseFractionBParameter = nullptr;
DebayerOutputFileNoiseScaleLowR*          TheDebayerOutputFileNoiseScaleLowRParameter = nullptr;
DebayerOutputFileNoiseScaleLowG*          TheDebayerOutputFileNoiseScaleLowGParameter = nullptr;
DebayerOutputFileNoiseScaleLowB*          TheDebayerOutputFileNoiseScaleLowBParameter = nullptr;
DebayerOutputFileNoiseScaleHighR*         TheDebayerOutputFileNoiseScaleHighRParameter = nullptr;
DebayerOutputFileNoiseScaleHighG*         TheDebayerOutputFileNoiseScaleHighGParameter = nullptr;
DebayerOutputFileNoiseScaleHighB*         TheDebayerOutputFileNoiseScaleHighBParameter = nullptr;
DebayerOutputFileNoiseAlgorithmR*         TheDebayerOutputFileNoiseAlgorithmRParameter = nullptr;
DebayerOutputFileNoiseAlgorithmG*         TheDebayerOutputFileNoiseAlgorithmGParameter = nullptr;
DebayerOutputFileNoiseAlgorithmB*         TheDebayerOutputFileNoiseAlgorithmBParameter = nullptr;

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
   case Gaussian:      return "PSFType_Gaussian";
   default:
   case Moffat4:       return "PSFType_Moffat4";
   case Moffat6:       return "PSFType_Moffat6";
   case Moffat8:       return "PSFType_Moffat8";
   case VariableShape: return "PSFType_VariableShape";
   }
}

IsoString DebayerPSFType::FunctionName( pcl_enum i )
{
   switch ( i )
   {
   case Gaussian:      return "Gaussian";
   default:
   case Moffat4:       return "Moffat4";
   case Moffat6:       return "Moffat6";
   case Moffat8:       return "Moffat8";
   case VariableShape: return "VariableShape";
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

DebayerPSFRejectionLimit::DebayerPSFRejectionLimit( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerPSFRejectionLimitParameter = this;
}

IsoString DebayerPSFRejectionLimit::Id() const
{
   return "psfRejectionLimit";
}

int DebayerPSFRejectionLimit::Precision() const
{
   return 2;
}

double DebayerPSFRejectionLimit::DefaultValue() const
{
   return 0.90;
}

double DebayerPSFRejectionLimit::MinimumValue() const
{
   return 0.50;
}

double DebayerPSFRejectionLimit::MaximumValue() const
{
   return 1.00;
}

// ----------------------------------------------------------------------------

DebayerPSFHighClippingPoint::DebayerPSFHighClippingPoint( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerPSFHighClippingPointParameter = this;
}

IsoString DebayerPSFHighClippingPoint::Id() const
{
   return "psfHighClippingPoint";
}

int DebayerPSFHighClippingPoint::Precision() const
{
   return 2;
}

double DebayerPSFHighClippingPoint::DefaultValue() const
{
   return 0.85;
}

double DebayerPSFHighClippingPoint::MinimumValue() const
{
   return 0.50;
}

double DebayerPSFHighClippingPoint::MaximumValue() const
{
   return 1.00;
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

DebayerPSFSignalEstimateR::DebayerPSFSignalEstimateR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFSignalEstimateRParameter = this;
}

IsoString DebayerPSFSignalEstimateR::Id() const
{
   return "psfSignalEstimateR";
}

int DebayerPSFSignalEstimateR::Precision() const
{
   return 4;
}

bool DebayerPSFSignalEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerPSFSignalEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFSignalEstimateG::DebayerPSFSignalEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFSignalEstimateGParameter = this;
}

IsoString DebayerPSFSignalEstimateG::Id() const
{
   return "psfSignalEstimateG";
}

int DebayerPSFSignalEstimateG::Precision() const
{
   return 4;
}

bool DebayerPSFSignalEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerPSFSignalEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFSignalEstimateB::DebayerPSFSignalEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFSignalEstimateBParameter = this;
}

IsoString DebayerPSFSignalEstimateB::Id() const
{
   return "psfSignalEstimateB";
}

int DebayerPSFSignalEstimateB::Precision() const
{
   return 4;
}

bool DebayerPSFSignalEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerPSFSignalEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFSignalPowerEstimateR::DebayerPSFSignalPowerEstimateR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFSignalPowerEstimateRParameter = this;
}

IsoString DebayerPSFSignalPowerEstimateR::Id() const
{
   return "psfSignalPowerEstimateR";
}

IsoString DebayerPSFSignalPowerEstimateR::Aliases() const
{
   return "psfPowerEstimateR";
}

int DebayerPSFSignalPowerEstimateR::Precision() const
{
   return 4;
}

bool DebayerPSFSignalPowerEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerPSFSignalPowerEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFSignalPowerEstimateG::DebayerPSFSignalPowerEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFSignalPowerEstimateGParameter = this;
}

IsoString DebayerPSFSignalPowerEstimateG::Id() const
{
   return "psfSignalPowerEstimateG";
}

IsoString DebayerPSFSignalPowerEstimateG::Aliases() const
{
   return "psfPowerEstimateG";
}

int DebayerPSFSignalPowerEstimateG::Precision() const
{
   return 4;
}

bool DebayerPSFSignalPowerEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerPSFSignalPowerEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFSignalPowerEstimateB::DebayerPSFSignalPowerEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFSignalPowerEstimateBParameter = this;
}

IsoString DebayerPSFSignalPowerEstimateB::Id() const
{
   return "psfSignalPowerEstimateB";
}

IsoString DebayerPSFSignalPowerEstimateB::Aliases() const
{
   return "psfPowerEstimateB";
}

int DebayerPSFSignalPowerEstimateB::Precision() const
{
   return 4;
}

bool DebayerPSFSignalPowerEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerPSFSignalPowerEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFFluxEstimateR::DebayerPSFFluxEstimateR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFFluxEstimateRParameter = this;
}

IsoString DebayerPSFFluxEstimateR::Id() const
{
   return "psfFluxEstimateR";
}

int DebayerPSFFluxEstimateR::Precision() const
{
   return 4;
}

bool DebayerPSFFluxEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerPSFFluxEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFFluxEstimateG::DebayerPSFFluxEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFFluxEstimateGParameter = this;
}

IsoString DebayerPSFFluxEstimateG::Id() const
{
   return "psfFluxEstimateG";
}

int DebayerPSFFluxEstimateG::Precision() const
{
   return 4;
}

bool DebayerPSFFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerPSFFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFFluxEstimateB::DebayerPSFFluxEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFFluxEstimateBParameter = this;
}

IsoString DebayerPSFFluxEstimateB::Id() const
{
   return "psfFluxEstimateB";
}

int DebayerPSFFluxEstimateB::Precision() const
{
   return 4;
}

bool DebayerPSFFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerPSFFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFFluxPowerEstimateR::DebayerPSFFluxPowerEstimateR( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFFluxPowerEstimateRParameter = this;
}

IsoString DebayerPSFFluxPowerEstimateR::Id() const
{
   return "psfFluxPowerEstimateR";
}

int DebayerPSFFluxPowerEstimateR::Precision() const
{
   return 4;
}

bool DebayerPSFFluxPowerEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerPSFFluxPowerEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFFluxPowerEstimateG::DebayerPSFFluxPowerEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFFluxPowerEstimateGParameter = this;
}

IsoString DebayerPSFFluxPowerEstimateG::Id() const
{
   return "psfFluxPowerEstimateG";
}

int DebayerPSFFluxPowerEstimateG::Precision() const
{
   return 4;
}

bool DebayerPSFFluxPowerEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerPSFFluxPowerEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerPSFFluxPowerEstimateB::DebayerPSFFluxPowerEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheDebayerPSFFluxPowerEstimateBParameter = this;
}

IsoString DebayerPSFFluxPowerEstimateB::Id() const
{
   return "psfFluxPowerEstimateB";
}

int DebayerPSFFluxPowerEstimateB::Precision() const
{
   return 4;
}

bool DebayerPSFFluxPowerEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerPSFFluxPowerEstimateB::IsReadOnly() const
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

DebayerOutputFilePSFSignalEstimateR::DebayerOutputFilePSFSignalEstimateR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFSignalEstimateRParameter = this;
}

IsoString DebayerOutputFilePSFSignalEstimateR::Id() const
{
   return "psfSignalEstimateR";
}

int DebayerOutputFilePSFSignalEstimateR::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFSignalEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFSignalEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFSignalEstimateG::DebayerOutputFilePSFSignalEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFSignalEstimateGParameter = this;
}

IsoString DebayerOutputFilePSFSignalEstimateG::Id() const
{
   return "psfSignalEstimateG";
}

int DebayerOutputFilePSFSignalEstimateG::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFSignalEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFSignalEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFSignalEstimateB::DebayerOutputFilePSFSignalEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFSignalEstimateBParameter = this;
}

IsoString DebayerOutputFilePSFSignalEstimateB::Id() const
{
   return "psfSignalEstimateB";
}

int DebayerOutputFilePSFSignalEstimateB::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFSignalEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFSignalEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFSignalPowerEstimateR::DebayerOutputFilePSFSignalPowerEstimateR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFSignalPowerEstimateRParameter = this;
}

IsoString DebayerOutputFilePSFSignalPowerEstimateR::Id() const
{
   return "psfSignalPowerEstimateR";
}

int DebayerOutputFilePSFSignalPowerEstimateR::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFSignalPowerEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFSignalPowerEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFSignalPowerEstimateG::DebayerOutputFilePSFSignalPowerEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFSignalPowerEstimateGParameter = this;
}

IsoString DebayerOutputFilePSFSignalPowerEstimateG::Id() const
{
   return "psfSignalPowerEstimateG";
}

int DebayerOutputFilePSFSignalPowerEstimateG::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFSignalPowerEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFSignalPowerEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFSignalPowerEstimateB::DebayerOutputFilePSFSignalPowerEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFSignalPowerEstimateBParameter = this;
}

IsoString DebayerOutputFilePSFSignalPowerEstimateB::Id() const
{
   return "psfSignalPowerEstimateB";
}

int DebayerOutputFilePSFSignalPowerEstimateB::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFSignalPowerEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFSignalPowerEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFFluxEstimateR::DebayerOutputFilePSFFluxEstimateR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFFluxEstimateRParameter = this;
}

IsoString DebayerOutputFilePSFFluxEstimateR::Id() const
{
   return "psfFluxEstimateR";
}

int DebayerOutputFilePSFFluxEstimateR::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFFluxEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFFluxEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFFluxEstimateG::DebayerOutputFilePSFFluxEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFFluxEstimateGParameter = this;
}

IsoString DebayerOutputFilePSFFluxEstimateG::Id() const
{
   return "psfFluxEstimateG";
}

int DebayerOutputFilePSFFluxEstimateG::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFFluxEstimateB::DebayerOutputFilePSFFluxEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFFluxEstimateBParameter = this;
}

IsoString DebayerOutputFilePSFFluxEstimateB::Id() const
{
   return "psfFluxEstimateB";
}

int DebayerOutputFilePSFFluxEstimateB::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFFluxPowerEstimateR::DebayerOutputFilePSFFluxPowerEstimateR( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFFluxPowerEstimateRParameter = this;
}

IsoString DebayerOutputFilePSFFluxPowerEstimateR::Id() const
{
   return "psfFluxPowerEstimateR";
}

int DebayerOutputFilePSFFluxPowerEstimateR::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFFluxPowerEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFFluxPowerEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFFluxPowerEstimateG::DebayerOutputFilePSFFluxPowerEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFFluxPowerEstimateGParameter = this;
}

IsoString DebayerOutputFilePSFFluxPowerEstimateG::Id() const
{
   return "psfFluxPowerEstimateG";
}

int DebayerOutputFilePSFFluxPowerEstimateG::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFFluxPowerEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFFluxPowerEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePSFFluxPowerEstimateB::DebayerOutputFilePSFFluxPowerEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheDebayerOutputFilePSFFluxPowerEstimateBParameter = this;
}

IsoString DebayerOutputFilePSFFluxPowerEstimateB::Id() const
{
   return "psfFluxPowerEstimateB";
}

int DebayerOutputFilePSFFluxPowerEstimateB::Precision() const
{
   return 4;
}

bool DebayerOutputFilePSFFluxPowerEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFilePSFFluxPowerEstimateB::IsReadOnly() const
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
// EOF DebayerParameters.cpp - Released 2021-12-29T20:37:28Z
