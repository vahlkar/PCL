//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.2.6
// ----------------------------------------------------------------------------
// CometAlignmentParameters.h - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2018 Nikolay Volkov
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#ifndef __CometAlignmentParameters_h
#define __CometAlignmentParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CATargetFrames : public MetaTable
{
public:

   CATargetFrames( MetaProcess* );

   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

class CATargetFrameEnabled : public MetaBoolean
{
public:

   CATargetFrameEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CATargetFramePath : public MetaString
{
public:

   CATargetFramePath( MetaTable* );

   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

class CATargetFrameDate : public MetaString
{
public:

   CATargetFrameDate( MetaTable* );

   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

class CATargetFrameJDate : public MetaDouble
{
public:

   CATargetFrameJDate( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
};

// ----------------------------------------------------------------------------

class CATargetFrameX : public MetaDouble
{
public:

   CATargetFrameX( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

class CATargetFrameY : public MetaDouble
{
public:

   CATargetFrameY( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

//--------------------------------------------------------------------------

class CADrizzlePath : public MetaString
{
public:

   CADrizzlePath( MetaTable* );

   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

class CAInputHints : public MetaString
{
public:

   CAInputHints( MetaProcess* );

   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

class CAOutputHints : public MetaString
{
public:

   CAOutputHints( MetaProcess* );

   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

class CAOutputDir : public MetaString
{
public:

   CAOutputDir( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAOutputExtension : public MetaString
{
public:

   CAOutputExtension( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAPrefix : public MetaString
{
public:

   CAPrefix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAPostfix : public MetaString
{
public:

   CAPostfix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAOverwrite : public MetaBoolean
{
public:

   CAOverwrite( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAReference : public MetaUInt16
{
public:

   CAReference( MetaProcess* );

   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

class CASubtractFile : public MetaString
{
public:

   CASubtractFile( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CASubtractMode : public MetaBoolean
{
public:

   CASubtractMode( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAEnableLinearFit : public MetaBoolean
{
public:

   CAEnableLinearFit( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CARejectLow : public MetaFloat
{
public:

   CARejectLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CARejectHigh : public MetaFloat
{
public:

   CARejectHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CANormalize : public MetaBoolean
{
public:

   CANormalize( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CADrzSaveSA : public MetaBoolean
{
public:

   CADrzSaveSA( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CADrzSaveCA : public MetaBoolean
{
public:

   CADrzSaveCA( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAOperandIsDI : public MetaBoolean
{
public:

   CAOperandIsDI( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAPixelInterpolation : public MetaEnumeration
{
public:

   enum
   {
      NearestNeighbor,
      Bilinear,
      BicubicSpline,
      BicubicBSpline,
      Lanczos3,
      Lanczos4,
      Lanczos5,
      MitchellNetravaliFilter,
      CatmullRomSplineFilter,
      CubicBSplineFilter,
      Auto,
      NumberOfInterpolationAlgorithms,
      Default = BicubicSpline
   };

   CAPixelInterpolation( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

// ----------------------------------------------------------------------------

class CALinearClampingThreshold : public MetaFloat
{
public:

   CALinearClampingThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

extern CATargetFrames* TheTargetFrames;
extern CATargetFrameEnabled* TheTargetFrameEnabled;
extern CATargetFramePath* TheTargetFramePath;

extern CATargetFrameDate* TheTargetFrameDate;
extern CATargetFrameJDate* TheTargetFrameJDate;
extern CATargetFrameX* TheTargetFrameX;
extern CATargetFrameY* TheTargetFrameY;

extern CADrizzlePath* TheDrizzlePath;

extern CAInputHints* TheCAInputHintsParameter;
extern CAOutputHints* TheCAOutputHintsParameter;
extern CAOutputDir* TheOutputDir;
extern CAOutputExtension* TheCAOutputExtensionParameter;
extern CAPrefix* ThePrefix;
extern CAPostfix* ThePostfix;
extern CAOverwrite* TheOverwrite;

extern CAReference* TheReference;

extern CASubtractFile* TheSubtractFile;
extern CASubtractMode* TheSubtractMode;
extern CAOperandIsDI* TheOperandIsDI;
extern CAEnableLinearFit* TheEnableLinearFit;
extern CARejectLow* TheRejectLow;
extern CARejectHigh* TheRejectHigh;
extern CANormalize* TheNormalize;
extern CADrzSaveSA* TheDrzSaveSA;
extern CADrzSaveCA* TheDrzSaveCA;

extern CAPixelInterpolation* ThePixelInterpolationParameter;
extern CALinearClampingThreshold* TheLinearClampingThresholdParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif // __CometAlignmentParameters_h

// ----------------------------------------------------------------------------
// EOF CometAlignmentParameters.h - Released 2021-04-09T19:41:49Z
