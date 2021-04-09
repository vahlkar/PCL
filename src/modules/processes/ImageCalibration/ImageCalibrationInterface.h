//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.5.1
// ----------------------------------------------------------------------------
// ImageCalibrationInterface.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#ifndef __ImageCalibrationInterface_h
#define __ImageCalibrationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/ToolButton.h>
#include <pcl/PushButton.h>
#include <pcl/NumericControl.h>
#include <pcl/TreeBox.h>
#include <pcl/SpinBox.h>
#include <pcl/ComboBox.h>
#include <pcl/CheckBox.h>

#include "ImageCalibrationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ImageCalibrationInterface : public ProcessInterface
{
public:

   ImageCalibrationInterface();
   virtual ~ImageCalibrationInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   void SaveSettings() const override;

private:

   ImageCalibrationInstance m_instance;

   struct GUIData
   {
      GUIData( ImageCalibrationInterface& );

      VerticalSizer  Global_Sizer;

      SectionBar        TargetImages_SectionBar;
      Control           TargetImages_Control;
      HorizontalSizer   TargetImages_Sizer;
         TreeBox           TargetImages_TreeBox;
         VerticalSizer     TargetButtons_Sizer;
            PushButton        AddFiles_PushButton;
            PushButton        SelectAll_PushButton;
            PushButton        InvertSelection_PushButton;
            PushButton        ToggleSelected_PushButton;
            PushButton        RemoveSelected_PushButton;
            PushButton        Clear_PushButton;
            CheckBox          FullPaths_CheckBox;

      HorizontalSizer   DetectCFA_Sizer;
         CheckBox          DetectCFA_CheckBox;
      HorizontalSizer   CFAPattern_Sizer;
         Label             CFAPattern_Label;
         ComboBox          CFAPattern_ComboBox;

      SectionBar        FormatHints_SectionBar;
      Control           FormatHints_Control;
      VerticalSizer     FormatHints_Sizer;
         HorizontalSizer   InputHints_Sizer;
            Label             InputHints_Label;
            Edit              InputHints_Edit;
         HorizontalSizer   OutputHints_Sizer;
            Label             OutputHints_Label;
            Edit              OutputHints_Edit;

      SectionBar        OutputFiles_SectionBar;
      Control           OutputFiles_Control;
      VerticalSizer     OutputFiles_Sizer;
         HorizontalSizer   OutputDirectory_Sizer;
            Label             OutputDirectory_Label;
            Edit              OutputDirectory_Edit;
            ToolButton        OutputDirectory_ToolButton;
         HorizontalSizer   OutputChunks_Sizer;
            Label             OutputPrefix_Label;
            Edit              OutputPrefix_Edit;
            Label             OutputPostfix_Label;
            Edit              OutputPostfix_Edit;
         HorizontalSizer   OutputSampleFormat_Sizer;
            Label             OutputSampleFormat_Label;
            ComboBox          OutputSampleFormat_ComboBox;
         HorizontalSizer   OutputPedestal_Sizer;
            Label             OutputPedestal_Label;
            SpinBox           OutputPedestal_SpinBox;
         HorizontalSizer   EvaluateNoise_Sizer;
            CheckBox          EvaluateNoise_CheckBox;
         HorizontalSizer   NoiseEvaluation_Sizer;
            Label             NoiseEvaluation_Label;
            ComboBox          NoiseEvaluation_ComboBox;
         HorizontalSizer   OverwriteExistingFiles_Sizer;
            CheckBox          OverwriteExistingFiles_CheckBox;
         HorizontalSizer   OnError_Sizer;
            Label             OnError_Label;
            ComboBox          OnError_ComboBox;

      SectionBar        Pedestal_SectionBar;
      Control           Pedestal_Control;
      VerticalSizer     Pedestal_Sizer;
         HorizontalSizer   PedestalMode_Sizer;
            Label             PedestalMode_Label;
            ComboBox          PedestalMode_ComboBox;
         HorizontalSizer   PedestalValue_Sizer;
            Label             PedestalValue_Label;
            SpinBox           PedestalValue_SpinBox;
         HorizontalSizer   PedestalKeyword_Sizer;
            Label             PedestalKeyword_Label;
            Edit              PedestalKeyword_Edit;

      SectionBar        Overscan_SectionBar;
      Control           Overscan_Control;
      VerticalSizer     Overscan_Sizer;
         HorizontalSizer   ImageRegion_Sizer;
            Label             ImageRegion_Label;
            NumericEdit          ImageRegionX0_NumericEdit;
            NumericEdit          ImageRegionY0_NumericEdit;
            NumericEdit          ImageRegionWidth_NumericEdit;
            NumericEdit          ImageRegionHeight_NumericEdit;
         HorizontalSizer   Overscan1_Sizer;
            CheckBox          Overscan1_CheckBox;
         HorizontalSizer   Overscan1Source_Sizer;
            Label             Overscan1Source_Label;
            NumericEdit          Overscan1SourceX0_NumericEdit;
            NumericEdit          Overscan1SourceY0_NumericEdit;
            NumericEdit          Overscan1SourceWidth_NumericEdit;
            NumericEdit          Overscan1SourceHeight_NumericEdit;
         HorizontalSizer   Overscan1Target_Sizer;
            Label             Overscan1Target_Label;
            NumericEdit          Overscan1TargetX0_NumericEdit;
            NumericEdit          Overscan1TargetY0_NumericEdit;
            NumericEdit          Overscan1TargetWidth_NumericEdit;
            NumericEdit          Overscan1TargetHeight_NumericEdit;
         HorizontalSizer   Overscan2_Sizer;
            CheckBox          Overscan2_CheckBox;
         HorizontalSizer   Overscan2Source_Sizer;
            Label             Overscan2Source_Label;
            NumericEdit          Overscan2SourceX0_NumericEdit;
            NumericEdit          Overscan2SourceY0_NumericEdit;
            NumericEdit          Overscan2SourceWidth_NumericEdit;
            NumericEdit          Overscan2SourceHeight_NumericEdit;
         HorizontalSizer   Overscan2Target_Sizer;
            Label             Overscan2Target_Label;
            NumericEdit          Overscan2TargetX0_NumericEdit;
            NumericEdit          Overscan2TargetY0_NumericEdit;
            NumericEdit          Overscan2TargetWidth_NumericEdit;
            NumericEdit          Overscan2TargetHeight_NumericEdit;
         HorizontalSizer   Overscan3_Sizer;
            CheckBox          Overscan3_CheckBox;
         HorizontalSizer   Overscan3Source_Sizer;
            Label             Overscan3Source_Label;
            NumericEdit          Overscan3SourceX0_NumericEdit;
            NumericEdit          Overscan3SourceY0_NumericEdit;
            NumericEdit          Overscan3SourceWidth_NumericEdit;
            NumericEdit          Overscan3SourceHeight_NumericEdit;
         HorizontalSizer   Overscan3Target_Sizer;
            Label             Overscan3Target_Label;
            NumericEdit          Overscan3TargetX0_NumericEdit;
            NumericEdit          Overscan3TargetY0_NumericEdit;
            NumericEdit          Overscan3TargetWidth_NumericEdit;
            NumericEdit          Overscan3TargetHeight_NumericEdit;
         HorizontalSizer   Overscan4_Sizer;
            CheckBox          Overscan4_CheckBox;
         HorizontalSizer   Overscan4Source_Sizer;
            Label             Overscan4Source_Label;
            NumericEdit          Overscan4SourceX0_NumericEdit;
            NumericEdit          Overscan4SourceY0_NumericEdit;
            NumericEdit          Overscan4SourceWidth_NumericEdit;
            NumericEdit          Overscan4SourceHeight_NumericEdit;
         HorizontalSizer   Overscan4Target_Sizer;
            Label             Overscan4Target_Label;
            NumericEdit          Overscan4TargetX0_NumericEdit;
            NumericEdit          Overscan4TargetY0_NumericEdit;
            NumericEdit          Overscan4TargetWidth_NumericEdit;
            NumericEdit          Overscan4TargetHeight_NumericEdit;

      SectionBar        MasterBias_SectionBar;
      Control           MasterBias_Control;
      VerticalSizer     MasterBias_Sizer;
         HorizontalSizer   MasterBiasPath_Sizer;
            Edit              MasterBiasPath_Edit;
            ToolButton        MasterBiasPath_ToolButton;
         HorizontalSizer   CalibrateMasterBias_Sizer;
            CheckBox          CalibrateMasterBias_CheckBox;

      SectionBar        MasterDark_SectionBar;
      Control           MasterDark_Control;
      VerticalSizer     MasterDark_Sizer;
         HorizontalSizer   MasterDarkPath_Sizer;
            Edit              MasterDarkPath_Edit;
            ToolButton        MasterDarkPath_ToolButton;
         HorizontalSizer   CalibrateMasterDark_Sizer;
            CheckBox          CalibrateMasterDark_CheckBox;
         HorizontalSizer   OptimizeDarks_Sizer;
            CheckBox          OptimizeDarks_CheckBox;
         NumericControl    DarkOptimizationThreshold_NumericControl;
         HorizontalSizer   DarkOptimizationWindow_Sizer;
            Label             DarkOptimizationWindow_Label;
            SpinBox           DarkOptimizationWindow_SpinBox;

      SectionBar        MasterFlat_SectionBar;
      Control           MasterFlat_Control;
      VerticalSizer     MasterFlat_Sizer;
         HorizontalSizer   MasterFlatPath_Sizer;
            Edit              MasterFlatPath_Edit;
            ToolButton        MasterFlatPath_ToolButton;
         HorizontalSizer   CalibrateMasterFlat_Sizer;
            CheckBox          CalibrateMasterFlat_CheckBox;
         HorizontalSizer   SeparateCFAFlatScalingFactors_Sizer;
            CheckBox          SeparateCFAFlatScalingFactors_CheckBox;
         NumericControl    FlatScaleClippingFactor_NumericControl;
   };

   GUIData* GUI = nullptr;

   /*
    * Interface Updates
    */
   void UpdateControls();
   void UpdateTargetImageItem( size_type );
   void UpdateTargetImagesList();
   void UpdateImageSelectionButtons();
   void UpdateCFAControls();
   void UpdateFormatHintsControls();
   void UpdateOutputFilesControls();
   void UpdatePedestalControls();
   void UpdateMasterFrameControls();
   void UpdateOverscanControls();

   /*
    * Event Handlers
    */
   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_NodeSelectionUpdated( TreeBox& sender );
   void e_Click( Button& sender, bool checked );
   void e_EditCompleted( Edit& sender );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_SpinValueUpdated( SpinBox& sender, int value );
   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_OverscanValueUpdated( NumericEdit& sender, double value );
   void e_CheckSection( SectionBar& sender, bool checked );
   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ImageCalibrationInterface* TheImageCalibrationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ImageCalibrationInterface_h

// ----------------------------------------------------------------------------
// EOF ImageCalibrationInterface.h - Released 2021-04-09T19:41:48Z
