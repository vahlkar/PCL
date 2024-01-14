//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.5
// ----------------------------------------------------------------------------
// Standard FindingChart Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// FindingChartParameters.cpp - Released 2024-01-13T15:48:22Z
// ----------------------------------------------------------------------------
// This file is part of the standard FindingChart PixInsight module.
//
// Copyright (c) 2003-2024 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "FindingChartParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FCChartSize*                TheFCChartSizeParameter = nullptr;
FCAutoChartResolution*      TheFCAutoChartResolutionParameter = nullptr;
FCChartResolution*          TheFCChartResolutionParameter = nullptr;
FCAutoChartMaxMagnitude*    TheFCAutoChartMaxMagnitudeParameter = nullptr;
FCChartMaxMagnitude*        TheFCChartMaxMagnitudeParameter = nullptr;
FCDrawGrid*                 TheFCDrawGridParameter = nullptr;
FCDrawConstellationBorders* TheFCDrawConstellationBordersParameter = nullptr;
FCDrawConstellationLines*   TheFCDrawConstellationLinesParameter = nullptr;
FCDrawConstellationNames*   TheFCDrawConstellationNamesParameter = nullptr;
FCDrawStars*                TheFCDrawStarsParameter = nullptr;
FCDrawStarNames*            TheFCDrawStarNamesParameter = nullptr;
FCDrawMessierObjects*       TheFCDrawMessierObjectsParameter = nullptr;
FCDrawNGCObjects*           TheFCDrawNGCObjectsParameter = nullptr;
FCGenerateBitmapFile*       TheFCGenerateBitmapFileParameter = nullptr;
FCOutputDirectory*          TheFCOutputDirectoryParameter = nullptr;
FCOutputSuffix*             TheFCOutputSuffixParameter = nullptr;
FCCloseOutputWindow*        TheFCCloseOutputWindowParameter = nullptr;
FCImageRegionColor*         TheFCImageRegionColorParameter = nullptr;
FCImageRegionBorderColor*   TheFCImageRegionBorderColorParameter = nullptr;
FCGridColor*                TheFCGridColorParameter = nullptr;
FCGridTextColor*            TheFCGridTextColorParameter = nullptr;
FCConstellationBorderColor* TheFCConstellationBorderColorParameter = nullptr;
FCConstellationLineColor*   TheFCConstellationLineColorParameter = nullptr;
FCConstellationTextColor*   TheFCConstellationTextColorParameter = nullptr;
FCStarTextColor*            TheFCStarTextColorParameter = nullptr;
FCMessierTextColor*         TheFCMessierTextColorParameter = nullptr;
FCNGCTextColor*             TheFCNGCTextColorParameter = nullptr;

// ----------------------------------------------------------------------------

FCChartSize::FCChartSize( MetaProcess* P )
   : MetaInt32( P )
{
   TheFCChartSizeParameter = this;
}

IsoString FCChartSize::Id() const
{
   return "chartSize";
}

double FCChartSize::DefaultValue() const
{
   return 1024;
}

double FCChartSize::MinimumValue() const
{
   return 512;
}

double FCChartSize::MaximumValue() const
{
   return 65536;
}

// ----------------------------------------------------------------------------

FCAutoChartResolution::FCAutoChartResolution( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCAutoChartResolutionParameter = this;
}

IsoString FCAutoChartResolution::Id() const
{
   return "autoChartResolution";
}

bool FCAutoChartResolution::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

FCChartResolution::FCChartResolution( MetaProcess* P )
   : MetaFloat( P )
{
   TheFCChartResolutionParameter = this;
}

IsoString FCChartResolution::Id() const
{
   return "chartResolution";
}

int FCChartResolution::Precision() const
{
   return 1;
}

double FCChartResolution::DefaultValue() const
{
   return 120.0;
}

double FCChartResolution::MinimumValue() const
{
   return 1.0;
}

double FCChartResolution::MaximumValue() const
{
   return 1200.0;
}

// ----------------------------------------------------------------------------

FCAutoChartMaxMagnitude::FCAutoChartMaxMagnitude( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCAutoChartMaxMagnitudeParameter = this;
}

IsoString FCAutoChartMaxMagnitude::Id() const
{
   return "autoChartMaxMagnitude";
}

bool FCAutoChartMaxMagnitude::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

FCChartMaxMagnitude::FCChartMaxMagnitude( MetaProcess* P )
   : MetaFloat( P )
{
   TheFCChartMaxMagnitudeParameter = this;
}

IsoString FCChartMaxMagnitude::Id() const
{
   return "chartMaxMagnitude";
}

int FCChartMaxMagnitude::Precision() const
{
   return 1;
}

double FCChartMaxMagnitude::DefaultValue() const
{
   return 7.0;
}

double FCChartMaxMagnitude::MinimumValue() const
{
   return 4.0;
}

double FCChartMaxMagnitude::MaximumValue() const
{
   return 16.0;
}

// ----------------------------------------------------------------------------

FCDrawGrid::FCDrawGrid( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCDrawGridParameter = this;
}

IsoString FCDrawGrid::Id() const
{
   return "drawGrid";
}

bool FCDrawGrid::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

FCDrawConstellationBorders::FCDrawConstellationBorders( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCDrawConstellationBordersParameter = this;
}

IsoString FCDrawConstellationBorders::Id() const
{
   return "drawConstellationBorders";
}

bool FCDrawConstellationBorders::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

FCDrawConstellationLines::FCDrawConstellationLines( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCDrawConstellationLinesParameter = this;
}

IsoString FCDrawConstellationLines::Id() const
{
   return "drawConstellationLines";
}

bool FCDrawConstellationLines::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

FCDrawConstellationNames::FCDrawConstellationNames( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCDrawConstellationNamesParameter = this;
}

IsoString FCDrawConstellationNames::Id() const
{
   return "drawConstellationNames";
}

bool FCDrawConstellationNames::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

FCDrawStars::FCDrawStars( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCDrawStarsParameter = this;
}

IsoString FCDrawStars::Id() const
{
   return "drawStars";
}

bool FCDrawStars::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

FCDrawStarNames::FCDrawStarNames( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCDrawStarNamesParameter = this;
}

IsoString FCDrawStarNames::Id() const
{
   return "drawStarNames";
}

bool FCDrawStarNames::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

FCDrawMessierObjects::FCDrawMessierObjects( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCDrawMessierObjectsParameter = this;
}

IsoString FCDrawMessierObjects::Id() const
{
   return "drawMessierObjects";
}

bool FCDrawMessierObjects::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

FCDrawNGCObjects::FCDrawNGCObjects( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCDrawNGCObjectsParameter = this;
}

IsoString FCDrawNGCObjects::Id() const
{
   return "drawNGCObjects";
}

bool FCDrawNGCObjects::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

FCGenerateBitmapFile::FCGenerateBitmapFile( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCGenerateBitmapFileParameter = this;
}

IsoString FCGenerateBitmapFile::Id() const
{
   return "generateBitmapFile";
}

bool FCGenerateBitmapFile::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

FCOutputDirectory::FCOutputDirectory( MetaProcess* P )
   : MetaString( P )
{
   TheFCOutputDirectoryParameter = this;
}

IsoString FCOutputDirectory::Id() const
{
   return "outputDirectory";
}

// ----------------------------------------------------------------------------

FCOutputSuffix::FCOutputSuffix( MetaProcess* P )
   : MetaString( P )
{
   TheFCOutputSuffixParameter = this;
}

IsoString FCOutputSuffix::Id() const
{
   return "outputSuffix";
}

String FCOutputSuffix::DefaultValue() const
{
   return "_fc";
}

// ----------------------------------------------------------------------------

FCCloseOutputWindow::FCCloseOutputWindow( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFCCloseOutputWindowParameter = this;
}

IsoString FCCloseOutputWindow::Id() const
{
   return "closeOutputWindow";
}

bool FCCloseOutputWindow::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

FCImageRegionColor::FCImageRegionColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheFCImageRegionColorParameter = this;
}

IsoString FCImageRegionColor::Id() const
{
   return "imageRegionColor";
}

double FCImageRegionColor::DefaultValue() const
{
   return 0x40FF0000;
}

// ----------------------------------------------------------------------------

FCImageRegionBorderColor::FCImageRegionBorderColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheFCImageRegionBorderColorParameter = this;
}

IsoString FCImageRegionBorderColor::Id() const
{
   return "imageRegionBorderColor";
}

double FCImageRegionBorderColor::DefaultValue() const
{
   return 0xA0FF0000;
}

// ----------------------------------------------------------------------------

FCGridColor::FCGridColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheFCGridColorParameter = this;
}

IsoString FCGridColor::Id() const
{
   return "gridColor";
}

double FCGridColor::DefaultValue() const
{
   return 0x7FFFFFFF;
}

// ----------------------------------------------------------------------------

FCGridTextColor::FCGridTextColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheFCGridTextColorParameter = this;
}

IsoString FCGridTextColor::Id() const
{
   return "gridTextColor";
}

double FCGridTextColor::DefaultValue() const
{
   return 0xFFFFFFFF;
}

// ----------------------------------------------------------------------------

FCConstellationBorderColor::FCConstellationBorderColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheFCConstellationBorderColorParameter = this;
}

IsoString FCConstellationBorderColor::Id() const
{
   return "constellationBorderColor";
}

double FCConstellationBorderColor::DefaultValue() const
{
   return 0x3000FFFF;
}

// ----------------------------------------------------------------------------

FCConstellationLineColor::FCConstellationLineColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheFCConstellationLineColorParameter = this;
}

IsoString FCConstellationLineColor::Id() const
{
   return "constellationLineColor";
}

double FCConstellationLineColor::DefaultValue() const
{
   return 0x8CF08080;
}

// ----------------------------------------------------------------------------

FCConstellationTextColor::FCConstellationTextColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheFCConstellationTextColorParameter = this;
}

IsoString FCConstellationTextColor::Id() const
{
   return "constellationTextColor";
}

double FCConstellationTextColor::DefaultValue() const
{
   return 0x4000FFFF;
}

// ----------------------------------------------------------------------------

FCStarTextColor::FCStarTextColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheFCStarTextColorParameter = this;
}

IsoString FCStarTextColor::Id() const
{
   return "starTextColor";
}

double FCStarTextColor::DefaultValue() const
{
   return 0xFFFFD700;
}

// ----------------------------------------------------------------------------

FCMessierTextColor::FCMessierTextColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheFCMessierTextColorParameter = this;
}

IsoString FCMessierTextColor::Id() const
{
   return "messierTextColor";
}

double FCMessierTextColor::DefaultValue() const
{
   return 0xFF8470FF;
}

// ----------------------------------------------------------------------------

FCNGCTextColor::FCNGCTextColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheFCNGCTextColorParameter = this;
}

IsoString FCNGCTextColor::Id() const
{
   return "ngcTextColor";
}

double FCNGCTextColor::DefaultValue() const
{
   return 0xB4CD853F;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FindingChartParameters.cpp - Released 2024-01-13T15:48:22Z
