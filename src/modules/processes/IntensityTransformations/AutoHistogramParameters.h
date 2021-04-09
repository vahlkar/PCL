//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.1
// ----------------------------------------------------------------------------
// AutoHistogramParameters.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#ifndef __AutoHistogramParameters_h
#define __AutoHistogramParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class AHClip : public MetaBoolean
{
public:

   AHClip( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern AHClip* TheAHClipParameter;

// ----------------------------------------------------------------------------

class AHClipTogether : public MetaBoolean
{
public:

   AHClipTogether( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern AHClipTogether* TheAHClipTogetherParameter;

// ----------------------------------------------------------------------------

class AHClipLowR : public MetaFloat
{
public:

   AHClipLowR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHClipLowR* TheAHClipLowRParameter;

// ----------------------------------------------------------------------------

class AHClipLowG : public MetaFloat
{
public:

   AHClipLowG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHClipLowG* TheAHClipLowGParameter;

// ----------------------------------------------------------------------------

class AHClipLowB : public MetaFloat
{
public:

   AHClipLowB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHClipLowB* TheAHClipLowBParameter;

// ----------------------------------------------------------------------------

class AHClipHighR : public MetaFloat
{
public:

   AHClipHighR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

// ----------------------------------------------------------------------------

extern AHClipHighR* TheAHClipHighRParameter;

class AHClipHighG : public MetaFloat
{
public:

   AHClipHighG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHClipHighG* TheAHClipHighGParameter;

// ----------------------------------------------------------------------------

class AHClipHighB : public MetaFloat
{
public:

   AHClipHighB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHClipHighB* TheAHClipHighBParameter;

// ----------------------------------------------------------------------------

class AHStretch : public MetaBoolean
{
public:

   AHStretch( MetaProcess* );
   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern AHStretch* TheAHStretchParameter;

// ----------------------------------------------------------------------------

class AHStretchTogether : public MetaBoolean
{
public:

   AHStretchTogether( MetaProcess* );
   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern AHStretchTogether* TheAHStretchTogetherParameter;

// ----------------------------------------------------------------------------

class AHStretchMethod : public MetaEnumeration
{
public:

   enum { Gamma,
          Logarithm,
          MTF,
          NumberOfItems,
          Default = Gamma };

   AHStretchMethod( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern AHStretchMethod* TheAHStretchMethodParameter;

// ----------------------------------------------------------------------------

class AHTargetMedianR : public MetaDouble
{
public:

   AHTargetMedianR( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHTargetMedianR* TheAHTargetMedianRParameter;

// ----------------------------------------------------------------------------

class AHTargetMedianG : public MetaDouble
{
public:

   AHTargetMedianG( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHTargetMedianG* TheAHTargetMedianGParameter;

// ----------------------------------------------------------------------------

class AHTargetMedianB : public MetaDouble
{
public:

   AHTargetMedianB( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHTargetMedianB* TheAHTargetMedianBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __AutoHistogramParameters_h

// ----------------------------------------------------------------------------
// EOF AutoHistogramParameters.h - Released 2021-04-09T19:41:48Z
