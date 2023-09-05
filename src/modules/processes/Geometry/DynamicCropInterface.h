//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// DynamicCropInterface.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#ifndef __DynamicCropInterface_h
#define __DynamicCropInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Graphics.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "DynamicCropInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS View;

class DynamicCropInterface : public ProcessInterface
{
public:

   DynamicCropInterface();
   virtual ~DynamicCropInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void Execute() override;
   void EditPreferences() override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool IsDynamicInterface() const override;
   void ExitDynamicMode() override;
   void DynamicMouseEnter( View& ) override;
   void DynamicMouseMove( View&, const DPoint&, unsigned buttons, unsigned modifiers ) override;
   void DynamicMousePress( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers ) override;
   void DynamicMouseRelease( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers ) override;
   void DynamicMouseDoubleClick( View&, const DPoint&, unsigned buttons, unsigned modifiers ) override;
   bool DynamicKeyPress( View& v, int key, unsigned modifiers ) override;
   bool RequiresDynamicUpdate( const View&, const DRect& ) const override;
   void DynamicPaint( const View&, VectorGraphics&, const DRect& ) const override;
   bool WantsReadoutNotifications() const override;
   void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A ) override;
   void SaveSettings() const override;
   void LoadSettings() override;

private:

   DynamicCropInstance m_instance;

   union Flags
   {
      struct
      {
         bool moving       :  1; // moving the cropping rectangle
         bool movingCenter :  1; // moving the center of rotation
         bool rotating     :  1; // rotating the cropping rectangle
         bool resizing     :  1; // resizing the cropping rectangle
         bool resizeLeft   :  1; // resizing on left edge
         bool resizeTop    :  1; // resizing on top edge
         bool resizeRight  :  1; // resizing on right edge
         bool resizeBottom :  1; // resizing on bottom edge
         int               : 24;
      }
      bits;

      uint32 allBits;

      Flags() { allBits = 0; }
      Flags( const Flags& x ) { allBits = x.allBits; }

      bool operator ==( const Flags& x ) const { return allBits == x.allBits; }
   };

   View     m_view;                        // the dynamic target
   double   m_width = 0, m_height = 0;     // dimensions of the cropping rectangle
   DPoint   m_center = 0.0;                // center of cropping rectangle
   DPoint   m_rotationCenter = 0.0;        // center of rotation
   bool     m_rotationFixed = false;       // true if rotation center is fixed, false if it moves with the cropping rect.
   int      m_anchorPoint = 4;             // anchor point. Rows (left,center,right): 0,1,2=top | 3,4,5=center | 6,7,8=bottom
   DPoint   m_anchor = 0.0;                // position of anchor point
   Flags    m_flags;                       // current operation flags
   bool     m_dragging = false;            // dragging the mouse
   DPoint   m_dragOrigin = 0.0;            // initial drag position
   bool     m_initializing = false;        // defining the initial cropping rectangle
   Rect     m_rect = 0;                    // initial cropping rectangle
   RGBA     m_selectionColor = 0xFFFFFFFF; // color for the cropping rectangle
   RGBA     m_centerColor = 0xFFFFFFFF;    // color for the center mark
   RGBA     m_fillColor = 0x28FFFFFF;      // color to fill the cropping rectangle

   using ProcessInterface::Initialize;
   void Initialize( const Rect& );

   void GetRotatedRect( DPoint& topLeft, DPoint& topRight, DPoint& bottomLeft, DPoint& bottomRight ) const;
   void GetRotatedBounds( DRect& ) const;

   void GetUnrotatedRect( DRect& ) const;

   void PaintRect( VectorGraphics&, ImageWindow& ) const;

   Flags OperationInfo( const DPoint& ) const;

   void UpdateDynamicCursor() const;

   void BeginOperation( const DPoint&, unsigned modifiers );
   void UpdateOperation( const DPoint&, unsigned modifiers );
   void UpdateRotation( DPoint&, unsigned modifiers );
   void UpdateCenterMove( DPoint&, unsigned modifiers );
   void UpdateMove( DPoint&, unsigned modifiers );
   void UpdateResize( DPoint&, unsigned modifiers );
   void EndOperation();

   void SetRotationAngle( double a );
   void SetRotationCenter( const DPoint& );
   void MoveTo( const DPoint& );
   void ResizeBy( double dL, double dT, double dR, double dB );

   void UpdateAnchorPosition();

   void UpdateInstance();

   bool IsPointInsideRect( const DPoint& ) const;
   bool IsPointNearRect( const DPoint& ) const;
   bool IsPointOnRectEdges( const DPoint&, bool& left, bool& top, bool& right, bool& bottom ) const;
   bool IsPointOnRectCenter( const DPoint& ) const;
   bool IsPointOnRotationCenter( const DPoint& ) const;

   // Rotation angle of a point with respect to the center of the cropping system
   double RotationAngle( const DPoint& p ) const
   {
      return ArcTan( m_rotationCenter.y - p.y, p.x - m_rotationCenter.x );
   }

   struct GUIData
   {
      GUIData( DynamicCropInterface& );

      VerticalSizer  Global_Sizer;

      SectionBar        SizePos_SectionBar;
      Control           SizePos_Control;
      HorizontalSizer   SizePos_Sizer;
         VerticalSizer     SizePosLeft_Sizer;
            NumericEdit       Width_NumericEdit;
            NumericEdit       Height_NumericEdit;
            NumericEdit       PosX_NumericEdit;
            NumericEdit       PosY_NumericEdit;
         Control           AnchorSelectors_Control;

      SectionBar        Rotation_SectionBar;
      Control           Rotation_Control;
      VerticalSizer     Rotation_Sizer;
         HorizontalSizer   RotationTop_Sizer;
            VerticalSizer     RotationLeft_Sizer;
               NumericEdit       Angle_NumericEdit;
               HorizontalSizer   Clockwise_Sizer;
                  Label             Clockwise_Label;
                  CheckBox          Clockwise_CheckBox;
               NumericEdit       CenterX_NumericEdit;
               NumericEdit       CenterY_NumericEdit;
            Control           Dial_Control;
         HorizontalSizer   RotationBottom_Sizer;
            CheckBox          OptimizeFast_CheckBox;

      SectionBar        Scale_SectionBar;
      Control           Scale_Control;
      VerticalSizer     Scale_Sizer;
         NumericEdit       ScaleX_NumericEdit;
         NumericEdit       ScaleY_NumericEdit;
         NumericEdit       ScaledWidthPx_NumericEdit;
         NumericEdit       ScaledHeightPx_NumericEdit;
         NumericEdit       ScaledWidthCm_NumericEdit;
         NumericEdit       ScaledHeightCm_NumericEdit;
         NumericEdit       ScaledWidthIn_NumericEdit;
         NumericEdit       ScaledHeightIn_NumericEdit;
         HorizontalSizer   PreserveAspectRatio_Sizer;
            CheckBox          PreserveAspectRatio_CheckBox;

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

      SectionBar        Resolution_SectionBar;
      Control           Resolution_Control;
      VerticalSizer     Resolution_Sizer;
         NumericEdit       HorizontalResolution_NumericEdit;
         NumericEdit       VerticalResolution_NumericEdit;
         HorizontalSizer   ResolutionUnit_Sizer;
            RadioButton       CentimeterUnits_RadioButton;
            RadioButton       InchUnits_RadioButton;
         HorizontalSizer   ForceResolution_Sizer;
            CheckBox          ForceResolution_CheckBox;

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

   void InitControls();
   void UpdateControls();
   void UpdateSizePosControls();
   void UpdateRotationControls();
   void UpdateScaleControls();
   void UpdateInterpolationControls();
   void UpdateResolutionControls();
   void UpdateFillColorControls();
   void UpdateView();

   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_ButtonClick( Button& sender, bool checked );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Paint( Control& sender, const Rect& updateRect );
   void e_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseMove( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void e_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseDoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );

   friend struct GUIData;
   friend class DynamicCropPreferencesDialog;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern DynamicCropInterface* TheDynamicCropInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DynamicCropInterface_h

// ----------------------------------------------------------------------------
// EOF DynamicCropInterface.h - Released 2023-08-28T15:23:41Z
