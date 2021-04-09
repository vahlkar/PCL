//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AnnotationParameters.h - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard Annotation PixInsight module.
//
// Copyright (c) 2010-2020 Zbynek Vrastil
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L.
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

#ifndef __AnnotationParameters_h
#define __AnnotationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class AnnotationText : public MetaString
{
public:

   AnnotationText( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String DefaultValue() const override;
};

extern AnnotationText* TheAnnotationTextParameter;

// ----------------------------------------------------------------------------

class AnnotationFont : public MetaString
{
public:

   AnnotationFont( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String DefaultValue() const override;
};

extern AnnotationFont* TheAnnotationFontParameter;

// ----------------------------------------------------------------------------

class AnnotationFontSize : public MetaUInt8
{
public:

   AnnotationFontSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern AnnotationFontSize* TheAnnotationFontSizeParameter;

// ----------------------------------------------------------------------------

class AnnotationFontBold : public MetaBoolean
{
public:

   AnnotationFontBold( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationFontBold* TheAnnotationFontBoldParameter;

// ----------------------------------------------------------------------------

class AnnotationFontItalic : public MetaBoolean
{
public:

   AnnotationFontItalic( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationFontItalic* TheAnnotationFontItalicParameter;

// ----------------------------------------------------------------------------

class AnnotationFontUnderline : public MetaBoolean
{
public:

   AnnotationFontUnderline( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationFontUnderline* TheAnnotationFontUnderlineParameter;

// ----------------------------------------------------------------------------

class AnnotationFontShadow : public MetaBoolean
{
public:

   AnnotationFontShadow( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationFontShadow* TheAnnotationFontShadowParameter;

// ----------------------------------------------------------------------------

class AnnotationColor : public MetaUInt32
{
public:

   AnnotationColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern AnnotationColor* TheAnnotationColorParameter;

// ----------------------------------------------------------------------------

class AnnotationPositionX : public MetaInt32
{
public:

   AnnotationPositionX( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationPositionX* TheAnnotationPositionXParameter;

// ----------------------------------------------------------------------------

class AnnotationPositionY : public MetaInt32
{
public:

   AnnotationPositionY( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationPositionY* TheAnnotationPositionYParameter;

// ----------------------------------------------------------------------------

class AnnotationShowLeader : public MetaBoolean
{
public:

   AnnotationShowLeader( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationShowLeader* TheAnnotationShowLeaderParameter;

// ----------------------------------------------------------------------------

class AnnotationLeaderPositionX : public MetaInt32
{
public:

   AnnotationLeaderPositionX( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationLeaderPositionX* TheAnnotationLeaderPositionXParameter;

// ----------------------------------------------------------------------------

class AnnotationLeaderPositionY : public MetaInt32
{
public:

   AnnotationLeaderPositionY( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationLeaderPositionY* TheAnnotationLeaderPositionYParameter;

// ----------------------------------------------------------------------------

class AnnotationOpacity : public MetaUInt8
{
public:

   AnnotationOpacity( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern AnnotationOpacity* TheAnnotationOpacityParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif // __AnnotationParameters_h

// ----------------------------------------------------------------------------
// EOF AnnotationParameters.h - Released 2021-04-09T19:41:49Z
