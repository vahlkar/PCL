//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// EphemerisGeneratorInterface.h - Released 2024-12-11T17:43:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
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

#ifndef __EphemerisGeneratorInterface_h
#define __EphemerisGeneratorInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/TextBox.h>
#include <pcl/ToolButton.h>

#include "EphemerisGeneratorInstance.h"
#include "TextDatabase.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class EphemerisGeneratorInterface : public ProcessInterface
{
public:

   EphemerisGeneratorInterface();
   virtual ~EphemerisGeneratorInterface();

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

   EphemerisGeneratorInstance m_instance;

   struct GUIData
   {
      GUIData( EphemerisGeneratorInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        InitialConditions_SectionBar;
      Control           InitialConditions_Control;
      VerticalSizer     InitialConditions_Sizer;
         HorizontalSizer   WorkingMode_Sizer;
            Label             WorkingMode_Label;
            ComboBox          WorkingMode_ComboBox;
         GroupBox          StateVectors_GroupBox;
         VerticalSizer     StateVectors_Sizer;
            HorizontalSizer   X_Sizer;
               NumericEdit       X_NumericEdit;
               NumericEdit       XDot_NumericEdit;
            HorizontalSizer   Y_Sizer;
               NumericEdit       Y_NumericEdit;
               NumericEdit       YDot_NumericEdit;
            HorizontalSizer   Z_Sizer;
               NumericEdit       Z_NumericEdit;
               NumericEdit       ZDot_NumericEdit;
               ToolButton        ToElements_ToolButton;
         GroupBox          OrbitalElements_GroupBox;
         VerticalSizer     OrbitalElements_Sizer;
            HorizontalSizer   AM_Sizer;
               NumericEdit       A_NumericEdit;
               NumericEdit       M_NumericEdit;
            HorizontalSizer   QT_Sizer;
               NumericEdit       Q_NumericEdit;
               NumericEdit       T_NumericEdit;
            HorizontalSizer   EO_Sizer;
               NumericEdit       E_NumericEdit;
               NumericEdit       O_NumericEdit;
            HorizontalSizer   IW_Sizer;
               NumericEdit       I_NumericEdit;
               NumericEdit       W_NumericEdit;
               ToolButton        ToVectors_ToolButton;
         Control           ObjectParameters_Control;
         VerticalSizer     ObjectParameters_Sizer;
            HorizontalSizer   Epoch_Sizer;
               Label             Epoch_Label;
               Edit              Epoch_Edit;
            HorizontalSizer   ObjectId_Sizer;
               Label             ObjectId_Label;
               Edit              ObjectId_Edit;
            HorizontalSizer   ObjectName_Sizer;
               Label             ObjectName_Label;
               Edit              ObjectName_Edit;
         GroupBox          Database_GroupBox;
         VerticalSizer     Database_Sizer;
            HorizontalSizer   DatabasePath_Sizer;
               Label             DatabasePath_Label;
               Edit              DatabasePath_Edit;
               ToolButton        DatabasePath_ToolButton;
            HorizontalSizer   DatabaseFormat_Sizer;
               Label             DatabaseFormat_Label;
               ComboBox          DatabaseFormat_ComboBox;
            HorizontalSizer   DatabaseObjects_Sizer;
               Label             DatabaseObjects_Label;
               TextBox           DatabaseObjects_TextBox;
            HorizontalSizer   DatabaseOptions_Sizer;
               CheckBox          UseRegularExpressions_CheckBox;
               CheckBox          ExcludeCoreAsteroids_CheckBox;
            HorizontalSizer   DatabaseButtons_Sizer;
               PushButton        GetFirst_Button;
               PushButton        CheckObjects_Button;

      SectionBar        PhysicalParameters_SectionBar;
      Control           PhysicalParameters_Control;
      VerticalSizer     PhysicalParameters_Sizer;
            HorizontalSizer   PhysicalParametersRow1_Sizer;
               CheckBox          H_CheckBox;
               NumericEdit       H_NumericEdit;
               CheckBox          G_CheckBox;
               NumericEdit       G_NumericEdit;
            HorizontalSizer   PhysicalParametersRow2_Sizer;
               CheckBox          M1_CheckBox;
               NumericEdit       M1_NumericEdit;
               CheckBox          K1_CheckBox;
               NumericEdit       K1_NumericEdit;
            HorizontalSizer   PhysicalParametersRow3_Sizer;
               CheckBox          M2_CheckBox;
               NumericEdit       M2_NumericEdit;
               CheckBox          K2_CheckBox;
               NumericEdit       K2_NumericEdit;
               CheckBox          PC_CheckBox;
               NumericEdit       PC_NumericEdit;
            HorizontalSizer   PhysicalParametersRow4_Sizer;
               CheckBox          B_V_CheckBox;
               NumericEdit       B_V_NumericEdit;
               CheckBox          U_B_CheckBox;
               NumericEdit       U_B_NumericEdit;
               CheckBox          I_R_CheckBox;
               NumericEdit       I_R_NumericEdit;
            HorizontalSizer   PhysicalParametersRow5_Sizer;
               CheckBox          D_CheckBox;
               NumericEdit       D_NumericEdit;

      SectionBar        NonGravitationalParameters_SectionBar;
      Control           NonGravitationalParameters_Control;
      VerticalSizer     NonGravitationalParameters_Sizer;
            HorizontalSizer   NonGravitationalParametersRow1_Sizer;
               CheckBox          A1_CheckBox;
               NumericEdit       A1_NumericEdit;
               CheckBox          A2_CheckBox;
               NumericEdit       A2_NumericEdit;
               CheckBox          A3_CheckBox;
               NumericEdit       A3_NumericEdit;
            HorizontalSizer   NonGravitationalParametersRow2_Sizer;
               CheckBox          DT_CheckBox;
               NumericEdit       DT_NumericEdit;

      SectionBar        NumericalIntegration_SectionBar;
      Control           NumericalIntegration_Control;
      VerticalSizer     NumericalIntegration_Sizer;
         HorizontalSizer   StartTime_Sizer;
            Label             StartTime_Label;
            Edit              StartTime_Edit;
         HorizontalSizer   EndTime_Sizer;
            Label             EndTime_Label;
            Edit              EndTime_Edit;
         HorizontalSizer   FundamentalEphemerides_Sizer;
            Label             FundamentalEphemerides_Label;
            Edit              FundamentalEphemerides_Edit;
            ToolButton        FundamentalEphemerides_ToolButton;
         HorizontalSizer   AsteroidPerturbers_Sizer;
            CheckBox          AsteroidPerturbers_CheckBox;
         HorizontalSizer   AsteroidEphemerides_Sizer;
            Label             AsteroidEphemerides_Label;
            Edit              AsteroidEphemerides_Edit;
            ToolButton        AsteroidEphemerides_ToolButton;
         HorizontalSizer   KBOPerturbers_Sizer;
            CheckBox          KBOPerturbers_CheckBox;
         HorizontalSizer   KBOEphemerides_Sizer;
            Label             KBOEphemerides_Label;
            Edit              KBOEphemerides_Edit;
            ToolButton        KBOEphemerides_ToolButton;
         HorizontalSizer   SeparateEarthMoon_Sizer;
            CheckBox          SeparateEarthMoon_CheckBox;
         HorizontalSizer   RelativisticPerturbations_Sizer;
            CheckBox          RelativisticPerturbations_CheckBox;
         HorizontalSizer   FigureEffects_Sizer;
            CheckBox          FigureEffects_CheckBox;
         HorizontalSizer   NonGravitationalPerturbations_Sizer;
            CheckBox          NonGravitationalPerturbations_CheckBox;

      SectionBar        Output_SectionBar;
      Control           Output_Control;
      VerticalSizer     Output_Sizer;
         HorizontalSizer   OutputXEPHFile_Sizer;
            CheckBox          OutputXEPHFile_CheckBox;
         HorizontalSizer   OutputXEPHFilePath_Sizer;
            Label             OutputXEPHFilePath_Label;
            Edit              OutputXEPHFilePath_Edit;
            ToolButton        OutputXEPHFilePath_ToolButton;
         HorizontalSizer   OutputVelocityExpansions_Sizer;
            CheckBox          OutputVelocityExpansions_CheckBox;
         HorizontalSizer   MaxExpansionLength_Sizer;
            Label             MaxExpansionLength_Label;
            SpinBox           MaxExpansionLength_SpinBox;
         NumericEdit       MaxTruncationError_NumericEdit;
         HorizontalSizer   OverwriteExistingFiles_Sizer;
            CheckBox          OverwriteExistingFiles_CheckBox;
   };

   GUIData* GUI = nullptr;

   double m_GMS = 0; // solar gravitational mass parameter

   void UpdateControls();
   void UpdateElementDependencies( int idx );

   void e_TimeEditCompleted( Edit& sender );
   void e_IdEditCompleted( Edit& sender );
   void e_FilePathEditCompleted( Edit& sender );
   void e_FilePathEditGetFocus( Control& sender );
   void e_FilePathEditLoseFocus( Control& sender );
   void e_EditValueUpdated( NumericEdit& sender, double value );
   void e_SpinValueUpdated( SpinBox& sender, int value );
   void e_Click( Button& sender, bool checked );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_TextUpdated( TextBox& sender, const String& );
   void e_GroupBoxCheck( GroupBox& sender, bool checked );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   Array<TextDatabase::ObjectData> SearchDatabase( bool& gotElements, int maxCount = 0 ) const;

   static String TimeParameterValueToString( double );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern EphemerisGeneratorInterface* TheEphemerisGeneratorInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __EphemerisGeneratorInterface_h

// ----------------------------------------------------------------------------
// EOF EphemerisGeneratorInterface.h - Released 2024-12-11T17:43:17Z
