//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.1
// ----------------------------------------------------------------------------
// ChannelExtractionInterface.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "ChannelExtractionInterface.h"
#include "ChannelExtractionParameters.h"
#include "ChannelExtractionProcess.h"

#include <pcl/ErrorHandler.h>

#define AUTO_ID   "<Auto>"

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelExtractionInterface* TheChannelExtractionInterface = nullptr;

// ----------------------------------------------------------------------------

ChannelExtractionInterface::ChannelExtractionInterface()
   : m_instance( TheChannelExtractionProcess )
{
   TheChannelExtractionInterface = this;
}

// ----------------------------------------------------------------------------

ChannelExtractionInterface::~ChannelExtractionInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString ChannelExtractionInterface::Id() const
{
   return "ChannelExtraction";
}

// ----------------------------------------------------------------------------

MetaProcess* ChannelExtractionInterface::Process() const
{
   return TheChannelExtractionProcess;
}

// ----------------------------------------------------------------------------

String ChannelExtractionInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/ChannelExtraction.svg";
}

// ----------------------------------------------------------------------------

void ChannelExtractionInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void ChannelExtractionInterface::ResetInstance()
{
   ChannelExtractionInstance defaultInstance( TheChannelExtractionProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool ChannelExtractionInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ChannelExtraction" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheChannelExtractionProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ChannelExtractionInterface::NewProcess() const
{
   return new ChannelExtractionInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool ChannelExtractionInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const ChannelExtractionInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a ChannelExtraction instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool ChannelExtractionInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ChannelExtractionInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void ChannelExtractionInterface::UpdateControls()
{
   GUI->RGB_RadioButton.SetChecked( m_instance.ColorSpace() == ColorSpaceId::RGB );
   GUI->HSV_RadioButton.SetChecked( m_instance.ColorSpace() == ColorSpaceId::HSV );
   GUI->HSI_RadioButton.SetChecked( m_instance.ColorSpace() == ColorSpaceId::HSI );
   GUI->CIEXYZ_RadioButton.SetChecked( m_instance.ColorSpace() == ColorSpaceId::CIEXYZ );
   GUI->CIELab_RadioButton.SetChecked( m_instance.ColorSpace() == ColorSpaceId::CIELab );
   GUI->CIELch_RadioButton.SetChecked( m_instance.ColorSpace() == ColorSpaceId::CIELch );

   GUI->C0_CheckBox.SetText( ColorSpaceId::ChannelId( m_instance.ColorSpace(), 0 ) );
   GUI->C0_CheckBox.SetChecked( m_instance.IsChannelEnabled( 0 ) );

   GUI->C0_Edit.SetText( m_instance.ChannelId( 0 ).IsEmpty() ? AUTO_ID : m_instance.ChannelId( 0 ) );
   GUI->C0_Edit.Enable( m_instance.IsChannelEnabled( 0 ) );

   GUI->C1_CheckBox.SetText( ColorSpaceId::ChannelId( m_instance.ColorSpace(), 1 ) );
   GUI->C1_CheckBox.SetChecked( m_instance.IsChannelEnabled( 1 ) );

   GUI->C1_Edit.SetText( m_instance.ChannelId( 1 ).IsEmpty() ? AUTO_ID : m_instance.ChannelId( 1 ) );
   GUI->C1_Edit.Enable( m_instance.IsChannelEnabled( 1 ) );

   GUI->C2_CheckBox.SetText( ColorSpaceId::ChannelId( m_instance.ColorSpace(), 2 ) );
   GUI->C2_CheckBox.SetChecked( m_instance.IsChannelEnabled( 2 ) );

   GUI->C2_Edit.SetText( m_instance.ChannelId( 2 ).IsEmpty() ? AUTO_ID : m_instance.ChannelId( 2 ) );
   GUI->C2_Edit.Enable( m_instance.IsChannelEnabled( 2 ) );

   GUI->SampleFormat_ComboBox.SetCurrentItem( m_instance.SampleFormat() );

   GUI->InheritAstrometricSolution_CheckBox.SetChecked( m_instance.p_inheritAstrometricSolution );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ChannelExtractionInterface::e_ColorSpace_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->RGB_RadioButton )
      m_instance.p_colorSpace = ColorSpaceId::RGB;
   else if ( sender == GUI->HSV_RadioButton )
      m_instance.p_colorSpace = ColorSpaceId::HSV;
   else if ( sender == GUI->HSI_RadioButton )
      m_instance.p_colorSpace = ColorSpaceId::HSI;
   else if ( sender == GUI->CIEXYZ_RadioButton )
      m_instance.p_colorSpace = ColorSpaceId::CIEXYZ;
   else if ( sender == GUI->CIELab_RadioButton )
      m_instance.p_colorSpace = ColorSpaceId::CIELab;
   else if ( sender == GUI->CIELch_RadioButton )
      m_instance.p_colorSpace = ColorSpaceId::CIELch;

   m_instance.p_channelEnabled[0] = m_instance.p_channelEnabled[1] = m_instance.p_channelEnabled[2] = true;

   UpdateControls();
}

// ----------------------------------------------------------------------------

void ChannelExtractionInterface::e_Channel_Click( Button& sender, bool checked )
{
   int i;
   if ( sender == GUI->C0_CheckBox )
      i = 0;
   else if ( sender == GUI->C1_CheckBox )
      i = 1;
   else if ( sender == GUI->C2_CheckBox )
      i = 2;
   else if ( sender == GUI->InheritAstrometricSolution_CheckBox )
   {
      m_instance.p_inheritAstrometricSolution = checked;
      return;
   }
   else
      return;

   int n = 0;
   for ( int j = 0; j < 3; ++j )
   {
      if ( j == i )
         m_instance.p_channelEnabled[i] = checked;
      if ( m_instance.p_channelEnabled[j] )
         ++n;
   }

   if ( n == 0 )
      for ( int j = 0; j < 3; ++j )
         m_instance.p_channelEnabled[j] = true;

   UpdateControls();
}

// ----------------------------------------------------------------------------

void ChannelExtractionInterface::e_ChannelId_GetFocus( Control& sender )
{
   Edit* e = dynamic_cast<Edit*>( &sender );
   if ( e != nullptr )
      if ( e->Text() == AUTO_ID )
         e->Clear();
}

// ----------------------------------------------------------------------------

void ChannelExtractionInterface::e_ChannelId_EditCompleted( Edit& sender )
{
   int i;
   if ( sender == GUI->C0_Edit )
      i = 0;
   else if ( sender == GUI->C1_Edit )
      i = 1;
   else if ( sender == GUI->C2_Edit )
      i = 2;
   else
      return;

   try
   {
      pcl::String id = sender.Text().Trimmed();
      if ( !id.IsEmpty() )
         if ( id != AUTO_ID )
            if ( !id.IsValidIdentifier() )
               throw Error( "Invalid identifier: " + id );

      m_instance.p_channelId[i] = (id != AUTO_ID) ? id : String();
      sender.SetText( m_instance.p_channelId[i].IsEmpty() ? AUTO_ID : m_instance.p_channelId[i] );
      return;
   }
   ERROR_CLEANUP(
      sender.SetText( m_instance.p_channelId[i] );
      sender.SelectAll();
      sender.Focus()
   )
}

// ----------------------------------------------------------------------------

void ChannelExtractionInterface::e_SampleFormat_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   m_instance.p_sampleFormat = itemIndex;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ChannelExtractionInterface::GUIData::GUIData( ChannelExtractionInterface& w )
{
   pcl::Font fnt = w.Font();
   int chEditWidth = fnt.Width( String( 'M', 35 ) );

   //

   RGB_RadioButton.SetText( "RGB" );
   RGB_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelExtractionInterface::e_ColorSpace_Click, w );

   HSV_RadioButton.SetText( "HSV" );
   HSV_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelExtractionInterface::e_ColorSpace_Click, w );

   HSI_RadioButton.SetText( "HSI" );
   HSI_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelExtractionInterface::e_ColorSpace_Click, w );

   ColorSpaceLeft_Sizer.Add( RGB_RadioButton );
   ColorSpaceLeft_Sizer.Add( HSV_RadioButton );
   ColorSpaceLeft_Sizer.Add( HSI_RadioButton );

   //

   CIEXYZ_RadioButton.SetText( "CIE XYZ" );
   CIEXYZ_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelExtractionInterface::e_ColorSpace_Click, w );

   CIELab_RadioButton.SetText( "CIE L*a*b*" );
   CIELab_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelExtractionInterface::e_ColorSpace_Click, w );

   CIELch_RadioButton.SetText( "CIE L*c*h*" );
   CIELch_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelExtractionInterface::e_ColorSpace_Click, w );

   ColorSpaceRight_Sizer.Add( CIEXYZ_RadioButton );
   ColorSpaceRight_Sizer.Add( CIELab_RadioButton );
   ColorSpaceRight_Sizer.Add( CIELch_RadioButton );

   //

   ColorSpace_Sizer.SetMargin( 6 );
   ColorSpace_Sizer.SetSpacing( 10 );
   ColorSpace_Sizer.Add( ColorSpaceLeft_Sizer );
   ColorSpace_Sizer.Add( ColorSpaceRight_Sizer );

   ColorSpace_GroupBox.SetTitle( "Color Space" );
   ColorSpace_GroupBox.SetSizer( ColorSpace_Sizer );

   //

   C0_CheckBox.SetText( "MM" );
   C0_CheckBox.AdjustToContents();
   C0_CheckBox.SetFixedWidth();
   C0_CheckBox.OnClick( (pcl::Button::click_event_handler)&ChannelExtractionInterface::e_Channel_Click, w );

   C0_Edit.SetMinWidth( chEditWidth );
   C0_Edit.OnGetFocus( (Control::event_handler)&ChannelExtractionInterface::e_ChannelId_GetFocus, w );
   C0_Edit.OnEditCompleted( (Edit::edit_event_handler)&ChannelExtractionInterface::e_ChannelId_EditCompleted, w );

   C0_Sizer.Add( C0_CheckBox );
   C0_Sizer.Add( C0_Edit, 100 );

   //

   C1_CheckBox.SetText( "MM" );
   C1_CheckBox.AdjustToContents();
   C1_CheckBox.SetFixedWidth();
   C1_CheckBox.OnClick( (pcl::Button::click_event_handler)&ChannelExtractionInterface::e_Channel_Click, w );

   C1_Edit.SetMinWidth( chEditWidth );
   C1_Edit.OnGetFocus( (Control::event_handler)&ChannelExtractionInterface::e_ChannelId_GetFocus, w );
   C1_Edit.OnEditCompleted( (Edit::edit_event_handler)&ChannelExtractionInterface::e_ChannelId_EditCompleted, w );

   C1_Sizer.Add( C1_CheckBox );
   C1_Sizer.Add( C1_Edit, 100 );

   //

   C2_CheckBox.SetText( "MM" );
   C2_CheckBox.AdjustToContents();
   C2_CheckBox.SetFixedWidth();
   C2_CheckBox.OnClick( (pcl::Button::click_event_handler)&ChannelExtractionInterface::e_Channel_Click, w );

   C2_Edit.SetMinWidth( chEditWidth );
   C2_Edit.OnGetFocus( (Control::event_handler)&ChannelExtractionInterface::e_ChannelId_GetFocus, w );
   C2_Edit.OnEditCompleted( (Edit::edit_event_handler)&ChannelExtractionInterface::e_ChannelId_EditCompleted, w );

   C2_Sizer.Add( C2_CheckBox );
   C2_Sizer.Add( C2_Edit, 100 );

   //

   SampleFormat_Label.SetText( "Sample Format:" );
   SampleFormat_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   SampleFormat_ComboBox.AddItem( "Same as source " );
   SampleFormat_ComboBox.AddItem( "8-bit unsigned integer (byte, 0 -> 255)" );
   SampleFormat_ComboBox.AddItem( "16-bit unsigned integer (word, 0 -> 65535)" );
   SampleFormat_ComboBox.AddItem( "32-bit unsigned integer (double word, 0 -> 4G)" );
   SampleFormat_ComboBox.AddItem( "32-bit IEEE 754 floating point (single precision)" );
   SampleFormat_ComboBox.AddItem( "64-bit IEEE 754 floating point (double precision)" );
   SampleFormat_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ChannelExtractionInterface::e_SampleFormat_ItemSelected, w );

   SampleFormat_Sizer.SetSpacing( 4 );
   SampleFormat_Sizer.Add( SampleFormat_Label );
   SampleFormat_Sizer.Add( SampleFormat_ComboBox, 100 );

   //

   InheritAstrometricSolution_CheckBox.SetText( "Inherit astrometric solution" );
   InheritAstrometricSolution_CheckBox.SetToolTip( "<p>Copy an existing astrometric solution to the newly generated "
      "channel images.</p>"
      "<p>This option is only applicable when the target image is either a main view or a complete preview. It is "
      "always ignored when the process is executed on a partial preview.</p>"
      "<p>When this option is enabled, a valid astrometric solution is inherited if it exists in the target image, "
      "that is, in the image where the ChannelExtraction process is executed. The astrometric solution is copied to "
      "each extracted channel image and regenerated.</p>" );
   InheritAstrometricSolution_CheckBox.OnClick( (pcl::Button::click_event_handler)&ChannelExtractionInterface::e_Channel_Click, w );

   InheritAstrometricSolution_Sizer.Add( InheritAstrometricSolution_CheckBox );
   InheritAstrometricSolution_Sizer.AddStretch();

   //

   Channels_Sizer.SetMargin( 6 );
   Channels_Sizer.SetSpacing( 4 );
   Channels_Sizer.Add( C0_Sizer );
   Channels_Sizer.Add( C1_Sizer );
   Channels_Sizer.Add( C2_Sizer );
   Channels_Sizer.Add( SampleFormat_Sizer );
   Channels_Sizer.AddSpacing( 4 );
   Channels_Sizer.Add( InheritAstrometricSolution_Sizer );

   Channels_GroupBox.SetTitle( "Channels / Target Images" );
   Channels_GroupBox.SetSizer( Channels_Sizer );
   Channels_GroupBox.AdjustToContents();

   //

   int minH = Min( ColorSpace_GroupBox.Height(), Channels_GroupBox.Height() );
   ColorSpace_GroupBox.SetMinHeight( minH );
   Channels_GroupBox.SetMinHeight( minH );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( ColorSpace_GroupBox );
   Global_Sizer.Add( Channels_GroupBox );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelExtractionInterface.cpp - Released 2024-12-28T16:54:15Z
