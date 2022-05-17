//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.29
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.1
// ----------------------------------------------------------------------------
// AutoHistogramInstance.h - Released 2022-05-17T17:15:11Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#ifndef __AutoHistogramInstance_h
#define __AutoHistogramInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // pcl_bool

namespace pcl
{

// ----------------------------------------------------------------------------

class AutoHistogramInstance : public ProcessImplementation
{
public:

   AutoHistogramInstance( const MetaProcess* );
   AutoHistogramInstance( const AutoHistogramInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

private:

   // Channel indexes: 0=R/K, 1=G, 2=B

   pcl_bool p_clip;              // perform histogram clipping ?
   pcl_bool p_clipTogether;      // GUI only: clip joint RGB channels ?
   float    p_clipLow[ 3 ];      // clipped pixels, shadows, percentage of total
   float    p_clipHigh[ 3 ];     // clipped pixels, highlights, percentage of total

   pcl_bool p_stretch;           // perform nonlinear stretch ?
   pcl_bool p_stretchTogether;   // GUI only: stretch joint RGB channels ?
   pcl_enum p_stretchMethod;     // nonlinear stretch method
   double   p_targetMedian[ 3 ]; // target median values in [0,1]

   friend class AutoHistogramEngine;
   friend class AutoHistogramProcess;
   friend class AutoHistogramInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AutoHistogramInstance_h

// ----------------------------------------------------------------------------
// EOF AutoHistogramInstance.h - Released 2022-05-17T17:15:11Z
