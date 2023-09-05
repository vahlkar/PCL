//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// SpectrophotometricColorCalibrationInterface.cpp - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "CurveExplorerDialog.h"
#include "FilterManagementDialog.h"
#include "SpectrophotometricColorCalibrationGraphInterface.h"
#include "SpectrophotometricColorCalibrationInterface.h"
#include "SpectrophotometricColorCalibrationParameters.h"
#include "SpectrophotometricColorCalibrationPreferencesDialog.h"
#include "SpectrophotometricColorCalibrationProcess.h"

#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/PreviewSelectionDialog.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SpectrophotometricColorCalibrationInterface* TheSpectrophotometricColorCalibrationInterface = nullptr;

// ----------------------------------------------------------------------------

SpectrophotometricColorCalibrationInterface::SpectrophotometricColorCalibrationInterface()
   : m_instance( TheSpectrophotometricColorCalibrationProcess )
{
   TheSpectrophotometricColorCalibrationInterface = this;
}

// ----------------------------------------------------------------------------

SpectrophotometricColorCalibrationInterface::~SpectrophotometricColorCalibrationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString SpectrophotometricColorCalibrationInterface::Id() const
{
   return "SpectrophotometricColorCalibration";
}

// ----------------------------------------------------------------------------

MetaProcess* SpectrophotometricColorCalibrationInterface::Process() const
{
   return TheSpectrophotometricColorCalibrationProcess;
}

// ----------------------------------------------------------------------------

String SpectrophotometricColorCalibrationInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/SpectrophotometricColorCalibration.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures SpectrophotometricColorCalibrationInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::PreferencesButton;
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::EditPreferences()
{
   SpectrophotometricColorCalibrationPreferencesDialog dialog;
   if ( dialog.Execute() )
   {
      SpectrophotometricColorCalibrationProcess::InitializeSpectrumData( String()/*whiteReferencesDatabaseFilePath*/,
                                                                         dialog.FiltersDatabaseFilePath() );
      m_instance.SetDefaultSpectrumData();
      RegenerateDatabaseAccessControls();
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::ResetInstance()
{
   SpectrophotometricColorCalibrationInstance defaultInstance( TheSpectrophotometricColorCalibrationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool SpectrophotometricColorCalibrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( !SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
      SpectrophotometricColorCalibrationProcess::InitializeSpectrumData();

   if ( GUI == nullptr )
   {

      GUI = new GUIData( *this );
      SetWindowTitle( "SpectrophotometricColorCalibration" );
      m_instance.SetDefaultSpectrumData();
      RegenerateDatabaseAccessControls();
      UpdateControls();
   }

   dynamic = false;
   return &P == TheSpectrophotometricColorCalibrationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* SpectrophotometricColorCalibrationInterface::NewProcess() const
{
   return new SpectrophotometricColorCalibrationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool SpectrophotometricColorCalibrationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const SpectrophotometricColorCalibrationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a SpectrophotometricColorCalibration instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricColorCalibrationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricColorCalibrationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define OUTPUT_DIR_DEFAULT       String( "<system-temp-dir>" )
#define OUTPUT_DIR( x )          (x.IsEmpty() ? OUTPUT_DIR_DEFAULT : x)
#define OUTPUT_DIR_INSTANCE      OUTPUT_DIR( m_instance.p_outputDirectory )

#define BKG_REFERENCE_DEFAULT    String( "<target-image>" )
#define BKG_REFERENCE( x )       (x.IsEmpty() ? BKG_REFERENCE_DEFAULT : x)
#define BKG_REFERENCE_INSTANCE   BKG_REFERENCE( m_instance.p_backgroundReferenceViewId )

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

void SpectrophotometricColorCalibrationInterface::RegenerateDatabaseAccessControls()
{
   GUI->WhiteReference_ComboBox.Clear();
   GUI->DeviceQE_ComboBox.Clear();
   GUI->RedFilter_ComboBox.Clear();
   GUI->GreenFilter_ComboBox.Clear();
   GUI->BlueFilter_ComboBox.Clear();

   if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
   {
      for ( const SampledSpectrumData& ref : SpectrophotometricColorCalibrationProcess::WhiteReferences() )
         GUI->WhiteReference_ComboBox.AddItem( ref.name );
      for ( const SampledSpectrumData& filter : SpectrophotometricColorCalibrationProcess::Filters() )
         if ( filter.channel == "R" || filter.channel.IsEmpty() )
            GUI->RedFilter_ComboBox.AddItem( filter.name );
      for ( const SampledSpectrumData& filter : SpectrophotometricColorCalibrationProcess::Filters() )
         if ( filter.channel == "G" || filter.channel.IsEmpty() )
            GUI->GreenFilter_ComboBox.AddItem( filter.name );
      for ( const SampledSpectrumData& filter : SpectrophotometricColorCalibrationProcess::Filters() )
         if ( filter.channel == "B" || filter.channel.IsEmpty() )
            GUI->BlueFilter_ComboBox.AddItem( filter.name );
      for ( const SampledSpectrumData& filter : SpectrophotometricColorCalibrationProcess::Filters() )
         if ( filter.channel == "Q" )
            GUI->DeviceQE_ComboBox.AddItem( filter.name );

      GUI->WhiteReference_Label.Enable();
      GUI->WhiteReference_ComboBox.Enable();
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
      GUI->WhiteReference_Label.Disable();
      GUI->WhiteReference_ComboBox.Disable();
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

void SpectrophotometricColorCalibrationInterface::UpdateControls()
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

   if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
   {
      int itemIndex = GUI->WhiteReference_ComboBox.FindItem( m_instance.p_whiteReferenceName, 0/*fromIdx*/, true/*exactMatch*/, true/*caseSensitive*/ );
      if ( itemIndex < 0 )
      {
         itemIndex = GUI->WhiteReference_ComboBox.NumberOfItems();
         GUI->WhiteReference_ComboBox.AddItem( "<custom white reference>" );
      }
      GUI->WhiteReference_ComboBox.SetCurrentItem( itemIndex );

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

   GUI->OptimizeForStars_CheckBox.SetChecked( m_instance.p_narrowbandOptimizeStars );

   GUI->GenerateGraphs_CheckBox.SetChecked( m_instance.p_generateGraphs );
   GUI->GenerateStarMaps_CheckBox.SetChecked( m_instance.p_generateStarMaps );
   GUI->GenerateTextFiles_CheckBox.SetChecked( m_instance.p_generateTextFiles );

   GUI->OutputDirectory_Label.Enable( m_instance.p_generateTextFiles );
   GUI->OutputDirectory_Edit.SetText( OUTPUT_DIR_INSTANCE );
   GUI->OutputDirectory_Edit.Enable( m_instance.p_generateTextFiles );
   GUI->OutputDirectory_ToolButton.Enable( m_instance.p_generateTextFiles );

   GUI->ApplyColorCalibration_CheckBox.SetChecked( m_instance.p_applyCalibration );

   GUI->AutoLimitMagnitude_CheckBox.SetChecked( m_instance.p_autoLimitMagnitude );
   GUI->LimitMagnitude_NumericEdit.SetValue( m_instance.p_limitMagnitude );
   GUI->LimitMagnitude_NumericEdit.Enable( !m_instance.p_autoLimitMagnitude );

   GUI->StructureLayers_SpinBox.SetValue( m_instance.p_structureLayers );
   GUI->SaturationThreshold_NumericControl.SetValue( m_instance.p_saturationThreshold );
   GUI->SaturationRelative_CheckBox.SetChecked( m_instance.p_saturationRelative );
   GUI->PSFNoiseLayers_SpinBox.SetValue( m_instance.p_psfNoiseLayers );
   GUI->PSFMinStructureSize_SpinBox.SetValue( m_instance.p_psfMinStructureSize );
   GUI->PSFHotPixelFilterRadius_SpinBox.SetValue( m_instance.p_psfHotPixelFilterRadius );
   GUI->PSFNoiseReductionFilterRadius_SpinBox.SetValue( m_instance.p_psfNoiseReductionFilterRadius );
   GUI->PSFMinSNR_NumericEdit.SetValue( m_instance.p_psfMinSNR );
   GUI->PSFType_ComboBox.SetCurrentItem( m_instance.p_psfType );
   GUI->PSFGrowth_NumericControl.SetValue( m_instance.p_psfGrowth );
   GUI->PSFMaxStars_SpinBox.SetValue( m_instance.p_psfMaxStars );
   GUI->PSFAllowClusteredSources_CheckBox.SetChecked( m_instance.p_psfAllowClusteredSources );

   GUI->BackgroundReference_SectionBar.SetChecked( m_instance.p_neutralizeBackground );
   GUI->BackgroundReference_Control.Enable( m_instance.p_applyCalibration );
   GUI->BackgroundReferenceViewId_Edit.SetText( BKG_REFERENCE_INSTANCE );
   GUI->BackgroundLow_NumericControl.SetValue( m_instance.p_backgroundLow );
   GUI->BackgroundHigh_NumericControl.SetValue( m_instance.p_backgroundHigh );
   GUI->BackgroundROI_GroupBox.SetChecked( m_instance.p_backgroundUseROI );
   GUI->BackgroundROIX0_SpinBox.SetValue( m_instance.p_backgroundROI.x0 );
   GUI->BackgroundROIY0_SpinBox.SetValue( m_instance.p_backgroundROI.y0 );
   GUI->BackgroundROIWidth_SpinBox.SetValue( m_instance.p_backgroundROI.Width() );
   GUI->BackgroundROIHeight_SpinBox.SetValue( m_instance.p_backgroundROI.Height() );
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::e_GetFocus( Control& sender )
{
   if ( sender == GUI->OutputDirectory_Edit )
   {
      Edit* e = dynamic_cast<Edit*>( &sender );
      if ( e != nullptr ) // ?!
         if ( e->Text().Trimmed() == OUTPUT_DIR_DEFAULT )
            e->Clear();
   }
   else if ( sender == GUI->BackgroundReferenceViewId_Edit )
   {
      Edit* e = dynamic_cast<Edit*>( &sender );
      if ( e != nullptr ) // ?!
         if ( e->Text().Trimmed() == BKG_REFERENCE_DEFAULT )
            e->Clear();
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::e_EditCompleted( Edit& sender )
{
   String text = sender.Text().Trimmed();

   if ( sender == GUI->OutputDirectory_Edit )
   {
      if ( text == OUTPUT_DIR_DEFAULT )
         text.Clear();
      m_instance.p_backgroundReferenceViewId = text;
      sender.SetText( OUTPUT_DIR_INSTANCE );
   }
   else if ( sender == GUI->BackgroundReferenceViewId_Edit )
   {
      try
      {
         if ( text == BKG_REFERENCE_DEFAULT )
            text.Clear();
         if ( !text.IsEmpty() )
            if ( !View::IsValidViewId( text ) )
               throw Error( "Invalid view identifier: " + text );
         m_instance.p_backgroundReferenceViewId = text;
         sender.SetText( BKG_REFERENCE_INSTANCE );
      }
      catch ( ... )
      {
         sender.SetText( BKG_REFERENCE_INSTANCE );
         try
         {
            throw;
         }
         ERROR_HANDLER
         sender.SelectAll();
         sender.Focus();
      }
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->WhiteReference_ComboBox )
   {
      if ( itemIndex >= 0 )
      {
         int index = SpectrophotometricColorCalibrationProcess::FindWhiteReferenceByName( sender.ItemText( itemIndex ).Trimmed() );
         if ( index >= 0 )
         {
            const SampledSpectrumData& s = SpectrophotometricColorCalibrationProcess::WhiteReferences()[index];
            m_instance.p_whiteReferenceSpectrum = s.data;
            m_instance.p_whiteReferenceName = s.name;
         }
      }
   }
   else if ( sender == GUI->RedFilter_ComboBox )
   {
      if ( itemIndex >= 0 )
      {
         int index = SpectrophotometricColorCalibrationProcess::FindFilterByName( sender.ItemText( itemIndex ).Trimmed() );
         if ( index >= 0 )
         {
            const SampledSpectrumData& s = SpectrophotometricColorCalibrationProcess::Filters()[index];
            m_instance.p_redFilterTrCurve = s.data;
            m_instance.p_redFilterName = s.name;
         }
      }
   }
   else if ( sender == GUI->GreenFilter_ComboBox )
   {
      if ( itemIndex >= 0 )
      {
         int index = SpectrophotometricColorCalibrationProcess::FindFilterByName( sender.ItemText( itemIndex ).Trimmed() );
         if ( index >= 0 )
         {
            const SampledSpectrumData& s = SpectrophotometricColorCalibrationProcess::Filters()[index];
            m_instance.p_greenFilterTrCurve = s.data;
            m_instance.p_greenFilterName = s.name;
         }
      }
   }
   else if ( sender == GUI->BlueFilter_ComboBox )
   {
      if ( itemIndex >= 0 )
      {
         int index = SpectrophotometricColorCalibrationProcess::FindFilterByName( sender.ItemText( itemIndex ).Trimmed() );
         if ( index >= 0 )
         {
            const SampledSpectrumData& s = SpectrophotometricColorCalibrationProcess::Filters()[index];
            m_instance.p_blueFilterTrCurve = s.data;
            m_instance.p_blueFilterName = s.name;
         }
      }
   }
   else if ( sender == GUI->DeviceQE_ComboBox )
   {
      if ( itemIndex >= 0 )
      {
         int index = SpectrophotometricColorCalibrationProcess::FindFilterByName( sender.ItemText( itemIndex ).Trimmed() );
         if ( index >= 0 )
         {
            const SampledSpectrumData& s = SpectrophotometricColorCalibrationProcess::Filters()[index];
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

static void SanitizeFileName( String& fileName )
{
   static const char* badChars = " #%&{}\\<>*?/$!'\":@+`|=";
   for ( String::char_type& c : fileName )
      if ( c < 255 )
         if ( c == ' ' )
            c = '-';
         else if ( strchr( badChars, char( c ) ) )
            c = '_';
}

void SpectrophotometricColorCalibrationInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->CurveExplorer_Button )
   {
      if ( m_curveExplorer == nullptr )
         m_curveExplorer = new CurveExplorerDialog;

      m_curveExplorer->Execute();
      if ( m_curveExplorer->MadeChanges() )
      {
         RegenerateDatabaseAccessControls();
         UpdateControls();
      }
   }
   else if ( sender == GUI->FilterManagement_Button )
   {
      FilterManagementDialog dialog;
      if ( dialog.Execute() )
      {
         try
         {
            Console console;
            console.Show();
            Module->ProcessEvents();

            switch ( dialog.Mode() )
            {
            case FilterManagementDialog::ImportXML:
               {
                  console.NoteLn( "<end><cbr><br>* SPCC: Importing filter definitions (XSPD format):" );
                  console.WriteLn( dialog.Path() );

                  SampledSpectrumDataList filters = SpectrophotometricColorCalibrationProcess::LoadFilters( dialog.Path() );
                  if ( filters.IsEmpty() )
                     throw Error( "No filter definitions available in database file: " + dialog.Path() );
                  SpectrophotometricColorCalibrationProcess::ImportFilters( filters, dialog.Merge() );

                  for ( const SampledSpectrumData& filter : filters )
                     console.WriteLn( filter.name );
                  console.WriteLn( String().Format( "<end><cbr><br>%u filter(s) imported, %u filters available.",
                                                    filters.Length(), SpectrophotometricColorCalibrationProcess::Filters().Length() ) );
                  m_instance.SetDefaultSpectrumData();
                  RegenerateDatabaseAccessControls();
                  UpdateControls();
               }
               break;
            case FilterManagementDialog::ExportXML:
               {
                  console.NoteLn( "<end><cbr><br>* SPCC: Exporting filter definitions (XSPD format):" );
                  console.WriteLn( dialog.Path() );

                  SpectrophotometricColorCalibrationProcess::ExportFiltersDatabase( dialog.Path(), SpectrophotometricColorCalibrationProcess::Filters() );

                  for ( const SampledSpectrumData& filter : SpectrophotometricColorCalibrationProcess::Filters() )
                     console.WriteLn( filter.name );
                  console.WriteLn( String().Format( "<end><cbr><br>%u filter(s) exported.",
                                                    SpectrophotometricColorCalibrationProcess::Filters().Length() ) );
               }
               break;
            case FilterManagementDialog::ImportCSV:
               {
                  StringList fileNames;
                  String dirPath = dialog.Path();
                  if ( !dirPath.EndsWith( '/' ) )
                     dirPath << '/';
                  FindFileInfo info;
                  for ( File::Find f( dirPath + "*.csv" ); f.NextItem( info ); )
                     if ( !info.IsDirectory() )
                        fileNames << info.name;
                  if ( fileNames.IsEmpty() )
                     throw Error( "No .csv files found in directory: " + dialog.Path() );

                  console.NoteLn( "<end><cbr><br>* SPCC: Importing filter definitions (CSV format):" );

                  SampledSpectrumDataList filters;
                  for ( const String& fileName : fileNames )
                  {
                     try
                     {
                        SampledSpectrumData filter = SpectrophotometricColorCalibrationProcess::DecodeFilterCSV(
                              File::ReadLines( dirPath + fileName, ReadTextOption::RemoveEmptyLines|ReadTextOption::TrimSpaces ) );
                        filters << filter;
                        console.WriteLn( dirPath + fileName + ": " + filter.name );
                     }
                     catch ( const Exception& x )
                     {
                        throw Error( x.Message() + ": " + dirPath + fileName );
                     }
                  }
                  SpectrophotometricColorCalibrationProcess::ImportFilters( filters, dialog.Merge() );

                  console.WriteLn( String().Format( "<end><cbr><br>%u filter(s) imported, %u filters available.",
                                                    filters.Length(), SpectrophotometricColorCalibrationProcess::Filters().Length() ) );
                  m_instance.SetDefaultSpectrumData();
                  RegenerateDatabaseAccessControls();
                  UpdateControls();
               }
               break;
            case FilterManagementDialog::ExportCSV:
               {
                  console.NoteLn( "<end><cbr><br>* SPCC: Exporting filter definitions (CSV format):" );

                  for ( const SampledSpectrumData& filter : SpectrophotometricColorCalibrationProcess::Filters() )
                  {
                     String filePath = dialog.Path();
                     if ( !filePath.EndsWith( '/' ) )
                        filePath << '/';
                     String fileName = filter.name.Trimmed();
                     SanitizeFileName( fileName );
                     filePath << fileName << ".csv";
                     File::WriteTextFile( filePath, IsoString().ToNewLineSeparated( SpectrophotometricColorCalibrationProcess::EncodeFilterCSV( filter ) ) );
                     console.WriteLn( filePath + ": " + filter.name );
                  }

                  console.WriteLn( String().Format( "<end><cbr><br>%u filter(s) exported.",
                                                    SpectrophotometricColorCalibrationProcess::Filters().Length() ) );
               }
               break;
            case FilterManagementDialog::MergeWithDefault:
               if ( MessageBox( "<p>About to merge the current filters with the factory-default set.</p>"
                                "<p><b>This cannot be undone. Proceed?</b></p>",
                                "SPCC",
                                StdIcon::Warning,
                                StdButton::No, StdButton::Yes ).Execute() == StdButton::Yes )
               {
                  SampledSpectrumDataList filters = SpectrophotometricColorCalibrationProcess::Filters();
                  SpectrophotometricColorCalibrationProcess::InitializeSpectrumData();
                  SpectrophotometricColorCalibrationProcess::ImportFilters( filters, true/*merge*/ );
                  console.NoteLn( String().Format( "<end><cbr><br>* SPCC: default filters merged, %u filters available.",
                                                   SpectrophotometricColorCalibrationProcess::Filters().Length() ) );
                  m_instance.SetDefaultSpectrumData();
                  RegenerateDatabaseAccessControls();
                  UpdateControls();
               }
               break;
            case FilterManagementDialog::Reset:
               if ( MessageBox( "<p>About to reset filters to the factory-default set.</p>"
                                "<p><b>This cannot be undone. Proceed?</b></p>",
                                "SPCC",
                                StdIcon::Warning,
                                StdButton::No, StdButton::Yes ).Execute() == StdButton::Yes )
               {
                  SpectrophotometricColorCalibrationProcess::InitializeSpectrumData();
                  console.NoteLn( String().Format( "<end><cbr><br>* SPCC: default filters loaded, %u filters available.",
                                                   SpectrophotometricColorCalibrationProcess::Filters().Length() ) );
                  m_instance.SetDefaultSpectrumData();
                  RegenerateDatabaseAccessControls();
                  UpdateControls();
               }
               break;
            }
         }
         ERROR_HANDLER

         Module->ProcessEvents();
      }
   }
   else if ( sender == GUI->NarrowbandMode_CheckBox )
   {
      m_instance.p_narrowbandMode = checked;
      UpdateControls();
   }
   else if ( sender == GUI->OptimizeForStars_CheckBox )
   {
      m_instance.p_narrowbandOptimizeStars = checked;
   }
   else if ( sender == GUI->GenerateGraphs_CheckBox )
   {
      m_instance.p_generateGraphs = checked;
      if ( checked )
         if ( !TheSpectrophotometricColorCalibrationGraphInterface->IsVisible() )
            TheSpectrophotometricColorCalibrationGraphInterface->Launch();
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
      d.SetCaption( "SPCC: Select Output Directory" );
      if ( d.Execute() )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = d.Directory() );
   }
   else if ( sender == GUI->ApplyColorCalibration_CheckBox )
   {
      m_instance.p_applyCalibration = checked;
      UpdateControls();
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
   else if ( sender == GUI->BackgroundReferenceViewId_ToolButton )
   {
      ViewSelectionDialog d( m_instance.p_backgroundReferenceViewId );
      if ( d.Execute() == StdDialogCode::Ok )
      {
         m_instance.p_backgroundReferenceViewId = d.Id();
         GUI->BackgroundReferenceViewId_Edit.SetText( BKG_REFERENCE_INSTANCE );
      }
   }
   else if ( sender == GUI->BackgroundROISelectPreview_Button )
   {
      PreviewSelectionDialog d;
      if ( d.Execute() == StdDialogCode::Ok )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               m_instance.p_backgroundROI = view.Window().PreviewRect( view.Id() );
               UpdateControls();
            }
         }
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::e_SectionCheck( SectionBar& sender, bool checked )
{
   if ( sender == GUI->BackgroundReference_SectionBar )
      m_instance.p_neutralizeBackground = checked;
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::e_GroupCheck( GroupBox& sender, bool checked )
{
   if ( sender == GUI->BackgroundROI_GroupBox )
      m_instance.p_backgroundUseROI = checked;
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::e_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->RedFilterWavelength_NumericEdit )
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
   else if ( sender == GUI->SaturationThreshold_NumericControl )
      m_instance.p_saturationThreshold = value;
   else if ( sender == GUI->PSFMinSNR_NumericEdit )
      m_instance.p_psfMinSNR = value;
   else if ( sender == GUI->PSFGrowth_NumericControl )
      m_instance.p_psfGrowth = value;
   else if ( sender == GUI->BackgroundLow_NumericControl )
   {
      m_instance.p_backgroundLow = value;
      if ( m_instance.p_backgroundHigh < m_instance.p_backgroundLow )
         GUI->BackgroundHigh_NumericControl.SetValue( m_instance.p_backgroundHigh = m_instance.p_backgroundLow );
   }
   else if ( sender == GUI->BackgroundHigh_NumericControl )
   {
      m_instance.p_backgroundHigh = value;
      if ( m_instance.p_backgroundHigh < m_instance.p_backgroundLow )
         GUI->BackgroundHigh_NumericControl.SetValue( m_instance.p_backgroundLow = m_instance.p_backgroundHigh );
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::e_SpinValueUpdated( SpinBox& sender, int value )
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
   else if ( sender == GUI->BackgroundROIX0_SpinBox )
      m_instance.p_backgroundROI.x0 = value;
   else if ( sender == GUI->BackgroundROIY0_SpinBox )
      m_instance.p_backgroundROI.y0 = value;
   else if ( sender == GUI->BackgroundROIWidth_SpinBox )
      m_instance.p_backgroundROI.x1 = m_instance.p_backgroundROI.x0 + value;
   else if ( sender == GUI->BackgroundROIHeight_SpinBox )
      m_instance.p_backgroundROI.y1 = m_instance.p_backgroundROI.y0 + value;
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->BackgroundReferenceViewId_Edit )
      wantsView = true;
   else if ( sender == GUI->BackgroundROI_GroupBox || sender == GUI->BackgroundROISelectPreview_Button )
      wantsView = view.IsPreview();
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->BackgroundReferenceViewId_Edit )
   {
      m_instance.p_backgroundReferenceViewId = view.FullId();
      GUI->BackgroundReferenceViewId_Edit.SetText( BKG_REFERENCE_INSTANCE );
   }
   else if ( sender == GUI->BackgroundROI_GroupBox || sender == GUI->BackgroundROISelectPreview_Button )
   {
      if ( view.IsPreview() )
      {
         m_instance.p_backgroundUseROI = true;
         m_instance.p_backgroundROI = view.Window().PreviewRect( view.Id() );
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->OutputDirectory_Edit )
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = files[0] );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

SpectrophotometricColorCalibrationInterface::GUIData::GUIData( SpectrophotometricColorCalibrationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( "Minimum detection SNR:m" );
   int labelWidth2 = fnt.Width( "Height:m" );
   int editWidth1 = fnt.Width( "9999999m" );
   int editWidth2 = fnt.Width( String( '0', 10 ) );
   int editWidth3 = fnt.Width( "Wavelengthm" );
   int comboWidth1 = fnt.Width( String( 'm', 24 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   Calibration_SectionBar.SetTitle( "Calibration" );
   Calibration_SectionBar.SetSection( Calibration_Control );

   const char* whiteReferenceToolTip = "<p>Reference object to calculate white balance functions.</p>"
      "<p>Hot (blue) references, such as B and A stars, tend to induce a red color cast. Cold (red) references, "
      "such as K and M stars, tend to cause a blue color cast.</p>"
      "<p>The default average spiral galaxy reference is, in our opinion, truly representative of the deep space, "
      "and hence the most plausible white reference for most applications.</p>";

   WhiteReference_Label.SetText( "White reference:" );
   WhiteReference_Label.SetFixedWidth( labelWidth1 );
   WhiteReference_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WhiteReference_Label.SetToolTip( whiteReferenceToolTip );

   WhiteReference_ComboBox.SetMinWidth( comboWidth1 );
   WhiteReference_ComboBox.SetToolTip( whiteReferenceToolTip );
   WhiteReference_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricColorCalibrationInterface::e_ItemSelected, w );

   WhiteReference_Sizer.SetSpacing( 4 );
   WhiteReference_Sizer.Add( WhiteReference_Label );
   WhiteReference_Sizer.Add( WhiteReference_ComboBox );
   WhiteReference_Sizer.AddStretch();

   //

   const char* deviceQEToolTip = "<p>Quantum efficiency of the device used to acquire the target image.</p>";

   DeviceQE_Label.SetText( "QE curve:" );
   DeviceQE_Label.SetFixedWidth( labelWidth1 );
   DeviceQE_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DeviceQE_Label.SetToolTip( deviceQEToolTip );

   DeviceQE_ComboBox.SetMinWidth( comboWidth1 );
   DeviceQE_ComboBox.SetToolTip( deviceQEToolTip );
   DeviceQE_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricColorCalibrationInterface::e_ItemSelected, w );

   DeviceQE_Sizer.SetSpacing( 4 );
   DeviceQE_Sizer.Add( DeviceQE_Label );
   DeviceQE_Sizer.Add( DeviceQE_ComboBox );
   DeviceQE_Sizer.AddStretch();

   //

   const char* redFilterToolTip = "<p>Red filter used to acquire the target image.</p>";

   RedFilter_Label.SetText( "Red filter:" );
   RedFilter_Label.SetFixedWidth( labelWidth1 );
   RedFilter_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RedFilter_Label.SetToolTip( redFilterToolTip );

   RedFilter_ComboBox.SetMinWidth( comboWidth1 );
   RedFilter_ComboBox.SetToolTip( redFilterToolTip );
   RedFilter_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricColorCalibrationInterface::e_ItemSelected, w );

   RedFilter_Sizer.SetSpacing( 4 );
   RedFilter_Sizer.Add( RedFilter_Label );
   RedFilter_Sizer.Add( RedFilter_ComboBox );
   RedFilter_Sizer.AddStretch();

   const char* greenFilterToolTip = "<p>Green filter used to acquire the target image.</p>";

   GreenFilter_Label.SetText( "Green filter:" );
   GreenFilter_Label.SetFixedWidth( labelWidth1 );
   GreenFilter_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GreenFilter_Label.SetToolTip( greenFilterToolTip );

   GreenFilter_ComboBox.SetMinWidth( comboWidth1 );
   GreenFilter_ComboBox.SetToolTip( greenFilterToolTip );
   GreenFilter_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricColorCalibrationInterface::e_ItemSelected, w );

   GreenFilter_Sizer.SetSpacing( 4 );
   GreenFilter_Sizer.Add( GreenFilter_Label );
   GreenFilter_Sizer.Add( GreenFilter_ComboBox );
   GreenFilter_Sizer.AddStretch();

   const char* blueFilterToolTip = "<p>Blue filter used to acquire the target image.</p>";

   BlueFilter_Label.SetText( "Blue filter:" );
   BlueFilter_Label.SetFixedWidth( labelWidth1 );
   BlueFilter_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BlueFilter_Label.SetToolTip( blueFilterToolTip );

   BlueFilter_ComboBox.SetMinWidth( comboWidth1 );
   BlueFilter_ComboBox.SetToolTip( blueFilterToolTip );
   BlueFilter_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricColorCalibrationInterface::e_ItemSelected, w );

   BlueFilter_Sizer.SetSpacing( 4 );
   BlueFilter_Sizer.Add( BlueFilter_Label );
   BlueFilter_Sizer.Add( BlueFilter_ComboBox );
   BlueFilter_Sizer.AddStretch();

   FilterSelection_Sizer.SetSpacing( 4 );
   FilterSelection_Sizer.Add( RedFilter_Sizer );
   FilterSelection_Sizer.Add( GreenFilter_Sizer );
   FilterSelection_Sizer.Add( BlueFilter_Sizer );

   CurveExplorer_Button.SetText( "Curve Explorer" );
   CurveExplorer_Button.SetToolTip( "<p>Explore the current filter and white reference databases with interactive graphs.</p>" );
   CurveExplorer_Button.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   FilterManagement_Button.SetText( "Filter Management" );
   FilterManagement_Button.SetToolTip( "<p>Manage filters by importing/exporting filter data in XML and CSV formats.</p>" );
   FilterManagement_Button.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   FilterButtons_Sizer.AddStretch();
   FilterButtons_Sizer.Add( CurveExplorer_Button );
   FilterButtons_Sizer.AddSpacing( 4 );
   FilterButtons_Sizer.Add( FilterManagement_Button );

   BroadbandFilters_Sizer.Add( FilterSelection_Sizer );
   BroadbandFilters_Sizer.AddSpacing( 12 );
   BroadbandFilters_Sizer.Add( FilterButtons_Sizer );

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

   RedFilterWavelength_NumericEdit.SetReal();
   RedFilterWavelength_NumericEdit.SetPrecision( 2 );
   RedFilterWavelength_NumericEdit.EnableFixedPrecision();
   RedFilterWavelength_NumericEdit.SetRange( TheSPCCRedFilterWavelengthParameter->MinimumValue(), TheSPCCRedFilterWavelengthParameter->MaximumValue() );
   RedFilterWavelength_NumericEdit.label.SetText( "Red filter:" );
   RedFilterWavelength_NumericEdit.label.SetToolTip( "<p>Red filter parameters.</p>" );
   RedFilterWavelength_NumericEdit.label.SetFixedWidth( labelWidth1 );
   RedFilterWavelength_NumericEdit.edit.SetFixedWidth( editWidth3 );
   RedFilterWavelength_NumericEdit.SetToolTip( "<p>Red filter, central wavelength (nm).</p>" );
   RedFilterWavelength_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

   RedFilterBandwidth_NumericEdit.SetReal();
   RedFilterBandwidth_NumericEdit.SetPrecision( 2 );
   RedFilterBandwidth_NumericEdit.EnableFixedPrecision();
   RedFilterBandwidth_NumericEdit.SetRange( TheSPCCRedFilterBandwidthParameter->MinimumValue(), TheSPCCRedFilterBandwidthParameter->MaximumValue() );
   RedFilterBandwidth_NumericEdit.label.Hide();
   RedFilterBandwidth_NumericEdit.edit.SetFixedWidth( editWidth3 );
   RedFilterBandwidth_NumericEdit.SetToolTip( "<p>Red filter, bandwidth (nm).</p>" );
   RedFilterBandwidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

   RedFilterParameters_Sizer.Add( RedFilterWavelength_NumericEdit );
   RedFilterParameters_Sizer.AddSpacing( 8 );
   RedFilterParameters_Sizer.Add( RedFilterBandwidth_NumericEdit );
   RedFilterParameters_Sizer.AddStretch();

   GreenFilterWavelength_NumericEdit.SetReal();
   GreenFilterWavelength_NumericEdit.SetPrecision( 2 );
   GreenFilterWavelength_NumericEdit.EnableFixedPrecision();
   GreenFilterWavelength_NumericEdit.SetRange( TheSPCCGreenFilterWavelengthParameter->MinimumValue(), TheSPCCGreenFilterWavelengthParameter->MaximumValue() );
   GreenFilterWavelength_NumericEdit.label.SetText( "Green filter:" );
   GreenFilterWavelength_NumericEdit.label.SetToolTip( "<p>Green filter parameters.</p>" );
   GreenFilterWavelength_NumericEdit.label.SetFixedWidth( labelWidth1 );
   GreenFilterWavelength_NumericEdit.edit.SetFixedWidth( editWidth3 );
   GreenFilterWavelength_NumericEdit.SetToolTip( "<p>Green filter, central wavelength (nm).</p>" );
   GreenFilterWavelength_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

   GreenFilterBandwidth_NumericEdit.SetReal();
   GreenFilterBandwidth_NumericEdit.SetPrecision( 2 );
   GreenFilterBandwidth_NumericEdit.EnableFixedPrecision();
   GreenFilterBandwidth_NumericEdit.SetRange( TheSPCCGreenFilterBandwidthParameter->MinimumValue(), TheSPCCGreenFilterBandwidthParameter->MaximumValue() );
   GreenFilterBandwidth_NumericEdit.label.Hide();
   GreenFilterBandwidth_NumericEdit.edit.SetFixedWidth( editWidth3 );
   GreenFilterBandwidth_NumericEdit.SetToolTip( "<p>Green filter, bandwidth (nm).</p>" );
   GreenFilterBandwidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

   GreenFilterParameters_Sizer.Add( GreenFilterWavelength_NumericEdit );
   GreenFilterParameters_Sizer.AddSpacing( 8 );
   GreenFilterParameters_Sizer.Add( GreenFilterBandwidth_NumericEdit );
   GreenFilterParameters_Sizer.AddStretch();

   BlueFilterWavelength_NumericEdit.SetReal();
   BlueFilterWavelength_NumericEdit.SetPrecision( 2 );
   BlueFilterWavelength_NumericEdit.EnableFixedPrecision();
   BlueFilterWavelength_NumericEdit.SetRange( TheSPCCBlueFilterWavelengthParameter->MinimumValue(), TheSPCCBlueFilterWavelengthParameter->MaximumValue() );
   BlueFilterWavelength_NumericEdit.label.SetText( "Blue filter:" );
   BlueFilterWavelength_NumericEdit.label.SetToolTip( "<p>Blue filter parameters.</p>" );
   BlueFilterWavelength_NumericEdit.label.SetFixedWidth( labelWidth1 );
   BlueFilterWavelength_NumericEdit.edit.SetFixedWidth( editWidth3 );
   BlueFilterWavelength_NumericEdit.SetToolTip( "<p>Blue filter, central wavelength (nm).</p>" );
   BlueFilterWavelength_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

   BlueFilterBandwidth_NumericEdit.SetReal();
   BlueFilterBandwidth_NumericEdit.SetPrecision( 2 );
   BlueFilterBandwidth_NumericEdit.EnableFixedPrecision();
   BlueFilterBandwidth_NumericEdit.SetRange( TheSPCCBlueFilterBandwidthParameter->MinimumValue(), TheSPCCBlueFilterBandwidthParameter->MaximumValue() );
   BlueFilterBandwidth_NumericEdit.label.Hide();
   BlueFilterBandwidth_NumericEdit.edit.SetFixedWidth( editWidth3 );
   BlueFilterBandwidth_NumericEdit.SetToolTip( "<p>Blue filter, bandwidth (nm).</p>" );
   BlueFilterBandwidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

   OptimizeForStars_CheckBox.SetText( "Optimize for stars" );
   OptimizeForStars_CheckBox.SetToolTip( "<p>Enable this option to perform a color calibration with narrowband filters "
      "optimized for star images. Disable it to optimize calibration for extended objects.</p>" );
   OptimizeForStars_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   BlueFilterParameters_Sizer.Add( BlueFilterWavelength_NumericEdit );
   BlueFilterParameters_Sizer.AddSpacing( 8 );
   BlueFilterParameters_Sizer.Add( BlueFilterBandwidth_NumericEdit );
   BlueFilterParameters_Sizer.AddSpacing( 24 );
   BlueFilterParameters_Sizer.Add( OptimizeForStars_CheckBox );
   BlueFilterParameters_Sizer.AddStretch();

   NarrowbandFilters_Sizer.SetSpacing( 4 );
   NarrowbandFilters_Sizer.Add( FiltersTitle_Sizer );
   NarrowbandFilters_Sizer.Add( RedFilterParameters_Sizer );
   NarrowbandFilters_Sizer.Add( GreenFilterParameters_Sizer );
   NarrowbandFilters_Sizer.Add( BlueFilterParameters_Sizer );

   NarrowbandFilters_Control.SetSizer( NarrowbandFilters_Sizer );

   //

   NarrowbandMode_CheckBox.SetText( "Narrowband filters mode" );
   NarrowbandMode_CheckBox.SetToolTip( "<p>Enable the narrowband filters working mode.</p>" );
   NarrowbandMode_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   NarrowbandMode_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   NarrowbandMode_Sizer.Add( NarrowbandMode_CheckBox );
   NarrowbandMode_Sizer.AddStretch();

   GenerateGraphs_CheckBox.SetText( "Generate graphs" );
   GenerateGraphs_CheckBox.SetToolTip( "<p>Generate interactive graphs to analyze the function fits among catalog "
      "and image color ratios for measured stars.</p>" );
   GenerateGraphs_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   GenerateGraphs_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateGraphs_Sizer.Add( GenerateGraphs_CheckBox );
   GenerateGraphs_Sizer.AddStretch();

   GenerateStarMaps_CheckBox.SetText( "Generate star maps" );
   GenerateStarMaps_CheckBox.SetToolTip( "<p>Generate monochrome 8-bit images representing the measured stars. These images "
      "can be used as masks on the target image.</p>" );
   GenerateStarMaps_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   GenerateStarMaps_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateStarMaps_Sizer.Add( GenerateStarMaps_CheckBox );
   GenerateStarMaps_Sizer.AddStretch();

   GenerateTextFiles_CheckBox.SetText( "Generate text files" );
   GenerateTextFiles_CheckBox.SetToolTip( "<p>Generate text files in CSV format with positional, catalog and photometric data.</p>" );
   GenerateTextFiles_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   GenerateTextFiles_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateTextFiles_Sizer.Add( GenerateTextFiles_CheckBox );
   GenerateTextFiles_Sizer.AddStretch();

   const char* outputDirectoryToolTip = "<p>The directory (or folder) where output text files (in CSV format) will be written.</p>"
      "<p>If this field is left blank (or with its default &lt;system-tmp-dir&gt; value), output files will be written to the "
      "system temporary directory.</p>";

   OutputDirectory_Label.SetText( "Output directory:" );
   OutputDirectory_Label.SetFixedWidth( labelWidth1 );
   OutputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputDirectory_Label.SetToolTip( outputDirectoryToolTip );

   OutputDirectory_Edit.SetToolTip( outputDirectoryToolTip );
   OutputDirectory_Edit.OnGetFocus( (Control::event_handler)&SpectrophotometricColorCalibrationInterface::e_GetFocus, w );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&SpectrophotometricColorCalibrationInterface::e_EditCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)&SpectrophotometricColorCalibrationInterface::e_FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)&SpectrophotometricColorCalibrationInterface::e_FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.SetToolTip( "<p>Select the output directory for CSV files.</p>" );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   ApplyColorCalibration_CheckBox.SetText( "Apply color calibration" );
   ApplyColorCalibration_CheckBox.SetToolTip( "<p>Disable this option to compute color calibration functions "
      "and scaling factors, but <i>without applying them</i> to the target image. This option should normally be enabled.</p>" );
   ApplyColorCalibration_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   ApplyColorCalibration_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ApplyColorCalibration_Sizer.Add( ApplyColorCalibration_CheckBox );
   ApplyColorCalibration_Sizer.AddStretch();

   Calibration_Sizer.SetSpacing( 4 );
   Calibration_Sizer.Add( WhiteReference_Sizer );
   Calibration_Sizer.Add( DeviceQE_Sizer );
   Calibration_Sizer.Add( BroadbandFilters_Control );
   Calibration_Sizer.Add( NarrowbandFilters_Control );
   Calibration_Sizer.Add( NarrowbandMode_Sizer );
   Calibration_Sizer.Add( GenerateGraphs_Sizer );
   Calibration_Sizer.Add( GenerateStarMaps_Sizer );
   Calibration_Sizer.Add( GenerateTextFiles_Sizer );
   Calibration_Sizer.Add( OutputDirectory_Sizer );
   Calibration_Sizer.Add( ApplyColorCalibration_Sizer );

   Calibration_Control.SetSizer( Calibration_Sizer );

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
   Catalog_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricColorCalibrationInterface::e_ItemSelected, w );

   Catalog_Sizer.SetSpacing( 4 );
   Catalog_Sizer.Add( Catalog_Label );
   Catalog_Sizer.Add( Catalog_ComboBox );
   Catalog_Sizer.AddStretch();

   AutoLimitMagnitude_CheckBox.SetText( "Automatic limit magnitude" );
   AutoLimitMagnitude_CheckBox.SetToolTip( "<p>When this option is enabled, SPCC will select an optimal limit magnitude "
      "for catalog search operations based on the field of view and center coordinates of the target image. Enabling this "
      "option is recommended under normal working conditions.</p>" );
   AutoLimitMagnitude_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   AutoLimitMagnitude_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AutoLimitMagnitude_Sizer.Add( AutoLimitMagnitude_CheckBox );
   AutoLimitMagnitude_Sizer.AddStretch();

   LimitMagnitude_NumericEdit.label.SetText( "Limit magnitude:" );
   LimitMagnitude_NumericEdit.label.SetFixedWidth( labelWidth1 );
   LimitMagnitude_NumericEdit.SetReal();
   LimitMagnitude_NumericEdit.SetRange( TheSPCCLimitMagnitudeParameter->MinimumValue(), TheSPCCLimitMagnitudeParameter->MaximumValue() );
   LimitMagnitude_NumericEdit.SetPrecision( TheSPCCLimitMagnitudeParameter->Precision() );
   LimitMagnitude_NumericEdit.edit.SetFixedWidth( editWidth1 );
   LimitMagnitude_NumericEdit.sizer.AddStretch();
   LimitMagnitude_NumericEdit.SetToolTip( "<p>This parameter limits the number of stars extracted from the spectrophotometric "
      "catalog to those with magnitudes (in the visual or G band) smaller than this value. The optimal value depends on the "
      "field of view of the target image. If this value is too low, the color calibration algorithm may not have enough stars "
      "to compute robust white balance factors. Too high of a limit magnitude will slow down the process without any benefit. "
      "The <i>Automatic limit magnitude</i> option is strongly recommended instead of specifying a literal limit magnitude "
      "value under normal working conditions.</p>" );
   LimitMagnitude_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

   CatalogSearch_Sizer.SetSpacing( 4 );
   CatalogSearch_Sizer.Add( Catalog_Sizer );
   CatalogSearch_Sizer.Add( AutoLimitMagnitude_Sizer );
   CatalogSearch_Sizer.Add( LimitMagnitude_NumericEdit );

   CatalogSearch_Control.SetSizer( CatalogSearch_Sizer );

   //

   SignalEvaluation_SectionBar.SetTitle( "Signal Evaluation" );
   SignalEvaluation_SectionBar.SetToolTip( "<p>SPCC detects stars in the target image and fits a point spread function model to each "
      "detected source separately in each nominal RGB channel. The fitted PSF parameters are then used to guide evaluation of the "
      "total flux of each source, and the RGB fluxes of matched stars in the selected spectrophotometric catalog are used to compute "
      "a robust and precise white balance factor based on star spectra and filter transmission functions.</p>" );
   SignalEvaluation_SectionBar.SetSection( SignalEvaluation_Control );

   const char* scaleStructureLayersToolTip =
   "<p>Number of wavelet layers used for structure detection.</p>"
   "<p>With more wavelet layers, larger stars will be detected and used for signal evaluation.</p>";

   StructureLayers_Label.SetText( "Detection scales:" );
   StructureLayers_Label.SetFixedWidth( labelWidth1 );
   StructureLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StructureLayers_Label.SetToolTip( scaleStructureLayersToolTip );

   StructureLayers_SpinBox.SetRange( int( TheSPCCStructureLayersParameter->MinimumValue() ), int( TheSPCCStructureLayersParameter->MaximumValue() ) );
   StructureLayers_SpinBox.SetToolTip( scaleStructureLayersToolTip );
   StructureLayers_SpinBox.SetFixedWidth( editWidth1 );
   StructureLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   StructureLayers_Sizer.SetSpacing( 4 );
   StructureLayers_Sizer.Add( StructureLayers_Label );
   StructureLayers_Sizer.Add( StructureLayers_SpinBox );
   StructureLayers_Sizer.AddStretch();

   SaturationThreshold_NumericControl.label.SetText( "Saturation threshold:" );
   SaturationThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   SaturationThreshold_NumericControl.slider.SetRange( 0, 250 );
   SaturationThreshold_NumericControl.SetReal();
   SaturationThreshold_NumericControl.SetRange( TheSPCCSaturationThresholdParameter->MinimumValue(), TheSPCCSaturationThresholdParameter->MaximumValue() );
   SaturationThreshold_NumericControl.SetPrecision( TheSPCCSaturationThresholdParameter->Precision() );
   SaturationThreshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   SaturationThreshold_NumericControl.SetToolTip( "<p>Saturation threshold in the [0,1] range.</p>"
      "<p>Detected stars with one or more pixels with values above this threshold will be excluded for signal evaluation.</p>"
      "<p>This parameter is expressed in the [0,1] range. It can be applied either as an absolute pixel sample value in the "
      "normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image (see the <i>Relative "
      "saturation threshold</i> parameter).</p>"
      "<p>The default saturation threshold is 0.75. For signal evaluation this parameter plays an imortant role to prevent "
      "contamination of the statistical sample of flux ratios by sources with saturated or nonlinear data. Changing the default "
      "value should not be necessary under normal conditions.</p>" );
   SaturationThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

   SaturationRelative_CheckBox.SetText( "Relative saturation threshold" );
   SaturationRelative_CheckBox.SetToolTip( "<p>The saturation threshold parameter can be applied either as an absolute pixel "
      "sample value in the normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image.</p>"
      "The relative saturation threshold option is enabled by default.</p>" );
   SaturationRelative_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   SaturationRelative_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SaturationRelative_Sizer.Add( SaturationRelative_CheckBox );
   SaturationRelative_Sizer.AddStretch();

   const char* psfNoiseLayersToolTip =
      "<p>Star detector: Number of wavelet layers used for noise reduction.</p>"
      "<p>Noise reduction prevents detection of bright noise structures as false stars, including hot pixels and "
      "cosmic rays. This parameter can also be used to control the sizes of the smallest detected stars (increase "
      "to exclude more stars), although the <i>minimum structure size</i> parameter can be more efficient for this purpose.</p>";

   PSFNoiseLayers_Label.SetText( "Noise scales:" );
   PSFNoiseLayers_Label.SetFixedWidth( labelWidth1 );
   PSFNoiseLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PSFNoiseLayers_Label.SetToolTip( psfNoiseLayersToolTip );

   PSFNoiseLayers_SpinBox.SetRange( int( TheSPCCPSFNoiseLayersParameter->MinimumValue() ), int( TheSPCCPSFNoiseLayersParameter->MaximumValue() ) );
   PSFNoiseLayers_SpinBox.SetToolTip( psfNoiseLayersToolTip );
   PSFNoiseLayers_SpinBox.SetFixedWidth( editWidth1 );
   PSFNoiseLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_SpinValueUpdated, w );

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

   PSFMinStructureSize_SpinBox.SetRange( int( TheSPCCPSFMinStructureSizeParameter->MinimumValue() ), int( TheSPCCPSFMinStructureSizeParameter->MaximumValue() ) );
   PSFMinStructureSize_SpinBox.SetMinimumValueText( "<Auto>" );
   PSFMinStructureSize_SpinBox.SetToolTip( psfMinStructureSizeToolTip );
   PSFMinStructureSize_SpinBox.SetFixedWidth( editWidth1 );
   PSFMinStructureSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_SpinValueUpdated, w );

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

   PSFHotPixelFilterRadius_SpinBox.SetRange( int( TheSPCCPSFHotPixelFilterRadiusParameter->MinimumValue() ), int( TheSPCCPSFHotPixelFilterRadiusParameter->MaximumValue() ) );
   PSFHotPixelFilterRadius_SpinBox.SetToolTip( psfHotPixelFilterRadiusToolTip );
   PSFHotPixelFilterRadius_SpinBox.SetFixedWidth( editWidth1 );
   PSFHotPixelFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_SpinValueUpdated, w );

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

   PSFNoiseReductionFilterRadius_SpinBox.SetRange( int( TheSPCCPSFNoiseReductionFilterRadiusParameter->MinimumValue() ), int( TheSPCCPSFNoiseReductionFilterRadiusParameter->MaximumValue() ) );
   PSFNoiseReductionFilterRadius_SpinBox.SetToolTip( psfNoiseReductionFilterRadiusToolTip );
   PSFNoiseReductionFilterRadius_SpinBox.SetFixedWidth( editWidth1 );
   PSFNoiseReductionFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   PSFNoiseReductionFilterRadius_Sizer.SetSpacing( 4 );
   PSFNoiseReductionFilterRadius_Sizer.Add( PSFNoiseReductionFilterRadius_Label );
   PSFNoiseReductionFilterRadius_Sizer.Add( PSFNoiseReductionFilterRadius_SpinBox );
   PSFNoiseReductionFilterRadius_Sizer.AddStretch();

   PSFMinSNR_NumericEdit.label.SetText( "Minimum detection SNR:" );
   PSFMinSNR_NumericEdit.label.SetFixedWidth( labelWidth1 );
   PSFMinSNR_NumericEdit.SetReal();
   PSFMinSNR_NumericEdit.SetRange( TheSPCCPSFMinSNRParameter->MinimumValue(), TheSPCCPSFMinSNRParameter->MaximumValue() );
   PSFMinSNR_NumericEdit.SetPrecision( TheSPCCPSFMinSNRParameter->Precision() );
   PSFMinSNR_NumericEdit.edit.SetFixedWidth( editWidth1 );
   PSFMinSNR_NumericEdit.sizer.AddStretch();
   PSFMinSNR_NumericEdit.SetToolTip( "<p>Minimum signal-to-noise ratio of a detectable star.</p>"
      "<p>Given a source with estimated brightness <i>s</i>, local background <i>b</i> and local background dispersion "
      "<i>n</i>, SNR is evaluated as (<i>s</i> &ndash; <i>b</i>)/<i>n</i>. Stars with measured SNR below the value of this "
      "parameter won't be used for signal evaluation.</p>"
      "<p>This parameter allows limiting star detection to a subset of the brightest sources in the image adaptively. By "
      "requiring relatively high SNR levels in the evaluated sources, the accuracy and robustness of the signal evaluation "
      "process can be largely improved. The default value is 40, which is quite appropriate in most cases.</p>" );
   PSFMinSNR_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

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
   PSFType_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SpectrophotometricColorCalibrationInterface::e_ItemSelected, w );

   PSFType_Sizer.SetSpacing( 4 );
   PSFType_Sizer.Add( PSFType_Label );
   PSFType_Sizer.Add( PSFType_ComboBox );
   PSFType_Sizer.AddStretch();

   PSFGrowth_NumericControl.label.SetText( "Growth factor:" );
   PSFGrowth_NumericControl.label.SetFixedWidth( labelWidth1 );
   PSFGrowth_NumericControl.slider.SetRange( 0, 250 );
   PSFGrowth_NumericControl.SetReal();
   PSFGrowth_NumericControl.SetRange( TheSPCCPSFGrowthParameter->MinimumValue(), TheSPCCPSFGrowthParameter->MaximumValue() );
   PSFGrowth_NumericControl.SetPrecision( TheSPCCPSFGrowthParameter->Precision() );
   PSFGrowth_NumericControl.edit.SetFixedWidth( editWidth1 );
   PSFGrowth_NumericControl.SetToolTip( "<p>Growing factor for expansion/contraction of the PSF flux measurement region for "
      "each source, in units of the Full Width at Tenth Maximum (FWTM).</p>"
      "<p>The default value of this parameter is 1.0, meaning that flux is measured exclusively for pixels within the elliptical "
      "region defined at one tenth of the fitted PSF maximum. Increasing this parameter can inprove accuracy of PSF flux "
      "measurements for undersampled images, where PSF fitting uncertainty is relatively large. Decreasing it can be beneficial "
      "in some cases working with very noisy data to restrict flux evaluation to star cores.</p>" );
   PSFGrowth_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

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

   PSFMaxStars_SpinBox.SetRange( int( TheSPCCPSFMaxStarsParameter->MinimumValue() ), int( TheSPCCPSFMaxStarsParameter->MaximumValue() ) );
   PSFMaxStars_SpinBox.SetToolTip( psfMaxStarsToolTip );
   PSFMaxStars_SpinBox.SetFixedWidth( editWidth2 );
   PSFMaxStars_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   PSFMaxStars_Sizer.SetSpacing( 4 );
   PSFMaxStars_Sizer.Add( PSFMaxStars_Label );
   PSFMaxStars_Sizer.Add( PSFMaxStars_SpinBox );
   PSFMaxStars_Sizer.AddStretch();

   PSFAllowClusteredSources_CheckBox.SetText( "Allow clustered sources" );
   PSFAllowClusteredSources_CheckBox.SetToolTip( "<p>If this parameter is disabled, multiple sources that are too close to be "
      "separated as individual structures, such as double and multiple stars, won't be detected for signal evaluation.</p>"
      "<p>If this parameter is enabled, non-separable multiple sources will be freely detectable as single objects.</p>"
      "<p>In general, sources with several local maxima pose difficulties for the determination of accurate star positions and PSF "
      "parameters, although this is usually a minor problem for signal evaluation. For this reason this parameter is enabled by "
      "default.</p>" );
   PSFAllowClusteredSources_CheckBox.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   PSFAllowClusteredSources_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PSFAllowClusteredSources_Sizer.Add( PSFAllowClusteredSources_CheckBox );
   PSFAllowClusteredSources_Sizer.AddStretch();

   SignalEvaluation_Sizer.SetSpacing( 4 );
   SignalEvaluation_Sizer.Add( StructureLayers_Sizer );
   SignalEvaluation_Sizer.Add( SaturationThreshold_NumericControl );
   SignalEvaluation_Sizer.Add( SaturationRelative_Sizer );
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

   BackgroundReference_SectionBar.SetTitle( "Background Neutralization" );
   BackgroundReference_SectionBar.EnableTitleCheckBox();
   BackgroundReference_SectionBar.OnCheck( (SectionBar::check_event_handler)&SpectrophotometricColorCalibrationInterface::e_SectionCheck, w );
   BackgroundReference_SectionBar.SetSection( BackgroundReference_Control );

   //

   BackgroundReferenceViewId_Label.SetText( "Reference image:" );
   BackgroundReferenceViewId_Label.SetFixedWidth( labelWidth1 );
   BackgroundReferenceViewId_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   BackgroundReferenceViewId_Edit.SetToolTip( "<p>Background reference image.</p>"
      "<p>SPCC will use pixels read from this image to compute a mean background level for each color channel. "
      "If you leave this field blank (or with its default &lt;target-image&gt; value), the target image will be "
      "also the background reference image during the color calibration process.</p>"
      "<p>You should specify a view that represents the <i>true background</i> of the image. In most cases this "
      "means that you must select a view whose pixels are strongly dominated by the sky background, as it is "
      "being represented on the target image. A typical example involves defining a small preview over a free "
      "sky area of the target image, and selecting it here as the background reference image.</p>"
      "<p>You can also drag a view selector to this control, and the corresponding view will be selected.</p>" );
   BackgroundReferenceViewId_Edit.OnGetFocus( (Control::event_handler)&SpectrophotometricColorCalibrationInterface::e_GetFocus, w );
   BackgroundReferenceViewId_Edit.OnEditCompleted( (Edit::edit_event_handler)&SpectrophotometricColorCalibrationInterface::e_EditCompleted, w );
   BackgroundReferenceViewId_Edit.OnViewDrag( (Control::view_drag_event_handler)&SpectrophotometricColorCalibrationInterface::e_ViewDrag, w );
   BackgroundReferenceViewId_Edit.OnViewDrop( (Control::view_drop_event_handler)&SpectrophotometricColorCalibrationInterface::e_ViewDrop, w );

   BackgroundReferenceViewId_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   BackgroundReferenceViewId_ToolButton.SetScaledFixedSize( 20, 20 );
   BackgroundReferenceViewId_ToolButton.SetToolTip( "<p>Select the background reference image.</p>" );
   BackgroundReferenceViewId_ToolButton.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );

   BackgroundReferenceViewId_Sizer.SetSpacing( 4 );
   BackgroundReferenceViewId_Sizer.Add( BackgroundReferenceViewId_Label );
   BackgroundReferenceViewId_Sizer.Add( BackgroundReferenceViewId_Edit );
   BackgroundReferenceViewId_Sizer.Add( BackgroundReferenceViewId_ToolButton );

   //

   BackgroundLow_NumericControl.label.SetText( "Lower limit:" );
   BackgroundLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundLow_NumericControl.slider.SetRange( -100, 100 );
   BackgroundLow_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundLow_NumericControl.SetReal();
   BackgroundLow_NumericControl.EnableFixedSign();
   BackgroundLow_NumericControl.SetRange( TheSPCCBackgroundLowParameter->MinimumValue(), TheSPCCBackgroundLowParameter->MaximumValue() );
   BackgroundLow_NumericControl.SetPrecision( TheSPCCBackgroundLowParameter->Precision() );
   BackgroundLow_NumericControl.SetToolTip( "<p>Lower bound of the set of background reference pixels, measured in sigma units "
      "from the the median of the background reference for each nominal channel. Background reference pixels with values less "
      "than or equal to this lower bound will be rejected for calculation of mean background levels.</p>"
      "<p>The default value is -2.8 sigma.</p>" );
   BackgroundLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

   //

   BackgroundHigh_NumericControl.label.SetText( "Upper limit:" );
   BackgroundHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundHigh_NumericControl.slider.SetRange( -100, 100 );
   BackgroundHigh_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundHigh_NumericControl.SetReal();
   BackgroundHigh_NumericControl.EnableFixedSign();
   BackgroundHigh_NumericControl.SetRange( TheSPCCBackgroundHighParameter->MinimumValue(), TheSPCCBackgroundHighParameter->MaximumValue() );
   BackgroundHigh_NumericControl.SetPrecision( TheSPCCBackgroundHighParameter->Precision() );
   BackgroundHigh_NumericControl.SetToolTip( "<p>Upper bound of the set of background reference pixels, measured in sigma units "
      "from the the median of the background reference for each nominal channel. Background reference pixels with values greater "
      "than or equal to this upper bound will be rejected for calculation of mean background levels.</p>"
      "<p>The default value is +2.0 sigma.</p>" );
   BackgroundHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_ValueUpdated, w );

   //

   BackgroundROI_GroupBox.SetTitle( "Region of Interest" );
   BackgroundROI_GroupBox.EnableTitleCheckBox();
   BackgroundROI_GroupBox.SetToolTip( "<p>You <i>should</i> define a rectangular region of interest (ROI) to restrict background "
      "evaluation to pixels within a region that represents the <i>true background</i> of the target image. If no ROI is defined, "
      "background evaluation will be performed on the entire target image, which can be inaccurate, especially for images with few "
      "free sky background regions.</p>" );
   BackgroundROI_GroupBox.OnCheck( (GroupBox::check_event_handler)&SpectrophotometricColorCalibrationInterface::e_GroupCheck, w );
   BackgroundROI_GroupBox.OnViewDrag( (Control::view_drag_event_handler)&SpectrophotometricColorCalibrationInterface::e_ViewDrag, w );
   BackgroundROI_GroupBox.OnViewDrop( (Control::view_drop_event_handler)&SpectrophotometricColorCalibrationInterface::e_ViewDrop, w );

   //

#define DELTA_FRAME  1 // width of a GroupBox frame line

   const char* backgroundROIX0ToolTip = "<p>X pixel coordinate of the upper-left corner of the ROI.</p>";

   BackgroundROIX0_Label.SetText( "Left:" );
   BackgroundROIX0_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   BackgroundROIX0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIX0_Label.SetToolTip( backgroundROIX0ToolTip );

   BackgroundROIX0_SpinBox.SetRange( 0, int_max );
   BackgroundROIX0_SpinBox.SetToolTip( backgroundROIX0ToolTip );
   BackgroundROIX0_SpinBox.SetMinWidth( editWidth2 );
   BackgroundROIX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   //

   const char* backgroundROIY0ToolTip = "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>";

   BackgroundROIY0_Label.SetText( "Top:" );
   BackgroundROIY0_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIY0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIY0_Label.SetToolTip( backgroundROIY0ToolTip );

   BackgroundROIY0_SpinBox.SetRange( 0, int_max );
   BackgroundROIY0_SpinBox.SetToolTip( backgroundROIY0ToolTip );
   BackgroundROIY0_SpinBox.SetMinWidth( editWidth2 );
   BackgroundROIY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   //

   BackgroundROIRow1_Sizer.SetSpacing( 4 );
   BackgroundROIRow1_Sizer.Add( BackgroundROIX0_Label );
   BackgroundROIRow1_Sizer.Add( BackgroundROIX0_SpinBox );
   BackgroundROIRow1_Sizer.Add( BackgroundROIY0_Label );
   BackgroundROIRow1_Sizer.Add( BackgroundROIY0_SpinBox );
   BackgroundROIRow1_Sizer.AddStretch();

   //

   const char* backgroundROIWidthToolTip = "<p>Width of the ROI in pixels.</p>";

   BackgroundROIWidth_Label.SetText( "Width:" );
   BackgroundROIWidth_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   BackgroundROIWidth_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIWidth_Label.SetToolTip( backgroundROIWidthToolTip );

   BackgroundROIWidth_SpinBox.SetRange( 0, int_max );
   BackgroundROIWidth_SpinBox.SetToolTip( backgroundROIWidthToolTip );
   BackgroundROIWidth_SpinBox.SetMinWidth( editWidth2 );
   BackgroundROIWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   //

   const char* backgroundROIHeightToolTip = "<p>Height of the ROI in pixels.</p>";

   BackgroundROIHeight_Label.SetText( "Height:" );
   BackgroundROIHeight_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIHeight_Label.SetToolTip( backgroundROIHeightToolTip );

   BackgroundROIHeight_SpinBox.SetRange( 0, int_max );
   BackgroundROIHeight_SpinBox.SetToolTip( backgroundROIHeightToolTip );
   BackgroundROIHeight_SpinBox.SetMinWidth( editWidth2 );
   BackgroundROIHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SpectrophotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   BackgroundROISelectPreview_Button.SetText( "From Preview" );
   BackgroundROISelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview.</p>"
      "<p>Or much easier, drag a preview selector to this button and the corresponding coordinates will be selected automatically.</p>" );
   BackgroundROISelectPreview_Button.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationInterface::e_Click, w );
   BackgroundROISelectPreview_Button.OnViewDrag( (Control::view_drag_event_handler)&SpectrophotometricColorCalibrationInterface::e_ViewDrag, w );
   BackgroundROISelectPreview_Button.OnViewDrop( (Control::view_drop_event_handler)&SpectrophotometricColorCalibrationInterface::e_ViewDrop, w );

   //

   BackgroundROIRow2_Sizer.SetSpacing( 4 );
   BackgroundROIRow2_Sizer.Add( BackgroundROIWidth_Label );
   BackgroundROIRow2_Sizer.Add( BackgroundROIWidth_SpinBox );
   BackgroundROIRow2_Sizer.Add( BackgroundROIHeight_Label );
   BackgroundROIRow2_Sizer.Add( BackgroundROIHeight_SpinBox );
   BackgroundROIRow2_Sizer.AddSpacing( 12 );
   BackgroundROIRow2_Sizer.Add( BackgroundROISelectPreview_Button );
   BackgroundROIRow2_Sizer.AddStretch();

   //

   BackgroundROI_Sizer.SetMargin( 6 );
   BackgroundROI_Sizer.SetSpacing( 4 );
   BackgroundROI_Sizer.Add( BackgroundROIRow1_Sizer );
   BackgroundROI_Sizer.Add( BackgroundROIRow2_Sizer );

   BackgroundROI_GroupBox.SetSizer( BackgroundROI_Sizer );

   //

   BackgroundReference_Sizer.SetSpacing( 4 );
   BackgroundReference_Sizer.Add( BackgroundReferenceViewId_Sizer );
   BackgroundReference_Sizer.Add( BackgroundLow_NumericControl );
   BackgroundReference_Sizer.Add( BackgroundHigh_NumericControl );
   BackgroundReference_Sizer.Add( BackgroundROI_GroupBox );

   BackgroundReference_Control.SetSizer( BackgroundReference_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( Calibration_SectionBar );
   Global_Sizer.Add( Calibration_Control );
   Global_Sizer.Add( CatalogSearch_SectionBar );
   Global_Sizer.Add( CatalogSearch_Control );
   Global_Sizer.Add( SignalEvaluation_SectionBar );
   Global_Sizer.Add( SignalEvaluation_Control );
   Global_Sizer.Add( BackgroundReference_SectionBar );
   Global_Sizer.Add( BackgroundReference_Control );

   //

   w.SetSizer( Global_Sizer );

   // The narrowband filters working mode is disabled by default.
   NarrowbandFilters_Control.Hide();

   SignalEvaluation_Control.Hide();

   /*
    * N.B.: Using a separate view as background reference with PCC is an
    * "advanced" (that is, usually wrong) option. So we'll hide these controls
    * for sanity.
    */
   BackgroundReferenceViewId_Label.Hide();
   BackgroundReferenceViewId_Edit.Hide();
   BackgroundReferenceViewId_ToolButton.Hide();

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SpectrophotometricColorCalibrationInterface.cpp - Released 2023-08-28T15:23:41Z
