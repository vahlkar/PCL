//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.1
// ----------------------------------------------------------------------------
// ColorSaturationInterface.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#ifndef __ColorSaturationInterface_h
#define __ColorSaturationInterface_h

#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/ScrollBox.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>
#include <pcl/ToolButton.h>
#include <pcl/View.h>

#include "ColorSaturationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS Graphics;

class ColorSaturationInterface : public ProcessInterface
{
public:

   ColorSaturationInterface();
   virtual ~ColorSaturationInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void RealTimePreviewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, const Rect&, int zoomLevel ) const override;
   bool GenerateRealTimePreview( UInt16Image&, const View&, const Rect&, int zoomLevel, String& info ) const override;
   bool WantsReadoutNotifications() const override;
   void BeginReadout( const View& v ) override;
   void UpdateReadout( const View& v, const DPoint& p, double R, double G, double B, double A ) override;
   void EndReadout( const View& v ) override;

private:

   ColorSaturationInstance m_instance;

   class RealTimeThread : public Thread
   {
   public:

      UInt16Image m_image;

      RealTimeThread();

      void Reset( const UInt16Image&, const ColorSaturationInstance& );

      void Run() override;

   private:

      ColorSaturationInstance m_instance;
   };

   mutable RealTimeThread* m_realTimeThread = nullptr;

   struct GUIData
   {
      GUIData( ColorSaturationInterface& );

      VerticalSizer     Global_Sizer;
         ScrollBox         Curve_ScrollBox;
         HorizontalSizer   Mode_Sizer;
            ToolButton        EditPointMode_ToolButton;
            ToolButton        SelectPointMode_ToolButton;
            ToolButton        DeletePointMode_ToolButton;
            ToolButton        ZoomInMode_ToolButton;
            ToolButton        ZoomOutMode_ToolButton;
            ToolButton        PanMode_ToolButton;
            SpinBox           Zoom_SpinBox;
            ToolButton        Zoom11_ToolButton;
            Label             Scale_Label;
            SpinBox           Scale_SpinBox;
            ToolButton        ShowGrid_ToolButton;
         Label             Info_Label;
         HorizontalSizer   CurveData_Row1_Sizer;
            NumericEdit       Input_NumericEdit;
            ToolButton        PrevPoint_ToolButton;
            ToolButton        FirstPoint_ToolButton;
            ToolButton        StoreCurve_ToolButton;
            ToolButton        RestoreCurve_ToolButton;
            ToolButton        ReverseCurve_ToolButton;
            ToolButton        ResetCurve_ToolButton;
         HorizontalSizer   CurveData_Row2_Sizer;
            NumericEdit       Output_NumericEdit;
            ToolButton        NextPoint_ToolButton;
            ToolButton        LastPoint_ToolButton;
            Label             CurrentPoint_Label;
            ToolButton        AkimaSubsplines_ToolButton;
            ToolButton        CubicSpline_ToolButton;
            ToolButton        Linear_ToolButton;
         NumericControl    HueShift_NumericControl;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;

   /*
    * Workbench
    */
   enum working_mode { EditMode, SelectMode, DeleteMode, ZoomInMode, ZoomOutMode, PanMode, NoMode = -1 };

   working_mode   m_mode            = EditMode;
   working_mode   m_savedMode       = NoMode; // for temporary keyboard mode switch

   size_type      m_currentPoint    = 0;     // point index

   bool           m_readoutActive   = false;
   double         m_readouts[ 4 ]   = {};    // 0=R 1=G 2=B 3=Alpha
   RGBColorSystem m_readoutRGBWS;

   int            m_zoomX           = 1;
   int            m_zoomY           = 1;
   int            m_scale           = 1;

   int            m_wheelSteps      = 0;     // accumulated 1/8-degree wheel steps

   bool           m_showGrid        = true;  // draw coordinate grids

   int            m_panning         = 0;     // panning the viewport?
   Point          m_panOrigin       = 0;

   bool           m_cursorVisible   = false;
   bool           m_dragging        = false; // dragging a curve point?
   Point          m_cursorPos       = -1;    // cursor position in viewport crds.
   DPoint         m_curvePos        = 0;     // cursor position in normalized crds.

   Bitmap         m_viewportBitmap  = Bitmap::Null();
   bool           m_viewportDirty   = true;

   HSCurve        m_storedCurve;

   RGBA           m_channelColor    = RGBAColor( 0xFF, 0xFF, 0x00 );
   RGBA           m_gridColor0      = RGBAColor( 0x50, 0x50, 0x50 );
   RGBA           m_gridColor1      = RGBAColor( 0x37, 0x37, 0x37 );
   RGBA           m_gridColor2      = RGBAColor( 0x60, 0x60, 0x60 );
   RGBA           m_backgroundColor = RGBAColor( 0x00, 0x00, 0x00 );

   int            m_minCurveWidth   = 300;   // these are constants currently, but who knows...
   int            m_minCurveHeight  = 300;
   int            m_scaleSize       = 8;

   bool           m_settingUp       = false; // true during viewport transitional states (e.g. resizing)

   /*
    * Auxiliary Functions
    */
   template <typename T> T ViewportToCurve( T y ) const
   {
      return 2*(y - 0.5)*m_scale;
   }

   template <typename T> T CurveToViewport( T y ) const
   {
      return 0.5*(1 + y/m_scale);
   }

   /*
    * Current curve point
    */
   size_type CurrentPoint() const
   {
      return m_currentPoint;
   }

   double CurrentInputValue() const
   {
      return m_instance.C.X( CurrentPoint() );
   }

   double CurrentOutputValue() const
   {
      return m_instance.C.Y( CurrentPoint() );
   }

   double& CurrentOutputValue()
   {
      return m_instance.C.Y( CurrentPoint() );
   }

   /*
    * Curve point modifiers
    */
   size_type FindPoint( double x, double y, int tolerancePx = 0 ) const;

   size_type FindPoint( const DPoint& p, int tolerancePx = 0 ) const
   {
      return FindPoint( p.x, p.y, tolerancePx );
   }

   size_type CreatePoint( double x, double y, int tolerancePx = 0 );

   size_type CreatePoint( const DPoint& p, int tolerancePx = 0 )
   {
      return CreatePoint( p.x, p.y, tolerancePx );
   }

   bool DragPoint( size_type i, double x, double y );

   bool DragPoint( size_type i, const DPoint& p )
   {
      return DragPoint( i, p.x, p.y );
   }

   size_type RemovePoint( double x, double y, int tolerancePx = 0 );

   size_type RemovePoint( const DPoint& p, int tolerancePx = 0 )
   {
      return RemovePoint( p.x, p.y, tolerancePx );
   }

   /*
    * Setting parameters
    */
   void SetZoom( int, int, const Point* = 0 );
   void SetMode( working_mode );
   void SetInterpolation( int );
   void SelectPoint( size_type, bool pan = true );

   /*
    * GUI Updates
    */
   void UpdateControls();
   void UpdateModeControls();
   void UpdateZoomControls();
   void UpdateGraphicsControls();
   void UpdateInterpolationControls();
   void UpdateCurveControls();
   void UpdateCurve();
   void UpdateCurveInfo();
   void UpdateRealTimePreview();

   /*
    * Screen bitmap generation
    */
   void RegenerateViewport();

   /*
    * Curve drawing primitives
    */
   void PlotGrid( Graphics&, const Rect& viewport, int width, int height, int hZoom, int vZoom );
   void PlotScale( Graphics&, const Rect& viewport, int width, int height );
   void PlotCurve( Graphics&, const Rect& viewport, int width, int height, int hZoom, int vZoom );
   void PlotReadouts( Graphics&, const Bitmap&, const Rect& viewport, int width, int height );
   void PlotCursor( Graphics&, const Rect& viewport );

   RGBA ScaleColor( float ) const;

   /*
    * GUI Event Handlers
    */
   void __Curve_Paint( Control& sender, const pcl::Rect& updateRect );
   void __Curve_Resize( Control& sender, int newWidth, int newHeight, int oldWidth, int oldHeight );
   void __Curve_ScrollPosUpdated( ScrollBox& sender, int pos );
   void __Curve_Enter( Control& sender );
   void __Curve_Leave( Control& sender );
   void __Curve_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Curve_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Curve_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
   void __Curve_MouseWheel( Control& sender, const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers );
   void __Curve_KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   void __CurveParameter_ValueUpdated( NumericEdit& sender, double value );
   void __Mode_ButtonClick( Button&, bool );
   void __Zoom_ButtonClick( Button&, bool );
   void __ShowGrid_ButtonClick( Button&, bool );
   void __PointNavigation_ButtonClick( Button&, bool );
   void __Interpolation_ButtonClick( Button&, bool );
   void __StoreCurve_ButtonClick( Button&, bool );
   void __RestoreCurve_ButtonClick( Button&, bool );
   void __ReverseCurve_ButtonClick( Button&, bool );
   void __ResetCurve_ButtonClick( Button&, bool );
   void __Zoom_ValueUpdated( SpinBox& sender, int value );
   void __Scale_ValueUpdated( SpinBox& sender, int value );
   void __KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   void __KeyRelease( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   void __HueShift_ValueUpdated( NumericEdit& sender, double value );
   void __UpdateRealTimePreview_Timer( Timer& );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ColorSaturationInterface* TheColorSaturationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ColorSaturationInterface_h

// ----------------------------------------------------------------------------
// EOF ColorSaturationInterface.h - Released 2021-04-09T19:41:48Z
