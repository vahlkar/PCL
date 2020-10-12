//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.1
// ----------------------------------------------------------------------------
// Standard GaiaDR2 Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// GaiaDR2Parameters.h - Released 2020-10-12T19:25:16Z
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

#ifndef __GaiaDR2Parameters_h
#define __GaiaDR2Parameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class GDR2Command : public MetaString
{
public:

   GDR2Command( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern GDR2Command* TheGDR2CommandParameter;

// ----------------------------------------------------------------------------

class GDR2CenterRA : public MetaDouble
{
public:

   GDR2CenterRA( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern GDR2CenterRA* TheGDR2CenterRAParameter;

// ----------------------------------------------------------------------------

class GDR2CenterDec : public MetaDouble
{
public:

   GDR2CenterDec( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern GDR2CenterDec* TheGDR2CenterDecParameter;

// ----------------------------------------------------------------------------

class GDR2Radius : public MetaDouble
{
public:

   GDR2Radius( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern GDR2Radius* TheGDR2RadiusParameter;

// ----------------------------------------------------------------------------

class GDR2MagnitudeLow : public MetaFloat
{
public:

   GDR2MagnitudeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern GDR2MagnitudeLow* TheGDR2MagnitudeLowParameter;

// ----------------------------------------------------------------------------

class GDR2MagnitudeHigh : public MetaFloat
{
public:

   GDR2MagnitudeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern GDR2MagnitudeHigh* TheGDR2MagnitudeHighParameter;

// ----------------------------------------------------------------------------

class GDR2SourceLimit : public MetaUInt32
{
public:

   GDR2SourceLimit( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern GDR2SourceLimit* TheGDR2SourceLimitParameter;

// ----------------------------------------------------------------------------

class GDR2IncludeFlags : public MetaUInt32
{
public:

   GDR2IncludeFlags( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern GDR2IncludeFlags* TheGDR2IncludeFlagsParameter;

// ----------------------------------------------------------------------------

class GDR2ExcludeFlags : public MetaUInt32
{
public:

   GDR2ExcludeFlags( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern GDR2ExcludeFlags* TheGDR2ExcludeFlagsParameter;

// ----------------------------------------------------------------------------

class GDR2Verbosity : public MetaInt32
{
public:

   GDR2Verbosity( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern GDR2Verbosity* TheGDR2VerbosityParameter;

// ----------------------------------------------------------------------------

class GDR2SortBy : public MetaEnumeration
{
public:

   enum { DontSort,
          RA,
          Dec,
          G,
          G_BP,
          G_RP,
          Parallax,
          NumberOfItems,
          Default = G };

   GDR2SortBy( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern GDR2SortBy* TheGDR2SortByParameter;

// ----------------------------------------------------------------------------

class GDR2GenerateTextOutput : public MetaBoolean
{
public:

   GDR2GenerateTextOutput( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern GDR2GenerateTextOutput* TheGDR2GenerateTextOutputParameter;

// ----------------------------------------------------------------------------

class GDR2TextFormat : public MetaEnumeration
{
public:

   enum { CSV,
          Tabular,
          TabularCompound,
          NumberOfItems,
          Default = TabularCompound };

   GDR2TextFormat( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern GDR2TextFormat* TheGDR2TextFormatParameter;

// ----------------------------------------------------------------------------

class GDR2TextHeaders : public MetaEnumeration
{
public:

   enum { None,
          TableColumns,
          SearchParameters,
          SearchParametersAndTableColumns,
          NumberOfItems,
          Default = SearchParametersAndTableColumns };

   GDR2TextHeaders( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern GDR2TextHeaders* TheGDR2TextHeadersParameter;

// ----------------------------------------------------------------------------

class GDR2OutputFilePath : public MetaString
{
public:

   GDR2OutputFilePath( MetaProcess* );

   IsoString Id() const override;
};

extern GDR2OutputFilePath* TheGDR2OutputFilePathParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class GDR2Sources : public MetaTable
{
public:

   GDR2Sources( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GDR2Sources* TheGDR2SourcesParameter;

// ----------------------------------------------------------------------------

class GDR2SourceRA : public MetaDouble
{
public:

   GDR2SourceRA( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2SourceRA* TheGDR2SourceRAParameter;

// ----------------------------------------------------------------------------

class GDR2SourceDec : public MetaDouble
{
public:

   GDR2SourceDec( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2SourceDec* TheGDR2SourceDecParameter;

// ----------------------------------------------------------------------------

class GDR2SourceParx : public MetaFloat
{
public:

   GDR2SourceParx( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2SourceParx* TheGDR2SourceParxParameter;

// ----------------------------------------------------------------------------

class GDR2SourcePMRA : public MetaFloat
{
public:

   GDR2SourcePMRA( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2SourcePMRA* TheGDR2SourcePMRAParameter;

// ----------------------------------------------------------------------------

class GDR2SourcePMDec : public MetaFloat
{
public:

   GDR2SourcePMDec( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2SourcePMDec* TheGDR2SourcePMDecParameter;

// ----------------------------------------------------------------------------

class GDR2SourceMagG : public MetaFloat
{
public:

   GDR2SourceMagG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2SourceMagG* TheGDR2SourceMagGParameter;

// ----------------------------------------------------------------------------

class GDR2SourceMagBP : public MetaFloat
{
public:

   GDR2SourceMagBP( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2SourceMagBP* TheGDR2SourceMagBPParameter;

// ----------------------------------------------------------------------------

class GDR2SourceMagRP : public MetaFloat
{
public:

   GDR2SourceMagRP( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2SourceMagRP* TheGDR2SourceMagRPParameter;

// ----------------------------------------------------------------------------

class GDR2SourceFlags : public MetaUInt32
{
public:

   GDR2SourceFlags( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GDR2SourceFlags* TheGDR2SourceFlagsParameter;

// ----------------------------------------------------------------------------

class GDR2ExcessCount : public MetaUInt32
{
public:

   GDR2ExcessCount( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GDR2ExcessCount* TheGDR2ExcessCountParameter;

// ----------------------------------------------------------------------------

class GDR2RejectCount : public MetaUInt32
{
public:

   GDR2RejectCount( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GDR2RejectCount* TheGDR2RejectCountParameter;

// ----------------------------------------------------------------------------

class GDR2TimeTotal : public MetaDouble
{
public:

   GDR2TimeTotal( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2TimeTotal* TheGDR2TimeTotalParameter;

// ----------------------------------------------------------------------------

class GDR2TimeIO : public MetaDouble
{
public:

   GDR2TimeIO( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2TimeIO* TheGDR2TimeIOParameter;

// ----------------------------------------------------------------------------

class GDR2CountIO : public MetaUInt32
{
public:

   GDR2CountIO( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GDR2CountIO* TheGDR2CountIOParameter;

// ----------------------------------------------------------------------------

class GDR2TimeUncompress : public MetaDouble
{
public:

   GDR2TimeUncompress( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2TimeUncompress* TheGDR2TimeUncompressParameter;

// ----------------------------------------------------------------------------

class GDR2TimeDecode : public MetaDouble
{
public:

   GDR2TimeDecode( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2TimeDecode* TheGDR2TimeDecodeParameter;

// ----------------------------------------------------------------------------

class GDR2DatabaseFilePaths : public MetaTable
{
public:

   GDR2DatabaseFilePaths( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GDR2DatabaseFilePaths* TheGDR2DatabaseFilePathsParameter;

// ----------------------------------------------------------------------------

class GDR2DatabaseFilePath : public MetaString
{
public:

   GDR2DatabaseFilePath( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GDR2DatabaseFilePath* TheGDR2DatabaseFilePathParameter;

// ----------------------------------------------------------------------------

class GDR2DatabaseMagnitudeLow : public MetaFloat
{
public:

   GDR2DatabaseMagnitudeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2DatabaseMagnitudeLow* TheGDR2DatabaseMagnitudeLowParameter;

// ----------------------------------------------------------------------------

class GDR2DatabaseMagnitudeHigh : public MetaFloat
{
public:

   GDR2DatabaseMagnitudeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDR2DatabaseMagnitudeHigh* TheGDR2DatabaseMagnitudeHighParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __GaiaDR2Parameters_h

// ----------------------------------------------------------------------------
// EOF GaiaDR2Parameters.h - Released 2020-10-12T19:25:16Z
