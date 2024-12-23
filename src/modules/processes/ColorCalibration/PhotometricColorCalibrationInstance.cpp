//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.5
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationInstance.cpp - Released 2024-12-23T11:33:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "CalibrationUtility.h"
#include "PhotometricColorCalibrationGraphInterface.h"
#include "PhotometricColorCalibrationInstance.h"
#include "PhotometricColorCalibrationParameters.h"

#include "../APASS/APASSInstance.h" // APASSBinaryHeader

#include <pcl/APASSDatabaseFile.h>
#include <pcl/AstrometricMetadata.h>
#include <pcl/AutoStatusCallbackRestorer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ImageWindow.h>
#include <pcl/LinearFit.h>
#include <pcl/MetaModule.h>
#include <pcl/NetworkTransfer.h>
#include <pcl/PSFEstimator.h>
#include <pcl/StandardStatus.h>
#include <pcl/Version.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PhotometricColorCalibrationInstance::PhotometricColorCalibrationInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_applyCalibration( ThePCCApplyCalibrationParameter->DefaultValue() )
   , p_whiteReferenceId( ThePCCWhiteReferenceIdParameter->DefaultValue() )
   , p_whiteReferenceName( ThePCCWhiteReferenceNameParameter->DefaultValue() )
   , p_whiteReferenceSr_JV( ThePCCWhiteReferenceSr_JVParameter->DefaultValue() )
   , p_whiteReferenceJB_JV( ThePCCWhiteReferenceJB_JVParameter->DefaultValue() )
   , p_zeroPointSr_JV( ThePCCZeroPointSr_JVParameter->DefaultValue() )
   , p_zeroPointJB_JV( ThePCCZeroPointJB_JVParameter->DefaultValue() )
   , p_autoCatalog( ThePCCAutoCatalogParameter->DefaultValue() )
   , p_catalogId( ThePCCCatalogIdParameter->DefaultValue() )
   , p_limitMagnitude( ThePCCLimitMagnitudeParameter->DefaultValue() )
   , p_autoLimitMagnitude( ThePCCAutoLimitMagnitudeParameter->DefaultValue() )
   , p_targetSourceCount( ThePCCTargetSourceCountParameter->DefaultValue() )
   , p_serverURL( ThePCCServerURLParameter->DefaultValue() )
   , p_structureLayers( ThePCCStructureLayersParameter->DefaultValue() )
   , p_saturationThreshold( ThePCCSaturationThresholdParameter->DefaultValue() )
   , p_saturationRelative( ThePCCSaturationRelativeParameter->DefaultValue() )
   , p_saturationShrinkFactor( ThePCCSaturationShrinkFactorParameter->DefaultValue() )
   , p_psfNoiseLayers( ThePCCPSFNoiseLayersParameter->DefaultValue() )
   , p_psfHotPixelFilterRadius( ThePCCPSFHotPixelFilterRadiusParameter->DefaultValue() )
   , p_psfNoiseReductionFilterRadius( ThePCCPSFNoiseReductionFilterRadiusParameter->DefaultValue() )
   , p_psfMinStructureSize( ThePCCPSFMinStructureSizeParameter->DefaultValue() )
   , p_psfMinSNR( ThePCCPSFMinSNRParameter->DefaultValue() )
   , p_psfAllowClusteredSources( ThePCCPSFAllowClusteredSourcesParameter->DefaultValue() )
   , p_psfType( PCCPSFType::Default )
   , p_psfGrowth( ThePCCPSFGrowthParameter->DefaultValue() )
   , p_psfMaxStars( ThePCCPSFMaxStarsParameter->DefaultValue() )
   , p_psfSearchTolerance( ThePCCPSFSearchToleranceParameter->DefaultValue() )
   , p_psfChannelSearchTolerance( ThePCCPSFChannelSearchToleranceParameter->DefaultValue() )
   , p_neutralizeBackground( ThePCCNeutralizeBackgroundParameter->DefaultValue() )
   , p_backgroundLow( ThePCCBackgroundLowParameter->DefaultValue() )
   , p_backgroundHigh( ThePCCBackgroundHighParameter->DefaultValue() )
   , p_backgroundUseROI( ThePCCBackgroundUseROIParameter->DefaultValue() )
   , p_generateGraphs( ThePCCGenerateGraphsParameter->DefaultValue() )
   , p_generateStarMaps( ThePCCGenerateStarMapsParameter->DefaultValue() )
   , p_generateTextFiles( ThePCCGenerateTextFilesParameter->DefaultValue() )
   , p_outputDirectory( ThePCCOutputDirectoryParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

PhotometricColorCalibrationInstance::PhotometricColorCalibrationInstance( const PhotometricColorCalibrationInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInstance::Assign( const ProcessImplementation& p )
{
   const PhotometricColorCalibrationInstance* x = dynamic_cast<const PhotometricColorCalibrationInstance*>( &p );
   if ( x != nullptr )
   {
      p_applyCalibration = x->p_applyCalibration;
      p_whiteReferenceId = x->p_whiteReferenceId;
      p_whiteReferenceName = x->p_whiteReferenceName;
      p_whiteReferenceSr_JV = x->p_whiteReferenceSr_JV;
      p_whiteReferenceJB_JV = x->p_whiteReferenceJB_JV;
      p_zeroPointSr_JV = x->p_zeroPointSr_JV;
      p_zeroPointJB_JV = x->p_zeroPointJB_JV;
      p_autoCatalog = x->p_autoCatalog;
      p_catalogId = x->p_catalogId;
      p_limitMagnitude = x->p_limitMagnitude;
      p_autoLimitMagnitude = x->p_autoLimitMagnitude;
      p_targetSourceCount = x->p_targetSourceCount;
      p_serverURL = x->p_serverURL;
      p_structureLayers = x->p_structureLayers;
      p_saturationThreshold = x->p_saturationThreshold;
      p_saturationRelative = x->p_saturationRelative;
      p_saturationShrinkFactor = x->p_saturationShrinkFactor;
      p_psfNoiseLayers = x->p_psfNoiseLayers;
      p_psfHotPixelFilterRadius = x->p_psfHotPixelFilterRadius;
      p_psfNoiseReductionFilterRadius = x->p_psfNoiseReductionFilterRadius;
      p_psfMinStructureSize = x->p_psfMinStructureSize;
      p_psfMinSNR = x->p_psfMinSNR;
      p_psfAllowClusteredSources = x->p_psfAllowClusteredSources;
      p_psfType = x->p_psfType;
      p_psfGrowth = x->p_psfGrowth;
      p_psfMaxStars = x->p_psfMaxStars;
      p_psfSearchTolerance = x->p_psfSearchTolerance;
      p_psfChannelSearchTolerance = x->p_psfChannelSearchTolerance;
      p_neutralizeBackground = x->p_neutralizeBackground;
      p_backgroundReferenceViewId = x->p_backgroundReferenceViewId;

      /*
       * Background sampling range parameters are different since version 3 of
       * thre PCC process. Previously they were literal pixel sample values,
       * since v3 they are expressed in sigma units from the median.
       */
      if ( p.Version() < 3 )
      {
         p_backgroundLow = ThePCCBackgroundLowParameter->DefaultValue();
         p_backgroundHigh = ThePCCBackgroundHighParameter->DefaultValue();
      }
      else
      {
         p_backgroundLow = x->p_backgroundLow;
         p_backgroundHigh = x->p_backgroundHigh;
      }

      p_backgroundUseROI = x->p_backgroundUseROI;
      p_backgroundROI = x->p_backgroundROI;
      p_generateGraphs = x->p_generateGraphs;
      p_generateStarMaps = x->p_generateStarMaps;
      p_generateTextFiles = x->p_generateTextFiles;
      p_outputDirectory = x->p_outputDirectory;
   }
}

// ----------------------------------------------------------------------------

UndoFlags PhotometricColorCalibrationInstance::UndoMode( const View& ) const
{
   return p_applyCalibration ? UndoFlag::ViewProperties | UndoFlag::PixelData
                             : UndoFlag::ViewProperties;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.IsPreview() )
   {
      whyNot = "PhotometricColorCalibration cannot be executed on previews.";
      return false;
   }
   if ( !view.IsColor() )
   {
      whyNot = "PhotometricColorCalibration can only be executed on color images.";
      return false;
   }
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "PhotometricColorCalibration cannot be executed on complex images.";
      return false;
   }
   if ( p_neutralizeBackground )
      if ( p_backgroundUseROI )
         if ( !p_backgroundROI.IsRect() )
         {
            whyNot = "Empty background reference ROI defined.";
            return false;
         }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct APASSSPDatabaseInfo
{
   StringList filePaths;
   int        dataRelease = -1;
   float      magnitudeLow = 0;
   float      magnitudeHigh = 0;
};

int APASSDataReleaseParameterValueFromCatalogId( const String& catId )
{
   if ( catId.CompareIC( "APASSDR9_XPSD" ) == 0 )
      return 1;
   if ( catId.CompareIC( "APASSDR10_XPSD" ) == 0 )
      return 2;
   return 0; /*APASS.DataRelease_BestAvailable*/
}

const char* APASSDataReleaseNameFromParameterValue( int drParam )
{
   switch ( drParam )
   {
   case 1:  return "APASS DR9";
   case 2:  return "APASS DR10";
   default: return "<unknown>";
   }
}

static APASSSPDatabaseInfo DatabaseInfoFromAPASSXPSDServer( int drParam )
{
   String scriptPath = File::UniqueFileName( File::SystemTempDirectory(), 12, "PCC_", ".js" );
   APASSSPDatabaseInfo info;
   try
   {
      File::WriteTextFile( scriptPath, IsoString().Format(
R"JS_SOURCE( var __PJSR_APASS_databaseFilePaths = "";
var __PJSR_APASS_databaseFilePaths = "";
var __PJSR_APASS_outputDataRelease = -1;
var __PJSR_APASS_databaseMagnitudeLow = 0;
var __PJSR_APASS_databaseMagnitudeHigh = 0;
if ( (typeof APASS) != 'undefined' )
{
   let xpsd = new APASS;
   xpsd.command = "get-info";
   xpsd.dataRelease = %d;
   xpsd.verbosity = 2;
   xpsd.executeGlobal();
   __PJSR_APASS_databaseFilePaths = xpsd.databaseFilePaths.join();
   __PJSR_APASS_outputDataRelease = xpsd.outputDataRelease;
   __PJSR_APASS_databaseMagnitudeLow = xpsd.databaseMagnitudeLow;
   __PJSR_APASS_databaseMagnitudeHigh = xpsd.databaseMagnitudeHigh;
} )JS_SOURCE", drParam ) );

      Console().ExecuteScript( scriptPath );

      Variant result = Module->EvaluateScript( "__PJSR_APASS_databaseFilePaths" );
      if ( result.IsValid() )
         result.ToString().Break( info.filePaths, ',' );
      result = Module->EvaluateScript( "__PJSR_APASS_outputDataRelease" );
      if ( result.IsValid() )
         info.dataRelease = result.ToInt();
      result = Module->EvaluateScript( "__PJSR_APASS_databaseMagnitudeLow" );
      if ( result.IsValid() )
         info.magnitudeLow = result.ToFloat();
      result = Module->EvaluateScript( "__PJSR_APASS_databaseMagnitudeHigh" );
      if ( result.IsValid() )
         info.magnitudeHigh = result.ToFloat();
   }
   catch ( ... )
   {
   }

   try
   {
      if ( File::Exists( scriptPath ) )
         File::Remove( scriptPath );
   }
   catch ( ... )
   {
   }

   return info;
}

// ----------------------------------------------------------------------------

static String
BestAvailableAPASSXPSDCatalog()
{
   String scriptPath = File::UniqueFileName( File::SystemTempDirectory(), 12, "PCC_", ".js" );
   String catalogId;
   try
   {
      File::WriteTextFile( scriptPath,
R"JS_SOURCE( var __PJSR_APASS_DR = -1;
if ( (typeof APASS) != 'undefined' )
{
   let xpsd = new APASS;
   xpsd.command = "get-info";
   xpsd.dataRelease = APASS.prototype.DataRelease_BestAvailable;
   xpsd.verbosity = 2;
   xpsd.executeGlobal();
   if ( xpsd.databaseFilePaths.length > 0 )
      switch ( xpsd.outputDataRelease )
      {
      case APASS.prototype.DataRelease_10:
         __PJSR_APASS_DR = 10;
         break;
      case APASS.prototype.DataRelease_9:
         __PJSR_APASS_DR = 9;
         break;
      default:
         break;
      }
} )JS_SOURCE" );

      Console().ExecuteScript( scriptPath );

      Variant result = Module->EvaluateScript( "__PJSR_APASS_DR" );
      if ( result.IsValid() )
         switch ( result.ToInt() )
         {
         case 10:
            catalogId = "APASSDR10_XPSD";
            break;
         case 9:
            catalogId = "APASSDR9_XPSD";
            break;
         default:
            break;
         }
   }
   catch ( ... )
   {
   }

   try
   {
      if ( File::Exists( scriptPath ) )
         File::Remove( scriptPath );
   }
   catch ( ... )
   {
   }

   return catalogId;
}

// ----------------------------------------------------------------------------

static float
APASSAutoLimitMagnitude( const DPoint& centerRD, double radius, const AstrometricMetadata& A, const String& catalogId, int targetCount )
{
   // Empiric formula for 1000 stars at 20 deg of galactic latitude
   float magnitudeHigh = RoundInt( 100*Min( 20.0, Max( 7.0, 14.5*Pow( A.Resolution() * Max( A.Width(), A.Height() ), -0.179 ) ) ) )/100.0;
   if ( catalogId == "APASSDR9" )
      return magnitudeHigh;

   const char* catalogSelector;
   if ( catalogId == "APASSDR10_XPSD" )
      catalogSelector = "DataRelease_10";
   else if ( catalogId == "APASSDR9_XPSD" )
      catalogSelector = "DataRelease_9";
   else
      throw Error( "Internal error: Invalid catalog identifier in APASSAutoLimitMagnitude()" );

   String scriptFilePath = File::UniqueFileName( File::SystemTempDirectory(), 12, "PCC_", ".js" );
   try
   {
      File::WriteTextFile( scriptFilePath, IsoString().Format(
R"JS_SOURCE(var __PJSR_APASS_limitMagnitude = -1;
console.writeln( "<end><cbr><br>* Searching for optimal magnitude limit..." );
let xpsd = ((typeof APASS) != 'undefined') ? (new APASS) : null;
if ( xpsd )
{
   xpsd.command = "get-info";
   xpsd.dataRelease = APASS.prototype.%s;
   xpsd.executeGlobal();
   if ( xpsd.isValid )
   {
      let magHigh = xpsd.databaseMagnitudeHigh;
      let m = %.2f;

      xpsd.command = "search";
      xpsd.centerRA = %.8f;
      xpsd.centerDec = %.8f;
      xpsd.radius = %.8f;
      xpsd.magnitudeLow = -1.5;
      xpsd.sourceLimit = 0; // do not retrieve objects, just count them.
      xpsd.exclusionFlags = 0x00000013; // APASSFlag_NoMag_V|APASSFlag_NoMag_B|APASSFlag_NoMag_r
      xpsd.verbosity = 0; // work quietly
      xpsd.generateTextOutput = false;

      for ( let i = 0;; ++i )
      {
         xpsd.magnitudeHigh = m;
         xpsd.executeGlobal();
         let stop = true;
         if ( xpsd.excessCount < %d )
         {
            stop = m > magHigh;
            if ( !stop )
               m *= 1.10;
         }
         else if ( xpsd.excessCount > %d )
         {
            stop = m < 7;
            if ( !stop )
               m *= 0.95;
         }

         if ( stop )
            break;
      }
      __PJSR_APASS_limitMagnitude = m;
   }
} )JS_SOURCE", catalogSelector, magnitudeHigh, centerRD.x, centerRD.y, radius, TruncInt( 0.875*targetCount ), targetCount ) );

      Console().ExecuteScript( scriptFilePath );

      Variant result = Module->EvaluateScript( "__PJSR_APASS_limitMagnitude" );
      if ( result.IsValid() )
         magnitudeHigh = result.ToFloat();
      if ( magnitudeHigh < 0 )
         throw Error( "Invalid APASS XPSD server execution" );
   }
   catch ( ... )
   {
   }

   try
   {
      if ( File::Exists( scriptFilePath ) )
         File::Remove( scriptFilePath );
   }
   catch ( ... )
   {
   }

   return magnitudeHigh;
}

// ----------------------------------------------------------------------------

struct StarData
{
   DPoint pos;     // image coordinates
   float  B, V, r; // magnitude in Johnson V,B and Sloan r'

   using component = DPoint::component;

   component operator []( int i ) const noexcept // QuadTree-compatible
   {
      return (i == 0) ? pos.x : pos.y;
   }
};

class APASSStarExtractionThread : public Thread
{
public:

   Array<StarData> stars;

   APASSStarExtractionThread( const AbstractImage::ThreadData& data,
                              const AstrometricMetadata& A,
                              const ByteArray& fileData,
                              size_type start, size_type end )
      : m_data( data )
      , m_A( A )
      , m_fileData( fileData )
      , m_start( start ), m_end( end )
   {
   }

   PCL_HOT_FUNCTION void Run() override
   {
      INIT_THREAD_MONITOR()

      ByteArray::const_iterator p = m_fileData.At( sizeof( APASSBinaryHeader ) + sizeof( APASSStarData )*m_start );
      for ( size_type i = m_start; i < m_end; ++i, p += sizeof( APASSStarData ) )
      {
         StarData star;
         const APASSStarData* starData = reinterpret_cast<const APASSStarData*>( p );
         if ( m_A.CelestialToImage( star.pos, DPoint( starData->ra, starData->dec ) ) )
            if ( m_A.Bounds().Includes( star.pos ) )
            {
               star.B = starData->mag_B;
               star.V = starData->mag_V;
               star.r = starData->mag_r;
               stars << star;
            }

         UPDATE_THREAD_MONITOR( 1 )
      }
   }

private:

   const AbstractImage::ThreadData& m_data;
   const AstrometricMetadata&       m_A;
   const ByteArray&                 m_fileData;
         size_type                  m_start, m_end;
};

static Array<StarData>
APASSXPSDSearch( const DPoint& centerRD, double radius, const AstrometricMetadata& A, float magnitudeHigh, const String& catalogId )
{
   const char* catalogSelector;
   if ( catalogId == "APASSDR10_XPSD" )
      catalogSelector = "DataRelease_10";
   else if ( catalogId == "APASSDR9_XPSD" )
      catalogSelector = "DataRelease_9";
   else
      throw Error( "Internal error: Invalid catalog identifier in APASSXPSDSearch()" );

   Array<StarData> stars;
   String scriptFilePath = File::UniqueFileName( File::SystemTempDirectory(), 12, "PCC_", ".js" );
   String binFilePath;
   try
   {
      File::WriteTextFile( scriptFilePath, IsoString().Format(
R"JS_SOURCE(var __PJSR_APASS_valid = false;
var __PJSR_APASS_outputFilePath = File.uniqueFileName( File.systemTempDirectory, 12/*randomLength*/, "APASS_"/*prefix*/, ".bin"/*postfix*/ );
var xpsd = new APASS;
xpsd.command = "search";
xpsd.centerRA = %.8f;
xpsd.centerDec = %.8f;
xpsd.radius = %.8f;
xpsd.magnitudeLow = -1.5;
xpsd.magnitudeHigh = %.2f;
xpsd.exclusionFlags = 0x00000013; // APASSFlag_NoMag_V|APASSFlag_NoMag_B|APASSFlag_NoMag_r
xpsd.verbosity = 2;
xpsd.dataRelease = APASS.prototype.%s;
xpsd.sortBy = APASS.prototype.SortBy_V;
xpsd.generateTextOutput = false;
xpsd.generateBinaryOutput = true;
xpsd.outputFilePath = __PJSR_APASS_outputFilePath;
if ( xpsd.executeGlobal() )
   __PJSR_APASS_valid = xpsd.isValid;
)JS_SOURCE", centerRD.x, centerRD.y, radius, magnitudeHigh, catalogSelector ) );

      Console().ExecuteScript( scriptFilePath );

      Variant result = Module->EvaluateScript( "__PJSR_APASS_valid" );
      if ( !result.IsValid() || !result.ToBoolean() )
         throw Error( "Invalid APASS XPSD server execution" );

      result = Module->EvaluateScript( "__PJSR_APASS_outputFilePath" );
      if ( !result.IsValid() )
         throw Error( "Invalid APASS XPSD server execution" );
      binFilePath = result.ToString();
      if ( binFilePath.IsEmpty() || !File::Exists( binFilePath ) )
         throw Error( "Invalid APASS XPSD server execution" );

      ByteArray fileData = File::ReadFile( binFilePath );

      const size_type totalStars = (fileData.Length() - sizeof( APASSBinaryHeader ))/sizeof( APASSStarData );

      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Extracting catalog star magnitude data", totalStars );

      Array<size_type> L = Thread::OptimalThreadLoads( totalStars, 16u/*overheadLimit*/ );
      ReferenceArray<APASSStarExtractionThread> threads;
      AbstractImage::ThreadData data( monitor, totalStars );
      for ( size_type i = 0, n = 0; i < L.Length(); n += L[i++] )
         threads.Add( new APASSStarExtractionThread( data, A, fileData, n, n + L[i] ) );
      AbstractImage::RunThreads( threads, data );
      for ( const APASSStarExtractionThread& thread : threads )
         stars << thread.stars;

      threads.Destroy();
   }
   catch ( ... )
   {
   }

   try
   {
      if ( File::Exists( scriptFilePath ) )
         File::Remove( scriptFilePath );
      if ( !binFilePath.IsEmpty() )
         if ( File::Exists( binFilePath ) )
            File::Remove( binFilePath );
   }
   catch ( ... )
   {
   }

   return stars;
}

// ----------------------------------------------------------------------------

class DownloadControl : public Control
{
public:

   bool Aborted() const
   {
      return m_aborted;
   }

   const IsoString& Text() const
   {
      return m_text;
   }

   IsoStringList TextLines() const
   {
      IsoStringList lines;
      m_text.Break( lines, '\n', true/*trim*/ );
      lines.Remove( IsoString() );
      return lines;
   }

   bool e_Download( NetworkTransfer& sender, const void* buffer, fsize_type size )
   {
      Module->ProcessEvents();
      if ( m_console.AbortRequested() )
      {
         m_console.Abort();
         m_aborted = true;
         return false;
      }

      m_text.Append( static_cast<const char*>( buffer ), size );
      return true;
   }

   bool e_Progress( NetworkTransfer& sender,
                    fsize_type downloadTotal, fsize_type downloadCurrent,
                    fsize_type uploadTotal, fsize_type uploadCurrent )
   {
      Module->ProcessEvents();
      if ( m_console.AbortRequested() )
      {
         m_console.Abort();
         m_aborted = true;
         return false;
      }

      if ( downloadTotal > 0 )
         m_console.Write( String().Format( "<end><clrbol>%u of %u bytes transferred (%d%%)",
                                           downloadCurrent, downloadTotal,
                                           RoundInt( 100.0*downloadCurrent/downloadTotal ) ) );
      else
         m_console.Write( String().Format( "<end><clrbol>%u bytes transferred (unknown size)",
                                           downloadCurrent ) );
      Module->ProcessEvents();
      return true;
   }

private:

   Console   m_console;
   IsoString m_text;
   bool      m_aborted = false;
};

static Array<StarData>
APASSOnlineSearch( const DPoint& centerRD, double radius, const AstrometricMetadata& A, float magnitudeHigh, const String& serverURL )
{
   const int maxRecords = 50000;
   String url = serverURL;
   if ( !url.EndsWith( '/' ) )
      url << '/';
   url.AppendFormat( "viz-bin/asu-tsv?-source=II/336/apass9"
                     "&-c=%.6f %.6f&-c.r=%.6f&-c.u=deg&-out.form=|&-out.max=%d&-out.add=_RAJ,_DEJ&-out=Bmag&-out=Vmag&-out=r'mag&Vmag=<%.2f",
                     centerRD.x, centerRD.y, radius, maxRecords, magnitudeHigh );
   DownloadControl control;
   NetworkTransfer transfer;
   transfer.SetURL( url );
   transfer.OnDownloadDataAvailable( (NetworkTransfer::download_event_handler)&DownloadControl::e_Download, control );
   transfer.OnTransferProgress( (NetworkTransfer::progress_event_handler)&DownloadControl::e_Progress, control );

   Console().WriteLn( "<end><cbr><br><wrap><raw>" + url + "</raw>" );
   Module->ProcessEvents();

   if ( !transfer.Download() )
   {
      if ( control.Aborted() )
         throw ProcessAborted();
      throw Error( "Download failed: " + transfer.ErrorInformation() );
   }

   IsoStringList lines = control.TextLines();

   if ( lines.Length() < 20 ) // VizieR always returns at least 20 comment lines
      throw Error( "Unknown catalog server error: too short response." );
   if ( lines.Length() > maxRecords - 100 )
      throw Error( "The catalog server has returned a possibly incomplete query result - please reduce the limit magnitude parameter." );

// ------------|------------|------|------|------
// 149.63261200|+09.07414000|16.383|15.608|15.383

   Array<StarData> stars;
   for ( const IsoString& line : lines )
      if ( IsoCharTraits::IsDigit( line[0] ) )
      {
         IsoStringList tokens;
         line.Break( tokens, '|', true/*trim*/ );
         if ( tokens.Length() >= 5 )
            if ( !tokens[2].IsEmpty() && !tokens[3].IsEmpty() && !tokens[4].IsEmpty() )
            {
               StarData star;
               if ( A.CelestialToImage( star.pos, DPoint( tokens[0].ToDouble(), tokens[1].ToDouble() ) ) )
                  if ( A.Bounds().Includes( star.pos ) )
                  {
                     star.B = tokens[2].ToFloat();
                     star.V = tokens[3].ToFloat();
                     star.r = tokens[4].ToFloat();
                     stars << star;
                  }
            }
      }

   return stars;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInstance::ExecuteOn( View& view )
{
   {
      String whyNot;
      if ( !CanExecuteOn( view, whyNot ) )
         throw Error( whyNot );
   }

   Console console;
   console.EnableAbort();

   AutoViewLock lock( view );

   ImageWindow window = view.Window();

   /*
    * Astrometric solution
    */
   AstrometricMetadata A( window );
   if ( !A.IsValid() )
      throw Error( "The image has no valid astrometric solution: " + view.Id() );

   /*
    * Identify the photometric catalog
    */
   String catalogId, catalogName;
   bool hasXPSDServer = false;
   if ( p_autoCatalog )
   {
      catalogId = BestAvailableAPASSXPSDCatalog();
      hasXPSDServer = !catalogId.IsEmpty();
      if ( !hasXPSDServer )
         catalogId = "APASSDR9";
   }
   else
   {
      if ( p_catalogId.CompareIC( "APASS" ) == 0 || p_catalogId.CompareIC( "APASSDR9" ) == 0 || p_catalogId.CompareIC( "APASS_DR9" ) == 0 )
         catalogId = "APASSDR9";
      else if ( p_catalogId.CompareIC( "APASSDR10_XPSD" ) == 0 || p_catalogId.CompareIC( "APASS_DR10_XPSD" ) == 0 )
      {
         catalogId = "APASSDR10_XPSD";
         hasXPSDServer = true;
      }
      else if ( p_catalogId.CompareIC( "APASSDR9_XPSD" ) == 0 || p_catalogId.CompareIC( "APASS_DR9_XPSD" ) == 0 )
      {
         catalogId = "APASSDR9_XPSD";
         hasXPSDServer = true;
      }
   }

   if ( catalogId == "APASSDR9" )
      catalogName = "APASS DR9 (VizieR)";
   else if ( catalogId == "APASSDR10_XPSD" )
      catalogName = "APASS DR10 (XPSD)";
   else if ( catalogId == "APASSDR9_XPSD" )
      catalogName = "APASS DR9 (XPSD)";
   else
      throw Error( "Unknown/unsupported photometric catalog '" + p_catalogId + "'" );

   console.NoteLn( "<end><cbr><br>* Using the " + catalogName + " catalog" + String( p_autoCatalog ? " (automatically selected)" : "" ) + '.' );

   /*
    * Get XPSD database information.
    */
   if ( hasXPSDServer )
   {
      console.WriteLn( "<end><cbr><br>* Retrieving " + catalogName + " database information" );
      Module->ProcessEvents();

      APASSSPDatabaseInfo dbInfo = DatabaseInfoFromAPASSXPSDServer( APASSDataReleaseParameterValueFromCatalogId( catalogId ) );

      if ( dbInfo.filePaths.IsEmpty() )
         throw Error( "Database files not available for the " + catalogName + " catalog, or the XPSD server is not properly configured." );

      console.WriteLn( String().Format( "Data release ...... %s\n"
                                        "Database files .... %u\n"
                                        "Magnitude range ... [%.2f,%.2f]\n",
                                        APASSDataReleaseNameFromParameterValue( dbInfo.dataRelease ),
                                        unsigned( dbInfo.filePaths.Length() ),
                                        dbInfo.magnitudeLow, dbInfo.magnitudeHigh ) );
      Module->ProcessEvents();
   }
   else
   {
      console.WarningLn( "<end><cbr><br>** Warning: Not using a local XPSD database - online catalog access can lead to suboptimal calibration results." );
   }

   /*
    * Search center and radius.
    */
   DRect bounds = view.Bounds();
   DPoint centerRD;
   if ( !A.ImageToCelestial( centerRD, bounds.Center() ) )
      throw Error( "Invalid astrometric solution: unable to compute celestial coordinates for the center of the image" );
   double radius = A.SearchRadius();

   /*
    * Limit magnitude
    */
   float magnitudeHigh = p_limitMagnitude;
   if ( p_autoLimitMagnitude )
      magnitudeHigh = APASSAutoLimitMagnitude( centerRD, radius, A, catalogId, p_targetSourceCount );

   console.WriteLn( String().Format( "<end><cbr>Limit magnitude: %.2f", magnitudeHigh ) );

   /*
    * Perform database search
    */
   console.WriteLn( "<end><cbr><br>* Searching catalog sources" );
   Module->ProcessEvents();

   Array<StarData> catalogStars = hasXPSDServer ? APASSXPSDSearch( centerRD, radius, A, magnitudeHigh, catalogId )
                                                : APASSOnlineSearch( centerRD, radius, A, magnitudeHigh, p_serverURL );
   if ( catalogStars.IsEmpty() )
      throw Error( "No catalog stars found" );

   console.WriteLn( String().Format( "<end><cbr><br>%u catalog sources found.", catalogStars.Length() ) );
   Module->ProcessEvents();

   // Search tree
   QuadTree<StarData> T( catalogStars );

   /*
    * Find optimal saturation threshold
    */
   ImageVariant image = view.Image();
   float saturationThreshold = p_saturationThreshold;
   if ( p_saturationRelative )
   {
      // Exclude border regions, which can contain outliers.
      int dx = TruncInt( image.Bounds().Width() * p_saturationShrinkFactor );
      int dy = TruncInt( image.Bounds().Height() * p_saturationShrinkFactor );
      Rect rect = image.Bounds().DeflatedBy( dx, dy );
      // The saturation limit is the smallest maximum among the three channels.
      saturationThreshold *= Min( Min( image.MaximumSampleValue( rect, 0, 0 ),
                                       image.MaximumSampleValue( rect, 1, 1 ) ),
                                       image.MaximumSampleValue( rect, 2, 2 ) );
      console.WriteLn( String().Format( "<end><cbr><br>Saturation threshold: %.4f", saturationThreshold ) );
   }

   /*
    * Configure PSF signal evaluation
    */
   PSFEstimator E;
   E.Detector().SetStructureLayers( p_structureLayers );
   E.Detector().SetNoiseLayers( p_psfNoiseLayers );
   E.Detector().SetHotPixelFilterRadius( p_psfHotPixelFilterRadius );
   E.Detector().SetNoiseReductionFilterRadius( p_psfNoiseReductionFilterRadius );
   E.Detector().SetMinStructureSize( p_psfMinStructureSize );
   E.Detector().SetMinSNR( p_psfMinSNR );
   E.Detector().EnableClusteredSources( p_psfAllowClusteredSources );
   E.SetPSFType( PCCPSFType::ToPSFFunction( p_psfType ) );
   E.SetSaturationThreshold( saturationThreshold );
   E.DisableRelativeSaturation();
   E.SetGrowthFactorForFluxMeasurement( p_psfGrowth );
   E.SetMaxStars( p_psfMaxStars );

   /*
    * Get PSF signal samples
    */
   console.WriteLn( "<end><cbr><br>* Extracting PSF signal samples" );
   Module->ProcessEvents();

   Array<StarPSFSample> psfSamples;
   {
      volatile AutoStatusCallbackRestorer saveStatus( image.Status() );
      StandardStatus status;
      image.SetStatusCallback( &status );

      Array<PSFData> psfData[ 3 ];
      for ( int c = 0; c < 3; ++c )
      {
         image.SelectChannel( c );
         psfData[c] = E.FitStars( image );
         if ( psfData[c].IsEmpty() )
            throw Error( "No stars found (channel " + String( c ) + ")" );
      }
      image.ResetSelections();

      // Match channel PSF samples
      QuadTree<PSFData> T0( psfData[0] );    // red
      QuadTree<PSFData> T2( psfData[2] );    // blue
      for ( const PSFData& p1 : psfData[1] ) // green is reference
      {
         QuadTree<PSFData>::rectangle r( p1.c0.x - p_psfChannelSearchTolerance, p1.c0.y - p_psfChannelSearchTolerance,
                                         p1.c0.x + p_psfChannelSearchTolerance, p1.c0.y + p_psfChannelSearchTolerance );
         QuadTree<PSFData>::point_list P0 = T0.Search( r );
         if ( !P0.IsEmpty() )
         {
            QuadTree<PSFData>::point_list P2 = T2.Search( r );
            if ( !P2.IsEmpty() )
            {
               StarPSFSample s;
               s.pos = p1.c0;
               s.bounds = p1.FWTMBounds();
               s.signal[0] = P0[0].signal;
               s.signal[1] = p1.signal;
               s.signal[2] = P2[0].signal;
               psfSamples << s;
            }
         }
      }
   }

   /*
    * Color samples
    */
   console.WriteLn( "<end><cbr><br>* Computing color calibration functions" );
   Module->ProcessEvents();

   Array<ColorSample> colorSamples;
   IsoString text;
   Array<StarPSFSample> validPSFSamples;
   for ( const StarPSFSample& psf : psfSamples )
   {
      // Match coordinates by proximity search
      QuadTree<StarData>::point_list P =
         T.Search( QuadTree<StarData>::rectangle( psf.pos.x - p_psfSearchTolerance, psf.pos.y - p_psfSearchTolerance,
                                                  psf.pos.x + p_psfSearchTolerance, psf.pos.y + p_psfSearchTolerance ) );
      if ( !P.IsEmpty() )
      {
         size_type j = 0;
         if ( P.Length() > 1 )
         {
            double d2 = P[0].pos.SquaredDistanceTo( psf.pos );
            for ( size_type i = 1; i < P.Length(); ++i )
            {
               double d2i = P[i].pos.SquaredDistanceTo( psf.pos );
               if ( d2i < d2 )
               {
                  j = i;
                  d2 = d2i;
               }
            }
         }

         ColorSample sample;
         sample.pos = psf.pos;
         sample.catalog[0] = P[j].r;
         sample.catalog[1] = P[j].V;
         sample.catalog[2] = P[j].B;
         sample.measured[0] = psf.signal[0];
         sample.measured[1] = psf.signal[1];
         sample.measured[2] = psf.signal[2];

         bool validSample = true;
         for ( int c = 0; c < 3; ++c )
            if ( 1 + sample.measured[c] == 1 || !IsFinite( sample.catalog[c] ) )
            {
               validSample = false;
               break;
            }
         if ( validSample )
         {
            colorSamples << sample;

            if ( p_generateTextFiles )
            {
               DPoint posRD = 0;
               A.ImageToCelestial( posRD, sample.pos );
               text << IsoString().Format( "%.2f,%.2f,%.7f,%.7f,%.6e,%.6e,%.6e,%.6f,%.6f,%.6f,%+.6f,%+.6f,%+.6f,%+.6f\n",
                                           sample.pos.x, sample.pos.y, posRD.x, posRD.y,
                                           sample.catalog[0], sample.catalog[1], sample.catalog[2],
                                           sample.measured[0], sample.measured[1], sample.measured[2],
                                           sample.catalog[0] - sample.catalog[1],
                                           sample.catalog[2] - sample.catalog[1],
                                           -2.5118 * Log( sample.measured[0]/sample.measured[1] ),
                                           -2.5118 * Log( sample.measured[2]/sample.measured[1] ) );
            }

            if ( p_generateStarMaps )
               validPSFSamples << psf;
         }
      }
   }

   if ( p_generateTextFiles )
   {
      text.Prepend( "x,y,alpha,delta,cat_0,cat_1,cat_2,img_0,img_1,img_2,cat_01,cat_21,img_01,img_21\n" );
      String outputDir = File::WindowsPathToUnix( p_outputDirectory.Trimmed() );
      if ( outputDir.IsEmpty() )
         outputDir = File::SystemTempDirectory();
      if ( !outputDir.EndsWith( '/' ) )
         outputDir << '/';
      File::WriteTextFile( outputDir + "PCC-data.csv", text );
   }

   if ( p_generateStarMaps )
      CreateStarMapWindow( image.Width(), image.Height(), "PCC_stars", validPSFSamples );

   /*
    * Fit color transformation functions.
    */
   Vector catIndexRG( colorSamples.Length() );
   Vector imgIndexRG( colorSamples.Length() );
   Vector catIndexBG( colorSamples.Length() );
   Vector imgIndexBG( colorSamples.Length() );
   for ( size_type i = 0; i < colorSamples.Length(); ++i )
   {
      catIndexRG[i] = colorSamples[i].catalog[0] - colorSamples[i].catalog[1];
      imgIndexRG[i] = -2.5118 * Log( colorSamples[i].measured[0]/colorSamples[i].measured[1] );
      catIndexBG[i] = colorSamples[i].catalog[2] - colorSamples[i].catalog[1];
      imgIndexBG[i] = -2.5118 * Log( colorSamples[i].measured[2]/colorSamples[i].measured[1] );
   }

   console.WriteLn();

   LinearFit LRG = RobustLinearRegression( catIndexRG, imgIndexRG, "R-G" );
   LinearFit LBG = RobustLinearRegression( catIndexBG, imgIndexBG, "B-G" );

   console.WriteLn( String().Format( "<end><cbr><br>* Color calibration functions:\n"
                                    "f_R : R-G = %+.7f*(r'- V) %c %.7f, sigma = %.7f\n"
                                    "f_B : B-G = %+.7f*(B - V) %c %.7f, sigma = %.7f",
                                    LRG.b, (LRG.a < 0) ? '-' : '+', Abs( LRG.a ), LRG.adev,
                                    LBG.b, (LBG.a < 0) ? '-' : '+', Abs( LBG.a ), LBG.adev ) );

   double WRG = p_whiteReferenceSr_JV - p_zeroPointSr_JV;
   double WBG = p_whiteReferenceJB_JV - p_zeroPointJB_JV;

   DVector W = DVector( Pow( 2.5118, LRG( WRG ) ),
                        1.0,
                        Pow( 2.5118, LBG( WBG ) ) );
   W /= W.MaxComponent();

   console.NoteLn( String().Format( "<end><cbr><br>* White balance factors:\n"
                                    "W_R : %.4f\n"
                                    "W_G : %.4f\n"
                                    "W_B : %.4f", W[0], W[1], W[2] ) );

   if ( p_generateGraphs )
   {
      if ( !ThePhotometricColorCalibrationGraphInterface->IsVisible() )
         ThePhotometricColorCalibrationGraphInterface->Launch();
      ThePhotometricColorCalibrationGraphInterface->UpdateGraphs( view.FullId(), catalogName,
                                                                  "Sloan r'", "Johnson V", "Johnson B",
                                                                  "n/a", p_whiteReferenceName,
                                                                  catIndexRG, imgIndexRG, LRG,
                                                                  catIndexBG, imgIndexBG, LBG,
                                                                  W, WRG, WBG );
   }

   /*
    * Metadata generation
    */
   {
      DVector W0( 2 );
      W0[0] = p_whiteReferenceSr_JV;
      W0[1] = p_whiteReferenceJB_JV;
      DVector Z0( 2 );
      Z0[0] = p_zeroPointSr_JV;
      Z0[1] = p_zeroPointJB_JV;

      window.MainView().SetStorablePermanentProperties( PropertyArray()
                                    << Property( "PCL:PCC:Fit_Sr_JV_R_G", DVector( LRG.a, LRG.b, LRG.adev ) )
                                    << Property( "PCL:PCC:Fit_JB_JV_B_G", DVector( LBG.a, LBG.b, LBG.adev ) )
                                    << Property( "PCL:PCC:White_Sr_JV", W0[0] )
                                    << Property( "PCL:PCC:White_JB_JV", W0[1] )
                                    << Property( "PCL:PCC:Zero_Sr_JV", Z0[0] )
                                    << Property( "PCL:PCC:Zero_JB_JV", Z0[1] )
                                    << Property( "PCL:PCC:Scale_Sr_JV_JB_JV", W ) );
   }

   /*
    * Apply white balance and background neutralization.
    */
   if ( p_applyCalibration )
   {
      ImageVariant image = view.Image();

      console.WriteLn();
      ApplyWhiteBalance( image, W );

      if ( p_neutralizeBackground )
      {
         if ( p_backgroundUseROI )
         {
            p_backgroundROI.Order();
            // ROI validity already checked by CanExecuteOn(), which we
            // have called above.
//                   if ( !p_backgroundROI.IsRect() )
//                      throw Error( "Empty background reference ROI defined" );
         }

         if ( p_backgroundHigh < p_backgroundLow )
            Swap( p_backgroundLow, p_backgroundHigh );

         DVector backgroundReference;
         {
            View bkgView;
            if ( p_backgroundReferenceViewId.IsEmpty() )
               bkgView = view;
            else
            {
               bkgView = View::ViewById( p_backgroundReferenceViewId );
               if ( bkgView.IsNull() )
                  throw Error( "No such view (background reference): " + p_backgroundReferenceViewId );
               if ( !bkgView.IsColor() )
                  throw Error( "The background reference view must be a color image: " + p_backgroundReferenceViewId );
            }

            AutoViewLock lock( bkgView, false/*lock*/ );
            if ( bkgView.CanWrite() )
               lock.LockForWrite();

            if ( bkgView != view )
            {
               String message = "<end><cbr><br>** Warning: Using a ";
               if ( bkgView.IsPreview() && bkgView.Window() == window )
                  message << "preview of the target image as background reference";
               else
                  message << "background reference view different from the target image";
               message << ". This usually does not work.";
               console.WarningLn( message );
            }

            ImageVariant bkgImage = bkgView.Image();

            if ( p_backgroundUseROI )
            {
               bkgImage.SelectRectangle( p_backgroundROI );
               if ( !bkgImage.SelectedRectangle().IsRect() )
                  bkgImage.ResetSelection();
            }

            console.WriteLn();
            backgroundReference = ComputeBackgroundReference( bkgImage, p_backgroundLow, p_backgroundHigh );
         }

         console.NoteLn( String().Format( "<end><cbr><br>* Background reference:\n"
                                          "B_R : %.8f\n"
                                          "B_G : %.8f\n"
                                          "B_B : %.8f",
                                          backgroundReference[0], backgroundReference[1], backgroundReference[2] ) );

         window.MainView().SetStorablePermanentPropertyValue( "PCL:PCC:BackgroundReference", backgroundReference );

         console.WriteLn();
         ApplyBackgroundNeutralization( image, backgroundReference );
      }
   }

   return true;
}

// ----------------------------------------------------------------------------

void* PhotometricColorCalibrationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == ThePCCApplyCalibrationParameter )
      return &p_applyCalibration;
   if ( p == ThePCCWhiteReferenceIdParameter )
      return p_whiteReferenceId.Begin();
   if ( p == ThePCCWhiteReferenceNameParameter )
      return p_whiteReferenceName.Begin();
   if ( p == ThePCCWhiteReferenceSr_JVParameter )
      return &p_whiteReferenceSr_JV;
   if ( p == ThePCCWhiteReferenceJB_JVParameter )
      return &p_whiteReferenceJB_JV;
   if ( p == ThePCCZeroPointSr_JVParameter )
      return &p_zeroPointSr_JV;
   if ( p == ThePCCZeroPointJB_JVParameter )
      return &p_zeroPointJB_JV;
   if ( p == ThePCCAutoCatalogParameter )
      return &p_autoCatalog;
   if ( p == ThePCCCatalogIdParameter )
      return p_catalogId.Begin();
   if ( p == ThePCCLimitMagnitudeParameter )
      return &p_limitMagnitude;
   if ( p == ThePCCAutoLimitMagnitudeParameter )
      return &p_autoLimitMagnitude;
   if ( p == ThePCCTargetSourceCountParameter )
      return &p_targetSourceCount;
   if ( p == ThePCCServerURLParameter )
      return p_serverURL.Begin();
   if ( p == ThePCCStructureLayersParameter )
      return &p_structureLayers;
   if ( p == ThePCCSaturationThresholdParameter )
      return &p_saturationThreshold;
   if ( p == ThePCCSaturationRelativeParameter )
      return &p_saturationRelative;
   if ( p == ThePCCSaturationShrinkFactorParameter )
      return &p_saturationShrinkFactor;
   if ( p == ThePCCPSFNoiseLayersParameter )
      return &p_psfNoiseLayers;
   if ( p == ThePCCPSFHotPixelFilterRadiusParameter )
      return &p_psfHotPixelFilterRadius;
   if ( p == ThePCCPSFNoiseReductionFilterRadiusParameter )
      return &p_psfNoiseReductionFilterRadius;
   if ( p == ThePCCPSFMinStructureSizeParameter )
      return &p_psfMinStructureSize;
   if ( p == ThePCCPSFMinSNRParameter )
      return &p_psfMinSNR;
   if ( p == ThePCCPSFAllowClusteredSourcesParameter )
      return &p_psfAllowClusteredSources;
   if ( p == ThePCCPSFTypeParameter )
      return &p_psfType;
   if ( p == ThePCCPSFGrowthParameter )
      return &p_psfGrowth;
   if ( p == ThePCCPSFMaxStarsParameter )
      return &p_psfMaxStars;
   if ( p == ThePCCPSFSearchToleranceParameter )
      return &p_psfSearchTolerance;
   if ( p == ThePCCPSFChannelSearchToleranceParameter )
      return &p_psfChannelSearchTolerance;
   if ( p == ThePCCNeutralizeBackgroundParameter )
      return &p_neutralizeBackground;
   if ( p == ThePCCBackgroundReferenceViewIdParameter )
      return p_backgroundReferenceViewId.Begin();
   if ( p == ThePCCBackgroundLowParameter )
      return &p_backgroundLow;
   if ( p == ThePCCBackgroundHighParameter )
      return &p_backgroundHigh;
   if ( p == ThePCCBackgroundUseROIParameter )
      return &p_backgroundUseROI;
   if ( p == ThePCCBackgroundROIX0Parameter )
      return &p_backgroundROI.x0;
   if ( p == ThePCCBackgroundROIY0Parameter )
      return &p_backgroundROI.y0;
   if ( p == ThePCCBackgroundROIX1Parameter )
      return &p_backgroundROI.x1;
   if ( p == ThePCCBackgroundROIY1Parameter )
      return &p_backgroundROI.y1;
   if ( p == ThePCCGenerateGraphsParameter )
      return &p_generateGraphs;
   if ( p == ThePCCGenerateStarMapsParameter )
      return &p_generateStarMaps;
   if ( p == ThePCCGenerateTextFilesParameter )
      return &p_generateTextFiles;
   if ( p == ThePCCOutputDirectoryParameter )
      return p_outputDirectory.Begin();

   return nullptr;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == ThePCCWhiteReferenceIdParameter )
   {
      p_whiteReferenceId.Clear();
      if ( sizeOrLength > 0 )
         p_whiteReferenceId.SetLength( sizeOrLength );
   }
   else if ( p == ThePCCWhiteReferenceNameParameter )
   {
      p_whiteReferenceName.Clear();
      if ( sizeOrLength > 0 )
         p_whiteReferenceName.SetLength( sizeOrLength );
   }
   else if ( p == ThePCCCatalogIdParameter )
   {
      p_catalogId.Clear();
      if ( sizeOrLength > 0 )
         p_catalogId.SetLength( sizeOrLength );
   }
   else if ( p == ThePCCServerURLParameter )
   {
      p_serverURL.Clear();
      if ( sizeOrLength > 0 )
         p_serverURL.SetLength( sizeOrLength );
   }
   else if ( p == ThePCCBackgroundReferenceViewIdParameter )
   {
      p_backgroundReferenceViewId.Clear();
      if ( sizeOrLength > 0 )
         p_backgroundReferenceViewId.SetLength( sizeOrLength );
   }
   else if ( p == ThePCCOutputDirectoryParameter )
   {
      p_outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_outputDirectory.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type PhotometricColorCalibrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == ThePCCWhiteReferenceIdParameter )
      return p_whiteReferenceId.Length();
   if ( p == ThePCCWhiteReferenceNameParameter )
      return p_whiteReferenceName.Length();
   if ( p == ThePCCCatalogIdParameter )
      return p_catalogId.Length();
   if ( p == ThePCCServerURLParameter )
      return p_serverURL.Length();
   if ( p == ThePCCBackgroundReferenceViewIdParameter )
      return p_backgroundReferenceViewId.Length();
   if ( p == ThePCCOutputDirectoryParameter )
      return p_outputDirectory.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationInstance.cpp - Released 2024-12-23T11:33:28Z
