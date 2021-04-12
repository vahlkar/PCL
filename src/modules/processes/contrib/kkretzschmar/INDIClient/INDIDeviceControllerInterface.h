//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.7
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.1
// ----------------------------------------------------------------------------
// INDIDeviceControllerInterface.h - Released 2020-12-17T15:46:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2020 Klaus Kretzschmar
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

#ifndef __INDIDeviceControllerInterface_h
#define __INDIDeviceControllerInterface_h

#include <pcl/CheckBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Timer.h>
#include <pcl/TreeBox.h>

#include "INDIDeviceControllerInstance.h"
#include "ZeroconfServiceDetection.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class INDIDeviceControllerInterface : public ProcessInterface
{
public:

   INDIDeviceControllerInterface();
   virtual ~INDIDeviceControllerInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   InterfaceFeatures Features() const override;

   void UpdateControls();

   struct GUIData
   {
      GUIData( INDIDeviceControllerInterface& );

      VerticalSizer Global_Sizer;
      SectionBar Server_SectionBar;
      Control Server_Control;
      HorizontalSizer Server_HSizer;
      VerticalSizer ServerName_VSizer;
      HorizontalSizer ServerName_HSizer;
      HorizontalSizer ServiceDetection_HSizer;
      Label HostName_Label;
      Edit HostName_Edit;
      Label Port_Label;
      SpinBox Port_SpinBox;
      CheckBox EnableServerDetection_CheckBox;
      VerticalSizer ServerAction_Sizer;
      PushButton Connect_PushButton;
      PushButton Disconnect_PushButton;
      SectionBar Devices_SectionBar;
      Control Devices_Control;
      HorizontalSizer Devices_Sizer;
      TreeBox Devices_TreeBox;
      VerticalSizer NodeAction_Sizer;
      PushButton NodeAction_PushButton;
      Label ServerMessage_Label;

      Timer SynchronizeWithServer_Timer;
#ifdef WITH_ZEROCONF
      ZeroConfServiceBrowser ZeroconfServiceBrowser_Thread;
      bool m_isIndigoServiceChanged = false;
#endif
   };

private:

   GUIData* GUI = nullptr;

   void UpdateDeviceLists();
   void UpdateNodeActionButtons( TreeBox::Node* );
   void AdjustTreeColumns();

   void e_Show( Control& );
   void e_Hide( Control& );
   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_Click( Button& sender, bool checked );
   void e_EditCompleted( Edit& sender );
   void e_SpinValueUpdated( SpinBox& sender, int value );
   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node&, int col );
   void e_NodeDoubleClicked( TreeBox& sender, TreeBox::Node&, int col );
   void e_NodeExpanded( TreeBox& sender, TreeBox::Node& );
   void e_NodeCollapsed( TreeBox& sender, TreeBox::Node& );
   void e_NodeSelectionUpdated( TreeBox& sender );
   void e_Timer( Timer& sender );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern INDIDeviceControllerInterface* TheINDIDeviceControllerInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __INDIDeviceControllerInterface_h

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInterface.h - Released 2020-12-17T15:46:56Z
