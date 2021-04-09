//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.5.1
// ----------------------------------------------------------------------------
// ColorCalibrationParameters.h - Released 2021-04-09T19:41:48Z
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

#ifndef __ColorCalibrationParameters_h
#define __ColorCalibrationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CCWhiteReferenceViewId : public MetaString
{
public:

   CCWhiteReferenceViewId( MetaProcess* );

   IsoString Id() const override;
};

extern CCWhiteReferenceViewId* TheCCWhiteReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class CCWhiteLow : public MetaFloat
{
public:

   CCWhiteLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern CCWhiteLow* TheCCWhiteLowParameter;

// ----------------------------------------------------------------------------

class CCWhiteHigh : public MetaFloat
{
public:

   CCWhiteHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern CCWhiteHigh* TheCCWhiteHighParameter;

// ----------------------------------------------------------------------------

class CCWhiteUseROI : public MetaBoolean
{
public:

   CCWhiteUseROI( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CCWhiteUseROI* TheCCWhiteUseROIParameter;

// ----------------------------------------------------------------------------

class CCWhiteROIX0 : public MetaInt32
{
public:

   CCWhiteROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CCWhiteROIX0* TheCCWhiteROIX0Parameter;


// ----------------------------------------------------------------------------

class CCWhiteROIY0 : public MetaInt32
{
public:

   CCWhiteROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CCWhiteROIY0* TheCCWhiteROIY0Parameter;

// ----------------------------------------------------------------------------

class CCWhiteROIX1 : public MetaInt32
{
public:

   CCWhiteROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CCWhiteROIX1* TheCCWhiteROIX1Parameter;

// ----------------------------------------------------------------------------

class CCWhiteROIY1 : public MetaInt32
{
public:

   CCWhiteROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CCWhiteROIY1* TheCCWhiteROIY1Parameter;

// ----------------------------------------------------------------------------

class CCStructureDetection : public MetaBoolean
{
public:

   CCStructureDetection( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CCStructureDetection* TheCCStructureDetectionParameter;

// ----------------------------------------------------------------------------

class CCStructureLayers : public MetaInt32
{
public:

   CCStructureLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CCStructureLayers* TheCCStructureLayersParameter;

// ----------------------------------------------------------------------------

class CCNoiseLayers : public MetaInt32
{
public:

   CCNoiseLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CCNoiseLayers* TheCCNoiseLayersParameter;

// ----------------------------------------------------------------------------

class CCManualWhiteBalance : public MetaBoolean
{
public:

   CCManualWhiteBalance( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CCManualWhiteBalance* TheCCManualWhiteBalanceParameter;

// ----------------------------------------------------------------------------

class CCManualRedFactor : public MetaFloat
{
public:

   CCManualRedFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern CCManualRedFactor* TheCCManualRedFactorParameter;

// ----------------------------------------------------------------------------

class CCManualGreenFactor : public MetaFloat
{
public:

   CCManualGreenFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern CCManualGreenFactor* TheCCManualGreenFactorParameter;

// ----------------------------------------------------------------------------

class CCManualBlueFactor : public MetaFloat
{
public:

   CCManualBlueFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern CCManualBlueFactor* TheCCManualBlueFactorParameter;

// ----------------------------------------------------------------------------

class CCBackgroundReferenceViewId : public MetaString
{
public:

   CCBackgroundReferenceViewId( MetaProcess* );

   IsoString Id() const override;
};

extern CCBackgroundReferenceViewId* TheCCBackgroundReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class CCBackgroundLow : public MetaFloat
{
public:

   CCBackgroundLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern CCBackgroundLow* TheCCBackgroundLowParameter;

// ----------------------------------------------------------------------------

class CCBackgroundHigh : public MetaFloat
{
public:

   CCBackgroundHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern CCBackgroundHigh* TheCCBackgroundHighParameter;

// ----------------------------------------------------------------------------

class CCBackgroundUseROI : public MetaBoolean
{
public:

   CCBackgroundUseROI( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CCBackgroundUseROI* TheCCBackgroundUseROIParameter;

// ----------------------------------------------------------------------------

class CCBackgroundROIX0 : public MetaInt32
{
public:

   CCBackgroundROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CCBackgroundROIX0* TheCCBackgroundROIX0Parameter;


// ----------------------------------------------------------------------------

class CCBackgroundROIY0 : public MetaInt32
{
public:

   CCBackgroundROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CCBackgroundROIY0* TheCCBackgroundROIY0Parameter;

// ----------------------------------------------------------------------------

class CCBackgroundROIX1 : public MetaInt32
{
public:

   CCBackgroundROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CCBackgroundROIX1* TheCCBackgroundROIX1Parameter;

// ----------------------------------------------------------------------------

class CCBackgroundROIY1 : public MetaInt32
{
public:

   CCBackgroundROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CCBackgroundROIY1* TheCCBackgroundROIY1Parameter;

// ----------------------------------------------------------------------------

class CCOutputWhiteReferenceMask : public MetaBoolean
{
public:

   CCOutputWhiteReferenceMask( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CCOutputWhiteReferenceMask* TheCCOutputWhiteReferenceMaskParameter;

// ----------------------------------------------------------------------------

class CCOutputBackgroundReferenceMask : public MetaBoolean
{
public:

   CCOutputBackgroundReferenceMask( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CCOutputBackgroundReferenceMask* TheCCOutputBackgroundReferenceMaskParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ColorCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF ColorCalibrationParameters.h - Released 2021-04-09T19:41:48Z
