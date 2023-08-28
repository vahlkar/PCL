//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.8.8
// ----------------------------------------------------------------------------
// SubframeSelectorMeasurementsInterface.cpp - Released 2023-08-28T15:23:41Z
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
   SSSortingProperty::PSFSNR,
   SSSortingProperty::PSFScaleSNR,
   SSSortingProperty::SNRWeight,
   SSSortingProperty::FWHM,
   SSSortingProperty::Eccentricity,
   SSSortingProperty::Stars,
   SSSortingProperty::Altitude,
   SSSortingProperty::Azimuth,
   SSSortingProperty::Noise,
   SSSortingProperty::NoiseRatio,
   SSSortingProperty::MStar,
   SSSortingProperty::NStar,
   SSSortingProperty::PSFFlux,
   SSSortingProperty::PSFFluxPower,
   SSSortingProperty::PSFTotalMeanFlux,
   SSSortingProperty::PSFTotalMeanPowerFlux,
   SSSortingProperty::PSFCount,
   SSSortingProperty::PSFScale,
   SSSortingProperty::StarResidual,
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
   SSGraphProperty::PSFSNR,
   SSGraphProperty::PSFScaleSNR,
   SSGraphProperty::SNRWeight,
   SSGraphProperty::FWHM,
   SSGraphProperty::Eccentricity,
   SSGraphProperty::Stars,
   SSGraphProperty::Altitude,
   SSGraphProperty::Azimuth,
   SSGraphProperty::Noise,
   SSGraphProperty::NoiseRatio,
   SSGraphProperty::MStar,
   SSGraphProperty::NStar,
   SSGraphProperty::PSFFlux,
   SSGraphProperty::PSFFluxPower,
   SSGraphProperty::PSFTotalMeanFlux,
   SSGraphProperty::PSFTotalMeanPowerFlux,
   SSGraphProperty::PSFCount,
   SSGraphProperty::PSFScale,
   SSGraphProperty::StarResidual,
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
      GUI->MeasurementTable_TreeBox.Clear();
      GUI->MeasurementGraph_WebView.Cleanup();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class MeasurementNode : public TreeBox::Node
{
public:

      MeasurementNode() = default;

      MeasurementNode( TreeBox::Node& parent, MeasureItem* item, int index )
         : TreeBox::Node( parent, index )
         , m_item( item )
      {
      }

      MeasurementNode( TreeBox& parentTree, MeasureItem* item, int index = -1 )
         : TreeBox::Node( parentTree, index )
         , m_item( item )
      {
      }

      const MeasureItem* Item() const
      {
         return m_item;
      }

      MeasureItem* Item()
      {
         return m_item;
      }

private:

   MeasureItem* m_item = nullptr;
};

// ----------------------------------------------------------------------------

MeasurementNode* SubframeSelectorMeasurementsInterface::MeasurementNodeByItem( const MeasureItem* item ) const
{
   if ( item != nullptr )
      for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         MeasurementNode* node = static_cast<MeasurementNode*>( GUI->MeasurementTable_TreeBox[i] );
         if ( node->Item() == item )
            return node;
      }
   return nullptr;
}

// ----------------------------------------------------------------------------

MeasureItem* SubframeSelectorMeasurementsInterface::MeasureItemByGraphIndex( int index ) const
{
   if ( --index >= 0 ) // 1-based to 0-based
      for ( MeasureItem& item : m_instance.o_measures )
         if ( item.index == unsigned( index ) )
            return &item;
   return nullptr;
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateControls()
{
   UpdateMeasurementTable();
   UpdateMeasurementGraph();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementQuantity()
{
   if ( m_instance.o_measures.IsEmpty() )
      GUI->MeasurementsTable_Quantities_Label.SetText( "No measurements" );
   else
   {
      size_type approved = 0;
      size_type locked = 0;
      size_type amount = m_instance.o_measures.Length();
      for ( const MeasureItem& item : m_instance.o_measures )
      {
         if ( item.enabled )
            ++approved;
         if ( item.locked )
            ++locked;
      }
      GUI->MeasurementsTable_Quantities_Label.SetText(
         String().Format( "%u/%u Approved (%.2f%%), %u Locked (%.2f%%)",
                          approved, amount, 100.0*approved/amount,
                          locked, 100.0*locked/amount ) );
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementNode( MeasurementNode* node )
{
   if ( node == nullptr )
      return;
   MeasureItem* item = node->Item();
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

   node->SetText( 6, String().Format( "%.4e", item->psfSNR ) );
   node->SetAlignment( 6, TextAlign::Center );

   node->SetText( 7, String().Format( "%.4e", item->psfScaleSNR ) );
   node->SetAlignment( 7, TextAlign::Center );

   node->SetText( 8, String().Format( "%.4e", item->snrWeight ) );
   node->SetAlignment( 8, TextAlign::Center );

   node->SetText( 9, String().Format( "%.4f", item->FWHM( m_instance.p_subframeScale, m_instance.p_scaleUnit ) ) );
   node->SetAlignment( 9, TextAlign::Center );

   node->SetText( 10, String().Format( "%.4f", item->eccentricity ) );
   node->SetAlignment( 10, TextAlign::Center );

   node->SetText( 11, String().Format( "%u", unsigned( item->stars ) ) );
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

   node->SetText( 16, String().Format( "%.4e", item->MStar ) );
   node->SetAlignment( 16, TextAlign::Center );

   node->SetText( 17, String().Format( "%.4e", item->NStar ) );
   node->SetAlignment( 17, TextAlign::Center );

   node->SetText( 18, String().Format( "%.4e", item->psfFlux ) );
   node->SetAlignment( 18, TextAlign::Center );

   node->SetText( 19, String().Format( "%.4e", item->psfFluxPower ) );
   node->SetAlignment( 19, TextAlign::Center );

   node->SetText( 20, String().Format( "%.4e", item->psfTotalMeanFlux ) );
   node->SetAlignment( 20, TextAlign::Center );

   node->SetText( 21, String().Format( "%.4e", item->psfTotalMeanPowerFlux ) );
   node->SetAlignment( 21, TextAlign::Center );

   node->SetText( 22, String().Format( "%u", unsigned( item->psfCount ) ) );
   node->SetAlignment( 22, TextAlign::Center );

   node->SetText( 23, String().Format( "%.4e", item->psfScale ) );
   node->SetAlignment( 23, TextAlign::Center );

   node->SetText( 24, String().Format( "%.4e", item->starResidual ) );
   node->SetAlignment( 24, TextAlign::Center );

   node->SetText( 25, String().Format( "%.4e", item->Median( m_instance.p_cameraGain,
                                 TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                 m_instance.p_dataUnit ) ) );
   node->SetAlignment( 25, TextAlign::Center );

   node->SetText( 26, String().Format( "%.4e", item->FWHMMeanDeviation( m_instance.p_subframeScale, m_instance.p_scaleUnit ) ) );
   node->SetAlignment( 26, TextAlign::Center );

   node->SetText( 27, String().Format( "%.4e", item->eccentricityMeanDev ) );
   node->SetAlignment( 27, TextAlign::Center );

   node->SetText( 28, String().Format( "%.4e", item->starResidualMeanDev ) );
   node->SetAlignment( 28, TextAlign::Center );

   node->SetText( 29, String().Format( "%.4e", item->MedianMeanDev( m_instance.p_cameraGain,
                                 TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                 m_instance.p_dataUnit ) ) );
   node->SetAlignment( 29, TextAlign::Center );
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementTable()
{
   GUI->MeasurementsTable_SortingProperty_ComboBox.SetCurrentItem(
            s_sortingPropertyToComboBoxItemIndex[m_instance.p_sortingProperty] );

   // Ensure Approval/Weighting is updated
   TheSubframeSelectorExpressionsInterface->ApplyWeightingExpression();
   TheSubframeSelectorExpressionsInterface->ApplyApprovalExpression();

   // Store current item selection to re-select later
   MeasureItemList selectedItems;
   {
      IndirectArray<TreeBox::Node> selectedNodes = GUI->MeasurementTable_TreeBox.SelectedNodes();
      for ( TreeBox::Node* node : selectedNodes )
         selectedItems << *static_cast<MeasurementNode*>( node )->Item();
   }

   // Prevent interface updates while we are modifying the table
   GUI->MeasurementTable_TreeBox.DisableUpdates();
   GUI->MeasurementTable_TreeBox.Clear();

   // Ensure items are sorted properly
   m_instance.o_measures.Sort( SubframeSortingBinaryPredicate( m_instance.p_sortingProperty,
                                    GUI->MeasurementsTable_SortingMode_ComboBox.CurrentItem() ) );

   // Regenerate the table
   for ( MeasureItem& item : m_instance.o_measures )
   {
      MeasurementNode* node = new MeasurementNode( GUI->MeasurementTable_TreeBox, &item );
      UpdateMeasurementNode( node );
      // Re-select previously selected items
      if ( selectedItems.Contains( item ) )
         node->Select();
   }

   // Adjust all table columns
   for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfColumns(); i < n; ++i )
      GUI->MeasurementTable_TreeBox.AdjustColumnWidthToContents( i );

   // Update everything
   GUI->MeasurementTable_TreeBox.EnableUpdates();
   UpdateMeasurementQuantity();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementGraph()
{
   GUI->MeasurementGraph_Property_ComboBox.SetCurrentItem(
            s_graphPropertyToComboBoxItemIndex[m_instance.p_graphProperty] );
   GUI->MeasurementGraph_AuxProperty_ComboBox.SetCurrentItem(
            s_graphPropertyToComboBoxItemIndex[m_instance.p_auxGraphProperty] );

   DataPointVector dataset( int( m_instance.o_measures.Length() ) );
   int i = 0;
   for ( const MeasureItem& item : m_instance.o_measures )
   {
      DataPoint& point = dataset[i++];

      point.x = item.index + 1; // N.B. 1-based table/graph index, 0-based arrays

      for ( int j = 0; j < 2; ++j )
      {
         double data;
         switch ( (j == 0) ? m_instance.p_graphProperty : m_instance.p_auxGraphProperty )
         {
         default: // ?!
         case SSGraphProperty::Weight:
            data = item.weight;
            break;
         case SSGraphProperty::FWHM:
            data = item.FWHM( m_instance.p_subframeScale, m_instance.p_scaleUnit );
            break;
         case SSGraphProperty::Eccentricity:
            data = item.eccentricity;
            break;
         case SSGraphProperty::PSFSignalWeight:
            data = item.psfSignalWeight;
            break;
         case SSGraphProperty::PSFSNR:
            data = item.psfSNR;
            break;
         case SSGraphProperty::PSFScale:
            data = item.psfScale;
            break;
         case SSGraphProperty::PSFScaleSNR:
            data = item.psfScaleSNR;
            break;
         case SSGraphProperty::PSFFlux:
            data = item.psfFlux;
            break;
         case SSGraphProperty::PSFFluxPower:
            data = item.psfFluxPower;
            break;
         case SSGraphProperty::PSFTotalMeanFlux:
            data = item.psfTotalMeanFlux;
            break;
         case SSGraphProperty::PSFTotalMeanPowerFlux:
            data = item.psfTotalMeanPowerFlux;
            break;
         case SSGraphProperty::PSFCount:
            data = item.psfCount;
            break;
         case SSGraphProperty::MStar:
            data = item.MStar;
            break;
         case SSGraphProperty::NStar:
            data = item.NStar;
            break;
         case SSGraphProperty::SNRWeight:
            data = item.snrWeight;
            break;
         case SSGraphProperty::Median:
            data = item.Median( m_instance.p_cameraGain,
                                TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                m_instance.p_dataUnit );
            break;
         case SSGraphProperty::MedianMeanDev:
            data = item.MedianMeanDev( m_instance.p_cameraGain,
                                       TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                       m_instance.p_dataUnit );
            break;
         case SSGraphProperty::Noise:
            data = item.Noise( m_instance.p_cameraGain,
                               TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                               m_instance.p_dataUnit );
            break;
         case SSGraphProperty::NoiseRatio:
            data = item.noiseRatio;
            break;
         case SSGraphProperty::Stars:
            data = item.stars;
            break;
         case SSGraphProperty::StarResidual:
            data = item.starResidual;
            break;
         case SSGraphProperty::FWHMMeanDev:
            data = item.FWHMMeanDeviation( m_instance.p_subframeScale, m_instance.p_scaleUnit );
            break;
         case SSGraphProperty::EccentricityMeanDev:
            data = item.eccentricityMeanDev;
            break;
         case SSGraphProperty::StarResidualMeanDev:
            data = item.starResidualMeanDev;
            break;
         case SSGraphProperty::Azimuth:
            data = item.azimuth;
            break;
         case SSGraphProperty::Altitude:
            data = item.altitude;
            break;
         }

         if ( j == 0 )
            point.data = data;
         else
            point.auxData = data;
      }

      point.approved = item.enabled;
      point.locked = item.locked;
   }

   GUI->MeasurementGraph_WebView.SetDataset( TheSSGraphPropertyParameter->ElementLabel( m_instance.p_graphProperty ),
                                             TheSSAuxGraphPropertyParameter->ElementLabel( m_instance.p_auxGraphProperty ),
                                             dataset );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::ExportCSV() const
{
   SaveFileDialog d;
   d.SetCaption( "SubframeSelector: Export CSV File" );
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

      IsoStringList headerItems;
         headerItems /* 00 */ << "Weight"
                     /* 01 */ << "PSF Signal Weight"
                     /* 02 */ << "PSF SNR"
                     /* 03 */ << "PSF Scale"
                     /* 04 */ << "PSF Scale SNR"
                     /* 05 */ << "PSF Count"
                     /* 06 */ << "M*"
                     /* 07 */ << "N*"
                     /* 08 */ << "SNR"
                     /* 09 */ << "FWHM"
                     /* 10 */ << "Eccentricity"
                     /* 11 */ << "Altitude"
                     /* 12 */ << "Azimuth"
                     /* 13 */ << "Median"
                     /* 14 */ << "Median Mean Deviation"
                     /* 15 */ << "Noise"
                     /* 16 */ << "Noise Ratio"
                     /* 17 */ << "Stars"
                     /* 18 */ << "Star Residual"
                     /* 19 */ << "PSF Total Flux"
                     /* 20 */ << "PSF Total Power Flux"
                     /* 21 */ << "PSF Total Mean Flux"
                     /* 22 */ << "PSF Total Mean Power Flux"
                     /* 23 */ << "FWHM Mean Deviation"
                     /* 24 */ << "Eccentricity Mean Deviation"
                     /* 25 */ << "Star Residual Mean Deviation";

      IsoStringList formatItems;
         formatItems /* 00 */ << "%.6e"
                     /* 01 */ << "%.6e"
                     /* 02 */ << "%.6e"
                     /* 03 */ << "%.6e"
                     /* 04 */ << "%.6e"
                     /* 05 */ << "%.0f"
                     /* 06 */ << "%.4e"
                     /* 07 */ << "%.4e"
                     /* 08 */ << "%.4e"
                     /* 09 */ << "%.4e"
                     /* 10 */ << "%.4e"
                     /* 11 */ << "%.4e"
                     /* 12 */ << "%.4e"
                     /* 13 */ << "%.6e"
                     /* 14 */ << "%.6e"
                     /* 15 */ << "%.4e"
                     /* 16 */ << "%.4e"
                     /* 17 */ << "%.0f"
                     /* 18 */ << "%.4e"
                     /* 19 */ << "%.6e"
                     /* 20 */ << "%.6e"
                     /* 21 */ << "%.6e"
                     /* 22 */ << "%.6e"
                     /* 23 */ << "%.4e"
                     /* 24 */ << "%.4e"
                     /* 25 */ << "%.4e";

      Array<Array<double>> csvData;
      for ( const MeasureItem& item : m_instance.o_measures )
         csvData  << (Array<double>()
                     /* 00 */ << item.weight
                     /* 01 */ << item.psfSignalWeight
                     /* 02 */ << item.psfSNR
                     /* 03 */ << item.psfScale
                     /* 04 */ << item.psfScaleSNR
                     /* 05 */ << item.psfCount
                     /* 06 */ << item.MStar
                     /* 07 */ << item.NStar
                     /* 08 */ << item.snrWeight
                     /* 09 */ << item.FWHM( m_instance.p_subframeScale, m_instance.p_scaleUnit )
                     /* 10 */ << item.eccentricity
                     /* 11 */ << item.altitude
                     /* 12 */ << item.azimuth
                     /* 13 */ << item.Median( m_instance.p_cameraGain, TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ), m_instance.p_dataUnit )
                     /* 14 */ << item.MedianMeanDev( m_instance.p_cameraGain, TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ), m_instance.p_dataUnit )
                     /* 15 */ << item.Noise( m_instance.p_cameraGain, TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ), m_instance.p_dataUnit )
                     /* 16 */ << item.noiseRatio
                     /* 17 */ << item.stars
                     /* 18 */ << item.starResidual
                     /* 19 */ << item.psfFlux
                     /* 20 */ << item.psfFluxPower
                     /* 21 */ << item.psfTotalMeanFlux
                     /* 22 */ << item.psfTotalMeanPowerFlux
                     /* 23 */ << item.FWHMMeanDeviation( m_instance.p_subframeScale, m_instance.p_scaleUnit )
                     /* 24 */ << item.eccentricityMeanDev
                     /* 25 */ << item.starResidualMeanDev);

      IsoStringList lines;

      // Newline characters must be represented as \n in CSV text fields.
      IsoString approvalExpression = m_instance.p_approvalExpression.ToUTF8().Trimmed();
      approvalExpression.ReplaceString( "\n", "\\n" );
      IsoString weightingExpression = m_instance.p_weightingExpression.ToUTF8().Trimmed();
      weightingExpression.ReplaceString( "\n", "\\n" );

      /*
       * CSV file header
       */
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
            << "Approval expression,\"" + approvalExpression + "\""
            << "Weighting expression,\"" + weightingExpression + "\"";

      /*
       * CSV table header
       */
      lines << "Index,Approved,Locked,File," + IsoString().ToCommaSeparated( headerItems );

      /*
       * CSV table rows
       */
      for ( size_type i = 0; i < m_instance.o_measures.Length(); ++i )
      {
         const MeasureItem& item = m_instance.o_measures[i];
         IsoStringList fields = IsoStringList()
               << IsoString( item.index + 1 ) // N.B. 1-based table/graph index, 0-based arrays
               << IsoString( bool( item.enabled ) )
               << IsoString( bool( item.locked ) )
               << '"' + item.path.ToUTF8() + '"';

         for ( size_type j = 0; j < formatItems.Length(); ++j )
            fields << IsoString().Format( formatItems[j].c_str(), csvData[i][j] );

         lines << IsoString().ToCommaSeparated( fields );

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
   d.SetCaption( "SubframeSelector: Export PDF File" );
   d.SetFilter( FileFilter( "PDF", ".pdf" ) );
   d.EnableOverwritePrompt();
   if ( d.Execute() )
   {
      Console console;
      String filePath = d.FileName();
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
   // placeholder
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   MeasurementNode& measurementNode = static_cast<MeasurementNode&>( node );
   MeasureItem* item = measurementNode.Item();
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
      UpdateMeasurementNode( &measurementNode );
      UpdateMeasurementQuantity();
      UpdateMeasurementGraph();
      break;
   case 2:
      // Activate the item's checkmark: toggle item's locked state.
      item->locked = !item->locked;
      UpdateMeasurementNode( &measurementNode );
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
   if ( sender == GUI->MeasurementsTable_ToggleApproved_ToolButton )
   {
      for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         MeasurementNode* node = static_cast<MeasurementNode*>( GUI->MeasurementTable_TreeBox[i] );
         if ( node->IsSelected() )
         {
            MeasureItem* item = node->Item();
            item->enabled = !item->enabled;
            item->locked = true;
            UpdateMeasurementNode( node );
         }
      }
      UpdateMeasurementQuantity();
      UpdateMeasurementGraph();
   }
   else if ( sender == GUI->MeasurementsTable_ToggleLocked_ToolButton )
   {
      for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         MeasurementNode* node = static_cast<MeasurementNode*>( GUI->MeasurementTable_TreeBox[i] );
         if ( node->IsSelected() )
         {
            MeasureItem* item = node->Item();
            item->locked = !item->locked;
            UpdateMeasurementNode( node );
         }
      }
      UpdateMeasurementQuantity();
      UpdateMeasurementGraph();
   }
   else if ( sender == GUI->MeasurementsTable_Invert_ToolButton )
   {
      for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->MeasurementTable_TreeBox[i]->Select( !GUI->MeasurementTable_TreeBox[i]->IsSelected() );
   }
   else if ( sender == GUI->MeasurementsTable_Remove_ToolButton )
   {
      MeasureItemList newMeasures;
      int selectedNodeIndex = -1;
      for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         MeasurementNode* node = static_cast<MeasurementNode*>( GUI->MeasurementTable_TreeBox[i] );
         if ( !node->IsSelected() )
            newMeasures << *node->Item();
         else if ( selectedNodeIndex < 0 )
            selectedNodeIndex = i;
      }

      Cleanup();
      m_instance.o_measures = newMeasures;
      if ( !m_instance.o_measures.IsEmpty() )
      {
         UpdateControls();
         if ( selectedNodeIndex >= 0 )
         {
            TreeBox::Node* currentNode = GUI->MeasurementTable_TreeBox[
               Min( selectedNodeIndex, GUI->MeasurementTable_TreeBox.NumberOfChildren()-1 )];
            currentNode->Select();
            GUI->MeasurementTable_TreeBox.SetNodeIntoView( currentNode );
         }
      }
   }
   else if ( sender == GUI->MeasurementsTable_Reset_ToolButton )
   {
      m_instance.o_measures.Apply( []( MeasureItem& item ){ item.enabled = true; item.locked = false; } );
      UpdateControls();
   }
   else if ( sender == GUI->MeasurementsTable_ExportCSV_ToolButton )
   {
      ExportCSV();
   }
   else if ( sender == GUI->MeasurementGraph_ExportPDF_ToolButton )
   {
      ExportPDF();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_GraphApprove( GraphWebView &sender, int index )
{
   MeasureItem* item = MeasureItemByGraphIndex( index );
   if ( item != nullptr )
   {
      item->enabled = !item->enabled;
      item->locked = true;

      MeasurementNode* node = MeasurementNodeByItem( item );
      if ( node != nullptr )
      {
         GUI->MeasurementTable_TreeBox.SetCurrentNode( node );
         GUI->MeasurementTable_TreeBox.SetNodeIntoView( node );
         UpdateMeasurementNode( node );
         UpdateMeasurementQuantity();
         UpdateMeasurementGraph();
      }
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_GraphUnlock( GraphWebView &sender, int index )
{
   MeasureItem* item = MeasureItemByGraphIndex( index );
   if ( item != nullptr )
   {
      item->locked = false;

      MeasurementNode* node = MeasurementNodeByItem( item );
      if ( node != nullptr )
      {
         GUI->MeasurementTable_TreeBox.SetCurrentNode( node );
         GUI->MeasurementTable_TreeBox.SetNodeIntoView( node );
         TheSubframeSelectorExpressionsInterface->ApplyApprovalExpression();
         UpdateMeasurementNode( node );
         UpdateMeasurementQuantity();
         UpdateMeasurementGraph();
      }
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->MeasurementsTable_SortingProperty_ComboBox )
   {
      m_instance.p_sortingProperty = s_comboBoxItemIndexToSortingProperty[itemIndex];
      UpdateMeasurementTable();
   }
   else if ( sender == GUI->MeasurementsTable_SortingMode_ComboBox )
   {
      UpdateMeasurementTable();
   }
   else if ( sender == GUI->MeasurementGraph_Property_ComboBox )
   {
      m_instance.p_graphProperty = s_comboBoxItemIndexToGraphProperty[itemIndex];
      UpdateMeasurementGraph();
   }
   else if ( sender == GUI->MeasurementGraph_AuxProperty_ComboBox )
   {
      m_instance.p_auxGraphProperty = s_comboBoxItemIndexToGraphProperty[itemIndex];
      UpdateMeasurementGraph();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

SubframeSelectorMeasurementsInterface::GUIData::GUIData( SubframeSelectorMeasurementsInterface& w )
{
   MeasurementTable_SectionBar.SetTitle( "Measurements Table" );
   MeasurementTable_SectionBar.SetSection( MeasurementTable_Control );
   MeasurementTable_SectionBar.OnToggleSection( (SectionBar::section_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ToggleSection, w );

   //

   for ( size_type i = 0; i < TheSSSortingPropertyParameter->NumberOfElements(); ++i )
      MeasurementsTable_SortingProperty_ComboBox.AddItem(
            TheSSSortingPropertyParameter->ElementLabel( s_comboBoxItemIndexToSortingProperty[i] ) );
   MeasurementsTable_SortingProperty_ComboBox.SetToolTip( "<p>Sorting property</p>" );
   MeasurementsTable_SortingProperty_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ItemSelected, w );

   MeasurementsTable_SortingMode_ComboBox.AddItem( "Ascending", Bitmap( w.ScaledResource( ":/icons/sort-down.png" ) ) );
   MeasurementsTable_SortingMode_ComboBox.AddItem( "Descending", Bitmap( w.ScaledResource( ":/icons/sort-up.png" ) ) );

   MeasurementsTable_SortingMode_ComboBox.SetToolTip( "<p>Sorting direction</p>" );
   MeasurementsTable_SortingMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ItemSelected, w );

   MeasurementsTable_TopLeft1_Sizer.SetSpacing( 4 );
   MeasurementsTable_TopLeft1_Sizer.Add( MeasurementsTable_SortingProperty_ComboBox );
   MeasurementsTable_TopLeft1_Sizer.Add( MeasurementsTable_SortingMode_ComboBox );
   MeasurementsTable_TopLeft1_Sizer.AddStretch();

   //

   MeasurementsTable_Quantities_Label.SetMinWidth( w.Font().Width( "999999/999999 Approved (100.00%), 999999 Locked (100.00%)" ) );

   MeasurementsTable_TopLeft2_Sizer.Add( MeasurementsTable_Quantities_Label );

   //

   MeasurementsTable_TopLeft_Sizer.SetSpacing( 4 );
   MeasurementsTable_TopLeft_Sizer.Add( MeasurementsTable_TopLeft1_Sizer );
   MeasurementsTable_TopLeft_Sizer.Add( MeasurementsTable_TopLeft2_Sizer );

   //

   MeasurementsTable_ToggleApproved_ToolButton.SetIcon( w.ScaledResource( ":/icons/check.png" ) );
   MeasurementsTable_ToggleApproved_ToolButton.SetScaledFixedSize( 20, 20 );
   MeasurementsTable_ToggleApproved_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   MeasurementsTable_ToggleApproved_ToolButton.SetToolTip( "<p>Toggle the approved state of currently selected measurements.</p>" );
   MeasurementsTable_ToggleApproved_ToolButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_ToggleLocked_ToolButton.SetIcon( w.ScaledResource( ":/icons/lock.png" ) );
   MeasurementsTable_ToggleLocked_ToolButton.SetScaledFixedSize( 20, 20 );
   MeasurementsTable_ToggleLocked_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   MeasurementsTable_ToggleLocked_ToolButton.SetToolTip( "<p>Toggle the locked state of currently selected measurements.</p>" );
   MeasurementsTable_ToggleLocked_ToolButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_Invert_ToolButton.SetIcon( w.ScaledResource( ":/icons/select-invert.png" ) );
   MeasurementsTable_Invert_ToolButton.SetScaledFixedSize( 20, 20 );
   MeasurementsTable_Invert_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   MeasurementsTable_Invert_ToolButton.SetToolTip( "<p>Invert the current selection of measurements.</p>" );
   MeasurementsTable_Invert_ToolButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_TopRight1_Sizer.SetSpacing( 12 );
   MeasurementsTable_TopRight1_Sizer.Add( MeasurementsTable_ToggleApproved_ToolButton );
   MeasurementsTable_TopRight1_Sizer.Add( MeasurementsTable_ToggleLocked_ToolButton );
   MeasurementsTable_TopRight1_Sizer.Add( MeasurementsTable_Invert_ToolButton );
   MeasurementsTable_TopRight1_Sizer.AddStretch();

   //

   MeasurementsTable_Remove_ToolButton.SetIcon( w.ScaledResource( ":/icons/remove.png" ) );
   MeasurementsTable_Remove_ToolButton.SetScaledFixedSize( 20, 20 );
   MeasurementsTable_Remove_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   MeasurementsTable_Remove_ToolButton.SetToolTip( "<p>Remove the selected measurements.</p>" );
   MeasurementsTable_Remove_ToolButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_Reset_ToolButton.SetIcon( w.ScaledResource( ":/icons/reload.png" ) );
   MeasurementsTable_Reset_ToolButton.SetScaledFixedSize( 20, 20 );
   MeasurementsTable_Reset_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   MeasurementsTable_Reset_ToolButton.SetToolTip( "<p>Reset all manually defined approved and locked states.</p>" );
   MeasurementsTable_Reset_ToolButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_ExportCSV_ToolButton.SetIcon( w.ScaledResource( ":/icons/document-csv.png" ) );
   MeasurementsTable_ExportCSV_ToolButton.SetScaledFixedSize( 20, 20 );
   MeasurementsTable_ExportCSV_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   MeasurementsTable_ExportCSV_ToolButton.SetToolTip( "<p>Export the table as a CSV file.</p>" );
   MeasurementsTable_ExportCSV_ToolButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_TopRight2_Sizer.SetSpacing( 12 );
   MeasurementsTable_TopRight2_Sizer.Add( MeasurementsTable_Remove_ToolButton );
   MeasurementsTable_TopRight2_Sizer.Add( MeasurementsTable_Reset_ToolButton );
   MeasurementsTable_TopRight2_Sizer.Add( MeasurementsTable_ExportCSV_ToolButton );
   MeasurementsTable_TopRight2_Sizer.AddStretch();

   //

   MeasurementsTable_TopRight_Sizer.SetSpacing( 4 );
   MeasurementsTable_TopRight_Sizer.Add( MeasurementsTable_TopRight1_Sizer );
   MeasurementsTable_TopRight_Sizer.Add( MeasurementsTable_TopRight2_Sizer );

   //

   MeasurementsTable_Top_Sizer.SetSpacing( 16 );
   MeasurementsTable_Top_Sizer.Add( MeasurementsTable_TopLeft_Sizer );
   MeasurementsTable_Top_Sizer.Add( MeasurementsTable_TopRight_Sizer );
   MeasurementsTable_Top_Sizer.AddStretch();

   //

   int numberOfTableColumns = ItemsInArray( s_comboBoxItemIndexToSortingProperty ) - 1 + 4; // -1: skip first 'Index' item

   MeasurementTable_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( w.Font() ) );
   MeasurementTable_TreeBox.SetScaledMinWidth( 400 );
   MeasurementTable_TreeBox.SetNumberOfColumns( numberOfTableColumns + 1 ); // plus one 'spacer' column
   MeasurementTable_TreeBox.SetHeaderText( 0, "Index" );
   MeasurementTable_TreeBox.SetScaledColumnWidth( 0, 40 );
   MeasurementTable_TreeBox.SetHeaderIcon( 1, Bitmap( w.ScaledResource( ":/icons/picture-ok.png" ) ) );
   MeasurementTable_TreeBox.SetHeaderText( 1, "" );
   MeasurementTable_TreeBox.SetScaledColumnWidth( 1, 30 );
   MeasurementTable_TreeBox.SetHeaderIcon( 2, Bitmap( w.ScaledResource( ":/icons/function-import.png" ) ) );
   MeasurementTable_TreeBox.SetHeaderText( 2, "" );
   MeasurementTable_TreeBox.SetScaledColumnWidth( 2, 30 );
   MeasurementTable_TreeBox.SetHeaderText( 3, "Filename" );
   for ( int i = 0; i < numberOfTableColumns; ++i )
   {
      MeasurementTable_TreeBox.SetHeaderAlignment( i, TextAlign::Center|TextAlign::VertCenter );
      if ( i >= 4 )
         MeasurementTable_TreeBox.SetHeaderText( i,
                  TheSSSortingPropertyParameter->ElementLabel( s_comboBoxItemIndexToSortingProperty[i-3] ) );
   }

   MeasurementTable_TreeBox.SetHeaderText( numberOfTableColumns, "" ); // blank 'spacer' column
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
   MeasurementTable_Sizer.Add( MeasurementsTable_Top_Sizer );
   MeasurementTable_Sizer.Add( MeasurementTable_TreeBox, 100 );

   MeasurementTable_Control.SetSizer( MeasurementTable_Sizer );

   //

   MeasurementGraph_SectionBar.SetTitle( "Measurements Graph" );
   MeasurementGraph_SectionBar.SetSection( MeasurementGraph_Control );
   MeasurementGraph_SectionBar.OnToggleSection( (SectionBar::section_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ToggleSection, w );


   for ( size_type i = 0; i < SSGraphProperty::NumberOfItems; ++i )
   {
      MeasurementGraph_Property_ComboBox.AddItem(
            TheSSGraphPropertyParameter->ElementLabel( s_comboBoxItemIndexToGraphProperty[i] ) );
      MeasurementGraph_AuxProperty_ComboBox.AddItem(
            TheSSAuxGraphPropertyParameter->ElementLabel( s_comboBoxItemIndexToGraphProperty[i] ) );
   }

   MeasurementGraph_Property_ComboBox.SetToolTip( "<p>Primary graph property</p>" );
   MeasurementGraph_Property_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ItemSelected, w );

   MeasurementGraph_AuxProperty_ComboBox.SetToolTip( "<p>Auxiliary graph property</p>" );
   MeasurementGraph_AuxProperty_ComboBox.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ItemSelected, w );

   MeasurementGraph_ExportPDF_ToolButton.SetIcon( w.ScaledResource( ":/script-editor/export-pdf.png" ) );
   MeasurementGraph_ExportPDF_ToolButton.SetScaledFixedSize( 20, 20 );
   MeasurementGraph_ExportPDF_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   MeasurementGraph_ExportPDF_ToolButton.SetToolTip( "<p>Export the graph as a PDF document.</p>" );
   MeasurementGraph_ExportPDF_ToolButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementGraph_Top_Sizer.SetSpacing( 4 );
   MeasurementGraph_Top_Sizer.Add( MeasurementGraph_Property_ComboBox );
   MeasurementGraph_Top_Sizer.Add( MeasurementGraph_AuxProperty_ComboBox );
   MeasurementGraph_Top_Sizer.AddSpacing( 16 );
   MeasurementGraph_Top_Sizer.Add( MeasurementGraph_ExportPDF_ToolButton );
   MeasurementGraph_Top_Sizer.AddStretch();

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
// EOF SubframeSelectorMeasurementsInterface.cpp - Released 2023-08-28T15:23:41Z
