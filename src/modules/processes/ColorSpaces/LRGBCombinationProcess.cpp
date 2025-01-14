//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.1
// ----------------------------------------------------------------------------
// LRGBCombinationProcess.cpp - Released 2024-12-28T16:54:15Z
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

#include "LRGBCombinationProcess.h"
#include "LRGBCombinationParameters.h"
#include "LRGBCombinationInstance.h"
#include "LRGBCombinationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LRGBCombinationProcess* TheLRGBCombinationProcess = nullptr;

// ----------------------------------------------------------------------------

LRGBCombinationProcess::LRGBCombinationProcess()
{
   TheLRGBCombinationProcess = this;

   new LRGBChannelTable( this );
   new LRGBChannelEnabled( TheLRGBChannelTableParameter );
   new LRGBChannelId( TheLRGBChannelTableParameter );
   new LRGBChannelWeight( TheLRGBChannelTableParameter );
   new LRGBLuminanceMTF( this );
   new LRGBSaturationMTF( this );
   new LRGBClipHighlights( this );
   new LRGBNoiseReduction( this );
   new LRGBLayersRemoved( this );
   new LRGBLayersProtected( this );
   TheLRGBCInheritAstrometricSolutionParameter = new CSInheritAstrometricSolution( this );
}

// ----------------------------------------------------------------------------

IsoString LRGBCombinationProcess::Id() const
{
   return "LRGBCombination";
}

// ----------------------------------------------------------------------------

IsoString LRGBCombinationProcess::Categories() const
{
   return "ColorSpaces";
}

// ----------------------------------------------------------------------------

String LRGBCombinationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String LRGBCombinationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/LRGBCombination.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* LRGBCombinationProcess::DefaultInterface() const
{
   return TheLRGBCombinationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* LRGBCombinationProcess::Create() const
{
   return new LRGBCombinationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* LRGBCombinationProcess::Clone( const ProcessImplementation& p ) const
{
   const LRGBCombinationInstance* instPtr = dynamic_cast<const LRGBCombinationInstance*>( &p );
   return (instPtr != nullptr) ? new LRGBCombinationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool LRGBCombinationProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LRGBCombinationProcess.cpp - Released 2024-12-28T16:54:15Z
