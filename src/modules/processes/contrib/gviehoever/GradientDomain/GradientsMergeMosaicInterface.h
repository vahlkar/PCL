//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 0.6.4
// ----------------------------------------------------------------------------
// GradientsMergeMosaicInterface.h - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2020. Licensed under LGPL 2.1
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#ifndef __GradientsMergeMosaicInterface_h
#define __GradientsMergeMosaicInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

#include "GradientsMergeMosaicInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class GradientsMergeMosaicInterface : public ProcessInterface
{
public:

   GradientsMergeMosaicInterface();
   virtual ~GradientsMergeMosaicInterface();

   IsoString Id() const override;
   IsoString Aliases() const override;
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
   void SaveSettings() const override;

private:

   GradientsMergeMosaicInstance instance;

   struct GUIData
   {
      GUIData( GradientsMergeMosaicInterface& );

      VerticalSizer Global_Sizer;

      SectionBar TargetImages_SectionBar;
      Control TargetImages_Control;
      HorizontalSizer TargetImages_Sizer;
      TreeBox TargetImages_TreeBox;
      VerticalSizer TargetButtons_Sizer;
      PushButton AddFiles_PushButton;
      PushButton MoveUp_PushButton;
      PushButton MoveDown_PushButton;
      PushButton SelectAll_PushButton;
      PushButton InvertSelection_PushButton;
      PushButton ToggleSelected_PushButton;
      PushButton RemoveSelected_PushButton;
      PushButton Clear_PushButton;
      CheckBox FullPaths_CheckBox;

      SectionBar Parameters_SectionBar;
      Control Parameters_Control;
      VerticalSizer Parameters_Sizer;
      HorizontalSizer Type_Sizer;
      Label Type_Label;
      ComboBox Type_ComboBox;
      HorizontalSizer ShrinkCount_Sizer;
      Label ShrinkCount_Label;
      SpinBox ShrinkCount_SpinBox;

      HorizontalSizer FeatherRadius_Sizer;
      Label FeatherRadius_Label;
      SpinBox FeatherRadius_SpinBox;

      NumericControl BlackPoint_NumericControl;
      HorizontalSizer GenerateMask_Sizer;
      Label GenerateMask_Label;
      CheckBox GenerateMask_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateTargetImageItem( size_type );
   void UpdateTargetImagesList();
   void UpdateImageSelectionButtons();
   void UpdateParameters();

   void __TargetImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void __TargetImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __TargetImages_NodeSelectionUpdated( TreeBox& sender );
   void __TargetImages_Click( Button& sender, bool checked );
   void __ToggleSection( SectionBar& sender, Control& section, bool start );
   void __TypeItemSelected( ComboBox& sender, int itemIndex );
   void __ShrinkCountValueUpdated( SpinBox& sender, int value );
   void __FeatherRadiusValueUpdated( SpinBox& sender, int value );
   void __BlackPointValueUpdated( NumericEdit& sender, double value );
   void __GenerateMaskClicked( Button& sender, bool checked );
   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern GradientsMergeMosaicInterface* TheGradientsMergeMosaicInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif

// ----------------------------------------------------------------------------
// EOF GradientsMergeMosaicInterface.h - Released 2021-04-09T19:41:49Z
