//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AnnotationParameters.cpp - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Annotation PixInsight module.
//
// Copyright (c) 2010-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
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

#include "AnnotationParameters.h"

#include "pcl/Color.h"
#include "pcl/Font.h"

namespace pcl
{

// ----------------------------------------------------------------------------

AnnotationText*            TheAnnotationTextParameter = nullptr;
AnnotationFont*            TheAnnotationFontParameter = nullptr;
AnnotationFontSize*        TheAnnotationFontSizeParameter = nullptr;
AnnotationFontBold*        TheAnnotationFontBoldParameter = nullptr;
AnnotationFontItalic*      TheAnnotationFontItalicParameter = nullptr;
AnnotationFontUnderline*   TheAnnotationFontUnderlineParameter = nullptr;
AnnotationFontShadow*      TheAnnotationFontShadowParameter = nullptr;
AnnotationColor*           TheAnnotationColorParameter = nullptr;
AnnotationPositionX*       TheAnnotationPositionXParameter = nullptr;
AnnotationPositionY*       TheAnnotationPositionYParameter = nullptr;
AnnotationShowLeader*      TheAnnotationShowLeaderParameter = nullptr;
AnnotationLeaderPositionX* TheAnnotationLeaderPositionXParameter = nullptr;
AnnotationLeaderPositionY* TheAnnotationLeaderPositionYParameter = nullptr;
AnnotationOpacity*         TheAnnotationOpacityParameter = nullptr;

// ----------------------------------------------------------------------------

AnnotationText::AnnotationText( MetaProcess* P )
   : MetaString( P )
{
   TheAnnotationTextParameter = this;
}

IsoString AnnotationText::Id() const
{
   return "annotationText";
}

size_type AnnotationText::MinLength() const
{
   return 0;
}

String AnnotationText::DefaultValue() const
{
   return "Annotation";
}

// ----------------------------------------------------------------------------

AnnotationFont::AnnotationFont( MetaProcess* P )
   : MetaString( P )
{
   TheAnnotationFontParameter = this;
}

IsoString AnnotationFont::Id() const
{
   return "annotationFont";
}

size_type AnnotationFont::MinLength() const
{
   return 0;
}

String AnnotationFont::DefaultValue() const
{
   return FontFamily::FamilyToFace( FontFamily::Default );
}

// ----------------------------------------------------------------------------

AnnotationFontSize::AnnotationFontSize( MetaProcess* P )
   : MetaUInt8( P )
{
   TheAnnotationFontSizeParameter = this;
}

IsoString AnnotationFontSize::Id() const
{
   return "annotationFontSize";
}

double AnnotationFontSize::DefaultValue() const
{
   return 12;
}

// ----------------------------------------------------------------------------

AnnotationFontBold::AnnotationFontBold( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAnnotationFontBoldParameter = this;
}

IsoString AnnotationFontBold::Id() const
{
   return "annotationFontBold";
}

// ----------------------------------------------------------------------------

AnnotationFontItalic::AnnotationFontItalic( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAnnotationFontItalicParameter = this;
}

IsoString AnnotationFontItalic::Id() const
{
   return "annotationFontItalic";
}

// ----------------------------------------------------------------------------

AnnotationFontUnderline::AnnotationFontUnderline( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAnnotationFontUnderlineParameter = this;
}

IsoString AnnotationFontUnderline::Id() const
{
   return "annotationFontUnderline";
}

// ----------------------------------------------------------------------------

AnnotationFontShadow::AnnotationFontShadow( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAnnotationFontShadowParameter = this;
}

IsoString AnnotationFontShadow::Id() const
{
   return "annotationFontShadow";
}

// ----------------------------------------------------------------------------

AnnotationColor::AnnotationColor( MetaProcess* P )
   : MetaUInt32( P )
{
   TheAnnotationColorParameter = this;
}

IsoString AnnotationColor::Id() const
{
   return "annotationColor";
}

double AnnotationColor::DefaultValue() const
{
   return RGBAColor( 192, 192, 192 ); //Silver
}

// ----------------------------------------------------------------------------

AnnotationPositionX::AnnotationPositionX( MetaProcess* P )
   : MetaInt32( P )
{
   TheAnnotationPositionXParameter = this;
}

IsoString AnnotationPositionX::Id() const
{
   return "annotationPositionX";
}

// ----------------------------------------------------------------------------

AnnotationPositionY::AnnotationPositionY( MetaProcess* P )
   : MetaInt32( P )
{
   TheAnnotationPositionYParameter = this;
}

IsoString AnnotationPositionY::Id() const
{
   return "annotationPositionY";
}

// ----------------------------------------------------------------------------

AnnotationShowLeader::AnnotationShowLeader( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAnnotationShowLeaderParameter = this;
}

IsoString AnnotationShowLeader::Id() const
{
   return "annotationShowLeader";
}

// ----------------------------------------------------------------------------

AnnotationLeaderPositionX::AnnotationLeaderPositionX( MetaProcess* P )
   : MetaInt32( P )
{
   TheAnnotationLeaderPositionXParameter = this;
}

IsoString AnnotationLeaderPositionX::Id() const
{
   return "annotationLeaderPositionX";
}

// ----------------------------------------------------------------------------

AnnotationLeaderPositionY::AnnotationLeaderPositionY( MetaProcess* P )
   : MetaInt32( P )
{
   TheAnnotationLeaderPositionYParameter = this;
}

IsoString AnnotationLeaderPositionY::Id() const
{
   return "annotationLeaderPositionY";
}

// ----------------------------------------------------------------------------

AnnotationOpacity::AnnotationOpacity( MetaProcess* P )
   : MetaUInt8( P )
{
   TheAnnotationOpacityParameter = this;
}

IsoString AnnotationOpacity::Id() const
{
   return "annotationOpacity";
}

double AnnotationOpacity::DefaultValue() const
{
   return 255;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF AnnotationParameters.cpp - Released 2023-08-28T15:23:41Z
