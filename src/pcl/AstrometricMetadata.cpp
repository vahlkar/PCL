//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// pcl/AstrometricMetadata.cpp - Released 2024-12-11T17:42:39Z
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

#include <pcl/AstrometricMetadata.h>
#include <pcl/AstrometricReprojection.h>
#include <pcl/ImageWindow.h>
#include <pcl/ProjectionFactory.h>
#include <pcl/Version.h>
#include <pcl/XISF.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

AstrometricMetadata::AstrometricMetadata( ProjectionBase* projection,
                                          WorldTransformation* worldTransformation, int width, int height )
   : m_projection( projection )
   , m_transformWI( worldTransformation )
   , m_width( width )
   , m_height( height )
{
   LinearTransformation linearIW = m_transformWI->ApproximateLinearTransform();
   double resx = Sqrt( linearIW.A00() * linearIW.A00() + linearIW.A01() * linearIW.A01() );
   double resy = Sqrt( linearIW.A10() * linearIW.A10() + linearIW.A11() * linearIW.A11() );
   m_resolution = (resx + resy)/2;
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Build( const PropertyArray& properties, const FITSKeywordArray& keywords,
                                 int width, int height, bool regenerate )
{
   m_description.Destroy();

   WCSKeywords wcs( properties, keywords );

   m_pixelSize = wcs.xpixsz;

   if ( wcs.dateobs.IsDefined() )
      m_obsStartTime = TimePoint( wcs.dateobs() );
   else
      m_obsStartTime = Optional<TimePoint>();

   if ( wcs.dateend.IsDefined() )
      m_obsEndTime = TimePoint( wcs.dateend() );
   else
      m_obsEndTime = Optional<TimePoint>();

   m_geoLongitude = wcs.longobs;
   m_geoLatitude = wcs.latobs;
   m_geoHeight = wcs.altobs;

   m_refSys = wcs.radesys;
   m_equinox = wcs.equinox;

   m_width = width;
   m_height = height;
   m_resolution = 0;

   m_creationTime = Optional<TimePoint>();
   m_catalog.Clear();
   m_creatorApp.Clear();
   m_creatorModule.Clear();
   m_creatorOS.Clear();

   const IsoString splineWTPrefix = SplineWorldTransformation::PropertyPrefix();
   bool hasSplineWT = false;
   for ( const Property& property : properties )
      if ( !hasSplineWT &&
           (property.Id().StartsWith( splineWTPrefix )
         || property.Id() == "PCL:AstrometricSolution:SplineWorldTransformation" // core 1.8.9-2
         || property.Id() == "Transformation_ImageToProjection") )               // core < 1.8.9-2
         hasSplineWT = true;
      else if ( property.Id() == "PCL:AstrometricSolution:CreationTime" )
         m_creationTime = property.Value().ToTimePoint();
      else if ( property.Id() == "PCL:AstrometricSolution:Catalog" )
         m_catalog = property.Value().ToString();
      else if ( property.Id() == "PCL:AstrometricSolution:CreatorApplication" )
         m_creatorApp = property.Value().ToString();
      else if ( property.Id() == "PCL:AstrometricSolution:CreatorModule" )
         m_creatorModule = property.Value().ToString();
      else if ( property.Id() == "PCL:AstrometricSolution:CreatorOS" )
         m_creatorOS = property.Value().ToString();

   if ( wcs.ctype1.StartsWith( "RA--" ) &&
        wcs.ctype2.StartsWith( "DEC-" ) &&
        wcs.crpix1.IsDefined() && wcs.crpix2.IsDefined() &&
        wcs.crval1.IsDefined() && wcs.crval2.IsDefined() )
   {
      m_projection = ProjectionFactory::Create( wcs );

      LinearTransformation linearIW;
      if ( wcs.ExtractWorldTransformation( linearIW, m_height ) )
      {
         if ( hasSplineWT )
            m_transformWI = new SplineWorldTransformation( properties, linearIW, regenerate );
         else
            m_transformWI = new LinearWorldTransformation( linearIW );

         double resx = Sqrt( linearIW.A00()*linearIW.A00() + linearIW.A01()*linearIW.A01() );
         double resy = Sqrt( linearIW.A10()*linearIW.A10() + linearIW.A11()*linearIW.A11() );
         m_resolution = (resx + resy)/2;
         if ( m_pixelSize.IsDefined() )
            if ( m_pixelSize() > 0 )
               m_focalLength = FocalFromResolution( m_resolution );
      }
   }

   if ( m_transformWI.IsNull() )
   {
      if ( wcs.focallen.IsDefined() )
         if ( wcs.focallen() > 0 )
            m_focalLength = wcs.focallen();
      if ( m_focalLength.IsDefined() )
         if ( m_focalLength() > 0 )
            if ( m_pixelSize.IsDefined() )
               if ( m_pixelSize() > 0 )
                  m_resolution = ResolutionFromFocal( m_focalLength() );
   }
}

// ----------------------------------------------------------------------------

IsoString AstrometricMetadata::ReferenceSystemFromMetadata( const PropertyArray& properties, const FITSKeywordArray& keywords )
{
   for ( const Property& property : properties )
      if ( property.Id() == "Observation:CelestialReferenceSystem" )
         return property.Value().ToIsoString();
   for ( const FITSHeaderKeyword& keyword : keywords )
      if ( keyword.name == "RADESYS" )
         return keyword.StripValueDelimiters().Uppercase();
   return IsoString();
}

// ----------------------------------------------------------------------------

AstrometricMetadata::AstrometricMetadata( const ImageWindow& window )
{
   int width, height;
   View view = window.MainView();
   view.GetSize( width, height );
   Build( view.Properties(), window.Keywords(), width, height );
}

// ----------------------------------------------------------------------------

AstrometricMetadata::AstrometricMetadata( XISFReader& reader )
{
   ImageInfo info = reader.ImageInfo();
   Build( reader.ReadImageProperties(), reader.ReadFITSKeywords(), info.width, info.height );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Write( ImageWindow& window, bool notify ) const
{
   if ( !IsValid() )
      throw Error( "AstrometricMetadata::Write(): Invalid or uninitialized metadata." );

   View view = window.MainView();
   if ( view.Width() != m_width || view.Height() != m_height )
      throw Error( "AstrometricMetadata::Write(): Metadata not compatible with the dimensions of the image." );

   FITSKeywordArray keywords = window.Keywords();
   UpdateBasicKeywords( keywords );
   UpdateWCSKeywords( keywords );
   window.SetKeywords( keywords );

   RemoveSplineWorldTransformationProperties( window );

   if ( m_focalLength.IsDefined() && m_focalLength() > 0 )
      view.SetStorablePermanentPropertyValue( "Instrument:Telescope:FocalLength", Round( m_focalLength()/1000, 6 ), notify );

   if ( m_pixelSize.IsDefined() && m_pixelSize() > 0 )
   {
      view.SetStorablePermanentPropertyValue( "Instrument:Sensor:XPixelSize", Round( m_pixelSize(), 3 ), notify );
      view.SetStorablePermanentPropertyValue( "Instrument:Sensor:YPixelSize", Round( m_pixelSize(), 3 ), notify );
   }

   if ( m_obsStartTime.IsDefined() )
      view.SetStorablePermanentPropertyValue( "Observation:Time:Start", m_obsStartTime(), notify );

   if ( m_obsEndTime.IsDefined() )
      view.SetStorablePermanentPropertyValue( "Observation:Time:End", m_obsEndTime(), notify );

   if ( m_geoLongitude.IsDefined() && m_geoLatitude.IsDefined() )
   {
      view.SetStorablePermanentPropertyValue( "Observation:Location:Longitude", Round( m_geoLongitude(), 6 ), notify );
      view.SetStorablePermanentPropertyValue( "Observation:Location:Latitude", Round( m_geoLatitude(), 6 ), notify );
      if ( m_geoHeight.IsDefined() )
         view.SetStorablePermanentPropertyValue( "Observation:Location:Elevation", RoundInt( m_geoHeight() ), notify );
   }

   DPoint pRD;
   if ( ImageToCelestial( pRD, DPoint( 0.5*m_width, 0.5*m_height ) ) )
   {
      view.SetStorablePermanentPropertyValue( "Observation:Center:RA", pRD.x, notify );
      view.SetStorablePermanentPropertyValue( "Observation:Center:Dec", pRD.y, notify );
      view.SetStorablePermanentPropertyValue( "Observation:CelestialReferenceSystem", ReferenceSystem(), notify );
      view.SetStorablePermanentPropertyValue( "Observation:Equinox", 2000.0, notify );
      // The default reference point is the geometric center of the image.
      view.DeletePropertyIfExists( "Observation:Center:X", notify );
      view.DeletePropertyIfExists( "Observation:Center:Y", notify );
   }

   LinearTransformation trans_I_W = m_transformWI->ApproximateLinearTransform();
   Matrix LT( 2, 2 );
   LT[0][0] = trans_I_W.A00();
   LT[0][1] = trans_I_W.A01();
   LT[1][0] = trans_I_W.A10();
   LT[1][1] = trans_I_W.A11();

   DPoint p0 = trans_I_W.TransformInverse( DPoint( 0 ) );
   view.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:ProjectionSystem", m_projection->Identifier(), notify );
   view.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:ReferenceCelestialCoordinates", m_projection->ReferenceCelestialCoordinates(), notify );
   view.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:ReferenceImageCoordinates", Vector{ p0.x, p0.y }, notify );
   view.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:ReferenceNativeCoordinates", m_projection->ReferenceNativeCoordinates(), notify );
   view.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:CelestialPoleNativeCoordinates", m_projection->CelestialPoleNativeCoordinates(), notify );
   view.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:LinearTransformationMatrix", LT, notify );

   if ( m_creationTime.IsDefined() )
      view.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:CreationTime", m_creationTime(), notify );
   else
      view.DeletePropertyIfExists( "PCL:AstrometricSolution:CreationTime", notify );

   if ( !m_catalog.IsEmpty() )
      view.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:Catalog", m_catalog, notify );
   else
      view.DeletePropertyIfExists( "PCL:AstrometricSolution:Catalog", notify );

   if ( !m_creatorOS.IsEmpty() )
      view.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:CreatorOS", m_creatorOS, notify );
   else
      view.DeletePropertyIfExists( "PCL:AstrometricSolution:CreatorOS", notify );

   if ( !m_creatorApp.IsEmpty() )
      view.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:CreatorApplication", m_creatorApp, notify );
   else
      view.DeletePropertyIfExists( "PCL:AstrometricSolution:CreatorApplication", notify );

   if ( !m_creatorModule.IsEmpty() )
      view.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:CreatorModule", m_creatorModule, notify );
   else
      view.DeletePropertyIfExists( "PCL:AstrometricSolution:CreatorModule", notify );

   const SplineWorldTransformation* S = dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() );
   if ( S != nullptr )
   {
      PropertyArray properties = S->ToProperties();
      for ( const Property& property : properties )
         view.SetStorablePermanentPropertyValue( property.Id(), property.Value(), notify );
   }
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Write( XISFWriter& writer ) const
{
   if ( !IsValid() )
      throw Error( "AstrometricMetadata::Write(): Invalid or uninitialized metadata." );

   FITSKeywordArray keywords = writer.FITSKeywords();
   UpdateBasicKeywords( keywords );
   UpdateWCSKeywords( keywords );
   writer.WriteFITSKeywords( keywords );

   if ( m_focalLength.IsDefined() && m_focalLength() > 0 )
      writer.WriteImageProperty( "Instrument:Telescope:FocalLength", Round( m_focalLength()/1000, 6 ) );

   if ( m_pixelSize.IsDefined() && m_pixelSize() > 0 )
   {
      writer.WriteImageProperty( "Instrument:Sensor:XPixelSize", Round( m_pixelSize(), 3 ) );
      writer.WriteImageProperty( "Instrument:Sensor:YPixelSize", Round( m_pixelSize(), 3 ) );
   }

   if ( m_obsStartTime.IsDefined() )
      writer.WriteImageProperty( "Observation:Time:Start", m_obsStartTime() );

   if ( m_obsEndTime.IsDefined() )
      writer.WriteImageProperty( "Observation:Time:End", m_obsEndTime() );

   if ( m_geoLongitude.IsDefined() && m_geoLatitude.IsDefined() )
   {
      writer.WriteImageProperty( "Observation:Location:Longitude", Round( m_geoLongitude(), 6 ) );
      writer.WriteImageProperty( "Observation:Location:Latitude", Round( m_geoLatitude(), 6 ) );
      if ( m_geoHeight.IsDefined() )
         writer.WriteImageProperty( "Observation:Location:Elevation", RoundInt( m_geoHeight() ) );
   }

   DPoint pRD;
   if ( ImageToCelestial( pRD, DPoint( 0.5*m_width, 0.5*m_height ) ) )
   {
      writer.WriteImageProperty( "Observation:Center:RA", pRD.x );
      writer.WriteImageProperty( "Observation:Center:Dec", pRD.y );
      writer.WriteImageProperty( "Observation:CelestialReferenceSystem", ReferenceSystem() );
      writer.WriteImageProperty( "Observation:Equinox", 2000.0 );
      // The default reference point is the geometric center of the image.
      writer.RemoveImageProperty( "Observation:Center:X" );
      writer.RemoveImageProperty( "Observation:Center:Y" );
   }

   LinearTransformation trans_I_W = m_transformWI->ApproximateLinearTransform();
   Matrix LT( 2, 2 );
   LT[0][0] = trans_I_W.A00();
   LT[0][1] = trans_I_W.A01();
   LT[1][0] = trans_I_W.A10();
   LT[1][1] = trans_I_W.A11();

   DPoint p0 = trans_I_W.TransformInverse( DPoint( 0 ) );
   writer.WriteImageProperty( "PCL:AstrometricSolution:ProjectionSystem", m_projection->Identifier() );
   writer.WriteImageProperty( "PCL:AstrometricSolution:ReferenceCelestialCoordinates", m_projection->ReferenceCelestialCoordinates() );
   writer.WriteImageProperty( "PCL:AstrometricSolution:ReferenceImageCoordinates", Vector{ p0.x, p0.y } );
   writer.WriteImageProperty( "PCL:AstrometricSolution:ReferenceNativeCoordinates", m_projection->ReferenceNativeCoordinates() );
   writer.WriteImageProperty( "PCL:AstrometricSolution:CelestialPoleNativeCoordinates", m_projection->CelestialPoleNativeCoordinates() );
   writer.WriteImageProperty( "PCL:AstrometricSolution:LinearTransformationMatrix", LT );

   if ( m_creationTime.IsDefined() )
      writer.WriteImageProperty( "PCL:AstrometricSolution:CreationTime", m_creationTime() );

   if ( !m_catalog.IsEmpty() )
      writer.WriteImageProperty( "PCL:AstrometricSolution:Catalog", m_catalog );

   if ( !m_creatorOS.IsEmpty() )
      writer.WriteImageProperty( "PCL:AstrometricSolution:CreatorOS", m_creatorOS );

   if ( !m_creatorApp.IsEmpty() )
      writer.WriteImageProperty( "PCL:AstrometricSolution:CreatorApplication", m_creatorApp );

   if ( !m_creatorModule.IsEmpty() )
      writer.WriteImageProperty( "PCL:AstrometricSolution:CreatorModule", m_creatorModule );

   const SplineWorldTransformation* S = dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() );
   if ( S != nullptr )
   {
      PropertyArray properties = S->ToProperties();
      for ( const Property& property : properties )
         writer.WriteImageProperty( property.Id(), property.Value() );
   }
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Verify( DPoint& centerErrors,
                                  DPoint& topLeftErrors, DPoint& topRightErrors,
                                  DPoint& bottomLeftErrors, DPoint& bottomRightErrors ) const
{
   if ( !IsValid() )
      throw Error( "AstrometricMetadata::Verify(): Invalid or uninitialized metadata." );

   /*
    * Performs full cycle transformations (image > celestial > image) and
    * reports the resulting absolute differences in pixels.
    */
   Array<DPoint> pI;
   pI << DPoint( 0.5*m_width, 0.5*m_height )
      << DPoint( 0.5,         0.5 )
      << DPoint( m_width-0.5, 0.5 )
      << DPoint( 0.5,         m_height-0.5 )
      << DPoint( m_width-0.5, m_height-0.5 );
   Array<DPoint> pIr;
   for ( int i = 0; i < 5; ++i )
   {
      DPoint pRD;
      if ( !ImageToCelestial( pRD, pI[i] ) )
         throw Error( String().Format(
            "AstrometricMetadata::Verify(): Failed to perform ImageToCelestial() coordinate transformation, step %d.", i+1 ) );
      DPoint pIi;
      if ( !CelestialToImage( pIi, pRD ) )
         throw Error( String().Format(
            "AstrometricMetadata::Verify(): Failed to perform CelestialToImage() coordinate transformation, step %d.", i+1 ) );
      pIr << DPoint( pI[i].x - pIi.x, pI[i].y - pIi.y );
   }
   centerErrors = pIr[0];
   topLeftErrors = pIr[1];
   topRightErrors = pIr[2];
   bottomLeftErrors = pIr[3];
   bottomRightErrors = pIr[4];
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Validate( double tolerance ) const
{
   DPoint e0, dontcare;
   Verify( e0, dontcare, dontcare, dontcare, dontcare );
   if ( Abs( e0.x ) > tolerance || Abs( e0.y ) > tolerance )
      throw Error( "AstrometricMetadata::Validate(): Inconsistent coordinate transformation results." );
}

// ----------------------------------------------------------------------------

AstrometricMetadata AstrometricMetadata::Undistorted() const
{
   const SplineWorldTransformation* S = dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() );
   if ( S == nullptr )
      return *this;

   AstrometricMetadata linearSolution;
   linearSolution.m_projection    = m_projection->Clone();
   linearSolution.m_transformWI   = new LinearWorldTransformation( S->ApproximateLinearTransform() );
   linearSolution.m_refSys        = m_refSys;
   linearSolution.m_equinox       = m_equinox;
   linearSolution.m_width         = m_width;
   linearSolution.m_height        = m_height;
   linearSolution.m_pixelSize     = m_pixelSize;
   linearSolution.m_obsStartTime  = m_obsStartTime;
   linearSolution.m_obsEndTime    = m_obsEndTime;
   linearSolution.m_geoLongitude  = m_geoLongitude;
   linearSolution.m_geoLatitude   = m_geoLatitude;
   linearSolution.m_geoHeight     = m_geoHeight;
   linearSolution.m_resolution    = m_resolution;
   linearSolution.m_focalLength   = m_focalLength;
   linearSolution.m_creationTime  = m_creationTime;
   linearSolution.m_catalog       = m_catalog;
   linearSolution.m_creatorApp    = m_creatorApp;
   linearSolution.m_creatorModule = m_creatorModule;
   linearSolution.m_creatorOS     = m_creatorOS;

   Rect rect = AstrometricReprojection::TargetRect( linearSolution, *this, Bounds() );

   linearSolution.m_width  = rect.Width();
   linearSolution.m_height = rect.Height();
   linearSolution.m_transformWI = new LinearWorldTransformation(
         S->ApproximateLinearTransform().Multiply( LinearTransformation( 1, 0, rect.x0,
                                                                         0, 1, rect.y0 ) ) );
   return linearSolution;
}

// ----------------------------------------------------------------------------

double AstrometricMetadata::Rotation( bool& flipped ) const
{
   if ( m_transformWI.IsNull() )
      throw Error( "AstrometricMetadata::Rotation(): No world transformation defined." );

   LinearTransformation linearIW = m_transformWI->ApproximateLinearTransform();
   flipped = linearIW.Determinant() < 0;
   double rotation = Deg( ArcTan( linearIW.A00() + linearIW.A01(), linearIW.A10() + linearIW.A11() ) ) + 135;
   if ( flipped )
      rotation = -90 - rotation;
   if ( rotation <= -180 )
      rotation += 360;
   if ( rotation > 180 )
      rotation -= 360;
   return rotation;
}

// ----------------------------------------------------------------------------

String AstrometricMetadata::Summary() const
{
   if ( !IsValid() )
      throw Error( "AstrometricMetadata::Summary(): No astrometric solution." );

   UpdateDescription();

   String summary;

   if ( !m_description->creationTime.IsEmpty() )
      summary << "Creation time ............ " << m_description->creationTime << '\n';
   if ( !m_description->creationSoftware.IsEmpty() )
      summary << "Creation software ........ " << m_description->creationSoftware << '\n';

   if ( !m_description->referenceCatalog.IsEmpty() )
      summary << "Reference catalog ........ " << m_description->referenceCatalog << '\n';

   summary    << "Linear transformation matrix (world[ra,dec] = matrix * image[x,y]):" << '\n'
              << m_description->referenceMatrix << '\n'
              << "WCS transformation ....... " << m_description->wcsTransformationType << '\n';

   if ( !m_description->controlPoints.IsEmpty() )
      summary << "Control points ........... " << m_description->controlPoints << '\n';

   if ( !m_description->splineLengths.IsEmpty() )
      summary << "Spline lengths ........... " << m_description->splineLengths << '\n';
   if ( !m_description->splineErrors.IsEmpty() )
      summary << "Surface residuals ........ " << m_description->splineErrors << '\n';

   summary    << "Projection ............... " << m_description->projectionName << '\n'
              << "Projection origin ........ " << m_description->projectionOrigin << '\n'
              << "Resolution ............... " << m_description->resolution << '\n'
              << "Rotation ................. " << m_description->rotation << '\n'
              << "Reference system ......... " << m_description->referenceSystem << '\n';

   if ( !m_description->observationStartTime.IsEmpty() )
      summary << "Observation start time ... " << m_description->observationStartTime << '\n';
   if ( !m_description->observationEndTime.IsEmpty() )
      summary << "Observation end time ..... " << m_description->observationEndTime << '\n';

   if ( !m_description->observerLocation.IsEmpty() )
      summary << "Geodetic coordinates ..... " << m_description->observerLocation << '\n';

   if ( !m_description->focalDistance.IsEmpty() )
      summary << "Focal distance ........... " << m_description->focalDistance << '\n';

   if ( !m_description->pixelSize.IsEmpty() )
      summary << "Pixel size ............... " << m_description->pixelSize << '\n';

   summary    << "Field of view ............ " << m_description->fieldOfView << '\n'
              << "Image center ............. " << m_description->centerCoordinates << '\n'
              << "Image bounds:" << '\n'
              << "   top-left .............. " << m_description->topLeftCoordinates << '\n'
              << "   top-right ............. " << m_description->topRightCoordinates << '\n'
              << "   bottom-left ........... " << m_description->bottomLeftCoordinates << '\n'
              << "   bottom-right .......... " << m_description->bottomRightCoordinates << '\n';

   return summary;
}

// ----------------------------------------------------------------------------

static void ModifyKeyword( FITSKeywordArray& keywords, const IsoString& name, const IsoString& value, const IsoString& comment )
{
   for ( FITSHeaderKeyword& keyword : keywords )
      if ( keyword.name == name )
      {
         keyword.value = value;
         keyword.comment = comment;
         return;
      }

   keywords << FITSHeaderKeyword( name, value, comment );
}

static void ModifySignatureKeyword( FITSKeywordArray& keywords )
{
   FITSHeaderKeyword signature( "COMMENT", IsoString(), "Astrometric solution by " + PixInsightVersion::AsString() );
   for ( FITSHeaderKeyword& keyword : keywords )
      if ( keyword.name == "COMMENT" )
         if ( keyword.comment.StartsWith( "Astrometric solution by" ) )
         {
            keyword = signature;
            return;
         }
   keywords << signature;
}

static void RemoveKeyword( FITSKeywordArray& keywords, const IsoString& name )
{
   keywords.Remove( FITSHeaderKeyword( name ), []( const FITSHeaderKeyword& k1, const FITSHeaderKeyword& k2 )
                                               { return k1.name == k2.name; } );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::UpdateBasicKeywords( FITSKeywordArray& keywords ) const
{
   if ( m_focalLength.IsDefined() && m_focalLength() > 0 )
      ModifyKeyword( keywords, "FOCALLEN", IsoString().Format( "%.8g", m_focalLength() ), "Focal length (mm)" );
   else
      RemoveKeyword( keywords, "FOCALLEN" );

   if ( m_pixelSize.IsDefined() && m_pixelSize() > 0 )
   {
      ModifyKeyword( keywords, "XPIXSZ", IsoString().Format( "%.6g", m_pixelSize() ), "Pixel size including binning, X-axis (um)" );
      ModifyKeyword( keywords, "YPIXSZ", IsoString().Format( "%.6g", m_pixelSize() ), "Pixel size including binning, Y-axis (um)" );
      RemoveKeyword( keywords, "PIXSIZE" );
   }

   ModifyKeyword( keywords, "TIMESYS", "\'UTC\'", "Time scale: Universal Time, Coordinated" );

   if ( m_obsStartTime.IsDefined() )
   {
      ModifyKeyword( keywords, "DATE-OBS",
            m_obsStartTime().ToIsoString( 3/*timeItems*/, 3/*precision*/, 0/*tz*/, false/*timeZone*/ ).SingleQuoted(),
            "Start date/time of observation (UTC)" );
      RemoveKeyword( keywords, "DATE-BEG" );
   }

   if ( m_obsEndTime.IsDefined() )
      ModifyKeyword( keywords, "DATE-END",
            m_obsEndTime().ToIsoString( 3/*timeItems*/, 3/*precision*/, 0/*tz*/, false/*timeZone*/ ).SingleQuoted(),
            "End date/time of observation (UTC)" );

   if ( m_geoLongitude.IsDefined() && m_geoLatitude.IsDefined() )
   {
      ModifyKeyword( keywords, "OBSGEO-L", IsoString().Format( "%.10g", m_geoLongitude() ), "Geodetic longitude of observation location (deg)" );
      ModifyKeyword( keywords, "LONG-OBS", IsoString().Format( "%.10g", m_geoLongitude() ), "Geodetic longitude (deg) (compatibility)" );
      RemoveKeyword( keywords, "SITELONG" );

      ModifyKeyword( keywords, "OBSGEO-B", IsoString().Format( "%.10g", m_geoLatitude() ), "Geodetic latitude of observation location (deg)" );
      ModifyKeyword( keywords, "LAT-OBS", IsoString().Format( "%.10g", m_geoLatitude() ), "Geodetic latitude (deg) (compatibility)" );
      RemoveKeyword( keywords, "SITELAT" );

      if ( m_geoHeight.IsDefined() )
      {
         ModifyKeyword( keywords, "OBSGEO-H", IsoString().Format( "%.0f", m_geoHeight() ), "Geodetic height of observation location (m)" );
         ModifyKeyword( keywords, "ALT-OBS", IsoString().Format( "%.0f", m_geoHeight() ), "Geodetic height (m) (compatibility)" );
         RemoveKeyword( keywords, "SITEELEV" );
      }
   }

   DPoint pRD;
   if ( ImageToCelestial( pRD, DPoint( 0.5*m_width, 0.5*m_height ) ) )
   {
      ModifyKeyword( keywords, "RA",
            IsoString().Format( "%.16g", pRD.x ),
            "Right ascension of the center of the image (deg)" );
      ModifyKeyword( keywords, "OBJCTRA",
            IsoString::ToSexagesimal( pRD.x/15, RAConversionOptions( 3/*precision*/, 0/*width*/ ) ).SingleQuoted(),
            "Right ascension (hours) (compatibility)" );

      ModifyKeyword( keywords, "DEC",
            IsoString().Format( "%.16g", pRD.y ),
            "Declination of the center of the image (deg)" );
      ModifyKeyword( keywords, "OBJCTDEC",
            IsoString::ToSexagesimal( pRD.y, DecConversionOptions( 2/*precision*/, 0/*width*/ ) ).SingleQuoted(),
            "Declination (deg) (compatibility)" );
   }
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::UpdateWCSKeywords( FITSKeywordArray& keywords, bool generate ) const
{
   RemoveKeywords( keywords, false/*removeCenterKeywords*/, false/*removeScaleKeywords*/ );

   if ( generate && IsValid() )
   {
      ModifySignatureKeyword( keywords );

      IsoString refSys = ReferenceSystem();
      if ( refSys == "ICRS" )
         keywords << FITSHeaderKeyword( "RADESYS", "'ICRS'", "Coordinates referred to ICRS / J2000.0" );
      else if ( refSys == "GCRS" )
         keywords << FITSHeaderKeyword( "RADESYS", "'GCRS'", "Coordinates referred to GCRS / J2000.0" );
      else
      {
         IsoString fitsRefSys = refSys.Uppercase();
         if ( fitsRefSys == "TRUE" || fitsRefSys == "MEAN" || fitsRefSys == "APPARENT" || fitsRefSys == "GAPPT" )
            keywords << FITSHeaderKeyword( "RADESYS", "'GAPPT'", "Geocentric apparent coordinates / J2000.0" );
         else
         {
            keywords << FITSHeaderKeyword( "RADESYS", refSys.SingleQuoted(), "Reference system of celestial coordinates" );
            if ( m_equinox.IsDefined() )
               keywords << FITSHeaderKeyword( "EQUINOX", IsoString( m_equinox() ), "Epoch of the mean equator and equinox (years)" );
         }
      }

      WCSKeywords wcs = ComputeWCSKeywords();

      keywords << FITSHeaderKeyword( "CTYPE1", wcs.ctype1, "Axis1 projection: " + m_projection->Name() )
               << FITSHeaderKeyword( "CTYPE2", wcs.ctype2, "Axis2 projection: " + m_projection->Name() )
               << FITSHeaderKeyword( "CRPIX1", IsoString().Format( "%.16g", wcs.crpix1() ), "Axis1 reference pixel coordinate" )
               << FITSHeaderKeyword( "CRPIX2", IsoString().Format( "%.16g", wcs.crpix2() ), "Axis2 reference pixel coordinate" );

      if ( wcs.crval1.IsDefined() )
         keywords << FITSHeaderKeyword( "CRVAL1", IsoString().Format( "%.16g", wcs.crval1() ), "Axis1 reference celestial coordinate" );
      if ( wcs.crval2.IsDefined() )
         keywords << FITSHeaderKeyword( "CRVAL2", IsoString().Format( "%.16g", wcs.crval2() ), "Axis2 reference celestial coordinate" );
      if ( wcs.pv1_1.IsDefined() )
         keywords << FITSHeaderKeyword( "PV1_1", IsoString().Format( "%.16g", wcs.pv1_1() ), "Native longitude of the reference point (deg)" );
      if ( wcs.pv1_2.IsDefined() )
         keywords << FITSHeaderKeyword( "PV1_2", IsoString().Format( "%.16g", wcs.pv1_2() ), "Native latitude of the reference point (deg)" );
      if ( wcs.lonpole.IsDefined() )
         keywords << FITSHeaderKeyword( "LONPOLE", IsoString().Format( "%.16g", wcs.lonpole() ), "Native longitude of the celestial north pole (deg)" );
      if ( wcs.latpole.IsDefined() )
         keywords << FITSHeaderKeyword( "LATPOLE", IsoString().Format( "%.16g", wcs.latpole() ), "Native latitude of the celestial north pole (deg)" );

      keywords << FITSHeaderKeyword( "CD1_1", IsoString().Format( "%.16g", wcs.cd1_1() ), "Linear transformation matrix (1,1)" )
               << FITSHeaderKeyword( "CD1_2", IsoString().Format( "%.16g", wcs.cd1_2() ), "Linear transformation matrix (1,2)" )
               << FITSHeaderKeyword( "CD2_1", IsoString().Format( "%.16g", wcs.cd2_1() ), "Linear transformation matrix (2,1)" )
               << FITSHeaderKeyword( "CD2_2", IsoString().Format( "%.16g", wcs.cd2_2() ), "Linear transformation matrix (2,2)" );

      // AIPS keywords (CDELT1, CDELT2, CROTA1, CROTA2)
      keywords << FITSHeaderKeyword( "CDELT1", IsoString().Format( "%.16g", wcs.cdelt1() ), "Axis1 scale" )
               << FITSHeaderKeyword( "CDELT2", IsoString().Format( "%.16g", wcs.cdelt2() ), "Axis2 scale" )
               << FITSHeaderKeyword( "CROTA1", IsoString().Format( "%.16g", wcs.crota1() ), "Axis1 rotation angle (deg)" )
               << FITSHeaderKeyword( "CROTA2", IsoString().Format( "%.16g", wcs.crota2() ), "Axis2 rotation angle (deg)" );
   }
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::RemoveKeywords( FITSKeywordArray& keywords, bool removeCenterKeywords, bool removeScaleKeywords )
{
   if ( removeCenterKeywords )
   {
      RemoveKeyword( keywords, "RA" );
      RemoveKeyword( keywords, "OBJCTRA" );
      RemoveKeyword( keywords, "DEC" );
      RemoveKeyword( keywords, "OBJCTDEC" );
   }

   if ( removeScaleKeywords )
   {
      RemoveKeyword( keywords, "FOCALLEN" );
      RemoveKeyword( keywords, "XPIXSZ" );
      RemoveKeyword( keywords, "YPIXSZ" );
      RemoveKeyword( keywords, "PIXSIZE" );
   }

   RemoveKeyword( keywords, "RADESYS" );
   RemoveKeyword( keywords, "EQUINOX" );
   RemoveKeyword( keywords, "EPOCH" );
   RemoveKeyword( keywords, "CTYPE1" );
   RemoveKeyword( keywords, "CTYPE2" );
   RemoveKeyword( keywords, "CRVAL1" );
   RemoveKeyword( keywords, "CRVAL2" );
   RemoveKeyword( keywords, "CRPIX1" );
   RemoveKeyword( keywords, "CRPIX2" );
   RemoveKeyword( keywords, "CD1_1" );
   RemoveKeyword( keywords, "CD1_2" );
   RemoveKeyword( keywords, "CD2_1" );
   RemoveKeyword( keywords, "CD2_2" );
   RemoveKeyword( keywords, "PC1_1" );
   RemoveKeyword( keywords, "PC1_2" );
   RemoveKeyword( keywords, "PC2_1" );
   RemoveKeyword( keywords, "PC2_2" );
   RemoveKeyword( keywords, "PV1_1" );
   RemoveKeyword( keywords, "PV1_2" );
   RemoveKeyword( keywords, "PV1_3" );
   RemoveKeyword( keywords, "PV1_4" );
   RemoveKeyword( keywords, "LONPOLE" );
   RemoveKeyword( keywords, "LATPOLE" );
   RemoveKeyword( keywords, "CDELT1" );
   RemoveKeyword( keywords, "CDELT2" );
   RemoveKeyword( keywords, "CROTA1" );
   RemoveKeyword( keywords, "CROTA2" );

   /*
    * Remove obsolete FITS keywords generated by plate solving scripts and
    * processes before core version 1.8.9-2.
    */
   RemoveKeyword( keywords, "POLYNDEG" );
   RemoveKeyword( keywords, "REFSPLIN" );
   RemoveKeyword( keywords, "REFSPLINE" ); // N.B. 9-char keyword name written by old versions, not FITS-compliant.

   /*
    * Remove COMMENT signature keywords, e.g.:
    *    'Astrometric solution by PixInsight x.y.z-t'
    */
   keywords.Remove( FITSHeaderKeyword(),
      []( const FITSHeaderKeyword& k1, const FITSHeaderKeyword& )
      {
         return k1.name == "COMMENT" && k1.comment.ContainsIC( "astrometric" ) && k1.comment.ContainsIC( "pixinsight" );
      } );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::RescalePixelSizeKeywords( FITSKeywordArray& keywords, double scalingFactor )
{
   for ( FITSHeaderKeyword& keyword : keywords )
      if ( keyword.name == "XPIXSZ" || keyword.name == "YPIXSZ" || keyword.name == "PIXSIZE" )
      {
         double size;
         if ( keyword.StripValueDelimiters().TryToDouble( size ) )
            keyword.value = IsoString().Format( "%.6g", size*scalingFactor );
      }
}

// ----------------------------------------------------------------------------

static void ModifyProperty( PropertyArray& properties, const IsoString& id, const Variant& value )
{
   PropertyArray::iterator i = properties.Search( id );
   if ( i != properties.End() )
      i->SetValue( value );
   else
      properties << Property( id, value );
}

static void RemoveProperty( PropertyArray& properties, const IsoString& id )
{
   properties.Remove( Property( id ) );
}

void AstrometricMetadata::UpdateProperties( PropertyArray& properties ) const
{
   RemoveSplineWorldTransformationProperties( properties );

   if ( IsValid() )
   {
      if ( m_focalLength.IsDefined() && m_focalLength() > 0 )
         ModifyProperty( properties, "Instrument:Telescope:FocalLength", Round( m_focalLength()/1000, 6 ) );
      else
         RemoveProperty( properties, "Instrument:Telescope:FocalLength" );

      if ( m_pixelSize.IsDefined() && m_pixelSize() > 0 )
      {
         ModifyProperty( properties, "Instrument:Sensor:XPixelSize", Round( m_pixelSize(), 3 ) );
         ModifyProperty( properties, "Instrument:Sensor:YPixelSize", Round( m_pixelSize(), 3 ) );
      }

      if ( m_obsStartTime.IsDefined() )
         ModifyProperty( properties, "Observation:Time:Start", m_obsStartTime() );

      if ( m_obsEndTime.IsDefined() )
         ModifyProperty( properties, "Observation:Time:End", m_obsEndTime() );

      if ( m_geoLongitude.IsDefined() && m_geoLatitude.IsDefined() )
      {
         ModifyProperty( properties, "Observation:Location:Longitude", Round( m_geoLongitude(), 6 ) );
         ModifyProperty( properties, "Observation:Location:Latitude", Round( m_geoLatitude(), 6 ) );
         if ( m_geoHeight.IsDefined() )
            ModifyProperty( properties, "Observation:Location:Elevation", RoundInt( m_geoHeight() ) );
      }

      DPoint pRD;
      if ( ImageToCelestial( pRD, DPoint( 0.5*m_width, 0.5*m_height ) ) )
      {
         ModifyProperty( properties, "Observation:Center:RA", pRD.x );
         ModifyProperty( properties, "Observation:Center:Dec", pRD.y );
         ModifyProperty( properties, "Observation:CelestialReferenceSystem", ReferenceSystem() );
         ModifyProperty( properties, "Observation:Equinox", 2000.0 );
         // The default reference point is the geometric center of the image.
         RemoveProperty( properties, "Observation:Center:X" );
         RemoveProperty( properties, "Observation:Center:Y" );
      }

      LinearTransformation trans_I_W = m_transformWI->ApproximateLinearTransform();
      Matrix LT( trans_I_W.A00(), trans_I_W.A01(),
                 trans_I_W.A10(), trans_I_W.A11() );
      DPoint p0 = trans_I_W.TransformInverse( DPoint( 0 ) );
      ModifyProperty( properties, "PCL:AstrometricSolution:ProjectionSystem", m_projection->Identifier() );
      ModifyProperty( properties, "PCL:AstrometricSolution:ReferenceCelestialCoordinates", m_projection->ReferenceCelestialCoordinates() );
      ModifyProperty( properties, "PCL:AstrometricSolution:ReferenceImageCoordinates", Vector{ p0.x, p0.y } );
      ModifyProperty( properties, "PCL:AstrometricSolution:ReferenceNativeCoordinates", m_projection->ReferenceNativeCoordinates() );
      ModifyProperty( properties, "PCL:AstrometricSolution:CelestialPoleNativeCoordinates", m_projection->CelestialPoleNativeCoordinates() );
      ModifyProperty( properties, "PCL:AstrometricSolution:LinearTransformationMatrix", LT );

      if ( m_creationTime.IsDefined() )
         ModifyProperty( properties, "PCL:AstrometricSolution:CreationTime", m_creationTime() );
      else
         RemoveProperty( properties, "PCL:AstrometricSolution:CreationTime" );

      if ( !m_catalog.IsEmpty() )
         ModifyProperty( properties, "PCL:AstrometricSolution:Catalog", m_catalog );
      else
         RemoveProperty( properties, "PCL:AstrometricSolution:Catalog" );

      if ( !m_creatorOS.IsEmpty() )
         ModifyProperty( properties, "PCL:AstrometricSolution:CreatorOS", m_creatorOS );
      else
         RemoveProperty( properties, "PCL:AstrometricSolution:CreatorOS" );

      if ( !m_creatorApp.IsEmpty() )
         ModifyProperty( properties, "PCL:AstrometricSolution:CreatorApplication", m_creatorApp );
      else
         RemoveProperty( properties, "PCL:AstrometricSolution:CreatorApplication" );

      if ( !m_creatorModule.IsEmpty() )
         ModifyProperty( properties, "PCL:AstrometricSolution:CreatorModule", m_creatorModule );
      else
         RemoveProperty( properties, "PCL:AstrometricSolution:CreatorModule" );

      const SplineWorldTransformation* S = dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() );
      if ( S != nullptr )
         properties << S->ToProperties();
   }
   else
   {
      RemoveProperty( properties, "PCL:AstrometricSolution:ProjectionSystem" );
      RemoveProperty( properties, "PCL:AstrometricSolution:ReferenceCelestialCoordinates" );
      RemoveProperty( properties, "PCL:AstrometricSolution:ReferenceImageCoordinates" );
      RemoveProperty( properties, "PCL:AstrometricSolution:ReferenceNativeCoordinates" );
      RemoveProperty( properties, "PCL:AstrometricSolution:CelestialPoleNativeCoordinates" );
      RemoveProperty( properties, "PCL:AstrometricSolution:LinearTransformationMatrix" );
      RemoveProperty( properties, "PCL:AstrometricSolution:CreationTime" );
      RemoveProperty( properties, "PCL:AstrometricSolution:Catalog" );
      RemoveProperty( properties, "PCL:AstrometricSolution:CreatorOS" );
      RemoveProperty( properties, "PCL:AstrometricSolution:CreatorApplication" );
      RemoveProperty( properties, "PCL:AstrometricSolution:CreatorModule" );
      RemoveProperty( properties, "PCL:AstrometricSolution:Information" );
   }
}

// ----------------------------------------------------------------------------

PropertyArray AstrometricMetadata::ToProperties() const
{
   PropertyArray properties;

   if ( IsValid() )
   {
      if ( m_focalLength.IsDefined() && m_focalLength() > 0 )
         properties << Property( "Instrument:Telescope:FocalLength", Round( m_focalLength()/1000, 6 ) );

      if ( m_pixelSize.IsDefined() && m_pixelSize() > 0 )
         properties << Property( "Instrument:Sensor:XPixelSize", Round( m_pixelSize(), 3 ) )
                    << Property( "Instrument:Sensor:YPixelSize", Round( m_pixelSize(), 3 ) );

      if ( m_obsStartTime.IsDefined() )
         properties << Property( "Observation:Time:Start", m_obsStartTime() );

      if ( m_obsEndTime.IsDefined() )
         properties << Property( "Observation:Time:End", m_obsEndTime() );

      if ( m_geoLongitude.IsDefined() && m_geoLatitude.IsDefined() )
      {
         properties << Property( "Observation:Location:Longitude", Round( m_geoLongitude(), 6 ) )
                    << Property( "Observation:Location:Latitude", Round( m_geoLatitude(), 6 ) );
         if ( m_geoHeight.IsDefined() )
            properties << Property( "Observation:Location:Elevation", RoundInt( m_geoHeight() ) );
      }

      DPoint pRD;
      if ( ImageToCelestial( pRD, DPoint( 0.5*m_width, 0.5*m_height ) ) )
         properties << Property( "Observation:Center:RA", pRD.x )
                    << Property( "Observation:Center:Dec", pRD.y )
                    << Property( "Observation:CelestialReferenceSystem", ReferenceSystem() )
                    << Property( "Observation:Equinox", 2000.0 );

      LinearTransformation trans_I_W = m_transformWI->ApproximateLinearTransform();
      Matrix LT( trans_I_W.A00(), trans_I_W.A01(),
                 trans_I_W.A10(), trans_I_W.A11() );
      DPoint p0 = trans_I_W.TransformInverse( DPoint( 0 ) );
      properties << Property( "PCL:AstrometricSolution:ProjectionSystem", m_projection->Identifier() )
                 << Property( "PCL:AstrometricSolution:ReferenceCelestialCoordinates", m_projection->ReferenceCelestialCoordinates() )
                 << Property( "PCL:AstrometricSolution:ReferenceImageCoordinates", Vector{ p0.x, p0.y } )
                 << Property( "PCL:AstrometricSolution:ReferenceNativeCoordinates", m_projection->ReferenceNativeCoordinates() )
                 << Property( "PCL:AstrometricSolution:CelestialPoleNativeCoordinates", m_projection->CelestialPoleNativeCoordinates() )
                 << Property( "PCL:AstrometricSolution:LinearTransformationMatrix", LT );

      if ( m_creationTime.IsDefined() )
         properties << Property( "PCL:AstrometricSolution:CreationTime", m_creationTime() );

      if ( !m_catalog.IsEmpty() )
         properties << Property( "PCL:AstrometricSolution:Catalog", m_catalog );

      if ( !m_creatorOS.IsEmpty() )
         properties << Property( "PCL:AstrometricSolution:CreatorOS", m_creatorOS );

      if ( !m_creatorApp.IsEmpty() )
         properties << Property( "PCL:AstrometricSolution:CreatorApplication", m_creatorApp );

      if ( !m_creatorModule.IsEmpty() )
         properties << Property( "PCL:AstrometricSolution:CreatorModule", m_creatorModule );

      const SplineWorldTransformation* S = dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() );
      if ( S != nullptr )
         properties << S->ToProperties();
   }

   return properties;
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::RemoveProperties( PropertyArray& properties, bool removeCenterProperties, bool removeScaleProperties )
{
   if ( removeCenterProperties )
   {
      RemoveProperty( properties, "Observation:Center:RA" );
      RemoveProperty( properties, "Observation:Center:Dec" );
      RemoveProperty( properties, "Observation:Center:X" );
      RemoveProperty( properties, "Observation:Center:Y" );
   }

   if ( removeScaleProperties )
   {
      RemoveProperty( properties, "Instrument:Telescope:FocalLength" );
      RemoveProperty( properties, "Instrument:Sensor:XPixelSize" );
      RemoveProperty( properties, "Instrument:Sensor:YPixelSize" );
   }

   RemoveProperty( properties, "Observation:CelestialReferenceSystem" );
   RemoveProperty( properties, "Observation:Equinox" );
   RemoveProperty( properties, "PCL:AstrometricSolution:ProjectionSystem" );
   RemoveProperty( properties, "PCL:AstrometricSolution:ReferenceCelestialCoordinates" );
   RemoveProperty( properties, "PCL:AstrometricSolution:ReferenceImageCoordinates" );
   RemoveProperty( properties, "PCL:AstrometricSolution:ReferenceNativeCoordinates" );
   RemoveProperty( properties, "PCL:AstrometricSolution:CelestialPoleNativeCoordinates" );
   RemoveProperty( properties, "PCL:AstrometricSolution:LinearTransformationMatrix" );
   RemoveProperty( properties, "PCL:AstrometricSolution:CreationTime" );
   RemoveProperty( properties, "PCL:AstrometricSolution:Catalog" );
   RemoveProperty( properties, "PCL:AstrometricSolution:CreatorOS" );
   RemoveProperty( properties, "PCL:AstrometricSolution:CreatorApplication" );
   RemoveProperty( properties, "PCL:AstrometricSolution:CreatorModule" );
   RemoveProperty( properties, "PCL:AstrometricSolution:Information" );

   RemoveSplineWorldTransformationProperties( properties );
}

void AstrometricMetadata::RemoveProperties( ImageWindow& window, bool removeCenterProperties, bool removeScaleProperties )
{
   View view = window.MainView();

   if ( removeCenterProperties )
   {
      view.DeletePropertyIfExists( "Observation:Center:RA" );
      view.DeletePropertyIfExists( "Observation:Center:Dec" );
      view.DeletePropertyIfExists( "Observation:Center:X" );
      view.DeletePropertyIfExists( "Observation:Center:Y" );
   }

   if ( removeScaleProperties )
   {
      view.DeletePropertyIfExists( "Instrument:Telescope:FocalLength" );
      view.DeletePropertyIfExists( "Instrument:Sensor:XPixelSize" );
      view.DeletePropertyIfExists( "Instrument:Sensor:YPixelSize" );
   }

   view.DeletePropertyIfExists( "Observation:CelestialReferenceSystem" );
   view.DeletePropertyIfExists( "Observation:Equinox" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:ProjectionSystem" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:ReferenceCelestialCoordinates" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:ReferenceImageCoordinates" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:ReferenceNativeCoordinates" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:CelestialPoleNativeCoordinates" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:LinearTransformationMatrix" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:CreationTime" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:Catalog" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:CreatorOS" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:CreatorApplication" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:CreatorModule" );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:Information" );

   RemoveSplineWorldTransformationProperties( window );
}

void AstrometricMetadata::RemoveSplineWorldTransformationProperties( PropertyArray& properties )
{
   const IsoString prefix = SplineWorldTransformation::PropertyPrefix();
   PropertyArray keepProperties;
   for ( const Property& property : properties )
      if ( !property.Id().StartsWith( prefix )
         && property.Id() != "PCL:AstrometricSolution:SplineWorldTransformation" // core <= 1.8.9-2
         && property.Id() != "Transformation_ImageToProjection" )                // core < 1.8.9-2
      {
         keepProperties << property;
      }
   properties = keepProperties;
}

void AstrometricMetadata::RemoveSplineWorldTransformationProperties( ImageWindow& window )
{
   const IsoString prefix = SplineWorldTransformation::PropertyPrefix();
   View view = window.MainView();
   PropertyDescriptionArray properties = view.PropertyDescriptions();
   for ( const PropertyDescription& property : properties )
      if ( property.id.StartsWith( prefix ) )
         view.DeletePropertyIfExists( property.id );
   view.DeletePropertyIfExists( "PCL:AstrometricSolution:SplineWorldTransformation" ); // core <= 1.8.9-2
   view.DeletePropertyIfExists( "Transformation_ImageToProjection" );                  // core < 1.8.9-2
}

void AstrometricMetadata::RemoveObservationLocationProperties( PropertyArray& properties )
{
   PropertyArray keepProperties;
   for ( const Property& property : properties )
      if ( !property.Id().StartsWith( "Observation:Location:" ) )
         keepProperties << property;
   properties = keepProperties;
}

void AstrometricMetadata::RemoveObservationLocationProperties( ImageWindow& window )
{
   View view = window.MainView();
   PropertyDescriptionArray properties = view.PropertyDescriptions();
   for ( const PropertyDescription& property : properties )
      if ( property.id.StartsWith( "Observation:Location:" ) )
         view.DeletePropertyIfExists( property.id );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::RescalePixelSizeProperties( PropertyArray& properties, double scalingFactor )
{
   PropertyArray::iterator i = properties.Search( IsoString( "Instrument:Sensor:XPixelSize" ) );
   if ( i != properties.End() )
      i->SetValue( i->Value().ToDouble()*scalingFactor );
   i = properties.Search( IsoString( "Instrument:Sensor:YPixelSize" ) );
   if ( i != properties.End() )
      i->SetValue( i->Value().ToDouble()*scalingFactor );
}

void AstrometricMetadata::RescalePixelSizeProperties( ImageWindow& window, double scalingFactor )
{
   View view = window.MainView();
   if ( view.HasProperty( "Instrument:Sensor:XPixelSize" ) )
      view.SetPropertyValue( "Instrument:Sensor:XPixelSize",
                             view.PropertyValue( "Instrument:Sensor:XPixelSize" ).ToDouble()*scalingFactor );
   if ( view.HasProperty( "Instrument:Sensor:YPixelSize" ) )
      view.SetPropertyValue( "Instrument:Sensor:YPixelSize",
                             view.PropertyValue( "Instrument:Sensor:YPixelSize" ).ToDouble()*scalingFactor );
}

// ----------------------------------------------------------------------------

WCSKeywords AstrometricMetadata::ComputeWCSKeywords() const
{
   if ( !IsValid() )
      throw Error( "AstrometricMetadata::ComputeWCSKeywords(): Invalid or uninitialized metadata" );

   LinearTransformation trans_F_I( 1,  0,         -0.5,
                                   0, -1, m_height+0.5 );
   LinearTransformation trans_I_W = m_transformWI->ApproximateLinearTransform();
   LinearTransformation trans_F_W = trans_I_W.Multiply( trans_F_I );

   WCSKeywords wcs;
   m_projection->GetWCS( wcs );

   wcs.cd1_1 = trans_F_W.A00();
   wcs.cd1_2 = trans_F_W.A01();
   wcs.cd2_1 = trans_F_W.A10();
   wcs.cd2_2 = trans_F_W.A11();

   DPoint orgF = trans_F_W.TransformInverse( DPoint( 0 ) );
   wcs.crpix1 = orgF.x;
   wcs.crpix2 = orgF.y;

   /*
    * CDELT1, CDELT2 and CROTA2 are computed using the formulas in section 6.2
    * of http://fits.gsfc.nasa.gov/fits_wcs.html "Representations of celestial
    * coordinates in FITS".
    */
   double rot1, rot2;
   if ( wcs.cd2_1() > 0 )
      rot1 = ArcTan( wcs.cd2_1(), wcs.cd1_1() );
   else if ( wcs.cd2_1() < 0 )
      rot1 = ArcTan( -wcs.cd2_1(), -wcs.cd1_1() );
   else
      rot1 = 0;

   if ( wcs.cd1_2() > 0 )
      rot2 = ArcTan( wcs.cd1_2(), -wcs.cd2_2() );
   else if ( wcs.cd1_2() < 0 )
      rot2 = ArcTan( -wcs.cd1_2(), wcs.cd2_2() );
   else
      rot2 = 0;

   double rot = (rot1 + rot2)/2;
   rot2 = rot1 = rot;
   double sinrot, cosrot;
   SinCos( rot, sinrot, cosrot );
   if ( Abs( cosrot ) > Abs( sinrot ) )
   {
      wcs.cdelt1 =  wcs.cd1_1()/cosrot;
      wcs.cdelt2 =  wcs.cd2_2()/cosrot;
   }
   else
   {
      wcs.cdelt1 =  wcs.cd2_1()/sinrot;
      wcs.cdelt2 = -wcs.cd1_2()/sinrot;
   }

   wcs.crota1 = Deg( rot1 );
   wcs.crota2 = Deg( rot2 );

   return wcs;
}

// ----------------------------------------------------------------------------

static String FieldString( double field )
{
   int sign, s1, s2; double s3;
   IsoString::ToSexagesimal( field ).ParseSexagesimal( sign, s1, s2, s3 );
   if ( s1 > 0 )
      return String().Format( "%dd %d' %.1f\"", s1, s2, s3 );
   if ( s2 > 0 )
      return String().Format( "%d' %.1f\"", s2, s3 );
   return String().Format( "%.2f\"", s3 );
}

static String CelestialToString( const DPoint& pRD )
{
   double ra = pRD.x;
   if ( ra < 0 )
      ra += 360;
   return String()
      << "RA: "
      << String().ToSexagesimal( ra/15, RAConversionOptions( 3/*precision*/ ) )
      << "  Dec: "
      << String().ToSexagesimal( pRD.y, DecConversionOptions( 2/*precision*/ ) );
}

static String ImageToCelestialToString( const AstrometricMetadata* A, const DPoint& pI )
{
   DPoint pRD;
   if ( A->ImageToCelestial( pRD, pI ) )
      return CelestialToString( pRD );
   return "------";
}

static String ReprojectionErrorsToString( const DPoint& e )
{
   return String().Format( "  ex: %+9.6f px  ey: %+9.6f px", e.x, e.y );
}

void AstrometricMetadata::UpdateDescription() const
{
   if ( m_description.IsNull() )
      if ( IsValid() )
      {
         LinearTransformation linearIW = m_transformWI->ApproximateLinearTransform();
         DPoint projOrgPx = linearIW.TransformInverse( DPoint( 0 ) );
         DPoint projOrgRD = m_projection->ProjectionOrigin();
         bool flipped;
         double rotation = Rotation( flipped );

         const SplineWorldTransformation* S = dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() );

         m_description = new DescriptionItems;
         m_description->referenceMatrix = linearIW.ToString();
         if ( S != nullptr )
         {
            m_description->wcsTransformationType = S->RBFTypeName();
            m_description->controlPoints = String( S->NumberOfControlPoints() );
            int nxWI, nyWI, nxIW, nyIW;
            S->GetSplineLengths( nxWI, nyWI, nxIW, nyIW );
            m_description->splineLengths = String().Format( "l:%d b:%d X:%d Y:%d", nxWI, nyWI, nxIW, nyIW );
            double exWI, eyWI, exIW, eyIW;
            S->GetSplineErrors( exWI, eyWI, exIW, eyIW );
            exIW *= 3600;
            eyIW *= 3600;
            if ( exWI >= 0.0005 || eyWI >= 0.0005 || exIW >= 0.0005 || eyIW >= 0.0005 )
               m_description->splineErrors = String().Format( "l:%.3f px b:%.3f px X:%.3f as Y:%.3f as", exWI, eyWI, exIW, eyIW );
         }
         else
            m_description->wcsTransformationType = "Linear";

         m_description->projectionName = m_projection->Name();
         m_description->projectionOrigin = String().Format( "[%.6f %.6f] px", projOrgPx.x, projOrgPx.y )
                                             << " -> [" << CelestialToString( projOrgRD ) << ']';
         m_description->resolution = String().Format( "%.3f arcsec/px", m_resolution*3600 );
         m_description->rotation = String().Format( "%.3f deg", rotation ) << (flipped ? " (flipped)" : "");

         m_description->referenceSystem = ReferenceSystem();

         if ( m_obsStartTime.IsDefined() )
            m_description->observationStartTime = m_obsStartTime().ToString( "%Y-%M-%D %h:%m:%s0 UTC" );

         if ( m_obsEndTime.IsDefined() )
            m_description->observationEndTime = m_obsEndTime().ToString( "%Y-%M-%D %h:%m:%s0 UTC" );

         if ( m_geoLongitude.IsDefined() && m_geoLatitude.IsDefined() )
         {
            m_description->observerLocation =
                  String().ToSexagesimal( Abs( m_geoLongitude() ),
                                          SexagesimalConversionOptions( 3/*items*/,
                                                                        0/*precision*/,
                                                                        false/*sign*/,
                                                                        3/*width*/,
                                                                        ' '/*separator*/ ) )
                  << ' ' << ((m_geoLongitude() < 0) ? 'W' : 'E')
                  << "  "
                  << String().ToSexagesimal( Abs( m_geoLatitude() ),
                                          SexagesimalConversionOptions( 3/*items*/,
                                                                        0/*precision*/,
                                                                        false/*sign*/,
                                                                        2/*width*/,
                                                                        ' '/*separator*/ ) )
                  << ' ' << ((m_geoLatitude() < 0) ? 'S' : 'N');

            if ( m_geoHeight.IsDefined() )
               m_description->observerLocation << String().Format( "  %.0f m", m_geoHeight() );
         }

         if ( m_pixelSize.IsDefined() )
            if ( m_pixelSize() > 0 )
               if ( m_focalLength.IsDefined() )
               {
                  m_description->focalDistance = String().Format( "%.2f mm", m_focalLength() );
                  m_description->pixelSize = String().Format( "%.2f um", m_pixelSize() );
               }

         m_description->fieldOfView = FieldString( m_width*m_resolution ) << " x " << FieldString( m_height*m_resolution );
         m_description->centerCoordinates = ImageToCelestialToString( this, DPoint( 0.5*m_width, 0.5*m_height ) );
         m_description->topLeftCoordinates = ImageToCelestialToString( this, DPoint( 0, 0 ) );
         m_description->topRightCoordinates = ImageToCelestialToString( this, DPoint( m_width, 0 ) );
         m_description->bottomLeftCoordinates = ImageToCelestialToString( this, DPoint( 0, m_height ) );
         m_description->bottomRightCoordinates = ImageToCelestialToString( this, DPoint( m_width, m_height ) );

         if ( S != nullptr )
         {
            DPoint e0, e1, e2, e3, e4;
            Verify( e0, e1, e2, e3, e4 );
            m_description->centerCoordinates << ReprojectionErrorsToString( e0 );
            m_description->topLeftCoordinates << ReprojectionErrorsToString( e1 );
            m_description->topRightCoordinates << ReprojectionErrorsToString( e2 );
            m_description->bottomLeftCoordinates << ReprojectionErrorsToString( e3 );
            m_description->bottomRightCoordinates << ReprojectionErrorsToString( e4 );
         }

         m_description->referenceCatalog = m_catalog;

         if ( m_creationTime.IsDefined() )
            m_description->creationTime = m_creationTime().ToString( "%Y-%M-%D %h:%m:%s0 UTC" );

         if ( !m_creatorApp.IsEmpty() )
         {
            m_description->creationSoftware = m_creatorApp;
            if ( !m_creatorModule.IsEmpty() )
               m_description->creationSoftware << " / " << m_creatorModule;
            if ( !m_creatorOS.IsEmpty() )
               m_description->creationSoftware << " (" << m_creatorOS << ")";
         }
      }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/AstrometricMetadata.cpp - Released 2024-12-11T17:42:39Z
