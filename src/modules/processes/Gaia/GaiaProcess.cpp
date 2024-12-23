//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard Gaia Process Module Version 1.2.6
// ----------------------------------------------------------------------------
// GaiaProcess.cpp - Released 2024-12-23T11:33:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard Gaia PixInsight module.
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

#include "GaiaInterface.h"
#include "GaiaParameters.h"
#include "GaiaProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/MetaModule.h>
#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

GaiaProcess* TheGaiaProcess = nullptr;

struct DatabaseData
{
   StringList                       databaseFilePaths;
   ReferenceArray<GaiaDatabaseFile> databases;
   AtomicInt                        databasesInitialized;
   Mutex                            mutex;
   bool                             preferencesLoaded = false;
};

static DatabaseData s_data[ 4 ]; // DR2, EDR3, DR3, DR3SP

static int DRParameterValueToDRIndex( pcl_enum p )
{
   switch( p )
   {
   case GDataRelease::DR2:   return 0;
   case GDataRelease::EDR3:  return 1;
   case GDataRelease::DR3:   return 2;
   case GDataRelease::DR3SP: return 3;
   default:
      throw Error( "Internal error: Invalid Gaia data release parameter." );
   }
}

static IsoString DRIndexToDRName( int dr )
{
   switch ( dr )
   {
   case 0: return "DR2";
   case 1: return "EDR3";
   case 2: return "DR3";
   case 3: return "DR3SP";
   default:
      throw Error( "Internal error: Invalid Gaia data release index." );
   }
}

static IsoString PreferencesKeyForDRIndex( int dr, int item )
{
   return DRIndexToDRName( dr ) + IsoString().Format( "DatabaseFilePath%02d", item );
}

// ----------------------------------------------------------------------------

GaiaProcess::GaiaProcess()
{
   TheGaiaProcess = this;

   new GCommand( this );
   new GCenterRA( this );
   new GCenterDec( this );
   new GRadius( this );
   new GMagnitudeLow( this );
   new GMagnitudeHigh( this );
   new GSourceLimit( this );
   new GRequiredFlags( this );
   new GInclusionFlags( this );
   new GExclusionFlags( this );
   new GNormalizeSpectrum( this );
   new GPhotonFluxUnits( this );
   new GVerbosity( this );
   new GDataRelease( this );
   new GSortBy( this );
   new GGenerateTextOutput( this );
   new GGenerateBinaryOutput( this );
   new GTextFormat( this );
   new GTextHeaders( this );
   new GOutputFilePath( this );
   new GDatabaseFilePaths( this );
   new GDatabaseFilePath( TheGDatabaseFilePathsParameter );

   new GSources( this );
   new GSourceRA( TheGSourcesParameter );
   new GSourceDec( TheGSourcesParameter );
   new GSourceParx( TheGSourcesParameter );
   new GSourcePMRA( TheGSourcesParameter );
   new GSourcePMDec( TheGSourcesParameter );
   new GSourceMagG( TheGSourcesParameter );
   new GSourceMagBP( TheGSourcesParameter );
   new GSourceMagRP( TheGSourcesParameter );
   new GSourceFlags( TheGSourcesParameter );
   new GSourceFlux( TheGSourcesParameter );
   new GExcessCount( this );
   new GRejectCount( this );
   new GTimeTotal( this );
   new GTimeIO( this );
   new GCountIO( this );
   new GTimeUncompress( this );
   new GTimeDecode( this );
   new GIsValid( this );
   new GOutputDataRelease( this );
   new GDatabaseMagnitudeLow( this );
   new GDatabaseMagnitudeHigh( this );
   new GDatabaseHasMeanSpectrumData( this );
   new GDatabaseSpectrumStart( this );
   new GDatabaseSpectrumStep( this );
   new GDatabaseSpectrumCount( this );
   new GDatabaseSpectrumBits( this );
}

// ----------------------------------------------------------------------------

IsoString GaiaProcess::Id() const
{
   return "Gaia";
}

// ----------------------------------------------------------------------------

IsoString GaiaProcess::Category() const
{
   return "StarCatalogs,Astrometry,Global";
}

// ----------------------------------------------------------------------------

uint32 GaiaProcess::Version() const
{
   return 1;
}

// ----------------------------------------------------------------------------

String GaiaProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String GaiaProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Gaia.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* GaiaProcess::DefaultInterface() const
{
   return TheGaiaInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* GaiaProcess::Create() const
{
   return new GaiaInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* GaiaProcess::Clone( const ProcessImplementation& p ) const
{
   const GaiaInstance* instPtr = dynamic_cast<const GaiaInstance*>( &p );
   return (instPtr != nullptr) ? new GaiaInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool GaiaProcess::PrefersGlobalExecution() const
{
   return true; // this is a global process
}

// ----------------------------------------------------------------------------

bool GaiaProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: Gaia [<arg_list>]"
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

int GaiaProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::NoItems );

   GaiaInstance instance( this );
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

bool GaiaProcess::CanProcessIPCMessages() const
{
   return true;
}

// ----------------------------------------------------------------------------

void GaiaProcess::IPCStart( int instance, const IsoString& messageUID, const String& parameters ) const
{
   m_ipcWorker.Start( parameters );
}

// ----------------------------------------------------------------------------

void GaiaProcess::IPCStop( int instance, const IsoString& messageUID ) const
{
   m_ipcWorker.Stop();
}

// ----------------------------------------------------------------------------

void GaiaProcess::IPCSetParameters( int instance, const IsoString& messageUID, const String& parameters ) const
{
   m_ipcWorker.Stop();
   m_ipcWorker.Start( parameters );
}

// ----------------------------------------------------------------------------

int GaiaProcess::IPCStatus( int instance, const IsoString& messageUID ) const
{
   return m_ipcWorker.Status();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void GaiaProcess::IPCWorker::Start( const String& parameters )
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
         TheGaiaProcess->SetDatabaseFilePaths( m_dataRelease, m_xpsdFilePaths );
         TheGaiaProcess->EnsureDatabasesInitialized( m_dataRelease, m_verbosity );

         if ( !TheGaiaProcess->IsValid( m_dataRelease ) )
            SetError();
         if ( TheGaiaInterface != nullptr )
            TheGaiaInterface->UpdateControls();
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

void GaiaProcess::IPCWorker::Stop()
{
   // All tasks are atomic
}

// ----------------------------------------------------------------------------

int GaiaProcess::IPCWorker::Status() const
{
   volatile AutoLock lock( m_mutex );
   return m_running ? 1 : (m_error ? -1 : 0);
}

// ----------------------------------------------------------------------------

bool GaiaProcess::IPCWorker::IsRunning() const
{
   volatile AutoLock lock( m_mutex );
   return m_running;
}

// ----------------------------------------------------------------------------

bool GaiaProcess::IPCWorker::IsError() const
{
   volatile AutoLock lock( m_mutex );
   return m_error;
}

// ----------------------------------------------------------------------------

void GaiaProcess::IPCWorker::GetParameters( const String& parameters )
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
         if ( tokens[1].CompareIC( "DR3" ) == 0 )
            m_dataRelease = GDataRelease::DR3;
         else if ( tokens[1].CompareIC( "DR3SP" ) == 0 )
            m_dataRelease = GDataRelease::DR3SP;
         else if ( tokens[1].CompareIC( "EDR3" ) == 0 )
            m_dataRelease = GDataRelease::EDR3;
         else if ( tokens[1].CompareIC( "DR2" ) == 0 )
            m_dataRelease = GDataRelease::DR2;
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
         throw Error( String( "Failure to configure the Gaia XPSD server: " ) + "Nonexistent directory specified: " + xpsdBaseDir );

      pcl::FindFileInfo info;
      for ( File::Find f( xpsdBaseDir + "*.xpsd" ); f.NextItem( info ); )
         if ( !info.IsDirectory() && !info.attributes.IsFlagSet( FileAttribute::SymbolicLink ) )
            m_xpsdFilePaths << File::FullPath( xpsdBaseDir + info.name );

      if ( m_xpsdFilePaths.IsEmpty() )
         throw Error( String( "Failure to configure the Gaia XPSD server: " ) + "No XPSD database files found: " + xpsdBaseDir );
   }
}

// ----------------------------------------------------------------------------

void GaiaProcess::IPCWorker::SetRunning()
{
   volatile AutoLock lock( m_mutex );
   m_running = true;
}

// ----------------------------------------------------------------------------

void GaiaProcess::IPCWorker::SetNotRunning()
{
   volatile AutoLock lock( m_mutex );
   m_running = false;
}

// ----------------------------------------------------------------------------

void GaiaProcess::IPCWorker::SetError()
{
   volatile AutoLock lock( m_mutex );
   m_error = true;
}

// ----------------------------------------------------------------------------

void GaiaProcess::IPCWorker::ClearData()
{
   volatile AutoLock lock( m_mutex );
   m_task.Clear();
   m_dataRelease = GDataRelease::DR3;
   m_xpsdFilePaths.Clear();
   m_verbosity = 2; // full
}

// ----------------------------------------------------------------------------

void GaiaProcess::IPCWorker::ClearStatus()
{
   volatile AutoLock lock( m_mutex );
   m_running = m_error = false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void GaiaProcess::SetDatabaseFilePaths( int dr, const StringList& paths )
{
   CloseDatabases( dr );
   int dri = DRParameterValueToDRIndex( dr );
   s_data[dri].databaseFilePaths = paths;
   SavePreferences( dr );
}

// ----------------------------------------------------------------------------

const StringList& GaiaProcess::DatabaseFilePaths( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   if ( s_data[dri].databaseFilePaths.IsEmpty() )
      const_cast<GaiaProcess*>( this )->LoadPreferences( dr );
   return s_data[dri].databaseFilePaths;
}

// ----------------------------------------------------------------------------

bool GaiaProcess::IsValid( int dr ) const
{
   return !Databases( dr ).IsEmpty();
}

// ----------------------------------------------------------------------------

bool GaiaProcess::HasDatabaseFiles( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   if ( s_data[dri].databaseFilePaths.IsEmpty() )
      const_cast<GaiaProcess*>( this )->LoadPreferences( dr );
   return !s_data[dri].databaseFilePaths.IsEmpty();
}

// ----------------------------------------------------------------------------

void GaiaProcess::EnsureDatabasesInitialized( int dr, int verbosity )
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
               console.WriteLn( "<end><cbr><br>Installing Gaia " + DRIndexToDRName( dri ) + " database files...\n" );
               Module->ProcessEvents();
            }

            for ( const String& filePath : data.databaseFilePaths )
            {
               GaiaDatabaseFile* file = new GaiaDatabaseFile( filePath );
               data.databases.Add( file );

               if ( DRIndexToDRName( dri ) == "DR3SP" )
               {
                  if ( file->DataRelease() != "DR3" || !file->HasMeanSpectrumData() )
                     throw Error( "Unexpected Gaia " + file->DataRelease() + " database file; "
                                 "expected a Gaia DR3 file with BP/RP sampled mean spectrum data: <raw>" + file->FilePath() + "</raw>" );
               }
               else
               {
                  if ( file->DataRelease() != DRIndexToDRName( dri ) )
                     throw Error( "Unexpected Gaia " + file->DataRelease() + " database file; "
                                 "expected a Gaia " + DRIndexToDRName( dri ) + " file: <raw>" + file->FilePath() + "</raw>" );
               }

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
               []( const GaiaDatabaseFile& d1, const GaiaDatabaseFile& d2 )
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

void GaiaProcess::CloseDatabases( int dr )
{
   int dri = DRParameterValueToDRIndex( dr );
   s_data[dri].databases.Destroy();
   s_data[dri].databasesInitialized = 0;
}

// ----------------------------------------------------------------------------

const ReferenceArray<GaiaDatabaseFile>& GaiaProcess::Databases( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].databases;
}

// ----------------------------------------------------------------------------

float GaiaProcess::MagnitudeLow( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].databases.IsEmpty() ? .0F : s_data[dri].databases.Begin()->MagnitudeLow();
}

// ----------------------------------------------------------------------------

float GaiaProcess::MagnitudeHigh( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].databases.IsEmpty() ? .0F : s_data[dri].databases.ReverseBegin()->MagnitudeHigh();
}

// ----------------------------------------------------------------------------

bool GaiaProcess::HasMeanSpectrumData( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].databases.IsEmpty() ? false : s_data[dri].databases.Begin()->HasMeanSpectrumData();
}

// ----------------------------------------------------------------------------

float GaiaProcess::SpectrumStart( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].databases.IsEmpty() ? .0F : s_data[dri].databases.Begin()->SpectrumStart();
}

// ----------------------------------------------------------------------------

float GaiaProcess::SpectrumStep( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].databases.IsEmpty() ? .0F : s_data[dri].databases.Begin()->SpectrumStep();
}

// ----------------------------------------------------------------------------

int GaiaProcess::SpectrumCount( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].databases.IsEmpty() ? 0 : s_data[dri].databases.Begin()->SpectrumCount();
}

// ----------------------------------------------------------------------------

int GaiaProcess::SpectrumBits( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].databases.IsEmpty() ? 0 : s_data[dri].databases.Begin()->SpectrumBits();
}

// ----------------------------------------------------------------------------

bool GaiaProcess::PreferencesLoaded( int dr ) const
{
   int dri = DRParameterValueToDRIndex( dr );
   return s_data[dri].preferencesLoaded;
}

// ----------------------------------------------------------------------------

void GaiaProcess::LoadPreferences( int dr )
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

void GaiaProcess::SavePreferences( int dr )
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
// EOF GaiaProcess.cpp - Released 2024-12-23T11:33:28Z
