//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.9
// ----------------------------------------------------------------------------
// pcl/ProjectionFactory.cpp - Released 2024-03-20T10:41:42Z
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

#include <pcl/GnomonicProjection.h>
#include <pcl/HammerAitoffProjection.h>
#include <pcl/MercatorProjection.h>
#include <pcl/OrthographicProjection.h>
#include <pcl/PlateCarreeProjection.h>
#include <pcl/ProjectionFactory.h>
#include <pcl/ZenithalProjections.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

ProjectionBase* ProjectionFactory::CreateByWCSCode( const IsoString& wcsCode, double ra, double dec )
{
   ProjectionBase* projection = nullptr;
   bool needsInitialization = true;
   switch ( wcsCode.Trimmed().CaseFolded().Hash32() )
   {
   case 0x5e1ebd44: // TAN
      projection = new GnomonicProjection( ra, dec );
      needsInitialization = false;
      break;
   case 0x1cafa6e6: // STG
      projection = new StereographicProjection();
      break;
   case 0x9f2b1ebc: // CAR
      projection = new PlateCarreeProjection();
      break;
   case 0xecd36fef: // MER
      projection = new MercatorProjection();
      break;
   case 0xaa395586: // AIT
      projection = new HammerAitoffProjection();
      break;
   case 0x97c16049: // ZEA
      projection = new ZenithalEqualAreaProjection();
      break;
   case 0x6e069940: // SIN
      projection = new OrthographicProjection();
      break;
   default:
      throw Error( "ProjectionFactory: Invalid/unsupported WCS projection code \'" + wcsCode + '\'' );
   }
   if ( needsInitialization )
      projection->InitFromRefPoint( ra, dec );
   return projection;
}

// ----------------------------------------------------------------------------

ProjectionBase* ProjectionFactory::CreateByIdentifier( const IsoString& identifier, double ra, double dec )
{
   ProjectionBase* projection = nullptr;
   bool needsInitialization = true;
   switch ( identifier.Trimmed().Hash32() )
   {
   case 0x4418268d: // Gnomonic
      projection = new GnomonicProjection( ra, dec );
      needsInitialization = false;
      break;
   case 0xc92ad8b1: // Stereographic
      projection = new StereographicProjection();
      break;
   case 0x43a51256: // PlateCarree
      projection = new PlateCarreeProjection();
      break;
   case 0x2edafe59: // Mercator
      projection = new MercatorProjection();
      break;
   case 0x50df063f: // HammerAitoff
      projection = new HammerAitoffProjection();
      break;
   case 0xbbb316b7: // ZenithalEqualArea
      projection = new ZenithalEqualAreaProjection();
      break;
   case 0xe5ca11ab: // Orthographic
      projection = new OrthographicProjection();
      break;
   default:
      throw Error( "ProjectionFactory: Invalid/unsupported projection identifier \'" + identifier + '\'' );
   }
   if ( needsInitialization )
      projection->InitFromRefPoint( ra, dec );
   return projection;
}

// ----------------------------------------------------------------------------

ProjectionBase* ProjectionFactory::Create( const WCSKeywords& wcs )
{
   IsoString ptype1 = wcs.ctype1.Substring( 5, 3 );
   IsoString ptype2 = wcs.ctype2.Substring( 5, 3 );
   if ( ptype1 != ptype2 )
      throw Error( "ProjectionFactory::Create(): Unsupported WCS coordinates: axes with different projections." );
   return CreateByWCSCode( ptype1, wcs.crval1, wcs.crval2 );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ProjectionFactory.cpp - Released 2024-03-20T10:41:42Z
