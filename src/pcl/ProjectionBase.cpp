//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/ProjectionBase.cpp - Released 2024-12-28T16:53:56Z
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

#include <pcl/ProjectionBase.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

void ProjectionBase::InitFromRefPoint( double lng0, double lat0, const Optional<double>& lonpole )
{
   m_ra0 = Rad( lng0 );
   m_dec0 = Rad( lat0 );

   m_lonpole = lonpole;
   m_latpole = Optional<double>();

   double phip;
   if ( lonpole.IsDefined() )
      phip = lonpole();
   else
   {
      // Default value for the native longitude of the celestial pole.
      phip = ((lat0 < m_theta0) ? 180 : 0) + m_phi0;
      if ( phip < -180 )
         phip += 360;
      else if ( phip > 180 )
         phip -= 360;
   }

   m_sph = SphericalRotation( lng0, lat0, m_phi0, m_theta0, phip, m_latpole );
}

// ----------------------------------------------------------------------------

void ProjectionBase::InitFromWCS( const WCSKeywords& wcs )
{
   m_ra0 = Rad( wcs.crval1.OrElseThrow( "Invalid WCS metadata: Undefined CRVAL1 field" ) );
   m_dec0 = Rad( wcs.crval2.OrElseThrow( "Invalid WCS metadata: Undefined CRVAL2 field" ) );

   if ( wcs.pv1_1.IsDefined() )
      m_phi0 = wcs.pv1_1();

   if ( wcs.pv1_2.IsDefined() )
   {
      m_theta0 = wcs.pv1_2();
      if ( Abs( m_theta0 ) > 90 )
      {
         if ( Abs( m_theta0 ) > 90 + SphericalRotation::Tolerance )
            throw Error( "Invalid WCS coordinates: theta0 > 90 deg" );
         m_theta0 = (m_theta0 > 90) ? 90 : -90;
      }
   }

   m_latpole = wcs.latpole;
   m_lonpole = wcs.lonpole;

   double phip;
   if ( wcs.lonpole.IsDefined() )
      phip = wcs.lonpole();
   else
   {
      // Default value for the native longitude of the celestial pole.
      phip = ((Deg( m_dec0 ) < m_theta0) ? 180 : 0) + m_phi0;
      if ( phip < -180 )
         phip += 360;
      else if ( phip > 180 )
         phip -= 360;
   }

   m_sph = SphericalRotation( wcs.crval1, wcs.crval2, m_phi0, m_theta0, phip, m_latpole );
}

// ----------------------------------------------------------------------------

void ProjectionBase::GetWCS( WCSKeywords& wcs ) const
{
   wcs.ctype1 = "'RA---" + ProjCode() + '\'';
   wcs.ctype2 = "'DEC--" + ProjCode() + '\'';
   wcs.crval1 = Deg( m_ra0  );
   wcs.crval2 = Deg( m_dec0 );
   wcs.pv1_1 = m_phi0;
   wcs.pv1_2 = m_theta0;
   wcs.lonpole = m_lonpole;
   wcs.latpole = m_latpole;
}

// ----------------------------------------------------------------------------

IsoString ProjectionBase::WCSCodeToProjectionIdentifier( const IsoString& wcsCode )
{
   switch ( wcsCode.Trimmed().CaseFolded().Hash32() )
   {
   case 0x5e1ebd44: // TAN
      return "Gnomonic";
   case 0x1cafa6e6: // STG
      return "Stereographic";
   case 0x9f2b1ebc: // CAR
      return "PlateCarree";
   case 0xecd36fef: // MER
      return "Mercator";
   case 0xaa395586: // AIT
      return "HammerAitoff";
   case 0x97c16049: // ZEA
      return "ZenithalEqualArea";
   case 0x6e069940: // SIN
      return "Orthographic";
   default:
      throw Error( "ProjectionBase: Invalid/unsupported WCS projection code \'" + wcsCode + '\'' );
   }
}

// ----------------------------------------------------------------------------

IsoString ProjectionBase::ProjectionIdentifierToWCSCode( const IsoString& identifier )
{
   switch ( identifier.Trimmed().Hash32() )
   {
   case 0x4418268d: // Gnomonic
      return "TAN";
   case 0xc92ad8b1: // Stereographic
      return "STG";
   case 0x43a51256: // PlateCarree
      return "CAR";
   case 0x2edafe59: // Mercator
      return "MER";
   case 0x50df063f: // HammerAitoff
      return "AIT";
   case 0xbbb316b7: // ZenithalEqualArea
      return "ZEA";
   case 0xe5ca11ab: // Orthographic
      return "SIN";
   default:
      throw Error( "ProjectionBase: Invalid/unsupported projection identifier \'" + identifier + '\'' );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ProjectionBase.cpp - Released 2024-12-28T16:53:56Z
