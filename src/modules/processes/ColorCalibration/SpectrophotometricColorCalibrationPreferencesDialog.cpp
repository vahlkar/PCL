//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.5
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// SpectrophotometricColorCalibrationPreferencesDialog.cpp - Released 2023-06-21T16:30:12Z
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

#include "SpectrophotometricColorCalibrationPreferencesDialog.h"
#include "SpectrophotometricColorCalibrationProcess.h"

#include <pcl/FileDialog.h>
#include <pcl/MessageBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SpectrophotometricColorCalibrationPreferencesDialog::SpectrophotometricColorCalibrationPreferencesDialog()
{
   m_filtersDatabaseFilePath = SpectrophotometricColorCalibrationProcess::FiltersDatabaseFilePath();

   //

   pcl::Font fnt = this->Font();
   int labelWidth1 = fnt.Width( "Filters database:m" );
   int editWidth1 = fnt.Width( String( 'm', 40 ) );

   //

   FiltersDatabasePath_Label.SetText( "Filters database:" );
   FiltersDatabasePath_Label.SetFixedWidth( labelWidth1 );
   FiltersDatabasePath_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   FiltersDatabasePath_Edit.OnEditCompleted( (Edit::edit_event_handler)&SpectrophotometricColorCalibrationPreferencesDialog::e_EditCompleted, *this );
   FiltersDatabasePath_Edit.SetMinWidth( editWidth1 );

   FiltersDatabasePath_ToolButton.SetIcon( Bitmap( ScaledResource( ":/icons/select-file.png" ) ) );
   FiltersDatabasePath_ToolButton.SetScaledFixedSize( 20, 20 );
   FiltersDatabasePath_ToolButton.OnClick( (Button::click_event_handler)&SpectrophotometricColorCalibrationPreferencesDialog::e_Click, *this );

   FiltersDatabasePath_Sizer.SetSpacing( 4 );
   FiltersDatabasePath_Sizer.Add( FiltersDatabasePath_Label );
   FiltersDatabasePath_Sizer.Add( FiltersDatabasePath_Edit, 100 );
   FiltersDatabasePath_Sizer.Add( FiltersDatabasePath_ToolButton );

   //

   LoadDefaultFiltersDatabase_PushButton.SetText( " Default Filters Database" );
   LoadDefaultFiltersDatabase_PushButton.SetIcon( Bitmap( ScaledResource( ":/icons/reload.png" ) ) );
   LoadDefaultFiltersDatabase_PushButton.OnClick( (pcl::Button::click_event_handler)&SpectrophotometricColorCalibrationPreferencesDialog::e_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&SpectrophotometricColorCalibrationPreferencesDialog::e_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&SpectrophotometricColorCalibrationPreferencesDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.Add( LoadDefaultFiltersDatabase_PushButton );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( FiltersDatabasePath_Sizer );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetFixedHeight();
   SetMinWidth();

   SetWindowTitle( "SPCC Preferences" );

   UpdateControls();
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationPreferencesDialog::UpdateControls()
{
   FiltersDatabasePath_Edit.SetText( m_filtersDatabaseFilePath );
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationPreferencesDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == FiltersDatabasePath_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.SetCaption( "SPCC: Filters Database File" );
      d.SetFilter( FileFilter( "XSPD Spectrum Database files", ".xspd" ) );
      if ( d.Execute() )
      {
         m_filtersDatabaseFilePath = d.FileName();
         UpdateControls();
      }
   }
   else if ( sender == LoadDefaultFiltersDatabase_PushButton )
   {
      m_filtersDatabaseFilePath = SpectrophotometricColorCalibrationProcess::DefaultFiltersDatabaseFilePath();
      UpdateControls();
   }
   else if ( sender == OK_PushButton )
   {
      if ( m_filtersDatabaseFilePath.IsEmpty() )
      {
         MessageBox( "<p>Please specify a filters database file.</p>",
                     "SPCC", StdIcon::Error, StdButton::Ok ).Execute();
         return;
      }
      if ( !File::Exists( m_filtersDatabaseFilePath ) )
      {
         MessageBox( "<p>The specified file does not exist or cannot be accessed.</p>",
                     "SPCC", StdIcon::Error, StdButton::Ok ).Execute();
         return;
      }

      Ok();
   }
   else if ( sender == Cancel_PushButton )
   {
      Cancel();
   }
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationPreferencesDialog::e_EditCompleted( Edit& sender )
{
   if ( sender == FiltersDatabasePath_Edit )
   {
      m_filtersDatabaseFilePath = sender.Text().Trimmed();
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SpectrophotometricColorCalibrationPreferencesDialog.cpp - Released 2023-06-21T16:30:12Z
