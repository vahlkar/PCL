//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// PixelMathParameters.h - Released 2021-05-05T15:38:07Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
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

#ifndef __PixelMathParameters_h
#define __PixelMathParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class PMExpression0 : public MetaString
{
public:

   PMExpression0( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
};

extern PMExpression0* ThePMExpression0Parameter;

// ----------------------------------------------------------------------------

class PMExpression1 : public MetaString
{
public:

   PMExpression1( MetaProcess* );

   IsoString Id() const override;
};

extern PMExpression1* ThePMExpression1Parameter;

// ----------------------------------------------------------------------------

class PMExpression2 : public MetaString
{
public:

   PMExpression2( MetaProcess* );

   IsoString Id() const override;
};

extern PMExpression2* ThePMExpression2Parameter;

// ----------------------------------------------------------------------------

class PMExpression3 : public MetaString
{
public:

   PMExpression3( MetaProcess* );

   IsoString Id() const override;
};

extern PMExpression3* ThePMExpression3Parameter;

// ----------------------------------------------------------------------------

class PMUseSingleExpression : public MetaBoolean
{
public:

   PMUseSingleExpression( MetaProcess* );

   IsoString Id() const override;

   bool DefaultValue() const override;
};

extern PMUseSingleExpression* ThePMUseSingleExpressionParameter;

// ----------------------------------------------------------------------------

class PMSymbols : public MetaString
{
public:

   PMSymbols( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
};

extern PMSymbols* ThePMSymbolsParameter;

// ----------------------------------------------------------------------------

class PMClearImageCacheAndExit : public MetaBoolean
{
public:

   PMClearImageCacheAndExit( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PMClearImageCacheAndExit* ThePMClearImageCacheAndExitParameter;

// ----------------------------------------------------------------------------

class PMCacheGeneratedImages : public MetaBoolean
{
public:

   PMCacheGeneratedImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PMCacheGeneratedImages* ThePMCacheGeneratedImagesParameter;

// ----------------------------------------------------------------------------

class PMGenerateOutput : public MetaBoolean
{
public:

   PMGenerateOutput( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PMGenerateOutput* ThePMGenerateOutputParameter;

// ----------------------------------------------------------------------------

class PMSingleThreaded : public MetaBoolean
{
public:

   PMSingleThreaded( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PMSingleThreaded* ThePMSingleThreadedParameter;

// ----------------------------------------------------------------------------

class PMOptimization : public MetaBoolean
{
public:

   PMOptimization( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PMOptimization* ThePMOptimizationParameter;

// ----------------------------------------------------------------------------

class PMUse64BitWorkingImage : public MetaBoolean
{
public:

   PMUse64BitWorkingImage( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PMUse64BitWorkingImage* ThePMUse64BitWorkingImageParameter;

// ----------------------------------------------------------------------------

class PMRescaleResult : public MetaBoolean
{
public:

   PMRescaleResult( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PMRescaleResult* ThePMRescaleResultParameter;

// ----------------------------------------------------------------------------

class PMRescaleLowerBound : public MetaDouble
{
public:

   PMRescaleLowerBound( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PMRescaleLowerBound* ThePMRescaleLowerBoundParameter;

// ----------------------------------------------------------------------------

class PMRescaleUpperBound : public MetaDouble
{
public:

   PMRescaleUpperBound( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PMRescaleUpperBound* ThePMRescaleUpperBoundParameter;

// ----------------------------------------------------------------------------

class PMTruncateResult : public MetaBoolean
{
public:

   PMTruncateResult( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PMTruncateResult* ThePMTruncateResultParameter;

// ----------------------------------------------------------------------------

class PMTruncateLowerBound : public MetaDouble
{
public:

   PMTruncateLowerBound( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PMTruncateLowerBound* ThePMTruncateLowerBoundParameter;

// ----------------------------------------------------------------------------

class PMTruncateUpperBound : public MetaDouble
{
public:

   PMTruncateUpperBound( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PMTruncateUpperBound* ThePMTruncateUpperBoundParameter;

// ----------------------------------------------------------------------------

class PMCreateNewImage : public MetaBoolean
{
public:

   PMCreateNewImage( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PMCreateNewImage* ThePMCreateNewImageParameter;

// ----------------------------------------------------------------------------

class PMShowNewImage : public MetaBoolean
{
public:

   PMShowNewImage( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PMShowNewImage* ThePMShowNewImageParameter;

// ----------------------------------------------------------------------------

class PMNewImageIdentifier : public MetaString
{
public:

   PMNewImageIdentifier( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;   // 0 = automatic
   String AllowedCharacters() const override;
};

extern PMNewImageIdentifier* ThePMNewImageIdentifierParameter;

// ----------------------------------------------------------------------------

class PMNewImageWidth : public MetaInt32
{
public:

   PMNewImageWidth( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;   // 0 = same as target
};

extern PMNewImageWidth* ThePMNewImageWidthParameter;

// ----------------------------------------------------------------------------

class PMNewImageHeight : public MetaInt32
{
public:

   PMNewImageHeight( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;   // 0 = same as target
};

extern PMNewImageHeight* ThePMNewImageHeightParameter;

// ----------------------------------------------------------------------------

class PMNewImageAlpha : public MetaBoolean
{
public:

   PMNewImageAlpha( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PMNewImageAlpha* ThePMNewImageAlphaParameter;

// ----------------------------------------------------------------------------

class PMNewImageColorSpace : public MetaEnumeration
{
public:

   enum { SameAsTarget,
          RGB,    // RGB Color
          Gray,   // Grayscale
          NumberOfColorSpaces,
          Default = SameAsTarget };

   PMNewImageColorSpace( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern PMNewImageColorSpace* ThePMNewImageColorSpaceParameter;

// ----------------------------------------------------------------------------

class PMNewImageSampleFormat : public MetaEnumeration
{
public:

   enum { SameAsTarget,
          I8,     // unsigned 8-bit integer
          I16,    // unsigned 16-bit integer
          I32,    // unsigned 32-bit integer
          F32,    // IEEE 32-bit floating point (simple precision)
          F64,    // IEEE 64-bit floating point (double precision)
          NumberOfSampleFormats,
          Default = SameAsTarget };

   PMNewImageSampleFormat( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern PMNewImageSampleFormat* ThePMNewImageSampleFormatParameter;

// ----------------------------------------------------------------------------

class PMOutputData : public MetaTable
{
public:

   PMOutputData( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern PMOutputData* ThePMOutputDataParameter;

// ----------------------------------------------------------------------------

class PMOutputGlobalVariableId : public MetaString
{
public:

   PMOutputGlobalVariableId( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern PMOutputGlobalVariableId* ThePMOutputGlobalVariableIdParameter;

// ----------------------------------------------------------------------------

class PMOutputGlobalVariableRK : public MetaDouble
{
public:

   PMOutputGlobalVariableRK( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
   int Precision() const override;
};

extern PMOutputGlobalVariableRK* ThePMOutputGlobalVariableRKParameter;

// ----------------------------------------------------------------------------

class PMOutputGlobalVariableG : public MetaDouble
{
public:

   PMOutputGlobalVariableG( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
   int Precision() const override;
};

extern PMOutputGlobalVariableG* ThePMOutputGlobalVariableGParameter;

// ----------------------------------------------------------------------------

class PMOutputGlobalVariableB : public MetaDouble
{
public:

   PMOutputGlobalVariableB( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
   int Precision() const override;
};

extern PMOutputGlobalVariableB* ThePMOutputGlobalVariableBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __PixelMathParameters_h

// ----------------------------------------------------------------------------
// EOF PixelMathParameters.h - Released 2021-05-05T15:38:07Z
