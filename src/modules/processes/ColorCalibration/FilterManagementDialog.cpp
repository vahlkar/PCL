//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.5
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// FilterManagementDialog.cpp - Released 2024-01-13T15:48:22Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "FilterManagementDialog.h"

#include <pcl/FileDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FilterManagementDialog::FilterManagementDialog()
{
   pcl::Font fnt = this->Font();
   int labelWidth1 = fnt.Width( "Output directory:m" );
   int editWidth1 = fnt.Width( String( 'm', 40 ) );
   int ui4 = LogicalPixelsToPhysical( 4 );

   ImportXMLFiltersDatabase_RadioButton.SetText( "Import XML filters database" );
   ImportXMLFiltersDatabase_RadioButton.SetToolTip( "<p>Load filter definitions from the specified input file in XSPD format.</p>" );
   ImportXMLFiltersDatabase_RadioButton.OnClick( (Button::click_event_handler)&FilterManagementDialog::e_Click, *this );

   ImportXMLFiltersDatabase_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ImportXMLFiltersDatabase_Sizer.Add( ImportXMLFiltersDatabase_RadioButton );
   ImportXMLFiltersDatabase_Sizer.AddStretch();

   ImportCSVFilterDefinitions_RadioButton.SetText( "Import CSV filter definitions" );
   ImportCSVFilterDefinitions_RadioButton.SetToolTip( "<p>Load filter definitions from all existing CSV files (with .csv extension) "
      "in the specified input directory.</p>" );
   ImportCSVFilterDefinitions_RadioButton.OnClick( (Button::click_event_handler)&FilterManagementDialog::e_Click, *this );

   Merge_CheckBox.SetText( "Merge with current filters" );
   Merge_CheckBox.SetToolTip( "<p>If disabled, the current filter set will be replaced with a new one loaded from the specified file(s). "
      "If enabled, imported filters will be merged with existing ones: new filters will be added and already existing ones will be "
      "updated with imported data.</p>" );
   Merge_CheckBox.OnClick( (Button::click_event_handler)&FilterManagementDialog::e_Click, *this );

   ImportCSVFilterDefinitions_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ImportCSVFilterDefinitions_Sizer.Add( ImportCSVFilterDefinitions_RadioButton );
   ImportCSVFilterDefinitions_Sizer.AddSpacing( 16 );
   ImportCSVFilterDefinitions_Sizer.Add( Merge_CheckBox );
   ImportCSVFilterDefinitions_Sizer.AddStretch();

   InputPath_Label.SetText( "Input path:" );
   InputPath_Label.SetFixedWidth( labelWidth1 );
   InputPath_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   InputPath_Edit.OnEditCompleted( (Edit::edit_event_handler)&FilterManagementDialog::e_EditCompleted, *this );
   InputPath_Edit.SetMinWidth( editWidth1 );

   InputPath_ToolButton.SetIcon( Bitmap( ScaledResource( ":/icons/select-file.png" ) ) );
   InputPath_ToolButton.SetScaledFixedSize( 20, 20 );
   InputPath_ToolButton.OnClick( (Button::click_event_handler)&FilterManagementDialog::e_Click, *this );

   InputPath_Sizer.SetSpacing( 4 );
   InputPath_Sizer.Add( InputPath_Label );
   InputPath_Sizer.Add( InputPath_Edit, 100 );
   InputPath_Sizer.Add( InputPath_ToolButton );

   //

   ExportXMLFiltersDatabase_RadioButton.SetText( "Export XML filters database" );
   ExportXMLFiltersDatabase_RadioButton.SetToolTip( "<p>Write current filter definitions to the specified output file in XSPD format.</p>" );
   ExportXMLFiltersDatabase_RadioButton.OnClick( (Button::click_event_handler)&FilterManagementDialog::e_Click, *this );

   ExportXMLFiltersDatabase_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ExportXMLFiltersDatabase_Sizer.Add( ExportXMLFiltersDatabase_RadioButton );
   ExportXMLFiltersDatabase_Sizer.AddStretch();

   ExportCSVFilterDefinitions_RadioButton.SetText( "Export CSV filter definitions" );
   ExportCSVFilterDefinitions_RadioButton.SetToolTip( "<p>Write current filter definitions as CSV files (with .csv extension) "
      "on the specified output directory.</p>" );
   ExportCSVFilterDefinitions_RadioButton.OnClick( (Button::click_event_handler)&FilterManagementDialog::e_Click, *this );

   ExportCSVFilterDefinitions_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ExportCSVFilterDefinitions_Sizer.Add( ExportCSVFilterDefinitions_RadioButton );
   ExportCSVFilterDefinitions_Sizer.AddStretch();

   OutputPath_Label.SetText( "Output path:" );
   OutputPath_Label.SetFixedWidth( labelWidth1 );
   OutputPath_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputPath_Edit.OnEditCompleted( (Edit::edit_event_handler)&FilterManagementDialog::e_EditCompleted, *this );
   OutputPath_Edit.SetMinWidth( editWidth1 );

   OutputPath_ToolButton.SetIcon( Bitmap( ScaledResource( ":/icons/select-file.png" ) ) );
   OutputPath_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputPath_ToolButton.OnClick( (Button::click_event_handler)&FilterManagementDialog::e_Click, *this );

   OutputPath_Sizer.SetSpacing( 4 );
   OutputPath_Sizer.Add( OutputPath_Label );
   OutputPath_Sizer.Add( OutputPath_Edit, 100 );
   OutputPath_Sizer.Add( OutputPath_ToolButton );

   //

   MergeWithDefaultFilters_RadioButton.SetText( "Merge with default filters" );
   MergeWithDefaultFilters_RadioButton.SetToolTip( "<p>Merge current filter definitions with the factory-default filter set.</p>" );
   MergeWithDefaultFilters_RadioButton.OnClick( (Button::click_event_handler)&FilterManagementDialog::e_Click, *this );

   MergeWithDefaultFilters_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   MergeWithDefaultFilters_Sizer.Add( MergeWithDefaultFilters_RadioButton );
   MergeWithDefaultFilters_Sizer.AddStretch();

   //

   ResetFilters_RadioButton.SetText( "Reset filters" );
   ResetFilters_RadioButton.SetToolTip( "<p>Reset current filter definitions to the factory-default filter set.</p>" );
   ResetFilters_RadioButton.OnClick( (Button::click_event_handler)&FilterManagementDialog::e_Click, *this );

   ResetFilters_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ResetFilters_Sizer.Add( ResetFilters_RadioButton );
   ResetFilters_Sizer.AddStretch();

   //

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&FilterManagementDialog::e_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&FilterManagementDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( ImportXMLFiltersDatabase_Sizer );
   Global_Sizer.Add( ImportCSVFilterDefinitions_Sizer );
   Global_Sizer.Add( InputPath_Sizer );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( ExportXMLFiltersDatabase_Sizer );
   Global_Sizer.Add( ExportCSVFilterDefinitions_Sizer );
   Global_Sizer.Add( OutputPath_Sizer );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( MergeWithDefaultFilters_Sizer );
   Global_Sizer.Add( ResetFilters_Sizer );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetFixedHeight();
   SetMinWidth();

   SetWindowTitle( "SPCC Filter Management" );

   UpdateControls();
}

// ----------------------------------------------------------------------------

void FilterManagementDialog::UpdateControls()
{
   switch ( m_mode )
   {
   case ImportXML:
      ImportXMLFiltersDatabase_RadioButton.SetChecked();
      InputPath_Label.SetText( "Input file:" );
      InputPath_Edit.SetText( m_inputXMLFilePath );
      InputPath_Edit.SetToolTip( "<p>The input XML filter database file.</p>" );
      InputPath_ToolButton.SetToolTip( "<p>Select the input XML filter database file.</p>" );
      OK_PushButton.Enable( !m_inputXMLFilePath.IsEmpty() );
      break;
   case ExportXML:
      ExportXMLFiltersDatabase_RadioButton.SetChecked();
      OutputPath_Label.SetText( "Output file:" );
      OutputPath_Edit.SetText( m_outputXMLFilePath );
      OutputPath_Edit.SetToolTip( "<p>The output XML filter database file.</p>" );
      OutputPath_ToolButton.SetToolTip( "<p>Select the output XML filter database file.</p>" );
      OK_PushButton.Enable( !m_outputXMLFilePath.IsEmpty() );
      break;
   case ImportCSV:
      ImportCSVFilterDefinitions_RadioButton.SetChecked();
      InputPath_Label.SetText( "Input directory:" );
      InputPath_Edit.SetText( m_inputCSVDirectory );
      InputPath_Edit.SetToolTip( "<p>The directory to import CSV filter definition files.</p>" );
      InputPath_ToolButton.SetToolTip( "<p>Select a directory to import CSV filter definition files.</p>" );
      OK_PushButton.Enable( !m_inputCSVDirectory.IsEmpty() );
      break;
   case ExportCSV:
      ExportCSVFilterDefinitions_RadioButton.SetChecked();
      OutputPath_Label.SetText( "Output directory:" );
      OutputPath_Edit.SetText( m_outputCSVDirectory );
      OutputPath_Edit.SetToolTip( "<p>The directory to export CSV filter definition files.</p>" );
      OutputPath_ToolButton.SetToolTip( "<p>Select a directory to export CSV filter definition files.</p>" );
      OK_PushButton.Enable( !m_outputCSVDirectory.IsEmpty() );
      break;
   case MergeWithDefault:
      MergeWithDefaultFilters_RadioButton.SetChecked();
      OK_PushButton.Enable();
      break;
   case Reset:
      ResetFilters_RadioButton.SetChecked();
      OK_PushButton.Enable();
      break;
   }

   switch ( m_mode )
   {
   case ImportXML:
   case ImportCSV:
      Merge_CheckBox.Enable();
      InputPath_Label.Enable();
      InputPath_Edit.Enable();
      InputPath_ToolButton.Enable();
      OutputPath_Label.Disable();
      OutputPath_Edit.Disable();
      OutputPath_ToolButton.Disable();
      break;
   case ExportXML:
   case ExportCSV:
      OutputPath_Label.Enable();
      OutputPath_Edit.Enable();
      OutputPath_ToolButton.Enable();
      Merge_CheckBox.Disable();
      InputPath_Label.Disable();
      InputPath_Edit.Disable();
      InputPath_ToolButton.Disable();
      break;
   case MergeWithDefault:
   case Reset:
      Merge_CheckBox.Disable();
      InputPath_Label.Disable();
      InputPath_Edit.Disable();
      InputPath_ToolButton.Disable();
      OutputPath_Label.Disable();
      OutputPath_Edit.Disable();
      OutputPath_ToolButton.Disable();
      break;
   }

   Merge_CheckBox.SetChecked( m_merge );
}

// ----------------------------------------------------------------------------

void FilterManagementDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == ImportXMLFiltersDatabase_RadioButton )
   {
      m_mode = ImportXML;
      UpdateControls();
   }
   else if ( sender == ImportCSVFilterDefinitions_RadioButton )
   {
      m_mode = ImportCSV;
      UpdateControls();
   }
   else if ( sender == Merge_CheckBox )
   {
      m_merge = checked;
   }
   else if ( sender == InputPath_ToolButton )
   {
      switch ( m_mode )
      {
      case ImportXML:
         {
            OpenFileDialog d;
            d.DisableMultipleSelections();
            d.SetCaption( "SPCC: Input Filters Database File" );
            d.SetFilter( FileFilter( "XSPD Spectrum Database files", ".xspd" ) );
            if ( d.Execute() )
            {
               m_inputXMLFilePath = d.FileName();
               UpdateControls();
            }
         }
         break;
      case ImportCSV:
         {
            GetDirectoryDialog d;
            d.SetCaption( "SPCC: Input CSV Filter Definitions Directory" );
            if ( d.Execute() )
            {
               m_inputCSVDirectory = d.Directory();
               UpdateControls();
            }
         }
         break;
      default: // ?!
         break;
      }
   }
   else if ( sender == ExportXMLFiltersDatabase_RadioButton )
   {
      m_mode = ExportXML;
      UpdateControls();
   }
   else if ( sender == ExportCSVFilterDefinitions_RadioButton )
   {
      m_mode = ExportCSV;
      UpdateControls();
   }
   else if ( sender == OutputPath_ToolButton )
   {
      switch ( m_mode )
      {
      case ExportXML:
         {
            SaveFileDialog d;
            d.SetCaption( "SPCC: Output Filters Database File" );
            d.SetFilter( FileFilter( "XSPD Spectrum Database files", ".xspd" ) );
            d.EnableOverwritePrompt();
            if ( d.Execute() )
            {
               m_outputXMLFilePath = d.FileName();
               UpdateControls();
            }
         }
         break;
      case ExportCSV:
         {
            GetDirectoryDialog d;
            d.SetCaption( "SPCC: Output CSV Filter Definitions Directory" );
            if ( d.Execute() )
            {
               m_outputCSVDirectory = d.Directory();
               UpdateControls();
            }
         }
         break;
      default: // ?!
         break;
      }
   }
   else if ( sender == MergeWithDefaultFilters_RadioButton )
   {
      m_mode = MergeWithDefault;
      UpdateControls();
   }
   else if ( sender == ResetFilters_RadioButton )
   {
      m_mode = Reset;
      UpdateControls();
   }
   else if ( sender == OK_PushButton )
   {
      Ok();
   }
   else if ( sender == Cancel_PushButton )
   {
      Cancel();
   }
}

// ----------------------------------------------------------------------------

void FilterManagementDialog::e_EditCompleted( Edit& sender )
{
   String text = sender.Text().Trimmed();

   if ( sender == InputPath_Edit )
   {
      switch ( m_mode )
      {
      case ImportXML:
         m_inputXMLFilePath = text;
         break;
      case ImportCSV:
         m_inputCSVDirectory = text;
         break;
      default: // ?!
         break;
      }
   }
   else if ( sender == OutputPath_Edit )
   {
      switch ( m_mode )
      {
      case ExportXML:
         m_outputXMLFilePath = text;
         break;
      case ExportCSV:
         m_outputCSVDirectory = text;
         break;
      default: // ?!
         break;
      }
   }

   sender.SetText( text );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FilterManagementDialog.cpp - Released 2024-01-13T15:48:22Z
