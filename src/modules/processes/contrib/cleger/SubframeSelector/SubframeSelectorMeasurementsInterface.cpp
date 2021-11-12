//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.15
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.6.0
// ----------------------------------------------------------------------------
// SubframeSelectorMeasurementsInterface.cpp - Released 2021-11-11T17:56:06Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2021 Cameron Leger
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

#include <pcl/Console.h>
#include <pcl/FileDialog.h>
#include <pcl/MetaModule.h>
#include <pcl/StandardStatus.h>

#include "SubframeSelectorExpressionsInterface.h"
#include "SubframeSelectorMeasurementsInterface.h"
#include "SubframeSelectorProcess.h"

#define IMAGELIST_MINHEIGHT( fnt )  (12*fnt.Height() + 2)

namespace pcl
{

// ----------------------------------------------------------------------------

static pcl_enum s_comboBoxItemIndexToSortingProperty[] =
{
   SSSortingProperty::Index,
   SSSortingProperty::Weight,
   SSSortingProperty::PSFSignalWeight,
   SSSortingProperty::PSFPowerWeight,
   SSSortingProperty::SNRWeight,
   SSSortingProperty::FWHM,
   SSSortingProperty::Eccentricity,
   SSSortingProperty::Stars,
   SSSortingProperty::StarResidual,
   SSSortingProperty::Altitude,
   SSSortingProperty::Azimuth,
   SSSortingProperty::Noise,
   SSSortingProperty::NoiseRatio,
   SSSortingProperty::Median,
   SSSortingProperty::FWHMMeanDev,
   SSSortingProperty::EccentricityMeanDev,
   SSSortingProperty::StarResidualMeanDev,
   SSSortingProperty::MedianMeanDev
};

static int s_sortingPropertyToComboBoxItemIndex[ SSSortingProperty::NumberOfItems ];

static pcl_enum s_comboBoxItemIndexToGraphProperty[] =
{
   SSGraphProperty::Weight,
   SSGraphProperty::PSFSignalWeight,
   SSGraphProperty::PSFPowerWeight,
   SSGraphProperty::SNRWeight,
   SSGraphProperty::FWHM,
   SSGraphProperty::Eccentricity,
   SSGraphProperty::Stars,
   SSGraphProperty::StarResidual,
   SSGraphProperty::Altitude,
   SSGraphProperty::Azimuth,
   SSGraphProperty::Noise,
   SSGraphProperty::NoiseRatio,
   SSGraphProperty::Median,
   SSGraphProperty::FWHMMeanDev,
   SSGraphProperty::EccentricityMeanDev,
   SSGraphProperty::StarResidualMeanDev,
   SSGraphProperty::MedianMeanDev
};

static int s_graphPropertyToComboBoxItemIndex[ SSGraphProperty::NumberOfItems ];

// ----------------------------------------------------------------------------

SubframeSelectorMeasurementsInterface* TheSubframeSelectorMeasurementsInterface = nullptr;

// ----------------------------------------------------------------------------

SubframeSelectorMeasurementsInterface::SubframeSelectorMeasurementsInterface( SubframeSelectorInstance& instance )
   : m_instance( instance )
{
   TheSubframeSelectorMeasurementsInterface = this;
}

// ----------------------------------------------------------------------------

SubframeSelectorMeasurementsInterface::~SubframeSelectorMeasurementsInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString SubframeSelectorMeasurementsInterface::Id() const
{
   return "SubframeSelectorMeasurements";
}

// ----------------------------------------------------------------------------

MetaProcess* SubframeSelectorMeasurementsInterface::Process() const
{
   return TheSubframeSelectorProcess;
}

// ----------------------------------------------------------------------------

String SubframeSelectorMeasurementsInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/SubframeSelector.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures SubframeSelectorMeasurementsInterface::Features() const
{
   return InterfaceFeature::None;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorMeasurementsInterface::IsInstanceGenerator() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorMeasurementsInterface::CanImportInstances() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorMeasurementsInterface::Launch( const MetaProcess&,
                                                    const ProcessImplementation*,
                                                    bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      for ( int i = 0; i < SSSortingProperty::NumberOfItems; ++i )
         for ( int j = 0; j < SSSortingProperty::NumberOfItems; ++j )
            if ( s_comboBoxItemIndexToSortingProperty[j] == pcl_enum( i ) )
            {
               s_sortingPropertyToComboBoxItemIndex[i] = j;
               break;
            }

      for ( int i = 0; i < SSGraphProperty::NumberOfItems; ++i )
         for ( int j = 0; j < SSGraphProperty::NumberOfItems; ++j )
            if ( s_comboBoxItemIndexToGraphProperty[j] == pcl_enum( i ) )
            {
               s_graphPropertyToComboBoxItemIndex[i] = j;
               break;
            }

      GUI = new GUIData( *this );
      SetWindowTitle( "SubframeSelector | Measurements" );
      UpdateControls();
   }

   dynamic = false;
   return true;
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::Cleanup()
{
   if ( GUI != nullptr )
   {
      GUI->MeasurementGraph_WebView.Cleanup();
      GUI->MeasurementDistributionGraph_WebView.Cleanup();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

MeasureItem* SubframeSelectorMeasurementsInterface::GetMeasurementItem( size_type i )
{
   TreeBox::Node* node = GUI->MeasurementTable_TreeBox[i];
   if ( node == nullptr )
      return nullptr;

   int index;
   if ( !node->Text( 0 ).TryToInt( index ) )
      return nullptr;

   // 1-based index, 0-based arrays
   if ( index > 0 ) // ?!
      --index;
   return m_instance.o_measures.At( index );
}

// ----------------------------------------------------------------------------

TreeBox::Node* SubframeSelectorMeasurementsInterface::GetMeasurementNode( MeasureItem* item )
{
   if ( item == nullptr )
      return nullptr;

   for ( int i = 0; i < GUI->MeasurementTable_TreeBox.NumberOfChildren(); ++i )
   {
      TreeBox::Node* node = GUI->MeasurementTable_TreeBox[i];
      if ( node == nullptr )
         return nullptr;

      int index;
      if ( !node->Text( 0 ).TryToInt( index ) )
         return nullptr;

      // 1-based index, 0-based arrays
      if ( index > 0 ) // ?!
         --index;
      if ( index == item->index )
         return node;
   }

   return nullptr;
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateControls()
{
   UpdateMeasurementImagesList();
   UpdateMeasurementGraph();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementQuantity()
{
   // Update the table and gather quantities
   int approved = 0;
   int locked = 0;
   size_type amount = m_instance.o_measures.Length();
   if ( amount > 0 )
   {
      for ( Array<MeasureItem>::const_iterator i = m_instance.o_measures.Begin(); i < m_instance.o_measures.End(); ++i )
      {
         if ( i->enabled )
            ++approved;
         if ( i->locked )
            ++locked;
      }

      GUI->MeasurementsTable_Quantities_Label.SetText(
         String().Format( "%i/%i Approved (%.2f%%), %i Locked (%.2f%%)",
                          approved, amount, 100.0*approved/amount,
                          locked, 100.0*locked/amount ) );
   }
   else
   {
      GUI->MeasurementsTable_Quantities_Label.SetText( "0/0 Approved (0%), 0 Locked (0%)" );
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementImageItem( size_type i, MeasureItem* item )
{
   TreeBox::Node* node = GUI->MeasurementTable_TreeBox[i];
   if ( node == nullptr )
      return;
   if ( item == nullptr )
      return;

   node->SetText( 0, String().Format( "%4d", item->index + 1 ) ); // N.B. 1-based index, 0-based arrays
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item->enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, Bitmap( ScaledResource( item->locked ? ":/file-explorer/read-only.png" : ":/icons/unlock.png" ) ) );
   node->SetAlignment( 2, TextAlign::Left );

   node->SetIcon( 3, Bitmap( ScaledResource( ":/browser/picture.png" ) ) );
   node->SetText( 3, File::ExtractNameAndSuffix( item->path ) );
   node->SetToolTip( 3, item->path );
   node->SetAlignment( 3, TextAlign::Left );

   node->SetText( 4, String().Format( "%.4e", item->weight ) );
   node->SetAlignment( 4, TextAlign::Center );

   node->SetText( 5, String().Format( "%.4e", item->psfSignalWeight ) );
   node->SetAlignment( 5, TextAlign::Center );

   node->SetText( 6, String().Format( "%.4e", item->psfPowerWeight ) );
   node->SetAlignment( 6, TextAlign::Center );

   node->SetText( 7, String().Format( "%.4e", item->snrWeight ) );
   node->SetAlignment( 7, TextAlign::Center );

   node->SetText( 8, String().Format( "%.4f", item->FWHM( m_instance.p_subframeScale, m_instance.p_scaleUnit ) ) );
   node->SetAlignment( 8, TextAlign::Center );

   node->SetText( 9, String().Format( "%.4f", item->eccentricity ) );
   node->SetAlignment( 9, TextAlign::Center );

   node->SetText( 10, String().Format( "%u", unsigned( item->stars ) ) );
   node->SetAlignment( 10, TextAlign::Center );

   node->SetText( 11, String().Format( "%.4e", item->starResidual ) );
   node->SetAlignment( 11, TextAlign::Center );

   node->SetText( 12, String().Format( "%.4f", item->altitude ) );
   node->SetAlignment( 12, TextAlign::Center );

   node->SetText( 13, String().Format( "%.4f", item->azimuth ) );
   node->SetAlignment( 13, TextAlign::Center );

   node->SetText( 14, String().Format( "%.4e",
            item->Noise( m_instance.p_cameraGain,
                         TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                         m_instance.p_dataUnit ) ) );
   node->SetAlignment( 14, TextAlign::Center );

   node->SetText( 15, String().Format( "%.4f", item->noiseRatio ) );
   node->SetAlignment( 15, TextAlign::Center );

   node->SetText( 16, String().Format( "%.4e", item->Median( m_instance.p_cameraGain,
                                 TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                 m_instance.p_dataUnit ) ) );
   node->SetAlignment( 16, TextAlign::Center );

   node->SetText( 17, String().Format( "%.4f", item->FWHMMeanDeviation( m_instance.p_subframeScale, m_instance.p_scaleUnit ) ) );
   node->SetAlignment( 17, TextAlign::Center );

   node->SetText( 18, String().Format( "%.4f", item->eccentricityMeanDev ) );
   node->SetAlignment( 18, TextAlign::Center );

   node->SetText( 19, String().Format( "%.4f", item->starResidualMeanDev ) );
   node->SetAlignment( 19, TextAlign::Center );

   node->SetText( 20, String().Format( "%.4f", item->MedianMeanDev( m_instance.p_cameraGain,
                                 TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                 m_instance.p_dataUnit ) ) );
   node->SetAlignment( 20, TextAlign::Center );
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementImagesList()
{
   GUI->MeasurementsTable_SortingProperty_ComboBox.SetCurrentItem(
            s_sortingPropertyToComboBoxItemIndex[m_instance.p_sortingProperty] );

   // Ensure Approval/Weighting is updated
   TheSubframeSelectorExpressionsInterface->ApplyWeightingExpression();
   TheSubframeSelectorExpressionsInterface->ApplyApprovalExpression();

   GUI->MeasurementTable_TreeBox.DisableUpdates();

   // Removing/Adding all the items can be slow; try to update each items text if possible
   bool shouldRecreate = m_instance.o_measures.Length()
                         != size_type( GUI->MeasurementTable_TreeBox.NumberOfChildren() );

   // Store all current selections to re-select later
   const IndirectArray<TreeBox::Node>& selections = GUI->MeasurementTable_TreeBox.SelectedNodes();
   Array<int> currentIds( selections.Length() );
   for ( size_type i = 0; i < selections.Length(); ++i )
      currentIds[i] = GUI->MeasurementTable_TreeBox.ChildIndex( selections[i] );

   // When the number of items changes, it's easier to add all items over again
   if ( shouldRecreate )
      GUI->MeasurementTable_TreeBox.Clear();

   // Ensure items are sorted properly
   Array<MeasureItem> measuresSorted( m_instance.o_measures );
   measuresSorted.Sort( SubframeSortingBinaryPredicate( m_instance.p_sortingProperty,
                                    GUI->MeasurementsTable_SortingMode_ComboBox.CurrentItem() ) );

   // Update the table
   size_type amount = measuresSorted.Length();
   for ( size_type i = 0; i < amount; ++i )
   {
      if ( shouldRecreate )
         new TreeBox::Node( GUI->MeasurementTable_TreeBox );
      UpdateMeasurementImageItem( i, &measuresSorted[i] );
   }

   for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfColumns(); i < n; ++i )
      GUI->MeasurementTable_TreeBox.AdjustColumnWidthToContents( i );

   // If the table was cleared, setup previous selections
   if ( shouldRecreate && !m_instance.o_measures.IsEmpty() )
      for ( size_type i = 0; i < currentIds.Length(); ++i )
         if ( currentIds[i] >= 0 )
            if ( currentIds[i] < GUI->MeasurementTable_TreeBox.NumberOfChildren() )
               GUI->MeasurementTable_TreeBox.Child( currentIds[i] )->Select();

   GUI->MeasurementTable_TreeBox.EnableUpdates();
   UpdateMeasurementQuantity();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementGraph()
{
   GUI->MeasurementGraph_WebViewProperty_ComboBox.SetCurrentItem(
            s_graphPropertyToComboBoxItemIndex[m_instance.p_graphProperty] );

   DataPointVector dataset( m_instance.o_measures.Length() );
   for ( size_type i = 0; i < m_instance.o_measures.Length(); ++i )
   {
      dataset[i].x = m_instance.o_measures[i].index + 1; // N.B. 1-based index, 0-based arrays
      switch ( m_instance.p_graphProperty )
      {
      default: // ?!
      case SSGraphProperty::Weight:
         dataset[i].data = m_instance.o_measures[i].weight;
         break;
      case SSGraphProperty::FWHM:
         dataset[i].data = m_instance.o_measures[i].FWHM( m_instance.p_subframeScale, m_instance.p_scaleUnit );
         break;
      case SSGraphProperty::Eccentricity:
         dataset[i].data = m_instance.o_measures[i].eccentricity;
         break;
      case SSGraphProperty::PSFSignalWeight:
         dataset[i].data = m_instance.o_measures[i].psfSignalWeight;
         break;
      case SSGraphProperty::PSFPowerWeight:
         dataset[i].data = m_instance.o_measures[i].psfPowerWeight;
         break;
      case SSGraphProperty::SNRWeight:
         dataset[i].data = m_instance.o_measures[i].snrWeight;
         break;
      case SSGraphProperty::Median:
         dataset[i].data = m_instance.o_measures[i].Median( m_instance.p_cameraGain,
                              TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                              m_instance.p_dataUnit );
         break;
      case SSGraphProperty::MedianMeanDev:
         dataset[i].data = m_instance.o_measures[i].MedianMeanDev( m_instance.p_cameraGain,
                              TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                              m_instance.p_dataUnit );
         break;
      case SSGraphProperty::Noise:
         dataset[i].data = m_instance.o_measures[i].Noise( m_instance.p_cameraGain,
                              TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                              m_instance.p_dataUnit );
         break;
      case SSGraphProperty::NoiseRatio:
         dataset[i].data = m_instance.o_measures[i].noiseRatio;
         break;
      case SSGraphProperty::Stars:
         dataset[i].data = m_instance.o_measures[i].stars;
         break;
      case SSGraphProperty::StarResidual:
         dataset[i].data = m_instance.o_measures[i].starResidual;
         break;
      case SSGraphProperty::FWHMMeanDev:
         dataset[i].data = m_instance.o_measures[i].FWHMMeanDeviation( m_instance.p_subframeScale, m_instance.p_scaleUnit );
         break;
      case SSGraphProperty::EccentricityMeanDev:
         dataset[i].data = m_instance.o_measures[i].eccentricityMeanDev;
         break;
      case SSGraphProperty::StarResidualMeanDev:
         dataset[i].data = m_instance.o_measures[i].starResidualMeanDev;
         break;
      case SSGraphProperty::Azimuth:
         dataset[i].data = m_instance.o_measures[i].azimuth;
         break;
      case SSGraphProperty::Altitude:
         dataset[i].data = m_instance.o_measures[i].altitude;
         break;
      }
      dataset[i].weight = m_instance.o_measures[i].weight;
      dataset[i].approved = m_instance.o_measures[i].enabled;
      dataset[i].locked = m_instance.o_measures[i].locked;
   }

   GUI->MeasurementGraph_WebView.SetDataset( TheSSGraphPropertyParameter->ElementLabel( m_instance.p_graphProperty ),
                                           &dataset );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::SetMeasurements( const Array<MeasureItem>& measures )
{
   m_instance.o_measures = measures;
   UpdateControls();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::ExportCSV() const
{
   SaveFileDialog d;
   d.SetCaption( "SubframeSelector: Save CSV File" );
   d.SetFilter( FileFilter( "CSV", ".csv" ) );
   d.EnableOverwritePrompt();
   if ( d.Execute() )
   {
      String filePath = d.FileName();
      Console().WriteLn( "Generating output CSV file: " + filePath );

      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Writing CSV file", m_instance.o_measures.Length() );

      IsoStringList lines;
      lines << Module->ReadableVersion()
            << IsoString().Format( "Subframe Scale,%.5f", m_instance.p_subframeScale )
            << IsoString().Format( "Camera Gain,%.5f", m_instance.p_cameraGain )
            << "Camera Resolution,\"" + TheSSCameraResolutionParameter->ElementLabel( m_instance.p_cameraResolution ) + "\""
            << IsoString().Format( "Site Local Midnight,%d", m_instance.p_siteLocalMidnight )
            << "Scale Unit,\"" + TheSSScaleUnitParameter->ElementLabel( m_instance.p_scaleUnit ) + "\""
            << "Data Unit,\"" + TheSSDataUnitParameter->ElementLabel( m_instance.p_dataUnit ) + "\""
            << IsoString().Format( "Trimming Factor,%.2f", m_instance.p_trimmingFactor )
            << IsoString().Format( "Structure Layers,%d", m_instance.p_structureLayers )
            << IsoString().Format( "Noise Layers,%d", m_instance.p_noiseLayers )
            << IsoString().Format( "Hot Pixel Filter Radius,%d", m_instance.p_hotPixelFilterRadius )
            << IsoString().Format( "Noise Reduction Filter Radius,%d", m_instance.p_noiseReductionFilterRadius )
            << IsoString().Format( "Sensitivity,%.5f", m_instance.p_sensitivity )
            << IsoString().Format( "Peak Response,%.3f", m_instance.p_peakResponse )
            << IsoString().Format( "Maximum Star Distortion,%.3f", m_instance.p_maxDistortion )
            << IsoString().Format( "Upper Limit,%.3f", m_instance.p_upperLimit )
            << IsoString().Format( "Pedestal,%d", m_instance.p_pedestal )
            << IsoString().Format( "Subframe Region,%d,%d,%d,%d",
                                   m_instance.p_roi.Left(), m_instance.p_roi.Top(),
                                   m_instance.p_roi.Width(), m_instance.p_roi.Height() )
            << "PSF Type,\"" + TheSSPSFFitParameter->ElementLabel( m_instance.p_psfFit ) + "\""
            << IsoString().Format( "Circular PSF,%s", m_instance.p_psfFitCircular ? "true" : "false" )
            << "Approval expression,\"" + m_instance.p_approvalExpression + "\""
            << "Weighting expression,\"" + m_instance.p_weightingExpression + "\""
            << "Index,Approved,Locked,File,Weight,PSF Signal Weight,PSF Power Weight,SNR Weight,FWHM,"
               "Eccentricity,Altitude,Azimuth,Median,Median Mean Deviation,Noise,Noise Ratio,Stars,Star Residual,FWHM Mean Deviation,"
               "Eccentricity Mean Deviation,Star Residual Mean Deviation";

      for ( const MeasureItem& i : m_instance.o_measures )
      {
         lines << IsoString( i.index + 1 ) + ',' + // N.B. 1-based index, 0-based arrays
                  (i.enabled ? "true," : "false,") +
                  (i.locked ? "true," : "false,") +
                  '"' + i.path.ToUTF8() + '"' + ',' +
                  IsoString().Format( "%.6e,%.6e,%.6e,%.6e,%.4f,%.4f,%.4f,%.4f,%.6e,%.6e,%.4e,%.4f,%d,%.4f,%.4f,%.4f,%.4f",
                     i.weight,
                     i.psfSignalWeight,
                     i.psfPowerWeight,
                     i.snrWeight,
                     i.FWHM( m_instance.p_subframeScale, m_instance.p_scaleUnit ),
                     i.eccentricity,
                     i.altitude,
                     i.azimuth,
                     i.Median( m_instance.p_cameraGain,
                               TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                               m_instance.p_dataUnit ),
                     i.MedianMeanDev( m_instance.p_cameraGain,
                                      TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                      m_instance.p_dataUnit ),
                     i.Noise( m_instance.p_cameraGain,
                              TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                              m_instance.p_dataUnit ),
                     i.noiseRatio,
                     i.stars,
                     i.starResidual,
                     i.FWHMMeanDeviation( m_instance.p_subframeScale, m_instance.p_scaleUnit ),
                     i.eccentricityMeanDev,
                     i.starResidualMeanDev );
         ++monitor;
      }

      lines << IsoString(); // make sure the file ends with a newline.

      File::WriteTextFile( filePath, IsoString().ToNewLineSeparated( lines ) );

      monitor.Complete();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::ExportPDF() const
{
   SaveFileDialog d;
   d.SetCaption( "SubframeSelector: Save PDF File" );
   d.SetFilter( FileFilter( "PDF", ".PDF" ) );
   d.EnableOverwritePrompt();
   if ( d.Execute() )
   {
      Console console;
      String filePath = d.FileName();
      //File f = File::CreateFileForWriting( filePath );
      console.WriteLn( "Generating output PDF file: " + filePath );
      GUI->MeasurementGraph_WebView.SaveAsPDF( filePath, 210/*width*/, 297/*height*/, 5, 5, 5, 5/*margins*/, true/*landscape*/ );
      console.WriteLn( "Generated PDF file: " + filePath );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
   {
      GUI->MeasurementTable_TreeBox.SetFixedHeight();
      GUI->MeasurementGraph_Control.SetFixedHeight();
   }
   else
   {
      GUI->MeasurementTable_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->MeasurementTable_TreeBox.SetMaxHeight( int_max );
      GUI->MeasurementGraph_Control.SetScaledMinHeight( 360 );
      GUI->MeasurementGraph_Control.SetMaxHeight( int_max );
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_CurrentNodeUpdated( TreeBox& sender,
                                                   TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.o_measures.Length() )
      throw Error( "SubframeSelectorMeasurementsInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= m_instance.o_measures.Length() )
      throw Error( "SubframeSelectorMeasurementsInterface: *Warning* Corrupted interface structures" );

   MeasureItem* item = GetMeasurementItem( index );
   if ( item == nullptr )
      return;

   switch ( col )
   {
   case 0:
      // Activate the item's index number: ignore.
      break;
   case 1:
      // Activate the item's checkmark: toggle item's enabled state.
      item->enabled = !item->enabled;
      item->locked = true;
      UpdateMeasurementImageItem( index, item );
      UpdateMeasurementQuantity();
      UpdateMeasurementGraph();
      break;
   case 2:
      // Activate the item's checkmark: toggle item's locked state.
      item->locked = !item->locked;
      UpdateMeasurementImageItem( index, item );
      UpdateMeasurementQuantity();
      UpdateMeasurementGraph();
      break;
   case 3:
      {
         // Activate the item's path: open the image.
         Array<ImageWindow> windows = ImageWindow::Open( item->path, IsoString()/*id*/ );
         for ( ImageWindow& window : windows )
            window.Show();
      }
      break;
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->MeasurementsTable_ToggleApproved_PushButton )
   {
      for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->MeasurementTable_TreeBox[i]->IsSelected() )
         {
            MeasureItem* item = GetMeasurementItem( i );
            if ( item == nullptr )
               continue;

            item->enabled = !item->enabled;
            item->locked = true;
         }
      UpdateControls();
   }
   else if ( sender == GUI->MeasurementsTable_ToggleLocked_PushButton )
   {
      for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->MeasurementTable_TreeBox[i]->IsSelected() )
         {
            MeasureItem* item = GetMeasurementItem( i );
            if ( item == nullptr )
               continue;

            item->locked = !item->locked;
         }
      UpdateControls();
   }
   else if ( sender == GUI->MeasurementsTable_Invert_PushButton )
   {
      for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->MeasurementTable_TreeBox[i]->Select( !GUI->MeasurementTable_TreeBox[i]->IsSelected() );
   }
   else if ( sender == GUI->MeasurementsTable_Remove_PushButton )
   {
      Array<MeasureItem> newMeasures;
      uint16 index = 0;
      for ( size_type i = 0; i < m_instance.o_measures.Length(); ++i )
      {
         MeasureItem* measure = m_instance.o_measures.At( i );
         TreeBox::Node* measureNode = GetMeasurementNode( measure );
         if ( measureNode == nullptr )
            continue;

         if ( !measureNode->IsSelected() )
         {
            measure->index = index++;
            newMeasures.Add( *measure );
         }
      }
      m_instance.o_measures = newMeasures;
      UpdateControls();
   }
   else if ( sender == GUI->MeasurementsTable_Clear_PushButton )
   {
      m_instance.o_measures.Clear();
      UpdateControls();
   }
   else if ( sender == GUI->MeasurementsTable_ExportCSV_PushButton )
   {
      ExportCSV();
   }
   else if ( sender == GUI->MeasurementGraph_ExportPDF_PushButton )
   {
      ExportPDF();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_GraphApprove( GraphWebView &sender, int index )
{
   if ( index < 1 || index > int( m_instance.o_measures.Length() ) )
      return;

   --index; // 1-based to 0-based

   MeasureItem* item = m_instance.o_measures.At( index );
   item->enabled = !item->enabled;
   item->locked = true;

   TreeBox::Node* node = GetMeasurementNode( item );
   if ( node == nullptr )
      return;

   GUI->MeasurementTable_TreeBox.SetCurrentNode( node );
   GUI->MeasurementTable_TreeBox.SetNodeIntoView( node );
   UpdateMeasurementImageItem( GUI->MeasurementTable_TreeBox.ChildIndex( node ), item );
   UpdateMeasurementQuantity();
   UpdateMeasurementGraph();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_GraphUnlock( GraphWebView &sender, int index )
{
   if ( index < 1 || index > int( m_instance.o_measures.Length() ) )
      return;

   --index; // 1-based to 0-based

   MeasureItem* item = m_instance.o_measures.At( index );
   item->locked = false;

   TreeBox::Node* node = GetMeasurementNode( item );
   if ( node == nullptr )
      return;

   GUI->MeasurementTable_TreeBox.SetCurrentNode( node );
   GUI->MeasurementTable_TreeBox.SetNodeIntoView( node );
   TheSubframeSelectorExpressionsInterface->ApplyApprovalExpression();
   UpdateMeasurementImageItem( GUI->MeasurementTable_TreeBox.ChildIndex( node ), item );
   UpdateMeasurementQuantity();
   UpdateMeasurementGraph();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->MeasurementsTable_SortingProperty_ComboBox )
   {
      m_instance.p_sortingProperty = s_comboBoxItemIndexToSortingProperty[itemIndex];
      UpdateMeasurementImagesList();
   }
   else if ( sender == GUI->MeasurementsTable_SortingMode_ComboBox )
   {
      UpdateMeasurementImagesList();
   }
   else if ( sender == GUI->MeasurementGraph_WebViewProperty_ComboBox )
   {
      m_instance.p_graphProperty = s_comboBoxItemIndexToGraphProperty[itemIndex];
      UpdateMeasurementGraph();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

SubframeSelectorMeasurementsInterface::GUIData::GUIData( SubframeSelectorMeasurementsInterface& w )
   : MeasurementGraph_WebView( w )
   , MeasurementDistributionGraph_WebView( w )
{
   int buttonWidth1 = w.Font().Width( String( "Toggle Approve" ) + "MM" );
   String buttonStyleSheet = String().Format( "QPushButton { min-width: %dpx; }", buttonWidth1 );

   MeasurementTable_SectionBar.SetTitle( "Measurements Table" );
   MeasurementTable_SectionBar.SetSection( MeasurementTable_Control );
   MeasurementTable_SectionBar.OnToggleSection( (SectionBar::section_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ToggleSection, w );

   for ( size_type i = 0; i < TheSSSortingPropertyParameter->NumberOfElements(); ++i )
      MeasurementsTable_SortingProperty_ComboBox.AddItem(
            TheSSSortingPropertyParameter->ElementLabel( s_comboBoxItemIndexToSortingProperty[i] ) );
   MeasurementsTable_SortingProperty_ComboBox.SetToolTip( "<p>Sorting property</p>" );
   MeasurementsTable_SortingProperty_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ItemSelected, w );

   MeasurementsTable_SortingMode_ComboBox.AddItem( "Asc." );
   MeasurementsTable_SortingMode_ComboBox.AddItem( "Desc." );
   MeasurementsTable_SortingMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ItemSelected, w );

   MeasurementsTable_ToggleApproved_PushButton.SetText( "Toggle Approve" );
   MeasurementsTable_ToggleApproved_PushButton.SetToolTip( "<p>Toggle the approved state of currently selected measurements.</p>" );
   MeasurementsTable_ToggleApproved_PushButton.SetStyleSheet( buttonStyleSheet );
   MeasurementsTable_ToggleApproved_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_ToggleLocked_PushButton.SetText( "Toggle Lock" );
   MeasurementsTable_ToggleLocked_PushButton.SetToolTip( "<p>Toggle the locked state of currently selected measurements.</p>" );
   MeasurementsTable_ToggleLocked_PushButton.SetStyleSheet( buttonStyleSheet );
   MeasurementsTable_ToggleLocked_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_Invert_PushButton.SetText( "Invert" );
   MeasurementsTable_Invert_PushButton.SetToolTip( "<p>Invert the current selection of measurements.</p>" );
   MeasurementsTable_Invert_PushButton.SetStyleSheet( buttonStyleSheet );
   MeasurementsTable_Invert_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_Remove_PushButton.SetText( "Remove" );
   MeasurementsTable_Remove_PushButton.SetToolTip( "<p>Remove the selected measurements.</p>" );
   MeasurementsTable_Remove_PushButton.SetStyleSheet( buttonStyleSheet );
   MeasurementsTable_Remove_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_Clear_PushButton.SetText( "Clear" );
   MeasurementsTable_Clear_PushButton.SetToolTip( "<p>Clear the list of measurements.</p>" );
   MeasurementsTable_Clear_PushButton.SetStyleSheet( buttonStyleSheet );
   MeasurementsTable_Clear_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_ExportCSV_PushButton.SetText( "Save CSV" );
   MeasurementsTable_ExportCSV_PushButton.SetToolTip( "<p>Export the table as a CSV file.</p>" );
   MeasurementsTable_ExportCSV_PushButton.SetStyleSheet( buttonStyleSheet );
   MeasurementsTable_ExportCSV_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_Top1_Sizer.SetSpacing( 4 );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_SortingProperty_ComboBox );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_SortingMode_ComboBox );
   MeasurementsTable_Top1_Sizer.AddStretch( 100 );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_ToggleApproved_PushButton );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_ToggleLocked_PushButton );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_Invert_PushButton );

   MeasurementsTable_Top2_Sizer.SetSpacing( 4 );
   MeasurementsTable_Top2_Sizer.Add( MeasurementsTable_Quantities_Label );
   MeasurementsTable_Top2_Sizer.AddStretch( 100 );
   MeasurementsTable_Top2_Sizer.Add( MeasurementsTable_Remove_PushButton );
   MeasurementsTable_Top2_Sizer.Add( MeasurementsTable_Clear_PushButton );
   MeasurementsTable_Top2_Sizer.Add( MeasurementsTable_ExportCSV_PushButton );

   MeasurementTable_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( w.Font() ) );
   MeasurementTable_TreeBox.SetScaledMinWidth( 400 );
   MeasurementTable_TreeBox.SetNumberOfColumns( 22 );
   MeasurementTable_TreeBox.SetHeaderText( 0, "Index" );
   MeasurementTable_TreeBox.SetScaledColumnWidth( 0, 40 );
   MeasurementTable_TreeBox.SetHeaderIcon( 1, Bitmap( w.ScaledResource( ":/icons/picture-ok.png" ) ) );
   MeasurementTable_TreeBox.SetHeaderText( 1, "" );
   MeasurementTable_TreeBox.SetScaledColumnWidth( 1, 30 );
   MeasurementTable_TreeBox.SetHeaderIcon( 2, Bitmap( w.ScaledResource( ":/icons/function-import.png" ) ) );
   MeasurementTable_TreeBox.SetHeaderText( 2, "" );
   MeasurementTable_TreeBox.SetScaledColumnWidth( 2, 30 );
   MeasurementTable_TreeBox.SetHeaderText( 3, "Filename" );
   for ( int i = 0; i <= 20; ++i )
   {
      MeasurementTable_TreeBox.SetHeaderAlignment( i, TextAlign::Center|TextAlign::VertCenter );
      if ( i >= 4 )
         MeasurementTable_TreeBox.SetHeaderText( i,
                  TheSSSortingPropertyParameter->ElementLabel( s_comboBoxItemIndexToSortingProperty[i-3] ) );
   }

   MeasurementTable_TreeBox.SetHeaderText( 21, "" ); // blank 'spacer' column
   MeasurementTable_TreeBox.EnableMultipleSelections();
   MeasurementTable_TreeBox.DisableRootDecoration();
   MeasurementTable_TreeBox.EnableAlternateRowColor();
#ifndef __PCL_MACOSX
   /*
    * ### TODO - Qt 5.15.7 LTS: this doesn't work on macOS... bug? idiosyncrasy?
    */
   MeasurementTable_TreeBox.SetStyleSheet( String().Format(
                              "* QHeaderView { font-size: %.2fpt; }", 0.9*w.Font().PointSize() ) );
#endif
   MeasurementTable_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_CurrentNodeUpdated, w );
   MeasurementTable_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_NodeActivated, w );

   MeasurementTable_Sizer.SetSpacing( 4 );
   MeasurementTable_Sizer.Add( MeasurementsTable_Top1_Sizer );
   MeasurementTable_Sizer.Add( MeasurementsTable_Top2_Sizer );
   MeasurementTable_Sizer.Add( MeasurementTable_TreeBox, 100 );

   MeasurementTable_Control.SetSizer( MeasurementTable_Sizer );

   //

   MeasurementGraph_SectionBar.SetTitle( "Measurements Graph" );
   MeasurementGraph_SectionBar.SetSection( MeasurementGraph_Control );
   MeasurementGraph_SectionBar.OnToggleSection( (SectionBar::section_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ToggleSection, w );

   for ( size_type i = 0; i < TheSSGraphPropertyParameter->NumberOfElements(); ++i )
      MeasurementGraph_WebViewProperty_ComboBox.AddItem(
            TheSSGraphPropertyParameter->ElementLabel( s_comboBoxItemIndexToGraphProperty[i] ) );

   MeasurementGraph_WebViewProperty_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ItemSelected, w );

   MeasurementGraph_ExportPDF_PushButton.SetText( "Save PDF" );
   MeasurementGraph_ExportPDF_PushButton.SetToolTip( "<p>Export the graph as a PDF file.</p>" );
   MeasurementGraph_ExportPDF_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementGraph_Top_Sizer.SetSpacing( 4 );
   MeasurementGraph_Top_Sizer.Add( MeasurementGraph_WebViewProperty_ComboBox );
   MeasurementGraph_Top_Sizer.Add( MeasurementGraph_ExportPDF_PushButton );

   MeasurementGraph_WebView.SetMinHeight( IMAGELIST_MINHEIGHT( w.Font() ) );
   MeasurementGraph_WebView.SetZoomFactor( MeasurementGraph_WebView.DisplayPixelRatio() );
   MeasurementGraph_WebView.SetBackgroundColor( MeasurementGraph_Control.BackgroundColor() );
   MeasurementGraph_WebView.OnApprove( (GraphWebView::approve_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_GraphApprove, w );
   MeasurementGraph_WebView.OnUnlock( (GraphWebView::unlock_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_GraphUnlock, w );

   MeasurementGraph_Sizer.SetSpacing( 4 );
   MeasurementGraph_Sizer.Add( MeasurementGraph_Top_Sizer );
   MeasurementGraph_Sizer.Add( MeasurementGraph_WebView, 100 );
   MeasurementGraph_Sizer.SetAlignment( MeasurementGraph_Top_Sizer, Sizer::item_alignment::Center );

   MeasurementGraph_Control.SetSizer( MeasurementGraph_Sizer );
   MeasurementGraph_Control.SetScaledMinSize( 640, 360 );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( MeasurementTable_SectionBar );
   Global_Sizer.Add( MeasurementTable_Control, 50 );
   Global_Sizer.Add( MeasurementGraph_SectionBar );
   Global_Sizer.Add( MeasurementGraph_Control, 100 );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasurementsInterface.cpp - Released 2021-11-11T17:56:06Z
