//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// pcl/SphericalRotation.h - Released 2023-08-28T15:23:15Z
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

#ifndef __PCL_SphericalRotation_h
#define __PCL_SphericalRotation_h

/// \file pcl/SphericalRotation.h

#include <pcl/Defs.h>

#include <pcl/Optional.h>
#include <pcl/Point.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SphericalRotation
 * \brief Rotation in spherical coordinates.
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS SphericalRotation
{
public:

   constexpr static double Tolerance = 1.0e-05;

   /*!
    * Default constructor.
    */
   SphericalRotation() = default;

   /*!
    * Copy constructor.
    */
   SphericalRotation( const SphericalRotation& ) = default;

   /*!
    * Construction with explicit initialization.
    *
    * \param lon0    Celestial longitude of the reference point in degrees.
    *
    * \param lat0    Celestial latitude of the reference point in degrees.
    *
    * \param phi0    Native projection longitude of the reference point in
    *                degrees.
    *
    * \param theta0  Native projection latitude of the reference point in
    *                degrees.
    *
    * \param phip    Native projection longitude of the north celestial pole in
    *                degrees.
    *
    * \param thetap  Native projection latitude of the north celestial pole in
    *                degrees. This parameter is optional. If not specified, 90
    *                degrees will be assumed.
    */
   SphericalRotation( double lon0, double lat0, double phi0, double theta0, double phip,
                      const Optional<double>& thetap );

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SphericalRotation& operator =( const SphericalRotation& ) = default;

   /*!
    * Returns the celestial coordinates corresponding to the specified location
    * in projection native coordinates.
    */
   DPoint NativeToCelestial( const DPoint& np ) const;

   /*!
    * Returns the projection native coordinates corresponding to the specified
    * location in celestial coordinates.
    */
   DPoint CelestialToNative( const DPoint& cp ) const;

private:

   double m_latpole;
   double m_alphaP;
   double m_deltaP;
   double m_phiP;
   double m_cosdeltaP;
   double m_sindeltaP;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SphericalRotation_h

// ----------------------------------------------------------------------------
// EOF pcl/SphericalRotation.h - Released 2023-08-28T15:23:15Z
