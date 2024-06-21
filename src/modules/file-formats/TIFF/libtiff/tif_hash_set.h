//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 1.0.10
// ----------------------------------------------------------------------------
// tif_hash_set.h - Released 2024-06-18T15:49:17Z
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

/**********************************************************************
 * $Id$
 *
 * Name:     tif_hash_set.h
 * Project:  TIFF - Common Portability Library
 * Purpose:  Hash set functions.
 * Author:   Even Rouault, <even dot rouault at spatialys.com>
 *
 **********************************************************************
 * Copyright (c) 2008-2009, Even Rouault <even dot rouault at spatialys.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef TIFF_HASH_SET_H_INCLUDED
#define TIFF_HASH_SET_H_INCLUDED

#include <stdbool.h>

/**
 * \file tif_hash_set.h
 *
 * Hash set implementation.
 *
 * An hash set is a data structure that holds elements that are unique
 * according to a comparison function. Operations on the hash set, such as
 * insertion, removal or lookup, are supposed to be fast if an efficient
 * "hash" function is provided.
 */

#ifdef __cplusplus
extern "C"
{
#endif

    /* Types */

    /** Opaque type for a hash set */
    typedef struct _TIFFHashSet TIFFHashSet;

    /** TIFFHashSetHashFunc */
    typedef unsigned long (*TIFFHashSetHashFunc)(const void *elt);

    /** TIFFHashSetEqualFunc */
    typedef bool (*TIFFHashSetEqualFunc)(const void *elt1, const void *elt2);

    /** TIFFHashSetFreeEltFunc */
    typedef void (*TIFFHashSetFreeEltFunc)(void *elt);

    /* Functions */

    TIFFHashSet *TIFFHashSetNew(TIFFHashSetHashFunc fnHashFunc,
                                TIFFHashSetEqualFunc fnEqualFunc,
                                TIFFHashSetFreeEltFunc fnFreeEltFunc);

    void TIFFHashSetDestroy(TIFFHashSet *set);

    int TIFFHashSetSize(const TIFFHashSet *set);

#ifdef notused
    void TIFFHashSetClear(TIFFHashSet *set);

    /** TIFFHashSetIterEltFunc */
    typedef int (*TIFFHashSetIterEltFunc)(void *elt, void *user_data);

    void TIFFHashSetForeach(TIFFHashSet *set, TIFFHashSetIterEltFunc fnIterFunc,
                            void *user_data);
#endif

    bool TIFFHashSetInsert(TIFFHashSet *set, void *elt);

    void *TIFFHashSetLookup(TIFFHashSet *set, const void *elt);

    bool TIFFHashSetRemove(TIFFHashSet *set, const void *elt);

#ifdef notused
    bool TIFFHashSetRemoveDeferRehash(TIFFHashSet *set, const void *elt);
#endif

#ifdef __cplusplus
}
#endif

#endif /* TIFF_HASH_SET_H_INCLUDED */

// ----------------------------------------------------------------------------
// EOF tif_hash_set.h - Released 2024-06-18T15:49:17Z
