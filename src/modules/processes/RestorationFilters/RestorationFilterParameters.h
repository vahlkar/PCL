//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 1.0.5
// ----------------------------------------------------------------------------
// RestorationFilterParameters.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
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

#ifndef __RestorationFilterParameters_h
#define __RestorationFilterParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class RFWienerK : public MetaDouble
{
public:

   RFWienerK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFWienerK* TheRFWienerKParameter;

// ----------------------------------------------------------------------------

class RFLeastSquaresGamma : public MetaDouble
{
public:

   RFLeastSquaresGamma( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFLeastSquaresGamma* TheRFLeastSquaresGammaParameter;

// ----------------------------------------------------------------------------

class RFAlgorithm : public MetaEnumeration
{
public:

   enum { Wiener,
          ConstrainedLeastSquares,
          NumberOfAlgorithms,
          Default = ConstrainedLeastSquares };

   RFAlgorithm( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern RFAlgorithm* TheRFAlgorithmParameter;

// ----------------------------------------------------------------------------

class RFPSFMode : public MetaEnumeration
{
public:

   enum { Parametric,
          MotionBlur,
          External,
          NumberOfModes,
          Default = Parametric };

   RFPSFMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
   IsoString ElementAliases() const override;
};

extern RFPSFMode* TheRFPSFModeParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricSigma : public MetaFloat
{
public:

   RFPSFParametricSigma( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFParametricSigma* TheRFPSFParametricSigmaParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricShape : public MetaFloat
{
public:

   RFPSFParametricShape( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFParametricShape* TheRFPSFParametricShapeParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricAspectRatio : public MetaFloat
{
public:

   RFPSFParametricAspectRatio( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFParametricAspectRatio* TheRFPSFParametricAspectRatioParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricRotationAngle : public MetaFloat
{
public:

   RFPSFParametricRotationAngle( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFParametricRotationAngle* TheRFPSFParametricRotationAngleParameter;

// ----------------------------------------------------------------------------

class RFPSFMotionLength : public MetaFloat
{
public:

   RFPSFMotionLength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFMotionLength* TheRFPSFMotionLengthParameter;

// ----------------------------------------------------------------------------

class RFPSFMotionRotationAngle : public MetaFloat
{
public:

   RFPSFMotionRotationAngle( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFMotionRotationAngle* TheRFPSFMotionRotationAngleParameter;

// ----------------------------------------------------------------------------

class RFPSFViewId : public MetaString
{
public:

   RFPSFViewId( MetaProcess* );

   IsoString Id() const override;
};

extern RFPSFViewId* TheRFPSFViewIdParameter;

// ----------------------------------------------------------------------------

class RFAmount : public MetaFloat
{
public:

   RFAmount( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFAmount* TheRFAmountParameter;

// ----------------------------------------------------------------------------

class RFToLuminance : public MetaBoolean
{
public:

   RFToLuminance( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern RFToLuminance* TheRFToLuminanceParameter;

// ----------------------------------------------------------------------------

class RFLinear : public MetaBoolean
{
public:

   RFLinear( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern RFLinear* TheRFLinearParameter;

// ----------------------------------------------------------------------------

class RFDeringing : public MetaBoolean
{
public:

   RFDeringing( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern RFDeringing* TheRFDeringingParameter;

// ----------------------------------------------------------------------------

class RFDeringingDark : public MetaFloat
{
public:

   RFDeringingDark( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern RFDeringingDark* TheRFDeringingDarkParameter;

// ----------------------------------------------------------------------------

class RFDeringingBright : public MetaFloat
{
public:

   RFDeringingBright( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern RFDeringingBright* TheRFDeringingBrightParameter;

// ----------------------------------------------------------------------------

class RFOutputDeringingMaps : public MetaBoolean
{
public:

   RFOutputDeringingMaps( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern RFOutputDeringingMaps* TheRFOutputDeringingMapsParameter;

// ----------------------------------------------------------------------------

class RFRangeLow : public MetaFloat
{
public:

   RFRangeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFRangeLow* TheRFRangeLowParameter;

// ----------------------------------------------------------------------------

class RFRangeHigh : public MetaFloat
{
public:

   RFRangeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFRangeHigh* TheRFRangeHighParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __RestorationFilterParameters_h

// ----------------------------------------------------------------------------
// EOF RestorationFilterParameters.h - Released 2021-04-09T19:41:48Z
