//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 1.11.2
// ----------------------------------------------------------------------------
// DebayerInterface.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

#ifndef __DebayerInterface_h
#define __DebayerInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

#include "DebayerInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class DebayerInterface : public ProcessInterface
{
public:

   DebayerInterface();
   virtual ~DebayerInterface();

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

private:

   DebayerInstance m_instance;

   struct GUIData
   {
      GUIData( DebayerInterface& );

      VerticalSizer     Global_Sizer;

         VerticalSizer     GeneralParameters_Sizer;
            HorizontalSizer   Pattern_Sizer;
               Label             Pattern_Label;
               ComboBox          Pattern_ComboBox;
            HorizontalSizer   DebayerMethod_Sizer;
               Label             DebayerMethod_Label;
               ComboBox          DebayerMethod_ComboBox;
            HorizontalSizer   OutputMode_Sizer;
               Label             OutputMode_Label;
               ComboBox          OutputMode_ComboBox;

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
            HorizontalSizer   OutputOptions_Sizer;
               CheckBox          OverwriteExistingFiles_CheckBox;
               Label             OnError_Label;
               ComboBox          OnError_ComboBox;

         SectionBar        SignalEvaluation_SectionBar;
         Control           SignalEvaluation_Control;
         VerticalSizer     SignalEvaluation_Sizer;
            HorizontalSizer   StructureLayers_Sizer;
               Label             StructureLayers_Label;
               SpinBox           StructureLayers_SpinBox;
            NumericControl    SaturationThreshold_NumericControl;
            HorizontalSizer   SaturationRelative_Sizer;
               CheckBox          SaturationRelative_CheckBox;
            HorizontalSizer   NoiseLayers_Sizer;
               Label             NoiseLayers_Label;
               SpinBox           NoiseLayers_SpinBox;
            HorizontalSizer   MinStructureSize_Sizer;
               Label             MinStructureSize_Label;
               SpinBox           MinStructureSize_SpinBox;
            HorizontalSizer   HotPixelFilterRadius_Sizer;
               Label             HotPixelFilterRadius_Label;
               SpinBox           HotPixelFilterRadius_SpinBox;
            HorizontalSizer   NoiseReductionFilterRadius_Sizer;
               Label             NoiseReductionFilterRadius_Label;
               SpinBox           NoiseReductionFilterRadius_SpinBox;
            HorizontalSizer   PSFType_Sizer;
               Label             PSFType_Label;
               ComboBox          PSFType_ComboBox;
            NumericControl    PSFGrowth_NumericControl;
            HorizontalSizer   MaxStars_Sizer;
               Label             MaxStars_Label;
               SpinBox           MaxStars_SpinBox;

         SectionBar        NoiseEvaluation_SectionBar;
         Control           NoiseEvaluation_Control;
         VerticalSizer     NoiseEvaluation_Sizer;
            HorizontalSizer   NoiseEvaluationAlgorithm_Sizer;
               Label             NoiseEvaluationAlgorithm_Label;
               ComboBox          NoiseEvaluationAlgorithm_ComboBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateGeneralParameterControls();
   void UpdateTargetImageItem( size_type );
   void UpdateTargetImagesList();
   void UpdateImageSelectionButtons();
   void UpdateFormatHintsControls();
   void UpdateOutputFilesControls();
   void UpdateSignalAndNoiseEvaluationControls();

   void e_EditCompleted( Edit& sender );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Click( Button& sender, bool checked );
   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_NodeSelectionUpdated( TreeBox& sender );
   void e_SpinValueUpdated( SpinBox& sender, int value );
   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_CheckSection( SectionBar& sender, bool checked );
   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern DebayerInterface* TheDebayerInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DebayerInterface_h

// ----------------------------------------------------------------------------
// EOF DebayerInterface.h - Released 2023-08-28T15:23:41Z
