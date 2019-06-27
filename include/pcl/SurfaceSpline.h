//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.12.0947
// ----------------------------------------------------------------------------
// pcl/SurfaceSpline.h - Released 2019-04-30T16:30:41Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
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

#ifndef __PCL_SurfaceSpline_h
#define __PCL_SurfaceSpline_h

/// \file pcl/SurfaceSpline.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Array.h>
#include <pcl/Point.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SurfaceSplineBase
 * \brief Base class of two-dimensional surface splines.
 */
class PCL_CLASS SurfaceSplineBase
{
protected:

   /*!
    * Default constructor.
    */
   SurfaceSplineBase() = default;

   /*!
    * Copy constructor.
    */
   SurfaceSplineBase( const SurfaceSplineBase& ) = default;

   /*!
    * Move constructor.
    */
   SurfaceSplineBase( SurfaceSplineBase&& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~SurfaceSplineBase()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SurfaceSplineBase& operator =( const SurfaceSplineBase& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   SurfaceSplineBase& operator =( SurfaceSplineBase&& ) = default;

   /*!
    * Surface spline generation, 32-bit floating point data.
    */
   static void Generate( float* c,
                         const float* x, const float* y, const float* z,
                         int n, int m, float r, const float* w );

   /*!
    * Surface spline generation, 64-bit floating point data.
    */
   static void Generate( double* c,
                         const double* x, const double* y, const double* z,
                         int n, int m, float r, const float* w );
};

// ----------------------------------------------------------------------------

/*!
 * \class SurfaceSpline
 * \brief Two-dimensional interpolating/approximating surface spline (thin plate).
 *
 * %SurfaceSpline implements interpolating or smoothing surface splines (also
 * known as <em>thin plates</em>) for arbitrarily distributed input nodes in
 * two dimensions.
 *
 * The most distinctive property of surface splines is their high adaptability
 * to local variations, which makes them ideal to model complex two dimensional
 * functions with high accuracy. An important advantage of our implementation
 * is the possibility to control adaptability with approximating (or smoothing)
 * surface splines, as opposed to interpolating splines, and the possibility to
 * control adaptability both as a global property of the modeling device, or on
 * a point-by-point basis. The main drawback of surface splines is that they
 * are computationally expensive, especially for large data sets. See the
 * GridInterpolation and PointGridInterpolation classes for discretized
 * implementations with much higher efficiency.
 *
 * \sa PointSurfaceSpline, GridInterpolation, PointGridInterpolation,
 * ShepardInterpolation, SurfacePolynomial
 */
template <typename T>
class PCL_CLASS SurfaceSpline : private SurfaceSplineBase
{
public:

   /*!
    * Represents a vector of coordinates, function values or spline
    * coefficients.
    */
   typedef GenericVector<T>               vector_type;

   /*!
    * The numeric type used to represent coordinates, function values and
    * spline coefficients.
    */
   typedef typename vector_type::scalar   scalar;

   /*!
    * Default constructor. Constructs an empty, two-dimensional interpolating
    * surface spline of second order.
    */
   SurfaceSpline() = default;

   /*!
    * Copy constructor.
    */
   SurfaceSpline( const SurfaceSpline& ) = default;

   /*!
    * Move constructor.
    */
   SurfaceSpline( SurfaceSpline&& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~SurfaceSpline()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SurfaceSpline& operator =( const SurfaceSpline& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   SurfaceSpline& operator =( SurfaceSpline&& ) = default;

   /*!
    * Returns true iff this surface spline is valid. A valid surface spline has
    * been initialized with three or more nodes.
    */
   bool IsValid() const
   {
      return m_x.Length() == m_y.Length() && m_x.Length() >= 3;
   }

   /*!
    * Returns the number of nodes used by this surface spline interpolation.
    */
   int Length() const
   {
      return m_x.Length();
   }

   /*!
    * Returns a vector with the X node coordinates used to initialize this
    * surface spline. If this object has not been initialized, this function
    * returns an empty vector.
    */
   vector_type X() const
   {
      vector_type x( m_x.Length() );
      if ( IsValid() )
         for ( int i = 0; i < m_x.Length(); ++i )
            x[i] = m_x0 + m_x[i]/m_r0;
      return x;
   }

   /*!
    * Returns a vector with the Y node coordinates used to initialize this
    * surface spline. If this object has not been initialized, this function
    * returns an empty vector.
    */
   vector_type Y() const
   {
      vector_type y( m_y.Length() );
      if ( IsValid() )
         for ( int i = 0; i < m_y.Length(); ++i )
            y[i] = m_y0 + m_y[i]/m_r0;
      return y;
   }

   /*!
    * Returns the derivative order of this surface spline.
    */
   int Order() const
   {
      return m_order;
   }

   /*!
    * Sets the derivative order of this surface spline.
    *
    * \param order   Derivative order. Must be >= 2.
    *
    * Calling this member function implicitly resets this %SurfaceSpline
    * object and destroys all internal working structures.
    *
    * The surface spline will be continuously differentiable up to the
    * specified order \a m. If this order is too high, an ill-conditioned
    * linear system may result.
    *
    * The default order is 2. Recommended values are 2 and 3.
    */
   void SetOrder( int order )
   {
      PCL_PRECONDITION( order > 1 )
      Clear();
      m_order = pcl::Max( 2, order );
   }

   /*!
    * Returns the <em>smoothing factor</em> of this surface spline.
    */
   float Smoothing() const
   {
      return m_smoothing;
   }

   /*!
    * Sets the <em>smoothing factor</em> of this surface spline.
    *
    * \param s    Smoothing factor. Must be >= 0.
    *
    * For \a s = 0, an interpolating spline will be generated: all node values
    * will be reproduced exactly at their respective coordinates.
    *
    * For \a s > 0, a smoothing (or approximating) spline will be generated:
    * increasing \a s values will generate splines closer to the reference
    * plane of the input node set.
    */
   void SetSmoothing( float s )
   {
      PCL_PRECONDITION( s >= 0 )
      Clear();
      m_smoothing = pcl::Max( 0.0F, s );
   }

   /*!
    * Generation of a two-dimensional surface spline (thin plate).
    *
    * \param x       X node coordinates.
    *
    * \param y       Y node coordinates.
    *
    * \param z       Node function values.
    *
    * \param n       Number of nodes. Must be &ge; 3
    *                (3 nodes * 2 coordinates = six degrees of freedom).
    *
    * \param w       When the smoothing factor of this spline is > 0, this is a
    *                vector of positive weights > 0 corresponding to the
    *                specified input nodes. If this parameter is \c nullptr,
    *                unit weights are assumed for all input nodes. When the
    *                smoothing factor is zero (interpolating spline), this
    *                parameter is ignored.
    *
    * The input nodes can be arbitrarily distributed, and they don't need to
    * follow any specific order. However, all nodes should be distinct with
    * respect to the machine epsilon for the floating point type T.
    *
    * This initialization function includes a sanitization routine. If there
    * are duplicate points in the specified set of input nodes, only the first
    * occurrence of each duplicate will be kept to build the surface spline,
    * and the rest of duplicate points will be ignored. Two points are
    * considered equal if their coordinates don't differ more than the machine
    * epsilon for the floating point type T.
    *
    * For an interpolating surface spline (smoothness = 0), all node values
    * will be reproduced exactly at their respective coordinates. In this case
    * the vector \a w of node weights will be ignored.
    *
    * For an approximating surface spline (smoothness > 0), if a vector \a w of
    * node weights is specified, it will be used to assign a different
    * interpolation \e strength to each interpolation node. In this case the
    * vector \a w must have at least \a n values greater than zero. A node
    * weight larger than one will reduce the smoothness of the interpolating
    * surface at the corresponding node coordinates, or in other words, it will
    * give more prominence to the corresponding data point. A node weight of
    * one will apply the current surface smoothness at its node position. A
    * node weight smaller than one will increase the interpolation smoothness.
    */
   void Initialize( const T* x, const T* y, const T* z, int n, const float* w = nullptr )
   {
      PCL_PRECONDITION( x != nullptr && y != nullptr && z != nullptr )
      PCL_PRECONDITION( n > 2 )

      if ( n < 3 )
         throw Error( "SurfaceSpline::Initialize(): At least three input nodes must be specified." );

      Clear();

      try
      {
         if ( m_smoothing <= 0 )
            w = nullptr;

         /*
          * Find mean coordinates.
          */
         m_x0 = m_y0 = 0;
         for ( int i = 0; i < n; ++i )
         {
            m_x0 += x[i];
            m_y0 += y[i];
         }
         m_x0 /= n;
         m_y0 /= n;

         /*
          * Find radius of largest containing circle.
          */
         m_r0 = 0;
         for ( int i = 0; i < n; ++i )
         {
            double dx = x[i] - m_x0;
            double dy = y[i] - m_y0;
            double r = Sqrt( dx*dx + dy*dy );
            if ( r > m_r0 )
               m_r0 = r;
         }
         if ( 1 + m_r0 == 1 )
            throw Error( "SurfaceSpline::Initialize(): Empty or insignificant interpolation space." );
         m_r0 = 1/m_r0;

         /*
          * Build point list with normalized node coordinates.
          */
         Array<Point3D> P;
         for ( int i = 0; i < n; ++i )
            P << Point3D( m_r0*(x[i] - m_x0),
                          m_r0*(y[i] - m_y0),
                          z[i], (w != nullptr && w[i] > 0) ? w[i] : 1.0F );

         /*
          * Find duplicate input nodes. Two nodes are considered equal if their
          * coordinates don't differ more than the machine epsilon for the
          * floating point type T.
          */
         P.Sort();
         Array<int> remove;
         for ( int i = 0, j = 1; j < n; ++i, ++j )
            if ( Abs( P[i].x - P[j].x ) <= std::numeric_limits<T>::epsilon() )
               if ( Abs( P[i].y - P[j].y ) <= std::numeric_limits<T>::epsilon() )
                  remove << i;

         /*
          * Build working vectors, excluding duplicate input nodes.
          */
         int N = n - int( remove.Length() );
         if ( N < 3 )
            throw Error( "SurfaceSpline::Initialize(): Less than three input nodes left after sanitization." );
         m_x = vector_type( N );
         m_y = vector_type( N );
         vector_type fz( N );
         if ( w != nullptr )
            m_weights = FVector( N );
         int i = 0, k = 0;
         for ( int j : remove )
         {
            for ( ; i < j; ++i, ++k )
            {
               m_x[k] = P[i].x;
               m_y[k] = P[i].y;
               fz[k] = P[i].z;
               if ( w != nullptr )
                  m_weights[k] = P[i].w;
            }
            ++i;
         }
         for ( ; i < n; ++i, ++k )
         {
            m_x[k] = P[i].x;
            m_y[k] = P[i].y;
            fz[k] = P[i].z;
            if ( w != nullptr )
               m_weights[k] = P[i].w;
         }

         m_spline = vector_type( T( 0 ), N + ((m_order*(m_order + 1)) >> 1) );

         Generate( m_spline.Begin(), m_x.Begin(), m_y.Begin(), fz.Begin(), N, m_order, m_smoothing, m_weights.Begin() );
      }
      catch ( ... )
      {
         Clear();
         throw;
      }
   }

   /*!
    * Resets this surface spline interpolation, deallocating all internal
    * working structures.
    */
   void Clear()
   {
      m_x.Clear();
      m_y.Clear();
      m_weights.Clear();
      m_spline.Clear();
   }

   /*!
    * Two-dimensional surface spline interpolation/approximation. Returns an
    * approximated or interpolated function value at the specified \a x and
    * \a y coordinates.
    *
    * Before calling this function, a valid surface spline must be generated by
    * calling Initialize(). If called fon an uninitialized object, this member
    * function invokes undefined behavior.
    */
   T operator ()( double x, double y ) const
   {
      PCL_PRECONDITION( !m_x.IsEmpty() && !m_y.IsEmpty() )
      PCL_CHECK( m_order >= 2 )
      PCL_CHECK( !m_spline.IsEmpty() )

      x = m_r0*(x - m_x0);
      y = m_r0*(y - m_y0);

      int n = m_x.Length();
      double z = m_spline[n];
      switch ( m_order )
      {
      case 2:
         z += m_spline[n+1]*x + m_spline[n+2]*y;
         break;
      case 3:
         z += (m_spline[n+1] + m_spline[n+3]*x + m_spline[n+4]*y)*x + (m_spline[n+2] + m_spline[n+5]*y)*y;
         break;
      default:
         for ( int i = 1, j = n+1, i1 = (m_order*(m_order + 1))>>1, ix = 0, iy = 0; i < i1; ++i, ++j )
            if ( ix == 0 )
            {
               ix = iy + 1;
               iy = 0;
               z += m_spline[j] * PowI( x, ix );
            }
            else
            {
               --ix;
               ++iy;
               z += m_spline[j] * PowI( x, ix ) * PowI( y, iy );
            }
         break;
      }

      for ( int i = 0; i < n; ++i )
      {
         double dx = m_x[i] - x;
         double dy = m_y[i] - y;
         double r2 = dx*dx + dy*dy;
         if ( r2 != 0 )
         {
            double r2m1 = r2;
            for ( int j = m_order; --j > 1; )
               r2m1 *= r2;
            z += m_spline[i] * pcl::Ln( r2 ) * r2m1;
         }
      }

      return z;
   }

   /*!
    * Returns an interpolated/approximated function value at the specified
    * \a p.x and \a p.y point coordinates. See operator()( double, double ) for
    * more information.
    */
   template <typename Tp>
   T operator ()( const GenericPoint<Tp>& p ) const
   {
      return operator ()( double( p.x ), double( p.y ) );
   }

protected:

   vector_type m_x;             // vector of normalized X node coordinates
   vector_type m_y;             // vector of normalized Y node coordinates
   double      m_r0 = 1;        // scaling factor for normalization of node coordinates
   double      m_x0 = 0;        // zero offset for normalization of X node coordinates
   double      m_y0 = 0;        // zero offset for normalization of Y node coordinates
   int         m_order = 2;     // derivative order > 1
   float       m_smoothing = 0; // smoothing factor, or interpolating 2-D spline if m_smoothing == 0
   FVector     m_weights;       // optional node weights for approximating spline
   vector_type m_spline;        // coefficients of the 2-D surface spline

   /*!
    * \struct Point3D
    * \brief Auxiliary structure for data sanitization.
    * \internal
    */
   struct Point3D
   {
      T x, y, z;
      float w;

      Point3D( T a_x, T a_y, T a_z, float a_w ) :
         x( a_x ), y( a_y ), z( a_z ), w( a_w )
      {
      }

      bool operator ==( const Point3D& p ) const
      {
         return x == p.x && y == p.y;
      }

      bool operator <( const Point3D& p ) const
      {
         return (x != p.x) ? x < p.x : y < p.y;
      }
   };

   friend class DrizzleData;
   friend class DrizzleDataDecoder;
};

// ----------------------------------------------------------------------------

/*!
 * \class PointSurfaceSpline
 * \brief Vector surface spline interpolation/approximation in two dimensions
 *
 * The template parameter P represents an interpolation point in two
 * dimensions. The type P must implement P::x and P::y data members accessible
 * from the current %PointSurfaceSpline template specialization. These members
 * must provide the values of the horizontal and vertical coordinates,
 * respectively, of an interpolation point. In addition, the scalar types of
 * the P::x and P::y point members must support conversion to double semantics.
 */
template <class P = DPoint>
class PCL_CLASS PointSurfaceSpline
{
public:

   /*!
    * Represents an interpolation point in two dimensions.
    */
   typedef P                     point;

   /*!
    * Represents a sequence of interpolation points.
    */
   typedef Array<point>          point_list;

   /*!
    * Represents a coordinate interpolating/approximating surface spline.
    */
   typedef SurfaceSpline<double> spline;

   /*!
    * Default constructor. Yields an empty instance that cannot be used without
    * initialization.
    */
   PointSurfaceSpline() = default;

   /*!
    * Copy constructor.
    */
   PointSurfaceSpline( const PointSurfaceSpline& ) = default;

   /*!
    * Move constructor.
    */
   PointSurfaceSpline( PointSurfaceSpline&& ) = default;

   /*!
    * Constructs a %PointSurfaceSpline object initialized for the specified
    * input data and interpolation parameters.
    *
    * See the corresponding Initialize() member function for a detailed
    * description of parameters.
    */
   PointSurfaceSpline( const point_list& P1, const point_list& P2,
                       float smoothness = 0, int order = 2, const FVector& W = FVector() )
   {
      Initialize( P1, P2, smoothness, W, order );
   }

   /*!
    * Constructs a %PointSurfaceSpline object initialized with prescribed point
    * surface splines.
    *
    * See the corresponding Initialize() member function for a more detailed
    * description of parameters and their required conditions.
    */
   PointSurfaceSpline( const spline& Sx, const spline& Sy )
   {
      Initialize( Sx, Sy );
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PointSurfaceSpline& operator =( const PointSurfaceSpline& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   PointSurfaceSpline& operator =( PointSurfaceSpline&& ) = default;

   /*!
    * Initializes this %PointSurfaceSpline object for the specified input data
    * and interpolation parameters.
    *
    * \param P1         A sequence of distinct interpolation node points.
    *
    * \param P2         A sequence of interpolation values. For each point in
    *                   \a P1, the coordinates of its counterpart point in
    *                   \a P2 will be used as the interpolation node values in
    *                   the X and Y directions.
    *
    * \param smoothness Smoothing factor. Must be >= 0. The default value is 0.
    *
    * \param W          Reference to a vector of positive node \e weights > 0,
    *                   when the smoothing factor is > 1. For an interpolating
    *                   spline this parameter will be ignored (see below).
    *
    * \param order      Derivative order. Must be >= 2. The default value is 2.
    *
    * For \a smoothness <= 0, an interpolating spline will be generated: all
    * node values will be reproduced exactly at their respective coordinates.
    * In this case the \a W parameter will be ignored.
    *
    * For \a smoothness > 0, a smoothing (or approximating) spline will be
    * generated: increasing \a smoothness values will generate splines closer
    * to the reference plane of the input node set. If nonempty, the specified
    * vector \a W of node weights will be used to assign a different
    * interpolation \e strength to each interpolation node. In this case the
    * vector \a W must have at least the same length as the shortest of the
    * \a P1 and \a P2 vectors, and must contain values greater than zero. A
    * node weight larger than one will reduce the smoothness of the
    * interpolating surface at the corresponding node coordinates, or in other
    * words, it will give more prominence to the corresponding data point. A
    * node weight of one will apply the specified \a smoothness at its node
    * position. Contrarily, a node weight smaller than one will increase the
    * interpolation smoothness locally.
    *
    * The surface spline will be continuously differentiable up to the
    * specified \a order. If this order is too high, an ill-conditioned
    * linear system may result, especially for large data sets. The recommended
    * values are 2, 3, 4 or 5, but order 2 is usually the most useful and
    * safest option for most applications. Derivative orders greater than 3 may
    * lead to numerically unstable interpolation devices, which should always
    * be used with care.
    */
   void Initialize( const point_list& P1, const point_list& P2,
                    float smoothness = 0, const FVector& W = FVector(), int order = 2 )
   {
      PCL_PRECONDITION( P1.Length() >= 3 )
      PCL_PRECONDITION( P1.Length() <= P2.Length() )
      PCL_PRECONDITION( order >= 2 )
      PCL_PRECONDITION( W.IsEmpty() || P1.Length() <= W.Length() )

      m_Sx.Clear();
      m_Sy.Clear();

      m_Sx.SetOrder( order );
      m_Sy.SetOrder( order );

      m_Sx.SetSmoothing( smoothness );
      m_Sy.SetSmoothing( smoothness );

      if ( P1.Length() < 3 || P2.Length() < 3 )
         throw Error( "PointSurfaceSpline::Initialize(): At least three input nodes must be specified." );

      if ( P1.Length() > P2.Length() ||
           !W.IsEmpty() && P1.Length() > size_type( W.Length() ) )
         throw Error( "PointSurfaceSpline::Initialize(): Insufficient data." );

      DVector X( int( P1.Length() ) ),
              Y( int( P1.Length() ) ),
              Zx( int( P1.Length() ) ),
              Zy( int( P1.Length() ) );
      for ( int i = 0; i < X.Length(); ++i )
      {
          X[i] = P1[i].x;
          Y[i] = P1[i].y;
         Zx[i] = P2[i].x;
         Zy[i] = P2[i].y;
      }
      m_Sx.Initialize( X.Begin(), Y.Begin(), Zx.Begin(), X.Length(), W.Begin() );
      m_Sy.Initialize( X.Begin(), Y.Begin(), Zy.Begin(), X.Length(), W.Begin() );
   }

   /*!
    *
    */
   void Initialize( const DVector& X1, const DVector& Y1, const DVector& Z1,
                    const DVector& X2, const DVector& Y2, const DVector& Z2,
                    float smoothness = 0,
                    const FVector& W1 = FVector(), const FVector& W2 = FVector(),
                    int order = 2 )
   {
      PCL_PRECONDITION( X1.Length() >= 3 )
      PCL_PRECONDITION( X2.Length() >= 3 )
      PCL_PRECONDITION( X1.Length() <= Y1.Length() )
      PCL_PRECONDITION( X1.Length() <= Z1.Length() )
      PCL_PRECONDITION( W1.IsEmpty() || X1.Length() <= W1.Length() )
      PCL_PRECONDITION( X2.Length() <= Y2.Length() )
      PCL_PRECONDITION( X2.Length() <= Z2.Length() )
      PCL_PRECONDITION( W2.IsEmpty() || X2.Length() <= W2.Length() )
      PCL_PRECONDITION( order >= 2 )

      m_Sx.Clear();
      m_Sy.Clear();

      m_Sx.SetOrder( order );
      m_Sy.SetOrder( order );

      m_Sx.SetSmoothing( smoothness );
      m_Sy.SetSmoothing( smoothness );

      if ( X1.Length() < 3 || X2.Length() < 3 )
         throw Error( "PointSurfaceSpline::Initialize(): At least three input nodes must be specified." );

      if ( X1.Length() > Y1.Length() ||
           X1.Length() > Z1.Length() ||
           !W1.IsEmpty() && X1.Length() > W1.Length() ||
           X2.Length() > Y2.Length() ||
           X2.Length() > Z2.Length() ||
           !W2.IsEmpty() && X2.Length() > W2.Length() )
         throw Error( "PointSurfaceSpline::Initialize(): Insufficient data." );

      m_Sx.Initialize( X1.Begin(), Y1.Begin(), Z1.Begin(), X1.Length(), W1.Begin() );
      m_Sy.Initialize( X2.Begin(), Y2.Begin(), Z2.Begin(), X2.Length(), W2.Begin() );
   }

   /*!
    * Initializes this %PointSurfaceSpline object with prescribed point surface
    * splines.
    *
    * \param Sx   2-D point surface spline for interpolation of function values
    *             in the X direction.
    *
    * \param Sy   2-D point surface spline for interpolation of function values
    *             in the Y direction.
    *
    * Both surface splines must be valid. If one or both splines are invalid,
    * calling this member function is equivalent to Clear().
    *
    * After calling this member function successfuly, this object will store
    * deep copies of the specified surface spline instances.
    */
   void Initialize( const spline& Sx, const spline& Sy )
   {
      if ( Sx.IsValid() && Sy.IsValid() )
      {
         m_Sx = Sx;
         m_Sy = Sy;
      }
      else
         Clear();
   }

   /*!
    * Returns an approximation to the inverse surface spline of this object.
    *
    * The returned object can be used to perform an inverse interpolation:
    * Given an interpolation point P2, the returned spline will interpolate the
    * corresponding node point P1. See Initialize() for more information on
    * spline initialization.
    *
    * In general, the returned object can only provide an approximation to the
    * inverse of the underlying coordinate transformation. In particular, if
    * this object has been initialized as an approximating surface spline, its
    * inverse spline will compute node point coordinates from approximate
    * (smoothed) interpolated coordinates, instead of the original ones.
    *
    * If two or more interpolation points were identical when this object was
    * initialized, calling this member function may lead to an ill-conditioned
    * linear system. In such case, an Error exception will be thrown.
    *
    * If this object has not been initialized, this function returns an
    * uninitialized %PointSurfaceSpline object.
    */
   PointSurfaceSpline Inverse() const
   {
      PointSurfaceSpline inverse;
      if ( IsValid() )
      {
         DVector X = m_Sx.X(),
                 Y = m_Sx.Y(),
                 Zx( m_Sx.X().Length() ),
                 Zy( m_Sx.Y().Length() );
         for ( int i = 0; i < X.Length(); ++i )
         {
            Zx[i] = m_Sx( X[i], Y[i] );
            Zy[i] = m_Sy( X[i], Y[i] );
         }
         inverse.m_Sx.SetOrder( m_Sx.Order() );
         inverse.m_Sy.SetOrder( m_Sy.Order() );
         inverse.m_Sx.Initialize( Zx.Begin(), Zy.Begin(), X.Begin(), X.Length() );
         inverse.m_Sy.Initialize( Zx.Begin(), Zy.Begin(), Y.Begin(), X.Length() );
      }
      return inverse;
   }

   /*!
    * Deallocates internal structures, yielding an empty spline that cannot be
    * used before a new call to Initialize().
    */
   void Clear()
   {
      m_Sx.Clear();
      m_Sy.Clear();
   }

   /*!
    * Returns true iff this is a valid, initialized object ready for
    * interpolation.
    */
   bool IsValid() const
   {
      return m_Sx.IsValid() && m_Sy.IsValid();
   }

   /*!
    * Returns a reference to the internal surface spline object used for
    * interpolation in the X plane direction.
    */
   const spline& SplineX() const
   {
      return m_Sx;
   }

   /*!
    * Returns a reference to the internal surface spline object used for
    * interpolation in the Y plane direction.
    */
   const spline& SplineY() const
   {
      return m_Sy;
   }

   /*!
    * Returns an interpolated point at the specified coordinates.
    */
   template <typename T>
   DPoint operator ()( T x, T y ) const
   {
      return DPoint( m_Sx( x, y ), m_Sy( x, y ) );
   }

   /*!
    * Returns an interpolated point at the given \a p.x and \a p.y coordinates.
    */
   template <typename T>
   DPoint operator ()( const GenericPoint<T>& p ) const
   {
      return operator ()( p.x, p.y );
   }

private:

   /*
    * The surface splines in the X and Y plane directions.
    */
   spline m_Sx, m_Sy;

   friend class DrizzleData;
   friend class DrizzleDataDecoder;
};

// ----------------------------------------------------------------------------

}  // pcl

#endif   // __PCL_SurfaceSpline_h

// ----------------------------------------------------------------------------
// EOF pcl/SurfaceSpline.h - Released 2019-04-30T16:30:41Z
