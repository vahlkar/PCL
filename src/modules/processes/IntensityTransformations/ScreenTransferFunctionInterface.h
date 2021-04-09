//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.1
// ----------------------------------------------------------------------------
// ScreenTransferFunctionInterface.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#ifndef __ScreenTransferFunctionInterface_h
#define __ScreenTransferFunctionInterface_h

#include <pcl/Atomic.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "ScreenTransferFunctionInstance.h"
#include "STFSliders.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class STFAutoStretchDialog;

class ScreenTransferFunctionInterface : public ProcessInterface
{
public:

   enum working_mode { EditMode, ZoomInMode, ZoomOutMode, PanMode };
   enum readout_mode { NoReadout, BlackPointReadout, MidtonesReadout, WhitePointReadout };

   ScreenTransferFunctionInterface();
   virtual ~ScreenTransferFunctionInterface();

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
   void ImageFocused( const View& ) override;
   void ImageUpdated( const View& ) override;
   void ImageSTFEnabled( const View& ) override;
   void ImageSTFDisabled( const View& ) override;
   void ImageSTFUpdated( const View& ) override;
   void ImageRenamed( const View& ) override;
   void ImageDeleted( const View& ) override;
   bool WantsReadoutNotifications() const override;
   void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A ) override;

   working_mode WorkingMode() const
   {
      return m_mode;
   }

   void SetWorkingMode( working_mode );

   void LinkRGBChannels( bool linked );

   void ComputeAutoStretch( View&, bool boost = false );

   void ApplyTo( View& ) const;
   void ApplyToCurrentView() const;

private:

   ScreenTransferFunctionInstance m_instance;
   working_mode                   m_mode             = EditMode;
   readout_mode                   m_readoutMode      = NoReadout;
   bool                           m_rgbLinked        = true;
   STFAutoStretchDialog*          m_autoAdjustDialog = nullptr;

   PCL_CLASS_REENTRANCY_GUARD

   struct GUIData
   {
      GUIData( ScreenTransferFunctionInterface& );

      VerticalSizer     Global_Sizer;
         HorizontalSizer   STF_Sizer[ 4 ];
            ToolButton        LinkRGB_ToolButton;
            ToolButton        Zoom11_ToolButton;
            ToolButton        Auto_ToolButton;
            ToolButton        Edit_ToolButton;
            ToolButton        EditMode_ToolButton;
            ToolButton        ZoomInMode_ToolButton;
            ToolButton        ZoomOutMode_ToolButton;
            ToolButton        PanMode_ToolButton;
            Label             ChannelId_Label[ 4 ];
            STFSliders        Sliders_Control[ 4 ];
            ToolButton        Reset_ToolButton[ 4 ];
            ToolButton        ReadoutShadows_ToolButton;
            ToolButton        ReadoutMidtones_ToolButton;
            ToolButton        ReadoutHighlights_ToolButton;
            ToolButton        STFEnabled_ToolButton;
   };

   GUIData* GUI = nullptr;

   void UpdateTitle( const View& );
   void UpdateControls();
   void UpdateFirstChannelLabelText();
   void UpdateSTFEnabledButton( const View& );

   void __SliderValueUpdated( STFSliders& sender, int channel, int item,
                              double value, unsigned modifiers, bool final );

   void __SliderRangeUpdated( STFSliders& sender, int channel,
                              double v0, double v1, unsigned modifiers );

   void __ModeButtonClick( Button& sender, bool checked );
   void __ResetButtonClick( Button& sender, bool checked );
   void __ReadoutButtonClick( Button& sender, bool checked );
   void __STFEnabledButtonClick( Button& sender, bool checked );

   void __STFAutoStretch_MouseRelease( Control& sender, const pcl::Point& pos,
                                       int button, unsigned buttons, unsigned modifiers );

   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ScreenTransferFunctionInterface* TheScreenTransferFunctionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ScreenTransferFunctionInterface_h

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionInterface.h - Released 2021-04-09T19:41:48Z
