//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// DynamicCropParameters.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#ifndef __DynamicCropParameters_h
#define __DynamicCropParameters_h

#include "CommonParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class DCCenterX : public MetaDouble
{
public:

   DCCenterX( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCCenterX* TheDCCenterXParameter;

// ----------------------------------------------------------------------------

class DCCenterY : public MetaDouble
{
public:

   DCCenterY( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCCenterY* TheDCCenterYParameter;

// ----------------------------------------------------------------------------

class DCWidth : public MetaDouble
{
public:

   DCWidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCWidth* TheDCWidthParameter;

// ----------------------------------------------------------------------------

class DCHeight : public MetaDouble
{
public:

   DCHeight( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCHeight* TheDCHeightParameter;

// ----------------------------------------------------------------------------

extern RotationAngle* TheDCRotationAngleParameter;

// ----------------------------------------------------------------------------

class DCScaleX : public MetaDouble
{
public:

   DCScaleX( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCScaleX* TheDCScaleXParameter;

// ----------------------------------------------------------------------------

class DCScaleY : public MetaDouble
{
public:

   DCScaleY( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCScaleY* TheDCScaleYParameter;

// ----------------------------------------------------------------------------

extern OptimizeFastRotations* TheDCOptimizeFastRotationsParameter;

extern InterpolationAlgorithm* TheDCInterpolationAlgorithmParameter;
extern ClampingThreshold* TheDCClampingThresholdParameter;
extern Smoothness* TheDCSmoothnessParameter;

extern XResolution* TheDCXResolutionParameter;
extern YResolution* TheDCYResolutionParameter;
extern MetricResolution* TheDCMetricResolutionParameter;
extern ForceResolution* TheDCForceResolutionParameter;

extern FillRed* TheDCFillRedParameter;
extern FillGreen* TheDCFillGreenParameter;
extern FillBlue* TheDCFillBlueParameter;
extern FillAlpha* TheDCFillAlphaParameter;

extern NoGUIMessages* TheDCNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __DynamicCropParameters_h

// ----------------------------------------------------------------------------
// EOF DynamicCropParameters.h - Released 2021-04-09T19:41:48Z
