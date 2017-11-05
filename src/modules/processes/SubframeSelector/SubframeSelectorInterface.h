//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorInterface.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
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

#ifndef __SubframeSelectorInterface_h
#define __SubframeSelectorInterface_h

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
#include <pcl/Label.h>
#include <pcl/GroupBox.h>
#include <pcl/TextBox.h>
#include <pcl/BitmapBox.h>
#include "GraphWebView.h"

#include "SubframeSelectorInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SubframeSelectorInterface : public ProcessInterface
{
public:

   SubframeSelectorInterface();
   virtual ~SubframeSelectorInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   InterfaceFeatures Features() const;

   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   void ClearMeasurements();
   void AddMeasurements( Array<MeasureItem>::const_iterator begin, Array<MeasureItem>::const_iterator end );

   private:

   SubframeSelectorInstance instance;

   struct GUIData
   {
      GUIData( SubframeSelectorInterface& );

      HorizontalSizer   Global_Sizer;
      VerticalSizer     Left_Sizer;
      VerticalSizer     Right_Sizer;

      HorizontalSizer   Routine_Sizer;
         Label             Routine_Label;
         ComboBox          Routine_Control;

      SectionBar        SubframeImages_SectionBar;
      Control           SubframeImages_Control;
      HorizontalSizer   SubframeImages_Sizer;
         TreeBox           SubframeImages_TreeBox;
         VerticalSizer     SubframeButtons_Sizer;
            PushButton        SubframeImages_AddFiles_PushButton;
            PushButton        SubframeImages_Invert_PushButton;
            PushButton        SubframeImages_Toggle_PushButton;
            PushButton        SubframeImages_Remove_PushButton;
            PushButton        SubframeImages_Clear_PushButton;

      SectionBar        SystemParameters_SectionBar;
      Control           SystemParameters_Control;
      VerticalSizer     SystemParameters_Sizer;
         HorizontalSizer   SystemParameters_SubframeScale_Sizer;
            NumericControl    SystemParameters_SubframeScale_Control;
            Label             SystemParameters_SubframeScale_Unit;
         HorizontalSizer   SystemParameters_CameraGain_Sizer;
            NumericControl    SystemParameters_CameraGain_Control;
            Label             SystemParameters_CameraGain_Unit;
         HorizontalSizer   SystemParameters_CameraResolution_Sizer;
            Label             SystemParameters_CameraResolution_Label;
            ComboBox          SystemParameters_CameraResolution_Control;
         HorizontalSizer   SystemParameters_SiteLocalMidnight_Sizer;
            Label             SystemParameters_SiteLocalMidnight_Label;
            SpinBox           SystemParameters_SiteLocalMidnight_Control;
            Label             SystemParameters_SiteLocalMidnight_Unit;
         HorizontalSizer   SystemParameters_ScaleUnit_Sizer;
            Label             SystemParameters_ScaleUnit_Label;
            ComboBox          SystemParameters_ScaleUnit_Control;
         HorizontalSizer   SystemParameters_DataUnit_Sizer;
            Label             SystemParameters_DataUnit_Label;
            ComboBox          SystemParameters_DataUnit_Control;

      SectionBar        StarDetectorParameters_SectionBar;
      Control           StarDetectorParameters_Control;
      VerticalSizer     StarDetectorParameters_Sizer;
         HorizontalSizer   StarDetectorParameters_StructureLayers_Sizer;
            Label             StarDetectorParameters_StructureLayers_Label;
            SpinBox           StarDetectorParameters_StructureLayers_Control;
         HorizontalSizer   StarDetectorParameters_NoiseLayers_Sizer;
            Label             StarDetectorParameters_NoiseLayers_Label;
            SpinBox           StarDetectorParameters_NoiseLayers_Control;
         HorizontalSizer   StarDetectorParameters_HotPixelFilterRadius_Sizer;
            Label             StarDetectorParameters_HotPixelFilterRadius_Label;
            SpinBox           StarDetectorParameters_HotPixelFilterRadius_Control;
         HorizontalSizer   StarDetectorParameters_ApplyHotPixelFilter_Sizer;
            Label             StarDetectorParameters_ApplyHotPixelFilter_Label;
            CheckBox          StarDetectorParameters_ApplyHotPixelFilter_Control;
         HorizontalSizer   StarDetectorParameters_NoiseReductionFilterRadius_Sizer;
            Label             StarDetectorParameters_NoiseReductionFilterRadius_Label;
            SpinBox           StarDetectorParameters_NoiseReductionFilterRadius_Control;
         NumericControl    StarDetectorParameters_Sensitivity_Control;
         NumericControl    StarDetectorParameters_PeakResponse_Control;
         NumericControl    StarDetectorParameters_MaxDistortion_Control;
         NumericControl    StarDetectorParameters_UpperLimit_Control;
         HorizontalSizer   StarDetectorParameters_BackgroundExpansion_Sizer;
            Label             StarDetectorParameters_BackgroundExpansion_Label;
            SpinBox           StarDetectorParameters_BackgroundExpansion_Control;
         NumericControl    StarDetectorParameters_XYStretch_Control;
         HorizontalSizer   StarDetectorParameters_PSFFit_Sizer;
            Label             StarDetectorParameters_PSFFit_Label;
            ComboBox          StarDetectorParameters_PSFFit_Control;
            Label             StarDetectorParameters_PSFFitCircular_Label;
            CheckBox          StarDetectorParameters_PSFFitCircular_Control;
         HorizontalSizer   StarDetectorParameters_Pedestal_Sizer;
            Label             StarDetectorParameters_Pedestal_Label;
            SpinBox           StarDetectorParameters_Pedestal_Control;
            Label             StarDetectorParameters_Pedestal_Unit;
         HorizontalSizer   StarDetectorParameters_ROIRow1_Sizer;
            Label             StarDetectorParameters_ROIX0_Label;
            SpinBox           StarDetectorParameters_ROIX0_Control;
            Label             StarDetectorParameters_ROIY0_Label;
            SpinBox           StarDetectorParameters_ROIY0_Control;
         HorizontalSizer   StarDetectorParameters_ROIRow2_Sizer;
            Label             StarDetectorParameters_ROIWidth_Label;
            SpinBox           StarDetectorParameters_ROIWidth_Control;
            Label             StarDetectorParameters_ROIHeight_Label;
            SpinBox           StarDetectorParameters_ROIHeight_Control;
            PushButton        StarDetectorParameters_ROISelectPreview_Button;

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
         HorizontalSizer   OutputKeyword_Sizer;
            Label             OutputKeyword_Label;
            Edit              OutputKeyword_Edit;
         HorizontalSizer   OutputMisc_Sizer;
            CheckBox          OverwriteExistingFiles_CheckBox;
            Label             OnError_Label;
            ComboBox          OnError_ComboBox;

      SectionBar        ExpressionParameters_SectionBar;
      Control           ExpressionParameters_Control;
      VerticalSizer     ExpressionParameters_Sizer;
         HorizontalSizer   ExpressionParameters_Approval_Sizer;
            BitmapBox         ExpressionParameters_Approval_Status;
            Label             ExpressionParameters_Approval_Label;
            Edit              ExpressionParameters_Approval_Control;
         HorizontalSizer   ExpressionParameters_Weighting_Sizer;
            BitmapBox         ExpressionParameters_Weighting_Status;
            Label             ExpressionParameters_Weighting_Label;
            Edit              ExpressionParameters_Weighting_Control;

      SectionBar        MeasurementTable_SectionBar;
      Control           MeasurementTable_Control;
      VerticalSizer     MeasurementTable_Sizer;
         HorizontalSizer   MeasurementsTable_Top_Sizer;
            ComboBox          MeasurementsTable_SortingProperty_Control;
            ComboBox          MeasurementsTable_SortingMode_Control;
            PushButton        MeasurementsTable_ToggleApproved_PushButton;
            PushButton        MeasurementsTable_ToggleLocked_PushButton;
            PushButton        MeasurementsTable_Invert_PushButton;
            PushButton        MeasurementsTable_Clear_PushButton;
            PushButton        MeasurementsTable_CSV_PushButton;
         TreeBox           MeasurementTable_TreeBox;

      SectionBar        MeasurementGraph_SectionBar;
      Control           MeasurementGraph_Control;
      VerticalSizer     MeasurementGraph_Sizer;
         HorizontalSizer   MeasurementGraph_Top_Sizer;
            ComboBox          MeasurementGraph_GraphProperty_Control;
            PushButton        MeasurementGraph_Save_PushButton;
         GraphWebView      MeasurementGraph_Graph;
   };

   GUIData* GUI = nullptr;

   MeasureItem* GetMeasurementItem( size_type );
   TreeBox::Node* GetMeasurementNode( MeasureItem* );

   void UpdateControls();
   void UpdateSubframeImageItem( size_type );
   void UpdateSubframeImagesList();
   void UpdateSubframeImageSelectionButtons();
   void UpdateSystemParameters();
   void UpdateStarDetectorParameters();
   void UpdateOutputFilesControls();
   void UpdateExpressionParameters();
   void UpdateMeasurementImageItem( size_type, MeasureItem* );
   void UpdateMeasurementImagesList();
   void UpdateMeasurementGraph();

   void ApplyApprovalExpression();
   void ApplyWeightingExpression();

   void ExportCSV() const;
   void ExportPDF() const;

   // Event Handlers

   void __ToggleSection( SectionBar& sender, Control& section, bool start );

   void __SubframeImages_CurrentNodeUpdated( TreeBox &sender, TreeBox::Node &current, TreeBox::Node &oldCurrent );
   void __SubframeImages_NodeActivated( TreeBox &sender, TreeBox::Node &node, int col );
   void __SubframeImages_NodeSelectionUpdated( TreeBox &sender );
   void __SubframeImages_Click( Button &sender, bool checked );

   void __MeasurementImages_CurrentNodeUpdated( TreeBox &sender, TreeBox::Node &current, TreeBox::Node &oldCurrent );
   void __MeasurementImages_NodeActivated( TreeBox &sender, TreeBox::Node &node, int col );
   void __MeasurementImages_Click( Button &sender, bool checked );

   void __MeasurementGraph_Approve( GraphWebView &sender, int& index );
   void __MeasurementGraph_Unlock( GraphWebView &sender, int& index );

   void __StarDetector_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers,
                                 bool& wantsView );
   void __StarDetector_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   void __RealValueUpdated( NumericEdit& sender, double value );
   void __IntegerValueUpdated( SpinBox& sender, int value );
   void __ComboSelected( ComboBox& sender, int itemIndex );
   void __CheckboxUpdated( Button& sender, Button::check_state state );
   void __ButtonClick( Button& sender, bool checked );
   void __TextUpdated( Edit& sender, const String& text );
   void __TextUpdateCompleted( Edit& sender );
   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend struct GUIData;
   friend class SubframeSelectorInstance;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SubframeSelectorInterface* TheSubframeSelectorInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SubframeSelectorInterface_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInterface.h - Released 2017-08-01T14:26:58Z
