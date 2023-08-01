//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.7
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationInterface.cpp - Released 2023-08-01T16:30:17Z
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

#include "PhotometricColorCalibrationGraphInterface.h"
#include "PhotometricColorCalibrationInterface.h"
#include "PhotometricColorCalibrationProcess.h"
#include "PhotometricColorCalibrationParameters.h"

#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/PreviewSelectionDialog.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct ServerData
{
   String name;
   String url;
};

struct CatalogData
{
   String id;
   String name;
   String description;
};

static Array<ServerData>  s_vizierServers;
static Array<CatalogData> s_photometryCatalogs;
static bool               s_dataInitialized = false;

static void InitializeServerAndCatalogData()
{
   if ( !s_dataInitialized )
   {
      s_vizierServers.Clear();
      s_vizierServers << ServerData{ "CDS Strasbourg, France", "https://vizier.cds.unistra.fr/" }
                      << ServerData{ "ADAC Tokyo, Japan", "http://vizier.nao.ac.jp/" }
                      << ServerData{ "IUCAA Pune, India", "https://vizier.iucaa.in/" }
                      << ServerData{ "INASAN Moscow, Russia", "https://vizier.inasan.ru/" }
                      << ServerData{ "NAOC Beijing, China", "http://vizier.china-vo.org/" }
                      << ServerData{ "CFA Harvard, Cambridge, USA", "https://vizier.cfa.harvard.edu/" }
                      << ServerData{ "IDIA, South Africa", "http://vizier.idia.ac.za/" };

      s_photometryCatalogs.Clear();
      s_photometryCatalogs << CatalogData{ "APASS", "APASS DR9 (VizieR)", "AAVSO Photometric All Sky Survey DR9 - Online VizieR Service (Henden+, 2016, 62 million stars)" }
                           << CatalogData{ "APASSDR9_XPSD", "APASS DR9 (XPSD)", "AAVSO Photometric All Sky Survey DR9 - Local XPSD Server (Henden+, 2016, 62 million stars)" }
                           << CatalogData{ "APASSDR10_XPSD", "APASS DR10 (XPSD)", "AAVSO Photometric All Sky Survey DR10 - Local XPSD Server (Henden+, 2018, 128 million stars)" };

      s_dataInitialized = true;
   }
}

// ----------------------------------------------------------------------------

PhotometricColorCalibrationInterface* ThePhotometricColorCalibrationInterface = nullptr;

// ----------------------------------------------------------------------------

PhotometricColorCalibrationInterface::PhotometricColorCalibrationInterface()
   : m_instance( ThePhotometricColorCalibrationProcess )
{
   ThePhotometricColorCalibrationInterface = this;
}

// ----------------------------------------------------------------------------

PhotometricColorCalibrationInterface::~PhotometricColorCalibrationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString PhotometricColorCalibrationInterface::Id() const
{
   return "PhotometricColorCalibration";
}

// ----------------------------------------------------------------------------

MetaProcess* PhotometricColorCalibrationInterface::Process() const
{
   return ThePhotometricColorCalibrationProcess;
}

// ----------------------------------------------------------------------------

String PhotometricColorCalibrationInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/PhotometricColorCalibration.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures PhotometricColorCalibrationInterface::Features() const
{
   return InterfaceFeature::Default;
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::ResetInstance()
{
   PhotometricColorCalibrationInstance defaultInstance( ThePhotometricColorCalibrationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   PhotometricColorCalibrationProcess::InitializeWhiteReferences();

   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "PhotometricColorCalibration" );
      UpdateControls();
   }

   dynamic = false;
   return &P == ThePhotometricColorCalibrationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* PhotometricColorCalibrationInterface::NewProcess() const
{
   return new PhotometricColorCalibrationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const PhotometricColorCalibrationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a PhotometricColorCalibration instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInterface::ImportProcess( const ProcessImplementation& p )
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
   for ( int i = 0; i < int( s_photometryCatalogs.Length() ); ++i )
      if ( s_photometryCatalogs[i].id == catalogId )
         return i;
   return -1;
}

static String CatalogIdFromComboBoxItemIndex( int itemIndex )
{
   if ( itemIndex < int( s_photometryCatalogs.Length() ) )
      return s_photometryCatalogs[itemIndex].id;
   return String();
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::UpdateControls()
{
   if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
   {
      int itemIndex = PhotometricColorCalibrationProcess::FindWhiteReferenceById( m_instance.p_whiteReferenceId );
      if ( itemIndex < 0 )
      {
         itemIndex = int( PhotometricColorCalibrationProcess::WhiteReferences().Length() );
         if ( GUI->WhiteReference_ComboBox.NumberOfItems() < itemIndex )
            GUI->WhiteReference_ComboBox.AddItem( "<custom reference>" );
      }
      GUI->WhiteReference_ComboBox.SetCurrentItem( itemIndex );
   }

   GUI->GenerateGraphs_CheckBox.SetChecked( m_instance.p_generateGraphs );
   GUI->GenerateStarMaps_CheckBox.SetChecked( m_instance.p_generateStarMaps );
   GUI->GenerateTextFiles_CheckBox.SetChecked( m_instance.p_generateTextFiles );

   GUI->OutputDirectory_Label.Enable( m_instance.p_generateTextFiles );
   GUI->OutputDirectory_Edit.SetText( OUTPUT_DIR_INSTANCE );
   GUI->OutputDirectory_Edit.Enable( m_instance.p_generateTextFiles );
   GUI->OutputDirectory_ToolButton.Enable( m_instance.p_generateTextFiles );

   GUI->ApplyColorCalibration_CheckBox.SetChecked( m_instance.p_applyCalibration );

   GUI->AutoCatalog_CheckBox.SetChecked( m_instance.p_autoCatalog );

   {
      int itemIndex = ComboBoxItemIndexFromCatalogId( m_instance.p_catalogId );
      if ( itemIndex < 0 )
      {
         itemIndex = GUI->Catalog_ComboBox.NumberOfItems();
         GUI->Catalog_ComboBox.AddItem( "<unknown catalog>" );
      }
      GUI->Catalog_ComboBox.SetCurrentItem( itemIndex );
   }

   GUI->Catalog_Label.Enable( !m_instance.p_autoCatalog );
   GUI->Catalog_ComboBox.Enable( !m_instance.p_autoCatalog );

   {
      int itemIndex = -1;
      for ( size_type i = 0; i < s_vizierServers.Length(); ++i )
         if ( m_instance.p_serverURL == s_vizierServers[i].url )
         {
            itemIndex = int( i );
            break;
         }
      if ( itemIndex < 0 )
      {
         itemIndex = int( s_vizierServers.Length() );
         if ( GUI->Server_ComboBox.NumberOfItems() < itemIndex+1 )
            GUI->Server_ComboBox.AddItem( "<custom server>" );
      }
      GUI->Server_ComboBox.SetCurrentItem( itemIndex );
      GUI->Server_ComboBox.SetToolTip( m_instance.p_serverURL );
   }

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

void PhotometricColorCalibrationInterface::e_GetFocus( Control& sender )
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

void PhotometricColorCalibrationInterface::e_EditCompleted( Edit& sender )
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

void PhotometricColorCalibrationInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->WhiteReference_ComboBox )
   {
      if ( itemIndex >= 0 )
         if ( size_type( itemIndex ) < PhotometricColorCalibrationProcess::WhiteReferences().Length() )
         {
            const WhiteReference& W = PhotometricColorCalibrationProcess::WhiteReferences()[itemIndex];
            m_instance.p_whiteReferenceId = W.id;
            m_instance.p_whiteReferenceName = W.name;
            m_instance.p_whiteReferenceJB_JV = W.JB_JV;
            m_instance.p_whiteReferenceSr_JV = W.Sr_JV;
            m_instance.p_zeroPointJB_JV = PhotometricColorCalibrationProcess::ZeroPoint().JB_JV;
            m_instance.p_zeroPointSr_JV = PhotometricColorCalibrationProcess::ZeroPoint().Sr_JV;
         }
   }
   else if ( sender == GUI->Catalog_ComboBox )
   {
      m_instance.p_catalogId = CatalogIdFromComboBoxItemIndex( itemIndex );
   }
   else if ( sender == GUI->Server_ComboBox )
   {
      if ( itemIndex >= 0 )
         if ( size_type( itemIndex ) < s_vizierServers.Length() )
            m_instance.p_serverURL = s_vizierServers[itemIndex].url;
      GUI->Server_ComboBox.SetToolTip( m_instance.p_serverURL );
   }
   else if ( sender == GUI->PSFType_ComboBox )
   {
      m_instance.p_psfType = itemIndex;
   }
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->GenerateGraphs_CheckBox )
   {
      m_instance.p_generateGraphs = checked;
      if ( checked )
         if ( !ThePhotometricColorCalibrationGraphInterface->IsVisible() )
            ThePhotometricColorCalibrationGraphInterface->Launch();
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
      d.SetCaption( "PCC: Select Output Directory" );
      if ( d.Execute() )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = d.Directory() );
   }
   else if ( sender == GUI->ApplyColorCalibration_CheckBox )
   {
      m_instance.p_applyCalibration = checked;
      UpdateControls();
   }
   else if ( sender == GUI->AutoCatalog_CheckBox )
   {
      m_instance.p_autoCatalog = checked;
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

void PhotometricColorCalibrationInterface::e_SectionCheck( SectionBar& sender, bool checked )
{
   if ( sender == GUI->BackgroundReference_SectionBar )
      m_instance.p_neutralizeBackground = checked;
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_GroupCheck( GroupBox& sender, bool checked )
{
   if ( sender == GUI->BackgroundROI_GroupBox )
      m_instance.p_backgroundUseROI = checked;
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->LimitMagnitude_NumericEdit )
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

void PhotometricColorCalibrationInterface::e_SpinValueUpdated( SpinBox& sender, int value )
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

void PhotometricColorCalibrationInterface::e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->BackgroundReferenceViewId_Edit )
      wantsView = true;
   else if ( sender == GUI->BackgroundROI_GroupBox || sender == GUI->BackgroundROISelectPreview_Button )
      wantsView = view.IsPreview();
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
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

void PhotometricColorCalibrationInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->OutputDirectory_Edit )
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = files[0] );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PhotometricColorCalibrationInterface::GUIData::GUIData( PhotometricColorCalibrationInterface& w )
{
   PhotometricColorCalibrationProcess::InitializeWhiteReferences();
   InitializeServerAndCatalogData();

   //

   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( "Minimum detection SNR:m" );
   int labelWidth2 = fnt.Width( "Height:m" );
   int editWidth1 = fnt.Width( "9999999m" );
   int editWidth2 = fnt.Width( String( '0', 10 ) );
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

   if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
   {
      for ( const WhiteReference& ref : PhotometricColorCalibrationProcess::WhiteReferences() )
         WhiteReference_ComboBox.AddItem( ref.name );
      WhiteReference_ComboBox.SetToolTip( whiteReferenceToolTip );
      WhiteReference_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PhotometricColorCalibrationInterface::e_ItemSelected, w );

      WhiteReference_Sizer.SetSpacing( 4 );
      WhiteReference_Sizer.Add( WhiteReference_Label );
      WhiteReference_Sizer.Add( WhiteReference_ComboBox );
      WhiteReference_Sizer.AddStretch();
   }
   else
   {
      WhiteReference_Label.Hide();
      WhiteReference_ComboBox.Hide();
   }

   //

   GenerateGraphs_CheckBox.SetText( "Generate graphs" );
   GenerateGraphs_CheckBox.SetToolTip( "<p>Generate interactive graphs to analyze the function fits among catalog "
      "and image color ratios for measured stars.</p>" );
   GenerateGraphs_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   GenerateGraphs_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateGraphs_Sizer.Add( GenerateGraphs_CheckBox );
   GenerateGraphs_Sizer.AddStretch();

   GenerateStarMaps_CheckBox.SetText( "Generate star maps" );
   GenerateStarMaps_CheckBox.SetToolTip( "<p>Generate monochrome 8-bit images representing the measured stars. These images "
      "can be used as masks on the target image.</p>" );
   GenerateStarMaps_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   GenerateStarMaps_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateStarMaps_Sizer.Add( GenerateStarMaps_CheckBox );
   GenerateStarMaps_Sizer.AddStretch();

   GenerateTextFiles_CheckBox.SetText( "Generate text files" );
   GenerateTextFiles_CheckBox.SetToolTip( "<p>Generate text files in CSV format with positional, catalog and photometric data.</p>" );
   GenerateTextFiles_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

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
   OutputDirectory_Edit.OnGetFocus( (Control::event_handler)&PhotometricColorCalibrationInterface::e_GetFocus, w );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&PhotometricColorCalibrationInterface::e_EditCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)&PhotometricColorCalibrationInterface::e_FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)&PhotometricColorCalibrationInterface::e_FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.SetToolTip( "<p>Select the output directory for CSV files.</p>" );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   ApplyColorCalibration_CheckBox.SetText( "Apply color calibration" );
   ApplyColorCalibration_CheckBox.SetToolTip( "<p>Disable this option to compute color calibration functions "
      "and scaling factors, but <i>without applying them</i> to the target image. This option should normally be enabled.</p>" );
   ApplyColorCalibration_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   ApplyColorCalibration_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ApplyColorCalibration_Sizer.Add( ApplyColorCalibration_CheckBox );
   ApplyColorCalibration_Sizer.AddStretch();

   Calibration_Sizer.SetSpacing( 4 );
   Calibration_Sizer.Add( WhiteReference_Sizer );
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

   AutoCatalog_CheckBox.SetText( "Automatic catalog" );
   AutoCatalog_CheckBox.SetToolTip( "<p>When this option is enabled, PhotometricColorCalibration will select an optimal "
      "photometric catalog automatically. If an APASS XPSD server is available and properly configured with local XPSD database files, "
      "it will be selected for the APASS DR10 or DR9 catalog, depending on database availability. Otherwise the APASS DR9 catalog will "
      "be used through an online VizieR service.</p>"
      "<p>Enabling this option is recommended under normal working conditions.</p>" );
   AutoCatalog_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   AutoCatalog_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AutoCatalog_Sizer.Add( AutoCatalog_CheckBox );
   AutoCatalog_Sizer.AddStretch();

   const char* catalogToolTip = "<p>This is the catalog used to acquire photometric data of stars.</p>"
      "<p>As of releasing this version of the PhotometricColorCalibration tool, the only catalog providing enough data for "
      "the implemented algorithms is the AAVSO Photometric All-Sky Survey (APASS). Future versions will support more "
      "catalogs as they become available and we manage to analyze them and adapt our algorithms as necessary.</p>"
      "<p>Under normal working conditions, you should enable the <i>Automatic catalog</i> option to use an optimal APASS "
      "catalog implementation selected as a function of database availability.</p>";

   Catalog_Label.SetText( "Catalog:" );
   Catalog_Label.SetFixedWidth( labelWidth1 );
   Catalog_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Catalog_Label.SetToolTip( catalogToolTip );

   for ( const CatalogData& catalog : s_photometryCatalogs )
      Catalog_ComboBox.AddItem( catalog.name );
   Catalog_ComboBox.SetToolTip( catalogToolTip );
   Catalog_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PhotometricColorCalibrationInterface::e_ItemSelected, w );

   Catalog_Sizer.SetSpacing( 4 );
   Catalog_Sizer.Add( Catalog_Label );
   Catalog_Sizer.Add( Catalog_ComboBox );
   Catalog_Sizer.AddStretch();

   const char* serverToolTip = "<p>PhotometricColorCalibration may depend on the VizieR service to access an online "
      "version of the APASS photometric catalog. This happens when local APASS databases are not available, or if the "
      "APASS database server process has not been properly configured. In such cases this option allows you to select "
      "the best VizieR mirror server, which usually is the closest one to your location.</p>"
      "<p>Note that you should always use local databases (in XPSD format) for the APASS photometric catalog required by "
      "this process, so this option should not be necessary. The APASS processes can be configured with XPSD database "
      "files implementing the APASS DR9 and DR10 catalogs. In this way the PhotometricColorCalibration tool does not "
      "require any online service, and hence does not need or use an active Internet connection.</p>";

   Server_Label.SetText( "VizieR server:" );
   Server_Label.SetFixedWidth( labelWidth1 );
   Server_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Server_Label.SetToolTip( serverToolTip );

   for ( const ServerData& server : s_vizierServers )
      Server_ComboBox.AddItem( server.name );
   Server_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PhotometricColorCalibrationInterface::e_ItemSelected, w );

   Server_Sizer.SetSpacing( 4 );
   Server_Sizer.Add( Server_Label );
   Server_Sizer.Add( Server_ComboBox );
   Server_Sizer.AddStretch();

   AutoLimitMagnitude_CheckBox.SetText( "Automatic limit magnitude" );
   AutoLimitMagnitude_CheckBox.SetToolTip( "<p>When this option is enabled, PCC will select an optimal limit magnitude "
      "for catalog search operations based on the field of view and center coordinates of the target image. Enabling this "
      "option is recommended under normal working conditions.</p>" );
   AutoLimitMagnitude_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   AutoLimitMagnitude_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AutoLimitMagnitude_Sizer.Add( AutoLimitMagnitude_CheckBox );
   AutoLimitMagnitude_Sizer.AddStretch();

   LimitMagnitude_NumericEdit.label.SetText( "Limit magnitude:" );
   LimitMagnitude_NumericEdit.label.SetFixedWidth( labelWidth1 );
   LimitMagnitude_NumericEdit.SetReal();
   LimitMagnitude_NumericEdit.SetRange( ThePCCLimitMagnitudeParameter->MinimumValue(), ThePCCLimitMagnitudeParameter->MaximumValue() );
   LimitMagnitude_NumericEdit.SetPrecision( ThePCCLimitMagnitudeParameter->Precision() );
   LimitMagnitude_NumericEdit.edit.SetFixedWidth( editWidth1 );
   LimitMagnitude_NumericEdit.sizer.AddStretch();
   LimitMagnitude_NumericEdit.SetToolTip( "<p>This parameter limits the number of stars extracted from the spectrophotometric "
      "catalog to those with magnitudes (in the visual or G band) smaller than this value. The optimal value depends on the "
      "field of view of the target image. If this value is too low, the color calibration algorithm may not have enough stars "
      "to compute robust white balance factors. Too high of a limit magnitude will slow down the process without any benefit. "
      "The <i>Automatic limit magnitude</i> option is strongly recommended instead of specifying a literal limit magnitude "
      "value under normal working conditions.</p>" );
   LimitMagnitude_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_ValueUpdated, w );

   CatalogSearch_Sizer.SetSpacing( 4 );
   CatalogSearch_Sizer.Add( AutoCatalog_Sizer );
   CatalogSearch_Sizer.Add( Catalog_Sizer );
   CatalogSearch_Sizer.Add( Server_Sizer );
   CatalogSearch_Sizer.Add( AutoLimitMagnitude_Sizer );
   CatalogSearch_Sizer.Add( LimitMagnitude_NumericEdit );

   CatalogSearch_Control.SetSizer( CatalogSearch_Sizer );

   //

   SignalEvaluation_SectionBar.SetTitle( "Signal Evaluation" );
   SignalEvaluation_SectionBar.SetToolTip( "<p>PCC detects stars in the target image and fits a point spread function model to each "
      "detected source separately in each nominal RGB channel. The fitted PSF parameters are then used to guide evaluation of the "
      "total flux of each source, and the RGB fluxes of matched stars in the selected photometric catalog are used to compute a "
      "robust and precise white balance factor based on catalog star magnitudes measured through different filters.</p>" );
   SignalEvaluation_SectionBar.SetSection( SignalEvaluation_Control );

   const char* scaleStructureLayersToolTip =
   "<p>Number of wavelet layers used for structure detection.</p>"
   "<p>With more wavelet layers, larger stars will be detected and used for signal evaluation.</p>";

   StructureLayers_Label.SetText( "Detection scales:" );
   StructureLayers_Label.SetFixedWidth( labelWidth1 );
   StructureLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StructureLayers_Label.SetToolTip( scaleStructureLayersToolTip );

   StructureLayers_SpinBox.SetRange( int( ThePCCStructureLayersParameter->MinimumValue() ), int( ThePCCStructureLayersParameter->MaximumValue() ) );
   StructureLayers_SpinBox.SetToolTip( scaleStructureLayersToolTip );
   StructureLayers_SpinBox.SetFixedWidth( editWidth1 );
   StructureLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   StructureLayers_Sizer.SetSpacing( 4 );
   StructureLayers_Sizer.Add( StructureLayers_Label );
   StructureLayers_Sizer.Add( StructureLayers_SpinBox );
   StructureLayers_Sizer.AddStretch();

   SaturationThreshold_NumericControl.label.SetText( "Saturation threshold:" );
   SaturationThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   SaturationThreshold_NumericControl.slider.SetRange( 0, 250 );
   SaturationThreshold_NumericControl.SetReal();
   SaturationThreshold_NumericControl.SetRange( ThePCCSaturationThresholdParameter->MinimumValue(), ThePCCSaturationThresholdParameter->MaximumValue() );
   SaturationThreshold_NumericControl.SetPrecision( ThePCCSaturationThresholdParameter->Precision() );
   SaturationThreshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   SaturationThreshold_NumericControl.SetToolTip( "<p>Saturation threshold in the [0,1] range.</p>"
      "<p>Detected stars with one or more pixels with values above this threshold will be excluded for signal evaluation.</p>"
      "<p>This parameter is expressed in the [0,1] range. It can be applied either as an absolute pixel sample value in the "
      "normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image (see the <i>Relative "
      "saturation threshold</i> parameter).</p>"
      "<p>The default saturation threshold is 0.75. For signal evaluation this parameter plays an imortant role to prevent "
      "contamination of the statistical sample of flux ratios by sources with saturated or nonlinear data. Changing the default "
      "value should not be necessary under normal conditions.</p>" );
   SaturationThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_ValueUpdated, w );

   SaturationRelative_CheckBox.SetText( "Relative saturation threshold" );
   SaturationRelative_CheckBox.SetToolTip( "<p>The saturation threshold parameter can be applied either as an absolute pixel "
      "sample value in the normalized [0,1] range, or as a value relative to the maximum pixel sample value of the measured image.</p>"
      "The relative saturation threshold option is enabled by default.</p>" );
   SaturationRelative_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

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

   PSFNoiseLayers_SpinBox.SetRange( int( ThePCCPSFNoiseLayersParameter->MinimumValue() ), int( ThePCCPSFNoiseLayersParameter->MaximumValue() ) );
   PSFNoiseLayers_SpinBox.SetToolTip( psfNoiseLayersToolTip );
   PSFNoiseLayers_SpinBox.SetFixedWidth( editWidth1 );
   PSFNoiseLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

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

   PSFMinStructureSize_SpinBox.SetRange( int( ThePCCPSFMinStructureSizeParameter->MinimumValue() ), int( ThePCCPSFMinStructureSizeParameter->MaximumValue() ) );
   PSFMinStructureSize_SpinBox.SetMinimumValueText( "<Auto>" );
   PSFMinStructureSize_SpinBox.SetToolTip( psfMinStructureSizeToolTip );
   PSFMinStructureSize_SpinBox.SetFixedWidth( editWidth1 );
   PSFMinStructureSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

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

   PSFHotPixelFilterRadius_SpinBox.SetRange( int( ThePCCPSFHotPixelFilterRadiusParameter->MinimumValue() ), int( ThePCCPSFHotPixelFilterRadiusParameter->MaximumValue() ) );
   PSFHotPixelFilterRadius_SpinBox.SetToolTip( psfHotPixelFilterRadiusToolTip );
   PSFHotPixelFilterRadius_SpinBox.SetFixedWidth( editWidth1 );
   PSFHotPixelFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

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

   PSFNoiseReductionFilterRadius_SpinBox.SetRange( int( ThePCCPSFNoiseReductionFilterRadiusParameter->MinimumValue() ), int( ThePCCPSFNoiseReductionFilterRadiusParameter->MaximumValue() ) );
   PSFNoiseReductionFilterRadius_SpinBox.SetToolTip( psfNoiseReductionFilterRadiusToolTip );
   PSFNoiseReductionFilterRadius_SpinBox.SetFixedWidth( editWidth1 );
   PSFNoiseReductionFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   PSFNoiseReductionFilterRadius_Sizer.SetSpacing( 4 );
   PSFNoiseReductionFilterRadius_Sizer.Add( PSFNoiseReductionFilterRadius_Label );
   PSFNoiseReductionFilterRadius_Sizer.Add( PSFNoiseReductionFilterRadius_SpinBox );
   PSFNoiseReductionFilterRadius_Sizer.AddStretch();

   PSFMinSNR_NumericEdit.label.SetText( "Minimum detection SNR:" );
   PSFMinSNR_NumericEdit.label.SetFixedWidth( labelWidth1 );
   PSFMinSNR_NumericEdit.SetReal();
   PSFMinSNR_NumericEdit.SetRange( ThePCCPSFMinSNRParameter->MinimumValue(), ThePCCPSFMinSNRParameter->MaximumValue() );
   PSFMinSNR_NumericEdit.SetPrecision( ThePCCPSFMinSNRParameter->Precision() );
   PSFMinSNR_NumericEdit.edit.SetFixedWidth( editWidth1 );
   PSFMinSNR_NumericEdit.sizer.AddStretch();
   PSFMinSNR_NumericEdit.SetToolTip( "<p>Minimum signal-to-noise ratio of a detectable star.</p>"
      "<p>Given a source with estimated brightness <i>s</i>, local background <i>b</i> and local background dispersion "
      "<i>n</i>, SNR is evaluated as (<i>s</i> &ndash; <i>b</i>)/<i>n</i>. Stars with measured SNR below the value of this "
      "parameter won't be used for signal evaluation.</p>"
      "<p>This parameter allows limiting star detection to a subset of the brightest sources in the image adaptively. By "
      "requiring relatively high SNR levels in the evaluated sources, the accuracy and robustness of the signal evaluation "
      "process can be largely improved. The default value is 40, which is quite appropriate in most cases.</p>" );
   PSFMinSNR_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_ValueUpdated, w );

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
   PSFType_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PhotometricColorCalibrationInterface::e_ItemSelected, w );

   PSFType_Sizer.SetSpacing( 4 );
   PSFType_Sizer.Add( PSFType_Label );
   PSFType_Sizer.Add( PSFType_ComboBox );
   PSFType_Sizer.AddStretch();

   PSFGrowth_NumericControl.label.SetText( "Growth factor:" );
   PSFGrowth_NumericControl.label.SetFixedWidth( labelWidth1 );
   PSFGrowth_NumericControl.slider.SetRange( 0, 250 );
   PSFGrowth_NumericControl.SetReal();
   PSFGrowth_NumericControl.SetRange( ThePCCPSFGrowthParameter->MinimumValue(), ThePCCPSFGrowthParameter->MaximumValue() );
   PSFGrowth_NumericControl.SetPrecision( ThePCCPSFGrowthParameter->Precision() );
   PSFGrowth_NumericControl.edit.SetFixedWidth( editWidth1 );
   PSFGrowth_NumericControl.SetToolTip( "<p>Growing factor for expansion/contraction of the PSF flux measurement region for "
      "each source, in units of the Full Width at Tenth Maximum (FWTM).</p>"
      "<p>The default value of this parameter is 1.0, meaning that flux is measured exclusively for pixels within the elliptical "
      "region defined at one tenth of the fitted PSF maximum. Increasing this parameter can inprove accuracy of PSF flux "
      "measurements for undersampled images, where PSF fitting uncertainty is relatively large. Decreasing it can be beneficial "
      "in some cases working with very noisy data to restrict flux evaluation to star cores.</p>" );
   PSFGrowth_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_ValueUpdated, w );

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

   PSFMaxStars_SpinBox.SetRange( int( ThePCCPSFMaxStarsParameter->MinimumValue() ), int( ThePCCPSFMaxStarsParameter->MaximumValue() ) );
   PSFMaxStars_SpinBox.SetToolTip( psfMaxStarsToolTip );
   PSFMaxStars_SpinBox.SetFixedWidth( editWidth2 );
   PSFMaxStars_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

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
   PSFAllowClusteredSources_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

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
   BackgroundReference_SectionBar.OnCheck( (SectionBar::check_event_handler)&PhotometricColorCalibrationInterface::e_SectionCheck, w );
   BackgroundReference_SectionBar.SetSection( BackgroundReference_Control );

   //

   BackgroundReferenceViewId_Label.SetText( "Reference image:" );
   BackgroundReferenceViewId_Label.SetFixedWidth( labelWidth1 );
   BackgroundReferenceViewId_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   BackgroundReferenceViewId_Edit.SetToolTip( "<p>Background reference image.</p>"
      "<p>PCC will use pixels read from this image to compute a mean background level for each color channel. "
      "If you leave this field blank (or with its default &lt;target-image&gt; value), the target image will be "
      "also the background reference image during the color calibration process.</p>"
      "<p>You should specify a view that represents the <i>true background</i> of the image. In most cases this "
      "means that you must select a view whose pixels are strongly dominated by the sky background, as it is "
      "being represented on the target image. A typical example involves defining a small preview over a free "
      "sky area of the target image, and selecting it here as the background reference image.</p>"
      "<p>You can also drag a view selector to this control, and the corresponding view will be selected.</p>" );
   BackgroundReferenceViewId_Edit.OnGetFocus( (Control::event_handler)&PhotometricColorCalibrationInterface::e_GetFocus, w );
   BackgroundReferenceViewId_Edit.OnEditCompleted( (Edit::edit_event_handler)&PhotometricColorCalibrationInterface::e_EditCompleted, w );
   BackgroundReferenceViewId_Edit.OnViewDrag( (Control::view_drag_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrag, w );
   BackgroundReferenceViewId_Edit.OnViewDrop( (Control::view_drop_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrop, w );

   BackgroundReferenceViewId_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   BackgroundReferenceViewId_ToolButton.SetScaledFixedSize( 20, 20 );
   BackgroundReferenceViewId_ToolButton.SetToolTip( "<p>Select the background reference image.</p>" );
   BackgroundReferenceViewId_ToolButton.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

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
   BackgroundLow_NumericControl.SetRange( ThePCCBackgroundLowParameter->MinimumValue(), ThePCCBackgroundLowParameter->MaximumValue() );
   BackgroundLow_NumericControl.SetPrecision( ThePCCBackgroundLowParameter->Precision() );
   BackgroundLow_NumericControl.SetToolTip( "<p>Lower bound of the set of background reference pixels, measured in sigma units "
      "from the the median of the background reference for each nominal channel. Background reference pixels with values less "
      "than or equal to this lower bound will be rejected for calculation of mean background levels.</p>"
      "<p>The default value is -2.8 sigma.</p>" );
   BackgroundLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_ValueUpdated, w );

   //

   BackgroundHigh_NumericControl.label.SetText( "Upper limit:" );
   BackgroundHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundHigh_NumericControl.slider.SetRange( -100, 100 );
   BackgroundHigh_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundHigh_NumericControl.SetReal();
   BackgroundHigh_NumericControl.EnableFixedSign();
   BackgroundHigh_NumericControl.SetRange( ThePCCBackgroundHighParameter->MinimumValue(), ThePCCBackgroundHighParameter->MaximumValue() );
   BackgroundHigh_NumericControl.SetPrecision( ThePCCBackgroundHighParameter->Precision() );
   BackgroundHigh_NumericControl.SetToolTip( "<p>Upper bound of the set of background reference pixels, measured in sigma units "
      "from the the median of the background reference for each nominal channel. Background reference pixels with values greater "
      "than or equal to this upper bound will be rejected for calculation of mean background levels.</p>"
      "<p>The default value is +2.0 sigma.</p>" );
   BackgroundHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_ValueUpdated, w );

   //

   BackgroundROI_GroupBox.SetTitle( "Region of Interest" );
   BackgroundROI_GroupBox.EnableTitleCheckBox();
   BackgroundROI_GroupBox.SetToolTip( "<p>You <i>should</i> define a rectangular region of interest (ROI) to restrict background "
      "evaluation to pixels within a region that represents the <i>true background</i> of the target image. If no ROI is defined, "
      "background evaluation will be performed on the entire target image, which can be inaccurate, especially for images with few "
      "free sky background regions.</p>" );
   BackgroundROI_GroupBox.OnCheck( (GroupBox::check_event_handler)&PhotometricColorCalibrationInterface::e_GroupCheck, w );
   BackgroundROI_GroupBox.OnViewDrag( (Control::view_drag_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrag, w );
   BackgroundROI_GroupBox.OnViewDrop( (Control::view_drop_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrop, w );

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
   BackgroundROIX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   //

   const char* backgroundROIY0ToolTip = "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>";

   BackgroundROIY0_Label.SetText( "Top:" );
   BackgroundROIY0_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIY0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIY0_Label.SetToolTip( backgroundROIY0ToolTip );

   BackgroundROIY0_SpinBox.SetRange( 0, int_max );
   BackgroundROIY0_SpinBox.SetToolTip( backgroundROIY0ToolTip );
   BackgroundROIY0_SpinBox.SetMinWidth( editWidth2 );
   BackgroundROIY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

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
   BackgroundROIWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   //

   const char* backgroundROIHeightToolTip = "<p>Height of the ROI in pixels.</p>";

   BackgroundROIHeight_Label.SetText( "Height:" );
   BackgroundROIHeight_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIHeight_Label.SetToolTip( backgroundROIHeightToolTip );

   BackgroundROIHeight_SpinBox.SetRange( 0, int_max );
   BackgroundROIHeight_SpinBox.SetToolTip( backgroundROIHeightToolTip );
   BackgroundROIHeight_SpinBox.SetMinWidth( editWidth2 );
   BackgroundROIHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   BackgroundROISelectPreview_Button.SetText( "From Preview" );
   BackgroundROISelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview.</p>"
      "<p>Or much easier, drag a preview selector to this button and the corresponding coordinates will be selected automatically.</p>" );
   BackgroundROISelectPreview_Button.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );
   BackgroundROISelectPreview_Button.OnViewDrag( (Control::view_drag_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrag, w );
   BackgroundROISelectPreview_Button.OnViewDrop( (Control::view_drop_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrop, w );

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
// EOF PhotometricColorCalibrationInterface.cpp - Released 2023-08-01T16:30:17Z
