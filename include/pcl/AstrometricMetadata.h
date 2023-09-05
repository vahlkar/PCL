//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// pcl/AstrometricMetadata.h - Released 2023-08-28T15:23:15Z
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

#ifndef __PCL_AstrometricMetadata_h
#define __PCL_AstrometricMetadata_h

/// \file pcl/AstrometricMetadata.h

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Optional.h>
#include <pcl/Point.h>
#include <pcl/ProjectionBase.h>
#include <pcl/Property.h>
#include <pcl/SphericalRotation.h>
#include <pcl/TimePoint.h>
#include <pcl/WCSKeywords.h>
#include <pcl/WorldTransformation.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
namespace pi
{
   class ImageWindow;
}
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup astrometry_support Astrometry Support Classes
 *
 * This group of classes implement the core astrometry support on the
 * PixInsight platform.
 *
 * The original implementation of these classes has been contributed by Spanish
 * software developer and PTeam member Andrés del Pozo, who is also the author
 * of all astrometry scripts available in the standard PixInsight distribution,
 * including the ImageSolver and AnnotateImage scripts, among others.
 */

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
class PCL_CLASS ImageWindow;
#endif

class PCL_CLASS XISFReader;
class PCL_CLASS XISFWriter;

/*!
 * \class AstrometricMetadata
 * \brief Astrometric metadata
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS AstrometricMetadata
{
public:

   /*!
    * \struct pcl::AstrometricMetadata::DescriptionItems
    * \brief A collection of strings describing the properties and parameters
    * of an astrometric solution.
    */
   struct DescriptionItems
   {
      String referenceMatrix;
      String wcsTransformationType;
      String controlPoints;
      String splineLengths;
      String splineErrors;
      String projectionName;
      String projectionOrigin;
      String resolution;
      String rotation;
      String referenceSystem;
      String observationStartTime;
      String observationEndTime;
      String observerLocation;
      String focalDistance;
      String pixelSize;
      String fieldOfView;
      String centerCoordinates;
      String topLeftCoordinates;
      String topRightCoordinates;
      String bottomLeftCoordinates;
      String bottomRightCoordinates;
      String referenceCatalog;
      String creationTime;
      String creationSoftware;
   };

   /*!
    * Default constructor. Constructs an uninitialized %AstrometricMetadata
    * object.
    */
   AstrometricMetadata() = default;

   /*!
    * Copy constructor.
    */
   AstrometricMetadata( const AstrometricMetadata& x )
      : m_projection( x.m_projection->Clone() )
      , m_transformWI( x.m_transformWI->Clone() )
      , m_refSys( x.m_refSys )
      , m_width( x.m_width )
      , m_height( x.m_height )
      , m_pixelSize( x.m_pixelSize )
      , m_obsStartTime( x.m_obsStartTime )
      , m_obsEndTime( x.m_obsEndTime )
      , m_geoLongitude( x.m_geoLongitude )
      , m_geoLatitude( x.m_geoLatitude )
      , m_geoHeight( x.m_geoHeight )
      , m_resolution( x.m_resolution )
      , m_focalLength( x.m_focalLength )
      , m_creationTime( x.m_creationTime )
      , m_catalog( x.m_catalog )
      , m_creatorApp( x.m_creatorApp )
      , m_creatorModule( x.m_creatorModule )
      , m_creatorOS( x.m_creatorOS )
      , m_description( x.m_description )
   {
   }

   /*!
    * Move constructor.
    */
   AstrometricMetadata( AstrometricMetadata&& ) = default;

   /*!
    * Constructor from minimal data.
    *
    * \param projection          Pointer to a dynamically allocated projection
    *                            system.
    *
    * \param worldTransformation Pointer to a dynamically allocated world
    *                            coordinate transformation.
    *
    * \param width               Width in pixels of the image with which this
    *                            astrometric solution is associated.
    *
    * \param height              Height in pixels of the image with which this
    *                            astrometric solution is associated.
    *
    * Ownership of the specified \a projection and \a worldTransformation
    * objects will be transferred to this object, which will destroy and
    * deallocate them when appropriate.
    */
   AstrometricMetadata( ProjectionBase* projection, WorldTransformation* worldTransformation, int width, int height );

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   // Implemented in /core/Components/ImageWindow/ImageWindow.cpp
   AstrometricMetadata( const pi::ImageWindow* );
#else
   /*!
    * Constructs an %AstrometricMetadata object from the existing keywords and
    * properties of an image \a window.
    */
   AstrometricMetadata( const ImageWindow& window );
#endif

   /*!
    * Constructs an %AstrometricMetadata object from the existing keywords and
    * properties of the currently selected image in an XISF \a reader.
    */
   AstrometricMetadata( XISFReader& reader );

   /*!
    * Virtual destructor.
    */
   virtual ~AstrometricMetadata()
   {
   }

   /*!
    * Copy constructor. Returns a reference to this object.
    */
   AstrometricMetadata& operator =( const AstrometricMetadata& x )
   {
      m_projection    = x.m_projection->Clone();
      m_transformWI   = x.m_transformWI->Clone();
      m_refSys        = x.m_refSys;
      m_equinox       = x.m_equinox;
      m_width         = x.m_width;
      m_height        = x.m_height;
      m_pixelSize     = x.m_pixelSize;
      m_obsStartTime  = x.m_obsStartTime;
      m_obsEndTime    = x.m_obsEndTime;
      m_geoLongitude  = x.m_geoLongitude;
      m_geoLatitude   = x.m_geoLatitude;
      m_geoHeight     = x.m_geoHeight;
      m_resolution    = x.m_resolution;
      m_focalLength   = x.m_focalLength;
      m_creationTime  = x.m_creationTime;
      m_catalog       = x.m_catalog;
      m_creatorApp    = x.m_creatorApp;
      m_creatorModule = x.m_creatorModule;
      m_creatorOS     = x.m_creatorOS;
      m_description   = x.m_description;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   AstrometricMetadata& operator =( AstrometricMetadata&& ) = default;

   /*!
    * Returns true iff this object is valid. A valid %AstrometricMetadata
    * instance defines a projection system and a world transformation.
    */
   bool IsValid() const
   {
      return !m_projection.IsNull() && !m_transformWI.IsNull();
   }

   /*!
    * Checks that this astrometric solution is valid and can perform coherent
    * coordinate transformations, and returns the reprojection errors
    * measured at the center and the four corners of the image.
    *
    * \param[out] centerErrors      Reprojection errors at the geometric center
    *                               of the image, or (Width()/2,Height()/2)
    *                               image coordinates.
    *
    * \param[out] topLeftErrors     Reprojection errors at (0,0) image
    *                               coordinates.
    *
    * \param[out] topRightErrors    Reprojection errors at (Width(),0) image
    *                               coordinates.
    *
    * \param[out] bottomLeftErrors  Reprojection errors at (0,Height()) image
    *                               coordinates.
    *
    * \param[out] bottomRightErrors Reprojection errors at (Width(),Height())
    *                               image coordinates.
    *
    * This routine performs five successive coordinate transformations, from
    * image to celestial and from celestial to image coordinates, computed for
    * the four corners and the geometric center of the image. The reported
    * values in the specified point variables are the differences between the
    * initial and final image coordinates, on the X and Y axes, in pixels.
    *
    * This function throws an Error exception if the solution has not been
    * initialized, or if it cannot perform valid coordinate transformations.
    */
   void Verify( DPoint& centerErrors,
                DPoint& topLeftErrors, DPoint& topRightErrors,
                DPoint& bottomLeftErrors, DPoint& bottomRightErrors ) const;

   /*!
    * Checks that this astrometric solution is valid and can perform coherent
    * coordinate transformations.
    *
    * \param tolerance     Maximum difference in pixels allowed for validation.
    *                      The default value is 0.01 pixels.
    *
    * This routine performs two successive coordinate transformations, from
    * image to celestial and from celestial to image coordinates, computed for
    * the geometric center of the image. If the absolute value of the
    * difference between the initial and final image coordinates is greater
    * than the specified tolerance in pixels, for one or both axes, an Error
    * exception is thrown.
    *
    * This function also throws an Error exception if the solution has not been
    * initialized, or if it cannot perform valid coordinate transformations.
    */
   void Validate( double tolerance = 0.01 ) const;

   /*!
    * Returns true if this object uses a world transformation based on 2-D
    * surface splines (or <em>thin plate splines</em>). Returns false if a
    * standard WCS linear transformation is being used.
    */
   bool HasSplineWorldTransformation() const
   {
      return dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() ) != nullptr;
   }

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   // Implemented in /core/Components/ImageWindow/ImageWindow.cpp
   void Write( pi::ImageWindow* window, bool notify = true ) const;
#else
   /*!
    * Updates the keywords and properties of an image \a window to represent
    * this astrometric solution.
    *
    * The pixel dimensions of the image in the target \a window must be
    * coherent with this astrometric solution. If that is not true, this member
    * function will throw an Error exception. An Error exception will also be
    * thrown if this object does not define a valid astrometric solution.
    *
    * See the UpdateBasicKeywords(), UpdateWCSKeywords() and UpdateProperties()
    * member functions for information on the metadata items modified by this
    * function.
    */
   void Write( ImageWindow& window, bool notify = true ) const;
#endif

   /*!
    * Updates the keywords and properties of the current image in an XISF
    * \a writer to store the astrometric solution represented by this object.
    *
    * The caller must ensure that the pixel dimensions of the current image in
    * the target %XISF \a writer, that is, the width and height of the image
    * being generated, are coherent with this astrometric solution. Currently
    * this condition cannot be verified or enforced by this member function.
    *
    * This function will throw an Error exception if this object does not
    * define a valid astrometric solution.
    *
    * See the UpdateBasicKeywords(), UpdateWCSKeywords() and UpdateProperties()
    * member functions for information on the metadata items modified by this
    * function.
    */
   void Write( XISFWriter& writer ) const;

   /*!
    * Returns the name of the coordinate reference system to which this
    * astrometric solution has been referred.
    *
    * This corresponds to the reference system of the coordinates of the
    * reference point sources (usually stars with data acquired from
    * astrometric catalogs) used to compute the astrometric solution. The
    * current implementation supports the following reference systems:
    *
    * \b ICRS
    *
    * Reference point source positions are \e astrometric: they include
    * corrections for space motion (proper motions, annual parallax and radial
    * velocity, when available) and gravitational deflection of light, but
    * \e not annual aberration. This is the default reference system that is
    * assumed when none is specified or acquired from existing metadata.
    *
    * \b GCRS
    *
    * Reference point source positions are \e proper: they include corrections
    * for space motion (proper motions, annual parallax and radial velocity,
    * when available), gravitational deflection of light, and annual aberration
    * (rigorous relativistic model).
    *
    * Other nonstandard values can be returned by this function, such as
    * different values of the RADESYS FITS keyword. Such values will be
    * preserved but won't be supported by standard platform implementations.
    */
   IsoString ReferenceSystem() const
   {
      return m_refSys.IsEmpty() ? IsoString( "ICRS" ) : m_refSys;
   }

   /*!
    * Sets the coordinate reference system. See ReferenceSystem() for more
    * information and supported values.
    */
   void SetReferenceSystem( const IsoString& refSys )
   {
      m_refSys = refSys.Trimmed();
   }

   /*!
    * Returns the name of the reference system of celestial coordinates
    * derived from the specified image metadata.
    *
    * \param properties       A list of XISF image properties.
    *
    * \param keywords         A list of FITS header keywords.
    *
    * The reference system will be inferred from the following metadata items:
    *
    * \li The standard XISF property 'Observation:CelestialReferenceSystem'
    *
    * \li The standard FITS keyword 'RADESYS'.
    *
    * In case of metadata duplicity, XISF properties always take precedence
    * over FITS keywords.
    *
    * Typical returned values are "ICRS" and "GCRS". If no valid reference
    * system can be deduced from the specified metadata, this function returns
    * an empty string.
    */
   static IsoString ReferenceSystemFromMetadata( const PropertyArray& properties, const FITSKeywordArray& keywords );

   /*!
    * Returns the width in pixels of the image associated with the astrometric
    * solution represented by this object.
    */
   int Width() const
   {
      return m_width;
   }

   /*!
    * Returns the height in pixels of the image associated with the astrometric
    * solution represented by this object.
    */
   int Height() const
   {
      return m_height;
   }

   /*!
    * Returns the bounding rectangle in pixels of the image associated with the
    * astrometric solution represented by this object.
    *
    * The returned rectangle is equivalent to:
    *
    * \code
    * Rect( Width(), Height() )
    * \endcode
    */
   Rect Bounds() const
   {
      return Rect( m_width, m_height );
   }

   /*!
    * Returns a pointer to the projection system of this astrometric solution.
    * Returns \c nullptr if this object is not valid.
    */
   const ProjectionBase* Projection() const
   {
      return m_projection.Pointer();
   }

   /*!
    * Returns a pointer to the world coordinate transformation of this
    * astrometric solution. Returns \c nullptr if this object is not valid.
    */
   const WorldTransformation* WorldTransform() const
   {
      return m_transformWI.Pointer();
   }

   /*!
    * Returns the image resolution in seconds of arc per pixel, calculated from
    * the specified \a focal distance in millimeters.
    */
   double ResolutionFromFocal( double focal ) const
   {
      return (focal > 0) ? m_pixelSize.OrElse( 0 )/focal * 0.18/Const<double>::pi() : 0.0;
   }

   /*!
    * Returns the focal distance in millimeters, calculated from the specified
    * image \a resolution expressed in seconds of arc per pixel.
    */
   double FocalFromResolution( double resolution ) const
   {
      return ResolutionFromFocal( resolution );
   }

   /*!
    * Returns the angle of rotation of the Y axis of the image with respect to
    * the north direction, as represented on a flat projection of the celestial
    * sphere. A rotation angle of zero aligns north along the positive Y axis.
    * A positive rotation angle rotates the Y axis from north to east.
    *
    * The returned value is the rotation angle of the projection in degrees.
    * The \a flipped variable will be true iff the projection is mirrored on
    * the east-west direction.
    */
   double Rotation( bool& flipped ) const;

   /*!
    * Returns the resolution of the image in degrees per pixel.
    *
    * This value usually is an approximation since it changes across the image.
    * It should only be used for informative purposes.
    */
   double Resolution() const
   {
      return m_resolution;
   }

   /*!
    * Returns the radius in degrees of a circular region of the sky
    * circumscribed to the bounding rectangle of this astrometric solution.
    *
    * The returned radius can be used to perform search operations with star
    * catalogs and solar system ephemerides.
    */
   double SearchRadius() const
   {
      if ( !IsValid() )
         throw Error( "Invalid call to AstrometricMetadata::SearchRadius(): No astrometric solution." );
      DPoint cRD;
      if ( !ImageToCelestial( cRD, DPoint( 0.5*m_width, 0.5*m_height ) ) )
         throw Error( "Failed to perform ImageToCelestial() coordinate transformation for the image center" );
      DPoint pRD1, pRD2, pRD3, pRD4;
      if ( !ImageToCelestial( pRD1, DPoint( 0,       0 ) )
        || !ImageToCelestial( pRD2, DPoint( m_width, 0 ) )
        || !ImageToCelestial( pRD3, DPoint( 0,       m_height ) )
        || !ImageToCelestial( pRD4, DPoint( m_width, m_height ) ) )
      {
         return 180;
      }
      return Max( Max( Max( m_projection->Distance( cRD, pRD1 ), m_projection->Distance( cRD, pRD2 ) ),
                       m_projection->Distance( cRD, pRD3 ) ),
                  m_projection->Distance( cRD, pRD4 ) );
   }

   /*!
    * Returns the observation start time, if available. When defined, the
    * returned value should be represented in the UTC timescale.
    */
   Optional<TimePoint> ObservationStartTime() const
   {
      return m_obsStartTime;
   }

   /*!
    * Sets the observation start time. The specified \a startTime value should
    * be represented in the UTC timescale.
    */
   void SetObservationStartTime( TimePoint startTime )
   {
      m_obsStartTime = startTime;
   }

   /*!
    * Returns the observation end time, if available. When defined, the
    * returned value should be represented in the UTC timescale.
    */
   Optional<TimePoint> ObservationEndTime() const
   {
      return m_obsEndTime;
   }

   /*!
    * Sets the observation end time. The specified \a endTime value should be
    * represented in the UTC timescale.
    */
   void SetObservationEndTime( TimePoint endTime )
   {
      m_obsEndTime = endTime;
   }

   /*!
    * Returns an estimate of the observation middle time. If both the start and
    * end times are defined, returns the time point between them. If only the
    * start time is defined, it is returned. Otherwise an undefined object is
    * returned.
    */
   Optional<TimePoint> ObservationMiddleTime() const
   {
      if ( !m_obsStartTime.IsDefined() )
         return Optional<TimePoint>();
      if ( !m_obsEndTime.IsDefined() )
         return m_obsStartTime;
      return m_obsStartTime() + (m_obsEndTime() - m_obsStartTime())/2;
   }

   /*!
    * Returns the geodetic longitude of the observation location, if available.
    * The returned value is expressed in degrees in the range (-180,+180],
    * reckoned positive eastward of the reference meridian.
    */
   Optional<double> LocationLongitude() const
   {
      return m_geoLongitude;
   }

   /*!
    * Sets the geodetic longitude of the observation location. The specified
    * \a longitude must be expressed in degrees and can be either in the range
    * [0,360] or [-180,+180], reckoned positive eastward.
    *
    * If a value out of the valid range is specified, this function will throw
    * an Error exception.
    */
   void SetLocationLongitude( double longitude )
   {
      if ( longitude > 180 )
         longitude -= 360;
      else if ( longitude <= -180 )
         longitude += 360;
      if ( longitude < -180 || longitude > +180 )
         throw Error( "AstrometricMetadata::SetLocationLongitude(): Geographic longitude out of range." );
      m_geoLongitude = longitude;
   }

   /*!
    * Returns the geodetic latitude of the observation location, if available.
    * The returned value is expressed in degrees in the range [-90,+90],
    * reckoned positive north of the equator.
    */
   Optional<double> LocationLatitude() const
   {
      return m_geoLatitude;
   }

   /*!
    * Sets the geodetic latitude of the observation location. The specified
    * \a latitude must be expressed in degrees and must be in the range
    * [-90,+90], reckoned positive north of the equator.
    *
    * If a value out of the valid range is specified, this function will throw
    * an Error exception.
    */
   void SetLocationLatitude( double latitude )
   {
      if ( latitude < -90 || latitude > +90 )
         throw Error( "AstrometricMetadata::SetLocationLatitude(): Geographic latitude out of range." );
      m_geoLatitude = latitude;
   }

   /*!
    * Returns the geodetic height of the observation location in meters, if
    * available.
    */
   Optional<double> LocationHeight() const
   {
      return m_geoHeight;
   }

   /*!
    * Sets the geodetic height of the observation location in meters.
    */
   void SetLocationHeight( double height )
   {
      m_geoHeight = height;
   }

   /*!
    * Returns the physical pixel size in micrometers, if available.
    */
   Optional<double> PixelSize() const
   {
      return m_pixelSize;
   }

   /*!
    * Redefines the physical pixel size in micrometers. Recalculates the focal
    * distance as a function of the pixel size.
    */
   void SetPixelSize( double pixelSize )
   {
      m_pixelSize = pixelSize;
      m_focalLength = FocalFromResolution( m_resolution );
   }

   /*!
    * Returns the time this astrometric solution was generated, if available.
    * When defined, the returned value should be represented in the UTC
    * timescale.
    */
   Optional<TimePoint> CreationTime() const
   {
      return m_creationTime;
   }

   /*!
    * Returns the name of the star catalog used to generate this astrometric
    * solution, or an empty string if the corresponding metadata item is not
    * available.
    */
   String Catalog() const
   {
      return m_catalog;
   }

   /*!
    * Returns the name (typically with version numbers) of the software
    * application used to generate this astrometric solution, or an empty
    * string if the corresponding metadata item is not available.
    */
   String CreatorApplication() const
   {
      return m_creatorApp;
   }

   /*!
    * Returns the name (typically with version numbers) of the software module
    * or subsystem (such as a process or script) used to generate this
    * astrometric solution, or an empty string if the corresponding metadata
    * item is not available.
    */
   String CreatorModule() const
   {
      return m_creatorModule;
   }

   /*!
    * Returns the name of the operating system under which this astrometric
    * solution was generated, or an empty string if the corresponding metadata
    * item is not available. Currently this member function can return one of
    * "FreeBSD", "Linux", "macOS", and "Windows".
    */
   String CreatorOS() const
   {
      return m_creatorOS;
   }

   /*!
    * Transformation from image coordinates to celestial coordinates.
    *
    * \param[out] pRD   Reference to a point where the output equatorial
    *                   spherical coordinates will be stored, expressed in
    *                   degrees. \a pRD.x will be the right ascension and
    *                   \a pRD.y the declination. Output right ascensions are
    *                   constrained to the [0,360) range. Output declinations
    *                   are in the range [-90,+90].
    *
    * \param pI         Input image coordinates in pixels. The specified
    *                   location can legally lie outside the image bounds
    *                   defined by [0,0]-[Width(),Height()].
    *
    * Returns true iff the specified point \a pI can be projected on the
    * celestial sphere using this astrometric solution.
    *
    * The output coordinates stored in \a pRD will be referred to the reference
    * system of this astrometric solution, either ICRS or GCRS, as returned by
    * the ReferenceSystem() member function.
    *
    * \sa RawImageToCelestial(), CelestialToImage()
    */
   bool ImageToCelestial( DPoint& pRD, const DPoint& pI ) const
   {
      if ( !IsValid() )
         throw Error( "Invalid call to AstrometricMetadata::ImageToCelestial(): No astrometric solution." );
      if ( m_projection->Inverse( pRD, m_transformWI->Inverse( pI ) ) )
      {
         // Constrain right ascension to the [0,360) range.
         if ( pRD.x < 0 )
            pRD.x += 360;
         else if ( pRD.x >= 360 )
            pRD.x -= 360;
         return true;
      }
      return false;
   }

   /*!
    * Transformation from image coordinates to celestial coordinates, without
    * enforcing a valid range of right ascensions.
    *
    * \param[out] pRD   Reference to a point where the output equatorial
    *                   spherical coordinates will be stored, expressed in
    *                   degrees. \a pRD.x will be the right ascension and
    *                   \a pRD.y the declination. Output right ascensions are
    *                   \e not constrained to the [0,360) range. Output
    *                   declinations are in the range [-90,+90].
    *
    * \param pI         Input image coordinates in pixels. The specified
    *                   location can legally lie outside the image bounds
    *                   defined by [0,0]-[Width(),Height()].
    *
    * Returns true iff the specified point \a pI can be projected on the
    * celestial sphere using this astrometric solution.
    *
    * The output coordinates stored in \a pRD will be referred to the reference
    * system of this astrometric solution, either ICRS or GCRS, as returned by
    * the ReferenceSystem() member function.
    *
    * This function is useful for interpolation schemes where discontinuities
    * caused by zero crossings in right ascension, i.e. abrupt changes from 360
    * to 0 degrees, are not admissible numerically. Right ascensions returned
    * by this function can be larger than 360 degrees or less than zero,
    * ensuring smooth transitions.
    *
    * \sa ImageToCelestial(), CelestialToImage()
    */
   bool RawImageToCelestial( DPoint& pRD, const DPoint& pI ) const
   {
      if ( !IsValid() )
         throw Error( "Invalid call to AstrometricMetadata::RawImageToCelestial(): No astrometric solution." );
      return m_projection->Inverse( pRD, m_transformWI->Inverse( pI ) );
   }

   /*!
    * Transformation from celestial coordinates to image coordinates.
    *
    * \param[out] pI    Reference to a point where the output image coordinates
    *                   will be stored.
    *
    * \param pRD        Input equatorial spherical coordinates expressed in
    *                   degrees. \a pRD.x is the right ascension and \a pRD.y
    *                   is declination.
    *
    * Returns true iff the specified celestial coordinates can be reprojected
    * on the image coordinate system. Note that the output image coordinates
    * can lie outside of the image bounds defined by [0,0]-[Width(),Height()].
    *
    * The input coordinates specified in \a pRD are expected to be referred to
    * the reference system of this astrometric solution, either ICRS or GCRS,
    * as returned by the ReferenceSystem() member function.
    *
    * \sa ImageToCelestial(), RawImageToCelestial()
    */
   bool CelestialToImage( DPoint& pI, const DPoint& pRD ) const
   {
      if ( !IsValid() )
         throw Error( "Invalid call to AstrometricMetadata::CelestialToImage(): No astrometric solution." );
      DPoint pW;
      if ( m_projection->Direct( pW, pRD ) )
      {
         pI = m_transformWI->Direct( pW );
         return true;
      }
      return false;
   }

   /*!
    * Regenerates the astrometric solution from standardized metadata.
    *
    * \param properties       A list of XISF image properties describing
    *                         critical astrometry-related metadata items.
    *
    * \param keywords         A list of FITS header keywords, which may
    *                         contain a set of standard WCS keywords to define
    *                         a linear world transformation from celestial to
    *                         image coordinates. If the required metadata is
    *                         available in the \a properties array, the
    *                         corresponding keywords contained by this object
    *                         will be ignored.
    *
    * \param width            Width in pixels of the image with which this
    *                         astrometric solution is associated.
    *
    * \param height           Height in pixels of the image with which this
    *                         astrometric solution is associated.
    *
    * The following standard %XISF properties will be extracted from the
    * specified \a properties array, if available:
    *
    * <pre>
    * Observation:CelestialReferenceSystem
    * Observation:Center:RA
    * Observation:Center:Dec
    * Observation:Equinox
    * Observation:Time:Start
    * Observation:Time:End
    * Observation:Location:Longitude
    * Observation:Location:Latitude
    * Observation:Location:Elevation
    * Instrument:Telescope:FocalLength
    * Instrument:Sensor:XPixelSize
    * </pre>
    *
    * The following nonstandard %XISF properties support our native astrometric
    * solutions since core version 1.8.9-2:
    *
    * <pre>
    * PCL:AstrometricSolution:ProjectionSystem
    * PCL:AstrometricSolution:ReferenceCelestialCoordinates
    * PCL:AstrometricSolution:ReferenceImageCoordinates
    * PCL:AstrometricSolution:ReferenceNativeCoordinates
    * PCL:AstrometricSolution:CelestialPoleNativeCoordinates
    * PCL:AstrometricSolution:LinearTransformationMatrix
    * PCL:AstrometricSolution:SplineWorldTransformation
    * PCL:AstrometricSolution:Catalog
    * PCL:AstrometricSolution:CreationTime
    * PCL:AstrometricSolution:CreatorApplication
    * PCL:AstrometricSolution:CreatorOS
    * PCL:AstrometricSolution:CreatorModule
    * </pre>
    *
    * %XISF properties will always take precedence over existing equivalent
    * %FITS keywords.
    *
    * If the specified \a properties array contains a valid serialization of
    * spline parameters and control points, the astrometric solution will use a
    * high-precision world transformation based on two-dimensional surface
    * splines, also knwon as <em>thin plate splines</em>, which is capable of
    * modeling local image distortions that are intractable with WCS linear
    * transformations.
    *
    * If this object contains valid metadata before calling this function, it
    * will be disposed as appropriate, and a completely new astrometric
    * solution will be constructed.
    *
    * This member function can throw exceptions (of the Error class) if the
    * specified \a properties and/or \a keywords arrays contain wrong or
    * invalid data, or if the generated coordinate transformations are invalid
    * (in the numerical or geometric sense).
    */
   void Build( const PropertyArray& properties, const FITSKeywordArray& keywords, int width, int height );

   /*!
    * Updates the specified \a keywords array with basic astrometric FITS
    * header keywords. This includes a number of standard instrumental and
    * observational keywords, as well as some customary nonstandard keywords,
    * included for compatibility with third-party applications:
    *
    * <pre>
    * RA
    * OBJCTRA
    * DEC
    * OBJCTDEC
    * DATE-OBS
    * DATE-END
    * OBSGEO-L
    * LONG-OBS
    * OBSGEO-B
    * LAT-OBS
    * OBSGEO-H
    * ALT-OBS
    * FOCALLEN
    * XPIXSZ
    * YPIXSZ
    * PIXSIZE
    * </pre>
    *
    * Keywords are updated when the corresponding metadata items are available.
    */
   void UpdateBasicKeywords( FITSKeywordArray& keywords ) const;

   /*!
    * Updates or removes standard WCS FITS header keywords in the specified
    * \a keywords array:
    *
    * <pre>
    * RADESYS
    * EQUINOX
    * EPOCH
    * CTYPE1
    * CTYPE2
    * CRVAL1
    * CRVAL2
    * CRPIX1
    * CRPIX2
    * CD1_1
    * CD1_2
    * CD2_1
    * CD2_2
    * PC1_1
    * PC1_2
    * PC2_1
    * PC2_2
    * CDELT1
    * CDELT2
    * CROTA1
    * CROTA2
    * PV1_1
    * PV1_2
    * PV1_3
    * PV1_4
    * LONPOLE
    * LATPOLE
    * </pre>
    *
    * if \a generate is true, a new set of WCS FITS keywords will be generated
    * (by replacing existing ones or creating them as necessary) in the
    * \a keywords array.
    *
    * If \a generate is false, existing WCS FITS keywords will be removed and
    * no new ones will be generated. This is the default behavior since
    * PixInsight core version 1.8.9-2, when our native astrometric solutions
    * no longer depend on FITS keywords.
    */
   void UpdateWCSKeywords( FITSKeywordArray& keywords, bool generate = false ) const;

   /*!
    * Updates the specified \a properties array with a restricted set of view
    * properties to reflect the state of this astrometric solution.
    *
    * The following standard XISF properties will be created or redefined:
    *
    * <pre>
    * Instrument:Telescope:FocalLength
    * Instrument:Sensor:XPixelSize
    * Instrument:Sensor:YPixelSize
    * Observation:Center:RA
    * Observation:Center:Dec
    * Observation:CelestialReferenceSystem
    * Observation:Equinox
    * </pre>
    *
    * The following properties can be removed if the transformation is valid,
    * since the default coordinate reference point is the geometric center of
    * the image, which is used to calculate the values of the standard
    * \c Observation:Center:RA and \c Observation:Center:Dec properties:
    *
    * <pre>
    * Observation:Center:X
    * Observation:Center:Y
    * </pre>
    *
    * The following properties will be created or redefined if the
    * corresponding metadata items are available, or removed otherwise:
    *
    * <pre>
    * Observation:Time:Start
    * Observation:Time:End
    * Observation:Location:Longitude
    * Observation:Location:Latitude
    * Observation:Location:Elevation
    * </pre>
    *
    * The following nonstandard properties (which support our native
    * astrometric solutions since core version 1.8.9-2) and will be created,
    * redefined, or removed as necessary:
    *
    * <pre>
    * PCL:AstrometricSolution:ProjectionSystem
    * PCL:AstrometricSolution:ReferenceCelestialCoordinates
    * PCL:AstrometricSolution:ReferenceImageCoordinates
    * PCL:AstrometricSolution:ReferenceNativeCoordinates
    * PCL:AstrometricSolution:CelestialPoleNativeCoordinates
    * PCL:AstrometricSolution:LinearTransformationMatrix
    * PCL:AstrometricSolution:SplineWorldTransformation
    * PCL:AstrometricSolution:Catalog
    * PCL:AstrometricSolution:CreationTime
    * PCL:AstrometricSolution:CreatorApplication
    * PCL:AstrometricSolution:CreatorOS
    * PCL:AstrometricSolution:CreatorModule
    * </pre>
    */
   void UpdateProperties( PropertyArray& properties ) const;

   /*!
    * Undefines the optional metadata items for observation time (start and end
    * times) and geodetic coordinates (longitude, latitude and height).
    *
    * This function is useful to propagate the astrometric solution to other
    * images, for example during an image registration task, without altering
    * image-specific observational properties.
    */
   void UndefineTimeAndLocationMetadata()
   {
      m_obsStartTime.Undefine();
      m_obsEndTime.Undefine();
      m_geoLongitude.Undefine();
      m_geoLatitude.Undefine();
      m_geoHeight.Undefine();
   }

   /*!
    * Removes astrometry-related FITS header keywords from the specified
    * \a keywords array. This includes some basic instrumental and
    * observational keywords, as well as the following keywords:
    *
    * <pre>
    * RADESYS
    * EQUINOX
    * EPOCH
    * CTYPE1
    * CTYPE2
    * CRVAL1
    * CRVAL2
    * CRPIX1
    * CRPIX2
    * CD1_1
    * CD1_2
    * CD2_1
    * CD2_2
    * PC1_1
    * PC1_2
    * PC2_1
    * PC2_2
    * CDELT1
    * CDELT2
    * CROTA1
    * CROTA2
    * PV1_1
    * PV1_2
    * PV1_3
    * PV1_4
    * LONPOLE
    * LATPOLE
    * </pre>
    *
    * If \a removeCenterKeywords is true, the following keywords will also be
    * removed:
    *
    * <pre>
    * RA
    * OBJCTRA
    * DEC
    * OBJCTDEC
    * </pre>
    *
    * If \a removeScaleKeywords is true, the following keywords will also be
    * removed:
    *
    * <pre>
    * FOCALLEN
    * XPIXSZ
    * YPIXSZ
    * PIXSIZE
    * </pre>
    */
   static void RemoveKeywords( FITSKeywordArray& keywords, bool removeCenterKeywords = true, bool removeScaleKeywords = true );

   /*!
    * Rescales the values of existing FITS header keywords defining physical
    * pixel dimensions in the specified \a keywords array.
    *
    * This function multiplies the values of the following keywords by the
    * specified \a scalingFactor:
    *
    * <pre>
    * XPIXSZ
    * YPIXSZ
    * PIXSIZE
    * </pre>
    *
    * This is useful in cases where a geometric transformation has to be
    * applied to an image to modify its scale, such as a resampling operation.
    */
   static void RescalePixelSizeKeywords( FITSKeywordArray& keywords, double scalingFactor );

   /*!
    * Removes astrometry-related XISF properties from the specified
    * \a properties array.
    *
    * If \a removeCenterProperties is true, the following properties will also
    * be removed:
    *
    * <pre>
    * Observation:Center:RA
    * Observation:Center:Dec
    * Observation:Center:X
    * Observation:Center:Y
    * Observation:CelestialReferenceSystem
    * Observation:Equinox
    * </pre>
    *
    * If \a removeScaleProperties is true, the following properties will also
    * be removed:
    *
    * <pre>
    * Instrument:Telescope:FocalLength
    * Instrument:Sensor:XPixelSize
    * Instrument:Sensor:YPixelSize
    * </pre>
    *
    * The following nonstandard properties will always be removed:
    *
    * <pre>
    * PCL:AstrometricSolution:ProjectionSystem
    * PCL:AstrometricSolution:ReferenceCelestialCoordinates
    * PCL:AstrometricSolution:ReferenceImageCoordinates
    * PCL:AstrometricSolution:ReferenceNativeCoordinates
    * PCL:AstrometricSolution:CelestialPoleNativeCoordinates
    * PCL:AstrometricSolution:LinearTransformationMatrix
    * PCL:AstrometricSolution:SplineWorldTransformation
    * PCL:AstrometricSolution:Catalog
    * PCL:AstrometricSolution:CreationTime
    * PCL:AstrometricSolution:CreatorApplication
    * PCL:AstrometricSolution:CreatorOS
    * PCL:AstrometricSolution:CreatorModule
    * </pre>
    */
   static void RemoveProperties( PropertyArray& properties, bool removeCenterProperties = true, bool removeScaleProperties = true );

   /*!
    * Removes astrometry-related XISF properties from the specified \a window's
    * main view.
    *
    * See RemoveProperties( PropertyArray&, bool, bool ) for detailed
    * information.
    */
   static void RemoveProperties( ImageWindow& window, bool removeCenterProperties = true, bool removeScaleProperties = true );

   /*!
    * Rescales the values of existing XISF properties defining physical pixel
    * dimensions in the specified \a properties array.
    *
    * This function multiplies the values of the following properties by the
    * specified \a scalingFactor:
    *
    * <pre>
    * Instrument:Sensor:XPixelSize
    * Instrument:Sensor:YPixelSize
    * </pre>
    *
    * This is useful in cases where a geometric transformation has to be
    * applied to an image to modify its scale, such as a resampling operation.
    */
   static void RescalePixelSizeProperties( PropertyArray& properties, double scalingFactor );

   /*!
    * Rescales the values of existing XISF properties defining physical pixel
    * dimensions for the specified \a window's main view.
    *
    * See RescalePixelSizeProperties( PropertyArray&, double ) for detailed
    * information.
    */
   static void RescalePixelSizeProperties( ImageWindow& window, double scalingFactor );

   /*!
    * Returns a printable textual representation of the metadata properties and
    * parameters of the astrometric solution represented by this object.
    */
   String Summary() const;

   /*!
    * Returns a collection of strings describing the metadata properties and
    * parameters of this astrometric solution.
    *
    * The returned object is a copy of an internal structure that is generated
    * automatically as necessary and cached between successive function calls.
    * This allows for efficient real-time representations of astrometric
    * metadata and parameters.
    */
   DescriptionItems Description() const
   {
      UpdateDescription();
      return m_description.IsNull() ? DescriptionItems() : *m_description;
   }

private:

   AutoPointer<ProjectionBase>      m_projection;
   AutoPointer<WorldTransformation> m_transformWI;
   IsoString                        m_refSys;         // ICRS(default) or GCRS
   Optional<double>                 m_equinox;        // years - deprecated
   int                              m_width = 0;      // px
   int                              m_height = 0;     // px
   Optional<double>                 m_pixelSize;      // um
   Optional<TimePoint>              m_obsStartTime;   // UTC
   Optional<TimePoint>              m_obsEndTime;     // UTC
   Optional<double>                 m_geoLongitude;   // deg
   Optional<double>                 m_geoLatitude;    // deg
   Optional<double>                 m_geoHeight;      // m
   double                           m_resolution = 0; // deg/px
   Optional<double>                 m_focalLength;    // mm
   Optional<TimePoint>              m_creationTime;
   String                           m_catalog;
   String                           m_creatorApp;
   String                           m_creatorModule;
   String                           m_creatorOS;
   mutable
   AutoPointer<DescriptionItems>    m_description;

   WCSKeywords ComputeWCSKeywords() const;
   void UpdateDescription() const;
};

} //pcl

#endif // __AstrometricMetadata_h

// ----------------------------------------------------------------------------
// EOF pcl/AstrometricMetadata.h - Released 2023-08-28T15:23:15Z
