//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.1
// ----------------------------------------------------------------------------
// GrayscaleRGBProcesses.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "GrayscaleRGBProcesses.h"
#include "GrayscaleRGBInstances.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ConvertToGrayscaleProcess* TheConvertToGrayscaleProcess = nullptr;

// ----------------------------------------------------------------------------

ConvertToGrayscaleProcess::ConvertToGrayscaleProcess()
{
   TheConvertToGrayscaleProcess = this;
}

// ----------------------------------------------------------------------------

IsoString ConvertToGrayscaleProcess::Id() const
{
   return "ConvertToGrayscale";
}

// ----------------------------------------------------------------------------

IsoString ConvertToGrayscaleProcess::Categories() const
{
   return "ColorSpaces, ColorSpaceConversion";
}

// ----------------------------------------------------------------------------

String ConvertToGrayscaleProcess::Description() const
{
   return

   "<html>"
   "<p>ConvertToGrayscale transforms pixels from the RGB color space to the monochrome "
   "grayscale space. Grayscale images have a single nominal channel instead of the "
   "three nominal channels defining RGB images, and are visually rendered by 256 "
   "tones of gray in PixInsight.</p>"

   "<p>Grayscale pixel values are calculated as the L* component of CIE L*a*b* from RGB "
   "source pixels. The transformation is carried out in the RGB Working Space (RGBWS) "
   "of the target image. Existing alpha channels are not modified by ConvertToGrayscale.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String ConvertToGrayscaleProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ConvertToGrayscale.svg";
}

// ----------------------------------------------------------------------------

ProcessImplementation* ConvertToGrayscaleProcess::Create() const
{
   return new ConvertToGrayscaleInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ConvertToGrayscaleProcess::Clone( const ProcessImplementation& p ) const
{
   const ConvertToGrayscaleInstance* instPtr = dynamic_cast<const ConvertToGrayscaleInstance*>( &p );
   return (instPtr != nullptr) ? new ConvertToGrayscaleInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool ConvertToGrayscaleProcess::IsAssignable() const
{
   return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ConvertToRGBColorProcess* TheConvertToRGBColorProcess = nullptr;

// ----------------------------------------------------------------------------

ConvertToRGBColorProcess::ConvertToRGBColorProcess()
{
   TheConvertToRGBColorProcess = this;
}

// ----------------------------------------------------------------------------

IsoString ConvertToRGBColorProcess::Id() const
{
   return "ConvertToRGBColor";
}

// ----------------------------------------------------------------------------

IsoString ConvertToRGBColorProcess::Categories() const
{
   return "ColorSpaces, ColorSpaceConversion";
}

// ----------------------------------------------------------------------------

String ConvertToRGBColorProcess::Description() const
{
   return
   "<html>"
   "<p>ConvertToRGBColor transforms pixels from the monochrome gray space to the RGB "
   "color space. RGB color images have three nominal channels corresponding to "
   "their red, green and blue pixel intensities.</p>"

   "<p>ConvertToRGBColor simply copies grayscale source pixel values to each individual "
   "RGB color channel of the resulting image. Existing alpha channels are not "
   "modified by ConvertToRGBColor.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String ConvertToRGBColorProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ConvertToRGBColor.svg";
}

// ----------------------------------------------------------------------------

ProcessImplementation* ConvertToRGBColorProcess::Create() const
{
   return new ConvertToRGBColorInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ConvertToRGBColorProcess::Clone( const ProcessImplementation& p ) const
{
   const ConvertToRGBColorInstance* instPtr = dynamic_cast<const ConvertToRGBColorInstance*>( &p );
   return (instPtr != nullptr) ? new ConvertToRGBColorInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool ConvertToRGBColorProcess::IsAssignable() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GrayscaleRGBProcesses.cpp - Released 2024-12-28T16:54:15Z
