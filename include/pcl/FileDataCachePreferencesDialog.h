//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// pcl/FileDataCachePreferencesDialog.h - Released 2023-08-28T15:23:15Z
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

#ifndef __PCL_FileDataCachePreferencesDialog_h
#define __PCL_FileDataCachePreferencesDialog_h

/// \file pcl/FileDataCache.h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS FileDataCache;

/*!
 * \class FileDataCachePreferencesDialog
 * \brief A dialog to edit file data cache preferences settings
 *
 * This dialog allows to define persistence and maximum duration for file data
 * cache items associated with a given FileDataCache instance. The dialog also
 * allows to save and clear all cache items in memory and persistent storage.
 */
class PCL_CLASS FileDataCachePreferencesDialog : public Dialog
{
public:

   /*!
    * Constructs a dialog to edit preferences settings for a file data cache.
    *
    * \param cache         Reference to a file data cache object that will be
    *                      managed by this cache preferences dialog.
    *
    * \param persistent    Reference to a variable storing a cache persistence
    *                      state. Persistent caches should be written to
    *                      permanent storage. Non-persistent caches are only
    *                      kept in volatile memory. This variable will be
    *                      updated with the user selection if the dialog is
    *                      accepted.
    */
   FileDataCachePreferencesDialog( FileDataCache& cache, bool& persistent );

private:

   FileDataCache& m_cache;
   bool&          m_persistent;

   VerticalSizer  Global_Sizer;
      HorizontalSizer   PersistentCache_Sizer;
         CheckBox          PersistentCache_CheckBox;
      HorizontalSizer   CacheDuration_Sizer;
         Label             CacheDuration_Label;
         SpinBox           CacheDuration_SpinBox;
      HorizontalSizer   ClearCache_Sizer;
         PushButton        ClearCache_PushButton;
      HorizontalSizer   PurgeCache_Sizer;
         PushButton        PurgeCache_PushButton;
      HorizontalSizer   SaveCache_Sizer;
         PushButton        SaveCache_PushButton;
      HorizontalSizer   Buttons_Sizer;
         Label             Info_Label;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void e_Click( Button& sender, bool checked );
   void e_Return( Dialog& sender, int retVal );

   void UpdateInfo();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FileDataCachePreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF pcl/FileDataCachePreferencesDialog.h - Released 2023-08-28T15:23:15Z
