//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.35
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 1.11.0
// ----------------------------------------------------------------------------
// DebayerInterface.cpp - Released 2022-11-21T14:47:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
//
// Copyright (c) 2003-2022 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "DebayerInterface.h"
#include "DebayerProcess.h"
#include "DebayerParameters.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/Settings.h>

#define IMAGELIST_MINHEIGHT( fnt )  RoundInt( 8.125*fnt.Height() )

namespace pcl
{

// ----------------------------------------------------------------------------

DebayerInterface* TheDebayerInterface = nullptr;

// ----------------------------------------------------------------------------

DebayerInterface::DebayerInterface()
   : m_instance( TheDebayerProcess )
{
   TheDebayerInterface = this;

   /*
    * The auto save geometry feature is of no good to interfaces that include
    * both auto-expanding controls (e.g. TreeBox) and collapsible sections
    * (e.g. SectionBar).
    */
   DisableAutoSaveGeometry();
}

// ----------------------------------------------------------------------------

DebayerInterface::~DebayerInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString DebayerInterface::Id() const
{
   return "Debayer";
}

// ----------------------------------------------------------------------------

MetaProcess* DebayerInterface::Process() const
{
   return TheDebayerProcess;
}

// ----------------------------------------------------------------------------

String DebayerInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/Debayer.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures DebayerInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::ApplyGlobalButton;
}

// ----------------------------------------------------------------------------

void DebayerInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void DebayerInterface::ResetInstance()
{
   DebayerInstance defaultInstance( TheDebayerProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool DebayerInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Debayer" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheDebayerProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* DebayerInterface::NewProcess() const
{
   return new DebayerInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool DebayerInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const DebayerInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Debayer instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool DebayerInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool DebayerInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void DebayerInterface::UpdateControls()
{
   UpdateGeneralParameterControls();
   UpdateTargetImagesList();
   UpdateImageSelectionButtons();
   UpdateFormatHintsControls();
   UpdateOutputFilesControls();
   UpdateSignalAndNoiseEvaluationControls();
}

// ----------------------------------------------------------------------------

void DebayerInterface::UpdateGeneralParameterControls()
{
   GUI->Pattern_ComboBox.SetCurrentItem( m_instance.p_bayerPattern );

   GUI->DebayerMethod_ComboBox.SetCurrentItem( m_instance.p_debayerMethod );

   if ( m_instance.p_outputRGBImages )
      if ( m_instance.p_outputSeparateChannels )
         GUI->OutputMode_ComboBox.SetCurrentItem( 2 );
      else
         GUI->OutputMode_ComboBox.SetCurrentItem( 0 );
   else
      GUI->OutputMode_ComboBox.SetCurrentItem( 1 );
}

// ----------------------------------------------------------------------------

void DebayerInterface::UpdateTargetImageItem( size_type i )
{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const DebayerInstance::Item& item = m_instance.p_targets[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, Bitmap( ScaledResource( ":/browser/document.png" ) ) );
   if ( !GUI->FullPaths_CheckBox.IsChecked() )
      node->SetText( 2, File::ExtractNameAndSuffix( item.path ) );
   else
      node->SetText( 2, item.path );
   node->SetToolTip( 2, item.path );
   node->SetAlignment( 2, TextAlign::Left );
}

// ----------------------------------------------------------------------------

void DebayerInterface::UpdateTargetImagesList()
{
   int currentIdx = GUI->TargetImages_TreeBox.ChildIndex( GUI->TargetImages_TreeBox.CurrentNode() );

   GUI->TargetImages_TreeBox.DisableUpdates();
   GUI->TargetImages_TreeBox.Clear();

   for ( size_type i = 0; i < m_instance.p_targets.Length(); ++i )
   {
      new TreeBox::Node( GUI->TargetImages_TreeBox );
      UpdateTargetImageItem( i );
   }

   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !m_instance.p_targets.IsEmpty() )
      if ( currentIdx >= 0 )
         if ( currentIdx < GUI->TargetImages_TreeBox.NumberOfChildren() )
            GUI->TargetImages_TreeBox.SetCurrentNode( GUI->TargetImages_TreeBox[currentIdx] );

   GUI->TargetImages_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void DebayerInterface::UpdateImageSelectionButtons()
{
   bool hasItems = GUI->TargetImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->TargetImages_TreeBox.HasSelectedTopLevelNodes();

   GUI->SelectAll_PushButton.Enable( hasItems );
   GUI->InvertSelection_PushButton.Enable( hasItems );
   GUI->ToggleSelected_PushButton.Enable( hasSelection );
   GUI->RemoveSelected_PushButton.Enable( hasSelection );
   GUI->Clear_PushButton.Enable( hasItems );
   //GUI->FullPaths_CheckBox.Enable( hasItems );   // always enabled
}

// ----------------------------------------------------------------------------

void DebayerInterface::UpdateFormatHintsControls()
{
   GUI->InputHints_Edit.SetText( m_instance.p_inputHints );
   GUI->OutputHints_Edit.SetText( m_instance.p_outputHints );
}

// ----------------------------------------------------------------------------

void DebayerInterface::UpdateOutputFilesControls()
{
   GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory );
   GUI->OutputPrefix_Edit.SetText( m_instance.p_outputPrefix );
   GUI->OutputPostfix_Edit.SetText( m_instance.p_outputPostfix );
   GUI->OverwriteExistingFiles_CheckBox.SetChecked( m_instance.p_overwriteExistingFiles );
   GUI->OnError_ComboBox.SetCurrentItem( m_instance.p_onError );
}

// ----------------------------------------------------------------------------

void DebayerInterface::UpdateSignalAndNoiseEvaluationControls()
{
   GUI->SignalEvaluation_SectionBar.SetChecked( m_instance.p_evaluateSignal );

   GUI->StructureLayers_SpinBox.SetValue( m_instance.p_structureLayers );
   GUI->SaturationThreshold_NumericControl.SetValue( m_instance.p_saturationThreshold );
   GUI->SaturationRelative_CheckBox.SetChecked( m_instance.p_saturationRelative );
   GUI->NoiseLayers_SpinBox.SetValue( m_instance.p_noiseLayers );
   GUI->MinStructureSize_SpinBox.SetValue( m_instance.p_minStructureSize );
   GUI->HotPixelFilterRadius_SpinBox.SetValue( m_instance.p_hotPixelFilterRadius );
   GUI->NoiseReductionFilterRadius_SpinBox.SetValue( m_instance.p_noiseReductionFilterRadius );
   GUI->PSFType_ComboBox.SetCurrentItem( m_instance.p_psfType );
   GUI->PSFGrowth_NumericControl.SetValue( m_instance.p_psfGrowth );
   GUI->MaxStars_SpinBox.SetValue( m_instance.p_maxStars );

   GUI->NoiseEvaluation_SectionBar.SetChecked( m_instance.p_evaluateNoise );

   GUI->NoiseEvaluationAlgorithm_ComboBox.SetCurrentItem( m_instance.p_noiseEvaluationAlgorithm );
}

// ----------------------------------------------------------------------------

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_EditCompleted( Edit& sender )
{
   try
   {
      String text = sender.Text().Trimmed();

      if ( sender == GUI->InputHints_Edit )
         m_instance.p_inputHints = text;
      else if ( sender == GUI->OutputHints_Edit )
         m_instance.p_outputHints = text;
      else if ( sender == GUI->OutputDirectory_Edit )
         m_instance.p_outputDirectory = text;
      else if ( sender == GUI->OutputPrefix_Edit )
         m_instance.p_outputPrefix = text;
      else if ( sender == GUI->OutputPostfix_Edit )
         m_instance.p_outputPostfix = text;

      sender.SetText( text );
   }
   ERROR_CLEANUP(
      sender.SelectAll();
      sender.Focus()
   )
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Pattern_ComboBox )
      m_instance.p_bayerPattern = itemIndex;
   else if ( sender == GUI->OutputMode_ComboBox )
   {
      m_instance.p_outputRGBImages = itemIndex == 0 || itemIndex == 2;
      m_instance.p_outputSeparateChannels = itemIndex == 1 || itemIndex == 2;
   }
   else if ( sender == GUI->DebayerMethod_ComboBox )
      m_instance.p_debayerMethod = itemIndex;
   else if ( sender == GUI->OnError_ComboBox )
      m_instance.p_onError = itemIndex;
   else if ( sender == GUI->PSFType_ComboBox )
      m_instance.p_psfType = itemIndex;
   else if ( sender == GUI->NoiseEvaluationAlgorithm_ComboBox )
      m_instance.p_noiseEvaluationAlgorithm = itemIndex;
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "Debayer: Select Target Images" );
      d.LoadImageFilters();
      d.EnableMultipleSelections();
      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
         for ( const String& fileName : d.FileNames() )
            m_instance.p_targets.Insert( m_instance.p_targets.At( i0++ ), DebayerInstance::Item( fileName ) );

         UpdateTargetImagesList();
         UpdateImageSelectionButtons();
      }
   }
   else if ( sender == GUI->SelectAll_PushButton )
   {
      GUI->TargetImages_TreeBox.SelectAllNodes();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->InvertSelection_PushButton )
   {
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->TargetImages_TreeBox[i]->Select( !GUI->TargetImages_TreeBox[i]->IsSelected() );

      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->ToggleSelected_PushButton )
   {
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->TargetImages_TreeBox[i]->IsSelected() )
            m_instance.p_targets[i].enabled = !m_instance.p_targets[i].enabled;

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      DebayerInstance::item_list newTargets;
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->TargetImages_TreeBox[i]->IsSelected() )
            newTargets.Add( m_instance.p_targets[i] );
      m_instance.p_targets = newTargets;

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      m_instance.p_targets.Clear();
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->FullPaths_CheckBox )
   {
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->OutputDirectory_ToolButton )
   {
      GetDirectoryDialog d;
      d.SetCaption( "Debayer: Select Output Directory" );
      if ( d.Execute() )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = d.Directory() );
   }
   else if ( sender == GUI->OverwriteExistingFiles_CheckBox )
   {
      m_instance.p_overwriteExistingFiles = checked;
   }
   else if ( sender == GUI->SaturationRelative_CheckBox )
   {
      m_instance.p_saturationRelative = checked;
   }
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.p_targets.Length() )
      throw Error( "DebayerInterface: *Warning* Corrupted interface structures" );

   // ### If there's something that depends on which image is selected in the list, do it now.
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= m_instance.p_targets.Length() )
      throw Error( "DebayerInterface: *Warning* Corrupted interface structures" );

   DebayerInstance::Item& item = m_instance.p_targets[index];

   switch ( col )
   {
   case 0:
      break;
   case 1:
      item.enabled = !item.enabled;
      UpdateTargetImageItem( index );
      break;
   case 2:
      {
         Array<ImageWindow> windows = ImageWindow::Open( item.path, IsoString()/*id*/, m_instance.p_inputHints );
         for ( ImageWindow& window : windows )
            window.Show();
      }
      break;
   }
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->StructureLayers_SpinBox )
      m_instance.p_structureLayers = value;
   else if ( sender == GUI->NoiseLayers_SpinBox )
      m_instance.p_noiseLayers = value;
   else if ( sender == GUI->MinStructureSize_SpinBox )
      m_instance.p_minStructureSize = value;
   else if ( sender == GUI->HotPixelFilterRadius_SpinBox )
      m_instance.p_hotPixelFilterRadius = value;
   else if ( sender == GUI->NoiseReductionFilterRadius_SpinBox )
      m_instance.p_noiseReductionFilterRadius = value;
   else if ( sender == GUI->MaxStars_SpinBox )
      m_instance.p_maxStars = value;
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->SaturationThreshold_NumericControl )
      m_instance.p_saturationThreshold = value;
   else if ( sender == GUI->PSFGrowth_NumericControl )
      m_instance.p_psfGrowth = value;
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_CheckSection( SectionBar& sender, bool checked )
{
   if ( sender == GUI->SignalEvaluation_SectionBar )
      m_instance.p_evaluateSignal = checked;
   else if ( sender == GUI->NoiseEvaluation_SectionBar )
      m_instance.p_evaluateNoise = checked;
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->TargetImages_TreeBox.SetFixedHeight();
   else
   {
      GUI->TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->TargetImages_TreeBox.SetMaxHeight( int_max );

      if ( GUI->TargetImages_Control.IsVisible() )
         SetVariableHeight();
      else
         SetFixedHeight();
   }
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
      wantsFiles = true;
   else if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
   {
      StringList inputFiles;
      bool recursive = IsControlOrCmdPressed();
      for ( const String& item : files )
         if ( File::Exists( item ) )
            inputFiles << item;
         else if ( File::DirectoryExists( item ) )
            inputFiles << FileFormat::SupportedImageFiles( item, true/*toRead*/, false/*toWrite*/, recursive );
      size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );

      inputFiles.Sort();
      for ( const String& file : inputFiles )
         m_instance.p_targets.Insert( m_instance.p_targets.At( i0++ ), DebayerInstance::Item( file ) );

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->OutputDirectory_Edit )
   {
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = files[0] );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DebayerInterface::GUIData::GUIData( DebayerInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Bayer/mosaic pattern:" ) ) + 16; // the longest label text
   int editWidth1 = fnt.Width( String( 'M', 5 ) );
   int editWidth2 = fnt.Width( String( '0', 10  ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   const char* patternToolTip =
      "<p>Select the CFA pattern of the camera (DSLR or OSC) used to acquire the target image.</p>"
      "<p>The Auto option requires view properties available in the target view. These properties are normally "
      "generated by the RAW format support module and stored in XISF files.</p>"
      "<p>For images acquired with X-Trans sensors this parameter is ignored and CFA patterns are always "
      "extracted from existing image properties.</p>";

   Pattern_Label.SetText( "Bayer/mosaic pattern:" );
   Pattern_Label.SetFixedWidth( labelWidth1 );
   Pattern_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Pattern_Label.SetToolTip( patternToolTip );

   Pattern_ComboBox.AddItem( "Auto" );
   Pattern_ComboBox.AddItem( "RGGB" );
   Pattern_ComboBox.AddItem( "BGGR" );
   Pattern_ComboBox.AddItem( "GBRG" );
   Pattern_ComboBox.AddItem( "GRBG" );
   Pattern_ComboBox.AddItem( "GRGB" );
   Pattern_ComboBox.AddItem( "GBGR" );
   Pattern_ComboBox.AddItem( "RGBG" );
   Pattern_ComboBox.AddItem( "BGRG" );
   Pattern_ComboBox.SetToolTip( patternToolTip );
   Pattern_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::e_ItemSelected, w );

   Pattern_Sizer.SetSpacing( 4 );
   Pattern_Sizer.Add( Pattern_Label );
   Pattern_Sizer.Add( Pattern_ComboBox );
   Pattern_Sizer.AddStretch();

   const char* methodToolTip =
      "<p>Select the demosaicing algorithm. SuperPixel is fast and creates a good quality, half-sized image. "
      "The remaining methods create a full sized image, increasing in quality and computational overhead.</p>"
      "<p>For images acquired with X-Trans sensors this parameter is ignored and a special X-Trans interpolation "
      "algorithm is always applied.</p>";

   DebayerMethod_Label.SetText( "Demosaicing method:" );
   DebayerMethod_Label.SetFixedWidth( labelWidth1 );
   DebayerMethod_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DebayerMethod_Label.SetToolTip( methodToolTip );

   DebayerMethod_ComboBox.AddItem( "SuperPixel" );
   DebayerMethod_ComboBox.AddItem( "Bilinear" );
   DebayerMethod_ComboBox.AddItem( "VNG" );
   DebayerMethod_ComboBox.SetToolTip( methodToolTip );
   DebayerMethod_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::e_ItemSelected, w );
   //DebayerMethod_ComboBox.AddItem("AHD (not yet implemented)");

   DebayerMethod_Sizer.SetSpacing( 4 );
   DebayerMethod_Sizer.Add( DebayerMethod_Label );
   DebayerMethod_Sizer.Add( DebayerMethod_ComboBox );
   DebayerMethod_Sizer.AddStretch();

   const char* outputModeToolTip =
      "<p>Demosaiced/interpolated images can be generated as combined RGB color images, as separate RGB "
      "channels stored as monochrome images, or applying both options at the same time.</p>"
      "<p>Generation of single RGB color images is the default option. Separate RGB channel images can be "
      "useful for correction of non-isotropic channel misalignments, such as those caused by chromatic "
      "aberration and atmospheric dispersion, by computing image registration transformations with "
      "distortion correction among all color components of a data set. This procedure is compatible with "
      "normal image integrations as well as drizzle integrations.</p>"
      "<p>Separate channel file names will carry the _R, _G and _B suffixes, respectively for the red, "
      "green and blue components.</p>";

   OutputMode_Label.SetText( "Output mode:" );
   OutputMode_Label.SetFixedWidth( labelWidth1 );
   OutputMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputMode_Label.SetToolTip( outputModeToolTip );

   OutputMode_ComboBox.AddItem( "Combined RGB color" );
   OutputMode_ComboBox.AddItem( "Separate RGB channels" );
   OutputMode_ComboBox.AddItem( "RGB color + separate RGB channels" );
   OutputMode_ComboBox.SetToolTip( outputModeToolTip );
   OutputMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::e_ItemSelected, w );

   OutputMode_Sizer.SetSpacing( 4 );
   OutputMode_Sizer.Add( OutputMode_Label );
   OutputMode_Sizer.Add( OutputMode_ComboBox );
   OutputMode_Sizer.AddStretch();

   GeneralParameters_Sizer.SetSpacing( 4 );
   GeneralParameters_Sizer.Add( Pattern_Sizer );
   GeneralParameters_Sizer.Add( DebayerMethod_Sizer );
   GeneralParameters_Sizer.Add( OutputMode_Sizer );

   //

   TargetImages_SectionBar.SetTitle( "Target Images" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DebayerInterface::e_ToggleSection, w );

   TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   TargetImages_TreeBox.SetScaledMinWidth( 344 );
   TargetImages_TreeBox.SetNumberOfColumns( 3 );
   TargetImages_TreeBox.HideHeader();
   TargetImages_TreeBox.EnableMultipleSelections();
   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&DebayerInterface::e_CurrentNodeUpdated, w );
   TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&DebayerInterface::e_NodeActivated, w );
   TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&DebayerInterface::e_NodeSelectionUpdated, w );
   TargetImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&DebayerInterface::e_FileDrag, w );
   TargetImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&DebayerInterface::e_FileDrop, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of target images.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all target images.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of target images.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected target images.</p>"
      "<p>Disabled target images will be ignored during the registration process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected target images.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of input images.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for target file items.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   TargetButtons_Sizer.SetSpacing( 4 );
   TargetButtons_Sizer.Add( AddFiles_PushButton );
   TargetButtons_Sizer.Add( SelectAll_PushButton );
   TargetButtons_Sizer.Add( InvertSelection_PushButton );
   TargetButtons_Sizer.Add( ToggleSelected_PushButton );
   TargetButtons_Sizer.Add( RemoveSelected_PushButton );
   TargetButtons_Sizer.Add( Clear_PushButton );
   TargetButtons_Sizer.Add( FullPaths_CheckBox );
   TargetButtons_Sizer.AddStretch();

   TargetImages_Sizer.SetSpacing( 4 );
   TargetImages_Sizer.Add( TargetImages_TreeBox, 100 );
   TargetImages_Sizer.Add( TargetButtons_Sizer );

   TargetImages_Control.SetSizer( TargetImages_Sizer );

   //

   FormatHints_SectionBar.SetTitle( "Format Hints" );
   FormatHints_SectionBar.SetSection( FormatHints_Control );
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DebayerInterface::e_ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings for image files used "
      "by specific processes. In Debayer, input hints change the way input images of some particular file formats are loaded, "
      "while output hints modify the way output demosaiced image files are written.</p>"
      "<p>For example, you can use the \"lower-range\" and \"upper-range\" input hints to load floating point FITS and TIFF files "
      "generated by other applications that don't use PixInsight's normalized [0,1] range. Similarly, you can specify the \"up-bottom\" "
      "output hint to write all normalized images (in FITS format) with the coordinate origin at the top-left corner. Most standard "
      "file format modules support hints; each format supports a number of input and/or output hints that you can use for different "
      "purposes with tools that give you access to format hints.</p>";

   InputHints_Label.SetText( "Input hints:" );
   InputHints_Label.SetFixedWidth( labelWidth1 );
   InputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InputHints_Label.SetToolTip( hintsToolTip );

   InputHints_Edit.SetToolTip( hintsToolTip );
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&DebayerInterface::e_EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   OutputHints_Label.SetText( "Output hints:" );
   OutputHints_Label.SetFixedWidth( labelWidth1 );
   OutputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputHints_Label.SetToolTip( hintsToolTip );

   OutputHints_Edit.SetToolTip( hintsToolTip );
   OutputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&DebayerInterface::e_EditCompleted, w );

   OutputHints_Sizer.SetSpacing( 4 );
   OutputHints_Sizer.Add( OutputHints_Label );
   OutputHints_Sizer.Add( OutputHints_Edit, 100 );

   FormatHints_Sizer.SetSpacing( 4 );
   FormatHints_Sizer.Add( InputHints_Sizer );
   FormatHints_Sizer.Add( OutputHints_Sizer );

   FormatHints_Control.SetSizer( FormatHints_Sizer );

   //

   OutputFiles_SectionBar.SetTitle( "Output Files" );
   OutputFiles_SectionBar.SetSection( OutputFiles_Control );
   OutputFiles_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DebayerInterface::e_ToggleSection, w );

   const char* outputDirectoryToolTip = "<p>This is the directory (or folder) where all output demosaiced files will be written.</p>"
      "<p>If this field is left blank, output files will be written to the same directories as their corresponding "
      "target files. In this case, make sure that source directories are writable, or the demosaicing process will fail.</p>";

   OutputDirectory_Label.SetText( "Output directory:" );
   OutputDirectory_Label.SetFixedWidth( labelWidth1 );
   OutputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputDirectory_Label.SetToolTip( outputDirectoryToolTip );

   OutputDirectory_Edit.SetToolTip( outputDirectoryToolTip );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&DebayerInterface::e_EditCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)&DebayerInterface::e_FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)&DebayerInterface::e_FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.SetToolTip( "<p>Select the output directory</p>" );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   const char* outputPrefixToolTip = "<p>This is a prefix that will be prepended to the file name of each demosaiced image.</p>";

   OutputPrefix_Label.SetText( "Prefix:" );
   OutputPrefix_Label.SetFixedWidth( labelWidth1 );
   OutputPrefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPrefix_Label.SetToolTip( outputPrefixToolTip );

   OutputPrefix_Edit.SetFixedWidth( editWidth1 );
   OutputPrefix_Edit.SetToolTip( outputPrefixToolTip );
   OutputPrefix_Edit.OnEditCompleted( (Edit::edit_event_handler)&DebayerInterface::e_EditCompleted, w );

   const char* outputPostfixToolTip = "<p>This is a postfix that will be appended to the file name of each demosaiced image.</p>";

   OutputPostfix_Label.SetText( "Postfix:" );
   OutputPostfix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPostfix_Label.SetToolTip( outputPostfixToolTip );

   OutputPostfix_Edit.SetFixedWidth( editWidth1 );
   OutputPostfix_Edit.SetToolTip( outputPostfixToolTip );
   OutputPostfix_Edit.OnEditCompleted( (Edit::edit_event_handler)&DebayerInterface::e_EditCompleted, w );

   OutputChunks_Sizer.SetSpacing( 4 );
   OutputChunks_Sizer.Add( OutputPrefix_Label );
   OutputChunks_Sizer.Add( OutputPrefix_Edit );
   OutputChunks_Sizer.AddSpacing( 12 );
   OutputChunks_Sizer.Add( OutputPostfix_Label );
   OutputChunks_Sizer.Add( OutputPostfix_Edit );
   OutputChunks_Sizer.AddStretch();

   OverwriteExistingFiles_CheckBox.SetText( "Overwrite existing files" );
   OverwriteExistingFiles_CheckBox.SetToolTip( "<p>If this option is selected, Debayer will overwrite "
      "existing files with the same names as generated output files. This can be dangerous because the original "
      "contents of overwritten files will be lost.</p>"
      "<p><b>Enable this option <u>at your own risk.</u></b></p>" );
   OverwriteExistingFiles_CheckBox.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   const char* onErrorToolTip = "<p>Specify what to do if there are errors during a batch demosaicing process.</p>";

   OnError_Label.SetText( "On error:" );
   OnError_Label.SetToolTip( onErrorToolTip );
   OnError_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OnError_ComboBox.AddItem( "Continue" );
   OnError_ComboBox.AddItem( "Abort" );
   OnError_ComboBox.AddItem( "Ask User" );
   OnError_ComboBox.SetToolTip( onErrorToolTip );
   OnError_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::e_ItemSelected, w );

   OutputOptions_Sizer.SetSpacing( 4 );
   OutputOptions_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OutputOptions_Sizer.Add( OverwriteExistingFiles_CheckBox );
   OutputOptions_Sizer.AddSpacing( 16 );
   OutputOptions_Sizer.Add( OnError_Label );
   OutputOptions_Sizer.Add( OnError_ComboBox );
   OutputOptions_Sizer.AddStretch();

   OutputFiles_Sizer.SetSpacing( 4 );
   OutputFiles_Sizer.Add( OutputDirectory_Sizer );
   OutputFiles_Sizer.Add( OutputChunks_Sizer );
   OutputFiles_Sizer.Add( OutputOptions_Sizer );

   OutputFiles_Control.SetSizer( OutputFiles_Sizer );

   //

   SignalEvaluation_SectionBar.SetTitle( "Signal Evaluation" );
   SignalEvaluation_SectionBar.SetToolTip( "<p>Compute standardized estimates of the signal present in target frames.</p>"
      "<p>Our current implementation uses PSF photometry to generate robust and accurate measures of total and mean flux from detected "
      "and fitted sources. These estimates, along with estimates of the standard deviation of the noise, can be used for image "
      "grading and weighting with the SubframeSelector and ImageIntegration processes.</p>"
      "<p>The signal evaluation result will be stored as a set of custom, per-channel XISF properties and FITS header keywords that "
      "other processes and scripts can use for specialized image analysis purposes.</p>"
      "<p><b>This option should always be enabled for demosaicing of deep-sky raw light frames.</b></p>" );
   SignalEvaluation_SectionBar.SetSection( SignalEvaluation_Control );
   SignalEvaluation_SectionBar.EnableTitleCheckBox();
   SignalEvaluation_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DebayerInterface::e_ToggleSection, w );
   SignalEvaluation_SectionBar.OnCheck( (SectionBar::check_event_handler)&DebayerInterface::e_CheckSection, w );

   const char* structureLayersToolTip =
      "<p>Number of wavelet layers used for structure detection.</p>"
      "<p>With more wavelet layers, larger stars (and perhaps also some nonstellar objects) will be detected.</p>";

   StructureLayers_Label.SetText( "Detection scales:" );
   StructureLayers_Label.SetFixedWidth( labelWidth1 );
   StructureLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StructureLayers_Label.SetToolTip( structureLayersToolTip );

   StructureLayers_SpinBox.SetRange( int( TheDebayerStructureLayersParameter->MinimumValue() ), int( TheDebayerStructureLayersParameter->MaximumValue() ) );
   StructureLayers_SpinBox.SetToolTip( structureLayersToolTip );
   StructureLayers_SpinBox.SetFixedWidth( editWidth2 );
   StructureLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DebayerInterface::e_SpinValueUpdated, w );

   StructureLayers_Sizer.SetSpacing( 4 );
   StructureLayers_Sizer.Add( StructureLayers_Label );
   StructureLayers_Sizer.Add( StructureLayers_SpinBox );
   StructureLayers_Sizer.AddStretch();

   SaturationThreshold_NumericControl.label.SetText( "Saturation threshold:" );
   SaturationThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   SaturationThreshold_NumericControl.slider.SetRange( 0, 250 );
   SaturationThreshold_NumericControl.SetReal();
   SaturationThreshold_NumericControl.SetRange( TheDebayerSaturationThresholdParameter->MinimumValue(), TheDebayerSaturationThresholdParameter->MaximumValue() );
   SaturationThreshold_NumericControl.SetPrecision( TheDebayerSaturationThresholdParameter->Precision() );
   SaturationThreshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   SaturationThreshold_NumericControl.SetToolTip( "<p>Saturation threshold in the [0,1] range.</p>"
      "<p>Detected stars with one or more pixels with values above this threshold will be excluded for signal evaluation. "
      "This parameter is expressed in the [0,1] range. It can applied either as an absolute pixel sample value in the "
      "normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image (see the <i>Relative "
      "saturation threshold</i> parameter).</p>"
      "<p>The default saturation threshold is 1.0. For signal evaluation, the implemented star detection and outlier rejection "
      "routines are normally able to avoid contamination from saturated sources, so the default value of this parameter should "
      "not be changed under normal conditions.</p>" );
   SaturationThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&DebayerInterface::e_ValueUpdated, w );

   SaturationRelative_CheckBox.SetText( "Relative saturation threshold" );
   SaturationRelative_CheckBox.SetToolTip( "<p>The saturation threshold parameter can be applied either as an absolute pixel "
      "sample value in the normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image.</p>"
      "The relative saturation threshold option is enabled by default.</p>" );
   SaturationRelative_CheckBox.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   SaturationRelative_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SaturationRelative_Sizer.Add( SaturationRelative_CheckBox );
   SaturationRelative_Sizer.AddStretch();

   const char* noiseLayersToolTip =
      "<p>Number of wavelet layers used for noise reduction.</p>"
      "<p>Noise reduction prevents detection of bright noise structures as false stars, including hot pixels and "
      "cosmic rays. This parameter can also be used to control the sizes of the smallest detected stars (increase it "
      "to exclude more stars), although the <i>minimum structure size</i> parameter can be more efficient for this purpose.</p>";

   NoiseLayers_Label.SetText( "Noise scales:" );
   NoiseLayers_Label.SetFixedWidth( labelWidth1 );
   NoiseLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   NoiseLayers_Label.SetToolTip( noiseLayersToolTip );

   NoiseLayers_SpinBox.SetRange( int( TheDebayerNoiseLayersParameter->MinimumValue() ), int( TheDebayerNoiseLayersParameter->MaximumValue() ) );
   NoiseLayers_SpinBox.SetToolTip( noiseLayersToolTip );
   NoiseLayers_SpinBox.SetFixedWidth( editWidth2 );
   NoiseLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DebayerInterface::e_SpinValueUpdated, w );

   NoiseLayers_Sizer.SetSpacing( 4 );
   NoiseLayers_Sizer.Add( NoiseLayers_Label );
   NoiseLayers_Sizer.Add( NoiseLayers_SpinBox );
   NoiseLayers_Sizer.AddStretch();

   const char* minStructureSizeToolTip =
      "<p>Minimum size of a detectable star structure in square pixels.</p>"
      "<p>This parameter can be used to prevent detection of small and bright image artifacts as stars, when "
      "such artifacts cannot be removed with a median filter (i.e., the <i>Hot pixel removal</i> parameter).</p>"
      "<p>Changing the default zero value of this parameter should not be necessary with correctly acquired and "
      "calibrated data. It may help, however, when working with poor quality data such as poorly tracked, poorly "
      "focused, wrongly calibrated, low-SNR raw frames, for which our star detection algorithms have not been "
      "designed specifically.</p>";

   MinStructureSize_Label.SetText( "Minimum structure size:" );
   MinStructureSize_Label.SetFixedWidth( labelWidth1 );
   MinStructureSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MinStructureSize_Label.SetToolTip( minStructureSizeToolTip );

   MinStructureSize_SpinBox.SetRange( int( TheDebayerMinStructureSizeParameter->MinimumValue() ), int( TheDebayerMinStructureSizeParameter->MaximumValue() ) );
   MinStructureSize_SpinBox.SetToolTip( minStructureSizeToolTip );
   MinStructureSize_SpinBox.SetFixedWidth( editWidth2 );
   MinStructureSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DebayerInterface::e_SpinValueUpdated, w );

   MinStructureSize_Sizer.SetSpacing( 4 );
   MinStructureSize_Sizer.Add( MinStructureSize_Label );
   MinStructureSize_Sizer.Add( MinStructureSize_SpinBox );
   MinStructureSize_Sizer.AddStretch();

   const char* hotPixelFilterRadiusToolTip =
      "<p>Size of the hot pixel removal filter.</p>"
      "<p>This is the radius in pixels of a median filter applied by the star detector before the structure "
      "detection phase. A median filter is very efficient to remove <i>hot pixels</i>. If not removed, hot pixels "
      "will be identified as stars, and if present in large amounts, can prevent a valid signal evaluation.</p>"
      "<p>To disable hot pixel removal, set this parameter to zero.</p>";

   HotPixelFilterRadius_Label.SetText( "Hot pixel removal:" );
   HotPixelFilterRadius_Label.SetFixedWidth( labelWidth1 );
   HotPixelFilterRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   HotPixelFilterRadius_Label.SetToolTip( hotPixelFilterRadiusToolTip );

   HotPixelFilterRadius_SpinBox.SetRange( int( TheDebayerHotPixelFilterRadiusParameter->MinimumValue() ), int( TheDebayerHotPixelFilterRadiusParameter->MaximumValue() ) );
   HotPixelFilterRadius_SpinBox.SetToolTip( hotPixelFilterRadiusToolTip );
   HotPixelFilterRadius_SpinBox.SetFixedWidth( editWidth2 );
   HotPixelFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DebayerInterface::e_SpinValueUpdated, w );

   HotPixelFilterRadius_Sizer.SetSpacing( 4 );
   HotPixelFilterRadius_Sizer.Add( HotPixelFilterRadius_Label );
   HotPixelFilterRadius_Sizer.Add( HotPixelFilterRadius_SpinBox );
   HotPixelFilterRadius_Sizer.AddStretch();

   const char* noiseReductionFilterRadiusToolTip =
      "<p>Size of the noise reduction filter.</p>"
      "<p>This is the radius in pixels of a Gaussian convolution filter applied to the working image used for star "
      "detection. Use it only for very low SNR images, where the star detector cannot find reliable stars with its "
      "default parameters.</p>"
      "<p>To disable noise reduction, set this parameter to zero.</p>";

   NoiseReductionFilterRadius_Label.SetText( "Noise reduction:" );
   NoiseReductionFilterRadius_Label.SetFixedWidth( labelWidth1 );
   NoiseReductionFilterRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   NoiseReductionFilterRadius_Label.SetToolTip( noiseReductionFilterRadiusToolTip );

   NoiseReductionFilterRadius_SpinBox.SetRange( int( TheDebayerNoiseReductionFilterRadiusParameter->MinimumValue() ), int( TheDebayerNoiseReductionFilterRadiusParameter->MaximumValue() ) );
   NoiseReductionFilterRadius_SpinBox.SetToolTip( noiseReductionFilterRadiusToolTip );
   NoiseReductionFilterRadius_SpinBox.SetFixedWidth( editWidth2 );
   NoiseReductionFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DebayerInterface::e_SpinValueUpdated, w );

   NoiseReductionFilterRadius_Sizer.SetSpacing( 4 );
   NoiseReductionFilterRadius_Sizer.Add( NoiseReductionFilterRadius_Label );
   NoiseReductionFilterRadius_Sizer.Add( NoiseReductionFilterRadius_SpinBox );
   NoiseReductionFilterRadius_Sizer.AddStretch();

   const char* psfTypeToolTip =
      "<p>Point spread function type used for PSF fitting and photometry.</p>"
      "<p>In all cases elliptical functions are fitted to detected star structures, and PSF sampling regions are "
      "defined adaptively using a median stabilization algorithm.</p>"
      "<p>When the <b>Auto</b> option is selected, a series of different PSFs will be fitted for each source, and "
      "the fit that leads to the least absolute difference among function values and sampled pixel values will be "
      "used for flux measurement. Currently the following functions are tested in this special automatic mode: "
      "Moffat functions with <i>beta</i> shape parameters equal to 2.5, 4, 6 and 10.</p>"
      "<p>The rest of options select a fixed PSF type for all detected sources, which improves execution times at "
      "the cost of a less adaptive, and hence potentially less accurate, signal estimation process.</p>";

   PSFType_Label.SetText( "PSF type:" );
   PSFType_Label.SetFixedWidth( labelWidth1 );
   PSFType_Label.SetToolTip( psfTypeToolTip );
   PSFType_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   PSFType_ComboBox.AddItem( "Gaussian" );
   PSFType_ComboBox.AddItem( "Moffat beta = 1.5" );
   PSFType_ComboBox.AddItem( "Moffat beta = 4" );
   PSFType_ComboBox.AddItem( "Moffat beta = 6" );
   PSFType_ComboBox.AddItem( "Moffat beta = 8" );
   PSFType_ComboBox.AddItem( "Moffat beta = 10" );
   PSFType_ComboBox.AddItem( "Auto" );
   PSFType_ComboBox.SetToolTip( psfTypeToolTip );
   PSFType_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::e_ItemSelected, w );

   PSFType_Sizer.SetSpacing( 4 );
   PSFType_Sizer.Add( PSFType_Label );
   PSFType_Sizer.Add( PSFType_ComboBox );
   PSFType_Sizer.AddStretch();

   PSFGrowth_NumericControl.label.SetText( "Growth factor:" );
   PSFGrowth_NumericControl.label.SetFixedWidth( labelWidth1 );
   PSFGrowth_NumericControl.slider.SetRange( 0, 250 );
   PSFGrowth_NumericControl.SetReal();
   PSFGrowth_NumericControl.SetRange( TheDebayerPSFGrowthParameter->MinimumValue(), TheDebayerPSFGrowthParameter->MaximumValue() );
   PSFGrowth_NumericControl.SetPrecision( TheDebayerPSFGrowthParameter->Precision() );
   PSFGrowth_NumericControl.edit.SetFixedWidth( editWidth1 );
   PSFGrowth_NumericControl.SetToolTip( "<p>Growing factor for expansion/contraction of the PSF flux measurement region for "
      "each source, in units of the Full Width at Tenth Maximum (FWTM).</p>"
      "<p>The default value of this parameter is 1.0, meaning that flux is measured exclusively for pixels within the elliptical "
      "region defined at one tenth of the fitted PSF maximum. Increasing this parameter can inprove accuracy of PSF flux "
      "measurements for undersampled images, where PSF fitting uncertainty is relatively large. Decreasing it can be beneficial "
      "in some cases working with very noisy data to restrict flux evaluation to star cores.</p>" );
   PSFGrowth_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&DebayerInterface::e_ValueUpdated, w );

   const char* maxStarsToolTip =
      "<p>The maximum number of stars that can be measured to compute mean signal estimates.</p>"
      "<p>PSF photometry will be performed for no more than the specified number of stars. The subset of measured stars "
      "will always start at the beginning of the set of detected stars, sorted by brightness in descending order.</p>"
      "<p>The default value imposes a generous limit of 24K stars. Limiting the number of photometric samples can improve "
      "performance for wide-field frames, where the number of detected stars can be very large. However, reducing the set "
      "of measured sources too much will damage the accuracy of signal estimation.</p>";

   MaxStars_Label.SetText( "Maximum stars:" );
   MaxStars_Label.SetFixedWidth( labelWidth1 );
   MaxStars_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MaxStars_Label.SetToolTip( maxStarsToolTip );

   MaxStars_SpinBox.SetRange( int( TheDebayerMaxStarsParameter->MinimumValue() ), int( TheDebayerMaxStarsParameter->MaximumValue() ) );
   MaxStars_SpinBox.SetToolTip( maxStarsToolTip );
   MaxStars_SpinBox.SetFixedWidth( editWidth2 );
   MaxStars_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DebayerInterface::e_SpinValueUpdated, w );

   MaxStars_Sizer.SetSpacing( 4 );
   MaxStars_Sizer.Add( MaxStars_Label );
   MaxStars_Sizer.Add( MaxStars_SpinBox );
   MaxStars_Sizer.AddStretch();

   SignalEvaluation_Sizer.SetSpacing( 4 );
   SignalEvaluation_Sizer.Add( StructureLayers_Sizer );
   SignalEvaluation_Sizer.Add( SaturationThreshold_NumericControl );
   SignalEvaluation_Sizer.Add( SaturationRelative_Sizer );
   SignalEvaluation_Sizer.Add( NoiseLayers_Sizer );
   SignalEvaluation_Sizer.Add( MinStructureSize_Sizer );
   SignalEvaluation_Sizer.Add( HotPixelFilterRadius_Sizer );
   SignalEvaluation_Sizer.Add( NoiseReductionFilterRadius_Sizer );
   SignalEvaluation_Sizer.Add( PSFType_Sizer );
   SignalEvaluation_Sizer.Add( PSFGrowth_NumericControl );
   SignalEvaluation_Sizer.Add( MaxStars_Sizer );

   SignalEvaluation_Control.SetSizer( SignalEvaluation_Sizer );

   //

   NoiseEvaluation_SectionBar.SetTitle( "Noise Evaluation" );
   NoiseEvaluation_SectionBar.SetToolTip( "<p>Compute per-channel estimates of the standard deviation of the noise "
      "and noise scaling factors for each target image, using a selected multiscale algorithm (MRS noise estimator "
      "by default). Noise estimates will be stored as a set of custom XISF properties and FITS header keywords in the "
      "output files.</p>"
      "<p>These estimates can be used later by several processes and scripts for specialized image analysis purposes, "
      "most notably by SubframeSelector and ImageIntegration for image grading and weighting. Noise estimates will "
      "always be computed from uninterpolated, raw calibrated data.</p>"
      "<p><b>This option should always be enabled under normal working conditions.</b></p>" );
   NoiseEvaluation_SectionBar.SetSection( NoiseEvaluation_Control );
   NoiseEvaluation_SectionBar.EnableTitleCheckBox();
   NoiseEvaluation_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DebayerInterface::e_ToggleSection, w );
   NoiseEvaluation_SectionBar.OnCheck( (SectionBar::check_event_handler)&DebayerInterface::e_CheckSection, w );

   const char* noiseEvaluationToolTip = "<p>Noise evaluation algorithm. This option selects an algorithm for automatic "
      "estimation of the standard deviation of the noise in the calibrated images.</p>"
      "<p>The multiresolution support (MRS) noise estimation routine implements the iterative algorithm described by "
      "Jean-Luc Starck and Fionn Murtagh in their paper <em>Automatic Noise Estimation from the Multiresolution Support</em> "
      "(Publications of the Royal Astronomical Society of the Pacific, vol. 110, pp. 193-199). In our implementation, the "
      "standard deviation of the noise is evaluated on the first four wavelet layers, assuming a Gaussian noise distribution. "
      "MRS is a remarkably accurate and robust algorithm and the default option for noise evaluation.</p>"
      "<p>The iterative k-sigma clipping algorithm can be used as a last-resort option in cases where the MRS algorithm "
      "does not converge systematically. This can happen on images with no detectable small-scale noise; for example, images "
      "that have been smoothed as a result of bilinear demosaicing interpolation.</p>"
      "<p>The N* robust noise estimator extracts a subset of residual pixels by comparison with a large-scale model of "
      "the local background of the image, generated with the multiscale median transform (MMT). Since the MMT is remarkably "
      "efficient at isolating image structures, it can be used to detect pixels that cannot contribute to significant structures. "
      "N* is an accurate and robust, alternative estimator of the standard deviation of the noise that does not assume or require "
      "any particular statistical distribution in the analyzed data.</p>";

   NoiseEvaluationAlgorithm_Label.SetText( "Evaluation algorithm:" );
   NoiseEvaluationAlgorithm_Label.SetFixedWidth( labelWidth1 );
   NoiseEvaluationAlgorithm_Label.SetToolTip( noiseEvaluationToolTip );
   NoiseEvaluationAlgorithm_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   NoiseEvaluationAlgorithm_ComboBox.AddItem( "Iterative k-sigma clipping" );
   NoiseEvaluationAlgorithm_ComboBox.AddItem( "Multiresolution support" );
   NoiseEvaluationAlgorithm_ComboBox.AddItem( "N* robust noise estimator" );
   NoiseEvaluationAlgorithm_ComboBox.SetToolTip( noiseEvaluationToolTip );
   NoiseEvaluationAlgorithm_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::e_ItemSelected, w );

   NoiseEvaluationAlgorithm_Sizer.SetSpacing( 4 );
   NoiseEvaluationAlgorithm_Sizer.Add( NoiseEvaluationAlgorithm_Label );
   NoiseEvaluationAlgorithm_Sizer.Add( NoiseEvaluationAlgorithm_ComboBox );
   NoiseEvaluationAlgorithm_Sizer.AddStretch();

   NoiseEvaluation_Sizer.SetSpacing( 4 );
   NoiseEvaluation_Sizer.Add( NoiseEvaluationAlgorithm_Sizer );

   NoiseEvaluation_Control.SetSizer( NoiseEvaluation_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );

   Global_Sizer.Add( GeneralParameters_Sizer );
   Global_Sizer.Add( TargetImages_SectionBar );
   Global_Sizer.Add( TargetImages_Control );
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add( OutputFiles_SectionBar );
   Global_Sizer.Add( OutputFiles_Control );
   Global_Sizer.Add( SignalEvaluation_SectionBar );
   Global_Sizer.Add( SignalEvaluation_Control );
   Global_Sizer.Add( NoiseEvaluation_SectionBar );
   Global_Sizer.Add( NoiseEvaluation_Control );

   w.SetSizer( Global_Sizer );

   FormatHints_Control.Hide();
   SignalEvaluation_Control.Hide();
   NoiseEvaluation_Control.Hide();

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedWidth();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DebayerInterface.cpp - Released 2022-11-21T14:47:18Z
