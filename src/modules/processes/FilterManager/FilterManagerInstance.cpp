//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.9
// ----------------------------------------------------------------------------
// Standard FilterManager Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// FilterManagerInstance.cpp - Released 2024-03-20T10:42:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard FilterManager PixInsight module.
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

#include "FilterManagerInstance.h"
#include "FilterManagerParameters.h"

#include <pcl/Console.h>
#include <pcl/FilterManager.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FilterManagerInstance::FilterManagerInstance( const MetaProcess* p )
   : ProcessImplementation( p )
   , p_task( FMTask::Default )
   , p_noGUIMessages( TheFMNoGUIMessagesParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

FilterManagerInstance::FilterManagerInstance( const FilterManagerInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void FilterManagerInstance::Assign( const ProcessImplementation& p )
{
   const FilterManagerInstance* x = dynamic_cast<const FilterManagerInstance*>( &p );
   if ( x != nullptr )
   {
      p_task = x->p_task;
      p_path = x->p_path;
      p_noGUIMessages = x->p_noGUIMessages;
   }
}

// ----------------------------------------------------------------------------

bool FilterManagerInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "FilterManager can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool FilterManagerInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   if ( p_task != FMTask::NoAction && p_task != FMTask::MergeDefault && p_task != FMTask::SetDefault )
      if ( p_path.IsEmpty() )
      {
         whyNot = "No file or directory has been specified, which is required for the specified FilterManager task.";
         return false;
      }

   return true;
}

// ----------------------------------------------------------------------------

static void SanitizeFileName( String& fileName )
{
   static const char* badChars = " #%&{}\\<>*?/$!'\":@+`|=";
   for ( String::char_type& c : fileName )
      if ( c < 255 )
         if ( c == ' ' )
            c = '-';
         else if ( strchr( badChars, char( c ) ) )
            c = '_';
}

bool FilterManagerInstance::ExecuteGlobal()
{
   Console console;

   switch ( p_task )
   {
   case FMTask::NoAction:
      console.NoteLn( "<end><cbr><br>* No action performed." );
      break;

   case FMTask::ImportXML:
   case FMTask::MergeXML:
      {
         String path = File::FullPath( p_path.Trimmed() );
         console.NoteLn( "<end><cbr><br>* Importing filter definitions (XSPD format):" );
         console.WriteLn( "<raw>" + path + "</raw>" );
         console.WriteLn();
         if ( !File::Exists( path ) )
            throw Error( "No such file: " + path );

         SampledSpectrumDataList filters = FilterManager::LoadFilters( path );
         if ( filters.IsEmpty() )
            throw Error( "No filter definitions available in database file: " + path );

         FilterManager manager;
         if ( p_task == FMTask::MergeXML )
            manager.Initialize();
         manager.ImportFilters( filters, p_task == FMTask::MergeXML );

         for ( const SampledSpectrumData& filter : filters )
            console.WriteLn( filter.name );
         console.WriteLn( String().Format( "<end><cbr><br>%u filter(s) imported, %u filters available.",
                                           filters.Length(), manager.Filters().Length() ) );
         manager.SaveToConfigurationFiltersDatabaseFile();
      }
      break;

   case FMTask::ImportCSV:
   case FMTask::MergeCSV:
      {
         String path = File::FullPath( p_path.Trimmed() );
         console.NoteLn( "<end><cbr><br>* Importing filter definitions (CSV format):" );
         console.WriteLn( "<raw>" + path + "</raw>" );
         console.WriteLn();
         if ( !File::DirectoryExists( path ) )
            throw Error( "No such directory: " + path );

         StringList fileNames;
         String dirPath = path;
         if ( !dirPath.EndsWith( '/' ) )
            dirPath << '/';
         FindFileInfo info;
         for ( File::Find f( dirPath + "*.csv" ); f.NextItem( info ); )
            if ( !info.IsDirectory() )
               fileNames << info.name;
         if ( fileNames.IsEmpty() )
            throw Error( "No .csv files found in directory: " + path );

         SampledSpectrumDataList filters;
         for ( const String& fileName : fileNames )
         {
            try
            {
               SampledSpectrumData filter = FilterManager::DecodeFilterCSV(
                     File::ReadLines( dirPath + fileName, ReadTextOption::RemoveEmptyLines|ReadTextOption::TrimSpaces ) );
               filters << filter;
               console.WriteLn( dirPath + fileName + ": " + filter.name );
            }
            catch ( const Exception& x )
            {
               throw Error( x.Message() + ": " + dirPath + fileName );
            }
         }

         FilterManager manager;
         if ( p_task == FMTask::MergeCSV )
            manager.Initialize();
         manager.ImportFilters( filters, p_task == FMTask::MergeCSV );

         console.WriteLn( String().Format( "<end><cbr><br>%u filter(s) imported, %u filters available.",
                                           filters.Length(), manager.Filters().Length() ) );
         manager.SaveToConfigurationFiltersDatabaseFile();
      }
      break;

   case FMTask::ExportXML:
      {
         String path = File::FullPath( p_path.Trimmed() );
         console.NoteLn( "<end><cbr><br>* Exporting filter definitions (XSPD format):" );
         console.WriteLn( "<raw>" + path + "</raw>" );
         console.WriteLn();
         if ( File::Exists( path ) )
            console.WarningLn( "** Warning: Overwriting existing file." );

         FilterManager manager;
         manager.Initialize();
         manager.ExportFiltersDatabase( path, manager.Filters() );

         for ( const SampledSpectrumData& filter : manager.Filters() )
            console.WriteLn( filter.name );
         console.WriteLn( String().Format( "<end><cbr><br>%u filter(s) exported.", manager.Filters().Length() ) );
      }
      break;

   case FMTask::ExportCSV:
      {
         String path = File::FullPath( p_path.Trimmed() );
         console.NoteLn( "<end><cbr><br>* Exporting filter definitions (CSV format):" );
         console.WriteLn( "<raw>" + path + "</raw>" );
         console.WriteLn();
         if ( !File::DirectoryExists( path ) )
            throw Error( "No such directory: " + path );

         FilterManager manager;
         manager.Initialize();

         for ( const SampledSpectrumData& filter : manager.Filters() )
         {
            String filePath = path;
            if ( !filePath.EndsWith( '/' ) )
               filePath << '/';
            String fileName = filter.name.Trimmed();
            SanitizeFileName( fileName );
            filePath << fileName << ".csv";
            if ( File::Exists( filePath ) )
               console.WarningLn( "** Warning: Overwriting existing file." );
            File::WriteTextFile( filePath, IsoString().ToNewLineSeparated( manager.EncodeFilterCSV( filter ) ) );
            console.WriteLn( filePath + ": " + filter.name );
         }

         console.WriteLn( String().Format( "<end><cbr><br>%u filter(s) exported.", manager.Filters().Length() ) );
      }
      break;

   case FMTask::MergeDefault:
      {
         if ( !p_noGUIMessages )
            if ( MessageBox( "<p>About to merge the current filters with the default set.</p>"
                              "<p><b>This cannot be undone. Proceed?</b></p>",
                              "FilterManager",
                              StdIcon::Warning,
                              StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
               break;

         console.NoteLn( "<end><cbr><br>* Merging default filters database:" );
         console.WriteLn( "<raw>" + FilterManager::DefaultFiltersDatabaseFilePath() + "</raw>" );
         FilterManager manager;
         manager.Initialize();
         SampledSpectrumDataList filters = manager.Filters();
         manager.Initialize( FilterManager::DefaultFiltersDatabaseFilePath(), FilterManager::DefaultWhiteReferencesDatabaseFilePath() );
         manager.ImportFilters( filters, true/*merge*/ );
         console.NoteLn( String().Format( "<end><cbr>* Default filters merged, %u filters available.", manager.Filters().Length() ) );
         manager.SaveToConfigurationFiltersDatabaseFile();
      }
      break;

   case FMTask::SetDefault:
      {
         if ( !p_noGUIMessages )
            if ( MessageBox( "<p>About to reset filters to the default set.</p>"
                              "<p><b>This cannot be undone. Proceed?</b></p>",
                              "FilterManager",
                              StdIcon::Warning,
                              StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
               break;

         console.NoteLn( "<end><cbr><br>* Loading default filters database:" );
         console.WriteLn( "<raw>" + FilterManager::DefaultFiltersDatabaseFilePath() + "</raw>" );
         FilterManager manager;
         manager.Initialize( FilterManager::DefaultFiltersDatabaseFilePath(), FilterManager::DefaultWhiteReferencesDatabaseFilePath() );
         console.NoteLn( String().Format( "<end><cbr>* Default filters loaded, %u filters available.", manager.Filters().Length() ) );
         manager.SaveToConfigurationFiltersDatabaseFile();
      }
      break;
   }

   return true;
}

// ----------------------------------------------------------------------------

void* FilterManagerInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheFMTaskParameter )
      return &p_task;
   if ( p == TheFMPathParameter )
      return p_path.Begin();
   if ( p == TheFMNoGUIMessagesParameter )
      return &p_noGUIMessages;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool FilterManagerInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheFMPathParameter )
   {
      p_path.Clear();
      if ( sizeOrLength != 0 )
         p_path.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type FilterManagerInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheFMPathParameter )
      return p_path.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FilterManagerInstance.cpp - Released 2024-03-20T10:42:12Z
