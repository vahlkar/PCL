//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard StarNet Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// StarNetInterface.h - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarNet PixInsight module.
//
// Copyright (c) 2018-2020 Nikita Misiura
//
// This software is available under Attribution-NonCommercial-ShareAlike 4.0
// International Creative Commons license (CC BY-NC-SA 4.0):
// 
// https://creativecommons.org/licenses/by-nc-sa/4.0/
// 
// In short: You are free to use and redistribute the code in any medium or
// format, but only under the same license terms. You can transform and build
// your projects upon it. You can NOT use the code for commercial purposes. You
// must give appropriate credit for usage of the code.
// 
// This product is based on software from the PixInsight project, developed by
// Pleiades Astrophoto and its contributors:
// 
// https://pixinsight.com/
// ----------------------------------------------------------------------------

#ifndef __StarNetInterface_h
#define __StarNetInterface_h

#include <pcl/BitmapBox.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>

#include "StarNetInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class StarNetInterface : public ProcessInterface
{
public:

   StarNetInterface();
   virtual ~StarNetInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void EditPreferences() override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   StarNetInstance m_instance;

   struct GUIData
   {
      GUIData( StarNetInterface& );

      VerticalSizer     Global_Sizer;
         HorizontalSizer   StrideParameter_Sizer;
            Label             StrideParameter_Label;
            ComboBox          StrideParameter_ComboBox;
         HorizontalSizer   MaskParameter_Sizer;
            CheckBox          MaskParameter_CheckBox;
         HorizontalSizer   RGBWeightsFile_Sizer;
            Label             RGBWeightsFile_Label;
            Edit              RGBWeightsFile_Edit;
            BitmapBox         RGBWeightsFile_BitmapBox;
         HorizontalSizer   GrayscaleWeightsFile_Sizer;
            Label             GrayscaleWeightsFile_Label;
            Edit              GrayscaleWeightsFile_Edit;
            BitmapBox         GrayscaleWeightsFile_BitmapBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Click( Button& sender, bool checked );

   friend struct GUIData;
   friend class StarNetPreferencesDialog;
};

PCL_BEGIN_LOCAL
extern StarNetInterface* TheStarNetInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarNetInterface_h

// ----------------------------------------------------------------------------
// EOF StarNetInterface.h - Released 2021-04-09T19:41:49Z
