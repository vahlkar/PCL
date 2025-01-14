//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Rectangle.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_Rectangle_h
#define __PCL_Rectangle_h

/// \file pcl/Rectangle.h

#include <pcl/Defs.h>

#include <pcl/Flags.h>
#include <pcl/Math.h>
#include <pcl/Point.h>
#include <pcl/Relational.h>

#ifdef __PCL_QT_INTERFACE
#  include <QtCore/QRect>
#  ifndef __PCL_QT_NO_RECT_DRAWING_HELPERS
#    include <QtGui/QPainter>
#    include <QtGui/QBrush>
#  endif
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::Clip
 * \brief     %Clip codes used by the Cohen-Sutherland line clipping algorithm.
 *
 * Cohen-Sutherland clip codes:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>Clip::Inside</td>  <td>There is no clipping; the specified end point is inside the clipping rectangle.</td></tr>
 * <tr><td>Clip::Left</td>    <td>Clipping occurs at the left side of the clipping rectangle.</td></tr>
 * <tr><td>Clip::Top</td>     <td>Clipping occurs at the top side of the clipping rectangle.</td></tr>
 * <tr><td>Clip::Right</td>   <td>Clipping occurs at the right side of the clipping rectangle.</td></tr>
 * <tr><td>Clip::Bottom</td>  <td>Clipping occurs at the bottom side of the clipping rectangle.</td></tr>
 * </table>
 */
namespace Clip
{
   enum mask_type
   {
      Inside = 0x00, // Unclipped: inside the clipping rectangle
      Left   = 0x01, // Clipped at the left side
      Top    = 0x02, // Clipped at the top side
      Right  = 0x04, // Clipped at the right side
      Bottom = 0x08  // Clipped at the bottom side
   };
}

/*!
 * A collection of Cohen-Sutherland clip code flags.
 */
using ClipFlags = Flags<Clip::mask_type>;

// ----------------------------------------------------------------------------

/*!
 * \defgroup rect_classification_2d Rectangle Classification Functions
 */

/*!
 * Returns true iff a set of rectangle coordinates correspond to a point.
 *
 * \param x0,y0   Upper left corner coordinates.
 * \param x1,y1   Lower right corner coordinates.
 *
 * The specified coordinates define a point if and only if
 * \a x0 = \a x1 and \a y0 = \a y1.
 *
 * \ingroup rect_classification_2d
 */
template <typename T> inline
bool IsPoint( T x0, T y0, T x1, T y1 ) noexcept
{
   return x0 == x1 && y0 == y1;
}

/*!
 * Returns true iff a set of rectangle coordinates define a line.
 *
 * \param x0,y0   Upper left corner coordinates.
 * \param x1,y1   Lower right corner coordinates.
 *
 * The specified coordinates define a line if any of two conditions hold:
 * \a x0 = \a x1 or \a y0 = \a y1, but not both conditions, that is, if
 * the coordinates don't define a point.
 *
 * \ingroup rect_classification_2d
 */
template <typename T> inline
bool IsLine( T x0, T y0, T x1, T y1 ) noexcept
{
   return ((x0 == x1) ^ (y0 == y1)) != 0;
}

/*!
 * Returns true iff a set of rectangle coordinates define a horizontal line.
 *
 * \param x0,y0   Upper left corner coordinates.
 * \param x1,y1   Lower right corner coordinates.
 *
 * The specified coordinates define a horizontal line if and only if
 * \a y0 = \a y1 and \a x0 != \a x1.
 *
 * \ingroup rect_classification_2d
 */
template <typename T> inline
bool IsHorizontalLine( T x0, T y0, T x1, T y1 ) noexcept
{
   return y0 == y1 && x0 != x1;
}

/*!
 * Returns true iff a set of rectangle coordinates define a vertical line.
 *
 * \param x0,y0   Upper left corner coordinates.
 * \param x1,y1   Lower right corner coordinates.
 *
 * The specified coordinates define a vertical line if and only if
 * \a x0 = \a x1 and \a y0 != \a y1.
 *
 * \ingroup rect_classification_2d
 */
template <typename T> inline
bool IsVerticalLine( T x0, T y0, T x1, T y1 ) noexcept
{
   return x0 == x1 && y0 != y1;
}

/*!
 * Returns true iff a set of rectangle coordinates define a point or a line.
 *
 * \param x0,y0   Upper left corner coordinates.
 * \param x1,y1   Lower right corner coordinates.
 *
 * The specified coordinates define a point or line if either
 * \a x0 = \a x1 or \a y0 = \a y1.
 *
 * \ingroup rect_classification_2d
 */
template <typename T> inline
bool IsPointOrLine( T x0, T y0, T x1, T y1 ) noexcept
{
   return x0 == x1 || y0 == y1;
}

/*!
 * Returns true iff a set of rectangle coordinates define a rectangle.
 *
 * \param x0,y0   Upper left corner coordinates.
 * \param x1,y1   Lower right corner coordinates.
 *
 * The specified coordinates define a rectangle if and only if
 * \a x0 != \a x1 and \a y0 != \a y1.
 *
 * \ingroup rect_classification_2d
 */
template <typename T> inline
bool IsRect( T x0, T y0, T x1, T y1 ) noexcept
{
   return x0 != x1 && y0 != y1;
}

/*!
 * Returns true iff a set of rectangle coordinates define a normal rectangle.
 *
 * \param x0,y0   Upper left corner coordinates.
 * \param x1,y1   Lower right corner coordinates.
 *
 * A normal rectangle requires that \a x0 < \a x1 and \a y0 < \a y1.
 *
 * \ingroup rect_classification_2d
 */
template <typename T> inline
bool IsNormalRect( T x0, T y0, T x1, T y1 ) noexcept
{
   return x0 < x1 && y0 < y1;
}

/*!
 * Returns true iff a set of rectangle coordinates define an ordered rectangle.
 *
 * \param x0,y0   Upper left corner coordinates.
 * \param x1,y1   Lower right corner coordinates.
 *
 * An ordered rectangle requires that \a x0 <= \a x1 and \a y0 <= \a y1, that
 * is, it can be a normal rectangle, a line, or a point.
 *
 * \ingroup rect_classification_2d
 */
template <typename T> inline
bool IsOrderedRect( T x0, T y0, T x1, T y1 ) noexcept
{
   return x0 <= x1 && y0 <= y1;
}

/*!
 * Orders a set of rectangle coordinates to define an ordered rectangle.
 *
 * \param[out] x0,y0 References to the upper left corner coordinates.
 * \param[out] x1,y1 References to the lower right corner coordinates.
 *
 * This function ensures that \a x0 <= \a x1 and \a y0 <= \a y1, by exchanging
 * coordinates if necessary.
 *
 * \ingroup rect_classification_2d
 */
template <typename T> inline
void OrderRect( T& x0, T& y0, T& x1, T& y1 ) noexcept
{
   if ( x1 < x0 )
      pcl::Swap( x0, x1 );
   if ( y1 < y0 )
      pcl::Swap( y0, y1 );
}

// ----------------------------------------------------------------------------

/*
 * ### NB: Template class GenericRectangle cannot have virtual member
 *         functions. This is because internal PCL and core routines rely on
 *         GenericRectangle<int>, GenericRectangle<float> and
 *         GenericRectangle<double> to be directly castable to int*, float* and
 *         double*, respectively. See also the PCL_ASSERT_RECT_SIZE() macro.
 */

#define PCL_ASSERT_RECT_SIZE() \
   static_assert( sizeof( *this ) == 4*sizeof( T ), "Invalid sizeof( GenericRectangle<> )" )

/*!
 * \class GenericRectangle
 * \brief A generic rectangle in the two-dimensional space.
 *
 * %GenericRectangle implements a rectangle in the plane, specified by the \a x
 * and \a y coordinates of two of its corners. The type T represents scalar
 * rectangle coordinates and can be any real or integer numerical type.
 *
 * The coordinates of %GenericRectangle are directly accessed by its x0, y0, x1
 * and y1 data members. If the rectangle is \e ordered, x0 and y0 are the
 * coordinates of its upper left corner, and x1, y1 are the coordinates of its
 * lower right corner. Given a %GenericRectangle instance \a r, you can use
 * \a r.x0, \a r.y0, \a r.x1 and \a r.y1 directly to get or set coordinate
 * values.
 *
 * \b Important - In PCL, the right and bottom coordinates of a rectangle (that
 * is, the values of its x1 and y1 members) are \e excluded from the
 * corresponding rectangular area. The following holds for any rectangle \a r
 * in PCL:
 *
 * \a r.Width() = \a r.x1 - \a r.x0 \n
 * \a r.Height() = \a r.y1 - \a r.y0
 *
 * This is particularly relevant with template instantiations for integer
 * types, as GenericRectangle\<int\>.
 *
 * \sa \ref rect_types_2d "2-D Rectangle Types",
 * \ref rect_functions_2d "2-D Rectangle Operators and Functions",
 * \ref rect_classification_2d "2-D Rectangle Classification Functions",
 * GenericPoint
 */
template <typename T>
class PCL_CLASS GenericRectangle
{
public:

   /*!
    * Represents the type of a point or rectangle component.
    */
   using component = T;

   /*!
    * Represents a point on the plane.
    */
   using point = GenericPoint<component>;

   /*
    * Rectangle coordinates: x0=left, y0=top, x1=right, y1=bottom.
    * The x1 (right) and y1 (bottom) coordinates are excluded from the
    * rectangular area, so we always have: width=x1-x0 and height=y1-y0.
    */
   component x0; //!< Horizontal coordinate of the upper left corner.
   component y0; //!< Vertical coordinate of the upper left corner.
   component x1; //!< Horizontal coordinate of the lower right corner.
   component y1; //!< Vertical coordinate of the lower right corner.

   /*!
    * Constructs a default %GenericRectangle instance. Rectangle coordinates
    * are not initialized, so they'll have unpredictable garbage values.
    */
   constexpr GenericRectangle()
   {
      PCL_ASSERT_RECT_SIZE();
   }

   /*!
    * Constructs a %GenericRectangle instance given by its coordinates in the
    * plane.
    *
    * \param left,top      Coordinates of the upper left corner.
    * \param right,bottom  Coordinates of the lower right corner.
    *
    * The type T1 can be any suitable real or integer numerical type, or a
    * type with numeric conversion semantics.
    */
   template <typename T1>
   constexpr GenericRectangle( T1 left, T1 top, T1 right, T1 bottom )
      : x0( component( left ) )
      , y0( component( top ) )
      , x1( component( right ) )
      , y1( component( bottom ) )
   {
      PCL_ASSERT_RECT_SIZE();
   }

   /*!
    * Constructs a %GenericRectangle instance from coordinates taken from the
    * specified initializer list \a l.
    *
    * This constructor will copy 4, 3, 2, 1 or zero rectangle coordinates,
    * depending on the number of values in the initializer list. Coordinates
    * that cannot be initialized from list values will be set to zero. For
    * example, the following code:
    *
    * \code
    *    Rect r1 = {};
    *    Rect r2 = { 1, 2 };
    *    Rect r3 = { 1, 2, 3 };
    *    Rect r4 = { 1, 2, 3, 4 };
    * \endcode
    *
    * is functionally equivalent to:
    *
    * \code
    *    Rect r1( 0, 0, 0, 0 );
    *    Rect r2( 1, 2, 0, 0 );
    *    Rect r3( 1, 2, 3, 0 };
    *    Rect r4( 1, 2, 3, 4 };
    * \endcode
    */
   template <typename T1>
   GenericRectangle( std::initializer_list<T1> l )
   {
      PCL_ASSERT_RECT_SIZE();
      switch ( l.size() )
      {
      default:
      case 4: y1 = component( l.begin()[3] );
      case 3: x1 = component( l.begin()[2] );
      case 2: y0 = component( l.begin()[1] );
      case 1: x0 = component( l.begin()[0] );
      case 0: break;
      }
      switch ( l.size() )
      {
      case 0: x0 = component( 0 );
      case 1: y0 = component( 0 );
      case 2: x1 = component( 0 );
      case 3: y1 = component( 0 );
      default:
      case 4: break;
      }
   }

   /*!
    * Constructs a %GenericRectangle instance given two points in the plane.
    *
    * \param leftTop       Position of the upper left corner.
    * \param rightBottom   Position of the lower right corner.
    *
    * The type T1 can be any suitable real or integer numerical type, or a
    * type with numeric conversion semantics.
    */
   template <typename T1>
   GenericRectangle( const pcl::GenericPoint<T1>& leftTop, const pcl::GenericPoint<T1>& rightBottom )
      : GenericRectangle( component( leftTop.x ), component( leftTop.y ),
                          component( rightBottom.x ), component( rightBottom.y ) )
   {
      PCL_ASSERT_RECT_SIZE();
   }

   /*!
    * Constructs a %GenericRectangle instance given its \a width and \a height.
    * The coordinates of the constructed rectangle will be as follows:
    *
    * x0 = y0 = 0 \n
    * x1 = width \n
    * y1 = height
    */
   constexpr GenericRectangle( component width, component height )
      : GenericRectangle( component( 0 ), component( 0 ), width, height )
   {
      PCL_ASSERT_RECT_SIZE();
   }

   /*!
    * Constructs a %GenericRectangle instance corresponding to a point located
    * at the coordinates specified by a scalar \a d.
    *
    * The constructed rectangle will have all of its coordinates equal to the
    * scalar \a d.
    */
   constexpr GenericRectangle( component d )
      : GenericRectangle( d, d, d, d )
   {
      PCL_ASSERT_RECT_SIZE();
   }

   /*!
    * Nontrivial copy constructor. Constructs a %GenericRectangle instance as a
    * copy of the specified rectangle \a r.
    */
   template <typename T1>
   GenericRectangle( const GenericRectangle<T1>& r )
      : GenericRectangle( component( r.x0 ), component( r.y0 ),
                          component( r.x1 ), component( r.y1 ) )
   {
      PCL_ASSERT_RECT_SIZE();
   }

#ifdef __PCL_QT_INTERFACE
   GenericRectangle( const QRect& r )
      : GenericRectangle( component( r.left() ), component( r.top() ),
                          component( r.right()+1 ), component( r.bottom()+1 ) )
   {
      PCL_ASSERT_RECT_SIZE();
   }

   GenericRectangle( const QPoint& p0, const QPoint& p1 )
      : GenericRectangle( component( p0.x() ), component( p0.y() ),
                          component( p1.x() ), component( p1.y() ) )
   {
      PCL_ASSERT_RECT_SIZE();
   }
#endif

   /*!
    * Returns the left coordinate of this rectangle. This function returns the
    * value of the x0 data member.
    */
   component Left() const noexcept
   {
      return x0;
   }

   /*!
    * Returns the top coordinate of this rectangle. This function returns the
    * value of the y0 data member.
    */
   component Top() const noexcept
   {
      return y0;
   }

   /*!
    * Returns the right coordinate of this rectangle. This function returns the
    * value of the x1 data member.
    */
   component Right() const noexcept
   {
      return x1;
   }

   /*!
    * Returns the bottom coordinate of this rectangle. This function returns
    * the value of the y1 data member.
    */
   component Bottom() const noexcept
   {
      return y1;
   }

   /*!
    * Returns a point with the coordinates of the upper left (left-top) corner
    * of this rectangle.
    */
   point LeftTop() const noexcept
   {
      return point( pcl::Min( x0, x1 ), pcl::Min( y0, y1 ) );
   }

   /*!
    * A synonym for LeftTop().
    */
   point TopLeft() const noexcept
   {
      return LeftTop();
   }

   /*!
    * Returns a point with the coordinates of the upper right (right-top)
    * corner of this rectangle.
    */
   point RightTop() const noexcept
   {
      return point( pcl::Max( x0, x1 ), pcl::Min( y0, y1 ) );
   }

   /*!
    * A synonym for RightTop().
    */
   point TopRight() const noexcept
   {
      return RightTop();
   }

   /*!
    * Returns a point with the coordinates of the lower left (left-bottom)
    * corner of this rectangle.
    */
   point LeftBottom() const noexcept
   {
      return point( pcl::Min( x0, x1 ), pcl::Max( y0, y1 ) );
   }

   /*!
    * A synonym for LeftBottom().
    */
   point BottomLeft() const noexcept
   {
      return LeftBottom();
   }

   /*!
    * Returns a point with the coordinates of the lower right (right-bottom)
    * corner of this rectangle.
    */
   point RightBottom() const noexcept
   {
      return point( pcl::Max( x0, x1 ), pcl::Max( y0, y1 ) );
   }

   /*!
    * A synonym for RightBottom().
    */
   point BottomRight() const noexcept
   {
      return RightBottom();
   }

   /*!
    * Returns a point with the coordinates of the center of this rectangle.
    */
   point Center() const noexcept
   {
      return point( (x0 + x1)/2, (y0 + y1)/2 );
   }

   /*!
    * Returns the upper middle (center-top) point of this rectangle.
    */
   point CenterTop() const noexcept
   {
      return point( (x0 + x1)/2, pcl::Min( y0, y1 ) );
   }

   /*!
    * Returns the lower middle (center-bottom) point of this rectangle.
    */
   point CenterBottom() const noexcept
   {
      return point( (x0 + x1)/2, pcl::Min( y0, y1 ) );
   }

   /*!
    * Returns the left middle (center-left) point of this rectangle.
    */
   point CenterLeft() const noexcept
   {
      return point( pcl::Min( x0, x1 ), (y0 + y1)/2 );
   }

   /*!
    * Returns the right middle (center-right) point of this rectangle.
    */
   point CenterRight() const noexcept
   {
      return point( pcl::Min( x0, x1 ), (y0 + y1)/2 );
   }

   /*!
    * Returns the width of this rectangle. The returned value is the absolute
    * difference between the x1 and x0 data members.
    */
   component Width() const noexcept
   {
      return pcl::Abs( x1 - x0 );
   }

   /*!
    * Returns the height of this rectangle. The returned value is the absolute
    * difference between the y1 and y0 data members.
    */
   component Height() const noexcept
   {
      return pcl::Abs( y1 - y0 );
   }

   /*!
    * Returns the perimeter of this rectangle. The returned value is equal to
    * twice the width plus twice the height.
    */
   component Perimeter() const noexcept
   {
      component w = Width(), h = Height();
      return w+w+h+h;
   }

   /*!
    * Returns the Manhattan distance between two opposite corners of this
    * rectangle.
    *
    * The returned value is equal to the width plus the height.
    */
   component ManhattanDistance() const noexcept
   {
      return Width() + Height();
   }

   /*!
    * Returns the area of this rectangle. The returned value is equal to the
    * width multiplied by the height.
    */
   component Area() const noexcept
   {
      return pcl::Abs( (x1 - x0)*(y1 - y0) );
   }

   /*!
    * Returns the x coordinate of the central point of this rectangle. The
    * returned value is a \c double real value equal to 0.5*(x0 + x1).
    */
   double CenterX() const noexcept
   {
      return 0.5*(x0 + x1);
   }

   /*!
    * Returns the y coordinate of the central point of this rectangle. The
    * returned value is a \c double real value equal to 0.5*(y0 + y1).
    */
   double CenterY() const noexcept
   {
      return 0.5*(y0 + y1);
   }

   /*!
    * Returns the square of the diagonal of this rectangle. This is also the
    * square of the hypotenuse of the half-triangle defined by this rectangle.
    * The returned value is equal to the square of the width multiplied by the
    * square of the height.
    */
   double Hypot() const noexcept
   {
      double w = x1 - x0, h = y1 - y0;
      return w*w + h*h;
   }

   /*!
    * Returns the length of the diagonal of this rectangle, equal to the square
    * root of the Hypot() function.
    */
   double Diagonal() const noexcept
   {
      return pcl::Sqrt( Hypot() );
   }

   /*!
    * Returns true iff this rectangle defines a point in the plane.
    */
   bool IsPoint() const noexcept
   {
      return pcl::IsPoint( x0, y0, x1, y1 );
   }

   /*!
    * Returns true iff this rectangle defines a line.
    */
   bool IsLine() const noexcept
   {
      return pcl::IsLine( x0, y0, x1, y1 );
   }

   /*!
    * Returns true iff this rectangle defines a horizontal line.
    */
   bool IsHorizontalLine() const noexcept
   {
      return pcl::IsHorizontalLine( x0, y0, x1, y1 );
   }

   /*!
    * Returns true iff this rectangle defines a vertical line.
    */
   bool IsVerticalLine() const noexcept
   {
      return pcl::IsVerticalLine( x0, y0, x1, y1 );
   }

   /*!
    * Returns true iff this rectangle defines a point or a line.
    */
   bool IsPointOrLine() const noexcept
   {
      return pcl::IsPointOrLine( x0, y0, x1, y1 );
   }

   /*!
    * Returns true iff the coordinates of this object define a rectangle,
    * instead of a point or a line.
    */
   bool IsRect() const noexcept
   {
      return pcl::IsRect( x0, y0, x1, y1 );
   }

   /*!
    * Returns true iff this is a normal rectangle.
    */
   bool IsNormal() const noexcept
   {
      return pcl::IsNormalRect( x0, y0, x1, y1 );
   }

   /*!
    * Returns true iff this is an ordered rectangle.
    */
   bool IsOrdered() const noexcept
   {
      return pcl::IsOrderedRect( x0, y0, x1, y1 );
   }

   /*!
    * Orders the coordinates of this rectangle.
    */
   void Order() noexcept
   {
      pcl::OrderRect( x0, y0, x1, y1 );
   }

   /*!
    * Returns an ordered rectangle equivalent to this.
    */
   GenericRectangle Ordered() const noexcept
   {
      GenericRectangle r = *this;
      r.Order();
      return r;
   }

   /*!
    * Given the coordinates \a x and \a y of a point in the plane, this
    * function returns a clip code for the Cohen-Sutherland line clipping
    * algorithm.
    *
    * The returned value is a combination of flags defined in the Clip
    * namespace.
    *
    * \sa ClipCodeFast( T1, T1 ) const
    */
   template <typename T1>
   ClipFlags ClipCode( T1 x, T1 y ) const noexcept
   {
      ClipFlags clip; // defaults to zero = Clip::Inside

      if ( x0 <= x1 )
      {
         if ( x < x0 ) clip |= Clip::Left;
         if ( x > x1 ) clip |= Clip::Right;
      }
      else
      {
         if ( x < x1 ) clip |= Clip::Left;
         if ( x > x0 ) clip |= Clip::Right;
      }

      if ( y0 <= y1 )
      {
         if ( y < y0 ) clip |= Clip::Top;
         if ( y > y1 ) clip |= Clip::Bottom;
      }
      else
      {
         if ( y < y1 ) clip |= Clip::Top;
         if ( y > y0 ) clip |= Clip::Bottom;
      }

      return clip;
   }

   /*!
    * Given a point \a p in the plane, this function returns a clip code for
    * the Cohen-Sutherland line clipping algorithm.
    *
    * The returned value is a combination of flags defined in the Clip
    * namespace.
    *
    * \sa ClipCodeFast( const pcl::GenericPoint<>& ) const
    */
   template <typename T1>
   ClipFlags ClipCode( const pcl::GenericPoint<T1>& p ) const noexcept
   {
      return ClipCode( p.x, p.y );
   }

   /*!
    * Given the coordinates \a x and \a y of a point in the plane, this
    * function returns a clip code for the Cohen-Sutherland line clipping
    * algorithm.
    *
    * The returned value is a combination of flags defined in the Clip
    * namespace.
    *
    * \note This function assumes an ordered rectangle. That is, it requires
    * that the conditions x0 &le; x1 and y0 &le; y1 hold. Otherwise, this
    * function will return a wrong result.
    *
    * \sa ClipCode( T1, T1 ) const
    */
   template <typename T1>
   ClipFlags ClipCodeFast( T1 x, T1 y ) const noexcept
   {
      ClipFlags clip; // defaults to zero = Clip::Inside
      if ( x < x0 ) clip |= Clip::Left;
      if ( x > x1 ) clip |= Clip::Right;
      if ( y < y0 ) clip |= Clip::Top;
      if ( y > y1 ) clip |= Clip::Bottom;
      return clip;
   }

   /*!
    * Given a point \a p in the plane, this function returns a clip code for
    * the Cohen-Sutherland line clipping algorithm.
    *
    * The returned value is a combination of flags defined in the Clip
    * namespace.
    *
    * \note This function assumes an ordered rectangle. That is, it requires
    * that the conditions x0 &le; x1 and y0 &le; y1 hold. Otherwise, this
    * function will return a wrong result.
    *
    * \sa ClipCode( const pcl::GenericPoint<>& ) const
    */
   template <typename T1>
   ClipFlags ClipCodeFast( const pcl::GenericPoint<T1>& p ) const noexcept
   {
      return ClipCodeFast( p.x, p.y );
   }

   /*!
    * Computes the clipped line segment coordinates for the specified line
    * endpoints \a lx0, \a ly0 and \a lx1, \a ly1 against this rectangle.
    *
    * Returns true if the specified line segment intersects this rectangle or
    * is entirely contained by this rectangular region. If an endpoint lies
    * outside this rectangle, its coordinates will be replaced, on output, with
    * the intersection with the corresponding rectangle side or vertex.
    *
    * Returns false if the specified line segment lies outside this rectangle,
    * in which case the line endpoint coordinates won't be changed.
    *
    * This function implements the Cohen-Sutherland line clipping algorithm.
    *
    * \sa ClipLineFast( T1&, T1&, T1&, T1& ) const
    */
   template <typename T1>
   bool ClipLine( T1& lx0, T1& ly0, T1& lx1, T1& ly1 ) const noexcept
   {
      T x0_ = pcl::Min( x0, x1 );
      T y0_ = pcl::Min( y0, y1 );
      T x1_ = pcl::Max( x0, x1 );
      T y1_ = pcl::Max( y0, y1 );
      ClipFlags clip0 = ClipCode( lx0, ly0 );
      ClipFlags clip1 = ClipCode( lx1, ly1 );
      for ( ;; )
      {
         if ( !(clip0 | clip1) ) // both endpoints inside
            return true;
         if ( clip0 & clip1 )    // both endpoints at the same side
            return false;

         // Only one endpoint outside
         ClipFlags clipOut = clip0 ? clip0 : clip1;
         T1 x, y;
         if ( clipOut & Clip::Left )
         {
            x = x0_;
            y = ly1 - (lx1 - x0_) * (ly1 - ly0)/(lx1 - lx0);
         }
         else if ( clipOut & Clip::Top )
         {
            x = lx0 - (ly0 - y0_) * (lx1 - lx0)/(ly1 - ly0);
            y = y0_;
         }
         else if ( clipOut & Clip::Right )
         {
            x = x1_;
            y = ly0 - (lx0 - x1_) * (ly1 - ly0)/(lx1 - lx0);
         }
         else // if ( clipOut & Clip::Bottom )
         {
            x = lx1 - (ly1 - y1_) * (lx1 - lx0)/(ly1 - ly0);
            y = y1_;
         }

         if ( clipOut == clip0 )
         {
            lx0 = x;
            ly0 = y;
            clip0 = ClipCode( lx0, ly0 );
         }
         else
         {
            lx1 = x;
            ly1 = y;
            clip1 = ClipCode( lx1, ly1 );
         }
      }
   }

   /*!
    * Computes the clipped line segment coordinates for the specified line
    * endpoints \a p0 and \a p1 against this rectangle.
    *
    * See ClipLine( T1&, T1&, T1&, T1& ) const for a complete description of
    * this function.
    *
    * \sa ClipLineFast( pcl::GenericPoint<>&, pcl::GenericPoint<>& ) const
    */
   template <typename T1>
   bool ClipLine( pcl::GenericPoint<T1>& p0, pcl::GenericPoint<T1>& p1 ) const noexcept
   {
      return ClipLine( p0.x, p0.y, p1.x, p1.y );
   }

   /*!
    * Computes the clipped line segment coordinates for the specified line
    * endpoints \a lx0, \a ly0 and \a lx1, \a ly1 against this rectangle.
    *
    * Returns true if the specified line segment intersects this rectangle or
    * is entirely contained by this rectangular region. If an endpoint lies
    * outside this rectangle, its coordinates will be replaced, on output, with
    * the intersection with the corresponding rectangle side or vertex.
    *
    * Returns false if the specified line segment lies outside this rectangle,
    * in which case the line endpoint coordinates won't be changed.
    *
    * This function implements the Cohen-Sutherland line clipping algorithm.
    *
    * \note This function assumes an ordered rectangle. That is, it requires
    * that the conditions x0 &le; x1 and y0 &le; y1 hold. Otherwise, this
    * function will return a wrong result.
    *
    * \sa ClipLine( T1&, T1&, T1&, T1& ) const
    */
   template <typename T1>
   bool ClipLineFast( T1& lx0, T1& ly0, T1& lx1, T1& ly1 ) const noexcept
   {
      ClipFlags clip0 = ClipCodeFast( lx0, ly0 );
      ClipFlags clip1 = ClipCodeFast( lx1, ly1 );
      for ( ;; )
      {
         if ( !(clip0 | clip1) ) // both endpoints inside
            return true;
         if ( clip0 & clip1 )    // both endpoints at the same side
            return false;

         // Only one endpoint outside
         ClipFlags clipOut = clip0 ? clip0 : clip1;
         T1 x, y;
         if ( clipOut & Clip::Left )
         {
            x = x0;
            y = ly1 - (lx1 - x0) * (ly1 - ly0)/(lx1 - lx0);
         }
         else if ( clipOut & Clip::Top )
         {
            x = lx0 - (ly0 - y0) * (lx1 - lx0)/(ly1 - ly0);
            y = y0;
         }
         else if ( clipOut & Clip::Right )
         {
            x = x1;
            y = ly0 - (lx0 - x1) * (ly1 - ly0)/(lx1 - lx0);
         }
         else // if ( clipOut & Clip::Bottom )
         {
            x = lx1 - (ly1 - y1) * (lx1 - lx0)/(ly1 - ly0);
            y = y1;
         }

         if ( clipOut == clip0 )
         {
            lx0 = x;
            ly0 = y;
            clip0 = ClipCodeFast( lx0, ly0 );
         }
         else
         {
            lx1 = x;
            ly1 = y;
            clip1 = ClipCodeFast( lx1, ly1 );
         }
      }
   }

   /*!
    * Computes the clipped line segment coordinates for the specified line
    * endpoints \a p0 and \a p1 against this rectangle.
    *
    * See ClipLineFast( T1&, T1&, T1&, T1& ) const for a complete description of
    * this function.
    *
    * \note This function assumes an ordered rectangle. That is, it requires
    * that the conditions x0 &le; x1 and y0 &le; y1 hold. Otherwise, this
    * function will return a wrong result.
    *
    * \sa ClipLine( pcl::GenericPoint<>&, pcl::GenericPoint<>& ) const
    */
   template <typename T1>
   bool ClipLineFast( pcl::GenericPoint<T1>& p0, pcl::GenericPoint<T1>& p1 ) const noexcept
   {
      return ClipLineFast( p0.x, p0.y, p1.x, p1.y );
   }

   /*!
    * Returns true iff this rectangle includes a point specified by its
    * separate \a x and \a y coordinates.
    */
   template <typename T1>
   bool Includes( T1 x, T1 y ) const noexcept
   {
      return ((x0 < x1) ? (x >= x0 && x <= x1) : (x >= x1 && x <= x0)) &&
             ((y0 < y1) ? (y >= y0 && y <= y1) : (y >= y1 && y <= y0));
   }

   /*!
    * Returns true iff this rectangle includes a point \a p.
    */
   template <typename T1>
   bool Includes( const pcl::GenericPoint<T1>& p ) const noexcept
   {
      return Includes( p.x, p.y );
   }

   /*!
    * Returns true iff this rectangle completely includes a rectangle \a r.
    */
   template <typename T1>
   bool Includes( const GenericRectangle<T1>& r ) const noexcept
   {
      return Includes( r.x0, r.y0 ) && Includes( r.x1, r.y1 );
   }

#ifdef __PCL_QT_INTERFACE
   bool Includes( const QPoint& p ) const noexcept
   {
      return Includes( p.x(), p.y() );
   }

   bool Includes( const QRect& r ) const noexcept
   {
      return Includes( r.left(), r.top() ) && Includes( r.right()+1, r.bottom()+1 );
   }
#endif

   /*!
    * Returns true iff this rectangle includes a point specified by its
    * separate \a x and \a y coordinates.
    *
    * \note This function assumes an ordered rectangle. That is, it requires
    * that the conditions x0 &le; x1 and y0 &le; y1 hold. Otherwise, this
    * function will return a wrong result.
    */
   template <typename T1>
   bool IncludesFast( T1 x, T1 y ) const noexcept
   {
      return x >= x0 && y >= y0 && x <= x1 && y <= y1;
   }

   /*!
    * Returns true iff this rectangle includes a point \a p.
    *
    * \note This function assumes an ordered rectangle. That is, it requires
    * that the conditions x0 &le; x1 and y0 &le; y1 hold. Otherwise, this
    * function will return a wrong result.
    */
   template <typename T1>
   bool IncludesFast( const pcl::GenericPoint<T1>& p ) const noexcept
   {
      return IncludesFast( p.x, p.y );
   }

   /*!
    * Returns true iff this rectangle intersects a rectangle specified by its
    * individual coordinates.
    *
    * \param left,top      Upper left corner coordinates of a rectangle to test
    *             for intersection.
    *
    * \param right,bottom  Lower right corner coordinates of a rectangle to test
    *             for intersection.
    */
   template <typename T1>
   bool Intersects( T1 left, T1 top, T1 right, T1 bottom ) const noexcept
   {
      OrderRect( left, top, right, bottom );
      return ((x0 < x1) ? (right >= x0 && left <= x1) : (right >= x1 && left <= x0)) &&
             ((y0 < y1) ? (bottom >= y0 && top <= y1) : (bottom >= y1 && top <= y0));
   }

   /*!
    * Returns true iff this rectangle intersects a rectangle \a r.
    */
   template <typename T1>
   bool Intersects( const pcl::GenericRectangle<T1>& r ) const noexcept
   {
      return Intersects( r.x0, r.y0, r.x1, r.y1 );
   }

#ifdef __PCL_QT_INTERFACE
   bool Intersects( const QRect& r ) const noexcept
   {
      return Intersects( r.left(), r.top(), r.right()+1, r.bottom()+1 );
   }
#endif

   /*!
    * Returns true iff this rectangle intersects a rectangle specified by its
    * individual coordinates.
    *
    * \param left,top      Upper left corner coordinates of a rectangle to test
    *             for intersection.
    *
    * \param right,bottom  Lower right corner coordinates of a rectangle to test
    *             for intersection.
    *
    * For a valid result, this function assumes the following conditions:
    *
    * \li The specified set \a left, \a top, \a right and \a bottom must define
    * an ordered rectangle. That is, the conditions \a left &le; \a right and
    * \a top &le; \a bottom must hold.
    *
    * \li This rectangle must be ordered. That is, the conditions x0 &le; x1
    * and y0 &le; y1 must hold.
    */
   template <typename T1>
   bool IntersectsFast( T1 left, T1 top, T1 right, T1 bottom ) const noexcept
   {
      return right >= x0 && left <= x1 && bottom >= y0 && top <= y1;
   }

   /*!
    * Returns true iff this rectangle intersects a rectangle \a r.
    *
    * To give a valid result, this function assumes that both this and the
    * specified object \a r are ordered rectangles.
    */
   template <typename T1>
   bool IntersectsFast( const pcl::GenericRectangle<T1>& r ) const noexcept
   {
      return IntersectsFast( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Causes this rectangle to include a given rectangle \a r, by adjusting its
    * coordinates as necessary.
    */
   template <typename T1>
   void Unite( const GenericRectangle<T1>& r ) noexcept
   {
      Unite( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Causes this rectangle to include a rectangle specified by its individual
    * coordinates.
    *
    * \param left,top      Upper left corner coordinates of a rectangle that
    *                      will be included by this rectangle.
    *
    * \param right,bottom  Lower right corner coordinates of a rectangle that
    *                      will be included by this rectangle.
    */
   template <typename T1>
   void Unite( T1 left, T1 top, T1 right, T1 bottom ) noexcept
   {
      if ( right < left )
         Swap( left, right );
      if ( bottom < top )
         Swap( top, bottom );

      if ( x0 <= x1 )
      {
         x0 = pcl::Min( x0, component( left ) );
         x1 = pcl::Max( x1, component( right ) );
      }
      else
      {
         x0 = pcl::Max( x0, component( right ) );
         x1 = pcl::Min( x1, component( left ) );
      }

      if ( y0 <= y1 )
      {
         y0 = pcl::Min( y0, component( top ) );
         y1 = pcl::Max( y1, component( bottom ) );
      }
      else
      {
         y0 = pcl::Max( y0, component( bottom ) );
         y1 = pcl::Min( y1, component( top ) );
      }
   }

   /*!
    * Causes this rectangle to include a given rectangle \a r, by adjusting its
    * coordinates as necessary.
    *
    * \note To produce a valid result, this function assumes that both this and
    * the specified object \a r are ordered rectangles.
    */
   template <typename T1>
   void UniteFast( const GenericRectangle<T1>& r ) noexcept
   {
      UniteFast( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Causes this rectangle to include a rectangle specified by its individual
    * coordinates.
    *
    * \param left,top      Upper left corner coordinates of a rectangle that
    *                      will be included by this rectangle.
    *
    * \param right,bottom  Lower right corner coordinates of a rectangle that
    *                      will be included by this rectangle.
    *
    * For a valid result, this function assumes the following conditions:
    *
    * \li The specified set \a left, \a top, \a right and \a bottom must define
    * an ordered rectangle. That is, the conditions \a left &le; \a right and
    * \a top &le; \a bottom must hold.
    *
    * \li This rectangle must be ordered. That is, the conditions x0 &le; x1
    * and y0 &le; y1 must hold.
    */
   template <typename T1>
   void UniteFast( T1 left, T1 top, T1 right, T1 bottom ) noexcept
   {
      x0 = pcl::Min( x0, component( left ) );
      y0 = pcl::Min( y0, component( top ) );
      x1 = pcl::Max( x1, component( right ) );
      y1 = pcl::Max( y1, component( bottom ) );
   }

   /*!
    * Returns a rectangle that includes this one and another rectangle \a r.
    */
   template <typename T1>
   GenericRectangle Union( const GenericRectangle<T1>& r ) const noexcept
   {
      GenericRectangle r1 = *this;
      r1.Unite( r );
      return r1;
   }

   /*!
    * Returns a rectangle that includes this one and another rectangle \a r.
    *
    * \note To give a valid result, this function assumes that both this and
    * the specified object \a r are ordered rectangles.
    */
   template <typename T1>
   GenericRectangle UnionFast( const GenericRectangle<T1>& r ) const noexcept
   {
      GenericRectangle r1 = *this;
      r1.UniteFast( r );
      return r1;
   }

   /*!
    * Causes this rectangle to include a given rectangle \a r, by adjusting
    * coordinates as necessary. Returns a reference to this rectangle.
    */
   template <typename T1>
   GenericRectangle& operator |=( const GenericRectangle<T1>& r ) noexcept
   {
      Unite( r );
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   void Unite( const QRect& r )
   {
      Unite( r.left(), r.top(), r.right()+1, r.bottom()+1 );
   }

   GenericRectangle Union( const QRect& r ) const noexcept
   {
      GenericRectangle r1 = *this;
      r1.Unite( r );
      return r1;
   }

   GenericRectangle& operator |=( const QRect& r ) noexcept
   {
      Unite( r );
      return *this;
   }
#endif

   /*!
    * Causes this rectangle to be equal to its intersection with a given
    * rectangle \a r.
    *
    * Returns true iff the resulting intersection is a nonempty rectangle, i.e.
    * iff this->x0 != this->x1 and this->y0 != this->y1 after calling this
    * function.
    */
   template <typename T1>
   bool Intersect( const GenericRectangle<T1>& r ) noexcept
   {
      return Intersect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Causes this rectangle to be equal to its intersection with a rectangle
    * specified by its individual coordinates.
    *
    * \param left,top      Upper left corner coordinates of a rectangle that
    *                      will intersect this rectangle.
    *
    * \param right,bottom  Lower right corner coordinates of a rectangle that will
    *                      intersect this rectangle.
    *
    * Returns true iff the resulting intersection is a nonempty rectangle, i.e.
    * iff this->x0 != this->x1 and this->y0 != this->y1 after calling this
    * function.
    */
   template <typename T1>
   bool Intersect( T1 left, T1 top, T1 right, T1 bottom ) noexcept
   {
      if ( right < left )
         Swap( left, right );
      if ( bottom < top )
         Swap( top, bottom );

      if ( x0 <= x1 )
      {
         x0 = pcl::Max( x0, component( left ) );
         x1 = pcl::Min( x1, component( right ) );
      }
      else
      {
         x0 = pcl::Min( x0, component( right ) );
         x1 = pcl::Max( x1, component( left ) );
      }

      if ( y0 <= y1 )
      {
         y0 = pcl::Max( y0, component( top ) );
         y1 = pcl::Min( y1, component( bottom ) );
      }
      else
      {
         y0 = pcl::Min( y0, component( bottom ) );
         y1 = pcl::Max( y1, component( top ) );
      }

      return IsRect();
   }

   /*!
    * Causes this rectangle to be equal to its intersection with a given
    * rectangle \a r.
    *
    * Returns true iff the resulting intersection is a nonempty rectangle, i.e.
    * iff this->x0 != this->x1 and this->y0 != this->y1 after calling this
    * function.
    *
    * \note To produce a valid result, this function assumes that both this and
    * the specified object \a r are ordered rectangles.
    */
   template <typename T1>
   bool IntersectFast( const GenericRectangle<T1>& r ) noexcept
   {
      return IntersectFast( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Causes this rectangle to be equal to its intersection with a rectangle
    * specified by its individual coordinates.
    *
    * \param left,top      Upper left corner coordinates of a rectangle that
    *                      will intersect this rectangle.
    *
    * \param right,bottom  Lower right corner coordinates of a rectangle that will
    *                      intersect this rectangle.
    *
    * Returns true iff the resulting intersection is a nonempty rectangle, i.e.
    * iff this->x0 != this->x1 and this->y0 != this->y1 after calling this
    * function.
    *
    * For a valid result, this function assumes the following conditions:
    *
    * \li The specified set \a left, \a top, \a right and \a bottom must define
    * an ordered rectangle. That is, the conditions \a left &le; \a right and
    * \a top &le; \a bottom must hold.
    *
    * \li This rectangle must be ordered. That is, the conditions x0 &le; x1
    * and y0 &le; y1 must hold.
    */
   template <typename T1>
   bool IntersectFast( T1 left, T1 top, T1 right, T1 bottom ) noexcept
   {
      x0 = pcl::Max( x0, component( left ) );
      y0 = pcl::Max( y0, component( top ) );
      x1 = pcl::Min( x1, component( right ) );
      y1 = pcl::Min( y1, component( bottom ) );
      return IsRect();
   }

   /*!
    * Returns a rectangle equal to the intersection of this rectangle and
    * another rectangle \a r.
    */
   template <typename T1>
   GenericRectangle Intersection( const GenericRectangle<T1>& r ) const noexcept
   {
      GenericRectangle r1 = *this;
      (void)r1.Intersect( r );
      return r1;
   }

   /*!
    * Returns a rectangle equal to the intersection of this rectangle and
    * another rectangle \a r.
    *
    * \note To give a valid result, this function assumes that both this and
    * the specified object \a r are ordered rectangles.
    */
   template <typename T1>
   GenericRectangle IntersectionFast( const GenericRectangle<T1>& r ) const noexcept
   {
      GenericRectangle r1 = *this;
      (void)r1.IntersectFast( r );
      return r1;
   }

   /*!
    * Causes this rectangle to be equal to its intersection with another
    * rectangle \a r. Returns a reference to this rectangle.
    */
   template <typename T1>
   GenericRectangle& operator &=( const GenericRectangle<T1>& r ) noexcept
   {
      Intersect( r );
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   bool Intersect( const QRect& r ) noexcept
   {
      return Intersect( r.left(), r.top(), r.right()+1, r.bottom()+1 );
   }

   GenericRectangle Intersection( const QRect& r ) const noexcept
   {
      GenericRectangle r1 = *this;
      (void)r1.Intersect( r );
      return r1;
   }

   GenericRectangle& operator &=( const QRect& r ) noexcept
   {
      Intersect( r );
      return *this;
   }
#endif

   /*!
    * Sets the four coordinates of this rectangle to the specified values.
    *
    * \param left,top      New upper left corner coordinates.
    * \param right,bottom  New lower right corner coordinates.
    */
   template <typename T1>
   void Set( T1 left, T1 top, T1 right, T1 bottom ) noexcept
   {
      x0 = component( left );
      y0 = component( top );
      x1 = component( right );
      y1 = component( bottom );
   }

   /*!
    * Moves this rectangle to the specified location, by setting its upper left
    * corner coordinates to those of a given point \a p.
    *
    * The coordinates of the lower right corner of this rectangle are
    * readjusted to keep its width and height unmodified.
    */
   template <typename T1>
   void MoveTo( const pcl::GenericPoint<T1>& p ) noexcept
   {
      MoveTo( p.x, p.y );
   }

   /*!
    * Moves this rectangle to the specified location, by setting its upper left
    * corner coordinates equal to the given \a x and \a y coordinates.
    *
    * The coordinates of the lower right corner of this rectangle are
    * readjusted to keep its width and height unmodified.
    */
   template <typename T1>
   void MoveTo( T1 x, T1 y ) noexcept
   {
      component dx = x1 - x0, dy = y1 - y0;
      x0 = component( x );
      y0 = component( y );
      x1 = x0 + dx;
      y1 = y0 + dy;
   }

#ifdef __PCL_QT_INTERFACE
   void MoveTo( const QPoint& p ) noexcept
   {
      MoveTo( p.x(), p.y() );
   }
#endif

   /*!
    * Returns a rectangle \a r equal to this rectangle moved to the specified
    * location:
    *
    * \code GenericRectangle r( *this ); r.MoveTo( p ); \endcode
    */
   template <typename T1>
   GenericRectangle MovedTo( const pcl::GenericPoint<T1>& p ) const noexcept
   {
      GenericRectangle r( *this );
      r.MoveTo( p );
      return r;
   }

   /*!
    * Returns a rectangle \a r equal to this rectangle moved to the specified
    * location:
    *
    * \code GenericRectangle<T> r( *this ); r.MoveTo( x, y ); \endcode
    */
   template <typename T1>
   GenericRectangle MovedTo( T1 x, T1 y ) const noexcept
   {
      GenericRectangle r( *this );
      r.MoveTo( x, y );
      return r;
   }

   /*!
    * Moves this rectangle relative to its current position, by applying the
    * coordinates of a point \a d as increments in the X and Y directions.
    */
   template <typename T1>
   void MoveBy( const pcl::GenericPoint<T1>& d ) noexcept
   {
      MoveBy( d.x, d.y );
   }

   /*!
    * Moves this rectangle relative to its current position, by applying the
    * specified \a dx and \a dy increments to its coordinates in the X and Y
    * directions, respectively.
    */
   template <typename T1>
   void MoveBy( T1 dx, T1 dy ) noexcept
   {
      x0 += component( dx );
      y0 += component( dy );
      x1 += component( dx );
      y1 += component( dy );
   }

   /*!
    * Moves this rectangle relative to its current position, by applying the
    * specified \a dxy increment to its four coordinates in the X and Y
    * directions.
    *
    * This function is functionally equivalent to:
    *
    * \code MoveBy( dxy, dxy ); \endcode
    */
   template <typename T1>
   void MoveBy( T1 dxy ) noexcept
   {
      x0 += component( dxy );
      y0 += component( dxy );
      x1 += component( dxy );
      y1 += component( dxy );
   }

#ifdef __PCL_QT_INTERFACE
   void MoveBy( const QPoint& p ) noexcept
   {
      MoveBy( p.x(), p.y() );
   }
#endif

   /*!
    * Returns a rectangle \a r equal to this rectangle moved relative to its
    * current position:
    *
    * \code GenericRectangle<T> r( *this ); r.MoveBy( d ); \endcode
    */
   template <typename T1>
   GenericRectangle MovedBy( const pcl::GenericPoint<T1>& d ) const noexcept
   {
      GenericRectangle r( *this );
      r.MoveBy( d );
      return r;
   }

   /*!
    * Returns a rectangle \a r equal to this rectangle moved relative to its
    * current position:
    *
    * \code GenericRectangle<T> r( *this ); r.MoveBy( dx, dy ); \endcode
    */
   template <typename T1>
   GenericRectangle MovedBy( T1 dx, T1 dy ) const noexcept
   {
      GenericRectangle r( *this );
      r.MoveBy( dx, dy );
      return r;
   }

   /*!
    * Changes the size of this rectangle to the specified width \a w and height
    * \a h, respectively.
    *
    * This function does not vary the current order of rectangle coordinates.
    * If a coordinate of the upper left corner is less than or equal to its
    * bottom right counterpart, the bottom right coordinate is modified.
    * Inversely, if the bottom right coordinate is less than the upper left
    * one, the upper left coordinate is modified.
    */
   template <typename T1>
   void ResizeTo( T1 w, T1 h ) noexcept
   {
      if ( x0 <= x1 )
         x1 = x0 + component( w );
      else
         x0 = x1 + component( w );

      if ( y0 <= y1 )
         y1 = y0 + component( h );
      else
         y0 = y1 + component( h );
   }

   /*!
    * Returns a rectangle \a r equal to this rectangle resized to the specified
    * dimensions:
    *
    * \code GenericRectangle<T> r( *this ); r.ResizeTo( w, h ); \endcode
    */
   template <typename T1>
   GenericRectangle ResizedTo( T1 w, T1 h ) const noexcept
   {
      GenericRectangle r( *this );
      r.ResizeTo( w, h );
      return r;
   }

   /*!
    * Changes the size of this rectangle relative to its current dimensions, by
    * applying the specified \a dw and \a dh increments to its width and
    * height, respectively.
    *
    * This function does not vary the current order of rectangle coordinates.
    * If a coordinate of the upper left corner is less than or equal to its
    * bottom right counterpart, the bottom right coordinate is modified.
    * Inversely, if the bottom right coordinate is less than the upper left
    * one, the upper left coordinate is modified.
    */
   template <typename T1>
   void ResizeBy( T1 dw, T1 dh ) noexcept
   {
      if ( x0 <= x1 )
         x1 += component( dw );
      else
         x0 += component( dw );

      if ( y0 <= y1 )
         y1 += component( dh );
      else
         y0 += component( dh );
   }

   /*!
    * Returns a rectangle \a r equal to this rectangle resized relative to its
    * current dimensions:
    *
    * \code GenericRectangle<T> r( *this ); r.ResizeBy( dw, dh ); \endcode
    */
   template <typename T1>
   GenericRectangle ResizedBy( T1 dw, T1 dh ) const noexcept
   {
      GenericRectangle r( *this );
      r.ResizeBy( dw, dh );
      return r;
   }

   /*!
    * Sets the width of this rectangle to the specified value \a w.
    *
    * This function does not vary the current order of horizontal rectangle
    * coordinates. If the x0 rectangle coordinate is less than or equal to x1,
    * the x1 coordinate is modified. Inversely, if the x1 coordinate is less
    * than x0, then x0 is modified.
    */
   template <typename T1>
   void SetWidth( T1 w ) noexcept
   {
      if ( x0 <= x1 )
         x1 = x0 + component( w );
      else
         x0 = x1 + component( w );
   }

   /*!
    * Sets the height of this rectangle to the specified value \a h.
    *
    * This function does not vary the current order of vertical rectangle
    * coordinates. If the y0 rectangle coordinate is less than or equal to y1,
    * the y1 coordinate is modified. Inversely, if the y1 coordinate is less
    * than y0, then y0 is modified.
    */
   template <typename T1>
   void SetHeight( T1 h ) noexcept
   {
      if ( y0 <= y1 )
         y1 = y0 + component( h );
      else
         y0 = y1 + component( h );
   }

   /*!
    * Inflates this rectangle by the specified \a dx and \a dy increments on
    * the X and Y axes respectively. Subtracts \a dx and \a dy to the upper
    * left corner, and adds them to the bottom right corner.
    */
   template <typename T1>
   void InflateBy( T1 dx, T1 dy ) noexcept
   {
      if ( x1 < x0 )
         dx = -dx;
      if ( y1 < y0 )
         dy = -dy;
      x0 -= dx;
      y0 -= dy;
      x1 += dx;
      y1 += dy;
   }

   /*!
    * Inflates this rectangle by the specified \a d increment on both axes.
    * Subtracts \a d to the upper left corner, and adds it to the bottom right
    * corner.
    */
   template <typename T1>
   void InflateBy( T1 d ) noexcept
   {
      if ( x0 <= x1 )
         x0 -= d, x1 += d;
      else
         x0 += d, x1 -= d;

      if ( y0 <= y1 )
         y0 -= d, y1 += d;
      else
         y0 += d, y1 -= d;
   }

   /*!
    * Returns a rectangle equivalent to this rectangle inflated by the
    * specified \a dx and \a dy increments.
    */
   template <typename T1>
   GenericRectangle InflatedBy( T1 dx, T1 dy ) const noexcept
   {
      GenericRectangle r( *this );
      r.InflateBy( dx, dy );
      return r;
   }

   /*!
    * Returns a rectangle equivalent to this rectangle inflated by the
    * specified \a d increments on both axes.
    */
   template <typename T1>
   GenericRectangle InflatedBy( T1 d ) const noexcept
   {
      GenericRectangle r( *this );
      r.InflateBy( d );
      return r;
   }

   /*!
    * Inflates this rectangle by the specified factors \a kx and \a ky with
    * respect to the current width and height, respectively. This function is
    * equivalent to:
    *
    * \code InflateBy( kx*Width()/2, ky*Height()/2 ); \endcode
    */
   void InflateByFactors( double kx, double ky ) noexcept
   {
      InflateBy( kx*Width()/2, ky*Height()/2 );
   }

   /*!
    * Inflates this rectangle by the specified factor \a k with respect to the
    * current width and height. This function is equivalent to:
    *
    * \code InflateBy( k*Width()/2, k*Height()/2 ); \endcode
    */
   void InflateByFactor( double k ) noexcept
   {
      InflateBy( k*Width()/2, k*Height()/2 );
   }

   /*!
    * Returns a rectangle equivalent to this rectangle inflated by the
    * specified factors \a kx and \a ky with respect to the current width and
    * height, respectively. See InflateByFactors() for more information.
    */
   GenericRectangle InflatedByFactors( double kx, double ky ) noexcept
   {
      GenericRectangle r( *this );
      r.InflateByFactors( kx, ky );
      return r;
   }

   /*!
    * Returns a rectangle equivalent to this rectangle inflated by the
    * specified factor \a k with respect to the current width and height. See
    * InflateByFactor() for more information.
    */
   GenericRectangle InflatedByFactor( double k ) noexcept
   {
      GenericRectangle r( *this );
      r.InflateByFactor( k );
      return r;
   }

   /*!
    * Shrinks this rectangle by the specified \a dx and \a dy increments on the
    * X and Y axes respectively. Adds \a dx and \a dy to the upper left corner,
    * and subtracts them to the bottom right corner.
    */
   template <typename T1>
   void DeflateBy( T1 dx, T1 dy ) noexcept
   {
      if ( x1 < x0 )
         dx = -dx;
      if ( y1 < y0 )
         dy = -dy;
      x0 += dx;
      y0 += dy;
      x1 -= dx;
      y1 -= dy;
   }

   /*!
    * Shrinks this rectangle by the specified \a d increment on both axes. Adds
    * \a d to the upper left corner, and subtracts it to the bottom right
    * corner.
    */
   template <typename T1>
   void DeflateBy( T1 d ) noexcept
   {
      if ( x0 <= x1 )
         x0 += d, x1 -= d;
      else
         x0 -= d, x1 += d;

      if ( y0 <= y1 )
         y0 += d, y1 -= d;
      else
         y0 -= d, y1 += d;
   }

   /*!
    * Returns a rectangle equivalent to this rectangle shrunk by the specified
    * \a dx and \a dy increments.
    */
   template <typename T1>
   GenericRectangle DeflatedBy( T1 dx, T1 dy ) const noexcept
   {
      GenericRectangle r( *this );
      r.DeflateBy( dx, dy );
      return r;
   }

   /*!
    * Returns a rectangle equivalent to this rectangle shrunk by the specified
    * \a d increment on both axes.
    */
   template <typename T1>
   GenericRectangle DeflatedBy( T1 d ) const noexcept
   {
      GenericRectangle r( *this );
      r.DeflateBy( d );
      return r;
   }

   /*!
    * Returns a rectangle \a r equal to this rectangle resized horizontally to
    * the specified width \a w:
    *
    * \code GenericRectangle<T> r( *this ); r.SetWidth( w ); \endcode
    */
   template <typename T1>
   GenericRectangle WidthSetTo( T1 w ) const noexcept
   {
      GenericRectangle r( *this );
      r.SetWidth( w );
      return r;
   }

   /*!
    * Returns a rectangle \a r equal to this rectangle resized vertically to
    * the specified height \a h:
    *
    * \code GenericRectangle<T> r( *this ); r.SetHeight( h ); \endcode
    */
   template <typename T1>
   GenericRectangle HeightSetTo( T1 h ) const noexcept
   {
      GenericRectangle r( *this );
      r.SetHeight( h );
      return r;
   }

   /*!
    * Rotates this rectangle in the plane by the specified \a angle in radians,
    * with respect to a center of rotation given by its coordinates \a xc and
    * \a yc.
    */
   template <typename T1, typename T2>
   void Rotate( T1 angle, T2 xc, T2 yc ) noexcept
   {
      T1 sa, ca; pcl::SinCos( angle, sa, ca );
      pcl::Rotate( x0, y0, sa, ca, xc, yc );
      pcl::Rotate( x1, y1, sa, ca, xc, yc );
   }

   /*!
    * Rotates this rectangle in the plane by the specified \a angle in radians,
    * with respect to the specified \a center of rotation.
    */
   template <typename T1, typename T2>
   void Rotate( T1 angle, const GenericPoint<T2>& center ) noexcept
   {
      Rotate( angle, center.x, center.y );
   }

   /*!
    * Rotates this rectangle in the plane by the specified angle, given by its
    * sine and cosine, \a sa and \a ca respectively, with respect to a center
    * of rotation given by its coordinates \a xc and \a yc.
    */
   template <typename T1, typename T2>
   void Rotate( T1 sa, T1 ca, T2 xc, T2 yc ) noexcept
   {
      pcl::Rotate( x0, y0, sa, ca, xc, yc );
      pcl::Rotate( x1, y1, sa, ca, xc, yc );
   }

   /*!
    * Rotates this rectangle in the plane by the specified angle, given by its
    * sine and cosine, \a sa and \a ca respectively, with respect to the
    * specified \a center of rotation.
    */
   template <typename T1, typename T2>
   void Rotate( T1 sa, T1 ca, const GenericPoint<T2>& center ) noexcept
   {
      Rotate( sa, ca, center.x, center.y );
   }

   /*!
    * Returns a rectangle whose coordinates are the coordinates of this object
    * rotated in the plane by the specified \a angle in radians, with respect
    * to a center of rotation given by its coordinates \a xc and \a yc.
    */
   template <typename T1, typename T2>
   GenericRectangle Rotated( T1 angle, T2 xc, T2 yc ) const noexcept
   {
      GenericRectangle r( *this );
      r.Rotate( angle, xc, yc );
      return r;
   }

   /*!
    * Returns a rectangle whose coordinates are the coordinates of this object
    * rotated in the plane by the specified \a angle in radians, with respect
    * to the specified \a center of rotation.
    */
   template <typename T1, typename T2>
   GenericRectangle Rotated( T1 angle, const GenericPoint<T2>& center ) const noexcept
   {
      GenericRectangle r( *this );
      r.Rotate( angle, center );
      return r;
   }

   /*!
    * Returns a rectangle whose coordinates are the coordinates of this object
    * rotated in the plane by the specified angle given by its sine and cosine,
    * \a sa and \a ca respectively, with respect to a center of rotation given
    * by its coordinates \a xc and \a yc.
    */
   template <typename T1, typename T2>
   GenericRectangle Rotated( T1 sa, T1 ca, T2 xc, T2 yc ) const noexcept
   {
      GenericRectangle r( *this );
      r.Rotate( sa, ca, xc, yc );
      return r;
   }

   /*!
    * Returns a rectangle whose coordinates are the coordinates of this object
    * rotated in the plane by the specified angle given by its sine and cosine,
    * \a sa and \a ca respectively, with respect to the specified \a center of
    * rotation.
    */
   template <typename T1, typename T2>
   GenericRectangle Rotated( T1 sa, T1 ca, const GenericPoint<T2>& center ) const noexcept
   {
      GenericRectangle r( *this );
      r.Rotate( sa, ca, center );
      return r;
   }

   /*!
    * Rounds the coordinates of this rectangle to their corresponding nearest
    * integer coordinates.
    */
   void Round() noexcept
   {
      x0 = component( pcl::Round( double( x0 ) ) );
      y0 = component( pcl::Round( double( y0 ) ) );
      x1 = component( pcl::Round( double( x1 ) ) );
      y1 = component( pcl::Round( double( y1 ) ) );
   }

   /*!
    * Rounds the coordinates of this rectangle to \a n fractional digits
    * (\a n >= 0).
    */
   void Round( int n ) noexcept
   {
      PCL_PRECONDITION( n >= 0 )
      if ( n < 0 )
         n = 0;
      x0 = component( pcl::Round( double( x0 ), n ) );
      y0 = component( pcl::Round( double( y0 ), n ) );
      x1 = component( pcl::Round( double( x1 ), n ) );
      y1 = component( pcl::Round( double( y1 ), n ) );
   }

   /*!
    * Returns a rectangle whose coordinates are the coordinates of this object
    * rounded to their nearest integers.
    */
   GenericRectangle Rounded() const noexcept
   {
      return GenericRectangle( component( pcl::Round( double( x0 ) ) ), component( pcl::Round( double( y0 ) ) ),
                               component( pcl::Round( double( x1 ) ) ), component( pcl::Round( double( y1 ) ) ) );
   }

   /*!
    * Returns a rectangle whose coordinates are the coordinates of this object
    * rounded to \a n fractional digits (\a n >= 0).
    */
   GenericRectangle Rounded( int n ) const noexcept
   {
      PCL_PRECONDITION( n >= 0 )
      return GenericRectangle( component( pcl::Round( double( x0 ), n ) ), component( pcl::Round( double( y0 ), n ) ),
                               component( pcl::Round( double( x1 ), n ) ), component( pcl::Round( double( y1 ), n ) ) );
   }

   /*!
    * Returns a rectangle of integer template type whose coordinates are the
    * coordinates of this object rounded to their nearest integers.
    */
   GenericRectangle<int> RoundedToInt() const noexcept
   {
      return GenericRectangle<int>( pcl::RoundInt( double( x0 ) ), pcl::RoundInt( double( y0 ) ),
                                    pcl::RoundInt( double( x1 ) ), pcl::RoundInt( double( y1 ) ) );
   }

   /*!
    * Integer truncation of coordinates. Sets the coordinates x0, y0, x1, y1 of
    * this rectangle to the nearest integer coordinates a, b, c, d such that
    * a <= x0, b <= y0, c <= x1, d <= y1.
    */
   void Truncate() noexcept
   {
      x0 = component( pcl::Trunc( double( x0 ) ) );
      y0 = component( pcl::Trunc( double( y0 ) ) );
      x1 = component( pcl::Trunc( double( x1 ) ) );
      y1 = component( pcl::Trunc( double( y1 ) ) );
   }

   /*!
    * Integer truncation of coordinates. Returns a rectangle whose coordinates
    * are the coordinates x0, y0, x1, y1 of this rectangle, truncated to their
    * nearest integer coordinates a, b, c, d such that a <= x0, b <= y0,
    * c <= x1, d <= y1.
    */
   GenericRectangle Truncated() const noexcept
   {
      return GenericRectangle( component( pcl::Trunc( double( x0 ) ) ), component( pcl::Trunc( double( y0 ) ) ),
                               component( pcl::Trunc( double( x1 ) ) ), component( pcl::Trunc( double( y1 ) ) ) );
   }

   /*!
    * Integer truncation of coordinates. Returns a rectangle of integer
    * template type whose coordinates are the coordinates x0, y0, x1, y1 of
    * this rectangle truncated to the nearest integer coordinates a, b, c, d
    * such that a <= x0, b <= y0, c <= x1, d <= y1.
    */
   GenericRectangle<int> TruncatedToInt() const noexcept
   {
      return GenericRectangle<int>( pcl::TruncInt( double( x0 ) ), pcl::TruncInt( double( y0 ) ),
                                    pcl::TruncInt( double( x1 ) ), pcl::TruncInt( double( y1 ) ) );
   }

   /*!
    * Assigns a rectangle \a r to this rectangle. Returns a reference to this
    * rectangle.
    */
   template <typename T1>
   GenericRectangle& operator =( const GenericRectangle<T1>& r ) noexcept
   {
      x0 = component( r.x0 );
      y0 = component( r.y0 );
      x1 = component( r.x1 );
      y1 = component( r.y1 );
      return *this;
   }

   /*!
    * Assigns a point \a p to this rectangle. Returns a reference to this
    * rectangle.
    *
    * The \a p.x and \a p.y coordinates are assigned, respectively, to the
    * horizontal and vertical coordinates of this rectangle.
    */
   template <typename T1>
   GenericRectangle& operator =( const pcl::GenericPoint<T1>& p ) noexcept
   {
      x0 = x1 = component( p.x );
      y0 = y1 = component( p.y );
      return *this;
   }

   /*!
    * Assigns a scalar \a d to this rectangle. Returns a reference to this
    * rectangle.
    *
    * The scalar \a d is assigned to the four coordinates of this rectangle.
    */
   GenericRectangle& operator =( component d ) noexcept
   {
      x0 = y0 = x1 = y1 = d;
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericRectangle& operator =( const QRect& r ) noexcept
   {
      x0 = component( r.left() );
      y0 = component( r.top() );
      x1 = component( r.right()+1 );
      y1 = component( r.bottom()+1 );
      return *this;
   }
#endif

   /*!
    * Adds a rectangle \a r to this rectangle. Returns a reference to this
    * rectangle.
    *
    * This function adds homonym coordinates. Given two rectangles \a r1 and
    * \a r2, the sum rectangle \a s = \a r1 + \a r2 is given by:
    *
    * \a s.x0 = \a r1.x0 + \a r2.x0 \n
    * \a s.y0 = \a r1.y0 + \a r2.y0 \n
    * \a s.x1 = \a r1.x1 + \a r2.x1 \n
    * \a s.y1 = \a r1.y1 + \a r2.y1
    */
   template <typename T1>
   GenericRectangle& operator +=( const GenericRectangle<T1>& r ) noexcept
   {
      x0 += component( r.x0 );
      y0 += component( r.y0 );
      x1 += component( r.x1 );
      y1 += component( r.y1 );
      return *this;
   }

   /*!
    * Adds a point \a p to this rectangle. Returns a reference to this
    * rectangle.
    *
    * This function is equivalent to a translation relative to the current
    * position. Given a rectangle \a r and a point \a p, the sum rectangle
    * \a s = \a r + \a p is given by:
    *
    * \a s.x0 = \a r.x0 + \a p.x \n
    * \a s.y0 = \a r.y0 + \a p.y \n
    * \a s.x1 = \a r.x1 + \a p.x \n
    * \a s.y1 = \a r.y1 + \a p.y
    */
   template <typename T1>
   GenericRectangle& operator +=( const pcl::GenericPoint<T1>& p ) noexcept
   {
      x0 += component( p.x );
      y0 += component( p.y );
      x1 += component( p.x );
      y1 += component( p.y );
      return *this;
   }

   /*!
    * Adds a scalar \a d to this rectangle. Returns a reference to this
    * rectangle.
    *
    * This function adds the specified scalar to the four rectangle
    * coordinates. This is equivalent to a translation relative to the current
    * position. Given a rectangle \a r and a scalar \a d, the sum rectangle
    * \a s = \a r + \a d is given by:
    *
    * \a s.x0 = \a r.x0 + \a d \n
    * \a s.y0 = \a r.y0 + \a d \n
    * \a s.x1 = \a r.x1 + \a d \n
    * \a s.y1 = \a r.y1 + \a d
    */
   GenericRectangle& operator +=( component d ) noexcept
   {
      x0 += d;
      y0 += d;
      x1 += d;
      y1 += d;
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericRectangle& operator +=( const QPoint& p ) noexcept
   {
      component dx = component( p.x() ), dy = component( p.y() );
      x0 += dx;
      y0 += dy;
      x1 += dx;
      y1 += dy;
      return *this;
   }
#endif

   /*!
    * Subtracts a rectangle \a r from this rectangle. Returns a reference to
    * this rectangle.
    *
    * This function subtracts homonym coordinates. Given two rectangles \a r1
    * and \a r2, the difference rectangle \a s = \a r1 - \a r2 is given by:
    *
    * \a s.x0 = \a r1.x0 - \a r2.x0 \n
    * \a s.y0 = \a r1.y0 - \a r2.y0 \n
    * \a s.x1 = \a r1.x1 - \a r2.x1 \n
    * \a s.y1 = \a r1.y1 - \a r2.y1
    */
   template <typename T1>
   GenericRectangle& operator -=( const GenericRectangle<T1>& r ) noexcept
   {
      x0 -= component( r.x0 );
      y0 -= component( r.y0 );
      x1 -= component( r.x1 );
      y1 -= component( r.y1 );
      return *this;
   }

   /*!
    * Subtracts a point \a p from this rectangle. Returns a reference to this
    * rectangle.
    *
    * This function is equivalent to a translation relative to the current
    * position. Given a rectangle \a r and a point \a p, the difference
    * rectangle \a s = \a r - \a p is given by:
    *
    * \a s.x0 = \a r.x0 - \a p.x \n
    * \a s.y0 = \a r.y0 - \a p.y \n
    * \a s.x1 = \a r.x1 - \a p.x \n
    * \a s.y1 = \a r.y1 - \a p.y
    */
   template <typename T1>
   GenericRectangle& operator -=( const pcl::GenericPoint<T1>& p ) noexcept
   {
      x0 -= component( p.x );
      y0 -= component( p.y );
      x1 -= component( p.x );
      y1 -= component( p.y );
      return *this;
   }

   /*!
    * Subtracts a scalar \a d from this rectangle. Returns a reference to this
    * rectangle.
    *
    * This function subtracts the specified scalar from the four rectangle
    * coordinates. This is equivalent to a translation relative to the current
    * position. Given a rectangle \a r and a scalar \a d, the difference
    * rectangle \a s = \a r - \a d is given by:
    *
    * \a s.x0 = \a r.x0 - \a d \n
    * \a s.y0 = \a r.y0 - \a d \n
    * \a s.x1 = \a r.x1 - \a d \n
    * \a s.y1 = \a r.y1 - \a d
    */
   GenericRectangle& operator -=( component d ) noexcept
   {
      x0 -= d;
      y0 -= d;
      x1 -= d;
      y1 -= d;
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericRectangle& operator -=( const QPoint& p ) noexcept
   {
      component dx = component( p.x() ), dy = component( p.y() );
      x0 -= dx;
      y0 -= dy;
      x1 -= dx;
      y1 -= dy;
      return *this;
   }
#endif

   /*!
    * Multiplies this rectangle by a rectangle \a r. Returns a reference to
    * this rectangle.
    *
    * This function multiplies homonym coordinates. Given two rectangles \a r1
    * and \a r2, the product rectangle \a P = \a r1 * \a r2 is given by:
    *
    * \a P.x0 = \a r1.x0 * \a r2.x0 \n
    * \a P.y0 = \a r1.y0 * \a r2.y0 \n
    * \a P.x1 = \a r1.x1 * \a r2.x1 \n
    * \a P.y1 = \a r1.y1 * \a r2.y1
    */
   template <typename T1>
   GenericRectangle& operator *=( const GenericRectangle<T1>& r ) noexcept
   {
      x0 *= component( r.x0 );
      y0 *= component( r.y0 );
      x1 *= component( r.x1 );
      y1 *= component( r.y1 );
      return *this;
   }

   /*!
    * Multiplies this rectangle by a point \a p. Returns a reference to this
    * rectangle.
    *
    * This function is equivalent to a translation relative to the current
    * position, plus a scaling factor applied to both sides of the rectangle.
    * Given a rectangle \a r and a point \a p, the product rectangle
    * \a P = \a r * \a p is given by:
    *
    * \a P.x0 = \a r.x0 * \a p.x \n
    * \a P.y0 = \a r.y0 * \a p.y \n
    * \a P.x1 = \a r.x1 * \a p.x \n
    * \a P.y1 = \a r.y1 * \a p.y
    */
   template <typename T1>
   GenericRectangle& operator *=( const pcl::GenericPoint<T1>& p ) noexcept
   {
      x0 *= component( p.x );
      y0 *= component( p.y );
      x1 *= component( p.x );
      y1 *= component( p.y );
      return *this;
   }

   /*!
    * Multiplies this rectangle by a scalar \a d. Returns a reference to this
    * rectangle.
    *
    * This function is equivalent to a translation relative to the current
    * position, plus a scaling factor applied to both sides of the rectangle.
    * Given a rectangle \a r and a scalar \a d, the product rectangle
    * \a P = \a r * \a d is given by:
    *
    * \a P.x0 = \a r.x0 * \a d \n
    * \a P.y0 = \a r.y0 * \a d \n
    * \a P.x1 = \a r.x1 * \a d \n
    * \a P.y1 = \a r.y1 * \a d
    */
   GenericRectangle& operator *=( component d ) noexcept
   {
      x0 *= d;
      y0 *= d;
      x1 *= d;
      y1 *= d;
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericRectangle& operator *=( const QPoint& p ) noexcept
   {
      component dx = component( p.x() ), dy = component( p.y() );
      x0 *= dx; y0 *= dy; x1 *= dx; y1 *= dy;
      return *this;
   }
#endif

   /*!
    * Divides this rectangle by a rectangle \a r. Returns a reference to this
    * rectangle.
    *
    * This function divides homonym coordinates. Given two rectangles \a r1
    * and \a r2, the quotient rectangle \a Q = \a r1 / \a r2 is given by:
    *
    * \a Q.x0 = \a r1.x0 / \a r2.x0 \n
    * \a Q.y0 = \a r1.y0 / \a r2.y0 \n
    * \a Q.x1 = \a r1.x1 / \a r2.x1 \n
    * \a Q.y1 = \a r1.y1 / \a r2.y1
    */
   template <typename T1>
   GenericRectangle& operator /=( const GenericRectangle<T1>& r ) noexcept
   {
      PCL_PRECONDITION( component( r.x0 ) != component( 0 ) && component( r.y0 ) != component( 0 ) &&
                        component( r.x1 ) != component( 0 ) && component( r.y1 ) != component( 0 ) )
      x0 /= component( r.x0 );
      y0 /= component( r.y0 );
      x1 /= component( r.x1 );
      y1 /= component( r.y1 );
      return *this;
   }

   /*!
    * Divides this rectangle by a point \a p. Returns a reference to this
    * rectangle.
    *
    * This function is equivalent to a translation relative to the current
    * position, plus a scaling factor applied to both sides of the rectangle.
    * Given a rectangle \a r and a point \a p, the quotient rectangle
    * \a Q = \a r / \a p is given by:
    *
    * \a Q.x0 = \a r.x0 / \a p.x \n
    * \a Q.y0 = \a r.y0 / \a p.y \n
    * \a Q.x1 = \a r.x1 / \a p.x \n
    * \a Q.y1 = \a r.y1 / \a p.y
    */
   template <typename T1>
   GenericRectangle& operator /=( const pcl::GenericPoint<T1>& p ) noexcept
   {
      PCL_PRECONDITION( component( p.x ) != component( 0 ) && component( p.y ) != component( 0 ) )
      x0 /= component( p.x );
      y0 /= component( p.y );
      x1 /= component( p.x );
      y1 /= component( p.y );
      return *this;
   }

   /*!
    * Divides this rectangle by a scalar \a d. Returns a reference to this
    * rectangle.
    *
    * This function is equivalent to a translation relative to the current
    * position, plus a scaling factor applied to both sides of the rectangle.
    * Given a rectangle \a r and a scalar \a d, the quotient rectangle
    * \a Q = \a r / \a d is given by:
    *
    * \a Q.x0 = \a r.x0 / \a d \n
    * \a Q.y0 = \a r.y0 / \a d \n
    * \a Q.x1 = \a r.x1 / \a d \n
    * \a Q.y1 = \a r.y1 / \a d
    */
   GenericRectangle& operator /=( component d ) noexcept
   {
      PCL_PRECONDITION( d != component( 0 ) )
      x0 /= d; y0 /= d; x1 /= d; y1 /= d;
      return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericRectangle& operator /=( const QPoint& p ) noexcept
   {
      PCL_PRECONDITION( component( p.x() ) != component( 0 ) && component( p.y() ) != component( 0 ) )
      component dx = component( p.x() ), dy = component( p.y() );
      x0 /= dx;
      y0 /= dy;
      x1 /= dx;
      y1 /= dy;
      return *this;
   }
#endif

   /*!
    * Returns a copy of this rectangle.
    */
   GenericRectangle operator +() const noexcept
   {
      return *this;
   }

   /*!
    * Returns a rectangle whose coordinates have the same magnitudes as the
    * coordinates of this rectangle, but opposite signs. The returned rectangle
    * so defined is symmetric to this rectangle with respect to the origin of
    * coordinates.
    */
   GenericRectangle operator -() const noexcept
   {
      return GenericRectangle( -x0, -y0, -x1, -y1 );
   }

#ifdef __PCL_QT_INTERFACE
   operator QRect() const noexcept
   {
      return QRect( int( x0 ), int( y0 ), int( x1-x0 ), int( y1-y0 ) );
   }
#endif

#ifdef __PCL_QT_INTERFACE
#  ifndef __PCL_QT_NO_RECT_DRAWING_HELPERS

   void Draw( QPainter& p, const QBrush* b ) const
   {
      int rx0, ry0, rx1, ry1;

      if ( x0 <= x1 )
         rx0 = x0, rx1 = x1;
      else
         rx0 = x1, rx1 = x0;

      if ( y0 <= y1 )
         ry0 = y0, ry1 = y1;
      else
         ry0 = y1, ry1 = y0;

      if ( rx1 - rx0 <= 1 )
      {
         if ( ry1 - ry0 <= 1 )
            p.drawPoint( rx0, ry0 );
         else
            p.drawLine( rx0, ry0, rx0, ry1-1 );
      }
      else if ( ry1 - ry0 <= 1 )
      {
         p.drawLine( rx0, ry0, rx1-1, ry0 );
      }
      else
      {
#    if ( QT_VERSION >= 0x040000 )
         int w = rx1-rx0-1, h = ry1-ry0-1;
#    else
         int w = rx1-rx0, h = ry1-ry0;
#    endif
         if ( b != 0 )
            p.fillRect( rx0, ry0, w, h, *b );
         p.drawRect( rx0, ry0, w, h );
      }
   }

   void Draw( QPainter& p ) const
   {
      Draw( p, 0 );
   }

   void Draw( QPainter& p, const QColor& c ) const
   {
      QBrush b( c );
      Draw( p, &b );
   }

#  endif // !__PCL_QT_NO_RECT_DRAWING_HELPERS
#endif // __PCL_QT_INTERFACE

}; // GenericRectangle<T>

#undef PCL_ASSERT_RECT_SIZE

// ----------------------------------------------------------------------------

/*!
 * \defgroup rect_functions_2d Rectangle Operators and Functions
 */

/*!
 * Returns true iff two rectangles \a r1 and \a r2 are equal. Two rectangles
 * are equal if their homonym coordinates are equal.
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
bool operator ==( const GenericRectangle<T1>& r1, const GenericRectangle<T2>& r2 ) noexcept
{
   return r1.x0 == r2.x0 && r1.y0 == r2.y0 && r1.x1 == r2.x1 && r1.y1 == r2.y1;
}

/*!
 * Returns true iff a rectangle \a r1 is equal to a scalar \a d2. A rectangle
 * is equal to a scalar \a d if the four coordinates of \a r are equal to \a d.
 * \ingroup rect_functions_2d
 */
template <typename T> inline
bool operator ==( const GenericRectangle<T>& r1, T d2 ) noexcept
{
   return r1.x0 == d2 && r1.y0 == d2 && r1.x1 == d2 && r1.y1 == d2;
}

/*!
 * Returns true iff a scalar \a d1 is equal to a rectangle \a r2. A scalar \a d
 * is equal to a rectangle \a r if the four coordinates of \a r are equal to
 * \a d.
 * \ingroup rect_functions_2d
 */
template <typename T> inline
bool operator ==( T d1, const GenericRectangle<T>& r2 ) noexcept
{
   return d1 == r2.x0 && d1 == r2.y0 && d1 == r2.x1 && d1 == r2.y1;
}

/*!
 * Returns true iff a rectangle \a r1 is less than another rectangle \a r2.
 * For rectangle comparisons, vertical coordinates have precedence over
 * horizontal coordinates.
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
bool operator <( const GenericRectangle<T1>& r1, const GenericRectangle<T2>& r2 ) noexcept
{
   T1 x01 = Min( r1.x0, r1.x1 ); T1 y01 = Min( r1.y0, r1.y1 );
   T1 x11 = Max( r1.x0, r1.x1 ); T1 y11 = Max( r1.y0, r1.y1 );
   T2 x02 = Min( r2.x0, r2.x1 ); T2 y02 = Min( r2.y0, r2.y1 );
   T2 x12 = Max( r2.x0, r2.x1 ); T2 y12 = Max( r2.y0, r2.y1 );
   if ( y01 != y02 )
      return y01 < y02;
   if ( x01 != x02 )
      return x01 < x02;
   if ( y11 != y12 )
      return y11 < y12;
   return x11 < x12;
}

/*!
 * Adds two rectangles \a r1 and \a r2, and returns the resulting sum
 * rectangle.
 *
 * Rectangle addition consists in adding homonym coordinates. Given two
 * rectangles \a r1 and \a r2, the sum rectangle \a s = \a r1 + \a r2 is
 * given by:
 *
 * \a s.x0 = \a r1.x0 + \a r2.x0 \n
 * \a s.y0 = \a r1.y0 + \a r2.y0 \n
 * \a s.x1 = \a r1.x1 + \a r2.x1 \n
 * \a s.y1 = \a r1.y1 + \a r2.y1
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T1> operator +( const GenericRectangle<T1>& r1, const GenericRectangle<T2>& r2 ) noexcept
{
   return GenericRectangle<T1>( T1( r1.x0 + r2.x0 ), T1( r1.y0 + r2.y0 ),
                                T1( r1.x1 + r2.x1 ), T1( r1.y1 + r2.y1 ) );
}

/*!
 * Adds a rectangle \a r1 and a point \a p2, and returns the resulting sum
 * rectangle.
 *
 * Given a rectangle \a r and a point \a p, the sum rectangle
 * \a s = \a r + \a p is given by:
 *
 * \a s.x0 = \a r.x0 + \a p.x \n
 * \a s.y0 = \a r.y0 + \a p.y \n
 * \a s.x1 = \a r.x1 + \a p.x \n
 * \a s.y1 = \a r.y1 + \a p.y
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T1> operator +( const GenericRectangle<T1>& r1, const GenericPoint<T2>& p2 ) noexcept
{
   return GenericRectangle<T1>( T1( r1.x0 + p2.x ), T1( r1.y0 + p2.y ),
                                T1( r1.x1 + p2.x ), T1( r1.y1 + p2.y ) );
}

/*!
 * Adds a point \a p1 and a rectangle \a r2, and returns the resulting sum
 * rectangle.
 *
 * Rectangle addition is a commutative operation, thus see
 * pcl::operator +( const GenericRectangle<T1>&, const GenericPoint<T2>& ).
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T2> operator +( const GenericPoint<T1>& p1, const GenericRectangle<T2>& r2 ) noexcept
{
   return GenericRectangle<T2>( T2( p1.x + r2.x0 ), T2( p1.y + r2.y0 ),
                                T2( p1.x + r2.x1 ), T2( p1.y + r2.y1 ) );
}

/*!
 * Adds a rectangle \a r1 and a scalar \a d2, and returns the resulting sum
 * rectangle.
 *
 * Given a rectangle \a r and a scalar \a d, the sum rectangle
 * \a s = \a r + \a d is given by:
 *
 * \a s.x0 = \a r.x0 + \a d \n
 * \a s.y0 = \a r.y0 + \a d \n
 * \a s.x1 = \a r.x1 + \a d \n
 * \a s.y1 = \a r.y1 + \a d
 *
 * \ingroup rect_functions_2d
 */
template <typename T> inline
GenericRectangle<T> operator +( const GenericRectangle<T>& r1, T d2 ) noexcept
{
   return GenericRectangle<T>( r1.x0+d2, r1.y0+d2, r1.x1+d2, r1.y1+d2 );
}

/*!
 * Adds a scalar \a d1 and a rectangle \a r2, and returns the resulting sum
 * rectangle.
 *
 * Rectangle addition is a commutative operation, thus see
 * pcl::operator +( const GenericRectangle<T>&, T ).
 *
 * \ingroup rect_functions_2d
 */
template <typename T> inline
GenericRectangle<T> operator +( T d1, const GenericRectangle<T>& r2 ) noexcept
{
   return GenericRectangle<T>( d1+r2.x0, d1+r2.y0, d1+r2.x1, d1+r2.y1 );
}

/*!
 * Subtracts two rectangles \a r1 and \a r2, and returns the resulting
 * difference rectangle.
 *
 * Rectangle subtraction consists in subtracting homonym coordinates. Given
 * two rectangles \a r1 and \a r2, the difference rectangle
 * \a s = \a r1 - \a r2 is given by:
 *
 * \a s.x0 = \a r1.x0 - \a r2.x0 \n
 * \a s.y0 = \a r1.y0 - \a r2.y0 \n
 * \a s.x1 = \a r1.x1 - \a r2.x1 \n
 * \a s.y1 = \a r1.y1 - \a r2.y1
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T1> operator -( const GenericRectangle<T1>& r1, const GenericRectangle<T2>& r2 ) noexcept
{
   return GenericRectangle<T1>( T1( r1.x0 - r2.x0 ), T1( r1.y0 - r2.y0 ),
                                T1( r1.x1 - r2.x1 ), T1( r1.y1 - r2.y1 ) );
}

/*!
 * Subtracts a point \a p2 from a rectangle \a r1, and returns the resulting
 * difference rectangle.
 *
 * Given a rectangle \a r and a point \a p, the difference rectangle
 * \a s = \a r - \a p is given by:
 *
 * \a s.x0 = \a r.x0 - \a p.x \n
 * \a s.y0 = \a r.y0 - \a p.y \n
 * \a s.x1 = \a r.x1 - \a p.x \n
 * \a s.y1 = \a r.y1 - \a p.y
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T1> operator -( const GenericRectangle<T1>& r1, const GenericPoint<T2>& p2 ) noexcept
{
   return GenericRectangle<T1>( T1( r1.x0 - p2.x ), T1( r1.y0 - p2.y ),
                                T1( r1.x1 - p2.x ), T1( r1.y1 - p2.y ) );
}

/*!
 * Subtracts a rectangle \a r2 from a point \a p1, and returns the resulting
 * difference rectangle.
 *
 * Given a rectangle \a r and a point \a p, the difference rectangle
 * \a s = \a p - \a r is given by:
 *
 * \a s.x0 = \a p.x - \a r.x0 \n
 * \a s.y0 = \a p.y - \a r.y0 \n
 * \a s.x1 = \a p.x - \a r.x1 \n
 * \a s.y1 = \a p.y - \a r.y1
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T2> operator -( const GenericPoint<T1>& p1, const GenericRectangle<T2>& r2 ) noexcept
{
   return GenericRectangle<T2>( T2( p1.x - r2.x0 ), T2( p1.y - r2.y0 ),
                                T2( p1.x - r2.x1 ), T2( p1.y - r2.y1 ) );
}

/*!
 * Subtracts a scalar \a d2 from a rectangle \a r1, and returns the resulting
 * difference rectangle.
 *
 * Given a rectangle \a r and a scalar \a d, the difference rectangle
 * \a s = \a r - \a d is given by:
 *
 * \a s.x0 = \a r.x0 - \a d \n
 * \a s.y0 = \a r.y0 - \a d \n
 * \a s.x1 = \a r.x1 - \a d \n
 * \a s.y1 = \a r.y1 - \a d
 *
 * \ingroup rect_functions_2d
 */
template <typename T> inline
GenericRectangle<T> operator -( const GenericRectangle<T>& r1, T d2 ) noexcept
{
   return GenericRectangle<T>( r1.x0-d2, r1.y0-d2, r1.x1-d2, r1.y1-d2 );
}

/*!
 * Subtracts a rectangle \a r2 from a scalar \a d1, and returns the resulting
 * difference rectangle.
 *
 * Given a rectangle \a r and a scalar \a d, the difference rectangle
 * \a s = \a d - \a r is given by:
 *
 * \a s.x0 = \a d - \a r.x0 \n
 * \a s.y0 = \a d - \a r.y0 \n
 * \a s.x1 = \a d - \a r.x1 \n
 * \a s.y1 = \a d - \a r.y1
 *
 * \ingroup rect_functions_2d
 */
template <typename T> inline
GenericRectangle<T> operator -( T d1, const GenericRectangle<T>& r2 ) noexcept
{
   return GenericRectangle<T>( d1-r2.x0, d1-r2.y0, d1-r2.x1, d1-r2.y1 );
}

/*!
 * Multiplies two rectangles \a r1 and \a r2, and returns the resulting
 * product rectangle.
 *
 * Rectangle multiplication consists in multiplying homonym coordinates. Given
 * two rectangles \a r1 and \a r2, the product rectangle \a P = \a r1 * \a r2
 * is given by:
 *
 * \a P.x0 = \a r1.x0 * \a r2.x0 \n
 * \a P.y0 = \a r1.y0 * \a r2.y0 \n
 * \a P.x1 = \a r1.x1 * \a r2.x1 \n
 * \a P.y1 = \a r1.y1 * \a r2.y1
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T1> operator *( const GenericRectangle<T1>& r1, const GenericRectangle<T2>& r2 ) noexcept
{
   return GenericRectangle<T1>( T1( r1.x0 * r2.x0 ), T1( r1.y0 * r2.y0 ),
                                T1( r1.x1 * r2.x1 ), T1( r1.y1 * r2.y1 ) );
}

/*!
 * Multiplies a rectangle \a r1 by a point \a p2, and returns the resulting
 * product rectangle.
 *
 * Given a rectangle \a r and a point \a p, the product rectangle
 * \a P = \a r * \a p is given by:
 *
 * \a P.x0 = \a r1.x0 * \a p.x \n
 * \a P.y0 = \a r1.y0 * \a p.y \n
 * \a P.x1 = \a r1.x1 * \a p.x \n
 * \a P.y1 = \a r1.y1 * \a p.y
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T1> operator *( const GenericRectangle<T1>& r1, const GenericPoint<T2>& p2 ) noexcept
{
   return GenericRectangle<T1>( T1( r1.x0 * p2.x ), T1( r1.y0 * p2.y ),
                                T1( r1.x1 * p2.x ), T1( r1.y1 * p2.y ) );
}

/*!
 * Multiplies a point \a p1 by a rectangle \a r2, and returns the resulting
 * product rectangle.
 *
 * Rectangle multiplication is a commutative operation, thus see
 * pcl::operator *( const GenericRectangle<T1>&, const GenericPoint<T2>& ).
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T2> operator *( const GenericPoint<T1>& p1, const GenericRectangle<T2>& r2 ) noexcept
{
   return GenericRectangle<T2>( T2( p1.x * r2.x0 ), T2( p1.y * r2.y0 ),
                                T2( p1.x * r2.x1 ), T2( p1.y * r2.y1 ) );
}

/*!
 * Multiplies a rectangle \a r1 by a scalar \a d2, and returns the resulting
 * product rectangle.
 *
 * Given a rectangle \a r and a scalar \a d, the product rectangle
 * \a P = \a r * \a d is given by:
 *
 * \a P.x0 = \a r1.x0 * \a d \n
 * \a P.y0 = \a r1.y0 * \a d \n
 * \a P.x1 = \a r1.x1 * \a d \n
 * \a P.y1 = \a r1.y1 * \a d
 *
 * \ingroup rect_functions_2d
 */
template <typename T> inline
GenericRectangle<T> operator *( const GenericRectangle<T>& r1, T d2 ) noexcept
{
   return GenericRectangle<T>( r1.x0*d2, r1.y0*d2, r1.x1*d2, r1.y1*d2 );
}

/*!
 * Multiplies a scalar \a d1 by a rectangle \a r2, and returns the resulting
 * product rectangle.
 *
 * Rectangle multiplication is a commutative operation, thus see
 * pcl::operator *( const GenericRectangle<T>&, T ).
 *
 * \ingroup rect_functions_2d
 */
template <typename T> inline
GenericRectangle<T> operator *( T d1, const GenericRectangle<T>& r2 ) noexcept
{
   return GenericRectangle<T>( d1*r2.x0, d1*r2.y0, d1*r2.x1, d1*r2.y1 );
}

/*!
 * Divides two rectangles \a r1 and \a r2, and returns the resulting quotient
 * rectangle.
 *
 * Rectangle division consists in dividing homonym coordinates. Given two
 * rectangles \a r1 and \a r2, the quotient rectangle
 * \a Q = \a r1 / \a r2 is given by:
 *
 * \a Q.x0 = \a r1.x0 / \a r2.x0 \n
 * \a Q.y0 = \a r1.y0 / \a r2.y0 \n
 * \a Q.x1 = \a r1.x1 / \a r2.x1 \n
 * \a Q.y1 = \a r1.y1 / \a r2.y1
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T1> operator /( const GenericRectangle<T1>& r1, const GenericRectangle<T2>& r2 ) noexcept
{
   PCL_PRECONDITION( r2.x0 != T2( 0 ) && r2.y0 != T2( 0 ) &&
                     r2.x1 != T2( 0 ) && r2.y1 != T2( 0 ) )
   return GenericRectangle<T1>( T1( r1.x0 / r2.x0 ), T1( r1.y0 / r2.y0 ),
                                T1( r1.x1 / r2.x1 ), T1( r1.y1 / r2.y1 ) );
}

/*!
 * Divides a rectangle \a r1 by a point \a p2, and returns the resulting
 * quotient rectangle.
 *
 * Given a rectangle \a r and a point \a p, the quotient rectangle
 * \a Q = \a r / \a p is given by:
 *
 * \a Q.x0 = \a r.x0 / \a p.x \n
 * \a Q.y0 = \a r.y0 / \a p.y \n
 * \a Q.x1 = \a r.x1 / \a p.x \n
 * \a Q.y1 = \a r.y1 / \a p.y
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T1> operator /( const GenericRectangle<T1>& r1, const GenericPoint<T2>& p2 ) noexcept
{
   PCL_PRECONDITION( p2.x != T2( 0 ) && p2.y != T2( 0 ) )
   return GenericRectangle<T1>( T1( r1.x0 / p2.x ), T1( r1.y0 / p2.y ),
                                T1( r1.x1 / p2.x ), T1( r1.y1 / p2.y ) );
}

/*!
 * Divides a point \a p1 by a rectangle \a r2, and returns the resulting
 * quotient rectangle.
 *
 * Given a rectangle \a r and a point \a p, the quotient rectangle
 * \a Q = \a p / \a r is given by:
 *
 * \a Q.x0 = \a p.x / \a r.x0 \n
 * \a Q.y0 = \a p.y / \a r.y0 \n
 * \a Q.x1 = \a p.x / \a r.x1 \n
 * \a Q.y1 = \a p.y / \a r.y1
 *
 * \ingroup rect_functions_2d
 */
template <typename T1, typename T2> inline
GenericRectangle<T2> operator /( const GenericPoint<T1>& p1, const GenericRectangle<T2>& r2 ) noexcept
{
   PCL_PRECONDITION( r2.x0 != T2( 0 ) && r2.y0 != T2( 0 ) &&
                     r2.x1 != T2( 0 ) && r2.y1 != T2( 0 ) )
   return GenericRectangle<T2>( T2( p1.x / r2.x0 ), T2( p1.y / r2.y0 ),
                                T2( p1.x / r2.x1 ), T2( p1.y / r2.y1 ) );
}

/*!
 * Divides a rectangle \a r1 by a scalar \a d2, and returns the resulting
 * quotient rectangle.
 *
 * Given a rectangle \a r and a scalar \a d, the quotient rectangle
 * \a Q = \a r / \a d is given by:
 *
 * \a Q.x0 = \a r.x0 / \a d \n
 * \a Q.y0 = \a r.y0 / \a d \n
 * \a Q.x1 = \a r.x1 / \a d \n
 * \a Q.y1 = \a r.y1 / \a d
 *
 * \ingroup rect_functions_2d
 */
template <typename T> inline
GenericRectangle<T> operator /( const GenericRectangle<T>& r1, T d2 ) noexcept
{
   PCL_PRECONDITION( d2 != T( 0 ) )
   return GenericRectangle<T>( r1.x0/d2, r1.y0/d2, r1.x1/d2, r1.y1/d2 );
}

/*!
 * Divides a scalar \a d1 by a rectangle \a r2, and returns the resulting
 * quotient rectangle.
 *
 * Given a rectangle \a r and a scalar \a d, the quotient rectangle
 * \a Q = \a d / \a r is given by:
 *
 * \a Q.x0 = \a d / \a r.x0 \n
 * \a Q.y0 = \a d / \a r.y0 \n
 * \a Q.x1 = \a d / \a r.x1 \n
 * \a Q.y1 = \a d / \a r.y1
 *
 * \ingroup rect_functions_2d
 */
template <typename T> inline
GenericRectangle<T> operator /( T d1, const GenericRectangle<T>& r2 ) noexcept
{
   PCL_PRECONDITION( r2.x0 != T( 0 ) && r2.y0 != T( 0 ) &&
                     r2.x1 != T( 0 ) && r2.y1 != T( 0 ) )
   return GenericRectangle<T>( d1/r2.x0, d1/r2.y0, d1/r2.x1, d1/r2.y1 );
}

/*!
 * Rotates a rectangle in the plane, given a rotation angle and the
 * coordinates of a center of rotation.
 *
 * \param r    Reference to a rectangle that will be rotated.
 * \param a    Rotation angle in radians. Positive angles are measured
 *             counter-clockwise.
 * \param xc   Abscissa (x coordinate) of the center of rotation.
 * \param yc   Ordinate (y coordinate) of the center of rotation.
 *
 * The coordinates \a r.x0, \a r.y0, \a r.x1 and \a r.y1 are replaced by their
 * corresponding rotated values.
 *
 * \ingroup rect_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericRectangle<T>& r, T1 a, T2 xc, T2 yc ) noexcept
{
   T1 sa, ca; pcl::SinCos( a, sa, ca );
   pcl::Rotate( r.x0, r.y0, sa, ca, xc, yc );
   pcl::Rotate( r.x1, r.y1, sa, ca, xc, yc );
}

/*!
 * Rotates a rectangle in the plane, given a rotation angle and a rotation
 * center point.
 *
 * \param r    Reference to a rectangle that will be rotated.
 * \param a    Rotation angle in radians. Positive angles are measured
 *             counter-clockwise.
 * \param c    Reference to a point that will be taken as the center of
 *             rotation.
 *
 * The coordinates \a r.x0, \a r.y0, \a r.x1 and \a r.y1 are replaced by their
 * corresponding rotated values.
 *
 * \ingroup rect_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericRectangle<T>& r, T1 a, const GenericPoint<T2>& c ) noexcept
{
   pcl::Rotate( r, a, c.x, c.y );
}

/*!
 * Rotates a rectangle in the plane, given a rotation angle by its sine and
 * cosine, and the coordinates of a center of rotation.
 *
 * \param r    Reference to a rectangle that will be rotated.
 * \param sa   Sine of the rotation angle.
 * \param ca   Cosine of the rotation angle.
 * \param xc   Abscissa (x coordinate) of the center of rotation.
 * \param yc   Ordinate (y coordinate) of the center of rotation.
 *
 * The coordinates \a r.x0, \a r.y0, \a r.x1 and \a r.y1 are replaced by their
 * corresponding rotated values.
 *
 * \ingroup rect_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericRectangle<T>& r, T1 sa, T1 ca, T2 xc, T2 yc ) noexcept
{
   pcl::Rotate( r.x0, r.y0, sa, ca, xc, yc );
   pcl::Rotate( r.x1, r.y1, sa, ca, xc, yc );
}

/*!
 * Rotates a rectangle in the plane, given a rotation angle by its sine and
 * cosine, and a rotation center point.
 *
 * \param r    Reference to a rectangle that will be rotated.
 * \param sa   Sine of the rotation angle.
 * \param ca   Cosine of the rotation angle.
 * \param c    Reference to a point that will be taken as the center of
 *             rotation.
 *
 * The coordinates \a r.x0, \a r.y0, \a r.x1 and \a r.y1 are replaced by their
 * corresponding rotated values.
 *
 * \ingroup rect_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericRectangle<T>& r, T1 sa, T1 ca, const GenericPoint<T2>& c ) noexcept
{
   pcl::Rotate( r, sa, ca, c.x, c.y );
}

/*!
 * Exchanges two rectangles \a r1 and \a r2. Calling this function is
 * equivalent to:
 *
 * \code
 * pcl::Swap( r1.x0, r2.x0 ); pcl::Swap( r1.y0, r2.y0 );
 * pcl::Swap( r1.x1, r2.x1 ); pcl::Swap( r1.y1, r2.y1 );
 * \endcode
 *
 * \ingroup rect_functions_2d
 */
template <typename T> inline
void Swap( GenericRectangle<T>& r1, GenericRectangle<T>& r2 ) noexcept
{
   pcl::Swap( r1.x0, r2.x0 ); pcl::Swap( r1.y0, r2.y0 );
   pcl::Swap( r1.x1, r2.x1 ); pcl::Swap( r1.y1, r2.y1 );
}

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_RECT_INSTANTIATE

/*!
 * \defgroup rect_types_2d Rectangle Types
 */

/*!
 * \class pcl::I32Rect
 * \ingroup rect_types_2d
 * \brief 32-bit integer rectangle on the plane.
 *
 * %I32Rect is a template instantiation of GenericRectangle for \c int32.
 */
using I32Rect = GenericRectangle<int32>;

/*!
 * \class pcl::Rect
 * \ingroup rect_types_2d
 * \brief 32-bit integer rectangle on the plane.
 *
 * %Rect is an alias for I32Rect. It is a template instantiation of
 * GenericRectangle for \c int32.
 */
using Rect = I32Rect;

/*!
 * \class pcl::F32Rect
 * \ingroup rect_types_2d
 * \brief 32-bit floating-point rectangle in the R^2 space.
 *
 * %F32Rect is a template instantiation of GenericRectangle for \c float.
 */
using F32Rect = GenericRectangle<float>;

/*!
 * \class pcl::FRect
 * \ingroup rect_types_2d
 * \brief 32-bit floating-point rectangle in the R^2 space.
 *
 * %FRect is an alias for F32Rect. It is a template instantiation of
 * GenericRectangle for \c float.
 */
using FRect = F32Rect;

/*!
 * \class pcl::F64Rect
 * \ingroup rect_types_2d
 * \brief 64-bit floating-point rectangle in the R^2 space.
 *
 * %F64Rect is a template instantiation of GenericRectangle for \c double.
 */
using F64Rect = GenericRectangle<double>;

/*!
 * \class pcl::DRect
 * \ingroup rect_types_2d
 * \brief 64-bit floating-point rectangle in the R^2 space.
 *
 * %DRect is an alias for F64Rect. It is a template instantiation of
 * GenericRectangle for \c double.
 */
using DRect = F64Rect;

#endif

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Rectangle_h

// ----------------------------------------------------------------------------
// EOF pcl/Rectangle.h - Released 2024-12-28T16:53:48Z
