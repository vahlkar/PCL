//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard APASS Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// APASSInstance.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard APASS PixInsight module.
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

#include "APASSInstance.h"
#include "APASSParameters.h"
#include "APASSProcess.h"

#include <pcl/Console.h>
#include <pcl/ElapsedTime.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

APASSInstance::APASSInstance( const MetaProcess* P )
   : ProcessImplementation( P )
   , p_command( TheACommandParameter->DefaultValue() )
   , p_verbosity( int32( TheAVerbosityParameter->DefaultValue() ) )
   , p_dataRelease( ADataRelease::Default )
   , p_sortBy( ASortBy::Default )
   , p_generateTextOutput( TheAGenerateTextOutputParameter->DefaultValue() )
   , p_textFormat( ATextFormat::Default )
   , p_textHeaders( ATextHeaders::Default )
{
}

// ----------------------------------------------------------------------------

APASSInstance::APASSInstance( const APASSInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void APASSInstance::Assign( const ProcessImplementation& p )
{
   const APASSInstance* x = dynamic_cast<const APASSInstance*>( &p );
   if ( x != nullptr )
   {
      p_command = x->p_command;
      p_verbosity = x->p_verbosity;
      p_dataRelease = x->p_dataRelease;
      p_sortBy = x->p_sortBy;
      p_generateTextOutput = x->p_generateTextOutput;
      p_textFormat = x->p_textFormat;
      p_textHeaders = x->p_textHeaders;
      p_outputFilePath = x->p_outputFilePath;
      p_searchData = x->p_searchData;
      o_isValid = x->o_isValid;
      o_dataRelease = x->o_dataRelease;
      o_databaseFilePaths = x->o_databaseFilePaths;
      o_databaseMagnitudeLow = x->o_databaseMagnitudeLow;
      o_databaseMagnitudeHigh = x->o_databaseMagnitudeHigh;
   }
}

// ----------------------------------------------------------------------------

bool APASSInstance::CanExecuteOn( const View&, String& whyNot ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool APASSInstance::CanExecuteGlobal( String& whyNot ) const
{
   whyNot = "APASS can only be executed in the global context.";
   return true;
}

// ----------------------------------------------------------------------------

bool APASSInstance::ExecuteGlobal()
{
   /*
    * Clear output data.
    */
   p_searchData.ResetSearchResults();
   o_isValid = false;
   o_dataRelease = OutputDataRelease();
   o_databaseFilePaths.Clear();
   o_databaseMagnitudeLow = o_databaseMagnitudeHigh = 0;

   /*
    * Install and initialize XPSD files. This will only happen the first time
    * this function is executed, or if preferences have been changed.
    */
   TheAPASSProcess->EnsureDatabasesInitialized( o_dataRelease, p_verbosity );

   if ( likely( p_command == "search" ) )
   {
      Search();
      if ( p_generateTextOutput )
         GenerateTextOutput();
   }
   else if ( p_command == "get-info" )
   {
      GetInfo();
   }
   else
   {
      if ( p_verbosity > 0 )
         Console().WarningLn( "<end><cbr>** Warning: Unknown/unsupported command '" + p_command + "'." );
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

class APASSSearchThread : public Thread
{
public:

   APASSSearchData data;

   APASSSearchThread( const APASSSearchData& searchData, pcl_enum dataRelease, int beginIndex, int endIndex )
      : data( searchData )
      , m_dataRelease( dataRelease )
      , m_beginIndex( beginIndex )
      , m_endIndex( endIndex )
   {
   }

   void Run() override
   {
      for ( int i = m_beginIndex; i < m_endIndex; ++i )
         TheAPASSProcess->Databases( m_dataRelease )[i].Search( data );
   }

private:

   pcl_enum m_dataRelease;
   int      m_beginIndex, m_endIndex;
};

void APASSInstance::Search()
{
   GetInfo();
   if ( !o_isValid )
      throw Error( "No database files have been selected for APASS " + ADataRelease::ReleaseName( o_dataRelease ) );

   switch ( p_verbosity )
   {
   default:
   case 0:
      break;
   case 1:
      Console().WriteLn( "<end><cbr>Executing APASS "
         + ADataRelease::ReleaseName( o_dataRelease ) + " search command..." );
      break;
   case 2:
      Console().WriteLn( "<end><cbr>Executing APASS "
         + ADataRelease::ReleaseName( o_dataRelease ) + " search command:"
         + IsoString().Format( "\nCenter Right Ascension ..... %13.9f deg"
                               "\nCenter Declination ......... %+13.9f deg"
                               "\nSearch radius .............. %13.9f deg"
                               "\nMagnitude low limit ........ %6.3f"
                               "\nMagnitude high limit ....... %6.3f"
                               "\nSource count limit ......... %u"
                               "\nRequired flags ............. %08x"
                               "\nInclusion flags ............ %08x"
                               "\nExclusion flags ............ %08x"
                               , p_searchData.centerRA
                               , p_searchData.centerDec
                               , p_searchData.radius
                               , p_searchData.magnitudeLow
                               , p_searchData.magnitudeHigh
                               , p_searchData.sourceLimit
                               , p_searchData.requiredFlags
                               , p_searchData.inclusionFlags
                               , p_searchData.exclusionFlags ) );
      break;
   }

   const ReferenceArray<APASSDatabaseFile>& databases = TheAPASSProcess->Databases( o_dataRelease );

   /*
    * Find the set of databases necessary for the specified magnitude range.
    */
   int firstDatabaseIndex = 0;
   for ( const APASSDatabaseFile& database : databases )
   {
      if ( database.MagnitudeHigh() >= p_searchData.magnitudeLow )
         break;
      ++firstDatabaseIndex;
   }
   if ( firstDatabaseIndex == int( databases.Length() ) )
   {
      if ( p_verbosity > 0 )
         Console().WarningLn( "<end><cbr>** Warning: The specified magnitude interval is out of the range supported by installed databases." );
      return;
   }

   int lastDatabaseIndex = firstDatabaseIndex;
   for ( const APASSDatabaseFile& database : databases )
   {
      if ( database.MagnitudeHigh() >= p_searchData.magnitudeHigh )
         break;
      ++lastDatabaseIndex;
   }
   if ( lastDatabaseIndex == int( databases.Length() ) )
      --lastDatabaseIndex;

   /*
    * Perform a database search.
    */
   Array<size_type> L = Thread::OptimalThreadLoads( lastDatabaseIndex - firstDatabaseIndex + 1 );
   if ( L.Length() == 1 )
   {
      // Single-file search
      databases[firstDatabaseIndex].Search( p_searchData );
   }
   else
   {
      // Multiple file parallel search.
      if ( p_verbosity > 1 )
         Console().WriteLn( String().Format( "<br>* Using %u parallel search threads.", L.Length() ) );

      ReferenceArray<APASSSearchThread> threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += L[i++] )
         threads.Add( new APASSSearchThread( p_searchData, o_dataRelease,
                                               firstDatabaseIndex + n,
                                               firstDatabaseIndex + n + L[i] ) );
      int i = 0;
      for ( APASSSearchThread& thread : threads )
         thread.Start( ThreadPriority::DefaultMax, i++ );
      for ( APASSSearchThread& thread : threads )
         thread.Wait();

      for ( APASSSearchThread& thread : threads )
      {
         p_searchData.stars << thread.data.stars;
         p_searchData.excessCount += thread.data.excessCount;
         p_searchData.rejectCount += thread.data.rejectCount;
         p_searchData.countIO += thread.data.countIO;
         if ( thread.data.timeTotal > p_searchData.timeTotal )
            p_searchData.timeTotal = thread.data.timeTotal;
         if ( thread.data.timeIO > p_searchData.timeIO )
            p_searchData.timeIO = thread.data.timeIO;
         if ( thread.data.timeUncompress > p_searchData.timeUncompress )
            p_searchData.timeUncompress = thread.data.timeUncompress;
         if ( thread.data.timeDecode > p_searchData.timeDecode )
            p_searchData.timeDecode = thread.data.timeDecode;
      }

      threads.Destroy();
   }

   /*
    * If requested, sort the output list of point sources.
    */
   switch ( p_sortBy )
   {
   case ASortBy::DontSort:
      break;
   case ASortBy::RA:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.ra < s2.ra; } );
      break;
   case ASortBy::Dec:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.dec < s2.dec; } );
      break;
   case ASortBy::V:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.mag_V < s2.mag_V; } );
      break;
   case ASortBy::B:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.mag_B < s2.mag_B; } );
      break;
   case ASortBy::g:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.mag_g < s2.mag_g; } );
      break;
   case ASortBy::r:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.mag_r < s2.mag_r; } );
      break;
   case ASortBy::i:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.mag_i < s2.mag_i; } );
      break;
   case ASortBy::z_s:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.mag_z_s < s2.mag_z_s; } );
      break;
   }

   /*
    * Truncate the output list of sources. This may be necessary in case we
    * have executed multiple worker threads.
    */
   if ( p_searchData.stars.Length() > p_searchData.sourceLimit )
      p_searchData.stars.Resize( p_searchData.sourceLimit );

   switch ( p_verbosity )
   {
   default:
   case 0:
      break;
   case 1:
      Console().WriteLn( "<end><cbr>" + String( p_searchData.stars.Length() ) + " sources found." );
      break;
   case 2:
      Console().WriteLn(
         "<end><cbr><br>Search results:"
         "\nTotal sources .............. " + String( p_searchData.stars.Length() ) +
         "\nExcess count ............... " + String( p_searchData.excessCount ) +
         "\nReject count ............... " + String( p_searchData.rejectCount ) +
         "\nTotal search time .......... " + ElapsedTime::ToString( p_searchData.timeTotal, 2 ) +
         "\nTotal I/O time ............. " + ElapsedTime::ToString( p_searchData.timeIO, 2 ) +
         "\nTotal I/O operations ....... " + String( p_searchData.countIO ) +
         "\nTotal uncompression time ... " + ElapsedTime::ToString( p_searchData.timeUncompress, 2 ) +
         "\nTotal decoding time ........ " + ElapsedTime::ToString( p_searchData.timeDecode, 2 ) );
      break;
   }
}

// ----------------------------------------------------------------------------

void APASSInstance::GenerateTextOutput() const
{
   if ( p_outputFilePath.Trimmed().IsEmpty() )
   {
      if ( p_verbosity > 0 )
         Console().WarningLn( "<end><cbr><br>** Warning: No output file path has been specified." );
      return;
   }

   File f = File::CreateFileForWriting( p_outputFilePath.Trimmed() );

   switch ( p_textFormat )
   {
   default: // ?!
   case ATextFormat::CSV:
      if ( p_textHeaders == ATextHeaders::SearchParameters
        || p_textHeaders == ATextHeaders::SearchParametersAndTableColumns )
      {
         f.OutTextLn( "center_ra,center_dec,radius,magnitude_low,magnitude_high,source_count_limit,required_flags,inclusion_flags,exclusion_flags" );
         f.OutTextLn( IsoString().Format( "%.9f,%.9f,%.9f,%.3f,%.3f,%u,%x,%x,%x"
                                 , p_searchData.centerRA
                                 , p_searchData.centerDec
                                 , p_searchData.radius
                                 , p_searchData.magnitudeLow
                                 , p_searchData.magnitudeHigh
                                 , p_searchData.sourceLimit
                                 , p_searchData.requiredFlags
                                 , p_searchData.inclusionFlags
                                 , p_searchData.exclusionFlags ) );
      }
      if ( p_textHeaders == ATextHeaders::TableColumns
        || p_textHeaders == ATextHeaders::SearchParametersAndTableColumns )
      {
         f.OutTextLn( "ra,dec,mag_V,mag_B,mag_g,mag_r,mag_i,mag_z_s,err_V,err_B,err_g,err_r,err_i,err_z_s,flags" );
      }
      for ( const APASSStarData& star : p_searchData.stars )
      {
         IsoString sa = IsoString().Format( "%.9f", star.ra );
         IsoString sd = IsoString().Format( "%.9f", star.dec );

         IsoString smag_V = (star.flags & APASSStarFlag::NoMag_V) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.mag_V );
         IsoString smag_B = (star.flags & APASSStarFlag::NoMag_B) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.mag_B );
         IsoString smag_g = (star.flags & APASSStarFlag::NoMag_g) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.mag_g );
         IsoString smag_r = (star.flags & APASSStarFlag::NoMag_r) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.mag_r );
         IsoString smag_i = (star.flags & APASSStarFlag::NoMag_i) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.mag_i );
         IsoString smag_z_s = (star.flags & APASSStarFlag::NoMag_z_s) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.mag_z_s );

         IsoString serr_V = (star.flags & APASSStarFlag::NoMag_V) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.err_V );
         IsoString serr_B = (star.flags & APASSStarFlag::NoMag_B) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.err_B );
         IsoString serr_g = (star.flags & APASSStarFlag::NoMag_g) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.err_g );
         IsoString serr_r = (star.flags & APASSStarFlag::NoMag_r) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.err_r );
         IsoString serr_i = (star.flags & APASSStarFlag::NoMag_i) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.err_i );
         IsoString serr_z_s = (star.flags & APASSStarFlag::NoMag_z_s) ?
                                IsoString()
                              : IsoString().Format( "%.3f", star.err_z_s );

         IsoString sflags = IsoString().Format( "%x", star.flags );

         f.OutTextLn( sa + ',' + sd
                    + ',' + smag_V + ',' + smag_B + ',' + smag_g + ',' + smag_r + ',' + smag_i + ',' + smag_z_s
                    + ',' + serr_V + ',' + serr_B + ',' + serr_g + ',' + serr_r + ',' + serr_i + ',' + serr_z_s
                    + ',' + sflags );
      }
      break;

   case ATextFormat::Tabular:
      if ( p_textHeaders == ATextHeaders::SearchParameters
        || p_textHeaders == ATextHeaders::SearchParametersAndTableColumns )
      {
         f.OutTextLn( "APASS " + ADataRelease::ReleaseName( o_dataRelease ) + " Search"
           + IsoString().Format( "\n================="
                                 "\nTotal sources ............ %u"
                                 "\nCenter Right Ascension ... %13.9f deg"
                                 "\nCenter Declination ....... %+13.9f deg"
                                 "\nSearch radius ............ %13.9f deg"
                                 "\nMagnitude low limit ...... %6.3f"
                                 "\nMagnitude high limit ..... %6.3f"
                                 "\nSource count limit ....... %u"
                                 "\nRequired flags ........... %08x"
                                 "\nInclusion flags .......... %08x"
                                 "\nExclusion flags .......... %08x"
                                 "\n"
                                 , p_searchData.stars.Length()
                                 , p_searchData.centerRA
                                 , p_searchData.centerDec
                                 , p_searchData.radius
                                 , p_searchData.magnitudeLow
                                 , p_searchData.magnitudeHigh
                                 , p_searchData.sourceLimit
                                 , p_searchData.requiredFlags
                                 , p_searchData.inclusionFlags
                                 , p_searchData.exclusionFlags ) );
      }
      if ( p_textHeaders == ATextHeaders::TableColumns
        || p_textHeaders == ATextHeaders::SearchParametersAndTableColumns )
      {
#ifdef _MSC_VER
         f.OutTextLn( "      \xce\xb1             \xce\xb4          V      B     g'     r'     i'     z_s    e_V    e_B    e_g'   e_r'   e_i'  e_z_s   Flags" );
         f.OutTextLn( "---------- ---------- ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ --------" );
         f.OutTextLn( "   \xc2\xb0          \xc2\xb0         mag    mag    mag    mag    mag    mag    mag    mag    mag    mag    mag    mag" );
         f.OutTextLn( "========== ========== ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ========" );
#else
         f.OutTextLn( String( u"    α          δ         V      B     g'     r'     i'     z_s    e_V    e_B    e_g'   e_r'   e_i'  e_z_s   Flags"  ).ToUTF8() );
         f.OutTextLn(          "---------- ---------- ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ --------" );
         f.OutTextLn( String( u"   °          °         mag    mag    mag    mag    mag    mag    mag    mag    mag    mag    mag    mag" ).ToUTF8() );
         f.OutTextLn(          "========== ========== ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ========" );
#endif
      }
      for ( const APASSStarData& star : p_searchData.stars )
      {
         IsoString sa = IsoString().Format( "%10.6f", star.ra );
         IsoString sd = IsoString().Format( "%+10.6f", star.dec );

         IsoString smag_V = (star.flags & APASSStarFlag::NoMag_V) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_V );
         IsoString smag_B = (star.flags & APASSStarFlag::NoMag_B) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_B );
         IsoString smag_g = (star.flags & APASSStarFlag::NoMag_g) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_g );
         IsoString smag_r = (star.flags & APASSStarFlag::NoMag_r) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_r );
         IsoString smag_i = (star.flags & APASSStarFlag::NoMag_i) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_i );
         IsoString smag_z_s = (star.flags & APASSStarFlag::NoMag_z_s) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_z_s );

         IsoString serr_V = (star.flags & APASSStarFlag::NoMag_V) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_V );
         IsoString serr_B = (star.flags & APASSStarFlag::NoMag_B) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_B );
         IsoString serr_g = (star.flags & APASSStarFlag::NoMag_g) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_g );
         IsoString serr_r = (star.flags & APASSStarFlag::NoMag_r) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_r );
         IsoString serr_i = (star.flags & APASSStarFlag::NoMag_i) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_i );
         IsoString serr_z_s = (star.flags & APASSStarFlag::NoMag_z_s) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_z_s );

         IsoString sflags = IsoString().Format( "%08x", star.flags );

         f.OutTextLn( sa + ' ' + sd
                    + ' ' + smag_V + ' ' + smag_B + ' ' + smag_g + ' ' + smag_r + ' ' + smag_i + ' ' + smag_z_s
                    + ' ' + serr_V + ' ' + serr_B + ' ' + serr_g + ' ' + serr_r + ' ' + serr_i + ' ' + serr_z_s
                    + ' ' + sflags );
      }
      break;

   case ATextFormat::TabularCompound:
      if ( p_textHeaders == ATextHeaders::SearchParameters
        || p_textHeaders == ATextHeaders::SearchParametersAndTableColumns )
      {
         IsoString sa = IsoString::ToSexagesimal( p_searchData.centerRA/15, RAConversionOptions( 4/*precision*/, 3/*width*/ ) );
         IsoString sd = IsoString::ToSexagesimal( p_searchData.centerDec, DecConversionOptions( 3/*precision*/ ) );
         IsoString sr = IsoString::ToSexagesimal( p_searchData.radius, AngleConversionOptions( 3/*precision*/ ) );
         f.OutTextLn(            "APASS " + ADataRelease::ReleaseName( o_dataRelease ) + " Search"
                                 "\n================="
                                 "\nTotal sources ............ " + IsoString( p_searchData.stars.Length() ) +
                                 "\nCenter Right Ascension ... " + sa + " (hms)"
#ifdef _MSC_VER
                                 "\nCenter Declination ....... " + sd + "  (\xc2\xb0\xe2\x80\xb2\xe2\x80\xb3)" +
                                 "\nSearch radius ............ " + sr + "  (\xc2\xb0\xe2\x80\xb2\xe2\x80\xb3)" +
#else
                                 "\nCenter Declination ....... " + sd + String( u"  (°′″)" ).ToUTF8() +
                                 "\nSearch radius ............ " + sr + String( u"  (°′″)" ).ToUTF8() +
#endif
                        IsoString().Format(
                                 "\nMagnitude low limit ...... %6.3f"
                                 "\nMagnitude high limit ..... %6.3f"
                                 "\nSource count limit ....... %u"
                                 "\nRequired flags ........... %08x"
                                 "\nInclusion flags .......... %08x"
                                 "\nExclusion flags .......... %08x"
                                 "\n"
                                 , p_searchData.magnitudeLow
                                 , p_searchData.magnitudeHigh
                                 , p_searchData.sourceLimit
                                 , p_searchData.requiredFlags
                                 , p_searchData.inclusionFlags
                                 , p_searchData.exclusionFlags ) );
      }
      if ( p_textHeaders == ATextHeaders::TableColumns
        || p_textHeaders == ATextHeaders::SearchParametersAndTableColumns )
      {
#ifdef _MSC_VER
         f.OutTextLn( "     \xce\xb1             \xce\xb4         V      B     g'     r'     i'     z_s    e_V    e_B    e_g'   e_r'   e_i'  e_z_s   Flags" );
         f.OutTextLn( "------------ ------------ ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ --------" );
         f.OutTextLn( "  h  m  s       \xc2\xb0  \xe2\x80\xb2  \xe2\x80\xb3     mag    mag    mag    mag    mag    mag    mag    mag    mag    mag    mag    mag" );
         f.OutTextLn( "============ ============ ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ========" );
#else
         f.OutTextLn( String( u"     α             δ         V      B     g'     r'     i'     z_s    e_V    e_B    e_g'   e_r'   e_i'  e_z_s   Flags" ).ToUTF8() );
         f.OutTextLn(          "------------ ------------ ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ ------ --------" );
         f.OutTextLn( String( u"  h  m  s       °  ′  ″     mag    mag    mag    mag    mag    mag    mag    mag    mag    mag    mag    mag" ).ToUTF8() );
         f.OutTextLn(          "============ ============ ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ====== ========" );
#endif
      }
      for ( const APASSStarData& star : p_searchData.stars )
      {
         IsoString sa = IsoString::ToSexagesimal( star.ra/15, RAConversionOptions( 3/*precision*/ ) );
         IsoString sd = IsoString::ToSexagesimal( star.dec, DecConversionOptions( 2/*precision*/ ) );

         IsoString smag_V = (star.flags & APASSStarFlag::NoMag_V) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_V );
         IsoString smag_B = (star.flags & APASSStarFlag::NoMag_B) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_B );
         IsoString smag_g = (star.flags & APASSStarFlag::NoMag_g) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_g );
         IsoString smag_r = (star.flags & APASSStarFlag::NoMag_r) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_r );
         IsoString smag_i = (star.flags & APASSStarFlag::NoMag_i) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_i );
         IsoString smag_z_s = (star.flags & APASSStarFlag::NoMag_z_s) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.mag_z_s );

         IsoString serr_V = (star.flags & APASSStarFlag::NoMag_V) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_V );
         IsoString serr_B = (star.flags & APASSStarFlag::NoMag_B) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_B );
         IsoString serr_g = (star.flags & APASSStarFlag::NoMag_g) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_g );
         IsoString serr_r = (star.flags & APASSStarFlag::NoMag_r) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_r );
         IsoString serr_i = (star.flags & APASSStarFlag::NoMag_i) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_i );
         IsoString serr_z_s = (star.flags & APASSStarFlag::NoMag_z_s) ?
                                IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.err_z_s );

         IsoString sflags = IsoString().Format( "%08x", star.flags );

         f.OutTextLn( sa + ' ' + sd
                    + ' ' + smag_V + ' ' + smag_B + ' ' + smag_g + ' ' + smag_r + ' ' + smag_i + ' ' + smag_z_s
                    + ' ' + serr_V + ' ' + serr_B + ' ' + serr_g + ' ' + serr_r + ' ' + serr_i + ' ' + serr_z_s
                    + ' ' + sflags );
      }
      break;
   }

   f.Flush();

   if ( p_verbosity > 0 )
      Console().WriteLn( "<end><cbr><br>* Output text file generated: " + p_outputFilePath );
}

// ----------------------------------------------------------------------------

void APASSInstance::GetInfo()
{
   o_isValid = TheAPASSProcess->IsValid( o_dataRelease );
   o_databaseFilePaths = TheAPASSProcess->DatabaseFilePaths( o_dataRelease );
   o_databaseMagnitudeLow = TheAPASSProcess->MagnitudeLow( o_dataRelease );
   o_databaseMagnitudeHigh = TheAPASSProcess->MagnitudeHigh( o_dataRelease );
}

// ----------------------------------------------------------------------------

void* APASSInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheACommandParameter )
      return p_command.Begin();
   if ( p == TheAVerbosityParameter )
      return &p_verbosity;
   if ( p == TheADataReleaseParameter )
      return &p_dataRelease;
   if ( p == TheASortByParameter )
      return &p_sortBy;
   if ( p == TheAGenerateTextOutputParameter )
      return &p_generateTextOutput;
   if ( p == TheATextFormatParameter )
      return &p_textFormat;
   if ( p == TheATextHeadersParameter )
      return &p_textHeaders;
   if ( p == TheAOutputFilePathParameter )
      return p_outputFilePath.Begin();
   if ( p == TheACenterRAParameter )
      return &p_searchData.centerRA;
   if ( p == TheACenterDecParameter )
      return &p_searchData.centerDec;
   if ( p == TheARadiusParameter )
      return &p_searchData.radius;
   if ( p == TheAMagnitudeLowParameter )
      return &p_searchData.magnitudeLow;
   if ( p == TheAMagnitudeHighParameter )
      return &p_searchData.magnitudeHigh;
   if ( p == TheASourceLimitParameter )
      return &p_searchData.sourceLimit;
   if ( p == TheARequiredFlagsParameter )
      return &p_searchData.requiredFlags;
   if ( p == TheAInclusionFlagsParameter )
      return &p_searchData.inclusionFlags;
   if ( p == TheAExclusionFlagsParameter )
      return &p_searchData.exclusionFlags;

   if ( p == TheASourceRAParameter )
      return &p_searchData.stars[tableRow].ra;
   if ( p == TheASourceDecParameter )
      return &p_searchData.stars[tableRow].dec;
   if ( p == TheASourceMag_VParameter )
      return &p_searchData.stars[tableRow].mag_V;
   if ( p == TheASourceMag_BParameter )
      return &p_searchData.stars[tableRow].mag_B;
   if ( p == TheASourceMag_gParameter )
      return &p_searchData.stars[tableRow].mag_g;
   if ( p == TheASourceMag_rParameter )
      return &p_searchData.stars[tableRow].mag_r;
   if ( p == TheASourceMag_iParameter )
      return &p_searchData.stars[tableRow].mag_i;
   if ( p == TheASourceMag_z_sParameter )
      return &p_searchData.stars[tableRow].mag_z_s;
   if ( p == TheASourceMag_V_ErrorParameter )
      return &p_searchData.stars[tableRow].err_V;
   if ( p == TheASourceMag_B_ErrorParameter )
      return &p_searchData.stars[tableRow].err_B;
   if ( p == TheASourceMag_g_ErrorParameter )
      return &p_searchData.stars[tableRow].err_g;
   if ( p == TheASourceMag_r_ErrorParameter )
      return &p_searchData.stars[tableRow].err_r;
   if ( p == TheASourceMag_i_ErrorParameter )
      return &p_searchData.stars[tableRow].err_i;
   if ( p == TheASourceMag_z_s_ErrorParameter )
      return &p_searchData.stars[tableRow].err_z_s;
   if ( p == TheASourceFlagsParameter )
      return &p_searchData.stars[tableRow].flags;
   if ( p == TheAExcessCountParameter )
      return &p_searchData.excessCount;
   if ( p == TheARejectCountParameter )
      return &p_searchData.rejectCount;
   if ( p == TheATimeTotalParameter )
      return &p_searchData.timeTotal;
   if ( p == TheATimeIOParameter )
      return &p_searchData.timeIO;
   if ( p == TheACountIOParameter )
      return &p_searchData.countIO;
   if ( p == TheATimeUncompressParameter )
      return &p_searchData.timeUncompress;
   if ( p == TheATimeDecodeParameter )
      return &p_searchData.timeDecode;
   if ( p == TheAIsValidParameter )
      return &o_isValid;
   if ( p == TheAOutputDataReleaseParameter )
      return &o_dataRelease;
   if ( p == TheADatabaseFilePathParameter )
      return o_databaseFilePaths[tableRow].Begin();
   if ( p == TheADatabaseMagnitudeLowParameter )
      return &o_databaseMagnitudeLow;
   if ( p == TheADatabaseMagnitudeHighParameter )
      return &o_databaseMagnitudeHigh;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool APASSInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheACommandParameter )
   {
      p_command.Clear();
      if ( sizeOrLength > 0 )
         p_command.SetLength( sizeOrLength );
   }
   else if ( p == TheAOutputFilePathParameter )
   {
      p_outputFilePath.Clear();
      if ( sizeOrLength > 0 )
         p_outputFilePath.SetLength( sizeOrLength );
   }
   else if ( p == TheASourcesParameter )
   {
      p_searchData.stars.Clear();
      if ( sizeOrLength > 0 )
         p_searchData.stars.Add( APASSStarData(), sizeOrLength );
   }
   else if ( p == TheADatabaseFilePathsParameter )
   {
      o_databaseFilePaths.Clear();
      if ( sizeOrLength > 0 )
         o_databaseFilePaths.Add( String(), sizeOrLength );
   }
   else if ( p == TheADatabaseFilePathParameter )
   {
      o_databaseFilePaths[tableRow].Clear();
      if ( sizeOrLength > 0 )
         o_databaseFilePaths[tableRow].SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type APASSInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheACommandParameter )
      return p_command.Length();
   if ( p == TheAOutputFilePathParameter )
      return p_outputFilePath.Length();
   if ( p == TheASourcesParameter )
      return p_searchData.stars.Length();
   if ( p == TheADatabaseFilePathsParameter )
      return o_databaseFilePaths.Length();
   if ( p == TheADatabaseFilePathParameter )
      return o_databaseFilePaths[tableRow].Length();

   return 0;
}

// ----------------------------------------------------------------------------

int APASSInstance::OutputDataRelease() const
{
   switch ( p_dataRelease )
   {
   default:
   case ADataRelease::DR9:
   case ADataRelease::DR10:
      return p_dataRelease;
   case ADataRelease::BestAvailable:
      if ( TheAPASSProcess->HasDatabaseFiles( ADataRelease::DR10 ) )
         return ADataRelease::DR10;
      if ( TheAPASSProcess->HasDatabaseFiles( ADataRelease::DR9 ) )
         return ADataRelease::DR9;
      return ADataRelease::DR10;
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF APASSInstance.cpp - Released 2021-04-09T19:41:48Z
