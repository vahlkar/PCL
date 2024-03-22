//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.9
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.5
// ----------------------------------------------------------------------------
// SpectrophotometricColorCalibrationParameters.h - Released 2024-03-20T10:42:12Z
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

#ifndef __SpectrophotometricColorCalibrationParameters_h
#define __SpectrophotometricColorCalibrationParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/PSFEstimator.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SPCCApplyCalibration : public MetaBoolean
{
public:

   SPCCApplyCalibration( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPCCApplyCalibration* TheSPCCApplyCalibrationParameter;

// ----------------------------------------------------------------------------

class SPCCNarrowbandMode : public MetaBoolean
{
public:

   SPCCNarrowbandMode( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPCCNarrowbandMode* TheSPCCNarrowbandModeParameter;

// ----------------------------------------------------------------------------

class SPCCNarrowbandOptimizeStars : public MetaBoolean
{
public:

   SPCCNarrowbandOptimizeStars( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPCCNarrowbandOptimizeStars* TheSPCCNarrowbandOptimizeStarsParameter;

// ----------------------------------------------------------------------------

class SPCCWhiteReferenceSpectrum : public MetaString
{
public:

   SPCCWhiteReferenceSpectrum( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCWhiteReferenceSpectrum* TheSPCCWhiteReferenceSpectrumParameter;

// ----------------------------------------------------------------------------

class SPCCWhiteReferenceName : public MetaString
{
public:

   SPCCWhiteReferenceName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCWhiteReferenceName* TheSPCCWhiteReferenceNameParameter;

// ----------------------------------------------------------------------------

class SPCCRedFilterTrCurve : public MetaString
{
public:

   SPCCRedFilterTrCurve( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCRedFilterTrCurve* TheSPCCRedFilterTrCurveParameter;

// ----------------------------------------------------------------------------

class SPCCRedFilterName : public MetaString
{
public:

   SPCCRedFilterName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCRedFilterName* TheSPCCRedFilterNameParameter;

// ----------------------------------------------------------------------------

class SPCCGreenFilterTrCurve : public MetaString
{
public:

   SPCCGreenFilterTrCurve( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCGreenFilterTrCurve* TheSPCCGreenFilterTrCurveParameter;

// ----------------------------------------------------------------------------

class SPCCGreenFilterName : public MetaString
{
public:

   SPCCGreenFilterName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCGreenFilterName* TheSPCCGreenFilterNameParameter;

// ----------------------------------------------------------------------------

class SPCCBlueFilterTrCurve : public MetaString
{
public:

   SPCCBlueFilterTrCurve( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCBlueFilterTrCurve* TheSPCCBlueFilterTrCurveParameter;

// ----------------------------------------------------------------------------

class SPCCBlueFilterName : public MetaString
{
public:

   SPCCBlueFilterName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCBlueFilterName* TheSPCCBlueFilterNameParameter;

// ----------------------------------------------------------------------------

class SPCCRedFilterWavelength : public MetaFloat
{
public:

   SPCCRedFilterWavelength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPCCRedFilterWavelength* TheSPCCRedFilterWavelengthParameter;

// ----------------------------------------------------------------------------

class SPCCRedFilterBandwidth : public MetaFloat
{
public:

   SPCCRedFilterBandwidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPCCRedFilterBandwidth* TheSPCCRedFilterBandwidthParameter;

// ----------------------------------------------------------------------------

class SPCCGreenFilterWavelength : public MetaFloat
{
public:

   SPCCGreenFilterWavelength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPCCGreenFilterWavelength* TheSPCCGreenFilterWavelengthParameter;

// ----------------------------------------------------------------------------

class SPCCGreenFilterBandwidth : public MetaFloat
{
public:

   SPCCGreenFilterBandwidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPCCGreenFilterBandwidth* TheSPCCGreenFilterBandwidthParameter;

// ----------------------------------------------------------------------------

class SPCCBlueFilterWavelength : public MetaFloat
{
public:

   SPCCBlueFilterWavelength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPCCBlueFilterWavelength* TheSPCCBlueFilterWavelengthParameter;

// ----------------------------------------------------------------------------

class SPCCBlueFilterBandwidth : public MetaFloat
{
public:

   SPCCBlueFilterBandwidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern SPCCBlueFilterBandwidth* TheSPCCBlueFilterBandwidthParameter;

// ----------------------------------------------------------------------------

class SPCCDeviceQECurve : public MetaString
{
public:

   SPCCDeviceQECurve( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCDeviceQECurve* TheSPCCDeviceQECurveParameter;

// ----------------------------------------------------------------------------

class SPCCDeviceQECurveName : public MetaString
{
public:

   SPCCDeviceQECurveName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCDeviceQECurveName* TheSPCCDeviceQECurveNameParameter;

// ----------------------------------------------------------------------------

class SPCCBroadbandIntegrationStepSize : public MetaFloat
{
public:

   SPCCBroadbandIntegrationStepSize( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCBroadbandIntegrationStepSize* TheSPCCBroadbandIntegrationStepSizeParameter;

// ----------------------------------------------------------------------------

class SPCCNarrowbandIntegrationSteps : public MetaInt32
{
public:

   SPCCNarrowbandIntegrationSteps( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCNarrowbandIntegrationSteps* TheSPCCNarrowbandIntegrationStepsParameter;

// ----------------------------------------------------------------------------

class SPCCCatalogId : public MetaString
{
public:

   SPCCCatalogId( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCCatalogId* TheSPCCCatalogIdParameter;

// ----------------------------------------------------------------------------

class SPCCLimitMagnitude : public MetaFloat
{
public:

   SPCCLimitMagnitude( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCLimitMagnitude* TheSPCCLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class SPCCAutoLimitMagnitude : public MetaBoolean
{
public:

   SPCCAutoLimitMagnitude( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPCCAutoLimitMagnitude* TheSPCCAutoLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class SPCCTargetSourceCount : public MetaInt32
{
public:

   SPCCTargetSourceCount( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCTargetSourceCount* TheSPCCTargetSourceCountParameter;

// ----------------------------------------------------------------------------

class SPCCStructureLayers : public MetaInt32
{
public:

   SPCCStructureLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCStructureLayers* TheSPCCStructureLayersParameter;

// ----------------------------------------------------------------------------

class SPCCSaturationThreshold : public MetaFloat
{
public:

   SPCCSaturationThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCSaturationThreshold* TheSPCCSaturationThresholdParameter;

// ----------------------------------------------------------------------------

class SPCCSaturationRelative : public MetaBoolean
{
public:

   SPCCSaturationRelative( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPCCSaturationRelative* TheSPCCSaturationRelativeParameter;

// ----------------------------------------------------------------------------

class SPCCSaturationShrinkFactor : public MetaFloat
{
public:

   SPCCSaturationShrinkFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCSaturationShrinkFactor* TheSPCCSaturationShrinkFactorParameter;

// ----------------------------------------------------------------------------

class SPCCPSFNoiseLayers : public MetaInt32
{
public:

   SPCCPSFNoiseLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCPSFNoiseLayers* TheSPCCPSFNoiseLayersParameter;

// ----------------------------------------------------------------------------

class SPCCPSFHotPixelFilterRadius : public MetaInt32
{
public:

   SPCCPSFHotPixelFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCPSFHotPixelFilterRadius* TheSPCCPSFHotPixelFilterRadiusParameter;

// ----------------------------------------------------------------------------

class SPCCPSFNoiseReductionFilterRadius : public MetaInt32
{
public:

   SPCCPSFNoiseReductionFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCPSFNoiseReductionFilterRadius* TheSPCCPSFNoiseReductionFilterRadiusParameter;

// ----------------------------------------------------------------------------

class SPCCPSFMinStructureSize : public MetaInt32
{
public:

   SPCCPSFMinStructureSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCPSFMinStructureSize* TheSPCCPSFMinStructureSizeParameter;

// ----------------------------------------------------------------------------

class SPCCPSFMinSNR : public MetaFloat
{
public:

   SPCCPSFMinSNR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCPSFMinSNR* TheSPCCPSFMinSNRParameter;

// ----------------------------------------------------------------------------

class SPCCPSFAllowClusteredSources : public MetaBoolean
{
public:

   SPCCPSFAllowClusteredSources( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPCCPSFAllowClusteredSources* TheSPCCPSFAllowClusteredSourcesParameter;

// ----------------------------------------------------------------------------

class SPCCPSFType : public MetaEnumeration
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

   SPCCPSFType( MetaProcess* );

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

extern SPCCPSFType* TheSPCCPSFTypeParameter;

// ----------------------------------------------------------------------------

class SPCCPSFGrowth : public MetaFloat
{
public:

   SPCCPSFGrowth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCPSFGrowth* TheSPCCPSFGrowthParameter;

// ----------------------------------------------------------------------------

class SPCCPSFMaxStars : public MetaInt32
{
public:

   SPCCPSFMaxStars( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCPSFMaxStars* TheSPCCPSFMaxStarsParameter;

// ----------------------------------------------------------------------------

class SPCCPSFSearchTolerance : public MetaFloat
{
public:

   SPCCPSFSearchTolerance( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCPSFSearchTolerance* TheSPCCPSFSearchToleranceParameter;

// ----------------------------------------------------------------------------

class SPCCPSFChannelSearchTolerance : public MetaFloat
{
public:

   SPCCPSFChannelSearchTolerance( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCPSFChannelSearchTolerance* TheSPCCPSFChannelSearchToleranceParameter;

// ----------------------------------------------------------------------------

class SPCCNeutralizeBackground : public MetaBoolean
{
public:

   SPCCNeutralizeBackground( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPCCNeutralizeBackground* TheSPCCNeutralizeBackgroundParameter;

// ----------------------------------------------------------------------------

class SPCCBackgroundReferenceViewId : public MetaString
{
public:

   SPCCBackgroundReferenceViewId( MetaProcess* );

   IsoString Id() const override;
};

extern SPCCBackgroundReferenceViewId* TheSPCCBackgroundReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class SPCCBackgroundLow : public MetaFloat
{
public:

   SPCCBackgroundLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern SPCCBackgroundLow* TheSPCCBackgroundLowParameter;

// ----------------------------------------------------------------------------

class SPCCBackgroundHigh : public MetaFloat
{
public:

   SPCCBackgroundHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern SPCCBackgroundHigh* TheSPCCBackgroundHighParameter;

// ----------------------------------------------------------------------------

class SPCCBackgroundUseROI : public MetaBoolean
{
public:

   SPCCBackgroundUseROI( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPCCBackgroundUseROI* TheSPCCBackgroundUseROIParameter;

// ----------------------------------------------------------------------------

class SPCCBackgroundROIX0 : public MetaInt32
{
public:

   SPCCBackgroundROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCBackgroundROIX0* TheSPCCBackgroundROIX0Parameter;

// ----------------------------------------------------------------------------

class SPCCBackgroundROIY0 : public MetaInt32
{
public:

   SPCCBackgroundROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCBackgroundROIY0* TheSPCCBackgroundROIY0Parameter;

// ----------------------------------------------------------------------------

class SPCCBackgroundROIX1 : public MetaInt32
{
public:

   SPCCBackgroundROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCBackgroundROIX1* TheSPCCBackgroundROIX1Parameter;

// ----------------------------------------------------------------------------

class SPCCBackgroundROIY1 : public MetaInt32
{
public:

   SPCCBackgroundROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SPCCBackgroundROIY1* TheSPCCBackgroundROIY1Parameter;

// ----------------------------------------------------------------------------

class SPCCGenerateGraphs : public MetaBoolean
{
public:

   SPCCGenerateGraphs( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPCCGenerateGraphs* TheSPCCGenerateGraphsParameter;

// ----------------------------------------------------------------------------

class SPCCGenerateStarMaps : public MetaBoolean
{
public:

   SPCCGenerateStarMaps( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPCCGenerateStarMaps* TheSPCCGenerateStarMapsParameter;

// ----------------------------------------------------------------------------

class SPCCGenerateTextFiles : public MetaBoolean
{
public:

   SPCCGenerateTextFiles( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SPCCGenerateTextFiles* TheSPCCGenerateTextFilesParameter;

// ----------------------------------------------------------------------------

class SPCCOutputDirectory : public MetaString
{
public:

   SPCCOutputDirectory( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SPCCOutputDirectory* TheSPCCOutputDirectoryParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __SpectrophotometricColorCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF SpectrophotometricColorCalibrationParameters.h - Released 2024-03-20T10:42:12Z
