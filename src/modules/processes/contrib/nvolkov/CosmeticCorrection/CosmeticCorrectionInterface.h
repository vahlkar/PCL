//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 1.2.5
// ----------------------------------------------------------------------------
// CosmeticCorrectionInterface.h - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2020 Nikolay Volkov
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L.
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

#ifndef __CosmeticCorrectionInterface_h
#define __CosmeticCorrectionInterface_h

#include <pcl/CheckBox.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

#include "CosmeticCorrectionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CosmeticCorrectionInterface : public ProcessInterface
{
public:

   CosmeticCorrectionInterface();
   virtual ~CosmeticCorrectionInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void RealTimePreviewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsReadoutNotifications() const override;
   void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A ) override;
   bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, const Rect& rect, int zoomLevel ) const override;
   bool GenerateRealTimePreview( UInt16Image&, const View&, const Rect& rect, int zoomLevel, String& info ) const override;
   bool WantsRealTimePreviewNotifications() const override;
   void RealTimePreviewOwnerChanged( ProcessInterface& ) override;
   void SaveSettings() const override;

private:

   CosmeticCorrectionInstance instance;

   struct GUIData
   {
      GUIData( CosmeticCorrectionInterface& );

      VerticalSizer Global_Sizer;

      SectionBar TargetImages_SectionBar;
      Control TargetImages_Control;
      HorizontalSizer TargetImages_Sizer;
      TreeBox TargetImages_TreeBox;
      VerticalSizer TargetButtons_Sizer;
      PushButton AddFiles_PushButton;
      PushButton SelectAll_PushButton;
      PushButton InvertSelection_PushButton;
      PushButton ToggleSelected_PushButton;
      PushButton RemoveSelected_PushButton;
      PushButton Clear_PushButton;
      CheckBox FullPaths_CheckBox;

      SectionBar Output_SectionBar;
      Control Output_Control;
      VerticalSizer Output_Sizer;
      HorizontalSizer OutputDir_Sizer;
      Label OutputDir_Label;
      Edit OutputDir_Edit;
      ToolButton OutputDir_SelectButton;
      ToolButton OutputDir_ClearButton;
      HorizontalSizer OutputChunks_Sizer;
      Label Prefix_Label;
      Edit Prefix_Edit;
      Label Postfix_Label;
      Edit Postfix_Edit;
      HorizontalSizer CFA_Sizer;
      CheckBox CFA_CheckBox;
      HorizontalSizer Overwrite_Sizer;
      CheckBox Overwrite_CheckBox;
      NumericControl Amount_NumericControl;

      // detect via MasterDark section
      SectionBar UseMasterDark_SectionBar;
      Control UseMasterDark_Control;
      VerticalSizer UseMasterDark_Sizer;

      GroupBox MasterDark_GroupBox;
      VerticalSizer MasterDark_Sizer;
      HorizontalSizer MasterDarkPath_Sizer;
      Edit MasterDarkPath_Edit;
      ToolButton MasterDarkPath_SelectButton;
      ToolButton MasterDarkPath_ClearButton;

      GroupBox Hot_GroupBox;
      VerticalSizer Hot_Sizer;
      CheckBox HotDark_CheckBox;
      NumericControl HotDarkLevel_NumericControl;
      NumericControl HotDarkSigma_NumericControl;
      NumericControl HotDarkQty_NumericControl;
      HorizontalSizer HotRealQty_Sizer;
      Label HotRealQty_Label;
      Label HotRealQty_Value;

      GroupBox Cold_GroupBox;
      VerticalSizer Cold_Sizer;
      CheckBox ColdDark_CheckBox;
      NumericControl ColdDarkLevel_NumericControl;
      NumericControl ColdDarkSigma_NumericControl;
      NumericControl ColdDarkQty_NumericControl;
      HorizontalSizer ColdRealQty_Sizer;
      Label ColdRealQty_Label;
      Label ColdRealQty_Value;

      // auto detect section
      SectionBar UseAutoDetect_SectionBar;
      Control UseAutoDetect_Control;
      VerticalSizer UseAutoDetect_Sizer;
      CheckBox DetectHot_CheckBox;
      NumericControl DetectHot_NumericControl;
      CheckBox DetectCold_CheckBox;
      NumericControl DetectCold_NumericControl;

      // via defective list section
      SectionBar UseDefectList_SectionBar;
      Control UseDefectList_Control;
      VerticalSizer UseDefectList_Sizer;

      HorizontalSizer DefectListTreeBox_Sizer;
      TreeBox DefectList_TreeBox;
      VerticalSizer DefectListButtons_Sizer;
      PushButton LoadList_PushButton;
      PushButton SaveList_PushButton;
      PushButton SelectAllDefect_PushButton;
      PushButton InvertSelectionDefect_PushButton;
      PushButton ToggleSelectedDefect_PushButton;
      PushButton RemoveSelectedDefect_PushButton;
      PushButton ClearDefect_PushButton;

      VerticalSizer DefineDefect_Sizer;
      HorizontalSizer DefineDefect_Sizer1;
      RadioButton Row_RadioButton;
      RadioButton Col_RadioButton;
      NumericEdit DefectPosition;
      PushButton AddDefect_PushButton;

      HorizontalSizer DefineDefect_Sizer2;
      CheckBox SelectRange_CheckBox;
      NumericEdit DefectStart;
      NumericEdit DefectEnd;

      // RTP section
      SectionBar RTP_SectionBar;
      Control RTP_Control;
      VerticalSizer RTP_Sizer;
      HorizontalSizer RTPLine1_Sizer;
      Label TRPTitleHot_Label;
      Label TRPTitleCold_Label;
      HorizontalSizer RTPLine2_Sizer;
      NumericEdit RTPDarkQtyHot_Edit;
      NumericEdit RTPDarkQtyCold_Edit;
      HorizontalSizer RTPLine3_Sizer;
      NumericEdit RTPAutoQtyHot_Edit;
      NumericEdit RTPAutoQtyCold_Edit;
      CheckBox RTPShowMap_CheckBox;
      PushButton RTPSnapshot_PushButton;
   };

   GUIData* GUI = nullptr;

   // Workbench
   DarkImg* m_md = nullptr; // masterDark image
   int m_channels = 1;      // qty channels in MasterDark image
   Array<Histogram> m_H;    // histograms, native PI's resolution
   double m_Mean = 0.5;     // average mean of all chanels
   double m_StdDev = 0.01;  // average StdDev of all chanels
   int m_MinSlider = 0;     // Slider range <= minimum
   int m_MaxSlider = 65535; // Slider range >= maximum

   // Main routines
   void SelectDir();
   void LoadMasterDark( const String& filePath );
   void SelectMasterFrameDialog();

   void HotUpdateGUI( const size_t );
   void HotLevelUpdated( const float, const bool ); // calculate & set Qty from Level
   void HotQtyUpdated( const size_t );              // calculate & set Level from Qty
   void HotSigmaUpdated( const float );             // calculate Level and QTY

   void ColdUpdateGUI( const size_t );
   void ColdLevelUpdated( const float, const bool ); // calculate & set Qty from Level
   void ColdQtyUpdated( const size_t );              // calculate & set Level from Qty
   void ColdSigmaUpdated( const float );             // calculate Level and QTY

   void AddDefect( const bool isRow, const int position, const bool setRange, const int start, const int end );

   void UpdateControls();
   void UpdateTargetImageItem( size_type );
   void UpdateTargetImagesList();
   void UpdateImageSelectionButtons();
   void ClearMasterDark();
   void UpdateMasterDarkControls();
   void UpdateAutoDetectControls();
   void UpdateDefectListSelectionButtons();
   void UpdateDefectListItem( size_type );
   void UpdateDefectListControls();

   void RTPApplyBadMap( UInt16Image& target, const UInt8Image& map, const UInt16Image& value ) const;
   void RTPGetStatistics( Array<double>& avgDev, const View& view ) const;
   void RTPGetImageRectangle( UInt16Image& wrk, const View& view ) const;
   void RTPGetDarkRectangle( UInt16Image& mdRect ) const;

   void RTPGetAvr( UInt16Image& avr, const UInt16Image& wrk ) const;
   void RTPGetMed( UInt16Image& med, const UInt16Image& wrk ) const;
   void RTPGetBkg( UInt16Image& bkg, const UInt16Image& wrk ) const;

   int RTPGetHotAutoMap( UInt8Image& mapAutoHot, float& hotAutoValue, const UInt16Image& med, const UInt16Image& bkg, const UInt16Image& avr, const UInt16Image& wrk, const Array<double>& avgDev ) const;
   int RTPGetColdAutoMap( UInt8Image& mapAutoCold, float& coldAutoValue, const UInt16Image& med, const UInt16Image& bkg, const UInt16Image& wrk, const Array<double>& avgDev ) const;
   int RTPGetHotDarkMap( UInt8Image& mapDarkHot, float& hotDarkValue, const UInt16Image& mdRect ) const;
   int RTPGetColdDarkMap( UInt8Image& mapDarkCold, float& coldDarkValue, const UInt16Image& mdRect ) const;
   void RTPGetListDefectMap( UInt8Image& mapListDef, const View& view ) const;
   void RTPMakeSnapshot( const UInt16Image& img, const View& view ) const;
   void RTPUpdate();

   void __TargetImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void __NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __NodeUpdated( TreeBox& sender, TreeBox::Node& node, int col );
   void __NodeSelectionUpdated( TreeBox& sender );
   void __TargetImages_Click( Button& sender, bool checked );
   void __ToggleSection( SectionBar& sender, Control& section, bool start );
   void __MouseDoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void __EditCompleted( Edit& sender );
   void __Button_Click( Button& sender, bool checked );
   void __RealValueUpdated( NumericEdit& sender, double value );
   void __CheckSection( SectionBar& sender, bool checked );
   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern CosmeticCorrectionInterface* TheCosmeticCorrectionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __CosmeticCorrectionInterface_h

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionInterface.h - Released 2021-04-09T19:41:49Z
