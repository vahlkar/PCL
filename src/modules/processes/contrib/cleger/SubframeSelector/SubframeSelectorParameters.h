//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.1
// ----------------------------------------------------------------------------
// SubframeSelectorParameters.h - Released 2024-12-17T18:15:44Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2021 Cameron Leger
// Copyright (c) 2020-2024 Juan Conejero, PTeam
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

#ifndef __SubframeSelectorParameters_h
#define __SubframeSelectorParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SSRoutine : public MetaEnumeration
{
public:

   enum { MeasureSubframes,
          OutputSubframes,
          StarDetectionPreview,
          NumberOfItems,
          Default = MeasureSubframes };

   SSRoutine( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   int ElementData( size_type ) const;
   IsoString Tooltip() const;
};

extern SSRoutine* TheSSRoutineParameter;

// ----------------------------------------------------------------------------

class SSNonInteractive : public MetaBoolean
{
public:

   SSNonInteractive( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSNonInteractive* TheSSNonInteractiveParameter;

// ----------------------------------------------------------------------------

class SSSubframes : public MetaTable
{
public:

   SSSubframes( MetaProcess* );

   IsoString Id() const override;
};

extern SSSubframes* TheSSSubframesParameter;

// ----------------------------------------------------------------------------

class SSSubframeEnabled : public MetaBoolean
{
public:

   SSSubframeEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SSSubframeEnabled* TheSSSubframeEnabledParameter;

// ----------------------------------------------------------------------------

class SSSubframePath : public MetaString
{
public:

   SSSubframePath( MetaTable* );

   IsoString Id() const override;
};

extern SSSubframePath* TheSSSubframePathParameter;

// ----------------------------------------------------------------------------

class SSLocalNormalizationDataPath : public MetaString
{
public:

   SSLocalNormalizationDataPath( MetaTable* );

   IsoString Id() const override;
};

extern SSLocalNormalizationDataPath* TheSSLocalNormalizationDataPathParameter;

// ----------------------------------------------------------------------------

class SSDrizzlePath : public MetaString
{
public:

   SSDrizzlePath( MetaTable* );

   IsoString Id() const override;
};

extern SSDrizzlePath* TheSSDrizzlePathParameter;

// ----------------------------------------------------------------------------

class SSFileCache : public MetaBoolean
{
public:

   SSFileCache( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSFileCache* TheSSFileCacheParameter;

// ----------------------------------------------------------------------------

class SSSubframeScale : public MetaFloat
{
public:

   SSSubframeScale( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSSubframeScale* TheSSSubframeScaleParameter;

// ----------------------------------------------------------------------------

class SSCameraGain : public MetaFloat
{
public:

   SSCameraGain( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSCameraGain* TheSSCameraGainParameter;

// ----------------------------------------------------------------------------

class SSCameraResolution : public MetaEnumeration
{
public:

   enum { Bits8,
          Bits10,
          Bits12,
          Bits14,
          Bits16,
          NumberOfItems,
          Default = Bits16 };

   SSCameraResolution( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   int ElementData( size_type ) const;
   IsoString Tooltip() const;
};

extern SSCameraResolution* TheSSCameraResolutionParameter;

// ----------------------------------------------------------------------------

class SSSiteLocalMidnight : public MetaInt32
{
public:

   SSSiteLocalMidnight( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSSiteLocalMidnight* TheSSSiteLocalMidnightParameter;

// ----------------------------------------------------------------------------

class SSScaleUnit : public MetaEnumeration
{
public:

   enum { ArcSeconds,
          Pixel,
          NumberOfItems,
          Default = ArcSeconds };

   SSScaleUnit( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   IsoString Tooltip() const;
};

extern SSScaleUnit* TheSSScaleUnitParameter;

// ----------------------------------------------------------------------------

class SSDataUnit : public MetaEnumeration
{
public:

   enum { Electron,
          DataNumber,
          Normalized,
          NumberOfItems,
          Default = Normalized };

   SSDataUnit( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   IsoString Tooltip() const;
};

extern SSDataUnit* TheSSDataUnitParameter;

// ----------------------------------------------------------------------------

class SSTrimmingFactor : public MetaFloat
{
public:

   SSTrimmingFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSTrimmingFactor* TheSSTrimmingFactorParameter;

// ----------------------------------------------------------------------------

class SSStructureLayers : public MetaInt32
{
public:

   SSStructureLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSStructureLayers* TheSSStructureLayersParameter;

// ----------------------------------------------------------------------------

class SSNoiseLayers : public MetaInt32
{
public:

   SSNoiseLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSNoiseLayers* TheSSNoiseLayersParameter;

// ----------------------------------------------------------------------------

class SSHotPixelFilterRadius : public MetaInt32
{
public:

   SSHotPixelFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSHotPixelFilterRadius* TheSSHotPixelFilterRadiusParameter;

// ----------------------------------------------------------------------------

class SSNoiseReductionFilterRadius : public MetaInt32
{
public:

   SSNoiseReductionFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSNoiseReductionFilterRadius* TheSSNoiseReductionFilterRadiusParameter;

// ----------------------------------------------------------------------------

class SSMinStructureSize : public MetaInt32
{
public:

   SSMinStructureSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSMinStructureSize* TheSSMinStructureSizeParameter;

// ----------------------------------------------------------------------------

class SSSensitivity : public MetaFloat
{
public:

   SSSensitivity( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSSensitivity* TheSSSensitivityParameter;

// ----------------------------------------------------------------------------

class SSPeakResponse : public MetaFloat
{
public:

   SSPeakResponse( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSPeakResponse* TheSSPeakResponseParameter;

// ----------------------------------------------------------------------------

class SSBrightThreshold : public MetaFloat
{
public:

   SSBrightThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSBrightThreshold* TheSSBrightThresholdParameter;

// ----------------------------------------------------------------------------

class SSMaxDistortion : public MetaFloat
{
public:

   SSMaxDistortion( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSMaxDistortion* TheSSMaxDistortionParameter;

// ----------------------------------------------------------------------------

class SSAllowClusteredSources : public MetaBoolean
{
public:

   SSAllowClusteredSources( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSAllowClusteredSources* TheSSAllowClusteredSourcesParameter;

// ----------------------------------------------------------------------------

class SSUpperLimit : public MetaFloat
{
public:

   SSUpperLimit( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSUpperLimit* TheSSUpperLimitParameter;

// ----------------------------------------------------------------------------

class SSPSFFit : public MetaEnumeration
{
public:

   enum { Gaussian,
          Moffat10,
          Moffat8,
          Moffat6,
          Moffat4,
          Moffat25,
          Moffat15,
          Lorentzian,
          NumberOfItems,
          Default = Moffat4 };

   SSPSFFit( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   IsoString Tooltip() const;
};

extern SSPSFFit* TheSSPSFFitParameter;

// ----------------------------------------------------------------------------

class SSPSFFitCircular : public MetaBoolean
{
public:

   SSPSFFitCircular( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSPSFFitCircular* TheSSPSFFitCircularParameter;

// ----------------------------------------------------------------------------

class SSMaxPSFFits : public MetaInt32
{
public:

   SSMaxPSFFits( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSMaxPSFFits* TheSSMaxPSFFitsParameter;

// ----------------------------------------------------------------------------

class SSROIX0 : public MetaInt32
{
public:

   SSROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSROIX0* TheSSROIX0Parameter;


// ----------------------------------------------------------------------------

class SSROIY0 : public MetaInt32
{
public:

   SSROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSROIY0* TheSSROIY0Parameter;

// ----------------------------------------------------------------------------

class SSROIX1 : public MetaInt32
{
public:

   SSROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSROIX1* TheSSROIX1Parameter;

// ----------------------------------------------------------------------------

class SSROIY1 : public MetaInt32
{
public:

   SSROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSROIY1* TheSSROIY1Parameter;

// ----------------------------------------------------------------------------

class SSNoNoiseAndSignalWarnings : public MetaBoolean
{
public:

   SSNoNoiseAndSignalWarnings( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SSNoNoiseAndSignalWarnings* TheSSNoNoiseAndSignalWarningsParameter;

// ----------------------------------------------------------------------------

class SSPedestalMode : public MetaEnumeration
{
public:

   enum { Literal,
          Keyword,
          CustomKeyword,
          NumberOfPedestalModes,
          Default = Keyword };

   SSPedestalMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
   IsoString Tooltip() const;
};

extern SSPedestalMode* TheSSPedestalModeParameter;

// ----------------------------------------------------------------------------

class SSPedestal : public MetaInt32
{
public:

   SSPedestal( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   IsoString Tooltip() const;
};

extern SSPedestal* TheSSPedestalParameter;

// ----------------------------------------------------------------------------

class SSPedestalKeyword : public MetaString
{
public:

   SSPedestalKeyword( MetaProcess* );

   IsoString Id() const override;
   IsoString Tooltip() const;
};

extern SSPedestalKeyword* TheSSPedestalKeywordParameter;

// ----------------------------------------------------------------------------

class SSInputHints : public MetaString
{
public:

   SSInputHints( MetaProcess* );

   IsoString Id() const override;

   IsoString Tooltip() const;
};

extern SSInputHints* TheSSInputHintsParameter;

// ----------------------------------------------------------------------------

class SSOutputHints : public MetaString
{
public:

   SSOutputHints( MetaProcess* );

   IsoString Id() const override;
};

extern SSOutputHints* TheSSOutputHintsParameter;

// ----------------------------------------------------------------------------

class SSOutputDirectory : public MetaString
{
public:

   SSOutputDirectory( MetaProcess* );

   IsoString Id() const override;

   IsoString Tooltip() const;
};

extern SSOutputDirectory* TheSSOutputDirectoryParameter;

// ----------------------------------------------------------------------------

class SSOutputExtension : public MetaString
{
public:

   SSOutputExtension( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SSOutputExtension* TheSSOutputExtensionParameter;

// ----------------------------------------------------------------------------

class SSOutputPrefix : public MetaString
{
public:

   SSOutputPrefix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSOutputPrefix* TheSSOutputPrefixParameter;

// ----------------------------------------------------------------------------

class SSOutputPostfix : public MetaString
{
public:

   SSOutputPostfix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSOutputPostfix* TheSSOutputPostfixParameter;

// ----------------------------------------------------------------------------

class SSOutputKeyword : public MetaString
{
public:

   SSOutputKeyword( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSOutputKeyword* TheSSOutputKeywordParameter;

// ----------------------------------------------------------------------------

class SSOverwriteExistingFiles : public MetaBoolean
{
public:

   SSOverwriteExistingFiles( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSOverwriteExistingFiles* TheSSOverwriteExistingFilesParameter;

// ----------------------------------------------------------------------------

class SSOnError : public MetaEnumeration
{
public:

   enum { Continue,
          Abort,
          AskUser,
          NumberOfItems,
          Default = Continue };

   SSOnError( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   IsoString Tooltip() const;
};

extern SSOnError* TheSSOnErrorParameter;

// ----------------------------------------------------------------------------

class SSApprovalExpression : public MetaString
{
public:

   SSApprovalExpression( MetaProcess* );

   IsoString Id() const override;

   IsoString Tooltip() const;
};

extern SSApprovalExpression* TheSSApprovalExpressionParameter;

// ----------------------------------------------------------------------------

class SSWeightingExpression : public MetaString
{
public:

   SSWeightingExpression( MetaProcess* );

   IsoString Id() const override;

   IsoString Tooltip() const;
};

extern SSWeightingExpression* TheSSWeightingExpressionParameter;

// ----------------------------------------------------------------------------

class SSSortingProperty : public MetaEnumeration
{
public:

   enum { Index,
          Weight,
          FWHM,
          Eccentricity,
          SNRWeight,
          Median,
          MedianMeanDev,
          Noise,
          NoiseRatio,
          Stars,
          StarResidual,
          FWHMMeanDev,
          EccentricityMeanDev,
          StarResidualMeanDev,
          PSFSignalWeight,
          Azimuth,
          Altitude,
          PSFFlux,
          PSFFluxPower,
          PSFTotalMeanFlux,
          PSFTotalMeanPowerFlux,
          PSFCount,
          MStar,
          NStar,
          PSFSNR,
          PSFScale,
          PSFScaleSNR,
          NumberOfItems,
          Default = Index };

   SSSortingProperty( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   IsoString ElementAliases() const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
};

extern SSSortingProperty* TheSSSortingPropertyParameter;

// ----------------------------------------------------------------------------

class SSGraphPropertyBase : public MetaEnumeration
{
public:

   enum { Weight,
          FWHM,
          Eccentricity,
          SNRWeight,
          Median,
          MedianMeanDev,
          Noise,
          NoiseRatio,
          Stars,
          StarResidual,
          FWHMMeanDev,
          EccentricityMeanDev,
          StarResidualMeanDev,
          PSFSignalWeight,
          Azimuth,
          Altitude,
          PSFFlux,
          PSFFluxPower,
          PSFTotalMeanFlux,
          PSFTotalMeanPowerFlux,
          PSFCount,
          MStar,
          NStar,
          PSFSNR,
          PSFScale,
          PSFScaleSNR,
          NumberOfItems,
          Default = PSFSignalWeight };

   SSGraphPropertyBase( MetaProcess* );

   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   IsoString ElementAliases() const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
};

class SSGraphProperty : public SSGraphPropertyBase
{
public:

   SSGraphProperty( MetaProcess* );

   IsoString Id() const override;
};

extern SSGraphProperty* TheSSGraphPropertyParameter;

class SSAuxGraphProperty : public SSGraphPropertyBase
{
public:

   SSAuxGraphProperty( MetaProcess* );

   IsoString Id() const override;
};

extern SSAuxGraphProperty* TheSSAuxGraphPropertyParameter;

// ----------------------------------------------------------------------------

class SSUseFileThreads : public MetaBoolean
{
public:

   SSUseFileThreads( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SSUseFileThreads* TheSSUseFileThreadsParameter;

// ----------------------------------------------------------------------------

class SSFileThreadOverload : public MetaFloat
{
public:

   SSFileThreadOverload( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSFileThreadOverload* TheSSFileThreadOverloadParameter;

// ----------------------------------------------------------------------------

class SSMaxFileReadThreads : public MetaInt32
{
public:

   SSMaxFileReadThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMaxFileReadThreads* TheSSMaxFileReadThreadsParameter;

// ----------------------------------------------------------------------------

class SSMaxFileWriteThreads : public MetaInt32
{
public:

   SSMaxFileWriteThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMaxFileWriteThreads* TheSSMaxFileWriteThreadsParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class SSMeasurements : public MetaTable
{
public:

   SSMeasurements( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurements* TheSSMeasurementsParameter;

// ----------------------------------------------------------------------------

class SSMeasurementIndex : public MetaUInt32
{
public:

   SSMeasurementIndex( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementIndex* TheSSMeasurementIndexParameter;

// ----------------------------------------------------------------------------

class SSMeasurementEnabled : public MetaBoolean
{
public:

   SSMeasurementEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementEnabled* TheSSMeasurementEnabledParameter;

// ----------------------------------------------------------------------------

class SSMeasurementLocked : public MetaBoolean
{
public:

   SSMeasurementLocked( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementLocked* TheSSMeasurementLockedParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPath : public MetaString
{
public:

   SSMeasurementPath( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementPath* TheSSMeasurementPathParameter;

// ----------------------------------------------------------------------------

class SSMeasurementWeight : public MetaDouble
{
public:

   SSMeasurementWeight( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementWeight* TheSSMeasurementWeightParameter;

// ----------------------------------------------------------------------------

class SSMeasurementFWHM : public MetaDouble
{
public:

   SSMeasurementFWHM( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementFWHM* TheSSMeasurementFWHMParameter;

// ----------------------------------------------------------------------------

class SSMeasurementEccentricity : public MetaDouble
{
public:

   SSMeasurementEccentricity( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementEccentricity* TheSSMeasurementEccentricityParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPSFSignalWeight : public MetaDouble
{
public:

   SSMeasurementPSFSignalWeight( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementPSFSignalWeight* TheSSMeasurementPSFSignalWeightParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPSFSNR : public MetaDouble
{
public:

   SSMeasurementPSFSNR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementPSFSNR* TheSSMeasurementPSFSNRParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPSFScale : public MetaDouble
{
public:

   SSMeasurementPSFScale( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementPSFScale* TheSSMeasurementPSFScaleParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPSFScaleSNR : public MetaDouble
{
public:

   SSMeasurementPSFScaleSNR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementPSFScaleSNR* TheSSMeasurementPSFScaleSNRParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPSFFlux : public MetaDouble
{
public:

   SSMeasurementPSFFlux( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementPSFFlux* TheSSMeasurementPSFFluxParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPSFFluxPower : public MetaDouble
{
public:

   SSMeasurementPSFFluxPower( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementPSFFluxPower* TheSSMeasurementPSFFluxPowerParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPSFTotalMeanFlux : public MetaDouble
{
public:

   SSMeasurementPSFTotalMeanFlux( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementPSFTotalMeanFlux* TheSSMeasurementPSFTotalMeanFluxParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPSFTotalMeanPowerFlux : public MetaDouble
{
public:

   SSMeasurementPSFTotalMeanPowerFlux( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementPSFTotalMeanPowerFlux* TheSSMeasurementPSFTotalMeanPowerFluxParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPSFCount : public MetaUInt32
{
public:

   SSMeasurementPSFCount( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementPSFCount* TheSSMeasurementPSFCountParameter;

// ----------------------------------------------------------------------------

class SSMeasurementMStar : public MetaDouble
{
public:

   SSMeasurementMStar( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementMStar* TheSSMeasurementMStarParameter;

// ----------------------------------------------------------------------------

class SSMeasurementNStar : public MetaDouble
{
public:

   SSMeasurementNStar( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementNStar* TheSSMeasurementNStarParameter;

// ----------------------------------------------------------------------------

class SSMeasurementSNRWeight : public MetaDouble
{
public:

   SSMeasurementSNRWeight( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementSNRWeight* TheSSMeasurementSNRWeightParameter;

// ----------------------------------------------------------------------------

class SSMeasurementMedian : public MetaDouble
{
public:

   SSMeasurementMedian( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementMedian* TheSSMeasurementMedianParameter;

// ----------------------------------------------------------------------------

class SSMeasurementMedianMeanDev : public MetaDouble
{
public:

   SSMeasurementMedianMeanDev( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementMedianMeanDev* TheSSMeasurementMedianMeanDevParameter;

// ----------------------------------------------------------------------------

class SSMeasurementNoise : public MetaDouble
{
public:

   SSMeasurementNoise( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementNoise* TheSSMeasurementNoiseParameter;

// ----------------------------------------------------------------------------

class SSMeasurementNoiseRatio : public MetaDouble
{
public:

   SSMeasurementNoiseRatio( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementNoiseRatio* TheSSMeasurementNoiseRatioParameter;

// ----------------------------------------------------------------------------

class SSMeasurementStars : public MetaUInt32
{
public:

   SSMeasurementStars( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementStars* TheSSMeasurementStarsParameter;

// ----------------------------------------------------------------------------

class SSMeasurementStarResidual : public MetaDouble
{
public:

   SSMeasurementStarResidual( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementStarResidual* TheSSMeasurementStarResidualParameter;

// ----------------------------------------------------------------------------

class SSMeasurementFWHMMeanDev : public MetaDouble
{
public:

   SSMeasurementFWHMMeanDev( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementFWHMMeanDev* TheSSMeasurementFWHMMeanDevParameter;

// ----------------------------------------------------------------------------

class SSMeasurementEccentricityMeanDev : public MetaDouble
{
public:

   SSMeasurementEccentricityMeanDev( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementEccentricityMeanDev* TheSSMeasurementEccentricityMeanDevParameter;

// ----------------------------------------------------------------------------

class SSMeasurementStarResidualMeanDev : public MetaDouble
{
public:

   SSMeasurementStarResidualMeanDev( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementStarResidualMeanDev* TheSSMeasurementStarResidualMeanDevParameter;

// ----------------------------------------------------------------------------

class SSMeasurementAzimuth : public MetaDouble
{
public:

   SSMeasurementAzimuth( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementAzimuth* TheSSMeasurementAzimuthParameter;

// ----------------------------------------------------------------------------

class SSMeasurementAltitude : public MetaDouble
{
public:

   SSMeasurementAltitude( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementAltitude* TheSSMeasurementAltitudeParameter;

// ----------------------------------------------------------------------------

class SSMeasurementUnused01 : public MetaDouble
{
public:

   SSMeasurementUnused01( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   bool IsReadOnly() const override;
};

extern SSMeasurementUnused01* TheSSMeasurementUnused01Parameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __SubframeSelectorParameters_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorParameters.h - Released 2024-12-17T18:15:44Z
