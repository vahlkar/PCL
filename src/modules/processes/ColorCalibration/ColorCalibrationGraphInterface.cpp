//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.9
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.5
// ----------------------------------------------------------------------------
// ColorCalibrationGraphInterface.cpp - Released 2024-03-20T10:42:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "ColorCalibrationGraphInterface.h"
#include "PhotometricColorCalibrationProcess.h"

#include <pcl/Console.h>
#include <pcl/FileDialog.h>
#include <pcl/LinearFit.h>
#include <pcl/MetaModule.h>
#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ColorCalibrationGraphInterface::ColorCalibrationGraphInterface()
{
   DisableAutoSaveGeometry();
}

// ----------------------------------------------------------------------------

ColorCalibrationGraphInterface::~ColorCalibrationGraphInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString ColorCalibrationGraphInterface::Id() const
{
   return Process()->Id() + "Graph";
}

// ----------------------------------------------------------------------------

String ColorCalibrationGraphInterface::IconImageSVGFile() const
{
   return Process()->IconImageSVGFile();
}

// ----------------------------------------------------------------------------

InterfaceFeatures ColorCalibrationGraphInterface::Features() const
{
   return InterfaceFeature::None;
}

// ----------------------------------------------------------------------------

bool ColorCalibrationGraphInterface::IsInstanceGenerator() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool ColorCalibrationGraphInterface::CanImportInstances() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool ColorCalibrationGraphInterface::Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( Process()->Id() );
   }

   dynamic = false;
   return true;
}

// ----------------------------------------------------------------------------

struct DataPoint
{
   double x, y, z;

   bool operator ==( const DataPoint& p ) const
   {
      return x == p.x;
   }

   bool operator <( const DataPoint& p ) const
   {
      return x < p.x;
   }
};

typedef GenericVector<DataPoint> DataPointVector;

void ColorCalibrationGraphInterface::UpdateGraphs(
                           const String& viewId, const String& catalogName,
                           const String& filterNameR, const String& filterNameG, const String& filterNameB,
                           const String& qeCurveName, const String& whiteRefName,
                           const Vector& catRG, const Vector& imgRG, const LinearFit& fitRG,
                           const Vector& catBG, const Vector& imgBG, const LinearFit& fitBG,
                           const Vector& wbFactors,
                           float catWR, float catWB )
{
   if ( GUI == nullptr )
      return;

   String RGGraph, RGGraphDiv, RGFunctionInfo;
   if ( fitRG.IsValid() && !fitRG.IsXAxis() )
   {
      DataPointVector PRG( catRG.Length() );
      for ( int i = 0; i < catRG.Length(); ++i )
      {
         PRG[i].x = catRG[i];
         PRG[i].y = imgRG[i];
         PRG[i].z = fitRG( catRG[i] );
      }
      PRG.Sort();

      String RGFunc = InMagnitudeUnits() ? "R-G" : "R/G";
      String RGLabel = InMagnitudeUnits() ? "R-G (mag)" : "R/G (flux)";
      String RGLabels = "labels: [ 'X', 'Y', 'LRG(X)' ]";
      String RGData = "[\n";
      for ( int i = 0;; )
      {
         RGData << "   [" << String().Format( "%g", PRG[i].x )
               << ',' << String().Format( "%g", PRG[i].y )
               << ',' << String().Format( "%g", PRG[i].z ) << ']';
         if ( ++i == catRG.Length() )
            break;
         RGData << ",\n";
      }
      RGData << "\n]";

      String RWCallback = String().Format(
         "         underlayCallback: function( ctx, area, dygraph )\n"
         "         {\n"
         "            let h0 = dygraph.toDomCoords( %g, %g );\n"
         "            let h1 = dygraph.toDomCoords( %g, %g );\n"
         "            let v0 = dygraph.toDomCoords( %g, %g );\n"
         "            let v1 = dygraph.toDomCoords( %g, %g );\n"
         "            ctx.save();\n"
         "            ctx.strokeStyle = 'black';\n"
         "            ctx.beginPath();\n"
         "            ctx.moveTo( -100000, h0[1] );\n"
         "            ctx.lineTo( +100000, h1[1] );\n"
         "            ctx.moveTo( v0[0], -100000 );\n"
         "            ctx.lineTo( v1[0], +100000 );\n"
         "            ctx.closePath();\n"
         "            ctx.stroke();\n"
         "            ctx.beginPath();\n"
         "            ctx.arc( v0[0], h0[1], 6, 0, 2*Math.PI );\n"
         "            ctx.closePath();\n"
         "            ctx.stroke();\n"
         "            ctx.restore();\n"
         "         },\n",
         catRG.MinComponent(), fitRG( catWR ), catRG.MaxComponent(), fitRG( catWR ),
         catWR, imgRG.MinComponent(), catWR, imgRG.MaxComponent() );

      RGGraph =
         "   var gRG = new Dygraph(\n"
         "      document.getElementById( 'graphRG' ),\n" +
         RGData + ",\n"
         "      {\n"
         "         " + RGLabels + ",\n"
         "         xlabel: 'Catalog " + RGLabel + "',\n"
         "         ylabel: 'Image " + RGLabel + "',\n"
         "         xLabelHeight: 15,\n"
         "         yLabelWidth: 15,\n"
         "         axisLabelFontSize: 10,\n"
         + RWCallback +
         "         series: { 'Y': { strokeWidth: 0, drawPoints: true, pointSize: 1.5, highlightCircleSize: 4 }, "
                            "'LRG(X)': { strokeWidth: 1.0, drawPoints: false } }\n"
         "      }\n"
         "   );\n";

      RGGraphDiv =
         "<div id=\"graphRG\" style=\"margin-top:1.2em; width:100%;\"></div>\n";

      RGFunctionInfo =
         RGFunc + " linear fit .......... " + String().Format( "y = +%.6f&middot;x %c %.6f, &sigma; = %.6f\n",
                                                               fitRG.b, (fitRG.a < 0) ? '-' : '+', Abs( fitRG.a ), fitRG.adev ) +
         RGFunc + " white point ......... " + String().Format( "x = %.6f, y = %.6f\n", catWR, fitRG( catWR ) );
   }

   String BGGraph, BGGraphDiv, BGFunctionInfo;
   if ( fitBG.IsValid() && !fitBG.IsXAxis() )
   {
      DataPointVector PBG( catBG.Length() );
      for ( int i = 0; i < catBG.Length(); ++i )
      {
         PBG[i].x = catBG[i];
         PBG[i].y = imgBG[i];
         PBG[i].z = fitBG( catBG[i] );
      }
      PBG.Sort();

      String BGFunc = InMagnitudeUnits() ? "B-G" : "B/G";
      String BGLabel = InMagnitudeUnits() ? "B-G (mag)" : "B/G (flux)";
      String BGLabels = "labels: [ 'X', 'Y', 'LBG(X)' ]";
      String BGData = "[\n";
      for ( int i = 0;; )
      {
         BGData << "   [" << String().Format( "%g", PBG[i].x )
               << ',' << String().Format( "%g", PBG[i].y )
               << ',' << String().Format( "%g", PBG[i].z ) << ']';
         if ( ++i == catBG.Length() )
            break;
         BGData << ",\n";
      }
      BGData << "\n]";

      String BWCallback = String().Format(
         "         underlayCallback: function( ctx, area, dygraph )\n"
         "         {\n"
         "            let h0 = dygraph.toDomCoords( %g, %g );\n"
         "            let h1 = dygraph.toDomCoords( %g, %g );\n"
         "            let v0 = dygraph.toDomCoords( %g, %g );\n"
         "            let v1 = dygraph.toDomCoords( %g, %g );\n"
         "            ctx.save();\n"
         "            ctx.strokeStyle = 'black';\n"
         "            ctx.beginPath();\n"
         "            ctx.moveTo( -100000, h0[1] );\n"
         "            ctx.lineTo( +100000, h1[1] );\n"
         "            ctx.moveTo( v0[0], -100000 );\n"
         "            ctx.lineTo( v1[0], +100000 );\n"
         "            ctx.closePath();\n"
         "            ctx.stroke();\n"
         "            ctx.beginPath();\n"
         "            ctx.arc( v0[0], h0[1], 6, 0, 2*Math.PI );\n"
         "            ctx.closePath();\n"
         "            ctx.stroke();\n"
         "            ctx.restore();\n"
         "         },\n",
         catBG.MinComponent(), fitBG( catWB ), catBG.MaxComponent(), fitBG( catWB ),
         catWB, imgBG.MinComponent(), catWB, imgBG.MaxComponent() );

      BGGraph =
         "   var gBG = new Dygraph(\n"
         "      document.getElementById( 'graphBG' ),\n" +
         BGData + ",\n" +
         "      {\n"
         "         " + BGLabels + ",\n"
         "         xlabel: 'Catalog " + BGLabel + "',\n"
         "         ylabel: 'Image " + BGLabel + "',\n"
         "         xLabelHeight: 15,\n"
         "         yLabelWidth: 15,\n"
         "         axisLabelFontSize: 10,\n"
         + BWCallback +
         "         series: { 'Y': { strokeWidth: 0, drawPoints: true, pointSize: 1.5, highlightCircleSize: 4 }, "
                            "'LBG(X)': { strokeWidth: 1.0, drawPoints: false } }\n"
         "      }\n"
         "   );\n";

      BGGraphDiv =
         "<div id=\"graphBG\" style=\"margin-top:1.2em; width:100%;\"></div>\n";

      BGFunctionInfo =
         BGFunc + " linear fit .......... " + String().Format( "y = +%.6f&middot;x %c %.6f, &sigma; = %.6f\n",
                                                               fitBG.b, (fitBG.a < 0) ? '-' : '+', Abs( fitBG.a ), fitBG.adev ) +
         BGFunc + " white point ......... " + String().Format( "x = %.6f, y = %.6f\n", catWB, fitBG( catWB ) );
   }

   String coreSrcDir = PixInsightSettings::GlobalString ( "Application/SrcDirectory" );
   String whatWeAreGraphing = InMagnitudeUnits() ? "indices" : "ratios";

   String html =
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<meta charset=\"utf-8\">\n"
"<script type=\"text/javascript\" src=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph.js" ) + "\"></script>\n"
"<script type=\"text/javascript\" src=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph-extra.js" ) + "\"></script>\n"
"<link rel=\"stylesheet\" href=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph-doc.css" ) + "\"/>\n"
"<link rel=\"stylesheet\" href=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph.css" ) + "\"/>\n"
"<style>\n"
".dygraph-legend {\n"
"  left: 70px !important;\n"
"  width: 400px !important;\n"
"  background-color: rgba(230, 230, 230, 1.0) !important;\n"
"  font-size: 11px !important;\n"
"}\n"
"</style>\n"
"<script type=\"text/javascript\">\n"
"function toggleDisplay( id )\n"
"{\n"
"   var obj = document.getElementById( id );\n"
"   if ( obj )\n"
"      obj.style.display = obj.style.display ? \"\" : \"none\";\n"
"}\n"
"</script>\n"
"</head>\n"
"<body>\n"
"<div id=\"Text\">\n"
"<h3 style=\"font-size:18px; margin-top:0; margin-bottom:0;\">White Balance Functions</h3>\n"
"<pre style=\"font-size:10px; line-height:1.2em;\">\n"
   "Image ................... " + viewId + '\n' +
   "Catalog ................. " + catalogName + '\n' +
   "Red filter .............. " + filterNameR + '\n' +
   "Green filter ............ " + filterNameG + '\n' +
   "Blue filter ............. " + filterNameB + '\n' +
   "QE curve ................ " + qeCurveName + '\n' +
   "White reference ......... " + whiteRefName + '\n' +
   "Total sources ........... " + String( catRG.Length() ) + '\n' +
RGFunctionInfo +
BGFunctionInfo +
   "White balance factors ... " + String().Format( "%.4f %.4f %.4f\n", wbFactors[0], wbFactors[1], wbFactors[2] ) +
   "Date processed .......... " + TimePoint::Now().ToString( "%Y-%M-%D %h:%m:%s UTC\n" ) +
"</pre>\n"
"</div>\n" // Text
"<div id=\"Help\" style=\"display:none;\">\n"
"<hr/>\n"
"<p>These graphs show the white balance functions fitted by the " + Process()->Id() + " process for the "
   "specified image and catalog.</p>\n"
"<p>On each graph, the X axis corresponds to color " + whatWeAreGraphing + " found in the catalog, while "
   "the Y axis represents color " + whatWeAreGraphing + " calculated from image pixel values. Each dot "
   "corresponds to a measured star, and the straight line is the white balance function fitted by the process. "
   "The horizontal and vertical black lines and the circles centered on the fitted linear functions represent "
   "the white points calculated for the red and blue channels.</p>\n"
"<p>Outlier stars can be easily identified as dots that depart considerably from the fitted linear function. "
   "The more grouped all dots are around the fitted line the better, but as you'll see, the implemented linear "
   "fitting routines have been designed to be very robust and resilient to outliers, so these cannot degrade "
   "the quality of the color calibration process under normal working conditions.</p>\n"
"<p>The graphs are interactive:</p>\n"
"<ul>\n"
"<li>Mouse over to highlight individual values for measured stars.</li>\n"
"<li>Click and drag to zoom. You can zoom both vertically and horizontally.</li>\n"
"<li>Double-click to zoom out.</li>\n"
"<li>On a zoomed graph, shift-drag to pan.</li>\n"
"</ul>\n"
"<hr/>\n"
"</div>\n" + // Help
RGGraphDiv +
BGGraphDiv +
"<script type=\"text/javascript\">\n" +
RGGraph +
BGGraph +
"</script>\n"
"</body>\n"
"</html>\n";

   String filePath = File::UniqueFileName( File::SystemTempDirectory(), 12, "CC_graph_", ".html" );
   File::WriteTextFile( filePath, html.ToUTF8() );
   m_htmlFiles << filePath;

   GUI->Graph_WebView.LoadContent( File::FileURI( filePath ) );
}

// ----------------------------------------------------------------------------

void ColorCalibrationGraphInterface::ExportPDF() const
{
   SaveFileDialog d;
   d.SetCaption( Process()->Id() + ": Export PDF File" );
   d.SetFilter( FileFilter( "PDF", ".pdf" ) );
   d.EnableOverwritePrompt();
   if ( d.Execute() )
   {
      String filePath = d.FileName();
      GUI->Graph_WebView.SaveAsPDF( filePath );
      Console console;
      console.Show();
      console.NoteLn( "<end><cbr><br>* " + Process()->Id() + ": Graphs exported to PDF document: <raw>" + filePath + "</raw><end>" );
      Module->ProcessEvents();
   }
}

// ----------------------------------------------------------------------------

void ColorCalibrationGraphInterface::ToggleText() const
{
   GUI->Graph_WebView.EvaluateScript( "toggleDisplay( 'Text' );" );
}

// ----------------------------------------------------------------------------

void ColorCalibrationGraphInterface::ToggleHelp() const
{
   GUI->Graph_WebView.EvaluateScript( "toggleDisplay( 'Help' );" );
}

// ----------------------------------------------------------------------------

void ColorCalibrationGraphInterface::CleanUp()
{
   for ( const String& filePath : m_htmlFiles )
      if ( !filePath.IsEmpty() )
      {
         try
         {
            if ( File::Exists( filePath ) )
               File::Remove( filePath );
         }
         catch ( ... )
         {
         }
      }
   m_htmlFiles.Clear();
}

// ----------------------------------------------------------------------------

void ColorCalibrationGraphInterface::e_LoadFinished( WebView& sender, bool state )
{
   /*
    * WebView contents are represented in physical pixels by default. The
    * following call ensures a representation in logical pixels on high-dpi
    * screens. Under non high-dpi screen resolutions, as well as on desktops
    * that use logical pixels by default (macOS), this call is a no-op.
    *
    * ### N.B. Since version 1.8.6, the core already scales WebView zoom
    * factors by the ratio between physical and logical pixels automatically,
    * so 1.0 will be transformed to 1.0*DisplayPixelRatio() internally.
    */
   GUI->Graph_WebView.SetZoomFactor( 1.0 );
}

// ----------------------------------------------------------------------------

void ColorCalibrationGraphInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->ExportPDF_ToolButton )
      ExportPDF();
   else if ( sender == GUI->ToggleText_ToolButton )
      ToggleText();
   else if ( sender == GUI->ToggleHelp_ToolButton )
      ToggleHelp();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ColorCalibrationGraphInterface::GUIData::GUIData( ColorCalibrationGraphInterface& w )
{
   ExportPDF_ToolButton.SetIcon( w.ScaledResource( ":/script-editor/export-pdf.png" ) );
   ExportPDF_ToolButton.SetScaledFixedSize( 22, 22 );
   ExportPDF_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExportPDF_ToolButton.SetToolTip( "<p>Export the graphs as a PDF document.</p>" );
   ExportPDF_ToolButton.OnClick( (Button::click_event_handler)&ColorCalibrationGraphInterface::e_Click, w );

   ToggleText_ToolButton.SetIcon( w.ScaledResource( ":/icons/window-text.png" ) );
   ToggleText_ToolButton.SetScaledFixedSize( 22, 22 );
   ToggleText_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ToggleText_ToolButton.SetToolTip( "<p>Show/hide the text header.</p>" );
   ToggleText_ToolButton.OnClick( (Button::click_event_handler)&ColorCalibrationGraphInterface::e_Click, w );

   ToggleHelp_ToolButton.SetIcon( w.ScaledResource( ":/icons/help.png" ) );
   ToggleHelp_ToolButton.SetScaledFixedSize( 22, 22 );
   ToggleHelp_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ToggleHelp_ToolButton.SetToolTip( "<p>Show/hide help on white balance function graphs.</p>" );
   ToggleHelp_ToolButton.OnClick( (Button::click_event_handler)&ColorCalibrationGraphInterface::e_Click, w );

   Buttons_Sizer.SetMargin( 4 );
   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( ExportPDF_ToolButton );
   Buttons_Sizer.Add( ToggleText_ToolButton );
   Buttons_Sizer.Add( ToggleHelp_ToolButton );

   Graph_WebView.OnLoadFinished( (WebView::state_event_handler)&ColorCalibrationGraphInterface::e_LoadFinished, w );
   Graph_WebView.SetStyleSheet( w.ScaledStyleSheet( "pi--WebView { min-width: 508px; min-height: 908px; }" ) );

   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );
   Global_Sizer.Add( Graph_WebView, 100 );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
//    w.SetMinSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorCalibrationGraphInterface.cpp - Released 2024-03-20T10:42:12Z
