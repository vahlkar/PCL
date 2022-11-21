//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.35
// ----------------------------------------------------------------------------
// Standard APASS Process Module Version 1.1.4
// ----------------------------------------------------------------------------
// APASSProcess.cpp - Released 2022-11-21T14:47:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard APASS PixInsight module.
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

#include "APASSInterface.h"
#include "APASSParameters.h"
#include "APASSProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/MetaModule.h>
#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

APASSProcess* TheAPASSProcess = nullptr;

struct DatabaseData
{
   StringList                        databaseFilePaths;
   ReferenceArray<APASSDatabaseFile> databases;
   AtomicInt                         databasesInitialized;
   Mutex                             mutex;
   bool                              preferencesLoaded = false;
};

static DatabaseData s_data[ 2 ]; // DR9, DR10

static int DRParameterValueToDRIndex( pcl_enum p )
{
   switch( p )
   {
   case ADataRelease::DR9:  return 0;
   case ADataRelease::DR10: return 1;
   default:
      throw Error( "Internal error: Invalid APASS data release parameter." );
   }
}

static IsoString DRIndexToDRName( int dr )
{
   switch ( dr )
   {
   case 0: return "DR9";
   case 1: return "DR10";
   default:
      throw Error( "Internal error: Invalid APASS data release index." );
   }
}

static IsoString PreferencesKeyForDRIndex( int dr, int item )
{
   return DRIndexToDRName( dr ) + IsoString().Format( "DatabaseFilePath%02d", item );
}

// ----------------------------------------------------------------------------

APASSProcess::APASSProcess()
{
   TheAPASSProcess = this;

   new ACommand( this );
   new ACenterRA( this );
   new ACenterDec( this );
   new ARadius( this );
   new AMagnitudeLow( this );
   new AMagnitudeHigh( this );
   new ASourceLimit( this );
   new ARequiredFlags( this );
   new AInclusionFlags( this );
   new AExclusionFlags( this );
   new AVerbosity( this );
   new ADataRelease( this );
   new ASortBy( this );
   new AGenerateTextOutput( this );
   new AGenerateBinaryOutput( this );
   new ATextFormat( this );
   new ATextHeaders( this );
   new AOutputFilePath( this );
   new ADatabaseFilePaths( this );
   new ADatabaseFilePath( TheADatabaseFilePathsParameter );

   new ASources( this );
   new ASourceRA( TheASourcesParameter );
   new ASourceDec( TheASourcesParameter );
   new ASourceMag_V( TheASourcesParameter );
   new ASourceMag_B( TheASourcesParameter );
   new ASourceMag_g( TheASourcesParameter );
   new ASourceMag_r( TheASourcesParameter );
   new ASourceMag_i( TheASourcesParameter );
   new ASourceMag_z_s( TheASourcesParameter );
   new ASourceMag_V_Error( TheASourcesParameter );
   new ASourceMag_B_Error( TheASourcesParameter );
   new ASourceMag_g_Error( TheASourcesParameter );
   new ASourceMag_r_Error( TheASourcesParameter );
   new ASourceMag_i_Error( TheASourcesParameter );
   new ASourceMag_z_s_Error( TheASourcesParameter );
   new ASourceFlags( TheASourcesParameter );
   new AExcessCount( this );
   new ARejectCount( this );
   new ATimeTotal( this );
   new ATimeIO( this );
   new ACountIO( this );
   new ATimeUncompress( this );
   new ATimeDecode( this );
   new AIsValid( this );
   new AOutputDataRelease( this );
   new ADatabaseMagnitudeLow( this );
   new ADatabaseMagnitudeHigh( this );
}

// ----------------------------------------------------------------------------

IsoString APASSProcess::Id() const
{
   return "APASS";
}

// ----------------------------------------------------------------------------

IsoString APASSProcess::Category() const
{
   return "StarCatalogs,Photometry,Global";
}

// ----------------------------------------------------------------------------

uint32 APASSProcess::Version() const
{
   return 1;
}

// ----------------------------------------------------------------------------

String APASSProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String APASSProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/APASS.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* APASSProcess::DefaultInterface() const
{
   return TheAPASSInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* APASSProcess::Create() const
{
   return new APASSInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* APASSProcess::Clone( const ProcessImplementation& p ) const
{
   const APASSInstance* instPtr = dynamic_cast<const APASSInstance*>( &p );
   return (instPtr != nullptr) ? new APASSInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool APASSProcess::PrefersGlobalExecution() const
{
   return true; // this is a global process
}

// ----------------------------------------------------------------------------

bool APASSProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: APASS [<arg_list>]"
"\n"
"\n--interface"
"\n"
"\n      Launches the interface of this process."
"\n"
"\n--help"
"\n"
"\n      Displays this help and exits."
"</raw>" );
}

int APASSProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::NoItems );

   APASSInstance instance( this );
   bool launchInterface = false;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "-interface" )
            launchInterface = true;
         else if ( arg.Id() == "-help" )
         {
            ShowHelp();
            return 0;
         }
         else
            throw Error( "Unknown argument: " + arg.Token() );
      }
      else if ( arg.IsItemList() )
         throw Error( "Invalid non-parametric argument: " + arg.Token() );
   }

   if ( launchInterface || arguments.IsEmpty() )
      instance.LaunchInterface();
   else
      instance.LaunchGlobal();

   return 0;
}

// ----------------------------------------------------------------------------

bool APASSProcess::CanProcessIPCMessages() const
{
   return true;
}

// ----------------------------------------------------------------------------

void APASSProcess::IPCStart( int instance, const IsoString& messageUID, const String& parameters ) const
{
   m_ipcWorker.Start( parameters );
}

// ----------------------------------------------------------------------------

void APASSProcess::IPCStop( int instance, const IsoString& messageUID ) const
{
   m_ipcWorker.Stop();
}

// ----------------------------------------------------------------------------

void APASSProcess::IPCSetParameters( int instance, const IsoString& messageUID, const String& parameters ) const
{
   m_ipcWorker.Stop();
   m_ipcWorker.Start( parameters );
}

// ----------------------------------------------------------------------------

int APASSProcess::IPCStatus( int instance, const IsoString& messageUID ) const
{
   return m_ipcWorker.Status();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void APASSProcess::IPCWorker::Start( const String& parameters )
{
   if ( IsRunning() )
      return;

   SetRunning();
   ClearData();
   ClearStatus();

   try
   {
      GetParameters( parameters );

      if ( m_task == "configure" )
      {
         TheAPASSProcess->SetDatabaseFilePaths( m_dataRelease, m_xpsdFilePaths );
         TheAPASSProcess->EnsureDatabasesInitialized( m_dataRelease, m_verbosity );

         if ( !TheAPASSProcess->IsValid( m_dataRelease ) )
            SetError();
         if ( TheAPASSInterface != nullptr )
            TheAPASSInterface->UpdateControls();
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

   ClearData();
   SetNotRunning();
}

// ----------------------------------------------------------------------------

void APASSProcess::IPCWorker::Stop()
{
   // All tasks are atomic
}

// ----------------------------------------------------------------------------

int APASSProcess::IPCWorker::Status() const
{
   volatile AutoLock lock( m_mutex );
   return m_running ? 1 : (m_error ? -1 : 0);
}

// ----------------------------------------------------------------------------

bool APASSProcess::IPCWorker::IsRunning() const
{
   volatile AutoLock lock( m_mutex );
   return m_running;
}

// ----------------------------------------------------------------------------

bool APASSProcess::IPCWorker::IsError() const
{
   volatile AutoLock lock( m_mutex );
   return m_error;
}

// ----------------------------------------------------------------------------

void APASSProcess::IPCWorker::GetParameters( const String& parameters )
{
   StringList items;
   parameters.Break( items, ',', true/*trim*/ );

   String xpsdBaseDir;
   for ( const String& item : items )
   {
      StringList tokens;
      item.Break( tokens, '=', true/*trim*/ );
      if ( tokens.Length() != 2 )
         continue;

      if ( tokens[0] == "task" )
         m_task = tokens[1];
      else if ( tokens[0] == "dataRelease" )
      {
         if ( tokens[1].CompareIC( "DR9" ) == 0 )
            m_dataRelease = ADataRelease::DR9;
         else if ( tokens[1].CompareIC( "DR10" ) == 0 )
            m_dataRelease = ADataRelease::DR10;
      }
      else if ( tokens[0] == "xpsdBaseDir" )
         xpsdBaseDir = tokens[1];
      else if ( tokens[0] == "verbosity" )
         tokens[1].TryToInt( m_verbosity );
   }

   if ( !xpsdBaseDir.IsEmpty() )
   {
      if ( !xpsdBaseDir.EndsWith( '/' ) )
         xpsdBaseDir << '/';
      if ( !File::DirectoryExists( xpsdBaseDir ) )
         throw Error( String( "Failure to configure the APASS XPSD server: " ) + "Nonexistent directory specified: " + xpsdBaseDir );

      pcl::FindFileInfo info;
      for ( File::Find f( xpsdBaseDir + "*.xpsd" ); f.NextItem( info ); )
         if ( !info.IsDirectory() && !info.attributes.IsFlagSet( FileAttribute::SymbolicLink ) )
            m_xpsdFilePaths << File::FullPath( xpsdBaseDir + info.name );

      if ( m_xpsdFilePaths.IsEmpty() )
         throw Error( String( "Failure to configure the APASS XPSD server: " ) + "No XPSD database files found: " + xpsdBaseDir );
   }
}

// ----------------------------------------------------------------------------

void APASSProcess::IPCWorker::SetRunning()
{
   volatile AutoLock lock( m_mutex );
   m_running = true;
}

// ----------------------------------------------------------------------------

void APASSProcess::IPCWorker::SetNotRunning()
{
   volatile AutoLock lock( m_mutex );
   m_running = false;
}

// ----------------------------------------------------------------------------

void APASSProcess::IPCWorker::SetError()
{
   volatile AutoLock lock( m_mutex );
   m_error = true;
}

// ----------------------------------------------------------------------------

void APASSProcess::IPCWorker::ClearData()
{
   volatile AutoLock lock( m_mutex );
   m_task.Clear();
   m_dataRelease = ADataRelease::DR10;
   m_xpsdFilePaths.Clear();
   m_verbosity = 2; // full
}

// ----------------------------------------------------------------------------

void APASSProcess::IPCWorker::ClearStatus()
{
   volatile AutoLock lock( m_mutex );
   m_running = m_error = false;
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void APASSProcess::SetDatabaseFilePaths( int dr, const StringList& paths )
{
   CloseDatabases( dr );
   int dri = DRParameterValueToDRIndex( dr );
   s_data[dri].databaseFilePaths = paths;
   SavePreferences( dr );
}

// ----------------------------------------------------------------------------

const StringList& APASSProcess::DatabaseFilePaths( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   if ( s_data[dri].databaseFilePaths.IsEmpty() )
      const_cast<APASSProcess*>( this )->LoadPreferences( dr );
   return s_data[dri].databaseFilePaths;
}

// ----------------------------------------------------------------------------

bool APASSProcess::IsValid( int dr ) const
{
   return !Databases( dr ).IsEmpty();
}

// ----------------------------------------------------------------------------

bool APASSProcess::HasDatabaseFiles( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   if ( s_data[dri].databaseFilePaths.IsEmpty() )
      const_cast<APASSProcess*>( this )->LoadPreferences( dr );
   return !s_data[dri].databaseFilePaths.IsEmpty();
}

// ----------------------------------------------------------------------------

void APASSProcess::EnsureDatabasesInitialized( int dr, int verbosity )
{
   int dri = DRParameterValueToDRIndex( dr );
   DatabaseData& data = s_data[dri];
   if ( !data.databasesInitialized )
   {
      volatile AutoLock lock( data.mutex );
      if ( data.databasesInitialized.Load() == 0 )
      {
         if ( data.databaseFilePaths.IsEmpty() )
            LoadPreferences( dr );

         try
         {
            data.databases.Destroy();

            if ( data.databaseFilePaths.IsEmpty() )
               throw Error( "No database files have been selected." );

            Console console;
            if ( verbosity > 1 )
            {
               console.Show();
               console.WriteLn( "<end><cbr><br>Installing APASS " + DRIndexToDRName( dri ) + " database files...\n" );
               Module->ProcessEvents();
            }

            for ( const String& filePath : data.databaseFilePaths )
            {
               APASSDatabaseFile* file = new APASSDatabaseFile( filePath );
               data.databases.Add( file );
               if ( file->DataRelease() != DRIndexToDRName( dri ) )
                  throw Error( "Unexpected APASS " + file->DataRelease() + " database file; "
                               "expected a APASS " + DRIndexToDRName( dri ) + " file: <raw>" + file->FilePath() + "</raw>" );
               if ( verbosity > 1 )
               {
                  if ( data.databases.Length() == 1 )
                     console.WriteLn( "<end><cbr>"
                        + file->Metadata().description.Trimmed()
                        + "<br><br>"
                        + file->Metadata().copyright.Trimmed()
                        + "<br>" );

                  console.WriteLn( "<b><raw>" + file->FilePath() + "</raw></b>\n"
                     +  "Database version .... " + file->Metadata().databaseIdentifier + ' ' + file->Metadata().databaseVersion + '\n'
                     + String().Format(
                        "Magnitude range ..... (%.2f,%.2f]\n"
                        "Total sources ....... %llu\n"
                        "Total index nodes ... %u"
                        , file->MagnitudeLow()
                        , file->MagnitudeHigh()
                        , file->Statistics().totalSources
                        , file->Statistics().totalNodes )
                     + " (" + File::SizeAsString( file->Statistics().totalNodes * 48 ) + ")\n"
                  );

                  Module->ProcessEvents();
               }
            }

            data.databases.Sort(
               []( const APASSDatabaseFile& d1, const APASSDatabaseFile& d2 )
               {
                  return d1.MagnitudeHigh() < d2.MagnitudeHigh();
               } );
         }
         catch ( ... )
         {
            data.databases.Destroy();
            if ( verbosity > 0 )
            {
               try
               {
                  throw;
               }
               ERROR_HANDLER
            }
         }

         data.databasesInitialized.Store( 1 );
      }
   }
}

// ----------------------------------------------------------------------------

void APASSProcess::CloseDatabases( int dr )
{
   int dri = DRParameterValueToDRIndex( dr );
   s_data[dri].databases.Destroy();
   s_data[dri].databasesInitialized = 0;
}

// ----------------------------------------------------------------------------

const ReferenceArray<APASSDatabaseFile>& APASSProcess::Databases( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].databases;
}

// ----------------------------------------------------------------------------

float APASSProcess::MagnitudeLow( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].databases.IsEmpty() ? .0F : s_data[dri].databases.Begin()->MagnitudeLow();
}

// ----------------------------------------------------------------------------

float APASSProcess::MagnitudeHigh( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].databases.IsEmpty() ? .0F : s_data[dri].databases.ReverseBegin()->MagnitudeHigh();
}

// ----------------------------------------------------------------------------

bool APASSProcess::PreferencesLoaded( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].preferencesLoaded;
}

// ----------------------------------------------------------------------------

void APASSProcess::LoadPreferences( int dr )
{
   int dri = DRParameterValueToDRIndex( dr );
   s_data[dri].databaseFilePaths.Clear();
   for ( int i = 0;; ++i )
   {
      String filePath;
      if ( !Settings::Read( PreferencesKeyForDRIndex( dri, i ), filePath ) )
         break;
      s_data[dri].databaseFilePaths << filePath;
   }
   s_data[dri].preferencesLoaded = true;
}

// ----------------------------------------------------------------------------

void APASSProcess::SavePreferences( int dr )
{
   int dri = DRParameterValueToDRIndex( dr );
   for ( int i = 0;; ++i )
   {
      IsoString key = PreferencesKeyForDRIndex( dri, i );
      String filePath;
      if ( !Settings::Read( key, filePath ) )
         break;
      Settings::Remove( key );
   }

   int i = 0;
   for ( const String& filePath : s_data[dri].databaseFilePaths )
      Settings::Write( PreferencesKeyForDRIndex( dri, i++ ), filePath );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF APASSProcess.cpp - Released 2022-11-21T14:47:17Z
