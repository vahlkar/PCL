//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Sandbox Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SandboxParameters.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard Sandbox PixInsight module.
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

#ifndef __SandboxParameters_h
#define __SandboxParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SandboxParameterOne : public MetaFloat
{
public:

   SandboxParameterOne( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SandboxParameterOne* TheSandboxParameterOneParameter;

// ----------------------------------------------------------------------------

class SandboxParameterTwo : public MetaInt32
{
public:

   SandboxParameterTwo( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SandboxParameterTwo* TheSandboxParameterTwoParameter;

// ----------------------------------------------------------------------------

class SandboxParameterThree : public MetaBoolean
{
public:

   SandboxParameterThree( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SandboxParameterThree* TheSandboxParameterThreeParameter;

// ----------------------------------------------------------------------------

class SandboxParameterFour : public MetaEnumeration
{
public:

   enum { FirstItem,
          SecondItem,
          ThirdItem,
          NumberOfItems,
          Default = FirstItem };

   SandboxParameterFour( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern SandboxParameterFour* TheSandboxParameterFourParameter;

// ----------------------------------------------------------------------------

class SandboxParameterFive : public MetaString
{
public:

   SandboxParameterFive( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
};

extern SandboxParameterFive* TheSandboxParameterFiveParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SandboxParameters_h

// ----------------------------------------------------------------------------
// EOF SandboxParameters.h - Released 2024-12-28T16:54:15Z
