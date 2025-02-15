//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/GridInterpolation.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_GridInterpolation_h
#define __PCL_GridInterpolation_h

/// \file pcl/GridInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AbstractImage.h>
#include <pcl/BicubicInterpolation.h>
#include <pcl/Matrix.h>
#include <pcl/ParallelProcess.h>
#include <pcl/Rectangle.h>
#include <pcl/ReferenceArray.h>
#include <pcl/Thread.h>


#include <pcl/File.h>
#include <pcl/TimePoint.h>



#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
#  include <pcl/Console.h>
#  include <pcl/StandardStatus.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class GridInterpolation
 * \brief Discretized scalar surface interpolation/approximation in two
 *        dimensions.
 *
 * This class performs the same tasks as a surface interpolation device, such
 * as SurfaceSpline or ShepardInterpolation, but allows for much faster
 * interpolation with negligible accuracy loss in most applications.
 *
 * Interpolation from discrete grids can be orders of magnitude faster than
 * direct evaluation of surface interpolation/approximation devices, depending
 * on the number of input data points.
 */
class PCL_CLASS GridInterpolation : public ParallelProcess
{
public:

   /*!
    * Default constructor. Yields an empty instance that cannot be used without
    * initialization.
    */
   GridInterpolation() = default;

   /*!
    * Copy constructor.
    */
   GridInterpolation( const GridInterpolation& ) = default;

   /*!
    * Move constructor.
    */
   GridInterpolation( GridInterpolation&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   GridInterpolation& operator =( const GridInterpolation& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   GridInterpolation& operator =( GridInterpolation&& ) = default;

   /*!
    * Initializes this %GridInterpolation object for the specified input data
    * and interpolation parameters.
    *
    * \param rect    Reference rectangle. Interpolation will be initialized
    *                within the boundaries of this rectangle at discrete
    *                \a delta coordinate intervals.
    *
    * \param delta   Grid distance for calculation of discrete function values.
    *                Must be &gt; 0.
    *
    * \param S       Reference to a surface interpolation/approximation object
    *                that will be used to evaluate function values at discrete
    *                coordinate intervals. This object must have been
    *                previously initialized and must be valid.
    *
    * \param verbose If true, this function will write information to the
    *                standard PixInsight console to provide some feedback to
    *                the user during the (potentially long) initialization
    *                process. If false, no feedback will be provided.
    *
    * The template parameter SI must implement the following member functions:
    *
    * 1. Function evaluation operator:
    *
    * double SI::operator ()( double x, double y ) const
    *
    * or an equivalent operator member function whose return value can be
    * statically casted to double, with two by-value parameters that can be
    * statically casted from the double type. This function will be called
    * multiple times for the \a S object to evaluate the approximated surface
    * at discrete grid coordinate pairs {x,y}, respectively on the X and Y
    * plane directions. The implementation of this member function must be
    * thread-safe if parallel processing has been enabled and is allowed for
    * this object.
    *
    * 2. Vector evaluation function:
    *
    * void SI::Evaluate( double* Z,\n
    *                    const double* X, const double* Y, size_type n ) const
    *
    * This function will be called multiple times for the \a S object for fast
    * multithreaded surface evaluation at a set of points in 2-D space
    * specified as the \a X and \a Y contiguous sequences of \a n coordinates.
    * The function must store the corresponding function values in the \a Z
    * array. The implementation of this member function must be thread-safe if
    * parallel processing has been enabled and is allowed for this object.
    *
    * 3. Information function:
    *
    * bool SI::HasFastVectorEvaluation() const
    *
    * This member function must return true if the \a S object provides fast
    * surface evaluation for vectors of points with the Evaluate() function. If
    * this function returns false for \a S, the operator ()() described above
    * will be called exclusively from grid generation threads.
    *
    * If parallel processing is allowed, this function executes the
    * initialization process using multiple concurrent threads. See
    * EnableParallelProcessing() for additional information.
    */
   template <class R, class SI>
   void Initialize( const R& rect, int delta, const SI& S, bool verbose = true )
   {
      PCL_PRECONDITION( rect.IsRect() )
      PCL_PRECONDITION( delta > 0 )

      m_rect = rect.Ordered();
      double width = m_rect.Width();
      double height = m_rect.Height();
      if ( !m_rect.IsRect() || 1 + width == 1 || 1 + height == 1 )
         throw Error( "GridInterpolation::Initialize(): Empty interpolation space." );

      m_delta = Abs( delta );
      if ( 1 + m_delta == 1 )
         throw Error( "GridInterpolation::Initialize(): Zero or insignificant grid distance." );

      int rows = 1 + int( Ceil( height/m_delta ) );
      int cols = 1 + int( Ceil( width/m_delta ) );
      m_G = DMatrix( rows, cols );

      StatusMonitor monitor;
#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
      StandardStatus status;
      if ( verbose )
      {
         monitor.SetCallback( &status );
         monitor.Initialize( "Building surface interpolation grid", rows );
      }
#endif

      Array<size_type> L = Thread::OptimalThreadLoads( rows,
                                                       1/*overheadLimit*/,
                                                       m_parallel ? m_maxProcessors : 1 );
      AbstractImage::ThreadData data( monitor, rows );
      ReferenceArray<GridInitializationThread<SI> > threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new GridInitializationThread<SI>( data, *this, S, n, n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      m_I.Initialize( m_G.Begin(), cols, rows );
   }

   /*!
    * Initializes this %GridInterpolation object with a prescribed discrete
    * interpolation matrix.
    *
    * \param rect    Reference rectangle. Interpolation will be initialized
    *                within the boundaries of this rectangle at discrete
    *                \a delta coordinate intervals.
    *
    * \param delta   Grid distance for calculation of discrete function values.
    *                Must be &gt; 0.
    *
    * \param G       interpolation matrix.
    *
    * The specified \a G matrix must have \a n rows and \a m columns, which are
    * given by:
    *
    * n = 1 + Ceil( rect.Height()/delta )\n
    * m = 1 + Ceil( rect.Width()/delta )
    *
    * If the dimensions of the specified matrix are different from the values
    * defined above, this function will throw an Error exception.
    *
    * Matrix elements must be function values computed at discrete \a delta
    * intervals within \a rect boundaries. For a given matrix row \a r and
    * matrix column \a c, the corresponding matrix element must be a function
    * value computed at coordinates {\a x,\a y} given by:
    *
    * x = rect.x0 + c*delta\n
    * y = rect.y0 + r*delta
    */
   template <class R>
   void Initialize( const R& rect, double delta, const DMatrix& G )
   {
      PCL_PRECONDITION( rect.IsRect() )
      PCL_PRECONDITION( delta > 0 )

      m_rect = DRect( rect.Ordered() );
      double width = m_rect.Width();
      double height = m_rect.Height();
      if ( !m_rect.IsRect() || 1 + width == 1 || 1 + height == 1 )
         throw Error( "GridInterpolation::Initialize(): Empty interpolation space." );

      m_delta = Abs( delta );
      if ( 1 + m_delta == 1 )
         throw Error( "GridInterpolation::Initialize(): Zero or insignificant grid distance." );

      int rows = 1 + int( Ceil( height/m_delta ) );
      int cols = 1 + int( Ceil( width/m_delta ) );
      if ( G.Rows() != rows || G.Cols() != cols )
         throw Error( "GridInterpolation::Initialize(): Invalid matrix dimensions." );

      m_G = G;
      m_I.Initialize( m_G.Begin(), cols, rows );
   }

   /*!
    * Returns true iff this is a valid, initialized object ready for
    * interpolation.
    */
   bool IsValid() const
   {
      return !m_G.IsEmpty();
   }

   /*!
    * Deallocates internal structures, yielding an empty object that cannot be
    * used before a new call to Initialize().
    */
   void Clear()
   {
      m_I.Clear();
      m_G.Clear();
   }

   /*!
    * Returns the current interpolation reference rectangle. See Initialize()
    * for more information.
    */
   const DRect& ReferenceRect() const
   {
      return m_rect;
   }

   /*!
    * Returns the current grid distance for calculation of discrete function
    * values. See Initialize() for more information.
    */
   double Delta() const
   {
      return m_delta;
   }

   /*!
    * Returns a reference to the discrete matrix used for interpolation of
    * function values.
    *
    * If this object has not been initialized, this member function returns a
    * reference to an empty matrix.
    */
   const DMatrix& InterpolationMatrix() const
   {
      return m_G;
   }

   /*!
    * Returns an interpolated function value at the specified coordinates.
    */
   template <typename T>
   double operator ()( T x, T y ) const
   {
      double fx = (double( x ) - m_rect.x0)/m_delta;
      double fy = (double( y ) - m_rect.y0)/m_delta;
      return m_I( fx, fy );
   }

   /*!
    * Returns an interpolated function value at \a p.x and \a p.y coordinates.
    */
   template <typename T>
   double operator ()( const GenericPoint<T>& p ) const
   {
      return operator ()( p.x, p.y );
   }

private:

   /*!
    * N.B.: ** Critical requirement ** We need a smooth interpolation function
    * without negative lobes here to prevent small-scale oscillations. Other
    * options are BilinearInterpolation and CubicBSplineFilter.
    */
   using grid_interpolation = BicubicBSplineInterpolation<double>;

   DRect              m_rect;
   double             m_delta;
   DMatrix            m_G;
   grid_interpolation m_I;

   template <class SI>
   class GridInitializationThread : public Thread
   {
   public:

      GridInitializationThread( const AbstractImage::ThreadData& data,
                                GridInterpolation& grid, const SI& surface, int startRow, int endRow )
         : m_data( data )
         , m_grid( grid )
         , m_surface( surface )
         , m_startRow( startRow )
         , m_endRow( endRow )
      {
         if ( m_surface.HasFastVectorEvaluation() )
         {
            m_N = int( Min( size_type( 4096 ), size_type( m_endRow - m_startRow )*m_grid.m_G.Cols() ) );
            m_X = DVector( m_N );
            m_Y = DVector( m_N );
            m_Z = DVector( m_N );
         }
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         if ( m_surface.HasFastVectorEvaluation() )
         {
            int i0 = m_startRow, j0 = 0, n = 0;
            double y = m_grid.m_rect.y0 + m_startRow*m_grid.m_delta;
            for ( int i = m_startRow; i < m_endRow; ++i, y += m_grid.m_delta )
            {
               double x = m_grid.m_rect.x0;
               for ( int j = 0; j < m_grid.m_G.Cols(); ++j, ++n, x += m_grid.m_delta )
               {
                  if ( n == m_N )
                  {
                     m_surface.Evaluate( m_Z.Begin(), m_X.Begin(), m_Y.Begin(), n );
                     for ( int i1 = i0, k1 = 0; i1 <= i && k1 < n; ++i1, j0 = 0 )
                        for ( int j1 = j0; j1 < m_grid.m_G.Cols() && k1 < n; ++j1, ++k1 )
                           m_grid.m_G[i1][j1] = m_Z[k1];
                     i0 = i;
                     j0 = j;
                     n = 0;
                  }

                  m_X[n] = x;
                  m_Y[n] = y;
               }

               UPDATE_THREAD_MONITOR( 1 )
            }

            if ( n > 0 )
            {
               m_surface.Evaluate( m_Z.Begin(), m_X.Begin(), m_Y.Begin(), n );
               for ( int i1 = i0, k1 = 0; i1 < m_endRow; ++i1, j0 = 0 )
                  for ( int j1 = j0; j1 < m_grid.m_G.Cols(); ++j1, ++k1 )
                     m_grid.m_G[i1][j1] = m_Z[k1];

               UPDATE_THREAD_MONITOR( 1 )
            }
         }
         else
         {
            double y = m_grid.m_rect.y0 + m_startRow*m_grid.m_delta;
            for ( int i = m_startRow; i < m_endRow; ++i, y += m_grid.m_delta )
            {
               double x = m_grid.m_rect.x0;
               for ( int j = 0; j < m_grid.m_G.Cols(); ++j, x += m_grid.m_delta )
                  m_grid.m_G[i][j] = m_surface( x, y );

               UPDATE_THREAD_MONITOR( 32 )
            }
         }
      }

   private:

      const AbstractImage::ThreadData& m_data;
            GridInterpolation& m_grid;
      const SI&                m_surface;
            int                m_startRow, m_endRow, m_N;
            DVector            m_X, m_Y, m_Z;
   };
};

// ----------------------------------------------------------------------------

/*!
 * \class PointGridInterpolation
 * \brief Discretized vector surface interpolation/approximation in two
 *        dimensions.
 *
 * This class performs the same tasks as a point surface interpolation device,
 * such as PointSurfaceSpline or PointShepardInterpolation, but allows for much
 * faster interpolation with negligible accuracy loss in most applications.
 *
 * Interpolation from discrete grids can be orders of magnitude faster than
 * direct evaluation of surface interpolation/approximation devices, depending
 * on the number of input data points.
 */
class PCL_CLASS PointGridInterpolation : public ParallelProcess
{
public:

   /*!
    * Default constructor. Yields an empty instance that cannot be used without
    * initialization.
    */
   PointGridInterpolation() = default;

   /*!
    * Copy constructor.
    */
   PointGridInterpolation( const PointGridInterpolation& ) = default;

   /*!
    * Move constructor.
    */
   PointGridInterpolation( PointGridInterpolation&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PointGridInterpolation& operator =( const PointGridInterpolation& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   PointGridInterpolation& operator =( PointGridInterpolation&& ) = default;

   /*!
    * Initializes a %PointGridInterpolation object with a point surface
    * interpolation/approximation.
    *
    * \param rect    Reference rectangle. Interpolation will be initialized
    *                within the boundaries of this rectangle at discrete
    *                \a delta coordinate intervals.
    *
    * \param delta   Grid distance for calculation of discrete function values.
    *                Must be &gt; 0.
    *
    * \param PS      Reference to a point surface interpolation/approximation
    *                object that will be used to evaluate function values at
    *                discrete coordinate intervals. This object must have been
    *                previously initialized and must be valid.
    *
    * \param verbose If true, this function will write information to the
    *                standard PixInsight console to provide some feedback to
    *                the user during the (potentially long) initialization
    *                process. If false, no feedback will be provided.
    *
    * The template parameter SI must implement the following member functions:
    *
    * 1. Function evaluation operator:
    *
    * DPoint PSI::operator ()( double x, double y ) const
    *
    * or an equivalent operator member function whose return value can be
    * statically casted to DPoint, with two by-value parameters that can be
    * statically casted from the double type. This function will be called
    * multiple times for the \a PS object to evaluate the approximated surface
    * at discrete grid coordinate pairs {x,y}, respectively on the X and Y
    * plane directions. The implementation of this member function must be
    * thread-safe if parallel processing has been enabled and is allowed for
    * this object.
    *
    * 2. Vector evaluation function:
    *
    * void PSI::Evaluate( double* ZX, double* ZY,\n
    *                    const double* X, const double* Y, size_type n ) const
    *
    * This function will be called multiple times for the \a PS object for fast
    * multithreaded surface evaluation at a set of points in 2-D space
    * specified as the \a X and \a Y contiguous sequences of \a n coordinates.
    * The function must store the corresponding function values in the \a ZX
    * and \a ZY arrays. The implementation of this member function must be
    * thread-safe if parallel processing has been enabled and is allowed for
    * this object.
    *
    * 3. Information function:
    *
    * bool PSI::HasFastVectorEvaluation() const
    *
    * This member function must return true if the \a PS object provides fast
    * surface evaluation for vectors of points with the Evaluate() function. If
    * this function returns false, the operator ()() described above will be
    * called exclusively from grid generation threads.
    *
    * If parallel processing is allowed, this function executes the
    * initialization process using multiple concurrent threads. See
    * EnableParallelProcessing() for additional information.
    */
   template <class R, class PSI>
   void Initialize( const R& rect, double delta, const PSI& PS, bool verbose = true )
   {
      PCL_PRECONDITION( rect.IsRect() )
      PCL_PRECONDITION( delta > 0 )

      m_rect = DRect( rect.Ordered() );
      double width = m_rect.Width();
      double height = m_rect.Height();
      if ( !m_rect.IsRect() || 1 + width == 1 || 1 + height == 1 )
         throw Error( "PointGridInterpolation::Initialize(): Empty interpolation space." );

      m_delta = Abs( delta );
      if ( 1 + m_delta == 1 )
         throw Error( "PointGridInterpolation::Initialize(): Zero or insignificant grid distance." );

      int rows = 1 + int( Ceil( height/m_delta ) );
      int cols = 1 + int( Ceil( width/m_delta ) );
      m_Gx = DMatrix( rows, cols );
      m_Gy = DMatrix( rows, cols );

      StatusMonitor monitor;
#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
      StandardStatus status;
      if ( verbose )
      {
         monitor.SetCallback( &status );
         monitor.Initialize( "Building surface interpolation grid", rows );
      }
#endif

      Array<size_type> L = Thread::OptimalThreadLoads( rows,
                                                       1/*overheadLimit*/,
                                                       m_parallel ? m_maxProcessors : 1 );
      AbstractImage::ThreadData data( monitor, rows );
      ReferenceArray<GridInitializationThread<PSI> > threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new GridInitializationThread<PSI>( data, *this, PS, n, n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      m_Ix.Initialize( m_Gx.Begin(), cols, rows );
      m_Iy.Initialize( m_Gy.Begin(), cols, rows );
   }

   /*!
    * Initializes a %PointGridInterpolation object with separate surface
    * interpolations/approximations for the X and Y directions.
    *
    * \param rect    Reference rectangle. Interpolation will be initialized
    *                within the boundaries of this rectangle at discrete
    *                \a delta coordinate intervals.
    *
    * \param delta   Grid distance for calculation of discrete function values.
    *                Must be &gt; 0.
    *
    * \param Sx      Reference to a surface interpolation/approximation object
    *                that will be used to evaluate function values at discrete
    *                coordinate intervals on the X axis. This object must have
    *                been previously initialized and must be valid.
    *
    * \param Sy      Reference to a surface interpolation/approximation object
    *                that will be used to evaluate function values at discrete
    *                coordinate intervals on the Y axis. This object must have
    *                been previously initialized and must be valid.
    *
    * \param verbose If true, this function will write information to the
    *                standard PixInsight console to provide some feedback to
    *                the user during the (potentially long) initialization
    *                process. If false, no feedback will be provided.
    *
    * The template parameter SI must implement the following member functions:
    *
    * 1. Function evaluation operator:
    *
    * double SI::operator ()( double x, double y ) const
    *
    * or an equivalent operator member function whose return value can be
    * statically casted to double, with two by-value parameters that can be
    * statically casted from the double type. This function will be called
    * multiple times for the \a Sx and \a Sy objects to evaluate the
    * approximated surface at discrete grid coordinate pairs {x,y},
    * respectively on the X and Y plane directions. The implementation of this
    * member function must be thread-safe if parallel processing has been
    * enabled and is allowed for this object.
    *
    * 2. Vector evaluation function:
    *
    * void SI::Evaluate( double* Z,\n
    *                    const double* X, const double* Y, size_type n ) const
    *
    * This function will be called multiple times for the \a Sx and \a Sy
    * objects for fast multithreaded surface evaluation at a set of points in
    * 2-D space specified as the \a X and \a Y contiguous sequences of \a n
    * coordinates. The function must store the corresponding function values in
    * the \a Z array. The implementation of this member function must be
    * thread-safe if parallel processing has been enabled and is allowed for
    * this object.
    *
    * 3. Information function:
    *
    * bool SI::HasFastVectorEvaluation() const
    *
    * This member function must return true if each of the \a Sx and \a Sy
    * objects provides fast surface evaluation for vectors of points with the
    * Evaluate() function. If this function returns false for one or both
    * objects, the operator ()() described above will be called exclusively
    * from grid generation threads.
    *
    * If parallel processing is allowed, this function executes the
    * initialization process using multiple concurrent threads. See
    * EnableParallelProcessing() for additional information.
    */
   template <class R, class SI>
   void Initialize( const R& rect, double delta, const SI& Sx, const SI& Sy, bool verbose = true )
   {
      PCL_PRECONDITION( rect.IsRect() )
      PCL_PRECONDITION( delta > 0 )

      m_rect = DRect( rect.Ordered() );
      double width = m_rect.Width();
      double height = m_rect.Height();
      if ( !m_rect.IsRect() || 1 + width == 1 || 1 + height == 1 )
         throw Error( "PointGridInterpolation::Initialize(): Empty interpolation space." );

      m_delta = Abs( delta );
      if ( 1 + m_delta == 1 )
         throw Error( "PointGridInterpolation::Initialize(): Zero or insignificant grid distance." );

      int rows = 1 + int( Ceil( height/m_delta ) );
      int cols = 1 + int( Ceil( width/m_delta ) );
      m_Gx = DMatrix( rows, cols );
      m_Gy = DMatrix( rows, cols );

      StatusMonitor monitor;
#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
      StandardStatus status;
      if ( verbose )
      {
         monitor.SetCallback( &status );
         monitor.Initialize( "Building surface interpolation grid", rows );
      }
#endif

      Array<size_type> L = Thread::OptimalThreadLoads( rows,
                                                       1/*overheadLimit*/,
                                                       m_parallel ? m_maxProcessors : 1 );
      AbstractImage::ThreadData data( monitor, rows );
      ReferenceArray<GridInitializationXYThread<SI> > threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new GridInitializationXYThread<SI>( data, *this, Sx, Sy, n, n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      m_Ix.Initialize( m_Gx.Begin(), cols, rows );
      m_Iy.Initialize( m_Gy.Begin(), cols, rows );
   }

   /*!
    * Initializes this %PointGridInterpolation object with prescribed
    * interpolation matrices.
    *
    * \param rect    Reference rectangle. Interpolation will be initialized
    *                within the boundaries of this rectangle at discrete
    *                \a delta coordinate intervals.
    *
    * \param delta   Grid distance for calculation of discrete function values.
    *                Must be > 0.
    *
    * \param Gx      interpolation matrix in the X direction.
    *
    * \param Gy      Interpolation matrix in the Y direction.
    *
    * Both \a Gx and \a Gy matrices must have \a n rows and \a m columns, which
    * are given by:
    *
    * n = 1 + Ceil( rect.Height()/delta )\n
    * m = 1 + Ceil( rect.Width()/delta )
    *
    * If one or both matrices have different dimensions, this function will
    * throw an Error exception.
    *
    * Matrix elements must be function values computed at discrete \a delta
    * intervals within \a rect boundaries. For a given matrix row \a r and
    * matrix column \a c, the corresponding matrix element must be a function
    * value computed at coordinates {\a x,\a y} given by:
    *
    * x = rect.x0 + c*delta\n
    * y = rect.y0 + r*delta
    */
   template <class R>
   void Initialize( const R& rect, int delta, const DMatrix& Gx, const DMatrix& Gy )
   {
      PCL_PRECONDITION( rect.IsRect() )
      PCL_PRECONDITION( delta > 0 )

      m_rect = DRect( rect.Ordered() );
      double width = m_rect.Width();
      double height = m_rect.Height();
      if ( !m_rect.IsRect() || 1 + width == 1 || 1 + height == 1 )
         throw Error( "PointGridInterpolation::Initialize(): Empty interpolation region." );

      m_delta = Abs( delta );
      if ( 1 + m_delta == 1 )
         throw Error( "PointGridInterpolation::Initialize(): Zero or insignificant grid distance." );

      int rows = 1 + int( Ceil( height/m_delta ) );
      int cols = 1 + int( Ceil( width/m_delta ) );
      if ( Gx.Rows() != rows || Gx.Cols() != cols || Gy.Rows() != rows || Gy.Cols() != cols )
         throw Error( "PointGridInterpolation::Initialize(): Invalid matrix dimensions." );

      m_Gx = Gx;
      m_Gy = Gy;
      m_Ix.Initialize( m_Gx.Begin(), cols, rows );
      m_Iy.Initialize( m_Gy.Begin(), cols, rows );
   }

   /*!
    *
    */
   template <class PSI>
   void ApplyLocalModel( const PSI& PS,
                         const String& message = "Applying local interpolation model",
                         bool verbose = true )
   {
      PCL_PRECONDITION( IsValid() )
      if ( !IsValid() )
         throw Error( "PointGridInterpolation::ApplyLocalModel(): Uninitialized interpolation." );

      StatusMonitor monitor;
#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
      StandardStatus status;
      if ( verbose )
      {
         monitor.SetCallback( &status );
         monitor.Initialize( message, m_Gx.Rows() );
      }
#endif

      Array<size_type> L = Thread::OptimalThreadLoads( m_Gx.Rows(),
                                                       1/*overheadLimit*/,
                                                       m_parallel ? m_maxProcessors : 1 );
      AbstractImage::ThreadData data( monitor, m_Gx.Rows() );
      ReferenceArray<LocalModelThread<PSI> > threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new LocalModelThread<PSI>( data, *this, PS, n, n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();
   }

   /*!
    *
    */
   template <class SI>
   void ApplyLocalModel( const SI& Sx, const SI& Sy,
                         const String& message = "Applying local interpolation model",
                         bool verbose = true )
   {
      PCL_PRECONDITION( IsValid() )
      if ( !IsValid() )
         throw Error( "PointGridInterpolation::ApplyLocalModel(): Uninitialized interpolation." );

      StatusMonitor monitor;
#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
      StandardStatus status;
      if ( verbose )
      {
         monitor.SetCallback( &status );
         monitor.Initialize( message, m_Gx.Rows() );
      }
#endif

      Array<size_type> L = Thread::OptimalThreadLoads( m_Gx.Rows(),
                                                       1/*overheadLimit*/,
                                                       m_parallel ? m_maxProcessors : 1 );
      AbstractImage::ThreadData data( monitor, m_Gx.Rows() );
      ReferenceArray<LocalModelThread2<SI> > threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new LocalModelThread2<SI>( data, *this, Sx, Sy, n, n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();
   }

   /*!
    * Returns true iff this is a valid, initialized object ready for
    * interpolation.
    */
   bool IsValid() const
   {
      return !m_Gx.IsEmpty() && !m_Gy.IsEmpty();
   }

   /*!
    * Deallocates internal structures, yielding an empty object that cannot be
    * used before a new call to Initialize().
    */
   void Clear()
   {
      m_Ix.Clear();
      m_Iy.Clear();
      m_Gx.Clear();
      m_Gy.Clear();
   }

   /*!
    * Returns the current interpolation reference rectangle. See Initialize()
    * for more information.
    *
    * The returned rectangle is ordered (see pcl::IsOrderedRect()).
    */
   const DRect& ReferenceRect() const
   {
      return m_rect;
   }

   /*!
    * Returns the current grid distance for calculation of discrete function
    * values. See Initialize() for more information.
    */
   double Delta() const
   {
      return m_delta;
   }

   /*!
    * Returns a reference to the discrete matrix used for interpolation of
    * function values in the X direction.
    *
    * If this object has not been initialized, this member function returns a
    * reference to an empty matrix.
    */
   const DMatrix& XInterpolationMatrix() const
   {
      return m_Gx;
   }

   /*!
    * Returns a reference to the discrete matrix used for interpolation of
    * function values in the Y direction.
    *
    * If this object has not been initialized, this member function returns a
    * reference to an empty matrix.
    */
   const DMatrix& YInterpolationMatrix() const
   {
      return m_Gy;
   }

   /*!
    * Returns an interpolated point at the specified coordinates.
    */
   template <typename T>
   DPoint operator ()( T x, T y ) const
   {
      PCL_PRECONDITION( IsValid() )
      double fx = (double( x ) - m_rect.x0)/m_delta;
      double fy = (double( y ) - m_rect.y0)/m_delta;
      return DPoint( m_Ix( fx, fy ), m_Iy( fx, fy ) );
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

   /*!
    * N.B.: ** Critical requirement ** We need a smooth interpolation function
    * without negative lobes here to prevent small-scale oscillations. Other
    * options are BilinearInterpolation and CubicBSplineFilter.
    */
   using grid_interpolation = BicubicBSplineInterpolation<double>;

   DRect              m_rect;
   double             m_delta;
   DMatrix            m_Gx, m_Gy;
   grid_interpolation m_Ix, m_Iy;

   template <class PSI>
   class GridInitializationThread : public Thread
   {
   public:

      GridInitializationThread( const AbstractImage::ThreadData& data,
                                PointGridInterpolation& grid, const PSI& surface, int startRow, int endRow )
         : m_data( data )
         , m_grid( grid )
         , m_surface( surface )
         , m_startRow( startRow )
         , m_endRow( endRow )
      {
         if ( m_surface.HasFastVectorEvaluation() )
         {
            m_N = int( Min( size_type( 4096 ), size_type( m_endRow - m_startRow )*m_grid.m_Gx.Cols() ) );
            m_X = DVector( m_N );
            m_Y = DVector( m_N );
            m_ZX = DVector( m_N );
            m_ZY = DVector( m_N );
         }
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         if ( m_surface.HasFastVectorEvaluation() )
         {
            int i0 = m_startRow, j0 = 0, n = 0;
            double y = m_grid.m_rect.y0 + m_startRow*m_grid.m_delta;
            for ( int i = m_startRow; i < m_endRow; ++i, y += m_grid.m_delta )
            {
               double x = m_grid.m_rect.x0;
               for ( int j = 0; j < m_grid.m_Gx.Cols(); ++j, ++n, x += m_grid.m_delta )
               {
                  if ( n == m_N )
                  {
                     m_surface.Evaluate( m_ZX.Begin(), m_ZY.Begin(), m_X.Begin(), m_Y.Begin(), n );
                     for ( int i1 = i0, k1 = 0; i1 <= i && k1 < n; ++i1, j0 = 0 )
                        for ( int j1 = j0; j1 < m_grid.m_Gx.Cols() && k1 < n; ++j1, ++k1 )
                        {
                           m_grid.m_Gx[i1][j1] = m_ZX[k1];
                           m_grid.m_Gy[i1][j1] = m_ZY[k1];
                        }
                     i0 = i;
                     j0 = j;
                     n = 0;
                  }

                  m_X[n] = x;
                  m_Y[n] = y;
               }

               UPDATE_THREAD_MONITOR( 1 )
            }

            if ( n > 0 )
            {
               m_surface.Evaluate( m_ZX.Begin(), m_ZY.Begin(), m_X.Begin(), m_Y.Begin(), n );
               for ( int i1 = i0, k1 = 0; i1 < m_endRow; ++i1, j0 = 0 )
                  for ( int j1 = j0; j1 < m_grid.m_Gx.Cols(); ++j1, ++k1 )
                  {
                     m_grid.m_Gx[i1][j1] = m_ZX[k1];
                     m_grid.m_Gy[i1][j1] = m_ZY[k1];
                  }

               UPDATE_THREAD_MONITOR( 1 )
            }
         }
         else
         {
            double y = m_grid.m_rect.y0 + m_startRow*m_grid.m_delta;
            for ( int i = m_startRow; i < m_endRow; ++i, y += m_grid.m_delta )
            {
               double x = m_grid.m_rect.x0;
               for ( int j = 0; j < m_grid.m_Gx.Cols(); ++j, x += m_grid.m_delta )
               {
                  DPoint p = m_surface( x, y );
                  m_grid.m_Gx[i][j] = p.x;
                  m_grid.m_Gy[i][j] = p.y;
               }

               UPDATE_THREAD_MONITOR( 32 )
            }
         }
      }

   private:

      const AbstractImage::ThreadData& m_data;
            PointGridInterpolation&    m_grid;
      const PSI&                       m_surface;
            int                        m_startRow, m_endRow, m_N;
            DVector                    m_X, m_Y, m_ZX, m_ZY;
   };

   template <class SI>
   class GridInitializationXYThread : public Thread
   {
   public:

      GridInitializationXYThread( const AbstractImage::ThreadData& data,
                                  PointGridInterpolation& grid,
                                  const SI& surfaceX, const SI& surfaceY, int startRow, int endRow )
         : m_data( data )
         , m_grid( grid )
         , m_surfaceX( surfaceX )
         , m_surfaceY( surfaceY )
         , m_startRow( startRow )
         , m_endRow( endRow )
      {
         if ( m_surfaceX.HasFastVectorEvaluation() && m_surfaceY.HasFastVectorEvaluation() )
         {
            m_N = int( Min( size_type( 4096 ), size_type( m_endRow - m_startRow )*m_grid.m_Gx.Cols() ) );
            m_X = DVector( m_N );
            m_Y = DVector( m_N );
            m_Z = DVector( m_N );
         }
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         if ( m_surfaceX.HasFastVectorEvaluation() && m_surfaceY.HasFastVectorEvaluation() )
         {
            int i0 = m_startRow, j0 = 0, n = 0;
            double y = m_grid.m_rect.y0 + m_startRow*m_grid.m_delta;
            for ( int i = m_startRow; i < m_endRow; ++i, y += m_grid.m_delta )
            {
               double x = m_grid.m_rect.x0;
               for ( int j = 0; j < m_grid.m_Gx.Cols(); ++j, ++n, x += m_grid.m_delta )
               {
                  if ( n == m_N )
                  {
                     m_surfaceX.Evaluate( m_Z.Begin(), m_X.Begin(), m_Y.Begin(), n );
                     for ( int i1 = i0, jj0 = j0, k1 = 0; i1 <= i && k1 < n; ++i1, jj0 = 0 )
                        for ( int j1 = jj0; j1 < m_grid.m_Gx.Cols() && k1 < n; ++j1, ++k1 )
                           m_grid.m_Gx[i1][j1] = m_Z[k1];
                     m_surfaceY.Evaluate( m_Z.Begin(), m_X.Begin(), m_Y.Begin(), n );
                     for ( int i1 = i0, jj0 = j0, k1 = 0; i1 <= i && k1 < n; ++i1, jj0 = 0 )
                        for ( int j1 = jj0; j1 < m_grid.m_Gx.Cols() && k1 < n; ++j1, ++k1 )
                           m_grid.m_Gy[i1][j1] = m_Z[k1];
                     i0 = i;
                     j0 = j;
                     n = 0;
                  }

                  m_X[n] = x;
                  m_Y[n] = y;
               }

               UPDATE_THREAD_MONITOR( 1 )
            }

            if ( n > 0 )
            {
               m_surfaceX.Evaluate( m_Z.Begin(), m_X.Begin(), m_Y.Begin(), n );
               for ( int i1 = i0, jj0 = j0, k1 = 0; i1 < m_endRow; ++i1, jj0 = 0 )
                  for ( int j1 = jj0; j1 < m_grid.m_Gx.Cols(); ++j1, ++k1 )
                     m_grid.m_Gx[i1][j1] = m_Z[k1];
               m_surfaceY.Evaluate( m_Z.Begin(), m_X.Begin(), m_Y.Begin(), n );
               for ( int i1 = i0, jj0 = j0, k1 = 0; i1 < m_endRow; ++i1, jj0 = 0 )
                  for ( int j1 = jj0; j1 < m_grid.m_Gx.Cols(); ++j1, ++k1 )
                     m_grid.m_Gy[i1][j1] = m_Z[k1];

               UPDATE_THREAD_MONITOR( 1 )
            }
         }
         else
         {
            double y = m_grid.m_rect.y0 + m_startRow*m_grid.m_delta;
            for ( int i = m_startRow; i < m_endRow; ++i, y += m_grid.m_delta )
            {
               double x = m_grid.m_rect.x0;
               for ( int j = 0; j < m_grid.m_Gx.Cols(); ++j, x += m_grid.m_delta )
               {
                  m_grid.m_Gx[i][j] = m_surfaceX( x, y );
                  m_grid.m_Gy[i][j] = m_surfaceY( x, y );
               }

               UPDATE_THREAD_MONITOR( 32 )
            }
         }
      }

   private:

      const AbstractImage::ThreadData& m_data;
            PointGridInterpolation&    m_grid;
      const SI&                        m_surfaceX;
      const SI&                        m_surfaceY;
            int                        m_startRow, m_endRow, m_N;
            DVector                    m_X, m_Y, m_Z;
   };

   template <class PSI>
   class LocalModelThread : public Thread
   {
   public:

      LocalModelThread( const AbstractImage::ThreadData& data,
                        PointGridInterpolation& grid, const PSI& model, int startRow, int endRow )
         : m_data( data )
         , m_grid( grid )
         , m_model( model )
         , m_startRow( startRow )
         , m_endRow( endRow )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         double y = m_grid.m_rect.y0 + m_startRow*m_grid.m_delta;
         for ( int i = m_startRow; i < m_endRow; ++i, y += m_grid.m_delta )
         {
            double x = m_grid.m_rect.x0;
            for ( int j = 0; j < m_grid.m_Gx.Cols(); ++j, x += m_grid.m_delta )
            {
               DPoint d = m_model( x, y );
               m_grid.m_Gx[i][j] += d.x;
               m_grid.m_Gy[i][j] += d.y;
            }

            UPDATE_THREAD_MONITOR( 32 )
         }
      }

   private:

      const AbstractImage::ThreadData& m_data;
            PointGridInterpolation&    m_grid;
      const PSI&                       m_model;
            int                        m_startRow, m_endRow;
   };

   template <class SI>
   class LocalModelThread2 : public Thread
   {
   public:

      LocalModelThread2( const AbstractImage::ThreadData& data,
                         PointGridInterpolation& grid, const SI& modelX, const SI& modelY, int startRow, int endRow )
         : m_data( data )
         , m_grid( grid )
         , m_modelX( modelX )
         , m_modelY( modelY )
         , m_startRow( startRow )
         , m_endRow( endRow )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         double y = m_grid.m_rect.y0 + m_startRow*m_grid.m_delta;
         for ( int i = m_startRow; i < m_endRow; ++i, y += m_grid.m_delta )
         {
            double x = m_grid.m_rect.x0;
            for ( int j = 0; j < m_grid.m_Gx.Cols(); ++j, x += m_grid.m_delta )
            {
               m_grid.m_Gx[i][j] += m_modelX( x, y );
               m_grid.m_Gy[i][j] += m_modelY( x, y );
            }

            UPDATE_THREAD_MONITOR( 32 )
         }
      }

   private:

      const AbstractImage::ThreadData& m_data;
            PointGridInterpolation&    m_grid;
      const SI&                        m_modelX, m_modelY;
            int                        m_startRow, m_endRow;
   };

   friend class SplineWorldTransformation;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_GridInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/GridInterpolation.h - Released 2024-12-28T16:53:48Z
