//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// Elements.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
//
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __Elements_h
#define __Elements_h

#include <pcl/TimePoint.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Classical orbital elements - vector indexes
 */
#define a_idx  0
#define q_idx  1
#define e_idx  2
#define M_idx  3
#define T_idx  4
#define i_idx  5
#define O_idx  6
#define w_idx  7

/*
 * Criterion for near-parabolic orbit identification. This has to be consistent
 * across all classes and functions defined in this module.
 *
 * For parabolic and near-parabolic orbits, we expect to have the perihelion
 * distance 'q' instead of the semimajor axis 'a' in all elements vectors.
 */
inline bool NearParabolic( const Vector& el )
{
   return Abs( 1 - el[e_idx] ) < 0.001;
}

/*
 * Mean motion.
 *
 * el       Vector of orbital elements (a, q, e, M, T, i, Omega, omega)
 *
 * GMS      Mass parameter of the Sun (au^3/day^2). If not specified or <= 0, a
 *          default value from current fundamental ephemerides will be used.
 *
 * m        Mass of the body in solar mass units.
 *
 * Returns the mean motion in radians, computed from the semimajor axis or the
 * perihelion distance (for near-parabolic motion).
 */
double MeanMotion( const Vector& el, double GMS = 0, double m = 0 );

/*
 * The mean anomaly from given time of perihelion passage (T) and epoch.
 *
 * el       Vector of orbital elements (a, q, e, M, T, i, O, w)
 *
 * t        Epoch of osculating elements (JD, TDB).
 *
 * GMS      Mass parameter of the Sun (au^3/day^2). If not specified or <= 0, a
 *          default value from current fundamental ephemerides will be used.
 *
 * m        Mass of the body in solar mass units.
 *
 * Returns the mean anomaly in degrees, in the range (-360,+360).
 */
double MeanAnomalyFromTimeOfPerihelionPassage( const Vector& el, double t, double GMS = 0, double m = 0 );

/*
 * Time of perihelion passage.
 *
 * el       Vector of orbital elements (a, q, e, M, T, i, Omega, omega)
 *
 * t        Epoch of osculating elements (JD, TDB).
 *
 * GMS      Mass parameter of the Sun (au^3/day^2). If not specified or <= 0, a
 *          default value from current fundamental ephemerides will be used.
 *
 * m        Mass of the body in solar mass units.
 *
 * Returns the time of perihelion passage (T) as a JD in TDB.
 */
double TimeOfPerihelionPassageFromMeanAnomaly( const Vector& el, double t, double GMS = 0, double m = 0 );

/*
 * Perihelion distance (au) from semimajor axis and eccentricity.
 */
double PerihelionDistanceFromSemimajorAxis( const Vector& el );

/*
 * Semimajor axis (au) from perihelion distance and eccentricity.
 */
double SemimajorAxisFromPerihelionDistance( const Vector& el );

/*
 * Position and velocity vectors from orbital elements.
 *
 * r[out]   Position vector (au)
 *
 * v[out]   Velocity vector (au/day)
 *
 * el       Vector of orbital elements (a, q, e, M, T, i, Omega, omega)
 *
 * t        Epoch of osculating elements (JD, TDB).
 *
 * GMS      Mass parameter of the Sun (au^3/day^2). If not specified or <= 0, a
 *          default value from current fundamental ephemerides will be used.
 *
 * m        Mass of the body in solar mass units.
 */
void StateVectorsFromOrbitalElements( Vector& r, Vector& v, const Vector& el, double t, double GMS = 0, double m = 0 );

/*
 * Orbital elements from position and velocity vectors.
 *
 * el[out]  Vector of orbital elements (a, q, e, M, T, i, Omega, omega)
 *
 * r        Position vector (au)
 *
 * v        Velocity vector (au/day)
 *
 * t        Epoch of state vectors (JD, TDB).
 *
 * GMS      Mass parameter of the Sun (au^3/day^2). If not specified or <= 0, a
 *          default value from current fundamental ephemerides will be used.
 *
 * m        Mass of the body in solar mass units.
 */
void OrbitalElementsFromStateVectors( Vector& el, const Vector& r, const Vector& v, double t, double GMS = 0, double m = 0 );

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Elements_h

// ----------------------------------------------------------------------------
// EOF Elements.h - Released 2021-04-09T19:41:48Z
