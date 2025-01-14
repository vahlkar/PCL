//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard RAW File Format Module Version 1.6.3
// ----------------------------------------------------------------------------
// RawFormat.cpp - Released 2024-12-28T16:54:05Z
// ----------------------------------------------------------------------------
// This file is part of the standard RAW PixInsight module.
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

#include "RawFormat.h"
#include "RawInstance.h"
#include "RawPreferencesDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IsoString RawFormat::Name() const
{
   return "RAW";
}

// ----------------------------------------------------------------------------

StringList RawFormat::FileExtensions() const
{
   return StringList() << ".3fr"  // Hasselblad
                       << ".ari"  // ARRIFLEX ARRIRAW format
                       << ".arw"  // Sony RAW file
                       << ".bay"  // Casio
                       << ".crw"  // Canon photo RAW
                       << ".cr2"  // Canon photo RAW
                       << ".cr3"  // Canon photo RAW
                       << ".cap"  // Phase_One
                       << ".dcs"  // Kodak Photo RAW
                       << ".dcr"  // Kodak Photo RAW
                       << ".dng"  // Adobe Digital Negative
                       << ".drf"  // Kodak Photo RAW
                       << ".eip"  // Phase_One
                       << ".erf"  // Epson
                       << ".fff"  // Imacon/Hasselblad RAW
                       << ".iiq"  // Phase_One
                       << ".k25"  // Kodak Photo RAW
                       << ".kdc"  // Kodak Photo RAW
                       << ".mdc"  // Minolta / Agfa
                       << ".mef"  // Mamiya photo RAW
                       << ".mos"  // CREO Photo RAW
                       << ".mrw"  // Minolta/Konica photo RAW
                       << ".nef"  // Nikon Electronic format
                       << ".nrw"  // Nikon Electronic format
                       << ".obm"  // Olympus Raw File
                       << ".orf"  // Olympus Raw File
                       << ".pef"  // Pentax photo RAW
                       << ".ptx"  // Pentax photo RAW
                       << ".pxn"  // Logitech Fotoman Pixtura
                       << ".r3d"  // RED
                       << ".raf"  // FUJI photo RAW
                       << ".raw"  // Leica / Panasonic photo RAW
                       << ".rwl"  // Leica
                       << ".rw2"  // Panasonic photo RAW
                       << ".rwz"  // Rawzor
                       << ".sr2"  // Sony RAW file
                       << ".srf"  // Sony RAW file
                       << ".srw"  // Samsung
                       << ".x3f"; // Sigma
}

// ----------------------------------------------------------------------------

uint32 RawFormat::Version() const
{
   return 0x105;
}

// ----------------------------------------------------------------------------

String RawFormat::Description() const
{
   return "Digital camera raw file format decoder based on LibRaw";
}

// ----------------------------------------------------------------------------

String RawFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>This implementation of digital camera raw format decoding uses LibRaw 0.21.3 "
   "Release (official version released 18 September 2024).</p>"

   "<p><b>LibRaw</b><br/>"
   "Copyright (C) 2008-2024 LibRaw LLC (https://www.libraw.org, info@libraw.org)</p>"

   "<p>LibRaw uses code from dcraw.c -- Dave Coffin's raw photo decoder,<br/>"
   "dcraw.c is copyright 1997-2018 by Dave Coffin, dcoffin a cybercom o net.<br/>"
   "LibRaw do not use RESTRICTED code from dcraw.c</p>"

   "<p>LibRaw uses DCB demosaic and FBDD denoise licensed under BSD-like 3-clause license<br/>"
   "DCB and FBDD are Copyright (C) 2010, Jacek Gozdz (cuniek@kft.umcs.lublin.pl)</p>"

   "<p>LibRaw uses X3F library to unpack Foveon Files, licensed BSD-style license<br/>"
   "Copyright (c) 2010, Roland Karlsson (roland@proxel.se). All rights reserved.</p>"

   "<p>LibRaw uses pieces of code from Adobe DNG SDK 1.4,<br/>"
   "Copyright (c) 2005 Adobe Systems Incorporated, licensed under MIT license</p>"

   "<p><b>PixInsight Class Library (PCL)</b><br/>"
   "Copyright (c) 2003-2024 Pleiades Astrophoto</p>"

   "<p style=\"white-space:pre;\">"
"\n-------------------------------------------------------------------------------"
"\nRAW Format Hints              Description"
"\n============================  ================================================="
"\nraw                     (r )  Load a raw Bayer RGB image without interpolation,"
"\n                              no white balancing, no black point correction,"
"\n                              with unused frame areas cropped, no frame"
"\n                              rotation, no highlights clipping, and no noise"
"\n                              reduction."
"\n-------------------------------------------------------------------------------"
"\ncfa                     (r )  Load a monochrome raw CFA frame."
"\n-------------------------------------------------------------------------------"
"\nno-cfa                  (r )  Do not load a monochrome raw CFA frame."
"\n-------------------------------------------------------------------------------"
"\nbayer-drizzle           (r )  Load a raw Bayer RGB image without interpolation."
"\n-------------------------------------------------------------------------------"
"\nno-bayer-drizzle        (r )  Do not load a raw Bayer RGB image."
"\n-------------------------------------------------------------------------------"
"\nsuper-pixels            (r )  Use super-pixel interpolation. The image will"
"\n                              have 1/2 of its nominal dimensions. Only valid"
"\n                              for 2x2 Bayer CFA filters; ignored for X-Trans"
"\n                              sensors."
"\n-------------------------------------------------------------------------------"
"\nno-super-pixels         (r )  Do not use super-pixel interpolation."
"\n-------------------------------------------------------------------------------"
"\nrgb                     (r )  Load an RGB image with VNG interpolation, camera"
"\n                              white balance applied, black point correction"
"\n                              applied, automatic frame rotation applied, unused"
"\n                              frame areas cropped, and no noise reduction."
"\n-------------------------------------------------------------------------------"
"\npreview                 (r )  Load a half-size interpolated image suitable for"
"\n                              thumbnail previews."
"\n-------------------------------------------------------------------------------"
"\nbilinear                (r )  Use bilinear interpolation."
"\n-------------------------------------------------------------------------------"
"\nfast                    (r )  Use bilinear interpolation (same as bilinear)."
"\n-------------------------------------------------------------------------------"
"\nvng                     (r )  Use VNG interpolation."
"\n-------------------------------------------------------------------------------"
"\nppg                     (r )  Use PPG interpolation."
"\n-------------------------------------------------------------------------------"
"\nahd                     (r )  Use AHD interpolation."
"\n-------------------------------------------------------------------------------"
"\ndcb                     (r )  Use DCB interpolation."
"\n-------------------------------------------------------------------------------"
"\ndcb-iterations n        (r )  Apply n iterations of DCB interpolation, with n"
"\n                              in the range [1,5]."
"\n-------------------------------------------------------------------------------"
"\ndcb-refinement          (r )  Enable a refinement step for DCB interpolation."
"\n-------------------------------------------------------------------------------"
"\nno-dcb-refinement       (r )  Disable a refinement step for DCB interpolation."
"\n-------------------------------------------------------------------------------"
"\nhalf-size               (r )  Use half-size interpolation. 2x faster than"
"\n                              bilinear, the image will have 1/2 of its nominal"
"\n                              dimensions."
"\n-------------------------------------------------------------------------------"
"\ninterpolate-as-4-colors (r )  Interpolate RGGB as four colors."
"\n-------------------------------------------------------------------------------"
"\nno-interpolate-as-4-colors"
"\n                        (r )  Do not interpolate RGGB as four colors."
"\n-------------------------------------------------------------------------------"
"\nauto-white-balance      (r )  Calculate white balancing factors by averaging"
"\n                              the entire image."
"\n-------------------------------------------------------------------------------"
"\nno-auto-white-balance   (r )  Do not apply an automatic white balance."
"\n-------------------------------------------------------------------------------"
"\ncamera-white-balance    (r )  Use the white balance specified by the camera,"
"\n                              when available."
"\n-------------------------------------------------------------------------------"
"\nno-camera-white-balance (r )  Do not apply a camera white balance."
"\n-------------------------------------------------------------------------------"
"\nno-white-balance        (r )  Do not apply any white balance."
"\n-------------------------------------------------------------------------------"
"\ndaylight-white-balance  (r )  Apply daylight white balance factors."
"\n-------------------------------------------------------------------------------"
"\nauto-flip               (r )  Apply an automatic frame rotation based on"
"\n                              existing metadata."
"\n-------------------------------------------------------------------------------"
"\nno-auto-flip            (r )  Do not rotate frames."
"\n-------------------------------------------------------------------------------"
"\nauto-crop               (r )  Crop unused frame regions automatically."
"\n-------------------------------------------------------------------------------"
"\nno-auto-crop            (r )  Do not crop unused frame regions."
"\n-------------------------------------------------------------------------------"
"\nblack-point-correction  (r )  Apply a black point correction to remove darkness"
"\n                              level pedestals."
"\n-------------------------------------------------------------------------------"
"\nno-black-point-correction"
"\n                        (r )  Do not apply black point correction."
"\n-------------------------------------------------------------------------------"
"\nclip-highlights         (r )  Clip the highlights to solid white."
"\n-------------------------------------------------------------------------------"
"\nno-clip-highlights      (r )  Leave the highlights unclipped, resulting in"
"\n                              various shades of pink."
"\n-------------------------------------------------------------------------------"
"\nforce-focal-length      (r )  Ignore existing focal length metadata and force a"
"\n                              user-defined focal length (via preferences)."
"\n-------------------------------------------------------------------------------"
"\nno-force-focal-length   (r )  Preserve existing focal length metadata."
"\n-------------------------------------------------------------------------------"
"\nfocal-length F          (r )  Ignore existing focal length metadata and force a"
"\n                              focal length F in millimeters. If F = 0, do not"
"\n                              generate focal length metadata."
"\n-------------------------------------------------------------------------------"
"\nforce-aperture          (r )  Ignore existing aperture metadata and force a"
"\n                              user-defined aperture (via preferences)."
"\n-------------------------------------------------------------------------------"
"\nno-force-aperture       (r )  Preserve existing aperture metadata."
"\n-------------------------------------------------------------------------------"
"\naperture D              (r )  Ignore existing aperture metadata and force an"
"\n                              aperture diameter D in millimeters. If D = 0, do"
"\n                              not generate aperture metadata."
"\n-------------------------------------------------------------------------------"
"\nwavelet-noise-threshold n"
"\n                        (r )  Apply a wavelet-based noise reduction with"
"\n                              threshold n in the range [0,2000]."
"\n-------------------------------------------------------------------------------"
"\nno-wavelet-noise-reduction"
"\n                        (r )  Do not apply a wavelet-based noise reduction."
"\n-------------------------------------------------------------------------------"
"\nfbdd-noise-threshold n  (r )  Apply FBDD noise reduction with quality n in the"
"\n                              range [0,2], where 0=disabled, 1=normal, 2=high."
"\n-------------------------------------------------------------------------------"
"\nno-fbdd-noise-reduction (r )  Disable the FBDD noise reduction algorithm."
"\n-------------------------------------------------------------------------------"
"\nverbosity n             (r )  n is a verbosity level in the range [0,3] to"
"\n                              control the amount of generated messages"
"\n                              (default = 2)."
"\n-------------------------------------------------------------------------------"
"\n"
   "</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String RawFormat::IconImageSVGFile() const
{
   return "@module_icons_dir/RAW.svg";
}

// ----------------------------------------------------------------------------

bool RawFormat::CanWrite() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool RawFormat::CanStore8Bit() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool RawFormat::CanStoreAlphaChannels() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool RawFormat::CanStoreResolution() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool RawFormat::CanStoreKeywords() const
{
   // We simulate some standard FITS header keywords from EXIF medatata:
   // DATE-OBS, EXPTIME, FOCALLEN, APTDIA
   return true;
}

// ----------------------------------------------------------------------------

bool RawFormat::CanStoreICCProfiles() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool RawFormat::CanStoreThumbnails() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool RawFormat::CanStoreImageProperties() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool RawFormat::SupportsViewProperties() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool RawFormat::CanEditPreferences() const
{
   return true;
}

// ----------------------------------------------------------------------------

FileFormatImplementation* RawFormat::Create() const
{
   return new RawInstance( this );
}

// ----------------------------------------------------------------------------

bool RawFormat::EditPreferences() const
{
   RawPreferences myRawPreferences;
   myRawPreferences.LoadFromSettings();

   RawPreferencesDialog d( myRawPreferences );
   if ( d.Execute() == StdDialogCode::Ok )
   {
      myRawPreferences.SaveToSettings();
      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RawFormat.cpp - Released 2024-12-28T16:54:05Z
