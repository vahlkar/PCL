//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.6
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.8.6
// ----------------------------------------------------------------------------
// SubframeSelectorParameters.cpp - Released 2023-07-06T16:53:46Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2021 Cameron Leger
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

#include "SubframeSelectorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SSRoutine*                          TheSSRoutineParameter = nullptr;
SSNonInteractive*                   TheSSNonInteractiveParameter = nullptr;

SSSubframes*                        TheSSSubframesParameter = nullptr;
SSSubframeEnabled*                  TheSSSubframeEnabledParameter = nullptr;
SSSubframePath*                     TheSSSubframePathParameter = nullptr;
SSLocalNormalizationDataPath*       TheSSLocalNormalizationDataPathParameter = nullptr;
SSDrizzlePath*                      TheSSDrizzlePathParameter = nullptr;

SSFileCache*                        TheSSFileCacheParameter = nullptr;

SSSubframeScale*                    TheSSSubframeScaleParameter = nullptr;
SSCameraGain*                       TheSSCameraGainParameter = nullptr;
SSCameraResolution*                 TheSSCameraResolutionParameter = nullptr;
SSSiteLocalMidnight*                TheSSSiteLocalMidnightParameter = nullptr;
SSScaleUnit*                        TheSSScaleUnitParameter = nullptr;
SSDataUnit*                         TheSSDataUnitParameter = nullptr;
SSTrimmingFactor*                   TheSSTrimmingFactorParameter = nullptr;

SSStructureLayers*                  TheSSStructureLayersParameter = nullptr;
SSNoiseLayers*                      TheSSNoiseLayersParameter = nullptr;
SSHotPixelFilterRadius*             TheSSHotPixelFilterRadiusParameter = nullptr;
SSNoiseReductionFilterRadius*       TheSSNoiseReductionFilterRadiusParameter = nullptr;
SSMinStructureSize*                 TheSSMinStructureSizeParameter = nullptr;
SSSensitivity*                      TheSSSensitivityParameter = nullptr;
SSPeakResponse*                     TheSSPeakResponseParameter = nullptr;
SSBrightThreshold*                  TheSSBrightThresholdParameter = nullptr;
SSMaxDistortion*                    TheSSMaxDistortionParameter = nullptr;
SSAllowClusteredSources*            TheSSAllowClusteredSourcesParameter = nullptr;
SSUpperLimit*                       TheSSUpperLimitParameter = nullptr;
SSPSFFit*                           TheSSPSFFitParameter = nullptr;
SSPSFFitCircular*                   TheSSPSFFitCircularParameter = nullptr;
SSMaxPSFFits*                       TheSSMaxPSFFitsParameter = nullptr;
SSROIX0*                            TheSSROIX0Parameter = nullptr;
SSROIY0*                            TheSSROIY0Parameter = nullptr;
SSROIX1*                            TheSSROIX1Parameter = nullptr;
SSROIY1*                            TheSSROIY1Parameter = nullptr;

SSPedestalMode*                     TheSSPedestalModeParameter = nullptr;
SSPedestal*                         TheSSPedestalParameter = nullptr;
SSPedestalKeyword*                  TheSSPedestalKeywordParameter = nullptr;

SSInputHints*                       TheSSInputHintsParameter = nullptr;
SSOutputHints*                      TheSSOutputHintsParameter = nullptr;

SSOutputDirectory*                  TheSSOutputDirectoryParameter = nullptr;
SSOutputExtension*                  TheSSOutputExtensionParameter = nullptr;
SSOutputPrefix*                     TheSSOutputPrefixParameter = nullptr;
SSOutputPostfix*                    TheSSOutputPostfixParameter = nullptr;
SSOutputKeyword*                    TheSSOutputKeywordParameter = nullptr;
SSOverwriteExistingFiles*           TheSSOverwriteExistingFilesParameter = nullptr;
SSOnError*                          TheSSOnErrorParameter = nullptr;

SSApprovalExpression*               TheSSApprovalExpressionParameter = nullptr;
SSWeightingExpression*              TheSSWeightingExpressionParameter = nullptr;

SSSortingProperty*                  TheSSSortingPropertyParameter = nullptr;
SSGraphProperty*                    TheSSGraphPropertyParameter = nullptr;
SSAuxGraphProperty*                 TheSSAuxGraphPropertyParameter = nullptr;

SSUseFileThreads*                   TheSSUseFileThreadsParameter = nullptr;
SSFileThreadOverload*               TheSSFileThreadOverloadParameter = nullptr;
SSMaxFileReadThreads*               TheSSMaxFileReadThreadsParameter = nullptr;
SSMaxFileWriteThreads*              TheSSMaxFileWriteThreadsParameter = nullptr;

SSMeasurements*                     TheSSMeasurementsParameter = nullptr;
SSMeasurementIndex*                 TheSSMeasurementIndexParameter = nullptr;
SSMeasurementEnabled*               TheSSMeasurementEnabledParameter = nullptr;
SSMeasurementLocked*                TheSSMeasurementLockedParameter = nullptr;
SSMeasurementPath*                  TheSSMeasurementPathParameter = nullptr;
SSMeasurementWeight*                TheSSMeasurementWeightParameter = nullptr;
SSMeasurementFWHM*                  TheSSMeasurementFWHMParameter = nullptr;
SSMeasurementEccentricity*          TheSSMeasurementEccentricityParameter = nullptr;
SSMeasurementPSFSignalWeight*       TheSSMeasurementPSFSignalWeightParameter = nullptr;
SSMeasurementPSFSNR*                TheSSMeasurementPSFSNRParameter = nullptr;
SSMeasurementPSFScale*              TheSSMeasurementPSFScaleParameter = nullptr;
SSMeasurementPSFScaleSNR*           TheSSMeasurementPSFScaleSNRParameter = nullptr;
SSMeasurementPSFFlux*               TheSSMeasurementPSFFluxParameter = nullptr;
SSMeasurementPSFFluxPower*          TheSSMeasurementPSFFluxPowerParameter = nullptr;
SSMeasurementPSFTotalMeanFlux*      TheSSMeasurementPSFTotalMeanFluxParameter = nullptr;
SSMeasurementPSFTotalMeanPowerFlux* TheSSMeasurementPSFTotalMeanPowerFluxParameter = nullptr;
SSMeasurementPSFCount*              TheSSMeasurementPSFCountParameter = nullptr;
SSMeasurementMStar*                 TheSSMeasurementMStarParameter;
SSMeasurementNStar*                 TheSSMeasurementNStarParameter;
SSMeasurementSNRWeight*             TheSSMeasurementSNRWeightParameter = nullptr;
SSMeasurementMedian*                TheSSMeasurementMedianParameter = nullptr;
SSMeasurementMedianMeanDev*         TheSSMeasurementMedianMeanDevParameter = nullptr;
SSMeasurementNoise*                 TheSSMeasurementNoiseParameter = nullptr;
SSMeasurementNoiseRatio*            TheSSMeasurementNoiseRatioParameter = nullptr;
SSMeasurementStars*                 TheSSMeasurementStarsParameter = nullptr;
SSMeasurementStarResidual*          TheSSMeasurementStarResidualParameter = nullptr;
SSMeasurementFWHMMeanDev*           TheSSMeasurementFWHMMeanDevParameter = nullptr;
SSMeasurementEccentricityMeanDev*   TheSSMeasurementEccentricityMeanDevParameter = nullptr;
SSMeasurementStarResidualMeanDev*   TheSSMeasurementStarResidualMeanDevParameter = nullptr;
SSMeasurementAzimuth*               TheSSMeasurementAzimuthParameter = nullptr;
SSMeasurementAltitude*              TheSSMeasurementAltitudeParameter = nullptr;

SSMeasurementUnused01*              TheSSMeasurementUnused01Parameter = nullptr;

// ----------------------------------------------------------------------------

SSRoutine::SSRoutine( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSRoutineParameter = this;
}

IsoString SSRoutine::Id() const
{
   return "routine";
}

size_type SSRoutine::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSRoutine::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case MeasureSubframes:     return "MeasureSubframes";
   case OutputSubframes:      return "OutputSubframes";
   case StarDetectionPreview: return "StarDetectionPreview";
   }
}

int SSRoutine::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSRoutine::ElementLabel( size_type i ) const
{
   switch ( i )
   {
   default:
   case MeasureSubframes:     return "Measure Subframes";
   case OutputSubframes:      return "Output Subframes";
   case StarDetectionPreview: return "Star Detection Preview";
   }
}

size_type SSRoutine::DefaultValueIndex() const
{
   return Default;
}

IsoString SSRoutine::Tooltip() const
{
   return "<p><strong>Measure Subframes:</strong> Measures subframe parameters "
          "and displays them for Approval and Weighting.</p>"
          "<p><strong>Output Subframes:</strong> Uses Approval and Weighting "
          "information to output subframes.</p>"
          "<p><strong>Star Detection Preview:</strong> Outputs a Structure Map from "
          "the Star Detector for the first subframe. Useful for tweaking Star Detector parameters.</p>";
};

// ----------------------------------------------------------------------------

SSNonInteractive::SSNonInteractive( MetaProcess* P ) : MetaBoolean( P )
{
   TheSSNonInteractiveParameter = this;
}

IsoString SSNonInteractive::Id() const
{
   return "nonInteractive";
}

bool SSNonInteractive::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SSSubframes::SSSubframes( MetaProcess* P ) : MetaTable( P )
{
   TheSSSubframesParameter = this;
}

IsoString SSSubframes::Id() const
{
   return "subframes";
}

// ----------------------------------------------------------------------------

SSSubframeEnabled::SSSubframeEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheSSSubframeEnabledParameter = this;
}

IsoString SSSubframeEnabled::Id() const
{
   return "subframeEnabled";
}

bool SSSubframeEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSSubframePath::SSSubframePath( MetaTable* T ) : MetaString( T )
{
   TheSSSubframePathParameter = this;
}

IsoString SSSubframePath::Id() const
{
   return "subframePath";
}

// ----------------------------------------------------------------------------

SSLocalNormalizationDataPath::SSLocalNormalizationDataPath( MetaTable* T ) : MetaString( T )
{
   TheSSLocalNormalizationDataPathParameter = this;
}

IsoString SSLocalNormalizationDataPath::Id() const
{
   return "localNormalizationDataPath";
}

// ----------------------------------------------------------------------------

SSDrizzlePath::SSDrizzlePath( MetaTable* T ) : MetaString( T )
{
   TheSSDrizzlePathParameter = this;
}

IsoString SSDrizzlePath::Id() const
{
   return "drizzlePath";
}

// ----------------------------------------------------------------------------

SSFileCache::SSFileCache( MetaProcess* P ) : MetaBoolean( P )
{
   TheSSFileCacheParameter = this;
}

IsoString SSFileCache::Id() const
{
   return "fileCache";
}

bool SSFileCache::DefaultValue() const
{
   return true;
}

IsoString SSFileCache::Tooltip() const
{
   return "<p>Enable this option to skip measuring subframes that have already been measured and are in the file cache. "
          "Each time a subframe is measured, its file cache entry is created or overwritten.</p>"
          "<p>When changes to process parameters affect measurements, the file cache will not use older items.</p>";
}

// ----------------------------------------------------------------------------

SSSubframeScale::SSSubframeScale( MetaProcess* P ) : MetaFloat( P )
{
   TheSSSubframeScaleParameter = this;
}

IsoString SSSubframeScale::Id() const
{
   return "subframeScale";
}

int SSSubframeScale::Precision() const
{
   return 4;
}

double SSSubframeScale::DefaultValue() const
{
   return 1;
}

double SSSubframeScale::MinimumValue() const
{
   return 0.01;
}

double SSSubframeScale::MaximumValue() const
{
   return 100.0;
}

IsoString SSSubframeScale::Tooltip() const
{
   return "<p>Subframe scale in arcseconds per pixel. This affects star-related measurements.</p>"
          "<p>This is only used if the Scale Units are not in Pixels</p>";
}

// ----------------------------------------------------------------------------

SSCameraGain::SSCameraGain( MetaProcess* P ) : MetaFloat( P )
{
   TheSSCameraGainParameter = this;
}

IsoString SSCameraGain::Id() const
{
   return "cameraGain";
}

int SSCameraGain::Precision() const
{
   return 4;
}

double SSCameraGain::DefaultValue() const
{
   return 1;
}

double SSCameraGain::MinimumValue() const
{
   return 0.01;
}

double SSCameraGain::MaximumValue() const
{
   return 100.0;
}

IsoString SSCameraGain::Tooltip() const
{
   return "<p>Subframe camera gain in electrons per Data Number. This affects star-related measurements.</p>"
          "<p>This is only used if the Data Units are not in Data Numbers</p>";
}

// ----------------------------------------------------------------------------

SSCameraResolution::SSCameraResolution( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSCameraResolutionParameter = this;
}

IsoString SSCameraResolution::Id() const
{
   return "cameraResolution";
}

size_type SSCameraResolution::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSCameraResolution::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Bits8:  return "Bits8";
   case Bits10: return "Bits10";
   case Bits12: return "Bits12";
   case Bits14: return "Bits14";
   default:
   case Bits16: return "Bits16";
   }
}

int SSCameraResolution::ElementValue( size_type i ) const
{
   return int( i );
}

int SSCameraResolution::ElementData( size_type i ) const
{
   switch ( i )
   {
   case Bits8:  return 255;
   case Bits10: return 1023;
   case Bits12: return 4095;
   case Bits14: return 16383;
   default:
   case Bits16: return 65535;
   }
}

IsoString SSCameraResolution::ElementLabel( size_type i ) const
{
   switch ( i )
   {
   case Bits8:  return "8-bit [0,255]";
   case Bits10: return "10-bit [0,1023]";
   case Bits12: return "12-bit [0,4095]";
   case Bits14: return "14-bit [0,16383]";
   default:
   case Bits16: return "16-bit [0,65535]";
   }
}

size_type SSCameraResolution::DefaultValueIndex() const
{
   return Default;
}

IsoString SSCameraResolution::Tooltip() const
{
   return "<p>Subframe camera resolution in bits per pixel. This affects some subframe-related measurements.</p>";
}

// ----------------------------------------------------------------------------

SSSiteLocalMidnight::SSSiteLocalMidnight( MetaProcess* P ) : MetaInt32( P )
{
   TheSSSiteLocalMidnightParameter = this;
}

IsoString SSSiteLocalMidnight::Id() const
{
   return "siteLocalMidnight";
}

double SSSiteLocalMidnight::DefaultValue() const
{
   return 24;
}

double SSSiteLocalMidnight::MinimumValue() const
{
   return 0;
}

double SSSiteLocalMidnight::MaximumValue() const
{
   return 24;
}

IsoString SSSiteLocalMidnight::Tooltip() const
{
   return "<p>This parameters specifies the Coordinated Universal Time (UTC) of local midnight "
          "at the site of target subframe observation, rounded to the nearest hour from 0 to "
          "23. If this time is unknown or varies by more than six hours, set this parameter "
          "to 24.</p>"
          "<p>This parameter and the value of the FITS keyword DATE-OBS (if available) are used to "
          "identify sequences of subframe observations that occurred during the same night for "
          "data presentation purposes.</p>";
}

// ----------------------------------------------------------------------------

SSScaleUnit::SSScaleUnit( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSScaleUnitParameter = this;
}

IsoString SSScaleUnit::Id() const
{
   return "scaleUnit";
}

size_type SSScaleUnit::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSScaleUnit::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case ArcSeconds: return "ArcSeconds";
   case Pixel:      return "Pixel";
   }
}

int SSScaleUnit::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSScaleUnit::ElementLabel( size_type i ) const
{
   switch ( i )
   {
   default:
   case ArcSeconds: return "Arcseconds (arcsec)";
   case Pixel:      return "Pixels (pixel)";
   }
}

size_type SSScaleUnit::DefaultValueIndex() const
{
   return Default;
}

IsoString SSScaleUnit::Tooltip() const
{
   return "<p>How to present some star-related measurements.</p>";
}

// ----------------------------------------------------------------------------

SSDataUnit::SSDataUnit( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSDataUnitParameter = this;
}

IsoString SSDataUnit::Id() const
{
   return "dataUnit";
}

size_type SSDataUnit::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSDataUnit::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Electron:   return "Electron";
   case DataNumber: return "DataNumber";
   default:
   case Normalized: return "Normalized";
   }
}

int SSDataUnit::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSDataUnit::ElementLabel( size_type i ) const
{
   switch ( i )
   {
   case Electron:   return "Electrons (e-)";
   case DataNumber: return "Data Numbers (DN)";
   default:
   case Normalized: return "Normalized to [0,1]";
   }
}

size_type SSDataUnit::DefaultValueIndex() const
{
   return Default;
}

IsoString SSDataUnit::Tooltip() const
{
   return "<p>How to present some subframe-related measurements.</p>";
}

// ----------------------------------------------------------------------------

SSTrimmingFactor::SSTrimmingFactor( MetaProcess* P ) : MetaFloat( P )
{
   TheSSTrimmingFactorParameter = this;
}

IsoString SSTrimmingFactor::Id() const
{
   return "trimmingFactor";
}

int SSTrimmingFactor::Precision() const
{
   return 2;
}

double SSTrimmingFactor::DefaultValue() const
{
   return 0.1;
}

double SSTrimmingFactor::MinimumValue() const
{
   return 0;
}

double SSTrimmingFactor::MaximumValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

SSStructureLayers::SSStructureLayers( MetaProcess* P ) : MetaInt32( P )
{
   TheSSStructureLayersParameter = this;
}

IsoString SSStructureLayers::Id() const
{
   return "structureLayers";
}

double SSStructureLayers::DefaultValue() const
{
   return 5;
}

double SSStructureLayers::MinimumValue() const
{
   return 1;
}

double SSStructureLayers::MaximumValue() const
{
   return 20;
}

IsoString SSStructureLayers::Tooltip() const
{
   return "<p>Number of wavelet layers used for structure detection.</p>"
      "<p>With more wavelet layers, larger stars (and perhaps also some nonstellar objects) will be detected.</p>";
}

// ----------------------------------------------------------------------------

SSNoiseLayers::SSNoiseLayers( MetaProcess* P ) : MetaInt32( P )
{
   TheSSNoiseLayersParameter = this;
}

IsoString SSNoiseLayers::Id() const
{
   return "noiseLayers";
}

double SSNoiseLayers::DefaultValue() const
{
   return 0;
}

double SSNoiseLayers::MinimumValue() const
{
   return 0;
}

double SSNoiseLayers::MaximumValue() const
{
   return 20;
}

IsoString SSNoiseLayers::Tooltip() const
{
   return "<p>Number of wavelet layers used for noise reduction.</p>"
      "<p>Noise reduction prevents detection of bright noise structures as false stars, including hot pixels and "
      "cosmic rays. This parameter can also be used to control the sizes of the smallest detected stars (increase "
      "to exclude more stars), although the <i>minimum structure size</i> parameter can be more efficient for this purpose.</p>";
}

// ----------------------------------------------------------------------------

SSHotPixelFilterRadius::SSHotPixelFilterRadius( MetaProcess* P ) : MetaInt32( P )
{
   TheSSHotPixelFilterRadiusParameter = this;
}

IsoString SSHotPixelFilterRadius::Id() const
{
   return "hotPixelFilterRadius";
}

double SSHotPixelFilterRadius::DefaultValue() const
{
   return 1;
}

double SSHotPixelFilterRadius::MinimumValue() const
{
   return 0;
}

double SSHotPixelFilterRadius::MaximumValue() const
{
   return 20;
}

IsoString SSHotPixelFilterRadius::Tooltip() const
{
   return "<p>Size of the hot pixel removal filter.</p>"
      "<p>This is the radius in pixels of a median filter applied by the star detector before the structure "
      "detection phase. A median filter is very efficient to remove <i>hot pixels</i>. Hot pixels will be "
      "identified as false stars, and if present in large amounts, can lead to wrong PSF parameters.</p>"
      "<p>To disable hot pixel removal, set this parameter to zero.</p>";
}

// ----------------------------------------------------------------------------

SSNoiseReductionFilterRadius::SSNoiseReductionFilterRadius( MetaProcess* P ) : MetaInt32( P )
{
   TheSSNoiseReductionFilterRadiusParameter = this;
}

IsoString SSNoiseReductionFilterRadius::Id() const
{
   return "noiseReductionFilterRadius";
}

double SSNoiseReductionFilterRadius::DefaultValue() const
{
   return 0;
}

double SSNoiseReductionFilterRadius::MinimumValue() const
{
   return 0;
}

double SSNoiseReductionFilterRadius::MaximumValue() const
{
   return 20;
}

IsoString SSNoiseReductionFilterRadius::Tooltip() const
{
   return "<p>Size of the noise reduction filter.</p>"
      "<p>This is the radius in pixels of a Gaussian convolution filter applied to the working image used for "
      "calculation of star positions during the star detection phase. Use it only for very low SNR images, where "
      "the star detector cannot find reliable stars with default parameters.</p>"
      "<p>Be aware that noise reduction will modify star profiles and hence the way star positions are calculated, "
      "resulting in a less accurate PSF fitting. Under extreme low-SNR conditions, however, this is probably "
      "better than working with the actual data anyway.</p>"
      "<p>To disable noise reduction, set this parameter to zero.</p>";
}

// ----------------------------------------------------------------------------

SSMinStructureSize::SSMinStructureSize( MetaProcess* p ) : MetaInt32( p )
{
   TheSSMinStructureSizeParameter = this;
}

IsoString SSMinStructureSize::Id() const
{
   return "minStructureSize";
}

double SSMinStructureSize::DefaultValue() const
{
   return 0;
}

double SSMinStructureSize::MinimumValue() const
{
   return 0;
}

double SSMinStructureSize::MaximumValue() const
{
   return 999999;
}

IsoString SSMinStructureSize::Tooltip() const
{
   return "<p>Minimum size of a detectable star structure in square pixels.</p>"
      "<p>This parameter can be used to prevent detection of small and bright image artifacts as stars. "
      "This can be useful to work with uncalibrated or wrongly calibrated data, especially demosaiced CFA frames "
      "where hot pixels have generated large bright artifacts that cannot be removed with a median filter "
      "(i.e., the <i>Hot pixel removal</i> parameter).</p>"
      "<p>Changing the default zero value of this parameter should not be necessary with correctly acquired and "
      "calibrated data. It may help, however, when working with poor quality data such as poorly tracked, poorly focused, "
      "wrongly calibrated, low-SNR raw frames, for which our algorithms and tools have not been designed specifically.</p>";
}

// ----------------------------------------------------------------------------

SSSensitivity::SSSensitivity( MetaProcess* P ) : MetaFloat( P )
{
   TheSSSensitivityParameter = this;
}

IsoString SSSensitivity::Id() const
{
   return "sensitivity";
}

int SSSensitivity::Precision() const
{
   return 2;
}

double SSSensitivity::DefaultValue() const
{
   return 0.5;
}

double SSSensitivity::MinimumValue() const
{
   return 0;
}

double SSSensitivity::MaximumValue() const
{
   return 1;
}

IsoString SSSensitivity::Tooltip() const
{
   return "<p>Star detection sensitivity.</p>"
      "<p>Internally, the sensitivity of the star detection algorithm is expressed in signal-to-noise ratio units with "
      "respect to the evaluated dispersion of local background pixels for each detected structure. Given a source with "
      "estimated brightness <i>s</i>, local background <i>b</i> and local background dispersion <i>n</i>, sensitivity "
      "is the minimum value of (<i>s</i> - <i>b</i>)/<i>n</i> necessary to trigger star detection.</p>"
      "<p>To isolate this interface from the internal implementation, this parameter is normalized to the [0,1] range, "
      "where 0 and 1 represent minimum and maximum sensitivity, respectively. This abstraction allows us to change the "
      "star detection engine without breaking dependent tools and processes.</p>"
      "<p>Increase this parameter to favor detection of fainter stars. Decrease it to restrict detection to brighter "
      "stars. The default value is 0.5. In general, you shouldn't need to change the default value of this parameter "
      "under normal working conditions.</p>";
}

// ----------------------------------------------------------------------------

SSPeakResponse::SSPeakResponse( MetaProcess* P ) : MetaFloat( P )
{
   TheSSPeakResponseParameter = this;
}

IsoString SSPeakResponse::Id() const
{
   return "peakResponse";
}

int SSPeakResponse::Precision() const
{
   return 2;
}

double SSPeakResponse::DefaultValue() const
{
   return 0.5;
}

double SSPeakResponse::MinimumValue() const
{
   return 0;
}

double SSPeakResponse::MaximumValue() const
{
   return 1;
}

IsoString SSPeakResponse::Tooltip() const
{
   return "<p>Peak sensitivity of the star detection device.</p>"
      "<p>Internally, the peak response property of the star detection algorithm is expressed in kurtosis units. For "
      "each detected structure, kurtosis is evaluated from all significant pixels with values greater than the estimated "
      "mean local background. Peak response is the minimum value of kurtosis necessary to trigger star detection.</p>"
      "<p>To isolate this interface from the internal implementation, this parameter is normalized to the [0,1] range, "
      "where 0 and 1 represent minimum and maximum peak response, respectively. This abstraction allows us to change the "
      "star detection engine without breaking dependent tools and processes.</p>"
      "<p>If you decrease this parameter, stars will need to have stronger (or more prominent) peaks to be detected. This "
      "is useful to prevent detection of saturated stars, as well as small nonstellar features. By increasing this "
      "parameter, the star detection algorithm will be more sensitive to <i>peakedness</i>, and hence more tolerant with "
      "relatively flat image features. The default value is 0.5. In general, you shouldn't need to change the default "
      "value of this parameter under normal working conditions.</p>";
}

// ----------------------------------------------------------------------------

SSBrightThreshold::SSBrightThreshold( MetaProcess* p ) : MetaFloat( p )
{
   TheSSBrightThresholdParameter = this;
}

IsoString SSBrightThreshold::Id() const
{
   return "brightThreshold";
}

int SSBrightThreshold::Precision() const
{
   return 2;
}

double SSBrightThreshold::DefaultValue() const
{
   return 3.0;
}

double SSBrightThreshold::MinimumValue() const
{
   return 1;
}

double SSBrightThreshold::MaximumValue() const
{
   return 100;
}

IsoString SSBrightThreshold::Tooltip() const
{
   return "<p>Bright star detection threshold</p>"
      "<p>Sources with measured SNR above this parameter in units of the minimum detection level (as defined by the "
      "sensitivity parameter) will always be detected, even if their profiles are too flat for the current peak response. "
      "This parameter allows us to force inclusion of relatively bright stars irrespective of their shapes, and also "
      "provides finer control on the amount of detectable stars, along with the sensitivity parameter. The default value "
      "is 3.0.</p>";
}

// ----------------------------------------------------------------------------

SSMaxDistortion::SSMaxDistortion( MetaProcess* P ) : MetaFloat( P )
{
   TheSSMaxDistortionParameter = this;
}

IsoString SSMaxDistortion::Id() const
{
   return "maxDistortion";
}

int SSMaxDistortion::Precision() const
{
   return 2;
}

double SSMaxDistortion::DefaultValue() const
{
   return 0.6;
}

double SSMaxDistortion::MinimumValue() const
{
   return 0;
}

double SSMaxDistortion::MaximumValue() const
{
   return 1;
}

IsoString SSMaxDistortion::Tooltip() const
{
   return "<p>Maximum star distortion.</p>"
      "<p>Internally, star distortion is evaluated in units of coverage of a square region circumscribed to each detected "
      "structure. The coverage of a perfectly circular star is &pi;/4 (about 0.8). Lower values denote elongated or "
      "irregular sources.</p>"
      "<p>To isolate this interface from the internal implementation, this parameter is normalized to the [0,1] range, "
      "where 0 and 1 represent minimum and maximum distortion, respectively. This abstraction allows us to change the "
      "star detection engine without breaking dependent tools and processes.</p>"
      "<p>Use this parameter, if necessary, to control inclusion of elongated stars, complex clusters of stars, and "
      "nonstellar image features. The default value is 0.6. In general, you shouldn't need to change the default value of "
      "this parameter under normal working conditions.</p>";
}

// ----------------------------------------------------------------------------

SSAllowClusteredSources::SSAllowClusteredSources( MetaProcess* p ) : MetaBoolean( p )
{
   TheSSAllowClusteredSourcesParameter = this;
}

IsoString SSAllowClusteredSources::Id() const
{
   return "allowClusteredSources";
}

bool SSAllowClusteredSources::DefaultValue() const
{
   return false;
}

IsoString SSAllowClusteredSources::Tooltip() const
{
   return "<p>If this parameter is disabled, a local maxima map will be generated to "
      "identify and prevent detection of multiple sources that are too close to be separated as individual structures, such "
      "as double and multiple stars. In general, sources with several local maxima pose difficulties for the determination of "
      "accurate star positions.</p>"
      "<p>If this parameter is enabled, non-separable multiple sources will be freely detectable as single objects. This can "
      "be necessary in special cases where extreme high noise levels and/or distortion of stellar images caused by optical "
      "aberrations limit the number of detectable sources excessively, preventing image registration.</p>"
      "<p>The default state is disabled.</p>";
}

// ----------------------------------------------------------------------------

SSUpperLimit::SSUpperLimit( MetaProcess* P ) : MetaFloat( P )
{
   TheSSUpperLimitParameter = this;
}

IsoString SSUpperLimit::Id() const
{
   return "upperLimit";
}

int SSUpperLimit::Precision() const
{
   return 4;
}

double SSUpperLimit::DefaultValue() const
{
   return 1.0;
}

double SSUpperLimit::MinimumValue() const
{
   return 0.0;
}

double SSUpperLimit::MaximumValue() const
{
   return 1.0;
}

IsoString SSUpperLimit::Tooltip() const
{
   return "<p>Upper star detection limit.</p>"
      "<p>Stars with peak values larger than this value won't be detected. This can be useful to reject all stars "
      "brighter than a fixed level systematically. To effectively disable this feature, set this parameter to one.</p>";
}

// ----------------------------------------------------------------------------

SSPSFFit::SSPSFFit( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSPSFFitParameter = this;
}

IsoString SSPSFFit::Id() const
{
   return "psfFit";
}

size_type SSPSFFit::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSPSFFit::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Gaussian:      return "Gaussian";
   case Moffat10:      return "Moffat10";
   case Moffat8:       return "Moffat8";
   case Moffat6:       return "Moffat6";
   default:
   case Moffat4:       return "Moffat4";
   case Moffat25:      return "Moffat25";
   case Moffat15:      return "Moffat15";
   case Lorentzian:    return "Lorentzian";
   }
}

int SSPSFFit::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSPSFFit::ElementLabel( size_type i ) const
{
   switch ( i )
   {
   case Gaussian:      return "Gaussian";
   case Moffat10:      return "Moffat beta = 10";
   case Moffat8:       return "Moffat beta = 8";
   case Moffat6:       return "Moffat beta = 6";
   default:
   case Moffat4:       return "Moffat beta = 4";
   case Moffat25:      return "Moffat beta = 2.5";
   case Moffat15:      return "Moffat beta = 1.5";
   case Lorentzian:    return "Lorentzian";
   }
}

size_type SSPSFFit::DefaultValueIndex() const
{
   return Default;
}

IsoString SSPSFFit::Tooltip() const
{
   return "<p>This parameter specifies the type of <i>point spread function</i> (PSF) used "
          "to fit star image models numerically.</p>"
          "<p>The PSF can be either a Gaussian or a Moffat function with a prescribed "
          "shape parameter.</p>"
          "<p>The default option is a Moffat PSF with <i>beta</i>=4, which is fast and quite "
          "accurate in most cases.</p>";
}

// ----------------------------------------------------------------------------

SSPSFFitCircular::SSPSFFitCircular( MetaProcess* P ) : MetaBoolean( P )
{
   TheSSPSFFitCircularParameter = this;
}

IsoString SSPSFFitCircular::Id() const
{
   return "psfFitCircular";
}

bool SSPSFFitCircular::DefaultValue() const
{
   return false;
}

IsoString SSPSFFitCircular::Tooltip() const
{
   return "<p>Enable this option to fit circular point spread functions. Disable it to fit elliptical functions.</p>"
          "Circular functions can provide more robust and useful results in cases of strong "
          "undersampling or high noise levels.</p>";
}

// ----------------------------------------------------------------------------

SSMaxPSFFits::SSMaxPSFFits( MetaProcess* P ) : MetaInt32( P )
{
   TheSSMaxPSFFitsParameter = this;
}

IsoString SSMaxPSFFits::Id() const
{
   return "maxPSFFits";
}

double SSMaxPSFFits::DefaultValue() const
{
   return 8000;
}

double SSMaxPSFFits::MinimumValue() const
{
   return 0;
}

double SSMaxPSFFits::MaximumValue() const
{
   return int32_max;
}

IsoString SSMaxPSFFits::Tooltip() const
{
   return "<p>The maximum number of PSF fits that will be performed on the set of detected stars.</p>"
          "<p>The default value is 8000 stars, which is more than reasonable in most cases. Set this parameter to zero to "
          "allow an unlimited number of PSF fits, that is, to attempt fitting all detected stars on each subframe.</p>"
          "<p>This parameter is intended to avoid fitting huge amounts of PSFs on wide-field images, where the number of "
          "detected stars can easily reach several tens of thousands, or even more than one hundred thousand in extreme cases. "
          "In such cases the measurement process can take a very long time to complete, but without any benefit in statistical "
          "terms, since fitting about 10,000 stars on each subframe is normally more than sufficient to acquire reliable FWHM "
          "and eccentricity measurements.</p>"
          "<p>When this parameter imposes a limit on the number of PSF fits, the subset of measured stars will always start at "
          "the beginning of the set of detected stars, sorted by brightness in descending order.</p>";
}

// ----------------------------------------------------------------------------

SSROIX0::SSROIX0( MetaProcess* P ) : MetaInt32( P )
{
   TheSSROIX0Parameter = this;
}

IsoString SSROIX0::Id() const
{
   return "roiX0";
}

double SSROIX0::DefaultValue() const
{
   return 0;
}

double SSROIX0::MinimumValue() const
{
   return 0;
}

double SSROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

SSROIY0::SSROIY0( MetaProcess* P ) : MetaInt32( P )
{
   TheSSROIY0Parameter = this;
}

IsoString SSROIY0::Id() const
{
   return "roiY0";
}

double SSROIY0::DefaultValue() const
{
   return 0;
}

double SSROIY0::MinimumValue() const
{
   return 0;
}

double SSROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

SSROIX1::SSROIX1( MetaProcess* P ) : MetaInt32( P )
{
   TheSSROIX1Parameter = this;
}

IsoString SSROIX1::Id() const
{
   return "roiX1";
}

double SSROIX1::DefaultValue() const
{
   return 0;
}

double SSROIX1::MinimumValue() const
{
   return 0;
}

double SSROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

SSROIY1::SSROIY1( MetaProcess* P ) : MetaInt32( P )
{
   TheSSROIY1Parameter = this;
}

IsoString SSROIY1::Id() const
{
   return "roiY1";
}

double SSROIY1::DefaultValue() const
{
   return 0;
}

double SSROIY1::MinimumValue() const
{
   return 0;
}

double SSROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

SSPedestalMode::SSPedestalMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSPedestalModeParameter = this;
}

IsoString SSPedestalMode::Id() const
{
   return "pedestalMode";
}

size_type SSPedestalMode::NumberOfElements() const
{
   return NumberOfPedestalModes;
}

IsoString SSPedestalMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Literal:       return "Pedestal_Literal";
   default:
   case Keyword:       return "Pedestal_Keyword";
   case CustomKeyword: return "Pedestal_CustomKeyword";
   }
}

int SSPedestalMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type SSPedestalMode::DefaultValueIndex() const
{
   return size_type( Default );
}

IsoString SSPedestalMode::Tooltip() const
{
   return
   "<p>The <i>pedestal mode</i> option specifies how to retrieve a (usually small) quantity that is subtracted "
   "from input images as the very first step before measurement. This quantity is known as <i>pedestal</i>, and "
   "must be expressed in the 16-bit unsigned integer range (from 0 to 65535). If present, a pedestal has been "
   "added by a previous calibration process to ensure positivity of the result.</p>"
   "<p><b>Literal value</b> lets you specify a pedestal value that will be subtracted from all input frames. "
   "You usually don't need this option, except in very special cases.</p>"
   "<p><b>Default FITS keyword</b> is the default mode. When this mode is selected, SubframeSelector will "
   "retrieve pedestal values from 'PEDESTAL' FITS keywords, when they are present in the input images.</p>"
   "<p><b>Custom FITS keyword</b> allows you to specify the name of a custom FITS keyword to retrieve "
   "pedestal values, when the specified keyword is present in the input images.</p>";
}

// ----------------------------------------------------------------------------

SSPedestal::SSPedestal( MetaProcess* P ) : MetaInt32( P )
{
   TheSSPedestalParameter = this;
}

IsoString SSPedestal::Id() const
{
   return "pedestal";
}

double SSPedestal::DefaultValue() const
{
   return 0;
}

double SSPedestal::MinimumValue() const
{
   return 0;
}

double SSPedestal::MaximumValue() const
{
   return 1000;
}

IsoString SSPedestal::Tooltip() const
{
   return
   "<p>Literal pedestal value in the 16-bit unsigned integer range (from 0 to 65535), when the "
   "<i>Literal value</i> option has been selected as <i>pedestal mode</i>.</p>";
}

// ----------------------------------------------------------------------------

SSPedestalKeyword::SSPedestalKeyword( MetaProcess* P ) : MetaString( P )
{
   TheSSPedestalKeywordParameter = this;
}

IsoString SSPedestalKeyword::Id() const
{
   return "pedestalKeyword";
}

IsoString SSPedestalKeyword::Tooltip() const
{
   return
   "<p>Custom FITS keyword to retrieve pedestal values in 16-bit DN.</p>"
   "<p>This is the name of a FITS keyword that will be used to retrieve input pedestals, if the "
   "<i>Custom FITS keyword</i> option has been selected as <i>pedestal mode</i>.</p>";
}

// ----------------------------------------------------------------------------

SSInputHints::SSInputHints( MetaProcess* P ) : MetaString( P )
{
   TheSSInputHintsParameter = this;
}

IsoString SSInputHints::Id() const
{
   return "inputHints";
}

IsoString SSInputHints::Tooltip() const
{
   return "<p><i>Format hints</i> allow you to override global file format settings "
          "for image files used by specific processes. In SubframeSelector, input hints change the way "
          "input images of some particular file formats are loaded, while output hints modify the way "
          "output image files are written.</p>"
          "<p>For example, you can use the \"lower-range\" and \"upper-range\" input hints to load "
          "floating point FITS and TIFF files generated by other applications that don't use "
          "PixInsight's normalized [0,1] range. Similarly, you can specify the \"compression-codec zlib+sh\" "
          "output hint to write all output images (in XISF format) using zlib compression with byte shuffling. "
          "Most standard file format modules support hints; each format supports a number of input and/or "
          "output hints that you can use for different purposes with tools that give you access to format "
          "hints. For information on hints supported by a particular file format, see the corresponding "
          "<em>implementation</em> information given on the Format Explorer window.</p>";
}

// ----------------------------------------------------------------------------

SSOutputHints::SSOutputHints( MetaProcess* P ) : MetaString( P )
{
   TheSSOutputHintsParameter = this;
}

IsoString SSOutputHints::Id() const
{
   return "outputHints";
}

// ----------------------------------------------------------------------------

SSOutputDirectory::SSOutputDirectory( MetaProcess* P ) : MetaString( P )
{
   TheSSOutputDirectoryParameter = this;
}

IsoString SSOutputDirectory::Id() const
{
   return "outputDirectory";
}

IsoString SSOutputDirectory::Tooltip() const
{
   return "<p>This is the directory (or folder) where all output files will be written.</p>"
          "<p>If this field is left blank, output files will be written to the same directories as their "
          "corresponding subframe files. In this case, make sure that source directories are writable, or the "
          "output process will fail.</p>";
}

// ----------------------------------------------------------------------------

SSOutputExtension::SSOutputExtension( MetaProcess* P ) : MetaString( P )
{
   TheSSOutputExtensionParameter = this;
}

IsoString SSOutputExtension::Id() const
{
   return "outputExtension";
}

String SSOutputExtension::DefaultValue() const
{
   return ".xisf";
}

// ----------------------------------------------------------------------------

SSOutputPrefix::SSOutputPrefix( MetaProcess* P ) : MetaString( P )
{
   TheSSOutputPrefixParameter = this;
}

IsoString SSOutputPrefix::Id() const
{
   return "outputPrefix";
}

String SSOutputPrefix::DefaultValue() const
{
   return String(); // no prefix
}

IsoString SSOutputPrefix::Tooltip() const
{
   return "<p>This is a prefix that will be prepended to all output file names.</p>";
}

// ----------------------------------------------------------------------------

SSOutputPostfix::SSOutputPostfix( MetaProcess* P ) : MetaString( P )
{
   TheSSOutputPostfixParameter = this;
}

IsoString SSOutputPostfix::Id() const
{
   return "outputPostfix";
}

String SSOutputPostfix::DefaultValue() const
{
   return "_a";
}

IsoString SSOutputPostfix::Tooltip() const
{
   return "<p>This is a postfix that will be appended to all output file names.</p>";
}

// ----------------------------------------------------------------------------

SSOutputKeyword::SSOutputKeyword( MetaProcess* P ) : MetaString( P )
{
   TheSSOutputKeywordParameter = this;
}

IsoString SSOutputKeyword::Id() const
{
   return "outputKeyword";
}

String SSOutputKeyword::DefaultValue() const
{
   return "SSWEIGHT";
}

IsoString SSOutputKeyword::Tooltip() const
{
   return "<p>This is the FITS Keyword name that will be used to store the Weight.</p>";
}

// ----------------------------------------------------------------------------

SSOverwriteExistingFiles::SSOverwriteExistingFiles( MetaProcess* P ) : MetaBoolean( P )
{
   TheSSOverwriteExistingFilesParameter = this;
}

IsoString SSOverwriteExistingFiles::Id() const
{
   return "overwriteExistingFiles";
}

bool SSOverwriteExistingFiles::DefaultValue() const
{
   return false;
}

IsoString SSOverwriteExistingFiles::Tooltip() const
{
   return "<p>If this option is selected, SubframeSelector will overwrite "
          "existing files with the same names as generated output files. This can "
          "be dangerous because the original contents of overwritten files will be lost.</p>"
          "<p><b>Warning: Enable this option <u>at your own risk.</u></b></p>";
}

// ----------------------------------------------------------------------------

SSOnError::SSOnError( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSOnErrorParameter = this;
}

IsoString SSOnError::Id() const
{
   return "onError";
}

size_type SSOnError::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSOnError::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Continue: return "Continue";
   case Abort:    return "Abort";
   case AskUser:  return "AskUser";
   }
}

IsoString SSOnError::ElementLabel( size_type i ) const
{
   switch ( i )
   {
   default:
   case Continue: return "Continue";
   case Abort:    return "Abort";
   case AskUser:  return "Ask User";
   }
}

int SSOnError::ElementValue( size_type i ) const
{
   return int( i );
}

size_type SSOnError::DefaultValueIndex() const
{
   return size_type( Default );
}

IsoString SSOnError::Tooltip() const
{
   return "<p>Specify what to do if there are errors during the output process.</p>";
}

// ----------------------------------------------------------------------------

SSApprovalExpression::SSApprovalExpression( MetaProcess* P ) : MetaString( P )
{
   TheSSApprovalExpressionParameter = this;
}

IsoString SSApprovalExpression::Id() const
{
   return "approvalExpression";
}

IsoString SSApprovalExpression::Tooltip() const
{
   return "<p>Subframe approval expression, a boolean combination of constraints. A blank "
          "expression will approve all subframes. This is technically JavaScript, and although "
          "some characters are limited to prevent misuse, many things are possible such as "
          "<i>Math.abs(FWHMSigma)</i> or using <i>//</i> to comment the end of the line, etc. "
          "Please see the documentation for more information.</p>"
          "<p>A <i>property</i> can be one of:</p>"
          "<p>Altitude | Azimuth | Eccentricity | EccentricityMeanDev | FWHM | FWHMMeanDev | Index | "
          "MStar | Median | MedianMeanDev | NStar | Noise | NoiseRatio | PSFCount | PSFFlux | "
          "PSFFluxPower | PSFSignalWeight = PSFSignal | PSFSNR | PSFScale | PSFScaleSNR | "
          "PSFTotalMeanFlux | PSFTotalMeanPowerFlux | SNR = SNRWeight | StarResidual | "
          "StarResidualMeanDev | Stars | Weight</p>"
          "<p>Each <i>property</i> (excluding <i>Index</i>) also has a Median, Min, and Max version, "
          "e.g. <i>FWHMMin</i>, which are computed across all subframes.</p>"
          "<p>Each <i>property</i> (excluding <i>Index</i>) also has a Sigma version, "
          "e.g. <i>PSFSignalSigma</i>, where the value is represented in sigma units of the "
          "mean absolute deviation from the median.</p>";
}

// ----------------------------------------------------------------------------

SSWeightingExpression::SSWeightingExpression( MetaProcess* P ) : MetaString( P )
{
   TheSSWeightingExpressionParameter = this;
}

IsoString SSWeightingExpression::Id() const
{
   return "weightingExpression";
}

IsoString SSWeightingExpression::Tooltip() const
{
   return "<p>Subframe weighting expression, an arithmetic combination of properties. A blank "
          "expression will assign a zero weight to all subframes. This is technically JavaScript, "
          "and although some characters are limited to prevent misuse, many things are possible such as "
          "<i>Math.abs(FWHMSigma)</i> or using <i>//</i> to comment the end of the line, etc. "
          "Please see the documentation for more information.</p>"
          "<p>A <i>property</i> can be one of:<p>"
          "<p>Altitude | Azimuth | Eccentricity | EccentricityMeanDev | FWHM | FWHMMeanDev | Index | "
          "MStar | Median | MedianMeanDev | NStar | Noise | NoiseRatio | PSFCount | PSFFlux | "
          "PSFFluxPower | PSFSignalWeight = PSFSignal | PSFSNR | PSFScale | PSFScaleSNR | "
          "PSFTotalMeanFlux | PSFTotalMeanPowerFlux | SNR = SNRWeight | StarResidual | "
          "StarResidualMeanDev | Stars</p>"
          "<p>Each <i>property</i> (excluding <i>Index</i>) also has a Median, Min, and Max version, "
          "e.g. <i>FWHMMin</i>, which are computed across all subframes.</p>"
          "<p>Each <i>property</i> (excluding <i>Index</i>) also has a Sigma version, "
          "e.g. <i>PSFSignalSigma</i>, where the value is represented in sigma units of the "
          "mean absolute deviation from the median.</p>";
}

// ----------------------------------------------------------------------------

SSSortingProperty::SSSortingProperty( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSSortingPropertyParameter = this;
}

IsoString SSSortingProperty::Id() const
{
   return "sortProperty";
}

size_type SSSortingProperty::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSSortingProperty::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Index:                 return "Index";
   case Weight:                return "Weight";
   case FWHM:                  return "FWHM";
   case Eccentricity:          return "Eccentricity";
   case SNRWeight:             return "SNR";
   case Median:                return "Median";
   case MedianMeanDev:         return "MedianMeanDev";
   case Noise:                 return "Noise";
   case NoiseRatio:            return "NoiseRatio";
   case Stars:                 return "Stars";
   case StarResidual:          return "StarResidual";
   case FWHMMeanDev:           return "FWHMMeanDev";
   case EccentricityMeanDev:   return "EccentricityMeanDev";
   case StarResidualMeanDev:   return "StarResidualMeanDev";
   case PSFSignalWeight:       return "PSFSignalWeight";
   case PSFSNR:                return "PSFSNR";
   case PSFScale:              return "PSFScale";
   case PSFScaleSNR:           return "PSFScaleSNR";
   case Azimuth:               return "Azimuth";
   case Altitude:              return "Altitude";
   case PSFFlux:               return "PSFFlux";
   case PSFFluxPower:          return "PSFFluxPower";
   case PSFTotalMeanFlux:      return "PSFTotalMeanFlux";
   case PSFTotalMeanPowerFlux: return "PSFTotalMeanPowerFlux";
   case PSFCount:              return "PSFCount";
   case MStar:                 return "MStar";
   case NStar:                 return "NStar";
   }
}

IsoString SSSortingProperty::ElementAliases() const
{
   return "SNRWeight=SNR";
}

int SSSortingProperty::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSSortingProperty::ElementLabel( size_type i ) const
{
   switch ( i )
   {
   default:
   case Index:                 return "Index";
   case Weight:                return "Weight";
   case FWHM:                  return "FWHM";
   case Eccentricity:          return "Eccentricity";
   case SNRWeight:             return "SNR";
   case Median:                return "Median";
   case MedianMeanDev:         return "Median Mean Dev.";
   case Noise:                 return "Noise";
   case NoiseRatio:            return "Noise Ratio";
   case Stars:                 return "Stars";
   case StarResidual:          return "Star Residual";
   case FWHMMeanDev:           return "FWHM Mean Dev.";
   case EccentricityMeanDev:   return "Eccentricity Mean Dev.";
   case StarResidualMeanDev:   return "StarResidual Mean Dev.";
   case PSFSignalWeight:       return "PSF Signal Weight";
   case PSFSNR:                return "PSF SNR";
   case PSFScale:              return "PSF Scale";
   case PSFScaleSNR:           return "PSF Scale SNR";
   case Azimuth:               return "Azimuth";
   case Altitude:              return "Altitude";
   case PSFFlux:               return "PSF Flux";
   case PSFFluxPower:          return "PSF Flux Power";
   case PSFTotalMeanFlux:      return "PSF Total Mean Flux";
   case PSFTotalMeanPowerFlux: return "PSF Total Mean Power Flux";
   case PSFCount:              return "PSF Count";
   case MStar:                 return "M*";
   case NStar:                 return "N*";
   }
}

size_type SSSortingProperty::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

SSGraphPropertyBase::SSGraphPropertyBase( MetaProcess* P ) : MetaEnumeration( P )
{
}

size_type SSGraphPropertyBase::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSGraphPropertyBase::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Weight:                return "Weight";
   case FWHM:                  return "FWHM";
   case Eccentricity:          return "Eccentricity";
   case SNRWeight:             return "SNR";
   case Median:                return "Median";
   case MedianMeanDev:         return "MedianMeanDev";
   case Noise:                 return "Noise";
   case NoiseRatio:            return "NoiseRatio";
   case Stars:                 return "Stars";
   case StarResidual:          return "StarResidual";
   case FWHMMeanDev:           return "FWHMMeanDev";
   case EccentricityMeanDev:   return "EccentricityMeanDev";
   case StarResidualMeanDev:   return "StarResidualMeanDev";
   case PSFSignalWeight:       return "PSFSignalWeight";
   case PSFSNR:                return "PSFSNR";
   case PSFScale:              return "PSFScale";
   case PSFScaleSNR:           return "PSFScaleSNR";
   case Azimuth:               return "Azimuth";
   case Altitude:              return "Altitude";
   case PSFFlux:               return "PSFFlux";
   case PSFFluxPower:          return "PSFFluxPower";
   case PSFTotalMeanFlux:      return "PSFTotalMeanFlux";
   case PSFTotalMeanPowerFlux: return "PSFTotalMeanPowerFlux";
   case PSFCount:              return "PSFCount";
   case MStar:                 return "MStar";
   case NStar:                 return "NStar";
   }
}

IsoString SSGraphPropertyBase::ElementAliases() const
{
   return "SNRWeight=SNR";
}

int SSGraphPropertyBase::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSGraphPropertyBase::ElementLabel( size_type i ) const
{
   switch ( i )
   {
   default:
   case Weight:                return "Weight";
   case FWHM:                  return "FWHM";
   case Eccentricity:          return "Eccentricity";
   case SNRWeight:             return "SNR";
   case Median:                return "Median";
   case MedianMeanDev:         return "Median Mean Dev.";
   case Noise:                 return "Noise";
   case NoiseRatio:            return "Noise Ratio";
   case Stars:                 return "Stars";
   case StarResidual:          return "Star Residual";
   case FWHMMeanDev:           return "FWHM Mean Dev.";
   case EccentricityMeanDev:   return "Eccentricity Mean Dev.";
   case StarResidualMeanDev:   return "Star Residual Mean Dev.";
   case PSFSignalWeight:       return "PSF Signal Weight";
   case PSFSNR:                return "PSF SNR";
   case PSFScale:              return "PSF Scale";
   case PSFScaleSNR:           return "PSF Scale SNR";
   case Azimuth:               return "Azimuth";
   case Altitude:              return "Altitude";
   case PSFFlux:               return "PSF Flux";
   case PSFFluxPower:          return "PSF Flux Power";
   case PSFTotalMeanFlux:      return "PSF Total Mean Flux";
   case PSFTotalMeanPowerFlux: return "PSF Total Mean Power Flux";
   case PSFCount:              return "PSF Count";
   case MStar:                 return "M*";
   case NStar:                 return "N*";
   }
}

size_type SSGraphPropertyBase::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

SSGraphProperty::SSGraphProperty( MetaProcess* P ) : SSGraphPropertyBase( P )
{
   TheSSGraphPropertyParameter = this;
}

IsoString SSGraphProperty::Id() const
{
   return "graphProperty";
}

// ----------------------------------------------------------------------------

SSAuxGraphProperty::SSAuxGraphProperty( MetaProcess* P ) : SSGraphPropertyBase( P )
{
   TheSSAuxGraphPropertyParameter = this;
}

IsoString SSAuxGraphProperty::Id() const
{
   return "auxGraphProperty";
}

// ----------------------------------------------------------------------------

SSUseFileThreads::SSUseFileThreads( MetaProcess* p ) : MetaBoolean( p )
{
   TheSSUseFileThreadsParameter = this;
}

IsoString SSUseFileThreads::Id() const
{
   return "useFileThreads";
}

bool SSUseFileThreads::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSFileThreadOverload::SSFileThreadOverload( MetaProcess* p ) : MetaFloat( p )
{
   TheSSFileThreadOverloadParameter = this;
}

IsoString SSFileThreadOverload::Id() const
{
   return "fileThreadOverload";
}

int SSFileThreadOverload::Precision() const
{
   return 2;
}

double SSFileThreadOverload::DefaultValue() const
{
   return 1.0;
}

double SSFileThreadOverload::MinimumValue() const
{
   return 1;
}

double SSFileThreadOverload::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

SSMaxFileReadThreads::SSMaxFileReadThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheSSMaxFileReadThreadsParameter = this;
}

IsoString SSMaxFileReadThreads::Id() const
{
   return "maxFileReadThreads";
}

double SSMaxFileReadThreads::DefaultValue() const
{
   return 0; // retrieve from global preferences
}

double SSMaxFileReadThreads::MinimumValue() const
{
   return 0;
}

double SSMaxFileReadThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

SSMaxFileWriteThreads::SSMaxFileWriteThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheSSMaxFileWriteThreadsParameter = this;
}

IsoString SSMaxFileWriteThreads::Id() const
{
   return "maxFileWriteThreads";
}

double SSMaxFileWriteThreads::DefaultValue() const
{
   return 0; // retrieve from global preferences
}

double SSMaxFileWriteThreads::MinimumValue() const
{
   return 0;
}

double SSMaxFileWriteThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

SSMeasurements::SSMeasurements( MetaProcess* P ) : MetaTable( P )
{
   TheSSMeasurementsParameter = this;
}

IsoString SSMeasurements::Id() const
{
   return "measurements";
}

bool SSMeasurements::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementIndex::SSMeasurementIndex( MetaTable* T ) : MetaUInt32( T )
{
   TheSSMeasurementIndexParameter = this;
}

IsoString SSMeasurementIndex::Id() const
{
   return "measurementIndex";
}

double SSMeasurementIndex::DefaultValue() const
{
   return 1;
}

double SSMeasurementIndex::MinimumValue() const
{
   return 1;
}

double SSMeasurementIndex::MaximumValue() const
{
   return uint32_max;
}

bool SSMeasurementIndex::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementEnabled::SSMeasurementEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheSSMeasurementEnabledParameter = this;
}

IsoString SSMeasurementEnabled::Id() const
{
   return "measurementEnabled";
}

bool SSMeasurementEnabled::DefaultValue() const
{
   return true;
}

bool SSMeasurementEnabled::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementLocked::SSMeasurementLocked( MetaTable* T ) : MetaBoolean( T )
{
   TheSSMeasurementLockedParameter = this;
}

IsoString SSMeasurementLocked::Id() const
{
   return "measurementLocked";
}

bool SSMeasurementLocked::DefaultValue() const
{
   return false;
}

bool SSMeasurementLocked::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementPath::SSMeasurementPath( MetaTable* T ) : MetaString( T )
{
   TheSSMeasurementPathParameter = this;
}

IsoString SSMeasurementPath::Id() const
{
   return "measurementPath";
}

bool SSMeasurementPath::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementWeight::SSMeasurementWeight( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementWeightParameter = this;
}

IsoString SSMeasurementWeight::Id() const
{
   return "measurementWeight";
}

int SSMeasurementWeight::Precision() const
{
   return 4;
}

double SSMeasurementWeight::DefaultValue() const
{
   return 0;
}

double SSMeasurementWeight::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementWeight::MaximumValue() const
{
   return 10000.0;
}

bool SSMeasurementWeight::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementFWHM::SSMeasurementFWHM( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementFWHMParameter = this;
}

IsoString SSMeasurementFWHM::Id() const
{
   return "measurementFWHM";
}

int SSMeasurementFWHM::Precision() const
{
   return 4;
}

double SSMeasurementFWHM::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementFWHM::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementFWHM::MaximumValue() const
{
   return 100.0;
}

bool SSMeasurementFWHM::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementEccentricity::SSMeasurementEccentricity( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementEccentricityParameter = this;
}

IsoString SSMeasurementEccentricity::Id() const
{
   return "measurementEccentricity";
}

int SSMeasurementEccentricity::Precision() const
{
   return 4;
}

double SSMeasurementEccentricity::DefaultValue() const
{
   return 0.5;
}

double SSMeasurementEccentricity::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementEccentricity::MaximumValue() const
{
   return 1.0;
}

bool SSMeasurementEccentricity::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementPSFSignalWeight::SSMeasurementPSFSignalWeight( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementPSFSignalWeightParameter = this;
}

IsoString SSMeasurementPSFSignalWeight::Id() const
{
   return "measurementPSFSignalWeight";
}

int SSMeasurementPSFSignalWeight::Precision() const
{
   return 4;
}

bool SSMeasurementPSFSignalWeight::ScientificNotation() const
{
   return true;
}

double SSMeasurementPSFSignalWeight::DefaultValue() const
{
   return 0.0;
}

bool SSMeasurementPSFSignalWeight::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementPSFSNR::SSMeasurementPSFSNR( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementPSFSNRParameter = this;
}

IsoString SSMeasurementPSFSNR::Id() const
{
   return "measurementPSFSNR";
}

int SSMeasurementPSFSNR::Precision() const
{
   return 4;
}

bool SSMeasurementPSFSNR::ScientificNotation() const
{
   return true;
}

double SSMeasurementPSFSNR::DefaultValue() const
{
   return 0.0;
}

bool SSMeasurementPSFSNR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementPSFScale::SSMeasurementPSFScale( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementPSFScaleParameter = this;
}

IsoString SSMeasurementPSFScale::Id() const
{
   return "measurementPSFScale";
}

int SSMeasurementPSFScale::Precision() const
{
   return 4;
}

bool SSMeasurementPSFScale::ScientificNotation() const
{
   return true;
}

double SSMeasurementPSFScale::DefaultValue() const
{
   return 0.0;
}

bool SSMeasurementPSFScale::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementPSFScaleSNR::SSMeasurementPSFScaleSNR( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementPSFScaleSNRParameter = this;
}

IsoString SSMeasurementPSFScaleSNR::Id() const
{
   return "measurementPSFScaleSNR";
}

int SSMeasurementPSFScaleSNR::Precision() const
{
   return 4;
}

bool SSMeasurementPSFScaleSNR::ScientificNotation() const
{
   return true;
}

double SSMeasurementPSFScaleSNR::DefaultValue() const
{
   return 0.0;
}

bool SSMeasurementPSFScaleSNR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementPSFFlux::SSMeasurementPSFFlux( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementPSFFluxParameter = this;
}

IsoString SSMeasurementPSFFlux::Id() const
{
   return "measurementPSFFlux";
}

int SSMeasurementPSFFlux::Precision() const
{
   return 4;
}

bool SSMeasurementPSFFlux::ScientificNotation() const
{
   return true;
}

double SSMeasurementPSFFlux::DefaultValue() const
{
   return 0.0;
}

bool SSMeasurementPSFFlux::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementPSFFluxPower::SSMeasurementPSFFluxPower( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementPSFFluxPowerParameter = this;
}

IsoString SSMeasurementPSFFluxPower::Id() const
{
   return "measurementPSFFluxPower";
}

int SSMeasurementPSFFluxPower::Precision() const
{
   return 4;
}

bool SSMeasurementPSFFluxPower::ScientificNotation() const
{
   return true;
}

double SSMeasurementPSFFluxPower::DefaultValue() const
{
   return 0.0;
}

bool SSMeasurementPSFFluxPower::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementPSFTotalMeanFlux::SSMeasurementPSFTotalMeanFlux( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementPSFTotalMeanFluxParameter = this;
}

IsoString SSMeasurementPSFTotalMeanFlux::Id() const
{
   return "measurementPSFTotalMeanFlux";
}

int SSMeasurementPSFTotalMeanFlux::Precision() const
{
   return 4;
}

bool SSMeasurementPSFTotalMeanFlux::ScientificNotation() const
{
   return true;
}

double SSMeasurementPSFTotalMeanFlux::DefaultValue() const
{
   return 0.0;
}

bool SSMeasurementPSFTotalMeanFlux::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementPSFTotalMeanPowerFlux::SSMeasurementPSFTotalMeanPowerFlux( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementPSFTotalMeanPowerFluxParameter = this;
}

IsoString SSMeasurementPSFTotalMeanPowerFlux::Id() const
{
   return "measurementPSFTotalMeanPowerFlux";
}

int SSMeasurementPSFTotalMeanPowerFlux::Precision() const
{
   return 4;
}

bool SSMeasurementPSFTotalMeanPowerFlux::ScientificNotation() const
{
   return true;
}

double SSMeasurementPSFTotalMeanPowerFlux::DefaultValue() const
{
   return 0.0;
}

bool SSMeasurementPSFTotalMeanPowerFlux::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementPSFCount::SSMeasurementPSFCount( MetaTable* T ) : MetaUInt32( T )
{
   TheSSMeasurementPSFCountParameter = this;
}

IsoString SSMeasurementPSFCount::Id() const
{
   return "measurementPSFCount";
}

double SSMeasurementPSFCount::DefaultValue() const
{
   return 0;
}

double SSMeasurementPSFCount::MinimumValue() const
{
   return 0;
}

double SSMeasurementPSFCount::MaximumValue() const
{
   return uint32_max;
}

bool SSMeasurementPSFCount::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementMStar::SSMeasurementMStar( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementMStarParameter = this;
}

IsoString SSMeasurementMStar::Id() const
{
   return "measurementMStar";
}

int SSMeasurementMStar::Precision() const
{
   return 4;
}

bool SSMeasurementMStar::ScientificNotation() const
{
   return true;
}

double SSMeasurementMStar::DefaultValue() const
{
   return 0.0;
}

bool SSMeasurementMStar::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementNStar::SSMeasurementNStar( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementNStarParameter = this;
}

IsoString SSMeasurementNStar::Id() const
{
   return "measurementNStar";
}

int SSMeasurementNStar::Precision() const
{
   return 4;
}

bool SSMeasurementNStar::ScientificNotation() const
{
   return true;
}

double SSMeasurementNStar::DefaultValue() const
{
   return 0.0;
}

bool SSMeasurementNStar::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementSNRWeight::SSMeasurementSNRWeight( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementSNRWeightParameter = this;
}

IsoString SSMeasurementSNRWeight::Id() const
{
   return "measurementSNRWeight";
}

int SSMeasurementSNRWeight::Precision() const
{
   return 4;
}

bool SSMeasurementSNRWeight::ScientificNotation() const
{
   return true;
}

double SSMeasurementSNRWeight::DefaultValue() const
{
   return 0.0;
}

bool SSMeasurementSNRWeight::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementMedian::SSMeasurementMedian( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementMedianParameter = this;
}

IsoString SSMeasurementMedian::Id() const
{
   return "measurementMedian";
}

int SSMeasurementMedian::Precision() const
{
   return 12;
}

double SSMeasurementMedian::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementMedian::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementMedian::MaximumValue() const
{
   return 1.0;
}

bool SSMeasurementMedian::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementMedianMeanDev::SSMeasurementMedianMeanDev( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementMedianMeanDevParameter = this;
}

IsoString SSMeasurementMedianMeanDev::Id() const
{
   return "measurementMedianMeanDev";
}

int SSMeasurementMedianMeanDev::Precision() const
{
   return 12;
}

double SSMeasurementMedianMeanDev::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementMedianMeanDev::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementMedianMeanDev::MaximumValue() const
{
   return 1.0;
}

bool SSMeasurementMedianMeanDev::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementNoise::SSMeasurementNoise( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementNoiseParameter = this;
}

IsoString SSMeasurementNoise::Id() const
{
   return "measurementNoise";
}

int SSMeasurementNoise::Precision() const
{
   return 12;
}

double SSMeasurementNoise::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementNoise::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementNoise::MaximumValue() const
{
   return 1.0;
}

bool SSMeasurementNoise::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementNoiseRatio::SSMeasurementNoiseRatio( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementNoiseRatioParameter = this;
}

IsoString SSMeasurementNoiseRatio::Id() const
{
   return "measurementNoiseRatio";
}

int SSMeasurementNoiseRatio::Precision() const
{
   return 8;
}

double SSMeasurementNoiseRatio::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementNoiseRatio::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementNoiseRatio::MaximumValue() const
{
   return 1.0;
}

bool SSMeasurementNoiseRatio::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementStars::SSMeasurementStars( MetaTable* T ) : MetaUInt32( T )
{
   TheSSMeasurementStarsParameter = this;
}

IsoString SSMeasurementStars::Id() const
{
   return "measurementStars";
}

double SSMeasurementStars::DefaultValue() const
{
   return 0;
}

double SSMeasurementStars::MinimumValue() const
{
   return 0;
}

double SSMeasurementStars::MaximumValue() const
{
   return uint32_max;
}

bool SSMeasurementStars::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementStarResidual::SSMeasurementStarResidual( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementStarResidualParameter = this;
}

IsoString SSMeasurementStarResidual::Id() const
{
   return "measurementStarResidual";
}

int SSMeasurementStarResidual::Precision() const
{
   return 4;
}

double SSMeasurementStarResidual::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementStarResidual::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementStarResidual::MaximumValue() const
{
   return DBL_MAX;
}

bool SSMeasurementStarResidual::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementFWHMMeanDev::SSMeasurementFWHMMeanDev( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementFWHMMeanDevParameter = this;
}

IsoString SSMeasurementFWHMMeanDev::Id() const
{
   return "measurementFWHMMeanDev";
}

int SSMeasurementFWHMMeanDev::Precision() const
{
   return 6;
}

double SSMeasurementFWHMMeanDev::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementFWHMMeanDev::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementFWHMMeanDev::MaximumValue() const
{
   return DBL_MAX;
}

bool SSMeasurementFWHMMeanDev::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementEccentricityMeanDev::SSMeasurementEccentricityMeanDev( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementEccentricityMeanDevParameter = this;
}

IsoString SSMeasurementEccentricityMeanDev::Id() const
{
   return "measurementEccentricityMeanDev";
}

int SSMeasurementEccentricityMeanDev::Precision() const
{
   return 4;
}

double SSMeasurementEccentricityMeanDev::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementEccentricityMeanDev::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementEccentricityMeanDev::MaximumValue() const
{
   return DBL_MAX;
}

bool SSMeasurementEccentricityMeanDev::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementStarResidualMeanDev::SSMeasurementStarResidualMeanDev( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementStarResidualMeanDevParameter = this;
}

IsoString SSMeasurementStarResidualMeanDev::Id() const
{
   return "measurementStarResidualMeanDev";
}

int SSMeasurementStarResidualMeanDev::Precision() const
{
   return 4;
}

double SSMeasurementStarResidualMeanDev::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementStarResidualMeanDev::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementStarResidualMeanDev::MaximumValue() const
{
   return DBL_MAX;
}

bool SSMeasurementStarResidualMeanDev::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementAzimuth::SSMeasurementAzimuth( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementAzimuthParameter = this;
}

IsoString SSMeasurementAzimuth::Id() const
{
   return "measurementAzimuth";
}

int SSMeasurementAzimuth::Precision() const
{
   return 4;
}

double SSMeasurementAzimuth::DefaultValue() const
{
   return 0;
}

double SSMeasurementAzimuth::MinimumValue() const
{
   return 0;
}

double SSMeasurementAzimuth::MaximumValue() const
{
   return 360;
}

bool SSMeasurementAzimuth::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementAltitude::SSMeasurementAltitude( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementAltitudeParameter = this;
}

IsoString SSMeasurementAltitude::Id() const
{
   return "measurementAltitude";
}

int SSMeasurementAltitude::Precision() const
{
   return 4;
}

double SSMeasurementAltitude::DefaultValue() const
{
   return 0;
}

double SSMeasurementAltitude::MinimumValue() const
{
   return 0;
}

double SSMeasurementAltitude::MaximumValue() const
{
   return 90;
}

bool SSMeasurementAltitude::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementUnused01::SSMeasurementUnused01( MetaTable* T ) : MetaDouble( T )
{
   TheSSMeasurementUnused01Parameter = this;
}

IsoString SSMeasurementUnused01::Id() const
{
   return "measurementUnused01";
}

int SSMeasurementUnused01::Precision() const
{
   return 0;
}

double SSMeasurementUnused01::DefaultValue() const
{
   return 0;
}

bool SSMeasurementUnused01::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorParameters.cpp - Released 2023-07-06T16:53:46Z
