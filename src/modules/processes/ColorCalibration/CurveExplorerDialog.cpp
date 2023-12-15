//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// CurveExplorerDialog.cpp - Released 2023-12-01T19:16:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "CurveExplorerDialog.h"
#include "SpectrophotometricColorCalibrationProcess.h"

#include <pcl/AkimaInterpolation.h>
#include <pcl/Console.h>
#include <pcl/Cryptography.h>
#include <pcl/Edit.h>
#include <pcl/FileDialog.h>
#include <pcl/Label.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>

// The represented range of wavelengths for filters (nm)
#define LAMBDA_0_FLT  290
#define LAMBDA_1_FLT 1110

// The represented range of wavelengths for white references (nm)
#define LAMBDA_0_WRF  200
#define LAMBDA_1_WRF 2500

// Minimum height of the filters TreeBox in logical pixels
#define MIN_FILTERS_TREEBOX_HEIGHT  200

namespace pcl
{

// ----------------------------------------------------------------------------

CurveExplorerDialog::CurveExplorerDialog()
{
   const char* curveTypeToolTip = "<p>The type of spectral curves that will be included in the list: "
      "either filters or white references.</p>";

   CurveType_Label.SetText( "Curve type:" );
   CurveType_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   CurveType_Label.SetToolTip( curveTypeToolTip );

   CurveType_ComboBox.AddItem( "Filters" );
   CurveType_ComboBox.AddItem( "White References" );
   CurveType_ComboBox.SetToolTip( curveTypeToolTip );
   CurveType_ComboBox.SetCurrentItem( 0 );
   CurveType_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&CurveExplorerDialog::e_ItemSelected, *this );

   CurveType_Sizer.SetSpacing( 4 );
   CurveType_Sizer.Add( CurveType_Label );
   CurveType_Sizer.Add( CurveType_ComboBox );
   CurveType_Sizer.AddStretch();

   Curves_TreeBox.SetNumberOfColumns( 3 );
   Curves_TreeBox.SetHeaderText( 0, "Chn" );
   Curves_TreeBox.SetHeaderText( 1, "Def" );
   Curves_TreeBox.SetHeaderText( 2, "Name" );
   Curves_TreeBox.EnableMultipleSelections();
   Curves_TreeBox.DisableRootDecoration();
   Curves_TreeBox.EnableAlternateRowColor();
   Curves_TreeBox.SetScaledMinHeight( MIN_FILTERS_TREEBOX_HEIGHT );
   Curves_TreeBox.AdjustColumnWidthToContents( 0 );
   Curves_TreeBox.AdjustColumnWidthToContents( 1 );
   Curves_TreeBox.AdjustColumnWidthToContents( 2 );
   Curves_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&CurveExplorerDialog::e_CurrentNodeUpdated, *this );
   Curves_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&CurveExplorerDialog::e_NodeSelectionUpdated, *this );

   VertSplitter_Control.SetScaledFixedHeight( 4 );
   VertSplitter_Control.EnableExpansion( true/*horzEnable*/, false/*vertEnable*/ );
   VertSplitter_Control.SetCursor( StdCursor::VerticalSplit );
   VertSplitter_Control.EnableMouseTracking();
   VertSplitter_Control.OnMousePress( (Control::mouse_button_event_handler)&CurveExplorerDialog::e_MousePress, *this );
   VertSplitter_Control.OnMouseRelease( (Control::mouse_button_event_handler)&CurveExplorerDialog::e_MouseRelease, *this );
   VertSplitter_Control.SetStyleSheet( ScaledStyleSheet( "QWidget { border: 1px solid darkgray; }" ) );
   VertSplitter_Control.BringToFront();

   Graph_WebView.OnLoadFinished( (WebView::state_event_handler)&CurveExplorerDialog::e_LoadFinished, *this );
   Graph_WebView.SetStyleSheet( ScaledStyleSheet( "pi--WebView { min-width: 508px; min-height: 440px; }" ) );

   ExportPDF_ToolButton.SetIcon( ScaledResource( ":/script-editor/export-pdf.png" ) );
   ExportPDF_ToolButton.SetScaledFixedSize( 22, 22 );
   ExportPDF_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExportPDF_ToolButton.SetToolTip( "<p>Export the graph as a PDF document.</p>" );
   ExportPDF_ToolButton.OnClick( (Button::click_event_handler)&CurveExplorerDialog::e_Click, *this );

   CombineFilters_ToolButton.SetIcon( ScaledResource( ":/icons/component-new.png" ) );
   CombineFilters_ToolButton.SetScaledFixedSize( 22, 22 );
   CombineFilters_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   CombineFilters_ToolButton.SetToolTip( "<p>Combine the selected filters to generate a new custom filter. The new filter will be "
      "generated by multiplication of transmission curves.</p>" );
   CombineFilters_ToolButton.OnClick( (Button::click_event_handler)&CurveExplorerDialog::e_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&CurveExplorerDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.Add( ExportPDF_ToolButton );
   Buttons_Sizer.Add( CombineFilters_ToolButton );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( CurveType_Sizer );
   Global_Sizer.Add( Curves_TreeBox );
   Global_Sizer.Add( VertSplitter_Control );
   Global_Sizer.Add( Graph_WebView, 100 );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetMinSize();

   SetWindowTitle( "SPCC Curve Explorer" );

   // Splitter actions
   EnableMouseTracking();
   OnMouseMove( (Control::mouse_event_handler)&CurveExplorerDialog::e_MouseMove, *this );

   OnClose( (pcl::Control::close_event_handler)&CurveExplorerDialog::e_Close, *this );
   OnShow( (pcl::Control::event_handler)&CurveExplorerDialog::e_Show, *this );
   OnHide( (pcl::Control::event_handler)&CurveExplorerDialog::e_Hide, *this );
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::Update()
{
   UpdateCurves();
   UpdateGraph();
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::UpdateCurves()
{
   Curves_TreeBox.DisableUpdates();
   Curves_TreeBox.Clear();

   const SampledSpectrumDataList& curves = m_workingWithFilters ? m_filters : m_whiteReferences;
   for ( const SampledSpectrumData& curve : curves )
   {
      TreeBox::Node* node = new TreeBox::Node( Curves_TreeBox );
      node->SetText( 0, curve.channel );
      node->SetText( 1, curve.defaultItem );
      node->SetText( 2, curve.name );
      node->SetAlignment( 0, Align::Center );
      node->SetAlignment( 1, Align::Center );
      node->SetAlignment( 2, Align::Left );

//       if ( m_workingWithFilters )
//          if ( curve.channel == "R" )
//             node->SetBackgroundColor( 0, 0xffffb9b9 );
//          else if ( curve.channel == "G" )
//             node->SetBackgroundColor( 0, 0xffb9ffb9 );
//          else if ( curve.channel == "B" )
//             node->SetBackgroundColor( 0, 0xffb9b9ff );
//          else if ( curve.channel == "Q" )
//             node->SetBackgroundColor( 0, 0xffffffb9 );
   }

   Curves_TreeBox.AdjustColumnWidthToContents( 0 );
   Curves_TreeBox.AdjustColumnWidthToContents( 1 );
   Curves_TreeBox.AdjustColumnWidthToContents( 2 );

   Curves_TreeBox.EnableUpdates();

   CombineFilters_ToolButton.SetVisible( m_workingWithFilters );
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::UpdateGraph()
{
   SampledSpectrumDataList curves;
   int qeCurveCount = 0, filterCurveCount = 0;
   {
      IndirectArray<TreeBox::Node> nodes = Curves_TreeBox.SelectedNodes();
      if ( nodes.IsEmpty() )
      {
         const TreeBox::Node* node = Curves_TreeBox.CurrentNode();
         if ( node == nullptr )
            return;
         nodes << node;
      }

      const SampledSpectrumDataList& sourceCurves = m_workingWithFilters ? m_filters : m_whiteReferences;
      for ( const TreeBox::Node* node : nodes )
      {
         int i = sourceCurves.Find( node->Text( 2 ) );
         if ( i >= 0 )
         {
            curves << sourceCurves[i];
            if ( sourceCurves[i].channel == "Q" )
               ++qeCurveCount;
            else
               ++filterCurveCount;
         }
      }

      if ( curves.IsEmpty() )
         return;
   }

   String hashData;
   for ( const SampledSpectrumData& curve : curves )
      hashData << curve.name;
   String filePath = File::SystemTempDirectory() + "/CE_graph_" + IsoString::ToHex( MD5().Hash( hashData ) ) + ".html";

   if ( !File::Exists( filePath ) )
   {
      Array<AkimaInterpolation<float> > filterInterpolators;
      for ( const SampledSpectrumData& curve : curves )
      {
         Array<float> filterData = curve.data.ParseListOfFloat();
         if ( filterData.Length() & 1 ) // ?!
            return;
         int length = int( filterData.Length() >> 1 );
         if ( length < 5 ) // ?!
            return;

         FVector wavelengths( length );
         FVector values( length );
         for ( int i = 0, j = 0; i < length; ++i )
         {
            wavelengths[i] = filterData[j++];
            values[i] = filterData[j++];
         }

         AkimaInterpolation<float> interpolator;
         interpolator.Initialize( wavelengths, values );
         filterInterpolators << interpolator;
      }

      const int lambda0 = m_workingWithFilters ? LAMBDA_0_FLT : LAMBDA_0_WRF;
      const int lambda1 = m_workingWithFilters ? LAMBDA_1_FLT : LAMBDA_1_WRF;
      const double delta = 0.1;
      String graphData = "[\n";
      for ( int i = 0, n = TruncInt( (lambda1 - lambda0)/delta );; )
      {
         double lambda = lambda0 + i*delta;
         graphData.AppendFormat( "[%g", lambda );
         for ( size_type j = 0; j < filterInterpolators.Length(); ++j )
            if ( lambda >= filterInterpolators[j].X()[0] && lambda <= filterInterpolators[j].X()[filterInterpolators[j].X().Length()-1] )
               graphData.AppendFormat( ",%g", Max( 0.0, filterInterpolators[j]( lambda ) ) );
            else
               graphData << ",0";
         graphData << ']';
         if ( ++i > n )
            break;
         graphData << ",\n";
      }
      graphData << "\n]";

      String graphLabels = "[\n            'X'";
      for ( const SampledSpectrumData& curve : curves )
         graphLabels << (",\n            '" + curve.name + "'");
      graphLabels << "\n         ]";

      String graphSeries = "{\n";
      for ( size_type j = 0;; )
      {
         graphSeries << String().Format( "            Y%u: {strokeWidth: 2, drawPoints: false}", j );
         if ( ++j == filterInterpolators.Length() )
            break;
         graphSeries << ",\n";
      }
      graphSeries << "\n         }";

      String coreSrcDir = PixInsightSettings::GlobalString ( "Application/SrcDirectory" );

      String yLabel;
      if ( m_workingWithFilters )
      {
         if ( qeCurveCount > 0 )
         {
            if ( filterCurveCount > 0 )
               yLabel = "Transmission / Quantum efficiency";
            else
               yLabel = "Quantum efficiency";
         }
         else
            yLabel = "Transmission";
      }
      else
         yLabel = "Normalized flux";

      String html =
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<meta charset=\"utf-8\">\n"
"<script type=\"text/javascript\" src=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph.js" ) + "\"></script>\n"
"<script type=\"text/javascript\" src=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph-extra.js" ) + "\"></script>\n"
"<link rel=\"stylesheet\" href=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph-doc.css" ) + "\"/>\n"
"<link rel=\"stylesheet\" href=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph.css" ) + "\"/>\n"
"</head>\n"
"<body>\n"
"<div id=\"graph\" style=\"margin:0; width:100%;\"></div>\n"
"<div id=\"legend\" style=\"margin:1.5em 0 0 0;\"></div>\n"
"<script type=\"text/javascript\">\n"
"   var g = new Dygraph(\n"
"      document.getElementById( 'graph' ),\n" +
graphData + ",\n"
"      {\n"
"         labelsDiv: 'legend',\n"
"         labelsSeparateLines: true,\n"
"         legend: 'always',\n"
"         labels: " + graphLabels + ",\n"
"         xlabel: 'Wavelength (nm)',\n"
"         ylabel: '" + yLabel + "',\n"
"         xLabelHeight: 15,\n"
"         yLabelWidth: 15,\n"
"         axisLabelFontSize: 10,\n"
"         series: " + graphSeries + "\n"
"      }\n"
"   );\n"
"</script>\n"
"</body>\n"
"</html>\n";

      m_htmlFiles << filePath;
      File::WriteTextFile( filePath, html.ToUTF8() );
   }

   Graph_WebView.LoadContent( File::FileURI( filePath ) );
}

// ----------------------------------------------------------------------------

class GetCombinedFilterNameAndChannelDialog : public Dialog
{
public:

   GetCombinedFilterNameAndChannelDialog()
   {
      int labelWidth1 = Font().Width( "Filter channel:m" );

      FilterName_Label.SetText( "Filter name:" );
      FilterName_Label.SetFixedWidth( labelWidth1 );
      FilterName_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

      FilterName_Edit.OnEditCompleted( (Edit::edit_event_handler)&GetCombinedFilterNameAndChannelDialog::e_EditCompleted, *this );
      FilterName_Edit.SetMinWidth( 32*Font().Width( 'm' ) );

      FilterName_Sizer.SetSpacing( 4 );
      FilterName_Sizer.Add( FilterName_Label );
      FilterName_Sizer.Add( FilterName_Edit, 100 );

      FilterChannel_Label.SetText( "Filter channel:" );
      FilterChannel_Label.SetFixedWidth( labelWidth1 );
      FilterChannel_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

      FilterChannel_Edit.OnEditCompleted( (Edit::edit_event_handler)&GetCombinedFilterNameAndChannelDialog::e_EditCompleted, *this );
      FilterChannel_Edit.SetMinWidth( 12*Font().Width( 'm' ) );

      FilterChannel_Sizer.SetSpacing( 4 );
      FilterChannel_Sizer.Add( FilterChannel_Label );
      FilterChannel_Sizer.Add( FilterChannel_Edit );
      FilterChannel_Sizer.AddStretch();

      OK_PushButton.SetText( "OK" );
      OK_PushButton.SetDefault();
      OK_PushButton.SetCursor( StdCursor::Checkmark );
      OK_PushButton.OnClick( (pcl::Button::click_event_handler)&GetCombinedFilterNameAndChannelDialog::e_Click, *this );
      OK_PushButton.Disable();

      Cancel_PushButton.SetText( "Cancel" );
      Cancel_PushButton.SetCursor( StdCursor::Crossmark );
      Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&GetCombinedFilterNameAndChannelDialog::e_Click, *this );

      Buttons_Sizer.SetSpacing( 8 );
      Buttons_Sizer.AddStretch();
      Buttons_Sizer.Add( OK_PushButton );
      Buttons_Sizer.Add( Cancel_PushButton );

      Global_Sizer.SetMargin( 8 );
      Global_Sizer.SetSpacing( 6 );
      Global_Sizer.Add( FilterName_Sizer );
      Global_Sizer.Add( FilterChannel_Sizer );
      Global_Sizer.AddSpacing( 8 );
      Global_Sizer.Add( Buttons_Sizer );

      SetSizer( Global_Sizer );

      EnsureLayoutUpdated();
      AdjustToContents();
      SetFixedHeight();
      SetMinWidth();

      SetWindowTitle( "SPCC: New Combined Filter" );
   }

   const String& FilterName() const
   {
      return m_filterName;
   }

   const String& FilterChannel() const
   {
      return m_filterChannel;
   }

private:

   String m_filterName;
   String m_filterChannel;

   VerticalSizer     Global_Sizer;
      HorizontalSizer   FilterName_Sizer;
         Label             FilterName_Label;
         Edit              FilterName_Edit;
      HorizontalSizer   FilterChannel_Sizer;
         Label             FilterChannel_Label;
         Edit              FilterChannel_Edit;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void e_Click( Button& sender, bool checked )
   {
      if ( sender == OK_PushButton )
         Ok();
      else if ( sender == Cancel_PushButton )
         Cancel();
   }

   void e_EditCompleted( Edit& sender )
   {
      String text = sender.Text().Trimmed();

      if ( sender == FilterName_Edit )
      {
         m_filterName = text;
         OK_PushButton.Enable( !m_filterName.IsEmpty() );
      }
      else if ( sender == FilterChannel_Edit )
         m_filterChannel = text;

      sender.SetText( text );
   }
};

void CurveExplorerDialog::CombineFilters()
{
   SampledSpectrumDataList filters;
   {
      IndirectArray<TreeBox::Node> nodes = Curves_TreeBox.SelectedNodes();
      for ( const TreeBox::Node* node : nodes )
      {
         int i = m_filters.Find( node->Text( 2 ) );
         if ( i >= 0 )
            filters << m_filters[i];
      }

      if ( filters.Length() < 2 )
      {
         MessageBox( "<p>Please select two or more filters to generate a new combined filter.</p>",
                     "SPCC",
                     StdIcon::Error,
                     StdButton::Ok ).Execute();
         return;
      }
   }

   Array<AkimaInterpolation<float> > filterInterpolators;
   float lambda0 = 0, lambda1 = 1e30;
   for ( const SampledSpectrumData& filter : filters )
   {
      Array<float> filterData = filter.data.ParseListOfFloat();
      if ( filterData.Length() & 1 ) // ?!
         return;
      int length = int( filterData.Length() >> 1 );
      if ( length < 5 ) // ?!
         return;

      FVector wavelengths( length );
      FVector values( length );
      for ( int i = 0, j = 0; i < length; ++i )
      {
         wavelengths[i] = filterData[j++];
         values[i] = filterData[j++];
      }
      lambda0 = Max( lambda0, wavelengths.MinComponent() );
      lambda1 = Min( lambda1, wavelengths.MaxComponent() );

      AkimaInterpolation<float> interpolator;
      interpolator.Initialize( wavelengths, values );
      filterInterpolators << interpolator;
   }

   Array<float> wavelengths, values;
   for ( float lambda = lambda0, delta = (lambda1 - lambda0 > 10) ? 1.0F : (lambda1 - lambda0)/10; lambda <= lambda1; lambda += delta )
   {
      wavelengths << lambda;
      double value = 1.0;
      for ( const AkimaInterpolation<float>& interpolator : filterInterpolators )
         value *= interpolator( lambda );
      values << value;
   }

   if ( values.IsEmpty() )
   {
      MessageBox( "<p>Cannot combine filters: there is no intersection among filter transmission curves.</p>",
                  "SPCC",
                  StdIcon::Error,
                  StdButton::Ok ).Execute();
      return;
   }

   SampledSpectrumData combinedFilter;
   for ( ;; )
   {
      GetCombinedFilterNameAndChannelDialog dialog;
      if ( !dialog.Execute() )
         return;
      combinedFilter.name = dialog.FilterName();
      combinedFilter.channel = dialog.FilterChannel();

      if ( !m_filters.Has( combinedFilter.name ) )
         break;

      if ( MessageBox( "<p>A filter '" + combinedFilter.name + "' already exists. Replace it?</p>",
                       "SPCC",
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() == StdButton::Yes )
         break;
   }
   for ( size_type i = 0;; )
   {
      combinedFilter.data << String().Format( "%.6g,%.6g", wavelengths[i], values[i] );
      if ( ++i == wavelengths.Length() )
         break;
      combinedFilter.data << ',';
   }
   m_filters << combinedFilter;
   m_modified = true;

   UpdateCurves();

   for ( int i = 0, n = Curves_TreeBox.NumberOfChildren(); i < n; ++i )
      if ( Curves_TreeBox[i]->Text( 2 ) == combinedFilter.name )
      {
         Curves_TreeBox[i]->Select();
         Curves_TreeBox.SetCurrentNode( Curves_TreeBox[i] );
         break;
      }

   UpdateGraph();
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::ExportPDF()
{
   SaveFileDialog d;
   d.SetCaption( "Filter Explorer: Export PDF File" );
   d.SetFilter( FileFilter( "PDF", ".pdf" ) );
   d.EnableOverwritePrompt();
   if ( d.Execute() )
   {
      String filePath = d.FileName();
      Graph_WebView.SaveAsPDF( filePath );
      Console console;
      console.Show();
      console.NoteLn( "<end><cbr><br>* Filter Explorer graph exported to PDF document: <raw>" + filePath + "</raw><end>" );
      Module->ProcessEvents();
   }
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   // placeholder
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::e_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateGraph();
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == ExportPDF_ToolButton )
   {
      ExportPDF();
   }
   else if ( sender == CombineFilters_ToolButton )
   {
      if ( m_workingWithFilters )
         CombineFilters();
   }
   else if ( sender == OK_PushButton )
   {
      if ( m_modified )
      {
         int resp = MessageBox( "<p>Some combined filters have been defined. Save them?</p>",
                                "SPCC",
                                StdIcon::Warning,
                                StdButton::Yes, StdButton::No, StdButton::Cancel ).Execute();
         if ( resp == StdButton::Cancel )
            return;
         if ( resp == StdButton::Yes )
         {
            SpectrophotometricColorCalibrationProcess::ImportFilters( m_filters, false/*merge*/ );
            m_madeChanges = true;
         }
      }

      Ok();
   }
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == CurveType_ComboBox )
   {
      bool newWorkingWithFilters = itemIndex == 0;
      if ( newWorkingWithFilters != m_workingWithFilters )
      {
         m_workingWithFilters = newWorkingWithFilters;
         UpdateCurves();
         UpdateGraph();
      }
   }
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::e_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( sender == VertSplitter_Control )
   {
      m_splitterPressed = true;
      m_splitterPos = sender.LocalToControl( *this, pos ).y;
   }
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::e_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   m_splitterPressed = false;
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::e_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( m_splitterPressed )
   {
      int dy = m_splitterPos - pos.y;
      if ( dy < 0 )
         if ( Graph_WebView.Height() <= Graph_WebView.MinHeight() )
            return;
      int newHeight = Max( LogicalPixelsToPhysical( MIN_FILTERS_TREEBOX_HEIGHT ), Curves_TreeBox.Height() - dy );
      if ( newHeight != Curves_TreeBox.Height() )
      {
         Curves_TreeBox.SetMinHeight( newHeight );
         EnsureLayoutUpdated();
         m_splitterPos = pos.y;
      }
   }
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::e_LoadFinished( WebView& sender, bool state )
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
   Graph_WebView.SetZoomFactor( 1.0 );
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::e_Close( Control& sender, bool& allowClose )
{
   if ( m_modified )
      if ( MessageBox( "<p>Some combined filters have been defined. Throw away them?</p>",
                        "SPCC",
                        StdIcon::Warning,
                        StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
      {
         allowClose = false;
         return;
      }

   allowClose = true;
}

// ----------------------------------------------------------------------------

void CurveExplorerDialog::e_Show( Control& sender )
{
   m_filters = SpectrophotometricColorCalibrationProcess::Filters();
   m_whiteReferences = SpectrophotometricColorCalibrationProcess::WhiteReferences();
   m_madeChanges = m_modified = false;
   Update();
}

void CurveExplorerDialog::e_Hide( Control& sender )
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
   m_modified = false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CurveExplorerDialog.cpp - Released 2023-12-01T19:16:17Z
