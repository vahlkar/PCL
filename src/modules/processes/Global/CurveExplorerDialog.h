//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.1
// ----------------------------------------------------------------------------
// CurveExplorerDialog.h - Released 2024-12-17T18:15:44Z
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

#ifndef __CurveExplorerDialog_h
#define __CurveExplorerDialog_h

#include <pcl/ComboBox.h>
#include <pcl/Dialog.h>
#include <pcl/FilterManager.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>
#include <pcl/WebView.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class CurveExplorerDialog : public Dialog
{
public:

   CurveExplorerDialog();

   bool MadeChanges() const
   {
      return m_madeChanges;
   }

   void ReloadData();

private:

   FilterManager           m_manager;
   SampledSpectrumDataList m_filters;
   SampledSpectrumDataList m_whiteReferences;
   bool                    m_workingWithFilters = true;
   bool                    m_modified = false;
   bool                    m_madeChanges = false;
   StringList              m_htmlFiles;
   bool                    m_splitterPressed = false;
   int                     m_splitterPos = 0;

   VerticalSizer     Global_Sizer;
      HorizontalSizer   CurveType_Sizer;
         Label             CurveType_Label;
         ComboBox          CurveType_ComboBox;
      TreeBox           Curves_TreeBox;
      Control           VertSplitter_Control;
      WebView           Graph_WebView;
      HorizontalSizer   Buttons_Sizer;
         ToolButton        ExportPDF_ToolButton;
         ToolButton        CombineFilters_ToolButton;
         PushButton        OK_PushButton;

   void Update();
   void UpdateCurves();
   void UpdateGraph();
   void CombineFilters();
   void ExportPDF();

   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeSelectionUpdated( TreeBox& sender );
   void e_Click( Button& sender, bool checked );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
   void e_LoadFinished( WebView& sender, bool state );
   void e_Close( Control& sender, bool& allowClose );
   void e_Show( Control& sender );
   void e_Hide( Control& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CurveExplorerDialog_h

// ----------------------------------------------------------------------------
// EOF CurveExplorerDialog.h - Released 2024-12-17T18:15:44Z
