//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Algebra.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_Algebra_h
#define __PCL_Algebra_h

/// \file pcl/Algebra.h

#include <pcl/Defs.h>

#include <pcl/Matrix.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup in_place_gauss_jordan In-Place Gauss-Jordan Solvers
 */

/*!
 * Solution to the linear system A*X = B
 *
 * On output, A is replaced by its inverse matrix and B is the set of solution
 * vectors X.
 *
 * This is an overloaded function for the Matrix type, which is a template
 * instantiation of GenericMatrix for double.
 *
 * \ingroup in_place_gauss_jordan
 */
void PCL_FUNC InPlaceGaussJordan( Matrix& A, Matrix& B );

/*!
 * Solution to the linear system A*X = B
 *
 * On output, A is replaced by its inverse matrix and B is the set of solution
 * vectors X.
 *
 * This is an overloaded function for the FMatrix type, which is a template
 * instantiation of GenericMatrix for float.
 *
 * \ingroup in_place_gauss_jordan
 */
void PCL_FUNC InPlaceGaussJordan( FMatrix& A, FMatrix& B );

/*!
 * \class GenericGaussJordan
 * \brief Generic Gauss-Jordan linear system solver.
 */
template <typename T>
class PCL_CLASS GenericGaussJordan
{
public:

   /*!
    * Represents a matrix involved in the solution of a linear system.
    */
   using matrix = GenericMatrix<T>;

   /*!
    * Represents a matrix element.
    */
   using matrix_element = typename matrix::element;

   /*!
    * Inverse matrix
    */
   matrix Ai;  // Inverse matrix

   /*!
    * Solution vectors
    */
   matrix X;   // Solution vectors

   /*!
    * Solution to the linear system A*X = B
    *
    * This constructor replaces the Ai member of this object with the inverse
    * matrix of \a A, and the X member with the set of solution vectors.
    */
   GenericGaussJordan( const matrix& A, const matrix& B )
   {
      Ai = A;
      X = B;
      InPlaceGaussJordan( Ai, X );
   }
};

/*!
 * \class GaussJordan
 * \brief Gauss-Jordan linear system solver for Matrix objects.
 *
 * %GaussJordan is a template instantiation of GenericGaussJordan for the
 * double type. %GaussJordan works with Matrix objects. %Matrix is a template
 * instantiation of GenericMatrix for double.
 */
class PCL_CLASS GaussJordan : public GenericGaussJordan<double>
{
public:

   /*!
    * Identifies the parent template class, which implements the underlying
    * algorithm for this class.
    */
   using algorithm_implementation = GenericGaussJordan<double>;

   /*!
    * Represents a matrix involved in the solution of a linear system.
    */
   using matrix = algorithm_implementation::matrix;

   /*!
    * Represents a matrix element.
    */
   using matrix_element = matrix::element;

   /*!
    * Solution to the linear system A*X = B
    *
    * This constructor replaces the Ai member of this object with the inverse
    * matrix of \a A, and the X member with the set of solution vectors.
    */
   GaussJordan( const Matrix& A, const Matrix& B )
      : algorithm_implementation( A, B )
   {
   }
};

/*!
 * \class FGaussJordan
 * \brief Gauss-Jordan linear system solver for FMatrix objects.
 *
 * %FGaussJordan is a template instantiation of GenericGaussJordan for the
 * float type. %FGaussJordan works with FMatrix objects. %FMatrix is a template
 * instantiation of GenericMatrix for float.
 */
class PCL_CLASS FGaussJordan : public GenericGaussJordan<float>
{
public:

   /*!
    * Identifies the parent template class, which implements the underlying
    * algorithm for this class.
    */
   using algorithm_implementation = GenericGaussJordan<float>;

   /*!
    * Represents a matrix involved in the solution of a linear system.
    */
   using matrix = algorithm_implementation::matrix;

   /*!
    * Represents a matrix element.
    */
   using matrix_element = matrix::element;

   /*!
    * Solution to the linear system A*X = B
    *
    * This constructor replaces the Ai member of this object with the inverse
    * matrix of \a A, and the X member with the set of solution vectors.
    */
   FGaussJordan( const FMatrix& A, const FMatrix& B )
      : algorithm_implementation( A, B )
   {
   }
};

// ----------------------------------------------------------------------------

void PCL_FUNC InPlaceSVDImplementation( Matrix&, Vector&, Matrix& );
void PCL_FUNC InPlaceSVDImplementation( FMatrix&, FVector&, FMatrix& );

/*!
 * \class GenericInPlaceSVD
 * \brief Generic in-place singular value decomposition algorithm.
 */
template <typename T>
class PCL_CLASS GenericInPlaceSVD
{
public:

   /*!
    * Represents a vector involved in a singular value decomposition.
    */
   using vector = GenericVector<T>;

   /*!
    * Represents a matrix involved in a singular value decomposition.
    */
   using matrix = GenericMatrix<T>;

   /*!
    * Represents a vector component.
    */
   using vector_component = typename vector::component;

   /*!
    * Represents a matrix element.
    */
   using matrix_element = typename matrix::element;

   /*!
    * The components of this vector are the m (positive) diagonal elements of
    * the matrix W in a singular value decomposition A=U*W*Vt. m is the number
    * of columns in the decomposed matrix A.
    */
   vector W;

   /*!
    * Each column of this m x m matrix is the eigenvector for the corresponding
    * element of W in a singular value decomposition A=U*W*Vt. m is the number
    * of columns in the decomposed matrix A.
    */
   matrix V;

   /*!
    * Singular Value Decomposition: A = U*W*Vt
    *
    * The dimensions of A are n rows and m columns. U is an n x m matrix. The m
    * components of W are the positive diagonal elements of W, and each column
    * of V (m x m) is the eigenvector for the corresponding element of W.
    *
    * On output, this constructor replaces the specified matrix \a A with the
    * matrix U that results from the SVD decomposition (indeed that's why this
    * is an in-place decomposition). W and V are stored in the corresponding
    * members of this object.
    */
   GenericInPlaceSVD( matrix& A )
      : W( A.Columns() )
      , V( A.Columns(), A.Columns() )
   {
      InPlaceSVDImplementation( A, W, V );
   }

   /*!
    * Returns the column index of the largest eigenvector in matrix V. This is
    * the index of the largest component of vector W.
    */
   int IndexOfLargestSingularValue() const
   {
      return W.IndexOfLargestComponent();
   }

   /*!
    * Returns the column index of the smallest eigenvector in matrix V. This is
    * the index of the smallest nonzero component of vector W.
    *
    * Before calling this function, you should edit the components of vector W
    * to set to zero all singular values below a suitable tolerance. For
    * example, using the machine epsilon:
    *
    * \code
    * Matrix A;
    * ...
    * InPlaceSVD svd( A );
    * for ( int i = 0; i < svd.W.Length(); ++i )
    *    if ( 1 + svd.W[i] == 1 )
    *       svd.W[i] = 0;
    * int i = svd.IndexOfSmallestSingularValue();
    * ...
    * \endcode
    */
   int IndexOfSmallestSingularValue() const
   {
      return W.IndexOfSmallestNonzeroComponent();
   }
};

/*!
 * \class InPlaceSVD
 * \brief In-place singular value decomposition algorithm for Matrix and Vector objects.
 *
 * %InPlaceSVD is a template instantiation of GenericInPlaceSVD for the double
 * type. %InPlaceSVD works with Matrix and Vector objects. %Matrix and %Vector
 * are template instantiations of GenericMatrix and GenericVector,
 * respectively, for the double type.
 */
class PCL_CLASS InPlaceSVD : public GenericInPlaceSVD<double>
{
public:

   /*!
    * Identifies the parent template class, which implements the underlying
    * algorithm for this class.
    */
   using algorithm_implementation = GenericInPlaceSVD<double>;

   /*!
    * Represents a vector involved in a singular value decomposition.
    */
   using vector = algorithm_implementation::vector;

   /*!
    * Represents a matrix involved in a singular value decomposition.
    */
   using matrix = algorithm_implementation::matrix;

   /*!
    * Represents a vector component.
    */
   using vector_component = vector::component;

   /*!
    * Represents a matrix element.
    */
   using matrix_element = matrix::element;

   /*!
    * Singular Value Decomposition: A = U*W*Vt
    *
    * The dimensions of A are n rows and m columns. U is an n x m matrix. The m
    * components of W are the positive diagonal elements of W, and each column
    * of V (m x m) is the eigenvector for the corresponding element of W.
    *
    * On output, this constructor replaces the specified matrix \a A with the
    * matrix U that results from the SVD decomposition (indeed that's why this
    * is an in-place decomposition). W and V are stored in the corresponding
    * members of this object.
    */
   InPlaceSVD( matrix& A )
      : algorithm_implementation( A )
   {
   }
};

/*!
 * \class FInPlaceSVD
 * \brief In-place singular value decomposition algorithm for FMatrix and FVector objects.
 *
 * %FInPlaceSVD is a template instantiation of GenericInPlaceSVD for the float
 * type. %FInPlaceSVD works with FMatrix and FVector objects. %FMatrix and
 * %FVector are template instantiations of GenericMatrix and GenericVector,
 * respectively, for the float type.
 */
class PCL_CLASS FInPlaceSVD : public GenericInPlaceSVD<float>
{
public:

   /*!
    * Identifies the parent template class, which implements the underlying
    * algorithm for this class.
    */
   using algorithm_implementation = GenericInPlaceSVD<float>;

   /*!
    * Represents a vector involved in a singular value decomposition.
    */
   using vector = algorithm_implementation::vector;

   /*!
    * Represents a matrix involved in a singular value decomposition.
    */
   using matrix = algorithm_implementation::matrix;

   /*!
    * Represents a vector component.
    */
   using vector_component = vector::component;

   /*!
    * Represents a matrix element.
    */
   using matrix_element = matrix::element;

   /*!
    * Singular Value Decomposition: A = U*W*Vt
    *
    * The dimensions of A are n rows and m columns. U is an n x m matrix. The m
    * components of W are the positive diagonal elements of W, and each column
    * of V (m x m) is the eigenvector for the corresponding element of W.
    *
    * On output, this constructor replaces the specified matrix \a A with the
    * matrix U that results from the SVD decomposition (indeed that's why this
    * is an in-place decomposition). W and V are stored in the corresponding
    * members of this object.
    */
   FInPlaceSVD( matrix& A )
      : algorithm_implementation( A )
   {
   }
};

/*!
 * \class GenericSVD
 * \brief Generic singular value decomposition algorithm.
 */
template <typename T>
class PCL_CLASS GenericSVD
{
public:

   /*!
    * Identifies the parent template class, which implements the underlying
    * algorithm for this class.
    */
   using algorithm_implementation = GenericInPlaceSVD<T>;

   /*!
    * Represents a vector involved in a singular value decomposition.
    */
   using vector = typename algorithm_implementation::vector;

   /*!
    * Represents a matrix involved in a singular value decomposition.
    */
   using matrix = typename algorithm_implementation::matrix;

   /*!
    * Represents a vector component.
    */
   using vector_component = typename vector::component;

   /*!
    * Represents a matrix element.
    */
   using matrix_element = typename matrix::element;

   /*!
    * This is the n x m matrix resulting from the singular value decomposition
    * A = U*W*Vt. n and m are the rows and columns of the decomposed matrix A.
    */
   matrix U;

   /*!
    * The components of this vector are the m (positive) diagonal elements of
    * the matrix W in a singular value decomposition A = U*W*Vt. m is the
    * number of columns in the decomposed matrix A.
    */
   vector W;

   /*!
    * Each column of this m x m matrix is the eigenvector for the corresponding
    * element of W in a singular value decomposition A = U*W*Vt. m is the
    * number of columns in the decomposed matrix A.
    */
   matrix V;

   /*!
    * Singular Value Decomposition: A = U*W*Vt
    *
    * The dimensions of A are n rows and m columns. U is an n x m matrix. The m
    * components of W are the positive diagonal elements of W, and each column
    * of V (m x m) is the eigenvector for the corresponding element of W.
    *
    * On output, this constructor stores U, W and V in the corresponding
    * members of this object.
    */
   GenericSVD( const matrix& A )
   {
      U = A;
      algorithm_implementation svd( U );
      W = svd.W;
      V = svd.V;
   }

   /*!
    * Returns the column index of the largest eigenvector in matrix V. This is
    * the index of the largest component of vector W.
    */
   int IndexOfLargestSingularValue() const
   {
      return W.IndexOfLargestComponent();
   }

   /*!
    * Returns the column index of the smallest eigenvector in matrix V. This is
    * the index of the smallest nonzero component of vector W.
    *
    * Before calling this function, you should edit the components of vector W
    * to set to zero all singular values below a suitable tolerance. For
    * example, using the machine epsilon:
    *
    * \code
    * Matrix A;
    * ...
    * SVD svd( A );
    * for ( int i = 0; i < svd.W.Length(); ++i )
    *    if ( 1 + svd.W[i] == 1 )
    *       svd.W[i] = 0;
    * int i = svd.IndexOfSmallestSingularValue();
    * ...
    * \endcode
    */
   int IndexOfSmallestSingularValue() const
   {
      return W.IndexOfSmallestNonzeroComponent();
   }
};

/*!
 * \class SVD
 * \brief Singular value decomposition algorithm for Matrix and Vector objects.
 *
 * %SVD is a template instantiation of GenericSVD for the double type. %SVD
 * works with Matrix and Vector objects. %Matrix and %Vector are template
 * instantiations of GenericMatrix and GenericVector, respectively, for the
 * double type.
 */
class PCL_CLASS SVD : public GenericSVD<double>
{
public:

   /*!
    * Identifies the parent template class, which implements the underlying
    * algorithm for this class.
    */
   using algorithm_implementation = GenericSVD<double>;

   /*!
    * Represents a vector involved in a singular value decomposition.
    */
   using vector = algorithm_implementation::vector;

   /*!
    * Represents a matrix involved in a singular value decomposition.
    */
   using matrix = algorithm_implementation::matrix;

   /*!
    * Represents a vector component.
    */
   using vector_component = vector::component;

   /*!
    * Represents a matrix element.
    */
   using matrix_element = matrix::element;

   /*!
    * Singular Value Decomposition: A = U*W*Vt
    *
    * The dimensions of A are n rows and m columns. U is an n x m matrix. The m
    * components of W are the positive diagonal elements of W, and each column
    * of V (m x m) is the eigenvector for the corresponding element of W.
    *
    * On output, this constructor stores U, W and V in the corresponding
    * members of this object.
    */
   SVD( const matrix& A )
      : algorithm_implementation( A )
   {
   }
};

/*!
 * \class FSVD
 * \brief Singular value decomposition algorithm for FMatrix and FVector objects.
 *
 * %FSVD is a template instantiation of GenericSVD for the float type. %FSVD
 * works with FMatrix and FVector objects. %FMatrix and %FVector are template
 * instantiations of GenericMatrix and GenericVector, respectively, for the
 * float type.
 */
class PCL_CLASS FSVD : public GenericSVD<float>
{
public:

   /*!
    * Identifies the parent template class, which implements the underlying
    * algorithm for this class.
    */
   using algorithm_implementation = GenericSVD<float>;

   /*!
    * Represents a vector involved in a singular value decomposition.
    */
   using vector = algorithm_implementation::vector;

   /*!
    * Represents a matrix involved in a singular value decomposition.
    */
   using matrix = algorithm_implementation::matrix;

   /*!
    * Represents a vector component.
    */
   using vector_component = vector::component;

   /*!
    * Represents a matrix element.
    */
   using matrix_element = matrix::element;

   /*!
    * Singular Value Decomposition: A = U*W*Vt
    *
    * The dimensions of A are n rows and m columns. U is an n x m matrix. The m
    * components of W are the positive diagonal elements of W, and each column
    * of V (m x m) is the eigenvector for the corresponding element of W.
    *
    * On output, this constructor stores U, W and V in the corresponding
    * members of this object.
    */
   FSVD( const matrix& A )
      : algorithm_implementation( A )
   {
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Algebra_h

// ----------------------------------------------------------------------------
// EOF pcl/Algebra.h - Released 2024-12-28T16:53:48Z
