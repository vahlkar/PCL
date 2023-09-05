//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.5.2
// ----------------------------------------------------------------------------
// ImageIntegrationParameters.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#ifndef __ImageIntegrationParameters_h
#define __ImageIntegrationParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/PSFSignalEstimator.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class IIImages : public MetaTable
{
public:

   IIImages( MetaProcess* );

   IsoString Id() const override;
};

extern IIImages* TheIIImagesParameter;

// ----------------------------------------------------------------------------

class IIImageEnabled : public MetaBoolean
{
public:

   IIImageEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIImageEnabled* TheIIImageEnabledParameter;

// ----------------------------------------------------------------------------

class IIImagePath : public MetaString
{
public:

   IIImagePath( MetaTable* );

   IsoString Id() const override;
};

extern IIImagePath* TheIIImagePathParameter;

// ----------------------------------------------------------------------------

class IIDrizzlePath : public MetaString
{
public:

   IIDrizzlePath( MetaTable* );

   IsoString Id() const override;
};

extern IIDrizzlePath* TheIIDrizzlePathParameter;

// ----------------------------------------------------------------------------

class IILocalNormalizationDataPath : public MetaString
{
public:

   IILocalNormalizationDataPath( MetaTable* );

   IsoString Id() const override;
};

extern IILocalNormalizationDataPath* TheIILocalNormalizationDataPathParameter;

// ----------------------------------------------------------------------------

class IIInputHints : public MetaString
{
public:

   IIInputHints( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern IIInputHints* TheIIInputHintsParameter;

// ----------------------------------------------------------------------------

class IICombination : public MetaEnumeration
{
public:

   enum { Average,
          Median,
          Minimum,
          Maximum,
          NumberOfItems,
          Default = Average };

   IICombination( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern IICombination* TheIICombinationParameter;

// ----------------------------------------------------------------------------

class IIWeightMode : public MetaEnumeration
{
public:

   enum { DontCare,             // weight = 1
          ExposureTimeWeight,   // EXPTIME keyword or ImageOptions::exposure
          SNREstimate,          // Scaled inverse noise variance
          SignalWeight,         // mean - median distance
          MedianWeight,         // median
          AverageWeight,        // mean
          KeywordWeight,        // user-specified keyword
          PSFSignalWeight,      // PSF Signal Weight (PSFSW) estimates
          PSFSNR,               // PSF SNR (PSFSNR) estimates
          PSFScaleSNR,          // PSF Scale SNR (PSFSSNR) - requires local normalization data
          NumberOfItems,
          Default = PSFSignalWeight };

   IIWeightMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   IsoString ElementAliases() const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static String ElementLabel( int );
};

extern IIWeightMode* TheIIWeightModeParameter;

// ----------------------------------------------------------------------------

class IIWeightKeyword : public MetaString
{
public:

   IIWeightKeyword( MetaProcess* );

   IsoString Id() const override;
};

extern IIWeightKeyword* TheIIWeightKeywordParameter;

// ----------------------------------------------------------------------------

class IIWeightScale : public MetaEnumeration
{
public:

   enum { AvgDev,    // average deviation from the median
          MAD,       // median deviation from the median (MAD)
          BWMV,      // biweight midvariance
          PBMV,      // DEPRECATED - percentage bend midvariance
          Sn,        // DEPRECATED - Rousseeuw/Croux Sn estimator of scale
          Qn,        // DEPRECATED - Rousseeuw/Croux Qn estimator of scale
          IKSS,      // DEPRECATED - iterative k-sigma estimator of scale
          NumberOfItems,
          Default = BWMV };

   IIWeightScale( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern IIWeightScale* TheIIWeightScaleParameter;

// ----------------------------------------------------------------------------

class IIMinWeight : public MetaFloat
{
public:

   IIMinWeight( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIMinWeight* TheIIMinWeightParameter;

// ----------------------------------------------------------------------------

class IICSVWeights : public MetaString
{
public:

   IICSVWeights( MetaProcess* );

   IsoString Id() const override;
};

extern IICSVWeights* TheIICSVWeightsParameter;

// ----------------------------------------------------------------------------

class IIAdaptiveGridSize : public MetaInt32
{
public:

   IIAdaptiveGridSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIAdaptiveGridSize* TheIIAdaptiveGridSizeParameter;

// ----------------------------------------------------------------------------

class IIAdaptiveNoScale : public MetaBoolean
{
public:

   IIAdaptiveNoScale( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIAdaptiveNoScale* TheIIAdaptiveNoScaleParameter;

// ----------------------------------------------------------------------------

class IIIgnoreNoiseKeywords : public MetaBoolean
{
public:

   IIIgnoreNoiseKeywords( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIIgnoreNoiseKeywords* TheIIIgnoreNoiseKeywordsParameter;

// ----------------------------------------------------------------------------

class IINormalization : public MetaEnumeration
{
public:

   enum { NoNormalization,
          Additive,
          Multiplicative,
          AdditiveWithScaling,
          MultiplicativeWithScaling,
          LocalNormalization,
          AdaptiveNormalization,
          NumberOfItems,
          Default = AdditiveWithScaling };

   IINormalization( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern IINormalization* TheIINormalizationParameter;

// ----------------------------------------------------------------------------

class IIRejection : public MetaEnumeration
{
public:

   enum { NoRejection,
          MinMax,
          PercentileClip,
          SigmaClip,
          WinsorizedSigmaClip,
          AveragedSigmaClip,
          LinearFit,
          CCDClip, // ### Deprecated
          ESD,
          RCR,
          NumberOfItems,
          Default = NoRejection };

   IIRejection( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static String ElementLabel( int );
};

extern IIRejection* TheIIRejectionParameter;

// ----------------------------------------------------------------------------

class IIRejectionNormalization : public MetaEnumeration
{
public:

   enum { NoRejectionNormalization,
          Scale,
          EqualizeFluxes,
          LocalRejectionNormalization,
          AdaptiveRejectionNormalization,
          NumberOfItems,
          Default = Scale };

   IIRejectionNormalization( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern IIRejectionNormalization* TheIIRejectionNormalizationParameter;

// ----------------------------------------------------------------------------

class IIMinMaxLow : public MetaInt32
{
public:

   IIMinMaxLow( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern IIMinMaxLow* TheIIMinMaxLowParameter;

// ----------------------------------------------------------------------------

class IIMinMaxHigh : public MetaInt32
{
public:

   IIMinMaxHigh( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern IIMinMaxHigh* TheIIMinMaxHighParameter;

// ----------------------------------------------------------------------------

class IIPercentileLow : public MetaFloat
{
public:

   IIPercentileLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIPercentileLow* TheIIPercentileLowParameter;

// ----------------------------------------------------------------------------

class IIPercentileHigh : public MetaFloat
{
public:

   IIPercentileHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIPercentileHigh* TheIIPercentileHighParameter;

// ----------------------------------------------------------------------------

class IISigmaLow : public MetaFloat
{
public:

   IISigmaLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IISigmaLow* TheIISigmaLowParameter;

// ----------------------------------------------------------------------------

class IISigmaHigh : public MetaFloat
{
public:

   IISigmaHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IISigmaHigh* TheIISigmaHighParameter;

// ----------------------------------------------------------------------------

class IIWinsorizationCutoff : public MetaFloat
{
public:

   IIWinsorizationCutoff( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIWinsorizationCutoff* TheIIWinsorizationCutoffParameter;

// ----------------------------------------------------------------------------

class IILinearFitLow : public MetaFloat
{
public:

   IILinearFitLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IILinearFitLow* TheIILinearFitLowParameter;

// ----------------------------------------------------------------------------

class IILinearFitHigh : public MetaFloat
{
public:

   IILinearFitHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IILinearFitHigh* TheIILinearFitHighParameter;

// ----------------------------------------------------------------------------

class IIESDOutliersFraction : public MetaFloat
{
public:

   IIESDOutliersFraction( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIESDOutliersFraction* TheIIESDOutliersFractionParameter;

// ----------------------------------------------------------------------------

class IIESDAlpha : public MetaFloat
{
public:

   IIESDAlpha( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIESDAlpha* TheIIESDAlphaParameter;

// ----------------------------------------------------------------------------

class IIESDLowRelaxation : public MetaFloat
{
public:

   IIESDLowRelaxation( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIESDLowRelaxation* TheIIESDLowRelaxationParameter;

// ----------------------------------------------------------------------------

class IIRCRLimit : public MetaFloat
{
public:

   IIRCRLimit( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIRCRLimit* TheIIRCRLimitParameter;

// ----------------------------------------------------------------------------

class IICCDGain : public MetaFloat
{
public:

   IICCDGain( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IICCDGain* TheIICCDGainParameter;

// ----------------------------------------------------------------------------

class IICCDReadNoise : public MetaFloat
{
public:

   IICCDReadNoise( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IICCDReadNoise* TheIICCDReadNoiseParameter;

// ----------------------------------------------------------------------------

class IICCDScaleNoise : public MetaFloat
{
public:

   IICCDScaleNoise( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IICCDScaleNoise* TheIICCDScaleNoiseParameter;

// ----------------------------------------------------------------------------

class IIClipLow : public MetaBoolean
{
public:

   IIClipLow( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIClipLow* TheIIClipLowParameter;

// ----------------------------------------------------------------------------

class IIClipHigh : public MetaBoolean
{
public:

   IIClipHigh( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIClipHigh* TheIIClipHighParameter;

// ----------------------------------------------------------------------------

class IIRangeClipLow : public MetaBoolean
{
public:

   IIRangeClipLow( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIRangeClipLow* TheIIRangeClipLowParameter;

// ----------------------------------------------------------------------------

class IIRangeLow : public MetaFloat
{
public:

   IIRangeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIRangeLow* TheIIRangeLowParameter;

// ----------------------------------------------------------------------------

class IIRangeClipHigh : public MetaBoolean
{
public:

   IIRangeClipHigh( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIRangeClipHigh* TheIIRangeClipHighParameter;

// ----------------------------------------------------------------------------

class IIRangeHigh : public MetaFloat
{
public:

   IIRangeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIRangeHigh* TheIIRangeHighParameter;

// ----------------------------------------------------------------------------

class IIMapRangeRejection : public MetaBoolean
{
public:

   IIMapRangeRejection( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIMapRangeRejection* TheIIMapRangeRejectionParameter;

// ----------------------------------------------------------------------------

class IIReportRangeRejection : public MetaBoolean
{
public:

   IIReportRangeRejection( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIReportRangeRejection* TheIIReportRangeRejectionParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipLow : public MetaBoolean
{
public:

   IILargeScaleClipLow( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IILargeScaleClipLow* TheIILargeScaleClipLowParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipLowProtectedLayers : public MetaInt32
{
public:

   IILargeScaleClipLowProtectedLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IILargeScaleClipLowProtectedLayers* TheIILargeScaleClipLowProtectedLayersParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipLowGrowth : public MetaInt32
{
public:

   IILargeScaleClipLowGrowth( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IILargeScaleClipLowGrowth* TheIILargeScaleClipLowGrowthParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipHigh : public MetaBoolean
{
public:

   IILargeScaleClipHigh( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IILargeScaleClipHigh* TheIILargeScaleClipHighParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipHighProtectedLayers : public MetaInt32
{
public:

   IILargeScaleClipHighProtectedLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IILargeScaleClipHighProtectedLayers* TheIILargeScaleClipHighProtectedLayersParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipHighGrowth : public MetaInt32
{
public:

   IILargeScaleClipHighGrowth( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IILargeScaleClipHighGrowth* TheIILargeScaleClipHighGrowthParameter;

// ----------------------------------------------------------------------------

class IIGenerate64BitResult : public MetaBoolean
{
public:

   IIGenerate64BitResult( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIGenerate64BitResult* TheIIGenerate64BitResultParameter;

// ----------------------------------------------------------------------------

class IIGenerateRejectionMaps : public MetaBoolean
{
public:

   IIGenerateRejectionMaps( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIGenerateRejectionMaps* TheIIGenerateRejectionMapsParameter;

// ----------------------------------------------------------------------------

class IIGenerateIntegratedImage : public MetaBoolean
{
public:

   IIGenerateIntegratedImage( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIGenerateIntegratedImage* TheIIGenerateIntegratedImageParameter;

// ----------------------------------------------------------------------------

class IIGenerateDrizzleData : public MetaBoolean
{
public:

   IIGenerateDrizzleData( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIGenerateDrizzleData* TheIIGenerateDrizzleDataParameter;

// ----------------------------------------------------------------------------

class IIClosePreviousImages : public MetaBoolean
{
public:

   IIClosePreviousImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIClosePreviousImages* TheIIClosePreviousImagesParameter;

// ----------------------------------------------------------------------------

class IIBufferSize : public MetaInt32
{
public:

   IIBufferSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIBufferSize* TheIIBufferSizeParameter;

// ----------------------------------------------------------------------------

class IIStackSize : public MetaInt32
{
public:

   IIStackSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIStackSize* TheIIStackSizeParameter;

// ----------------------------------------------------------------------------

class IIAutoMemorySize : public MetaBoolean
{
public:

   IIAutoMemorySize( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIAutoMemorySize* TheIIAutoMemorySizeParameter;

// ----------------------------------------------------------------------------

class IIAutoMemoryLimit : public MetaFloat
{
public:

   IIAutoMemoryLimit( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIAutoMemoryLimit* TheIIAutoMemoryLimitParameter;

// ----------------------------------------------------------------------------

class IIUseROI : public MetaBoolean
{
public:

   IIUseROI( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIUseROI* TheIIUseROIParameter;

// ----------------------------------------------------------------------------

class IIROIX0 : public MetaInt32
{
public:

   IIROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIROIX0* TheIIROIX0Parameter;


// ----------------------------------------------------------------------------

class IIROIY0 : public MetaInt32
{
public:

   IIROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIROIY0* TheIIROIY0Parameter;

// ----------------------------------------------------------------------------

class IIROIX1 : public MetaInt32
{
public:

   IIROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIROIX1* TheIIROIX1Parameter;

// ----------------------------------------------------------------------------

class IIROIY1 : public MetaInt32
{
public:

   IIROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIROIY1* TheIIROIY1Parameter;

// ----------------------------------------------------------------------------

class IIUseCache : public MetaBoolean
{
public:

   IIUseCache( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIUseCache* TheIIUseCacheParameter;

// ----------------------------------------------------------------------------

class IIEvaluateSNR : public MetaBoolean
{
public:

   IIEvaluateSNR( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern IIEvaluateSNR* TheIIEvaluateSNRParameter;

// ----------------------------------------------------------------------------

class IINoiseEvaluationAlgorithm : public MetaEnumeration
{
public:

   enum { KSigma,
          MRS,
          NStar,
          NumberOfItems,
          Default = MRS };

   IINoiseEvaluationAlgorithm( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern IINoiseEvaluationAlgorithm* TheIINoiseEvaluationAlgorithmParameter;

// ----------------------------------------------------------------------------

class IIMRSMinDataFraction : public MetaFloat
{
public:

   IIMRSMinDataFraction( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIMRSMinDataFraction* TheIIMRSMinDataFractionParameter;

// ----------------------------------------------------------------------------

class IIPSFStructureLayers : public MetaInt32
{
public:

   IIPSFStructureLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIPSFStructureLayers* TheIIPSFStructureLayersParameter;

// ----------------------------------------------------------------------------

class IIPSFType : public MetaEnumeration
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
          Default = Moffat4 };

   IIPSFType( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static PSFSignalEstimator::psf_function ToPSFFunction( pcl_enum x )
   {
      switch ( x )
      {
      case Gaussian: return PSFunction::Gaussian;
      case Moffat15: return PSFunction::Moffat15;
      default:
      case Moffat4:  return PSFunction::Moffat4;
      case Moffat6:  return PSFunction::Moffat6;
      case Moffat8:  return PSFunction::Moffat8;
      case MoffatA:  return PSFunction::MoffatA;
      case Auto:     return PSFunction::Auto;
      }
   }

   static IsoString FunctionName( pcl_enum );
};

extern IIPSFType* TheIIPSFTypeParameter;

// ----------------------------------------------------------------------------

class IISubtractPedestals : public MetaBoolean
{
public:

   IISubtractPedestals( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IISubtractPedestals* TheIISubtractPedestalsParameter;

// ----------------------------------------------------------------------------

class IITruncateOnOutOfRange : public MetaBoolean
{
public:

   IITruncateOnOutOfRange( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IITruncateOnOutOfRange* TheIITruncateOnOutOfRangeParameter;

// ----------------------------------------------------------------------------

// ### DEPRECATED
class IINoGUIMessages : public MetaBoolean
{
public:

   IINoGUIMessages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IINoGUIMessages* TheIINoGUIMessagesParameter;

// ----------------------------------------------------------------------------

class IIShowImages : public MetaBoolean
{
public:

   IIShowImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIShowImages* TheIIShowImagesParameter;

// ----------------------------------------------------------------------------

class IIUseFileThreads : public MetaBoolean
{
public:

   IIUseFileThreads( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIUseFileThreads* TheIIUseFileThreadsParameter;

// ----------------------------------------------------------------------------

class IIFileThreadOverload : public MetaFloat
{
public:

   IIFileThreadOverload( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIFileThreadOverload* TheIIFileThreadOverloadParameter;

// ----------------------------------------------------------------------------

class IIUseBufferThreads : public MetaBoolean
{
public:

   IIUseBufferThreads( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IIUseBufferThreads* TheIIUseBufferThreadsParameter;

// ----------------------------------------------------------------------------

class IIMaxBufferThreads : public MetaInt32
{
public:

   IIMaxBufferThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IIMaxBufferThreads* TheIIMaxBufferThreadsParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Output properties
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class IIIntegrationImageId : public MetaString
{
public:

   IIIntegrationImageId( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIIntegrationImageId* TheIIIntegrationImageIdParameter;

// ----------------------------------------------------------------------------

class IILowRejectionMapImageId : public MetaString
{
public:

   IILowRejectionMapImageId( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IILowRejectionMapImageId* TheIILowRejectionMapImageIdParameter;

// ----------------------------------------------------------------------------

class IIHighRejectionMapImageId : public MetaString
{
public:

   IIHighRejectionMapImageId( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIHighRejectionMapImageId* TheIIHighRejectionMapImageIdParameter;

// ----------------------------------------------------------------------------

class IISlopeMapImageId : public MetaString
{
public:

   IISlopeMapImageId( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IISlopeMapImageId* TheIISlopeMapImageIdParameter;

// ----------------------------------------------------------------------------

class IINumberOfChannels : public MetaInt32
{
public:

   IINumberOfChannels( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IINumberOfChannels* TheIINumberOfChannelsParameter;

// ----------------------------------------------------------------------------

class IINumberOfPixels : public MetaUInt64
{
public:

   IINumberOfPixels( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IINumberOfPixels* TheIINumberOfPixelsParameter;

// ----------------------------------------------------------------------------

class IITotalPixels : public MetaUInt64
{
public:

   IITotalPixels( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IITotalPixels* TheIITotalPixelsParameter;

// ----------------------------------------------------------------------------

class IIOutputRangeLow : public MetaDouble
{
public:

   IIOutputRangeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIOutputRangeLow* TheIIOutputRangeLowParameter;

// ----------------------------------------------------------------------------

class IIOutputRangeHigh : public MetaDouble
{
public:

   IIOutputRangeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIOutputRangeHigh* TheIIOutputRangeHighParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedLowRK : public MetaUInt64
{
public:

   IITotalRejectedLowRK( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IITotalRejectedLowRK* TheIITotalRejectedLowRKParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedLowG : public MetaUInt64
{
public:

   IITotalRejectedLowG( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IITotalRejectedLowG* TheIITotalRejectedLowGParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedLowB : public MetaUInt64
{
public:

   IITotalRejectedLowB( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IITotalRejectedLowB* TheIITotalRejectedLowBParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedHighRK : public MetaUInt64
{
public:

   IITotalRejectedHighRK( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IITotalRejectedHighRK* TheIITotalRejectedHighRKParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedHighG : public MetaUInt64
{
public:

   IITotalRejectedHighG( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IITotalRejectedHighG* TheIITotalRejectedHighGParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedHighB : public MetaUInt64
{
public:

   IITotalRejectedHighB( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IITotalRejectedHighB* TheIITotalRejectedHighBParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseEstimateRK : public MetaDouble
{
public:

   IIFinalNoiseEstimateRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseEstimateRK* TheIIFinalNoiseEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseEstimateG : public MetaDouble
{
public:

   IIFinalNoiseEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseEstimateG* TheIIFinalNoiseEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseEstimateB : public MetaDouble
{
public:

   IIFinalNoiseEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseEstimateB* TheIIFinalNoiseEstimateBParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseScaleEstimateLowRK : public MetaDouble
{
public:

   IIFinalNoiseScaleEstimateLowRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseScaleEstimateLowRK* TheIIFinalNoiseScaleEstimateLowRKParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseScaleEstimateLowG : public MetaDouble
{
public:

   IIFinalNoiseScaleEstimateLowG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseScaleEstimateLowG* TheIIFinalNoiseScaleEstimateLowGParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseScaleEstimateLowB : public MetaDouble
{
public:

   IIFinalNoiseScaleEstimateLowB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseScaleEstimateLowB* TheIIFinalNoiseScaleEstimateLowBParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseScaleEstimateHighRK : public MetaDouble
{
public:

   IIFinalNoiseScaleEstimateHighRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseScaleEstimateHighRK* TheIIFinalNoiseScaleEstimateHighRKParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseScaleEstimateHighG : public MetaDouble
{
public:

   IIFinalNoiseScaleEstimateHighG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseScaleEstimateHighG* TheIIFinalNoiseScaleEstimateHighGParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseScaleEstimateHighB : public MetaDouble
{
public:

   IIFinalNoiseScaleEstimateHighB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseScaleEstimateHighB* TheIIFinalNoiseScaleEstimateHighBParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseAlgorithmRK : public MetaString
{
public:

   IIFinalNoiseAlgorithmRK( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseAlgorithmRK* TheIIFinalNoiseAlgorithmRKParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseAlgorithmG : public MetaString
{
public:

   IIFinalNoiseAlgorithmG( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseAlgorithmG* TheIIFinalNoiseAlgorithmGParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseAlgorithmB : public MetaString
{
public:

   IIFinalNoiseAlgorithmB( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIFinalNoiseAlgorithmB* TheIIFinalNoiseAlgorithmBParameter;

// ----------------------------------------------------------------------------

class IIFinalScaleEstimateRK : public MetaDouble
{
public:

   IIFinalScaleEstimateRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalScaleEstimateRK* TheIIFinalScaleEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalScaleEstimateG : public MetaDouble
{
public:

   IIFinalScaleEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalScaleEstimateG* TheIIFinalScaleEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalScaleEstimateB : public MetaDouble
{
public:

   IIFinalScaleEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalScaleEstimateB* TheIIFinalScaleEstimateBParameter;

// ----------------------------------------------------------------------------

class IIFinalLocationEstimateRK : public MetaDouble
{
public:

   IIFinalLocationEstimateRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalLocationEstimateRK* TheIIFinalLocationEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalLocationEstimateG : public MetaDouble
{
public:

   IIFinalLocationEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalLocationEstimateG* TheIIFinalLocationEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalLocationEstimateB : public MetaDouble
{
public:

   IIFinalLocationEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalLocationEstimateB* TheIIFinalLocationEstimateBParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalFluxEstimateRK : public MetaDouble
{
public:

   IIFinalPSFTotalFluxEstimateRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalFluxEstimateRK* TheIIFinalPSFTotalFluxEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalFluxEstimateG : public MetaDouble
{
public:

   IIFinalPSFTotalFluxEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalFluxEstimateG* TheIIFinalPSFTotalFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalFluxEstimateB : public MetaDouble
{
public:

   IIFinalPSFTotalFluxEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalFluxEstimateB* TheIIFinalPSFTotalFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalPowerFluxEstimateRK : public MetaDouble
{
public:

   IIFinalPSFTotalPowerFluxEstimateRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalPowerFluxEstimateRK* TheIIFinalPSFTotalPowerFluxEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalPowerFluxEstimateG : public MetaDouble
{
public:

   IIFinalPSFTotalPowerFluxEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalPowerFluxEstimateG* TheIIFinalPSFTotalPowerFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalPowerFluxEstimateB : public MetaDouble
{
public:

   IIFinalPSFTotalPowerFluxEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalPowerFluxEstimateB* TheIIFinalPSFTotalPowerFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalMeanFluxEstimateRK : public MetaDouble
{
public:

   IIFinalPSFTotalMeanFluxEstimateRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalMeanFluxEstimateRK* TheIIFinalPSFTotalMeanFluxEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalMeanFluxEstimateG : public MetaDouble
{
public:

   IIFinalPSFTotalMeanFluxEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalMeanFluxEstimateG* TheIIFinalPSFTotalMeanFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalMeanFluxEstimateB : public MetaDouble
{
public:

   IIFinalPSFTotalMeanFluxEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalMeanFluxEstimateB* TheIIFinalPSFTotalMeanFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalMeanPowerFluxEstimateRK : public MetaDouble
{
public:

   IIFinalPSFTotalMeanPowerFluxEstimateRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalMeanPowerFluxEstimateRK* TheIIFinalPSFTotalMeanPowerFluxEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalMeanPowerFluxEstimateG : public MetaDouble
{
public:

   IIFinalPSFTotalMeanPowerFluxEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalMeanPowerFluxEstimateG* TheIIFinalPSFTotalMeanPowerFluxEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFTotalMeanPowerFluxEstimateB : public MetaDouble
{
public:

   IIFinalPSFTotalMeanPowerFluxEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFTotalMeanPowerFluxEstimateB* TheIIFinalPSFTotalMeanPowerFluxEstimateBParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFMStarEstimateRK : public MetaDouble
{
public:

   IIFinalPSFMStarEstimateRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFMStarEstimateRK* TheIIFinalPSFMStarEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFMStarEstimateG : public MetaDouble
{
public:

   IIFinalPSFMStarEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFMStarEstimateG* TheIIFinalPSFMStarEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFMStarEstimateB : public MetaDouble
{
public:

   IIFinalPSFMStarEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFMStarEstimateB* TheIIFinalPSFMStarEstimateBParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFNStarEstimateRK : public MetaDouble
{
public:

   IIFinalPSFNStarEstimateRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFNStarEstimateRK* TheIIFinalPSFNStarEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFNStarEstimateG : public MetaDouble
{
public:

   IIFinalPSFNStarEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFNStarEstimateG* TheIIFinalPSFNStarEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFNStarEstimateB : public MetaDouble
{
public:

   IIFinalPSFNStarEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFNStarEstimateB* TheIIFinalPSFNStarEstimateBParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFCountRK : public MetaUInt32
{
public:

   IIFinalPSFCountRK( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFCountRK* TheIIFinalPSFCountRKParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFCountG : public MetaUInt32
{
public:

   IIFinalPSFCountG( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFCountG* TheIIFinalPSFCountGParameter;

// ----------------------------------------------------------------------------

class IIFinalPSFCountB : public MetaUInt32
{
public:

   IIFinalPSFCountB( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIFinalPSFCountB* TheIIFinalPSFCountBParameter;

// ----------------------------------------------------------------------------

class IIReferenceNoiseReductionRK : public MetaFloat
{
public:

   IIReferenceNoiseReductionRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIReferenceNoiseReductionRK* TheIIReferenceNoiseReductionRKParameter;

// ----------------------------------------------------------------------------

class IIReferenceNoiseReductionG : public MetaFloat
{
public:

   IIReferenceNoiseReductionG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIReferenceNoiseReductionG* TheIIReferenceNoiseReductionGParameter;

// ----------------------------------------------------------------------------

class IIReferenceNoiseReductionB : public MetaFloat
{
public:

   IIReferenceNoiseReductionB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIReferenceNoiseReductionB* TheIIReferenceNoiseReductionBParameter;

// ----------------------------------------------------------------------------

class IIMedianNoiseReductionRK : public MetaFloat
{
public:

   IIMedianNoiseReductionRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIMedianNoiseReductionRK* TheIIMedianNoiseReductionRKParameter;

// ----------------------------------------------------------------------------

class IIMedianNoiseReductionG : public MetaFloat
{
public:

   IIMedianNoiseReductionG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIMedianNoiseReductionG* TheIIMedianNoiseReductionGParameter;

// ----------------------------------------------------------------------------

class IIMedianNoiseReductionB : public MetaFloat
{
public:

   IIMedianNoiseReductionB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIMedianNoiseReductionB* TheIIMedianNoiseReductionBParameter;

// ----------------------------------------------------------------------------

class IIReferenceSNRIncrementRK : public MetaFloat
{
public:

   IIReferenceSNRIncrementRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIReferenceSNRIncrementRK* TheIIReferenceSNRIncrementRKParameter;

// ----------------------------------------------------------------------------

class IIReferenceSNRIncrementG : public MetaFloat
{
public:

   IIReferenceSNRIncrementG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIReferenceSNRIncrementG* TheIIReferenceSNRIncrementGParameter;

// ----------------------------------------------------------------------------

class IIReferenceSNRIncrementB : public MetaFloat
{
public:

   IIReferenceSNRIncrementB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIReferenceSNRIncrementB* TheIIReferenceSNRIncrementBParameter;

// ----------------------------------------------------------------------------

class IIAverageSNRIncrementRK : public MetaFloat
{
public:

   IIAverageSNRIncrementRK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIAverageSNRIncrementRK* TheIIAverageSNRIncrementRKParameter;

// ----------------------------------------------------------------------------

class IIAverageSNRIncrementG : public MetaFloat
{
public:

   IIAverageSNRIncrementG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIAverageSNRIncrementG* TheIIAverageSNRIncrementGParameter;

// ----------------------------------------------------------------------------

class IIAverageSNRIncrementB : public MetaFloat
{
public:

   IIAverageSNRIncrementB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIAverageSNRIncrementB* TheIIAverageSNRIncrementBParameter;

// ----------------------------------------------------------------------------

class IIImageData : public MetaTable
{
public:

   IIImageData( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIImageData* TheIIImageDataParameter;

// ----------------------------------------------------------------------------

class IIImageWeightRK : public MetaFloat
{
public:

   IIImageWeightRK( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIImageWeightRK* TheIIImageWeightRKParameter;

// ----------------------------------------------------------------------------

class IIImageWeightG : public MetaFloat
{
public:

   IIImageWeightG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIImageWeightG* TheIIImageWeightGParameter;

// ----------------------------------------------------------------------------

class IIImageWeightB : public MetaFloat
{
public:

   IIImageWeightB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern IIImageWeightB* TheIIImageWeightBParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedLowRK : public MetaUInt64
{
public:

   IIImageRejectedLowRK( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIImageRejectedLowRK* TheIIImageRejectedLowRKParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedLowG : public MetaUInt64
{
public:

   IIImageRejectedLowG( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIImageRejectedLowG* TheIIImageRejectedLowGParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedLowB : public MetaUInt64
{
public:

   IIImageRejectedLowB( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIImageRejectedLowB* TheIIImageRejectedLowBParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedHighRK : public MetaUInt64
{
public:

   IIImageRejectedHighRK( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIImageRejectedHighRK* TheIIImageRejectedHighRKParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedHighG : public MetaUInt64
{
public:

   IIImageRejectedHighG( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIImageRejectedHighG* TheIIImageRejectedHighGParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedHighB : public MetaUInt64
{
public:

   IIImageRejectedHighB( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IIImageRejectedHighB* TheIIImageRejectedHighBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ImageIntegrationParameters_h

// ----------------------------------------------------------------------------
// EOF ImageIntegrationParameters.h - Released 2023-08-28T15:23:41Z
