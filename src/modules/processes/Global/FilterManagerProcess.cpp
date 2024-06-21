//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.0
// ----------------------------------------------------------------------------
// FilterManagerProcess.cpp - Released 2024-06-18T15:49:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#include "FilterManagerInterface.h"
#include "FilterManagerParameters.h"
#include "FilterManagerProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FilterManagerProcess* TheFilterManagerProcess = nullptr;

// ----------------------------------------------------------------------------

FilterManagerProcess::FilterManagerProcess()
{
   TheFilterManagerProcess = this;

   new FMTask( this );
   new FMPath( this );
   new FMNoGUIMessages( this );
}

// ----------------------------------------------------------------------------

IsoString FilterManagerProcess::Id() const
{
   return "FilterManager";
}

// ----------------------------------------------------------------------------

IsoString FilterManagerProcess::Category() const
{
   return "Global,Spectrophotometry";
}

// ----------------------------------------------------------------------------

uint32 FilterManagerProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String FilterManagerProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String FilterManagerProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/FilterManager.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* FilterManagerProcess::DefaultInterface() const
{
   return TheFilterManagerInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* FilterManagerProcess::Create() const
{
   return new FilterManagerInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* FilterManagerProcess::Clone( const ProcessImplementation& p ) const
{
   const FilterManagerInstance* instPtr = dynamic_cast<const FilterManagerInstance*>( &p );
   return (instPtr != nullptr) ? new FilterManagerInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool FilterManagerProcess::PrefersGlobalExecution() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FilterManagerProcess.cpp - Released 2024-06-18T15:49:25Z
