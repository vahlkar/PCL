//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard StarNet Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// StarNetInterface.cpp - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarNet PixInsight module.
//
// Copyright (c) 2018-2020 Nikita Misiura
//
// This software is available under Attribution-NonCommercial-ShareAlike 4.0
// International Creative Commons license (CC BY-NC-SA 4.0):
// 
// https://creativecommons.org/licenses/by-nc-sa/4.0/
// 
// In short: You are free to use and redistribute the code in any medium or
// format, but only under the same license terms. You can transform and build
// your projects upon it. You can NOT use the code for commercial purposes. You
// must give appropriate credit for usage of the code.
// 
// This product is based on software from the PixInsight project, developed by
// Pleiades Astrophoto and its contributors:
// 
// https://pixinsight.com/
// ----------------------------------------------------------------------------

#include "StarNetInterface.h"
#include "StarNetParameters.h"
#include "StarNetProcess.h"

#include <pcl/Dialog.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/PushButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

StarNetInterface* TheStarNetInterface = nullptr;

// ----------------------------------------------------------------------------

class StarNetPreferencesDialog : public Dialog
{
public:

   StarNetPreferencesDialog();

private:

   String m_rgbWeightsFilePath;
   String m_grayscaleWeightsFilePath;

   VerticalSizer     Global_Sizer;
      HorizontalSizer   RGBWeightsFile_Sizer;
         Label             RGBWeightsFile_Label;
         Edit              RGBWeightsFile_Edit;
         ToolButton        RGBWeightsFile_ToolButton;
      HorizontalSizer   GrayscaleWeightsFile_Sizer;
         Label             GrayscaleWeightsFile_Label;
         Edit              GrayscaleWeightsFile_Edit;
         ToolButton        GrayscaleWeightsFile_ToolButton;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void UpdateControls();

   void e_EditCompleted( Edit& sender );
   void e_Click( Button& sender, bool checked );
   void e_Return( Dialog& sender, int retVal );
};

StarNetPreferencesDialog::StarNetPreferencesDialog()
{
   int labelWidth1 = Font().Width( String( "Grayscale weights file:" ) ); // the longest label text
   int editWidth1 = Font().Width( 'M' )*40;

   //

   const char* rgbWeightsFileToolTip = "<p>Path to the RGB weights file.</p>";

   RGBWeightsFile_Label.SetText( "RGB weights file:" );
   RGBWeightsFile_Label.SetFixedWidth( labelWidth1 );
   RGBWeightsFile_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RGBWeightsFile_Label.SetToolTip( rgbWeightsFileToolTip );

   RGBWeightsFile_Edit.SetToolTip( rgbWeightsFileToolTip );
   RGBWeightsFile_Edit.SetMinWidth( editWidth1 );
   RGBWeightsFile_Edit.OnEditCompleted( (Edit::edit_event_handler)&StarNetPreferencesDialog::e_EditCompleted, *this );

   RGBWeightsFile_ToolButton.SetIcon( ScaledResource( ":/browser/select-file.png" ) );
   RGBWeightsFile_ToolButton.SetScaledFixedSize( 20, 20 );
   RGBWeightsFile_ToolButton.SetToolTip( "<p>Select the RGB weights file.</p>" );
   RGBWeightsFile_ToolButton.OnClick( (Button::click_event_handler)&StarNetPreferencesDialog::e_Click, *this );

   RGBWeightsFile_Sizer.SetSpacing( 4 );
   RGBWeightsFile_Sizer.Add( RGBWeightsFile_Label );
   RGBWeightsFile_Sizer.Add( RGBWeightsFile_Edit, 100 );
   RGBWeightsFile_Sizer.Add( RGBWeightsFile_ToolButton );

   //

   const char* grayscaleWeightsFileToolTip = "<p>Path to the grayscale weights file.</p>";

   GrayscaleWeightsFile_Label.SetText( "Grayscale weights file:" );
   GrayscaleWeightsFile_Label.SetFixedWidth( labelWidth1 );
   GrayscaleWeightsFile_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GrayscaleWeightsFile_Label.SetToolTip( grayscaleWeightsFileToolTip );

   GrayscaleWeightsFile_Edit.SetToolTip( grayscaleWeightsFileToolTip );
   GrayscaleWeightsFile_Edit.SetMinWidth( editWidth1 );
   GrayscaleWeightsFile_Edit.OnEditCompleted( (Edit::edit_event_handler)&StarNetPreferencesDialog::e_EditCompleted, *this );

   GrayscaleWeightsFile_ToolButton.SetIcon( ScaledResource( ":/browser/select-file.png" ) );
   GrayscaleWeightsFile_ToolButton.SetScaledFixedSize( 20, 20 );
   GrayscaleWeightsFile_ToolButton.SetToolTip( "<p>Select the grayscale weights file.</p>" );
   GrayscaleWeightsFile_ToolButton.OnClick( (Button::click_event_handler)&StarNetPreferencesDialog::e_Click, *this );

   GrayscaleWeightsFile_Sizer.SetSpacing( 4 );
   GrayscaleWeightsFile_Sizer.Add( GrayscaleWeightsFile_Label );
   GrayscaleWeightsFile_Sizer.Add( GrayscaleWeightsFile_Edit, 100 );
   GrayscaleWeightsFile_Sizer.Add( GrayscaleWeightsFile_ToolButton );

   //

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&StarNetPreferencesDialog::e_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&StarNetPreferencesDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( RGBWeightsFile_Sizer );
   Global_Sizer.Add( GrayscaleWeightsFile_Sizer );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetFixedHeight();
   SetMinWidth();

   SetWindowTitle( "StarNet Preferences" );

   OnReturn( (pcl::Dialog::return_event_handler)&StarNetPreferencesDialog::e_Return, *this );

   if ( !TheStarNetProcess->PreferencesLoaded() )
      TheStarNetProcess->LoadPreferences();

   m_rgbWeightsFilePath = TheStarNetProcess->RGBWeightsFilePath();
   m_grayscaleWeightsFilePath = TheStarNetProcess->GrayscaleWeightsFilePath();

   UpdateControls();
}

void StarNetPreferencesDialog::UpdateControls()
{
   RGBWeightsFile_Edit.SetText( m_rgbWeightsFilePath );
   GrayscaleWeightsFile_Edit.SetText( m_grayscaleWeightsFilePath );
}

void StarNetPreferencesDialog::e_EditCompleted( Edit& sender )
{
   try
   {
      String filePath = sender.Text().Trimmed();
      if ( sender == RGBWeightsFile_Edit )
         m_rgbWeightsFilePath = filePath;
      else if ( sender == GrayscaleWeightsFile_Edit )
         m_grayscaleWeightsFilePath = filePath;
      UpdateControls();
   }
   ERROR_CLEANUP(
      sender.SelectAll();
      sender.Focus()
   )
}

void StarNetPreferencesDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == RGBWeightsFile_ToolButton )
   {
      OpenFileDialog d;
      d.SetCaption( "StarNet: Select RGB Weights File" );
      d.AddFilter( FileFilter( "StarNet Weights Files", ".pb" ) );
      d.AddFilter( FileFilter( "Any Files", "*" ) );
      d.DisableMultipleSelections();
      if ( d.Execute() )
      {
         m_rgbWeightsFilePath = d.FileName();
         UpdateControls();
      }
   }
   else if ( sender == GrayscaleWeightsFile_ToolButton )
   {
      OpenFileDialog d;
      d.SetCaption( "StarNet: Select Grayscale Weights File" );
      d.AddFilter( FileFilter( "StarNet Weights Files", ".pb" ) );
      d.AddFilter( FileFilter( "Any Files", "*" ) );
      d.DisableMultipleSelections();
      if ( d.Execute() )
      {
         m_grayscaleWeightsFilePath = d.FileName();
         UpdateControls();
      }
   }
   else if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

void StarNetPreferencesDialog::e_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      TheStarNetProcess->SetRGBWeightsFilePath( m_rgbWeightsFilePath );
      TheStarNetProcess->SetGrayscaleWeightsFilePath( m_grayscaleWeightsFilePath );
      if ( TheStarNetInterface != nullptr )
         TheStarNetInterface->UpdateControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

StarNetInterface::StarNetInterface()
   : m_instance( TheStarNetProcess )
{
   TheStarNetInterface = this;
}

// ----------------------------------------------------------------------------

StarNetInterface::~StarNetInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString StarNetInterface::Id() const
{
   return "StarNet";
}

// ----------------------------------------------------------------------------

MetaProcess* StarNetInterface::Process() const
{
   return TheStarNetProcess;
}

// ----------------------------------------------------------------------------

String StarNetInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/StarNet.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures StarNetInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::PreferencesButton;
}

// ----------------------------------------------------------------------------

void StarNetInterface::EditPreferences()
{
   StarNetPreferencesDialog().Execute();
}

// ----------------------------------------------------------------------------

void StarNetInterface::ResetInstance()
{
   StarNetInstance defaultInstance( TheStarNetProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool StarNetInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( !TheStarNetProcess->PreferencesLoaded() )
      TheStarNetProcess->LoadPreferences();

   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "StarNet" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheStarNetProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* StarNetInterface::NewProcess() const
{
   return new StarNetInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool StarNetInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const StarNetInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a StarNet instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool StarNetInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool StarNetInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void StarNetInterface::UpdateControls()
{
   GUI->StrideParameter_ComboBox.SetCurrentItem( m_instance.p_stride );

   GUI->MaskParameter_CheckBox.SetChecked( m_instance.p_mask );

   GUI->RGBWeightsFile_Edit.SetText( TheStarNetProcess->RGBWeightsFilePath() );

   {
      Bitmap bmp( ScaledResource( TheStarNetProcess->IsValidRGBWeightsFilePath() ?
                                                   ":/icons/ok.png" : ":/icons/error.png" ) );
#ifdef __PCL_MACOSX
      bmp.SetPhysicalPixelRatio( ResourcePixelRatio() );
#endif
      GUI->RGBWeightsFile_BitmapBox.SetBitmap( bmp );
   }

   GUI->GrayscaleWeightsFile_Edit.SetText( TheStarNetProcess->GrayscaleWeightsFilePath() );

   {
      Bitmap bmp( ScaledResource( TheStarNetProcess->IsValidGrayscaleWeightsFilePath() ?
                                                ":/icons/ok.png" : ":/icons/error.png" ) );
#ifdef __PCL_MACOSX
      bmp.SetPhysicalPixelRatio( ResourcePixelRatio() );
#endif
      GUI->GrayscaleWeightsFile_BitmapBox.SetBitmap( bmp );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void StarNetInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->StrideParameter_ComboBox )
      m_instance.p_stride = itemIndex;
}

// ----------------------------------------------------------------------------

void StarNetInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->MaskParameter_CheckBox )
      m_instance.p_mask = checked;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

StarNetInterface::GUIData::GUIData( StarNetInterface& w )
{
   pcl::Font font = w.Font();
   int labelWidth1 = font.Width( String( "Grayscale weights file:" ) ); // the longest label text
   int editWidth1 = font.Width( 'M' )*40;

   //

   const char* strideToolTip =
   "<p>The target image will be transformed tile by tile, each tile having size stride by stride pixels.</p>"
   "<p>Decreasing this value might help with edge artifacts on large stars, but it is usually not worth it.</p>";

   StrideParameter_Label.SetText( "Stride:" );
   StrideParameter_Label.SetMinWidth( labelWidth1 );
   StrideParameter_Label.SetToolTip( strideToolTip );
   StrideParameter_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StrideParameter_ComboBox.AddItem( "128" );
   StrideParameter_ComboBox.AddItem( "64" );
   StrideParameter_ComboBox.AddItem( "32" );
   StrideParameter_ComboBox.AddItem( "16" );
   StrideParameter_ComboBox.AddItem( "8" );
   StrideParameter_ComboBox.SetToolTip( strideToolTip );
   StrideParameter_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&StarNetInterface::e_ItemSelected, w );

   StrideParameter_Sizer.SetSpacing( 4 );
   StrideParameter_Sizer.Add( StrideParameter_Label );
   StrideParameter_Sizer.Add( StrideParameter_ComboBox );
   StrideParameter_Sizer.AddStretch();

   //

   MaskParameter_CheckBox.SetText( "Create star mask" );
   MaskParameter_CheckBox.SetToolTip( "<p>Generate a star mask along with a starless image.</p>" );
   MaskParameter_CheckBox.OnClick( (pcl::Button::click_event_handler)&StarNetInterface::e_Click, w );

   MaskParameter_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   MaskParameter_Sizer.Add( MaskParameter_CheckBox );
   MaskParameter_Sizer.AddStretch();

   //

   const char* grayscaleWeightsFileIndicatorToolTip =
   "<p>A red icon with a cross means that the corresponding weights file has either not been selected, "
   "does not exist, or is invalid.</p>"
   "<p>A green checkmark icon means that the selected file is correct and ready to be used.</p>";

   const char* rgbWeightsFileToolTip =
   "<p>Path to the RGB weights file.</p>"
   "<p>To select an RGB weights file, open the StarNet Preferences dialog by clicking the corresponding "
   "button (wrench icon) on the tool's control bar. Selected weights files will be stored as persistent "
   "module settings, and hence remembered across PixInsight sessions.</p>";

   RGBWeightsFile_Label.SetText( "RGB weights file:" );
   RGBWeightsFile_Label.SetFixedWidth( labelWidth1 );
   RGBWeightsFile_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RGBWeightsFile_Label.SetToolTip( rgbWeightsFileToolTip );

   RGBWeightsFile_Edit.SetToolTip( rgbWeightsFileToolTip );
   RGBWeightsFile_Edit.SetMinWidth( editWidth1 );
   RGBWeightsFile_Edit.SetReadOnly();

   RGBWeightsFile_BitmapBox.SetScaledFixedSize( 20, 20 );
   RGBWeightsFile_BitmapBox.SetToolTip( grayscaleWeightsFileIndicatorToolTip );

   RGBWeightsFile_Sizer.SetSpacing( 4 );
   RGBWeightsFile_Sizer.Add( RGBWeightsFile_Label );
   RGBWeightsFile_Sizer.Add( RGBWeightsFile_Edit, 100 );
   RGBWeightsFile_Sizer.Add( RGBWeightsFile_BitmapBox );

   //

   const char* grayscaleWeightsFileToolTip =
   "<p>Path to the grayscale weights file.</p>"
   "<p>To select a grayscale weights file, open the StarNet Preferences dialog by clicking the corresponding "
   "button (wrench icon) on the tool's control bar. Selected weights files will be stored as persistent "
   "module settings, and hence remembered across PixInsight sessions.</p>";

   GrayscaleWeightsFile_Label.SetText( "Grayscale weights file:" );
   GrayscaleWeightsFile_Label.SetFixedWidth( labelWidth1 );
   GrayscaleWeightsFile_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GrayscaleWeightsFile_Label.SetToolTip( grayscaleWeightsFileToolTip );

   GrayscaleWeightsFile_Edit.SetToolTip( grayscaleWeightsFileToolTip );
   GrayscaleWeightsFile_Edit.SetMinWidth( editWidth1 );
   GrayscaleWeightsFile_Edit.SetReadOnly();

   GrayscaleWeightsFile_BitmapBox.SetScaledFixedSize( 20, 20 );
   GrayscaleWeightsFile_BitmapBox.SetToolTip( grayscaleWeightsFileIndicatorToolTip );

   GrayscaleWeightsFile_Sizer.SetSpacing( 4 );
   GrayscaleWeightsFile_Sizer.Add( GrayscaleWeightsFile_Label );
   GrayscaleWeightsFile_Sizer.Add( GrayscaleWeightsFile_Edit, 100 );
   GrayscaleWeightsFile_Sizer.Add( GrayscaleWeightsFile_BitmapBox );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( StrideParameter_Sizer );
   Global_Sizer.Add( MaskParameter_Sizer );
   Global_Sizer.Add( RGBWeightsFile_Sizer );
   Global_Sizer.Add( GrayscaleWeightsFile_Sizer );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetMinWidth();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF StarNetInterface.cpp - Released 2021-04-09T19:41:49Z
