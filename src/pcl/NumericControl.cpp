//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/NumericControl.cpp - Released 2024-12-28T16:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/ErrorHandler.h>
#include <pcl/NumericControl.h>

namespace pcl
{

// ----------------------------------------------------------------------------

NumericEdit::NumericEdit( Control& parent )
   : Control( parent )
{
   SetSizer( sizer );

   sizer.SetSpacing( 4 );
   sizer.Add( label );
   sizer.Add( edit );

   label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   label.OnMousePress( (Control::mouse_button_event_handler)&NumericEdit::MousePress, *this );

   edit.OnKeyPress( (Control::keyboard_event_handler)&NumericEdit::KeyPressed, *this );
   edit.OnEditCompleted( (Edit::edit_event_handler)&NumericEdit::EditCompleted, *this );
   edit.OnReturnPressed( (Edit::edit_event_handler)&NumericEdit::ReturnPressed, *this );
   edit.OnGetFocus( (Control::event_handler)&NumericEdit::GetFocus, *this );
   edit.OnLoseFocus( (Control::event_handler)&NumericEdit::LoseFocus, *this );

   AdjustToContents();

   SetFixedHeight();
   SetBackgroundColor( 0 ); // transparent background

   SetChildToFocus( edit );

   UpdateRegExp();
}

// ----------------------------------------------------------------------------

void NumericEdit::SetValue( double value )
{
   m_value = Range( m_real ? value : Round( value ), m_lowerBound, m_upperBound );
   UpdateControls();
}

// ----------------------------------------------------------------------------

String NumericEdit::ValueAsString( double value ) const
{
   value = Range( value, m_lowerBound, m_upperBound );

   String vs;
   char sc = 0;
   if ( m_sign )
   {
      sc = SignChar( Round( value, m_precision ) );
      value = Abs( value );
   }

   if ( m_real )
   {
      if ( UseScientific( value ) )
         vs.Format( "%.*e", m_precision, value );
      else
         vs.Format( "%.*f", PrecisionForValue( value ), value );
   }
   else
      vs.Format( "%.0f", value );

   return m_sign ? sc + vs : vs;
}

// ----------------------------------------------------------------------------

int NumericEdit::PrecisionForValue( double value ) const
{
   if ( !m_fixed )
   {
      value = Abs( value );
      if ( value >= 10 )
         return Max( 0, m_precision - Max( 0, TruncInt( Log( value ) ) ) );
   }
   return m_precision;
}

// ----------------------------------------------------------------------------

bool NumericEdit::UseScientific( double value ) const
{
   return m_scientific &&
      (m_sciTriggerExp < 0 || value != 0 && (Abs( value ) > Pow10I<double>()( +m_sciTriggerExp ) ||
                                             Abs( value ) < Pow10I<double>()( -m_sciTriggerExp )));
}

// ----------------------------------------------------------------------------

void NumericEdit::UpdateRegExp()
{
   if ( m_useRegExp )
   {
      edit.SetValidatingRegExp( m_real ?
         "([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)|(0[xX][0-9a-fA-F]+)" :
         "([-+]?[0-9]+)|(0[xX][0-9a-fA-F]+)" );
   }
   else
      edit.DisableValidatingRegExp();
}

// ----------------------------------------------------------------------------

int NumericEdit::MinEditWidth() const
{
   int n = int( Max( ValueAsString( m_lowerBound ).Length(), ValueAsString( m_upperBound ).Length() ) );
   return edit.Font().Width( String( '0', n+1 ) ) + LogicalPixelsToPhysical( 1+2+2+1 );
}

// ----------------------------------------------------------------------------

void NumericEdit::AdjustEditWidth()
{
   edit.SetFixedWidth( MinEditWidth() );
   AdjustToContents();
}

// ----------------------------------------------------------------------------

void NumericEdit::SetReal( bool real )
{
   if ( real != m_real )
   {
      m_real = real;
      if ( !m_real )
         m_value = Round( m_value );
      if ( m_autoEditWidth )
         AdjustEditWidth();
      SetValue( m_value );
      UpdateRegExp();
   }
}

// ----------------------------------------------------------------------------

void NumericEdit::SetRange( double lr, double ur )
{
   m_lowerBound = Min( lr, ur );
   m_upperBound = Max( lr, ur );
   if ( m_autoEditWidth )
      AdjustEditWidth();
   SetValue( m_value );
}

// ----------------------------------------------------------------------------

void NumericEdit::SetPrecision( int n )
{
   m_precision = Range( n, 0, 16 );
   if ( m_autoEditWidth )
      AdjustEditWidth();
   UpdateControls();
}

// ----------------------------------------------------------------------------

void NumericEdit::EnableFixedPrecision( bool enable )
{
   m_fixed = enable;
   if ( m_autoEditWidth )
      AdjustEditWidth();
   UpdateControls();
}

// ----------------------------------------------------------------------------

void NumericEdit::EnableScientificNotation( bool enable )
{
   m_scientific = enable;
   if ( m_autoEditWidth )
      AdjustEditWidth();
   UpdateControls();
}

// ----------------------------------------------------------------------------

void NumericEdit::SetScientificNotationTriggerExponent( int exp10 )
{
   m_sciTriggerExp = exp10;
   if ( m_autoEditWidth )
      AdjustEditWidth();
   UpdateControls();
}

// ----------------------------------------------------------------------------

void NumericEdit::EnableFixedSign( bool enable )
{
   m_sign = enable;
   if ( m_autoEditWidth )
      AdjustEditWidth();
   UpdateControls();
}

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void NumericEdit::OnValueUpdated( value_event_handler f, Control& c )
{
   if ( f == nullptr || c.IsNull() )
   {
      if ( !m_handlers.IsNull() )
      {
         m_handlers->onValueUpdated = nullptr;
         m_handlers->onValueUpdatedReceiver = nullptr;
      }
   }
   else
   {
      INIT_EVENT_HANDLERS();
      m_handlers->onValueUpdated = f;
      m_handlers->onValueUpdatedReceiver = &c;
   }
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

void NumericEdit::UpdateControls()
{
   edit.SetText( ValueAsString() );
}

// ----------------------------------------------------------------------------

void NumericEdit::EditCompleted( Edit& sender )
{
   PCL_MEMBER_REENTRANCY_GUARDED_BEGIN( EditCompleted )

   try
   {
      double newValue;
      if ( m_real )
      {
         newValue = sender.Text().ToDouble();
         if ( UseScientific( newValue ) )
            newValue = IsoString().Format( "%.*e", m_precision, newValue ).ToDouble();
         else
            newValue = Round( newValue, PrecisionForValue( newValue ) );
      }
      else
         newValue = sender.Text().ToInt();

      if ( m_lowerBound < m_upperBound )
         if ( newValue < m_lowerBound || newValue > m_upperBound )
            throw ParseError( String().Format( "Numeric value out of range: %.16g - "
                                               "valid range is [%.16g,%.16g]", newValue, m_lowerBound, m_upperBound ) );
      bool changed = newValue != m_value;
      if ( changed )
         m_value = newValue;
      UpdateControls();
      if ( changed )
         if ( !m_handlers.IsNull() )
            if ( m_handlers->onValueUpdated != nullptr )
               (m_handlers->onValueUpdatedReceiver->*m_handlers->onValueUpdated)( *this, m_value );
      return;
   }
   ERROR_HANDLER

   UpdateControls();

   PCL_REENTRANCY_GUARDED_END
}

// ----------------------------------------------------------------------------

void NumericEdit::KeyPressed( Control& sender, int key, unsigned modifiers, bool& wantsKey )
{
   wantsKey = false;
}

// ----------------------------------------------------------------------------

void NumericEdit::ReturnPressed( Edit& /*sender*/ )
{
   // placeholder
}

// ----------------------------------------------------------------------------

void NumericEdit::GetFocus( Control& /*sender*/ )
{
   //if ( !edit.IsReadOnly() )
   //   edit.SelectAll();
}

// ----------------------------------------------------------------------------

void NumericEdit::LoseFocus( Control& /*sender*/ )
{
   if ( !edit.IsReadOnly() )
      EditCompleted( edit );
}

// ----------------------------------------------------------------------------

void NumericEdit::MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( !edit.IsReadOnly() )
      if ( sender == label )
      {
         EditCompleted( edit );
         edit.Focus();
         edit.SelectAll();
      }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

NumericControl::NumericControl( Control& parent )
   : NumericEdit( parent )
{
   sizer.Add( slider, 100 );

   slider.SetRange( 0, 50 );
   slider.SetScaledMinWidth( 50+16 );
   slider.SetFixedHeight( Height() );
   slider.SetPageSize( 5 );
   slider.SetTickInterval( 5 );
   slider.SetTickStyle( TickStyle::NoTicks );
   slider.SetFocusStyle( FocusStyle::Click );
   slider.OnGetFocus( (Control::event_handler)&NumericControl::GetFocus, *this );
   slider.OnValueUpdated( (Slider::value_event_handler)&NumericControl::ValueUpdated, *this );

   AdjustToContents();
}

// ----------------------------------------------------------------------------

void NumericControl::SetRange( double lr, double ur )
{
   if ( lr < 0 || ur < 0 )
      m_exponential = false;
   NumericEdit::SetRange( lr, ur );
}

// ----------------------------------------------------------------------------

void NumericControl::EnableExponentialResponse( bool enable )
{
   if ( enable && m_lowerBound < 0 )
      throw Error( "NumericControl: Exponential slider response unavailable for negative lower bound." );
   m_exponential = enable;
   UpdateControls();
}

// ----------------------------------------------------------------------------

double NumericControl::SliderValueToControl( int sliderValue ) const
{
   int sliderMinValue, sliderMaxValue;
   slider.GetRange( sliderMinValue, sliderMaxValue );
   double sliderDelta = sliderMaxValue - sliderMinValue;
   double sliderNormValue = (sliderValue - sliderMinValue)/sliderDelta;
   return Range( Round( m_exponential ?
                              (1 + m_lowerBound)*Exp( Ln( (1 + m_upperBound)/(1 + m_lowerBound) )*sliderNormValue ) - 1 :
                              m_lowerBound + (m_upperBound - m_lowerBound)*sliderNormValue,
                        m_real ? Max( 0, TruncInt( Log( sliderDelta ) ) ) : 0 ), m_lowerBound, m_upperBound );
}

// ----------------------------------------------------------------------------

int NumericControl::ControlValueToSlider( double value ) const
{
   int sliderMinValue, sliderMaxValue;
   slider.GetRange( sliderMinValue, sliderMaxValue );
   double sliderDelta = sliderMaxValue - sliderMinValue;
   return Range( RoundInt( sliderMinValue + sliderDelta*(m_exponential ?
                              Ln( (1 + value)/(1 + m_lowerBound) )/Ln( (1 + m_upperBound)/(1 + m_lowerBound) ) :
                              (value - m_lowerBound)/(m_upperBound - m_lowerBound)) ),
                 sliderMinValue, sliderMaxValue );
}

// ----------------------------------------------------------------------------

void NumericControl::UpdateControls()
{
   NumericEdit::UpdateControls();
   slider.SetValue( ControlValueToSlider( m_value ) );
}

// ----------------------------------------------------------------------------

void NumericControl::ValueUpdated( Slider& sender, int value )
{
   double newValue = SliderValueToControl( value );
   if ( newValue != m_value )
   {
      m_value = newValue;
      edit.SetText( ValueAsString() );
      if ( !m_handlers.IsNull() )
         if ( m_handlers->onValueUpdated != nullptr )
            (m_handlers->onValueUpdatedReceiver->*m_handlers->onValueUpdated)( *this, m_value );
   }
}

// ----------------------------------------------------------------------------

void NumericControl::GetFocus( Control& sender )
{
   if ( sender == slider )
   {
      if ( !edit.IsReadOnly() )
      {
         edit.Focus();
         edit.SelectAll();
      }
   }
   else
   {
      NumericEdit::GetFocus( sender );
   }
}

// ----------------------------------------------------------------------------

void NumericControl::KeyPressed( Control& sender, int key, unsigned modifiers, bool& wantsKey )
{
   int delta;
   switch ( key )
   {
   case KeyCode::Up:
      delta = +100;
      break;
   case KeyCode::Down:
      delta = -100;
      break;
   case KeyCode::PageUp:
      delta = +10;
      break;
   case KeyCode::PageDown:
      delta = -10;
      break;
   default:
      wantsKey = false;
      return;
   }

   int sliderMinValue, sliderMaxValue;
   slider.GetRange( sliderMinValue, sliderMaxValue );
   int sliderValue = slider.Value();
   int newSliderValue = RoundInt( sliderValue + double( slider.MaxValue() - slider.MinValue() )/delta );
   newSliderValue = Range( newSliderValue, sliderMinValue, sliderMaxValue );
   if ( newSliderValue != sliderValue )
   {
      slider.SetValue( newSliderValue );
      ValueUpdated( slider, newSliderValue );
   }
   wantsKey = true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/NumericControl.cpp - Released 2024-12-28T16:53:56Z
