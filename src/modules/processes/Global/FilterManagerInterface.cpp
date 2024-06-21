//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.0
// ----------------------------------------------------------------------------
// FilterManagerInterface.cpp - Released 2024-06-18T15:49:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#include "CurveExplorerDialog.h"
#include "FilterManagerInterface.h"
#include "FilterManagerParameters.h"
#include "FilterManagerProcess.h"

#include <pcl/Console.h>
#include <pcl/FileDialog.h>
#include <pcl/MessageBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FilterManagerInterface* TheFilterManagerInterface = nullptr;

// ----------------------------------------------------------------------------

FilterManagerInterface::FilterManagerInterface()
   : m_instance( TheFilterManagerProcess )
{
   TheFilterManagerInterface = this;
}

// ----------------------------------------------------------------------------

FilterManagerInterface::~FilterManagerInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString FilterManagerInterface::Id() const
{
   return "FilterManager";
}

// ----------------------------------------------------------------------------

MetaProcess* FilterManagerInterface::Process() const
{
   return TheFilterManagerProcess;
}

// ----------------------------------------------------------------------------

String FilterManagerInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/FilterManager.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures FilterManagerInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

// ----------------------------------------------------------------------------

void FilterManagerInterface::ApplyInstance() const
{
   m_instance.LaunchGlobal();
}

// ----------------------------------------------------------------------------

void FilterManagerInterface::ResetInstance()
{
   FilterManagerInstance defaultInstance( TheFilterManagerProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool FilterManagerInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "FilterManager" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheFilterManagerProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* FilterManagerInterface::NewProcess() const
{
   return new FilterManagerInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool FilterManagerInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const FilterManagerInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a FilterManager instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool FilterManagerInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool FilterManagerInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

bool FilterManagerInterface::WantsGlobalNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void FilterManagerInterface::GlobalFiltersUpdated()
{
   if ( m_curveExplorer != nullptr )
      if ( m_curveExplorer->IsVisible() )
         m_curveExplorer->ReloadData();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FilterManagerInterface::UpdateControls()
{
   if ( GUI != nullptr )
   {
      GUI->Task_ComboBox.SetCurrentItem( m_instance.p_task );

      switch ( m_instance.p_task )
      {
      case FMTask::NoAction:
      case FMTask::MergeDefault:
      case FMTask::SetDefault:
         GUI->Path_Edit.Disable();
         GUI->Path_ToolButton.Disable();
         break;
      default:
         GUI->Path_Edit.Enable();
         GUI->Path_ToolButton.Enable();
         break;
      }

      switch ( m_instance.p_task )
      {
      case FMTask::NoAction:
      case FMTask::MergeDefault:
      case FMTask::SetDefault:
         GUI->Path_Label.SetText( "File or directory:" );
         break;
      case FMTask::ImportXML:
      case FMTask::MergeXML:
         GUI->Path_Label.SetText( "Input file:" );
         break;
      case FMTask::ImportCSV:
      case FMTask::MergeCSV:
         GUI->Path_Label.SetText( "Input directory:" );
         break;
      case FMTask::ExportXML:
         GUI->Path_Label.SetText( "Output file:" );
         break;
      case FMTask::ExportCSV:
         GUI->Path_Label.SetText( "Output directory:" );
         break;
      }
   }

   GUI->Path_Edit.SetText( m_instance.p_path );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FilterManagerInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Task_ComboBox )
   {
      m_instance.p_task = itemIndex;
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

void FilterManagerInterface::e_EditCompleted( Edit& sender )
{
   if ( sender == GUI->Path_Edit )
      sender.SetText( m_instance.p_path = sender.Text().Trimmed() );
}

// ----------------------------------------------------------------------------

void FilterManagerInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->CurveExplorer_Button )
   {
      if ( m_curveExplorer == nullptr )
         m_curveExplorer = new CurveExplorerDialog;

      m_curveExplorer->Execute();
      if ( m_curveExplorer->MadeChanges() )
         UpdateControls();
   }
   else if ( sender == GUI->Path_ToolButton )
   {
      switch ( m_instance.p_task )
      {
      case FMTask::ImportXML:
      case FMTask::MergeXML:
         {
            OpenFileDialog d;
            d.DisableMultipleSelections();
            d.SetCaption( "FilterManager: Input Filters Database File" );
            d.SetFilter( FileFilter( "XSPD Spectrum Database files", ".xspd" ) );
            if ( d.Execute() )
               m_instance.p_path = d.FileName().Trimmed();
         }
         break;
      case FMTask::ImportCSV:
      case FMTask::MergeCSV:
         {
            GetDirectoryDialog d;
            d.SetCaption( "FilterManager: Input CSV Filter Definitions Directory" );
            if ( d.Execute() )
               m_instance.p_path = d.Directory().Trimmed();
         }
         break;
      case FMTask::ExportXML:
         {
            SaveFileDialog d;
            d.SetCaption( "FilterManager: Output Filters Database File" );
            d.SetFilter( FileFilter( "XSPD Spectrum Database files", ".xspd" ) );
            d.EnableOverwritePrompt();
            if ( d.Execute() )
               m_instance.p_path = d.FileName().Trimmed();
         }
         break;
      case FMTask::ExportCSV:
         {
            GetDirectoryDialog d;
            d.SetCaption( "FilterManager: Output CSV Filter Definitions Directory" );
            if ( d.Execute() )
               m_instance.p_path = d.Directory().Trimmed();
         }
         break;
      default:
         break;
      }

      UpdateControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FilterManagerInterface::GUIData::GUIData( FilterManagerInterface& w )
{
   pcl::Font fnt = w.Font();
   int emWidth = fnt.Width( "m" );
   int labelWidth1 = fnt.Width( "File or directory:" ) + 2*emWidth;
   int editWidth1 = 32*emWidth;
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   CurveExplorer_Button.SetText( "Curve Explorer" );
   CurveExplorer_Button.SetToolTip( "<p>Explore the current filter and white reference databases with interactive graphs.</p>" );
   CurveExplorer_Button.OnClick( (Button::click_event_handler)&FilterManagerInterface::e_Click, w );

   CurveExplorer_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CurveExplorer_Sizer.Add( CurveExplorer_Button );
   CurveExplorer_Sizer.AddStretch();

   //

   const char* taskToolTip = "<p>The maintenance task that will be performed on global execution.</p>"
   "<p><b>Import XML filters database.</b> The current filter set will be replaced with a new one loaded "
   "from the specified input file in XSPD format.</p>"
   "<p><b>Merge XML filters database.</b> Loads new filter definitions from the specified input file "
   "(XSPD format) and merges them with existing ones. New filters will be added and already existing ones "
   "will be updated with imported data.</p>"
   "<p><b>Import CSV filter definitions.</b> Scans the specified input directory for valid filter "
   "definitions in CSV format (with .csv extension), which will replace the current filter set.</p>"
   "<p><b>Merge CSV filter definitions.</b> Scans the specified input directory for valid filter "
   "definitions in CSV format and merges them with existing ones. New filters will be added and already "
   "existing ones will be updated with imported data.</p>"
   "<p><b>Export XML filters database.</b> Writes the current filter definitions to the specified "
   "output file in XSPD format.</p>"
   "<p><b>Export CSV filter definitions.</b> Writes current filter definitions as individual CSV files "
   "(with .csv extension) on the specified output directory.</p>"
   "<p><b>Merge with default filters.</b> Merge current filter definitions with the factory default "
   "filter set.</p>"
   "<p><b>Set default filters.</b> Reset current filter definitions back to the factory default filter set.</p>";

   Task_Label.SetText( "Task:" );
   Task_Label.SetFixedWidth( labelWidth1 );
   Task_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Task_Label.SetToolTip( taskToolTip );

   Task_ComboBox.AddItem( "No action" );
   Task_ComboBox.AddItem( "Import XML filters database" );
   Task_ComboBox.AddItem( "Merge XML filters database" );
   Task_ComboBox.AddItem( "Import CSV filter definitions" );
   Task_ComboBox.AddItem( "Merge CSV filter definitions" );
   Task_ComboBox.AddItem( "Export XML filters database" );
   Task_ComboBox.AddItem( "Export CSV filter definitions" );
   Task_ComboBox.AddItem( "Merge with default filters" );
   Task_ComboBox.AddItem( "Set default filters" );
   Task_ComboBox.SetToolTip( taskToolTip );
   Task_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&FilterManagerInterface::e_ItemSelected, w );

   Task_Sizer.SetSpacing( 4 );
   Task_Sizer.Add( Task_Label );
   Task_Sizer.Add( Task_ComboBox );
   Task_Sizer.AddStretch();

   //

   Path_Label.SetText( "Input path:" );
   Path_Label.SetFixedWidth( labelWidth1 );
   Path_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Path_Edit.OnEditCompleted( (Edit::edit_event_handler)&FilterManagerInterface::e_EditCompleted, w );
   Path_Edit.SetMinWidth( editWidth1 );

   Path_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   Path_ToolButton.SetScaledFixedSize( 20, 20 );
   Path_ToolButton.OnClick( (Button::click_event_handler)&FilterManagerInterface::e_Click, w );

   Path_Sizer.SetSpacing( 4 );
   Path_Sizer.Add( Path_Label );
   Path_Sizer.Add( Path_Edit, 100 );
   Path_Sizer.Add( Path_ToolButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( CurveExplorer_Sizer );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Task_Sizer );
   Global_Sizer.Add( Path_Sizer );

   //

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FilterManagerInterface.cpp - Released 2024-06-18T15:49:25Z
