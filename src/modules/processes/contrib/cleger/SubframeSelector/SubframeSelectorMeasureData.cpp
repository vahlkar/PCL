//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.3
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.8.5
// ----------------------------------------------------------------------------
// SubframeSelectorMeasureData.cpp - Released 2023-05-17T17:06:42Z
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

#include "SubframeSelectorInstance.h"
#include "SubframeSelectorMeasureData.h"

namespace pcl
{

// ----------------------------------------------------------------------------

void MeasureData::ResetCacheableData()
{
   fwhm = TheSSMeasurementFWHMParameter->DefaultValue();
   fwhmMeanDev           = 0;
   eccentricity          = TheSSMeasurementEccentricityParameter->DefaultValue();
   eccentricityMeanDev   = 0;
   psfSignalWeight       = TheSSMeasurementPSFSignalWeightParameter->DefaultValue();
   psfSNR                = TheSSMeasurementPSFSNRParameter->DefaultValue();
   psfScale              = TheSSMeasurementPSFScaleParameter->DefaultValue();
   psfScaleSNR           = TheSSMeasurementPSFScaleSNRParameter->DefaultValue();
   psfFlux               = TheSSMeasurementPSFFluxParameter->DefaultValue();
   psfFluxPower          = TheSSMeasurementPSFFluxPowerParameter->DefaultValue();
   psfTotalMeanFlux      = TheSSMeasurementPSFTotalMeanFluxParameter->DefaultValue();
   psfTotalMeanPowerFlux = TheSSMeasurementPSFTotalMeanPowerFluxParameter->DefaultValue();
   psfCount              = uint32( TheSSMeasurementPSFCountParameter->DefaultValue() );
   MStar                 = TheSSMeasurementMStarParameter->DefaultValue();
   NStar                 = TheSSMeasurementNStarParameter->DefaultValue();
   snrWeight             = TheSSMeasurementSNRWeightParameter->DefaultValue();
   median                = TheSSMeasurementMedianParameter->DefaultValue();
   medianMeanDev         = 0;
   noise                 = TheSSMeasurementNoiseParameter->DefaultValue();
   noiseRatio            = TheSSMeasurementNoiseRatioParameter->DefaultValue();
   stars                 = uint32( TheSSMeasurementStarsParameter->DefaultValue() );
   starResidual          = TheSSMeasurementStarResidualParameter->DefaultValue();
   starResidualMeanDev   = 0;
   azimuth               = TheSSMeasurementAzimuthParameter->DefaultValue();
   altitude              = TheSSMeasurementAltitudeParameter->DefaultValue();
}

// ----------------------------------------------------------------------------

void MeasureData::AddToCache( const SubframeSelectorInstance& instance ) const
{
   if ( TheSubframeSelectorCache != nullptr )
   {
      SubframeSelectorCacheItem item( path );
      item.fwhm                  = fwhm;
      item.fwhmMeanDev           = fwhmMeanDev;
      item.eccentricity          = eccentricity;
      item.eccentricityMeanDev   = eccentricityMeanDev;
      item.psfSignalWeight       = psfSignalWeight;
      item.psfSNR                = psfSNR;
      item.psfScale              = psfScale;
      item.psfScaleSNR           = psfScaleSNR;
      item.psfFlux               = psfFlux;
      item.psfFluxPower          = psfFluxPower;
      item.psfTotalMeanFlux      = psfTotalMeanFlux;
      item.psfTotalMeanPowerFlux = psfTotalMeanPowerFlux;
      item.psfCount              = psfCount;
      item.MStar                 = MStar;
      item.NStar                 = NStar;
      item.snrWeight             = snrWeight;
      item.median                = median;
      item.medianMeanDev         = medianMeanDev;
      item.noise                 = noise;
      item.noiseRatio            = noiseRatio;
      item.stars                 = stars;
      item.starResidual          = starResidual;
      item.starResidualMeanDev   = starResidualMeanDev;
      item.azimuth               = azimuth;
      item.altitude              = altitude;
      item.instanceParameters    = instance.EncodedCacheSensitiveParameters();
      TheSubframeSelectorCache->Add( item );
   }
}

// ----------------------------------------------------------------------------

bool MeasureData::GetFromCache( const SubframeSelectorInstance& instance )
{
   ResetCacheableData();

   if ( TheSubframeSelectorCache != nullptr )
   {
      SubframeSelectorCacheItem item;
      if ( TheSubframeSelectorCache->Get( item, path ) )
      {
         fwhm                  = item.fwhm;
         fwhmMeanDev           = item.fwhmMeanDev;
         eccentricity          = item.eccentricity;
         eccentricityMeanDev   = item.eccentricityMeanDev;
         psfSignalWeight       = item.psfSignalWeight;
         psfSNR                = item.psfSNR;
         psfScale              = item.psfScale;
         psfScaleSNR           = item.psfScaleSNR;
         psfFlux               = item.psfFlux;
         psfFluxPower          = item.psfFluxPower;
         psfTotalMeanFlux      = item.psfTotalMeanFlux;
         psfTotalMeanPowerFlux = item.psfTotalMeanPowerFlux;
         psfCount              = item.psfCount;
         MStar                 = item.MStar;
         NStar                 = item.NStar;
         snrWeight             = item.snrWeight;
         median                = item.median;
         medianMeanDev         = item.medianMeanDev;
         noise                 = item.noise;
         noiseRatio            = item.noiseRatio;
         stars                 = item.stars;
         starResidual          = item.starResidual;
         starResidualMeanDev   = item.starResidualMeanDev;
         azimuth               = item.azimuth;
         altitude              = item.altitude;

         return item.instanceParameters == instance.EncodedCacheSensitiveParameters();
      }
   }

   return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

String MeasureItem::JavaScriptParameters( double subframeScale, int scaleUnit, double cameraGain,
                                          int cameraResolution, int dataUnit, const MeasureProperties& properties ) const
{
   return

   String().Format( "let Index = %i;\n", index ) +

   String().Format( "let Weight = %.15e;\n", weight ) +
   String().Format( "let WeightMin = %.15e;\n", properties.weight.min ) +
   String().Format( "let WeightMax = %.15e;\n", properties.weight.max ) +
   String().Format( "let WeightMedian = %.15e;\n", properties.weight.median ) +
   String().Format( "let WeightSigma = %.15e;\n", DeviationNormalize(
      weight, properties.weight.median, properties.weight.deviation ) ) +

   String().Format( "let FWHM = %.15e;\n", FWHM( subframeScale, scaleUnit ) ) +
   String().Format( "let FWHMMin = %.15e;\n", properties.fwhm.min ) +
   String().Format( "let FWHMMax = %.15e;\n", properties.fwhm.max ) +
   String().Format( "let FWHMMedian = %.15e;\n", properties.fwhm.median ) +
   String().Format( "let FWHMSigma = %.15e;\n", DeviationNormalize( FWHM(
                  subframeScale, scaleUnit ), properties.fwhm.median, properties.fwhm.deviation ) ) +

   String().Format( "let Eccentricity = %.15e;\n", eccentricity ) +
   String().Format( "let EccentricityMin = %.15e;\n", properties.eccentricity.min ) +
   String().Format( "let EccentricityMax = %.15e;\n", properties.eccentricity.max ) +
   String().Format( "let EccentricityMedian = %.15e;\n", properties.eccentricity.median ) +
   String().Format( "let EccentricitySigma = %.15e;\n", DeviationNormalize(
                  eccentricity, properties.eccentricity.median, properties.eccentricity.deviation ) ) +

   /**/
   String().Format( "let PSFSignalWeight = %.15e;\n", psfSignalWeight ) +
   String().Format( "let PSFSignalWeightMin = %.15e;\n", properties.psfSignalWeight.min ) +
   String().Format( "let PSFSignalWeightMax = %.15e;\n", properties.psfSignalWeight.max ) +
   String().Format( "let PSFSignalWeightMedian = %.15e;\n", properties.psfSignalWeight.median ) +
   String().Format( "let PSFSignalWeightSigma = %.15e;\n", DeviationNormalize(
                  psfSignalWeight, properties.psfSignalWeight.median, properties.psfSignalWeight.deviation ) ) +

   String().Format( "let PSFSignal = %.15e;\n", psfSignalWeight ) +
   String().Format( "let PSFSignalMin = %.15e;\n", properties.psfSignalWeight.min ) +
   String().Format( "let PSFSignalMax = %.15e;\n", properties.psfSignalWeight.max ) +
   String().Format( "let PSFSignalMedian = %.15e;\n", properties.psfSignalWeight.median ) +
   String().Format( "let PSFSignalSigma = %.15e;\n", DeviationNormalize(
                  psfSignalWeight, properties.psfSignalWeight.median, properties.psfSignalWeight.deviation ) ) +
   /**/

   String().Format( "let PSFSNR = %.15e;\n", psfSNR ) +
   String().Format( "let PSFSNRMin = %.15e;\n", properties.psfSNR.min ) +
   String().Format( "let PSFSNRMax = %.15e;\n", properties.psfSNR.max ) +
   String().Format( "let PSFSNRMedian = %.15e;\n", properties.psfSNR.median ) +
   String().Format( "let PSFSNRSigma = %.15e;\n", DeviationNormalize(
                  psfSNR, properties.psfSNR.median, properties.psfSNR.deviation ) ) +

   String().Format( "let PSFScale = %.15e;\n", psfScale ) +
   String().Format( "let PSFScaleMin = %.15e;\n", properties.psfScale.min ) +
   String().Format( "let PSFScaleMax = %.15e;\n", properties.psfScale.max ) +
   String().Format( "let PSFScaleMedian = %.15e;\n", properties.psfScale.median ) +
   String().Format( "let PSFScaleSigma = %.15e;\n", DeviationNormalize(
                  psfScale, properties.psfScale.median, properties.psfScale.deviation ) ) +

   String().Format( "let PSFScaleSNR = %.15e;\n", psfScaleSNR ) +
   String().Format( "let PSFScaleSNRMin = %.15e;\n", properties.psfScaleSNR.min ) +
   String().Format( "let PSFScaleSNRMax = %.15e;\n", properties.psfScaleSNR.max ) +
   String().Format( "let PSFScaleSNRMedian = %.15e;\n", properties.psfScaleSNR.median ) +
   String().Format( "let PSFScaleSNRSigma = %.15e;\n", DeviationNormalize(
                  psfScaleSNR, properties.psfScaleSNR.median, properties.psfScaleSNR.deviation ) ) +

   /**/

   String().Format( "let PSFFlux = %.15e;\n", psfFlux ) +
   String().Format( "let PSFFluxMin = %.15e;\n", properties.psfFlux.min ) +
   String().Format( "let PSFFluxMax = %.15e;\n", properties.psfFlux.max ) +
   String().Format( "let PSFFluxMedian = %.15e;\n", properties.psfFlux.median ) +
   String().Format( "let PSFFluxSigma = %.15e;\n", DeviationNormalize(
                  psfFlux, properties.psfFlux.median, properties.psfFlux.deviation ) ) +

   String().Format( "let PSFFluxPower = %.15e;\n", psfFluxPower ) +
   String().Format( "let PSFFluxPowerMin = %.15e;\n", properties.psfFluxPower.min ) +
   String().Format( "let PSFFluxPowerMax = %.15e;\n", properties.psfFluxPower.max ) +
   String().Format( "let PSFFluxPowerMedian = %.15e;\n", properties.psfFluxPower.median ) +
   String().Format( "let PSFFluxPowerSigma = %.15e;\n", DeviationNormalize(
                  psfFluxPower, properties.psfFluxPower.median, properties.psfFluxPower.deviation ) ) +

   /**/

   String().Format( "let PSFTotalMeanFlux = %.15e;\n", psfTotalMeanFlux ) +
   String().Format( "let PSFTotalMeanFluxMin = %.15e;\n", properties.psfTotalMeanFlux.min ) +
   String().Format( "let PSFTotalMeanFluxMax = %.15e;\n", properties.psfTotalMeanFlux.max ) +
   String().Format( "let PSFTotalMeanFluxMedian = %.15e;\n", properties.psfTotalMeanFlux.median ) +
   String().Format( "let PSFTotalMeanFluxSigma = %.15e;\n", DeviationNormalize(
                  psfTotalMeanFlux, properties.psfTotalMeanFlux.median, properties.psfTotalMeanFlux.deviation ) ) +

   String().Format( "let PSFTotalMeanPowerFlux = %.15e;\n", psfTotalMeanPowerFlux ) +
   String().Format( "let PSFTotalMeanPowerFluxMin = %.15e;\n", properties.psfTotalMeanPowerFlux.min ) +
   String().Format( "let PSFTotalMeanPowerFluxMax = %.15e;\n", properties.psfTotalMeanPowerFlux.max ) +
   String().Format( "let PSFTotalMeanPowerFluxMedian = %.15e;\n", properties.psfTotalMeanPowerFlux.median ) +
   String().Format( "let PSFTotalMeanPowerFluxSigma = %.15e;\n", DeviationNormalize(
                  psfTotalMeanPowerFlux, properties.psfTotalMeanPowerFlux.median, properties.psfTotalMeanPowerFlux.deviation ) ) +

   String().Format( "let PSFCount = %u;\n", psfCount ) +
   String().Format( "let PSFCountMin = %.15e;\n", properties.psfCount.min ) +
   String().Format( "let PSFCountMax = %.15e;\n", properties.psfCount.max ) +
   String().Format( "let PSFCountMedian = %.15e;\n", properties.psfCount.median ) +
   String().Format( "let PSFCountSigma = %.15e;\n", DeviationNormalize(
                  psfCount, properties.psfCount.median, properties.psfCount.deviation ) ) +

   String().Format( "let MStar = %.15e;\n", MStar ) +
   String().Format( "let MStarMin = %.15e;\n", properties.MStar.min ) +
   String().Format( "let MStarMax = %.15e;\n", properties.MStar.max ) +
   String().Format( "let MStarMedian = %.15e;\n", properties.MStar.median ) +
   String().Format( "let MStarSigma = %.15e;\n", DeviationNormalize(
                  MStar, properties.MStar.median, properties.MStar.deviation ) ) +

   String().Format( "let NStar = %.15e;\n", NStar ) +
   String().Format( "let NStarMin = %.15e;\n", properties.NStar.min ) +
   String().Format( "let NStarMax = %.15e;\n", properties.NStar.max ) +
   String().Format( "let NStarMedian = %.15e;\n", properties.NStar.median ) +
   String().Format( "let NStarSigma = %.15e;\n", DeviationNormalize(
                  NStar, properties.NStar.median, properties.NStar.deviation ) ) +

   /**/

   // Obsolete SNRWeight, for compatibility
   String().Format( "let SNRWeight = %.15e;\n", snrWeight ) +
   String().Format( "let SNRWeightMin = %.15e;\n", properties.snrWeight.min ) +
   String().Format( "let SNRWeightMax = %.15e;\n", properties.snrWeight.max ) +
   String().Format( "let SNRWeightMedian = %.15e;\n", properties.snrWeight.median ) +
   String().Format( "let SNRWeightSigma = %.15e;\n", DeviationNormalize(
                  snrWeight, properties.snrWeight.median, properties.snrWeight.deviation ) ) +

   String().Format( "let SNR = %.15e;\n", snrWeight ) +
   String().Format( "let SNRMin = %.15e;\n", properties.snrWeight.min ) +
   String().Format( "let SNRMax = %.15e;\n", properties.snrWeight.max ) +
   String().Format( "let SNRMedian = %.15e;\n", properties.snrWeight.median ) +
   String().Format( "let SNRSigma = %.15e;\n", DeviationNormalize(
                  snrWeight, properties.snrWeight.median, properties.snrWeight.deviation ) ) +
   /**/

   String().Format( "let Median = %.15e;\n", Median( cameraGain, cameraResolution, dataUnit ) ) +
   String().Format( "let MedianMin = %.15e;\n", properties.median.min ) +
   String().Format( "let MedianMax = %.15e;\n", properties.median.max ) +
   String().Format( "let MedianMedian = %.15e;\n", properties.median.median ) +
   String().Format( "let MedianSigma = %.15e;\n", DeviationNormalize(
                  Median( cameraGain, cameraResolution, dataUnit ),
                  properties.median.median, properties.median.deviation ) ) +

   String().Format( "let MedianMeanDev = %.15e;\n", MedianMeanDev( cameraGain, cameraResolution, dataUnit ) ) +
   String().Format( "let MedianMeanDevMin = %.15e;\n", properties.medianMeanDev.min ) +
   String().Format( "let MedianMeanDevMax = %.15e;\n", properties.medianMeanDev.max ) +
   String().Format( "let MedianMeanDevMedian = %.15e;\n", properties.medianMeanDev.median ) +
   String().Format( "let MedianMeanDevSigma = %.15e;\n", DeviationNormalize(
                  MedianMeanDev( cameraGain, cameraResolution, dataUnit ),
                  properties.medianMeanDev.median, properties.medianMeanDev.deviation ) ) +

   String().Format( "let Noise = %.15e;\n", Noise( cameraGain, cameraResolution, dataUnit ) ) +
   String().Format( "let NoiseMin = %.15e;\n", properties.noise.min ) +
   String().Format( "let NoiseMax = %.15e;\n", properties.noise.max ) +
   String().Format( "let NoiseMedian = %.15e;\n", properties.noise.median ) +
   String().Format( "let NoiseSigma = %.15e;\n", DeviationNormalize(
                  Noise( cameraGain, cameraResolution, dataUnit ),
                  properties.noise.median, properties.noise.deviation ) ) +

   String().Format( "let NoiseRatio = %.15e;\n", noiseRatio ) +
   String().Format( "let NoiseRatioMin = %.15e;\n", properties.noiseRatio.min ) +
   String().Format( "let NoiseRatioMax = %.15e;\n", properties.noiseRatio.max ) +
   String().Format( "let NoiseRatioMedian = %.15e;\n", properties.noiseRatio.median ) +
   String().Format( "let NoiseRatioSigma = %.15e;\n", DeviationNormalize(
                  noiseRatio, properties.noiseRatio.median, properties.noiseRatio.deviation ) ) +

   String().Format( "let Stars = %u;\n", stars ) +
   String().Format( "let StarsMin = %.15e;\n", properties.stars.min ) +
   String().Format( "let StarsMax = %.15e;\n", properties.stars.max ) +
   String().Format( "let StarsMedian = %.15e;\n", properties.stars.median ) +
   String().Format( "let StarsSigma = %.15e;\n", DeviationNormalize(
                  stars, properties.stars.median, properties.stars.deviation ) ) +

   String().Format( "let StarResidual = %.15e;\n", starResidual ) +
   String().Format( "let StarResidualMin = %.15e;\n", properties.starResidual.min ) +
   String().Format( "let StarResidualMax = %.15e;\n", properties.starResidual.max ) +
   String().Format( "let StarResidualMedian = %.15e;\n", properties.starResidual.median ) +
   String().Format( "let StarResidualSigma = %.15e;\n", DeviationNormalize(
                  starResidual, properties.starResidual.median, properties.starResidual.deviation ) ) +

   String().Format( "let FWHMMeanDev = %.15e;\n", FWHMMeanDeviation( subframeScale, scaleUnit ) ) +
   String().Format( "let FWHMMeanDevMin = %.15e;\n", properties.fwhmMeanDev.min ) +
   String().Format( "let FWHMMeanDevMax = %.15e;\n", properties.fwhmMeanDev.max ) +
   String().Format( "let FWHMMeanDevMedian = %.15e;\n", properties.fwhmMeanDev.median ) +
   String().Format( "let FWHMMeanDevSigma = %.15e;\n", DeviationNormalize(
                  FWHMMeanDeviation( subframeScale, scaleUnit ),
                  properties.fwhmMeanDev.median, properties.fwhmMeanDev.deviation ) ) +

   String().Format( "let EccentricityMeanDev = %.15e;\n", eccentricityMeanDev ) +
   String().Format( "let EccentricityMeanDevMin = %.15e;\n", properties.eccentricityMeanDev.min ) +
   String().Format( "let EccentricityMeanDevMax = %.15e;\n", properties.eccentricityMeanDev.max ) +
   String().Format( "let EccentricityMeanDevMedian = %.15e;\n", properties.eccentricityMeanDev.median ) +
   String().Format( "let EccentricityMeanDevSigma = %.15e;\n", DeviationNormalize(
                  eccentricityMeanDev, properties.eccentricityMeanDev.median, properties.eccentricityMeanDev.deviation ) ) +

   String().Format( "let StarResidualMeanDev = %.15e;\n", starResidualMeanDev ) +
   String().Format( "let StarResidualMeanDevMin = %.15e;\n", properties.starResidualMeanDev.min ) +
   String().Format( "let StarResidualMeanDevMax = %.15e;\n", properties.starResidualMeanDev.max ) +
   String().Format( "let StarResidualMeanDevMedian = %.15e;\n", properties.starResidualMeanDev.median ) +
   String().Format( "let StarResidualMeanDevSigma = %.15e;\n", DeviationNormalize(
                  starResidualMeanDev, properties.starResidualMeanDev.median, properties.starResidualMeanDev.deviation ) ) +

   String().Format( "let Azimuth = %.15e;\n", azimuth ) +
   String().Format( "let AzimuthMin = %.15e;\n", properties.azimuth.min ) +
   String().Format( "let AzimuthMax = %.15e;\n", properties.azimuth.max ) +
   String().Format( "let AzimuthMedian = %.15e;\n", properties.azimuth.median ) +
   String().Format( "let AzimuthSigma = %.15e;\n", DeviationNormalize(
                  azimuth, properties.azimuth.median, properties.azimuth.deviation ) ) +

   String().Format( "let Altitude = %.15e;\n", altitude ) +
   String().Format( "let AltitudeMin = %.15e;\n", properties.altitude.min ) +
   String().Format( "let AltitudeMax = %.15e;\n", properties.altitude.max ) +
   String().Format( "let AltitudeMedian = %.15e;\n", properties.altitude.median ) +
   String().Format( "let AltitudeSigma = %.15e;\n", DeviationNormalize(
                  altitude, properties.altitude.median, properties.altitude.deviation ) );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool MeasureUtils::IsValidExpression( const String& expression )
{
   if ( expression.IsEmpty() )
      return true;

   int pOpen = 0;
   int pClose = 0;
   int bOpen = 0;
   int bClose = 0;
   int cOpen = 0;
   int cClose = 0;
   int a = 0;
   int o = 0;
   for ( char16_type c : expression )
   {
      if (     !CharTraits::IsAlpha( c )
            && !CharTraits::IsDigit( c )
            && !CharTraits::IsSpace( c )
            && c != '(' && c != ')' && c != '[' && c != ']' && c != '{' && c != '}'
            && c != '&' && c != '|' && c != '*' && c != '+' && c != '-' && c != '/'
            && c != '.' && c != ',' && c != '%' && c != '?' && c != ':' && c != '_'
            && c != '<' && c != '>' && c != '=' && c != '!' && c != ';' )
         return false;

      if ( c == '(' )
         ++pOpen;
      if ( c == ')' )
         ++pClose;

      if ( c == '[' )
         ++bOpen;
      if ( c == ']' )
         ++bClose;

      if ( c == '{' )
         ++cOpen;
      if ( c == '}' )
         ++cClose;

      if ( c == '&' )
         ++a;
      if ( c == '|' )
         ++o;
   }

   return pOpen == pClose && bOpen == bClose && cOpen == cClose && a%2 == 0 && o%2 == 0;
}

// ----------------------------------------------------------------------------

void MeasureUtils::MeasureProperties( const MeasureItemList& measures, double subframeScale,
                                      int scaleUnit, double cameraGain,
                                      int cameraResolution, int dataUnit,
                                      pcl::MeasureProperties& properties )
{
   Array<double>
   weight, fwhm, eccentricity,
   psfSignalWeight, psfSNR, psfScale, psfScaleSNR, psfFlux, psfFluxPower, psfTotalMeanFlux, psfTotalMeanPowerFlux, psfCount, MStar, NStar,
   snrWeight, median, medianMeanDev, noise, noiseRatio, stars, starResidual, fwhmMeanDev,
   eccentricityMeanDev, starResidualMeanDev, azimuth, altitude;

   for ( const MeasureItem& item : measures )
   {
      weight << item.weight;
      fwhm << item.FWHM( subframeScale, scaleUnit );
      eccentricity << item.eccentricity;
      psfSignalWeight << item.psfSignalWeight;
      psfSNR << item.psfSNR;
      psfScale << item.psfScale;
      psfScaleSNR << item.psfScaleSNR;
      psfFlux << item.psfFlux;
      psfFluxPower << item.psfFluxPower;
      psfTotalMeanFlux << item.psfTotalMeanFlux;
      psfTotalMeanPowerFlux << item.psfTotalMeanPowerFlux;
      psfCount << item.psfCount;
      MStar << item.MStar;
      NStar << item.NStar;
      snrWeight << item.snrWeight;
      median << item.Median( cameraGain, TheSSCameraResolutionParameter->ElementData( cameraResolution ), dataUnit );
      medianMeanDev << item.MedianMeanDev( cameraGain, TheSSCameraResolutionParameter->ElementData( cameraResolution ), dataUnit );
      noise << item.Noise( cameraGain, TheSSCameraResolutionParameter->ElementData( cameraResolution ), dataUnit );
      noiseRatio << item.noiseRatio;
      stars << item.stars;
      starResidual << item.starResidual;
      fwhmMeanDev << item.FWHMMeanDeviation( subframeScale, scaleUnit );
      eccentricityMeanDev << item.eccentricityMeanDev;
      starResidualMeanDev << item.starResidualMeanDev;
      azimuth << item.azimuth;
      altitude << item.altitude;
   }

   MeasureUtils::MeasureProperty( weight, properties.weight );
   MeasureUtils::MeasureProperty( fwhm, properties.fwhm );
   MeasureUtils::MeasureProperty( eccentricity, properties.eccentricity );
   MeasureUtils::MeasureProperty( psfSignalWeight, properties.psfSignalWeight );
   MeasureUtils::MeasureProperty( psfSNR, properties.psfSNR );
   MeasureUtils::MeasureProperty( psfScale, properties.psfScale );
   MeasureUtils::MeasureProperty( psfScaleSNR, properties.psfScaleSNR );
   MeasureUtils::MeasureProperty( psfFlux, properties.psfFlux );
   MeasureUtils::MeasureProperty( psfFluxPower, properties.psfFluxPower );
   MeasureUtils::MeasureProperty( psfTotalMeanFlux, properties.psfTotalMeanFlux );
   MeasureUtils::MeasureProperty( psfTotalMeanPowerFlux, properties.psfTotalMeanPowerFlux );
   MeasureUtils::MeasureProperty( psfCount, properties.psfCount );
   MeasureUtils::MeasureProperty( MStar, properties.MStar );
   MeasureUtils::MeasureProperty( NStar, properties.NStar );
   MeasureUtils::MeasureProperty( snrWeight, properties.snrWeight );
   MeasureUtils::MeasureProperty( median, properties.median );
   MeasureUtils::MeasureProperty( medianMeanDev, properties.medianMeanDev );
   MeasureUtils::MeasureProperty( noise, properties.noise );
   MeasureUtils::MeasureProperty( noiseRatio, properties.noiseRatio );
   MeasureUtils::MeasureProperty( stars, properties.stars );
   MeasureUtils::MeasureProperty( starResidual, properties.starResidual );
   MeasureUtils::MeasureProperty( fwhmMeanDev, properties.fwhmMeanDev );
   MeasureUtils::MeasureProperty( eccentricityMeanDev, properties.eccentricityMeanDev );
   MeasureUtils::MeasureProperty( starResidualMeanDev, properties.starResidualMeanDev );
   MeasureUtils::MeasureProperty( azimuth, properties.azimuth );
   MeasureUtils::MeasureProperty( altitude, properties.altitude );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasureData.cpp - Released 2023-05-17T17:06:42Z
