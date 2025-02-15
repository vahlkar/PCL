//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Point.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_Point_h
#define __PCL_Point_h

/// \file pcl/Point.h

#include <pcl/Defs.h>

#include <pcl/Math.h>
#include <pcl/Relational.h>

#ifdef __PCL_QT_INTERFACE
#  include <QtCore/QPoint>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ### NB: Template class GenericPoint cannot have virtual member functions.
 *         This is because internal PCL and core routines rely on
 *         GenericPoint<int>, GenericPoint<float> and GenericPoint<double> to
 *         be directly castable to int*, float* and double*, respectively. See
 *         also the PCL_ASSERT_POINT_SIZE() macro.
 */

#define PCL_ASSERT_POINT_SIZE() \
   static_assert( sizeof( *this ) == 2*sizeof( T ), "Invalid sizeof( GenericPoint<> )" )

/*!
 * \class GenericPoint
 * \brief A generic point in the two-dimensional space.
 *
 * %GenericPoint implements a point in the plane, specified by its \a x and
 * \a y coordinates. The type T represents scalar point coordinates and can be
 * any real or integer numerical type.
 *
 * The coordinates of %GenericPoint are accessed directly by its x and y data
 * members. Given a %GenericPoint instance \a p, you can use \a p.x and \a p.y
 * directly to get or set coordinate values.
 *
 * \sa \ref point_types_2d "2-D Point Types",
 * \ref point_functions_2d "2-D Point Operators and Functions",
 * GenericRectangle
 */
template <typename T>
class PCL_CLASS GenericPoint
{
public:

   /*!
    * Represents the scalar type of a point component.
    */
   using component = T;

   /*
    * Point coordinates
    */
   component x; //!< Abscissa (horizontal, or X-axis coordinate).
   component y; //!< Ordinate (vertical, or Y-axis coordinate).

   /*!
    * Constructs a default %GenericPoint instance. Point coordinates are not
    * initialized, so they'll have unpredictable garbage values.
    */
   constexpr GenericPoint()
   {
      PCL_ASSERT_POINT_SIZE();
   }

   /*!
    * Constructs a %GenericPoint instance given its coordinates \a xPos and
    * \a yPos in the plane.
    *
    * The types \c T1 and \c T2 must be convertible to the component type \c T.
    */
   template <typename T1, typename T2>
   constexpr GenericPoint( T1 xPos, T2 yPos )
      : x( component( xPos ) )
      , y( component( yPos ) )
   {
      PCL_ASSERT_POINT_SIZE();
   }

   /*!
    * Constructs a %GenericPoint instance with both point coordinates equal to
    * the specified scalar \a d.
    */
   constexpr GenericPoint( component d )
      : GenericPoint( d, d )
   {
      PCL_ASSERT_POINT_SIZE();
   }

   /*!
    * Constructs a %GenericPoint instance from coordinates taken from the
    * specified initializer list \a l.
    *
    * This constructor will copy 2, 1 or zero point coordinates, depending on
    * the number of values in the initializer list. Coordinates that cannot be
    * initialized from list values will be set to zero. For example, the
    * following code:
    *
    * \code
    *    Point p1 = {};
    *    Point p2 = { 1 };
    *    Point p3 = { 1, 2 };
    * \endcode
    *
    * is functionally equivalent to:
    *
    * \code
    *    Point p1( 0, 0 );
    *    Point p2( 1, 0 );
    *    Point p3( 1, 2 );
    * \endcode
    *
    * The type \c T1 must be convertible to the component type \c T.
    */
   template <typename T1>
   GenericPoint( std::initializer_list<T1> l )
   {
      PCL_ASSERT_POINT_SIZE();
      switch ( l.size() )
      {
      default:
      case 2: y = component( l.begin()[1] );
      case 1: x = component( l.begin()[0] );
      case 0: break;
      }
      switch ( l.size() )
      {
      case 0: x = component( 0 );
      case 1: y = component( 0 );
      default:
      case 2: break;
      }
   }

   /*!
    * Nontrivial copy constructor. Constructs a %GenericPoint instance as a
    * copy (possibly involving a conversion between different numerical types)
    * of another point \a p with a different template argument.
    *
    * The type \c T1 must be convertible to the component type \c T.
    */
   template <typename T1>
   GenericPoint( const GenericPoint<T1>& p )
      : GenericPoint( p.x, p.y )
   {
      PCL_ASSERT_POINT_SIZE();
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint( const QPoint& p )
      : GenericPoint( p.x(), p.y() )
   {
      PCL_ASSERT_POINT_SIZE();
   }
#endif

   /*!
    * Returns the square of the Euclidian distance between this point and
    * another point \a p in the plane.
    *
    * The Euclidian distance between two points p and q is the length of the
    * straight line between both points: Sqrt( (p.x - q.x)^2 + (p.y - q.y)^2 ).
    *
    * This function returns just the radicand (p.x - q.x)^2 + (p.y - q.y)^2.
    * This is useful because saves calculation of a square root, which is
    * unnecessary in some practical cases. One of them is ordering of
    * distances: Sqrt(A) < Sqrt(B) implies A < B.
    *
    * The type \c T1 must be convertible to \c double.
    *
    * \sa DistanceTo(), ManhattanDistanceTo()
    */
   template <typename T1>
   double SquaredDistanceTo( const GenericPoint<T1>& p ) const noexcept
   {
      double dx = double( p.x ) - double( x );
      double dy = double( p.y ) - double( y );
      return dx*dx + dy*dy;
   }

   /*!
    * Returns the Euclidian distance between this point and another point \a p
    * in the plane.
    *
    * The Euclidian distance between two points p and q is the length of the
    * straight line between both points: Sqrt( (p.x - q.x)^2 + (p.y - q.y)^2 ).
    *
    * The type \c T1 must be convertible to \c double.
    *
    * \sa SquaredDistanceTo(), ManhattanDistanceTo()
    */
   template <typename T1>
   double DistanceTo( const GenericPoint<T1>& p ) const noexcept
   {
      return pcl::Sqrt( SquaredDistanceTo( p ) );
   }

   /*!
    * Returns the square of the Euclidian distance between this point and the
    * origin of coordinates in the plane. In other words, this function returns
    * the squared length of the two-dimensional vector represented by this
    * point object.
    *
    * This function is equivalent to:
    *
    * \code SquaredDistanceTo( GenericPoint<double>( 0 ) ) \endcode
    *
    * but potentially faster, depending mainly on compiler optimizations.
    *
    * \sa DistanceToOrigin()
    */
   double SquaredDistanceToOrigin() const noexcept
   {
      return double( x )*double( x ) + double( y )*double( y );
   }

   /*!
    * Returns the Euclidian distance between this point and the origin of
    * coordinates in the plane. In other words, this function returns the
    * length of the two-dimensional vector represented by this point object.
    *
    * This function is equivalent to:
    *
    * \code DistanceTo( GenericPoint<double>( 0 ) ) \endcode
    *
    * but potentially faster, depending mainly on compiler optimizations.
    *
    * \sa SquaredDistanceToOrigin(), ManhattanDistanceToOrigin()
    */
   double DistanceToOrigin() const noexcept
   {
      return pcl::Sqrt( SquaredDistanceToOrigin() );
   }

   /*!
    * Returns the Manhattan distance between this point and another point \a p
    * in the plane.
    *
    * The Manhattan distance between two points p and q is the sum of distances
    * measured along axes at right angles: |p.x - q.x| + |p.y - q.y|.
    *
    * The type \c T1 must be convertible to \c double.
    *
    * \sa DistanceTo(), ManhattanDistanceToOrigin()
    */
   template <typename T1>
   double ManhattanDistanceTo( const GenericPoint<T1>& p ) const noexcept
   {
      return Abs( double( p.x ) - double( x ) ) + Abs( double( p.y ) - double( y ) );
   }

   /*!
    * Returns the Manhattan distance between this point and the origin of
    * coordinates in the plane. In other words, this function returns the sum
    * of the absolute values of this point's x and y coordinates.
    *
    * This function is equivalent to:
    *
    * \code ManhattanDistanceTo( GenericPoint<double>( 0 ) ) \endcode
    *
    * but potentially faster, depending mainly on compiler optimizations.
    *
    * \sa ManhattanDistanceTo(), DistanceToOrigin()
    */
   double ManhattanDistanceToOrigin() const noexcept
   {
      return Abs( double( x ) ) + Abs( double( y ) );
   }

   /*!
    * Moves this point to the location of another point \a p.
    *
    * The type \c T1 must be convertible to the component type \c T.
    *
    * \sa MoveBy()
    */
   template <typename T1>
   void MoveTo( const GenericPoint<T1>& p ) noexcept
   {
      MoveTo( p.x, p.y );
   }

   /*!
    * Moves this point to the specified absolute coordinates \a xPos and
    * \a yPos in the plane.
    *
    * The types \c T1 and \c T2 must be convertible to the component type \c T.
    *
    * \sa MoveBy()
    */
   template <typename T1, typename T2>
   void MoveTo( T1 xPos, T2 yPos ) noexcept
   {
      x = component( xPos ); y = component( yPos );
   }

#ifdef __PCL_QT_INTERFACE
   void MoveTo( const QPoint& p ) noexcept
   {
      MoveTo( p.x(), p.y() );
   }
#endif

   /*!
    * Returns a point whose coordinates are equal to the coordinates of the
    * specified point \a d.
    *
    * The type \c T1 must be convertible to the component type \c T.
    */
   template <typename T1>
   GenericPoint MovedTo( const GenericPoint<T1>& p ) const noexcept
   {
      return GenericPoint( component( p.x ), component( p.y ) );
   }

   /*!
    * Returns a point at the specified \a xPos and \a yPos coordinates.
    *
    * The types \c T1 and \c T2 must be convertible to the component type \c T.
    */
   template <typename T1, typename T2>
   GenericPoint MovedTo( T1 xPos, T2 yPos ) const noexcept
   {
      return GenericPoint( component( xPos ), component( yPos ) );
   }

   /*!
    * Moves this point by equal increments in the X and Y directions, relative
    * to its current position.
    *
    * \param d    Increments in the X and Y directions, given by \a d.x and
    *             \a d.y, respectively.
    *
    * The type \c T1 must be convertible to the component type \c T.
    *
    * \sa MoveTo()
    */
   template <typename T1>
   void MoveBy( const GenericPoint<T1>& d ) noexcept
   {
      MoveBy( d.x, d.y );
   }

   /*!
    * Moves this point by increments \a dx and \a dy in the X and Y directions,
    * respectively, relative to its current position.
    *
    * For example, given the declaration:
    *
    * \code GenericPoint<float> p; \endcode
    *
    * These two expressions are equivalent:
    *
    * \code
    * p.MoveBy( -3, 7 );
    * p.MoveTo( p.x-3, p.y+7 );
    * \endcode
    *
    * The types \c T1 and \c T2 must be convertible to the component type \c T.
    *
    * \sa MoveTo()
    */
   template <typename T1, typename T2>
   void MoveBy( T1 dx, T2 dy ) noexcept
   {
      x += component( dx ); y += component( dy );
   }

   /*!
    * Moves this point by the same increment \a dxy in the X and Y directions,
    * relative to its current position.
    *
    * This function is functionally equivalent to:
    *
    * \code MoveBy( dxy, dxy ); \endcode
    *
    * \sa MoveTo()
    */
   template <typename T1>
   void MoveBy( T1 dxy ) noexcept
   {
      x += component( dxy ); y += component( dxy );
   }

#ifdef __PCL_QT_INTERFACE
   void MoveBy( const QPoint& p )
   {
      MoveBy( p.x(), p.y() );
   }
#endif

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * displaced by the increments specified as the point \a d.
    *
    * The type \c T1 must be convertible to the component type \c T.
    */
   template <typename T1>
   GenericPoint MovedBy( const GenericPoint<T1>& d ) const noexcept
   {
      return GenericPoint( x + component( d.x ), y + component( d.y ) );
   }

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * displaced by the specified increments \a dx and \a dy.
    *
    * The types \c T1 and \c T2 must be convertible to the component type \c T.
    */
   template <typename T1, typename T2>
   GenericPoint MovedBy( T1 dx, T2 dy ) const noexcept
   {
      return GenericPoint( x + component( dx ), y + component( dy ) );
   }

   /*!
    * Rotates this point in the plane by the specified \a angle in radians,
    * with respect to a center of rotation given by its coordinates \a xc and
    * \a yc.
    *
    * The types \c T1 and \c T2 must be convertible to \c double.
    */
   template <typename T1, typename T2>
   void Rotate( T1 angle, T2 xc, T2 yc ) noexcept
   {
      pcl::Rotate( x, y, angle, xc, yc );
   }

   /*!
    * Rotates this point in the plane by the specified \a angle in radians,
    * with respect to the specified \a center of rotation.
    *
    * The types \c T1 and \c T2 must be convertible to \c double.
    */
   template <typename T1, typename T2>
   void Rotate( T1 angle, const GenericPoint<T2>& center ) noexcept
   {
      Rotate( angle, center.x, center.y );
   }

   /*!
    * Rotates this point in the plane by the specified angle, given by its sine
    * and cosine, \a sa and \a ca respectively, with respect to a center of
    * rotation given by its coordinates \a xc and \a yc.
    *
    * The types \c T1 and \c T2 must be convertible to \c double.
    */
   template <typename T1, typename T2>
   void Rotate( T1 sa, T1 ca, T2 xc, T2 yc ) noexcept
   {
      pcl::Rotate( x, y, sa, ca, xc, yc );
   }

   /*!
    * Rotates this point in the plane by the specified angle, given by its sine
    * and cosine, \a sa and \a ca respectively, with respect to the specified
    * \a center of rotation.
    *
    * The types \c T1 and \c T2 must be convertible to \c double.
    */
   template <typename T1, typename T2>
   void Rotate( T1 sa, T1 ca, const GenericPoint<T2>& center ) noexcept
   {
      Rotate( sa, ca, center.x, center.y );
   }

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * rotated in the plane by the specified \a angle in radians, with respect
    * to a center of rotation given by its coordinates \a xc and \a yc.
    *
    * The types \c T1 and \c T2 must be convertible to \c double.
    */
   template <typename T1, typename T2>
   GenericPoint Rotated( T1 angle, T2 xc, T2 yc ) const noexcept
   {
      GenericPoint p( *this );
      p.Rotate( angle, xc, yc );
      return p;
   }

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * rotated in the plane by the specified \a angle in radians, with respect
    * to the specified \a center of rotation.
    *
    * The types \c T1 and \c T2 must be convertible to \c double.
    */
   template <typename T1, typename T2>
   GenericPoint Rotated( T1 angle, const GenericPoint<T2>& center ) const noexcept
   {
      GenericPoint p( *this );
      p.Rotate( angle, center );
      return p;
   }

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * rotated in the plane by the specified angle given by its sine and cosine,
    * \a sa and \a ca respectively, with respect to a center of rotation given
    * by its coordinates \a xc and \a yc.
    *
    * The types \c T1 and \c T2 must be convertible to \c double.
    */
   template <typename T1, typename T2>
   GenericPoint Rotated( T1 sa, T1 ca, T2 xc, T2 yc ) const noexcept
   {
      GenericPoint p( *this );
      p.Rotate( sa, ca, xc, yc );
      return p;
   }

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * rotated in the plane by the specified angle given by its sine and cosine,
    * \a sa and \a ca respectively, with respect to the specified \a center of
    * rotation.
    *
    * The types \c T1 and \c T2 must be convertible to \c double.
    */
   template <typename T1, typename T2>
   GenericPoint Rotated( T1 sa, T1 ca, const GenericPoint<T2>& center ) const noexcept
   {
      GenericPoint p( *this );
      p.Rotate( sa, ca, center );
      return p;
   }

   /*!
    * Rounds the coordinates of this point to their corresponding nearest
    * integer coordinates.
    *
    * \sa Round( int ), Rounded(), Truncate(), RoundedToInt(), TruncatedToInt()
    */
   void Round() noexcept
   {
      x = component( pcl::Round( double( x ) ) );
      y = component( pcl::Round( double( y ) ) );
   }

   /*!
    * Rounds the coordinates of this point to \a n fractional digits (\a n >= 0).
    *
    * \sa Round(), Rounded( int ), Truncate(), RoundedToInt(), TruncatedToInt()
    */
   void Round( int n ) noexcept
   {
      PCL_PRECONDITION( n >= 0 )
      x = component( pcl::Round( double( x ), n ) );
      y = component( pcl::Round( double( y ), n ) );
   }

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * rounded to their nearest integers.
    *
    * \sa Round(), Rounded( int ), Truncated(), RoundedToInt(), TruncatedToInt()
    */
   GenericPoint Rounded() const noexcept
   {
      return GenericPoint( component( pcl::Round( double( x ) ) ), component( pcl::Round( double( y ) ) ) );
   }

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * rounded to \a n fractional digits (\a n >= 0).
    *
    * \sa Round( int ), Rounded(), Truncated(), RoundedToInt(), TruncatedToInt()
    */
   GenericPoint Rounded( int n ) const noexcept
   {
      PCL_PRECONDITION( n >= 0 )
      return GenericPoint( component( pcl::Round( double( x ), n ) ), component( pcl::Round( double( y ), n ) ) );
   }

   /*!
    * Returns a point of integer template type whose coordinates are the
    * coordinates of this point rounded to their nearest integers.
    *
    * \sa Round(), Rounded(), Truncated(), TruncatedToInt()
    */
   GenericPoint<int> RoundedToInt() const noexcept
   {
      return GenericPoint<int>( pcl::RoundInt( double( x ) ), pcl::RoundInt( double( y ) ) );
   }

   /*!
    * Integer truncation of coordinates. Sets the coordinates x, y of this
    * point to the corresponding nearest integer coordinates i, j, such that
    * i <= x and j <= y.
    *
    * \sa Truncated(), Round(), RoundedToInt(), TruncatedToInt()
    */
   void Truncate() noexcept
   {
      x = component( pcl::Trunc( double( x ) ) );
      y = component( pcl::Trunc( double( y ) ) );
   }

   /*!
    * Integer truncation of coordinates. Returns a point whose coordinates are
    * the coordinates x, y of this point truncated to their corresponding
    * nearest integer coordinates i, j, such that i <= x and j <= y.
    *
    * \sa Truncate(), Rounded(), RoundedToInt(), TruncatedToInt()
    */
   GenericPoint Truncated() const noexcept
   {
      return GenericPoint( component( pcl::Trunc( double( x ) ) ), component( pcl::Trunc( double( y ) ) ) );
   }

   /*!
    * Integer truncation of coordinates. Returns a point of integer template
    * type whose coordinates are the coordinates x, y of this point truncated
    * to the nearest integer coordinates i, j, such that i <= x and j <= y.
    *
    * \sa Truncate(), Truncated(), Rounded(), RoundedToInt()
    */
   GenericPoint<int> TruncatedToInt() const noexcept
   {
      return GenericPoint<int>( pcl::TruncInt( double( x ) ), pcl::TruncInt( double( y ) ) );
   }

   /*!
    * Nontrivial assignment operator. Moves this point to the location
    * specified as a point \a p. Returns a reference to this point.
    *
    * The type \c T1 must be convertible to the component type \c T.
    *
    * \sa MoveTo()
    */
   template <typename T1>
   GenericPoint& operator =( const GenericPoint<T1>& p ) noexcept
   {
      x = component( p.x );
      y = component( p.y );
      return *this;
   }

   /*!
    * Assignment operator. Given a coordinate \a v, moves this point to a
    * location x = y = \a v. Returns a reference to this point.
    *
    * \sa MoveTo()
    */
   GenericPoint& operator =( component v ) noexcept
   {
      x = y = v;
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint& operator =( const QPoint& p ) noexcept
   {
      x = component( p.x() );
      y = component( p.y() );
      return *this;
   }
#endif

   /*!
    * Adds each coordinate of a given point \a p to its homonym coordinate in
    * this point. Returns a reference to this point.
    *
    * The type \c T1 must be convertible to the component type \c T.
    *
    * \sa MoveBy(), operator -=()
    */
   template <typename T1>
   GenericPoint& operator +=( const GenericPoint<T1>& p ) noexcept
   {
      x += component( p.x );
      y += component( p.y );
      return *this;
   }

   /*!
    * Adds a scalar \a d to both coordinates of this point. Returns a reference
    * to this point.
    *
    * \sa MoveBy(), operator -=()
    */
   GenericPoint& operator +=( component d ) noexcept
   {
      x += d;
      y += d;
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint& operator +=( const QPoint& p ) noexcept
   {
      x += component( p.x() );
      y += component( p.y() );
      return *this;
   }
#endif

   /*!
    * Subtracts each coordinate of a given point \a p from its homonym
    * coordinate in this point. Returns a reference to this point.
    *
    * The type \c T1 must be convertible to the component type \c T.
    *
    * \sa MoveBy(), operator +=()
    */
   template <typename T1>
   GenericPoint& operator -=( const GenericPoint<T1>& p ) noexcept
   {
      x -= component( p.x );
      y -= component( p.y );
      return *this;
   }

   /*!
    * Subtracts a scalar \a d from both coordinates of this point. Returns a
    * reference to this point.
    *
    * \sa MoveBy(), operator +=()
    */
   GenericPoint& operator -=( component d ) noexcept
   {
      x -= d;
      y -= d;
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint& operator -=( const QPoint& p ) noexcept
   {
      x -= component( p.x() );
      y -= component( p.y() );
      return *this;
   }
#endif

   /*!
    * Multiplies each coordinate of this point by its homonym coordinate of a
    * given point \a p. Returns a reference to this point.
    *
    * The type \c T1 must be convertible to the component type \c T.
    *
    * \sa operator /=(), operator +=()
    */
   template <typename T1>
   GenericPoint& operator *=( const GenericPoint<T1>& p ) noexcept
   {
      x *= component( p.x );
      y *= component( p.y );
      return *this;
   }

   /*!
    * Multiplies both coordinates of this point by a scalar \a d. Returns a
    * reference to this point.
    *
    * \sa operator /=(), operator +=()
    */
   GenericPoint& operator *=( component d ) noexcept
   {
      x *= d;
      y *= d;
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint& operator *=( const QPoint& p ) noexcept
   {
      x *= component( p.x() );
      y *= component( p.y() );
      return *this;
   }
#endif

   /*!
    * Divides each coordinate of this point by its homonym coordinate of a
    * given point \a p. Returns a reference to this point.
    *
    * The type \c T1 must be convertible to the component type \c T.
    *
    * \sa operator *=(), operator -=()
    */
   template <typename T1>
   GenericPoint& operator /=( const GenericPoint<T1>& p ) noexcept
   {
      PCL_PRECONDITION( component( p.x ) != component( 0 ) && component( p.y ) != component( 0 ) )
      x /= component( p.x );
      y /= component( p.y );
      return *this;
   }

   /*!
    * Divides both coordinates of this point by a scalar \a d. Returns a
    * reference to this point.
    *
    * \sa operator *=(), operator -=()
    */
   GenericPoint& operator /=( component d ) noexcept
   {
      PCL_PRECONDITION( d != component( 0 ) )
      x /= d;
      y /= d;
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint& operator /=( const QPoint& p ) noexcept
   {
      PCL_PRECONDITION( component( p.x() ) != component( 0 ) && component( p.y() ) != component( 0 ) )
      x /= component( p.x() );
      y /= component( p.y() );
      return *this;
   }
#endif

   /*!
    * Returns a copy of this point.
    */
   GenericPoint operator +() const noexcept
   {
      return *this;
   }

   /*!
    * Returns a point whose coordinates have the same magnitudes as the
    * coordinates of this point, but opposite signs. The returned point so
    * defined represents a vector diametrically opposed to this one.
    */
   GenericPoint operator -() const noexcept
   {
      return GenericPoint( -x, -y );
   }

   /*!
    * Reflects this point in the origin. This transformation changes the signs
    * of both point coordinates.
    */
   void Reflect() noexcept
   {
      x = -x;
      y = -y;
   }

   /*!
    * Reflects this point across the X axis. This transformation changes the
    * sign of this point's y-coordinate.
    */
   void ReflectX() noexcept
   {
      y = -y;
   }

   /*!
    * Reflects this point across the Y axis. This transformation changes the
    * sign of this point's x-coordinate.
    */
   void ReflectY() noexcept
   {
      x = -x;
   }

   /*!
    * Returns the reflexion of this point in the origin. The returned point has
    * the coordinates of this point with inverse signs.
    */
   GenericPoint Reflected() const noexcept
   {
      return GenericPoint( -x, -y );
   }

   /*!
    * Returns the reflexion of this point across the X axis. The returned point
    * has the same x-coordinate as this point and the y-coordinate of this
    * point with inverse sign.
    */
   GenericPoint ReflectedX() const noexcept
   {
      return GenericPoint( x, -y );
   }

   /*!
    * Returns the reflexion of this point across the Y axis. The returned point
    * has the x-coordinate of this point with inverse sign and the same
    * y-coordinate as this point.
    */
   GenericPoint ReflectedY() const noexcept
   {
      return GenericPoint( -x, y );
   }

   /*
    * Returns a point whose coordinates are the coordinates of this point
    * converted to radians, assuming that this point has coordinates expressed
    * in degrees.
    */
   GenericPoint Rad() const noexcept
   {
      return GenericPoint( pcl::Rad( x ), pcl::Rad( y ) );
   }

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * converted to degrees, assuming that this point has coordinates expressed
    * in radians.
    */
   GenericPoint Deg() const noexcept
   {
      return GenericPoint( pcl::Deg( x ), pcl::Deg( y ) );
   }

   /*!
    * Converts the coordinates of this point to radians, assuming that they are
    * expressed in degrees before calling this function. Returns a reference to
    * this object.
    */
   GenericPoint& ToRad() noexcept
   {
      x = pcl::Rad( x );
      y = pcl::Rad( y );
      return *this;
   }

   /*!
    * Converts the coordinates of this point to degrees, assuming that they are
    * expressed in radians before calling this function. Returns a reference to
    * this object.
    */
   GenericPoint& ToDeg() noexcept
   {
      x = pcl::Deg( x );
      y = pcl::Deg( y );
      return *this;
   }

   /*!
    * Returns a reference to a point component. Returns a reference to the X
    * point coordinate if the specified index \a i is zero, or a reference to
    * the Y coordinate otherwise.
    */
   component& operator []( int i ) noexcept
   {
      return (i == 0) ? x : y;
   }

   /*!
    * Returns a copy of a point component. Returns the X point coordinate if
    * the specified index \a i is zero, or the Y coordinate otherwise.
    */
   component operator []( int i ) const noexcept
   {
      return (i == 0) ? x : y;
   }

   /*!
    * Returns the dot product of this point and another point given by its
    * coordinates \a px and \a py.
    *
    * The types \c T1 and \c T2 must be convertible to \c double.
    */
   template <typename T1, typename T2>
   double Dot( T1 px, T2 py ) const noexcept
   {
      return double( x )*double( px ) + double( y )*double( py );
   }

   /*!
    * Returns the dot product of this point and another point \a p.
    *
    * The type \c T1 must be convertible to \c double.
    */
   template <typename T1>
   double Dot( const GenericPoint<T1>& p ) const noexcept
   {
      return Dot( p.x, p.y );
   }

#ifdef __PCL_QT_INTERFACE
   operator QPoint() const noexcept
   {
      return QPoint( int( x ), int( y ) );
   }
#endif
};

#undef PCL_ASSERT_POINT_SIZE

// ----------------------------------------------------------------------------

/*!
 * \defgroup point_functions_2d Point Operators and Functions
 */

/*!
 * Returns true iff two points \a p1 and \a p2 are equal. Two points are equal
 * if their homonym coordinates are equal.
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
bool operator ==( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 ) noexcept
{
   return p1.x == p2.x && p1.y == p2.y;
}

/*!
 * Returns true iff a point \a p1 is equal to a scalar \a d2. A point \p is
 * equal to a scalar \a d if both coordinates of \a p are equal to \a d.
 * \ingroup point_functions_2d
 */
template <typename T> inline
bool operator ==( const GenericPoint<T>& p1, T d2 ) noexcept
{
   return p1.x == d2 && p1.y == d2;
}

/*!
 * Returns true iff a scalar \a d1 is equal to a point \a p2. A scalar \a d is
 * equal to a point \a p if both coordinates of \a p are equal to \a d.
 * \ingroup point_functions_2d
 */
template <typename T> inline
bool operator ==( T d1, const GenericPoint<T>& p2 ) noexcept
{
   return d1 == p2.x && d1 == p2.y;
}

/*!
 * Returns true iff a point \a p1 is less than another point \a p2.
 *
 * Points in the plane are sorted by ordinates in PCL. Given two points \a p
 * and \a q in the plane, it holds that:
 *
 * \a p < \a q if \a p.y < \a q.y or \a p.y == \a q.y and \a p.x < \a q.x
 *
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
bool operator <( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 ) noexcept
{
   return p1.y < p2.y || p1.y == p2.y && p1.x < p2.x;
}

/*!
 * Returns true iff a point \a p1 is less than a scalar \a d2.
 *
 * Given a point \a p in the plane and a scalar \a d, it holds that:
 *
 * \a p < \a d if \a p.y < \a d or \a p.y == \a d and \a p.x < \a d
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
bool operator <( const GenericPoint<T>& p1, T d2 ) noexcept
{
   return p1.y < d2 || p1.y == d2 && p1.x < d2;
}

/*!
 * Returns true iff a scalar \a d1 is less than a point \a p2.
 *
 * Given a point \a p in the plane and a scalar \a d, it holds that:
 *
 * \a d < \a p if \a d < \a p.y or \a d == \a p.y and \a d < \a p.x
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
bool operator <( T d1, const GenericPoint<T>& p2 ) noexcept
{
   return d1 < p2.y || d1 == p2.y && d1 < p2.x;
}

/*!
 * Adds two points \a p1 and \a p2, and returns the resulting sum point.
 *
 * Point addition consists in adding homonym coordinates. Given two points
 * \a p and \a q in the plane, the sum point \a s = \a p + \a q is given by:
 *
 * \a s.x = \a p.x + \a q.x \n
 * \a s.y = \a p.y + \a q.y
 *
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
GenericPoint<T1> operator +( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 ) noexcept
{
   return GenericPoint<T1>( T1( p1.x + p2.x ), T1( p1.y + p2.y ) );
}

/*!
 * Adds a point \a p1 and a scalar \a d2, and returns the resulting sum point.
 *
 * Given a point \a p in the plane and a scalar \a d, the sum point
 * \a s = \a p + \a d is given by:
 *
 * \a s.x = \a p.x + \a d \n
 * \a s.y = \a p.y + \a d
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator +( const GenericPoint<T>& p1, T d2 ) noexcept
{
   return GenericPoint<T>( p1.x+d2, p1.y+d2 );
}

/*!
 * Adds a scalar \a d1 and a point \a p2, and returns the resulting sum point.
 *
 * Point addition is a commutative operation, thus see
 * pcl::operator +( const GenericPoint<T>&, T ).
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator +( T d1, const GenericPoint<T>& p2 ) noexcept
{
   return GenericPoint<T>( p2.x+d1, p2.y+d1 );
}

/*!
 * Subtracts two points \a p1 and \a p2, and returns the resulting difference
 * point.
 *
 * Point subtraction consists in subtracting homonym coordinates. Given two
 * points \a p and \a q in the plane, the difference point \a r = \a p - \a q
 * is given by:
 *
 * \a r.x = \a p.x - \a q.x \n
 * \a r.y = \a p.y - \a q.y
 *
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
GenericPoint<T1> operator -( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 ) noexcept
{
   return GenericPoint<T1>( T1( p1.x - p2.x ), T1( p1.y - p2.y ) );
}

/*!
 * Subtracts a scalar \a d2 from a point \a p1, and returns the resulting
 * difference point.
 *
 * Given a point \a p in the plane and a scalar \a d, the difference point
 * \a r = \a p - \a d is given by:
 *
 * \a r.x = \a p.x - \a d \n
 * \a r.y = \a p.y - \a d
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator -( const GenericPoint<T>& p1, T d2 ) noexcept
{
   return GenericPoint<T>( p1.x-d2, p1.y-d2 );
}

/*!
 * Subtracts a point \a p2 from a scalar \a d1, and returns the resulting
 * difference point.
 *
 * Given a point \a p in the plane and a scalar \a d, the difference point
 * \a r = \a d - \a p is given by:
 *
 * \a r.x = \a d - \a p.x \n
 * \a r.y = \a d - \a p.y
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator -( T d1, const GenericPoint<T>& p2 ) noexcept
{
   return GenericPoint<T>( d1-p2.x, d1-p2.y );
}

/*!
 * Multiplies two points \a p1 and \a p2, and returns the resulting product
 * point.
 *
 * Point multiplication consists in multiplying homonym coordinates. Given two
 * points \a p and \a q in the plane, the product point \a P = \a p * \a q is
 * given by:
 *
 * \a P.x = \a p.x * \a q.x \n
 * \a P.y = \a p.y * \a q.y
 *
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
GenericPoint<T1> operator *( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 ) noexcept
{
   return GenericPoint<T1>( T1( p1.x * p2.x ), T1( p1.y * p2.y ) );
}

/*!
 * Multiplies a point \a p1 by a scalar \a d2, and returns the resulting
 * product point.
 *
 * Given a point \a p in the plane and a scalar \a d, the product point
 * \a P = \a p * \a d is given by:
 *
 * \a P.x = \a p.x * \a d \n
 * \a P.y = \a p.y * \a d
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator *( const GenericPoint<T>& p1, T d2 ) noexcept
{
   return GenericPoint<T>( p1.x*d2, p1.y*d2 );
}

/*!
 * Multiplies a scalar \a d1 by a point \a p2, and returns the resulting
 * product point.
 *
 * Point multiplication is a commutative operation, thus see
 * pcl::operator *( const GenericPoint<T>&, T ).
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator *( T d1, const GenericPoint<T>& p2 ) noexcept
{
   return GenericPoint<T>( p2.x*d1, p2.y*d1 );
}

/*!
 * Divides two points \a p1 and \a p2, and returns the resulting quotient point.
 *
 * Given two points \a p and \a q in the plane, the quotient point
 * \a Q = \a p / \a q is given by:
 *
 * \a Q.x = \a p.x / \a q.x \n
 * \a Q.y = \a p.y / \a q.y
 *
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
GenericPoint<T1> operator /( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 ) noexcept
{
   PCL_PRECONDITION( p2.x != T2( 0 ) && p2.y != T2( 0 ) )
   return GenericPoint<T1>( T1( p1.x / p2.x ), T1( p1.y / p2.y ) );
}

/*!
 * Divides a point \a p1 by a scalar \a d2, and returns the resulting
 * quotient point.
 *
 * Given a point \a p in the plane and a scalar \a d, the quotient point
 * \a Q = \a p / \a d is given by:
 *
 * \a Q.x = \a p.x / \a d \n
 * \a Q.y = \a p.y / \a d
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator /( const GenericPoint<T>& p1, T d2 ) noexcept
{
   PCL_PRECONDITION( d2 != T( 0 ) )
   return GenericPoint<T>( p1.x/d2, p1.y/d2 );
}

/*!
 * Divides a scalar \a d1 by a point \a p2, and returns the resulting
 * quotient point.
 *
 * Given a point \a p in the plane and a scalar \a d, the quotient point
 * \a Q = \a d / \a q is given by:
 *
 * \a Q.x = \a d / \a p.x \n
 * \a Q.y = \a d / \a p.y
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator /( T d1, const GenericPoint<T>& p2 ) noexcept
{
   PCL_PRECONDITION( p2.x != T( 0 ) && p2.y != T( 0 ) )
   return GenericPoint<T>( d1/p2.x, d1/p2.y );
}

/*!
 * Returns the distance between two points \a p1 and \a p2 in the plane.
 * \ingroup point_functions_2d
 */
// ### FIXME: We cannot use two template arguments here due to a conflict with
//            Distance( FI, FI ) defined in Iterator.h
//template <typename T1, typename T2> inline
template <typename T> inline
double Distance( const GenericPoint<T>& p1, const GenericPoint<T>& p2 ) noexcept
{
   double dx = double( p2.x ) - double( p1.x );
   double dy = double( p2.y ) - double( p1.y );
   return pcl::Sqrt( dx*dx + dy*dy );
}

/*!
 * Returns the Manhattan distance between two points \a p1 and \a p2 in the
 * plane.
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
double ManhattanDistance( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 ) noexcept
{
   return Abs( double( p2.x ) - double( p1.x ) ) + Abs( double( p2.y ) - double( p1.y ) );
}

/*!
 * Rotates a point in the plane, given a rotation angle and the coordinates of
 * a center of rotation.
 *
 * \param p    Reference to a point that will be rotated.
 * \param a    Rotation angle in radians. Positive angles are measured
 *             counter-clockwise.
 * \param xc   Abscissa (x coordinate) of the center of rotation.
 * \param yc   Ordinate (y coordinate) of the center of rotation.
 *
 * The coordinates \a p.x and \a p.y are replaced by their corresponding
 * rotated values.
 *
 * \ingroup point_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericPoint<T>& p, T1 a, T2 xc, T2 yc ) noexcept
{
   pcl::Rotate( p.x, p.y, a, xc, yc );
}

/*!
 * Rotates a point in the plane, given a rotation angle and a rotation center
 * point.
 *
 * \param p    Reference to a point that will be rotated.
 * \param a    Rotation angle in radians. Positive angles are measured
 *             counter-clockwise.
 * \param c    Reference to a point that will be taken as the center of
 *             rotation.
 *
 * The coordinates \a p.x and \a p.y are replaced by their corresponding
 * rotated values.
 *
 * \ingroup point_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericPoint<T>& p, T1 a, const GenericPoint<T2>& c ) noexcept
{
   pcl::Rotate( p, a, c.x, c.y );
}

/*!
 * Rotates a point in the plane, given a rotation angle by its sine and
 * cosine, and the coordinates of a center of rotation.
 *
 * \param p    Reference to a point that will be rotated.
 * \param sa   Sine of the rotation angle.
 * \param ca   Cosine of the rotation angle.
 * \param xc   Abscissa (x coordinate) of the center of rotation.
 * \param yc   Ordinate (y coordinate) of the center of rotation.
 *
 * The coordinates \a p.x and \a p.y are replaced by their corresponding
 * rotated values.
 *
 * \ingroup point_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericPoint<T>& p, T1 sa, T1 ca, T2 xc, T2 yc ) noexcept
{
   pcl::Rotate( p.x, p.y, sa, ca, xc, yc );
}

/*!
 * Rotates a point in the plane, given a rotation angle by its sine and
 * cosine, and a rotation center point.
 *
 * \param p    Reference to a point that will be rotated.
 * \param sa   Sine of the rotation angle.
 * \param ca   Cosine of the rotation angle.
 * \param c    Reference to a point that will be taken as the center of
 *             rotation.
 *
 * The coordinates \a p.x and \a p.y are replaced by their corresponding
 * rotated values.
 *
 * \ingroup point_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericPoint<T>& p, T1 sa, T1 ca, const GenericPoint<T2>& c ) noexcept
{
   pcl::Rotate( p, sa, ca, c.x, c.y );
}

/*!
 * Exchanges two points \a p1 and \a p2 in the plane. Calling this function is
 * equivalent to:
 *
 * \code Swap( p1.x, p2.x ); Swap( p1.y, p2.y ); \endcode
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
void Swap( GenericPoint<T>& p1, GenericPoint<T>& p2 ) noexcept
{
   pcl::Swap( p1.x, p2.x );
   pcl::Swap( p1.y, p2.y );
}

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_POINT_INSTANTIATE

/*!
 * \defgroup point_types_2d Point Types
 */

/*!
 * \class pcl::I32Point
 * \ingroup point_types_2d
 * \brief 32-bit integer point on the plane.
 *
 * %I32Point is a template instantiation of GenericPoint for the \c int32 type.
 */
using I32Point = GenericPoint<int32>;

/*!
 * \class pcl::Point
 * \ingroup point_types_2d
 * \brief 32-bit integer point on the plane.
 *
 * %Point is an alias for I32Point. It is a template instantiation of
 * GenericPoint for \c int32.
 */
using Point = I32Point;

/*!
 * \class pcl::F32Point
 * \ingroup point_types_2d
 * \brief 32-bit floating-point point in the R^2 space.
 *
 * %F32Point is a template instantiation of GenericPoint for the \c float type.
 */
using F32Point = GenericPoint<float>;

/*!
 * \class pcl::FPoint
 * \ingroup point_types_2d
 * \brief 32-bit floating-point point in the R^2 space.
 *
 * %FPoint is an alias for F32Point. It is a template instantiation of
 * GenericPoint for \c float.
 */
using FPoint = F32Point;

/*!
 * \class pcl::F64Point
 * \ingroup point_types_2d
 * \brief 64-bit floating-point point in the R^2 space.
 *
 * %F64Point is a template instantiation of GenericPoint for \c double.
 */
using F64Point = GenericPoint<double>;

/*!
 * \class pcl::DPoint
 * \ingroup point_types_2d
 * \brief 64-bit floating-point point in the R^2 space.
 *
 * %DPoint is an alias for F64Point. It is a template instantiation of
 * GenericPoint for \c double.
 */
using DPoint = F64Point;

#endif   // !__PCL_NO_POINT_INSTANTIATE

// ----------------------------------------------------------------------------

/*!
 * \class PointsClockwisePredicate
 * \brief Predicate class for sorting a set of points in clockwise direction.
 */
class PCL_CLASS PointsClockwisePredicate
{
public:

   /*!
    * Default constructor.
    */
   PointsClockwisePredicate() = default;

   /*!
    * Copy constructor.
    */
   PointsClockwisePredicate( const PointsClockwisePredicate& ) = default;

   /*!
    * Constructor from a set of points.
    *
    * Initializes point sorting with respect to the barycenter coordinates of
    * the specified set of points \a P.
    */
   template <class C>
   PointsClockwisePredicate( const C& P )
   {
      PCL_PRECONDITION( P.Length() > 2 )
      for ( const auto& p : P )
      {
         m_cx += double( p.x );
         m_cy += double( p.y );
      }
      m_cx /= P.Length();
      m_cy /= P.Length();
   }

   /*!
    * Constructor with a prescribed barycenter point \a p.
    */
   template <typename T>
   PointsClockwisePredicate( const GenericPoint<T>& p )
      : m_cx( double( p.x ) )
      , m_cy( double( p.y ) )
   {
   }

   /*!
    * Predicate function: Returns true iff the point \a a precedes the point
    * \a b in clockwise sorting order.
    */
   template <class P1, class P2>
   bool operator()( const P1& a, const P2& b ) const
   {
      /*
       * Points on different sides of the vertical line passing through c.
       *
       * - a to the right and b to the left: a < b
       * - a to the left and b to the right: a > b
       */
      if ( a.x >= m_cx )
      {
         if ( b.x < m_cx )
            return true;
      }
      else
      {
         if ( b.x >= m_cx )
            return false;
      }

      /*
       * If the points are not collinear, sort in clockwise direction.
       *
       * d:  > 0 if b is to the left of the line c-a
       *     < 0 if b is to the right of the line c-a
       *    == 0 if b is on the line c-a
       */
      double d = (a.x - m_cx)*(b.y - m_cy) - (b.x - m_cx)*(a.y - m_cy);
      if ( likely( d != 0 ) )
         return d < 0;

      /*
       * Sort collinear points by their distance to the center.
       */
      double dxa = a.x - m_cx;
      double dxb = b.x - m_cx;
      if ( dxa != dxb )
         return pcl::Abs( dxa ) < pcl::Abs( dxb );
      double dya = a.y - m_cy;
      double dyb = b.y - m_cy;
      return pcl::Abs( dya ) < pcl::Abs( dyb );
   }

private:

   // Reference center point
   double m_cx = 0.0;
   double m_cy = 0.0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Point_h

// ----------------------------------------------------------------------------
// EOF pcl/Point.h - Released 2024-12-28T16:53:48Z
