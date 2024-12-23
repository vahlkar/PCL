//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 2.2.8
// ----------------------------------------------------------------------------
// SpectrophotometricFluxCalibrationInterface.cpp - Released 2024-12-23T11:33:28Z
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

#include "SpectrophotometricFluxCalibrationGraphInterface.h"
#include "SpectrophotometricFluxCalibrationInterface.h"
#include "SpectrophotometricFluxCalibrationParameters.h"
#include "SpectrophotometricFluxCalibrationProcess.h"

#include <pcl/FileDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SpectrophotometricFluxCalibrationInterface* TheSpectrophotometricFluxCalibrationInterface = nullptr;

// ----------------------------------------------------------------------------

SpectrophotometricFluxCalibrationInterface::SpectrophotometricFluxCalibrationInterface()
   : m_instance( TheSpectrophotometricFluxCalibrationProcess )
{
   TheSpectrophotometricFluxCalibrationInterface = this;
}

// ----------------------------------------------------------------------------

SpectrophotometricFluxCalibrationInterface::~SpectrophotometricFluxCalibrationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString SpectrophotometricFluxCalibrationInterface::Id() const
{
   return "SpectrophotometricFluxCalibration";
}

// ----------------------------------------------------------------------------

MetaProcess* SpectrophotometricFluxCalibrationInterface::Process() const
{
   return TheSpectrophotometricFluxCalibrationProcess;
}

// ----------------------------------------------------------------------------

String SpectrophotometricFluxCalibrationInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/SpectrophotometricFluxCalibration.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures SpectrophotometricFluxCalibrationInterface::Features() const
{
   return InterfaceFeature::Default;
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInterface::ResetInstance()
{
   SpectrophotometricFluxCalibrationInstance defaultInstance( TheSpectrophotometricFluxCalibrationProcess );
   defaultInstance.SetDefaultSpectrumData();
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool SpectrophotometricFluxCalibrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( !TheSpectraDatabase.IsValid() )
      TheSpectraDatabase.Initialize();

   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "SpectrophotometricFluxCalibration" );
      m_instance.SetDefaultSpectrumData();
      RegenerateDatabaseAccessControls();
      UpdateControls();
   }

   dynamic = false;
   return &P == TheSpectrophotometricFluxCalibrationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* SpectrophotometricFluxCalibrationInterface::NewProcess() const
{
   return new SpectrophotometricFluxCalibrationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool SpectrophotometricFluxCalibrationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const SpectrophotometricFluxCalibrationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a SpectrophotometricFluxCalibration instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricFluxCalibrationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricFluxCalibrationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricFluxCalibrationInterface::WantsGlobalNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInterface::GlobalFiltersUpdated()
{
   if ( GUI != nullptr )
   {
      TheSpectraDatabase.Initialize();
      m_instance.SetDefaultSpectrumData();
      RegenerateDatabaseAccessControls();
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define OUTPUT_DIR_DEFAULT       String( "<system-temp-dir>" )
#define OUTPUT_DIR( x )          (x.IsEmpty() ? OUTPUT_DIR_DEFAULT : x)
#define OUTPUT_DIR_INSTANCE      OUTPUT_DIR( m_instance.p_outputDirectory )

// ----------------------------------------------------------------------------

static int ComboBoxItemIndexFromCatalogId( const String& catalogId )
{
   if ( catalogId == "GaiaDR3SP" || catalogId == "GaiaDR3" )
      return 0;
   return -1;
}

static String CatalogIdFromComboBoxItemIndex( int itemIndex )
{
   if ( itemIndex == 0 )
      return "GaiaDR3SP";
   return String();
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInterface::RegenerateDatabaseAccessControls()
{
   GUI->DeviceQE_ComboBox.Clear();
   GUI->GrayFilter_ComboBox.Clear();
   GUI->RedFilter_ComboBox.Clear();
   GUI->GreenFilter_ComboBox.Clear();
   GUI->BlueFilter_ComboBox.Clear();

   if ( TheSpectraDatabase.IsValid() )
   {
      for ( const SampledSpectrumData& filter : TheSpectraDatabase.Filters() )
         GUI->GrayFilter_ComboBox.AddItem( filter.name );
      for ( const SampledSpectrumData& filter : TheSpectraDatabase.Filters() )
         if ( filter.channel == "R" || filter.channel.IsEmpty() )
            GUI->RedFilter_ComboBox.AddItem( filter.name );
      for ( const SampledSpectrumData& filter : TheSpectraDatabase.Filters() )
         if ( filter.channel == "G" || filter.channel.IsEmpty() )
            GUI->GreenFilter_ComboBox.AddItem( filter.name );
      for ( const SampledSpectrumData& filter : TheSpectraDatabase.Filters() )
         if ( filter.channel == "B" || filter.channel.IsEmpty() )
            GUI->BlueFilter_ComboBox.AddItem( filter.name );
      for ( const SampledSpectrumData& filter : TheSpectraDatabase.Filters() )
         if ( filter.channel == "Q" )
            GUI->DeviceQE_ComboBox.AddItem( filter.name );

      GUI->GrayFilter_Label.Enable();
      GUI->GrayFilter_ComboBox.Enable();
      GUI->RedFilter_Label.Enable();
      GUI->RedFilter_ComboBox.Enable();
      GUI->GreenFilter_Label.Enable();
      GUI->GreenFilter_ComboBox.Enable();
      GUI->BlueFilter_Label.Enable();
      GUI->BlueFilter_ComboBox.Enable();
      GUI->DeviceQE_ComboBox.Enable();
   }
   else
   {
      GUI->GrayFilter_Label.Disable();
      GUI->GrayFilter_ComboBox.Disable();
      GUI->RedFilter_Label.Disable();
      GUI->RedFilter_ComboBox.Disable();
      GUI->GreenFilter_Label.Disable();
      GUI->GreenFilter_ComboBox.Disable();
      GUI->BlueFilter_Label.Disable();
      GUI->BlueFilter_ComboBox.Disable();
      GUI->DeviceQE_ComboBox.Disable();
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInterface::UpdateControls()
{
   {
      int itemIndex = ComboBoxItemIndexFromCatalogId( m_instance.p_catalogId );
      if ( itemIndex < 0 )
      {
         itemIndex = GUI->Catalog_ComboBox.NumberOfItems();
         GUI->Catalog_ComboBox.AddItem( "<unknown catalog>" );
      }
      GUI->Catalog_ComboBox.SetCurrentItem( itemIndex );
   }

   if ( TheSpectraDatabase.IsValid() )
   {
      int itemIndex = GUI->GrayFilter_ComboBox.FindItem( m_instance.p_grayFilterName, 0/*fromIdx*/, true/*exactMatch*/, true/*caseSensitive*/ );
      if ( itemIndex < 0 )
      {
         itemIndex = GUI->GrayFilter_ComboBox.NumberOfItems();
         GUI->GrayFilter_ComboBox.AddItem( "<custom filter>" );
      }
      GUI->GrayFilter_ComboBox.SetCurrentItem( itemIndex );

      itemIndex = GUI->RedFilter_ComboBox.FindItem( m_instance.p_redFilterName, 0/*fromIdx*/, true/*exactMatch*/, true/*caseSensitive*/ );
      if ( itemIndex < 0 )
      {
         itemIndex = GUI->RedFilter_ComboBox.NumberOfItems();
         GUI->RedFilter_ComboBox.AddItem( "<custom filter>" );
      }
      GUI->RedFilter_ComboBox.SetCurrentItem( itemIndex );

      itemIndex = GUI->GreenFilter_ComboBox.FindItem( m_instance.p_greenFilterName, 0/*fromIdx*/, true/*exactMatch*/, true/*caseSensitive*/ );
      if ( itemIndex < 0 )
      {
         itemIndex = GUI->GreenFilter_ComboBox.NumberOfItems();
         GUI->GreenFilter_ComboBox.AddItem( "<custom filter>" );
      }
      GUI->GreenFilter_ComboBox.SetCurrentItem( itemIndex );

      itemIndex = GUI->BlueFilter_ComboBox.FindItem( m_instance.p_blueFilterName, 0/*fromIdx*/, true/*exactMatch*/, true/*caseSensitive*/ );
      if ( itemIndex < 0 )
      {
         itemIndex = GUI->BlueFilter_ComboBox.NumberOfItems();
         GUI->BlueFilter_ComboBox.AddItem( "<custom filter>" );
      }
      GUI->BlueFilter_ComboBox.SetCurrentItem( itemIndex );

      itemIndex = GUI->DeviceQE_ComboBox.FindItem( m_instance.p_deviceQECurveName, 0/*fromIdx*/, true/*exactMatch*/, true/*caseSensitive*/ );
      if ( itemIndex < 0 )
      {
         itemIndex = GUI->DeviceQE_ComboBox.NumberOfItems();
         GUI->DeviceQE_ComboBox.AddItem( "<custom curve>" );
      }
      GUI->DeviceQE_ComboBox.SetCurrentItem( itemIndex );
   }

   GUI->GrayFilterWavelength_NumericEdit.SetValue( m_instance.p_grayFilterWavelength );
   GUI->GrayFilterBandwidth_NumericEdit.SetValue( m_instance.p_grayFilterBandwidth );

   GUI->RedFilterWavelength_NumericEdit.SetValue( m_instance.p_redFilterWavelength );
   GUI->RedFilterBandwidth_NumericEdit.SetValue( m_instance.p_redFilterBandwidth );

   GUI->GreenFilterWavelength_NumericEdit.SetValue( m_instance.p_greenFilterWavelength );
   GUI->GreenFilterBandwidth_NumericEdit.SetValue( m_instance.p_greenFilterBandwidth );

   GUI->BlueFilterWavelength_NumericEdit.SetValue( m_instance.p_blueFilterWavelength );
   GUI->BlueFilterBandwidth_NumericEdit.SetValue( m_instance.p_blueFilterBandwidth );

   GUI->NarrowbandMode_CheckBox.SetChecked( m_instance.p_narrowbandMode );

   if ( m_instance.p_narrowbandMode )
   {
      if ( !GUI->NarrowbandFilters_Control.IsVisible() )
      {
         GUI->BroadbandFilters_Control.Hide();
         GUI->NarrowbandFilters_Control.Show();
         SetVariableHeight();
         EnsureLayoutUpdated();
         AdjustToContents();
         SetFixedSize();
      }
   }
   else
   {
      if ( !GUI->BroadbandFilters_Control.IsVisible() )
      {
         GUI->NarrowbandFilters_Control.Hide();
         GUI->BroadbandFilters_Control.Show();
         SetVariableHeight();
         EnsureLayoutUpdated();
         AdjustToContents();
         SetFixedSize();
      }
   }

   GUI->GenerateGraphs_CheckBox.SetChecked( m_instance.p_generateGraphs );
   GUI->GenerateStarMaps_CheckBox.SetChecked( m_instance.p_generateStarMaps );
   GUI->GenerateTextFiles_CheckBox.SetChecked( m_instance.p_generateTextFiles );

   GUI->OutputDirectory_Label.Enable( m_instance.p_generateTextFiles );
   GUI->OutputDirectory_Edit.SetText( OUTPUT_DIR_INSTANCE );
   GUI->OutputDirectory_Edit.Enable( m_instance.p_generateTextFiles );
   GUI->OutputDirectory_ToolButton.Enable( m_instance.p_generateTextFiles );

   GUI->AutoLimitMagnitude_CheckBox.SetChecked( m_instance.p_autoLimitMagnitude );
   GUI->LimitMagnitude_NumericEdit.SetValue( m_instance.p_limitMagnitude );
   GUI->LimitMagnitude_NumericEdit.Enable( !m_instance.p_autoLimitMagnitude );
   GUI->MinMagnitude_NumericEdit.SetValue( m_instance.p_minMagnitude );

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
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInterface::e_GetFocus( Control& sender )
{
   if ( sender == GUI->OutputDirectory_Edit )
   {
      Edit* e = dynamic_cast<Edit*>( &sender );
      if ( e != nullptr ) // ?!
         if ( e->Text().Trimmed() == OUTPUT_DIR_DEFAULT )
            e->Clear();
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInterface::e_EditCompleted( Edit& sender )
{
   String text = sender.Text().Trimmed();

   if ( sender == GUI->OutputDirectory_Edit )
   {
      if ( text == OUTPUT_DIR_DEFAULT )
         text.Clear();
      m_instance.p_outputDirectory = text;
      sender.SetText( OUTPUT_DIR_INSTANCE );
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->GrayFilter_ComboBox )
   {
      if ( itemIndex >= 0 )
      {
         int index = TheSpectraDatabase.FindFilterByName( sender.ItemText( itemIndex ).Trimmed() );
         if ( index >= 0 )
         {
            const SampledSpectrumData& s = TheSpectraDatabase.Filters()[index];
            m_instance.p_grayFilterTrCurve = s.data;
            m_instance.p_grayFilterName = s.name;
         }
      }
   }
   else if ( sender == GUI->RedFilter_ComboBox )
   {
      if ( itemIndex >= 0 )
      {
         int index = TheSpectraDatabase.FindFilterByName( sender.ItemText( itemIndex ).Trimmed() );
         if ( index >= 0 )
         {
            const SampledSpectrumData& s = TheSpectraDatabase.Filters()[index];
            m_instance.p_redFilterTrCurve = s.data;
            m_instance.p_redFilterName = s.name;
         }
      }
   }
   else if ( sender == GUI->GreenFilter_ComboBox )
   {
      if ( itemIndex >= 0 )
      {
         int index = TheSpectraDatabase.FindFilterByName( sender.ItemText( itemIndex ).Trimmed() );
         if ( index >= 0 )
         {
            const SampledSpectrumData& s = TheSpectraDatabase.Filters()[index];
            m_instance.p_greenFilterTrCurve = s.data;
            m_instance.p_greenFilterName = s.name;
         }
      }
   }
   else if ( sender == GUI->BlueFilter_ComboBox )
   {
      if ( itemIndex >= 0 )
      {
         int index = TheSpectraDatabase.FindFilterByName( sender.ItemText( itemIndex ).Trimmed() );
         if ( index >= 0 )
         {
            const SampledSpectrumData& s = TheSpectraDatabase.Filters()[index];
            m_instance.p_blueFilterTrCurve = s.data;
            m_instance.p_blueFilterName = s.name;
         }
      }
   }
   else if ( sender == GUI->DeviceQE_ComboBox )
   {
      if ( itemIndex >= 0 )
      {
         int index = TheSpectraDatabase.FindFilterByName( sender.ItemText( itemIndex ).Trimmed() );
         if ( index >= 0 )
         {
            const SampledSpectrumData& s = TheSpectraDatabase.Filters()[index];
            m_instance.p_deviceQECurve = s.data;
            m_instance.p_deviceQECurveName = s.name;
         }
      }
   }
   else if ( sender == GUI->Catalog_ComboBox )
   {
      m_instance.p_catalogId = CatalogIdFromComboBoxItemIndex( itemIndex );
   }
   else if ( sender == GUI->PSFType_ComboBox )
   {
      m_instance.p_psfType = itemIndex;
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->NarrowbandMode_CheckBox )
   {
      m_instance.p_narrowbandMode = checked;
      UpdateControls();
   }
   else if ( sender == GUI->GenerateGraphs_CheckBox )
   {
      m_instance.p_generateGraphs = checked;
      if ( checked )
         if ( !TheSpectrophotometricFluxCalibrationGraphInterface->IsVisible() )
            TheSpectrophotometricFluxCalibrationGraphInterface->Launch();
   }
   else if ( sender == GUI->GenerateStarMaps_CheckBox )
   {
      m_instance.p_generateStarMaps = checked;
   }
   else if ( sender == GUI->GenerateTextFiles_CheckBox )
   {
      m_instance.p_generateTextFiles = checked;
      UpdateControls();
   }
   else if ( sender == GUI->OutputDirectory_ToolButton )
   {
      GetDirectoryDialog d;
      d.SetCaption( "SPFC: Select Output Directory" );
      if ( d.Execute() )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = d.Directory() );
   }
   else if ( sender == GUI->AutoLimitMagnitude_CheckBox )
   {
      m_instance.p_autoLimitMagnitude = checked;
      UpdateControls();
   }
   else if ( sender == GUI->SaturationRelative_CheckBox )
   {
      m_instance.p_saturationRelative = checked;
   }
   else if ( sender == GUI->PSFAllowClusteredSources_CheckBox )
   {
      m_instance.p_psfAllowClusteredSources = checked;
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInterface::e_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->GrayFilterWavelength_NumericEdit )
      m_instance.p_grayFilterWavelength = value;
   else if ( sender == GUI->GrayFilterBandwidth_NumericEdit )
      m_instance.p_grayFilterBandwidth = value;
   else if ( sender == GUI->RedFilterWavelength_NumericEdit )
      m_instance.p_redFilterWavelength = value;
   else if ( sender == GUI->RedFilterBandwidth_NumericEdit )
      m_instance.p_redFilterBandwidth = value;
   else if ( sender == GUI->GreenFilterWavelength_NumericEdit )
      m_instance.p_greenFilterWavelength = value;
   else if ( sender == GUI->GreenFilterBandwidth_NumericEdit )
      m_instance.p_greenFilterBandwidth = value;
   else if ( sender == GUI->BlueFilterWavelength_NumericEdit )
      m_instance.p_blueFilterWavelength = value;
   else if ( sender == GUI->BlueFilterBandwidth_NumericEdit )
      m_instance.p_blueFilterBandwidth = value;
   else if ( sender == GUI->LimitMagnitude_NumericEdit )
      m_instance.p_limitMagnitude = value;
   else if ( sender == GUI->MinMagnitude_NumericEdit )
      m_instance.p_minMagnitude = value;
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

void SpectrophotometricFluxCalibrationInterface::e_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->StructureLayers_SpinBox )
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

void SpectrophotometricFluxCalibrationInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->OutputDirectory_Edit )
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = files[0] );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

SpectrophotometricFluxCalibrationInterface::GUIData::GUIData( SpectrophotometricFluxCalibrationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( "Minimum detection SNR:m" );
   int editWidth1 = fnt.Width( "999999999m" );
   int editWidth2 = fnt.Width( String( '0', 10 ) );
   int editWidth3 = fnt.Width( "Wavelengthm" );
   int comboWidth1 = fnt.Width( String( 'm', 24 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   SystemParameters_SectionBar.SetTitle( "System Parameters" );
   SystemParameters_SectionBar.SetSection( SystemParameters_Control );

   //

   const char* deviceQEToolTip = "<p>Quantum efficiency of the device used to acquire the target image.</p>";

   DeviceQE_Label.SetText( "QE curve:" );
   DeviceQE_Label.SetFixedWidth( labelWidth1 );
   DeviceQE_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DeviceQE_Label.SetToolTip( deviceQEToolTip );

   DeviceQE_ComboBox.SetMinWidth( comboWidth1 );
   DeviceQE_ComboBox.SetToolTip( deviceQEToolTip );
   DeviceQE_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ItemSelected, w );

   DeviceQE_Sizer.SetSpacing( 4 );
   DeviceQE_Sizer.Add( DeviceQE_Label );
   DeviceQE_Sizer.Add( DeviceQE_ComboBox, 100 );
   DeviceQE_Sizer.AddStretch();

   //

   const char* grayFilterToolTip = "<p>Gray filter used to acquire the target image (only applicable to grayscale images).</p>";

   GrayFilter_Label.SetText( "Gray filter:" );
   GrayFilter_Label.SetFixedWidth( labelWidth1 );
   GrayFilter_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GrayFilter_Label.SetToolTip( grayFilterToolTip );

   GrayFilter_ComboBox.SetMinWidth( comboWidth1 );
   GrayFilter_ComboBox.SetToolTip( grayFilterToolTip );
   GrayFilter_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ItemSelected, w );

   GrayFilter_Sizer.SetSpacing( 4 );
   GrayFilter_Sizer.Add( GrayFilter_Label );
   GrayFilter_Sizer.Add( GrayFilter_ComboBox, 100 );
   GrayFilter_Sizer.AddStretch();

   const char* redFilterToolTip = "<p>Red filter used to acquire the target image (only applicable to RGB color images).</p>";

   RedFilter_Label.SetText( "Red filter:" );
   RedFilter_Label.SetFixedWidth( labelWidth1 );
   RedFilter_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RedFilter_Label.SetToolTip( redFilterToolTip );

   RedFilter_ComboBox.SetMinWidth( comboWidth1 );
   RedFilter_ComboBox.SetToolTip( redFilterToolTip );
   RedFilter_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ItemSelected, w );

   RedFilter_Sizer.SetSpacing( 4 );
   RedFilter_Sizer.Add( RedFilter_Label );
   RedFilter_Sizer.Add( RedFilter_ComboBox, 100 );
   RedFilter_Sizer.AddStretch();

   const char* greenFilterToolTip = "<p>Green filter used to acquire the target image (only applicable to RGB color images).</p>";

   GreenFilter_Label.SetText( "Green filter:" );
   GreenFilter_Label.SetFixedWidth( labelWidth1 );
   GreenFilter_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GreenFilter_Label.SetToolTip( greenFilterToolTip );

   GreenFilter_ComboBox.SetMinWidth( comboWidth1 );
   GreenFilter_ComboBox.SetToolTip( greenFilterToolTip );
   GreenFilter_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ItemSelected, w );

   GreenFilter_Sizer.SetSpacing( 4 );
   GreenFilter_Sizer.Add( GreenFilter_Label );
   GreenFilter_Sizer.Add( GreenFilter_ComboBox, 100 );
   GreenFilter_Sizer.AddStretch();

   const char* blueFilterToolTip = "<p>Blue filter used to acquire the target image (only applicable to RGB color images).</p>";

   BlueFilter_Label.SetText( "Blue filter:" );
   BlueFilter_Label.SetFixedWidth( labelWidth1 );
   BlueFilter_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BlueFilter_Label.SetToolTip( blueFilterToolTip );

   BlueFilter_ComboBox.SetMinWidth( comboWidth1 );
   BlueFilter_ComboBox.SetToolTip( blueFilterToolTip );
   BlueFilter_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ItemSelected, w );

   BlueFilter_Sizer.SetSpacing( 4 );
   BlueFilter_Sizer.Add( BlueFilter_Label );
   BlueFilter_Sizer.Add( BlueFilter_ComboBox, 100 );
   BlueFilter_Sizer.AddStretch();

   BroadbandFilters_Sizer.SetSpacing( 4 );
   BroadbandFilters_Sizer.Add( GrayFilter_Sizer );
   BroadbandFilters_Sizer.Add( RedFilter_Sizer );
   BroadbandFilters_Sizer.Add( GreenFilter_Sizer );
   BroadbandFilters_Sizer.Add( BlueFilter_Sizer );

   BroadbandFilters_Control.SetSizer( BroadbandFilters_Sizer );

   //

   FilterWavelengthTitle_Label.SetText( "Wavelength" );
   FilterWavelengthTitle_Label.SetFixedWidth( editWidth3 );
   FilterWavelengthTitle_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   FilterWavelengthTitle_Label.SetStyleSheet( "*{ font-style: italic; }" );
   FilterWavelengthTitle_Label.SetToolTip( "<p>Filter central wavelength in nanometers.</p>" );

   FilterBandwidthTitle_Label.SetText( "Bandwidth" );
   FilterBandwidthTitle_Label.SetFixedWidth( editWidth3 );
   FilterBandwidthTitle_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   FilterBandwidthTitle_Label.SetStyleSheet( "*{ font-style: italic; }" );
   FilterBandwidthTitle_Label.SetToolTip( "<p>Filter bandwidth in nanometers.</p>" );

   FiltersTitle_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   FiltersTitle_Sizer.Add( FilterWavelengthTitle_Label );
   FiltersTitle_Sizer.AddSpacing( 8 );
   FiltersTitle_Sizer.Add( FilterBandwidthTitle_Label );
   FiltersTitle_Sizer.AddStretch();

   GrayFilterWavelength_NumericEdit.SetReal();
   GrayFilterWavelength_NumericEdit.SetPrecision( 2 );
   GrayFilterWavelength_NumericEdit.EnableFixedPrecision();
   GrayFilterWavelength_NumericEdit.SetRange( TheSPFCGrayFilterWavelengthParameter->MinimumValue(), TheSPFCGrayFilterWavelengthParameter->MaximumValue() );
   GrayFilterWavelength_NumericEdit.label.SetText( "Gray filter:" );
   GrayFilterWavelength_NumericEdit.label.SetToolTip( "<p>Gray filter parameters (applicable to narrowband grayscale images only).</p>" );
   GrayFilterWavelength_NumericEdit.label.SetFixedWidth( labelWidth1 );
   GrayFilterWavelength_NumericEdit.edit.SetFixedWidth( editWidth3 );
   GrayFilterWavelength_NumericEdit.SetToolTip( "<p>Gray filter, central wavelength (nm).</p>" );
   GrayFilterWavelength_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   GrayFilterBandwidth_NumericEdit.SetReal();
   GrayFilterBandwidth_NumericEdit.SetPrecision( 2 );
   GrayFilterBandwidth_NumericEdit.EnableFixedPrecision();
   GrayFilterBandwidth_NumericEdit.SetRange( TheSPFCGrayFilterBandwidthParameter->MinimumValue(), TheSPFCGrayFilterBandwidthParameter->MaximumValue() );
   GrayFilterBandwidth_NumericEdit.label.Hide();
   GrayFilterBandwidth_NumericEdit.edit.SetFixedWidth( editWidth3 );
   GrayFilterBandwidth_NumericEdit.SetToolTip( "<p>Gray filter, bandwidth (nm).</p>" );
   GrayFilterBandwidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   GrayFilterParameters_Sizer.Add( GrayFilterWavelength_NumericEdit );
   GrayFilterParameters_Sizer.AddSpacing( 8 );
   GrayFilterParameters_Sizer.Add( GrayFilterBandwidth_NumericEdit );
   GrayFilterParameters_Sizer.AddStretch();

   RedFilterWavelength_NumericEdit.SetReal();
   RedFilterWavelength_NumericEdit.SetPrecision( 2 );
   RedFilterWavelength_NumericEdit.EnableFixedPrecision();
   RedFilterWavelength_NumericEdit.SetRange( TheSPFCRedFilterWavelengthParameter->MinimumValue(), TheSPFCRedFilterWavelengthParameter->MaximumValue() );
   RedFilterWavelength_NumericEdit.label.SetText( "Red filter:" );
   RedFilterWavelength_NumericEdit.label.SetToolTip( "<p>Red filter parameters.</p>" );
   RedFilterWavelength_NumericEdit.label.SetFixedWidth( labelWidth1 );
   RedFilterWavelength_NumericEdit.edit.SetFixedWidth( editWidth3 );
   RedFilterWavelength_NumericEdit.SetToolTip( "<p>Red filter, central wavelength (nm).</p>" );
   RedFilterWavelength_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   RedFilterBandwidth_NumericEdit.SetReal();
   RedFilterBandwidth_NumericEdit.SetPrecision( 2 );
   RedFilterBandwidth_NumericEdit.EnableFixedPrecision();
   RedFilterBandwidth_NumericEdit.SetRange( TheSPFCRedFilterBandwidthParameter->MinimumValue(), TheSPFCRedFilterBandwidthParameter->MaximumValue() );
   RedFilterBandwidth_NumericEdit.label.Hide();
   RedFilterBandwidth_NumericEdit.edit.SetFixedWidth( editWidth3 );
   RedFilterBandwidth_NumericEdit.SetToolTip( "<p>Red filter, bandwidth (nm).</p>" );
   RedFilterBandwidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   RedFilterParameters_Sizer.Add( RedFilterWavelength_NumericEdit );
   RedFilterParameters_Sizer.AddSpacing( 8 );
   RedFilterParameters_Sizer.Add( RedFilterBandwidth_NumericEdit );
   RedFilterParameters_Sizer.AddStretch();

   GreenFilterWavelength_NumericEdit.SetReal();
   GreenFilterWavelength_NumericEdit.SetPrecision( 2 );
   GreenFilterWavelength_NumericEdit.EnableFixedPrecision();
   GreenFilterWavelength_NumericEdit.SetRange( TheSPFCGreenFilterWavelengthParameter->MinimumValue(), TheSPFCGreenFilterWavelengthParameter->MaximumValue() );
   GreenFilterWavelength_NumericEdit.label.SetText( "Green filter:" );
   GreenFilterWavelength_NumericEdit.label.SetToolTip( "<p>Green filter parameters.</p>" );
   GreenFilterWavelength_NumericEdit.label.SetFixedWidth( labelWidth1 );
   GreenFilterWavelength_NumericEdit.edit.SetFixedWidth( editWidth3 );
   GreenFilterWavelength_NumericEdit.SetToolTip( "<p>Green filter, central wavelength (nm).</p>" );
   GreenFilterWavelength_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   GreenFilterBandwidth_NumericEdit.SetReal();
   GreenFilterBandwidth_NumericEdit.SetPrecision( 2 );
   GreenFilterBandwidth_NumericEdit.EnableFixedPrecision();
   GreenFilterBandwidth_NumericEdit.SetRange( TheSPFCGreenFilterBandwidthParameter->MinimumValue(), TheSPFCGreenFilterBandwidthParameter->MaximumValue() );
   GreenFilterBandwidth_NumericEdit.label.Hide();
   GreenFilterBandwidth_NumericEdit.edit.SetFixedWidth( editWidth3 );
   GreenFilterBandwidth_NumericEdit.SetToolTip( "<p>Green filter, bandwidth (nm).</p>" );
   GreenFilterBandwidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   GreenFilterParameters_Sizer.Add( GreenFilterWavelength_NumericEdit );
   GreenFilterParameters_Sizer.AddSpacing( 8 );
   GreenFilterParameters_Sizer.Add( GreenFilterBandwidth_NumericEdit );
   GreenFilterParameters_Sizer.AddStretch();

   BlueFilterWavelength_NumericEdit.SetReal();
   BlueFilterWavelength_NumericEdit.SetPrecision( 2 );
   BlueFilterWavelength_NumericEdit.EnableFixedPrecision();
   BlueFilterWavelength_NumericEdit.SetRange( TheSPFCBlueFilterWavelengthParameter->MinimumValue(), TheSPFCBlueFilterWavelengthParameter->MaximumValue() );
   BlueFilterWavelength_NumericEdit.label.SetText( "Blue filter:" );
   BlueFilterWavelength_NumericEdit.label.SetToolTip( "<p>Blue filter parameters.</p>" );
   BlueFilterWavelength_NumericEdit.label.SetFixedWidth( labelWidth1 );
   BlueFilterWavelength_NumericEdit.edit.SetFixedWidth( editWidth3 );
   BlueFilterWavelength_NumericEdit.SetToolTip( "<p>Blue filter, central wavelength (nm).</p>" );
   BlueFilterWavelength_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   BlueFilterBandwidth_NumericEdit.SetReal();
   BlueFilterBandwidth_NumericEdit.SetPrecision( 2 );
   BlueFilterBandwidth_NumericEdit.EnableFixedPrecision();
   BlueFilterBandwidth_NumericEdit.SetRange( TheSPFCBlueFilterBandwidthParameter->MinimumValue(), TheSPFCBlueFilterBandwidthParameter->MaximumValue() );
   BlueFilterBandwidth_NumericEdit.label.Hide();
   BlueFilterBandwidth_NumericEdit.edit.SetFixedWidth( editWidth3 );
   BlueFilterBandwidth_NumericEdit.SetToolTip( "<p>Blue filter, bandwidth (nm).</p>" );
   BlueFilterBandwidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   BlueFilterParameters_Sizer.Add( BlueFilterWavelength_NumericEdit );
   BlueFilterParameters_Sizer.AddSpacing( 8 );
   BlueFilterParameters_Sizer.Add( BlueFilterBandwidth_NumericEdit );
   BlueFilterParameters_Sizer.AddStretch();

   NarrowbandFilters_Sizer.SetSpacing( 4 );
   NarrowbandFilters_Sizer.Add( FiltersTitle_Sizer );
   NarrowbandFilters_Sizer.Add( GrayFilterParameters_Sizer );
   NarrowbandFilters_Sizer.Add( RedFilterParameters_Sizer );
   NarrowbandFilters_Sizer.Add( GreenFilterParameters_Sizer );
   NarrowbandFilters_Sizer.Add( BlueFilterParameters_Sizer );

   NarrowbandFilters_Control.SetSizer( NarrowbandFilters_Sizer );

   //

   NarrowbandMode_CheckBox.SetText( "Narrowband filters mode" );
   NarrowbandMode_CheckBox.SetToolTip( "<p>Enable the narrowband filters working mode.</p>" );
   NarrowbandMode_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricFluxCalibrationInterface::e_Click, w );

   NarrowbandMode_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   NarrowbandMode_Sizer.Add( NarrowbandMode_CheckBox );
   NarrowbandMode_Sizer.AddStretch();

   GenerateGraphs_CheckBox.SetText( "Generate graphs" );
   GenerateGraphs_CheckBox.SetToolTip( "<p>Generate interactive graphs to analyze the function fits among catalog "
      "and image signal samples for measured stars.</p>" );
   GenerateGraphs_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricFluxCalibrationInterface::e_Click, w );

   GenerateGraphs_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateGraphs_Sizer.Add( GenerateGraphs_CheckBox );
   GenerateGraphs_Sizer.AddStretch();

   GenerateStarMaps_CheckBox.SetText( "Generate star maps" );
   GenerateStarMaps_CheckBox.SetToolTip( "<p>Generate monochrome 8-bit images representing the measured stars. These images "
      "can be used as masks on the target image.</p>" );
   GenerateStarMaps_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricFluxCalibrationInterface::e_Click, w );

   GenerateStarMaps_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateStarMaps_Sizer.Add( GenerateStarMaps_CheckBox );
   GenerateStarMaps_Sizer.AddStretch();

   GenerateTextFiles_CheckBox.SetText( "Generate text files" );
   GenerateTextFiles_CheckBox.SetToolTip( "<p>Generate text files in CSV format with positional, photometric and scale sampling data.</p>" );
   GenerateTextFiles_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricFluxCalibrationInterface::e_Click, w );

   GenerateTextFiles_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateTextFiles_Sizer.Add( GenerateTextFiles_CheckBox );
   GenerateTextFiles_Sizer.AddStretch();

   const char* outputDirectoryToolTip = "<p>The directory (or folder) where output text files (in CSV format) will be written.</p>"
      "<p>If this field is left blank (or with its default &lt;system-temp-dir&gt; value), output files will be written to the "
      "system temporary files directory.</p>";

   OutputDirectory_Label.SetText( "Output directory:" );
   OutputDirectory_Label.SetFixedWidth( labelWidth1 );
   OutputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputDirectory_Label.SetToolTip( outputDirectoryToolTip );

   OutputDirectory_Edit.SetToolTip( outputDirectoryToolTip );
   OutputDirectory_Edit.OnGetFocus( (Control::event_handler)&SpectrophotometricFluxCalibrationInterface::e_GetFocus, w );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&SpectrophotometricFluxCalibrationInterface::e_EditCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)&SpectrophotometricFluxCalibrationInterface::e_FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)&SpectrophotometricFluxCalibrationInterface::e_FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.SetToolTip( "<p>Select the output directory for CSV files.</p>" );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)&SpectrophotometricFluxCalibrationInterface::e_Click, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   //

   SystemParameters_Sizer.SetSpacing( 4 );
   SystemParameters_Sizer.Add( DeviceQE_Sizer );
   SystemParameters_Sizer.Add( BroadbandFilters_Control );
   SystemParameters_Sizer.Add( NarrowbandFilters_Control );
   SystemParameters_Sizer.Add( NarrowbandMode_Sizer );
   SystemParameters_Sizer.Add( GenerateGraphs_Sizer );
   SystemParameters_Sizer.Add( GenerateStarMaps_Sizer );
   SystemParameters_Sizer.Add( GenerateTextFiles_Sizer );
   SystemParameters_Sizer.Add( OutputDirectory_Sizer );

   SystemParameters_Control.SetSizer( SystemParameters_Sizer );

   //

   CatalogSearch_SectionBar.SetTitle( "Catalog Search" );
//    CatalogSearch_SectionBar.SetToolTip( "<p></p>" );
   CatalogSearch_SectionBar.SetSection( CatalogSearch_Control );

   const char* catalogToolTip = "<p>Spectrophotometric catalog. Currently only the Gaia DR3 (DR3/SP database variant) is "
      "supported as a local database with the Gaia XPSD server process.</p>";

   Catalog_Label.SetText( "Catalog:" );
   Catalog_Label.SetFixedWidth( labelWidth1 );
   Catalog_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Catalog_Label.SetToolTip( catalogToolTip );

   Catalog_ComboBox.SetToolTip( catalogToolTip );
   Catalog_ComboBox.AddItem( "Gaia DR3/SP" );
   Catalog_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ItemSelected, w );

   Catalog_Sizer.SetSpacing( 4 );
   Catalog_Sizer.Add( Catalog_Label );
   Catalog_Sizer.Add( Catalog_ComboBox );
   Catalog_Sizer.AddStretch();

   AutoLimitMagnitude_CheckBox.SetText( "Automatic limit magnitude" );
   AutoLimitMagnitude_CheckBox.SetToolTip( "<p>When this option is enabled, the process will select an optimal limit magnitude "
      "for catalog search operations based on the field of view and center coordinates of the target image. Enabling this "
      "option is recommended under normal working conditions.</p>" );
   AutoLimitMagnitude_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricFluxCalibrationInterface::e_Click, w );

   AutoLimitMagnitude_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AutoLimitMagnitude_Sizer.Add( AutoLimitMagnitude_CheckBox );
   AutoLimitMagnitude_Sizer.AddStretch();

   LimitMagnitude_NumericEdit.label.SetText( "Limit magnitude:" );
   LimitMagnitude_NumericEdit.label.SetFixedWidth( labelWidth1 );
   LimitMagnitude_NumericEdit.SetReal();
   LimitMagnitude_NumericEdit.SetRange( TheSPFCLimitMagnitudeParameter->MinimumValue(), TheSPFCLimitMagnitudeParameter->MaximumValue() );
   LimitMagnitude_NumericEdit.SetPrecision( TheSPFCLimitMagnitudeParameter->Precision() );
   LimitMagnitude_NumericEdit.edit.SetFixedWidth( editWidth1 );
   LimitMagnitude_NumericEdit.sizer.AddStretch();
   LimitMagnitude_NumericEdit.SetToolTip( "<p>This parameter limits the number of stars extracted from the spectrophotometric "
      "catalog to those with magnitudes (in the visual or G band) smaller than this value. The optimal value depends on the "
      "field of view of the target image. If this value is too low, the color calibration algorithm may not have enough stars "
      "to compute robust white balance factors. Too high of a limit magnitude will slow down the process without any benefit. "
      "The <i>Automatic limit magnitude</i> option is strongly recommended instead of specifying a literal limit magnitude "
      "value under normal working conditions.</p>" );
   LimitMagnitude_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   MinMagnitude_NumericEdit.label.SetText( "Minimum magnitude:" );
   MinMagnitude_NumericEdit.label.SetFixedWidth( labelWidth1 );
   MinMagnitude_NumericEdit.SetReal();
   MinMagnitude_NumericEdit.SetRange( TheSPFCMinMagnitudeParameter->MinimumValue(), TheSPFCMinMagnitudeParameter->MaximumValue() );
   MinMagnitude_NumericEdit.SetPrecision( TheSPFCMinMagnitudeParameter->Precision() );
   MinMagnitude_NumericEdit.edit.SetFixedWidth( editWidth1 );
   MinMagnitude_NumericEdit.sizer.AddStretch();
   MinMagnitude_NumericEdit.SetToolTip( "<p>Minimum magnitude (G band) of extracted catalog stars. This parameter can be useful "
      "in cases where sampling too bright stars can lead to accuracy problems caused by contamination of signal estimates in "
      "highly crowded fields. This parameter is also helpful in automatically achieving a good optimal limit magnitude when most "
      "stars are saturated below a relatively high magnitude value. These issues often happen with wide-field images.</p>" );
   MinMagnitude_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   CatalogSearch_Sizer.SetSpacing( 4 );
   CatalogSearch_Sizer.Add( Catalog_Sizer );
   CatalogSearch_Sizer.Add( AutoLimitMagnitude_Sizer );
   CatalogSearch_Sizer.Add( LimitMagnitude_NumericEdit );
   CatalogSearch_Sizer.Add( MinMagnitude_NumericEdit );

   CatalogSearch_Control.SetSizer( CatalogSearch_Sizer );

   //

   SignalEvaluation_SectionBar.SetTitle( "Signal Evaluation" );
   SignalEvaluation_SectionBar.SetToolTip( "<p>Stars are detected in the target image and a point spread function model if fitted to each "
      "detected source separately in each nominal RGB channel. The fitted PSF parameters are then used to guide evaluation of the total "
      "flux of each source, and the RGB fluxes of matched stars in the selected spectrophotometric catalog are used to compute a robust "
      "and precise scaling factor based on star spectra and filter transmission functions.</p>" );
   SignalEvaluation_SectionBar.SetSection( SignalEvaluation_Control );

   const char* scaleStructureLayersToolTip =
   "<p>Number of wavelet layers used for structure detection.</p>"
   "<p>With more wavelet layers, larger stars will be detected and used for signal evaluation.</p>";

   StructureLayers_Label.SetText( "Detection scales:" );
   StructureLayers_Label.SetFixedWidth( labelWidth1 );
   StructureLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StructureLayers_Label.SetToolTip( scaleStructureLayersToolTip );

   StructureLayers_SpinBox.SetRange( int( TheSPFCStructureLayersParameter->MinimumValue() ), int( TheSPFCStructureLayersParameter->MaximumValue() ) );
   StructureLayers_SpinBox.SetToolTip( scaleStructureLayersToolTip );
   StructureLayers_SpinBox.SetFixedWidth( editWidth1 );
   StructureLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_SpinValueUpdated, w );

   StructureLayers_Sizer.SetSpacing( 4 );
   StructureLayers_Sizer.Add( StructureLayers_Label );
   StructureLayers_Sizer.Add( StructureLayers_SpinBox );
   StructureLayers_Sizer.AddStretch();

   SaturationThreshold_NumericControl.label.SetText( "Saturation threshold:" );
   SaturationThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   SaturationThreshold_NumericControl.slider.SetRange( 0, 250 );
   SaturationThreshold_NumericControl.SetReal();
   SaturationThreshold_NumericControl.SetRange( TheSPFCSaturationThresholdParameter->MinimumValue(), TheSPFCSaturationThresholdParameter->MaximumValue() );
   SaturationThreshold_NumericControl.SetPrecision( TheSPFCSaturationThresholdParameter->Precision() );
   SaturationThreshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   SaturationThreshold_NumericControl.SetToolTip( "<p>Saturation threshold in the [0,1] range.</p>"
      "<p>Detected stars with one or more pixels with values above this threshold will be excluded for signal evaluation.</p>"
      "<p>This parameter is expressed in the [0,1] range. It can be applied either as an absolute pixel sample value in the "
      "normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image (see the <i>Relative "
      "saturation threshold</i> parameter).</p>"
      "<p>The default saturation threshold is 0.75. For signal evaluation this parameter plays an imortant role to prevent "
      "contamination of the statistical sample of flux ratios by sources with saturated or nonlinear data. Changing the default "
      "value should not be necessary under normal conditions.</p>" );
   SaturationThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   SaturationRelative_CheckBox.SetText( "Relative saturation threshold" );
   SaturationRelative_CheckBox.SetToolTip( "<p>The saturation threshold parameter can be applied either as an absolute pixel "
      "sample value in the normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image.</p>"
      "The relative saturation threshold option is enabled by default.</p>" );
   SaturationRelative_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricFluxCalibrationInterface::e_Click, w );

   SaturationRelative_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SaturationRelative_Sizer.Add( SaturationRelative_CheckBox );
   SaturationRelative_Sizer.AddStretch();

   RejectionLimit_NumericControl.label.SetText( "Rejection limit:" );
   RejectionLimit_NumericControl.label.SetFixedWidth( labelWidth1 );
   RejectionLimit_NumericControl.slider.SetRange( 0, 250 );
   RejectionLimit_NumericControl.SetReal();
   RejectionLimit_NumericControl.SetRange( TheSPFCRejectionLimitParameter->MinimumValue(), TheSPFCRejectionLimitParameter->MaximumValue() );
   RejectionLimit_NumericControl.SetPrecision( TheSPFCRejectionLimitParameter->Precision() );
   RejectionLimit_NumericControl.edit.SetFixedWidth( editWidth1 );
   RejectionLimit_NumericControl.SetToolTip( "<p>Limit for the modified Chauvenet rejection criterion.</p>"
      "<p>A modified Robust Chauvenet Rejection (RCR) routine is used internally by this implementation for rejection of "
      "outlier relative scale samples. The larger the value of this parameter, the more samples will be rejected by the RCR "
      "algorithm.</p>"
      "<p>The original Chauvenet rejection criterion is N*P(x > |z|) &lt; 0.5, where N is the number of measurements and "
      "P() represents the probability of x being more than z standard deviations from the mean. This parameter modifies the "
      "rejection criterion by replacing 0.5 with an arbitrary limit in the [0,1] range, in order to make the algorithm "
      "controllable. The default rejection limit is 0.3.</p>" );
   RejectionLimit_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   const char* psfNoiseLayersToolTip =
      "<p>Star detector: Number of wavelet layers used for noise reduction.</p>"
      "<p>Noise reduction prevents detection of bright noise structures as false stars, including hot pixels and "
      "cosmic rays. This parameter can also be used to control the sizes of the smallest detected stars (increase "
      "to exclude more stars), although the <i>minimum structure size</i> parameter can be more efficient for this purpose.</p>";

   PSFNoiseLayers_Label.SetText( "Noise scales:" );
   PSFNoiseLayers_Label.SetFixedWidth( labelWidth1 );
   PSFNoiseLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PSFNoiseLayers_Label.SetToolTip( psfNoiseLayersToolTip );

   PSFNoiseLayers_SpinBox.SetRange( int( TheSPFCPSFNoiseLayersParameter->MinimumValue() ), int( TheSPFCPSFNoiseLayersParameter->MaximumValue() ) );
   PSFNoiseLayers_SpinBox.SetToolTip( psfNoiseLayersToolTip );
   PSFNoiseLayers_SpinBox.SetFixedWidth( editWidth1 );
   PSFNoiseLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_SpinValueUpdated, w );

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

   PSFMinStructureSize_SpinBox.SetRange( int( TheSPFCPSFMinStructureSizeParameter->MinimumValue() ), int( TheSPFCPSFMinStructureSizeParameter->MaximumValue() ) );
   PSFMinStructureSize_SpinBox.SetMinimumValueText( "<Auto>" );
   PSFMinStructureSize_SpinBox.SetToolTip( psfMinStructureSizeToolTip );
   PSFMinStructureSize_SpinBox.SetFixedWidth( editWidth1 );
   PSFMinStructureSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_SpinValueUpdated, w );

   PSFMinStructureSize_Sizer.SetSpacing( 4 );
   PSFMinStructureSize_Sizer.Add( PSFMinStructureSize_Label );
   PSFMinStructureSize_Sizer.Add( PSFMinStructureSize_SpinBox );
   PSFMinStructureSize_Sizer.AddStretch();

   const char* psfHotPixelFilterRadiusToolTip =
      "<p>Star detector: Size of the hot pixel removal filter.</p>"
      "<p>This is the radius in pixels of a median filter applied by the star detector before the structure "
      "detection phase. A median filter is very efficient to remove <i>hot pixels</i>. Hot pixels can be "
      "identified as false stars, and if present in large amounts, can prevent a valid signal evaluation.</p>"
      "<p>To disable hot pixel removal, set this parameter to zero.</p>";

   PSFHotPixelFilterRadius_Label.SetText( "Hot pixel removal:" );
   PSFHotPixelFilterRadius_Label.SetFixedWidth( labelWidth1 );
   PSFHotPixelFilterRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PSFHotPixelFilterRadius_Label.SetToolTip( psfHotPixelFilterRadiusToolTip );

   PSFHotPixelFilterRadius_SpinBox.SetRange( int( TheSPFCPSFHotPixelFilterRadiusParameter->MinimumValue() ), int( TheSPFCPSFHotPixelFilterRadiusParameter->MaximumValue() ) );
   PSFHotPixelFilterRadius_SpinBox.SetToolTip( psfHotPixelFilterRadiusToolTip );
   PSFHotPixelFilterRadius_SpinBox.SetFixedWidth( editWidth1 );
   PSFHotPixelFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_SpinValueUpdated, w );

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

   PSFNoiseReductionFilterRadius_SpinBox.SetRange( int( TheSPFCPSFNoiseReductionFilterRadiusParameter->MinimumValue() ), int( TheSPFCPSFNoiseReductionFilterRadiusParameter->MaximumValue() ) );
   PSFNoiseReductionFilterRadius_SpinBox.SetToolTip( psfNoiseReductionFilterRadiusToolTip );
   PSFNoiseReductionFilterRadius_SpinBox.SetFixedWidth( editWidth1 );
   PSFNoiseReductionFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_SpinValueUpdated, w );

   PSFNoiseReductionFilterRadius_Sizer.SetSpacing( 4 );
   PSFNoiseReductionFilterRadius_Sizer.Add( PSFNoiseReductionFilterRadius_Label );
   PSFNoiseReductionFilterRadius_Sizer.Add( PSFNoiseReductionFilterRadius_SpinBox );
   PSFNoiseReductionFilterRadius_Sizer.AddStretch();

   PSFMinSNR_NumericEdit.label.SetText( "Minimum detection SNR:" );
   PSFMinSNR_NumericEdit.label.SetFixedWidth( labelWidth1 );
   PSFMinSNR_NumericEdit.SetReal();
   PSFMinSNR_NumericEdit.SetRange( TheSPFCPSFMinSNRParameter->MinimumValue(), TheSPFCPSFMinSNRParameter->MaximumValue() );
   PSFMinSNR_NumericEdit.SetPrecision( TheSPFCPSFMinSNRParameter->Precision() );
   PSFMinSNR_NumericEdit.edit.SetFixedWidth( editWidth1 );
   PSFMinSNR_NumericEdit.sizer.AddStretch();
   PSFMinSNR_NumericEdit.SetToolTip( "<p>Minimum signal-to-noise ratio of a detectable star.</p>"
      "<p>Given a source with estimated brightness <i>s</i>, local background <i>b</i> and local background dispersion "
      "<i>n</i>, SNR is evaluated as (<i>s</i> &ndash; <i>b</i>)/<i>n</i>. Stars with measured SNR below the value of this "
      "parameter won't be used for signal evaluation.</p>"
      "<p>This parameter allows limiting star detection to a subset of the brightest sources in the image adaptively. By "
      "requiring relatively high SNR levels in the evaluated sources, the accuracy and robustness of the signal evaluation "
      "process can be largely improved. The default value is 40, which is quite appropriate in most cases.</p>" );
   PSFMinSNR_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   const char* psfTypeToolTip = "<p>Point spread function type used for PSF fitting and photometry.</p>"
      "<p>In all cases elliptical functions are fitted to detected star structures, and PSF sampling regions are "
      "defined adaptively using a robust median stabilization algorithm.</p>"
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
   PSFType_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ItemSelected, w );

   PSFType_Sizer.SetSpacing( 4 );
   PSFType_Sizer.Add( PSFType_Label );
   PSFType_Sizer.Add( PSFType_ComboBox );
   PSFType_Sizer.AddStretch();

   PSFGrowth_NumericControl.label.SetText( "Growth factor:" );
   PSFGrowth_NumericControl.label.SetFixedWidth( labelWidth1 );
   PSFGrowth_NumericControl.slider.SetRange( 0, 250 );
   PSFGrowth_NumericControl.SetReal();
   PSFGrowth_NumericControl.SetRange( TheSPFCPSFGrowthParameter->MinimumValue(), TheSPFCPSFGrowthParameter->MaximumValue() );
   PSFGrowth_NumericControl.SetPrecision( TheSPFCPSFGrowthParameter->Precision() );
   PSFGrowth_NumericControl.edit.SetFixedWidth( editWidth1 );
   PSFGrowth_NumericControl.SetToolTip( "<p>Growing factor for expansion/contraction of the PSF flux measurement region for "
      "each source, in units of the Full Width at Tenth Maximum (FWTM).</p>"
      "<p>A value of 1.0 means that flux will be measured exclusively from pixels within the elliptical region defined at one "
      "tenth of the fitted PSF maximum. Increasing this parameter can inprove accuracy of PSF flux measurements for "
      "undersampled images, where PSF fitting uncertainty is relatively large. Decreasing it can be beneficial in some cases "
      "working with very noisy data to restrict flux evaluation to star cores. The default value is 1.75.</p>" );
   PSFGrowth_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_ValueUpdated, w );

   const char* psfMaxStarsToolTip =
      "<p>The maximum number of stars that can be measured to compute signal estimates.</p>"
      "<p>PSF photometry will be performed for no more than the specified number of stars. The subset of measured stars "
      "will always start at the beginning of the set of detected stars, sorted by brightness in descending order.</p>"
      "<p>The default value imposes a generous limit of 24K stars. Limiting the number of photometric samples can improve "
      "performance for calibration of wide-field frames, where the number of detected stars can be very large. However, "
      "reducing the set of measured sources too much will damage the accuracy of signal evaluation.</p>";

   PSFMaxStars_Label.SetText( "Maximum stars:" );
   PSFMaxStars_Label.SetFixedWidth( labelWidth1 );
   PSFMaxStars_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PSFMaxStars_Label.SetToolTip( psfMaxStarsToolTip );

   PSFMaxStars_SpinBox.SetRange( int( TheSPFCPSFMaxStarsParameter->MinimumValue() ), int( TheSPFCPSFMaxStarsParameter->MaximumValue() ) );
   PSFMaxStars_SpinBox.SetToolTip( psfMaxStarsToolTip );
   PSFMaxStars_SpinBox.SetFixedWidth( editWidth2 );
   PSFMaxStars_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricFluxCalibrationInterface::e_SpinValueUpdated, w );

   PSFMaxStars_Sizer.SetSpacing( 4 );
   PSFMaxStars_Sizer.Add( PSFMaxStars_Label );
   PSFMaxStars_Sizer.Add( PSFMaxStars_SpinBox );
   PSFMaxStars_Sizer.AddStretch();

   PSFAllowClusteredSources_CheckBox.SetText( "Allow clustered sources" );
   PSFAllowClusteredSources_CheckBox.SetToolTip( "<p>If this parameter is disabled, multiple sources too close to be "
      "separated as individual structures, such as double and multiple stars, won't be detected for signal evaluation.</p>"
      "<p>If this parameter is enabled, non-separable multiple sources will be freely detectable as single objects.</p>"
      "<p>This parameter is disabled by default. Under normal conditions, it should always be turned off for the flux "
      "calibration task since sources with several local maxima pose difficulties in determining accurate star positions "
      "and, more importantly, tend to degrade the quality of PSF photometry.</p>" );
   PSFAllowClusteredSources_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricFluxCalibrationInterface::e_Click, w );

   PSFAllowClusteredSources_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PSFAllowClusteredSources_Sizer.Add( PSFAllowClusteredSources_CheckBox );
   PSFAllowClusteredSources_Sizer.AddStretch();

   SignalEvaluation_Sizer.SetSpacing( 4 );
   SignalEvaluation_Sizer.Add( StructureLayers_Sizer );
   SignalEvaluation_Sizer.Add( SaturationThreshold_NumericControl );
   SignalEvaluation_Sizer.Add( SaturationRelative_Sizer );
   SignalEvaluation_Sizer.Add( RejectionLimit_NumericControl );
   SignalEvaluation_Sizer.Add( PSFNoiseLayers_Sizer );
   SignalEvaluation_Sizer.Add( PSFMinStructureSize_Sizer );
   SignalEvaluation_Sizer.Add( PSFHotPixelFilterRadius_Sizer );
   SignalEvaluation_Sizer.Add( PSFNoiseReductionFilterRadius_Sizer );
   SignalEvaluation_Sizer.Add( PSFMinSNR_NumericEdit );
   SignalEvaluation_Sizer.Add( PSFType_Sizer );
   SignalEvaluation_Sizer.Add( PSFGrowth_NumericControl );
   SignalEvaluation_Sizer.Add( PSFMaxStars_Sizer );
   SignalEvaluation_Sizer.Add( PSFAllowClusteredSources_Sizer );

   SignalEvaluation_Control.SetSizer( SignalEvaluation_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( SystemParameters_SectionBar );
   Global_Sizer.Add( SystemParameters_Control );
   Global_Sizer.Add( CatalogSearch_SectionBar );
   Global_Sizer.Add( CatalogSearch_Control );
   Global_Sizer.Add( SignalEvaluation_SectionBar );
   Global_Sizer.Add( SignalEvaluation_Control );

   //

   w.SetSizer( Global_Sizer );

   // The narrowband filters working mode is disabled by default.
   NarrowbandFilters_Control.Hide();
   SignalEvaluation_Control.Hide();

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SpectrophotometricFluxCalibrationInterface.cpp - Released 2024-12-23T11:33:28Z
