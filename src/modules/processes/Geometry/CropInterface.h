//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// CropInterface.h - Released 2023-08-28T15:23:41Z
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

#ifndef __CropInterface_h
#define __CropInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/ViewList.h>

#include "CropInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CropInterface : public ProcessInterface
{
public:

   CropInterface();
   virtual ~CropInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void TrackViewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsImageNotifications() const override;
   void ImageUpdated( const View& ) override;
   void ImageFocused( const View& ) override;
   bool WantsReadoutNotifications() const override;
   void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A ) override;

private:

   CropInstance m_instance;

   // Sample source dimensions in pixels.
   int sourceWidth = 1000;
   int sourceHeight = 1000;

   // Current anchor point:
   // 0 1 2
   // 3 4 5
   // 6 7 8
   int anchor = 4; // center

   struct GUIData
   {
      GUIData( CropInterface& );

      VerticalSizer     Global_Sizer;

      ViewList          AllImages_ViewList;

      SectionBar        CropMargins_SectionBar;
      Control           CropMargins_Control;
      VerticalSizer     CropMargins_Sizer;
         HorizontalSizer   CropMarginsTop_Sizer;
            NumericEdit       TopMargin_NumericEdit;
         HorizontalSizer   CropMarginsMiddle_Sizer;
            NumericEdit       LeftMargin_NumericEdit;
            VerticalSizer        AnchorButtons_Sizer;
               HorizontalSizer      AnchorRow1_Sizer;
                  ToolButton           TL_ToolButton;
                  ToolButton           TM_ToolButton;
                  ToolButton           TR_ToolButton;
               HorizontalSizer      AnchorRow2_Sizer;
                  ToolButton           ML_ToolButton;
                  ToolButton           MM_ToolButton;
                  ToolButton           MR_ToolButton;
               HorizontalSizer      AnchorRow3_Sizer;
                  ToolButton           BL_ToolButton;
                  ToolButton           BM_ToolButton;
                  ToolButton           BR_ToolButton;
            NumericEdit       RightMargin_NumericEdit;
         HorizontalSizer   CropMarginsBottom_Sizer;
            NumericEdit       BottomMargin_NumericEdit;

      SectionBar        Dimensions_SectionBar;
      Control           Dimensions_Control;
      VerticalSizer     Dimensions_Sizer;
         HorizontalSizer   DimensionsRow1_Sizer;
            Label             SourcePixels_Label;
            Label             TargetPixels_Label;
            Label             TargetPercent_Label;
            Label             TargetCentimeters_Label;
            Label             TargetInches_Label;
         HorizontalSizer   DimensionsRow2_Sizer;
            Label             Width_Label;
            NumericEdit       SourceWidthPixels_NumericEdit;
            NumericEdit       TargetWidthPixels_NumericEdit;
            NumericEdit       TargetWidthPercent_NumericEdit;
            NumericEdit       TargetWidthCentimeters_NumericEdit;
            NumericEdit       TargetWidthInches_NumericEdit;
         HorizontalSizer   DimensionsRow3_Sizer;
            Label             Height_Label;
            NumericEdit       SourceHeightPixels_NumericEdit;
            NumericEdit       TargetHeightPixels_NumericEdit;
            NumericEdit       TargetHeightPercent_NumericEdit;
            NumericEdit       TargetHeightCentimeters_NumericEdit;
            NumericEdit       TargetHeightInches_NumericEdit;
         HorizontalSizer   DimensionsRow4_Sizer;
            Label             SizeInfo_Label;

      SectionBar        Resolution_SectionBar;
      Control           Resolution_Control;
      VerticalSizer     Resolution_Sizer;
         HorizontalSizer   ResolutionRow1_Sizer;
            NumericEdit       HorizontalResolution_NumericEdit;
            NumericEdit       VerticalResolution_NumericEdit;
         HorizontalSizer   ResolutionRow2_Sizer;
            RadioButton       CentimeterUnits_RadioButton;
            RadioButton       InchUnits_RadioButton;
            CheckBox          ForceResolution_CheckBox;

      SectionBar        Mode_SectionBar;
      Control           Mode_Control;
      HorizontalSizer   Mode_Sizer;
         Label             CropMode_Label;
         ComboBox          CropMode_ComboBox;

      SectionBar        FillColor_SectionBar;
      Control           FillColor_Control;
      VerticalSizer     FillColor_Sizer;
         NumericControl    Red_NumericControl;
         NumericControl    Green_NumericControl;
         NumericControl    Blue_NumericControl;
         NumericControl    Alpha_NumericControl;
         Control           ColorSample_Control;

      Bitmap L_Bitmap, R_Bitmap, T_Bitmap, B_Bitmap, TL_Bitmap, TR_Bitmap, BL_Bitmap, BR_Bitmap;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateAnchors();
   void UpdateNumericControls();
   void UpdateFillColorControls();

   void e_ViewSelected( ViewList& sender, View& view );
   void e_ButtonClick( Button& sender, bool checked );
   void e_Anchor_ButtonClick( Button& sender, bool checked );
   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_Width_ValueUpdated( NumericEdit& sender, double value );
   void e_Height_ValueUpdated( NumericEdit& sender, double value );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Paint( Control& sender, const Rect& updateRect );
   void e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern CropInterface* TheCropInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CropInterface_h

// ----------------------------------------------------------------------------
// EOF CropInterface.h - Released 2023-08-28T15:23:41Z
