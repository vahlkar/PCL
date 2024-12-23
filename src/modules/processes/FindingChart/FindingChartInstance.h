//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard FindingChart Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// FindingChartInstance.h - Released 2024-12-23T11:33:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard FindingChart PixInsight module.
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

#ifndef __FindingChartInstance_h
#define __FindingChartInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FindingChartInstance : public ProcessImplementation
{
public:

   FindingChartInstance( const MetaProcess* );
   FindingChartInstance( const FindingChartInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsHistoryUpdater( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   int32    p_chartSize;         // px, >= 512
   pcl_bool p_autoChartResolution;
   float    p_chartResolution;   // as, ignored if p_autoChartResolution
   pcl_bool p_autoChartMaxMagnitude;
   float    p_chartMaxMagnitude; // mag, ignored if p_autoChartMaxMagnitude
   pcl_bool p_drawGrid;
   pcl_bool p_drawEcliptic;
   pcl_bool p_drawGalacticEquator;
   pcl_bool p_drawConstellationBorders;
   pcl_bool p_drawConstellationLines;
   pcl_bool p_drawConstellationNames;
   pcl_bool p_drawStars;
   pcl_bool p_drawStarNames;
   pcl_bool p_drawMessierObjects;
   pcl_bool p_drawNGCObjects;
   pcl_bool p_generateBitmapFile;
   String   p_outputDirectory;
   String   p_outputSuffix;
   pcl_bool p_closeOutputWindow; // ignored if !p_generateBitmapFile
   uint32   p_imageRegionColor;
   uint32   p_imageRegionBorderColor;
   uint32   p_gridColor;
   uint32   p_gridTextColor;
   uint32   p_eclipticColor;
   uint32   p_eclipticTextColor;
   uint32   p_galacticEquatorColor;
   uint32   p_galacticEquatorTextColor;
   uint32   p_constellationBorderColor;
   uint32   p_constellationLineColor;
   uint32   p_constellationTextColor;
   uint32   p_starTextColor;
   uint32   p_messierTextColor;
   uint32   p_ngcTextColor;

   friend class FindingChartEngine;
   friend class FindingChartProcess;
   friend class FindingChartInterface;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __FindingChartInstance_h

// ----------------------------------------------------------------------------
// EOF FindingChartInstance.h - Released 2024-12-23T11:33:28Z
