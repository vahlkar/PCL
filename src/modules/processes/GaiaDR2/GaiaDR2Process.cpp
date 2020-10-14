//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.1
// ----------------------------------------------------------------------------
// Standard GaiaDR2 Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// GaiaDR2Process.cpp - Released 2020-10-12T19:25:16Z
// ----------------------------------------------------------------------------
// This file is part of the standard GaiaDR2 PixInsight module.
//
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "GaiaDR2Interface.h"
#include "GaiaDR2Parameters.h"
#include "GaiaDR2Process.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/MetaModule.h>
#include <pcl/Settings.h>

// Settings keys
#define KEY_DATABASE_FILE_PATH_PREFIX  "DatabaseFilePath"

namespace pcl
{

// ----------------------------------------------------------------------------

GaiaDR2Process* TheGaiaDR2Process = nullptr;

static StringList                          s_databaseFilePaths;
static ReferenceArray<GaiaDR2DatabaseFile> s_databases;
static AtomicInt                           s_databasesInitialized;
static Mutex                               s_mutex;
static bool                                s_preferencesLoaded = false;

// ----------------------------------------------------------------------------

GaiaDR2Process::GaiaDR2Process()
{
   TheGaiaDR2Process = this;

   new GDR2Command( this );
   new GDR2CenterRA( this );
   new GDR2CenterDec( this );
   new GDR2Radius( this );
   new GDR2MagnitudeLow( this );
   new GDR2MagnitudeHigh( this );
   new GDR2SourceLimit( this );
   new GDR2IncludeFlags( this );
   new GDR2ExcludeFlags( this );
   new GDR2Verbosity( this );
   new GDR2SortBy( this );
   new GDR2GenerateTextOutput( this );
   new GDR2TextFormat( this );
   new GDR2TextHeaders( this );
   new GDR2OutputFilePath( this );

   new GDR2Sources( this );
   new GDR2SourceRA( TheGDR2SourcesParameter );
   new GDR2SourceDec( TheGDR2SourcesParameter );
   new GDR2SourceParx( TheGDR2SourcesParameter );
   new GDR2SourcePMRA( TheGDR2SourcesParameter );
   new GDR2SourcePMDec( TheGDR2SourcesParameter );
   new GDR2SourceMagG( TheGDR2SourcesParameter );
   new GDR2SourceMagBP( TheGDR2SourcesParameter );
   new GDR2SourceMagRP( TheGDR2SourcesParameter );
   new GDR2SourceFlags( TheGDR2SourcesParameter );
   new GDR2ExcessCount( this );
   new GDR2RejectCount( this );
   new GDR2TimeTotal( this );
   new GDR2TimeIO( this );
   new GDR2CountIO( this );
   new GDR2TimeUncompress( this );
   new GDR2TimeDecode( this );
   new GDR2DatabaseFilePaths( this );
   new GDR2DatabaseFilePath( TheGDR2DatabaseFilePathsParameter );
   new GDR2DatabaseMagnitudeLow( this );
   new GDR2DatabaseMagnitudeHigh( this );
}

// ----------------------------------------------------------------------------

IsoString GaiaDR2Process::Id() const
{
   return "GaiaDR2";
}

// ----------------------------------------------------------------------------

IsoString GaiaDR2Process::Category() const
{
   return "StarCatalogs,Global";
}

// ----------------------------------------------------------------------------

uint32 GaiaDR2Process::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String GaiaDR2Process::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String GaiaDR2Process::IconImageSVGFile() const
{
   return "@module_icons_dir/GaiaDR2.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* GaiaDR2Process::DefaultInterface() const
{
   return TheGaiaDR2Interface;
}

// -------------------------------------------------------------------------

ProcessImplementation* GaiaDR2Process::Create() const
{
   return new GaiaDR2Instance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* GaiaDR2Process::Clone( const ProcessImplementation& p ) const
{
   const GaiaDR2Instance* instPtr = dynamic_cast<const GaiaDR2Instance*>( &p );
   return (instPtr != nullptr) ? new GaiaDR2Instance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool GaiaDR2Process::PrefersGlobalExecution() const
{
   return true; // this is a global process
}

// ----------------------------------------------------------------------------

bool GaiaDR2Process::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: GaiaDR2 [<arg_list>]"
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

int GaiaDR2Process::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::NoItems );

   GaiaDR2Instance instance( this );
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
// ----------------------------------------------------------------------------

void GaiaDR2Process::SetDatabaseFilePaths( const StringList& paths )
{
   CloseDatabases();
   s_databaseFilePaths = paths;
   SavePreferences();
}

// ----------------------------------------------------------------------------

const StringList& GaiaDR2Process::DatabaseFilePaths() const
{
   return s_databaseFilePaths;
}

// ----------------------------------------------------------------------------

void GaiaDR2Process::EnsureDatabasesInitialized( int verbosity )
{
   if ( !s_databasesInitialized )
   {
      volatile AutoLock lock( s_mutex );
      if ( s_databasesInitialized.Load() == 0 )
      {
         if ( s_databaseFilePaths.IsEmpty() )
            LoadPreferences();

         try
         {
            s_databases.Destroy();

            if ( s_databaseFilePaths.IsEmpty() )
               throw Error( "No database files have been selected." );

            Console console;
            if ( verbosity > 1 )
            {
               console.Show();
               console.WriteLn( "<end><cbr><br>Installing Gaia DR2 database files...\n" );
               Module->ProcessEvents();
            }

            for ( const String& filePath : s_databaseFilePaths )
            {
               GaiaDR2DatabaseFile* file = new GaiaDR2DatabaseFile( filePath );
               s_databases.Add( file );
               if ( verbosity > 1 )
               {
                  console.WriteLn( "<b><raw>" + file->FilePath() + "</raw></b>\n"
                     +  "Database version .... " + file->Metadata().databaseIdentifier + ' ' + file->Metadata().databaseVersion + '\n'
                     + String().Format(
                        "Magnitude range ..... %.2f -> %.2f\n"
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

            s_databases.Sort(
               []( const GaiaDR2DatabaseFile& d1, const GaiaDR2DatabaseFile& d2 )
               {
                  return d1.MagnitudeHigh() < d2.MagnitudeHigh();
               } );
         }
         catch ( ... )
         {
            s_databases.Destroy();
            if ( verbosity > 0 )
            {
               try
               {
                  throw;
               }
               ERROR_HANDLER
            }
         }

         s_databasesInitialized.Store( 1 );
      }
   }
}

// ----------------------------------------------------------------------------

void GaiaDR2Process::CloseDatabases()
{
   s_databases.Destroy();
   s_databasesInitialized = 0;
}

// ----------------------------------------------------------------------------

const ReferenceArray<GaiaDR2DatabaseFile>& GaiaDR2Process::Databases() const
{
   return s_databases;
}

// ----------------------------------------------------------------------------

float GaiaDR2Process::MagnitudeLow() const
{
   return s_databases.IsEmpty() ? .0F : s_databases.Begin()->MagnitudeLow();
}

// ----------------------------------------------------------------------------

float GaiaDR2Process::MagnitudeHigh() const
{
   return s_databases.IsEmpty() ? .0F : s_databases.ReverseBegin()->MagnitudeHigh();
}

// ----------------------------------------------------------------------------

bool GaiaDR2Process::PreferencesLoaded() const
{
   return s_preferencesLoaded;
}

// ----------------------------------------------------------------------------

void GaiaDR2Process::LoadPreferences()
{
   s_databaseFilePaths.Clear();
   for ( int i = 0;; ++i )
   {
      String filePath;
      if ( !Settings::Read( IsoString().Format( KEY_DATABASE_FILE_PATH_PREFIX "%03d", i ), filePath ) )
         break;
      s_databaseFilePaths << filePath;
   }
   s_preferencesLoaded = true;
}

// ----------------------------------------------------------------------------

void GaiaDR2Process::SavePreferences()
{
   for ( int i = 0;; ++i )
   {
      IsoString key = IsoString().Format( KEY_DATABASE_FILE_PATH_PREFIX "%03d", i );
      String filePath;
      if ( !Settings::Read( key, filePath ) )
         break;
      Settings::Remove( key );
   }

   int i = 0;
   for ( const String& filePath : s_databaseFilePaths )
      Settings::Write( IsoString().Format( KEY_DATABASE_FILE_PATH_PREFIX "%03d", i++ ), filePath );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GaiaDR2Process.cpp - Released 2020-10-12T19:25:16Z
