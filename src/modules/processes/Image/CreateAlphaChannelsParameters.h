//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.2
// ----------------------------------------------------------------------------
// CreateAlphaChannelsParameters.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#ifndef __CreateAlphaChannelsParameters_h
#define __CreateAlphaChannelsParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CAFromImage : public MetaBoolean
{
public:

   CAFromImage( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CAFromImage* TheCAFromImageParameter;

// ----------------------------------------------------------------------------

class CASourceImageIdentifier : public MetaString
{
public:

   CASourceImageIdentifier( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;   // 0 = automatic
   String AllowedCharacters() const override;
};

extern CASourceImageIdentifier* TheCASourceImageIdentifierParameter;

// ----------------------------------------------------------------------------

class CAInvertSourceImage : public MetaBoolean
{
public:

   CAInvertSourceImage( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CAInvertSourceImage* TheCAInvertSourceImageParameter;

// ----------------------------------------------------------------------------

class CACloseSourceImage : public MetaBoolean
{
public:

   CACloseSourceImage( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CACloseSourceImage* TheCACloseSourceImageParameter;

// ----------------------------------------------------------------------------

class CATransparency : public MetaDouble
{
public:

   CATransparency( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CATransparency* TheCATransparencyParameter;

// ----------------------------------------------------------------------------

class CAReplaceExistingChannels : public MetaBoolean
{
public:

   CAReplaceExistingChannels( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CAReplaceExistingChannels* TheCAReplaceExistingChannelsParameter;

// ----------------------------------------------------------------------------

class CAChannelCount : public MetaInt32
{
public:

   CAChannelCount( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern CAChannelCount* TheCAChannelCountParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __CreateAlphaChannelsParameters_h

// ----------------------------------------------------------------------------
// EOF CreateAlphaChannelsParameters.h - Released 2021-04-09T19:41:48Z
