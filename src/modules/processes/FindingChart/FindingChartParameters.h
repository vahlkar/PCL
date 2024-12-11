//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// Standard FindingChart Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// FindingChartParameters.h - Released 2024-12-11T17:43:17Z
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

#ifndef __FindingChartParameters_h
#define __FindingChartParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class FCChartSize : public MetaInt32
{
public:

   FCChartSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern FCChartSize* TheFCChartSizeParameter;

// ----------------------------------------------------------------------------

class FCAutoChartResolution : public MetaBoolean
{
public:

   FCAutoChartResolution( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCAutoChartResolution* TheFCAutoChartResolutionParameter;

// ----------------------------------------------------------------------------

class FCChartResolution : public MetaFloat
{
public:

   FCChartResolution( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern FCChartResolution* TheFCChartResolutionParameter;

// ----------------------------------------------------------------------------

class FCAutoChartMaxMagnitude : public MetaBoolean
{
public:

   FCAutoChartMaxMagnitude( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCAutoChartMaxMagnitude* TheFCAutoChartMaxMagnitudeParameter;

// ----------------------------------------------------------------------------

class FCChartMaxMagnitude : public MetaFloat
{
public:

   FCChartMaxMagnitude( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern FCChartMaxMagnitude* TheFCChartMaxMagnitudeParameter;

// ----------------------------------------------------------------------------

class FCDrawGrid : public MetaBoolean
{
public:

   FCDrawGrid( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCDrawGrid* TheFCDrawGridParameter;

// ----------------------------------------------------------------------------

class FCDrawEcliptic : public MetaBoolean
{
public:

   FCDrawEcliptic( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCDrawEcliptic* TheFCDrawEclipticParameter;

// ----------------------------------------------------------------------------

class FCDrawGalacticEquator : public MetaBoolean
{
public:

   FCDrawGalacticEquator( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCDrawGalacticEquator* TheFCDrawGalacticEquatorParameter;

// ----------------------------------------------------------------------------

class FCDrawConstellationBorders : public MetaBoolean
{
public:

   FCDrawConstellationBorders( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCDrawConstellationBorders* TheFCDrawConstellationBordersParameter;

// ----------------------------------------------------------------------------

class FCDrawConstellationLines : public MetaBoolean
{
public:

   FCDrawConstellationLines( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCDrawConstellationLines* TheFCDrawConstellationLinesParameter;

// ----------------------------------------------------------------------------

class FCDrawConstellationNames : public MetaBoolean
{
public:

   FCDrawConstellationNames( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCDrawConstellationNames* TheFCDrawConstellationNamesParameter;

// ----------------------------------------------------------------------------

class FCDrawStars : public MetaBoolean
{
public:

   FCDrawStars( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCDrawStars* TheFCDrawStarsParameter;

// ----------------------------------------------------------------------------

class FCDrawStarNames : public MetaBoolean
{
public:

   FCDrawStarNames( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCDrawStarNames* TheFCDrawStarNamesParameter;

// ----------------------------------------------------------------------------

class FCDrawMessierObjects : public MetaBoolean
{
public:

   FCDrawMessierObjects( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCDrawMessierObjects* TheFCDrawMessierObjectsParameter;

// ----------------------------------------------------------------------------

class FCDrawNGCObjects : public MetaBoolean
{
public:

   FCDrawNGCObjects( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCDrawNGCObjects* TheFCDrawNGCObjectsParameter;

// ----------------------------------------------------------------------------

class FCGenerateBitmapFile : public MetaBoolean
{
public:

   FCGenerateBitmapFile( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCGenerateBitmapFile* TheFCGenerateBitmapFileParameter;

// ----------------------------------------------------------------------------

class FCOutputDirectory : public MetaString
{
public:

   FCOutputDirectory( MetaProcess* );

   IsoString Id() const override;
};

extern FCOutputDirectory* TheFCOutputDirectoryParameter;

// ----------------------------------------------------------------------------

class FCOutputSuffix : public MetaString
{
public:

   FCOutputSuffix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern FCOutputSuffix* TheFCOutputSuffixParameter;

// ----------------------------------------------------------------------------

class FCCloseOutputWindow : public MetaBoolean
{
public:

   FCCloseOutputWindow( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FCCloseOutputWindow* TheFCCloseOutputWindowParameter;

// ----------------------------------------------------------------------------

class FCImageRegionColor : public MetaUInt32
{
public:

   FCImageRegionColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCImageRegionColor* TheFCImageRegionColorParameter;

// ----------------------------------------------------------------------------

class FCImageRegionBorderColor : public MetaUInt32
{
public:

   FCImageRegionBorderColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCImageRegionBorderColor* TheFCImageRegionBorderColorParameter;

// ----------------------------------------------------------------------------

class FCGridColor : public MetaUInt32
{
public:

   FCGridColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCGridColor* TheFCGridColorParameter;

// ----------------------------------------------------------------------------

class FCGridTextColor : public MetaUInt32
{
public:

   FCGridTextColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCGridTextColor* TheFCGridTextColorParameter;

// ----------------------------------------------------------------------------

class FCEclipticColor : public MetaUInt32
{
public:

   FCEclipticColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCEclipticColor* TheFCEclipticColorParameter;

// ----------------------------------------------------------------------------

class FCEclipticTextColor : public MetaUInt32
{
public:

   FCEclipticTextColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCEclipticTextColor* TheFCEclipticTextColorParameter;

// ----------------------------------------------------------------------------

class FCGalacticEquatorColor : public MetaUInt32
{
public:

   FCGalacticEquatorColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCGalacticEquatorColor* TheFCGalacticEquatorColorParameter;

// ----------------------------------------------------------------------------

class FCGalacticEquatorTextColor : public MetaUInt32
{
public:

   FCGalacticEquatorTextColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCGalacticEquatorTextColor* TheFCGalacticEquatorTextColorParameter;

// ----------------------------------------------------------------------------

class FCConstellationBorderColor : public MetaUInt32
{
public:

   FCConstellationBorderColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCConstellationBorderColor* TheFCConstellationBorderColorParameter;

// ----------------------------------------------------------------------------

class FCConstellationLineColor : public MetaUInt32
{
public:

   FCConstellationLineColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCConstellationLineColor* TheFCConstellationLineColorParameter;

// ----------------------------------------------------------------------------

class FCConstellationTextColor : public MetaUInt32
{
public:

   FCConstellationTextColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCConstellationTextColor* TheFCConstellationTextColorParameter;

// ----------------------------------------------------------------------------

class FCStarTextColor : public MetaUInt32
{
public:

   FCStarTextColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCStarTextColor* TheFCStarTextColorParameter;

// ----------------------------------------------------------------------------

class FCMessierTextColor : public MetaUInt32
{
public:

   FCMessierTextColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCMessierTextColor* TheFCMessierTextColorParameter;

// ----------------------------------------------------------------------------

class FCNGCTextColor : public MetaUInt32
{
public:

   FCNGCTextColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern FCNGCTextColor* TheFCNGCTextColorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FindingChartParameters_h

// ----------------------------------------------------------------------------
// EOF FindingChartParameters.h - Released 2024-12-11T17:43:17Z
