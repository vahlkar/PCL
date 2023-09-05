//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.1
// ----------------------------------------------------------------------------
// AdaptiveStretchInterface.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __AdaptiveStretchInterface_h
#define __AdaptiveStretchInterface_h

#include <pcl/CheckBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>

#include "ExponentialNumericControl.h"
#include "AdaptiveStretchInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class AdaptiveStretchInterface : public ProcessInterface
{
public:

   AdaptiveStretchInterface();
   virtual ~AdaptiveStretchInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void RealTimePreviewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, const Rect&, int zoomLevel ) const override;
   bool GenerateRealTimePreview( UInt16Image&, const View&, const Rect&, int zoomLevel, String& info ) const override;

private:

   AdaptiveStretchInstance m_instance;

   class RealTimeThread : public Thread
   {
   public:

      UInt16Image  m_image;
      StretchCurve m_curve;

      RealTimeThread( const View& );

      void Reset( const UInt16Image&, const AdaptiveStretchInstance& );

      void Run() override;

   private:

      AdaptiveStretchInstance m_instance;
      const View&             m_view;
   };

   mutable RealTimeThread* m_realTimeThread = nullptr;

   struct GUIData
   {
      GUIData( AdaptiveStretchInterface& );

      VerticalSizer  Global_Sizer;

         ExponentialNumericControl  NoiseThreshold_NumericControl;
         HorizontalSizer            Protection_Sizer;
            ExponentialNumericControl  Protection_NumericControl;
            CheckBox                   UseProtection_CheckBox;
         NumericEdit                MaxCurvePoints_NumericEdit;
         HorizontalSizer            PlotCurveGraph_Sizer;
            CheckBox                   PlotCurveGraph_CheckBox;

         SectionBar                 ROI_SectionBar;
         Control                    ROI_Control;
         VerticalSizer              ROI_Sizer;
            HorizontalSizer            ROIRow1_Sizer;
               NumericEdit                ROIX0_NumericEdit;
               NumericEdit                ROIY0_NumericEdit;
            HorizontalSizer            ROIRow2_Sizer;
               NumericEdit                ROIWidth_NumericEdit;
               NumericEdit                ROIHeight_NumericEdit;
               PushButton                 ROISelectPreview_Button;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateRealTimePreview();
   void OpenCurveGraph() const;
   void CloseCurveGraph() const;
   void UpdateCurveGraph( const StretchCurve& ) const;

   void __ValueUpdated( NumericEdit& sender, double value );
   void __Click( Button& sender, bool checked );
   void __SectionBarCheck( SectionBar& sender, bool checked );
   void __Hide( Control& );
   void __UpdateRealTimePreview_Timer( Timer& );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AdaptiveStretchInterface* TheAdaptiveStretchInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AdaptiveStretchInterface_h

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchInterface.h - Released 2023-08-28T15:23:41Z
