//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// DeviceConfigBase.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2024 Klaus Kretzschmar
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

#include "DeviceConfigBase.h"
#include "INDIClient.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ConfigDialogBase::ConfigDialogBase( const String& deviceName )
   : m_device( deviceName )
{
   SaveConfig_Button.SetText( "Save" );
   SaveConfig_Button.SetToolTip( "<p>Stores the current configuration on the Indigo server.</p>" );
   SaveConfig_Button.SetIcon( ScaledResource( ":/icons/save.png" ) );
   SaveConfig_Button.OnClick( (Button::click_event_handler)&ConfigDialogBase::e_Click, *this );

   Ok_Button.SetText( "Ok" );
   Ok_Button.SetIcon( ScaledResource( ":/icons/ok.png" ) );
   Ok_Button.OnClick( (Button::click_event_handler)&ConfigDialogBase::e_Click, *this );

   Cancel_Button.SetText( "Cancel" );
   Cancel_Button.SetIcon( ScaledResource( ":/icons/cancel.png" ) );
   Cancel_Button.OnClick( (Button::click_event_handler)&ConfigDialogBase::e_Click, *this );

   ConfigButton_Sizer.SetSpacing( 8 );
   ConfigButton_Sizer.SetMargin( 8 );
   ConfigButton_Sizer.Add( SaveConfig_Button );
   ConfigButton_Sizer.AddStretch();
   ConfigButton_Sizer.Add( Ok_Button );
   ConfigButton_Sizer.Add( Cancel_Button );

   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.SetMargin( 8 );

   SetSizer( Global_Sizer );

   SetWindowTitle( "Configuration Dialog" );

   OnShow( (Control::event_handler)&ConfigDialogBase::e_Show, *this );
}

// ----------------------------------------------------------------------------

void ConfigDialogBase::AddBaseControls()
{
   Global_Sizer.Add( ConfigButton_Sizer );
}

// ----------------------------------------------------------------------------

void ConfigDialogBase::e_Show( Control& )
{
   if ( m_firstTimeShown )
   {
      m_firstTimeShown = false;
      AdjustToContents();
      SetFixedHeight();
      SetMinSize();
   }
}

// ----------------------------------------------------------------------------

void ConfigDialogBase::e_Click( Button& sender, bool checked )
{
   if ( sender == Ok_Button )
   {
      SendUpdatedProperties();
      Ok();
   }

   if ( sender == Cancel_Button )
   {
      Cancel();
   }

   if ( sender == SaveConfig_Button )
   {
      SendUpdatedProperties();
      INDIClient::TheClient()->SendNewPropertyItem( m_device, CONFIG_PROPERTY_NAME, "INDI_SWITCH", CONFIG_SAVE_ITEM_NAME, "ON" );
      Ok();
   }
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF DeviceConfigBase.cpp - Released 2024-12-28T16:54:15Z
