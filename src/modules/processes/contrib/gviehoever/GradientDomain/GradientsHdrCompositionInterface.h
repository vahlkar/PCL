//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 0.6.4
// ----------------------------------------------------------------------------
// GradientsHdrCompositionInterface.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2020. Licensed under LGPL 2.1
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
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

#ifndef __GradientsHdrCompositionInterface_h
#define __GradientsHdrCompositionInterface_h

#include <pcl/CheckBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

#include "GradientsHdrCompositionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class GradientsHdrCompositionInterface : public ProcessInterface
{
public:

   GradientsHdrCompositionInterface();
   virtual ~GradientsHdrCompositionInterface();

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

   GradientsHdrCompositionInstance instance;

   struct GUIData
   {
      GUIData( GradientsHdrCompositionInterface& );

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

      SectionBar Parameters_SectionBar;
      Control Parameters_Control;
      VerticalSizer Parameters_Sizer;
      HorizontalSizer Bias_Sizer;
      NumericControl LogBias_NumericControl;
      HorizontalSizer KeepLog_Sizer;
      Label KeepLog_Label;
      CheckBox KeepLog_CheckBox;
      HorizontalSizer GenerateMask_Sizer;
      Label GenerateMask_Label;
      CheckBox GenerateMask_CheckBox;
      Label NegativeBias_Label;
      CheckBox NegativeBias_CheckBox;
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

   // void __CheckSection( SectionBar& sender, bool checked );
   void __ToggleSection( SectionBar& sender, Control& section, bool start );

   /// callback for logBias numeric control
   void __logBiasUpdated( NumericEdit& sender, double value );

   /// callback for keepLog checkbox
   void __KeepLogClicked( Button& sender, bool checked );

   /// callback for negativeBias checkbox
   void __NegativeBiasClicked( Button& sender, bool checked );

   /// callback for generate mask checkbox
   void __GenerateMaskClicked( Button& sender, bool checked );

   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern GradientsHdrCompositionInterface* TheGradientsHdrCompositionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif

// ----------------------------------------------------------------------------
// EOF GradientsHdrCompositionInterface.h - Released 2024-12-28T16:54:15Z
