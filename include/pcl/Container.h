//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Container.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_Container_h
#define __PCL_Container_h

/// \file pcl/Container.h

#include <pcl/Defs.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Container
 * \brief Root base class of all PCL containers.
 */
class PCL_CLASS Container
{
};

/*!
 * \class DirectContainer
 * \brief Root base class of all PCL containers of objects.
 */
template <typename T>
class PCL_CLASS DirectContainer : public Container
{
public:

   /*!
    * The type of contained objects.
    */
   using item_type = T;

   /*!
    * The type of contained immutable objects.
    */
   using const_item_type = const T;
};

/*!
 * \class IndirectContainer
 * \brief Root base class of all PCL containers of pointers to objects.
 */
template <typename T>
class PCL_CLASS IndirectContainer : public Container
{
public:

   /*!
    * The type of contained pointers.
    */
   using item_type = T*;

   /*!
    * The type of contained pointers to immutable objects.
    */
   using const_item_type = const T*;
};

/*!
 * \class DirectSortedContainer
 * \brief Root base class of all PCL sorted containers of objects.
 */
template <typename T>
class PCL_CLASS DirectSortedContainer: public DirectContainer<T>
{
public:

   /*!
    * The type of contained objects.
    */
   using item_type = typename DirectContainer<T>::item_type;

   /*!
    * The type of contained immutable objects.
    */
   using const_item_type = typename DirectContainer<T>::const_item_type;
};

/*!
 * \class IndirectSortedContainer
 * \brief Root base class of all PCL sorted containers of pointers to objects.
 */
template <typename T>
class PCL_CLASS IndirectSortedContainer : public IndirectContainer<T>
{
public:

   /*!
    * The type of contained pointers.
    */
   using item_type = typename IndirectContainer<T>::item_type;

   /*!
    * The type of contained pointers to immutable objects.
    */
   using const_item_type = typename IndirectContainer<T>::const_item_type;
};

#define PCL_ASSERT_DIRECT_CONTAINER( C, T )                                   \
   static_assert( std::is_base_of<DirectContainer<T>, C>::value,              \
                  "Argument type must derive from DirectContainer<T>." )

#define PCL_ASSERT_INDIRECT_CONTAINER( C, T )                                 \
   static_assert( std::is_base_of<IndirectContainer<T>, C>::value,            \
                  "Argument type must derive from IndirectContainer<T>." )

#define PCL_ASSERT_CONTAINER( C, T )                                          \
   static_assert( std::is_base_of<DirectContainer<T>, C>::value               \
               || std::is_base_of<IndirectContainer<T>, C>::value,            \
                  "Argument type must derive from DirectContainer<T> or IndirectContainer<T>." )

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Container_h

// ----------------------------------------------------------------------------
// EOF pcl/Container.h - Released 2024-12-28T16:53:48Z
