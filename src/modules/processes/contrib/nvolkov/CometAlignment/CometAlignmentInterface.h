//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.3.7
// ----------------------------------------------------------------------------
// CometAlignmentInterface.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2023 Nikolay Volkov
// Copyright (c) 2019-2023 Juan Conejero (PTeam)
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L.
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

#ifndef __CometAlignmentInterface_h
#define __CometAlignmentInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

#include "CometAlignmentInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CometAlignmentInterface : public ProcessInterface
{
public:

   CometAlignmentInterface();
   virtual ~CometAlignmentInterface();

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
   bool IsDynamicInterface() const override;
   bool EnterDynamicMode() override;
   void ExitDynamicMode() override;
   void DynamicMousePress( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers ) override;
   bool RequiresDynamicUpdate( const View&, const DRect& ) const override;
   void DynamicPaint( const View&, VectorGraphics&, const DRect& ) const override;
   void SaveSettings() const override;

private:

   CometAlignmentInstance m_instance;

   struct GUIData
   {
      GUIData( CometAlignmentInterface& );

      VerticalSizer Global_Sizer;

      TreeBox Monitor_TreeBox;

      SectionBar        TargetImages_SectionBar;
      Control           TargetImages_Control;
      HorizontalSizer   TargetImages_Sizer;
         TreeBox           TargetImages_TreeBox;
         VerticalSizer  TargetButtons_Sizer;
            PushButton     AddFiles_PushButton;
            PushButton     AddDrizzleFiles_PushButton;
            PushButton     ClearDrizzleFiles_PushButton;
            PushButton     SetReference_PushButton;
            PushButton     SelectAll_PushButton;
            PushButton     InvertSelection_PushButton;
            PushButton     ToggleSelected_PushButton;
            PushButton     RemoveSelected_PushButton;
            PushButton     Clear_PushButton;
            CheckBox       FullPaths_CheckBox;

      SectionBar        FormatHints_SectionBar;
      Control           FormatHints_Control;
      VerticalSizer     FormatHints_Sizer;
         HorizontalSizer   InputHints_Sizer;
            Label             InputHints_Label;
            Edit              InputHints_Edit;
         HorizontalSizer   OutputHints_Sizer;
            Label             OutputHints_Label;
            Edit              OutputHints_Edit;

      SectionBar        Output_SectionBar;
      Control           Output_Control;
      VerticalSizer     Output_Sizer;
         HorizontalSizer   OutputDir_Sizer;
            Label             OutputDir_Label;
            Edit              OutputDir_Edit;
            ToolButton        OutputDir_SelectButton;
         HorizontalSizer   OutputChunks_Sizer;
            Label             Prefix_Label;
            Edit              Prefix_Edit;
            Label             Postfix_Label;
            Edit              Postfix_Edit;
            CheckBox          Overwrite_CheckBox;
         HorizontalSizer   CometPathMap_Sizer;
            CheckBox          CometPathMap_CheckBox;

      SectionBar        Parameter_SectionBar;
      Control           Parameter_Control;
      VerticalSizer     Parameter_Sizer;
         HorizontalSizer   FirstFrame_Sizer;
            NumericEdit       X0_NumericEdit;
            NumericEdit       Y0_NumericEdit;
            PushButton        ShowFirstFrame_PushButton;
         HorizontalSizer   LastFrame_Sizer;
            NumericEdit       X1_NumericEdit;
            NumericEdit       Y1_NumericEdit;
            PushButton        ShowLastFrame_PushButton;
         HorizontalSizer   Velocity_Sizer;
            NumericEdit       XDot_NumericEdit;
            NumericEdit       YDot_NumericEdit;
            PushButton        ResetPositions_PushButton;
         HorizontalSizer   PSFFits_Sizer;
            CheckBox          PSFFits_CheckBox;

      SectionBar        Subtract_SectionBar;
      Control           Subtract_Control;
      VerticalSizer     Subtract_Sizer;
         HorizontalSizer   SubtractFile_Sizer;
            Label             SubtractFile_Label;
            Edit              SubtractFile_Edit;
            ToolButton        SubtractFile_SelectButton;
            ToolButton        SubtractFile_ShowButton;
            ToolButton        SubtractFile_ClearButton;
         HorizontalSizer   SubtractImgOption_Sizer;
            GroupBox          SubtractImgType_GroupBox;
            VerticalSizer     SubtractImgType_Sizer;
               RadioButton       SubtractStars_RadioButton;
               RadioButton       SubtractComet_RadioButton;
            GroupBox          DrzSave_GroupBox;
            VerticalSizer        DrzSave_Sizer;
               CheckBox          DrzSaveSA_CheckBox;
               CheckBox          DrzSaveCA_CheckBox;
         HorizontalSizer   SubtractChekers_Sizer;
            CheckBox          LinearFit_CheckBox;
            CheckBox          Normalize_CheckBox;
         NumericControl    RejectLow_NumericControl;
         NumericControl    RejectHigh_NumericControl;

      SectionBar        Interpolation_SectionBar;
      Control           Interpolation_Control;
      VerticalSizer     Interpolation_Sizer;
         HorizontalSizer   PixelInterpolation_Sizer;
            Label             PixelInterpolation_Label;
            ComboBox          PixelInterpolation_ComboBox;
            NumericControl    ClampingThreshold_NumericControl;
   };

   GUIData* GUI = nullptr;

   bool           m_drizzle;  // true iff .xdrz file present for every image
   mutable DPoint m_imagePos; // centroid position on the image (dynamic paint)

   // Main routines
   void SelectDir();
   void SelectSubtractFile();
   void ShowSubtractFile();
   void SetFramePosition( size_type index, const DPoint& );
   void ResetFramePositions();
   void RedrawAll();

   // CometAlignmentInterface Updates
   void UpdateControls();
   void UpdateTargetImageItem( size_type index );
   void UpdateTargetImagesList();
   void UpdateImageSelectionButtons();
   void UpdateSubtractSection();
   void SetReferenceFrame( size_type index );

   // Event handlers
   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_NodeSelectionUpdated( TreeBox& sender );
   void e_TargetFrames_ButtonClick( Button& sender, bool checked );
   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_EditCompleted( Edit& sender );
   void e_ButtonClick( Button& sender, bool checked );
   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   void AddFiles( const StringList& );
   String DrizzleTargetName( const String& filePath );

   friend struct GUIData;
   friend class CometAlignmentInstance;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern CometAlignmentInterface* TheCometAlignmentInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __CometAlignmentInterface_h

// ----------------------------------------------------------------------------
// EOF CometAlignmentInterface.h - Released 2023-08-28T15:23:41Z
