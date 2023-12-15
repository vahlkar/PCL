//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// pcl/FileDataCachePreferencesDialog.cpp - Released 2023-12-01T19:15:53Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/FileDataCache.h>
#include <pcl/FileDataCachePreferencesDialog.h>
#include <pcl/MessageBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FileDataCachePreferencesDialog::FileDataCachePreferencesDialog( FileDataCache& cache, bool& persistent )
   : m_cache( cache )
   , m_persistent( persistent )
{
   int labelWidth1 = Font().Width( String( "Cache duration (days):" ) + 'T' );
   int ui4 = LogicalPixelsToPhysical( 4 );

   //

   const char* persistentCacheToolTip =
   "<p>A persistent cache is kept across PixInsight sessions. If you disable "
   "this option, the file cache will still be used, but only during the current "
   "session; as soon as you exit the PixInsight core application, all the newly "
   "cached information will be lost. With the persistent cache option enabled, "
   "cached data generated during this session will be stored as a disk file and "
   "will be available the next time you run PixInsight.</p>";

   PersistentCache_CheckBox.SetText( "Persistent file cache" );
   PersistentCache_CheckBox.SetToolTip( persistentCacheToolTip );
   PersistentCache_CheckBox.SetChecked( m_persistent );
   PersistentCache_CheckBox.OnClick( (pcl::Button::click_event_handler)&FileDataCachePreferencesDialog::e_Click, *this );

   PersistentCache_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PersistentCache_Sizer.Add( PersistentCache_CheckBox );
   PersistentCache_Sizer.AddStretch();

   //

   const char* cacheDurationToolTip =
   "<p>Persistent file cache items can be automatically removed after a "
   "specified period without accessing the corresponding files. Enter the "
   "desired period in days (120 days maximum), or specify zero to disable this "
   "<i>automatic purge</i> feature, so that existing file cache items will "
   "never be removed automatically.</p>";

   CacheDuration_Label.SetText( "Cache duration (days):" );
   CacheDuration_Label.SetMinWidth( labelWidth1 );
   CacheDuration_Label.SetToolTip( cacheDurationToolTip );
   CacheDuration_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CacheDuration_Label.Enable( m_persistent );

   CacheDuration_SpinBox.SetRange( 0, 120 );
   CacheDuration_SpinBox.SetMinimumValueText( "<Forever>" );
   CacheDuration_SpinBox.SetToolTip( cacheDurationToolTip );
   CacheDuration_SpinBox.SetValue( m_cache.MaxItemDuration() );
   CacheDuration_SpinBox.Enable( m_persistent );

   CacheDuration_Sizer.SetSpacing( 4 );
   CacheDuration_Sizer.Add( CacheDuration_Label );
   CacheDuration_Sizer.Add( CacheDuration_SpinBox );
   CacheDuration_Sizer.AddStretch();

   //

   ClearCache_PushButton.SetText( "Clear Memory Cache Now" );
   ClearCache_PushButton.SetToolTip( "<p>Click this button to remove all cache items currently stored in volatile RAM.</p>" );
   ClearCache_PushButton.OnClick( (pcl::Button::click_event_handler)&FileDataCachePreferencesDialog::e_Click, *this );
   ClearCache_PushButton.Enable( !m_cache.IsEmpty() );

   ClearCache_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClearCache_Sizer.Add( ClearCache_PushButton, 100 );

   //

   PurgeCache_PushButton.SetText( "Purge Persistent Cache Now" );
   PurgeCache_PushButton.SetToolTip( "<p>Click this button to remove all stored persistent cache items.</p>" );
   PurgeCache_PushButton.OnClick( (pcl::Button::click_event_handler)&FileDataCachePreferencesDialog::e_Click, *this );

   PurgeCache_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PurgeCache_Sizer.Add( PurgeCache_PushButton, 100 );

   //

   SaveCache_PushButton.SetText( "Save Persistent Cache Now" );
   SaveCache_PushButton.SetToolTip( "<p>Click this button to write the current cache state to a disk file.</p>" );
   SaveCache_PushButton.OnClick( (pcl::Button::click_event_handler)&FileDataCachePreferencesDialog::e_Click, *this );
   SaveCache_PushButton.Enable( m_cache.IsModified() );

   SaveCache_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SaveCache_Sizer.Add( SaveCache_PushButton, 100 );

   //

   Info_Label.SetFixedWidth( labelWidth1 );
   Info_Label.SetToolTip( "<p>Current cache file size.</p>" );
   Info_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&FileDataCachePreferencesDialog::e_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&FileDataCachePreferencesDialog::e_Click, *this );

   Buttons_Sizer.Add( Info_Label );
   Buttons_Sizer.AddSpacing( 4 );
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.AddSpacing( 8 );
   Buttons_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( PersistentCache_Sizer );
   Global_Sizer.Add( CacheDuration_Sizer );
   Global_Sizer.Add( ClearCache_Sizer );
   Global_Sizer.Add( PurgeCache_Sizer );
   Global_Sizer.Add( SaveCache_Sizer );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( m_cache.CacheName() + " Preferences" );

   UpdateInfo();

   OnReturn( (pcl::Dialog::return_event_handler)&FileDataCachePreferencesDialog::e_Return, *this );
}

// ----------------------------------------------------------------------------

void FileDataCachePreferencesDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == ClearCache_PushButton )
   {
      if ( MessageBox( "<p>All cache items currently stored in volatile memory will be deleted.</p>"
                       "<p><b>This action is irreversible. Proceed?</b></p>",
                       m_cache.CacheName() + " - Clear Memory Cache", // caption
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() == StdButton::Yes )
      {
         size_type n = m_cache.NumberOfItems();
         m_cache.Clear();
         MessageBox( "<p>" + String( n ) + " cache item(s) were deleted.</p>",
                     m_cache.CacheName() + " - Clear Memory Cache", // caption
                     StdIcon::Information,
                     StdButton::Ok ).Execute();
      }
   }
   else if ( sender == PurgeCache_PushButton )
   {
      if ( MessageBox( "<p>All stored persistent file cache items will be deleted.</p>"
                       "<p><b>This action is irreversible. Proceed?</b></p>",
                       m_cache.CacheName() + " - Purge Persistent Cache", // caption
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() == StdButton::Yes )
      {
         m_cache.Purge();
         UpdateInfo();
         MessageBox( "<p>All persistent cache items were deleted.</p>",
                     m_cache.CacheName() + " - Purge Persistent Cache", // caption
                     StdIcon::Information,
                     StdButton::Ok ).Execute();
      }
   }
   else if ( sender == SaveCache_PushButton )
   {
      if ( MessageBox( "<p>All stored persistent file cache items will be deleted and "
                       "replaced with the current cache state.</p>"
                       "<p><b>This action is irreversible. Proceed?</b></p>",
                       m_cache.CacheName() + " - Save Persistent Cache", // caption
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() == StdButton::Yes )
      {
         m_cache.Save();
         UpdateInfo();
         SaveCache_PushButton.Enable( m_cache.IsModified() );
         MessageBox( "<p>Cache data has been written to persistent storage.</p>",
                     m_cache.CacheName() + " - Save Persistent Cache", // caption
                     StdIcon::Information,
                     StdButton::Ok ).Execute();
      }
   }
   else if ( sender == PersistentCache_CheckBox )
   {
      CacheDuration_Label.Enable( checked );
      CacheDuration_SpinBox.Enable( checked );
   }
   else if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

// ----------------------------------------------------------------------------

void FileDataCachePreferencesDialog::e_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      m_persistent = PersistentCache_CheckBox.IsChecked();
      m_cache.SetMaxItemDuration( CacheDuration_SpinBox.Value() );
   }
}

// ----------------------------------------------------------------------------

void FileDataCachePreferencesDialog::UpdateInfo()
{
   Info_Label.SetText( File::SizeAsString( m_cache.FileSize() ) );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FileDataCachePreferencesDialog.cpp - Released 2023-12-01T19:15:53Z
