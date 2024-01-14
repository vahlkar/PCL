//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.5
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.3.7
// ----------------------------------------------------------------------------
// CometAlignmentParameters.h - Released 2024-01-13T15:48:23Z
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2023 Nikolay Volkov
// Copyright (c) 2019-2023 Juan Conejero (PTeam)
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L.
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
   IsoString Aliases() const override;
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

// ----------------------------------------------------------------------------

class CATargetFrameFixed : public MetaBoolean
{
public:

   CATargetFrameFixed( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

//--------------------------------------------------------------------------

class CATargetFrameDrizzlePath : public MetaString
{
public:

   CATargetFrameDrizzlePath( MetaTable* );

   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

class CAReferenceIndex : public MetaUInt32
{
public:

   CAReferenceIndex( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
};

// ----------------------------------------------------------------------------

class CAFitPSF : public MetaBoolean
{
public:

   CAFitPSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAOperandImageFilePath : public MetaString
{
public:

   CAOperandImageFilePath( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
};

// ----------------------------------------------------------------------------

class CAOperandSubtractAligned : public MetaBoolean
{
public:

   CAOperandSubtractAligned( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAOperandLinearFit : public MetaBoolean
{
public:

   CAOperandLinearFit( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAOperandLinearFitLow : public MetaFloat
{
public:

   CAOperandLinearFitLow( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAOperandLinearFitHigh : public MetaFloat
{
public:

   CAOperandLinearFitHigh( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAOperandNormalize : public MetaBoolean
{
public:

   CAOperandNormalize( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CADrizzleWriteStarAlignedImage : public MetaBoolean
{
public:

   CADrizzleWriteStarAlignedImage( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CADrizzleWriteCometAlignedImage : public MetaBoolean
{
public:

   CADrizzleWriteCometAlignedImage( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
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
      Default = Lanczos4
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

class CAOutputDirectory : public MetaString
{
public:

   CAOutputDirectory( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
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

class CAOutputPrefix : public MetaString
{
public:

   CAOutputPrefix( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   String DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAOutputPostfix : public MetaString
{
public:

   CAOutputPostfix( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   String DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAOverwriteExistingFiles : public MetaBoolean
{
public:

   CAOverwriteExistingFiles( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAGenerateCometPathMap : public MetaBoolean
{
public:

   CAGenerateCometPathMap( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAOnError : public MetaEnumeration
{
public:

   enum { Continue,
          Abort,
          AskUser,
          NumberOfItems,
          Default = Continue };

   CAOnError( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

// ----------------------------------------------------------------------------

class CAUseFileThreads : public MetaBoolean
{
public:

   CAUseFileThreads( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class CAFileThreadOverload : public MetaFloat
{
public:

   CAFileThreadOverload( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

class CAMaxFileReadThreads : public MetaInt32
{
public:

   CAMaxFileReadThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};


// ----------------------------------------------------------------------------

class CAMaxFileWriteThreads : public MetaInt32
{
public:

   CAMaxFileWriteThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

extern CATargetFrames* TheCATargetFramesParameter;
extern CATargetFramePath* TheCATargetFramePathParameter;
extern CATargetFrameEnabled* TheCATargetFrameEnabledParameter;
extern CATargetFrameDate* TheCATargetFrameDateParameter;
extern CATargetFrameJDate* TheCATargetFrameJDateParameter;
extern CATargetFrameX* TheCATargetFrameXParameter;
extern CATargetFrameY* TheCATargetFrameYParameter;
extern CATargetFrameFixed* TheCATargetFrameFixedParameter;
extern CATargetFrameDrizzlePath* TheCATargetFrameDrizzlePathParameter;

extern CAReferenceIndex* TheCAReferenceIndexParameter;
extern CAFitPSF* TheCAFitPSFParameter;
extern CAOperandImageFilePath* TheCAOperandImageFilePathParameter;
extern CAOperandSubtractAligned* TheCAOperandSubtractAlignedParameter;
extern CAOperandLinearFit* TheCAOperandLinearFitParameter;
extern CAOperandLinearFitLow* TheCAOperandLinearFitLowParameter;
extern CAOperandLinearFitHigh* TheCAOperandLinearFitHighParameter;
extern CAOperandNormalize* TheCAOperandNormalizeParameter;
extern CADrizzleWriteStarAlignedImage* TheCADrizzleWriteStarAlignedImageParameter;
extern CADrizzleWriteCometAlignedImage* TheCADrizzleWriteCometAlignedImageParameter;
extern CAPixelInterpolation* TheCAPixelInterpolationParameter;
extern CALinearClampingThreshold* TheCALinearClampingThresholdParameter;

extern CAInputHints* TheCAInputHintsParameter;
extern CAOutputHints* TheCAOutputHintsParameter;

extern CAOutputDirectory* TheCAOutputDirectoryParameter;
extern CAOutputExtension* TheCAOutputExtensionParameter;
extern CAOutputPrefix* TheCAOutputPrefixParameter;
extern CAOutputPostfix* TheCAOutputPostfixParameter;
extern CAOverwriteExistingFiles* TheCAOverwriteExistingFilesParameter;
extern CAGenerateCometPathMap* TheCAGenerateCometPathMapParameter;
extern CAOnError* TheCAOnErrorParameter;

extern CAUseFileThreads* TheCAUseFileThreadsParameter;
extern CAFileThreadOverload* TheCAFileThreadOverloadParameter;
extern CAMaxFileReadThreads* TheCAMaxFileReadThreadsParameter;
extern CAMaxFileWriteThreads* TheCAMaxFileWriteThreadsParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif // __CometAlignmentParameters_h

// ----------------------------------------------------------------------------
// EOF CometAlignmentParameters.h - Released 2024-01-13T15:48:23Z
