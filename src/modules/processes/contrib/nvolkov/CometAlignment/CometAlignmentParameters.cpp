//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.3.8
// ----------------------------------------------------------------------------
// CometAlignmentParameters.cpp - Released 2024-12-11T17:43:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2023 Nikolay Volkov
// Copyright (c) 2019-2023 Juan Conejero (PTeam)
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L.
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

#include "CometAlignmentParameters.h"

namespace pcl
{
// ----------------------------------------------------------------------------

CATargetFrames*                  TheCATargetFramesParameter = nullptr;
CATargetFramePath*               TheCATargetFramePathParameter = nullptr;
CATargetFrameEnabled*            TheCATargetFrameEnabledParameter = nullptr;
CATargetFrameDate*               TheCATargetFrameDateParameter = nullptr;
CATargetFrameJDate*              TheCATargetFrameJDateParameter = nullptr;
CATargetFrameX*                  TheCATargetFrameXParameter = nullptr;
CATargetFrameY*                  TheCATargetFrameYParameter = nullptr;
CATargetFrameFixed*              TheCATargetFrameFixedParameter = nullptr;
CATargetFrameDrizzlePath*        TheCATargetFrameDrizzlePathParameter = nullptr;

CAReferenceIndex*                TheCAReferenceIndexParameter = nullptr;
CAFitPSF*                        TheCAFitPSFParameter = nullptr;
CAOperandImageFilePath*          TheCAOperandImageFilePathParameter = nullptr;
CAOperandSubtractAligned*        TheCAOperandSubtractAlignedParameter = nullptr;
CAOperandLinearFit*              TheCAOperandLinearFitParameter = nullptr;
CAOperandLinearFitLow*           TheCAOperandLinearFitLowParameter = nullptr;
CAOperandLinearFitHigh*          TheCAOperandLinearFitHighParameter = nullptr;
CAOperandNormalize*              TheCAOperandNormalizeParameter = nullptr;
CADrizzleWriteStarAlignedImage*  TheCADrizzleWriteStarAlignedImageParameter = nullptr;
CADrizzleWriteCometAlignedImage* TheCADrizzleWriteCometAlignedImageParameter = nullptr;
CAPixelInterpolation*            TheCAPixelInterpolationParameter = nullptr;
CALinearClampingThreshold*       TheCALinearClampingThresholdParameter = nullptr;

CAInputHints*                    TheCAInputHintsParameter = nullptr;
CAOutputHints*                   TheCAOutputHintsParameter = nullptr;

CAOutputDirectory*               TheCAOutputDirectoryParameter = nullptr;
CAOutputExtension*               TheCAOutputExtensionParameter = nullptr;
CAOutputPrefix*                  TheCAOutputPrefixParameter = nullptr;
CAOutputPostfix*                 TheCAOutputPostfixParameter = nullptr;
CAOverwriteExistingFiles*        TheCAOverwriteExistingFilesParameter = nullptr;
CAGenerateCometPathMap*          TheCAGenerateCometPathMapParameter = nullptr;
CAOnError*                       TheCAOnErrorParameter = nullptr;

CAUseFileThreads*                TheCAUseFileThreadsParameter = nullptr;
CAFileThreadOverload*            TheCAFileThreadOverloadParameter = nullptr;
CAMaxFileReadThreads*            TheCAMaxFileReadThreadsParameter = nullptr;
CAMaxFileWriteThreads*           TheCAMaxFileWriteThreadsParameter = nullptr;

// ----------------------------------------------------------------------------

CATargetFrames::CATargetFrames( MetaProcess* P )
   : MetaTable( P )
{
   TheCATargetFramesParameter = this;
}

IsoString CATargetFrames::Id() const
{
   return "targetFrames";
}

// ----------------------------------------------------------------------------

CATargetFramePath::CATargetFramePath( MetaTable* T )
   : MetaString( T )
{
   TheCATargetFramePathParameter = this;
}

IsoString CATargetFramePath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

CATargetFrameEnabled::CATargetFrameEnabled( MetaTable* T )
   : MetaBoolean( T )
{
   TheCATargetFrameEnabledParameter = this;
}

IsoString CATargetFrameEnabled::Id() const
{
   return "enabled";
}

bool CATargetFrameEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CATargetFrameDate::CATargetFrameDate( MetaTable* T ) // ### DEPRECATED
   : MetaString( T )
{
   TheCATargetFrameDateParameter = this;
}

IsoString CATargetFrameDate::Id() const
{
   return "date";
}

// ----------------------------------------------------------------------------

CATargetFrameJDate::CATargetFrameJDate( MetaTable* T )
   : MetaDouble( T )
{
   TheCATargetFrameJDateParameter = this;
}

IsoString CATargetFrameJDate::Id() const
{
   return "jd";
}

IsoString CATargetFrameJDate::Aliases() const
{
   return "jDate";
}

int CATargetFrameJDate::Precision() const
{
   return 15;
}

// ----------------------------------------------------------------------------

CATargetFrameX::CATargetFrameX( MetaTable* T )
   : MetaDouble( T )
{
   TheCATargetFrameXParameter = this;
}

IsoString CATargetFrameX::Id() const
{
   return "x";
}

int CATargetFrameX::Precision() const
{
   return 2;
}

double CATargetFrameX::MinimumValue() const
{
   return 0;
}

double CATargetFrameX::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

CATargetFrameY::CATargetFrameY( MetaTable* T )
   : MetaDouble( T )
{
   TheCATargetFrameYParameter = this;
}

IsoString CATargetFrameY::Id() const
{
   return "y";
}

int CATargetFrameY::Precision() const
{
   return 2;
}

double CATargetFrameY::MinimumValue() const
{
   return 0;
}

double CATargetFrameY::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

CATargetFrameFixed::CATargetFrameFixed( MetaTable* T )
   : MetaBoolean( T )
{
   TheCATargetFrameFixedParameter = this;
}

IsoString CATargetFrameFixed::Id() const
{
   return "fixed";
}

bool CATargetFrameFixed::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CATargetFrameDrizzlePath::CATargetFrameDrizzlePath( MetaTable* T )
   : MetaString( T )
{
   TheCATargetFrameDrizzlePathParameter = this;
}

IsoString CATargetFrameDrizzlePath::Id() const
{
   return "drizzlePath";
}

// ----------------------------------------------------------------------------

CAReferenceIndex::CAReferenceIndex( MetaProcess* P )
   : MetaUInt32( P )
{
   TheCAReferenceIndexParameter = this;
}

IsoString CAReferenceIndex::Id() const
{
   return "referenceIndex";
}

IsoString CAReferenceIndex::Aliases() const
{
   return "reference";
}

// ----------------------------------------------------------------------------

CAFitPSF::CAFitPSF( MetaProcess* P )
   : MetaBoolean( P )
{
   TheCAFitPSFParameter = this;
}

IsoString CAFitPSF::Id() const
{
   return "fitPSF";
}

bool CAFitPSF::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CAOperandImageFilePath::CAOperandImageFilePath( MetaProcess* P )
   : MetaString( P )
{
   TheCAOperandImageFilePathParameter = this;
}

IsoString CAOperandImageFilePath::Id() const
{
   return "operandImageFilePath";
}

IsoString CAOperandImageFilePath::Aliases() const
{
   return "subtractFile";
}

// ----------------------------------------------------------------------------

CAOperandSubtractAligned::CAOperandSubtractAligned( MetaProcess* P )
   : MetaBoolean( P )
{
   TheCAOperandSubtractAlignedParameter = this;
}

IsoString CAOperandSubtractAligned::Id() const
{
   return "operandSubtractAligned";
}

IsoString CAOperandSubtractAligned::Aliases() const
{
   return "subtractMode";
}

bool CAOperandSubtractAligned::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CAOperandLinearFit::CAOperandLinearFit( MetaProcess* P )
   : MetaBoolean( P )
{
   TheCAOperandLinearFitParameter = this;
}

IsoString CAOperandLinearFit::Id() const
{
   return "operandLinearFit";
}

IsoString CAOperandLinearFit::Aliases() const
{
   return "enableLinearFit";
}

bool CAOperandLinearFit::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CAOperandLinearFitLow::CAOperandLinearFitLow( MetaProcess* P )
   : MetaFloat( P )
{
   TheCAOperandLinearFitLowParameter = this;
}

IsoString CAOperandLinearFitLow::Id() const
{
   return "operandLinearFitLow";
}

IsoString CAOperandLinearFitLow::Aliases() const
{
   return "rejectLow";
}

int CAOperandLinearFitLow::Precision() const
{
   return 6;
}

double CAOperandLinearFitLow::MinimumValue() const
{
   return 0;
}

double CAOperandLinearFitLow::MaximumValue() const
{
   return 1;
}

double CAOperandLinearFitLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

CAOperandLinearFitHigh::CAOperandLinearFitHigh( MetaProcess* P )
   : MetaFloat( P )
{
   TheCAOperandLinearFitHighParameter = this;
}

IsoString CAOperandLinearFitHigh::Id() const
{
   return "operandLinearFitHigh";
}

IsoString CAOperandLinearFitHigh::Aliases() const
{
   return "rejectHigh";
}

int CAOperandLinearFitHigh::Precision() const
{
   return 6;
}

double CAOperandLinearFitHigh::MinimumValue() const
{
   return 0;
}

double CAOperandLinearFitHigh::MaximumValue() const
{
   return 1;
}

double CAOperandLinearFitHigh::DefaultValue() const
{
   return 0.92;
}

// ----------------------------------------------------------------------------

CAOperandNormalize::CAOperandNormalize( MetaProcess* P )
   : MetaBoolean( P )
{
   TheCAOperandNormalizeParameter = this;
}

IsoString CAOperandNormalize::Id() const
{
   return "operandNormalize";
}

IsoString CAOperandNormalize::Aliases() const
{
   return "normalize";
}

bool CAOperandNormalize::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CADrizzleWriteStarAlignedImage::CADrizzleWriteStarAlignedImage( MetaProcess* P )
   : MetaBoolean( P )
{
   TheCADrizzleWriteStarAlignedImageParameter = this;
}

IsoString CADrizzleWriteStarAlignedImage::Id() const
{
   return "drizzleWriteStarAlignedImage";
}

IsoString CADrizzleWriteStarAlignedImage::Aliases() const
{
   return "drzSaveStarsAligned";
}

bool CADrizzleWriteStarAlignedImage::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CADrizzleWriteCometAlignedImage::CADrizzleWriteCometAlignedImage( MetaProcess* P )
   : MetaBoolean( P )
{
   TheCADrizzleWriteCometAlignedImageParameter = this;
}

IsoString CADrizzleWriteCometAlignedImage::Id() const
{
   return "drizzleWriteCometAlignedImage";
}

IsoString CADrizzleWriteCometAlignedImage::Aliases() const
{
   return "drzSaveCometAligned";
}

bool CADrizzleWriteCometAlignedImage::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CAPixelInterpolation::CAPixelInterpolation( MetaProcess* p )
   : MetaEnumeration( p )
{
   TheCAPixelInterpolationParameter = this;
}

IsoString CAPixelInterpolation::Id() const
{
   return "pixelInterpolation";
}

size_type CAPixelInterpolation::NumberOfElements() const
{
   return NumberOfInterpolationAlgorithms;
}

IsoString CAPixelInterpolation::ElementId( size_type i ) const
{
   switch ( i )
   {
   case NearestNeighbor:         return "NearestNeighbor";
   case Bilinear:                return "Bilinear";
   case BicubicSpline:           return "BicubicSpline";
   case BicubicBSpline:          return "BicubicBSpline";
   case Lanczos3:                return "Lanczos3";
   default:
   case Lanczos4:                return "Lanczos4";
   case Lanczos5:                return "Lanczos5";
   case MitchellNetravaliFilter: return "MitchellNetravaliFilter";
   case CatmullRomSplineFilter:  return "CatmullRomSplineFilter";
   case CubicBSplineFilter:      return "CubicBSplineFilter";
   case Auto:                    return "Auto";
   }
}

int CAPixelInterpolation::ElementValue( size_type i ) const
{
   return int( i );
}

size_type CAPixelInterpolation::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

CALinearClampingThreshold::CALinearClampingThreshold( MetaProcess* p )
   : MetaFloat( p )
{
   TheCALinearClampingThresholdParameter = this;
}

IsoString CALinearClampingThreshold::Id() const
{
   return "linearClampingThreshold";
}

int CALinearClampingThreshold::Precision() const
{
   return 2;
}

double CALinearClampingThreshold::DefaultValue() const
{
   return 0.3;
} // __PCL_BICUBIC_SPLINE_CLAMPING_THRESHOLD;

double CALinearClampingThreshold::MinimumValue() const
{
   return 0;
}

double CALinearClampingThreshold::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CAInputHints::CAInputHints( MetaProcess* P )
   : MetaString( P )
{
   TheCAInputHintsParameter = this;
}

IsoString CAInputHints::Id() const
{
   return "inputHints";
}

// ----------------------------------------------------------------------------

CAOutputHints::CAOutputHints( MetaProcess* P )
   : MetaString( P )
{
   TheCAOutputHintsParameter = this;
}

IsoString CAOutputHints::Id() const
{
   return "outputHints";
}

// ----------------------------------------------------------------------------

CAOutputDirectory::CAOutputDirectory( MetaProcess* P )
   : MetaString( P )
{
   TheCAOutputDirectoryParameter = this;
}

IsoString CAOutputDirectory::Id() const
{
   return "outputDirectory";
}

IsoString CAOutputDirectory::Aliases() const
{
   return "outputDir";
}

String CAOutputDirectory::DefaultValue() const
{
   return String();
}

// ----------------------------------------------------------------------------

CAOutputExtension::CAOutputExtension( MetaProcess* P )
   : MetaString( P )
{
   TheCAOutputExtensionParameter = this;
}

IsoString CAOutputExtension::Id() const
{
   return "outputExtension";
}

String CAOutputExtension::DefaultValue() const
{
   return ".xisf";
}

// ----------------------------------------------------------------------------

CAOutputPrefix::CAOutputPrefix( MetaProcess* P )
   : MetaString( P )
{
   TheCAOutputPrefixParameter = this;
}

IsoString CAOutputPrefix::Id() const
{
   return "outputPrefix";
}

IsoString CAOutputPrefix::Aliases() const
{
   return "prefix";
}

String CAOutputPrefix::DefaultValue() const
{
   return "";
}

// ----------------------------------------------------------------------------

CAOutputPostfix::CAOutputPostfix( MetaProcess* P )
   : MetaString( P )
{
   TheCAOutputPostfixParameter = this;
}

IsoString CAOutputPostfix::Id() const
{
   return "outputPostfix";
}

IsoString CAOutputPostfix::Aliases() const
{
   return "postfix";
}

String CAOutputPostfix::DefaultValue() const
{
   return "_ca";
}

// ----------------------------------------------------------------------------

CAOverwriteExistingFiles::CAOverwriteExistingFiles( MetaProcess* P )
   : MetaBoolean( P )
{
   TheCAOverwriteExistingFilesParameter = this;
}

IsoString CAOverwriteExistingFiles::Id() const
{
   return "overwriteExistingFiles";
}

IsoString CAOverwriteExistingFiles::Aliases() const
{
   return "overwrite";
}

bool CAOverwriteExistingFiles::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CAGenerateCometPathMap::CAGenerateCometPathMap( MetaProcess* P )
   : MetaBoolean( P )
{
   TheCAGenerateCometPathMapParameter = this;
}

IsoString CAGenerateCometPathMap::Id() const
{
   return "generateCometPathMap";
}

bool CAGenerateCometPathMap::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CAOnError::CAOnError( MetaProcess* p )
   : MetaEnumeration( p )
{
   TheCAOnErrorParameter = this;
}

IsoString CAOnError::Id() const
{
   return "onError";
}

size_type CAOnError::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString CAOnError::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Continue: return "OnError_Continue";
   case Abort:    return "OnError_Abort";
   case AskUser:  return "OnError_AskUser";
   }
}

int CAOnError::ElementValue( size_type i ) const
{
   return int( i );
}

size_type CAOnError::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

CAUseFileThreads::CAUseFileThreads( MetaProcess* p )
   : MetaBoolean( p )
{
   TheCAUseFileThreadsParameter = this;
}

IsoString CAUseFileThreads::Id() const
{
   return "useFileThreads";
}

bool CAUseFileThreads::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CAFileThreadOverload::CAFileThreadOverload( MetaProcess* p )
   : MetaFloat( p )
{
   TheCAFileThreadOverloadParameter = this;
}

IsoString CAFileThreadOverload::Id() const
{
   return "fileThreadOverload";
}

int CAFileThreadOverload::Precision() const
{
   return 2;
}

double CAFileThreadOverload::DefaultValue() const
{
   return 1.0;
}

double CAFileThreadOverload::MinimumValue() const
{
   return 1;
}

double CAFileThreadOverload::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

CAMaxFileReadThreads::CAMaxFileReadThreads( MetaProcess* p )
   : MetaInt32( p )
{
   TheCAMaxFileReadThreadsParameter = this;
}

IsoString CAMaxFileReadThreads::Id() const
{
   return "maxFileReadThreads";
}

double CAMaxFileReadThreads::DefaultValue() const
{
   return 0;
}

double CAMaxFileReadThreads::MinimumValue() const
{
   return 0;
}

double CAMaxFileReadThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

CAMaxFileWriteThreads::CAMaxFileWriteThreads( MetaProcess* p )
   : MetaInt32( p )
{
   TheCAMaxFileWriteThreadsParameter = this;
}

IsoString CAMaxFileWriteThreads::Id() const
{
   return "maxFileWriteThreads";
}

double CAMaxFileWriteThreads::DefaultValue() const
{
   return 0;
}

double CAMaxFileWriteThreads::MinimumValue() const
{
   return 0;
}

double CAMaxFileWriteThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF CometAlignmentParameters.cpp - Released 2024-12-11T17:43:17Z
