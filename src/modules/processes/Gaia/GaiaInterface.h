//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Gaia Process Module Version 1.2.6
// ----------------------------------------------------------------------------
// GaiaInterface.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard Gaia PixInsight module.
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

#ifndef __GaiaInterface_h
#define __GaiaInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "GaiaInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class OnlineObjectSearchDialog;

class GaiaInterface : public ProcessInterface
{
public:

   GaiaInterface();
   virtual ~GaiaInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void EditPreferences() override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   GaiaInstance              m_instance;
   OnlineObjectSearchDialog* m_searchDialog = nullptr;
   bool                      m_showCompoundAngles = true;
   bool                      m_raInTimeUnits = true;

   struct GUIData
   {
      GUIData( GaiaInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        SearchParameters_SectionBar;
      Control           SearchParameters_Control;
      VerticalSizer     SearchParameters_Sizer;
         HorizontalSizer   DataRelease_Sizer;
            Label             DataRelease_Label;
            ComboBox          DataRelease_ComboBox;
         HorizontalSizer   SearchParametersBottom_Sizer;
            VerticalSizer     SearchParametersLeft_Sizer;
               HorizontalSizer   CenterRA_Sizer;
                  Label             CenterRA_Label;
                  Edit              CenterRA_Edit;
               HorizontalSizer   CenterDec_Sizer;
                  Label             CenterDec_Label;
                  Edit              CenterDec_Edit;
               HorizontalSizer   Radius_Sizer;
                  Label             Radius_Label;
                  Edit              Radius_Edit;
               HorizontalSizer   ShowCompoundAngles_Sizer;
                  CheckBox          ShowCompoundAngles_CheckBox;
               HorizontalSizer   RAInTimeUnits_Sizer;
                  CheckBox          RAInTimeUnits_CheckBox;
               NumericEdit       MagnitudeLow_NumericEdit;
               NumericEdit       MagnitudeHigh_NumericEdit;
               HorizontalSizer   RequiredFlags_Sizer;
                  Label             RequiredFlags_Label;
                  Edit              RequiredFlags_Edit;
                  ToolButton        RequiredFlagsClear_ToolButton;
                  ToolButton        RequiredFlags_ToolButton;
               HorizontalSizer   InclusionFlags_Sizer;
                  Label             InclusionFlags_Label;
                  Edit              InclusionFlags_Edit;
                  ToolButton        InclusionFlagsClear_ToolButton;
                  ToolButton        InclusionFlags_ToolButton;
               HorizontalSizer   ExclusionFlags_Sizer;
                  Label             ExclusionFlags_Label;
                  Edit              ExclusionFlags_Edit;
                  ToolButton        ExclusionFlagsClear_ToolButton;
                  ToolButton        ExclusionFlags_ToolButton;
               NumericEdit       SourceLimit_NumericEdit;
               ToolButton        SourceLimitReset_ToolButton;
            VerticalSizer     SearchParametersRight_Sizer;
                  PushButton        SearchCoordinates_Button;

      SectionBar        OutputParameters_SectionBar;
      Control           OutputParameters_Control;
      VerticalSizer     OutputParameters_Sizer;
         HorizontalSizer   SortBy_Sizer;
            Label             SortBy_Label;
            ComboBox          SortBy_ComboBox;
         HorizontalSizer   GenerateTextOutput_Sizer;
            CheckBox          GenerateTextOutput_CheckBox;
         HorizontalSizer   GenerateBinaryOutput_Sizer;
            CheckBox          GenerateBinaryOutput_CheckBox;
         HorizontalSizer   TextFormat_Sizer;
            Label             TextFormat_Label;
            ComboBox          TextFormat_ComboBox;
         HorizontalSizer   TextHeaders_Sizer;
            Label             TextHeaders_Label;
            ComboBox          TextHeaders_ComboBox;
         HorizontalSizer   OutputFilePath_Sizer;
            Label             OutputFilePath_Label;
            Edit              OutputFilePath_Edit;
            ToolButton        OutputFilePath_ToolButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   String RAToString( double ) const;
   String DecToString( double ) const;
   String DistToString( double ) const;

   void e_EditCompleted( Edit& sender );
   void e_EditValueUpdated( NumericEdit& sender, double value );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Click( Button& sender, bool checked );

   friend struct GUIData;
   friend class GaiaPreferencesDialog;
   friend class GaiaInstance;
   friend class GaiaProcess;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern GaiaInterface* TheGaiaInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GaiaInterface_h

// ----------------------------------------------------------------------------
// EOF GaiaInterface.h - Released 2024-12-28T16:54:15Z
