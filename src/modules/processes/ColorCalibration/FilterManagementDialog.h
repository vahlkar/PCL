//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.3
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// FilterManagementDialog.h - Released 2023-05-17T17:06:42Z
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

#ifndef __FilterManagementDialog_h
#define __FilterManagementDialog_h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FilterManagementDialog : public Dialog
{
public:

   enum working_mode { ImportXML, ExportXML, ImportCSV, ExportCSV, MergeWithDefault, Reset };

   FilterManagementDialog();

   working_mode Mode() const
   {
      return m_mode;
   }

   String Path() const
   {
      switch ( m_mode )
      {
      case ImportXML:
         return m_inputXMLFilePath;
      case ExportXML:
         return m_outputXMLFilePath;
      case ImportCSV:
         return m_inputCSVDirectory;
      case ExportCSV:
         return m_outputCSVDirectory;
      default: // ?!
         return String();
      }
   }

   bool Merge() const
   {
      return m_merge;
   }

private:

   working_mode m_mode = ImportCSV;
   String       m_outputXMLFilePath;
   String       m_outputCSVDirectory;
   String       m_inputXMLFilePath;
   String       m_inputCSVDirectory;
   bool         m_merge = true;

   VerticalSizer     Global_Sizer;
      HorizontalSizer   ImportXMLFiltersDatabase_Sizer;
         RadioButton       ImportXMLFiltersDatabase_RadioButton;
      HorizontalSizer   ImportCSVFilterDefinitions_Sizer;
         RadioButton       ImportCSVFilterDefinitions_RadioButton;
      HorizontalSizer   InputPath_Sizer;
         Label             InputPath_Label;
         Edit              InputPath_Edit;
         ToolButton        InputPath_ToolButton;
      HorizontalSizer   ExportXMLFiltersDatabase_Sizer;
         RadioButton       ExportXMLFiltersDatabase_RadioButton;
      HorizontalSizer   ExportCSVFilterDefinitions_Sizer;
         RadioButton       ExportCSVFilterDefinitions_RadioButton;
         CheckBox          Merge_CheckBox;
      HorizontalSizer   OutputPath_Sizer;
         Label             OutputPath_Label;
         Edit              OutputPath_Edit;
         ToolButton        OutputPath_ToolButton;
      HorizontalSizer   MergeWithDefaultFilters_Sizer;
         RadioButton       MergeWithDefaultFilters_RadioButton;
      HorizontalSizer   ResetFilters_Sizer;
         RadioButton       ResetFilters_RadioButton;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void UpdateControls();

   void e_Click( Button& sender, bool checked );
   void e_EditCompleted( Edit& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FilterManagementDialog_h

// ----------------------------------------------------------------------------
// EOF FilterManagementDialog.h - Released 2023-05-17T17:06:42Z
