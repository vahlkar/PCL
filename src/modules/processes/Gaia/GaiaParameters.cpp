//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Gaia Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// GaiaParameters.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Gaia PixInsight module.
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

#include "GaiaParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GCommand*               TheGCommandParameter = nullptr;
GCenterRA*              TheGCenterRAParameter = nullptr;
GCenterDec*             TheGCenterDecParameter = nullptr;
GRadius*                TheGRadiusParameter = nullptr;
GMagnitudeLow*          TheGMagnitudeLowParameter = nullptr;
GMagnitudeHigh*         TheGMagnitudeHighParameter = nullptr;
GSourceLimit*           TheGSourceLimitParameter = nullptr;
GRequiredFlags*         TheGRequiredFlagsParameter = nullptr;
GInclusionFlags*        TheGInclusionFlagsParameter = nullptr;
GExclusionFlags*        TheGExclusionFlagsParameter = nullptr;
GVerbosity*             TheGVerbosityParameter = nullptr;
GDataRelease*           TheGDataReleaseParameter = nullptr;
GSortBy*                TheGSortByParameter = nullptr;
GGenerateTextOutput*    TheGGenerateTextOutputParameter = nullptr;
GTextFormat*            TheGTextFormatParameter = nullptr;
GTextHeaders*           TheGTextHeadersParameter = nullptr;
GOutputFilePath*        TheGOutputFilePathParameter = nullptr;

GSources*               TheGSourcesParameter = nullptr;
GSourceRA*              TheGSourceRAParameter = nullptr;
GSourceDec*             TheGSourceDecParameter = nullptr;
GSourceParx*            TheGSourceParxParameter = nullptr;
GSourcePMRA*            TheGSourcePMRAParameter = nullptr;
GSourcePMDec*           TheGSourcePMDecParameter = nullptr;
GSourceMagG*            TheGSourceMagGParameter = nullptr;
GSourceMagBP*           TheGSourceMagBPParameter = nullptr;
GSourceMagRP*           TheGSourceMagRPParameter = nullptr;
GSourceFlags*           TheGSourceFlagsParameter = nullptr;
GExcessCount*           TheGExcessCountParameter = nullptr;
GRejectCount*           TheGRejectCountParameter = nullptr;
GTimeTotal*             TheGTimeTotalParameter = nullptr;
GTimeIO*                TheGTimeIOParameter = nullptr;
GCountIO*               TheGCountIOParameter = nullptr;
GTimeUncompress*        TheGTimeUncompressParameter = nullptr;
GTimeDecode*            TheGTimeDecodeParameter = nullptr;
GIsValid*               TheGIsValidParameter = nullptr;
GOutputDataRelease*     TheGOutputDataReleaseParameter = nullptr;
GDatabaseFilePaths*     TheGDatabaseFilePathsParameter = nullptr;
GDatabaseFilePath*      TheGDatabaseFilePathParameter = nullptr;
GDatabaseMagnitudeLow*  TheGDatabaseMagnitudeLowParameter = nullptr;
GDatabaseMagnitudeHigh* TheGDatabaseMagnitudeHighParameter = nullptr;

// ----------------------------------------------------------------------------

GCommand::GCommand( MetaProcess* P )
   : MetaString( P )
{
   TheGCommandParameter = this;
}

IsoString GCommand::Id() const
{
   return "command";
}

String GCommand::DefaultValue() const
{
   return "search";
}

// ----------------------------------------------------------------------------

GCenterRA::GCenterRA( MetaProcess* P )
   : MetaDouble( P )
{
   TheGCenterRAParameter = this;
}

IsoString GCenterRA::Id() const
{
   return "centerRA";
}

int GCenterRA::Precision() const
{
   return 9;
}

double GCenterRA::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

GCenterDec::GCenterDec( MetaProcess* P )
   : MetaDouble( P )
{
   TheGCenterDecParameter = this;
}

IsoString GCenterDec::Id() const
{
   return "centerDec";
}

int GCenterDec::Precision() const
{
   return 9;
}

double GCenterDec::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

GRadius::GRadius( MetaProcess* P )
   : MetaDouble( P )
{
   TheGRadiusParameter = this;
}

IsoString GRadius::Id() const
{
   return "radius";
}

int GRadius::Precision() const
{
   return 6;
}

double GRadius::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

GMagnitudeLow::GMagnitudeLow( MetaProcess* P )
   : MetaFloat( P )
{
   TheGMagnitudeLowParameter = this;
}

IsoString GMagnitudeLow::Id() const
{
   return "magnitudeLow";
}

int GMagnitudeLow::Precision() const
{
   return 3;
}

double GMagnitudeLow::DefaultValue() const
{
   return -100;
}

// ----------------------------------------------------------------------------

GMagnitudeHigh::GMagnitudeHigh( MetaProcess* P )
   : MetaFloat( P )
{
   TheGMagnitudeHighParameter = this;
}

IsoString GMagnitudeHigh::Id() const
{
   return "magnitudeHigh";
}

int GMagnitudeHigh::Precision() const
{
   return 3;
}

double GMagnitudeHigh::DefaultValue() const
{
   return +100;
}

// ----------------------------------------------------------------------------

GSourceLimit::GSourceLimit( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGSourceLimitParameter = this;
}

IsoString GSourceLimit::Id() const
{
   return "sourceLimit";
}

double GSourceLimit::DefaultValue() const
{
   return uint32_max;
}

// ----------------------------------------------------------------------------

GRequiredFlags::GRequiredFlags( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGRequiredFlagsParameter = this;
}

IsoString GRequiredFlags::Id() const
{
   return "requiredFlags";
}

double GRequiredFlags::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

GInclusionFlags::GInclusionFlags( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGInclusionFlagsParameter = this;
}

IsoString GInclusionFlags::Id() const
{
   return "inclusionFlags";
}

double GInclusionFlags::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

GExclusionFlags::GExclusionFlags( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGExclusionFlagsParameter = this;
}

IsoString GExclusionFlags::Id() const
{
   return "exclusionFlags";
}

double GExclusionFlags::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

GVerbosity::GVerbosity( MetaProcess* P )
   : MetaInt32( P )
{
   TheGVerbosityParameter = this;
}

IsoString GVerbosity::Id() const
{
   return "verbosity";
}

double GVerbosity::DefaultValue() const
{
   return 2; // 0=silent 1=minimal 2=full
}

// ----------------------------------------------------------------------------

GDataRelease::GDataRelease( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheGDataReleaseParameter = this;
}

IsoString GDataRelease::Id() const
{
   return "dataRelease";
}

size_type GDataRelease::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString GDataRelease::ElementId( size_type index ) const
{
   switch ( index )
   {
   case DR2:           return "DataRelease_2";
   default:
   case EDR3:          return "DataRelease_E3";
   case BestAvailable: return "DataRelease_BestAvailable";
   }
}

int GDataRelease::ElementValue( size_type index ) const
{
   return int( index );
}

size_type GDataRelease::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

GSortBy::GSortBy( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheGSortByParameter = this;
}

IsoString GSortBy::Id() const
{
   return "sortBy";
}

size_type GSortBy::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString GSortBy::ElementId( size_type index ) const
{
   switch ( index )
   {
   case DontSort: return "SortBy_DontSort";
   case RA:       return "SortBy_RA";
   case Dec:      return "SortBy_Dec";
   default:
   case G:        return "SortBy_G";
   case G_BP:     return "SortBy_G_BP";
   case G_RP:     return "SortBy_G_RP";
   case Parallax: return "SortBy_Parallax";
   }
}

int GSortBy::ElementValue( size_type index ) const
{
   return int( index );
}

size_type GSortBy::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

GGenerateTextOutput::GGenerateTextOutput( MetaProcess* P )
   : MetaBoolean( P )
{
   TheGGenerateTextOutputParameter = this;
}

IsoString GGenerateTextOutput::Id() const
{
   return "generateTextOutput";
}

bool GGenerateTextOutput::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

GTextFormat::GTextFormat( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheGTextFormatParameter = this;
}

IsoString GTextFormat::Id() const
{
   return "textFormat";
}

size_type GTextFormat::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString GTextFormat::ElementId( size_type index ) const
{
   switch ( index )
   {
   case CSV:             return "TextFormat_CSV";
   case Tabular:         return "TextFormat_Tabular";
   default:
   case TabularCompound: return "TextFormat_TabularCompound";
   }
}

int GTextFormat::ElementValue( size_type index ) const
{
   return int( index );
}

size_type GTextFormat::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

GTextHeaders::GTextHeaders( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheGTextHeadersParameter = this;
}

IsoString GTextHeaders::Id() const
{
   return "textHeaders";
}

size_type GTextHeaders::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString GTextHeaders::ElementId( size_type index ) const
{
   switch ( index )
   {
   case None:                            return "TextHeaders_None";
   case TableColumns:                    return "TextHeaders_TableColumns";
   case SearchParameters:                return "TextHeaders_SearchParameters";
   default:
   case SearchParametersAndTableColumns: return "TextHeaders_SearchParametersAndTableColumns";
   }
}

int GTextHeaders::ElementValue( size_type index ) const
{
   return int( index );
}

size_type GTextHeaders::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

GOutputFilePath::GOutputFilePath( MetaProcess* P )
   : MetaString( P )
{
   TheGOutputFilePathParameter = this;
}

IsoString GOutputFilePath::Id() const
{
   return "outputFilePath";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

GSources::GSources( MetaProcess* P )
   : MetaTable( P )
{
   TheGSourcesParameter = this;
}

IsoString GSources::Id() const
{
   return "sources";
}

bool GSources::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GSourceRA::GSourceRA( MetaTable* T )
   : MetaDouble( T )
{
   TheGSourceRAParameter = this;
}

IsoString GSourceRA::Id() const
{
   return "ra";
}

int GSourceRA::Precision() const
{
   return 9;
}

bool GSourceRA::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GSourceDec::GSourceDec( MetaTable* T )
   : MetaDouble( T )
{
   TheGSourceDecParameter = this;
}

IsoString GSourceDec::Id() const
{
   return "dec";
}

int GSourceDec::Precision() const
{
   return 9;
}

bool GSourceDec::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GSourceParx::GSourceParx( MetaTable* T )
   : MetaFloat( T )
{
   TheGSourceParxParameter = this;
}

IsoString GSourceParx::Id() const
{
   return "parx";
}

int GSourceParx::Precision() const
{
   return 4;
}

bool GSourceParx::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GSourcePMRA::GSourcePMRA( MetaTable* T )
   : MetaFloat( T )
{
   TheGSourcePMRAParameter = this;
}

IsoString GSourcePMRA::Id() const
{
   return "pmra";
}

int GSourcePMRA::Precision() const
{
   return 4;
}

bool GSourcePMRA::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GSourcePMDec::GSourcePMDec( MetaTable* T )
   : MetaFloat( T )
{
   TheGSourcePMDecParameter = this;
}

IsoString GSourcePMDec::Id() const
{
   return "pmdec";
}

int GSourcePMDec::Precision() const
{
   return 4;
}

bool GSourcePMDec::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GSourceMagG::GSourceMagG( MetaTable* T )
   : MetaFloat( T )
{
   TheGSourceMagGParameter = this;
}

IsoString GSourceMagG::Id() const
{
   return "magG";
}

int GSourceMagG::Precision() const
{
   return 3;
}

bool GSourceMagG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GSourceMagBP::GSourceMagBP( MetaTable* T )
   : MetaFloat( T )
{
   TheGSourceMagBPParameter = this;
}

IsoString GSourceMagBP::Id() const
{
   return "magBP";
}

int GSourceMagBP::Precision() const
{
   return 3;
}

bool GSourceMagBP::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GSourceMagRP::GSourceMagRP( MetaTable* T )
   : MetaFloat( T )
{
   TheGSourceMagRPParameter = this;
}

IsoString GSourceMagRP::Id() const
{
   return "magRP";
}

int GSourceMagRP::Precision() const
{
   return 3;
}

bool GSourceMagRP::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GSourceFlags::GSourceFlags( MetaTable* T )
   : MetaUInt32( T )
{
   TheGSourceFlagsParameter = this;
}

IsoString GSourceFlags::Id() const
{
   return "flags";
}

bool GSourceFlags::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GExcessCount::GExcessCount( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGExcessCountParameter = this;
}

IsoString GExcessCount::Id() const
{
   return "excessCount";
}

bool GExcessCount::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GRejectCount::GRejectCount( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGRejectCountParameter = this;
}

IsoString GRejectCount::Id() const
{
   return "rejectCount";
}

bool GRejectCount::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GTimeTotal::GTimeTotal( MetaProcess* P )
   : MetaDouble( P )
{
   TheGTimeTotalParameter = this;
}

IsoString GTimeTotal::Id() const
{
   return "timeTotal";
}

int GTimeTotal::Precision() const
{
   return 4;
}

bool GTimeTotal::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GTimeIO::GTimeIO( MetaProcess* P )
   : MetaDouble( P )
{
   TheGTimeIOParameter = this;
}

IsoString GTimeIO::Id() const
{
   return "timeIO";
}

int GTimeIO::Precision() const
{
   return 4;
}

bool GTimeIO::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GCountIO::GCountIO( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGCountIOParameter = this;
}

IsoString GCountIO::Id() const
{
   return "countIO";
}

bool GCountIO::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GTimeUncompress::GTimeUncompress( MetaProcess* P )
   : MetaDouble( P )
{
   TheGTimeUncompressParameter = this;
}

IsoString GTimeUncompress::Id() const
{
   return "timeUncompress";
}

int GTimeUncompress::Precision() const
{
   return 4;
}

bool GTimeUncompress::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GTimeDecode::GTimeDecode( MetaProcess* P )
   : MetaDouble( P )
{
   TheGTimeDecodeParameter = this;
}

IsoString GTimeDecode::Id() const
{
   return "timeDecode";
}

int GTimeDecode::Precision() const
{
   return 4;
}

bool GTimeDecode::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GIsValid::GIsValid( MetaProcess* P )
   : MetaBoolean( P )
{
   TheGIsValidParameter = this;
}

IsoString GIsValid::Id() const
{
   return "isValid";
}

bool GIsValid::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GOutputDataRelease::GOutputDataRelease( MetaProcess* P )
   : MetaInt32( P )
{
   TheGOutputDataReleaseParameter = this;
}

IsoString GOutputDataRelease::Id() const
{
   return "outputDataRelease";
}

bool GOutputDataRelease::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDatabaseFilePaths::GDatabaseFilePaths( MetaProcess* P )
   : MetaTable( P )
{
   TheGDatabaseFilePathsParameter = this;
}

IsoString GDatabaseFilePaths::Id() const
{
   return "databaseFilePaths";
}

bool GDatabaseFilePaths::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDatabaseFilePath::GDatabaseFilePath( MetaTable* T )
   : MetaString( T )
{
   TheGDatabaseFilePathParameter = this;
}

IsoString GDatabaseFilePath::Id() const
{
   return "filePath";
}

bool GDatabaseFilePath::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDatabaseMagnitudeLow::GDatabaseMagnitudeLow( MetaProcess* P )
   : MetaFloat( P )
{
   TheGDatabaseMagnitudeLowParameter = this;
}

IsoString GDatabaseMagnitudeLow::Id() const
{
   return "databaseMagnitudeLow";
}

int GDatabaseMagnitudeLow::Precision() const
{
   return 2;
}

bool GDatabaseMagnitudeLow::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDatabaseMagnitudeHigh::GDatabaseMagnitudeHigh( MetaProcess* P )
   : MetaFloat( P )
{
   TheGDatabaseMagnitudeHighParameter = this;
}

IsoString GDatabaseMagnitudeHigh::Id() const
{
   return "databaseMagnitudeHigh";
}

int GDatabaseMagnitudeHigh::Precision() const
{
   return 2;
}

bool GDatabaseMagnitudeHigh::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GaiaParameters.cpp - Released 2021-04-09T19:41:48Z
