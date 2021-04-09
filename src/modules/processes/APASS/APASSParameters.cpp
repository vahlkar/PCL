//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard APASS Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// APASSParameters.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard APASS PixInsight module.
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

#include "APASSParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ACommand*               TheACommandParameter = nullptr;
ACenterRA*              TheACenterRAParameter = nullptr;
ACenterDec*             TheACenterDecParameter = nullptr;
ARadius*                TheARadiusParameter = nullptr;
AMagnitudeLow*          TheAMagnitudeLowParameter = nullptr;
AMagnitudeHigh*         TheAMagnitudeHighParameter = nullptr;
ASourceLimit*           TheASourceLimitParameter = nullptr;
ARequiredFlags*         TheARequiredFlagsParameter = nullptr;
AInclusionFlags*        TheAInclusionFlagsParameter = nullptr;
AExclusionFlags*        TheAExclusionFlagsParameter = nullptr;
AVerbosity*             TheAVerbosityParameter = nullptr;
ADataRelease*           TheADataReleaseParameter = nullptr;
ASortBy*                TheASortByParameter = nullptr;
AGenerateTextOutput*    TheAGenerateTextOutputParameter = nullptr;
ATextFormat*            TheATextFormatParameter = nullptr;
ATextHeaders*           TheATextHeadersParameter = nullptr;
AOutputFilePath*        TheAOutputFilePathParameter = nullptr;

ASources*               TheASourcesParameter = nullptr;
ASourceRA*              TheASourceRAParameter = nullptr;
ASourceDec*             TheASourceDecParameter = nullptr;
ASourceMag_V*           TheASourceMag_VParameter = nullptr;
ASourceMag_B*           TheASourceMag_BParameter = nullptr;
ASourceMag_g*           TheASourceMag_gParameter = nullptr;
ASourceMag_r*           TheASourceMag_rParameter = nullptr;
ASourceMag_i*           TheASourceMag_iParameter = nullptr;
ASourceMag_z_s*         TheASourceMag_z_sParameter = nullptr;
ASourceMag_V_Error*     TheASourceMag_V_ErrorParameter = nullptr;
ASourceMag_B_Error*     TheASourceMag_B_ErrorParameter = nullptr;
ASourceMag_g_Error*     TheASourceMag_g_ErrorParameter = nullptr;
ASourceMag_r_Error*     TheASourceMag_r_ErrorParameter = nullptr;
ASourceMag_i_Error*     TheASourceMag_i_ErrorParameter = nullptr;
ASourceMag_z_s_Error*   TheASourceMag_z_s_ErrorParameter = nullptr;
ASourceFlags*           TheASourceFlagsParameter = nullptr;
AExcessCount*           TheAExcessCountParameter = nullptr;
ARejectCount*           TheARejectCountParameter = nullptr;
ATimeTotal*             TheATimeTotalParameter = nullptr;
ATimeIO*                TheATimeIOParameter = nullptr;
ACountIO*               TheACountIOParameter = nullptr;
ATimeUncompress*        TheATimeUncompressParameter = nullptr;
ATimeDecode*            TheATimeDecodeParameter = nullptr;
AIsValid*               TheAIsValidParameter = nullptr;
AOutputDataRelease*     TheAOutputDataReleaseParameter = nullptr;
ADatabaseFilePaths*     TheADatabaseFilePathsParameter = nullptr;
ADatabaseFilePath*      TheADatabaseFilePathParameter = nullptr;
ADatabaseMagnitudeLow*  TheADatabaseMagnitudeLowParameter = nullptr;
ADatabaseMagnitudeHigh* TheADatabaseMagnitudeHighParameter = nullptr;

// ----------------------------------------------------------------------------

ACommand::ACommand( MetaProcess* P )
   : MetaString( P )
{
   TheACommandParameter = this;
}

IsoString ACommand::Id() const
{
   return "command";
}

String ACommand::DefaultValue() const
{
   return "search";
}

// ----------------------------------------------------------------------------

ACenterRA::ACenterRA( MetaProcess* P )
   : MetaDouble( P )
{
   TheACenterRAParameter = this;
}

IsoString ACenterRA::Id() const
{
   return "centerRA";
}

int ACenterRA::Precision() const
{
   return 9;
}

double ACenterRA::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ACenterDec::ACenterDec( MetaProcess* P )
   : MetaDouble( P )
{
   TheACenterDecParameter = this;
}

IsoString ACenterDec::Id() const
{
   return "centerDec";
}

int ACenterDec::Precision() const
{
   return 9;
}

double ACenterDec::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ARadius::ARadius( MetaProcess* P )
   : MetaDouble( P )
{
   TheARadiusParameter = this;
}

IsoString ARadius::Id() const
{
   return "radius";
}

int ARadius::Precision() const
{
   return 6;
}

double ARadius::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

AMagnitudeLow::AMagnitudeLow( MetaProcess* P )
   : MetaFloat( P )
{
   TheAMagnitudeLowParameter = this;
}

IsoString AMagnitudeLow::Id() const
{
   return "magnitudeLow";
}

int AMagnitudeLow::Precision() const
{
   return 3;
}

double AMagnitudeLow::DefaultValue() const
{
   return -100;
}

// ----------------------------------------------------------------------------

AMagnitudeHigh::AMagnitudeHigh( MetaProcess* P )
   : MetaFloat( P )
{
   TheAMagnitudeHighParameter = this;
}

IsoString AMagnitudeHigh::Id() const
{
   return "magnitudeHigh";
}

int AMagnitudeHigh::Precision() const
{
   return 3;
}

double AMagnitudeHigh::DefaultValue() const
{
   return +100;
}

// ----------------------------------------------------------------------------

ASourceLimit::ASourceLimit( MetaProcess* P )
   : MetaUInt32( P )
{
   TheASourceLimitParameter = this;
}

IsoString ASourceLimit::Id() const
{
   return "sourceLimit";
}

double ASourceLimit::DefaultValue() const
{
   return uint32_max;
}

// ----------------------------------------------------------------------------

ARequiredFlags::ARequiredFlags( MetaProcess* P )
   : MetaUInt32( P )
{
   TheARequiredFlagsParameter = this;
}

IsoString ARequiredFlags::Id() const
{
   return "requiredFlags";
}

double ARequiredFlags::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

AInclusionFlags::AInclusionFlags( MetaProcess* P )
   : MetaUInt32( P )
{
   TheAInclusionFlagsParameter = this;
}

IsoString AInclusionFlags::Id() const
{
   return "inclusionFlags";
}

double AInclusionFlags::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

AExclusionFlags::AExclusionFlags( MetaProcess* P )
   : MetaUInt32( P )
{
   TheAExclusionFlagsParameter = this;
}

IsoString AExclusionFlags::Id() const
{
   return "exclusionFlags";
}

double AExclusionFlags::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

AVerbosity::AVerbosity( MetaProcess* P )
   : MetaInt32( P )
{
   TheAVerbosityParameter = this;
}

IsoString AVerbosity::Id() const
{
   return "verbosity";
}

double AVerbosity::DefaultValue() const
{
   return 2; // 0=silent 1=minimal 2=full
}

// ----------------------------------------------------------------------------

ADataRelease::ADataRelease( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheADataReleaseParameter = this;
}

IsoString ADataRelease::Id() const
{
   return "dataRelease";
}

size_type ADataRelease::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString ADataRelease::ElementId( size_type index ) const
{
   switch ( index )
   {
   case DR9:           return "DataRelease_9";
   default:
   case DR10:          return "DataRelease_10";
   case BestAvailable: return "DataRelease_BestAvailable";
   }
}

int ADataRelease::ElementValue( size_type index ) const
{
   return int( index );
}

size_type ADataRelease::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ASortBy::ASortBy( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheASortByParameter = this;
}

IsoString ASortBy::Id() const
{
   return "sortBy";
}

size_type ASortBy::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString ASortBy::ElementId( size_type index ) const
{
   switch ( index )
   {
   case DontSort: return "SortBy_DontSort";
   case RA:       return "SortBy_RA";
   case Dec:      return "SortBy_Dec";
   default:
   case V:        return "SortBy_V";
   case B:        return "SortBy_B";
   case g:        return "SortBy_g";
   case r:        return "SortBy_r";
   case i:        return "SortBy_i";
   case z_s:      return "SortBy_z_s";
   }
}

int ASortBy::ElementValue( size_type index ) const
{
   return int( index );
}

size_type ASortBy::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

AGenerateTextOutput::AGenerateTextOutput( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAGenerateTextOutputParameter = this;
}

IsoString AGenerateTextOutput::Id() const
{
   return "generateTextOutput";
}

bool AGenerateTextOutput::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ATextFormat::ATextFormat( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheATextFormatParameter = this;
}

IsoString ATextFormat::Id() const
{
   return "textFormat";
}

size_type ATextFormat::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString ATextFormat::ElementId( size_type index ) const
{
   switch ( index )
   {
   case CSV:             return "TextFormat_CSV";
   case Tabular:         return "TextFormat_Tabular";
   default:
   case TabularCompound: return "TextFormat_TabularCompound";
   }
}

int ATextFormat::ElementValue( size_type index ) const
{
   return int( index );
}

size_type ATextFormat::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ATextHeaders::ATextHeaders( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheATextHeadersParameter = this;
}

IsoString ATextHeaders::Id() const
{
   return "textHeaders";
}

size_type ATextHeaders::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString ATextHeaders::ElementId( size_type index ) const
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

int ATextHeaders::ElementValue( size_type index ) const
{
   return int( index );
}

size_type ATextHeaders::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

AOutputFilePath::AOutputFilePath( MetaProcess* P )
   : MetaString( P )
{
   TheAOutputFilePathParameter = this;
}

IsoString AOutputFilePath::Id() const
{
   return "outputFilePath";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ASources::ASources( MetaProcess* P )
   : MetaTable( P )
{
   TheASourcesParameter = this;
}

IsoString ASources::Id() const
{
   return "sources";
}

bool ASources::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceRA::ASourceRA( MetaTable* T )
   : MetaDouble( T )
{
   TheASourceRAParameter = this;
}

IsoString ASourceRA::Id() const
{
   return "ra";
}

int ASourceRA::Precision() const
{
   return 9;
}

bool ASourceRA::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceDec::ASourceDec( MetaTable* T )
   : MetaDouble( T )
{
   TheASourceDecParameter = this;
}

IsoString ASourceDec::Id() const
{
   return "dec";
}

int ASourceDec::Precision() const
{
   return 9;
}

bool ASourceDec::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_V::ASourceMag_V( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_VParameter = this;
}

IsoString ASourceMag_V::Id() const
{
   return "mag_V";
}

int ASourceMag_V::Precision() const
{
   return 3;
}

bool ASourceMag_V::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_B::ASourceMag_B( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_BParameter = this;
}

IsoString ASourceMag_B::Id() const
{
   return "mag_B";
}

int ASourceMag_B::Precision() const
{
   return 3;
}

bool ASourceMag_B::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_g::ASourceMag_g( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_gParameter = this;
}

IsoString ASourceMag_g::Id() const
{
   return "mag_g";
}

int ASourceMag_g::Precision() const
{
   return 3;
}

bool ASourceMag_g::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_r::ASourceMag_r( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_rParameter = this;
}

IsoString ASourceMag_r::Id() const
{
   return "mag_r";
}

int ASourceMag_r::Precision() const
{
   return 3;
}

bool ASourceMag_r::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_i::ASourceMag_i( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_iParameter = this;
}

IsoString ASourceMag_i::Id() const
{
   return "mag_i";
}

int ASourceMag_i::Precision() const
{
   return 3;
}

bool ASourceMag_i::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_z_s::ASourceMag_z_s( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_z_sParameter = this;
}

IsoString ASourceMag_z_s::Id() const
{
   return "mag_z_s";
}

int ASourceMag_z_s::Precision() const
{
   return 3;
}

bool ASourceMag_z_s::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_V_Error::ASourceMag_V_Error( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_V_ErrorParameter = this;
}

IsoString ASourceMag_V_Error::Id() const
{
   return "mag_V_error";
}

int ASourceMag_V_Error::Precision() const
{
   return 3;
}

bool ASourceMag_V_Error::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_B_Error::ASourceMag_B_Error( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_B_ErrorParameter = this;
}

IsoString ASourceMag_B_Error::Id() const
{
   return "mag_B_error";
}

int ASourceMag_B_Error::Precision() const
{
   return 3;
}

bool ASourceMag_B_Error::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_g_Error::ASourceMag_g_Error( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_g_ErrorParameter = this;
}

IsoString ASourceMag_g_Error::Id() const
{
   return "mag_g_error";
}

int ASourceMag_g_Error::Precision() const
{
   return 3;
}

bool ASourceMag_g_Error::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_r_Error::ASourceMag_r_Error( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_r_ErrorParameter = this;
}

IsoString ASourceMag_r_Error::Id() const
{
   return "mag_r_error";
}

int ASourceMag_r_Error::Precision() const
{
   return 3;
}

bool ASourceMag_r_Error::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_i_Error::ASourceMag_i_Error( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_i_ErrorParameter = this;
}

IsoString ASourceMag_i_Error::Id() const
{
   return "mag_i_error";
}

int ASourceMag_i_Error::Precision() const
{
   return 3;
}

bool ASourceMag_i_Error::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceMag_z_s_Error::ASourceMag_z_s_Error( MetaTable* T )
   : MetaFloat( T )
{
   TheASourceMag_z_s_ErrorParameter = this;
}

IsoString ASourceMag_z_s_Error::Id() const
{
   return "mag_z_s_error";
}

int ASourceMag_z_s_Error::Precision() const
{
   return 3;
}

bool ASourceMag_z_s_Error::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ASourceFlags::ASourceFlags( MetaTable* T )
   : MetaUInt32( T )
{
   TheASourceFlagsParameter = this;
}

IsoString ASourceFlags::Id() const
{
   return "flags";
}

bool ASourceFlags::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

AExcessCount::AExcessCount( MetaProcess* P )
   : MetaUInt32( P )
{
   TheAExcessCountParameter = this;
}

IsoString AExcessCount::Id() const
{
   return "excessCount";
}

bool AExcessCount::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ARejectCount::ARejectCount( MetaProcess* P )
   : MetaUInt32( P )
{
   TheARejectCountParameter = this;
}

IsoString ARejectCount::Id() const
{
   return "rejectCount";
}

bool ARejectCount::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ATimeTotal::ATimeTotal( MetaProcess* P )
   : MetaDouble( P )
{
   TheATimeTotalParameter = this;
}

IsoString ATimeTotal::Id() const
{
   return "timeTotal";
}

int ATimeTotal::Precision() const
{
   return 4;
}

bool ATimeTotal::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ATimeIO::ATimeIO( MetaProcess* P )
   : MetaDouble( P )
{
   TheATimeIOParameter = this;
}

IsoString ATimeIO::Id() const
{
   return "timeIO";
}

int ATimeIO::Precision() const
{
   return 4;
}

bool ATimeIO::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ACountIO::ACountIO( MetaProcess* P )
   : MetaUInt32( P )
{
   TheACountIOParameter = this;
}

IsoString ACountIO::Id() const
{
   return "countIO";
}

bool ACountIO::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ATimeUncompress::ATimeUncompress( MetaProcess* P )
   : MetaDouble( P )
{
   TheATimeUncompressParameter = this;
}

IsoString ATimeUncompress::Id() const
{
   return "timeUncompress";
}

int ATimeUncompress::Precision() const
{
   return 4;
}

bool ATimeUncompress::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ATimeDecode::ATimeDecode( MetaProcess* P )
   : MetaDouble( P )
{
   TheATimeDecodeParameter = this;
}

IsoString ATimeDecode::Id() const
{
   return "timeDecode";
}

int ATimeDecode::Precision() const
{
   return 4;
}

bool ATimeDecode::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

AIsValid::AIsValid( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAIsValidParameter = this;
}

IsoString AIsValid::Id() const
{
   return "isValid";
}

bool AIsValid::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

AOutputDataRelease::AOutputDataRelease( MetaProcess* P )
   : MetaInt32( P )
{
   TheAOutputDataReleaseParameter = this;
}

IsoString AOutputDataRelease::Id() const
{
   return "outputDataRelease";
}

bool AOutputDataRelease::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ADatabaseFilePaths::ADatabaseFilePaths( MetaProcess* P )
   : MetaTable( P )
{
   TheADatabaseFilePathsParameter = this;
}

IsoString ADatabaseFilePaths::Id() const
{
   return "databaseFilePaths";
}

bool ADatabaseFilePaths::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ADatabaseFilePath::ADatabaseFilePath( MetaTable* T )
   : MetaString( T )
{
   TheADatabaseFilePathParameter = this;
}

IsoString ADatabaseFilePath::Id() const
{
   return "filePath";
}

bool ADatabaseFilePath::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ADatabaseMagnitudeLow::ADatabaseMagnitudeLow( MetaProcess* P )
   : MetaFloat( P )
{
   TheADatabaseMagnitudeLowParameter = this;
}

IsoString ADatabaseMagnitudeLow::Id() const
{
   return "databaseMagnitudeLow";
}

int ADatabaseMagnitudeLow::Precision() const
{
   return 2;
}

bool ADatabaseMagnitudeLow::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ADatabaseMagnitudeHigh::ADatabaseMagnitudeHigh( MetaProcess* P )
   : MetaFloat( P )
{
   TheADatabaseMagnitudeHighParameter = this;
}

IsoString ADatabaseMagnitudeHigh::Id() const
{
   return "databaseMagnitudeHigh";
}

int ADatabaseMagnitudeHigh::Precision() const
{
   return 2;
}

bool ADatabaseMagnitudeHigh::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF APASSParameters.cpp - Released 2021-04-09T19:41:48Z
