//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// NoiseGeneratorParameters.cpp - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "NoiseGeneratorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

NGNoiseAmount*                 TheNGNoiseAmountParameter = nullptr;
NGNoiseDistribution*           TheNGNoiseDistributionParameter = nullptr;
NGImpulsionalNoiseProbability* TheNGImpulsionalNoiseProbabilityParameter = nullptr;
NGPreserveBrightness*          TheNGPreserveBrightnessParameter = nullptr; // ### DEPRECATED

// ----------------------------------------------------------------------------

NGNoiseAmount::NGNoiseAmount( MetaProcess* p ) : MetaFloat( p )
{
   TheNGNoiseAmountParameter = this;
}

IsoString NGNoiseAmount::Id() const
{
   return "amount";
}

int NGNoiseAmount::Precision() const
{
   return 8;
}

double NGNoiseAmount::DefaultValue() const
{
   return 0.1;
}

// ----------------------------------------------------------------------------

NGNoiseDistribution::NGNoiseDistribution( MetaProcess* p ) : MetaEnumeration( p )
{
   TheNGNoiseDistributionParameter = this;
}

IsoString NGNoiseDistribution::Id() const
{
   return "distribution";
}

size_type NGNoiseDistribution::NumberOfElements() const
{
   return NumberOfNGNoiseDistributions;
}

IsoString NGNoiseDistribution::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Uniform:     return "Uniform";
   case Normal:      return "Normal";
   case Poisson:     return "Poisson";
   case Impulsional: return "Impulsional";
   }
}

int NGNoiseDistribution::ElementValue( size_type i ) const
{
   return int( i );
}

size_type NGNoiseDistribution::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

NGImpulsionalNoiseProbability::NGImpulsionalNoiseProbability( MetaProcess* p ) : MetaFloat( p )
{
   TheNGImpulsionalNoiseProbabilityParameter = this;
}

IsoString NGImpulsionalNoiseProbability::Id() const
{
   return "impulsionalNoiseProbability";
}

int NGImpulsionalNoiseProbability::Precision() const
{
   return 4;
}

double NGImpulsionalNoiseProbability::DefaultValue() const
{
   return 0.0500;
}

// ----------------------------------------------------------------------------

// ### DEPRECATED
NGPreserveBrightness::NGPreserveBrightness( MetaProcess* p ) : MetaEnumeration( p )
{
   TheNGPreserveBrightnessParameter = this;
}

IsoString NGPreserveBrightness::Id() const
{
   return "preserveBrightness";
}

size_type NGPreserveBrightness::NumberOfElements() const
{
   return NumberOfNGPreserveBrightnessModes;
}

IsoString NGPreserveBrightness::ElementId( size_type i ) const
{
   switch ( i )
   {
   case None:           return "None";
   default:
   case PreserveMedian: return "PreserveMedian";
   case PreserveMean:   return "PreserveMean";
   }
}

int NGPreserveBrightness::ElementValue( size_type i ) const
{
   return int( i );
}

size_type NGPreserveBrightness::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NoiseGeneratorParameters.cpp - Released 2023-08-28T15:23:41Z
