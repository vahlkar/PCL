//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.11
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 2.1.0
// ----------------------------------------------------------------------------
// SpectrophotometricFluxCalibrationParameters.h - Released 2024-05-07T15:28:00Z
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

#ifndef __SpectrophotometricFluxCalibrationParameters_h
#define __SpectrophotometricFluxCalibrationParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/PSFEstimator.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SPFCNarrowbandMode : public MetaBoolean
{
public:

   SPFCNarrowbandMode( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPFCNarrowbandMode* TheSPFCNarrowbandModeParameter;

// ----------------------------------------------------------------------------

class SPFCGrayFilterTrCurve : public MetaString
{
public:

   SPFCGrayFilterTrCurve( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPFCGrayFilterTrCurve* TheSPFCGrayFilterTrCurveParameter;

// ----------------------------------------------------------------------------

class SPFCGrayFilterName : public MetaString
{
public:

   SPFCGrayFilterName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPFCGrayFilterName* TheSPFCGrayFilterNameParameter;

// ----------------------------------------------------------------------------

class SPFCRedFilterTrCurve : public MetaString
{
public:

   SPFCRedFilterTrCurve( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPFCRedFilterTrCurve* TheSPFCRedFilterTrCurveParameter;

// ----------------------------------------------------------------------------

class SPFCRedFilterName : public MetaString
{
public:

   SPFCRedFilterName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPFCRedFilterName* TheSPFCRedFilterNameParameter;

// ----------------------------------------------------------------------------

class SPFCGreenFilterTrCurve : public MetaString
{
public:

   SPFCGreenFilterTrCurve( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPFCGreenFilterTrCurve* TheSPFCGreenFilterTrCurveParameter;

// ----------------------------------------------------------------------------

class SPFCGreenFilterName : public MetaString
{
public:

   SPFCGreenFilterName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPFCGreenFilterName* TheSPFCGreenFilterNameParameter;

// ----------------------------------------------------------------------------

class SPFCBlueFilterTrCurve : public MetaString
{
public:

   SPFCBlueFilterTrCurve( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPFCBlueFilterTrCurve* TheSPFCBlueFilterTrCurveParameter;

// ----------------------------------------------------------------------------

class SPFCBlueFilterName : public MetaString
{
public:

   SPFCBlueFilterName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPFCBlueFilterName* TheSPFCBlueFilterNameParameter;

// ----------------------------------------------------------------------------

class SPFCGrayFilterWavelength : public MetaFloat
{
public:

   SPFCGrayFilterWavelength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPFCGrayFilterWavelength* TheSPFCGrayFilterWavelengthParameter;

// ----------------------------------------------------------------------------

class SPFCGrayFilterBandwidth : public MetaFloat
{
public:

   SPFCGrayFilterBandwidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPFCGrayFilterBandwidth* TheSPFCGrayFilterBandwidthParameter;

// ----------------------------------------------------------------------------

class SPFCRedFilterWavelength : public MetaFloat
{
public:

   SPFCRedFilterWavelength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPFCRedFilterWavelength* TheSPFCRedFilterWavelengthParameter;

// ----------------------------------------------------------------------------

class SPFCRedFilterBandwidth : public MetaFloat
{
public:

   SPFCRedFilterBandwidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPFCRedFilterBandwidth* TheSPFCRedFilterBandwidthParameter;

// ----------------------------------------------------------------------------

class SPFCGreenFilterWavelength : public MetaFloat
{
public:

   SPFCGreenFilterWavelength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPFCGreenFilterWavelength* TheSPFCGreenFilterWavelengthParameter;

// ----------------------------------------------------------------------------

class SPFCGreenFilterBandwidth : public MetaFloat
{
public:

   SPFCGreenFilterBandwidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPFCGreenFilterBandwidth* TheSPFCGreenFilterBandwidthParameter;

// ----------------------------------------------------------------------------

class SPFCBlueFilterWavelength : public MetaFloat
{
public:

   SPFCBlueFilterWavelength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPFCBlueFilterWavelength* TheSPFCBlueFilterWavelengthParameter;

// ----------------------------------------------------------------------------

class SPFCBlueFilterBandwidth : public MetaFloat
{
public:

   SPFCBlueFilterBandwidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPFCBlueFilterBandwidth* TheSPFCBlueFilterBandwidthParameter;

// ----------------------------------------------------------------------------

class SPFCDeviceQECurve : public MetaString
{
public:

   SPFCDeviceQECurve( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPFCDeviceQECurve* TheSPFCDeviceQECurveParameter;

// ----------------------------------------------------------------------------

class SPFCDeviceQECurveName : public MetaString
{
public:

   SPFCDeviceQECurveName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPFCDeviceQECurveName* TheSPFCDeviceQECurveNameParameter;

// ----------------------------------------------------------------------------

class SPFCBroadbandIntegrationStepSize : public MetaFloat
{
public:

   SPFCBroadbandIntegrationStepSize( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCBroadbandIntegrationStepSize* TheSPFCBroadbandIntegrationStepSizeParameter;

// ----------------------------------------------------------------------------

class SPFCNarrowbandIntegrationSteps : public MetaInt32
{
public:

   SPFCNarrowbandIntegrationSteps( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCNarrowbandIntegrationSteps* TheSPFCNarrowbandIntegrationStepsParameter;

// ----------------------------------------------------------------------------

class SPFCTrimmingFraction : public MetaFloat
{
public:

   SPFCTrimmingFraction( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCTrimmingFraction* TheSPFCTrimmingFractionParameter;

// ----------------------------------------------------------------------------

class SPFCCatalogId : public MetaString
{
public:

   SPFCCatalogId( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPFCCatalogId* TheSPFCCatalogIdParameter;

// ----------------------------------------------------------------------------

class SPFCLimitMagnitude : public MetaFloat
{
public:

   SPFCLimitMagnitude( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCLimitMagnitude* TheSPFCLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class SPFCAutoLimitMagnitude : public MetaBoolean
{
public:

   SPFCAutoLimitMagnitude( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPFCAutoLimitMagnitude* TheSPFCAutoLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class SPFCStructureLayers : public MetaInt32
{
public:

   SPFCStructureLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCStructureLayers* TheSPFCStructureLayersParameter;

// ----------------------------------------------------------------------------

class SPFCSaturationThreshold : public MetaFloat
{
public:

   SPFCSaturationThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCSaturationThreshold* TheSPFCSaturationThresholdParameter;

// ----------------------------------------------------------------------------

class SPFCSaturationRelative : public MetaBoolean
{
public:

   SPFCSaturationRelative( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPFCSaturationRelative* TheSPFCSaturationRelativeParameter;

// ----------------------------------------------------------------------------

class SPFCSaturationShrinkFactor : public MetaFloat
{
public:

   SPFCSaturationShrinkFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCSaturationShrinkFactor* TheSPFCSaturationShrinkFactorParameter;

// ----------------------------------------------------------------------------

class SPFCPSFNoiseLayers : public MetaInt32
{
public:

   SPFCPSFNoiseLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCPSFNoiseLayers* TheSPFCPSFNoiseLayersParameter;

// ----------------------------------------------------------------------------

class SPFCPSFHotPixelFilterRadius : public MetaInt32
{
public:

   SPFCPSFHotPixelFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCPSFHotPixelFilterRadius* TheSPFCPSFHotPixelFilterRadiusParameter;

// ----------------------------------------------------------------------------

class SPFCPSFNoiseReductionFilterRadius : public MetaInt32
{
public:

   SPFCPSFNoiseReductionFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCPSFNoiseReductionFilterRadius* TheSPFCPSFNoiseReductionFilterRadiusParameter;

// ----------------------------------------------------------------------------

class SPFCPSFMinStructureSize : public MetaInt32
{
public:

   SPFCPSFMinStructureSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCPSFMinStructureSize* TheSPFCPSFMinStructureSizeParameter;

// ----------------------------------------------------------------------------

class SPFCPSFMinSNR : public MetaFloat
{
public:

   SPFCPSFMinSNR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCPSFMinSNR* TheSPFCPSFMinSNRParameter;

// ----------------------------------------------------------------------------

class SPFCPSFAllowClusteredSources : public MetaBoolean
{
public:

   SPFCPSFAllowClusteredSources( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPFCPSFAllowClusteredSources* TheSPFCPSFAllowClusteredSourcesParameter;

// ----------------------------------------------------------------------------

class SPFCPSFType : public MetaEnumeration
{
public:

   enum { Gaussian,
          Moffat15,
          Moffat4,
          Moffat6,
          Moffat8,
          MoffatA,
          Auto,
          NumberOfItems,
          Default = Auto };

   SPFCPSFType( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static PSFEstimator::psf_function ToPSFFunction( pcl_enum x )
   {
      switch ( x )
      {
      case Gaussian: return PSFunction::Gaussian;
      case Moffat15: return PSFunction::Moffat15;
      case Moffat4:  return PSFunction::Moffat4;
      case Moffat6:  return PSFunction::Moffat6;
      case Moffat8:  return PSFunction::Moffat8;
      case MoffatA:  return PSFunction::MoffatA;
      default:
      case Auto:       return PSFunction::Auto;
      }
   }

   static IsoString FunctionName( pcl_enum );
};

extern SPFCPSFType* TheSPFCPSFTypeParameter;

// ----------------------------------------------------------------------------

class SPFCPSFGrowth : public MetaFloat
{
public:

   SPFCPSFGrowth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCPSFGrowth* TheSPFCPSFGrowthParameter;

// ----------------------------------------------------------------------------

class SPFCPSFMaxStars : public MetaInt32
{
public:

   SPFCPSFMaxStars( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCPSFMaxStars* TheSPFCPSFMaxStarsParameter;

// ----------------------------------------------------------------------------

class SPFCPSFSearchTolerance : public MetaFloat
{
public:

   SPFCPSFSearchTolerance( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCPSFSearchTolerance* TheSPFCPSFSearchToleranceParameter;

// ----------------------------------------------------------------------------

class SPFCPSFChannelSearchTolerance : public MetaFloat
{
public:

   SPFCPSFChannelSearchTolerance( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPFCPSFChannelSearchTolerance* TheSPFCPSFChannelSearchToleranceParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __SpectrophotometricFluxCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF SpectrophotometricFluxCalibrationParameters.h - Released 2024-05-07T15:28:00Z
