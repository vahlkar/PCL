//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// DynamicCropParameters.cpp - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "DynamicCropParameters.h"

#include <pcl/Constants.h>

namespace pcl
{

// ----------------------------------------------------------------------------

DCRefWidth*             TheDCRefWidthParameter = nullptr;
DCRefHeight*            TheDCRefHeightParameter = nullptr;
DCOutWidth*             TheDCOutWidthParameter = nullptr;
DCOutHeight*            TheDCOutHeightParameter = nullptr;
DCCenterX*              TheDCCenterXParameter = nullptr;
DCCenterY*              TheDCCenterYParameter = nullptr;
DCWidth*                TheDCWidthParameter = nullptr;
DCHeight*               TheDCHeightParameter = nullptr;
RotationAngle*          TheDCRotationAngleParameter = nullptr;
DCScaleX*               TheDCScaleXParameter = nullptr;
DCScaleY*               TheDCScaleYParameter = nullptr;
OptimizeFastRotations*  TheDCOptimizeFastRotationsParameter = nullptr;
InterpolationAlgorithm* TheDCInterpolationAlgorithmParameter = nullptr;
ClampingThreshold*      TheDCClampingThresholdParameter = nullptr;
Smoothness*             TheDCSmoothnessParameter = nullptr;
GammaCorrection*        TheDCGammaCorrectionParameter = nullptr;
XResolution*            TheDCXResolutionParameter = nullptr;
YResolution*            TheDCYResolutionParameter = nullptr;
MetricResolution*       TheDCMetricResolutionParameter = nullptr;
ForceResolution*        TheDCForceResolutionParameter = nullptr;
FillRed*                TheDCFillRedParameter = nullptr;
FillGreen*              TheDCFillGreenParameter = nullptr;
FillBlue*               TheDCFillBlueParameter = nullptr;
FillAlpha*              TheDCFillAlphaParameter = nullptr;
NoGUIMessages*          TheDCNoGUIMessagesParameter = nullptr;

// ----------------------------------------------------------------------------

DCRefWidth::DCRefWidth( MetaProcess* P )
   : MetaInt32( P )
{
   TheDCRefWidthParameter = this;
}

IsoString DCRefWidth::Id() const
{
   return "refWidth";
}

double DCRefWidth::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

DCRefHeight::DCRefHeight( MetaProcess* P )
   : MetaInt32( P )
{
   TheDCRefHeightParameter = this;
}

IsoString DCRefHeight::Id() const
{
   return "refHeight";
}

double DCRefHeight::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

DCOutWidth::DCOutWidth( MetaProcess* P )
   : MetaDouble( P )
{
   TheDCOutWidthParameter = this;
}

IsoString DCOutWidth::Id() const
{
   return "outWidth";
}

int DCOutWidth::Precision() const
{
   return 2;
}

double DCOutWidth::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

DCOutHeight::DCOutHeight( MetaProcess* P )
   : MetaDouble( P )
{
   TheDCOutHeightParameter = this;
}

IsoString DCOutHeight::Id() const
{
   return "outHeight";
}

int DCOutHeight::Precision() const
{
   return 2;
}

double DCOutHeight::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

DCCenterX::DCCenterX( MetaProcess* P )
   : MetaDouble( P )
{
   TheDCCenterXParameter = this;
}

IsoString DCCenterX::Id() const
{
   return "centerX";
}

int DCCenterX::Precision() const
{
   return 8;
}

double DCCenterX::DefaultValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

DCCenterY::DCCenterY( MetaProcess* P )
   : MetaDouble( P )
{
   TheDCCenterYParameter = this;
}

IsoString DCCenterY::Id() const
{
   return "centerY";
}

int DCCenterY::Precision() const
{
   return 8;
}

double DCCenterY::DefaultValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

DCWidth::DCWidth( MetaProcess* P )
   : MetaDouble( P )
{
   TheDCWidthParameter = this;
}

IsoString DCWidth::Id() const
{
   return "width";
}

int DCWidth::Precision() const
{
   return 8;
}

double DCWidth::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

DCHeight::DCHeight( MetaProcess* P )
   : MetaDouble( P )
{
   TheDCHeightParameter = this;
}

IsoString DCHeight::Id() const
{
   return "height";
}

int DCHeight::Precision() const
{
   return 8;
}

double DCHeight::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

DCScaleX::DCScaleX( MetaProcess* P )
   : MetaDouble( P )
{
   TheDCScaleXParameter = this;
}

IsoString DCScaleX::Id() const
{
   return "scaleX";
}

int DCScaleX::Precision() const
{
   return 8;
}

double DCScaleX::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

DCScaleY::DCScaleY( MetaProcess* P )
   : MetaDouble( P )
{
   TheDCScaleYParameter = this;
}

IsoString DCScaleY::Id() const
{
   return "scaleY";
}

int DCScaleY::Precision() const
{
   return 8;
}

double DCScaleY::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicCropParameters.cpp - Released 2023-08-28T15:23:41Z
