//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.1
// ----------------------------------------------------------------------------
// LRGBCombinationParameters.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2024 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __LRGBCombinationParameters_h
#define __LRGBCombinationParameters_h

#include "ChannelParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class LRGBChannelTable : public MetaTable
{
public:

   LRGBChannelTable( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   size_type MaxLength() const override;
};

extern LRGBChannelTable*   TheLRGBChannelTableParameter;

// ----------------------------------------------------------------------------

class LRGBChannelEnabled : public ChannelEnabled
{
public:

   LRGBChannelEnabled( MetaTable* );
};

extern LRGBChannelEnabled* TheLRGBChannelEnabledParameter;

// ----------------------------------------------------------------------------

class LRGBChannelId : public ChannelId
{
public:

   LRGBChannelId( MetaTable* );
};

extern LRGBChannelId*      TheLRGBChannelIdParameter;

// ----------------------------------------------------------------------------

class LRGBChannelWeight : public MetaDouble
{
public:

   LRGBChannelWeight( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LRGBChannelWeight*  TheLRGBChannelWeightParameter;

// ----------------------------------------------------------------------------

class LRGBLuminanceMTF : public MetaFloat
{
public:

   LRGBLuminanceMTF( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LRGBLuminanceMTF*   TheLRGBLuminanceMTFParameter;

// ----------------------------------------------------------------------------

class LRGBSaturationMTF : public MetaFloat
{
public:

   LRGBSaturationMTF( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LRGBSaturationMTF*  TheLRGBSaturationMTFParameter;

// ----------------------------------------------------------------------------

class LRGBClipHighlights : public MetaBoolean
{
public:

   LRGBClipHighlights( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LRGBClipHighlights* TheLRGBClipHighlightsParameter;

// ----------------------------------------------------------------------------

class LRGBNoiseReduction : public MetaBoolean
{
public:

   LRGBNoiseReduction( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LRGBNoiseReduction* TheLRGBNoiseReductionParameter;

// ----------------------------------------------------------------------------

class LRGBLayersRemoved : public MetaUInt32
{
public:

   LRGBLayersRemoved( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LRGBLayersRemoved* TheLRGBLayersRemovedParameter;

// ----------------------------------------------------------------------------

class LRGBLayersProtected : public MetaUInt32
{
public:

   LRGBLayersProtected( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LRGBLayersProtected* TheLRGBLayersProtectedParameter;

// ----------------------------------------------------------------------------

extern CSInheritAstrometricSolution* TheLRGBCInheritAstrometricSolutionParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __LRGBCombinationParameters_h

// ----------------------------------------------------------------------------
// EOF LRGBCombinationParameters.h - Released 2024-12-28T16:54:15Z
