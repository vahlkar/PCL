//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.1
// ----------------------------------------------------------------------------
// MaskedStretchParameters.h - Released 2021-04-09T19:41:48Z
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

#ifndef __MaskedStretchParameters_h
#define __MaskedStretchParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class MSTargetBackground : public MetaDouble
{
public:

   MSTargetBackground( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern MSTargetBackground* TheMSTargetBackgroundParameter;

// ----------------------------------------------------------------------------

class MSNumberOfIterations : public MetaInt32
{
public:

   MSNumberOfIterations( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MSNumberOfIterations* TheMSNumberOfIterationsParameter;

// ----------------------------------------------------------------------------

class MSClippingFraction : public MetaDouble
{
public:

   MSClippingFraction( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern MSClippingFraction* TheMSClippingFractionParameter;

// ----------------------------------------------------------------------------

class MSBackgroundReferenceViewId : public MetaString
{
public:

   MSBackgroundReferenceViewId( MetaProcess* );

   IsoString Id() const override;
};

extern MSBackgroundReferenceViewId* TheMSBackgroundReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class MSBackgroundLow : public MetaDouble
{
public:

   MSBackgroundLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern MSBackgroundLow* TheMSBackgroundLowParameter;

// ----------------------------------------------------------------------------

class MSBackgroundHigh : public MetaDouble
{
public:

   MSBackgroundHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern MSBackgroundHigh* TheMSBackgroundHighParameter;

// ----------------------------------------------------------------------------

class MSUseROI : public MetaBoolean
{
public:

   MSUseROI( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern MSUseROI* TheMSUseROIParameter;

// ----------------------------------------------------------------------------

class MSROIX0 : public MetaInt32
{
public:

   MSROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MSROIX0* TheMSROIX0Parameter;

// ----------------------------------------------------------------------------

class MSROIY0 : public MetaInt32
{
public:

   MSROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MSROIY0* TheMSROIY0Parameter;

// ----------------------------------------------------------------------------

class MSROIX1 : public MetaInt32
{
public:

   MSROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MSROIX1* TheMSROIX1Parameter;

// ----------------------------------------------------------------------------

class MSROIY1 : public MetaInt32
{
public:

   MSROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MSROIY1* TheMSROIY1Parameter;

// ----------------------------------------------------------------------------

class MSMaskType : public MetaEnumeration
{
public:

   enum { Intensity,
          Value,
          NumberOfItems,
          Default = Intensity };

   MSMaskType( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern MSMaskType* TheMSMaskTypeParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __MaskedStretchParameters_h

// ----------------------------------------------------------------------------
// EOF MaskedStretchParameters.h - Released 2021-04-09T19:41:48Z
