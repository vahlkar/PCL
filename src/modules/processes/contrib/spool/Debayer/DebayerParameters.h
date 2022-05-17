//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.29
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 1.11.0
// ----------------------------------------------------------------------------
// DebayerParameters.h - Released 2022-05-17T17:15:11Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
//
// Copyright (c) 2003-2022 Pleiades Astrophoto S.L. All Rights Reserved.
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
          NStar,
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

class DebayerSaturationThreshold : public MetaFloat
{
public:

   DebayerSaturationThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerSaturationThreshold* TheDebayerSaturationThresholdParameter;

// ----------------------------------------------------------------------------

class DebayerSaturationRelative : public MetaBoolean
{
public:

   DebayerSaturationRelative( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerSaturationRelative* TheDebayerSaturationRelativeParameter;

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
          Moffat15,
          Moffat4,
          Moffat6,
          Moffat8,
          MoffatA,
          Auto,
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

extern DebayerPSFType* TheDebayerPSFTypeParameter;

// ----------------------------------------------------------------------------

class DebayerPSFGrowth : public MetaFloat
{
public:

   DebayerPSFGrowth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerPSFGrowth* TheDebayerPSFGrowthParameter;

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

class DebayerPSFTotalFluxEstimateR : public MetaDouble
{
public:

   DebayerPSFTotalFluxEstimateR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalFluxEstimateR* TheDebayerPSFTotalFluxEstimateRParameter;


// ----------------------------------------------------------------------------

class DebayerPSFTotalFluxEstimateG : public MetaDouble
{
public:

   DebayerPSFTotalFluxEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalFluxEstimateG* TheDebayerPSFTotalFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerPSFTotalFluxEstimateB : public MetaDouble
{
public:

   DebayerPSFTotalFluxEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalFluxEstimateB* TheDebayerPSFTotalFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerPSFTotalPowerFluxEstimateR : public MetaDouble
{
public:

   DebayerPSFTotalPowerFluxEstimateR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalPowerFluxEstimateR* TheDebayerPSFTotalPowerFluxEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerPSFTotalPowerFluxEstimateG : public MetaDouble
{
public:

   DebayerPSFTotalPowerFluxEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalPowerFluxEstimateG* TheDebayerPSFTotalPowerFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerPSFTotalPowerFluxEstimateB : public MetaDouble
{
public:

   DebayerPSFTotalPowerFluxEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalPowerFluxEstimateB* TheDebayerPSFTotalPowerFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerPSFTotalMeanFluxEstimateR : public MetaDouble
{
public:

   DebayerPSFTotalMeanFluxEstimateR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalMeanFluxEstimateR* TheDebayerPSFTotalMeanFluxEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerPSFTotalMeanFluxEstimateG : public MetaDouble
{
public:

   DebayerPSFTotalMeanFluxEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalMeanFluxEstimateG* TheDebayerPSFTotalMeanFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerPSFTotalMeanFluxEstimateB : public MetaDouble
{
public:

   DebayerPSFTotalMeanFluxEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalMeanFluxEstimateB* TheDebayerPSFTotalMeanFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerPSFTotalMeanPowerFluxEstimateR : public MetaDouble
{
public:

   DebayerPSFTotalMeanPowerFluxEstimateR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalMeanPowerFluxEstimateR* TheDebayerPSFTotalMeanPowerFluxEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerPSFTotalMeanPowerFluxEstimateG : public MetaDouble
{
public:

   DebayerPSFTotalMeanPowerFluxEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalMeanPowerFluxEstimateG* TheDebayerPSFTotalMeanPowerFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerPSFTotalMeanPowerFluxEstimateB : public MetaDouble
{
public:

   DebayerPSFTotalMeanPowerFluxEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFTotalMeanPowerFluxEstimateB* TheDebayerPSFTotalMeanPowerFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerPSFMStarEstimateR : public MetaDouble
{
public:

   DebayerPSFMStarEstimateR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFMStarEstimateR* TheDebayerPSFMStarEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerPSFMStarEstimateG : public MetaDouble
{
public:

   DebayerPSFMStarEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFMStarEstimateG* TheDebayerPSFMStarEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerPSFMStarEstimateB : public MetaDouble
{
public:

   DebayerPSFMStarEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFMStarEstimateB* TheDebayerPSFMStarEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerPSFNStarEstimateR : public MetaDouble
{
public:

   DebayerPSFNStarEstimateR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFNStarEstimateR* TheDebayerPSFNStarEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerPSFNStarEstimateG : public MetaDouble
{
public:

   DebayerPSFNStarEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFNStarEstimateG* TheDebayerPSFNStarEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerPSFNStarEstimateB : public MetaDouble
{
public:

   DebayerPSFNStarEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerPSFNStarEstimateB* TheDebayerPSFNStarEstimateBParameter;

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

class DebayerOutputFilePSFTotalFluxEstimateR : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalFluxEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalFluxEstimateR* TheDebayerOutputFilePSFTotalFluxEstimateRParameter;


// ----------------------------------------------------------------------------

class DebayerOutputFilePSFTotalFluxEstimateG : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalFluxEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalFluxEstimateG* TheDebayerOutputFilePSFTotalFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFTotalFluxEstimateB : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalFluxEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalFluxEstimateB* TheDebayerOutputFilePSFTotalFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFTotalPowerFluxEstimateR : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalPowerFluxEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalPowerFluxEstimateR* TheDebayerOutputFilePSFTotalPowerFluxEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFTotalPowerFluxEstimateG : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalPowerFluxEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalPowerFluxEstimateG* TheDebayerOutputFilePSFTotalPowerFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFTotalPowerFluxEstimateB : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalPowerFluxEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalPowerFluxEstimateB* TheDebayerOutputFilePSFTotalPowerFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFTotalMeanFluxEstimateR : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalMeanFluxEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalMeanFluxEstimateR* TheDebayerOutputFilePSFTotalMeanFluxEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFTotalMeanFluxEstimateG : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalMeanFluxEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalMeanFluxEstimateG* TheDebayerOutputFilePSFTotalMeanFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFTotalMeanFluxEstimateB : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalMeanFluxEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalMeanFluxEstimateB* TheDebayerOutputFilePSFTotalMeanFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFTotalMeanPowerFluxEstimateR : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalMeanPowerFluxEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalMeanPowerFluxEstimateR* TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFTotalMeanPowerFluxEstimateG : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalMeanPowerFluxEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalMeanPowerFluxEstimateG* TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFTotalMeanPowerFluxEstimateB : public MetaDouble
{
public:

   DebayerOutputFilePSFTotalMeanPowerFluxEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFTotalMeanPowerFluxEstimateB* TheDebayerOutputFilePSFTotalMeanPowerFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFMStarEstimateR : public MetaDouble
{
public:

   DebayerOutputFilePSFMStarEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFMStarEstimateR* TheDebayerOutputFilePSFMStarEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFMStarEstimateG : public MetaDouble
{
public:

   DebayerOutputFilePSFMStarEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFMStarEstimateG* TheDebayerOutputFilePSFMStarEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFMStarEstimateB : public MetaDouble
{
public:

   DebayerOutputFilePSFMStarEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFMStarEstimateB* TheDebayerOutputFilePSFMStarEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFNStarEstimateR : public MetaDouble
{
public:

   DebayerOutputFilePSFNStarEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFNStarEstimateR* TheDebayerOutputFilePSFNStarEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFNStarEstimateG : public MetaDouble
{
public:

   DebayerOutputFilePSFNStarEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFNStarEstimateG* TheDebayerOutputFilePSFNStarEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePSFNStarEstimateB : public MetaDouble
{
public:

   DebayerOutputFilePSFNStarEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePSFNStarEstimateB* TheDebayerOutputFilePSFNStarEstimateBParameter;

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
// EOF DebayerParameters.h - Released 2022-05-17T17:15:11Z
