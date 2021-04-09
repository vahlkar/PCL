//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.5.1
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationParameters.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L. All Rights Reserved.
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

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class PCCWorkingMode : public MetaEnumeration
{
public:

   enum { Broadband,
          Narrowband,
          NumberOfItems,
          Default = Broadband };

   PCCWorkingMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern PCCWorkingMode* ThePCCWorkingModeParameter;

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

class PCCRedFilterWavelength : public MetaFloat
{
public:

   PCCRedFilterWavelength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PCCRedFilterWavelength* ThePCCRedFilterWavelengthParameter;

// ----------------------------------------------------------------------------

class PCCRedFilterBandwidth : public MetaFloat
{
public:

   PCCRedFilterBandwidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PCCRedFilterBandwidth* ThePCCRedFilterBandwidthParameter;

// ----------------------------------------------------------------------------

class PCCGreenFilterWavelength : public MetaFloat
{
public:

   PCCGreenFilterWavelength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PCCGreenFilterWavelength* ThePCCGreenFilterWavelengthParameter;

// ----------------------------------------------------------------------------

class PCCGreenFilterBandwidth : public MetaFloat
{
public:

   PCCGreenFilterBandwidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PCCGreenFilterBandwidth* ThePCCGreenFilterBandwidthParameter;

// ----------------------------------------------------------------------------

class PCCBlueFilterWavelength : public MetaFloat
{
public:

   PCCBlueFilterWavelength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PCCBlueFilterWavelength* ThePCCBlueFilterWavelengthParameter;

// ----------------------------------------------------------------------------

class PCCBlueFilterBandwidth : public MetaFloat
{
public:

   PCCBlueFilterBandwidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern PCCBlueFilterBandwidth* ThePCCBlueFilterBandwidthParameter;

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

class PCCFocalLength : public MetaFloat
{
public:

   PCCFocalLength( MetaProcess* );
   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCFocalLength* ThePCCFocalLengthParameter;

// ----------------------------------------------------------------------------

class PCCPixelSize : public MetaFloat
{
public:

   PCCPixelSize( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCPixelSize* ThePCCPixelSizeParameter;

// ----------------------------------------------------------------------------

class PCCCenterRA : public MetaDouble
{
public:

   PCCCenterRA( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCCenterRA* ThePCCCenterRAParameter;

// ----------------------------------------------------------------------------

class PCCCenterDec : public MetaDouble
{
public:

   PCCCenterDec( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCCenterDec* ThePCCCenterDecParameter;

// ----------------------------------------------------------------------------

class PCCEpochJD : public MetaDouble
{
public:

   PCCEpochJD( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCEpochJD* ThePCCEpochJDParameter;

// ----------------------------------------------------------------------------

class PCCForcePlateSolve : public MetaBoolean
{
public:

   PCCForcePlateSolve( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCForcePlateSolve* ThePCCForcePlateSolveParameter;

// ----------------------------------------------------------------------------

class PCCIgnoreImagePositionAndScale : public MetaBoolean
{
public:

   PCCIgnoreImagePositionAndScale( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCIgnoreImagePositionAndScale* ThePCCIgnoreImagePositionAndScaleParameter;

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

class PCCSolverCatalogName : public MetaString
{
public:

   PCCSolverCatalogName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern PCCSolverCatalogName* ThePCCSolverCatalogNameParameter;

// ----------------------------------------------------------------------------

class PCCSolverAutoCatalog : public MetaBoolean
{
public:

   PCCSolverAutoCatalog( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCSolverAutoCatalog* ThePCCSolverAutoCatalogParameter;

// ----------------------------------------------------------------------------

class PCCSolverLimitMagnitude : public MetaInt32
{
public:

   PCCSolverLimitMagnitude( MetaProcess* );

   IsoString Id() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCSolverLimitMagnitude* ThePCCSolverLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class PCCSolverAutoLimitMagnitude : public MetaBoolean
{
public:

   PCCSolverAutoLimitMagnitude( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCSolverAutoLimitMagnitude* ThePCCSolverAutoLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class PCCSolverAutoLimitMagnitudeFactor : public MetaFloat
{
public:

   PCCSolverAutoLimitMagnitudeFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCSolverAutoLimitMagnitudeFactor* ThePCCSolverAutoLimitMagnitudeFactorParameter;

// ----------------------------------------------------------------------------

class PCCSolverStarSensitivity : public MetaFloat
{
public:

   PCCSolverStarSensitivity( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCSolverStarSensitivity* ThePCCSolverStarSensitivityParameter;

// ----------------------------------------------------------------------------

class PCCSolverNoiseLayers : public MetaInt32
{
public:

   PCCSolverNoiseLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern PCCSolverNoiseLayers* ThePCCSolverNoiseLayersParameter;

// ----------------------------------------------------------------------------

class PCCSolverAlignmentDevice : public MetaEnumeration
{
public:

   // N.B.: Enumerated items must be coherent with the ImageSolver script.
   enum { TriangleSimilarity,
          PolygonMatching,
          NumberOfItems,
          Default = TriangleSimilarity };

   PCCSolverAlignmentDevice( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern PCCSolverAlignmentDevice* ThePCCSolverAlignmentDeviceParameter;

// ----------------------------------------------------------------------------

class PCCSolverDistortionCorrection : public MetaBoolean
{
public:

   PCCSolverDistortionCorrection( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCSolverDistortionCorrection* ThePCCSolverDistortionCorrectionParameter;

// ----------------------------------------------------------------------------

class PCCSolverSplineSmoothing : public MetaFloat
{
public:

   PCCSolverSplineSmoothing( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCSolverSplineSmoothing* ThePCCSolverSplineSmoothingParameter;

// ----------------------------------------------------------------------------

class PCCSolverProjection : public MetaEnumeration
{
public:

   // N.B.: Enumerated items must be coherent with the ImageSolver script.
   enum { Gnomonic,
          Stereographic,
          PlateCarree,
          Mercator,
          HammerAitoff,
          ZenithalEqualArea,
          Orthographic,
          NumberOfItems,
          Default = Gnomonic };

   PCCSolverProjection( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern PCCSolverProjection* ThePCCSolverProjectionParameter;

// ----------------------------------------------------------------------------

class PCCPhotCatalogName : public MetaString
{
public:

   PCCPhotCatalogName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern PCCPhotCatalogName* ThePCCPhotCatalogNameParameter;

// ----------------------------------------------------------------------------

class PCCPhotAutoCatalog : public MetaBoolean
{
public:

   PCCPhotAutoCatalog( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCPhotAutoCatalog* ThePCCPhotAutoCatalogParameter;

// ----------------------------------------------------------------------------

class PCCPhotLimitMagnitude : public MetaInt32
{
public:

   PCCPhotLimitMagnitude( MetaProcess* );

   IsoString Id() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCPhotLimitMagnitude* ThePCCPhotLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class PCCPhotAutoLimitMagnitude : public MetaBoolean
{
public:

   PCCPhotAutoLimitMagnitude( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCPhotAutoLimitMagnitude* ThePCCPhotAutoLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class PCCPhotAutoLimitMagnitudeFactor : public MetaFloat
{
public:

   PCCPhotAutoLimitMagnitudeFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCPhotAutoLimitMagnitudeFactor* ThePCCPhotAutoLimitMagnitudeFactorParameter;

// ----------------------------------------------------------------------------

class PCCPhotAutoAperture : public MetaBoolean
{
public:

   PCCPhotAutoAperture( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCPhotAutoAperture* ThePCCPhotAutoApertureParameter;

// ----------------------------------------------------------------------------

class PCCPhotAperture : public MetaInt32
{
public:

   PCCPhotAperture( MetaProcess* );

   IsoString Id() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCPhotAperture* ThePCCPhotApertureParameter;

// ----------------------------------------------------------------------------

class PCCPhotUsePSF : public MetaBoolean
{
public:

   PCCPhotUsePSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCPhotUsePSF* ThePCCPhotUsePSFParameter;

// ----------------------------------------------------------------------------

class PCCPhotSaturationThreshold : public MetaFloat
{
public:

   PCCPhotSaturationThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern PCCPhotSaturationThreshold* ThePCCPhotSaturationThresholdParameter;

// ----------------------------------------------------------------------------

class PCCPhotShowDetectedStars : public MetaBoolean
{
public:

   PCCPhotShowDetectedStars( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCPhotShowDetectedStars* ThePCCPhotShowDetectedStarsParameter;

// ----------------------------------------------------------------------------

class PCCPhotShowBackgroundModels : public MetaBoolean
{
public:

   PCCPhotShowBackgroundModels( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCPhotShowBackgroundModels* ThePCCPhotShowBackgroundModelsParameter;

// ----------------------------------------------------------------------------

class PCCPhotGenerateGraphs : public MetaBoolean
{
public:

   PCCPhotGenerateGraphs( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern PCCPhotGenerateGraphs* ThePCCPhotGenerateGraphsParameter;

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

PCL_END_LOCAL

} // pcl

#endif   // __PhotometricColorCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationParameters.h - Released 2021-04-09T19:41:48Z
