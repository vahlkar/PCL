//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.9.8
// ----------------------------------------------------------------------------
// ImageCalibrationParameters.h - Released 2023-11-25T17:12:53Z
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

#ifndef __ImageCalibrationParameters_h
#define __ImageCalibrationParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/PSFSignalEstimator.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ICTargetFrames : public MetaTable
{
public:

   ICTargetFrames( MetaProcess* );

   IsoString Id() const override;
};

extern ICTargetFrames* TheICTargetFramesParameter;

// ----------------------------------------------------------------------------

class ICTargetFrameEnabled : public MetaBoolean
{
public:

   ICTargetFrameEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICTargetFrameEnabled* TheICTargetFrameEnabledParameter;

// ----------------------------------------------------------------------------

class ICTargetFramePath : public MetaString
{
public:

   ICTargetFramePath( MetaTable* );

   IsoString Id() const override;
};

extern ICTargetFramePath* TheICTargetFramePathParameter;

// ----------------------------------------------------------------------------

class ICEnableCFA : public MetaBoolean
{
public:

   ICEnableCFA( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICEnableCFA* TheICEnableCFAParameter;

// ----------------------------------------------------------------------------

class ICCFAPattern : public MetaEnumeration
{
public:

   enum { Auto, // includes X-Trans
          RGGB,
          BGGR,
          GBRG,
          GRBG,
          GRGB,
          GBGR,
          RGBG,
          BGRG,
          NumberOfItems,
          Default = Auto };

   ICCFAPattern( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICCFAPattern* TheICCFAPatternParameter;

// ----------------------------------------------------------------------------

class ICInputHints : public MetaString
{
public:

   ICInputHints( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICInputHints* TheICInputHintsParameter;

// ----------------------------------------------------------------------------

class ICOutputHints : public MetaString
{
public:

   ICOutputHints( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICOutputHints* TheICOutputHintsParameter;

// ----------------------------------------------------------------------------

class ICPedestal : public MetaInt32
{
public:

   ICPedestal( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICPedestal* TheICPedestalParameter;

// ----------------------------------------------------------------------------

class ICPedestalMode : public MetaEnumeration
{
public:

   enum { Literal,
          Keyword,
          CustomKeyword,
          NumberOfItems,
          Default = Keyword };

   ICPedestalMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICPedestalMode* TheICPedestalModeParameter;

// ----------------------------------------------------------------------------

class ICPedestalKeyword : public MetaString
{
public:

   ICPedestalKeyword( MetaProcess* );

   IsoString Id() const override;
};

extern ICPedestalKeyword* TheICPedestalKeywordParameter;

// ----------------------------------------------------------------------------

class ICOverscanEnabled : public MetaBoolean
{
public:

   ICOverscanEnabled( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICOverscanEnabled* TheICOverscanEnabledParameter;

// ----------------------------------------------------------------------------

class ICOverscanImageX0 : public MetaInt32
{
public:

   ICOverscanImageX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanImageX0* TheICOverscanImageX0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanImageY0 : public MetaInt32
{
public:

   ICOverscanImageY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanImageY0* TheICOverscanImageY0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanImageX1 : public MetaInt32
{
public:

   ICOverscanImageX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanImageX1* TheICOverscanImageX1Parameter;

// ----------------------------------------------------------------------------

class ICOverscanImageY1 : public MetaInt32
{
public:

   ICOverscanImageY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanImageY1* TheICOverscanImageY1Parameter;

// ----------------------------------------------------------------------------

class ICOverscanRegions : public MetaTable
{
public:

   ICOverscanRegions( MetaProcess* );

   IsoString Id() const override;
};

extern ICOverscanRegions* TheICOverscanRegionsParameter;

// ----------------------------------------------------------------------------

class ICOverscanRegionEnabled : public MetaBoolean
{
public:

   ICOverscanRegionEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICOverscanRegionEnabled* TheICOverscanRegionEnabledParameter;

// ----------------------------------------------------------------------------

class ICOverscanSourceX0 : public MetaInt32
{
public:

   ICOverscanSourceX0( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanSourceX0* TheICOverscanSourceX0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanSourceY0 : public MetaInt32
{
public:

   ICOverscanSourceY0( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanSourceY0* TheICOverscanSourceY0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanSourceX1 : public MetaInt32
{
public:

   ICOverscanSourceX1( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanSourceX1* TheICOverscanSourceX1Parameter;

// ----------------------------------------------------------------------------

class ICOverscanSourceY1 : public MetaInt32
{
public:

   ICOverscanSourceY1( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanSourceY1* TheICOverscanSourceY1Parameter;

// ----------------------------------------------------------------------------

class ICOverscanTargetX0 : public MetaInt32
{
public:

   ICOverscanTargetX0( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanTargetX0* TheICOverscanTargetX0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanTargetY0 : public MetaInt32
{
public:

   ICOverscanTargetY0( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanTargetY0* TheICOverscanTargetY0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanTargetX1 : public MetaInt32
{
public:

   ICOverscanTargetX1( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanTargetX1* TheICOverscanTargetX1Parameter;

// ----------------------------------------------------------------------------

class ICOverscanTargetY1 : public MetaInt32
{
public:

   ICOverscanTargetY1( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern ICOverscanTargetY1* TheICOverscanTargetY1Parameter;

// ----------------------------------------------------------------------------

class ICMasterBiasEnabled : public MetaBoolean
{
public:

   ICMasterBiasEnabled( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICMasterBiasEnabled* TheICMasterBiasEnabledParameter;

// ----------------------------------------------------------------------------

class ICMasterBiasPath : public MetaString
{
public:

   ICMasterBiasPath( MetaProcess* );

   IsoString Id() const override;
};

extern ICMasterBiasPath* TheICMasterBiasPathParameter;

// ----------------------------------------------------------------------------

class ICMasterDarkEnabled : public MetaBoolean
{
public:

   ICMasterDarkEnabled( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICMasterDarkEnabled* TheICMasterDarkEnabledParameter;

// ----------------------------------------------------------------------------

class ICMasterDarkPath : public MetaString
{
public:

   ICMasterDarkPath( MetaProcess* );

   IsoString Id() const override;
};

extern ICMasterDarkPath* TheICMasterDarkPathParameter;

// ----------------------------------------------------------------------------

class ICMasterFlatEnabled : public MetaBoolean
{
public:

   ICMasterFlatEnabled( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICMasterFlatEnabled* TheICMasterFlatEnabledParameter;

// ----------------------------------------------------------------------------

class ICMasterFlatPath : public MetaString
{
public:

   ICMasterFlatPath( MetaProcess* );

   IsoString Id() const override;
};

extern ICMasterFlatPath* TheICMasterFlatPathParameter;

// ----------------------------------------------------------------------------

class ICCalibrateBias : public MetaBoolean
{
public:

   ICCalibrateBias( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICCalibrateBias* TheICCalibrateBiasParameter;

// ----------------------------------------------------------------------------

class ICCalibrateDark : public MetaBoolean
{
public:

   ICCalibrateDark( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICCalibrateDark* TheICCalibrateDarkParameter;

// ----------------------------------------------------------------------------

class ICCalibrateFlat : public MetaBoolean
{
public:

   ICCalibrateFlat( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICCalibrateFlat* TheICCalibrateFlatParameter;

// ----------------------------------------------------------------------------

class ICOptimizeDarks : public MetaBoolean
{
public:

   ICOptimizeDarks( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICOptimizeDarks* TheICOptimizeDarksParameter;

// ----------------------------------------------------------------------------

/* ### DEPRECATED */
class ICDarkOptimizationThreshold : public MetaFloat
{
public:

   ICDarkOptimizationThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICDarkOptimizationThreshold* TheICDarkOptimizationThresholdParameter;

// ----------------------------------------------------------------------------

class ICDarkOptimizationLow : public MetaFloat
{
public:

   ICDarkOptimizationLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICDarkOptimizationLow* TheICDarkOptimizationLowParameter;

// ----------------------------------------------------------------------------

class ICDarkOptimizationWindow : public MetaInt32
{
public:

   ICDarkOptimizationWindow( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICDarkOptimizationWindow* TheICDarkOptimizationWindowParameter;

// ----------------------------------------------------------------------------

class ICDarkCFADetectionMode : public MetaEnumeration
{
public:

   enum { DetectCFA,
          ForceCFA,
          IgnoreCFA,
          NumberOfItems,
          Default = DetectCFA };

   ICDarkCFADetectionMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICDarkCFADetectionMode* TheICDarkCFADetectionModeParameter;

// ----------------------------------------------------------------------------

class ICSeparateCFAFlatScalingFactors : public MetaBoolean
{
public:

   ICSeparateCFAFlatScalingFactors( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICSeparateCFAFlatScalingFactors* TheICSeparateCFAFlatScalingFactorsParameter;

// ----------------------------------------------------------------------------

class ICFlatScaleClippingFactor : public MetaFloat
{
public:

   ICFlatScaleClippingFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFlatScaleClippingFactor* TheICFlatScaleClippingFactorParameter;

// ----------------------------------------------------------------------------

class ICEvaluateNoise : public MetaBoolean
{
public:

   ICEvaluateNoise( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICEvaluateNoise* TheICEvaluateNoiseParameter;

// ----------------------------------------------------------------------------

class ICNoiseEvaluationAlgorithm : public MetaEnumeration
{
public:

   enum { KSigma,
          MRS,
          NStar,
          NumberOfItems,
          Default = MRS };

   ICNoiseEvaluationAlgorithm( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICNoiseEvaluationAlgorithm* TheICNoiseEvaluationAlgorithmParameter;

// ----------------------------------------------------------------------------

class ICEvaluateSignal : public MetaBoolean
{
public:

   ICEvaluateSignal( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICEvaluateSignal* TheICEvaluateSignalParameter;

// ----------------------------------------------------------------------------

class ICStructureLayers : public MetaInt32
{
public:

   ICStructureLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICStructureLayers* TheICStructureLayersParameter;

// ----------------------------------------------------------------------------

class ICSaturationThreshold : public MetaFloat
{
public:

   ICSaturationThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICSaturationThreshold* TheICSaturationThresholdParameter;

// ----------------------------------------------------------------------------

class ICSaturationRelative : public MetaBoolean
{
public:

   ICSaturationRelative( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICSaturationRelative* TheICSaturationRelativeParameter;

// ----------------------------------------------------------------------------

class ICNoiseLayers : public MetaInt32
{
public:

   ICNoiseLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICNoiseLayers* TheICNoiseLayersParameter;

// ----------------------------------------------------------------------------

class ICHotPixelFilterRadius : public MetaInt32
{
public:

   ICHotPixelFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICHotPixelFilterRadius* TheICHotPixelFilterRadiusParameter;

// ----------------------------------------------------------------------------

class ICNoiseReductionFilterRadius : public MetaInt32
{
public:

   ICNoiseReductionFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICNoiseReductionFilterRadius* TheICNoiseReductionFilterRadiusParameter;

// ----------------------------------------------------------------------------

class ICMinStructureSize : public MetaInt32
{
public:

   ICMinStructureSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICMinStructureSize* TheICMinStructureSizeParameter;

// ----------------------------------------------------------------------------

class ICPSFType : public MetaEnumeration
{
public:

   enum { Gaussian,
          Moffat15,
          Moffat4,
          Moffat6,
          Moffat8,
          MoffatA,
          Auto,
          NumberOfItems,
          Default = Moffat4 };

   ICPSFType( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static PSFSignalEstimator::psf_function ToPSFFunction( pcl_enum x )
   {
      switch ( x )
      {
      case Gaussian: return PSFunction::Gaussian;
      case Moffat15: return PSFunction::Moffat15;
      default:
      case Moffat4:  return PSFunction::Moffat4;
      case Moffat6:  return PSFunction::Moffat6;
      case Moffat8:  return PSFunction::Moffat8;
      case MoffatA:  return PSFunction::MoffatA;
      case Auto:       return PSFunction::Auto;
      }
   }

   static IsoString FunctionName( pcl_enum );
};

extern ICPSFType* TheICPSFTypeParameter;

// ----------------------------------------------------------------------------

class ICPSFGrowth : public MetaFloat
{
public:

   ICPSFGrowth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICPSFGrowth* TheICPSFGrowthParameter;

// ----------------------------------------------------------------------------

class ICMaxStars : public MetaInt32
{
public:

   ICMaxStars( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICMaxStars* TheICMaxStarsParameter;

// ----------------------------------------------------------------------------

class ICOutputDirectory : public MetaString
{
public:

   ICOutputDirectory( MetaProcess* );

   IsoString Id() const override;
};

extern ICOutputDirectory* TheICOutputDirectoryParameter;

// ----------------------------------------------------------------------------

class ICOutputExtension : public MetaString
{
public:

   ICOutputExtension( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICOutputExtension* TheICOutputExtensionParameter;

// ----------------------------------------------------------------------------

class ICOutputPrefix : public MetaString
{
public:

   ICOutputPrefix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICOutputPrefix* TheICOutputPrefixParameter;

// ----------------------------------------------------------------------------

class ICOutputPostfix : public MetaString
{
public:

   ICOutputPostfix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICOutputPostfix* TheICOutputPostfixParameter;

// ----------------------------------------------------------------------------

class ICOutputSampleFormat : public MetaEnumeration
{
public:

   enum { I16,    // unsigned 16-bit integer
          I32,    // unsigned 32-bit integer
          F32,    // IEEE 32-bit floating point (simple precision)
          F64,    // IEEE 64-bit floating point (double precision)
          NumberOfItems,
          Default = F32 };

   ICOutputSampleFormat( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICOutputSampleFormat* TheICOutputSampleFormatParameter;

// ----------------------------------------------------------------------------

class ICOutputPedestal : public MetaInt32
{
public:

   ICOutputPedestal( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICOutputPedestal* TheICOutputPedestalParameter;

// ----------------------------------------------------------------------------

class ICOutputPedestalMode : public MetaEnumeration
{
public:

   enum { Literal,
          Auto,
          NumberOfItems,
          Default = Literal };

   ICOutputPedestalMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICOutputPedestalMode* TheICOutputPedestalModeParameter;

// ----------------------------------------------------------------------------

class ICAutoPedestalLimit : public MetaFloat
{
public:

   ICAutoPedestalLimit( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICAutoPedestalLimit* TheICAutoPedestalLimitParameter;

// ----------------------------------------------------------------------------

class ICOverwriteExistingFiles : public MetaBoolean
{
public:

   ICOverwriteExistingFiles( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICOverwriteExistingFiles* TheICOverwriteExistingFilesParameter;

// ----------------------------------------------------------------------------

class ICOnError : public MetaEnumeration
{
public:

   enum { Continue,
          Abort,
          AskUser,
          NumberOfItems,
          Default = Continue };

   ICOnError( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICOnError* TheICOnErrorParameter;

// ----------------------------------------------------------------------------

// ### DEPRECATED
class ICNoGUIMessages : public MetaBoolean
{
public:

   ICNoGUIMessages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICNoGUIMessages* TheICNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

class ICUseFileThreads : public MetaBoolean
{
public:

   ICUseFileThreads( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICUseFileThreads* TheICUseFileThreadsParameter;

// ----------------------------------------------------------------------------

class ICFileThreadOverload : public MetaFloat
{
public:

   ICFileThreadOverload( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFileThreadOverload* TheICFileThreadOverloadParameter;

// ----------------------------------------------------------------------------

class ICMaxFileReadThreads : public MetaInt32
{
public:

   ICMaxFileReadThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICMaxFileReadThreads* TheICMaxFileReadThreadsParameter;

// ----------------------------------------------------------------------------

class ICMaxFileWriteThreads : public MetaInt32
{
public:

   ICMaxFileWriteThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICMaxFileWriteThreads* TheICMaxFileWriteThreadsParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Output properties
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class ICOutputData : public MetaTable
{
public:

   ICOutputData( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICOutputData* TheICOutputDataParameter;

// ----------------------------------------------------------------------------

class ICOutputFilePath : public MetaString
{
public:

   ICOutputFilePath( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICOutputFilePath* TheICOutputFilePathParameter;

// ----------------------------------------------------------------------------

class ICDarkScalingFactorRK : public MetaFloat
{
public:

   ICDarkScalingFactorRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ICDarkScalingFactorRK* TheICDarkScalingFactorRKParameter;

// ----------------------------------------------------------------------------

class ICDarkScalingFactorG : public MetaFloat
{
public:

   ICDarkScalingFactorG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ICDarkScalingFactorG* TheICDarkScalingFactorGParameter;

// ----------------------------------------------------------------------------

class ICDarkScalingFactorB : public MetaFloat
{
public:

   ICDarkScalingFactorB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ICDarkScalingFactorB* TheICDarkScalingFactorBParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalFluxEstimateRK : public MetaDouble
{
public:

   ICPSFTotalFluxEstimateRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalFluxEstimateRK* TheICPSFTotalFluxEstimateRKParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalFluxEstimateG : public MetaDouble
{
public:

   ICPSFTotalFluxEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalFluxEstimateG* TheICPSFTotalFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalFluxEstimateB : public MetaDouble
{
public:

   ICPSFTotalFluxEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalFluxEstimateB* TheICPSFTotalFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalPowerFluxEstimateRK : public MetaDouble
{
public:

   ICPSFTotalPowerFluxEstimateRK( MetaTable* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalPowerFluxEstimateRK* TheICPSFTotalPowerFluxEstimateRKParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalPowerFluxEstimateG : public MetaDouble
{
public:

   ICPSFTotalPowerFluxEstimateG( MetaTable* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalPowerFluxEstimateG* TheICPSFTotalPowerFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalPowerFluxEstimateB : public MetaDouble
{
public:

   ICPSFTotalPowerFluxEstimateB( MetaTable* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalPowerFluxEstimateB* TheICPSFTotalPowerFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalMeanFluxEstimateRK : public MetaDouble
{
public:

   ICPSFTotalMeanFluxEstimateRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalMeanFluxEstimateRK* TheICPSFTotalMeanFluxEstimateRKParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalMeanFluxEstimateG : public MetaDouble
{
public:

   ICPSFTotalMeanFluxEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalMeanFluxEstimateG* TheICPSFTotalMeanFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalMeanFluxEstimateB : public MetaDouble
{
public:

   ICPSFTotalMeanFluxEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalMeanFluxEstimateB* TheICPSFTotalMeanFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalMeanPowerFluxEstimateRK : public MetaDouble
{
public:

   ICPSFTotalMeanPowerFluxEstimateRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalMeanPowerFluxEstimateRK* TheICPSFTotalMeanPowerFluxEstimateRKParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalMeanPowerFluxEstimateG : public MetaDouble
{
public:

   ICPSFTotalMeanPowerFluxEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalMeanPowerFluxEstimateG* TheICPSFTotalMeanPowerFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class ICPSFTotalMeanPowerFluxEstimateB : public MetaDouble
{
public:

   ICPSFTotalMeanPowerFluxEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFTotalMeanPowerFluxEstimateB* TheICPSFTotalMeanPowerFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class ICPSFMStarEstimateRK : public MetaDouble
{
public:

   ICPSFMStarEstimateRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFMStarEstimateRK* TheICPSFMStarEstimateRKParameter;

// ----------------------------------------------------------------------------

class ICPSFMStarEstimateG : public MetaDouble
{
public:

   ICPSFMStarEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFMStarEstimateG* TheICPSFMStarEstimateGParameter;

// ----------------------------------------------------------------------------

class ICPSFMStarEstimateB : public MetaDouble
{
public:

   ICPSFMStarEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFMStarEstimateB* TheICPSFMStarEstimateBParameter;

// ----------------------------------------------------------------------------

class ICPSFNStarEstimateRK : public MetaDouble
{
public:

   ICPSFNStarEstimateRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFNStarEstimateRK* TheICPSFNStarEstimateRKParameter;

// ----------------------------------------------------------------------------

class ICPSFNStarEstimateG : public MetaDouble
{
public:

   ICPSFNStarEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFNStarEstimateG* TheICPSFNStarEstimateGParameter;

// ----------------------------------------------------------------------------

class ICPSFNStarEstimateB : public MetaDouble
{
public:

   ICPSFNStarEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICPSFNStarEstimateB* TheICPSFNStarEstimateBParameter;

// ----------------------------------------------------------------------------

class ICPSFCountRK : public MetaInt32
{
public:

   ICPSFCountRK( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICPSFCountRK* TheICPSFCountRKParameter;

// ----------------------------------------------------------------------------

class ICPSFCountG : public MetaInt32
{
public:

   ICPSFCountG( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICPSFCountG* TheICPSFCountGParameter;

// ----------------------------------------------------------------------------

class ICPSFCountB : public MetaInt32
{
public:

   ICPSFCountB( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICPSFCountB* TheICPSFCountBParameter;

// ----------------------------------------------------------------------------

class ICNoiseEstimateRK : public MetaDouble
{
public:

   ICNoiseEstimateRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseEstimateRK* TheICNoiseEstimateRKParameter;

// ----------------------------------------------------------------------------

class ICNoiseEstimateG : public MetaDouble
{
public:

   ICNoiseEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseEstimateG* TheICNoiseEstimateGParameter;

// ----------------------------------------------------------------------------

class ICNoiseEstimateB : public MetaDouble
{
public:

   ICNoiseEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseEstimateB* TheICNoiseEstimateBParameter;

// ----------------------------------------------------------------------------

class ICNoiseFractionRK : public MetaDouble
{
public:

   ICNoiseFractionRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseFractionRK* TheICNoiseFractionRKParameter;

// ----------------------------------------------------------------------------

class ICNoiseFractionG : public MetaDouble
{
public:

   ICNoiseFractionG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseFractionG* TheICNoiseFractionGParameter;

// ----------------------------------------------------------------------------

class ICNoiseFractionB : public MetaDouble
{
public:

   ICNoiseFractionB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseFractionB* TheICNoiseFractionBParameter;

// ----------------------------------------------------------------------------

class ICNoiseScaleLowRK : public MetaDouble
{
public:

   ICNoiseScaleLowRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseScaleLowRK* TheICNoiseScaleLowRKParameter;

// ----------------------------------------------------------------------------

class ICNoiseScaleLowG : public MetaDouble
{
public:

   ICNoiseScaleLowG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseScaleLowG* TheICNoiseScaleLowGParameter;

// ----------------------------------------------------------------------------

class ICNoiseScaleLowB : public MetaDouble
{
public:

   ICNoiseScaleLowB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseScaleLowB* TheICNoiseScaleLowBParameter;

// ----------------------------------------------------------------------------

class ICNoiseScaleHighRK : public MetaDouble
{
public:

   ICNoiseScaleHighRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseScaleHighRK* TheICNoiseScaleHighRKParameter;

// ----------------------------------------------------------------------------

class ICNoiseScaleHighG : public MetaDouble
{
public:

   ICNoiseScaleHighG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseScaleHighG* TheICNoiseScaleHighGParameter;

// ----------------------------------------------------------------------------

class ICNoiseScaleHighB : public MetaDouble
{
public:

   ICNoiseScaleHighB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseScaleHighB* TheICNoiseScaleHighBParameter;

// ----------------------------------------------------------------------------

class ICNoiseAlgorithmRK : public MetaString
{
public:

   ICNoiseAlgorithmRK( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseAlgorithmRK* TheICNoiseAlgorithmRKParameter;

// ----------------------------------------------------------------------------

class ICNoiseAlgorithmG : public MetaString
{
public:

   ICNoiseAlgorithmG( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseAlgorithmG* TheICNoiseAlgorithmGParameter;

// ----------------------------------------------------------------------------

class ICNoiseAlgorithmB : public MetaString
{
public:

   ICNoiseAlgorithmB( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICNoiseAlgorithmB* TheICNoiseAlgorithmBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ImageCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF ImageCalibrationParameters.h - Released 2023-11-25T17:12:53Z
