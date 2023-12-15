//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// SpectrophotometricColorCalibrationInstance.cpp - Released 2023-12-01T19:16:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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
#include "SpectrophotometricColorCalibrationGraphInterface.h"
#include "SpectrophotometricColorCalibrationInstance.h"
#include "SpectrophotometricColorCalibrationParameters.h"
#include "SpectrophotometricColorCalibrationProcess.h"

#include "../Gaia/GaiaInstance.h" // GaiaBinaryHeader

#include <pcl/AkimaInterpolation.h>
#include <pcl/AstrometricMetadata.h>
#include <pcl/AutoStatusCallbackRestorer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/GaiaDatabaseFile.h>
#include <pcl/ImageWindow.h>
#include <pcl/LinearFit.h>
#include <pcl/MetaModule.h>
#include <pcl/PSFEstimator.h>
#include <pcl/Position.h>
#include <pcl/StandardStatus.h>
#include <pcl/Version.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SpectrophotometricColorCalibrationInstance::SpectrophotometricColorCalibrationInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_applyCalibration( TheSPCCApplyCalibrationParameter->DefaultValue() )
   , p_narrowbandMode( TheSPCCNarrowbandModeParameter->DefaultValue() )
   , p_narrowbandOptimizeStars( TheSPCCNarrowbandOptimizeStarsParameter->DefaultValue() )
   , p_redFilterWavelength( TheSPCCRedFilterWavelengthParameter->DefaultValue() )
   , p_redFilterBandwidth( TheSPCCRedFilterBandwidthParameter->DefaultValue() )
   , p_greenFilterWavelength( TheSPCCGreenFilterWavelengthParameter->DefaultValue() )
   , p_greenFilterBandwidth( TheSPCCGreenFilterBandwidthParameter->DefaultValue() )
   , p_blueFilterWavelength( TheSPCCBlueFilterWavelengthParameter->DefaultValue() )
   , p_blueFilterBandwidth( TheSPCCBlueFilterBandwidthParameter->DefaultValue() )
   , p_broadbandIntegrationStepSize( TheSPCCBroadbandIntegrationStepSizeParameter->DefaultValue() )
   , p_narrowbandIntegrationSteps( TheSPCCNarrowbandIntegrationStepsParameter->DefaultValue() )
   , p_catalogId( TheSPCCCatalogIdParameter->DefaultValue() )
   , p_limitMagnitude( TheSPCCLimitMagnitudeParameter->DefaultValue() )
   , p_autoLimitMagnitude( TheSPCCAutoLimitMagnitudeParameter->DefaultValue() )
   , p_targetSourceCount( TheSPCCTargetSourceCountParameter->DefaultValue() )
   , p_structureLayers( TheSPCCStructureLayersParameter->DefaultValue() )
   , p_saturationThreshold( TheSPCCSaturationThresholdParameter->DefaultValue() )
   , p_saturationRelative( TheSPCCSaturationRelativeParameter->DefaultValue() )
   , p_saturationShrinkFactor( TheSPCCSaturationShrinkFactorParameter->DefaultValue() )
   , p_psfNoiseLayers( TheSPCCPSFNoiseLayersParameter->DefaultValue() )
   , p_psfHotPixelFilterRadius( TheSPCCPSFHotPixelFilterRadiusParameter->DefaultValue() )
   , p_psfNoiseReductionFilterRadius( TheSPCCPSFNoiseReductionFilterRadiusParameter->DefaultValue() )
   , p_psfMinStructureSize( TheSPCCPSFMinStructureSizeParameter->DefaultValue() )
   , p_psfMinSNR( TheSPCCPSFMinSNRParameter->DefaultValue() )
   , p_psfAllowClusteredSources( TheSPCCPSFAllowClusteredSourcesParameter->DefaultValue() )
   , p_psfType( SPCCPSFType::Default )
   , p_psfGrowth( TheSPCCPSFGrowthParameter->DefaultValue() )
   , p_psfMaxStars( TheSPCCPSFMaxStarsParameter->DefaultValue() )
   , p_psfSearchTolerance( TheSPCCPSFSearchToleranceParameter->DefaultValue() )
   , p_psfChannelSearchTolerance( TheSPCCPSFChannelSearchToleranceParameter->DefaultValue() )
   , p_neutralizeBackground( TheSPCCNeutralizeBackgroundParameter->DefaultValue() )
   , p_backgroundLow( TheSPCCBackgroundLowParameter->DefaultValue() )
   , p_backgroundHigh( TheSPCCBackgroundHighParameter->DefaultValue() )
   , p_backgroundUseROI( TheSPCCBackgroundUseROIParameter->DefaultValue() )
   , p_generateGraphs( TheSPCCGenerateGraphsParameter->DefaultValue() )
   , p_generateStarMaps( TheSPCCGenerateStarMapsParameter->DefaultValue() )
   , p_generateTextFiles( TheSPCCGenerateTextFilesParameter->DefaultValue() )
   , p_outputDirectory( TheSPCCOutputDirectoryParameter->DefaultValue() )
{
   SetDefaultSpectrumData();
}

// ----------------------------------------------------------------------------

SpectrophotometricColorCalibrationInstance::SpectrophotometricColorCalibrationInstance( const SpectrophotometricColorCalibrationInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInstance::Assign( const ProcessImplementation& p )
{
   const SpectrophotometricColorCalibrationInstance* x = dynamic_cast<const SpectrophotometricColorCalibrationInstance*>( &p );
   if ( x != nullptr )
   {
      p_applyCalibration = x->p_applyCalibration;
      p_narrowbandMode = x->p_narrowbandMode;
      p_narrowbandOptimizeStars = x->p_narrowbandOptimizeStars;
      p_whiteReferenceSpectrum = x->p_whiteReferenceSpectrum;
      p_whiteReferenceName = x->p_whiteReferenceName;
      p_redFilterTrCurve = x->p_redFilterTrCurve;
      p_redFilterName = x->p_redFilterName;
      p_greenFilterTrCurve = x->p_greenFilterTrCurve;
      p_greenFilterName = x->p_greenFilterName;
      p_blueFilterTrCurve = x->p_blueFilterTrCurve;
      p_blueFilterName = x->p_blueFilterName;
      p_redFilterWavelength = x->p_redFilterWavelength;
      p_redFilterBandwidth = x->p_redFilterBandwidth;
      p_greenFilterWavelength = x->p_greenFilterWavelength;
      p_greenFilterBandwidth = x->p_greenFilterBandwidth;
      p_blueFilterWavelength = x->p_blueFilterWavelength;
      p_blueFilterBandwidth = x->p_blueFilterBandwidth;
      p_deviceQECurve = x->p_deviceQECurve;
      p_deviceQECurveName = x->p_deviceQECurveName;
      p_broadbandIntegrationStepSize = x->p_broadbandIntegrationStepSize;
      p_narrowbandIntegrationSteps = x->p_narrowbandIntegrationSteps;
      p_catalogId = x->p_catalogId;
      p_limitMagnitude = x->p_limitMagnitude;
      p_autoLimitMagnitude = x->p_autoLimitMagnitude;
      p_targetSourceCount = x->p_targetSourceCount;
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
      p_backgroundLow = x->p_backgroundLow;
      p_backgroundHigh = x->p_backgroundHigh;
      p_backgroundUseROI = x->p_backgroundUseROI;
      p_backgroundROI = x->p_backgroundROI;
      p_generateGraphs = x->p_generateGraphs;
      p_generateStarMaps = x->p_generateStarMaps;
      p_generateTextFiles = x->p_generateTextFiles;
      p_outputDirectory = x->p_outputDirectory;
   }
}

// ----------------------------------------------------------------------------

UndoFlags SpectrophotometricColorCalibrationInstance::UndoMode( const View& ) const
{
   return p_applyCalibration ? UndoFlag::Keywords | UndoFlag::PixelData
                             : UndoFlag::Keywords;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricColorCalibrationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.IsPreview() )
   {
      whyNot = "SpectrophotometricColorCalibration cannot be executed on previews.";
      return false;
   }
   if ( !view.IsColor() )
   {
      whyNot = "SpectrophotometricColorCalibration can only be executed on color images.";
      return false;
   }
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "SpectrophotometricColorCalibration cannot be executed on complex images.";
      return false;
   }
   if ( p_whiteReferenceSpectrum.Trimmed().IsEmpty() )
   {
      whyNot = "No white reference spectrum has been defined.";
      return false;
   }
   if ( p_redFilterTrCurve.Trimmed().IsEmpty() )
   {
      whyNot = "No red filter transmission curve has been defined.";
      return false;
   }
   if ( p_greenFilterTrCurve.Trimmed().IsEmpty() )
   {
      whyNot = "No green filter transmission curve has been defined.";
      return false;
   }
   if ( p_blueFilterTrCurve.Trimmed().IsEmpty() )
   {
      whyNot = "No blue filter transmission curve has been defined.";
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
   String scriptPath = File::UniqueFileName( File::SystemTempDirectory(), 12, "SPCC_", ".js" );
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

static float GaiaSPAutoLimitMagnitude( const DPoint& centerRD, double radius, const AstrometricMetadata& A, int targetCount )
{
   float magnitudeHigh = -1;
   String scriptFilePath = File::UniqueFileName( File::SystemTempDirectory(), 12, "SPCC_", ".js" );

   try
   {
      File::WriteTextFile( scriptFilePath, IsoString().Format(
R"JS_SOURCE(var __PJSR_Gaia_SP_limitMagnitude = -1;
console.writeln( "<end><cbr><br>* Searching for optimal magnitude limit..." );
let xpsd = ((typeof Gaia) != 'undefined') ? (new Gaia) : null;
if ( xpsd )
{
   xpsd.command = "get-info";
   xpsd.dataRelease = Gaia.prototype.DataRelease_3_SP;
   xpsd.executeGlobal();
   if ( xpsd.isValid )
   {
      let magHigh = xpsd.databaseMagnitudeHigh;
      let m = 10;

      xpsd.command = "search";
      xpsd.centerRA = %.8f;
      xpsd.centerDec = %.8f;
      xpsd.radius = %.8f;
      xpsd.magnitudeLow = -1.5;
      xpsd.sourceLimit = 0; // do not retrieve objects, just count them.
      xpsd.exclusionFlags = 0x00000001; // GaiaFlag_NoPM
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
      __PJSR_Gaia_SP_limitMagnitude = m;
   }
} )JS_SOURCE", centerRD.x, centerRD.y, radius, TruncInt( 0.875*targetCount ), targetCount ) );

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
   DPoint          pos;  // image coordinates
   SPInterpolation flux; // interpolate the normalized sampled spectrum

   using component = DPoint::component;

   component operator []( int i ) const noexcept // QuadTree-compatible
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
         if ( refSys == "GAPPT" )
            return &Position::Apparent;
      }
      return &Position::Astrometric;
   }
};

static Array<StarSPData> GaiaSPSearch( const DPoint& centerRD, double radius, const AstrometricMetadata& A,
                                       const GaiaSPDatabaseInfo& dbInfo, float magnitudeHigh )
{
   Array<StarSPData> stars;
   String scriptFilePath = File::UniqueFileName( File::SystemTempDirectory(), 12, "SPCC_", ".js" );
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
xpsd.magnitudeLow = -1.5;
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
)JS_SOURCE", centerRD.x, centerRD.y, radius, magnitudeHigh ) );

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
   int length = int( data.Length() >> 1 );
   if ( length < 5 )
      throw Error( "Parsing CSV spectrum parameter (" + name + "): At least 5 items are required, only " + String( length ) + " are available." );

   FVector wavelengths( length );
   FVector values( length );
   for ( int i = 0, j = 0; i < length; ++i )
   {
      wavelengths[i] = data[j++];
      values[i] = data[j++];
   }

   SPInterpolation sp;
   sp.Initialize( wavelengths, values );
   return sp;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricColorCalibrationInstance::ExecuteOn( View& view )
{
   {
      String whyNot;
      if ( !CanExecuteOn( view, whyNot ) )
         throw Error( whyNot );
   }

   Console console;
   console.EnableAbort();

   /*
    * Identify the spectrophotometric catalog
    */
   if ( p_catalogId.CompareIC( "GaiaDR3SP" ) != 0 )
      if ( p_catalogId.CompareIC( "GaiaDR3" ) != 0 )
         throw Error( "Unknown/unsupported spectrophotometric catalog '" + p_catalogId + "'" );
   String catalogName = "Gaia DR3/SP";

   /*
    * White reference curve interpolation
    */
   SPInterpolation whiteReference = ParseSpectrumParameter( p_whiteReferenceSpectrum, "white reference" );

   /*
    * Quantum efficiency curve
    */
   SPInterpolation deviceQECurve = ParseSpectrumParameter( p_deviceQECurve, "device QE curve" );

   /*
    * Broadband filter curve interpolations
    */
   SPInterpolation filters[ 3 ];
   filters[0] = ParseSpectrumParameter( p_redFilterTrCurve, "red filter" );
   filters[1] = ParseSpectrumParameter( p_greenFilterTrCurve, "green filter" );
   filters[2] = ParseSpectrumParameter( p_blueFilterTrCurve, "blue filter" );

   /*
    * Narrowband filter parameters
    */
   double wavelengths[ 3 ], bandwidths[ 3 ];
   wavelengths[0] = p_redFilterWavelength;
   bandwidths[0] = p_redFilterBandwidth;
   wavelengths[1] = p_greenFilterWavelength;
   bandwidths[1] = p_greenFilterBandwidth;
   wavelengths[2] = p_blueFilterWavelength;
   bandwidths[2] = p_blueFilterBandwidth;

   bool redIsGreen = false, blueIsGreen = false;
//    if ( p_narrowbandMode )
//    {
//       redIsGreen = p_redFilterWavelength == p_greenFilterWavelength && p_redFilterBandwidth == p_greenFilterBandwidth;
//       blueIsGreen = p_blueFilterWavelength == p_greenFilterWavelength && p_blueFilterBandwidth == p_greenFilterBandwidth;
//       if ( redIsGreen )
//          if ( blueIsGreen )
//             throw Error( "Identical wavelengths and bandwidths have been defined for the red, green and blue channels. "
//                          "At least two different filters are required for calibration in narrowband mode." );
//    }

   AutoViewLock lock( view );

   ImageWindow window = view.Window();

   /*
    * Astrometric solution
    */
   AstrometricMetadata A( window );
   if ( !A.IsValid() )
      throw Error( "The image has no valid astrometric solution: " + view.Id() );

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
    * Perform XPSD database search
    */
   console.WriteLn( "<end><cbr><br>* Searching catalog spectrum sources" );
   Module->ProcessEvents();

   // Search center and radius.
   DRect bounds = view.Bounds();
   DPoint centerRD;
   if ( !A.ImageToCelestial( centerRD, bounds.Center() ) )
      throw Error( "Invalid astrometric solution: unable to compute celestial coordinates for the center of the image" );
   double radius = A.SearchRadius();

   // Limit magnitude
   float magnitudeHigh = p_autoLimitMagnitude ? GaiaSPAutoLimitMagnitude( centerRD, radius, A, p_targetSourceCount ) : p_limitMagnitude;
   console.WriteLn( String().Format( "<end><cbr>Limit magnitude: %.2f", magnitudeHigh ) );

   Array<StarSPData> catalogStars = GaiaSPSearch( centerRD, radius, A, dbInfo, magnitudeHigh );
   if ( catalogStars.IsEmpty() )
      throw Error( "No catalog stars found" );

   console.WriteLn( String().Format( "<end><cbr><br>%u catalog sources found.", catalogStars.Length() ) );
   Module->ProcessEvents();

   // Search tree
   QuadTree<StarSPData> T( catalogStars );

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
   E.SetPSFType( SPCCPSFType::ToPSFFunction( p_psfType ) );
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
         if ( redIsGreen )
         {
            if ( c == 1 )
            {
               psfData[1] = psfData[0];
               continue;
            }
         }
         else if ( blueIsGreen )
         {
            if ( c == 2 )
            {
               psfData[2] = psfData[1];
               continue;
            }
         }

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

   console.WriteLn( "<end><cbr><br>* Integrating spectrum data" );
   Module->ProcessEvents();

   /*
    * Color samples
    */
   Array<ColorSample> colorSamples;
   IsoString text;
   Array<StarPSFSample> validPSFSamples;
   for ( const StarPSFSample& psf : psfSamples )
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

         ColorSample sample;
         sample.pos = psf.pos;
         for ( int c = 0; c < 3; ++c )
         {
            if ( p_narrowbandMode )
            {
               float l0 = wavelengths[c] - bandwidths[c]/2;
               float l1 = wavelengths[c] + bandwidths[c]/2;
               float dl = (l1 - l0)/p_narrowbandIntegrationSteps;
               sample.catalog[c] = 0;
               for ( int i = 1;; ++i )
               {
                  double la = l0 + (i-1)*dl;
                  double lb = l0 + i*dl;
                  double lm = (la + lb)/2;
                  double fa = deviceQECurve( la ) * P[j].flux( la );
                  double fb = deviceQECurve( lb ) * P[j].flux( lb );
                  double fm = deviceQECurve( lm ) * P[j].flux( lm );
                  sample.catalog[c] += (fa + fb + 4*fm)*(lb - la)/6;
                  if ( lb >= l1 )
                     break;
               }
               if ( p_narrowbandOptimizeStars )
                  sample.catalog[c] /= bandwidths[c];
            }
            else
            {
               const SPInterpolation& filter = filters[c];
               double l0 = filter.X()[0];
               double l1 = filter.X()[filter.X().Length()-1];
               sample.catalog[c] = 0;
               for ( int i = 1;; ++i )
               {
                  double la = l0 + (i-1)*p_broadbandIntegrationStepSize;
                  double lb = l0 + i*p_broadbandIntegrationStepSize;
                  double lm = (la + lb)/2;
                  double fa = filter( la ) * deviceQECurve( la ) * P[j].flux( la );
                  double fb = filter( lb ) * deviceQECurve( lb ) * P[j].flux( lb );
                  double fm = filter( lm ) * deviceQECurve( lm ) * P[j].flux( lm );
                  sample.catalog[c] += (fa + fb + 4*fm)*(lb - la)/6;
                  if ( lb >= l1 )
                     break;
               }
            }

            sample.measured[c] = psf.signal[c];
         }
         bool validSample = true;
         for ( int c = 0; c < 3; ++c )
            if ( 1 + sample.measured[c] == 1 || 1 + sample.catalog[c] == 1 )
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
               text << IsoString().Format( "%.2f,%.2f,%.7f,%.7f,%.6e,%.6e,%.6e,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
                                             sample.pos.x, sample.pos.y, posRD.x, posRD.y,
                                             sample.catalog[0], sample.catalog[1], sample.catalog[2],
                                             sample.measured[0], sample.measured[1], sample.measured[2],
                                             sample.catalog[0]/sample.catalog[1],
                                             sample.catalog[2]/sample.catalog[1],
                                             sample.measured[0]/sample.measured[1],
                                             sample.measured[2]/sample.measured[1] );
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
      File::WriteTextFile( outputDir + "SPCC-data.csv", text );
   }

   if ( p_generateStarMaps )
      CreateStarMapWindow( image.Width(), image.Height(), "SPCC_stars", validPSFSamples );

   /*
    * White reference
    */
   double whiteSamples[ 3 ];
   for ( int c = 0; c < 3; ++c )
   {
      if ( p_narrowbandMode )
      {
         float l0 = wavelengths[c] - bandwidths[c]/2;
         float l1 = wavelengths[c] + bandwidths[c]/2;
         float dl = (l1 - l0)/p_narrowbandIntegrationSteps;
         whiteSamples[c] = 0;
         for ( int i = 1;; ++i )
         {
            double la = l0 + (i-1)*dl;
            double lb = l0 + i*dl;
            double lm = (la + lb)/2;
            double fa = deviceQECurve( la ) * whiteReference( la );
            double fb = deviceQECurve( lb ) * whiteReference( lb );
            double fm = deviceQECurve( lm ) * whiteReference( lm );
            whiteSamples[c] += (fa + fb + 4*fm)*(lb - la)/6;
            if ( lb >= l1 )
               break;
         }
         if ( p_narrowbandOptimizeStars )
            whiteSamples[c] /= bandwidths[c];
      }
      else
      {
         const SPInterpolation& filter = filters[c];
         float l0 = filter.X()[0];
         float l1 = filter.X()[filter.X().Length()-1];
         whiteSamples[c] = 0;
         for ( int i = 1;; ++i )
         {
            double la = l0 + (i-1)*p_broadbandIntegrationStepSize;
            double lb = l0 + i*p_broadbandIntegrationStepSize;
            double lm = (la + lb)/2;
            double fa = filter( la ) * deviceQECurve( la ) * whiteReference( la );
            double fb = filter( lb ) * deviceQECurve( lb ) * whiteReference( lb );
            double fm = filter( lm ) * deviceQECurve( lm ) * whiteReference( lm );
            whiteSamples[c] += (fa + fb + 4*fm)*(lb - la)/6;
            if ( lb >= l1 )
               break;
         }
      }
   }

   /*
    * Color calibration functions
    */
   int N = int( colorSamples.Length() );
   DVector CRG( N ), CBG( N ); // catalog R/G and B/G samples
   DVector MRG( N ), MBG( N ); // measured R/G and B/G samples
   for ( int i = 0; i < N; ++i )
   {
      const ColorSample& sample = colorSamples[i];
      CRG[i] = sample.catalog[0] / sample.catalog[1];
      CBG[i] = sample.catalog[2] / sample.catalog[1];
      MRG[i] = sample.measured[0] / sample.measured[1];
      MBG[i] = sample.measured[2] / sample.measured[1];
   }

   console.WriteLn();

   LinearFit LRG, LBG;
   if ( !redIsGreen )
      LRG = RobustLinearRegression( CRG, MRG, "R/G" );
   if ( !blueIsGreen )
      LBG = RobustLinearRegression( CBG, MBG, "B/G" );

   console.WriteLn( "<end><cbr><br>* Color calibration functions:" );
   if ( !redIsGreen )
      console.WriteLn( String().Format( "f_R : R/G = %+.6f*FR/FG %c %.6f, sigma = %.6f",
                                        LRG.b, (LRG.a < 0) ? '-' : '+', Abs( LRG.a ), LRG.adev ) );
   if ( !blueIsGreen )
      console.WriteLn( String().Format( "f_B : B/G = %+.6f*FB/FG %c %.6f, sigma = %.6f",
                                        LBG.b, (LBG.a < 0) ? '-' : '+', Abs( LBG.a ), LBG.adev ) );

   double WRG = whiteSamples[0]/whiteSamples[1];
   double WBG = whiteSamples[2]/whiteSamples[1];

   /*
    * White balance factors
    */
   DVector W( redIsGreen ? 1.0 : 1.0/LRG( WRG ),
              1.0,
              blueIsGreen ? 1.0 : 1.0/LBG( WBG ) );
   W /= W.MaxComponent();

   console.NoteLn( String().Format( "<end><cbr><br>* White balance factors:\n"
                                    "W_R : %.4f\n"
                                    "W_G : %.4f\n"
                                    "W_B : %.4f", W[0], W[1], W[2] ) );

   if ( p_generateGraphs )
   {
      String redFilterName, greenFilterName, blueFilterName;
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

      if ( !TheSpectrophotometricColorCalibrationGraphInterface->IsVisible() )
         TheSpectrophotometricColorCalibrationGraphInterface->Launch();
      TheSpectrophotometricColorCalibrationGraphInterface->UpdateGraphs( view.FullId(), catalogName,
                                                                         redFilterName, greenFilterName, blueFilterName,
                                                                         p_deviceQECurveName, p_whiteReferenceName,
                                                                         CRG, MRG, LRG,
                                                                         CBG, MBG, LBG,
                                                                         W, WRG, WBG );
   }

   /*
    * Metadata generation
    */
   window.MainView().SetStorablePermanentPropertyValue( "PCL:SPCC:WhiteBalanceFactors", W );

   FITSKeywordArray keywords = window.Keywords();

   keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Color calibration with "  + PixInsightVersion::AsString() )
            << FITSHeaderKeyword( "HISTORY", IsoString(), "Color calibration with "  + Module->ReadableVersion() )
            << FITSHeaderKeyword( "HISTORY", IsoString(), "Color calibration with SpectrophotometricColorCalibration process" )
            << FITSHeaderKeyword( "HISTORY", IsoString(),
                                  IsoString().Format( "SPCC.whiteBalanceFactors: %.4f %.4f %.4f",
                                                      W[0], W[1], W[2] ) );

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

         window.MainView().SetStorablePermanentPropertyValue( "PCL:SPCC:BackgroundReference", backgroundReference );

         keywords << FITSHeaderKeyword( "HISTORY", IsoString(),
                        IsoString().Format( "SPCC.backgroundReference: %.8f %.8f %.8f",
                                             backgroundReference[0], backgroundReference[1], backgroundReference[2] ) );

         console.WriteLn();
         ApplyBackgroundNeutralization( image, backgroundReference );
      }
   }

   window.SetKeywords( keywords );

   return true;
}

// ----------------------------------------------------------------------------

void* SpectrophotometricColorCalibrationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheSPCCApplyCalibrationParameter )
      return &p_applyCalibration;
   if ( p == TheSPCCNarrowbandModeParameter )
      return &p_narrowbandMode;
   if ( p == TheSPCCNarrowbandOptimizeStarsParameter )
      return &p_narrowbandOptimizeStars;
   if ( p == TheSPCCWhiteReferenceSpectrumParameter )
      return p_whiteReferenceSpectrum.Begin();
   if ( p == TheSPCCWhiteReferenceNameParameter )
      return p_whiteReferenceName.Begin();
   if ( p == TheSPCCRedFilterTrCurveParameter )
      return p_redFilterTrCurve.Begin();
   if ( p == TheSPCCRedFilterNameParameter )
      return p_redFilterName.Begin();
   if ( p == TheSPCCGreenFilterTrCurveParameter )
      return p_greenFilterTrCurve.Begin();
   if ( p == TheSPCCGreenFilterNameParameter )
      return p_greenFilterName.Begin();
   if ( p == TheSPCCBlueFilterTrCurveParameter )
      return p_blueFilterTrCurve.Begin();
   if ( p == TheSPCCBlueFilterNameParameter )
      return p_blueFilterName.Begin();
   if ( p == TheSPCCRedFilterWavelengthParameter )
      return &p_redFilterWavelength;
   if ( p == TheSPCCRedFilterBandwidthParameter )
      return &p_redFilterBandwidth;
   if ( p == TheSPCCGreenFilterWavelengthParameter )
      return &p_greenFilterWavelength;
   if ( p == TheSPCCGreenFilterBandwidthParameter )
      return &p_greenFilterBandwidth;
   if ( p == TheSPCCBlueFilterWavelengthParameter )
      return &p_blueFilterWavelength;
   if ( p == TheSPCCBlueFilterBandwidthParameter )
      return &p_blueFilterBandwidth;
   if ( p == TheSPCCDeviceQECurveParameter )
      return p_deviceQECurve.Begin();
   if ( p == TheSPCCDeviceQECurveNameParameter )
      return p_deviceQECurveName.Begin();
   if ( p == TheSPCCBroadbandIntegrationStepSizeParameter )
      return &p_broadbandIntegrationStepSize;
   if ( p == TheSPCCNarrowbandIntegrationStepsParameter )
      return &p_narrowbandIntegrationSteps;
   if ( p == TheSPCCCatalogIdParameter )
      return p_catalogId.Begin();
   if ( p == TheSPCCLimitMagnitudeParameter )
      return &p_limitMagnitude;
   if ( p == TheSPCCAutoLimitMagnitudeParameter )
      return &p_autoLimitMagnitude;
   if ( p == TheSPCCTargetSourceCountParameter )
      return &p_targetSourceCount;
   if ( p == TheSPCCStructureLayersParameter )
      return &p_structureLayers;
   if ( p == TheSPCCSaturationThresholdParameter )
      return &p_saturationThreshold;
   if ( p == TheSPCCSaturationRelativeParameter )
      return &p_saturationRelative;
   if ( p == TheSPCCSaturationShrinkFactorParameter )
      return &p_saturationShrinkFactor;
   if ( p == TheSPCCPSFNoiseLayersParameter )
      return &p_psfNoiseLayers;
   if ( p == TheSPCCPSFHotPixelFilterRadiusParameter )
      return &p_psfHotPixelFilterRadius;
   if ( p == TheSPCCPSFNoiseReductionFilterRadiusParameter )
      return &p_psfNoiseReductionFilterRadius;
   if ( p == TheSPCCPSFMinStructureSizeParameter )
      return &p_psfMinStructureSize;
   if ( p == TheSPCCPSFMinSNRParameter )
      return &p_psfMinSNR;
   if ( p == TheSPCCPSFAllowClusteredSourcesParameter )
      return &p_psfAllowClusteredSources;
   if ( p == TheSPCCPSFTypeParameter )
      return &p_psfType;
   if ( p == TheSPCCPSFGrowthParameter )
      return &p_psfGrowth;
   if ( p == TheSPCCPSFMaxStarsParameter )
      return &p_psfMaxStars;
   if ( p == TheSPCCPSFSearchToleranceParameter )
      return &p_psfSearchTolerance;
   if ( p == TheSPCCPSFChannelSearchToleranceParameter )
      return &p_psfChannelSearchTolerance;
   if ( p == TheSPCCNeutralizeBackgroundParameter )
      return &p_neutralizeBackground;
   if ( p == TheSPCCBackgroundReferenceViewIdParameter )
      return p_backgroundReferenceViewId.Begin();
   if ( p == TheSPCCBackgroundLowParameter )
      return &p_backgroundLow;
   if ( p == TheSPCCBackgroundHighParameter )
      return &p_backgroundHigh;
   if ( p == TheSPCCBackgroundUseROIParameter )
      return &p_backgroundUseROI;
   if ( p == TheSPCCBackgroundROIX0Parameter )
      return &p_backgroundROI.x0;
   if ( p == TheSPCCBackgroundROIY0Parameter )
      return &p_backgroundROI.y0;
   if ( p == TheSPCCBackgroundROIX1Parameter )
      return &p_backgroundROI.x1;
   if ( p == TheSPCCBackgroundROIY1Parameter )
      return &p_backgroundROI.y1;
   if ( p == TheSPCCGenerateGraphsParameter )
      return &p_generateGraphs;
   if ( p == TheSPCCGenerateStarMapsParameter )
      return &p_generateStarMaps;
   if ( p == TheSPCCGenerateTextFilesParameter )
      return &p_generateTextFiles;
   if ( p == TheSPCCOutputDirectoryParameter )
      return p_outputDirectory.Begin();

   return nullptr;
}

// ----------------------------------------------------------------------------

bool SpectrophotometricColorCalibrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSPCCWhiteReferenceSpectrumParameter )
   {
      p_whiteReferenceSpectrum.Clear();
      if ( sizeOrLength > 0 )
         p_whiteReferenceSpectrum.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCWhiteReferenceNameParameter )
   {
      p_whiteReferenceName.Clear();
      if ( sizeOrLength > 0 )
         p_whiteReferenceName.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCRedFilterTrCurveParameter )
   {
      p_redFilterTrCurve.Clear();
      if ( sizeOrLength > 0 )
         p_redFilterTrCurve.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCRedFilterNameParameter )
   {
      p_redFilterName.Clear();
      if ( sizeOrLength > 0 )
         p_redFilterName.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCGreenFilterTrCurveParameter )
   {
      p_greenFilterTrCurve.Clear();
      if ( sizeOrLength > 0 )
         p_greenFilterTrCurve.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCGreenFilterNameParameter )
   {
      p_greenFilterName.Clear();
      if ( sizeOrLength > 0 )
         p_greenFilterName.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCBlueFilterTrCurveParameter )
   {
      p_blueFilterTrCurve.Clear();
      if ( sizeOrLength > 0 )
         p_blueFilterTrCurve.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCBlueFilterNameParameter )
   {
      p_blueFilterName.Clear();
      if ( sizeOrLength > 0 )
         p_blueFilterName.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCDeviceQECurveParameter )
   {
      p_deviceQECurve.Clear();
      if ( sizeOrLength > 0 )
         p_deviceQECurve.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCDeviceQECurveNameParameter )
   {
      p_deviceQECurveName.Clear();
      if ( sizeOrLength > 0 )
         p_deviceQECurveName.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCCatalogIdParameter )
   {
      p_catalogId.Clear();
      if ( sizeOrLength > 0 )
         p_catalogId.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCBackgroundReferenceViewIdParameter )
   {
      p_backgroundReferenceViewId.Clear();
      if ( sizeOrLength > 0 )
         p_backgroundReferenceViewId.SetLength( sizeOrLength );
   }
   else if ( p == TheSPCCOutputDirectoryParameter )
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

size_type SpectrophotometricColorCalibrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheSPCCWhiteReferenceSpectrumParameter )
      return p_whiteReferenceSpectrum.Length();
   if ( p == TheSPCCWhiteReferenceNameParameter )
      return p_whiteReferenceName.Length();
   if ( p == TheSPCCRedFilterTrCurveParameter )
      return p_redFilterTrCurve.Length();
   if ( p == TheSPCCRedFilterNameParameter )
      return p_redFilterName.Length();
   if ( p == TheSPCCGreenFilterTrCurveParameter )
      return p_greenFilterTrCurve.Length();
   if ( p == TheSPCCGreenFilterNameParameter )
      return p_greenFilterName.Length();
   if ( p == TheSPCCBlueFilterTrCurveParameter )
      return p_blueFilterTrCurve.Length();
   if ( p == TheSPCCBlueFilterNameParameter )
      return p_blueFilterName.Length();
   if ( p == TheSPCCDeviceQECurveParameter )
      return p_deviceQECurve.Length();
   if ( p == TheSPCCDeviceQECurveNameParameter )
      return p_deviceQECurveName.Length();
   if ( p == TheSPCCCatalogIdParameter )
      return p_catalogId.Length();
   if ( p == TheSPCCBackgroundReferenceViewIdParameter )
      return p_backgroundReferenceViewId.Length();
   if ( p == TheSPCCOutputDirectoryParameter )
      return p_outputDirectory.Length();

   return 0;
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationInstance::SetDefaultSpectrumData()
{
   if ( SpectrophotometricColorCalibrationProcess::HasValidSpectrumData() )
   {
      p_whiteReferenceSpectrum = SpectrophotometricColorCalibrationProcess::DefaultWhiteReference().data;
      p_whiteReferenceName = SpectrophotometricColorCalibrationProcess::DefaultWhiteReference().name;
      p_redFilterTrCurve = SpectrophotometricColorCalibrationProcess::DefaultRedFilter().data;
      p_redFilterName = SpectrophotometricColorCalibrationProcess::DefaultRedFilter().name;
      p_greenFilterTrCurve = SpectrophotometricColorCalibrationProcess::DefaultGreenFilter().data;
      p_greenFilterName = SpectrophotometricColorCalibrationProcess::DefaultGreenFilter().name;
      p_blueFilterTrCurve = SpectrophotometricColorCalibrationProcess::DefaultBlueFilter().data;
      p_blueFilterName = SpectrophotometricColorCalibrationProcess::DefaultBlueFilter().name;
      p_deviceQECurve = SpectrophotometricColorCalibrationProcess::DefaultDeviceQE().data;
      p_deviceQECurveName = SpectrophotometricColorCalibrationProcess::DefaultDeviceQE().name;
   }
   else
   {
      p_whiteReferenceSpectrum.Clear();
      p_whiteReferenceName.Clear();
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
// EOF SpectrophotometricColorCalibrationInstance.cpp - Released 2023-12-01T19:16:17Z
