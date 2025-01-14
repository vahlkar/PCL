//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 1.2.5
// ----------------------------------------------------------------------------
// BlinkProcess.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
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

#include "BlinkProcess.h"
#include "BlinkInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

BlinkProcess* TheBlinkProcess = nullptr;

// ----------------------------------------------------------------------------

BlinkProcess::BlinkProcess()
{
   TheBlinkProcess = this;
}

// ----------------------------------------------------------------------------

IsoString BlinkProcess::Id() const
{
   return "Blink";
}

// ----------------------------------------------------------------------------

IsoString BlinkProcess::Category() const
{
   return "ImageInspection";
}

// ----------------------------------------------------------------------------

String BlinkProcess::Description() const
{
   return
   "<html>"
   "<p>The Blink tool shows a set of images like a movie to help compare images "
   "visually and sort them by their quality or other criteria.</p>"
   "<p>Blink also produces reports with image statistics and FITS keywords, and "
   "can also generate video files for selected regions with optional automatic "
   "screen stretch functions applied.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String BlinkProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Blink.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* BlinkProcess::DefaultInterface() const
{
   return TheBlinkInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* BlinkProcess::Create() const
{
   return nullptr;
}

// ----------------------------------------------------------------------------

ProcessImplementation* BlinkProcess::Clone( const ProcessImplementation& ) const
{
   return nullptr;
}

// ----------------------------------------------------------------------------

bool BlinkProcess::IsAssignable() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BlinkProcess.cpp - Released 2024-12-28T16:54:15Z
