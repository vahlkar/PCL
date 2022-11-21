//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.35
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.9.7
// ----------------------------------------------------------------------------
// ImageCalibrationInterface.cpp - Released 2022-11-21T14:47:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "ImageCalibrationInterface.h"
#include "ImageCalibrationProcess.h"

#include <pcl/Dialog.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/ViewList.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ErrorHandler.h>

#define IMAGELIST_MINHEIGHT( fnt )  (8*fnt.Height() + 2)

namespace pcl
{

// ----------------------------------------------------------------------------

ImageCalibrationInterface* TheImageCalibrationInterface = nullptr;

// ----------------------------------------------------------------------------

ImageCalibrationInterface::ImageCalibrationInterface()
   : m_instance( TheImageCalibrationProcess )
{
   TheImageCalibrationInterface = this;

   /*
    * The auto save geometry feature is of no good to interfaces that include
    * both auto-expanding controls (e.g. TreeBox) and collapsible sections
    * (e.g. SectionBar).
    */
   DisableAutoSaveGeometry();
}

// ----------------------------------------------------------------------------

ImageCalibrationInterface::~ImageCalibrationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString ImageCalibrationInterface::Id() const
{
   return "ImageCalibration";
}

// ----------------------------------------------------------------------------

MetaProcess* ImageCalibrationInterface::Process() const
{
   return TheImageCalibrationProcess;
}

// ----------------------------------------------------------------------------

String ImageCalibrationInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/ImageCalibration.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures ImageCalibrationInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::ResetInstance()
{
   ImageCalibrationInstance defaultInstance( TheImageCalibrationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ImageCalibration" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheImageCalibrationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ImageCalibrationInterface::NewProcess() const
{
   return new ImageCalibrationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const ImageCalibrationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an ImageCalibration instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::SaveSettings() const
{
   SaveGeometry();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateControls()
{
   UpdateTargetImagesList();
   UpdateImageSelectionButtons();
   UpdateCFAControls();
   UpdateFormatHintsControls();
   UpdateOutputFilesControls();
   UpdateSignalAndNoiseEvaluationControls();
   UpdatePedestalControls();
   UpdateMasterFrameControls();
   UpdateOverscanControls();
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateTargetImageItem( size_type i )
{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const ImageCalibrationInstance::ImageItem& item = m_instance.p_targetFrames[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, Bitmap( ScaledResource( ":/browser/document.png" ) ) );
   if ( GUI->FullPaths_CheckBox.IsChecked() )
      node->SetText( 2, item.path );
   else
      node->SetText( 2, File::ExtractNameAndSuffix( item.path ) );
   node->SetToolTip( 2, item.path );
   node->SetAlignment( 2, TextAlign::Left );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateTargetImagesList()
{
   int currentIdx = GUI->TargetImages_TreeBox.ChildIndex( GUI->TargetImages_TreeBox.CurrentNode() );

   GUI->TargetImages_TreeBox.DisableUpdates();
   GUI->TargetImages_TreeBox.Clear();

   for ( size_type i = 0; i < m_instance.p_targetFrames.Length(); ++i )
   {
      new TreeBox::Node( GUI->TargetImages_TreeBox );
      UpdateTargetImageItem( i );
   }

   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !m_instance.p_targetFrames.IsEmpty() )
      if ( currentIdx >= 0 && currentIdx < GUI->TargetImages_TreeBox.NumberOfChildren() )
         GUI->TargetImages_TreeBox.SetCurrentNode( GUI->TargetImages_TreeBox[currentIdx] );

   GUI->TargetImages_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateImageSelectionButtons()
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

void ImageCalibrationInterface::UpdateCFAControls()
{
   GUI->DetectCFA_CheckBox.SetChecked( m_instance.p_enableCFA );

   GUI->CFAPattern_Label.Enable( m_instance.p_enableCFA );

   GUI->CFAPattern_ComboBox.SetCurrentItem( m_instance.p_cfaPattern );
   GUI->CFAPattern_ComboBox.Enable( m_instance.p_enableCFA );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateFormatHintsControls()
{
   GUI->InputHints_Edit.SetText( m_instance.p_inputHints );
   GUI->OutputHints_Edit.SetText( m_instance.p_outputHints );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateOutputFilesControls()
{
   GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory );

   GUI->OutputPrefix_Edit.SetText( m_instance.p_outputPrefix );

   GUI->OutputPostfix_Edit.SetText( m_instance.p_outputPostfix );

   GUI->OutputSampleFormat_ComboBox.SetCurrentItem( m_instance.p_outputSampleFormat );

   GUI->OutputPedestalMode_ComboBox.SetCurrentItem( m_instance.p_outputPedestalMode );

   GUI->AutoPedestalLimit_NumericEdit.SetValue( m_instance.p_autoPedestalLimit );
   GUI->AutoPedestalLimit_NumericEdit.Enable( m_instance.p_outputPedestalMode == ICOutputPedestalMode::Auto );

   GUI->OutputPedestal_Label.Enable( m_instance.p_outputPedestalMode == ICOutputPedestalMode::Literal );

   GUI->OutputPedestal_SpinBox.SetValue( m_instance.p_outputPedestal );
   GUI->OutputPedestal_SpinBox.Enable( m_instance.p_outputPedestalMode == ICOutputPedestalMode::Literal );

   GUI->OverwriteExistingFiles_CheckBox.SetChecked( m_instance.p_overwriteExistingFiles );

   GUI->OnError_ComboBox.SetCurrentItem( m_instance.p_onError );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateSignalAndNoiseEvaluationControls()
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

void ImageCalibrationInterface::UpdatePedestalControls()
{
   GUI->PedestalMode_ComboBox.SetCurrentItem( m_instance.p_pedestalMode );

   GUI->PedestalValue_Label.Enable( m_instance.p_pedestalMode == ICPedestalMode::Literal );

   GUI->PedestalValue_SpinBox.SetValue( m_instance.p_pedestal );
   GUI->PedestalValue_SpinBox.Enable( m_instance.p_pedestalMode == ICPedestalMode::Literal );

   GUI->PedestalKeyword_Label.Enable( m_instance.p_pedestalMode == ICPedestalMode::CustomKeyword );

   GUI->PedestalKeyword_Edit.SetText( m_instance.p_pedestalKeyword );
   GUI->PedestalKeyword_Edit.Enable( m_instance.p_pedestalMode == ICPedestalMode::CustomKeyword );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateMasterFrameControls()
{
   GUI->MasterBias_SectionBar.SetChecked( m_instance.p_masterBias.enabled );

   GUI->MasterBiasPath_Edit.SetText( m_instance.p_masterBias.path );

   GUI->CalibrateMasterBias_CheckBox.SetChecked( m_instance.p_calibrateBias );

   GUI->MasterDark_SectionBar.SetChecked( m_instance.p_masterDark.enabled );

   GUI->MasterDarkPath_Edit.SetText( m_instance.p_masterDark.path );

   GUI->CalibrateMasterDark_CheckBox.SetChecked( m_instance.p_calibrateDark );

   GUI->OptimizeDarks_CheckBox.SetChecked( m_instance.p_optimizeDarks );

   GUI->DarkOptimizationThreshold_NumericControl.SetValue( m_instance.p_darkOptimizationLow );
   GUI->DarkOptimizationThreshold_NumericControl.Enable( m_instance.p_optimizeDarks );

//    GUI->DarkOptimizationWindow_Label.Enable( m_instance.p_optimizeDarks );

//    GUI->DarkOptimizationWindow_SpinBox.SetValue( m_instance.p_darkOptimizationWindow );
//    GUI->DarkOptimizationWindow_SpinBox.Enable( m_instance.p_optimizeDarks );

   GUI->MasterFlat_SectionBar.SetChecked( m_instance.p_masterFlat.enabled );

   GUI->MasterFlatPath_Edit.SetText( m_instance.p_masterFlat.path );

   GUI->CalibrateMasterFlat_CheckBox.SetChecked( m_instance.p_calibrateFlat );

   GUI->SeparateCFAFlatScalingFactors_CheckBox.SetChecked( m_instance.p_separateCFAFlatScalingFactors );
   GUI->SeparateCFAFlatScalingFactors_CheckBox.Enable( m_instance.p_enableCFA );

   GUI->FlatScaleClippingFactor_NumericControl.SetValue( m_instance.p_flatScaleClippingFactor );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateOverscanControls()
{
   GUI->Overscan_SectionBar.SetChecked( m_instance.p_overscan.enabled );

   GUI->ImageRegionX0_NumericEdit.SetValue( m_instance.p_overscan.imageRect.x0 );
   GUI->ImageRegionY0_NumericEdit.SetValue( m_instance.p_overscan.imageRect.y0 );
   GUI->ImageRegionWidth_NumericEdit.SetValue( m_instance.p_overscan.imageRect.Width() );
   GUI->ImageRegionHeight_NumericEdit.SetValue( m_instance.p_overscan.imageRect.Height() );

   const ImageCalibrationInstance::OverscanRegions& ov1 = m_instance.p_overscan.overscan[0];
   const ImageCalibrationInstance::OverscanRegions& ov2 = m_instance.p_overscan.overscan[1];
   const ImageCalibrationInstance::OverscanRegions& ov3 = m_instance.p_overscan.overscan[2];
   const ImageCalibrationInstance::OverscanRegions& ov4 = m_instance.p_overscan.overscan[3];

   bool en1 = ov1.enabled;
   bool en2 = ov2.enabled;
   bool en3 = ov3.enabled;
   bool en4 = ov4.enabled;

   GUI->Overscan1_CheckBox.SetChecked( en1 );

   GUI->Overscan1Source_Label.Enable( en1 );
   GUI->Overscan1SourceX0_NumericEdit.SetValue( ov1.sourceRect.x0 );
   GUI->Overscan1SourceX0_NumericEdit.Enable( en1 );
   GUI->Overscan1SourceY0_NumericEdit.SetValue( ov1.sourceRect.y0 );
   GUI->Overscan1SourceY0_NumericEdit.Enable( en1 );
   GUI->Overscan1SourceWidth_NumericEdit.SetValue( ov1.sourceRect.Width() );
   GUI->Overscan1SourceWidth_NumericEdit.Enable( en1 );
   GUI->Overscan1SourceHeight_NumericEdit.SetValue( ov1.sourceRect.Height() );
   GUI->Overscan1SourceHeight_NumericEdit.Enable( en1 );

   GUI->Overscan1Target_Label.Enable( en1 );
   GUI->Overscan1TargetX0_NumericEdit.SetValue( ov1.targetRect.x0 );
   GUI->Overscan1TargetX0_NumericEdit.Enable( en1 );
   GUI->Overscan1TargetY0_NumericEdit.SetValue( ov1.targetRect.y0 );
   GUI->Overscan1TargetY0_NumericEdit.Enable( en1 );
   GUI->Overscan1TargetWidth_NumericEdit.SetValue( ov1.targetRect.Width() );
   GUI->Overscan1TargetWidth_NumericEdit.Enable( en1 );
   GUI->Overscan1TargetHeight_NumericEdit.SetValue( ov1.targetRect.Height() );
   GUI->Overscan1TargetHeight_NumericEdit.Enable( en1 );

   GUI->Overscan2_CheckBox.SetChecked( en2 );

   GUI->Overscan2Source_Label.Enable( en2 );
   GUI->Overscan2SourceX0_NumericEdit.SetValue( ov2.sourceRect.x0 );
   GUI->Overscan2SourceX0_NumericEdit.Enable( en2 );
   GUI->Overscan2SourceY0_NumericEdit.SetValue( ov2.sourceRect.y0 );
   GUI->Overscan2SourceY0_NumericEdit.Enable( en2 );
   GUI->Overscan2SourceWidth_NumericEdit.SetValue( ov2.sourceRect.Width() );
   GUI->Overscan2SourceWidth_NumericEdit.Enable( en2 );
   GUI->Overscan2SourceHeight_NumericEdit.SetValue( ov2.sourceRect.Height() );
   GUI->Overscan2SourceHeight_NumericEdit.Enable( en2 );

   GUI->Overscan2Target_Label.Enable( en2 );
   GUI->Overscan2TargetX0_NumericEdit.SetValue( ov2.targetRect.x0 );
   GUI->Overscan2TargetX0_NumericEdit.Enable( en2 );
   GUI->Overscan2TargetY0_NumericEdit.SetValue( ov2.targetRect.y0 );
   GUI->Overscan2TargetY0_NumericEdit.Enable( en2 );
   GUI->Overscan2TargetWidth_NumericEdit.SetValue( ov2.targetRect.Width() );
   GUI->Overscan2TargetWidth_NumericEdit.Enable( en2 );
   GUI->Overscan2TargetHeight_NumericEdit.SetValue( ov2.targetRect.Height() );
   GUI->Overscan2TargetHeight_NumericEdit.Enable( en2 );

   GUI->Overscan3_CheckBox.SetChecked( en3 );

   GUI->Overscan3Source_Label.Enable( en3 );
   GUI->Overscan3SourceX0_NumericEdit.SetValue( ov3.sourceRect.x0 );
   GUI->Overscan3SourceX0_NumericEdit.Enable( en3 );
   GUI->Overscan3SourceY0_NumericEdit.SetValue( ov3.sourceRect.y0 );
   GUI->Overscan3SourceY0_NumericEdit.Enable( en3 );
   GUI->Overscan3SourceWidth_NumericEdit.SetValue( ov3.sourceRect.Width() );
   GUI->Overscan3SourceWidth_NumericEdit.Enable( en3 );
   GUI->Overscan3SourceHeight_NumericEdit.SetValue( ov3.sourceRect.Height() );
   GUI->Overscan3SourceHeight_NumericEdit.Enable( en3 );

   GUI->Overscan3Target_Label.Enable( en3 );
   GUI->Overscan3TargetX0_NumericEdit.SetValue( ov3.targetRect.x0 );
   GUI->Overscan3TargetX0_NumericEdit.Enable( en3 );
   GUI->Overscan3TargetY0_NumericEdit.SetValue( ov3.targetRect.y0 );
   GUI->Overscan3TargetY0_NumericEdit.Enable( en3 );
   GUI->Overscan3TargetWidth_NumericEdit.SetValue( ov3.targetRect.Width() );
   GUI->Overscan3TargetWidth_NumericEdit.Enable( en3 );
   GUI->Overscan3TargetHeight_NumericEdit.SetValue( ov3.targetRect.Height() );
   GUI->Overscan3TargetHeight_NumericEdit.Enable( en3 );

   GUI->Overscan4_CheckBox.SetChecked( en4 );

   GUI->Overscan4Source_Label.Enable( en4 );
   GUI->Overscan4SourceX0_NumericEdit.SetValue( ov4.sourceRect.x0 );
   GUI->Overscan4SourceX0_NumericEdit.Enable( en4 );
   GUI->Overscan4SourceY0_NumericEdit.SetValue( ov4.sourceRect.y0 );
   GUI->Overscan4SourceY0_NumericEdit.Enable( en4 );
   GUI->Overscan4SourceWidth_NumericEdit.SetValue( ov4.sourceRect.Width() );
   GUI->Overscan4SourceWidth_NumericEdit.Enable( en4 );
   GUI->Overscan4SourceHeight_NumericEdit.SetValue( ov4.sourceRect.Height() );
   GUI->Overscan4SourceHeight_NumericEdit.Enable( en4 );

   GUI->Overscan4Target_Label.Enable( en4 );
   GUI->Overscan4TargetX0_NumericEdit.SetValue( ov4.targetRect.x0 );
   GUI->Overscan4TargetX0_NumericEdit.Enable( en4 );
   GUI->Overscan4TargetY0_NumericEdit.SetValue( ov4.targetRect.y0 );
   GUI->Overscan4TargetY0_NumericEdit.Enable( en4 );
   GUI->Overscan4TargetWidth_NumericEdit.SetValue( ov4.targetRect.Width() );
   GUI->Overscan4TargetWidth_NumericEdit.Enable( en4 );
   GUI->Overscan4TargetHeight_NumericEdit.SetValue( ov4.targetRect.Height() );
   GUI->Overscan4TargetHeight_NumericEdit.Enable( en4 );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ImageCalibrationInterface::e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.p_targetFrames.Length() )
      throw Error( "ImageCalibrationInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= m_instance.p_targetFrames.Length() )
      throw Error( "ImageCalibrationInterface: *Warning* Corrupted interface structures" );

   ImageCalibrationInstance::ImageItem& item = m_instance.p_targetFrames[index];

   switch ( col )
   {
   case 0:
      // Activate the item's index number: ignore.
      break;
   case 1:
      // Activate the item's checkmark: toggle item's enabled state.
      item.enabled = !item.enabled;
      UpdateTargetImageItem( index );
      break;
   case 2:
      {
         // Activate the item's path: open the image.
         Array<ImageWindow> windows = ImageWindow::Open( item.path, IsoString()/*id*/, m_instance.p_inputHints );
         for ( ImageWindow& window : windows )
            window.Show();
      }
      break;
   }
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::e_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

// ----------------------------------------------------------------------------

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.EnableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "ImageCalibration: Select Target Frames" );
      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
            m_instance.p_targetFrames.Insert( m_instance.p_targetFrames.At( i0++ ), ImageCalibrationInstance::ImageItem( *i ) );
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
            m_instance.p_targetFrames[i].enabled = !m_instance.p_targetFrames[i].enabled;
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      ImageCalibrationInstance::image_list newTargets;
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->TargetImages_TreeBox[i]->IsSelected() )
            newTargets.Add( m_instance.p_targetFrames[i] );
      m_instance.p_targetFrames = newTargets;
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      m_instance.p_targetFrames.Clear();
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->FullPaths_CheckBox )
   {
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->DetectCFA_CheckBox )
   {
      m_instance.p_enableCFA = checked;
      UpdateCFAControls();
      UpdateMasterFrameControls();
   }
   else if ( sender == GUI->OutputDirectory_ToolButton )
   {
      GetDirectoryDialog d;
      d.SetCaption( "ImageCalibration: Select Output Directory" );
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
   else if ( sender == GUI->MasterBiasPath_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "ImageCalibration: Select Master Bias Frame" );
      if ( d.Execute() )
      {
         m_instance.p_masterBias.path = d.FileName();
         UpdateMasterFrameControls();
      }
   }
   else if ( sender == GUI->CalibrateMasterBias_CheckBox )
   {
      m_instance.p_calibrateBias = checked;
      UpdateMasterFrameControls();
   }
   else if ( sender == GUI->MasterDarkPath_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "ImageCalibration: Select Master Dark Frame" );
      if ( d.Execute() )
      {
         m_instance.p_masterDark.path = d.FileName();
         UpdateMasterFrameControls();
      }
   }
   else if ( sender == GUI->CalibrateMasterDark_CheckBox )
   {
      m_instance.p_calibrateDark = checked;
      UpdateMasterFrameControls();
   }
   else if ( sender == GUI->OptimizeDarks_CheckBox )
   {
      m_instance.p_optimizeDarks = checked;
      UpdateMasterFrameControls();
   }
   else if ( sender == GUI->MasterFlatPath_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "ImageCalibration: Select Master Flat Frame" );
      if ( d.Execute() )
      {
         m_instance.p_masterFlat.path = d.FileName();
         UpdateMasterFrameControls();
      }
   }
   else if ( sender == GUI->CalibrateMasterFlat_CheckBox )
   {
      m_instance.p_calibrateFlat = checked;
      UpdateMasterFrameControls();
   }
   else if ( sender == GUI->SeparateCFAFlatScalingFactors_CheckBox )
   {
      m_instance.p_separateCFAFlatScalingFactors = checked;
   }
   else if ( sender == GUI->Overscan1_CheckBox )
   {
      m_instance.p_overscan.overscan[0].enabled = checked;
      UpdateOverscanControls();
   }
   else if ( sender == GUI->Overscan2_CheckBox )
   {
      m_instance.p_overscan.overscan[1].enabled = checked;
      UpdateOverscanControls();
   }
   else if ( sender == GUI->Overscan3_CheckBox )
   {
      m_instance.p_overscan.overscan[2].enabled = checked;
      UpdateOverscanControls();
   }
   else if ( sender == GUI->Overscan4_CheckBox )
   {
      m_instance.p_overscan.overscan[3].enabled = checked;
      UpdateOverscanControls();
   }
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::e_EditCompleted( Edit& sender )
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
   else if ( sender == GUI->PedestalKeyword_Edit )
      m_instance.p_pedestalKeyword = text;
   else if ( sender == GUI->MasterBiasPath_Edit )
      m_instance.p_masterBias.path = text;
   else if ( sender == GUI->MasterDarkPath_Edit )
      m_instance.p_masterDark.path = text;
   else if ( sender == GUI->MasterFlatPath_Edit )
      m_instance.p_masterFlat.path = text;

   sender.SetText( text );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->CFAPattern_ComboBox )
      m_instance.p_cfaPattern = itemIndex;
   else if ( sender == GUI->OutputSampleFormat_ComboBox )
      m_instance.p_outputSampleFormat = itemIndex;
   else if ( sender == GUI->OutputPedestalMode_ComboBox )
   {
      m_instance.p_outputPedestalMode = itemIndex;
      UpdateOutputFilesControls();
   }
   else if ( sender == GUI->NoiseEvaluationAlgorithm_ComboBox )
      m_instance.p_noiseEvaluationAlgorithm = itemIndex;
   else if ( sender == GUI->OnError_ComboBox )
      m_instance.p_onError = itemIndex;
   else if ( sender == GUI->PSFType_ComboBox )
      m_instance.p_psfType = itemIndex;
   else if ( sender == GUI->PedestalMode_ComboBox )
   {
      m_instance.p_pedestalMode = itemIndex;
      UpdatePedestalControls();
   }
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::e_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->OutputPedestal_SpinBox )
      m_instance.p_outputPedestal = value;
   else if ( sender == GUI->PedestalValue_SpinBox )
      m_instance.p_pedestal = value;
//    else if ( sender == GUI->DarkOptimizationWindow_SpinBox )
//       m_instance.p_darkOptimizationWindow = value;
   else if ( sender == GUI->StructureLayers_SpinBox )
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

void ImageCalibrationInterface::e_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->AutoPedestalLimit_NumericEdit )
      m_instance.p_autoPedestalLimit = value;
   else if ( sender == GUI->SaturationThreshold_NumericControl )
      m_instance.p_saturationThreshold = value;
   else if ( sender == GUI->PSFGrowth_NumericControl )
      m_instance.p_psfGrowth = value;
   else if ( sender == GUI->DarkOptimizationThreshold_NumericControl )
   {
      m_instance.p_darkOptimizationLow = value;
      m_instance.p_darkOptimizationThreshold = 0; // deprecated parameter, for compatibility with old versions.
   }
   else if ( sender == GUI->FlatScaleClippingFactor_NumericControl )
      m_instance.p_flatScaleClippingFactor = value;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::e_OverscanValueUpdated( NumericEdit& sender, double value_ )
{
#define SETX0( r, x0_ ) { int w = r.Width(); r.x1 = (r.x0 = x0_) + w; }
#define SETY0( r, y0_ ) { int h = r.Height(); r.y1 = (r.y0 = y0_) + h; }

   int value = TruncI( value_ );

   if ( sender == GUI->ImageRegionX0_NumericEdit )
      SETX0( m_instance.p_overscan.imageRect, value )
   else if ( sender == GUI->ImageRegionY0_NumericEdit )
      SETY0( m_instance.p_overscan.imageRect, value )
   else if ( sender == GUI->ImageRegionWidth_NumericEdit )
      m_instance.p_overscan.imageRect.SetWidth( value );
   else if ( sender == GUI->ImageRegionHeight_NumericEdit )
      m_instance.p_overscan.imageRect.SetHeight( value );

   else if ( sender == GUI->Overscan1SourceX0_NumericEdit )
      SETX0( m_instance.p_overscan.overscan[0].sourceRect, value )
   else if ( sender == GUI->Overscan1SourceY0_NumericEdit )
      SETY0( m_instance.p_overscan.overscan[0].sourceRect, value )
   else if ( sender == GUI->Overscan1SourceWidth_NumericEdit )
      m_instance.p_overscan.overscan[0].sourceRect.SetWidth( value );
   else if ( sender == GUI->Overscan1SourceHeight_NumericEdit )
      m_instance.p_overscan.overscan[0].sourceRect.SetHeight( value );

   else if ( sender == GUI->Overscan1TargetX0_NumericEdit )
      SETX0( m_instance.p_overscan.overscan[0].targetRect, value )
   else if ( sender == GUI->Overscan1TargetY0_NumericEdit )
      SETY0( m_instance.p_overscan.overscan[0].targetRect, value )
   else if ( sender == GUI->Overscan1TargetWidth_NumericEdit )
      m_instance.p_overscan.overscan[0].targetRect.SetWidth( value );
   else if ( sender == GUI->Overscan1TargetHeight_NumericEdit )
      m_instance.p_overscan.overscan[0].targetRect.SetHeight( value );

   else if ( sender == GUI->Overscan2SourceX0_NumericEdit )
      SETX0( m_instance.p_overscan.overscan[1].sourceRect, value )
   else if ( sender == GUI->Overscan2SourceY0_NumericEdit )
      SETY0( m_instance.p_overscan.overscan[1].sourceRect, value )
   else if ( sender == GUI->Overscan2SourceWidth_NumericEdit )
      m_instance.p_overscan.overscan[1].sourceRect.SetWidth( value );
   else if ( sender == GUI->Overscan2SourceHeight_NumericEdit )
      m_instance.p_overscan.overscan[1].sourceRect.SetHeight( value );

   else if ( sender == GUI->Overscan2TargetX0_NumericEdit )
      SETX0( m_instance.p_overscan.overscan[1].targetRect, value )
   else if ( sender == GUI->Overscan2TargetY0_NumericEdit )
      SETY0( m_instance.p_overscan.overscan[1].targetRect, value )
   else if ( sender == GUI->Overscan2TargetWidth_NumericEdit )
      m_instance.p_overscan.overscan[1].targetRect.SetWidth( value );
   else if ( sender == GUI->Overscan2TargetHeight_NumericEdit )
      m_instance.p_overscan.overscan[1].targetRect.SetHeight( value );

   else if ( sender == GUI->Overscan3SourceX0_NumericEdit )
      SETX0( m_instance.p_overscan.overscan[2].sourceRect, value )
   else if ( sender == GUI->Overscan3SourceY0_NumericEdit )
      SETY0( m_instance.p_overscan.overscan[2].sourceRect, value )
   else if ( sender == GUI->Overscan3SourceWidth_NumericEdit )
      m_instance.p_overscan.overscan[2].sourceRect.SetWidth( value );
   else if ( sender == GUI->Overscan3SourceHeight_NumericEdit )
      m_instance.p_overscan.overscan[2].sourceRect.SetHeight( value );

   else if ( sender == GUI->Overscan3TargetX0_NumericEdit )
      SETX0( m_instance.p_overscan.overscan[2].targetRect, value )
   else if ( sender == GUI->Overscan3TargetY0_NumericEdit )
      SETY0( m_instance.p_overscan.overscan[2].targetRect, value )
   else if ( sender == GUI->Overscan3TargetWidth_NumericEdit )
      m_instance.p_overscan.overscan[2].targetRect.SetWidth( value );
   else if ( sender == GUI->Overscan3TargetHeight_NumericEdit )
      m_instance.p_overscan.overscan[2].targetRect.SetHeight( value );

   else if ( sender == GUI->Overscan4SourceX0_NumericEdit )
      SETX0( m_instance.p_overscan.overscan[3].sourceRect, value )
   else if ( sender == GUI->Overscan4SourceY0_NumericEdit )
      SETY0( m_instance.p_overscan.overscan[3].sourceRect, value )
   else if ( sender == GUI->Overscan4SourceWidth_NumericEdit )
      m_instance.p_overscan.overscan[3].sourceRect.SetWidth( value );
   else if ( sender == GUI->Overscan4SourceHeight_NumericEdit )
      m_instance.p_overscan.overscan[3].sourceRect.SetHeight( value );

   else if ( sender == GUI->Overscan4TargetX0_NumericEdit )
      SETX0( m_instance.p_overscan.overscan[3].targetRect, value )
   else if ( sender == GUI->Overscan4TargetY0_NumericEdit )
      SETY0( m_instance.p_overscan.overscan[3].targetRect, value )
   else if ( sender == GUI->Overscan4TargetWidth_NumericEdit )
      m_instance.p_overscan.overscan[3].targetRect.SetWidth( value );
   else if ( sender == GUI->Overscan4TargetHeight_NumericEdit )
      m_instance.p_overscan.overscan[3].targetRect.SetHeight( value );

#undef SETX0
#undef SETY0
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::e_CheckSection( SectionBar& sender, bool checked )
{
   if ( sender == GUI->SignalEvaluation_SectionBar )
      m_instance.p_evaluateSignal = checked;
   else if ( sender == GUI->NoiseEvaluation_SectionBar )
      m_instance.p_evaluateNoise = checked;
   else if ( sender == GUI->MasterBias_SectionBar )
      m_instance.p_masterBias.enabled = checked;
   else if ( sender == GUI->MasterDark_SectionBar )
      m_instance.p_masterDark.enabled = checked;
   else if ( sender == GUI->MasterFlat_SectionBar )
      m_instance.p_masterFlat.enabled = checked;
   else if ( sender == GUI->Overscan_SectionBar )
      m_instance.p_overscan.enabled = checked;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
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

void ImageCalibrationInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
      wantsFiles = true;
   else if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
   else if ( sender == GUI->MasterBiasPath_Edit || sender == GUI->MasterDarkPath_Edit || sender == GUI->MasterFlatPath_Edit )
      wantsFiles = files.Length() == 1 && File::Exists( files[0] );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
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

      inputFiles.Sort();
      size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
      for ( const String& file : inputFiles )
         m_instance.p_targetFrames.Insert( m_instance.p_targetFrames.At( i0++ ), ImageCalibrationInstance::ImageItem( file ) );

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->OutputDirectory_Edit )
   {
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = files[0] );
   }
   else if ( sender == GUI->MasterBiasPath_Edit )
   {
      if ( File::Exists( files[0] ) )
         GUI->MasterBiasPath_Edit.SetText( m_instance.p_masterBias.path = files[0] );
   }
   else if ( sender == GUI->MasterDarkPath_Edit )
   {
      if ( File::Exists( files[0] ) )
         GUI->MasterDarkPath_Edit.SetText( m_instance.p_masterDark.path = files[0] );
   }
   else if ( sender == GUI->MasterFlatPath_Edit )
   {
      if ( File::Exists( files[0] ) )
         GUI->MasterFlatPath_Edit.SetText( m_instance.p_masterFlat.path = files[0] );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ImageCalibrationInterface::GUIData::GUIData( ImageCalibrationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Optimization window (px):" ) + 'M' );
   int editWidth1 = fnt.Width( String( 'M', 5  ) );
   int editWidth2 = fnt.Width( String( '0', 10  ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   TargetImages_SectionBar.SetTitle( "Target Frames" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::e_ToggleSection, w );

   TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   TargetImages_TreeBox.SetScaledMinWidth( 344 );
   TargetImages_TreeBox.SetNumberOfColumns( 3 );
   TargetImages_TreeBox.HideHeader();
   TargetImages_TreeBox.EnableMultipleSelections();
   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&ImageCalibrationInterface::e_CurrentNodeUpdated, w );
   TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&ImageCalibrationInterface::e_NodeActivated, w );
   TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&ImageCalibrationInterface::e_NodeSelectionUpdated, w );
   TargetImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&ImageCalibrationInterface::e_FileDrag, w );
   TargetImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&ImageCalibrationInterface::e_FileDrop, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of target frames.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all target frames.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of target frames.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected target frames.</p>"
      "<p>Disabled target frames will be ignored during the calibration process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected target frames.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of target frames.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for target frame files.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

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

   DetectCFA_CheckBox.SetText( "Enable CFA" );
   DetectCFA_CheckBox.SetToolTip( "<p>Enable this option to support data acquired with digital cameras that generate "
      "images mosaiced with a Color Filter Array (CFA). Bayer and X-Trans CFAs are supported. When this option is "
      "selected, the CFA pattern can be selected with the <i>CFA mosaic pattern</i> parameter. You normally should "
      "leave that parameter with its default Auto selection, which should detect the correct pattern automatically "
      "under normal working conditions.</p>" );
   DetectCFA_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   DetectCFA_Sizer.SetSpacing( 4 );
   DetectCFA_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   DetectCFA_Sizer.Add( DetectCFA_CheckBox );
   DetectCFA_Sizer.AddStretch();

   const char* patternToolTip =
      "<p>Select the CFA pattern of the camera (such as a DSLR or OSC camera) used to acquire the calibration master "
      "and target frames.</p>"
      "<p>The Auto option uses view properties that can be available in the master and target frame files. Under "
      "normal working conditions, these properties are either generated automatically by the RAW format support module "
      "and stored in XISF files, or available as FITS header keywords in the case of raw OSC frames.</p>"
      "<p>For non-CFA data acquired with monochrome CCD or CMOS cameras, the Auto option is the correct selection "
      "because in these cases the process will detect no CFA pattern automatically.</p>"
      "<p>For images acquired with X-Trans sensors this parameter is ignored and CFA patterns are always extracted "
      "from existing image properties.</p>";

   CFAPattern_Label.SetText( "CFA mosaic pattern:" );
   CFAPattern_Label.SetFixedWidth( labelWidth1 );
   CFAPattern_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CFAPattern_Label.SetToolTip( patternToolTip );

   CFAPattern_ComboBox.AddItem( "Auto" );
   CFAPattern_ComboBox.AddItem( "RGGB" );
   CFAPattern_ComboBox.AddItem( "BGGR" );
   CFAPattern_ComboBox.AddItem( "GBRG" );
   CFAPattern_ComboBox.AddItem( "GRBG" );
   CFAPattern_ComboBox.AddItem( "GRGB" );
   CFAPattern_ComboBox.AddItem( "GBGR" );
   CFAPattern_ComboBox.AddItem( "RGBG" );
   CFAPattern_ComboBox.AddItem( "BGRG" );
   CFAPattern_ComboBox.SetToolTip( patternToolTip );
   CFAPattern_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::e_ItemSelected, w );

   CFAPattern_Sizer.SetSpacing( 4 );
   CFAPattern_Sizer.Add( CFAPattern_Label );
   CFAPattern_Sizer.Add( CFAPattern_ComboBox );
   CFAPattern_Sizer.AddStretch();

   //

   FormatHints_SectionBar.SetTitle( "Format Hints" );
   FormatHints_SectionBar.SetSection( FormatHints_Control );
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::e_ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings "
      "for image files used by specific processes. In ImageCalibration, input hints change the way input images "
      "of some particular file formats (target frames and master calibration frames) are read, while output hints "
      "modify the way output calibrated images are written.</p>"
      "<p>For example, you can use the \"raw cfa\" input hints to force the RAW format to load pure raw CFA frames "
      "without applying any demosaicing, interpolation, white balance, or black point correction. Similarly, you can "
      "use the \"up-bottom\" input hint to load raw images from FITS files with the coordinate origin at the top-left "
      "corner. Most standard file format modules support hints; each format supports a number of input and/or output "
      "hints that you can use for different purposes with tools that give you access to format hints.</p>";

   InputHints_Label.SetText( "Input hints:" );
   InputHints_Label.SetFixedWidth( labelWidth1 );
   InputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InputHints_Label.SetToolTip( hintsToolTip );

   InputHints_Edit.SetToolTip( hintsToolTip );
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::e_EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   OutputHints_Label.SetText( "Output hints:" );
   OutputHints_Label.SetFixedWidth( labelWidth1 );
   OutputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputHints_Label.SetToolTip( hintsToolTip );

   OutputHints_Edit.SetToolTip( hintsToolTip );
   OutputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::e_EditCompleted, w );

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
   OutputFiles_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::e_ToggleSection, w );

   const char* outputDirectoryToolTip = "<p>This is the directory (or folder) where all output files "
      "will be written.</p>"
      "<p>If this field is left blank, output files will be written to the same directories as their "
      "corresponding target files. In this case, make sure that source directories are writable, or the "
      "calibration process will fail.</p>";

   OutputDirectory_Label.SetText( "Output directory:" );
   OutputDirectory_Label.SetFixedWidth( labelWidth1 );
   OutputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputDirectory_Label.SetToolTip( outputDirectoryToolTip );

   OutputDirectory_Edit.SetToolTip( outputDirectoryToolTip );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::e_EditCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)&ImageCalibrationInterface::e_FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)&ImageCalibrationInterface::e_FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.SetToolTip( "<p>Select the output directory</p>" );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   const char* outputPrefixToolTip =
      "<p>This is a prefix that will be prepended to all output file names.</p>";

   OutputPrefix_Label.SetText( "Prefix:" );
   OutputPrefix_Label.SetFixedWidth( labelWidth1 );
   OutputPrefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPrefix_Label.SetToolTip( outputPrefixToolTip );

   OutputPrefix_Edit.SetFixedWidth( editWidth1 );
   OutputPrefix_Edit.SetToolTip( outputPrefixToolTip );
   OutputPrefix_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::e_EditCompleted, w );

   const char* outputPostfixToolTip =
      "<p>This is a postfix that will be appended to the file name of each registered image.</p>";

   OutputPostfix_Label.SetText( "Postfix:" );
   OutputPostfix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPostfix_Label.SetToolTip( outputPostfixToolTip );

   OutputPostfix_Edit.SetFixedWidth( editWidth1 );
   OutputPostfix_Edit.SetToolTip( outputPostfixToolTip );
   OutputPostfix_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::e_EditCompleted, w );

   OutputChunks_Sizer.SetSpacing( 4 );
   OutputChunks_Sizer.Add( OutputPrefix_Label );
   OutputChunks_Sizer.Add( OutputPrefix_Edit );
   OutputChunks_Sizer.AddSpacing( 12 );
   OutputChunks_Sizer.Add( OutputPostfix_Label );
   OutputChunks_Sizer.Add( OutputPostfix_Edit );
   OutputChunks_Sizer.AddStretch();

   const char* sampleFormatToolTip = "<p>Sample data format for newly created calibrated images.</p>";

   OutputSampleFormat_Label.SetText( "Sample format:" );
   OutputSampleFormat_Label.SetToolTip( sampleFormatToolTip );
   OutputSampleFormat_Label.SetMinWidth( labelWidth1 );
   OutputSampleFormat_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputSampleFormat_ComboBox.AddItem( "16-bit integer" );
   OutputSampleFormat_ComboBox.AddItem( "32-bit integer" );
   OutputSampleFormat_ComboBox.AddItem( "32-bit floating point" );
   OutputSampleFormat_ComboBox.AddItem( "64-bit floating point" );
   OutputSampleFormat_ComboBox.SetToolTip( sampleFormatToolTip );
   OutputSampleFormat_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::e_ItemSelected, w );

   OutputSampleFormat_Sizer.SetSpacing( 4 );
   OutputSampleFormat_Sizer.Add( OutputSampleFormat_Label );
   OutputSampleFormat_Sizer.Add( OutputSampleFormat_ComboBox );
   OutputSampleFormat_Sizer.AddStretch();

   const char* outputPedestalModeToolTip = "<p>This parameter defines how to calculate and apply output pedestals.</p>"
      "<p><b>Literal value</b> allows you to specify a pedestal value in 16-bit data number units (DN) with the "
      "<i>output pedestal</i> parameter.</p>"
      "<p><b>Automatic</b> will apply an automatically calculated pedestal value to frames that have negative, zero "
      "or insignificant (to machine epsilon) minimum pixel values after calibration. Automatic pedestals can be used "
      "to ensure positivity of the calibrated image with minimal truncation.</p>"
      "<p>See also the <i>auto pedestal limit</i> and <i>output pedestal</i> parameters for more information.</p>";

   OutputPedestalMode_Label.SetText( "Output pedestal mode:" );
   OutputPedestalMode_Label.SetToolTip( outputPedestalModeToolTip );
   OutputPedestalMode_Label.SetMinWidth( labelWidth1 );
   OutputPedestalMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputPedestalMode_ComboBox.AddItem( "Literal value" );
   OutputPedestalMode_ComboBox.AddItem( "Automatic" );
   OutputPedestalMode_ComboBox.SetToolTip( outputPedestalModeToolTip );
   OutputPedestalMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::e_ItemSelected, w );

   OutputPedestalMode_Sizer.SetSpacing( 4 );
   OutputPedestalMode_Sizer.Add( OutputPedestalMode_Label );
   OutputPedestalMode_Sizer.Add( OutputPedestalMode_ComboBox );
   OutputPedestalMode_Sizer.AddStretch();

   AutoPedestalLimit_NumericEdit.label.SetText( "Auto pedestal limit:" );
   AutoPedestalLimit_NumericEdit.label.SetFixedWidth( labelWidth1 );
   AutoPedestalLimit_NumericEdit.SetReal();
   AutoPedestalLimit_NumericEdit.SetRange( TheICAutoPedestalLimitParameter->MinimumValue(), TheICAutoPedestalLimitParameter->MaximumValue() );
   AutoPedestalLimit_NumericEdit.SetPrecision( TheICAutoPedestalLimitParameter->Precision() );
   AutoPedestalLimit_NumericEdit.edit.SetFixedWidth( editWidth2 );
   AutoPedestalLimit_NumericEdit.sizer.AddStretch();
   AutoPedestalLimit_NumericEdit.SetToolTip( "<p>Maximum fraction of negative or insignificant calibrated pixels allowed "
      "in automatic pedestal generation mode.</p>"
      "<p>This parameter represents a fraction of the total image pixels in the [0,1] range. When the image has more than this "
      "fraction of negative or insignificant pixel values after calibration and the <i>output pedestal mode</i> is set to <i>automatic</i>, "
      "the process will generate an additive pedestal to ensure that no more than this fraction of negative or insignificant pixels will be "
      "present in the calibrated image.</p>"
      "<p>The default value is 0.0001, which represents a 0.01% of the total pixels.</p>" );
   AutoPedestalLimit_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_ValueUpdated, w );

   const char* outputPedestalTip = "<p>The <i>output pedestal</i> is a small quantity expressed in the 16-bit "
      "unsigned integer range (from 0 to 65535). It is added at the end of the calibration process and its "
      "purpose is to prevent negative values that can occur sometimes as a result of overscan and bias subtraction. "
      "Under normal conditions, you should need no pedestal to calibrate science frames because mean sky background "
      "levels should be sufficiently high as to avoid negative values. A small pedestal can be necessary sometimes "
      "to calibrate individual bias and dark frames. The default value is zero.</p>";

   OutputPedestal_Label.SetText( "Output pedestal (DN):" );
   OutputPedestal_Label.SetFixedWidth( labelWidth1 );
   OutputPedestal_Label.SetToolTip( outputPedestalTip );
   OutputPedestal_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputPedestal_SpinBox.SetRange( int( TheICOutputPedestalParameter->MinimumValue() ), int( TheICOutputPedestalParameter->MaximumValue() ) );
   OutputPedestal_SpinBox.SetToolTip( outputPedestalTip );
   OutputPedestal_SpinBox.SetFixedWidth( editWidth2 );
   OutputPedestal_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::e_SpinValueUpdated, w );

   OutputPedestal_Sizer.Add( OutputPedestal_Label );
   OutputPedestal_Sizer.AddSpacing( 4 );
   OutputPedestal_Sizer.Add( OutputPedestal_SpinBox );
   OutputPedestal_Sizer.AddStretch();

   OverwriteExistingFiles_CheckBox.SetText( "Overwrite existing files" );
   OverwriteExistingFiles_CheckBox.SetToolTip( "<p>If this option is selected, ImageCalibration will overwrite "
      "existing files with the same names as generated output files. This can be dangerous because the original "
      "contents of overwritten files will be lost.</p>"
      "<p><b>Enable this option <u>at your own risk.</u></b></p>" );
   OverwriteExistingFiles_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   OverwriteExistingFiles_Sizer.SetSpacing( 4 );
   OverwriteExistingFiles_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OverwriteExistingFiles_Sizer.Add( OverwriteExistingFiles_CheckBox );
   OverwriteExistingFiles_Sizer.AddStretch();

   const char* onErrorToolTip = "<p>Specify what to do if there are errors during the batch calibration process.</p>";

   OnError_Label.SetText( "On error:" );
   OnError_Label.SetFixedWidth( labelWidth1 );
   OnError_Label.SetToolTip( onErrorToolTip );
   OnError_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OnError_ComboBox.AddItem( "Continue" );
   OnError_ComboBox.AddItem( "Abort" );
   OnError_ComboBox.AddItem( "Ask User" );
   OnError_ComboBox.SetToolTip( onErrorToolTip );
   OnError_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::e_ItemSelected, w );

   OnError_Sizer.SetSpacing( 4 );
   OnError_Sizer.Add( OnError_Label );
   OnError_Sizer.Add( OnError_ComboBox );
   OnError_Sizer.AddStretch();

   OutputFiles_Sizer.SetSpacing( 4 );
   OutputFiles_Sizer.Add( OutputDirectory_Sizer );
   OutputFiles_Sizer.Add( OutputChunks_Sizer );
   OutputFiles_Sizer.Add( OutputSampleFormat_Sizer );
   OutputFiles_Sizer.Add( OutputPedestalMode_Sizer );
   OutputFiles_Sizer.Add( AutoPedestalLimit_NumericEdit );
   OutputFiles_Sizer.Add( OutputPedestal_Sizer );
   OutputFiles_Sizer.Add( OverwriteExistingFiles_Sizer );
   OutputFiles_Sizer.Add( OnError_Sizer );

   OutputFiles_Control.SetSizer( OutputFiles_Sizer );

   //

   SignalEvaluation_SectionBar.SetTitle( "Signal Evaluation" );
   SignalEvaluation_SectionBar.SetToolTip( "<p>Compute standardized estimates of the signal present in target calibration frames.</p>"
      "<p>Our current implementation uses PSF photometry to generate robust and accurate measures of total and mean flux from detected "
      "and fitted sources. These estimates, along with estimates of the standard deviation of the noise, can be used for image "
      "grading and weighting with the SubframeSelector and ImageIntegration processes.</p>"
      "<p>The signal evaluation result will be stored as a set of custom, per-channel XISF properties and FITS header keywords that "
      "other processes and scripts can use for specialized image analysis purposes.</p>"
      "<p><b>This option should always be enabled for calibration of deep-sky, non-CFA raw light frames. For CFA data, signal and "
      "noise evaluation should be performed by the Debayer process, and hence this option should be disabled in ImageCalibration</b></p>" );
   SignalEvaluation_SectionBar.SetSection( SignalEvaluation_Control );
   SignalEvaluation_SectionBar.EnableTitleCheckBox();
   SignalEvaluation_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::e_ToggleSection, w );
   SignalEvaluation_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageCalibrationInterface::e_CheckSection, w );

   const char* structureLayersToolTip =
   "<p>Number of wavelet layers used for structure detection.</p>"
   "<p>With more wavelet layers, larger stars (and perhaps also some nonstellar objects) will be detected.</p>";

   StructureLayers_Label.SetText( "Detection scales:" );
   StructureLayers_Label.SetFixedWidth( labelWidth1 );
   StructureLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StructureLayers_Label.SetToolTip( structureLayersToolTip );

   StructureLayers_SpinBox.SetRange( int( TheICStructureLayersParameter->MinimumValue() ), int( TheICStructureLayersParameter->MaximumValue() ) );
   StructureLayers_SpinBox.SetToolTip( structureLayersToolTip );
   StructureLayers_SpinBox.SetFixedWidth( editWidth2 );
   StructureLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::e_SpinValueUpdated, w );

   StructureLayers_Sizer.SetSpacing( 4 );
   StructureLayers_Sizer.Add( StructureLayers_Label );
   StructureLayers_Sizer.Add( StructureLayers_SpinBox );
   StructureLayers_Sizer.AddStretch();

   SaturationThreshold_NumericControl.label.SetText( "Saturation threshold:" );
   SaturationThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   SaturationThreshold_NumericControl.slider.SetRange( 0, 250 );
   SaturationThreshold_NumericControl.SetReal();
   SaturationThreshold_NumericControl.SetRange( TheICSaturationThresholdParameter->MinimumValue(), TheICSaturationThresholdParameter->MaximumValue() );
   SaturationThreshold_NumericControl.SetPrecision( TheICSaturationThresholdParameter->Precision() );
   SaturationThreshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   SaturationThreshold_NumericControl.SetToolTip( "<p>Saturation threshold in the [0,1] range.</p>"
      "<p>Detected stars with one or more pixels with values above this threshold will be excluded for signal evaluation. "
      "This parameter is expressed in the [0,1] range. It can be applied either as an absolute pixel sample value in the "
      "normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image (see the <i>Relative "
      "saturation threshold</i> parameter).</p>"
      "<p>The default saturation threshold is 1.0. For signal evaluation, the implemented star detection and outlier rejection "
      "routines are normally able to avoid contamination from saturated sources, so the default value of this parameter should "
      "not be changed under normal conditions.</p>" );
   SaturationThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_ValueUpdated, w );

   SaturationRelative_CheckBox.SetText( "Relative saturation threshold" );
   SaturationRelative_CheckBox.SetToolTip( "<p>The saturation threshold parameter can be applied either as an absolute pixel "
      "sample value in the normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image.</p>"
      "The relative saturation threshold option is enabled by default.</p>" );
   SaturationRelative_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

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

   NoiseLayers_SpinBox.SetRange( int( TheICNoiseLayersParameter->MinimumValue() ), int( TheICNoiseLayersParameter->MaximumValue() ) );
   NoiseLayers_SpinBox.SetToolTip( noiseLayersToolTip );
   NoiseLayers_SpinBox.SetFixedWidth( editWidth2 );
   NoiseLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::e_SpinValueUpdated, w );

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

   MinStructureSize_SpinBox.SetRange( int( TheICMinStructureSizeParameter->MinimumValue() ), int( TheICMinStructureSizeParameter->MaximumValue() ) );
   MinStructureSize_SpinBox.SetToolTip( minStructureSizeToolTip );
   MinStructureSize_SpinBox.SetFixedWidth( editWidth2 );
   MinStructureSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::e_SpinValueUpdated, w );

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

   HotPixelFilterRadius_SpinBox.SetRange( int( TheICHotPixelFilterRadiusParameter->MinimumValue() ), int( TheICHotPixelFilterRadiusParameter->MaximumValue() ) );
   HotPixelFilterRadius_SpinBox.SetToolTip( hotPixelFilterRadiusToolTip );
   HotPixelFilterRadius_SpinBox.SetFixedWidth( editWidth2 );
   HotPixelFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::e_SpinValueUpdated, w );

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

   NoiseReductionFilterRadius_SpinBox.SetRange( int( TheICNoiseReductionFilterRadiusParameter->MinimumValue() ), int( TheICNoiseReductionFilterRadiusParameter->MaximumValue() ) );
   NoiseReductionFilterRadius_SpinBox.SetToolTip( noiseReductionFilterRadiusToolTip );
   NoiseReductionFilterRadius_SpinBox.SetFixedWidth( editWidth2 );
   NoiseReductionFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::e_SpinValueUpdated, w );

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
   PSFType_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::e_ItemSelected, w );

   PSFType_Sizer.SetSpacing( 4 );
   PSFType_Sizer.Add( PSFType_Label );
   PSFType_Sizer.Add( PSFType_ComboBox );
   PSFType_Sizer.AddStretch();

   PSFGrowth_NumericControl.label.SetText( "Growth factor:" );
   PSFGrowth_NumericControl.label.SetFixedWidth( labelWidth1 );
   PSFGrowth_NumericControl.slider.SetRange( 0, 250 );
   PSFGrowth_NumericControl.SetReal();
   PSFGrowth_NumericControl.SetRange( TheICPSFGrowthParameter->MinimumValue(), TheICPSFGrowthParameter->MaximumValue() );
   PSFGrowth_NumericControl.SetPrecision( TheICPSFGrowthParameter->Precision() );
   PSFGrowth_NumericControl.edit.SetFixedWidth( editWidth1 );
   PSFGrowth_NumericControl.SetToolTip( "<p>Growing factor for expansion/contraction of the PSF flux measurement region for "
      "each source, in units of the Full Width at Tenth Maximum (FWTM).</p>"
      "<p>The default value of this parameter is 1.0, meaning that flux is measured exclusively for pixels within the elliptical "
      "region defined at one tenth of the fitted PSF maximum. Increasing this parameter can inprove accuracy of PSF flux "
      "measurements for undersampled images, where PSF fitting uncertainty is relatively large. Decreasing it can be beneficial "
      "in some cases working with very noisy data to restrict flux evaluation to star cores.</p>" );
   PSFGrowth_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_ValueUpdated, w );

   const char* maxStarsToolTip =
      "<p>The maximum number of stars that can be measured to compute PSF flux estimates.</p>"
      "<p>PSF photometry will be performed for no more than the specified number of stars. The subset of measured stars "
      "will always start at the beginning of the set of detected stars, sorted by brightness in descending order.</p>"
      "<p>The default value imposes a generous limit of 24K stars. Limiting the number of photometric samples can improve "
      "performance for calibration of wide-field frames, where the number of detected stars can be very large. However, "
      "reducing the set of measured sources too much will damage the accuracy of signal estimation.</p>";

   MaxStars_Label.SetText( "Maximum stars:" );
   MaxStars_Label.SetFixedWidth( labelWidth1 );
   MaxStars_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MaxStars_Label.SetToolTip( maxStarsToolTip );

   MaxStars_SpinBox.SetRange( int( TheICMaxStarsParameter->MinimumValue() ), int( TheICMaxStarsParameter->MaximumValue() ) );
   MaxStars_SpinBox.SetToolTip( maxStarsToolTip );
   MaxStars_SpinBox.SetFixedWidth( editWidth2 );
   MaxStars_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::e_SpinValueUpdated, w );

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
      "and noise scaling factors for each target image, using a multiscale algorithm (MRS noise estimator by "
      "default). Noise estimates will be stored as a set of custom XISF properties and FITS header keywords in the "
      "output files.</p>"
      "<p>These estimates can be used later by several processes and scripts for specialized image analysis purposes, "
      "most notably by SubframeSelector and ImageIntegration for image grading and weighting.</p>"
      "<p><b>This option should always be enabled for calibration of non-CFA raw light frames. For CFA data, signal "
      "and noise evaluation should be performed by the Debayer process, and hence this option should be disabled in "
      "ImageCalibration in such case.</b></p>" );
   NoiseEvaluation_SectionBar.SetSection( NoiseEvaluation_Control );
   NoiseEvaluation_SectionBar.EnableTitleCheckBox();
   NoiseEvaluation_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::e_ToggleSection, w );
   NoiseEvaluation_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageCalibrationInterface::e_CheckSection, w );

   const char* noiseEvaluationToolTip =
      "<p>Noise evaluation algorithm. This option selects an algorithm for automatic estimation of the standard deviation of "
      "the noise in the calibrated images.</p>"
      "<p>The <b>multiresolution support (MRS)</b> noise estimation routine implements the iterative algorithm described by "
      "Jean-Luc Starck and Fionn Murtagh in their paper <em>Automatic Noise Estimation from the Multiresolution Support</em> "
      "(Publications of the Royal Astronomical Society of the Pacific, vol. 110, pp. 193-199). In our implementation, the "
      "standard deviation of the noise is evaluated on the first four wavelet layers, assuming a Gaussian noise distribution. "
      "MRS is a remarkably accurate and robust algorithm and the default option for noise evaluation.</p>"
      "<p>The <b>iterative k-sigma clipping</b> algorithm can be used as a last-resort option in cases where the MRS algorithm "
      "does not converge systematically. This can happen on images with no detectable small-scale noise, but should never "
      "happen with uncalibrated data under normal working conditions.</p>"
      "<p>The <b>N* robust noise estimator</b> extracts a subset of residual pixels by comparison with a large-scale model of "
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
   NoiseEvaluationAlgorithm_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::e_ItemSelected, w );

   NoiseEvaluationAlgorithm_Sizer.SetSpacing( 4 );
   NoiseEvaluationAlgorithm_Sizer.Add( NoiseEvaluationAlgorithm_Label );
   NoiseEvaluationAlgorithm_Sizer.Add( NoiseEvaluationAlgorithm_ComboBox );
   NoiseEvaluationAlgorithm_Sizer.AddStretch();

   NoiseEvaluation_Sizer.SetSpacing( 4 );
   NoiseEvaluation_Sizer.Add( NoiseEvaluationAlgorithm_Sizer );

   NoiseEvaluation_Control.SetSizer( NoiseEvaluation_Sizer );

   //

   Pedestal_SectionBar.SetTitle( "Input Pedestal" );
   Pedestal_SectionBar.SetSection( Pedestal_Control );
   Pedestal_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::e_ToggleSection, w );

   const char* pedestalModeTip =
      "<p>The <i>pedestal mode</i> option specifies how to retrieve a (usually small) quantity that is subtracted from input "
      "images as the very first calibration step. This quantity is known as <i>pedestal</i>, and must be expressed in the "
      "16-bit unsigned integer range (from 0 to 65535). If present, a pedestal has been added by a previous calibration process "
      "to ensure positivity of the result. For example, you might have specified an <i>output pedestal</i> (see the "
      "corresponding parameter above) to enforce positivity of previously calibrated master bias, dark and flat frames.</p>"
      "<p><b>Literal value</b> lets you specify a pedestal value that will be subtracted from all target and master calibration "
      "frames. You usually don't need this option, except in very special cases.</p>"
      "<p><b>Default FITS keyword</b> is the default mode. When this mode is selected, ImageCalibration will retrieve pedestal "
      "values from 'PEDESTAL' FITS keywords, when they are present in the input images. Note that ImageCalibration writes "
      "PEDESTAL keywords to calibrated images when a nonzero <i>output pedestal</i> is specified.</p>"
      "<p><b>Custom FITS keyword</b> allows you to specify the name of a custom FITS keyword to retrieve pedestal values, when "
      "the specified keyword is present in the input images.</p>";

   PedestalMode_Label.SetText( "Pedestal mode:" );
   PedestalMode_Label.SetToolTip( pedestalModeTip );
   PedestalMode_Label.SetMinWidth( labelWidth1 );
   PedestalMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   PedestalMode_ComboBox.AddItem( "Literal value" );
   PedestalMode_ComboBox.AddItem( "Default FITS keyword (PEDESTAL)" );
   PedestalMode_ComboBox.AddItem( "Custom FITS keyword" );
   PedestalMode_ComboBox.SetToolTip( pedestalModeTip );
   PedestalMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::e_ItemSelected, w );

   PedestalMode_Sizer.SetSpacing( 4 );
   PedestalMode_Sizer.Add( PedestalMode_Label );
   PedestalMode_Sizer.Add( PedestalMode_ComboBox );
   PedestalMode_Sizer.AddStretch();

   const char* pedestalValueTip =
      "<p>Literal pedestal value in the 16-bit unsigned integer range (from 0 to 65535), when the <i>Literal value</i> option "
      "has been selected as <i>pedestal mode</i>.</p>";

   PedestalValue_Label.SetText( "Pedestal value (DN):" );
   PedestalValue_Label.SetFixedWidth( labelWidth1 );
   PedestalValue_Label.SetToolTip( pedestalValueTip );
   PedestalValue_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   PedestalValue_SpinBox.SetRange( int( TheICPedestalParameter->MinimumValue() ), int( TheICPedestalParameter->MaximumValue() ) );
   PedestalValue_SpinBox.SetToolTip( pedestalValueTip );
   PedestalValue_SpinBox.SetFixedWidth( editWidth2 );
   PedestalValue_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::e_SpinValueUpdated, w );

   PedestalValue_Sizer.Add( PedestalValue_Label );
   PedestalValue_Sizer.AddSpacing( 4 );
   PedestalValue_Sizer.Add( PedestalValue_SpinBox );
   PedestalValue_Sizer.AddStretch();

   const char* pedestalKeywordTip =
      "<p>Custom FITS keyword to retrieve pedestal values in 16-bit DN.</p>"
      "<p>This is the name of a FITS keyword that will be used to retrieve input pedestals, if the <i>Custom FITS keyword</i> "
      "option has been selected as <i>pedestal mode</i>.</p>";

   PedestalKeyword_Label.SetText( "Pedestal keyword:" );
   PedestalKeyword_Label.SetFixedWidth( labelWidth1 );
   PedestalKeyword_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PedestalKeyword_Label.SetToolTip( pedestalKeywordTip );

   PedestalKeyword_Edit.SetMinWidth( editWidth2 );
   PedestalKeyword_Edit.SetToolTip( pedestalKeywordTip );
   PedestalKeyword_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::e_EditCompleted, w );

   PedestalKeyword_Sizer.SetSpacing( 4 );
   PedestalKeyword_Sizer.Add( PedestalKeyword_Label );
   PedestalKeyword_Sizer.Add( PedestalKeyword_Edit );
   PedestalKeyword_Sizer.AddStretch();

   Pedestal_Sizer.SetSpacing( 4 );
   Pedestal_Sizer.Add( PedestalMode_Sizer );
   Pedestal_Sizer.Add( PedestalValue_Sizer );
   Pedestal_Sizer.Add( PedestalKeyword_Sizer );

   Pedestal_Control.SetSizer( Pedestal_Sizer );

   //

   Overscan_SectionBar.SetTitle( "Overscan" );
   Overscan_SectionBar.SetSection( Overscan_Control );
   Overscan_SectionBar.EnableTitleCheckBox();
   Overscan_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::e_ToggleSection, w );
   Overscan_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageCalibrationInterface::e_CheckSection, w );

   //

   ImageRegion_Label.SetText( "Image region:" );
   ImageRegion_Label.SetFixedWidth( labelWidth1 );
   ImageRegion_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ImageRegion_Label.SetToolTip( "<p>The image region defines a rectangular area of the CCD frame that "
      "will be cropped after overscan correction. It usually corresponds to the true science image "
      "without any overscan regions.</p>" );

   ImageRegionX0_NumericEdit.SetInteger();
   ImageRegionX0_NumericEdit.SetRange( 0, uint16_max );
   ImageRegionX0_NumericEdit.label.Hide();
   ImageRegionX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ImageRegionX0_NumericEdit.SetToolTip( "Image region, left pixel coordinate" );
   ImageRegionX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   ImageRegionY0_NumericEdit.SetInteger();
   ImageRegionY0_NumericEdit.SetRange( 0, uint16_max );
   ImageRegionY0_NumericEdit.label.Hide();
   ImageRegionY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ImageRegionY0_NumericEdit.SetToolTip( "Image region, top pixel coordinate" );
   ImageRegionY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   ImageRegionWidth_NumericEdit.SetInteger();
   ImageRegionWidth_NumericEdit.SetRange( 0, uint16_max );
   ImageRegionWidth_NumericEdit.label.Hide();
   ImageRegionWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ImageRegionWidth_NumericEdit.SetToolTip( "Image region, width in pixels" );
   ImageRegionWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   ImageRegionHeight_NumericEdit.SetInteger();
   ImageRegionHeight_NumericEdit.SetRange( 0, uint16_max );
   ImageRegionHeight_NumericEdit.label.Hide();
   ImageRegionHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ImageRegionHeight_NumericEdit.SetToolTip( "Image region, height in pixels" );
   ImageRegionHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   ImageRegion_Sizer.SetSpacing( 4 );
   ImageRegion_Sizer.Add( ImageRegion_Label );
   ImageRegion_Sizer.Add( ImageRegionX0_NumericEdit );
   ImageRegion_Sizer.Add( ImageRegionY0_NumericEdit );
   ImageRegion_Sizer.Add( ImageRegionWidth_NumericEdit );
   ImageRegion_Sizer.Add( ImageRegionHeight_NumericEdit );
   ImageRegion_Sizer.AddStretch();

   //

   Overscan1_CheckBox.SetText( "Overscan #1" );
   Overscan1_CheckBox.SetToolTip( "<p>Enable the first overscan. Each overscan allows you to define "
      "arbitrary <i>source and target overscan regions</i> in CCD pixel coordinates. A source overscan "
      "region is used to compute an overscan correction value (as the median of all source region pixels) "
      "that will be subtracted from all pixels in the corresponding target region.</p>" );
   Overscan1_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   Overscan1_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Overscan1_Sizer.Add( Overscan1_CheckBox );
   Overscan1_Sizer.AddStretch();

   Overscan1Source_Label.SetText( "Source region:" );
   Overscan1Source_Label.SetFixedWidth( labelWidth1 );
   Overscan1Source_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan1Source_Label.SetToolTip( "<p>Overscan source region #1</p>" );

   Overscan1SourceX0_NumericEdit.SetInteger();
   Overscan1SourceX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan1SourceX0_NumericEdit.label.Hide();
   Overscan1SourceX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1SourceX0_NumericEdit.SetToolTip( "Overscan #1, source region, left pixel coordinate" );
   Overscan1SourceX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan1SourceY0_NumericEdit.SetInteger();
   Overscan1SourceY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan1SourceY0_NumericEdit.label.Hide();
   Overscan1SourceY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1SourceY0_NumericEdit.SetToolTip( "Overscan #1, source region, top pixel coordinate" );
   Overscan1SourceY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan1SourceWidth_NumericEdit.SetInteger();
   Overscan1SourceWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan1SourceWidth_NumericEdit.label.Hide();
   Overscan1SourceWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1SourceWidth_NumericEdit.SetToolTip( "Overscan #1, source region, width in pixels" );
   Overscan1SourceWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan1SourceHeight_NumericEdit.SetInteger();
   Overscan1SourceHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan1SourceHeight_NumericEdit.label.Hide();
   Overscan1SourceHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1SourceHeight_NumericEdit.SetToolTip( "Overscan #1, source region, height in pixels" );
   Overscan1SourceHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan1Source_Sizer.SetSpacing( 4 );
   Overscan1Source_Sizer.Add( Overscan1Source_Label );
   Overscan1Source_Sizer.Add( Overscan1SourceX0_NumericEdit );
   Overscan1Source_Sizer.Add( Overscan1SourceY0_NumericEdit );
   Overscan1Source_Sizer.Add( Overscan1SourceWidth_NumericEdit );
   Overscan1Source_Sizer.Add( Overscan1SourceHeight_NumericEdit );
   Overscan1Source_Sizer.AddStretch();

   //

   Overscan1Target_Label.SetText( "Target region:" );
   Overscan1Target_Label.SetFixedWidth( labelWidth1 );
   Overscan1Target_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan1Target_Label.SetToolTip( "<p>Overscan target region #1</p>" );

   Overscan1TargetX0_NumericEdit.SetInteger();
   Overscan1TargetX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan1TargetX0_NumericEdit.label.Hide();
   Overscan1TargetX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1TargetX0_NumericEdit.SetToolTip( "Overscan #1, target region, left pixel coordinate" );
   Overscan1TargetX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan1TargetY0_NumericEdit.SetInteger();
   Overscan1TargetY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan1TargetY0_NumericEdit.label.Hide();
   Overscan1TargetY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1TargetY0_NumericEdit.SetToolTip( "Overscan #1, target region, top pixel coordinate" );
   Overscan1TargetY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan1TargetWidth_NumericEdit.SetInteger();
   Overscan1TargetWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan1TargetWidth_NumericEdit.label.Hide();
   Overscan1TargetWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1TargetWidth_NumericEdit.SetToolTip( "Overscan #1, target region, width in pixels" );
   Overscan1TargetWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan1TargetHeight_NumericEdit.SetInteger();
   Overscan1TargetHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan1TargetHeight_NumericEdit.label.Hide();
   Overscan1TargetHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1TargetHeight_NumericEdit.SetToolTip( "Overscan #1, target region, height in pixels" );
   Overscan1TargetHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan1Target_Sizer.SetSpacing( 4 );
   Overscan1Target_Sizer.Add( Overscan1Target_Label );
   Overscan1Target_Sizer.Add( Overscan1TargetX0_NumericEdit );
   Overscan1Target_Sizer.Add( Overscan1TargetY0_NumericEdit );
   Overscan1Target_Sizer.Add( Overscan1TargetWidth_NumericEdit );
   Overscan1Target_Sizer.Add( Overscan1TargetHeight_NumericEdit );
   Overscan1Target_Sizer.AddStretch();

   //

   Overscan2_CheckBox.SetText( "Overscan #2" );
   Overscan2_CheckBox.SetToolTip( "<p>Enable the second overscan. Each overscan allows you to define "
      "arbitrary <i>source and target overscan regions</i> in CCD pixel coordinates. A source overscan "
      "region is used to compute an overscan correction value (as the median of all source region pixels) "
      "that will be subtracted from all pixels in the corresponding target region.</p>" );
   Overscan2_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   Overscan2_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Overscan2_Sizer.Add( Overscan2_CheckBox );
   Overscan2_Sizer.AddStretch();

   Overscan2Source_Label.SetText( "Source region:" );
   Overscan2Source_Label.SetFixedWidth( labelWidth1 );
   Overscan2Source_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan2Source_Label.SetToolTip( "<p>Overscan source region #2</p>" );

   Overscan2SourceX0_NumericEdit.SetInteger();
   Overscan2SourceX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan2SourceX0_NumericEdit.label.Hide();
   Overscan2SourceX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2SourceX0_NumericEdit.SetToolTip( "Overscan #2, source region, left pixel coordinate" );
   Overscan2SourceX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan2SourceY0_NumericEdit.SetInteger();
   Overscan2SourceY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan2SourceY0_NumericEdit.label.Hide();
   Overscan2SourceY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2SourceY0_NumericEdit.SetToolTip( "Overscan #2, source region, top pixel coordinate" );
   Overscan2SourceY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan2SourceWidth_NumericEdit.SetInteger();
   Overscan2SourceWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan2SourceWidth_NumericEdit.label.Hide();
   Overscan2SourceWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2SourceWidth_NumericEdit.SetToolTip( "Overscan #2, source region, width in pixels" );
   Overscan2SourceWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan2SourceHeight_NumericEdit.SetInteger();
   Overscan2SourceHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan2SourceHeight_NumericEdit.label.Hide();
   Overscan2SourceHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2SourceHeight_NumericEdit.SetToolTip( "Overscan #2, source region, height in pixels" );
   Overscan2SourceHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan2Source_Sizer.SetSpacing( 4 );
   Overscan2Source_Sizer.Add( Overscan2Source_Label );
   Overscan2Source_Sizer.Add( Overscan2SourceX0_NumericEdit );
   Overscan2Source_Sizer.Add( Overscan2SourceY0_NumericEdit );
   Overscan2Source_Sizer.Add( Overscan2SourceWidth_NumericEdit );
   Overscan2Source_Sizer.Add( Overscan2SourceHeight_NumericEdit );
   Overscan2Source_Sizer.AddStretch();

   //

   Overscan2Target_Label.SetText( "Target region:" );
   Overscan2Target_Label.SetFixedWidth( labelWidth1 );
   Overscan2Target_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan2Target_Label.SetToolTip( "<p>Overscan target region #2</p>" );

   Overscan2TargetX0_NumericEdit.SetInteger();
   Overscan2TargetX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan2TargetX0_NumericEdit.label.Hide();
   Overscan2TargetX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2TargetX0_NumericEdit.SetToolTip( "Overscan #2, target region, left pixel coordinate" );
   Overscan2TargetX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan2TargetY0_NumericEdit.SetInteger();
   Overscan2TargetY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan2TargetY0_NumericEdit.label.Hide();
   Overscan2TargetY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2TargetY0_NumericEdit.SetToolTip( "Overscan #2, target region, top pixel coordinate" );
   Overscan2TargetY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan2TargetWidth_NumericEdit.SetInteger();
   Overscan2TargetWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan2TargetWidth_NumericEdit.label.Hide();
   Overscan2TargetWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2TargetWidth_NumericEdit.SetToolTip( "Overscan #2, target region, width in pixels" );
   Overscan2TargetWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan2TargetHeight_NumericEdit.SetInteger();
   Overscan2TargetHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan2TargetHeight_NumericEdit.label.Hide();
   Overscan2TargetHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2TargetHeight_NumericEdit.SetToolTip( "Overscan #2, target region, height in pixels" );
   Overscan2TargetHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan2Target_Sizer.SetSpacing( 4 );
   Overscan2Target_Sizer.Add( Overscan2Target_Label );
   Overscan2Target_Sizer.Add( Overscan2TargetX0_NumericEdit );
   Overscan2Target_Sizer.Add( Overscan2TargetY0_NumericEdit );
   Overscan2Target_Sizer.Add( Overscan2TargetWidth_NumericEdit );
   Overscan2Target_Sizer.Add( Overscan2TargetHeight_NumericEdit );
   Overscan2Target_Sizer.AddStretch();

   //

   Overscan3_CheckBox.SetText( "Overscan #3" );
   Overscan3_CheckBox.SetToolTip( "<p>Enable the third overscan. Each overscan allows you to define "
      "arbitrary <i>source and target overscan regions</i> in CCD pixel coordinates. A source overscan "
      "region is used to compute an overscan correction value (as the median of all source region pixels) "
      "that will be subtracted from all pixels in the corresponding target region.</p>" );
   Overscan3_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   Overscan3_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Overscan3_Sizer.Add( Overscan3_CheckBox );
   Overscan3_Sizer.AddStretch();

   Overscan3Source_Label.SetText( "Source region:" );
   Overscan3Source_Label.SetFixedWidth( labelWidth1 );
   Overscan3Source_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan3Source_Label.SetToolTip( "<p>Overscan source region #3</p>" );

   Overscan3SourceX0_NumericEdit.SetInteger();
   Overscan3SourceX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan3SourceX0_NumericEdit.label.Hide();
   Overscan3SourceX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3SourceX0_NumericEdit.SetToolTip( "Overscan #3, source region, left pixel coordinate" );
   Overscan3SourceX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan3SourceY0_NumericEdit.SetInteger();
   Overscan3SourceY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan3SourceY0_NumericEdit.label.Hide();
   Overscan3SourceY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3SourceY0_NumericEdit.SetToolTip( "Overscan #3, source region, top pixel coordinate" );
   Overscan3SourceY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan3SourceWidth_NumericEdit.SetInteger();
   Overscan3SourceWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan3SourceWidth_NumericEdit.label.Hide();
   Overscan3SourceWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3SourceWidth_NumericEdit.SetToolTip( "Overscan #3, source region, width in pixels" );
   Overscan3SourceWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan3SourceHeight_NumericEdit.SetInteger();
   Overscan3SourceHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan3SourceHeight_NumericEdit.label.Hide();
   Overscan3SourceHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3SourceHeight_NumericEdit.SetToolTip( "Overscan #3, source region, height in pixels" );
   Overscan3SourceHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan3Source_Sizer.SetSpacing( 4 );
   Overscan3Source_Sizer.Add( Overscan3Source_Label );
   Overscan3Source_Sizer.Add( Overscan3SourceX0_NumericEdit );
   Overscan3Source_Sizer.Add( Overscan3SourceY0_NumericEdit );
   Overscan3Source_Sizer.Add( Overscan3SourceWidth_NumericEdit );
   Overscan3Source_Sizer.Add( Overscan3SourceHeight_NumericEdit );
   Overscan3Source_Sizer.AddStretch();

   //

   Overscan3Target_Label.SetText( "Target region:" );
   Overscan3Target_Label.SetFixedWidth( labelWidth1 );
   Overscan3Target_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan3Target_Label.SetToolTip( "<p>Overscan target region #3</p>" );

   Overscan3TargetX0_NumericEdit.SetInteger();
   Overscan3TargetX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan3TargetX0_NumericEdit.label.Hide();
   Overscan3TargetX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3TargetX0_NumericEdit.SetToolTip( "Overscan #3, target region, left pixel coordinate" );
   Overscan3TargetX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan3TargetY0_NumericEdit.SetInteger();
   Overscan3TargetY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan3TargetY0_NumericEdit.label.Hide();
   Overscan3TargetY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3TargetY0_NumericEdit.SetToolTip( "Overscan #3, target region, top pixel coordinate" );
   Overscan3TargetY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan3TargetWidth_NumericEdit.SetInteger();
   Overscan3TargetWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan3TargetWidth_NumericEdit.label.Hide();
   Overscan3TargetWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3TargetWidth_NumericEdit.SetToolTip( "Overscan #3, target region, width in pixels" );
   Overscan3TargetWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan3TargetHeight_NumericEdit.SetInteger();
   Overscan3TargetHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan3TargetHeight_NumericEdit.label.Hide();
   Overscan3TargetHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3TargetHeight_NumericEdit.SetToolTip( "Overscan #3, target region, height in pixels" );
   Overscan3TargetHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan3Target_Sizer.SetSpacing( 4 );
   Overscan3Target_Sizer.Add( Overscan3Target_Label );
   Overscan3Target_Sizer.Add( Overscan3TargetX0_NumericEdit );
   Overscan3Target_Sizer.Add( Overscan3TargetY0_NumericEdit );
   Overscan3Target_Sizer.Add( Overscan3TargetWidth_NumericEdit );
   Overscan3Target_Sizer.Add( Overscan3TargetHeight_NumericEdit );
   Overscan3Target_Sizer.AddStretch();

   //

   Overscan4_CheckBox.SetText( "Overscan #4" );
   Overscan4_CheckBox.SetToolTip( "<p>Enable the fourth overscan. Each overscan allows you to define "
      "arbitrary <i>source and target overscan regions</i> in CCD pixel coordinates. A source overscan "
      "region is used to compute an overscan correction value (as the median of all source region pixels) "
      "that will be subtracted from all pixels in the corresponding target region.</p>" );
   Overscan4_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   Overscan4_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Overscan4_Sizer.Add( Overscan4_CheckBox );
   Overscan4_Sizer.AddStretch();

   Overscan4Source_Label.SetText( "Source region:" );
   Overscan4Source_Label.SetFixedWidth( labelWidth1 );
   Overscan4Source_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan4Source_Label.SetToolTip( "<p>Overscan source region #4</p>" );

   Overscan4SourceX0_NumericEdit.SetInteger();
   Overscan4SourceX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan4SourceX0_NumericEdit.label.Hide();
   Overscan4SourceX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4SourceX0_NumericEdit.SetToolTip( "Overscan #4, source region, left pixel coordinate" );
   Overscan4SourceX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan4SourceY0_NumericEdit.SetInteger();
   Overscan4SourceY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan4SourceY0_NumericEdit.label.Hide();
   Overscan4SourceY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4SourceY0_NumericEdit.SetToolTip( "Overscan #4, source region, top pixel coordinate" );
   Overscan4SourceY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan4SourceWidth_NumericEdit.SetInteger();
   Overscan4SourceWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan4SourceWidth_NumericEdit.label.Hide();
   Overscan4SourceWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4SourceWidth_NumericEdit.SetToolTip( "Overscan #4, source region, width in pixels" );
   Overscan4SourceWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan4SourceHeight_NumericEdit.SetInteger();
   Overscan4SourceHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan4SourceHeight_NumericEdit.label.Hide();
   Overscan4SourceHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4SourceHeight_NumericEdit.SetToolTip( "Overscan #4, source region, height in pixels" );
   Overscan4SourceHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan4Source_Sizer.SetSpacing( 4 );
   Overscan4Source_Sizer.Add( Overscan4Source_Label );
   Overscan4Source_Sizer.Add( Overscan4SourceX0_NumericEdit );
   Overscan4Source_Sizer.Add( Overscan4SourceY0_NumericEdit );
   Overscan4Source_Sizer.Add( Overscan4SourceWidth_NumericEdit );
   Overscan4Source_Sizer.Add( Overscan4SourceHeight_NumericEdit );
   Overscan4Source_Sizer.AddStretch();

   //

   Overscan4Target_Label.SetText( "Target region:" );
   Overscan4Target_Label.SetFixedWidth( labelWidth1 );
   Overscan4Target_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan4Target_Label.SetToolTip( "<p>Overscan target region #4</p>" );

   Overscan4TargetX0_NumericEdit.SetInteger();
   Overscan4TargetX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan4TargetX0_NumericEdit.label.Hide();
   Overscan4TargetX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4TargetX0_NumericEdit.SetToolTip( "Overscan #4, target region, left pixel coordinate" );
   Overscan4TargetX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan4TargetY0_NumericEdit.SetInteger();
   Overscan4TargetY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan4TargetY0_NumericEdit.label.Hide();
   Overscan4TargetY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4TargetY0_NumericEdit.SetToolTip( "Overscan #4, target region, top pixel coordinate" );
   Overscan4TargetY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan4TargetWidth_NumericEdit.SetInteger();
   Overscan4TargetWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan4TargetWidth_NumericEdit.label.Hide();
   Overscan4TargetWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4TargetWidth_NumericEdit.SetToolTip( "Overscan #4, target region, width in pixels" );
   Overscan4TargetWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan4TargetHeight_NumericEdit.SetInteger();
   Overscan4TargetHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan4TargetHeight_NumericEdit.label.Hide();
   Overscan4TargetHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4TargetHeight_NumericEdit.SetToolTip( "Overscan #4, target region, height in pixels" );
   Overscan4TargetHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_OverscanValueUpdated, w );

   Overscan4Target_Sizer.SetSpacing( 4 );
   Overscan4Target_Sizer.Add( Overscan4Target_Label );
   Overscan4Target_Sizer.Add( Overscan4TargetX0_NumericEdit );
   Overscan4Target_Sizer.Add( Overscan4TargetY0_NumericEdit );
   Overscan4Target_Sizer.Add( Overscan4TargetWidth_NumericEdit );
   Overscan4Target_Sizer.Add( Overscan4TargetHeight_NumericEdit );
   Overscan4Target_Sizer.AddStretch();

   //

   Overscan_Sizer.SetSpacing( 4 );
   Overscan_Sizer.Add( ImageRegion_Sizer );
   Overscan_Sizer.Add( Overscan1_Sizer );
   Overscan_Sizer.Add( Overscan1Source_Sizer );
   Overscan_Sizer.Add( Overscan1Target_Sizer );
   Overscan_Sizer.Add( Overscan2_Sizer );
   Overscan_Sizer.Add( Overscan2Source_Sizer );
   Overscan_Sizer.Add( Overscan2Target_Sizer );
   Overscan_Sizer.Add( Overscan3_Sizer );
   Overscan_Sizer.Add( Overscan3Source_Sizer );
   Overscan_Sizer.Add( Overscan3Target_Sizer );
   Overscan_Sizer.Add( Overscan4_Sizer );
   Overscan_Sizer.Add( Overscan4Source_Sizer );
   Overscan_Sizer.Add( Overscan4Target_Sizer );

   Overscan_Control.SetSizer( Overscan_Sizer );

   //

   MasterBias_SectionBar.SetTitle( "Master Bias" );
   MasterBias_SectionBar.SetSection( MasterBias_Control );
   MasterBias_SectionBar.EnableTitleCheckBox();
   MasterBias_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::e_ToggleSection, w );
   MasterBias_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageCalibrationInterface::e_CheckSection, w );

   MasterBiasPath_Edit.SetToolTip( "<p>File path of the master bias frame.</p>" );
   MasterBiasPath_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::e_EditCompleted, w );
   MasterBiasPath_Edit.OnFileDrag( (Control::file_drag_event_handler)&ImageCalibrationInterface::e_FileDrag, w );
   MasterBiasPath_Edit.OnFileDrop( (Control::file_drop_event_handler)&ImageCalibrationInterface::e_FileDrop, w );

   MasterBiasPath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   MasterBiasPath_ToolButton.SetScaledFixedSize( 20, 20 );
   MasterBiasPath_ToolButton.SetToolTip( "<p>Select the master bias image file.</p>" );
   MasterBiasPath_ToolButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   MasterBiasPath_Sizer.Add( MasterBiasPath_Edit, 100 );
   MasterBiasPath_Sizer.AddSpacing( 4 );
   MasterBiasPath_Sizer.Add( MasterBiasPath_ToolButton );

   CalibrateMasterBias_CheckBox.SetText( "Calibrate" );
   CalibrateMasterBias_CheckBox.SetToolTip( "<p>Select this option to calibrate the master bias frame "
      "at the beginning of the batch calibration process. Bias frames are only corrected for overscan, when "
      "one or more overscan regions have been defined and are enabled.</p>" );
   CalibrateMasterBias_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   CalibrateMasterBias_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CalibrateMasterBias_Sizer.Add( CalibrateMasterBias_CheckBox );
   CalibrateMasterBias_Sizer.AddStretch();

   MasterBias_Sizer.Add( MasterBiasPath_Sizer );
   MasterBias_Sizer.AddSpacing( 4 );
   MasterBias_Sizer.Add( CalibrateMasterBias_Sizer );

   MasterBias_Control.SetSizer( MasterBias_Sizer );

   //

   MasterDark_SectionBar.SetTitle( "Master Dark" );
   MasterDark_SectionBar.SetSection( MasterDark_Control );
   MasterDark_SectionBar.EnableTitleCheckBox();
   MasterDark_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::e_ToggleSection, w );
   MasterDark_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageCalibrationInterface::e_CheckSection, w );

   MasterDarkPath_Edit.SetToolTip( "<p>File path of the master dark frame.</p>" );
   MasterDarkPath_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::e_EditCompleted, w );
   MasterDarkPath_Edit.OnFileDrag( (Control::file_drag_event_handler)&ImageCalibrationInterface::e_FileDrag, w );
   MasterDarkPath_Edit.OnFileDrop( (Control::file_drop_event_handler)&ImageCalibrationInterface::e_FileDrop, w );

   MasterDarkPath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   MasterDarkPath_ToolButton.SetScaledFixedSize( 20, 20 );
   MasterDarkPath_ToolButton.SetToolTip( "<p>Select the master dark image file.</p>" );
   MasterDarkPath_ToolButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   MasterDarkPath_Sizer.Add( MasterDarkPath_Edit, 100 );
   MasterDarkPath_Sizer.AddSpacing( 4 );
   MasterDarkPath_Sizer.Add( MasterDarkPath_ToolButton );

   CalibrateMasterDark_CheckBox.SetText( "Calibrate" );
   CalibrateMasterDark_CheckBox.SetToolTip( "<p>Select this option to calibrate the master dark frame "
      "at the beginning of the batch calibration process. The master dark frame is corrected for overscan, "
      "when one or more overscan regions have been defined and are enabled, and bias-subtracted, if a master "
      "bias frame has been selected and is enabled.</p>" );
   CalibrateMasterDark_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   CalibrateMasterDark_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CalibrateMasterDark_Sizer.Add( CalibrateMasterDark_CheckBox );
   CalibrateMasterDark_Sizer.AddStretch();

   OptimizeDarks_CheckBox.SetText( "Optimize" );
   OptimizeDarks_CheckBox.SetToolTip( "<p>Select this option to apply <i>dark frame optimization.</i></p>"
      "<p>The dark frame optimization routine computes a dark scaling factor to minimize the noise "
      "induced by dark subtraction. Optimization is carried out separately for each target frame, including "
      "the master flat frame, if selected. Dark frame optimization has been implemented using robust linear "
      "minimization of variance.</p>" );
   OptimizeDarks_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   OptimizeDarks_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OptimizeDarks_Sizer.Add( OptimizeDarks_CheckBox );
   OptimizeDarks_Sizer.AddStretch();

   DarkOptimizationThreshold_NumericControl.label.SetText( "Optimization threshold:" );
   DarkOptimizationThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   DarkOptimizationThreshold_NumericControl.slider.SetRange( 0, 200 );
   DarkOptimizationThreshold_NumericControl.SetReal();
   DarkOptimizationThreshold_NumericControl.SetRange( TheICDarkOptimizationLowParameter->MinimumValue(), TheICDarkOptimizationLowParameter->MaximumValue() );
   DarkOptimizationThreshold_NumericControl.SetPrecision( TheICDarkOptimizationLowParameter->Precision() );
   DarkOptimizationThreshold_NumericControl.edit.SetFixedWidth( editWidth2 );
   DarkOptimizationThreshold_NumericControl.SetToolTip( "<p>Lower bound for the set of dark optimization pixels, "
      "measured in sigma units from the median.</p>"
      "<p>This parameter defines the set of dark frame pixels that will be used to compute dark optimization "
      "factors adaptively. By restricting this set to relatively bright pixels, the optimization process can "
      "be more robust to readout noise present in the master bias and dark frames. Increase this parameter to "
      "remove more dark pixels from the optimization set.</p>" );
   DarkOptimizationThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_ValueUpdated, w );

//    const char* darkOptimizationWindowTip = "<p>This parameter is the size in pixels of a square region "
//       "used to compute noise estimates during the dark optimization procedure. The square region is centered "
//       "on each target image. By default, a <i>window</i> of one megapixel (1024x1024 pixels) is used.</p>"
//       "<p>By using a reduced subset of pixels, the dark optimization process can be much faster, and noise "
//       "evaluation on a reduced region is in general as accurate as noise evaluation on the whole image, for "
//       "dark frame scaling purposes.</p>"
//       "<p>To disable this feature, and hence to use the whole image to compute noise estimates, select a "
//       "zero window size. If the selected window size is larger than the dimensions of the target image, then "
//       "it will be ignored and the whole image will be used for noise evaluation.</p>";
//
//    DarkOptimizationWindow_Label.SetText( "Optimization window (px):" );
//    DarkOptimizationWindow_Label.SetFixedWidth( labelWidth1 );
//    DarkOptimizationWindow_Label.SetToolTip( darkOptimizationWindowTip );
//    DarkOptimizationWindow_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
//
//    DarkOptimizationWindow_SpinBox.SetRange( int( TheICDarkOptimizationWindowParameter->MinimumValue() ), int( TheICDarkOptimizationWindowParameter->MaximumValue() ) );
//    DarkOptimizationWindow_SpinBox.SetToolTip( darkOptimizationWindowTip );
//    DarkOptimizationWindow_SpinBox.SetFixedWidth( editWidth2 );
//    DarkOptimizationWindow_SpinBox.SetStepSize( 256 );
//    DarkOptimizationWindow_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::e_SpinValueUpdated, w );
//
//    DarkOptimizationWindow_Sizer.Add( DarkOptimizationWindow_Label );
//    DarkOptimizationWindow_Sizer.AddSpacing( 4 );
//    DarkOptimizationWindow_Sizer.Add( DarkOptimizationWindow_SpinBox );
//    DarkOptimizationWindow_Sizer.AddStretch();

   MasterDark_Sizer.SetSpacing( 4 );
   MasterDark_Sizer.Add( MasterDarkPath_Sizer );
   MasterDark_Sizer.Add( CalibrateMasterDark_Sizer );
   MasterDark_Sizer.Add( OptimizeDarks_Sizer );
   MasterDark_Sizer.Add( DarkOptimizationThreshold_NumericControl );
//    MasterDark_Sizer.Add( DarkOptimizationWindow_Sizer );

   MasterDark_Control.SetSizer( MasterDark_Sizer );

   //

   MasterFlat_SectionBar.SetTitle( "Master Flat" );
   MasterFlat_SectionBar.SetSection( MasterFlat_Control );
   MasterFlat_SectionBar.EnableTitleCheckBox();
   MasterFlat_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::e_ToggleSection, w );
   MasterFlat_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageCalibrationInterface::e_CheckSection, w );

   MasterFlatPath_Edit.SetToolTip( "<p>File path of the master flat frame.</p>" );
   MasterFlatPath_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::e_EditCompleted, w );
   MasterFlatPath_Edit.OnFileDrag( (Control::file_drag_event_handler)&ImageCalibrationInterface::e_FileDrag, w );
   MasterFlatPath_Edit.OnFileDrop( (Control::file_drop_event_handler)&ImageCalibrationInterface::e_FileDrop, w );

   MasterFlatPath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   MasterFlatPath_ToolButton.SetScaledFixedSize( 20, 20 );
   MasterFlatPath_ToolButton.SetToolTip( "<p>Select the master flat image file.</p>" );
   MasterFlatPath_ToolButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   MasterFlatPath_Sizer.Add( MasterFlatPath_Edit, 100 );
   MasterFlatPath_Sizer.AddSpacing( 4 );
   MasterFlatPath_Sizer.Add( MasterFlatPath_ToolButton );

   CalibrateMasterFlat_CheckBox.SetText( "Calibrate" );
   CalibrateMasterFlat_CheckBox.SetToolTip( "<p>Select this option to calibrate the master flat frame "
      "at the beginning of the batch calibration process. The master flat frame is corrected for overscan, "
      "bias-subtracted, and dark-subtracted with optional optimization, when the corresponding overscan "
      "regions and master calibration frames have been defined and are enabled.</p>" );
   CalibrateMasterFlat_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   CalibrateMasterFlat_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CalibrateMasterFlat_Sizer.Add( CalibrateMasterFlat_CheckBox );
   CalibrateMasterFlat_Sizer.AddStretch();

   SeparateCFAFlatScalingFactors_CheckBox.SetText( "Separate CFA flat scaling factors" );
   SeparateCFAFlatScalingFactors_CheckBox.SetToolTip( "<p>When this option is enabled and the master flat frame is "
      "a single-channel image mosaiced with a Color Filter Array (CFA), such as a Bayer or X-Trans filter pattern, "
      "three separate master flat scaling factors are computed for the red, green and blue CFA components, "
      "respectively. When this option is disabled, a single scaling factor is computed for the whole master flat "
      "frame, ignoring CFA components.</p>" );
   SeparateCFAFlatScalingFactors_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::e_Click, w );

   SeparateCFAFlatScalingFactors_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SeparateCFAFlatScalingFactors_Sizer.Add( SeparateCFAFlatScalingFactors_CheckBox );
   SeparateCFAFlatScalingFactors_Sizer.AddStretch();

   FlatScaleClippingFactor_NumericControl.label.SetText( "Scale clipping factor:" );
   FlatScaleClippingFactor_NumericControl.label.SetFixedWidth( labelWidth1 );
   FlatScaleClippingFactor_NumericControl.slider.SetRange( 0, 250 );
   FlatScaleClippingFactor_NumericControl.SetReal();
   FlatScaleClippingFactor_NumericControl.SetRange( TheICFlatScaleClippingFactorParameter->MinimumValue(),
                                                    TheICFlatScaleClippingFactorParameter->MaximumValue() );
   FlatScaleClippingFactor_NumericControl.SetPrecision( TheICFlatScaleClippingFactorParameter->Precision() );
   FlatScaleClippingFactor_NumericControl.edit.SetFixedWidth( editWidth2 );
   FlatScaleClippingFactor_NumericControl.SetToolTip( "<p>Master flat frame scaling factors are computed as robust "
      "mean pixel values for each channel or CFA component. This parameter defines the fraction of pixels that will "
      "be rejected to compute two-sided, symmetric trimmed means. The default value is 0.05, which rejects a 5% of "
      "pixels at both ends of the flat frame distribution.</p>" );
   FlatScaleClippingFactor_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::e_ValueUpdated, w );

   MasterFlat_Sizer.SetSpacing( 4 );
   MasterFlat_Sizer.Add( MasterFlatPath_Sizer );
   MasterFlat_Sizer.Add( CalibrateMasterFlat_Sizer );
   MasterFlat_Sizer.Add( SeparateCFAFlatScalingFactors_Sizer );
   MasterFlat_Sizer.Add( FlatScaleClippingFactor_NumericControl );

   MasterFlat_Control.SetSizer( MasterFlat_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( TargetImages_SectionBar );
   Global_Sizer.Add( TargetImages_Control );
   Global_Sizer.Add( DetectCFA_Sizer );
   Global_Sizer.Add( CFAPattern_Sizer );
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add( OutputFiles_SectionBar );
   Global_Sizer.Add( OutputFiles_Control );
   Global_Sizer.Add( SignalEvaluation_SectionBar );
   Global_Sizer.Add( SignalEvaluation_Control );
   Global_Sizer.Add( NoiseEvaluation_SectionBar );
   Global_Sizer.Add( NoiseEvaluation_Control );
   Global_Sizer.Add( Pedestal_SectionBar );
   Global_Sizer.Add( Pedestal_Control );
   Global_Sizer.Add( Overscan_SectionBar );
   Global_Sizer.Add( Overscan_Control );
   Global_Sizer.Add( MasterBias_SectionBar );
   Global_Sizer.Add( MasterBias_Control );
   Global_Sizer.Add( MasterDark_SectionBar );
   Global_Sizer.Add( MasterDark_Control );
   Global_Sizer.Add( MasterFlat_SectionBar );
   Global_Sizer.Add( MasterFlat_Control );

   w.SetSizer( Global_Sizer );

   FormatHints_Control.Hide();
   SignalEvaluation_Control.Hide();
   NoiseEvaluation_Control.Hide();
   Pedestal_Control.Hide();
   Overscan_Control.Hide();

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedWidth();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageCalibrationInterface.cpp - Released 2022-11-21T14:47:17Z
