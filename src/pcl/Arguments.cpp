//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Arguments.cpp - Released 2024-12-28T16:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Arguments.h>
#include <pcl/Exception.h>
#include <pcl/File.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>

#include <stdlib.h> // getenv()

namespace pcl
{

// ----------------------------------------------------------------------------

void Argument::Parse( const char16_type* argv )
{
   Initialize();

   token = String( argv ).Trimmed();
   if ( token.IsEmpty() )
      throw Error( "No argument identifier." );

   size_type nsep = token.Find( '=' );

   if ( nsep != String::notFound )
   {
      /*
       * Arguments with value:
       *
       * - Numeric arguments, when numeric conversion of value succeeds.
       * - String arguments, when numeric conversion of value fails.
       */
      String argValue = token.Substring( nsep+1 ).Trimmed();
      if ( argValue.IsEmpty() )
         throw ParseError( "Expected a numeric or string argument value", token, int( nsep+1 ) );

      if ( argValue.TryToDouble( asNumeric ) )
         type = numeric_arg;
      else
      {
         type = string_arg;
         asString = argValue.Unquoted();
         if ( asString.IsEmpty() )
            throw ParseError( "Expected a nonempty string argument value", token, int( nsep+1 ) );
      }

      id = token.Left( nsep );
   }
   else
   {
      /*
       * Arguments without value:
       *
       * - Switch arguments, when the token ends with either '+' or '-'.
       * - Literal arguments otherwise.
       */
      if ( token.EndsWith( '+' ) )
      {
         type = switch_arg;
         asSwitch = true;
         id = token.Left( token.UpperBound() );
      }
      else if ( token.EndsWith( '-' ) )
      {
         type = switch_arg;
         asSwitch = false;
         id = token.Left( token.UpperBound() );
      }
      else
      {
         type = literal_arg;
         id = token;
      }
   }
}

// ----------------------------------------------------------------------------

static void SearchDirectory_Recursive( StringList& fileNames, const String& fileName, bool recursive )
{
   String fileDir = File::ExtractDrive( fileName ) + File::ExtractDirectory( fileName );
   String wildSpec = File::ExtractName( fileName ) + File::ExtractExtension( fileName );

   FindFileInfo info;

   for ( File::Find f( fileName ); f.NextItem( info ); )
      if ( !info.IsDirectory() )
         if ( info.name.WildMatch( wildSpec ) )
         {
            String path = fileDir + '/' + info.name;
            if ( !fileNames.Contains( path ) )
               fileNames.Add( path );
         }

   if ( recursive )
   {
      StringList directories;

      for ( File::Find f( fileDir + "/*" ); f.NextItem( info ); )
         if ( info.IsDirectory() && info.name != "." && info.name != ".." )
            directories.Add( info.name );

      for ( const String& dir : directories )
         SearchDirectory_Recursive( fileNames, fileDir + '/' + dir + '/' + wildSpec, true );
   }
}

StringList SearchDirectory( const String& fileName, bool recursive )
{
   StringList fileNames;
   SearchDirectory_Recursive( fileNames, fileName, recursive );
   return fileNames;
}

// ----------------------------------------------------------------------------

static String s_recursiveSearchArg = "-r";

String RecursiveDirSearchArgument()
{
   return s_recursiveSearchArg;
}

void SetRecursiveDirSearchArgument( const String& argId )
{
   s_recursiveSearchArg = argId.IsEmpty() ? String( "-r" ) : argId;
}

// ----------------------------------------------------------------------------

static void AddView( StringList& items, const View& v )
{
   String id( v.FullId() );
   if ( !items.Contains( id ) )
      items.Add( id );
}

static void FindPreviews( StringList& items, const ImageWindow& w, const String& previewId )
{
   if ( previewId.HasWildcards() )
   {
      Array<View> P = w.Previews();
      for ( size_type i = 0; i < P.Length(); ++i )
         if ( String( P[i].Id() ).WildMatch( previewId ) )
            AddView( items, P[i] );
   }
   else
   {
      View p = w.PreviewById( IsoString( previewId ) );
      if ( p.IsNull() )
         throw ParseError( "Preview not found", previewId );
      AddView( items, p );
   }
}

static void FindPreviews( StringList& items, const String& imageId, const String& previewId )
{
   if ( imageId.HasWildcards() )
   {
      Array<ImageWindow> W = ImageWindow::AllWindows();
      for ( size_type i = 0; i < W.Length(); ++i )
         if ( String( W[i].MainView().Id() ).WildMatch( imageId ) )
            FindPreviews( items, W[i], previewId );
   }
   else
   {
      ImageWindow w = ImageWindow::WindowById( IsoString( imageId ) );
      if ( w.IsNull() )
         throw ParseError( "Image not found", imageId );
      FindPreviews( items, w, previewId );
   }
}

ArgumentList ExtractArguments( const StringList& argv, argument_item_mode mode, ArgumentOptions options )
{
   bool noItems             = mode == ArgumentItemMode::NoItems;
   bool itemsAsFiles        = mode == ArgumentItemMode::AsFiles;
   bool itemsAsViews        = mode == ArgumentItemMode::AsViews;
   bool allowWildcards      = !noItems && options.IsFlagSet( ArgumentOption::AllowWildcards );
   bool noPreviews          = itemsAsViews && options.IsFlagSet( ArgumentOption::NoPreviews );
   bool recursiveDirSearch  = itemsAsFiles && allowWildcards && options.IsFlagSet( ArgumentOption::RecursiveDirSearch );
   bool recursiveSearchArgs = recursiveDirSearch && options.IsFlagSet( ArgumentOption::RecursiveSearchArgs );

   // This is the recursive search mode flag, controlled by --r[+|-]
   bool recursiveSearch = false;

   // The list of existing view identifiers, in case itemsAsViews = true.
   SortedStringList imageIds;

   // The list of extracted arguments
   ArgumentList arguments;

   for ( const String& token : argv )
   {
      if ( token.StartsWith( '-' ) )
      {
         Argument arg( token.At( 1 ) );

         if ( recursiveSearchArgs && arg.Id() == s_recursiveSearchArg )
         {
            if ( arg.IsSwitch() )
               recursiveSearch = arg.SwitchState();
            else if ( arg.IsLiteral() )
               recursiveSearch = true;
            else
               arguments << arg;
         }
         else
            arguments << arg;
      }
      else
      {
         if ( noItems )
            throw ParseError( "Non-parametric arguments are not allowed", token );

         StringList items;

         if ( itemsAsFiles )
         {
            String fileName = token.Unquoted().Trimmed();
            if ( fileName.IsEmpty() )
               throw ParseError( "Empty file path specification", token );

            fileName = File::FullPath( fileName );

            if ( fileName.HasWildcards() )
            {
               if ( !allowWildcards )
                  throw ParseError( "Wildcards not allowed", fileName );
               items = SearchDirectory( fileName, recursiveSearch );
            }
            else
               items << fileName;
         }
         else if ( itemsAsViews )
         {
            String viewId = token;

            if ( !allowWildcards )
               if ( viewId.HasWildcards() )
                  throw ParseError( "Wildcards not allowed", viewId );

            size_type p = viewId.Find( "->" );

            if ( p != String::notFound )
            {
               if ( noPreviews )
                  throw ParseError( "Preview identifiers not allowed", viewId );

               String imageId = viewId.Left( p );
               if ( imageId.IsEmpty() )
                  throw ParseError( "Missing image identifier", viewId );

               String previewId = viewId.Substring( p+2 );
               if ( previewId.IsEmpty() )
                  throw ParseError( "Missing preview identifier", viewId );

               FindPreviews( items, imageId, previewId );
            }
            else
            {
               if ( viewId.HasWildcards() )
               {
                  Array<ImageWindow> W = ImageWindow::AllWindows();
                  for ( size_type i = 0; i < W.Length(); ++i )
                  {
                     View v = W[i].MainView();
                     if ( String( v.Id() ).WildMatch( viewId ) )
                        AddView( items, v );
                  }
               }
               else
               {
                  ImageWindow w = ImageWindow::WindowById( IsoString( viewId ) );
                  if ( w.IsNull() )
                     throw ParseError( "Image not found", viewId );
                  AddView( items, w.MainView() );
               }
            }
         }
         else
            items << token;

         arguments << Argument( token, items );
      }
   }

   return arguments;
}

// ----------------------------------------------------------------------------

String ReplaceEnvironmentVariables( const String& s0 )
{
   String s( s0 );
   size_type p = 0;

   for ( ;; )
   {
      for ( ;; )
      {
         p = s.Find( '$', p );
         if ( p == String::notFound )
            return s;
         if ( p == 0 || s[p-1] != '\\' )
            break;
         s.Delete( p-1 );
      }

      size_type p1 = p;
      size_type n = s.Length();
      while ( ++p1 < n )
      {
         int c = int( s[p1] );
         if ( (c < 'A' || c > 'Z') && (c < 'a' || c > 'z') && (c < '0' || c > '9') && c != '_' )
            break;
      }

      String var( s.Substring( p+1, p1-p-1 ) );
      if ( !var.IsEmpty() )
      {
         IsoString v8( var );
         var = ReplaceEnvironmentVariables( String( ::getenv( v8.c_str() ) ) );
      }

      s.Replace( p, p1-p, var );

      p += var.Length();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Arguments.cpp - Released 2024-12-28T16:53:56Z
