//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/StringList.h - Released 2024-12-28T16:53:48Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_StringList_h
#define __PCL_StringList_h

/// \file pcl/StringList.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/KeyValue.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup string_lists String List Classes
 */

/*!
 * \class pcl::StringList
 * \ingroup string_lists
 * \brief Dynamic list of Unicode (UTF-16) strings.
 *
 * %StringList is a template instantiation of Array for the String class.
 */
using StringList = Array<String>;

/*!
 * \class pcl::SortedStringList
 * \ingroup string_lists
 * \brief Dynamic sorted list of Unicode (UTF-16) strings.
 *
 * %SortedStringList is a template instantiation of SortedArray for String.
 */
using SortedStringList = SortedArray<String>;

/*!
 * \class pcl::IsoStringList
 * \ingroup string_lists
 * \brief Dynamic list of 8-bit strings.
 *
 * %IsoStringList is a template instantiation of Array for IsoString.
 */
using IsoStringList = Array<IsoString>;

/*!
 * \class pcl::SortedIsoStringList
 * \ingroup string_lists
 * \brief Dynamic sorted list of 8-bit strings.
 *
 * %SortedIsoStringList is a template instantiation of SortedArray for the
 * IsoString class.
 */
using SortedIsoStringList = SortedArray<IsoString>;

/*!
 * \defgroup key_value_string_lists Key-Value String List Classes
 */

/*!
 * \class pcl::StringKeyValueList
 * \ingroup string_lists
 * \ingroup key_value_string_lists
 * \brief Dynamic list of key-value associations implemented as Unicode (UTF-16) strings.
 */
using StringKeyValueList = Array<StringKeyValue>;

/*!
 * \class pcl::SortedStringKeyValueList
 * \ingroup string_lists
 * \ingroup key_value_string_lists
 * \brief Dynamic sorted list of key-value associations implemented as Unicode (UTF-16) strings.
 */
using SortedStringKeyValueList = SortedArray<StringKeyValue>;

/*!
 * \class pcl::IsoStringKeyValueList
 * \ingroup string_lists
 * \ingroup key_value_string_lists
 * \brief Dynamic list of key-value associations implemented as 8-bit strings.
 */
using IsoStringKeyValueList = Array<IsoStringKeyValue>;

/*!
 * \class pcl::SortedIsoStringKeyValueList
 * \ingroup string_lists
 * \ingroup key_value_string_lists
 * \brief Dynamic sorted list of key-value associations implemented as 8-bit strings.
 */
using SortedIsoStringKeyValueList = SortedArray<IsoStringKeyValue>;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_StringList_h

// ----------------------------------------------------------------------------
// EOF pcl/StringList.h - Released 2024-12-28T16:53:48Z
