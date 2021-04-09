//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Gaia Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// GaiaProcess.cpp - Released 2021-04-09T19:41:48Z
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

static DatabaseData s_data[ 2 ]; // DR2, EDR3

static int DRParameterValueToDRIndex( pcl_enum p )
{
   switch( p )
   {
   case GDataRelease::DR2:  return 0;
   case GDataRelease::EDR3: return 1;
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
   new GVerbosity( this );
   new GDataRelease( this );
   new GSortBy( this );
   new GGenerateTextOutput( this );
   new GTextFormat( this );
   new GTextHeaders( this );
   new GOutputFilePath( this );

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
   new GExcessCount( this );
   new GRejectCount( this );
   new GTimeTotal( this );
   new GTimeIO( this );
   new GCountIO( this );
   new GTimeUncompress( this );
   new GTimeDecode( this );
   new GIsValid( this );
   new GOutputDataRelease( this );
   new GDatabaseFilePaths( this );
   new GDatabaseFilePath( TheGDatabaseFilePathsParameter );
   new GDatabaseMagnitudeLow( this );
   new GDatabaseMagnitudeHigh( this );
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
   return 0x100;
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
// ----------------------------------------------------------------------------

void GaiaProcess::SetDatabaseFilePaths( int dr, const StringList& paths )
{
   CloseDatabases( dr );
   dr = DRParameterValueToDRIndex( dr );
   s_data[dr].databaseFilePaths = paths;
   SavePreferences( dr );
}

// ----------------------------------------------------------------------------

const StringList& GaiaProcess::DatabaseFilePaths( int dr ) const
{
   dr = DRParameterValueToDRIndex( dr );
   if ( s_data[dr].databaseFilePaths.IsEmpty() )
      const_cast<GaiaProcess*>( this )->LoadPreferences( dr );
   return s_data[dr].databaseFilePaths;
}

// ----------------------------------------------------------------------------

bool GaiaProcess::IsValid( int dr ) const
{
   return !Databases( dr ).IsEmpty();
}

// ----------------------------------------------------------------------------

bool GaiaProcess::HasDatabaseFiles( int dr ) const
{
   dr = DRParameterValueToDRIndex( dr );
   if ( s_data[dr].databaseFilePaths.IsEmpty() )
      const_cast<GaiaProcess*>( this )->LoadPreferences( dr );
   return !s_data[dr].databaseFilePaths.IsEmpty();
}

// ----------------------------------------------------------------------------

void GaiaProcess::EnsureDatabasesInitialized( int dr, int verbosity )
{
   dr = DRParameterValueToDRIndex( dr );
   DatabaseData& data = s_data[dr];
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
               console.WriteLn( "<end><cbr><br>Installing Gaia " + DRIndexToDRName( dr ) + " database files...\n" );
               Module->ProcessEvents();
            }

            for ( const String& filePath : data.databaseFilePaths )
            {
               GaiaDatabaseFile* file = new GaiaDatabaseFile( filePath );
               data.databases.Add( file );
               if ( file->DataRelease() != DRIndexToDRName( dr ) )
                  throw Error( "Unexpected Gaia " + file->DataRelease() + " database file; "
                               "expected a Gaia " + DRIndexToDRName( dr ) + " file: <raw>" + file->FilePath() + "</raw>" );
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
   dr = DRParameterValueToDRIndex( dr );
   s_data[dr].databases.Destroy();
   s_data[dr].databasesInitialized = 0;
}

// ----------------------------------------------------------------------------

const ReferenceArray<GaiaDatabaseFile>& GaiaProcess::Databases( int dr ) const
{
   dr = DRParameterValueToDRIndex( dr );
   return s_data[dr].databases;
}

// ----------------------------------------------------------------------------

float GaiaProcess::MagnitudeLow( int dr ) const
{
   dr = DRParameterValueToDRIndex( dr );
   return s_data[dr].databases.IsEmpty() ? .0F : s_data[dr].databases.Begin()->MagnitudeLow();
}

// ----------------------------------------------------------------------------

float GaiaProcess::MagnitudeHigh( int dr ) const
{
   dr = DRParameterValueToDRIndex( dr );
   return s_data[dr].databases.IsEmpty() ? .0F : s_data[dr].databases.ReverseBegin()->MagnitudeHigh();
}

// ----------------------------------------------------------------------------

bool GaiaProcess::PreferencesLoaded( int dr ) const
{
   dr = DRParameterValueToDRIndex( dr );
   return s_data[dr].preferencesLoaded;
}

// ----------------------------------------------------------------------------

void GaiaProcess::LoadPreferences( int dr )
{
   dr = DRParameterValueToDRIndex( dr );
   s_data[dr].databaseFilePaths.Clear();
   for ( int i = 0;; ++i )
   {
      String filePath;
      if ( !Settings::Read( PreferencesKeyForDRIndex( dr, i ), filePath ) )
         break;
      s_data[dr].databaseFilePaths << filePath;
   }
   s_data[dr].preferencesLoaded = true;
}

// ----------------------------------------------------------------------------

void GaiaProcess::SavePreferences( int dr )
{
   dr = DRParameterValueToDRIndex( dr );
   for ( int i = 0;; ++i )
   {
      IsoString key = PreferencesKeyForDRIndex( dr, i );
      String filePath;
      if ( !Settings::Read( key, filePath ) )
         break;
      Settings::Remove( key );
   }

   int i = 0;
   for ( const String& filePath : s_data[dr].databaseFilePaths )
      Settings::Write( PreferencesKeyForDRIndex( dr, i++ ), filePath );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GaiaProcess.cpp - Released 2021-04-09T19:41:48Z
