//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.1.16
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.1.0
// ----------------------------------------------------------------------------
// INDICCDFrameInstance.cpp - Released 2019-09-29T12:27:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2019 Klaus Kretzschmar
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
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
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

#include "INDICCDFrameInstance.h"
#include "INDICCDFrameParameters.h"
#include "INDIClient.h"
#include "INDIMountInterface.h"

#undef J2000 // #defined in INDI/indicom.h

#include <pcl/ColorFilterArray.h>
#include <pcl/Console.h>
#include <pcl/DisplayFunction.h>
#include <pcl/ElapsedTime.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ICCProfile.h>
#include <pcl/MetaModule.h>
#include <pcl/Position.h>
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/Version.h>

namespace pcl
{

// ----------------------------------------------------------------------------

INDICCDFrameInstance::INDICCDFrameInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_deviceName( TheICFDeviceNameParameter->DefaultValue() ),
   p_uploadMode( ICFUploadMode::Default ),
   p_serverUploadDirectory( TheICFServerUploadDirectoryParameter->DefaultValue() ),
   p_serverFileNameTemplate( TheICFServerFileNameTemplateParameter->DefaultValue() ),
   p_frameType( ICFFrameType::Default ),
   p_binningX( int32( TheICFBinningXParameter->DefaultValue() ) ),
   p_binningY( int32( TheICFBinningYParameter->DefaultValue() ) ),
   p_filterSlot( int32( TheICFFilterSlotParameter->DefaultValue() ) ),
   p_exposureTime( TheICFExposureTimeParameter->DefaultValue() ),
   p_exposureDelay( TheICFExposureDelayParameter->DefaultValue() ),
   p_exposureCount( TheICFExposureCountParameter->DefaultValue() ),
   p_openClientImages( TheICFOpenClientImagesParameter->DefaultValue() ),
   p_newImageIdTemplate( TheICFNewImageIdTemplateParameter->DefaultValue() ),
   p_reuseImageWindow( TheICFReuseImageWindowParameter->DefaultValue() ),
   p_autoStretch( TheICFAutoStretchParameter->DefaultValue() ),
   p_linkedAutoStretch( TheICFLinkedAutoStretchParameter->DefaultValue() ),
   p_saveClientImages( TheICFSaveClientImagesParameter->DefaultValue() ),
   p_overwriteClientImages( TheICFOverwriteClientImagesParameter->DefaultValue() ),
   p_clientDownloadDirectory( TheICFClientDownloadDirectoryParameter->DefaultValue() ),
   p_clientFileNameTemplate( TheICFClientFileNameTemplateParameter->DefaultValue() ),
   p_clientOutputFormatHints( TheICFClientOutputFormatHintsParameter->DefaultValue() ),
   p_objectName( TheICFObjectNameParameter->DefaultValue() ),
   p_telescopeSelection( ICFTelescopeSelection::Default ),
   p_requireSelectedTelescope( TheICFRequireSelectedTelescopeParameter->DefaultValue() ),
   p_telescopeDeviceName( TheICFTelescopeDeviceNameParameter->DefaultValue() ),
   p_extFilterWheelDeviceName( TheICFExternalFilterWheelDeviceNameParameter->DefaultValue() ),
   p_enableAlignmentCorrection( TheICFEnableAlignmentCorrectionParameter->DefaultValue() ),
   p_alignmentFile( TheICFAlignmentFileParameter->DefaultValue() ),
   p_telescopeFocalLength( TheICFTelescopeFocalLengthParameter->DefaultValue()),

   m_exposureNumber( 0 )
{
}

// ----------------------------------------------------------------------------

INDICCDFrameInstance::INDICCDFrameInstance( const INDICCDFrameInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void INDICCDFrameInstance::Assign( const ProcessImplementation& p )
{
   const INDICCDFrameInstance* x = dynamic_cast<const INDICCDFrameInstance*>( &p );
   if ( x != nullptr )
   {
      p_deviceName               = x->p_deviceName;
      p_uploadMode               = x->p_uploadMode;
      p_serverUploadDirectory    = x->p_serverUploadDirectory;
      p_serverFileNameTemplate   = x->p_serverFileNameTemplate;
      p_frameType                = x->p_frameType;
      p_binningX                 = x->p_binningX;
      p_binningY                 = x->p_binningY;
      p_filterSlot               = x->p_filterSlot;
      p_exposureTime             = x->p_exposureTime;
      p_exposureDelay            = x->p_exposureDelay;
      p_exposureCount            = x->p_exposureCount;
      p_openClientImages         = x->p_openClientImages;
      p_newImageIdTemplate       = x->p_newImageIdTemplate;
      p_reuseImageWindow         = x->p_reuseImageWindow;
      p_autoStretch              = x->p_autoStretch;
      p_linkedAutoStretch        = x->p_linkedAutoStretch;
      p_saveClientImages         = x->p_saveClientImages;
      p_overwriteClientImages    = x->p_overwriteClientImages;
      p_clientDownloadDirectory  = x->p_clientDownloadDirectory;
      p_clientFileNameTemplate   = x->p_clientFileNameTemplate;
      p_clientOutputFormatHints  = x->p_clientOutputFormatHints;
      p_objectName               = x->p_objectName;
      p_telescopeSelection       = x->p_telescopeSelection;
      p_requireSelectedTelescope = x->p_requireSelectedTelescope;
      p_telescopeDeviceName      = x->p_telescopeDeviceName;
      p_extFilterWheelDeviceName = x->p_extFilterWheelDeviceName;
      p_enableAlignmentCorrection = x->p_enableAlignmentCorrection;
      p_alignmentFile             = x->p_alignmentFile;
      p_telescopeFocalLength         = x->p_telescopeFocalLength;

      o_clientViewIds            = x->o_clientViewIds;
      o_clientFilePaths          = x->o_clientFilePaths;
      o_serverFrames             = x->o_serverFrames;
   }
}

// ----------------------------------------------------------------------------

bool INDICCDFrameInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "INDICCDFrame can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool INDICCDFrameInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_deviceName.IsEmpty() )
   {
      whyNot = "No device has been specified";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool INDICCDFrameInstance::ValidateDevice( bool throwErrors ) const
{
   if ( p_deviceName.IsEmpty() )
   {
      if ( throwErrors )
         throw Error( "No device has been specified" );
      return false;
   }

   INDIClient* indi = INDIClient::TheClientOrDie();
   ExclConstDeviceList x = indi->ConstDeviceList();
   const INDIDeviceListItemArray& devices( x );
   for ( auto device : devices )
      if ( device.DeviceName == p_deviceName )
      {
         if ( !indi->HasPropertyItem( device.DeviceName, CCD_FRAME_PROPERTY_NAME, CCD_FRAME_WIDTH_ITEM_NAME ) ) // is this a camera device?
         {
            if ( throwErrors )
               throw Error( '\'' + p_deviceName + "' does not seem to be a valid Indigo CCD device" );
            return false;
         }
         return true;
      }
   if ( throwErrors )
      throw Error( "Indigo device not available: '" + p_deviceName + "'" );
   return false;
}

// ----------------------------------------------------------------------------

void  INDICCDFrameInstance::SetTelescopeAlignmentModelParameter( bool throwErrors )
{
   throwErrors &= bool( p_requireSelectedTelescope );
   switch ( p_telescopeSelection )
   {
   case ICFTelescopeSelection::NoTelescope:
   case ICFTelescopeSelection::ActiveTelescope:
      if ( throwErrors )
         throw Error( "Cannot set telescope alignment parameters: specify a telescope selection method" );
      break;
   case ICFTelescopeSelection::MountControllerTelescope:
   case ICFTelescopeSelection::MountControllerOrActiveTelescope:
      p_alignmentFile = TheINDIMountInterface->AlignmentFilePath();
      p_enableAlignmentCorrection = TheINDIMountInterface->ShouldApplyAlignmentCorrection();
      break;
   default:
      break;
   }
}

// ----------------------------------------------------------------------------

void INDICCDFrameInstance::SetTelescopeFocalLength( )
{
  p_telescopeFocalLength = TheINDIMountInterface->TelescopeFocalLength();
}

// ----------------------------------------------------------------------------

String INDICCDFrameInstance::TelescopeDeviceName( bool throwErrors ) const
{
   if ( p_frameType != ICFFrameType::LightFrame )
      return String();

   INDIClient* indi = INDIClient::TheClientOrDie();
   throwErrors &= bool( p_requireSelectedTelescope );
   String deviceName;
   INDIPropertyListItem item;
   switch ( p_telescopeSelection )
   {
   case ICFTelescopeSelection::NoTelescope:
      break;
   case ICFTelescopeSelection::ActiveTelescope:
      if ( indi->GetPropertyItem( p_deviceName, "ACTIVE_DEVICES", "ACTIVE_TELESCOPE", item ) )
      {
         deviceName = item.PropertyValue.Trimmed();
         if ( deviceName.IsEmpty() )
            if ( throwErrors )
               throw Error( "Cannot retrieve the active telescope device: No active telescope has been defined" );
      }
      else
      {
         if ( throwErrors )
            throw Error( "Cannot retrieve the active telescope device: The active telescope property is not available" );
         break;
      }
      break;
   case ICFTelescopeSelection::MountControllerTelescope:
      deviceName = TheINDIMountInterface->CurrentDeviceName();
      if ( deviceName.IsEmpty() )
         if ( throwErrors )
            throw Error( "Cannot retrieve required telescope device: The Indigo Mount Controller interface has no selected device" );
      break;
   default:
   case ICFTelescopeSelection::MountControllerOrActiveTelescope:
      deviceName = TheINDIMountInterface->CurrentDeviceName();
      if ( deviceName.IsEmpty() )
      {
         if ( indi->GetPropertyItem( p_deviceName, "ACTIVE_DEVICES", "ACTIVE_TELESCOPE", item ) )
            deviceName = item.PropertyValue.Trimmed();
         if ( deviceName.IsEmpty() )
            if ( throwErrors )
               throw Error( "Cannot retrieve required telescope device: No telescope device is available or selected on the Indigo server and/or Indigo Mount Controller interface" );
      }
      break;
   case ICFTelescopeSelection::TelescopeDeviceName:
      deviceName = p_telescopeDeviceName.Trimmed();
      if ( deviceName.IsEmpty() )
         if ( throwErrors )
            throw Error( "Cannot retrieve required telescope device: No telescope device name has been specified" );
      break;
   }

   if ( !deviceName.IsEmpty() )
   {
      ExclConstDeviceList x = indi->ConstDeviceList();
      const INDIDeviceListItemArray& devices( x );
      for ( auto device : devices )
         if ( device.DeviceName == deviceName )
         {
            if ( !indi->HasPropertyItem( deviceName, MOUNT_EQUATORIAL_COORDINATES_PROPERTY_NAME, MOUNT_EQUATORIAL_COORDINATES_RA_ITEM_NAME ) ) // is this a mount device?
            {
               if ( throwErrors )
                  throw Error( "The required device '" + deviceName + "' does not seem to be a valid Indigo mount device" );
               deviceName.Clear();
            }
            return deviceName;
         }
      if ( throwErrors )
         throw Error( "Required Indigo device not available: '" + deviceName + '\'' );
   }

   return String();
}

// ----------------------------------------------------------------------------

void INDICCDFrameInstance::SendDeviceProperties( bool async ) const
{
   INDIClient* indi = INDIClient::TheClientOrDie();

   indi->MaybeSendNewPropertyItem( p_deviceName, CCD_UPLOAD_MODE_PROPERTY_NAME, "INDI_SWITCH", UploadModePropertyString( p_uploadMode ), "ON", async );

   if ( !p_serverUploadDirectory.IsEmpty() )
      indi->MaybeSendNewPropertyItem( p_deviceName, CCD_LOCAL_MODE_PROPERTY_NAME, "INDI_TEXT", CCD_LOCAL_MODE_DIR_ITEM_NAME, p_serverUploadDirectory, async );

   indi->MaybeSendNewPropertyItem( p_deviceName, CCD_FRAME_TYPE_PROPERTY_NAME, "INDI_SWITCH", CCDFrameTypePropertyString( p_frameType ), "ON", async );

   if ( p_binningX > 0 && p_binningY > 0)
      indi->MaybeSendNewPropertyItem( p_deviceName, CCD_BIN_PROPERTY_NAME, "INDI_NUMBER", CCD_BIN_HORIZONTAL_ITEM_NAME, p_binningX, CCD_BIN_VERTICAL_ITEM_NAME, p_binningY,async );

   if ( p_filterSlot > 0 )
      indi->MaybeSendNewPropertyItem( p_extFilterWheelDeviceName != TheICFExternalFilterWheelDeviceNameParameter->DefaultValue()  ? p_extFilterWheelDeviceName : p_deviceName, WHEEL_SLOT_PROPERTY_NAME, "INDI_NUMBER", WHEEL_SLOT_ITEM_NAME, p_filterSlot, async );
}

// ----------------------------------------------------------------------------

String INDICCDFrameInstance::FileNameFromTemplate( const String& fileNameTemplate ) const
{
   INDIClient* indi = INDIClient::TheClientOrDie();
   INDIPropertyListItem item;

   String fileName;
   for ( String::const_iterator i = fileNameTemplate.Begin(); i < fileNameTemplate.End(); ++i )
      switch ( *i )
      {
      case '%':
         if ( ++i < fileNameTemplate.End() )
            switch ( *i )
            {
            case 'f':
               fileName << CCDFrameTypePrefix( p_frameType );
               break;
            case 'b':
               fileName << String().Format( "%dx%d", p_binningX, p_binningY );
               break;
            case 'e':
               fileName << String().Format( "%.3lf", p_exposureTime );
               break;
            case 'F':
              {
                String filterDeviceName = p_extFilterWheelDeviceName != TheICFExternalFilterWheelDeviceNameParameter->DefaultValue() ? p_extFilterWheelDeviceName : p_deviceName;
                if ( indi->GetPropertyItem( filterDeviceName, WHEEL_SLOT_PROPERTY_NAME, WHEEL_SLOT_ITEM_NAME, item ) )
                {
                   item.PropertyValue.Trim();
                   if ( indi->GetPropertyItem( filterDeviceName, WHEEL_SLOT_NAME_PROPERTY_NAME, "SLOT_NAME_" + item.PropertyValue, item ) )
                      fileName << item.PropertyValue;
                }
              }
               break;
            case 'T':
               if ( indi->GetPropertyItem( p_deviceName, CCD_TEMPERATURE_PROPERTY_NAME, CCD_TEMPERATURE_ITEM_NAME, item ) )
                  fileName << String().Format( "%+.2lf", item.PropertyValue.ToDouble() );
               break;
            case 't':
            case 'd':
               {
                  TimePoint t = TimePoint::Now();
                  fileName << t.ToString( (*i == 't') ? "%Y-%M-%DT%h:%m:%s" : "%Y-%M-%D" );
               }
               break;
            case 'n':
               fileName << String().Format( "%03d", m_exposureNumber+1 );
               break;
            case 'u':
               fileName << IsoString::UUID();
               break;
            case '%':
               fileName << '%';
               break;
            default:
               break;
            }
         break;

      default:
         fileName << *i;
         break;
      }

   return fileName.Trimmed();
}

// ----------------------------------------------------------------------------

String INDICCDFrameInstance::ServerFileName() const
{
   String fileName = FileNameFromTemplate( p_serverFileNameTemplate );
   if ( !fileName.IsEmpty() )
      return fileName;
   return FileNameFromTemplate( TheICFServerFileNameTemplateParameter->DefaultValue() );

}

// ----------------------------------------------------------------------------

String INDICCDFrameInstance::ClientFileName() const
{
   String fileName = FileNameFromTemplate( p_clientFileNameTemplate );
   if ( !fileName.IsEmpty() )
      return fileName;
   return FileNameFromTemplate( TheICFClientFileNameTemplateParameter->DefaultValue() );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class INDICCDFrameInstanceExecution : public AbstractINDICCDFrameExecution
{
public:

   INDICCDFrameInstanceExecution( INDICCDFrameInstance& instance ) :
      AbstractINDICCDFrameExecution( instance )
   {
      m_monitor.SetCallback( &m_status );
      m_waitMonitor.SetCallback( &m_spin );
   }

private:

   Console        m_console;
   StandardStatus m_status;
   StatusMonitor  m_monitor;
   SpinStatus     m_spin;
   StatusMonitor  m_waitMonitor;

   virtual void StartAcquisitionEvent()
   {
      m_console.EnableAbort();
   }

   virtual void NewExposureEvent( int expNum, int expCount )
   {
   }

   virtual void StartExposureDelayEvent( double totalDelayTime )
   {
      m_monitor.Initialize( String().Format( "Waiting for %.3gs", totalDelayTime ), RoundInt( totalDelayTime*100 ) );
   }

   virtual void ExposureDelayEvent( double delayTime )
   {
      m_monitor += size_type( RoundInt( delayTime*100 ) ) - m_monitor.Count();
   }

   virtual void EndExposureDelayEvent()
   {
      m_monitor.Complete();
   }

   virtual void StartExposureEvent( int expNum, int expCount, double expTime )
   {
      if ( m_waitMonitor.IsInitialized() )
         m_waitMonitor.Complete();
      m_monitor.Initialize( String().Format( "<end><cbr>Exposure %d of %d (%.3gs)", expNum+1, expCount, expTime ), RoundInt( expTime*100 ) );
   }

   virtual void ExposureEvent( int expNum, int expCount, double expTime )
   {
      m_monitor += size_type( RoundInt( expTime*100 ) ) - m_monitor.Count();
   }

   virtual void ExposureErrorEvent( const String& errMsg )
   {
      m_console.CriticalLn( "<end><cbr>*** Error: " + errMsg + '.' );
   }

   virtual void EndExposureEvent( int expNum )
   {
      m_monitor.Complete();

      // Print latest Indigo server message
      if ( INDIClient::TheClient()->Verbosity() > 1 )
      {
         String message = INDIClient::TheClient()->CurrentServerMessage().m_message;
         if ( !message.IsEmpty() && INDIClient::TheClient()->CurrentServerMessage().m_messageSeverity == INDIGO_ALERT_STATE ) {
             m_console.CriticalLn( "<end><cbr><br>* Latest Indigo server log entry: ERROR: " );
             m_console.CriticalLn( message );
             m_console.WriteLn();
         }
         else {
            m_console.NoteLn( "<end><cbr><br>* Latest Indigo server log entry:" );
            m_console.NoteLn( message );
            m_console.WriteLn();
         }
      }
   }

   virtual void WaitingForServerEvent()
   {
      if ( m_waitMonitor.IsInitialized() )
         ++m_waitMonitor;
      else
         m_waitMonitor.Initialize( "Waiting for Indigo server" );
   }

   virtual void NewFrameEvent( ImageWindow& window, bool reusedWindow, bool geometryChanged )
   {
      if ( reusedWindow )
      {
         if ( geometryChanged )
            window.ZoomToFit( false/*allowZoom*/ );
         else
            window.Regenerate();
      }
      else
      {
         window.BringToFront();
         window.Show();
         window.ZoomToFit( false/*allowZoom*/ );
      }
   }

   virtual void NewFrameEvent( const String& filePath )
   {
   }

   virtual void EndAcquisitionEvent()
   {
      m_console.DisableAbort();
      m_console.NoteLn( String().Format( "<end><cbr><br>===== INDICCDFrame: %d succeeded, %d failed =====",
                                         SuccessCount(), ErrorCount() ) );
   }

   virtual void AbortEvent()
   {
   }
};

// ----------------------------------------------------------------------------

bool INDICCDFrameInstance::ExecuteGlobal()
{
   INDICCDFrameInstanceExecution( *this ).Perform();
   return true;
}

// ----------------------------------------------------------------------------

void* INDICCDFrameInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheICFDeviceNameParameter )
      return p_deviceName.Begin();
   if ( p == TheICFUploadModeParameter )
      return &p_uploadMode;
   if ( p == TheICFServerUploadDirectoryParameter )
      return p_serverUploadDirectory.Begin();
   if ( p == TheICFServerFileNameTemplateParameter )
      return p_serverFileNameTemplate.Begin();
   if ( p == TheICFFrameTypeParameter )
      return &p_frameType;
   if ( p == TheICFBinningXParameter )
      return &p_binningX;
   if ( p == TheICFBinningYParameter )
      return &p_binningY;
   if ( p == TheICFFilterSlotParameter )
      return &p_filterSlot;
   if ( p == TheICFExposureTimeParameter )
      return &p_exposureTime;
   if ( p == TheICFExposureDelayParameter )
      return &p_exposureDelay;
   if ( p == TheICFExposureCountParameter )
      return &p_exposureCount;
   if ( p == TheICFOpenClientImagesParameter )
      return &p_openClientImages;
   if ( p == TheICFNewImageIdTemplateParameter )
      return p_newImageIdTemplate.Begin();
   if ( p == TheICFReuseImageWindowParameter )
      return &p_reuseImageWindow;
   if ( p == TheICFAutoStretchParameter )
      return &p_autoStretch;
   if ( p == TheICFLinkedAutoStretchParameter )
      return &p_linkedAutoStretch;
   if ( p == TheICFSaveClientImagesParameter )
      return &p_saveClientImages;
   if ( p == TheICFOverwriteClientImagesParameter )
      return &p_overwriteClientImages;
   if ( p == TheICFClientDownloadDirectoryParameter )
      return p_clientDownloadDirectory.Begin();
   if ( p == TheICFClientFileNameTemplateParameter )
      return p_clientFileNameTemplate.Begin();
   if ( p == TheICFClientOutputFormatHintsParameter )
      return p_clientOutputFormatHints.Begin();
   if ( p == TheICFObjectNameParameter )
      return p_objectName.Begin();
   if ( p == TheICFTelescopeSelectionParameter )
      return &p_telescopeSelection;
   if ( p == TheICFRequireSelectedTelescopeParameter )
      return &p_requireSelectedTelescope;
   if ( p == TheICFTelescopeDeviceNameParameter )
      return p_telescopeDeviceName.Begin();
   if ( p == TheICFExternalFilterWheelDeviceNameParameter )
            return p_extFilterWheelDeviceName.Begin();
   if ( p == TheICFEnableAlignmentCorrectionParameter )
      return &p_enableAlignmentCorrection;
   if ( p == TheICFAlignmentFileParameter )
      return p_alignmentFile.Begin();
   if ( p == TheICFTelescopeFocalLengthParameter )
       return &p_telescopeFocalLength;

   if ( p == TheICFClientViewIdParameter )
      return o_clientViewIds[tableRow].Begin();
   if ( p == TheICFClientFilePathParameter )
      return o_clientFilePaths[tableRow].Begin();
   if ( p == TheICFServerFrameParameter )
      return o_serverFrames[tableRow].Begin();

   return nullptr;
}

// ----------------------------------------------------------------------------

bool INDICCDFrameInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheICFDeviceNameParameter )
   {
      p_deviceName.Clear();
      if ( sizeOrLength > 0 )
         p_deviceName.SetLength( sizeOrLength );
   }
   else if ( p == TheICFServerUploadDirectoryParameter )
   {
      p_serverUploadDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_serverUploadDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheICFServerFileNameTemplateParameter )
   {
      p_serverFileNameTemplate.Clear();
      if ( sizeOrLength > 0 )
         p_serverFileNameTemplate.SetLength( sizeOrLength );
   }
   else if ( p == TheICFNewImageIdTemplateParameter )
   {
      p_newImageIdTemplate.Clear();
      if ( sizeOrLength > 0 )
         p_newImageIdTemplate.SetLength( sizeOrLength );
   }
   else if ( p == TheICFClientDownloadDirectoryParameter )
   {
      p_clientDownloadDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_clientDownloadDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheICFClientFileNameTemplateParameter )
   {
      p_clientFileNameTemplate.Clear();
      if ( sizeOrLength > 0 )
         p_clientFileNameTemplate.SetLength( sizeOrLength );
   }
   else if ( p == TheICFClientOutputFormatHintsParameter )
   {
      p_clientOutputFormatHints.Clear();
      if ( sizeOrLength > 0 )
         p_clientOutputFormatHints.SetLength( sizeOrLength );
   }
   else if ( p == TheICFObjectNameParameter )
   {
      p_objectName.Clear();
      if ( sizeOrLength > 0 )
         p_objectName.SetLength( sizeOrLength );
   }
   else if ( p == TheICFTelescopeDeviceNameParameter )
   {
      p_telescopeDeviceName.Clear();
      if ( sizeOrLength > 0 )
         p_telescopeDeviceName.SetLength( sizeOrLength );
   } else if ( p == TheICFExternalFilterWheelDeviceNameParameter )
   {
	   p_extFilterWheelDeviceName.Clear();
	   if ( sizeOrLength > 0)
		   p_extFilterWheelDeviceName.SetLength( sizeOrLength );
   }

   else if ( p == TheICFClientFramesParameter )
   {
      o_clientViewIds.Clear();
      if ( sizeOrLength > 0 )
         o_clientViewIds.Add( String(), sizeOrLength );
   }
   else if ( p == TheICFClientViewIdParameter )
   {
      o_clientViewIds[tableRow].Clear();
      if ( sizeOrLength > 0 )
         o_clientViewIds[tableRow].SetLength( sizeOrLength );
   }
   else if ( p == TheICFClientFilePathParameter )
   {
      o_clientFilePaths[tableRow].Clear();
      if ( sizeOrLength > 0 )
         o_clientFilePaths[tableRow].SetLength( sizeOrLength );
   }
   else if ( p == TheICFServerFramesParameter )
   {
      o_serverFrames.Clear();
      if ( sizeOrLength > 0 )
         o_serverFrames.Add( String(), sizeOrLength );
   }
   else if ( p == TheICFServerFrameParameter )
   {
      o_serverFrames[tableRow].Clear();
      if ( sizeOrLength > 0 )
         o_serverFrames[tableRow].SetLength( sizeOrLength );
   }
   else if ( p == TheICFAlignmentFileParameter )
   {
      p_alignmentFile.Clear();
      if ( sizeOrLength > 0 )
         p_alignmentFile.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type INDICCDFrameInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheICFDeviceNameParameter )
      return p_deviceName.Length();
   if ( p == TheICFServerUploadDirectoryParameter )
      return p_serverUploadDirectory.Length();
   if ( p == TheICFServerFileNameTemplateParameter )
      return p_serverFileNameTemplate.Length();
   if ( p == TheICFNewImageIdTemplateParameter )
      return p_newImageIdTemplate.Length();
   if ( p == TheICFClientDownloadDirectoryParameter )
      return p_clientDownloadDirectory.Length();
   if ( p == TheICFClientFileNameTemplateParameter )
      return p_clientFileNameTemplate.Length();
   if ( p == TheICFClientOutputFormatHintsParameter )
      return p_clientOutputFormatHints.Length();
   if ( p == TheICFObjectNameParameter )
      return p_objectName.Length();
   if ( p == TheICFTelescopeDeviceNameParameter )
      return p_telescopeDeviceName.Length();
   if ( p == TheICFExternalFilterWheelDeviceNameParameter )
	   return p_extFilterWheelDeviceName.Length();
   if ( p == TheICFAlignmentFileParameter )
         return p_alignmentFile.Length();

   if ( p == TheICFClientFramesParameter )
      return o_clientViewIds.Length();
   if ( p == TheICFClientViewIdParameter )
      return o_clientViewIds[tableRow].Length();
   if ( p == TheICFClientFilePathParameter )
      return o_clientFilePaths[tableRow].Length();
   if ( p == TheICFServerFramesParameter )
      return o_serverFrames.Length();
   if ( p == TheICFServerFrameParameter )
      return o_serverFrames[tableRow].Length();

   return 0;
}

// ----------------------------------------------------------------------------

String INDICCDFrameInstance::UploadModePropertyString( int uploadModeIdx )
{
   switch ( uploadModeIdx )
   {
   case ICFUploadMode::UploadClient:
      return CCD_UPLOAD_MODE_CLIENT_ITEM_NAME;
   case ICFUploadMode::UploadServer:
      return CCD_UPLOAD_MODE_LOCAL_ITEM_NAME;
   case ICFUploadMode::UploadServerAndClient:
      return CCD_UPLOAD_MODE_BOTH_ITEM_NAME;
   default:
      throw Error( "Internal error: INDICCDFrameInstance: Invalid upload mode." );
   }
}

// ----------------------------------------------------------------------------

String INDICCDFrameInstance::CCDFrameTypePropertyString( int frameTypeIdx )
{
   switch ( frameTypeIdx )
   {
   case ICFFrameType::LightFrame:
      return CCD_FRAME_TYPE_LIGHT_ITEM_NAME;
   case ICFFrameType::BiasFrame:
      return CCD_FRAME_TYPE_BIAS_ITEM_NAME;
   case ICFFrameType::DarkFrame:
      return CCD_FRAME_TYPE_DARK_ITEM_NAME;
   case ICFFrameType::FlatFrame:
      return CCD_FRAME_TYPE_FLAT_ITEM_NAME;
   default:
      throw Error( "Internal error: INDICCDFrameInstance: Invalid frame type." );
   }
}

// ----------------------------------------------------------------------------

String INDICCDFrameInstance::CCDFrameTypePrefix( int frameTypeIdx )
{
   switch ( frameTypeIdx )
   {
   case ICFFrameType::LightFrame:
      return "LIGHT";
   case ICFFrameType::BiasFrame:
      return "BIAS";
   case ICFFrameType::DarkFrame:
      return "DARK";
   case ICFFrameType::FlatFrame:
      return "FLAT";
   default:
      throw Error( "Internal error: INDICCDFrameInstance: Invalid frame type." );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

typedef KeyValue<IsoString,Variant> ImageProperty;
typedef Array<ImageProperty>        ImagePropertyList;

struct ImageMetadata
{
   Optional<String>   cameraName;
   Optional<String>   frameType;
   Optional<String>   filterName;
   Optional<double>   expTime;
   Optional<double>   sensorTemp;
   Optional<double>   xPixSize;
   Optional<double>   yPixSize;
   Optional<unsigned> xBinning;
   Optional<unsigned> yBinning;
   Optional<unsigned> xOrigin;
   Optional<unsigned> yOrigin;

   Optional<String>   telescopeName;
   Optional<double>   focalLength;
   Optional<double>   aperture;
   Optional<double>   apertureArea;
   Optional<String>   telescopePierSide;

   Optional<String>   objectName;
   Optional<int>      year;
   Optional<int>      month;
   Optional<int>      day;
   Optional<double>   dayf;
   Optional<double>   tz;
   Optional<double>   ra;
   Optional<double>   dec;
   Optional<double>   equinox;
   Optional<double>   localSiderealTime;
   Optional<double>   geographicLongitude;
   Optional<double>   geographicLatitude;
   Optional<double>   geographicHeight;
   Optional<double>   eodRa;
   Optional<double>   eodDec;
};

static ImageMetadata
ImageMetadataFromFITSKeywords( const FITSKeywordArray& keywords )
{
   ImageMetadata data;
   for ( auto k : keywords )
   {
      IsoString value = k.StripValueDelimiters();
      if ( k.name == "INSTRUME" )
         data.cameraName = String( value );
      else if ( k.name == "FRAME" || k.name == "IMAGETYP" )
         data.frameType = String( value );
      else if ( k.name == "FILTER" )
         data.filterName = String( value );
      else if ( k.name == "EXPTIME" )
         data.expTime = value.ToDouble();
      else if ( k.name == "CCD-TEMP" )
         data.sensorTemp = value.ToDouble();
      else if ( k.name == "PIXSIZE1" )
         data.xPixSize = value.ToDouble();
      else if ( k.name == "PIXSIZE2" )
         data.yPixSize = value.ToDouble();
      else if ( k.name == "XBINNING" )
         data.xBinning = value.ToUInt( 10 );
      else if ( k.name == "YBINNING" )
         data.yBinning = value.ToUInt( 10 );
      else if ( k.name == "XORGSUBF" )
         data.xOrigin = value.ToUInt( 10 );
      else if ( k.name == "YORGSUBF" )
         data.yOrigin = value.ToUInt( 10 );
      else if ( k.name == "TELESCOP" )
         data.telescopeName = String( value );
      else if ( k.name == "FOCALLEN" )
         data.focalLength = value.ToDouble()/1000;
      else if ( k.name == "APTDIA" )
         data.aperture = value.ToDouble()/1000;
      else if ( k.name == "APTAREA" )
         data.apertureArea = value.ToDouble()/1000;
      else if ( k.name == "OBJNAME" || k.name == "OBJECT" )
         data.objectName = String( value );
      else if ( k.name == "DATE-OBS" )
      {
         int year, month, day;
         double dayf, tz;
         value.ParseISO8601DateTime( year, month, day, dayf, tz );
         data.year = year;
         data.month = month;
         data.day = day;
         data.dayf = dayf;
         data.tz = tz;
      }
      else if ( k.name == "OBJCTRA" )
         data.ra = value.SexagesimalToDouble( ' ' ) * 15;
      else if ( k.name == "OBJCTDEC" )
         data.dec = value.SexagesimalToDouble( ' ' );
      else if ( k.name == "EQUINOX" )
         data.equinox = value.ToDouble();
      else if ( k.name == "LOCALLST" )
    	  data.localSiderealTime = value.SexagesimalToDouble( ' ' );;
   }
   if ( !data.equinox.IsDefined() )
      if ( data.ra.IsDefined() && data.dec.IsDefined() )
         data.equinox = 2000;
   return data;
}

// ----------------------------------------------------------------------------

static ImagePropertyList
ImagePropertiesFromImageMetadata( const ImageMetadata& data )
{
   ImagePropertyList properties;
   if ( data.expTime.IsDefined() )
      properties << ImageProperty( "Instrument:ExposureTime", Round( data.expTime(), 4 ) );
   if ( data.cameraName.IsDefined() )
      properties << ImageProperty( "Instrument:Camera:Name", data.cameraName() );
   if ( data.xBinning.IsDefined() )
      properties << ImageProperty( "Instrument:Camera:XBinning", data.xBinning() );
   if ( data.yBinning.IsDefined() )
      properties << ImageProperty( "Instrument:Camera:YBinning", data.yBinning() );
   if ( data.xOrigin.IsDefined() )
      properties << ImageProperty( "Instrument:Camera:XOrigin", data.xOrigin() );
   if ( data.yOrigin.IsDefined() )
      properties << ImageProperty( "Instrument:Camera:YOrigin", data.yOrigin() );
   if ( data.telescopeName.IsDefined() )
      properties << ImageProperty( "Instrument:Telescope:Name", data.telescopeName() );
   if ( data.focalLength.IsDefined() )
      properties << ImageProperty( "Instrument:Telescope:FocalLength", Round( data.focalLength(), 3 ) );
   if ( data.aperture.IsDefined() )
      properties << ImageProperty( "Instrument:Telescope:Aperture", Round( data.aperture(), 3 ) );
   if ( data.apertureArea.IsDefined() )
      properties << ImageProperty( "Instrument:Telescope:CollectingArea", Round( data.apertureArea(), 3 ) );
   if ( data.eodRa.IsDefined() )
      properties << ImageProperty( "Instrument:Telescope:Pointing:RA", data.ra() );
   if ( data.eodDec.IsDefined() )
      properties << ImageProperty( "Instrument:Telescope:Pointing:Dec", data.dec() );
   if ( data.filterName.IsDefined() )
      properties << ImageProperty( "Instrument:Filter:Name", data.filterName() );
   if ( data.sensorTemp.IsDefined() )
      properties << ImageProperty( "Instrument:Sensor:Temperature", data.sensorTemp() );
   if ( data.xPixSize.IsDefined() )
      properties << ImageProperty( "Instrument:Sensor:XPixelSize", Round( data.xPixSize(), 3 ) );
   if ( data.yPixSize.IsDefined() )
      properties << ImageProperty( "Instrument:Sensor:YPixelSize", Round( data.yPixSize(), 3 ) );
   if ( data.frameType.IsDefined() )
      properties << ImageProperty( "Observation:Image:Type", data.frameType() );
   if ( data.geographicLongitude.IsDefined() )
      properties << ImageProperty( "Observation:Location:Longitude", data.geographicLongitude() );
   if ( data.geographicLatitude.IsDefined() )
      properties << ImageProperty( "Observation:Location:Latitude", data.geographicLatitude() );
   if ( data.geographicHeight.IsDefined() )
      properties << ImageProperty( "Observation:Location:Elevation", data.geographicHeight() );
   if ( data.objectName.IsDefined() )
      properties << ImageProperty( "Observation:Object:Name", data.objectName() );
   if ( data.year.IsDefined() )
      properties << ImageProperty( "Observation:Time:Start",
            IsoString::ToISO8601DateTime( data.year(), data.month(), data.day(), data.dayf() - data.tz()/24, 0/*tz*/,
                                          ISO8601ConversionOptions( 3/*timeItems*/, 3/*precision*/, true/*timeZone(zulu)*/ ) ) );
   if ( data.ra.IsDefined() )
      properties << ImageProperty( "Observation:Center:RA", data.ra() );
   if ( data.dec.IsDefined() )
      properties << ImageProperty( "Observation:Center:Dec", data.dec() );

   if ( data.equinox.IsDefined() )
   {
      properties << ImageProperty( "Observation:Equinox", data.equinox() );
      if ( data.equinox() == 2000.0 )
         properties << ImageProperty( "Observation:CelestialReferenceSystem", "GCRS" );
   }
   else
      properties << ImageProperty( "Observation:CelestialReferenceSystem", "True" );

   /*
    * ### Redundant: Can be calculated more accurately from observation
    * location and time properties. In addition, LST (GAST) depends on a
    * particular theory of Earth's rotation.
    */
//    if ( data.localSiderealTime.IsDefined() )
//       properties << ImageProperty( "Observation:LocalSiderealTime", data.localSiderealTime() );

   return properties;
}

// ----------------------------------------------------------------------------

static String UniqueFilePath( const String& filePath )
{
   if ( !File::Exists( filePath ) )
      return filePath;
   for ( unsigned u = 1; ; ++u )
   {
      String tryFilePath = File::AppendToName( filePath, '_' + String( u ) );
      if ( !File::Exists( tryFilePath ) )
         return tryFilePath;
   }
}

// ----------------------------------------------------------------------------

void AbstractINDICCDFrameExecution::Perform()
{
   m_instance.o_clientViewIds.Clear();
   m_instance.o_clientFilePaths.Clear();
   m_instance.o_serverFrames.Clear();

   if ( IsRunning() )
      throw Error( "Internal error: Recursive call to AbstractINDICCDFrameExecution::Perform() detected." );

   if ( m_instance.p_deviceName.IsEmpty() )
      throw Error( "No device has been specified." );

   INDIClient* indi = INDIClient::TheClientOrDie();

   if ( !indi->HasDevices() )
      throw Error( "No Indigo device has been connected." );

   try
   {
      m_instance.ValidateDevice();

      String telescopeName = m_instance.TelescopeDeviceName();

      m_instance.SetTelescopeAlignmentModelParameter();
      m_instance.SetTelescopeFocalLength();


      m_instance.SendDeviceProperties( false/*async*/ );

      INDIPropertyListItem item;
      bool serverSendsImage = true; // be compatible with legacy drivers not implementing UPLOAD_MODE
      bool serverKeepsImage = false;
      if ( indi->GetPropertyItem( m_instance.p_deviceName, CCD_UPLOAD_MODE_PROPERTY_NAME, CCD_UPLOAD_MODE_LOCAL_ITEM_NAME, item ) )
         if ( item.PropertyValue == "ON" )
         {
            serverSendsImage = false;
            serverKeepsImage = true;
         }
         else
         {
            if ( indi->GetPropertyItem( m_instance.p_deviceName, CCD_UPLOAD_MODE_PROPERTY_NAME, CCD_UPLOAD_MODE_BOTH_ITEM_NAME, item ) )
               if ( item.PropertyValue == "ON" )
                  serverKeepsImage = true;
         }

      m_running = true;
      m_aborted = false;
      m_successCount = m_errorCount = 0;

      StartAcquisitionEvent();

      for ( m_instance.m_exposureNumber = 0; m_instance.m_exposureNumber < m_instance.p_exposureCount; ++m_instance.m_exposureNumber )
      {
         NewExposureEvent( m_instance.m_exposureNumber, m_instance.p_exposureCount );

         if ( m_instance.m_exposureNumber > 0 )
            if ( m_instance.p_exposureDelay > 0 )
            {
               StartExposureDelayEvent( m_instance.p_exposureDelay );

               for ( ElapsedTime T; T() < m_instance.p_exposureDelay; )
                  ExposureDelayEvent( T() );

               EndExposureDelayEvent();
            }

         indi->ClearDownloadedImagePath();

         if ( serverKeepsImage )
         {
            String serverFileName = m_instance.ServerFileName();
            m_instance.o_serverFrames << serverFileName;
            indi->SendNewPropertyItem( m_instance.p_deviceName, CCD_LOCAL_MODE_PROPERTY_NAME, "INDI_TEXT", CCD_LOCAL_MODE_PREFIX_ITEM_NAME, serverFileName, false/*async*/ );
         }

         if ( !indi->SendNewPropertyItem( m_instance.p_deviceName, CCD_EXPOSURE_PROPERTY_NAME, "INDI_NUMBER", CCD_EXPOSURE_ITEM_NAME, m_instance.p_exposureTime, true/*async*/ ) )
         {
            ExposureErrorEvent( "Failure to send new property values to Indigo server" );
            ++m_errorCount;
            continue; // ### TODO: Implement a p_onError process parameter
         }

         double telescopeRA = 0, telescopeDec = 0;
         if ( !telescopeName.IsEmpty() )
         {
            // Get telescope apparent epoch-of-date coordinates.
            INDIPropertyListItem itemRA, itemDec;
            if ( !indi->GetPropertyItem( telescopeName, MOUNT_EQUATORIAL_COORDINATES_PROPERTY_NAME, MOUNT_EQUATORIAL_COORDINATES_RA_ITEM_NAME, itemRA, false/*formatted*/ ) ||
                 !indi->GetPropertyItem( telescopeName, MOUNT_EQUATORIAL_COORDINATES_PROPERTY_NAME, MOUNT_EQUATORIAL_COORDINATES_DEC_ITEM_NAME, itemDec, false/*formatted*/ ) )
               throw Error( "Cannot get current mount coordinates for device '" + telescopeName + "'" );
            double ra = 0;
            double dec = 0;
            itemRA.PropertyValue.TryToDouble(ra);
            itemDec.PropertyValue.TryToDouble(dec);
            telescopeRA = Rad( ra *15 );
            telescopeDec = Rad( dec );
          }

         ElapsedTime T;

         for ( bool inExposure = false; ; )
            if ( indi->GetPropertyItem( m_instance.p_deviceName, CCD_EXPOSURE_PROPERTY_NAME, CCD_EXPOSURE_ITEM_NAME, item, false/*formatted*/ ) ) {
               if ( item.PropertyState == INDIGO_BUSY_STATE )
               {
                  if ( inExposure )
                  {
                     // Exposure running
                     double t = 0;
                     item.PropertyValue.TryToDouble( t );
                     ExposureEvent( m_instance.m_exposureNumber, m_instance.p_exposureCount, m_instance.p_exposureTime - t ); // CCD_EXPOSURE_VALUE is the remaining exp. time
                  }
                  else
                  {
                     // Exposure started
                     inExposure = true;
                     StartExposureEvent( m_instance.m_exposureNumber, m_instance.p_exposureCount, m_instance.p_exposureTime );
                  }
               }
               else
               {
                  if ( inExposure )
                  {
                     // Exposure completed
                     EndExposureEvent( m_instance.m_exposureNumber );
                     ++m_successCount;
                     break;
                  }

                  if ( T() > 1 )
                     WaitingForServerEvent();
               }
            } else {
               throw Error( "CCD_EXPOSURE.EXPOSURE property not found." );
            }

         if ( serverSendsImage )
         {
            for ( T.Reset(); !indi->HasDownloadedImage(); )
               if ( T() > 1 )
                  WaitingForServerEvent();

            String filePath = indi->DownloadedImagePath();
            FileFormat inputFormat( File::ExtractExtension( filePath ), true/*read*/, false/*write*/ );
            FileFormatInstance inputFile( inputFormat );

            ImageDescriptionArray images;
            if ( !inputFile.Open( images, filePath, "raw cfa verbosity 0 up-bottom signed-is-physical" ) )
               throw CaughtException();
            if ( images.IsEmpty() )
               throw Error( filePath + ": Empty image file." );
            if ( !images[0].info.supported || images[0].info.NumberOfSamples() == 0 )
               throw Error( filePath + ": Invalid or unsupported image." );

            ImagePropertyList properties;
            if ( inputFormat.CanStoreImageProperties() )
            {
               PropertyDescriptionArray descriptions = inputFile.ImageProperties();
               for ( auto description : descriptions )
                  properties << ImageProperty( description.id, inputFile.ReadImageProperty( description.id ) );
            }

            FITSKeywordArray keywords;
            if ( inputFormat.CanStoreKeywords() )
               if ( inputFile.ReadFITSKeywords( keywords ) )
                  if ( !keywords.IsEmpty() )
                  {
                     ImageMetadata data = ImageMetadataFromFITSKeywords( keywords );

                     if ( m_instance.p_frameType == ICFFrameType::LightFrame )
                        if ( !m_instance.p_objectName.IsEmpty() )
                        {
                           // Replace or add OBJNAME/OBJECT keywords.
                           if ( data.objectName.IsDefined() )
                           {
                              for ( FITSHeaderKeyword& k : keywords )
                                 if ( k.name == "OBJNAME" || k.name == "OBJECT" )
                                 {
                                    k.value = '\'' + m_instance.p_objectName + '\'';
                                    k.comment = "Name of observed object";
                                 }
                           }
                           else
                              keywords << FITSHeaderKeyword( "OBJECT", m_instance.p_objectName, "Name of observed object" );

                           data.objectName = m_instance.p_objectName;
                        }

                     // If not already available, try to get the local
                     // sidereal time.
                     if ( !data.localSiderealTime.IsDefined() )
                        if ( indi->GetPropertyItem( telescopeName, MOUNT_LST_TIME_PROPERTY_NAME, MOUNT_LST_TIME_ITEM_NAME, item, false/*formatted*/ ) )
                        {
                           data.localSiderealTime = item.PropertyValue.ToDouble();
                           IsoString lstSexagesimal = IsoString::ToSexagesimal( data.localSiderealTime(),
                                             SexagesimalConversionOptions( 3/*items*/, 2/*precision*/, false/*sign*/, 0/*width*/, ':'/*separator*/ ) );
                           keywords << FITSHeaderKeyword( "LOCALLST", lstSexagesimal, "Local sidereal time (LST) - after exposure" );
                        }

                     if ( !telescopeName.IsEmpty() )
                     {
                        // If not already available, try to get the telescope
                        // pier side from standard device properties.
                        if ( !data.telescopePierSide.IsDefined() )
                           if ( indi->GetPropertyItem( telescopeName, MOUNT_SIDE_OF_PIER_PROPERTY_NAME, MOUNT_SIDE_OF_PIER_WEST_ITEM_NAME, item) )
                           {
                              if (item.PropertyValue == "ON" )
                              {
                                 data.telescopePierSide = "WEST";
                                 keywords << FITSHeaderKeyword( "PIERSIDE", "West", "Counterweight pointing East." );
                              } else
                              {
                                 data.telescopePierSide = "EAST";
                                 keywords << FITSHeaderKeyword( "PIERSIDE", "East", "Counterweight pointing West." );
                              }
                           } else {
                              data.telescopePierSide = "NONE";
                           }

                        // Apply the inverse of the alignment model
                        if ( m_instance.p_enableAlignmentCorrection )
                        {
                           Console().WriteLn( "<end><cbr>INDICCDFrame: Apply alignment correction" );
                           AutoPointer<AlignmentModel> aModel = AlignmentModel::Create( m_instance.p_alignmentFile );
                           double localSiderialTime = data.localSiderealTime();
                           double newHourAngle = -1;
                           double newDec = -1;
                           pcl_enum pierSide = data.telescopePierSide() == "EAST" ? IMCPierSide::East : IMCPierSide::West;
                           aModel->ApplyInverse( newHourAngle, newDec,
                                    AlignmentModel::RangeShiftHourAngle( localSiderialTime - Deg( telescopeRA )/15 ),
                                    Deg( telescopeDec ),
                                    pierSide );
                           telescopeRA = Rad( (localSiderialTime - newHourAngle)*15 );
                           telescopeDec = Rad( newDec );
                        }

                        data.telescopeName = telescopeName;

                        // Store the epoch-of-date coordinates
                        if (!data.eodRa.IsDefined() || !data.eodDec.IsDefined())
                        {
                           data.eodRa = Deg( telescopeRA );
                           data.eodDec = Deg( telescopeDec );
                        }

                        // Compute GCRS / J2000.0 coordinates from telescope
                        // true / EOD coordinates if epoch == JNow
						// Note: The EOD coordinates consider the proper motion of the target objects, 
						//       if they were acquired via the coordinate search dialog. Since we do not
						//       have these information here the resulting J2000 coordinate will have this
						//       mismatch.  
						data.ra = Deg(telescopeRA);
						data.dec = Deg(telescopeDec);
						data.equinox = 2000.0;
						INDIPropertyListItem item;
						INDIClient* indi = INDIClient::TheClientOrDie();
						bool computeApparentPositions = false;
						if (indi->GetPropertyItem(telescopeName, MOUNT_EPOCH_PROPERTY_NAME, MOUNT_EPOCH_ITEM_NAME, item, false/*formatted*/))
						{
							if (TruncInt(item.PropertyValue.ToDouble()) == 0)
								computeApparentPositions = true;
						}
						else
						{
							// fallback: if EPOCH property is not defined assume JNow
							computeApparentPositions = true;
						}

                        if ( data.year.IsDefined() && computeApparentPositions)
                        {
							TimePoint t(data.year(), data.month(), data.day(), data.dayf() - data.tz() / 24);
							Position P(t, "UTC");
							P.InitEquinoxBasedParameters();
							Vector u3 = Vector::FromSpherical(telescopeRA, telescopeDec);
							// Apparent -> GCRS
							Vector u2 = P.EquinoxBiasPrecessionNutationInverseMatrix() * u3;
							// ### TODO: Topocentric -> geocentric coordinates.
							u2.ToSpherical2Pi(telescopeRA, telescopeDec);
							data.ra = Deg(telescopeRA);
							data.dec = Deg(telescopeDec);
                        }

                        // If not already available, try to get the telescope
                        // focal length from standard device properties.
                        if ( !data.focalLength.IsDefined())
                           if ( indi->GetPropertyItem( telescopeName, "TELESCOPE_INFO", "TELESCOPE_FOCAL_LENGTH", item, false/*formatted*/ ) )
                           {
                              double focalLengthMM = Round( item.PropertyValue.ToDouble(), 3 );
                              data.focalLength = focalLengthMM/1000;
                              keywords << FITSHeaderKeyword( "FOCALLEN", focalLengthMM, "Focal length (mm)" );
                           }
                           else if (m_instance.p_telescopeFocalLength != 0)
                           {
                              double focalLengthMM = m_instance.p_telescopeFocalLength;
                              data.focalLength = focalLengthMM/1000;
                              keywords << FITSHeaderKeyword( "FOCALLEN", focalLengthMM, "Focal length (mm)" );
                           }
                     }
                     // Replace existing coordinate keywords with our
                     // (rigorously calculated) GCRS coordinates.
					 bool raKeywordFound = false;
					 bool decKeywordFound = false;
					 bool equinoxKeywordFound = false;
					 if (data.ra.IsDefined() && data.dec.IsDefined()) {
						 for (FITSHeaderKeyword& k : keywords)
							 if (k.name == "OBJCTRA")
							 {
								 raKeywordFound = true;
								 k.value = '\'' + IsoString::ToSexagesimal(data.ra() / 15,
									 RAConversionOptions(3/*precision*/, 0/*width*/)) + '\'';
								 k.comment = "Right ascension of the center of the image";
							 }
							 else if (k.name == "OBJCTDEC")
							 {
								 decKeywordFound = true;
								 k.value = '\'' + IsoString::ToSexagesimal(data.dec(),
									 DecConversionOptions(2/*precision*/, 0/*width*/)) + '\'';
								 k.comment = "Declination of the center of the image";
							 }
							 else if (k.name == "EQUINOX")
							 {
								 equinoxKeywordFound = true;
								 k.value = "2000.0";
								 k.comment = "Coordinates referred to GCRS / J2000.0";
							 }
					 }
					 
					 if (!raKeywordFound)
					 {
						 IsoString sexagesimal = '\'' + IsoString::ToSexagesimal(data.ra() / 15,
							 RAConversionOptions(3/*precision*/, 0/*width*/)) + '\'';
						 keywords << FITSHeaderKeyword("OBJCTRA", sexagesimal, "Right ascension of the center of the image");
					 }
					 if (!decKeywordFound)
					 {
						 IsoString sexagesimal = '\'' + IsoString::ToSexagesimal(data.dec(),
							 DecConversionOptions(2/*precision*/, 0/*width*/)) + '\'';
						 keywords << FITSHeaderKeyword("OBJCTDEC", sexagesimal, "Declination of the center of the image");
					 }
					 if (!equinoxKeywordFound)
					 {
						 keywords << FITSHeaderKeyword("EQUINOX", 2000.0, "Coordinates referred to GCRS / J2000.0");
					 }
					 

                     // If not already available, try to get the local
                     // geographic longitude of observatory.
                     if (!data.geographicLongitude.IsDefined())
                        if ( indi->GetPropertyItem( telescopeName, GEOGRAPHIC_COORDINATES_PROPERTY_NAME, GEOGRAPHIC_COORDINATES_LONGITUDE_ITEM_NAME, item, false/*formatted*/ ))
                           data.geographicLongitude = item.PropertyValue.ToDouble();

                     // If not already available, try to get the local
                     // geographic latitude of observatory.
                     if (!data.geographicLatitude.IsDefined())
                        if ( indi->GetPropertyItem( telescopeName, GEOGRAPHIC_COORDINATES_PROPERTY_NAME, GEOGRAPHIC_COORDINATES_LATITUDE_ITEM_NAME, item, false/*formatted*/ ))
                           data.geographicLatitude = item.PropertyValue.ToDouble();

                     // If not already available, try to get the local
                     // geographic height of observatory.
                     if (!data.geographicHeight.IsDefined())
                        if ( indi->GetPropertyItem( telescopeName, GEOGRAPHIC_COORDINATES_PROPERTY_NAME, GEOGRAPHIC_COORDINATES_ELEVATION_ITEM_NAME, item, false/*formatted*/ ))
                           data.geographicHeight = item.PropertyValue.ToDouble();

                     if (!data.geographicHeight.IsDefined())
                     {
                       String filterDeviceName = m_instance.p_extFilterWheelDeviceName != TheICFExternalFilterWheelDeviceNameParameter->DefaultValue() ? m_instance.p_extFilterWheelDeviceName : m_instance.p_deviceName;
                       if ( indi->GetPropertyItem( filterDeviceName, WHEEL_SLOT_PROPERTY_NAME, WHEEL_SLOT_ITEM_NAME, item ) )
                       {
                          if ( indi->GetPropertyItem(  filterDeviceName, WHEEL_SLOT_NAME_PROPERTY_NAME, "SLOT_NAME_" + String( item.PropertyValue.ToInt() ), item ) )
                             data.filterName = item.PropertyValue;
                       }
                     }


                     properties << ImagePropertiesFromImageMetadata( data );
               }

            // Generate an initial XISF history record.
            {
               String timeStamp = TimePoint::Now().ToString( 3/*timeItems*/, 0/*precision*/ );
               properties << ImageProperty( "Processing:History",
                                            timeStamp + " Acquired with " + PixInsightVersion::AsString() + '\n' +
                                            timeStamp + " Acquired with " + Module->ReadableVersion() + '\n' +
                                            timeStamp + " Acquired with " + m_instance.Meta()->Id() + " process\n" );
            }

            // Generate FITS COMMENT and HISTORY records
            keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Acquired with " + PixInsightVersion::AsString() )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), "Acquired with " + Module->ReadableVersion() )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), "Acquired with " + m_instance.Meta()->Id() + " process" );

            ICCProfile iccProfile;
            if ( inputFormat.CanStoreICCProfiles() )
               inputFile.ReadICCProfile( iccProfile );

            RGBColorSystem rgbws;
            if ( inputFormat.CanStoreRGBWS() )
               inputFile.ReadRGBWorkingSpace( rgbws );

            DisplayFunction df;
            if ( inputFormat.CanStoreDisplayFunctions() )
               inputFile.ReadDisplayFunction( df );

            ColorFilterArray cfa;
            if ( inputFormat.CanStoreColorFilterArrays() )
               inputFile.ReadColorFilterArray( cfa );

            ImageWindow window;
            bool reusedWindow = false;

            if ( m_instance.p_saveClientImages )
            {
               ImageVariant image;
               image.CreateSharedImage( images[0].options.ieeefpSampleFormat,
                                        false/*isComplex*/,
                                        images[0].options.bitsPerSample );
               if ( !inputFile.ReadImage( image ) )
                  throw CaughtException();

               inputFile.Close();

               FileFormat outputFormat( ".xisf", false/*read*/, true/*write*/ );
               FileFormatInstance outputFile( outputFormat );

               String outputFilePath = m_instance.p_clientDownloadDirectory.Trimmed();
               if ( outputFilePath.IsEmpty() )
               {
                  outputFilePath = PixInsightSettings::GlobalString( "ImageWindow/DownloadsDirectory" );
                  if ( outputFilePath.IsEmpty() ) // this cannot happen
                     outputFilePath = File::SystemTempDirectory();
               }
               if ( !outputFilePath.EndsWith( '/' ) )
                  outputFilePath += '/';
               outputFilePath += m_instance.ClientFileName();
               outputFilePath += ".xisf";
               if ( !m_instance.p_overwriteClientImages )
                  outputFilePath = UniqueFilePath( outputFilePath );

               String outputHints = m_instance.p_clientOutputFormatHints.Trimmed();
               if ( !outputHints.IsEmpty() )
                  outputHints += ' ';
               outputHints += "image-type " + m_instance.CCDFrameTypePrefix( m_instance.p_frameType ).Lowercase();

               if ( !outputFile.Create( outputFilePath, outputHints ) )
                  throw CaughtException();

               for ( auto property : properties )
                  outputFile.WriteProperty( property.key, property.value );

               outputFile.WriteFITSKeywords( keywords );

               if ( iccProfile.IsProfile() )
                  outputFile.WriteICCProfile( iccProfile );

               if ( rgbws != RGBColorSystem::sRGB )
                  outputFile.WriteRGBWorkingSpace( rgbws );

               if ( m_instance.p_autoStretch )
               {
                  int n = image.NumberOfNominalChannels();
                  DVector center( n );
                  DVector sigma( n );
                  for ( int i = 0; i < n; ++i )
                  {
                     center[i] = image.Median( Rect( 0 ), i, i );
                     sigma[i] = 1.4826 * image.MAD( center[i], Rect( 0 ), i, i );
                  }
                  df.SetLinkedRGB( m_instance.p_linkedAutoStretch );
                  df.ComputeAutoStretch( sigma, center );
               }

               if ( !df.IsIdentityTransformation() )
                  outputFile.WriteDisplayFunction( df );

               if ( !cfa.IsEmpty() )
                  outputFile.WriteColorFilterArray( cfa );

               outputFile.SetOptions( images[0].options );

               if ( !outputFile.WriteImage( image ) )
                  throw CaughtException();

               outputFile.Close();

               m_instance.o_clientFilePaths << outputFilePath;

               NewFrameEvent( outputFilePath );

               if ( m_instance.p_openClientImages )
               {
                  Array<ImageWindow> windows = ImageWindow::Open( outputFilePath/*url*/,
                        IsoString()/*id*/, IsoString()/*formatHints*/, false/*asCopy*/, false/*allowMessages*/ );
                  if ( !windows.IsEmpty() )
                     window = windows[0];
               }
            }
            else
            {
               if ( m_instance.p_openClientImages )
               {
                  if ( m_instance.p_reuseImageWindow )
                  {
                     window = ImageWindow::WindowById( m_instance.p_newImageIdTemplate );
                     if ( !window.IsNull() ) // Make sure our window is valid and has not been write-locked by other task.
                        if ( !window.MainView().CanWrite() )
                           window = ImageWindow::Null();
                  }
                  reusedWindow = !window.IsNull();
                  if ( !reusedWindow )
                  {
                     window = ImageWindow( 1, 1, 1,
                                          images[0].options.bitsPerSample,
                                          images[0].options.ieeefpSampleFormat,
                                          false/*color*/,
                                          true/*initialProcessing*/,
                                          m_instance.p_newImageIdTemplate );

                     s_width = s_height = s_numberOfChannels = 0;
                  }

                  View view = window.MainView();

                  ImageVariant image = view.Image();
                  if ( !inputFile.ReadImage( image ) )
                     throw CaughtException();

                  inputFile.Close();

                  for ( auto property : properties )
                     view.SetPropertyValue( property.key, property.value, false/*notify*/,
                                            ViewPropertyAttribute::Storable|ViewPropertyAttribute::Permanent );

                  window.SetKeywords( keywords );

                  if ( inputFormat.CanStoreResolution() )
                     window.SetResolution( images[0].options.xResolution, images[0].options.yResolution, images[0].options.metricResolution );

                  if ( iccProfile )
                     window.SetICCProfile( iccProfile );

                  if ( inputFormat.CanStoreRGBWS() )
                     window.SetRGBWS( rgbws );

                  if ( !cfa.IsEmpty() )
                  {
                     // ### TODO - Cannot be implemented until the core provides support for CFAs
                  }

                  if ( m_instance.p_autoStretch )
                  {
                     int n = image.NumberOfNominalChannels();
                     DVector center( n );
                     DVector sigma( n );
                     for ( int i = 0; i < n; ++i )
                     {
                        center[i] = image.Median( Rect( 0 ), i, i );
                        sigma[i] = 1.4826 * image.MAD( center[i], Rect( 0 ), i, i );
                     }
                     df.SetLinkedRGB( m_instance.p_linkedAutoStretch );
                     df.ComputeAutoStretch( sigma, center );
                     View::stf_list stf( 4 );
                     for ( int i = 0; i < n; ++i )
                        stf[i] = df[i];
                     view.SetScreenTransferFunctions( stf );
                     view.EnableScreenTransferFunctions();
                  }
               }

               m_instance.o_clientFilePaths << String();
            }

            if ( window.IsNull() )
               m_instance.o_clientViewIds << String();
            else
            {
               m_instance.o_clientViewIds << window.MainView().Id();

               ImageVariant image = window.MainView().Image();
               bool geometryChanged = s_width > 0 &&
                  (image.Width() != s_width || image.Height() != s_height || image.NumberOfChannels() != s_numberOfChannels);
               s_width = image.Width();
               s_height = image.Height();
               s_numberOfChannels = image.NumberOfChannels();

               NewFrameEvent( window, reusedWindow, geometryChanged );
            }
         }
      }

      indi->ClearDownloadedImagePath();

      m_running = false;

      EndAcquisitionEvent();
   }
   catch ( ... )
   {
      m_running = false;
      EndAcquisitionEvent();

      try
      {
         throw;
      }
      catch ( ProcessAborted& )
      {
         m_aborted = true;
         indi->MaybeSendNewPropertyItem( m_instance.p_deviceName, CCD_ABORT_EXPOSURE_PROPERTY_NAME, "INDI_SWITCH", CCD_ABORT_EXPOSURE_ITEM_NAME, "ON", true/*async*/ );
         AbortEvent();
         throw;
      }
      catch ( ... )
      {
         throw;
      }
   }
}

// ----------------------------------------------------------------------------

void AbstractINDICCDFrameExecution::Abort()
{
   if ( IsRunning() )
      throw ProcessAborted();
}

// ----------------------------------------------------------------------------

int AbstractINDICCDFrameExecution::s_width = 0;
int AbstractINDICCDFrameExecution::s_height = 0;
int AbstractINDICCDFrameExecution::s_numberOfChannels = 0;

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDICCDFrameInstance.cpp - Released 2019-09-29T12:27:58Z
