//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// pcl/ProjectionBase.h - Released 2024-06-18T15:48:54Z
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

#ifndef __PCL_ProjectionBase_h
#define __PCL_ProjectionBase_h

/// \file pcl/ProjectionBase.h

#include <pcl/Defs.h>

#include <pcl/Optional.h>
#include <pcl/Point.h>
#include <pcl/SphericalRotation.h>
#include <pcl/WCSKeywords.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ProjectionBase
 * \brief Base class of all projection systems
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS ProjectionBase
{
public:

   /*!
    * Virtual destructor.
    */
   virtual ~ProjectionBase()
   {
   }

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   virtual ProjectionBase* Clone() const = 0;

   /*!
    * Returns the FITS WCS projection identifier for this projection system.
    */
   virtual IsoString ProjCode() const = 0;

   /*!
    * Returns the XISF serializable projection identifier.
    */
   virtual IsoString Identifier() const = 0;

   /*!
    * Returns the readable name of this projection system.
    */
   virtual IsoString Name() const = 0;

   /*!
    * Updates the specified WCS metadata with field values corresponding to
    * this projection.
    */
   virtual void GetWCS( WCSKeywords& wcs ) const;

   /*!
    * Transforms from celestial coordinates to world coordinates.
    */
   virtual bool Direct( DPoint& pW, const DPoint& pRD ) const noexcept
   {
      DPoint pN = m_sph.CelestialToNative( pRD );
      return IsFinite( pN.x ) && IsFinite( pN.y ) && Project( pW, pN );
   }

   /*!
    * Transforms from world coordinates to celestial coordinates.
    */
   virtual bool Inverse( DPoint& pRD, const DPoint& pW ) const noexcept
   {
      DPoint pN;
      if ( Unproject( pN, pW ) )
         if ( IsFinite( pN.x ) )
            if ( IsFinite( pN.y ) )
            {
               pRD = m_sph.NativeToCelestial( pN );
               return true;
            }
      return false;
   }

   /*!
    *
    */
   virtual bool CheckBrokenLine( const DPoint& cp1, const DPoint& cp2 ) const noexcept
   {
      return DistanceFast( m_sph.CelestialToNative( cp1 ), m_sph.CelestialToNative( cp2 ) ) < 150;
   }

   /*!
    * Returns the equatorial coordinates of the projection origin.
    */
   virtual DPoint ProjectionOrigin() const
   {
      return DPoint( Deg( m_ra0 ), Deg( m_dec0 ) );
   }

   /*!
    * Returns the distance in degrees between two locations in equatorial
    * coordinates.
    */
   static double Distance( const DPoint& p1, const DPoint& p2 )
   {
      double sinX, cosX;
      SinCos( Rad( Abs( p1.x - p2.x ) ), sinX, cosX );
      double sinY1, cosY1;
      SinCos( Rad( p1.y ), sinY1, cosY1 );
      double sinY2, cosY2;
      SinCos( Rad( p2.y ), sinY2, cosY2 );
      double K = cosY1*sinY2 - sinY1*cosY2*cosX;
      return Deg( ArcTan( Sqrt( cosY2*sinX*cosY2*sinX + K*K ), sinY1*sinY2 + cosY1*cosY2*cosX ) );
   };

   /*!
    * Returns the distance in degrees between two locations in equatorial
    * coordinates. Uses an approximate, faster algorithm.
    */
   static double DistanceFast( const DPoint& p1, const DPoint& p2 )
   {
      double sinY1, cosY1;
      SinCos( Rad( p1.y ), sinY1, cosY1 );
      double sinY2, cosY2;
      SinCos( Rad( p2.y ), sinY2, cosY2 );
      return Deg( ArcCos( sinY1*sinY2 + cosY1*cosY2*Cos( Rad( p1.x - p2.x ) ) ) );
   }

   /*!
    * Returns the celestial equatorial coordinates (right ascension and
    * declination in degrees) of the reference point of this projection system.
    *
    * The returned value is a vector suitable for XISF property serialization.
    */
   Vector ReferenceCelestialCoordinates() const
   {
      return Vector{ Deg( m_ra0  ), Deg( m_dec0 ) };
   }

   /*!
    * Returns the native coordinates (longitude and latitude in degrees) of the
    * reference point of this projection system.
    *
    * The returned value is a vector suitable for XISF property serialization.
    */
   Vector ReferenceNativeCoordinates() const
   {
      return Vector{ m_phi0, m_theta0 };
   }

   /*!
    * Returns the native coordinates (longitude and latitude in degrees) of the
    * north celestial pole in this projection system.
    *
    * The returned value is a vector suitable for XISF property serialization.
    */
   Vector CelestialPoleNativeCoordinates() const
   {
      double lonpole;
      if ( m_lonpole.IsDefined() )
         lonpole = m_lonpole();
      else
      {
         // Default value for the native longitude of the celestial pole.
         lonpole = ((Deg( m_dec0 ) < m_theta0) ? 180 : 0) + m_phi0;
         if ( lonpole < -180 )
            lonpole += 360;
         else if ( lonpole > 180 )
            lonpole -= 360;
      }
      return Vector{ lonpole, m_latpole.OrElse( 90.0 ) };
   }

   /*!
    * Returns the XISF projection identifier corresponding to the specified
    * FITS WCS projection code.
    */
   static IsoString WCSCodeToProjectionIdentifier( const IsoString& wcsCode );

   /*!
    * Returns the standard FITS WCS projection code corresponding to the
    * specified XISF projection identifier.
    */
   static IsoString ProjectionIdentifierToWCSCode( const IsoString& identifier );

protected:

   double            m_ra0 = 0;    // celestial longitude of the reference point (radians)
   double            m_dec0 = 0;   // celestial latitude of the reference point (radians)
   double            m_phi0 = 0;   // native longitude of the reference point (degrees)
   double            m_theta0 = 0; // native latitude of the reference point (degrees)
   Optional<double>  m_lonpole;    // native longitude of the celestial pole (degrees)
   Optional<double>  m_latpole;    // native latitude of the celestial pole (degrees)
   SphericalRotation m_sph;

   /*!
    * Default constructor.
    */
   ProjectionBase() = default;

   /*!
    * Copy constructor.
    */
   ProjectionBase( const ProjectionBase& ) = default;

   /*!
    * Initialization from reference celestial coordinates and optional native
    * longitude of the celestial pole.
    */
   void InitFromRefPoint( double lng0, double lat0, const Optional<double>& lonpole = Optional<double>() );

   /*!
    * Initialization from WCS metadata.
    */
   void InitFromWCS( const WCSKeywords& );

   /*!
    * Transforms from world coordinates to native spherical coordinates.
    */
   virtual bool Project( DPoint& pW, const DPoint& pN ) const noexcept = 0;

   /*!
    * Transforms from native spherical coordinates to world coordinates.
    */
   virtual bool Unproject( DPoint& pN, const DPoint& pW ) const noexcept = 0;

   friend class ProjectionFactory;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ProjectionBase_h

// ----------------------------------------------------------------------------
// EOF pcl/ProjectionBase.h - Released 2024-06-18T15:48:54Z
