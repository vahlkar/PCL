//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.0
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// IntegerResampleInterface.cpp - Released 2023-09-14T17:02:22Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "IntegerResampleInterface.h"
#include "IntegerResampleParameters.h"
#include "IntegerResampleProcess.h"

#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IntegerResampleInterface* TheIntegerResampleInterface = nullptr;

// ----------------------------------------------------------------------------

#define currentView  GUI->AllImages_ViewList.CurrentView()
#define downsample   GUI->Downsample_RadioButton.IsChecked()
#define upsample     GUI->Upsample_RadioButton.IsChecked()

// ----------------------------------------------------------------------------

IntegerResampleInterface::IntegerResampleInterface()
   : m_instance( TheIntegerResampleProcess )
{
   TheIntegerResampleInterface = this;
}

// ----------------------------------------------------------------------------

IntegerResampleInterface::~IntegerResampleInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString IntegerResampleInterface::Id() const
{
   return "IntegerResample";
}

// ----------------------------------------------------------------------------

MetaProcess* IntegerResampleInterface::Process() const
{
   return TheIntegerResampleProcess;
}

// ----------------------------------------------------------------------------

String IntegerResampleInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/IntegerResample.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures IntegerResampleInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::TrackViewButton;
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::TrackViewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
      {
         ImageWindow w = ImageWindow::ActiveWindow();
         if ( !w.IsNull() )
            ImageFocused( w.MainView() );
         else
            UpdateControls();
      }
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::ResetInstance()
{
   IntegerResampleInstance defaultInstance( TheIntegerResampleProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool IntegerResampleInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "IntegerResample" );
      GUI->AllImages_ViewList.Regenerate( true, false ); // exclude previews
      UpdateControls();
   }

   dynamic = false;
   return &P == TheIntegerResampleProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* IntegerResampleInterface::NewProcess() const
{
   return new IntegerResampleInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool IntegerResampleInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const IntegerResampleInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an IntegerResample instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool IntegerResampleInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool IntegerResampleInterface::ImportProcess( const ProcessImplementation& p )
{
   DeactivateTrackView();
   GUI->AllImages_ViewList.SelectView( View::Null() );

   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

bool IntegerResampleInterface::WantsImageNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::ImageUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( v == currentView )
      {
         v.Window().MainView().GetSize( sourceWidth, sourceHeight );
         UpdateControls();
      }
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::ImageFocused( const View& v )
{
   if ( !v.IsNull() )
      if ( GUI != nullptr )
         if ( IsTrackViewActive() )
         {
            ImageWindow w = v.Window();
            View mainView = w.MainView();

            mainView.GetSize( sourceWidth, sourceHeight );

            GUI->AllImages_ViewList.SelectView( mainView ); // normally not necessary, but we can invoke this f() directly

            double xRes, yRes;
            bool metric;
            w.GetResolution( xRes, yRes, metric );

            m_instance.p_resolution.x = xRes;
            m_instance.p_resolution.y = yRes;
            m_instance.p_metric = metric;

            UpdateControls();
         }
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::UpdateControls()
{
   GUI->ResampleFactor_SpinBox.SetValue( Abs( m_instance.p_zoomFactor ) );

   GUI->Downsample_RadioButton.SetChecked( m_instance.p_zoomFactor < 0 );
   GUI->Upsample_RadioButton.SetChecked( m_instance.p_zoomFactor > 0 );

   GUI->DownsampleMode_Label.Enable( m_instance.p_zoomFactor < 0 );
   GUI->DownsampleMode_ComboBox.Enable( m_instance.p_zoomFactor < 0 );
   GUI->DownsampleMode_ComboBox.SetCurrentItem( m_instance.p_downsampleMode );

   GUI->GammaCorrection_CheckBox.SetChecked( m_instance.p_gammaCorrection );

   int w = sourceWidth, h = sourceHeight;
   m_instance.GetNewSizes( w, h );

   double wcm, hcm, win, hin;

   if ( m_instance.p_metric )
   {
      wcm = w/m_instance.p_resolution.x;
      hcm = h/m_instance.p_resolution.y;
      win = wcm/2.54;
      hin = hcm/2.54;
   }
   else
   {
      win = w/m_instance.p_resolution.x;
      hin = h/m_instance.p_resolution.y;
      wcm = win*2.54;
      hcm = hin*2.54;
   }

   GUI->SourceWidthPixels_NumericEdit.SetValue( sourceWidth );
   GUI->SourceWidthPixels_NumericEdit.Enable( currentView.IsNull() );

   GUI->TargetWidthPixels_NumericEdit.SetValue( w );
   GUI->TargetWidthCentimeters_NumericEdit.SetValue( wcm );
   GUI->TargetWidthInches_NumericEdit.SetValue( win );

   GUI->SourceHeightPixels_NumericEdit.SetValue( sourceHeight );
   GUI->SourceHeightPixels_NumericEdit.Enable( currentView.IsNull() );

   GUI->TargetHeightPixels_NumericEdit.SetValue( h );
   GUI->TargetHeightCentimeters_NumericEdit.SetValue( hcm );
   GUI->TargetHeightInches_NumericEdit.SetValue( hin );

   String info;
   size_type wasArea = size_type( sourceWidth )*size_type( sourceHeight );
   size_type area = size_type( w )*size_type( h );
   if ( currentView.IsNull() )
      info.Format( "32-bit channel size: %.3f MiB, was %.3f MiB", (area*4)/1048576.0, (wasArea*4)/1048576.0 );
   else
   {
      ImageVariant image = currentView.Window().MainView().Image();
      size_type wasBytes = wasArea * image.NumberOfChannels() * image.BytesPerSample();
      size_type bytes = area * image.NumberOfChannels() * image.BytesPerSample();
      info.Format( "%d-bit total size: %.3f MiB, was %.3f MiB", image.BitsPerSample(), bytes/1048576.0, wasBytes/1048576.0 );
   }

   GUI->SizeInfo_Label.SetText( info );

   GUI->HorizontalResolution_NumericEdit.SetValue( m_instance.p_resolution.x );
   GUI->VerticalResolution_NumericEdit.SetValue( m_instance.p_resolution.y );

   GUI->CentimeterUnits_RadioButton.SetChecked( m_instance.p_metric );
   GUI->InchUnits_RadioButton.SetChecked( !m_instance.p_metric );

   GUI->ForceResolution_CheckBox.SetChecked( m_instance.p_forceResolution );
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::e_ViewSelected( ViewList& sender, View& view )
{
   if ( sender == GUI->AllImages_ViewList )
   {
      DeactivateTrackView();

      if ( !currentView.IsNull() )
      {
         ImageWindow w = currentView.Window();

         w.MainView().GetSize( sourceWidth, sourceHeight );

         double xRes, yRes;
         bool metric;
         w.GetResolution( xRes, yRes, metric );

         m_instance.p_resolution.x = xRes;
         m_instance.p_resolution.y = yRes;
         m_instance.p_metric = metric;
      }

      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::e_SpinBoxValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ResampleFactor_SpinBox )
   {
      m_instance.p_zoomFactor = downsample ? -value : +value;
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::e_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->Downsample_RadioButton || sender == GUI->Upsample_RadioButton )
   {
      if ( checked )
         m_instance.p_zoomFactor = -m_instance.p_zoomFactor;
   }
   else if ( sender == GUI->GammaCorrection_CheckBox )
      m_instance.p_gammaCorrection = checked;
   else if ( sender == GUI->CentimeterUnits_RadioButton )
      m_instance.p_metric = true;
   else if ( sender == GUI->InchUnits_RadioButton )
      m_instance.p_metric = false;
   else if ( sender == GUI->ForceResolution_CheckBox )
      m_instance.p_forceResolution = checked;

   UpdateControls();
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::e_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->SourceWidthPixels_NumericEdit )
      sourceWidth = int( value );
   else if ( sender == GUI->SourceHeightPixels_NumericEdit )
      sourceHeight = int( value );
   else if ( sender == GUI->HorizontalResolution_NumericEdit )
      m_instance.p_resolution.x = value;
   else if ( sender == GUI->VerticalResolution_NumericEdit )
      m_instance.p_resolution.y = value;

   UpdateControls();
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->DownsampleMode_ComboBox )
      m_instance.p_downsampleMode = itemIndex;
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->AllImages_ViewList )
      wantsView = view.IsMainView();
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->AllImages_ViewList )
      if ( view.IsMainView() )
      {
         GUI->AllImages_ViewList.SelectView( view );
         View theView = view;
         e_ViewSelected( GUI->AllImages_ViewList, theView );
      }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

IntegerResampleInterface::GUIData::GUIData( IntegerResampleInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Height:" ) + 'M' );
   int labelWidth2 = fnt.Width( String( "Horizontal:" ) + 'M' );
   int labelWidth3 = fnt.Width( String( "Resample factor:" ) + 'M' );
   int editWidth1 = fnt.Width( String( '0', 12 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );
   int ui6 = w.LogicalPixelsToPhysical( 6 );

   //

   AllImages_ViewList.OnViewSelected( (ViewList::view_event_handler)&IntegerResampleInterface::e_ViewSelected, w );
   AllImages_ViewList.OnViewDrag( (Control::view_drag_event_handler)&IntegerResampleInterface::e_ViewDrag, w );
   AllImages_ViewList.OnViewDrop( (Control::view_drop_event_handler)&IntegerResampleInterface::e_ViewDrop, w );

   //

   IntegerResample_SectionBar.SetTitle( "Integer Resample" );
   IntegerResample_SectionBar.SetSection( IntegerResample_Control );

   ResampleFactor_Label.SetText( "Resample factor:" );
   ResampleFactor_Label.SetMinWidth( labelWidth3 );
   ResampleFactor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ResampleFactor_SpinBox.SetRange( 1, int( TheIRZoomFactorParameter->MaximumValue() ) );
   ResampleFactor_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&IntegerResampleInterface::e_SpinBoxValueUpdated, w );

   ResampleFactor_Sizer.SetSpacing( 4 );
   ResampleFactor_Sizer.Add( ResampleFactor_Label );
   ResampleFactor_Sizer.Add( ResampleFactor_SpinBox );
   ResampleFactor_Sizer.AddStretch();

   DownsampleMode_Label.SetText( "Downsample mode:" );
   DownsampleMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   DownsampleMode_ComboBox.AddItem( "Average" );
   DownsampleMode_ComboBox.AddItem( "Median" );
   DownsampleMode_ComboBox.AddItem( "Maximum" );
   DownsampleMode_ComboBox.AddItem( "Minimum" );
   DownsampleMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&IntegerResampleInterface::e_ItemSelected, w );

   Downsample_RadioButton.SetText( "Downsample" );
   Downsample_RadioButton.OnClick( (Button::click_event_handler)&IntegerResampleInterface::e_ButtonClick, w );

   Downsample_Sizer.SetSpacing( 4 );
   Downsample_Sizer.AddUnscaledSpacing( labelWidth3 + ui4 );
   Downsample_Sizer.Add( Downsample_RadioButton );
   Downsample_Sizer.AddSpacing( 24 );
   Downsample_Sizer.Add( DownsampleMode_Label );
   Downsample_Sizer.Add( DownsampleMode_ComboBox );

   Upsample_RadioButton.SetText( "Upsample" );
   Upsample_RadioButton.OnClick( (Button::click_event_handler)&IntegerResampleInterface::e_ButtonClick, w );

   Upsample_Sizer.AddUnscaledSpacing( labelWidth3 + ui4 );
   Upsample_Sizer.Add( Upsample_RadioButton );

   GammaCorrection_CheckBox.SetText( "Gamma correction" );
   GammaCorrection_CheckBox.SetToolTip( "<p>Interpolate pixel sample values with gamma correction. In general, this option should be enabled to "
      "scale nonlinear images, i.e. images whose pixel values have been stretched nonlinearly. The applied transformations (gamma exponent or "
      "sRGB gamma function) depend on the RGB working space of the target image.</p>" );
   GammaCorrection_CheckBox.OnClick( (Button::click_event_handler)&IntegerResampleInterface::e_ButtonClick, w );

   GammaCorrection_Sizer.AddUnscaledSpacing( labelWidth3 + ui4 );
   GammaCorrection_Sizer.Add( GammaCorrection_CheckBox );
   GammaCorrection_Sizer.AddStretch();

   IntegerResample_Sizer.Add( ResampleFactor_Sizer );
   IntegerResample_Sizer.AddSpacing( 4 );
   IntegerResample_Sizer.Add( Downsample_Sizer );
   IntegerResample_Sizer.Add( Upsample_Sizer );
   IntegerResample_Sizer.AddSpacing( 6 );
   IntegerResample_Sizer.Add( GammaCorrection_Sizer );

   IntegerResample_Control.SetSizer( IntegerResample_Sizer );

   //

   Dimensions_SectionBar.SetTitle( "Dimensions" );
   Dimensions_SectionBar.SetSection( Dimensions_Control );

   SourcePixels_Label.SetText( "Original px" );
   SourcePixels_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   SourcePixels_Label.SetFixedWidth( editWidth1 );

   TargetPixels_Label.SetText( "Target px" );
   TargetPixels_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   TargetPixels_Label.SetFixedWidth( editWidth1 );

   TargetCentimeters_Label.SetText( "cm" );
   TargetCentimeters_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   TargetCentimeters_Label.SetFixedWidth( editWidth1 );

   TargetInches_Label.SetText( "inch" );
   TargetInches_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   TargetInches_Label.SetFixedWidth( editWidth1 );

   DimensionsRow1_Sizer.SetSpacing( 6 );
   DimensionsRow1_Sizer.AddUnscaledSpacing( labelWidth1 + ui6 );
   DimensionsRow1_Sizer.Add( SourcePixels_Label );
   DimensionsRow1_Sizer.Add( TargetPixels_Label );
   DimensionsRow1_Sizer.Add( TargetCentimeters_Label );
   DimensionsRow1_Sizer.Add( TargetInches_Label );

   Width_Label.SetText( "Width:" );
   Width_Label.SetMinWidth( labelWidth1 );
   Width_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   SourceWidthPixels_NumericEdit.label.Hide();
   SourceWidthPixels_NumericEdit.SetInteger();
   SourceWidthPixels_NumericEdit.SetRange( 1, int_max );
   SourceWidthPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   SourceWidthPixels_NumericEdit.SetFixedWidth( editWidth1 );
   SourceWidthPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&IntegerResampleInterface::e_ValueUpdated, w );

   TargetWidthPixels_NumericEdit.label.Hide();
   TargetWidthPixels_NumericEdit.SetInteger();
   TargetWidthPixels_NumericEdit.SetRange( 1, int_max );
   TargetWidthPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthPixels_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthPixels_NumericEdit.Disable();

   TargetWidthCentimeters_NumericEdit.label.Hide();
   TargetWidthCentimeters_NumericEdit.SetReal();
   TargetWidthCentimeters_NumericEdit.SetRange( 0.001, int_max );
   TargetWidthCentimeters_NumericEdit.SetPrecision( 3 );
   TargetWidthCentimeters_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthCentimeters_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthCentimeters_NumericEdit.Disable();

   TargetWidthInches_NumericEdit.label.Hide();
   TargetWidthInches_NumericEdit.SetReal();
   TargetWidthInches_NumericEdit.SetRange( 0.001, int_max );
   TargetWidthInches_NumericEdit.SetPrecision( 3 );
   TargetWidthInches_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthInches_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthInches_NumericEdit.Disable();

   DimensionsRow2_Sizer.SetSpacing( 6 );
   DimensionsRow2_Sizer.Add( Width_Label );
   DimensionsRow2_Sizer.Add( SourceWidthPixels_NumericEdit );
   DimensionsRow2_Sizer.Add( TargetWidthPixels_NumericEdit );
   DimensionsRow2_Sizer.Add( TargetWidthCentimeters_NumericEdit );
   DimensionsRow2_Sizer.Add( TargetWidthInches_NumericEdit );

   Height_Label.SetText( "Height:" );
   Height_Label.SetMinWidth( labelWidth1 );
   Height_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   SourceHeightPixels_NumericEdit.label.Hide();
   SourceHeightPixels_NumericEdit.SetInteger();
   SourceHeightPixels_NumericEdit.SetRange( 1, int_max );
   SourceHeightPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   SourceHeightPixels_NumericEdit.SetFixedWidth( editWidth1 );
   SourceHeightPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&IntegerResampleInterface::e_ValueUpdated, w );

   TargetHeightPixels_NumericEdit.label.Hide();
   TargetHeightPixels_NumericEdit.SetInteger();
   TargetHeightPixels_NumericEdit.SetRange( 1, int_max );
   TargetHeightPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightPixels_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightPixels_NumericEdit.Disable();

   TargetHeightCentimeters_NumericEdit.label.Hide();
   TargetHeightCentimeters_NumericEdit.SetReal();
   TargetHeightCentimeters_NumericEdit.SetRange( 0.001, int_max );
   TargetHeightCentimeters_NumericEdit.SetPrecision( 3 );
   TargetHeightCentimeters_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightCentimeters_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightCentimeters_NumericEdit.Disable();

   TargetHeightInches_NumericEdit.label.Hide();
   TargetHeightInches_NumericEdit.SetReal();
   TargetHeightInches_NumericEdit.SetRange( 0.001, int_max );
   TargetHeightInches_NumericEdit.SetPrecision( 3 );
   TargetHeightInches_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightInches_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightInches_NumericEdit.Disable();

   DimensionsRow3_Sizer.SetSpacing( 6 );
   DimensionsRow3_Sizer.Add( Height_Label );
   DimensionsRow3_Sizer.Add( SourceHeightPixels_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightPixels_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightCentimeters_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightInches_NumericEdit );

   SizeInfo_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   DimensionsRow4_Sizer.AddUnscaledSpacing( labelWidth1 + ui6 );
   DimensionsRow4_Sizer.Add( SizeInfo_Label, 100 );

   Dimensions_Sizer.SetSpacing( 4 );
   Dimensions_Sizer.Add( DimensionsRow1_Sizer );
   Dimensions_Sizer.Add( DimensionsRow2_Sizer );
   Dimensions_Sizer.Add( DimensionsRow3_Sizer );
   Dimensions_Sizer.Add( DimensionsRow4_Sizer );

   Dimensions_Control.SetSizer( Dimensions_Sizer );

   //

   Resolution_SectionBar.SetTitle( "Resolution" );
   Resolution_SectionBar.SetSection( Resolution_Control );

   HorizontalResolution_NumericEdit.label.SetText( "Horizontal:" );
   HorizontalResolution_NumericEdit.label.SetFixedWidth( labelWidth2 );
   HorizontalResolution_NumericEdit.SetReal();
   HorizontalResolution_NumericEdit.SetRange( 1, 10000 );
   HorizontalResolution_NumericEdit.SetPrecision( 3 );
   HorizontalResolution_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&IntegerResampleInterface::e_ValueUpdated, w );

   VerticalResolution_NumericEdit.label.SetText( "Vertical:" );
   VerticalResolution_NumericEdit.SetReal();
   VerticalResolution_NumericEdit.SetRange( 1, 10000 );
   VerticalResolution_NumericEdit.SetPrecision( 3 );
   VerticalResolution_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&IntegerResampleInterface::e_ValueUpdated, w );

   ResolutionRow1_Sizer.SetSpacing( 16 );
   ResolutionRow1_Sizer.Add( HorizontalResolution_NumericEdit );
   ResolutionRow1_Sizer.Add( VerticalResolution_NumericEdit );
   ResolutionRow1_Sizer.AddStretch();

   CentimeterUnits_RadioButton.SetText( "Centimeters" );
   CentimeterUnits_RadioButton.OnClick( (Button::click_event_handler)&IntegerResampleInterface::e_ButtonClick, w );

   InchUnits_RadioButton.SetText( "Inches" );
   InchUnits_RadioButton.OnClick( (Button::click_event_handler)&IntegerResampleInterface::e_ButtonClick, w );

   ForceResolution_CheckBox.SetText( "Force resolution" );
   ForceResolution_CheckBox.SetToolTip( "Modify resolution metadata of target image(s)" );
   ForceResolution_CheckBox.OnClick( (Button::click_event_handler)&IntegerResampleInterface::e_ButtonClick, w );

   ResolutionRow2_Sizer.SetSpacing( 8 );
   ResolutionRow2_Sizer.AddUnscaledSpacing( labelWidth2 + ui4 );
   ResolutionRow2_Sizer.Add( CentimeterUnits_RadioButton );
   ResolutionRow2_Sizer.Add( InchUnits_RadioButton );
   ResolutionRow2_Sizer.Add( ForceResolution_CheckBox );
   ResolutionRow2_Sizer.AddStretch();

   Resolution_Sizer.SetSpacing( 6 );
   Resolution_Sizer.Add( ResolutionRow1_Sizer );
   Resolution_Sizer.Add( ResolutionRow2_Sizer );

   Resolution_Control.SetSizer( Resolution_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( AllImages_ViewList );
   Global_Sizer.Add( IntegerResample_SectionBar );
   Global_Sizer.Add( IntegerResample_Control );
   Global_Sizer.Add( Dimensions_SectionBar );
   Global_Sizer.Add( Dimensions_Control );
   Global_Sizer.Add( Resolution_SectionBar );
   Global_Sizer.Add( Resolution_Control );

   Resolution_Control.Hide();

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegerResampleInterface.cpp - Released 2023-09-14T17:02:22Z
