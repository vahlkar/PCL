//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// LarsonSekaninaParameters.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
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

#ifndef __LarsonSekaninaParameters_h
#define __LarsonSekaninaParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class LSInterpolation : public MetaEnumeration
{
public:

   enum { Bilinear,
          Bicubic,
          BicubicSpline,
          BicubicBSpline,
          NumberOfInterpolationModes,
          Default = Bicubic };

   LSInterpolation( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern LSInterpolation* TheLSInterpolationParameter;

// ----------------------------------------------------------------------------

class LSRadiusDiff : public MetaFloat
{
public:

   LSRadiusDiff( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSRadiusDiff* TheLSRadiusDiffParameter;

// ----------------------------------------------------------------------------

class LSAngleDiff : public MetaFloat
{
public:

   LSAngleDiff( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSAngleDiff* TheLSAngleDiffParameter;

// ----------------------------------------------------------------------------

class LSCenterX : public MetaFloat
{
public:

   LSCenterX( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSCenterX* TheLSCenterXParameter;

// ----------------------------------------------------------------------------

class LSCenterY : public MetaFloat
{
public:

   LSCenterY( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSCenterY* TheLSCenterYParameter;

// ----------------------------------------------------------------------------

class LSAmount : public MetaFloat
{
public:

   LSAmount( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSAmount* TheLSAmountParameter;

// ----------------------------------------------------------------------------

class LSThreshold : public MetaFloat
{
public:

   LSThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSThreshold* TheLSThresholdParameter;

// ----------------------------------------------------------------------------

class LSDeringing : public MetaFloat
{
public:

   LSDeringing( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSDeringing* TheLSDeringingParameter;

// ----------------------------------------------------------------------------

class LSRangeLow : public MetaFloat
{
public:

   LSRangeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSRangeLow* TheLSRangeLowParameter;

// ----------------------------------------------------------------------------

class LSRangeHigh : public MetaFloat
{
public:

   LSRangeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSRangeHigh* TheLSRangeHighParameter;

// ----------------------------------------------------------------------------

class LSUseLuminance : public MetaBoolean
{
public:

   LSUseLuminance( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LSUseLuminance* TheLSUseLuminanceParameter;

// ----------------------------------------------------------------------------

class LSHighPass : public MetaBoolean
{
public:

   LSHighPass( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LSHighPass* TheLSHighPassParameter;

// ----------------------------------------------------------------------------

class LSDisableExtension : public MetaBoolean
{
public:

   LSDisableExtension( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LSDisableExtension* TheLSDisableExtensionParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ShapernParameters_h

// ----------------------------------------------------------------------------
// EOF LarsonSekaninaParameters.h - Released 2021-04-09T19:41:48Z
