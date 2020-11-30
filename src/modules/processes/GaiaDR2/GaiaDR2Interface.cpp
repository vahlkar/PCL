//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.3
// ----------------------------------------------------------------------------
// Standard GaiaDR2 Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// GaiaDR2Interface.cpp - Released 2020-11-27T11:02:59Z
// ----------------------------------------------------------------------------
// This file is part of the standard GaiaDR2 PixInsight module.
//
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "GaiaDR2Interface.h"
#include "GaiaDR2Process.h"
#include "GaiaDR2Parameters.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/OnlineObjectSearchDialog.h>
#include <pcl/TreeBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class GaiaDR2FlagsSelectionDialog : public Dialog
{
public:

   uint32 flags = 0;

   GaiaDR2FlagsSelectionDialog( uint32 );

private:

   VerticalSizer  Global_Sizer;
      CheckBox       NoPM_CheckBox;
      CheckBox       NoGBPMag_CheckBox;
      CheckBox       NoGRPMag_CheckBox;
      CheckBox       GoodRA_CheckBox;
      CheckBox       GoodDec_CheckBox;
      CheckBox       GoodPMRA_CheckBox;
      CheckBox       GoodPMDec_CheckBox;
      CheckBox       GoodParx_CheckBox;
      CheckBox       BadRA_CheckBox;
      CheckBox       BadDec_CheckBox;
      CheckBox       BadPMRA_CheckBox;
      CheckBox       BadPMDec_CheckBox;
      CheckBox       BadParx_CheckBox;
      CheckBox       GoodGMag_CheckBox;
      CheckBox       GoodGBPMag_CheckBox;
      CheckBox       GoodGRPMag_CheckBox;
      CheckBox       BadGMag_CheckBox;
      CheckBox       BadGBPMag_CheckBox;
      CheckBox       BadGRPMag_CheckBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void e_Click( Button& sender, bool checked );
   void e_Return( Dialog& sender, int retVal );
};

GaiaDR2FlagsSelectionDialog::GaiaDR2FlagsSelectionDialog( uint32 a_flags )
   : flags( a_flags )
{
   NoPM_CheckBox.SetText( "No proper motions" );
   NoPM_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No proper motions and parallax available.</p>" );
   NoPM_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   NoGBPMag_CheckBox.SetText( "No GBP magnitude" );
   NoGBPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No G-BP magnitude available.</p>" );
   NoGBPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   NoGRPMag_CheckBox.SetText( "No GRP magnitude" );
   NoGRPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No G-RP magnitude available.</p>" );
   NoGRPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   GoodRA_CheckBox.SetText( "High-quality right ascension" );
   GoodRA_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of right ascension &lt; 1 mas.</p>" );
   GoodRA_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   GoodDec_CheckBox.SetText( "High quality declination" );
   GoodDec_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of declination &lt; 1 mas.</p>" );
   GoodDec_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   GoodPMRA_CheckBox.SetText( "High quality proper motion in R.A." );
   GoodPMRA_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of proper motion in right ascension &lt; 0.75 mas/year.</p>" );
   GoodPMRA_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   GoodPMDec_CheckBox.SetText( "High quality proper motion in declination" );
   GoodPMDec_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of proper motion in declination &lt; 0.75 mas/year.</p>" );
   GoodPMDec_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   GoodParx_CheckBox.SetText( "High quality parallax" );
   GoodParx_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of parallax &lt; 0.25 mas.</p>" );
   GoodParx_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   BadRA_CheckBox.SetText( "Low quality right ascension" );
   BadRA_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of right ascension &gt; 2 mas.</p>" );
   BadRA_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   BadDec_CheckBox.SetText( "Low quality declination" );
   BadDec_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of declination &gt; 2 mas.</p>" );
   BadDec_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   BadPMRA_CheckBox.SetText( "Low quality proper motion in R.A." );
   BadPMRA_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of proper motion in right ascension &gt; 1.5 mas/year.</p>" );
   BadPMRA_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   BadPMDec_CheckBox.SetText( "Low quality proper motion in declination" );
   BadPMDec_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of proper motion in declination &gt; 1.5 mas/year.</p>" );
   BadPMDec_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   BadParx_CheckBox.SetText( "Low quality parallax" );
   BadParx_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of parallax &gt; 0.65 mas.</p>" );
   BadParx_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   GoodGMag_CheckBox.SetText( "High quality G magnitude" );
   GoodGMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on G-band mean flux &lt; 1.5 e-/s.</p>" );
   GoodGMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   GoodGBPMag_CheckBox.SetText( "High quality GBP magnitude" );
   GoodGBPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on the integrated BP mean flux &lt; 10 e-/s.</p>" );
   GoodGBPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   GoodGRPMag_CheckBox.SetText( "High quality GRP magnitude" );
   GoodGRPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on the integrated RP mean flux &lt; 10 e-/s.</p>" );
   GoodGRPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   BadGMag_CheckBox.SetText( "Low quality G magnitude" );
   BadGMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on G-band mean flux &gt; 2.5 e-/s.</p>" );
   BadGMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   BadGBPMag_CheckBox.SetText( "Low quality GBP magnitude" );
   BadGBPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on the integrated BP mean flux &gt; 20 e-/s,<br/>or BP/RP excess factor &gt; 2.5.</p>" );
   BadGBPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   BadGRPMag_CheckBox.SetText( "Low quality GRP magnitude" );
   BadGRPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on the integrated RP mean flux &gt; 20 e-/s,<br/>or BP/RP excess factor &gt; 2.5.</p>" );
   BadGRPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&GaiaDR2FlagsSelectionDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( NoPM_CheckBox );
   Global_Sizer.Add( NoGBPMag_CheckBox );
   Global_Sizer.Add( NoGRPMag_CheckBox );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( GoodRA_CheckBox );
   Global_Sizer.Add( GoodDec_CheckBox );
   Global_Sizer.Add( GoodPMRA_CheckBox );
   Global_Sizer.Add( GoodPMDec_CheckBox );
   Global_Sizer.Add( GoodParx_CheckBox );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( BadRA_CheckBox );
   Global_Sizer.Add( BadDec_CheckBox );
   Global_Sizer.Add( BadPMRA_CheckBox );
   Global_Sizer.Add( BadPMDec_CheckBox );
   Global_Sizer.Add( BadParx_CheckBox );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( GoodGMag_CheckBox );
   Global_Sizer.Add( GoodGBPMag_CheckBox );
   Global_Sizer.Add( GoodGRPMag_CheckBox );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( BadGMag_CheckBox );
   Global_Sizer.Add( BadGBPMag_CheckBox );
   Global_Sizer.Add( BadGRPMag_CheckBox );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "GaiaDR2 Source Flags" );

   NoPM_CheckBox.SetChecked(       (flags & GaiaDR2StarFlag::NoPM) != 0 );
   NoGBPMag_CheckBox.SetChecked(   (flags & GaiaDR2StarFlag::NoGBPMag) != 0 );
   NoGRPMag_CheckBox.SetChecked(   (flags & GaiaDR2StarFlag::NoGRPMag) != 0 );
   GoodRA_CheckBox.SetChecked(     (flags & GaiaDR2StarFlag::GoodRA) != 0 );
   GoodDec_CheckBox.SetChecked(    (flags & GaiaDR2StarFlag::GoodDec) != 0 );
   GoodPMRA_CheckBox.SetChecked(   (flags & GaiaDR2StarFlag::GoodPMRA) != 0 );
   GoodPMDec_CheckBox.SetChecked(  (flags & GaiaDR2StarFlag::GoodPMDec) != 0 );
   GoodParx_CheckBox.SetChecked(   (flags & GaiaDR2StarFlag::GoodParx) != 0 );
   BadRA_CheckBox.SetChecked(      (flags & GaiaDR2StarFlag::BadRA) != 0 );
   BadDec_CheckBox.SetChecked(     (flags & GaiaDR2StarFlag::BadDec) != 0 );
   BadPMRA_CheckBox.SetChecked(    (flags & GaiaDR2StarFlag::BadPMRA) != 0 );
   BadPMDec_CheckBox.SetChecked(   (flags & GaiaDR2StarFlag::BadPMDec) != 0 );
   BadParx_CheckBox.SetChecked(    (flags & GaiaDR2StarFlag::BadParx) != 0 );
   GoodGMag_CheckBox.SetChecked(   (flags & GaiaDR2StarFlag::GoodGMag) != 0 );
   GoodGBPMag_CheckBox.SetChecked( (flags & GaiaDR2StarFlag::GoodGBPMag) != 0 );
   GoodGRPMag_CheckBox.SetChecked( (flags & GaiaDR2StarFlag::GoodGRPMag) != 0 );
   BadGMag_CheckBox.SetChecked(    (flags & GaiaDR2StarFlag::BadGMag) != 0 );
   BadGBPMag_CheckBox.SetChecked(  (flags & GaiaDR2StarFlag::BadGBPMag) != 0 );
   BadGRPMag_CheckBox.SetChecked(  (flags & GaiaDR2StarFlag::BadGRPMag) != 0 );
}

void GaiaDR2FlagsSelectionDialog::e_Click( Button& sender, bool checked )
{
        if ( sender == NoPM_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::NoPM;
      else
         flags &= ~GaiaDR2StarFlag::NoPM;
   }
   else if ( sender == NoGBPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::NoGBPMag;
      else
         flags &= ~GaiaDR2StarFlag::NoGBPMag;
   }
   else if ( sender == NoGRPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::NoGRPMag;
      else
         flags &= ~GaiaDR2StarFlag::NoGRPMag;
   }
   else if ( sender == GoodRA_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::GoodRA;
      else
         flags &= ~GaiaDR2StarFlag::GoodRA;
   }
   else if ( sender == GoodDec_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::GoodDec;
      else
         flags &= ~GaiaDR2StarFlag::GoodDec;
   }
   else if ( sender == GoodPMRA_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::GoodPMRA;
      else
         flags &= ~GaiaDR2StarFlag::GoodPMRA;
   }
   else if ( sender == GoodPMDec_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::GoodPMDec;
      else
         flags &= ~GaiaDR2StarFlag::GoodPMDec;
   }
   else if ( sender == GoodParx_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::GoodParx;
      else
         flags &= ~GaiaDR2StarFlag::GoodParx;
   }
   else if ( sender == BadRA_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::BadRA;
      else
         flags &= ~GaiaDR2StarFlag::BadRA;
   }
   else if ( sender == BadDec_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::BadDec;
      else
         flags &= ~GaiaDR2StarFlag::BadDec;
   }
   else if ( sender == BadPMRA_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::BadPMRA;
      else
         flags &= ~GaiaDR2StarFlag::BadPMRA;
   }
   else if ( sender == BadPMDec_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::BadPMDec;
      else
         flags &= ~GaiaDR2StarFlag::BadPMDec;
   }
   else if ( sender == BadParx_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::BadParx;
      else
         flags &= ~GaiaDR2StarFlag::BadParx;
   }
   else if ( sender == GoodGMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::GoodGMag;
      else
         flags &= ~GaiaDR2StarFlag::GoodGMag;
   }
   else if ( sender == GoodGBPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::GoodGBPMag;
      else
         flags &= ~GaiaDR2StarFlag::GoodGBPMag;
   }
   else if ( sender == GoodGRPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::GoodGRPMag;
      else
         flags &= ~GaiaDR2StarFlag::GoodGRPMag;
   }
   else if ( sender == BadGMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::BadGMag;
      else
         flags &= ~GaiaDR2StarFlag::BadGMag;
   }
   else if ( sender == BadGBPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::BadGBPMag;
      else
         flags &= ~GaiaDR2StarFlag::BadGBPMag;
   }
   else if ( sender == BadGRPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaDR2StarFlag::BadGRPMag;
      else
         flags &= ~GaiaDR2StarFlag::BadGRPMag;
   }
   else if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

// ----------------------------------------------------------------------------

class GaiaDR2PreferencesDialog : public Dialog
{
public:

   GaiaDR2PreferencesDialog();

private:

   StringList m_databaseFilePaths;

   VerticalSizer     Global_Sizer;
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
   void e_Click( Button& sender, bool checked );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );
   void e_Return( Dialog& sender, int retVal );
};

GaiaDR2PreferencesDialog::GaiaDR2PreferencesDialog()
{
   pcl::Font fnt = Font();

   Title_Label.SetText( "Gaia DR2 Database Files" );
   Title_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   FilePaths_TreeBox.SetNumberOfColumns( 1 );
   FilePaths_TreeBox.HideHeader();
   FilePaths_TreeBox.DisableMultipleSelections();
   FilePaths_TreeBox.DisableRootDecoration();
   FilePaths_TreeBox.EnableAlternateRowColor();
   FilePaths_TreeBox.SetMinSize( 40*fnt.Width( "M" ), 24*fnt.Height() );
   FilePaths_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&GaiaDR2PreferencesDialog::e_NodeActivated, *this );
   FilePaths_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&GaiaDR2PreferencesDialog::e_FileDrag, *this );
   FilePaths_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&GaiaDR2PreferencesDialog::e_FileDrop, *this );

   SelectFiles_PushButton.SetText( "Select" );
   SelectFiles_PushButton.SetToolTip( "<p>Select a new set of database files.</p>" );
   SelectFiles_PushButton.OnClick( (Button::click_event_handler)&GaiaDR2PreferencesDialog::e_Click, *this );

   RemoveFile_PushButton.SetText( "Remove" );
   RemoveFile_PushButton.SetToolTip( "<p>Remove the selected database file from the list.</p>" );
   RemoveFile_PushButton.OnClick( (Button::click_event_handler)&GaiaDR2PreferencesDialog::e_Click, *this );

   ClearFiles_PushButton.SetText( "Clear" );
   ClearFiles_PushButton.SetToolTip( "<p>Clear the list of database files.</p>" );
   ClearFiles_PushButton.OnClick( (Button::click_event_handler)&GaiaDR2PreferencesDialog::e_Click, *this );

   FileButtons_Sizer.SetSpacing( 4 );
   FileButtons_Sizer.Add( SelectFiles_PushButton );
   FileButtons_Sizer.Add( RemoveFile_PushButton );
   FileButtons_Sizer.AddStretch();
   FileButtons_Sizer.Add( ClearFiles_PushButton );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&GaiaDR2PreferencesDialog::e_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&GaiaDR2PreferencesDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Title_Label );
   Global_Sizer.Add( FilePaths_TreeBox );
   Global_Sizer.Add( FileButtons_Sizer );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetMinSize();

   SetWindowTitle( "GaiaDR2 Preferences" );

   OnReturn( (pcl::Dialog::return_event_handler)&GaiaDR2PreferencesDialog::e_Return, *this );

   if ( !TheGaiaDR2Process->PreferencesLoaded() )
      TheGaiaDR2Process->LoadPreferences();

   m_databaseFilePaths = TheGaiaDR2Process->DatabaseFilePaths();

   UpdateControls();
}

void GaiaDR2PreferencesDialog::UpdateControls()
{
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

void GaiaDR2PreferencesDialog::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   OpenFileDialog d;
   d.SetCaption( "GaiaDR2: Select Database File" );
   d.DisableMultipleSelections();
   d.SetFilter( FileFilter( "XPSD Files", ".xpsd" ) );
   if ( d.Execute() )
      m_databaseFilePaths[sender.ChildIndex( &node )] = d.FileName();
}

void GaiaDR2PreferencesDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == SelectFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "GaiaDR2: Select Database Files" );
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

void GaiaDR2PreferencesDialog::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
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

void GaiaDR2PreferencesDialog::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
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

void GaiaDR2PreferencesDialog::e_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      TheGaiaDR2Process->SetDatabaseFilePaths( m_databaseFilePaths );
      if ( TheGaiaDR2Interface != nullptr )
         TheGaiaDR2Interface->UpdateControls();
   }
}

// ----------------------------------------------------------------------------

GaiaDR2Interface* TheGaiaDR2Interface = nullptr;

// ----------------------------------------------------------------------------

GaiaDR2Interface::GaiaDR2Interface()
   : m_instance( TheGaiaDR2Process )
{
   TheGaiaDR2Interface = this;
}

// ----------------------------------------------------------------------------

GaiaDR2Interface::~GaiaDR2Interface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString GaiaDR2Interface::Id() const
{
   return "GaiaDR2";
}

// ----------------------------------------------------------------------------

MetaProcess* GaiaDR2Interface::Process() const
{
   return TheGaiaDR2Process;
}

// ----------------------------------------------------------------------------

String GaiaDR2Interface::IconImageSVGFile() const
{
   return "@module_icons_dir/GaiaDR2.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures GaiaDR2Interface::Features() const
{
   return InterfaceFeature::DefaultGlobal | InterfaceFeature::PreferencesButton;
}

// ----------------------------------------------------------------------------

void GaiaDR2Interface::EditPreferences()
{
   GaiaDR2PreferencesDialog().Execute();
}

// ----------------------------------------------------------------------------

void GaiaDR2Interface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void GaiaDR2Interface::ResetInstance()
{
   GaiaDR2Instance defaultInstance( TheGaiaDR2Process );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool GaiaDR2Interface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "GaiaDR2" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheGaiaDR2Process;
}

// ----------------------------------------------------------------------------

ProcessImplementation* GaiaDR2Interface::NewProcess() const
{
   return new GaiaDR2Instance( m_instance );
}

// ----------------------------------------------------------------------------

bool GaiaDR2Interface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const GaiaDR2Instance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a GaiaDR2 instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool GaiaDR2Interface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool GaiaDR2Interface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void GaiaDR2Interface::UpdateControls()
{
   GUI->CenterRA_Edit.SetText( RAToString( m_instance.p_searchData.centerRA ) );

   GUI->CenterDec_Edit.SetText( DecToString( m_instance.p_searchData.centerDec ) );

   GUI->Radius_Edit.SetText( DistToString( m_instance.p_searchData.radius ) );

   GUI->ShowCompoundAngles_CheckBox.SetChecked( m_showCompoundAngles );

   GUI->RAInTimeUnits_CheckBox.SetChecked( m_raInTimeUnits );

   GUI->MagnitudeLow_NumericEdit.SetValue( m_instance.p_searchData.magnitudeLow );

   GUI->MagnitudeHigh_NumericEdit.SetValue( m_instance.p_searchData.magnitudeHigh );

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

String GaiaDR2Interface::RAToString( double ra ) const
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

String GaiaDR2Interface::DecToString( double dec ) const
{
   if ( m_showCompoundAngles )
      return String::ToSexagesimal( dec, SexagesimalConversionOptions( 3/*items*/,
                                                                       3/*precision*/,
                                                                       true/*sign*/,
                                                                       0/*width*/,
                                                                       ' '/*separator*/ ) );
   return String().Format( "%+.9f", dec );
}

String GaiaDR2Interface::DistToString( double dist ) const
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

void GaiaDR2Interface::e_EditCompleted( Edit& sender )
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

void GaiaDR2Interface::e_EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->MagnitudeLow_NumericEdit )
      m_instance.p_searchData.magnitudeLow = value;
   else if ( sender == GUI->MagnitudeHigh_NumericEdit )
      m_instance.p_searchData.magnitudeHigh = value;
   else if ( sender == GUI->SourceLimit_NumericEdit )
      m_instance.p_searchData.sourceLimit = uint32( value );
}

// ----------------------------------------------------------------------------

void GaiaDR2Interface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->TextFormat_ComboBox )
      m_instance.p_textFormat = itemIndex;
   else if ( sender == GUI->TextHeaders_ComboBox )
      m_instance.p_textHeaders = itemIndex;
   else if ( sender == GUI->SortBy_ComboBox )
      m_instance.p_sortBy = itemIndex;
}

// ----------------------------------------------------------------------------

void GaiaDR2Interface::e_Click( Button& sender, bool checked )
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
   else if ( sender == GUI->InclusionFlagsClear_ToolButton )
   {
      m_instance.p_searchData.inclusionFlags = 0;
      UpdateControls();
   }
   else if ( sender == GUI->InclusionFlags_ToolButton )
   {
      GaiaDR2FlagsSelectionDialog d( m_instance.p_searchData.inclusionFlags );
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
      GaiaDR2FlagsSelectionDialog d( m_instance.p_searchData.exclusionFlags );
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
      d.SetCaption( "GaiaDR2: Output CSV File" );
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

GaiaDR2Interface::GUIData::GUIData( GaiaDR2Interface& w )
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
   SearchParameters_SectionBar.SetToolTip( "<p>Parameters of the Gaia DR2 database search operation.</p>" );

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
   CenterRA_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaDR2Interface::e_EditCompleted, w );

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
   CenterDec_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaDR2Interface::e_EditCompleted, w );

   CenterDec_Sizer.SetSpacing( 4 );
   CenterDec_Sizer.Add( CenterDec_Label );
   CenterDec_Sizer.Add( CenterDec_Edit, 100 );
   CenterDec_Sizer.AddStretch();

   //

   ShowCompoundAngles_CheckBox.SetText( "Show compound angles" );
   ShowCompoundAngles_CheckBox.SetToolTip( "<p>If enabled, coordinates and angular distances will be shown as compound angular "
   "quantities (degrees/hours, (arc)minutes, and (arc)seconds). If disabled, coordinates will be shown as scalars (degrees or "
   "hours with decimals).</p>" );
   ShowCompoundAngles_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2Interface::e_Click, w );

   ShowCompoundAngles_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowCompoundAngles_Sizer.Add( ShowCompoundAngles_CheckBox );
   ShowCompoundAngles_Sizer.AddStretch();

   //

   RAInTimeUnits_CheckBox.SetText( "R.A. in time units" );
   RAInTimeUnits_CheckBox.SetToolTip( "<p>When enabled, the right ascension coordinate will be shown expressed in hours, "
      "minutes, and seconds. When disabled, the right ascension will be expressed in degrees, arcminutes and arcseconds.</p>" );
   RAInTimeUnits_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2Interface::e_Click, w );

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
   Radius_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaDR2Interface::e_EditCompleted, w );

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
   MagnitudeLow_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&GaiaDR2Interface::e_EditValueUpdated, w );

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
   MagnitudeHigh_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&GaiaDR2Interface::e_EditValueUpdated, w );

   //

   const char* inclusionFlagsToolTip = "<p>If non-zero, only sources with all of these flags set will be included "
      "in the search result.</p>";

   InclusionFlags_Label.SetText( "Inclusion flags:" );
   InclusionFlags_Label.SetFixedWidth( labelWidth1 );
   InclusionFlags_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InclusionFlags_Label.SetToolTip( inclusionFlagsToolTip );

   InclusionFlags_Edit.SetToolTip( inclusionFlagsToolTip );
   InclusionFlags_Edit.SetMinWidth( editWidth3 );
   InclusionFlags_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaDR2Interface::e_EditCompleted, w );

   InclusionFlagsClear_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   InclusionFlagsClear_ToolButton.SetScaledFixedSize( 20, 20 );
   InclusionFlagsClear_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   InclusionFlagsClear_ToolButton.SetToolTip( "<p>Clear inclusion flags.</p>" );
   InclusionFlagsClear_ToolButton.OnClick( (ToolButton::click_event_handler)&GaiaDR2Interface::e_Click, w );

   InclusionFlags_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/control-multiple-selection.png" ) ) );
   InclusionFlags_ToolButton.SetScaledFixedSize( 20, 20 );
   InclusionFlags_ToolButton.SetToolTip( "<p>Define inclusion flags.</p>" );
   InclusionFlags_ToolButton.OnClick( (Button::click_event_handler)&GaiaDR2Interface::e_Click, w );

   InclusionFlags_Sizer.SetSpacing( 4 );
   InclusionFlags_Sizer.Add( InclusionFlags_Label );
   InclusionFlags_Sizer.Add( InclusionFlags_Edit, 100 );
   InclusionFlags_Sizer.Add( InclusionFlagsClear_ToolButton );
   InclusionFlags_Sizer.Add( InclusionFlags_ToolButton );
   InclusionFlags_Sizer.AddStretch();

   //

   const char* exclusionFlagsToolTip = "<p>Sources with any of these flags set will <i>not</i> be included "
      "in the search result.</p>";

   ExclusionFlags_Label.SetText( "Exclusion flags:" );
   ExclusionFlags_Label.SetFixedWidth( labelWidth1 );
   ExclusionFlags_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ExclusionFlags_Label.SetToolTip( exclusionFlagsToolTip );

   ExclusionFlags_Edit.SetToolTip( exclusionFlagsToolTip );
   ExclusionFlags_Edit.SetMinWidth( editWidth3 );
   ExclusionFlags_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaDR2Interface::e_EditCompleted, w );

   ExclusionFlagsClear_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   ExclusionFlagsClear_ToolButton.SetScaledFixedSize( 20, 20 );
   ExclusionFlagsClear_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExclusionFlagsClear_ToolButton.SetToolTip( "<p>Clear exclusion flags.</p>" );
   ExclusionFlagsClear_ToolButton.OnClick( (ToolButton::click_event_handler)&GaiaDR2Interface::e_Click, w );

   ExclusionFlags_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/control-multiple-selection.png" ) ) );
   ExclusionFlags_ToolButton.SetScaledFixedSize( 20, 20 );
   ExclusionFlags_ToolButton.SetToolTip( "<p>Define exclusion flags.</p>" );
   ExclusionFlags_ToolButton.OnClick( (Button::click_event_handler)&GaiaDR2Interface::e_Click, w );

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
   SourceLimit_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&GaiaDR2Interface::e_EditValueUpdated, w );

   SourceLimitReset_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   SourceLimitReset_ToolButton.SetScaledFixedSize( 20, 20 );
   SourceLimitReset_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   SourceLimitReset_ToolButton.SetToolTip( "<p>Reset source count limit to perform an unlimited search.</p>" );
   SourceLimitReset_ToolButton.OnClick( (ToolButton::click_event_handler)&GaiaDR2Interface::e_Click, w );

   //

   SearchParametersLeft_Sizer.SetSpacing( 4 );
   SearchParametersLeft_Sizer.Add( CenterRA_Sizer );
   SearchParametersLeft_Sizer.Add( CenterDec_Sizer );
   SearchParametersLeft_Sizer.Add( Radius_Sizer );
   SearchParametersLeft_Sizer.Add( ShowCompoundAngles_Sizer );
   SearchParametersLeft_Sizer.Add( RAInTimeUnits_Sizer );
   SearchParametersLeft_Sizer.Add( MagnitudeLow_NumericEdit );
   SearchParametersLeft_Sizer.Add( MagnitudeHigh_NumericEdit );
   SearchParametersLeft_Sizer.Add( InclusionFlags_Sizer );
   SearchParametersLeft_Sizer.Add( ExclusionFlags_Sizer );
   SearchParametersLeft_Sizer.Add( SourceLimit_NumericEdit );

   //

   SearchCoordinates_Button.SetText( "Search Coordinates" );
   SearchCoordinates_Button.SetIcon( w.ScaledResource( ":/icons/find.png" ) );
   SearchCoordinates_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   SearchCoordinates_Button.SetToolTip( "<p>Open the Online Coordinate Search dialog.</p>" );
   SearchCoordinates_Button.OnClick( (Button::click_event_handler)&GaiaDR2Interface::e_Click, w );

   //

   SearchParametersRight_Sizer.Add( SearchCoordinates_Button );
   SearchParametersRight_Sizer.AddStretch();

   //

   SearchParameters_Sizer.SetSpacing( 32 );
   SearchParameters_Sizer.Add( SearchParametersLeft_Sizer, 100 );
   SearchParameters_Sizer.Add( SearchParametersRight_Sizer );

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
   SortBy_ComboBox.AddItem( "G mean magnitude" );
   SortBy_ComboBox.AddItem( "G_BP mean magnitude" );
   SortBy_ComboBox.AddItem( "G_RP mean magnitude" );
   SortBy_ComboBox.AddItem( "Parallax" );
   SortBy_ComboBox.SetToolTip( sortByToolTip );
   SortBy_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&GaiaDR2Interface::e_ItemSelected, w );

   SortBy_Sizer.SetSpacing( 4 );
   SortBy_Sizer.Add( SortBy_Label );
   SortBy_Sizer.Add( SortBy_ComboBox );
   SortBy_Sizer.AddStretch();

   //

   GenerateTextOutput_CheckBox.SetText( "Generate text output" );
   GenerateTextOutput_CheckBox.SetToolTip( "<p>When enabled, the search operation will generate an output "
      "plain text file including available data for all sources fulfilling the specified search criteria.</p>" );
   GenerateTextOutput_CheckBox.OnClick( (Button::click_event_handler)&GaiaDR2Interface::e_Click, w );

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
   TextFormat_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&GaiaDR2Interface::e_ItemSelected, w );

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
   TextHeaders_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&GaiaDR2Interface::e_ItemSelected, w );

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
   OutputFilePath_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaDR2Interface::e_EditCompleted, w );

   OutputFilePath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select-file.png" ) ) );
   OutputFilePath_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputFilePath_ToolButton.SetToolTip( "<p>Select the output file.</p>" );
   OutputFilePath_ToolButton.OnClick( (Button::click_event_handler)&GaiaDR2Interface::e_Click, w );

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
// EOF GaiaDR2Interface.cpp - Released 2020-11-27T11:02:59Z
