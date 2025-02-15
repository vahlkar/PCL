//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIMountInterface.cpp - Released 2024-12-28T16:54:15Z
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

#include <pcl/Console.h>
#include <pcl/ExternalProcess.h>
#include <pcl/FileDialog.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Graphics.h>
#include <pcl/Math.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/Position.h>

#include "Alignment.h"
#include "INDIDeviceControllerInterface.h"
#include "INDIMountInterface.h"
#include "INDIMountProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

INDIMountInterface* TheINDIMountInterface = nullptr;

// ----------------------------------------------------------------------------

void INDIMountInterfaceExecution::StartMountEvent( double targetRA, double currentRA, double targetDec, double currentDec, pcl_enum command )
   {
      m_iface->m_execution = this;
      m_command = command;
      m_iface->GUI->TargetRA_H_SpinBox.Disable();
      m_iface->GUI->TargetRA_M_SpinBox.Disable();
      m_iface->GUI->TargetDec_H_SpinBox.Disable();
      m_iface->GUI->TargetDec_M_SpinBox.Disable();
      m_iface->GUI->TargetRA_S_NumericEdit.Disable();
      m_iface->GUI->TargetDec_S_NumericEdit.Disable();
      m_iface->GUI->MountTargetDECIsSouth_CheckBox.Disable();
      m_iface->GUI->MountSearch_Button.Disable();
      m_iface->GUI->MountPlanets_Button.Disable();
      m_iface->GUI->MountAsteroids_Button.Disable();
      m_iface->GUI->SlewLeft_Control.Disable();
      m_iface->GUI->MountGoToStart_Button.Disable();
      m_iface->GUI->MountSync_Button.Disable();
      m_iface->GUI->MountPark_Button.Disable();
      m_iface->GUI->MountGoToCancel_Button.Enable();
      m_iface->GUI->MountGoToInfo_Label.Clear();
      m_iface->GUI->AlignmentFile_Label.Disable();
      m_iface->GUI->AlignmentFile_Edit.Disable();
      m_iface->GUI->AlignmentFile_ToolButton.Disable();
      m_iface->GUI->MountAligmentModelConfig_Button.Disable();
      m_iface->GUI->MountAligmentModelFit_Button.Disable();
      m_iface->GUI->MountAligmentModelConfig_Button.Disable();
      m_iface->GUI->MountAlignmentCorrection_CheckBox.Disable();
      Module->ProcessEvents();
   }

   void INDIMountInterfaceExecution::MountEvent( double targetRA, double currentRA, double targetDec, double currentDec )
   {
      if ( m_abortRequested )
         AbstractINDIMountExecution::Abort();

      switch ( m_command )
      {
      case IMCCommand::GoTo:
         m_iface->GUI->MountGoToInfo_Label.SetText(
            "Slewing: dRA = "
            + String::ToSexagesimal( targetRA - currentRA,
               SexagesimalConversionOptions( 3 /*items*/, 3 /*precision*/, true /*sign*/ ) )
            + ", dDec = "
            + String::ToSexagesimal( targetDec - currentDec,
               SexagesimalConversionOptions( 3 /*items*/, 2 /*precision*/, true /*sign*/ ) ) );
         break;

      case IMCCommand::Park:
      case IMCCommand::ParkDefault:
         m_iface->GUI->MountGoToInfo_Label.SetText( "Parking telescope..." );
         break;

      case IMCCommand::Sync:
         break;

      default:
         break;
      };

      Module->ProcessEvents();
   }

   void INDIMountInterfaceExecution::EndMountEvent()
   {
      m_iface->m_execution = nullptr;
      m_iface->GUI->TargetRA_H_SpinBox.Enable();
      m_iface->GUI->TargetRA_M_SpinBox.Enable();
      m_iface->GUI->TargetDec_H_SpinBox.Enable();
      m_iface->GUI->TargetDec_M_SpinBox.Enable();
      m_iface->GUI->TargetRA_S_NumericEdit.Enable();
      m_iface->GUI->TargetDec_S_NumericEdit.Enable();
      m_iface->GUI->MountTargetDECIsSouth_CheckBox.Enable();
      m_iface->GUI->MountSearch_Button.Enable();
      m_iface->GUI->MountPlanets_Button.Enable();
      m_iface->GUI->MountAsteroids_Button.Enable();
      m_iface->GUI->SlewLeft_Control.Enable();
      m_iface->GUI->MountGoToStart_Button.Enable();
      m_iface->GUI->MountSync_Button.Enable();
      m_iface->GUI->MountPark_Button.Enable();
      m_iface->GUI->MountGoToCancel_Button.Disable();
      m_iface->GUI->MountGoToInfo_Label.Clear();
      m_iface->GUI->AlignmentFile_Label.Enable();
      m_iface->GUI->AlignmentFile_Edit.Enable();
      m_iface->GUI->AlignmentFile_ToolButton.Enable();
      m_iface->GUI->MountAligmentModelConfig_Button.Enable();
      m_iface->GUI->MountAligmentModelFit_Button.Enable();
      m_iface->GUI->MountAligmentModelConfig_Button.Enable();
      m_iface->GUI->MountAlignmentCorrection_CheckBox.Enable();

      Module->ProcessEvents();
   }

   void INDIMountInterfaceExecution::WaitEvent()
   {
      Module->ProcessEvents();
   }

   void INDIMountInterfaceExecution::AbortEvent()
   {
      EndMountEvent();
   }

// ----------------------------------------------------------------------------

CoordinateSearchDialog::CoordinateSearchDialog( INDIMountInterface& parent )
   : m_parent( parent )
{
   const char* objectNameToolTip =
      "<p>Name or identifier of the object to search for. Examples: M31, Pleiades, NGC 253, Orion Nebula, Antares.</p>";

   ObjectName_Label.SetText( "Object: " );
   ObjectName_Label.SetToolTip( objectNameToolTip );
   ObjectName_Label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   ObjectName_Edit.SetToolTip( objectNameToolTip );
   ObjectName_Edit.OnGetFocus( (Control::event_handler)&CoordinateSearchDialog::e_GetFocus, *this );
   ObjectName_Edit.OnLoseFocus( (Control::event_handler)&CoordinateSearchDialog::e_LoseFocus, *this );

   Search_Button.SetText( "Search" );
   Search_Button.SetIcon( ScaledResource( ":/icons/find.png" ) );
   Search_Button.SetToolTip( "<p>Perform online coordinate search.</p>" );
   Search_Button.OnClick( (Button::click_event_handler)&CoordinateSearchDialog::e_Click, *this );

   Search_Sizer.SetSpacing( 4 );
   Search_Sizer.Add( ObjectName_Label );
   Search_Sizer.Add( ObjectName_Edit, 100 );
   Search_Sizer.Add( Search_Button );

   SearchInfo_TextBox.SetReadOnly();
   SearchInfo_TextBox.SetStyleSheet( ScaledStyleSheet(
      "* {"
      "font-family: Hack, DejaVu Sans Mono, Monospace;"
      "font-size: 8pt;"
      "background: #141414;" // borrowed from /rsc/qss/core-standard.qss
      "color: #E8E8E8;"
      "}" ) );
   SearchInfo_TextBox.Restyle();
   SearchInfo_TextBox.SetMinSize( SearchInfo_TextBox.Font().Width( 'm' ) * 81, SearchInfo_TextBox.Font().Height() * 22 );

   Get_Button.SetText( "Get" );
   Get_Button.SetIcon( ScaledResource( ":/icons/window-import.png" ) );
   Get_Button.SetToolTip( "<p>Acquire apparent coordinates.</p>" );
   Get_Button.OnClick( (Button::click_event_handler)&CoordinateSearchDialog::e_Click, *this );
   Get_Button.Disable();

   GoTo_Button.SetText( "GoTo" );
   GoTo_Button.SetIcon( ScaledResource( ":/icons/play.png" ) );
   GoTo_Button.SetToolTip( "<p>Slew to apparent coordinates.</p>" );
   GoTo_Button.OnClick( (Button::click_event_handler)&CoordinateSearchDialog::e_Click, *this );
   GoTo_Button.Disable();

   Cancel_Button.SetText( "Cancel" );
   Cancel_Button.SetIcon( ScaledResource( ":/icons/cancel.png" ) );
   Cancel_Button.OnClick( (Button::click_event_handler)&CoordinateSearchDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( Get_Button );
   Buttons_Sizer.Add( GoTo_Button );
   Buttons_Sizer.Add( Cancel_Button );

   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.SetMargin( 8 );
   Global_Sizer.Add( Search_Sizer );
   Global_Sizer.Add( SearchInfo_TextBox, 100 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   SetWindowTitle( "Online Coordinate Search" );

   OnShow( (Control::event_handler)&CoordinateSearchDialog::e_Show, *this );
}

// ----------------------------------------------------------------------------

void CoordinateSearchDialog::e_Show( Control& )
{
   if ( m_firstTimeShown )
   {
      m_firstTimeShown = false;
      AdjustToContents();
      SetMinSize();
   }
}

// ----------------------------------------------------------------------------

void CoordinateSearchDialog::e_GetFocus( Control& sender )
{
   if ( sender == ObjectName_Edit )
      Search_Button.SetDefault();
}

// ----------------------------------------------------------------------------

void CoordinateSearchDialog::e_LoseFocus( Control& sender )
{
   if ( sender == ObjectName_Edit )
      Get_Button.SetDefault();
}

// ----------------------------------------------------------------------------

bool CoordinateSearchDialog::e_Download( NetworkTransfer& sender, const void* buffer, fsize_type size )
{
   if ( m_abort )
      return false;

   m_downloadData.Append( static_cast<const char*>( buffer ), size );
   return true;
}

// ----------------------------------------------------------------------------

bool CoordinateSearchDialog::e_Progress( NetworkTransfer& sender,
   fsize_type downloadTotal, fsize_type downloadCurrent,
   fsize_type uploadTotal, fsize_type uploadCurrent )
{
   if ( m_abort )
      return false;

   if ( downloadTotal > 0 )
      SearchInfo_TextBox.Insert( String().Format( "<end><clrbol>%u of %u bytes transferred (%d%%)",
                                                  downloadCurrent, downloadTotal,
                                                  RoundInt( 100.0 * downloadCurrent / downloadTotal ) ) );
   else
      SearchInfo_TextBox.Insert( String().Format( "<end><clrbol>%u bytes transferred (unknown size)",
                                                  downloadCurrent ) );
   SearchInfo_TextBox.Focus();
   Module->ProcessEvents();
   return true;
}

// ----------------------------------------------------------------------------

void CoordinateSearchDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == Search_Button )
   {
      String objectName = ObjectName_Edit.Text().Trimmed();
      ObjectName_Edit.SetText( objectName );
      if ( objectName.IsEmpty() )
      {
         SearchInfo_TextBox.SetText( "\x1b[31m*** Error: No object has been specified.\x1b[39m<br>" );
         ObjectName_Edit.Focus();
         return;
      }

      m_valid = false;
      Get_Button.Disable();
      GoTo_Button.Disable();

      //String url( "http://vizier.cfa.harvard.edu/viz-bin/nph-sesame/-oI/A?" );
      //url << objectName;
      String url( "http://simbad.u-strasbg.fr/simbad/sim-tap/sync?request=doQuery&lang=adql&format=TSV&query=" );
      String select_stmt = "SELECT oid, ra, dec, pmra, pmdec, plx_value, rvz_radvel, main_id, otype_txt, sp_type, flux "
                           "FROM basic "
                           "JOIN ident ON ident.oidref = oid "
                           "LEFT OUTER JOIN flux ON flux.oidref = oid AND flux.filter = 'V' "
                           "WHERE id = '"
         + objectName + "';";
      url << select_stmt;

      NetworkTransfer transfer;
      transfer.SetURL( url );
      transfer.OnDownloadDataAvailable( (NetworkTransfer::download_event_handler)&CoordinateSearchDialog::e_Download, *this );
      transfer.OnTransferProgress( (NetworkTransfer::progress_event_handler)&CoordinateSearchDialog::e_Progress, *this );

      SearchInfo_TextBox.SetText( "<wrap><raw>" + url + "</raw><br><br><flush>" );
      Module->ProcessEvents();

      m_downloadData.Clear();
      m_downloading = true;
      m_abort = false;
      bool ok = transfer.Download();
      m_downloading = false;

      if ( ok )
      {
         SearchInfo_TextBox.Insert( String().Format( "<end><clrbol>%d bytes downloaded @ %.3g KiB/s<br>",
            transfer.BytesTransferred(), transfer.TotalSpeed() ) );
         //SearchInfo_TextBox.Insert( "<end><cbr><br><raw>" + m_downloadData + "</raw>" );

         StringList lines;
         m_downloadData.Break( lines, '\n' );
         if ( lines.Length() >= 2 )
         {
            // The first line has column titles. The second line has values.
            StringList tokens;
            lines[1].Break( tokens, '\t', true /*trim*/ );
            if ( tokens.Length() == 11 )
            {
               String objectName;
               String objectType;
               String spectralType;
               Optional<double> vmag;     // flux in the V filter
               Optional<double> ra;       // hours
               Optional<double> dec;      // degrees
               Optional<double> muAlpha;  // mas/year * cos( delta )
               Optional<double> muDelta;  // mas/year
               Optional<double> parallax; // arcsec
               Optional<double> radVel;   // km/s

               if ( !tokens[1].IsEmpty() )
                  ra = tokens[1].ToDouble() / 15; // degrees -> hours
               if ( !tokens[2].IsEmpty() )
                  dec = tokens[2].ToDouble(); // degrees
               if ( !tokens[3].IsEmpty() )
                  muAlpha = tokens[3].ToDouble(); // mas/yr * cos(delta)
               if ( !tokens[4].IsEmpty() )
                  muDelta = tokens[4].ToDouble(); // mas/yr
               if ( !tokens[5].IsEmpty() )
                  parallax = tokens[5].ToDouble() / 1000; // mas -> arcsec
               if ( !tokens[6].IsEmpty() )
                  radVel = tokens[6].ToDouble(); // km/s
               objectName = tokens[7].Unquoted().Trimmed();
               objectType = tokens[8].Unquoted().Trimmed();
               spectralType = tokens[9].Unquoted().Trimmed();
               if ( !tokens[10].IsEmpty() )
                  vmag = tokens[10].ToDouble();

               if ( ra.IsDefined() && dec.IsDefined() )
               {
                  StarPosition S( ra(),
                     dec(),
                     muAlpha.OrElse( 0 ),
                     muDelta.OrElse( 0 ),
                     parallax.OrElse( 0 ),
                     radVel.OrElse( 0 ) );
                  ObserverPosition O( m_parent.GeographicLongitude(),
                     m_parent.GeographicLatitude(),
                     m_parent.GeographicHeight() );
                  TimePoint t = TimePoint::Now();
                  pcl::Position P( t, "UTC" );
                  if ( m_parent.ShouldComputeTopocentricApparentCoordinates() )
                  {
                     P.SetObserver( O );
                     P.Apparent( S ).ToSpherical2Pi( m_alpha, m_delta );
                     m_alpha = Deg( m_alpha ) / 15;
                     m_delta = Deg( m_delta );
                  }
                  else
                  {
                     m_alpha = ra();
                     m_delta = dec();
                  }

                  String info = String()
                     << "<end><cbr><br><b>Object            :</b> "
                     << objectName
                     <<           "<br><b>Object type       :</b> "
                     << objectType
                     <<           "<br><b>Right Ascension   :</b> "
                     << String::ToSexagesimal( ra(), RAConversionOptions( 3 /*precision*/, 3 /*width*/ ) )
                     <<           "<br><b>Declination       :</b> "
                     << String::ToSexagesimal( dec(), DecConversionOptions( 2 /*precision*/, 3 /*width*/ ) );
                  if ( muAlpha.IsDefined() )
                     info <<      "<br><b>Proper motion RA* :</b> "
                          << String().Format( "%+8.2f mas/year * cos(delta)", muAlpha() );
                  if ( muDelta.IsDefined() )
                     info <<      "<br><b>Proper motion Dec :</b> "
                          << String().Format( "%+8.2f mas/year", muDelta() );
                  if ( parallax.IsDefined() )
                     info <<      "<br><b>Parallax          :</b> "
                          << String().Format( "%8.2f arcsec", parallax() );
                  if ( radVel.IsDefined() )
                     info <<      "<br><b>Radial velocity   :</b> "
                          << String().Format( "%+7.1f km/s", radVel() );
                  if ( !spectralType.IsEmpty() )
                     info <<      "<br><b>Spectral type     :</b> "
                          << spectralType;
                  if ( vmag.IsDefined() )
                     info <<      "<br><b>Visual Magnitude  :</b> "
                          << String().Format( "%.4g", vmag() );

                  if ( m_parent.ShouldComputeTopocentricApparentCoordinates() )
                  {
                     info << "<br>" << t.ToString( "%Y-%M-%D %h:%m:%s2" ) << " UTC"
                          << "<br><br><b>*** Topocentric Apparent Coordinates ***</b>"
                          << "<br><b>Right Ascension   :</b> "
                          << String::ToSexagesimal( m_alpha, RAConversionOptions( 3 /*precision*/, 3 /*width*/ ) )
                          << "<br><b>Declination       :</b> "
                          << String::ToSexagesimal( m_delta, DecConversionOptions( 2 /*precision*/, 3 /*width*/ ) )
                          << "<br>";
                  }
                  SearchInfo_TextBox.Insert( info );
                  m_valid = true;
                  Get_Button.Enable();
                  GoTo_Button.Enable();
               }
            }
         }

         if ( !m_valid )
            SearchInfo_TextBox.Insert( "<end><cbr><br>\x1b[31m*** Error: Unable to acquire valid coordinate information.\x1b[39m<br>" );
      }
      else
      {
         if ( m_abort )
            SearchInfo_TextBox.Insert( "<end><cbr><br>\x1b[31m<raw><* abort *></raw>\x1b[39m<br>" );
         else
            SearchInfo_TextBox.Insert( "<end><cbr><br>\x1b[31m*** Error: Download failed: <raw>" + transfer.ErrorInformation() + "</raw>\x1b[39m<br>" );
      }

      // ### FIXME: Workaround to force visibility of TextBox focus.
      SearchInfo_TextBox.Unfocus();
      Module->ProcessEvents();
      SearchInfo_TextBox.Focus();

      m_downloadData.Clear();
   }
   else if ( sender == Get_Button )
   {
      m_goto = false;
      Ok();
   }
   else if ( sender == GoTo_Button )
   {
      m_goto = true;
      Ok();
   }
   else if ( sender == Cancel_Button )
   {
      if ( m_downloading )
         m_abort = true;
      else
         Cancel();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

EphemerisSearchDialog::EphemerisSearchDialog( INDIMountInterface& parent )
   : m_parent( parent )
{
   const char* objectToolTip =
      "<p>Select one of the solar system bodies available to calculate ephemerides.</p>";

   Object_Label.SetText( "Object: " );
   Object_Label.SetToolTip( objectToolTip );
   Object_Label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   Objects_ComboBox.SetToolTip( objectToolTip );
   Objects_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&EphemerisSearchDialog::e_ItemSelected, *this );

   Calculate_Button.SetText( "Calculate" );
   Calculate_Button.SetIcon( ScaledResource( ":/icons/function.png" ) );
   Calculate_Button.SetToolTip( "<p>Compute apparent position.</p>" );
   Calculate_Button.OnClick( (Button::click_event_handler)&EphemerisSearchDialog::e_Click, *this );

   Objects_Sizer.SetSpacing( 4 );
   Objects_Sizer.Add( Object_Label );
   Objects_Sizer.Add( Objects_ComboBox, 100 );
   Objects_Sizer.Add( Calculate_Button );
   Objects_Sizer.AddStretch();

   ObjectInfo_TextBox.SetReadOnly();
   ObjectInfo_TextBox.SetStyleSheet( ScaledStyleSheet(
      "* {"
      "font-family: Hack, DejaVu Sans Mono, Monospace;"
      "font-size: 8pt;"
      "background: #141414;" // borrowed from /rsc/qss/core-standard.qss
      "color: #E8E8E8;"
      "}" ) );
   ObjectInfo_TextBox.Restyle();
   ObjectInfo_TextBox.SetMinSize( ObjectInfo_TextBox.Font().Width( 'm' ) * 81, ObjectInfo_TextBox.Font().Height() * 22 );

   Clear_Button.SetText( "Clear" );
   Clear_Button.SetIcon( ScaledResource( ":/icons/clear.png" ) );
   Clear_Button.SetToolTip( "<p>Clear coordinates.</p>" );
   Clear_Button.OnClick( (Button::click_event_handler)&EphemerisSearchDialog::e_Click, *this );

   Get_Button.SetText( "Get" );
   Get_Button.SetIcon( ScaledResource( ":/icons/window-import.png" ) );
   Get_Button.SetToolTip( "<p>Acquire apparent coordinates.</p>" );
   Get_Button.OnClick( (Button::click_event_handler)&EphemerisSearchDialog::e_Click, *this );
   Get_Button.Disable();

   GoTo_Button.SetText( "GoTo" );
   GoTo_Button.SetIcon( ScaledResource( ":/icons/play.png" ) );
   GoTo_Button.SetToolTip( "<p>Slew to apparent coordinates.</p>" );
   GoTo_Button.OnClick( (Button::click_event_handler)&EphemerisSearchDialog::e_Click, *this );
   GoTo_Button.Disable();

   Cancel_Button.SetText( "Cancel" );
   Cancel_Button.SetIcon( ScaledResource( ":/icons/cancel.png" ) );
   Cancel_Button.OnClick( (Button::click_event_handler)&EphemerisSearchDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.Add( Clear_Button );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( Get_Button );
   Buttons_Sizer.Add( GoTo_Button );
   Buttons_Sizer.Add( Cancel_Button );

   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.SetMargin( 8 );
   Global_Sizer.Add( Objects_Sizer );
   Global_Sizer.Add( ObjectInfo_TextBox, 100 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   OnShow( (Control::event_handler)&EphemerisSearchDialog::e_Show, *this );
}

// ----------------------------------------------------------------------------

String EphemerisSearchDialog::ObjectName( const EphemerisObject& object ) const
{
   return object.objectName;
}

// ----------------------------------------------------------------------------

void EphemerisSearchDialog::e_Show( Control& )
{
   if ( m_firstTimeShown )
   {
      m_firstTimeShown = false;

      EphemerisObjectList objects = Objects();
      for ( auto object : objects )
         Objects_ComboBox.AddItem( ObjectName( object ) );

      EnsureLayoutUpdated();
      AdjustToContents();
      SetMinSize();

      e_ItemSelected( Objects_ComboBox, Objects_ComboBox.CurrentItem() );
   }
}

// ----------------------------------------------------------------------------

void EphemerisSearchDialog::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == Objects_ComboBox )
      m_objectName = sender.ItemText( itemIndex ).Trimmed();
}

// ----------------------------------------------------------------------------

void EphemerisSearchDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == Calculate_Button )
   {
      bool isMoon = m_objectName.CompareIC( "Moon" ) == 0;
      TimePoint t = TimePoint::Now();
      EphemerisFile::Handle H( Ephemerides(), m_objectName, isMoon ? "Ea" : "SSB" );
      ObserverPosition O( m_parent.GeographicLongitude(),
         m_parent.GeographicLatitude(),
         m_parent.GeographicHeight() );
      pcl::Position P( t, "UTC" );
      P.SetObserver( O );
      P.Apparent( H ).ToSpherical2Pi( m_alpha, m_delta );
      m_alpha = Deg( m_alpha ) / 15;
      m_delta = Deg( m_delta );

      String info = String()
         << "<end><cbr><br><b>" << ObjectName() << "</b> "
         << "<br>" << t.ToString( "%Y-%M-%D %h:%m:%s2" ) << " UTC"
         << "<br><b>*** Topocentric Apparent Coordinates ***</b>"
         << "<br><b>Right ascension  :</b> "
         << String::ToSexagesimal( m_alpha, RAConversionOptions( 3 /*precision*/, 3 /*width*/ ) )
         << "<br><b>Declination      :</b> "
         << String::ToSexagesimal( m_delta, DecConversionOptions( 2 /*precision*/, 3 /*width*/ ) )
         << "<br><b>True distance    :</b> "
         << String().Format( "%10.*f %s", P.TrueDistance( H ), isMoon ? 3 : 7, isMoon ? "km" : "au" );
      if ( P.CanComputeApparentVisualMagnitude( H ) )
      {
         Optional<double> V = P.ApparentVisualMagnitude( H )();
         if ( V.IsDefined() )
            info << "<br><b>Visual magnitude :</b> " << String().Format( "%.2f", V() );
      }

      info << "<br>";

      ObjectInfo_TextBox.Insert( info );
      m_valid = true;
      Get_Button.Enable();
      GoTo_Button.Enable();
      ObjectInfo_TextBox.Focus();
   }
   else if ( sender == Clear_Button )
   {
      ObjectInfo_TextBox.Clear();
      Get_Button.Disable();
      GoTo_Button.Disable();
   }
   else if ( sender == Get_Button || sender == GoTo_Button )
   {
      if ( m_objectName == "Sun" )
         if ( MessageBox( "<p>You are about to point your telescope at the Sun.</p>"
                          "<p><b>THIS IS EXTREMELY DANGEROUS - BY DOING THIS YOU CAN CAUSE "
                          "IRREVERSIBLE DAMAGE TO YOUR EYES AND/OR TO YOUR EQUIPMENT.</b></p>"
                          "<p>Do you really want to do this?</p>",
                          WindowTitle(),
                          StdIcon::Warning, StdButton::Cancel, StdButton::Ok ).Execute() != StdButton::Ok )
         {
            return;
         }

      m_goto = sender == GoTo_Button;
      Ok();
   }
   else if ( sender == Cancel_Button )
   {
      Cancel();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PlanetSearchDialog::PlanetSearchDialog( INDIMountInterface& parent )
   : EphemerisSearchDialog( parent )
{
   SetWindowTitle( "Planet Coordinates" );
}

EphemerisObjectList PlanetSearchDialog::Objects() const
{
   return EphemerisObjectList() << EphemerisObject( "Me", "SSB", "Mercury" )
                                << EphemerisObject( "Ve", "SSB", "Venus" )
                                << EphemerisObject( "Ma", "SSB", "Mars" )
                                << EphemerisObject( "Ju", "SSB", "Jupiter" )
                                << EphemerisObject( "Sa", "SSB", "Saturn" )
                                << EphemerisObject( "Ur", "SSB", "Uranus" )
                                << EphemerisObject( "Ne", "SSB", "Neptune" )
                                << EphemerisObject( "Pl", "SSB", "Pluto" )
                                << EphemerisObject( "Mn", "Ea", "Moon" )
                                << EphemerisObject( "Sn", "SSB", "Sun" );
}

const EphemerisFile& PlanetSearchDialog::Ephemerides() const
{
   return EphemerisFile::FundamentalEphemerides();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

AsteroidSearchDialog::AsteroidSearchDialog( INDIMountInterface& parent )
   : EphemerisSearchDialog( parent )
{
   SetWindowTitle( "Asteroid Coordinates" );
}

EphemerisObjectList AsteroidSearchDialog::Objects() const
{
   EphemerisObjectList objects = EphemerisFile::AsteroidEphemerides().Objects();
   objects.Sort( []( const EphemerisObject& a, const EphemerisObject& b )
      {
         return a.objectId.ToInt() < b.objectId.ToInt();
      } );
   return objects;
}

const EphemerisFile& AsteroidSearchDialog::Ephemerides() const
{
   return EphemerisFile::AsteroidEphemerides();
}

String AsteroidSearchDialog::ObjectName( const EphemerisObject& object ) const
{
   return object.objectId + ' ' + object.objectName;
}

void AsteroidSearchDialog::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == Objects_ComboBox )
   {
      StringList tokens;
      sender.ItemText( itemIndex ).Break( tokens, ' ' );
      if ( tokens.Length() > 1 ) // !! always true
      {
         m_objectId = tokens[0].ToIsoString();
         m_objectName = tokens[1].ToIsoString();
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

SyncDataListDialog::SyncDataListDialog( Array<SyncDataPoint>& syncDataArray )
   : m_syncDataList( syncDataArray )
{
   SnycData_TreeBox.SetMinHeight( Font().Width( 'm' ) * 60 );
   SnycData_TreeBox.SetScaledMinWidth( Font().Height() * 50 );
   SnycData_TreeBox.SetNumberOfColumns( 8 );
   SnycData_TreeBox.SetHeaderText( 2, "HA" );
   SnycData_TreeBox.SetHeaderText( 3, "Dec" );
   SnycData_TreeBox.SetHeaderText( 4, "Delta HA" );
   SnycData_TreeBox.SetHeaderText( 5, "Delta Dec" );
   SnycData_TreeBox.SetHeaderText( 6, "Pier side" );
   SnycData_TreeBox.SetHeaderText( 7, "Created on" );
   SnycData_TreeBox.EnableAlternateRowColor();
   SnycData_TreeBox.EnableMultipleSelections( true );

   for ( size_t index = 0; index < syncDataArray.Length(); ++index )
   {
      TreeBox::Node* node = new TreeBox::Node();
      auto syncDataPoint = syncDataArray[index];
      node->SetText( 0, String().Format( "%i", index ) );
      node->SetIcon( 1, ScaledResource( syncDataPoint.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) );
      node->SetText( 2, String().Format( "%2.3f", AlignmentModel::RangeShiftHourAngle( syncDataPoint.localSiderialTime - syncDataPoint.celestialRA ) ) );
      node->SetText( 3, String().Format( "%2.3f", syncDataPoint.celestialDEC ) );
      node->SetText( 4, String().Format( "%2.5f", syncDataPoint.telecopeRA - syncDataPoint.celestialRA ) );
      node->SetText( 5, String().Format( "%2.5f", syncDataPoint.celestialDEC - syncDataPoint.telecopeDEC ) );
      node->SetText( 6, String( ( syncDataPoint.pierSide == IMCPierSide::West ) ? "West" : ( ( syncDataPoint.pierSide == IMCPierSide::East ) ? "East" : "" ) ) );
      node->SetText( 7, syncDataPoint.creationTime.ToString() );
      SnycData_TreeBox.Add( node );
   }

   for ( int i = 0; i < SnycData_TreeBox.NumberOfColumns(); ++i )
      SnycData_TreeBox.AdjustColumnWidthToContents( i );

   SyncDataList_Sizer.SetSpacing( 8 );
   SyncDataList_Sizer.Add( SnycData_TreeBox, 100 );

   Delete_Button.SetText( "Delete" );
   //Delete_Button.SetIcon( ScaledResource( ":/icons/window-import.png" ) );
   Delete_Button.OnClick( (Button::click_event_handler)&SyncDataListDialog::e_Click, *this );

   Enable_Button.SetText( "Enable" );
   //Delete_Button.SetIcon( ScaledResource( ":/icons/window-import.png" ) );
   Enable_Button.OnClick( (Button::click_event_handler)&SyncDataListDialog::e_Click, *this );

   Disable_Button.SetText( "Disable" );
   //Delete_Button.SetIcon( ScaledResource( ":/icons/window-import.png" ) );
   Disable_Button.OnClick( (Button::click_event_handler)&SyncDataListDialog::e_Click, *this );

   Ok_Button.SetText( "Ok" );
   Ok_Button.SetIcon( ScaledResource( ":/icons/ok.png" ) );
   Ok_Button.OnClick( (Button::click_event_handler)&SyncDataListDialog::e_Click, *this );

   Cancel_Button.SetText( "Cancel" );
   Cancel_Button.SetIcon( ScaledResource( ":/icons/cancel.png" ) );
   Cancel_Button.OnClick( (Button::click_event_handler)&SyncDataListDialog::e_Click, *this );

   SyncDataListButton_Sizer.SetSpacing( 8 );
   SyncDataListButton_Sizer.Add( Enable_Button );
   SyncDataListButton_Sizer.Add( Disable_Button );
   SyncDataListButton_Sizer.Add( Delete_Button );
   SyncDataListButton_Sizer.AddStretch();
   SyncDataListButton_Sizer.Add( Ok_Button );
   SyncDataListButton_Sizer.Add( Cancel_Button );

   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.SetMargin( 8 );
   Global_Sizer.Add( SyncDataList_Sizer );
   Global_Sizer.Add( SyncDataListButton_Sizer );

   SetSizer( Global_Sizer );
   SetWindowTitle( "List of Sync Data Points" );

   OnShow( (Control::event_handler)&SyncDataListDialog::e_Show, *this );
   OnClose( (Control::close_event_handler)&SyncDataListDialog::e_Close, *this );
}

// ----------------------------------------------------------------------------

void SyncDataListDialog::e_Show( Control& )
{
   if ( m_firstTimeShown )
   {
      m_firstTimeShown = false;

      EnsureLayoutUpdated();
      AdjustToContents();
      SetMinSize();
   }
}

// ----------------------------------------------------------------------------

void SyncDataListDialog::e_Close( Control& sender, bool& allowClose )
{
   // ### Placeholder
}

// ----------------------------------------------------------------------------

void SyncDataListDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == Disable_Button )
   {
      for ( auto node : SnycData_TreeBox.SelectedNodes() )
      {
         int index = SnycData_TreeBox.ChildIndex( node );
         m_syncDataList[index].enabled = false;
         node->SetIcon( 1, ScaledResource( ":/browser/disabled.png" ) );
      }
   }
   else if ( sender == Enable_Button )
   {
      for ( auto node : SnycData_TreeBox.SelectedNodes() )
      {
         int index = SnycData_TreeBox.ChildIndex( node );
         m_syncDataList[index].enabled = true;
         node->SetIcon( 1, ScaledResource( ":/browser/enabled.png" ) );
      }
   }
   else if ( sender == Delete_Button )
   {
      for ( auto node : SnycData_TreeBox.SelectedNodes() )
      {
         int index = SnycData_TreeBox.ChildIndex( node );
         m_syncDataList.Remove( m_syncDataList[index] );
         SnycData_TreeBox.Remove( index );
      }
   }
   else if ( sender == Ok_Button )
   {
      Ok();
   }
   else if ( sender == Cancel_Button )
   {
      Cancel();
   }
}

// ----------------------------------------------------------------------------

AlignmentConfigDialog::AlignmentConfigDialog( INDIMountInterface& w )
   : m_interface( w )
{
   const char* pierSideToolTipText =
      "<p>Create a model for each side of the pier. This is useful for mounts that perform a meridian "
      "flip when objects cross the meridian.</p>"
      "<p>Creating models for each pier side can dramatically improve the accuracy of pointing model, "
      "since some pointing error sources depend on the position and balance of the mounted devices.</p>"
      "<p>Note that two pointing model need twice as many sync points.</p>";

   ModelPierSide_CheckBox.SetText( "Model each pier side" );
   ModelPierSide_CheckBox.SetToolTip( pierSideToolTipText );
   ModelPierSide_CheckBox.Check();

   const char* offsetToolTipText =
      "<p>Zero-point offset in the right ascension and declination axes.</p>";

   Offset_CheckBox.SetText( "Offset" );
   Offset_CheckBox.SetToolTip( offsetToolTipText );
   Offset_CheckBox.Check();

   const char* collimationToolTipText =
      "<p>Misalignment between optical and mechanical axes</p>";

   Collimation_CheckBox.SetText( "Collimation" );
   Collimation_CheckBox.SetToolTip( collimationToolTipText );
   Collimation_CheckBox.Check();

   const char* nonPerpendicularToolTipText =
      "<p>Polar/declination axis non-orthogonality error.</p>";

   NonPerpendicular_CheckBox.SetText( "Perpendicularity" );
   NonPerpendicular_CheckBox.SetToolTip( nonPerpendicularToolTipText );
   NonPerpendicular_CheckBox.Check();

   const char* polarAxisDisplacementToolTipText =
      "<p>Polar axis displacement between instrumental and true poles.</p>";

   PolarAxisDisplacement_CheckBox.SetText( "Polar axis displacement" );
   PolarAxisDisplacement_CheckBox.SetToolTip( polarAxisDisplacementToolTipText );
   PolarAxisDisplacement_CheckBox.Check();

   const char* tubeFlexureToolTipText =
      "<p>Tube flexure away from zenith.</p>";

   TubeFlexure_CheckBox.SetText( "Tube flexure" );
   TubeFlexure_CheckBox.SetToolTip( tubeFlexureToolTipText );
   TubeFlexure_CheckBox.Check();

   const char* forkFlexureToolTipText =
      "<p>Fork flexure away from zenith (only for non-German equatorial mounts).</p>";

   ForkFlexure_CheckBox.SetText( "Fork flexure" );
   ForkFlexure_CheckBox.SetToolTip( forkFlexureToolTipText );

   const char* deltaAxisToolTipText =
      "<p>Bending of the declination axis (only for German equatorial mounts).</p>";

   DeltaAxisFlexure_CheckBox.SetText( "Declination axis bending" );
   DeltaAxisFlexure_CheckBox.SetToolTip( deltaAxisToolTipText );
   DeltaAxisFlexure_CheckBox.Check();

   const char* linearTermToolTipText =
      "<p>Linear scale error in hour angle.</p>";

   Linear_CheckBox.SetText( "Linear" );
   Linear_CheckBox.SetToolTip( linearTermToolTipText );
   Linear_CheckBox.Check();

   const char* quadraticTermToolTipText =
      "<p>Quadratic scale error in hour angle.</p>";

   Quadratic_CheckBox.SetText( "Quadratic" );
   Quadratic_CheckBox.SetToolTip( quadraticTermToolTipText );
   Quadratic_CheckBox.Check();

   MountAlignmentConfig_Sizer.SetSpacing( 8 );
   MountAlignmentConfig_Sizer.SetMargin( 8 );
   MountAlignmentConfig_Sizer.Add( ModelPierSide_CheckBox );
   MountAlignmentConfig_Sizer.Add( Offset_CheckBox );
   MountAlignmentConfig_Sizer.Add( Collimation_CheckBox );
   MountAlignmentConfig_Sizer.Add( NonPerpendicular_CheckBox );
   MountAlignmentConfig_Sizer.Add( PolarAxisDisplacement_CheckBox );
   MountAlignmentConfig_Sizer.Add( TubeFlexure_CheckBox );
   MountAlignmentConfig_Sizer.Add( ForkFlexure_CheckBox );
   MountAlignmentConfig_Sizer.Add( DeltaAxisFlexure_CheckBox );
   MountAlignmentConfig_Sizer.Add( Linear_CheckBox );
   MountAlignmentConfig_Sizer.Add( Quadratic_CheckBox );

   AnalyticalAlignment_ConfigControl.SetSizer( MountAlignmentConfig_Sizer );

   AlignmentConfig_TabBox.AddPage( AnalyticalAlignment_ConfigControl, "Analytical Pointing Model" );
   AlignmentConfig_TabBox.OnPageSelected( (TabBox::page_event_handler)&AlignmentConfigDialog::e_PageSelected, *this );
   //AlignmentConfig_TabBox.AddPage( SurfaceSplineAlignment_ConfigControl, "Surface Splines Pointing Model" );

   Ok_Button.SetText( "Ok" );
   Ok_Button.SetIcon( ScaledResource( ":/icons/ok.png" ) );
   Ok_Button.OnClick( (Button::click_event_handler)&AlignmentConfigDialog::e_Click, *this );

   Cancel_Button.SetText( "Cancel" );
   Cancel_Button.SetIcon( ScaledResource( ":/icons/cancel.png" ) );
   Cancel_Button.OnClick( (Button::click_event_handler)&AlignmentConfigDialog::e_Click, *this );

   AlignmentConfigButton_Sizer.SetSpacing( 8 );
   AlignmentConfigButton_Sizer.AddStretch();
   AlignmentConfigButton_Sizer.Add( Ok_Button );
   AlignmentConfigButton_Sizer.Add( Cancel_Button );

   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.SetMargin( 8 );
   Global_Sizer.Add( AlignmentConfig_TabBox );
   Global_Sizer.Add( AlignmentConfigButton_Sizer );

   SetSizer( Global_Sizer );
   SetWindowTitle( "Telescope Pointing Models" );

   OnShow( (Control::event_handler)&AlignmentConfigDialog::e_Show, *this );
}

// ----------------------------------------------------------------------------

void AlignmentConfigDialog::e_Show( Control& )
{
   if ( m_firstTimeShown )
   {
      m_firstTimeShown = false;

      EnsureLayoutUpdated();
      SetMinWidth( RoundInt( 2.5 * Font().Width( WindowTitle() ) ) );
      AdjustToContents();
      SetFixedSize();
   }
}

// ----------------------------------------------------------------------------

void AlignmentConfigDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == Ok_Button )
   {
      m_interface.GUI->m_modelBothPierSides = ModelPierSide_CheckBox.IsChecked();
      m_interface.GUI->m_alignmentConfigOffset = Offset_CheckBox.IsChecked();
      m_interface.GUI->m_alignmentConfigCollimation = Collimation_CheckBox.IsChecked();
      m_interface.GUI->m_alignmentConfigNonPerpendicular = NonPerpendicular_CheckBox.IsChecked();
      m_interface.GUI->m_alignmentConfigPolarAxisDisp = PolarAxisDisplacement_CheckBox.IsChecked();
      m_interface.GUI->m_alignmentConfigDecAxisFlexure = DeltaAxisFlexure_CheckBox.IsChecked();
      m_interface.GUI->m_alignmentConfigTubeFlexure = TubeFlexure_CheckBox.IsChecked();
      m_interface.GUI->m_alignmentConfigForkFlexure = ForkFlexure_CheckBox.IsChecked();
      m_interface.GUI->m_alignmentLinear = Linear_CheckBox.IsChecked();
      m_interface.GUI->m_alignmentQuadratic = Quadratic_CheckBox.IsChecked();
      m_interface.GUI->m_alignmentModelIndex = AlignmentConfig_TabBox.CurrentPageIndex() + 1;
      Ok();
   }
   else if ( sender == Cancel_Button )
   {
      Cancel();
   }
}

// ----------------------------------------------------------------------------

void AlignmentConfigDialog::e_PageSelected( TabBox& sender, int tabIndex )
{
   if ( sender == AlignmentConfig_TabBox )
   {
      // ### Placeholder
   }
}

// ----------------------------------------------------------------------------

MountConfigDialog::MountConfigDialog( const String& deviceName,
                                      double geoLat, double geoLong, double geoHeight,
                                      String utcTime, double utcOffset, double telescopeFocalLength )
   : ConfigDialogBase( deviceName )
   , m_device( deviceName )
{
   int emWidth = Font().Width( 'm' );
   int editWidth1 = RoundInt( 4.25 * emWidth );
   int editWidth2 = RoundInt( 5.25 * emWidth );

   int sign, s1, s2;
   double s3;

   int labelWidth1 = Font().Width( "Geographic Longitude:" ) + emWidth;

   Latitude_Label.SetText( "Geographic Latitude:" );
   Latitude_Label.SetToolTip( "<p>Position of observatory: Geographic latitude in degrees, arcminutes and arcseconds.</p>" );
   Latitude_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   Latitude_Label.SetFixedWidth( labelWidth1 );

   DecimalToSexagesimal( sign, s1, s2, s3, geoLat );

   Latitude_H_SpinBox.SetRange( 0, 90 );
   Latitude_H_SpinBox.SetFixedWidth( editWidth1 );
   Latitude_H_SpinBox.SetValue( Abs( s1 ) );

   Latitude_M_SpinBox.SetRange( 0, 59 );
   Latitude_M_SpinBox.SetFixedWidth( editWidth1 );
   Latitude_M_SpinBox.SetValue( s2 );

   Latitude_S_NumericEdit.SetReal();
   Latitude_S_NumericEdit.SetPrecision( 3 );
   Latitude_S_NumericEdit.EnableFixedPrecision();
   Latitude_S_NumericEdit.SetRange( 0, 59.999 );
   Latitude_S_NumericEdit.label.Hide();
   Latitude_S_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Latitude_S_NumericEdit.SetValue( s3 );

   LatitudeIsSouth_CheckBox.SetText( "South" );
   LatitudeIsSouth_CheckBox.SetToolTip( "<p>When checked, latitude is negative (southern hemisphere).</p>" );
   LatitudeIsSouth_CheckBox.SetChecked( sign < 0 );

   Latitude_Sizer.SetSpacing( 4 );
   Latitude_Sizer.Add( Latitude_Label );
   Latitude_Sizer.Add( Latitude_H_SpinBox );
   Latitude_Sizer.Add( Latitude_M_SpinBox );
   Latitude_Sizer.Add( Latitude_S_NumericEdit );
   Latitude_Sizer.Add( LatitudeIsSouth_CheckBox );
   Latitude_Sizer.AddStretch();

   DecimalToSexagesimal( sign, s1, s2, s3, geoLong );

   Longitude_Label.SetText( "Geographic Longitude:" );
   Longitude_Label.SetToolTip( "<p>Position of observatory: Geographic longitude in degrees, arcminutes and arcseconds.</p>" );
   Longitude_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   Longitude_Label.SetFixedWidth( labelWidth1 );

   Longitude_H_SpinBox.SetRange( 0, 180 );
   Longitude_H_SpinBox.SetFixedWidth( editWidth1 );
   Longitude_H_SpinBox.SetValue( Abs( s1 ) );

   Longitude_M_SpinBox.SetRange( 0, 59 );
   Longitude_M_SpinBox.SetFixedWidth( editWidth1 );
   Longitude_M_SpinBox.SetValue( s2 );

   Longitude_S_NumericEdit.SetReal();
   Longitude_S_NumericEdit.SetPrecision( 2 );
   Longitude_S_NumericEdit.EnableFixedPrecision();
   Longitude_S_NumericEdit.SetRange( 0, 59.99 );
   Longitude_S_NumericEdit.label.Hide();
   Longitude_S_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Longitude_S_NumericEdit.SetValue( s3 );

   LongitudeIsWest_CheckBox.SetText( "West" );
   LongitudeIsWest_CheckBox.SetToolTip( "<p>When checked, longitude is negative (western longitude).</p>" );
   LongitudeIsWest_CheckBox.SetChecked( sign < 0 );

   Longitude_Sizer.SetSpacing( 4 );
   Longitude_Sizer.Add( Longitude_Label );
   Longitude_Sizer.Add( Longitude_H_SpinBox );
   Longitude_Sizer.Add( Longitude_M_SpinBox );
   Longitude_Sizer.Add( Longitude_S_NumericEdit );
   Longitude_Sizer.Add( LongitudeIsWest_CheckBox );
   Longitude_Sizer.AddStretch();

   Height_NumericEdit.SetInteger();
   Height_NumericEdit.SetRange( 0, 500000 );
   Height_NumericEdit.label.SetText( "Geographic Height:" );
   Height_NumericEdit.label.SetToolTip( "<p>Position of observatory: Geographic height in meters.</p></p>" );
   Height_NumericEdit.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   Height_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Height_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Height_NumericEdit.sizer.AddStretch();
   Height_NumericEdit.SetValue( geoHeight );

   double dayFraction;
   double timeZone;
   int year, month, day;
   utcTime.TryParseISO8601DateTime( year, month, day, dayFraction, timeZone );

   Date_Label.SetText( "Date:" );
   Date_Label.SetToolTip( "<p>Current date</p>" );
   Date_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   Date_Label.SetFixedWidth( labelWidth1 );

   Date_Day_SpinBox.SetRange( 0, 31 );
   Date_Day_SpinBox.SetFixedWidth( editWidth1 );
   Date_Day_SpinBox.SetValue( day );

   Date_Month_SpinBox.SetRange( 0, 12 );
   Date_Month_SpinBox.SetFixedWidth( editWidth1 );
   Date_Month_SpinBox.SetValue( month );

   Date_Year_SpinBox.SetRange( 0, 9999 );
   Date_Year_SpinBox.SetFixedWidth( editWidth1 );
   Date_Year_SpinBox.SetValue( year );

   GetHostDateTime_PushButton.SetText( "Get" );
   GetHostDateTime_PushButton.SetToolTip( "<p>Get date and utc time from the host where the Indigo server is running on.</p>" );
   GetHostDateTime_PushButton.OnClick( (Button::click_event_handler)&MountConfigDialog::e_Click, *this );

   Date_Sizer.SetSpacing( 4 );
   Date_Sizer.Add( Date_Label );
   Date_Sizer.Add( Date_Day_SpinBox );
   Date_Sizer.Add( Date_Month_SpinBox );
   Date_Sizer.Add( Date_Year_SpinBox );
   Date_Sizer.Add( GetHostDateTime_PushButton );
   Date_Sizer.AddStretch();

   DecimalToSexagesimal( sign, s1, s2, s3, dayFraction * 24 );

   UtcTime_Label.SetText( "UTC time:" );
   UtcTime_Label.SetToolTip( "<p>Current UTC time</p>" );
   UtcTime_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   UtcTime_Label.SetFixedWidth( labelWidth1 );

   UtcTime_H_SpinBox.SetRange( 0, 180 );
   UtcTime_H_SpinBox.SetFixedWidth( editWidth1 );
   UtcTime_H_SpinBox.SetValue( s1 );

   UtcTime_M_SpinBox.SetRange( 0, 59 );
   UtcTime_M_SpinBox.SetFixedWidth( editWidth1 );
   UtcTime_M_SpinBox.SetValue( s2 );

   UtcTime_S_NumericEdit.SetReal();
   UtcTime_S_NumericEdit.SetPrecision( 2 );
   UtcTime_S_NumericEdit.EnableFixedPrecision();
   UtcTime_S_NumericEdit.SetRange( 0, 59.99 );
   UtcTime_S_NumericEdit.label.Hide();
   UtcTime_S_NumericEdit.edit.SetFixedWidth( editWidth2 );
   UtcTime_S_NumericEdit.SetValue( s3 );

   UtcTime_Sizer.SetSpacing( 4 );
   UtcTime_Sizer.Add( UtcTime_Label );
   UtcTime_Sizer.Add( UtcTime_H_SpinBox );
   UtcTime_Sizer.Add( UtcTime_M_SpinBox );
   UtcTime_Sizer.Add( UtcTime_S_NumericEdit );
   UtcTime_Sizer.AddStretch();

   UpdateUtc_Timer.SetInterval( 1 );
   UpdateUtc_Timer.SetPeriodic( true );
   UpdateUtc_Timer.OnTimer( (Timer::timer_event_handler)&MountConfigDialog::e_Timer, *this );
   UpdateUtc_Timer.Start();

   /*TelescopeAperture_NumericEdit.label.SetText( "Telescope aperture:" );
   TelescopeAperture_NumericEdit.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   TelescopeAperture_NumericEdit.label.SetToolTip( "<p>Telescope's aperture in millimeters.</p>" );
   TelescopeAperture_NumericEdit.label.SetFixedWidth( labelWidth1 );
   TelescopeAperture_NumericEdit.SetInteger();
   TelescopeAperture_NumericEdit.SetRange( 10, 10000 );
   TelescopeAperture_NumericEdit.edit.SetFixedWidth( editWidth2 );
   TelescopeAperture_NumericEdit.sizer.AddStretch();
   TelescopeAperture_NumericEdit.SetValue( telescopeAperture );
*/
   TelescopeFocalLength_NumericEdit.label.SetText( "Telescope focal length:" );
   TelescopeFocalLength_NumericEdit.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   TelescopeFocalLength_NumericEdit.label.SetToolTip( "<p>Telescope's focal length in millimeters.</p> " );
   TelescopeFocalLength_NumericEdit.label.SetFixedWidth( labelWidth1 );
   TelescopeFocalLength_NumericEdit.SetInteger();
   TelescopeFocalLength_NumericEdit.SetRange( 0, 100000 );
   TelescopeFocalLength_NumericEdit.edit.SetFixedWidth( editWidth2 );
   TelescopeFocalLength_NumericEdit.sizer.AddStretch();
   TelescopeFocalLength_NumericEdit.SetValue( telescopeFocalLength );

   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.SetMargin( 8 );
   Global_Sizer.Add( Latitude_Sizer );
   Global_Sizer.Add( Longitude_Sizer );
   Global_Sizer.Add( Height_NumericEdit );
   Global_Sizer.Add( Date_Sizer );
   Global_Sizer.Add( UtcTime_Sizer );
   Global_Sizer.Add( TelescopeFocalLength_NumericEdit );

   AddBaseControls();
}

// ----------------------------------------------------------------------------

void MountConfigDialog::SendUpdatedProperties()
{
   double lat = SexagesimalToDecimal( LatitudeIsSouth_CheckBox.IsChecked() ? -1 : +1,
      Latitude_H_SpinBox.Value(), Latitude_M_SpinBox.Value(), Latitude_S_NumericEdit.Value() );

   INDIClient::TheClient()->SendNewPropertyItem( m_device, GEOGRAPHIC_COORDINATES_PROPERTY_NAME, "INDI_NUMBER", GEOGRAPHIC_COORDINATES_LATITUDE_ITEM_NAME, lat );

   double longitude = SexagesimalToDecimal( LongitudeIsWest_CheckBox.IsChecked() ? -1 : +1,
      Longitude_H_SpinBox.Value(), Longitude_M_SpinBox.Value(), Longitude_S_NumericEdit.Value() );

   INDIClient::TheClient()->SendNewPropertyItem( m_device, GEOGRAPHIC_COORDINATES_PROPERTY_NAME, "INDI_NUMBER", GEOGRAPHIC_COORDINATES_LONGITUDE_ITEM_NAME, longitude );

   INDIPropertyListItem mountProp;
   if ( INDIClient::TheClient()->GetPropertyItem( m_device, GEOGRAPHIC_COORDINATES_PROPERTY_NAME, GEOGRAPHIC_COORDINATES_ELEVATION_ITEM_NAME, mountProp, false /*formatted*/ ) )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, GEOGRAPHIC_COORDINATES_PROPERTY_NAME, "INDI_NUMBER", GEOGRAPHIC_COORDINATES_ELEVATION_ITEM_NAME, Height_NumericEdit.Value() );
   }
   if ( INDIClient::TheClient()->GetPropertyItem( m_device, UTC_TIME_PROPERTY_NAME, UTC_TIME_ITEM_NAME, mountProp, false /*formatted*/ ) )
   {
      double newUtcTime = SexagesimalToDecimal( +1, UtcTime_H_SpinBox.Value(), UtcTime_M_SpinBox.Value(), UtcTime_S_NumericEdit.Value() );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, UTC_TIME_PROPERTY_NAME, "INDI_NUMBER", UTC_TIME_ITEM_NAME, newUtcTime );
   }
   // sending telescope info in bulk request
   if ( INDIClient::TheClient()->GetPropertyItem( m_device, "TELESCOPE_INFO", "TELESCOPE_FOCAL_LENGTH", mountProp, false /*formatted*/ ) )
   {
      INDINewPropertyItem newPropertyItem( m_device, "TELESCOPE_INFO", "INDI_NUMBER" );
      newPropertyItem.ElementValues << ElementValue( "TELESCOPE_APERTURE", TelescopeAperture_NumericEdit.Value() );
      newPropertyItem.ElementValues << ElementValue( "TELESCOPE_FOCAL_LENGTH", TelescopeFocalLength_NumericEdit.Value() );
      newPropertyItem.ElementValues << ElementValue( "GUIDER_APERTURE", 10 );
      newPropertyItem.ElementValues << ElementValue( "GUIDER_FOCAL_LENGTH", 100 );
      INDIClient::TheClient()->SendNewPropertyItem( newPropertyItem );
   }
}

// ----------------------------------------------------------------------------

void MountConfigDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == GetHostDateTime_PushButton )
   {
      INDIPropertyListItem mountProp;
      if ( INDIClient::TheClient()->GetPropertyItem( m_device,
                                                     MOUNT_SET_HOST_TIME_PROPERTY_NAME,
                                                     MOUNT_SET_HOST_TIME_ITEM_NAME,
                                                     mountProp,
                                                     false /*formatted*/ ) )
      {
         INDIClient::TheClient()->MaybeSendNewPropertyItem( m_device,
                                                            MOUNT_SET_HOST_TIME_PROPERTY_NAME,
                                                            "INDI_SWITCH",
                                                            MOUNT_SET_HOST_TIME_ITEM_NAME,
                                                            "ON",
                                                            true /*async*/ );
      }
   }
}

void MountConfigDialog::e_Timer( Timer& sender )
{
   if ( sender == UpdateUtc_Timer )
   {
      INDIPropertyListItem mountProp;
      if ( INDIClient::TheClient()->GetPropertyItem( m_device,
                                                     UTC_TIME_PROPERTY_NAME,
                                                     UTC_TIME_ITEM_NAME,
                                                     mountProp,
                                                     false /*formatted*/ ) )
      {
         double dayFraction;
         double timeZone;
         int year, month, day;
         mountProp.PropertyValue.TryParseISO8601DateTime( year, month, day, dayFraction, timeZone );
         Date_Day_SpinBox.SetValue( day );
         Date_Month_SpinBox.SetValue( month );
         Date_Year_SpinBox.SetValue( year );

         int sign, hour, minutes;
         double seconds;
         DecimalToSexagesimal( sign, hour, minutes, seconds, dayFraction * 24 );
         UtcTime_H_SpinBox.SetValue( hour );
         UtcTime_M_SpinBox.SetValue( minutes );
         UtcTime_S_NumericEdit.SetValue( seconds );
      }
      else
      {
         Date_Label.Disable();
         Date_Day_SpinBox.Disable();
         Date_Month_SpinBox.Disable();
         Date_Year_SpinBox.Disable();
         UtcTime_Label.Disable();
         UtcTime_H_SpinBox.Disable();
         UtcTime_M_SpinBox.Disable();
         UtcTime_S_NumericEdit.Disable();
         GetHostDateTime_PushButton.Disable();
      }
   }
}

// ----------------------------------------------------------------------------

INDIMountInterface::INDIMountInterface()
{
   TheINDIMountInterface = this;
}

// ----------------------------------------------------------------------------

INDIMountInterface::~INDIMountInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString INDIMountInterface::Id() const
{
   return "IndigoMount";
}

// ----------------------------------------------------------------------------

MetaProcess* INDIMountInterface::Process() const
{
   return TheINDIMountProcess;
}

// ----------------------------------------------------------------------------

String INDIMountInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/IndigoMount.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures INDIMountInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

// ----------------------------------------------------------------------------

void INDIMountInterface::ResetInstance()
{
   INDIMountInstance defaultInstance( TheINDIMountProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool INDIMountInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Indigo Mount Controller" );
      ResetInstance();
      UpdateControls();
   }

   dynamic = false;
   return &P == TheINDIMountProcess;
}

// ----------------------------------------------------------------------------

uint32 INDIMountInterface::GUIData::AlignmentConfigParameter() const
{
   uint32 configParam = 0;

   if ( m_modelBothPierSides )
      configParam |= 1 << 0; // set if modeling for each pier side is requested

   if ( m_alignmentConfigOffset )
      configParam |= 1 << 1; // offset

   if ( m_alignmentConfigCollimation )
      configParam |= 1 << 2; // collimation error

   if ( m_alignmentConfigNonPerpendicular )
      configParam |= 1 << 3; // non-perpendicular dec-ra axis error

   if ( m_alignmentConfigPolarAxisDisp )
   {
      configParam |= 1 << 4; // polar-axis horizontal displacement
      configParam |= 1 << 5; // polar-axis vertical displacement
   }

   if ( m_alignmentConfigTubeFlexure )
      configParam |= 1 << 6; // tube flexure

   if ( m_alignmentConfigForkFlexure )
      configParam |= 1 << 7; // fork flexure

   if ( m_alignmentConfigDecAxisFlexure )
      configParam |= 1 << 8; // delta-axis flexure

   if ( m_alignmentLinear )
      configParam |= 1 << 9; // linear terms

   if ( m_alignmentQuadratic )
      configParam |= 1 << 10; // quadratic terms

   return configParam;
}

// ----------------------------------------------------------------------------

int INDIMountInterface::AlignmentMethod() const
{
   if ( GUI == nullptr )
      return IMCAlignmentMethod::Default;
   return GUI->m_alignmentModelIndex;
}

bool INDIMountInterface::ShouldComputeTopocentricApparentCoordinates()
{
   INDIPropertyListItem item;
   INDIClient* indi = INDIClient::TheClientOrDie();
   if ( !indi->GetPropertyItem( m_device,
                                MOUNT_EPOCH_PROPERTY_NAME,
                                MOUNT_EPOCH_ITEM_NAME,
                                item,
                                false /*formatted*/ ) )
   {
      return true;
   }

   return TruncInt( item.PropertyValue.ToDouble() ) == 0;
}

// ----------------------------------------------------------------------------

ProcessImplementation* INDIMountInterface::NewProcess() const
{
   INDIMountInstance* instance = new INDIMountInstance( TheINDIMountProcess );
   instance->p_deviceName = m_device;
   instance->p_slewRate = GUI->SlewSpeed_ComboBox.CurrentItem();

   instance->p_targetRA = SexagesimalToDecimal( 0,
      GUI->TargetRA_H_SpinBox.Value(), GUI->TargetRA_M_SpinBox.Value(), GUI->TargetRA_S_NumericEdit.Value() );

   instance->p_targetDec = SexagesimalToDecimal( GUI->MountTargetDECIsSouth_CheckBox.IsChecked() ? -1 : +1,
      GUI->TargetDec_H_SpinBox.Value(), GUI->TargetDec_M_SpinBox.Value(), GUI->TargetDec_S_NumericEdit.Value() );

   instance->p_alignmentMethod = GUI->m_alignmentModelIndex;

   instance->p_enableAlignmentCorrection = GUI->MountAlignmentCorrection_CheckBox.IsChecked();

   switch ( instance->p_alignmentMethod )
   {
   case IMCAlignmentMethod::AnalyticalModel:
      instance->p_alignmentConfig = GUI->AlignmentConfigParameter();
      break;
      // ### More alignment methods must be inserted here.
   default:
      break;
   }

   instance->p_alignmentFile = GUI->AlignmentFile_Edit.Text();
   instance->GetCurrentCoordinates();
   instance->p_pierSide = m_pierSide;
   instance->o_geographicLatitude = m_geoLatitude;

   return instance;
}

// ----------------------------------------------------------------------------

bool INDIMountInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const INDIMountInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an IndigoMount instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool INDIMountInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool INDIMountInterface::ImportProcess( const ProcessImplementation& p )
{
   const INDIMountInstance* instance = dynamic_cast<const INDIMountInstance*>( &p );
   if ( instance != nullptr )
   {
      double targetRA, targetDec;
      instance->GetTargetCoordinates( targetRA, targetDec );

      int sign, s1, s2;
      double s3;
      DecimalToSexagesimal( sign, s1, s2, s3, targetRA );
      GUI->TargetRA_H_SpinBox.SetValue( s1 );
      GUI->TargetRA_M_SpinBox.SetValue( s2 );
      GUI->TargetRA_S_NumericEdit.SetValue( s3 );

      DecimalToSexagesimal( sign, s1, s2, s3, targetDec );
      GUI->TargetDec_H_SpinBox.SetValue( s1 );
      GUI->TargetDec_M_SpinBox.SetValue( s2 );
      GUI->TargetDec_S_NumericEdit.SetValue( s3 );
      GUI->MountTargetDECIsSouth_CheckBox.SetChecked( sign < 0 );

      if ( instance->ValidateDevice( false /*throwErrors*/ ) )
         m_device = instance->p_deviceName;
      else
         m_device.Clear();

      UpdateControls();

      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------

void INDIMountInterface::UpdateControls()
{
   if ( m_device.IsEmpty() )
   {
      GUI->UpdateDeviceProperties_Timer.Stop();
      GUI->MountProperties_Control.Disable();
      GUI->MountAlignment_Control.Disable();
      GUI->Slew_Control.Disable();
      GUI->MountGoTo_Control.Disable();
   }
   else
   {
      if ( !GUI->UpdateDeviceProperties_Timer.IsRunning() )
         GUI->UpdateDeviceProperties_Timer.Start();

      GUI->MountProperties_Control.Enable();
      GUI->MountAlignment_Control.Enable();
      GUI->Slew_Control.Enable();
      GUI->MountGoTo_Control.Enable();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

INDIMountInterface::GUIData::GUIData( INDIMountInterface& w )
{
   const String infoLabelStyleSheet =
      "QLabel {"
         "font-family: Hack, DejaVu Sans Mono, Monospace;"
      "}";

   const String buttonStyleSheet1 =
      "QPushButton {"
         "text-align: left;"
         "min-width: 6em;"
         "max-width: 6em;"
      "}";

   const String buttonStyleSheet2 =
      "QPushButton {"
         "text-align: left;"
         "min-width: 8em;"
         "max-width: 8em;"
      "}";

   const String moveButtonStyleSheet = w.ScaledStyleSheet(
      "QToolButton, QToolButton:hover, QToolButton:pressed {"
         "border-image: none;"
         "border: 1px solid gray;"
         "border-radius: 4px;"
      "}" );

   int emWidth = w.Font().Width( 'm' );
   int labelWidth1 = w.Font().Width( "Local Sidereal Time:" ) + emWidth;
   int editWidth1 = RoundInt( 4.25 * emWidth );
   int editWidth2 = RoundInt( 5.25 * emWidth );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   ServerParameters_SectionBar.SetTitle( "Device Properties" );
   ServerParameters_SectionBar.SetSection( ServerParameters_Control );

   MountDevice_Label.SetText( "Indigo Mount device:" );
   MountDevice_Label.SetToolTip( "<p>Select an Indigo Mount device.</p>" );
   MountDevice_Label.SetMinWidth( labelWidth1 );
   MountDevice_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   MountDeviceConfig_ToolButton.SetIcon( w.ScaledResource( ":/icons/wrench.png" ) );
   MountDeviceConfig_ToolButton.SetScaledFixedSize( 22, 22 );
   MountDeviceConfig_ToolButton.SetToolTip( "<p>Configure Indigo mount device</p>" );

   MountDeviceConfig_ToolButton.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountDevice_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDIMountInterface::e_ItemSelected, w );

   MountDevice_Sizer.SetSpacing( 4 );
   MountDevice_Sizer.Add( MountDevice_Label );
   MountDevice_Sizer.Add( MountDevice_Combo, 100 );
   MountDevice_Sizer.Add( MountDeviceConfig_ToolButton );

   LST_Label.SetText( "Local Sidereal Time:" );
   LST_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   LST_Label.SetFixedWidth( labelWidth1 );

   LST_Value_Label.SetStyleSheet( infoLabelStyleSheet );
   LST_Value_Label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   MountLST_Sizer.SetSpacing( 4 );
   MountLST_Sizer.Add( LST_Label );
   MountLST_Sizer.Add( LST_Value_Label, 100 );

   RA_Label.SetText( "Right Ascension:" );
   RA_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   RA_Label.SetFixedWidth( labelWidth1 );

   RA_Value_Label.SetStyleSheet( infoLabelStyleSheet );
   RA_Value_Label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   MountEQRA_Sizer.SetSpacing( 4 );
   MountEQRA_Sizer.Add( RA_Label );
   MountEQRA_Sizer.Add( RA_Value_Label, 100 );

   Dec_Label.SetText( "Declination:" );
   Dec_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   Dec_Label.SetFixedWidth( labelWidth1 );

   Dec_Value_Label.SetStyleSheet( infoLabelStyleSheet );
   Dec_Value_Label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   MountEQDec_Sizer.SetSpacing( 4 );
   MountEQDec_Sizer.Add( Dec_Label );
   MountEQDec_Sizer.Add( Dec_Value_Label, 100 );

   AZ_Label.SetText( "Azimuth:" );
   AZ_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   AZ_Label.SetFixedWidth( labelWidth1 );

   AZ_Value_Label.SetStyleSheet( infoLabelStyleSheet );
   AZ_Value_Label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   MountHZAZ_Sizer.SetSpacing( 4 );
   MountHZAZ_Sizer.Add( AZ_Label );
   MountHZAZ_Sizer.Add( AZ_Value_Label, 100 );

   ALT_Label.SetText( "Altitude:" );
   ALT_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   ALT_Label.SetFixedWidth( labelWidth1 );

   ALT_Value_Label.SetStyleSheet( infoLabelStyleSheet );
   ALT_Value_Label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   MountHZALT_Sizer.SetSpacing( 4 );
   MountHZALT_Sizer.Add( ALT_Label );
   MountHZALT_Sizer.Add( ALT_Value_Label, 100 );

   MountProperties_Sizer.SetSpacing( 4 );
   MountProperties_Sizer.Add( MountLST_Sizer );
   MountProperties_Sizer.Add( MountEQRA_Sizer );
   MountProperties_Sizer.Add( MountEQDec_Sizer );
   MountProperties_Sizer.Add( MountHZAZ_Sizer );
   MountProperties_Sizer.Add( MountHZALT_Sizer );

   MountProperties_Control.SetSizer( MountProperties_Sizer );

   ServerParameters_Sizer.SetSpacing( 4 );
   ServerParameters_Sizer.Add( MountDevice_Sizer );
   ServerParameters_Sizer.Add( MountProperties_Control );

   ServerParameters_Control.SetSizer( ServerParameters_Sizer );

   //
   MountAlignment_SectionBar.SetTitle( "Pointing Models" );
   MountAlignment_SectionBar.SetSection( MountAlignment_Control );

   const char* alignmentfileToolTipText =
      "<p>Path to a file with metadata for a telescope pointing model.</p>";

   AlignmentFile_Label.SetText( "Pointing model:" );
   AlignmentFile_Label.SetToolTip( alignmentfileToolTipText );
   AlignmentFile_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   AlignmentFile_Label.SetFixedWidth( labelWidth1 );

   AlignmentFile_Edit.SetToolTip( alignmentfileToolTipText );
   AlignmentFile_Edit.SetReadOnly();

   AlignmentFile_ToolButton.SetIcon( w.ScaledResource( ":/icons/select-file.png" ) );
   AlignmentFile_ToolButton.SetScaledFixedSize( 22, 22 );
   AlignmentFile_ToolButton.SetToolTip( "<p>Select the ponting model file:</p>" );
   AlignmentFile_ToolButton.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountAlignmentFile_Sizer.SetSpacing( 4 );
   MountAlignmentFile_Sizer.Add( AlignmentFile_Label );
   MountAlignmentFile_Sizer.Add( AlignmentFile_Edit, 100 );
   MountAlignmentFile_Sizer.AddSpacing( 4 );
   MountAlignmentFile_Sizer.Add( AlignmentFile_ToolButton );

   const char* alignmentConfigToolTipText = "<p>Configure and create a new pointing model.</p>";

   MountAligmentModelConfig_Button.SetText( "Configure" );
   MountAligmentModelConfig_Button.SetIcon( w.ScaledResource( ":/icons/wrench.png" ) );
   MountAligmentModelConfig_Button.SetToolTip( alignmentConfigToolTipText );
   MountAligmentModelConfig_Button.SetStyleSheet( buttonStyleSheet1 );
   MountAligmentModelConfig_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountAligmentModelFit_Button.SetText( "Create Model" );
   MountAligmentModelFit_Button.SetIcon( w.ScaledResource( ":/icons/gear.png" ) );
   MountAligmentModelFit_Button.SetToolTip( alignmentConfigToolTipText );
   MountAligmentModelFit_Button.SetStyleSheet( buttonStyleSheet2 );
   MountAligmentModelFit_Button.Disable();
   MountAligmentModelFit_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   const char* syncdatafileToolTipText = "<p>Shows the table of sync data points.</p>";

   SyncDataList_Button.SetText( "Data List" );
   SyncDataList_Button.SetIcon( w.ScaledResource( ":/icons/list.png" ) );
   SyncDataList_Button.SetToolTip( syncdatafileToolTipText );
   SyncDataList_Button.SetStyleSheet( buttonStyleSheet1 );
   SyncDataList_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountAlignmentConfig_Sizer.SetSpacing( 8 );
   MountAlignmentConfig_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   MountAlignmentConfig_Sizer.Add( MountAligmentModelConfig_Button );
   MountAlignmentConfig_Sizer.Add( MountAligmentModelFit_Button );
   MountAlignmentConfig_Sizer.Add( SyncDataList_Button );
   MountAlignmentConfig_Sizer.AddStretch();

   MountAlignmentCorrection_CheckBox.SetText( "Apply pointing corrections" );
   MountAlignmentCorrection_CheckBox.SetToolTip( "<p>Compute and apply telescope pointing corrections to target coordinates.</p>" );

   MountAlignmentPlotResiduals_CheckBox.SetText( "Plot residuals" );
   //MountAlignmentPlotResiduals_CheckBox.SetToolTip("<p>Plot residuals.</p>");

   MountAlignmentCorrection_Sizer.SetSpacing( 8 );
   MountAlignmentCorrection_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   MountAlignmentCorrection_Sizer.Add( MountAlignmentCorrection_CheckBox );
   MountAlignmentCorrection_Sizer.Add( MountAlignmentPlotResiduals_CheckBox );
   MountAlignmentCorrection_Sizer.AddStretch();

   MountAlignment_Sizer.SetSpacing( 8 );
   MountAlignment_Sizer.Add( MountAlignmentFile_Sizer );
   MountAlignment_Sizer.Add( MountAlignmentConfig_Sizer );
   MountAlignment_Sizer.Add( MountAlignmentCorrection_Sizer );

   MountAlignment_Control.SetSizer( MountAlignment_Sizer );

   //

   MountGoTo_SectionBar.SetTitle( "GoTo" );
   MountGoTo_SectionBar.SetSection( MountGoTo_Control );

   TargetRA_Label.SetText( "Right Ascension:" );
   TargetRA_Label.SetToolTip( "<p>Target object position in equatorial coordinates: Right Ascension coordinate (h:m:s)</p>" );
   TargetRA_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   TargetRA_Label.SetFixedWidth( labelWidth1 );

   TargetRA_H_SpinBox.SetRange( 0, 23 );
   TargetRA_H_SpinBox.SetFixedWidth( editWidth1 );

   TargetRA_M_SpinBox.SetRange( 0, 59 );
   TargetRA_M_SpinBox.SetFixedWidth( editWidth1 );

   TargetRA_S_NumericEdit.SetReal();
   TargetRA_S_NumericEdit.SetPrecision( 3 );
   TargetRA_S_NumericEdit.EnableFixedPrecision();
   TargetRA_S_NumericEdit.SetRange( 0, 59.999 );
   TargetRA_S_NumericEdit.label.Hide();
   TargetRA_S_NumericEdit.edit.SetFixedWidth( editWidth2 );

   MountTargetRA_Sizer.SetSpacing( 4 );
   MountTargetRA_Sizer.Add( TargetRA_Label );
   MountTargetRA_Sizer.Add( TargetRA_H_SpinBox );
   MountTargetRA_Sizer.Add( TargetRA_M_SpinBox );
   MountTargetRA_Sizer.Add( TargetRA_S_NumericEdit );
   MountTargetRA_Sizer.AddStretch();

   TargetDec_Label.SetText( "Declination:" );
   TargetDec_Label.SetToolTip( "<p>Target object position in equatorial coordinates: Declination coordinate (d:m:s)</p>" );
   TargetDec_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   TargetDec_Label.SetFixedWidth( labelWidth1 );

   TargetDec_H_SpinBox.SetRange( 0, 90 );
   TargetDec_H_SpinBox.SetFixedWidth( editWidth1 );

   TargetDec_M_SpinBox.SetRange( 0, 59 );
   TargetDec_M_SpinBox.SetFixedWidth( editWidth1 );

   TargetDec_S_NumericEdit.SetReal();
   TargetDec_S_NumericEdit.SetPrecision( 2 );
   TargetDec_S_NumericEdit.EnableFixedPrecision();
   TargetDec_S_NumericEdit.SetRange( 0, 59.99 );
   TargetDec_S_NumericEdit.label.Hide();
   TargetDec_S_NumericEdit.edit.SetFixedWidth( editWidth2 );

   MountTargetDECIsSouth_CheckBox.SetText( "South" );
   MountTargetDECIsSouth_CheckBox.SetToolTip( "<p>When checked, declination is negative (Southern hemisphere).</p>" );

   MountTargetDec_Sizer.SetSpacing( 4 );
   MountTargetDec_Sizer.Add( TargetDec_Label );
   MountTargetDec_Sizer.Add( TargetDec_H_SpinBox );
   MountTargetDec_Sizer.Add( TargetDec_M_SpinBox );
   MountTargetDec_Sizer.Add( TargetDec_S_NumericEdit );
   MountTargetDec_Sizer.Add( MountTargetDECIsSouth_CheckBox );
   MountTargetDec_Sizer.AddStretch();

   MountSearch_Button.SetText( "Search" );
   MountSearch_Button.SetIcon( w.ScaledResource( ":/icons/find.png" ) );
   MountSearch_Button.SetStyleSheet( buttonStyleSheet1 );
   MountSearch_Button.SetToolTip( "<p>Open the Online Coordinate Search dialog.</p>" );
   MountSearch_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountPlanets_Button.SetText( "Planets" );
   MountPlanets_Button.SetIcon( w.ScaledResource( ":/planets/saturn.png" ) );
   MountPlanets_Button.SetStyleSheet( buttonStyleSheet1 );
   MountPlanets_Button.SetToolTip( "<p>Open the Planet Coordinates dialog.</p>" );
   MountPlanets_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountAsteroids_Button.SetText( "Asteroids" );
   MountAsteroids_Button.SetIcon( w.ScaledResource( ":/planets/asteroid.png" ) );
   MountAsteroids_Button.SetStyleSheet( buttonStyleSheet1 );
   MountAsteroids_Button.SetToolTip( "<p>Open the Asteroid Coordinates dialog.</p>" );
   MountAsteroids_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountSearch_Sizer.SetSpacing( 8 );
   MountSearch_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   MountSearch_Sizer.Add( MountSearch_Button );
   MountSearch_Sizer.Add( MountPlanets_Button );
   MountSearch_Sizer.Add( MountAsteroids_Button );
   MountSearch_Sizer.AddStretch();

   MountGoToStart_Button.SetText( "GoTo" );
   MountGoToStart_Button.SetIcon( w.ScaledResource( ":/icons/play.png" ) );
   MountGoToStart_Button.SetStyleSheet( buttonStyleSheet1 );
   MountGoToStart_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountSync_Button.SetText( "Sync" );
   MountSync_Button.SetIcon( w.ScaledResource( ":/icons/track.png" ) );
   MountSync_Button.SetStyleSheet( buttonStyleSheet1 );
   MountSync_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountPark_Button.SetIcon( w.ScaledResource( ":/icons/move-center.png" ) );
   MountPark_Button.SetStyleSheet( buttonStyleSheet1 );
   MountPark_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountGoToStart_Sizer.SetSpacing( 8 );
   MountGoToStart_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   MountGoToStart_Sizer.Add( MountGoToStart_Button );
   MountGoToStart_Sizer.Add( MountSync_Button );
   MountGoToStart_Sizer.Add( MountPark_Button );
   MountGoToStart_Sizer.AddStretch();

   MountGoToCancel_Button.SetText( "Cancel" );
   MountGoToCancel_Button.SetIcon( w.ScaledResource( ":/icons/cancel.png" ) );
   MountGoToCancel_Button.SetStyleSheet( buttonStyleSheet1 );
   MountGoToCancel_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );
   MountGoToCancel_Button.Disable();

   MountGoToCancel_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   MountGoToCancel_Sizer.Add( MountGoToCancel_Button );
   MountGoToCancel_Sizer.AddStretch();

   MountGoToInfo_Label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   MountGoTo_Sizer.SetSpacing( 8 );
   MountGoTo_Sizer.Add( MountTargetRA_Sizer );
   MountGoTo_Sizer.Add( MountTargetDec_Sizer );
   MountGoTo_Sizer.Add( MountSearch_Sizer );
   MountGoTo_Sizer.Add( MountGoToStart_Sizer );
   MountGoTo_Sizer.Add( MountGoToCancel_Sizer );
   MountGoTo_Sizer.Add( MountGoToInfo_Label );

   MountGoTo_Control.SetSizer( MountGoTo_Sizer );

   //

   Slew_SectionBar.SetTitle( "Slew" );
   Slew_SectionBar.SetSection( Slew_Control );

   SlewTopLeft_Button.SetIcon( w.ScaledResource( ":/icons/move-left-up.png" ) );
   SlewTopLeft_Button.SetScaledFixedSize( 32, 32 );
   SlewTopLeft_Button.SetToolTip( "Northwest" );
   SlewTopLeft_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewTopLeft_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewTopLeft_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewTop_Button.SetIcon( w.ScaledResource( ":/icons/up.png" ) );
   SlewTop_Button.SetScaledFixedSize( 32, 32 );
   SlewTop_Button.SetToolTip( "North" );
   SlewTop_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewTop_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewTop_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewTopRight_Button.SetIcon( w.ScaledResource( ":/icons/move-right-up.png" ) );
   SlewTopRight_Button.SetScaledFixedSize( 32, 32 );
   SlewTopRight_Button.SetToolTip( "Northeast" );
   SlewTopRight_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewTopRight_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewTopRight_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewTopRow_Sizer.SetSpacing( 8 );
   SlewTopRow_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SlewTopRow_Sizer.Add( SlewTopLeft_Button );
   SlewTopRow_Sizer.Add( SlewTop_Button );
   SlewTopRow_Sizer.Add( SlewTopRight_Button );

   SlewLeft_Button.SetIcon( w.ScaledResource( ":/icons/left.png" ) );
   SlewLeft_Button.SetScaledFixedSize( 32, 32 );
   SlewLeft_Button.SetToolTip( "West" );
   SlewLeft_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewLeft_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewLeft_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewStop_Button.SetIcon( w.ScaledResource( ":/icons/stop.png" ) );
   SlewStop_Button.SetScaledFixedSize( 32, 32 );
   SlewStop_Button.SetToolTip( "Stop" );
   SlewStop_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewStop_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewStop_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewRight_Button.SetIcon( w.ScaledResource( ":/icons/right.png" ) );
   SlewRight_Button.SetScaledFixedSize( 32, 32 );
   SlewRight_Button.SetToolTip( "East" );
   SlewRight_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewRight_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewRight_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewMiddleRow_Sizer.SetSpacing( 8 );
   SlewMiddleRow_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SlewMiddleRow_Sizer.Add( SlewLeft_Button );
   SlewMiddleRow_Sizer.Add( SlewStop_Button );
   SlewMiddleRow_Sizer.Add( SlewRight_Button );

   SlewBottomLeft_Button.SetIcon( w.ScaledResource( ":/icons/move-left-down.png" ) );
   SlewBottomLeft_Button.SetScaledFixedSize( 32, 32 );
   SlewBottomLeft_Button.SetToolTip( "Southwest" );
   SlewBottomLeft_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewBottomLeft_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewBottomLeft_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewBottom_Button.SetIcon( w.ScaledResource( ":/icons/down.png" ) );
   SlewBottom_Button.SetScaledFixedSize( 32, 32 );
   SlewBottom_Button.SetToolTip( "South" );
   SlewBottom_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewBottom_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewBottom_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewBottomRight_Button.SetIcon( w.ScaledResource( ":/icons/move-right-down.png" ) );
   SlewBottomRight_Button.SetScaledFixedSize( 32, 32 );
   SlewBottomRight_Button.SetToolTip( "Southeast" );
   SlewBottomRight_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewBottomRight_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewBottomRight_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewBottomRow_Sizer.SetSpacing( 8 );
   SlewBottomRow_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SlewBottomRow_Sizer.Add( SlewBottomLeft_Button );
   SlewBottomRow_Sizer.Add( SlewBottom_Button );
   SlewBottomRow_Sizer.Add( SlewBottomRight_Button );

   SlewLeft_Sizer.SetSpacing( 8 );
   SlewLeft_Sizer.Add( SlewTopRow_Sizer );
   SlewLeft_Sizer.Add( SlewMiddleRow_Sizer );
   SlewLeft_Sizer.Add( SlewBottomRow_Sizer );

   SlewLeft_Control.SetSizer( SlewLeft_Sizer );

   const char* slewSpeedTooltipText =
      "<p>Predefined slew rates, in ascending speed order: Guide, Centering, Find, Maximum.</p>"
      "<p>There might be more device-specific slew rates, which can be selected with Indigo Device Controller.</p>";

   SlewSpeed_Label.SetText( "Slew speed:" );
   SlewSpeed_Label.SetToolTip( slewSpeedTooltipText );
   SlewSpeed_Label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   SlewSpeed_ComboBox.AddItem( "Guide" );
   SlewSpeed_ComboBox.AddItem( "Centering" );
   SlewSpeed_ComboBox.AddItem( "Find" );
   SlewSpeed_ComboBox.AddItem( "Maximum" );
   SlewSpeed_ComboBox.SetToolTip( slewSpeedTooltipText );
   SlewSpeed_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&INDIMountInterface::e_ItemSelected, w );

   SlewRight_Sizer.AddStretch();
   SlewRight_Sizer.Add( SlewSpeed_Label );
   SlewRight_Sizer.AddSpacing( 2 );
   SlewRight_Sizer.Add( SlewSpeed_ComboBox );
   SlewRight_Sizer.AddStretch();

   Slew_Sizer.SetSpacing( 32 );
   Slew_Sizer.Add( SlewLeft_Control );
   Slew_Sizer.Add( SlewRight_Sizer );
   Slew_Sizer.AddStretch();

   Slew_Control.SetSizer( Slew_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( ServerParameters_SectionBar );
   Global_Sizer.Add( ServerParameters_Control );
   Global_Sizer.Add( MountAlignment_SectionBar );
   Global_Sizer.Add( MountAlignment_Control );
   Global_Sizer.Add( MountGoTo_SectionBar );
   Global_Sizer.Add( MountGoTo_Control );
   Global_Sizer.Add( Slew_SectionBar );
   Global_Sizer.Add( Slew_Control );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedHeight();

   UpdateDeviceList_Timer.SetInterval( 0.5 );
   UpdateDeviceList_Timer.SetPeriodic( true );
   UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&INDIMountInterface::e_Timer, w );
   UpdateDeviceList_Timer.Start();

   UpdateDeviceProperties_Timer.SetInterval( 1 );
   UpdateDeviceProperties_Timer.SetPeriodic( true );
   UpdateDeviceProperties_Timer.OnTimer( (Timer::timer_event_handler)&INDIMountInterface::e_Timer, w );
}

// ----------------------------------------------------------------------------

void INDIMountInterface::e_Timer( Timer& sender )
{
   if ( sender == GUI->UpdateDeviceList_Timer )
   {
      GUI->MountDevice_Combo.Clear();
      if ( INDIClient::HasClient() )
      {
         INDIClient* indi = INDIClient::TheClient();
         ExclConstDeviceList x = indi->ConstDeviceList();
         const INDIDeviceListItemArray& devices( x );
         if ( devices.IsEmpty() )
            GUI->MountDevice_Combo.AddItem( "<No Device Available>" );
         else
         {
            GUI->MountDevice_Combo.AddItem( String() );

            for ( auto device : devices )
               if ( indi->HasPropertyItem( device.DeviceName, MOUNT_EQUATORIAL_COORDINATES_PROPERTY_NAME, MOUNT_EQUATORIAL_COORDINATES_RA_ITEM_NAME ) ) // is this a mount device?
                  GUI->MountDevice_Combo.AddItem( device.DeviceName );

            GUI->MountDevice_Combo.SetItemText( 0,
               ( GUI->MountDevice_Combo.NumberOfItems() > 1 ) ? "<No Device Selected>" : "<No Mount Device Available>" );

            int i = Max( 0, GUI->MountDevice_Combo.FindItem( m_device ) );
            GUI->MountDevice_Combo.SetCurrentItem( i );
            if ( i > 0 )
               goto __device_found;
         }
      }
      else
         GUI->MountDevice_Combo.AddItem( "<Indigo Server Not Connected>" );

      m_device.Clear();

__device_found:

      UpdateControls();
   }
   else if ( sender == GUI->UpdateDeviceProperties_Timer )
   {
      if ( !INDIClient::HasClient() )
         return;

      INDIClient* indi = INDIClient::TheClient();
      INDIPropertyListItem mountProp;

      double time_lst = 0;
      if ( indi->GetPropertyItem( m_device, MOUNT_LST_TIME_PROPERTY_NAME, MOUNT_LST_TIME_ITEM_NAME, mountProp, false /*formatted*/ ) )
      {
         time_lst = mountProp.PropertyValue.ToDouble();
      }
      else
      {
         time_lst = LocalApparentSiderialTime( GeographicLongitude() );
      }
      GUI->LST_Value_Label.SetText( String::ToSexagesimal( time_lst,
         SexagesimalConversionOptions( 3 /*items*/, 3 /*precision*/, false /*sign*/, 3 /*width*/ ) ) );

      double coord_ra = 0;
      if ( indi->GetPropertyItem( m_device, MOUNT_EQUATORIAL_COORDINATES_PROPERTY_NAME, MOUNT_EQUATORIAL_COORDINATES_RA_ITEM_NAME, mountProp, false /*formatted*/ ) )
      {
         GUI->RA_Value_Label.SetText( String::ToSexagesimal( mountProp.PropertyValue.ToDouble(),
            SexagesimalConversionOptions( 3 /*items*/, 3 /*precision*/, false /*sign*/, 3 /*width*/, ' ' /*separator*/ ) ) );
         coord_ra = mountProp.PropertyValue.ToDouble();
      }
      else
         GUI->RA_Value_Label.Clear();

      if ( indi->GetPropertyItem( m_device, MOUNT_EQUATORIAL_COORDINATES_PROPERTY_NAME, MOUNT_EQUATORIAL_COORDINATES_DEC_ITEM_NAME, mountProp, false /*formatted*/ ) )
         GUI->Dec_Value_Label.SetText( String::ToSexagesimal( mountProp.PropertyValue.ToDouble(),
            SexagesimalConversionOptions( 3 /*items*/, 2 /*precision*/, true /*sign*/, 3 /*width*/, ' ' /*separator*/ ) ) );
      else
         GUI->Dec_Value_Label.Clear();

      static const char* indiPierSides[] = { MOUNT_SIDE_OF_PIER_WEST_ITEM_NAME, MOUNT_SIDE_OF_PIER_EAST_ITEM_NAME };
      bool deviceHasPierSideProperty = true;
      for ( size_type i = 0; i < ItemsInArray( indiPierSides ); ++i )
      {
         if ( indi->GetPropertyItem( m_device, MOUNT_SIDE_OF_PIER_PROPERTY_NAME, indiPierSides[i], mountProp ) )
         {
            if ( mountProp.PropertyValue == "ON" )
            {
               m_pierSide = i;
               break;
            }
         }
         else
         {
            deviceHasPierSideProperty = false;
            break;
         }
      }

      if ( !deviceHasPierSideProperty )
      {
         // pier side fallback
         // If the Indigo mount device does not support the TELESCOPE_PIER_SIDE property, compute the pierside from hour angle
         double hourAngle = AlignmentModel::RangeShiftHourAngle( time_lst - coord_ra );
         m_pierSide = hourAngle <= 0 ? IMCPierSide::West : IMCPierSide::East;
      }

      if ( indi->GetPropertyItem( m_device, MOUNT_HORIZONTAL_COORDINATES_PROPERTY_NAME, MOUNT_HORIZONTAL_COORDINATES_ALT_ITEM_NAME, mountProp, false /*formatted*/ ) )
         GUI->ALT_Value_Label.SetText( String::ToSexagesimal( mountProp.PropertyValue.ToDouble(),
            SexagesimalConversionOptions( 3 /*items*/, 2 /*precision*/, true /*sign*/, 3 /*width*/ ) ) );
      else
         GUI->ALT_Value_Label.Clear();

      if ( indi->GetPropertyItem( m_device, MOUNT_HORIZONTAL_COORDINATES_PROPERTY_NAME, MOUNT_HORIZONTAL_COORDINATES_AZ_ITEM_NAME, mountProp, false /*formatted*/ ) )
         GUI->AZ_Value_Label.SetText( String::ToSexagesimal( mountProp.PropertyValue.ToDouble(),
            SexagesimalConversionOptions( 3 /*items*/, 2 /*precision*/, true /*sign*/, 3 /*width*/ ) ) );
      else
         GUI->AZ_Value_Label.Clear();

      bool foundSlewRate = false;
      static const char* indiSlewRates[] = { MOUNT_SLEW_RATE_GUIDE_ITEM_NAME, MOUNT_SLEW_RATE_CENTERING_ITEM_NAME, MOUNT_SLEW_RATE_FIND_ITEM_NAME, MOUNT_SLEW_RATE_MAX_ITEM_NAME };
      for ( size_type i = 0; i < ItemsInArray( indiSlewRates ); ++i )
         if ( indi->GetPropertyItem( m_device, MOUNT_SLEW_RATE_PROPERTY_NAME, indiSlewRates[i], mountProp ) )
            if ( mountProp.PropertyValue == "ON" )
            {
               foundSlewRate = true;
               GUI->SlewSpeed_ComboBox.SetCurrentItem( i );
               break;
            }
      GUI->SlewSpeed_Label.Enable( foundSlewRate );
      GUI->SlewSpeed_ComboBox.Enable( foundSlewRate );

      if ( indi->GetPropertyItem( m_device, GEOGRAPHIC_COORDINATES_PROPERTY_NAME, GEOGRAPHIC_COORDINATES_LATITUDE_ITEM_NAME, mountProp, false /*formatted*/ ) )
         m_geoLatitude = mountProp.PropertyValue.ToDouble();
      else
         m_geoLatitude = 0;

      if ( indi->GetPropertyItem( m_device, GEOGRAPHIC_COORDINATES_PROPERTY_NAME, GEOGRAPHIC_COORDINATES_LONGITUDE_ITEM_NAME, mountProp, false /*formatted*/ ) )
         m_geoLongitude = mountProp.PropertyValue.ToDouble();
      else
         m_geoLongitude = 0;

      if ( indi->GetPropertyItem( m_device, GEOGRAPHIC_COORDINATES_PROPERTY_NAME, GEOGRAPHIC_COORDINATES_ELEVATION_ITEM_NAME, mountProp, false /*formatted*/ ) )
         m_geoHeight = mountProp.PropertyValue.ToDouble();
      else
         m_geoHeight = 0;

      if ( indi->GetPropertyItem( m_device, UTC_TIME_PROPERTY_NAME, UTC_TIME_ITEM_NAME, mountProp, false /*formatted*/ ) )
         m_utcTime = mountProp.PropertyValue;

      if ( indi->GetPropertyItem( m_device, UTC_TIME_PROPERTY_NAME, UTC_OFFSET_ITEM_NAME, mountProp, false /*formatted*/ ) )
         m_utcOffset = mountProp.PropertyValue.ToDouble();
      else
         m_utcOffset = 0;

      if ( indi->GetPropertyItem( m_device, MOUNT_PARK_PROPERTY_NAME, MOUNT_PARK_PARKED_ITEM_NAME, mountProp, false /*formatted*/ ) )
         if ( mountProp.PropertyValue == "ON" )
         {
            GUI->MountPark_Button.SetText( "Unpark" );
         }
         else
         {
            GUI->MountPark_Button.SetText( "Park" );
         }

      if ( indi->GetPropertyItem( m_device, "TELESCOPE_INFO", "TELESCOPE_APERTURE", mountProp, false /*formatted*/ ) )
         m_telescopeAperture = mountProp.PropertyValue.ToDouble();
      else
         m_telescopeAperture = 0;

      if ( indi->GetPropertyItem( m_device, "TELESCOPE_INFO", "TELESCOPE_FOCAL_LENGTH", mountProp, false /*formatted*/ ) )
         m_telescopeFocalLength = mountProp.PropertyValue.ToDouble();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------



void INDIMountInterface::e_Click( Button& sender, bool checked )
{
   if ( !INDIClient::HasClient() )
      return;

   if ( sender == GUI->MountGoToStart_Button )
   {
      INDIMountInterfaceExecution( this ).Perform( IMCCommand::GoTo );
   }
   else if ( sender == GUI->MountSync_Button )
   {
      INDIMountInterfaceExecution( this ).Perform( IMCCommand::Sync );
   }
   else if ( sender == GUI->MountPark_Button )
   {
      if ( GUI->MountPark_Button.Text() == "Park" )
      {
         INDIMountInterfaceExecution( this ).Perform( IMCCommand::ParkDefault );
      }
      else
      {
         INDIMountInterfaceExecution( this ).Perform( IMCCommand::Unpark );
      }
   }
   else if ( sender == GUI->MountGoToCancel_Button )
   {
      if ( m_execution != nullptr )
         m_execution->Abort();
   }
   else if ( sender == GUI->MountSearch_Button )
   {
      if ( m_searchDialog.IsNull() )
         m_searchDialog = new CoordinateSearchDialog( *this );
      if ( m_searchDialog->Execute() )
         if ( m_searchDialog->HasValidCoordinates() )
         {
            int sign, s1, s2;
            double s3;
            DecimalToSexagesimal( sign, s1, s2, s3, m_searchDialog->RA() );
            GUI->TargetRA_H_SpinBox.SetValue( s1 );
            GUI->TargetRA_M_SpinBox.SetValue( s2 );
            GUI->TargetRA_S_NumericEdit.SetValue( s3 );

            DecimalToSexagesimal( sign, s1, s2, s3, m_searchDialog->Dec() );
            GUI->TargetDec_H_SpinBox.SetValue( s1 );
            GUI->TargetDec_M_SpinBox.SetValue( s2 );
            GUI->TargetDec_S_NumericEdit.SetValue( s3 );
            GUI->MountTargetDECIsSouth_CheckBox.SetChecked( sign < 0 );

            if ( m_searchDialog->GoToTarget() )
               INDIMountInterfaceExecution( this ).Perform( IMCCommand::GoTo );
         }
   }
   else if ( sender == GUI->MountPlanets_Button )
   {
      if ( m_planetDialog.IsNull() )
         m_planetDialog = new PlanetSearchDialog( *this );
      if ( m_planetDialog->Execute() )
      {
         int sign, s1, s2;
         double s3;
         DecimalToSexagesimal( sign, s1, s2, s3, m_planetDialog->RA() );
         GUI->TargetRA_H_SpinBox.SetValue( s1 );
         GUI->TargetRA_M_SpinBox.SetValue( s2 );
         GUI->TargetRA_S_NumericEdit.SetValue( s3 );

         DecimalToSexagesimal( sign, s1, s2, s3, m_planetDialog->Dec() );
         GUI->TargetDec_H_SpinBox.SetValue( s1 );
         GUI->TargetDec_M_SpinBox.SetValue( s2 );
         GUI->TargetDec_S_NumericEdit.SetValue( s3 );
         GUI->MountTargetDECIsSouth_CheckBox.SetChecked( sign < 0 );

         if ( m_planetDialog->GoToTarget() )
            INDIMountInterfaceExecution( this ).Perform( IMCCommand::GoTo );
      }
   }
   else if ( sender == GUI->MountAsteroids_Button )
   {
      if ( m_asteroidDialog.IsNull() )
         m_asteroidDialog = new AsteroidSearchDialog( *this );
      if ( m_asteroidDialog->Execute() )
      {
         int sign, s1, s2;
         double s3;
         DecimalToSexagesimal( sign, s1, s2, s3, m_asteroidDialog->RA() );
         GUI->TargetRA_H_SpinBox.SetValue( s1 );
         GUI->TargetRA_M_SpinBox.SetValue( s2 );
         GUI->TargetRA_S_NumericEdit.SetValue( s3 );

         DecimalToSexagesimal( sign, s1, s2, s3, m_asteroidDialog->Dec() );
         GUI->TargetDec_H_SpinBox.SetValue( s1 );
         GUI->TargetDec_M_SpinBox.SetValue( s2 );
         GUI->TargetDec_S_NumericEdit.SetValue( s3 );
         GUI->MountTargetDECIsSouth_CheckBox.SetChecked( sign < 0 );

         if ( m_asteroidDialog->GoToTarget() )
            INDIMountInterfaceExecution( this ).Perform( IMCCommand::GoTo );
      }
   }
   else if ( sender == GUI->SlewStop_Button )
   {
      INDIClient::TheClient()->MaybeSendNewPropertyItem( m_device, MOUNT_ABORT_MOTION_PROPERTY_NAME, "INDI_SWITCH", MOUNT_ABORT_MOTION_ITEM_NAME, "ON", true /*async*/ );
   }
   else if ( sender == GUI->AlignmentFile_ToolButton )
   {
      SaveFileDialog f;
      FileFilter filter;
      filter.AddExtension( "xtpm" );
      f.SetFilter( filter );
      f.SetCaption( "IndigoMount: Select Alignment File" );
      if ( f.Execute() )
         GUI->AlignmentFile_Edit.SetText( f.FileName() );
   }
   else if ( sender == GUI->MountAligmentModelFit_Button )
   {
      INDIMountInterfaceExecution( this ).Perform( IMCCommand::FitPointingModel );

      if ( GUI->MountAlignmentPlotResiduals_CheckBox.IsChecked() )
      {
         AutoPointer<AlignmentModel> aModel;
         uint32 alignmentConfig = GUI->AlignmentConfigParameter();
         switch ( GUI->m_alignmentModelIndex )
         {
         case IMCAlignmentMethod::AnalyticalModel:
            aModel = GeneralAnalyticalPointingModel::Create( m_geoLatitude, alignmentConfig, GUI->m_modelBothPierSides );
            break;
         default:
            throw Error( "Internal error: AbstractINDIMountInterface: Unknown Pointing Model." );
         }

         aModel->ReadObject( GUI->AlignmentFile_Edit.Text().Trimmed() );
         plotAlignemtResiduals( aModel.Ptr() );
      }
   }
   else if ( sender == GUI->SyncDataList_Button )
   {
      AutoPointer<AlignmentModel> aModel = AlignmentModel::Create( GUI->AlignmentFile_Edit.Text() );
      Array<SyncDataPoint>& syncDataList = aModel->SyncDataPoints();
      AutoPointer<SyncDataListDialog> syncDataListDialog = new SyncDataListDialog( syncDataList );
      if ( syncDataListDialog->Execute() )
         aModel->WriteObject( GUI->AlignmentFile_Edit.Text().Trimmed() );
   }
   else if ( sender == GUI->MountAligmentModelConfig_Button )
   {
      if ( m_alignmentConfigDialog.IsNull() )
         m_alignmentConfigDialog = new AlignmentConfigDialog( *this );
      if ( m_alignmentConfigDialog->Execute() )
         GUI->MountAligmentModelFit_Button.Enable();
   }
   else if ( sender == GUI->MountDeviceConfig_ToolButton )
   {
      MountConfigDialog mountConfig( m_device,
                                     GeographicLatitude(),
                                     GeographicLongitude(),
                                     GeographicHeight(),
                                     UtcTime(),
                                     UtcOffset(),
                                     TelescopeFocalLength() );
      if ( mountConfig.Execute() && INDIClient::HasClient() )
      {
         m_telescopeFocalLength = mountConfig.getTelescopeFocalLength();
      }
   }
}

// ----------------------------------------------------------------------------

void INDIMountInterface::e_Press( Button& sender )
{
   if ( !INDIClient::HasClient() )
      return;

   if ( sender == GUI->SlewTopLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_NORTH_ITEM_NAME, "ON", true /*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_RA_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_WEST_ITEM_NAME, "ON", true /*async*/ );
   }
   else if ( sender == GUI->SlewTop_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_NORTH_ITEM_NAME, "ON", true /*async*/ );
   }
   if ( sender == GUI->SlewTopRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_NORTH_ITEM_NAME, "ON", true /*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_RA_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_EAST_ITEM_NAME, "ON", true /*async*/ );
   }
   else if ( sender == GUI->SlewLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_RA_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_WEST_ITEM_NAME, "ON", true /*async*/ );
   }
   else if ( sender == GUI->SlewRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_RA_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_EAST_ITEM_NAME, "ON", true /*async*/ );
   }
   else if ( sender == GUI->SlewBottomLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_SOUTH_ITEM_NAME, "ON", true /*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_RA_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_WEST_ITEM_NAME, "ON", true /*async*/ );
   }
   else if ( sender == GUI->SlewBottom_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_SOUTH_ITEM_NAME, "ON", true /*async*/ );
   }
   else if ( sender == GUI->SlewBottomRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_SOUTH_ITEM_NAME, "ON", true /*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_RA_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_EAST_ITEM_NAME, "ON", true /*async*/ );
   }
}

// ----------------------------------------------------------------------------

void INDIMountInterface::e_Release( Button& sender )
{
   if ( !INDIClient::HasClient() )
      return;

   if ( sender == GUI->SlewTopLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_NORTH_ITEM_NAME, "OFF", true /*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", MOUNT_MOTION_WEST_ITEM_NAME, "OFF", true /*async*/ );
   }
   else if ( sender == GUI->SlewTop_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", "MOTION_NORTH", "OFF", true /*async*/ );
   }
   if ( sender == GUI->SlewTopRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_NORTH_ITEM_NAME, "OFF", true /*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_RA_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_EAST_ITEM_NAME, "OFF", true /*async*/ );
   }
   else if ( sender == GUI->SlewLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_RA_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_WEST_ITEM_NAME, "OFF", true /*async*/ );
   }
   else if ( sender == GUI->SlewRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_RA_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_EAST_ITEM_NAME, "OFF", true /*async*/ );
   }
   else if ( sender == GUI->SlewBottomLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_SOUTH_ITEM_NAME, "OFF", true /*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_RA_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_WEST_ITEM_NAME, "OFF", true /*async*/ );
   }
   else if ( sender == GUI->SlewBottom_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_SOUTH_ITEM_NAME, "OFF", true /*async*/ );
   }
   else if ( sender == GUI->SlewBottomRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_DEC_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_SOUTH_ITEM_NAME, "OFF", true /*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, MOUNT_MOTION_RA_PROPERTY_NAME, "INDI_SWITCH", MOUNT_MOTION_EAST_ITEM_NAME, "OFF", true /*async*/ );
   }
}

// ----------------------------------------------------------------------------

void INDIMountInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( !INDIClient::HasClient() )
      return;

   if ( sender == GUI->MountDevice_Combo )
   {
      m_device = ( itemIndex > 0 ) ? sender.ItemText( itemIndex ).Trimmed() : String();
      UpdateControls();

      // get initial properties and unpark mount
      if ( !m_device.IsEmpty() )
      {
         INDIPropertyListItem item;

         // load configuration on server
         INDIClient::TheClient()->SendNewPropertyItem( m_device, CONFIG_PROPERTY_NAME, "INDI_SWITCH", CONFIG_LOAD_ITEM_NAME, "ON" );

         if ( INDIClient::TheClient()->GetPropertyTargetItem( m_device, MOUNT_EQUATORIAL_COORDINATES_PROPERTY_NAME, MOUNT_EQUATORIAL_COORDINATES_RA_ITEM_NAME, item, false /*formatted*/ ) )
         {
            int dum, h, m;
            double s;
            DecimalToSexagesimal( dum, h, m, s, item.PropertyTarget.ToDouble() );
            GUI->TargetRA_H_SpinBox.SetValue( h );
            GUI->TargetRA_M_SpinBox.SetValue( m );
            GUI->TargetRA_S_NumericEdit.SetValue( s );
         }

         if ( INDIClient::TheClient()->GetPropertyTargetItem( m_device, MOUNT_EQUATORIAL_COORDINATES_PROPERTY_NAME, MOUNT_EQUATORIAL_COORDINATES_DEC_ITEM_NAME, item, false /*formatted*/ ) )
         {
            int sign, d, m;
            double s;
            DecimalToSexagesimal( sign, d, m, s, item.PropertyTarget.ToDouble() );
            GUI->TargetDec_H_SpinBox.SetValue( d );
            GUI->TargetDec_M_SpinBox.SetValue( m );
            GUI->TargetDec_S_NumericEdit.SetValue( s );
            GUI->MountTargetDECIsSouth_CheckBox.SetChecked( sign < 0 );
         }
      }
   }
   else if ( sender == GUI->SlewSpeed_ComboBox )
   {
      INDIClient::TheClient()->MaybeSendNewPropertyItem( m_device, MOUNT_SLEW_RATE_PROPERTY_NAME, "INDI_SWITCH",
         INDIMountInstance::MountSlewRatePropertyString( itemIndex ), "ON", true /*async*/ );
   }
}

// ----------------------------------------------------------------------------

//set multiplot layout 2,2 title "Hourangle residuals in [arcmin]"
//set multiplot layout 2,2 title "Declination residuals in [arcmin]"
//unset multiplot

const char* RESIDUAL_GNUPLOT_TEMPLATE = R"(
set terminal svg enhanced size 800,600 enhanced background rgb 'white' font 'helvetica,12'
set ticslevel 0
set xzeroaxis
set ylabel "residual hourangle [arcmin]"
set xlabel "Hourangle [deg]"
set output '%s'
plot '%s' index 0 using 1:3 title "west" with points pointtype 5, '%s' index 1 using 1:3 title "east" with points pointtype 7
set xlabel "Declination [deg]"
set output '%s'
plot '%s' index 0 using 2:3 title "west" with points pointtype 5, '%s' index 1 using 2:3 title "east" with points pointtype 7
set xlabel "Hourangle [deg]"
set ylabel "Declination [deg]"
set zlabel "residual hourangle [arcmin]"
set output '%s'
splot '%s' index 0 using 1:2:3 title "west" with points pointtype 5, '%s' index 1 using 1:2:3 title "east" with points pointtype 7,0 notitle
set ylabel "residual declination [arcmin]"
set xlabel "Hourangle [deg]"
set output '%s'
plot '%s' index 0 using 1:4 title "west" with points pointtype 5, '%s' index 1 using 1:4 title "east" with points pointtype 7
set xlabel "Declination [arcmin]"
set output '%s'
plot '%s' index 0 using 2:4 title "west" with points pointtype 5, '%s' index 1 using 2:4 title "east" with points pointtype 7
set xlabel "Hourangle [deg]"
set ylabel "Declination [deg]"
set zlabel "residual hourangle [arcmin]"
set output '%s'
splot '%s' index 0 using 1:2:4 title "west" with points pointtype 5, '%s' index 1 using 1:2:4 title "east" with points pointtype 7,0 notitle
set terminal svg size 800,600 enhanced background rgb 'white' font 'helvetica,12'
set xrange[ %f:%f]
set yrange[ %f:%f]
unset border
set xlabel "residual hourangle [arcmin]"
set ylabel "residual declination [arcmin]"
set xtics axis nomirror
set ytics axis nomirror
set output '%s'
set yzeroaxis
set object 10 circle size 1 fs transparent border 1
set object 11 circle size 0.5 fs transparent border 2
set object 12 circle size 0.1 fs transparent border 3
plot '%s' index 0 using 3:4 title "west" with points pointtype 5, '%s' index 1 using 3:4 title "east" with points pointtype 7
)";

void INDIMountInterface::plotAlignemtResiduals( AlignmentModel* model )
{
   const Array<SyncDataPoint>& syncDatapointList = model->SyncDataPoints();
   Array<double> delHAs;
   Array<double> delDecs;

   // create residual data file
   String modelResidualDataPath = File::SystemTempDirectory();
   if ( !modelResidualDataPath.EndsWith( '/' ) )
      modelResidualDataPath << '/';
   modelResidualDataPath << "ModelResidual.dat";

   IsoString fileContent;

   // pier side is west or None
   const double ra_factor = 15 * 60;
   const double dec_factor = 60;
   for ( auto syncPoint : syncDatapointList )
   {
      if ( !syncPoint.enabled || syncPoint.pierSide == IMCPierSide::East )
         continue;

      double cel_ha = AlignmentModel::RangeShiftHourAngle( syncPoint.localSiderialTime - syncPoint.celestialRA );
      double tel_ha = AlignmentModel::RangeShiftHourAngle( syncPoint.localSiderialTime - syncPoint.telecopeRA );
      double cel_dec = syncPoint.celestialDEC;
      double del_ha = cel_ha - tel_ha;
      double del_dec = syncPoint.celestialDEC - syncPoint.telecopeDEC;

      // compute alignment correction
      double haCor = 0;
      double decCor = 0;
      model->Apply( haCor, decCor, cel_ha, cel_dec, IMCPierSide::West );
      double del_haCor = cel_ha - haCor;
      double del_decCor = cel_dec - decCor;
      double haResidual = ( del_ha - del_haCor ) * ra_factor;
      double decResidual = ( del_dec - del_decCor ) * dec_factor;
      fileContent.AppendFormat( "%f %f %f %f\n", cel_ha, cel_dec, haResidual, decResidual );
      delHAs << haResidual;
      delDecs << decResidual;
   }
   fileContent << '\n'
               << '\n';

   // pier side is east
   for ( auto syncPoint : syncDatapointList )
   {
      if ( !syncPoint.enabled || syncPoint.pierSide != IMCPierSide::East )
         continue;

      double cel_ha = AlignmentModel::RangeShiftHourAngle( syncPoint.localSiderialTime - syncPoint.celestialRA );
      double tel_ha = AlignmentModel::RangeShiftHourAngle( syncPoint.localSiderialTime - syncPoint.telecopeRA );
      double cel_dec = syncPoint.celestialDEC;
      double del_ha = cel_ha - tel_ha;
      double del_dec = syncPoint.celestialDEC - syncPoint.telecopeDEC;

      // compute alignment correction
      double haCor = 0;
      double decCor = 0;
      model->Apply( haCor, decCor, cel_ha, cel_dec, IMCPierSide::East );
      double del_haCor = cel_ha - haCor;
      double del_decCor = cel_dec - decCor;
      double haResidual = ( del_ha - del_haCor ) * ra_factor;
      double decResidual = ( del_dec - del_decCor ) * dec_factor;
      fileContent.AppendFormat( "%f %f %f %f\n", cel_ha, cel_dec, haResidual, decResidual );
      delHAs << haResidual;
      delDecs << decResidual;
   }

   File::WriteTextFile( modelResidualDataPath, fileContent );

   // now create gnuplot file
   String tmpFilePath = File::SystemTempDirectory();
   if ( !tmpFilePath.EndsWith( '/' ) )
      tmpFilePath << '/';
   String gnuFilePath = tmpFilePath + "AlignmentResiduals.gnu";

   // output file names
   StringList outputFiles;
   outputFiles << tmpFilePath + "residuals_ha_ha.svg"
               << tmpFilePath + "residuals_dec_ha.svg"
               << tmpFilePath + "residuals_ha_3d.svg"
               << tmpFilePath + "residuals_ha_dec.svg"
               << tmpFilePath + "residuals_dec_dec.svg"
               << tmpFilePath + "residuals_dec_3d.svg"
               << tmpFilePath + "residuals_delha_deldec.svg";

   {
      IsoString modelResidualDataPath8 = modelResidualDataPath.ToUTF8();
      IsoString outputFiles80 = outputFiles[0].ToUTF8();
      IsoString outputFiles81 = outputFiles[1].ToUTF8();
      IsoString outputFiles82 = outputFiles[2].ToUTF8();
      IsoString outputFiles83 = outputFiles[3].ToUTF8();
      IsoString outputFiles84 = outputFiles[4].ToUTF8();
      IsoString outputFiles85 = outputFiles[5].ToUTF8();
      IsoString outputFiles86 = outputFiles[6].ToUTF8();
      double maxDev = Max( *delHAs.MaxItem(), *delDecs.MaxItem() );

      File::WriteTextFile( gnuFilePath, IsoString().Format( RESIDUAL_GNUPLOT_TEMPLATE,
                                                            outputFiles80.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            outputFiles81.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            outputFiles82.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            outputFiles83.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            outputFiles84.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            outputFiles85.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            -maxDev,
                                                            maxDev,
                                                            -maxDev,
                                                            maxDev,
                                                            outputFiles86.c_str(),
                                                            modelResidualDataPath8.c_str(),
                                                            modelResidualDataPath8.c_str() ) );
   }

   int exitCode = ExternalProcess::ExecuteProgram(
      PixInsightSettings::GlobalString( "Application/BinDirectory" ) + "/gnuplot",
      StringList() << gnuFilePath );

   if ( exitCode == 0 )
   {
      for ( auto imagePath : outputFiles )
      {
         Array<ImageWindow> windowArray = ImageWindow::Open( imagePath );
         windowArray[0].SetZoomFactor( 1 );
         windowArray[0].FitWindow();
         windowArray[0].Show();
      }
   }
   else
   {
      MessageBox( "<p>Failure to generate alignment residuals graph. "
                  "Gnuplot exited with code " + String( exitCode ) + ".</p>",
                  WindowTitle(),
                  StdIcon::Error, StdButton::Ok ).Execute();
   }
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF INDIMountInterface.cpp - Released 2024-12-28T16:54:15Z
