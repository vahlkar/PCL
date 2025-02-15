//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Matrix.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_Matrix_h
#define __PCL_Matrix_h

/// \file pcl/Matrix.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Container.h>
#include <pcl/Exception.h>
#include <pcl/Math.h>
#include <pcl/Memory.h>
#include <pcl/ReferenceCounter.h>
#include <pcl/Rotate.h> // pcl::Reverse()
#include <pcl/Utility.h>
#include <pcl/Vector.h>

#ifndef __PCL_NO_MATRIX_STATISTICS
#  include <pcl/Selection.h>
#  include <pcl/Sort.h>
#endif

#if !defined( __PCL_NO_MATRIX_IMAGE_RENDERING ) && !defined( __PCL_NO_MATRIX_IMAGE_CONVERSION )
#  include <pcl/Image.h>
#  include <pcl/ImageVariant.h>
#endif

#if !defined( __PCL_NO_MATRIX_PHASE_MATRICES ) && !defined( __PCL_NO_VECTOR_INSTANTIATE )
#  include <pcl/Complex.h>
#endif

/*
 * Valid filter kernel sizes are odd integers >= 3. This macro is used for the
 * KernelFilter and SeparableFilter classes to ensure validity of filter
 * matrices and vectors.
 */
#define PCL_VALID_KERNEL_SIZE( n )  (n ? Max( 3, n|1 ) : 0)

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class GenericMatrix
 * \brief Generic dynamic matrix of arbitrary dimensions.
 *
 * %GenericMatrix is a lightweight template class implementing a matrix of
 * arbitrary size. This class provides the following main features:
 *
 * \li Implicit data sharing with reference counting and copy-on-write
 * functionality. %GenericMatrix instances can safely be passed as function
 * return values and by-value function arguments.
 *
 * \li Thread-safe. %GenericMatrix instances can safely be accessed from
 * multiple threads. The reference counter implements atomic reference and
 * dereference operations.
 *
 * \li Efficient matrix storage and access to matrix elements. %Matrix
 * elements are dynamically allocated as a single, contiguous memory block.
 *
 * \li Support for a comprehensive set of matrix operations, including matrix
 * inversion and transposition, scalar-to-matrix and matrix-to-matrix
 * arithmetic operations, and image to/from matrix conversions.
 *
 * \li Calculation of a variety of descriptive statistics for matrix elements.
 *
 * \sa GenericVector, \ref matrix_operators, \ref matrix_types
 */
template <typename T>
class PCL_CLASS GenericMatrix : public DirectContainer<T>
{
public:

   /*! #
    */
   using container_type = DirectContainer<T>;

   /*! #
    */
   using item_type = typename container_type::item_type;

   /*! #
    */
   using const_item_type = typename container_type::const_item_type;

   /*!
    * Represents a matrix element.
    */
   using element = T;

   /*!
    * Represents a vector of matrix elements.
    */
   using vector = GenericVector<element>;

   /*!
    * Represents a mutable matrix block iterator. In a matrix of n rows and m
    * columns, a block iterator allows iteration on the entire set of matrix
    * elements from {0,0} to {n-1,m-1} in row-column order.
    */
   using block_iterator = element*;

   /*!
    * Represents an immutable matrix block iterator. In a matrix of n rows and
    * m columns, a block iterator allows iteration on the entire set of matrix
    * elements from {0,0} to {n-1,m-1} in row-column order.
    */
   using const_block_iterator = const element*;

   /*!
    * Constructs an empty matrix.
    * An empty matrix has no elements and zero dimensions.
    */
   GenericMatrix()
   {
      m_data = new Data;
   }

   /*!
    * Constructs an uninitialized matrix.
    *
    * \param rows    Number of matrix rows (&ge; 0).
    * \param cols    Number of matrix columns (&ge; 0).
    *
    * Matrix elements are not initialized by this constructor; the newly
    * created matrix will contain unpredictable values.
    */
   GenericMatrix( int rows, int cols )
   {
      PCL_PRECONDITION( rows >= 0 && cols >= 0 )
      m_data = new Data( rows, cols );
   }

   /*!
    * Constructs a matrix and fills it with a constant value.
    *
    * \param x       Initial value for all matrix elements.
    * \param rows    Number of matrix rows (&ge; 0).
    * \param cols    Number of matrix columns (&ge; 0).
    */
   GenericMatrix( const element& x, int rows, int cols )
   {
      PCL_PRECONDITION( rows >= 0 && cols >= 0 )
      m_data = new Data( rows, cols );
      pcl::Fill( m_data->Begin(), m_data->End(), x );
   }

   /*!
    * Constructs a matrix and initializes it with values from a static buffer.
    *
    * \param a       Address of the first item of a static array for
    *                initialization of matrix elements. The array must provide
    *                at least \a rows x \a cols consecutive elements stored in
    *                row order (all elements of the first matrix row followed
    *                by all elements of the second row, and so on).
    *
    * \param rows    Number of matrix rows (&ge; 0).
    *
    * \param cols    Number of matrix columns (&ge; 0).
    */
   template <typename T1>
   GenericMatrix( const T1* a, int rows, int cols )
   {
      PCL_PRECONDITION( rows >= 0 && cols >= 0 )
      m_data = new Data( rows, cols );
      if ( a != nullptr )
      {
               block_iterator __restrict__ i = m_data->Begin();
         const_block_iterator __restrict__ j = m_data->End();
         const T1*            __restrict__ k = a;
         PCL_IVDEP
         for ( ; i < j; ++i, ++k )
            *i = element( *k );
      }
   }

   /*!
    * Constructs a 2x2 matrix initialized with the specified element values.
    *
    * The resulting square matrix will be initialized as follows:
    *
    * <pre>
    * a00, a01
    * a10, a11
    * </pre>
    */
   template <typename T1>
   GenericMatrix( const T1& a00, const T1& a01,
                  const T1& a10, const T1& a11 )
   {
      m_data = new Data( 2, 2 );
      block_iterator __restrict__ v = m_data->Begin();
      v[0] = element( a00 ); v[1] = element( a01 );
      v[2] = element( a10 ); v[3] = element( a11 );
   }

   /*!
    * Constructs a 3x3 matrix initialized with the specified element values.
    *
    * The resulting square matrix will be initialized as follows:
    *
    * <pre>
    * a00, a01, a02
    * a10, a11, a12
    * a20, a21, a22
    * </pre>
    */
   template <typename T1>
   GenericMatrix( const T1& a00, const T1& a01, const T1& a02,
                  const T1& a10, const T1& a11, const T1& a12,
                  const T1& a20, const T1& a21, const T1& a22 )
   {
      m_data = new Data( 3, 3 );
      block_iterator __restrict__ v = m_data->Begin();
      v[0] = element( a00 ); v[1] = element( a01 ); v[2] = element( a02 );
      v[3] = element( a10 ); v[4] = element( a11 ); v[5] = element( a12 );
      v[6] = element( a20 ); v[7] = element( a21 ); v[8] = element( a22 );
   }

   /*!
    * Copy constructor. This object will reference the same data that is being
    * referenced by the specified matrix \a x.
    */
   GenericMatrix( const GenericMatrix& x )
      : m_data( x.m_data )
   {
      m_data->Attach();
   }

   /*!
    * Move constructor.
    */
   GenericMatrix( GenericMatrix&& x )
      : m_data( x.m_data )
   {
      x.m_data = nullptr;
   }

   /*!
    * Constructs a matrix as a copy of a rectangular subset of another matrix.
    *
    * \param x       Source matrix from which this object will acquire element
    *                values.
    *
    * \param i0      Starting row in \a x. Corresponds to the first (top) row
    *                of this matrix.
    *
    * \param j0      Starting column in \a x. Corresponds to the first
    *                (leftmost) column of this matrix.
    *
    * \param rows    Number of matrix rows.
    *
    * \param cols    Number of matrix columns.
    *
    * If the specified submatrix is invalid, i.e. if the specified row or
    * column don't exist, or if the specified number of rows and columns exceed
    * the dimensions of the source matrix \a x, this constructor constrains the
    * submatrix coordinates and dimensions to the nearest valid values,
    * possibly constructing an empty matrix.
    */
   GenericMatrix( const GenericMatrix& x, int i0, int j0, int rows, int cols )
   {
      i0 = Range( i0, 0, Max( 0, x.Rows()-1 ) );
      j0 = Range( j0, 0, Max( 0, x.Cols()-1 ) );
      rows = Range( rows, 0, Max( 0, x.Rows()-i0 ) );
      cols = Range( cols, 0, Max( 0, x.Cols()-j0 ) );
      m_data = new Data( rows, cols );
      for ( int i = 0; i < m_data->Rows(); ++i, ++i0 )
         for ( int j = 0; j < m_data->Cols(); ++j, ++j0 )
            m_data->v[i][j] = x.m_data->v[i0][j0];
   }

#ifndef __PCL_NO_MATRIX_IMAGE_CONVERSION

   /*!
    * Constructs a matrix from image pixel samples.
    *
    * This constructor converts pixel sample values to matrix elements for a
    * rectangular region of a channel of the specified \a image.
    *
    * \param image   %Image from which matrix elements will be generated.
    *
    * \param rect    A rectangular region in image pixel coordinates, from
    *                which this matrix will be generated. If this parameter is
    *                not specified, or if an empty rectangle is specified, this
    *                function will perform the matrix conversion for the
    *                current rectangular selection in the image, that is, for
    *                image.SelectedRectangle().
    *
    * \param channel Channel index. Must be the zero-based index of an existing
    *                channel in the image, or an integer < 0. If this parameter
    *                is not specified, or if a negative integer is specified,
    *                this function will perform the matrix conversion for the
    *                currently selected channel in the image, that is, for
    *                image.SelectedChannel().
    *
    * The newly created matrix is a representation of the intersection of the
    * specified (or implicitly selected) rectangular region with the image for
    * the scalar data type T of this matrix instantiation. All predefined pixel
    * sample types are supported, including integer, real and complex pixels.
    *
    * If there is no intersection between the rectangular region and the image,
    * this constructor yields an empty matrix.
    *
    * Note that if the source image is of a floating point type (either real or
    * complex) and the scalar type T of this matrix instantiation is of an
    * integer type, out-of-range matrix elements will be truncated to either
    * zero or to the maximum integer value, if the corresponding source pixel
    * sample is outside the normalized [0,1] range.
    */
   template <class P>
   GenericMatrix( const GenericImage<P>& image, const Rect& rect = Rect( 0 ), int channel = -1 )
   {
      GenericMatrix M = FromImage( image, rect, channel );
      pcl::Swap( m_data, M.m_data );
   }

   /*!
    * Constructs a matrix from image pixel samples.
    *
    * This constructor converts pixel sample values to matrix elements for a
    * rectangular region of a channel of the image transported by the specified
    * %ImageVariant.
    *
    * If the specified %ImageVariant object does not transport an image, this
    * constructor yields an empty matrix.
    *
    * Refer to the documentation for
    * GenericMatrix::GenericMatrix( const GenericImage&, const Rect&, int ) for
    * more information about the conversion performed by this constructor.
    */
   GenericMatrix( const ImageVariant& image, const Rect& rect = Rect( 0 ), int channel = -1 )
   {
      GenericMatrix M = FromImage( image, rect, channel );
      pcl::Swap( m_data, M.m_data );
   }

#endif   // !__PCL_NO_MATRIX_IMAGE_CONVERSION

   /*!
    * Virtual destructor. Dereferences the matrix data. If the matrix data
    * becomes unreferenced, it is destroyed and deallocated immediately.
    */
   virtual ~GenericMatrix()
   {
      if ( m_data != nullptr )
      {
         DetachFromData();
         m_data = nullptr;
      }
   }

   /*!
    * Deallocates matrix data, yielding an empty matrix.
    */
   void Clear()
   {
      if ( !IsEmpty() )
         if ( m_data->IsUnique() )
            m_data->Deallocate();
         else
         {
            Data* newData = new Data( 0, 0 );
            DetachFromData();
            m_data = newData;
         }
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    *
    * If this instance and the specified source instance \a x reference
    * different matrix data, the data previously referenced by this object is
    * dereferenced. If the previous data becomes unreferenced, it is destroyed
    * and deallocated. Then the data being referenced by \a x is also
    * referenced by this object.
    *
    * If this instance and the specified source instance \a x already reference
    * the same matrix data, then this function does nothing.
    */
   GenericMatrix& operator =( const GenericMatrix& x )
   {
      Assign( x );
      return *this;
   }

   /*!
    * Assigns a matrix \a x to this object.
    *
    * If this instance and the specified source instance \a x reference
    * different matrix data, the data previously referenced by this object is
    * dereferenced. If the previous data becomes unreferenced, it is destroyed
    * and deallocated. Then the data being referenced by \a x is also
    * referenced by this object.
    *
    * If this instance and the specified source instance \a x already reference
    * the same matrix data, then this function does nothing.
    */
   void Assign( const GenericMatrix& x )
   {
      x.m_data->Attach();
      DetachFromData();
      m_data = x.m_data;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   GenericMatrix& operator =( GenericMatrix&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Transfers data from another matrix \a x to this object.
    *
    * Decrements the reference counter of the current matrix data. If the data
    * becomes unreferenced, it is destroyed and deallocated. The matrix data
    * referenced by the source object \a x is then transferred to this object,
    * and the source object \a x is left empty.
    *
    * \warning The source matrix \a x will be an invalid object after calling
    * this function, and hence should be destroyed immediately. Any attempt to
    * access an invalid object will most likely lead to a crash.
    */
   void Transfer( GenericMatrix& x )
   {
      if ( &x != this )
      {
         DetachFromData();
         m_data = x.m_data;
         x.m_data = nullptr;
      }
   }

   /*!
    * Transfers data from another matrix \a x to this object.
    *
    * Decrements the reference counter of the current matrix data. If the data
    * becomes unreferenced, it is destroyed and deallocated. The matrix data
    * referenced by the source object \a x is then transferred to this object,
    * and the source object \a x is left empty.
    *
    * \warning The source matrix \a x will be an invalid object after calling
    * this function, and hence should be destroyed immediately. Any attempt to
    * access an invalid object will most likely lead to a crash.
    */
   void Transfer( GenericMatrix&& x )
   {
      if ( &x != this )
      {
         DetachFromData();
         m_data = x.m_data;
         x.m_data = nullptr;
      }
   }

   /*!
    * Exchanges two matrices \a x1 and \a x2.
    *
    * This function is efficient because it simply swaps the internal matrix
    * data pointers owned by the objects.
    */
   friend void Swap( GenericMatrix& x1, GenericMatrix& x2 ) noexcept
   {
      pcl::Swap( x1.m_data, x2.m_data );
   }

   /*!
    * Assigns a constant scalar \a x to all elements of this matrix. Returns a
    * reference to this object.
    *
    * Before assigning a constant value to all matrix elements, this function
    * ensures that this instance uniquely references its matrix data,
    * generating a new matrix data block if necessary.
    */
   GenericMatrix& operator =( const element& x )
   {
      if ( !IsUnique() )
      {
         Data* newData = new Data( m_data->Rows(), m_data->Cols() );
         DetachFromData();
         m_data = newData;
      }
      pcl::Fill( m_data->Begin(), m_data->End(), x );
      return *this;
   }

#define IMPLEMENT_SCALAR_ASSIGN_OP( op )                                      \
      if ( IsUnique() )                                                       \
      {                                                                       \
               block_iterator __restrict__ i = m_data->Begin();               \
         const_block_iterator __restrict__ j = m_data->End();                 \
         PCL_IVDEP                                                            \
         for ( ; i < j; ++i )                                                 \
            *i op##= x;                                                       \
      }                                                                       \
      else                                                                    \
      {                                                                       \
         Data* newData = new Data( m_data->Rows(), m_data->Cols() );          \
               block_iterator __restrict__ i = newData->Begin();              \
         const_block_iterator __restrict__ j = newData->End();                \
         const_block_iterator __restrict__ k = m_data->Begin();               \
         PCL_IVDEP                                                            \
         for ( ; i < j; ++i, ++k )                                            \
            *i = *k op x;                                                     \
         DetachFromData();                                                    \
         m_data = newData;                                                    \
      }                                                                       \
      return *this;

   /*!
    * Adds a constant scalar \a x to all elements of this matrix. Returns a
    * reference to this object.
    *
    * Before adding a constant value to all matrix elements, this function
    * ensures that this instance uniquely references its matrix data,
    * generating a duplicate if necessary.
    */
   GenericMatrix& operator +=( const element& x )
   {
      IMPLEMENT_SCALAR_ASSIGN_OP( + )
   }

   /*!
    * Subtracts a constant scalar \a x from all elements of this matrix.
    * Returns a reference to this object.
    *
    * Before subtracting a constant value from all matrix elements, this
    * function ensures that this instance uniquely references its matrix data,
    * generating a duplicate if necessary.
    */
   GenericMatrix& operator -=( const element& x )
   {
      IMPLEMENT_SCALAR_ASSIGN_OP( - )
   }

   /*!
    * Multiplies all elements of this matrix by a constant scalar \a x. Returns
    * a reference to this object.
    *
    * Before multiplying all matrix elements by a constant value, this function
    * ensures that this instance uniquely references its matrix data,
    * generating a duplicate if necessary.
    */
   GenericMatrix& operator *=( const element& x )
   {
      IMPLEMENT_SCALAR_ASSIGN_OP( * )
   }

   /*!
    * Divides all elements of this matrix by a constant scalar \a x. Returns a
    * reference to this object.
    *
    * Before dividing all matrix elements by a constant value, this function
    * ensures that this instance uniquely references its matrix data,
    * generating a duplicate if necessary.
    */
   GenericMatrix& operator /=( const element& x )
   {
      IMPLEMENT_SCALAR_ASSIGN_OP( / )
   }

   /*!
    * Raises all elements of this matrix to a constant scalar \a x. Returns a
    * reference to this object.
    *
    * Before raising all matrix elements to a constant value, this function
    * ensures that this instance uniquely references its matrix data,
    * generating a duplicate if necessary.
    */
   GenericMatrix& operator ^=( const element& x )
   {
      if ( IsUnique() )
      {
               block_iterator __restrict__ i = m_data->Begin();
         const_block_iterator __restrict__ j = m_data->End();
         PCL_IVDEP
         for ( ; i < j; ++i )
            *i = pcl::Pow( *i, x );
      }
      else
      {
         Data* newData = new Data( m_data->Rows(), m_data->Cols() );
               block_iterator __restrict__ i = newData->Begin();
         const_block_iterator __restrict__ j = newData->End();
         const_block_iterator __restrict__ k = m_data->Begin();
         PCL_IVDEP
         for ( ; i < j; ++i, ++k )
            *i = pcl::Pow( *k, x );
         DetachFromData();
         m_data = newData;
      }
      return *this;
   }

#undef IMPLEMENT_SCALAR_ASSIGN_OP

#ifndef __PCL_NO_MATRIX_ELEMENT_WISE_ARITHMETIC_OPERATIONS

#define IMPLEMENT_ELEMENT_WISE_ASSIGN_OP( op )                                                        \
      if ( IsUnique() )                                                                               \
      {                                                                                               \
               block_iterator __restrict__ i = m_data->Begin();                                       \
         const_block_iterator __restrict__ j = pcl::Min( m_data->End(),                               \
                                                         m_data->Begin() + x.NumberOfElements() );    \
         const_block_iterator __restrict__ k = x.m_data->Begin();                                     \
         PCL_IVDEP                                                                                    \
         for ( ; i < j; ++i, ++k )                                                                    \
            *i op##= *k;                                                                              \
      }                                                                                               \
      else                                                                                            \
      {                                                                                               \
         Data* newData = new Data( m_data->Rows(), m_data->Cols() );                                  \
               block_iterator __restrict__ i = newData->Begin();                                      \
         const_block_iterator __restrict__ j = pcl::Min( newData->End(),                              \
                                                         newData->Begin() + x.NumberOfElements() );   \
         const_block_iterator __restrict__ k = x.m_data->Begin();                                     \
         const_block_iterator __restrict__ t = m_data->Begin();                                       \
         PCL_IVDEP                                                                                    \
         for ( ; i < j; ++i, ++k, ++t )                                                               \
            *i = *t op *k;                                                                            \
         DetachFromData();                                                                            \
         m_data = newData;                                                                            \
      }                                                                                               \
      return *this;

   /*!
    * Performs the element-wise addition of a matrix \a x to this matrix.
    * Returns a reference to this object.
    *
    * The specified operand matrix \a x should have at least the same number of
    * elements as this matrix. Otherwise the operation will be aborted upon
    * reaching the end of the operand matrix, but no exception will be thrown.
    *
    * This function ensures that this instance uniquely references its matrix
    * data before the element-wise addition operation, generating a duplicate
    * if necessary.
    */
   GenericMatrix& operator +=( const GenericMatrix& x )
   {
      IMPLEMENT_ELEMENT_WISE_ASSIGN_OP( + )
   }

   /*!
    * Performs the element-wise subtraction of a matrix \a x to this matrix.
    * Returns a reference to this object.
    *
    * The specified operand matrix \a x should have at least the same number of
    * elements as this matrix. Otherwise the operation will be aborted upon
    * reaching the end of the operand matrix, but no exception will be thrown.
    *
    * This function ensures that this instance uniquely references its matrix
    * data before the element-wise subtraction operation, generating a
    * duplicate if necessary.
    */
   GenericMatrix& operator -=( const GenericMatrix& x )
   {
      IMPLEMENT_ELEMENT_WISE_ASSIGN_OP( - )
   }

   /*!
    * Performs the element-wise multiplication of a matrix \a x with this
    * matrix. Returns a reference to this object.
    *
    * The specified operand matrix \a x should have at least the same number of
    * elements as this matrix. Otherwise the operation will be aborted upon
    * reaching the end of the operand matrix, but no exception will be thrown.
    *
    * This function ensures that this instance uniquely references its matrix
    * data before the element-wise multiplication operation, generating a
    * duplicate if necessary.
    */
   GenericMatrix& operator *=( const GenericMatrix& x )
   {
      IMPLEMENT_ELEMENT_WISE_ASSIGN_OP( * )
   }

   /*!
    * Performs the element-wise division of this matrix by a matrix \a x.
    * Returns a reference to this object.
    *
    * The specified operand matrix \a x should have at least the same number of
    * elements as this matrix. Otherwise the operation will be aborted upon
    * reaching the end of the operand matrix, but no exception will be thrown.
    *
    * This function ensures that this instance uniquely references its matrix
    * data before the element-wise division operation, generating a duplicate
    * if necessary.
    */
   GenericMatrix& operator /=( const GenericMatrix& x )
   {
      IMPLEMENT_ELEMENT_WISE_ASSIGN_OP( / )
   }

   /*!
    * Performs the element-wise exponentiation of this matrix by a matrix \a x.
    * Returns a reference to this object.
    *
    * The specified matrix \a x must have at least the same number of elements
    * as this matrix. Otherwise an Error exception will be thrown.
    *
    * This function ensures that this instance uniquely references its matrix
    * data before the element-wise raise operation, generating a duplicate if
    * necessary.
    */
   GenericMatrix& operator ^=( const GenericMatrix& x )
   {
      if ( IsUnique() )
      {
               block_iterator __restrict__ i = m_data->Begin();
         const_block_iterator __restrict__ j = pcl::Min( m_data->End(),
                                                         m_data->Begin() + x.NumberOfElements() );
         const_block_iterator __restrict__ k = x.m_data->Begin();
         PCL_IVDEP
         for ( ; i < j; ++i, ++k )
            *i = pcl::Pow( *i, *k );
      }
      else
      {
         Data* newData = new Data( m_data->Rows(), m_data->Cols() );
               block_iterator __restrict__ i = newData->Begin();
         const_block_iterator __restrict__ j = pcl::Min( newData->End(),
                                                         newData->Begin() + x.NumberOfElements() );
         const_block_iterator __restrict__ k = x.m_data->Begin();
         const_block_iterator __restrict__ t = m_data->Begin();
         PCL_IVDEP
         for ( ; i < j; ++i, ++k, ++t )
            *i = pcl::Pow( *t, *k );
         DetachFromData();
         m_data = newData;
      }
      return *this;
   }

#undef IMPLEMENT_ELEMENT_WISE_ASSIGN_OP

#endif   // __PCL_NO_MATRIX_ELEMENT_WISE_ARITHMETIC_OPERATIONS

   /*!
    * Returns the square of this matrix. The result is a new matrix of the same
    * dimensions where each element is the square of its counterpart in this
    * matrix.
    */
   GenericMatrix Sqr() const
   {
      GenericMatrix R( m_data->Rows(), m_data->Cols() );
            block_iterator __restrict__ i = R.m_data->Begin();
      const_block_iterator __restrict__ j = R.m_data->End();
      const_block_iterator __restrict__ k = m_data->Begin();
      PCL_IVDEP
      for ( ; i < j; ++i, ++k )
         *i = *k * *k;
      return R;
   }

   /*!
    * Replaces all elements of this matrix with their squares.
    *
    * Before performing its task, this function ensures that this instance
    * uniquely references its matrix data, generating a duplicate if necessary.
    */
   void SetSqr()
   {
      if ( IsUnique() )
      {
               block_iterator __restrict__ i = m_data->Begin();
         const_block_iterator __restrict__ j = m_data->End();
         PCL_IVDEP
         for ( ; i < j; ++i )
            *i *= *i;
      }
      else
      {
         Data* newData = new Data( m_data->Rows(), m_data->Cols() );
               block_iterator __restrict__ i = newData->Begin();
         const_block_iterator __restrict__ j = newData->End();
         const_block_iterator __restrict__ k = m_data->Begin();
         PCL_IVDEP
         for ( ; i < j; ++i, ++k )
            *i = *k * *k;
         DetachFromData();
         m_data = newData;
      }
   }

   /*!
    * Returns the square root of this matrix. The result is a new matrix of the
    * same dimensions where each element is the square root of its counterpart
    * in this matrix.
    */
   GenericMatrix Sqrt() const
   {
      GenericMatrix R( m_data->Rows(), m_data->Cols() );
            block_iterator __restrict__ i = R.m_data->Begin();
      const_block_iterator __restrict__ j = R.m_data->End();
      const_block_iterator __restrict__ k = m_data->Begin();
      PCL_IVDEP
      for ( ; i < j; ++i, ++k )
         *i = pcl::Sqrt( *k );
      return R;
   }

   /*!
    * Replaces all elements of this matrix with their square roots.
    *
    * Before performing its task, this function ensures that this instance
    * uniquely references its matrix data, generating a duplicate if necessary.
    */
   void SetSqrt()
   {
      if ( IsUnique() )
      {
               block_iterator __restrict__ i = m_data->Begin();
         const_block_iterator __restrict__ j = m_data->End();
         PCL_IVDEP
         for ( ; i < j; ++i )
            *i = pcl::Sqrt( *i );
      }
      else
      {
         Data* newData = new Data( m_data->Rows(), m_data->Cols() );
               block_iterator __restrict__ i = newData->Begin();
         const_block_iterator __restrict__ j = newData->End();
         const_block_iterator __restrict__ k = m_data->Begin();
         PCL_IVDEP
         for ( ; i < j; ++i, ++k )
            *i = pcl::Sqrt( *k );
         DetachFromData();
         m_data = newData;
      }
   }

   /*!
    * Returns the absolute value of this matrix. The result is a new matrix of
    * the same dimensions where each element is the absolute value of its
    * counterpart in this matrix.
    */
   GenericMatrix Abs() const
   {
      GenericMatrix R( m_data->Rows(), m_data->Cols() );
            block_iterator __restrict__ i = R.m_data->Begin();
      const_block_iterator __restrict__ j = R.m_data->End();
      const_block_iterator __restrict__ k = m_data->Begin();
      PCL_IVDEP
      for ( ; i < j; ++i, ++k )
         *i = pcl::Abs( *k );
      return R;
   }

   /*!
    * Replaces all elements of this matrix with their absolute values.
    *
    * Before performing its task, this function ensures that this instance
    * uniquely references its matrix data, generating a duplicate if necessary.
    */
   void SetAbs()
   {
      if ( IsUnique() )
      {
               block_iterator __restrict__ i = m_data->Begin();
         const_block_iterator __restrict__ j = m_data->End();
         PCL_IVDEP
         for ( ; i < j; ++i )
            *i = pcl::Abs( *i );
      }
      else
      {
         Data* newData = new Data( m_data->Rows(), m_data->Cols() );
               block_iterator __restrict__ i = newData->Begin();
         const_block_iterator __restrict__ j = newData->End();
         const_block_iterator __restrict__ k = m_data->Begin();
         PCL_IVDEP
         for ( ; i < j; ++i, ++k )
            *i = pcl::Abs( *k );
         DetachFromData();
         m_data = newData;
      }
   }

   /*!
    * Returns true iff this instance uniquely references its matrix data.
    */
   bool IsUnique() const noexcept
   {
      return m_data->IsUnique();
   }

   /*!
    * Returns true iff this instance references (shares) the same matrix data as
    * another instance \a x.
    */
   bool IsAliasOf( const GenericMatrix& x ) const noexcept
   {
      return m_data == x.m_data;
   }

   /*!
    * Ensures that this instance uniquely references its matrix data.
    *
    * If necessary, this member function generates a duplicate of the matrix
    * data, references it, and then decrements the reference counter of the
    * original matrix data.
    */
   void EnsureUnique()
   {
      if ( !IsUnique() )
      {
         Data* newData = new Data( m_data->Rows(), m_data->Cols() );
         const_block_iterator __restrict__ i = m_data->Begin();
         const_block_iterator __restrict__ j = m_data->End();
               block_iterator __restrict__ k = newData->Begin();
         PCL_IVDEP
         for ( ; i < j; ++i, ++k )
            *k = *i;
         DetachFromData();
         m_data = newData;
      }
   }

   /*!
    * Returns the number of rows in this matrix. If this object is an empty
    * matrix, this member function returns zero.
    */
   int Rows() const noexcept
   {
      return m_data->Rows();
   }

   /*!
    * Returns the number of columns in this matrix. If this object is an empty
    * matrix, this member function returns zero.
    */
   int Cols() const noexcept
   {
      return m_data->Cols();
   }

   /*!
    * Returns the number of columns in this matrix. If this object is an empty
    * matrix, this member function returns zero.
    *
    * This function is a synonym for Cols().
    */
   int Columns() const noexcept
   {
      return Cols();
   }

   /*!
    * Returns true only if this matrix is valid. A matrix is valid if it
    * references an internal matrix structure, even if it is an empty matrix.
    *
    * In general, all %GenericMatrix objects are valid with only two
    * exceptions:
    *
    * \li Objects that have been move-copied or move-assigned to other
    * matrices.
    * \li Objects that have been invalidated explicitly by calling Transfer().
    *
    * An invalid matrix object cannot be used and should be destroyed
    * immediately. Invalid matrices are always destroyed automatically during
    * move construction and move assignment operations.
    */
   bool IsValid() const noexcept
   {
      return m_data != nullptr;
   }

   /*!
    * Returns true iff this is an empty matrix. An empty matrix has no elements,
    * and hence its dimensions are zero.
    */
   bool IsEmpty() const noexcept
   {
      return Rows() == 0 || Cols() == 0;
   }

   /*!
    * Returns true iff this matrix is not empty. This operator is equivalent to:
    *
    * \code !IsEmpty(); \endcode
    */
   operator bool() const noexcept
   {
      return !IsEmpty();
   }

   /*!
    * Returns the total number of matrix elements in this object, or
    * Rows()*Cols().
    */
   size_type NumberOfElements() const noexcept
   {
      return m_data->NumberOfElements();
   }

   /*!
    * Returns the total number of bytes required to store the data contained in
    * this matrix.
    */
   size_type Size() const noexcept
   {
      return m_data->Size();
   }

   /*!
    * Equality operator. Returns true if this matrix is equal to another matrix
    * \a x.
    *
    * %Matrix comparisons are performed element-wise. Two matrices are equal if
    * both have the same dimensions and identical element values.
    */
   bool operator ==( const GenericMatrix& x ) const noexcept
   {
      return IsAliasOf( x ) || SameDimensions( x ) && pcl::Equal( Begin(), x.Begin(), x.End() );
   }

   /*!
    * Less than relational operator. Returns true if this matrix is less than
    * another matrix \a x.
    *
    * In this operator, matrix comparisons are performed element-wise,
    * irrespective of matrix dimensions, until either two matrix elements
    * differ, or until the end of one of the matrices is reached. In the latter
    * case the shortest matrix is the lesser one.
    */
   bool operator <( const GenericMatrix& x ) const noexcept
   {
      return !IsAliasOf( x ) && pcl::Compare( Begin(), End(), x.Begin(), x.End() ) < 0;
   }

   /*!
    * Returns true iff this matrix has the same dimensions, i.e. the same number
    * of rows and columns, as another matrix \a x.
    */
   bool SameDimensions( const GenericMatrix& x ) const noexcept
   {
      return Rows() == x.Rows() && Cols() == x.Cols();
   }

   /*!
    * Returns true iff this matrix has the same elements as another matrix \a x.
    *
    * In this member function, matrix comparisons are performed element-wise,
    * irrespective of matrix dimensions. Note that two matrices can have
    * exactly the same elements even if their dimensions, i.e. their number of
    * rows and columns, are different. Only the number and order of elements
    * are relevant for this comparison.
    */
   bool SameElements( const GenericMatrix& x ) const noexcept
   {
      if ( unlikely( IsAliasOf( x ) ) )
         return true;
      if ( unlikely( NumberOfElements() != x.NumberOfElements() ) )
         return false;
      const_block_iterator __restrict__ i = Begin();
      const_block_iterator __restrict__ j = End();
      const_block_iterator __restrict__ k = x.Begin();
      PCL_IVDEP
      for ( ; i < j; ++i, ++k )
         if ( *i != *k )
            return false;
      return true;
   }

   /*!
    * Returns a reference to the matrix element at row \a i and column \a j.
    *
    * Before returning, this function ensures that this instance uniquely
    * references its matrix data.
    */
   element& Element( int i, int j )
   {
      EnsureUnique();
      return m_data->Element( i, j );
   }

   /*!
    * Returns a reference to the immutable matrix element at row \a i and
    * column \a j.
    */
   const element& Element( int i, int j ) const noexcept
   {
      return m_data->Element( i, j );
   }

   /*!
    * Returns a pointer to the first matrix element of row \a i.
    *
    * All elements in row \a i are guaranteed to be stored at consecutive
    * locations addressable from the pointer returned by this function.
    *
    * Before returning, this function ensures that this instance uniquely
    * references its matrix data.
    */
   block_iterator operator []( int i )
   {
      EnsureUnique();
      return m_data->v[i];
   }

   /*!
    * Returns a pointer to the immutable first matrix element of row \a i.
    *
    * All elements in row \a i are guaranteed to be stored at consecutive
    * locations addressable from the pointer returned by this function.
    */
   const_block_iterator operator []( int i ) const noexcept
   {
      return m_data->v[i];
   }

   /*!
    * Returns a pointer to the first matrix element, that is to the element at
    * row 0 and column 0 of this matrix.
    *
    * All matrix elements are guaranteed to be stored at consecutive locations
    * addressable from the pointer returned by this function. Matrix elements
    * are stored in row order: all elements of row 0 followed by all elements
    * of row 1, and so on.
    *
    * Before returning, this function ensures that this instance uniquely
    * references its matrix data.
    */
   block_iterator Begin()
   {
      EnsureUnique();
      return m_data->Begin();
   }

   /*!
    * Returns a pointer to the immutable first matrix element, that is to the
    * element at row 0 and column 0 of this matrix.
    *
    * All matrix elements are guaranteed to be stored at consecutive locations
    * addressable from the pointer returned by this function. Matrix elements
    * are stored in row order: all elements of row 0 followed by all elements
    * of row 1, and so on.
    */
   const_block_iterator Begin() const noexcept
   {
      return m_data->Begin();
   }

   /*!
    * A synonym for Begin() const.
    */
   const_block_iterator ConstBegin() const noexcept
   {
      return Begin();
   }

   /*!
    * Returns a pointer to the first matrix element, that is to the element at
    * row 0 and column 0 of this matrix.
    *
    * This member function is a convenience alias to Begin().
    */
   block_iterator operator *()
   {
      return Begin();
   }

   /*!
    * Returns a pointer to the immutable first matrix element, that is to the
    * element at row 0 and column 0 of this matrix.
    *
    * This member function is a convenience alias to Begin() const.
    */
   const_block_iterator operator *() const noexcept
   {
      return Begin();
   }

   /*!
    * Returns a pointer past the last matrix element, that is a pointer to an
    * (nonexistent) element located at row=Rows()-1 and column=Cols().
    *
    * All matrix elements are guaranteed to be stored in reverse order at
    * consecutive locations addressable from the pointer returned by this
    * function. Matrix elements are stored in row order (all elements of row n
    * followed by all elements of row n-1, and so on).
    *
    * Before returning, this function ensures that this instance uniquely
    * references its matrix data.
    */
   block_iterator End()
   {
      EnsureUnique();
      return m_data->End();
   }

   /*!
    * Returns a pointer past the last immutable matrix element, that is a
    * pointer to an immutable (and nonexistent) element located at row=Rows()-1
    * and column=Cols().
    *
    * All matrix elements are guaranteed to be stored in reverse order at
    * consecutive locations addressable from the pointer returned by this
    * function. Matrix elements are stored in row order (all elements of row n
    * followed by all elements of row n-1, and so on).
    */
   const_block_iterator End() const noexcept
   {
      return m_data->End();
   }

   /*!
    * A synonym for End() const.
    */
   const_block_iterator ConstEnd() const noexcept
   {
      return End();
   }

   /*!
    * Returns a pointer to a pointer to the first element in this matrix.
    *
    * The returned pointer can be used as a common C array, where all matrix
    * rows are guaranteed to be addressable through consecutive pointers
    * starting from the pointer returned by this function.
    *
    * This member function does nothing to ensure that this instance uniquely
    * references its matrix data. Consequently, the caller must take into
    * account that all modifications made to matrix elements accessed through
    * the value returned by this function will apply to all instances sharing
    * the same matrix data.
    *
    * This function can be used to perform fast operations on matrix elements,
    * avoiding the overhead caused by deep copies of matrix data, when such
    * copies are not necessary. Typically this happens when two or more threads
    * work simultaneously on non-overlapping regions of the same matrix.
    */
   block_iterator* DataPtr() noexcept
   {
      return m_data->v;
   }

   /*!
    * Returns a pointer to the first matrix element of row \a i.
    *
    * All elements in row \a i are guaranteed to be stored at consecutive
    * locations addressable from the pointer returned by this function.
    *
    * This member function does not ensure that the data referenced by this
    * matrix is unique. See DataPtr() for more information on how to use this
    * member function.
    */
   block_iterator RowPtr( int i ) noexcept
   {
      return m_data->v[i];
   }

#ifndef __PCL_NO_STL_COMPATIBLE_ITERATORS
   /*!
    * STL-compatible iteration. Equivalent to Begin().
    */
   block_iterator begin()
   {
      return Begin();
   }

   /*!
    * STL-compatible iteration. Equivalent to Begin() const.
    */
   const_block_iterator begin() const noexcept
   {
      return Begin();
   }

   /*!
    * STL-compatible iteration. Equivalent to End().
    */
   block_iterator end()
   {
      return End();
   }

   /*!
    * STL-compatible iteration. Equivalent to End() const.
    */
   const_block_iterator end() const noexcept
   {
      return End();
   }
#endif   // !__PCL_NO_STL_COMPATIBLE_ITERATORS

   /*!
    * Returns a vector with the matrix elements at the specified row \a i.
    */
   vector RowVector( int i ) const
   {
      vector r( m_data->Cols() );
      for ( int j = 0; j < m_data->Cols(); ++j )
         r[j] = m_data->v[i][j];
      return r;
   }

   /*!
    * Returns a vector with the matrix elements at the specified column \a j.
    */
   vector ColumnVector( int j ) const
   {
      vector c( m_data->Rows() );
      for ( int i = 0; i < m_data->Rows(); ++i )
         c[i] = m_data->v[i][j];
      return c;
   }

   /*!
    * Returns a vector with the matrix elements at the specified column \a j.
    *
    * This member function is an alias for ColumnVector().
    */
   vector ColVector( int j ) const
   {
      return ColumnVector( j );
   }

   /*!
    * Assigns the components of a vector \a r to the corresponding elements of
    * a matrix row \a i.
    */
   template <class V>
   void SetRow( int i, const V& r )
   {
      EnsureUnique();
      for ( int j = 0; j < m_data->Cols() && j < r.Length(); ++j )
         m_data->v[i][j] = element( r[j] );
   }

   /*!
    * Assigns the components of a vector \a c to the corresponding elements of
    * a matrix column \a i.
    */
   template <class V>
   void SetColumn( int j, const V& c )
   {
      EnsureUnique();
      for ( int i = 0; i < m_data->Rows() && i < c.Length(); ++i )
         m_data->v[i][j] = element( c[i] );
   }

   /*!
    * Assigns the components of a vector \a c to the corresponding elements of
    * a matrix column \a i.
    *
    * This member function is an alias for SetColumn().
    */
   template <class V>
   void SetCol( int j, const V& c )
   {
      SetColumn( j, c );
   }

   /*!
    * Returns a matrix whose first row is a copy of the specified vector \a r.
    * The rest of matrix elements are set to zero. The returned object is a
    * square matrix whose dimensions are equal to the length of \a r.
    */
   static GenericMatrix FromRowVector( const vector& r )
   {
      GenericMatrix R( element( 0 ), r.Length(), r.Length() );
      for ( int j = 0; j < r.Length(); ++j )
         R.m_data->v[0][j] = r[j];
      return R;
   }

   /*!
    * Returns a matrix whose first column is a copy of the specified vector
    * \a c. The rest of matrix elements are set to zero. The returned object is
    * a square matrix whose dimensions are equal to the length of \a c.
    */
   static GenericMatrix FromColumnVector( const vector& c )
   {
      GenericMatrix C( element( 0 ), c.Length(), c.Length() );
      for ( int i = 0; i < c.Length(); ++i )
         C.m_data->v[i][0] = c[i];
      return C;
   }

   /*!
    * This member function is an alias for FromColumnVector().
    */
   static GenericMatrix FromColVector( const vector& c )
   {
      return FromColumnVector( c );
   }

   /*!
    * Returns a unit matrix of size \a n.
    *
    * A unit matrix is a square matrix whose elements are ones on its main
    * diagonal and zeros elsewhere.
    */
   static GenericMatrix UnitMatrix( int n )
   {
      GenericMatrix One( element( 0 ), n, n );
      for ( int i = 0; i < n; ++i )
         One[i][i] = element( 1 );
      return One;
   }

   /*!
    * Returns the transpose of this matrix.
    *
    * The transpose of a matrix A is another matrix A<sup>T</sup> whose rows
    * are the columns of A (or, equivalently, whose columns are the rows of A).
    */
   GenericMatrix Transpose() const
   {
      GenericMatrix Tr( m_data->Cols(), m_data->Rows() );
      for ( int i = 0; i < m_data->Rows(); ++i )
         for ( int j = 0; j < m_data->Cols(); ++j )
            Tr.m_data->v[j][i] = m_data->v[i][j];
      return Tr;
   }

   /*!
    * Returns the inverse of this matrix.
    *
    * When a square matrix is multiplied by its inverse, the result is the
    * identity matrix. Non-square matrices do not have inverses.
    *
    * If this is an empty or non-square matrix, or if it is singular (i.e. if
    * its determinant is zero or insignificant), then this function throws the
    * appropriate Error exception.
    */
   GenericMatrix Inverse() const;

   /*!
    * Inverts this matrix.
    *
    * When a square matrix is multiplied by its inverse, the result is the
    * identity matrix. Non-square matrices do not have inverses.
    *
    * If this is an empty or non-square matrix, or if it is singular (i.e. if
    * its determinant is zero or insignificant), then this function throws the
    * appropriate Error exception.
    */
   void Invert();

   /*!
    * Flips this matrix. Flipping a matrix consists of rotating its elements by
    * 180 degrees.
    *
    * This function ensures that this instance uniquely references its matrix
    * data before performing matrix rotation.
    */
   void Flip()
   {
      EnsureUnique();
      pcl::Reverse( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns a flipped copy of this matrix. Flipping a matrix consists of
    * rotating its elements by 180 degrees.
    */
   GenericMatrix Flipped() const
   {
      GenericMatrix Tf( m_data->Cols(), m_data->Rows() );
      pcl::CopyReversed( Tf.m_data->End(), m_data->Begin(), m_data->End() );
      return Tf;
   }

   /*!
    * Returns a 3x3 translation matrix.
    *
    * \param dx   Translation increment on the X axis.
    * \param dy   Translation increment on the Y axis.
    *
    * The returned matrix is:
    *
    * <pre>
    * 1   0   dx
    * 0   1   dy
    * 0   0   1
    * </pre>
    *
    * \sa Translate( double, double )
    */
   static GenericMatrix Translation( double dx, double dy )
   {
      return GenericMatrix( element( 1 ), element( 0 ), element( dx ),
                            element( 0 ), element( 1 ), element( dy ),
                            element( 0 ), element( 0 ), element( 1  ) );
   }

   /*!
    * Returns a 3x3 translation matrix for the specified translation vector
    * \a delta.
    *
    * The type V is expected to provide the array subscript operator:
    *
    * C V::operator []( int i ) const
    *
    * where the type C must have conversion-to-double semantics. The subindex i
    * will be either 0 or 1 for the translation increments on the X and Y axes,
    * respectively.
    *
    * \sa Translation( double, double )
    */
   template <class V>
   static GenericMatrix Translation( const V& delta )
   {
      return GenericMatrix( element( 1 ), element( 0 ), element( double( delta[0] ) ),
                            element( 0 ), element( 1 ), element( double( delta[1] ) ),
                            element( 0 ), element( 0 ), element( 1                ) );
   }

   /*!
    * Translates this 3x3 matrix.
    *
    * \param dx   Translation increment on the X axis.
    * \param dy   Translation increment on the Y axis.
    *
    * This function multiplies this matrix by the translation matrix:
    *
    * <pre>
    * 1   0   dx
    * 0   1   dy
    * 0   0   1
    * </pre>
    *
    * If this matrix has dimensions different from 3 rows and 3 columns, this
    * function invokes undefined behavior. For the sake of performance, this
    * condition is not explicitly verified.
    *
    * \sa Translation( double, double )
    */
   void Translate( double dx, double dy )
   {
      PCL_PRECONDITION( Rows() == 3 && Cols() == 3 )
      EnsureUnique();
      block_iterator __restrict__ A0 = m_data->v[0];
      block_iterator __restrict__ A1 = m_data->v[1];
      block_iterator __restrict__ A2 = m_data->v[2];
      A0[2] = element( A0[2] + A0[0]*dx + A0[1]*dy );
      A1[2] = element( A1[2] + A1[0]*dx + A1[1]*dy );
      A2[2] = element( A2[2] + A2[0]*dx + A2[1]*dy );
   }

   /*!
    * Translates this 3x3 matrix by the specified translation vector \a delta.
    *
    * See Translation( const V& ) for detailed information on the expected
    * properties of the type V.
    *
    * \sa Translate( double, double )
    */
   template <class V>
   void Translate( const V& delta )
   {
      Translate( double( delta[0] ), double( delta[1] ) );
   }

   /*!
    * Returns a translated copy of this 3x3 matrix by the specified increments
    * \a dx and \a dy on the X and Y axes, respectively. See Translate().
    *
    * \sa Translate( double, double )
    */
   GenericMatrix Translated( double dx, double dy ) const
   {
      GenericMatrix R( *this );
      R.Translate( dx, dy );
      return R;
   }

   /*!
    * Returns a translated copy of this 3x3 matrix by the specified translation
    * vector \a delta.
    *
    * See Translation( const V& ) for detailed information on the expected
    * properties of the type V.
    *
    * \sa Translated( double, double )
    */
   template <class V>
   GenericMatrix Translated( const V& delta ) const
   {
      return Translated( double( delta[0] ), double( delta[1] ) );
   }

   /*!
    * Returns a 3x3 rotation matrix about the X axis.
    *
    * \param sphi    Sine of the rotation angle.
    *
    * \param cphi    Cosine of the rotation angle.
    *
    * Positive rotation angles apply anticlockwise rotations about the X axis,
    * as seen looking towards the origin from positive x.
    *
    * The returned matrix is:
    *
    * <pre>
    * 1     0       0
    * 0   +cphi   +sphi
    * 0   -sphi   +cphi
    * </pre>
    *
    * \sa RotationX( double )
    */
   static GenericMatrix RotationX( double sphi, double cphi )
   {
      return GenericMatrix( element( 1 ), element(     0 ), element(     0 ),
                            element( 0 ), element( +cphi ), element( +sphi ),
                            element( 0 ), element( -sphi ), element( +cphi ) );
   }

   /*!
    * Returns a 3x3 rotation matrix about the X axis by the specified angle
    * \a phi in radians.
    *
    * Calling this function is equivalent to:
    *
    * \code RotationX( Sin( phi ), Cos( phi ) ) \endcode
    */
   static GenericMatrix RotationX( double phi )
   {
      double sphi, cphi;
      SinCos( phi, sphi, cphi );
      return RotationX( sphi, cphi );
   }

   /*!
    * Returns a 3x3 rotation matrix about the Y axis.
    *
    * \param sphi    Sine of the rotation angle.
    *
    * \param cphi    Cosine of the rotation angle.
    *
    * Positive rotation angles apply anticlockwise rotations about the Y axis,
    * as seen looking towards the origin from positive y.
    *
    * The returned matrix is:
    *
    * <pre>
    * +cphi  0  -sphi
    *   0    1    0
    * +sphi  0  +cphi
    * </pre>
    *
    * \sa RotationY( double )
    */
   static GenericMatrix RotationY( double sphi, double cphi )
   {
      return GenericMatrix( element( +cphi ), element( 0 ), element( -sphi ),
                            element(     0 ), element( 1 ), element( 0     ),
                            element( +sphi ), element( 0 ), element( +cphi ) );
   }

   /*!
    * Returns a 3x3 rotation matrix about the Y axis by the specified angle
    * \a phi in radians.
    *
    * Calling this function is equivalent to:
    *
    * \code RotationY( Sin( phi ), Cos( phi ) ) \endcode
    */
   static GenericMatrix RotationY( double phi )
   {
      double sphi, cphi;
      SinCos( phi, sphi, cphi );
      return RotationY( sphi, cphi );
   }

   /*!
    * Returns a 3x3 rotation matrix about the Z axis.
    *
    * \param sphi    Sine of the rotation angle.
    *
    * \param cphi    Cosine of the rotation angle.
    *
    * Positive rotation angles apply anticlockwise rotations about the Z axis,
    * as seen looking towards the origin from positive z.
    *
    * The returned matrix is:
    *
    * <pre>
    * +cphi  +sphi  0
    * -sphi  +cphi  0
    *   0      0    1
    * </pre>
    *
    * \sa RotationZ( double )
    */
   static GenericMatrix RotationZ( double sphi, double cphi )
   {
      return GenericMatrix( element( +cphi ), element( +sphi ), element( 0 ),
                            element( -sphi ), element( +cphi ), element( 0 ),
                            element(     0 ), element(     0 ), element( 1 ) );
   }

   /*!
    * Returns a 3x3 rotation matrix about the Z axis by the specified angle
    * \a phi in radians.
    *
    * Calling this function is equivalent to:
    *
    * \code RotationZ( Sin( phi ), Cos( phi ) ) \endcode
    */
   static GenericMatrix RotationZ( double phi )
   {
      double sphi, cphi;
      SinCos( phi, sphi, cphi );
      return RotationZ( sphi, cphi );
   }

   /*!
    * Rotates this 3x3 matrix about the X axis.
    *
    * \param sphi    Sine of the rotation angle.
    *
    * \param cphi    Cosine of the rotation angle.
    *
    * Positive rotation angles apply anticlockwise rotations about the X axis,
    * as seen looking towards the origin from positive x. The applied
    * transformation can be represented by the matrix:
    *
    * <pre>
    * 1     0        0
    * 0   +cphi   +sphi
    * 0   -sphi   +cphi
    * </pre>
    *
    * If this matrix has dimensions different from 3 rows and 3 columns, this
    * function invokes undefined behavior. For the sake of performance, this
    * condition is not explicitly verified.
    *
    * \sa RotateX( double ), RotatedX()
    */
   void RotateX( double sphi, double cphi )
   {
      PCL_PRECONDITION( Rows() == 3 && Cols() == 3 )
      EnsureUnique();
      block_iterator __restrict__ A1 = m_data->v[1];
      block_iterator __restrict__ A2 = m_data->v[2];
      double a10 =  cphi*A1[0] + sphi*A2[0];
      double a11 =  cphi*A1[1] + sphi*A2[1];
      double a12 =  cphi*A1[2] + sphi*A2[2];
      double a20 = -sphi*A1[0] + cphi*A2[0];
      double a21 = -sphi*A1[1] + cphi*A2[1];
      double a22 = -sphi*A1[2] + cphi*A2[2];
      A1[0] = element( a10 );
      A1[1] = element( a11 );
      A1[2] = element( a12 );
      A2[0] = element( a20 );
      A2[1] = element( a21 );
      A2[2] = element( a22 );
   }

   /*!
    * Rotates this 3x3 matrix about the X axis by the specified angle \a phi in
    * radians.
    *
    * Calling this function is equivalent to:
    *
    * \code RotateX( Sin( phi ), Cos( phi ) ) \endcode
    */
   void RotateX( double phi )
   {
      double sphi, cphi;
      SinCos( phi, sphi, cphi );
      RotateX( sphi, cphi );
   }

   /*!
    * Returns a rotated copy of this 3x3 matrix about the X axis by a rotation
    * angle given by its sine \a sphi and cosine \a cphi.
    * See RotateX( double, double ).
    */
   GenericMatrix RotatedX( double sphi, double cphi ) const
   {
      GenericMatrix R( *this );
      R.RotateX( sphi, cphi );
      return R;
   }

   /*!
    * Returns a rotated copy of this 3x3 matrix about the X axis by the
    * specified rotation angle \a phi in radians. See RotateX( double ).
    */
   GenericMatrix RotatedX( double phi ) const
   {
      GenericMatrix R( *this );
      R.RotateX( phi );
      return R;
   }

   /*!
    * Rotates this 3x3 matrix about the Y axis.
    *
    * \param sphi    Sine of the rotation angle.
    *
    * \param cphi    Cosine of the rotation angle.
    *
    * Positive rotation angles apply anticlockwise rotations about the Y axis,
    * as seen looking towards the origin from positive y. The applied
    * transformation can be represented by the matrix:
    *
    * <pre>
    * +cphi  0  -sphi
    *   0    1    0
    * +sphi  0  +cphi
    * </pre>
    *
    * If this matrix has dimensions different from 3 rows and 3 columns, this
    * function invokes undefined behavior. For the sake of performance, this
    * condition is not explicitly verified.
    *
    * \sa RotateY( double ), RotatedY()
    */
   void RotateY( double sphi, double cphi )
   {
      PCL_PRECONDITION( Rows() == 3 && Cols() == 3 )
      EnsureUnique();
      block_iterator __restrict__ A0 = m_data->v[0];
      block_iterator __restrict__ A2 = m_data->v[2];
      double a00 = cphi*A0[0] - sphi*A2[0];
      double a01 = cphi*A0[1] - sphi*A2[1];
      double a02 = cphi*A0[2] - sphi*A2[2];
      double a20 = sphi*A0[0] + cphi*A2[0];
      double a21 = sphi*A0[1] + cphi*A2[1];
      double a22 = sphi*A0[2] + cphi*A2[2];
      A0[0] = a00;
      A0[1] = a01;
      A0[2] = a02;
      A2[0] = a20;
      A2[1] = a21;
      A2[2] = a22;
   }

   /*!
    * Rotates this 3x3 matrix about the Y axis by the specified angle \a phi in
    * radians.
    *
    * Calling this function is equivalent to:
    *
    * \code RotateY( Sin( phi ), Cos( phi ) ) \endcode
    */
   void RotateY( double phi )
   {
      double sphi, cphi;
      SinCos( phi, sphi, cphi );
      RotateY( sphi, cphi );
   }

   /*!
    * Returns a rotated copy of this 3x3 matrix about the Y axis by a rotation
    * angle given by its sine \a sphi and cosine \a cphi.
    * See RotateY( double, double ).
    */
   GenericMatrix RotatedY( double sphi, double cphi ) const
   {
      GenericMatrix R( *this );
      R.RotateY( sphi, cphi );
      return R;
   }

   /*!
    * Returns a rotated copy of this 3x3 matrix about the Y axis by the
    * specified rotation angle \a phi in radians. See RotateY( double ).
    */
   GenericMatrix RotatedY( double phi ) const
   {
      GenericMatrix R( *this );
      R.RotateY( phi );
      return R;
   }

   /*!
    * Rotates this 3x3 matrix about the Z axis.
    *
    * \param sphi    Sine of the rotation angle.
    *
    * \param cphi    Cosine of the rotation angle.
    *
    * Positive rotation angles apply anticlockwise rotations about the Z axis,
    * as seen looking towards the origin from positive z. The applied
    * transformation can be represented by the matrix:
    *
    * <pre>
    * +cphi  +sphi  0
    * -sphi  +cphi  0
    *   0      0    1
    * </pre>
    *
    * If this matrix has dimensions different from 3 rows and 3 columns, this
    * function invokes undefined behavior. For the sake of performance, this
    * condition is not explicitly verified.
    *
    * \sa RotateZ( double ), RotatedZ()
    */
   void RotateZ( double sphi, double cphi )
   {
      PCL_PRECONDITION( Rows() == 3 && Cols() == 3 )
      EnsureUnique();
      block_iterator __restrict__ A0 = m_data->v[0];
      block_iterator __restrict__ A1 = m_data->v[1];
      double a00 =  cphi*A0[0] + sphi*A1[0];
      double a01 =  cphi*A0[1] + sphi*A1[1];
      double a02 =  cphi*A0[2] + sphi*A1[2];
      double a10 = -sphi*A0[0] + cphi*A1[0];
      double a11 = -sphi*A0[1] + cphi*A1[1];
      double a12 = -sphi*A0[2] + cphi*A1[2];
      A0[0] = a00;
      A0[1] = a01;
      A0[2] = a02;
      A1[0] = a10;
      A1[1] = a11;
      A1[2] = a12;
   }

   /*!
    * Rotates this 3x3 matrix about the Z axis by the specified angle \a phi in
    * radians.
    *
    * Calling this function is equivalent to:
    *
    * \code RotateZ( Sin( phi ), Cos( phi ) ) \endcode
    */
   void RotateZ( double phi )
   {
      double sphi, cphi;
      SinCos( phi, sphi, cphi );
      RotateZ( sphi, cphi );
   }

   /*!
    * Returns a rotated copy of this 3x3 matrix about the Z axis by a rotation
    * angle given by its sine \a sphi and cosine \a cphi.
    * See RotateZ( double, double ).
    */
   GenericMatrix RotatedZ( double sphi, double cphi ) const
   {
      GenericMatrix R( *this );
      R.RotateZ( sphi, cphi );
      return R;
   }

   /*!
    * Returns a rotated copy of this 3x3 matrix about the Z axis by the
    * specified rotation angle \a phi in radians. See RotateZ( double ).
    */
   GenericMatrix RotatedZ( double phi ) const
   {
      GenericMatrix R( *this );
      R.RotateZ( phi );
      return R;
   }

   /*!
    * Truncates all matrix elements to the specified range.
    *
    * \param f0   Lower bound of the truncation range. If not specified, the
    *             default value is zero.
    *
    * \param f1   Upper bound of the truncation range. If not specified, the
    *             default value is one.
    *
    * After calling this member function, all matrix elements will be within
    * the  [\a f0,\a f1] range.
    */
   void Truncate( const element& f0 = element( 0 ), const element& f1 = element( 1 ) )
   {
      EnsureUnique();
      block_iterator __restrict__ i = m_data->Begin();
      block_iterator __restrict__ j = m_data->End();
      PCL_IVDEP
      for ( ; i < j; ++i )
         if ( *i < f0 )
            *i = f0;
         else if ( *i > f1 )
            *i = f1;
   }

   /*!
    * Returns a truncated copy of this matrix. See Truncate().
    */
   GenericMatrix Truncated( const element& f0 = element( 0 ), const element& f1 = element( 1 ) ) const
   {
      GenericMatrix R( *this );
      R.Truncate( f0, f1 );
      return R;
   }

   /*!
    * Rescales all matrix elements to the specified range.
    *
    * \param f0   Lower bound of the rescaling range. If not specified, the
    *             default value is zero.
    *
    * \param f1   Upper bound of the rescaling range. If not specified, the
    *             default value is one.
    *
    * The rescaling operation is as follows. if \a m and \a M are the minimum
    * and maximum element values in this matrix, respectively, then for each
    * matrix element s, its rescaled value r is given by:
    *
    * \a r = \a f0 + (\a s - \a m)*(\a f1 - \a f0)/(\a M - \a m)
    */
   void Rescale( const element& f0 = element( 0 ), const element& f1 = element( 1 ) )
   {
      element v0 = MinElement();
      element v1 = MaxElement();
      if ( v0 != f0 || v1 != f1 )
      {
         EnsureUnique();
         if ( v0 != v1 )
         {
            if ( f0 != f1 )
            {
               block_iterator __restrict__ i = m_data->Begin();
               block_iterator __restrict__ j = m_data->End();
               double d = (double( f1 ) - double( f0 ))/(double( v1 ) - double( v0 ));
               if ( f0 == element( 0 ) )
               {
                  PCL_IVDEP
                  for ( ; i < j; ++i )
                     *i = element( d*(*i - v0) );
               }
               else
               {
                  PCL_IVDEP
                  for ( ; i < j; ++i )
                     *i = element( d*(*i - v0) + f0 );
               }
            }
            else
               pcl::Fill( m_data->Begin(), m_data->End(), f0 );
         }
         else
            pcl::Fill( m_data->Begin(), m_data->End(), pcl::Range( v0, f0, f1 ) );
      }
   }

   /*!
    * Returns a rescaled copy of this matrix. See Rescale().
    */
   GenericMatrix Rescaled( const element& f0 = element( 0 ), const element& f1 = element( 1 ) ) const
   {
      GenericMatrix R( *this );
      R.Rescale( f0, f1 );
      return R;
   }

   /*!
    * Sorts the elements of this matrix in ascending order.
    */
   void Sort()
   {
      EnsureUnique();
      pcl::Sort( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns a sorted copy of this matrix.
    */
   GenericMatrix Sorted() const
   {
      GenericMatrix R( *this );
      R.Sort();
      return R;
   }

   /*!
    * Sorts the elements of this matrix in reverse (descending) order.
    */
   void ReverseSort()
   {
      EnsureUnique();
      pcl::Sort( m_data->Begin(), m_data->End(),
                 []( const element& a, const element& b ){ return b < a; } );
   }

   /*!
    * Returns a reverse sorted copy of this matrix.
    */
   GenericMatrix ReverseSorted() const
   {
      GenericMatrix R( *this );
      R.ReverseSort();
      return R;
   }

   /*!
    * Sorts the elements of this matrix in ascending order. Ordering of matrix
    * elements is defined such that for any pair a, b of matrix elements, the
    * specified binary predicate p( a, b ) is true if a precedes b.
    */
   template <class BP>
   void Sort( BP p )
   {
      EnsureUnique();
      pcl::Sort( m_data->Begin(), m_data->End(), p );
   }

   /*!
    * Returns a sorted copy of this matrix, where ordering of matrix elements
    * is defined by the specified binary predicate \a p. See Sort( BP p ).
    */
   template <class BP>
   GenericMatrix Sorted( BP p ) const
   {
      GenericMatrix R( *this );
      R.Sort( p );
      return R;
   }

   /*!
    * Returns a pointer to the first immutable matrix element with the
    * specified value \a x, or nullptr if this matrix does not contain such
    * value.
    */
   const_block_iterator Find( const element& x ) const noexcept
   {
      const_block_iterator p = pcl::LinearSearch( m_data->Begin(), m_data->End(), x );
      return (p != m_data->End()) ? p : nullptr;
   }

   /*!
    * Returns a pointer to the first immutable matrix element with the
    * specified value \a x, or nullptr if this matrix does not contain such
    * value. This function is an alias to Find().
    */
   const_block_iterator FindFirst( const element& x ) const noexcept
   {
      return Find( x );
   }

   /*!
    * Returns a pointer to the last immutable matrix element with the specified
    * value \a x, or nullptr if this matrix does not contain such value.
    */
   const_block_iterator FindLast( const element& x ) const noexcept
   {
      const_block_iterator p = pcl::LinearSearchLast( m_data->Begin(), m_data->End(), x );
      return (p != m_data->End()) ? p : nullptr;
   }

   /*!
    * Returns true iff this matrix contains the specified value \a x.
    */
   bool Contains( const element& x ) const noexcept
   {
      return pcl::LinearSearch( m_data->Begin(), m_data->End(), x ) != m_data->End();
   }

#ifndef __PCL_NO_MATRIX_STATISTICS

   /*!
    * Returns the value of the minimum element in this matrix.
    * For empty matrices, this function returns zero.
    */
   element MinElement() const noexcept
   {
      if ( !IsEmpty() )
         return *MinItem( m_data->Begin(), m_data->End() );
      return element( 0 );
   }

   /*!
    * Returns the value of the minimum element in this matrix and its
    * coordinates in the specified point \a p.
    * For empty matrices, this function returns zero and assigns zero to the
    * point coordinates.
    */
   element MinElement( Point& p ) const noexcept
   {
      if ( !IsEmpty() )
      {
         const_block_iterator m = MinItem( m_data->Begin(), m_data->End() );
         distance_type d = m - m_data->Begin();
         p.y = int( d/m_data->Cols() );
         p.x = int( d%m_data->Cols() );
         return *m;
      }
      p = 0;
      return element( 0 );
   }

   /*!
    * Returns the value of the maximum element in this matrix.
    * For empty matrices, this function returns zero.
    */
   element MaxElement() const noexcept
   {
      if ( !IsEmpty() )
         return *MaxItem( m_data->Begin(), m_data->End() );
      return element( 0 );
   }

   /*!
    * Returns the value of the maximum element in this matrix and its
    * coordinates in the specified point \a p.
    * For empty matrices, this function returns zero and assigns zero to the
    * point coordinates.
    */
   element MaxElement( Point& p ) const noexcept
   {
      if ( !IsEmpty() )
      {
         const_block_iterator m = MaxItem( m_data->Begin(), m_data->End() );
         int d = m - m_data->Begin();
         p.y = d/m_data->Cols();
         p.x = d%m_data->Cols();
         return *m;
      }
      p = 0;
      return element( 0 );
   }

   /*!
    * Returns the sum of all matrix elements.
    *
    * For empty matrices, this function returns zero.
    */
   double Sum() const noexcept
   {
      return pcl::Sum( m_data->Begin(), m_data->End() );
   }

   /*!
    * Computes the sum of all matrix elements using a numerically stable
    * summation algorithm to minimize roundoff error.
    *
    * For empty matrices, this function returns zero.
    */
   double StableSum() const noexcept
   {
      return pcl::StableSum( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns the sum of the absolute values of all matrix elements.
    *
    * For empty matrices, this function returns zero.
    */
   double Modulus() const noexcept
   {
      return pcl::Modulus( m_data->Begin(), m_data->End() );
   }

   /*!
    * Computes the sum of the absolute values of all matrix elements using a
    * numerically stable summation algorithm to minimize roundoff error.
    *
    * For empty matrices, this function returns zero.
    */
   double StableModulus() const noexcept
   {
      return pcl::StableModulus( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns the sum of the squares of all matrix elements.
    *
    * For empty matrices, this function returns zero.
    */
   double SumOfSquares() const noexcept
   {
      return pcl::SumOfSquares( m_data->Begin(), m_data->End() );
   }

   /*!
    * Computes the sum of the squares of all matrix elements using a
    * numerically stable summation algorithm to minimize roundoff error.
    *
    * For empty matrices, this function returns zero.
    */
   double StableSumOfSquares() const noexcept
   {
      return pcl::StableSumOfSquares( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns the mean of the element values in this matrix.
    *
    * For empty matrices, this function returns zero.
    */
   double Mean() const noexcept
   {
      return pcl::Mean( m_data->Begin(), m_data->End() );
   }

   /*!
    * Computes the mean of the element values in this matrix using a
    * numerically stable summation algorithm to minimize roundoff error.
    *
    * For empty matrices, this function returns zero.
    */
   double StableMean() const noexcept
   {
      return pcl::StableMean( m_data->Begin(), m_data->End() );
   }

   /*!
    * Computes the two-sided, asymmetric trimmed mean of the values in this
    * matrix. See pcl::TrimmedMean() for a complete description of the
    * implemented algorithm with information on function parameters.
    *
    * For empty matrices, this function returns zero.
    */
   double TrimmedMean( distance_type l = 1, distance_type h = 1 ) const noexcept
   {
      return pcl::TrimmedMean( m_data->Begin(), m_data->End(), l, h );
   }

   /*!
    * Computes the two-sided, asymmetric trimmed mean of the squared values in
    * this matrix. See pcl::TrimmedMeanOfSquares() for a complete description
    * of the implemented algorithm with information on function parameters.
    *
    * For empty matrices, this function returns zero.
    */
   double TrimmedMeanOfSquares( distance_type l = 1, distance_type h = 1 ) const noexcept
   {
      return pcl::TrimmedMeanOfSquares( m_data->Begin(), m_data->End(), l, h );
   }

   /*!
    * Returns the variance from the mean for the values in this matrix.
    *
    * For matrices with less than two elements, this function returns zero.
    */
   double Variance() const noexcept
   {
      return pcl::Variance( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns the standard deviation from the mean for the values in this
    * matrix.
    *
    * For matrices with less than two elements, this function returns zero.
    */
   double StdDev() const noexcept
   {
      return pcl::StdDev( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns the median of element values in this matrix.
    *
    * For matrices with less than two elements, this function returns zero.
    */
   double Median() const
   {
      return pcl::Median( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns the average absolute deviation with respect to the specified
    * \a center value.
    *
    * When the median of the matrix elements is used as the center value, this
    * function returns the average absolute deviation from the median, which is
    * a well-known estimator of dispersion.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the average absolute deviation about the median consistent
    * with the standard deviation of a normal distribution, it must be
    * multiplied by the constant 1.2533.
    */
   double AvgDev( double center ) const noexcept
   {
      return pcl::AvgDev( m_data->Begin(), m_data->End(), center );
   }

   /*!
    * Computes the average absolute deviation with respect to the specified
    * \a center value, using a numerically stable summation algorithm to
    * minimize roundoff error.
    *
    * When the median of the matrix elements is used as the center value, this
    * function returns the average absolute deviation from the median, which is
    * a well-known estimator of dispersion.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the average absolute deviation about the median consistent
    * with the standard deviation of a normal distribution, it must be
    * multiplied by the constant 1.2533.
    */
   double StableAvgDev( double center ) const noexcept
   {
      return pcl::StableAvgDev( m_data->Begin(), m_data->End(), center );
   }

   /*!
    * Returns the average absolute deviation from the median.
    *
    * The mean absolute deviation from the median is a well-known estimator of
    * dispersion.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the average absolute deviation about the median consistent
    * with the standard deviation of a normal distribution, it must be
    * multiplied by the constant 1.2533.
    */
   double AvgDev() const
   {
      return pcl::AvgDev( m_data->Begin(), m_data->End() );
   }

   /*!
    * Computes the average absolute deviation from the median using a
    * numerically stable summation algorithm to minimize roundoff error.
    *
    * The mean absolute deviation from the median is a well-known estimator of
    * dispersion.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the average absolute deviation about the median consistent
    * with the standard deviation of a normal distribution, it must be
    * multiplied by the constant 1.2533.
    */
   double StableAvgDev() const
   {
      return pcl::StableAvgDev( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns the two-sided average absolute deviation with respect to the
    * specified \a center value.
    *
    * See AvgDev( double ) for more information.
    */
   TwoSidedEstimate TwoSidedAvgDev( double center ) const noexcept
   {
      return pcl::TwoSidedAvgDev( m_data->Begin(), m_data->End(), center );
   }

   /*!
    * Returns the two-sided average absolute deviation from the median.
    *
    * See AvgDev() for more information.
    */
   TwoSidedEstimate TwoSidedAvgDev() const
   {
      return pcl::TwoSidedAvgDev( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns the median absolute deviation (MAD) with respect to the specified
    * \a center value.
    *
    * The MAD is a well-known robust estimator of scale.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the MAD estimator consistent with the standard deviation of
    * a normal distribution, it must be multiplied by the constant 1.4826.
    */
   double MAD( double center ) const
   {
      return pcl::MAD( m_data->Begin(), m_data->End(), center );
   }

   /*!
    * Returns the median absolute deviation from the median (MAD).
    *
    * The MAD is a well-known robust estimator of scale.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the MAD estimator consistent with the standard deviation of
    * a normal distribution, it must be multiplied by the constant 1.4826.
    */
   double MAD() const
   {
      return pcl::MAD( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns the two-sided median absolute deviation (MAD) with respect to the
    * specified \a center value.
    *
    * See MAD( double ) for more information.
    */
   TwoSidedEstimate TwoSidedMAD( double center ) const
   {
      return pcl::TwoSidedMAD( m_data->Begin(), m_data->End(), center );
   }

   /*!
    * Returns the two-sided median absolute deviation from the median (MAD).
    *
    * See MAD() for more information.
    */
   TwoSidedEstimate TwoSidedMAD() const
   {
      return pcl::TwoSidedMAD( m_data->Begin(), m_data->End() );
   }

   /*!
    * Returns the biweight midvariance (BWMV).
    *
    * \param center  Reference center value. Normally, the median of the matrix
    *                elements should be used.
    *
    * \param sigma   A reference estimate of dispersion. Normally, the median
    *                absolute deviation from the median (MAD) of the matrix
    *                elements should be used.
    *
    * \param k       Rejection limit in sigma units. The default value is k=9.
    *
    * \param reducedLength    If true, reduce the sample size to exclude
    *                rejected matrix elements. This tends to approximate the
    *                true dispersion of the data more accurately for relatively
    *                small samples, or samples with large amounts of outliers.
    *                Note that this departs from the standard definition of
    *                biweight midvariance, where the total number of data items
    *                is used to scale the variance estimate. If false, use the
    *                full number of matrix elements, including rejected
    *                elements, which gives a standard biweight midvariance
    *                estimate.
    *
    * The square root of the biweight midvariance is a robust estimator of
    * scale. It is an efficient estimator with respect to many statistical
    * distributions (about 87% Gaussian efficiency), and appears to have a
    * breakdown point close to 0.5 (the same as MAD).
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \b References
    *
    * Rand R. Wilcox (2017), <em>Introduction to Robust Estimation and
    * Hypothesis Testing, 4th Edition</em>, Elsevier Inc., Section 3.12.1.
    */
   double BiweightMidvariance( double center, double sigma, int k = 9, bool reducedLength = false ) const noexcept
   {
      return pcl::BiweightMidvariance( m_data->Begin(), m_data->End(), center, sigma, k, reducedLength );
   }

   /*!
    * Returns the biweight midvariance (BWMV) with respect to the median and
    * the median absolute deviation from the median (MAD).
    *
    * \param k       Rejection limit in sigma units. The default value is k=9.
    *
    * \param reducedLength    If true, reduce the sample size to exclude
    *                rejected matrix elements. This tends to approximate the
    *                true dispersion of the data more accurately for relatively
    *                small samples, or samples with large amounts of outliers.
    *                Note that this departs from the standard definition of
    *                biweight midvariance, where the total number of data items
    *                is used to scale the variance estimate. If false, use the
    *                full number of matrix elements, including rejected
    *                elements, which gives a standard biweight midvariance
    *                estimate.
    *
    * The square root of the biweight midvariance is a robust estimator of
    * scale. It is an efficient estimator with respect to many statistical
    * distributions (about 87% Gaussian efficiency), and appears to have a
    * breakdown point close to 0.5 (the same as MAD).
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \b References
    *
    * Rand R. Wilcox (2017), <em>Introduction to Robust Estimation and
    * Hypothesis Testing, 4th Edition</em>, Elsevier Inc., Section 3.12.1.
    */
   double BiweightMidvariance( int k = 9, bool reducedLength = false ) const
   {
      double center = Median();
      return BiweightMidvariance( center, MAD( center ), k, reducedLength );
   }

   /*!
    * Returns the two-sided biweight midvariance (BWMV).
    *
    * \param center  Reference center value. Normally, the median of the vector
    *                components should be used.
    *
    * \param sigma   A two-sided reference estimate of dispersion. Normally,
    *                the two-sided median absolute deviation from the median
    *                (MAD) of the vector components should be used. See the
    *                TwoSidedMAD() member function.
    *
    * See BiweightMidvariance( double, double, int, bool ) for more information
    * on the rest of parameters and references.
    */
   TwoSidedEstimate TwoSidedBiweightMidvariance( double center, const TwoSidedEstimate& sigma,
                                                 int k = 9, bool reducedLength = false ) const noexcept
   {
      return pcl::TwoSidedBiweightMidvariance( m_data->Begin(), m_data->End(), center, sigma, k, reducedLength );
   }

   /*!
    * Returns the two-sided biweight midvariance (BWMV) with respect to the
    * median and the two-sided median absolute deviation from the median (MAD).
    *
    * See BiweightMidvariance( int, bool ) for more information and references.
    */
   TwoSidedEstimate TwoSidedBiweightMidvariance( int k = 9, bool reducedLength = false ) const
   {
      double center = Median();
      return TwoSidedBiweightMidvariance( center, TwoSidedMAD( center ), k, reducedLength );
   }

   /*!
    * Returns a percentage bend midvariance (PBMV).
    *
    * \param center  Reference center value. Normally, the median of the matrix
    *                elements should be used.
    *
    * \param beta    Rejection parameter in the [0,0.5] range. Higher values
    *                improve robustness to outliers (i.e., increase the
    *                breakdown point of the estimator) at the expense of lower
    *                efficiency. The default value is beta=0.2.
    *
    * The square root of the percentage bend midvariance is a robust estimator
    * of scale. With the default beta=0.2, its Gaussian efficiency is 67%. With
    * beta=0.1, its efficiency is 85% but its breakdown is only 0.1.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \b References
    *
    * Rand R. Wilcox (2017), <em>Introduction to Robust Estimation and
    * Hypothesis Testing, 4th Edition</em>, Elsevier Inc., Section 3.12.3.
    */
   double BendMidvariance( double center, double beta = 0.2 ) const
   {
      return pcl::BendMidvariance( m_data->Begin(), m_data->End(), center, beta );
   }

   /*!
    * Returns a percentage bend midvariance (PBMV) with respect to the median.
    *
    * \param beta    Rejection parameter in the [0,0.5] range. Higher values
    *                improve robustness to outliers (i.e., increase the
    *                breakdown point of the estimator) at the expense of lower
    *                efficiency. The default value is beta=0.2.
    *
    * The square root of the percentage bend midvariance is a robust estimator
    * of scale. With the default beta=0.2, its Gaussian efficiency is 67%. With
    * beta=0.1, its efficiency is 85% but its breakdown is only 0.1.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \b References
    *
    * Rand R. Wilcox (2012), <em>Introduction to Robust Estimation and
    * Hypothesis Testing, 3rd Edition</em>, Elsevier Inc., Section 3.12.3.
    */
   double BendMidvariance( double beta = 0.2 ) const
   {
      return pcl::BendMidvariance( m_data->Begin(), m_data->End(), Median(), beta );
   }

   /*!
    * Returns the Sn scale estimator of Rousseeuw and Croux:
    *
    * <pre>
    * Sn = c * low_median( high_median( |x_i - x_j| ) )
    * </pre>
    *
    * where low_median() is the order statistic of rank (n + 1)/2, and
    * high_median() is the order statistic of rank n/2 + 1.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * The constant c = 1.1926 must be used to make the Sn estimator converge to
    * the standard deviation of a pure normal distribution. However, this
    * implementation does not apply it (it uses c=1 implicitly), for
    * consistency with other implementations of scale estimators.
    *
    * \b References
    *
    * P.J. Rousseeuw and C. Croux (1993), <em>Alternatives to the Median
    * Absolute Deviation,</em> Journal of the American Statistical Association,
    * Vol. 88, pp. 1273-1283.
    */
   double Sn() const
   {
      GenericMatrix A( *this );
      return pcl::Sn( A.Begin(), A.End() );
   }

   /*!
    * Returns the Qn scale estimator of Rousseeuw and Croux:
    *
    * <pre>
    * Qn = c * first_quartile( |x_i - x_j| : i < j )
    * </pre>
    *
    * where first_quartile() is the order statistic of rank (n + 1)/4.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * The constant c = 2.2219 must be used to make the Qn estimator converge to
    * the standard deviation of a pure normal distribution. However, this
    * implementation does not apply it (it uses c=1 implicitly), for
    * consistency with other implementations of scale estimators.
    *
    * \b References
    *
    * P.J. Rousseeuw and C. Croux (1993), <em>Alternatives to the Median Absolute
    * Deviation,</em> Journal of the American Statistical Association, Vol. 88,
    * pp. 1273-1283.
    */
   double Qn() const
   {
      GenericMatrix A( *this );
      return pcl::Qn( A.Begin(), A.End() );
   }

#endif   // !__PCL_NO_MATRIX_STATISTICS

   /*!
    * Returns a 64-bit non-cryptographic hash value computed for this matrix.
    *
    * This function calls pcl::Hash64() for the internal matrix data.
    *
    * The \a seed parameter can be used to generate repeatable hash values. It
    * can also be set to a random value in compromised environments.
    */
   uint64 Hash64( uint64 seed = 0 ) const noexcept
   {
      return pcl::Hash64( m_data->Begin(), m_data->Size(), seed );
   }

   /*!
    * Returns a 32-bit non-cryptographic hash value computed for this matrix.
    *
    * This function calls pcl::Hash32() for the internal matrix data.
    *
    * The \a seed parameter can be used to generate repeatable hash values. It
    * can also be set to a random value in compromised environments.
    */
   uint32 Hash32( uint32 seed = 0 ) const noexcept
   {
      return pcl::Hash32( m_data->Begin(), m_data->Size(), seed );
   }

   /*!
    * Returns a non-cryptographic hash value computed for this matrix. This
    * function is a synonym for Hash64().
    */
   uint64 Hash( uint64 seed = 0 ) const noexcept
   {
      return Hash64( seed );
   }

   /*!
    * Generates a sequence of string tokens separated with the specified
    * \a separator string. Returns a reference to the target string \a s.
    *
    * For each matrix element, this function appends a string representation
    * (known as a \e token) to the target string \a s. If the matrix contains
    * more than one element, successive tokens are separated with the specified
    * \a separator.
    *
    * The string type S must have a meaningful %Append() member function and
    * type conversion semantics to transform a matrix element to a string. The
    * standard String and IsoString PCL classes provide the required
    * functionality for most scalar types, although it is probably better to
    * use String::ToSeparated() and IsoString::ToSeparated() instead of calling
    * these functions directly.
    */
   template <class S, typename SP>
   S& ToSeparated( S& s, SP separator ) const
   {
      const_block_iterator i = m_data->Begin(), j = m_data->End();
      if ( i < j )
      {
         s.Append( S( *i ) );
         if ( ++i < j )
            do
            {
               s.Append( separator );
               s.Append( S( *i ) );
            }
            while ( ++i < j );
      }
      return s;
   }

   /*!
    * Generates a sequence of string tokens separated with the specified
    * \a separator string by calling an \a append function. Returns a reference
    * to the target string \a s.
    *
    * For each matrix element x, this function appends a string representation
    * (known as a \e token) to the target string \a s by calling the \a append
    * function:
    *
    *\code append( s, S( x ) ); \endcode
    *
    * If the matrix has more than one element, successive tokens are separated
    * by calling:
    *
    * \code append( s, S( separator ) ); \endcode
    *
    * The string type S must have type conversion semantics to transform a
    * matrix element to a string. The standard String and IsoString PCL classes
    * provide the required functionality for most scalar types, although it is
    * probably better to use String::ToSeparated() and IsoString::ToSeparated()
    * instead of calling these functions directly.
    */
   template <class S, typename SP, class AF>
   S& ToSeparated( S& s, SP separator, AF append ) const
   {
      const_block_iterator i = m_data->Begin(), j = m_data->End();
      if ( i < j )
      {
         append( s, S( *i ) );
         if ( ++i < j )
         {
            S p( separator );
            do
            {
               append( s, p );
               append( s, S( *i ) );
            }
            while ( ++i < j );
         }
      }
      return s;
   }

   /*!
    * Generates a comma-separated sequence of string tokens. Returns a
    * reference to the target string \a s.
    *
    * This function is equivalent to:
    *
    * \code ToSeparated( s, ',' ); \endcode
    */
   template <class S>
   S& ToCommaSeparated( S& s ) const
   {
      return ToSeparated( s, ',' );
   }

   /*!
    * Generates a space-separated sequence of string tokens. Returns a
    * reference to the target string \a s.
    *
    * This function is equivalent to:
    *
    * \code ToSeparated( s, ' ' ); \endcode
    */
   template <class S>
   S& ToSpaceSeparated( S& s ) const
   {
      return ToSeparated( s, ' ' );
   }

   /*!
    * Generates a tabulator-separated sequence of string tokens. Returns a
    * reference to the target string \a s.
    *
    * This function is equivalent to:
    *
    * \code ToSeparated( s, '\t' ); \endcode
    */
   template <class S>
   S& ToTabSeparated( S& s ) const
   {
      return ToSeparated( s, '\t' );
   }

#ifndef __PCL_NO_MATRIX_PHASE_MATRICES

   /*!
    * Returns the phase correlation matrix for two matrices \a A and \a B.
    * \ingroup phase_matrices
    */
   static GenericMatrix PhaseCorrelationMatrix( const GenericMatrix& A, const GenericMatrix& B )
   {
      if ( B.Rows() != A.Rows() || B.Cols() != A.Cols() )
         throw Error( "Invalid matrix dimensions in PhaseCorrelationMatrix()" );
      GenericMatrix R( A.Rows(), A.Cols() );
      PhaseCorrelationMatrix( R.Begin(), R.End(), A.Begin(), B.Begin() );
      return R;
   }

   /*!
    * Returns the cross power spectrum matrix for two matrices \a A and \a B.
    * \ingroup phase_matrices
    */
   static GenericMatrix CrossPowerSpectrumMatrix( const GenericMatrix& A, const GenericMatrix& B )
   {
      if ( B.Rows() != A.Rows() || B.Cols() != A.Cols() )
         throw Error( "Invalid matrix dimensions in CrossPowerSpectrumMatrix()" );
      GenericMatrix R( A.Rows(), A.Cols() );
      CrossPowerSpectrumMatrix( R.Begin(), R.End(), A.Begin(), B.Begin() );
      return R;
   }

#endif   // !__PCL_NO_MATRIX_PHASE_MATRICES

#ifndef __PCL_NO_MATRIX_IMAGE_RENDERING

   /*!
    * Renders this matrix as an image.
    *
    * The contents of the specified \a image will be replaced with a grayscale
    * rendition of this matrix, where the value of each pixel sample will be
    * proportional to its corresponding matrix component counterpart.
    *
    * Note that if this matrix has out-of-range values for the pixel sample
    * type of the target image, pixel saturation will occur at either the white
    * or black points, or both. This only happens when the target image is of
    * an integer type and this matrix contains floating point values outside
    * the normalized [0,1] range.
    *
    * If the target image is of a floating-point type (either real or complex)
    * and this matrix also stores floating point values, the output image may
    * require a rescaling or normalization operation to constrain all pixel
    * values to the normalized [0,1] range after calling this member function.
    */
   template <class P>
   void ToImage( GenericImage<P>& image ) const
   {
      image.AllocateData( Cols(), Rows() );
      typename P::sample*  __restrict__ v = *image;
      const_block_iterator __restrict__ i = m_data->Begin();
      const_block_iterator __restrict__ j = m_data->End();
      PCL_IVDEP
      for ( ; i < j; ++i, ++v )
         *v = P::ToSample( *i );
   }

   /*!
    * Renders this matrix as an image.
    *
    * The contents of the image transported by the specified %ImageVariant
    * object will be replaced with a grayscale rendition of this matrix, where
    * the value of each pixel sample will be proportional to its corresponding
    * matrix component counterpart.
    *
    * If the specified %ImageVariant does not transport an image, a new one
    * will be created in 32-bit floating point format.
    *
    * Also take into account the information given for
    * ToImage( GenericImage& ), relative to out-of-range values, which is
    * entirely applicable to this member function.
    */
   void ToImage( ImageVariant& image ) const
   {
      if ( !image )
         image.CreateFloatImage();

      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: ToImage( static_cast<Image&>( *image ) ); break;
         case 64: ToImage( static_cast<DImage&>( *image ) ); break;
         }
      else if ( image.IsComplexSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: ToImage( static_cast<ComplexImage&>( *image ) ); break;
         case 64: ToImage( static_cast<DComplexImage&>( *image ) ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: ToImage( static_cast<UInt8Image&>( *image ) ); break;
         case 16: ToImage( static_cast<UInt16Image&>( *image ) ); break;
         case 32: ToImage( static_cast<UInt32Image&>( *image ) ); break;
         }
   }

#endif   // !__PCL_NO_MATRIX_IMAGE_RENDERING

#ifndef __PCL_NO_MATRIX_IMAGE_CONVERSION

   /*!
    * Creates a matrix from image pixel samples.
    *
    * This static member function converts pixel sample values to matrix
    * elements for a rectangular region of a channel of the specified \a image.
    *
    * \param image   Image from which matrix elements will be generated.
    *
    * \param rect    A rectangular region in image pixel coordinates, from
    *                which the returned matrix will be generated. If this
    *                parameter is not specified, or if an empty rectangle is
    *                specified, this function will perform the matrix
    *                conversion for the current rectangular selection in the
    *                image, that is, for image.SelectedRectangle().
    *
    * \param channel Channel index. Must be the zero-based index of an existing
    *                channel in the image, or an integer < 0. If this parameter
    *                is not specified, or if a negative integer is specified,
    *                this function will perform the matrix conversion for the
    *                currently selected channel in the image, that is, for
    *                image.SelectedChannel().
    *
    * The returned matrix is a representation of the intersection of the
    * specified (or implicitly selected) rectangular region with the image for
    * the scalar data type T of this matrix instantiation. All predefined pixel
    * sample types are supported, including integer, real and complex pixels.
    *
    * If there is no intersection between the rectangular region and the image,
    * or if an invalid channel index is specified, this function returns an
    * empty matrix.
    *
    * Note that if the source image is of a floating point type (either real or
    * complex) and the scalar type T of this matrix instantiation is of an
    * integer type, out-of-range matrix elements will be truncated to either
    * zero or to the maximum integer value, if the corresponding source pixel
    * sample is outside the normalized [0,1] range.
    */
   template <class P>
   static GenericMatrix FromImage( const GenericImage<P>& image, const Rect& rect = Rect( 0 ), int channel = -1 )
   {
      Rect r = rect;
      if ( !image.ParseSelection( r, channel ) )
         return GenericMatrix();

      if ( r == image.Bounds() )
      {
         GenericMatrix M( image.Height(), image.Width() );
         const typename P::sample* __restrict__ v = image[channel];
               block_iterator      __restrict__ i = M.m_data->Begin();
         const_block_iterator      __restrict__ j = M.m_data->End();
         PCL_IVDEP
         for ( ; i < j; ++i, ++v )
            P::FromSample( *i, *v );
         return M;
      }
      else
      {
         int w = r.Width();
         int h = r.Height();
         GenericMatrix M( h, w );
         block_iterator __restrict__ m = M.m_data->Begin();
         PCL_IVDEP
         for ( int i = r.y0; i < r.y1; ++i )
         {
            const typename P::sample* __restrict__ v = image.PixelAddress( r.x0, i, channel );
            PCL_IVDEP
            for ( int j = 0; j < w; ++j )
               P::FromSample( *m++, *v++ );
         }
         return M;
      }
   }

   /*!
    * Creates a matrix from image pixel samples.
    *
    * This static member function converts pixel sample values to matrix
    * elements for a rectangular region of a channel of the image transported
    * by the specified %ImageVariant.
    *
    * If the specified %ImageVariant object does not transport an image, this
    * member function returns an empty matrix.
    *
    * Refer to the documentation for
    * FromImage( const GenericImage&, const Rect&, int ) for more information
    * about the conversion performed by this member function.
    */
   static GenericMatrix FromImage( const ImageVariant& image, const Rect& rect = Rect( 0 ), int channel = -1 )
   {
      if ( image )
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: return FromImage( static_cast<const Image&>( *image ), rect, channel );
            case 64: return FromImage( static_cast<const DImage&>( *image ), rect, channel );
            }
         else if ( image.IsComplexSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: return FromImage( static_cast<const ComplexImage&>( *image ), rect, channel );
            case 64: return FromImage( static_cast<const DComplexImage&>( *image ), rect, channel );
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: return FromImage( static_cast<const UInt8Image&>( *image ), rect, channel );
            case 16: return FromImage( static_cast<const UInt16Image&>( *image ), rect, channel );
            case 32: return FromImage( static_cast<const UInt32Image&>( *image ), rect, channel );
            }

      return GenericMatrix();
   }

#endif   // !__PCL_NO_MATRIX_IMAGE_CONVERSION

private:

   /*!
    * \struct Data
    * \internal
    * Reference-counted matrix data structure.
    */
   struct Data : public ReferenceCounter
   {
      int             n = 0;       //!< Number of matrix rows
      int             m = 0;       //!< Number of matrix columns
      block_iterator* v = nullptr; //!< Pointer to a pointer to a contiguous block of matrix elements

      Data() = default;

      Data( int rows, int cols )
      {
         if ( rows > 0 && cols > 0 )
            Allocate( rows, cols );
      }

      ~Data()
      {
         Deallocate();
      }

      int Rows() const noexcept
      {
         return n;
      }

      int Cols() const noexcept
      {
         return m;
      }

      size_type NumberOfElements() const noexcept
      {
         return size_type( n )*size_type( m );
      }

      size_type Size() const noexcept
      {
         return NumberOfElements()*sizeof( element );
      }

      block_iterator Begin() const
      {
         block_iterator p = (v != nullptr) ? *v : nullptr;
         if ( likely( std::is_scalar<element>::value ) )
            return reinterpret_cast<block_iterator>( PCL_ASSUME_ALIGNED_32( p ) );
         return p;
      }

      block_iterator End() const noexcept
      {
         return (v != nullptr) ? *v + NumberOfElements() : nullptr;
      }

      element& Element( int i, int j ) const noexcept
      {
         return v[i][j];
      }

      void Allocate( int rows, int cols )
      {
         n = rows;
         m = cols;
         v = new block_iterator[ n ];
         if ( likely( std::is_scalar<element>::value ) )
         {
            *v = reinterpret_cast<block_iterator>( PCL_ALIGNED_MALLOC( Size(), 32 ) );
            if ( unlikely( *v == nullptr ) )
            {
               delete [] v;
               v = nullptr;
               n = m = 0;
               throw std::bad_alloc();
            }
         }
         else
            *v = new element[ NumberOfElements() ];
         for ( int i = 1; i < n; ++i )
            v[i] = v[i-1] + m;
      }

      void Deallocate()
      {
         PCL_PRECONDITION( refCount == 0 )
         if ( v != nullptr )
         {
            if ( likely( std::is_scalar<element>::value ) )
               PCL_ALIGNED_FREE( *v );
            else
               delete [] *v;
            delete [] v;
            v = nullptr;
            n = m = 0;
         }
      }
   };

   /*!
    * \internal
    * The reference-counted matrix data.
    */
   Data* m_data = nullptr;

   /*!
    * \internal
    * Dereferences matrix data and disposes it if it becomes garbage.
    */
   void DetachFromData()
   {
      if ( !m_data->Detach() )
         delete m_data;
   }

   /*!
    * \internal
    * Inverts a small matrix of dimension <= 3.
    */
   static bool Invert( block_iterator* Ai, const_block_iterator const* A, int n ) noexcept
   {
      switch ( n )
      {
      case 1:
         if ( 1 + **A == 1 )
            return false;
         **Ai = 1/(**A);
         break;
      case 2:
         {
            const_block_iterator __restrict__ A0 = A[0];
            const_block_iterator __restrict__ A1 = A[1];
            element d = A0[0]*A1[1] - A0[1]*A1[0];
            if ( 1 + d == 1 )
               return false;
            Ai[0][0] =  A1[1]/d;
            Ai[0][1] = -A0[1]/d;
            Ai[1][0] = -A1[0]/d;
            Ai[1][1] =  A0[0]/d;
         }
         break;
      case 3:
         {
            const_block_iterator __restrict__ A0 = A[0];
            const_block_iterator __restrict__ A1 = A[1];
            const_block_iterator __restrict__ A2 = A[2];
            element d1 = A1[1]*A2[2] - A1[2]*A2[1];
            element d2 = A1[2]*A2[0] - A1[0]*A2[2];
            element d3 = A1[0]*A2[1] - A1[1]*A2[0];
            element d  = A0[0]*d1 + A0[1]*d2 + A0[2]*d3;
            if ( 1 + d == 1 )
               return false;
            Ai[0][0] = d1/d;
            Ai[0][1] = (A2[1]*A0[2] - A2[2]*A0[1])/d;
            Ai[0][2] = (A0[1]*A1[2] - A0[2]*A1[1])/d;
            Ai[1][0] = d2/d;
            Ai[1][1] = (A2[2]*A0[0] - A2[0]*A0[2])/d;
            Ai[1][2] = (A0[2]*A1[0] - A0[0]*A1[2])/d;
            Ai[2][0] = d3/d;
            Ai[2][1] = (A2[0]*A0[1] - A2[1]*A0[0])/d;
            Ai[2][2] = (A0[0]*A1[1] - A0[1]*A1[0])/d;
         }
         break;
      default: // ?!
         return false;
      }
      return true;
   }
};

// ### N.B.: Visual C++ is unable to compile the next two member functions if
//           the following external function declarations are placed within the
//           member function bodies. This forces us to implement them after the
//           GenericMatrix<> class declaration.

void PCL_FUNC InPlaceGaussJordan( GenericMatrix<double>&, GenericMatrix<double>& );
void PCL_FUNC InPlaceGaussJordan( GenericMatrix<float>&, GenericMatrix<float>& );

template <typename T> inline
GenericMatrix<T> GenericMatrix<T>::Inverse() const
{
   if ( Rows() != Cols() || Rows() == 0 )
      throw Error( "Invalid matrix inversion: Non-square or empty matrix." );

   /*
    * - Use direct formulae to invert 1x1, 2x2 and 3x3 matrices.
    * - Use Gauss-Jordan elimination to invert 4x4 and larger matrices.
    */
   if ( Rows() < 4 )
   {
      GenericMatrix Ai( Rows(), Rows() );
      if ( !Invert( Ai.m_data->v, m_data->v, Rows() ) )
         throw Error( "Invalid matrix inversion: Singular matrix." );
      return Ai;
   }
   else
   {
      GenericMatrix Ai( *this );
      GenericMatrix B = UnitMatrix( Rows() );
      InPlaceGaussJordan( Ai, B );
      return Ai;
   }
}

template <typename T> inline
void GenericMatrix<T>::Invert()
{
   if ( Rows() <= 3 )
      Transfer( Inverse() );
   else
   {
      if ( Rows() != Cols() || Rows() == 0 )
         throw Error( "Invalid matrix inversion: Non-square or empty matrix." );
      EnsureUnique();
      GenericMatrix B = UnitMatrix( Rows() );
      InPlaceGaussJordan( *this, B );
   }
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup matrix_operators Matrix Operators
 *
 * This section includes scalar-to-matrix and matrix-to-matrix arithmetic
 * operator functions that are not members of the GenericMatrix template class.
 */

/*!
 * Returns the sum of two matrices \a A and \a B.
 *
 * If the specified matrices are incompatible for matrix addition (because
 * their dimensions are different), this function throws an Error exception.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator +( const GenericMatrix<T>& A, const GenericMatrix<T>& B )
{
   if ( B.Rows() != A.Rows() || B.Cols() != A.Cols() )
      throw Error( "Invalid matrix addition." );
   GenericMatrix<T> R( A.Rows(), A.Cols() );
   typename       GenericMatrix<T>::block_iterator __restrict__ i = R.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ j = R.End();
   typename GenericMatrix<T>::const_block_iterator __restrict__ k = A.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ t = B.Begin();
   PCL_IVDEP
   for ( ; i < j; ++i, ++k, ++t )
      *i = *k + *t;
   return R;
}

/*!
 * Returns the sum of a matrix \a A and a scalar \a x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator +( const GenericMatrix<T>& A, const T& x )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
   typename       GenericMatrix<T>::block_iterator __restrict__ i = R.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ j = R.End();
   typename GenericMatrix<T>::const_block_iterator __restrict__ k = A.Begin();
   PCL_IVDEP
   for ( ; i < j; ++i, ++k )
      *i = *k + x;
   return R;
}

/*!
 * Returns the sum of a scalar \a x and a matrix \a A.
 *
 * This function exists to implement the commutative property of
 * scalar-to-matrix addition. It is equivalent to A + x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator +( const T& x, const GenericMatrix<T>& A )
{
   return A + x;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the result of subtracting a matrix \a B from another matrix \a A.
 *
 * If the specified matrices are incompatible for matrix subtraction (because
 * their dimensions are different), this function throws an Error exception.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator -( const GenericMatrix<T>& A, const GenericMatrix<T>& B )
{
   if ( B.Rows() != A.Rows() || B.Cols() != A.Cols() )
      throw Error( "Invalid matrix subtraction." );
   GenericMatrix<T> R( A.Rows(), A.Cols() );
   typename       GenericMatrix<T>::block_iterator __restrict__ i = R.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ j = R.End();
   typename GenericMatrix<T>::const_block_iterator __restrict__ k = A.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ t = B.Begin();
   PCL_IVDEP
   for ( ; i < j; ++i, ++k, ++t )
      *i = *k - *t;
   return R;
}

/*!
 * Returns the subtraction of a scalar \a x from a matrix \a A.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator -( const GenericMatrix<T>& A, const T& x )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
   typename       GenericMatrix<T>::block_iterator __restrict__ i = R.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ j = R.End();
   typename GenericMatrix<T>::const_block_iterator __restrict__ k = A.Begin();
   PCL_IVDEP
   for ( ; i < j; ++i, ++k )
      *i = *k - x;
   return R;
}

/*!
 * Returns the subtraction of a matrix \a A from a scalar \a x.
 *
 * This function exists because scalar-to-matrix subtraction is not a
 * commutative operation. A - x is not equal to x - A (the resulting matrix
 * elements have the same magnitudes but opposite signs).
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator -( const T& x, const GenericMatrix<T>& A )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
   typename       GenericMatrix<T>::block_iterator __restrict__ i = R.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ j = R.End();
   typename GenericMatrix<T>::const_block_iterator __restrict__ k = A.Begin();
   PCL_IVDEP
   for ( ; i < j; ++i, ++k )
      *i = x - *k;
   return R;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the product of two matrices \a A and \a B.
 *
 * If the specified matrices are incompatible for matrix multiplication
 * (because the number of rows in \a B is not equal to the number of columns in
 * \a A), this function throws an appropriate Error exception.
 *
 * Bear in mind that matrix multiplication has O(N^3) complexity. This is
 * relevant to multiply large matrices.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator *( const GenericMatrix<T>& A, const GenericMatrix<T>& B )
{
   int n = A.Rows();
   int m = B.Cols();
   int p = A.Cols();
   if ( B.Rows() != p )
      throw Error( "Invalid matrix multiplication." );
   GenericMatrix<T> R( n, m );
   for ( int i = 0; i < n; ++i )
      for ( int j = 0; j < m; ++j )
      {
         T rij = 0;
         for ( int k = 0; k < p; ++k )
            rij += A[i][k] * B[k][j];
         R[i][j] = rij;
      }
   return R;
}

/*!
 * Returns the product of a matrix \a A by a vector \a x.
 *
 * If the specified objects are incompatible for multiplication (because the
 * number of components in \a x is not equal to the number of columns in \a A),
 * this function throws the appropriate Error exception.
 *
 * The result of the product of a matrix with \e m rows and \e n columns by a
 * vector of \e n components is a vector of \e m components.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericVector<T> operator *( const GenericMatrix<T>& A, const GenericVector<T>& x )
{
   int n = A.Cols();
   int m = A.Rows();
   if ( x.Length() != n )
      throw Error( "Invalid matrix-vector multiplication." );
   GenericVector<T> r( m );
   for ( int i = 0; i < m; ++i )
   {
      T ri = 0;
      for ( int j = 0; j < n; ++j )
         ri += A[i][j] * x[j];
      r[i] = ri;
   }
   return r;
}

/*!
 * Returns the product of a matrix \a A and a scalar \a x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator *( const GenericMatrix<T>& A, const T& x )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
   typename       GenericMatrix<T>::block_iterator __restrict__ i = R.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ j = R.End();
   typename GenericMatrix<T>::const_block_iterator __restrict__ k = A.Begin();
   PCL_IVDEP
   for ( ; i < j; ++i, ++k )
      *i = *k * x;
   return R;
}

/*!
 * Returns the product of a scalar \a x and a matrix \a A.
 *
 * This function exists to implement the commutative property of
 * scalar-to-matrix multiplication. It is equivalent to A * x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator *( const T& x, const GenericMatrix<T>& A )
{
   return A * x;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the result of dividing a matrix \a A by a scalar \a x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator /( const GenericMatrix<T>& A, const T& x )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
   typename       GenericMatrix<T>::block_iterator __restrict__ i = R.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ j = R.End();
   typename GenericMatrix<T>::const_block_iterator __restrict__ k = A.Begin();
   PCL_IVDEP
   for ( ; i < j; ++i, ++k )
      *i = *k / x;
   return R;
}

/*!
 * Returns the result of dividing a scalar \a x by a matrix \a A.
 *
 * This function exists because scalar-to-matrix division is not a
 * commutative operation. A/x is not equal to x/A.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator /( const T& x, const GenericMatrix<T>& A )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
   typename       GenericMatrix<T>::block_iterator __restrict__ i = R.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ j = R.End();
   typename GenericMatrix<T>::const_block_iterator __restrict__ k = A.Begin();
   PCL_IVDEP
   for ( ; i < j; ++i, ++k )
      *i = x / *k;
   return R;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the result of raising a matrix \a A to a scalar \a x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator ^( const GenericMatrix<T>& A, const T& x )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
   typename       GenericMatrix<T>::block_iterator __restrict__ i = R.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ j = R.End();
   typename GenericMatrix<T>::const_block_iterator __restrict__ k = A.Begin();
   PCL_IVDEP
   for ( ; i < j; ++i, ++k )
      *i = pcl::Pow( *k, x );
   return R;
}

/*!
 * Returns the result of raising a scalar \a x to a matrix \a A.
 *
 * This function exists because scalar-to-matrix exponentiation is not a
 * commutative operation. A^x is not equal to x^A.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator ^( const T& x, const GenericMatrix<T>& A )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
   typename       GenericMatrix<T>::block_iterator __restrict__ i = R.Begin();
   typename GenericMatrix<T>::const_block_iterator __restrict__ j = R.End();
   typename GenericMatrix<T>::const_block_iterator __restrict__ k = A.Begin();
   PCL_IVDEP
   for ( ; i < j; ++i, ++k )
      *i = pcl::Pow( x, *k );
   return R;
}

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_MATRIX_INSTANTIATE

/*!
 * \defgroup matrix_types Matrix Types
 */

/*!
 * \class pcl::I8Matrix
 * \ingroup matrix_types
 * \brief 8-bit signed integer matrix.
 *
 * %I8Matrix is a template instantiation of GenericMatrix for \c int8.
 */
using I8Matrix = GenericMatrix<int8>;

/*!
 * \class pcl::CharMatrix
 * \ingroup matrix_types
 * \brief 8-bit signed integer matrix.
 *
 * %CharMatrix is an alias for I8Matrix. It is a template instantiation of
 * GenericMatrix for \c int8.
 */
using CharMatrix = I8Matrix;

/*!
 * \class pcl::UI8Matrix
 * \ingroup matrix_types
 * \brief 8-bit unsigned integer matrix.
 *
 * %UI8Matrix is a template instantiation of GenericMatrix for \c uint8.
 */
using UI8Matrix = GenericMatrix<uint8>;

/*!
 * \class pcl::ByteMatrix
 * \ingroup matrix_types
 * \brief 8-bit unsigned integer matrix.
 *
 * %ByteMatrix is an alias for UI8Matrix. It is a template instantiation of
 * GenericMatrix for \c uint8.
 */
using ByteMatrix = UI8Matrix;

/*!
 * \class pcl::I16Matrix
 * \ingroup matrix_types
 * \brief 16-bit signed integer matrix.
 *
 * %I16Matrix is a template instantiation of GenericMatrix for \c int16.
 */
using I16Matrix = GenericMatrix<int16>;

/*!
 * \class pcl::UI16Matrix
 * \ingroup matrix_types
 * \brief 16-bit unsigned integer matrix.
 *
 * %UI16Matrix is a template instantiation of GenericMatrix for \c uint16.
 */
using UI16Matrix = GenericMatrix<uint16>;

/*!
 * \class pcl::I32Matrix
 * \ingroup matrix_types
 * \brief 32-bit signed integer matrix.
 *
 * %I32Matrix is a template instantiation of GenericMatrix for \c int32.
 */
using I32Matrix = GenericMatrix<int32>;

/*!
 * \class pcl::IMatrix
 * \ingroup matrix_types
 * \brief 32-bit signed integer matrix.
 *
 * %IMatrix is an alias for I32Matrix. It is a template instantiation of
 * GenericMatrix for \c int32.
 */
using IMatrix = I32Matrix;

/*!
 * \class pcl::UI32Matrix
 * \ingroup matrix_types
 * \brief 32-bit unsigned integer matrix.
 *
 * %UI32Matrix is a template instantiation of GenericMatrix for \c uint32.
 */
using UI32Matrix = GenericMatrix<uint32>;

/*!
 * \class pcl::UIMatrix
 * \ingroup matrix_types
 * \brief Unsigned integer matrix.
 *
 * %UIMatrix is an alias for UI32Matrix. It is a template instantiation of
 * GenericMatrix for \c uint32.
 */
using UIMatrix = UI32Matrix;

/*!
 * \class pcl::I64Matrix
 * \ingroup matrix_types
 * \brief 64-bit integer matrix.
 *
 * %I64Matrix is a template instantiation of GenericMatrix for \c int64.
 */
using I64Matrix = GenericMatrix<int64>;

/*!
 * \class pcl::UI64Matrix
 * \ingroup matrix_types
 * \brief 64-bit unsigned integer matrix.
 *
 * %UI64Matrix is a template instantiation of GenericMatrix for \c uint64.
 */
using UI64Matrix = GenericMatrix<uint64>;

/*!
 * \class pcl::F32Matrix
 * \ingroup matrix_types
 * \brief 32-bit floating point real matrix.
 *
 * %F32Matrix is a template instantiation of GenericMatrix for \c float.
 */
using F32Matrix = GenericMatrix<float>;

/*!
 * \class pcl::FMatrix
 * \ingroup matrix_types
 * \brief 32-bit floating point real matrix.
 *
 * %FMatrix is an alias for F32Matrix. It is a template instantiation of
 * GenericMatrix for \c float.
 */
using FMatrix = F32Matrix;

/*!
 * \class pcl::F64Matrix
 * \ingroup matrix_types
 * \brief 64-bit floating point real matrix.
 *
 * %F64Matrix is a template instantiation of GenericMatrix for \c double.
 */
using F64Matrix = GenericMatrix<double>;

/*!
 * \class pcl::DMatrix
 * \ingroup matrix_types
 * \brief 64-bit floating point real matrix.
 *
 * %DMatrix is an alias for F64Matrix. It is a template instantiation of
 * GenericMatrix for \c double.
 */
using DMatrix = F64Matrix;

/*!
 * \class pcl::Matrix
 * \ingroup matrix_types
 * \brief 64-bit floating point real matrix.
 *
 * %Matrix is an alias for DMatrix. It is a template instantiation of
 * GenericMatrix for the \c double type.
 */
using Matrix = DMatrix;

/*!
 * \class pcl::C32Matrix
 * \ingroup matrix_types
 * \brief 32-bit floating point complex matrix.
 *
 * %C32Matrix is a template instantiation of GenericMatrix for \c Complex32.
 */
using C32Matrix = GenericMatrix<Complex32>;

/*!
 * \class pcl::C64Matrix
 * \ingroup matrix_types
 * \brief 64-bit floating point complex matrix.
 *
 * %C64Matrix is a template instantiation of GenericMatrix for \c Complex64.
 */
using C64Matrix = GenericMatrix<Complex64>;

#ifndef _MSC_VER

/*!
 * \class pcl::F80Matrix
 * \ingroup matrix_types
 * \brief 80-bit extended precision floating point real matrix.
 *
 * %F80Matrix is a template instantiation of GenericMatrix for \c long
 * \c double.
 *
 * \note This template instantiation is not available on Windows with Visual
 * C++ compilers.
 */
using F80Matrix = GenericMatrix<long double>;

/*!
 * \class pcl::LDMatrix
 * \ingroup matrix_types
 * \brief 80-bit extended precision floating point real matrix.
 *
 * %LDMatrix is an alias for F80Matrix. It is a template instantiation of
 * GenericMatrix for \c long \c double.
 *
 * \note This template instantiation is not available on Windows with Visual
 * C++ compilers.
 */
using LDMatrix = F80Matrix;

#endif   // !_MSC_VER

#endif   // !__PCL_NO_MATRIX_INSTANTIATE

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Matrix_h

// ----------------------------------------------------------------------------
// EOF pcl/Matrix.h - Released 2024-12-28T16:53:48Z
