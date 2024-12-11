//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 2.2.7
// ----------------------------------------------------------------------------
// LocalNormalizationInterface.cpp - Released 2024-12-11T17:43:17Z
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

#include "LocalNormalizationInterface.h"
#include "LocalNormalizationProcess.h"
#include "LocalNormalizationParameters.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/MultiViewSelectionDialog.h>
#include <pcl/ViewSelectionDialog.h>

#define IMAGELIST_MINHEIGHT( fnt )  RoundInt( 8.125*fnt.Height() )

namespace pcl
{

// ----------------------------------------------------------------------------

LocalNormalizationInterface* TheLocalNormalizationInterface = nullptr;

// ----------------------------------------------------------------------------

LocalNormalizationInterface::LocalNormalizationInterface()
   : m_instance( TheLocalNormalizationProcess )
{
   TheLocalNormalizationInterface = this;

   /*
    * The auto save geometry feature is of no good to interfaces that include
    * both auto-expanding controls (e.g. TreeBox) and collapsible sections
    * (e.g. SectionBar).
    */
   DisableAutoSaveGeometry();
}

// ----------------------------------------------------------------------------

LocalNormalizationInterface::~LocalNormalizationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString LocalNormalizationInterface::Id() const
{
   return "LocalNormalization";
}

// ----------------------------------------------------------------------------

MetaProcess* LocalNormalizationInterface::Process() const
{
   return TheLocalNormalizationProcess;
}

// ----------------------------------------------------------------------------

String LocalNormalizationInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/LocalNormalization.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures LocalNormalizationInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::ApplyGlobalButton;
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::ResetInstance()
{
   LocalNormalizationInstance defaultInstance( TheLocalNormalizationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "LocalNormalization" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheLocalNormalizationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* LocalNormalizationInterface::NewProcess() const
{
   return new LocalNormalizationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const LocalNormalizationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a LocalNormalization instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateControls()
{
   UpdateGeneralParameterControls();
   UpdateOutlierRejectionControls();
   UpdateScaleEvaluationControls();
   UpdateTargetImagesList();
   UpdateImageSelectionButtons();
   UpdateFormatHintsControls();
   UpdateOutputFilesControls();
}

// ----------------------------------------------------------------------------

static int NormalizationScaleToComboBoxItem( int scale )
{
   return LocalNormalizationProcess::SupportedNormalizationScales().Find(
            LocalNormalizationProcess::NearestSupportedNormalizationScale( scale ) );
}

static int ComboBoxItemToNormalizationScale( int item )
{
   const IVector& supportedScales = LocalNormalizationProcess::SupportedNormalizationScales();
   return supportedScales[Range( item, 0, supportedScales.Length()-1 )];
}

void LocalNormalizationInterface::UpdateGeneralParameterControls()
{
   GUI->ReferenceImage_Edit.SetText( m_instance.p_referencePathOrViewId );
   GUI->ReferenceImage_ComboBox.SetCurrentItem( m_instance.p_referenceIsView ? 1 : 0 );

   GUI->Scale_ComboBox.SetCurrentItem( NormalizationScaleToComboBoxItem( m_instance.p_scale ) );
   GUI->GenerateNormalizedImages_ComboBox.SetCurrentItem( m_instance.p_generateNormalizedImages );
   GUI->NoScale_CheckBox.SetChecked( m_instance.p_noScale );
   GUI->GlobalLocationNormalization_CheckBox.SetChecked( m_instance.p_globalLocationNormalization );
   GUI->GenerateNormalizationData_CheckBox.SetChecked( m_instance.p_generateNormalizationData );
   GUI->ShowBackgroundModels_CheckBox.SetChecked( m_instance.p_showBackgroundModels );
//    GUI->PlotNormalizationFunctions_ComboBox.SetCurrentItem( m_instance.p_plotNormalizationFunctions );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateOutlierRejectionControls()
{
   GUI->OutlierRejection_SectionBar.SetChecked( m_instance.p_rejection );
   GUI->HotPixelFilterRadius_SpinBox.SetValue( m_instance.p_hotPixelFilterRadius );
   GUI->NoiseReductionFilterRadius_SpinBox.SetValue( m_instance.p_noiseReductionFilterRadius );
   GUI->LowClippingLevel_NumericEdit.SetValue( m_instance.p_lowClippingLevel );
   GUI->HighClippingLevel_NumericControl.SetValue( m_instance.p_highClippingLevel );
   GUI->ReferenceRejectionThreshold_NumericControl.SetValue( m_instance.p_referenceRejectionThreshold );
   GUI->TargetRejectionThreshold_NumericControl.SetValue( m_instance.p_targetRejectionThreshold );
   GUI->ReferenceRejection_CheckBox.SetChecked( m_instance.p_referenceRejection );
   GUI->ShowRejectionMaps_CheckBox.SetChecked( m_instance.p_showRejectionMaps );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateScaleEvaluationControls()
{
   GUI->ScaleEvaluationMethod_ComboBox.SetCurrentItem( m_instance.p_scaleEvaluationMethod );

   GUI->LocalScaleCorrections_CheckBox.SetChecked( m_instance.p_localScaleCorrections );
   GUI->LocalScaleCorrections_CheckBox.Enable( m_instance.p_scaleEvaluationMethod == LNScaleEvaluationMethod::PSFSignal );

   GUI->ShowLocalScaleModels_CheckBox.SetChecked( m_instance.p_showLocalScaleModels );
   GUI->ShowLocalScaleModels_CheckBox.Enable( m_instance.p_scaleEvaluationMethod == LNScaleEvaluationMethod::PSFSignal &&
                                              m_instance.p_localScaleCorrections );

   GUI->StructureLayers_SpinBox.SetValue( m_instance.p_structureLayers );

   GUI->SaturationThreshold_NumericControl.SetValue( m_instance.p_saturationThreshold );

   GUI->SaturationRelative_CheckBox.SetChecked( m_instance.p_saturationRelative );

   GUI->RejectionLimit_NumericControl.SetValue( m_instance.p_rejectionLimit );

   GUI->PSFNoiseLayers_SpinBox.SetValue( m_instance.p_psfNoiseLayers );
   GUI->PSFMinStructureSize_SpinBox.SetValue( m_instance.p_psfMinStructureSize );
   GUI->PSFHotPixelFilterRadius_SpinBox.SetValue( m_instance.p_psfHotPixelFilterRadius );
   GUI->PSFNoiseReductionFilterRadius_SpinBox.SetValue( m_instance.p_psfNoiseReductionFilterRadius );
   GUI->PSFMinSNR_NumericEdit.SetValue( m_instance.p_psfMinSNR );
   GUI->PSFType_ComboBox.SetCurrentItem( m_instance.p_psfType );
   GUI->PSFGrowth_NumericControl.SetValue( m_instance.p_psfGrowth );
   GUI->PSFMaxStars_SpinBox.SetValue( m_instance.p_psfMaxStars );
   GUI->PSFAllowClusteredSources_CheckBox.SetChecked( m_instance.p_psfAllowClusteredSources );

   GUI->PSFScaleEvaluation_Control.Enable( m_instance.p_scaleEvaluationMethod == LNScaleEvaluationMethod::PSFSignal );

   GUI->ShowStructureMaps_CheckBox.SetChecked( m_instance.p_showStructureMaps );
   GUI->ShowStructureMaps_CheckBox.Enable( m_instance.p_scaleEvaluationMethod == LNScaleEvaluationMethod::MultiscaleAnalysis );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateTargetImageItem( size_type i )
{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const LocalNormalizationInstance::Item& item = m_instance.p_targets[i];

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

void LocalNormalizationInterface::UpdateTargetImagesList()
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

void LocalNormalizationInterface::UpdateImageSelectionButtons()
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

void LocalNormalizationInterface::UpdateFormatHintsControls()
{
   GUI->InputHints_Edit.SetText( m_instance.p_inputHints );
   GUI->OutputHints_Edit.SetText( m_instance.p_outputHints );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateOutputFilesControls()
{
   GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory );
   GUI->OutputPrefix_Edit.SetText( m_instance.p_outputPrefix );
   GUI->OutputPostfix_Edit.SetText( m_instance.p_outputPostfix );
   GUI->OverwriteExistingFiles_CheckBox.SetChecked( m_instance.p_overwriteExistingFiles );
   GUI->OnError_ComboBox.SetCurrentItem( m_instance.p_onError );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_EditCompleted( Edit& sender )
{
   try
   {
      String text = sender.Text().Trimmed();

      if ( sender == GUI->ReferenceImage_Edit )
      {
         if ( !text.IsEmpty() )
            if ( m_instance.p_referenceIsView )
               if ( !View::IsValidViewId( text ) )
               {
                  sender.SetText( m_instance.p_referencePathOrViewId );
                  throw Error( "Invalid view identifier '" + text + '\'' );
               }

         m_instance.p_referencePathOrViewId = text;
      }
      else if ( sender == GUI->InputHints_Edit )
      {
         m_instance.p_inputHints = text;
      }
      else if ( sender == GUI->OutputHints_Edit )
      {
         m_instance.p_outputHints = text;
      }
      else if ( sender == GUI->OutputDirectory_Edit )
      {
         m_instance.p_outputDirectory = text;
      }
      else if ( sender == GUI->OutputPrefix_Edit )
      {
         m_instance.p_outputPrefix = text;
      }
      else if ( sender == GUI->OutputPostfix_Edit )
      {
         m_instance.p_outputPostfix = text;
      }

      sender.SetText( text );
   }
   ERROR_CLEANUP(
      sender.SelectAll();
      sender.Focus()
   )
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->ReferenceImage_ComboBox )
   {
      bool isView = itemIndex > 0;
      if ( isView != bool( m_instance.p_referenceIsView ) ) // VC++ needs an explicit cast to bool. Argh!
      {
         m_instance.p_referenceIsView = isView;
         GUI->ReferenceImage_Edit.SetText( m_instance.p_referencePathOrViewId = String() );
      }
   }
   else if ( sender == GUI->Scale_ComboBox )
   {
      m_instance.p_scale = ComboBoxItemToNormalizationScale( itemIndex );
   }
   else if ( sender == GUI->GenerateNormalizedImages_ComboBox )
   {
      m_instance.p_generateNormalizedImages = itemIndex;
   }
//    else if ( sender == GUI->PlotNormalizationFunctions_ComboBox )
//    {
//       m_instance.p_plotNormalizationFunctions = itemIndex;
//    }
   else if ( sender == GUI->ScaleEvaluationMethod_ComboBox )
   {
      m_instance.p_scaleEvaluationMethod = itemIndex;
      UpdateScaleEvaluationControls();
   }
   else if ( sender == GUI->PSFType_ComboBox )
   {
      m_instance.p_psfType = itemIndex;
   }
   else if ( sender == GUI->OnError_ComboBox )
   {
      m_instance.p_onError = itemIndex;
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->ReferenceImage_ToolButton )
   {
      if ( m_instance.p_referenceIsView )
      {
         ViewSelectionDialog d( m_instance.p_referencePathOrViewId );
         d.SetWindowTitle( "LocalNormalization: Select Reference Image" );
         if ( d.Execute() != StdDialogCode::Ok )
            return;
         m_instance.p_referencePathOrViewId = d.Id();
      }
      else
      {
         OpenFileDialog d;
         d.SetCaption( "LocalNormalization: Select Reference Image" );
         d.LoadImageFilters();
         d.DisableMultipleSelections();
         if ( !d.Execute() )
            return;
         m_instance.p_referencePathOrViewId = d.FileName();
      }

      GUI->ReferenceImage_Edit.SetText( m_instance.p_referencePathOrViewId );
   }
   else if ( sender == GUI->NoScale_CheckBox )
   {
      m_instance.p_noScale = checked;
   }
   else if ( sender == GUI->GlobalLocationNormalization_CheckBox )
   {
      m_instance.p_globalLocationNormalization = checked;
   }
   else if ( sender == GUI->GenerateNormalizationData_CheckBox )
   {
      m_instance.p_generateNormalizationData = checked;
   }
   else if ( sender == GUI->ShowBackgroundModels_CheckBox )
   {
      m_instance.p_showBackgroundModels = checked;
   }
   else if ( sender == GUI->ReferenceRejection_CheckBox )
   {
      m_instance.p_referenceRejection = checked;
   }
   else if ( sender == GUI->ShowRejectionMaps_CheckBox )
   {
      m_instance.p_showRejectionMaps = checked;
   }
   else if ( sender == GUI->LocalScaleCorrections_CheckBox )
   {
      m_instance.p_localScaleCorrections = checked;
      UpdateScaleEvaluationControls();
   }
   else if ( sender == GUI->ShowLocalScaleModels_CheckBox )
   {
      m_instance.p_showLocalScaleModels = checked;
   }
   else if ( sender == GUI->SaturationRelative_CheckBox )
   {
      m_instance.p_saturationRelative = checked;
   }
   else if ( sender == GUI->PSFAllowClusteredSources_CheckBox )
   {
      m_instance.p_psfAllowClusteredSources = checked;
   }
   else if ( sender == GUI->ShowStructureMaps_CheckBox )
   {
      m_instance.p_showStructureMaps = checked;
   }
   else if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "LocalNormalization: Select Target Images" );
      d.LoadImageFilters();
      d.EnableMultipleSelections();
      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
         for ( const String& fileName : d.FileNames() )
            m_instance.p_targets.Insert( m_instance.p_targets.At( i0++ ), LocalNormalizationInstance::Item( fileName ) );

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
      LocalNormalizationInstance::item_list newTargets;
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
      d.SetCaption( "LocalNormalization: Select Output Directory" );
      if ( d.Execute() )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = d.Directory() );
   }
   else if ( sender == GUI->OverwriteExistingFiles_CheckBox )
   {
      m_instance.p_overwriteExistingFiles = checked;
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_CheckSection( SectionBar& sender, bool checked )
{
   if ( sender == GUI->OutlierRejection_SectionBar )
   {
      m_instance.p_rejection = checked;
      UpdateOutlierRejectionControls();
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->LowClippingLevel_NumericEdit )
      m_instance.p_lowClippingLevel = value;
   else if ( sender == GUI->HighClippingLevel_NumericControl )
      m_instance.p_highClippingLevel = value;
   else if ( sender == GUI->ReferenceRejectionThreshold_NumericControl )
      m_instance.p_referenceRejectionThreshold = value;
   else if ( sender == GUI->TargetRejectionThreshold_NumericControl )
      m_instance.p_targetRejectionThreshold = value;
   else if ( sender == GUI->SaturationThreshold_NumericControl )
      m_instance.p_saturationThreshold = value;
   else if ( sender == GUI->RejectionLimit_NumericControl )
      m_instance.p_rejectionLimit = value;
   else if ( sender == GUI->PSFMinSNR_NumericEdit )
      m_instance.p_psfMinSNR = value;
   else if ( sender == GUI->PSFGrowth_NumericControl )
      m_instance.p_psfGrowth = value;
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.p_targets.Length() )
      throw Error( "LocalNormalizationInterface: *Warning* Corrupted interface structures" );

   // ### If there's something that depends on which image is selected in the list, do it now.
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= m_instance.p_targets.Length() )
      throw Error( "LocalNormalizationInterface: *Warning* Corrupted interface structures" );

   LocalNormalizationInstance::Item& item = m_instance.p_targets[index];

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

void LocalNormalizationInterface::e_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->HotPixelFilterRadius_SpinBox )
      m_instance.p_hotPixelFilterRadius = value;
   else if ( sender == GUI->NoiseReductionFilterRadius_SpinBox )
      m_instance.p_noiseReductionFilterRadius = value;
   else if ( sender == GUI->StructureLayers_SpinBox )
      m_instance.p_structureLayers = value;
   else if ( sender == GUI->PSFNoiseLayers_SpinBox )
      m_instance.p_psfNoiseLayers = value;
   else if ( sender == GUI->PSFMinStructureSize_SpinBox )
      m_instance.p_psfMinStructureSize = value;
   else if ( sender == GUI->PSFHotPixelFilterRadius_SpinBox )
      m_instance.p_psfHotPixelFilterRadius = value;
   else if ( sender == GUI->PSFNoiseReductionFilterRadius_SpinBox )
      m_instance.p_psfNoiseReductionFilterRadius = value;
   else if ( sender == GUI->PSFMaxStars_SpinBox )
      m_instance.p_psfMaxStars = value;
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
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

void LocalNormalizationInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->ReferenceImage_Edit )
      wantsFiles = files.Length() == 1 && FileFormat::IsSupportedFileFormatBySuffix( files[0], true/*toRead*/ ) && File::Exists( files[0] );
   else if ( sender == GUI->TargetImages_TreeBox.Viewport() )
      wantsFiles = true;
   else if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->ReferenceImage_Edit )
   {
      if ( FileFormat::IsSupportedFileFormatBySuffix( files[0], true/*toRead*/ ) )
         if ( File::Exists( files[0] ) )
         {
            m_instance.p_referenceIsView = false;
            m_instance.p_referencePathOrViewId = files[0];
            UpdateGeneralParameterControls();
         }
   }
   else if ( sender == GUI->TargetImages_TreeBox.Viewport() )
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
         m_instance.p_targets.Insert( m_instance.p_targets.At( i0++ ), LocalNormalizationInstance::Item( file ) );

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

void LocalNormalizationInterface::e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->ReferenceImage_Edit )
      wantsView = true;
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->ReferenceImage_Edit )
   {
      m_instance.p_referenceIsView = true;
      m_instance.p_referencePathOrViewId = view.FullId();
      UpdateGeneralParameterControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

LocalNormalizationInterface::GUIData::GUIData( LocalNormalizationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Minimum structure size:" ) + "MM" );
   int editWidth1 = fnt.Width( String( '0', 8 ) );
   int editWidth2 = fnt.Width( String( 'M', 5 ) );
   int editWidth3 = fnt.Width( String( '0', 10 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   const char* referenceToolTip = "<p>Normalization reference image.</p>"
      "<p>Local normalization functions will be calculated for each target image with respect to the reference image, "
      "which will be acquired either from an image view, or from an existing disk file.</p>"
      "<p><b>Important:</b> For execution in the global context (batch procedure), if the reference image refers to a disk "
      "file that will be part of an image integration task, it must also be included in the target images list. Otherwise no "
      ".xnml file and/or normalized image will be generated for the reference image.</p>";

   ReferenceImage_Label.SetText( "Reference image:" );
   ReferenceImage_Label.SetFixedWidth( labelWidth1 );
   ReferenceImage_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ReferenceImage_Label.SetToolTip( referenceToolTip );

   ReferenceImage_Edit.SetToolTip( referenceToolTip );
   ReferenceImage_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );
   ReferenceImage_Edit.OnFileDrag( (Control::file_drag_event_handler)&LocalNormalizationInterface::e_FileDrag, w );
   ReferenceImage_Edit.OnFileDrop( (Control::file_drop_event_handler)&LocalNormalizationInterface::e_FileDrop, w );
   ReferenceImage_Edit.OnViewDrag( (Control::view_drag_event_handler)&LocalNormalizationInterface::e_ViewDrag, w );
   ReferenceImage_Edit.OnViewDrop( (Control::view_drop_event_handler)&LocalNormalizationInterface::e_ViewDrop, w );

   ReferenceImage_ComboBox.AddItem( "File" );
   ReferenceImage_ComboBox.AddItem( "View" );
   ReferenceImage_ComboBox.SetToolTip( "<p>Select what the reference image refers to: Either a disk file or a view.</p>" );
   ReferenceImage_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalNormalizationInterface::e_ItemSelected, w );

   ReferenceImage_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select.png" ) ) );
   ReferenceImage_ToolButton.SetScaledFixedSize( 20, 20 );
   ReferenceImage_ToolButton.SetToolTip( "<p>Select the reference image</p>" );
   ReferenceImage_ToolButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   ReferenceImage_Sizer.SetSpacing( 4 );
   ReferenceImage_Sizer.Add( ReferenceImage_Label );
   ReferenceImage_Sizer.Add( ReferenceImage_Edit, 100 );
   ReferenceImage_Sizer.Add( ReferenceImage_ComboBox );
   ReferenceImage_Sizer.Add( ReferenceImage_ToolButton );

   //

   {
      const IVector& scales = LocalNormalizationProcess::SupportedNormalizationScales();

      String scaleTip = "<p>Normalization scale.</p>"
         "<p>LocalNormalization implements a multiscale normalization algorithm. This parameter is the size in pixels of the "
         "sampling scale for local image normalization. The larger this parameter, the less locally adaptive the local "
         "normalization function will be. Smaller values tend to reproduce variations among small-scale structures in the "
         "reference image. Larger values tend to reproduce variations among large-scale structures.</p>"
         "<p>To better understand the role of this parameter, suppose we applied the algorithm at the scale of one pixel. The "
         "result would be an exact copy of the reference image. On the other hand, if we applied the algorithm at a scale similar "
         "to the size of the whole image, the result would be a <i>global normalization</i>: a single linear function would be "
         "applied for normalization of the entire target image.</p>"
         "<p>The default scale is 1024 pixels, which is quite appropriate for most deep-sky images. Suitable scales are generally "
         "in the range from 256 to 1024 pixels, depending on the dimensions of the image and the complexity of the gradients. "
         "Although in theory the value of this parameter could be set arbitrarily, for performance and accuracy reasons the "
         "current implementation is limited to a set of prescribed scales in the range from "
         + String( scales[0] ) + " to " + String( scales[scales.Length()-1] ) + " pixels.</p>";

      Scale_Label.SetText( "Scale:" );
      Scale_Label.SetFixedWidth( labelWidth1 );
      Scale_Label.SetToolTip( scaleTip );
      Scale_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

      for ( int scale : scales )
         Scale_ComboBox.AddItem( String( scale ) );

      Scale_ComboBox.SetMinWidth( editWidth3 );
      Scale_ComboBox.SetToolTip( scaleTip );
      Scale_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalNormalizationInterface::e_ItemSelected, w );
   }

   Scale_Sizer.SetSpacing( 4 );
   Scale_Sizer.Add( Scale_Label );
   Scale_Sizer.Add( Scale_ComboBox );
   Scale_Sizer.AddStretch();

   //

   const char* generateNormalizedImagesToolTip = "<p>This parameter defines when to apply the local normalization functions to "
      "generate normalized images. This includes newly created image files for global execution, as well as modifying the target "
      "image for view execution.</p>"
      "<p>This option is set to <i>view execution only</i> by default because local normalization functions, stored in .xnml files, "
      "can be used by the ImageIntegration and DrizzleIntegration processes for normalization in the pixel rejection and/or "
      "integration output tasks. This means that the normalization functions can be applied internally by these processes, so "
      "writing normalized images to disk files is generally not necessary. In addition, the ImageIntegration and DrizzleIntegration "
      "processes apply normalization functions internally without any truncation or rescaling of the data, so the entire data set "
      "is always integrated without any loss or artificial alteration when local normalization files are used.</p>"
      "<p>For view execution, however, inspecting the transformed image is often necessary for testing purposes. Generation of "
      "normalized image files during batch execution can also be useful in very difficult cases, for example when the data set "
      "includes strong and large artifacts, such as big plane trails. In these cases you may want to inspect locally normalized "
      "images manually with analysis tools such as Blink. Normalized images may also be necessary for applications of local "
      "normalization different from integration of astronomical images.</p>";

   GenerateNormalizedImages_Label.SetText( "Apply normalization:" );
   GenerateNormalizedImages_Label.SetFixedWidth( labelWidth1 );
   GenerateNormalizedImages_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GenerateNormalizedImages_Label.SetToolTip( generateNormalizedImagesToolTip );

   GenerateNormalizedImages_ComboBox.AddItem( "Disabled" );
   GenerateNormalizedImages_ComboBox.AddItem( "Always" );
   GenerateNormalizedImages_ComboBox.AddItem( "View execution only" );
   GenerateNormalizedImages_ComboBox.AddItem( "Global execution only" );
   GenerateNormalizedImages_ComboBox.SetToolTip( generateNormalizedImagesToolTip );
   GenerateNormalizedImages_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalNormalizationInterface::e_ItemSelected, w );

   GenerateNormalizedImages_Sizer.SetSpacing( 4 );
   GenerateNormalizedImages_Sizer.Add( GenerateNormalizedImages_Label );
   GenerateNormalizedImages_Sizer.Add( GenerateNormalizedImages_ComboBox );
   GenerateNormalizedImages_Sizer.AddStretch();

   //

   NoScale_CheckBox.SetText( "No scale component" );
   NoScale_CheckBox.SetToolTip( "<p>Compute only the offset component of the local normalization function, and set the scale "
      "component constant equal to one.</p>"
      "<p>This option should be disabled under normal working conditions. It can be useful in special cases to limit local "
      "normalization to correction of additive gradients exclusively. However, without its multiplicative component the "
      "normalization function will be less accurate in general.</p>" );
   NoScale_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   NoScale_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   NoScale_Sizer.Add( NoScale_CheckBox );
   NoScale_Sizer.AddStretch();

   //

   GlobalLocationNormalization_CheckBox.SetText( "Global location normalization" );
   GlobalLocationNormalization_CheckBox.SetToolTip( "<p>Compute global location parameters to be applied after local normalization.</p>"
      "<p>For some applications it may be desirable matching the global statistical parameter of location (or central tendency) "
      "of the reference image after local normalization. In such cases this option can be enabled to compute the required global "
      "normalization parameters, which will be included in generated .xnml files and applied to output normalized images, as "
      "appropriate.</p>" );
   GlobalLocationNormalization_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   GlobalLocationNormalization_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GlobalLocationNormalization_Sizer.Add( GlobalLocationNormalization_CheckBox );
   GlobalLocationNormalization_Sizer.AddStretch();

   //

   GenerateNormalizationData_CheckBox.SetText( "Generate normalization data" );
   GenerateNormalizationData_CheckBox.SetToolTip( "<p>Generate XML normalization data files (XNML format, .xnml files) that can be "
      "used with the ImageIntegration and DrizzleIntegration processes. .xnml files store normalization parameters, image references "
      "and function matrices, among other parameters, that can be used by these processes to generate normalized pixel sample values "
      "required for pixel rejection and optimal integration output. For applications of local normalization to integration of "
      "astronomical images, this option should always be enabled.</p>" );
   GenerateNormalizationData_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   GenerateNormalizationData_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateNormalizationData_Sizer.Add( GenerateNormalizationData_CheckBox );
   GenerateNormalizationData_Sizer.AddStretch();

   //

   ShowBackgroundModels_CheckBox.SetText( "Show background models" );
   ShowBackgroundModels_CheckBox.SetToolTip( "<p>Generate floating point images with the computed local background models for the "
      "reference and target images. This option is only applicable to execution on views, and is always ignored when LocalNormalization "
      "is executed in the global context (that is, as a batch procedure).</p>"
      "<p>Analysis of local backgrounds can be important for testing purposes, especially in difficult cases, for example to know how "
      "complex gradients are being evaluated, or to judge the quality of outlier rejection.</p>" );
   ShowBackgroundModels_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   ShowBackgroundModels_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowBackgroundModels_Sizer.Add( ShowBackgroundModels_CheckBox );
   ShowBackgroundModels_Sizer.AddStretch();

   //

//    const char* plotNormalizationFunctionsToolTip = "<p>Generate graphical representations of the local normalization function matrices. "
//       "This option is only applicable to execution on views; it is always ignored when LocalNormalization is executed in the global context "
//       "(that is, as a batch procedure).</p>"
//       "<p>When this option is enabled, two images are generated for each image channel with graphs representing the parameters of the "
//       "two-dimensional surface linear normalization function: one for the scaling coefficients (or slopes), and another for the zero offsets "
//       "(or Y-intercepts). These graphs, besides being beautiful, can be outstandingly useful for evaluation and testing purposes.</p>"
//       "<p>The <i>lines 3D</i> mode is the fastest option, but also the less accurate because of potential artifacts caused by crossing grid "
//       "lines. The <i>palette 3D</i> and <i>map 3D</i> modes generate high-quality, smooth and accurate renditions, but require more computation "
//       "time. The default option is <i>map 3D</i>.</p>";
//
//    PlotNormalizationFunctions_Label.SetText( "Plot functions:" );
//    PlotNormalizationFunctions_Label.SetFixedWidth( labelWidth1 );
//    PlotNormalizationFunctions_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
//    PlotNormalizationFunctions_Label.SetToolTip( plotNormalizationFunctionsToolTip );
//
//    PlotNormalizationFunctions_ComboBox.AddItem( "Disabled" );
//    PlotNormalizationFunctions_ComboBox.AddItem( "Lines 3D" );
//    PlotNormalizationFunctions_ComboBox.AddItem( "Palette 3D" );
//    PlotNormalizationFunctions_ComboBox.AddItem( "Map 3D" );
//    PlotNormalizationFunctions_ComboBox.SetToolTip( plotNormalizationFunctionsToolTip );
//    PlotNormalizationFunctions_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalNormalizationInterface::e_ItemSelected, w );
//
//    PlotNormalizationFunctions_Sizer.SetSpacing( 4 );
//    PlotNormalizationFunctions_Sizer.Add( PlotNormalizationFunctions_Label );
//    PlotNormalizationFunctions_Sizer.Add( PlotNormalizationFunctions_ComboBox );
//    PlotNormalizationFunctions_Sizer.AddStretch();

   //

   GeneralParameters_Sizer.SetSpacing( 4 );
   GeneralParameters_Sizer.Add( ReferenceImage_Sizer );
   GeneralParameters_Sizer.Add( Scale_Sizer );
   GeneralParameters_Sizer.Add( GenerateNormalizedImages_Sizer );
   GeneralParameters_Sizer.Add( NoScale_Sizer );
   GeneralParameters_Sizer.Add( GlobalLocationNormalization_Sizer );
   GeneralParameters_Sizer.Add( GenerateNormalizationData_Sizer );
   GeneralParameters_Sizer.Add( ShowBackgroundModels_Sizer );
//    GeneralParameters_Sizer.Add( PlotNormalizationFunctions_Sizer );

   //

   OutlierRejection_SectionBar.SetTitle( "Outlier Rejection" );
   OutlierRejection_SectionBar.SetSection( OutlierRejection_Control );
   OutlierRejection_SectionBar.SetToolTip( "<p>The implemented local normalization algorithms are sensitive to differences between the "
      "reference and target images caused by spurious image features, such as hot pixels, plane and satellite trails, cosmic rays, etc.</p>"
      "<p>Under normal working conditions, this option should be enabled to detect and reject outlier image structures prior to "
      "calculation of local normalization functions.</p>" );
   OutlierRejection_SectionBar.EnableTitleCheckBox();
   OutlierRejection_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&LocalNormalizationInterface::e_ToggleSection, w );
   OutlierRejection_SectionBar.OnCheck( (SectionBar::check_event_handler)&LocalNormalizationInterface::e_CheckSection, w );

   //

   const char* hotPixelFilterRadiusToolTip =
   "<p>Size of the hot pixel removal filter.</p>"
   "<p>This is the radius in pixels of a median filter applied for robust outlier rejection. A median filter is very efficient "
   "to remove hot pixels, cold pixels, and similar small-scale outlier image structures that can contaminate the local "
   "normalization functions, degrading their accuracy, or even introducing artifacts in extreme cases.</p>"
   "<p>To disable hot pixel removal (not recommended), set this parameter to zero. The default value is two pixels.</p>";

   HotPixelFilterRadius_Label.SetText( "Hot pixel removal:" );
   HotPixelFilterRadius_Label.SetFixedWidth( labelWidth1 );
   HotPixelFilterRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   HotPixelFilterRadius_Label.SetToolTip( hotPixelFilterRadiusToolTip );

   HotPixelFilterRadius_SpinBox.SetRange( int( TheLNHotPixelFilterRadiusParameter->MinimumValue() ), int( TheLNHotPixelFilterRadiusParameter->MaximumValue() ) );
   HotPixelFilterRadius_SpinBox.SetToolTip( hotPixelFilterRadiusToolTip );
   HotPixelFilterRadius_SpinBox.SetFixedWidth( editWidth1 );
   HotPixelFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::e_SpinValueUpdated, w );

   HotPixelFilterRadius_Sizer.SetSpacing( 4 );
   HotPixelFilterRadius_Sizer.Add( HotPixelFilterRadius_Label );
   HotPixelFilterRadius_Sizer.Add( HotPixelFilterRadius_SpinBox );
   HotPixelFilterRadius_Sizer.AddStretch();

   //

   const char* noiseReductionFilterRadiusToolTip =
   "<p>Size of the noise reduction filter.</p>"
   "<p>This is the radius in pixels of a Gaussian convolution filter applied for noise reduction. Noise reduction is disabled "
   "by default; it is only necessary for images suffering from severe noise, especially images with clipped histograms and similar "
   "artifacts. Increasing this parameter should not be necessary for well calibrated images.</p>";

   NoiseReductionFilterRadius_Label.SetText( "Noise reduction:" );
   NoiseReductionFilterRadius_Label.SetFixedWidth( labelWidth1 );
   NoiseReductionFilterRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   NoiseReductionFilterRadius_Label.SetToolTip( noiseReductionFilterRadiusToolTip );

   NoiseReductionFilterRadius_SpinBox.SetRange( int( TheLNNoiseReductionFilterRadiusParameter->MinimumValue() ), int( TheLNNoiseReductionFilterRadiusParameter->MaximumValue() ) );
   NoiseReductionFilterRadius_SpinBox.SetStepSize( 5 );
   NoiseReductionFilterRadius_SpinBox.SetToolTip( noiseReductionFilterRadiusToolTip );
   NoiseReductionFilterRadius_SpinBox.SetFixedWidth( editWidth1 );
   NoiseReductionFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::e_SpinValueUpdated, w );

   NoiseReductionFilterRadius_Sizer.SetSpacing( 4 );
   NoiseReductionFilterRadius_Sizer.Add( NoiseReductionFilterRadius_Label );
   NoiseReductionFilterRadius_Sizer.Add( NoiseReductionFilterRadius_SpinBox );
   NoiseReductionFilterRadius_Sizer.AddStretch();

   //

   LowClippingLevel_NumericEdit.label.SetText( "Low clipping level:" );
   LowClippingLevel_NumericEdit.label.SetFixedWidth( labelWidth1 );
   LowClippingLevel_NumericEdit.SetReal();
   LowClippingLevel_NumericEdit.SetRange( TheLNLowClippingLevelParameter->MinimumValue(), TheLNLowClippingLevelParameter->MaximumValue() );
   LowClippingLevel_NumericEdit.SetPrecision( 2 /*TheLNLowClippingLevelParameter->Precision()*/ );
   LowClippingLevel_NumericEdit.EnableScientificNotation();
   LowClippingLevel_NumericEdit.sizer.AddStretch();
   LowClippingLevel_NumericEdit.SetToolTip( "<p>Low clipping pixel sample value in the [0,1] range.</p>"
      "<p>All pixels with values smaller than or equal to the value of this parameter will be replaced with statistically "
      "plausible estimates, computed from nearby image regions.</p>"
      "<p>Rejection of very low pixels is necessary to prevent generation of large-scale artifacts around large dark structures, "
      "such as black borders generated by image registration. Under normal conditions you shouldn't need to change the default "
      "value of this parameter (4.5&times;10<sup>-5</sup>). Increase it in the unlikely case that you detect dark artifacts "
      "around black borders&mdash;which, at any rate, will always denote a defective image calibration that you should fix "
      "before normalization.</p>" );
   LowClippingLevel_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&LocalNormalizationInterface::e_ValueUpdated, w );

   //

   HighClippingLevel_NumericControl.label.SetText( "High clipping level:" );
   HighClippingLevel_NumericControl.label.SetFixedWidth( labelWidth1 );
   HighClippingLevel_NumericControl.slider.SetRange( 0, 250 );
   HighClippingLevel_NumericControl.SetReal();
   HighClippingLevel_NumericControl.SetRange( TheLNHighClippingLevelParameter->MinimumValue(), TheLNHighClippingLevelParameter->MaximumValue() );
   HighClippingLevel_NumericControl.SetPrecision( TheLNHighClippingLevelParameter->Precision() );
   HighClippingLevel_NumericControl.edit.SetFixedWidth( editWidth1 );
   HighClippingLevel_NumericControl.SetToolTip( "<p>High clipping pixel sample value in the [0,1] range.</p>"
      "<p>All pixels with values greater than or equal to the value of this parameter will be replaced with statistically "
      "plausible estimates, computed from nearby image regions.</p>"
      "<p>Rejection of saturated pixels is necessary to prevent generation of large-scale artifacts around large saturated "
      "structures, such as bright stars, in local background models. Under normal conditions you shouldn't need to change the "
      "default value of this parameter (0.85). Decrease it in the unlikely case that you detect dark artifacts around large "
      "saturated objects.</p>" );
   HighClippingLevel_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalNormalizationInterface::e_ValueUpdated, w );

   //

   ReferenceRejectionThreshold_NumericControl.label.SetText( "Reference threshold:" );
   ReferenceRejectionThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   ReferenceRejectionThreshold_NumericControl.slider.SetRange( 0, 200 );
   ReferenceRejectionThreshold_NumericControl.SetReal();
   ReferenceRejectionThreshold_NumericControl.SetRange( TheLNReferenceRejectionThresholdParameter->MinimumValue(), TheLNReferenceRejectionThresholdParameter->MaximumValue() );
   ReferenceRejectionThreshold_NumericControl.SetPrecision( TheLNReferenceRejectionThresholdParameter->Precision() );
   ReferenceRejectionThreshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   ReferenceRejectionThreshold_NumericControl.SetToolTip( "<p>Reference image: maximum difference in units of the standard "
      "deviation of the noise over the local background for detection of outlier structures. Increasing this parameter will "
      "tend to relax outlier detection. The default value is 3 sigmas.</p>" );
   ReferenceRejectionThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalNormalizationInterface::e_ValueUpdated, w );

   //

   TargetRejectionThreshold_NumericControl.label.SetText( "Target threshold:" );
   TargetRejectionThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   TargetRejectionThreshold_NumericControl.slider.SetRange( 0, 200 );
   TargetRejectionThreshold_NumericControl.SetReal();
   TargetRejectionThreshold_NumericControl.SetRange( TheLNTargetRejectionThresholdParameter->MinimumValue(), TheLNTargetRejectionThresholdParameter->MaximumValue() );
   TargetRejectionThreshold_NumericControl.SetPrecision( TheLNTargetRejectionThresholdParameter->Precision() );
   TargetRejectionThreshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   TargetRejectionThreshold_NumericControl.SetToolTip( "<p>Target image: minimum difference in units of the standard "
      "deviation of the noise over the local background for detection of outlier structures. Increasing this parameter will "
      "tend to relax outlier detection. The default value is 3.2 sigmas.</p>" );
   TargetRejectionThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalNormalizationInterface::e_ValueUpdated, w );

   //

   ReferenceRejection_CheckBox.SetText( "Reference rejection" );
   ReferenceRejection_CheckBox.SetToolTip( "<p>Perform outlier rejection for the normalization reference image.</p>"
      "<p>If this option is enabled, the target image (either the target view or the first target file available) will be used "
      "to perform rejection of outlier pixels on the reference image. This is only necessary when the reference image can "
      "contain relatively large outlier structures, such as big plane trails.</p>"
      "<p>This option should normally be disabled when the normalization reference image is of good quality and does not have "
      "significant outliers.</p>" );
   ReferenceRejection_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   ReferenceRejection_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ReferenceRejection_Sizer.Add( ReferenceRejection_CheckBox );
   ReferenceRejection_Sizer.AddStretch();

   //

   ShowRejectionMaps_CheckBox.SetText( "Show rejection maps" );
   ShowRejectionMaps_CheckBox.SetToolTip( "<p>Generate unsigned 8-bit images where nonzero pixel samples correspond to rejected "
      "samples in the reference and target images. This option is only applicable to execution on views, and is always ignored when "
      "LocalNormalization is executed in the global context (that is, as a batch procedure).</p>"
      "<p>Rejection maps are useful for testing purposes, especially in very difficult cases where the images have large outliers "
      "such as plane trails.</p>" );
   ShowRejectionMaps_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   ShowRejectionMaps_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowRejectionMaps_Sizer.Add( ShowRejectionMaps_CheckBox );
   ShowRejectionMaps_Sizer.AddStretch();

   //

   OutlierRejection_Sizer.SetSpacing( 4 );
   OutlierRejection_Sizer.Add( HotPixelFilterRadius_Sizer );
   OutlierRejection_Sizer.Add( NoiseReductionFilterRadius_Sizer );
   OutlierRejection_Sizer.Add( LowClippingLevel_NumericEdit );
   OutlierRejection_Sizer.Add( HighClippingLevel_NumericControl );
   OutlierRejection_Sizer.Add( ReferenceRejectionThreshold_NumericControl );
   OutlierRejection_Sizer.Add( TargetRejectionThreshold_NumericControl );
   OutlierRejection_Sizer.Add( ReferenceRejection_Sizer );
   OutlierRejection_Sizer.Add( ShowRejectionMaps_Sizer );

   OutlierRejection_Control.SetSizer( OutlierRejection_Sizer );

   //

   ScaleEvaluation_SectionBar.SetTitle( "Scale Evaluation" );
   ScaleEvaluation_SectionBar.SetToolTip( "<p>The multiplicative component of the local normalization function requires an "
      "estimate of the global relative scale of the reference image with respect to each target image. The relative scale is "
      "defined as the ratio of fluxes measured on significant image structures. We have implemented two different methods "
      "for scale evaluation: PSF flux and multiscale analysis. This section includes all relevant parameters available "
      "to control the application of these algorithms.</p>" );
   ScaleEvaluation_SectionBar.SetSection( ScaleEvaluation_Control );
   ScaleEvaluation_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&LocalNormalizationInterface::e_ToggleSection, w );

   const char* scaleEvaluationMethodToolTip = "<p>In this version of the LocalNormalization process we have implemented "
      "two different methods for evaluation of the global relative scale of the reference image with respect to each "
      "normalization target image: PSF flux evaluation and multiscale analysis.</p>"
      "<p>The <b>PSF flux evaluation</b> method detects stars in the images and fits a point spread function model to each detected "
      "source. The fitted PSF parameters are then used to guide evaluation of the total flux of each source, and the fluxes of matched "
      "pairs of stars in both images are used to compute a robust and precise scale factor. This method is usually the best choice for "
      "normalization of deep-sky astronomical images where stars can be detected efficiently.</p>"
      "<p>The <b>multiscale analysis</b> method uses wavelet transforms and morphological operations to isolate significant image "
      "structures. The pixels gathered on the intersection between significant structures of the reference and target images are then "
      "evaluated statistically to estimate the scale factor.</p>";

   ScaleEvaluationMethod_Label.SetText( "Method:" );
   ScaleEvaluationMethod_Label.SetFixedWidth( labelWidth1 );
   ScaleEvaluationMethod_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ScaleEvaluationMethod_Label.SetToolTip( scaleEvaluationMethodToolTip );

   ScaleEvaluationMethod_ComboBox.AddItem( "PSF flux evaluation" );
   ScaleEvaluationMethod_ComboBox.AddItem( "Multiscale analysis" );
   ScaleEvaluationMethod_ComboBox.SetToolTip( scaleEvaluationMethodToolTip );
   ScaleEvaluationMethod_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalNormalizationInterface::e_ItemSelected, w );

   ScaleEvaluationMethod_Sizer.SetSpacing( 4 );
   ScaleEvaluationMethod_Sizer.Add( ScaleEvaluationMethod_Label );
   ScaleEvaluationMethod_Sizer.Add( ScaleEvaluationMethod_ComboBox );
   ScaleEvaluationMethod_Sizer.AddStretch();

   LocalScaleCorrections_CheckBox.SetText( "Local scale corrections" );
   LocalScaleCorrections_CheckBox.SetToolTip( "<p>Compute first order local scale corrections.</p>"
      "<p>If this option is enabled, LocalNormalization will model local scale variations, i.e. multiplicative gradients "
      "that cannot be corrected with a single global scale factor.</p>"
      "<p><b>Warning: The implementation of this feature is still experimental.</b></p>" );
   LocalScaleCorrections_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   LocalScaleCorrections_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   LocalScaleCorrections_Sizer.Add( LocalScaleCorrections_CheckBox );
   LocalScaleCorrections_Sizer.AddStretch();

   ShowLocalScaleModels_CheckBox.SetText( "Show local scale correction models" );
   ShowLocalScaleModels_CheckBox.SetToolTip( "<p>Generate floating point images with the computed local scale correction models for the "
      "normalization function. These model images are rescaled linearly to the [0,1] range.</p>"
      "<p>This option is only applicable to execution on views, and is always ignored when LocalNormalization is executed "
      "in the global context (that is, as a batch procedure).</p>" );
   ShowLocalScaleModels_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   ShowLocalScaleModels_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowLocalScaleModels_Sizer.Add( ShowLocalScaleModels_CheckBox );
   ShowLocalScaleModels_Sizer.AddStretch();

   const char* scaleStructureLayersToolTip =
   "<p>Number of wavelet layers used for structure detection.</p>"
   "<p>With more wavelet layers, larger stars and significant structures will be detected and used for relative scale evaluation.</p>";

   StructureLayers_Label.SetText( "Detection scales:" );
   StructureLayers_Label.SetFixedWidth( labelWidth1 );
   StructureLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StructureLayers_Label.SetToolTip( scaleStructureLayersToolTip );

   StructureLayers_SpinBox.SetRange( int( TheLNStructureLayersParameter->MinimumValue() ), int( TheLNStructureLayersParameter->MaximumValue() ) );
   StructureLayers_SpinBox.SetToolTip( scaleStructureLayersToolTip );
   StructureLayers_SpinBox.SetFixedWidth( editWidth1 );
   StructureLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::e_SpinValueUpdated, w );

   StructureLayers_Sizer.SetSpacing( 4 );
   StructureLayers_Sizer.Add( StructureLayers_Label );
   StructureLayers_Sizer.Add( StructureLayers_SpinBox );
   StructureLayers_Sizer.AddStretch();

   SaturationThreshold_NumericControl.label.SetText( "Saturation threshold:" );
   SaturationThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   SaturationThreshold_NumericControl.slider.SetRange( 0, 250 );
   SaturationThreshold_NumericControl.SetReal();
   SaturationThreshold_NumericControl.SetRange( TheLNSaturationThresholdParameter->MinimumValue(), TheLNSaturationThresholdParameter->MaximumValue() );
   SaturationThreshold_NumericControl.SetPrecision( TheLNSaturationThresholdParameter->Precision() );
   SaturationThreshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   SaturationThreshold_NumericControl.SetToolTip( "<p>Saturation threshold in the [0,1] range.</p>"
      "<p>Detected stars with one or more pixels with values above this threshold will be excluded for relative scale evaluation. "
      "Similarly, when the multiscale analysis method of scale evaluation is selected, significant structure pixels above this "
      "value will also be excluded.</p>"
      "<p>This parameter is expressed in the [0,1] range. It can be applied either as an absolute pixel sample value in the "
      "normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image (see the <i>Relative "
      "saturation threshold</i> parameter).</p>"
      "<p>The default saturation threshold is 0.75. For relative scale evaluation this parameter is crucial in order to prevent "
      "contamination of the statistical sample of flux ratios by sources with saturated or nonlinear data. Changing the default "
      "value should not be necessary under normal conditions.</p>" );
   SaturationThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalNormalizationInterface::e_ValueUpdated, w );

   SaturationRelative_CheckBox.SetText( "Relative saturation threshold" );
   SaturationRelative_CheckBox.SetToolTip( "<p>The saturation threshold parameter can be applied either as an absolute pixel "
      "sample value in the normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image.</p>"
      "The relative saturation threshold option is enabled by default.</p>" );
   SaturationRelative_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   SaturationRelative_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SaturationRelative_Sizer.Add( SaturationRelative_CheckBox );
   SaturationRelative_Sizer.AddStretch();

   RejectionLimit_NumericControl.label.SetText( "Rejection limit:" );
   RejectionLimit_NumericControl.label.SetFixedWidth( labelWidth1 );
   RejectionLimit_NumericControl.slider.SetRange( 0, 250 );
   RejectionLimit_NumericControl.SetReal();
   RejectionLimit_NumericControl.SetRange( TheLNRejectionLimitParameter->MinimumValue(), TheLNRejectionLimitParameter->MaximumValue() );
   RejectionLimit_NumericControl.SetPrecision( TheLNRejectionLimitParameter->Precision() );
   RejectionLimit_NumericControl.edit.SetFixedWidth( editWidth1 );
   RejectionLimit_NumericControl.SetToolTip( "<p>Limit for the modified Chauvenet rejection criterion.</p>"
      "<p>A modified Robust Chauvenet Rejection (RCR) routine is used internally by this implementation for rejection of "
      "outlier relative scale samples. The larger the value of this parameter, the more samples will be rejected by the RCR "
      "algorithm.</p>"
      "<p>The original Chauvenet rejection criterion is N*P(x > |z|) &lt; 0.5, where N is the number of measurements and "
      "P() represents the probability of x being more than z standard deviations from the mean. This parameter modifies the "
      "rejection criterion by replacing 0.5 with an arbitrary limit in the [0,1] range, in order to make the algorithm "
      "controllable. The default rejection limit is 0.3.</p>" );
   RejectionLimit_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalNormalizationInterface::e_ValueUpdated, w );

   const char* psfNoiseLayersToolTip =
      "<p>Star detector: Number of wavelet layers used for noise reduction.</p>"
      "<p>Noise reduction prevents detection of bright noise structures as false stars, including hot pixels and "
      "cosmic rays. This parameter can also be used to control the sizes of the smallest detected stars (increase "
      "to exclude more stars), although the <i>minimum structure size</i> parameter can be more efficient for this purpose.</p>";

   PSFNoiseLayers_Label.SetText( "Noise scales:" );
   PSFNoiseLayers_Label.SetFixedWidth( labelWidth1 );
   PSFNoiseLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PSFNoiseLayers_Label.SetToolTip( psfNoiseLayersToolTip );

   PSFNoiseLayers_SpinBox.SetRange( int( TheLNPSFNoiseLayersParameter->MinimumValue() ), int( TheLNPSFNoiseLayersParameter->MaximumValue() ) );
   PSFNoiseLayers_SpinBox.SetToolTip( psfNoiseLayersToolTip );
   PSFNoiseLayers_SpinBox.SetFixedWidth( editWidth1 );
   PSFNoiseLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::e_SpinValueUpdated, w );

   PSFNoiseLayers_Sizer.SetSpacing( 4 );
   PSFNoiseLayers_Sizer.Add( PSFNoiseLayers_Label );
   PSFNoiseLayers_Sizer.Add( PSFNoiseLayers_SpinBox );
   PSFNoiseLayers_Sizer.AddStretch();

   const char* psfMinStructureSizeToolTip =
      "<p>Minimum size of a detectable star structure in square pixels.</p>"
      "<p>This parameter can be used to prevent the detection of small and bright image artifacts wrongly as stars, "
      "when such artifacts cannot be removed with a median filter (i.e., the <i>Hot pixel removal</i> parameter), "
      "or for rejection of cosmic rays.</p>"
      "<p>This parameter can be used in three ways:</p>"
      "<p><b>* Automatic mode.</b> A zero value enables an adaptive algorithm to find an optimal minimum structure "
      "size using statistical analysis techniques. This is the default option.</p>"
      "<p><b>* Disabled.</b> A value of one turns off minimum structure size rejection since no detectable star can "
      "be represented by less than one pixel.</p>"
      "<p><b>* Literal value.</b> A value &gt; 1 forces using the specified minimum structure size in square pixels.</p>";

   PSFMinStructureSize_Label.SetText( "Minimum structure size:" );
   PSFMinStructureSize_Label.SetFixedWidth( labelWidth1 );
   PSFMinStructureSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PSFMinStructureSize_Label.SetToolTip( psfMinStructureSizeToolTip );

   PSFMinStructureSize_SpinBox.SetRange( int( TheLNPSFMinStructureSizeParameter->MinimumValue() ), int( TheLNPSFMinStructureSizeParameter->MaximumValue() ) );
   PSFMinStructureSize_SpinBox.SetMinimumValueText( "<Auto>" );
   PSFMinStructureSize_SpinBox.SetToolTip( psfMinStructureSizeToolTip );
   //PSFMinStructureSize_SpinBox.SetFixedWidth( editWidth1 );
   PSFMinStructureSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::e_SpinValueUpdated, w );

   PSFMinStructureSize_Sizer.SetSpacing( 4 );
   PSFMinStructureSize_Sizer.Add( PSFMinStructureSize_Label );
   PSFMinStructureSize_Sizer.Add( PSFMinStructureSize_SpinBox );
   PSFMinStructureSize_Sizer.AddStretch();

   const char* psfHotPixelFilterRadiusToolTip =
      "<p>Star detector: Size of the hot pixel removal filter.</p>"
      "<p>This is the radius in pixels of a median filter applied by the star detector before the structure "
      "detection phase. A median filter is very efficient to remove <i>hot pixels</i>. Hot pixels will be "
      "identified as false stars, and if present in large amounts, can prevent a valid signal evaluation.</p>"
      "<p>To disable hot pixel removal, set this parameter to zero.</p>";

   PSFHotPixelFilterRadius_Label.SetText( "Hot pixel removal:" );
   PSFHotPixelFilterRadius_Label.SetFixedWidth( labelWidth1 );
   PSFHotPixelFilterRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PSFHotPixelFilterRadius_Label.SetToolTip( psfHotPixelFilterRadiusToolTip );

   PSFHotPixelFilterRadius_SpinBox.SetRange( int( TheLNPSFHotPixelFilterRadiusParameter->MinimumValue() ), int( TheLNPSFHotPixelFilterRadiusParameter->MaximumValue() ) );
   PSFHotPixelFilterRadius_SpinBox.SetToolTip( psfHotPixelFilterRadiusToolTip );
   PSFHotPixelFilterRadius_SpinBox.SetFixedWidth( editWidth1 );
   PSFHotPixelFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::e_SpinValueUpdated, w );

   PSFHotPixelFilterRadius_Sizer.SetSpacing( 4 );
   PSFHotPixelFilterRadius_Sizer.Add( PSFHotPixelFilterRadius_Label );
   PSFHotPixelFilterRadius_Sizer.Add( PSFHotPixelFilterRadius_SpinBox );
   PSFHotPixelFilterRadius_Sizer.AddStretch();

   const char* psfNoiseReductionFilterRadiusToolTip =
      "<p>Star detector: Size of the noise reduction filter.</p>"
      "<p>This is the radius in pixels of a Gaussian convolution filter applied to the working image used for star "
      "detection. Use it only for very low SNR images, where the star detector cannot find reliable stars with its "
      "default parameters.</p>"
      "<p>To disable noise reduction, set this parameter to zero.</p>";

   PSFNoiseReductionFilterRadius_Label.SetText( "Noise reduction:" );
   PSFNoiseReductionFilterRadius_Label.SetFixedWidth( labelWidth1 );
   PSFNoiseReductionFilterRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PSFNoiseReductionFilterRadius_Label.SetToolTip( psfNoiseReductionFilterRadiusToolTip );

   PSFNoiseReductionFilterRadius_SpinBox.SetRange( int( TheLNPSFNoiseReductionFilterRadiusParameter->MinimumValue() ), int( TheLNPSFNoiseReductionFilterRadiusParameter->MaximumValue() ) );
   PSFNoiseReductionFilterRadius_SpinBox.SetToolTip( psfNoiseReductionFilterRadiusToolTip );
   PSFNoiseReductionFilterRadius_SpinBox.SetFixedWidth( editWidth1 );
   PSFNoiseReductionFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::e_SpinValueUpdated, w );

   PSFNoiseReductionFilterRadius_Sizer.SetSpacing( 4 );
   PSFNoiseReductionFilterRadius_Sizer.Add( PSFNoiseReductionFilterRadius_Label );
   PSFNoiseReductionFilterRadius_Sizer.Add( PSFNoiseReductionFilterRadius_SpinBox );
   PSFNoiseReductionFilterRadius_Sizer.AddStretch();

   PSFMinSNR_NumericEdit.label.SetText( "Minimum detection SNR:" );
   PSFMinSNR_NumericEdit.label.SetFixedWidth( labelWidth1 );
   PSFMinSNR_NumericEdit.SetReal();
   PSFMinSNR_NumericEdit.SetRange( TheLNPSFMinSNRParameter->MinimumValue(), TheLNPSFMinSNRParameter->MaximumValue() );
   PSFMinSNR_NumericEdit.SetPrecision( TheLNPSFMinSNRParameter->Precision() );
   PSFMinSNR_NumericEdit.edit.SetFixedWidth( editWidth1 );
   PSFMinSNR_NumericEdit.sizer.AddStretch();
   PSFMinSNR_NumericEdit.SetToolTip( "<p>Minimum signal-to-noise ratio of a detectable star.</p>"
      "<p>Given a source with estimated brightness <i>s</i>, local background <i>b</i> and local background dispersion "
      "<i>n</i>, SNR is evaluated as (<i>s</i> &ndash; <i>b</i>)/<i>n</i>. Stars with measured SNR below the value of this "
      "parameter won't be used for relative scale evaluation.</p>"
      "<p>This parameter allows limiting star detection to a subset of the brightest sources in the image adaptively. By "
      "requiring relatively high SNR levels in the evaluated sources, the accuracy and robustness of the scale evaluation "
      "process can be largely improved. The default value is 40, which is quite appropriate in most cases.</p>" );
   PSFMinSNR_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&LocalNormalizationInterface::e_ValueUpdated, w );

   const char* psfTypeToolTip = "<p>Point spread function type used for PSF fitting and photometry.</p>"
      "<p>In all cases elliptical functions are fitted to detected star structures, and PSF sampling regions are "
      "defined adaptively using a median stabilization algorithm.</p>"
      "<p>When the <b>Auto</b> option is selected, a series of different PSFs will be fitted for each source, and "
      "the fit that leads to the least absolute difference among function values and sampled pixel values will be "
      "used for scale estimation. Currently the following functions are tested in this special automatic mode: "
      "Moffat functions with <i>beta</i> shape parameters equal to 2.5, 4, 6 and 10.</p>"
      "<p>The rest of options select a fixed PSF type for all detected sources, which improves execution times at "
      "the cost of a less adaptive, and hence potentially less accurate, relative scale measurement process.</p>";

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
   PSFType_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalNormalizationInterface::e_ItemSelected, w );

   PSFType_Sizer.SetSpacing( 4 );
   PSFType_Sizer.Add( PSFType_Label );
   PSFType_Sizer.Add( PSFType_ComboBox );
   PSFType_Sizer.AddStretch();

   PSFGrowth_NumericControl.label.SetText( "Growth factor:" );
   PSFGrowth_NumericControl.label.SetFixedWidth( labelWidth1 );
   PSFGrowth_NumericControl.slider.SetRange( 0, 250 );
   PSFGrowth_NumericControl.SetReal();
   PSFGrowth_NumericControl.SetRange( TheLNPSFGrowthParameter->MinimumValue(), TheLNPSFGrowthParameter->MaximumValue() );
   PSFGrowth_NumericControl.SetPrecision( TheLNPSFGrowthParameter->Precision() );
   PSFGrowth_NumericControl.edit.SetFixedWidth( editWidth1 );
   PSFGrowth_NumericControl.SetToolTip( "<p>Growing factor for expansion/contraction of the PSF flux measurement region for "
      "each source, in units of the Full Width at Tenth Maximum (FWTM).</p>"
      "<p>The default value of this parameter is 1.0, meaning that flux is measured exclusively for pixels within the elliptical "
      "region defined at one tenth of the fitted PSF maximum. Increasing this parameter can inprove accuracy of PSF flux "
      "measurements for undersampled images, where PSF fitting uncertainty is relatively large. Decreasing it can be beneficial "
      "in some cases working with very noisy data to restrict flux evaluation to star cores.</p>" );
   PSFGrowth_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalNormalizationInterface::e_ValueUpdated, w );

   const char* psfMaxStarsToolTip =
      "<p>The maximum number of stars that can be measured to compute mean scale estimates.</p>"
      "<p>PSF photometry will be performed for no more than the specified number of stars. The subset of measured stars "
      "will always start at the beginning of the set of detected stars, sorted by brightness in descending order.</p>"
      "<p>The default value imposes a generous limit of 24K stars. Limiting the number of photometric samples can improve "
      "performance for normalization of wide-field frames, where the number of detected stars can be very large. However, "
      "reducing the set of measured sources too much will damage the accuracy of scale evaluation.</p>";

   PSFMaxStars_Label.SetText( "Maximum stars:" );
   PSFMaxStars_Label.SetFixedWidth( labelWidth1 );
   PSFMaxStars_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PSFMaxStars_Label.SetToolTip( psfMaxStarsToolTip );

   PSFMaxStars_SpinBox.SetRange( int( TheLNPSFMaxStarsParameter->MinimumValue() ), int( TheLNPSFMaxStarsParameter->MaximumValue() ) );
   PSFMaxStars_SpinBox.SetToolTip( psfMaxStarsToolTip );
   PSFMaxStars_SpinBox.SetFixedWidth( editWidth3 );
   PSFMaxStars_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::e_SpinValueUpdated, w );

   PSFMaxStars_Sizer.SetSpacing( 4 );
   PSFMaxStars_Sizer.Add( PSFMaxStars_Label );
   PSFMaxStars_Sizer.Add( PSFMaxStars_SpinBox );
   PSFMaxStars_Sizer.AddStretch();

   PSFAllowClusteredSources_CheckBox.SetText( "Allow clustered sources" );
   PSFAllowClusteredSources_CheckBox.SetToolTip( "<p>If this parameter is disabled, multiple sources that are too close to be "
      "separated as individual structures, such as double and multiple stars, won't be detected for relative scale evaluation.</p>"
      "<p>If this parameter is enabled, non-separable multiple sources will be freely detectable as single objects.</p>"
      "<p>In general, sources with several local maxima pose difficulties for the determination of accurate star positions and PSF "
      "parameters, although this is usually a minor problem for scale evaluation. For this reason this parameter is enabled by "
      "default.</p>" );
   PSFAllowClusteredSources_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   PSFAllowClusteredSources_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PSFAllowClusteredSources_Sizer.Add( PSFAllowClusteredSources_CheckBox );
   PSFAllowClusteredSources_Sizer.AddStretch();

   PSFScaleEvaluation_Sizer.SetSpacing( 4 );
   PSFScaleEvaluation_Sizer.Add( PSFNoiseLayers_Sizer );
   PSFScaleEvaluation_Sizer.Add( PSFMinStructureSize_Sizer );
   PSFScaleEvaluation_Sizer.Add( PSFHotPixelFilterRadius_Sizer );
   PSFScaleEvaluation_Sizer.Add( PSFNoiseReductionFilterRadius_Sizer );
   PSFScaleEvaluation_Sizer.Add( PSFMinSNR_NumericEdit );
   PSFScaleEvaluation_Sizer.Add( PSFType_Sizer );
   PSFScaleEvaluation_Sizer.Add( PSFGrowth_NumericControl );
   PSFScaleEvaluation_Sizer.Add( PSFMaxStars_Sizer );
   PSFScaleEvaluation_Sizer.Add( PSFAllowClusteredSources_Sizer );

   PSFScaleEvaluation_Control.SetSizer( PSFScaleEvaluation_Sizer );

   ShowStructureMaps_CheckBox.SetText( "Show structure maps" );
   ShowStructureMaps_CheckBox.SetToolTip( "<p>Generate unsigned 8-bit images where nonzero pixel samples belong to detected significant "
      "structures in the reference and target images. This option is only available for the multiscale analysis scale evaluation method, "
      "and is only applicable to execution on views. This option is always ignored when LocalNormalization is executed in the global "
      "context (that is, as a batch procedure).</p>" );
   ShowStructureMaps_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   ShowStructureMaps_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowStructureMaps_Sizer.Add( ShowStructureMaps_CheckBox );
   ShowStructureMaps_Sizer.AddStretch();

   ScaleEvaluation_Sizer.SetSpacing( 4 );
   ScaleEvaluation_Sizer.Add( ScaleEvaluationMethod_Sizer );
   ScaleEvaluation_Sizer.Add( LocalScaleCorrections_Sizer );
   ScaleEvaluation_Sizer.Add( ShowLocalScaleModels_Sizer );
   ScaleEvaluation_Sizer.Add( StructureLayers_Sizer );
   ScaleEvaluation_Sizer.Add( SaturationThreshold_NumericControl );
   ScaleEvaluation_Sizer.Add( SaturationRelative_Sizer );
   ScaleEvaluation_Sizer.Add( RejectionLimit_NumericControl );
   ScaleEvaluation_Sizer.Add( PSFScaleEvaluation_Control );
   ScaleEvaluation_Sizer.Add( ShowStructureMaps_Sizer );

   ScaleEvaluation_Control.SetSizer( ScaleEvaluation_Sizer );

   //

   TargetImages_SectionBar.SetTitle( "Target Images" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&LocalNormalizationInterface::e_ToggleSection, w );

   TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   TargetImages_TreeBox.SetScaledMinWidth( 344 );
   TargetImages_TreeBox.SetNumberOfColumns( 3 );
   TargetImages_TreeBox.HideHeader();
   TargetImages_TreeBox.EnableMultipleSelections();
   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&LocalNormalizationInterface::e_CurrentNodeUpdated, w );
   TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&LocalNormalizationInterface::e_NodeActivated, w );
   TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&LocalNormalizationInterface::e_NodeSelectionUpdated, w );
   TargetImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&LocalNormalizationInterface::e_FileDrag, w );
   TargetImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&LocalNormalizationInterface::e_FileDrop, w );
   TargetImages_TreeBox.Viewport().OnViewDrag( (Control::view_drag_event_handler)&LocalNormalizationInterface::e_ViewDrag, w );
   TargetImages_TreeBox.Viewport().OnViewDrop( (Control::view_drop_event_handler)&LocalNormalizationInterface::e_ViewDrop, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of target images.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all target images.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of target images.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected target images.</p>"
      "<p>Disabled target images will be ignored during the registration process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected target images.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of input images.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for target file items.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

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
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&LocalNormalizationInterface::e_ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings for image files used by "
      "specific processes. In LocalNormalization, input hints change the way input images of some particular file formats "
      "(reference and target image files) are loaded, while output hints modify the way output normalized image files are written.</p>"
      "<p>For example, you can use the \"lower-range\" and \"upper-range\" input hints to load floating point FITS and TIFF files "
      "generated by other applications that don't use PixInsight's normalized [0,1] range. Similarly, you can specify the \"up-bottom\" "
      "input hint to load input images from FITS files with the coordinate origin at the top-left corner. Most standard file format "
      "modules support hints; each format supports a number of input and/or output hints that you can use for different purposes with "
      "tools that give you access to format hints.</p>";

   InputHints_Label.SetText( "Input hints:" );
   InputHints_Label.SetFixedWidth( labelWidth1 );
   InputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InputHints_Label.SetToolTip( hintsToolTip );

   InputHints_Edit.SetToolTip( hintsToolTip );
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   OutputHints_Label.SetText( "Output hints:" );
   OutputHints_Label.SetFixedWidth( labelWidth1 );
   OutputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputHints_Label.SetToolTip( hintsToolTip );

   OutputHints_Edit.SetToolTip( hintsToolTip );
   OutputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );

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
   OutputFiles_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&LocalNormalizationInterface::e_ToggleSection, w );

   const char* outputDirectoryToolTip = "<p>This is the directory (or folder) where all output files will be written. "
      "This includes .xnml data files and, if the corresponding option has been enabled, normalized image files.</p>"
      "<p>If this field is left blank, output files will be written to the same directories as their  corresponding "
      "target files. In this case, make sure that source directories are writable, or the normalization process will fail.</p>";

   OutputDirectory_Label.SetText( "Output directory:" );
   OutputDirectory_Label.SetFixedWidth( labelWidth1 );
   OutputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputDirectory_Label.SetToolTip( outputDirectoryToolTip );

   OutputDirectory_Edit.SetToolTip( outputDirectoryToolTip );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)&LocalNormalizationInterface::e_FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)&LocalNormalizationInterface::e_FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.SetToolTip( "<p>Select the output directory</p>" );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   const char* outputPrefixToolTip = "<p>This is a prefix that will be prepended to the file name of each normalized image.</p>";

   OutputPrefix_Label.SetText( "Prefix:" );
   OutputPrefix_Label.SetFixedWidth( labelWidth1 );
   OutputPrefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPrefix_Label.SetToolTip( outputPrefixToolTip );

   OutputPrefix_Edit.SetFixedWidth( editWidth2 );
   OutputPrefix_Edit.SetToolTip( outputPrefixToolTip );
   OutputPrefix_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );

   const char* outputPostfixToolTip = "<p>This is a postfix that will be appended to the file name of each normalized image.</p>";

   OutputPostfix_Label.SetText( "Postfix:" );
   OutputPostfix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPostfix_Label.SetToolTip( outputPostfixToolTip );

   OutputPostfix_Edit.SetFixedWidth( editWidth2 );
   OutputPostfix_Edit.SetToolTip( outputPostfixToolTip );
   OutputPostfix_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );

   OutputChunks_Sizer.SetSpacing( 4 );
   OutputChunks_Sizer.Add( OutputPrefix_Label );
   OutputChunks_Sizer.Add( OutputPrefix_Edit );
   OutputChunks_Sizer.AddSpacing( 12 );
   OutputChunks_Sizer.Add( OutputPostfix_Label );
   OutputChunks_Sizer.Add( OutputPostfix_Edit );
   OutputChunks_Sizer.AddStretch();

   OverwriteExistingFiles_CheckBox.SetText( "Overwrite existing files" );
   OverwriteExistingFiles_CheckBox.SetToolTip( "<p>If this option is selected, LocalNormalization will overwrite "
      "existing files with the same names as generated output files. This can be dangerous because the original "
      "contents of overwritten files will be lost.</p>"
      "<p><b>Enable this option <u>at your own risk.</u></b></p>" );
   OverwriteExistingFiles_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   const char* onErrorToolTip = "<p>Specify what to do if there are errors during a batch local normalization process.</p>";

   OnError_Label.SetText( "On error:" );
   OnError_Label.SetToolTip( onErrorToolTip );
   OnError_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OnError_ComboBox.AddItem( "Continue" );
   OnError_ComboBox.AddItem( "Abort" );
   OnError_ComboBox.AddItem( "Ask User" );
   OnError_ComboBox.SetToolTip( onErrorToolTip );
   OnError_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalNormalizationInterface::e_ItemSelected, w );

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

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( GeneralParameters_Sizer );
   Global_Sizer.Add( OutlierRejection_SectionBar );
   Global_Sizer.Add( OutlierRejection_Control );
   Global_Sizer.Add( ScaleEvaluation_SectionBar );
   Global_Sizer.Add( ScaleEvaluation_Control );
   Global_Sizer.Add( TargetImages_SectionBar );
   Global_Sizer.Add( TargetImages_Control );
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add( OutputFiles_SectionBar );
   Global_Sizer.Add( OutputFiles_Control );

   //

   w.SetSizer( Global_Sizer );

   OutlierRejection_Control.Hide();
   ScaleEvaluation_Control.Hide();
   FormatHints_Control.Hide();

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedWidth();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalNormalizationInterface.cpp - Released 2024-12-11T17:43:17Z
