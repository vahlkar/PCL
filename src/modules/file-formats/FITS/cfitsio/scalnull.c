//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.1
// ----------------------------------------------------------------------------
// scalnull.c - Released 2024-12-28T16:54:05Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

/*  This file, scalnull.c, contains the FITSIO routines used to define     */
/*  the starting heap address, the value scaling and the null values.      */

/*  The FITSIO software was written by William Pence at the High Energy    */
/*  Astrophysic Science Archive Research Center (HEASARC) at the NASA      */
/*  Goddard Space Flight Center.                                           */

#include <string.h>
#include "fitsio2.h"
/*--------------------------------------------------------------------------*/
int ffpthp(fitsfile *fptr,      /* I - FITS file pointer */
           long theap,          /* I - starting addrss for the heap */
           int *status)         /* IO - error status     */
/*
  Define the starting address for the heap for a binary table.
  The default address is NAXIS1 * NAXIS2.  It is in units of
  bytes relative to the beginning of the regular binary table data.
  This routine also writes the appropriate THEAP keyword to the
  FITS header.
*/
{
    if (*status > 0 || theap < 1)
        return(*status);

    /* reset position to the correct HDU if necessary */
    if (fptr->HDUposition != (fptr->Fptr)->curhdu)
        ffmahd(fptr, (fptr->HDUposition) + 1, NULL, status);

    (fptr->Fptr)->heapstart = theap;

    ffukyj(fptr, "THEAP", theap, "byte offset to heap area", status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpscl(fitsfile *fptr,      /* I - FITS file pointer               */
           double scale,        /* I - scaling factor: value of BSCALE */
           double zero,         /* I - zero point: value of BZERO      */
           int *status)         /* IO - error status                   */
/*
  Define the linear scaling factor for the primary array or image extension
  pixel values. This routine overrides the scaling values given by the
  BSCALE and BZERO keywords if present.  Note that this routine does not
  write or modify the BSCALE and BZERO keywords, but instead only modifies
  the values temporarily in the internal buffer.  Thus, a subsequent call to
  the ffrdef routine will reset the scaling back to the BSCALE and BZERO
  keyword values (or 1. and 0. respectively if the keywords are not present).
*/
{
    tcolumn *colptr;
    int hdutype;

    if (*status > 0)
        return(*status);

    if (scale == 0)
        return(*status = ZERO_SCALE);  /* zero scale value is illegal */

    if (ffghdt(fptr, &hdutype, status) > 0)  /* get HDU type */
        return(*status);

    if (hdutype != IMAGE_HDU)
        return(*status = NOT_IMAGE);         /* not proper HDU type */

    if (fits_is_compressed_image(fptr, status)) /* compressed images */
    {
        (fptr->Fptr)->cn_bscale = scale;
        (fptr->Fptr)->cn_bzero  = zero;
        return(*status);
    }

    /* set pointer to the first 'column' (contains group parameters if any) */
    colptr = (fptr->Fptr)->tableptr;

    colptr++;   /* increment to the 2nd 'column' pointer  (the image itself) */

    colptr->tscale = scale;
    colptr->tzero = zero;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpnul(fitsfile *fptr,      /* I - FITS file pointer                */
           LONGLONG nulvalue,   /* I - null pixel value: value of BLANK */
           int *status)         /* IO - error status                    */
/*
  Define the value used to represent undefined pixels in the primary array or
  image extension. This only applies to integer image pixel (i.e. BITPIX > 0).
  This routine overrides the null pixel value given by the BLANK keyword
  if present.  Note that this routine does not write or modify the BLANK
  keyword, but instead only modifies the value temporarily in the internal
  buffer. Thus, a subsequent call to the ffrdef routine will reset the null
  value back to the BLANK  keyword value (or not defined if the keyword is not
  present).
*/
{
    tcolumn *colptr;
    int hdutype;

    if (*status > 0)
        return(*status);

    if (ffghdt(fptr, &hdutype, status) > 0)  /* get HDU type */
        return(*status);

    if (hdutype != IMAGE_HDU)
        return(*status = NOT_IMAGE);         /* not proper HDU type */

    if (fits_is_compressed_image(fptr, status)) /* ignore compressed images */
        return(*status);

    /* set pointer to the first 'column' (contains group parameters if any) */
    colptr = (fptr->Fptr)->tableptr;

    colptr++;   /* increment to the 2nd 'column' pointer  (the image itself) */

    colptr->tnull = nulvalue;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fftscl(fitsfile *fptr,      /* I - FITS file pointer */
           int colnum,          /* I - column number to apply scaling to */
           double scale,        /* I - scaling factor: value of TSCALn   */
           double zero,         /* I - zero point: value of TZEROn       */
           int *status)         /* IO - error status     */
/*
  Define the linear scaling factor for the TABLE or BINTABLE extension
  column values. This routine overrides the scaling values given by the
  TSCALn and TZEROn keywords if present.  Note that this routine does not
  write or modify the TSCALn and TZEROn keywords, but instead only modifies
  the values temporarily in the internal buffer.  Thus, a subsequent call to
  the ffrdef routine will reset the scaling back to the TSCALn and TZEROn
  keyword values (or 1. and 0. respectively if the keywords are not present).
*/
{
    tcolumn *colptr;
    int hdutype;

    if (*status > 0)
        return(*status);

    if (scale == 0)
        return(*status = ZERO_SCALE);  /* zero scale value is illegal */

    if (ffghdt(fptr, &hdutype, status) > 0)  /* get HDU type */
        return(*status);

    if (hdutype == IMAGE_HDU)
        return(*status = NOT_TABLE);         /* not proper HDU type */

    colptr = (fptr->Fptr)->tableptr;   /* set pointer to the first column */
    colptr += (colnum - 1);     /* increment to the correct column */

    colptr->tscale = scale;
    colptr->tzero = zero;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fftnul(fitsfile *fptr,      /* I - FITS file pointer                  */
           int colnum,          /* I - column number to apply nulvalue to */
           LONGLONG nulvalue,   /* I - null pixel value: value of TNULLn  */
           int *status)         /* IO - error status                      */
/*
  Define the value used to represent undefined pixels in the BINTABLE column.
  This only applies to integer datatype columns (TFORM = B, I, or J).
  This routine overrides the null pixel value given by the TNULLn keyword
  if present.  Note that this routine does not write or modify the TNULLn
  keyword, but instead only modifies the value temporarily in the internal
  buffer. Thus, a subsequent call to the ffrdef routine will reset the null
  value back to the TNULLn  keyword value (or not defined if the keyword is not
  present).
*/
{
    tcolumn *colptr;
    int hdutype;

    if (*status > 0)
        return(*status);

    if (ffghdt(fptr, &hdutype, status) > 0)  /* get HDU type */
        return(*status);

    if (hdutype != BINARY_TBL)
        return(*status = NOT_BTABLE);        /* not proper HDU type */

    colptr = (fptr->Fptr)->tableptr;   /* set pointer to the first column */
    colptr += (colnum - 1);    /* increment to the correct column */

    colptr->tnull = nulvalue;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffsnul(fitsfile *fptr,      /* I - FITS file pointer                  */
           int colnum,          /* I - column number to apply nulvalue to */
           char *nulstring,     /* I - null pixel value: value of TNULLn  */
           int *status)         /* IO - error status                      */
/*
  Define the string used to represent undefined pixels in the ASCII TABLE
  column. This routine overrides the null  value given by the TNULLn keyword
  if present.  Note that this routine does not write or modify the TNULLn
  keyword, but instead only modifies the value temporarily in the internal
  buffer. Thus, a subsequent call to the ffrdef routine will reset the null
  value back to the TNULLn keyword value (or not defined if the keyword is not
  present).
*/
{
    tcolumn *colptr;
    int hdutype;

    if (*status > 0)
        return(*status);

    if (ffghdt(fptr, &hdutype, status) > 0)  /* get HDU type */
        return(*status);

    if (hdutype != ASCII_TBL)
        return(*status = NOT_ATABLE);        /* not proper HDU type */

    colptr = (fptr->Fptr)->tableptr;   /* set pointer to the first column */
    colptr += (colnum - 1);    /* increment to the correct column */

    colptr->strnull[0] = '\0';
    strncat(colptr->strnull, nulstring, 19);  /* limit string to 19 chars */

    return(*status);
}

// ----------------------------------------------------------------------------
// EOF scalnull.c - Released 2024-12-28T16:54:05Z
