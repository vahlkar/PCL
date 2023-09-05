//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.1
// ----------------------------------------------------------------------------
// AdaptiveStretchCurveGraphInterface.h - Released 2023-08-28T15:23:41Z
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

#ifndef __AdaptiveStretchCurveGraphInterface_h
#define __AdaptiveStretchCurveGraphInterface_h

#include <pcl/ComboBox.h>
#include <pcl/Control.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "AdaptiveStretchInstance.h" // for StretchCurve declaration

namespace pcl
{

// ----------------------------------------------------------------------------

class AdaptiveStretchCurveGraphInterface : public ProcessInterface
{
public:

   AdaptiveStretchCurveGraphInterface();
   virtual ~AdaptiveStretchCurveGraphInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   using ProcessInterface::Launch;
   void SaveSettings() const override;
   void LoadSettings() override;

   void UpdateGraph( const StretchCurve& curve );

private:

   struct GUIData
   {
      GUIData( AdaptiveStretchCurveGraphInterface& );

      VerticalSizer     Global_Sizer;
         Control           CurveGraph_Control;
         HorizontalSizer   Options_Sizer;
            ComboBox          Size_ComboBox;
            ToolButton        Render_ToolButton;
            ToolButton        Edit_ToolButton;
   };

   GUIData* GUI = nullptr;

   /*
    * Parameters
    */
   int    m_width           = 400;
   int    m_height          = 400;
   RGBA   m_backgroundColor = 0xFFFFFFFF; // white
   RGBA   m_curveColor      = 0xFFFF0000; // red
   RGBA   m_gridColor       = 0xFFD0D0D0; // light gray
   RGBA   m_axisColor       = 0xFF000000; // black
   String m_fontFace        = "Helvetica";
   int    m_fontSize        = 12;
   int    m_tickSize        = 5;
   int    m_margin          = 15;

   /*
    * Working data
    */
   StretchCurve m_curve;
   Rect         m_curveRect   = 0;
   Bitmap       m_gridBitmap  = Bitmap::Null();
   Bitmap       m_curveBitmap = Bitmap::Null();

   void UpdateControls();
   void Resize( int width, int height );

   void GenerateGraphGrid();
   void GenerateGraphCurve();

   void __Paint( Control& sender, const pcl::Rect& updateRect );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __Click( Button& sender, bool checked );

   void __Hide( Control& );
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AdaptiveStretchCurveGraphInterface* TheAdaptiveStretchCurveGraphInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AdaptiveStretchCurveGraphInterface_h

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchCurveGraphInterface.h - Released 2023-08-28T15:23:41Z
