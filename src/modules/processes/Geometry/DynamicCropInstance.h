//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.6
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// DynamicCropInstance.h - Released 2024-01-19T15:23:39Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#ifndef __DynamicCropInstance_h
#define __DynamicCropInstance_h

#include <pcl/Point.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/Rectangle.h>

#include "DynamicCropParameters.h"

namespace pcl
{

class PixelInterpolation;

// ----------------------------------------------------------------------------

class DynamicCropInstance : public ProcessImplementation
{
public:

   DynamicCropInstance( const MetaProcess* );
   DynamicCropInstance( const DynamicCropInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool BeforeExecution( View& ) override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

private:

   int32    p_refWidth, p_refHeight; // target image dimensions (px)
   double   p_outWidth, p_outHeight; // output image dimensions (px)
   DPoint   p_center;                // center of cropping rectangle
   double   p_width, p_height;       // normalized dimensions of cropping area ### DEPRECATED
   double   p_angle;                 // rotation angle, radians
   double   p_scaleX, p_scaleY;      // scaling factors
   pcl_bool p_optimizeFast;          // optimize fast rotations: +/-90d, 180d
   pcl_enum p_interpolation;         // interpolation algorithm
   float    p_clampingThreshold;     // deringing clamping for bicubic spline and Lanczos
   float    p_smoothness;            // smoothness for cubic filter interpolations
   pcl_bool p_gammaCorrection;       // interpolate with gamma correction
   DPoint   p_resolution;            // horizontal and vertical resolution in pixels
   pcl_bool p_metric;                // metric resolution?
   pcl_bool p_forceResolution;       // set resolution of target image window?
   DVector  p_fillColor;             // filling values for unused areas (R/K,G,B,A)
   pcl_bool p_noGUIMessages;         // only show warning messages on the console

   RGBColorSystem m_rgbws;

   void EnsureCompatibleWithV1Instance();

   friend class DynamicCropEngine;
   friend class DynamicCropInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DynamicCropInstance_h

// ----------------------------------------------------------------------------
// EOF DynamicCropInstance.h - Released 2024-01-19T15:23:39Z
