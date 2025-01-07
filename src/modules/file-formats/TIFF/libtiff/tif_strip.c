//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 1.0.10
// ----------------------------------------------------------------------------
// tif_strip.c - Released 2024-12-28T16:54:05Z
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

/*
 * Copyright (c) 1991-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

/*
 * TIFF Library.
 *
 * Strip-organized Image Support Routines.
 */
#include "tiffiop.h"

/*
 * Compute which strip a (row,sample) value is in.
 */
uint32_t TIFFComputeStrip(TIFF *tif, uint32_t row, uint16_t sample)
{
    static const char module[] = "TIFFComputeStrip";
    TIFFDirectory *td = &tif->tif_dir;
    uint32_t strip;

    strip = row / td->td_rowsperstrip;
    if (td->td_planarconfig == PLANARCONFIG_SEPARATE)
    {
        if (sample >= td->td_samplesperpixel)
        {
            TIFFErrorExtR(tif, module, "%lu: Sample out of range, max %lu",
                          (unsigned long)sample,
                          (unsigned long)td->td_samplesperpixel);
            return (0);
        }
        strip += (uint32_t)sample * td->td_stripsperimage;
    }
    return (strip);
}

/*
 * Compute how many strips are in an image.
 */
uint32_t TIFFNumberOfStrips(TIFF *tif)
{
    TIFFDirectory *td = &tif->tif_dir;
    uint32_t nstrips;

    nstrips = (td->td_rowsperstrip == (uint32_t)-1
                   ? 1
                   : TIFFhowmany_32(td->td_imagelength, td->td_rowsperstrip));
    if (td->td_planarconfig == PLANARCONFIG_SEPARATE)
        nstrips =
            _TIFFMultiply32(tif, nstrips, (uint32_t)td->td_samplesperpixel,
                            "TIFFNumberOfStrips");
    return (nstrips);
}

/*
 * Compute the # bytes in a variable height, row-aligned strip.
 */
uint64_t TIFFVStripSize64(TIFF *tif, uint32_t nrows)
{
    static const char module[] = "TIFFVStripSize64";
    TIFFDirectory *td = &tif->tif_dir;
    if (nrows == (uint32_t)(-1))
        nrows = td->td_imagelength;
    if ((td->td_planarconfig == PLANARCONFIG_CONTIG) &&
        (td->td_photometric == PHOTOMETRIC_YCBCR) && (!isUpSampled(tif)))
    {
        /*
         * Packed YCbCr data contain one Cb+Cr for every
         * HorizontalSampling*VerticalSampling Y values.
         * Must also roundup width and height when calculating
         * since images that are not a multiple of the
         * horizontal/vertical subsampling area include
         * YCbCr data for the extended image.
         */
        uint16_t ycbcrsubsampling[2];
        uint16_t samplingblock_samples;
        uint32_t samplingblocks_hor;
        uint32_t samplingblocks_ver;
        uint64_t samplingrow_samples;
        uint64_t samplingrow_size;
        if (td->td_samplesperpixel != 3)
        {
            TIFFErrorExtR(tif, module, "Invalid td_samplesperpixel value");
            return 0;
        }
        TIFFGetFieldDefaulted(tif, TIFFTAG_YCBCRSUBSAMPLING,
                              ycbcrsubsampling + 0, ycbcrsubsampling + 1);
        if ((ycbcrsubsampling[0] != 1 && ycbcrsubsampling[0] != 2 &&
             ycbcrsubsampling[0] != 4) ||
            (ycbcrsubsampling[1] != 1 && ycbcrsubsampling[1] != 2 &&
             ycbcrsubsampling[1] != 4))
        {
            TIFFErrorExtR(tif, module, "Invalid YCbCr subsampling (%dx%d)",
                          ycbcrsubsampling[0], ycbcrsubsampling[1]);
            return 0;
        }
        samplingblock_samples = ycbcrsubsampling[0] * ycbcrsubsampling[1] + 2;
        samplingblocks_hor =
            TIFFhowmany_32(td->td_imagewidth, ycbcrsubsampling[0]);
        samplingblocks_ver = TIFFhowmany_32(nrows, ycbcrsubsampling[1]);
        samplingrow_samples = _TIFFMultiply64(tif, samplingblocks_hor,
                                              samplingblock_samples, module);
        samplingrow_size = TIFFhowmany8_64(_TIFFMultiply64(
            tif, samplingrow_samples, td->td_bitspersample, module));
        return (
            _TIFFMultiply64(tif, samplingrow_size, samplingblocks_ver, module));
    }
    else
        return (_TIFFMultiply64(tif, nrows, TIFFScanlineSize64(tif), module));
}
tmsize_t TIFFVStripSize(TIFF *tif, uint32_t nrows)
{
    static const char module[] = "TIFFVStripSize";
    uint64_t m;
    m = TIFFVStripSize64(tif, nrows);
    return _TIFFCastUInt64ToSSize(tif, m, module);
}

/*
 * Compute the # bytes in a raw strip.
 */
uint64_t TIFFRawStripSize64(TIFF *tif, uint32_t strip)
{
    static const char module[] = "TIFFRawStripSize64";
    uint64_t bytecount = TIFFGetStrileByteCount(tif, strip);

    if (bytecount == 0)
    {
        TIFFErrorExtR(tif, module,
                      "%" PRIu64 ": Invalid strip byte count, strip %lu",
                      (uint64_t)bytecount, (unsigned long)strip);
        bytecount = (uint64_t)-1;
    }

    return bytecount;
}
tmsize_t TIFFRawStripSize(TIFF *tif, uint32_t strip)
{
    static const char module[] = "TIFFRawStripSize";
    uint64_t m;
    tmsize_t n;
    m = TIFFRawStripSize64(tif, strip);
    if (m == (uint64_t)(-1))
        n = (tmsize_t)(-1);
    else
    {
        n = (tmsize_t)m;
        if ((uint64_t)n != m)
        {
            TIFFErrorExtR(tif, module, "Integer overflow");
            n = 0;
        }
    }
    return (n);
}

/*
 * Compute the # bytes in a (row-aligned) strip.
 *
 * Note that if RowsPerStrip is larger than the
 * recorded ImageLength, then the strip size is
 * truncated to reflect the actual space required
 * to hold the strip.
 */
uint64_t TIFFStripSize64(TIFF *tif)
{
    TIFFDirectory *td = &tif->tif_dir;
    uint32_t rps = td->td_rowsperstrip;
    if (rps > td->td_imagelength)
        rps = td->td_imagelength;
    return (TIFFVStripSize64(tif, rps));
}
tmsize_t TIFFStripSize(TIFF *tif)
{
    static const char module[] = "TIFFStripSize";
    uint64_t m;
    m = TIFFStripSize64(tif);
    return _TIFFCastUInt64ToSSize(tif, m, module);
}

/*
 * Compute a default strip size based on the image
 * characteristics and a requested value.  If the
 * request is <1 then we choose a strip size according
 * to certain heuristics.
 */
uint32_t TIFFDefaultStripSize(TIFF *tif, uint32_t request)
{
    return (*tif->tif_defstripsize)(tif, request);
}

uint32_t _TIFFDefaultStripSize(TIFF *tif, uint32_t s)
{
    if ((int32_t)s < 1)
    {
        /*
         * If RowsPerStrip is unspecified, try to break the
         * image up into strips that are approximately
         * STRIP_SIZE_DEFAULT bytes long.
         */
        uint64_t scanlinesize;
        uint64_t rows;
        scanlinesize = TIFFScanlineSize64(tif);
        if (scanlinesize == 0)
            scanlinesize = 1;
        rows = (uint64_t)STRIP_SIZE_DEFAULT / scanlinesize;
        if (rows == 0)
            rows = 1;
        else if (rows > 0xFFFFFFFF)
            rows = 0xFFFFFFFF;
        s = (uint32_t)rows;
    }
    return (s);
}

/*
 * Return the number of bytes to read/write in a call to
 * one of the scanline-oriented i/o routines.  Note that
 * this number may be 1/samples-per-pixel if data is
 * stored as separate planes.
 * The ScanlineSize in case of YCbCrSubsampling is defined as the
 * strip size divided by the strip height, i.e. the size of a pack of vertical
 * subsampling lines divided by vertical subsampling. It should thus make
 * sense when multiplied by a multiple of vertical subsampling.
 */
uint64_t TIFFScanlineSize64(TIFF *tif)
{
    static const char module[] = "TIFFScanlineSize64";
    TIFFDirectory *td = &tif->tif_dir;
    uint64_t scanline_size;
    if (td->td_planarconfig == PLANARCONFIG_CONTIG)
    {
        if ((td->td_photometric == PHOTOMETRIC_YCBCR) &&
            (td->td_samplesperpixel == 3) && (!isUpSampled(tif)))
        {
            uint16_t ycbcrsubsampling[2];
            uint16_t samplingblock_samples;
            uint32_t samplingblocks_hor;
            uint64_t samplingrow_samples;
            uint64_t samplingrow_size;
            if (td->td_samplesperpixel != 3)
            {
                TIFFErrorExtR(tif, module, "Invalid td_samplesperpixel value");
                return 0;
            }
            TIFFGetFieldDefaulted(tif, TIFFTAG_YCBCRSUBSAMPLING,
                                  ycbcrsubsampling + 0, ycbcrsubsampling + 1);
            if (((ycbcrsubsampling[0] != 1) && (ycbcrsubsampling[0] != 2) &&
                 (ycbcrsubsampling[0] != 4)) ||
                ((ycbcrsubsampling[1] != 1) && (ycbcrsubsampling[1] != 2) &&
                 (ycbcrsubsampling[1] != 4)))
            {
                TIFFErrorExtR(tif, module, "Invalid YCbCr subsampling");
                return 0;
            }
            samplingblock_samples =
                ycbcrsubsampling[0] * ycbcrsubsampling[1] + 2;
            samplingblocks_hor =
                TIFFhowmany_32(td->td_imagewidth, ycbcrsubsampling[0]);
            samplingrow_samples = _TIFFMultiply64(
                tif, samplingblocks_hor, samplingblock_samples, module);
            samplingrow_size =
                TIFFhowmany_64(_TIFFMultiply64(tif, samplingrow_samples,
                                               td->td_bitspersample, module),
                               8);
            scanline_size = (samplingrow_size / ycbcrsubsampling[1]);
        }
        else
        {
            uint64_t scanline_samples;
            scanline_samples = _TIFFMultiply64(tif, td->td_imagewidth,
                                               td->td_samplesperpixel, module);
            scanline_size =
                TIFFhowmany_64(_TIFFMultiply64(tif, scanline_samples,
                                               td->td_bitspersample, module),
                               8);
        }
    }
    else
    {
        scanline_size =
            TIFFhowmany_64(_TIFFMultiply64(tif, td->td_imagewidth,
                                           td->td_bitspersample, module),
                           8);
    }
    if (scanline_size == 0)
    {
        TIFFErrorExtR(tif, module, "Computed scanline size is zero");
        return 0;
    }
    return (scanline_size);
}
tmsize_t TIFFScanlineSize(TIFF *tif)
{
    static const char module[] = "TIFFScanlineSize";
    uint64_t m;
    m = TIFFScanlineSize64(tif);
    return _TIFFCastUInt64ToSSize(tif, m, module);
}

/*
 * Return the number of bytes required to store a complete
 * decoded and packed raster scanline (as opposed to the
 * I/O size returned by TIFFScanlineSize which may be less
 * if data is store as separate planes).
 */
uint64_t TIFFRasterScanlineSize64(TIFF *tif)
{
    static const char module[] = "TIFFRasterScanlineSize64";
    TIFFDirectory *td = &tif->tif_dir;
    uint64_t scanline;

    scanline =
        _TIFFMultiply64(tif, td->td_bitspersample, td->td_imagewidth, module);
    if (td->td_planarconfig == PLANARCONFIG_CONTIG)
    {
        scanline =
            _TIFFMultiply64(tif, scanline, td->td_samplesperpixel, module);
        return (TIFFhowmany8_64(scanline));
    }
    else
        return (_TIFFMultiply64(tif, TIFFhowmany8_64(scanline),
                                td->td_samplesperpixel, module));
}
tmsize_t TIFFRasterScanlineSize(TIFF *tif)
{
    static const char module[] = "TIFFRasterScanlineSize";
    uint64_t m;
    m = TIFFRasterScanlineSize64(tif);
    return _TIFFCastUInt64ToSSize(tif, m, module);
}

// ----------------------------------------------------------------------------
// EOF tif_strip.c - Released 2024-12-28T16:54:05Z
