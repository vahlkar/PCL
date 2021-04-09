//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// ChannelMatchInterface.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#ifndef __ChannelMatchInterface_h
#define __ChannelMatchInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "ChannelMatchInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ChannelMatchInterface : public ProcessInterface
{
public:

   ChannelMatchInterface();
   virtual ~ChannelMatchInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   ChannelMatchInstance m_instance;

   struct GUIData
   {
      GUIData( ChannelMatchInterface& );

      HorizontalSizer   Global_Sizer;

         GroupBox          Channels_GroupBox;
         VerticalSizer     Channels_Sizer;
            CheckBox          R_CheckBox;
            CheckBox          G_CheckBox;
            CheckBox          B_CheckBox;

         GroupBox          XOffset_GroupBox;
         VerticalSizer     XOffset_Sizer;
            HorizontalSizer   R_XOffset_Sizer;
               NumericEdit       R_XOffset_NumericEdit;
               ToolButton        R_Left_ToolButton;
               ToolButton        R_Right_ToolButton;
            HorizontalSizer   G_XOffset_Sizer;
               NumericEdit       G_XOffset_NumericEdit;
               ToolButton        G_Left_ToolButton;
               ToolButton        G_Right_ToolButton;
            HorizontalSizer   B_XOffset_Sizer;
               NumericEdit       B_XOffset_NumericEdit;
               ToolButton        B_Left_ToolButton;
               ToolButton        B_Right_ToolButton;

         GroupBox          YOffset_GroupBox;
         VerticalSizer     YOffset_Sizer;
            HorizontalSizer   R_YOffset_Sizer;
               NumericEdit       R_YOffset_NumericEdit;
               ToolButton        R_Up_ToolButton;
               ToolButton        R_Down_ToolButton;
            HorizontalSizer   G_YOffset_Sizer;
               NumericEdit       G_YOffset_NumericEdit;
               ToolButton        G_Up_ToolButton;
               ToolButton        G_Down_ToolButton;
            HorizontalSizer   B_YOffset_Sizer;
               NumericEdit       B_YOffset_NumericEdit;
               ToolButton        B_Up_ToolButton;
               ToolButton        B_Down_ToolButton;

         GroupBox          Factors_GroupBox;
         VerticalSizer     Factors_Sizer;
            NumericControl    R_Factor_NumericControl;
            NumericControl    G_Factor_NumericControl;
            NumericControl    B_Factor_NumericControl;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __Channel_Click( Button& sender, bool checked );
   void __Channel_Offset_Click( Button& sender, bool checked );
   void __Channel_Offset_ValueUpdated( NumericEdit& sender, double value );
   void __Channel_Factor_ValueUpdated( NumericEdit& sender, double value );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ChannelMatchInterface* TheChannelMatchInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelMatchInterface_h

// ----------------------------------------------------------------------------
// EOF ChannelMatchInterface.h - Released 2021-04-09T19:41:48Z
