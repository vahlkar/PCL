//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/MultiVector.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_MultiVector_h
#define __PCL_MultiVector_h

/// \file pcl/MultiVector.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Array.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class GenericMultiVector
 * \brief Generic array of vectors.
 *
 * A multivector is a dynamic array of vectors. It can also be seen as a vector
 * of vectors, or a vector whose components are also vectors.
 * %GenericMultiVector implements a homogeneous multivector based on the Array
 * and GenericVector template classes. Besides all the member functions
 * inherited from its base class, it also provides some useful constructors and
 * assignment operators, including arithmetic scalar-to-vector operators that
 * work on all the contained vectors as a whole.
 *
 * Some typical applications of multivectors include:
 *
 * \li Implementation of more sophisticated mathematical objects and
 * structures, such as tensors.
 *
 * \li Matrices where rows (or equivalently, columns) have to be manipulated
 * as independent structures. For example, to swap two rows of a GenericMatrix
 * you have to swap each pair of row elements successively, which is an O(N)
 * operation. Swapping two component vectors of a multivector is an O(1)
 * operation thanks to the implicit data sharing feature of %GenericVector, and
 * hence does not require copying or duplicating data.
 *
 * \li Arrays and matrices where each component is a vector of variable length.
 *
 * As most PCL containers, %GenericMultiVector is a reference-counted class
 * with implicit data sharing (or \e copy-on-write) functionality.
 *
 * \sa Array, GenericVector, GenericMatrix
 */
template <typename T>
class PCL_CLASS GenericMultiVector : public Array<GenericVector<T> >
{
public:

   /*!
    * The structure implementing this multivector class.
    */
   using multivector_implementation = Array<GenericVector<T> >;

   /*!
    * Represents a vector.
    */
   using vector = GenericVector<T>;

   /*!
    * Represents a scalar.
    */
   using scalar = typename vector::scalar;

   /*!
    * Represents a vector component.
    */
   using component = typename vector::component;

   /*!
    * Represents a mutable multivector iterator.
    */
   using iterator = typename multivector_implementation::iterator;

   /*!
    * Represents an immutable multivector iterator.
    */
   using const_iterator = typename multivector_implementation::const_iterator;

   /*!
    * Represents a mutable vector iterator.
    */
   using vector_iterator = typename vector::iterator;

   /*!
    * Represents an immutable vector iterator.
    */
   using const_vector_iterator = typename vector::const_iterator;

   /*!
    * Constructs an empty multivector. An empty multivector has no component
    * vectors and zero length.
    */
   GenericMultiVector() = default;

   /*!
    * Constructs an uninitialized multivector of the specified \a length.
    *
    * This constructor does not initialize component vectors. The newly created
    * multivector will contain \a length empty vectors.
    */
   GenericMultiVector( size_type length )
      : multivector_implementation( length )
   {
   }

   /*!
    * Constructs a multivector with uninitialized component vectors.
    *
    * \param length        Number of multivector components.
    *
    * \param vectorLength  Number of vector components (>= 0).
    *
    * This constructor does not initialize vector components. The newly created
    * vectors will contain unpredictable values.
    */
   GenericMultiVector( size_type length, int vectorLength )
      : multivector_implementation( length )
   {
      for ( iterator i = this->Begin(), j = this->End(); i < j; ++i )
         *i = vector( vectorLength );
   }

   /*!
    * Constructs a multivector and fills it with a constant scalar.
    *
    * \param value         Initial value for all vector components.
    *
    * \param length        Number of multivector components.
    *
    * \param vectorLength  Number of vector components (>= 0).
    */
   GenericMultiVector( const scalar& value, size_type length, int vectorLength )
      : multivector_implementation( length )
   {
      for ( iterator i = this->Begin(), j = this->End(); i < j; ++i )
         *i = vector( value, vectorLength );
   }

   /*!
    * Constructs a multivector of two components initialized with copies of the
    * specified vectors \a v0 and \a v1, respectively for the first and second
    * component vectors.
    */
   GenericMultiVector( const vector& v0, const vector& v1 )
      : GenericMultiVector( size_type( 2 ) )
   {
      iterator i = this->Begin();
      *i = v0; *++i = v1;
   }

   /*!
    * Constructs a multivector of three components initialized with copies of
    * the specified vectors \a v0, \a v1 and \a v2, respectively for the first,
    * second and third component vectors.
    */
   GenericMultiVector( const vector& v0, const vector& v1, const vector& v2 )
      : GenericMultiVector( size_type( 3 ) )
   {
      iterator i = this->Begin();
      *i = v0; *++i = v1; *++i = v2;
   }

   /*!
    * Constructs a multivector of four components initialized with copies of
    * the specified vectors \a v0, \a v1, \a v2 and \a v3, respectively for the
    * first, second, third and fourth component vectors.
    */
   GenericMultiVector( const vector& v0, const vector& v1, const vector& v2, const vector& v3 )
      : GenericMultiVector( size_type( 4 ) )
   {
      iterator i = this->Begin();
      *i = v0; *++i = v1; *++i = v2; *++i = v3;
   }

   /*!
    * Copy constructor.
    */
   GenericMultiVector( const GenericMultiVector& ) = default;

   /*!
    * Move constructor.
    */
   GenericMultiVector( GenericMultiVector&& ) = default;

   /*!
    * Destroys a %GenericMultiVector object.
    */
   ~GenericMultiVector()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   GenericMultiVector& operator =( const GenericMultiVector& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   GenericMultiVector& operator =( GenericMultiVector&& ) = default;

   /*!
    * Assigns a constant scalar \a x to all vector components in this
    * multivector. Returns a reference to this object.
    */
   GenericMultiVector& operator =( const scalar& x )
   {
      for ( iterator i = this->Begin(), j = this->End(); i < j; ++i )
         *i = x;
      return *this;
   }

   /*!
    * Adds a constant scalar \a x to all vector components in this multivector.
    * Returns a reference to this object.
    */
   GenericMultiVector& operator +=( const scalar& x )
   {
      for ( iterator i = this->Begin(), j = this->End(); i < j; ++i )
         *i += x;
      return *this;
   }

   /*!
    * Subtracts a constant scalar \a x from all vector components of this
    * multivector. Returns a reference to this object.
    */
   GenericMultiVector& operator -=( const scalar& x )
   {
      for ( iterator i = this->Begin(), j = this->End(); i < j; ++i )
         *i -= x;
      return *this;
   }

   /*!
    * Multiplies all vector components in this multivector by a constant scalar
    * \a x. Returns a reference to this object.
    */
   GenericMultiVector& operator *=( const scalar& x )
   {
      for ( iterator i = this->Begin(), j = this->End(); i < j; ++i )
         *i *= x;
      return *this;
   }

   /*!
    * Divides all vector components in this multivector by a constant scalar
    * \a x. Returns a reference to this object.
    */
   GenericMultiVector& operator /=( const scalar& x )
   {
      for ( iterator i = this->Begin(), j = this->End(); i < j; ++i )
         *i /= x;
      return *this;
   }

   /*!
    * Returns the sum of all vector components.
    */
   double Sum() const
   {
      double s = 0;
      for ( const_iterator i = this->Begin(), j = this->End(); i < j; ++i )
         s += i->Sum();
      return s;
   }

   /*!
    * Computes the sum of vector components using a numerically stable
    * summation algorithm to minimize roundoff error.
    */
   double StableSum() const
   {
      DVector s( this->Length() );
      size_type n = 0;
      for ( const_iterator i = this->Begin(), j = this->End(); i < j; ++i )
         s[n++] = i->StableSum();
      return s.StableSum();
   }

#ifndef __PCL_NO_VECTOR_STATISTICS

   /*!
    * Returns the value of the smallest vector component. For empty
    * multivectors, this function returns zero.
    */
   component MinComponent() const
   {
      if ( !this->IsEmpty() )
      {
         component min = this->Begin()->MinComponent();
         for ( const_iterator i = this->Begin(), j = this->End(); ++i < j; )
            min = pcl::Min( min, i->MinComponent() );
         return min;
      }
      return component( 0 );
   }

   /*!
    * Returns the value of the largest vector component. For empty
    * multivectors, this function returns zero.
    */
   component MaxComponent() const
   {
      if ( !this->IsEmpty() )
      {
         component max = this->Begin()->MaxComponent();
         for ( const_iterator i = this->Begin(), j = this->End(); ++i < j; )
            max = pcl::Max( max, i->MaxComponent() );
         return max;
      }
      return component( 0 );
   }

#endif   // !__PCL_NO_VECTOR_STATISTICS
};

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_MULTIVECTOR_INSTANTIATE

/*!
 * \defgroup multivector_types Multivector Types
 */

/*!
 * \class pcl::DMultiVector
 * \ingroup multivector_types
 * \brief 64-bit floating point real multivector.
 *
 * %DMultiVector is a template instantiation of GenericMultiVector for the
 * \c double data type.
 */
using DMultiVector = GenericMultiVector<double>;

/*!
 * \class pcl::FMultiVector
 * \ingroup multivector_types
 * \brief 32-bit floating point real multivector.
 *
 * %FMultiVector is a template instantiation of GenericMultiVector for the
 * \c float data type.
 */
using FMultiVector = GenericMultiVector<float>;

/*!
 * \class pcl::IMultiVector
 * \ingroup multivector_types
 * \brief Integer multivector.
 *
 * %IMultiVector is a template instantiation of GenericMultiVector for the
 * \c int data type.
 */
using IMultiVector = GenericMultiVector<int>;

/*!
 * \class pcl::MultiVector
 * \ingroup multivector_types
 * \brief 64-bit floating point real multivector.
 *
 * %MultiVector is an alias for DMultiVector. It is a template instantiation of
 * GenericMultiVector for the \c double data type.
 */
using MultiVector = DMultiVector;

#endif   // !__PCL_NO_MULTIVECTOR_INSTANTIATE

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MultiVector_h

// ----------------------------------------------------------------------------
// EOF pcl/MultiVector.h - Released 2024-12-28T16:53:48Z
