//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// Standard FindingChart Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// FindingChartInstance.cpp - Released 2023-12-01T19:16:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard FindingChart PixInsight module.
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

#include "FindingChartInstance.h"
#include "FindingChartParameters.h"

#include <pcl/AstrometricMetadata.h>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/DisplayFunction.h>
#include <pcl/ErrorHandler.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Graphics.h>
#include <pcl/MetaModule.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

// N.B.: NULLMAG definition coherent with the same macro declared in
// AstronomicalCatalogs.jsh
#define NULLMAG   1000
#define SNULLMAG  "1000"

namespace pcl
{

// ----------------------------------------------------------------------------

FindingChartInstance::FindingChartInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_chartSize( int32( TheFCChartSizeParameter->DefaultValue() ) )
   , p_autoChartResolution( TheFCAutoChartResolutionParameter->DefaultValue() )
   , p_chartResolution( TheFCChartResolutionParameter->DefaultValue() )
   , p_autoChartMaxMagnitude( TheFCAutoChartMaxMagnitudeParameter->DefaultValue() )
   , p_chartMaxMagnitude( TheFCChartMaxMagnitudeParameter->DefaultValue() )
   , p_drawGrid( TheFCDrawGridParameter->DefaultValue() )
   , p_drawConstellationBorders( TheFCDrawConstellationBordersParameter->DefaultValue() )
   , p_drawConstellationLines( TheFCDrawConstellationLinesParameter->DefaultValue() )
   , p_drawConstellationNames( TheFCDrawConstellationNamesParameter->DefaultValue() )
   , p_drawStars( TheFCDrawStarsParameter->DefaultValue() )
   , p_drawStarNames( TheFCDrawStarNamesParameter->DefaultValue() )
   , p_drawMessierObjects( TheFCDrawMessierObjectsParameter->DefaultValue() )
   , p_drawNGCObjects( TheFCDrawNGCObjectsParameter->DefaultValue() )
   , p_generateBitmapFile( TheFCGenerateBitmapFileParameter->DefaultValue() )
   , p_outputSuffix( TheFCOutputSuffixParameter->DefaultValue() )
   , p_closeOutputWindow( TheFCCloseOutputWindowParameter->DefaultValue() )
   , p_imageRegionColor( uint32( TheFCImageRegionColorParameter->DefaultValue() ) )
   , p_imageRegionBorderColor( uint32( TheFCImageRegionBorderColorParameter->DefaultValue() ) )
   , p_gridColor( uint32( TheFCGridColorParameter->DefaultValue() ) )
   , p_gridTextColor( uint32( TheFCGridTextColorParameter->DefaultValue() ) )
   , p_constellationBorderColor( uint32( TheFCConstellationBorderColorParameter->DefaultValue() ) )
   , p_constellationLineColor( uint32( TheFCConstellationLineColorParameter->DefaultValue() ) )
   , p_constellationTextColor( uint32( TheFCConstellationTextColorParameter->DefaultValue() ) )
   , p_starTextColor( uint32( TheFCStarTextColorParameter->DefaultValue() ) )
   , p_messierTextColor( uint32( TheFCMessierTextColorParameter->DefaultValue() ) )
   , p_ngcTextColor( uint32( TheFCNGCTextColorParameter->DefaultValue() ) )
{
}

// ----------------------------------------------------------------------------

FindingChartInstance::FindingChartInstance( const FindingChartInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void FindingChartInstance::Assign( const ProcessImplementation& p )
{
   const FindingChartInstance* x = dynamic_cast<const FindingChartInstance*>( &p );
   if ( x != nullptr )
   {
      p_chartSize = x->p_chartSize;
      p_autoChartResolution = x->p_autoChartResolution;
      p_chartResolution = x->p_chartResolution;
      p_autoChartMaxMagnitude = x->p_autoChartMaxMagnitude;
      p_chartMaxMagnitude = x->p_chartMaxMagnitude;
      p_drawGrid = x->p_drawGrid;
      p_drawConstellationBorders = x->p_drawConstellationBorders;
      p_drawConstellationLines = x->p_drawConstellationLines;
      p_drawConstellationNames = x->p_drawConstellationNames;
      p_drawStars = x->p_drawStars;
      p_drawStarNames = x->p_drawStarNames;
      p_drawMessierObjects = x->p_drawMessierObjects;
      p_drawNGCObjects = x->p_drawNGCObjects;
      p_generateBitmapFile = x->p_generateBitmapFile;
      p_outputDirectory = x->p_outputDirectory;
      p_outputSuffix = x->p_outputSuffix;
      p_closeOutputWindow = x->p_closeOutputWindow;
      p_imageRegionColor = x->p_imageRegionColor;
      p_imageRegionBorderColor = x->p_imageRegionBorderColor;
      p_gridColor = x->p_gridColor;
      p_gridTextColor = x->p_gridTextColor;
      p_constellationBorderColor = x->p_constellationBorderColor;
      p_constellationLineColor = x->p_constellationLineColor;
      p_constellationTextColor = x->p_constellationTextColor;
      p_starTextColor = x->p_starTextColor;
      p_messierTextColor = x->p_messierTextColor;
      p_ngcTextColor = x->p_ngcTextColor;
   }
}

// ----------------------------------------------------------------------------

bool FindingChartInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool FindingChartInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( !view.IsMainView() )
   {
      whyNot = "FindingChart cannot be executed on previews: " + view.FullId();
      return false;
   }

   if ( p_generateBitmapFile )
   {
      String outputDirectory = p_outputDirectory.Trimmed();

      if ( outputDirectory.IsEmpty() )
      {
         whyNot = "Generation of a bitmap file has been enabled, but no output directory has been specified.";
         return false;
      }

      if ( !File::DirectoryExists( outputDirectory ) )
      {
         whyNot = "The specified output directory does not exist: " + outputDirectory;
         return false;
      }
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Predicate class for sorting a set of points in clockwise direction.
 */
class PointsClockwisePredicate
{
public:

   /*
    * Initialize point sorting with respect to the barycenter of the specified
    * set of points.
    */
   PointsClockwisePredicate( const Array<DPoint>& P )
   {
      // Compute the polygon's barycenter
      PCL_PRECONDITION( P.Length() > 2 )
      for ( const DPoint& p : P )
         m_c += p;
      m_c /= P.Length();
   }

   /*
    * Predicate function: Returns true iff the point a precedes the point b in
    * clockwise sorting order.
    */
   bool operator()( const DPoint& a, const DPoint& b ) const
   {
      /*
       * Points on different sides of the vertical line passing through c.
       *
       * - a to the right and b to the left: a < b
       * - a to the left and b to the right: a > b
       */
      if ( a.x >= m_c.x )
      {
         if ( b.x < m_c.x )
            return true;
      }
      else
      {
         if ( b.x >= m_c.x )
            return false;
      }

      /*
       * If the points are not collinear, sort in clockwise direction.
       *
       * d:  > 0 if b is to the left of the line c-a
       *     < 0 if b is to the right of the line c-a
       *    == 0 if b is on the line c-a
       */
      double d = (a.x - m_c.x)*(b.y - m_c.y) - (b.x - m_c.x)*(a.y - m_c.y);
      if ( likely( d != 0 ) )
         return d < 0;

      /*
       * Sort collinear points by their distance to the center.
       */
      double dxa = a.x - m_c.x;
      double dxb = b.x - m_c.x;
      if ( dxa != dxb )
         return Abs( dxa ) < Abs( dxb );
      double dya = a.y - m_c.y;
      double dyb = b.y - m_c.y;
      return Abs( dya ) < Abs( dyb );
   }

private:

   DPoint m_c = 0.0; // reference center point
};

bool FindingChartInstance::ExecuteOn( View& view )
{
   {
      String whyNot;
      if ( !CanExecuteOn( view, whyNot ) )
         throw Error( whyNot );
   }

   Console console;

   AutoViewLock lock( view );

   ImageWindow window = view.Window();

   // Fail if the image has no valid astrometric solution
   AstrometricMetadata A( window );
   if ( !A.IsValid() )
      throw Error( "The image has no valid astrometric solution: " + view.Id() );

   /*
    * Finding chart parameters
    */

   // Resolution in arcsec/px
   float resolution;
   if ( p_autoChartResolution )
   {
      // For narrow fields and moderate wide fields we generate finding charts
      // at 120 as/px. For extreme wide fields we try to generate wider charts
      // adaptively.
      double r = 3600 * A.Resolution() * Max( window.MainView().Width(), window.MainView().Height() ) / p_chartSize;
      resolution = (r < 80) ? 120.0 : r + 50;
      console.NoteLn( String().Format( "* Automatically calculated resolution: %.1f as/px", resolution ) );
   }
   else
      resolution = p_chartResolution;

   // Maximum magnitude for the synthetic star field
   float maxMagnitude;
   if ( p_autoChartMaxMagnitude )
   {
      maxMagnitude = (resolution < 125) ? 8.0 : 7.0;
      console.NoteLn( String().Format( "* Automatically calculated limit magnitude: %.1f", maxMagnitude ) );
   }
   else
      maxMagnitude = p_chartMaxMagnitude;

   // The working synthetic star field window
   ImageWindow starFieldWindow;

   try
   {
      /*
       * Compute image border points in spherical equatorial coordinates.
       */
      DRect bounds = window.MainView().Bounds();
      DPoint C; // center
      Array<DPoint> B( 16u ); // 5*top, 3*left, 5*bottom, 3*right
      if (  !A.ImageToCelestial( C,  bounds.Center() )
         || !A.ImageToCelestial( B[ 0], DPoint( 0.0,                 0.0             ) )
         || !A.ImageToCelestial( B[ 1], DPoint( 0.25*bounds.Width(), 0.0             ) )
         || !A.ImageToCelestial( B[ 2], DPoint( 0.50*bounds.Width(), 0.0             ) )
         || !A.ImageToCelestial( B[ 3], DPoint( 0.75*bounds.Width(), 0.0             ) )
         || !A.ImageToCelestial( B[ 4], DPoint(      bounds.Width(), 0.0             ) )
         || !A.ImageToCelestial( B[ 5], DPoint( 0.0,            0.25*bounds.Height() ) )
         || !A.ImageToCelestial( B[ 6], DPoint( 0.0,            0.50*bounds.Height() ) )
         || !A.ImageToCelestial( B[ 7], DPoint( 0.0,            0.75*bounds.Height() ) )
         || !A.ImageToCelestial( B[ 8], DPoint( 0.0,                 bounds.Height() ) )
         || !A.ImageToCelestial( B[ 9], DPoint( 0.25*bounds.Width(), bounds.Height() ) )
         || !A.ImageToCelestial( B[10], DPoint( 0.50*bounds.Width(), bounds.Height() ) )
         || !A.ImageToCelestial( B[11], DPoint( 0.75*bounds.Width(), bounds.Height() ) )
         || !A.ImageToCelestial( B[12], DPoint(      bounds.Width(), bounds.Height() ) )
         || !A.ImageToCelestial( B[13], DPoint( bounds.Width(), 0.25*bounds.Height() ) )
         || !A.ImageToCelestial( B[14], DPoint( bounds.Width(), 0.50*bounds.Height() ) )
         || !A.ImageToCelestial( B[15], DPoint( bounds.Width(), 0.75*bounds.Height() ) ) )
      {
         return false;
      }

      /*
       * The observation time as a JD
       */
      double observationJD = A.ObservationMiddleTime().OrElse( TimePoint::J2000() ).JD();

      String coreSrcDir = PixInsightSettings::GlobalString( "Application/SrcDirectory" );

      /*
       * Generate the synthetic star field
       */
      StringKeyValueList arguments = StringKeyValueList()
                  << StringKeyValue( "sgen_useImageMetadata", "false" )
                  << StringKeyValue( "sgen_width", String( p_chartSize ) )
                  << StringKeyValue( "sgen_height", String( p_chartSize ) )
                  << StringKeyValue( "sgen_projection", "0" ) // Gnomonic
                  << StringKeyValue( "sgen_ra", String().Format( "%.6f", C.x ) )
                  << StringKeyValue( "sgen_dec", String().Format( "%.6f", C.y ) )
                  << StringKeyValue( "sgen_epoch", String().Format( "%.4f", observationJD ) )
                  << StringKeyValue( "sgen_rotation", "0" )
                  << StringKeyValue( "sgen_resolution", String().Format( "%.6f", resolution/3600 ) )
                  << StringKeyValue( "sgen_xpixsz", "5" )
                  << StringKeyValue( "BrightStars_magMin", SNULLMAG )
                  << StringKeyValue( "BrightStars_magMax", "7" )
                  << StringKeyValue( "BrightStars_magnitudeFilter", "Vmag" )
                  << StringKeyValue( "TYCHO2_magMin", SNULLMAG )
                  << StringKeyValue( "TYCHO2_magMax", SNULLMAG )
                  << StringKeyValue( "TYCHO2_magnitudeFilter", "VTmag" )
                  << StringKeyValue( "sgen_projectionOriginMode", "0" ) // center
                  << StringKeyValue( "sgen_vizierServer", "http://vizier.u-strasbg.fr/" )
                  << StringKeyValue( "sgen_autoGraphics", "true" )
                  << StringKeyValue( "sgen_minMagnitude", "-2" )
                  << StringKeyValue( "sgen_maxMagnitude", String().Format( "%.2f", maxMagnitude ) )
                  << StringKeyValue( "sgen_backgroundMagnitude", "16" )
                  << StringKeyValue( "sgen_PSF", "1" ) // Gaussian
                  << StringKeyValue( "sgen_FWHM", "100" )
                  << StringKeyValue( "sgen_beta", "1.5" )
                  << StringKeyValue( "sgen_generateStars", String( bool( p_drawStars ) ) )
                  << StringKeyValue( "sgen_generateNoise", "false" )
                  << StringKeyValue( "sgen_catalogName", "Bright Stars" )
                  << StringKeyValue( "sgen_catalog2Name", "TYCHO-2" )
                  << StringKeyValue( "sgen_outputImageId", window.MainView().Id() + "_FindingChart" )
                  << StringKeyValue( "non_interactive", "true" );

      console.ExecuteScriptGlobal( coreSrcDir + "/scripts/AdP/CatalogStarGenerator.js", arguments );

      Variant result = Module->EvaluateScript( "__PJSR_AdpCatalogStarGenerator_WindowId" );
      if ( !result.IsValid() )
         throw Error( "Internal error: Invalid star generation script execution" );
      IsoString windowId = result.ToIsoString();
      if ( windowId.IsEmpty() )
         throw Error( "Failure to generate finding chart image" );

      starFieldWindow = ImageWindow::WindowById( windowId );
      if ( starFieldWindow.IsNull() )
         throw Error( "Internal error: Unable to access finding chart image window" );

      ImageVariant starFieldImage = starFieldWindow.MainView().Image();

      /*
       * Apply an automatic histogram stretch
       */
      Vector center( starFieldImage.NumberOfChannels() );
      Vector sigma( starFieldImage.NumberOfChannels() );
      for ( int c = 0; c < starFieldImage.NumberOfChannels(); ++c )
      {
         starFieldImage.SelectChannel( c );
         sigma[c] = 1.4826*starFieldImage.MAD( center[c] = starFieldImage.Median() );
      }
      DisplayFunction D;
      D.SetLinkedRGB( false );
      D.ComputeAutoStretch( sigma, center );
      D >> starFieldImage;

      /*
       * Highlight the image region
       */
      {
         if ( !starFieldWindow.HasAstrometricSolution() )
            throw Error( "Internal error: Invalid finder chart image generation" );

         // Project image border points on the synthetic star field.
         for ( DPoint& b : B )
            if ( !starFieldWindow.CelestialToImage( b.x, b.y ) )
               throw Error( "Internal error: Invalid finder chart image generation" );

         Bitmap bmp( starFieldImage.Width(), starFieldImage.Height() );
         bmp.Fill( 0 ); // transparent
         VectorGraphics G( bmp );
         G.EnableAntialiasing();

         // Draw a dot to indicate the top-left corner of the image
         G.FillCircle( B[0], 4, Brush( p_imageRegionBorderColor ) );
         // Ensure that the image region is represented as a convex polygon
         InsertionSort( B.Begin(), B.End(), PointsClockwisePredicate( B ) );
         // Fill the image region
         G.FillPolygon( B, Brush( p_imageRegionColor ) );
         // Highlight region border
         G.StrokePolygon( B, Pen( p_imageRegionBorderColor ) );

         G.EndPaint();

         starFieldImage.SetColorSpace( ColorSpace::RGB );
         starFieldImage.Blend( bmp, Point( 0 ) );
      }

      /*
       * Annotate the synthetic star field.
       */
      arguments = StringKeyValueList()
                  << StringKeyValue( "engine_vizierServer", "http://cdsarc.u-strasbg.fr/" )
                  << StringKeyValue( "engine_removeDuplicates", "true" )
                  << StringKeyValue( "engine_outputMode", "0" ) // raster
                  << StringKeyValue( "engine_applySTF", "false" )
                  << StringKeyValue( "engine_textScale", "1.5" )
                  << StringKeyValue( "engine_graphicsScale", "1" )
                  << StringKeyValue( "engine_writeObjects", "false" )
                  << StringKeyValue( "engine_optimizeLabelPlacement", "true" )
                  << StringKeyValue( "engine_dropShadow", "false" )
                  << StringKeyValue( "engine_epoch", String().Format( "%.4f", observationJD ) )
                  << StringKeyValue( "engine_topocentric", "false" )
                  << StringKeyValue( "engine_scalingFactor", "1" )
                  // Grid
                  << StringKeyValue( "ly0_visible", String( bool( p_drawGrid ) ) )
                  << StringKeyValue( "ly0_showMarkers", "true" )
                  << StringKeyValue( "ly0_lineColor", String( p_gridColor ) )
                  << StringKeyValue( "ly0_lineWidth", "1" )
                  << StringKeyValue( "ly0_showLabels", "true" )
                  << StringKeyValue( "ly0_labelSize", "12" )
                  << StringKeyValue( "ly0_labelBold", "false" )
                  << StringKeyValue( "ly0_labelItalic", "false" )
                  << StringKeyValue( "ly0_labelColor", String( p_gridTextColor ) )
                  << StringKeyValue( "ly0_labelFace", "DejaVu Sans" )
                  << StringKeyValue( "ly0_labelFields", "|||||||" )
                  << StringKeyValue( "ly0_density", "4" )
                  // Constellation Borders
                  << StringKeyValue( "ly1_visible", String( bool( p_drawConstellationBorders ) ) )
                  << StringKeyValue( "ly1_showMarkers", "true" )
                  << StringKeyValue( "ly1_lineColor", String( p_constellationBorderColor ) )
                  << StringKeyValue( "ly1_lineWidth", "4" )
                  << StringKeyValue( "ly1_showLabels", String( bool( p_drawConstellationNames ) ) )
                  << StringKeyValue( "ly1_labelSize", "18" )
                  << StringKeyValue( "ly1_labelBold", "false" )
                  << StringKeyValue( "ly1_labelItalic", "false" )
                  << StringKeyValue( "ly1_labelColor", String( p_constellationTextColor ) )
                  << StringKeyValue( "ly1_labelFace", "DejaVu Sans" )
                  << StringKeyValue( "ly1_labelFields", "|||||||" )
                  // Constellation Lines
                  << StringKeyValue( "ly2_visible", String( bool( p_drawConstellationLines ) ) )
                  << StringKeyValue( "ly2_showMarkers", "true" )
                  << StringKeyValue( "ly2_lineColor", String( p_constellationLineColor ) )
                  << StringKeyValue( "ly2_lineWidth", "4" )
                  << StringKeyValue( "ly2_showLabels", "false" )
                  << StringKeyValue( "ly2_labelSize", "32" )
                  << StringKeyValue( "ly2_labelBold", "false" )
                  << StringKeyValue( "ly2_labelItalic", "false" )
                  << StringKeyValue( "ly2_labelColor", String( p_constellationLineColor ) )
                  << StringKeyValue( "ly2_labelFace", "DejaVu Sans" )
                  << StringKeyValue( "ly2_labelFields", "|||||||" )
                  << StringKeyValue( "ly2_margin", "8" )
                  // Messier
                  << StringKeyValue( "ly3_visible", String( bool( p_drawMessierObjects ) ) )
                  << StringKeyValue( "ly3_showMarkers", "true" )
                  << StringKeyValue( "ly3_lineColor", String( p_messierTextColor ) )
                  << StringKeyValue( "ly3_lineWidth", "1" )
                  << StringKeyValue( "ly3_showLabels", "true" )
                  << StringKeyValue( "ly3_labelSize", "16" )
                  << StringKeyValue( "ly3_labelBold", "false" )
                  << StringKeyValue( "ly3_labelItalic", "false" )
                  << StringKeyValue( "ly3_labelColor", String( p_messierTextColor ) )
                  << StringKeyValue( "ly3_labelFace", "DejaVu Sans" )
                  << StringKeyValue( "ly3_labelFields", "||||Name|||Common name" )
                  << StringKeyValue( "ly3_maxObjects", "4294967295" )
                  // NamedStars
                  << StringKeyValue( "ly4_visible", String( p_drawStars && p_drawStarNames ) )
                  << StringKeyValue( "ly4_showMarkers", "false" )
                  << StringKeyValue( "ly4_lineColor", String( p_starTextColor ) )
                  << StringKeyValue( "ly4_lineWidth", "1" )
                  << StringKeyValue( "ly4_showLabels", "true" )
                  << StringKeyValue( "ly4_labelSize", "14" )
                  << StringKeyValue( "ly4_labelBold", "false" )
                  << StringKeyValue( "ly4_labelItalic", "false" )
                  << StringKeyValue( "ly4_labelColor", String( p_starTextColor ) )
                  << StringKeyValue( "ly4_labelFace", "DejaVu Sans" )
                  << StringKeyValue( "ly4_labelFields", "||||Common name|||" )
                  << StringKeyValue( "ly4_magMax", "2.0" )
                  << StringKeyValue( "ly4_maxObjects", "4294967295" )
                  // NGC-IC
                  << StringKeyValue( "ly5_visible", String( bool( p_drawNGCObjects ) ) )
                  << StringKeyValue( "ly5_showMarkers", "true" )
                  << StringKeyValue( "ly5_lineColor", String( p_ngcTextColor ) )
                  << StringKeyValue( "ly5_lineWidth", "1" )
                  << StringKeyValue( "ly5_showLabels", "true" )
                  << StringKeyValue( "ly5_labelSize", "16" )
                  << StringKeyValue( "ly5_labelBold", "false" )
                  << StringKeyValue( "ly5_labelItalic", "false" )
                  << StringKeyValue( "ly5_labelColor", String( p_ngcTextColor ) )
                  << StringKeyValue( "ly5_labelFace", "DejaVu Sans" )
                  << StringKeyValue( "ly5_labelFields", "||||Name|||Common name" )
                  << StringKeyValue( "ly5_maxObjects", "4294967295" )
                  //
                  << StringKeyValue( "engine_layers", "Grid|Constellation Borders|Constellation Lines|Messier|NamedStars|NGC-IC" )
                  << StringKeyValue( "non_interactive", "true" );

      Console().ExecuteScriptOn( starFieldWindow.MainView(), coreSrcDir + "/scripts/AdP/AnnotateImage.js", arguments );

      result = Module->EvaluateScript( "__PJSR_AdpAnnotateImage_SuccessCount" );
      if ( !result.IsValid() )
         throw Error( "Internal error: Invalid annotation script execution" );
      int successCount = result.ToInt();
      if ( successCount != 1 )
         throw Error( "Failure to annotate finder chart image" );

      if ( p_generateBitmapFile )
      {
         /*
          * If requested, generate an output bitmap file.
          */
         String outputDirectory = p_outputDirectory.Trimmed();
         if ( !outputDirectory.EndsWith( '/' ) )
            outputDirectory << '/';

         String fileName;
         {
            String srcFilePath = window.FilePath();
            if ( srcFilePath.IsEmpty() )
               fileName = window.MainView().Id();
            else
               fileName = File::ExtractName( srcFilePath );
         }

         Bitmap::Render( starFieldImage ).Save( outputDirectory + fileName + p_outputSuffix + ".png" );

         if ( p_closeOutputWindow )
            starFieldWindow.ForceClose();
         else
            starFieldWindow.Show();
      }
      else
         starFieldWindow.Show();

      return true;
   }
   catch ( ... )
   {
      if ( !starFieldWindow.IsNull() )
         starFieldWindow.ForceClose();
      throw;
   }
}

// ----------------------------------------------------------------------------

void* FindingChartInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheFCChartSizeParameter )
      return &p_chartSize;
   if ( p == TheFCAutoChartResolutionParameter )
      return &p_autoChartResolution;
   if ( p == TheFCChartResolutionParameter )
      return &p_chartResolution;
   if ( p == TheFCAutoChartMaxMagnitudeParameter )
      return &p_autoChartMaxMagnitude;
   if ( p == TheFCChartMaxMagnitudeParameter )
      return &p_chartMaxMagnitude;
   if ( p == TheFCDrawGridParameter )
      return &p_drawGrid;
   if ( p == TheFCDrawConstellationBordersParameter )
      return &p_drawConstellationBorders;
   if ( p == TheFCDrawConstellationLinesParameter )
      return &p_drawConstellationLines;
   if ( p == TheFCDrawConstellationNamesParameter )
      return &p_drawConstellationNames;
   if ( p == TheFCDrawStarsParameter )
      return &p_drawStars;
   if ( p == TheFCDrawStarNamesParameter )
      return &p_drawStarNames;
   if ( p == TheFCDrawMessierObjectsParameter )
      return &p_drawMessierObjects;
   if ( p == TheFCDrawNGCObjectsParameter )
      return &p_drawNGCObjects;
   if ( p == TheFCGenerateBitmapFileParameter )
      return &p_generateBitmapFile;
   if ( p == TheFCOutputDirectoryParameter )
      return p_outputDirectory.Begin();
   if ( p == TheFCOutputSuffixParameter )
      return p_outputSuffix.Begin();
   if ( p == TheFCCloseOutputWindowParameter )
      return &p_closeOutputWindow;
   if ( p == TheFCImageRegionColorParameter )
      return &p_imageRegionColor;
   if ( p == TheFCImageRegionBorderColorParameter )
      return &p_imageRegionBorderColor;
   if ( p == TheFCGridColorParameter )
      return &p_gridColor;
   if ( p == TheFCGridTextColorParameter )
      return &p_gridTextColor;
   if ( p == TheFCConstellationBorderColorParameter )
      return &p_constellationBorderColor;
   if ( p == TheFCConstellationLineColorParameter )
      return &p_constellationLineColor;
   if ( p == TheFCConstellationTextColorParameter )
      return &p_constellationTextColor;
   if ( p == TheFCStarTextColorParameter )
      return &p_starTextColor;
   if ( p == TheFCMessierTextColorParameter )
      return &p_messierTextColor;
   if ( p == TheFCNGCTextColorParameter )
      return &p_ngcTextColor;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool FindingChartInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheFCOutputDirectoryParameter )
   {
      p_outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_outputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheFCOutputSuffixParameter )
   {
      p_outputSuffix.Clear();
      if ( sizeOrLength > 0 )
         p_outputSuffix.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type FindingChartInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheFCOutputDirectoryParameter )
      return p_outputDirectory.Length();
   if ( p == TheFCOutputSuffixParameter )
      return p_outputSuffix.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FindingChartInstance.cpp - Released 2023-12-01T19:16:17Z
