//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Gaia Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// GaiaInstance.cpp - Released 2021-04-09T19:41:48Z
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

#include "GaiaInstance.h"
#include "GaiaParameters.h"
#include "GaiaProcess.h"

#include <pcl/Console.h>
#include <pcl/ElapsedTime.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

GaiaInstance::GaiaInstance( const MetaProcess* P )
   : ProcessImplementation( P )
   , p_command( TheGCommandParameter->DefaultValue() )
   , p_verbosity( int32( TheGVerbosityParameter->DefaultValue() ) )
   , p_dataRelease( GDataRelease::Default )
   , p_sortBy( GSortBy::Default )
   , p_generateTextOutput( TheGGenerateTextOutputParameter->DefaultValue() )
   , p_textFormat( GTextFormat::Default )
   , p_textHeaders( GTextHeaders::Default )
{
}

// ----------------------------------------------------------------------------

GaiaInstance::GaiaInstance( const GaiaInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void GaiaInstance::Assign( const ProcessImplementation& p )
{
   const GaiaInstance* x = dynamic_cast<const GaiaInstance*>( &p );
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

bool GaiaInstance::CanExecuteOn( const View&, String& whyNot ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool GaiaInstance::CanExecuteGlobal( String& whyNot ) const
{
   whyNot = "Gaia can only be executed in the global context.";
   return true;
}

// ----------------------------------------------------------------------------

bool GaiaInstance::ExecuteGlobal()
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
   TheGaiaProcess->EnsureDatabasesInitialized( o_dataRelease, p_verbosity );

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

class GaiaSearchThread : public Thread
{
public:

   GaiaSearchData data;

   GaiaSearchThread( const GaiaSearchData& searchData, pcl_enum dataRelease, int beginIndex, int endIndex )
      : data( searchData )
      , m_dataRelease( dataRelease )
      , m_beginIndex( beginIndex )
      , m_endIndex( endIndex )
   {
   }

   void Run() override
   {
      for ( int i = m_beginIndex; i < m_endIndex; ++i )
         TheGaiaProcess->Databases( m_dataRelease )[i].Search( data );
   }

private:

   pcl_enum m_dataRelease;
   int      m_beginIndex, m_endIndex;
};

void GaiaInstance::Search()
{
   GetInfo();
   if ( !o_isValid )
      throw Error( "No database files have been selected for Gaia " + GDataRelease::ReleaseName( o_dataRelease ) );

   switch ( p_verbosity )
   {
   default:
   case 0:
      break;
   case 1:
      Console().WriteLn( "<end><cbr>Executing Gaia "
         + GDataRelease::ReleaseName( o_dataRelease ) + " search command..." );
      break;
   case 2:
      Console().WriteLn( "<end><cbr>Executing Gaia "
         + GDataRelease::ReleaseName( o_dataRelease ) + " search command:"
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

   const ReferenceArray<GaiaDatabaseFile>& databases = TheGaiaProcess->Databases( o_dataRelease );

   /*
    * Find the set of databases necessary for the specified magnitude range.
    */
   int firstDatabaseIndex = 0;
   for ( const GaiaDatabaseFile& database : databases )
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
   for ( const GaiaDatabaseFile& database : databases )
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

      ReferenceArray<GaiaSearchThread> threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += L[i++] )
         threads.Add( new GaiaSearchThread( p_searchData, o_dataRelease,
                                               firstDatabaseIndex + n,
                                               firstDatabaseIndex + n + L[i] ) );
      int i = 0;
      for ( GaiaSearchThread& thread : threads )
         thread.Start( ThreadPriority::DefaultMax, i++ );
      for ( GaiaSearchThread& thread : threads )
         thread.Wait();

      for ( GaiaSearchThread& thread : threads )
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
   case GSortBy::DontSort:
      break;
   case GSortBy::RA:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.ra < s2.ra; } );
      break;
   case GSortBy::Dec:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.dec < s2.dec; } );
      break;
   default:
   case GSortBy::G:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.magG < s2.magG; } );
      break;
   case GSortBy::G_BP:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.magBP < s2.magBP; } );
      break;
   case GSortBy::G_RP:
      p_searchData.stars.Sort( []( const auto& s1, const auto& s2 ) { return s1.magRP < s2.magRP; } );
      break;
   case GSortBy::Parallax:
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

void GaiaInstance::GenerateTextOutput() const
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
   case GTextFormat::CSV:
      if ( p_textHeaders == GTextHeaders::SearchParameters
        || p_textHeaders == GTextHeaders::SearchParametersAndTableColumns )
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
      if ( p_textHeaders == GTextHeaders::TableColumns
        || p_textHeaders == GTextHeaders::SearchParametersAndTableColumns )
      {
         f.OutTextLn( "ra,dec,parallax,pmra,pmdec,phot_g_mean_mag,phot_bp_mean_mag,phot_rp_mean_mag,flags" );
      }
      for ( const GaiaStarData& star : p_searchData.stars )
      {
         IsoString sa = IsoString().Format( "%.9f", star.ra );
         IsoString sd = IsoString().Format( "%.9f", star.dec );

         IsoString smotion = (star.flags & GaiaStarFlag::NoPM) ?
                                 IsoString( ",," )
                              : IsoString().Format( "%.4f,%.4f,%.4f", star.parx, star.pmra, star.pmdec );

         IsoString smagG = IsoString().Format( "%.3f", star.magG );
         IsoString smagBP = (star.flags & GaiaStarFlag::NoGBPMag) ?
                                 IsoString()
                              : IsoString().Format( "%.3f", star.magBP );
         IsoString smagRP = (star.flags & GaiaStarFlag::NoGRPMag) ?
                                 IsoString()
                              : IsoString().Format( "%.3f", star.magRP );

         IsoString sflags = IsoString().Format( "%x", star.flags );

         f.OutTextLn( sa + ',' + sd + ',' + smotion + ',' + smagG + ',' + smagBP + ',' + smagRP + ',' + sflags );
      }
      break;

   case GTextFormat::Tabular:
      if ( p_textHeaders == GTextHeaders::SearchParameters
        || p_textHeaders == GTextHeaders::SearchParametersAndTableColumns )
      {
         f.OutTextLn( "Gaia " + GDataRelease::ReleaseName( o_dataRelease ) + " Search"
           + IsoString().Format( "\n================"
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
      if ( p_textHeaders == GTextHeaders::TableColumns
        || p_textHeaders == GTextHeaders::SearchParametersAndTableColumns )
      {
#ifdef _MSC_VER
         f.OutTextLn( "      \xce\xb1             \xce\xb4            \xcf\x96         \xce\xbc\xce\xb1*        \xce\xbc\xce\xb4       G     G_BP   G_RP   Flags" );
         f.OutTextLn( "------------- ------------- ---------- ---------- ---------- ------ ------ ------ --------" );
         f.OutTextLn( "   \xc2\xb0             \xc2\xb0               mas       mas/yr     mas/yr   mag    mag    mag" );
         f.OutTextLn( "============= ============= ========== ========== ========== ====== ====== ====== ========" );
#else
         f.OutTextLn( String( u"      α             δ            ϖ         μα*        μδ       G     G_BP   G_RP   Flags" ).ToUTF8() );
         f.OutTextLn(          "------------- ------------- ---------- ---------- ---------- ------ ------ ------ --------" );
         f.OutTextLn( String( u"   °             °               mas       mas/yr     mas/yr   mag    mag    mag" ).ToUTF8() );
         f.OutTextLn(          "============= ============= ========== ========== ========== ====== ====== ====== ========" );
#endif
      }
      for ( const GaiaStarData& star : p_searchData.stars )
      {
         IsoString sa = IsoString().Format( "%13.9f", star.ra );
         IsoString sd = IsoString().Format( "%+13.9f", star.dec );

         IsoString smotion = (star.flags & GaiaStarFlag::NoPM) ?
                                 IsoString( ' ', 32 )
                              : IsoString().Format( "%10.4f %+10.4f %+10.4f", star.parx, star.pmra, star.pmdec );

         IsoString smagG = IsoString().Format( "%6.3f", star.magG );
         IsoString smagBP = (star.flags & GaiaStarFlag::NoGBPMag) ?
                                 IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.magBP );
         IsoString smagRP = (star.flags & GaiaStarFlag::NoGRPMag) ?
                                 IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.magRP );

         IsoString sflags = IsoString().Format( "%08x", star.flags );

         f.OutTextLn( sa + ' ' + sd + ' ' + smotion + ' ' + smagG + ' ' + smagBP + ' ' + smagRP + ' ' + sflags );
      }
      break;

   case GTextFormat::TabularCompound:
      if ( p_textHeaders == GTextHeaders::SearchParameters
        || p_textHeaders == GTextHeaders::SearchParametersAndTableColumns )
      {
         IsoString sa = IsoString::ToSexagesimal( p_searchData.centerRA/15, RAConversionOptions( 4/*precision*/, 3/*width*/ ) );
         IsoString sd = IsoString::ToSexagesimal( p_searchData.centerDec, DecConversionOptions( 3/*precision*/ ) );
         IsoString sr = IsoString::ToSexagesimal( p_searchData.radius, AngleConversionOptions( 3/*precision*/ ) );
         f.OutTextLn(            "Gaia " + GDataRelease::ReleaseName( o_dataRelease ) + " Search"
                                 "\n================"
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
      if ( p_textHeaders == GTextHeaders::TableColumns
        || p_textHeaders == GTextHeaders::SearchParametersAndTableColumns )
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
      for ( const GaiaStarData& star : p_searchData.stars )
      {
         IsoString sa = IsoString::ToSexagesimal( star.ra/15, RAConversionOptions( 6/*precision*/ ) );
         IsoString sd = IsoString::ToSexagesimal( star.dec, DecConversionOptions( 5/*precision*/ ) );

         IsoString smotion = (star.flags & GaiaStarFlag::NoPM) ?
                                 IsoString( ' ', 32 )
                              : IsoString().Format( "%10.4f %+10.4f %+10.4f", star.parx, star.pmra, star.pmdec );

         IsoString smagG = IsoString().Format( "%6.3f", star.magG );
         IsoString smagBP = (star.flags & GaiaStarFlag::NoGBPMag) ?
                                 IsoString( ' ', 6 )
                              : IsoString().Format( "%6.3f", star.magBP );
         IsoString smagRP = (star.flags & GaiaStarFlag::NoGRPMag) ?
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

void GaiaInstance::GetInfo()
{
   o_isValid = TheGaiaProcess->IsValid( o_dataRelease );
   o_databaseFilePaths = TheGaiaProcess->DatabaseFilePaths( o_dataRelease );
   o_databaseMagnitudeLow = TheGaiaProcess->MagnitudeLow( o_dataRelease );
   o_databaseMagnitudeHigh = TheGaiaProcess->MagnitudeHigh( o_dataRelease );
}

// ----------------------------------------------------------------------------

void* GaiaInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheGCommandParameter )
      return p_command.Begin();
   if ( p == TheGVerbosityParameter )
      return &p_verbosity;
   if ( p == TheGDataReleaseParameter )
      return &p_dataRelease;
   if ( p == TheGSortByParameter )
      return &p_sortBy;
   if ( p == TheGGenerateTextOutputParameter )
      return &p_generateTextOutput;
   if ( p == TheGTextFormatParameter )
      return &p_textFormat;
   if ( p == TheGTextHeadersParameter )
      return &p_textHeaders;
   if ( p == TheGOutputFilePathParameter )
      return p_outputFilePath.Begin();
   if ( p == TheGCenterRAParameter )
      return &p_searchData.centerRA;
   if ( p == TheGCenterDecParameter )
      return &p_searchData.centerDec;
   if ( p == TheGRadiusParameter )
      return &p_searchData.radius;
   if ( p == TheGMagnitudeLowParameter )
      return &p_searchData.magnitudeLow;
   if ( p == TheGMagnitudeHighParameter )
      return &p_searchData.magnitudeHigh;
   if ( p == TheGSourceLimitParameter )
      return &p_searchData.sourceLimit;
   if ( p == TheGRequiredFlagsParameter )
      return &p_searchData.requiredFlags;
   if ( p == TheGInclusionFlagsParameter )
      return &p_searchData.inclusionFlags;
   if ( p == TheGExclusionFlagsParameter )
      return &p_searchData.exclusionFlags;

   if ( p == TheGSourceRAParameter )
      return &p_searchData.stars[tableRow].ra;
   if ( p == TheGSourceDecParameter )
      return &p_searchData.stars[tableRow].dec;
   if ( p == TheGSourceParxParameter )
      return &p_searchData.stars[tableRow].parx;
   if ( p == TheGSourcePMRAParameter )
      return &p_searchData.stars[tableRow].pmra;
   if ( p == TheGSourcePMDecParameter )
      return &p_searchData.stars[tableRow].pmdec;
   if ( p == TheGSourceMagGParameter )
      return &p_searchData.stars[tableRow].magG;
   if ( p == TheGSourceMagBPParameter )
      return &p_searchData.stars[tableRow].magBP;
   if ( p == TheGSourceMagRPParameter )
      return &p_searchData.stars[tableRow].magRP;
   if ( p == TheGSourceFlagsParameter )
      return &p_searchData.stars[tableRow].flags;
   if ( p == TheGExcessCountParameter )
      return &p_searchData.excessCount;
   if ( p == TheGRejectCountParameter )
      return &p_searchData.rejectCount;
   if ( p == TheGTimeTotalParameter )
      return &p_searchData.timeTotal;
   if ( p == TheGTimeIOParameter )
      return &p_searchData.timeIO;
   if ( p == TheGCountIOParameter )
      return &p_searchData.countIO;
   if ( p == TheGTimeUncompressParameter )
      return &p_searchData.timeUncompress;
   if ( p == TheGTimeDecodeParameter )
      return &p_searchData.timeDecode;
   if ( p == TheGIsValidParameter )
      return &o_isValid;
   if ( p == TheGOutputDataReleaseParameter )
      return &o_dataRelease;
   if ( p == TheGDatabaseFilePathParameter )
      return o_databaseFilePaths[tableRow].Begin();
   if ( p == TheGDatabaseMagnitudeLowParameter )
      return &o_databaseMagnitudeLow;
   if ( p == TheGDatabaseMagnitudeHighParameter )
      return &o_databaseMagnitudeHigh;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool GaiaInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheGCommandParameter )
   {
      p_command.Clear();
      if ( sizeOrLength > 0 )
         p_command.SetLength( sizeOrLength );
   }
   else if ( p == TheGOutputFilePathParameter )
   {
      p_outputFilePath.Clear();
      if ( sizeOrLength > 0 )
         p_outputFilePath.SetLength( sizeOrLength );
   }
   else if ( p == TheGSourcesParameter )
   {
      p_searchData.stars.Clear();
      if ( sizeOrLength > 0 )
         p_searchData.stars.Add( GaiaStarData(), sizeOrLength );
   }
   else if ( p == TheGDatabaseFilePathsParameter )
   {
      o_databaseFilePaths.Clear();
      if ( sizeOrLength > 0 )
         o_databaseFilePaths.Add( String(), sizeOrLength );
   }
   else if ( p == TheGDatabaseFilePathParameter )
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

size_type GaiaInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheGCommandParameter )
      return p_command.Length();
   if ( p == TheGOutputFilePathParameter )
      return p_outputFilePath.Length();
   if ( p == TheGSourcesParameter )
      return p_searchData.stars.Length();
   if ( p == TheGDatabaseFilePathsParameter )
      return o_databaseFilePaths.Length();
   if ( p == TheGDatabaseFilePathParameter )
      return o_databaseFilePaths[tableRow].Length();

   return 0;
}

// ----------------------------------------------------------------------------

int GaiaInstance::OutputDataRelease() const
{
   switch ( p_dataRelease )
   {
   case GDataRelease::DR2:
   default:
   case GDataRelease::EDR3:
      return p_dataRelease;
   case GDataRelease::BestAvailable:
      if ( TheGaiaProcess->HasDatabaseFiles( GDataRelease::EDR3 ) )
         return GDataRelease::EDR3;
      if ( TheGaiaProcess->HasDatabaseFiles( GDataRelease::DR2 ) )
         return GDataRelease::DR2;
      return GDataRelease::EDR3;
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GaiaInstance.cpp - Released 2021-04-09T19:41:48Z
