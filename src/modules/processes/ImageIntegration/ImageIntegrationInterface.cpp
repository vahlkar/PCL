//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// ImageIntegrationInterface.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
//
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "ImageIntegrationInterface.h"
#include "ImageIntegrationProcess.h"
#include "IntegrationCache.h"

#include <pcl/DrizzleData.h>
#include <pcl/FileDataCachePreferencesDialog.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/LocalNormalizationData.h>
#include <pcl/MessageBox.h>
#include <pcl/PreviewSelectionDialog.h>

#define IMAGELIST_MINHEIGHT( fnt )  RoundInt( 8.125*fnt.Height() )

namespace pcl
{

// ----------------------------------------------------------------------------

ImageIntegrationInterface* TheImageIntegrationInterface = nullptr;

// ----------------------------------------------------------------------------

ImageIntegrationInterface::ImageIntegrationInterface()
   : m_instance( TheImageIntegrationProcess )
{
   TheImageIntegrationInterface = this;

   /*
    * The auto save geometry feature is of no good to interfaces that include
    * both auto-expanding controls (e.g. TreeBox) and collapsible sections
    * (e.g. SectionBar).
    */
   DisableAutoSaveGeometry();
}

// ----------------------------------------------------------------------------

ImageIntegrationInterface::~ImageIntegrationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString ImageIntegrationInterface::Id() const
{
   return "ImageIntegration";
}

// ----------------------------------------------------------------------------

MetaProcess* ImageIntegrationInterface::Process() const
{
   return TheImageIntegrationProcess;
}

// ----------------------------------------------------------------------------

String ImageIntegrationInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/ImageIntegration.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures ImageIntegrationInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal | InterfaceFeature::PreferencesButton;
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::EditPreferences()
{
   if ( TheIntegrationCache == nullptr )
      new IntegrationCache; // loads upon construction
   FileDataCachePreferencesDialog dlg( TheIntegrationCache );
   dlg.Execute();
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::ResetInstance()
{
   ImageIntegrationInstance defaultInstance( TheImageIntegrationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ImageIntegration" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheImageIntegrationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ImageIntegrationInterface::NewProcess() const
{
   return new ImageIntegrationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const ImageIntegrationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an ImageIntegration instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::SaveSettings() const
{
   SaveGeometry();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ImageIntegrationInterface::UpdateControls()
{
   UpdateInputImagesList();
   UpdateImageSelectionButtons();
   UpdateFormatHintsControls();
   UpdateIntegrationControls();
   UpdateRejectionControls();
   UpdateROIControls();
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::UpdateInputImagesItem( size_type i )
{
   TreeBox::Node* node = GUI->InputImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const ImageIntegrationInstance::ImageItem& item = m_instance.p_images[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) );
   node->SetAlignment( 1, TextAlign::Left );

   String fileText;
   if ( !item.nmlPath.IsEmpty() )
      fileText << "<n> ";
   if ( !item.drzPath.IsEmpty() )
      fileText << "<d> ";
   if ( GUI->FullPaths_CheckBox.IsChecked() )
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

void ImageIntegrationInterface::UpdateInputImagesList()
{
   int currentIdx = GUI->InputImages_TreeBox.ChildIndex( GUI->InputImages_TreeBox.CurrentNode() );

   GUI->InputImages_TreeBox.DisableUpdates();
   GUI->InputImages_TreeBox.Clear();

   for ( size_type i = 0; i < m_instance.p_images.Length(); ++i )
   {
      new TreeBox::Node( GUI->InputImages_TreeBox );
      UpdateInputImagesItem( i );
   }

   GUI->InputImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->InputImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->InputImages_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !m_instance.p_images.IsEmpty() )
      if ( currentIdx >= 0 && currentIdx < GUI->InputImages_TreeBox.NumberOfChildren() )
         GUI->InputImages_TreeBox.SetCurrentNode( GUI->InputImages_TreeBox[currentIdx] );

   GUI->InputImages_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::UpdateImageSelectionButtons()
{
   bool hasItems = GUI->InputImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->InputImages_TreeBox.HasSelectedTopLevelNodes();

   GUI->AddDrizzleFiles_PushButton.Enable( hasItems );
   GUI->ClearDrizzleFiles_PushButton.Enable( hasItems );
   GUI->AddLocalNormalizationFiles_PushButton.Enable( hasItems );
   GUI->ClearLocalNormalizationFiles_PushButton.Enable( hasItems );
   GUI->SetReference_PushButton.Enable( GUI->InputImages_TreeBox.CurrentNode() != 0 );
   GUI->SelectAll_PushButton.Enable( hasItems );
   GUI->InvertSelection_PushButton.Enable( hasItems );
   GUI->ToggleSelected_PushButton.Enable( hasSelection );
   GUI->RemoveSelected_PushButton.Enable( hasSelection );
   GUI->Clear_PushButton.Enable( hasItems );
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::UpdateFormatHintsControls()
{
   GUI->InputHints_Edit.SetText( m_instance.p_inputHints );
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::UpdateIntegrationControls()
{
   bool isAverage = m_instance.p_generateIntegratedImage && m_instance.p_combination == IICombination::Average;
   bool isKeywordWeight = m_instance.p_weightMode == IIWeightMode::KeywordWeight;

   GUI->Combination_Label.Enable( m_instance.p_generateIntegratedImage );

   GUI->Combination_ComboBox.SetCurrentItem( m_instance.p_combination );
   GUI->Combination_ComboBox.Enable( m_instance.p_generateIntegratedImage );

   GUI->Normalization_ComboBox.SetCurrentItem( m_instance.p_normalization );

   GUI->AdaptiveGridSize_SpinBox.SetValue( m_instance.p_adaptiveGridSize );

   GUI->AdaptiveNoScale_CheckBox.SetChecked( m_instance.p_adaptiveNoScale );

   GUI->WeightMode_Label.Enable( isAverage );

   GUI->WeightMode_ComboBox.Enable( isAverage );
   GUI->WeightMode_ComboBox.SetCurrentItem( m_instance.p_weightMode );

   GUI->WeightKeyword_Label.Enable( isAverage && isKeywordWeight );

   GUI->WeightScale_ComboBox.Enable( isAverage );
   GUI->WeightScale_ComboBox.SetCurrentItem( m_instance.p_weightScale );

   GUI->WeightKeyword_Edit.Enable( isAverage && isKeywordWeight );
   GUI->WeightKeyword_Edit.SetText( m_instance.p_weightKeyword );

   GUI->IgnoreNoiseKeywords_CheckBox.SetChecked( m_instance.p_ignoreNoiseKeywords );

   GUI->GenerateIntegratedImage_CheckBox.SetChecked( m_instance.p_generateIntegratedImage );

   GUI->Generate64BitResult_CheckBox.SetChecked( m_instance.p_generate64BitResult );
   GUI->Generate64BitResult_CheckBox.Enable( m_instance.p_generateIntegratedImage );

   GUI->GenerateDrizzleData_CheckBox.SetChecked( m_instance.p_generateDrizzleData );

   GUI->SubtractPedestals_CheckBox.SetChecked( m_instance.p_subtractPedestals );

   GUI->TruncateOnOutOfRange_CheckBox.SetChecked( m_instance.p_truncateOnOutOfRange );
   GUI->TruncateOnOutOfRange_CheckBox.Enable( m_instance.p_generateIntegratedImage );

   GUI->EvaluateNoise_CheckBox.SetChecked( m_instance.p_evaluateNoise );
   GUI->EvaluateNoise_CheckBox.Enable( m_instance.p_generateIntegratedImage );

   GUI->ClosePreviousImages_CheckBox.SetChecked( m_instance.p_closePreviousImages );

   GUI->AutoMemorySize_CheckBox.SetChecked( m_instance.p_autoMemorySize );

   GUI->BufferSize_SpinBox.SetValue( m_instance.p_bufferSizeMB );

   GUI->BufferSize_Label.Enable( !m_instance.p_autoMemorySize );
   GUI->BufferSize_SpinBox.Enable( !m_instance.p_autoMemorySize );

   GUI->StackSize_SpinBox.SetValue( m_instance.p_stackSizeMB );

   GUI->StackSize_Label.Enable( !m_instance.p_autoMemorySize );
   GUI->StackSize_SpinBox.Enable( !m_instance.p_autoMemorySize );

   GUI->UseCache_CheckBox.SetChecked( m_instance.p_useCache );
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::UpdateRejectionControls()
{
   bool doesRejection = m_instance.p_rejection != IIRejection::NoRejection;
   bool doesRangeRejection = m_instance.p_rangeClipLow || m_instance.p_rangeClipHigh;

   bool doesMinMaxRejection = m_instance.p_rejection == IIRejection::MinMax;

   bool doesPercentileClipRejection = m_instance.p_rejection == IIRejection::PercentileClip;

   bool doesSigmaClipRejection = m_instance.p_rejection == IIRejection::SigmaClip ||
                                 m_instance.p_rejection == IIRejection::WinsorizedSigmaClip ||
                                 m_instance.p_rejection == IIRejection::AveragedSigmaClip;

   bool doesLinearFitRejection = m_instance.p_rejection == IIRejection::LinearFit;

   bool doesESDRejection = m_instance.p_rejection == IIRejection::ESD;

   bool doesCCDClipRejection = m_instance.p_rejection == IIRejection::CCDClip;

   GUI->RejectionAlgorithm_ComboBox.SetCurrentItem( m_instance.p_rejection );

   GUI->RejectionNormalization_ComboBox.Enable( doesRejection );
   GUI->RejectionNormalization_ComboBox.SetCurrentItem( m_instance.p_rejectionNormalization );

   GUI->GenerateRejectionMaps_CheckBox.Enable( doesRejection || doesRangeRejection );
   GUI->GenerateRejectionMaps_CheckBox.SetChecked( m_instance.p_generateRejectionMaps );

   GUI->ClipLow_CheckBox.Enable( doesRejection );
   GUI->ClipLow_CheckBox.SetChecked( m_instance.p_clipLow );

   GUI->ClipHigh_CheckBox.Enable( doesRejection );
   GUI->ClipHigh_CheckBox.SetChecked( m_instance.p_clipHigh );

   GUI->ClipLowRange_CheckBox.SetChecked( m_instance.p_rangeClipLow );

   GUI->ClipHighRange_CheckBox.SetChecked( m_instance.p_rangeClipHigh );

   GUI->ReportRangeRejection_CheckBox.Enable( doesRangeRejection );
   GUI->ReportRangeRejection_CheckBox.SetChecked( m_instance.p_reportRangeRejection );

   GUI->MapRangeRejection_CheckBox.Enable( m_instance.p_generateRejectionMaps && doesRangeRejection );
   GUI->MapRangeRejection_CheckBox.SetChecked( m_instance.p_mapRangeRejection );

   GUI->MinMaxLow_Label.Enable( doesMinMaxRejection && m_instance.p_clipLow );

   GUI->MinMaxLow_SpinBox.Enable( doesMinMaxRejection && m_instance.p_clipLow );
   GUI->MinMaxLow_SpinBox.SetValue( m_instance.p_minMaxLow );

   GUI->MinMaxHigh_Label.Enable( doesMinMaxRejection && m_instance.p_clipHigh );

   GUI->MinMaxHigh_SpinBox.Enable( doesMinMaxRejection && m_instance.p_clipHigh );
   GUI->MinMaxHigh_SpinBox.SetValue( m_instance.p_minMaxHigh );

   GUI->PercentileLow_NumericControl.Enable( doesPercentileClipRejection && m_instance.p_clipLow );
   GUI->PercentileLow_NumericControl.SetValue( m_instance.p_pcClipLow );

   GUI->PercentileHigh_NumericControl.Enable( doesPercentileClipRejection && m_instance.p_clipHigh );
   GUI->PercentileHigh_NumericControl.SetValue( m_instance.p_pcClipHigh );

   GUI->SigmaLow_NumericControl.Enable( doesSigmaClipRejection && m_instance.p_clipLow );
   GUI->SigmaLow_NumericControl.SetValue( m_instance.p_sigmaLow );

   GUI->SigmaHigh_NumericControl.Enable( doesSigmaClipRejection && m_instance.p_clipHigh );
   GUI->SigmaHigh_NumericControl.SetValue( m_instance.p_sigmaHigh );

   GUI->WinsorizationCutoff_NumericControl.Enable( m_instance.p_rejection == IIRejection::WinsorizedSigmaClip );
   GUI->WinsorizationCutoff_NumericControl.SetValue( m_instance.p_winsorizationCutoff );

   GUI->LinearFitLow_NumericControl.Enable( doesLinearFitRejection );
   GUI->LinearFitLow_NumericControl.SetValue( m_instance.p_linearFitLow );

   GUI->LinearFitHigh_NumericControl.Enable( doesLinearFitRejection );
   GUI->LinearFitHigh_NumericControl.SetValue( m_instance.p_linearFitHigh );

   GUI->ESDOutliersFraction_NumericControl.Enable( doesESDRejection );
   GUI->ESDOutliersFraction_NumericControl.SetValue( m_instance.p_esdOutliersFraction );

   GUI->ESDAlpha_NumericControl.Enable( doesESDRejection );
   GUI->ESDAlpha_NumericControl.SetValue( m_instance.p_esdAlpha );

   GUI->ESDLowRelaxation_NumericControl.Enable( doesESDRejection );
   GUI->ESDLowRelaxation_NumericControl.SetValue( m_instance.p_esdLowRelaxation );

   GUI->CCDGain_NumericControl.Enable( doesCCDClipRejection );
   GUI->CCDGain_NumericControl.SetValue( m_instance.p_ccdGain );

   GUI->CCDReadNoise_NumericControl.Enable( doesCCDClipRejection );
   GUI->CCDReadNoise_NumericControl.SetValue( m_instance.p_ccdReadNoise );

   GUI->CCDScaleNoise_NumericControl.Enable( doesCCDClipRejection );
   GUI->CCDScaleNoise_NumericControl.SetValue( m_instance.p_ccdScaleNoise );

   GUI->RangeLow_NumericControl.Enable( m_instance.p_rangeClipLow );
   GUI->RangeLow_NumericControl.SetValue( m_instance.p_rangeLow );

   GUI->RangeHigh_NumericControl.Enable( m_instance.p_rangeClipHigh );
   GUI->RangeHigh_NumericControl.SetValue( m_instance.p_rangeHigh );

   GUI->LargeScaleRejection_Control.Enable( doesRejection );

   GUI->RejectLargeScaleLow_CheckBox.SetChecked( m_instance.p_largeScaleClipLow );
   GUI->RejectLargeScaleLow_CheckBox.Enable( m_instance.p_clipLow );

   GUI->SmallScaleLayersLow_Label.Enable( m_instance.p_clipLow && m_instance.p_largeScaleClipLow );

   GUI->SmallScaleLayersLow_SpinBox.SetValue( m_instance.p_largeScaleClipLowProtectedLayers );
   GUI->SmallScaleLayersLow_SpinBox.Enable( m_instance.p_clipLow && m_instance.p_largeScaleClipLow );

   GUI->GrowthLow_Label.Enable( m_instance.p_clipLow && m_instance.p_largeScaleClipLow );

   GUI->GrowthLow_SpinBox.SetValue( m_instance.p_largeScaleClipLowGrowth );
   GUI->GrowthLow_SpinBox.Enable( m_instance.p_clipLow && m_instance.p_largeScaleClipLow );

   GUI->RejectLargeScaleHigh_CheckBox.SetChecked( m_instance.p_largeScaleClipHigh );
   GUI->RejectLargeScaleHigh_CheckBox.Enable( m_instance.p_clipHigh );

   GUI->SmallScaleLayersHigh_Label.Enable( m_instance.p_clipHigh && m_instance.p_largeScaleClipHigh );

   GUI->SmallScaleLayersHigh_SpinBox.SetValue( m_instance.p_largeScaleClipHighProtectedLayers );
   GUI->SmallScaleLayersHigh_SpinBox.Enable( m_instance.p_clipHigh && m_instance.p_largeScaleClipHigh );

   GUI->GrowthHigh_Label.Enable( m_instance.p_clipHigh && m_instance.p_largeScaleClipHigh );

   GUI->GrowthHigh_SpinBox.SetValue( m_instance.p_largeScaleClipHighGrowth );
   GUI->GrowthHigh_SpinBox.Enable( m_instance.p_clipHigh && m_instance.p_largeScaleClipHigh );
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::UpdateROIControls()
{
   GUI->ROI_SectionBar.SetChecked( m_instance.p_useROI );
   GUI->ROIX0_SpinBox.SetValue( m_instance.p_roi.x0 );
   GUI->ROIY0_SpinBox.SetValue( m_instance.p_roi.y0 );
   GUI->ROIWidth_SpinBox.SetValue( m_instance.p_roi.Width() );
   GUI->ROIHeight_SpinBox.SetValue( m_instance.p_roi.Height() );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_InputImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.p_images.Length() )
      throw Error( "ImageIntegrationInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_InputImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= m_instance.p_images.Length() )
      throw Error( "ImageIntegrationInterface: *Warning* Corrupted interface structures" );

   ImageIntegrationInstance::ImageItem& item = m_instance.p_images[index];

   switch ( col )
   {
   case 0:
      break;
   case 1:
      item.enabled = !item.enabled;
      UpdateInputImagesItem( index );
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

void ImageIntegrationInterface::e_InputImages_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

// ----------------------------------------------------------------------------

String ImageIntegrationInterface::LocalNormalizationTargetName( const String& filePath )
{
   LocalNormalizationData nml( filePath, true/*ignoreNormalizationData*/ );

   /*
    * If the XNML file includes a target normalization path, use it. Otherwise
    * the target should have the same name as the .xnml file.
    */
   String targetfilePath = nml.TargetFilePath();
   if ( targetfilePath.IsEmpty() )
      targetfilePath = filePath;

   if ( GUI->StaticDataTargets_CheckBox.IsChecked() )
      return File::ChangeExtension( targetfilePath, String() );
   return File::ExtractName( targetfilePath );
}

// ----------------------------------------------------------------------------

String ImageIntegrationInterface::DrizzleTargetName( const String& filePath )
{
   DrizzleData drz( filePath, true/*ignoreIntegrationData*/ );

   /*
    * If the XDRZ file includes a target alignment path, use it. Otherwise
    * the target should have the same name as the .xdrz file.
    */
   String targetfilePath = drz.AlignmentTargetFilePath();
   if ( targetfilePath.IsEmpty() )
      targetfilePath = filePath;

   if ( GUI->StaticDataTargets_CheckBox.IsChecked() )
      return File::ChangeExtension( targetfilePath, String() );
   return File::ExtractName( targetfilePath );
}

// ----------------------------------------------------------------------------

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

void ImageIntegrationInterface::e_InputImages_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "ImageIntegration: Select Input Images" );
      d.LoadImageFilters();
      d.EnableMultipleSelections();

      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->InputImages_TreeBox );
         ImageIntegrationInstance::image_list newImages;
         for ( size_type i = 0; i < i0; ++i )
            newImages << m_instance.p_images[i];
         for ( const String& path : d.FileNames() )
            newImages << ImageIntegrationInstance::ImageItem( path );
         for ( size_type i = i0; i < m_instance.p_images.Length(); ++i )
            newImages << m_instance.p_images[i];
         m_instance.p_images = newImages;

//          for ( const String& path : d.FileNames() )
//             m_instance.p_images.Insert( m_instance.p_images.At( i0++ ), ImageIntegrationInstance::ImageItem( path ) );

         UpdateInputImagesList();
         UpdateImageSelectionButtons();
      }
   }
   else if ( sender == GUI->AddDrizzleFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "ImageIntegration: Select Drizzle Data Files" );
      d.SetFilter( FileFilter( "Drizzle Data Files", StringList() << ".xdrz" << ".drz" ) );
      d.EnableMultipleSelections();
      if ( d.Execute() )
      {
         IVector assigned( 0, int( m_instance.p_images.Length() ) );
         for ( const String& path : d.FileNames() )
         {
            String targetName = DrizzleTargetName( path );
            IVector::iterator n = assigned.Begin();
            for ( ImageIntegrationInstance::ImageItem& item : m_instance.p_images )
            {
               String name = GUI->StaticDataTargets_CheckBox.IsChecked() ?
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

         UpdateInputImagesList();

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
            MessageBox( "<p>No drizzle data files have been assigned to integration source images.</p>",
                        "ImageIntegration",
                        StdIcon::Error,
                        StdButton::Ok ).Execute();
         }
         else
         {
            if ( total < assigned.Length() || duplicates )
               MessageBox( String().Format( "<p>%d of %d drizzle data files have been assigned.<br/>"
                                            "%d duplicate assignment(s)</p>", total, assigned.Length(), duplicates ),
                           "ImageIntegration",
                           StdIcon::Warning,
                           StdButton::Ok ).Execute();

            if ( !m_instance.p_generateDrizzleData )
            {
               m_instance.p_generateDrizzleData = true;
               UpdateIntegrationControls();
            }
         }
      }
   }
   else if ( sender == GUI->ClearDrizzleFiles_PushButton )
   {
      for ( ImageIntegrationInstance::ImageItem& item : m_instance.p_images )
         item.drzPath.Clear();
      UpdateInputImagesList();

      if ( m_instance.p_generateDrizzleData )
      {
         m_instance.p_generateDrizzleData = false;
         UpdateIntegrationControls();
      }
   }
   else if ( sender == GUI->AddLocalNormalizationFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "ImageIntegration: Select Local Normalization Data Files" );
      d.SetFilter( FileFilter( "Local Normalization Data Files", ".xnml" ) );
      d.EnableMultipleSelections();
      if ( d.Execute() )
      {
         IVector assigned( 0, int( m_instance.p_images.Length() ) );
         for ( const String& path : d.FileNames() )
         {
            String targetName = LocalNormalizationTargetName( path );
            IVector::iterator n = assigned.Begin();
            for ( ImageIntegrationInstance::ImageItem& item : m_instance.p_images )
            {
               String name = GUI->StaticDataTargets_CheckBox.IsChecked() ?
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

         UpdateInputImagesList();

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
            MessageBox( "<p>No local normalization data files have been assigned to integration source images.</p>",
                        "ImageIntegration",
                        StdIcon::Error,
                        StdButton::Ok ).Execute();
         }
         else
         {
            if ( total < assigned.Length() || duplicates )
               MessageBox( String().Format( "<p>%d of %d local normalization data files have been assigned.<br/>"
                                            "%d duplicate assignment(s)</p>", total, assigned.Length(), duplicates ),
                           "ImageIntegration",
                           StdIcon::Warning,
                           StdButton::Ok ).Execute();
         }
      }
   }
   else if ( sender == GUI->ClearLocalNormalizationFiles_PushButton )
   {
      for ( ImageIntegrationInstance::ImageItem& item : m_instance.p_images )
         item.nmlPath.Clear();
      UpdateInputImagesList();
   }
   else if ( sender == GUI->SelectAll_PushButton )
   {
      GUI->InputImages_TreeBox.SelectAllNodes();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->SetReference_PushButton )
   {
      TreeBox::Node* node = GUI->InputImages_TreeBox.CurrentNode();
      if ( node != nullptr )
      {
         int idx = GUI->InputImages_TreeBox.ChildIndex( node );
         if ( idx > 0 )
         {
            ImageIntegrationInstance::ImageItem item = m_instance.p_images[idx];
            m_instance.p_images.Remove( m_instance.p_images.At( idx ) );
            item.enabled = true;
            m_instance.p_images.Insert( m_instance.p_images.Begin(), item );
            UpdateInputImagesList();
            GUI->InputImages_TreeBox.SetCurrentNode( GUI->InputImages_TreeBox[0] );
            UpdateImageSelectionButtons();
         }
      }
   }
   else if ( sender == GUI->InvertSelection_PushButton )
   {
      for ( int i = 0, n = GUI->InputImages_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->InputImages_TreeBox[i]->Select( !GUI->InputImages_TreeBox[i]->IsSelected() );
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->ToggleSelected_PushButton )
   {
      for ( int i = 0, n = GUI->InputImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->InputImages_TreeBox[i]->IsSelected() )
            m_instance.p_images[i].enabled = !m_instance.p_images[i].enabled;
      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      ImageIntegrationInstance::image_list newImages;
      for ( int i = 0, n = GUI->InputImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->InputImages_TreeBox[i]->IsSelected() )
            newImages << m_instance.p_images[i];
      m_instance.p_images = newImages;
      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      m_instance.p_images.Clear();
      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->FullPaths_CheckBox )
   {
      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_FormatHints_EditCompleted( Edit& sender )
{
   String hints = sender.Text().Trimmed();

   if ( sender == GUI->InputHints_Edit )
      m_instance.p_inputHints = hints;

   sender.SetText( hints );
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_Integration_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Combination_ComboBox )
   {
      m_instance.p_combination = itemIndex;
      UpdateIntegrationControls();
   }
   else if ( sender == GUI->Normalization_ComboBox )
   {
      m_instance.p_normalization = itemIndex;
   }
   else if ( sender == GUI->WeightMode_ComboBox )
   {
      m_instance.p_weightMode = itemIndex;
      UpdateIntegrationControls();
   }
   else if ( sender == GUI->WeightScale_ComboBox )
   {
      m_instance.p_weightScale = itemIndex;
      UpdateIntegrationControls();
   }
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_Integration_EditCompleted( Edit& sender )
{
   if ( sender == GUI->WeightKeyword_Edit )
   {
      m_instance.p_weightKeyword = sender.Text();
      m_instance.p_weightKeyword.Trim();
      m_instance.p_weightKeyword.ToUppercase();
      sender.SetText( m_instance.p_weightKeyword );
   }
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_Integration_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->AdaptiveGridSize_SpinBox )
      m_instance.p_adaptiveGridSize = value;
   else if ( sender == GUI->BufferSize_SpinBox )
      m_instance.p_bufferSizeMB = value;
   else if ( sender == GUI->StackSize_SpinBox )
      m_instance.p_stackSizeMB = value;
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_Integration_Click( Button& sender, bool checked )
{
   if ( sender == GUI->GenerateIntegratedImage_CheckBox )
   {
      m_instance.p_generateIntegratedImage = checked;
      UpdateIntegrationControls();
   }
   else if ( sender == GUI->Generate64BitResult_CheckBox )
      m_instance.p_generate64BitResult = checked;
   else if ( sender == GUI->GenerateDrizzleData_CheckBox )
      m_instance.p_generateDrizzleData = checked;
   else if ( sender == GUI->IgnoreNoiseKeywords_CheckBox )
      m_instance.p_ignoreNoiseKeywords = checked;
   else if ( sender == GUI->AdaptiveNoScale_CheckBox )
      m_instance.p_adaptiveNoScale = checked;
   else if ( sender == GUI->SubtractPedestals_CheckBox )
      m_instance.p_subtractPedestals = checked;
   else if ( sender == GUI->TruncateOnOutOfRange_CheckBox )
      m_instance.p_truncateOnOutOfRange = checked;
   else if ( sender == GUI->EvaluateNoise_CheckBox )
      m_instance.p_evaluateNoise = checked;
   else if ( sender == GUI->ClosePreviousImages_CheckBox )
      m_instance.p_closePreviousImages = checked;
   else if ( sender == GUI->AutoMemorySize_CheckBox )
   {
      m_instance.p_autoMemorySize = checked;
      UpdateIntegrationControls();
   }
   else if ( sender == GUI->UseCache_CheckBox )
      m_instance.p_useCache = checked;
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_Rejection_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->RejectionAlgorithm_ComboBox )
   {
      m_instance.p_rejection = itemIndex;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->RejectionNormalization_ComboBox )
   {
      m_instance.p_rejectionNormalization = itemIndex;
      UpdateRejectionControls();
   }
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_Rejection_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->MinMaxLow_SpinBox )
      m_instance.p_minMaxLow = value;
   else if ( sender == GUI->MinMaxHigh_SpinBox )
      m_instance.p_minMaxHigh = value;
   else if ( sender == GUI->SmallScaleLayersLow_SpinBox )
      m_instance.p_largeScaleClipLowProtectedLayers = value;
   else if ( sender == GUI->GrowthLow_SpinBox )
      m_instance.p_largeScaleClipLowGrowth = value;
   else if ( sender == GUI->SmallScaleLayersHigh_SpinBox )
      m_instance.p_largeScaleClipHighProtectedLayers = value;
   else if ( sender == GUI->GrowthHigh_SpinBox )
      m_instance.p_largeScaleClipHighGrowth = value;
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_Rejection_EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->PercentileLow_NumericControl )
      m_instance.p_pcClipLow = value;
   else if ( sender == GUI->PercentileHigh_NumericControl )
      m_instance.p_pcClipHigh = value;
   else if ( sender == GUI->SigmaLow_NumericControl )
      m_instance.p_sigmaLow = value;
   else if ( sender == GUI->SigmaHigh_NumericControl )
      m_instance.p_sigmaHigh = value;
   else if ( sender == GUI->WinsorizationCutoff_NumericControl )
      m_instance.p_winsorizationCutoff = value;
   else if ( sender == GUI->LinearFitLow_NumericControl )
      m_instance.p_linearFitLow = value;
   else if ( sender == GUI->LinearFitHigh_NumericControl )
      m_instance.p_linearFitHigh = value;
   else if ( sender == GUI->ESDOutliersFraction_NumericControl )
      m_instance.p_esdOutliersFraction = value;
   else if ( sender == GUI->ESDAlpha_NumericControl )
      m_instance.p_esdAlpha = value;
   else if ( sender == GUI->ESDLowRelaxation_NumericControl )
      m_instance.p_esdLowRelaxation = value;
   else if ( sender == GUI->CCDGain_NumericControl )
      m_instance.p_ccdGain = value;
   else if ( sender == GUI->CCDReadNoise_NumericControl )
      m_instance.p_ccdReadNoise = value;
   else if ( sender == GUI->CCDScaleNoise_NumericControl )
      m_instance.p_ccdScaleNoise = value;
   else if ( sender == GUI->RangeLow_NumericControl )
      m_instance.p_rangeLow = value;
   else if ( sender == GUI->RangeHigh_NumericControl )
      m_instance.p_rangeHigh = value;
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_Rejection_Click( Button& sender, bool checked )
{
   if ( sender == GUI->GenerateRejectionMaps_CheckBox )
   {
      m_instance.p_generateRejectionMaps = checked;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->ClipLow_CheckBox )
   {
      m_instance.p_clipLow = checked;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->ClipHigh_CheckBox )
   {
      m_instance.p_clipHigh = checked;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->ClipLowRange_CheckBox )
   {
      m_instance.p_rangeClipLow = checked;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->ClipHighRange_CheckBox )
   {
      m_instance.p_rangeClipHigh = checked;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->ReportRangeRejection_CheckBox )
      m_instance.p_reportRangeRejection = checked;
   else if ( sender == GUI->MapRangeRejection_CheckBox )
      m_instance.p_mapRangeRejection = checked;
   else if ( sender == GUI->RejectLargeScaleLow_CheckBox )
   {
      m_instance.p_largeScaleClipLow = checked;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->RejectLargeScaleHigh_CheckBox )
   {
      m_instance.p_largeScaleClipHigh = checked;
      UpdateRejectionControls();
   }
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_ROI_Check( SectionBar& sender, bool checked )
{
   if ( sender == GUI->ROI_SectionBar )
      m_instance.p_useROI = checked;
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_ROI_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ROIX0_SpinBox )
      m_instance.p_roi.x0 = value;
   else if ( sender == GUI->ROIY0_SpinBox )
      m_instance.p_roi.y0 = value;
   else if ( sender == GUI->ROIWidth_SpinBox )
      m_instance.p_roi.x1 = m_instance.p_roi.x0 + value;
   else if ( sender == GUI->ROIHeight_SpinBox )
      m_instance.p_roi.y1 = m_instance.p_roi.y0 + value;
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_ROI_Click( Button& sender, bool checked )
{
   if ( sender == GUI->SelectPreview_Button )
   {
      PreviewSelectionDialog d;
      d.SetWindowTitle( "Select ROI Preview" );
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               m_instance.p_roi = view.Window().PreviewRect( view.Id() );
               UpdateROIControls();
            }
         }
   }
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->InputImages_TreeBox.SetFixedHeight();
   else
   {
      GUI->InputImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->InputImages_TreeBox.SetMaxHeight( int_max );

      if ( GUI->InputImages_Control.IsVisible() )
         SetVariableHeight();
      else
         SetFixedHeight();
   }
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->InputImages_TreeBox.Viewport() )
      wantsFiles = true;
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->InputImages_TreeBox.Viewport() )
   {
      StringList localNormalizationFiles, drizzleFiles;
      bool recursive = IsControlOrCmdPressed();
      size_type i0 = TreeInsertionIndex( GUI->InputImages_TreeBox );
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
               m_instance.p_images.Insert( m_instance.p_images.At( i0++ ), ImageIntegrationInstance::ImageItem( file ) );
         }
      }

      for ( const String& file : localNormalizationFiles )
      {
         String targetName = LocalNormalizationTargetName( file );
         for ( ImageIntegrationInstance::ImageItem& item : m_instance.p_images )
         {
            String name = GUI->StaticDataTargets_CheckBox.IsChecked() ?
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
         for ( ImageIntegrationInstance::ImageItem& item : m_instance.p_images )
         {
            String name = GUI->StaticDataTargets_CheckBox.IsChecked() ?
                              File::ChangeExtension( item.path, String() ) : File::ExtractName( item.path );
            if ( name == targetName )
            {
               item.drzPath = file;
               break;
            }
         }
      }

      UpdateInputImagesList();
      UpdateImageSelectionButtons();

      if ( !drizzleFiles.IsEmpty() )
         if ( !m_instance.p_generateDrizzleData )
         {
            m_instance.p_generateDrizzleData = true;
            UpdateIntegrationControls();
         }
   }
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->ROI_SectionBar || sender == GUI->ROI_Control || sender == GUI->SelectPreview_Button )
      wantsView = view.IsPreview();
}

// ----------------------------------------------------------------------------

void ImageIntegrationInterface::e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->ROI_SectionBar || sender == GUI->ROI_Control || sender == GUI->SelectPreview_Button )
      if ( view.IsPreview() )
      {
         m_instance.p_useROI = true;
         m_instance.p_roi = view.Window().PreviewRect( view.Id() );
         GUI->ROI_SectionBar.ShowSection();
         UpdateROIControls();
      }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ImageIntegrationInterface::GUIData::GUIData( ImageIntegrationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Winsorization cutoff:" ) + 'M' );
   int editWidth1 = fnt.Width( String( 'M', 16 ) );
   int editWidth2 = fnt.Width( String( '0', 11 ) );
   //int spinWidth1 = fnt.Width( String( '0', 11 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   InputImages_SectionBar.SetTitle( "Input Images" );
   InputImages_SectionBar.SetSection( InputImages_Control );
   InputImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::e_ToggleSection, w );

   InputImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   InputImages_TreeBox.SetNumberOfColumns( 3 );
   InputImages_TreeBox.HideHeader();
   InputImages_TreeBox.EnableMultipleSelections();
   InputImages_TreeBox.DisableRootDecoration();
   InputImages_TreeBox.EnableAlternateRowColor();
   InputImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&ImageIntegrationInterface::e_InputImages_CurrentNodeUpdated, w );
   InputImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&ImageIntegrationInterface::e_InputImages_NodeActivated, w );
   InputImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&ImageIntegrationInterface::e_InputImages_NodeSelectionUpdated, w );
   InputImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&ImageIntegrationInterface::e_FileDrag, w );
   InputImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&ImageIntegrationInterface::e_FileDrop, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of input images.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   AddLocalNormalizationFiles_PushButton.SetText( "Add L.Norm. Files" );
   AddLocalNormalizationFiles_PushButton.SetToolTip( "<p>Associate existing local normalization data files with input images.</p>"
      "<p>Local normalization data files carry the .xnml suffix. Normally you should select .xnml files generated by "
      "the LocalNormalization tool for the same files that you are integrating.</p>" );
   AddLocalNormalizationFiles_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   ClearLocalNormalizationFiles_PushButton.SetText( "Clear L.Norm. Files" );
   ClearLocalNormalizationFiles_PushButton.SetToolTip( "<p>Remove all local normalization data files currently associated with "
      "input images.</p>"
      "<p>This removes just file associations, not the actual local normalization data files.</p>" );
   ClearLocalNormalizationFiles_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   AddDrizzleFiles_PushButton.SetText( "Add Drizzle Files" );
   AddDrizzleFiles_PushButton.SetToolTip( "<p>Associate existing drizzle data files with input images.</p>"
      "<p>Drizzle data files carry the .xdrz suffix. Normally you should select .xdrz files generated by "
      "the StarAlignment tool for the same files that you are integrating.</p>" );
   AddDrizzleFiles_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   ClearDrizzleFiles_PushButton.SetText( "Clear Drizzle Files" );
   ClearDrizzleFiles_PushButton.SetToolTip( "<p>Remove all drizzle data files currently associated with input images.</p>"
      "<p>This removes just file associations, not the actual drizzle data files.</p>" );
   ClearDrizzleFiles_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   SetReference_PushButton.SetText( "Set Reference" );
   SetReference_PushButton.SetToolTip( "<p>Make the currently selected file on the list the reference image.</p>"
      "<p>The reference image is the first one in the list of images to integrate. Its statistical properties "
      "will be taken as the basis to calculate normalization parameters and relative combination weights for the "
      "rest of integrated images.</p>" );
   SetReference_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all input images.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of input images.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected input images.</p>"
      "<p>Disabled input images will be ignored during the integration process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected input images.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of input images.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   StaticDataTargets_CheckBox.SetText( "Static data targets" );
   StaticDataTargets_CheckBox.SetToolTip( "<p>When assigning drizzle and/or local normalization data files to target "
      "images, take into account full file paths stored in .xdrz and .xnml files. This allows you to integrate images "
      "with duplicate file names on different directories. However, by enabling this option your data set gets tied to "
      "specific locations on the local filesystem. When this option is disabled (the default state), only file names are "
      "used to associate target images with .xdrz and .xnml files, which allows you to move your images freely throughout "
      "the filesystem, including the possibility to migrate them to different machines.</p>"
      "<p>Changes to this option will come into play the next time you associate .xdrz and/or .xnml files with target "
      "images. Existing file associations are not affected.</p>");
   //StaticDataTargets_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for input image files.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_InputImages_Click, w );

   InputButtons_Sizer.SetSpacing( 4 );
   InputButtons_Sizer.Add( AddFiles_PushButton );
   InputButtons_Sizer.Add( AddLocalNormalizationFiles_PushButton );
   InputButtons_Sizer.Add( ClearLocalNormalizationFiles_PushButton );
   InputButtons_Sizer.Add( AddDrizzleFiles_PushButton );
   InputButtons_Sizer.Add( ClearDrizzleFiles_PushButton );
   InputButtons_Sizer.Add( SetReference_PushButton );
   InputButtons_Sizer.Add( SelectAll_PushButton );
   InputButtons_Sizer.Add( InvertSelection_PushButton );
   InputButtons_Sizer.Add( ToggleSelected_PushButton );
   InputButtons_Sizer.Add( RemoveSelected_PushButton );
   InputButtons_Sizer.Add( Clear_PushButton );
   InputButtons_Sizer.Add( StaticDataTargets_CheckBox );
   InputButtons_Sizer.Add( FullPaths_CheckBox );
   InputButtons_Sizer.AddStretch();

   InputImages_Sizer.SetSpacing( 4 );
   InputImages_Sizer.Add( InputImages_TreeBox, 100 );
   InputImages_Sizer.Add( InputButtons_Sizer );

   InputImages_Control.SetSizer( InputImages_Sizer );

   //

   FormatHints_SectionBar.SetTitle( "Format Hints" );
   FormatHints_SectionBar.SetSection( FormatHints_Control );
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::e_ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings "
      "for image files used by specific processes. In ImageIntegration, input hints change the way input images "
      "of some particular file formats are loaded during the integration process. There are no output hints in "
      "ImageIntegration since this process does not write images to disk files.</p>"
      "<p>For example, you can use the \"raw cfa\" hints to force the RAW format to load pure raw images without "
      "applying any demosaicing, interpolation, white balance, or black point correction. You can also specify the "
      "\"lower-range\" and \"upper-range\" hints to load floating point FITS and TIFF files generated by other "
      "applications that don't use PixInsight's normalized [0,1] range. Most standard file format modules support "
      "hints; each format supports a number of input and/or output hints that you can use for different purposes "
      "with tools that give you access to format hints.</p>";

   InputHints_Label.SetText( "Input hints:" );
   InputHints_Label.SetFixedWidth( labelWidth1 );
   InputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InputHints_Label.SetToolTip( hintsToolTip );

   InputHints_Edit.SetToolTip( hintsToolTip );
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageIntegrationInterface::e_FormatHints_EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   FormatHints_Sizer.SetSpacing( 4 );
   FormatHints_Sizer.Add( InputHints_Sizer );

   FormatHints_Control.SetSizer( FormatHints_Sizer );

   //

   Integration_SectionBar.SetTitle( "Image Integration" );
   Integration_SectionBar.SetSection( Integration_Control );
   Integration_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::e_ToggleSection, w );

   const char* combinationToolTip = "<p>Select a pixel combination operation.</p>"
      "<p><b>Average</b> combination provides the best signal-to-noise ratio in the integrated result.</p>"
      "<p><b>Median</b> combination provides implicit robust rejection of outliers, but at the cost of more noise.</p>";

   Combination_Label.SetText( "Combination:" );
   Combination_Label.SetFixedWidth( labelWidth1 );
   Combination_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Combination_Label.SetToolTip( combinationToolTip );

   Combination_ComboBox.AddItem( "Average" );
   Combination_ComboBox.AddItem( "Median" );
   Combination_ComboBox.AddItem( "Minimum" );
   Combination_ComboBox.AddItem( "Maximum" );
   Combination_ComboBox.SetToolTip( combinationToolTip );
   Combination_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::e_Integration_ItemSelected, w );

   Combination_Sizer.SetSpacing( 4 );
   Combination_Sizer.Add( Combination_Label );
   Combination_Sizer.Add( Combination_ComboBox );
   Combination_Sizer.AddStretch();

   const char* normalizationToolTip = "<p>Image normalization for combination.</p>"
      "<p>If one of these options is selected, ImageIntegration will normalize and scale all the input images "
      "before combining them to generate the output image. Note that these normalization and scaling operations "
      "are independent from the similar operations performed before pixel rejection.</p>"

      "<p>Normalization matches mean background values. This can be done either as an <b>additive</b> or as a "
      "<b>multiplicative</b> process. In general, both ways lead to very similar results, but multiplicative "
      "normalization should be used to integrate images that are to be further combined or applied by "
      "multiplication or division, such as flat fields.</p>"

      "<p><b>Scaling</b> matches dispersion. This works as a sort of <i>automatic weighting</i> to combine the input "
      "images, and tends to improve the SNR of the result, especially for images with different overall illumination.</p>"

      "<p><b>Local normalization</b> applies per-pixel linear normalization functions previously calculated and stored "
      "in XNML files (.xnml file name suffix) by the LocalNormalization process. To apply this option, existing XNML "
      "files must be associated with input integration files. This option should normally be used when local normalization "
      "is also used for pixel rejection.</p>"

      "<p><b>Adaptive normalization</b> applies per-pixel additive/scaling normalization functions, computed by surface "
      "spline interpolation from matrices of location and two-sided scale estimates calculated for each integrated image. "
      "This algorithm is an alternative to local normalization that does not require auxiliary files. See also the "
      "information provided for adaptive rejection normalization.</p>"

      "<p>The default option is additive normalization with scaling.</p>";

   Normalization_Label.SetText( "Normalization:" );
   Normalization_Label.SetFixedWidth( labelWidth1 );
   Normalization_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Normalization_Label.SetToolTip( normalizationToolTip );

   Normalization_ComboBox.AddItem( "No normalization" );
   Normalization_ComboBox.AddItem( "Additive" );
   Normalization_ComboBox.AddItem( "Multiplicative" );
   Normalization_ComboBox.AddItem( "Additive with scaling" );
   Normalization_ComboBox.AddItem( "Multiplicative with scaling" );
   Normalization_ComboBox.AddItem( "Local normalization" );
   Normalization_ComboBox.AddItem( "Adaptive normalization" );
   Normalization_ComboBox.SetToolTip( normalizationToolTip );
   Normalization_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::e_Integration_ItemSelected, w );

   Normalization_Sizer.SetSpacing( 4 );
   Normalization_Sizer.Add( Normalization_Label );
   Normalization_Sizer.Add( Normalization_ComboBox );
   Normalization_Sizer.AddStretch();

   const char* adaptiveGridToolTip =
      "<p>This parameter defines the number of samples used to interpolate per-pixel statistical location and scale "
      "estimates for adaptive normalization. The more samples, the more locally adaptive the normalization will be.</p>"

      "<p>A more adaptive normalization attempts to compensate for additive and multiplicative differences between "
      "integrated subframes at a smaller scale. A less adaptive normalization acts at a larger scale; for example, the "
      "default <i>additive with scaling</i> output normalization (or the <i>scale + zero offset</i> normalization for "
      "rejection) uses a single sample of location and scale, and hence can be regarded as a special case of adaptive "
      "normalization working at the scale of the entire image.</p>"

      "<p>The value specified with this parameter determines the number of columns or rows (according to the largest "
      "dimension of the image) in the matrix of regularly distributed samples of location and scale computed from pixels "
      "of each integrated subframe. This value will be used equally for rejection and output adaptive normalizations.</p>"

      "<p><b>Warning:</b> Specifying a value that is too high for this parameter may lead to generation of artifacts in "
      "the integrated image. This is because too small sampling regions tend to map small-scale local variations that may "
      "not be representative of significant statistical variations in the image. Always watch rejection maps closely when "
      "using adaptive normalization. <b>Local normalizations must <i>never</i> be applied blindly without analyzing the "
      "results.</b></p>";

   AdaptiveGridSize_Label.SetText( "Adaptive grid size:" );
   AdaptiveGridSize_Label.SetFixedWidth( labelWidth1 );
   AdaptiveGridSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   AdaptiveGridSize_Label.SetToolTip( adaptiveGridToolTip );

   AdaptiveGridSize_SpinBox.SetRange( int( TheIIAdaptiveGridSizeParameter->MinimumValue() ), int( TheIIAdaptiveGridSizeParameter->MaximumValue() ) );
   AdaptiveGridSize_SpinBox.SetToolTip( adaptiveGridToolTip );
   AdaptiveGridSize_SpinBox.SetFixedWidth( editWidth2 );
   AdaptiveGridSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_Integration_SpinValueUpdated, w );

   AdaptiveGridSize_Sizer.SetSpacing( 4 );
   AdaptiveGridSize_Sizer.Add( AdaptiveGridSize_Label );
   AdaptiveGridSize_Sizer.Add( AdaptiveGridSize_SpinBox );
   AdaptiveGridSize_Sizer.AddStretch();

   AdaptiveNoScale_CheckBox.SetText( "No adaptive scale components" );
   AdaptiveNoScale_CheckBox.SetToolTip( "<p>Apply only the location (additive) components of adaptive normalization "
      "functions, and use global two-sided scale estimates instead of adaptive ones.</p>"
      "<p>This option can be used to limit adaptive normalization to correction of purely additive gradients. Without "
      "multiplicative components, adaptive normalization can be more tolerant of excessive grid size parameter values. "
      "However, in difficult cases with strong gradients, scale components can be necessary to achieve a sufficiently "
      "accurate normalization.</p>" );
   AdaptiveNoScale_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Integration_Click, w );

   AdaptiveNoScale_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AdaptiveNoScale_Sizer.Add( AdaptiveNoScale_CheckBox );
   AdaptiveNoScale_Sizer.AddStretch();

   const char* weightModeToolTip = "<p>Image weighting criterion.</p>"
      "<p>Exposure times will be retrieved from standard EXPTIME and EXPOSURE FITS keywords (in that order).</p>"

      "<p>The <b>noise evaluation</b> option uses multiscale noise evaluation techniques to compute relative "
      "SNR values that tend to minimize mean squared error in the integrated image. This is usually the most "
      "robust approach for automatic image weighting, and therefore the default option.</p>"

      "<p>Noise estimates are obtained either from cached data, if available, or from NOISExx FITS keywords, if "
      "they are present in the images. Otherwise ImageIntegration will compute Gaussian noise estimates using the "
      "multiresolution support (MRS) noise evaluation algorithm.</p>"

      "<p>The <b>average signal strength</b> option attempts to derive relative exposures directly from statistical "
      "properties of the data. This option is less robust and may not work if some images have additive "
      "illumination variations, such as sky gradients.</p>"

      "<p>If you select the <b>FITS keyword</b> option, please specify the name of a FITS keyword to retrieve "
      "image weights. The specified keyword must be present in all input images and its value must be of numeric "
      "type and positive.</p>";

   WeightMode_Label.SetText( "Weights:" );
   WeightMode_Label.SetFixedWidth( labelWidth1 );
   WeightMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WeightMode_Label.SetToolTip( weightModeToolTip );

   WeightMode_ComboBox.AddItem( "Don't care (all weights = 1)" );
   WeightMode_ComboBox.AddItem( "Exposure time" );
   WeightMode_ComboBox.AddItem( "Noise evaluation" );
   WeightMode_ComboBox.AddItem( "Average signal strength" );
   WeightMode_ComboBox.AddItem( "Median value" );
   WeightMode_ComboBox.AddItem( "Average value" );
   WeightMode_ComboBox.AddItem( "FITS keyword" );
   WeightMode_ComboBox.SetToolTip( weightModeToolTip );
   WeightMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::e_Integration_ItemSelected, w );

   WeightMode_Sizer.SetSpacing( 4 );
   WeightMode_Sizer.Add( WeightMode_Label );
   WeightMode_Sizer.Add( WeightMode_ComboBox );
   WeightMode_Sizer.AddStretch();

   const char* weightKeywordToolTip = "<p>Custom FITS keyword to retrieve image weights.</p>"
      "<p>This is the name of a FITS keyword that will be used to retrieve image weights, if the "
      "<i>FITS keyword</i> option has been selected as the weighting criterion.</p>";

   WeightKeyword_Label.SetText( "Weight keyword:" );
   WeightKeyword_Label.SetFixedWidth( labelWidth1 );
   WeightKeyword_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WeightKeyword_Label.SetToolTip( weightKeywordToolTip );

   WeightKeyword_Edit.SetMinWidth( editWidth1 );
   WeightKeyword_Edit.SetToolTip( weightKeywordToolTip );
   WeightKeyword_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageIntegrationInterface::e_Integration_EditCompleted, w );

   WeightKeyword_Sizer.SetSpacing( 4 );
   WeightKeyword_Sizer.Add( WeightKeyword_Label );
   WeightKeyword_Sizer.Add( WeightKeyword_Edit );
   WeightKeyword_Sizer.AddStretch();

   const char* weightScaleToolTip = "<p>Estimator of scale used for image weighting and normalization.</p>"
      "<p>Statistical estimates of scale, or dispersion, are necessary for most image weighting, output and "
      "rejection normalization/scaling algorithms. Robust and efficient scale estimators are crucial to make the data "
      "from different images statistically compatible.</p>"

      "<p>The <b>average absolute deviation from the median</b> is robustified by trimming all pixel samples outside "
      "the [0.00002,0.99998] range, which excludes cold and hot pixels, as well as saturated pixels and bright spurious "
      "features (cosmics, etc).</p>"

      "<p>The <b>median absolute deviation from the median (MAD)</b> is a well-known robust estimator of scale. Although "
      "it has the best possible breakdown point (50%), its Gaussian efficiency is rather low (37%).</p>"

      "<p>The square root of the <b>biweight midvariance</b> is a robust estimator of scale with a 50% breakdown point "
      "(as good as MAD) and high efficiency with respect to several distributions (about 87%). This is the default scale "
      "estimator in current versions of the ImageIntegration tool.</p>"

      "<p>All scale estimators have been implemented as two-sided or bilateral algorithms, where estimates of scale are "
      "computed separately for low and high pixel samples, which are samples with values less than or equal and greater "
      "than the computed estimate of location, respectively, for each image channel. This helps to achieve a more robust "
      "and efficient normalization for pixel rejection, especially for input images with asymmetric or skewed "
      "distributions.</p>"

      "<p>In all cases the median is used as a robust estimator of location. This makes sense for deep-sky images "
      "because the median closely represents the mean background of the image in most cases.</p>";

   WeightScale_Label.SetText( "Scale estimator:" );
   WeightScale_Label.SetFixedWidth( labelWidth1 );
   WeightScale_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WeightScale_Label.SetToolTip( weightScaleToolTip );

   WeightScale_ComboBox.AddItem( "Average absolute deviation from the median" );
   WeightScale_ComboBox.AddItem( "Median absolute deviation from the median (MAD)" );
   WeightScale_ComboBox.AddItem( "Biweight midvariance (BWMV)" );
   WeightScale_ComboBox.SetToolTip( weightScaleToolTip );
   WeightScale_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::e_Integration_ItemSelected, w );

   WeightScale_Sizer.SetSpacing( 4 );
   WeightScale_Sizer.Add( WeightScale_Label );
   WeightScale_Sizer.Add( WeightScale_ComboBox );
   WeightScale_Sizer.AddStretch();

   IgnoreNoiseKeywords_CheckBox.SetText( "Ignore noise keywords" );
   IgnoreNoiseKeywords_CheckBox.SetToolTip( "<p>Ignore NOISExx keywords. Always compute new noise estimates for "
      "input images, or retrieve them from the cache (if the cache is enabled and valid cached noise estimates are "
      "available).</p>" );
   IgnoreNoiseKeywords_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Integration_Click, w );

   IgnoreNoiseKeywords_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   IgnoreNoiseKeywords_Sizer.Add( IgnoreNoiseKeywords_CheckBox );
   IgnoreNoiseKeywords_Sizer.AddStretch();

   GenerateIntegratedImage_CheckBox.SetText( "Generate integrated image" );
   GenerateIntegratedImage_CheckBox.SetToolTip( "<p>Generate the result of the integration process as a new "
      "image window.</p>"
      "<p>This option should be enabled for normal use. If you disable it, the integrated image won't be "
      "generated at the end of the process. You can disable this option to save a relatively modest amount of "
      "computation time and resources while you are trying out rejection parameters. To evaluate the quality "
      "of pixel rejection, you normally are only interested in rejection pixel counts and/or rejection maps.</p>" );
   GenerateIntegratedImage_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Integration_Click, w );

   GenerateIntegratedImage_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateIntegratedImage_Sizer.Add( GenerateIntegratedImage_CheckBox );
   GenerateIntegratedImage_Sizer.AddStretch();

   Generate64BitResult_CheckBox.SetText( "Generate a 64-bit result image" );
   Generate64BitResult_CheckBox.SetToolTip( "<p>If this option is selected, ImageIntegration will generate a "
      "64-bit floating point result image. Otherwise the integration result will be generated as a 32-bit "
      "floating point image.</p>" );
   Generate64BitResult_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Integration_Click, w );

   Generate64BitResult_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Generate64BitResult_Sizer.Add( Generate64BitResult_CheckBox );
   Generate64BitResult_Sizer.AddStretch();

   GenerateDrizzleData_CheckBox.SetText( "Generate drizzle data" );
   GenerateDrizzleData_CheckBox.SetToolTip( "<p>Generate an XML drizzle data file for each integrated image.</p>"
      "<p>Drizzle data files contain geometrical projection parameters, pixel rejection maps, statistical data "
      "and metadata for the DrizzleIntegration tool. StarAlignment creates drizzle files with projection data, "
      "and the ImageIntegration tool appends rejection and statistical data to the same files. Drizzle files carry "
      "the .xdrz file suffix.</p>" );
   GenerateDrizzleData_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Integration_Click, w );

   GenerateDrizzleData_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateDrizzleData_Sizer.Add( GenerateDrizzleData_CheckBox );
   GenerateDrizzleData_Sizer.AddStretch();

   SubtractPedestals_CheckBox.SetText( "Subtract pedestals" );
   SubtractPedestals_CheckBox.SetToolTip( "<p>Subtract PEDESTAL keyword values, if present in input images.</p>"
      "<p>Some applications add small positive values (typically about 100 DN) systematically to calibrated light frames. "
      "<p>These small <i>pedestals</i> should be subtracted from source integration pixels to refer all input data to the "
      "same zero point consistently.</p>"
      "<p>This option should only be enabled for integration of calibrated light frames with PEDESTAL keywords. This is "
      "typically the case if you have no access to uncalibrated raw data, and are forced to work with data already "
      "calibrated in other applications.</p>"
      "<p>This option should be <i>disabled</i> for integration of raw bias and dark frames, since in these cases existing "
      "PEDESTAL keywords should be preserved in the generated master bias and dark frames. These pedestals will be "
      "subtracted automatically by the ImageCalibration process for calibration of flat and light frames.</p>" );
   SubtractPedestals_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Integration_Click, w );

   SubtractPedestals_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SubtractPedestals_Sizer.Add( SubtractPedestals_CheckBox );
   SubtractPedestals_Sizer.AddStretch();

   TruncateOnOutOfRange_CheckBox.SetText( "Truncate on out-of-range" );
   TruncateOnOutOfRange_CheckBox.SetToolTip( "<p>If the output integrated image has saturated pixel samples out of "
      "the nominal [0,1] range, truncate them instead of rescaling the whole image.</p>"
      "<p>No out-of-range values should occur after integration of a well-calibrated data set under normal conditions. "
      "However, sometimes saturated pixels may lead to out-of-range values after output normalization, depending on "
      "the frame selected as integration reference.</p>"
      "<p>When this happens, the best option for integration of light or science frames is a linear rescaling, which "
      "preserves all of the integrated data. However, in some cases altering all pixel values is not admissible, so a "
      "rescaling operation is not applicable. This is the case for integration of flat frames, where truncation is the "
      "only option available to preserve the correct illumination profile in the integrated master flat frame.</p>" );
   TruncateOnOutOfRange_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Integration_Click, w );

   TruncateOnOutOfRange_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   TruncateOnOutOfRange_Sizer.Add( TruncateOnOutOfRange_CheckBox );
   TruncateOnOutOfRange_Sizer.AddStretch();

   EvaluateNoise_CheckBox.SetText( "Evaluate noise" );
   EvaluateNoise_CheckBox.SetToolTip( "<p>Evaluate the standard deviation of Gaussian noise for the final "
      "integrated image.</p>"
      "<p>Noise evaluation uses wavelet-based techniques and provides estimates to within 1% accuracy. "
      "This option is useful to compare the results of different integration procedures. For example, by "
      "comparing noise estimates you can know which image normalization and weighting criteria lead to the "
      "best result in terms of signal-to-noise ratio improvement.</p>" );
   EvaluateNoise_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Integration_Click, w );

   EvaluateNoise_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   EvaluateNoise_Sizer.Add( EvaluateNoise_CheckBox );
   EvaluateNoise_Sizer.AddStretch();

   ClosePreviousImages_CheckBox.SetText( "Close previous images" );
   ClosePreviousImages_CheckBox.SetToolTip( "<p>Select this option to close existing integration and rejection "
      "map images before running a new integration process. This is useful to avoid accumulation of multiple "
      "results on the workspace, when the same integration is being tested repeatedly.</p>" );
   ClosePreviousImages_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Integration_Click, w );

   ClosePreviousImages_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClosePreviousImages_Sizer.Add( ClosePreviousImages_CheckBox );
   ClosePreviousImages_Sizer.AddStretch();

   AutoMemorySize_CheckBox.SetText( "Automatic buffer sizes" );
   AutoMemorySize_CheckBox.SetToolTip( "<p>Use the largest possible buffer and stack sizes, calculated "
      "automatically from the amount of physical memory currently available to PixInsight.</p>"
      "<p>Usually this is the best option to optimize image integration performance, but be aware that for large "
      "data sets and low-memory machines (relative to the total size of the data) the process may use too many "
      "system resources. If you want finer control on system resources usage, disable this option and tweak the "
      "values of the <i>buffer size</i> and <i>stack size</i> parameters.</p>" );
   AutoMemorySize_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Integration_Click, w );

   AutoMemorySize_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AutoMemorySize_Sizer.Add( AutoMemorySize_CheckBox );
   AutoMemorySize_Sizer.AddStretch();

   const char* bufferSizeToolTip =
      "<p>This parameter defines the size of the working buffers used to read pixel rows. There is an "
      "independent buffer per input image. A reasonably large buffer size will improve performance by "
      "minimizing disk reading operations. The default value of 16 MiB is usually quite appropriate.</p>"
      "<p>Decrease this parameter if you experience out-of-memory errors during integration. This may "
      "be necessary for integration of large image sets on systems with low memory resources. The "
      "minimum value is zero, which will use a single row of pixels per input image.</p>";

   BufferSize_Label.SetText( "Buffer size (MiB):" );
   BufferSize_Label.SetFixedWidth( labelWidth1 );
   BufferSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BufferSize_Label.SetToolTip( bufferSizeToolTip );

   BufferSize_SpinBox.SetRange( int( TheIIBufferSizeParameter->MinimumValue() ), int( TheIIBufferSizeParameter->MaximumValue() ) );
   BufferSize_SpinBox.SetToolTip( bufferSizeToolTip );
   BufferSize_SpinBox.SetFixedWidth( editWidth2 );
   BufferSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_Integration_SpinValueUpdated, w );

   BufferSize_Sizer.SetSpacing( 4 );
   BufferSize_Sizer.Add( BufferSize_Label );
   BufferSize_Sizer.Add( BufferSize_SpinBox );
   BufferSize_Sizer.AddStretch();

   const char* stackSizeToolTip =
      "<p>This is the size of the working integration stack structure. In general, the larger this "
      "parameter, the better the performance, especially on multiprocessor/multicore systems.</p>"

      "<p>The best performance is achieved when the whole set of integrated pixels can be loaded at "
      "once in the integration stack. For this to happen, the following conditions must be true:</p>"

      "<ul>"
      "<li><b>Buffer size</b> must be large enough as to allow loading an input file in 32-bit floating "
      "point format completely with a single file read operation.</li>"
      "<li><b>Stack size</b> must be larger than or equal to W*H*(12*N + 4), where W and H are the image "
      "width and height in pixels, respectively, and N is the number of integrated images. For linear fit "
      "clipping rejection, replace 4 with 8 in the above equation.</li>"
      "</ul>"

      "<p>Note that this may require a large amount of RAM available for relatively large image sets. "
      "As an example, the default stack size of 1024 (1 GiB) is sufficient to integrate 20 2048x2048 monochrome "
      "images optimally with the default buffer size of 16 MiB. With a stack size of 4 GiB and a buffer size of "
      "64 MiB you could integrate 20 4Kx4K monochrome images with optimum performance.</p>";

   StackSize_Label.SetText( "Stack size (MiB):" );
   StackSize_Label.SetFixedWidth( labelWidth1 );
   StackSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StackSize_Label.SetToolTip( stackSizeToolTip );

   // Limit to one half the parameter's maximum (500 GiB) to keep the SpinBox size within editWidth2.
   StackSize_SpinBox.SetRange( int( TheIIStackSizeParameter->MinimumValue() ), int( TheIIStackSizeParameter->MaximumValue() ) >> 1 );
   StackSize_SpinBox.SetToolTip( stackSizeToolTip );
   StackSize_SpinBox.SetFixedWidth( editWidth2 );
   StackSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_Integration_SpinValueUpdated, w );

   StackSize_Sizer.SetSpacing( 4 );
   StackSize_Sizer.Add( StackSize_Label );
   StackSize_Sizer.Add( StackSize_SpinBox );
   StackSize_Sizer.AddStretch();

   UseCache_CheckBox.SetText( "Use file cache" );
   UseCache_CheckBox.SetToolTip( "<p>By default, ImageIntegration generates and uses a dynamic cache of "
      "working image parameters, including pixel statistics and normalization data. This cache greatly "
      "improves performance when the same images are being integrated several times, for example to find "
      "optimal pixel rejection parameters.</p>"

      "<p>Disable this option if for some reason you don't want to use the cache. This will force "
      "recalculation of all statistical data required for normalization, which involves loading all "
      "integrated image files from disk.</p>"

      "<p>The file cache can also be <i>persistent</i> across PixInsight core application executions. The "
      "persistent cache and its options can be controlled with the ImageIntegration Preferences dialog.</p>");
   UseCache_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Integration_Click, w );

   Cache_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Cache_Sizer.Add( UseCache_CheckBox );
   Cache_Sizer.AddStretch();

   Integration_Sizer.SetSpacing( 4 );
   Integration_Sizer.Add( Combination_Sizer );
   Integration_Sizer.Add( Normalization_Sizer );
   Integration_Sizer.Add( AdaptiveGridSize_Sizer );
   Integration_Sizer.Add( AdaptiveNoScale_Sizer );
   Integration_Sizer.Add( WeightMode_Sizer );
   Integration_Sizer.Add( WeightKeyword_Sizer );
   Integration_Sizer.Add( WeightScale_Sizer );
   Integration_Sizer.Add( IgnoreNoiseKeywords_Sizer );
   Integration_Sizer.Add( GenerateIntegratedImage_Sizer );
   Integration_Sizer.Add( Generate64BitResult_Sizer );
   Integration_Sizer.Add( GenerateDrizzleData_Sizer );
   Integration_Sizer.Add( SubtractPedestals_Sizer );
   Integration_Sizer.Add( TruncateOnOutOfRange_Sizer );
   Integration_Sizer.Add( EvaluateNoise_Sizer );
   Integration_Sizer.Add( ClosePreviousImages_Sizer );
   Integration_Sizer.Add( AutoMemorySize_Sizer );
   Integration_Sizer.Add( BufferSize_Sizer );
   Integration_Sizer.Add( StackSize_Sizer );
   Integration_Sizer.Add( Cache_Sizer );

   Integration_Control.SetSizer( Integration_Sizer );

   //

   Rejection1_SectionBar.SetTitle( "Pixel Rejection (1)" );
   Rejection1_SectionBar.SetSection( Rejection1_Control );
   Rejection1_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::e_ToggleSection, w );

   const char* rejectionAlgorithmToolTip = "<p>Pixel rejection algorithm</p>"

      "<p>The <b>iterative sigma clipping</b> algorithm is usually a good option to integrate more than "
      "10 or 15 images. Keep in mind that for sigma clipping to work, the standard deviation must be a good "
      "estimate of dispersion, which requires a sufficient number of pixels per stack (the more images the "
      "better).</p>"

      "<p><b>Winsorized sigma clipping</b> is similar to the normal sigma clipping algorithm, but uses a "
      "special iterative procedure based on Huber's method of robust estimation of parameters through "
      "<i>Winsorization</i>. This algorithm can yield superior rejection of outliers with better preservation "
      "of significant data for large sets of images.</p>"

      "<p><b>Linear fit clipping</b> fits each pixel stack to a straigtht line. The linear fit is optimized "
      "in the twofold sense of minimizing average absolute deviation and maximizing inliers. This rejection "
      "algorithm is more robust than sigma clipping for large sets of images, especially in presence of "
      "additive sky gradients of varying intensity and spatial distribution. For the best performance, use "
      "this algorithm for large sets of at least 15 images. Five images is the minimum required.</p>"

      "<p>The <b>Generalized Extreme Studentized Deviate (ESD) Test</b> rejection algorithm is an implementation "
      "of the method described by Bernard Rosner in his 1983 paper <i>Percentage Points for a Generalized ESD "
      "Many-Outlier procedure</i>, adapted to the image integration task. The ESD algorithm assumes that each "
      "pixel stack, in absence of outliers, follows an approximately normal (Gaussian) distribution. It aims at "
      "avoiding <i>masking</i>, a serious issue that occurs when an outlier goes undetected because its value is "
      "similar to another outlier. The performance of this algorithm can be excellent for large data sets of 25 "
      "or more images, and especially for very large sets of 50 or more frames. The minimum required is 3 images.</p>"

      "<p><b>Percentile clipping</b> rejection is excellent to integrate reduced sets of images, such as "
      "3 to 6 images. This is a single-pass algorithm that rejects pixels outside a fixed range of values "
      "relative to the median of each pixel stack.</p>"

      "<p><b>Averaged iterative sigma clipping</b> is intended for sets of 10 or more images. This algorithm "
      "tries to derive the gain of an ideal CCD detector from existing pixel data, assuming zero readout noise, "
      "then uses a Poisson noise model to perform rejection. For large sets of images however, sigma clipping "
      "tends to be superior.</p>"

      "<p>The <b>min/max</b> method can be used to ensure rejection of extreme values. Min/max performs an "
      "unconditional rejection of a fixed number of pixels from each stack, without any statistical basis. "
      "Rejection methods based on robust statistics, such as percentile, Winsorized sigma clipping, linear "
      "fitting and averaged sigma clipping are in general preferable.</p>"

      "<p>Finally, the <b>CCD noise model</b> algorithm requires unmodified (uncalibrated) data and accurate "
      "sensor parameters. This rejection algorithm can only be useful to integrate calibration images (bias "
      "frames, dark frames and flat fields).</p>";

   RejectionAlgorithm_Label.SetText( "Rejection algorithm:" );
   RejectionAlgorithm_Label.SetFixedWidth( labelWidth1 );
   RejectionAlgorithm_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RejectionAlgorithm_Label.SetToolTip( rejectionAlgorithmToolTip );

   RejectionAlgorithm_ComboBox.AddItem( "No rejection" );
   RejectionAlgorithm_ComboBox.AddItem( "Min/Max" );
   RejectionAlgorithm_ComboBox.AddItem( "Percentile Clipping" );
   RejectionAlgorithm_ComboBox.AddItem( "Sigma Clipping" );
   RejectionAlgorithm_ComboBox.AddItem( "Winsorized Sigma Clipping" );
   RejectionAlgorithm_ComboBox.AddItem( "Averaged Sigma Clipping" );
   RejectionAlgorithm_ComboBox.AddItem( "Linear Fit Clipping" );
   RejectionAlgorithm_ComboBox.AddItem( "CCD Noise Model" );
   RejectionAlgorithm_ComboBox.AddItem( "Generalized Extreme Studentized Deviate (ESD) Test" );
   RejectionAlgorithm_ComboBox.SetToolTip( rejectionAlgorithmToolTip );
   RejectionAlgorithm_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::e_Rejection_ItemSelected, w );

   RejectionAlgorithm_Sizer.SetSpacing( 4 );
   RejectionAlgorithm_Sizer.Add( RejectionAlgorithm_Label );
   RejectionAlgorithm_Sizer.Add( RejectionAlgorithm_ComboBox );
   RejectionAlgorithm_Sizer.AddStretch();

   const char* rejectionNormalizationToolTip = "<p>Image normalization for pixel rejection.</p>"
      "<p>Normalization is essential to perform a correct pixel rejection, since it ensures that the data "
      "from all integrated images are compatible in terms of their statistical distribution (mean background "
      "values and dispersion).</p>"

      "<p><b>Scale + zero offset</b> matches mean background values and dispersion. This involves "
      "multiplicative and additive transformations. This is the default rejection normalization method that "
      "should be used to integrate calibrated images.</p>"

      "<p><b>Equalize fluxes</b> simply matches the main histogram peaks of all images prior to pixel rejection. "
      "This is done by multiplication with the ratio of the reference median to the median of each integrated image. "
      "This is the method of choice to integrate sky flat fields, since in this case trying to match dispersion "
      "does not make sense, due to the irregular illumination distribution. For the same reason, this type of "
      "rejection normalization can also be useful to integrate uncalibrated images, or images suffering from "
      "strong gradients.</p>"

      "<p><b>Local normalization</b> applies per-pixel linear normalization functions previously calculated and stored "
      "in XNML files (.xnml file name suffix) by the LocalNormalization process. To apply this option, existing XNML "
      "files must be associated with input integration files. Local normalization can be the most accurate and robust "
      "option for rejection normalization of data sets with large variations, such as those caused by varying "
      "acquisition conditions, strong gradients with different orientations and intensities, or data acquired with "
      "different instruments. In such cases, global normalization techniques based on statistical properties of the "
      "whole image may be unable to make the data statistically compatible with the required locality, leading to "
      "inaccurate pixel rejection.</p>"

      "<p><b>Adaptive normalization</b> applies per-pixel additive/scaling normalization functions, computed by surface "
      "spline interpolation from matrices of location and two-sided scale estimates calculated for each integrated image. "
      "This algorithm is an alternative to local normalization that does not require auxiliary files. Adaptive "
      "normalization is intended to solve the same problems described for local normalization: data with strong local "
      "variations, especially strong gradients of varying orientations and intensities. Although local normalization is "
      "much more accurate and can be necessary to solve difficult problems, adaptive normalization is much easier to use "
      "and works well in most practical cases.</p>";

   RejectionNormalization_Label.SetText( "Normalization:" );
   RejectionNormalization_Label.SetFixedWidth( labelWidth1 );
   RejectionNormalization_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RejectionNormalization_Label.SetToolTip( rejectionNormalizationToolTip );

   RejectionNormalization_ComboBox.AddItem( "No normalization" );
   RejectionNormalization_ComboBox.AddItem( "Scale + zero offset" );
   RejectionNormalization_ComboBox.AddItem( "Equalize fluxes" );
   RejectionNormalization_ComboBox.AddItem( "Local normalization" );
   RejectionNormalization_ComboBox.AddItem( "Adaptive normalization" );
   RejectionNormalization_ComboBox.SetToolTip( rejectionNormalizationToolTip );
   RejectionNormalization_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::e_Rejection_ItemSelected, w );

   RejectionNormalization_Sizer.SetSpacing( 4 );
   RejectionNormalization_Sizer.Add( RejectionNormalization_Label );
   RejectionNormalization_Sizer.Add( RejectionNormalization_ComboBox );
   RejectionNormalization_Sizer.AddStretch();

   GenerateRejectionMaps_CheckBox.SetText( "Generate rejection maps" );
   GenerateRejectionMaps_CheckBox.SetToolTip( "<p>Rejection maps have pixel values proportional to the "
      "number of rejected pixels for each output pixel. Low and high rejected pixels are represented as two "
      "separate rejection maps, which are generated as 32-bit floating point images.</p>"
      "<p>Use rejection maps along with console rejection statistics to evaluate performance of pixel "
      "rejection parameters.</p>" );
   GenerateRejectionMaps_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Rejection_Click, w );

   GenerateRejectionMaps_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateRejectionMaps_Sizer.Add( GenerateRejectionMaps_CheckBox );
   GenerateRejectionMaps_Sizer.AddStretch();

   ClipLow_CheckBox.SetText( "Clip low pixels" );
   ClipLow_CheckBox.SetToolTip( "<p>Enable rejection of low pixels. Low pixels have values below the median of "
      "a pixel stack.</p>"
      "<p>If this option is disabled, rejection algorithms can only reject high pixels, i.e. pixels above the median.</p>" );
   ClipLow_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Rejection_Click, w );

   ClipLow_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClipLow_Sizer.Add( ClipLow_CheckBox );
   ClipLow_Sizer.AddStretch();

   ClipHigh_CheckBox.SetText( "Clip high pixels" );
   ClipHigh_CheckBox.SetToolTip( "<p>Enable rejection of high pixels. High pixels have values above the median of "
      "a pixel stack.</p>"
      "<p>If this option is disabled, rejection algorithms can only reject low pixels, i.e. pixels below the median.</p>" );
   ClipHigh_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Rejection_Click, w );

   ClipHigh_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClipHigh_Sizer.Add( ClipHigh_CheckBox );
   ClipHigh_Sizer.AddStretch();

   ClipLowRange_CheckBox.SetText( "Clip low range" );
   ClipLowRange_CheckBox.SetToolTip( "<p>Enable rejection of pixels with values less than or equal to the "
      "<i>low rejection range</i> parameter.</p>" );
   ClipLowRange_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Rejection_Click, w );

   ClipLowRange_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClipLowRange_Sizer.Add( ClipLowRange_CheckBox );
   ClipLowRange_Sizer.AddStretch();

   ClipHighRange_CheckBox.SetText( "Clip high range" );
   ClipHighRange_CheckBox.SetToolTip( "<p>Enable rejection of pixels with values greater than or equal to the "
      "<i>high rejection range</i> parameter.</p>" );
   ClipHighRange_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Rejection_Click, w );

   ClipHighRange_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClipHighRange_Sizer.Add( ClipHighRange_CheckBox );
   ClipHighRange_Sizer.AddStretch();

   ReportRangeRejection_CheckBox.SetText( "Report range rejection" );
   ReportRangeRejection_CheckBox.SetToolTip( "<p>Include range rejected pixels in rejected pixel counts "
      "reported on console summaries.</p>" );
   ReportRangeRejection_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Rejection_Click, w );

   ReportRangeRejection_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ReportRangeRejection_Sizer.Add( ReportRangeRejection_CheckBox );
   ReportRangeRejection_Sizer.AddStretch();

   MapRangeRejection_CheckBox.SetText( "Map range rejection" );
   MapRangeRejection_CheckBox.SetToolTip( "<p>Include range rejected pixels in pixel rejection maps.</p>" );
   MapRangeRejection_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Rejection_Click, w );

   MapRangeRejection_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   MapRangeRejection_Sizer.Add( MapRangeRejection_CheckBox );
   MapRangeRejection_Sizer.AddStretch();

   Rejection1_Sizer.SetSpacing( 4 );
   Rejection1_Sizer.Add( RejectionAlgorithm_Sizer );
   Rejection1_Sizer.Add( RejectionNormalization_Sizer );
   Rejection1_Sizer.Add( GenerateRejectionMaps_Sizer );
   Rejection1_Sizer.Add( ClipLow_Sizer );
   Rejection1_Sizer.Add( ClipHigh_Sizer );
   Rejection1_Sizer.Add( ClipLowRange_Sizer );
   Rejection1_Sizer.Add( ClipHighRange_Sizer );
   Rejection1_Sizer.Add( ReportRangeRejection_Sizer );
   Rejection1_Sizer.Add( MapRangeRejection_Sizer );

   Rejection1_Control.SetSizer( Rejection1_Sizer );

   //

   Rejection2_SectionBar.SetTitle( "Pixel Rejection (2)" );
   Rejection2_SectionBar.SetSection( Rejection2_Control );
   Rejection2_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::e_ToggleSection, w );

   const char* minMaxLowToolTip = "<p>Number of low (dark) pixels to be rejected by the min/max algorithm.</p>";

   MinMaxLow_Label.SetText( "Min/Max low:" );
   MinMaxLow_Label.SetFixedWidth( labelWidth1 );
   MinMaxLow_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MinMaxLow_Label.SetToolTip( minMaxLowToolTip );

   MinMaxLow_SpinBox.SetRange( int( TheIIMinMaxLowParameter->MinimumValue() ), 65536 );
   MinMaxLow_SpinBox.SetToolTip( minMaxLowToolTip );
   MinMaxLow_SpinBox.SetFixedWidth( editWidth2 );
   MinMaxLow_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_Rejection_SpinValueUpdated, w );

   MinMaxLow_Sizer.SetSpacing( 4 );
   MinMaxLow_Sizer.Add( MinMaxLow_Label );
   MinMaxLow_Sizer.Add( MinMaxLow_SpinBox );
   MinMaxLow_Sizer.AddStretch();

   const char* minMaxHighToolTip = "<p>Number of high (bright) pixels to be rejected by the min/max algorithm.</p>";

   MinMaxHigh_Label.SetText( "Min/Max high:" );
   MinMaxHigh_Label.SetFixedWidth( labelWidth1 );
   MinMaxHigh_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MinMaxHigh_Label.SetToolTip( minMaxHighToolTip );

   MinMaxHigh_SpinBox.SetRange( int( TheIIMinMaxHighParameter->MinimumValue() ), 65536 );
   MinMaxHigh_SpinBox.SetToolTip( minMaxHighToolTip );
   MinMaxHigh_SpinBox.SetFixedWidth( editWidth2 );
   MinMaxHigh_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_Rejection_SpinValueUpdated, w );

   MinMaxHigh_Sizer.SetSpacing( 4 );
   MinMaxHigh_Sizer.Add( MinMaxHigh_Label );
   MinMaxHigh_Sizer.Add( MinMaxHigh_SpinBox );
   MinMaxHigh_Sizer.AddStretch();

   PercentileLow_NumericControl.label.SetText( "Percentile low:" );
   PercentileLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   PercentileLow_NumericControl.slider.SetRange( 0, 100 );
   PercentileLow_NumericControl.slider.SetScaledMinWidth( 250 );
   PercentileLow_NumericControl.SetReal();
   PercentileLow_NumericControl.SetRange( TheIIPercentileLowParameter->MinimumValue(), TheIIPercentileLowParameter->MaximumValue() );
   PercentileLow_NumericControl.SetPrecision( TheIIPercentileLowParameter->Precision() );
   PercentileLow_NumericControl.edit.SetFixedWidth( editWidth2 );
   PercentileLow_NumericControl.SetToolTip( "<p>Low clipping factor for the percentile clipping rejection algorithm.</p>" );
   PercentileLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   PercentileHigh_NumericControl.label.SetText( "Percentile high:" );
   PercentileHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   PercentileHigh_NumericControl.slider.SetRange( 0, 100 );
   PercentileHigh_NumericControl.slider.SetScaledMinWidth( 250 );
   PercentileHigh_NumericControl.SetReal();
   PercentileHigh_NumericControl.SetRange( TheIIPercentileHighParameter->MinimumValue(), TheIIPercentileHighParameter->MaximumValue() );
   PercentileHigh_NumericControl.SetPrecision( TheIIPercentileHighParameter->Precision() );
   PercentileHigh_NumericControl.edit.SetFixedWidth( editWidth2 );
   PercentileHigh_NumericControl.SetToolTip( "<p>High clipping factor for the percentile clipping rejection algorithm.</p>" );
   PercentileHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   SigmaLow_NumericControl.label.SetText( "Sigma low:" );
   SigmaLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   SigmaLow_NumericControl.slider.SetRange( 0, 100 );
   SigmaLow_NumericControl.slider.SetScaledMinWidth( 250 );
   SigmaLow_NumericControl.SetReal();
   SigmaLow_NumericControl.SetRange( TheIISigmaLowParameter->MinimumValue(), TheIISigmaLowParameter->MaximumValue() );
   SigmaLow_NumericControl.SetPrecision( TheIISigmaLowParameter->Precision() );
   SigmaLow_NumericControl.edit.SetFixedWidth( editWidth2 );
   SigmaLow_NumericControl.SetToolTip( "<p>Low sigma clipping factor for the sigma clipping and averaged sigma clipping "
      "rejection algorithms.</p>" );
   SigmaLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   SigmaHigh_NumericControl.label.SetText( "Sigma high:" );
   SigmaHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   SigmaHigh_NumericControl.slider.SetRange( 0, 100 );
   SigmaHigh_NumericControl.slider.SetScaledMinWidth( 250 );
   SigmaHigh_NumericControl.SetReal();
   SigmaHigh_NumericControl.SetRange( TheIISigmaHighParameter->MinimumValue(), TheIISigmaHighParameter->MaximumValue() );
   SigmaHigh_NumericControl.SetPrecision( TheIISigmaHighParameter->Precision() );
   SigmaHigh_NumericControl.edit.SetFixedWidth( editWidth2 );
   SigmaHigh_NumericControl.SetToolTip( "<p>High sigma clipping factor for the sigma clipping and averaged sigma clipping "
      "rejection algorithms.</p>" );
   SigmaHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   WinsorizationCutoff_NumericControl.label.SetText( "Winsorization cutoff:" );
   WinsorizationCutoff_NumericControl.label.SetFixedWidth( labelWidth1 );
   WinsorizationCutoff_NumericControl.slider.SetRange( 30, 100 );
   WinsorizationCutoff_NumericControl.slider.SetScaledMinWidth( 250 );
   WinsorizationCutoff_NumericControl.SetReal();
   WinsorizationCutoff_NumericControl.SetRange( TheIIWinsorizationCutoffParameter->MinimumValue(), TheIIWinsorizationCutoffParameter->MaximumValue() );
   WinsorizationCutoff_NumericControl.SetPrecision( TheIIWinsorizationCutoffParameter->Precision() );
   WinsorizationCutoff_NumericControl.edit.SetFixedWidth( editWidth2 );
   WinsorizationCutoff_NumericControl.SetToolTip( "<p>Cutoff point for the Winsorized sigma clipping rejection algorithm, "
      "in sigma units.</p>"
      "<p>All pixel samples with absolute differences from the median larger than this parameter will be set equal to the median "
      "of the pixel stack in the first rejection iteration. This replaces extreme outliers, such as cosmics, hot and cold pixels, "
      "with plausible values instead of their nearest neighbors.</p>" );
   WinsorizationCutoff_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   LinearFitLow_NumericControl.label.SetText( "Linear fit low:" );
   LinearFitLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   LinearFitLow_NumericControl.slider.SetRange( 0, 100 );
   LinearFitLow_NumericControl.slider.SetScaledMinWidth( 250 );
   LinearFitLow_NumericControl.SetReal();
   LinearFitLow_NumericControl.SetRange( TheIILinearFitLowParameter->MinimumValue(), TheIILinearFitLowParameter->MaximumValue() );
   LinearFitLow_NumericControl.SetPrecision( TheIILinearFitLowParameter->Precision() );
   LinearFitLow_NumericControl.edit.SetFixedWidth( editWidth2 );
   LinearFitLow_NumericControl.SetToolTip( "<p>Tolerance of the linear fit clipping algorithm for low pixel values, in sigma units.</p>" );
   LinearFitLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   LinearFitHigh_NumericControl.label.SetText( "Linear fit high:" );
   LinearFitHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   LinearFitHigh_NumericControl.slider.SetRange( 0, 100 );
   LinearFitHigh_NumericControl.slider.SetScaledMinWidth( 250 );
   LinearFitHigh_NumericControl.SetReal();
   LinearFitHigh_NumericControl.SetRange( TheIILinearFitHighParameter->MinimumValue(), TheIILinearFitHighParameter->MaximumValue() );
   LinearFitHigh_NumericControl.SetPrecision( TheIILinearFitHighParameter->Precision() );
   LinearFitHigh_NumericControl.edit.SetFixedWidth( editWidth2 );
   LinearFitHigh_NumericControl.SetToolTip( "<p>Tolerance of the linear fit clipping algorithm for high pixel values, in sigma units.</p>" );
   LinearFitHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   ESDOutliersFraction_NumericControl.label.SetText( "ESD outliers:" );
   ESDOutliersFraction_NumericControl.label.SetFixedWidth( labelWidth1 );
   ESDOutliersFraction_NumericControl.slider.SetRange( 0, 100 );
   ESDOutliersFraction_NumericControl.slider.SetScaledMinWidth( 250 );
   ESDOutliersFraction_NumericControl.SetReal();
   ESDOutliersFraction_NumericControl.SetRange( TheIIESDOutliersFractionParameter->MinimumValue(), TheIIESDOutliersFractionParameter->MaximumValue() );
   ESDOutliersFraction_NumericControl.SetPrecision( TheIIESDOutliersFractionParameter->Precision() );
   ESDOutliersFraction_NumericControl.edit.SetFixedWidth( editWidth2 );
   ESDOutliersFraction_NumericControl.SetToolTip( "<p>Expected maximum fraction of outliers for the generalized "
      "ESD rejection algorithm.</p>"
      "<p>For example, a value of 0.2 applied to a stack of 10 pixels means that the ESD algorithm will be limited "
      "to detect a maximum of two outlier pixels, or in other words, only 0, 1 or 2 outliers will be detectable in "
      "such case. The default value is 0.3, which allows the algorithm to detect up to a 30% of outlier pixels in "
      "each pixel stack.</p>" );
   ESDOutliersFraction_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   ESDAlpha_NumericControl.label.SetText( "ESD significance:" );
   ESDAlpha_NumericControl.label.SetFixedWidth( labelWidth1 );
   ESDAlpha_NumericControl.slider.SetRange( 0, 100 );
   ESDAlpha_NumericControl.slider.SetScaledMinWidth( 250 );
   ESDAlpha_NumericControl.SetReal();
   ESDAlpha_NumericControl.SetRange( TheIIESDAlphaParameter->MinimumValue(), TheIIESDAlphaParameter->MaximumValue() );
   ESDAlpha_NumericControl.SetPrecision( TheIIESDAlphaParameter->Precision() );
   ESDAlpha_NumericControl.edit.SetFixedWidth( editWidth2 );
   ESDAlpha_NumericControl.SetToolTip( "<p>Probability of making a type I error (false positive) in the generalized "
      "ESD rejection algorithm.</p>"
      "<p>This is the significance level of the outlier detection hypothesis test. For example, a significance level "
      "of 0.01 means that a 1% chance of being wrong when rejecting the null hypothesis (that there are no outliers in "
      "a given pixel stack) is acceptable. The default value is 0.05 (5% significance level).</p>"
      "<p>By increasing this parameter the ESD algorithm will tend to reject more pixels in each stack. The effect of "
      "increasing this parameter is similar to reducing the threshold of a sigma clipping algorithm (although the "
      "underlying mechanism is very different).</p>" );
   ESDAlpha_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   ESDLowRelaxation_NumericControl.label.SetText( "ESD low relaxation:" );
   ESDLowRelaxation_NumericControl.label.SetFixedWidth( labelWidth1 );
   ESDLowRelaxation_NumericControl.slider.SetRange( 0, 50 );
   ESDLowRelaxation_NumericControl.slider.SetScaledMinWidth( 250 );
   ESDLowRelaxation_NumericControl.SetReal();
   ESDLowRelaxation_NumericControl.SetRange( TheIIESDLowRelaxationParameter->MinimumValue(), TheIIESDLowRelaxationParameter->MaximumValue() );
   ESDLowRelaxation_NumericControl.SetPrecision( TheIIESDLowRelaxationParameter->Precision() );
   ESDLowRelaxation_NumericControl.edit.SetFixedWidth( editWidth2 );
   ESDLowRelaxation_NumericControl.SetToolTip( "<p>Relaxation factor for ESD rejection of low pixels.</p>"
      "<p>The larger the value of this parameter, the more permissive the ESD algorithm will be for rejection of pixels "
      "with values below the median of each stack. This can be useful to reject less dark pixels on sky background areas "
      "and extended nebular regions, where high dispersion induced by noise may lead to excessive detection of false "
      "outliers.</p>" );
   ESDLowRelaxation_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   RangeLow_NumericControl.label.SetText( "Range low:" );
   RangeLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   RangeLow_NumericControl.slider.SetRange( 0, 100 );
   RangeLow_NumericControl.slider.SetScaledMinWidth( 250 );
   RangeLow_NumericControl.SetReal();
   RangeLow_NumericControl.SetRange( TheIIRangeLowParameter->MinimumValue(), TheIIRangeLowParameter->MaximumValue() );
   RangeLow_NumericControl.SetPrecision( TheIIRangeLowParameter->Precision() );
   RangeLow_NumericControl.edit.SetFixedWidth( editWidth2 );
   RangeLow_NumericControl.SetToolTip( "<p>Low rejection range. If the <i>clip low range</i> option is enabled, "
      "all pixels with values less than or equal to this low rejection range parameter will be rejected.</p>" );
   RangeLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   RangeHigh_NumericControl.label.SetText( "Range high:" );
   RangeHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   RangeHigh_NumericControl.slider.SetRange( 0, 100 );
   RangeHigh_NumericControl.slider.SetScaledMinWidth( 250 );
   RangeHigh_NumericControl.SetReal();
   RangeHigh_NumericControl.SetRange( TheIIRangeHighParameter->MinimumValue(), TheIIRangeHighParameter->MaximumValue() );
   RangeHigh_NumericControl.SetPrecision( TheIIRangeHighParameter->Precision() );
   RangeHigh_NumericControl.edit.SetFixedWidth( editWidth2 );
   RangeHigh_NumericControl.SetToolTip( "<p>High rejection range. If the <i>clip high range</i> option is enabled, "
      "all pixels with values greater than or equal to this high rejection range parameter will be rejected.</p>" );
   RangeHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   Rejection2_Sizer.SetSpacing( 4 );
   Rejection2_Sizer.Add( MinMaxLow_Sizer );
   Rejection2_Sizer.Add( MinMaxHigh_Sizer );
   Rejection2_Sizer.Add( PercentileLow_NumericControl );
   Rejection2_Sizer.Add( PercentileHigh_NumericControl );
   Rejection2_Sizer.Add( SigmaLow_NumericControl );
   Rejection2_Sizer.Add( SigmaHigh_NumericControl );
   Rejection2_Sizer.Add( WinsorizationCutoff_NumericControl );
   Rejection2_Sizer.Add( LinearFitLow_NumericControl );
   Rejection2_Sizer.Add( LinearFitHigh_NumericControl );
   Rejection2_Sizer.Add( ESDOutliersFraction_NumericControl );
   Rejection2_Sizer.Add( ESDAlpha_NumericControl );
   Rejection2_Sizer.Add( ESDLowRelaxation_NumericControl );
   Rejection2_Sizer.Add( RangeLow_NumericControl );
   Rejection2_Sizer.Add( RangeHigh_NumericControl );

   Rejection2_Control.SetSizer( Rejection2_Sizer );

   //

   Rejection3_SectionBar.SetTitle( "Pixel Rejection (3)" );
   Rejection3_SectionBar.SetSection( Rejection3_Control );
   Rejection3_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::e_ToggleSection, w );

   CCDGain_NumericControl.label.SetText( "CCD gain:" );
   CCDGain_NumericControl.label.SetFixedWidth( labelWidth1 );
   CCDGain_NumericControl.slider.SetRange( 0, 100 );
   CCDGain_NumericControl.slider.SetScaledMinWidth( 250 );
   CCDGain_NumericControl.SetReal();
   CCDGain_NumericControl.SetRange( TheIICCDGainParameter->MinimumValue(), TheIICCDGainParameter->MaximumValue() );
   CCDGain_NumericControl.SetPrecision( TheIICCDGainParameter->Precision() );
   CCDGain_NumericControl.edit.SetFixedWidth( editWidth2 );
   CCDGain_NumericControl.SetToolTip( "<p>CCD sensor gain in electrons per data number (e-/ADU). Only used by the CCD noise "
      "clipping rejection algorithm.</p>" );
   CCDGain_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   CCDReadNoise_NumericControl.label.SetText( "CCD readout noise:" );
   CCDReadNoise_NumericControl.label.SetFixedWidth( labelWidth1 );
   CCDReadNoise_NumericControl.slider.SetRange( 0, 100 );
   CCDReadNoise_NumericControl.slider.SetScaledMinWidth( 250 );
   CCDReadNoise_NumericControl.SetReal();
   CCDReadNoise_NumericControl.SetRange( TheIICCDReadNoiseParameter->MinimumValue(), TheIICCDReadNoiseParameter->MaximumValue() );
   CCDReadNoise_NumericControl.SetPrecision( TheIICCDReadNoiseParameter->Precision() );
   CCDReadNoise_NumericControl.edit.SetFixedWidth( editWidth2 );
   CCDReadNoise_NumericControl.SetToolTip( "<p>CCD readout noise in electrons. Only used by the CCD noise "
      "clipping rejection algorithm.</p>" );
   CCDReadNoise_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   CCDScaleNoise_NumericControl.label.SetText( "CCD scale noise:" );
   CCDScaleNoise_NumericControl.label.SetFixedWidth( labelWidth1 );
   CCDScaleNoise_NumericControl.slider.SetRange( 0, 100 );
   CCDScaleNoise_NumericControl.slider.SetScaledMinWidth( 250 );
   CCDScaleNoise_NumericControl.SetReal();
   CCDScaleNoise_NumericControl.SetRange( TheIICCDScaleNoiseParameter->MinimumValue(), TheIICCDScaleNoiseParameter->MaximumValue() );
   CCDScaleNoise_NumericControl.SetPrecision( TheIICCDScaleNoiseParameter->Precision() );
   CCDScaleNoise_NumericControl.edit.SetFixedWidth( editWidth2 );
   CCDScaleNoise_NumericControl.SetToolTip( "<p>CCD <i>scale noise</i> (AKA <i>sensitivity noise</i>). This is a "
      "dimensionless factor, only used by the CCD noise clipping rejection algorithm. Scale noise typically comes "
      "from noise introduced during flat fielding.</p>" );
   CCDScaleNoise_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::e_Rejection_EditValueUpdated, w );

   Rejection3_Sizer.SetSpacing( 4 );
   Rejection3_Sizer.Add( CCDGain_NumericControl );
   Rejection3_Sizer.Add( CCDReadNoise_NumericControl );
   Rejection3_Sizer.Add( CCDScaleNoise_NumericControl );

   Rejection3_Control.SetSizer( Rejection3_Sizer );

   //

   LargeScaleRejection_SectionBar.SetTitle( "Large-Scale Pixel Rejection" );
   LargeScaleRejection_SectionBar.SetSection( LargeScaleRejection_Control );
   LargeScaleRejection_SectionBar.SetToolTip( "<p>Large-scale pixel rejection uses multiscale analysis techniques to detect structures "
      "of rejected pixels. These structures are grown to cover borderline regions where high uncertainty may lead to insufficient rejection "
      "by statistical pixel rejection algorithms.</p>"
      "<p>This feature is very efficient to improve rejection of plane and satellite trails and flashes. With large-scale high pixel rejection "
      "enabled, you can be sure these artifacts will be correctly rejected without needing to reduce clipping factors, which in turn allows "
      "for more accurate rejection with inclusion of more signal in the integrated result. Large-scale low rejection may be useful to remove "
      "dust motes and other relatively large dark artifacts.</p>" );
   LargeScaleRejection_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::e_ToggleSection, w );

   RejectLargeScaleLow_CheckBox.SetText( "Reject low large-scale structures" );
   RejectLargeScaleLow_CheckBox.SetToolTip( "<p>Enable large-scale rejection for low pixels. A low pixel sample has a value below the "
      "estimated central value of its pixel stack.</p>" );
   RejectLargeScaleLow_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Rejection_Click, w );

   RejectLargeScaleLow_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   RejectLargeScaleLow_Sizer.Add( RejectLargeScaleLow_CheckBox );
   RejectLargeScaleLow_Sizer.AddStretch();

   const char* smallScaleLayersLowToolTip = "<p>Number of small-scale layers that will be excluded from large-scale low pixel "
   "rejection maps. Increase this value to detect larger groups of contiguous rejected pixels. Excluding too few layers may lead to "
   "excessive (unnecessary) large-scale rejection. Excluding too many layers may lead to insufficient large-scale rejection. The "
   "default value of two layers is generally a good compromise.</p>";

   SmallScaleLayersLow_Label.SetText( "Layers (low):" );
   SmallScaleLayersLow_Label.SetFixedWidth( labelWidth1 );
   SmallScaleLayersLow_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SmallScaleLayersLow_Label.SetToolTip( smallScaleLayersLowToolTip );

   SmallScaleLayersLow_SpinBox.SetRange( int( TheIILargeScaleClipHighProtectedLayersParameter->MinimumValue() ),
                                         int( TheIILargeScaleClipHighProtectedLayersParameter->MaximumValue() ) );
   SmallScaleLayersLow_SpinBox.SetToolTip( smallScaleLayersLowToolTip );
   SmallScaleLayersLow_SpinBox.SetFixedWidth( editWidth2 );
   SmallScaleLayersLow_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_Rejection_SpinValueUpdated, w );

   SmallScaleLayersLow_Sizer.SetSpacing( 4 );
   SmallScaleLayersLow_Sizer.Add( SmallScaleLayersLow_Label );
   SmallScaleLayersLow_Sizer.Add( SmallScaleLayersLow_SpinBox );
   SmallScaleLayersLow_Sizer.AddStretch();

   const char* growthLowToolTip = "<p>Growth of large-scale pixel rejection structures. Increase to extend rejection to more "
   "adjacent pixels. Excessive growth will reject too many pixels unnecessarily around large-scale structures, such as plane trails. "
   "Insufficient growth will leave unrejected pixels on high-uncertainty regions. The default value of two pixels is generally a "
   "good option.</p>";

   GrowthLow_Label.SetText( "Growth (low):" );
   GrowthLow_Label.SetFixedWidth( labelWidth1 );
   GrowthLow_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GrowthLow_Label.SetToolTip( growthLowToolTip );

   GrowthLow_SpinBox.SetRange( int( TheIILargeScaleClipHighGrowthParameter->MinimumValue() ),
                               int( TheIILargeScaleClipHighGrowthParameter->MaximumValue() ) );
   GrowthLow_SpinBox.SetToolTip( growthLowToolTip );
   GrowthLow_SpinBox.SetFixedWidth( editWidth2 );
   GrowthLow_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_Rejection_SpinValueUpdated, w );

   GrowthLow_Sizer.SetSpacing( 4 );
   GrowthLow_Sizer.Add( GrowthLow_Label );
   GrowthLow_Sizer.Add( GrowthLow_SpinBox );
   GrowthLow_Sizer.AddStretch();

   RejectLargeScaleHigh_CheckBox.SetText( "Reject high large-scale structures" );
   RejectLargeScaleHigh_CheckBox.SetToolTip( "<p>Enable large-scale rejection for high pixels. A high pixel sample has a value above the "
      "estimated central value of its pixel stack.</p>" );
   RejectLargeScaleHigh_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_Rejection_Click, w );

   RejectLargeScaleHigh_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   RejectLargeScaleHigh_Sizer.Add( RejectLargeScaleHigh_CheckBox );
   RejectLargeScaleHigh_Sizer.AddStretch();

   const char* smallScaleLayersHighToolTip = "<p>Number of small-scale layers that will be excluded from large-scale high pixel "
   "rejection maps. Increase this value to detect larger groups of contiguous rejected pixels. Excluding too few layers may lead to "
   "excessive (unnecessary) large-scale rejection. Excluding too many layers may lead to insufficient large-scale rejection. The "
   "default value of two layers is generally a good compromise.</p>";

   SmallScaleLayersHigh_Label.SetText( "Layers (high):" );
   SmallScaleLayersHigh_Label.SetFixedWidth( labelWidth1 );
   SmallScaleLayersHigh_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SmallScaleLayersHigh_Label.SetToolTip( smallScaleLayersHighToolTip );

   SmallScaleLayersHigh_SpinBox.SetRange( int( TheIILargeScaleClipHighProtectedLayersParameter->MinimumValue() ),
                                          int( TheIILargeScaleClipHighProtectedLayersParameter->MaximumValue() ) );
   SmallScaleLayersHigh_SpinBox.SetToolTip( smallScaleLayersHighToolTip );
   SmallScaleLayersHigh_SpinBox.SetFixedWidth( editWidth2 );
   SmallScaleLayersHigh_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_Rejection_SpinValueUpdated, w );

   SmallScaleLayersHigh_Sizer.SetSpacing( 4 );
   SmallScaleLayersHigh_Sizer.Add( SmallScaleLayersHigh_Label );
   SmallScaleLayersHigh_Sizer.Add( SmallScaleLayersHigh_SpinBox );
   SmallScaleLayersHigh_Sizer.AddStretch();

   const char* growthHighToolTip = "<p>Growth of large-scale pixel rejection structures. Increase to extend rejection to more "
   "adjacent pixels. Excessive growth will reject too many pixels unnecessarily around large-scale structures, such as plane trails. "
   "Insufficient growth will leave unrejected pixels on high-uncertainty regions. The default value of two pixels is generally a "
   "good option.</p>";

   GrowthHigh_Label.SetText( "Growth (high):" );
   GrowthHigh_Label.SetFixedWidth( labelWidth1 );
   GrowthHigh_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GrowthHigh_Label.SetToolTip( growthHighToolTip );

   GrowthHigh_SpinBox.SetRange( int( TheIILargeScaleClipHighGrowthParameter->MinimumValue() ),
                                int( TheIILargeScaleClipHighGrowthParameter->MaximumValue() ) );
   GrowthHigh_SpinBox.SetToolTip( growthHighToolTip );
   GrowthHigh_SpinBox.SetFixedWidth( editWidth2 );
   GrowthHigh_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_Rejection_SpinValueUpdated, w );

   GrowthHigh_Sizer.SetSpacing( 4 );
   GrowthHigh_Sizer.Add( GrowthHigh_Label );
   GrowthHigh_Sizer.Add( GrowthHigh_SpinBox );
   GrowthHigh_Sizer.AddStretch();

   LargeScaleRejection_Sizer.SetSpacing( 4 );
   LargeScaleRejection_Sizer.Add( RejectLargeScaleLow_Sizer );
   LargeScaleRejection_Sizer.Add( SmallScaleLayersLow_Sizer );
   LargeScaleRejection_Sizer.Add( GrowthLow_Sizer );
   LargeScaleRejection_Sizer.Add( RejectLargeScaleHigh_Sizer );
   LargeScaleRejection_Sizer.Add( SmallScaleLayersHigh_Sizer );
   LargeScaleRejection_Sizer.Add( GrowthHigh_Sizer );

   LargeScaleRejection_Control.SetSizer( LargeScaleRejection_Sizer );

   //

   ROI_SectionBar.SetTitle( "Region of Interest" );
   ROI_SectionBar.EnableTitleCheckBox();
   ROI_SectionBar.SetSection( ROI_Control );
   ROI_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageIntegrationInterface::e_ROI_Check, w );
   ROI_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::e_ToggleSection, w );
   ROI_SectionBar.OnViewDrag( (Control::view_drag_event_handler)&ImageIntegrationInterface::e_ViewDrag, w );
   ROI_SectionBar.OnViewDrop( (Control::view_drop_event_handler)&ImageIntegrationInterface::e_ViewDrop, w );

   const char* roiX0ToolTip = "<p>X pixel coordinate of the upper-left corner of the ROI.</p>";

   ROIX0_Label.SetText( "Left:" );
   ROIX0_Label.SetFixedWidth( labelWidth1 );
   ROIX0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIX0_Label.SetToolTip( roiX0ToolTip );

   ROIX0_SpinBox.SetRange( 0, int_max );
   //ROIX0_SpinBox.SetFixedWidth( spinWidth1 );
   ROIX0_SpinBox.SetToolTip( roiX0ToolTip );
   ROIX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_ROI_SpinValueUpdated, w );

   ROIX0_Sizer.SetSpacing( 4 );
   ROIX0_Sizer.Add( ROIX0_Label );
   ROIX0_Sizer.Add( ROIX0_SpinBox );
   ROIX0_Sizer.AddStretch();

   const char* roiY0ToolTip = "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>";

   ROIY0_Label.SetText( "Top:" );
   ROIY0_Label.SetFixedWidth( labelWidth1 );
   ROIY0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIY0_Label.SetToolTip( roiY0ToolTip );

   ROIY0_SpinBox.SetRange( 0, int_max );
   //ROIY0_SpinBox.SetFixedWidth( spinWidth1 );
   ROIY0_SpinBox.SetToolTip( roiY0ToolTip );
   ROIY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_ROI_SpinValueUpdated, w );

   ROIY0_Sizer.SetSpacing( 4 );
   ROIY0_Sizer.Add( ROIY0_Label );
   ROIY0_Sizer.Add( ROIY0_SpinBox );
   ROIY0_Sizer.AddStretch();

   const char* roiWidthToolTip = "<p>Width of the ROI in pixels.</p>";

   ROIWidth_Label.SetText( "Width:" );
   ROIWidth_Label.SetFixedWidth( labelWidth1 );
   ROIWidth_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIWidth_Label.SetToolTip( roiWidthToolTip );

   ROIWidth_SpinBox.SetRange( 0, int_max );
   //ROIWidth_SpinBox.SetFixedWidth( spinWidth1 );
   ROIWidth_SpinBox.SetToolTip( roiWidthToolTip );
   ROIWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_ROI_SpinValueUpdated, w );

   ROIWidth_Sizer.SetSpacing( 4 );
   ROIWidth_Sizer.Add( ROIWidth_Label );
   ROIWidth_Sizer.Add( ROIWidth_SpinBox );
   ROIWidth_Sizer.AddStretch();

   const char* roiHeightToolTip = "<p>Height of the ROI in pixels.</p>";

   ROIHeight_Label.SetText( "Height:" );
   ROIHeight_Label.SetFixedWidth( labelWidth1 );
   ROIHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIHeight_Label.SetToolTip( roiHeightToolTip );

   ROIHeight_SpinBox.SetRange( 0, int_max );
   //ROIHeight_SpinBox.SetFixedWidth( spinWidth1 );
   ROIHeight_SpinBox.SetToolTip( roiHeightToolTip );
   ROIHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::e_ROI_SpinValueUpdated, w );

   SelectPreview_Button.SetText( "From Preview" );
   SelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview.</p>" );
   SelectPreview_Button.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::e_ROI_Click, w );
   SelectPreview_Button.OnViewDrag( (Control::view_drag_event_handler)&ImageIntegrationInterface::e_ViewDrag, w );
   SelectPreview_Button.OnViewDrop( (Control::view_drop_event_handler)&ImageIntegrationInterface::e_ViewDrop, w );

   ROIHeight_Sizer.SetSpacing( 4 );
   ROIHeight_Sizer.Add( ROIHeight_Label );
   ROIHeight_Sizer.Add( ROIHeight_SpinBox );
   ROIHeight_Sizer.AddStretch();
   ROIHeight_Sizer.Add( SelectPreview_Button );

   ROI_Sizer.SetSpacing( 4 );
   ROI_Sizer.Add( ROIX0_Sizer );
   ROI_Sizer.Add( ROIY0_Sizer );
   ROI_Sizer.Add( ROIWidth_Sizer );
   ROI_Sizer.Add( ROIHeight_Sizer );

   ROI_Control.SetSizer( ROI_Sizer );
   ROI_Control.OnViewDrag( (Control::view_drag_event_handler)&ImageIntegrationInterface::e_ViewDrag, w );
   ROI_Control.OnViewDrop( (Control::view_drop_event_handler)&ImageIntegrationInterface::e_ViewDrop, w );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( InputImages_SectionBar );
   Global_Sizer.Add( InputImages_Control );
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add( Integration_SectionBar );
   Global_Sizer.Add( Integration_Control );
   Global_Sizer.Add( Rejection1_SectionBar );
   Global_Sizer.Add( Rejection1_Control );
   Global_Sizer.Add( Rejection2_SectionBar );
   Global_Sizer.Add( Rejection2_Control );
   Global_Sizer.Add( Rejection3_SectionBar );
   Global_Sizer.Add( Rejection3_Control );
   Global_Sizer.Add( LargeScaleRejection_SectionBar );
   Global_Sizer.Add( LargeScaleRejection_Control );
   Global_Sizer.Add( ROI_SectionBar );
   Global_Sizer.Add( ROI_Control );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedWidth();

   FormatHints_Control.Hide();
   Rejection1_Control.Hide();
   Rejection2_Control.Hide();
   Rejection3_Control.Hide();
   LargeScaleRejection_Control.Hide();
   ROI_Control.Hide();

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageIntegrationInterface.cpp - Released 2021-04-09T19:41:48Z
