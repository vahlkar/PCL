//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.5.1
// ----------------------------------------------------------------------------
// LocalNormalizationParameters.h - Released 2021-04-09T19:41:48Z
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

#ifndef __LocalNormalizationParameters_h
#define __LocalNormalizationParameters_h

#include <pcl/MetaParameter.h>

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

class LNRejection : public MetaBoolean
{
public:

   LNRejection( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNRejection* TheLNRejectionParameter;

// ----------------------------------------------------------------------------

class LNBackgroundRejectionLimit : public MetaFloat
{
public:

   LNBackgroundRejectionLimit( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LNBackgroundRejectionLimit* TheLNBackgroundRejectionLimitParameter;

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

class LNShowBackgroundModels : public MetaBoolean
{
public:

   LNShowBackgroundModels( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LNShowBackgroundModels* TheLNShowBackgroundModelsParameter;

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

class LNPlotNormalizationFunctions : public MetaEnumeration
{
public:

   enum { DontPlot,
          Line3D,
          Palette3D,
          Map3D,
          NumberOfModes,
          Default = Palette3D };

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
          NumberOfErrorPolicies,
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

PCL_END_LOCAL

} // pcl

#endif   // __LocalNormalizationParameters_h

// ----------------------------------------------------------------------------
// EOF LocalNormalizationParameters.h - Released 2021-04-09T19:41:48Z
