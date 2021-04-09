//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.1
// ----------------------------------------------------------------------------
// BinarizeInterface.cpp - Released 2021-04-09T19:41:48Z
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

#include "BinarizeInterface.h"
#include "BinarizeParameters.h"
#include "BinarizeProcess.h"

#include <pcl/Color.h>
#include <pcl/Graphics.h>
#include <pcl/MuteStatus.h>
#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BinarizeInterface* TheBinarizeInterface = nullptr;

// ----------------------------------------------------------------------------

BinarizeInterface::BinarizeInterface()
   : m_instance( TheBinarizeProcess )
{
   TheBinarizeInterface = this;
}

// ----------------------------------------------------------------------------

BinarizeInterface::~BinarizeInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString BinarizeInterface::Id() const
{
   return "Binarize";
}

// ----------------------------------------------------------------------------

MetaProcess* BinarizeInterface::Process() const
{
   return TheBinarizeProcess;
}

// ----------------------------------------------------------------------------

String BinarizeInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/Binarize.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures BinarizeInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton;
}

// ----------------------------------------------------------------------------

void BinarizeInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}

// ----------------------------------------------------------------------------

void BinarizeInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void BinarizeInterface::ResetInstance()
{
   BinarizeInstance defaultInstance( TheBinarizeProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool BinarizeInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Binarize" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheBinarizeProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* BinarizeInterface::NewProcess() const
{
   return new BinarizeInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool BinarizeInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const BinarizeInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Binarize instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool BinarizeInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool BinarizeInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

bool BinarizeInterface::WantsReadoutNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void BinarizeInterface::UpdateReadout( const View& v, const DPoint&, double R, double G, double B, double /*A*/ )
{
   if ( GUI != nullptr && IsVisible() )
   {
      if ( m_instance.p_global )
      {
         RGBColorSystem rgbws;
         v.Window().GetRGBWS( rgbws );
         m_instance.p_level[0] = m_instance.p_level[1] = m_instance.p_level[2] = rgbws.Lightness( R, G, B );
      }
      else
      {
         m_instance.p_level[0] = R;
         m_instance.p_level[1] = G;
         m_instance.p_level[2] = B;
      }

      UpdateControls();

      if ( !RealTimePreview::IsUpdating() )
         RealTimePreview::Update();
   }
}

// ----------------------------------------------------------------------------

bool BinarizeInterface::RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, const Rect&, int ) const
{
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BinarizeInterface::RealTimeThread::RealTimeThread()
   : m_instance( TheBinarizeProcess )
{
}

// ----------------------------------------------------------------------------

void BinarizeInterface::RealTimeThread::Reset( const UInt16Image& image, const BinarizeInstance& instance )
{
   image.ResetSelections();
   m_image.Assign( image );
   m_instance.Assign( instance );
}

// ----------------------------------------------------------------------------

void BinarizeInterface::RealTimeThread::Run()
{
   MuteStatus status;
   m_image.SetStatusCallback( &status );
   m_instance.Preview( m_image );
   m_image.ResetSelections();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool BinarizeInterface::GenerateRealTimePreview( UInt16Image& image, const View&, const Rect&, int, String& ) const
{
   m_realTimeThread = new RealTimeThread;

   for ( ;; )
   {
      m_realTimeThread->Reset( image, m_instance );
      m_realTimeThread->Start();

      while ( m_realTimeThread->IsActive() )
      {
         ProcessEvents();

         if ( !IsRealTimePreviewActive() )
         {
            m_realTimeThread->Abort();
            m_realTimeThread->Wait();

            delete m_realTimeThread;
            m_realTimeThread = nullptr;
            return false;
         }
      }

      if ( !m_realTimeThread->IsAborted() )
      {
         image.Assign( m_realTimeThread->m_image );

         delete m_realTimeThread;
         m_realTimeThread = nullptr;
         return true;
      }
   }
}

// ----------------------------------------------------------------------------

void BinarizeInterface::UpdateControls()
{
   GUI->GlobalK_RadioButton.SetChecked( m_instance.p_global );
   GUI->RGB_RadioButton.SetChecked( !m_instance.p_global );

   GUI->V0_NumericControl.label.SetText( m_instance.p_global ? "RGB/K:" : "R/K:" );
   GUI->V0_NumericControl.SetValue( m_instance.p_level[0] );

   GUI->V1_NumericControl.Enable( !m_instance.p_global );
   GUI->V1_NumericControl.SetValue( m_instance.p_level[1] );

   GUI->V2_NumericControl.Enable( !m_instance.p_global );
   GUI->V2_NumericControl.SetValue( m_instance.p_level[2] );

   GUI->ColorSample_Control.Update();
}

// ----------------------------------------------------------------------------

void BinarizeInterface::UpdateRealTimePreview()
{
   if ( IsRealTimePreviewActive() )
   {
      if ( m_realTimeThread != nullptr )
         m_realTimeThread->Abort();
      GUI->UpdateRealTimePreview_Timer.Start();
   }
}

// ----------------------------------------------------------------------------

void BinarizeInterface::__Mode_Click( Button& sender, bool checked )
{
   if ( sender == GUI->RGB_RadioButton )
      m_instance.p_global = false;
   else if ( sender == GUI->GlobalK_RadioButton )
   {
      m_instance.p_global = true;
      m_instance.p_level[1] = m_instance.p_level[2] = m_instance.p_level[0];
   }

   UpdateControls();
   UpdateRealTimePreview();
}

// ----------------------------------------------------------------------------

void BinarizeInterface::__LevelValues_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->V0_NumericControl )
   {
      m_instance.p_level[0] = value;

      if ( m_instance.p_global )
      {
         m_instance.p_level[1] = m_instance.p_level[2] = value;
         GUI->V1_NumericControl.SetValue( value );
         GUI->V2_NumericControl.SetValue( value );
      }
   }
   else if ( sender == GUI->V1_NumericControl )
      m_instance.p_level[1] = value;
   else if ( sender == GUI->V2_NumericControl )
      m_instance.p_level[2] = value;

   GUI->ColorSample_Control.Update();

   UpdateRealTimePreview();
}

// ----------------------------------------------------------------------------

void BinarizeInterface::__ColorSample_Paint( Control& sender, const Rect& /*updateRect*/ )
{
   Graphics g( sender );
   g.SetBrush( RGBAColor( float( m_instance.p_level[0] ), float( m_instance.p_level[1] ), float( m_instance.p_level[2] ) ) );
   g.SetPen( 0xff000000, sender.DisplayPixelRatio() );
   g.DrawRect( sender.BoundsRect() );
}

// ----------------------------------------------------------------------------

void BinarizeInterface::__UpdateRealTimePreview_Timer( Timer& sender )
{
   if ( m_realTimeThread != nullptr )
      if ( m_realTimeThread->IsActive() )
         return;

   if ( IsRealTimePreviewActive() )
      RealTimePreview::Update();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BinarizeInterface::GUIData::GUIData( BinarizeInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth = fnt.Width( String( "RGB/K:" ) + 'T' );

   //

   RGB_RadioButton.SetText( "Individual RGB/K channels" );
   RGB_RadioButton.SetToolTip( "Use an independent binarization threshold for each RGB/gray channel." );
   RGB_RadioButton.OnClick( (Button::click_event_handler)&BinarizeInterface::__Mode_Click, w );

   GlobalK_RadioButton.SetText( "Joint RGB/K channels" );
   GlobalK_RadioButton.SetToolTip( "Use a unique binarization threshold for all RGB/gray channels." );
   GlobalK_RadioButton.OnClick( (Button::click_event_handler)&BinarizeInterface::__Mode_Click, w );

   Mode_Sizer.SetSpacing( 8 );
   Mode_Sizer.Add( GlobalK_RadioButton );
   Mode_Sizer.Add( RGB_RadioButton );
   Mode_Sizer.AddStretch();

   //

   V0_NumericControl.label.SetText( "RGB/K:" );
   V0_NumericControl.label.SetFixedWidth( labelWidth );
   V0_NumericControl.slider.SetScaledMinWidth( 200 );
   V0_NumericControl.slider.SetRange( 0, 100 );
   V0_NumericControl.SetReal();
   V0_NumericControl.SetRange( TheBinarizeLevelRParameter->MinimumValue(), TheBinarizeLevelRParameter->MaximumValue() );
   V0_NumericControl.SetPrecision( TheBinarizeLevelRParameter->Precision() );
   V0_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BinarizeInterface::__LevelValues_ValueUpdated, w );

   //

   V1_NumericControl.label.SetText( "G:" );
   V1_NumericControl.label.SetFixedWidth( labelWidth );
   V1_NumericControl.slider.SetScaledMinWidth( 200 );
   V1_NumericControl.slider.SetRange( 0, 100 );
   V1_NumericControl.SetReal();
   V1_NumericControl.SetRange( TheBinarizeLevelGParameter->MinimumValue(), TheBinarizeLevelGParameter->MaximumValue() );
   V1_NumericControl.SetPrecision( TheBinarizeLevelGParameter->Precision() );
   V1_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BinarizeInterface::__LevelValues_ValueUpdated, w );

   //

   V2_NumericControl.label.SetText( "B:" );
   V2_NumericControl.label.SetFixedWidth( labelWidth );
   V2_NumericControl.slider.SetScaledMinWidth( 200 );
   V2_NumericControl.slider.SetRange( 0, 100 );
   V2_NumericControl.SetReal();
   V2_NumericControl.SetRange( TheBinarizeLevelBParameter->MinimumValue(), TheBinarizeLevelBParameter->MaximumValue() );
   V2_NumericControl.SetPrecision( TheBinarizeLevelBParameter->Precision() );
   V2_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BinarizeInterface::__LevelValues_ValueUpdated, w );

   //

   Level_Sizer.SetSpacing( 4 );
   Level_Sizer.Add( V0_NumericControl );
   Level_Sizer.Add( V1_NumericControl );
   Level_Sizer.Add( V2_NumericControl );

   //

   ColorSample_Control.SetScaledFixedWidth( 60 );
   ColorSample_Control.OnPaint( (Control::paint_event_handler)&BinarizeInterface::__ColorSample_Paint, w );

   Threshold_Sizer.SetSpacing( 4 );
   Threshold_Sizer.Add( Level_Sizer, 100 );
   Threshold_Sizer.Add( ColorSample_Control );

   //

   Parameters_Sizer.SetMargin( 6 );
   Parameters_Sizer.SetSpacing( 4 );
   Parameters_Sizer.Add( Mode_Sizer );
   Parameters_Sizer.Add( Threshold_Sizer );

   Parameters_GroupBox.SetTitle( "Binarization Threshold" );
   Parameters_GroupBox.SetSizer( Parameters_Sizer );

   //

   Global_Sizer.SetMargin( 6 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( Parameters_GroupBox );

   //

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval( 0.025 );
   UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&BinarizeInterface::__UpdateRealTimePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BinarizeInterface.cpp - Released 2021-04-09T19:41:48Z
