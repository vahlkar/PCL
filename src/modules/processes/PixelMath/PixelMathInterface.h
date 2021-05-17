//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// PixelMathInterface.h - Released 2021-05-05T15:38:07Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
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

#ifndef __PixelMathInterface_h
#define __PixelMathInterface_h

#include <pcl/CheckBox.h>
#include <pcl/CodeEditor.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/TabBox.h>

#include "PixelMathInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PixelMathInterface : public ProcessInterface
{
public:

   PixelMathInterface();
   virtual ~PixelMathInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ApplyInstanceGlobal() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

   bool WantsImageNotifications() const override;
   void ImageUpdated( const View& view ) override;
   void ImageRenamed( const View& view ) override;
   void ImageDeleted( const View& view ) override;

private:

   PixelMathInstance m_instance;

   struct GUIData
   {
      GUIData( PixelMathInterface& );

      VerticalSizer   Global_Sizer;

      SectionBar        Expression_SectionBar;
      Control           Expression_Control;
      VerticalSizer     Expression_Sizer;
         TabBox            Editors_TabBox;
            Control           RK_Control;
            HorizontalSizer   RK_Sizer;
               CodeEditor        RK_CodeEditor;
            Control           G_Control;
            HorizontalSizer   G_Sizer;
               CodeEditor        G_CodeEditor;
            Control           B_Control;
            HorizontalSizer   B_Sizer;
               CodeEditor        B_CodeEditor;
            Control           A_Control;
            HorizontalSizer   A_Sizer;
               CodeEditor        A_CodeEditor;
            Control           Symbols_Control;
            HorizontalSizer   Symbols_Sizer;
               CodeEditor        Symbols_CodeEditor;
            Control           Summary_Control;
            HorizontalSizer   Summary_Sizer;
               CodeEditor        Summary_CodeEditor;
         HorizontalSizer   UseSingleExpression_Sizer;
            CheckBox          UseSingleExpression_CheckBox;
         HorizontalSizer   ExpressionEditor_Sizer;
            PushButton        ExpressionEditor_Button;

      SectionBar        Destination_SectionBar;
      Control           Destination_Control;
      HorizontalSizer   Destination_Sizer;
         VerticalSizer     DestinationLeft_Sizer;
            HorizontalSizer   GenerateOutput_Sizer;
               CheckBox          GenerateOutput_CheckBox;
            HorizontalSizer   CacheGeneratedImages_Sizer;
               CheckBox          CacheGeneratedImages_CheckBox;
            HorizontalSizer   SingleThreaded_Sizer;
               CheckBox          SingleThreaded_CheckBox;
            HorizontalSizer   Optimization_Sizer;
               CheckBox          Optimization_CheckBox;
            HorizontalSizer   Use64BitWorkingImage_Sizer;
               CheckBox          Use64BitWorkingImage_CheckBox;
            HorizontalSizer   Rescale_Sizer;
               CheckBox          Rescale_CheckBox;
            NumericEdit       RescaleLower_NumericEdit;
            NumericEdit       RescaleUpper_NumericEdit;
            HorizontalSizer   ReplaceTarget_Sizer;
               RadioButton       ReplaceTarget_RadioButton;
            HorizontalSizer   CreateNewImage_Sizer;
               RadioButton       CreateNewImage_RadioButton;
            HorizontalSizer   ImageId_Sizer;
               Label             ImageId_Label;
               Edit              ImageId_Edit;
               ToolButton        ImageId_Button;
            HorizontalSizer   Width_Sizer;
               Label             Width_Label;
               SpinBox           Width_SpinBox;
            HorizontalSizer   Height_Sizer;
               Label             Height_Label;
               SpinBox           Height_SpinBox;
            HorizontalSizer   ColorSpace_Sizer;
               Label             ColorSpace_Label;
               ComboBox          ColorSpace_ComboBox;
            HorizontalSizer   CreateAlpha_Sizer;
               CheckBox          CreateAlpha_CheckBox;
            HorizontalSizer   SampleFormat_Sizer;
               Label             SampleFormat_Label;
               ComboBox          SampleFormat_ComboBox;
         VerticalSizer     DestinationRight_Sizer;
            PushButton        ClearGeneratedImages_Button;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void ImportSourceCode();
   void ExportSourceCode();
   void MakeSummary();

   void e_PageSelected( TabBox& sender, int pageIndex );
   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_EditCompleted( Edit& sender );
   void e_EditGetFocus( Control& sender );
   void e_ButtonClick( Button& sender, bool checked );
   void e_NumericEditValueUpdated( NumericEdit& sender, double value );
   void e_SpinBoxValueUpdated( SpinBox& sender, int value );
   void e_ComboBoxItemSelected( ComboBox& sender, int itemIndex );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern PixelMathInterface* ThePixelMathInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PixelMathInterface_h

// ----------------------------------------------------------------------------
// EOF PixelMathInterface.h - Released 2021-05-05T15:38:07Z
