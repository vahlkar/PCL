//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.3
// ----------------------------------------------------------------------------
// Standard GaiaDR2 Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// GaiaDR2Parameters.cpp - Released 2020-11-27T11:02:59Z
// ----------------------------------------------------------------------------
// This file is part of the standard GaiaDR2 PixInsight module.
//
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "GaiaDR2Parameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GDR2Command*               TheGDR2CommandParameter = nullptr;
GDR2CenterRA*              TheGDR2CenterRAParameter = nullptr;
GDR2CenterDec*             TheGDR2CenterDecParameter = nullptr;
GDR2Radius*                TheGDR2RadiusParameter = nullptr;
GDR2MagnitudeLow*          TheGDR2MagnitudeLowParameter = nullptr;
GDR2MagnitudeHigh*         TheGDR2MagnitudeHighParameter = nullptr;
GDR2SourceLimit*           TheGDR2SourceLimitParameter = nullptr;
GDR2IncludeFlags*          TheGDR2IncludeFlagsParameter = nullptr;
GDR2ExcludeFlags*          TheGDR2ExcludeFlagsParameter = nullptr;
GDR2Verbosity*             TheGDR2VerbosityParameter = nullptr;
GDR2SortBy*                TheGDR2SortByParameter = nullptr;
GDR2GenerateTextOutput*    TheGDR2GenerateTextOutputParameter = nullptr;
GDR2TextFormat*            TheGDR2TextFormatParameter = nullptr;
GDR2TextHeaders*           TheGDR2TextHeadersParameter = nullptr;
GDR2OutputFilePath*        TheGDR2OutputFilePathParameter = nullptr;

GDR2Sources*               TheGDR2SourcesParameter = nullptr;
GDR2SourceRA*              TheGDR2SourceRAParameter = nullptr;
GDR2SourceDec*             TheGDR2SourceDecParameter = nullptr;
GDR2SourceParx*            TheGDR2SourceParxParameter = nullptr;
GDR2SourcePMRA*            TheGDR2SourcePMRAParameter = nullptr;
GDR2SourcePMDec*           TheGDR2SourcePMDecParameter = nullptr;
GDR2SourceMagG*            TheGDR2SourceMagGParameter = nullptr;
GDR2SourceMagBP*           TheGDR2SourceMagBPParameter = nullptr;
GDR2SourceMagRP*           TheGDR2SourceMagRPParameter = nullptr;
GDR2SourceFlags*           TheGDR2SourceFlagsParameter = nullptr;
GDR2ExcessCount*           TheGDR2ExcessCountParameter = nullptr;
GDR2RejectCount*           TheGDR2RejectCountParameter = nullptr;
GDR2TimeTotal*             TheGDR2TimeTotalParameter = nullptr;
GDR2TimeIO*                TheGDR2TimeIOParameter = nullptr;
GDR2CountIO*               TheGDR2CountIOParameter = nullptr;
GDR2TimeUncompress*        TheGDR2TimeUncompressParameter = nullptr;
GDR2TimeDecode*            TheGDR2TimeDecodeParameter = nullptr;
GDR2DatabaseFilePaths*     TheGDR2DatabaseFilePathsParameter = nullptr;
GDR2DatabaseFilePath*      TheGDR2DatabaseFilePathParameter = nullptr;
GDR2DatabaseMagnitudeLow*  TheGDR2DatabaseMagnitudeLowParameter = nullptr;
GDR2DatabaseMagnitudeHigh* TheGDR2DatabaseMagnitudeHighParameter = nullptr;

// ----------------------------------------------------------------------------

GDR2Command::GDR2Command( MetaProcess* P )
   : MetaString( P )
{
   TheGDR2CommandParameter = this;
}

IsoString GDR2Command::Id() const
{
   return "command";
}

String GDR2Command::DefaultValue() const
{
   return "search";
}

// ----------------------------------------------------------------------------

GDR2CenterRA::GDR2CenterRA( MetaProcess* P )
   : MetaDouble( P )
{
   TheGDR2CenterRAParameter = this;
}

IsoString GDR2CenterRA::Id() const
{
   return "centerRA";
}

int GDR2CenterRA::Precision() const
{
   return 9;
}

double GDR2CenterRA::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

GDR2CenterDec::GDR2CenterDec( MetaProcess* P )
   : MetaDouble( P )
{
   TheGDR2CenterDecParameter = this;
}

IsoString GDR2CenterDec::Id() const
{
   return "centerDec";
}

int GDR2CenterDec::Precision() const
{
   return 9;
}

double GDR2CenterDec::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

GDR2Radius::GDR2Radius( MetaProcess* P )
   : MetaDouble( P )
{
   TheGDR2RadiusParameter = this;
}

IsoString GDR2Radius::Id() const
{
   return "radius";
}

int GDR2Radius::Precision() const
{
   return 6;
}

double GDR2Radius::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

GDR2MagnitudeLow::GDR2MagnitudeLow( MetaProcess* P )
   : MetaFloat( P )
{
   TheGDR2MagnitudeLowParameter = this;
}

IsoString GDR2MagnitudeLow::Id() const
{
   return "magnitudeLow";
}

int GDR2MagnitudeLow::Precision() const
{
   return 3;
}

double GDR2MagnitudeLow::DefaultValue() const
{
   return -100;
}

// ----------------------------------------------------------------------------

GDR2MagnitudeHigh::GDR2MagnitudeHigh( MetaProcess* P )
   : MetaFloat( P )
{
   TheGDR2MagnitudeHighParameter = this;
}

IsoString GDR2MagnitudeHigh::Id() const
{
   return "magnitudeHigh";
}

int GDR2MagnitudeHigh::Precision() const
{
   return 3;
}

double GDR2MagnitudeHigh::DefaultValue() const
{
   return +100;
}

// ----------------------------------------------------------------------------

GDR2SourceLimit::GDR2SourceLimit( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGDR2SourceLimitParameter = this;
}

IsoString GDR2SourceLimit::Id() const
{
   return "sourceLimit";
}

double GDR2SourceLimit::DefaultValue() const
{
   return uint32_max;
}

// ----------------------------------------------------------------------------

GDR2IncludeFlags::GDR2IncludeFlags( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGDR2IncludeFlagsParameter = this;
}

IsoString GDR2IncludeFlags::Id() const
{
   return "inclusionFlags";
}

double GDR2IncludeFlags::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

GDR2ExcludeFlags::GDR2ExcludeFlags( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGDR2ExcludeFlagsParameter = this;
}

IsoString GDR2ExcludeFlags::Id() const
{
   return "exclusionFlags";
}

double GDR2ExcludeFlags::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

GDR2Verbosity::GDR2Verbosity( MetaProcess* P )
   : MetaInt32( P )
{
   TheGDR2VerbosityParameter = this;
}

IsoString GDR2Verbosity::Id() const
{
   return "verbosity";
}

double GDR2Verbosity::DefaultValue() const
{
   return 2; // 0=silent 1=minimal 2=full
}

// ----------------------------------------------------------------------------

GDR2SortBy::GDR2SortBy( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheGDR2SortByParameter = this;
}

IsoString GDR2SortBy::Id() const
{
   return "sortBy";
}

size_type GDR2SortBy::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString GDR2SortBy::ElementId( size_type index ) const
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

int GDR2SortBy::ElementValue( size_type index ) const
{
   return int( index );
}

size_type GDR2SortBy::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

GDR2GenerateTextOutput::GDR2GenerateTextOutput( MetaProcess* P )
   : MetaBoolean( P )
{
   TheGDR2GenerateTextOutputParameter = this;
}

IsoString GDR2GenerateTextOutput::Id() const
{
   return "generateTextOutput";
}

bool GDR2GenerateTextOutput::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2TextFormat::GDR2TextFormat( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheGDR2TextFormatParameter = this;
}

IsoString GDR2TextFormat::Id() const
{
   return "textFormat";
}

size_type GDR2TextFormat::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString GDR2TextFormat::ElementId( size_type index ) const
{
   switch ( index )
   {
   case CSV:             return "TextFormat_CSV";
   case Tabular:         return "TextFormat_Tabular";
   default:
   case TabularCompound: return "TextFormat_TabularCompound";
   }
}

int GDR2TextFormat::ElementValue( size_type index ) const
{
   return int( index );
}

size_type GDR2TextFormat::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

GDR2TextHeaders::GDR2TextHeaders( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheGDR2TextHeadersParameter = this;
}

IsoString GDR2TextHeaders::Id() const
{
   return "textHeaders";
}

size_type GDR2TextHeaders::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString GDR2TextHeaders::ElementId( size_type index ) const
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

int GDR2TextHeaders::ElementValue( size_type index ) const
{
   return int( index );
}

size_type GDR2TextHeaders::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

GDR2OutputFilePath::GDR2OutputFilePath( MetaProcess* P )
   : MetaString( P )
{
   TheGDR2OutputFilePathParameter = this;
}

IsoString GDR2OutputFilePath::Id() const
{
   return "outputFilePath";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

GDR2Sources::GDR2Sources( MetaProcess* P )
   : MetaTable( P )
{
   TheGDR2SourcesParameter = this;
}

IsoString GDR2Sources::Id() const
{
   return "sources";
}

bool GDR2Sources::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2SourceRA::GDR2SourceRA( MetaTable* T )
   : MetaDouble( T )
{
   TheGDR2SourceRAParameter = this;
}

IsoString GDR2SourceRA::Id() const
{
   return "ra";
}

int GDR2SourceRA::Precision() const
{
   return 9;
}

bool GDR2SourceRA::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2SourceDec::GDR2SourceDec( MetaTable* T )
   : MetaDouble( T )
{
   TheGDR2SourceDecParameter = this;
}

IsoString GDR2SourceDec::Id() const
{
   return "dec";
}

int GDR2SourceDec::Precision() const
{
   return 9;
}

bool GDR2SourceDec::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2SourceParx::GDR2SourceParx( MetaTable* T )
   : MetaFloat( T )
{
   TheGDR2SourceParxParameter = this;
}

IsoString GDR2SourceParx::Id() const
{
   return "parx";
}

int GDR2SourceParx::Precision() const
{
   return 4;
}

bool GDR2SourceParx::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2SourcePMRA::GDR2SourcePMRA( MetaTable* T )
   : MetaFloat( T )
{
   TheGDR2SourcePMRAParameter = this;
}

IsoString GDR2SourcePMRA::Id() const
{
   return "pmra";
}

int GDR2SourcePMRA::Precision() const
{
   return 4;
}

bool GDR2SourcePMRA::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2SourcePMDec::GDR2SourcePMDec( MetaTable* T )
   : MetaFloat( T )
{
   TheGDR2SourcePMDecParameter = this;
}

IsoString GDR2SourcePMDec::Id() const
{
   return "pmdec";
}

int GDR2SourcePMDec::Precision() const
{
   return 4;
}

bool GDR2SourcePMDec::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2SourceMagG::GDR2SourceMagG( MetaTable* T )
   : MetaFloat( T )
{
   TheGDR2SourceMagGParameter = this;
}

IsoString GDR2SourceMagG::Id() const
{
   return "magG";
}

int GDR2SourceMagG::Precision() const
{
   return 3;
}

bool GDR2SourceMagG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2SourceMagBP::GDR2SourceMagBP( MetaTable* T )
   : MetaFloat( T )
{
   TheGDR2SourceMagBPParameter = this;
}

IsoString GDR2SourceMagBP::Id() const
{
   return "magBP";
}

int GDR2SourceMagBP::Precision() const
{
   return 3;
}

bool GDR2SourceMagBP::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2SourceMagRP::GDR2SourceMagRP( MetaTable* T )
   : MetaFloat( T )
{
   TheGDR2SourceMagRPParameter = this;
}

IsoString GDR2SourceMagRP::Id() const
{
   return "magRP";
}

int GDR2SourceMagRP::Precision() const
{
   return 3;
}

bool GDR2SourceMagRP::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2SourceFlags::GDR2SourceFlags( MetaTable* T )
   : MetaUInt32( T )
{
   TheGDR2SourceFlagsParameter = this;
}

IsoString GDR2SourceFlags::Id() const
{
   return "flags";
}

bool GDR2SourceFlags::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2ExcessCount::GDR2ExcessCount( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGDR2ExcessCountParameter = this;
}

IsoString GDR2ExcessCount::Id() const
{
   return "excessCount";
}

bool GDR2ExcessCount::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2RejectCount::GDR2RejectCount( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGDR2RejectCountParameter = this;
}

IsoString GDR2RejectCount::Id() const
{
   return "rejectCount";
}

bool GDR2RejectCount::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2TimeTotal::GDR2TimeTotal( MetaProcess* P )
   : MetaDouble( P )
{
   TheGDR2TimeTotalParameter = this;
}

IsoString GDR2TimeTotal::Id() const
{
   return "timeTotal";
}

int GDR2TimeTotal::Precision() const
{
   return 4;
}

bool GDR2TimeTotal::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2TimeIO::GDR2TimeIO( MetaProcess* P )
   : MetaDouble( P )
{
   TheGDR2TimeIOParameter = this;
}

IsoString GDR2TimeIO::Id() const
{
   return "timeIO";
}

int GDR2TimeIO::Precision() const
{
   return 4;
}

bool GDR2TimeIO::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2CountIO::GDR2CountIO( MetaProcess* P )
   : MetaUInt32( P )
{
   TheGDR2CountIOParameter = this;
}

IsoString GDR2CountIO::Id() const
{
   return "countIO";
}

bool GDR2CountIO::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2TimeUncompress::GDR2TimeUncompress( MetaProcess* P )
   : MetaDouble( P )
{
   TheGDR2TimeUncompressParameter = this;
}

IsoString GDR2TimeUncompress::Id() const
{
   return "timeUncompress";
}

int GDR2TimeUncompress::Precision() const
{
   return 4;
}

bool GDR2TimeUncompress::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2TimeDecode::GDR2TimeDecode( MetaProcess* P )
   : MetaDouble( P )
{
   TheGDR2TimeDecodeParameter = this;
}

IsoString GDR2TimeDecode::Id() const
{
   return "timeDecode";
}

int GDR2TimeDecode::Precision() const
{
   return 4;
}

bool GDR2TimeDecode::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2DatabaseFilePaths::GDR2DatabaseFilePaths( MetaProcess* P )
   : MetaTable( P )
{
   TheGDR2DatabaseFilePathsParameter = this;
}

IsoString GDR2DatabaseFilePaths::Id() const
{
   return "databaseFilePaths";
}

bool GDR2DatabaseFilePaths::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2DatabaseFilePath::GDR2DatabaseFilePath( MetaTable* T )
   : MetaString( T )
{
   TheGDR2DatabaseFilePathParameter = this;
}

IsoString GDR2DatabaseFilePath::Id() const
{
   return "filePath";
}

bool GDR2DatabaseFilePath::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2DatabaseMagnitudeLow::GDR2DatabaseMagnitudeLow( MetaProcess* P )
   : MetaFloat( P )
{
   TheGDR2DatabaseMagnitudeLowParameter = this;
}

IsoString GDR2DatabaseMagnitudeLow::Id() const
{
   return "databaseMagnitudeLow";
}

int GDR2DatabaseMagnitudeLow::Precision() const
{
   return 2;
}

bool GDR2DatabaseMagnitudeLow::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

GDR2DatabaseMagnitudeHigh::GDR2DatabaseMagnitudeHigh( MetaProcess* P )
   : MetaFloat( P )
{
   TheGDR2DatabaseMagnitudeHighParameter = this;
}

IsoString GDR2DatabaseMagnitudeHigh::Id() const
{
   return "databaseMagnitudeHigh";
}

int GDR2DatabaseMagnitudeHigh::Precision() const
{
   return 2;
}

bool GDR2DatabaseMagnitudeHigh::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GaiaDR2Parameters.cpp - Released 2020-11-27T11:02:59Z
