//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// RotationInterface.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#ifndef __RotationInterface_h
#define __RotationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "RotationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RotationInterface : public ProcessInterface
{
public:

   RotationInterface();
   virtual ~RotationInterface();

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
   bool WantsReadoutNotifications() const override;
   void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A ) override;

private:

   RotationInstance m_instance;

   struct GUIData
   {
      GUIData( RotationInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        Rotation_SectionBar;
      Control           Rotation_Control;
      HorizontalSizer   Rotation_Sizer;
         VerticalSizer     RotationLeft_Sizer;
            NumericEdit       Angle_NumericEdit;
            HorizontalSizer   Clockwise_Sizer;
               Label             Clockwise_Label;
               CheckBox          Clockwise_CheckBox;
            HorizontalSizer   OptimizeFast_Sizer;
               Label             OptimizeFast_Label;
               CheckBox          OptimizeFast_CheckBox;
         Control           Dial_Control;

      SectionBar        Interpolation_SectionBar;
      Control           Interpolation_Control;
      VerticalSizer     Interpolation_Sizer;
         HorizontalSizer   Algorithm_Sizer;
            Label             Algorithm_Label;
            ComboBox          Algorithm_ComboBox;
         NumericEdit       ClampingThreshold_NumericEdit;
         NumericEdit       Smoothness_NumericEdit;
         HorizontalSizer   GammaCorrection_Sizer;
            CheckBox          GammaCorrection_CheckBox;

      SectionBar        FillColor_SectionBar;
      Control           FillColor_Control;
      VerticalSizer     FillColor_Sizer;
         NumericControl    Red_NumericControl;
         NumericControl    Green_NumericControl;
         NumericControl    Blue_NumericControl;
         NumericControl    Alpha_NumericControl;
         Control           ColorSample_Control;
   };

   GUIData* GUI = nullptr;

   bool m_dragging = false; // dragging the mouse cursor on the angle dial

   void UpdateControls();
   void UpdateNumericControls();
   void UpdateFillColorControls();

   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_ButtonClick( Button& sender, bool checked );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Paint( Control& sender, const Rect& updateRect );
   void e_MouseMove( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void e_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern RotationInterface* TheRotationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RotationInterface_h

// ----------------------------------------------------------------------------
// EOF RotationInterface.h - Released 2024-12-28T16:54:15Z
