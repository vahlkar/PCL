//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// SplitCFAInterface.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
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

#ifndef __SplitCFAInterface_h
#define __SplitCFAInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

#include "SplitCFAInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SplitCFAInterface : public ProcessInterface
{
public:

   SplitCFAInterface();
   virtual ~SplitCFAInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   SplitCFAInstance m_instance;

   struct GUIData
   {
      GUIData( SplitCFAInterface& );

      VerticalSizer Global_Sizer;

      SectionBar TargetImages_SectionBar;
      Control TargetImages_Control;
      HorizontalSizer TargetImages_Sizer;
      TreeBox Files_TreeBox;
      VerticalSizer TargetButtons_Sizer;
      PushButton AddFolder_PushButton;
      PushButton AddFiles_PushButton;
      PushButton SelectAll_PushButton;
      PushButton InvertSelection_PushButton;
      PushButton ToggleSelected_PushButton;
      PushButton RemoveSelected_PushButton;
      PushButton Clear_PushButton;
      CheckBox FullPaths_CheckBox;

      SectionBar Output_SectionBar;
      Control Output_Control;
      VerticalSizer Output_Sizer;
      HorizontalSizer OutputDir_Sizer;
      CheckBox OutputTree_CheckBox;
      Edit OutputDir_Edit;
      ToolButton OutputDir_SelectButton;
      ToolButton OutputDir_ClearButton;
      HorizontalSizer OutputChunks_Sizer;
      CheckBox OutputSubDirCFA_CheckBox;
      CheckBox Overwrite_CheckBox;
      Label Prefix_Label;
      Edit Prefix_Edit;
      Label Postfix_Label;
      Edit Postfix_Edit;
   };

   GUIData* GUI = nullptr;

   void SelectDir();                                        // Select output directory
   void SearchFile( const String&, const String& );         // Recursiv Search file in sub-folders
   int FileInList( const String& );                         // is a file in target frames or not? : -1 = not found; otherwise return pointer >=0
   void AddFile( const String&, const String& = String() ); // add one file to target list
   void AddFiles();                                         // add selected files
   void AddFolders();                                       // search all sub-folders and add all files
   void RemoveSelectedFiles();

   void UpdateControls();
   void UpdateTargetImagesList();
   void UpdateImageSelectionButtons();

   void __TargetImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void __TargetImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __TargetImages_NodeSelectionUpdated( TreeBox& sender );
   void __TargetImages_BottonClick( Button& sender, bool checked );
   void __ToggleSection( SectionBar& sender, Control& section, bool start );
   void __MouseDoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void __EditCompleted( Edit& sender );
   void __Button_Click( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SplitCFAInterface* TheSplitCFAInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __SplitCFAInterface_h

// ----------------------------------------------------------------------------
// EOF SplitCFAInterface.h - Released 2024-12-28T16:54:15Z
