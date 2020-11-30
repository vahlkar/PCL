//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.3
// ----------------------------------------------------------------------------
// Standard GaiaDR2 Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// GaiaDR2Instance.cpp - Released 2020-11-27T11:02:59Z
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

#include "GaiaDR2Instance.h"
#include "GaiaDR2Parameters.h"
#include "GaiaDR2Process.h"

#include <pcl/Console.h>
#include <pcl/ElapsedTime.h>
#include <pcl/StandardStatus.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

GaiaDR2Instance::GaiaDR2Instance( const MetaProcess* P )
   : ProcessImplementation( P )
   , p_command( TheGDR2CommandParameter->DefaultValue() )
   , p_verbosity( int32( TheGDR2VerbosityParameter->DefaultValue() ) )
   , p_sortBy( GDR2SortBy::Default )
   , p_generateTextOutput( TheGDR2GenerateTextOutputParameter->DefaultValue() )
   , p_textFormat( GDR2TextFormat::Default )
   , p_textHeaders( GDR2TextHeaders::Default )
{
}

// ----------------------------------------------------------------------------

GaiaDR2Instance::GaiaDR2Instance( const GaiaDR2Instance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void GaiaDR2Instance::Assign( const ProcessImplementation& p )
{
   const GaiaDR2Instance* x = dynamic_cast<const GaiaDR2Instance*>( &p );
   if ( x != nullptr )
   {
      p_command = x->p_command;
      p_verbosity = x->p_verbosity;
      p_sortBy = x->p_sortBy;
      p_generateTextOutput = x->p_generateTextOutput;
      p_textFormat = x->p_textFormat;
      p_textHeaders = x->p_textHeaders;
      p_outputFilePath = x->p_outputFilePath;
      p_searchData = x->p_searchData;
      o_databaseFilePaths = x->o_databaseFilePaths;
      o_databaseMagnitudeLow = x->o_databaseMagnitudeLow;
      o_databaseMagnitudeHigh = x->o_databaseMagnitudeHigh;
   }
}

// ----------------------------------------------------------------------------

bool GaiaDR2Instance::CanExecuteOn( const View&, String& whyNot ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool GaiaDR2Instance::CanExecuteGlobal( String& whyNot ) const
{
   whyNot = "GaiaDR2 can only be executed in the global context.";
   return true;
}

// ----------------------------------------------------------------------------

class GaiaDR2SearchThread : public Thread
{
public:

   GaiaDR2SearchData data;

   GaiaDR2SearchThread( const GaiaDR2SearchData& searchData, int beginIndex, int endIndex )
      : data( searchData )
      , m_beginIndex( beginIndex )
      , m_endIndex( endIndex )
   {
   }

   void Run() override
   {
      for ( int i = m_beginIndex; i < m_endIndex; ++i )
         TheGaiaDR2Process->Databases()[i].Search( data );
   }

private:

   int m_beginIndex, m_endIndex;
};

bool GaiaDR2Instance::ExecuteGlobal()
{
   /*
    * Clear output data.
    */
   p_searchData.ResetSearchResults();
   o_databaseFilePaths.Clear();
   o_databaseMagnitudeLow = o_databaseMagnitudeHigh = 0;

   /*
    * Install and initialize XPSD files. This will only happen the first time
    * this function is executed, or if preferences have been changed.
    */
   TheGaiaDR2Process->EnsureDatabasesInitialized( p_verbosity );

   if ( !TheGaiaDR2Process->IsValid() )
      return false;

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

void GaiaDR2Instance::Search()
{
   switch ( p_verbosity )
   {
   default:
   case 0:
      break;
   case 1:
      Console().WriteLn( "<end><cbr>Executing Gaia DR2 search command..." );
      break;
   case 2:
      Console().WriteLn( String().Format(
         "<end><cbr>Executing Gaia DR2 search command:\n"
         "Center Right Ascension ..... %13.9f deg\n"
         "Center Declination ......... %+13.9f deg\n"
         "Search radius .............. %13.9f deg\n"
         "Magnitude low limit ........ %6.3f\n"
         "Magnitude high limit ....... %6.3f\n"
         "Source count limit ......... %u\n"
         "Inclusion flags ............ %08x\n"
         "Exclusion flags ............ %08x"
         , p_searchData.centerRA
         , p_searchData.centerDec
         , p_searchData.radius
         , p_searchData.magnitudeLow
         , p_searchData.magnitudeHigh
         , p_searchData.sourceLimit
         , p_searchData.inclusionFlags
         , p_searchData.exclusionFlags ) );
      break;
   }

   /*
    * Find the set of databases necessary for the specified magnitude range.
    */
   int firstDatabaseIndex = 0;
   for ( const GaiaDR2DatabaseFile& database : TheGaiaDR2Process->Databases() )
   {
      if ( database.MagnitudeHigh() >= p_searchData.magnitudeLow )
         break;
      ++firstDatabaseIndex;
   }
   if ( firstDatabaseIndex == int( TheGaiaDR2Process->Databases().Length() ) )
   {
      if ( p_verbosity > 0 )
         Console().WarningLn( "<end><cbr>** Warning: The specified magnitude interval is out of the range supported by installed databases." );
      return;
   }

   int lastDatabaseIndex = firstDatabaseIndex;
   for ( const GaiaDR2DatabaseFile& database : TheGaiaDR2Process->Databases() )
   {
      if ( database.MagnitudeHigh() >= p_searchData.magnitudeHigh )
         break;
      ++lastDatabaseIndex;
   }
   if ( lastDatabaseIndex == int( TheGaiaDR2Process->Databases().Length() ) )
      --lastDatabaseIndex;

   /*
    * Perform a database search.
    */
   Array<size_type> L = Thread::OptimalThreadLoads( lastDatabaseIndex - firstDatabaseIndex + 1 );
   if ( L.Length() == 1 )
   {
      // Single-file search
      TheGaiaDR2Process->Databases()[firstDatabaseIndex].Search( p_searchData );
   }
   else
   {
      // Multiple file parallel search.
      if ( p_verbosity > 1 )
         Console().WriteLn( String().Format( "<br>* Using %u parallel search threads.", L.Length() ) );

      ReferenceArray<GaiaDR2SearchThread> threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += L[i++] )
         threads.Add( new GaiaDR2SearchThread( p_searchData,
                                               firstDatabaseIndex + n,
                                               firstDatabaseIndex + n + L[i] ) );
      int i = 0;
      for ( GaiaDR2SearchThread& thread : threads )
         thread.Start( ThreadPriority::DefaultMax, i++ );
      for ( GaiaDR2SearchThread& thread : threads )
         thread.Wait();

      for ( GaiaDR2SearchThread& thread : threads )
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
   case GDR2SortBy::DontSort:
      break;
   case GDR2SortBy::RA:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.ra < s2.ra; } );
      break;
   case GDR2SortBy::Dec:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.dec < s2.dec; } );
      break;
   default:
   case GDR2SortBy::G:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.magG < s2.magG; } );
      break;
   case GDR2SortBy::G_BP:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.magBP < s2.magBP; } );
      break;
   case GDR2SortBy::G_RP:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.magRP < s2.magRP; } );
      break;
   case GDR2SortBy::Parallax:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.parx < s2.parx; } );
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
         "<end><cbr><br>Search results:\n"
         "Total sources .............. " + String( p_searchData.stars.Length() ) + "\n"
         "Excess count ............... " + String( p_searchData.excessCount ) + "\n"
         "Reject count ............... " + String( p_searchData.rejectCount ) + "\n"
         "Total search time .......... " + ElapsedTime::ToString( p_searchData.timeTotal, 2 ) + "\n"
         "Total I/O time ............. " + ElapsedTime::ToString( p_searchData.timeIO, 2 ) + "\n"
         "Total I/O operations ....... " + String( p_searchData.countIO ) + "\n"
         "Total uncompression time ... " + ElapsedTime::ToString( p_searchData.timeUncompress, 2 ) + "\n"
         "Total decoding time ........ " + ElapsedTime::ToString( p_searchData.timeDecode, 2 ) );
      break;
   }
}

// ----------------------------------------------------------------------------

void GaiaDR2Instance::GenerateTextOutput() const
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
   case GDR2TextFormat::CSV:
      if ( p_textHeaders == GDR2TextHeaders::SearchParameters
        || p_textHeaders == GDR2TextHeaders::SearchParametersAndTableColumns )
      {
         f.OutTextLn( "center_ra,center_dec,radius,magnitude_low,magnitude_high,source_count_limit,inclusion_flags,exclusion_flags" );
         f.OutTextLn( IsoString().Format( "%.9f,%.9f,%.9f,%.3f,%.3f,%u,%x,%x"
                                 , p_searchData.centerRA
                                 , p_searchData.centerDec
                                 , p_searchData.radius
                                 , p_searchData.magnitudeLow
                                 , p_searchData.magnitudeHigh
                                 , p_searchData.sourceLimit
                                 , p_searchData.inclusionFlags
                                 , p_searchData.exclusionFlags ) );
      }
      if ( p_textHeaders == GDR2TextHeaders::TableColumns
        || p_textHeaders == GDR2TextHeaders::SearchParametersAndTableColumns )
      {
         f.OutTextLn( "ra,dec,parallax,pmra,pmdec,phot_g_mean_mag,phot_bp_mean_mag,phot_rp_mean_mag,flags" );
      }
      for ( const GaiaDR2StarData& star : p_searchData.stars )
      {
         IsoString sa = IsoString().Format( "%.9f", star.ra );
         IsoString sd = IsoString().Format( "%.9f", star.dec );

         IsoString smotion = (star.flags & GaiaDR2StarFlag::NoPM) ?
                                 IsoString( ",," )
                              : IsoString().Format( "%.4f,%.4f,%.4f", star.parx, star.pmra, star.pmdec );

         IsoString smagG = IsoString().Format( "%.3f", star.magG );
         IsoString smagBP = (star.flags & GaiaDR2StarFlag::NoGBPMag) ?
                                 IsoString()
                              : IsoString().Format( "%.3f", star.magBP );
         IsoString smagRP = (star.flags & GaiaDR2StarFlag::NoGRPMag) ?
                                 IsoString()
                              : IsoString().Format( "%.3f", star.magRP );

         IsoString sflags = IsoString().Format( "%x", star.flags );

         f.OutTextLn( sa + ',' + sd + ',' + smotion + ',' + smagG + ',' + smagBP + ',' + smagRP + ',' + sflags );
      }
      break;

   case GDR2TextFormat::Tabular:
      if ( p_textHeaders == GDR2TextHeaders::SearchParameters
        || p_textHeaders == GDR2TextHeaders::SearchParametersAndTableColumns )
      {
         f.OutTextLn( IsoString().Format(
                                 "Gaia DR2 Search\n"
                                 "===============\n"
                                 "Total sources ............ %u\n"
                                 "Center Right Ascension ... %13.9f deg\n"
                                 "Center Declination ....... %+13.9f deg\n"
                                 "Search radius ............ %13.9f deg\n"
                                 "Magnitude low limit ...... %6.3f\n"
                                 "Magnitude high limit ..... %6.3f\n"
                                 "Source count limit ....... %u\n"
                                 "Inclusion flags .......... %08x\n"
                                 "Exclusion flags .......... %08x\n\n"
                                 , p_searchData.stars.Length()
                                 , p_searchData.centerRA
                                 , p_searchData.centerDec
                                 , p_searchData.radius
                                 , p_searchData.magnitudeLow
                                 , p_searchData.magnitudeHigh
                                 , p_searchData.sourceLimit
                                 , p_searchData.inclusionFlags
                                 , p_searchData.exclusionFlags ) );
      }
      if ( p_textHeaders == GDR2TextHeaders::TableColumns
        || p_textHeaders == GDR2TextHeaders::SearchParametersAndTableColumns )
      {
         f.OutTextLn( String( u"      α             δ            ϖ         μα*        μδ       G     G_BP   G_RP   Flags" ).ToUTF8() );
         f.OutTextLn(          "------------- ------------- ---------- ---------- ---------- ------ ------ ------ --------" );
         f.OutTextLn( String( u"   °             °               mas       mas/yr     mas/yr   mag    mag    mag" ).ToUTF8() );
         f.OutTextLn(          "============= ============= ========== ========== ========== ====== ====== ====== ========" );
      }
      for ( const GaiaDR2StarData& star : p_searchData.stars )
      {
         IsoString sa = IsoString().Format( "%13.9f", star.ra );
         IsoString sd = IsoString().Format( "%+13.9f", star.dec );

         IsoString smotion = (star.flags & GaiaDR2StarFlag::NoPM) ?
                                 IsoString( ' ', 32 )
                              : IsoString().Format( "%10.4f %+10.4f %+10.4f", star.parx, star.pmra, star.pmdec );

         IsoString smagG = IsoString().Format( "%6.3f", star.magG );
         IsoString smagBP = (star.flags & GaiaDR2StarFlag::NoGBPMag) ?
                                 IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.magBP );
         IsoString smagRP = (star.flags & GaiaDR2StarFlag::NoGRPMag) ?
                                 IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.magRP );

         IsoString sflags = IsoString().Format( "%08x", star.flags );

         f.OutTextLn( sa + ' ' + sd + ' ' + smotion + ' ' + smagG + ' ' + smagBP + ' ' + smagRP + ' ' + sflags );
      }
      break;

   case GDR2TextFormat::TabularCompound:
      if ( p_textHeaders == GDR2TextHeaders::SearchParameters
        || p_textHeaders == GDR2TextHeaders::SearchParametersAndTableColumns )
      {
         IsoString sa = IsoString::ToSexagesimal( p_searchData.centerRA/15, RAConversionOptions( 4/*precision*/, 3/*width*/ ) );
         IsoString sd = IsoString::ToSexagesimal( p_searchData.centerDec, DecConversionOptions( 3/*precision*/ ) );
         IsoString sr = IsoString::ToSexagesimal( p_searchData.radius, AngleConversionOptions( 3/*precision*/ ) );
         f.OutTextLn(            "Gaia DR2 Search\n"
                                 "===============\n"
                                 "Total sources ............ " + IsoString( p_searchData.stars.Length() ) + '\n' +
                                 "Center Right Ascension ... " + sa + " (hms)\n" +
#ifdef _MSC_VER
                                 "Center Declination ....... " + sd + "  (\xc2\xb0\xe2\x80\xb2\xe2\x80\xb3)\n" +
                                 "Search radius ............ " + sr + "  (\xc2\xb0\xe2\x80\xb2\xe2\x80\xb3)\n" +
#else
                                 "Center Declination ....... " + sd + String( u"  (°′″)\n" ).ToUTF8() +
                                 "Search radius ............ " + sr + String( u"  (°′″)\n" ).ToUTF8() +
#endif
                        IsoString().Format(
                                 "Magnitude low limit ...... %6.3f\n"
                                 "Magnitude high limit ..... %6.3f\n"
                                 "Source count limit ....... %u\n"
                                 "Inclusion flags .......... %08x\n"
                                 "Exclusion flags .......... %08x\n"
                                 , p_searchData.magnitudeLow
                                 , p_searchData.magnitudeHigh
                                 , p_searchData.sourceLimit
                                 , p_searchData.inclusionFlags
                                 , p_searchData.exclusionFlags ) );
      }
      if ( p_textHeaders == GDR2TextHeaders::TableColumns
        || p_textHeaders == GDR2TextHeaders::SearchParametersAndTableColumns )
      {
#ifdef _MSC_VER
         f.OutTextLn( "       \xce\xb1               \xce\xb4             \xcf\x96         \xce\xbc\xce\xb1*        \xce\xbc\xce\xb4       G     G_BP   G_RP   Flags" );
         f.OutTextLn( "--------------- --------------- ---------- ---------- ---------- ------ ------ ------ --------" );
         f.OutTextLn( "  h  m  s          \xc2\xb0  \xe2\x80\xb2  \xe2\x80\xb3           mas       mas/yr     mas/yr   mag    mag    mag" );
         f.OutTextLn(          "=============== =============== ========== ========== ========== ====== ====== ====== ========" );
#else
         f.OutTextLn( String( u"       α               δ             ϖ         μα*        μδ       G     G_BP   G_RP   Flags" ).ToUTF8() );
         f.OutTextLn(          "--------------- --------------- ---------- ---------- ---------- ------ ------ ------ --------" );
         f.OutTextLn( String( u"  h  m  s          °  ′  ″           mas       mas/yr     mas/yr   mag    mag    mag" ).ToUTF8() );
         f.OutTextLn(          "=============== =============== ========== ========== ========== ====== ====== ====== ========" );
#endif
      }
      for ( const GaiaDR2StarData& star : p_searchData.stars )
      {
         IsoString sa = IsoString::ToSexagesimal( star.ra/15, RAConversionOptions( 6/*precision*/ ) );
         IsoString sd = IsoString::ToSexagesimal( star.dec, DecConversionOptions( 5/*precision*/ ) );

         IsoString smotion = (star.flags & GaiaDR2StarFlag::NoPM) ?
                                 IsoString( ' ', 32 )
                              : IsoString().Format( "%10.4f %+10.4f %+10.4f", star.parx, star.pmra, star.pmdec );

         IsoString smagG = IsoString().Format( "%6.3f", star.magG );
         IsoString smagBP = (star.flags & GaiaDR2StarFlag::NoGBPMag) ?
                                 IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.magBP );
         IsoString smagRP = (star.flags & GaiaDR2StarFlag::NoGRPMag) ?
                                 IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.magRP );

         IsoString sflags = IsoString().Format( "%08x", star.flags );

         f.OutTextLn( sa + ' ' + sd + ' ' + smotion + ' ' + smagG + ' ' + smagBP + ' ' + smagRP + ' ' + sflags );
      }
      break;
   }

   f.Flush();

   if ( p_verbosity > 0 )
      Console().WriteLn( "<end><cbr><br>* Output text file generated: " + p_outputFilePath );
}

// ----------------------------------------------------------------------------

void GaiaDR2Instance::GetInfo()
{
   o_databaseFilePaths = TheGaiaDR2Process->DatabaseFilePaths();
   o_databaseMagnitudeLow = TheGaiaDR2Process->MagnitudeLow();
   o_databaseMagnitudeHigh = TheGaiaDR2Process->MagnitudeHigh();
}

// ----------------------------------------------------------------------------

void* GaiaDR2Instance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheGDR2CommandParameter )
      return p_command.Begin();
   if ( p == TheGDR2VerbosityParameter )
      return &p_verbosity;
   if ( p == TheGDR2SortByParameter )
      return &p_sortBy;
   if ( p == TheGDR2GenerateTextOutputParameter )
      return &p_generateTextOutput;
   if ( p == TheGDR2TextFormatParameter )
      return &p_textFormat;
   if ( p == TheGDR2TextHeadersParameter )
      return &p_textHeaders;
   if ( p == TheGDR2OutputFilePathParameter )
      return p_outputFilePath.Begin();
   if ( p == TheGDR2CenterRAParameter )
      return &p_searchData.centerRA;
   if ( p == TheGDR2CenterDecParameter )
      return &p_searchData.centerDec;
   if ( p == TheGDR2RadiusParameter )
      return &p_searchData.radius;
   if ( p == TheGDR2MagnitudeLowParameter )
      return &p_searchData.magnitudeLow;
   if ( p == TheGDR2MagnitudeHighParameter )
      return &p_searchData.magnitudeHigh;
   if ( p == TheGDR2SourceLimitParameter )
      return &p_searchData.sourceLimit;
   if ( p == TheGDR2IncludeFlagsParameter )
      return &p_searchData.inclusionFlags;
   if ( p == TheGDR2ExcludeFlagsParameter )
      return &p_searchData.exclusionFlags;

   if ( p == TheGDR2SourceRAParameter )
      return &p_searchData.stars[tableRow].ra;
   if ( p == TheGDR2SourceDecParameter )
      return &p_searchData.stars[tableRow].dec;
   if ( p == TheGDR2SourceParxParameter )
      return &p_searchData.stars[tableRow].parx;
   if ( p == TheGDR2SourcePMRAParameter )
      return &p_searchData.stars[tableRow].pmra;
   if ( p == TheGDR2SourcePMDecParameter )
      return &p_searchData.stars[tableRow].pmdec;
   if ( p == TheGDR2SourceMagGParameter )
      return &p_searchData.stars[tableRow].magG;
   if ( p == TheGDR2SourceMagBPParameter )
      return &p_searchData.stars[tableRow].magBP;
   if ( p == TheGDR2SourceMagRPParameter )
      return &p_searchData.stars[tableRow].magRP;
   if ( p == TheGDR2SourceFlagsParameter )
      return &p_searchData.stars[tableRow].flags;
   if ( p == TheGDR2ExcessCountParameter )
      return &p_searchData.excessCount;
   if ( p == TheGDR2RejectCountParameter )
      return &p_searchData.rejectCount;
   if ( p == TheGDR2TimeTotalParameter )
      return &p_searchData.timeTotal;
   if ( p == TheGDR2TimeIOParameter )
      return &p_searchData.timeIO;
   if ( p == TheGDR2CountIOParameter )
      return &p_searchData.countIO;
   if ( p == TheGDR2TimeUncompressParameter )
      return &p_searchData.timeUncompress;
   if ( p == TheGDR2TimeDecodeParameter )
      return &p_searchData.timeDecode;
   if ( p == TheGDR2DatabaseFilePathParameter )
      return o_databaseFilePaths[tableRow].Begin();
   if ( p == TheGDR2DatabaseMagnitudeLowParameter )
      return &o_databaseMagnitudeLow;
   if ( p == TheGDR2DatabaseMagnitudeHighParameter )
      return &o_databaseMagnitudeHigh;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool GaiaDR2Instance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheGDR2CommandParameter )
   {
      p_command.Clear();
      if ( sizeOrLength > 0 )
         p_command.SetLength( sizeOrLength );
   }
   else if ( p == TheGDR2OutputFilePathParameter )
   {
      p_outputFilePath.Clear();
      if ( sizeOrLength > 0 )
         p_outputFilePath.SetLength( sizeOrLength );
   }
   else if ( p == TheGDR2SourcesParameter )
   {
      p_searchData.stars.Clear();
      if ( sizeOrLength > 0 )
         p_searchData.stars.Add( GaiaDR2StarData(), sizeOrLength );
   }
   else if ( p == TheGDR2DatabaseFilePathsParameter )
   {
      o_databaseFilePaths.Clear();
      if ( sizeOrLength > 0 )
         o_databaseFilePaths.Add( String(), sizeOrLength );
   }
   else if ( p == TheGDR2DatabaseFilePathParameter )
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

size_type GaiaDR2Instance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheGDR2CommandParameter )
      return p_command.Length();
   if ( p == TheGDR2OutputFilePathParameter )
      return p_outputFilePath.Length();
   if ( p == TheGDR2SourcesParameter )
      return p_searchData.stars.Length();
   if ( p == TheGDR2DatabaseFilePathsParameter )
      return o_databaseFilePaths.Length();
   if ( p == TheGDR2DatabaseFilePathParameter )
      return o_databaseFilePaths[tableRow].Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GaiaDR2Instance.cpp - Released 2020-11-27T11:02:59Z
