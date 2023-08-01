//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.7
// ----------------------------------------------------------------------------
// pcl/WCSKeywords.cpp - Released 2023-08-01T16:29:57Z
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

#include <pcl/LinearTransformation.h>
#include <pcl/ProjectionBase.h>
#include <pcl/TimePoint.h>
#include <pcl/WCSKeywords.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

void WCSKeywords::Read( const PropertyArray& properties, const FITSKeywordArray& keywords )
{
   Optional<double> expTime; // only if Observation:Time:End is not available

   /*
    * XISF properties take precedence over FITS keywords.
    */
   for ( const Property& property : properties )
   {
      if ( property.Id() == "Observation:Center:RA" )
         objctra = property.Value().ToDouble();
      else if ( property.Id() == "Observation:Center:Dec" )
         objctdec = property.Value().ToDouble();
      else if ( property.Id() == "Observation:CelestialReferenceSystem" )
         radesys = property.Value().ToIsoString();
      else if ( property.Id() == "Observation:Equinox" )
         equinox = property.Value().ToDouble();
      else if ( property.Id() == "Observation:Time:Start" )
         dateobs = property.Value().ToTimePoint().JD();
      else if ( property.Id() == "Observation:Time:End" )
         dateend = property.Value().ToTimePoint().JD();
      else if ( property.Id() == "Observation:Location:Longitude" )
         longobs = property.Value().ToDouble();
      else if ( property.Id() == "Observation:Location:Latitude" )
         latobs = property.Value().ToDouble();
      else if ( property.Id() == "Observation:Location:Elevation" )
         altobs = property.Value().ToDouble();
      else if ( property.Id() == "Instrument:Telescope:FocalLength" )
         focallen = property.Value().ToDouble() * 1000;
      else if ( property.Id() == "Instrument:Sensor:XPixelSize" )
         xpixsz = property.Value().ToDouble();
      else if ( property.Id() == "Instrument:ExposureTime" )
         expTime = property.Value().ToDouble();

      /*
       * Since core version 1.8.9-2
       * ### TODO: When defined by the XISF standard, remove the PCL prefix.
       */
      else if ( property.Id() == "PCL:AstrometricSolution:ProjectionSystem" )
      {
         IsoString wcsCode = ProjectionBase::ProjectionIdentifierToWCSCode( property.Value().ToIsoString() );
         if ( !wcsCode.IsEmpty() )
         {
            ctype1 = "RA---" + wcsCode;
            ctype2 = "DEC--" + wcsCode;
         }
      }
      else if ( property.Id() == "PCL:AstrometricSolution:ReferenceCelestialCoordinates" )
      {
         Vector p = property.Value().ToVector();
         if ( p.Length() == 2 )
         {
            crval1 = p[0];
            crval2 = p[1];
         }
      }
      else if ( property.Id() == "PCL:AstrometricSolution:ReferenceImageCoordinates" )
      {
         Vector p = property.Value().ToVector();
         if ( p.Length() == 2 )
         {
            crpix1 = p[0];
            crpix2 = p[1];
         }
      }
      else if ( property.Id() == "PCL:AstrometricSolution:ReferenceNativeCoordinates" )
      {
         Vector p = property.Value().ToVector();
         if ( p.Length() == 2 )
         {
            pv1_1 = p[0];
            pv1_2 = p[1];
         }
      }
      else if ( property.Id() == "PCL:AstrometricSolution:CelestialPoleNativeCoordinates" )
      {
         Vector p = property.Value().ToVector();
         if ( p.Length() == 2 )
         {
            lonpole = p[0];
            latpole = p[1];
         }
      }
      else if ( property.Id() == "PCL:AstrometricSolution:LinearTransformationMatrix" )
      {
         Matrix L = property.Value().ToMatrix();
         if ( L.Rows() == 2 && L.Columns() == 2 )
         {
            cd1_1 = L[0][0];
            cd1_2 = L[0][1];
            cd2_1 = L[1][0];
            cd2_2 = L[1][1];
         }
      }
   }

   m_fromFITS = !cd1_1.IsDefined();

   /*
    * Standard WCS FITS keywords.
    */
   for ( const FITSHeaderKeyword& keyword : keywords )
   {
      IsoString svalue = keyword.StripValueDelimiters();
      double nvalue;
      if ( ctype1.IsEmpty() && keyword.name == "CTYPE1" )
      {
         ctype1 = svalue;
      }
      else if ( ctype2.IsEmpty() && keyword.name == "CTYPE2" )
      {
         ctype2 = svalue;
      }
      else if ( !crval1.IsDefined() && keyword.name == "CRVAL1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crval1 = nvalue;
      }
      else if ( !crval2.IsDefined() && keyword.name == "CRVAL2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crval2 = nvalue;
      }
      else if ( !crpix1.IsDefined() && keyword.name == "CRPIX1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crpix1 = nvalue;
      }
      else if ( !crpix2.IsDefined() && keyword.name == "CRPIX2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crpix2 = nvalue;
      }
      else if ( !cd1_1.IsDefined() && keyword.name == "CD1_1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cd1_1 = nvalue;
      }
      else if ( !cd1_2.IsDefined() && keyword.name == "CD1_2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cd1_2 = nvalue;
      }
      else if ( !cd2_1.IsDefined() && keyword.name == "CD2_1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cd2_1 = nvalue;
      }
      else if ( !cd2_2.IsDefined() && keyword.name == "CD2_2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cd2_2 = nvalue;
      }
      else if ( keyword.name == "CDELT1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cdelt1 = nvalue;
      }
      else if ( keyword.name == "CDELT2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cdelt2 = nvalue;
      }
      else if ( keyword.name == "CROTA1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crota1 = nvalue;
      }
      else if ( keyword.name == "CROTA2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crota2 = nvalue;
      }
      else if ( !pv1_1.IsDefined() && keyword.name == "PV1_1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            pv1_1 = nvalue;
      }
      else if ( !pv1_2.IsDefined() && keyword.name == "PV1_2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            pv1_2 = nvalue;
      }
      else if ( !lonpole.IsDefined() && (keyword.name == "PV1_3" || keyword.name == "LONPOLE") )
      {
         if ( svalue.TryToDouble( nvalue ) )
            lonpole = nvalue;
      }
      else if ( !latpole.IsDefined() && (keyword.name == "PV1_4" || keyword.name == "LATPOLE") )
      {
         if ( svalue.TryToDouble( nvalue ) )
            latpole = nvalue;
      }
   }

   /*
    * Primary optional FITS keywords.
    */
   for ( const FITSHeaderKeyword& keyword : keywords )
   {
      IsoString svalue = keyword.StripValueDelimiters();
      double nvalue;
      if ( !objctra.IsDefined() && keyword.name == "RA" )
      {
         if ( svalue.TrySexagesimalToDouble( nvalue, Array<char>() << ' ' << ':' ) )
            if ( nvalue >= 0 )
            {
               /*
                * The RA keyword value can be either a complex angular
                * representation in hours (hh mm ss.sss) or a scalar in degrees
                * ([+|-]ddd.dddddd).
                */
               if ( svalue.Contains( ' ' ) || svalue.Contains( ':' ) )
                  nvalue *= 15;
               if ( nvalue <= 360 )
               {
                  if ( nvalue == 360 )
                     nvalue = 0;
                  objctra = nvalue;
               }
            }
      }
      else if ( !objctdec.IsDefined() && keyword.name == "DEC" )
      {
         /*
          * The DEC keyword value can be either a complex angular
          * representation in degrees ([+|-]dd mm ss.sss) or a scalar
          * ([+|-]ddd.dddddd), also in degrees.
          */
         if ( svalue.TrySexagesimalToDouble( nvalue, Array<char>() << ' ' << ':' ) )
            if ( nvalue >= -90 )
               if ( nvalue <= +90 )
                  objctdec = nvalue;
      }
      else if ( radesys.IsEmpty() && keyword.name == "RADESYS" )
         radesys = svalue.Uppercase();
      else if ( !equinox.IsDefined() && keyword.name == "EQUINOX" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            equinox = nvalue;
      }
      else if ( !dateobs.IsDefined() && keyword.name == "DATE-BEG" )
      {
         TimePoint t;
         if ( TimePoint::TryFromString( t, svalue ) )
            dateobs = t.JD();
      }
      else if ( !dateend.IsDefined() && keyword.name == "DATE-END" )
      {
         TimePoint t;
         if ( TimePoint::TryFromString( t, svalue ) )
            dateend = t.JD();
      }
      else if ( !longobs.IsDefined() && keyword.name == "OBSGEO-L" )
      {
         /*
          * The OBSGEO-L keyword value can be either a complex angular
          * representation in degrees ([+|-]ddd mm ss.sss) or a scalar in
          * degrees ([+|-]ddd.dddddd), positive East.
          */
         if ( svalue.TrySexagesimalToDouble( nvalue, Array<char>() << ' ' << ':' ) )
         {
            if ( nvalue > 180 )
               nvalue -= 360;
            else if ( nvalue <= -180 )
               nvalue += 360;
            if ( nvalue >= -180 )
               if ( nvalue <= 180 )
                  longobs = nvalue;
         }
      }
      else if ( !latobs.IsDefined() && keyword.name == "OBSGEO-B" )
      {
         /*
          * The OBSGEO-B keyword value can be either a complex angular
          * representation in degrees ([+|-]dd mm ss.sss) or a scalar in
          * degrees ([+|-]dd.dddddd), positive North.
          */
         if ( svalue.TrySexagesimalToDouble( nvalue, Array<char>() << ' ' << ':' ) )
            if ( nvalue >= -90 )
               if ( nvalue <= +90 )
                  latobs = nvalue;
      }
      else if ( !altobs.IsDefined() && keyword.name == "OBSGEO-H" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            altobs = nvalue;
      }
      else if ( !focallen.IsDefined() && keyword.name == "FOCALLEN" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            focallen = nvalue;
      }
      else if ( !xpixsz.IsDefined() && keyword.name == "XPIXSZ" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            xpixsz = nvalue;
      }
      else if ( !expTime.IsDefined() && keyword.name == "EXPTIME" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            expTime = nvalue;
      }
   }

   /*
    * Secondary optional FITS keywords, supported for compatibility with some
    * applications.
    */
   for ( const FITSHeaderKeyword& keyword : keywords )
   {
      IsoString svalue = keyword.StripValueDelimiters();
      double nvalue;
      if ( !objctra.IsDefined() && keyword.name == "OBJCTRA" )
      {
         if ( svalue.TrySexagesimalToDouble( nvalue, Array<char>() << ' ' << ':' ) )
            if ( nvalue >= 0 )
            {
               nvalue *= 15;
               if ( nvalue <= 360 )
               {
                  if ( nvalue == 360 )
                     nvalue = 0;
                  objctra = nvalue;
               }
            }
      }
      else if ( !objctdec.IsDefined() && keyword.name == "OBJCTDEC" )
      {
         if ( svalue.TrySexagesimalToDouble( nvalue, Array<char>() << ' ' << ':' ) )
            if ( nvalue >= -90 )
               if ( nvalue <= +90 )
                  objctdec = nvalue;
      }
      else if ( !dateobs.IsDefined() && keyword.name == "DATE-OBS" )
      {
         TimePoint t;
         if ( TimePoint::TryFromString( t, svalue ) )
            dateobs = t.JD();
      }
      else if ( !longobs.IsDefined() && (keyword.name == "LONG-OBS" || keyword.name == "SITELONG") )
      {
         if ( svalue.TrySexagesimalToDouble( nvalue, Array<char>() << ' ' << ':' ) )
         {
            if ( nvalue > 180 )
               nvalue -= 360;
            else if ( nvalue <= -180 )
               nvalue += 360;
            if ( nvalue >= -180 )
               if ( nvalue <= 180 )
                  longobs = nvalue;
         }
      }
      else if ( !latobs.IsDefined() && (keyword.name == "LAT-OBS" || keyword.name == "SITELAT") )
      {
         if ( svalue.TrySexagesimalToDouble( nvalue, Array<char>() << ' ' << ':' ) )
            if ( nvalue >= -90 )
               if ( nvalue <= +90 )
                  latobs = nvalue;
      }
      else if ( !altobs.IsDefined() && (keyword.name == "ALT-OBS" || keyword.name == "SITEELEV") )
      {
         if ( svalue.TryToDouble( nvalue ) )
            altobs = nvalue;
      }
      else if ( !xpixsz.IsDefined() && keyword.name == "PIXSIZE" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            xpixsz = nvalue;
      }
      else if ( !expTime.IsDefined() && keyword.name == "EXPOSURE" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            expTime = nvalue;
      }
   }

   /*
    * If Observation:Time:End is not available, try to approximate it from the
    * observation start time and exposure time in seconds.
    */
   if ( !dateend.IsDefined() )
      if ( dateobs.IsDefined() )
         if ( expTime.IsDefined() )
            dateend = dateobs() + expTime()/86400;

   // For mental sanity, ensure start_time <= end_time.
   if ( dateobs.IsDefined() )
      if ( dateend.IsDefined() )
         if ( dateend() < dateobs() )
            Swap( dateobs, dateend );
}

// ----------------------------------------------------------------------------

PropertyArray WCSKeywords::ToProperties() const
{
   PropertyArray properties;

   if ( objctra.IsDefined() )
      properties << Property( "Observation:Center:RA", objctra() );

   if ( objctdec.IsDefined() )
      properties << Property( "Observation:Center:Dec", objctdec() );

   if ( !radesys.IsEmpty() )
      properties << Property( "Observation:CelestialReferenceSystem", radesys );

   if ( equinox.IsDefined() )
      properties << Property( "Observation:Equinox", equinox() );

   if ( dateobs.IsDefined() )
      properties << Property( "Observation:Time:Start", dateobs() );

   if ( dateend.IsDefined() )
      properties << Property( "Observation:Time:End", dateend() );

   if ( longobs.IsDefined() && latobs.IsDefined() )
   {
      properties << Property( "Observation:Location:Longitude", longobs() );
      properties << Property( "Observation:Location:Latitude", latobs() );
      if ( altobs.IsDefined() )
         properties << Property( "Observation:Location:Elevation", altobs() );
   }

   if ( focallen.IsDefined() )
      properties << Property( "Instrument:Telescope:FocalLength", focallen() );

   if ( xpixsz.IsDefined() )
      properties << Property( "Instrument:Sensor:XPixelSize", xpixsz() );

   if ( !ctype1.IsEmpty() )
      properties << Property( "PCL:AstrometricSolution:ProjectionSystem", ProjectionBase::WCSCodeToProjectionIdentifier( ctype1.Substring( 5, 3 ) ) );

   if ( crval1.IsDefined() && crval2.IsDefined() )
      properties << Property( "PCL:AstrometricSolution:ReferenceCelestialCoordinates", Vector{ crval1(), crval2() } );

   if ( crpix1.IsDefined() && crpix2.IsDefined() )
      properties << Property( "PCL:AstrometricSolution:ReferenceImageCoordinates", Vector{ crpix1(), crpix2() } );

   if ( pv1_1.IsDefined() && pv1_2.IsDefined() )
      properties << Property( "PCL:AstrometricSolution:ReferenceNativeCoordinates", Vector{ pv1_1(), pv1_2() } );

   if ( lonpole.IsDefined() && latpole.IsDefined() )
      properties << Property( "PCL:AstrometricSolution:CelestialPoleNativeCoordinates", Vector{ lonpole(), latpole() } );

   if ( cd1_1.IsDefined() && cd1_2.IsDefined() && cd2_1.IsDefined() && cd2_2.IsDefined() )
   {
      Matrix L( 2, 2 );
      L[0][0] = cd1_1();
      L[0][1] = cd1_2();
      L[1][0] = cd2_1();
      L[1][1] = cd2_2();
      properties << Property( "PCL:AstrometricSolution:LinearTransformationMatrix", L );
   }

   return properties;
}

// ----------------------------------------------------------------------------

bool WCSKeywords::ExtractWorldTransformation( LinearTransformation& transIW, int imageHeight )
{
   bool bottomUp = true;
   if ( cd1_1.IsDefined() && cd1_2.IsDefined() && cd2_1.IsDefined() && cd2_2.IsDefined() )
   {
      transIW = LinearTransformation( cd1_1(), cd1_2(), -cd1_1()*crpix1() - cd1_2()*crpix2(),
                                      cd2_1(), cd2_2(), -cd2_1()*crpix1() - cd2_2()*crpix2() );
      if ( m_fromFITS )
      {
         /*
          * See "Representations of celestial coordinates in FITS", Sect. 6.2.
          */
         double rot1, rot2;
         if ( cd2_1() > 0 )
            rot1 = ArcTan( cd2_1(), cd1_1() );
         else if ( cd2_1() < 0 )
            rot1 = ArcTan( -cd2_1(), -cd1_1() );
         else
            rot1 = 0;

         if ( cd1_2() > 0 )
            rot2 = ArcTan( cd1_2(), -cd2_2() );
         else if ( cd1_2() < 0 )
            rot2 = ArcTan( -cd1_2(), cd2_2() );
         else
            rot2 = 0;

         double sinrot, cosrot;
         SinCos( (rot1 + rot2)/2, sinrot, cosrot );
         double cdelt2 = (Abs( cosrot ) > Abs( sinrot )) ? cd2_2()/cosrot : -cd1_2()/sinrot;
         if ( cdelt2 < 0 )
            bottomUp = false;
      }
   }
   else if ( cdelt1.IsDefined() && cdelt2.IsDefined() )
   {
      if ( !crota2.IsDefined() )
         crota2 = 0;
      double sinr, cosr;
      SinCos( Rad( crota2() ), sinr, cosr );
      double cd1_1 =  cdelt1()*cosr;
      double cd1_2 = -cdelt2()*sinr;
      double cd2_1 =  cdelt1()*sinr;
      double cd2_2 =  cdelt2()*cosr;
      transIW = LinearTransformation( cd1_1, cd1_2, -cd1_1*crpix1() - cd1_2*crpix2(),
                                      cd2_1, cd2_2, -cd2_1*crpix1() - cd2_2*crpix2() );
      if ( m_fromFITS )
         if ( cdelt2() < 0 )
            bottomUp = false;
   }
   else
      return false;

   /*
    * If the data comes from FITS keywords, transform pixel coordinates from
    * the FITS coordinate system to our native coordinate system.
    */
   if ( m_fromFITS )
   {
      LinearTransformation ref_F_I;
      if ( bottomUp )
         ref_F_I = LinearTransformation( 1,  0,            -0.5,
                                         0, -1, imageHeight+0.5 );
      else
         ref_F_I = LinearTransformation( 1,  0,            -0.5,
                                         0,  1,            -0.5 );

      transIW = transIW.Multiply( ref_F_I.Inverse() );
   }

   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/WCSKeywords.cpp - Released 2023-08-01T16:29:57Z
