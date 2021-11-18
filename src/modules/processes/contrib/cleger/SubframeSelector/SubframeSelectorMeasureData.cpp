//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.15
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.6.2
// ----------------------------------------------------------------------------
// SubframeSelectorMeasureData.cpp - Released 2021-11-18T17:01:48Z
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
   fwhmMeanDev = 0;
   eccentricity = TheSSMeasurementEccentricityParameter->DefaultValue();
   eccentricityMeanDev = 0;
   psfSignalWeight = TheSSMeasurementPSFSignalWeightParameter->DefaultValue();
   psfPowerWeight = TheSSMeasurementPSFPowerWeightParameter->DefaultValue();
   snrWeight = TheSSMeasurementSNRWeightParameter->DefaultValue();
   median = TheSSMeasurementMedianParameter->DefaultValue();
   medianMeanDev = 0;
   noise = TheSSMeasurementNoiseParameter->DefaultValue();
   noiseRatio = TheSSMeasurementNoiseRatioParameter->DefaultValue();
   stars = TheSSMeasurementStarsParameter->DefaultValue();
   starResidual = TheSSMeasurementStarResidualParameter->DefaultValue();
   starResidualMeanDev = 0;
   azimuth = TheSSMeasurementAzimuthParameter->DefaultValue();
   altitude = TheSSMeasurementAltitudeParameter->DefaultValue();
}

// ----------------------------------------------------------------------------

void MeasureData::AddToCache( const SubframeSelectorInstance& instance ) const
{
   if ( TheSubframeSelectorCache != nullptr )
   {
      SubframeSelectorCacheItem item( path );
      item.fwhm                = fwhm;
      item.fwhmMeanDev         = fwhmMeanDev;
      item.eccentricity        = eccentricity;
      item.eccentricityMeanDev = eccentricityMeanDev;
      item.psfSignalWeight     = psfSignalWeight;
      item.psfPowerWeight      = psfPowerWeight;
      item.snrWeight           = snrWeight;
      item.median              = median;
      item.medianMeanDev       = medianMeanDev;
      item.noise               = noise;
      item.noiseRatio          = noiseRatio;
      item.stars               = stars;
      item.starResidual        = starResidual;
      item.starResidualMeanDev = starResidualMeanDev;
      item.azimuth             = azimuth;
      item.altitude            = altitude;
      item.instanceParameters  = instance.EncodedCacheSensitiveParameters();
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
         fwhm                = item.fwhm;
         fwhmMeanDev         = item.fwhmMeanDev;
         eccentricity        = item.eccentricity;
         eccentricityMeanDev = item.eccentricityMeanDev;
         psfSignalWeight     = item.psfSignalWeight;
         psfPowerWeight      = item.psfPowerWeight;
         snrWeight           = item.snrWeight;
         median              = item.median;
         medianMeanDev       = item.medianMeanDev;
         noise               = item.noise;
         noiseRatio          = item.noiseRatio;
         stars               = item.stars;
         starResidual        = item.starResidual;
         starResidualMeanDev = item.starResidualMeanDev;
         azimuth             = item.azimuth;
         altitude            = item.altitude;

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

   String().Format( "let Weight = %.8e;\n", weight ) +
   String().Format( "let WeightMin = %.8e;\n", properties.weight.min ) +
   String().Format( "let WeightMax = %.8e;\n", properties.weight.max ) +
   String().Format( "let WeightMedian = %.8e;\n", properties.weight.median ) +
   String().Format( "let WeightSigma = %.8e;\n", DeviationNormalize(
      weight, properties.weight.median, properties.weight.deviation ) ) +

   String().Format( "let FWHM = %.8e;\n", FWHM( subframeScale, scaleUnit ) ) +
   String().Format( "let FWHMMin = %.8e;\n", properties.fwhm.min ) +
   String().Format( "let FWHMMax = %.8e;\n", properties.fwhm.max ) +
   String().Format( "let FWHMMedian = %.8e;\n", properties.fwhm.median ) +
   String().Format( "let FWHMSigma = %.8e;\n", DeviationNormalize( FWHM(
                  subframeScale, scaleUnit ), properties.fwhm.median, properties.fwhm.deviation ) ) +

   String().Format( "let Eccentricity = %.8e;\n", eccentricity ) +
   String().Format( "let EccentricityMin = %.8e;\n", properties.eccentricity.min ) +
   String().Format( "let EccentricityMax = %.8e;\n", properties.eccentricity.max ) +
   String().Format( "let EccentricityMedian = %.8e;\n", properties.eccentricity.median ) +
   String().Format( "let EccentricitySigma = %.8e;\n", DeviationNormalize(
                  eccentricity, properties.eccentricity.median, properties.eccentricity.deviation ) ) +

   String().Format( "let PSFSignalWeight = %.8e;\n", psfSignalWeight ) +
   String().Format( "let PSFSignalWeightMin = %.8e;\n", properties.psfSignalWeight.min ) +
   String().Format( "let PSFSignalWeightMax = %.8e;\n", properties.psfSignalWeight.max ) +
   String().Format( "let PSFSignalWeightMedian = %.8e;\n", properties.psfSignalWeight.median ) +
   String().Format( "let PSFSignalWeightSigma = %.8e;\n", DeviationNormalize(
                  psfSignalWeight, properties.psfSignalWeight.median, properties.psfSignalWeight.deviation ) ) +

   String().Format( "let PSFPowerWeight = %.8e;\n", psfPowerWeight ) +
   String().Format( "let PSFPowerWeightMin = %.8e;\n", properties.psfPowerWeight.min ) +
   String().Format( "let PSFPowerWeightMax = %.8e;\n", properties.psfPowerWeight.max ) +
   String().Format( "let PSFPowerWeightMedian = %.8e;\n", properties.psfPowerWeight.median ) +
   String().Format( "let PSFPowerWeightSigma = %.8e;\n", DeviationNormalize(
                  psfPowerWeight, properties.psfPowerWeight.median, properties.psfPowerWeight.deviation ) ) +

   String().Format( "let SNRWeight = %.8e;\n", snrWeight ) +
   String().Format( "let SNRWeightMin = %.8e;\n", properties.snrWeight.min ) +
   String().Format( "let SNRWeightMax = %.8e;\n", properties.snrWeight.max ) +
   String().Format( "let SNRWeightMedian = %.8e;\n", properties.snrWeight.median ) +
   String().Format( "let SNRWeightSigma = %.8e;\n", DeviationNormalize(
                  snrWeight, properties.snrWeight.median, properties.snrWeight.deviation ) ) +

   String().Format( "let Median = %.8e;\n", Median( cameraGain, cameraResolution, dataUnit ) ) +
   String().Format( "let MedianMin = %.8e;\n", properties.median.min ) +
   String().Format( "let MedianMax = %.8e;\n", properties.median.max ) +
   String().Format( "let MedianMedian = %.8e;\n", properties.median.median ) +
   String().Format( "let MedianSigma = %.8e;\n", DeviationNormalize(
                  Median( cameraGain, cameraResolution, dataUnit ),
                  properties.median.median, properties.median.deviation ) ) +

   String().Format( "let MedianMeanDev = %.8e;\n", MedianMeanDev( cameraGain, cameraResolution, dataUnit ) ) +
   String().Format( "let MedianMeanDevMin = %.8e;\n", properties.medianMeanDev.min ) +
   String().Format( "let MedianMeanDevMax = %.8e;\n", properties.medianMeanDev.max ) +
   String().Format( "let MedianMeanDevMedian = %.8e;\n", properties.medianMeanDev.median ) +
   String().Format( "let MedianMeanDevSigma = %.8e;\n", DeviationNormalize(
                  MedianMeanDev( cameraGain, cameraResolution, dataUnit ),
                  properties.medianMeanDev.median, properties.medianMeanDev.deviation ) ) +

   String().Format( "let Noise = %.8e;\n", Noise( cameraGain, cameraResolution, dataUnit ) ) +
   String().Format( "let NoiseMin = %.8e;\n", properties.noise.min ) +
   String().Format( "let NoiseMax = %.8e;\n", properties.noise.max ) +
   String().Format( "let NoiseMedian = %.8e;\n", properties.noise.median ) +
   String().Format( "let NoiseSigma = %.8e;\n", DeviationNormalize(
                  Noise( cameraGain, cameraResolution, dataUnit ),
                  properties.noise.median, properties.noise.deviation ) ) +

   String().Format( "let NoiseRatio = %.8e;\n", noiseRatio ) +
   String().Format( "let NoiseRatioMin = %.8e;\n", properties.noiseRatio.min ) +
   String().Format( "let NoiseRatioMax = %.8e;\n", properties.noiseRatio.max ) +
   String().Format( "let NoiseRatioMedian = %.8e;\n", properties.noiseRatio.median ) +
   String().Format( "let NoiseRatioSigma = %.8e;\n", DeviationNormalize(
                  noiseRatio, properties.noiseRatio.median, properties.noiseRatio.deviation ) ) +

   String().Format( "let Stars = %i;\n", stars ) +
   String().Format( "let StarsMin = %.8e;\n", properties.stars.min ) +
   String().Format( "let StarsMax = %.8e;\n", properties.stars.max ) +
   String().Format( "let StarsMedian = %.8e;\n", properties.stars.median ) +
   String().Format( "let StarsSigma = %.8e;\n", DeviationNormalize(
                  stars, properties.stars.median, properties.stars.deviation ) ) +

   String().Format( "let StarResidual = %.8e;\n", starResidual ) +
   String().Format( "let StarResidualMin = %.8e;\n", properties.starResidual.min ) +
   String().Format( "let StarResidualMax = %.8e;\n", properties.starResidual.max ) +
   String().Format( "let StarResidualMedian = %.8e;\n", properties.starResidual.median ) +
   String().Format( "let StarResidualSigma = %.8e;\n", DeviationNormalize(
                  starResidual, properties.starResidual.median, properties.starResidual.deviation ) ) +

   String().Format( "let FWHMMeanDev = %.8e;\n", FWHMMeanDeviation( subframeScale, scaleUnit ) ) +
   String().Format( "let FWHMMeanDevMin = %.8e;\n", properties.fwhmMeanDev.min ) +
   String().Format( "let FWHMMeanDevMax = %.8e;\n", properties.fwhmMeanDev.max ) +
   String().Format( "let FWHMMeanDevMedian = %.8e;\n", properties.fwhmMeanDev.median ) +
   String().Format( "let FWHMMeanDevSigma = %.8e;\n", DeviationNormalize(
                  FWHMMeanDeviation( subframeScale, scaleUnit ),
                  properties.fwhmMeanDev.median, properties.fwhmMeanDev.deviation ) ) +

   String().Format( "let EccentricityMeanDev = %.8e;\n", eccentricityMeanDev ) +
   String().Format( "let EccentricityMeanDevMin = %.8e;\n", properties.eccentricityMeanDev.min ) +
   String().Format( "let EccentricityMeanDevMax = %.8e;\n", properties.eccentricityMeanDev.max ) +
   String().Format( "let EccentricityMeanDevMedian = %.8e;\n", properties.eccentricityMeanDev.median ) +
   String().Format( "let EccentricityMeanDevSigma = %.8e;\n", DeviationNormalize(
                  eccentricityMeanDev, properties.eccentricityMeanDev.median, properties.eccentricityMeanDev.deviation ) ) +

   String().Format( "let StarResidualMeanDev = %.8e;\n", starResidualMeanDev ) +
   String().Format( "let StarResidualMeanDevMin = %.8e;\n", properties.starResidualMeanDev.min ) +
   String().Format( "let StarResidualMeanDevMax = %.8e;\n", properties.starResidualMeanDev.max ) +
   String().Format( "let StarResidualMeanDevMedian = %.8e;\n", properties.starResidualMeanDev.median ) +
   String().Format( "let StarResidualMeanDevSigma = %.8e;\n", DeviationNormalize(
                  starResidualMeanDev, properties.starResidualMeanDev.median, properties.starResidualMeanDev.deviation ) ) +

   String().Format( "let Azimuth = %.8e;\n", azimuth ) +
   String().Format( "let AzimuthMin = %.8e;\n", properties.azimuth.min ) +
   String().Format( "let AzimuthMax = %.8e;\n", properties.azimuth.max ) +
   String().Format( "let AzimuthMedian = %.8e;\n", properties.azimuth.median ) +
   String().Format( "let AzimuthSigma = %.8e;\n", DeviationNormalize(
                  azimuth, properties.azimuth.median, properties.azimuth.deviation ) ) +

   String().Format( "let Altitude = %.8e;\n", altitude ) +
   String().Format( "let AltitudeMin = %.8e;\n", properties.altitude.min ) +
   String().Format( "let AltitudeMax = %.8e;\n", properties.altitude.max ) +
   String().Format( "let AltitudeMedian = %.8e;\n", properties.altitude.median ) +
   String().Format( "let AltitudeSigma = %.8e;\n", DeviationNormalize(
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
      if (    !CharTraits::IsAlpha( c )
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
   size_type measuresLength( measures.Length() );

   Array<double> weight( measuresLength );
   Array<double> fwhm( measuresLength );
   Array<double> eccentricity( measuresLength );
   Array<double> psfSignalWeight( measuresLength );
   Array<double> psfPowerWeight( measuresLength );
   Array<double> snrWeight( measuresLength );
   Array<double> median( measuresLength );
   Array<double> medianMeanDev( measuresLength );
   Array<double> noise( measuresLength );
   Array<double> noiseRatio( measuresLength );
   Array<double> stars( measuresLength );
   Array<double> starResidual( measuresLength );
   Array<double> fwhmMeanDev( measuresLength );
   Array<double> eccentricityMeanDev( measuresLength );
   Array<double> starResidualMeanDev( measuresLength );
   Array<double> azimuth( measuresLength );
   Array<double> altitude( measuresLength );

   for ( size_type i = 0; i < measuresLength; ++i )
   {
      weight[i] = measures[i].weight;
      fwhm[i] = measures[i].FWHM( subframeScale, scaleUnit );
      eccentricity[i] = measures[i].eccentricity;
      psfSignalWeight[i] = measures[i].psfSignalWeight;
      psfPowerWeight[i] = measures[i].psfPowerWeight;
      snrWeight[i] = measures[i].snrWeight;
      median[i] = measures[i].Median( cameraGain,
                                      TheSSCameraResolutionParameter->ElementData( cameraResolution ),
                                      dataUnit );
      medianMeanDev[i] = measures[i].MedianMeanDev( cameraGain,
                                                    TheSSCameraResolutionParameter->ElementData( cameraResolution ),
                                                    dataUnit );
      noise[i] = measures[i].Noise( cameraGain,
                                    TheSSCameraResolutionParameter->ElementData( cameraResolution ),
                                    dataUnit );
      noiseRatio[i] = measures[i].noiseRatio;
      stars[i] = measures[i].stars;
      starResidual[i] = measures[i].starResidual;
      fwhmMeanDev[i] = measures[i].FWHMMeanDeviation( subframeScale, scaleUnit );
      eccentricityMeanDev[i] = measures[i].eccentricityMeanDev;
      starResidualMeanDev[i] = measures[i].starResidualMeanDev;
      azimuth[i] = measures[i].azimuth;
      altitude[i] = measures[i].altitude;
   }

   MeasureUtils::MeasureProperty( weight, properties.weight );
   MeasureUtils::MeasureProperty( fwhm, properties.fwhm );
   MeasureUtils::MeasureProperty( eccentricity, properties.eccentricity );
   MeasureUtils::MeasureProperty( psfSignalWeight, properties.psfSignalWeight );
   MeasureUtils::MeasureProperty( psfPowerWeight, properties.psfPowerWeight );
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
// EOF SubframeSelectorMeasureData.cpp - Released 2021-11-18T17:01:48Z
