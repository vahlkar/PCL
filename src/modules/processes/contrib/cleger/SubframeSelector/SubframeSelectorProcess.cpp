//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.8.8
// ----------------------------------------------------------------------------
// SubframeSelectorProcess.cpp - Released 2023-08-28T15:23:41Z
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
#include "SubframeSelectorInterface.h"
#include "SubframeSelectorParameters.h"
#include "SubframeSelectorProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorProcess* TheSubframeSelectorProcess = nullptr;

// ----------------------------------------------------------------------------

SubframeSelectorProcess::SubframeSelectorProcess()
{
   TheSubframeSelectorProcess = this;

   new SSRoutine( this );
   new SSNonInteractive( this );

   new SSSubframes( this );
   new SSSubframeEnabled( TheSSSubframesParameter );
   new SSSubframePath( TheSSSubframesParameter );
   new SSLocalNormalizationDataPath( TheSSSubframesParameter );
   new SSDrizzlePath( TheSSSubframesParameter );

   new SSFileCache( this );

   new SSSubframeScale( this );
   new SSCameraGain( this );
   new SSCameraResolution( this );
   new SSSiteLocalMidnight( this );
   new SSScaleUnit( this );
   new SSDataUnit( this );
   new SSTrimmingFactor( this );

   new SSStructureLayers( this );
   new SSNoiseLayers( this );
   new SSHotPixelFilterRadius( this );
   new SSNoiseReductionFilterRadius( this );
   new SSMinStructureSize( this );
   new SSSensitivity( this );
   new SSPeakResponse( this );
   new SSBrightThreshold( this );
   new SSMaxDistortion( this );
   new SSAllowClusteredSources( this );
   new SSUpperLimit( this );
   new SSPSFFit( this );
   new SSPSFFitCircular( this );
   new SSMaxPSFFits( this );
   new SSROIX0( this );
   new SSROIY0( this );
   new SSROIX1( this );
   new SSROIY1( this );

   new SSPedestalMode( this );
   new SSPedestal( this );
   new SSPedestalKeyword( this );

   new SSInputHints( this );
   new SSOutputHints( this );

   new SSOutputDirectory( this );
   new SSOutputExtension( this );
   new SSOutputPrefix( this );
   new SSOutputPostfix( this );
   new SSOutputKeyword( this );
   new SSOverwriteExistingFiles( this );
   new SSOnError( this );

   new SSApprovalExpression( this );
   new SSWeightingExpression( this );

   new SSSortingProperty( this );
   new SSGraphProperty( this );
   new SSAuxGraphProperty( this );

   new SSUseFileThreads( this );
   new SSFileThreadOverload( this );
   new SSMaxFileReadThreads( this );
   new SSMaxFileWriteThreads( this );

   /*
    * ### N.B. Please do not change the order of instantiation of the following
    * table row parameters, as doing so will break compatibility with existing
    * scripts that depend on the SubframeSelector process.
    */

   new SSMeasurements( this );
   new SSMeasurementIndex( TheSSMeasurementsParameter );
   new SSMeasurementEnabled( TheSSMeasurementsParameter );
   new SSMeasurementLocked( TheSSMeasurementsParameter );
   new SSMeasurementPath( TheSSMeasurementsParameter );
   new SSMeasurementWeight( TheSSMeasurementsParameter );
   new SSMeasurementFWHM( TheSSMeasurementsParameter );
   new SSMeasurementEccentricity( TheSSMeasurementsParameter );
   new SSMeasurementPSFSignalWeight( TheSSMeasurementsParameter );
   new SSMeasurementUnused01( TheSSMeasurementsParameter ); // ### retained for compatibility
   new SSMeasurementSNRWeight( TheSSMeasurementsParameter );
   new SSMeasurementMedian( TheSSMeasurementsParameter );
   new SSMeasurementMedianMeanDev( TheSSMeasurementsParameter );
   new SSMeasurementNoise( TheSSMeasurementsParameter );
   new SSMeasurementNoiseRatio( TheSSMeasurementsParameter );
   new SSMeasurementStars( TheSSMeasurementsParameter );
   new SSMeasurementStarResidual( TheSSMeasurementsParameter );
   new SSMeasurementFWHMMeanDev( TheSSMeasurementsParameter );
   new SSMeasurementEccentricityMeanDev( TheSSMeasurementsParameter );
   new SSMeasurementStarResidualMeanDev( TheSSMeasurementsParameter );
   new SSMeasurementAzimuth( TheSSMeasurementsParameter );
   new SSMeasurementAltitude( TheSSMeasurementsParameter );
   new SSMeasurementPSFFlux( TheSSMeasurementsParameter );
   new SSMeasurementPSFFluxPower( TheSSMeasurementsParameter );
   new SSMeasurementPSFTotalMeanFlux( TheSSMeasurementsParameter );
   new SSMeasurementPSFTotalMeanPowerFlux( TheSSMeasurementsParameter );
   new SSMeasurementPSFCount( TheSSMeasurementsParameter );
   new SSMeasurementMStar( TheSSMeasurementsParameter );
   new SSMeasurementNStar( TheSSMeasurementsParameter );
   new SSMeasurementPSFSNR( TheSSMeasurementsParameter );
   new SSMeasurementPSFScale( TheSSMeasurementsParameter );
   new SSMeasurementPSFScaleSNR( TheSSMeasurementsParameter );
}

// ----------------------------------------------------------------------------

IsoString SubframeSelectorProcess::Id() const
{
   return "SubframeSelector";
}

// ----------------------------------------------------------------------------

IsoString SubframeSelectorProcess::Category() const
{
   return "ImageInspection,Preprocessing";
}

// ----------------------------------------------------------------------------

uint32 SubframeSelectorProcess::Version() const
{
   return 2;
}

// ----------------------------------------------------------------------------

String SubframeSelectorProcess::Description() const
{
   return
   "<html>"
   "Facilitates subframe evaluation, selection and weighting based on several subframe "
   "quality related measurements, including estimates of star profile <i>full width at "
   "half maximum</i> (FWHM), star profile <i>eccentricity</i> and subframe "
   "<i>signal to noise ratio weight</i>. Approved/rejected subframes may be copied/moved "
   "to output directories for postprocessing. Subframe weights may be recorded in the "
   "FITS header of the copies."
   "</html>";
}

// ----------------------------------------------------------------------------

String SubframeSelectorProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/SubframeSelector.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* SubframeSelectorProcess::DefaultInterface() const
{
   return TheSubframeSelectorInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* SubframeSelectorProcess::Create() const
{
   return new SubframeSelectorInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SubframeSelectorProcess::Clone( const ProcessImplementation& p ) const
{
   const SubframeSelectorInstance* instance = dynamic_cast<const SubframeSelectorInstance*>( &p );
   return (instance != nullptr) ? new SubframeSelectorInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorProcess.cpp - Released 2023-08-28T15:23:41Z
