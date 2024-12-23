//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 2.2.8
// ----------------------------------------------------------------------------
// SpectrophotometricFluxCalibrationGraphInterface.cpp - Released 2024-12-23T11:33:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "SpectrophotometricFluxCalibrationGraphInterface.h"
#include "SpectrophotometricFluxCalibrationProcess.h"

#include <pcl/Console.h>
#include <pcl/FileDialog.h>
#include <pcl/MetaModule.h>
#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SpectrophotometricFluxCalibrationGraphInterface* TheSpectrophotometricFluxCalibrationGraphInterface = nullptr;

// ----------------------------------------------------------------------------

SpectrophotometricFluxCalibrationGraphInterface::SpectrophotometricFluxCalibrationGraphInterface()
{
   TheSpectrophotometricFluxCalibrationGraphInterface = this;
   DisableAutoSaveGeometry();
}

// ----------------------------------------------------------------------------

SpectrophotometricFluxCalibrationGraphInterface::~SpectrophotometricFluxCalibrationGraphInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
   if ( this == TheSpectrophotometricFluxCalibrationGraphInterface )
      TheSpectrophotometricFluxCalibrationGraphInterface = nullptr;
}

// ----------------------------------------------------------------------------

IsoString SpectrophotometricFluxCalibrationGraphInterface::Id() const
{
   return Process()->Id() + "Graph";
}

// ----------------------------------------------------------------------------

String SpectrophotometricFluxCalibrationGraphInterface::IconImageSVGFile() const
{
   return Process()->IconImageSVGFile();
}

// ----------------------------------------------------------------------------

InterfaceFeatures SpectrophotometricFluxCalibrationGraphInterface::Features() const
{
   return InterfaceFeature::None;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricFluxCalibrationGraphInterface::IsInstanceGenerator() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricFluxCalibrationGraphInterface::CanImportInstances() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricFluxCalibrationGraphInterface::Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
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

MetaProcess* SpectrophotometricFluxCalibrationGraphInterface::Process() const
{
   return TheSpectrophotometricFluxCalibrationProcess;
}

// ----------------------------------------------------------------------------

static
String MakeData( const Vector& f )
{
   String data = "[\n";
   for ( int i = 0;; )
   {
      data << String().Format( "[%d,%.6e]", i, f[i] );
      if ( ++i == f.Length() )
         break;
      data << ",\n";
   }
   data << "\n]";
   return data;
}

static
String MakeCallback( const Vector& f, double y, double sigma )
{
   return String().Format(
      "underlayCallback: function( ctx, area, dygraph )\n"
      "{\n"
      "   let h0 = dygraph.toDomCoords( 0, %g );\n"
      "   let h1 = dygraph.toDomCoords( %d, %g );\n"
      "   let s0 = dygraph.toDomCoords( 0, %g );\n"
      "   let s1 = dygraph.toDomCoords( %d, %g );\n"
      "   ctx.save();\n"
      "   ctx.strokeStyle = 'black';\n"
      "   ctx.beginPath();\n"
      "   ctx.moveTo( -1e6, h0[1] );\n"
      "   ctx.lineTo( +1e6, h1[1] );\n"
      "   ctx.fillStyle = 'rgba(0, 0, 0, 0.1)';\n"
      "   ctx.fillRect( -1e6, s0[1], +2e6, s1[1]-s0[1] );\n"
      "   ctx.strokeStyle = 'gray';\n"
      "   ctx.setLineDash([5, 3]); /* dashes=5px spaces=3px */\n"
      "   ctx.strokeRect( -1e6, s0[1], +2e6, s1[1]-s0[1] );\n"
      "   ctx.closePath();\n"
      "   ctx.stroke();\n"
      "   ctx.restore();\n"
      "},\n",
      y, f.Length()-1, y, y-sigma, f.Length()-1, y+sigma );
}

void SpectrophotometricFluxCalibrationGraphInterface::UpdateGraphs(
               const String& viewId, const String& catalogName,
               const String& filterNameK, const String& filterNameR, const String& filterNameG, const String& filterNameB,
               const String& qeCurveName,
               const Vector& samplesRK, const Vector& samplesG, const Vector& samplesB,
               const Vector& scaleFactors, const Vector& sigmas )
{
   if ( GUI == nullptr )
      return;

   String filtersInfo, sourcesInfo, scaleFactorsInfo, graphs, divs;

   if ( scaleFactors.Length() < 3 ) // if grayscale
   {
      filtersInfo =
         "Filter .................. " + filterNameK;

      sourcesInfo = String().Format(
         "Total sources ........... %d", samplesRK.Length() );

      scaleFactorsInfo = String().Format(
         "Scale factor ............ %.4e\n"
         "Dispersion .............. %.4e", scaleFactors[0], sigmas[0] );

      graphs =
         "   var gK = new Dygraph(\n"
         "      document.getElementById( 'graphK' ),\n" +
         MakeData( samplesRK ) + ",\n"
         "      {\n"
         "         labels: [ 'X', 'scale' ],\n"
         "         xlabel: '',\n"
         "         ylabel: 'scale',\n"
         "         xLabelHeight: 15,\n"
         "         yLabelWidth: 15,\n"
         "         axisLabelFontSize: 10,\n" +
         MakeCallback( samplesRK, scaleFactors[0], sigmas[0] ) +
         "         series: { 'Y': { strokeWidth: 0, drawPoints: true, pointSize: 1.5, highlightCircleSize: 4 } }\n"
         "      }\n"
         "   );";

      divs =
         "<div id=\"graphK\" style=\"margin-top:1.2em; width:100%;\"></div>";
   }
   else
   {
      filtersInfo =
         "Red filter .............. " + filterNameR + '\n' +
         "Green filter ............ " + filterNameG + '\n' +
         "Blue filter ............. " + filterNameB;

      sourcesInfo = String().Format(
         "Total sources ........... R:%d G:%d B:%d", samplesRK.Length(), samplesG.Length(), samplesB.Length() );

      scaleFactorsInfo = String().Format(
         "Scale factors ........... R:%.4e G:%.4e B:%.4e\n"
         "Dispersions ............. R:%.4e G:%.4e B:%.4e",
         scaleFactors[0], scaleFactors[1], scaleFactors[2], sigmas[0], sigmas[1], sigmas[2] );

      graphs =
         "   var gR = new Dygraph(\n"
         "      document.getElementById( 'graphR' ),\n" +
         MakeData( samplesRK ) + ",\n"
         "      {\n"
         "         labels: [ 'X', 'scale R' ],\n"
         "         xlabel: '',\n"
         "         ylabel: 'scale R',\n"
         "         xLabelHeight: 15,\n"
         "         yLabelWidth: 15,\n"
         "         axisLabelFontSize: 10,\n" +
         MakeCallback( samplesRK, scaleFactors[0], sigmas[0] ) +
         "         series: { 'Y': { strokeWidth: 0, drawPoints: true, pointSize: 1.5, highlightCircleSize: 4 } }\n"
         "      }\n"
         "   );\n"
         "   var gG = new Dygraph(\n"
         "      document.getElementById( 'graphG' ),\n" +
         MakeData( samplesG ) + ",\n"
         "      {\n"
         "         labels: [ 'X', 'scale G' ],\n"
         "         xlabel: '',\n"
         "         ylabel: 'scale G',\n"
         "         xLabelHeight: 15,\n"
         "         yLabelWidth: 15,\n"
         "         axisLabelFontSize: 10,\n" +
         MakeCallback( samplesG, scaleFactors[1], sigmas[1] ) +
         "         series: { 'Y': { strokeWidth: 0, drawPoints: true, pointSize: 1.5, highlightCircleSize: 4 } }\n"
         "      }\n"
         "   );\n"
         "   var gB = new Dygraph(\n"
         "      document.getElementById( 'graphB' ),\n" +
         MakeData( samplesB ) + ",\n"
         "      {\n"
         "         labels: [ 'X', 'scale B' ],\n"
         "         xlabel: '',\n"
         "         ylabel: 'scale B',\n"
         "         xLabelHeight: 15,\n"
         "         yLabelWidth: 15,\n"
         "         axisLabelFontSize: 10,\n" +
         MakeCallback( samplesB, scaleFactors[2], sigmas[2] ) +
         "         series: { 'Y': { strokeWidth: 0, drawPoints: true, pointSize: 1.5, highlightCircleSize: 4 } }\n"
         "      }\n"
         "   );";

      divs =
         "<div id=\"graphR\" style=\"margin-top:1.2em; width:100%; height:220px;\"></div>\n"
         "<div id=\"graphG\" style=\"margin-top:1.2em; width:100%; height:220px;\"></div>\n"
         "<div id=\"graphB\" style=\"margin-top:1.2em; width:100%; height:220px;\"></div>";
   }

   String coreSrcDir = PixInsightSettings::GlobalString ( "Application/SrcDirectory" );

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
"<h3 style=\"font-size:18px; margin-top:0; margin-bottom:0;\">Flux Calibration Functions</h3>\n"
"<pre style=\"font-size:10px; line-height:1.2em;\">\n"
   "Image ................... " + viewId + '\n' +
   "Catalog ................. " + catalogName + '\n' +
   filtersInfo + '\n' +
   "QE curve ................ " + qeCurveName + '\n' +
   sourcesInfo + '\n' +
   scaleFactorsInfo + '\n' +
   "Date processed .......... " + TimePoint::Now().ToString( "%Y-%M-%D %h:%m:%s UTC\n" ) +
"</pre>\n"
"</div>\n" // Text
"<div id=\"Help\" style=\"display:none;\">\n"
"<hr/>\n"
"<p>These graphs show the scale factor samples acquired by the " + Process()->Id() + " process for the "
   "specified image and catalog.</p>\n"
"<p>Each dot in these graphs corresponds to a measured star. The X-axis corresponds to the catalog source index, "
   "and the Y-axis represents scale factors calculated using photometry from image pixel values and integrated "
   "spectral flux from catalog data. The horizontal thick lines represent the estimated scale factors of the "
   "image in each channel. The gray areas represent the estimated dispersion of these measurements. Ideally, all "
   "samples should be distributed on a horizontal straight line. The straighter and more horizontal the "
   "distribution and the narrower the dispersion area, the lower the uncertainty in the estimated scale factor.</p>\n"
"<p>The graphs are interactive:</p>\n"
"<ul>\n"
"<li>Mouse over to highlight individual values for measured stars.</li>\n"
"<li>Click and drag to zoom. You can zoom both vertically and horizontally.</li>\n"
"<li>Double-click to zoom out.</li>\n"
"<li>On a zoomed graph, shift-drag to pan.</li>\n"
"</ul>\n"
"<hr/>\n"
"</div>\n" + // Help
divs + '\n' +
"<script type=\"text/javascript\">\n" +
graphs + '\n' +
"</script>\n"
"</body>\n"
"</html>\n";

   String filePath = File::UniqueFileName( File::SystemTempDirectory(), 12, "SPFC_graph_", ".html" );
   File::WriteTextFile( filePath, html.ToUTF8() );
   m_htmlFiles << filePath;

   GUI->Graph_WebView.LoadContent( File::FileURI( filePath ) );
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationGraphInterface::ExportPDF() const
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

void SpectrophotometricFluxCalibrationGraphInterface::ToggleText() const
{
   GUI->Graph_WebView.EvaluateScript( "toggleDisplay( 'Text' );" );
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationGraphInterface::ToggleHelp() const
{
   GUI->Graph_WebView.EvaluateScript( "toggleDisplay( 'Help' );" );
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationGraphInterface::CleanUp()
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

void SpectrophotometricFluxCalibrationGraphInterface::e_LoadFinished( WebView& sender, bool state )
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

void SpectrophotometricFluxCalibrationGraphInterface::e_Click( Button& sender, bool checked )
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

SpectrophotometricFluxCalibrationGraphInterface::GUIData::GUIData( SpectrophotometricFluxCalibrationGraphInterface& w )
{
   ExportPDF_ToolButton.SetIcon( w.ScaledResource( ":/script-editor/export-pdf.png" ) );
   ExportPDF_ToolButton.SetScaledFixedSize( 22, 22 );
   ExportPDF_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExportPDF_ToolButton.SetToolTip( "<p>Export the graphs as a PDF document.</p>" );
   ExportPDF_ToolButton.OnClick( (Button::click_event_handler)&SpectrophotometricFluxCalibrationGraphInterface::e_Click, w );

   ToggleText_ToolButton.SetIcon( w.ScaledResource( ":/icons/window-text.png" ) );
   ToggleText_ToolButton.SetScaledFixedSize( 22, 22 );
   ToggleText_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ToggleText_ToolButton.SetToolTip( "<p>Show/hide the text header.</p>" );
   ToggleText_ToolButton.OnClick( (Button::click_event_handler)&SpectrophotometricFluxCalibrationGraphInterface::e_Click, w );

   ToggleHelp_ToolButton.SetIcon( w.ScaledResource( ":/icons/help.png" ) );
   ToggleHelp_ToolButton.SetScaledFixedSize( 22, 22 );
   ToggleHelp_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ToggleHelp_ToolButton.SetToolTip( "<p>Show/hide help on flux calibration function graphs.</p>" );
   ToggleHelp_ToolButton.OnClick( (Button::click_event_handler)&SpectrophotometricFluxCalibrationGraphInterface::e_Click, w );

   Buttons_Sizer.SetMargin( 4 );
   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( ExportPDF_ToolButton );
   Buttons_Sizer.Add( ToggleText_ToolButton );
   Buttons_Sizer.Add( ToggleHelp_ToolButton );

   Graph_WebView.OnLoadFinished( (WebView::state_event_handler)&SpectrophotometricFluxCalibrationGraphInterface::e_LoadFinished, w );
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
// EOF SpectrophotometricFluxCalibrationGraphInterface.cpp - Released 2024-12-23T11:33:28Z
