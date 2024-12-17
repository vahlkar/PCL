//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceInterface.h - Released 2024-12-17T18:15:44Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
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

#ifndef __NetworkServiceInterface_h
#define __NetworkServiceInterface_h

#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/TextBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS NetworkTransfer;

class NetworkServiceInterface : public ProcessInterface
{
public:

   NetworkServiceInterface();
   virtual ~NetworkServiceInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;

private:

   String m_taskProviderURL;
   String m_workerName;
   String m_taskType;
   String m_userName;
   String m_userPassword;

   struct GUIData
   {
      GUIData( NetworkServiceInterface& );

      VerticalSizer     Global_Sizer;
         HorizontalSizer   TaskProvider_Sizer;
            Label             TaskProvider_Label;
            Edit              TaskProvider_Edit;
         HorizontalSizer   TaskType_Sizer;
            Label             TaskType_Label;
            Edit              TaskType_Edit;
         HorizontalSizer   WorkerName_Sizer;
            Label             WorkerName_Label;
            Edit              WorkerName_Edit;
         HorizontalSizer   UserName_Sizer;
            Label             UserName_Label;
            Edit              UserName_Edit;
         HorizontalSizer   UserPassword_Sizer;
            Label             UserPassword_Label;
            Edit              UserPassword_Edit;
         TextBox           Log_TextBox;
         Label             Status_Label;
         HorizontalSizer   Buttons_Sizer;
            PushButton        Start_PushButton;
            PushButton        Stop_PushButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_EditGetFocus( Control& sender );
   void e_EditCompleted( Edit& sender );
   void e_ButtonClick( Button& sender, bool checked );

   friend struct GUIData;
   friend class UILogger;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern NetworkServiceInterface* TheNetworkServiceInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NetworkServiceInterface_h

// ----------------------------------------------------------------------------
// EOF NetworkServiceInterface.h - Released 2024-12-17T18:15:44Z
