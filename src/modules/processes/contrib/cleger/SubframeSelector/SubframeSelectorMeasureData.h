//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.15
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.6.0
// ----------------------------------------------------------------------------
// SubframeSelectorMeasureData.h - Released 2021-11-11T17:56:06Z
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

#ifndef __SubframeSelectorMeasureData_h
#define __SubframeSelectorMeasureData_h

#include "SubframeSelectorCache.h"
#include "SubframeSelectorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

inline double DeviationNormalize( const double& value, const double& median,
                                  const double& deviation )
{
   return (value - median)/((1 + deviation != 1) ? deviation : 1.0);
}

struct MeasureProperty
{
   double min = DBL_MAX;
   double max = DBL_MIN;
   double median = 0;
   double deviation = 0;
};

// ----------------------------------------------------------------------------

struct MeasureProperties
{
   MeasureProperty weight;
   MeasureProperty fwhm;
   MeasureProperty fwhmMeanDev;
   MeasureProperty eccentricity;
   MeasureProperty eccentricityMeanDev;
   MeasureProperty psfSignalWeight;
   MeasureProperty psfPowerWeight;
   MeasureProperty snrWeight;
   MeasureProperty median;
   MeasureProperty medianMeanDev;
   MeasureProperty noise;
   MeasureProperty noiseRatio;
   MeasureProperty stars;
   MeasureProperty starResidual;
   MeasureProperty starResidualMeanDev;
   MeasureProperty azimuth;
   MeasureProperty altitude;
};

// ----------------------------------------------------------------------------

class SubframeSelectorInstance;

struct MeasureData
{
   String path;
   double fwhm;
   double fwhmMeanDev;
   double eccentricity;
   double eccentricityMeanDev;
   double psfSignalWeight;
   double psfPowerWeight;
   double snrWeight;
   double median;
   double medianMeanDev;
   double noise;
   double noiseRatio;
   uint16 stars;
   double starResidual;
   double starResidualMeanDev;
   double azimuth;
   double altitude;

   MeasureData( const String& p = String() )
      : path( p )
   {
      ResetCacheableData();
   }

   MeasureData( const MeasureData& ) = default;

   void ResetCacheableData();
   void AddToCache( const SubframeSelectorInstance& instance ) const;
   bool GetFromCache( const SubframeSelectorInstance& instance );
};

// ----------------------------------------------------------------------------

struct MeasureItem
{
   uint16   index;
   pcl_bool enabled;
   pcl_bool locked;
   String   path;
   double   weight;
   double   fwhm;
   double   eccentricity;
   double   psfSignalWeight;
   double   psfPowerWeight;
   double   snrWeight;
   double   median;
   double   medianMeanDev;
   double   noise;
   double   noiseRatio;
   uint16   stars;
   double   starResidual;
   double   fwhmMeanDev;
   double   eccentricityMeanDev;
   double   starResidualMeanDev;
   double   azimuth;
   double   altitude;

   MeasureItem( uint16 a_index, const String& a_path = String() )
      : index( a_index )
      , enabled( TheSSMeasurementEnabledParameter->DefaultValue() )
      , locked( TheSSMeasurementLockedParameter->DefaultValue() )
      , path( a_path )
      , weight( TheSSMeasurementWeightParameter->DefaultValue() )
      , fwhm( TheSSMeasurementFWHMParameter->DefaultValue() )
      , eccentricity( TheSSMeasurementEccentricityParameter->DefaultValue() )
      , psfSignalWeight( TheSSMeasurementPSFSignalWeightParameter->DefaultValue() )
      , psfPowerWeight( TheSSMeasurementPSFPowerWeightParameter->DefaultValue() )
      , snrWeight( TheSSMeasurementSNRWeightParameter->DefaultValue() )
      , median( TheSSMeasurementMedianParameter->DefaultValue() )
      , medianMeanDev( TheSSMeasurementMedianMeanDevParameter->DefaultValue() )
      , noise( TheSSMeasurementNoiseParameter->DefaultValue() )
      , noiseRatio( TheSSMeasurementNoiseRatioParameter->DefaultValue() )
      , stars( TheSSMeasurementStarsParameter->DefaultValue() )
      , starResidual( TheSSMeasurementStarResidualParameter->DefaultValue() )
      , fwhmMeanDev( TheSSMeasurementFWHMMeanDevParameter->DefaultValue() )
      , eccentricityMeanDev( TheSSMeasurementEccentricityMeanDevParameter->DefaultValue() )
      , starResidualMeanDev( TheSSMeasurementStarResidualMeanDevParameter->DefaultValue() )
      , azimuth( TheSSMeasurementAzimuthParameter->DefaultValue() )
      , altitude( TheSSMeasurementAltitudeParameter->DefaultValue() )
   {
   }

   MeasureItem( const MeasureItem& ) = default;

   void Input( const MeasureData& measureData )
   {
      path = measureData.path;
      fwhm = measureData.fwhm;
      eccentricity = measureData.eccentricity;
      psfSignalWeight = measureData.psfSignalWeight;
      psfPowerWeight = measureData.psfPowerWeight;
      snrWeight = measureData.snrWeight;
      median = measureData.median;
      medianMeanDev = measureData.medianMeanDev;
      noise = measureData.noise;
      noiseRatio = measureData.noiseRatio;
      stars = measureData.stars;
      starResidual = measureData.starResidual;
      fwhmMeanDev = measureData.fwhmMeanDev;
      eccentricityMeanDev = measureData.eccentricityMeanDev;
      starResidualMeanDev = measureData.starResidualMeanDev;
      azimuth = measureData.azimuth;
      altitude = measureData.altitude;
   }

   double FWHM( double subframeScale, int scaleUnit ) const
   {
      if ( scaleUnit == SSScaleUnit::ArcSeconds )
         return fwhm * subframeScale;
      if ( scaleUnit == SSScaleUnit::Pixel )
         return fwhm;
      return fwhm;
   }

   double Median( double cameraGain, int cameraResolution, int dataUnit ) const
   {
      if ( dataUnit == SSDataUnit::Electron )
         return median * cameraResolution * cameraGain;
      if ( dataUnit == SSDataUnit::DataNumber )
         return median * cameraResolution;
      return median;
   }

   double MedianMeanDev( double cameraGain, int cameraResolution, int dataUnit ) const
   {
      if ( dataUnit == SSDataUnit::Electron )
         return medianMeanDev * cameraResolution * cameraGain;
      if ( dataUnit == SSDataUnit::DataNumber )
         return medianMeanDev * cameraResolution;
      return medianMeanDev;
   }

   double Noise( double cameraGain, int cameraResolution, int dataUnit ) const
   {
      if ( dataUnit == SSDataUnit::Electron )
         return noise * cameraResolution * cameraGain;
      if ( dataUnit == SSDataUnit::DataNumber )
         return noise * cameraResolution;
      return noise;
   }

   double FWHMMeanDeviation( double subframeScale, int scaleUnit ) const
   {
      if ( scaleUnit == SSScaleUnit::ArcSeconds )
         return fwhmMeanDev * subframeScale;
      if ( scaleUnit == SSScaleUnit::Pixel )
         return fwhmMeanDev;
      return fwhmMeanDev;
   }

   String JavaScriptParameters( double subframeScale, int scaleUnit, double cameraGain,
                                int cameraResolution, int dataUnit, const MeasureProperties& properties ) const;

   double SortingValue( pcl_enum sortBy ) const
   {
      switch ( sortBy )
      {
      case SSSortingProperty::Index:               return index;
      case SSSortingProperty::Weight:              return weight;
      case SSSortingProperty::FWHM:                return fwhm;
      case SSSortingProperty::Eccentricity:        return eccentricity;
      case SSSortingProperty::PSFSignalWeight:     return psfSignalWeight;
      case SSSortingProperty::PSFPowerWeight:      return psfPowerWeight;
      case SSSortingProperty::SNRWeight:           return snrWeight;
      case SSSortingProperty::Median:              return median;
      case SSSortingProperty::MedianMeanDev:       return medianMeanDev;
      case SSSortingProperty::Noise:               return noise;
      case SSSortingProperty::NoiseRatio:          return noiseRatio;
      case SSSortingProperty::Stars:               return stars;
      case SSSortingProperty::StarResidual:        return starResidual;
      case SSSortingProperty::FWHMMeanDev:         return fwhmMeanDev;
      case SSSortingProperty::EccentricityMeanDev: return eccentricityMeanDev;
      case SSSortingProperty::StarResidualMeanDev: return starResidualMeanDev;
      case SSSortingProperty::Azimuth:             return azimuth;
      case SSSortingProperty::Altitude:            return altitude;
      default:                                     return 0; // ?
      }
   }
};

// ----------------------------------------------------------------------------

struct MeasureUtils
{
   // Rudimentary first-line check for a JS Expression
   static bool IsValidExpression( const String& );

   static void MeasureProperties( const Array<MeasureItem>& measures, double subframeScale,
                                  int scaleUnit, double cameraGain,
                                  int cameraResolution, int dataUnit,
                                  pcl::MeasureProperties& properties );

   static void MeasureProperty( Array<double>& values, MeasureProperty& property )
   {
      for ( Array<double>::const_iterator i = values.Begin(); i != values.End(); ++i )
      {
         property.min = pcl::Min( *i, property.min );
         property.max = pcl::Max( *i, property.max );
      }
      property.median = pcl::Median( values.Begin(), values.End() );
      property.deviation = pcl::AvgDev( values.Begin(), values.End(), property.median );
   }

   static void MedianAndMeanDeviation( Array<double>& values,
                                       double& median, double& deviation )
   {
      median = pcl::Median( values.Begin(), values.End() );
      deviation = pcl::AvgDev( values.Begin(), values.End(), median );
   }
};

// ----------------------------------------------------------------------------

class SubframeSortingBinaryPredicate
{
public:

   SubframeSortingBinaryPredicate( pcl_enum sortBy, int ascending )
      : m_sortBy( sortBy )
      , m_ascending( ascending )
   {
   }

   bool operator()( const MeasureItem& s1, const MeasureItem& s2 ) const
   {
      if ( m_ascending > 0 )
         return s1.SortingValue( m_sortBy ) > s2.SortingValue( m_sortBy );
      return s1.SortingValue( m_sortBy ) < s2.SortingValue( m_sortBy );
   }

private:

   pcl_enum m_sortBy;
   int      m_ascending;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SubframeSelectorMeasureData_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasureData.h - Released 2021-11-11T17:56:06Z
