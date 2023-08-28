//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.5.2
// ----------------------------------------------------------------------------
// ImageIntegrationParameters.cpp - Released 2023-08-28T15:23:41Z
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

#include "ImageIntegrationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IIImages*                               TheIIImagesParameter = nullptr;
IIImageEnabled*                         TheIIImageEnabledParameter = nullptr;
IIImagePath*                            TheIIImagePathParameter = nullptr;
IIDrizzlePath*                          TheIIDrizzlePathParameter = nullptr;
IILocalNormalizationDataPath*           TheIILocalNormalizationDataPathParameter = nullptr;
IIInputHints*                           TheIIInputHintsParameter = nullptr;
IICombination*                          TheIICombinationParameter = nullptr;
IIWeightMode*                           TheIIWeightModeParameter = nullptr;
IIWeightKeyword*                        TheIIWeightKeywordParameter = nullptr;
IIWeightScale*                          TheIIWeightScaleParameter = nullptr;
IIMinWeight*                            TheIIMinWeightParameter = nullptr;
IICSVWeights*                           TheIICSVWeightsParameter = nullptr;
IIAdaptiveGridSize*                     TheIIAdaptiveGridSizeParameter = nullptr;
IIAdaptiveNoScale*                      TheIIAdaptiveNoScaleParameter = nullptr;
IIIgnoreNoiseKeywords*                  TheIIIgnoreNoiseKeywordsParameter = nullptr;
IINormalization*                        TheIINormalizationParameter = nullptr;
IIRejection*                            TheIIRejectionParameter = nullptr;
IIRejectionNormalization*               TheIIRejectionNormalizationParameter = nullptr;
IIMinMaxLow*                            TheIIMinMaxLowParameter = nullptr;
IIMinMaxHigh*                           TheIIMinMaxHighParameter = nullptr;
IIPercentileLow*                        TheIIPercentileLowParameter = nullptr;
IIPercentileHigh*                       TheIIPercentileHighParameter = nullptr;
IISigmaLow*                             TheIISigmaLowParameter = nullptr;
IISigmaHigh*                            TheIISigmaHighParameter = nullptr;
IIWinsorizationCutoff*                  TheIIWinsorizationCutoffParameter = nullptr;
IILinearFitLow*                         TheIILinearFitLowParameter = nullptr;
IILinearFitHigh*                        TheIILinearFitHighParameter = nullptr;
IIESDOutliersFraction*                  TheIIESDOutliersFractionParameter = nullptr;
IIESDAlpha*                             TheIIESDAlphaParameter = nullptr;
IIESDLowRelaxation*                     TheIIESDLowRelaxationParameter = nullptr;
IIRCRLimit*                             TheIIRCRLimitParameter = nullptr;
IICCDGain*                              TheIICCDGainParameter = nullptr;
IICCDReadNoise*                         TheIICCDReadNoiseParameter = nullptr;
IICCDScaleNoise*                        TheIICCDScaleNoiseParameter = nullptr;
IIClipLow*                              TheIIClipLowParameter = nullptr;
IIClipHigh*                             TheIIClipHighParameter = nullptr;
IIRangeClipLow*                         TheIIRangeClipLowParameter = nullptr;
IIRangeLow*                             TheIIRangeLowParameter = nullptr;
IIRangeClipHigh*                        TheIIRangeClipHighParameter = nullptr;
IIRangeHigh*                            TheIIRangeHighParameter = nullptr;
IIMapRangeRejection*                    TheIIMapRangeRejectionParameter = nullptr;
IIReportRangeRejection*                 TheIIReportRangeRejectionParameter = nullptr;
IILargeScaleClipLow*                    TheIILargeScaleClipLowParameter = nullptr;
IILargeScaleClipLowProtectedLayers*     TheIILargeScaleClipLowProtectedLayersParameter = nullptr;
IILargeScaleClipLowGrowth*              TheIILargeScaleClipLowGrowthParameter = nullptr;
IILargeScaleClipHigh*                   TheIILargeScaleClipHighParameter = nullptr;
IILargeScaleClipHighProtectedLayers*    TheIILargeScaleClipHighProtectedLayersParameter = nullptr;
IILargeScaleClipHighGrowth*             TheIILargeScaleClipHighGrowthParameter = nullptr;
IIGenerate64BitResult*                  TheIIGenerate64BitResultParameter = nullptr;
IIGenerateRejectionMaps*                TheIIGenerateRejectionMapsParameter = nullptr;
IIGenerateIntegratedImage*              TheIIGenerateIntegratedImageParameter = nullptr;
IIGenerateDrizzleData*                  TheIIGenerateDrizzleDataParameter = nullptr;
IIClosePreviousImages*                  TheIIClosePreviousImagesParameter = nullptr;
IIBufferSize*                           TheIIBufferSizeParameter = nullptr;
IIStackSize*                            TheIIStackSizeParameter = nullptr;
IIAutoMemorySize*                       TheIIAutoMemorySizeParameter = nullptr;
IIAutoMemoryLimit*                      TheIIAutoMemoryLimitParameter = nullptr;
IIUseROI*                               TheIIUseROIParameter = nullptr;
IIROIX0*                                TheIIROIX0Parameter = nullptr;
IIROIY0*                                TheIIROIY0Parameter = nullptr;
IIROIX1*                                TheIIROIX1Parameter = nullptr;
IIROIY1*                                TheIIROIY1Parameter = nullptr;
IIUseCache*                             TheIIUseCacheParameter = nullptr;
IIEvaluateSNR*                          TheIIEvaluateSNRParameter = nullptr;
IINoiseEvaluationAlgorithm*             TheIINoiseEvaluationAlgorithmParameter = nullptr;
IIMRSMinDataFraction*                   TheIIMRSMinDataFractionParameter = nullptr;
IIPSFStructureLayers*                   TheIIPSFStructureLayersParameter = nullptr;
IIPSFType*                              TheIIPSFTypeParameter = nullptr;
IISubtractPedestals*                    TheIISubtractPedestalsParameter = nullptr;
IITruncateOnOutOfRange*                 TheIITruncateOnOutOfRangeParameter = nullptr;
IINoGUIMessages*                        TheIINoGUIMessagesParameter = nullptr;
IIShowImages*                           TheIIShowImagesParameter = nullptr;
IIUseFileThreads*                       TheIIUseFileThreadsParameter = nullptr;
IIFileThreadOverload*                   TheIIFileThreadOverloadParameter = nullptr;
IIUseBufferThreads*                     TheIIUseBufferThreadsParameter = nullptr;
IIMaxBufferThreads*                     TheIIMaxBufferThreadsParameter = nullptr;

IIIntegrationImageId*                   TheIIIntegrationImageIdParameter = nullptr;
IILowRejectionMapImageId*               TheIILowRejectionMapImageIdParameter = nullptr;
IIHighRejectionMapImageId*              TheIIHighRejectionMapImageIdParameter = nullptr;
IISlopeMapImageId*                      TheIISlopeMapImageIdParameter = nullptr;
IINumberOfChannels*                     TheIINumberOfChannelsParameter = nullptr;
IINumberOfPixels*                       TheIINumberOfPixelsParameter = nullptr;
IITotalPixels*                          TheIITotalPixelsParameter = nullptr;
IIOutputRangeLow*                       TheIIOutputRangeLowParameter = nullptr;
IIOutputRangeHigh*                      TheIIOutputRangeHighParameter = nullptr;
IITotalRejectedLowRK*                   TheIITotalRejectedLowRKParameter = nullptr;
IITotalRejectedLowG*                    TheIITotalRejectedLowGParameter = nullptr;
IITotalRejectedLowB*                    TheIITotalRejectedLowBParameter = nullptr;
IITotalRejectedHighRK*                  TheIITotalRejectedHighRKParameter = nullptr;
IITotalRejectedHighG*                   TheIITotalRejectedHighGParameter = nullptr;
IITotalRejectedHighB*                   TheIITotalRejectedHighBParameter = nullptr;
IIFinalNoiseEstimateRK*                 TheIIFinalNoiseEstimateRKParameter = nullptr;
IIFinalNoiseEstimateG*                  TheIIFinalNoiseEstimateGParameter = nullptr;
IIFinalNoiseEstimateB*                  TheIIFinalNoiseEstimateBParameter = nullptr;
IIFinalNoiseScaleEstimateLowRK*         TheIIFinalNoiseScaleEstimateLowRKParameter = nullptr;
IIFinalNoiseScaleEstimateLowG*          TheIIFinalNoiseScaleEstimateLowGParameter = nullptr;
IIFinalNoiseScaleEstimateLowB*          TheIIFinalNoiseScaleEstimateLowBParameter = nullptr;
IIFinalNoiseScaleEstimateHighRK*        TheIIFinalNoiseScaleEstimateHighRKParameter = nullptr;
IIFinalNoiseScaleEstimateHighG*         TheIIFinalNoiseScaleEstimateHighGParameter = nullptr;
IIFinalNoiseScaleEstimateHighB*         TheIIFinalNoiseScaleEstimateHighBParameter = nullptr;
IIFinalNoiseAlgorithmRK*                TheIIFinalNoiseAlgorithmRKParameter = nullptr;
IIFinalNoiseAlgorithmG*                 TheIIFinalNoiseAlgorithmGParameter = nullptr;
IIFinalNoiseAlgorithmB*                 TheIIFinalNoiseAlgorithmBParameter = nullptr;
IIFinalScaleEstimateRK*                 TheIIFinalScaleEstimateRKParameter = nullptr;
IIFinalScaleEstimateG*                  TheIIFinalScaleEstimateGParameter = nullptr;
IIFinalScaleEstimateB*                  TheIIFinalScaleEstimateBParameter = nullptr;
IIFinalLocationEstimateRK*              TheIIFinalLocationEstimateRKParameter = nullptr;
IIFinalLocationEstimateG*               TheIIFinalLocationEstimateGParameter = nullptr;
IIFinalLocationEstimateB*               TheIIFinalLocationEstimateBParameter = nullptr;
IIFinalPSFTotalFluxEstimateRK*          TheIIFinalPSFTotalFluxEstimateRKParameter = nullptr;
IIFinalPSFTotalFluxEstimateG*           TheIIFinalPSFTotalFluxEstimateGParameter = nullptr;
IIFinalPSFTotalFluxEstimateB*           TheIIFinalPSFTotalFluxEstimateBParameter = nullptr;
IIFinalPSFTotalPowerFluxEstimateRK*     TheIIFinalPSFTotalPowerFluxEstimateRKParameter = nullptr;
IIFinalPSFTotalPowerFluxEstimateG*      TheIIFinalPSFTotalPowerFluxEstimateGParameter = nullptr;
IIFinalPSFTotalPowerFluxEstimateB*      TheIIFinalPSFTotalPowerFluxEstimateBParameter = nullptr;
IIFinalPSFTotalMeanFluxEstimateRK*      TheIIFinalPSFTotalMeanFluxEstimateRKParameter = nullptr;
IIFinalPSFTotalMeanFluxEstimateG*       TheIIFinalPSFTotalMeanFluxEstimateGParameter = nullptr;
IIFinalPSFTotalMeanFluxEstimateB*       TheIIFinalPSFTotalMeanFluxEstimateBParameter = nullptr;
IIFinalPSFTotalMeanPowerFluxEstimateRK* TheIIFinalPSFTotalMeanPowerFluxEstimateRKParameter = nullptr;
IIFinalPSFTotalMeanPowerFluxEstimateG*  TheIIFinalPSFTotalMeanPowerFluxEstimateGParameter = nullptr;
IIFinalPSFTotalMeanPowerFluxEstimateB*  TheIIFinalPSFTotalMeanPowerFluxEstimateBParameter = nullptr;
IIFinalPSFMStarEstimateRK*              TheIIFinalPSFMStarEstimateRKParameter = nullptr;
IIFinalPSFMStarEstimateG*               TheIIFinalPSFMStarEstimateGParameter = nullptr;
IIFinalPSFMStarEstimateB*               TheIIFinalPSFMStarEstimateBParameter = nullptr;
IIFinalPSFNStarEstimateRK*              TheIIFinalPSFNStarEstimateRKParameter = nullptr;
IIFinalPSFNStarEstimateG*               TheIIFinalPSFNStarEstimateGParameter = nullptr;
IIFinalPSFNStarEstimateB*               TheIIFinalPSFNStarEstimateBParameter = nullptr;
IIFinalPSFCountRK*                      TheIIFinalPSFCountRKParameter = nullptr;
IIFinalPSFCountG*                       TheIIFinalPSFCountGParameter = nullptr;
IIFinalPSFCountB*                       TheIIFinalPSFCountBParameter = nullptr;
IIReferenceNoiseReductionRK*            TheIIReferenceNoiseReductionRKParameter = nullptr;
IIReferenceNoiseReductionG*             TheIIReferenceNoiseReductionGParameter = nullptr;
IIReferenceNoiseReductionB*             TheIIReferenceNoiseReductionBParameter = nullptr;
IIMedianNoiseReductionRK*               TheIIMedianNoiseReductionRKParameter = nullptr;
IIMedianNoiseReductionG*                TheIIMedianNoiseReductionGParameter = nullptr;
IIMedianNoiseReductionB*                TheIIMedianNoiseReductionBParameter = nullptr;
IIReferenceSNRIncrementRK*              TheIIReferenceSNRIncrementRKParameter = nullptr;
IIReferenceSNRIncrementG*               TheIIReferenceSNRIncrementGParameter = nullptr;
IIReferenceSNRIncrementB*               TheIIReferenceSNRIncrementBParameter = nullptr;
IIAverageSNRIncrementRK*                TheIIAverageSNRIncrementRKParameter = nullptr;
IIAverageSNRIncrementG*                 TheIIAverageSNRIncrementGParameter = nullptr;
IIAverageSNRIncrementB*                 TheIIAverageSNRIncrementBParameter = nullptr;
IIImageData*                            TheIIImageDataParameter = nullptr;
IIImageWeightRK*                        TheIIImageWeightRKParameter = nullptr;
IIImageWeightG*                         TheIIImageWeightGParameter = nullptr;
IIImageWeightB*                         TheIIImageWeightBParameter = nullptr;
IIImageRejectedLowRK*                   TheIIImageRejectedLowRKParameter = nullptr;
IIImageRejectedLowG*                    TheIIImageRejectedLowGParameter = nullptr;
IIImageRejectedLowB*                    TheIIImageRejectedLowBParameter = nullptr;
IIImageRejectedHighRK*                  TheIIImageRejectedHighRKParameter = nullptr;
IIImageRejectedHighG*                   TheIIImageRejectedHighGParameter = nullptr;
IIImageRejectedHighB*                   TheIIImageRejectedHighBParameter = nullptr;

// ----------------------------------------------------------------------------

IIImages::IIImages( MetaProcess* P ) : MetaTable( P )
{
   TheIIImagesParameter = this;
}

IsoString IIImages::Id() const
{
   return "images";
}

// ----------------------------------------------------------------------------

IIImageEnabled::IIImageEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheIIImageEnabledParameter = this;
}

IsoString IIImageEnabled::Id() const
{
   return "enabled";
}

bool IIImageEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIImagePath::IIImagePath( MetaTable* T ) : MetaString( T )
{
   TheIIImagePathParameter = this;
}

IsoString IIImagePath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

IIDrizzlePath::IIDrizzlePath( MetaTable* T ) : MetaString( T )
{
   TheIIDrizzlePathParameter = this;
}

IsoString IIDrizzlePath::Id() const
{
   return "drizzlePath";
}

// ----------------------------------------------------------------------------

IILocalNormalizationDataPath::IILocalNormalizationDataPath( MetaTable* T ) : MetaString( T )
{
   TheIILocalNormalizationDataPathParameter = this;
}

IsoString IILocalNormalizationDataPath::Id() const
{
   return "localNormalizationDataPath";
}

// ----------------------------------------------------------------------------

IIInputHints::IIInputHints( MetaProcess* P ) : MetaString( P )
{
   TheIIInputHintsParameter = this;
}

IsoString IIInputHints::Id() const
{
   return "inputHints";
}

String IIInputHints::DefaultValue() const
{
   // Input format hints:
   // * XISF: fits-keywords normalize
   // * RAW: raw cfa
   // * FITS: signed-is-physical
   return "fits-keywords normalize raw cfa signed-is-physical";
}

// ----------------------------------------------------------------------------

IICombination::IICombination( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIICombinationParameter = this;
}

IsoString IICombination::Id() const
{
   return "combination";
}

size_type IICombination::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString IICombination::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Average: return "Average";
   case Median:  return "Median";
   case Minimum: return "Minimum";
   case Maximum: return "Maximum";
   }
}

int IICombination::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IICombination::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

IIWeightMode::IIWeightMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIIWeightModeParameter = this;
}

IsoString IIWeightMode::Id() const
{
   return "weightMode";
}

size_type IIWeightMode::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString IIWeightMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   case DontCare:           return "DontCare";
   case ExposureTimeWeight: return "ExposureTime";
   case SNREstimate:        return "SNREstimate";
   case SignalWeight:       return "SignalWeight";
   case MedianWeight:       return "MedianWeight";
   case AverageWeight:      return "AverageWeight";
   case KeywordWeight:      return "KeywordWeight";
   default:
   case PSFSignalWeight:    return "PSFSignalWeight";
   case PSFSNR:             return "PSFSNR";
   case PSFScaleSNR:        return "PSFScaleSNR";
   }
}

IsoString IIWeightMode::ElementAliases() const
{
   return "NoiseEvaluation=SNREstimate";
}

int IIWeightMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IIWeightMode::DefaultValueIndex() const
{
   return size_type( Default );
}

String IIWeightMode::ElementLabel( int index )
{
   switch ( index )
   {
   case DontCare:           return "Don't care (all weights = 1)";
   case ExposureTimeWeight: return "Exposure time";
   case SNREstimate:        return "SNR";
   case SignalWeight:       return "Average signal strength";
   case MedianWeight:       return "Median value";
   case AverageWeight:      return "Average value";
   case KeywordWeight:      return "FITS keyword";
   default:
   case PSFSignalWeight:    return "PSF Signal Weight";
   case PSFSNR:             return "PSF SNR";
   case PSFScaleSNR:        return "PSF Scale SNR";
   }
}

// ----------------------------------------------------------------------------

IIWeightKeyword::IIWeightKeyword( MetaProcess* P ) : MetaString( P )
{
   TheIIWeightKeywordParameter = this;
}

IsoString IIWeightKeyword::Id() const
{
   return "weightKeyword";
}

// ----------------------------------------------------------------------------

IIWeightScale::IIWeightScale( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIIWeightScaleParameter = this;
}

IsoString IIWeightScale::Id() const
{
   return "weightScale";
}

size_type IIWeightScale::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString IIWeightScale::ElementId( size_type i ) const
{
   switch ( i )
   {
   case AvgDev: return "WeightScale_AvgDev";
   case MAD:    return "WeightScale_MAD";
   default:
   case BWMV:   return "WeightScale_BWMV";

   // DEPRECATED
   case PBMV:   return "WeightScale_PBMV";
   case Sn:     return "WeightScale_Sn";
   case Qn:     return "WeightScale_Qn";
   case IKSS:   return "WeightScale_IKSS";
   }
}

int IIWeightScale::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IIWeightScale::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

IIMinWeight::IIMinWeight( MetaProcess* P ) : MetaFloat( P )
{
   TheIIMinWeightParameter = this;
}

IsoString IIMinWeight::Id() const
{
   return "minWeight";
}

int IIMinWeight::Precision() const
{
   return 6;
}

double IIMinWeight::DefaultValue() const
{
   return 0.005;
}

double IIMinWeight::MinimumValue() const
{
   return 0;
}

double IIMinWeight::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

IICSVWeights::IICSVWeights( MetaProcess* P ) : MetaString( P )
{
   TheIICSVWeightsParameter = this;
}

IsoString IICSVWeights::Id() const
{
   return "csvWeights";
}

// ----------------------------------------------------------------------------

IIAdaptiveGridSize::IIAdaptiveGridSize( MetaProcess* P ) : MetaInt32( P )
{
   TheIIAdaptiveGridSizeParameter = this;
}

IsoString IIAdaptiveGridSize::Id() const
{
   return "adaptiveGridSize";
}

double IIAdaptiveGridSize::DefaultValue() const
{
   return 16;
}

double IIAdaptiveGridSize::MinimumValue() const
{
   return 2;
}

double IIAdaptiveGridSize::MaximumValue() const
{
   return 50;
}

// ----------------------------------------------------------------------------

IIAdaptiveNoScale::IIAdaptiveNoScale( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIAdaptiveNoScaleParameter = this;
}

IsoString IIAdaptiveNoScale::Id() const
{
   return "adaptiveNoScale";
}

bool IIAdaptiveNoScale::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IIIgnoreNoiseKeywords::IIIgnoreNoiseKeywords( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIIgnoreNoiseKeywordsParameter = this;
}

IsoString IIIgnoreNoiseKeywords::Id() const
{
   return "ignoreNoiseKeywords";
}

bool IIIgnoreNoiseKeywords::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IINormalization::IINormalization( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIINormalizationParameter = this;
}

IsoString IINormalization::Id() const
{
   return "normalization";
}

size_type IINormalization::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString IINormalization::ElementId( size_type i ) const
{
   switch ( i )
   {
   case NoNormalization:           return "NoNormalization";
   case Additive:                  return "Additive";
   case Multiplicative:            return "Multiplicative";
   default:
   case AdditiveWithScaling:       return "AdditiveWithScaling";
   case MultiplicativeWithScaling: return "MultiplicativeWithScaling";
   case LocalNormalization:        return "LocalNormalization";
   case AdaptiveNormalization:     return "AdaptiveNormalization";
   }
}

int IINormalization::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IINormalization::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

IIRejection::IIRejection( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIIRejectionParameter = this;
}

IsoString IIRejection::Id() const
{
   return "rejection";
}

size_type IIRejection::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString IIRejection::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case NoRejection:         return "NoRejection";
   case MinMax:              return "MinMax";
   case PercentileClip:      return "PercentileClip";
   case SigmaClip:           return "SigmaClip";
   case WinsorizedSigmaClip: return "WinsorizedSigmaClip";
   case AveragedSigmaClip:   return "AveragedSigmaClip";
   case LinearFit:           return "LinearFit";
   case CCDClip:             return "CCDClip"; // ### Deprecated
   case ESD:                 return "Rejection_ESD";
   case RCR:                 return "Rejection_RCR";
   }
}

int IIRejection::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IIRejection::DefaultValueIndex() const
{
   return size_type( Default );
}

String IIRejection::ElementLabel( int index )
{
   switch ( index )
   {
   default:
   case NoRejection:         return "No rejection";
   case MinMax:              return "Min/Max";
   case PercentileClip:      return "Percentile Clipping";
   case SigmaClip:           return "Sigma Clipping";
   case WinsorizedSigmaClip: return "Winsorized Sigma Clipping";
   case AveragedSigmaClip:   return "Averaged Sigma Clipping";
   case LinearFit:           return "Linear Fit Clipping";
   case CCDClip:             return "CCD Noise Model"; // ### Deprecated
   case ESD:                 return "Generalized Extreme Studentized Deviate (ESD)";
   case RCR:                 return "Robust Chauvenet Rejection (RCR)";
   }
}

// ----------------------------------------------------------------------------

IIRejectionNormalization::IIRejectionNormalization( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIIRejectionNormalizationParameter = this;
}

IsoString IIRejectionNormalization::Id() const
{
   return "rejectionNormalization";
}

size_type IIRejectionNormalization::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString IIRejectionNormalization::ElementId( size_type i ) const
{
   switch ( i )
   {
   case NoRejectionNormalization:       return "NoRejectionNormalization";
   default:
   case Scale:                          return "Scale";
   case EqualizeFluxes:                 return "EqualizeFluxes";
   case LocalRejectionNormalization:    return "LocalRejectionNormalization";
   case AdaptiveRejectionNormalization: return "AdaptiveRejectionNormalization";
   }
}

int IIRejectionNormalization::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IIRejectionNormalization::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

IIMinMaxLow::IIMinMaxLow( MetaProcess* P ) : MetaInt32( P )
{
   TheIIMinMaxLowParameter = this;
}

IsoString IIMinMaxLow::Id() const
{
   return "minMaxLow";
}

double IIMinMaxLow::DefaultValue() const
{
   return 1;
}

double IIMinMaxLow::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

IIMinMaxHigh::IIMinMaxHigh( MetaProcess* P ) : MetaInt32( P )
{
   TheIIMinMaxHighParameter = this;
}

IsoString IIMinMaxHigh::Id() const
{
   return "minMaxHigh";
}

double IIMinMaxHigh::DefaultValue() const
{
   return 1;
}

double IIMinMaxHigh::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

IIPercentileLow::IIPercentileLow( MetaProcess* P ) : MetaFloat( P )
{
   TheIIPercentileLowParameter = this;
}

IsoString IIPercentileLow::Id() const
{
   return "pcClipLow";
}

int IIPercentileLow::Precision() const
{
   return 3;
}

double IIPercentileLow::DefaultValue() const
{
   return 0.2;
}

double IIPercentileLow::MinimumValue() const
{
   return 0;
}

double IIPercentileLow::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

IIPercentileHigh::IIPercentileHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheIIPercentileHighParameter = this;
}

IsoString IIPercentileHigh::Id() const
{
   return "pcClipHigh";
}

int IIPercentileHigh::Precision() const
{
   return 3;
}

double IIPercentileHigh::DefaultValue() const
{
   return 0.1;
}

double IIPercentileHigh::MinimumValue() const
{
   return 0;
}

double IIPercentileHigh::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

IISigmaLow::IISigmaLow( MetaProcess* P ) : MetaFloat( P )
{
   TheIISigmaLowParameter = this;
}

IsoString IISigmaLow::Id() const
{
   return "sigmaLow";
}

int IISigmaLow::Precision() const
{
   return 3;
}

double IISigmaLow::DefaultValue() const
{
   return 4;
}

double IISigmaLow::MinimumValue() const
{
   return 0;
}

double IISigmaLow::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

IISigmaHigh::IISigmaHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheIISigmaHighParameter = this;
}

IsoString IISigmaHigh::Id() const
{
   return "sigmaHigh";
}

int IISigmaHigh::Precision() const
{
   return 3;
}

double IISigmaHigh::DefaultValue() const
{
   return 3;
}

double IISigmaHigh::MinimumValue() const
{
   return 0;
}

double IISigmaHigh::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

IIWinsorizationCutoff::IIWinsorizationCutoff( MetaProcess* P ) : MetaFloat( P )
{
   TheIIWinsorizationCutoffParameter = this;
}

IsoString IIWinsorizationCutoff::Id() const
{
   return "winsorizationCutoff";
}

int IIWinsorizationCutoff::Precision() const
{
   return 3;
}

double IIWinsorizationCutoff::DefaultValue() const
{
   return 5;
}

double IIWinsorizationCutoff::MinimumValue() const
{
   return 3;
}

double IIWinsorizationCutoff::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

IILinearFitLow::IILinearFitLow( MetaProcess* P ) : MetaFloat( P )
{
   TheIILinearFitLowParameter = this;
}

IsoString IILinearFitLow::Id() const
{
   return "linearFitLow";
}

int IILinearFitLow::Precision() const
{
   return 3;
}

double IILinearFitLow::DefaultValue() const
{
   return 5;
}

double IILinearFitLow::MinimumValue() const
{
   return 0;
}

double IILinearFitLow::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

IILinearFitHigh::IILinearFitHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheIILinearFitHighParameter = this;
}

IsoString IILinearFitHigh::Id() const
{
   return "linearFitHigh";
}

int IILinearFitHigh::Precision() const
{
   return 3;
}

double IILinearFitHigh::DefaultValue() const
{
   return 4;
}

double IILinearFitHigh::MinimumValue() const
{
   return 0;
}

double IILinearFitHigh::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

IIESDOutliersFraction::IIESDOutliersFraction( MetaProcess* P ) : MetaFloat( P )
{
   TheIIESDOutliersFractionParameter = this;
}

IsoString IIESDOutliersFraction::Id() const
{
   return "esdOutliersFraction";
}

int IIESDOutliersFraction::Precision() const
{
   return 2;
}

double IIESDOutliersFraction::DefaultValue() const
{
   return 0.30;
}

double IIESDOutliersFraction::MinimumValue() const
{
   return 0;
}

double IIESDOutliersFraction::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

IIESDAlpha::IIESDAlpha( MetaProcess* P ) : MetaFloat( P )
{
   TheIIESDAlphaParameter = this;
}

IsoString IIESDAlpha::Id() const
{
   return "esdAlpha";
}

int IIESDAlpha::Precision() const
{
   return 2;
}

double IIESDAlpha::DefaultValue() const
{
   return 0.05;
}

double IIESDAlpha::MinimumValue() const
{
   return 0;
}

double IIESDAlpha::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

IIESDLowRelaxation::IIESDLowRelaxation( MetaProcess* P ) : MetaFloat( P )
{
   TheIIESDLowRelaxationParameter = this;
}

IsoString IIESDLowRelaxation::Id() const
{
   return "esdLowRelaxation";
}

int IIESDLowRelaxation::Precision() const
{
   return 2;
}

double IIESDLowRelaxation::DefaultValue() const
{
   return 1.0;
}

double IIESDLowRelaxation::MinimumValue() const
{
   return 1.0;
}

double IIESDLowRelaxation::MaximumValue() const
{
   return 5.0;
}

// ----------------------------------------------------------------------------

IIRCRLimit::IIRCRLimit( MetaProcess* P ) : MetaFloat( P )
{
   TheIIRCRLimitParameter = this;
}

IsoString IIRCRLimit::Id() const
{
   return "rcrLimit";
}

int IIRCRLimit::Precision() const
{
   return 2;
}

double IIRCRLimit::DefaultValue() const
{
   return 0.1;
}

double IIRCRLimit::MinimumValue() const
{
   return 0;
}

double IIRCRLimit::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

IICCDGain::IICCDGain( MetaProcess* P ) : MetaFloat( P )
{
   TheIICCDGainParameter = this;
}

IsoString IICCDGain::Id() const
{
   return "ccdGain";
}

int IICCDGain::Precision() const
{
   return 2;
}

double IICCDGain::DefaultValue() const
{
   return 1;
}

double IICCDGain::MinimumValue() const
{
   return 0.1;
}

double IICCDGain::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

IICCDReadNoise::IICCDReadNoise( MetaProcess* P ) : MetaFloat( P )
{
   TheIICCDReadNoiseParameter = this;
}

IsoString IICCDReadNoise::Id() const
{
   return "ccdReadNoise";
}

int IICCDReadNoise::Precision() const
{
   return 2;
}

double IICCDReadNoise::DefaultValue() const
{
   return 10;
}

double IICCDReadNoise::MinimumValue() const
{
   return 0;
}

double IICCDReadNoise::MaximumValue() const
{
   return 20;
}

// ----------------------------------------------------------------------------

IICCDScaleNoise::IICCDScaleNoise( MetaProcess* P ) : MetaFloat( P )
{
   TheIICCDScaleNoiseParameter = this;
}

IsoString IICCDScaleNoise::Id() const
{
   return "ccdScaleNoise";
}

int IICCDScaleNoise::Precision() const
{
   return 2;
}

double IICCDScaleNoise::DefaultValue() const
{
   return 0;
}

double IICCDScaleNoise::MinimumValue() const
{
   return 0;
}

double IICCDScaleNoise::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

IIClipLow::IIClipLow( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIClipLowParameter = this;
}

IsoString IIClipLow::Id() const
{
   return "clipLow";
}

bool IIClipLow::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIClipHigh::IIClipHigh( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIClipHighParameter = this;
}

IsoString IIClipHigh::Id() const
{
   return "clipHigh";
}

bool IIClipHigh::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIRangeClipLow::IIRangeClipLow( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIRangeClipLowParameter = this;
}

IsoString IIRangeClipLow::Id() const
{
   return "rangeClipLow";
}

bool IIRangeClipLow::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIRangeLow::IIRangeLow( MetaProcess* P ) : MetaFloat( P )
{
   TheIIRangeLowParameter = this;
}

IsoString IIRangeLow::Id() const
{
   return "rangeLow";
}

int IIRangeLow::Precision() const
{
   return 6;
}

double IIRangeLow::DefaultValue() const
{
   return 0.0;
}

double IIRangeLow::MinimumValue() const
{
   return 0.0;
}

double IIRangeLow::MaximumValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

IIRangeClipHigh::IIRangeClipHigh( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIRangeClipHighParameter = this;
}

IsoString IIRangeClipHigh::Id() const
{
   return "rangeClipHigh";
}

bool IIRangeClipHigh::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IIRangeHigh::IIRangeHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheIIRangeHighParameter = this;
}

IsoString IIRangeHigh::Id() const
{
   return "rangeHigh";
}

int IIRangeHigh::Precision() const
{
   return 6;
}

double IIRangeHigh::DefaultValue() const
{
   return 0.98;
}

double IIRangeHigh::MinimumValue() const
{
   return 0.5;
}

double IIRangeHigh::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

IIMapRangeRejection::IIMapRangeRejection( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIMapRangeRejectionParameter = this;
}

IsoString IIMapRangeRejection::Id() const
{
   return "mapRangeRejection";
}

bool IIMapRangeRejection::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIReportRangeRejection::IIReportRangeRejection( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIReportRangeRejectionParameter = this;
}

IsoString IIReportRangeRejection::Id() const
{
   return "reportRangeRejection";
}

bool IIReportRangeRejection::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IILargeScaleClipLow::IILargeScaleClipLow( MetaProcess* P ) : MetaBoolean( P )
{
   TheIILargeScaleClipLowParameter = this;
}

IsoString IILargeScaleClipLow::Id() const
{
   return "largeScaleClipLow";
}

bool IILargeScaleClipLow::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IILargeScaleClipLowProtectedLayers::IILargeScaleClipLowProtectedLayers( MetaProcess* P ) : MetaInt32( P )
{
   TheIILargeScaleClipLowProtectedLayersParameter = this;
}

IsoString IILargeScaleClipLowProtectedLayers::Id() const
{
   return "largeScaleClipLowProtectedLayers";
}

double IILargeScaleClipLowProtectedLayers::DefaultValue() const
{
   return 2;
}

double IILargeScaleClipLowProtectedLayers::MinimumValue() const
{
   return 1;
}

double IILargeScaleClipLowProtectedLayers::MaximumValue() const
{
   return 6;
}

// ----------------------------------------------------------------------------

IILargeScaleClipLowGrowth::IILargeScaleClipLowGrowth( MetaProcess* P ) : MetaInt32( P )
{
   TheIILargeScaleClipLowGrowthParameter = this;
}

IsoString IILargeScaleClipLowGrowth::Id() const
{
   return "largeScaleClipLowGrowth";
}

double IILargeScaleClipLowGrowth::DefaultValue() const
{
   return 2;
}

double IILargeScaleClipLowGrowth::MinimumValue() const
{
   return 1;
}

double IILargeScaleClipLowGrowth::MaximumValue() const
{
   return 20;
}

// ----------------------------------------------------------------------------

IILargeScaleClipHigh::IILargeScaleClipHigh( MetaProcess* P ) : MetaBoolean( P )
{
   TheIILargeScaleClipHighParameter = this;
}

IsoString IILargeScaleClipHigh::Id() const
{
   return "largeScaleClipHigh";
}

bool IILargeScaleClipHigh::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IILargeScaleClipHighProtectedLayers::IILargeScaleClipHighProtectedLayers( MetaProcess* P ) : MetaInt32( P )
{
   TheIILargeScaleClipHighProtectedLayersParameter = this;
}

IsoString IILargeScaleClipHighProtectedLayers::Id() const
{
   return "largeScaleClipHighProtectedLayers";
}

double IILargeScaleClipHighProtectedLayers::DefaultValue() const
{
   return 2;
}

double IILargeScaleClipHighProtectedLayers::MinimumValue() const
{
   return 1;
}

double IILargeScaleClipHighProtectedLayers::MaximumValue() const
{
   return 6;
}

// ----------------------------------------------------------------------------

IILargeScaleClipHighGrowth::IILargeScaleClipHighGrowth( MetaProcess* P ) : MetaInt32( P )
{
   TheIILargeScaleClipHighGrowthParameter = this;
}

IsoString IILargeScaleClipHighGrowth::Id() const
{
   return "largeScaleClipHighGrowth";
}

double IILargeScaleClipHighGrowth::DefaultValue() const
{
   return 2;
}

double IILargeScaleClipHighGrowth::MinimumValue() const
{
   return 1;
}

double IILargeScaleClipHighGrowth::MaximumValue() const
{
   return 20;
}

// ----------------------------------------------------------------------------

IIGenerate64BitResult::IIGenerate64BitResult( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIGenerate64BitResultParameter = this;
}

IsoString IIGenerate64BitResult::Id() const
{
   return "generate64BitResult";
}

bool IIGenerate64BitResult::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IIGenerateRejectionMaps::IIGenerateRejectionMaps( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIGenerateRejectionMapsParameter = this;
}

IsoString IIGenerateRejectionMaps::Id() const
{
   return "generateRejectionMaps";
}

bool IIGenerateRejectionMaps::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIGenerateIntegratedImage::IIGenerateIntegratedImage( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIGenerateIntegratedImageParameter = this;
}

IsoString IIGenerateIntegratedImage::Id() const
{
   return "generateIntegratedImage";
}

bool IIGenerateIntegratedImage::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIGenerateDrizzleData::IIGenerateDrizzleData( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIGenerateDrizzleDataParameter = this;
}

IsoString IIGenerateDrizzleData::Id() const
{
   return "generateDrizzleData";
}

bool IIGenerateDrizzleData::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IIClosePreviousImages::IIClosePreviousImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIClosePreviousImagesParameter = this;
}

IsoString IIClosePreviousImages::Id() const
{
   return "closePreviousImages";
}

bool IIClosePreviousImages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IIBufferSize::IIBufferSize( MetaProcess* P ) : MetaInt32( P )
{
   TheIIBufferSizeParameter = this;
}

IsoString IIBufferSize::Id() const
{
   return "bufferSizeMB";
}

double IIBufferSize::DefaultValue() const
{
   return 16;
}

double IIBufferSize::MinimumValue() const
{
   return 0;   // zero means use a one-row buffer
}

double IIBufferSize::MaximumValue() const
{
   return 1024;   // 1 GB
}

// ----------------------------------------------------------------------------

IIStackSize::IIStackSize( MetaProcess* P ) : MetaInt32( P )
{
   TheIIStackSizeParameter = this;
}

IsoString IIStackSize::Id() const
{
   return "stackSizeMB";
}

double IIStackSize::DefaultValue() const
{
   return 1024; // 1 GB
}

double IIStackSize::MinimumValue() const
{
   return 0;   // zero means use a one-row stack
}

double IIStackSize::MaximumValue() const
{
   return 1024*1024;   // 1 TB
}

// ----------------------------------------------------------------------------

IIAutoMemorySize::IIAutoMemorySize( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIAutoMemorySizeParameter = this;
}

IsoString IIAutoMemorySize::Id() const
{
   return "autoMemorySize";
}

bool IIAutoMemorySize::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIAutoMemoryLimit::IIAutoMemoryLimit( MetaProcess* P ) : MetaFloat( P )
{
   TheIIAutoMemoryLimitParameter = this;
}

IsoString IIAutoMemoryLimit::Id() const
{
   return "autoMemoryLimit";
}

int IIAutoMemoryLimit::Precision() const
{
   return 2;
}

double IIAutoMemoryLimit::DefaultValue() const
{
   return 0.75;
}

double IIAutoMemoryLimit::MinimumValue() const
{
   return 0.10;
}

double IIAutoMemoryLimit::MaximumValue() const
{
   return 1.00;
}

// ----------------------------------------------------------------------------

IIUseROI::IIUseROI( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIUseROIParameter = this;
}

IsoString IIUseROI::Id() const
{
   return "useROI";
}

bool IIUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IIROIX0::IIROIX0( MetaProcess* P ) : MetaInt32( P )
{
   TheIIROIX0Parameter = this;
}

IsoString IIROIX0::Id() const
{
   return "roiX0";
}

double IIROIX0::DefaultValue() const
{
   return 0;
}

double IIROIX0::MinimumValue() const
{
   return 0;
}

double IIROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

IIROIY0::IIROIY0( MetaProcess* P ) : MetaInt32( P )
{
   TheIIROIY0Parameter = this;
}

IsoString IIROIY0::Id() const
{
   return "roiY0";
}

double IIROIY0::DefaultValue() const
{
   return 0;
}

double IIROIY0::MinimumValue() const
{
   return 0;
}

double IIROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

IIROIX1::IIROIX1( MetaProcess* P ) : MetaInt32( P )
{
   TheIIROIX1Parameter = this;
}

IsoString IIROIX1::Id() const
{
   return "roiX1";
}

double IIROIX1::DefaultValue() const
{
   return 0;
}

double IIROIX1::MinimumValue() const
{
   return 0;
}

double IIROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

IIROIY1::IIROIY1( MetaProcess* P ) : MetaInt32( P )
{
   TheIIROIY1Parameter = this;
}

IsoString IIROIY1::Id() const
{
   return "roiY1";
}

double IIROIY1::DefaultValue() const
{
   return 0;
}

double IIROIY1::MinimumValue() const
{
   return 0;
}

double IIROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

IIUseCache::IIUseCache( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIUseCacheParameter = this;
}

IsoString IIUseCache::Id() const
{
   return "useCache";
}

bool IIUseCache::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIEvaluateSNR::IIEvaluateSNR( MetaProcess* P ) : MetaBoolean( P )
{
   TheIIEvaluateSNRParameter = this;
}

IsoString IIEvaluateSNR::Id() const
{
   return "evaluateSNR";
}

IsoString IIEvaluateSNR::Aliases() const
{
   return "evaluateNoise"; // compatibility with module versions < 1.4.2
}

bool IIEvaluateSNR::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IINoiseEvaluationAlgorithm::IINoiseEvaluationAlgorithm( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIINoiseEvaluationAlgorithmParameter = this;
}

IsoString IINoiseEvaluationAlgorithm::Id() const
{
   return "noiseEvaluationAlgorithm";
}

size_type IINoiseEvaluationAlgorithm::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString IINoiseEvaluationAlgorithm::ElementId( size_type i ) const
{
   switch ( i )
   {
   case KSigma: return "NoiseEvaluation_KSigma";
   default:
   case MRS:    return "NoiseEvaluation_MRS";
   case NStar:  return "NoiseEvaluation_NStar";
   }
}

int IINoiseEvaluationAlgorithm::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IINoiseEvaluationAlgorithm::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

IIMRSMinDataFraction::IIMRSMinDataFraction( MetaProcess* P ) : MetaFloat( P )
{
   TheIIMRSMinDataFractionParameter = this;
}

IsoString IIMRSMinDataFraction::Id() const
{
   return "mrsMinDataFraction";
}

int IIMRSMinDataFraction::Precision() const
{
   return 3;
}

double IIMRSMinDataFraction::DefaultValue() const
{
   return 0.01;
}

double IIMRSMinDataFraction::MinimumValue() const
{
   return 0;
}

double IIMRSMinDataFraction::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

IIPSFStructureLayers::IIPSFStructureLayers( MetaProcess* p ) : MetaInt32( p )
{
   TheIIPSFStructureLayersParameter = this;
}

IsoString IIPSFStructureLayers::Id() const
{
   return "psfStructureLayers";
}

double IIPSFStructureLayers::DefaultValue() const
{
   return 5;
}

double IIPSFStructureLayers::MinimumValue() const
{
   return 1;
}

double IIPSFStructureLayers::MaximumValue() const
{
   return 8;
}

// ----------------------------------------------------------------------------

IIPSFType::IIPSFType( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIIPSFTypeParameter = this;
}

IsoString IIPSFType::Id() const
{
   return "psfType";
}

size_type IIPSFType::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString IIPSFType::ElementId( size_type i ) const
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

IsoString IIPSFType::FunctionName( pcl_enum i )
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

int IIPSFType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IIPSFType::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

IISubtractPedestals::IISubtractPedestals( MetaProcess* p ) : MetaBoolean( p )
{
   TheIISubtractPedestalsParameter = this;
}

IsoString IISubtractPedestals::Id() const
{
   return "subtractPedestals";
}

bool IISubtractPedestals::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IITruncateOnOutOfRange::IITruncateOnOutOfRange( MetaProcess* p ) : MetaBoolean( p )
{
   TheIITruncateOnOutOfRangeParameter = this;
}

IsoString IITruncateOnOutOfRange::Id() const
{
   return "truncateOnOutOfRange";
}

bool IITruncateOnOutOfRange::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

// ### DEPRECATED
IINoGUIMessages::IINoGUIMessages( MetaProcess* p ) : MetaBoolean( p )
{
   TheIINoGUIMessagesParameter = this;
}

IsoString IINoGUIMessages::Id() const
{
   return "noGUIMessages";
}

bool IINoGUIMessages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIShowImages::IIShowImages( MetaProcess* p ) : MetaBoolean( p )
{
   TheIIShowImagesParameter = this;
}

IsoString IIShowImages::Id() const
{
   return "showImages";
}

bool IIShowImages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIUseFileThreads::IIUseFileThreads( MetaProcess* p ) : MetaBoolean( p )
{
   TheIIUseFileThreadsParameter = this;
}

IsoString IIUseFileThreads::Id() const
{
   return "useFileThreads";
}

bool IIUseFileThreads::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFileThreadOverload::IIFileThreadOverload( MetaProcess* p ) : MetaFloat( p )
{
   TheIIFileThreadOverloadParameter = this;
}

IsoString IIFileThreadOverload::Id() const
{
   return "fileThreadOverload";
}

int IIFileThreadOverload::Precision() const
{
   return 2;
}

double IIFileThreadOverload::DefaultValue() const
{
   return 1.0;
}

double IIFileThreadOverload::MinimumValue() const
{
   return 1;
}

double IIFileThreadOverload::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

IIUseBufferThreads::IIUseBufferThreads( MetaProcess* p ) : MetaBoolean( p )
{
   TheIIUseBufferThreadsParameter = this;
}

IsoString IIUseBufferThreads::Id() const
{
   return "useBufferThreads";
}

bool IIUseBufferThreads::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIMaxBufferThreads::IIMaxBufferThreads( MetaProcess* P ) : MetaInt32( P )
{
   TheIIMaxBufferThreadsParameter = this;
}

IsoString IIMaxBufferThreads::Id() const
{
   return "maxBufferThreads";
}

double IIMaxBufferThreads::DefaultValue() const
{
   return 0;
}

double IIMaxBufferThreads::MinimumValue() const
{
   return 0;
}

double IIMaxBufferThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Output properties
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

IIIntegrationImageId::IIIntegrationImageId( MetaProcess* P ) : MetaString( P )
{
   TheIIIntegrationImageIdParameter = this;
}

IsoString IIIntegrationImageId::Id() const
{
   return "integrationImageId";
}

bool IIIntegrationImageId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IILowRejectionMapImageId::IILowRejectionMapImageId( MetaProcess* P ) : MetaString( P )
{
   TheIILowRejectionMapImageIdParameter = this;
}

IsoString IILowRejectionMapImageId::Id() const
{
   return "lowRejectionMapImageId";
}

bool IILowRejectionMapImageId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIHighRejectionMapImageId::IIHighRejectionMapImageId( MetaProcess* P ) : MetaString( P )
{
   TheIIHighRejectionMapImageIdParameter = this;
}

IsoString IIHighRejectionMapImageId::Id() const
{
   return "highRejectionMapImageId";
}

bool IIHighRejectionMapImageId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IISlopeMapImageId::IISlopeMapImageId( MetaProcess* P ) : MetaString( P )
{
   TheIISlopeMapImageIdParameter = this;
}

IsoString IISlopeMapImageId::Id() const
{
   return "slopeMapImageId";
}

bool IISlopeMapImageId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IINumberOfChannels::IINumberOfChannels( MetaProcess* P ) : MetaInt32( P )
{
   TheIINumberOfChannelsParameter = this;
}

IsoString IINumberOfChannels::Id() const
{
   return "numberOfChannels";
}

bool IINumberOfChannels::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IINumberOfPixels::IINumberOfPixels( MetaProcess* P ) : MetaUInt64( P )
{
   TheIINumberOfPixelsParameter = this;
}

IsoString IINumberOfPixels::Id() const
{
   return "numberOfPixels";
}

bool IINumberOfPixels::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IITotalPixels::IITotalPixels( MetaProcess* P ) : MetaUInt64( P )
{
   TheIITotalPixelsParameter = this;
}

IsoString IITotalPixels::Id() const
{
   return "totalPixels";
}

bool IITotalPixels::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIOutputRangeLow::IIOutputRangeLow( MetaProcess* P ) : MetaDouble( P )
{
   TheIIOutputRangeLowParameter = this;
}

IsoString IIOutputRangeLow::Id() const
{
   return "outputRangeLow";
}

int IIOutputRangeLow::Precision() const
{
   return -15;
}

bool IIOutputRangeLow::ScientificNotation() const
{
   return true;
}

bool IIOutputRangeLow::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIOutputRangeHigh::IIOutputRangeHigh( MetaProcess* P ) : MetaDouble( P )
{
   TheIIOutputRangeHighParameter = this;
}

IsoString IIOutputRangeHigh::Id() const
{
   return "outputRangeHigh";
}

int IIOutputRangeHigh::Precision() const
{
   return -15;
}

bool IIOutputRangeHigh::ScientificNotation() const
{
   return true;
}

bool IIOutputRangeHigh::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IITotalRejectedLowRK::IITotalRejectedLowRK( MetaProcess* P ) : MetaUInt64( P )
{
   TheIITotalRejectedLowRKParameter = this;
}

IsoString IITotalRejectedLowRK::Id() const
{
   return "totalRejectedLowRK";
}

bool IITotalRejectedLowRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IITotalRejectedLowG::IITotalRejectedLowG( MetaProcess* P ) : MetaUInt64( P )
{
   TheIITotalRejectedLowGParameter = this;
}

IsoString IITotalRejectedLowG::Id() const
{
   return "totalRejectedLowG";
}

bool IITotalRejectedLowG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IITotalRejectedLowB::IITotalRejectedLowB( MetaProcess* P ) : MetaUInt64( P )
{
   TheIITotalRejectedLowBParameter = this;
}

IsoString IITotalRejectedLowB::Id() const
{
   return "totalRejectedLowB";
}

bool IITotalRejectedLowB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IITotalRejectedHighRK::IITotalRejectedHighRK( MetaProcess* P ) : MetaUInt64( P )
{
   TheIITotalRejectedHighRKParameter = this;
}

IsoString IITotalRejectedHighRK::Id() const
{
   return "totalRejectedHighRK";
}

bool IITotalRejectedHighRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IITotalRejectedHighG::IITotalRejectedHighG( MetaProcess* P ) : MetaUInt64( P )
{
   TheIITotalRejectedHighGParameter = this;
}

IsoString IITotalRejectedHighG::Id() const
{
   return "totalRejectedHighG";
}

bool IITotalRejectedHighG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IITotalRejectedHighB::IITotalRejectedHighB( MetaProcess* P ) : MetaUInt64( P )
{
   TheIITotalRejectedHighBParameter = this;
}

IsoString IITotalRejectedHighB::Id() const
{
   return "totalRejectedHighB";
}

bool IITotalRejectedHighB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseEstimateRK::IIFinalNoiseEstimateRK( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalNoiseEstimateRKParameter = this;
}

IsoString IIFinalNoiseEstimateRK::Id() const
{
   return "finalNoiseEstimateRK";
}

int IIFinalNoiseEstimateRK::Precision() const
{
   return 3;
}

bool IIFinalNoiseEstimateRK::ScientificNotation() const
{
   return true;
}

bool IIFinalNoiseEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseEstimateG::IIFinalNoiseEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalNoiseEstimateGParameter = this;
}

IsoString IIFinalNoiseEstimateG::Id() const
{
   return "finalNoiseEstimateG";
}

int IIFinalNoiseEstimateG::Precision() const
{
   return 3;
}

bool IIFinalNoiseEstimateG::ScientificNotation() const
{
   return true;
}

bool IIFinalNoiseEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseEstimateB::IIFinalNoiseEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalNoiseEstimateBParameter = this;
}

IsoString IIFinalNoiseEstimateB::Id() const
{
   return "finalNoiseEstimateB";
}

int IIFinalNoiseEstimateB::Precision() const
{
   return 3;
}

bool IIFinalNoiseEstimateB::ScientificNotation() const
{
   return true;
}

bool IIFinalNoiseEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseScaleEstimateLowRK::IIFinalNoiseScaleEstimateLowRK( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalNoiseScaleEstimateLowRKParameter = this;
}

IsoString IIFinalNoiseScaleEstimateLowRK::Id() const
{
   return "finalNoiseScaleEstimateLowRK";
}

int IIFinalNoiseScaleEstimateLowRK::Precision() const
{
   return 6;
}

bool IIFinalNoiseScaleEstimateLowRK::ScientificNotation() const
{
   return true;
}

bool IIFinalNoiseScaleEstimateLowRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseScaleEstimateLowG::IIFinalNoiseScaleEstimateLowG( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalNoiseScaleEstimateLowGParameter = this;
}

IsoString IIFinalNoiseScaleEstimateLowG::Id() const
{
   return "finalNoiseScaleEstimateLowG";
}

int IIFinalNoiseScaleEstimateLowG::Precision() const
{
   return 6;
}

bool IIFinalNoiseScaleEstimateLowG::ScientificNotation() const
{
   return true;
}

bool IIFinalNoiseScaleEstimateLowG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseScaleEstimateLowB::IIFinalNoiseScaleEstimateLowB( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalNoiseScaleEstimateLowBParameter = this;
}

IsoString IIFinalNoiseScaleEstimateLowB::Id() const
{
   return "finalNoiseScaleEstimateLowB";
}

int IIFinalNoiseScaleEstimateLowB::Precision() const
{
   return 6;
}

bool IIFinalNoiseScaleEstimateLowB::ScientificNotation() const
{
   return true;
}

bool IIFinalNoiseScaleEstimateLowB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseScaleEstimateHighRK::IIFinalNoiseScaleEstimateHighRK( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalNoiseScaleEstimateHighRKParameter = this;
}

IsoString IIFinalNoiseScaleEstimateHighRK::Id() const
{
   return "finalNoiseScaleEstimateHighRK";
}

int IIFinalNoiseScaleEstimateHighRK::Precision() const
{
   return 6;
}

bool IIFinalNoiseScaleEstimateHighRK::ScientificNotation() const
{
   return true;
}

bool IIFinalNoiseScaleEstimateHighRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseScaleEstimateHighG::IIFinalNoiseScaleEstimateHighG( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalNoiseScaleEstimateHighGParameter = this;
}

IsoString IIFinalNoiseScaleEstimateHighG::Id() const
{
   return "finalNoiseScaleEstimateHighG";
}

int IIFinalNoiseScaleEstimateHighG::Precision() const
{
   return 6;
}

bool IIFinalNoiseScaleEstimateHighG::ScientificNotation() const
{
   return true;
}

bool IIFinalNoiseScaleEstimateHighG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseScaleEstimateHighB::IIFinalNoiseScaleEstimateHighB( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalNoiseScaleEstimateHighBParameter = this;
}

IsoString IIFinalNoiseScaleEstimateHighB::Id() const
{
   return "finalNoiseScaleEstimateHighB";
}

int IIFinalNoiseScaleEstimateHighB::Precision() const
{
   return 6;
}

bool IIFinalNoiseScaleEstimateHighB::ScientificNotation() const
{
   return true;
}

bool IIFinalNoiseScaleEstimateHighB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseAlgorithmRK::IIFinalNoiseAlgorithmRK( MetaProcess* P ) : MetaString( P )
{
   TheIIFinalNoiseAlgorithmRKParameter = this;
}

IsoString IIFinalNoiseAlgorithmRK::Id() const
{
   return "finalNoiseAlgorithmRK";
}

bool IIFinalNoiseAlgorithmRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseAlgorithmG::IIFinalNoiseAlgorithmG( MetaProcess* P ) : MetaString( P )
{
   TheIIFinalNoiseAlgorithmGParameter = this;
}

IsoString IIFinalNoiseAlgorithmG::Id() const
{
   return "finalNoiseAlgorithmG";
}

bool IIFinalNoiseAlgorithmG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalNoiseAlgorithmB::IIFinalNoiseAlgorithmB( MetaProcess* P ) : MetaString( P )
{
   TheIIFinalNoiseAlgorithmBParameter = this;
}

IsoString IIFinalNoiseAlgorithmB::Id() const
{
   return "finalNoiseAlgorithmB";
}

bool IIFinalNoiseAlgorithmB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalScaleEstimateRK::IIFinalScaleEstimateRK( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalScaleEstimateRKParameter = this;
}

IsoString IIFinalScaleEstimateRK::Id() const
{
   return "finalScaleEstimateRK";
}

int IIFinalScaleEstimateRK::Precision() const
{
   return 4;
}

bool IIFinalScaleEstimateRK::ScientificNotation() const
{
   return true;
}

bool IIFinalScaleEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalScaleEstimateG::IIFinalScaleEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalScaleEstimateGParameter = this;
}

IsoString IIFinalScaleEstimateG::Id() const
{
   return "finalScaleEstimateG";
}

int IIFinalScaleEstimateG::Precision() const
{
   return 4;
}

bool IIFinalScaleEstimateG::ScientificNotation() const
{
   return true;
}

bool IIFinalScaleEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalScaleEstimateB::IIFinalScaleEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalScaleEstimateBParameter = this;
}

IsoString IIFinalScaleEstimateB::Id() const
{
   return "finalScaleEstimateB";
}

int IIFinalScaleEstimateB::Precision() const
{
   return 4;
}

bool IIFinalScaleEstimateB::ScientificNotation() const
{
   return true;
}

bool IIFinalScaleEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalLocationEstimateRK::IIFinalLocationEstimateRK( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalLocationEstimateRKParameter = this;
}

IsoString IIFinalLocationEstimateRK::Id() const
{
   return "finalLocationEstimateRK";
}

int IIFinalLocationEstimateRK::Precision() const
{
   return 4;
}

bool IIFinalLocationEstimateRK::ScientificNotation() const
{
   return true;
}

bool IIFinalLocationEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalLocationEstimateG::IIFinalLocationEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalLocationEstimateGParameter = this;
}

IsoString IIFinalLocationEstimateG::Id() const
{
   return "finalLocationEstimateG";
}

int IIFinalLocationEstimateG::Precision() const
{
   return 4;
}

bool IIFinalLocationEstimateG::ScientificNotation() const
{
   return true;
}

bool IIFinalLocationEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalLocationEstimateB::IIFinalLocationEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalLocationEstimateBParameter = this;
}

IsoString IIFinalLocationEstimateB::Id() const
{
   return "finalLocationEstimateB";
}

int IIFinalLocationEstimateB::Precision() const
{
   return 4;
}

bool IIFinalLocationEstimateB::ScientificNotation() const
{
   return true;
}

bool IIFinalLocationEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalFluxEstimateRK::IIFinalPSFTotalFluxEstimateRK( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalFluxEstimateRKParameter = this;
}

IsoString IIFinalPSFTotalFluxEstimateRK::Id() const
{
   return "finalPSFTotalFluxEstimateRK";
}

int IIFinalPSFTotalFluxEstimateRK::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalFluxEstimateRK::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalFluxEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalFluxEstimateG::IIFinalPSFTotalFluxEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalFluxEstimateGParameter = this;
}

IsoString IIFinalPSFTotalFluxEstimateG::Id() const
{
   return "finalPSFTotalFluxEstimateG";
}

int IIFinalPSFTotalFluxEstimateG::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalFluxEstimateB::IIFinalPSFTotalFluxEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalFluxEstimateBParameter = this;
}

IsoString IIFinalPSFTotalFluxEstimateB::Id() const
{
   return "finalPSFTotalFluxEstimateB";
}

int IIFinalPSFTotalFluxEstimateB::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalPowerFluxEstimateRK::IIFinalPSFTotalPowerFluxEstimateRK( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalPowerFluxEstimateRKParameter = this;
}

IsoString IIFinalPSFTotalPowerFluxEstimateRK::Id() const
{
   return "finalPSFTotalPowerFluxEstimateRK";
}

int IIFinalPSFTotalPowerFluxEstimateRK::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalPowerFluxEstimateRK::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalPowerFluxEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalPowerFluxEstimateG::IIFinalPSFTotalPowerFluxEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalPowerFluxEstimateGParameter = this;
}

IsoString IIFinalPSFTotalPowerFluxEstimateG::Id() const
{
   return "finalPSFTotalPowerFluxEstimateG";
}

int IIFinalPSFTotalPowerFluxEstimateG::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalPowerFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalPowerFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalPowerFluxEstimateB::IIFinalPSFTotalPowerFluxEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalPowerFluxEstimateBParameter = this;
}

IsoString IIFinalPSFTotalPowerFluxEstimateB::Id() const
{
   return "finalPSFTotalPowerFluxEstimateB";
}

int IIFinalPSFTotalPowerFluxEstimateB::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalPowerFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalPowerFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalMeanFluxEstimateRK::IIFinalPSFTotalMeanFluxEstimateRK( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalMeanFluxEstimateRKParameter = this;
}

IsoString IIFinalPSFTotalMeanFluxEstimateRK::Id() const
{
   return "finalPSFTotalMeanFluxEstimateRK";
}

int IIFinalPSFTotalMeanFluxEstimateRK::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalMeanFluxEstimateRK::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalMeanFluxEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalMeanFluxEstimateG::IIFinalPSFTotalMeanFluxEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalMeanFluxEstimateGParameter = this;
}

IsoString IIFinalPSFTotalMeanFluxEstimateG::Id() const
{
   return "finalPSFTotalMeanFluxEstimateG";
}

int IIFinalPSFTotalMeanFluxEstimateG::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalMeanFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalMeanFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalMeanFluxEstimateB::IIFinalPSFTotalMeanFluxEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalMeanFluxEstimateBParameter = this;
}

IsoString IIFinalPSFTotalMeanFluxEstimateB::Id() const
{
   return "finalPSFTotalMeanFluxEstimateB";
}

int IIFinalPSFTotalMeanFluxEstimateB::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalMeanFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalMeanFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalMeanPowerFluxEstimateRK::IIFinalPSFTotalMeanPowerFluxEstimateRK( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalMeanPowerFluxEstimateRKParameter = this;
}

IsoString IIFinalPSFTotalMeanPowerFluxEstimateRK::Id() const
{
   return "finalPSFTotalMeanPowerFluxEstimateRK";
}

int IIFinalPSFTotalMeanPowerFluxEstimateRK::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalMeanPowerFluxEstimateRK::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalMeanPowerFluxEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalMeanPowerFluxEstimateG::IIFinalPSFTotalMeanPowerFluxEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalMeanPowerFluxEstimateGParameter = this;
}

IsoString IIFinalPSFTotalMeanPowerFluxEstimateG::Id() const
{
   return "finalPSFTotalMeanPowerFluxEstimateG";
}

int IIFinalPSFTotalMeanPowerFluxEstimateG::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalMeanPowerFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalMeanPowerFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFTotalMeanPowerFluxEstimateB::IIFinalPSFTotalMeanPowerFluxEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFTotalMeanPowerFluxEstimateBParameter = this;
}

IsoString IIFinalPSFTotalMeanPowerFluxEstimateB::Id() const
{
   return "finalPSFTotalMeanPowerFluxEstimateB";
}

int IIFinalPSFTotalMeanPowerFluxEstimateB::Precision() const
{
   return 4;
}

bool IIFinalPSFTotalMeanPowerFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFTotalMeanPowerFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFMStarEstimateRK::IIFinalPSFMStarEstimateRK( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFMStarEstimateRKParameter = this;
}

IsoString IIFinalPSFMStarEstimateRK::Id() const
{
   return "finalPSFMStarEstimateRK";
}

int IIFinalPSFMStarEstimateRK::Precision() const
{
   return 4;
}

bool IIFinalPSFMStarEstimateRK::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFMStarEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFMStarEstimateG::IIFinalPSFMStarEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFMStarEstimateGParameter = this;
}

IsoString IIFinalPSFMStarEstimateG::Id() const
{
   return "finalPSFMStarEstimateG";
}

int IIFinalPSFMStarEstimateG::Precision() const
{
   return 4;
}

bool IIFinalPSFMStarEstimateG::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFMStarEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFMStarEstimateB::IIFinalPSFMStarEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFMStarEstimateBParameter = this;
}

IsoString IIFinalPSFMStarEstimateB::Id() const
{
   return "finalPSFMStarEstimateB";
}

int IIFinalPSFMStarEstimateB::Precision() const
{
   return 4;
}

bool IIFinalPSFMStarEstimateB::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFMStarEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFNStarEstimateRK::IIFinalPSFNStarEstimateRK( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFNStarEstimateRKParameter = this;
}

IsoString IIFinalPSFNStarEstimateRK::Id() const
{
   return "finalPSFNStarEstimateRK";
}

int IIFinalPSFNStarEstimateRK::Precision() const
{
   return 4;
}

bool IIFinalPSFNStarEstimateRK::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFNStarEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFNStarEstimateG::IIFinalPSFNStarEstimateG( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFNStarEstimateGParameter = this;
}

IsoString IIFinalPSFNStarEstimateG::Id() const
{
   return "finalPSFNStarEstimateG";
}

int IIFinalPSFNStarEstimateG::Precision() const
{
   return 4;
}

bool IIFinalPSFNStarEstimateG::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFNStarEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFNStarEstimateB::IIFinalPSFNStarEstimateB( MetaProcess* P ) : MetaDouble( P )
{
   TheIIFinalPSFNStarEstimateBParameter = this;
}

IsoString IIFinalPSFNStarEstimateB::Id() const
{
   return "finalPSFNStarEstimateB";
}

int IIFinalPSFNStarEstimateB::Precision() const
{
   return 4;
}

bool IIFinalPSFNStarEstimateB::ScientificNotation() const
{
   return true;
}

bool IIFinalPSFNStarEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFCountRK::IIFinalPSFCountRK( MetaProcess* P ) : MetaUInt32( P )
{
   TheIIFinalPSFCountRKParameter = this;
}

IsoString IIFinalPSFCountRK::Id() const
{
   return "finalPSFCountRK";
}

bool IIFinalPSFCountRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFCountG::IIFinalPSFCountG( MetaProcess* P ) : MetaUInt32( P )
{
   TheIIFinalPSFCountGParameter = this;
}

IsoString IIFinalPSFCountG::Id() const
{
   return "finalPSFCountG";
}

bool IIFinalPSFCountG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIFinalPSFCountB::IIFinalPSFCountB( MetaProcess* P ) : MetaUInt32( P )
{
   TheIIFinalPSFCountBParameter = this;
}

IsoString IIFinalPSFCountB::Id() const
{
   return "finalPSFCountB";
}

bool IIFinalPSFCountB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIReferenceNoiseReductionRK::IIReferenceNoiseReductionRK( MetaProcess* P ) : MetaFloat( P )
{
   TheIIReferenceNoiseReductionRKParameter = this;
}

IsoString IIReferenceNoiseReductionRK::Id() const
{
   return "referenceNoiseReductionRK";
}

int IIReferenceNoiseReductionRK::Precision() const
{
   return 4;
}

bool IIReferenceNoiseReductionRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIReferenceNoiseReductionG::IIReferenceNoiseReductionG( MetaProcess* P ) : MetaFloat( P )
{
   TheIIReferenceNoiseReductionGParameter = this;
}

IsoString IIReferenceNoiseReductionG::Id() const
{
   return "referenceNoiseReductionG";
}

int IIReferenceNoiseReductionG::Precision() const
{
   return 4;
}

bool IIReferenceNoiseReductionG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIReferenceNoiseReductionB::IIReferenceNoiseReductionB( MetaProcess* P ) : MetaFloat( P )
{
   TheIIReferenceNoiseReductionBParameter = this;
}

IsoString IIReferenceNoiseReductionB::Id() const
{
   return "referenceNoiseReductionB";
}

int IIReferenceNoiseReductionB::Precision() const
{
   return 4;
}

bool IIReferenceNoiseReductionB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIMedianNoiseReductionRK::IIMedianNoiseReductionRK( MetaProcess* P ) : MetaFloat( P )
{
   TheIIMedianNoiseReductionRKParameter = this;
}

IsoString IIMedianNoiseReductionRK::Id() const
{
   return "medianNoiseReductionRK";
}

int IIMedianNoiseReductionRK::Precision() const
{
   return 4;
}

bool IIMedianNoiseReductionRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIMedianNoiseReductionG::IIMedianNoiseReductionG( MetaProcess* P ) : MetaFloat( P )
{
   TheIIMedianNoiseReductionGParameter = this;
}

IsoString IIMedianNoiseReductionG::Id() const
{
   return "medianNoiseReductionG";
}

int IIMedianNoiseReductionG::Precision() const
{
   return 4;
}

bool IIMedianNoiseReductionG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIMedianNoiseReductionB::IIMedianNoiseReductionB( MetaProcess* P ) : MetaFloat( P )
{
   TheIIMedianNoiseReductionBParameter = this;
}

IsoString IIMedianNoiseReductionB::Id() const
{
   return "medianNoiseReductionB";
}

int IIMedianNoiseReductionB::Precision() const
{
   return 4;
}

bool IIMedianNoiseReductionB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIReferenceSNRIncrementRK::IIReferenceSNRIncrementRK( MetaProcess* P ) : MetaFloat( P )
{
   TheIIReferenceSNRIncrementRKParameter = this;
}

IsoString IIReferenceSNRIncrementRK::Id() const
{
   return "referenceSNRIncrementRK";
}

int IIReferenceSNRIncrementRK::Precision() const
{
   return 4;
}

bool IIReferenceSNRIncrementRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIReferenceSNRIncrementG::IIReferenceSNRIncrementG( MetaProcess* P ) : MetaFloat( P )
{
   TheIIReferenceSNRIncrementGParameter = this;
}

IsoString IIReferenceSNRIncrementG::Id() const
{
   return "referenceSNRIncrementG";
}

int IIReferenceSNRIncrementG::Precision() const
{
   return 4;
}

bool IIReferenceSNRIncrementG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIReferenceSNRIncrementB::IIReferenceSNRIncrementB( MetaProcess* P ) : MetaFloat( P )
{
   TheIIReferenceSNRIncrementBParameter = this;
}

IsoString IIReferenceSNRIncrementB::Id() const
{
   return "referenceSNRIncrementB";
}

int IIReferenceSNRIncrementB::Precision() const
{
   return 4;
}

bool IIReferenceSNRIncrementB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIAverageSNRIncrementRK::IIAverageSNRIncrementRK( MetaProcess* P ) : MetaFloat( P )
{
   TheIIAverageSNRIncrementRKParameter = this;
}

IsoString IIAverageSNRIncrementRK::Id() const
{
   return "averageSNRIncrementRK";
}

int IIAverageSNRIncrementRK::Precision() const
{
   return 4;
}

bool IIAverageSNRIncrementRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIAverageSNRIncrementG::IIAverageSNRIncrementG( MetaProcess* P ) : MetaFloat( P )
{
   TheIIAverageSNRIncrementGParameter = this;
}

IsoString IIAverageSNRIncrementG::Id() const
{
   return "averageSNRIncrementG";
}

int IIAverageSNRIncrementG::Precision() const
{
   return 4;
}

bool IIAverageSNRIncrementG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIAverageSNRIncrementB::IIAverageSNRIncrementB( MetaProcess* P ) : MetaFloat( P )
{
   TheIIAverageSNRIncrementBParameter = this;
}

IsoString IIAverageSNRIncrementB::Id() const
{
   return "averageSNRIncrementB";
}

int IIAverageSNRIncrementB::Precision() const
{
   return 4;
}

bool IIAverageSNRIncrementB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIImageData::IIImageData( MetaProcess* P ) : MetaTable( P )
{
   TheIIImageDataParameter = this;
}

IsoString IIImageData::Id() const
{
   return "imageData";
}

bool IIImageData::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIImageWeightRK::IIImageWeightRK( MetaTable* T ) : MetaFloat( T )
{
   TheIIImageWeightRKParameter = this;
}

IsoString IIImageWeightRK::Id() const
{
   return "weightRK";
}

int IIImageWeightRK::Precision() const
{
   return 5;
}

bool IIImageWeightRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIImageWeightG::IIImageWeightG( MetaTable* T ) : MetaFloat( T )
{
   TheIIImageWeightGParameter = this;
}

IsoString IIImageWeightG::Id() const
{
   return "weightG";
}

int IIImageWeightG::Precision() const
{
   return 5;
}

bool IIImageWeightG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIImageWeightB::IIImageWeightB( MetaTable* T ) : MetaFloat( T )
{
   TheIIImageWeightBParameter = this;
}

IsoString IIImageWeightB::Id() const
{
   return "weightB";
}

int IIImageWeightB::Precision() const
{
   return 5;
}

bool IIImageWeightB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIImageRejectedLowRK::IIImageRejectedLowRK( MetaTable* T ) : MetaUInt64( T )
{
   TheIIImageRejectedLowRKParameter = this;
}

IsoString IIImageRejectedLowRK::Id() const
{
   return "rejectedLowRK";
}

bool IIImageRejectedLowRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIImageRejectedLowG::IIImageRejectedLowG( MetaTable* T ) : MetaUInt64( T )
{
   TheIIImageRejectedLowGParameter = this;
}

IsoString IIImageRejectedLowG::Id() const
{
   return "rejectedLowG";
}

bool IIImageRejectedLowG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIImageRejectedLowB::IIImageRejectedLowB( MetaTable* T ) : MetaUInt64( T )
{
   TheIIImageRejectedLowBParameter = this;
}

IsoString IIImageRejectedLowB::Id() const
{
   return "rejectedLowB";
}

bool IIImageRejectedLowB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIImageRejectedHighRK::IIImageRejectedHighRK( MetaTable* T ) : MetaUInt64( T )
{
   TheIIImageRejectedHighRKParameter = this;
}

IsoString IIImageRejectedHighRK::Id() const
{
   return "rejectedHighRK";
}

bool IIImageRejectedHighRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIImageRejectedHighG::IIImageRejectedHighG( MetaTable* T ) : MetaUInt64( T )
{
   TheIIImageRejectedHighGParameter = this;
}

IsoString IIImageRejectedHighG::Id() const
{
   return "rejectedHighG";
}

bool IIImageRejectedHighG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IIImageRejectedHighB::IIImageRejectedHighB( MetaTable* T ) : MetaUInt64( T )
{
   TheIIImageRejectedHighBParameter = this;
}

IsoString IIImageRejectedHighB::Id() const
{
   return "rejectedHighB";
}

bool IIImageRejectedHighB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageIntegrationParameters.cpp - Released 2023-08-28T15:23:41Z
