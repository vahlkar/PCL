//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// IntegerResampleInterface.h - Released 2023-08-28T15:23:41Z
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

#ifndef __IntegerResampleInterface_h
#define __IntegerResampleInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ViewList.h>

#include "IntegerResampleInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class IntegerResampleInterface : public ProcessInterface
{
public:

   IntegerResampleInterface();
   virtual ~IntegerResampleInterface();

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

private:

   IntegerResampleInstance m_instance;

   // Sample source dimensions in pixels.
   int sourceWidth = 1000;
   int sourceHeight = 1000;

   struct GUIData
   {
      GUIData( IntegerResampleInterface& );

      VerticalSizer     Global_Sizer;

      ViewList          AllImages_ViewList;

      SectionBar        IntegerResample_SectionBar;
      Control           IntegerResample_Control;
      VerticalSizer     IntegerResample_Sizer;
         HorizontalSizer   ResampleFactor_Sizer;
            Label             ResampleFactor_Label;
            SpinBox           ResampleFactor_SpinBox;
         HorizontalSizer   Downsample_Sizer;
            RadioButton       Downsample_RadioButton;
            Label             DownsampleMode_Label;
            ComboBox          DownsampleMode_ComboBox;
         HorizontalSizer   Upsample_Sizer;
            RadioButton       Upsample_RadioButton;
         HorizontalSizer   GammaCorrection_Sizer;
            CheckBox          GammaCorrection_CheckBox;

      SectionBar        Dimensions_SectionBar;
      Control           Dimensions_Control;
      VerticalSizer     Dimensions_Sizer;
         HorizontalSizer   DimensionsRow1_Sizer;
            Label             SourcePixels_Label;
            Label             TargetPixels_Label;
            Label             TargetCentimeters_Label;
            Label             TargetInches_Label;
         HorizontalSizer   DimensionsRow2_Sizer;
            Label             Width_Label;
            NumericEdit       SourceWidthPixels_NumericEdit;
            NumericEdit       TargetWidthPixels_NumericEdit;
            NumericEdit       TargetWidthCentimeters_NumericEdit;
            NumericEdit       TargetWidthInches_NumericEdit;
         HorizontalSizer   DimensionsRow3_Sizer;
            Label             Height_Label;
            NumericEdit       SourceHeightPixels_NumericEdit;
            NumericEdit       TargetHeightPixels_NumericEdit;
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
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_ViewSelected( ViewList& sender, View& view );
   void e_ButtonClick( Button& sender, bool checked );
   void e_SpinBoxValueUpdated( SpinBox& sender, int value );
   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern IntegerResampleInterface* TheIntegerResampleInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __IntegerResampleInterface_h

// ----------------------------------------------------------------------------
// EOF IntegerResampleInterface.h - Released 2023-08-28T15:23:41Z
