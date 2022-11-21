//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.35
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceProcess.cpp - Released 2022-11-21T14:47:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
//
// Copyright (c) 2003-2022 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "NetworkServiceDownloader.h"
#include "NetworkServiceInterface.h"
#include "NetworkServiceLogger.h"
#include "NetworkServiceProcess.h"
#include "NetworkServiceTask.h"

#include <pcl/ErrorHandler.h>
#include <pcl/MetaModule.h>
#include <pcl/NetworkTransfer.h>

namespace pcl
{

// ----------------------------------------------------------------------------

NetworkServiceProcess* TheNetworkServiceProcess = nullptr;

// ----------------------------------------------------------------------------

NetworkServiceProcess::NetworkServiceProcess()
{
   TheNetworkServiceProcess = this;
}

// ----------------------------------------------------------------------------

IsoString NetworkServiceProcess::Id() const
{
   return "NetworkService";
}

// ----------------------------------------------------------------------------

IsoString NetworkServiceProcess::Category() const
{
   return IsoString(); // No category
}

// ----------------------------------------------------------------------------

String NetworkServiceProcess::Description() const
{
   return
   "<html>"
   "</html>";
}

// ----------------------------------------------------------------------------

String NetworkServiceProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/NetworkService.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* NetworkServiceProcess::DefaultInterface() const
{
   return TheNetworkServiceInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* NetworkServiceProcess::Create() const
{
   return nullptr;
}

// ----------------------------------------------------------------------------

ProcessImplementation* NetworkServiceProcess::Clone( const ProcessImplementation& p ) const
{
   return nullptr;
}

// ----------------------------------------------------------------------------

bool NetworkServiceProcess::IsAssignable() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool NetworkServiceProcess::CanProcessIPCMessages() const
{
   return true;
}

// ----------------------------------------------------------------------------

void NetworkServiceProcess::IPCStart( int instance, const IsoString& messageUID, const String& parameters ) const
{
   m_worker.Start( parameters );
}

// ----------------------------------------------------------------------------

void NetworkServiceProcess::IPCStop( int instance, const IsoString& messageUID ) const
{
   m_worker.Stop();
}

// ----------------------------------------------------------------------------

void NetworkServiceProcess::IPCSetParameters( int instance, const IsoString& messageUID, const String& parameters ) const
{
   m_worker.Stop();
   m_worker.Start( parameters );
}

// ----------------------------------------------------------------------------

int NetworkServiceProcess::IPCStatus( int instance, const IsoString& messageUID ) const
{
   return m_worker.Status();
}

// ----------------------------------------------------------------------------

void NetworkServiceProcess::Start( const String& parameters, NetworkServiceLogger* logger ) const
{
   m_worker.Start( parameters, logger );
}

// ----------------------------------------------------------------------------

void NetworkServiceProcess::Stop() const
{
   m_worker.Stop();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FileLogger : public NetworkServiceLogger
{
public:

   FileLogger( const String& filePath )
      : m_filePath( filePath )
   {
      if ( m_filePath.IsEmpty() )
         m_filePath = File::SystemTempDirectory() + TimePoint::Now().ToString( "/NS%Y%M%D%h%m%s.log" );
   }

   FileLogger( const FileLogger& ) = delete;

   virtual ~FileLogger() noexcept( false )
   {
      End();
   }

   void Begin() override
   {
      try
      {
         m_file.CreateForWriting( m_filePath );
      }
      catch ( ... )
      {
      }
   }

   void End() override
   {
      try
      {
         if ( m_file.IsOpen() )
         {
            m_file.Flush();
            m_file.Close();
         }
      }
      catch ( ... )
      {
      }
   }

   void Section( const String& text ) override
   {
      try
      {
         if ( m_file.IsOpen() )
         {
            m_file.OutTextLn();
            m_file.OutTextLn( TimestampPrefix() + text.ToUTF8() );
         }
      }
      catch ( ... )
      {
      }
   }

   void Line( const String& text ) override
   {
      try
      {
         if ( m_file.IsOpen() )
            m_file.OutTextLn( TimestampPrefix() + text.ToUTF8() );
      }
      catch ( ... )
      {
      }
   }

   void Warning( const String& text ) override
   {
      try
      {
         if ( m_file.IsOpen() )
            m_file.OutTextLn( TimestampPrefix() + "** Warning: " + text.ToUTF8() );
      }
      catch ( ... )
      {
      }
   }

   void Error( const String& text ) override
   {
      try
      {
         if ( m_file.IsOpen() )
            m_file.OutTextLn( TimestampPrefix() + "*** Error: " + text.ToUTF8() );
      }
      catch ( ... )
      {
      }
   }

private:

   String m_filePath;
   File   m_file;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

int NetworkServiceProcess::Worker::Status() const
{
   volatile AutoLock lock( m_mutex );
   return m_running ? 1 : (m_error ? -1 : 0);
}

bool NetworkServiceProcess::Worker::IsRunning() const
{
   volatile AutoLock lock( m_mutex );
   return m_running;
}

bool NetworkServiceProcess::Worker::IsError() const
{
   volatile AutoLock lock( m_mutex );
   return m_error;
}

bool NetworkServiceProcess::Worker::StopRequested() const
{
   volatile AutoLock lock( m_mutex );
   return m_stop;
}

void NetworkServiceProcess::Worker::Stop()
{
   volatile AutoLock lock( m_mutex );
   m_stop = true;
}

void NetworkServiceProcess::Worker::SetRunning()
{
   volatile AutoLock lock( m_mutex );
   m_running = true;
}

void NetworkServiceProcess::Worker::SetNotRunning()
{
   volatile AutoLock lock( m_mutex );
   m_running = false;
}

void NetworkServiceProcess::Worker::SetError()
{
   volatile AutoLock lock( m_mutex );
   m_error = true;
}

void NetworkServiceProcess::Worker::ClearStatus()
{
   volatile AutoLock lock( m_mutex );
   m_running = m_error = m_stop = false;
}

// ----------------------------------------------------------------------------

void NetworkServiceProcess::Worker::GetParameters( const String& parameters )
{
   StringList items;
   parameters.Break( items, ',', true/*trim*/ );

   for ( const String& item : items )
   {
      StringList tokens;
      item.Break( tokens, '=', true/*trim*/ );
      if ( tokens.Length() != 2 )
         continue;

      if ( tokens[0] == "taskProviderURL" )
         m_taskProviderURL = tokens[1];
      else if ( tokens[0] == "workerName" )
         m_workerName = tokens[1];
      else if ( tokens[0] == "taskType" )
         m_taskType = tokens[1];
      else if ( tokens[0] == "userName" )
         m_userName = tokens[1];
      else if ( tokens[0] == "userPassword" )
         m_userPassword = tokens[1];
      else if ( tokens[0] == "logFilePath" )
         m_logFilePath = tokens[1];
      else if ( tokens[0] == "waitTimeSecs" )
         tokens[1].TryToUInt( m_waitTimeSecs );
      else if ( tokens[0] == "debug" )
         tokens[1].TryToBool( m_debug );
   }
}

void NetworkServiceProcess::Worker::ValidateParameters()
{
   if ( m_taskProviderURL.IsEmpty() )
      throw Error( "No task provider has been specified." );
   if ( m_taskType.IsEmpty() )
      throw Error( "No task type has been specified." );
   if ( m_userName.IsEmpty() )
      throw Error( "No user name has been specified." );

   m_waitTimeSecs = Range( m_waitTimeSecs, 1u, 60u );
}

// ----------------------------------------------------------------------------

void NetworkServiceProcess::Worker::Start( const String& parameters, NetworkServiceLogger* log )
{
   if ( IsRunning() )
      return;

   m_taskProviderURL.Clear();
   m_workerName = "anonymous";
   m_taskType.Clear();
   m_userName.Clear();
   m_userPassword.Clear();
   m_logFilePath.Clear();
   m_downloadData.Clear();
   m_waitTimeSecs = 5;
   m_debug = false;

   GetParameters( parameters );

   AutoPointer<NetworkServiceLogger> logAuto;
   m_log = log;
   if ( m_log == nullptr )
      logAuto = m_log = new FileLogger( m_logFilePath );

   try
   {
      ClearStatus();

      SetRunning();

      m_log->Begin();

      ValidateParameters();

      m_log->Section( "Performing network service:" );
      m_log->Line( "Task provider: " + m_taskProviderURL );
      m_log->Line( "Task type:     " + m_taskType );
      m_log->Line( "User name:     " + m_userName );

      for ( ;; )
      {
         Module->ProcessEvents();

         if ( StopRequested() )
         {
            m_log->Section( "<* stop *>" );
            break;
         }

         try
         {
            m_log->SetStatus( String() );
            QueryTask();
            PerformTask();
            Wait();
         }
         catch ( const Exception& x )
         {
            m_log->Error( x.FormatInfo() );
         }
      }
   }
   catch ( ... )
   {
      SetError();

      try
      {
         throw;
      }
      ERROR_HANDLER
   }

   SetNotRunning();

   m_log->SetStatus( String() );
   m_log->End();
}

// ----------------------------------------------------------------------------

void NetworkServiceProcess::Worker::QueryTask()
{
   m_log->SetStatus( "Sending task query" );
   NetworkTransfer transfer;
   transfer.SetURL( m_taskProviderURL );
   transfer.SetSSL( true/*enableSSL*/, false/*forceSSL*/ );
   NetworkServiceDownloader download;
   download.Attach( transfer );
   if ( !transfer.POST( "userName=" + m_userName
                      + "&userPassword=" + m_userPassword
                      + "&taskType=" + m_taskType ) )
      if ( !StopRequested() )
         throw Error( "POST operation failed: " + transfer.ErrorInformation() );
   m_downloadData = download.data;
}

// ----------------------------------------------------------------------------

void NetworkServiceProcess::Worker::PerformTask()
{
   IsoString serverResponse( reinterpret_cast<const char*>( m_downloadData.Begin() ),
                             reinterpret_cast<const char*>( m_downloadData.End() ) );
   IsoStringList tokens;
   serverResponse.Break( tokens, '\n', true/*trim*/ );
   tokens.Remove( IsoString() );

   if ( tokens.IsEmpty() )
      return;

   if ( tokens[0].CaseFolded() != "ok" )
   {
      m_log->Line( serverResponse.UTF8ToUTF16() );
      return;
   }

   TaskData data;
   data.workerName = m_workerName;

   for ( IsoStringList::const_iterator i = tokens.Begin(); ++i != tokens.End(); )
   {
      IsoStringList items;
      i->Break( items, '=', true/*trim*/ );
      if ( items.Length() != 2 )
         throw Error( "Invalid server response: '<raw>" + *i + "</raw>'" );
      if ( items[0] == "serialNumber" )
         data.serialNumber = items[1];
      else if ( items[0] == "requestUTC" )
         data.requestUTC = items[1];
      else if ( items[0] == "taskType" )
         data.taskType = items[1];
      else if ( items[0] == "taskParams" )
         data.parameters = items[1].URLDecoded().UTF8ToUTF16();
      else if ( items[0] == "callbackURL" )
         data.callbackURL = items[1].URLDecoded();
   }

   if ( data.serialNumber.IsEmpty() || data.requestUTC.IsEmpty() || data.taskType.IsEmpty() )
      throw Error( "Missing required data item(s) in server response" );
   if ( data.callbackURL.IsEmpty() )
      throw Error( "Missing callback URL" );

   m_log->Section( "Performing new task: " );
   m_log->Line( "Serial number   : " + data.serialNumber );
   m_log->Line( "Request UTC     : " + data.requestUTC );
   m_log->Line( "Task type       : " + data.taskType );
   m_log->Line( "Callback URL    : " + data.callbackURL );

   m_log->SetStatus( "Performing task: " + data.taskType );

   NetworkServiceTask* task = nullptr;
   try
   {
      task = NetworkServiceTask::TaskByType( data.taskType );
      if ( task == nullptr )
         throw Error( "Unsupported task type: " + data.taskType );

      task->Perform( data, *m_log );
   }
   catch ( const Exception& x )
   {
      m_log->Section( "Sending error report: " + data.callbackURL );
      NetworkTransfer transfer;
      transfer.SetURL( data.callbackURL );
      transfer.SetSSL( true/*enableSSL*/, false/*forceSSL*/ );
      NetworkServiceDownloader download;
      download.Attach( transfer );
      String postVars;
      postVars << "serialNumber=" << data.serialNumber
               << "&taskType=" << data.taskType
               << "&workerName=" << (data.workerName.IsEmpty() ? "anonymous" : data.workerName)
               << "&status=ERROR"
               << ((task != nullptr) ? "&taskVersion=" + task->TaskVersion() : String())
               << "&errorMessage=" << x.FormatInfo().ToUTF8().URLEncoded();
      if ( !transfer.POST( postVars ) )
         if ( !StopRequested() )
            m_log->Warning( "POST operation failed: " + data.callbackURL );
      throw;
   }
}

// ----------------------------------------------------------------------------

void NetworkServiceProcess::Worker::Wait()
{
   m_log->SetStatus( "Idle" );
   const unsigned stepSize = 25; // ms
   for ( unsigned nsteps = m_waitTimeSecs*1000/stepSize, step = 0; step < nsteps; ++step )
   {
      Module->ProcessEvents();
      if ( StopRequested() )
         break;
      pcl::Sleep( stepSize );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NetworkServiceProcess.cpp - Released 2022-11-21T14:47:18Z
