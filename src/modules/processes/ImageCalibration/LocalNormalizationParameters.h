//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.9.8
// ----------------------------------------------------------------------------
// LocalNormalizationParameters.h - Released 2023-08-28T15:23:41Z
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

#ifndef __LocalNormalizationParameters_h
#define __LocalNormalizationParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/PSFScaleEstimator.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class LNScale : public MetaInt32
{
public:

   LNScale( MetaProcess* );

   IsoString Id() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LNScale* TheLNScaleParameter;

// ----------------------------------------------------------------------------

class LNNoScale : public MetaBoolean
{
public:

   LNNoScale( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNNoScale* TheLNNoScaleParameter;

// ----------------------------------------------------------------------------

class LNGlobalLocationNormalization : public MetaBoolean
{
public:

   LNGlobalLocationNormalization( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNGlobalLocationNormalization* TheLNGlobalLocationNormalizationParameter;

// ----------------------------------------------------------------------------

class LNTruncate : public MetaBoolean
{
public:

   LNTruncate( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNTruncate* TheLNTruncateParameter;

// ----------------------------------------------------------------------------

class LNBackgroundSamplingDelta : public MetaInt32
{
public:

   LNBackgroundSamplingDelta( MetaProcess* );

   IsoString Id() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LNBackgroundSamplingDelta* TheLNBackgroundSamplingDeltaParameter;

// ----------------------------------------------------------------------------

class LNRejection : public MetaBoolean
{
public:

   LNRejection( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNRejection* TheLNRejectionParameter;

// ----------------------------------------------------------------------------

class LNReferenceRejection : public MetaBoolean
{
public:

   LNReferenceRejection( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNReferenceRejection* TheLNReferenceRejectionParameter;

// ----------------------------------------------------------------------------

class LNLowClippingLevel : public MetaDouble
{
public:

   LNLowClippingLevel( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNLowClippingLevel* TheLNLowClippingLevelParameter;

// ----------------------------------------------------------------------------

class LNHighClippingLevel : public MetaFloat
{
public:

   LNHighClippingLevel( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNHighClippingLevel* TheLNHighClippingLevelParameter;

// ----------------------------------------------------------------------------

class LNReferenceRejectionThreshold : public MetaFloat
{
public:

   LNReferenceRejectionThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNReferenceRejectionThreshold* TheLNReferenceRejectionThresholdParameter;

// ----------------------------------------------------------------------------

class LNTargetRejectionThreshold : public MetaFloat
{
public:

   LNTargetRejectionThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNTargetRejectionThreshold* TheLNTargetRejectionThresholdParameter;

// ----------------------------------------------------------------------------

class LNHotPixelFilterRadius : public MetaInt32
{
public:

   LNHotPixelFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNHotPixelFilterRadius* TheLNHotPixelFilterRadiusParameter;

// ----------------------------------------------------------------------------

class LNNoiseReductionFilterRadius : public MetaInt32
{
public:

   LNNoiseReductionFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNNoiseReductionFilterRadius* TheLNNoiseReductionFilterRadiusParameter;

// ----------------------------------------------------------------------------

class LNModelScalingFactor : public MetaFloat
{
public:

   LNModelScalingFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNModelScalingFactor* TheLNModelScalingFactorParameter;

// ----------------------------------------------------------------------------

class LNScaleEvaluationMethod : public MetaEnumeration
{
public:

   enum { PSFSignal,
          MultiscaleAnalysis,
          NumberOfItems,
          Default = PSFSignal };

   LNScaleEvaluationMethod( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern LNScaleEvaluationMethod* TheLNScaleEvaluationMethodParameter;

// ----------------------------------------------------------------------------

class LNLocalScaleCorrections : public MetaBoolean
{
public:

   LNLocalScaleCorrections( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNLocalScaleCorrections* TheLNLocalScaleCorrectionsParameter;

// ----------------------------------------------------------------------------

class LNStructureLayers : public MetaInt32
{
public:

   LNStructureLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNStructureLayers* TheLNStructureLayersParameter;

// ----------------------------------------------------------------------------

class LNSaturationThreshold : public MetaFloat
{
public:

   LNSaturationThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNSaturationThreshold* TheLNSaturationThresholdParameter;

// ----------------------------------------------------------------------------

class LNSaturationRelative : public MetaBoolean
{
public:

   LNSaturationRelative( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNSaturationRelative* TheLNSaturationRelativeParameter;

// ----------------------------------------------------------------------------

class LNRejectionLimit : public MetaFloat
{
public:

   LNRejectionLimit( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNRejectionLimit* TheLNRejectionLimitParameter;

// ----------------------------------------------------------------------------

class LNPSFNoiseLayers : public MetaInt32
{
public:

   LNPSFNoiseLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNPSFNoiseLayers* TheLNPSFNoiseLayersParameter;

// ----------------------------------------------------------------------------

class LNPSFHotPixelFilterRadius : public MetaInt32
{
public:

   LNPSFHotPixelFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNPSFHotPixelFilterRadius* TheLNPSFHotPixelFilterRadiusParameter;

// ----------------------------------------------------------------------------

class LNPSFNoiseReductionFilterRadius : public MetaInt32
{
public:

   LNPSFNoiseReductionFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNPSFNoiseReductionFilterRadius* TheLNPSFNoiseReductionFilterRadiusParameter;

// ----------------------------------------------------------------------------

class LNPSFMinStructureSize : public MetaInt32
{
public:

   LNPSFMinStructureSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNPSFMinStructureSize* TheLNPSFMinStructureSizeParameter;

// ----------------------------------------------------------------------------

class LNPSFMinSNR : public MetaFloat
{
public:

   LNPSFMinSNR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNPSFMinSNR* TheLNPSFMinSNRParameter;


// ----------------------------------------------------------------------------

class LNPSFAllowClusteredSources : public MetaBoolean
{
public:

   LNPSFAllowClusteredSources( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNPSFAllowClusteredSources* TheLNPSFAllowClusteredSourcesParameter;

// ----------------------------------------------------------------------------

class LNPSFType : public MetaEnumeration
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
          Default = Auto };

   LNPSFType( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static PSFScaleEstimator::psf_function ToPSFFunction( pcl_enum x )
   {
      switch ( x )
      {
      case Gaussian: return PSFunction::Gaussian;
      case Moffat15: return PSFunction::Moffat15;
      case Moffat4:  return PSFunction::Moffat4;
      case Moffat6:  return PSFunction::Moffat6;
      case Moffat8:  return PSFunction::Moffat8;
      case MoffatA:  return PSFunction::MoffatA;
      default:
      case Auto:       return PSFunction::Auto;
      }
   }

   static IsoString FunctionName( pcl_enum );
};

extern LNPSFType* TheLNPSFTypeParameter;

// ----------------------------------------------------------------------------

class LNPSFGrowth : public MetaFloat
{
public:

   LNPSFGrowth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNPSFGrowth* TheLNPSFGrowthParameter;

// ----------------------------------------------------------------------------

class LNPSFMaxStars : public MetaInt32
{
public:

   LNPSFMaxStars( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNPSFMaxStars* TheLNPSFMaxStarsParameter;

// ----------------------------------------------------------------------------

class LNReferencePathOrViewId : public MetaString
{
public:

   LNReferencePathOrViewId( MetaProcess* );

   IsoString Id() const override;
};

extern LNReferencePathOrViewId* TheLNReferencePathOrViewIdParameter;

// ----------------------------------------------------------------------------

class LNReferenceIsView : public MetaBoolean
{
public:

   LNReferenceIsView( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNReferenceIsView* TheLNReferenceIsViewParameter;

// ----------------------------------------------------------------------------

class LNTargetItems : public MetaTable
{
public:

   LNTargetItems( MetaProcess* );

   IsoString Id() const override;
};

extern LNTargetItems* TheLNTargetItemsParameter;

// ----------------------------------------------------------------------------

class LNTargetEnabled : public MetaBoolean
{
public:

   LNTargetEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNTargetEnabled* TheLNTargetEnabledParameter;

// ----------------------------------------------------------------------------

class LNTargetImage : public MetaString
{
public:

   LNTargetImage( MetaTable* );

   IsoString Id() const override;
};

extern LNTargetImage* TheLNTargetImageParameter;

// ----------------------------------------------------------------------------

class LNInputHints : public MetaString
{
public:

   LNInputHints( MetaProcess* );

   IsoString Id() const override;
};

extern LNInputHints* TheLNInputHintsParameter;

// ----------------------------------------------------------------------------

class LNOutputHints : public MetaString
{
public:

   LNOutputHints( MetaProcess* );

   IsoString Id() const override;
};

extern LNOutputHints* TheLNOutputHintsParameter;

// ----------------------------------------------------------------------------

class LNGenerateNormalizedImages : public MetaEnumeration
{
public:

   enum { Never,
          Always,
          ViewExecutionOnly,
          GlobalExecutionOnly,
          NumberOfModes,
          Default = ViewExecutionOnly };

   LNGenerateNormalizedImages( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern LNGenerateNormalizedImages* TheLNGenerateNormalizedImagesParameter;

// ----------------------------------------------------------------------------

class LNGenerateNormalizationData : public MetaBoolean
{
public:

   LNGenerateNormalizationData( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNGenerateNormalizationData* TheLNGenerateNormalizationDataParameter;

// ----------------------------------------------------------------------------

class LNGenerateInvalidData : public MetaBoolean
{
public:

   LNGenerateInvalidData( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNGenerateInvalidData* TheLNGenerateInvalidDataParameter;

// ----------------------------------------------------------------------------

class LNShowBackgroundModels : public MetaBoolean
{
public:

   LNShowBackgroundModels( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNShowBackgroundModels* TheLNShowBackgroundModelsParameter;

// ----------------------------------------------------------------------------

class LNShowLocalScaleModels : public MetaBoolean
{
public:

   LNShowLocalScaleModels( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNShowLocalScaleModels* TheLNShowLocalScaleModelsParameter;

// ----------------------------------------------------------------------------

class LNShowRejectionMaps : public MetaBoolean
{
public:

   LNShowRejectionMaps( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNShowRejectionMaps* TheLNShowRejectionMapsParameter;

// ----------------------------------------------------------------------------

class LNShowStructureMaps : public MetaBoolean
{
public:

   LNShowStructureMaps( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNShowStructureMaps* TheLNShowStructureMapsParameter;

// ----------------------------------------------------------------------------

class LNPlotNormalizationFunctions : public MetaEnumeration
{
public:

   enum { DontPlot,
          Line3D,
          Palette3D,
          Map3D,
          NumberOfModes,
          Default = DontPlot };

   LNPlotNormalizationFunctions( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern LNPlotNormalizationFunctions* TheLNPlotNormalizationFunctionsParameter;

// ----------------------------------------------------------------------------

class LNNoGUIMessages : public MetaBoolean
{
public:

   LNNoGUIMessages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNNoGUIMessages* TheLNNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

class LNAutoMemoryLimit : public MetaFloat
{
public:

   LNAutoMemoryLimit( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNAutoMemoryLimit* TheLNAutoMemoryLimitParameter;

// ----------------------------------------------------------------------------

class LNOutputDirectory : public MetaString
{
public:

   LNOutputDirectory( MetaProcess* );

   IsoString Id() const override;
};

extern LNOutputDirectory* TheLNOutputDirectoryParameter;

// ----------------------------------------------------------------------------

class LNOutputExtension : public MetaString
{
public:

   LNOutputExtension( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern LNOutputExtension* TheLNOutputExtensionParameter;

// ----------------------------------------------------------------------------

class LNOutputPrefix : public MetaString
{
public:

   LNOutputPrefix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern LNOutputPrefix* TheLNOutputPrefixParameter;

// ----------------------------------------------------------------------------

class LNOutputPostfix : public MetaString
{
public:

   LNOutputPostfix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern LNOutputPostfix* TheLNOutputPostfixParameter;

// ----------------------------------------------------------------------------

class LNOverwriteExistingFiles : public MetaBoolean
{
public:

   LNOverwriteExistingFiles( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNOverwriteExistingFiles* TheLNOverwriteExistingFilesParameter;

// ----------------------------------------------------------------------------

class LNOnError : public MetaEnumeration
{
public:

   enum { Continue,
          Abort,
          AskUser,
          NumberOfItems,
          Default = Continue };

   LNOnError( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern LNOnError* TheLNOnErrorParameter;

// ----------------------------------------------------------------------------

class LNUseFileThreads : public MetaBoolean
{
public:

   LNUseFileThreads( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNUseFileThreads* TheLNUseFileThreadsParameter;

// ----------------------------------------------------------------------------

class LNFileThreadOverload : public MetaFloat
{
public:

   LNFileThreadOverload( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNFileThreadOverload* TheLNFileThreadOverloadParameter;

// ----------------------------------------------------------------------------

class LNMaxFileReadThreads : public MetaInt32
{
public:

   LNMaxFileReadThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNMaxFileReadThreads* TheLNMaxFileReadThreadsParameter;

// ----------------------------------------------------------------------------

class LNMaxFileWriteThreads : public MetaInt32
{
public:

   LNMaxFileWriteThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNMaxFileWriteThreads* TheLNMaxFileWriteThreadsParameter;

// ----------------------------------------------------------------------------

class LNGraphSize : public MetaInt32
{
public:

   LNGraphSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNGraphSize* TheLNGraphSizeParameter;

// ----------------------------------------------------------------------------

class LNGraphTextSize : public MetaInt32
{
public:

   LNGraphTextSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNGraphTextSize* TheLNGraphTextSizeParameter;

// ----------------------------------------------------------------------------

class LNGraphTitleSize : public MetaInt32
{
public:

   LNGraphTitleSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNGraphTitleSize* TheLNGraphTitleSizeParameter;

// ----------------------------------------------------------------------------

class LNGraphTransparent : public MetaBoolean
{
public:

   LNGraphTransparent( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNGraphTransparent* TheLNGraphTransparentParameter;

// ----------------------------------------------------------------------------

class LNGraphOutputDirectory : public MetaString
{
public:

   LNGraphOutputDirectory( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern LNGraphOutputDirectory* TheLNGraphOutputDirectoryParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Output properties
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class LNOutputData : public MetaTable
{
public:

   LNOutputData( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern LNOutputData* TheLNOutputDataParameter;

// ----------------------------------------------------------------------------

class LNOutputFilePathXNML : public MetaString
{
public:

   LNOutputFilePathXNML( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern LNOutputFilePathXNML* TheLNOutputFilePathXNMLParameter;

// ----------------------------------------------------------------------------

class LNOutputFilePath : public MetaString
{
public:

   LNOutputFilePath( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern LNOutputFilePath* TheLNOutputFilePathParameter;

// ----------------------------------------------------------------------------

class LNScaleFactorRK : public MetaFloat
{
public:

   LNScaleFactorRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern LNScaleFactorRK* TheLNScaleFactorRKParameter;

// ----------------------------------------------------------------------------

class LNScaleFactorG : public MetaFloat
{
public:

   LNScaleFactorG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern LNScaleFactorG* TheLNScaleFactorGParameter;

// ----------------------------------------------------------------------------

class LNScaleFactorB : public MetaFloat
{
public:

   LNScaleFactorB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern LNScaleFactorB* TheLNScaleFactorBParameter;

// ----------------------------------------------------------------------------

class LNValid : public MetaBoolean
{
public:

   LNValid( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern LNValid* TheLNValidParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __LocalNormalizationParameters_h

// ----------------------------------------------------------------------------
// EOF LocalNormalizationParameters.h - Released 2023-08-28T15:23:41Z
