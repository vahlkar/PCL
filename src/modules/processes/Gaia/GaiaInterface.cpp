//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Gaia Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// GaiaInterface.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Gaia PixInsight module.
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

#include "GaiaInterface.h"
#include "GaiaProcess.h"
#include "GaiaParameters.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/OnlineObjectSearchDialog.h>
#include <pcl/TreeBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class GaiaFlagsSelectionDialog : public Dialog
{
public:

   uint32 flags = 0;

   GaiaFlagsSelectionDialog( uint32 );

private:

   VerticalSizer  Global_Sizer;

   CheckBox       NoPM_CheckBox;
   CheckBox       NoGBPMag_CheckBox;
   CheckBox       NoGRPMag_CheckBox;

   CheckBox       GoldRA_CheckBox;
   CheckBox       GoldDec_CheckBox;
   CheckBox       GoldPMRA_CheckBox;
   CheckBox       GoldPMDec_CheckBox;
   CheckBox       GoldParx_CheckBox;

   CheckBox       SilverRA_CheckBox;
   CheckBox       SilverDec_CheckBox;
   CheckBox       SilverPMRA_CheckBox;
   CheckBox       SilverPMDec_CheckBox;
   CheckBox       SilverParx_CheckBox;

   CheckBox       BronzeRA_CheckBox;
   CheckBox       BronzeDec_CheckBox;
   CheckBox       BronzePMRA_CheckBox;
   CheckBox       BronzePMDec_CheckBox;
   CheckBox       BronzeParx_CheckBox;

   CheckBox       GoldGMag_CheckBox;
   CheckBox       GoldGBPMag_CheckBox;
   CheckBox       GoldGRPMag_CheckBox;

   CheckBox       SilverGMag_CheckBox;
   CheckBox       SilverGBPMag_CheckBox;
   CheckBox       SilverGRPMag_CheckBox;

   CheckBox       BronzeGMag_CheckBox;
   CheckBox       BronzeGBPMag_CheckBox;
   CheckBox       BronzeGRPMag_CheckBox;

   HorizontalSizer   Buttons_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   void e_Click( Button& sender, bool checked );
   void e_Return( Dialog& sender, int retVal );
};

GaiaFlagsSelectionDialog::GaiaFlagsSelectionDialog( uint32 a_flags )
   : flags( a_flags )
{
   NoPM_CheckBox.SetText( "No proper motions" );
   NoPM_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No proper motions and parallax available.</p>" );
   NoPM_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   NoGBPMag_CheckBox.SetText( "No GBP magnitude" );
   NoGBPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No G-BP magnitude available.</p>" );
   NoGBPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   NoGRPMag_CheckBox.SetText( "No GRP magnitude" );
   NoGRPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">No G-RP magnitude available.</p>" );
   NoGRPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   //

   GoldRA_CheckBox.SetText( "Gold right ascension" );
   GoldRA_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of right ascension &lt; 0.13 mas.</p>" );
   GoldRA_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   GoldDec_CheckBox.SetText( "Gold declination" );
   GoldDec_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of declination &lt; 0.12 mas.</p>" );
   GoldDec_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   GoldPMRA_CheckBox.SetText( "Gold proper motion in R.A." );
   GoldPMRA_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of proper motion in right ascension &lt; 0.14 mas/year.</p>" );
   GoldPMRA_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   GoldPMDec_CheckBox.SetText( "Gold proper motion in declination" );
   GoldPMDec_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of proper motion in declination &lt; 0.12 mas/year.</p>" );
   GoldPMDec_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   GoldParx_CheckBox.SetText( "Gold parallax" );
   GoldParx_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of parallax &lt; 0.13 mas.</p>" );
   GoldParx_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   //

   SilverRA_CheckBox.SetText( "Silver right ascension" );
   SilverRA_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of right ascension in the range [0.13,1.43) mas.</p>" );
   SilverRA_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   SilverDec_CheckBox.SetText( "Silver declination" );
   SilverDec_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of declination in the range [0.12,1.28) mas.</p>" );
   SilverDec_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   SilverPMRA_CheckBox.SetText( "Silver proper motion in R.A." );
   SilverPMRA_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of proper motion in right ascension in the range [0.14,0.97) mas/year.</p>" );
   SilverPMRA_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   SilverPMDec_CheckBox.SetText( "Silver proper motion in declination" );
   SilverPMDec_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of proper motion in declination in the range [0.12,0.85) mas/year.</p>" );
   SilverPMDec_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   SilverParx_CheckBox.SetText( "Silver parallax" );
   SilverParx_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of parallax in the range [0.13,0.86) mas.</p>" );
   SilverParx_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   //

   BronzeRA_CheckBox.SetText( "Bronze right ascension" );
   BronzeRA_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of right ascension in the range [1.43,2.49) mas.</p>" );
   BronzeRA_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   BronzeDec_CheckBox.SetText( "Bronze declination" );
   BronzeDec_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of declination in the range [1.28,2.22) mas.</p>" );
   BronzeDec_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   BronzePMRA_CheckBox.SetText( "Bronze proper motion in R.A." );
   BronzePMRA_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of proper motion in right ascension in the range [0.97,1.58) mas/year.</p>" );
   BronzePMRA_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   BronzePMDec_CheckBox.SetText( "Bronze proper motion in declination" );
   BronzePMDec_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of proper motion in declination in the range [0.85,1.38) mas/year.</p>" );
   BronzePMDec_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   BronzeParx_CheckBox.SetText( "Bronze parallax" );
   BronzeParx_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Standard error of parallax in the range [0.86,1.38) mas.</p>" );
   BronzeParx_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   //

   GoldGMag_CheckBox.SetText( "Gold G magnitude" );
   GoldGMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on G-band mean flux &lt; 0.84 e-/s.</p>" );
   GoldGMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   GoldGBPMag_CheckBox.SetText( "Gold GBP magnitude" );
   GoldGBPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on the integrated BP mean flux &lt; 4.94 e-/s.</p>" );
   GoldGBPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   GoldGRPMag_CheckBox.SetText( "Gold GRP magnitude" );
   GoldGRPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on the integrated RP mean flux &lt; 5.89 e-/s.</p>" );
   GoldGRPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   //

   SilverGMag_CheckBox.SetText( "Silver G magnitude" );
   SilverGMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on G-band mean flux in the range [0.84,2.13) e-/s.</p>" );
   SilverGMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   SilverGBPMag_CheckBox.SetText( "Silver GBP magnitude" );
   SilverGBPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on the integrated BP mean flux in the range [4.94,12.61) e-/s.</p>" );
   SilverGBPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   SilverGRPMag_CheckBox.SetText( "Silver GRP magnitude" );
   SilverGRPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on the integrated RP mean flux in the range [5.89,15.40) e-/s.</p>" );
   SilverGRPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   //

   BronzeGMag_CheckBox.SetText( "Bronze G magnitude" );
   BronzeGMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on G-band mean flux in the range [2.13,3.08) e-/s.</p>" );
   BronzeGMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   BronzeGBPMag_CheckBox.SetText( "Bronze GBP magnitude" );
   BronzeGBPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on the integrated BP mean flux in the range [12.61,18.04) e-/s.</p>" );
   BronzeGBPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   BronzeGRPMag_CheckBox.SetText( "Bronze GRP magnitude" );
   BronzeGRPMag_CheckBox.SetToolTip( "<p style=\"white-space: nowrap;\">Error on the integrated RP mean flux in the range [15.40,22.35) e-/s.</p>" );
   BronzeGRPMag_CheckBox.OnClick( (Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   //

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&GaiaFlagsSelectionDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( NoPM_CheckBox );
   Global_Sizer.Add( NoGBPMag_CheckBox );
   Global_Sizer.Add( NoGRPMag_CheckBox );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( GoldRA_CheckBox );
   Global_Sizer.Add( GoldDec_CheckBox );
   Global_Sizer.Add( GoldPMRA_CheckBox );
   Global_Sizer.Add( GoldPMDec_CheckBox );
   Global_Sizer.Add( GoldParx_CheckBox );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( SilverRA_CheckBox );
   Global_Sizer.Add( SilverDec_CheckBox );
   Global_Sizer.Add( SilverPMRA_CheckBox );
   Global_Sizer.Add( SilverPMDec_CheckBox );
   Global_Sizer.Add( SilverParx_CheckBox );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( BronzeRA_CheckBox );
   Global_Sizer.Add( BronzeDec_CheckBox );
   Global_Sizer.Add( BronzePMRA_CheckBox );
   Global_Sizer.Add( BronzePMDec_CheckBox );
   Global_Sizer.Add( BronzeParx_CheckBox );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( GoldGMag_CheckBox );
   Global_Sizer.Add( GoldGBPMag_CheckBox );
   Global_Sizer.Add( GoldGRPMag_CheckBox );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( SilverGMag_CheckBox );
   Global_Sizer.Add( SilverGBPMag_CheckBox );
   Global_Sizer.Add( SilverGRPMag_CheckBox );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( BronzeGMag_CheckBox );
   Global_Sizer.Add( BronzeGBPMag_CheckBox );
   Global_Sizer.Add( BronzeGRPMag_CheckBox );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "Gaia Source Flags" );

   NoPM_CheckBox.SetChecked(         (flags & GaiaStarFlag::NoPM         ) != 0 );
   NoGBPMag_CheckBox.SetChecked(     (flags & GaiaStarFlag::NoGBPMag     ) != 0 );
   NoGRPMag_CheckBox.SetChecked(     (flags & GaiaStarFlag::NoGRPMag     ) != 0 );

   GoldRA_CheckBox.SetChecked(       (flags & GaiaStarFlag::GoldRA       ) != 0 );
   GoldDec_CheckBox.SetChecked(      (flags & GaiaStarFlag::GoldDec      ) != 0 );
   GoldPMRA_CheckBox.SetChecked(     (flags & GaiaStarFlag::GoldPMRA     ) != 0 );
   GoldPMDec_CheckBox.SetChecked(    (flags & GaiaStarFlag::GoldPMDec    ) != 0 );
   GoldParx_CheckBox.SetChecked(     (flags & GaiaStarFlag::GoldParx     ) != 0 );

   SilverRA_CheckBox.SetChecked(     (flags & GaiaStarFlag::SilverRA     ) != 0 );
   SilverDec_CheckBox.SetChecked(    (flags & GaiaStarFlag::SilverDec    ) != 0 );
   SilverPMRA_CheckBox.SetChecked(   (flags & GaiaStarFlag::SilverPMRA   ) != 0 );
   SilverPMDec_CheckBox.SetChecked(  (flags & GaiaStarFlag::SilverPMDec  ) != 0 );
   SilverParx_CheckBox.SetChecked(   (flags & GaiaStarFlag::SilverParx   ) != 0 );

   BronzeRA_CheckBox.SetChecked(     (flags & GaiaStarFlag::BronzeRA     ) != 0 );
   BronzeDec_CheckBox.SetChecked(    (flags & GaiaStarFlag::BronzeDec    ) != 0 );
   BronzePMRA_CheckBox.SetChecked(   (flags & GaiaStarFlag::BronzePMRA   ) != 0 );
   BronzePMDec_CheckBox.SetChecked(  (flags & GaiaStarFlag::BronzePMDec  ) != 0 );
   BronzeParx_CheckBox.SetChecked(   (flags & GaiaStarFlag::BronzeParx   ) != 0 );

   GoldGMag_CheckBox.SetChecked(     (flags & GaiaStarFlag::GoldGMag     ) != 0 );
   GoldGBPMag_CheckBox.SetChecked(   (flags & GaiaStarFlag::GoldGBPMag   ) != 0 );
   GoldGRPMag_CheckBox.SetChecked(   (flags & GaiaStarFlag::GoldGRPMag   ) != 0 );

   SilverGMag_CheckBox.SetChecked(   (flags & GaiaStarFlag::SilverGMag   ) != 0 );
   SilverGBPMag_CheckBox.SetChecked( (flags & GaiaStarFlag::SilverGBPMag ) != 0 );
   SilverGRPMag_CheckBox.SetChecked( (flags & GaiaStarFlag::SilverGRPMag ) != 0 );

   BronzeGMag_CheckBox.SetChecked(   (flags & GaiaStarFlag::BronzeGMag   ) != 0 );
   BronzeGBPMag_CheckBox.SetChecked( (flags & GaiaStarFlag::BronzeGBPMag ) != 0 );
   BronzeGRPMag_CheckBox.SetChecked( (flags & GaiaStarFlag::BronzeGRPMag ) != 0 );
}

void GaiaFlagsSelectionDialog::e_Click( Button& sender, bool checked )
{
        if ( sender == NoPM_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::NoPM;
      else
         flags &= ~GaiaStarFlag::NoPM;
   }
   else if ( sender == NoGBPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::NoGBPMag;
      else
         flags &= ~GaiaStarFlag::NoGBPMag;
   }
   else if ( sender == NoGRPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::NoGRPMag;
      else
         flags &= ~GaiaStarFlag::NoGRPMag;
   }

   else if ( sender == GoldRA_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::GoldRA;
      else
         flags &= ~GaiaStarFlag::GoldRA;
   }
   else if ( sender == GoldDec_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::GoldDec;
      else
         flags &= ~GaiaStarFlag::GoldDec;
   }
   else if ( sender == GoldPMRA_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::GoldPMRA;
      else
         flags &= ~GaiaStarFlag::GoldPMRA;
   }
   else if ( sender == GoldPMDec_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::GoldPMDec;
      else
         flags &= ~GaiaStarFlag::GoldPMDec;
   }
   else if ( sender == GoldParx_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::GoldParx;
      else
         flags &= ~GaiaStarFlag::GoldParx;
   }

   else if ( sender == SilverRA_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::SilverRA;
      else
         flags &= ~GaiaStarFlag::SilverRA;
   }
   else if ( sender == SilverDec_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::SilverDec;
      else
         flags &= ~GaiaStarFlag::SilverDec;
   }
   else if ( sender == SilverPMRA_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::SilverPMRA;
      else
         flags &= ~GaiaStarFlag::SilverPMRA;
   }
   else if ( sender == SilverPMDec_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::SilverPMDec;
      else
         flags &= ~GaiaStarFlag::SilverPMDec;
   }
   else if ( sender == SilverParx_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::SilverParx;
      else
         flags &= ~GaiaStarFlag::SilverParx;
   }

   else if ( sender == BronzeRA_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::BronzeRA;
      else
         flags &= ~GaiaStarFlag::BronzeRA;
   }
   else if ( sender == BronzeDec_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::BronzeDec;
      else
         flags &= ~GaiaStarFlag::BronzeDec;
   }
   else if ( sender == BronzePMRA_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::BronzePMRA;
      else
         flags &= ~GaiaStarFlag::BronzePMRA;
   }
   else if ( sender == BronzePMDec_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::BronzePMDec;
      else
         flags &= ~GaiaStarFlag::BronzePMDec;
   }
   else if ( sender == BronzeParx_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::BronzeParx;
      else
         flags &= ~GaiaStarFlag::BronzeParx;
   }

   else if ( sender == GoldGMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::GoldGMag;
      else
         flags &= ~GaiaStarFlag::GoldGMag;
   }
   else if ( sender == GoldGBPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::GoldGBPMag;
      else
         flags &= ~GaiaStarFlag::GoldGBPMag;
   }
   else if ( sender == GoldGRPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::GoldGRPMag;
      else
         flags &= ~GaiaStarFlag::GoldGRPMag;
   }

   else if ( sender == SilverGMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::SilverGMag;
      else
         flags &= ~GaiaStarFlag::SilverGMag;
   }
   else if ( sender == SilverGBPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::SilverGBPMag;
      else
         flags &= ~GaiaStarFlag::SilverGBPMag;
   }
   else if ( sender == SilverGRPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::SilverGRPMag;
      else
         flags &= ~GaiaStarFlag::SilverGRPMag;
   }

   else if ( sender == BronzeGMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::BronzeGMag;
      else
         flags &= ~GaiaStarFlag::BronzeGMag;
   }
   else if ( sender == BronzeGBPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::BronzeGBPMag;
      else
         flags &= ~GaiaStarFlag::BronzeGBPMag;
   }
   else if ( sender == BronzeGRPMag_CheckBox )
   {
      if ( checked )
         flags |= GaiaStarFlag::BronzeGRPMag;
      else
         flags &= ~GaiaStarFlag::BronzeGRPMag;
   }

   else if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class GaiaPreferencesDialog : public Dialog
{
public:

   GaiaPreferencesDialog();

private:

   pcl_enum   m_dataRelease = GDataRelease::EDR3;
   StringList m_databaseFilePaths;

   VerticalSizer     Global_Sizer;
      HorizontalSizer   DataRelease_Sizer;
         Label             DataRelease_Label;
         ComboBox          DataRelease_ComboBox;
      Label             Title_Label;
      TreeBox           FilePaths_TreeBox;
      HorizontalSizer   FileButtons_Sizer;
         PushButton        SelectFiles_PushButton;
         PushButton        RemoveFile_PushButton;
         PushButton        ClearFiles_PushButton;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void UpdateControls();

   void e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Click( Button& sender, bool checked );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );
   void e_Return( Dialog& sender, int retVal );
};

GaiaPreferencesDialog::GaiaPreferencesDialog()
{
   pcl::Font fnt = Font();

   const char* dataReleaseToolTip = "<p>You can choose between Gaia Data Release 2 (Gaia DR2) and Gaia Early Data Release 3 (EDR3). "
      "Both catalogs are available in XPSD format and are fully supported by this implementation.</p>";

   DataRelease_Label.SetText( "Data release:" );
   DataRelease_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DataRelease_Label.SetToolTip( dataReleaseToolTip );

   DataRelease_ComboBox.AddItem( "Gaia DR2" );
   DataRelease_ComboBox.AddItem( "Gaia EDR3" );
   DataRelease_ComboBox.SetToolTip( dataReleaseToolTip );
   DataRelease_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&GaiaPreferencesDialog::e_ItemSelected, *this );

   DataRelease_Sizer.SetSpacing( 4 );
   DataRelease_Sizer.Add( DataRelease_Label );
   DataRelease_Sizer.Add( DataRelease_ComboBox );
   DataRelease_Sizer.AddStretch();

   Title_Label.SetText( "Gaia Database Files" );
   Title_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   FilePaths_TreeBox.SetNumberOfColumns( 1 );
   FilePaths_TreeBox.HideHeader();
   FilePaths_TreeBox.DisableMultipleSelections();
   FilePaths_TreeBox.DisableRootDecoration();
   FilePaths_TreeBox.EnableAlternateRowColor();
   FilePaths_TreeBox.SetMinSize( 40*fnt.Width( "M" ), 24*fnt.Height() );
   FilePaths_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&GaiaPreferencesDialog::e_NodeActivated, *this );
   FilePaths_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&GaiaPreferencesDialog::e_FileDrag, *this );
   FilePaths_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&GaiaPreferencesDialog::e_FileDrop, *this );

   SelectFiles_PushButton.SetText( "Select" );
   SelectFiles_PushButton.SetToolTip( "<p>Select a new set of database files.</p>" );
   SelectFiles_PushButton.OnClick( (Button::click_event_handler)&GaiaPreferencesDialog::e_Click, *this );

   RemoveFile_PushButton.SetText( "Remove" );
   RemoveFile_PushButton.SetToolTip( "<p>Remove the selected database file from the list.</p>" );
   RemoveFile_PushButton.OnClick( (Button::click_event_handler)&GaiaPreferencesDialog::e_Click, *this );

   ClearFiles_PushButton.SetText( "Clear" );
   ClearFiles_PushButton.SetToolTip( "<p>Clear the list of database files.</p>" );
   ClearFiles_PushButton.OnClick( (Button::click_event_handler)&GaiaPreferencesDialog::e_Click, *this );

   FileButtons_Sizer.SetSpacing( 4 );
   FileButtons_Sizer.Add( SelectFiles_PushButton );
   FileButtons_Sizer.Add( RemoveFile_PushButton );
   FileButtons_Sizer.AddStretch();
   FileButtons_Sizer.Add( ClearFiles_PushButton );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&GaiaPreferencesDialog::e_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&GaiaPreferencesDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( DataRelease_Sizer );
   Global_Sizer.Add( Title_Label );
   Global_Sizer.Add( FilePaths_TreeBox );
   Global_Sizer.Add( FileButtons_Sizer );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetMinSize();

   SetWindowTitle( "Gaia Preferences" );

   OnReturn( (pcl::Dialog::return_event_handler)&GaiaPreferencesDialog::e_Return, *this );

   if ( TheGaiaInterface != nullptr )
      m_dataRelease = TheGaiaInterface->m_instance.OutputDataRelease();

   if ( !TheGaiaProcess->PreferencesLoaded( m_dataRelease ) )
      TheGaiaProcess->LoadPreferences( m_dataRelease );
   m_databaseFilePaths = TheGaiaProcess->DatabaseFilePaths( m_dataRelease );
   UpdateControls();
}

void GaiaPreferencesDialog::UpdateControls()
{
   DataRelease_ComboBox.SetCurrentItem( m_dataRelease );

   Title_Label.SetText( "Gaia " + GDataRelease::ReleaseName( m_dataRelease ) + " Database Files" );

   int currentIdx = FilePaths_TreeBox.ChildIndex( FilePaths_TreeBox.CurrentNode() );

   FilePaths_TreeBox.DisableUpdates();
   FilePaths_TreeBox.Clear();

   for ( size_type i = 0; i < m_databaseFilePaths.Length(); ++i )
      (new TreeBox::Node( FilePaths_TreeBox ))->SetText( 0, m_databaseFilePaths[i] );

   if ( currentIdx >= 0 && currentIdx < int( m_databaseFilePaths.Length() ) )
      FilePaths_TreeBox.SetCurrentNode( FilePaths_TreeBox[currentIdx] );

   FilePaths_TreeBox.EnableUpdates();

   RemoveFile_PushButton.Enable( FilePaths_TreeBox.NumberOfChildren() > 0 );
   ClearFiles_PushButton.Enable( FilePaths_TreeBox.NumberOfChildren() > 0 );
}

void GaiaPreferencesDialog::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   OpenFileDialog d;
   d.SetCaption( "Gaia: Select Database File" );
   d.DisableMultipleSelections();
   d.SetFilter( FileFilter( "XPSD Files", ".xpsd" ) );
   if ( d.Execute() )
      m_databaseFilePaths[sender.ChildIndex( &node )] = d.FileName();
}

void GaiaPreferencesDialog::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == DataRelease_ComboBox )
      if ( m_dataRelease != itemIndex )
      {
         m_dataRelease = itemIndex;
         if ( !TheGaiaProcess->PreferencesLoaded( m_dataRelease ) )
            TheGaiaProcess->LoadPreferences( m_dataRelease );
         m_databaseFilePaths = TheGaiaProcess->DatabaseFilePaths( m_dataRelease );
         UpdateControls();
      }
}

void GaiaPreferencesDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == SelectFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "Gaia: Select Database Files" );
      d.EnableMultipleSelections();
      d.SetFilter( FileFilter( "XPSD Files", ".xpsd" ) );
      if ( d.Execute() )
      {
         m_databaseFilePaths = d.FileNames();
         UpdateControls();
      }
   }
   else if ( sender == RemoveFile_PushButton )
   {
      const TreeBox::Node* n = FilePaths_TreeBox.CurrentNode();
      if ( n != nullptr )
      {
         m_databaseFilePaths.Remove( m_databaseFilePaths.At( FilePaths_TreeBox.ChildIndex( n ) ) );
         UpdateControls();
      }
   }
   else if ( sender == ClearFiles_PushButton )
   {
      m_databaseFilePaths.Clear();
      UpdateControls();
   }
   else if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

void GaiaPreferencesDialog::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == FilePaths_TreeBox.Viewport() )
   {
      for ( const String& file : files )
      {
         if ( !File::Exists( file ) )
            return;
         if ( File::ExtractExtension( file ).CaseFolded() != ".xpsd" )
            return;
      }
      wantsFiles = true;
   }
}

void GaiaPreferencesDialog::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == FilePaths_TreeBox.Viewport() )
   {
      StringList inputFiles;
      for ( const String& file : files )
         if ( File::Exists( file ) )
            if ( File::ExtractExtension( file ).CaseFolded() == ".xpsd" )
               inputFiles << file;
      if ( !inputFiles.IsEmpty() )
      {
         m_databaseFilePaths = inputFiles;
         UpdateControls();
      }
   }
}

void GaiaPreferencesDialog::e_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      TheGaiaProcess->SetDatabaseFilePaths( m_dataRelease, m_databaseFilePaths );
      if ( TheGaiaInterface != nullptr )
         TheGaiaInterface->UpdateControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

GaiaInterface* TheGaiaInterface = nullptr;

// ----------------------------------------------------------------------------

GaiaInterface::GaiaInterface()
   : m_instance( TheGaiaProcess )
{
   TheGaiaInterface = this;
}

// ----------------------------------------------------------------------------

GaiaInterface::~GaiaInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString GaiaInterface::Id() const
{
   return "Gaia";
}

// ----------------------------------------------------------------------------

MetaProcess* GaiaInterface::Process() const
{
   return TheGaiaProcess;
}

// ----------------------------------------------------------------------------

String GaiaInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/Gaia.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures GaiaInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal | InterfaceFeature::PreferencesButton;
}

// ----------------------------------------------------------------------------

void GaiaInterface::EditPreferences()
{
   GaiaPreferencesDialog().Execute();
}

// ----------------------------------------------------------------------------

void GaiaInterface::ApplyInstance() const
{
   m_instance.LaunchGlobal();
}

// ----------------------------------------------------------------------------

void GaiaInterface::ResetInstance()
{
   GaiaInstance defaultInstance( TheGaiaProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool GaiaInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Gaia" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheGaiaProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* GaiaInterface::NewProcess() const
{
   return new GaiaInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool GaiaInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const GaiaInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Gaia instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool GaiaInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool GaiaInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void GaiaInterface::UpdateControls()
{
   GUI->DataRelease_ComboBox.SetCurrentItem( m_instance.OutputDataRelease() );

   GUI->CenterRA_Edit.SetText( RAToString( m_instance.p_searchData.centerRA ) );

   GUI->CenterDec_Edit.SetText( DecToString( m_instance.p_searchData.centerDec ) );

   GUI->Radius_Edit.SetText( DistToString( m_instance.p_searchData.radius ) );

   GUI->ShowCompoundAngles_CheckBox.SetChecked( m_showCompoundAngles );

   GUI->RAInTimeUnits_CheckBox.SetChecked( m_raInTimeUnits );

   GUI->MagnitudeLow_NumericEdit.SetValue( m_instance.p_searchData.magnitudeLow );

   GUI->MagnitudeHigh_NumericEdit.SetValue( m_instance.p_searchData.magnitudeHigh );

   GUI->RequiredFlags_Edit.SetText( String().Format( "%08x", m_instance.p_searchData.requiredFlags ) );

   GUI->InclusionFlags_Edit.SetText( String().Format( "%08x", m_instance.p_searchData.inclusionFlags ) );

   GUI->ExclusionFlags_Edit.SetText( String().Format( "%08x", m_instance.p_searchData.exclusionFlags ) );

   GUI->SourceLimit_NumericEdit.SetValue( m_instance.p_searchData.sourceLimit );

   GUI->SortBy_ComboBox.SetCurrentItem( m_instance.p_sortBy );

   GUI->GenerateTextOutput_CheckBox.SetChecked( m_instance.p_generateTextOutput );

   GUI->TextFormat_Label.Enable( m_instance.p_generateTextOutput );

   GUI->TextFormat_ComboBox.SetCurrentItem( m_instance.p_textFormat );
   GUI->TextFormat_ComboBox.Enable( m_instance.p_generateTextOutput );

   GUI->TextHeaders_Label.Enable( m_instance.p_generateTextOutput );

   GUI->TextHeaders_ComboBox.SetCurrentItem( m_instance.p_textHeaders );
   GUI->TextHeaders_ComboBox.Enable( m_instance.p_generateTextOutput );

   GUI->OutputFilePath_Label.Enable( m_instance.p_generateTextOutput );

   GUI->OutputFilePath_Edit.SetText( m_instance.p_outputFilePath );
   GUI->OutputFilePath_Edit.Enable( m_instance.p_generateTextOutput );

   GUI->OutputFilePath_ToolButton.Enable( m_instance.p_generateTextOutput );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

String GaiaInterface::RAToString( double ra ) const
{
   if ( m_raInTimeUnits )
      ra /= 15;

   if ( m_showCompoundAngles )
      return String::ToSexagesimal( ra, SexagesimalConversionOptions( 3/*items*/,
                                                                      4/*precision*/,
                                                                      false/*sign*/,
                                                                      0/*width*/,
                                                                      ' '/*separator*/ ) );
   return String().Format( "%.9f", ra );
}

String GaiaInterface::DecToString( double dec ) const
{
   if ( m_showCompoundAngles )
      return String::ToSexagesimal( dec, SexagesimalConversionOptions( 3/*items*/,
                                                                       3/*precision*/,
                                                                       true/*sign*/,
                                                                       0/*width*/,
                                                                       ' '/*separator*/ ) );
   return String().Format( "%+.9f", dec );
}

String GaiaInterface::DistToString( double dist ) const
{
   if ( m_showCompoundAngles )
      return String::ToSexagesimal( dist, SexagesimalConversionOptions( 3/*items*/,
                                                                        3/*precision*/,
                                                                        false/*sign*/,
                                                                        0/*width*/,
                                                                        ' '/*separator*/ ) );
   return String().Format( "%.9f", dist );
}

// ----------------------------------------------------------------------------

void GaiaInterface::e_EditCompleted( Edit& sender )
{
   String text = sender.Text().Trimmed();

   if ( sender == GUI->CenterRA_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            double deg = text.SexagesimalToDouble( ' '/*separator*/ );
            if ( m_raInTimeUnits )
               deg *= 15;
            while ( deg < 0 )
               deg += 360;
            while ( deg > 360 )
               deg -= 360;
            m_instance.p_searchData.centerRA = deg;
         }
         ERROR_HANDLER
      }
      sender.SetText( RAToString( m_instance.p_searchData.centerRA ) );
   }
   else if ( sender == GUI->CenterDec_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            m_instance.p_searchData.centerDec = Range( text.SexagesimalToDouble( ' '/*separator*/ ), -90.0, +90.0 );
         }
         ERROR_HANDLER
      }
      sender.SetText( DecToString( m_instance.p_searchData.centerDec ) );
   }
   else if ( sender == GUI->Radius_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            m_instance.p_searchData.radius = Range( text.SexagesimalToDouble( ' '/*separator*/ ), 0.0, 45.0 );
         }
         ERROR_HANDLER
      }
      sender.SetText( DistToString( m_instance.p_searchData.radius ) );
   }
   else if ( sender == GUI->RequiredFlags_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            m_instance.p_searchData.requiredFlags = text.ToUInt( 16 );
         }
         ERROR_HANDLER
      }
      UpdateControls();
   }
   else if ( sender == GUI->InclusionFlags_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            m_instance.p_searchData.inclusionFlags = text.ToUInt( 16 );
         }
         ERROR_HANDLER
      }
      UpdateControls();
   }
   else if ( sender == GUI->ExclusionFlags_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            m_instance.p_searchData.exclusionFlags = text.ToUInt( 16 );
         }
         ERROR_HANDLER
      }
      UpdateControls();
   }
   else if ( sender == GUI->OutputFilePath_Edit )
   {
      sender.SetText( m_instance.p_outputFilePath = text );
   }
}

// ----------------------------------------------------------------------------

void GaiaInterface::e_EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->MagnitudeLow_NumericEdit )
      m_instance.p_searchData.magnitudeLow = value;
   else if ( sender == GUI->MagnitudeHigh_NumericEdit )
      m_instance.p_searchData.magnitudeHigh = value;
   else if ( sender == GUI->SourceLimit_NumericEdit )
      m_instance.p_searchData.sourceLimit = uint32( value );
}

// ----------------------------------------------------------------------------

void GaiaInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->DataRelease_ComboBox )
      m_instance.p_dataRelease = itemIndex;
   else if ( sender == GUI->TextFormat_ComboBox )
      m_instance.p_textFormat = itemIndex;
   else if ( sender == GUI->TextHeaders_ComboBox )
      m_instance.p_textHeaders = itemIndex;
   else if ( sender == GUI->SortBy_ComboBox )
      m_instance.p_sortBy = itemIndex;
}

// ----------------------------------------------------------------------------

void GaiaInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->ShowCompoundAngles_CheckBox  )
   {
      m_showCompoundAngles = checked;
      UpdateControls();
   }
   else if ( sender == GUI->RAInTimeUnits_CheckBox  )
   {
      m_raInTimeUnits = checked;
      UpdateControls();
   }
   else if ( sender == GUI->SearchCoordinates_Button )
   {
      if ( m_searchDialog == nullptr )
      {
         m_searchDialog = new OnlineObjectSearchDialog;
         m_searchDialog->SetWindowTitle( "Online Coordinate Search" );
      }
      if ( m_searchDialog->Execute() )
         if ( m_searchDialog->IsValid() )
         {
            m_instance.p_searchData.centerRA = m_searchDialog->RA();
            m_instance.p_searchData.centerDec = m_searchDialog->Dec();
            UpdateControls();
         }
   }
   else if ( sender == GUI->SourceLimitReset_ToolButton )
   {
      m_instance.p_searchData.sourceLimit = uint32_max;
      UpdateControls();
   }
   else if ( sender == GUI->GenerateTextOutput_CheckBox )
   {
      m_instance.p_generateTextOutput = checked;
      UpdateControls();
   }
   else if ( sender == GUI->RequiredFlagsClear_ToolButton )
   {
      m_instance.p_searchData.requiredFlags = 0;
      UpdateControls();
   }
   else if ( sender == GUI->RequiredFlags_ToolButton )
   {
      GaiaFlagsSelectionDialog d( m_instance.p_searchData.requiredFlags );
      d.SetWindowTitle( "Required Flags" );
      if ( d.Execute() )
      {
         m_instance.p_searchData.requiredFlags = d.flags;
         UpdateControls();
      }
   }
   else if ( sender == GUI->InclusionFlagsClear_ToolButton )
   {
      m_instance.p_searchData.inclusionFlags = 0;
      UpdateControls();
   }
   else if ( sender == GUI->InclusionFlags_ToolButton )
   {
      GaiaFlagsSelectionDialog d( m_instance.p_searchData.inclusionFlags );
      d.SetWindowTitle( "Inclusion Flags" );
      if ( d.Execute() )
      {
         m_instance.p_searchData.inclusionFlags = d.flags;
         UpdateControls();
      }
   }
   else if ( sender == GUI->ExclusionFlagsClear_ToolButton )
   {
      m_instance.p_searchData.exclusionFlags = 0;
      UpdateControls();
   }
   else if ( sender == GUI->ExclusionFlags_ToolButton )
   {
      GaiaFlagsSelectionDialog d( m_instance.p_searchData.exclusionFlags );
      d.SetWindowTitle( "Exclusion Flags" );
      if ( d.Execute() )
      {
         m_instance.p_searchData.exclusionFlags = d.flags;
         UpdateControls();
      }
   }
   else if ( sender == GUI->OutputFilePath_ToolButton )
   {
      SaveFileDialog d;
      d.SetCaption( "Gaia: Output CSV File" );
      d.SetFilters( FileDialog::filter_list() << FileFilter( "Any Files", "*" )
                                              << FileFilter( "Plain Text Files", ".txt" )
                                              << FileFilter( "CSV Files", ".csv" ) );
      d.EnableOverwritePrompt();
      if ( d.Execute() )
      {
         m_instance.p_outputFilePath = d.FileName();
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

GaiaInterface::GUIData::GUIData( GaiaInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( "Center right ascension:" );
   int editWidth1 = fnt.Width( "999 99 99.999 mm" );
   int editWidth2 = fnt.Width( "99.999 mm" );
   int editWidth3 = fnt.Width( "DDDDDDDD mm" );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   SearchParameters_SectionBar.SetTitle( "Search Parameters" );
   SearchParameters_SectionBar.SetSection( SearchParameters_Control );
   SearchParameters_SectionBar.SetToolTip( "<p>Parameters of the Gaia  database search operation.</p>" );

   //

   const char* dataReleaseToolTip = "<p>You can choose between Gaia Data Release 2 (Gaia DR2) and Gaia Early Data Release 3 (EDR3). "
      "Both catalogs are available in XPSD format and are fully supported by this implementation.</p>";

   DataRelease_Label.SetText( "Data release:" );
   DataRelease_Label.SetFixedWidth( labelWidth1 );
   DataRelease_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DataRelease_Label.SetToolTip( dataReleaseToolTip );

   DataRelease_ComboBox.AddItem( "Gaia DR2" );
   DataRelease_ComboBox.AddItem( "Gaia EDR3" );
   DataRelease_ComboBox.SetToolTip( dataReleaseToolTip );
   DataRelease_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&GaiaInterface::e_ItemSelected, w );

   DataRelease_Sizer.SetSpacing( 4 );
   DataRelease_Sizer.Add( DataRelease_Label );
   DataRelease_Sizer.Add( DataRelease_ComboBox );
   DataRelease_Sizer.AddStretch();

   //

   const char* raToolTip = "<p>Right ascension equatorial coordinate of the center of the search region.</p>"
      "<p>You can write an explicit right ascension coordinate as a space-separated list of hours (or degrees, if <i>R.A. "
      "in time units</i> is unchecked), (arc)minutes and (arc)seconds. The (arc)minutes and (arc)seconds items are optional, "
      "and you can specify a fractional part for the last item. Alternatively, you can search for object coordinates online "
      "by clicking the <i>Search Coordinates</i> button.</p>";

   CenterRA_Label.SetText( "Center right ascension:" );
   CenterRA_Label.SetFixedWidth( labelWidth1 );
   CenterRA_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CenterRA_Label.SetToolTip( raToolTip );

   CenterRA_Edit.SetToolTip( raToolTip );
   CenterRA_Edit.SetMinWidth( editWidth1 );
   CenterRA_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaInterface::e_EditCompleted, w );

   CenterRA_Sizer.SetSpacing( 4 );
   CenterRA_Sizer.Add( CenterRA_Label );
   CenterRA_Sizer.Add( CenterRA_Edit, 100 );
   CenterRA_Sizer.AddStretch();

   //

   const char* decToolTip = "<p>Declination equatorial coordinate of the center of the search region.</p>"
      "<p>You can write an explicit declination coordinate as a space-separated list of degrees, arcminutes and arcseconds. "
      "The arcminutes and arcseconds items are optional, and you can specify a fractional part for the last item. A plus or minus "
      "sign character is optional before the first degrees item, and obviously necessary for Southern declinations. "
      "Alternatively, you can search for object coordinates online by clicking the <i>Search Coordinates</i> button.</p>";

   CenterDec_Label.SetText( "Center declination:" );
   CenterDec_Label.SetFixedWidth( labelWidth1 );
   CenterDec_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CenterDec_Label.SetToolTip( decToolTip );

   CenterDec_Edit.SetToolTip( decToolTip );
   CenterDec_Edit.SetMinWidth( editWidth1 );
   CenterDec_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaInterface::e_EditCompleted, w );

   CenterDec_Sizer.SetSpacing( 4 );
   CenterDec_Sizer.Add( CenterDec_Label );
   CenterDec_Sizer.Add( CenterDec_Edit, 100 );
   CenterDec_Sizer.AddStretch();

   //

   ShowCompoundAngles_CheckBox.SetText( "Show compound angles" );
   ShowCompoundAngles_CheckBox.SetToolTip( "<p>If enabled, coordinates and angular distances will be shown as compound angular "
   "quantities (degrees/hours, (arc)minutes, and (arc)seconds). If disabled, coordinates will be shown as scalars (degrees or "
   "hours with decimals).</p>" );
   ShowCompoundAngles_CheckBox.OnClick( (Button::click_event_handler)&GaiaInterface::e_Click, w );

   ShowCompoundAngles_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowCompoundAngles_Sizer.Add( ShowCompoundAngles_CheckBox );
   ShowCompoundAngles_Sizer.AddStretch();

   //

   RAInTimeUnits_CheckBox.SetText( "R.A. in time units" );
   RAInTimeUnits_CheckBox.SetToolTip( "<p>When enabled, the right ascension coordinate will be shown expressed in hours, "
      "minutes, and seconds. When disabled, the right ascension will be expressed in degrees, arcminutes and arcseconds.</p>" );
   RAInTimeUnits_CheckBox.OnClick( (Button::click_event_handler)&GaiaInterface::e_Click, w );

   RAInTimeUnits_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   RAInTimeUnits_Sizer.Add( RAInTimeUnits_CheckBox );
   RAInTimeUnits_Sizer.AddStretch();

   //

   const char* radiusToolTip = "<p>Search radius in degrees. Only sources at angular distances smaller than or equal "
      "to this value will be included in the search result.</p>"
      "<p>You can write an explicit angular distance as a space-separated list of degrees, arcminutes and arcseconds. "
      "The arcminutes and arcseconds items are optional, and you can specify a fractional part for the last item.</p>";

   Radius_Label.SetText( "Search radius:" );
   Radius_Label.SetFixedWidth( labelWidth1 );
   Radius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Radius_Label.SetToolTip( radiusToolTip );

   Radius_Edit.SetToolTip( radiusToolTip );
   Radius_Edit.SetMinWidth( editWidth1 );
   Radius_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaInterface::e_EditCompleted, w );

   Radius_Sizer.SetSpacing( 4 );
   Radius_Sizer.Add( Radius_Label );
   Radius_Sizer.Add( Radius_Edit, 100 );
   Radius_Sizer.AddStretch();

   //

   MagnitudeLow_NumericEdit.SetReal();
   MagnitudeLow_NumericEdit.SetPrecision( 3 );
   MagnitudeLow_NumericEdit.EnableFixedPrecision();
   MagnitudeLow_NumericEdit.SetRange( -1.5, 26 );
   MagnitudeLow_NumericEdit.label.SetText( "Low magnitude limit:" );
   MagnitudeLow_NumericEdit.label.SetFixedWidth( labelWidth1 );
   MagnitudeLow_NumericEdit.edit.SetFixedWidth( editWidth2 );
   MagnitudeLow_NumericEdit.sizer.AddStretch();
   MagnitudeLow_NumericEdit.SetToolTip( "<p>Lower bound of the search magnitude range. Only sources with G magnitudes "
      "greater than this value will be included in the search result.</p>" );
   MagnitudeLow_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&GaiaInterface::e_EditValueUpdated, w );

   //

   MagnitudeHigh_NumericEdit.SetReal();
   MagnitudeHigh_NumericEdit.SetPrecision( 3 );
   MagnitudeHigh_NumericEdit.EnableFixedPrecision();
   MagnitudeHigh_NumericEdit.SetRange( -1.5, 26 );
   MagnitudeHigh_NumericEdit.label.SetText( "High magnitude limit:" );
   MagnitudeHigh_NumericEdit.label.SetFixedWidth( labelWidth1 );
   MagnitudeHigh_NumericEdit.edit.SetFixedWidth( editWidth2 );
   MagnitudeHigh_NumericEdit.sizer.AddStretch();
   MagnitudeHigh_NumericEdit.SetToolTip( "<p>Upper bound of the search magnitude range. Only sources with G magnitudes "
      "smaller than or equal to this value will be included in the search result.</p>" );
   MagnitudeHigh_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&GaiaInterface::e_EditValueUpdated, w );

   //

   const char* requiredFlagsToolTip = "<p>If non-zero, only sources with <i>all</i> of these flags set will be included "
      "in the search result.</p>";

   RequiredFlags_Label.SetText( "Required flags:" );
   RequiredFlags_Label.SetFixedWidth( labelWidth1 );
   RequiredFlags_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RequiredFlags_Label.SetToolTip( requiredFlagsToolTip );

   RequiredFlags_Edit.SetToolTip( requiredFlagsToolTip );
   RequiredFlags_Edit.SetMinWidth( editWidth3 );
   RequiredFlags_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaInterface::e_EditCompleted, w );

   RequiredFlagsClear_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   RequiredFlagsClear_ToolButton.SetScaledFixedSize( 20, 20 );
   RequiredFlagsClear_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RequiredFlagsClear_ToolButton.SetToolTip( "<p>Clear required flags.</p>" );
   RequiredFlagsClear_ToolButton.OnClick( (ToolButton::click_event_handler)&GaiaInterface::e_Click, w );

   RequiredFlags_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/control-multiple-selection.png" ) ) );
   RequiredFlags_ToolButton.SetScaledFixedSize( 20, 20 );
   RequiredFlags_ToolButton.SetToolTip( "<p>Define required flags.</p>" );
   RequiredFlags_ToolButton.OnClick( (Button::click_event_handler)&GaiaInterface::e_Click, w );

   RequiredFlags_Sizer.SetSpacing( 4 );
   RequiredFlags_Sizer.Add( RequiredFlags_Label );
   RequiredFlags_Sizer.Add( RequiredFlags_Edit, 100 );
   RequiredFlags_Sizer.Add( RequiredFlagsClear_ToolButton );
   RequiredFlags_Sizer.Add( RequiredFlags_ToolButton );
   RequiredFlags_Sizer.AddStretch();

   //

   const char* inclusionFlagsToolTip = "<p>If non-zero, sources with <i>any</i> of these flags set will be included "
      "in the search result.</p>";

   InclusionFlags_Label.SetText( "Inclusion flags:" );
   InclusionFlags_Label.SetFixedWidth( labelWidth1 );
   InclusionFlags_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InclusionFlags_Label.SetToolTip( inclusionFlagsToolTip );

   InclusionFlags_Edit.SetToolTip( inclusionFlagsToolTip );
   InclusionFlags_Edit.SetMinWidth( editWidth3 );
   InclusionFlags_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaInterface::e_EditCompleted, w );

   InclusionFlagsClear_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   InclusionFlagsClear_ToolButton.SetScaledFixedSize( 20, 20 );
   InclusionFlagsClear_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   InclusionFlagsClear_ToolButton.SetToolTip( "<p>Clear inclusion flags.</p>" );
   InclusionFlagsClear_ToolButton.OnClick( (ToolButton::click_event_handler)&GaiaInterface::e_Click, w );

   InclusionFlags_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/control-multiple-selection.png" ) ) );
   InclusionFlags_ToolButton.SetScaledFixedSize( 20, 20 );
   InclusionFlags_ToolButton.SetToolTip( "<p>Define inclusion flags.</p>" );
   InclusionFlags_ToolButton.OnClick( (Button::click_event_handler)&GaiaInterface::e_Click, w );

   InclusionFlags_Sizer.SetSpacing( 4 );
   InclusionFlags_Sizer.Add( InclusionFlags_Label );
   InclusionFlags_Sizer.Add( InclusionFlags_Edit, 100 );
   InclusionFlags_Sizer.Add( InclusionFlagsClear_ToolButton );
   InclusionFlags_Sizer.Add( InclusionFlags_ToolButton );
   InclusionFlags_Sizer.AddStretch();

   //

   const char* exclusionFlagsToolTip = "<p>If non-zero, sources with <i>any</i> of these flags set will <i>not</i> be included "
      "in the search result.</p>";

   ExclusionFlags_Label.SetText( "Exclusion flags:" );
   ExclusionFlags_Label.SetFixedWidth( labelWidth1 );
   ExclusionFlags_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ExclusionFlags_Label.SetToolTip( exclusionFlagsToolTip );

   ExclusionFlags_Edit.SetToolTip( exclusionFlagsToolTip );
   ExclusionFlags_Edit.SetMinWidth( editWidth3 );
   ExclusionFlags_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaInterface::e_EditCompleted, w );

   ExclusionFlagsClear_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   ExclusionFlagsClear_ToolButton.SetScaledFixedSize( 20, 20 );
   ExclusionFlagsClear_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExclusionFlagsClear_ToolButton.SetToolTip( "<p>Clear exclusion flags.</p>" );
   ExclusionFlagsClear_ToolButton.OnClick( (ToolButton::click_event_handler)&GaiaInterface::e_Click, w );

   ExclusionFlags_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/control-multiple-selection.png" ) ) );
   ExclusionFlags_ToolButton.SetScaledFixedSize( 20, 20 );
   ExclusionFlags_ToolButton.SetToolTip( "<p>Define exclusion flags.</p>" );
   ExclusionFlags_ToolButton.OnClick( (Button::click_event_handler)&GaiaInterface::e_Click, w );

   ExclusionFlags_Sizer.SetSpacing( 4 );
   ExclusionFlags_Sizer.Add( ExclusionFlags_Label );
   ExclusionFlags_Sizer.Add( ExclusionFlags_Edit, 100 );
   ExclusionFlags_Sizer.Add( ExclusionFlagsClear_ToolButton );
   ExclusionFlags_Sizer.Add( ExclusionFlags_ToolButton );
   ExclusionFlags_Sizer.AddStretch();

   //

   SourceLimit_NumericEdit.SetInteger();
   SourceLimit_NumericEdit.SetRange( 0, uint32_max );
   SourceLimit_NumericEdit.label.SetText( "Source count limit:" );
   SourceLimit_NumericEdit.label.SetFixedWidth( labelWidth1 );
   SourceLimit_NumericEdit.edit.SetFixedWidth( editWidth3 );
   SourceLimit_NumericEdit.sizer.Add( SourceLimitReset_ToolButton );
   SourceLimit_NumericEdit.sizer.AddStretch();
   SourceLimit_NumericEdit.SetToolTip( "<p>The maximum number of sources that can be included in the search result.</p>" );
   SourceLimit_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&GaiaInterface::e_EditValueUpdated, w );

   SourceLimitReset_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   SourceLimitReset_ToolButton.SetScaledFixedSize( 20, 20 );
   SourceLimitReset_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   SourceLimitReset_ToolButton.SetToolTip( "<p>Reset source count limit to perform an unlimited search.</p>" );
   SourceLimitReset_ToolButton.OnClick( (ToolButton::click_event_handler)&GaiaInterface::e_Click, w );

   //

   SearchParametersLeft_Sizer.SetSpacing( 4 );
   SearchParametersLeft_Sizer.Add( CenterRA_Sizer );
   SearchParametersLeft_Sizer.Add( CenterDec_Sizer );
   SearchParametersLeft_Sizer.Add( Radius_Sizer );
   SearchParametersLeft_Sizer.Add( ShowCompoundAngles_Sizer );
   SearchParametersLeft_Sizer.Add( RAInTimeUnits_Sizer );
   SearchParametersLeft_Sizer.Add( MagnitudeLow_NumericEdit );
   SearchParametersLeft_Sizer.Add( MagnitudeHigh_NumericEdit );
   SearchParametersLeft_Sizer.Add( RequiredFlags_Sizer );
   SearchParametersLeft_Sizer.Add( InclusionFlags_Sizer );
   SearchParametersLeft_Sizer.Add( ExclusionFlags_Sizer );
   SearchParametersLeft_Sizer.Add( SourceLimit_NumericEdit );

   //

   SearchCoordinates_Button.SetText( "Search Coordinates" );
   SearchCoordinates_Button.SetIcon( w.ScaledResource( ":/icons/find.png" ) );
   SearchCoordinates_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   SearchCoordinates_Button.SetToolTip( "<p>Open the Online Coordinate Search dialog.</p>" );
   SearchCoordinates_Button.OnClick( (Button::click_event_handler)&GaiaInterface::e_Click, w );

   //

   SearchParametersRight_Sizer.Add( SearchCoordinates_Button );
   SearchParametersRight_Sizer.AddStretch();

   //

   SearchParametersBottom_Sizer.SetSpacing( 32 );
   SearchParametersBottom_Sizer.Add( SearchParametersLeft_Sizer, 100 );
   SearchParametersBottom_Sizer.Add( SearchParametersRight_Sizer );

   //

   SearchParameters_Sizer.SetSpacing( 4 );
   SearchParameters_Sizer.Add( DataRelease_Sizer );
   SearchParameters_Sizer.Add( SearchParametersBottom_Sizer );

   SearchParameters_Control.SetSizer( SearchParameters_Sizer );

   //

   OutputParameters_SectionBar.SetTitle( "Output Parameters" );
   OutputParameters_SectionBar.SetSection( OutputParameters_Control );

   //

   const char* sortByToolTip = "<p>This is the data item used to sort output point sources in ascensing order.</p>";

   SortBy_Label.SetText( "Sort by:" );
   SortBy_Label.SetFixedWidth( labelWidth1 );
   SortBy_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SortBy_Label.SetToolTip( sortByToolTip );

   SortBy_ComboBox.AddItem( "Don't sort" );
   SortBy_ComboBox.AddItem( "Right Ascension" );
   SortBy_ComboBox.AddItem( "Declination" );
   SortBy_ComboBox.AddItem( "G mean magnitude" );
   SortBy_ComboBox.AddItem( "G_BP mean magnitude" );
   SortBy_ComboBox.AddItem( "G_RP mean magnitude" );
   SortBy_ComboBox.AddItem( "Parallax" );
   SortBy_ComboBox.SetToolTip( sortByToolTip );
   SortBy_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&GaiaInterface::e_ItemSelected, w );

   SortBy_Sizer.SetSpacing( 4 );
   SortBy_Sizer.Add( SortBy_Label );
   SortBy_Sizer.Add( SortBy_ComboBox );
   SortBy_Sizer.AddStretch();

   //

   GenerateTextOutput_CheckBox.SetText( "Generate text output" );
   GenerateTextOutput_CheckBox.SetToolTip( "<p>When enabled, the search operation will generate an output "
      "plain text file including available data for all sources fulfilling the specified search criteria.</p>" );
   GenerateTextOutput_CheckBox.OnClick( (Button::click_event_handler)&GaiaInterface::e_Click, w );

   GenerateTextOutput_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateTextOutput_Sizer.Add( GenerateTextOutput_CheckBox );

   //

   const char* textFormatToolTip = "<p>The format of generated output text files.</p>"
      "<p>CSV will generate standard comma-separated files (CSV format).</p>"
      "<p>Tabular formats can be selected to write coordinates either as scalars in degrees, or using compound "
      "angular representations where right ascension is represented as separate hours, minutes and seconds, and "
      "declination as degrees, arcminutes and arcseconds.</p>"
      "<p>In all cases output text files can contain optional headers, which you can control with the "
      "<i>Text headers</i> option below.</p>";

   TextFormat_Label.SetText( "Text format:" );
   TextFormat_Label.SetFixedWidth( labelWidth1 );
   TextFormat_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   TextFormat_Label.SetToolTip( textFormatToolTip );

   TextFormat_ComboBox.AddItem( "CSV" );
   TextFormat_ComboBox.AddItem( "Tabular, scalar coordinates" );
   TextFormat_ComboBox.AddItem( "Tabular, compound coordinates" );
   TextFormat_ComboBox.SetToolTip( textFormatToolTip );
   TextFormat_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&GaiaInterface::e_ItemSelected, w );

   TextFormat_Sizer.SetSpacing( 4 );
   TextFormat_Sizer.Add( TextFormat_Label );
   TextFormat_Sizer.Add( TextFormat_ComboBox );
   TextFormat_Sizer.AddStretch();

   //

   const char* textHeadersToolTip = "<p></p>";

   TextHeaders_Label.SetText( "Text headers:" );
   TextHeaders_Label.SetFixedWidth( labelWidth1 );
   TextHeaders_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   TextHeaders_Label.SetToolTip( textHeadersToolTip );

   TextHeaders_ComboBox.AddItem( "None" );
   TextHeaders_ComboBox.AddItem( "Table columns" );
   TextHeaders_ComboBox.AddItem( "Search parameters" );
   TextHeaders_ComboBox.AddItem( "Search parameters and table columns" );
   TextHeaders_ComboBox.SetToolTip( textHeadersToolTip );
   TextHeaders_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&GaiaInterface::e_ItemSelected, w );

   TextHeaders_Sizer.SetSpacing( 4 );
   TextHeaders_Sizer.Add( TextHeaders_Label );
   TextHeaders_Sizer.Add( TextHeaders_ComboBox );
   TextHeaders_Sizer.AddStretch();

   //

   const char* outputFilePathToolTip = "<p>Output file path.</p>"
      "<p>When the <i>Generate text output</i> option is enabled, the search process will write its output "
      "data to the specified file in a plain text format.</p>";

   OutputFilePath_Label.SetText( "Output file:" );
   OutputFilePath_Label.SetFixedWidth( labelWidth1 );
   OutputFilePath_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputFilePath_Label.SetToolTip( outputFilePathToolTip );

   OutputFilePath_Edit.SetToolTip( outputFilePathToolTip );
   OutputFilePath_Edit.OnEditCompleted( (Edit::edit_event_handler)&GaiaInterface::e_EditCompleted, w );

   OutputFilePath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select-file.png" ) ) );
   OutputFilePath_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputFilePath_ToolButton.SetToolTip( "<p>Select the output file.</p>" );
   OutputFilePath_ToolButton.OnClick( (Button::click_event_handler)&GaiaInterface::e_Click, w );

   OutputFilePath_Sizer.SetSpacing( 4 );
   OutputFilePath_Sizer.Add( OutputFilePath_Label );
   OutputFilePath_Sizer.Add( OutputFilePath_Edit, 100 );
   OutputFilePath_Sizer.Add( OutputFilePath_ToolButton );

   //

   OutputParameters_Sizer.SetSpacing( 4 );
   OutputParameters_Sizer.Add( SortBy_Sizer );
   OutputParameters_Sizer.Add( GenerateTextOutput_Sizer );
   OutputParameters_Sizer.Add( TextFormat_Sizer );
   OutputParameters_Sizer.Add( TextHeaders_Sizer );
   OutputParameters_Sizer.Add( OutputFilePath_Sizer );

   OutputParameters_Control.SetSizer( OutputParameters_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( SearchParameters_SectionBar );
   Global_Sizer.Add( SearchParameters_Control );
   Global_Sizer.Add( OutputParameters_SectionBar );
   Global_Sizer.Add( OutputParameters_Control );

   //

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GaiaInterface.cpp - Released 2021-04-09T19:41:48Z
