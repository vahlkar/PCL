//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.1
// ----------------------------------------------------------------------------
// SubframeSelectorInterface.cpp - Released 2024-12-17T18:15:44Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2021 Cameron Leger
// Copyright (c) 2020-2024 Juan Conejero, PTeam
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

#include "SubframeSelectorExpressionsInterface.h"
#include "SubframeSelectorInterface.h"
#include "SubframeSelectorMeasurementsInterface.h"
#include "SubframeSelectorProcess.h"

#include <pcl/Console.h>
#include <pcl/Dialog.h>
#include <pcl/DrizzleData.h>
#include <pcl/FileDataCachePreferencesDialog.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/IntegrationMetadata.h>
#include <pcl/LocalNormalizationData.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/PreviewSelectionDialog.h>
#include <pcl/RadioButton.h>
#include <pcl/StandardStatus.h>
#include <pcl/ViewList.h>

#define IMAGELIST_MINHEIGHT( fnt )  (12*fnt.Height() + 2)

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorInterface* TheSubframeSelectorInterface = nullptr;

// ----------------------------------------------------------------------------

SubframeSelectorInterface::SubframeSelectorInterface()
   : m_instance( TheSubframeSelectorProcess )
{
   TheSubframeSelectorInterface = this;

   if ( TheSubframeSelectorExpressionsInterface == nullptr )
      new SubframeSelectorExpressionsInterface( m_instance );

   if ( TheSubframeSelectorMeasurementsInterface == nullptr )
      new SubframeSelectorMeasurementsInterface( m_instance );

   /*
    * The auto save geometry feature is of no good to interfaces that include
    * both auto-expanding controls (e.g. TreeBox) and collapsible sections
    * (e.g. SectionBar).
    */
   DisableAutoSaveGeometry();
}

// ----------------------------------------------------------------------------

SubframeSelectorInterface::~SubframeSelectorInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString SubframeSelectorInterface::Id() const
{
   return "SubframeSelector";
}

// ----------------------------------------------------------------------------

MetaProcess* SubframeSelectorInterface::Process() const
{
   return TheSubframeSelectorProcess;
}

// ----------------------------------------------------------------------------

String SubframeSelectorInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/SubframeSelector.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures SubframeSelectorInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal | InterfaceFeature::PreferencesButton;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::EditPreferences()
{
   if ( TheSubframeSelectorCache == nullptr )
      new SubframeSelectorCache;

   bool persistent = TheSubframeSelectorCache->IsPersistent();
   FileDataCachePreferencesDialog dlg( *TheSubframeSelectorCache, persistent );
   if ( dlg.Execute() == StdDialogCode::Ok )
      TheSubframeSelectorCache->SetPersistent( persistent );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::ResetInstance()
{
   SubframeSelectorInstance defaultInstance( TheSubframeSelectorProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "SubframeSelector" );

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   /*
    * ### N.B. The order of initialization is critical here, since the three
    * interfaces have mutual dependencies. *Do not* alter the order in which
    * the following member functions are being called.
    */
   if ( m_expressionsWasVisible )
      ShowExpressionsInterface();
   if ( m_measurementsWasVisible )
      ShowMeasurementsInterface();
   UpdateControls();

   dynamic = false;
   return &P == TheSubframeSelectorProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* SubframeSelectorInterface::NewProcess() const
{
   return new SubframeSelectorInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const SubframeSelectorInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a SubframeSelector instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInterface::ImportProcess( const ProcessImplementation& p )
{

   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class SortSubframesDialog : public Dialog
{
public:

   SubframeSelectorInterface::SubframeSortingCriterion sortBy = SubframeSelectorInterface::SortByTimeAscending;

   SortSubframesDialog()
   {
      SortingCriterion_GroupBox.SetTitle( "Sorting Criterion" );

      SortByAcquisitionTime_RadioButton.SetText( "Acquisition time" );
      SortByFilePath_RadioButton.SetText( "File path" );
      SortAscending_CheckBox.SetText( "Ascending order" );

      SortByAcquisitionTime_RadioButton.SetChecked();
      SortAscending_CheckBox.SetChecked();

      SortingCriterion_Sizer.SetMargin( 6 );
      SortingCriterion_Sizer.SetSpacing( 6 );
      SortingCriterion_Sizer.Add( SortByAcquisitionTime_RadioButton );
      SortingCriterion_Sizer.Add( SortByFilePath_RadioButton );
      SortingCriterion_Sizer.AddSpacing( 6 );
      SortingCriterion_Sizer.Add( SortAscending_CheckBox );

      SortingCriterion_GroupBox.SetSizer( SortingCriterion_Sizer );
      SortingCriterion_GroupBox.SetMinWidth( Font().Width( String( '0', 40 ) ) );

      OK_PushButton.SetText( "OK" );
      OK_PushButton.SetDefault();
      OK_PushButton.SetCursor( StdCursor::Checkmark );
      OK_PushButton.OnClick( (Button::click_event_handler)&SortSubframesDialog::e_Click, *this );

      Cancel_PushButton.SetText( "Cancel" );
      Cancel_PushButton.SetCursor( StdCursor::Crossmark );
      Cancel_PushButton.OnClick( (Button::click_event_handler)&SortSubframesDialog::e_Click, *this );

      Buttons_Sizer.SetSpacing( 8 );
      Buttons_Sizer.AddStretch();
      Buttons_Sizer.Add( OK_PushButton );
      Buttons_Sizer.Add( Cancel_PushButton );

      Global_Sizer.SetMargin( 8 );
      Global_Sizer.SetSpacing( 6 );
      Global_Sizer.Add( SortingCriterion_GroupBox );
      Global_Sizer.AddSpacing( 6 );
      Global_Sizer.Add( Buttons_Sizer );

      SetSizer( Global_Sizer );

      EnsureLayoutUpdated();
      AdjustToContents();
      SetFixedSize();

      SetWindowTitle( "Sort Subframes" );
   }

private:

   VerticalSizer  Global_Sizer;
      GroupBox       SortingCriterion_GroupBox;
      VerticalSizer  SortingCriterion_Sizer;
         RadioButton SortByAcquisitionTime_RadioButton;
         RadioButton SortByFilePath_RadioButton;
         CheckBox    SortAscending_CheckBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void e_Click( Button& sender, bool checked )
   {
      if ( sender == OK_PushButton )
      {
         if ( SortByAcquisitionTime_RadioButton.IsChecked() )
            sortBy = SortAscending_CheckBox.IsChecked() ?
                        SubframeSelectorInterface::SortByTimeAscending :
                        SubframeSelectorInterface::SortByTimeDescending;
         else if ( SortByFilePath_RadioButton.IsChecked() )
            sortBy = SortAscending_CheckBox.IsChecked() ?
                        SubframeSelectorInterface::SortByPathAscending :
                        SubframeSelectorInterface::SortByPathDescending;
         Ok();
      }
      else if ( sender == Cancel_PushButton )
      {
         Cancel();
      }
   }
};

static SortSubframesDialog* s_sortSubframesDialog = nullptr;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SubframeSelectorInterface::ShowExpressionsInterface() const
{
   if ( !TheSubframeSelectorExpressionsInterface->IsVisible() )
      TheSubframeSelectorExpressionsInterface->Launch();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::HideExpressionsInterface() const
{
   if ( TheSubframeSelectorExpressionsInterface->IsVisible() )
      TheSubframeSelectorExpressionsInterface->Hide();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::ShowMeasurementsInterface() const
{
   if ( !TheSubframeSelectorMeasurementsInterface->IsVisible() )
      TheSubframeSelectorMeasurementsInterface->Launch();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::HideMeasurementsInterface() const
{
   if ( TheSubframeSelectorMeasurementsInterface->IsVisible() )
      TheSubframeSelectorMeasurementsInterface->Hide();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateControls()
{
   TheSubframeSelectorMeasurementsInterface->Cleanup();

   GUI->Routine_ComboBox.SetCurrentItem( m_instance.p_routine );
   GUI->SubframeImages_FileCache_CheckBox.SetChecked( m_instance.p_fileCache );
   UpdateSubframeImagesList();
   UpdateSubframeImageSelectionButtons();
   UpdateSystemParametersControls();
   UpdateStarDetectorControls();
   UpdatePedestalControls();
   UpdateFormatHintsControls();
   UpdateOutputFilesControls();
   TheSubframeSelectorExpressionsInterface->UpdateControls();
   TheSubframeSelectorMeasurementsInterface->UpdateControls();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateSubframeImageItem( size_type i )
{
   TreeBox::Node* node = GUI->SubframeImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const SubframeSelectorInstance::SubframeItem& item = m_instance.p_subframes[i];

   node->SetText( 0, String( i + 1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   String fileText;
   if ( !item.nmlPath.IsEmpty() )
      fileText << "<n> ";
   if ( !item.drzPath.IsEmpty() )
      fileText << "<d> ";
   if ( GUI->SubframeImages_FullPaths_CheckBox.IsChecked() )
      fileText << item.path;
   else
      fileText << File::ExtractNameAndSuffix( item.path );

   node->SetText( 2, fileText );
   node->SetAlignment( 2, TextAlign::Left );

   String toolTip = item.path;
   if ( !item.nmlPath.IsEmpty() )
      toolTip << '\n' << item.nmlPath;
   if ( !item.drzPath.IsEmpty() )
      toolTip << '\n' << item.drzPath;
   node->SetToolTip( 2, toolTip );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateSubframeImagesList()
{
   int currentIdx = GUI->SubframeImages_TreeBox.ChildIndex( GUI->SubframeImages_TreeBox.CurrentNode() );

   GUI->SubframeImages_TreeBox.DisableUpdates();
   GUI->SubframeImages_TreeBox.Clear();

   for ( size_type i = 0; i < m_instance.p_subframes.Length(); ++i )
   {
      new TreeBox::Node( GUI->SubframeImages_TreeBox );
      UpdateSubframeImageItem( i );
   }

   GUI->SubframeImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->SubframeImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->SubframeImages_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !m_instance.p_subframes.IsEmpty() )
      if ( currentIdx >= 0 )
         if ( currentIdx < GUI->SubframeImages_TreeBox.NumberOfChildren() )
            GUI->SubframeImages_TreeBox.SetCurrentNode( GUI->SubframeImages_TreeBox[currentIdx] );

   GUI->SubframeImages_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateSubframeImageSelectionButtons()
{
   bool hasItems = GUI->SubframeImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->SubframeImages_TreeBox.HasSelectedTopLevelNodes();

   GUI->SubframeImages_AddDrizzleFiles_PushButton.Enable( hasItems );
   GUI->SubframeImages_ClearDrizzleFiles_PushButton.Enable( hasItems );
   GUI->SubframeImages_AddLocalNormalizationFiles_PushButton.Enable( hasItems );
   GUI->SubframeImages_ClearLocalNormalizationFiles_PushButton.Enable( hasItems );
   GUI->SubframeImages_SelectAll_PushButton.Enable( hasItems );
   GUI->SubframeImages_Sort_PushButton.Enable( hasItems );
   GUI->SubframeImages_Invert_PushButton.Enable( hasItems );
   GUI->SubframeImages_Toggle_PushButton.Enable( hasSelection );
   GUI->SubframeImages_Remove_PushButton.Enable( hasSelection );
   GUI->SubframeImages_Clear_PushButton.Enable( hasItems );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateSystemParametersControls()
{
   GUI->SystemParameters_SubframeScale_Control.SetValue( m_instance.p_subframeScale );
   GUI->SystemParameters_CameraGain_Control.SetValue( m_instance.p_cameraGain );
   GUI->SystemParameters_CameraResolution_ComboBox.SetCurrentItem( m_instance.p_cameraResolution );
   GUI->SystemParameters_SiteLocalMidnight_SpinBox.SetValue( m_instance.p_siteLocalMidnight );
   GUI->SystemParameters_ScaleUnit_ComboBox.SetCurrentItem( m_instance.p_scaleUnit );
   GUI->SystemParameters_DataUnit_ComboBox.SetCurrentItem( m_instance.p_dataUnit );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateStarDetectorControls()
{
   GUI->StarDetectorParameters_StructureLayers_SpinBox.SetValue( m_instance.p_structureLayers );
   GUI->StarDetectorParameters_NoiseLayers_SpinBox.SetValue( m_instance.p_noiseLayers );
   GUI->StarDetectorParameters_MinStructureSize_SpinBox.SetValue( m_instance.p_minStructureSize );
   GUI->StarDetectorParameters_HotPixelFilterRadius_SpinBox.SetValue( m_instance.p_hotPixelFilterRadius );
   GUI->StarDetectorParameters_NoiseReductionFilterRadius_SpinBox.SetValue( m_instance.p_noiseReductionFilterRadius );
   GUI->StarDetectorParameters_Sensitivity_Control.SetValue( m_instance.p_sensitivity );
   GUI->StarDetectorParameters_PeakResponse_Control.SetValue( m_instance.p_peakResponse );
   GUI->StarDetectorParameters_BrightThreshold_Control.SetValue( m_instance.p_brightThreshold );
   GUI->StarDetectorParameters_MaxDistortion_Control.SetValue( m_instance.p_maxDistortion );
   GUI->StarDetectorParameters_UpperLimit_Control.SetValue( m_instance.p_upperLimit );
   GUI->StarDetectorParameters_AllowClusteredSources_CheckBox.SetChecked( m_instance.p_allowClusteredSources );
   GUI->StarDetectorParameters_PSFFit_ComboBox.SetCurrentItem( m_instance.p_psfFit );
   GUI->StarDetectorParameters_PSFFitCircular_CheckBox.SetChecked( m_instance.p_psfFitCircular );
   GUI->StarDetectorParameters_MaxPSFFits_SpinBox.SetValue( m_instance.p_maxPSFFits );

   GUI->RegionOfInterestX0_SpinBox.SetValue( m_instance.p_roi.x0 );
   GUI->RegionOfInterestY0_SpinBox.SetValue( m_instance.p_roi.y0 );
   GUI->RegionOfInterestWidth_SpinBox.SetValue( m_instance.p_roi.Width() );
   GUI->RegionOfInterestHeight_SpinBox.SetValue( m_instance.p_roi.Height() );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdatePedestalControls()
{
   GUI->PedestalMode_ComboBox.SetCurrentItem( m_instance.p_pedestalMode );
   GUI->PedestalValue_Label.Enable( m_instance.p_pedestalMode == SSPedestalMode::Literal );
   GUI->PedestalValue_SpinBox.SetValue( m_instance.p_pedestal );
   GUI->PedestalValue_SpinBox.Enable( m_instance.p_pedestalMode == SSPedestalMode::Literal );
   GUI->PedestalKeyword_Label.Enable( m_instance.p_pedestalMode == SSPedestalMode::CustomKeyword );
   GUI->PedestalKeyword_Edit.SetText( m_instance.p_pedestalKeyword );
   GUI->PedestalKeyword_Edit.Enable( m_instance.p_pedestalMode == SSPedestalMode::CustomKeyword );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateFormatHintsControls()
{
   GUI->InputHints_Edit.SetText( m_instance.p_inputHints );
   GUI->OutputHints_Edit.SetText( m_instance.p_outputHints );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateOutputFilesControls()
{
   GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory );
   GUI->OutputPrefix_Edit.SetText( m_instance.p_outputPrefix );
   GUI->OutputPostfix_Edit.SetText( m_instance.p_outputPostfix );
   GUI->OutputKeyword_Edit.SetText( m_instance.p_outputKeyword );
   GUI->OverwriteExistingFiles_CheckBox.SetChecked( m_instance.p_overwriteExistingFiles );
   GUI->OnError_ComboBox.SetCurrentItem( m_instance.p_onError );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->SubframeImages_TreeBox.SetFixedHeight();
   else
   {
      GUI->SubframeImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->SubframeImages_TreeBox.SetMaxHeight( int_max );
   }
}

// ----------------------------------------------------------------------------

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_SubframeImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.p_subframes.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_SubframeImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= m_instance.p_subframes.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );

   SubframeSelectorInstance::SubframeItem& item = m_instance.p_subframes[index];

   switch ( col )
   {
   case 0:
      // Activate the item's index number: ignore.
      break;
   case 1:
      // Activate the item's checkmark: toggle item's enabled state.
      item.enabled = !item.enabled;
      UpdateSubframeImageItem( index );
      break;
   case 2:
      {
         // Activate the item's path: open the image.
         Array<ImageWindow> windows = ImageWindow::Open( item.path, IsoString()/*id*/ );
         for ( ImageWindow& window : windows )
            window.Show();
      }
      break;
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_SubframeImages_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateSubframeImageSelectionButtons();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_SubframeImages_Click( Button& sender, bool checked )
{
   if ( sender == GUI->SubframeImages_AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.EnableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "SubframeSelector: Select Subframes" );

      if ( d.Execute() )
      {
         size_type i = TreeInsertionIndex( GUI->SubframeImages_TreeBox );
         for ( const String& path : d.FileNames() )
            m_instance.p_subframes.Insert( m_instance.p_subframes.At( i++ ), SubframeSelectorInstance::SubframeItem( path ) );
         UpdateSubframeImagesList();
         UpdateSubframeImageSelectionButtons();
      }
   }
   else if ( sender == GUI->SubframeImages_AddDrizzleFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "SubframeSelector: Select Drizzle Data Files" );
      d.SetFilter( FileFilter( "Drizzle Data Files", StringList() << ".xdrz" << ".drz" ) );
      d.EnableMultipleSelections();
      if ( d.Execute() )
      {
         IVector assigned( 0, int( m_instance.p_subframes.Length() ) );
         for ( const String& path : d.FileNames() )
         {
            String targetName = DrizzleTargetName( path );
            IVector::iterator n = assigned.Begin();
            for ( SubframeSelectorInstance::SubframeItem& item : m_instance.p_subframes )
            {
               String name = GUI->SubframeImages_StaticDataTargets_CheckBox.IsChecked() ?
                                 File::ChangeExtension( item.path, String() ) : File::ExtractName( item.path );
               if ( name == targetName )
               {
                  item.drzPath = path;
                  ++*n;
                  break;
               }
               ++n;
            }
         }

         UpdateSubframeImagesList();

         int total = 0;
         int duplicates = 0;
         for ( int i = 0; i < assigned.Length(); ++i )
            if ( assigned[i] > 0 )
            {
               ++total;
               if ( assigned[i] > 1 )
                  ++duplicates;
            }

         if ( total == 0 )
         {
            MessageBox( "<p>No drizzle data files have been assigned to input subframes.</p>",
                        "SubframeSelector",
                        StdIcon::Error,
                        StdButton::Ok ).Execute();
         }
         else
         {
            if ( total < assigned.Length() || duplicates )
               MessageBox( String().Format( "<p>%d of %d drizzle data files have been assigned.<br/>"
                                            "%d duplicate assignment(s)</p>", total, assigned.Length(), duplicates ),
                           "SubframeSelector",
                           StdIcon::Warning,
                           StdButton::Ok ).Execute();
         }
      }
   }
   else if ( sender == GUI->SubframeImages_ClearDrizzleFiles_PushButton )
   {
      for ( SubframeSelectorInstance::SubframeItem& item : m_instance.p_subframes )
         item.drzPath.Clear();
      UpdateSubframeImagesList();
   }
   else if ( sender == GUI->SubframeImages_AddLocalNormalizationFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "SubframeSelector: Select Local Normalization Data Files" );
      d.SetFilter( FileFilter( "Local Normalization Data Files", ".xnml" ) );
      d.EnableMultipleSelections();
      if ( d.Execute() )
      {
         IVector assigned( 0, int( m_instance.p_subframes.Length() ) );
         for ( const String& path : d.FileNames() )
         {
            String targetName = LocalNormalizationTargetName( path );
            IVector::iterator n = assigned.Begin();
            for ( SubframeSelectorInstance::SubframeItem& item : m_instance.p_subframes )
            {
               String name = GUI->SubframeImages_StaticDataTargets_CheckBox.IsChecked() ?
                                 File::ChangeExtension( item.path, String() ) : File::ExtractName( item.path );
               if ( name == targetName )
               {
                  item.nmlPath = path;
                  ++*n;
                  break;
               }
               ++n;
            }
         }

         UpdateSubframeImagesList();

         int total = 0;
         int duplicates = 0;
         for ( int i = 0; i < assigned.Length(); ++i )
            if ( assigned[i] > 0 )
            {
               ++total;
               if ( assigned[i] > 1 )
                  ++duplicates;
            }

         if ( total == 0 )
         {
            MessageBox( "<p>No local normalization data files have been assigned to input subframes.</p>",
                        "SubframeSelector",
                        StdIcon::Error,
                        StdButton::Ok ).Execute();
         }
         else
         {
            if ( total < assigned.Length() || duplicates )
               MessageBox( String().Format( "<p>%d of %d local normalization data files have been assigned.<br/>"
                                            "%d duplicate assignment(s)</p>", total, assigned.Length(), duplicates ),
                           "SubframeSelector",
                           StdIcon::Warning,
                           StdButton::Ok ).Execute();
         }
      }
   }
   else if ( sender == GUI->SubframeImages_ClearLocalNormalizationFiles_PushButton )
   {
      for ( SubframeSelectorInstance::SubframeItem& item : m_instance.p_subframes )
         item.nmlPath.Clear();
      UpdateSubframeImagesList();
   }
   else if ( sender == GUI->SubframeImages_SelectAll_PushButton )
   {
      GUI->SubframeImages_TreeBox.SelectAllNodes();
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->SubframeImages_Sort_PushButton )
   {
      if ( s_sortSubframesDialog == nullptr )
         s_sortSubframesDialog = new SortSubframesDialog;
      if ( s_sortSubframesDialog->Execute() )
      {
         SortSubframes( s_sortSubframesDialog->sortBy );
         UpdateSubframeImagesList();
      }
   }
   else if ( sender == GUI->SubframeImages_Invert_PushButton )
   {
      for ( int i = 0, n = GUI->SubframeImages_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->SubframeImages_TreeBox[i]->Select( !GUI->SubframeImages_TreeBox[i]->IsSelected() );
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->SubframeImages_Toggle_PushButton )
   {
      for ( int i = 0, n = GUI->SubframeImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->SubframeImages_TreeBox[i]->IsSelected() )
            m_instance.p_subframes[i].enabled = !m_instance.p_subframes[i].enabled;
      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->SubframeImages_Remove_PushButton )
   {
      SubframeSelectorInstance::subframe_list newTargets;
      for ( int i = 0, n = GUI->SubframeImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->SubframeImages_TreeBox[i]->IsSelected() )
            newTargets.Add( m_instance.p_subframes[i] );
      m_instance.p_subframes = newTargets;
      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->SubframeImages_Clear_PushButton )
   {
      m_instance.p_subframes.Clear();
      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->SubframeImages_FullPaths_CheckBox )
   {
      UpdateSubframeImagesList();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_RealValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->SystemParameters_SubframeScale_Control )
   {
      m_instance.p_subframeScale = value;
      TheSubframeSelectorMeasurementsInterface->UpdateControls();
   }
   else if ( sender == GUI->SystemParameters_CameraGain_Control )
   {
      m_instance.p_cameraGain = value;
      TheSubframeSelectorMeasurementsInterface->UpdateControls();
   }
   else if ( sender == GUI->StarDetectorParameters_Sensitivity_Control )
      m_instance.p_sensitivity = value;
   else if ( sender == GUI->StarDetectorParameters_PeakResponse_Control )
      m_instance.p_peakResponse = value;
   else if ( sender == GUI->StarDetectorParameters_BrightThreshold_Control )
      m_instance.p_brightThreshold = value;
   else if ( sender == GUI->StarDetectorParameters_MaxDistortion_Control )
      m_instance.p_maxDistortion = value;
   else if ( sender == GUI->StarDetectorParameters_UpperLimit_Control )
      m_instance.p_upperLimit = value;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->SystemParameters_SiteLocalMidnight_SpinBox )
      m_instance.p_siteLocalMidnight = value;
   else if ( sender == GUI->StarDetectorParameters_StructureLayers_SpinBox )
      m_instance.p_structureLayers = value;
   else if ( sender == GUI->StarDetectorParameters_NoiseLayers_SpinBox )
      m_instance.p_noiseLayers = value;
   else if ( sender == GUI->StarDetectorParameters_MinStructureSize_SpinBox )
      m_instance.p_minStructureSize = value;
   else if ( sender == GUI->StarDetectorParameters_HotPixelFilterRadius_SpinBox )
      m_instance.p_hotPixelFilterRadius = value;
   else if ( sender == GUI->StarDetectorParameters_NoiseReductionFilterRadius_SpinBox )
      m_instance.p_noiseReductionFilterRadius = value;
   else if ( sender == GUI->StarDetectorParameters_MaxPSFFits_SpinBox )
      m_instance.p_maxPSFFits = value;
   else if ( sender == GUI->RegionOfInterestX0_SpinBox )
      m_instance.p_roi.x0 = value;
   else if ( sender == GUI->RegionOfInterestY0_SpinBox )
      m_instance.p_roi.y0 = value;
   else if ( sender == GUI->RegionOfInterestWidth_SpinBox )
      m_instance.p_roi.x1 = m_instance.p_roi.x0 + value;
   else if ( sender == GUI->RegionOfInterestHeight_SpinBox )
      m_instance.p_roi.y1 = m_instance.p_roi.y0 + value;
   else if ( sender == GUI->PedestalValue_SpinBox )
      m_instance.p_pedestal = value;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Routine_ComboBox )
      m_instance.p_routine = itemIndex;
   else if ( sender == GUI->SystemParameters_CameraResolution_ComboBox )
   {
      m_instance.p_cameraResolution = itemIndex;
      TheSubframeSelectorMeasurementsInterface->UpdateControls();
   }
   else if ( sender == GUI->SystemParameters_ScaleUnit_ComboBox )
   {
      m_instance.p_scaleUnit = itemIndex;
      TheSubframeSelectorMeasurementsInterface->UpdateControls();
   }
   else if ( sender == GUI->SystemParameters_DataUnit_ComboBox )
   {
      m_instance.p_dataUnit = itemIndex;
      TheSubframeSelectorMeasurementsInterface->UpdateControls();
   }
   else if ( sender == GUI->StarDetectorParameters_PSFFit_ComboBox )
      m_instance.p_psfFit = itemIndex;
   else if ( sender == GUI->PedestalMode_ComboBox )
   {
      m_instance.p_pedestalMode = itemIndex;
      UpdatePedestalControls();
   }
   else if ( sender == GUI->OnError_ComboBox )
      m_instance.p_onError = itemIndex;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_CheckboxUpdated( Button& sender, Button::check_state state )
{
   if ( sender == GUI->SubframeImages_FileCache_CheckBox )
      m_instance.p_fileCache = state == CheckState::Checked;
   else if ( sender == GUI->StarDetectorParameters_AllowClusteredSources_CheckBox )
      m_instance.p_allowClusteredSources = state == CheckState::Checked;
   else if ( sender == GUI->StarDetectorParameters_PSFFitCircular_CheckBox )
      m_instance.p_psfFitCircular = state == CheckState::Checked;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->GoToExpressionsWindow_ToolButton )
   {
      HideExpressionsInterface();
      ShowExpressionsInterface();
      TheSubframeSelectorExpressionsInterface->Focus();
   }
   else if ( sender == GUI->GoToMeasurementsWindow_ToolButton )
   {
      HideMeasurementsInterface();
      ShowMeasurementsInterface();
      TheSubframeSelectorMeasurementsInterface->Focus();
   }
   else if ( sender == GUI->RegionOfInterestSelectPreview_Button )
   {
      PreviewSelectionDialog d;
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               m_instance.p_roi = view.Window().PreviewRect( view.Id() );
               UpdateControls();
            }
         }
   }
   else if ( sender == GUI->OutputDirectory_ToolButton )
   {
      GetDirectoryDialog d;
      d.SetCaption( "SubframeSelector: Select Output Directory" );
      if ( d.Execute() )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = d.Directory() );
   }
   else if ( sender == GUI->OverwriteExistingFiles_CheckBox )
      m_instance.p_overwriteExistingFiles = checked;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_EditCompleted( Edit& sender )
{
   String text = sender.Text().Trimmed();

   if ( sender == GUI->PedestalKeyword_Edit )
      m_instance.p_pedestalKeyword = text;
   else if ( sender == GUI->InputHints_Edit )
      m_instance.p_inputHints = text;
   else if ( sender == GUI->OutputHints_Edit )
      m_instance.p_outputHints = text;
   else if ( sender == GUI->OutputDirectory_Edit )
      m_instance.p_outputDirectory = text;
   else if ( sender == GUI->OutputPrefix_Edit )
      m_instance.p_outputPrefix = text;
   else if ( sender == GUI->OutputPostfix_Edit )
      m_instance.p_outputPostfix = text;
   else if ( sender == GUI->OutputKeyword_Edit )
      m_instance.p_outputKeyword = text;

   sender.SetText( text );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_StarDetector_ViewDrag( Control& sender, const Point& pos, const View& view,
                                                         unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->RegionOfInterestSelectPreview_Button )
      wantsView = view.IsPreview();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_StarDetector_ViewDrop( Control& sender, const Point& pos, const View& view,
                                                         unsigned modifiers )
{
   if ( sender == GUI->RegionOfInterestSelectPreview_Button )
      if ( view.IsPreview() )
      {
         m_instance.p_roi = view.Window().PreviewRect( view.Id() );
         UpdateControls();
      }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files,
                                            unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->SubframeImages_TreeBox.Viewport() )
      wantsFiles = true;
   else if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files,
                                            unsigned modifiers )
{
   if ( sender == GUI->SubframeImages_TreeBox.Viewport() )
   {
      StringList localNormalizationFiles, drizzleFiles;
      bool recursive = IsControlOrCmdPressed();
      size_type i0 = TreeInsertionIndex( GUI->SubframeImages_TreeBox );
      for ( const String& item : files )
      {
         StringList inputFiles;
         if ( File::Exists( item ) )
         {
            String ext = File::ExtractSuffix( item ).CaseFolded();
            if ( ext == ".xnml" )
               localNormalizationFiles << item;
            else if ( ext == ".xdrz" || ext == ".drz" )
               drizzleFiles << item;
            else
               inputFiles << item;
         }
         else if ( File::DirectoryExists( item ) )
         {
            inputFiles << FileFormat::SupportedImageFiles( item, true/*toRead*/, false/*toWrite*/, recursive );
            localNormalizationFiles << FileFormat::LocalNormalizationFiles( item, recursive );
            drizzleFiles << FileFormat::DrizzleFiles( item, recursive );
         }

         inputFiles.Sort();
         for ( const String& file : inputFiles )
         {
            String ext = File::ExtractSuffix( file ).CaseFolded();
            if ( ext == ".xnml" )
               localNormalizationFiles << file;
            else if ( ext == ".xdrz" || ext == ".drz" )
               drizzleFiles << file;
            else
               m_instance.p_subframes.Insert( m_instance.p_subframes.At( i0++ ), SubframeSelectorInstance::SubframeItem( file ) );
         }
      }

      for ( const String& file : localNormalizationFiles )
      {
         String targetName = LocalNormalizationTargetName( file );
         for ( SubframeSelectorInstance::SubframeItem& item : m_instance.p_subframes )
         {
            String name = GUI->SubframeImages_StaticDataTargets_CheckBox.IsChecked() ?
                              File::ChangeExtension( item.path, String() ) : File::ExtractName( item.path );
            if ( name == targetName )
            {
               item.nmlPath = file;
               break;
            }
         }
      }

      for ( const String& file : drizzleFiles )
      {
         String targetName = DrizzleTargetName( file );
         for ( SubframeSelectorInstance::SubframeItem& item : m_instance.p_subframes )
         {
            String name = GUI->SubframeImages_StaticDataTargets_CheckBox.IsChecked() ?
                              File::ChangeExtension( item.path, String() ) : File::ExtractName( item.path );
            if ( name == targetName )
            {
               item.drzPath = file;
               break;
            }
         }
      }

      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->OutputDirectory_Edit )
   {
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = files[0] );
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::e_Hide( Control& )
{
   m_expressionsWasVisible = TheSubframeSelectorExpressionsInterface->IsVisible();
   m_measurementsWasVisible = TheSubframeSelectorMeasurementsInterface->IsVisible();
   HideExpressionsInterface();
   HideMeasurementsInterface();
}

// ----------------------------------------------------------------------------

String SubframeSelectorInterface::LocalNormalizationTargetName( const String& filePath )
{
   LocalNormalizationData nml( filePath, true/*ignoreNormalizationData*/ );

   /*
    * If the XNML file includes a target normalization path, use it. Otherwise
    * the target should have the same name as the .xnml file.
    */
   String targetfilePath = nml.TargetFilePath();
   if ( targetfilePath.IsEmpty() )
      targetfilePath = filePath;

   if ( GUI->SubframeImages_StaticDataTargets_CheckBox.IsChecked() )
      return File::ChangeExtension( targetfilePath, String() );
   return File::ExtractName( targetfilePath );
}

// ----------------------------------------------------------------------------

String SubframeSelectorInterface::DrizzleTargetName( const String& filePath )
{
   DrizzleData drz( filePath, DrizzleParserOption::IgnoreIntegrationData );

   /*
    * If the XDRZ file includes a target alignment path, use it. Otherwise
    * the target should have the same name as the .xdrz file.
    */
   String targetfilePath = drz.AlignmentTargetFilePath();
   if ( targetfilePath.IsEmpty() )
      targetfilePath = filePath;

   if ( GUI->SubframeImages_StaticDataTargets_CheckBox.IsChecked() )
      return File::ChangeExtension( targetfilePath, String() );
   return File::ExtractName( targetfilePath );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::SortSubframes( SubframeSortingCriterion sortBy )
{
   if ( sortBy == SortByTimeAscending || sortBy == SortByTimeDescending )
   {
      Console console;
      console.Show();
      console.WriteLn( "<end><cbr><br>SubframeSelector: Retrieving acquisition times:<br>" );
      Module->ProcessEvents();

      for ( SubframeSelectorInstance::SubframeItem& item : m_instance.p_subframes )
      {
         console.WriteLn( "<end><cbr><raw>" + item.path + "</raw>" );

         FileFormat format( File::ExtractExtension( item.path ), true/*read*/, false/*write*/ );
         FileFormatInstance file( format );

         ImageDescriptionArray images;
         if ( !file.Open( images, item.path, m_instance.p_inputHints ) )
            throw CaughtException();

         FITSKeywordArray keywords;
         if ( format.CanStoreKeywords() )
            file.ReadFITSKeywords( keywords );

         PropertyArray properties;
         if ( format.CanStoreImageProperties() )
            properties = file.ReadImageProperties();

         item.obsTime = TimePoint(); // invalid TimePoint
         IntegrationMetadata metadata( properties, keywords );
         if ( metadata.IsValid() )
         {
            if ( metadata.startTime.IsDefined() )
            {
               item.obsTime = metadata.startTime();
               console.NoteLn( "<end><cbr>* Acquisition time: " + item.obsTime.ToString() + " UTC" );
            }
            else
            {
               console.WarningLn( "<end><cbr>** Warning: no valid acquisition time could be retrieved from file: <raw>" + item.path + "</raw>" );
            }
         }
         else
         {
            console.WarningLn( "<end><cbr>** Warning: no valid image metadata could be retrieved from file: <raw>" + item.path + "</raw>" );
         }

         (void)file.Close();
         Module->ProcessEvents();
      }
   }

   for ( MeasureItem& item : m_instance.o_measures )
      m_instance.p_subframes[item.index].measureItem = &item;

   m_instance.p_subframes.Sort(
      [sortBy]( SubframeSelectorInstance::SubframeItem& a,
                SubframeSelectorInstance::SubframeItem& b )
      {
         switch ( sortBy )
         {
         default: // ?!
         case SortByTimeAscending:  return a.obsTime < b.obsTime;
         case SortByTimeDescending: return b.obsTime < a.obsTime;
         case SortByPathAscending:  return a.path < b.path;
         case SortByPathDescending: return b.path < a.path;
         }
      } );

   if ( !m_instance.o_measures.IsEmpty() )
   {
      uint32 idx = 0;
      for ( SubframeSelectorInstance::SubframeItem& item : m_instance.p_subframes )
      {
         item.measureItem->index = idx++;
         item.measureItem = nullptr;
      }
   }

   UpdateSubframeImagesList();

   m_instance.p_sortingProperty = SSSortingProperty::Index;

   TheSubframeSelectorMeasurementsInterface->Cleanup();
   TheSubframeSelectorMeasurementsInterface->UpdateControls();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

SubframeSelectorInterface::GUIData::GUIData( SubframeSelectorInterface& w )
{
   int labelWidth1 = w.Font().Width( String( "Background expansion:" ) + 'M' );
   int labelWidth2 = w.Font().Width( String( "Height:" ) + 'M' );
   int editWidth1 = w.Font().Width( String( '0', 10  ) );

   //

   Routine_Label.SetText( "Routine:" );
   Routine_Label.SetMinWidth( labelWidth1 );
   Routine_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSRoutineParameter->NumberOfElements(); ++i )
      Routine_ComboBox.AddItem( TheSSRoutineParameter->ElementLabel( i ) );
   Routine_ComboBox.SetToolTip( TheSSRoutineParameter->Tooltip() );
   Routine_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorInterface::e_ItemSelected, w );

   GoToExpressionsWindow_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/function.png" ) ) );
   GoToExpressionsWindow_ToolButton.SetScaledFixedSize( 20, 20 );
   GoToExpressionsWindow_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   GoToExpressionsWindow_ToolButton.SetToolTip( "<p>Go to Expressions window.</p>" );
   GoToExpressionsWindow_ToolButton.OnClick( (ToolButton::click_event_handler)
                                    &SubframeSelectorInterface::e_ButtonClick, w );

   GoToMeasurementsWindow_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/chart.png" ) ) );
   GoToMeasurementsWindow_ToolButton.SetScaledFixedSize( 20, 20 );
   GoToMeasurementsWindow_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   GoToMeasurementsWindow_ToolButton.SetToolTip( "<p>Go to Measurements window.</p>" );
   GoToMeasurementsWindow_ToolButton.OnClick( (ToolButton::click_event_handler)
                                    &SubframeSelectorInterface::e_ButtonClick, w );

   Routine_Sizer.SetSpacing( 4 );
   Routine_Sizer.Add( Routine_Label );
   Routine_Sizer.Add( Routine_ComboBox );
   Routine_Sizer.AddStretch();
   Routine_Sizer.Add( GoToExpressionsWindow_ToolButton );
   Routine_Sizer.AddSpacing( 8 );
   Routine_Sizer.Add( GoToMeasurementsWindow_ToolButton );

   //

   SubframeImages_SectionBar.SetTitle( "Subframes" );
   SubframeImages_SectionBar.SetSection( SubframeImages_Control );
   SubframeImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)
                                    &SubframeSelectorInterface::e_ToggleSection, w );

   SubframeImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( w.Font() ) );
   SubframeImages_TreeBox.SetScaledMinWidth( 344 );
   SubframeImages_TreeBox.SetNumberOfColumns( 3 );
   SubframeImages_TreeBox.HideHeader();
   SubframeImages_TreeBox.EnableMultipleSelections();
   SubframeImages_TreeBox.DisableRootDecoration();
   SubframeImages_TreeBox.EnableAlternateRowColor();
   SubframeImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)
                                    &SubframeSelectorInterface::e_SubframeImages_CurrentNodeUpdated, w );
   SubframeImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)
                                    &SubframeSelectorInterface::e_SubframeImages_NodeActivated, w );
   SubframeImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)
                                    &SubframeSelectorInterface::e_SubframeImages_NodeSelectionUpdated, w );
   SubframeImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)
                                    &SubframeSelectorInterface::e_FileDrag, w );
   SubframeImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)
                                    &SubframeSelectorInterface::e_FileDrop, w );

   SubframeImages_AddFiles_PushButton.SetText( "Add Files" );
   SubframeImages_AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of subframes.</p>" );
   SubframeImages_AddFiles_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_AddLocalNormalizationFiles_PushButton.SetText( "Add L.Norm. Files" );
   SubframeImages_AddLocalNormalizationFiles_PushButton.SetToolTip( "<p>Associate existing local normalization data files with input subframes.</p>"
      "<p>Local normalization data files carry the .xnml suffix. Normally you should select .xnml files generated by "
      "the LocalNormalization tool for the same files that you are measuring.</p>" );
   SubframeImages_AddLocalNormalizationFiles_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_ClearLocalNormalizationFiles_PushButton.SetText( "Clear L.Norm. Files" );
   SubframeImages_ClearLocalNormalizationFiles_PushButton.SetToolTip( "<p>Remove all local normalization data files currently associated with "
      "input subframes.</p>"
      "<p>This removes just file associations, not the actual local normalization data files.</p>" );
   SubframeImages_ClearLocalNormalizationFiles_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_AddDrizzleFiles_PushButton.SetText( "Add Drizzle Files" );
   SubframeImages_AddDrizzleFiles_PushButton.SetToolTip( "<p>Associate existing drizzle data files with input subframes.</p>"
      "<p>Drizzle data files carry the .xdrz suffix. Normally you should select .xdrz files generated by "
      "the StarAlignment tool for the same files that you are measuring.</p>" );
   SubframeImages_AddDrizzleFiles_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_ClearDrizzleFiles_PushButton.SetText( "Clear Drizzle Files" );
   SubframeImages_ClearDrizzleFiles_PushButton.SetToolTip( "<p>Remove all drizzle data files currently associated with input subframes.</p>"
      "<p>This removes just file associations, not the actual drizzle data files.</p>" );
   SubframeImages_ClearDrizzleFiles_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_Sort_PushButton.SetText( "Sort..." );
   SubframeImages_Sort_PushButton.SetToolTip( "<p>Sort the list of input subframes.</p>" );
   SubframeImages_Sort_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_SelectAll_PushButton.SetText( "Select All" );
   SubframeImages_SelectAll_PushButton.SetToolTip( "<p>Select all input subframes.</p>" );
   SubframeImages_SelectAll_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_Invert_PushButton.SetText( "Invert Selection" );
   SubframeImages_Invert_PushButton.SetToolTip( "<p>Invert the current selection of subframes.</p>" );
   SubframeImages_Invert_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_Toggle_PushButton.SetText( "Toggle Selected" );
   SubframeImages_Toggle_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected subframes.</p>"
                                                 "<p>Disabled subframes will be ignored during the measuring and output processes.</p>" );
   SubframeImages_Toggle_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_Remove_PushButton.SetText( "Remove Selected" );
   SubframeImages_Remove_PushButton.SetToolTip( "<p>Remove all currently selected subframes.</p>" );
   SubframeImages_Remove_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_Clear_PushButton.SetText( "Clear" );
   SubframeImages_Clear_PushButton.SetToolTip( "<p>Clear the list of subframes.</p>" );
   SubframeImages_Clear_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_StaticDataTargets_CheckBox.SetText( "Static data targets" );
   SubframeImages_StaticDataTargets_CheckBox.SetToolTip( "<p>When assigning drizzle and/or local normalization data files to "
      "input subframes, take into account full file paths stored in .xdrz and .xnml files. This allows you to measure images "
      "with duplicate file names on different directories. However, by enabling this option your data set gets tied to "
      "specific locations on the local filesystem. When this option is disabled (the default state), only file names are "
      "used to associate target images with .xdrz and .xnml files, which allows you to move your images freely throughout "
      "the filesystem, including the possibility to migrate them to different machines.</p>"
      "<p>Changes to this option will come into play the next time you associate .xdrz and/or .xnml files with input "
      "subframes. Existing file associations are not affected.</p>");
   //SubframeImages_StaticDataTargets_CheckBox.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_FullPaths_CheckBox.SetText( "Full paths" );
   SubframeImages_FullPaths_CheckBox.SetToolTip( "<p>Show full paths for input image files.</p>" );
   SubframeImages_FullPaths_CheckBox.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::e_SubframeImages_Click, w );

   SubframeImages_FileCache_CheckBox.SetText( "File cache" );
   SubframeImages_FileCache_CheckBox.SetToolTip( TheSSFileCacheParameter->Tooltip() );
   SubframeImages_FileCache_CheckBox.OnCheck( (Button::check_event_handler)
                                    &SubframeSelectorInterface::e_CheckboxUpdated, w );

   SubframeButtons_Sizer.SetSpacing( 4 );
   SubframeButtons_Sizer.Add( SubframeImages_AddFiles_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_AddLocalNormalizationFiles_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_ClearLocalNormalizationFiles_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_AddDrizzleFiles_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_ClearDrizzleFiles_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_Sort_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_SelectAll_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_Invert_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_Toggle_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_Remove_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_Clear_PushButton );
   SubframeButtons_Sizer.AddStretch();
   SubframeButtons_Sizer.Add( SubframeImages_StaticDataTargets_CheckBox );
   SubframeButtons_Sizer.Add( SubframeImages_FullPaths_CheckBox );
   SubframeButtons_Sizer.Add( SubframeImages_FileCache_CheckBox );

   SubframeImages_Sizer.SetSpacing( 4 );
   SubframeImages_Sizer.Add( SubframeImages_TreeBox, 100 );
   SubframeImages_Sizer.Add( SubframeButtons_Sizer );

   SubframeImages_Control.SetSizer( SubframeImages_Sizer );

   //

   SystemParameters_SectionBar.SetTitle( "System Parameters" );
   SystemParameters_SectionBar.SetSection( SystemParameters_Control );
   SystemParameters_SectionBar.OnToggleSection( (SectionBar::section_event_handler)
                                    &SubframeSelectorInterface::e_ToggleSection, w );

   SystemParameters_SubframeScale_Control.label.SetText( "Subframe scale:" );
   SystemParameters_SubframeScale_Control.label.SetMinWidth( labelWidth1 );
   SystemParameters_SubframeScale_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   SystemParameters_SubframeScale_Control.slider.Hide();
   SystemParameters_SubframeScale_Control.SetReal();
   SystemParameters_SubframeScale_Control.SetRange( TheSSSubframeScaleParameter->MinimumValue(),
                                                    TheSSSubframeScaleParameter->MaximumValue() );
   SystemParameters_SubframeScale_Control.SetPrecision( TheSSSubframeScaleParameter->Precision() );
   SystemParameters_SubframeScale_Control.SetToolTip( TheSSSubframeScaleParameter->Tooltip() );
   SystemParameters_SubframeScale_Control.OnValueUpdated( (NumericEdit::value_event_handler)
                                    &SubframeSelectorInterface::e_RealValueUpdated, w );

   SystemParameters_SubframeScale_Unit_Label.SetText( "arcseconds / pixel" );
   SystemParameters_SubframeScale_Unit_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   SystemParameters_SubframeScale_Sizer.SetSpacing( 4 );
   SystemParameters_SubframeScale_Sizer.Add( SystemParameters_SubframeScale_Control );
   SystemParameters_SubframeScale_Sizer.Add( SystemParameters_SubframeScale_Unit_Label );
   SystemParameters_SubframeScale_Sizer.AddStretch();

   SystemParameters_CameraGain_Control.label.SetText( "Camera gain:" );
   SystemParameters_CameraGain_Control.label.SetMinWidth( labelWidth1 );
   SystemParameters_CameraGain_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   SystemParameters_CameraGain_Control.slider.Hide();
   SystemParameters_CameraGain_Control.SetReal();
   SystemParameters_CameraGain_Control.SetRange( TheSSCameraGainParameter->MinimumValue(),
                                                 TheSSCameraGainParameter->MaximumValue() );
   SystemParameters_CameraGain_Control.SetPrecision( TheSSCameraGainParameter->Precision() );
   SystemParameters_CameraGain_Control.SetToolTip( TheSSCameraGainParameter->Tooltip() );
   SystemParameters_CameraGain_Control.OnValueUpdated( (NumericEdit::value_event_handler)
                                    &SubframeSelectorInterface::e_RealValueUpdated, w );

   SystemParameters_CameraGain_Unit_Label.SetText( "electrons / Data Number" );
   SystemParameters_CameraGain_Unit_Label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   SystemParameters_CameraGain_Sizer.SetSpacing( 4 );
   SystemParameters_CameraGain_Sizer.Add( SystemParameters_CameraGain_Control );
   SystemParameters_CameraGain_Sizer.Add( SystemParameters_CameraGain_Unit_Label );
   SystemParameters_CameraGain_Sizer.AddStretch();

   SystemParameters_CameraResolution_Label.SetText( "Camera resolution:" );
   SystemParameters_CameraResolution_Label.SetMinWidth( labelWidth1 );
   SystemParameters_CameraResolution_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSCameraResolutionParameter->NumberOfElements(); ++i )
      SystemParameters_CameraResolution_ComboBox.AddItem( TheSSCameraResolutionParameter->ElementLabel( i ) );
   SystemParameters_CameraResolution_ComboBox.SetToolTip( TheSSCameraResolutionParameter->Tooltip() );
   SystemParameters_CameraResolution_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorInterface::e_ItemSelected, w );

   SystemParameters_CameraResolution_Sizer.SetSpacing( 4 );
   SystemParameters_CameraResolution_Sizer.Add( SystemParameters_CameraResolution_Label );
   SystemParameters_CameraResolution_Sizer.Add( SystemParameters_CameraResolution_ComboBox );
   SystemParameters_CameraResolution_Sizer.AddStretch();

   SystemParameters_SiteLocalMidnight_Label.SetText( "Site local midnight:" );
   SystemParameters_SiteLocalMidnight_Label.SetMinWidth( labelWidth1 );
   SystemParameters_SiteLocalMidnight_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   SystemParameters_SiteLocalMidnight_SpinBox.SetRange( TheSSSiteLocalMidnightParameter->MinimumValue(),
                                                        TheSSSiteLocalMidnightParameter->MaximumValue() );
   SystemParameters_SiteLocalMidnight_SpinBox.SetToolTip( TheSSSiteLocalMidnightParameter->Tooltip() );
   SystemParameters_SiteLocalMidnight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)
                                    &SubframeSelectorInterface::e_SpinValueUpdated, w );

   SystemParameters_SiteLocalMidnight_Unit_Label.SetText( "hours (UTC)" );
   SystemParameters_SiteLocalMidnight_Unit_Label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   SystemParameters_SiteLocalMidnight_Sizer.SetSpacing( 4 );
   SystemParameters_SiteLocalMidnight_Sizer.Add( SystemParameters_SiteLocalMidnight_Label );
   SystemParameters_SiteLocalMidnight_Sizer.Add( SystemParameters_SiteLocalMidnight_SpinBox );
   SystemParameters_SiteLocalMidnight_Sizer.Add( SystemParameters_SiteLocalMidnight_Unit_Label );
   SystemParameters_SiteLocalMidnight_Sizer.AddStretch();

   SystemParameters_ScaleUnit_Label.SetText( "Scale unit:" );
   SystemParameters_ScaleUnit_Label.SetMinWidth( labelWidth1 );
   SystemParameters_ScaleUnit_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSScaleUnitParameter->NumberOfElements(); ++i )
      SystemParameters_ScaleUnit_ComboBox.AddItem( TheSSScaleUnitParameter->ElementLabel( i ) );
   SystemParameters_ScaleUnit_ComboBox.SetToolTip( TheSSScaleUnitParameter->Tooltip() );
   SystemParameters_ScaleUnit_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorInterface::e_ItemSelected, w );

   SystemParameters_ScaleUnit_Sizer.SetSpacing( 4 );
   SystemParameters_ScaleUnit_Sizer.Add( SystemParameters_ScaleUnit_Label );
   SystemParameters_ScaleUnit_Sizer.Add( SystemParameters_ScaleUnit_ComboBox );
   SystemParameters_ScaleUnit_Sizer.AddStretch();

   SystemParameters_DataUnit_Label.SetText( "Data unit:" );
   SystemParameters_DataUnit_Label.SetMinWidth( labelWidth1 );
   SystemParameters_DataUnit_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSDataUnitParameter->NumberOfElements(); ++i )
      SystemParameters_DataUnit_ComboBox.AddItem( TheSSDataUnitParameter->ElementLabel( i ) );
   SystemParameters_DataUnit_ComboBox.SetToolTip( TheSSDataUnitParameter->Tooltip() );
   SystemParameters_DataUnit_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorInterface::e_ItemSelected, w );

   SystemParameters_DataUnit_Sizer.SetSpacing( 4 );
   SystemParameters_DataUnit_Sizer.Add( SystemParameters_DataUnit_Label );
   SystemParameters_DataUnit_Sizer.Add( SystemParameters_DataUnit_ComboBox );
   SystemParameters_DataUnit_Sizer.AddStretch();

   SystemParameters_Sizer.SetSpacing( 4 );
   SystemParameters_Sizer.Add( SystemParameters_SubframeScale_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_CameraGain_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_CameraResolution_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_SiteLocalMidnight_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_ScaleUnit_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_DataUnit_Sizer );

   SystemParameters_Control.SetSizer( SystemParameters_Sizer );

   //

   StarDetectorParameters_SectionBar.SetTitle( "Star Detector Parameters" );
   StarDetectorParameters_SectionBar.SetSection( StarDetectorParameters_Control );
   StarDetectorParameters_SectionBar.OnToggleSection( (SectionBar::section_event_handler)
                                    &SubframeSelectorInterface::e_ToggleSection, w );

   StarDetectorParameters_StructureLayers_Label.SetText( "Structure layers:" );
   StarDetectorParameters_StructureLayers_Label.SetMinWidth( labelWidth1 );
   StarDetectorParameters_StructureLayers_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_StructureLayers_SpinBox.SetRange( TheSSStructureLayersParameter->MinimumValue(),
                                                            TheSSStructureLayersParameter->MaximumValue() );
   StarDetectorParameters_StructureLayers_SpinBox.SetToolTip( TheSSStructureLayersParameter->Tooltip() );
   StarDetectorParameters_StructureLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)
                                    &SubframeSelectorInterface::e_SpinValueUpdated, w );

   StarDetectorParameters_StructureLayers_Sizer.SetSpacing( 4 );
   StarDetectorParameters_StructureLayers_Sizer.Add( StarDetectorParameters_StructureLayers_Label );
   StarDetectorParameters_StructureLayers_Sizer.Add( StarDetectorParameters_StructureLayers_SpinBox );
   StarDetectorParameters_StructureLayers_Sizer.AddStretch();

   StarDetectorParameters_NoiseLayers_Label.SetText( "Noise layers:" );
   StarDetectorParameters_NoiseLayers_Label.SetMinWidth( labelWidth1 );
   StarDetectorParameters_NoiseLayers_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_NoiseLayers_SpinBox.SetRange( TheSSNoiseLayersParameter->MinimumValue(),
                                                        TheSSNoiseLayersParameter->MaximumValue() );
   StarDetectorParameters_NoiseLayers_SpinBox.SetToolTip( TheSSNoiseLayersParameter->Tooltip() );
   StarDetectorParameters_NoiseLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)
                                    &SubframeSelectorInterface::e_SpinValueUpdated, w );

   StarDetectorParameters_NoiseLayers_Sizer.SetSpacing( 4 );
   StarDetectorParameters_NoiseLayers_Sizer.Add( StarDetectorParameters_NoiseLayers_Label );
   StarDetectorParameters_NoiseLayers_Sizer.Add( StarDetectorParameters_NoiseLayers_SpinBox );
   StarDetectorParameters_NoiseLayers_Sizer.AddStretch();

   StarDetectorParameters_MinStructureSize_Label.SetText( "Minimum structure size:" );
   StarDetectorParameters_MinStructureSize_Label.SetFixedWidth( labelWidth1 );
   StarDetectorParameters_MinStructureSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StarDetectorParameters_MinStructureSize_Label.SetToolTip( TheSSMinStructureSizeParameter->Tooltip() );

   StarDetectorParameters_MinStructureSize_SpinBox.SetRange( int( TheSSMinStructureSizeParameter->MinimumValue() ), int( TheSSMinStructureSizeParameter->MaximumValue() ) );
   StarDetectorParameters_MinStructureSize_SpinBox.SetMinimumValueText( "<Auto>" );
   StarDetectorParameters_MinStructureSize_SpinBox.SetToolTip( TheSSMinStructureSizeParameter->Tooltip() );
   StarDetectorParameters_MinStructureSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)
                                    &SubframeSelectorInterface::e_SpinValueUpdated, w );

   StarDetectorParameters_MinStructureSize_Sizer.SetSpacing( 4 );
   StarDetectorParameters_MinStructureSize_Sizer.Add( StarDetectorParameters_MinStructureSize_Label );
   StarDetectorParameters_MinStructureSize_Sizer.Add( StarDetectorParameters_MinStructureSize_SpinBox );
   StarDetectorParameters_MinStructureSize_Sizer.AddStretch();

   StarDetectorParameters_HotPixelFilterRadius_Label.SetText( "Hot pixel filter:" );
   StarDetectorParameters_HotPixelFilterRadius_Label.SetMinWidth( labelWidth1 );
   StarDetectorParameters_HotPixelFilterRadius_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_HotPixelFilterRadius_SpinBox.SetRange( TheSSHotPixelFilterRadiusParameter->MinimumValue(),
                                                                 TheSSHotPixelFilterRadiusParameter->MaximumValue() );
   StarDetectorParameters_HotPixelFilterRadius_SpinBox.SetToolTip( TheSSHotPixelFilterRadiusParameter->Tooltip() );
   StarDetectorParameters_HotPixelFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)
                                    &SubframeSelectorInterface::e_SpinValueUpdated, w );

   StarDetectorParameters_HotPixelFilterRadius_Sizer.SetSpacing( 4 );
   StarDetectorParameters_HotPixelFilterRadius_Sizer.Add( StarDetectorParameters_HotPixelFilterRadius_Label );
   StarDetectorParameters_HotPixelFilterRadius_Sizer.Add( StarDetectorParameters_HotPixelFilterRadius_SpinBox );
   StarDetectorParameters_HotPixelFilterRadius_Sizer.AddStretch();

   StarDetectorParameters_NoiseReductionFilterRadius_Label.SetText( "Noise reduction filter:" );
   StarDetectorParameters_NoiseReductionFilterRadius_Label.SetMinWidth( labelWidth1 );
   StarDetectorParameters_NoiseReductionFilterRadius_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_NoiseReductionFilterRadius_SpinBox.SetRange(
           TheSSNoiseReductionFilterRadiusParameter->MinimumValue(),
           TheSSNoiseReductionFilterRadiusParameter->MaximumValue() );
   StarDetectorParameters_NoiseReductionFilterRadius_SpinBox.SetToolTip( TheSSNoiseReductionFilterRadiusParameter->Tooltip() );
   StarDetectorParameters_NoiseReductionFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)
                                    &SubframeSelectorInterface::e_SpinValueUpdated, w );

   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.SetSpacing( 4 );
   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.Add( StarDetectorParameters_NoiseReductionFilterRadius_Label );
   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.Add( StarDetectorParameters_NoiseReductionFilterRadius_SpinBox );
   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.AddStretch();

   StarDetectorParameters_Sensitivity_Control.label.SetText( "Sensitivity:" );
   StarDetectorParameters_Sensitivity_Control.label.SetMinWidth( labelWidth1 );
   StarDetectorParameters_Sensitivity_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_Sensitivity_Control.SetReal();
   StarDetectorParameters_Sensitivity_Control.SetRange( TheSSSensitivityParameter->MinimumValue(),
                                                        TheSSSensitivityParameter->MaximumValue() );
   StarDetectorParameters_Sensitivity_Control.SetPrecision( TheSSSensitivityParameter->Precision() );
   StarDetectorParameters_Sensitivity_Control.SetToolTip( TheSSSensitivityParameter->Tooltip() );
   StarDetectorParameters_Sensitivity_Control.OnValueUpdated( (NumericEdit::value_event_handler)
                                    &SubframeSelectorInterface::e_RealValueUpdated, w );

   StarDetectorParameters_PeakResponse_Control.label.SetText( "Peak response:" );
   StarDetectorParameters_PeakResponse_Control.label.SetMinWidth( labelWidth1 );
   StarDetectorParameters_PeakResponse_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_PeakResponse_Control.SetReal();
   StarDetectorParameters_PeakResponse_Control.SetRange( TheSSPeakResponseParameter->MinimumValue(),
                                                         TheSSPeakResponseParameter->MaximumValue() );
   StarDetectorParameters_PeakResponse_Control.SetPrecision( TheSSPeakResponseParameter->Precision() );
   StarDetectorParameters_PeakResponse_Control.SetToolTip( TheSSPeakResponseParameter->Tooltip() );
   StarDetectorParameters_PeakResponse_Control.OnValueUpdated( (NumericEdit::value_event_handler)
                                    &SubframeSelectorInterface::e_RealValueUpdated, w );

   StarDetectorParameters_BrightThreshold_Control.label.SetText( "Bright threshold:" );
   StarDetectorParameters_BrightThreshold_Control.label.SetFixedWidth( labelWidth1 );
   StarDetectorParameters_BrightThreshold_Control.slider.SetRange( 1, 100 );
   StarDetectorParameters_BrightThreshold_Control.SetReal();
   StarDetectorParameters_BrightThreshold_Control.SetRange( TheSSBrightThresholdParameter->MinimumValue(),
                                                                   TheSSBrightThresholdParameter->MaximumValue() );
   StarDetectorParameters_BrightThreshold_Control.SetPrecision( TheSSBrightThresholdParameter->Precision() );
   StarDetectorParameters_BrightThreshold_Control.SetToolTip( TheSSBrightThresholdParameter->Tooltip() );
   StarDetectorParameters_BrightThreshold_Control.OnValueUpdated( (NumericEdit::value_event_handler)
                                    &SubframeSelectorInterface::e_RealValueUpdated, w );

   StarDetectorParameters_MaxDistortion_Control.label.SetText( "Maximum distortion:" );
   StarDetectorParameters_MaxDistortion_Control.label.SetMinWidth( labelWidth1 );
   StarDetectorParameters_MaxDistortion_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_MaxDistortion_Control.SetReal();
   StarDetectorParameters_MaxDistortion_Control.SetRange( TheSSMaxDistortionParameter->MinimumValue(),
                                                          TheSSMaxDistortionParameter->MaximumValue() );
   StarDetectorParameters_MaxDistortion_Control.SetPrecision( TheSSMaxDistortionParameter->Precision() );
   StarDetectorParameters_MaxDistortion_Control.SetToolTip( TheSSMaxDistortionParameter->Tooltip() );
   StarDetectorParameters_MaxDistortion_Control.OnValueUpdated( (NumericEdit::value_event_handler)
                                    &SubframeSelectorInterface::e_RealValueUpdated, w );

   StarDetectorParameters_UpperLimit_Control.label.SetText( "Upper limit:" );
   StarDetectorParameters_UpperLimit_Control.label.SetMinWidth( labelWidth1 );
   StarDetectorParameters_UpperLimit_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_UpperLimit_Control.SetReal();
   StarDetectorParameters_UpperLimit_Control.SetRange( TheSSUpperLimitParameter->MinimumValue(),
                                                       TheSSUpperLimitParameter->MaximumValue() );
   StarDetectorParameters_UpperLimit_Control.SetPrecision( TheSSUpperLimitParameter->Precision() );
   StarDetectorParameters_UpperLimit_Control.SetToolTip( TheSSUpperLimitParameter->Tooltip() );
   StarDetectorParameters_UpperLimit_Control.OnValueUpdated( (NumericEdit::value_event_handler)
                                    &SubframeSelectorInterface::e_RealValueUpdated, w );

   StarDetectorParameters_AllowClusteredSources_CheckBox.SetText( "Allow clustered sources" );
   StarDetectorParameters_AllowClusteredSources_CheckBox.SetToolTip( TheSSAllowClusteredSourcesParameter->Tooltip() );
   StarDetectorParameters_AllowClusteredSources_CheckBox.OnCheck( (Button::check_event_handler)
                                    &SubframeSelectorInterface::e_CheckboxUpdated, w );

   StarDetectorParameters_PSFFit_Label.SetText( "PSF fit:" );
   StarDetectorParameters_PSFFit_Label.SetMinWidth( labelWidth1 );
   StarDetectorParameters_PSFFit_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSPSFFitParameter->NumberOfElements(); ++i )
      StarDetectorParameters_PSFFit_ComboBox.AddItem( TheSSPSFFitParameter->ElementLabel( i ) );

   StarDetectorParameters_PSFFit_ComboBox.SetToolTip( TheSSPSFFitParameter->Tooltip() );
   StarDetectorParameters_PSFFit_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorInterface::e_ItemSelected, w );

   StarDetectorParameters_PSFFit_Sizer.SetSpacing( 4 );
   StarDetectorParameters_PSFFit_Sizer.Add( StarDetectorParameters_PSFFit_Label );
   StarDetectorParameters_PSFFit_Sizer.Add( StarDetectorParameters_PSFFit_ComboBox );
   StarDetectorParameters_PSFFit_Sizer.AddStretch();

   StarDetectorParameters_PSFFitCircular_CheckBox.SetText( "Circular PSF" );
   StarDetectorParameters_PSFFitCircular_CheckBox.SetToolTip( TheSSPSFFitCircularParameter->Tooltip() );
   StarDetectorParameters_PSFFitCircular_CheckBox.OnCheck( (Button::check_event_handler)
                                    &SubframeSelectorInterface::e_CheckboxUpdated, w );

   StarDetectorParameters_PSFFitCircular_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   StarDetectorParameters_PSFFitCircular_Sizer.Add( StarDetectorParameters_PSFFitCircular_CheckBox );
   StarDetectorParameters_PSFFitCircular_Sizer.AddStretch();

   StarDetectorParameters_MaxPSFFits_Label.SetText( "Maximum PSF fits:" );
   StarDetectorParameters_MaxPSFFits_Label.SetMinWidth( labelWidth1 );
   StarDetectorParameters_MaxPSFFits_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_MaxPSFFits_SpinBox.SetRange( TheSSMaxPSFFitsParameter->MinimumValue(),
                                                       TheSSMaxPSFFitsParameter->MaximumValue() );
   StarDetectorParameters_MaxPSFFits_SpinBox.SetMinimumValueText( "<Unlimited>" );
   StarDetectorParameters_MaxPSFFits_SpinBox.SetToolTip( TheSSMaxPSFFitsParameter->Tooltip() );
   StarDetectorParameters_MaxPSFFits_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)
                                    &SubframeSelectorInterface::e_SpinValueUpdated, w );

   StarDetectorParameters_MaxPSFFits_Sizer.SetSpacing( 4 );
   StarDetectorParameters_MaxPSFFits_Sizer.Add( StarDetectorParameters_MaxPSFFits_Label );
   StarDetectorParameters_MaxPSFFits_Sizer.Add( StarDetectorParameters_MaxPSFFits_SpinBox );
   StarDetectorParameters_MaxPSFFits_Sizer.AddStretch();

   StarDetectorParameters_Sizer.SetSpacing( 4 );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_StructureLayers_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_NoiseLayers_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_MinStructureSize_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_HotPixelFilterRadius_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_NoiseReductionFilterRadius_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_Sensitivity_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_PeakResponse_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_BrightThreshold_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_MaxDistortion_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_UpperLimit_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_AllowClusteredSources_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_PSFFit_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_PSFFitCircular_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_MaxPSFFits_Sizer );

   StarDetectorParameters_Control.SetSizer( StarDetectorParameters_Sizer );

   //

   RegionOfInterest_SectionBar.SetTitle( "Region of Interest" );
   RegionOfInterest_SectionBar.SetSection( RegionOfInterest_Control );
   RegionOfInterest_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&SubframeSelectorInterface::e_ToggleSection, w );

   RegionOfInterestX0_Label.SetText( "Left:" );
   RegionOfInterestX0_Label.SetMinWidth( labelWidth1 );
   RegionOfInterestX0_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   RegionOfInterestX0_Label.SetToolTip( "<p>X pixel coordinate of the upper-left corner of the ROI.</p>" );

   RegionOfInterestX0_SpinBox.SetRange( TheSSROIX0Parameter->MinimumValue(), TheSSROIX0Parameter->MaximumValue() );
   RegionOfInterestX0_SpinBox.SetFixedWidth( editWidth1 );
   RegionOfInterestX0_SpinBox.SetToolTip( "<p>X pixel coordinate of the upper-left corner of the ROI.</p>" );
   RegionOfInterestX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::e_SpinValueUpdated, w );

   RegionOfInterestY0_Label.SetText( "Top:" );
   RegionOfInterestY0_Label.SetMinWidth( labelWidth2 );
   RegionOfInterestY0_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   RegionOfInterestY0_Label.SetToolTip( "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>" );

   RegionOfInterestY0_SpinBox.SetRange( TheSSROIY0Parameter->MinimumValue(), TheSSROIY0Parameter->MaximumValue() );
   RegionOfInterestY0_SpinBox.SetFixedWidth( editWidth1 );
   RegionOfInterestY0_SpinBox.SetToolTip( "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>" );
   RegionOfInterestY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::e_SpinValueUpdated, w );

   RegionOfInterestRow1_Sizer.SetSpacing( 4 );
   RegionOfInterestRow1_Sizer.Add( RegionOfInterestX0_Label );
   RegionOfInterestRow1_Sizer.Add( RegionOfInterestX0_SpinBox );
   RegionOfInterestRow1_Sizer.Add( RegionOfInterestY0_Label );
   RegionOfInterestRow1_Sizer.Add( RegionOfInterestY0_SpinBox );
   RegionOfInterestRow1_Sizer.AddStretch();

   RegionOfInterestWidth_Label.SetText( "Width:" );
   RegionOfInterestWidth_Label.SetMinWidth( labelWidth1 );
   RegionOfInterestWidth_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   RegionOfInterestWidth_Label.SetToolTip( "<p>Width of the ROI in pixels.</p>" );

   RegionOfInterestWidth_SpinBox.SetRange( TheSSROIX1Parameter->MinimumValue(), TheSSROIX1Parameter->MaximumValue() );
   RegionOfInterestWidth_SpinBox.SetFixedWidth( editWidth1 );
   RegionOfInterestWidth_SpinBox.SetToolTip( "<p>Width of the ROI in pixels.</p>" );
   RegionOfInterestWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::e_SpinValueUpdated, w );

   RegionOfInterestHeight_Label.SetText( "Height:" );
   RegionOfInterestHeight_Label.SetMinWidth( labelWidth2 );
   RegionOfInterestHeight_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   RegionOfInterestHeight_Label.SetToolTip( "<p>Height of the ROI in pixels.</p>" );

   RegionOfInterestHeight_SpinBox.SetRange( TheSSROIY1Parameter->MinimumValue(), TheSSROIY1Parameter->MaximumValue() );
   RegionOfInterestHeight_SpinBox.SetFixedWidth( editWidth1 );
   RegionOfInterestHeight_SpinBox.SetToolTip( "<p>Height of the ROI in pixels.</p>" );
   RegionOfInterestHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::e_SpinValueUpdated, w );

   RegionOfInterestSelectPreview_Button.SetText( "From Preview" );
   RegionOfInterestSelectPreview_Button.SetToolTip(
           "<p>Import ROI coordinates from an existing preview.</p>" );
   RegionOfInterestSelectPreview_Button.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::e_ButtonClick, w );
   RegionOfInterestSelectPreview_Button.OnViewDrag( (Control::view_drag_event_handler)&SubframeSelectorInterface::e_StarDetector_ViewDrag, w );
   RegionOfInterestSelectPreview_Button.OnViewDrop( (Control::view_drop_event_handler)&SubframeSelectorInterface::e_StarDetector_ViewDrop, w );

   RegionOfInterestRow2_Sizer.SetSpacing( 4 );
   RegionOfInterestRow2_Sizer.Add( RegionOfInterestWidth_Label );
   RegionOfInterestRow2_Sizer.Add( RegionOfInterestWidth_SpinBox );
   RegionOfInterestRow2_Sizer.Add( RegionOfInterestHeight_Label );
   RegionOfInterestRow2_Sizer.Add( RegionOfInterestHeight_SpinBox );
   RegionOfInterestRow2_Sizer.AddSpacing( 12 );
   RegionOfInterestRow2_Sizer.Add( RegionOfInterestSelectPreview_Button );
   RegionOfInterestRow2_Sizer.AddStretch();

   RegionOfInterest_Sizer.SetSpacing( 4 );
   RegionOfInterest_Sizer.Add( RegionOfInterestRow1_Sizer );
   RegionOfInterest_Sizer.Add( RegionOfInterestRow2_Sizer );

   RegionOfInterest_Control.SetSizer( RegionOfInterest_Sizer );

   //

   Pedestal_SectionBar.SetTitle( "Pedestal" );
   Pedestal_SectionBar.SetSection( Pedestal_Control );
   Pedestal_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&SubframeSelectorInterface::e_ToggleSection, w );

   PedestalMode_Label.SetText( "Pedestal mode:" );
   PedestalMode_Label.SetToolTip( TheSSPedestalModeParameter->Tooltip() );
   PedestalMode_Label.SetMinWidth( labelWidth1 );
   PedestalMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   PedestalMode_ComboBox.AddItem( "Literal value" );
   PedestalMode_ComboBox.AddItem( "Default FITS keyword (PEDESTAL)" );
   PedestalMode_ComboBox.AddItem( "Custom FITS keyword" );
   PedestalMode_ComboBox.SetToolTip( TheSSPedestalModeParameter->Tooltip() );
   PedestalMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SubframeSelectorInterface::e_ItemSelected, w );

   PedestalMode_Sizer.SetSpacing( 4 );
   PedestalMode_Sizer.Add( PedestalMode_Label );
   PedestalMode_Sizer.Add( PedestalMode_ComboBox );
   PedestalMode_Sizer.AddStretch();

   PedestalValue_Label.SetText( "Pedestal value (DN):" );
   PedestalValue_Label.SetFixedWidth( labelWidth1 );
   PedestalValue_Label.SetToolTip( TheSSPedestalParameter->Tooltip() );
   PedestalValue_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   PedestalValue_SpinBox.SetRange( int( TheSSPedestalParameter->MinimumValue() ), int( TheSSPedestalParameter->MaximumValue() ) );
   PedestalValue_SpinBox.SetToolTip( TheSSPedestalParameter->Tooltip() );
   PedestalValue_SpinBox.SetFixedWidth( editWidth1 );
   PedestalValue_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::e_SpinValueUpdated, w );

   PedestalValue_Sizer.Add( PedestalValue_Label );
   PedestalValue_Sizer.AddSpacing( 4 );
   PedestalValue_Sizer.Add( PedestalValue_SpinBox );
   PedestalValue_Sizer.AddStretch();

   PedestalKeyword_Label.SetText( "Pedestal keyword:" );
   PedestalKeyword_Label.SetFixedWidth( labelWidth1 );
   PedestalKeyword_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PedestalKeyword_Label.SetToolTip( TheSSPedestalKeywordParameter->Tooltip() );

   PedestalKeyword_Edit.SetMinWidth( editWidth1 );
   PedestalKeyword_Edit.SetToolTip( TheSSPedestalKeywordParameter->Tooltip() );
   PedestalKeyword_Edit.OnEditCompleted( (Edit::edit_event_handler)&SubframeSelectorInterface::e_EditCompleted, w );

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

   FormatHints_SectionBar.SetTitle( "Format Hints" );
   FormatHints_SectionBar.SetSection( FormatHints_Control );
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&SubframeSelectorInterface::e_ToggleSection, w );

   InputHints_Label.SetText( "Input hints:" );
   InputHints_Label.SetFixedWidth( labelWidth1 );
   InputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InputHints_Label.SetToolTip( TheSSInputHintsParameter->Tooltip() );

   InputHints_Edit.SetToolTip( TheSSInputHintsParameter->Tooltip() );
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&SubframeSelectorInterface::e_EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   OutputHints_Label.SetText( "Output hints:" );
   OutputHints_Label.SetFixedWidth( labelWidth1 );
   OutputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputHints_Label.SetToolTip( TheSSInputHintsParameter->Tooltip() );

   OutputHints_Edit.SetToolTip( TheSSInputHintsParameter->Tooltip() );
   OutputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&SubframeSelectorInterface::e_EditCompleted, w );

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
   OutputFiles_SectionBar.OnToggleSection( (SectionBar::section_event_handler)
                                    &SubframeSelectorInterface::e_ToggleSection, w );

   OutputDirectory_Label.SetText( "Directory:" );
   OutputDirectory_Label.SetFixedWidth( labelWidth1 );
   OutputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputDirectory_Edit.SetToolTip( TheSSOutputDirectoryParameter->Tooltip() );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)
                                    &SubframeSelectorInterface::e_EditCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)
                                    &SubframeSelectorInterface::e_FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)
                                    &SubframeSelectorInterface::e_FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorInterface::e_ButtonClick, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   OutputPrefix_Label.SetText( "Prefix:" );
   OutputPrefix_Label.SetFixedWidth( labelWidth1 );
   OutputPrefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputPrefix_Edit.SetToolTip( TheSSOutputPrefixParameter->Tooltip() );
   OutputPrefix_Edit.OnEditCompleted( (Edit::edit_event_handler)
                                    &SubframeSelectorInterface::e_EditCompleted, w );

   OutputPostfix_Label.SetText( "Postfix:" );
   OutputPostfix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputPostfix_Edit.SetToolTip( TheSSOutputPostfixParameter->Tooltip() );
   OutputPostfix_Edit.OnEditCompleted( (Edit::edit_event_handler)
                                    &SubframeSelectorInterface::e_EditCompleted, w );

   OutputChunks_Sizer.SetSpacing( 4 );
   OutputChunks_Sizer.Add( OutputPrefix_Label );
   OutputChunks_Sizer.Add( OutputPrefix_Edit );
   OutputChunks_Sizer.AddSpacing( 12 );
   OutputChunks_Sizer.Add( OutputPostfix_Label );
   OutputChunks_Sizer.Add( OutputPostfix_Edit );
   OutputChunks_Sizer.AddStretch();

   OutputKeyword_Label.SetText( "Keyword:" );
   OutputKeyword_Label.SetFixedWidth( labelWidth1 );
   OutputKeyword_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputKeyword_Edit.SetToolTip( TheSSOutputKeywordParameter->Tooltip() );
   OutputKeyword_Edit.OnEditCompleted( (Edit::edit_event_handler)
                                    &SubframeSelectorInterface::e_EditCompleted, w );

   OutputKeyword_Sizer.SetSpacing( 4 );
   OutputKeyword_Sizer.Add( OutputKeyword_Label );
   OutputKeyword_Sizer.Add( OutputKeyword_Edit );
   OutputKeyword_Sizer.AddStretch();

   OverwriteExistingFiles_CheckBox.SetText( "Overwrite existing files" );
   OverwriteExistingFiles_CheckBox.SetToolTip( TheSSOverwriteExistingFilesParameter->Tooltip() );
   OverwriteExistingFiles_CheckBox.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorInterface::e_ButtonClick, w );

   OverwriteExistingFiles_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   OverwriteExistingFiles_Sizer.Add( OverwriteExistingFiles_CheckBox );
   OverwriteExistingFiles_Sizer.AddStretch();

   OnError_Label.SetText( "On error:" );
   OnError_Label.SetFixedWidth( labelWidth1 );
   OnError_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSOnErrorParameter->NumberOfElements(); ++i )
      OnError_ComboBox.AddItem( TheSSOnErrorParameter->ElementLabel( i ) );

   OnError_ComboBox.SetToolTip( TheSSOnErrorParameter->Tooltip() );
   OnError_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorInterface::e_ItemSelected, w );

   OnError_Sizer.SetSpacing( 4 );
   OnError_Sizer.Add( OnError_Label );
   OnError_Sizer.Add( OnError_ComboBox );
   OnError_Sizer.AddStretch();

   OutputFiles_Sizer.SetSpacing( 4 );
   OutputFiles_Sizer.Add( OutputDirectory_Sizer );
   OutputFiles_Sizer.Add( OutputChunks_Sizer );
   OutputFiles_Sizer.Add( OutputKeyword_Sizer );
   OutputFiles_Sizer.Add( OverwriteExistingFiles_Sizer );
   OutputFiles_Sizer.Add( OnError_Sizer );

   OutputFiles_Control.SetSizer( OutputFiles_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Routine_Sizer );
   Global_Sizer.Add( SubframeImages_SectionBar );
   Global_Sizer.Add( SubframeImages_Control );
   Global_Sizer.Add( SystemParameters_SectionBar );
   Global_Sizer.Add( SystemParameters_Control );
   Global_Sizer.Add( StarDetectorParameters_SectionBar );
   Global_Sizer.Add( StarDetectorParameters_Control );
   Global_Sizer.Add( RegionOfInterest_SectionBar );
   Global_Sizer.Add( RegionOfInterest_Control );
   Global_Sizer.Add( Pedestal_SectionBar );
   Global_Sizer.Add( Pedestal_Control );
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add( OutputFiles_SectionBar );
   Global_Sizer.Add( OutputFiles_Control );

   w.SetSizer( Global_Sizer );

   StarDetectorParameters_Control.Hide();
   Pedestal_Control.Hide();
   FormatHints_Control.Hide();
   RegionOfInterest_Control.Hide();

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedWidth();

   w.OnHide( (Control::event_handler)&SubframeSelectorInterface::e_Hide, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInterface.cpp - Released 2024-12-17T18:15:44Z
