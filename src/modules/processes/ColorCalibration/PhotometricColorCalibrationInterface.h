//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationInterface.h - Released 2023-12-01T19:16:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#ifndef __PhotometricColorCalibrationInterface_h
#define __PhotometricColorCalibrationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>

#include "PhotometricColorCalibrationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PhotometricColorCalibrationInterface : public ProcessInterface
{
public:

   PhotometricColorCalibrationInterface();
   virtual ~PhotometricColorCalibrationInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   PhotometricColorCalibrationInstance m_instance;

   struct GUIData
   {
      GUIData( PhotometricColorCalibrationInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        Calibration_SectionBar;
      Control           Calibration_Control;
      VerticalSizer     Calibration_Sizer;
         HorizontalSizer   WhiteReference_Sizer;
            Label             WhiteReference_Label;
            ComboBox          WhiteReference_ComboBox;
         HorizontalSizer   GenerateGraphs_Sizer;
            CheckBox          GenerateGraphs_CheckBox;
         HorizontalSizer   GenerateStarMaps_Sizer;
            CheckBox          GenerateStarMaps_CheckBox;
         HorizontalSizer   GenerateTextFiles_Sizer;
            CheckBox          GenerateTextFiles_CheckBox;
         HorizontalSizer   OutputDirectory_Sizer;
            Label             OutputDirectory_Label;
            Edit              OutputDirectory_Edit;
            ToolButton        OutputDirectory_ToolButton;
         HorizontalSizer   ApplyColorCalibration_Sizer;
            CheckBox          ApplyColorCalibration_CheckBox;

      SectionBar        CatalogSearch_SectionBar;
      Control           CatalogSearch_Control;
      VerticalSizer     CatalogSearch_Sizer;
         HorizontalSizer   AutoCatalog_Sizer;
            CheckBox          AutoCatalog_CheckBox;
         HorizontalSizer   Catalog_Sizer;
            Label             Catalog_Label;
            ComboBox          Catalog_ComboBox;
         HorizontalSizer   Server_Sizer;
            Label             Server_Label;
            ComboBox          Server_ComboBox;
         HorizontalSizer   AutoLimitMagnitude_Sizer;
            CheckBox          AutoLimitMagnitude_CheckBox;
         NumericEdit       LimitMagnitude_NumericEdit;

      SectionBar        SignalEvaluation_SectionBar;
      Control           SignalEvaluation_Control;
      VerticalSizer     SignalEvaluation_Sizer;
         HorizontalSizer   StructureLayers_Sizer;
            Label             StructureLayers_Label;
            SpinBox           StructureLayers_SpinBox;
         NumericControl    SaturationThreshold_NumericControl;
         HorizontalSizer   SaturationRelative_Sizer;
            CheckBox          SaturationRelative_CheckBox;
         HorizontalSizer   PSFNoiseLayers_Sizer;
            Label             PSFNoiseLayers_Label;
            SpinBox           PSFNoiseLayers_SpinBox;
         HorizontalSizer   PSFMinStructureSize_Sizer;
            Label             PSFMinStructureSize_Label;
            SpinBox           PSFMinStructureSize_SpinBox;
         HorizontalSizer   PSFHotPixelFilterRadius_Sizer;
            Label             PSFHotPixelFilterRadius_Label;
            SpinBox           PSFHotPixelFilterRadius_SpinBox;
         HorizontalSizer   PSFNoiseReductionFilterRadius_Sizer;
            Label             PSFNoiseReductionFilterRadius_Label;
            SpinBox           PSFNoiseReductionFilterRadius_SpinBox;
         NumericEdit       PSFMinSNR_NumericEdit;
         HorizontalSizer   PSFType_Sizer;
            Label             PSFType_Label;
            ComboBox          PSFType_ComboBox;
         NumericControl    PSFGrowth_NumericControl;
         HorizontalSizer   PSFMaxStars_Sizer;
            Label             PSFMaxStars_Label;
            SpinBox           PSFMaxStars_SpinBox;
         HorizontalSizer   PSFAllowClusteredSources_Sizer;
            CheckBox          PSFAllowClusteredSources_CheckBox;

      SectionBar        BackgroundReference_SectionBar;
      Control           BackgroundReference_Control;
      VerticalSizer     BackgroundReference_Sizer;
         HorizontalSizer   BackgroundReferenceViewId_Sizer;
            Label             BackgroundReferenceViewId_Label;
            Edit              BackgroundReferenceViewId_Edit;
            ToolButton        BackgroundReferenceViewId_ToolButton;
         NumericControl    BackgroundLow_NumericControl;
         NumericControl    BackgroundHigh_NumericControl;
         GroupBox          BackgroundROI_GroupBox;
         VerticalSizer     BackgroundROI_Sizer;
            HorizontalSizer   BackgroundROIRow1_Sizer;
               Label             BackgroundROIX0_Label;
               SpinBox           BackgroundROIX0_SpinBox;
               Label             BackgroundROIY0_Label;
               SpinBox           BackgroundROIY0_SpinBox;
            HorizontalSizer   BackgroundROIRow2_Sizer;
               Label             BackgroundROIWidth_Label;
               SpinBox           BackgroundROIWidth_SpinBox;
               Label             BackgroundROIHeight_Label;
               SpinBox           BackgroundROIHeight_SpinBox;
               PushButton        BackgroundROISelectPreview_Button;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_GetFocus( Control& sender );
   void e_EditCompleted( Edit& sender );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Click( Button& sender, bool checked );
   void e_SectionCheck( SectionBar& sender, bool checked );
   void e_GroupCheck( GroupBox& sender, bool checked );
   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_SpinValueUpdated( SpinBox& sender, int value );
   void e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern PhotometricColorCalibrationInterface* ThePhotometricColorCalibrationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PhotometricColorCalibrationInterface_h

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationInterface.h - Released 2023-12-01T19:16:17Z
