#include "StarNetInterface.h"
#include "StarNetParameters.h"
#include "StarNetProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

StarNetInterface* TheStarNetInterface = nullptr;

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

void StarNetInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
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
   GUI->MaskParameter_CheckBox.SetChecked( m_instance.p_mask );
   GUI->StrideParameter_ComboBox.SetCurrentItem( m_instance.p_stride );
   GUI->RGBWeightsFile_Edit.SetText( TheStarNetProcess->RGBWeightsFilePath() );
   GUI->GrayscaleWeightsFile_Edit.SetText( TheStarNetProcess->GrayscaleWeightsFilePath() );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void StarNetInterface::__ItemClicked( Button& sender, bool checked )
{
   if ( sender == GUI->MaskParameter_CheckBox )
      m_instance.p_mask = checked;
}

// ----------------------------------------------------------------------------

void StarNetInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->StrideParameter_ComboBox )
      m_instance.p_stride = itemIndex;
}

// ----------------------------------------------------------------------------

void StarNetInterface::__EditCompleted( Edit& sender )
{
   try
   {
      String text = sender.Text().Trimmed();

      if ( sender == GUI->RGBWeightsFile_Edit )
      {
         if ( !text.IsEmpty() )
            TheStarNetProcess->SetRGBWeightsFilePath( text );
      }
      else if ( sender == GUI->GrayscaleWeightsFile_Edit )
      {
         if ( !text.IsEmpty() )
            TheStarNetProcess->SetGrayscaleWeightsFilePath( text );
      }

      UpdateControls();
   }
   ERROR_CLEANUP(
      sender.SelectAll();
      sender.Focus()
   )
}

// ----------------------------------------------------------------------------

void StarNetInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->RGBWeightsFile_ToolButton )
   {
      OpenFileDialog d;
      d.SetCaption( "StarNet: Select RGB Weights File" );
      d.AddFilter( FileFilter( "StarNet Weights Files", ".pb" ) );
      d.DisableMultipleSelections();
      if ( d.Execute() )
      {
         TheStarNetProcess->SetRGBWeightsFilePath( d.FileName() );
         UpdateControls();
      }
   }
   else if ( sender == GUI->GrayscaleWeightsFile_ToolButton )
   {
      OpenFileDialog d;
      d.SetCaption( "StarNet: Select Grayscale Weights File" );
      d.AddFilter( FileFilter( "StarNet Weights Files", ".pb" ) );
      d.DisableMultipleSelections();
      if ( d.Execute() )
      {
         TheStarNetProcess->SetGrayscaleWeightsFilePath( d.FileName() );
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------

void StarNetInterface::__FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->RGBWeightsFile_Edit || sender == GUI->GrayscaleWeightsFile_Edit )
      wantsFiles = files.Length() == 1 && File::Exists( files[0] );
}

// ----------------------------------------------------------------------------

void StarNetInterface::__FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->RGBWeightsFile_Edit )
   {
      if ( File::Exists( files[0] ) )
      {
         TheStarNetProcess->SetRGBWeightsFilePath( files[0] );
         UpdateControls();
      }
   }
   else if ( sender == GUI->GrayscaleWeightsFile_Edit )
   {
      if ( File::Exists( files[0] ) )
      {
         TheStarNetProcess->SetGrayscaleWeightsFilePath( files[0] );
         UpdateControls();
      }
   }
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
      "<p>The target image will be transformed tile by tile, each tile having size stride by stride.</p>"
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
   StrideParameter_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&StarNetInterface::__ItemSelected, w );

   StrideParameter_Sizer.SetSpacing( 4 );
   StrideParameter_Sizer.Add( StrideParameter_Label );
   StrideParameter_Sizer.Add( StrideParameter_ComboBox );
   StrideParameter_Sizer.AddStretch();

   //

   MaskParameter_CheckBox.SetText( "Create starmask" );
   MaskParameter_CheckBox.SetToolTip( "<p>Generate a star mask instead of a starless image.</p>" );
   MaskParameter_CheckBox.OnClick( (pcl::Button::click_event_handler)&StarNetInterface::__ItemClicked, w );

   MaskParameter_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   MaskParameter_Sizer.Add( MaskParameter_CheckBox );
   MaskParameter_Sizer.AddStretch();

   //

   const char* rgbWeightsFileToolTip = "<p>Path to the RGB weights file.</p>";

   RGBWeightsFile_Label.SetText( "RGB weights file:" );
   RGBWeightsFile_Label.SetFixedWidth( labelWidth1 );
   RGBWeightsFile_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RGBWeightsFile_Label.SetToolTip( rgbWeightsFileToolTip );

   RGBWeightsFile_Edit.SetToolTip( rgbWeightsFileToolTip );
   RGBWeightsFile_Edit.SetMinWidth( editWidth1 );
   RGBWeightsFile_Edit.OnEditCompleted( (Edit::edit_event_handler)&StarNetInterface::__EditCompleted, w );
   RGBWeightsFile_Edit.OnFileDrag( (Control::file_drag_event_handler)&StarNetInterface::__FileDrag, w );
   RGBWeightsFile_Edit.OnFileDrop( (Control::file_drop_event_handler)&StarNetInterface::__FileDrop, w );

   RGBWeightsFile_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select.png" ) ) );
   RGBWeightsFile_ToolButton.SetScaledFixedSize( 20, 20 );
   RGBWeightsFile_ToolButton.SetToolTip( "<p>Select the reference image</p>" );
   RGBWeightsFile_ToolButton.OnClick( (Button::click_event_handler)&StarNetInterface::__Click, w );

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
   GrayscaleWeightsFile_Edit.OnEditCompleted( (Edit::edit_event_handler)&StarNetInterface::__EditCompleted, w );
   GrayscaleWeightsFile_Edit.OnFileDrag( (Control::file_drag_event_handler)&StarNetInterface::__FileDrag, w );
   GrayscaleWeightsFile_Edit.OnFileDrop( (Control::file_drop_event_handler)&StarNetInterface::__FileDrop, w );

   GrayscaleWeightsFile_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select.png" ) ) );
   GrayscaleWeightsFile_ToolButton.SetScaledFixedSize( 20, 20 );
   GrayscaleWeightsFile_ToolButton.SetToolTip( "<p>Select the reference image</p>" );
   GrayscaleWeightsFile_ToolButton.OnClick( (Button::click_event_handler)&StarNetInterface::__Click, w );

   GrayscaleWeightsFile_Sizer.SetSpacing( 4 );
   GrayscaleWeightsFile_Sizer.Add( GrayscaleWeightsFile_Label );
   GrayscaleWeightsFile_Sizer.Add( GrayscaleWeightsFile_Edit, 100 );
   GrayscaleWeightsFile_Sizer.Add( GrayscaleWeightsFile_ToolButton );

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
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl
