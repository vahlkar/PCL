//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.15
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 1.10.1
// ----------------------------------------------------------------------------
// DebayerParameters.h - Released 2021-11-25T11:45:24Z
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

#ifndef __DebayerParameters_h
#define __DebayerParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/PSFSignalEstimator.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class DebayerBayerPatternParameter : public MetaEnumeration
{
public:

   enum { Auto,
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

   DebayerBayerPatternParameter( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DebayerBayerPatternParameter* TheDebayerBayerPatternParameter;

// ----------------------------------------------------------------------------

class DebayerMethodParameter : public MetaEnumeration
{
public:

   enum { SuperPixel,
          Bilinear,
          VNG,
          /*AHD,*/ // ### Uncomment when implemented
          NumberOfItems,
          Default = VNG };

   DebayerMethodParameter( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DebayerMethodParameter* TheDebayerMethodParameter;

// ----------------------------------------------------------------------------

class DebayerFBDDNoiseReduction : public MetaInt32
{
public:

   DebayerFBDDNoiseReduction( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerFBDDNoiseReduction* TheDebayerFBDDNoiseReductionParameter;

// ----------------------------------------------------------------------------

class DebayerShowImages : public MetaBoolean
{
public:

   DebayerShowImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerShowImages* TheDebayerShowImagesParameter;

// ----------------------------------------------------------------------------

class DebayerCFASourceFilePath : public MetaString
{
public:

   DebayerCFASourceFilePath( MetaProcess* );

   IsoString Id() const override;
};

extern DebayerCFASourceFilePath* TheDebayerCFASourceFilePathParameter;

// ----------------------------------------------------------------------------

class DebayerTargetItems : public MetaTable
{
public:

   DebayerTargetItems( MetaProcess* );

   IsoString Id() const override;
};

extern DebayerTargetItems* TheDebayerTargetItemsParameter;

// ----------------------------------------------------------------------------

class DebayerTargetEnabled : public MetaBoolean
{
public:

   DebayerTargetEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerTargetEnabled* TheDebayerTargetEnabledParameter;

// ----------------------------------------------------------------------------

class DebayerTargetImage : public MetaString
{
public:

   DebayerTargetImage( MetaTable* );

   IsoString Id() const override;
};

extern DebayerTargetImage* TheDebayerTargetImageParameter;

// ----------------------------------------------------------------------------

class DebayerNoGUIMessages : public MetaBoolean
{
public:

   DebayerNoGUIMessages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerNoGUIMessages* TheDebayerNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

class DebayerEvaluateNoise : public MetaBoolean
{
public:

   DebayerEvaluateNoise( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerEvaluateNoise* TheDebayerEvaluateNoiseParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEvaluationAlgorithm : public MetaEnumeration
{
public:

   enum { KSigma,
          MRS,
          NumberOfItems,
          Default = MRS };

   DebayerNoiseEvaluationAlgorithm( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DebayerNoiseEvaluationAlgorithm* TheDebayerNoiseEvaluationAlgorithmParameter;

// ----------------------------------------------------------------------------

class DebayerEvaluateSignal : public MetaBoolean
{
public:

   DebayerEvaluateSignal( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerEvaluateSignal* TheDebayerEvaluateSignalParameter;

// ----------------------------------------------------------------------------

class DebayerStructureLayers : public MetaInt32
{
public:

   DebayerStructureLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerStructureLayers* TheDebayerStructureLayersParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseLayers : public MetaInt32
{
public:

   DebayerNoiseLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerNoiseLayers* TheDebayerNoiseLayersParameter;

// ----------------------------------------------------------------------------

class DebayerHotPixelFilterRadius : public MetaInt32
{
public:

   DebayerHotPixelFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerHotPixelFilterRadius* TheDebayerHotPixelFilterRadiusParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseReductionFilterRadius : public MetaInt32
{
public:

   DebayerNoiseReductionFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerNoiseReductionFilterRadius* TheDebayerNoiseReductionFilterRadiusParameter;

// ----------------------------------------------------------------------------

class DebayerMinStructureSize : public MetaInt32
{
public:

   DebayerMinStructureSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerMinStructureSize* TheDebayerMinStructureSizeParameter;

// ----------------------------------------------------------------------------

class DebayerPSFType : public MetaEnumeration
{
public:

   enum { Gaussian,
          Moffat4,
          Moffat6,
          Moffat8,
          VariableShape,
          NumberOfItems,
          Default = Moffat4 };

   DebayerPSFType( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static PSFSignalEstimator::psf_function ToPSFFunction( pcl_enum x )
   {
      switch ( x )
      {
      case Gaussian:      return PSFunction::Gaussian;
      default:
      case Moffat4:       return PSFunction::Moffat4;
      case Moffat6:       return PSFunction::Moffat6;
      case Moffat8:       return PSFunction::Moffat8;
      case VariableShape: return PSFunction::VariableShape;
      }
   }

   static IsoString FunctionName( pcl_enum );
};

extern DebayerPSFType* TheDebayerPSFTypeParameter;

// ----------------------------------------------------------------------------

class DebayerPSFRejectionLimit : public MetaFloat
{
public:

   DebayerPSFRejectionLimit( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerPSFRejectionLimit* TheDebayerPSFRejectionLimitParameter;

// ----------------------------------------------------------------------------

class DebayerMaxStars : public MetaInt32
{
public:

   DebayerMaxStars( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerMaxStars* TheDebayerMaxStarsParameter;

// ----------------------------------------------------------------------------

class DebayerInputHints : public MetaString
{
public:

   DebayerInputHints( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern DebayerInputHints* TheDebayerInputHintsParameter;

// ----------------------------------------------------------------------------

class DebayerOutputHints : public MetaString
{
public:

   DebayerOutputHints( MetaProcess* );

   IsoString Id() const override;
};

extern DebayerOutputHints* TheDebayerOutputHintsParameter;

// ----------------------------------------------------------------------------

class DebayerOutputRGBImages : public MetaBoolean
{
public:

   DebayerOutputRGBImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerOutputRGBImages* TheDebayerOutputRGBImagesParameter;

// ----------------------------------------------------------------------------

class DebayerOutputSeparateChannels : public MetaBoolean
{
public:

   DebayerOutputSeparateChannels( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerOutputSeparateChannels* TheDebayerOutputSeparateChannelsParameter;

// ----------------------------------------------------------------------------

class DebayerOutputDirectory : public MetaString
{
public:

   DebayerOutputDirectory( MetaProcess* );

   IsoString Id() const override;
};

extern DebayerOutputDirectory* TheDebayerOutputDirectoryParameter;

// ----------------------------------------------------------------------------

class DebayerOutputExtension : public MetaString
{
public:

   DebayerOutputExtension( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern DebayerOutputExtension* TheDebayerOutputExtensionParameter;

// ----------------------------------------------------------------------------

class DebayerOutputPrefix : public MetaString
{
public:

   DebayerOutputPrefix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern DebayerOutputPrefix* TheDebayerOutputPrefixParameter;

// ----------------------------------------------------------------------------

class DebayerOutputPostfix : public MetaString
{
public:

   DebayerOutputPostfix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern DebayerOutputPostfix* TheDebayerOutputPostfixParameter;

// ----------------------------------------------------------------------------

class DebayerOverwriteExistingFiles : public MetaBoolean
{
public:

   DebayerOverwriteExistingFiles( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerOverwriteExistingFiles* TheDebayerOverwriteExistingFilesParameter;

// ----------------------------------------------------------------------------

class DebayerOnError : public MetaEnumeration
{
public:

   enum { Continue,
          Abort,
          AskUser,
          NumberOfErrorPolicies,
          Default = Continue };

   DebayerOnError( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DebayerOnError* TheDebayerOnErrorParameter;

// ----------------------------------------------------------------------------

class DebayerUseFileThreads : public MetaBoolean
{
public:

   DebayerUseFileThreads( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerUseFileThreads* TheDebayerUseFileThreadsParameter;

// ----------------------------------------------------------------------------

class DebayerFileThreadOverload : public MetaFloat
{
public:

   DebayerFileThreadOverload( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerFileThreadOverload* TheDebayerFileThreadOverloadParameter;

// ----------------------------------------------------------------------------

class DebayerMaxFileReadThreads : public MetaInt32
{
public:

   DebayerMaxFileReadThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerMaxFileReadThreads* TheDebayerMaxFileReadThreadsParameter;

// ----------------------------------------------------------------------------

class DebayerMaxFileWriteThreads : public MetaInt32
{
public:

   DebayerMaxFileWriteThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerMaxFileWriteThreads* TheDebayerMaxFileWriteThreadsParameter;

// ----------------------------------------------------------------------------

class DebayerMemoryLoadControl : public MetaBoolean
{
public:

   DebayerMemoryLoadControl( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerMemoryLoadControl* TheDebayerMemoryLoadControlParameter;

// ----------------------------------------------------------------------------

class DebayerMemoryLoadLimit : public MetaFloat
{
public:

   DebayerMemoryLoadLimit( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerMemoryLoadLimit* TheDebayerMemoryLoadLimitParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DebayerOutputImage : public MetaString
{
public:

   DebayerOutputImage( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputImage* TheDebayerOutputImageParameter;

// ----------------------------------------------------------------------------

class DebayerOutputChannelImageR : public MetaString
{
public:

   DebayerOutputChannelImageR( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputChannelImageR* TheDebayerOutputChannelImageRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputChannelImageG : public MetaString
{
public:

   DebayerOutputChannelImageG( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputChannelImageG* TheDebayerOutputChannelImageGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputChannelImageB : public MetaString
{
public:

   DebayerOutputChannelImageB( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputChannelImageB* TheDebayerOutputChannelImageBParameter;

// ----------------------------------------------------------------------------

class DebayerPSFSignalEstimateR : public MetaDouble
{
public:

   DebayerPSFSignalEstimateR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFSignalEstimateR* TheDebayerPSFSignalEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerPSFSignalEstimateG : public MetaDouble
{
public:

   DebayerPSFSignalEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFSignalEstimateG* TheDebayerPSFSignalEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerPSFSignalEstimateB : public MetaDouble
{
public:

   DebayerPSFSignalEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFSignalEstimateB* TheDebayerPSFSignalEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerPSFSignalPowerEstimateR : public MetaDouble
{
public:

   DebayerPSFSignalPowerEstimateR( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFSignalPowerEstimateR* TheDebayerPSFSignalPowerEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerPSFSignalPowerEstimateG : public MetaDouble
{
public:

   DebayerPSFSignalPowerEstimateG( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFSignalPowerEstimateG* TheDebayerPSFSignalPowerEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerPSFSignalPowerEstimateB : public MetaDouble
{
public:

   DebayerPSFSignalPowerEstimateB( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFSignalPowerEstimateB* TheDebayerPSFSignalPowerEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerPSFFluxEstimateR : public MetaDouble
{
public:

   DebayerPSFFluxEstimateR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFFluxEstimateR* TheDebayerPSFFluxEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerPSFFluxEstimateG : public MetaDouble
{
public:

   DebayerPSFFluxEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFFluxEstimateG* TheDebayerPSFFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerPSFFluxEstimateB : public MetaDouble
{
public:

   DebayerPSFFluxEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFFluxEstimateB* TheDebayerPSFFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerPSFFluxPowerEstimateR : public MetaDouble
{
public:

   DebayerPSFFluxPowerEstimateR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFFluxPowerEstimateR* TheDebayerPSFFluxPowerEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerPSFFluxPowerEstimateG : public MetaDouble
{
public:

   DebayerPSFFluxPowerEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFFluxPowerEstimateG* TheDebayerPSFFluxPowerEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerPSFFluxPowerEstimateB : public MetaDouble
{
public:

   DebayerPSFFluxPowerEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFFluxPowerEstimateB* TheDebayerPSFFluxPowerEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerPSFCountR : public MetaInt32
{
public:

   DebayerPSFCountR( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFCountR* TheDebayerPSFCountRParameter;

// ----------------------------------------------------------------------------

class DebayerPSFCountG : public MetaInt32
{
public:

   DebayerPSFCountG( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFCountG* TheDebayerPSFCountGParameter;

// ----------------------------------------------------------------------------

class DebayerPSFCountB : public MetaInt32
{
public:

   DebayerPSFCountB( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFCountB* TheDebayerPSFCountBParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateR : public MetaDouble
{
public:

   DebayerNoiseEstimateR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseEstimateR* TheDebayerNoiseEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateG : public MetaDouble
{
public:

   DebayerNoiseEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseEstimateG* TheDebayerNoiseEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateB : public MetaDouble
{
public:

   DebayerNoiseEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseEstimateB* TheDebayerNoiseEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionR : public MetaDouble
{
public:

   DebayerNoiseFractionR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseFractionR* TheDebayerNoiseFractionRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionG : public MetaDouble
{
public:

   DebayerNoiseFractionG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseFractionG* TheDebayerNoiseFractionGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionB : public MetaDouble
{
public:

   DebayerNoiseFractionB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseFractionB* TheDebayerNoiseFractionBParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseScaleLowR : public MetaDouble
{
public:

   DebayerNoiseScaleLowR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseScaleLowR* TheDebayerNoiseScaleLowRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseScaleLowG : public MetaDouble
{
public:

   DebayerNoiseScaleLowG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseScaleLowG* TheDebayerNoiseScaleLowGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseScaleLowB : public MetaDouble
{
public:

   DebayerNoiseScaleLowB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseScaleLowB* TheDebayerNoiseScaleLowBParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseScaleHighR : public MetaDouble
{
public:

   DebayerNoiseScaleHighR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseScaleHighR* TheDebayerNoiseScaleHighRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseScaleHighG : public MetaDouble
{
public:

   DebayerNoiseScaleHighG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseScaleHighG* TheDebayerNoiseScaleHighGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseScaleHighB : public MetaDouble
{
public:

   DebayerNoiseScaleHighB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseScaleHighB* TheDebayerNoiseScaleHighBParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmR : public MetaString
{
public:

   DebayerNoiseAlgorithmR( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseAlgorithmR* TheDebayerNoiseAlgorithmRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmG : public MetaString
{
public:

   DebayerNoiseAlgorithmG( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseAlgorithmG* TheDebayerNoiseAlgorithmGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmB : public MetaString
{
public:

   DebayerNoiseAlgorithmB( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseAlgorithmB* TheDebayerNoiseAlgorithmBParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DebayerOutputFileData : public MetaTable
{
public:

   DebayerOutputFileData( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileData* TheDebayerOutputFileDataParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePath : public MetaString
{
public:

   DebayerOutputFilePath( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePath* TheDebayerOutputFilePathParameter;

// ----------------------------------------------------------------------------

class DebayerOutputChannelFilePathR : public MetaString
{
public:

   DebayerOutputChannelFilePathR( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputChannelFilePathR* TheDebayerOutputChannelFilePathRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputChannelFilePathG : public MetaString
{
public:

   DebayerOutputChannelFilePathG( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputChannelFilePathG* TheDebayerOutputChannelFilePathGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputChannelFilePathB : public MetaString
{
public:

   DebayerOutputChannelFilePathB( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputChannelFilePathB* TheDebayerOutputChannelFilePathBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFSignalEstimateR : public MetaDouble
{
public:

   DebayerOutputFilePSFSignalEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFSignalEstimateR* TheDebayerOutputFilePSFSignalEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFSignalEstimateG : public MetaDouble
{
public:

   DebayerOutputFilePSFSignalEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFSignalEstimateG* TheDebayerOutputFilePSFSignalEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFSignalEstimateB : public MetaDouble
{
public:

   DebayerOutputFilePSFSignalEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFSignalEstimateB* TheDebayerOutputFilePSFSignalEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFSignalPowerEstimateR : public MetaDouble
{
public:

   DebayerOutputFilePSFSignalPowerEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFSignalPowerEstimateR* TheDebayerOutputFilePSFSignalPowerEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFSignalPowerEstimateG : public MetaDouble
{
public:

   DebayerOutputFilePSFSignalPowerEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFSignalPowerEstimateG* TheDebayerOutputFilePSFSignalPowerEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFSignalPowerEstimateB : public MetaDouble
{
public:

   DebayerOutputFilePSFSignalPowerEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFSignalPowerEstimateB* TheDebayerOutputFilePSFSignalPowerEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFFluxEstimateR : public MetaDouble
{
public:

   DebayerOutputFilePSFFluxEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFFluxEstimateR* TheDebayerOutputFilePSFFluxEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFFluxEstimateG : public MetaDouble
{
public:

   DebayerOutputFilePSFFluxEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFFluxEstimateG* TheDebayerOutputFilePSFFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFFluxEstimateB : public MetaDouble
{
public:

   DebayerOutputFilePSFFluxEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFFluxEstimateB* TheDebayerOutputFilePSFFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFFluxPowerEstimateR : public MetaDouble
{
public:

   DebayerOutputFilePSFFluxPowerEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFFluxPowerEstimateR* TheDebayerOutputFilePSFFluxPowerEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFFluxPowerEstimateG : public MetaDouble
{
public:

   DebayerOutputFilePSFFluxPowerEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFFluxPowerEstimateG* TheDebayerOutputFilePSFFluxPowerEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFFluxPowerEstimateB : public MetaDouble
{
public:

   DebayerOutputFilePSFFluxPowerEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFFluxPowerEstimateB* TheDebayerOutputFilePSFFluxPowerEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFCountR : public MetaInt32
{
public:

   DebayerOutputFilePSFCountR( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFCountR* TheDebayerOutputFilePSFCountRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFCountG : public MetaInt32
{
public:

   DebayerOutputFilePSFCountG( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFCountG* TheDebayerOutputFilePSFCountGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFCountB : public MetaInt32
{
public:

   DebayerOutputFilePSFCountB( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFCountB* TheDebayerOutputFilePSFCountBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseEstimateR : public MetaDouble
{
public:

   DebayerOutputFileNoiseEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseEstimateR* TheDebayerOutputFileNoiseEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseEstimateG : public MetaDouble
{
public:

   DebayerOutputFileNoiseEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseEstimateG* TheDebayerOutputFileNoiseEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseEstimateB : public MetaDouble
{
public:

   DebayerOutputFileNoiseEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseEstimateB* TheDebayerOutputFileNoiseEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseFractionR : public MetaDouble
{
public:

   DebayerOutputFileNoiseFractionR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseFractionR* TheDebayerOutputFileNoiseFractionRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseFractionG : public MetaDouble
{
public:

   DebayerOutputFileNoiseFractionG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseFractionG* TheDebayerOutputFileNoiseFractionGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseFractionB : public MetaDouble
{
public:

   DebayerOutputFileNoiseFractionB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseFractionB* TheDebayerOutputFileNoiseFractionBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseScaleLowR : public MetaDouble
{
public:

   DebayerOutputFileNoiseScaleLowR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseScaleLowR* TheDebayerOutputFileNoiseScaleLowRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseScaleLowG : public MetaDouble
{
public:

   DebayerOutputFileNoiseScaleLowG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseScaleLowG* TheDebayerOutputFileNoiseScaleLowGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseScaleLowB : public MetaDouble
{
public:

   DebayerOutputFileNoiseScaleLowB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseScaleLowB* TheDebayerOutputFileNoiseScaleLowBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseScaleHighR : public MetaDouble
{
public:

   DebayerOutputFileNoiseScaleHighR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseScaleHighR* TheDebayerOutputFileNoiseScaleHighRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseScaleHighG : public MetaDouble
{
public:

   DebayerOutputFileNoiseScaleHighG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseScaleHighG* TheDebayerOutputFileNoiseScaleHighGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseScaleHighB : public MetaDouble
{
public:

   DebayerOutputFileNoiseScaleHighB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseScaleHighB* TheDebayerOutputFileNoiseScaleHighBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseAlgorithmR : public MetaString
{
public:

   DebayerOutputFileNoiseAlgorithmR( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseAlgorithmR* TheDebayerOutputFileNoiseAlgorithmRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseAlgorithmG : public MetaString
{
public:

   DebayerOutputFileNoiseAlgorithmG( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseAlgorithmG* TheDebayerOutputFileNoiseAlgorithmGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseAlgorithmB : public MetaString
{
public:

   DebayerOutputFileNoiseAlgorithmB( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseAlgorithmB* TheDebayerOutputFileNoiseAlgorithmBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __DebayerParameters_h

// ----------------------------------------------------------------------------
// EOF DebayerParameters.h - Released 2021-11-25T11:45:24Z
