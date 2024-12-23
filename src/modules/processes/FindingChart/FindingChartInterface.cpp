//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard FindingChart Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// FindingChartInterface.cpp - Released 2024-12-23T11:33:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard FindingChart PixInsight module.
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

#include "FindingChartInterface.h"
#include "FindingChartParameters.h"
#include "FindingChartProcess.h"

#include <pcl/ColorDialog.h>
#include <pcl/FileDialog.h>
#include <pcl/Graphics.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FindingChartInterface* TheFindingChartInterface = nullptr;

// ----------------------------------------------------------------------------

FindingChartInterface::FindingChartInterface()
   : m_instance( TheFindingChartProcess )
{
   TheFindingChartInterface = this;
}

// ----------------------------------------------------------------------------

FindingChartInterface::~FindingChartInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString FindingChartInterface::Id() const
{
   return "FindingChart";
}

// ----------------------------------------------------------------------------

MetaProcess* FindingChartInterface::Process() const
{
   return TheFindingChartProcess;
}

// ----------------------------------------------------------------------------

String FindingChartInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/FindingChart.svg";
}

// ----------------------------------------------------------------------------

void FindingChartInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void FindingChartInterface::ResetInstance()
{
   FindingChartInstance defaultInstance( TheFindingChartProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool FindingChartInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "FindingChart" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheFindingChartProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* FindingChartInterface::NewProcess() const
{
   return new FindingChartInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool FindingChartInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const FindingChartInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a FindingChart instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool FindingChartInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool FindingChartInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void FindingChartInterface::UpdateControls()
{
   GUI->ChartSize_SpinBox.SetValue( m_instance.p_chartSize );

   GUI->AutoChartResolution_CheckBox.SetChecked( m_instance.p_autoChartResolution );

   GUI->ChartResolution_NumericControl.SetValue( m_instance.p_chartResolution );
   GUI->ChartResolution_NumericControl.Enable( !m_instance.p_autoChartResolution );

   GUI->AutoChartMaxMagnitude_CheckBox.SetChecked( m_instance.p_autoChartMaxMagnitude );

   GUI->ChartMaxMagnitude_NumericControl.SetValue( m_instance.p_chartMaxMagnitude );
   GUI->ChartMaxMagnitude_NumericControl.Enable( !m_instance.p_autoChartMaxMagnitude );

   GUI->GenerateBitmapFile_CheckBox.SetChecked( m_instance.p_generateBitmapFile );

   GUI->OutputDirectory_Label.Enable( m_instance.p_generateBitmapFile );

   GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory );
   GUI->OutputDirectory_Edit.Enable( m_instance.p_generateBitmapFile );

   GUI->OutputDirectory_ToolButton.Enable( m_instance.p_generateBitmapFile );

   GUI->OutputSuffix_Label.Enable( m_instance.p_generateBitmapFile );

   GUI->OutputSuffix_Edit.SetText( m_instance.p_outputSuffix );
   GUI->OutputSuffix_Edit.Enable( m_instance.p_generateBitmapFile );

   GUI->CloseOutputWindow_CheckBox.SetChecked( m_instance.p_closeOutputWindow );
   GUI->CloseOutputWindow_CheckBox.Enable( m_instance.p_generateBitmapFile );

   GUI->DrawGrid_CheckBox.SetChecked( m_instance.p_drawGrid );

   GUI->DrawEcliptic_CheckBox.SetChecked( m_instance.p_drawEcliptic );

   GUI->DrawGalacticEquator_CheckBox.SetChecked( m_instance.p_drawGalacticEquator );

   GUI->DrawConstellationBorders_CheckBox.SetChecked( m_instance.p_drawConstellationBorders );

   GUI->DrawConstellationLines_CheckBox.SetChecked( m_instance.p_drawConstellationLines );

   GUI->DrawConstellationNames_CheckBox.SetChecked( m_instance.p_drawConstellationNames );
   GUI->DrawConstellationNames_CheckBox.Enable( m_instance.p_drawConstellationBorders );

   GUI->DrawStars_CheckBox.SetChecked( m_instance.p_drawStars );

   GUI->DrawStarNames_CheckBox.SetChecked( m_instance.p_drawStarNames );
   GUI->DrawStarNames_CheckBox.Enable( m_instance.p_drawStars );

   GUI->DrawMessierObjects_CheckBox.SetChecked( m_instance.p_drawMessierObjects );

   GUI->DrawNGCObjects_CheckBox.SetChecked( m_instance.p_drawNGCObjects );

   GUI->ImageRegionColor_ComboBox.SetCurrentColor( m_instance.p_imageRegionColor );
   GUI->ImageRegionColor_Control.Update();

   GUI->ImageRegionBorderColor_ComboBox.SetCurrentColor( m_instance.p_imageRegionBorderColor );
   GUI->ImageRegionBorderColor_Control.Update();

   GUI->GridColor_ComboBox.SetCurrentColor( m_instance.p_gridColor );
   GUI->GridColor_Control.Update();

   GUI->GridTextColor_ComboBox.SetCurrentColor( m_instance.p_gridTextColor );
   GUI->GridTextColor_Control.Update();

   GUI->EclipticColor_ComboBox.SetCurrentColor( m_instance.p_eclipticColor );
   GUI->EclipticColor_Control.Update();

   GUI->EclipticTextColor_ComboBox.SetCurrentColor( m_instance.p_eclipticTextColor );
   GUI->EclipticTextColor_Control.Update();

   GUI->GalacticEquatorColor_ComboBox.SetCurrentColor( m_instance.p_galacticEquatorColor );
   GUI->GalacticEquatorColor_Control.Update();

   GUI->GalacticEquatorTextColor_ComboBox.SetCurrentColor( m_instance.p_galacticEquatorTextColor );
   GUI->GalacticEquatorTextColor_Control.Update();

   GUI->ConstellationBorderColor_ComboBox.SetCurrentColor( m_instance.p_constellationBorderColor );
   GUI->ConstellationBorderColor_Control.Update();

   GUI->ConstellationLineColor_ComboBox.SetCurrentColor( m_instance.p_constellationLineColor );
   GUI->ConstellationLineColor_Control.Update();

   GUI->ConstellationTextColor_ComboBox.SetCurrentColor( m_instance.p_constellationTextColor );
   GUI->ConstellationTextColor_Control.Update();

   GUI->StarTextColor_ComboBox.SetCurrentColor( m_instance.p_starTextColor );
   GUI->StarTextColor_Control.Update();

   GUI->MessierColor_ComboBox.SetCurrentColor( m_instance.p_messierTextColor );
   GUI->MessierColor_Control.Update();

   GUI->NGCColor_ComboBox.SetCurrentColor( m_instance.p_ngcTextColor );
   GUI->NGCColor_Control.Update();
}

// ----------------------------------------------------------------------------

void FindingChartInterface::e_RealValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ChartResolution_NumericControl )
      m_instance.p_chartResolution = value;
   else if ( sender == GUI->ChartMaxMagnitude_NumericControl )
      m_instance.p_chartMaxMagnitude = value;
}

// ----------------------------------------------------------------------------

void FindingChartInterface::e_IntegerValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ChartSize_SpinBox )
      m_instance.p_chartSize = value;
}

// ----------------------------------------------------------------------------

void FindingChartInterface::e_ItemClicked( Button& sender, bool checked )
{
   if ( sender == GUI->AutoChartResolution_CheckBox )
   {
      m_instance.p_autoChartResolution = checked;
      UpdateControls();
   }
   else if ( sender == GUI->AutoChartMaxMagnitude_CheckBox )
   {
      m_instance.p_autoChartMaxMagnitude = checked;
      UpdateControls();
   }
   else if ( sender == GUI->GenerateBitmapFile_CheckBox )
   {
      m_instance.p_generateBitmapFile = checked;
      UpdateControls();
   }
   else if ( sender == GUI->OutputDirectory_ToolButton )
   {
      GetDirectoryDialog d;
      d.SetCaption( "FindingChart: Select Output Directory" );
      if ( d.Execute() )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = d.Directory() );
   }
   else if ( sender == GUI->CloseOutputWindow_CheckBox )
      m_instance.p_closeOutputWindow = checked;
   else if ( sender == GUI->DrawGrid_CheckBox )
      m_instance.p_drawGrid = checked;
   else if ( sender == GUI->DrawConstellationBorders_CheckBox )
   {
      m_instance.p_drawConstellationBorders = checked;
      UpdateControls();
   }
   else if ( sender == GUI->DrawConstellationLines_CheckBox )
      m_instance.p_drawConstellationLines = checked;
   else if ( sender == GUI->DrawConstellationNames_CheckBox )
      m_instance.p_drawConstellationNames = checked;
   else if ( sender == GUI->DrawStars_CheckBox )
   {
      m_instance.p_drawStars = checked;
      UpdateControls();
   }
   else if ( sender == GUI->DrawStarNames_CheckBox )
      m_instance.p_drawStarNames = checked;
   else if ( sender == GUI->DrawMessierObjects_CheckBox )
      m_instance.p_drawMessierObjects = checked;
   else if ( sender == GUI->DrawNGCObjects_CheckBox )
      m_instance.p_drawNGCObjects = checked;
}

// ----------------------------------------------------------------------------

void FindingChartInterface::e_EditGetFocus( Control& sender )
{
   // Placeholder
   if ( sender == GUI->OutputDirectory_Edit )
   {
   }
   else if ( sender == GUI->OutputSuffix_Edit )
   {
   }
}

// ----------------------------------------------------------------------------

void FindingChartInterface::e_EditCompleted( Edit& sender )
{
   if ( sender == GUI->OutputDirectory_Edit )
      m_instance.p_outputDirectory = sender.Text().Trimmed();
   else if ( sender == GUI->OutputSuffix_Edit )
      m_instance.p_outputSuffix = sender.Text().Trimmed();
}

// ----------------------------------------------------------------------------

void FindingChartInterface::e_ColorSelected( ColorComboBox& sender, RGBA color )
{
   if ( sender == GUI->ImageRegionColor_ComboBox )
      m_instance.p_imageRegionColor = uint32( color );
   else if ( sender == GUI->ImageRegionBorderColor_ComboBox )
      m_instance.p_imageRegionBorderColor = uint32( color );
   else if ( sender == GUI->GridColor_ComboBox )
      m_instance.p_gridColor = uint32( color );
   else if ( sender == GUI->GridTextColor_ComboBox )
      m_instance.p_gridTextColor = uint32( color );
   else if ( sender == GUI->EclipticColor_ComboBox )
      m_instance.p_eclipticColor = uint32( color );
   else if ( sender == GUI->EclipticTextColor_ComboBox )
      m_instance.p_eclipticTextColor = uint32( color );
   else if ( sender == GUI->GalacticEquatorColor_ComboBox )
      m_instance.p_galacticEquatorColor = uint32( color );
   else if ( sender == GUI->GalacticEquatorTextColor_ComboBox )
      m_instance.p_galacticEquatorTextColor = uint32( color );
   else if ( sender == GUI->ConstellationBorderColor_ComboBox )
      m_instance.p_constellationBorderColor = uint32( color );
   else if ( sender == GUI->ConstellationLineColor_ComboBox )
      m_instance.p_constellationLineColor = uint32( color );
   else if ( sender == GUI->ConstellationTextColor_ComboBox )
      m_instance.p_constellationTextColor = uint32( color );
   else if ( sender == GUI->StarTextColor_ComboBox )
      m_instance.p_starTextColor = uint32( color );
   else if ( sender == GUI->MessierColor_ComboBox )
      m_instance.p_messierTextColor = uint32( color );
   else if ( sender == GUI->NGCColor_ComboBox )
      m_instance.p_ngcTextColor = uint32( color );

   UpdateControls();
}

// ----------------------------------------------------------------------------

void FindingChartInterface::e_Paint( Control& sender, const Rect& /*updateRect*/ )
{
   RGBA color;
   if ( sender == GUI->ImageRegionColor_Control )
      color = m_instance.p_imageRegionColor;
   else if ( sender == GUI->ImageRegionBorderColor_Control )
      color = m_instance.p_imageRegionBorderColor;
   else if ( sender == GUI->GridColor_Control )
      color = m_instance.p_gridColor;
   else if ( sender == GUI->GridTextColor_Control )
      color = m_instance.p_gridTextColor;
   else if ( sender == GUI->EclipticColor_Control )
      color = m_instance.p_eclipticColor;
   else if ( sender == GUI->EclipticTextColor_Control )
      color = m_instance.p_eclipticTextColor;
   else if ( sender == GUI->GalacticEquatorColor_Control )
      color = m_instance.p_galacticEquatorColor;
   else if ( sender == GUI->GalacticEquatorTextColor_Control )
      color = m_instance.p_galacticEquatorTextColor;
   else if ( sender == GUI->ConstellationBorderColor_Control )
      color = m_instance.p_constellationBorderColor;
   else if ( sender == GUI->ConstellationLineColor_Control )
      color = m_instance.p_constellationLineColor;
   else if ( sender == GUI->ConstellationTextColor_Control )
      color = m_instance.p_constellationTextColor;
   else if ( sender == GUI->StarTextColor_Control )
      color = m_instance.p_starTextColor;
   else if ( sender == GUI->MessierColor_Control )
      color = m_instance.p_messierTextColor;
   else if ( sender == GUI->NGCColor_Control )
      color = m_instance.p_ngcTextColor;
   else
      return; // ?!

   Graphics g( sender );
   g.EnableAntialiasing();
   g.SetBrush( color );
   g.SetPen( 0xFF000000, sender.DisplayPixelRatio() );
   g.DrawRect( sender.BoundsRect() );
}

// ----------------------------------------------------------------------------

void FindingChartInterface::e_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( sender.IsUnderMouse() )
   {
      SimpleColorDialog dlg( nullptr/*color*/, SimpleColorDialog::AlphaEnabled );
      if ( sender == GUI->ImageRegionColor_Control )
         dlg.SetColor( m_instance.p_imageRegionColor );
      else if ( sender == GUI->ImageRegionBorderColor_Control )
         dlg.SetColor( m_instance.p_imageRegionBorderColor );
      else if ( sender == GUI->GridColor_Control )
         dlg.SetColor( m_instance.p_gridColor );
      else if ( sender == GUI->GridTextColor_Control )
         dlg.SetColor( m_instance.p_gridTextColor );
      else if ( sender == GUI->EclipticColor_Control )
         dlg.SetColor( m_instance.p_eclipticColor );
      else if ( sender == GUI->EclipticTextColor_Control )
         dlg.SetColor( m_instance.p_eclipticTextColor );
      else if ( sender == GUI->GalacticEquatorColor_Control )
         dlg.SetColor( m_instance.p_galacticEquatorColor );
      else if ( sender == GUI->GalacticEquatorTextColor_Control )
         dlg.SetColor( m_instance.p_galacticEquatorTextColor );
      else if ( sender == GUI->ConstellationBorderColor_Control )
         dlg.SetColor( m_instance.p_constellationBorderColor );
      else if ( sender == GUI->ConstellationLineColor_Control )
         dlg.SetColor( m_instance.p_constellationLineColor );
      else if ( sender == GUI->ConstellationTextColor_Control )
         dlg.SetColor( m_instance.p_constellationTextColor );
      else if ( sender == GUI->StarTextColor_Control )
         dlg.SetColor( m_instance.p_starTextColor );
      else if ( sender == GUI->MessierColor_Control )
         dlg.SetColor( m_instance.p_messierTextColor );
      else if ( sender == GUI->NGCColor_Control )
         dlg.SetColor( m_instance.p_ngcTextColor );
      else
         return; // ?!

      if ( dlg.Execute() == StdDialogCode::Ok )
      {
         uint32 color = uint32( dlg.Color() );
         if ( sender == GUI->ImageRegionColor_Control )
            m_instance.p_imageRegionColor = color;
         else if ( sender == GUI->ImageRegionBorderColor_Control )
            m_instance.p_imageRegionBorderColor = color;
         else if ( sender == GUI->GridColor_Control )
            m_instance.p_gridColor = color;
         else if ( sender == GUI->GridTextColor_Control )
            m_instance.p_gridTextColor = color;
         else if ( sender == GUI->EclipticColor_Control )
            m_instance.p_eclipticColor = color;
         else if ( sender == GUI->EclipticTextColor_Control )
            m_instance.p_eclipticTextColor = color;
         else if ( sender == GUI->GalacticEquatorColor_Control )
            m_instance.p_galacticEquatorColor = color;
         else if ( sender == GUI->GalacticEquatorTextColor_Control )
            m_instance.p_galacticEquatorTextColor = color;
         else if ( sender == GUI->ConstellationBorderColor_Control )
            m_instance.p_constellationBorderColor = color;
         else if ( sender == GUI->ConstellationLineColor_Control )
            m_instance.p_constellationLineColor = color;
         else if ( sender == GUI->ConstellationTextColor_Control )
            m_instance.p_constellationTextColor = color;
         else if ( sender == GUI->StarTextColor_Control )
            m_instance.p_starTextColor = color;
         else if ( sender == GUI->MessierColor_Control )
            m_instance.p_messierTextColor = color;
         else if ( sender == GUI->NGCColor_Control )
            m_instance.p_ngcTextColor = color;

         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------

void FindingChartInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = m_instance.p_generateBitmapFile && files.Length() == 1 && File::DirectoryExists( files[0] );
}

// ----------------------------------------------------------------------------

void FindingChartInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->OutputDirectory_Edit )
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = files[0] );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FindingChartInterface::GUIData::GUIData( FindingChartInterface& w )
{
   pcl::Font font = w.Font();
   int labelWidth1 = font.Width( String( "Galactic longitude labels:" ) + 'T' );
   int editWidth1 = font.Width( String( '0', 8 ) );

   //

   const char* chartSizeToolTip =
   "<p>The chart size in pixels. The default value is 1024. FindingChart generates square charts.</p>";

   ChartSize_Label.SetText( "Chart size:" );
   ChartSize_Label.SetMinWidth( labelWidth1 );
   ChartSize_Label.SetToolTip( chartSizeToolTip );
   ChartSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ChartSize_SpinBox.SetRange( int( TheFCChartSizeParameter->MinimumValue() ), int( TheFCChartSizeParameter->MaximumValue() ) );
   ChartSize_SpinBox.SetToolTip( chartSizeToolTip );
   ChartSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&FindingChartInterface::e_IntegerValueUpdated, w );

   ChartSize_Sizer.SetSpacing( 4 );
   ChartSize_Sizer.Add( ChartSize_Label );
   ChartSize_Sizer.Add( ChartSize_SpinBox );
   ChartSize_Sizer.AddStretch();

   //

   AutoChartResolution_CheckBox.SetText( "Automatic chart resolution" );
   AutoChartResolution_CheckBox.SetToolTip( "<p>Enable this option to allow FindingChart calculate an optimal "
      "chart resolution in arcseconds per pixel, which will be based on the field of view of the target image.</p>" );
   AutoChartResolution_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   AutoChartResolution_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   AutoChartResolution_Sizer.Add( AutoChartResolution_CheckBox );
   AutoChartResolution_Sizer.AddStretch();

   //

   ChartResolution_NumericControl.label.SetText( "Chart resolution:" );
   ChartResolution_NumericControl.label.SetFixedWidth( labelWidth1 );
   ChartResolution_NumericControl.slider.SetScaledMinWidth( 250 );
   ChartResolution_NumericControl.slider.SetRange( 0, 240 );
   ChartResolution_NumericControl.SetReal();
   ChartResolution_NumericControl.SetRange( TheFCChartResolutionParameter->MinimumValue(), TheFCChartResolutionParameter->MaximumValue() );
   ChartResolution_NumericControl.SetPrecision( TheFCChartResolutionParameter->Precision() );
   ChartResolution_NumericControl.SetToolTip( "<p>The chart resolution in arcseconds per pixel.</p>"
      "<p>This parameter determines the scale of the generated chart as projected on the celestial sphere. "
      "We recommend to enable the <i>automatic chart resolution</i> option, which will calculate an optimal value "
      "automatically, based on the field of view of the target image.</p>" );
   ChartResolution_NumericControl.edit.SetFixedWidth( editWidth1 );
   ChartResolution_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&FindingChartInterface::e_RealValueUpdated, w );

   //

   AutoChartMaxMagnitude_CheckBox.SetText( "Automatic limit magnitude" );
   AutoChartMaxMagnitude_CheckBox.SetToolTip( "<p>Enable this option to allow FindingChart calculate an optimal limit magnitude automatically, "
      "which will be based on the field of view of the target image and the chart resolution.</p>" );
   AutoChartMaxMagnitude_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   AutoChartMaxMagnitude_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   AutoChartMaxMagnitude_Sizer.Add( AutoChartMaxMagnitude_CheckBox );
   AutoChartMaxMagnitude_Sizer.AddStretch();

   //

   ChartMaxMagnitude_NumericControl.label.SetText( "Limit magnitude:" );
   ChartMaxMagnitude_NumericControl.label.SetFixedWidth( labelWidth1 );
   ChartMaxMagnitude_NumericControl.slider.SetScaledMinWidth( 250 );
   ChartMaxMagnitude_NumericControl.slider.SetRange( 4, 160 );
   ChartMaxMagnitude_NumericControl.SetReal();
   ChartMaxMagnitude_NumericControl.SetRange( TheFCChartMaxMagnitudeParameter->MinimumValue(), TheFCChartMaxMagnitudeParameter->MaximumValue() );
   ChartMaxMagnitude_NumericControl.SetPrecision( TheFCChartMaxMagnitudeParameter->Precision() );
   ChartMaxMagnitude_NumericControl.EnableFixedPrecision();
   ChartMaxMagnitude_NumericControl.SetToolTip( "<p>Magnitude of the dimmest stars rendered on the finding chart.</p>"
      "<p>We recommend to enable the <i>automatic limit magnitude</i> option, which will calculate an optimal value "
      "automatically, based on the field of view of the target image and the chart resolution.</p>" );
   ChartMaxMagnitude_NumericControl.edit.SetFixedWidth( editWidth1 );
   ChartMaxMagnitude_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&FindingChartInterface::e_RealValueUpdated, w );

   //

   GenerateBitmapFile_CheckBox.SetText( "Generate bitmap file" );
   GenerateBitmapFile_CheckBox.SetToolTip( "<p>Enable this option to generate an output file in PNG format.</p>" );
   GenerateBitmapFile_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   GenerateBitmapFile_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   GenerateBitmapFile_Sizer.Add( GenerateBitmapFile_CheckBox );
   GenerateBitmapFile_Sizer.AddStretch();

   //

   const char* outputDirectoryToolTip = "<p>This is the directory (or folder) where the output PNG file will be written.</p>";

   OutputDirectory_Label.SetText( "Output directory:" );
   OutputDirectory_Label.SetFixedWidth( labelWidth1 );
   OutputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputDirectory_Label.SetToolTip( outputDirectoryToolTip );

   OutputDirectory_Edit.SetToolTip( outputDirectoryToolTip );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&FindingChartInterface::e_EditCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)&FindingChartInterface::e_FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)&FindingChartInterface::e_FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.SetToolTip( "<p>Select the output directory</p>" );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   //

   const char* outputSuffixToolTip = "<p>This is a suffix that will be appended to generated bitmap file names.</p>";

   OutputSuffix_Label.SetText( "Output suffix:" );
   OutputSuffix_Label.SetFixedWidth( labelWidth1 );
   OutputSuffix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputSuffix_Label.SetToolTip( outputSuffixToolTip );

   OutputSuffix_Edit.SetToolTip( outputSuffixToolTip );
   OutputSuffix_Edit.SetFixedWidth( editWidth1 );
   OutputSuffix_Edit.OnEditCompleted( (Edit::edit_event_handler)&FindingChartInterface::e_EditCompleted, w );

   OutputSuffix_Sizer.SetSpacing( 4 );
   OutputSuffix_Sizer.Add( OutputSuffix_Label );
   OutputSuffix_Sizer.Add( OutputSuffix_Edit );
   OutputSuffix_Sizer.AddStretch();

   //

   CloseOutputWindow_CheckBox.SetText( "Close output window" );
   CloseOutputWindow_CheckBox.SetToolTip( "<p>When the <i>generate bitmap file</i> option is enabled, close the "
      "output image window after the PNG file has been written. This option can be useful for automation of FindingChart "
      "execution on sets of images using ImageContainer or scripts.</p>" );
   CloseOutputWindow_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   CloseOutputWindow_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   CloseOutputWindow_Sizer.Add( CloseOutputWindow_CheckBox );
   CloseOutputWindow_Sizer.AddStretch();

   //

   Contents_SectionBar.SetTitle( "Contents" );
   Contents_SectionBar.SetSection( Contents_Control );

   //

   DrawGrid_CheckBox.SetText( "Coordinate grids" );
   DrawGrid_CheckBox.SetToolTip( "<p>Draw labeled grids for right ascension and declination coordinates.</p>" );
   DrawGrid_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   DrawGrid_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   DrawGrid_Sizer.Add( DrawGrid_CheckBox );
   DrawGrid_Sizer.AddStretch();

   //

   DrawEcliptic_CheckBox.SetText( "Ecliptic" );
   DrawEcliptic_CheckBox.SetToolTip( "<p>Draw the ecliptic great circle with ecliptic longitude labels.</p>" );
   DrawEcliptic_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   DrawEcliptic_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   DrawEcliptic_Sizer.Add( DrawEcliptic_CheckBox );
   DrawEcliptic_Sizer.AddStretch();

   //

   DrawGalacticEquator_CheckBox.SetText( "Galactic equator" );
   DrawGalacticEquator_CheckBox.SetToolTip( "<p>Draw the galactic equator great circle with galactic longitude labels.</p>" );
   DrawGalacticEquator_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   DrawGalacticEquator_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   DrawGalacticEquator_Sizer.Add( DrawGalacticEquator_CheckBox );
   DrawGalacticEquator_Sizer.AddStretch();

   //

   DrawConstellationBorders_CheckBox.SetText( "Constellation borders" );
   DrawConstellationBorders_CheckBox.SetToolTip( "<p>Draw constellation borders.</p>" );
   DrawConstellationBorders_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   DrawConstellationBorders_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   DrawConstellationBorders_Sizer.Add( DrawConstellationBorders_CheckBox );
   DrawConstellationBorders_Sizer.AddStretch();

   //

   DrawConstellationLines_CheckBox.SetText( "Constellation lines" );
   DrawConstellationLines_CheckBox.SetToolTip( "<p>Draw constellations by connecting stars with lines.</p>" );
   DrawConstellationLines_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   DrawConstellationLines_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   DrawConstellationLines_Sizer.Add( DrawConstellationLines_CheckBox );
   DrawConstellationLines_Sizer.AddStretch();

   //

   DrawConstellationNames_CheckBox.SetText( "Constellation names" );
   DrawConstellationNames_CheckBox.SetToolTip( "<p>Draw constellation names.</p>" );
   DrawConstellationNames_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   DrawConstellationNames_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   DrawConstellationNames_Sizer.Add( DrawConstellationNames_CheckBox );
   DrawConstellationNames_Sizer.AddStretch();

   //

   DrawStars_CheckBox.SetText( "Stars" );
   DrawStars_CheckBox.SetToolTip( "<p>Draw a synthetic star field.</p>" );
   DrawStars_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   DrawStars_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   DrawStars_Sizer.Add( DrawStars_CheckBox );
   DrawStars_Sizer.AddStretch();

   //

   DrawStarNames_CheckBox.SetText( "Star names" );
   DrawStarNames_CheckBox.SetToolTip( "<p>Draw the names of the brightest stars.</p>" );
   DrawStarNames_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   DrawStarNames_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   DrawStarNames_Sizer.Add( DrawStarNames_CheckBox );
   DrawStarNames_Sizer.AddStretch();

   //

   DrawMessierObjects_CheckBox.SetText( "Messier objects" );
   DrawMessierObjects_CheckBox.SetToolTip( "<p>Include Messier objects.</p>" );
   DrawMessierObjects_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   DrawMessierObjects_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   DrawMessierObjects_Sizer.Add( DrawMessierObjects_CheckBox );
   DrawMessierObjects_Sizer.AddStretch();

   //

   DrawNGCObjects_CheckBox.SetText( "NGC/IC objects" );
   DrawNGCObjects_CheckBox.SetToolTip( "<p>Include NGC/IC objects.</p>" );
   DrawNGCObjects_CheckBox.OnClick( (pcl::Button::click_event_handler)&FindingChartInterface::e_ItemClicked, w );

   DrawNGCObjects_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   DrawNGCObjects_Sizer.Add( DrawNGCObjects_CheckBox );
   DrawNGCObjects_Sizer.AddStretch();

   //

   Contents_Sizer.SetSpacing( 4 );
   Contents_Sizer.Add( DrawGrid_Sizer );
   Contents_Sizer.Add( DrawEcliptic_Sizer );
   Contents_Sizer.Add( DrawGalacticEquator_Sizer );
   Contents_Sizer.Add( DrawConstellationBorders_Sizer );
   Contents_Sizer.Add( DrawConstellationLines_Sizer );
   Contents_Sizer.Add( DrawConstellationNames_Sizer );
   Contents_Sizer.Add( DrawStars_Sizer );
   Contents_Sizer.Add( DrawStarNames_Sizer );
   Contents_Sizer.Add( DrawMessierObjects_Sizer );
   Contents_Sizer.Add( DrawNGCObjects_Sizer );

   Contents_Control.SetSizer( Contents_Sizer );

   //

   Colors_SectionBar.SetTitle( "Colors" );
   Colors_SectionBar.SetSection( Colors_Control );

   //

   const char* imageRegionColorToolTip = "<p>Fill color for the image region polygon.</p>";

   ImageRegionColor_Label.SetText( "Image region:" );
   ImageRegionColor_Label.SetMinWidth( labelWidth1 );
   ImageRegionColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ImageRegionColor_Label.SetToolTip( imageRegionColorToolTip );

   ImageRegionColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   ImageRegionColor_ComboBox.SetToolTip( imageRegionColorToolTip );

   ImageRegionColor_Control.SetScaledMinWidth( 30 );
   ImageRegionColor_Control.SetCursor( StdCursor::UpArrow );
   ImageRegionColor_Control.SetToolTip( imageRegionColorToolTip );
   ImageRegionColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   ImageRegionColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   ImageRegionColor_Sizer.SetSpacing( 4 );
   ImageRegionColor_Sizer.Add( ImageRegionColor_Label );
   ImageRegionColor_Sizer.Add( ImageRegionColor_ComboBox, 100 );
   ImageRegionColor_Sizer.Add( ImageRegionColor_Control );

   //

   const char* imageRegionBorderColorToolTip = "<p>Border color for the image region polygon and the top-left corner indicator.</p>";

   ImageRegionBorderColor_Label.SetText( "Image region border:" );
   ImageRegionBorderColor_Label.SetMinWidth( labelWidth1 );
   ImageRegionBorderColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ImageRegionBorderColor_Label.SetToolTip( imageRegionBorderColorToolTip );

   ImageRegionBorderColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   ImageRegionBorderColor_ComboBox.SetToolTip( imageRegionBorderColorToolTip );

   ImageRegionBorderColor_Control.SetScaledMinWidth( 30 );
   ImageRegionBorderColor_Control.SetCursor( StdCursor::UpArrow );
   ImageRegionBorderColor_Control.SetToolTip( imageRegionBorderColorToolTip );
   ImageRegionBorderColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   ImageRegionBorderColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   ImageRegionBorderColor_Sizer.SetSpacing( 4 );
   ImageRegionBorderColor_Sizer.Add( ImageRegionBorderColor_Label );
   ImageRegionBorderColor_Sizer.Add( ImageRegionBorderColor_ComboBox, 100 );
   ImageRegionBorderColor_Sizer.Add( ImageRegionBorderColor_Control );

   //

   const char* gridColorToolTip = "<p>Color of the coordinate grid lines.</p>";

   GridColor_Label.SetText( "Grid lines:" );
   GridColor_Label.SetMinWidth( labelWidth1 );
   GridColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GridColor_Label.SetToolTip( gridColorToolTip );

   GridColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   GridColor_ComboBox.SetToolTip( gridColorToolTip );

   GridColor_Control.SetScaledMinWidth( 30 );
   GridColor_Control.SetCursor( StdCursor::UpArrow );
   GridColor_Control.SetToolTip( gridColorToolTip );
   GridColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   GridColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   GridColor_Sizer.SetSpacing( 4 );
   GridColor_Sizer.Add( GridColor_Label );
   GridColor_Sizer.Add( GridColor_ComboBox, 100 );
   GridColor_Sizer.Add( GridColor_Control );

   //

   const char* gridTextColorToolTip = "<p>Color of the coordinate grid text labels.</p>";

   GridTextColor_Label.SetText( "Grid labels:" );
   GridTextColor_Label.SetMinWidth( labelWidth1 );
   GridTextColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GridTextColor_Label.SetToolTip( gridTextColorToolTip );

   GridTextColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   GridTextColor_ComboBox.SetToolTip( gridTextColorToolTip );

   GridTextColor_Control.SetScaledMinWidth( 30 );
   GridTextColor_Control.SetCursor( StdCursor::UpArrow );
   GridTextColor_Control.SetToolTip( gridTextColorToolTip );
   GridTextColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   GridTextColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   GridTextColor_Sizer.SetSpacing( 4 );
   GridTextColor_Sizer.Add( GridTextColor_Label );
   GridTextColor_Sizer.Add( GridTextColor_ComboBox, 100 );
   GridTextColor_Sizer.Add( GridTextColor_Control );

   //

   const char* eclipticColorToolTip = "<p>Color of the ecliptic great circle.</p>";

   EclipticColor_Label.SetText( "Ecliptic:" );
   EclipticColor_Label.SetMinWidth( labelWidth1 );
   EclipticColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   EclipticColor_Label.SetToolTip( eclipticColorToolTip );

   EclipticColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   EclipticColor_ComboBox.SetToolTip( eclipticColorToolTip );

   EclipticColor_Control.SetScaledMinWidth( 30 );
   EclipticColor_Control.SetCursor( StdCursor::UpArrow );
   EclipticColor_Control.SetToolTip( eclipticColorToolTip );
   EclipticColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   EclipticColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   EclipticColor_Sizer.SetSpacing( 4 );
   EclipticColor_Sizer.Add( EclipticColor_Label );
   EclipticColor_Sizer.Add( EclipticColor_ComboBox, 100 );
   EclipticColor_Sizer.Add( EclipticColor_Control );

   //

   const char* eclipticTextColorToolTip = "<p>Color of the ecliptic longitude labels.</p>";

   EclipticTextColor_Label.SetText( "Ecliptic longitude labels:" );
   EclipticTextColor_Label.SetMinWidth( labelWidth1 );
   EclipticTextColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   EclipticTextColor_Label.SetToolTip( eclipticTextColorToolTip );

   EclipticTextColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   EclipticTextColor_ComboBox.SetToolTip( eclipticTextColorToolTip );

   EclipticTextColor_Control.SetScaledMinWidth( 30 );
   EclipticTextColor_Control.SetCursor( StdCursor::UpArrow );
   EclipticTextColor_Control.SetToolTip( eclipticTextColorToolTip );
   EclipticTextColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   EclipticTextColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   EclipticTextColor_Sizer.SetSpacing( 4 );
   EclipticTextColor_Sizer.Add( EclipticTextColor_Label );
   EclipticTextColor_Sizer.Add( EclipticTextColor_ComboBox, 100 );
   EclipticTextColor_Sizer.Add( EclipticTextColor_Control );

   //

   const char* galacticEquatorColorToolTip = "<p>Color of the galactic equator great circle.</p>";

   GalacticEquatorColor_Label.SetText( "Galactic equator:" );
   GalacticEquatorColor_Label.SetMinWidth( labelWidth1 );
   GalacticEquatorColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GalacticEquatorColor_Label.SetToolTip( galacticEquatorColorToolTip );

   GalacticEquatorColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   GalacticEquatorColor_ComboBox.SetToolTip( galacticEquatorColorToolTip );

   GalacticEquatorColor_Control.SetScaledMinWidth( 30 );
   GalacticEquatorColor_Control.SetCursor( StdCursor::UpArrow );
   GalacticEquatorColor_Control.SetToolTip( galacticEquatorColorToolTip );
   GalacticEquatorColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   GalacticEquatorColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   GalacticEquatorColor_Sizer.SetSpacing( 4 );
   GalacticEquatorColor_Sizer.Add( GalacticEquatorColor_Label );
   GalacticEquatorColor_Sizer.Add( GalacticEquatorColor_ComboBox, 100 );
   GalacticEquatorColor_Sizer.Add( GalacticEquatorColor_Control );

   //

   const char* galacticEquatorTextColorToolTip = "<p>Color of the galactic longitude labels.</p>";

   GalacticEquatorTextColor_Label.SetText( "Galactic longitude labels:" );
   GalacticEquatorTextColor_Label.SetMinWidth( labelWidth1 );
   GalacticEquatorTextColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GalacticEquatorTextColor_Label.SetToolTip( galacticEquatorTextColorToolTip );

   GalacticEquatorTextColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   GalacticEquatorTextColor_ComboBox.SetToolTip( galacticEquatorTextColorToolTip );

   GalacticEquatorTextColor_Control.SetScaledMinWidth( 30 );
   GalacticEquatorTextColor_Control.SetCursor( StdCursor::UpArrow );
   GalacticEquatorTextColor_Control.SetToolTip( galacticEquatorTextColorToolTip );
   GalacticEquatorTextColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   GalacticEquatorTextColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   GalacticEquatorTextColor_Sizer.SetSpacing( 4 );
   GalacticEquatorTextColor_Sizer.Add( GalacticEquatorTextColor_Label );
   GalacticEquatorTextColor_Sizer.Add( GalacticEquatorTextColor_ComboBox, 100 );
   GalacticEquatorTextColor_Sizer.Add( GalacticEquatorTextColor_Control );

   //

   const char* constellationBorderColorToolTip = "<p>Color of the constellation border lines.</p>";

   ConstellationBorderColor_Label.SetText( "Constellation borders:" );
   ConstellationBorderColor_Label.SetMinWidth( labelWidth1 );
   ConstellationBorderColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ConstellationBorderColor_Label.SetToolTip( constellationBorderColorToolTip );

   ConstellationBorderColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   ConstellationBorderColor_ComboBox.SetToolTip( constellationBorderColorToolTip );

   ConstellationBorderColor_Control.SetScaledMinWidth( 30 );
   ConstellationBorderColor_Control.SetCursor( StdCursor::UpArrow );
   ConstellationBorderColor_Control.SetToolTip( constellationBorderColorToolTip );
   ConstellationBorderColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   ConstellationBorderColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   ConstellationBorderColor_Sizer.SetSpacing( 4 );
   ConstellationBorderColor_Sizer.Add( ConstellationBorderColor_Label );
   ConstellationBorderColor_Sizer.Add( ConstellationBorderColor_ComboBox, 100 );
   ConstellationBorderColor_Sizer.Add( ConstellationBorderColor_Control );

   //

   const char* constellationLineColorToolTip = "<p>Color of the constellation lines.</p>";

   ConstellationLineColor_Label.SetText( "Constellation lines:" );
   ConstellationLineColor_Label.SetMinWidth( labelWidth1 );
   ConstellationLineColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ConstellationLineColor_Label.SetToolTip( constellationLineColorToolTip );

   ConstellationLineColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   ConstellationLineColor_ComboBox.SetToolTip( constellationLineColorToolTip );

   ConstellationLineColor_Control.SetScaledMinWidth( 30 );
   ConstellationLineColor_Control.SetCursor( StdCursor::UpArrow );
   ConstellationLineColor_Control.SetToolTip( constellationLineColorToolTip );
   ConstellationLineColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   ConstellationLineColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   ConstellationLineColor_Sizer.SetSpacing( 4 );
   ConstellationLineColor_Sizer.Add( ConstellationLineColor_Label );
   ConstellationLineColor_Sizer.Add( ConstellationLineColor_ComboBox, 100 );
   ConstellationLineColor_Sizer.Add( ConstellationLineColor_Control );

   //

   const char* constellationTextColorToolTip = "<p>Color of the constellation text labels.</p>";

   ConstellationTextColor_Label.SetText( "Constellation labels:" );
   ConstellationTextColor_Label.SetMinWidth( labelWidth1 );
   ConstellationTextColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ConstellationTextColor_Label.SetToolTip( constellationTextColorToolTip );

   ConstellationTextColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   ConstellationTextColor_ComboBox.SetToolTip( constellationTextColorToolTip );

   ConstellationTextColor_Control.SetScaledMinWidth( 30 );
   ConstellationTextColor_Control.SetCursor( StdCursor::UpArrow );
   ConstellationTextColor_Control.SetToolTip( constellationTextColorToolTip );
   ConstellationTextColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   ConstellationTextColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   ConstellationTextColor_Sizer.SetSpacing( 4 );
   ConstellationTextColor_Sizer.Add( ConstellationTextColor_Label );
   ConstellationTextColor_Sizer.Add( ConstellationTextColor_ComboBox, 100 );
   ConstellationTextColor_Sizer.Add( ConstellationTextColor_Control );

   //

   const char* starTextColorToolTip = "<p>Color of the star markers and labels.</p>";

   StarTextColor_Label.SetText( "Star labels:" );
   StarTextColor_Label.SetMinWidth( labelWidth1 );
   StarTextColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StarTextColor_Label.SetToolTip( starTextColorToolTip );

   StarTextColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   StarTextColor_ComboBox.SetToolTip( starTextColorToolTip );

   StarTextColor_Control.SetScaledMinWidth( 30 );
   StarTextColor_Control.SetCursor( StdCursor::UpArrow );
   StarTextColor_Control.SetToolTip( starTextColorToolTip );
   StarTextColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   StarTextColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   StarTextColor_Sizer.SetSpacing( 4 );
   StarTextColor_Sizer.Add( StarTextColor_Label );
   StarTextColor_Sizer.Add( StarTextColor_ComboBox, 100 );
   StarTextColor_Sizer.Add( StarTextColor_Control );

   //

   const char* messierTextColorToolTip = "<p>Color of the markers and labels for Messier objects.</p>";

   MessierColor_Label.SetText( "Messier labels:" );
   MessierColor_Label.SetMinWidth( labelWidth1 );
   MessierColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MessierColor_Label.SetToolTip( messierTextColorToolTip );

   MessierColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   MessierColor_ComboBox.SetToolTip( messierTextColorToolTip );

   MessierColor_Control.SetScaledMinWidth( 30 );
   MessierColor_Control.SetCursor( StdCursor::UpArrow );
   MessierColor_Control.SetToolTip( messierTextColorToolTip );
   MessierColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   MessierColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   MessierColor_Sizer.SetSpacing( 4 );
   MessierColor_Sizer.Add( MessierColor_Label );
   MessierColor_Sizer.Add( MessierColor_ComboBox, 100 );
   MessierColor_Sizer.Add( MessierColor_Control );

   //

   const char* ngcTextColorToolTip = "<p>Color of the markers and labels for NGC/IC objects.</p>";

   NGCColor_Label.SetText( "NGC/IC labels:" );
   NGCColor_Label.SetMinWidth( labelWidth1 );
   NGCColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   NGCColor_Label.SetToolTip( ngcTextColorToolTip );

   NGCColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&FindingChartInterface::e_ColorSelected, w );
   NGCColor_ComboBox.SetToolTip( ngcTextColorToolTip );

   NGCColor_Control.SetScaledMinWidth( 30 );
   NGCColor_Control.SetCursor( StdCursor::UpArrow );
   NGCColor_Control.SetToolTip( ngcTextColorToolTip );
   NGCColor_Control.OnPaint( (Control::paint_event_handler)&FindingChartInterface::e_Paint, w );
   NGCColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&FindingChartInterface::e_MouseRelease, w );

   NGCColor_Sizer.SetSpacing( 4 );
   NGCColor_Sizer.Add( NGCColor_Label );
   NGCColor_Sizer.Add( NGCColor_ComboBox, 100 );
   NGCColor_Sizer.Add( NGCColor_Control );

   //

   Colors_Sizer.SetSpacing( 4 );
   Colors_Sizer.Add( ImageRegionColor_Sizer );
   Colors_Sizer.Add( ImageRegionBorderColor_Sizer );
   Colors_Sizer.Add( GridColor_Sizer );
   Colors_Sizer.Add( GridTextColor_Sizer );
   Colors_Sizer.Add( EclipticColor_Sizer );
   Colors_Sizer.Add( EclipticTextColor_Sizer );
   Colors_Sizer.Add( GalacticEquatorColor_Sizer );
   Colors_Sizer.Add( GalacticEquatorTextColor_Sizer );
   Colors_Sizer.Add( ConstellationBorderColor_Sizer );
   Colors_Sizer.Add( ConstellationLineColor_Sizer );
   Colors_Sizer.Add( ConstellationTextColor_Sizer );
   Colors_Sizer.Add( StarTextColor_Sizer );
   Colors_Sizer.Add( MessierColor_Sizer );
   Colors_Sizer.Add( NGCColor_Sizer );

   Colors_Control.SetSizer( Colors_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( ChartSize_Sizer );
   Global_Sizer.Add( AutoChartResolution_Sizer );
   Global_Sizer.Add( ChartResolution_NumericControl );
   Global_Sizer.Add( AutoChartMaxMagnitude_Sizer );
   Global_Sizer.Add( ChartMaxMagnitude_NumericControl );
   Global_Sizer.Add( GenerateBitmapFile_Sizer );
   Global_Sizer.Add( OutputDirectory_Sizer );
   Global_Sizer.Add( OutputSuffix_Sizer );
   Global_Sizer.Add( CloseOutputWindow_Sizer );
   Global_Sizer.Add( Contents_SectionBar );
   Global_Sizer.Add( Contents_Control );
   Global_Sizer.Add( Colors_SectionBar );
   Global_Sizer.Add( Colors_Control );

   w.SetSizer( Global_Sizer );

   Contents_Control.Hide();
   Colors_Control.Hide();

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FindingChartInterface.cpp - Released 2024-12-23T11:33:28Z
