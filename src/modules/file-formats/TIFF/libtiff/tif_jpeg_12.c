//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 1.0.10
// ----------------------------------------------------------------------------
// tif_jpeg_12.c - Released 2024-06-18T15:49:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
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

#include "tiffiop.h"

#if defined(HAVE_JPEGTURBO_DUAL_MODE_8_12)
#define JPEG_DUAL_MODE_8_12
#endif

#if defined(JPEG_DUAL_MODE_8_12)

#define FROM_TIF_JPEG_12

#ifdef TIFFInitJPEG
#undef TIFFInitJPEG
#endif
#define TIFFInitJPEG TIFFInitJPEG_12

#ifdef TIFFJPEGIsFullStripRequired
#undef TIFFJPEGIsFullStripRequired
#endif
#define TIFFJPEGIsFullStripRequired TIFFJPEGIsFullStripRequired_12

int TIFFInitJPEG_12(TIFF *tif, int scheme);

#if !defined(HAVE_JPEGTURBO_DUAL_MODE_8_12)
#include LIBJPEG_12_PATH
#endif

#include "tif_jpeg.c"

int TIFFReInitJPEG_12(TIFF *tif, const JPEGOtherSettings *otherSettings,
                      int scheme, int is_encode)
{
    JPEGState *sp;
    uint8_t *new_tif_data;

    (void)scheme;
    assert(scheme == COMPRESSION_JPEG);

    new_tif_data =
        (uint8_t *)_TIFFreallocExt(tif, tif->tif_data, sizeof(JPEGState));

    if (new_tif_data == NULL)
    {
        TIFFErrorExtR(tif, "TIFFReInitJPEG_12",
                      "No space for JPEG state block");
        return 0;
    }

    tif->tif_data = new_tif_data;
    _TIFFmemset(tif->tif_data, 0, sizeof(JPEGState));

    TIFFInitJPEGCommon(tif);

    sp = JState(tif);
    sp->otherSettings = *otherSettings;

    if (is_encode)
        return JPEGSetupEncode(tif);
    else
        return JPEGSetupDecode(tif);
}

#endif /* defined(JPEG_DUAL_MODE_8_12) */

// ----------------------------------------------------------------------------
// EOF tif_jpeg_12.c - Released 2024-06-18T15:49:17Z
