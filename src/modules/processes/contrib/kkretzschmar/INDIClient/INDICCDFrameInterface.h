//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.7
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.1
// ----------------------------------------------------------------------------
// INDICCDFrameInterface.h - Released 2020-12-17T15:46:56Z
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

#ifndef __INDICCDFrameInterface_h
#define __INDICCDFrameInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Timer.h>
#include <pcl/TreeBox.h>

#include "DeviceConfigBase.h"
#include "INDIClient.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class INDICCDFrameInterfaceExecution;

class FilterConfigDialog : public ConfigDialogBase
{
public:

   FilterConfigDialog( const String& deviceName );

   void addFilterName( size_t filterSlot, const String& filterName );
   void adjustTreeColumns();

private:

   HorizontalSizer FilterConfig_Sizer;
   TreeBox FilterNames_TreeBox;
   VerticalSizer FilterToolBox_Sizer;
   ToolButton FilterRename_ToolButton;

   void SendUpdatedProperties() override;

   void e_Click( Button& sender, bool checked );
};

// ----------------------------------------------------------------------------

class INDICCDFrameInterface : public ProcessInterface
{
public:

   INDICCDFrameInterface();
   virtual ~INDICCDFrameInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

   const String& CurrentDeviceName() const
   {
      return m_device;
   }

   const String CurrentFilterWheelDeviceName() const
   {
      String externalFilterWheelDeviceName = GUI->ExternalFilterDevice_Combo.ItemText( GUI->ExternalFilterDevice_Combo.CurrentItem() );
      return externalFilterWheelDeviceName != String( "<No filter wheel>" ) ? externalFilterWheelDeviceName : m_device;
   }

   int TelescopeFocalLength() const
   {
      return m_telescopeFocalLength;
   }

private:

   String m_device;
   INDICCDFrameInterfaceExecution* m_execution = nullptr;

   struct GUIData
   {
      GUIData( INDICCDFrameInterface& );

      Timer UpdateDeviceList_Timer;
      Timer UpdateDeviceProperties_Timer;

      VerticalSizer Global_Sizer;

      SectionBar ServerParameters_SectionBar;
      Control ServerParameters_Control;
      VerticalSizer ServerParameters_Sizer;
      HorizontalSizer CCDDevice_Sizer;
      Label CCDDevice_Label;
      ComboBox CCDDevice_Combo;
      Control CCDProperties_Control;
      VerticalSizer CCDProperties_Sizer;
      HorizontalSizer CCDTemp_HSizer;
      NumericEdit CCDTemp_NumericEdit;
      NumericControl CCDTargetTemp_NumericEdit;
      ToolButton CCDTargetTemp_ToolButton;
      HorizontalSizer CCDBinX_HSizer;
      Label CCDBinX_Label;
      ComboBox CCDBinX_Combo;
      HorizontalSizer CCDBinY_HSizer;
      Label CCDBinY_Label;
      ComboBox CCDBinY_Combo;
      HorizontalSizer CCDFilter_HSizer;
      Label CCDFilter_Label;
      ComboBox CCDFilter_Combo;
      ToolButton FilterConfig_ToolButton;
      HorizontalSizer CCDFrameType_HSizer;
      Label CCDFrameType_Label;
      ComboBox CCDFrameType_Combo;
      HorizontalSizer UploadMode_HSizer;
      Label UploadMode_Label;
      ComboBox UploadMode_Combo;
      HorizontalSizer ServerUploadDir_HSizer;
      Label ServerUploadDir_Label;
      Edit ServerUploadDir_Edit;
      ToolButton ServerUploadDir_ToolButton;
      HorizontalSizer ServerFileNameTemplate_HSizer;
      Label ServerFileNameTemplate_Label;
      Edit ServerFileNameTemplate_Edit;
      PushButton ServerFileNameTemplate_PushButton;

      SectionBar ClientParameters_SectionBar;
      Control ClientParameters_Control;
      VerticalSizer ClientParameters_Sizer;
      HorizontalSizer OpenClientFrames_Sizer;
      CheckBox OpenClientFrames_CheckBox;
      HorizontalSizer ReuseImageWindow_Sizer;
      CheckBox ReuseImageWindow_CheckBox;
      HorizontalSizer AutoStretch_Sizer;
      CheckBox AutoStretch_CheckBox;
      HorizontalSizer LinkedAutoStretch_Sizer;
      CheckBox LinkedAutoStretch_CheckBox;
      HorizontalSizer SaveClientFrames_Sizer;
      CheckBox SaveClientFrames_CheckBox;
      HorizontalSizer PlateSolving_Sizer;
      CheckBox PlateSolving_CheckBox;
      HorizontalSizer CenterTarget_Sizer;
      CheckBox CenterTarget_CheckBox;
      HorizontalSizer OverwriteClientFrames_Sizer;
      CheckBox OverwriteClientFrames_CheckBox;
      HorizontalSizer ClientDownloadDir_HSizer;
      Label ClientDownloadDir_Label;
      Edit ClientDownloadDir_Edit;
      ToolButton ClientDownloadDir_ToolButton;
      HorizontalSizer ClientFileNameTemplate_HSizer;
      Label ClientFileNameTemplate_Label;
      Edit ClientFileNameTemplate_Edit;
      HorizontalSizer ClientOutputFormatHints_HSizer;
      Label ClientOutputFormatHints_Label;
      Edit ClientOutputFormatHints_Edit;

      SectionBar FrameAcquisition_SectionBar;
      Control FrameAcquisition_Control;
      VerticalSizer FrameAcquisition_Sizer;
      HorizontalSizer ExposureParameters_Sizer;
      VerticalSizer ExposureParametersLeft_Sizer;
      NumericEdit ExposureTime_NumericEdit;
      NumericEdit ExposureDelay_NumericEdit;
      HorizontalSizer ExposureCount_Sizer;
      Label ExposureCount_Label;
      SpinBox ExposureCount_SpinBox;
      VerticalSizer ExposureParametersRight_Sizer;
      HorizontalSizer StartExposure_Sizer;
      PushButton StartExposure_PushButton;
      HorizontalSizer CancelExposure_Sizer;
      PushButton CancelExposure_PushButton;
      Label ExposureInfo_Label;
      HorizontalSizer ObjectName_Sizer;
      Label ObjectName_Label;
      Edit ObjectName_Edit;
      HorizontalSizer TelescopeDevice_Sizer;
      Label TelescopeDevice_Label;
      ComboBox TelescopeDevice_Combo;
      HorizontalSizer ExternalFilterDevice_Sizer;
      Label ExternalFilterDevice_Label;
      ComboBox ExternalFilterDevice_Combo;

      SectionBar        PlateSolverParameters_SectionBar;
         Control           PlateSolverParameters_Control;
         VerticalSizer     PlateSolverParameters_Sizer;
            HorizontalSizer   SolverAutoCatalog_Sizer;
               CheckBox          SolverAutoCatalog_CheckBox;
            HorizontalSizer   SolverCatalog_Sizer;
               Label             SolverCatalog_Label;
               ComboBox          SolverCatalog_ComboBox;
            HorizontalSizer   SolverAutoLimitMagnitude_Sizer;
               CheckBox          SolverAutoLimitMagnitude_CheckBox;
            HorizontalSizer   SolverLimitMagnitude_Sizer;
               Label             SolverLimitMagnitude_Label;
               SpinBox           SolverLimitMagnitude_SpinBox;
            HorizontalSizer   DistortionCorrection_Sizer;
               CheckBox          DistortionCorrection_CheckBox;
            HorizontalSizer   ForcePlateSolve_Sizer;
               CheckBox          ForcePlateSolve_CheckBox;
            HorizontalSizer   IgnorePositionAndScale_Sizer;
               CheckBox          IgnorePositionAndScale_CheckBox;
               ToolButton        ResetSolverConfiguration_ToolButton;

         SectionBar        AdvancedPlateSolverParameters_SectionBar;
         Control           AdvancedPlateSolverParameters_Control;
         VerticalSizer     AdvancedPlateSolverParameters_Sizer;
            HorizontalSizer   ProjectionSystem_Sizer;
               Label             ProjectionSystem_Label;
               ComboBox          ProjectionSystem_ComboBox;
            NumericControl    StarSensitivity_NumericControl;
            HorizontalSizer   NoiseReduction_Sizer;
               Label             NoiseReduction_Label;
               SpinBox           NoiseReduction_SpinBox;
            HorizontalSizer   AlignmentDevice_Sizer;
               Label             AlignmentDevice_Label;
               ComboBox          AlignmentDevice_ComboBox;
            NumericControl    SplineSmoothing_NumericControl;
   };

   GUIData* GUI = nullptr;

   int32_t m_telescopeFocalLength = 0;

   void UpdateControls();

   void e_Show( Control& );
   void e_Hide( Control& );
   void e_Timer( Timer& sender );
   void e_Click( Button& sender, bool checked );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_EditValueUpdated( NumericEdit& sender, double value );
   void e_SpinValueUpdated( SpinBox& sender, int value );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend class INDICCDFrameInterfaceExecution;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern INDICCDFrameInterface* TheINDICCDFrameInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __INDICCDFrameInterface_h

// ----------------------------------------------------------------------------
// EOF INDICCDFrameInterface.h - Released 2020-12-17T15:46:56Z
