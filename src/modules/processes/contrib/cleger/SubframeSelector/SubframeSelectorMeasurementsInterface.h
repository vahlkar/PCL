//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.23
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.8.0
// ----------------------------------------------------------------------------
// SubframeSelectorMeasurementsInterface.h - Released 2022-03-12T18:59:53Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2021 Cameron Leger
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

#ifndef e_SubframeSelectorMeasurementsInterface_h
#define e_SubframeSelectorMeasurementsInterface_h

#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ToolButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/TreeBox.h>

#include "GraphWebView.h"
#include "SubframeSelectorInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class MeasurementNode;

class SubframeSelectorMeasurementsInterface : public ProcessInterface
{
public:

   SubframeSelectorMeasurementsInterface( SubframeSelectorInstance& );
   virtual ~SubframeSelectorMeasurementsInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   using ProcessInterface::Launch;

   void Cleanup();

private:

   SubframeSelectorInstance& m_instance;

   struct GUIData
   {
      GUIData( SubframeSelectorMeasurementsInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        MeasurementTable_SectionBar;
      Control           MeasurementTable_Control;
      VerticalSizer     MeasurementTable_Sizer;
         HorizontalSizer   MeasurementsTable_Top_Sizer;
            VerticalSizer     MeasurementsTable_TopLeft_Sizer;
               HorizontalSizer   MeasurementsTable_TopLeft1_Sizer;
                  ComboBox          MeasurementsTable_SortingProperty_ComboBox;
                  ComboBox          MeasurementsTable_SortingMode_ComboBox;
               HorizontalSizer   MeasurementsTable_TopLeft2_Sizer;
                  Label             MeasurementsTable_Quantities_Label;
            VerticalSizer     MeasurementsTable_TopRight_Sizer;
               HorizontalSizer   MeasurementsTable_TopRight1_Sizer;
                  ToolButton        MeasurementsTable_ToggleApproved_ToolButton;
                  ToolButton        MeasurementsTable_ToggleLocked_ToolButton;
                  ToolButton        MeasurementsTable_Invert_ToolButton;
               HorizontalSizer   MeasurementsTable_TopRight2_Sizer;
                  ToolButton        MeasurementsTable_Remove_ToolButton;
                  ToolButton        MeasurementsTable_Reset_ToolButton;
                  ToolButton        MeasurementsTable_ExportCSV_ToolButton;
         TreeBox           MeasurementTable_TreeBox;

      SectionBar        MeasurementGraph_SectionBar;
      Control           MeasurementGraph_Control;
      VerticalSizer     MeasurementGraph_Sizer;
         HorizontalSizer   MeasurementGraph_Top_Sizer;
            ComboBox          MeasurementGraph_Property_ComboBox;
            ComboBox          MeasurementGraph_AuxProperty_ComboBox;
            ToolButton        MeasurementGraph_ExportPDF_ToolButton;
         HorizontalSizer   MeasurementGraph_Bottom_Sizer;
            GraphWebView      MeasurementGraph_WebView;
            GraphWebView      MeasurementDistributionGraph_WebView;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateMeasurementQuantity();
   void UpdateMeasurementNode( MeasurementNode* );
   void UpdateMeasurementTable();
   void UpdateMeasurementGraph();

   MeasurementNode* MeasurementNodeByItem( const MeasureItem* ) const;
   MeasureItem* MeasureItemByGraphIndex( int index ) const;

   void ExportCSV() const;
   void ExportPDF() const;

   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_ButtonClick( Button& sender, bool checked );
   void e_GraphApprove( GraphWebView &sender, int measureIndex );
   void e_GraphUnlock( GraphWebView &sender, int measureIndex );
   void e_ItemSelected( ComboBox& sender, int itemIndex );

   friend struct GUIData;
   friend class SubframeSelectorInstance;
   friend class SubframeSelectorInterface;
   friend class SubframeSelectorExpressionsInterface;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SubframeSelectorMeasurementsInterface* TheSubframeSelectorMeasurementsInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // e_SubframeSelectorMeasurementsInterface_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasurementsInterface.h - Released 2022-03-12T18:59:53Z
