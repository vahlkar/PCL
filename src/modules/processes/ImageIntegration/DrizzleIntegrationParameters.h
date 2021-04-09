//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// DrizzleIntegrationParameters.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#ifndef __DrizzleIntegrationParameters_h
#define __DrizzleIntegrationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class DZInputData : public MetaTable
{
public:

   DZInputData( MetaProcess* );

   IsoString Id() const override;
};

extern DZInputData* TheDZInputDataParameter;

// ----------------------------------------------------------------------------

class DZItemEnabled : public MetaBoolean
{
public:

   DZItemEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZItemEnabled* TheDZItemEnabledParameter;

// ----------------------------------------------------------------------------

class DZItemPath : public MetaString
{
public:

   DZItemPath( MetaTable* );

   IsoString Id() const override;
};

extern DZItemPath* TheDZItemPathParameter;

// ----------------------------------------------------------------------------

class DZLocalNormalizationDataPath : public MetaString
{
public:

   DZLocalNormalizationDataPath( MetaTable* );

   IsoString Id() const override;
};

extern DZLocalNormalizationDataPath* TheDZLocalNormalizationDataPathParameter;

// ----------------------------------------------------------------------------

class DZInputHints : public MetaString
{
public:

   DZInputHints( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern DZInputHints* TheDZInputHintsParameter;

// ----------------------------------------------------------------------------

class DZInputDirectory : public MetaString
{
public:

   DZInputDirectory( MetaProcess* );

   IsoString Id() const override;
};

extern DZInputDirectory* TheDZInputDirectoryParameter;

// ----------------------------------------------------------------------------

class DZScale : public MetaFloat
{
public:

   DZScale( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DZScale* TheDZScaleParameter;

// ----------------------------------------------------------------------------

class DZDropShrink : public MetaFloat
{
public:

   DZDropShrink( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DZDropShrink* TheDZDropShrinkParameter;

// ----------------------------------------------------------------------------

class DZKernelFunction : public MetaEnumeration
{
public:

   enum { Square,
          Circular,
          Gaussian,
          Variable10,
          Variable15,
          Variable30,
          Variable40,
          Variable50,
          Variable60,
          Count,
          Default = Square };

   DZKernelFunction( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static bool IsIntegratedKernel( pcl_enum k )
   {
      return k != Square && k != Circular;
   }
};

extern DZKernelFunction* TheDZKernelFunctionParameter;

// ----------------------------------------------------------------------------

class DZKernelGridSize : public MetaInt32
{
public:

   DZKernelGridSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DZKernelGridSize* TheDZKernelGridSizeParameter;

// ----------------------------------------------------------------------------

class DZOriginX : public MetaFloat
{
public:

   DZOriginX( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DZOriginX* TheDZOriginXParameter;

// ----------------------------------------------------------------------------

class DZOriginY : public MetaFloat
{
public:

   DZOriginY( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DZOriginY* TheDZOriginYParameter;

// ----------------------------------------------------------------------------

class DZEnableCFA : public MetaBoolean
{
public:

   DZEnableCFA( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZEnableCFA* TheDZEnableCFAParameter;

// ----------------------------------------------------------------------------

class DZCFAPattern : public MetaString
{
public:

   DZCFAPattern( MetaProcess* );

   IsoString Id() const override;
};

extern DZCFAPattern* TheDZCFAPatternParameter;

// ----------------------------------------------------------------------------

class DZEnableRejection : public MetaBoolean
{
public:

   DZEnableRejection( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZEnableRejection* TheDZEnableRejectionParameter;

// ----------------------------------------------------------------------------

class DZEnableImageWeighting : public MetaBoolean
{
public:

   DZEnableImageWeighting( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZEnableImageWeighting* TheDZEnableImageWeightingParameter;

// ----------------------------------------------------------------------------

class DZEnableSurfaceSplines : public MetaBoolean
{
public:

   DZEnableSurfaceSplines( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZEnableSurfaceSplines* TheDZEnableSurfaceSplinesParameter;

// ----------------------------------------------------------------------------

class DZEnableLocalDistortion : public MetaBoolean
{
public:

   DZEnableLocalDistortion( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZEnableLocalDistortion* TheDZEnableLocalDistortionParameter;

// ----------------------------------------------------------------------------

class DZEnableLocalNormalization : public MetaBoolean
{
public:

   DZEnableLocalNormalization( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZEnableLocalNormalization* TheDZEnableLocalNormalizationParameter;

// ----------------------------------------------------------------------------

class DZEnableAdaptiveNormalization : public MetaBoolean
{
public:

   DZEnableAdaptiveNormalization( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZEnableAdaptiveNormalization* TheDZEnableAdaptiveNormalizationParameter;

// ----------------------------------------------------------------------------

class DZUseROI : public MetaBoolean
{
public:

   DZUseROI( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZUseROI* TheDZUseROIParameter;

// ----------------------------------------------------------------------------

class DZROIX0 : public MetaInt32
{
public:

   DZROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DZROIX0* TheDZROIX0Parameter;

// ----------------------------------------------------------------------------

class DZROIY0 : public MetaInt32
{
public:

   DZROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DZROIY0* TheDZROIY0Parameter;

// ----------------------------------------------------------------------------

class DZROIX1 : public MetaInt32
{
public:

   DZROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DZROIX1* TheDZROIX1Parameter;

// ----------------------------------------------------------------------------

class DZROIY1 : public MetaInt32
{
public:

   DZROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DZROIY1* TheDZROIY1Parameter;

// ----------------------------------------------------------------------------

class DZClosePreviousImages : public MetaBoolean
{
public:

   DZClosePreviousImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZClosePreviousImages* TheDZClosePreviousImagesParameter;

// ----------------------------------------------------------------------------

class DZTruncateOnOutOfRange : public MetaBoolean
{
public:

   DZTruncateOnOutOfRange( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZTruncateOnOutOfRange* TheDZTruncateOnOutOfRangeParameter;

// ----------------------------------------------------------------------------

class DZNoGUIMessages : public MetaBoolean
{
public:

   DZNoGUIMessages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DZNoGUIMessages* TheDZNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

class DZOnError : public MetaEnumeration
{
public:

   enum { Continue,
          Abort,
          AskUser,
          Count,
          Default = Continue };

   DZOnError( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DZOnError* TheDZOnErrorParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Output properties
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DZIntegrationImageId : public MetaString
{
public:

   DZIntegrationImageId( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZIntegrationImageId* TheDZIntegrationImageIdParameter;

// ----------------------------------------------------------------------------

class DZWeightImageId : public MetaString
{
public:

   DZWeightImageId( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZWeightImageId* TheDZWeightImageIdParameter;

// ----------------------------------------------------------------------------

class DZNumberOfChannels : public MetaInt32
{
public:

   DZNumberOfChannels( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZNumberOfChannels* TheDZNumberOfChannelsParameter;

// ----------------------------------------------------------------------------

class DZOutputPixels : public MetaUInt64
{
public:

   DZOutputPixels( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZOutputPixels* TheDZOutputPixelsParameter;

// ----------------------------------------------------------------------------

class DZIntegratedPixels : public MetaUInt64
{
public:

   DZIntegratedPixels( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZIntegratedPixels* TheDZIntegratedPixelsParameter;

// ----------------------------------------------------------------------------

class DZOutputRangeLow : public MetaDouble
{
public:

   DZOutputRangeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DZOutputRangeLow* TheDZOutputRangeLowParameter;

// ----------------------------------------------------------------------------

class DZOutputRangeHigh : public MetaDouble
{
public:

   DZOutputRangeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DZOutputRangeHigh* TheDZOutputRangeHighParameter;

// ----------------------------------------------------------------------------

class DZTotalRejectedLowRK : public MetaUInt64
{
public:

   DZTotalRejectedLowRK( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZTotalRejectedLowRK* TheDZTotalRejectedLowRKParameter;

class DZTotalRejectedLowG : public MetaUInt64
{
public:

   DZTotalRejectedLowG( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZTotalRejectedLowG* TheDZTotalRejectedLowGParameter;

class DZTotalRejectedLowB : public MetaUInt64
{
public:

   DZTotalRejectedLowB( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZTotalRejectedLowB* TheDZTotalRejectedLowBParameter;

// ----------------------------------------------------------------------------

class DZTotalRejectedHighRK : public MetaUInt64
{
public:

   DZTotalRejectedHighRK( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZTotalRejectedHighRK* TheDZTotalRejectedHighRKParameter;

class DZTotalRejectedHighG : public MetaUInt64
{
public:

   DZTotalRejectedHighG( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZTotalRejectedHighG* TheDZTotalRejectedHighGParameter;

class DZTotalRejectedHighB : public MetaUInt64
{
public:

   DZTotalRejectedHighB( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZTotalRejectedHighB* TheDZTotalRejectedHighBParameter;

// ----------------------------------------------------------------------------

class DZOutputData : public MetaFloat
{
public:

   DZOutputData( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DZOutputData* TheDZOutputDataParameter;

// ----------------------------------------------------------------------------

class DZImageData : public MetaTable
{
public:

   DZImageData( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZImageData* TheDZImageDataParameter;

// ----------------------------------------------------------------------------

class DZImageFilePath : public MetaString
{
public:

   DZImageFilePath( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZImageFilePath* TheDZImageFilePathParameter;

// ----------------------------------------------------------------------------

class DZImageWeightRK : public MetaFloat
{
public:

   DZImageWeightRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DZImageWeightRK* TheDZImageWeightRKParameter;

class DZImageWeightG : public MetaFloat
{
public:

   DZImageWeightG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DZImageWeightG* TheDZImageWeightGParameter;

class DZImageWeightB : public MetaFloat
{
public:

   DZImageWeightB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DZImageWeightB* TheDZImageWeightBParameter;

// ----------------------------------------------------------------------------

class DZImageLocationRK : public MetaDouble
{
public:

   DZImageLocationRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DZImageLocationRK* TheDZImageLocationRKParameter;

class DZImageLocationG : public MetaDouble
{
public:

   DZImageLocationG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DZImageLocationG* TheDZImageLocationGParameter;

class DZImageLocationB : public MetaDouble
{
public:

   DZImageLocationB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DZImageLocationB* TheDZImageLocationBParameter;

// ----------------------------------------------------------------------------

class DZImageReferenceLocationRK : public MetaDouble
{
public:

   DZImageReferenceLocationRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DZImageReferenceLocationRK* TheDZImageReferenceLocationRKParameter;

class DZImageReferenceLocationG : public MetaDouble
{
public:

   DZImageReferenceLocationG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DZImageReferenceLocationG* TheDZImageReferenceLocationGParameter;

class DZImageReferenceLocationB : public MetaDouble
{
public:

   DZImageReferenceLocationB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DZImageReferenceLocationB* TheDZImageReferenceLocationBParameter;

// ----------------------------------------------------------------------------

class DZImageScaleRK : public MetaDouble
{
public:

   DZImageScaleRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DZImageScaleRK* TheDZImageScaleRKParameter;

class DZImageScaleG : public MetaDouble
{
public:

   DZImageScaleG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DZImageScaleG* TheDZImageScaleGParameter;

class DZImageScaleB : public MetaDouble
{
public:

   DZImageScaleB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DZImageScaleB* TheDZImageScaleBParameter;

// ----------------------------------------------------------------------------

class DZImageRejectedLowRK : public MetaUInt64
{
public:

   DZImageRejectedLowRK( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZImageRejectedLowRK* TheDZImageRejectedLowRKParameter;

class DZImageRejectedLowG : public MetaUInt64
{
public:

   DZImageRejectedLowG( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZImageRejectedLowG* TheDZImageRejectedLowGParameter;

class DZImageRejectedLowB : public MetaUInt64
{
public:

   DZImageRejectedLowB( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZImageRejectedLowB* TheDZImageRejectedLowBParameter;

// ----------------------------------------------------------------------------

class DZImageRejectedHighRK : public MetaUInt64
{
public:

   DZImageRejectedHighRK( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZImageRejectedHighRK* TheDZImageRejectedHighRKParameter;

class DZImageRejectedHighG : public MetaUInt64
{
public:

   DZImageRejectedHighG( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZImageRejectedHighG* TheDZImageRejectedHighGParameter;

class DZImageRejectedHighB : public MetaUInt64
{
public:

   DZImageRejectedHighB( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DZImageRejectedHighB* TheDZImageRejectedHighBParameter;

// ----------------------------------------------------------------------------

class DZImageOutputData : public MetaFloat
{
public:

   DZImageOutputData( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DZImageOutputData* TheDZImageOutputDataParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __DrizzleIntegrationParameters_h

// ----------------------------------------------------------------------------
// EOF DrizzleIntegrationParameters.h - Released 2021-04-09T19:41:48Z
