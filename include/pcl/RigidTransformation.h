//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.3
// ----------------------------------------------------------------------------
// pcl/RigidTransformation.h - Released 2023-11-23T18:44:57Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_RigidTransformation_h
#define __PCL_RigidTransformation_h

/// \file pcl/RigidTransformation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Algebra.h>
#include <pcl/Array.h>
#include <pcl/Matrix.h>
#include <pcl/Point.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class RigidTransformation
 * \brief Rigid geometric transformation
 *
 * A two-dimensional rigid transformation, or Euclidean transformation,
 * preserves the Euclidean distance between every pair of transformed points.
 * Rigid transformations may include rotations, translations and reflections in
 * any order.
 */
class PCL_CLASS RigidTransformation
{
public:

   /*!
    * Default constructor. Constructs a no-op transformation with a unit
    * transformation matrix.
    */
   RigidTransformation()
      : m_H( Matrix::UnitMatrix( 3 ) )
   {
   }

   /*!
    * Constructor from a given transformation matrix.
    */
   RigidTransformation( const Matrix& H )
      : m_H( H )
   {
   }

   /*!
    * Constructor from two 2D point lists.
    *
    * Computes a rigid transformation to generate a list \a P2 of transformed
    * points from a list \a P1 of original points. In other words, the computed
    * transformation H works as follows:
    *
    * P2 = H( P1 )
    *
    * The transformation matrix is calculated by solving a least-squares
    * problem using the SVD method (see references). Both point lists must
    * contain at least three points.
    *
    * If one of the specified point lists contains less than three points, or
    * if no transformation can be estimated from the specified point lists
    * (which leads to a singular transformation matrix), this constructor
    * throws an appropriate Error exception.
    *
    * \b References
    *
    * Olga Sorkine-Hornung and Michael Rabinovich, <em>Least-Squares Rigid
    * Motion Using SVD.</em> Department of Computer Science, ETH Zurich.
    * January 16, 2017.
    *
    * Arun, K.S., Huang, T.S., Blostein, S.D, <em>Least-Squares Fitting of Two
    * 3-D Point Sets</em>, IEEE Transactions on Pattern Analysis and Machine
    * Intelligence, Volume 9 Issue 5, May 1987.
    */
   template <class point_list1, class point_list2>
   RigidTransformation( const point_list1& P1, const point_list2& P2 )
      : m_H( LS_SVD( P1, P2 ) )
   {
   }

   /*!
    * Copy constructor.
    */
   RigidTransformation( const RigidTransformation& ) = default;

   /*!
    * Copy assignment operator.
    */
   RigidTransformation& operator =( const RigidTransformation& ) = default;

   /*!
    * Move constructor.
    */
   RigidTransformation( RigidTransformation&& ) = default;

   /*!
    * Move assignment operator.
    */
   RigidTransformation& operator =( RigidTransformation&& ) = default;

   /*!
    * Coordinate transformation. Applies the transformation to the specified
    * \a x and \a y coordinates.
    *
    * The types T1 and T2 must be constructible from and convertible to
    * \c double.
    */
   template <typename T1, typename T2>
   void Apply( T1& x, T2& y ) const
   {
      double fx = double( x );
      double fy = double( y );
      x = T1( m_H[0][0]*fx + m_H[0][1]*fy + m_H[0][2] );
      y = T2( m_H[1][0]*fx + m_H[1][1]*fy + m_H[1][2] );
   }

   /*!
    * Coordinate transformation. Applies the transformation to the specified
    * point \a p. Returns a reference to \a p;
    *
    * The type T must be constructible from and convertible to \c double.
    */
   template <typename T>
   GenericPoint<T>& Apply( GenericPoint<T>& p ) const
   {
      Apply( p.x, p.y );
      return p;
   }

   /*!
    * Coordinate transformation operator. Applies the transformation to the
    * specified \a x and \a y coordinates. Returns the transformed point as a
    * two-dimensional point with real coordinates.
    */
   DPoint operator ()( double x, double y ) const
   {
      DPoint p( x, y );
      Apply( p.x, p.y );
      return p;
   }

   /*!
    * Point transformation operator. Applies the transformation to the
    * coordinates of the specified point \a p. Returns the transformed point as
    * a two-dimensional point with real coordinates.
    */
   template <typename T>
   DPoint operator ()( const GenericPoint<T>& p ) const
   {
      return operator ()( double( p.x ), double( p.y ) );
   }

   /*!
    * Returns the inverse of this transformation.
    *
    * If this transformation has been computed from two point lists \a P1 and
    * \a P2:
    *
    * P2 = H( P1 )
    *
    * then this function returns a transformation H1 such that:
    *
    * P1 = H1( P2 )
    */
   RigidTransformation Inverse() const
   {
      return RigidTransformation( m_H.Inverse() );
   }

   /*!
    * Returns the rigid transformation matrix.
    */
   const Matrix& TransformationMatrix() const
   {
      return m_H;
   }

   /*!
    * Returns the rigid transformation matrix.
    */
   operator const Matrix&() const
   {
      return TransformationMatrix();
   }

   /*!
    * Returns true iff this transformation has been initialized and is valid.
    */
   bool IsValid() const
   {
      return !m_H.IsEmpty();
   }

   /*!
    * Ensures that this object uniquely references its internal matrix data.
    */
   void EnsureUnique()
   {
      m_H.EnsureUnique();
   }

private:

   Matrix m_H;

   /*
    * Implementation of the SVD-based least squares method to compute an
    * optimal rigid transformation matrix.
    */
   template <class point_list1, class point_list2>
   static Matrix LS_SVD( const point_list1& P1, const point_list2& P2 )
   {
      int n = int( Min( P1.Length(), P2.Length() ) );
      if ( n < 3 )
         throw Error( "RigidTransformation::LS_SVD(): Less than three points specified." );

      /*
       * Centroid coordinates
       */
      DPoint p10( 0 );
      for ( int i = 0; i < n; ++i )
         p10 += P1[i];
      p10 /= n;
      DPoint p20( 0 );
      for ( int i = 0; i < n; ++i )
         p20 += P2[i];
      p20 /= n;

      /*
       * Covariance matrix
       */
      double xx = 0, xy = 0, yy = 0;
      for ( int i = 0; i < n; ++i )
      {
         double dx = P1[i].x - p10.x;
         double dy = P1[i].y - p10.y;
         xx += dx*dx;
         xy += dx*dy;
         yy += dy*dy;
      }
      int n1 = n - 1;
      xx /= n1;
      xy /= n1;
      yy /= n1;
      Matrix M1( xx, xy,
                 xy, yy );
      xx = 0, xy = 0, yy = 0;
      for ( int i = 0; i < n; ++i )
      {
         double dx = P2[i].x - p20.x;
         double dy = P2[i].y - p20.y;
         xx += dx*dx;
         xy += dx*dy;
         yy += dy*dy;
      }
      xx /= n1;
      xy /= n1;
      yy /= n1;
      Matrix M2( xx, xy,
                 xy, yy );

      /*
       * Rotation matrix R
       */
      Matrix H = M1 * M2.Transpose();
      InPlaceSVD svd( H );
      Matrix R = svd.V * H.Transpose();

      /*
       * Orientation rectification (reflection case)
       */
      if ( R[0][0]*R[1][1] - R[0][1]*R[1][0] < 0 ) // if determinant < 0
      {
         InPlaceSVD svd( R );
         svd.V[0][1] *= -1;
         svd.V[1][1] *= -1;
         R = svd.V * R.Transpose();
      }

      /*
       * Rotation + translation matrix
       */
      return Matrix( R[0][0], R[0][1], p20.x - R[0][0]*p10.x - R[0][1]*p10.y,
                     R[1][0], R[1][1], p20.y - R[1][0]*p10.x - R[1][1]*p10.y,
                     0.0,     0.0,     1.0 );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_RigidTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/RigidTransformation.h - Released 2023-11-23T18:44:57Z
