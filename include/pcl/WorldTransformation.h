//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/WorldTransformation.h - Released 2024-12-27T18:16:00Z
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

#ifndef __PCL_WorldTransformation_h
#define __PCL_WorldTransformation_h

/// \file pcl/WorldTransformation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/GridInterpolation.h>
#include <pcl/LinearTransformation.h>
#include <pcl/SurfaceSpline.h>
#include <pcl/WCSKeywords.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

#define __PCL_WCS_SPLINE_VERSION_CURRENT                     "2.0"
#define __PCL_WCS_SPLINE_VERSION_OLD                         "1.3"
#define __PCL_WCS_MIN_SPLINE_MAX_POINTS                      1000
#define __PCL_WCS_MAX_SPLINE_MAX_DENSE_POINTS                2100
#define __PCL_WCS_MAX_SPLINE_MAX_DDM_POINTS                  25000
#define __PCL_WCS_DEFAULT_SPLINE_MAX_DENSE_POINTS            2100
#define __PCL_WCS_DEFAULT_SPLINE_MAX_DDM_POINTS              4000
#define __PCL_WCS_DEFAULT_SPLINE_MAX_POINTS                  4000
#define __PCL_WCS_DEFAULT_SPLINE_RBF                         RadialBasisFunction::DDMThinPlateSpline
#define __PCL_WCS_DEFAULT_SPLINE_ORDER                       2
#define __PCL_WCS_DEFAULT_SPLINE_SMOOTHNESS                  0.005F
#define __PCL_WCS_DEFAULT_SURFACE_SIMPLIFIER_ENABLED         true
#define __PCL_WCS_DEFAULT_SURFACE_SIMPLIFIER_REJECT_FRACTION 0.10F

// ----------------------------------------------------------------------------

/*!
 * \class WorldTransformation
 * \brief Abstract base class of world coordinate transformations
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS WorldTransformation
{
public:

   /*!
    * Default constructor.
    */
   WorldTransformation() = default;

   /*!
    * Copy constructor.
    */
   WorldTransformation( const WorldTransformation& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~WorldTransformation()
   {
   }

   /*!
    * Returns true iff this transformation is empty (uninitialized, invalid).
    */
   virtual bool IsEmpty() const
   {
      return false;
   }

   /*!
    * Returns a dynamically allocated copy of this object.
    */
   virtual WorldTransformation* Clone() const = 0;

   /*!
    * Transforms from native spherical coordinates to image coordinates.
    *
    * The point \a p contains native spherical coordinates: \a p.x is the
    * native longitude and \a p.y is the native latitude, both expressed in
    * degrees. Returns image coordinates in pixels corresponding to \a p.
    */
   virtual DPoint Direct( const DPoint& p ) const = 0;

   /*!
    * Transforms from image coordinates to native spherical coordinates.
    *
    * The specified point \a p contains image coordinates in pixels. Returns a
    * point \a q where \a q.x is the native longitude and \a q.y is the native
    * latitude, both expressed in degrees, corresponding to \a p.
    */
   virtual DPoint Inverse( const DPoint& p ) const = 0;

   /*!
    * Returns an approximate linear transformation from Image to World
    * coordinates.
    */
   virtual const LinearTransformation& ApproximateLinearTransform() const = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \class LinearWorldTransformation
 * \brief Linear world coordinate transformation
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS LinearWorldTransformation : public WorldTransformation
{
public:

   /*!
    * Constructor from a linear transformation. The specified transformation
    * \a transIW must transform from image coordinates to native spherical
    * coordinates.
    */
   LinearWorldTransformation( const LinearTransformation& transIW )
      : m_transWI( transIW.Inverse() )
      , m_transIW( transIW )
   {
   }

   /*!
    * Copy constructor.
    */
   LinearWorldTransformation( const LinearWorldTransformation& ) = default;

   /*!
    * Move constructor.
    */
   LinearWorldTransformation( LinearWorldTransformation&& ) = default;

   /*!
    * Destroys a %LinearWorldTransformation object.
    */
   ~LinearWorldTransformation() override
   {
   }

   /*!
    * Returns false, since a linear WCS transformation cannot be empty.
    */
   bool IsEmpty() const override
   {
      return false;
   }

   /*!
    */
   WorldTransformation* Clone() const override
   {
      return new LinearWorldTransformation( *this );
   }

   /*!
    */
   DPoint Direct( const DPoint& p ) const override
   {
      return m_transWI.Transform( p );
   }

   /*!
    */
   DPoint Inverse( const DPoint& p ) const override
   {
      return m_transIW.Transform( p );
   }

   /*!
    * Returns a reference to the internal linear transformation from image to
    * native spherical coordinates.
    */
   const LinearTransformation& ApproximateLinearTransform() const override
   {
      return m_transIW;
   }

private:

   LinearTransformation m_transWI; // world -> image
   LinearTransformation m_transIW; // image -> world
};

// ----------------------------------------------------------------------------

class PCL_CLASS XMLElement;

/*!
 * \class SplineWorldTransformation
 * \brief Surface spline world coordinate transformation
 *
 * %SplineWorldTransformation implements a world coordinate transform based on
 * 2-D interpolating/approximating surface splines (also known as <em>thin
 * plate splines</em>). These mathematical modeling devices allow for accurate
 * representations of coordinate systems subject to arbitrary local
 * distortions, which are impossible to achieve with linear transformations.
 *
 * The underlying implementation of this class uses the SurfaceSpline,
 * PointSurfaceSpline, SurfaceSimplifier and PointGridInterpolation classes.
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS SplineWorldTransformation : public WorldTransformation
{
public:

   using rbf_type = PointSurfaceSpline::rbf_type;

   /*!
    * Constructs a 2-D spline based world coordinate transformation.
    *
    * \param controlPointsW   Array of world control points. Each point in this
    *                         array must contain spherical coordinates in the
    *                         native world coordinate system of the astrometric
    *                         solution, expressed in degrees. For a given
    *                         point p in this array, p.x is the native
    *                         longitude coordinate and p.y is the latitude.
    *
    * \param controlPointsI   Array of image control points. Each point in this
    *                         array must contain the coordinates on the X and Y
    *                         axes of the image plane corresponding to the same
    *                         point in the \a controlPointsW array. In other
    *                         words, there must be a one-to-one correspondence
    *                         between world and image control points.
    *
    * \param rbf              The type of radial basis function (RBF) that will
    *                         be used for the generation of interpolation or
    *                         approximation surface splines. Currently the
    *                         following RBFs are supported for spline-based
    *                         astrometric solutions:\n
    *                         \n
    *                         RadialBasisFunction::ThinPlateSpline \n
    *                         RadialBasisFunction::VariableOrder \n
    *                         RadialBasisFunction::DDMVariableOrder \n
    *                         RadialBasisFunction::DDMThinPlateSpline \n
    *                         RadialBasisFunction::DDMMultiquadric\n
    *                         \n
    *                         The default value is DDMThinPlateSpline when a
    *                         running core PixInsight application is available;
    *                         ThinPlateSpline otherwise.
    *
    * \param smoothness       Spline regularization factor. When this parameter
    *                         is greater than zero, approximating splines will
    *                         be generated instead of interpolating splines.
    *                         The higher this value, the closer the 2-D
    *                         approximating surface will be to the reference
    *                         plane of the image. Approximating surface splines
    *                         are more robust to noise in the sets of control
    *                         points and hence recommended in virtually all
    *                         cases. The value of this parameter should be
    *                         small if \a enableSimplifier is true, since the
    *                         surface simplification algorithm already performs
    *                         robust rejection of outliers. The default value
    *                         is 0.005.
    *
    * \param order            Derivative order of continuity. The default value
    *                         is 2. Higher orders, such as 3 or 4, can improve
    *                         adaptability to model complex field distortions.
    *                         Orders above 4 can lead to ill-conditioned linear
    *                         systems, causing errors while building the
    *                         surface splines.
    *
    * \param enableSimplifier If true, a surface simplification algorithm will
    *                         be applied to the lists of control points for
    *                         surface spline generation. The use of surface
    *                         simplification greatly improves efficiency of
    *                         surface splines by removing all redundant points
    *                         and keeping only the control points required to
    *                         define the coordinate transformations accurately.
    *                         In addition, the applied surface simplification
    *                         algorithm implements robust PCA fitting and
    *                         outlier rejection techniques that improve the
    *                         generated interpolation devices in terms of
    *                         resilience to noise and invalid data in the
    *                         underlying astrometric solution. This option is
    *                         enabled by default. When this parameter is true,
    *                         the \a weights parameter is ignored.
    *
    * \param simplifierRejectFraction  Fraction of rejected control points for
    *                         simplification of surface subregions. The default
    *                         value is 0.10.
    *
    * \param weights          When the \a smoothness parameter is greater than
    *                         zero and this vector is not empty, it must define
    *                         a positive weight greater than zero for each
    *                         point defined by the \a controlPointsW and
    *                         \a controlPointsI arrays. If \a smoothness is
    *                         zero or negative, this parameter will be ignored.
    *                         See the PointSurfaceSpline::Initialize() member
    *                         function for detailed information on 2-D spline
    *                         node weights. If \a enableSimplifier is true,
    *                         control point weights are not used and hence the
    *                         value of this parameter will be ignored.
    *
    * Thanks to the implemented surface simplification and DDM-based RBF
    * interpolation algorithms (the latter only when a running PixInsight core
    * application is available), we can work with large sets of control points
    * to generate astrometric solutions able to model strong and complex
    * arbitrary field distortions accurately and efficiently. Surface
    * simplification concentrates control points where they are necessary to
    * represent distortions, leaving undistorted image regions covered by a
    * minimal grid. DDM RBF interpolation/approximation allows us to use very
    * large sets of control points thanks to its O(n^2) time complexity, in
    * contrast to standard implementations based on the solution of dense
    * linear systems, requiring O(n^3) time.
    *
    * Newly constructed instances are guaranteed to be valid (in the structural
    * and numerical senses; note that this does not necessarily imply that the
    * underlying astrometric solution is valid). In the event of invalid input
    * data or spline initialization problems, this constructor will throw an
    * Error exception.
    *
    * After surface spline initialization, an approximate linear transformation
    * will also be calculated automatically.
    */
   SplineWorldTransformation( const Array<DPoint>& controlPointsW,
                              const Array<DPoint>& controlPointsI,
                              rbf_type rbf = __PCL_WCS_DEFAULT_SPLINE_RBF,
                              float smoothness = __PCL_WCS_DEFAULT_SPLINE_SMOOTHNESS,
                              int order = __PCL_WCS_DEFAULT_SPLINE_ORDER,
                              bool enableSimplifier = __PCL_WCS_DEFAULT_SURFACE_SIMPLIFIER_ENABLED,
                              float simplifierRejectFraction = __PCL_WCS_DEFAULT_SURFACE_SIMPLIFIER_REJECT_FRACTION,
                              const FVector& weights = FVector() )
      : m_controlPointsW( controlPointsW )
      , m_controlPointsI( controlPointsI )
      , m_rbf( rbf )
      , m_order( order )
      , m_smoothness( smoothness )
      , m_useSimplifiers( enableSimplifier )
      , m_simplifierRejectFraction( simplifierRejectFraction )
      , m_weights( weights )
   {
      EnsureValidRBFParameters();
      InitializeSplines();
      CalculateLinearApproximation();
   }

   /*!
    * Constructs a %SplineWorldTransformation object from a list of XISF image
    * \a properties and an optional linear approximation \a linearIW.
    *
    * This constructor will acquire structural parameters and data from XISF
    * properties with the PCL:AstrometricSolution:SplineWorldTransformation:
    * identifier prefix.
    *
    * If a valid linear transformation is specified, an existing
    * PCL:AstrometricSolution:SplineWorldTransformation:LinearApproximation
    * property will be ignored and the specified linear transformation will be
    * used instead.
    *
    * If the \a regenerate parameter is true, the internal surface splines and
    * other working structures will be recalculated, even if they can be
    * reconstructed from existing properties, and no grid interpolations will
    * be loaded or calculated.
    */
   SplineWorldTransformation( const PropertyArray& properties,
                              const LinearTransformation& linearIW = LinearTransformation::Null(), bool regenerate = false );

   /*!
    * Constructs a %SplineWorldTransformation instance by deserializing the
    * specified raw \a data, with an optional approximate linear transformation
    * \a linearIW.
    *
    * If no valid linear transformation is specified, an approximate one will
    * be calculated automatically.
    *
    * \deprecated This constructor is only provided for compatibility with old
    * modules where spline world transformations were serialized as single
    * ByteArray objects. It will be removed in a future version of PCL.
    */
   SplineWorldTransformation( const ByteArray& data, const LinearTransformation& linearIW = LinearTransformation::Null() )
   {
      Deserialize( data );
      EnsureValidRBFParameters();
      InitializeSplines();
      if ( linearIW.IsSingularMatrix() )
         CalculateLinearApproximation();
      else
         m_linearIW = linearIW;
   }

   /*!
    * Copy constructor.
    */
   SplineWorldTransformation( const SplineWorldTransformation& ) = default;

   /*!
    * Move constructor.
    */
   SplineWorldTransformation( SplineWorldTransformation&& ) = default;

   /*!
    * Destroys a %SplineWorldTransformation object.
    */
   ~SplineWorldTransformation() override
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SplineWorldTransformation& operator =( const SplineWorldTransformation& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   SplineWorldTransformation& operator =( SplineWorldTransformation&& ) = default;

   /*!
    * Returns true iff this object has no working data.
    *
    * With the restrictions imposed by class constructors, this can only happen
    * if this object is an xvalue after move construction or assignment.
    */
   bool IsEmpty() const override
   {
      return m_controlPointsW.IsEmpty() || m_controlPointsI.IsEmpty();
   }

   /*!
    * Returns a dynamically allocated copy of this object.
    */
   WorldTransformation* Clone() const override
   {
      return new SplineWorldTransformation( *this );
   }

   /*!
    * Transforms from native spherical coordinates to image coordinates.
    *
    * The point \a pW contains native spherical coordinates: \a pW.x is the
    * native longitude and \a pW.y is the native latitude, both expressed in
    * degrees. Returns image coordinates in pixels corresponding to \a pW.
    *
    * If grid interpolations have been initialized for this object, and the
    * specified point \a pW is included in the reference rectangle used for
    * grid initialization, they will be used transparently by this member
    * function. Otherwise, the surface splines will be evaluated directly,
    * which can be much slower, depending on the number of control points
    * defined by the astrometric solution.
    *
    * \sa Inverse(), InitializeGridInterpolations()
    */
   DPoint Direct( const DPoint& pW ) const override
   {
      if ( m_gridWI.IsValid() )
         if ( m_gridWI.ReferenceRect().IncludesFast( pW ) )
            return m_gridWI( pW );
      return m_splineWI( pW );
   }

   /*!
    * Transforms from image coordinates to native spherical coordinates.
    *
    * The specified point \a pI contains image coordinates in pixels. Returns a
    * point \a pW where \a pW.x is the native longitude and \a pW.y is the
    * native latitude, both expressed in degrees, corresponding to \a pI.
    *
    * If grid interpolations have been initialized for this object, and the
    * specified point \a pI is included in the reference rectangle used for
    * grid initialization, they will be used transparently by this member
    * function. Otherwise, the surface splines will be evaluated directly,
    * which can be much slower, depending on the number of control points
    * defined by the astrometric solution.
    *
    * \sa Direct(), InitializeGridInterpolations()
    */
   DPoint Inverse( const DPoint& pI ) const override
   {
      if ( m_gridIW.IsValid() )
         if ( m_gridIW.ReferenceRect().IncludesFast( pI ) )
            return m_gridIW( pI );
      return m_splineIW( pI );
   }

   /*!
    * Returns an approximate linear transformation from image to native
    * spherical coordinates, computed from the internal point surface splines.
    */
   const LinearTransformation& ApproximateLinearTransform() const override
   {
      return m_linearIW;
   }

   /*!
    * Returns the serialization version of this %SplineWorldTransformation
    * object. Currently this member function returns "1.3".
    */
   const IsoString& Version() const
   {
      return m_version;
   }

   /*!
    * Initializes the internal grid interpolation devices for the specified
    * reference rectangular region \a rectI and grid distance \a deltaI in
    * pixels.
    *
    * A grid distance of 16 pixels is applied by default. This is normally more
    * than sufficient to yield accurate coordinate readouts, even for strongly
    * distorted images. The accepted range is from 1 to 64 pixels.
    *
    * See GridInterpolation::Initialize() for detailed information on spline
    * grid interpolation and its working parameters.
    *
    * \note This member function will show no console messages. If some
    * feedback must be provided to the user during the potentially long
    * operation, it must be given before calling this function.
    */
   void InitializeGridInterpolations( const Rect& rectI, int deltaI = 16 )
   {
      PCL_PRECONDITION( deltaI >= 1 && deltaI <= 64 )
      deltaI = Range( deltaI, 1, 64 );

      /*
       * Compute reference region and grid distance in native spherical
       * coordinates.
       */
      DRect rectW( std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
                   std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest() );
      for ( int x = 0; x < rectI.Width(); ++x )
         for ( int p = 0; p < 2; ++p )
         {
            DPoint pW = m_splineIW( DPoint( double( x ), p*(rectI.Height() - 1) ) );
            if ( pW.x < rectW.x0 )
               rectW.x0 = pW.x;
            if ( pW.x > rectW.x1 )
               rectW.x1 = pW.x;
            if ( pW.y < rectW.y0 )
               rectW.y0 = pW.y;
            if ( pW.y > rectW.y1 )
               rectW.y1 = pW.y;
         }
      for ( int y = 0; y < rectI.Height(); ++y )
         for ( int p = 0; p < 2; ++p )
         {
            DPoint pW = m_splineIW( DPoint( p*(rectI.Width() - 1), double( y ) ) );
            if ( pW.x < rectW.x0 )
               rectW.x0 = pW.x;
            if ( pW.x > rectW.x1 )
               rectW.x1 = pW.x;
            if ( pW.y < rectW.y0 )
               rectW.y0 = pW.y;
            if ( pW.y > rectW.y1 )
               rectW.y1 = pW.y;
         }
      double deltaW = m_splineIW( DPoint( -deltaI/2, -deltaI/2 ) ).DistanceTo(
                           m_splineIW( DPoint( +deltaI/2, +deltaI/2 ) ) )/1.4142;

      m_gridWI.Initialize( rectW, deltaW, m_splineWI, false/*verbose*/ );
      m_gridIW.Initialize( rectI, deltaI, m_splineIW, false/*verbose*/ );
   }

   /*!
    * Returns true if the internal grid interpolation devices have been
    * initialized. See InitializeGridInterpolations() for information on grid
    * interpolation.
    */
   bool HasGridInterpolations() const
   {
      return m_gridWI.IsValid() && m_gridIW.IsValid();
   }

   /*!
    * Returns the grid distance in pixels used for generation of grid
    * interpolations, or zero if grid interpolations have not been initialized
    * or are not valid in this %SplineWorldTransformation object.
    *
    * See InitializeGridInterpolations() for information on grid interpolation.
    */
   double GridInterpolationDelta() const
   {
      return HasGridInterpolations() ? m_gridIW.Delta() : 0.0;
   }

   /*!
    * Returns the identifier prefix of all XISF properties used to serialize a
    * %SplineWorldTransformation object.
    */
   static IsoString PropertyPrefix()
   {
      return "PCL:AstrometricSolution:SplineWorldTransformation:";
   }

   /*!
    * Returns a list of XISF image properties serializing this
    * %SplineWorldTransformation object.
    */
   PropertyArray ToProperties() const;

   /*!
    * Serializes this %SplineWorldTransformation instance in raw binary format
    * and stores the result in the specified \a data array.
    *
    * \deprecated This function only exists to support legacy modules and
    * applications, and will be removed in a future PCL version. It must not be
    * used in newly produced code.
    */
   void Serialize( ByteArray& data ) const;

   /*!
    * Returns the number of control points employed to generate the surface
    * splines used for coordinate transformations.
    *
    * The value returned by this function is the length of the original list of
    * control points specified upon object construction or deserialization; the
    * actual sets of data points being used internally by surface splines are
    * usually smaller because surface simplification is enabled by default.
    *
    * \sa NativeControlPoints(), ImageControlPoints(), GetSplineLengths()
    */
   int NumberOfControlPoints() const
   {
      return int( m_controlPointsW.Length() );
   }

   /*!
    * Returns a reference to the list of control points in native spherical
    * coordinates.
    *
    * For each point \e p in the returned array, \e p.x is the native longitude
    * and \e p.y is the native latitude, both expressed in degrees.
    *
    * For each point \a p in the returned array, there is a corresponding point
    * \a q in the array of image control points returned by the
    * \a ImageControlPoints() member function, whose \e q.x and \e q.y
    * components are the image coordinates in pixels corresponding to \e p.
    */
   const Array<DPoint>& NativeControlPoints() const
   {
      return m_controlPointsW;
   }

   /*!
    * Returns a reference to the list of control points in image coordinates.
    *
    * For each point \e p in the returned array, \e p.x and \e p.y are the
    * components of a control point in image coordinates, expressed in pixels.
    *
    * For each point \a p in the returned array, there is a corresponding point
    * \a q in the array of native control points returned by the
    * \a NativeControlPoints() member function, whose \e q.x and \e q.y
    * components are the spherical native coordinates in degrees corresponding
    * to \e p.
    */
   const Array<DPoint>& ImageControlPoints() const
   {
      return m_controlPointsI;
   }

   /*!
    * Returns the type of radial basis function (RBF) used by the internal
    * surface splines. In the current implementation the following basis
    * functions are supported for spline-based astrometric solutions:
    *
    * RadialBasisFunction::ThinPlateSpline \n
    * RadialBasisFunction::VariableOrder \n
    * RadialBasisFunction::DDMThinPlateSpline \n
    * RadialBasisFunction::DDMMultiquadric
    */
   rbf_type RBFType() const
   {
      return m_rbf;
   }

   /*!
    * Returns the derivative order of continuity of the internal surface
    * splines used for coordinate transformations.
    */
   int SplineOrder() const
   {
      return m_order;
   }

   /*!
    * Returns an identifying name for the RBF interpolation used for generation
    * of surface splines.
    */
   IsoString RBFTypeName() const;

   /*!
    * Returns the maximum number of interpolation points allowed for the
    * internal surface splines used for coordinate transformations.
    */
   int MaxSplinePoints() const
   {
      return m_maxSplinePoints;
   }

   /*!
    * Provides the number of data points in the internal surface splines used
    * for coordinate transformations.
    *
    * \param[out] xWI   Number of spline points used for world-to-image
    *                   coordinate transformations on the native longitude
    *                   direction.
    *
    * \param[out] yWI   Number of spline points used for world-to-image
    *                   coordinate transformations on the native latitude
    *                   direction.
    *
    * \param[out] xIW   Number of spline points used for image-to-world
    *                   coordinate transformations on the X-axis direction.
    *
    * \param[out] yIW   Number of spline points used for image-to-world
    *                   coordinate transformations on the Y-axis direction.
    */
   void GetSplineLengths( int& xWI, int& yWI, int& xIW, int& yIW ) const
   {
      xWI = m_splineWI.SplineX().Length();
      yWI = m_splineWI.SplineY().Length();
      xIW = m_splineIW.SplineX().Length();
      yIW = m_splineIW.SplineY().Length();
   }

   /*!
    * Returns true iff the lists of transformation control points were
    * truncated before generation of surface splines.
    *
    * No truncation should happen under normal working conditions if surface
    * simplification is enabled.
    *
    * If no simplification is used, truncation may happen directly on the
    * original lists of control points. Without surface simplification, a
    * maximum of 2100 control points is imposed in the current implementation
    * to prevent excessive execution times for surface spline generation, which
    * grow approximately with O(n^3) complexity.
    */
   bool TruncatedControlPoints() const
   {
      return m_truncated; // fetched during initialization
   }

   /*!
    * Returns true iff the surface simplification algorithm has been enabled
    * for generation of surface splines. See the class constructor for more
    * information.
    */
   bool SimplifiersEnabled() const
   {
      return m_useSimplifiers;
   }

   /*!
    * Returns the outlier rejection fraction of the surface simplifier used for
    * generation of surface splines. See the class constructor for more
    * information.
    */
   float SimplifierRejectFraction() const
   {
      return m_simplifierRejectFraction;
   }

   /*!
    * Returns the residual errors in pixels after surface simplification.
    *
    * \param[out] xWI   Simplification error for the world-to-image coordinate
    *                   transformation on the native longitude direction.
    *
    * \param[out] yWI   Simplification error for the world-to-image coordinate
    *                   transformation on the native latitude direction.
    *
    * \param[out] xIW   Simplification error for image-to-world coordinate
    *                   transformation on the X-axis direction.
    *
    * \param[out] yIW   Simplification error for image-to-world coordinate
    *                   transformation on the Y-axis direction.
    */
   void GetSplineErrors( double& xWI, double& yWI, double& xIW, double& yIW ) const
   {
      xWI = m_splineWI.ErrorX();
      yWI = m_splineWI.ErrorY();
      xIW = m_splineIW.ErrorX();
      yIW = m_splineIW.ErrorY();
   }

private:

   IsoString              m_version = __PCL_WCS_SPLINE_VERSION_CURRENT;
   Array<DPoint>          m_controlPointsW;
   Array<DPoint>          m_controlPointsI;
   rbf_type               m_rbf = __PCL_WCS_DEFAULT_SPLINE_RBF;
   int                    m_order = __PCL_WCS_DEFAULT_SPLINE_ORDER;
   float                  m_smoothness = __PCL_WCS_DEFAULT_SPLINE_SMOOTHNESS;
   int                    m_maxSplinePoints = __PCL_WCS_DEFAULT_SPLINE_MAX_POINTS;
   bool                   m_useSimplifiers = __PCL_WCS_DEFAULT_SURFACE_SIMPLIFIER_ENABLED;
   float                  m_simplifierRejectFraction = __PCL_WCS_DEFAULT_SURFACE_SIMPLIFIER_REJECT_FRACTION;
   FVector                m_weights;
   bool                   m_truncated = false; // true => truncated control point vectors
   Homography             m_projectiveWI;
   Homography             m_projectiveIW;
   PointSurfaceSpline     m_splineWI;
   PointSurfaceSpline     m_splineIW;
   PointGridInterpolation m_gridWI;
   PointGridInterpolation m_gridIW;
   LinearTransformation   m_linearIW;

   static PointSurfaceSpline PointSurfaceSplineFromProperties( const PropertyArray&, const IsoString& version );
   static PropertyArray PointSurfaceSplineToProperties( const PointSurfaceSpline&, const IsoString& );

   static SurfaceSpline<double> SurfaceSplineFromProperties( const PropertyArray&, const IsoString& version );
   static PropertyArray SurfaceSplineToProperties( const SurfaceSpline<double>&, const IsoString& );

   static PointGridInterpolation PointGridInterpolationFromProperties( const PropertyArray&, const IsoString& version );
   static PropertyArray PointGridInterpolationToProperties( const PointGridInterpolation&, const IsoString& );

   void Deserialize( const ByteArray& );
   void EnsureValidRBFParameters();
   void InitializeSplines();
   void CalculateLinearApproximation();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_WorldTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/WorldTransformation.h - Released 2024-12-27T18:16:00Z
