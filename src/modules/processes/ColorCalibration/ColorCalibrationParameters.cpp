//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.5.1
// ----------------------------------------------------------------------------
// ColorCalibrationParameters.cpp - Released 2021-04-09T19:41:48Z
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

#include "ColorCalibrationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CCWhiteReferenceViewId*          TheCCWhiteReferenceViewIdParameter = nullptr;
CCWhiteLow*                      TheCCWhiteLowParameter = nullptr;
CCWhiteHigh*                     TheCCWhiteHighParameter = nullptr;
CCWhiteUseROI*                   TheCCWhiteUseROIParameter = nullptr;
CCWhiteROIX0*                    TheCCWhiteROIX0Parameter = nullptr;
CCWhiteROIY0*                    TheCCWhiteROIY0Parameter = nullptr;
CCWhiteROIX1*                    TheCCWhiteROIX1Parameter = nullptr;
CCWhiteROIY1*                    TheCCWhiteROIY1Parameter = nullptr;
CCStructureDetection*            TheCCStructureDetectionParameter = nullptr;
CCStructureLayers*               TheCCStructureLayersParameter = nullptr;
CCNoiseLayers*                   TheCCNoiseLayersParameter = nullptr;
CCManualWhiteBalance*            TheCCManualWhiteBalanceParameter = nullptr;
CCManualRedFactor*               TheCCManualRedFactorParameter = nullptr;
CCManualGreenFactor*             TheCCManualGreenFactorParameter = nullptr;
CCManualBlueFactor*              TheCCManualBlueFactorParameter = nullptr;
CCBackgroundReferenceViewId*     TheCCBackgroundReferenceViewIdParameter = nullptr;
CCBackgroundLow*                 TheCCBackgroundLowParameter = nullptr;
CCBackgroundHigh*                TheCCBackgroundHighParameter = nullptr;
CCBackgroundUseROI*              TheCCBackgroundUseROIParameter = nullptr;
CCBackgroundROIX0*               TheCCBackgroundROIX0Parameter = nullptr;
CCBackgroundROIY0*               TheCCBackgroundROIY0Parameter = nullptr;
CCBackgroundROIX1*               TheCCBackgroundROIX1Parameter = nullptr;
CCBackgroundROIY1*               TheCCBackgroundROIY1Parameter = nullptr;
CCOutputWhiteReferenceMask*      TheCCOutputWhiteReferenceMaskParameter = nullptr;
CCOutputBackgroundReferenceMask* TheCCOutputBackgroundReferenceMaskParameter = nullptr;

// ----------------------------------------------------------------------------

CCWhiteReferenceViewId::CCWhiteReferenceViewId( MetaProcess* P ) : MetaString( P )
{
   TheCCWhiteReferenceViewIdParameter = this;
}

IsoString CCWhiteReferenceViewId::Id() const
{
   return "whiteReferenceViewId";
}

// ----------------------------------------------------------------------------

CCWhiteLow::CCWhiteLow( MetaProcess* P ) : MetaFloat( P )
{
   TheCCWhiteLowParameter = this;
}

IsoString CCWhiteLow::Id() const
{
   return "whiteLow";
}

int CCWhiteLow::Precision() const
{
   return 7;
}

double CCWhiteLow::MinimumValue() const
{
   return 0;
}

double CCWhiteLow::MaximumValue() const
{
   return 1;
}

double CCWhiteLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

CCWhiteHigh::CCWhiteHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheCCWhiteHighParameter = this;
}

IsoString CCWhiteHigh::Id() const
{
   return "whiteHigh";
}

int CCWhiteHigh::Precision() const
{
   return 7;
}

double CCWhiteHigh::MinimumValue() const
{
   return 0;
}

double CCWhiteHigh::MaximumValue() const
{
   return 1;
}

double CCWhiteHigh::DefaultValue() const
{
   return 0.9;
}

// ----------------------------------------------------------------------------

CCWhiteUseROI::CCWhiteUseROI( MetaProcess* P ) : MetaBoolean( P )
{
   TheCCWhiteUseROIParameter = this;
}

IsoString CCWhiteUseROI::Id() const
{
   return "whiteUseROI";
}

bool CCWhiteUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCWhiteROIX0::CCWhiteROIX0( MetaProcess* P ) : MetaInt32( P )
{
   TheCCWhiteROIX0Parameter = this;
}

IsoString CCWhiteROIX0::Id() const
{
   return "whiteROIX0";
}

double CCWhiteROIX0::DefaultValue() const
{
   return 0;
}

double CCWhiteROIX0::MinimumValue() const
{
   return 0;
}

double CCWhiteROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

CCWhiteROIY0::CCWhiteROIY0( MetaProcess* P ) : MetaInt32( P )
{
   TheCCWhiteROIY0Parameter = this;
}

IsoString CCWhiteROIY0::Id() const
{
   return "whiteROIY0";
}

double CCWhiteROIY0::DefaultValue() const
{
   return 0;
}

double CCWhiteROIY0::MinimumValue() const
{
   return 0;
}

double CCWhiteROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

CCWhiteROIX1::CCWhiteROIX1( MetaProcess* P ) : MetaInt32( P )
{
   TheCCWhiteROIX1Parameter = this;
}

IsoString CCWhiteROIX1::Id() const
{
   return "whiteROIX1";
}

double CCWhiteROIX1::DefaultValue() const
{
   return 0;
}

double CCWhiteROIX1::MinimumValue() const
{
   return 0;
}

double CCWhiteROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

CCWhiteROIY1::CCWhiteROIY1( MetaProcess* P ) : MetaInt32( P )
{
   TheCCWhiteROIY1Parameter = this;
}

IsoString CCWhiteROIY1::Id() const
{
   return "whiteROIY1";
}

double CCWhiteROIY1::DefaultValue() const
{
   return 0;
}

double CCWhiteROIY1::MinimumValue() const
{
   return 0;
}

double CCWhiteROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

CCStructureDetection::CCStructureDetection( MetaProcess* P ) : MetaBoolean( P )
{
   TheCCStructureDetectionParameter = this;
}

IsoString CCStructureDetection::Id() const
{
   return "structureDetection";
}

bool CCStructureDetection::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CCStructureLayers::CCStructureLayers( MetaProcess* P ) : MetaInt32( P )
{
   TheCCStructureLayersParameter = this;
}

IsoString CCStructureLayers::Id() const
{
   return "structureLayers";
}

double CCStructureLayers::MinimumValue() const
{
   return 2;
}

double CCStructureLayers::MaximumValue() const
{
   return 8;
}

double CCStructureLayers::DefaultValue() const
{
   return 5;
}

// ----------------------------------------------------------------------------

CCNoiseLayers::CCNoiseLayers( MetaProcess* P ) : MetaInt32( P )
{
   TheCCNoiseLayersParameter = this;
}

IsoString CCNoiseLayers::Id() const
{
   return "noiseLayers";
}

double CCNoiseLayers::MinimumValue() const
{
   return 0;
}

double CCNoiseLayers::MaximumValue() const
{
   return 4;
}

double CCNoiseLayers::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CCManualWhiteBalance::CCManualWhiteBalance( MetaProcess* P ) : MetaBoolean( P )
{
   TheCCManualWhiteBalanceParameter = this;
}

IsoString CCManualWhiteBalance::Id() const
{
   return "manualWhiteBalance";
}

bool CCManualWhiteBalance::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCManualRedFactor::CCManualRedFactor( MetaProcess* P ) : MetaFloat( P )
{
   TheCCManualRedFactorParameter = this;
}

IsoString CCManualRedFactor::Id() const
{
   return "manualRedFactor";
}

int CCManualRedFactor::Precision() const
{
   return 4;
}

double CCManualRedFactor::MinimumValue() const
{
   return 0;
}

double CCManualRedFactor::MaximumValue() const
{
   return 1;
}

double CCManualRedFactor::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CCManualGreenFactor::CCManualGreenFactor( MetaProcess* P ) : MetaFloat( P )
{
   TheCCManualGreenFactorParameter = this;
}

IsoString CCManualGreenFactor::Id() const
{
   return "manualGreenFactor";
}

int CCManualGreenFactor::Precision() const
{
   return 4;
}

double CCManualGreenFactor::MinimumValue() const
{
   return 0;
}

double CCManualGreenFactor::MaximumValue() const
{
   return 1;
}

double CCManualGreenFactor::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CCManualBlueFactor::CCManualBlueFactor( MetaProcess* P ) : MetaFloat( P )
{
   TheCCManualBlueFactorParameter = this;
}

IsoString CCManualBlueFactor::Id() const
{
   return "manualBlueFactor";
}

int CCManualBlueFactor::Precision() const
{
   return 4;
}

double CCManualBlueFactor::MinimumValue() const
{
   return 0;
}

double CCManualBlueFactor::MaximumValue() const
{
   return 1;
}

double CCManualBlueFactor::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CCBackgroundReferenceViewId::CCBackgroundReferenceViewId( MetaProcess* P ) : MetaString( P )
{
   TheCCBackgroundReferenceViewIdParameter = this;
}

IsoString CCBackgroundReferenceViewId::Id() const
{
   return "backgroundReferenceViewId";
}

// ----------------------------------------------------------------------------

CCBackgroundLow::CCBackgroundLow( MetaProcess* P ) : MetaFloat( P )
{
   TheCCBackgroundLowParameter = this;
}

IsoString CCBackgroundLow::Id() const
{
   return "backgroundLow";
}

int CCBackgroundLow::Precision() const
{
   return 7;
}

double CCBackgroundLow::MinimumValue() const
{
   return 0;
}

double CCBackgroundLow::MaximumValue() const
{
   return 1;
}

double CCBackgroundLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

CCBackgroundHigh::CCBackgroundHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheCCBackgroundHighParameter = this;
}

IsoString CCBackgroundHigh::Id() const
{
   return "backgroundHigh";
}

int CCBackgroundHigh::Precision() const
{
   return 7;
}

double CCBackgroundHigh::MinimumValue() const
{
   return 0;
}

double CCBackgroundHigh::MaximumValue() const
{
   return 1;
}

double CCBackgroundHigh::DefaultValue() const
{
   return 0.1;
}

// ----------------------------------------------------------------------------

CCBackgroundUseROI::CCBackgroundUseROI( MetaProcess* P ) : MetaBoolean( P )
{
   TheCCBackgroundUseROIParameter = this;
}

IsoString CCBackgroundUseROI::Id() const
{
   return "backgroundUseROI";
}

bool CCBackgroundUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCBackgroundROIX0::CCBackgroundROIX0( MetaProcess* P ) : MetaInt32( P )
{
   TheCCBackgroundROIX0Parameter = this;
}

IsoString CCBackgroundROIX0::Id() const
{
   return "backgroundROIX0";
}

double CCBackgroundROIX0::DefaultValue() const
{
   return 0;
}

double CCBackgroundROIX0::MinimumValue() const
{
   return 0;
}

double CCBackgroundROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

CCBackgroundROIY0::CCBackgroundROIY0( MetaProcess* P ) : MetaInt32( P )
{
   TheCCBackgroundROIY0Parameter = this;
}

IsoString CCBackgroundROIY0::Id() const
{
   return "backgroundROIY0";
}

double CCBackgroundROIY0::DefaultValue() const
{
   return 0;
}

double CCBackgroundROIY0::MinimumValue() const
{
   return 0;
}

double CCBackgroundROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

CCBackgroundROIX1::CCBackgroundROIX1( MetaProcess* P ) : MetaInt32( P )
{
   TheCCBackgroundROIX1Parameter = this;
}

IsoString CCBackgroundROIX1::Id() const
{
   return "backgroundROIX1";
}

double CCBackgroundROIX1::DefaultValue() const
{
   return 0;
}

double CCBackgroundROIX1::MinimumValue() const
{
   return 0;
}

double CCBackgroundROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

CCBackgroundROIY1::CCBackgroundROIY1( MetaProcess* P ) : MetaInt32( P )
{
   TheCCBackgroundROIY1Parameter = this;
}

IsoString CCBackgroundROIY1::Id() const
{
   return "backgroundROIY1";
}

double CCBackgroundROIY1::DefaultValue() const
{
   return 0;
}

double CCBackgroundROIY1::MinimumValue() const
{
   return 0;
}

double CCBackgroundROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

CCOutputWhiteReferenceMask::CCOutputWhiteReferenceMask( MetaProcess* P ) : MetaBoolean( P )
{
   TheCCOutputWhiteReferenceMaskParameter = this;
}

IsoString CCOutputWhiteReferenceMask::Id() const
{
   return "outputWhiteReferenceMask";
}

bool CCOutputWhiteReferenceMask::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCOutputBackgroundReferenceMask::CCOutputBackgroundReferenceMask( MetaProcess* P ) : MetaBoolean( P )
{
   TheCCOutputBackgroundReferenceMaskParameter = this;
}

IsoString CCOutputBackgroundReferenceMask::Id() const
{
   return "outputBackgroundReferenceMask";
}

bool CCOutputBackgroundReferenceMask::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorCalibrationParameters.cpp - Released 2021-04-09T19:41:48Z
