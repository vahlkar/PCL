//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.5.1
// ----------------------------------------------------------------------------
// BackgroundNeutralizationParameters.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#ifndef __BackgroundNeutralizationParameters_h
#define __BackgroundNeutralizationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class BNBackgroundReferenceViewId : public MetaString
{
public:

   BNBackgroundReferenceViewId( MetaProcess* );

   IsoString Id() const override;
};

extern BNBackgroundReferenceViewId* TheBNBackgroundReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class BNBackgroundLow : public MetaFloat
{
public:

   BNBackgroundLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern BNBackgroundLow* TheBNBackgroundLowParameter;

// ----------------------------------------------------------------------------

class BNBackgroundHigh : public MetaFloat
{
public:

   BNBackgroundHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern BNBackgroundHigh* TheBNBackgroundHighParameter;

// ----------------------------------------------------------------------------

class BNUseROI : public MetaBoolean
{
public:

   BNUseROI( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern BNUseROI* TheBNUseROIParameter;

// ----------------------------------------------------------------------------

class BNROIX0 : public MetaInt32
{
public:

   BNROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BNROIX0* TheBNROIX0Parameter;


// ----------------------------------------------------------------------------

class BNROIY0 : public MetaInt32
{
public:

   BNROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BNROIY0* TheBNROIY0Parameter;

// ----------------------------------------------------------------------------

class BNROIX1 : public MetaInt32
{
public:

   BNROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BNROIX1* TheBNROIX1Parameter;

// ----------------------------------------------------------------------------

class BNROIY1 : public MetaInt32
{
public:

   BNROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BNROIY1* TheBNROIY1Parameter;

// ----------------------------------------------------------------------------

class BNMode : public MetaEnumeration
{
public:

   enum { TargetBackground,
          Rescale,
          RescaleAsNeeded,
          Truncate,
          NumberOfModes,
          Default = RescaleAsNeeded };

   BNMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern BNMode* TheBNModeParameter;

// ----------------------------------------------------------------------------

class BNTargetBackground : public MetaFloat
{
public:

   BNTargetBackground( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern BNTargetBackground* TheBNTargetBackgroundParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __BackgroundNeutralizationParameters_h

// ----------------------------------------------------------------------------
// EOF BackgroundNeutralizationParameters.h - Released 2021-04-09T19:41:48Z
