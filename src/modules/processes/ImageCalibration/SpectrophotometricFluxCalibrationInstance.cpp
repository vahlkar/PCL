//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 2.2.4
// ----------------------------------------------------------------------------
// SpectrophotometricFluxCalibrationInstance.cpp - Released 2024-08-02T18:17:27Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "SpectrophotometricFluxCalibrationGraphInterface.h"
#include "SpectrophotometricFluxCalibrationInstance.h"
#include "SpectrophotometricFluxCalibrationInterface.h"
#include "SpectrophotometricFluxCalibrationParameters.h"
#include "SpectrophotometricFluxCalibrationProcess.h"

#include "../Gaia/GaiaInstance.h" // GaiaBinaryHeader

#include <pcl/AkimaInterpolation.h>
#include <pcl/AstrometricMetadata.h>
#include <pcl/AutoStatusCallbackRestorer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ElapsedTime.h>
#include <pcl/GaiaDatabaseFile.h>
#include <pcl/Graphics.h>
#include <pcl/ImageWindow.h>
#include <pcl/MetaModule.h>
#include <pcl/Position.h>
#include <pcl/RobustChauvenetRejection.h>
#include <pcl/SurfaceSimplifier.h>
#include <pcl/Thread.h>
#include <pcl/Version.h>

#include <pcl/api/APIInterface.h>

/*
 * Normalization factor applied to all SPFC flux samples. This allows us to
 * generate scale factors within manageable ranges for typical deep-sky images,
 * i.e., in [100,0.001] instead of [1e-7,1e-12], for example.
 */
#define SPFC_NORMALIZATION_FACTOR   1.0e+08

namespace pcl
{

// ----------------------------------------------------------------------------

SpectrophotometricFluxCalibrationInstance::SpectrophotometricFluxCalibrationInstance( const MetaProcess* P )
   : ProcessImplementation( P )
   , p_narrowbandMode( TheSPFCNarrowbandModeParameter->DefaultValue() )
   , p_grayFilterWavelength( TheSPFCGrayFilterWavelengthParameter->DefaultValue() )
   , p_grayFilterBandwidth( TheSPFCGrayFilterBandwidthParameter->DefaultValue() )
   , p_redFilterWavelength( TheSPFCRedFilterWavelengthParameter->DefaultValue() )
   , p_redFilterBandwidth( TheSPFCRedFilterBandwidthParameter->DefaultValue() )
   , p_greenFilterWavelength( TheSPFCGreenFilterWavelengthParameter->DefaultValue() )
   , p_greenFilterBandwidth( TheSPFCGreenFilterBandwidthParameter->DefaultValue() )
   , p_blueFilterWavelength( TheSPFCBlueFilterWavelengthParameter->DefaultValue() )
   , p_blueFilterBandwidth( TheSPFCBlueFilterBandwidthParameter->DefaultValue() )
   , p_broadbandIntegrationStepSize( TheSPFCBroadbandIntegrationStepSizeParameter->DefaultValue() )
   , p_narrowbandIntegrationSteps( TheSPFCNarrowbandIntegrationStepsParameter->DefaultValue() )
   , p_rejectionLimit( TheSPFCRejectionLimitParameter->DefaultValue() )
   , p_catalogId( TheSPFCCatalogIdParameter->DefaultValue() )
   , p_minMagnitude( TheSPFCMinMagnitudeParameter->DefaultValue() )
   , p_limitMagnitude( TheSPFCLimitMagnitudeParameter->DefaultValue() )
   , p_autoLimitMagnitude( TheSPFCAutoLimitMagnitudeParameter->DefaultValue() )
   , p_structureLayers( TheSPFCStructureLayersParameter->DefaultValue() )
   , p_saturationThreshold( TheSPFCSaturationThresholdParameter->DefaultValue() )
   , p_saturationRelative( TheSPFCSaturationRelativeParameter->DefaultValue() )
   , p_saturationShrinkFactor( TheSPFCSaturationShrinkFactorParameter->DefaultValue() )
   , p_psfNoiseLayers( TheSPFCPSFNoiseLayersParameter->DefaultValue() )
   , p_psfHotPixelFilterRadius( TheSPFCPSFHotPixelFilterRadiusParameter->DefaultValue() )
   , p_psfNoiseReductionFilterRadius( TheSPFCPSFNoiseReductionFilterRadiusParameter->DefaultValue() )
   , p_psfMinStructureSize( TheSPFCPSFMinStructureSizeParameter->DefaultValue() )
   , p_psfMinSNR( TheSPFCPSFMinSNRParameter->DefaultValue() )
   , p_psfAllowClusteredSources( TheSPFCPSFAllowClusteredSourcesParameter->DefaultValue() )
   , p_psfType( SPFCPSFType::Default )
   , p_psfGrowth( TheSPFCPSFGrowthParameter->DefaultValue() )
   , p_psfMaxStars( TheSPFCPSFMaxStarsParameter->DefaultValue() )
   , p_psfSearchTolerance( TheSPFCPSFSearchToleranceParameter->DefaultValue() )
   , p_psfChannelSearchTolerance( TheSPFCPSFChannelSearchToleranceParameter->DefaultValue() )
   , p_generateGraphs( TheSPFCGenerateGraphsParameter->DefaultValue() )
   , p_generateStarMaps( TheSPFCGenerateStarMapsParameter->DefaultValue() )
   , p_generateTextFiles( TheSPFCGenerateTextFilesParameter->DefaultValue() )
   , p_outputDirectory( TheSPFCOutputDirectoryParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

SpectrophotometricFluxCalibrationInstance::SpectrophotometricFluxCalibrationInstance( const SpectrophotometricFluxCalibrationInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInstance::Assign( const ProcessImplementation& p )
{
   const SpectrophotometricFluxCalibrationInstance* x = dynamic_cast<const SpectrophotometricFluxCalibrationInstance*>( &p );
   if ( x != nullptr )
   {
      p_grayFilterTrCurve             = x->p_grayFilterTrCurve;
      p_grayFilterName                = x->p_grayFilterName;
      p_redFilterTrCurve              = x->p_redFilterTrCurve;
      p_redFilterName                 = x->p_redFilterName;
      p_greenFilterTrCurve            = x->p_greenFilterTrCurve;
      p_greenFilterName               = x->p_greenFilterName;
      p_blueFilterTrCurve             = x->p_blueFilterTrCurve;
      p_blueFilterName                = x->p_blueFilterName;
      p_grayFilterWavelength          = x->p_grayFilterWavelength;
      p_grayFilterBandwidth           = x->p_grayFilterBandwidth;
      p_redFilterWavelength           = x->p_redFilterWavelength;
      p_redFilterBandwidth            = x->p_redFilterBandwidth;
      p_greenFilterWavelength         = x->p_greenFilterWavelength;
      p_greenFilterBandwidth          = x->p_greenFilterBandwidth;
      p_blueFilterWavelength          = x->p_blueFilterWavelength;
      p_blueFilterBandwidth           = x->p_blueFilterBandwidth;
      p_deviceQECurve                 = x->p_deviceQECurve;
      p_deviceQECurveName             = x->p_deviceQECurveName;
      p_narrowbandMode                = x->p_narrowbandMode;
      p_broadbandIntegrationStepSize  = x->p_broadbandIntegrationStepSize;
      p_narrowbandIntegrationSteps    = x->p_narrowbandIntegrationSteps;
      p_rejectionLimit                = x->p_rejectionLimit;
      p_catalogId                     = x->p_catalogId;
      p_minMagnitude                  = x->p_minMagnitude;
      p_limitMagnitude                = x->p_limitMagnitude;
      p_autoLimitMagnitude            = x->p_autoLimitMagnitude;
      p_structureLayers               = x->p_structureLayers;
      p_saturationThreshold           = x->p_saturationThreshold;
      p_saturationRelative            = x->p_saturationRelative;
      p_saturationShrinkFactor        = x->p_saturationShrinkFactor;
      p_psfNoiseLayers                = x->p_psfNoiseLayers;
      p_psfHotPixelFilterRadius       = x->p_psfHotPixelFilterRadius;
      p_psfNoiseReductionFilterRadius = x->p_psfNoiseReductionFilterRadius;
      p_psfMinStructureSize           = x->p_psfMinStructureSize;
      p_psfMinSNR                     = x->p_psfMinSNR;
      p_psfAllowClusteredSources      = x->p_psfAllowClusteredSources;
      p_psfType                       = x->p_psfType;
      p_psfGrowth                     = x->p_psfGrowth;
      p_psfMaxStars                   = x->p_psfMaxStars;
      p_psfSearchTolerance            = x->p_psfSearchTolerance;
      p_psfChannelSearchTolerance     = x->p_psfChannelSearchTolerance;
      p_generateGraphs                = x->p_generateGraphs;
      p_generateStarMaps              = x->p_generateStarMaps;
      p_generateTextFiles             = x->p_generateTextFiles;
      p_outputDirectory               = x->p_outputDirectory;
   }
}

// ----------------------------------------------------------------------------

UndoFlags SpectrophotometricFluxCalibrationInstance::UndoMode( const View& ) const
{
   return UndoFlag::ViewProperties;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricFluxCalibrationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.IsPreview() )
   {
      whyNot = "SpectrophotometricFluxCalibration cannot be executed on previews.";
      return false;
   }
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "SpectrophotometricFluxCalibration cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct GaiaSPDatabaseInfo
{
   StringList filePaths;
   int        dataRelease = -1;
   float      magnitudeLow = 0;
   float      magnitudeHigh = 0;
   float      spectrumStart = 0; // nm
   float      spectrumStep = 0;  // nm
   int        spectrumCount = 0;
   int        spectrumBits = 0;
   FVector    wavelengths; // nm
};

int GaiaDataReleaseParameterValueFromName( const String& drName )
{
   if ( drName.CompareIC( "GaiaDR3SP" ) == 0 )
      return 4;
   if ( drName.CompareIC( "GaiaDR3" ) == 0 )
      return 3;
   if ( drName.CompareIC( "GaiaEDR3" ) == 0 )
      return 2;
   if ( drName.CompareIC( "GaiaDR2" ) == 0 )
      return 1;
   return -1;
}

const char* GaiaDataReleaseNameFromParameterValue( int drParam )
{
   switch ( drParam )
   {
   case 1:  return "Gaia DR2";
   case 2:  return "Gaia EDR3";
   case 3:  return "Gaia DR3";
   case 4:  return "Gaia DR3SP";
   default: return "<unknown>";
   }
}

static GaiaSPDatabaseInfo DatabaseInfoFromGaiaXPSDServer( int drParam )
{
   String scriptPath = File::UniqueFileName( File::SystemTempDirectory(), 12, "GM_", ".js" );
   GaiaSPDatabaseInfo info;
   try
   {
      File::WriteTextFile( scriptPath, IsoString().Format(
R"JS_SOURCE( var __PJSR_Gaia_SP_databaseFilePaths = "";
var __PJSR_Gaia_SP_databaseFilePaths = "";
var __PJSR_Gaia_SP_outputDataRelease = -1;
var __PJSR_Gaia_SP_databaseMagnitudeLow = 0;
var __PJSR_Gaia_SP_databaseMagnitudeHigh = 0;
var __PJSR_Gaia_SP_databaseSpectrumStart = 0;
var __PJSR_Gaia_SP_databaseSpectrumStep = 0;
var __PJSR_Gaia_SP_databaseSpectrumCount = 0;
var __PJSR_Gaia_SP_databaseSpectrumBits = 0;
if ( (typeof Gaia) != 'undefined' )
{
   let xpsd = new Gaia;
   xpsd.command = "get-info";
   xpsd.dataRelease = %d;
   xpsd.verbosity = 2;
   xpsd.executeGlobal();
   __PJSR_Gaia_SP_databaseFilePaths = xpsd.databaseFilePaths.join();
   __PJSR_Gaia_SP_outputDataRelease = xpsd.outputDataRelease;
   __PJSR_Gaia_SP_databaseMagnitudeLow = xpsd.databaseMagnitudeLow;
   __PJSR_Gaia_SP_databaseMagnitudeHigh = xpsd.databaseMagnitudeHigh;
   __PJSR_Gaia_SP_databaseSpectrumStart = xpsd.databaseSpectrumStart;
   __PJSR_Gaia_SP_databaseSpectrumStep = xpsd.databaseSpectrumStep;
   __PJSR_Gaia_SP_databaseSpectrumCount = xpsd.databaseSpectrumCount;
   __PJSR_Gaia_SP_databaseSpectrumBits = xpsd.databaseSpectrumBits;
} )JS_SOURCE", drParam ) );

      Console().ExecuteScript( scriptPath );

      Variant result = Module->EvaluateScript( "__PJSR_Gaia_SP_databaseFilePaths" );
      if ( result.IsValid() )
         result.ToString().Break( info.filePaths, ',' );
      result = Module->EvaluateScript( "__PJSR_Gaia_SP_outputDataRelease" );
      if ( result.IsValid() )
         info.dataRelease = result.ToInt();
      result = Module->EvaluateScript( "__PJSR_Gaia_SP_databaseMagnitudeLow" );
      if ( result.IsValid() )
         info.magnitudeLow = result.ToFloat();
      result = Module->EvaluateScript( "__PJSR_Gaia_SP_databaseMagnitudeHigh" );
      if ( result.IsValid() )
         info.magnitudeHigh = result.ToFloat();
      result = Module->EvaluateScript( "__PJSR_Gaia_SP_databaseSpectrumStart" );
      if ( result.IsValid() )
         info.spectrumStart = result.ToFloat();
      result = Module->EvaluateScript( "__PJSR_Gaia_SP_databaseSpectrumStep" );
      if ( result.IsValid() )
         info.spectrumStep = result.ToFloat();
      result = Module->EvaluateScript( "__PJSR_Gaia_SP_databaseSpectrumCount" );
      if ( result.IsValid() )
         info.spectrumCount = result.ToInt();
      result = Module->EvaluateScript( "__PJSR_Gaia_SP_databaseSpectrumBits" );
      if ( result.IsValid() )
         info.spectrumBits = result.ToInt();

      if ( info.spectrumStart > 0 )
         if ( info.spectrumStep > 0 )
            if ( info.spectrumCount >= 5 )
            {
               info.wavelengths = FVector( info.spectrumCount );
               for ( int i = 0; i < info.spectrumCount; ++i )
                  info.wavelengths[i] = info.spectrumStart + i*info.spectrumStep;
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

   return info;
}

// ----------------------------------------------------------------------------

static float GaiaSPAutoLimitMagnitude( const DPoint& centerRD, double radius, int targetCount )
{
   float magnitudeHigh = -1;
   String scriptFilePath = File::UniqueFileName( File::SystemTempDirectory(), 12, "GM_", ".js" );

   try
   {
      File::WriteTextFile( scriptFilePath, IsoString().Format(
R"JS_SOURCE(var __PJSR_Gaia_SP_limitMagnitude = -1;
console.writeln( "<end><cbr><br>* Searching for optimal magnitude limit. Target: %d stars" );
let xpsd = ((typeof Gaia) != 'undefined') ? (new Gaia) : null;
if ( xpsd )
{
   xpsd.command = "get-info";
   xpsd.dataRelease = Gaia.prototype.DataRelease_3_SP;
   xpsd.executeGlobal();
   if ( xpsd.isValid )
   {
      xpsd.command = "search";
      xpsd.centerRA = %.8f;
      xpsd.centerDec = %.8f;
      xpsd.radius = %.8f;
      xpsd.magnitudeLow = -1.5;
      xpsd.sourceLimit = 0; // do not retrieve objects, just count them.
      xpsd.exclusionFlags = 0x00000001; // GaiaFlag_NoPM
      xpsd.inclusionFlags = 0;
      xpsd.verbosity = 0; // work quietly
      xpsd.generateTextOutput = false;

      let m = 10;
      for ( let m0 = 7, m1 = xpsd.databaseMagnitudeHigh, i = 0; i < 100; ++i )
      {
         xpsd.magnitudeHigh = m;
         xpsd.executeGlobal();
         if ( xpsd.excessCount < %d )
         {
            if ( m1 - m < 0.05 )
               break;
            m0 = m;
            m += (m1 - m)/2;
         }
         else if ( xpsd.excessCount > %d )
         {
            if ( m - m0 < 0.05 )
               break;
            m1 = m;
            m -= (m - m0)/2;
         }
         else
            break;
      }
      __PJSR_Gaia_SP_limitMagnitude = m;
   }
} )JS_SOURCE", targetCount, centerRD.x, centerRD.y, radius, targetCount, TruncInt( 1.05*targetCount ) ) );

      Console().ExecuteScript( scriptFilePath );

      Variant result = Module->EvaluateScript( "__PJSR_Gaia_SP_limitMagnitude" );
      if ( result.IsValid() )
         magnitudeHigh = result.ToFloat();
      if ( magnitudeHigh < 0 )
         throw Error( "Invalid Gaia/SP XPSD server execution" );
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

using SPInterpolation = AkimaInterpolation<float>;

struct StarSPData
{
   DPoint          pos;   // image coordinates
   SPInterpolation flux;  // interpolate the normalized sampled spectrum
   float           magG;  // mean G magnitude
   float           magBP; // mean G_BP magnitude
   float           magRP; // mean G_RP magnitude

   // QuadTree compatibility

   using component = DPoint::component;

   component operator []( int i ) const noexcept
   {
      return (i == 0) ? pos.x : pos.y;
   }
};

class GaiaSPStarExtractionThread : public Thread
{
public:

   Array<StarSPData> stars;

   GaiaSPStarExtractionThread( const AbstractImage::ThreadData& data,
                               const AstrometricMetadata& A,
                               const ByteArray& fileData,
                               const GaiaSPDatabaseInfo& dbInfo,
                               size_type start, size_type end )
      : m_data( data )
      , m_A( A )
      , m_fileData( fileData )
      , m_dbInfo( dbInfo )
      , m_start( start ), m_end( end )
   {
   }

   PCL_HOT_FUNCTION void Run() override
   {
      INIT_THREAD_MONITOR()

      const size_type fluxTableSize = m_dbInfo.wavelengths.Length() * sizeof( float );
      const size_type starDataSize = sizeof( GaiaStarDataBase ) + fluxTableSize;

      Position position( m_A.ObservationMiddleTime().OrElse( TimePoint::ReferenceEpoch() ), "UTC" );
      if ( m_A.LocationLongitude().IsDefined() )
         if ( m_A.LocationLatitude().IsDefined() )
            position.SetObserver( ObserverPosition( m_A.LocationLongitude()(), m_A.LocationLatitude()(), m_A.LocationHeight().OrElse( 0 ) ) );

      CoordinateReductionFunction crf = CoordinateReductionFunctionForReferenceSystem( m_A.ReferenceSystem() );

      ByteArray::const_iterator p = m_fileData.At( sizeof( GaiaBinaryHeader ) + starDataSize*m_start );
      for ( size_type i = m_start; i < m_end; ++i, p += starDataSize )
      {
         const GaiaStarDataBase* g = reinterpret_cast<const GaiaStarDataBase*>( p );
         StarPosition s( g->ra, g->dec, g->pmra, g->pmdec, g->parx, 0/*rv*/, 2457389.0/*2016.0*/ );
         DPoint pRD;
         (position.*crf)( s ).ToSpherical2Pi( pRD.x, pRD.y );

         StarSPData star;
         if ( m_A.CelestialToImage( star.pos, pRD.ToDeg() ) )
            if ( m_A.Bounds().Includes( star.pos ) )
            {
               star.flux.Initialize( m_dbInfo.wavelengths,
                                     FVector( reinterpret_cast<const float*>( p + sizeof( GaiaStarDataBase ) ),
                                              m_dbInfo.wavelengths.Length() ) );
               star.magG = g->magG;
               star.magBP = g->magBP;
               star.magRP = g->magRP;
               stars << star;
            }

         UPDATE_THREAD_MONITOR( 1 )
      }
   }

private:

   const AbstractImage::ThreadData& m_data;
   const AstrometricMetadata&       m_A;
   const ByteArray&                 m_fileData;
   const GaiaSPDatabaseInfo&        m_dbInfo;
         size_type                  m_start, m_end;

   using CoordinateReductionFunction = Vector (Position::*)( const StarPosition& );

   static CoordinateReductionFunction CoordinateReductionFunctionForReferenceSystem( const IsoString& refSys )
   {
      if ( unlikely( refSys != "ICRS" ) )
      {
         if ( refSys == "GCRS" )
            return &Position::Proper;

         IsoString r = refSys.Uppercase();
         if ( r == "TRUE" || r == "MEAN" || r == "APPARENT" || r == "GAPPT" )
            return &Position::Apparent;
      }
      return &Position::Astrometric;
   }
};

static Array<StarSPData> GaiaSPSearch( const DPoint& centerRD, double radius, const AstrometricMetadata& A,
                                       const GaiaSPDatabaseInfo& dbInfo, float magnitudeLow, float magnitudeHigh )
{
   Array<StarSPData> stars;
   String scriptFilePath = File::UniqueFileName( File::SystemTempDirectory(), 12, "GM_", ".js" );
   String binFilePath;
   try
   {
      File::WriteTextFile( scriptFilePath, IsoString().Format(
R"JS_SOURCE(var __PJSR_Gaia_SP_valid = false;
var __PJSR_Gaia_SP_outputFilePath = File.uniqueFileName( File.systemTempDirectory, 12/*randomLength*/, "Gaia_SP_"/*prefix*/, ".bin"/*postfix*/ );
var xpsd = new Gaia;
xpsd.command = "search";
xpsd.centerRA = %.8f;
xpsd.centerDec = %.8f;
xpsd.radius = %.8f;
xpsd.magnitudeLow = %.2f;
xpsd.magnitudeHigh = %.2f;
xpsd.exclusionFlags = 0x00000001; // GaiaFlag_NoPM
xpsd.verbosity = 2;
xpsd.dataRelease = Gaia.prototype.DataRelease_3_SP;
xpsd.sortBy = Gaia.prototype.SortBy_G;
xpsd.generateTextOutput = false;
xpsd.generateBinaryOutput = true;
xpsd.photonFluxUnits = true;
xpsd.outputFilePath = __PJSR_Gaia_SP_outputFilePath;
if ( xpsd.executeGlobal() )
   __PJSR_Gaia_SP_valid = xpsd.isValid;
)JS_SOURCE", centerRD.x, centerRD.y, radius, magnitudeLow, magnitudeHigh ) );

      Console().ExecuteScript( scriptFilePath );

      Variant result = Module->EvaluateScript( "__PJSR_Gaia_SP_valid" );
      if ( !result.IsValid() || !result.ToBoolean() )
         throw Error( "Invalid Gaia/SP XPSD server execution" );

      result = Module->EvaluateScript( "__PJSR_Gaia_SP_outputFilePath" );
      if ( !result.IsValid() )
         throw Error( "Invalid Gaia/SP XPSD server execution" );
      binFilePath = result.ToString();
      if ( binFilePath.IsEmpty() || !File::Exists( binFilePath ) )
         throw Error( "Invalid Gaia/SP XPSD server execution" );

      ByteArray fileData = File::ReadFile( binFilePath );

      const size_type fluxTableSize = dbInfo.wavelengths.Length() * sizeof( float );
      const size_type starDataSize = sizeof( GaiaStarDataBase ) + fluxTableSize;
      const size_type totalStars = (fileData.Length() - sizeof( GaiaBinaryHeader ))/starDataSize;

      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Extracting catalog star spectrum data", totalStars );

      Array<size_type> L = Thread::OptimalThreadLoads( totalStars, 16u/*overheadLimit*/ );
      ReferenceArray<GaiaSPStarExtractionThread> threads;
      AbstractImage::ThreadData data( monitor, totalStars );
      for ( size_type i = 0, n = 0; i < L.Length(); n += L[i++] )
         threads.Add( new GaiaSPStarExtractionThread( data, A, fileData, dbInfo, n, n + L[i] ) );
      AbstractImage::RunThreads( threads, data );
      for ( const GaiaSPStarExtractionThread& thread : threads )
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

SPInterpolation ParseSpectrumParameter( const String& csv, const String& name )
{
   Array<float> data = csv.ParseListOfFloat();
   if ( data.Length() & 1 )
      throw Error( "Parsing CSV spectrum parameter (" + name + "): Expected a numeric list of even length." );

   // Trim possible zero segments at the tails of the spectrum definition.
   int length = int( data.Length() >> 1 );
   int i0, i1, j;
   for ( i0 = 0, j = 1; i0 < length; ++i0, j += 2 )
      if ( 1 + data[j] != 1 )
         break;
   for ( i1 = length, j = int( data.Length()-1 ); i1 > i0; --i1, j -= 2 )
      if ( 1 + data[j] != 1 )
         break;
   if ( i0 > 0 )
      --i0;
   if ( i1 < length )
      ++i1;
   length = i1 - i0;
   if ( length < 5 )
      throw Error( "Parsing CSV spectrum parameter (" + name + "): At least 5 items are required, only " + String( length ) + " are available." );

   FVector wavelengths( length );
   FVector values( length );
   for ( int i = 0, j = i0 << 1; i < length; ++i )
   {
      wavelengths[i] = data[j++];
      values[i] = data[j++];
   }

   SPInterpolation sp;
   sp.Initialize( wavelengths, values );
   return sp;
}

// ----------------------------------------------------------------------------

struct PSFSample
{
   DPoint pos;
   DRect  bounds;
   float  angle; // deg
   float  signal[ 3 ]; // 0=L or 0=R,1=G,2=B
};

struct SpectrumSample
{
   double    value; // measured_flux/catalog_flux
   PSFSample star;
   float     magG;
   float     magBP;
   float     magRP;

   operator double() const
   {
      return value;
   }

   bool operator ==( const SpectrumSample& x ) const
   {
      return value == x.value;
   }

   bool operator <( const SpectrumSample& x ) const
   {
      return value < x.value;
   }
};

// ----------------------------------------------------------------------------

static Bitmap RenderStars( int width, int height, const Array<SpectrumSample>& samples )
{
   Bitmap bmp( width, height );
   bmp.Fill( 0 ); // transparent

   VectorGraphics G( bmp );
   G.EnableAntialiasing();
   G.SetPen( 0xff000000 ); // solid black

   for ( const SpectrumSample& sample : samples )
   {
      const PSFSample& s = sample.star;
      G.DrawLine( s.pos.x, s.pos.y-3, s.pos.x, s.pos.y-1 );
      G.DrawLine( s.pos.x, s.pos.y+1, s.pos.x, s.pos.y+3 );
      G.DrawLine( s.pos.x-3, s.pos.y, s.pos.x-1, s.pos.y );
      G.DrawLine( s.pos.x+1, s.pos.y, s.pos.x+3, s.pos.y );
      G.PushState();
      G.TranslateTransformation( s.pos.x, s.pos.y );
      G.RotateTransformation( Rad( s.angle ) );
      G.StrokeEllipse( s.bounds - s.pos );
      G.PopState();
   }

   G.EndPaint();

   return bmp;
}

void CreateStarMapWindow( int width, int height, const IsoString& id, const Array<SpectrumSample>& samples )
{
   ImageWindow window( 1, 1, 1, 8/*bitsPerSample*/, false/*floatSample*/, false/*color*/, true/*initialProcessing*/, id );
   if ( window.IsNull() )
      throw Error( "Unable to create image window: '" + id + '\'' );
   window.MainView().Image().AllocateData( width, height ).White().Blend( RenderStars( width, height, samples ) );
   window.Show();
}

bool SpectrophotometricFluxCalibrationInstance::ExecuteOn( View& view )
{
   {
      String whyNot;
      if ( !CanExecuteOn( view, whyNot ) )
         throw Error( whyNot );
   }

   Console console;
   console.EnableAbort();

   /*
    * Identify the spectrophotometric catalog.
    */
   if ( p_catalogId.CompareIC( "GaiaDR3SP" ) != 0 )
      if ( p_catalogId.CompareIC( "GaiaDR3" ) != 0 )
         throw Error( "Unknown/unsupported spectrophotometric catalog '" + p_catalogId + "'" );
   String catalogName = "Gaia DR3/SP";

   AutoViewLock lock( view );

   ImageWindow window = view.Window();

   /*
    * Astrometric solution.
    */
   AstrometricMetadata A( window );
   if ( !A.IsValid() )
      throw Error( "The image has no valid astrometric solution: " + view.Id() );

   /*
    * Find optimal saturation threshold.
    */
   ImageVariant image = view.Image();
   float saturationThreshold = p_saturationThreshold;
   if ( p_saturationRelative )
   {
      // Exclude border regions, which can contain outliers.
      int dx = TruncInt( image.Bounds().Width() * p_saturationShrinkFactor );
      int dy = TruncInt( image.Bounds().Height() * p_saturationShrinkFactor );
      Rect rect = image.Bounds().DeflatedBy( dx, dy );
      // For color images, the saturation limit is the smallest maximum among
      // the three channels.
      if ( image.IsColor() )
         saturationThreshold *= Min( Min( image.MaximumSampleValue( rect, 0, 0 ),
                                          image.MaximumSampleValue( rect, 1, 1 ) ),
                                          image.MaximumSampleValue( rect, 2, 2 ) );
      else
         saturationThreshold *= image.MaximumSampleValue( rect, 0, 0 );

      console.WriteLn( String().Format( "<end><cbr><br>Saturation threshold: %.4f", saturationThreshold ) );
   }

   /*
    * Configure PSF signal evaluation.
    */
   PSFEstimator E;
   E.Detector().SetStructureLayers( p_structureLayers );
   E.Detector().SetNoiseLayers( p_psfNoiseLayers );
   E.Detector().SetHotPixelFilterRadius( p_psfHotPixelFilterRadius );
   E.Detector().SetNoiseReductionFilterRadius( p_psfNoiseReductionFilterRadius );
   E.Detector().SetMinStructureSize( p_psfMinStructureSize );
   E.Detector().SetMinSNR( p_psfMinSNR );
   E.Detector().EnableClusteredSources( p_psfAllowClusteredSources );
   E.SetPSFType( SPFCPSFType::ToPSFFunction( p_psfType ) );
   E.SetSaturationThreshold( saturationThreshold );
   E.DisableRelativeSaturation();
   E.SetGrowthFactorForFluxMeasurement( p_psfGrowth );
   E.SetMaxStars( p_psfMaxStars );

   /*
    * Get PSF signal samples.
    */
   console.WriteLn( "<end><cbr><br>* Extracting PSF signal samples" );
   Module->ProcessEvents();

   /*
    * Get XPSD database information.
    */
   console.WriteLn( "<end><cbr><br>* Retrieving " + catalogName + " database information" );
   Module->ProcessEvents();

   GaiaSPDatabaseInfo dbInfo = DatabaseInfoFromGaiaXPSDServer( 4/*Gaia.DataRelease_3_SP*/ );

   if ( dbInfo.filePaths.IsEmpty() )
      throw Error( "Database files not available for the " + catalogName + " catalog, or the XPSD server is not properly configured." );

   if ( dbInfo.wavelengths.IsEmpty() )
      throw Error( "Invalid " + catalogName + " XPSD server execution: Couldn't retrieve a valid spectrum wavelength table." );

   console.WriteLn( String().Format( "Data release .......... %s\n"
                                     "Database files ........ %u\n"
                                     "Magnitude range ....... [%.2f,%.2f]\n"
                                     "Wavelength range ...... [%.2f,%.2f] nm\n"
                                     "Spectrum step ......... %.2f nm\n"
                                     "Spectrum count ........ %d\n"
                                     "Spectrum resolution ... %d bits",
                                     GaiaDataReleaseNameFromParameterValue( dbInfo.dataRelease ),
                                     unsigned( dbInfo.filePaths.Length() ),
                                     dbInfo.magnitudeLow, dbInfo.magnitudeHigh,
                                     dbInfo.wavelengths[0], dbInfo.wavelengths[dbInfo.spectrumCount-1],
                                     dbInfo.spectrumStep, dbInfo.spectrumCount, dbInfo.spectrumBits ) );
   Module->ProcessEvents();

   /*
    * Perform photometry.
    */
   Array<PSFSample> psfSamples;
   {
      volatile AutoStatusCallbackRestorer saveStatus( image.Status() );
      StandardStatus status;
      image.SetStatusCallback( &status );

      if ( image.IsColor() )
      {
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
                  PSFSample s;
                  s.pos = p1.c0;
                  s.bounds = p1.FWTMBounds( p_psfGrowth );
                  s.angle = p1.theta;
                  s.signal[0] = P0[0].signal;
                  s.signal[1] = p1.signal;
                  s.signal[2] = P2[0].signal;
                  psfSamples << s;
               }
            }
         }
      }
      else
      {
         Array<PSFData> psfData;
         image.SelectChannel( 0 );
         psfData = E.FitStars( image );
         if ( psfData.IsEmpty() )
            throw Error( "No stars found" );
         image.ResetSelections();

         for ( const PSFData& p : psfData )
         {
            PSFSample s;
            s.pos = p.c0;
            s.bounds = p.FWTMBounds();
            s.angle = p.theta;
            s.signal[0] = s.signal[1] = s.signal[2] = p.signal;
            psfSamples << s;
         }
      }
   }

   /*
    * Quantum efficiency curve.
    */
   SPInterpolation deviceQECurve = ParseSpectrumParameter( p_deviceQECurve, "device QE curve" );

   /*
    * Broadband filter curve interpolations.
    */
   SPInterpolation filters[ 3 ]; // L | R,G,B
   if ( image.IsColor() )
   {
      filters[0] = ParseSpectrumParameter( p_redFilterTrCurve, "red filter" );
      filters[1] = ParseSpectrumParameter( p_greenFilterTrCurve, "green filter" );
      filters[2] = ParseSpectrumParameter( p_blueFilterTrCurve, "blue filter" );
   }
   else
      filters[0] = ParseSpectrumParameter( p_grayFilterTrCurve, "gray filter" );

   /*
    * Narrowband filter parameters.
    */
   double wavelengths[ 3 ] = {}, bandwidths[ 3 ] = {}; // L,R,G,B
   if ( image.IsColor() )
   {
      wavelengths[0] = p_redFilterWavelength;
      bandwidths[0] = p_redFilterBandwidth;
      wavelengths[1] = p_greenFilterWavelength;
      bandwidths[1] = p_greenFilterBandwidth;
      wavelengths[2] = p_blueFilterWavelength;
      bandwidths[2] = p_blueFilterBandwidth;
   }
   else
   {
      wavelengths[0] = p_grayFilterWavelength;
      bandwidths[0] = p_grayFilterBandwidth;
   }

   /*
    * Perform XPSD database search.
    */
   console.WriteLn( "<end><cbr><br>* Searching catalog spectrum sources" );
   Module->ProcessEvents();

   // Search center and radius.
   DRect bounds = view.Bounds();
   DPoint centerRD;
   if ( !A.ImageToCelestial( centerRD, bounds.Center() ) )
      throw Error( "Invalid astrometric solution: unable to compute celestial coordinates for the center of the image." );
   double radius = A.SearchRadius();

   // Limit magnitude
   float magnitudeHigh = p_autoLimitMagnitude ? GaiaSPAutoLimitMagnitude( centerRD, radius, psfSamples.Length() ) : p_limitMagnitude;
   float magnitudeLow = Min( p_minMagnitude, magnitudeHigh );
   console.WriteLn( String().Format( "<end><cbr>Magnitude range: %.2f -> %.2f", magnitudeLow, magnitudeHigh ) );

   Array<StarSPData> catalogStars = GaiaSPSearch( centerRD, radius, A, dbInfo, magnitudeLow, magnitudeHigh );
   if ( catalogStars.IsEmpty() )
      throw Error( "No catalog stars found" );

   console.WriteLn( String().Format( "<end><cbr><br>%u catalog sources found.", catalogStars.Length() ) );
   Module->ProcessEvents();

   // Search tree
   QuadTree<StarSPData> T( catalogStars );
   
   /*
    * Spectrum samples.
    */
   Array<Array<SpectrumSample>> samples( image.NumberOfNominalChannels() );
   for ( const PSFSample& psf : psfSamples )
   {
      // Match coordinates by proximity search
      QuadTree<StarSPData>::point_list P =
         T.Search( QuadTree<StarSPData>::rectangle( psf.pos.x - p_psfSearchTolerance, psf.pos.y - p_psfSearchTolerance,
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

         for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
            if ( 1 + psf.signal[c] != 1 )
            {
               double catalogFlux = 0;
               double curveArea = 0;
               if ( p_narrowbandMode )
               {
                  float l0 = wavelengths[c] - bandwidths[c]/2;
                  float l1 = wavelengths[c] + bandwidths[c]/2;
                  float dl = (l1 - l0)/p_narrowbandIntegrationSteps;
                  for ( int i = 1;; ++i )
                  {
                     double la = l0 + (i - 1)*dl;
                     double lb = l0 + i*dl;
                     double lm = (la + lb)/2;

                     double ka = deviceQECurve( la );
                     double kb = deviceQECurve( lb );
                     double km = deviceQECurve( lm );
                     curveArea += (ka + kb + 4*km)*(lb - la)/6;

                     double fa = ka * P[j].flux( la );
                     double fb = kb * P[j].flux( lb );
                     double fm = km * P[j].flux( lm );
                     catalogFlux += (fa + fb + 4*fm)*(lb - la)/6;
                     if ( lb >= l1 )
                        break;
                  }
               }
               else
               {
                  const SPInterpolation& filter = filters[c];
                  double l0 = filter.X()[0];
                  double l1 = filter.X()[filter.X().Length()-1];

                  for ( int i = 1;; ++i )
                  {
                     double la = l0 + (i - 1)*p_broadbandIntegrationStepSize;
                     double lb = l0 + i*p_broadbandIntegrationStepSize;
                     double lm = (la + lb)/2;

                     double ka = filter( la ) * deviceQECurve( la );
                     double kb = filter( lb ) * deviceQECurve( lb );
                     double km = filter( lm ) * deviceQECurve( lm );
                     curveArea += (ka + kb + 4*km)*(lb - la)/6;

                     double fa = ka * P[j].flux( la );
                     double fb = kb * P[j].flux( lb );
                     double fm = km * P[j].flux( lm );
                     catalogFlux += (fa + fb + 4*fm)*(lb - la)/6;

                     if ( lb >= l1 )
                        break;
                  }
               }

               catalogFlux /= curveArea;

               if ( 1 + catalogFlux != 1 )
               {
                  SpectrumSample sample;
                  sample.value = (SPFC_NORMALIZATION_FACTOR * psf.signal[c]) / catalogFlux;
                  sample.star = psf;
                  sample.magG = P[j].magG;
                  sample.magBP = P[j].magBP;
                  sample.magRP = P[j].magRP;
                  samples[c] << sample;
               }
            }
      }
   }

   /*
    * Scale factors / sigmas / counts.
    */
   DVector K( image.NumberOfNominalChannels() );
   DVector S( image.NumberOfNominalChannels() );
   IVector N( image.NumberOfNominalChannels() );
   for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
   {
      if ( samples[c].Length() < 5 )
         throw Error( "Insufficient data: only " + String( samples[c].Length() ) + " sample(s) are available; at least 5 are required." );

      /*
       * Perform the outlier rejection. The samples[c] container will be sorted
       * in ascending order as a side-effect of RCR. We need them sorted anyway
       * for graph generation.
       */
      double mean, sigma;
      int i, j;
      RobustChauvenetRejection R;
      /*
       * ### N.B. Should we detect a performance issue with large data sets in
       * the RCR routine, we can force bulk rejection to accelerate the process
       * by uncommenting the following line - with an accuracy loss, of course.
       */
      // R.SetLargeSampleSize( 1 );
      R.SetRejectionLimit( p_rejectionLimit );
      R.PerformRejection( i, j, mean, sigma, samples[c] );
      samples[c].RemoveLast( samples[c].Length() - j );
      samples[c].RemoveFirst( i );
      K[c] = mean;
      S[c] = sigma;
      N[c] = j - i;
   }

   if ( p_generateStarMaps )
      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
         CreateStarMapWindow( image.Width(), image.Height(), IsoString().Format( "SPFC_stars_%d", c ), samples[c] );

   if ( p_generateTextFiles )
      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         IsoString text = "x,y,alpha,delta,mag_G,mag_BP,mag_RP,img_signal,scale\n";
         for ( const SpectrumSample& sample : samples[c] )
         {
            DPoint posRD = 0;
            A.ImageToCelestial( posRD, sample.star.pos );
            text.AppendFormat( "%.2f,%.2f,%.7f,%.7f,%.2f,%.2f,%.2f,%.6e,%.4e\n",
                               sample.star.pos.x, sample.star.pos.y, posRD.x, posRD.y,
                               sample.magG, sample.magBP, sample.magRP,
                               sample.star.signal[c], sample.value );
         }

         String outputDir = File::WindowsPathToUnix( p_outputDirectory.Trimmed() );
         if ( outputDir.IsEmpty() )
            outputDir = File::SystemTempDirectory();
         if ( !outputDir.EndsWith( '/' ) )
            outputDir << '/';
         String filePath = outputDir + String().Format( "SPFC-data-%d.csv", c );
         File::WriteTextFile( filePath, text );
         console.WriteLn( "<end><cbr>Text file generated: <raw>" + filePath + "</raw>" );
      }

   if ( p_generateGraphs )
   {
      Array<DVector> sampleValues( 3 );
      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         sampleValues[c] = DVector( int( samples[c].Length() ) );
         for ( size_type i = 0; i < samples[c].Length(); ++i )
            sampleValues[c][i] = samples[c][i].value;
      }

      String grayFilterName, redFilterName, greenFilterName, blueFilterName;
      if ( image.IsColor() ) {
         if ( p_narrowbandMode )
         {
            redFilterName   = String().Format( "&lambda; = %.2f nm, B = %.2f nm", p_redFilterWavelength, p_redFilterBandwidth );
            greenFilterName = String().Format( "&lambda; = %.2f nm, B = %.2f nm", p_greenFilterWavelength, p_greenFilterBandwidth );
            blueFilterName  = String().Format( "&lambda; = %.2f nm, B = %.2f nm", p_blueFilterWavelength, p_blueFilterBandwidth );
         }
         else
         {
            redFilterName   = p_redFilterName;
            greenFilterName = p_greenFilterName;
            blueFilterName  = p_blueFilterName;
         }
      } 
      else 
      {
          if ( p_narrowbandMode )
          {
              grayFilterName = String().Format( "&lambda; = %.2f nm, B = %.2f nm", p_grayFilterWavelength, p_grayFilterBandwidth );
           }
          else
          {
              grayFilterName = p_grayFilterName;
          }
      }
      if ( !TheSpectrophotometricFluxCalibrationGraphInterface->IsVisible() )
         TheSpectrophotometricFluxCalibrationGraphInterface->Launch();
      TheSpectrophotometricFluxCalibrationGraphInterface->UpdateGraphs( view.FullId(), catalogName,
                                    grayFilterName, redFilterName, greenFilterName, blueFilterName,
                                    p_deviceQECurveName,
                                    sampleValues[0], sampleValues[1], sampleValues[2],
                                    K, S );
   }

   console.NoteLn( "<end><cbr><br>* Scale factors:" );
   int nn = 0;
   for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      nn = pcl::Max( nn, int( Log( N[c] ) ) );
   for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      console.NoteLn( String().Format( "K_%d : %.4e | sigma_%d : %.4e | N_%d : %*d", c, K[c], c, S[c], c, 1+nn, N[c] ) );

   /*
    * Metadata generation.
    */
   window.MainView().SetStorablePermanentPropertyValue( "PCL:SPFC:ScaleFactors", K );
   window.MainView().SetStorablePermanentPropertyValue( "PCL:SPFC:Sigmas", S );
   window.MainView().SetStorablePermanentPropertyValue( "PCL:SPFC:Counts", N );
   window.MainView().SetStorablePermanentPropertyValue( "PCL:SPFC:NormalizationFactor", SPFC_NORMALIZATION_FACTOR );
   window.MainView().SetStorablePermanentPropertyValue( "PCL:SPFC:Version", TheSpectrophotometricFluxCalibrationProcess->Version() );

   return true;
}

// ----------------------------------------------------------------------------

void* SpectrophotometricFluxCalibrationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSPFCNarrowbandModeParameter )
      return &p_narrowbandMode;

   if ( p == TheSPFCGrayFilterTrCurveParameter )
      return p_grayFilterTrCurve.Begin();
   if ( p == TheSPFCGrayFilterNameParameter )
      return p_grayFilterName.Begin();
   if ( p == TheSPFCRedFilterTrCurveParameter )
      return p_redFilterTrCurve.Begin();
   if ( p == TheSPFCRedFilterNameParameter )
      return p_redFilterName.Begin();
   if ( p == TheSPFCGreenFilterTrCurveParameter )
      return p_greenFilterTrCurve.Begin();
   if ( p == TheSPFCGreenFilterNameParameter )
      return p_greenFilterName.Begin();
   if ( p == TheSPFCBlueFilterTrCurveParameter )
      return p_blueFilterTrCurve.Begin();
   if ( p == TheSPFCBlueFilterNameParameter )
      return p_blueFilterName.Begin();

   if ( p == TheSPFCGrayFilterWavelengthParameter )
      return &p_grayFilterWavelength;
   if ( p == TheSPFCGrayFilterBandwidthParameter )
      return &p_grayFilterBandwidth;
   if ( p == TheSPFCRedFilterWavelengthParameter )
      return &p_redFilterWavelength;
   if ( p == TheSPFCRedFilterBandwidthParameter )
      return &p_redFilterBandwidth;
   if ( p == TheSPFCGreenFilterWavelengthParameter )
      return &p_greenFilterWavelength;
   if ( p == TheSPFCGreenFilterBandwidthParameter )
      return &p_greenFilterBandwidth;
   if ( p == TheSPFCBlueFilterWavelengthParameter )
      return &p_blueFilterWavelength;
   if ( p == TheSPFCBlueFilterBandwidthParameter )
      return &p_blueFilterBandwidth;

   if ( p == TheSPFCDeviceQECurveParameter )
      return p_deviceQECurve.Begin();
   if ( p == TheSPFCDeviceQECurveNameParameter )
      return p_deviceQECurveName.Begin();

   if ( p == TheSPFCBroadbandIntegrationStepSizeParameter )
      return &p_broadbandIntegrationStepSize;
   if ( p == TheSPFCNarrowbandIntegrationStepsParameter )
      return &p_narrowbandIntegrationSteps;
   if ( p == TheSPFCRejectionLimitParameter )
      return &p_rejectionLimit;

   if ( p == TheSPFCCatalogIdParameter )
      return p_catalogId.Begin();
   if ( p == TheSPFCMinMagnitudeParameter )
      return &p_minMagnitude;
   if ( p == TheSPFCLimitMagnitudeParameter )
      return &p_limitMagnitude;
   if ( p == TheSPFCAutoLimitMagnitudeParameter )
      return &p_autoLimitMagnitude;

   if ( p == TheSPFCStructureLayersParameter )
      return &p_structureLayers;
   if ( p == TheSPFCSaturationThresholdParameter )
      return &p_saturationThreshold;
   if ( p == TheSPFCSaturationRelativeParameter )
      return &p_saturationRelative;
   if ( p == TheSPFCSaturationShrinkFactorParameter )
      return &p_saturationShrinkFactor;
   if ( p == TheSPFCPSFNoiseLayersParameter )
      return &p_psfNoiseLayers;
   if ( p == TheSPFCPSFHotPixelFilterRadiusParameter )
      return &p_psfHotPixelFilterRadius;
   if ( p == TheSPFCPSFNoiseReductionFilterRadiusParameter )
      return &p_psfNoiseReductionFilterRadius;
   if ( p == TheSPFCPSFMinStructureSizeParameter )
      return &p_psfMinStructureSize;
   if ( p == TheSPFCPSFMinSNRParameter )
      return &p_psfMinSNR;
   if ( p == TheSPFCPSFAllowClusteredSourcesParameter )
      return &p_psfAllowClusteredSources;
   if ( p == TheSPFCPSFTypeParameter )
      return &p_psfType;
   if ( p == TheSPFCPSFGrowthParameter )
      return &p_psfGrowth;
   if ( p == TheSPFCPSFMaxStarsParameter )
      return &p_psfMaxStars;
   if ( p == TheSPFCPSFSearchToleranceParameter )
      return &p_psfSearchTolerance;
   if ( p == TheSPFCPSFChannelSearchToleranceParameter )
      return &p_psfChannelSearchTolerance;

   if ( p == TheSPFCGenerateGraphsParameter )
      return &p_generateGraphs;
   if ( p == TheSPFCGenerateStarMapsParameter )
      return &p_generateStarMaps;
   if ( p == TheSPFCGenerateTextFilesParameter )
      return &p_generateTextFiles;
   if ( p == TheSPFCOutputDirectoryParameter )
      return p_outputDirectory.Begin();

   return nullptr;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricFluxCalibrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSPFCGrayFilterTrCurveParameter )
   {
      p_grayFilterTrCurve.Clear();
      if ( sizeOrLength > 0 )
         p_grayFilterTrCurve.SetLength( sizeOrLength );
   }
   else if ( p == TheSPFCGrayFilterNameParameter )
   {
      p_grayFilterName.Clear();
      if ( sizeOrLength > 0 )
         p_grayFilterName.SetLength( sizeOrLength );
   }
   else if ( p == TheSPFCRedFilterTrCurveParameter )
   {
      p_redFilterTrCurve.Clear();
      if ( sizeOrLength > 0 )
         p_redFilterTrCurve.SetLength( sizeOrLength );
   }
   else if ( p == TheSPFCRedFilterNameParameter )
   {
      p_redFilterName.Clear();
      if ( sizeOrLength > 0 )
         p_redFilterName.SetLength( sizeOrLength );
   }
   else if ( p == TheSPFCGreenFilterTrCurveParameter )
   {
      p_greenFilterTrCurve.Clear();
      if ( sizeOrLength > 0 )
         p_greenFilterTrCurve.SetLength( sizeOrLength );
   }
   else if ( p == TheSPFCGreenFilterNameParameter )
   {
      p_greenFilterName.Clear();
      if ( sizeOrLength > 0 )
         p_greenFilterName.SetLength( sizeOrLength );
   }
   else if ( p == TheSPFCBlueFilterTrCurveParameter )
   {
      p_blueFilterTrCurve.Clear();
      if ( sizeOrLength > 0 )
         p_blueFilterTrCurve.SetLength( sizeOrLength );
   }
   else if ( p == TheSPFCBlueFilterNameParameter )
   {
      p_blueFilterName.Clear();
      if ( sizeOrLength > 0 )
         p_blueFilterName.SetLength( sizeOrLength );
   }
   else if ( p == TheSPFCDeviceQECurveParameter )
   {
      p_deviceQECurve.Clear();
      if ( sizeOrLength > 0 )
         p_deviceQECurve.SetLength( sizeOrLength );
   }
   else if ( p == TheSPFCDeviceQECurveNameParameter )
   {
      p_deviceQECurveName.Clear();
      if ( sizeOrLength > 0 )
         p_deviceQECurveName.SetLength( sizeOrLength );
   }
   else if ( p == TheSPFCCatalogIdParameter )
   {
      p_catalogId.Clear();
      if ( sizeOrLength > 0 )
         p_catalogId.SetLength( sizeOrLength );
   }
   else if ( p == TheSPFCOutputDirectoryParameter )
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

size_type SpectrophotometricFluxCalibrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheSPFCGrayFilterTrCurveParameter )
      return p_grayFilterTrCurve.Length();
   if ( p == TheSPFCGrayFilterNameParameter )
      return p_grayFilterName.Length();
   if ( p == TheSPFCRedFilterTrCurveParameter )
      return p_redFilterTrCurve.Length();
   if ( p == TheSPFCRedFilterNameParameter )
      return p_redFilterName.Length();
   if ( p == TheSPFCGreenFilterTrCurveParameter )
      return p_greenFilterTrCurve.Length();
   if ( p == TheSPFCGreenFilterNameParameter )
      return p_greenFilterName.Length();
   if ( p == TheSPFCBlueFilterTrCurveParameter )
      return p_blueFilterTrCurve.Length();
   if ( p == TheSPFCBlueFilterNameParameter )
      return p_blueFilterName.Length();
   if ( p == TheSPFCDeviceQECurveParameter )
      return p_deviceQECurve.Length();
   if ( p == TheSPFCDeviceQECurveNameParameter )
      return p_deviceQECurveName.Length();
   if ( p == TheSPFCCatalogIdParameter )
      return p_catalogId.Length();
   if ( p == TheSPFCOutputDirectoryParameter )
      return p_outputDirectory.Length();

   return 0;
}

// ----------------------------------------------------------------------------

void SpectrophotometricFluxCalibrationInstance::SetDefaultSpectrumData()
{
   if ( API )
      if ( !TheSpectraDatabase.IsValid() )
         TheSpectraDatabase.Initialize();

   if ( TheSpectraDatabase.IsValid() )
   {
      p_grayFilterTrCurve = TheSpectraDatabase.DefaultLuminanceFilter().data;
      p_grayFilterName = TheSpectraDatabase.DefaultLuminanceFilter().name;
      p_redFilterTrCurve = TheSpectraDatabase.DefaultRedFilter().data;
      p_redFilterName = TheSpectraDatabase.DefaultRedFilter().name;
      p_greenFilterTrCurve = TheSpectraDatabase.DefaultGreenFilter().data;
      p_greenFilterName = TheSpectraDatabase.DefaultGreenFilter().name;
      p_blueFilterTrCurve = TheSpectraDatabase.DefaultBlueFilter().data;
      p_blueFilterName = TheSpectraDatabase.DefaultBlueFilter().name;
      p_deviceQECurve = TheSpectraDatabase.DefaultDeviceQE().data;
      p_deviceQECurveName = TheSpectraDatabase.DefaultDeviceQE().name;
   }
   else
   {
      p_grayFilterTrCurve.Clear();
      p_grayFilterName.Clear();
      p_redFilterTrCurve.Clear();
      p_redFilterName.Clear();
      p_greenFilterTrCurve.Clear();
      p_greenFilterName.Clear();
      p_blueFilterTrCurve.Clear();
      p_blueFilterName.Clear();
      p_deviceQECurve.Clear();
      p_deviceQECurveName.Clear();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SpectrophotometricFluxCalibrationInstance.cpp - Released 2024-08-02T18:17:27Z
