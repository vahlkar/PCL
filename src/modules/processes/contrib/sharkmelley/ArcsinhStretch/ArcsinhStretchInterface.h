//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// ArcsinhStretchInterface.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017-2021 Mark Shelley
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

#ifndef __ArcsinhStretchInterface_h
#define __ArcsinhStretchInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Frame.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>

#include "ArcsinhStretchInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ArcsinhStretchInterface : public ProcessInterface
{
public:

   ArcsinhStretchInterface();
   virtual ~ArcsinhStretchInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool WantsRealTimePreviewNotifications() const override;
   void RealTimePreviewOwnerChanged(ProcessInterface& iface) override;
   void RealTimePreviewUpdated(bool active) override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool RequiresRealTimePreviewUpdate(const UInt16Image&, const View&, const Rect&, int zoomLevel) const override;
   bool GenerateRealTimePreview(UInt16Image&, const View&, const Rect&, int zoomLevel, String& info) const override;

private:

   ArcsinhStretchInstance instance;

   class RealTimeThread : public Thread
   {
   public:

      UInt16Image m_image;

      RealTimeThread();

      void Reset( const UInt16Image&, const ArcsinhStretchInstance& );

      void Run() override;

   private:

      ArcsinhStretchInstance m_instance;
   };

   struct GUIData
   {
      GUIData( ArcsinhStretchInterface& );

      VerticalSizer     Global_Sizer;
         NumericControl    Stretch_NumericControl;
         VerticalSizer     BlackPoint_Sizer;
            NumericControl    BlackPoint_NumericControl;
            HorizontalSizer   FineAdjust_Sizer;
               Slider            FineAdjust_Slider;
         HorizontalSizer   ParameterProtectHighlights_Sizer;
            CheckBox          ParameterProtectHighlights_CheckBox;
         HorizontalSizer   ParameterUseRgbws_Sizer;
            CheckBox          ParameterUseRgbws_CheckBox;
         GroupBox          PreviewControlsGroupBox;
         HorizontalSizer   PreviewControls_Sizer;
            PushButton        EstimateBlackPoint_Button;
            CheckBox          ParameterPreviewClipped_CheckBox;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;
   mutable AutoPointer<RealTimeThread> m_realTimeThread;
   mutable double m_preview_blackpoint_level = 0;

   void UpdateControls();
   void UpdateSliderControls();
   void UpdateRealTimePreview();

   void __RealValueUpdated( NumericEdit& sender, double value );
   void __SliderValueUpdated( Slider& sender, int value );
   void __ItemClicked( Button& sender, bool checked );
   void __UpdateRealTimePreview_Timer( Timer& );
   void __FineAdjustSliderEnter( Control& sender );
   void __FineAdjustSliderLeave( Control& sender );
   void __FineAdjustSliderGetFocus( Control& sender );
   void __FineAdjustSliderLoseFocus( Control& sender );
   void __FineAdjustSliderMousePress( Control& sender, const pcl::Point &pos, int button, unsigned buttons, unsigned modifiers );
   void __FineAdjustSliderMouseRelease( Control& sender, const pcl::Point &pos, int button, unsigned buttons, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ArcsinhStretchInterface* TheArcsinhStretchInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ArcsinhStretchInterface_h

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchInterface.h - Released 2024-12-28T16:54:15Z
