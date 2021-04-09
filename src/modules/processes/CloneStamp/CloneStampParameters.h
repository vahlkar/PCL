//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// CloneStampParameters.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
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

#ifndef __CloneStampParameters_h
#define __CloneStampParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CSClonerTable : public MetaTable
{
public:

   CSClonerTable( MetaProcess* );

   IsoString Id() const override;
};

extern CSClonerTable* TheCSClonerTableParameter;

// ----------------------------------------------------------------------------

class CSActionIndex : public MetaUInt32
{
public:

   CSActionIndex( MetaTable* );

   IsoString Id() const override;
};

extern CSActionIndex* TheCSActionIndexParameter;

// ----------------------------------------------------------------------------

class CSTargetX : public MetaInt32
{
public:

   CSTargetX( MetaTable* );

   IsoString Id() const override;
};

extern CSTargetX* TheCSTargetXParameter;

// ----------------------------------------------------------------------------

class CSTargetY : public MetaInt32
{
public:

   CSTargetY( MetaTable* );

   IsoString Id() const override;
};

extern CSTargetY* TheCSTargetYParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CSActionTable : public MetaTable
{
public:

   CSActionTable( MetaProcess* );

   IsoString Id() const override;
};

extern CSActionTable* TheCSActionTableParameter;

// ----------------------------------------------------------------------------

class CSSourceIdentifier : public MetaString
{
public:

   CSSourceIdentifier( MetaTable* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern CSSourceIdentifier* TheCSSourceIdentifierParameter;

// ----------------------------------------------------------------------------

class CSSourceWidth : public MetaInt32
{
public:

   CSSourceWidth( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern CSSourceWidth* TheCSSourceWidthParameter;

// ----------------------------------------------------------------------------

class CSSourceHeight : public MetaInt32
{
public:

   CSSourceHeight( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern CSSourceHeight* TheCSSourceHeightParameter;

// ----------------------------------------------------------------------------

class CSSourceX : public MetaInt32
{
public:

   CSSourceX( MetaTable* );

   IsoString Id() const override;
};

extern CSSourceX* TheCSSourceXParameter;

// ----------------------------------------------------------------------------

class CSSourceY : public MetaInt32
{
public:

   CSSourceY( MetaTable* );

   IsoString Id() const override;
};

extern CSSourceY* TheCSSourceYParameter;

// ----------------------------------------------------------------------------

class CSRadius : public MetaInt32
{
public:

   CSRadius( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CSRadius* TheCSRadiusParameter;

// ----------------------------------------------------------------------------

class CSSoftness : public MetaFloat
{
public:

   CSSoftness( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CSSoftness* TheCSSoftnessParameter;

// ----------------------------------------------------------------------------

class CSOpacity : public MetaFloat
{
public:

   CSOpacity( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CSOpacity* TheCSOpacityParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CSWidth : public MetaInt32
{
public:

   CSWidth( MetaProcess* );

   IsoString Id() const override;
};

extern CSWidth* TheCSWidthParameter;

// ----------------------------------------------------------------------------

class CSHeight : public MetaInt32
{
public:

   CSHeight( MetaProcess* );

   IsoString Id() const override;
};

extern CSHeight* TheCSHeightParameter;

// ----------------------------------------------------------------------------

class CSColor : public MetaUInt32
{
public:

   CSColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern CSColor* TheCSColorParameter;

// ----------------------------------------------------------------------------

class CSBoundsColor : public MetaUInt32
{
public:

   CSBoundsColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern CSBoundsColor* TheCSBoundsColorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __CloneStampParameters_h

// ----------------------------------------------------------------------------
// EOF CloneStampParameters.h - Released 2021-04-09T19:41:48Z
