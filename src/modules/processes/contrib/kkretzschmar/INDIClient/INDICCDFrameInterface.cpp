//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.7
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.1
// ----------------------------------------------------------------------------
// INDICCDFrameInterface.cpp - Released 2020-12-17T15:46:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2020 Klaus Kretzschmar
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

#include "INDICCDFrameInterface.h"
#include "INDICCDFrameInstance.h"
#include "INDICCDFrameParameters.h"
#include "INDICCDFrameProcess.h"
#include "INDIMountInterface.h"
#include "INDIDeviceControllerInstance.h"

#include <pcl/Console.h>
#include <pcl/Dialog.h>
#include <pcl/FileDialog.h>

namespace pcl
{

struct ServerData
{
   String name;
   String url;
};

struct CatalogData
{
   String name;
   String description;
};

static Array<ServerData>  s_vizierServers;
static Array<CatalogData> s_solverCatalogs;
static bool               s_dataInitialized = false;


static void InitializeData()
{
   if ( !s_dataInitialized )
   {
      s_vizierServers.Clear();
      s_vizierServers << ServerData{ "CDS Strasbourg, France", "http://cdsarc.u-strasbg.fr/" }
                      << ServerData{ "ADAC Tokyo, Japan", "http://vizier.nao.ac.jp/" }
                      << ServerData{ "CADC Victoria, Canada ", "http://vizier.hia.nrc.ca/" }
                      << ServerData{ "CASU Cambridge, UK", "http://vizier.ast.cam.ac.uk/" }
                      << ServerData{ "IUCAA Pune, India", "http://vizier.iucaa.ernet.in/" }
                      << ServerData{ "NAOC Beijing, China", "http://VizieR.china-vo.org/" }
                      << ServerData{ "INASAN Moscow, Russia", "http://vizier.inasan.ru/" }
                      << ServerData{ "CFA Harvard, Cambridge, USA", "http://vizier.cfa.harvard.edu/" }
                      << ServerData{ "JAC Hilo, Hawaii, USA", "http://www.ukirt.hawaii.edu/" }
                      << ServerData{ "SAAO, South Africa", "http://viziersaao.chpc.ac.za/" };

      s_solverCatalogs.Clear();
      s_solverCatalogs << CatalogData{ "GaiaEDR3_XPSD", "Gaia Early Data Release 3 - Local XPSD Server (Gaia collaboration, 2020, 1,806,254,432 sources)" }
                       << CatalogData{ "GaiaDR2_XPSD", "Gaia Data Release 2 - Local XPSD Server (Gaia collaboration, 2018, 1,692,919,135 sources)" }
                       << CatalogData{ "GaiaDR2", "Gaia Data Release 2 - Online Vizier Service (Gaia collaboration, 2018, 1,692,919,135 sources)" }
                       << CatalogData{ "PPMXL", "PPMXL catalog (910,469,430 objects)" }
                       << CatalogData{ "UCAC3", "UCAC3 catalog (100,765,502 objects)" }
                       << CatalogData{ "TYCHO-2", "Tycho-2 catalog (2,539,913 stars)" }
                       << CatalogData{ "Bright Stars", "Bright Star Catalog, 5th ed. (Hoffleit+, 9110 stars)" };

      s_dataInitialized = true;
   }
}

// ----------------------------------------------------------------------------

INDICCDFrameInterface* TheINDICCDFrameInterface = nullptr;

// ----------------------------------------------------------------------------

class SimpleGetStringDialog : public Dialog
{
public:

   SimpleGetStringDialog( const String& label, const String& text = String(), const String& title = "IndigoCCDFrame" )
   {
      Text_Label.SetText( label );
      Text_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

      Text_Edit.SetText( text.Trimmed() );
      Text_Edit.SetMinWidth( 32 * Font().Width( 'm' ) );

      Text_Sizer.SetSpacing( 6 );
      Text_Sizer.Add( Text_Label );
      Text_Sizer.Add( Text_Edit, 100 );

      OK_PushButton.SetText( "OK" );
      OK_PushButton.SetDefault();
      OK_PushButton.OnClick( (Button::click_event_handler)&SimpleGetStringDialog::e_Click, *this );

      Cancel_PushButton.SetText( "Cancel" );
      Cancel_PushButton.OnClick( (Button::click_event_handler)&SimpleGetStringDialog::e_Click, *this );

      Buttons_Sizer.SetSpacing( 8 );
      Buttons_Sizer.AddStretch();
      Buttons_Sizer.Add( OK_PushButton );
      Buttons_Sizer.Add( Cancel_PushButton );

      Global_Sizer.SetMargin( 8 );
      Global_Sizer.SetSpacing( 6 );
      Global_Sizer.Add( Text_Sizer );
      Global_Sizer.Add( Buttons_Sizer );

      SetSizer( Global_Sizer );

      SetWindowTitle( title );

      OnShow( (Control::event_handler)&SimpleGetStringDialog::e_Show, *this );
   }

   String Text() const
   {
      return Text_Edit.Text().Trimmed();
   }

private:

   VerticalSizer Global_Sizer;
   HorizontalSizer Text_Sizer;
   Label Text_Label;
   Edit Text_Edit;
   HorizontalSizer Buttons_Sizer;
   PushButton OK_PushButton;
   PushButton Cancel_PushButton;

   void e_Show( Control& )
   {
      AdjustToContents();
      SetFixedHeight();
      SetMinWidth();
   }

   void e_Click( Button& sender, bool checked )
   {
      if ( sender == OK_PushButton )
         Ok();
      else if ( sender == Cancel_PushButton )
         Cancel();
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FilterConfigDialog::FilterConfigDialog( const String& deviceName )
   : ConfigDialogBase( deviceName )
{
   FilterNames_TreeBox.SetMinHeight( 8 * Font().Height() );
   FilterNames_TreeBox.SetScaledMinWidth( 100 );
   FilterNames_TreeBox.SetNumberOfColumns( 2 );
   FilterNames_TreeBox.SetHeaderText( 0, "Filter Slot" );
   FilterNames_TreeBox.SetHeaderText( 1, "Filter Name" );
   FilterNames_TreeBox.DisableMultipleSelections();
   FilterNames_TreeBox.SetStyleSheet( ScaledStyleSheet(
      "QTreeView {"
         "font-family: Hack, DejaVu Sans Mono, Monospace;"
         "font-size: 9pt;"
      "}" ) );

   FilterRename_ToolButton.SetIcon( ScaledResource( ":/icons/write.png" ) );
   FilterRename_ToolButton.SetScaledFixedSize( 22, 22 );
   FilterRename_ToolButton.SetToolTip( "<p>Configure Indigo filter slot name</p>" );
   FilterRename_ToolButton.OnClick( (Button::click_event_handler)&FilterConfigDialog::e_Click, *this );

   FilterToolBox_Sizer.SetSpacing( 8 );
   FilterToolBox_Sizer.SetMargin( 8 );
   FilterToolBox_Sizer.Add( FilterRename_ToolButton );
   FilterToolBox_Sizer.AddStretch();

   FilterConfig_Sizer.Add( FilterNames_TreeBox );
   FilterConfig_Sizer.Add( FilterToolBox_Sizer );
   FilterConfig_Sizer.AddStretch();

   Global_Sizer.Add( FilterConfig_Sizer );

   AddBaseControls();
}

// ----------------------------------------------------------------------------

void FilterConfigDialog::SendUpdatedProperties()
{
   for ( int i = 0, n = FilterNames_TreeBox.NumberOfChildren(); i < n; ++i )
      INDIClient::TheClient()->SendNewPropertyItem( m_device,
         WHEEL_SLOT_NAME_PROPERTY_NAME,
         "INDI_TEXT",
         "SLOT_NAME_" + String( i + 1 ), FilterNames_TreeBox.Child( i )->Text( 1 ) );
}

// ----------------------------------------------------------------------------

void FilterConfigDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == FilterRename_ToolButton )
   {
      TreeBox::Node* node = FilterNames_TreeBox.CurrentNode();
      SimpleGetStringDialog dialog( "New Filter Name:", node->Text( 1 ) );
      if ( dialog.Execute() )
         node->SetText( 1, dialog.Text() );
   }
}

// ----------------------------------------------------------------------------

void FilterConfigDialog::addFilterName( size_t filterSlot, const String& filterName )
{
   TreeBox::Node* node = new TreeBox::Node();
   node->SetText( 0, String( filterSlot ) );
   node->SetText( 1, filterName );
   FilterNames_TreeBox.Add( node );
}

// ----------------------------------------------------------------------------

void FilterConfigDialog::adjustTreeColumns()
{
   for ( int i = 0, n = FilterNames_TreeBox.NumberOfColumns(); i < n; ++i )
      FilterNames_TreeBox.AdjustColumnWidthToContents( i );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

INDICCDFrameInterface::INDICCDFrameInterface()
{
   TheINDICCDFrameInterface = this;
}

// ----------------------------------------------------------------------------

INDICCDFrameInterface::~INDICCDFrameInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString INDICCDFrameInterface::Id() const
{
   return "IndigoCCDFrame";
}

// ----------------------------------------------------------------------------

MetaProcess* INDICCDFrameInterface::Process() const
{
   return TheINDICCDFrameProcess;
}

// ----------------------------------------------------------------------------

String INDICCDFrameInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/IndigoCCDFrame.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures INDICCDFrameInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

// ----------------------------------------------------------------------------

void INDICCDFrameInterface::ResetInstance()
{
   INDICCDFrameInstance defaultInstance( TheINDICCDFrameProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool INDICCDFrameInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Indigo CCD Controller" );
      ResetInstance();
      UpdateControls();
   }

   dynamic = false;
   return &P == TheINDICCDFrameProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* INDICCDFrameInterface::NewProcess() const
{
   INDICCDFrameInstance* instance = new INDICCDFrameInstance( TheINDICCDFrameProcess );
   instance->p_deviceName = m_device;
   instance->p_uploadMode = GUI->UploadMode_Combo.CurrentItem();
   instance->p_serverUploadDirectory = GUI->ServerUploadDir_Edit.Text().Trimmed();
   instance->p_serverFileNameTemplate = GUI->ServerFileNameTemplate_Edit.Text().Trimmed();
   instance->p_frameType = GUI->CCDFrameType_Combo.CurrentItem();
   instance->p_binningX = GUI->CCDBinX_Combo.CurrentItem() + 1;
   instance->p_binningY = GUI->CCDBinY_Combo.CurrentItem() + 1;
   instance->p_filterSlot = GUI->CCDFilter_Combo.CurrentItem() + 1;
   instance->p_exposureTime = GUI->ExposureTime_NumericEdit.Value();
   instance->p_exposureDelay = GUI->ExposureDelay_NumericEdit.Value();
   instance->p_exposureCount = GUI->ExposureCount_SpinBox.Value();
   instance->p_openClientImages = GUI->OpenClientFrames_CheckBox.IsChecked();
   //instance->p_newImageIdTemplate = ; // ### TODO
   instance->p_reuseImageWindow = GUI->ReuseImageWindow_CheckBox.IsChecked();
   instance->p_autoStretch = GUI->AutoStretch_CheckBox.IsChecked();
   instance->p_linkedAutoStretch = GUI->LinkedAutoStretch_CheckBox.IsChecked();
   instance->p_saveClientImages = GUI->SaveClientFrames_CheckBox.IsChecked();
   instance->p_overwriteClientImages = GUI->OverwriteClientFrames_CheckBox.IsChecked();
   instance->p_clientDownloadDirectory = GUI->ClientDownloadDir_Edit.Text().Trimmed();
   instance->p_clientFileNameTemplate = GUI->ClientFileNameTemplate_Edit.Text().Trimmed();
   instance->p_clientOutputFormatHints = GUI->ClientOutputFormatHints_Edit.Text().Trimmed();
   instance->p_objectName = GUI->ObjectName_Edit.Text().Trimmed();
   instance->p_telescopeSelection = GUI->TelescopeDevice_Combo.CurrentItem();
   instance->p_extFilterWheelDeviceName = GUI->ExternalFilterDevice_Combo.ItemText( GUI->ExternalFilterDevice_Combo.CurrentItem() );
   instance->p_applyPlateSolver = GUI->PlateSolving_CheckBox.IsChecked();
   instance->p_telescopeFocalLength = TheINDIMountInterface != nullptr ? TheINDIMountInterface->TelescopeFocalLength() : 0;
   instance->p_centerTarget = GUI->CenterTarget_CheckBox.IsChecked();
   instance->p_solverAutoCatalog = GUI->SolverAutoCatalog_CheckBox.IsChecked();
   instance->p_solverAutoLimitMagnitude = GUI->SolverAutoLimitMagnitude_CheckBox.IsChecked();
   instance->p_solverCatalogName = s_solverCatalogs[GUI->SolverCatalog_ComboBox.CurrentItem()].name;
   instance->p_solverDistortionCorrection = GUI->DistortionCorrection_CheckBox.IsChecked();
   instance->p_solverNoiseLayers = GUI->NoiseReduction_SpinBox.Value();
   instance->p_solverProjection = GUI->ProjectionSystem_ComboBox.CurrentItem();
   instance->p_solverSplineSmoothing = GUI->SplineSmoothing_NumericControl.Value();
   instance->p_solverStarSensitivity = GUI->StarSensitivity_NumericControl.Value();
   return instance;
}

// ----------------------------------------------------------------------------

bool INDICCDFrameInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const INDICCDFrameInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an IndigoCCDFrame instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool INDICCDFrameInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool INDICCDFrameInterface::ImportProcess( const ProcessImplementation& p )
{
   const INDICCDFrameInstance* instance = dynamic_cast<const INDICCDFrameInstance*>( &p );
   if ( instance != nullptr )
   {
      GUI->ServerFileNameTemplate_Edit.SetText( instance->p_serverFileNameTemplate );
      GUI->ExposureTime_NumericEdit.SetValue( instance->p_exposureTime );
      GUI->ExposureDelay_NumericEdit.SetValue( instance->p_exposureDelay );
      GUI->ExposureCount_SpinBox.SetValue( instance->p_exposureCount );
      GUI->ObjectName_Edit.SetText( instance->p_objectName );
      GUI->TelescopeDevice_Combo.SetCurrentItem( Max( instance->p_telescopeSelection, GUI->TelescopeDevice_Combo.NumberOfItems() - 1 ) );
      GUI->OpenClientFrames_CheckBox.SetChecked( instance->p_openClientImages );
      GUI->ReuseImageWindow_CheckBox.SetChecked( instance->p_reuseImageWindow );
      GUI->AutoStretch_CheckBox.SetChecked( instance->p_autoStretch );
      GUI->LinkedAutoStretch_CheckBox.SetChecked( instance->p_linkedAutoStretch );
      GUI->SaveClientFrames_CheckBox.SetChecked( instance->p_saveClientImages );
      GUI->OverwriteClientFrames_CheckBox.SetChecked( instance->p_overwriteClientImages );
      GUI->ClientDownloadDir_Edit.SetText( instance->p_clientDownloadDirectory );
      GUI->ClientFileNameTemplate_Edit.SetText( instance->p_clientFileNameTemplate );
      GUI->ClientOutputFormatHints_Edit.SetText( instance->p_clientOutputFormatHints );
      GUI->PlateSolving_CheckBox.SetChecked(instance->p_applyPlateSolver);
      GUI->CenterTarget_CheckBox.SetChecked(instance->p_centerTarget);
      GUI->SolverAutoCatalog_CheckBox.SetChecked(instance->p_solverAutoCatalog);
      GUI->SolverAutoLimitMagnitude_CheckBox.SetChecked(instance->p_solverAutoLimitMagnitude);
      //instance->p_solverCatalogName = s_solverCatalogs[GUI->SolverCatalog_ComboBox.CurrentItem()].name;
      GUI->DistortionCorrection_CheckBox.SetChecked(instance->p_solverDistortionCorrection);
      GUI->NoiseReduction_SpinBox.SetValue(instance->p_solverNoiseLayers);
      GUI->ProjectionSystem_ComboBox.SetCurrentItem(instance->p_solverProjection);
      GUI->SplineSmoothing_NumericControl.SetValue(instance->p_solverSplineSmoothing);
      GUI->StarSensitivity_NumericControl.SetValue(instance->p_solverStarSensitivity);

      if ( instance->ValidateDevice( false /*throwErrors*/ ) )
      {
         m_device = instance->p_deviceName;
         instance->SendDeviceProperties();
      }
      else
         m_device.Clear();

      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

INDICCDFrameInterface::GUIData::GUIData( INDICCDFrameInterface& w )
{
   InitializeData();

   int emWidth = w.Font().Width( 'm' );
   int labelWidth1 = w.Font().Width( "Server file name template:" ) + emWidth;
   int editWidth1 = 5 * emWidth;
   int editWidth2 = 8 * emWidth;
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   ServerParameters_SectionBar.SetTitle( "Device Properties" );
   ServerParameters_SectionBar.SetSection( ServerParameters_Control );

   CCDDevice_Label.SetText( "Indigo CCD device:" );
   CCDDevice_Label.SetToolTip( "<p>Select an Indigo CCD device.</p>" );
   CCDDevice_Label.SetMinWidth( labelWidth1 );
   CCDDevice_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   FilterConfig_ToolButton.SetIcon( w.ScaledResource( ":/icons/wrench.png" ) );
   FilterConfig_ToolButton.SetScaledFixedSize( 22, 22 );
   FilterConfig_ToolButton.SetToolTip( "<p>Configure Indigo filter wheel device</p>" );
   FilterConfig_ToolButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );
   FilterConfig_ToolButton.Disable();

   CCDDevice_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );

   CCDDevice_Sizer.SetSpacing( 4 );
   CCDDevice_Sizer.Add( CCDDevice_Label );
   CCDDevice_Sizer.Add( CCDDevice_Combo, 100 );

   CCDTemp_NumericEdit.SetReal();
   CCDTemp_NumericEdit.SetPrecision( 2 );
   CCDTemp_NumericEdit.SetRange( -50, +50 );
   CCDTemp_NumericEdit.label.SetText( "Temperature:" );
   CCDTemp_NumericEdit.label.SetFixedWidth( labelWidth1 );
   CCDTemp_NumericEdit.edit.SetFixedWidth( editWidth1 );
   CCDTemp_NumericEdit.edit.SetReadOnly();
   CCDTemp_NumericEdit.sizer.AddStretch();
   CCDTemp_NumericEdit.SetToolTip( "<p>Current chip temperature in degrees Celsius.</p>" );

   CCDTargetTemp_NumericEdit.SetReal();
   CCDTargetTemp_NumericEdit.SetPrecision( 2 );
   CCDTargetTemp_NumericEdit.SetRange( -30, +25 );
   CCDTargetTemp_NumericEdit.edit.SetFixedWidth( editWidth1 );
   CCDTargetTemp_NumericEdit.slider.SetScaledMinWidth( 200 );
   CCDTargetTemp_NumericEdit.slider.SetRange( 0, 110 );
   CCDTargetTemp_NumericEdit.SetToolTip( "<p>Target chip temperature in degrees Celsius.</p>" );
   CCDTargetTemp_NumericEdit.Disable();

   CCDTargetTemp_ToolButton.SetIcon( w.ScaledResource( ":/icons/upload.png" ) );
   CCDTargetTemp_ToolButton.SetScaledFixedSize( 22, 22 );
   CCDTargetTemp_ToolButton.SetToolTip( "<p>Send a target chip temperature request.</p>" );
   CCDTargetTemp_ToolButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );

   CCDTemp_HSizer.SetSpacing( 4 );
   CCDTemp_HSizer.Add( CCDTemp_NumericEdit );
   CCDTemp_HSizer.Add( CCDTargetTemp_NumericEdit, 100 );
   CCDTemp_HSizer.AddSpacing( 4 );
   CCDTemp_HSizer.Add( CCDTargetTemp_ToolButton );

   const char* ccdBinXToolTipText =
      "<p>Horizontal (X-axis) pixel binning factor.</p>";

   CCDBinX_Label.SetText( "Binning X:" );
   CCDBinX_Label.SetFixedWidth( labelWidth1 );
   CCDBinX_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   CCDBinX_Label.SetToolTip( ccdBinXToolTipText );
   CCDBinX_Label.Disable();

   CCDBinX_Combo.AddItem( "1" );
   CCDBinX_Combo.AddItem( "2" );
   CCDBinX_Combo.AddItem( "3" );
   CCDBinX_Combo.AddItem( "4" );
   CCDBinX_Combo.AddItem( "5" );
   CCDBinX_Combo.AddItem( "6" );
   CCDBinX_Combo.AddItem( "7" );
   CCDBinX_Combo.AddItem( "8" );
   CCDBinX_Combo.SetFixedWidth( editWidth1 );
   CCDBinX_Combo.SetToolTip( ccdBinXToolTipText );
   //CCDBinX_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   CCDBinX_Combo.Disable();

   CCDBinX_HSizer.SetSpacing( 4 );
   CCDBinX_HSizer.Add( CCDBinX_Label );
   CCDBinX_HSizer.Add( CCDBinX_Combo );
   CCDBinX_HSizer.AddStretch();

   const char* ccdBinYToolTipText =
      "<p>Vertical (Y-axis) pixel binning factor.</p>";

   CCDBinY_Label.SetText( "Binning Y:" );
   CCDBinY_Label.SetFixedWidth( labelWidth1 );
   CCDBinY_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   CCDBinY_Label.SetToolTip( ccdBinYToolTipText );
   CCDBinY_Label.Disable();

   CCDBinY_Combo.AddItem( "1" );
   CCDBinY_Combo.AddItem( "2" );
   CCDBinY_Combo.AddItem( "3" );
   CCDBinY_Combo.AddItem( "4" );
   CCDBinY_Combo.AddItem( "5" );
   CCDBinY_Combo.AddItem( "6" );
   CCDBinY_Combo.AddItem( "7" );
   CCDBinY_Combo.AddItem( "8" );
   CCDBinY_Combo.SetFixedWidth( editWidth1 );
   CCDBinY_Combo.SetToolTip( ccdBinYToolTipText );
   CCDBinY_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   CCDBinY_Combo.Disable();

   CCDBinY_HSizer.SetSpacing( 4 );
   CCDBinY_HSizer.Add( CCDBinY_Label );
   CCDBinY_HSizer.Add( CCDBinY_Combo );
   CCDBinY_HSizer.AddStretch();

   const char* ccdFilterToolTipText =
      "<p>Filter for frame acquisition, if available. "
      "The selected filter name will be stored as a standard property and FITS header keyword in each acquired frame.</p>";

   CCDFilter_Label.SetText( "Filter:" );
   CCDFilter_Label.SetFixedWidth( labelWidth1 );
   CCDFilter_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   CCDFilter_Label.SetToolTip( ccdFilterToolTipText );
   CCDFilter_Label.Disable();

   CCDFilter_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   CCDFilter_Combo.SetToolTip( ccdFilterToolTipText );
   CCDFilter_Combo.Disable();

   CCDFilter_HSizer.SetSpacing( 4 );
   CCDFilter_HSizer.Add( CCDFilter_Label );
   CCDFilter_HSizer.Add( CCDFilter_Combo, 100 );
   CCDFilter_HSizer.Add( FilterConfig_ToolButton );

   const char* ccdFrameTypeToolTipText =
      "<p>The frame type will be stored as a standard property and FITS header keyword in each acquired frame.</p>";

   CCDFrameType_Label.SetText( "Frame type:" );
   CCDFrameType_Label.SetFixedWidth( labelWidth1 );
   CCDFrameType_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   CCDFrameType_Label.SetToolTip( ccdFrameTypeToolTipText );
   CCDFrameType_Label.Disable();

   CCDFrameType_Combo.AddItem( "Light frame" );
   CCDFrameType_Combo.AddItem( "Bias frame" );
   CCDFrameType_Combo.AddItem( "Dark frame" );
   CCDFrameType_Combo.AddItem( "Flat frame" );
   CCDFrameType_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   CCDFrameType_Combo.SetToolTip( ccdFrameTypeToolTipText );
   CCDFrameType_Combo.Disable();

   CCDFrameType_HSizer.SetSpacing( 4 );
   CCDFrameType_HSizer.Add( CCDFrameType_Label );
   CCDFrameType_HSizer.Add( CCDFrameType_Combo );
   CCDFrameType_HSizer.AddStretch();

   const char* uploadModeToolTipText =
      "<p>Upload to client: The frame will be uploaded to the client and displayed on a new image window. "
      "The frame will not be stored on the client's file system.</p>"
      "<p>Upload to server: The frame will be stored on the file system of the Indigo server. "
      "The server upload directory and file name template can be specified. "
      "The frame will not be uploaded to the client.</p>"
      "<p>Upload both: The frame will be stored on the server's file system, then uploaded to the client and displayed.</p>";

   UploadMode_Label.SetText( "Upload mode:" );
   UploadMode_Label.SetToolTip( uploadModeToolTipText );
   UploadMode_Label.SetMinWidth( labelWidth1 );
   UploadMode_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   UploadMode_Label.Disable();

   UploadMode_Combo.AddItem( "Upload to client only" );
   UploadMode_Combo.AddItem( "Upload to Indigo server only" );
   UploadMode_Combo.AddItem( "Upload both: Client and server" );
   UploadMode_Combo.AdjustToContents();
   UploadMode_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   UploadMode_Combo.SetToolTip( uploadModeToolTipText );
   UploadMode_Combo.Disable();

   UploadMode_HSizer.SetSpacing( 4 );
   UploadMode_HSizer.Add( UploadMode_Label );
   UploadMode_HSizer.Add( UploadMode_Combo );
   UploadMode_HSizer.AddStretch();

   const char* uploadDirToolTipText =
      "<p>The directory where newly acquired CCD frames will be stored on the Indigo server.</p>";

   ServerUploadDir_Label.SetText( "Server upload directory:" );
   ServerUploadDir_Label.SetToolTip( uploadDirToolTipText );
   ServerUploadDir_Label.SetMinWidth( labelWidth1 );
   ServerUploadDir_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   ServerUploadDir_Label.Disable();

   ServerUploadDir_Edit.SetReadOnly();
   ServerUploadDir_Edit.SetToolTip( uploadDirToolTipText );

   ServerUploadDir_ToolButton.SetIcon( w.ScaledResource( ":/icons/network-folder.png" ) );
   ServerUploadDir_ToolButton.SetScaledFixedSize( 22, 22 );
   ServerUploadDir_ToolButton.SetToolTip( "<p>Set the server upload directory.</p>" );
   ServerUploadDir_ToolButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );

   ServerUploadDir_HSizer.SetSpacing( 4 );
   ServerUploadDir_HSizer.Add( ServerUploadDir_Label );
   ServerUploadDir_HSizer.Add( ServerUploadDir_Edit, 100 );
   ServerUploadDir_HSizer.AddSpacing( 4 );
   ServerUploadDir_HSizer.Add( ServerUploadDir_ToolButton );

   const char* fileNameTemplateToolTipText =
      "<p>A file name template can be any valid text suitable to specify file names on the target filesystem, and may include "
      "one or more <i>template specifiers</i>. Template specifiers are replaced automatically with selected tokens when "
      "new frames are acquired. Supported template specifiers are the following:</p>"
      "<p><table border=\"1\" cellspacing=\"1\" cellpadding=\"4\">"
      "<tr>"
      "<td><i>Template specifier</i></td>"
      "<td width=\"90%\"><i>Will be replaced by</i></td>"
      "</tr>"
      "<tr>"
      "<td>%f</td>"
      "<td>Frame type (light, flat, dark, bias).</td>"
      "</tr>"
      "<tr>"
      "<td>%b</td>"
      "<td>CCD binning with the format HxV, where H and V are, respectively, the horizontal and vertical binning factors.</td>"
      "</tr>"
      "<tr>"
      "<td>%e</td>"
      "<td>Exposure time in seconds.</td>"
      "</tr>"
      "<tr>"
      "<td>%F</td>"
      "<td>Filter name</td>"
      "</tr>"
      "<tr>"
      "<td>%T</td>"
      "<td>CCD temperature in degrees Celsius.</td>"
      "</tr>"
      "<tr>"
      "<td>%t</td>"
      "<td>Acquisition date and time in the UTC time scale, ISO 8601 format.</td>"
      "</tr>"
      "<tr>"
      "<td>%d</td>"
      "<td>Acquisition date in the UTC time scale, yyyy-mm-dd format.</td>"
      "</tr>"
      "<tr>"
      "<td>%n</td>"
      "<td>The frame number starting from one, with three digits and left-padded with zeros.</td>"
      "</tr>"
      "<tr>"
      "<td>%u</td>"
      "<td>A universally unique identifier (UUID) in canonical form (36 characters).</td>"
      "</tr>"
      "</table></p>"
      "<p>For example, the default template %f_B%b_E%e_%n would produce the following file name:</p>"
      "<p>LIGHT_B2x2_E300.00_002.fits</p>"
      "<p>for the second light frame of a series with exposure time of 300 seconds at binning 2x2.</p>";

   const String serverFileNameTemplateToolTipText =
      String( "<p>A template to build the file names of newly acquired frames stored on the Indigo server.</p>" )
      + fileNameTemplateToolTipText;

   ServerFileNameTemplate_Label.SetText( "Server file name template:" );
   ServerFileNameTemplate_Label.SetToolTip( serverFileNameTemplateToolTipText );
   ServerFileNameTemplate_Label.SetMinWidth( labelWidth1 );
   ServerFileNameTemplate_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   ServerFileNameTemplate_Label.Disable();

   ServerFileNameTemplate_Edit.SetToolTip( serverFileNameTemplateToolTipText );
   ServerFileNameTemplate_Edit.SetText( TheICFServerFileNameTemplateParameter->DefaultValue() );
   ServerFileNameTemplate_Edit.Disable();

   ServerFileNameTemplate_HSizer.SetSpacing( 4 );
   ServerFileNameTemplate_HSizer.Add( ServerFileNameTemplate_Label );
   ServerFileNameTemplate_HSizer.Add( ServerFileNameTemplate_Edit, 100 );

   CCDProperties_Sizer.SetSpacing( 4 );
   CCDProperties_Sizer.Add( CCDTemp_HSizer );
   CCDProperties_Sizer.Add( CCDBinX_HSizer );
   CCDProperties_Sizer.Add( CCDBinY_HSizer );
   CCDProperties_Sizer.Add( CCDFilter_HSizer );
   CCDProperties_Sizer.Add( CCDFrameType_HSizer );
   CCDProperties_Sizer.Add( UploadMode_HSizer );
   CCDProperties_Sizer.Add( ServerUploadDir_HSizer );
   CCDProperties_Sizer.Add( ServerFileNameTemplate_HSizer );

   CCDProperties_Control.SetSizer( CCDProperties_Sizer );

   ServerParameters_Sizer.SetSpacing( 4 );
   ServerParameters_Sizer.Add( CCDDevice_Sizer );
   ServerParameters_Sizer.Add( CCDProperties_Control );

   ServerParameters_Control.SetSizer( ServerParameters_Sizer );

   //

   ClientParameters_SectionBar.SetTitle( "Client Frames" );
   ClientParameters_SectionBar.SetSection( ClientParameters_Control );

   OpenClientFrames_CheckBox.SetText( "Open client frames" );
   OpenClientFrames_CheckBox.SetToolTip( "<p>Load newly acquired frames as image windows.</p>" );

   OpenClientFrames_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OpenClientFrames_Sizer.Add( OpenClientFrames_CheckBox );
   OpenClientFrames_Sizer.AddStretch();

   ReuseImageWindow_CheckBox.SetText( "Reuse image window" );
   ReuseImageWindow_CheckBox.SetToolTip( "<p>Load newly acquired client frames on the same image window, if available.</p>" );
   ReuseImageWindow_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ReuseImageWindow_Sizer.Add( ReuseImageWindow_CheckBox );
   ReuseImageWindow_Sizer.AddStretch();

   AutoStretch_CheckBox.SetText( "AutoStretch" );
   AutoStretch_CheckBox.SetToolTip( "<p>Compute and apply adaptive screen transfer functions (STF) for newly acquired client frames.</p>" );
   AutoStretch_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AutoStretch_Sizer.Add( AutoStretch_CheckBox );
   AutoStretch_Sizer.AddStretch();

   LinkedAutoStretch_CheckBox.SetText( "Linked AutoStretch" );
   LinkedAutoStretch_CheckBox.SetToolTip( "<p>If enabled, compute and apply a single adaptive STF for all nominal channels of "
                                          "each acquired color image.</p>"
                                          "<p>If disabled, compute a separate adaptive STF for each nominal color channel.</p>" );
   LinkedAutoStretch_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   LinkedAutoStretch_Sizer.Add( LinkedAutoStretch_CheckBox );
   LinkedAutoStretch_Sizer.AddStretch();

   PlateSolving_CheckBox.SetText( "Apply astrometric solution" );
   PlateSolving_CheckBox.Uncheck();
   PlateSolving_CheckBox.SetToolTip( "<p>Apply astrometic solution to newly acquired frames.</p>" );

   PlateSolving_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PlateSolving_Sizer.Add( PlateSolving_CheckBox );
   PlateSolving_Sizer.AddStretch();

   CenterTarget_CheckBox.SetText( "Center target" );
   CenterTarget_CheckBox.Uncheck();
   CenterTarget_CheckBox.SetToolTip( "<p>If enabled, compute the distance of the target coordinates from the actual image center "
                                      "and apply a correction to compensate this deviation.</p>"
                                      "<p>Requires an astrometric solution of the active image view. </p>" );

   CenterTarget_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CenterTarget_Sizer.Add( CenterTarget_CheckBox );
   CenterTarget_Sizer.AddStretch();

   SaveClientFrames_CheckBox.SetText( "Save client frames" );
   SaveClientFrames_CheckBox.SetToolTip( "<p>Save newly acquired frames to local image files in XISF format.</p>" );

   SaveClientFrames_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SaveClientFrames_Sizer.Add( SaveClientFrames_CheckBox );
   SaveClientFrames_Sizer.AddStretch();

   OverwriteClientFrames_CheckBox.SetText( "Overwrite existing files" );
   OverwriteClientFrames_CheckBox.SetToolTip( "<p>If this option is selected, IndigoCCDFrame will overwrite "
                                              "existing files with the same names as generated output files. This can be dangerous because the original "
                                              "contents of overwritten files will be lost.</p>"
                                              "<p><b>Enable this option <u>at your own risk.</u></b></p>" );

   OverwriteClientFrames_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OverwriteClientFrames_Sizer.Add( OverwriteClientFrames_CheckBox );
   OverwriteClientFrames_Sizer.AddStretch();

   const char* downloadDirToolTipText =
      "<p>The directory where newly acquired frames will be stored on the local filesystem.</p>"
      "<p>If you leave this parameter empty, new files will be created on the current downloads directory, as defined by global settings.</p>";

   ClientDownloadDir_Label.SetText( "Client download directory:" );
   ClientDownloadDir_Label.SetToolTip( downloadDirToolTipText );
   ClientDownloadDir_Label.SetMinWidth( labelWidth1 );
   ClientDownloadDir_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   ClientDownloadDir_Label.Disable();

   ClientDownloadDir_Edit.SetToolTip( downloadDirToolTipText );
   ClientDownloadDir_Edit.OnFileDrag( (Control::file_drag_event_handler)&INDICCDFrameInterface::e_FileDrag, w );
   ClientDownloadDir_Edit.OnFileDrop( (Control::file_drop_event_handler)&INDICCDFrameInterface::e_FileDrop, w );

   ClientDownloadDir_ToolButton.SetIcon( w.ScaledResource( ":/icons/select-file.png" ) );
   ClientDownloadDir_ToolButton.SetScaledFixedSize( 22, 22 );
   ClientDownloadDir_ToolButton.SetToolTip( "<p>Select the client download directory</p>" );
   ClientDownloadDir_ToolButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );

   ClientDownloadDir_HSizer.SetSpacing( 4 );
   ClientDownloadDir_HSizer.Add( ClientDownloadDir_Label );
   ClientDownloadDir_HSizer.Add( ClientDownloadDir_Edit, 100 );
   ClientDownloadDir_HSizer.AddSpacing( 4 );
   ClientDownloadDir_HSizer.Add( ClientDownloadDir_ToolButton );

   const String clientFileNameTemplateToolTipText =
      String( "<p>A template to build the file names of newly acquired frames stored on the Indigo client.</p>" )
      + fileNameTemplateToolTipText;

   ClientFileNameTemplate_Label.SetText( "Client file name template:" );
   ClientFileNameTemplate_Label.SetToolTip( clientFileNameTemplateToolTipText );
   ClientFileNameTemplate_Label.SetMinWidth( labelWidth1 );
   ClientFileNameTemplate_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   ClientFileNameTemplate_Label.Disable();

   ClientFileNameTemplate_Edit.SetToolTip( clientFileNameTemplateToolTipText );
   ClientFileNameTemplate_Edit.SetText( TheICFClientFileNameTemplateParameter->DefaultValue() );
   ClientFileNameTemplate_Edit.Disable();

   ClientFileNameTemplate_HSizer.SetSpacing( 4 );
   ClientFileNameTemplate_HSizer.Add( ClientFileNameTemplate_Label );
   ClientFileNameTemplate_HSizer.Add( ClientFileNameTemplate_Edit, 100 );

   const char* clientOutputFormatHintsToolTipText =
      "<p><i>Format hints</i> allow you to override global file format settings for image files used by specific processes. "
      "In IndigoCCDFrame, output hints allow you to control the way newly acquired image files are generated on the Indigo client.</p>"
      "<p>For example, you can use the \"compression-codec zlib\" hint to force the XISF format support module to compress "
      "images using the Zlib data compression algorithm. To gain more control on compression, you can use the \"compression-level <i>n</i>\""
      "hint to specify a compression level <i>n</i> in the range from 0 (default compression) to 100 (maximum compression). See the XISF "
      "format documentation for detailed information on supported XISF format hints.</p>";

   ClientOutputFormatHints_Label.SetText( "Output format hints:" );
   ClientOutputFormatHints_Label.SetToolTip( clientOutputFormatHintsToolTipText );
   ClientOutputFormatHints_Label.SetMinWidth( labelWidth1 );
   ClientOutputFormatHints_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   ClientOutputFormatHints_Label.Disable();

   ClientOutputFormatHints_Edit.SetToolTip( clientOutputFormatHintsToolTipText );
   ClientOutputFormatHints_Edit.SetText( TheICFClientOutputFormatHintsParameter->DefaultValue() );
   ClientOutputFormatHints_Edit.Disable();

   ClientOutputFormatHints_HSizer.SetSpacing( 4 );
   ClientOutputFormatHints_HSizer.Add( ClientOutputFormatHints_Label );
   ClientOutputFormatHints_HSizer.Add( ClientOutputFormatHints_Edit, 100 );

   ClientParameters_Sizer.SetSpacing( 4 );
   ClientParameters_Sizer.Add( OpenClientFrames_Sizer );
   ClientParameters_Sizer.Add( ReuseImageWindow_Sizer );
   ClientParameters_Sizer.Add( AutoStretch_Sizer );
   ClientParameters_Sizer.Add( LinkedAutoStretch_Sizer );
   ClientParameters_Sizer.Add( PlateSolving_Sizer );
   ClientParameters_Sizer.Add( CenterTarget_Sizer ) ;
   ClientParameters_Sizer.Add( SaveClientFrames_Sizer );
   ClientParameters_Sizer.Add( OverwriteClientFrames_Sizer );
   ClientParameters_Sizer.Add( ClientDownloadDir_HSizer );
   ClientParameters_Sizer.Add( ClientFileNameTemplate_HSizer );
   ClientParameters_Sizer.Add( ClientOutputFormatHints_HSizer );

   ClientParameters_Control.SetSizer( ClientParameters_Sizer );

   //

   FrameAcquisition_SectionBar.SetTitle( "Frame Acquisition" );
   FrameAcquisition_SectionBar.SetSection( FrameAcquisition_Control );

   ExposureTime_NumericEdit.SetReal();
   ExposureTime_NumericEdit.SetPrecision( 3 );
   ExposureTime_NumericEdit.SetRange( 0.001, 60000 ); // 1000 minutes = 16.666... hours
   ExposureTime_NumericEdit.label.SetText( "Exposure time:" );
   ExposureTime_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ExposureTime_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ExposureTime_NumericEdit.sizer.AddStretch();
   ExposureTime_NumericEdit.SetValue( 1 );
   ExposureTime_NumericEdit.SetToolTip( "<p>Exposure time in seconds.</p>" );

   ExposureDelay_NumericEdit.SetReal();
   ExposureDelay_NumericEdit.SetPrecision( 3 );
   ExposureDelay_NumericEdit.SetRange( 0, 600 ); // 10 minutes
   ExposureDelay_NumericEdit.label.SetText( "Exposure delay:" );
   ExposureDelay_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ExposureDelay_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ExposureDelay_NumericEdit.sizer.AddStretch();
   ExposureDelay_NumericEdit.SetValue( 0 );
   ExposureDelay_NumericEdit.SetToolTip( "<p>Waiting time between exposures in seconds.</p>" );

   ExposureCount_Label.SetText( "Number of frames:" );
   ExposureCount_Label.SetToolTip( "<p>Number of frames to be acquired.</p>" );
   ExposureCount_Label.SetMinWidth( labelWidth1 );
   ExposureCount_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   ExposureCount_SpinBox.SetRange( 1, 1000 );
   ExposureCount_SpinBox.SetFixedWidth( editWidth2 );
   ExposureCount_SpinBox.SetValue( 1 );

   ExposureCount_Sizer.SetSpacing( 4 );
   ExposureCount_Sizer.Add( ExposureCount_Label );
   ExposureCount_Sizer.Add( ExposureCount_SpinBox );
   ExposureCount_Sizer.AddStretch();

   ExposureParametersLeft_Sizer.SetSpacing( 4 );
   ExposureParametersLeft_Sizer.Add( ExposureTime_NumericEdit );
   ExposureParametersLeft_Sizer.Add( ExposureDelay_NumericEdit );
   ExposureParametersLeft_Sizer.Add( ExposureCount_Sizer );

   StartExposure_PushButton.SetText( "Start" );
   StartExposure_PushButton.SetIcon( w.ScaledResource( ":/icons/play.png" ) );
   StartExposure_PushButton.SetStyleSheet( "QPushButton { text-align: left; }" );
   StartExposure_PushButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );

   StartExposure_Sizer.Add( StartExposure_PushButton );
   StartExposure_Sizer.AddStretch();

   CancelExposure_PushButton.SetText( "Cancel" );
   CancelExposure_PushButton.SetIcon( w.ScaledResource( ":/icons/stop.png" ) );
   CancelExposure_PushButton.SetStyleSheet( "QPushButton { text-align: left; }" );
   CancelExposure_PushButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );
   CancelExposure_PushButton.Disable();

   CancelExposure_Sizer.Add( CancelExposure_PushButton );
   CancelExposure_Sizer.AddStretch();

   ExposureParametersRight_Sizer.SetSpacing( 4 );
   ExposureParametersRight_Sizer.Add( StartExposure_Sizer );
   ExposureParametersRight_Sizer.Add( CancelExposure_Sizer );
   ExposureParametersRight_Sizer.Add( ExposureInfo_Label, 100 );

   ExposureParameters_Sizer.SetSpacing( 16 );
   ExposureParameters_Sizer.Add( ExposureParametersLeft_Sizer );
   ExposureParameters_Sizer.Add( ExposureParametersRight_Sizer, 100 );
   ExposureParameters_Sizer.AddStretch();

   const char* objectNameToolTip =
      "<p>Name of the main astronomical object or subject in the acquired images. The specified text will be the "
      "value of Observation:Object:Name standard XISF properties and OBJECT FITS keywords in newly created light frames.</p>";

   ObjectName_Label.SetText( "Object name:" );
   ObjectName_Label.SetToolTip( objectNameToolTip );
   ObjectName_Label.SetMinWidth( labelWidth1 );
   ObjectName_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   ObjectName_Edit.SetToolTip( objectNameToolTip );
   ObjectName_Edit.SetText( TheICFObjectNameParameter->DefaultValue() );

   ObjectName_Sizer.SetSpacing( 4 );
   ObjectName_Sizer.Add( ObjectName_Label );
   ObjectName_Sizer.Add( ObjectName_Edit, 100 );

   const char* telescopeDeviceToolTip =
      "<p>This parameter tells IndigoCCDFrame how to select the telescope used for acquisition of light frames:</p>"

      "<p><b>No telescope.</b> Newly acquired light frames won't have any property or keyword related to accurate observation "
      "coordinates. The OBJCTRA and OBJCDEC FITS keywords provided by Indigo will be left intact. This option is <i>not recommended</i> "
      "unless you are acquiring test frames with a camera on a table, for example.</p>"

      "<p><b>Active telescope</b> Select the device specified as the ACTIVE_DEVICES/ACTIVE_TELESCOPE property of the Indigo CCD device "
      "being used. You must define this property manually with the name of the appropriate telescope device.</p>"

      "<p><b>Mount controller telescope.</b> Use the device currently selected in the Indigo Mount Controller interface.</p>"

      "<p><b>Mount controller or active telescope.</b> Use the Indigo Mount Controller device if available, or the "
      "ACTIVE_DEVICES/ACTIVE_TELESCOPE device otherwise. This is the default option.</p>"

      "<p>When a telescope device is available, it is used to retrieve its current EOD (epoch of date) coordinates, just before "
      "starting each light frame exposure. These are apparent coordinates, which are reduced to mean positions referred to the mean "
      "equinox and equator of J2000.0 (ICRS) by an accurate inverse transformation, including corrections for precession, nutation "
      "and stellar aberration. The computed coordinates are directly comparable to standard catalog positions, irrespective of the "
      "date of acquisition. Once these coordinates are computed for each acquired image, they are stored as standard XISF properties "
      "(Observation:Center:RA and Observation:Center:Dec). For compatibility with legacy applications, the same coordinates replace "
      "the OBJCTRA and OBJCTDEC FITS keywords provided by the Indigo server.</p>";

   TelescopeDevice_Label.SetText( "Telescope device:" );
   TelescopeDevice_Label.SetToolTip( telescopeDeviceToolTip );
   TelescopeDevice_Label.SetMinWidth( labelWidth1 );
   TelescopeDevice_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   TelescopeDevice_Combo.AddItem( "No telescope" );
   TelescopeDevice_Combo.AddItem( "Active telescope" );
   TelescopeDevice_Combo.AddItem( "Mount controller telescope" );
   TelescopeDevice_Combo.AddItem( "Mount controller or active telescope" );
   TelescopeDevice_Combo.SetToolTip( telescopeDeviceToolTip );
   //TelescopeDevice_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );

   TelescopeDevice_Sizer.SetSpacing( 4 );
   TelescopeDevice_Sizer.Add( TelescopeDevice_Label );
   TelescopeDevice_Sizer.Add( TelescopeDevice_Combo, 100 );

   const char* externalFilterDeviceToolTip =
      "<p>When the CCD device does not have an integrated filter wheel, it is possible to </p>"
      "specify the device name of an external filter wheel here.</p>";

   ExternalFilterDevice_Label.SetText( "External filter wheel device:" );
   ExternalFilterDevice_Label.SetToolTip( externalFilterDeviceToolTip );
   ExternalFilterDevice_Label.SetMinWidth( labelWidth1 );
   ExternalFilterDevice_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   ExternalFilterDevice_Label.Disable();

   ExternalFilterDevice_Combo.AddItem( "<No filter wheel>" );
   ExternalFilterDevice_Combo.SetToolTip( externalFilterDeviceToolTip );
   ExternalFilterDevice_Combo.Disable();
   ExternalFilterDevice_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );

   ExternalFilterDevice_Sizer.SetSpacing( 4 );
   ExternalFilterDevice_Sizer.Add( ExternalFilterDevice_Label );
   ExternalFilterDevice_Sizer.Add( ExternalFilterDevice_Combo, 100 );

   FrameAcquisition_Sizer.SetSpacing( 4 );
   FrameAcquisition_Sizer.Add( ExposureParameters_Sizer );
   FrameAcquisition_Sizer.Add( ObjectName_Sizer );
   FrameAcquisition_Sizer.Add( TelescopeDevice_Sizer );
   FrameAcquisition_Sizer.Add( ExternalFilterDevice_Sizer );

   FrameAcquisition_Control.SetSizer( FrameAcquisition_Sizer );

   //

      //

   PlateSolverParameters_SectionBar.SetTitle( "Plate Solving Parameters" );
   PlateSolverParameters_SectionBar.SetSection( PlateSolverParameters_Control );

   //

   SolverAutoCatalog_CheckBox.SetText( "Automatic catalog" );
   SolverAutoCatalog_CheckBox.SetToolTip( "<p>When this option is enabled, PhotometricColorCalibration will select an optimal "
      "astrometric catalog for the field of view of the target image.</p>"
      "<p>Enabling this option is recommended under normal working conditions.</p>" );
   SolverAutoCatalog_CheckBox.Check();
   SolverAutoCatalog_CheckBox.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );

   SolverAutoCatalog_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SolverAutoCatalog_Sizer.Add( SolverAutoCatalog_CheckBox );
   SolverAutoCatalog_Sizer.AddStretch();

   //

   const char* solverCatalogToolTip = "<p>This is the catalog used to acquire positions and proper motions of stars for "
      "plate solving the target image.</p>"
      "<p>Under normal working conditions, you should select the <i>Automatic catalog</i> option to use an optimal catalog "
      "selected as a function of the field of view of the image.</p>"
      "<p>When using a manually selected catalog, you should select the appropriate catalog for your image. For example, for "
      "very wide field images (focal lengths smaller than 25 mm), you should use the Bright Stars catalog because it provides "
      "enough stars and the queries are fast for large fields. Gaia and PPMXL are useful for very deep images because they "
      "include very dim stars; however, online queries in these two catalogs can be slow for relatively large fields.</p>"
      "<p>If you have a Gaia XPSD server configured with local database files, you should use it in all cases except when the "
      "Bright Stars catalog is applicable.</p>";

   SolverCatalog_Label.SetText( "Astrometry catalog:" );
   SolverCatalog_Label.SetFixedWidth( labelWidth1 );
   SolverCatalog_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SolverCatalog_Label.SetToolTip( solverCatalogToolTip );

   for ( const CatalogData& catalog : s_solverCatalogs )
      SolverCatalog_ComboBox.AddItem( catalog.name );
   SolverCatalog_ComboBox.SetToolTip( solverCatalogToolTip );
   SolverCatalog_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );

   SolverCatalog_Sizer.SetSpacing( 4 );
   SolverCatalog_Sizer.Add( SolverCatalog_Label );
   SolverCatalog_Sizer.Add( SolverCatalog_ComboBox );
   SolverCatalog_Sizer.AddStretch();

   //

   SolverAutoLimitMagnitude_CheckBox.SetText( "Automatic limit magnitude" );
   SolverAutoLimitMagnitude_CheckBox.Check();
   SolverAutoLimitMagnitude_CheckBox.SetToolTip( "<p>When this option is enabled, PhotometricColorCalibration will select an optimal "
      "limit magnitude for plate solving based on the field of view and center coordinates of the target image. Enabling this option "
      "is recommended under normal working conditions.</p>" );
   SolverAutoLimitMagnitude_CheckBox.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );

   SolverAutoLimitMagnitude_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SolverAutoLimitMagnitude_Sizer.Add( SolverAutoLimitMagnitude_CheckBox );
   SolverAutoLimitMagnitude_Sizer.AddStretch();

   //

   const char* solverLimitMagnitudeToolTip = "<p>This parameter limits the number of stars extracted from the catalog to those "
      "with magnitude smaller than this value. This limit magnitude should be similar to the magnitude of the dimmest stars in the image. "
      "If this value is too low, the plate solving algorithm may not have enough stars to compare, but if the value is too high, it will "
      "try to match catalog stars that are not visible on the image. This will slow down the process, and could prevent the algorithm "
      "from finding a valid solution in extreme cases.</p>";

   SolverLimitMagnitude_Label.SetText( "Limit magnitude:" );
   SolverLimitMagnitude_Label.SetFixedWidth( labelWidth1 );
   SolverLimitMagnitude_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SolverLimitMagnitude_Label.SetToolTip( solverLimitMagnitudeToolTip );

   SolverLimitMagnitude_SpinBox.SetRange( int( TheICFSolverLimitMagnitudeParameter->MinimumValue() ),
                                          int( TheICFSolverLimitMagnitudeParameter->MaximumValue() ) );
   SolverLimitMagnitude_SpinBox.SetToolTip( solverLimitMagnitudeToolTip );
   SolverLimitMagnitude_SpinBox.SetValue(12);

   SolverLimitMagnitude_Sizer.SetSpacing( 4 );
   SolverLimitMagnitude_Sizer.Add( SolverLimitMagnitude_Label );
   SolverLimitMagnitude_Sizer.Add( SolverLimitMagnitude_SpinBox );
   SolverLimitMagnitude_Sizer.AddStretch();

   //

   DistortionCorrection_CheckBox.SetText( "Distortion correction" );
   DistortionCorrection_CheckBox.SetToolTip( "<p>This option enables a distortion correction algorithm based on 2-D surface splines, "
      "also known as <i>thin plates</i>. This option should always be selected when the target image suffers from significant "
      "distortions, or if it deviates significantly from an ideal projected image, which usually happens in images taken with focal "
      "lengths shorter than 400 mm.</p>" );
   DistortionCorrection_CheckBox.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );

   DistortionCorrection_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   DistortionCorrection_Sizer.Add( DistortionCorrection_CheckBox );
   DistortionCorrection_Sizer.AddStretch();

   //

   ForcePlateSolve_CheckBox.SetText( "Force plate solving" );
   ForcePlateSolve_CheckBox.SetToolTip( "<p>If enabled, the target image will be solved even if it contains a valid astrometric "
      "solution, which will be replaced with a newly computed one. See also the <i>Ignore existing metadata</i> option.</p>" );
   ForcePlateSolve_CheckBox.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );

   ForcePlateSolve_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ForcePlateSolve_Sizer.Add( ForcePlateSolve_CheckBox );
   ForcePlateSolve_Sizer.AddStretch();

   //

   IgnorePositionAndScale_CheckBox.SetText( "Ignore existing metadata" );
   IgnorePositionAndScale_CheckBox.SetToolTip( "<p>If enabled, existing metadata will be ignored and the values set by the "
      "running instance, that is, on this interface, will be used instead. This includes the right ascension and declination "
      "coordinates of the center of the image, the focal length, pixel size and observation date. If disabled, existing metadata "
      "will take precedence over the values defined by this instance.</p>"
      "<p>Note that for this option to take effect the image must be solved astrometrically, either because the <i>Force plate "
      "solving</i> option is enabled, or because the image has no valid astrometric solution.</p>" );
   IgnorePositionAndScale_CheckBox.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );

   //

   const char* resetSolverToolTip = "<p>Reset configuration settings for the ImageSolver script.</p>"
      "<p>This is normally not necessary, since scripts manage their own configuration settings internally. However, "
      "sometimes there are configuration parameters that may become wrong or corrupted for a variety of reasons. Use this "
      "option to initialize all working parameters of the ImageSolver script to factory-default values.</p>";

   ResetSolverConfiguration_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/reload.png" ) ) );
   ResetSolverConfiguration_ToolButton.SetScaledFixedSize( 22, 22 );
   ResetSolverConfiguration_ToolButton.SetToolTip( resetSolverToolTip );
   ResetSolverConfiguration_ToolButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );

   //

   IgnorePositionAndScale_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   IgnorePositionAndScale_Sizer.Add( IgnorePositionAndScale_CheckBox );
   IgnorePositionAndScale_Sizer.AddStretch();
   IgnorePositionAndScale_Sizer.Add( ResetSolverConfiguration_ToolButton );

   //

   PlateSolverParameters_Sizer.SetSpacing( 4 );
   PlateSolverParameters_Sizer.Add( SolverAutoCatalog_Sizer );
   PlateSolverParameters_Sizer.Add( SolverCatalog_Sizer );
   PlateSolverParameters_Sizer.Add( SolverAutoLimitMagnitude_Sizer );
   PlateSolverParameters_Sizer.Add( SolverLimitMagnitude_Sizer );
   PlateSolverParameters_Sizer.Add( DistortionCorrection_Sizer );
   PlateSolverParameters_Sizer.Add( ForcePlateSolve_Sizer );
   PlateSolverParameters_Sizer.Add( IgnorePositionAndScale_Sizer );

   PlateSolverParameters_Control.SetSizer( PlateSolverParameters_Sizer );

   //

   AdvancedPlateSolverParameters_SectionBar.SetTitle( "Advanced Plate Solving Parameters" );
   AdvancedPlateSolverParameters_SectionBar.SetSection( AdvancedPlateSolverParameters_Control );

   //

   const char* projectionSystemToolTip = "<p>Most images can be solved using the default Gnomonic projection, since it can "
      "approximate how most optical systems project an image on the focal plane. However, some images, especially very "
      "wide-field images (focal lengths less than 10-15 mm), usually require different projections to find a consistent solution. "
      "Several projections are provided besides Gnomonic, which can be more suitable for wide angle images.</p>";

   ProjectionSystem_Label.SetText( "Projection system:" );
   ProjectionSystem_Label.SetFixedWidth( labelWidth1 );
   ProjectionSystem_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ProjectionSystem_Label.SetToolTip( projectionSystemToolTip );

   // N.B.: Items must be coherent with the ImageSolver script.
   ProjectionSystem_ComboBox.AddItem( "Gnomonic" );
   ProjectionSystem_ComboBox.AddItem( "Stereographic" );
   ProjectionSystem_ComboBox.AddItem( u"Plate carrée" );
   ProjectionSystem_ComboBox.AddItem( "Mercator" );
   ProjectionSystem_ComboBox.AddItem( "Hammer-Aitoff" );
   ProjectionSystem_ComboBox.AddItem( "Zenithal equal area" );
   ProjectionSystem_ComboBox.AddItem( "Orthographic" );
   ProjectionSystem_ComboBox.SetToolTip( projectionSystemToolTip );
   ProjectionSystem_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );

   ProjectionSystem_Sizer.SetSpacing( 4 );
   ProjectionSystem_Sizer.Add( ProjectionSystem_Label );
   ProjectionSystem_Sizer.Add( ProjectionSystem_ComboBox );
   ProjectionSystem_Sizer.AddStretch();

   //

   StarSensitivity_NumericControl.label.SetText( "Log(sensitivity):" );
   StarSensitivity_NumericControl.label.SetFixedWidth( labelWidth1 );
   StarSensitivity_NumericControl.slider.SetRange( 0, 50 );
   StarSensitivity_NumericControl.slider.SetScaledMinWidth( 250 );
   StarSensitivity_NumericControl.SetReal();
   StarSensitivity_NumericControl.SetRange( TheICFSolverStarSensitivityParameter->MinimumValue(), TheICFSolverStarSensitivityParameter->MaximumValue() );
   StarSensitivity_NumericControl.SetPrecision( TheICFSolverStarSensitivityParameter->Precision() );
   StarSensitivity_NumericControl.SetToolTip( "<p>Logarithm of the star detection sensitivity. Increase this parameter to "
      "detect less stars. The default value of -1 is normally appropriate for most linear images.</p>" );

   //

   const char* noiseReductionToolTip = "<p>Some images have so much noise that the star detection algorithm can mistake "
      "noise for stars. This parameter removes noise from the image in order to improve star detection. This value is the "
      "number of wavelet layers that will be removed for noise reduction. Use 0 for no noise reduction.</p>";

   NoiseReduction_Label.SetText( "Noise reduction:" );
   NoiseReduction_Label.SetFixedWidth( labelWidth1 );
   NoiseReduction_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   NoiseReduction_Label.SetToolTip( noiseReductionToolTip );

   NoiseReduction_SpinBox.SetRange( int( TheICFSolverNoiseLayersParameter->MinimumValue() ),
                                    int( TheICFSolverNoiseLayersParameter->MaximumValue() ) );
   NoiseReduction_SpinBox.SetToolTip( noiseReductionToolTip );
//    NoiseReduction_SpinBox.SetFixedWidth( editWidth1 );

   NoiseReduction_Sizer.SetSpacing( 4 );
   NoiseReduction_Sizer.Add( NoiseReduction_Label );
   NoiseReduction_Sizer.Add( NoiseReduction_SpinBox );
   NoiseReduction_Sizer.AddStretch();

   //

   const char* alignmentDeviceToolTip = "<p>The algorithmic device used for star matching during the initial image "
      "registration step. Triangle similarity works for most images and supports specular transformations, that is, images "
      "mirrored horizontally or vertically. Polygon matching is much more robust and tolerant of distortions, but does not "
      "support mirrored images.</p>";

   AlignmentDevice_Label.SetText( "Alignment device:" );
   AlignmentDevice_Label.SetFixedWidth( labelWidth1 );
   AlignmentDevice_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   AlignmentDevice_Label.SetToolTip( alignmentDeviceToolTip );

   // N.B.: Items must be coherent with the ImageSolver script.
   AlignmentDevice_ComboBox.AddItem( "Triangle similarity" );
   AlignmentDevice_ComboBox.AddItem( "Polygon matching" );
   AlignmentDevice_ComboBox.SetToolTip( alignmentDeviceToolTip );
   AlignmentDevice_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );

   AlignmentDevice_Sizer.SetSpacing( 4 );
   AlignmentDevice_Sizer.Add( AlignmentDevice_Label );
   AlignmentDevice_Sizer.Add( AlignmentDevice_ComboBox );
   AlignmentDevice_Sizer.AddStretch();

   //

   SplineSmoothing_NumericControl.label.SetText( "Spline smoothing:" );
   SplineSmoothing_NumericControl.label.SetFixedWidth( labelWidth1 );
   SplineSmoothing_NumericControl.slider.SetRange( 0, 1000 );
   SplineSmoothing_NumericControl.slider.SetScaledMinWidth( 250 );
   SplineSmoothing_NumericControl.SetReal();
   SplineSmoothing_NumericControl.SetRange( TheICFSolverSplineSmoothingParameter->MinimumValue(), TheICFSolverSplineSmoothingParameter->MaximumValue() );
   SplineSmoothing_NumericControl.SetPrecision( TheICFSolverSplineSmoothingParameter->Precision() );
   SplineSmoothing_NumericControl.SetToolTip( "<p>When nonzero, this parameter causes the distortion correction algorithm to "
      "use approximating instead of interpolating surface splines (thin plates). Approximating surface splines are robust to "
      "outliers in the distortion correction model, including uncertainty in star detection, moving asteroids and other varying "
      "image features that can be accidentally interpreted and matched as stars.</p>" );

   //

   AdvancedPlateSolverParameters_Sizer.SetSpacing( 4 );
   AdvancedPlateSolverParameters_Sizer.Add( ProjectionSystem_Sizer );
   AdvancedPlateSolverParameters_Sizer.Add( StarSensitivity_NumericControl );
   AdvancedPlateSolverParameters_Sizer.Add( NoiseReduction_Sizer );
   AdvancedPlateSolverParameters_Sizer.Add( AlignmentDevice_Sizer );
   AdvancedPlateSolverParameters_Sizer.Add( SplineSmoothing_NumericControl );

   AdvancedPlateSolverParameters_Control.SetSizer( AdvancedPlateSolverParameters_Sizer );


   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( ServerParameters_SectionBar );
   Global_Sizer.Add( ServerParameters_Control );
   Global_Sizer.Add( ClientParameters_SectionBar );
   Global_Sizer.Add( ClientParameters_Control );
   Global_Sizer.Add( FrameAcquisition_SectionBar );
   Global_Sizer.Add( FrameAcquisition_Control );
   Global_Sizer.Add( PlateSolverParameters_SectionBar );
   Global_Sizer.Add( PlateSolverParameters_Control );
   Global_Sizer.Add( AdvancedPlateSolverParameters_SectionBar );
   Global_Sizer.Add( AdvancedPlateSolverParameters_Control );

   w.SetSizer( Global_Sizer );

   PlateSolverParameters_SectionBar.Disable();
   AdvancedPlateSolverParameters_SectionBar.Disable();
   PlateSolverParameters_Control.Hide();
   AdvancedPlateSolverParameters_Control.Hide();

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedHeight();

   w.OnShow( (Control::event_handler)&INDICCDFrameInterface::e_Show, w );
   w.OnHide( (Control::event_handler)&INDICCDFrameInterface::e_Hide, w );

   //

   UpdateDeviceList_Timer.SetInterval( 1 );
   UpdateDeviceList_Timer.SetPeriodic();
   UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&INDICCDFrameInterface::e_Timer, w );

   UpdateDeviceProperties_Timer.SetInterval( 1 );
   UpdateDeviceProperties_Timer.SetPeriodic();
   UpdateDeviceProperties_Timer.OnTimer( (Timer::timer_event_handler)&INDICCDFrameInterface::e_Timer, w );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void INDICCDFrameInterface::UpdateControls()
{
   if ( m_device.IsEmpty() )
   {
      GUI->UpdateDeviceProperties_Timer.Stop();

      GUI->CCDProperties_Control.Disable();
      GUI->ClientParameters_Control.Disable();
      GUI->FrameAcquisition_Control.Disable();
   }
   else
   {
      if ( !GUI->UpdateDeviceProperties_Timer.IsRunning() )
         GUI->UpdateDeviceProperties_Timer.Start();

      GUI->CCDProperties_Control.Enable();
      GUI->ClientParameters_Control.Enable();
      GUI->FrameAcquisition_Control.Enable();

   }
}

// ----------------------------------------------------------------------------

void INDICCDFrameInterface::e_Show( Control& )
{
   if ( GUI != nullptr )
   {
      e_Timer( GUI->UpdateDeviceList_Timer );
      GUI->UpdateDeviceList_Timer.Start();
   }
}

// ----------------------------------------------------------------------------

void INDICCDFrameInterface::e_Hide( Control& )
{
   if ( GUI != nullptr )
   {
      GUI->UpdateDeviceList_Timer.Stop();
      GUI->UpdateDeviceProperties_Timer.Stop();
   }
}

// ----------------------------------------------------------------------------

void INDICCDFrameInterface::e_Timer( Timer& sender )
{
   if ( sender == GUI->UpdateDeviceList_Timer )
   {
      GUI->CCDDevice_Combo.Clear();

      if ( INDIClient::HasClient() )
      {
         INDIClient* indi = INDIClient::TheClient();
         ExclConstDeviceList x = indi->ConstDeviceList();
         const INDIDeviceListItemArray& devices( x );
         if ( devices.IsEmpty() )
            GUI->CCDDevice_Combo.AddItem( "<No Device Available>" );
         else
         {
            GUI->CCDDevice_Combo.AddItem( String() );

            for ( auto device : devices )
            {
               INDIPropertyListItem item;
               if ( indi->HasPropertyItem( device.DeviceName, CCD_FRAME_PROPERTY_NAME, CCD_FRAME_WIDTH_ITEM_NAME ) ) // is this a camera device?
                  GUI->CCDDevice_Combo.AddItem( device.DeviceName );
            }

            GUI->CCDDevice_Combo.SetItemText( 0,
               ( GUI->CCDDevice_Combo.NumberOfItems() > 1 ) ? "<No Device Selected>" : "<No Camera Device Available>" );

            int i = Max( 0, GUI->CCDDevice_Combo.FindItem( m_device ) );
            GUI->CCDDevice_Combo.SetCurrentItem( i );
            if ( i > 0 )
               goto __device_found;
         }
      }
      else
         GUI->CCDDevice_Combo.AddItem( "<Indigo Server Not Connected>" );

      m_device.Clear();

   __device_found:

      UpdateControls();
   }
   else if ( sender == GUI->UpdateDeviceProperties_Timer )
   {
      if ( !INDIClient::HasClient() )
         return;

      INDIClient* indi = INDIClient::TheClient();
      INDIPropertyListItem item;

      if ( indi->GetPropertyItem( m_device, CCD_TEMPERATURE_PROPERTY_NAME, CCD_TEMPERATURE_ITEM_NAME, item ) )
      {
         GUI->CCDTargetTemp_NumericEdit.Enable();
         GUI->CCDTargetTemp_NumericEdit.label.Enable();
         GUI->CCDTargetTemp_ToolButton.Enable();
         GUI->CCDTemp_NumericEdit.SetValue( item.PropertyValue.ToDouble() );
      }
      else
      {
         GUI->CCDTargetTemp_NumericEdit.Disable();
         GUI->CCDTargetTemp_NumericEdit.label.Disable();
         GUI->CCDTargetTemp_ToolButton.Disable();
      }

      if ( indi->GetPropertyItem( m_device, CCD_BIN_PROPERTY_NAME, CCD_BIN_HORIZONTAL_ITEM_NAME, item ) )
      {
         GUI->CCDBinX_Combo.Enable();
         GUI->CCDBinX_Label.Enable();
         GUI->CCDBinX_Combo.SetCurrentItem( item.PropertyValue.ToInt() - 1 );
      }
      else
      {
         GUI->CCDBinX_Combo.Disable();
         GUI->CCDBinX_Label.Disable();
      }

      if ( indi->GetPropertyItem( m_device, CCD_BIN_PROPERTY_NAME, CCD_BIN_VERTICAL_ITEM_NAME, item ) )
      {
         GUI->CCDBinY_Combo.Enable();
         GUI->CCDBinY_Label.Enable();
         GUI->CCDBinY_Combo.SetCurrentItem( item.PropertyValue.ToInt() - 1 );
      }
      else
      {
         GUI->CCDBinY_Combo.Disable();
         GUI->CCDBinY_Label.Disable();
      }

      String externalFilterWheelDeviceName = GUI->ExternalFilterDevice_Combo.ItemText( GUI->ExternalFilterDevice_Combo.CurrentItem() );
      if ( indi->GetPropertyItem( externalFilterWheelDeviceName != String( "<No filter wheel>" ) ? externalFilterWheelDeviceName : m_device, WHEEL_SLOT_PROPERTY_NAME, WHEEL_SLOT_ITEM_NAME, item ) )
      {
         int currentFilterIndex = item.PropertyValue.ToInt() - 1;
         GUI->CCDFilter_Combo.Clear();
         for ( int i = 1; i <= 256; ++i )
         {
            if ( !indi->GetPropertyItem( externalFilterWheelDeviceName != String( "<No filter wheel>" ) ? externalFilterWheelDeviceName : m_device, WHEEL_SLOT_NAME_PROPERTY_NAME, "SLOT_NAME_" + String( i ), item ) )
               break;
            GUI->CCDFilter_Combo.AddItem( item.PropertyValue );
         }

         GUI->CCDFilter_Combo.Enable();
         GUI->CCDFilter_Label.Enable();
         GUI->FilterConfig_ToolButton.Enable();
         GUI->CCDFilter_Combo.SetCurrentItem( currentFilterIndex );
      }
      else
      {
         GUI->CCDFilter_Combo.Clear();
         GUI->CCDFilter_Combo.Disable();
         GUI->CCDFilter_Label.Disable();
      }

      int uploadModeIndex = -1;
      if ( indi->GetPropertyItem( m_device, CCD_UPLOAD_MODE_PROPERTY_NAME, CCD_UPLOAD_MODE_CLIENT_ITEM_NAME, item ) )
      {
         if ( item.PropertyValue == "OFF" )
         {
            if ( indi->GetPropertyItem( m_device, CCD_UPLOAD_MODE_PROPERTY_NAME, CCD_UPLOAD_MODE_LOCAL_ITEM_NAME, item ) )
               if ( item.PropertyValue == "OFF" )
                  uploadModeIndex = ICFUploadMode::UploadServerAndClient;
               else
                  uploadModeIndex = ICFUploadMode::UploadServer;
         }
         else
            uploadModeIndex = ICFUploadMode::UploadClient;
      }

      if ( uploadModeIndex >= 0 )
      {
         GUI->UploadMode_Combo.Enable();
         GUI->UploadMode_Label.Enable();
         GUI->UploadMode_Combo.SetCurrentItem( uploadModeIndex );

         if ( m_execution == nullptr )
            if ( uploadModeIndex == ICFUploadMode::UploadClient || uploadModeIndex == ICFUploadMode::UploadServerAndClient )
            {
               bool openClientFrames = GUI->OpenClientFrames_CheckBox.IsChecked();
               bool saveClientFrames = GUI->SaveClientFrames_CheckBox.IsChecked();
               GUI->OpenClientFrames_CheckBox.Enable();
               GUI->ReuseImageWindow_CheckBox.Enable( openClientFrames && !saveClientFrames );
               GUI->AutoStretch_CheckBox.Enable( openClientFrames || saveClientFrames );
               GUI->LinkedAutoStretch_CheckBox.Enable( ( openClientFrames || saveClientFrames ) && GUI->AutoStretch_CheckBox.IsChecked() );
               GUI->PlateSolving_CheckBox.Enable(openClientFrames);
               if (!openClientFrames)
                  GUI->PlateSolving_CheckBox.SetChecked(false);
               GUI->SaveClientFrames_CheckBox.Enable();
               GUI->OverwriteClientFrames_CheckBox.Enable( saveClientFrames );
               GUI->ClientDownloadDir_Label.Enable( saveClientFrames );
               GUI->ClientDownloadDir_Edit.Enable( saveClientFrames );
               GUI->ClientDownloadDir_ToolButton.Enable( saveClientFrames );
               GUI->ClientFileNameTemplate_Label.Enable( saveClientFrames );
               GUI->ClientFileNameTemplate_Edit.Enable( saveClientFrames );
               GUI->ClientOutputFormatHints_Label.Enable( saveClientFrames );
               GUI->ClientOutputFormatHints_Edit.Enable( saveClientFrames );
            }
            else
            {
               GUI->OpenClientFrames_CheckBox.Disable();
               GUI->ReuseImageWindow_CheckBox.Disable();
               GUI->AutoStretch_CheckBox.Disable();
               GUI->LinkedAutoStretch_CheckBox.Disable();
               GUI->SaveClientFrames_CheckBox.Disable();
               GUI->PlateSolving_CheckBox.Disable();
               GUI->OverwriteClientFrames_CheckBox.Disable();
               GUI->ClientDownloadDir_Label.Disable();
               GUI->ClientDownloadDir_Edit.Disable();
               GUI->ClientDownloadDir_ToolButton.Disable();
               GUI->ClientFileNameTemplate_Label.Disable();
               GUI->ClientFileNameTemplate_Edit.Disable();
               GUI->ClientOutputFormatHints_Label.Disable();
               GUI->ClientOutputFormatHints_Edit.Disable();
            }

         if ( uploadModeIndex == ICFUploadMode::UploadServer || uploadModeIndex == ICFUploadMode::UploadServerAndClient )
         {
            GUI->ServerUploadDir_Label.Enable();
            GUI->ServerUploadDir_ToolButton.Enable();
            GUI->ServerFileNameTemplate_Label.Enable();
            GUI->ServerFileNameTemplate_Edit.Enable();
         }
         else
         {
            GUI->ServerUploadDir_Label.Disable();
            GUI->ServerUploadDir_ToolButton.Disable();
            GUI->ServerFileNameTemplate_Label.Disable();
            GUI->ServerFileNameTemplate_Edit.Disable();
         }
      }
      else
      {
         GUI->UploadMode_Combo.Disable();
         GUI->UploadMode_Label.Disable();
      }

      if (GUI->PlateSolving_CheckBox.IsChecked())
      {
         GUI->PlateSolverParameters_SectionBar.Enable();
         GUI->AdvancedPlateSolverParameters_SectionBar.Enable();
         GUI->CenterTarget_CheckBox.Enable();
      }
      else
      {
         GUI->PlateSolverParameters_SectionBar.Disable();
         GUI->AdvancedPlateSolverParameters_SectionBar.Disable();
         GUI->CenterTarget_CheckBox.SetChecked(false);
         GUI->CenterTarget_CheckBox.Disable();
      }


      {
         static const char* indiFrameTypes[] = { CCD_FRAME_TYPE_LIGHT_ITEM_NAME, CCD_FRAME_TYPE_BIAS_ITEM_NAME, CCD_FRAME_TYPE_DARK_ITEM_NAME, CCD_FRAME_TYPE_FLAT_ITEM_NAME };
         for ( size_type i = 0; i < ItemsInArray( indiFrameTypes ); ++i )
            if ( indi->GetPropertyItem( m_device, CCD_FRAME_TYPE_PROPERTY_NAME, indiFrameTypes[i], item ) )
               if ( item.PropertyValue == "ON" )
               {
                  GUI->CCDFrameType_Label.Enable();
                  GUI->CCDFrameType_Combo.SetCurrentItem( i );
                  GUI->CCDFrameType_Combo.Enable();
                  break;
               }

         if ( m_execution == nullptr )
         {
            bool isLightFrame = GUI->CCDFrameType_Combo.CurrentItem() == ICFFrameType::LightFrame;
            GUI->ObjectName_Label.Enable( isLightFrame );
            GUI->ObjectName_Edit.Enable( isLightFrame );
            GUI->TelescopeDevice_Label.Enable( isLightFrame );
            GUI->TelescopeDevice_Combo.Enable( isLightFrame );
         }
      }

      if ( indi->GetPropertyItem( m_device, CCD_LOCAL_MODE_PROPERTY_NAME, CCD_LOCAL_MODE_DIR_ITEM_NAME, item ) )
         GUI->ServerUploadDir_Edit.SetText( item.PropertyValue );

      {
         if ( !GUI->ExternalFilterDevice_Combo.IsEnabled() && !GUI->CCDFilter_Combo.IsEnabled() )
         {
            GUI->ExternalFilterDevice_Combo.Clear();
            ExclConstDeviceList x = indi->ConstDeviceList();
            const INDIDeviceListItemArray& devices( x );
            for ( auto device : devices )
            {
               // add filter devices to combo box
               if ( indi->HasPropertyItem( device.DeviceName, WHEEL_SLOT_PROPERTY_NAME, WHEEL_SLOT_ITEM_NAME ) )
                  GUI->ExternalFilterDevice_Combo.AddItem( device.DeviceName );
            }
            GUI->ExternalFilterDevice_Combo.Enable();
            GUI->ExternalFilterDevice_Label.Enable();
         }
      }
   }
}

// ----------------------------------------------------------------------------

void INDICCDFrameInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( !INDIClient::HasClient() )
      return;

   INDIClient* indi = INDIClient::TheClient();

   if ( sender == GUI->CCDDevice_Combo )
   {
      // The first item in the combo box list has been reserved for a "no device" selection.
      m_device = ( itemIndex > 0 ) ? sender.ItemText( itemIndex ).Trimmed() : String();
      UpdateControls();

      // check for cooler connection (e.g. Atik cameras)
      if ( !m_device.IsEmpty() )
      {
         INDIPropertyListItem item;
         if ( indi->GetPropertyItem( m_device, CCD_COOLER_PROPERTY_NAME, CCD_COOLER_ON_ITEM_NAME, item ) )
            if ( item.PropertyValue == "OFF" )
               indi->SendNewPropertyItem( m_device, CCD_COOLER_PROPERTY_NAME, "INDI_SWITCH", CCD_COOLER_ON_ITEM_NAME, "ON", true /*async*/ );

         // load configuration on server
         indi->SendNewPropertyItem( m_device, CONFIG_PROPERTY_NAME, "INDI_SWITCH", CONFIG_LOAD_ITEM_NAME, "ON" );
      }
   }
   else if ( sender == GUI->CCDBinX_Combo )
   {
      indi->MaybeSendNewPropertyItem( m_device, CCD_BIN_PROPERTY_NAME, "INDI_NUMBER",
         CCD_BIN_HORIZONTAL_ITEM_NAME, GUI->CCDBinX_Combo.ItemText( itemIndex ).Trimmed(), true /*async*/ );
   }
   else if ( sender == GUI->CCDBinY_Combo )
   {
      indi->MaybeSendNewPropertyItem( m_device, CCD_BIN_PROPERTY_NAME, "INDI_NUMBER",
         CCD_BIN_HORIZONTAL_ITEM_NAME, GUI->CCDBinX_Combo.ItemText( itemIndex ).Trimmed(),
         CCD_BIN_VERTICAL_ITEM_NAME, GUI->CCDBinY_Combo.ItemText( itemIndex ).Trimmed(),
         true /*async*/ );
   }
   else if ( sender == GUI->CCDFilter_Combo )
   {
      indi->MaybeSendNewPropertyItem( CurrentFilterWheelDeviceName(), WHEEL_SLOT_PROPERTY_NAME, "INDI_NUMBER",
         WHEEL_SLOT_ITEM_NAME, itemIndex + 1, true /*async*/ );
   }
   else if ( sender == GUI->UploadMode_Combo )
   {
      indi->MaybeSendNewPropertyItem( m_device, CCD_UPLOAD_MODE_PROPERTY_NAME, "INDI_SWITCH",
         INDICCDFrameInstance::UploadModePropertyString( itemIndex ), "ON", true /*async*/ );
   }
   else if ( sender == GUI->CCDFrameType_Combo )
   {
      indi->MaybeSendNewPropertyItem( m_device, CCD_FRAME_TYPE_PROPERTY_NAME, "INDI_SWITCH",
         INDICCDFrameInstance::CCDFrameTypePropertyString( itemIndex ), "ON", true /*async*/ );
   }
   else if ( sender == GUI->ExternalFilterDevice_Combo )
   {
      String externalFilterWheelDeviceName = sender.ItemText( itemIndex ).Trimmed();
      // load configuration on server
      indi->MaybeSendNewPropertyItem( externalFilterWheelDeviceName, CONFIG_PROPERTY_NAME, "INDI_SWITCH", CONFIG_LOAD_ITEM_NAME, "ON", true /*async*/ );
   }
}


// ----------------------------------------------------------------------------


void INDICCDFrameInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->ClientDownloadDir_Edit )
      if ( sender.IsEnabled() )
         wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
}

// ----------------------------------------------------------------------------

void INDICCDFrameInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->ClientDownloadDir_Edit )
      if ( sender.IsEnabled() )
         if ( File::DirectoryExists( files[0] ) )
            GUI->ClientDownloadDir_Edit.SetText( files[0] );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class INDICCDFrameInterfaceExecution : public AbstractINDICCDFrameExecution
{
public:
   INDICCDFrameInterfaceExecution( INDICCDFrameInterface* iface )
      : AbstractINDICCDFrameExecution( *dynamic_cast<INDICCDFrameInstance*>( iface->NewProcess() ) )
      , m_iface( iface )
      , m_instanceAuto( &m_instance )
      , m_abortRequested( false )
   {
   }

   virtual void Abort()
   {
      m_abortRequested = true;
   }

private:

   INDICCDFrameInterface* m_iface;
   AutoPointer<INDICCDFrameInstance> m_instanceAuto;
   bool m_abortRequested;

   virtual void StartAcquisitionEvent()
   {
      m_iface->m_execution = this;
      m_iface->GUI->ExposureTime_NumericEdit.Disable();
      m_iface->GUI->ExposureDelay_NumericEdit.Disable();
      m_iface->GUI->ExposureCount_Label.Disable();
      m_iface->GUI->ExposureCount_SpinBox.Disable();
      m_iface->GUI->ObjectName_Label.Disable();
      m_iface->GUI->ObjectName_Edit.Disable();
      m_iface->GUI->TelescopeDevice_Label.Disable();
      m_iface->GUI->TelescopeDevice_Combo.Disable();
      m_iface->GUI->OpenClientFrames_CheckBox.Disable();
      m_iface->GUI->ReuseImageWindow_CheckBox.Disable();
      m_iface->GUI->AutoStretch_CheckBox.Disable();
      m_iface->GUI->LinkedAutoStretch_CheckBox.Disable();
      m_iface->GUI->SaveClientFrames_CheckBox.Disable();
      m_iface->GUI->OverwriteClientFrames_CheckBox.Disable();
      m_iface->GUI->ClientDownloadDir_Label.Disable();
      m_iface->GUI->ClientDownloadDir_Edit.Disable();
      m_iface->GUI->ClientDownloadDir_ToolButton.Disable();
      m_iface->GUI->ClientFileNameTemplate_Label.Disable();
      m_iface->GUI->ClientFileNameTemplate_Edit.Disable();
      m_iface->GUI->ClientOutputFormatHints_Label.Disable();
      m_iface->GUI->ClientOutputFormatHints_Edit.Disable();
      m_iface->GUI->StartExposure_PushButton.Disable();
      m_iface->GUI->CancelExposure_PushButton.Enable();
      m_iface->GUI->ExposureInfo_Label.Clear();
      m_iface->GUI->PlateSolving_CheckBox.Disable();
      m_iface->GUI->CenterTarget_CheckBox.Disable();
   }

   virtual void NewExposureEvent( int expNum, int expCount )
   {
      m_iface->ProcessEvents();
   }

   virtual void StartExposureDelayEvent( double totalDelayTime )
   {
      m_iface->GUI->ExposureInfo_Label.SetText( String().Format( "Waiting for %.3gs", totalDelayTime ) );
      m_iface->ProcessEvents();
   }

   virtual void ExposureDelayEvent( double delayTime )
   {
      if ( m_abortRequested )
         AbstractINDICCDFrameExecution::Abort();

      m_iface->ProcessEvents();
   }

   virtual void EndExposureDelayEvent()
   {
      m_iface->GUI->ExposureInfo_Label.Clear();
      m_iface->ProcessEvents();
   }

   virtual void StartExposureEvent( int expNum, int expCount, double expTime )
   {
      m_iface->ProcessEvents();
   }

   virtual void ExposureEvent( int expNum, int expCount, double expTime )
   {
      if ( m_abortRequested )
         AbstractINDICCDFrameExecution::Abort();

      m_iface->GUI->ExposureInfo_Label.SetText( String().Format( "Exposure %d of %d: %.3gs", expNum + 1, expCount, expTime ) );
      m_iface->ProcessEvents();
   }

   virtual void ExposureErrorEvent( const String& errMsg )
   {
      m_iface->GUI->ExposureInfo_Label.SetText( "*** Error: " + errMsg + '.' );
      m_iface->ProcessEvents();
   }

   virtual void EndExposureEvent( int expNum )
   {
      m_iface->GUI->ExposureInfo_Label.Clear();
      m_iface->ProcessEvents();
   }

   virtual void WaitingForServerEvent()
   {
      if ( m_abortRequested )
         AbstractINDICCDFrameExecution::Abort();

      m_iface->GUI->ExposureInfo_Label.SetText( "Waiting for Indigo server" );
      m_iface->ProcessEvents();
   }

   virtual void NewFrameEvent( ImageWindow& window, bool reusedWindow, bool geometryChanged )
   {
      if ( reusedWindow )
      {
         if ( geometryChanged )
            window.ZoomToFit( false /*allowZoom*/ );
         else
            window.Regenerate();
      }
      else
      {
         window.BringToFront();
         window.Show();
         window.ZoomToFit( false /*allowZoom*/ );
      }
      m_iface->ProcessEvents();
   }

   virtual void NewFrameEvent( const String& filePath )
   {
      m_iface->ProcessEvents();
   }

   virtual void EndAcquisitionEvent()
   {
      m_iface->m_execution = nullptr;
      m_iface->GUI->ExposureTime_NumericEdit.Enable();
      m_iface->GUI->ExposureDelay_NumericEdit.Enable();
      m_iface->GUI->ExposureCount_Label.Enable();
      m_iface->GUI->ExposureCount_SpinBox.Enable();
      m_iface->GUI->StartExposure_PushButton.Enable();
      m_iface->GUI->PlateSolving_CheckBox.Enable();
      m_iface->GUI->CenterTarget_CheckBox.Enable();
      m_iface->GUI->CancelExposure_PushButton.Disable();
      m_iface->GUI->ExposureInfo_Label.Clear();
      m_iface->ProcessEvents();
      // N.B.: The rest of client interface items will be enabled/disabled by
      // timer event handlers.
   }

   virtual void AbortEvent()
   {
      EndAcquisitionEvent();
      m_iface->GUI->ExposureInfo_Label.SetText( "Aborted" );
   }
};

// ----------------------------------------------------------------------------

void INDICCDFrameInterface::e_Click( Button& sender, bool checked )
{
   if ( !INDIClient::HasClient() )
      return;

   if ( sender == GUI->CCDTargetTemp_ToolButton )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, CCD_TEMPERATURE_PROPERTY_NAME, "INDI_NUMBER",
         CCD_TEMPERATURE_ITEM_NAME, GUI->CCDTargetTemp_NumericEdit.Value(), true /*async*/ );
   }
   else if ( sender == GUI->ServerUploadDir_ToolButton )
   {
      SimpleGetStringDialog dialog( "Server upload directory:", GUI->ServerUploadDir_Edit.Text() );
      if ( dialog.Execute() )
         INDIClient::TheClient()->SendNewPropertyItem( m_device, CCD_LOCAL_MODE_PROPERTY_NAME, "INDI_TEXT", CCD_LOCAL_MODE_DIR_ITEM_NAME, dialog.Text(), true /*async*/ );
   }
   else if ( sender == GUI->ClientDownloadDir_ToolButton )
   {
      GetDirectoryDialog d;
      d.SetCaption( "IndigoCCDFrame: Select Client Download Directory" );
      if ( d.Execute() )
         GUI->ClientDownloadDir_Edit.SetText( d.Directory() );
   }
   else if ( sender == GUI->StartExposure_PushButton )
   {
      INDICCDFrameInterfaceExecution( this ).Perform();
   }
   else if ( sender == GUI->CancelExposure_PushButton )
   {
      if ( m_execution != nullptr )
         m_execution->Abort();
   }
   else if ( sender == GUI->FilterConfig_ToolButton )
   {
      FilterConfigDialog filterWheelConfig( CurrentFilterWheelDeviceName() );
      for ( size_t index = 0; index < (size_t)GUI->CCDFilter_Combo.NumberOfItems(); index++ )
      {
         filterWheelConfig.addFilterName( index + 1, GUI->CCDFilter_Combo.ItemText( index ) );
      }
      filterWheelConfig.adjustTreeColumns();
      if ( filterWheelConfig.Execute() && INDIClient::HasClient() )
      {
         // ??
      }
   }
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF INDICCDFrameInterface.cpp - Released 2020-12-17T15:46:56Z
