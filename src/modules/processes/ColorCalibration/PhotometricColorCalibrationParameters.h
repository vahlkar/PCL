//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.5
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationParameters.h - Released 2024-12-11T17:43:17Z
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

#ifndef __PhotometricColorCalibrationParameters_h
#define __PhotometricColorCalibrationParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/PSFEstimator.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class PCCApplyCalibration : public MetaBoolean
{
public:

   PCCApplyCalibration( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCApplyCalibration* ThePCCApplyCalibrationParameter;

// ----------------------------------------------------------------------------

class PCCWhiteReferenceId : public MetaString
{
public:

   PCCWhiteReferenceId( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern PCCWhiteReferenceId* ThePCCWhiteReferenceIdParameter;

// ----------------------------------------------------------------------------

class PCCWhiteReferenceName : public MetaString
{
public:

   PCCWhiteReferenceName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern PCCWhiteReferenceName* ThePCCWhiteReferenceNameParameter;

// ----------------------------------------------------------------------------

class PCCWhiteReferenceSr_JV : public MetaFloat
{
public:

   PCCWhiteReferenceSr_JV( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PCCWhiteReferenceSr_JV* ThePCCWhiteReferenceSr_JVParameter;

// ----------------------------------------------------------------------------

class PCCWhiteReferenceJB_JV : public MetaFloat
{
public:

   PCCWhiteReferenceJB_JV( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PCCWhiteReferenceJB_JV* ThePCCWhiteReferenceJB_JVParameter;

// ----------------------------------------------------------------------------

class PCCZeroPointSr_JV : public MetaFloat
{
public:

   PCCZeroPointSr_JV( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PCCZeroPointSr_JV* ThePCCZeroPointSr_JVParameter;

// ----------------------------------------------------------------------------

class PCCZeroPointJB_JV : public MetaFloat
{
public:

   PCCZeroPointJB_JV( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PCCZeroPointJB_JV* ThePCCZeroPointJB_JVParameter;

// ----------------------------------------------------------------------------

class PCCAutoCatalog : public MetaBoolean
{
public:

   PCCAutoCatalog( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCAutoCatalog* ThePCCAutoCatalogParameter;

// ----------------------------------------------------------------------------

class PCCCatalogId : public MetaString
{
public:

   PCCCatalogId( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern PCCCatalogId* ThePCCCatalogIdParameter;

// ----------------------------------------------------------------------------

class PCCLimitMagnitude : public MetaFloat
{
public:

   PCCLimitMagnitude( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCLimitMagnitude* ThePCCLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class PCCAutoLimitMagnitude : public MetaBoolean
{
public:

   PCCAutoLimitMagnitude( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCAutoLimitMagnitude* ThePCCAutoLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class PCCTargetSourceCount : public MetaInt32
{
public:

   PCCTargetSourceCount( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCTargetSourceCount* ThePCCTargetSourceCountParameter;

// ----------------------------------------------------------------------------

class PCCServerURL : public MetaString
{
public:

   PCCServerURL( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern PCCServerURL* ThePCCServerURLParameter;

// ----------------------------------------------------------------------------

class PCCStructureLayers : public MetaInt32
{
public:

   PCCStructureLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCStructureLayers* ThePCCStructureLayersParameter;

// ----------------------------------------------------------------------------

class PCCSaturationThreshold : public MetaFloat
{
public:

   PCCSaturationThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCSaturationThreshold* ThePCCSaturationThresholdParameter;

// ----------------------------------------------------------------------------

class PCCSaturationShrinkFactor : public MetaFloat
{
public:

   PCCSaturationShrinkFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCSaturationShrinkFactor* ThePCCSaturationShrinkFactorParameter;

// ----------------------------------------------------------------------------

class PCCSaturationRelative : public MetaBoolean
{
public:

   PCCSaturationRelative( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCSaturationRelative* ThePCCSaturationRelativeParameter;

// ----------------------------------------------------------------------------

class PCCPSFNoiseLayers : public MetaInt32
{
public:

   PCCPSFNoiseLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCPSFNoiseLayers* ThePCCPSFNoiseLayersParameter;

// ----------------------------------------------------------------------------

class PCCPSFHotPixelFilterRadius : public MetaInt32
{
public:

   PCCPSFHotPixelFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCPSFHotPixelFilterRadius* ThePCCPSFHotPixelFilterRadiusParameter;

// ----------------------------------------------------------------------------

class PCCPSFNoiseReductionFilterRadius : public MetaInt32
{
public:

   PCCPSFNoiseReductionFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCPSFNoiseReductionFilterRadius* ThePCCPSFNoiseReductionFilterRadiusParameter;

// ----------------------------------------------------------------------------

class PCCPSFMinStructureSize : public MetaInt32
{
public:

   PCCPSFMinStructureSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCPSFMinStructureSize* ThePCCPSFMinStructureSizeParameter;

// ----------------------------------------------------------------------------

class PCCPSFMinSNR : public MetaFloat
{
public:

   PCCPSFMinSNR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCPSFMinSNR* ThePCCPSFMinSNRParameter;

// ----------------------------------------------------------------------------

class PCCPSFAllowClusteredSources : public MetaBoolean
{
public:

   PCCPSFAllowClusteredSources( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCPSFAllowClusteredSources* ThePCCPSFAllowClusteredSourcesParameter;

// ----------------------------------------------------------------------------

class PCCPSFType : public MetaEnumeration
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

   PCCPSFType( MetaProcess* );

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

extern PCCPSFType* ThePCCPSFTypeParameter;

// ----------------------------------------------------------------------------

class PCCPSFGrowth : public MetaFloat
{
public:

   PCCPSFGrowth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCPSFGrowth* ThePCCPSFGrowthParameter;

// ----------------------------------------------------------------------------

class PCCPSFMaxStars : public MetaInt32
{
public:

   PCCPSFMaxStars( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCPSFMaxStars* ThePCCPSFMaxStarsParameter;

// ----------------------------------------------------------------------------

class PCCPSFSearchTolerance : public MetaFloat
{
public:

   PCCPSFSearchTolerance( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCPSFSearchTolerance* ThePCCPSFSearchToleranceParameter;

// ----------------------------------------------------------------------------

class PCCPSFChannelSearchTolerance : public MetaFloat
{
public:

   PCCPSFChannelSearchTolerance( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCPSFChannelSearchTolerance* ThePCCPSFChannelSearchToleranceParameter;

// ----------------------------------------------------------------------------

class PCCNeutralizeBackground : public MetaBoolean
{
public:

   PCCNeutralizeBackground( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCNeutralizeBackground* ThePCCNeutralizeBackgroundParameter;

// ----------------------------------------------------------------------------

class PCCBackgroundReferenceViewId : public MetaString
{
public:

   PCCBackgroundReferenceViewId( MetaProcess* );

   IsoString Id() const override;
};

extern PCCBackgroundReferenceViewId* ThePCCBackgroundReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class PCCBackgroundLow : public MetaFloat
{
public:

   PCCBackgroundLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCBackgroundLow* ThePCCBackgroundLowParameter;

// ----------------------------------------------------------------------------

class PCCBackgroundHigh : public MetaFloat
{
public:

   PCCBackgroundHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCBackgroundHigh* ThePCCBackgroundHighParameter;

// ----------------------------------------------------------------------------

class PCCBackgroundUseROI : public MetaBoolean
{
public:

   PCCBackgroundUseROI( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCBackgroundUseROI* ThePCCBackgroundUseROIParameter;

// ----------------------------------------------------------------------------

class PCCBackgroundROIX0 : public MetaInt32
{
public:

   PCCBackgroundROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCBackgroundROIX0* ThePCCBackgroundROIX0Parameter;

// ----------------------------------------------------------------------------

class PCCBackgroundROIY0 : public MetaInt32
{
public:

   PCCBackgroundROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCBackgroundROIY0* ThePCCBackgroundROIY0Parameter;

// ----------------------------------------------------------------------------

class PCCBackgroundROIX1 : public MetaInt32
{
public:

   PCCBackgroundROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCBackgroundROIX1* ThePCCBackgroundROIX1Parameter;

// ----------------------------------------------------------------------------

class PCCBackgroundROIY1 : public MetaInt32
{
public:

   PCCBackgroundROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCBackgroundROIY1* ThePCCBackgroundROIY1Parameter;

// ----------------------------------------------------------------------------

class PCCGenerateGraphs : public MetaBoolean
{
public:

   PCCGenerateGraphs( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCGenerateGraphs* ThePCCGenerateGraphsParameter;

// ----------------------------------------------------------------------------

class PCCGenerateStarMaps : public MetaBoolean
{
public:

   PCCGenerateStarMaps( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCGenerateStarMaps* ThePCCGenerateStarMapsParameter;

// ----------------------------------------------------------------------------

class PCCGenerateTextFiles : public MetaBoolean
{
public:

   PCCGenerateTextFiles( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCGenerateTextFiles* ThePCCGenerateTextFilesParameter;

// ----------------------------------------------------------------------------

class PCCOutputDirectory : public MetaString
{
public:

   PCCOutputDirectory( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern PCCOutputDirectory* ThePCCOutputDirectoryParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __PhotometricColorCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationParameters.h - Released 2024-12-11T17:43:17Z
