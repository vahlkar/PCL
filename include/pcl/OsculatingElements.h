//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/OsculatingElements.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_OsculatingElements_h
#define __PCL_OsculatingElements_h

/// \file pcl/OsculatingElements.h

#include <pcl/Defs.h>

#include <pcl/TimePoint.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class OsculatingElements
 * \brief Osculating conic orbital elements.
 *
 * This class represents a set of osculating conic orbital elements that define
 * the size and orientation of an orbit in space at a given epoch.
 *
 * The length and time units are implicitly defined by the gravitational mass
 * parameters of the central body used upon construction from state vectors
 * (position and velocity vectors). If the default parameters are used, a
 * heliocentric orbit is implicitly assumed where the units of length and time
 * are the astronomical unit (au) and the day of 86400 SI seconds in the TDB
 * timescale, respectively. All data members representing angular elements
 * (mean and true anomalies, inclination, longitude of the ascending node and
 * argument of periapsis) are stored in radians.
 *
 * The orbital elements describe the orbit of a celestial body about a
 * dynamical center located at the origin of an intertial rectangular
 * coordinate system. The same elements are used to describe the three types of
 * conic orbits: elliptic, hyperbolic, and (near-)parabolic.
 *
 * \ingroup solar_system_ephemerides
 */
class PCL_CLASS OsculatingElements
{
public:

   double    a = 0.0; //!< Semimajor axis. Unused for (near-)parabolic orbits.
   double    q = 0.0; //!< Perifocal distance. Required for (near-)parabolic orbits.
   double    e = 0.0; //!< Eccentricity.
   double    M = 0.0; //!< Mean anomaly (radians).
   TimePoint T = 0.0; //!< Time of perifocal passage. Undefined for circular orbits.
   double    i = 0.0; //!< Inclination over the reference plane (radians).
   double    O = 0.0; //!< Longitude of the ascending node (radians). Undefined if i = 0.
   double    w = 0.0; //!< Argument of periapsis (radians). Undefined for circular orbits or when i = 0.

   /*!
    * Default constructor. Constructs an uninitialized object where all orbital
    * elements are set to zero.
    */
   OsculatingElements() = default;

   /*!
    * Copy constructor.
    */
   OsculatingElements( const OsculatingElements& ) = default;

   /*!
    * Constructor from state vectors.
    *
    * \param r    Position vector.
    *
    * \param v    Velocity vector.
    *
    * \param t    Epoch of state vectors (TDB).
    *
    * \param GM   Gravitational mass parameter of the central body in units of
    *             length^3/time^2. If not specified or &le; 0, a default value
    *             for the Sun in au^3/day^2 units, consistent with current
    *             fundamental ephemerides, will be used.
    *
    * \param m    Mass of the body in central body mass units. Zero by default.
    *
    * \note The calculated orbital elements will describe an orbit in the same
    * inertial rectangular coordinate system where the specified \a r and \a v
    * vectors are defined. For example, if the state vectors provide
    * heliocentric ecliptic rectangular coordiantes, the orbit will be referred
    * to the same fundamental plane (ecliptic) and origin (the Sun).
    */
   OsculatingElements( const Vector& r, const Vector& v, const TimePoint& t, double GM = 0, double m = 0 );

   /*!
    * Returns a new %OsculatingElements object with orbital elements calculated
    * from the specified state vectors.
    *
    * \param r    Position vector.
    *
    * \param v    Velocity vector.
    *
    * \param t    Epoch of state vectors (TDB).
    *
    * \param GM   Gravitational mass parameter of the central body in units of
    *             length^3/time^2. If not specified or &le; 0, a default value
    *             for the Sun in au^3/day^2 units, consistent with current
    *             fundamental ephemerides, will be used.
    *
    * \param m    Mass of the body in central body mass units. Zero by default.
    *
    * \note The calculated orbital elements will describe an orbit in the same
    * inertial rectangular coordinate system where the specified \a r and \a v
    * vectors are defined. For example, if the state vectors provide
    * heliocentric ecliptic rectangular coordiantes, the orbit will be referred
    * to the same fundamental plane (ecliptic) and origin (the Sun).
    */
   static OsculatingElements FromStateVectors( const Vector& r, const Vector& v, const TimePoint& t, double GM = 0, double m = 0 )
   {
      return OsculatingElements( r, v, t, GM, m );
   }

   /*!
    * Computes position and velocity vectors from conic orbital elements.
    *
    * \param[out] r  Position vector.
    *
    * \param[out] v  Velocity vector.
    *
    * \param t       Epoch of state vectors (TDB).
    *
    * \param GM      Gravitational mass parameter of the central body in units
    *                of length^3/time^2. If not specified or &le; 0, a default
    *                value for the Sun in au^3/day^2 units, consistent with
    *                current fundamental ephemerides, will be used.
    *
    * \param m       Mass of the body in central body mass units. Zero by
    *                default.
    *
    * \note The output \a r and \a v vectors will be referred to the same
    * inertial rectangular coordinate system where orbital elements are
    * defined. For example, in the case of a heliocentric orbit, the state
    * vectors will give heliocentric ecliptic rectangular coordiantes.
    */
   void ToStateVectors( Vector& r, Vector& v, const TimePoint& t, double GM = 0, double m = 0 ) const;

   /*!
    * Returns true iff this object defines a parabolic or near-parabolic orbit.
    *
    * With the current implementation, an orbit is considered near-parabolic if
    * its eccentricity is &gt; 0.999.
    *
    * For parabolic and near-parabolic orbits, we expect to have the perifocal
    * distance \a q instead of the semimajor axis \a a.
    */
   inline bool IsNearParabolic() const
   {
      return Abs( 1 - this->e ) < 0.001;
   }

   /*!
    * Mean orbital motion.
    *
    * \param GM   Gravitational mass parameter of the central body in units of
    *             length^3/time^2. If not specified or &le; 0, a default value
    *             for the Sun in au^3/day^2 units, consistent with current
    *             fundamental ephemerides, will be used.
    *
    * \param m    Mass of the body in central body mass units. Zero by default.
    *
    * Returns the mean motion in radians, computed from the semimajor axis for
    * elliptical and hyperbolic orbits, or from the perifocal distance for
    * near-parabolic orbits.
   */
   double MeanMotion( double GM = 0, double m = 0 ) const;

   /*!
    * Orbital period of the elliptic orbit.
    *
    * \param GM   Gravitational mass parameter of the central body in units of
    *             length^3/time^2. If not specified or &le; 0, a default value
    *             for the Sun in au^3/day^2 units, consistent with current
    *             fundamental ephemerides, will be used.
    *
    * \param m    Mass of the body in central body mass units. Zero by default.
    *
    * Returns the orbital period of an elliptic orbit (e < 1) in time units.
    * For parabolic and hyperbolic orbits, this function returns zero.
    */
   double OrbitalPeriod( double GM = 0, double m = 0 ) const;

   /*!
    * The mean anomaly calculated from the time of perifocal passage.
    *
    * \param t    Epoch of osculating elements, TDB.
    *
    * \param GM   Gravitational mass parameter of the central body in units of
    *             length^3/time^2. If not specified or &le; 0, a default value
    *             for the Sun in au^3/day^2 units, consistent with current
    *             fundamental ephemerides, will be used.
    *
    * \param m    Mass of the body in central body mass units. Zero by default.
    *
    * Returns the mean anomaly in radians, in the range [0,2pi).
    */
   double MeanAnomalyFromTimeOfPerifocalPassage( const TimePoint& t, double GM = 0, double m = 0 ) const
   {
      return Norm2Pi( MeanMotion( GM, m )*(t - this->T) );
   }

   /*!
    * The mean anomaly calculated from the time of perihelion passage.
    *
    * This function is a convenience synonym for
    * MeanAnomalyFromTimeOfPerifocalPassage().
    */
   double MeanAnomalyFromTimeOfPerihelionPassage( const TimePoint& t, double GM = 0, double m = 0 ) const
   {
      return MeanAnomalyFromTimeOfPerifocalPassage( t, GM, m );
   }

   /*!
    * The time of perifocal passage calculated from the mean anomaly.
    *
    * \param t    Epoch of osculating elements, TDB.
    *
    * \param GM   Gravitational mass parameter of the central body in units of
    *             length^3/time^2. If not specified or &le; 0, a default value
    *             for the Sun in au^3/day^2 units, consistent with current
    *             fundamental ephemerides, will be used.
    *
    * \param m    Mass of the body in central body mass units. Zero by default.
    *
    * Returns the time of perifocal passage in the TDB timescale.
    */
   TimePoint TimeOfPerifocalPassageFromMeanAnomaly( const TimePoint& t, double GM = 0, double m = 0 ) const
   {
      return t - this->M/MeanMotion( GM, m );
   }

   /*!
    * The time of perihelion passage calculated from the mean anomaly.
    *
    * This function is a convenience synonym for
    * TimeOfPerifocalPassageFromMeanAnomaly().
    */
   TimePoint TimeOfPerihelionPassageFromMeanAnomaly( const TimePoint& t, double GM = 0, double m = 0 ) const
   {
      return TimeOfPerifocalPassageFromMeanAnomaly( t, GM, m );
   }

   /*!
    * The perifocal distance calculated from semimajor axis and eccentricity.
    */
   double PerifocalDistanceFromSemimajorAxis() const
   {
      return (1 - this->e)*this->a;
   }

   /*!
    * The perihelion distance calculated from semimajor axis and eccentricity.
    *
    * This function is a convenience synonym for
    * PerifocalDistanceFromSemimajorAxis().
    */
   double PerihelionDistanceFromSemimajorAxis() const
   {
      return PerifocalDistanceFromSemimajorAxis();
   }

   /*!
    * The semimajor axis calculated from perifocal distance and eccentricity.
    */
   double SemimajorAxisFromPerifocalDistance() const
   {
      double ai = (1 - this->e)/this->q;
      if ( Abs( ai ) < 2*std::numeric_limits<double>::epsilon() )
         return 0;
      return 1/ai;
   }

   /*!
    * The semimajor axis calculated from perihelion distance and eccentricity.
    *
    * This function is a convenience synonym for
    * SemimajorAxisFromPerifocalDistance().
    */
   double SemimajorAxisFromPerihelionDistance() const
   {
      return SemimajorAxisFromPerifocalDistance();
   }

   /*!
    * Position and velocity components in the perifocal reference frame.
    *
    * \param[out] xp,yp    Perifocal position vector components.
    *
    * \param[out] xv,yv    Perifocal velocity vector components.
    *
    * \param t             Epoch of osculating elements (TDB).
    *
    * \param GM            Gravitational mass parameter of the central body in
    *                      units of length^3/time^2. If not specified or
    *                      &le; 0, a default value for the Sun in au^3/day^2
    *                      units, consistent with current fundamental
    *                      ephemerides, will be used.
    *
    * \param m             Mass of the body in central body mass units. Zero by
    *                      default.
    *
    * The output position and velocity components are referred to a
    * right-handed Cartesian coordinate system where the XY plane coincides
    * with the orbit plane and the X axis is aligned with the semimajor axis,
    * positive towards the perifocus. The Z axis points in the same direction
    * as the angular momentum vector. This system is known as the <em>perifocal
    * reference frame.</em>
    */
   void ToPerifocalPositionAndVelocity( double& xp, double& yp, double& xv, double& yv, const TimePoint& t, double GM = 0, double m = 0 ) const;

   /*!
    * Position components in the perifocal reference frame.
    *
    * \param[out] xp,yp    Perifocal position vector components.
    *
    * \param t             Epoch of osculating elements (TDB).
    *
    * \param GM            Gravitational mass parameter of the central body in
    *                      units of length^3/time^2. If not specified or
    *                      &le; 0, a default value for the Sun in au^3/day^2
    *                      units, consistent with current fundamental
    *                      ephemerides, will be used.
    *
    * \param m             Mass of the body in central body mass units. Zero by
    *                      default.
    *
    * The output position components are referred to a right-handed Cartesian
    * coordinate system where the XY plane coincides with the orbit plane and
    * the X axis is aligned with the semimajor axis, positive towards the
    * perifocus. The Z axis points in the same direction as the angular
    * momentum vector. This system is known as the <em>perifocal reference
    * frame.</em>
    */
   void ToPerifocalPosition( double& xp, double& yp, const TimePoint& t, double GM = 0, double m = 0 ) const
   {
      double u1, u2;
      ToPerifocalPositionAndVelocity( xp, yp, u1, u2, t, GM, m );
   }

   /*!
    * Computes the orbit orientation angles from given state vectors.
    *
    * \param[out] i  Inclination over the reference plane (radians).
    *
    * \param[out] O  Longitude of the ascending node (radians).
    *
    * \param[out] w  Argument of periapsis (radians).
    *
    * \param r       Position vector.
    *
    * \param v       Velocity vector.
    *
    * \param GM      Gravitational mass parameter of the central body in units
    *                of length^3/time^2. If not specified or &le; 0, a default
    *                value for the Sun in au^3/day^2 units, consistent with
    *                current fundamental ephemerides, will be used.
    *
    * \param m       Mass of the body in central body mass units. Zero by
    *                default.
    *
    * This function is useful when only the orbit orientation in space is
    * required, saving the calculation of a complete set of orbital elements.
    *
    * \note The output orbital elements will describe the orientation of an
    * orbit in the same inertial rectangular coordinate system where the
    * specified \a r and \a v vectors are defined. For example, if the state
    * vectors provide heliocentric ecliptic rectangular coordiantes, the output
    * orientation angles will be referred to the same fundamental plane
    * (ecliptic) and origin (the Sun).
    */
   static void GetOrbitOrientationFromStateVectors( double& i, double& O, double& w, const Vector& r, const Vector& v, double GM = 0, double m = 0 );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_OsculatingElements_h

// ----------------------------------------------------------------------------
// EOF pcl/OsculatingElements.h - Released 2024-12-28T16:53:48Z
