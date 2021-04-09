//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 0.6.4
// ----------------------------------------------------------------------------
// GradientsHdrInterface.h - Released 2021-04-09T19:41:49Z
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

#ifndef __GradientsHdrInterface_h
#define __GradientsHdrInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/CheckBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/Sizer.h>
#include <pcl/Timer.h>

#include "GradientsHdrInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class GradientsHdrInterface : public ProcessInterface
{
public:

   GradientsHdrInterface();
   virtual ~GradientsHdrInterface();

   IsoString Id() const override;
   IsoString Aliases() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void RealTimePreviewUpdated( bool active ) override;
   bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, const Rect&, int zoomLevel ) const override;
   bool GenerateRealTimePreview( UInt16Image&, const View&, const Rect&, int zoomLevel, String& info ) const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   GradientsHdrInstance instance;

   struct GUIData
   {
      GUIData( GradientsHdrInterface& );

      VerticalSizer global_Sizer;
      NumericControl logMaxGradient_NumericControl;
      NumericControl logMinGradient_NumericControl;
      NumericControl expGradient_NumericControl;
      HorizontalSizer rescale01_Sizer;
      Label rescale01_Label;
      CheckBox rescale01_CheckBox;
      Label preserveColor_Label;
      CheckBox preserveColor_CheckBox;
      Timer UpdateRealTime_Timer;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateRealTimePreview();

   void __expGradientUpdated( NumericEdit& sender, double value );
   void __logMaxGradientUpdated( NumericEdit& sender, double value );
   void __logMinGradientUpdated( NumericEdit& sender, double value );
   void __rescale01Clicked( Button& sender, bool value );
   void __preserveColorClicked( Button& sender, bool value );
   void __UpdateRealTime_Timer( Timer& );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern GradientsHdrInterface* TheGradientsHdrInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __GradientsHdrInterface_h

// ----------------------------------------------------------------------------
// EOF GradientsHdrInterface.h - Released 2021-04-09T19:41:49Z
