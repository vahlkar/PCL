//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 2.2.8
// ----------------------------------------------------------------------------
// SpectrophotometricFluxCalibrationInterface.h - Released 2024-12-23T11:33:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#ifndef __SpectrophotometricFluxCalibrationInterface_h
#define __SpectrophotometricFluxCalibrationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "SpectrophotometricFluxCalibrationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SpectrophotometricFluxCalibrationInterface : public ProcessInterface
{
public:

   SpectrophotometricFluxCalibrationInterface();
   virtual ~SpectrophotometricFluxCalibrationInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsGlobalNotifications() const override;
   void GlobalFiltersUpdated() override;

private:

   SpectrophotometricFluxCalibrationInstance m_instance;

   struct GUIData
   {
      GUIData( SpectrophotometricFluxCalibrationInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        SystemParameters_SectionBar;
      Control           SystemParameters_Control;
      VerticalSizer     SystemParameters_Sizer;

         HorizontalSizer   DeviceQE_Sizer;
            Label             DeviceQE_Label;
            ComboBox          DeviceQE_ComboBox;

         Control        BroadbandFilters_Control;
         VerticalSizer  BroadbandFilters_Sizer;
            HorizontalSizer   GrayFilter_Sizer;
               Label             GrayFilter_Label;
               ComboBox          GrayFilter_ComboBox;
            HorizontalSizer   RedFilter_Sizer;
               Label             RedFilter_Label;
               ComboBox          RedFilter_ComboBox;
            HorizontalSizer   GreenFilter_Sizer;
               Label             GreenFilter_Label;
               ComboBox          GreenFilter_ComboBox;
            HorizontalSizer   BlueFilter_Sizer;
               Label             BlueFilter_Label;
               ComboBox          BlueFilter_ComboBox;

         Control        NarrowbandFilters_Control;
         VerticalSizer  NarrowbandFilters_Sizer;
            HorizontalSizer   FiltersTitle_Sizer;
               Label             FilterWavelengthTitle_Label;
               Label             FilterBandwidthTitle_Label;
            HorizontalSizer   GrayFilterParameters_Sizer;
               NumericEdit       GrayFilterWavelength_NumericEdit;
               NumericEdit       GrayFilterBandwidth_NumericEdit;
            HorizontalSizer   RedFilterParameters_Sizer;
               NumericEdit       RedFilterWavelength_NumericEdit;
               NumericEdit       RedFilterBandwidth_NumericEdit;
            HorizontalSizer   GreenFilterParameters_Sizer;
               NumericEdit       GreenFilterWavelength_NumericEdit;
               NumericEdit       GreenFilterBandwidth_NumericEdit;
            HorizontalSizer   BlueFilterParameters_Sizer;
               NumericEdit       BlueFilterWavelength_NumericEdit;
               NumericEdit       BlueFilterBandwidth_NumericEdit;

         HorizontalSizer   NarrowbandMode_Sizer;
            CheckBox          NarrowbandMode_CheckBox;
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

      SectionBar        CatalogSearch_SectionBar;
      Control           CatalogSearch_Control;
      VerticalSizer     CatalogSearch_Sizer;
         HorizontalSizer   Catalog_Sizer;
            Label             Catalog_Label;
            ComboBox          Catalog_ComboBox;
         HorizontalSizer   AutoLimitMagnitude_Sizer;
            CheckBox          AutoLimitMagnitude_CheckBox;
         NumericEdit       LimitMagnitude_NumericEdit;
         NumericEdit       MinMagnitude_NumericEdit;

      SectionBar        SignalEvaluation_SectionBar;
      Control           SignalEvaluation_Control;
      VerticalSizer     SignalEvaluation_Sizer;
         HorizontalSizer   StructureLayers_Sizer;
            Label             StructureLayers_Label;
            SpinBox           StructureLayers_SpinBox;
         NumericControl    SaturationThreshold_NumericControl;
         HorizontalSizer   SaturationRelative_Sizer;
            CheckBox          SaturationRelative_CheckBox;
         NumericControl    RejectionLimit_NumericControl;
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
   };

   GUIData* GUI = nullptr;

   void RegenerateDatabaseAccessControls();
   void UpdateControls();

   void e_GetFocus( Control& sender );
   void e_EditCompleted( Edit& sender );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Click( Button& sender, bool checked );
   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_SpinValueUpdated( SpinBox& sender, int value );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SpectrophotometricFluxCalibrationInterface* TheSpectrophotometricFluxCalibrationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SpectrophotometricFluxCalibrationInterface_h

// ----------------------------------------------------------------------------
// EOF SpectrophotometricFluxCalibrationInterface.h - Released 2024-12-23T11:33:28Z
