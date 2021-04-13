//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// TextDatabase.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
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

#include "Elements.h"
#include "TextDatabase.h"

#include <pcl/Console.h>
#include <pcl/ElapsedTime.h>
#include <pcl/EphemerisFile.h>
#include <pcl/ErrorHandler.h>
#include <pcl/File.h>
#include <pcl/GlobalSettings.h>
#include <pcl/MetaModule.h>
#include <pcl/StandardStatus.h>
#include <pcl/XML.h>

/*
 * ### N.B. Beware of std::regex - it is unstable and poorly implemented, at
 * least as of g++ 9.3 (2021).
 *
 * Here we use Boost.Xpressive, which is fully stable and comprehensive:
 * https://www.boost.org/doc/libs/1_75_0/doc/html/xpressive.html
 */
#include <boost/xpressive/xpressive.hpp>

using namespace boost::xpressive;

namespace pcl
{

// ----------------------------------------------------------------------------

// Core asteroid and KBO identifiers for optional exclusion.
static SortedArray<IsoString> s_excluded;

// ----------------------------------------------------------------------------

class SearchThread : public Thread
{
public:

   Array<TextDatabase::ObjectData> objects;

   SearchThread( const AbstractImage::ThreadData& data,
                 const IsoStringList& lines,
                 const TextDatabase::FormatDescription& format,
                 const Array<cregex>& regs,
                 const IsoStringList& tokens,
                 const Array<Point>& ranges,
                 bool exclude,
                 size_type maxCount,
                 size_type start, size_type end )
      : m_data( data )
      , m_lines( lines )
      , m_format( format )
      , m_regs( regs )
      , m_tokens( tokens )
      , m_ranges( ranges )
      , m_exclude( exclude )
      , m_maxCount( maxCount )
      , m_start( start )
      , m_end( end )
   {
   }

   void Run() override
   {
      INIT_THREAD_MONITOR()

      try
      {
         objects.Clear();

         m_success = false;

         size_type mustHaveTokens = 6/*state*/ + 1/*epoch*/;
         if ( m_format.fld_id )
            ++mustHaveTokens;
         if ( m_format.fld_name )
            ++mustHaveTokens;
         if ( m_format.fld_H )
            ++mustHaveTokens;
         if ( m_format.fld_G )
            ++mustHaveTokens;
         if ( m_format.fld_B_V )
            ++mustHaveTokens;
         if ( m_format.fld_D )
            ++mustHaveTokens;

         size_type count = 0;

         for ( size_type l = m_start; l < m_end; ++l )
         {
            Module->ProcessEvents();

            const IsoString& line = m_lines[l];

            // Ignore empty and blank lines
            bool empty = true;
            for ( auto ch : line )
               if ( !IsoCharTraits::IsTrimable( ch ) )
               {
                  empty = false;
                  break;
               }
            if ( empty )
               continue;

            IsoStringList tokens;
            switch ( m_format.fieldDisposition )
            {
            default: // ?!
            case TextDatabase::FieldDisposition_CommaSeparated:

               line.Break( tokens, ',' );
               break;

            case TextDatabase::FieldDisposition_FixedWidth:

               if ( m_format.fld_id )
                  tokens << ExtractToken( m_format.fld_id, line );
               if ( m_format.fld_name )
                  tokens << ExtractToken( m_format.fld_name, line );

               tokens << ExtractToken( m_format.fld_epoch, line );

               if ( m_format.contents == TextDatabase::Contents_OrbitalElements )
               {
                  tokens << ExtractToken( m_format.fld_a ? m_format.fld_a : m_format.fld_q, line )
                         << ExtractToken( m_format.fld_e, line )
                         << ExtractToken( m_format.fld_M ? m_format.fld_M : m_format.fld_T, line )
                         << ExtractToken( m_format.fld_i, line )
                         << ExtractToken( m_format.fld_O, line )
                         << ExtractToken( m_format.fld_w, line );
               }
               else // TextDatabase::Contents_StateVectors
               {
                  tokens << ExtractToken( m_format.fld_x, line )
                         << ExtractToken( m_format.fld_y, line )
                         << ExtractToken( m_format.fld_z, line )
                         << ExtractToken( m_format.fld_vx, line )
                         << ExtractToken( m_format.fld_vy, line )
                         << ExtractToken( m_format.fld_vz, line );
               }

               if ( m_format.fld_H )
                  tokens << ExtractToken( m_format.fld_H, line );
               if ( m_format.fld_G )
                  tokens << ExtractToken( m_format.fld_G, line );
               if ( m_format.fld_B_V )
                  tokens << ExtractToken( m_format.fld_B_V, line );
               if ( m_format.fld_D )
                  tokens << ExtractToken( m_format.fld_D, line );
               break;
            }

            if ( tokens.Length() < mustHaveTokens )
               throw Error( "Missing required tokens in CSV database format: got "
                           + String( tokens.Length() ) + ", " + String( mustHaveTokens ) + " required." );

            int i = 0;
            IsoString objectId = m_format.fld_id ? tokens[i++].Trimmed() : IsoString();
            IsoString objectName = m_format.fld_name ? tokens[i++].Trimmed() : IsoString();

            // In MPC asteroid databases, asteroid numbers are between
            // parentheses.
            if ( m_format.idFormat == TextDatabase::IdFormat_Parenthesized )
               if ( objectId.StartsWith( '(' ) )
                  if ( objectId.EndsWith( ')' ) )
                     objectId = objectId.Substring( 1, objectId.Length()-2 );

            UPDATE_THREAD_MONITOR( 16 )

            if ( !m_regs.IsEmpty() )
            {
               try
               {
                  if ( !objectId.IsEmpty() )
                     for ( const cregex& e : m_regs )
                        if ( boost::xpressive::regex_match( objectId.c_str(), e ) )
                           goto __matched;
                  if ( !objectName.IsEmpty() )
                     for ( const cregex& e : m_regs )
                        if ( boost::xpressive::regex_match( objectName.c_str(), e ) )
                           goto __matched;
               }
               catch ( ... )
               {
                  throw Error( "Internal error: regular expression evaluation failure" );
               }

               if ( m_tokens.IsEmpty() )
                  if ( m_ranges.IsEmpty() )
                     continue;
            }

            if ( !m_tokens.IsEmpty() )
            {
               if ( !objectId.IsEmpty() )
                  for ( const IsoString& t : m_tokens )
                     if ( objectId.ContainsIC( t ) )
                        goto __matched;
               if ( !objectName.IsEmpty() )
                  for ( const IsoString& t : m_tokens )
                     if ( objectName.ContainsIC( t ) )
                        goto __matched;

               if ( m_ranges.IsEmpty() )
                  continue;
            }

            if ( !m_ranges.IsEmpty() )
            {
               if ( !objectId.IsEmpty() )
               {
                  int id;
                  if ( objectId.TryToInt( id, 10 ) )
                     for ( const Point& p : m_ranges )
                        if ( id >= p.x )
                           if ( id <= p.y )
                              goto __matched;
               }

               continue;
            }

__matched:
            if ( m_exclude )
               if ( !objectId.IsEmpty() )
                  if ( s_excluded.Contains( objectId ) )
                     continue;

            TextDatabase::ObjectData object;

            object.id = objectId;
            object.name = objectName;

            if ( !ParseDateToken( object.epochJD, tokens[i++], m_format.epochFormat ) )
                  throw Error( "Missing 'epoch' field value" );

            if ( m_format.contents == TextDatabase::Contents_OrbitalElements )
            {
               if ( m_format.fld_a )
               {
                  if ( !ParseRealToken( object.state[a_idx], tokens[i++], -1.0e6, +1.0e6 ) )
                     throw Error( "Missing 'a' orbital element value" );
               }
               else // m_format.fld_q
                  if ( !ParseRealToken( object.state[q_idx], tokens[i++], 0.0, 10000.0 ) )
                     throw Error( "Missing 'q' orbital element value" );

               if ( !ParseRealToken( object.state[e_idx], tokens[i++], 0.0, 100.0 ) )
                  throw Error( "Missing 'e' orbital element value" );

               if ( m_format.fld_M )
               {
                  if ( !ParseRealToken( object.state[M_idx], tokens[i++], -360.0, +360.0 ) )
                     throw Error( "Missing 'M' orbital element value" );
               }
               else // m_format.fld_T
                  if ( !ParseDateToken( object.state[T_idx], tokens[i++], m_format.timePPFormat ) )
                     throw Error( "Missing 'T' orbital element value" );

               if ( !ParseRealToken( object.state[i_idx], tokens[i++], 0.0, 180.0 ) )
                  throw Error( "Missing 'i' orbital element value" );
               if ( !ParseRealToken( object.state[O_idx], tokens[i++], 0.0, 360.0 ) )
                  throw Error( "Missing 'O' orbital element value" );
               if ( !ParseRealToken( object.state[w_idx], tokens[i++], 0.0, 360.0 ) )
                  throw Error( "Missing 'w' orbital element value" );
            }
            else // TextDatabase::Contents_StateVectors
            {
               if ( !ParseRealToken( object.state[0], tokens[i++], -1000.0, +1000.0 ) )
                  throw Error( "Missing 'x' state vector value" );
               if ( !ParseRealToken( object.state[1], tokens[i++], -1000.0, +1000.0 ) )
                  throw Error( "Missing 'y' state vector value" );
               if ( !ParseRealToken( object.state[2], tokens[i++], -1000.0, +1000.0 ) )
                  throw Error( "Missing 'z' state vector value" );
               if ( !ParseRealToken( object.state[3], tokens[i++], -1.0, +1.0 ) )
                  throw Error( "Missing 'vx' state vector value" );
               if ( !ParseRealToken( object.state[4], tokens[i++], -1.0, +1.0 ) )
                  throw Error( "Missing 'vy' state vector value" );
               if ( !ParseRealToken( object.state[5], tokens[i++], -1.0, +1.0 ) )
                  throw Error( "Missing 'vz' state vector value" );
            }

            if ( m_format.fld_a )
               object.state[q_idx] = PerihelionDistanceFromSemimajorAxis( object.state );
            else
               object.state[a_idx] = SemimajorAxisFromPerihelionDistance( object.state );

            if ( m_format.fld_M )
               object.state[T_idx] = TimeOfPerihelionPassageFromMeanAnomaly( object.state, object.epochJD );
            else
               object.state[M_idx] = MeanAnomalyFromTimeOfPerihelionPassage( object.state, object.epochJD );

            if ( m_format.fld_H )
            {
               float H;
               if ( ParseRealToken( H, tokens[i++], -3.0, 100.0 ) )
                  if ( H < 99 ) // astorb.dat uses 99.99 to signal 'H not available'
                  object.H = H;
            }

            if ( m_format.fld_G )
            {
               float G;
               if ( ParseRealToken( G, tokens[i++], -1.0, 100.0 ) )
                  object.G = G;
            }

            if ( m_format.fld_B_V )
            {
               float B_V;
               if ( ParseRealToken( B_V, tokens[i++], 0.0, 2.0 ) )
                  object.B_V = B_V;
            }

            if ( m_format.fld_D )
            {
               float D;
               if ( ParseRealToken( D, tokens[i++], 0.0, 4000.0 ) )
                  object.D = D;
            }

            objects << object;

            if ( m_maxCount > 0 )
               if ( ++count == m_maxCount )
                  break;
         } // for

         objects.Sort();

         m_success = true;
         return;
      }
      catch ( ... )
      {
         objects.Clear();

         if ( IsRootThread() )
            throw;

         String text = ConsoleOutputText();
         ClearConsoleOutputText();
         try
         {
            throw;
         }
         ERROR_HANDLER
         m_errorInfo = ConsoleOutputText();
         ClearConsoleOutputText();
         Console().Write( text );
      }
   }

   bool Succeeded() const
   {
      return m_success;
   }

   String ErrorInfo() const
   {
      return m_errorInfo;
   }

private:

   const AbstractImage::ThreadData&       m_data;
   const IsoStringList&                   m_lines;
   const TextDatabase::FormatDescription& m_format;
   const Array<cregex>&                   m_regs;
   const IsoStringList&                   m_tokens;
   const Array<Point>&                    m_ranges;
         bool                             m_exclude;
         size_type                        m_maxCount;
         size_type                        m_start, m_end;
         String                           m_errorInfo;
         bool                             m_success = false;

   static IsoString ExtractToken( const TextDatabase::Field& field, const IsoString& line )
   {
      if ( size_type( field.pos-1 + field.width ) > line.Length() )
         throw Error( "Format validation error" );
      return line.Substring( field.pos-1, field.width );
   }

   static bool BracketToken( IsoString::const_iterator& i, IsoString::const_iterator& j, const IsoString& token )
   {
      for ( i = token.Begin(), j = token.End(); ; ++i )
      {
         if ( i == j )
            return false;
         if ( !IsoCharTraits::IsSpace( *i ) )
            break;
      }
      while ( IsoCharTraits::IsSpace( *(j-1) ) )
         --j;
      return true;
   }

   static bool ParseRealToken( double& val, const IsoString& token, double minVal = 0, double maxVal = 0 )
   {
      IsoString::const_iterator i, j;
      if ( BracketToken( i, j, token ) )
      {
         val = IsoString( i, j ).ToDouble();
         if ( minVal < maxVal )
            if ( val < minVal || val > maxVal )
               throw Error( "Numeric value out of range: '" + IsoString( i, j ) + '\'' );
         return true;
      }
      return false;
   }

   static bool ParseRealToken( float& val, const IsoString& token, double minVal = 0, double maxVal = 0 )
   {
      double dval;
      if ( ParseRealToken( dval, token, minVal, maxVal ) )
      {
         val = float( dval );
         return true;
      }
      return false;
   }

   static bool ParseDateToken( double& jd, const IsoString& token, TextDatabase::date_format_type epochFormat )
   {
      IsoString::const_iterator i, j;
      if ( !BracketToken( i, j, token ) )
         return false;

      int yy = 0, mm = 0, dd = 0;
      double df = 0;
      switch ( epochFormat )
      {
      case TextDatabase::DateFormat_ISO:
         // ISO8601 date/time
         jd = TimePoint( token ).JD();
         return true;

      case TextDatabase::DateFormat_Compact:
         // YYYYMMDD[.ddd]
         if ( j-i < 8 )
            throw Error( "Invalid compact date: '" + IsoString( i, j ) + '\'' );
         yy = IsoString( i,   i+4 ).ToInt( 10 );
         mm = IsoString( i+4, i+6 ).ToInt( 10 );
         df = IsoString( i+6, j ).ToDouble();
         dd = TruncInt( df );
         df -= dd;
         break;

      case TextDatabase::DateFormat_Separate:
         // YYYY MM DD[.ddd]
         {
            IsoStringList items;
            token.Break( items, ' ' );
            items.Remove( IsoString() );
            if ( items.Length() != 3 )
               throw Error( "Invalid separate date: '" + IsoString( i, j ) + '\'' );
            yy = items[0].ToInt( 10 );
            mm = items[1].ToInt( 10 );
            df = items[1].ToDouble();
            dd = TruncInt( df );
            df -= dd;
         }
         break;

      case TextDatabase::DateFormat_JD:
         // DDDDDDD.ddd
         jd = token.ToDouble();
         return true;

      case TextDatabase::DateFormat_MJD:
         // +/-DDDDD.ddd
         jd = token.ToDouble() + 2400000.5;
         return true;

      case TextDatabase::DateFormat_PackedMPC:
         // YYYMD[f...]
         if ( j-i < 5 )
            throw Error( "Invalid MPC packed date: '" + IsoString( i, j ) + '\'' );
         switch ( *i )
         {
         case 'I': yy = 1800; break;
         case 'J': yy = 1900; break;
         case 'K': yy = 2000; break;
         default:
            throw Error( "Invalid MPC packed date representation: '" + IsoString( i, j ) + '\'' );
         }
         yy += IsoString( i+1, i+3 ).ToInt( 10 );
         if ( i[3] <= '9' )
            mm = i[3] - '1';
         else
            mm = i[3] - 'A' + 10;
         if ( i[4] <= '9' )
            dd = i[4] - '1';
         else
            dd = i[4] - 'A' + 10;
         if ( j-i > 5 )
            df = ('.' + IsoString( i+5, j )).ToDouble();
         break;
      }

      jd = CalendarTimeToJD( yy, mm, dd, df );
      return true;
   }
};

// ----------------------------------------------------------------------------

Array<TextDatabase::ObjectData> TextDatabase::Search( const EphemerisGeneratorInstance& instance, size_type maxCount ) const
{
   m_format.Validate();

   /*
    * Parse the comma-separated list of object search tokens in the instance.
    * Depending on instance parameters and tokens forms, this may generate
    * regular expressions, literal search tokens, and ranges of asteroid
    * identifiers.
    */
   Array<cregex> regs;
   IsoStringList tokens;
   Array<Point>  ranges;
   {
      IsoStringList tokenList;
      instance.p_objects.ToIsoString().Break( tokenList, ',', true/*trimItems*/ );
      tokenList.Remove( IsoString() );
      if ( instance.p_useRegularExpressions )
      {
         for ( const IsoString& pattern : tokenList )
         {
            try
            {
               // N.B. Do not use operator <<() with Array here, since it
               // conflicts with cregex.
               regs.Add( cregex::compile( pattern.c_str(), regex_constants::ECMAScript | regex_constants::icase ) );
            }
            catch ( ... )
            {
               throw Error( "Invalid regular expression syntax: " + pattern );
            }
         }
      }
      else
      {
         for ( const IsoString& token : tokenList )
            if ( token.Contains( "->" ) )
            {
               IsoStringList items;
               token.Break( items, "->" );
               Point r;
               if ( items.Length() != 2 || !items[0].TryToInt( r.x, 10 ) || !items[1].TryToInt( r.y, 10 ) )
                  throw Error( "Invalid asteroid id range syntax: " + token );
               ranges << r;
            }
            else
               tokens << token;
      }
   }

   if ( instance.p_excludeCoreAsteroids )
      if ( s_excluded.IsEmpty() )
      {
         String filePath = PixInsightSettings::GlobalString( "Application/AsteroidEphemeridesFilePath" );
         if ( filePath.IsEmpty() )
            throw Error( "The core asteroid ephemerides file has not been defined." );
         EphemerisFile ast( filePath );
         EphemerisObjectList asts = ast.Objects();
         for ( const EphemerisObject& a : asts )
            if ( !a.objectId.IsEmpty() )
               s_excluded << a.objectId;
      }

   ElapsedTime T;

   IsoStringList lines = File::ReadLines( instance.p_databaseFilePath );

   if ( lines.Length() <= size_type( m_format.skipHeaderLines ) )
      return Array<ObjectData>();

   size_type numberOfLines = lines.Length() - m_format.skipHeaderLines;

   Console console;
   console.Show();
   Module->ProcessEvents();

   StandardStatus status;
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "EphemerisGenerator: Searching database", numberOfLines );

   Array<size_type> L = Thread::OptimalThreadLoads( numberOfLines, 1024, maxCount );
   ReferenceArray<SearchThread> threads;
   AbstractImage::ThreadData data( monitor, numberOfLines );
   for ( size_type i = 0, n = 0; i < L.Length(); n += L[i++] )
      threads.Add( new SearchThread( data, lines, m_format, regs, tokens, ranges
                                   , instance.p_excludeCoreAsteroids
                                   , maxCount
                                   , n + m_format.skipHeaderLines
                                   , n + m_format.skipHeaderLines + L[i] ) );
   AbstractImage::RunThreads( threads, data );

   for ( SearchThread& thread : threads )
      if ( !thread.Succeeded() )
      {
         String info = thread.ErrorInfo();
         threads.Destroy();
         throw Error( info );
      }

   monitor.Complete();

   Array<ObjectData> objects;
   for ( SearchThread& thread : threads )
      objects << thread.objects;
   objects.Sort();
   if ( maxCount > 0 )
      if ( objects.Length() > maxCount )
         objects.Resize( maxCount );

   threads.Destroy();

   console.WriteLn( "<end><cbr>" + String( objects.Length() ) + " object(s) found in " + T.ToString() );

   return objects;
}

// ----------------------------------------------------------------------------

void TextDatabase::FormatDescription::Validate() const
{
   if ( name.IsEmpty() )
      throw Error( "Missing database format name." );
   if ( !fld_epoch )
      throw Error( "Missing required epoch database field." );
   if ( contents == Contents_OrbitalElements )
   {
      if (  !fld_a && !fld_q
         || !fld_e
         || !fld_M && !fld_T
         || !fld_i || !fld_O || !fld_w )
         throw Error( "Missing required orbital element database fields." );
   }
   else
   {
      if (  !fld_x || !fld_y || !fld_z
         || !fld_vx || !fld_vy || !fld_vz )
         throw Error( "Missing required state vector component database fields." );
   }
}

// ----------------------------------------------------------------------------

Array<TextDatabase::FormatDescription> TextDatabase::FormatDescription::Parse( const String& filePath )
{
   if ( !File::Exists( filePath ) )
      throw String( "The format description file does not exist." );

   /*
    * XODF - eXtensible Object Database Description Format
    */
   XMLDocument xml;
   xml.SetParserOption( XMLParserOption::IgnoreComments );
   xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
   xml.Parse( File::ReadTextFile( filePath ).UTF8ToUTF16() );

   if ( xml.RootElement() == nullptr )
      throw Error( "The XML document has no root element." );
   if ( xml.RootElement()->Name() != "xodf" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
      throw Error( "Not an XODF version 1.0 document." );

   Console console;

   Array<FormatDescription> formats;

   for ( const XMLNode& node : *xml.RootElement() )
   {
      if ( !node.IsElement() )
      {
         console.WarningLn( "<end><cbr>** Warning: <raw>" + filePath
                           + "</raw>: Ignoring unexpected XML root child node of " + XMLNodeType::AsString( node.NodeType() ) + " type." );
         continue;
      }

      const XMLElement& topLevelElement = static_cast<const XMLElement&>( node );

      try
      {
         if ( topLevelElement.Name() == "Format" )
         {
            String name = topLevelElement.AttributeValue( "name" );
            if ( name.IsEmpty() )
               throw Error( "Missing required name attribute" );

            for ( const auto& format : formats )
               if ( format.name.CompareIC( name ) == 0 )
                  throw Error( "Duplicate format description '" + name + "'" );

            FormatDescription format;
            format.name = name;

            for ( const XMLNode& node : topLevelElement )
            {
               if ( !node.IsElement() )
               {
                  console.WarningLn( "<end><cbr>** Warning: <raw>" + filePath
                                    + "</raw>: Ignoring unexpected Format child node of " + XMLNodeType::AsString( node.NodeType() ) + " type." );
                  continue;
               }

               const XMLElement& element = static_cast<const XMLElement&>( node );

               try
               {
                  if ( element.Name() == "Field" )
                  {
                     String sid = element.AttributeValue( "id" );
                     if ( sid.IsEmpty() )
                        throw Error( "Missing required Field id attribute" );

                     String spos = element.AttributeValue( "pos" );
                     if ( spos.IsEmpty() )
                        throw Error( "Missing required pos attribute" );

                     int pos = spos.ToInt( 10 );
                     if ( pos < 1 )
                        throw Error( "Invalid Field pos attribute value '" + spos + "'" );

                     int width = 0;
                     if ( format.fieldDisposition == FieldDisposition_FixedWidth )
                     {
                        String swidth = element.AttributeValue( "width" );
                        if ( swidth.IsEmpty() )
                           throw Error( "Missing required width attribute" );
                        width = swidth.ToInt( 10 );
                        if ( width < 1 )
                           throw Error( "Invalid Field width attribute value '" + swidth + "'" );
                     }

                     switch ( sid.Hash32() )
                     {
                     case 0x34014e80: // 'id'
                        format.fld_id.pos = pos;
                        format.fld_id.width = width;
                        break;
                     case 0x15111066: // 'name'
                        format.fld_name.pos = pos;
                        format.fld_name.width = width;
                        break;
                     case 0xf736088c: // 'epoch'
                        format.fld_epoch.pos = pos;
                        format.fld_epoch.width = width;
                        break;
                     case 0x3bf8312c: // 'H'
                        format.fld_H.pos = pos;
                        format.fld_H.width = width;
                        break;
                     case 0x5ce66b21: // 'G'
                        format.fld_G.pos = pos;
                        format.fld_G.width = width;
                        break;
                     case 0x81f9bc9e: // 'B_V'
                        format.fld_B_V.pos = pos;
                        format.fld_B_V.width = width;
                        break;
                     case 0x94a883a6: // 'D'
                        format.fld_D.pos = pos;
                        format.fld_D.width = width;
                        break;
                     case 0x2cd5278b: // 'a'
                        format.fld_a.pos = pos;
                        format.fld_a.width = width;
                        break;
                     case 0x06e9649a: // 'q'
                        format.fld_q.pos = pos;
                        format.fld_q.width = width;
                        break;
                     case 0xfc192325: // 'e'
                        format.fld_e.pos = pos;
                        format.fld_e.width = width;
                        break;
                     case 0x0c22181c: // 'M'
                        format.fld_M.pos = pos;
                        format.fld_M.width = width;
                        break;
                     case 0xdf8e4e9f: // 'T'
                        format.fld_T.pos = pos;
                        format.fld_T.width = width;
                        break;
                     case 0x3b35f6d2: // 'i'
                        format.fld_i.pos = pos;
                        format.fld_i.width = width;
                        break;
                     case 0x1317836a: // 'O'
                        format.fld_O.pos = pos;
                        format.fld_O.width = width;
                        break;
                     case 0x6f050b82: // 'w'
                        format.fld_w.pos = pos;
                        format.fld_w.width = width;
                        break;
                     case 0x1d548d86: // 'x'
                        format.fld_x.pos = pos;
                        format.fld_x.width = width;
                        break;
                     case 0xc9d5b642: // 'y'
                        format.fld_y.pos = pos;
                        format.fld_y.width = width;
                        break;
                     case 0xd2b03dc3: // 'z'
                        format.fld_z.pos = pos;
                        format.fld_z.width = width;
                        break;
                     case 0xcf7d008c: // 'vx'
                        format.fld_vx.pos = pos;
                        format.fld_vx.width = width;
                        break;
                     case 0x000eba9a: // 'vy'
                        format.fld_vy.pos = pos;
                        format.fld_vy.width = width;
                        break;
                     case 0x8bf1cecf: // 'vz'
                        format.fld_vz.pos = pos;
                        format.fld_vz.width = width;
                        break;
                     default:
                        throw Error( "Unknown Field id attribute value '" + sid + "'" );
                     }
                  }
                  else if ( element.Name() == "Database" )
                  {
                     String scontents = element.AttributeValue( "contents" );
                     String sfieldDisposition = element.AttributeValue( "fieldDisposition" );
                     String sepochFormat = element.AttributeValue( "epochFormat" );
                     String stimePPFormat = element.AttributeValue( "timePPFormat" );
                     String sidFormat = element.AttributeValue( "idFormat" );
                     String sskipHeaderLines = element.AttributeValue( "skipHeaderLines" );

                     if ( !scontents.IsEmpty() )
                        if ( scontents == "OrbitalElements" )
                           format.contents = Contents_OrbitalElements;
                        else if ( scontents == "StateVectors" )
                           format.contents = Contents_StateVectors;
                        else
                           throw Error( "Unknown Database contents attribute value '" + scontents + '\'' );

                     if ( !sfieldDisposition.IsEmpty() )
                        if ( sfieldDisposition == "CommaSeparated" )
                           format.fieldDisposition = FieldDisposition_CommaSeparated;
                        else if ( sfieldDisposition == "FixedWidth" )
                           format.fieldDisposition = FieldDisposition_FixedWidth;
                        else
                           throw Error( "Unknown Database fieldDisposition attribute value '" + sfieldDisposition + '\'' );

                     if ( !sepochFormat.IsEmpty() )
                        if ( sepochFormat == "ISO" )
                           format.epochFormat = DateFormat_ISO;
                        else if ( sepochFormat == "Compact" )
                           format.epochFormat = DateFormat_Compact;
                        else if ( sepochFormat == "Separate" )
                           format.epochFormat = DateFormat_Separate;
                        else if ( sepochFormat == "JD" )
                           format.epochFormat = DateFormat_JD;
                        else if ( sepochFormat == "MJD" )
                           format.epochFormat = DateFormat_MJD;
                        else if ( sepochFormat == "PackedMPC" )
                           format.epochFormat = DateFormat_PackedMPC;
                        else
                           throw Error( "Unknown Database epochFormat attribute value '" + sepochFormat + '\'' );

                     if ( !stimePPFormat.IsEmpty() )
                        if ( stimePPFormat == "ISO" )
                           format.timePPFormat = DateFormat_ISO;
                        else if ( stimePPFormat == "Compact" )
                           format.timePPFormat = DateFormat_Compact;
                        else if ( stimePPFormat == "Separate" )
                           format.timePPFormat = DateFormat_Separate;
                        else if ( stimePPFormat == "JD" )
                           format.timePPFormat = DateFormat_JD;
                        else if ( stimePPFormat == "MJD" )
                           format.timePPFormat = DateFormat_MJD;
                        else if ( stimePPFormat == "PackedMPC" )
                           format.timePPFormat = DateFormat_PackedMPC;
                        else
                           throw Error( "Unknown Database timePPFormat attribute value '" + stimePPFormat + '\'' );

                     if ( !sidFormat.IsEmpty() )
                        if ( sidFormat == "Plain" )
                           format.idFormat = IdFormat_Plain;
                        else if ( sidFormat == "PackedMPC" )
                           format.idFormat = IdFormat_PackedMPC;
                        else if ( sidFormat == "Parenthesized" )
                           format.idFormat = IdFormat_Parenthesized;
                        else
                           throw Error( "Unknown Database idFormat attribute value '" + sidFormat + '\'' );

                     if ( !sskipHeaderLines.IsEmpty() )
                     {
                        format.skipHeaderLines = sskipHeaderLines.ToInt( 10 );
                        if ( format.skipHeaderLines < 0 )
                           throw Error( "Invalid Database skipHeaderLines attribute value '" + sskipHeaderLines + "'" );
                     }
                  }
                  else
                  {
                     console.WarningLn( "<end><cbr>** Warning: <raw>" + filePath
                                       + "</raw>: Ignoring unknown child element '" + element.Name() + "'" );
                     continue;
                  }
               }
               catch ( Exception& x )
               {
                  try
                  {
                     throw XMLParseError( element, "<raw>" + filePath
                                 + "</raw>: Parsing " + element.Name() + " child element", x.Message() );
                  }
                  catch ( Exception& x )
                  {
                     x.Show();
                  }
               }
               catch ( ... )
               {
                  throw;
               }
            }

            if ( !format.fld_epoch )
               throw Error( "Missing required epoch field description." );

            if ( format.contents == Contents_OrbitalElements )
            {
               if (  !format.fld_a && !format.fld_q
                  || !format.fld_e
                  || !format.fld_M && !format.fld_T
                  || !format.fld_i || !format.fld_O || !format.fld_w )
                  throw Error( "Missing required orbital element field description(s)." );
            }
            else // Contents_StateVectors
            {
               if (  !format.fld_x || !format.fld_y || !format.fld_z
                  || !format.fld_vx || !format.fld_vy || !format.fld_vz )
                  throw Error( "Missing required state vector component field description(s)." );
            }

            formats << format;
         }
         else
         {
            console.WarningLn( "<end><cbr>** Warning: <raw>" + filePath
                              + "</raw>: Ignoring unknown top-level element '" + topLevelElement.Name() + "'" );
            continue;
         }
      }
      catch ( Exception& x )
      {
         try
         {
            throw XMLParseError( topLevelElement, "<raw>" + filePath
                                 + "</raw>: Parsing " + topLevelElement.Name() + " top-level element", x.Message() );
         }
         catch ( Exception& x )
         {
            x.Show();
         }
      }
      catch ( ... )
      {
         throw;
      }
   }

   if ( formats.IsEmpty() )
      throw Error( "EphemerisGenerator: No database formats have been defined." );

   return formats;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF TextDatabase.cpp - Released 2021-04-09T19:41:48Z
