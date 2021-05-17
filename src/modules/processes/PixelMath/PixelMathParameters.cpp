//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// PixelMathParameters.cpp - Released 2021-05-05T15:38:07Z
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

#include "PixelMathParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

PMExpression0*            ThePMExpression0Parameter = nullptr;
PMExpression1*            ThePMExpression1Parameter = nullptr;
PMExpression2*            ThePMExpression2Parameter = nullptr;
PMExpression3*            ThePMExpression3Parameter = nullptr;
PMUseSingleExpression*    ThePMUseSingleExpressionParameter = nullptr;
PMSymbols*                ThePMSymbolsParameter = nullptr;
PMClearImageCacheAndExit* ThePMClearImageCacheAndExitParameter = nullptr;
PMCacheGeneratedImages*   ThePMCacheGeneratedImagesParameter = nullptr;
PMGenerateOutput*         ThePMGenerateOutputParameter = nullptr;
PMSingleThreaded*         ThePMSingleThreadedParameter = nullptr;
PMOptimization*           ThePMOptimizationParameter = nullptr;
PMUse64BitWorkingImage*   ThePMUse64BitWorkingImageParameter = nullptr;
PMRescaleResult*          ThePMRescaleResultParameter = nullptr;
PMRescaleLowerBound*      ThePMRescaleLowerBoundParameter = nullptr;
PMRescaleUpperBound*      ThePMRescaleUpperBoundParameter = nullptr;
PMTruncateResult*         ThePMTruncateResultParameter = nullptr;
PMTruncateLowerBound*     ThePMTruncateLowerBoundParameter = nullptr;
PMTruncateUpperBound*     ThePMTruncateUpperBoundParameter = nullptr;
PMCreateNewImage*         ThePMCreateNewImageParameter = nullptr;
PMShowNewImage*           ThePMShowNewImageParameter = nullptr;
PMNewImageIdentifier*     ThePMNewImageIdentifierParameter = nullptr;
PMNewImageWidth*          ThePMNewImageWidthParameter = nullptr;
PMNewImageHeight*         ThePMNewImageHeightParameter = nullptr;
PMNewImageAlpha*          ThePMNewImageAlphaParameter = nullptr;
PMNewImageColorSpace*     ThePMNewImageColorSpaceParameter = nullptr;
PMNewImageSampleFormat*   ThePMNewImageSampleFormatParameter = nullptr;
PMOutputData*             ThePMOutputDataParameter = nullptr;
PMOutputGlobalVariableId* ThePMOutputGlobalVariableIdParameter = nullptr;
PMOutputGlobalVariableRK* ThePMOutputGlobalVariableRKParameter = nullptr;
PMOutputGlobalVariableG*  ThePMOutputGlobalVariableGParameter = nullptr;
PMOutputGlobalVariableB*  ThePMOutputGlobalVariableBParameter = nullptr;

// ----------------------------------------------------------------------------

PMExpression0::PMExpression0( MetaProcess* P ) : MetaString( P )
{
   ThePMExpression0Parameter = this;
}

IsoString PMExpression0::Id() const
{
   return "expression";
}

IsoString PMExpression0::Aliases() const
{
   return "expression0";
}

PMExpression1::PMExpression1( MetaProcess* P ) : MetaString( P )
{
   ThePMExpression1Parameter = this;
}

IsoString PMExpression1::Id() const
{
   return "expression1";
}

PMExpression2::PMExpression2( MetaProcess* P ) : MetaString( P )
{
   ThePMExpression2Parameter = this;
}

IsoString PMExpression2::Id() const
{
   return "expression2";
}

PMExpression3::PMExpression3( MetaProcess* P ) : MetaString( P )
{
   ThePMExpression3Parameter = this;
}

IsoString PMExpression3::Id() const
{
   return "expression3";
}

// ----------------------------------------------------------------------------

PMUseSingleExpression::PMUseSingleExpression( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMUseSingleExpressionParameter = this;
}

IsoString PMUseSingleExpression::Id() const
{
   return "useSingleExpression";
}

bool PMUseSingleExpression::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PMSymbols::PMSymbols( MetaProcess* P ) : MetaString( P )
{
   ThePMSymbolsParameter = this;
}

IsoString PMSymbols::Id() const
{
   return "symbols";
}

IsoString PMSymbols::Aliases() const
{
   return "variables"; // Be compatible with very old versions - TODO: remove
}

// ----------------------------------------------------------------------------

PMClearImageCacheAndExit::PMClearImageCacheAndExit( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMClearImageCacheAndExitParameter = this;
}

IsoString PMClearImageCacheAndExit::Id() const
{
   return "clearImageCacheAndExit";
}

bool PMClearImageCacheAndExit::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PMCacheGeneratedImages::PMCacheGeneratedImages( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMCacheGeneratedImagesParameter = this;
}

IsoString PMCacheGeneratedImages::Id() const
{
   return "cacheGeneratedImages";
}

bool PMCacheGeneratedImages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PMGenerateOutput::PMGenerateOutput( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMGenerateOutputParameter = this;
}

IsoString PMGenerateOutput::Id() const
{
   return "generateOutput";
}

bool PMGenerateOutput::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PMSingleThreaded::PMSingleThreaded( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMSingleThreadedParameter = this;
}

IsoString PMSingleThreaded::Id() const
{
   return "singleThreaded";
}

bool PMSingleThreaded::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PMOptimization::PMOptimization( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMOptimizationParameter = this;
}

IsoString PMOptimization::Id() const
{
   return "optimization";
}

bool PMOptimization::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PMUse64BitWorkingImage::PMUse64BitWorkingImage( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMUse64BitWorkingImageParameter = this;
}

IsoString PMUse64BitWorkingImage::Id() const
{
   return "use64BitWorkingImage";
}

bool PMUse64BitWorkingImage::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PMRescaleResult::PMRescaleResult( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMRescaleResultParameter = this;
}

IsoString PMRescaleResult::Id() const
{
   return "rescale";
}

bool PMRescaleResult::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PMRescaleLowerBound::PMRescaleLowerBound( MetaProcess* P ) : MetaDouble( P )
{
   ThePMRescaleLowerBoundParameter = this;
}

IsoString PMRescaleLowerBound::Id() const
{
   return "rescaleLower";
}

int PMRescaleLowerBound::Precision() const
{
   return -16;
}

double PMRescaleLowerBound::DefaultValue() const
{
   return 0;
}

double PMRescaleLowerBound::MinimumValue() const
{
   return 0;
}

double PMRescaleLowerBound::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

PMRescaleUpperBound::PMRescaleUpperBound( MetaProcess* P ) : MetaDouble( P )
{
   ThePMRescaleUpperBoundParameter = this;
}

IsoString PMRescaleUpperBound::Id() const
{
   return "rescaleUpper";
}

int PMRescaleUpperBound::Precision() const
{
   return -16;
}

double PMRescaleUpperBound::DefaultValue() const
{
   return 1;
}

double PMRescaleUpperBound::MinimumValue() const
{
   return 0;
}

double PMRescaleUpperBound::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

PMTruncateResult::PMTruncateResult( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMTruncateResultParameter = this;
}

IsoString PMTruncateResult::Id() const
{
   return "truncate";
}

bool PMTruncateResult::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PMTruncateLowerBound::PMTruncateLowerBound( MetaProcess* P ) : MetaDouble( P )
{
   ThePMTruncateLowerBoundParameter = this;
}

IsoString PMTruncateLowerBound::Id() const
{
   return "truncateLower";
}

int PMTruncateLowerBound::Precision() const
{
   return -16;
}

double PMTruncateLowerBound::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

PMTruncateUpperBound::PMTruncateUpperBound( MetaProcess* P ) : MetaDouble( P )
{
   ThePMTruncateUpperBoundParameter = this;
}

IsoString PMTruncateUpperBound::Id() const
{
   return "truncateUpper";
}

int PMTruncateUpperBound::Precision() const
{
   return -16;
}

double PMTruncateUpperBound::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

PMCreateNewImage::PMCreateNewImage( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMCreateNewImageParameter = this;
}

IsoString PMCreateNewImage::Id() const
{
   return "createNewImage";
}

bool PMCreateNewImage::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PMShowNewImage::PMShowNewImage( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMShowNewImageParameter = this;
}

IsoString PMShowNewImage::Id() const
{
   return "showNewImage";
}

bool PMShowNewImage::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PMNewImageIdentifier::PMNewImageIdentifier( MetaProcess* P ) : MetaString( P )
{
   ThePMNewImageIdentifierParameter = this;
}

IsoString PMNewImageIdentifier::Id() const
{
   return "newImageId";
}

size_type PMNewImageIdentifier::MinLength() const
{
   return 0; // can be void
}

String PMNewImageIdentifier::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

PMNewImageWidth::PMNewImageWidth( MetaProcess* P ) : MetaInt32( P )
{
   ThePMNewImageWidthParameter = this;
}

IsoString PMNewImageWidth::Id() const
{
   return "newImageWidth";
}

double PMNewImageWidth::DefaultValue() const
{
   return 0;   // 0 = same as target
}

double PMNewImageWidth::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

PMNewImageHeight::PMNewImageHeight( MetaProcess* P ) : MetaInt32( P )
{
   ThePMNewImageHeightParameter = this;
}

IsoString PMNewImageHeight::Id() const
{
   return "newImageHeight";
}

double PMNewImageHeight::DefaultValue() const
{
   return 0;   // 0 = same as target
}

double PMNewImageHeight::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

PMNewImageAlpha::PMNewImageAlpha( MetaProcess* P ) : MetaBoolean( P )
{
   ThePMNewImageAlphaParameter = this;
}

IsoString PMNewImageAlpha::Id() const
{
   return "newImageAlpha";
}

bool PMNewImageAlpha::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PMNewImageColorSpace::PMNewImageColorSpace( MetaProcess* P ) : MetaEnumeration( P )
{
   ThePMNewImageColorSpaceParameter = this;
}

IsoString PMNewImageColorSpace::Id() const
{
   return "newImageColorSpace";
}

size_type PMNewImageColorSpace::NumberOfElements() const
{
   return NumberOfColorSpaces;
}

IsoString PMNewImageColorSpace::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case SameAsTarget:   return "SameAsTarget";
   case RGB:            return "RGB";
   case Gray:           return "Gray";
   }
}

int PMNewImageColorSpace::ElementValue( size_type i ) const
{
   return int( i );
}

size_type PMNewImageColorSpace::DefaultValueIndex() const
{
   return size_type( SameAsTarget );
}

// ----------------------------------------------------------------------------

PMNewImageSampleFormat::PMNewImageSampleFormat( MetaProcess* P ) : MetaEnumeration( P )
{
   ThePMNewImageSampleFormatParameter = this;
}

IsoString PMNewImageSampleFormat::Id() const
{
   return "newImageSampleFormat";
}

size_type PMNewImageSampleFormat::NumberOfElements() const
{
   return NumberOfSampleFormats;
}

IsoString PMNewImageSampleFormat::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case SameAsTarget:   return "SameAsTarget";
   case I8:             return "i8";
   case I16:            return "i16";
   case I32:            return "i32";
   case F32:            return "f32";
   case F64:            return "f64";
   }
}

int PMNewImageSampleFormat::ElementValue( size_type i ) const
{
   return int( i );
}

size_type PMNewImageSampleFormat::DefaultValueIndex() const
{
   return size_type( SameAsTarget );
}

// ----------------------------------------------------------------------------

PMOutputData::PMOutputData( MetaProcess* P ) : MetaTable( P )
{
   ThePMOutputDataParameter = this;
}

IsoString PMOutputData::Id() const
{
   return "outputData";
}

bool PMOutputData::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

PMOutputGlobalVariableId::PMOutputGlobalVariableId( MetaTable* T ) : MetaString( T )
{
   ThePMOutputGlobalVariableIdParameter = this;
}

IsoString PMOutputGlobalVariableId::Id() const
{
   return "globalVariableId";
}

bool PMOutputGlobalVariableId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

PMOutputGlobalVariableRK::PMOutputGlobalVariableRK( MetaTable* T ) : MetaDouble( T )
{
   ThePMOutputGlobalVariableRKParameter = this;
}

IsoString PMOutputGlobalVariableRK::Id() const
{
   return "globalVariableRK";
}

bool PMOutputGlobalVariableRK::IsReadOnly() const
{
   return true;
}

int PMOutputGlobalVariableRK::Precision() const
{
   return -16;
}

// ----------------------------------------------------------------------------

PMOutputGlobalVariableG::PMOutputGlobalVariableG( MetaTable* T ) : MetaDouble( T )
{
   ThePMOutputGlobalVariableGParameter = this;
}

IsoString PMOutputGlobalVariableG::Id() const
{
   return "globalVariableG";
}

bool PMOutputGlobalVariableG::IsReadOnly() const
{
   return true;
}

int PMOutputGlobalVariableG::Precision() const
{
   return -16;
}

// ----------------------------------------------------------------------------

PMOutputGlobalVariableB::PMOutputGlobalVariableB( MetaTable* T ) : MetaDouble( T )
{
   ThePMOutputGlobalVariableBParameter = this;
}

IsoString PMOutputGlobalVariableB::Id() const
{
   return "globalVariableB";
}

bool PMOutputGlobalVariableB::IsReadOnly() const
{
   return true;
}

int PMOutputGlobalVariableB::Precision() const
{
   return -16;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PixelMathParameters.cpp - Released 2021-05-05T15:38:07Z
