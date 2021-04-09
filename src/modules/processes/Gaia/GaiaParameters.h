//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Gaia Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// GaiaParameters.h - Released 2021-04-09T19:41:48Z
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

#ifndef __GaiaParameters_h
#define __GaiaParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class GCommand : public MetaString
{
public:

   GCommand( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern GCommand* TheGCommandParameter;

// ----------------------------------------------------------------------------

class GCenterRA : public MetaDouble
{
public:

   GCenterRA( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern GCenterRA* TheGCenterRAParameter;

// ----------------------------------------------------------------------------

class GCenterDec : public MetaDouble
{
public:

   GCenterDec( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern GCenterDec* TheGCenterDecParameter;

// ----------------------------------------------------------------------------

class GRadius : public MetaDouble
{
public:

   GRadius( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern GRadius* TheGRadiusParameter;

// ----------------------------------------------------------------------------

class GMagnitudeLow : public MetaFloat
{
public:

   GMagnitudeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern GMagnitudeLow* TheGMagnitudeLowParameter;

// ----------------------------------------------------------------------------

class GMagnitudeHigh : public MetaFloat
{
public:

   GMagnitudeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern GMagnitudeHigh* TheGMagnitudeHighParameter;

// ----------------------------------------------------------------------------

class GSourceLimit : public MetaUInt32
{
public:

   GSourceLimit( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern GSourceLimit* TheGSourceLimitParameter;

// ----------------------------------------------------------------------------

class GRequiredFlags : public MetaUInt32
{
public:

   GRequiredFlags( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern GRequiredFlags* TheGRequiredFlagsParameter;

// ----------------------------------------------------------------------------

class GInclusionFlags : public MetaUInt32
{
public:

   GInclusionFlags( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern GInclusionFlags* TheGInclusionFlagsParameter;

// ----------------------------------------------------------------------------

class GExclusionFlags : public MetaUInt32
{
public:

   GExclusionFlags( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern GExclusionFlags* TheGExclusionFlagsParameter;

// ----------------------------------------------------------------------------

class GVerbosity : public MetaInt32
{
public:

   GVerbosity( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern GVerbosity* TheGVerbosityParameter;

// ----------------------------------------------------------------------------

class GDataRelease : public MetaEnumeration
{
public:

   enum { DR2,
          EDR3,
          BestAvailable,
          NumberOfItems,
          Default = EDR3 };

   GDataRelease( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static IsoString ReleaseName( pcl_enum dr )
   {
      switch ( dr )
      {
      case DR2:  return "DR2";
      default:
      case EDR3: return "EDR3";
      }
   }
};

extern GDataRelease* TheGDataReleaseParameter;

// ----------------------------------------------------------------------------

class GSortBy : public MetaEnumeration
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

   GSortBy( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern GSortBy* TheGSortByParameter;

// ----------------------------------------------------------------------------

class GGenerateTextOutput : public MetaBoolean
{
public:

   GGenerateTextOutput( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern GGenerateTextOutput* TheGGenerateTextOutputParameter;

// ----------------------------------------------------------------------------

class GTextFormat : public MetaEnumeration
{
public:

   enum { CSV,
          Tabular,
          TabularCompound,
          NumberOfItems,
          Default = TabularCompound };

   GTextFormat( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern GTextFormat* TheGTextFormatParameter;

// ----------------------------------------------------------------------------

class GTextHeaders : public MetaEnumeration
{
public:

   enum { None,
          TableColumns,
          SearchParameters,
          SearchParametersAndTableColumns,
          NumberOfItems,
          Default = SearchParametersAndTableColumns };

   GTextHeaders( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern GTextHeaders* TheGTextHeadersParameter;

// ----------------------------------------------------------------------------

class GOutputFilePath : public MetaString
{
public:

   GOutputFilePath( MetaProcess* );

   IsoString Id() const override;
};

extern GOutputFilePath* TheGOutputFilePathParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class GSources : public MetaTable
{
public:

   GSources( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GSources* TheGSourcesParameter;

// ----------------------------------------------------------------------------

class GSourceRA : public MetaDouble
{
public:

   GSourceRA( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GSourceRA* TheGSourceRAParameter;

// ----------------------------------------------------------------------------

class GSourceDec : public MetaDouble
{
public:

   GSourceDec( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GSourceDec* TheGSourceDecParameter;

// ----------------------------------------------------------------------------

class GSourceParx : public MetaFloat
{
public:

   GSourceParx( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GSourceParx* TheGSourceParxParameter;

// ----------------------------------------------------------------------------

class GSourcePMRA : public MetaFloat
{
public:

   GSourcePMRA( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GSourcePMRA* TheGSourcePMRAParameter;

// ----------------------------------------------------------------------------

class GSourcePMDec : public MetaFloat
{
public:

   GSourcePMDec( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GSourcePMDec* TheGSourcePMDecParameter;

// ----------------------------------------------------------------------------

class GSourceMagG : public MetaFloat
{
public:

   GSourceMagG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GSourceMagG* TheGSourceMagGParameter;

// ----------------------------------------------------------------------------

class GSourceMagBP : public MetaFloat
{
public:

   GSourceMagBP( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GSourceMagBP* TheGSourceMagBPParameter;

// ----------------------------------------------------------------------------

class GSourceMagRP : public MetaFloat
{
public:

   GSourceMagRP( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GSourceMagRP* TheGSourceMagRPParameter;

// ----------------------------------------------------------------------------

class GSourceFlags : public MetaUInt32
{
public:

   GSourceFlags( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GSourceFlags* TheGSourceFlagsParameter;

// ----------------------------------------------------------------------------

class GExcessCount : public MetaUInt32
{
public:

   GExcessCount( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GExcessCount* TheGExcessCountParameter;

// ----------------------------------------------------------------------------

class GRejectCount : public MetaUInt32
{
public:

   GRejectCount( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GRejectCount* TheGRejectCountParameter;

// ----------------------------------------------------------------------------

class GTimeTotal : public MetaDouble
{
public:

   GTimeTotal( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GTimeTotal* TheGTimeTotalParameter;

// ----------------------------------------------------------------------------

class GTimeIO : public MetaDouble
{
public:

   GTimeIO( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GTimeIO* TheGTimeIOParameter;

// ----------------------------------------------------------------------------

class GCountIO : public MetaUInt32
{
public:

   GCountIO( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GCountIO* TheGCountIOParameter;

// ----------------------------------------------------------------------------

class GTimeUncompress : public MetaDouble
{
public:

   GTimeUncompress( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GTimeUncompress* TheGTimeUncompressParameter;

// ----------------------------------------------------------------------------

class GTimeDecode : public MetaDouble
{
public:

   GTimeDecode( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GTimeDecode* TheGTimeDecodeParameter;

// ----------------------------------------------------------------------------

class GIsValid : public MetaBoolean
{
public:

   GIsValid( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GIsValid* TheGIsValidParameter;

// ----------------------------------------------------------------------------

class GOutputDataRelease : public MetaInt32
{
public:

   GOutputDataRelease( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GOutputDataRelease* TheGOutputDataReleaseParameter;

// ----------------------------------------------------------------------------

class GDatabaseFilePaths : public MetaTable
{
public:

   GDatabaseFilePaths( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GDatabaseFilePaths* TheGDatabaseFilePathsParameter;

// ----------------------------------------------------------------------------

class GDatabaseFilePath : public MetaString
{
public:

   GDatabaseFilePath( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern GDatabaseFilePath* TheGDatabaseFilePathParameter;

// ----------------------------------------------------------------------------

class GDatabaseMagnitudeLow : public MetaFloat
{
public:

   GDatabaseMagnitudeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDatabaseMagnitudeLow* TheGDatabaseMagnitudeLowParameter;

// ----------------------------------------------------------------------------

class GDatabaseMagnitudeHigh : public MetaFloat
{
public:

   GDatabaseMagnitudeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern GDatabaseMagnitudeHigh* TheGDatabaseMagnitudeHighParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __GaiaParameters_h

// ----------------------------------------------------------------------------
// EOF GaiaParameters.h - Released 2021-04-09T19:41:48Z
