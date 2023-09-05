//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.4
// ----------------------------------------------------------------------------
// FITSHeaderInterface.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#ifndef __FITSHeaderInterface_h
#define __FITSHeaderInterface_h

#include <pcl/CheckBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/TreeBox.h>
#include <pcl/ViewList.h>

#include "FITSHeaderInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class FITSHeaderInterface : public ProcessInterface
{
public:

   FITSHeaderInterface();
   virtual ~FITSHeaderInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void TrackViewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsImageNotifications() const override;
   void ImageUpdated( const View& v ) override;
   void ImageFocused( const View& v ) override;

   void SelectImage( const ImageWindow& );

private:

   FITSHeaderInstance instance;

   struct GUIData
   {
      GUIData( FITSHeaderInterface& );

      VerticalSizer     Global_Sizer;
         ViewList          AllImages_ViewList;
         TreeBox           Keywords_List;
         HorizontalSizer   Name_Sizer;
            Label             Name_Label;
            Edit              Name_Edit;
         HorizontalSizer   Value_Sizer;
            Label             Value_Label;
            Edit              Value_Edit;
         HorizontalSizer   Comment_Sizer;
            Label             Comment_Label;
            Edit              Comment_Edit;
         HorizontalSizer   Buttons_Sizer;
            CheckBox          HIERARCH_CheckBox;
            Label             Info_Label;
            PushButton        Add_PushButton;
            PushButton        Replace_PushButton;
            PushButton        Remove_PushButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateKeywordList();
   void UpdateButtons();
   void UpdateInfo();
   void UpdateKeywordNode( int index );

   bool GetKeywordData( FITSHeaderInstance::Keyword& k );

   void __ViewList_ViewSelected( ViewList& sender, View& view );
   void __Keywords_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void __Keywords_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __Keywords_NodeSelectionUpdated( TreeBox& sender );
   void __Keyword_ButtonClick( Button& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern FITSHeaderInterface* TheFITSHeaderInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FITSHeaderInterface_h

// ----------------------------------------------------------------------------
// EOF FITSHeaderInterface.h - Released 2023-08-28T15:23:41Z
