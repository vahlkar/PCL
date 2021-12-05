//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.15
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ImageCalibrationParameters.cpp - Released 2021-11-25T11:45:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "ImageCalibrationParameters.h"

#include <pcl/BicubicInterpolation.h>  // for __PCL_BICUBIC_SPLINE_CLAMPING_THRESHOLD

namespace pcl
{

// ----------------------------------------------------------------------------

ICTargetFrames*                  TheICTargetFramesParameter = nullptr;
ICTargetFrameEnabled*            TheICTargetFrameEnabledParameter = nullptr;
ICTargetFramePath*               TheICTargetFramePathParameter = nullptr;

ICEnableCFA*                     TheICEnableCFAParameter = nullptr;
ICCFAPattern*                    TheICCFAPatternParameter = nullptr;

ICInputHints*                    TheICInputHintsParameter = nullptr;
ICOutputHints*                   TheICOutputHintsParameter = nullptr;

ICPedestal*                      TheICPedestalParameter = nullptr;
ICPedestalMode*                  TheICPedestalModeParameter = nullptr;
ICPedestalKeyword*               TheICPedestalKeywordParameter = nullptr;

ICOverscanEnabled*               TheICOverscanEnabledParameter = nullptr;

ICOverscanImageX0*               TheICOverscanImageX0Parameter = nullptr;
ICOverscanImageY0*               TheICOverscanImageY0Parameter = nullptr;
ICOverscanImageX1*               TheICOverscanImageX1Parameter = nullptr;
ICOverscanImageY1*               TheICOverscanImageY1Parameter = nullptr;

ICOverscanRegions*               TheICOverscanRegionsParameter = nullptr;

ICOverscanRegionEnabled*         TheICOverscanRegionEnabledParameter = nullptr;

ICOverscanSourceX0*              TheICOverscanSourceX0Parameter = nullptr;
ICOverscanSourceY0*              TheICOverscanSourceY0Parameter = nullptr;
ICOverscanSourceX1*              TheICOverscanSourceX1Parameter = nullptr;
ICOverscanSourceY1*              TheICOverscanSourceY1Parameter = nullptr;

ICOverscanTargetX0*              TheICOverscanTargetX0Parameter = nullptr;
ICOverscanTargetY0*              TheICOverscanTargetY0Parameter = nullptr;
ICOverscanTargetX1*              TheICOverscanTargetX1Parameter = nullptr;
ICOverscanTargetY1*              TheICOverscanTargetY1Parameter = nullptr;

ICMasterBiasEnabled*             TheICMasterBiasEnabledParameter = nullptr;
ICMasterBiasPath*                TheICMasterBiasPathParameter = nullptr;

ICMasterDarkEnabled*             TheICMasterDarkEnabledParameter = nullptr;
ICMasterDarkPath*                TheICMasterDarkPathParameter = nullptr;

ICMasterFlatEnabled*             TheICMasterFlatEnabledParameter = nullptr;
ICMasterFlatPath*                TheICMasterFlatPathParameter = nullptr;

ICCalibrateBias*                 TheICCalibrateBiasParameter = nullptr;
ICCalibrateDark*                 TheICCalibrateDarkParameter = nullptr;
ICCalibrateFlat*                 TheICCalibrateFlatParameter = nullptr;

ICOptimizeDarks*                 TheICOptimizeDarksParameter = nullptr;
ICDarkOptimizationThreshold*     TheICDarkOptimizationThresholdParameter = nullptr;
ICDarkOptimizationLow*           TheICDarkOptimizationLowParameter = nullptr;
ICDarkOptimizationWindow*        TheICDarkOptimizationWindowParameter = nullptr;
ICDarkCFADetectionMode*          TheICDarkCFADetectionModeParameter = nullptr;

ICSeparateCFAFlatScalingFactors* TheICSeparateCFAFlatScalingFactorsParameter = nullptr;
ICFlatScaleClippingFactor*       TheICFlatScaleClippingFactorParameter = nullptr;

ICEvaluateNoise*                 TheICEvaluateNoiseParameter = nullptr;
ICNoiseEvaluationAlgorithm*      TheICNoiseEvaluationAlgorithmParameter = nullptr;

ICEvaluateSignal*                TheICEvaluateSignalParameter = nullptr;
ICStructureLayers*               TheICStructureLayersParameter = nullptr;
ICNoiseLayers*                   TheICNoiseLayersParameter = nullptr;
ICHotPixelFilterRadius*          TheICHotPixelFilterRadiusParameter = nullptr;
ICNoiseReductionFilterRadius*    TheICNoiseReductionFilterRadiusParameter = nullptr;
ICMinStructureSize*              TheICMinStructureSizeParameter = nullptr;
ICPSFType*                       TheICPSFTypeParameter = nullptr;
ICPSFRejectionLimit*             TheICPSFRejectionLimitParameter = nullptr;
ICMaxStars*                      TheICMaxStarsParameter = nullptr;

ICOutputDirectory*               TheICOutputDirectoryParameter = nullptr;
ICOutputExtension*               TheICOutputExtensionParameter = nullptr;
ICOutputPrefix*                  TheICOutputPrefixParameter = nullptr;
ICOutputPostfix*                 TheICOutputPostfixParameter = nullptr;
ICOutputSampleFormat*            TheICOutputSampleFormatParameter = nullptr;
ICOutputPedestal*                TheICOutputPedestalParameter = nullptr;
ICOverwriteExistingFiles*        TheICOverwriteExistingFilesParameter = nullptr;
ICOnError*                       TheICOnErrorParameter = nullptr;
ICNoGUIMessages*                 TheICNoGUIMessagesParameter = nullptr;

ICUseFileThreads*                TheICUseFileThreadsParameter = nullptr;
ICFileThreadOverload*            TheICFileThreadOverloadParameter = nullptr;
ICMaxFileReadThreads*            TheICMaxFileReadThreadsParameter = nullptr;
ICMaxFileWriteThreads*           TheICMaxFileWriteThreadsParameter = nullptr;

ICOutputData*                    TheICOutputDataParameter = nullptr;
ICOutputFilePath*                TheICOutputFilePathParameter = nullptr;
ICDarkScalingFactorRK*           TheICDarkScalingFactorRKParameter = nullptr;
ICDarkScalingFactorG*            TheICDarkScalingFactorGParameter = nullptr;
ICDarkScalingFactorB*            TheICDarkScalingFactorBParameter = nullptr;
ICPSFSignalEstimateRK*           TheICPSFSignalEstimateRKParameter = nullptr;
ICPSFSignalEstimateG*            TheICPSFSignalEstimateGParameter = nullptr;
ICPSFSignalEstimateB*            TheICPSFSignalEstimateBParameter = nullptr;
ICPSFSignalPowerEstimateRK*      TheICPSFSignalPowerEstimateRKParameter = nullptr;
ICPSFSignalPowerEstimateG*       TheICPSFSignalPowerEstimateGParameter = nullptr;
ICPSFSignalPowerEstimateB*       TheICPSFSignalPowerEstimateBParameter = nullptr;
ICPSFFluxEstimateRK*             TheICPSFFluxEstimateRKParameter = nullptr;
ICPSFFluxEstimateG*              TheICPSFFluxEstimateGParameter = nullptr;
ICPSFFluxEstimateB*              TheICPSFFluxEstimateBParameter = nullptr;
ICPSFFluxPowerEstimateRK*        TheICPSFFluxPowerEstimateRKParameter = nullptr;
ICPSFFluxPowerEstimateG*         TheICPSFFluxPowerEstimateGParameter = nullptr;
ICPSFFluxPowerEstimateB*         TheICPSFFluxPowerEstimateBParameter = nullptr;
ICPSFCountRK*                    TheICPSFCountRKParameter = nullptr;
ICPSFCountG*                     TheICPSFCountGParameter = nullptr;
ICPSFCountB*                     TheICPSFCountBParameter = nullptr;
ICNoiseEstimateRK*               TheICNoiseEstimateRKParameter = nullptr;
ICNoiseEstimateG*                TheICNoiseEstimateGParameter = nullptr;
ICNoiseEstimateB*                TheICNoiseEstimateBParameter = nullptr;
ICNoiseFractionRK*               TheICNoiseFractionRKParameter = nullptr;
ICNoiseFractionG*                TheICNoiseFractionGParameter = nullptr;
ICNoiseFractionB*                TheICNoiseFractionBParameter = nullptr;
ICNoiseScaleLowRK*               TheICNoiseScaleLowRKParameter = nullptr;
ICNoiseScaleLowG*                TheICNoiseScaleLowGParameter = nullptr;
ICNoiseScaleLowB*                TheICNoiseScaleLowBParameter = nullptr;
ICNoiseScaleHighRK*              TheICNoiseScaleHighRKParameter = nullptr;
ICNoiseScaleHighG*               TheICNoiseScaleHighGParameter = nullptr;
ICNoiseScaleHighB*               TheICNoiseScaleHighBParameter = nullptr;
ICNoiseAlgorithmRK*              TheICNoiseAlgorithmRKParameter = nullptr;
ICNoiseAlgorithmG*               TheICNoiseAlgorithmGParameter = nullptr;
ICNoiseAlgorithmB*               TheICNoiseAlgorithmBParameter = nullptr;

// ----------------------------------------------------------------------------

ICTargetFrames::ICTargetFrames( MetaProcess* P ) : MetaTable( P )
{
   TheICTargetFramesParameter = this;
}

IsoString ICTargetFrames::Id() const
{
   return "targetFrames";
}

// ----------------------------------------------------------------------------

ICTargetFrameEnabled::ICTargetFrameEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheICTargetFrameEnabledParameter = this;
}

IsoString ICTargetFrameEnabled::Id() const
{
   return "enabled";
}

bool ICTargetFrameEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICTargetFramePath::ICTargetFramePath( MetaTable* T ) : MetaString( T )
{
   TheICTargetFramePathParameter = this;
}

IsoString ICTargetFramePath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

ICEnableCFA::ICEnableCFA( MetaProcess* P ) : MetaBoolean( P )
{
   TheICEnableCFAParameter = this;
}

IsoString ICEnableCFA::Id() const
{
   return "enableCFA";
}

bool ICEnableCFA::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICCFAPattern::ICCFAPattern( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICCFAPatternParameter = this;
}

IsoString ICCFAPattern::Id() const
{
   return "cfaPattern";
}

size_type ICCFAPattern::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString ICCFAPattern::ElementId( size_type i ) const
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

int ICCFAPattern::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICCFAPattern::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

ICInputHints::ICInputHints( MetaProcess* P ) : MetaString( P )
{
   TheICInputHintsParameter = this;
}

IsoString ICInputHints::Id() const
{
   return "inputHints";
}

String ICInputHints::DefaultValue() const
{
   // Input format hints:
   // * XISF: fits-keywords normalize
   // * RAW: raw cfa
   // * FITS: signed-is-physical
   return "fits-keywords normalize raw cfa signed-is-physical";
}

// ----------------------------------------------------------------------------

ICOutputHints::ICOutputHints( MetaProcess* P ) : MetaString( P )
{
   TheICOutputHintsParameter = this;
}

IsoString ICOutputHints::Id() const
{
   return "outputHints";
}

String ICOutputHints::DefaultValue() const
{
   // Output format hints:
   // * XISF: properties fits-keywords no-compress-data no-embedded-data no-resolution
   return "properties fits-keywords no-compress-data no-embedded-data no-resolution";
}

// ----------------------------------------------------------------------------

ICPedestal::ICPedestal( MetaProcess* P ) : MetaInt32( P )
{
   TheICPedestalParameter = this;
}

IsoString ICPedestal::Id() const
{
   return "pedestal";
}

double ICPedestal::DefaultValue() const
{
   return 0;
}

double ICPedestal::MinimumValue() const
{
   return 0;
}

double ICPedestal::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

ICPedestalMode::ICPedestalMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICPedestalModeParameter = this;
}

IsoString ICPedestalMode::Id() const
{
   return "pedestalMode";
}

size_type ICPedestalMode::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString ICPedestalMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Literal:       return "Literal";
   default:
   case Keyword:       return "Keyword";
   case CustomKeyword: return "CustomKeyword";
   }
}

int ICPedestalMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICPedestalMode::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICPedestalKeyword::ICPedestalKeyword( MetaProcess* P ) : MetaString( P )
{
   TheICPedestalKeywordParameter = this;
}

IsoString ICPedestalKeyword::Id() const
{
   return "pedestalKeyword";
}

// ----------------------------------------------------------------------------

ICOverscanEnabled::ICOverscanEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheICOverscanEnabledParameter = this;
}

IsoString ICOverscanEnabled::Id() const
{
   return "overscanEnabled";
}

bool ICOverscanEnabled::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICOverscanImageX0::ICOverscanImageX0( MetaProcess* P ) : MetaInt32( P )
{
   TheICOverscanImageX0Parameter = this;
}

IsoString ICOverscanImageX0::Id() const
{
   return "overscanImageX0";
}

double ICOverscanImageX0::DefaultValue() const
{
   return 0;
}

double ICOverscanImageX0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanImageY0::ICOverscanImageY0( MetaProcess* P ) : MetaInt32( P )
{
   TheICOverscanImageY0Parameter = this;
}

IsoString ICOverscanImageY0::Id() const
{
   return "overscanImageY0";
}

double ICOverscanImageY0::DefaultValue() const
{
   return 0;
}

double ICOverscanImageY0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanImageX1::ICOverscanImageX1( MetaProcess* P ) : MetaInt32( P )
{
   TheICOverscanImageX1Parameter = this;
}

IsoString ICOverscanImageX1::Id() const
{
   return "overscanImageX1";
}

double ICOverscanImageX1::DefaultValue() const
{
   return 0;
}

double ICOverscanImageX1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanImageY1::ICOverscanImageY1( MetaProcess* P ) : MetaInt32( P )
{
   TheICOverscanImageY1Parameter = this;
}

IsoString ICOverscanImageY1::Id() const
{
   return "overscanImageY1";
}

double ICOverscanImageY1::DefaultValue() const
{
   return 0;
}

double ICOverscanImageY1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanRegions::ICOverscanRegions( MetaProcess* P ) : MetaTable( P )
{
   TheICOverscanRegionsParameter = this;
}

IsoString ICOverscanRegions::Id() const
{
   return "overscanRegions";
}

// ----------------------------------------------------------------------------

ICOverscanRegionEnabled::ICOverscanRegionEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheICOverscanRegionEnabledParameter = this;
}

IsoString ICOverscanRegionEnabled::Id() const
{
   return "enabled";
}

bool ICOverscanRegionEnabled::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICOverscanSourceX0::ICOverscanSourceX0( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanSourceX0Parameter = this;
}

IsoString ICOverscanSourceX0::Id() const
{
   return "sourceX0";
}

double ICOverscanSourceX0::DefaultValue() const
{
   return 0;
}

double ICOverscanSourceX0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanSourceY0::ICOverscanSourceY0( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanSourceY0Parameter = this;
}

IsoString ICOverscanSourceY0::Id() const
{
   return "sourceY0";
}

double ICOverscanSourceY0::DefaultValue() const
{
   return 0;
}

double ICOverscanSourceY0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanSourceX1::ICOverscanSourceX1( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanSourceX1Parameter = this;
}

IsoString ICOverscanSourceX1::Id() const
{
   return "sourceX1";
}

double ICOverscanSourceX1::DefaultValue() const
{
   return 0;
}

double ICOverscanSourceX1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanSourceY1::ICOverscanSourceY1( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanSourceY1Parameter = this;
}

IsoString ICOverscanSourceY1::Id() const
{
   return "sourceY1";
}

double ICOverscanSourceY1::DefaultValue() const
{
   return 0;
}

double ICOverscanSourceY1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanTargetX0::ICOverscanTargetX0( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanTargetX0Parameter = this;
}

IsoString ICOverscanTargetX0::Id() const
{
   return "targetX0";
}

double ICOverscanTargetX0::DefaultValue() const
{
   return 0;
}

double ICOverscanTargetX0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanTargetY0::ICOverscanTargetY0( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanTargetY0Parameter = this;
}

IsoString ICOverscanTargetY0::Id() const
{
   return "targetY0";
}

double ICOverscanTargetY0::DefaultValue() const
{
   return 0;
}

double ICOverscanTargetY0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanTargetX1::ICOverscanTargetX1( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanTargetX1Parameter = this;
}

IsoString ICOverscanTargetX1::Id() const
{
   return "targetX1";
}

double ICOverscanTargetX1::DefaultValue() const
{
   return 0;
}

double ICOverscanTargetX1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanTargetY1::ICOverscanTargetY1( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanTargetY1Parameter = this;
}

IsoString ICOverscanTargetY1::Id() const
{
   return "targetY1";
}

double ICOverscanTargetY1::DefaultValue() const
{
   return 0;
}

double ICOverscanTargetY1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICMasterBiasEnabled::ICMasterBiasEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheICMasterBiasEnabledParameter = this;
}

IsoString ICMasterBiasEnabled::Id() const
{
   return "masterBiasEnabled";
}

bool ICMasterBiasEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICMasterBiasPath::ICMasterBiasPath( MetaProcess* P ) : MetaString( P )
{
   TheICMasterBiasPathParameter = this;
}

IsoString ICMasterBiasPath::Id() const
{
   return "masterBiasPath";
}

// ----------------------------------------------------------------------------

ICMasterDarkEnabled::ICMasterDarkEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheICMasterDarkEnabledParameter = this;
}

IsoString ICMasterDarkEnabled::Id() const
{
   return "masterDarkEnabled";
}

bool ICMasterDarkEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICMasterDarkPath::ICMasterDarkPath( MetaProcess* P ) : MetaString( P )
{
   TheICMasterDarkPathParameter = this;
}

IsoString ICMasterDarkPath::Id() const
{
   return "masterDarkPath";
}

// ----------------------------------------------------------------------------

ICMasterFlatEnabled::ICMasterFlatEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheICMasterFlatEnabledParameter = this;
}

IsoString ICMasterFlatEnabled::Id() const
{
   return "masterFlatEnabled";
}

bool ICMasterFlatEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICMasterFlatPath::ICMasterFlatPath( MetaProcess* P ) : MetaString( P )
{
   TheICMasterFlatPathParameter = this;
}

IsoString ICMasterFlatPath::Id() const
{
   return "masterFlatPath";
}

// ----------------------------------------------------------------------------

ICCalibrateBias::ICCalibrateBias( MetaProcess* P ) : MetaBoolean( P )
{
   TheICCalibrateBiasParameter = this;
}

IsoString ICCalibrateBias::Id() const
{
   return "calibrateBias";
}

bool ICCalibrateBias::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICCalibrateDark::ICCalibrateDark( MetaProcess* P ) : MetaBoolean( P )
{
   TheICCalibrateDarkParameter = this;
}

IsoString ICCalibrateDark::Id() const
{
   return "calibrateDark";
}

bool ICCalibrateDark::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICCalibrateFlat::ICCalibrateFlat( MetaProcess* P ) : MetaBoolean( P )
{
   TheICCalibrateFlatParameter = this;
}

IsoString ICCalibrateFlat::Id() const
{
   return "calibrateFlat";
}

bool ICCalibrateFlat::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICOptimizeDarks::ICOptimizeDarks( MetaProcess* P ) : MetaBoolean( P )
{
   TheICOptimizeDarksParameter = this;
}

IsoString ICOptimizeDarks::Id() const
{
   return "optimizeDarks";
}

bool ICOptimizeDarks::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICDarkOptimizationThreshold::ICDarkOptimizationThreshold( MetaProcess* P ) : MetaFloat( P )
{
   TheICDarkOptimizationThresholdParameter = this;
}

IsoString ICDarkOptimizationThreshold::Id() const
{
   return "darkOptimizationThreshold";
}

int ICDarkOptimizationThreshold::Precision() const
{
   return 5;
}

double ICDarkOptimizationThreshold::DefaultValue() const
{
   return 0.0;
}

double ICDarkOptimizationThreshold::MinimumValue() const
{
   return 0.0;
}

double ICDarkOptimizationThreshold::MaximumValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

ICDarkOptimizationLow::ICDarkOptimizationLow( MetaProcess* P ) : MetaFloat( P )
{
   TheICDarkOptimizationLowParameter = this;
}

IsoString ICDarkOptimizationLow::Id() const
{
   return "darkOptimizationLow";
}

int ICDarkOptimizationLow::Precision() const
{
   return 4;
}

double ICDarkOptimizationLow::DefaultValue() const
{
   return 3.0;
}

double ICDarkOptimizationLow::MinimumValue() const
{
   return 0.0;
}

double ICDarkOptimizationLow::MaximumValue() const
{
   return 10.0;
}

// ----------------------------------------------------------------------------

ICDarkOptimizationWindow::ICDarkOptimizationWindow( MetaProcess* P ) : MetaInt32( P )
{
   TheICDarkOptimizationWindowParameter = this;
}

IsoString ICDarkOptimizationWindow::Id() const
{
   return "darkOptimizationWindow";
}

double ICDarkOptimizationWindow::DefaultValue() const
{
   return 0;
}

double ICDarkOptimizationWindow::MinimumValue() const
{
   return 0; // zero means window disabled
}

double ICDarkOptimizationWindow::MaximumValue() const
{
   return 65536;
}

// ----------------------------------------------------------------------------

ICDarkCFADetectionMode::ICDarkCFADetectionMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICDarkCFADetectionModeParameter = this;
}

IsoString ICDarkCFADetectionMode::Id() const
{
   return "darkCFADetectionMode";
}

size_type ICDarkCFADetectionMode::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString ICDarkCFADetectionMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case DetectCFA: return "DetectCFA";
   case ForceCFA:  return "ForceCFA";
   case IgnoreCFA: return "IgnoreCFA";
   }
}

int ICDarkCFADetectionMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICDarkCFADetectionMode::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICSeparateCFAFlatScalingFactors::ICSeparateCFAFlatScalingFactors( MetaProcess* P ) : MetaBoolean( P )
{
   TheICSeparateCFAFlatScalingFactorsParameter = this;
}

IsoString ICSeparateCFAFlatScalingFactors::Id() const
{
   return "separateCFAFlatScalingFactors";
}

bool ICSeparateCFAFlatScalingFactors::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICFlatScaleClippingFactor::ICFlatScaleClippingFactor( MetaProcess* P ) : MetaFloat( P )
{
   TheICFlatScaleClippingFactorParameter = this;
}

IsoString ICFlatScaleClippingFactor::Id() const
{
   return "flatScaleClippingFactor";
}

int ICFlatScaleClippingFactor::Precision() const
{
   return 2;
}

double ICFlatScaleClippingFactor::DefaultValue() const
{
   return 0.05;
}

double ICFlatScaleClippingFactor::MinimumValue() const
{
   return 0.00;
}

double ICFlatScaleClippingFactor::MaximumValue() const
{
   return 0.25;
}

// ----------------------------------------------------------------------------

ICEvaluateNoise::ICEvaluateNoise( MetaProcess* P ) : MetaBoolean( P )
{
   TheICEvaluateNoiseParameter = this;
}

IsoString ICEvaluateNoise::Id() const
{
   return "evaluateNoise";
}

bool ICEvaluateNoise::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseEvaluationAlgorithm::ICNoiseEvaluationAlgorithm( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICNoiseEvaluationAlgorithmParameter = this;
}

IsoString ICNoiseEvaluationAlgorithm::Id() const
{
   return "noiseEvaluationAlgorithm";
}

size_type ICNoiseEvaluationAlgorithm::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString ICNoiseEvaluationAlgorithm::ElementId( size_type i ) const
{
   switch ( i )
   {
   case KSigma: return "NoiseEvaluation_KSigma";
   default:
   case MRS:    return "NoiseEvaluation_MRS";
   }
}

int ICNoiseEvaluationAlgorithm::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICNoiseEvaluationAlgorithm::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICEvaluateSignal::ICEvaluateSignal( MetaProcess* P ) : MetaBoolean( P )
{
   TheICEvaluateSignalParameter = this;
}

IsoString ICEvaluateSignal::Id() const
{
   return "evaluateSignal";
}

bool ICEvaluateSignal::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICStructureLayers::ICStructureLayers( MetaProcess* p ) : MetaInt32( p )
{
   TheICStructureLayersParameter = this;
}

IsoString ICStructureLayers::Id() const
{
   return "structureLayers";
}

double ICStructureLayers::DefaultValue() const
{
   return 5;
}

double ICStructureLayers::MinimumValue() const
{
   return 1;
}

double ICStructureLayers::MaximumValue() const
{
   return 8;
}

// ----------------------------------------------------------------------------

ICNoiseLayers::ICNoiseLayers( MetaProcess* p ) : MetaInt32( p )
{
   TheICNoiseLayersParameter = this;
}

IsoString ICNoiseLayers::Id() const
{
   return "noiseLayers";
}

double ICNoiseLayers::DefaultValue() const
{
   return 1;
}

double ICNoiseLayers::MinimumValue() const
{
   return 0;
}

double ICNoiseLayers::MaximumValue() const
{
   return 4;
}

// ----------------------------------------------------------------------------

ICHotPixelFilterRadius::ICHotPixelFilterRadius( MetaProcess* p ) : MetaInt32( p )
{
   TheICHotPixelFilterRadiusParameter = this;
}

IsoString ICHotPixelFilterRadius::Id() const
{
   return "hotPixelFilterRadius";
}

double ICHotPixelFilterRadius::DefaultValue() const
{
   return 1;
}

double ICHotPixelFilterRadius::MinimumValue() const
{
   return 0;
}

double ICHotPixelFilterRadius::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

ICNoiseReductionFilterRadius::ICNoiseReductionFilterRadius( MetaProcess* p ) : MetaInt32( p )
{
   TheICNoiseReductionFilterRadiusParameter = this;
}

IsoString ICNoiseReductionFilterRadius::Id() const
{
   return "noiseReductionFilterRadius";
}

double ICNoiseReductionFilterRadius::DefaultValue() const
{
   return 0;
}

double ICNoiseReductionFilterRadius::MinimumValue() const
{
   return 0;
}

double ICNoiseReductionFilterRadius::MaximumValue() const
{
   return 50;
}

// ----------------------------------------------------------------------------

ICMinStructureSize::ICMinStructureSize( MetaProcess* p ) : MetaInt32( p )
{
   TheICMinStructureSizeParameter = this;
}

IsoString ICMinStructureSize::Id() const
{
   return "minStructureSize";
}

double ICMinStructureSize::DefaultValue() const
{
   return 0;
}

double ICMinStructureSize::MinimumValue() const
{
   return 0;
}

double ICMinStructureSize::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

ICPSFType::ICPSFType( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICPSFTypeParameter = this;
}

IsoString ICPSFType::Id() const
{
   return "psfType";
}

size_type ICPSFType::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString ICPSFType::ElementId( size_type i ) const
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

IsoString ICPSFType::FunctionName( pcl_enum i )
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

int ICPSFType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICPSFType::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICPSFRejectionLimit::ICPSFRejectionLimit( MetaProcess* P ) : MetaFloat( P )
{
   TheICPSFRejectionLimitParameter = this;
}

IsoString ICPSFRejectionLimit::Id() const
{
   return "psfRejectionLimit";
}

int ICPSFRejectionLimit::Precision() const
{
   return 2;
}

double ICPSFRejectionLimit::DefaultValue() const
{
   return 5.00;
}

double ICPSFRejectionLimit::MinimumValue() const
{
   return 0.00;
}

double ICPSFRejectionLimit::MaximumValue() const
{
   return 10.0;
}

// ----------------------------------------------------------------------------

ICMaxStars::ICMaxStars( MetaProcess* p ) : MetaInt32( p )
{
   TheICMaxStarsParameter = this;
}

IsoString ICMaxStars::Id() const
{
   return "maxStars";
}

double ICMaxStars::DefaultValue() const
{
   return 24*1024;
}

double ICMaxStars::MinimumValue() const
{
   return 256;
}

double ICMaxStars::MaximumValue() const
{
   return 999999;
}

// ----------------------------------------------------------------------------

ICOutputDirectory::ICOutputDirectory( MetaProcess* P ) : MetaString( P )
{
   TheICOutputDirectoryParameter = this;
}

IsoString ICOutputDirectory::Id() const
{
   return "outputDirectory";
}

// ----------------------------------------------------------------------------

ICOutputExtension::ICOutputExtension( MetaProcess* P ) : MetaString( P )
{
   TheICOutputExtensionParameter = this;
}

IsoString ICOutputExtension::Id() const
{
   return "outputExtension";
}

String ICOutputExtension::DefaultValue() const
{
   return ".xisf";
}

// ----------------------------------------------------------------------------

ICOutputPrefix::ICOutputPrefix( MetaProcess* P ) : MetaString( P )
{
   TheICOutputPrefixParameter = this;
}

IsoString ICOutputPrefix::Id() const
{
   return "outputPrefix";
}

String ICOutputPrefix::DefaultValue() const
{
   return String(); // no prefix
}

// ----------------------------------------------------------------------------

ICOutputPostfix::ICOutputPostfix( MetaProcess* P ) : MetaString( P )
{
   TheICOutputPostfixParameter = this;
}

IsoString ICOutputPostfix::Id() const
{
   return "outputPostfix";
}

String ICOutputPostfix::DefaultValue() const
{
   return "_c";
}

// ----------------------------------------------------------------------------

ICOutputSampleFormat::ICOutputSampleFormat( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICOutputSampleFormatParameter = this;
}

IsoString ICOutputSampleFormat::Id() const
{
   return "outputSampleFormat";
}

size_type ICOutputSampleFormat::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString ICOutputSampleFormat::ElementId( size_type i ) const
{
   switch ( i )
   {
   case I16:          return "i16";
   case I32:          return "i32";
   default:
   case F32:          return "f32";
   case F64:          return "f64";
   }
}

int ICOutputSampleFormat::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICOutputSampleFormat::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICOutputPedestal::ICOutputPedestal( MetaProcess* P ) : MetaInt32( P )
{
   TheICOutputPedestalParameter = this;
}

IsoString ICOutputPedestal::Id() const
{
   return "outputPedestal";
}

double ICOutputPedestal::DefaultValue() const
{
   return 0;
}

double ICOutputPedestal::MinimumValue() const
{
   return 0;
}

double ICOutputPedestal::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

ICOverwriteExistingFiles::ICOverwriteExistingFiles( MetaProcess* P ) : MetaBoolean( P )
{
   TheICOverwriteExistingFilesParameter = this;
}

IsoString ICOverwriteExistingFiles::Id() const
{
   return "overwriteExistingFiles";
}

bool ICOverwriteExistingFiles::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICOnError::ICOnError( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICOnErrorParameter = this;
}

IsoString ICOnError::Id() const
{
   return "onError";
}

size_type ICOnError::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString ICOnError::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Continue: return "Continue";
   case Abort:    return "Abort";
   case AskUser:  return "AskUser";
   }
}

int ICOnError::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICOnError::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

// ### DEPRECATED
ICNoGUIMessages::ICNoGUIMessages( MetaProcess* p ) : MetaBoolean( p )
{
   TheICNoGUIMessagesParameter = this;
}

IsoString ICNoGUIMessages::Id() const
{
   return "noGUIMessages";
}

bool ICNoGUIMessages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICUseFileThreads::ICUseFileThreads( MetaProcess* p ) : MetaBoolean( p )
{
   TheICUseFileThreadsParameter = this;
}

IsoString ICUseFileThreads::Id() const
{
   return "useFileThreads";
}

bool ICUseFileThreads::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICFileThreadOverload::ICFileThreadOverload( MetaProcess* p ) : MetaFloat( p )
{
   TheICFileThreadOverloadParameter = this;
}

IsoString ICFileThreadOverload::Id() const
{
   return "fileThreadOverload";
}

int ICFileThreadOverload::Precision() const
{
   return 2;
}

double ICFileThreadOverload::DefaultValue() const
{
   return 1.0;
}

double ICFileThreadOverload::MinimumValue() const
{
   return 1;
}

double ICFileThreadOverload::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

ICMaxFileReadThreads::ICMaxFileReadThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheICMaxFileReadThreadsParameter = this;
}

IsoString ICMaxFileReadThreads::Id() const
{
   return "maxFileReadThreads";
}

double ICMaxFileReadThreads::DefaultValue() const
{
   return 0;
}

double ICMaxFileReadThreads::MinimumValue() const
{
   return 0;
}

double ICMaxFileReadThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

ICMaxFileWriteThreads::ICMaxFileWriteThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheICMaxFileWriteThreadsParameter = this;
}

IsoString ICMaxFileWriteThreads::Id() const
{
   return "maxFileWriteThreads";
}

double ICMaxFileWriteThreads::DefaultValue() const
{
   return 0;
}

double ICMaxFileWriteThreads::MinimumValue() const
{
   return 0;
}

double ICMaxFileWriteThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ICOutputData::ICOutputData( MetaProcess* P ) : MetaTable( P )
{
   TheICOutputDataParameter = this;
}

IsoString ICOutputData::Id() const
{
   return "outputData";
}

bool ICOutputData::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICOutputFilePath::ICOutputFilePath( MetaTable* T ) : MetaString( T )
{
   TheICOutputFilePathParameter = this;
}

IsoString ICOutputFilePath::Id() const
{
   return "outputFilePath";
}

bool ICOutputFilePath::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICDarkScalingFactorRK::ICDarkScalingFactorRK( MetaTable* T ) : MetaFloat( T )
{
   TheICDarkScalingFactorRKParameter = this;
}

IsoString ICDarkScalingFactorRK::Id() const
{
   return "darkScalingFactorRK";
}

int ICDarkScalingFactorRK::Precision() const
{
   return 3;
}

bool ICDarkScalingFactorRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICDarkScalingFactorG::ICDarkScalingFactorG( MetaTable* T ) : MetaFloat( T )
{
   TheICDarkScalingFactorGParameter = this;
}

IsoString ICDarkScalingFactorG::Id() const
{
   return "darkScalingFactorG";
}

int ICDarkScalingFactorG::Precision() const
{
   return 3;
}

bool ICDarkScalingFactorG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICDarkScalingFactorB::ICDarkScalingFactorB( MetaTable* T ) : MetaFloat( T )
{
   TheICDarkScalingFactorBParameter = this;
}

IsoString ICDarkScalingFactorB::Id() const
{
   return "darkScalingFactorB";
}

int ICDarkScalingFactorB::Precision() const
{
   return 3;
}

bool ICDarkScalingFactorB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFSignalEstimateRK::ICPSFSignalEstimateRK( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFSignalEstimateRKParameter = this;
}

IsoString ICPSFSignalEstimateRK::Id() const
{
   return "psfSignalEstimateRK";
}

int ICPSFSignalEstimateRK::Precision() const
{
   return 4;
}

bool ICPSFSignalEstimateRK::ScientificNotation() const
{
   return true;
}

bool ICPSFSignalEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFSignalEstimateG::ICPSFSignalEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFSignalEstimateGParameter = this;
}

IsoString ICPSFSignalEstimateG::Id() const
{
   return "psfSignalEstimateG";
}

int ICPSFSignalEstimateG::Precision() const
{
   return 4;
}

bool ICPSFSignalEstimateG::ScientificNotation() const
{
   return true;
}

bool ICPSFSignalEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFSignalEstimateB::ICPSFSignalEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFSignalEstimateBParameter = this;
}

IsoString ICPSFSignalEstimateB::Id() const
{
   return "psfSignalEstimateB";
}

int ICPSFSignalEstimateB::Precision() const
{
   return 4;
}

bool ICPSFSignalEstimateB::ScientificNotation() const
{
   return true;
}

bool ICPSFSignalEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFSignalPowerEstimateRK::ICPSFSignalPowerEstimateRK( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFSignalPowerEstimateRKParameter = this;
}

IsoString ICPSFSignalPowerEstimateRK::Id() const
{
   return "psfSignalPowerEstimateRK";
}

IsoString ICPSFSignalPowerEstimateRK::Aliases() const
{
   return "psfPowerEstimateRK";
}

int ICPSFSignalPowerEstimateRK::Precision() const
{
   return 4;
}

bool ICPSFSignalPowerEstimateRK::ScientificNotation() const
{
   return true;
}

bool ICPSFSignalPowerEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFSignalPowerEstimateG::ICPSFSignalPowerEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFSignalPowerEstimateGParameter = this;
}

IsoString ICPSFSignalPowerEstimateG::Id() const
{
   return "psfSignalPowerEstimateG";
}

IsoString ICPSFSignalPowerEstimateG::Aliases() const
{
   return "psfPowerEstimateG";
}

int ICPSFSignalPowerEstimateG::Precision() const
{
   return 4;
}

bool ICPSFSignalPowerEstimateG::ScientificNotation() const
{
   return true;
}

bool ICPSFSignalPowerEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFSignalPowerEstimateB::ICPSFSignalPowerEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFSignalPowerEstimateBParameter = this;
}

IsoString ICPSFSignalPowerEstimateB::Id() const
{
   return "psfSignalPowerEstimateB";
}

IsoString ICPSFSignalPowerEstimateB::Aliases() const
{
   return "psfPowerEstimateB";
}

int ICPSFSignalPowerEstimateB::Precision() const
{
   return 4;
}

bool ICPSFSignalPowerEstimateB::ScientificNotation() const
{
   return true;
}

bool ICPSFSignalPowerEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFFluxEstimateRK::ICPSFFluxEstimateRK( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFFluxEstimateRKParameter = this;
}

IsoString ICPSFFluxEstimateRK::Id() const
{
   return "psfFluxEstimateRK";
}

int ICPSFFluxEstimateRK::Precision() const
{
   return 4;
}

bool ICPSFFluxEstimateRK::ScientificNotation() const
{
   return true;
}

bool ICPSFFluxEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFFluxEstimateG::ICPSFFluxEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFFluxEstimateGParameter = this;
}

IsoString ICPSFFluxEstimateG::Id() const
{
   return "psfFluxEstimateG";
}

int ICPSFFluxEstimateG::Precision() const
{
   return 4;
}

bool ICPSFFluxEstimateG::ScientificNotation() const
{
   return true;
}

bool ICPSFFluxEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFFluxEstimateB::ICPSFFluxEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFFluxEstimateBParameter = this;
}

IsoString ICPSFFluxEstimateB::Id() const
{
   return "psfFluxEstimateB";
}

int ICPSFFluxEstimateB::Precision() const
{
   return 4;
}

bool ICPSFFluxEstimateB::ScientificNotation() const
{
   return true;
}

bool ICPSFFluxEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFFluxPowerEstimateRK::ICPSFFluxPowerEstimateRK( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFFluxPowerEstimateRKParameter = this;
}

IsoString ICPSFFluxPowerEstimateRK::Id() const
{
   return "psfFluxPowerEstimateRK";
}

int ICPSFFluxPowerEstimateRK::Precision() const
{
   return 4;
}

bool ICPSFFluxPowerEstimateRK::ScientificNotation() const
{
   return true;
}

bool ICPSFFluxPowerEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFFluxPowerEstimateG::ICPSFFluxPowerEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFFluxPowerEstimateGParameter = this;
}

IsoString ICPSFFluxPowerEstimateG::Id() const
{
   return "psfFluxPowerEstimateG";
}

int ICPSFFluxPowerEstimateG::Precision() const
{
   return 4;
}

bool ICPSFFluxPowerEstimateG::ScientificNotation() const
{
   return true;
}

bool ICPSFFluxPowerEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFFluxPowerEstimateB::ICPSFFluxPowerEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheICPSFFluxPowerEstimateBParameter = this;
}

IsoString ICPSFFluxPowerEstimateB::Id() const
{
   return "psfFluxPowerEstimateB";
}

int ICPSFFluxPowerEstimateB::Precision() const
{
   return 4;
}

bool ICPSFFluxPowerEstimateB::ScientificNotation() const
{
   return true;
}

bool ICPSFFluxPowerEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFCountRK::ICPSFCountRK( MetaTable* T ) : MetaInt32( T )
{
   TheICPSFCountRKParameter = this;
}

IsoString ICPSFCountRK::Id() const
{
   return "psfCountRK";
}

bool ICPSFCountRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFCountG::ICPSFCountG( MetaTable* T ) : MetaInt32( T )
{
   TheICPSFCountGParameter = this;
}

IsoString ICPSFCountG::Id() const
{
   return "psfCountG";
}

bool ICPSFCountG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICPSFCountB::ICPSFCountB( MetaTable* T ) : MetaInt32( T )
{
   TheICPSFCountBParameter = this;
}

IsoString ICPSFCountB::Id() const
{
   return "psfCountB";
}

bool ICPSFCountB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseEstimateRK::ICNoiseEstimateRK( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseEstimateRKParameter = this;
}

IsoString ICNoiseEstimateRK::Id() const
{
   return "noiseEstimateRK";
}

int ICNoiseEstimateRK::Precision() const
{
   return 3;
}

bool ICNoiseEstimateRK::ScientificNotation() const
{
   return true;
}

bool ICNoiseEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseEstimateG::ICNoiseEstimateG( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseEstimateGParameter = this;
}

IsoString ICNoiseEstimateG::Id() const
{
   return "noiseEstimateG";
}

int ICNoiseEstimateG::Precision() const
{
   return 3;
}

bool ICNoiseEstimateG::ScientificNotation() const
{
   return true;
}

bool ICNoiseEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseEstimateB::ICNoiseEstimateB( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseEstimateBParameter = this;
}

IsoString ICNoiseEstimateB::Id() const
{
   return "noiseEstimateB";
}

int ICNoiseEstimateB::Precision() const
{
   return 3;
}

bool ICNoiseEstimateB::ScientificNotation() const
{
   return true;
}

bool ICNoiseEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseFractionRK::ICNoiseFractionRK( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseFractionRKParameter = this;
}

IsoString ICNoiseFractionRK::Id() const
{
   return "noiseFractionRK";
}

int ICNoiseFractionRK::Precision() const
{
   return 3;
}

bool ICNoiseFractionRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseFractionG::ICNoiseFractionG( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseFractionGParameter = this;
}

IsoString ICNoiseFractionG::Id() const
{
   return "noiseFractionG";
}

int ICNoiseFractionG::Precision() const
{
   return 3;
}

bool ICNoiseFractionG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseFractionB::ICNoiseFractionB( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseFractionBParameter = this;
}

IsoString ICNoiseFractionB::Id() const
{
   return "noiseFractionB";
}

int ICNoiseFractionB::Precision() const
{
   return 3;
}

bool ICNoiseFractionB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseScaleLowRK::ICNoiseScaleLowRK( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseScaleLowRKParameter = this;
}

IsoString ICNoiseScaleLowRK::Id() const
{
   return "noiseScaleLowRK";
}

int ICNoiseScaleLowRK::Precision() const
{
   return 6;
}

bool ICNoiseScaleLowRK::ScientificNotation() const
{
   return true;
}

bool ICNoiseScaleLowRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseScaleLowG::ICNoiseScaleLowG( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseScaleLowGParameter = this;
}

IsoString ICNoiseScaleLowG::Id() const
{
   return "noiseScaleLowG";
}

int ICNoiseScaleLowG::Precision() const
{
   return 6;
}

bool ICNoiseScaleLowG::ScientificNotation() const
{
   return true;
}

bool ICNoiseScaleLowG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseScaleLowB::ICNoiseScaleLowB( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseScaleLowBParameter = this;
}

IsoString ICNoiseScaleLowB::Id() const
{
   return "noiseScaleLowB";
}

int ICNoiseScaleLowB::Precision() const
{
   return 6;
}

bool ICNoiseScaleLowB::ScientificNotation() const
{
   return true;
}

bool ICNoiseScaleLowB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseScaleHighRK::ICNoiseScaleHighRK( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseScaleHighRKParameter = this;
}

IsoString ICNoiseScaleHighRK::Id() const
{
   return "noiseScaleHighRK";
}

int ICNoiseScaleHighRK::Precision() const
{
   return 6;
}

bool ICNoiseScaleHighRK::ScientificNotation() const
{
   return true;
}

bool ICNoiseScaleHighRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseScaleHighG::ICNoiseScaleHighG( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseScaleHighGParameter = this;
}

IsoString ICNoiseScaleHighG::Id() const
{
   return "noiseScaleHighG";
}

int ICNoiseScaleHighG::Precision() const
{
   return 6;
}

bool ICNoiseScaleHighG::ScientificNotation() const
{
   return true;
}

bool ICNoiseScaleHighG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseScaleHighB::ICNoiseScaleHighB( MetaTable* T ) : MetaDouble( T )
{
   TheICNoiseScaleHighBParameter = this;
}

IsoString ICNoiseScaleHighB::Id() const
{
   return "noiseScaleHighB";
}

int ICNoiseScaleHighB::Precision() const
{
   return 6;
}

bool ICNoiseScaleHighB::ScientificNotation() const
{
   return true;
}

bool ICNoiseScaleHighB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseAlgorithmRK::ICNoiseAlgorithmRK( MetaTable* T ) : MetaString( T )
{
   TheICNoiseAlgorithmRKParameter = this;
}

IsoString ICNoiseAlgorithmRK::Id() const
{
   return "noiseAlgorithmRK";
}

bool ICNoiseAlgorithmRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseAlgorithmG::ICNoiseAlgorithmG( MetaTable* T ) : MetaString( T )
{
   TheICNoiseAlgorithmGParameter = this;
}

IsoString ICNoiseAlgorithmG::Id() const
{
   return "noiseAlgorithmG";
}

bool ICNoiseAlgorithmG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseAlgorithmB::ICNoiseAlgorithmB( MetaTable* T ) : MetaString( T )
{
   TheICNoiseAlgorithmBParameter = this;
}

IsoString ICNoiseAlgorithmB::Id() const
{
   return "noiseAlgorithmB";
}

bool ICNoiseAlgorithmB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageCalibrationParameters.cpp - Released 2021-11-25T11:45:24Z
