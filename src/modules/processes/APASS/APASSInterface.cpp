//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard APASS Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// APASSInterface.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard APASS PixInsight module.
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

#include "APASSInterface.h"
#include "APASSProcess.h"
#include "APASSParameters.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/OnlineObjectSearchDialog.h>
#include <pcl/TreeBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class APASSFlagsSelectionDialog : public Dialog
{
public:

   uint32 flags = 0;

   APASSFlagsSelectionDialog( uint32 );

private:

   VerticalSizer  Global_Sizer;

   CheckBox       NoMag_B_CheckBox;
   CheckBox       NoMag_V_CheckBox;
   CheckBox       NoMag_u_CheckBox;
   CheckBox       NoMag_g_CheckBox;
   CheckBox       NoMag_r_CheckBox;
   CheckBox       NoMag_i_CheckBox;
   CheckBox       NoMag_z_s_CheckBox;
   CheckBox       NoMag_Y_CheckBox;
   CheckBox       PosErrorHigh_CheckBox;

   HorizontalSizer   Buttons_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   void e_Click( Button& sender, bool checked );
   void e_Return( Dialog& sender, int retVal );
};

APASSFlagsSelectionDialog::APASSFlagsSelectionDialog( uint32 a_flags )
   : flags( a_flags )
{
   NoMag_B_CheckBox.SetText( "No Johnson B magnitude" );
   NoMag_B_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No Johnson V magnitude (Vega system) available.</p>" );
   NoMag_B_CheckBox.OnClick( (Button::click_event_handler)&APASSFlagsSelectionDialog::e_Click, *this );

   NoMag_V_CheckBox.SetText( "No Johnson V magnitude" );
   NoMag_V_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No Johnson V magnitude (Vega system) available.</p>" );
   NoMag_V_CheckBox.OnClick( (Button::click_event_handler)&APASSFlagsSelectionDialog::e_Click, *this );

   NoMag_u_CheckBox.SetText( "No Sloan u' magnitude" );
   NoMag_u_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No Sloan u' magnitude (AB system) available.</p>" );
   NoMag_u_CheckBox.OnClick( (Button::click_event_handler)&APASSFlagsSelectionDialog::e_Click, *this );

   NoMag_g_CheckBox.SetText( "No Sloan g' magnitude" );
   NoMag_g_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No Sloan g' magnitude (AB system) available.</p>" );
   NoMag_g_CheckBox.OnClick( (Button::click_event_handler)&APASSFlagsSelectionDialog::e_Click, *this );

   NoMag_r_CheckBox.SetText( "No Sloan r' magnitude" );
   NoMag_r_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No Sloan r' magnitude (AB system) available.</p>" );
   NoMag_r_CheckBox.OnClick( (Button::click_event_handler)&APASSFlagsSelectionDialog::e_Click, *this );

   NoMag_i_CheckBox.SetText( "No Sloan i' magnitude" );
   NoMag_i_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No Sloan i' magnitude (AB system) available.</p>" );
   NoMag_i_CheckBox.OnClick( (Button::click_event_handler)&APASSFlagsSelectionDialog::e_Click, *this );

   NoMag_z_s_CheckBox.SetText( "No Sloan z_s magnitude" );
   NoMag_z_s_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No Sloan z_s magnitude (AB system) available.</p>" );
   NoMag_z_s_CheckBox.OnClick( (Button::click_event_handler)&APASSFlagsSelectionDialog::e_Click, *this );

   NoMag_Y_CheckBox.SetText( "No Sloan Y magnitude" );
   NoMag_Y_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No Sloan Y magnitude (AB system) available.</p>" );
   NoMag_Y_CheckBox.OnClick( (Button::click_event_handler)&APASSFlagsSelectionDialog::e_Click, *this );

   PosErrorHigh_CheckBox.SetText( "High positional error" );
   PosErrorHigh_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Right ascension and/or declination uncertainty &ge; 0.75 arcseconds.</p>" );
   PosErrorHigh_CheckBox.OnClick( (Button::click_event_handler)&APASSFlagsSelectionDialog::e_Click, *this );

   //

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&APASSFlagsSelectionDialog::e_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&APASSFlagsSelectionDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( NoMag_B_CheckBox );
   Global_Sizer.Add( NoMag_V_CheckBox );
   Global_Sizer.Add( NoMag_u_CheckBox );
   Global_Sizer.Add( NoMag_g_CheckBox );
   Global_Sizer.Add( NoMag_r_CheckBox );
   Global_Sizer.Add( NoMag_i_CheckBox );
   Global_Sizer.Add( NoMag_z_s_CheckBox );
   Global_Sizer.Add( NoMag_Y_CheckBox );
   Global_Sizer.Add( PosErrorHigh_CheckBox );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "APASS Source Flags" );

   NoMag_B_CheckBox.SetChecked(      (flags & APASSStarFlag::NoMag_B      ) != 0 );
   NoMag_V_CheckBox.SetChecked(      (flags & APASSStarFlag::NoMag_V      ) != 0 );
   NoMag_u_CheckBox.SetChecked(      (flags & APASSStarFlag::NoMag_u      ) != 0 );
   NoMag_g_CheckBox.SetChecked(      (flags & APASSStarFlag::NoMag_g      ) != 0 );
   NoMag_r_CheckBox.SetChecked(      (flags & APASSStarFlag::NoMag_r      ) != 0 );
   NoMag_i_CheckBox.SetChecked(      (flags & APASSStarFlag::NoMag_i      ) != 0 );
   NoMag_z_s_CheckBox.SetChecked(    (flags & APASSStarFlag::NoMag_z_s    ) != 0 );
   NoMag_Y_CheckBox.SetChecked(      (flags & APASSStarFlag::NoMag_Y      ) != 0 );
   PosErrorHigh_CheckBox.SetChecked( (flags & APASSStarFlag::PosErrorHigh ) != 0 );
}

void APASSFlagsSelectionDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == NoMag_B_CheckBox )
   {
      if ( checked )
         flags |= APASSStarFlag::NoMag_B;
      else
         flags &= ~APASSStarFlag::NoMag_B;
   }
   else if ( sender == NoMag_V_CheckBox )
   {
      if ( checked )
         flags |= APASSStarFlag::NoMag_V;
      else
         flags &= ~APASSStarFlag::NoMag_V;
   }
   else if ( sender == NoMag_u_CheckBox )
   {
      if ( checked )
         flags |= APASSStarFlag::NoMag_u;
      else
         flags &= ~APASSStarFlag::NoMag_u;
   }
   else if ( sender == NoMag_g_CheckBox )
   {
      if ( checked )
         flags |= APASSStarFlag::NoMag_g;
      else
         flags &= ~APASSStarFlag::NoMag_g;
   }
   else if ( sender == NoMag_r_CheckBox )
   {
      if ( checked )
         flags |= APASSStarFlag::NoMag_r;
      else
         flags &= ~APASSStarFlag::NoMag_r;
   }
   else if ( sender == NoMag_i_CheckBox )
   {
      if ( checked )
         flags |= APASSStarFlag::NoMag_i;
      else
         flags &= ~APASSStarFlag::NoMag_i;
   }
   else if ( sender == NoMag_z_s_CheckBox )
   {
      if ( checked )
         flags |= APASSStarFlag::NoMag_z_s;
      else
         flags &= ~APASSStarFlag::NoMag_z_s;
   }
   else if ( sender == NoMag_Y_CheckBox )
   {
      if ( checked )
         flags |= APASSStarFlag::NoMag_Y;
      else
         flags &= ~APASSStarFlag::NoMag_Y;
   }
   else if ( sender == PosErrorHigh_CheckBox )
   {
      if ( checked )
         flags |= APASSStarFlag::PosErrorHigh;
      else
         flags &= ~APASSStarFlag::PosErrorHigh;
   }

   else if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class APASSPreferencesDialog : public Dialog
{
public:

   APASSPreferencesDialog();

private:

   pcl_enum   m_dataRelease = ADataRelease::DR10;
   StringList m_databaseFilePaths;

   VerticalSizer     Global_Sizer;
      HorizontalSizer   DataRelease_Sizer;
         Label             DataRelease_Label;
         ComboBox          DataRelease_ComboBox;
      Label             Title_Label;
      TreeBox           FilePaths_TreeBox;
      HorizontalSizer   FileButtons_Sizer;
         PushButton        SelectFiles_PushButton;
         PushButton        RemoveFile_PushButton;
         PushButton        ClearFiles_PushButton;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void UpdateControls();

   void e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Click( Button& sender, bool checked );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );
   void e_Return( Dialog& sender, int retVal );
};

APASSPreferencesDialog::APASSPreferencesDialog()
{
   pcl::Font fnt = Font();

   const char* dataReleaseToolTip = "<p>You can choose between APASS data releases 9 and 10 (APASS DR9 and DR10). "
      "Both catalogs are available in XPSD format and are fully supported by this implementation.</p>";

   DataRelease_Label.SetText( "Data release:" );
   DataRelease_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DataRelease_Label.SetToolTip( dataReleaseToolTip );

   DataRelease_ComboBox.AddItem( "APASS DR9" );
   DataRelease_ComboBox.AddItem( "APASS DR10" );
   DataRelease_ComboBox.SetToolTip( dataReleaseToolTip );
   DataRelease_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&APASSPreferencesDialog::e_ItemSelected, *this );

   DataRelease_Sizer.SetSpacing( 4 );
   DataRelease_Sizer.Add( DataRelease_Label );
   DataRelease_Sizer.Add( DataRelease_ComboBox );
   DataRelease_Sizer.AddStretch();

   Title_Label.SetText( "APASS Database Files" );
   Title_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   FilePaths_TreeBox.SetNumberOfColumns( 1 );
   FilePaths_TreeBox.HideHeader();
   FilePaths_TreeBox.DisableMultipleSelections();
   FilePaths_TreeBox.DisableRootDecoration();
   FilePaths_TreeBox.EnableAlternateRowColor();
   FilePaths_TreeBox.SetMinSize( 40*fnt.Width( "M" ), 24*fnt.Height() );
   FilePaths_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&APASSPreferencesDialog::e_NodeActivated, *this );
   FilePaths_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&APASSPreferencesDialog::e_FileDrag, *this );
   FilePaths_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&APASSPreferencesDialog::e_FileDrop, *this );

   SelectFiles_PushButton.SetText( "Select" );
   SelectFiles_PushButton.SetToolTip( "<p>Select a new set of database files.</p>" );
   SelectFiles_PushButton.OnClick( (Button::click_event_handler)&APASSPreferencesDialog::e_Click, *this );

   RemoveFile_PushButton.SetText( "Remove" );
   RemoveFile_PushButton.SetToolTip( "<p>Remove the selected database file from the list.</p>" );
   RemoveFile_PushButton.OnClick( (Button::click_event_handler)&APASSPreferencesDialog::e_Click, *this );

   ClearFiles_PushButton.SetText( "Clear" );
   ClearFiles_PushButton.SetToolTip( "<p>Clear the list of database files.</p>" );
   ClearFiles_PushButton.OnClick( (Button::click_event_handler)&APASSPreferencesDialog::e_Click, *this );

   FileButtons_Sizer.SetSpacing( 4 );
   FileButtons_Sizer.Add( SelectFiles_PushButton );
   FileButtons_Sizer.Add( RemoveFile_PushButton );
   FileButtons_Sizer.AddStretch();
   FileButtons_Sizer.Add( ClearFiles_PushButton );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&APASSPreferencesDialog::e_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&APASSPreferencesDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( DataRelease_Sizer );
   Global_Sizer.Add( Title_Label );
   Global_Sizer.Add( FilePaths_TreeBox );
   Global_Sizer.Add( FileButtons_Sizer );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetMinSize();

   SetWindowTitle( "APASS Preferences" );

   OnReturn( (pcl::Dialog::return_event_handler)&APASSPreferencesDialog::e_Return, *this );

   if ( TheAPASSInterface != nullptr )
      m_dataRelease = TheAPASSInterface->m_instance.OutputDataRelease();

   if ( !TheAPASSProcess->PreferencesLoaded( m_dataRelease ) )
      TheAPASSProcess->LoadPreferences( m_dataRelease );
   m_databaseFilePaths = TheAPASSProcess->DatabaseFilePaths( m_dataRelease );
   UpdateControls();
}

void APASSPreferencesDialog::UpdateControls()
{
   DataRelease_ComboBox.SetCurrentItem( m_dataRelease );

   Title_Label.SetText( "APASS " + ADataRelease::ReleaseName( m_dataRelease ) + " Database Files" );

   int currentIdx = FilePaths_TreeBox.ChildIndex( FilePaths_TreeBox.CurrentNode() );

   FilePaths_TreeBox.DisableUpdates();
   FilePaths_TreeBox.Clear();

   for ( size_type i = 0; i < m_databaseFilePaths.Length(); ++i )
      (new TreeBox::Node( FilePaths_TreeBox ))->SetText( 0, m_databaseFilePaths[i] );

   if ( currentIdx >= 0 && currentIdx < int( m_databaseFilePaths.Length() ) )
      FilePaths_TreeBox.SetCurrentNode( FilePaths_TreeBox[currentIdx] );

   FilePaths_TreeBox.EnableUpdates();

   RemoveFile_PushButton.Enable( FilePaths_TreeBox.NumberOfChildren() > 0 );
   ClearFiles_PushButton.Enable( FilePaths_TreeBox.NumberOfChildren() > 0 );
}

void APASSPreferencesDialog::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   OpenFileDialog d;
   d.SetCaption( "APASS: Select Database File" );
   d.DisableMultipleSelections();
   d.SetFilter( FileFilter( "XPSD Files", ".xpsd" ) );
   if ( d.Execute() )
      m_databaseFilePaths[sender.ChildIndex( &node )] = d.FileName();
}

void APASSPreferencesDialog::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == DataRelease_ComboBox )
      if ( m_dataRelease != itemIndex )
      {
         m_dataRelease = itemIndex;
         if ( !TheAPASSProcess->PreferencesLoaded( m_dataRelease ) )
            TheAPASSProcess->LoadPreferences( m_dataRelease );
         m_databaseFilePaths = TheAPASSProcess->DatabaseFilePaths( m_dataRelease );
         UpdateControls();
      }
}

void APASSPreferencesDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == SelectFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "APASS: Select Database Files" );
      d.EnableMultipleSelections();
      d.SetFilter( FileFilter( "XPSD Files", ".xpsd" ) );
      if ( d.Execute() )
      {
         m_databaseFilePaths = d.FileNames();
         UpdateControls();
      }
   }
   else if ( sender == RemoveFile_PushButton )
   {
      const TreeBox::Node* n = FilePaths_TreeBox.CurrentNode();
      if ( n != nullptr )
      {
         m_databaseFilePaths.Remove( m_databaseFilePaths.At( FilePaths_TreeBox.ChildIndex( n ) ) );
         UpdateControls();
      }
   }
   else if ( sender == ClearFiles_PushButton )
   {
      m_databaseFilePaths.Clear();
      UpdateControls();
   }
   else if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

void APASSPreferencesDialog::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == FilePaths_TreeBox.Viewport() )
   {
      for ( const String& file : files )
      {
         if ( !File::Exists( file ) )
            return;
         if ( File::ExtractExtension( file ).CaseFolded() != ".xpsd" )
            return;
      }
      wantsFiles = true;
   }
}

void APASSPreferencesDialog::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == FilePaths_TreeBox.Viewport() )
   {
      StringList inputFiles;
      for ( const String& file : files )
         if ( File::Exists( file ) )
            if ( File::ExtractExtension( file ).CaseFolded() == ".xpsd" )
               inputFiles << file;
      if ( !inputFiles.IsEmpty() )
      {
         m_databaseFilePaths = inputFiles;
         UpdateControls();
      }
   }
}

void APASSPreferencesDialog::e_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      TheAPASSProcess->SetDatabaseFilePaths( m_dataRelease, m_databaseFilePaths );
      if ( TheAPASSInterface != nullptr )
         TheAPASSInterface->UpdateControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

APASSInterface* TheAPASSInterface = nullptr;

// ----------------------------------------------------------------------------

APASSInterface::APASSInterface()
   : m_instance( TheAPASSProcess )
{
   TheAPASSInterface = this;
}

// ----------------------------------------------------------------------------

APASSInterface::~APASSInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString APASSInterface::Id() const
{
   return "APASS";
}

// ----------------------------------------------------------------------------

MetaProcess* APASSInterface::Process() const
{
   return TheAPASSProcess;
}

// ----------------------------------------------------------------------------

String APASSInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/APASS.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures APASSInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal | InterfaceFeature::PreferencesButton;
}

// ----------------------------------------------------------------------------

void APASSInterface::EditPreferences()
{
   APASSPreferencesDialog().Execute();
}

// ----------------------------------------------------------------------------

void APASSInterface::ApplyInstance() const
{
   m_instance.LaunchGlobal();
}

// ----------------------------------------------------------------------------

void APASSInterface::ResetInstance()
{
   APASSInstance defaultInstance( TheAPASSProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool APASSInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "APASS" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheAPASSProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* APASSInterface::NewProcess() const
{
   return new APASSInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool APASSInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const APASSInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a APASS instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool APASSInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool APASSInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void APASSInterface::UpdateControls()
{
   GUI->DataRelease_ComboBox.SetCurrentItem( m_instance.OutputDataRelease() );

   GUI->CenterRA_Edit.SetText( RAToString( m_instance.p_searchData.centerRA ) );

   GUI->CenterDec_Edit.SetText( DecToString( m_instance.p_searchData.centerDec ) );

   GUI->Radius_Edit.SetText( DistToString( m_instance.p_searchData.radius ) );

   GUI->ShowCompoundAngles_CheckBox.SetChecked( m_showCompoundAngles );

   GUI->RAInTimeUnits_CheckBox.SetChecked( m_raInTimeUnits );

   GUI->MagnitudeLow_NumericEdit.SetValue( m_instance.p_searchData.magnitudeLow );

   GUI->MagnitudeHigh_NumericEdit.SetValue( m_instance.p_searchData.magnitudeHigh );

   GUI->RequiredFlags_Edit.SetText( String().Format( "%08x", m_instance.p_searchData.requiredFlags ) );

   GUI->InclusionFlags_Edit.SetText( String().Format( "%08x", m_instance.p_searchData.inclusionFlags ) );

   GUI->ExclusionFlags_Edit.SetText( String().Format( "%08x", m_instance.p_searchData.exclusionFlags ) );

   GUI->SourceLimit_NumericEdit.SetValue( m_instance.p_searchData.sourceLimit );

   GUI->SortBy_ComboBox.SetCurrentItem( m_instance.p_sortBy );

   GUI->GenerateTextOutput_CheckBox.SetChecked( m_instance.p_generateTextOutput );

   GUI->TextFormat_Label.Enable( m_instance.p_generateTextOutput );

   GUI->TextFormat_ComboBox.SetCurrentItem( m_instance.p_textFormat );
   GUI->TextFormat_ComboBox.Enable( m_instance.p_generateTextOutput );

   GUI->TextHeaders_Label.Enable( m_instance.p_generateTextOutput );

   GUI->TextHeaders_ComboBox.SetCurrentItem( m_instance.p_textHeaders );
   GUI->TextHeaders_ComboBox.Enable( m_instance.p_generateTextOutput );

   GUI->OutputFilePath_Label.Enable( m_instance.p_generateTextOutput );

   GUI->OutputFilePath_Edit.SetText( m_instance.p_outputFilePath );
   GUI->OutputFilePath_Edit.Enable( m_instance.p_generateTextOutput );

   GUI->OutputFilePath_ToolButton.Enable( m_instance.p_generateTextOutput );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

String APASSInterface::RAToString( double ra ) const
{
   if ( m_raInTimeUnits )
      ra /= 15;

   if ( m_showCompoundAngles )
      return String::ToSexagesimal( ra, SexagesimalConversionOptions( 3/*items*/,
                                                                      4/*precision*/,
                                                                      false/*sign*/,
                                                                      0/*width*/,
                                                                      ' '/*separator*/ ) );
   return String().Format( "%.9f", ra );
}

String APASSInterface::DecToString( double dec ) const
{
   if ( m_showCompoundAngles )
      return String::ToSexagesimal( dec, SexagesimalConversionOptions( 3/*items*/,
                                                                       3/*precision*/,
                                                                       true/*sign*/,
                                                                       0/*width*/,
                                                                       ' '/*separator*/ ) );
   return String().Format( "%+.9f", dec );
}

String APASSInterface::DistToString( double dist ) const
{
   if ( m_showCompoundAngles )
      return String::ToSexagesimal( dist, SexagesimalConversionOptions( 3/*items*/,
                                                                        3/*precision*/,
                                                                        false/*sign*/,
                                                                        0/*width*/,
                                                                        ' '/*separator*/ ) );
   return String().Format( "%.9f", dist );
}

// ----------------------------------------------------------------------------

void APASSInterface::e_EditCompleted( Edit& sender )
{
   String text = sender.Text().Trimmed();

   if ( sender == GUI->CenterRA_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            double deg = text.SexagesimalToDouble( ' '/*separator*/ );
            if ( m_raInTimeUnits )
               deg *= 15;
            while ( deg < 0 )
               deg += 360;
            while ( deg > 360 )
               deg -= 360;
            m_instance.p_searchData.centerRA = deg;
         }
         ERROR_HANDLER
      }
      sender.SetText( RAToString( m_instance.p_searchData.centerRA ) );
   }
   else if ( sender == GUI->CenterDec_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            m_instance.p_searchData.centerDec = Range( text.SexagesimalToDouble( ' '/*separator*/ ), -90.0, +90.0 );
         }
         ERROR_HANDLER
      }
      sender.SetText( DecToString( m_instance.p_searchData.centerDec ) );
   }
   else if ( sender == GUI->Radius_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            m_instance.p_searchData.radius = Range( text.SexagesimalToDouble( ' '/*separator*/ ), 0.0, 45.0 );
         }
         ERROR_HANDLER
      }
      sender.SetText( DistToString( m_instance.p_searchData.radius ) );
   }
   else if ( sender == GUI->RequiredFlags_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            m_instance.p_searchData.requiredFlags = text.ToUInt( 16 );
         }
         ERROR_HANDLER
      }
      UpdateControls();
   }
   else if ( sender == GUI->InclusionFlags_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            m_instance.p_searchData.inclusionFlags = text.ToUInt( 16 );
         }
         ERROR_HANDLER
      }
      UpdateControls();
   }
   else if ( sender == GUI->ExclusionFlags_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            m_instance.p_searchData.exclusionFlags = text.ToUInt( 16 );
         }
         ERROR_HANDLER
      }
      UpdateControls();
   }
   else if ( sender == GUI->OutputFilePath_Edit )
   {
      sender.SetText( m_instance.p_outputFilePath = text );
   }
}

// ----------------------------------------------------------------------------

void APASSInterface::e_EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->MagnitudeLow_NumericEdit )
      m_instance.p_searchData.magnitudeLow = value;
   else if ( sender == GUI->MagnitudeHigh_NumericEdit )
      m_instance.p_searchData.magnitudeHigh = value;
   else if ( sender == GUI->SourceLimit_NumericEdit )
      m_instance.p_searchData.sourceLimit = uint32( value );
}

// ----------------------------------------------------------------------------

void APASSInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->DataRelease_ComboBox )
      m_instance.p_dataRelease = itemIndex;
   else if ( sender == GUI->TextFormat_ComboBox )
      m_instance.p_textFormat = itemIndex;
   else if ( sender == GUI->TextHeaders_ComboBox )
      m_instance.p_textHeaders = itemIndex;
   else if ( sender == GUI->SortBy_ComboBox )
      m_instance.p_sortBy = itemIndex;
}

// ----------------------------------------------------------------------------

void APASSInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->ShowCompoundAngles_CheckBox  )
   {
      m_showCompoundAngles = checked;
      UpdateControls();
   }
   else if ( sender == GUI->RAInTimeUnits_CheckBox  )
   {
      m_raInTimeUnits = checked;
      UpdateControls();
   }
   else if ( sender == GUI->SearchCoordinates_Button )
   {
      if ( m_searchDialog == nullptr )
      {
         m_searchDialog = new OnlineObjectSearchDialog;
         m_searchDialog->SetWindowTitle( "Online Coordinate Search" );
      }
      if ( m_searchDialog->Execute() )
         if ( m_searchDialog->IsValid() )
         {
            m_instance.p_searchData.centerRA = m_searchDialog->RA();
            m_instance.p_searchData.centerDec = m_searchDialog->Dec();
            UpdateControls();
         }
   }
   else if ( sender == GUI->SourceLimitReset_ToolButton )
   {
      m_instance.p_searchData.sourceLimit = uint32_max;
      UpdateControls();
   }
   else if ( sender == GUI->GenerateTextOutput_CheckBox )
   {
      m_instance.p_generateTextOutput = checked;
      UpdateControls();
   }
   else if ( sender == GUI->RequiredFlagsClear_ToolButton )
   {
      m_instance.p_searchData.requiredFlags = 0;
      UpdateControls();
   }
   else if ( sender == GUI->RequiredFlags_ToolButton )
   {
      APASSFlagsSelectionDialog d( m_instance.p_searchData.requiredFlags );
      d.SetWindowTitle( "Required Flags" );
      if ( d.Execute() )
      {
         m_instance.p_searchData.requiredFlags = d.flags;
         UpdateControls();
      }
   }
   else if ( sender == GUI->InclusionFlagsClear_ToolButton )
   {
      m_instance.p_searchData.inclusionFlags = 0;
      UpdateControls();
   }
   else if ( sender == GUI->InclusionFlags_ToolButton )
   {
      APASSFlagsSelectionDialog d( m_instance.p_searchData.inclusionFlags );
      d.SetWindowTitle( "Inclusion Flags" );
      if ( d.Execute() )
      {
         m_instance.p_searchData.inclusionFlags = d.flags;
         UpdateControls();
      }
   }
   else if ( sender == GUI->ExclusionFlagsClear_ToolButton )
   {
      m_instance.p_searchData.exclusionFlags = 0;
      UpdateControls();
   }
   else if ( sender == GUI->ExclusionFlags_ToolButton )
   {
      APASSFlagsSelectionDialog d( m_instance.p_searchData.exclusionFlags );
      d.SetWindowTitle( "Exclusion Flags" );
      if ( d.Execute() )
      {
         m_instance.p_searchData.exclusionFlags = d.flags;
         UpdateControls();
      }
   }
   else if ( sender == GUI->OutputFilePath_ToolButton )
   {
      SaveFileDialog d;
      d.SetCaption( "APASS: Output CSV File" );
      d.SetFilters( FileDialog::filter_list() << FileFilter( "Any Files", "*" )
                                              << FileFilter( "Plain Text Files", ".txt" )
                                              << FileFilter( "CSV Files", ".csv" ) );
      d.EnableOverwritePrompt();
      if ( d.Execute() )
      {
         m_instance.p_outputFilePath = d.FileName();
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

APASSInterface::GUIData::GUIData( APASSInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( "Center right ascension:" );
   int editWidth1 = fnt.Width( "999 99 99.999 mm" );
   int editWidth2 = fnt.Width( "99.999 mm" );
   int editWidth3 = fnt.Width( "DDDDDDDD mm" );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   SearchParameters_SectionBar.SetTitle( "Search Parameters" );
   SearchParameters_SectionBar.SetSection( SearchParameters_Control );
   SearchParameters_SectionBar.SetToolTip( "<p>Parameters of the APASS  database search operation.</p>" );

   //

   const char* dataReleaseToolTip = "<p>You can choose between APASS data releases 9 and 10 (APASS DR9 and DR10). "
      "Both catalogs are available in XPSD format and are fully supported by this implementation.</p>";

   DataRelease_Label.SetText( "Data release:" );
   DataRelease_Label.SetFixedWidth( labelWidth1 );
   DataRelease_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DataRelease_Label.SetToolTip( dataReleaseToolTip );

   DataRelease_ComboBox.AddItem( "APASS DR9" );
   DataRelease_ComboBox.AddItem( "APASS DR10" );
   DataRelease_ComboBox.SetToolTip( dataReleaseToolTip );
   DataRelease_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&APASSInterface::e_ItemSelected, w );

   DataRelease_Sizer.SetSpacing( 4 );
   DataRelease_Sizer.Add( DataRelease_Label );
   DataRelease_Sizer.Add( DataRelease_ComboBox );
   DataRelease_Sizer.AddStretch();

   //

   const char* raToolTip = "<p>Right ascension equatorial coordinate of the center of the search region.</p>"
      "<p>You can write an explicit right ascension coordinate as a space-separated list of hours (or degrees, if <i>R.A. "
      "in time units</i> is unchecked), (arc)minutes and (arc)seconds. The (arc)minutes and (arc)seconds items are optional, "
      "and you can specify a fractional part for the last item. Alternatively, you can search for object coordinates online "
      "by clicking the <i>Search Coordinates</i> button.</p>";

   CenterRA_Label.SetText( "Center right ascension:" );
   CenterRA_Label.SetFixedWidth( labelWidth1 );
   CenterRA_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CenterRA_Label.SetToolTip( raToolTip );

   CenterRA_Edit.SetToolTip( raToolTip );
   CenterRA_Edit.SetMinWidth( editWidth1 );
   CenterRA_Edit.OnEditCompleted( (Edit::edit_event_handler)&APASSInterface::e_EditCompleted, w );

   CenterRA_Sizer.SetSpacing( 4 );
   CenterRA_Sizer.Add( CenterRA_Label );
   CenterRA_Sizer.Add( CenterRA_Edit, 100 );
   CenterRA_Sizer.AddStretch();

   //

   const char* decToolTip = "<p>Declination equatorial coordinate of the center of the search region.</p>"
      "<p>You can write an explicit declination coordinate as a space-separated list of degrees, arcminutes and arcseconds. "
      "The arcminutes and arcseconds items are optional, and you can specify a fractional part for the last item. A plus or minus "
      "sign character is optional before the first degrees item, and obviously necessary for Southern declinations. "
      "Alternatively, you can search for object coordinates online by clicking the <i>Search Coordinates</i> button.</p>";

   CenterDec_Label.SetText( "Center declination:" );
   CenterDec_Label.SetFixedWidth( labelWidth1 );
   CenterDec_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CenterDec_Label.SetToolTip( decToolTip );

   CenterDec_Edit.SetToolTip( decToolTip );
   CenterDec_Edit.SetMinWidth( editWidth1 );
   CenterDec_Edit.OnEditCompleted( (Edit::edit_event_handler)&APASSInterface::e_EditCompleted, w );

   CenterDec_Sizer.SetSpacing( 4 );
   CenterDec_Sizer.Add( CenterDec_Label );
   CenterDec_Sizer.Add( CenterDec_Edit, 100 );
   CenterDec_Sizer.AddStretch();

   //

   ShowCompoundAngles_CheckBox.SetText( "Show compound angles" );
   ShowCompoundAngles_CheckBox.SetToolTip( "<p>If enabled, coordinates and angular distances will be shown as compound angular "
   "quantities (degrees/hours, (arc)minutes, and (arc)seconds). If disabled, coordinates will be shown as scalars (degrees or "
   "hours with decimals).</p>" );
   ShowCompoundAngles_CheckBox.OnClick( (Button::click_event_handler)&APASSInterface::e_Click, w );

   ShowCompoundAngles_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowCompoundAngles_Sizer.Add( ShowCompoundAngles_CheckBox );
   ShowCompoundAngles_Sizer.AddStretch();

   //

   RAInTimeUnits_CheckBox.SetText( "R.A. in time units" );
   RAInTimeUnits_CheckBox.SetToolTip( "<p>When enabled, the right ascension coordinate will be shown expressed in hours, "
      "minutes, and seconds. When disabled, the right ascension will be expressed in degrees, arcminutes and arcseconds.</p>" );
   RAInTimeUnits_CheckBox.OnClick( (Button::click_event_handler)&APASSInterface::e_Click, w );

   RAInTimeUnits_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   RAInTimeUnits_Sizer.Add( RAInTimeUnits_CheckBox );
   RAInTimeUnits_Sizer.AddStretch();

   //

   const char* radiusToolTip = "<p>Search radius in degrees. Only sources at angular distances smaller than or equal "
      "to this value will be included in the search result.</p>"
      "<p>You can write an explicit angular distance as a space-separated list of degrees, arcminutes and arcseconds. "
      "The arcminutes and arcseconds items are optional, and you can specify a fractional part for the last item.</p>";

   Radius_Label.SetText( "Search radius:" );
   Radius_Label.SetFixedWidth( labelWidth1 );
   Radius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Radius_Label.SetToolTip( radiusToolTip );

   Radius_Edit.SetToolTip( radiusToolTip );
   Radius_Edit.SetMinWidth( editWidth1 );
   Radius_Edit.OnEditCompleted( (Edit::edit_event_handler)&APASSInterface::e_EditCompleted, w );

   Radius_Sizer.SetSpacing( 4 );
   Radius_Sizer.Add( Radius_Label );
   Radius_Sizer.Add( Radius_Edit, 100 );
   Radius_Sizer.AddStretch();

   //

   MagnitudeLow_NumericEdit.SetReal();
   MagnitudeLow_NumericEdit.SetPrecision( 3 );
   MagnitudeLow_NumericEdit.EnableFixedPrecision();
   MagnitudeLow_NumericEdit.SetRange( -1.5, 26 );
   MagnitudeLow_NumericEdit.label.SetText( "Low magnitude limit:" );
   MagnitudeLow_NumericEdit.label.SetFixedWidth( labelWidth1 );
   MagnitudeLow_NumericEdit.edit.SetFixedWidth( editWidth2 );
   MagnitudeLow_NumericEdit.sizer.AddStretch();
   MagnitudeLow_NumericEdit.SetToolTip( "<p>Lower bound of the search magnitude range. Only sources with G magnitudes "
      "greater than this value will be included in the search result.</p>" );
   MagnitudeLow_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&APASSInterface::e_EditValueUpdated, w );

   //

   MagnitudeHigh_NumericEdit.SetReal();
   MagnitudeHigh_NumericEdit.SetPrecision( 3 );
   MagnitudeHigh_NumericEdit.EnableFixedPrecision();
   MagnitudeHigh_NumericEdit.SetRange( -1.5, 26 );
   MagnitudeHigh_NumericEdit.label.SetText( "High magnitude limit:" );
   MagnitudeHigh_NumericEdit.label.SetFixedWidth( labelWidth1 );
   MagnitudeHigh_NumericEdit.edit.SetFixedWidth( editWidth2 );
   MagnitudeHigh_NumericEdit.sizer.AddStretch();
   MagnitudeHigh_NumericEdit.SetToolTip( "<p>Upper bound of the search magnitude range. Only sources with G magnitudes "
      "smaller than or equal to this value will be included in the search result.</p>" );
   MagnitudeHigh_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&APASSInterface::e_EditValueUpdated, w );

   //

   const char* requiredFlagsToolTip = "<p>If non-zero, only sources with <i>all</i> of these flags set will be included "
      "in the search result.</p>";

   RequiredFlags_Label.SetText( "Required flags:" );
   RequiredFlags_Label.SetFixedWidth( labelWidth1 );
   RequiredFlags_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RequiredFlags_Label.SetToolTip( requiredFlagsToolTip );

   RequiredFlags_Edit.SetToolTip( requiredFlagsToolTip );
   RequiredFlags_Edit.SetMinWidth( editWidth3 );
   RequiredFlags_Edit.OnEditCompleted( (Edit::edit_event_handler)&APASSInterface::e_EditCompleted, w );

   RequiredFlagsClear_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   RequiredFlagsClear_ToolButton.SetScaledFixedSize( 20, 20 );
   RequiredFlagsClear_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RequiredFlagsClear_ToolButton.SetToolTip( "<p>Clear required flags.</p>" );
   RequiredFlagsClear_ToolButton.OnClick( (ToolButton::click_event_handler)&APASSInterface::e_Click, w );

   RequiredFlags_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/control-multiple-selection.png" ) ) );
   RequiredFlags_ToolButton.SetScaledFixedSize( 20, 20 );
   RequiredFlags_ToolButton.SetToolTip( "<p>Define required flags.</p>" );
   RequiredFlags_ToolButton.OnClick( (Button::click_event_handler)&APASSInterface::e_Click, w );

   RequiredFlags_Sizer.SetSpacing( 4 );
   RequiredFlags_Sizer.Add( RequiredFlags_Label );
   RequiredFlags_Sizer.Add( RequiredFlags_Edit, 100 );
   RequiredFlags_Sizer.Add( RequiredFlagsClear_ToolButton );
   RequiredFlags_Sizer.Add( RequiredFlags_ToolButton );
   RequiredFlags_Sizer.AddStretch();

   //

   const char* inclusionFlagsToolTip = "<p>If non-zero, sources with <i>any</i> of these flags set will be included "
      "in the search result.</p>";

   InclusionFlags_Label.SetText( "Inclusion flags:" );
   InclusionFlags_Label.SetFixedWidth( labelWidth1 );
   InclusionFlags_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InclusionFlags_Label.SetToolTip( inclusionFlagsToolTip );

   InclusionFlags_Edit.SetToolTip( inclusionFlagsToolTip );
   InclusionFlags_Edit.SetMinWidth( editWidth3 );
   InclusionFlags_Edit.OnEditCompleted( (Edit::edit_event_handler)&APASSInterface::e_EditCompleted, w );

   InclusionFlagsClear_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   InclusionFlagsClear_ToolButton.SetScaledFixedSize( 20, 20 );
   InclusionFlagsClear_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   InclusionFlagsClear_ToolButton.SetToolTip( "<p>Clear inclusion flags.</p>" );
   InclusionFlagsClear_ToolButton.OnClick( (ToolButton::click_event_handler)&APASSInterface::e_Click, w );

   InclusionFlags_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/control-multiple-selection.png" ) ) );
   InclusionFlags_ToolButton.SetScaledFixedSize( 20, 20 );
   InclusionFlags_ToolButton.SetToolTip( "<p>Define inclusion flags.</p>" );
   InclusionFlags_ToolButton.OnClick( (Button::click_event_handler)&APASSInterface::e_Click, w );

   InclusionFlags_Sizer.SetSpacing( 4 );
   InclusionFlags_Sizer.Add( InclusionFlags_Label );
   InclusionFlags_Sizer.Add( InclusionFlags_Edit, 100 );
   InclusionFlags_Sizer.Add( InclusionFlagsClear_ToolButton );
   InclusionFlags_Sizer.Add( InclusionFlags_ToolButton );
   InclusionFlags_Sizer.AddStretch();

   //

   const char* exclusionFlagsToolTip = "<p>If non-zero, sources with <i>any</i> of these flags set will <i>not</i> be included "
      "in the search result.</p>";

   ExclusionFlags_Label.SetText( "Exclusion flags:" );
   ExclusionFlags_Label.SetFixedWidth( labelWidth1 );
   ExclusionFlags_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ExclusionFlags_Label.SetToolTip( exclusionFlagsToolTip );

   ExclusionFlags_Edit.SetToolTip( exclusionFlagsToolTip );
   ExclusionFlags_Edit.SetMinWidth( editWidth3 );
   ExclusionFlags_Edit.OnEditCompleted( (Edit::edit_event_handler)&APASSInterface::e_EditCompleted, w );

   ExclusionFlagsClear_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   ExclusionFlagsClear_ToolButton.SetScaledFixedSize( 20, 20 );
   ExclusionFlagsClear_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExclusionFlagsClear_ToolButton.SetToolTip( "<p>Clear exclusion flags.</p>" );
   ExclusionFlagsClear_ToolButton.OnClick( (ToolButton::click_event_handler)&APASSInterface::e_Click, w );

   ExclusionFlags_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/control-multiple-selection.png" ) ) );
   ExclusionFlags_ToolButton.SetScaledFixedSize( 20, 20 );
   ExclusionFlags_ToolButton.SetToolTip( "<p>Define exclusion flags.</p>" );
   ExclusionFlags_ToolButton.OnClick( (Button::click_event_handler)&APASSInterface::e_Click, w );

   ExclusionFlags_Sizer.SetSpacing( 4 );
   ExclusionFlags_Sizer.Add( ExclusionFlags_Label );
   ExclusionFlags_Sizer.Add( ExclusionFlags_Edit, 100 );
   ExclusionFlags_Sizer.Add( ExclusionFlagsClear_ToolButton );
   ExclusionFlags_Sizer.Add( ExclusionFlags_ToolButton );
   ExclusionFlags_Sizer.AddStretch();

   //

   SourceLimit_NumericEdit.SetInteger();
   SourceLimit_NumericEdit.SetRange( 0, uint32_max );
   SourceLimit_NumericEdit.label.SetText( "Source count limit:" );
   SourceLimit_NumericEdit.label.SetFixedWidth( labelWidth1 );
   SourceLimit_NumericEdit.edit.SetFixedWidth( editWidth3 );
   SourceLimit_NumericEdit.sizer.Add( SourceLimitReset_ToolButton );
   SourceLimit_NumericEdit.sizer.AddStretch();
   SourceLimit_NumericEdit.SetToolTip( "<p>The maximum number of sources that can be included in the search result.</p>" );
   SourceLimit_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&APASSInterface::e_EditValueUpdated, w );

   SourceLimitReset_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   SourceLimitReset_ToolButton.SetScaledFixedSize( 20, 20 );
   SourceLimitReset_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   SourceLimitReset_ToolButton.SetToolTip( "<p>Reset source count limit to perform an unlimited search.</p>" );
   SourceLimitReset_ToolButton.OnClick( (ToolButton::click_event_handler)&APASSInterface::e_Click, w );

   //

   SearchParametersLeft_Sizer.SetSpacing( 4 );
   SearchParametersLeft_Sizer.Add( CenterRA_Sizer );
   SearchParametersLeft_Sizer.Add( CenterDec_Sizer );
   SearchParametersLeft_Sizer.Add( Radius_Sizer );
   SearchParametersLeft_Sizer.Add( ShowCompoundAngles_Sizer );
   SearchParametersLeft_Sizer.Add( RAInTimeUnits_Sizer );
   SearchParametersLeft_Sizer.Add( MagnitudeLow_NumericEdit );
   SearchParametersLeft_Sizer.Add( MagnitudeHigh_NumericEdit );
   SearchParametersLeft_Sizer.Add( RequiredFlags_Sizer );
   SearchParametersLeft_Sizer.Add( InclusionFlags_Sizer );
   SearchParametersLeft_Sizer.Add( ExclusionFlags_Sizer );
   SearchParametersLeft_Sizer.Add( SourceLimit_NumericEdit );

   //

   SearchCoordinates_Button.SetText( "Search Coordinates" );
   SearchCoordinates_Button.SetIcon( w.ScaledResource( ":/icons/find.png" ) );
   SearchCoordinates_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   SearchCoordinates_Button.SetToolTip( "<p>Open the Online Coordinate Search dialog.</p>" );
   SearchCoordinates_Button.OnClick( (Button::click_event_handler)&APASSInterface::e_Click, w );

   //

   SearchParametersRight_Sizer.Add( SearchCoordinates_Button );
   SearchParametersRight_Sizer.AddStretch();

   //

   SearchParametersBottom_Sizer.SetSpacing( 32 );
   SearchParametersBottom_Sizer.Add( SearchParametersLeft_Sizer, 100 );
   SearchParametersBottom_Sizer.Add( SearchParametersRight_Sizer );

   //

   SearchParameters_Sizer.SetSpacing( 4 );
   SearchParameters_Sizer.Add( DataRelease_Sizer );
   SearchParameters_Sizer.Add( SearchParametersBottom_Sizer );

   SearchParameters_Control.SetSizer( SearchParameters_Sizer );

   //

   OutputParameters_SectionBar.SetTitle( "Output Parameters" );
   OutputParameters_SectionBar.SetSection( OutputParameters_Control );

   //

   const char* sortByToolTip = "<p>This is the data item used to sort output point sources in ascensing order.</p>";

   SortBy_Label.SetText( "Sort by:" );
   SortBy_Label.SetFixedWidth( labelWidth1 );
   SortBy_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SortBy_Label.SetToolTip( sortByToolTip );

   SortBy_ComboBox.AddItem( "Don't sort" );
   SortBy_ComboBox.AddItem( "Right Ascension" );
   SortBy_ComboBox.AddItem( "Declination" );
   SortBy_ComboBox.AddItem( "Johnson V magnitude" );
   SortBy_ComboBox.AddItem( "Johnson B magnitude" );
   SortBy_ComboBox.AddItem( "Sloan g' magnitude" );
   SortBy_ComboBox.AddItem( "Sloan r' magnitude" );
   SortBy_ComboBox.AddItem( "Sloan i' magnitude" );
   SortBy_ComboBox.AddItem( "Sloan z_s magnitude" );
   SortBy_ComboBox.SetToolTip( sortByToolTip );
   SortBy_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&APASSInterface::e_ItemSelected, w );

   SortBy_Sizer.SetSpacing( 4 );
   SortBy_Sizer.Add( SortBy_Label );
   SortBy_Sizer.Add( SortBy_ComboBox );
   SortBy_Sizer.AddStretch();

   //

   GenerateTextOutput_CheckBox.SetText( "Generate text output" );
   GenerateTextOutput_CheckBox.SetToolTip( "<p>When enabled, the search operation will generate an output "
      "plain text file including available data for all sources fulfilling the specified search criteria.</p>" );
   GenerateTextOutput_CheckBox.OnClick( (Button::click_event_handler)&APASSInterface::e_Click, w );

   GenerateTextOutput_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateTextOutput_Sizer.Add( GenerateTextOutput_CheckBox );

   //

   const char* textFormatToolTip = "<p>The format of generated output text files.</p>"
      "<p>CSV will generate standard comma-separated files (CSV format).</p>"
      "<p>Tabular formats can be selected to write coordinates either as scalars in degrees, or using compound "
      "angular representations where right ascension is represented as separate hours, minutes and seconds, and "
      "declination as degrees, arcminutes and arcseconds.</p>"
      "<p>In all cases output text files can contain optional headers, which you can control with the "
      "<i>Text headers</i> option below.</p>";

   TextFormat_Label.SetText( "Text format:" );
   TextFormat_Label.SetFixedWidth( labelWidth1 );
   TextFormat_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   TextFormat_Label.SetToolTip( textFormatToolTip );

   TextFormat_ComboBox.AddItem( "CSV" );
   TextFormat_ComboBox.AddItem( "Tabular, scalar coordinates" );
   TextFormat_ComboBox.AddItem( "Tabular, compound coordinates" );
   TextFormat_ComboBox.SetToolTip( textFormatToolTip );
   TextFormat_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&APASSInterface::e_ItemSelected, w );

   TextFormat_Sizer.SetSpacing( 4 );
   TextFormat_Sizer.Add( TextFormat_Label );
   TextFormat_Sizer.Add( TextFormat_ComboBox );
   TextFormat_Sizer.AddStretch();

   //

   const char* textHeadersToolTip = "<p></p>";

   TextHeaders_Label.SetText( "Text headers:" );
   TextHeaders_Label.SetFixedWidth( labelWidth1 );
   TextHeaders_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   TextHeaders_Label.SetToolTip( textHeadersToolTip );

   TextHeaders_ComboBox.AddItem( "None" );
   TextHeaders_ComboBox.AddItem( "Table columns" );
   TextHeaders_ComboBox.AddItem( "Search parameters" );
   TextHeaders_ComboBox.AddItem( "Search parameters and table columns" );
   TextHeaders_ComboBox.SetToolTip( textHeadersToolTip );
   TextHeaders_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&APASSInterface::e_ItemSelected, w );

   TextHeaders_Sizer.SetSpacing( 4 );
   TextHeaders_Sizer.Add( TextHeaders_Label );
   TextHeaders_Sizer.Add( TextHeaders_ComboBox );
   TextHeaders_Sizer.AddStretch();

   //

   const char* outputFilePathToolTip = "<p>Output file path.</p>"
      "<p>When the <i>Generate text output</i> option is enabled, the search process will write its output "
      "data to the specified file in a plain text format.</p>";

   OutputFilePath_Label.SetText( "Output file:" );
   OutputFilePath_Label.SetFixedWidth( labelWidth1 );
   OutputFilePath_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputFilePath_Label.SetToolTip( outputFilePathToolTip );

   OutputFilePath_Edit.SetToolTip( outputFilePathToolTip );
   OutputFilePath_Edit.OnEditCompleted( (Edit::edit_event_handler)&APASSInterface::e_EditCompleted, w );

   OutputFilePath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select-file.png" ) ) );
   OutputFilePath_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputFilePath_ToolButton.SetToolTip( "<p>Select the output file.</p>" );
   OutputFilePath_ToolButton.OnClick( (Button::click_event_handler)&APASSInterface::e_Click, w );

   OutputFilePath_Sizer.SetSpacing( 4 );
   OutputFilePath_Sizer.Add( OutputFilePath_Label );
   OutputFilePath_Sizer.Add( OutputFilePath_Edit, 100 );
   OutputFilePath_Sizer.Add( OutputFilePath_ToolButton );

   //

   OutputParameters_Sizer.SetSpacing( 4 );
   OutputParameters_Sizer.Add( SortBy_Sizer );
   OutputParameters_Sizer.Add( GenerateTextOutput_Sizer );
   OutputParameters_Sizer.Add( TextFormat_Sizer );
   OutputParameters_Sizer.Add( TextHeaders_Sizer );
   OutputParameters_Sizer.Add( OutputFilePath_Sizer );

   OutputParameters_Control.SetSizer( OutputParameters_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( SearchParameters_SectionBar );
   Global_Sizer.Add( SearchParameters_Control );
   Global_Sizer.Add( OutputParameters_SectionBar );
   Global_Sizer.Add( OutputParameters_Control );

   //

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF APASSInterface.cpp - Released 2021-04-09T19:41:48Z
