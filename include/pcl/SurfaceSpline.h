//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.23
// ----------------------------------------------------------------------------
// pcl/SurfaceSpline.h - Released 2022-03-12T18:59:29Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2022 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_SurfaceSpline_h
#define __PCL_SurfaceSpline_h

/// \file pcl/SurfaceSpline.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AbstractImage.h>
#include <pcl/Array.h>
#include <pcl/AutoPointer.h>
#include <pcl/ParallelProcess.h>
#include <pcl/Point.h>
#include <pcl/QuadTree.h>
#include <pcl/Thread.h>
#include <pcl/Vector.h>

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
#  include <pcl/Console.h>
#  include <pcl/StandardStatus.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Default quadtree bucket capacity for recursive surface subspline generation.
 */
#define __PCL_RSSPLINE_DEFAULT_TREE_BUCKET_CAPACITY   64

/*
 * Default maximum spline length for a non-recursive quadtree node in a
 * recursive surface spline.
 */
#define __PCL_RSSPLINE_DEFAULT_SPLINE_MAX_LENGTH      1600

/*
 * Whether to allow extrapolation outside the interpolation region for
 * recursive surface splines. Extrapolation is disabled by default because
 * recursively defined subsplines are slightly more prone to oscillation than
 * normal surface splines.
 */
#define __PCL_RSSPLINE_DEFAULT_ALLOW_EXTRAPOLATION    false

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::RadialBasisFunction
 * \brief     Implemented radial basis functions (RBFs) for surface spline
 *            interpolation.
 *
 * The symbols used in the following table are:
 *
 *    phi(r)   The radial basis function evaluated at a point (x,y)
 *    r        Distance to center, r^2 = (x-xc)^2 + (y-yc)^2
 *    (xc,yc)  Center point
 *    eps      Shape parameter
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>RadialBasisFunction::Unknown</td>             <td>Unknown or unsupported function.</td></tr>
 * <tr><td>RadialBasisFunction::VariableOrder</td>       <td>phi(r) = (r^2)^(m-1) * Ln( r^2 )</td></tr>
 * <tr><td>RadialBasisFunction::ThinPlateSpline</td>     <td>phi(r) = r^2 * Ln( r )</td></tr>
 * <tr><td>RadialBasisFunction::Gaussian</td>            <td>phi(r) = Exp( -(eps*r)^2 )</td></tr>
 * <tr><td>RadialBasisFunction::Multiquadric</td>        <td>phi(r) = Sqrt( 1 + (eps*r)^2 )</td></tr>
 * <tr><td>RadialBasisFunction::InverseMultiquadric</td> <td>phi(r) = 1/Sqrt( 1 + (eps*r)^2 )</td></tr>
 * <tr><td>RadialBasisFunction::InverseQuadratic</td>    <td>phi(r) = 1/( 1 + (eps*r)^2 )</td></tr>
 * </table>
 */
namespace RadialBasisFunction
{
   enum value_type
   {
      Unknown = -1,
      VariableOrder = 0,   // phi(r) = (r^2)^(m-1) * Ln( r^2 )
      ThinPlateSpline,     // phi(r) = r^2 * Ln( r )
      Gaussian,            // phi(r) = Exp( -(eps*r)^2 )
      Multiquadric,        // phi(r) = Sqrt( 1 + (eps*r)^2 )
      InverseMultiquadric, // phi(r) = 1/Sqrt( 1 + (eps*r)^2 )
      InverseQuadratic,    // phi(r) = 1/( 1 + (eps*r)^2 )
      __number_of_items__,
      Default = ThinPlateSpline
   };

   inline static bool Validate( int rbf )
   {
      return rbf >= 0 && rbf < __number_of_items__;
   }
}

// ----------------------------------------------------------------------------

/*!
 * \class SurfaceSplineBase
 * \brief Base class of two-dimensional surface splines.
 */
class PCL_CLASS SurfaceSplineBase
{
public:

   /*!
    * Represents a radial basis function (RBF) supported by this surface spline
    * implementation.
    */
   typedef RadialBasisFunction::value_type   rbf_type;

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
   static void Generate( float* __restrict__ c,
                         rbf_type, double e2, bool polynomial,
                         const float* __restrict__ x, const float* __restrict__ y, const float* __restrict__ z,
                         int n, int m, float r, const float* __restrict__ w );

   /*!
    * Surface spline generation, 64-bit floating point data.
    */
   static void Generate( double* __restrict__ c,
                         rbf_type, double e2, bool polynomial,
                         const double* __restrict__ x, const double* __restrict__ y, const double* __restrict__ z,
                         int n, int m, float r, const float* __restrict__ w );
};

// ----------------------------------------------------------------------------

/*!
 * \class SurfaceSpline
 * \brief Two-dimensional interpolating/approximating surface spline.
 *
 * %SurfaceSpline implements interpolating or smoothing surface splines for
 * arbitrarily distributed input nodes in two dimensions using radial basis
 * functions (RBFs).
 *
 * The most distinctive property of surface splines is their high adaptability
 * to local variations, which makes them ideal to model complex two dimensional
 * functions with high accuracy. These interpolation properties largely depend
 * on the employed radial basis functions. The well-known <em>thin plate
 * spline</em> function is the default RBF used by this implementation. A thin
 * plate spline describes the minimal-energy bending of a thin sheet of metal
 * passing through a set of interpolation points in three-dimensional space.
 * This physical justification gives thin plate splines accuracy and
 * adaptability properties that we have been applying successfully to many
 * critical data modeling tasks, including very especially image registration
 * and astrometric solutions with arbitrary distortion corrections.
 *
 * Other RBFs supported by this class include a variable order function that
 * allows imposing the derivability order of the interpolation/approximation
 * spline, as well as less globally dependent functions such as Gaussian and
 * multiquadric RBFs. See the RadialBasisFunction namespace for the complete
 * list of supported RBFs.
 *
 * An important advantage of our implementation is the possibility to control
 * adaptability with approximating (or smoothing) surface splines, as opposed
 * to interpolating splines, and the possibility to control adaptability both
 * as a global property of the modeling device, or on a point-by-point basis.
 *
 * The main drawback of surface splines is that they are computationally
 * expensive, especially for large data sets. Generation of surface splines
 * usually requires solving dense linear systems, which involves the use of
 * algorithms with O(N^3) complexity (such as the Bunch-Kaufman diagonal
 * pivoting method). This limits the applicability of this implementation to
 * data sets of no more than about 2000-3000 interpolation nodes using current
 * hardware. See the RecursivePointSurfaceSpline class for a more efficient
 * implementation to generate local subsplines based on quadtrees.
 *
 * For fast evaluation of surface splines in massive problems, see the
 * GridInterpolation and PointGridInterpolation classes for highly efficient
 * discretized implementations.
 *
 * \sa PointSurfaceSpline, RecursivePointSurfaceSpline, GridInterpolation,
 * PointGridInterpolation, ShepardInterpolation, SurfacePolynomial
 */
template <typename T>
class PCL_CLASS SurfaceSpline : private SurfaceSplineBase
{
public:

   /*!
    * The numeric type used to represent coordinates, function values and
    * spline coefficients.
    */
   typedef T                           scalar;

   /*!
    * Represents a vector of coordinates, function values or spline
    * coefficients.
    */
   typedef GenericVector<scalar>       vector;

   /*!
    * The numeric type used to represent the interpolation strength of a
    * surface interpolation node.
    */
   typedef float                       weight;

   /*!
    * Represents a vector of interpolation node weights.
    */
   typedef GenericVector<weight>       weight_vector;

   /*!
    * Represents a radial basis function (RBF) supported by this surface spline
    * implementation.
    */
   typedef SurfaceSplineBase::rbf_type rbf_type;

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
    * been initialized with three or more interpolation nodes.
    */
   bool IsValid() const
   {
      return m_x.Length() >= 3 && m_x.Length() == m_y.Length();
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
   vector X() const
   {
      vector x( m_x.Length() );
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
   vector Y() const
   {
      vector y( m_y.Length() );
      if ( IsValid() )
         for ( int i = 0; i < m_y.Length(); ++i )
            y[i] = m_y0 + m_y[i]/m_r0;
      return y;
   }

   /*!
    * Returns the type of radial basis function (RBF) used by this surface
    * spline. See the RadialBasisFunction namespace for the list of supported
    * RBFs in the current implementation.
    */
   rbf_type RBFType() const
   {
      return m_rbf;
   }

   /*!
    * Sets the radial basis function that will be used by this surface spline.
    *
    * Calling this member function implicitly resets this %SurfaceSpline object
    * and destroys all internal working structures. If the specified RBF is not
    * of a variable order type, the derivability order of this surface spline
    * will be reset to second order.
    */
   void SetRBFType( rbf_type rbf )
   {
      Clear();
      m_rbf = rbf;
   }

   /*!
    * Returns the shape parameter applied to scale the radial basis function
    * used by this surface spline.
    *
    * The shape parameter is only used by decreasing RBFs, i.e., it is not used
    * by the variable order and thin plate spline RBFs, which are
    * parameter-free.
    */
   double ShapeParameter() const
   {
      return Sqrt( m_eps2 );
   }

   /*!
    * Sets the shape parameter applied to scale the radial basis function used
    * by this surface spline.
    *
    * If the specified value \a eps is zero, an optimal shape parameter value
    * will be computed automatically during initialization of spline functions.
    * See the Initialize() member function for more information.
    *
    * Calling this member function implicitly resets this %SurfaceSpline object
    * and destroys all internal working structures.
    *
    * The shape parameter is only used by decreasing RBFs, i.e., it is not used
    * by the variable order and thin plate spline RBFs, which are
    * parameter-free.
    */
   void SetShapeParameter( double eps )
   {
      Clear();
      m_eps = Abs( eps );
      m_eps2 = m_eps*m_eps;
   }

   /*!
    * Returns the derivability order of this surface spline.
    */
   int Order() const
   {
      return m_order;
   }

   /*!
    * Sets the derivability order of this surface spline.
    *
    * \param order   Derivability order. Must be &ge; 2.
    *
    * Calling this member function implicitly resets this %SurfaceSpline object
    * and destroys all internal working structures.
    *
    * Specifying a derivability order &ge; 2 is really useful for variable
    * order radial basis functions (RadialBasisFunction::VariableOrder). Other
    * RBFs are implicitly of second order, so when they are selected the order
    * specified by calling this function will only change the polynomial part
    * of the surface spline.
    *
    * Only when a variable order RBF is selected, the surface spline will be
    * continuously differentiable up to the specified \a order after a
    * subsequent initialization. If the order is too high, an ill-conditioned
    * linear system may result even for a set of valid interpolation nodes.
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
    * Returns true iff this surface spline adds a stabilizing first-degree
    * polynomial to the RBF interpolation/approximation function.
    *
    * A polynomial part is always enabled by default. Disabling it can generate
    * some stability issues, especially for data sets with highly varying node
    * values.
    */
   bool IsPolynomialEnabled() const
   {
      return m_havePolynomial;
   }

   /*!
    * Enables the use of a stabilizing first-degree polynomial added to the RBF
    * interpolation/approximation function.
    *
    * Calling this member function implicitly resets this %SurfaceSpline object
    * and destroys all internal working structures.
    */
   void EnablePolynomial( bool enable = true )
   {
      Clear();
      m_havePolynomial = enable;
   }

   /*!
    * Disables the use of a stabilizing first-degree polynomial added to the
    * RBF interpolation/approximation function.
    *
    * Calling this member function implicitly resets this %SurfaceSpline object
    * and destroys all internal working structures.
    */
   void DisablePolynomial( bool disable = true )
   {
      EnablePolynomial( !disable );
   }

   /*!
    * Returns the <em>smoothing factor</em> of this surface spline. See
    * SetSmoothing() for more information.
    */
   float Smoothing() const
   {
      return m_smoothing;
   }

   /*!
    * Sets the <em>smoothing factor</em> of this surface spline.
    *
    * \param s    Smoothing factor. Must be &ge; 0.
    *
    * For \a s = 0, an interpolating surface spline will be generated: all node
    * values will be reproduced exactly at their respective coordinates.
    *
    * For \a s > 0, a smoothing (or approximating) surface spline will be
    * generated: increasing \a s values will generate splines closer to the
    * reference plane of the input node set.
    *
    * \note This parameter is only used for the thin plate spline and variable
    * order spline radial basis functions. For other RBFs only interpolation
    * surface splines can be generated with this implementation, and hence this
    * parameter is always ignored.
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
    * For an interpolating surface spline (when smoothness = 0), all node
    * function values will be reproduced exactly at their respective
    * coordinates. In this case the vector \a w of node weights will be
    * ignored.
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
    *
    * \note The smoothing factor and the \a w parameter are only used for the
    * thin plate spline and variable order spline radial basis functions. For
    * other RBFs only interpolation surface splines can be generated with this
    * implementation, and hence the \a w parameter is always ignored.
    */
   void Initialize( const scalar* x, const scalar* y, const scalar* z, int n, const weight* w = nullptr )
   {
      PCL_PRECONDITION( x != nullptr && y != nullptr && z != nullptr )
      PCL_PRECONDITION( n > 2 )

      if ( x == nullptr || y == nullptr || z == nullptr )
         throw Error( "SurfaceSpline::Initialize(): Null vector pointer(s)." );

      if ( n < 3 )
         throw Error( "SurfaceSpline::Initialize(): At least three input nodes must be specified." );

      Clear();

      if ( m_smoothing <= 0 )
         w = nullptr;

      try
      {
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
          * If requested, compute an optimal shape parameter.
          */
         if ( m_eps == 0 )
         {
            Array<double> R2;
            for ( int i = 0; i < n; ++i )
               for ( int j = i; ++j < n; )
               {
                  double dx = x[i] - x[j];
                  double dy = y[i] - y[j];
                  R2 << dx*dx + dy*dy;
               }
            m_eps2 = 1/(m_r0*4*Sqrt( Median( R2.Begin(), R2.End() ) ));
         }
         else
            m_eps2 = m_r0*m_eps;
         m_eps2 *= m_eps2;

         /*
          * Build point list with normalized node coordinates.
          */
         Array<NodeData> P;
         for ( int i = 0; i < n; ++i )
            P << NodeData( m_r0*(x[i] - m_x0),
                           m_r0*(y[i] - m_y0),
                           z[i],
                           (w != nullptr && w[i] > 0) ? w[i] : 1.0F );

         /*
          * Find duplicate input nodes. Two nodes are considered equal if their
          * (normalized) coordinates don't differ more than the machine epsilon
          * for the floating point type T.
          */
         P.Sort();
         Array<int> remove;
         for ( int i = 0, j = 1; j < n; ++i, ++j )
            if ( Abs( P[i].x - P[j].x ) <= std::numeric_limits<scalar>::epsilon() )
               if ( Abs( P[i].y - P[j].y ) <= std::numeric_limits<scalar>::epsilon() )
                  remove << i;

         /*
          * Build working vectors, excluding duplicate input nodes.
          */
         int N = n - int( remove.Length() );
         if ( N < 3 )
            throw Error( "SurfaceSpline::Initialize(): Less than three input nodes left after sanitization." );
         m_x = vector( N );
         m_y = vector( N );
         vector fz( N );
         if ( w != nullptr )
            m_weights = weight_vector( N );
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

         m_spline = vector( scalar( 0 ), N + (m_havePolynomial ? ((m_order*(m_order + 1)) >> 1) : 0) );

         Generate( m_spline.Begin(), m_rbf, m_eps2, m_havePolynomial,
                   m_x.Begin(), m_y.Begin(), fz.Begin(), N, m_order,
                   m_smoothing, m_weights.Begin() );
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
   scalar operator ()( double x, double y ) const
   {
      PCL_PRECONDITION( !m_x.IsEmpty() && !m_y.IsEmpty() )
      PCL_CHECK( m_order >= 2 )
      PCL_CHECK( !m_spline.IsEmpty() )

      /*
       * Normalized interpolation coordinates.
       */
      x = m_r0*(x - m_x0);
      y = m_r0*(y - m_y0);

      /*
       * Add polynomial part of the surface spline.
       */
      int n = m_x.Length();
      double z = 0;
      if ( m_havePolynomial )
      {
         z += m_spline[n];
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
      }

      /*
       * Add radial basis functions.
       */
      switch ( m_rbf )
      {
      case RadialBasisFunction::VariableOrder:
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
               z += m_spline[i] * r2m1 * pcl::Ln( r2 );
            }
         }
         break;
      case RadialBasisFunction::ThinPlateSpline:
         for ( int i = 0; i < n; ++i )
         {
            double dx = m_x[i] - x;
            double dy = m_y[i] - y;
            double r2 = dx*dx + dy*dy;
            if ( r2 != 0 )
               z += m_spline[i] * r2 * pcl::Ln( Sqrt( r2 ) );
         }
         break;
      case RadialBasisFunction::Gaussian:
         for ( int i = 0; i < n; ++i )
         {
            double dx = m_x[i] - x;
            double dy = m_y[i] - y;
            z += m_spline[i] * pcl::Exp( -m_eps2 * (dx*dx + dy*dy) );
         }
         break;
      case RadialBasisFunction::Multiquadric:
         for ( int i = 0; i < n; ++i )
         {
            double dx = m_x[i] - x;
            double dy = m_y[i] - y;
            z += m_spline[i] * pcl::Sqrt( 1 + m_eps2 * (dx*dx + dy*dy) );
         }
         break;
      case RadialBasisFunction::InverseMultiquadric:
         for ( int i = 0; i < n; ++i )
         {
            double dx = m_x[i] - x;
            double dy = m_y[i] - y;
            z += m_spline[i] / pcl::Sqrt( 1 + m_eps2 * (dx*dx + dy*dy) );
         }
         break;
      case RadialBasisFunction::InverseQuadratic:
         for ( int i = 0; i < n; ++i )
         {
            double dx = m_x[i] - x;
            double dy = m_y[i] - y;
            z += m_spline[i] / (1 + m_eps2 * (dx*dx + dy*dy));
         }
         break;
      default:
         break;
      }

      return scalar( z );
   }

   /*!
    * Returns an interpolated/approximated function value at the specified
    * \a p.x and \a p.y point coordinates. See operator()( double, double ) for
    * more information.
    */
   template <typename Tp>
   scalar operator ()( const GenericPoint<Tp>& p ) const
   {
      return operator ()( double( p.x ), double( p.y ) );
   }

protected:

   rbf_type      m_rbf = RadialBasisFunction::Default;
   bool          m_havePolynomial = true; // use 1st order polynomial for stabilization
   double        m_eps = 0;       // shape parameter, 0 -> find optimal value automatically
   double        m_eps2 = 0;      // squared shape parameter
   vector        m_x;             // vector of normalized X node coordinates
   vector        m_y;             // vector of normalized Y node coordinates
   double        m_r0 = 1;        // scaling factor for normalization of node coordinates
   double        m_x0 = 0;        // zero offset for normalization of X node coordinates
   double        m_y0 = 0;        // zero offset for normalization of Y node coordinates
   int           m_order = 2;     // derivative order >= 2
   float         m_smoothing = 0; // <= 0 -> interpolating spline, > 0 -> smoothing factor of approximating spline
   weight_vector m_weights;       // optional node weights for approximating spline
   vector        m_spline;        // coefficients of the 2-D surface spline, polynomial coeffs. at tail

   /*!
    * \struct NodeData
    * \brief Auxiliary structure for data sanitization.
    * \internal
    */
   struct NodeData
   {
      scalar x, y, z;
      weight w;

      NodeData( scalar a_x, scalar a_y, scalar a_z, weight a_w )
         : x( a_x ), y( a_y ), z( a_z ), w( a_w )
      {
      }

      bool operator ==( const NodeData& p ) const
      {
         return x == p.x && y == p.y;
      }

      bool operator <( const NodeData& p ) const
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
 *
 * \sa SurfaceSpline, RecursivePointSurfaceSpline
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
    * Represents a radial basis function (RBF) supported by this point surface
    * spline implementation.
    */
   typedef spline::rbf_type      rbf_type;

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
   template <class weight_vector = FVector>
   PointSurfaceSpline( const point_list& P1, const point_list& P2,
                       float smoothness = 0, int order = 2,
                       const weight_vector& W = weight_vector(),
                       rbf_type rbf = RadialBasisFunction::Default,
                       double eps = 0,
                       bool polynomial = true )
   {
      Initialize( P1, P2, smoothness, W, order, rbf, eps, polynomial );
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
    * \param smoothness Smoothing factor. Must be &ge; 0. The default value is
    *                   zero, that is, interpolating surface splines are
    *                   generated by default.
    *
    * \param W          Reference to a vector of positive node \e weights > 0,
    *                   when the smoothing factor is > 1. For an interpolating
    *                   spline this parameter will be ignored (see below). This
    *                   argument must be a reference to a vector or array of
    *                   \c float values.
    *
    * \param order      Derivative order of continuity. Must be &ge; 2. The
    *                   default value is 2. This parameter is only taken into
    *                   account when the \a rbf parameter is
    *                   RadialBasisFunction::VariableOrder.
    *
    * \param rbf        The type of radial basis function (RBF) used for
    *                   surface spline generation. The default is
    *                   RadialBasisFunction::ThinPlateSpline in the current
    *                   implementation.
    *
    * \param eps        The shape parameter used for decreasing radial basis
    *                   functions. Increasing RBFs, such as thin plate splines
    *                   and variable order functions, are parameter-free and
    *                   ignore this parameter.
    *
    * \param polynomial If true, surface spline generation will include a first
    *                   order polynomial part for stabilization. If false, only
    *                   radial basis functions will be used without a
    *                   polynomial component.
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
    * When the variable order RBF is specified, the surface spline will be
    * continuously differentiable up to the specified \a order. If this order
    * is too high, an ill-conditioned linear system may result, especially for
    * large data sets. The recommended values are 2, 3, 4 or 5, but order 2 is
    * usually the most useful and safest option for most applications.
    * Derivative orders greater than 3 may lead to numerically unstable
    * interpolation devices, which should always be used with care. For the
    * rest of RBFs the \a order parameter is ignored and second order functions
    * are always used.
    *
    * \note The \a smoothness and \a W parameters are only used for the thin
    * plate spline and variable order spline radial basis functions. For other
    * RBFs only interpolation surface splines can be generated with this
    * implementation, and hence these parameters are always ignored.
    */
   template <class weight_vector = FVector>
   void Initialize( const point_list& P1, const point_list& P2,
                    float smoothness = 0, const weight_vector& W = weight_vector(), int order = 2,
                    rbf_type rbf = RadialBasisFunction::Default,
                    double eps = 0,
                    bool polynomial = true )
   {
      PCL_PRECONDITION( P1.Length() >= 3 )
      PCL_PRECONDITION( P1.Length() <= P2.Length() )
      PCL_PRECONDITION( order >= 2 )
      PCL_PRECONDITION( W.IsEmpty() || P1.Length() <= W.Length() )

      Clear();

      if ( P1.Length() < 3 || P2.Length() < 3 )
         throw Error( "PointSurfaceSpline::Initialize(): At least three input nodes must be specified." );

      if ( P1.Length() > P2.Length() ||
           !W.IsEmpty() && P1.Length() > size_type( W.Length() ) )
         throw Error( "PointSurfaceSpline::Initialize(): Insufficient data." );

      m_Sx.SetRBFType( rbf );
      m_Sx.SetOrder( order );
      m_Sx.SetShapeParameter( eps );
      m_Sx.EnablePolynomial( polynomial );
      m_Sx.SetSmoothing( smoothness );

      m_Sy.SetRBFType( rbf );
      m_Sy.SetOrder( order );
      m_Sy.SetShapeParameter( eps );
      m_Sy.EnablePolynomial( polynomial );
      m_Sy.SetSmoothing( smoothness );

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
    * \internal
    * Internal-use initialization routine.
    */
   void Initialize( const DVector& X1, const DVector& Y1, const DVector& Z1,
                    const DVector& X2, const DVector& Y2, const DVector& Z2,
                    float smoothness = 0,
                    const FVector& W1 = FVector(), const FVector& W2 = FVector(),
                    int order = 2,
                    rbf_type rbf = RadialBasisFunction::Default,
                    double eps = 0,
                    bool polynomial = true )
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

      Clear();

      if ( X1.Length() < 3 || X2.Length() < 3 )
         throw Error( "PointSurfaceSpline::Initialize(): At least three input nodes must be specified." );

      if ( X1.Length() > Y1.Length() ||
           X1.Length() > Z1.Length() ||
           !W1.IsEmpty() && X1.Length() > W1.Length() ||
           X2.Length() > Y2.Length() ||
           X2.Length() > Z2.Length() ||
           !W2.IsEmpty() && X2.Length() > W2.Length() )
         throw Error( "PointSurfaceSpline::Initialize(): Insufficient data." );

      m_Sx.SetRBFType( rbf );
      m_Sx.SetOrder( order );
      m_Sx.SetShapeParameter( eps );
      m_Sx.EnablePolynomial( polynomial );
      m_Sx.SetSmoothing( smoothness );
      m_Sx.Initialize( X1.Begin(), Y1.Begin(), Z1.Begin(), X1.Length(), W1.Begin() );

      m_Sy.SetRBFType( rbf );
      m_Sy.SetOrder( order );
      m_Sy.SetShapeParameter( eps );
      m_Sy.EnablePolynomial( polynomial );
      m_Sy.SetSmoothing( smoothness );
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
      Clear();
      if ( Sx.IsValid() && Sy.IsValid() )
      {
         m_Sx = Sx;
         m_Sy = Sy;
      }
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

         inverse.m_Sx.SetRBFType( m_Sx.RBFType() );
         inverse.m_Sx.SetOrder( m_Sx.Order() );
         inverse.m_Sx.SetShapeParameter( m_Sx.ShapeParameter() );
         inverse.m_Sx.EnablePolynomial( m_Sx.IsPolynomialEnabled() );
         inverse.m_Sx.Initialize( Zx.Begin(), Zy.Begin(), X.Begin(), X.Length() );

         inverse.m_Sy.SetRBFType( m_Sy.RBFType() );
         inverse.m_Sy.SetOrder( m_Sy.Order() );
         inverse.m_Sy.SetShapeParameter( m_Sy.ShapeParameter() );
         inverse.m_Sy.EnablePolynomial( m_Sy.IsPolynomialEnabled() );
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

/*!
 * \class RecursivePointSurfaceSpline
 * \brief Vector surface spline interpolation/approximation in two dimensions
 * with recursive subspline generation.
 *
 * This class is a functional replacement for PointSurfaceSpline. It splits the
 * interpolation region recursively using quadtrees. At each quadtree leaf node
 * a subspline is generated with a subset of the interpolation nodes, with
 * additional data redundancy to ensure smoothness across subregion boundaries.
 *
 * This recursive scheme is appropriate for large-scale problems, where single
 * thin plate splines can be impractical because their generation has O(n^3)
 * time complexity. An instance of %RecursivePointSurfaceSpline can be built
 * with an unlimited number of arbitrarily distributed interpolation points.
 *
 * \sa PointSurfaceSpline, SurfaceSpline
 */
template <class P = DPoint>
class PCL_CLASS RecursivePointSurfaceSpline : public ParallelProcess
{
public:

   /*!
    * Represents a point surface spline.
    */
   typedef PointSurfaceSpline<P>             spline;

   /*
    * Represents a recursive point surface spline.
    */
   typedef RecursivePointSurfaceSpline<P>    recursive_spline;

   /*!
    * Represents an interpolation point in two dimensions.
    */
   typedef typename spline::point            point;

   /*!
    * Represents a sequence of interpolation points.
    */
   typedef typename spline::point_list       point_list;

   /*!
    * Default constructor. Yields an empty instance that cannot be used without
    * initialization.
    */
   RecursivePointSurfaceSpline() = default;

   /*!
    * Copy constructor. Copy construction is disabled because this class uses
    * internal data structures that cannot be copy-constructed. However,
    * %RecursivePointSurfaceSpline implements move construction and move
    * assignment.
    */
   RecursivePointSurfaceSpline( const RecursivePointSurfaceSpline& ) = delete;

   /*!
    * Copy assignment operator. Copy assignment is disabled because this class
    * uses internal data structures that cannot be copy-assigned. However,
    * %RecursivePointSurfaceSpline implements move assignment and move
    * construction.
    */
   RecursivePointSurfaceSpline& operator =( const RecursivePointSurfaceSpline& ) = delete;

   /*!
    * Move constructor.
    */
   RecursivePointSurfaceSpline( RecursivePointSurfaceSpline&& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   RecursivePointSurfaceSpline& operator =( RecursivePointSurfaceSpline&& ) = default;

   /*!
    * Constructs a %RecursivePointSurfaceSpline object initialized for the
    * specified input data and interpolation parameters.
    *
    * See the corresponding Initialize() member function for a detailed
    * description of parameters.
    */
   template <class weight_vector = FVector>
   RecursivePointSurfaceSpline( const point_list& P1, const point_list& P2,
                                float smoothness = 0,
                                int order = 2,
                                const weight_vector& W = weight_vector(),
                                bool allowExtrapolation = __PCL_RSSPLINE_DEFAULT_ALLOW_EXTRAPOLATION,
                                int maxSplineLength = __PCL_RSSPLINE_DEFAULT_SPLINE_MAX_LENGTH,
                                int bucketCapacity = __PCL_RSSPLINE_DEFAULT_TREE_BUCKET_CAPACITY,
                                bool verbose = true )
   {
      Initialize( P1, P2, smoothness, W, order, allowExtrapolation, maxSplineLength, bucketCapacity, verbose );
   }

   /*!
    * Virtual destructor.
    */
   virtual ~RecursivePointSurfaceSpline()
   {
      Clear();
   }

   /*!
    * Initializes this %RecursivePointSurfaceSpline object for the specified
    * input data and interpolation parameters.
    *
    * \param P1         A sequence of distinct interpolation node points.
    *
    * \param P2         A sequence of interpolation values. For each point in
    *                   \a P1, the coordinates of its counterpart point in
    *                   \a P2 will be used as the interpolation node values in
    *                   the X and Y directions.
    *
    * \param smoothness Smoothing factor. Must be &ge; 0 (default = 0).
    *
    * \param W          Reference to a vector of positive node \e weights > 0,
    *                   when the smoothing factor is > 1. For an interpolating
    *                   spline this parameter will be ignored (see below). This
    *                   argument must be a reference to a vector or array of
    *                   \c float values.
    *
    * \param order      Derivative order. Must be &ge; 2 (default = 2).
    *
    * \param allowExtrapolation  Whether to allow extrapolation on points
    *                   exterior to the region defined by the specified set of
    *                   interpolation node points \a P1. Extrapolation is not
    *                   allowed by default because recursively generated
    *                   subsplines are slightly more prone to oscillation than
    *                   normal surface splines.
    *
    * \param maxSplineLength  Maximum length of a point surface spline in a
    *                   non-recursive %RecursivePointSurfaceSpline instance.
    *                   The default value is 1600 points. Be aware that surface
    *                   spline generation has O(n^3) time complexity (that's
    *                   why this class exists after all), and the number of
    *                   generated quadtree nodes grows exponentially with the
    *                   total number of interpolation points. The default value
    *                   is a reasonable compromise for balanced efficiency in
    *                   most practical cases.
    *
    * \param bucketCapacity   Bucket capacity for quadtree generation. The
    *                   default value is 64 points. By decreasing this value
    *                   the recursive surface spline initialization process
    *                   will be faster, but the generated subsplines will be
    *                   less accurate, leading to more border artifacts between
    *                   contiguous quadtree node regions.
    *
    * \param verbose    If true, this function will write information to the
    *                   standard PixInsight console to provide some feedback to
    *                   the user during the (potentially long) initialization
    *                   process. If false, no feedback will be provided.
    *
    * For \a smoothness &le; 0, interpolating subsplines will be generated: all
    * node values will be reproduced exactly at their respective coordinates.
    * In this case the \a W parameter will be ignored.
    *
    * For \a smoothness > 0, smoothing (or approximating) subsplines will be
    * generated: increasing \a smoothness values will generate splines closer
    * to the reference planes of their input node sets. If nonempty, the
    * specified vector \a W of node weights will be used to assign a different
    * interpolation strength to each interpolation node. In this case the
    * vector \a W must have at least the same length as the shortest of the
    * \a P1 and \a P2 vectors, and must contain values greater than zero. A
    * node weight larger than one will reduce the smoothness of the
    * interpolating surface at the corresponding node coordinates, or in other
    * words, it will give more prominence to the corresponding data point. A
    * node weight of one will apply the specified \a smoothness at its node
    * position. Contrarily, a node weight smaller than one will increase the
    * interpolation smoothness locally.
    *
    * The generated surface subsplines will be continuously differentiable up
    * to the specified \a order. If this order is too high, ill-conditioned
    * linear systems may result, especially for large data sets. The
    * recommended values are 2, 3, 4 or 5, but order 2 is usually the most
    * useful and safest option for most applications. Derivative orders greater
    * than 3 may lead to numerically unstable interpolation devices, which
    * should always be used with care.
    */
   template <class weight_vector = FVector>
   void Initialize( const point_list& P1, const point_list& P2,
                    float smoothness = 0,
                    const weight_vector& W = weight_vector(),
                    int order = 2,
                    bool allowExtrapolation = __PCL_RSSPLINE_DEFAULT_ALLOW_EXTRAPOLATION,
                    int maxSplineLength = __PCL_RSSPLINE_DEFAULT_SPLINE_MAX_LENGTH,
                    int bucketCapacity = __PCL_RSSPLINE_DEFAULT_TREE_BUCKET_CAPACITY,
                    bool verbose = true )
   {
      PCL_PRECONDITION( P1.Length() >= 3 )
      PCL_PRECONDITION( P1.Length() <= P2.Length() )
      PCL_PRECONDITION( order >= 2 )
      PCL_PRECONDITION( W.IsEmpty() || P1.Length() <= W.Length() )

      Clear();

      if ( P1.Length() < 3 || P2.Length() < 3 )
         throw Error( "RecursivePointSurfaceSpline::Initialize(): At least three input nodes must be specified." );

      bool weighted = smoothness > 0 && !W.IsEmpty();

      if ( P1.Length() > P2.Length() || weighted && P1.Length() > size_type( W.Length() ) )
         throw Error( "RecursivePointSurfaceSpline::Initialize(): Insufficient data." );

      m_extrapolate = allowExtrapolation;

      if ( P1.Length() <= size_type( maxSplineLength ) )
      {
         StatusMonitor monitor;
#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
         StandardStatus status;
         if ( verbose )
         {
            monitor.SetCallback( &status );
            monitor.Initialize( "Building surface subsplines", 100 );
         }
#endif
         for ( const auto& p : P1 )
         {
            if ( p.x < m_rect.x0 )
               m_rect.x0 = p.x;
            else if ( p.x > m_rect.x1 )
               m_rect.x1 = p.x;
            if ( p.y < m_rect.y0 )
               m_rect.y0 = p.y;
            else if ( p.y > m_rect.y1 )
               m_rect.y1 = p.y;
         }

         m_spline.Initialize( P1, P2, smoothness, W, order,
                              (order == 2) ? RadialBasisFunction::ThinPlateSpline
                                           : RadialBasisFunction::VariableOrder );

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
         if ( verbose )
            monitor.Complete();
#endif
      }
      else
      {
         /*
          * Find the total interpolation region.
          */
         search_rectangle rect = search_coordinate( 0 );
         node_list data;
         for ( size_type i = 0; i < P1.Length(); ++i )
         {
            const point& p1 = P1[i];
            const point& p2 = P2[i];
            data << (weighted ? Node( p1, p2, W[i] ) : Node( p1, p2 ));
            if ( p1.x < rect.x0 )
               rect.x0 = p1.x;
            else if ( p1.x > rect.x1 )
               rect.x1 = p1.x;
            if ( p1.y < rect.y0 )
               rect.y0 = p1.y;
            else if ( p1.y > rect.y1 )
               rect.y1 = p1.y;
         }
         // Force a square interpolation region for optimal quadtree behavior.
         if ( rect.Width() < rect.Height() )
            rect.InflateBy( (rect.Height() - rect.Width())/2, search_coordinate( 0 ) );
         else
            rect.InflateBy( search_coordinate( 0 ), (rect.Width() - rect.Height())/2 );

         /*
          * Build the interpolation quadtree.
          */
         m_tree.Build( rect, data, bucketCapacity );

         /*
          * Build subspline interpolation data.
          */
         Array<SubsplineData> subsplineData;
         m_tree.Traverse(
            [&]( const search_rectangle& rect, const node_list& points, void*& D )
            {
               /*
                * Ensure redundancy for all subsplines by gathering a set of
                * interpolation points in an extended rectangular region around
                * each quadtree node.
                */
               search_rectangle r = rect.InflatedBy( 1.5*Max( rect.Width(), rect.Height() ) );
               node_list N = m_tree.Search( r );

               /*
                * Sort the cloud of interpolation points by proximity to a
                * circle centered at the argument of the minimum RBF value
                * (approximately 0.61 in normalized coordinates).
                */
               if ( N.Length() > size_type( maxSplineLength ) )
               {
                  point c = rect.Center();
                  double d = 0.61 * (Max( r.Width(), r.Height() )/2 + Max( rect.Width(), rect.Height() )/4);
                  N.Sort(
                     [&]( const auto& a, const auto& b )
                     {
                        return Abs( a.position.DistanceTo( c ) - d ) < Abs( b.position.DistanceTo( c ) - d );
                     } );
               }

               /*
                * Get the optimal subset of at most maxSplineLength redundant
                * interpolation points for this quadtree node.
                */
               point_list P1, P2;
               Array<float> PW;
               for ( int j = 0, l = Min( int( N.Length() ), maxSplineLength ); j < l; ++j )
               {
                  P1 << N[j].position;
                  P2 << N[j].value;
                  if ( weighted )
                     PW << N[j].weight;
               }

               subsplineData << SubsplineData( P1, P2, PW, D );
            } );

         /*
          * Generate all subsplines.
          */
         StatusMonitor monitor;
#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
         StandardStatus status;
         if ( verbose )
         {
            monitor.SetCallback( &status );
            monitor.Initialize( "Building recursive surface subsplines", subsplineData.Length() );
         }
#endif
         Array<size_type> L = Thread::OptimalThreadLoads( subsplineData.Length(),
                                                          1/*overheadLimit*/,
                                                          m_parallel ? m_maxProcessors : 1 );
         AbstractImage::ThreadData threadData( monitor, subsplineData.Length() );
         ReferenceArray<SubsplineGenerationThread> threads;
         for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
            threads.Add( new SubsplineGenerationThread( threadData,
                                                        subsplineData,
                                                        smoothness,
                                                        order,
                                                        allowExtrapolation,
                                                        maxSplineLength,
                                                        bucketCapacity,
                                                        n,
                                                        n + int( L[i] ) ) );
         AbstractImage::RunThreads( threads, threadData );
         threads.Destroy();
      }
   }

   /*!
    * Deallocates internal structures, yielding an empty object that cannot be
    * used before a new call to Initialize().
    */
   void Clear()
   {
      m_tree.Traverse(
         []( const search_rectangle&, node_list&, void*& D )
         {
            delete reinterpret_cast<recursive_spline*&>( D );
         } );
      m_tree.Clear();
      m_spline.Clear();
      m_rect = search_coordinate( 0 );
   }

   /*!
    * Returns true if this object splits the interpolation space recursively
    * with subsplines. In such case this object contains a quadtree structure
    * that will be searched for suitable point surface splines to interpolate
    * function values.
    *
    * Returns false is this object performs no further recursion. In such case
    * this object contains a point surface spline that will be used directly to
    * interpolate function values.
    */
   bool IsRecursive() const
   {
      return !m_tree.IsEmpty();
   }

   /*!
    * Returns true iff this is a valid, initialized object ready for
    * interpolation.
    */
   bool IsValid() const
   {
      return IsRecursive() || m_spline.IsValid();
   }

   /*!
    * Returns an interpolated point at the specified \a x, \a y coordinates.
    *
    * If extrapolation is disabled for this object and the specified
    * coordinates fall outside the interpolation region defined upon
    * initialization, a point at the origin (that is, with zero coordinates) is
    * returned conventionally. Otherwise the nearest subspline will be used to
    * extrapolate the returned value.
    */
   template <typename T>
   DPoint operator ()( T x, T y ) const
   {
      if ( m_spline.IsValid() )
      {
         if ( m_extrapolate || m_rect.IncludesFast( x, y ) )
            return m_spline( x, y );
      }
      else
      {
         const typename tree::Node* node = m_tree.NodeAt( search_point( x, y ) );
         if ( node == nullptr )
         {
            if ( !m_extrapolate )
               return 0.0;

            search_rectangle r0 = m_tree.Root()->rect;
            if ( x <= r0.x0 )
            {
               if ( y <= r0.y0 )
                  node = m_tree.NodeAt( search_point( r0.x0 + SearchDelta, r0.y0 + SearchDelta ) );
               else if ( y >= r0.y1 )
                  node = m_tree.NodeAt( search_point( r0.x0 + SearchDelta, r0.y1 - SearchDelta ) );
               else
                  node = m_tree.NodeAt( search_point( r0.x0 + SearchDelta, search_coordinate( y ) ) );
            }
            else if ( x >= r0.x1 )
            {
               if ( y <= r0.y0 )
                  node = m_tree.NodeAt( search_point( r0.x1 - SearchDelta, r0.y0 + SearchDelta ) );
               else if ( y >= r0.y1 )
                  node = m_tree.NodeAt( search_point( r0.x1 - SearchDelta, r0.y1 - SearchDelta ) );
               else
                  node = m_tree.NodeAt( search_point( r0.x1 - SearchDelta, search_coordinate( y ) ) );
            }
            else if ( y <= r0.y0 )
               node = m_tree.NodeAt( search_point( search_coordinate( x ), r0.y0 + SearchDelta ) );
            else // y >= r0.y1
               node = m_tree.NodeAt( search_point( search_coordinate( x ), r0.y1 - SearchDelta ) );

            if ( node == nullptr ) // ?!
               return 0.0;
         }

         if ( node->IsLeaf() )
         {
            const typename tree::LeafNode* leaf = static_cast<const typename tree::LeafNode*>( node );
            if ( leaf->data == nullptr ) // ?!
               return 0.0;
            return reinterpret_cast<recursive_spline*>( leaf->data )->operator()( x, y );
         }

         DPoint p = 0.0;
         int n = 0;
         if ( node->nw != nullptr )
         {
            const typename tree::LeafNode* leaf;
            if ( y <= node->nw->rect.y1 )
               leaf = m_tree.LeafNodeAt( search_point( node->nw->rect.x1 - SearchDelta, search_coordinate( y ) ) );
            else if ( x <= node->nw->rect.x1 )
               leaf = m_tree.LeafNodeAt( search_point( search_coordinate( x ), node->nw->rect.y1 - SearchDelta ) );
            else
               leaf = m_tree.LeafNodeAt( search_point( node->nw->rect.x1 - SearchDelta, node->nw->rect.y1 - SearchDelta ) );

            if ( leaf != nullptr )
            {
               p += reinterpret_cast<recursive_spline*>( leaf->data )->operator()( x, y );
               ++n;
            }
         }
         if ( node->ne != nullptr )
         {
            const typename tree::LeafNode* leaf;
            if ( y <= node->ne->rect.y1 )
               leaf = m_tree.LeafNodeAt( search_point( node->ne->rect.x0 + SearchDelta, search_coordinate( y ) ) );
            else if ( x <= node->ne->rect.x0 )
               leaf = m_tree.LeafNodeAt( search_point( node->ne->rect.x0 + SearchDelta, node->ne->rect.y1 - SearchDelta ) );
            else
               leaf = m_tree.LeafNodeAt( search_point( search_coordinate( x ), node->ne->rect.y1 - SearchDelta ) );

            if ( leaf != nullptr )
            {
               p += reinterpret_cast<recursive_spline*>( leaf->data )->operator()( x, y );
               ++n;
            }
         }
         if ( node->sw != nullptr )
         {
            const typename tree::LeafNode* leaf;
            if ( y >= node->sw->rect.y0 )
               leaf = m_tree.LeafNodeAt( search_point( node->sw->rect.x1 - SearchDelta, search_coordinate( y ) ) );
            else if ( x <= node->sw->rect.x1 )
               leaf = m_tree.LeafNodeAt( search_point( search_coordinate( x ), node->sw->rect.y0 + SearchDelta ) );
            else
               leaf = m_tree.LeafNodeAt( search_point( node->sw->rect.x1 - SearchDelta, node->sw->rect.y0 + SearchDelta ) );

            if ( leaf != nullptr )
            {
               p += reinterpret_cast<recursive_spline*>( leaf->data )->operator()( x, y );
               ++n;
            }
         }
         if ( node->se != nullptr )
         {
            const typename tree::LeafNode* leaf;
            if ( y >= node->se->rect.y0 )
               leaf = m_tree.LeafNodeAt( search_point( node->se->rect.x0 + SearchDelta, search_coordinate( y ) ) );
            else if ( x <= node->se->rect.x0 )
               leaf = m_tree.LeafNodeAt( search_point( node->se->rect.x0 + SearchDelta, node->se->rect.y0 + SearchDelta ) );
            else
               leaf = m_tree.LeafNodeAt( search_point( search_coordinate( x ), node->se->rect.y0 + SearchDelta ) );

            if ( leaf != nullptr )
            {
               p += reinterpret_cast<recursive_spline*>( leaf->data )->operator()( x, y );
               ++n;
            }
         }

         if ( n > 0 )
            return p/double( n );
      }

      return 0.0;
   }

   /*!
    * Returns an interpolated point at the given \a p.x and \a p.y coordinates.
    *
    * If extrapolation is disabled for this object and the specified point
    * lies outside the interpolation region defined upon initialization, a
    * point at the origin (that is, with zero coordinates) is returned
    * conventionally. Otherwise the nearest subspline will be used to
    * extrapolate the returned value.
    */
   template <typename T>
   DPoint operator ()( const GenericPoint<T>& p ) const
   {
      return operator ()( p.x, p.y );
   }

private:

   /*
    * Interpolation data point, QuadTree-compatible.
    */
   struct Node
   {
      typedef typename point::component   component;

      point position, value;
      float weight;

      Node( const point& p, const point& v )
         : position( p )
         , value( v )
      {
      }

      Node( const point& p, const point& v, float w )
         : position( p )
         , value( v )
         , weight( w )
      {
      }

      Node( const Node& ) = default;

      component& operator []( int i )
      {
         return position[i];
      }

      component operator []( int i ) const
      {
         return position[i];
      }
   };

   typedef QuadTree<Node>                  tree;
   typedef typename tree::point_list       node_list;
   typedef typename tree::rectangle        search_rectangle;
   typedef typename tree::coordinate       search_coordinate;
   typedef GenericPoint<search_coordinate> search_point;

   tree             m_tree;   // the tree of subsplines
   spline           m_spline; // final point spline if there is no further recursion
   search_rectangle m_rect = search_coordinate( 0 ); // the interpolation region for this subspline
   bool             m_extrapolate = __PCL_RSSPLINE_DEFAULT_ALLOW_EXTRAPOLATION;

   static constexpr search_coordinate SearchDelta =
                        2 * std::numeric_limits<search_coordinate>::epsilon();

   /*
    * Parallel subspline generation.
    */
   struct SubsplineData
   {
      point_list     P1, P2;
      Array<float>   PW;
      mutable void** nodeData;

      SubsplineData( const point_list& p1, const point_list& p2, const Array<float>& pw, void*& nd )
         : P1( p1 )
         , P2( p2 )
         , PW( pw )
         , nodeData( &nd )
      {
      }
   };

   class SubsplineGenerationThread : public Thread
   {
   public:

      SubsplineGenerationThread( const AbstractImage::ThreadData& data,
                                 const Array<SubsplineData>& subsplineData,
                                 float smoothness,
                                 int order,
                                 bool allowExtrapolation,
                                 int maxSplineLength,
                                 int bucketCapacity,
                                 int startIndex, int endIndex )
         : m_data( data )
         , m_subsplineData( subsplineData )
         , m_smoothness( smoothness )
         , m_order( order )
         , m_allowExtrapolation( allowExtrapolation )
         , m_maxSplineLength( maxSplineLength )
         , m_bucketCapacity( bucketCapacity )
         , m_startIndex( startIndex )
         , m_endIndex( endIndex )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         for ( int i = m_startIndex; i < m_endIndex; ++i )
         {
            const SubsplineData& d = m_subsplineData[i];
            AutoPointer<recursive_spline> s(
               new recursive_spline( d.P1, d.P2, m_smoothness, m_order, d.PW,
                                     m_allowExtrapolation,
                                     m_maxSplineLength,
                                     m_bucketCapacity,
                                     false/*verbose*/ ) );
            *reinterpret_cast<recursive_spline**>( d.nodeData ) = s.Release();

            UPDATE_THREAD_MONITOR( 1 )
         }

         m_success = true;
      }

      operator bool() const
      {
         return m_success;
      }

   private:

      const AbstractImage::ThreadData& m_data;
      const Array<SubsplineData>&      m_subsplineData;
            float                      m_smoothness;
            int                        m_order;
            bool                       m_allowExtrapolation;
            int                        m_maxSplineLength;
            int                        m_bucketCapacity;
            int                        m_startIndex, m_endIndex;
            bool                       m_success = false;
   };

   friend class DrizzleData;
   friend class DrizzleDataDecoder;
};

// ----------------------------------------------------------------------------

}  // pcl

#endif   // __PCL_SurfaceSpline_h

// ----------------------------------------------------------------------------
// EOF pcl/SurfaceSpline.h - Released 2022-03-12T18:59:29Z
