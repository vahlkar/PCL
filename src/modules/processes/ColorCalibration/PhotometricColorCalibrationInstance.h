//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.35
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.0
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationInstance.h - Released 2022-11-21T14:47:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2022 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PhotometricColorCalibrationInstance_h
#define __PhotometricColorCalibrationInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PhotometricColorCalibrationInstance : public ProcessImplementation
{
public:

   PhotometricColorCalibrationInstance( const MetaProcess* );
   PhotometricColorCalibrationInstance( const PhotometricColorCalibrationInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   pcl_bool p_applyCalibration;

   /*
    * White reference parameters
    */
   String   p_whiteReferenceId;
   String   p_whiteReferenceName; // informative only
   float    p_whiteReferenceSr_JV;
   float    p_whiteReferenceJB_JV;
   float    p_zeroPointSr_JV;
   float    p_zeroPointJB_JV;

   /*
    * Catalog search parameters
    */
   pcl_bool p_autoCatalog;
   String   p_catalogId;
   float    p_limitMagnitude;
   pcl_bool p_autoLimitMagnitude;
   int32    p_targetSourceCount;
   String   p_serverURL; // VizieR server for online APASS catalog access.

   /*
    * Signal evaluation parameters
    */
   int32    p_structureLayers;
   float    p_saturationThreshold;
   pcl_bool p_saturationRelative;
   int32    p_psfNoiseLayers;
   int32    p_psfHotPixelFilterRadius;
   int32    p_psfNoiseReductionFilterRadius;
   int32    p_psfMinStructureSize;
   float    p_psfMinSNR;
   pcl_bool p_psfAllowClusteredSources;
   pcl_enum p_psfType;
   float    p_psfGrowth;
   int32    p_psfMaxStars;
   float    p_psfSearchTolerance;
   float    p_psfChannelSearchTolerance;

   /*
    * Background neutralization parameters.
    */
   pcl_bool p_neutralizeBackground;
   String   p_backgroundReferenceViewId; // if empty, use $T
   float    p_backgroundLow;
   float    p_backgroundHigh;
   pcl_bool p_backgroundUseROI; // use a region of interest; entire image otherwise
   Rect     p_backgroundROI = 0;

   /*
    * Generation of control data
    */
   pcl_bool p_generateGraphs;
   pcl_bool p_generateStarMaps;
   pcl_bool p_generateTextFiles;
   String   p_outputDirectory;

   friend class PhotometricColorCalibrationInterface;
   friend class PhotometricColorCalibrationProcess;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PhotometricColorCalibrationInstance_h

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationInstance.h - Released 2022-11-21T14:47:17Z
