//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 0.6.4
// ----------------------------------------------------------------------------
// GradientsHdrCompositionProcess.cpp - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2020. Licensed under LGPL 2.1
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#include "GradientsHdrCompositionProcess.h"
#include "GradientsHdrCompositionInstance.h"
#include "GradientsHdrCompositionInterface.h"
#include "GradientsHdrCompositionParameters.h"

namespace pcl
{
// ----------------------------------------------------------------------------

GradientsHdrCompositionProcess* TheGradientsHdrCompositionProcess = nullptr;

// ----------------------------------------------------------------------------

GradientsHdrCompositionProcess::GradientsHdrCompositionProcess()
{
   TheGradientsHdrCompositionProcess = this;

   new GradientsHdrCompositionTargetFrames( this );
   new GradientsHdrCompositionTargetFrameEnabled( TheGradientsHdrCompositionTargetFramesParameter );
   new GradientsHdrCompositionTargetFramePath( TheGradientsHdrCompositionTargetFramesParameter );
   new GradientsHdrCompositionLogBias( this );
   new GradientsHdrCompositionKeepLog( this );
   new GradientsHdrCompositionNegativeBias( this );
   new GradientsHdrCompositionGenerateMask( this );
}

// ----------------------------------------------------------------------------

IsoString GradientsHdrCompositionProcess::Id() const
{
   return "GradientHDRComposition";
}

// ----------------------------------------------------------------------------

IsoString GradientsHdrCompositionProcess::Aliases() const
{
   return "GradientsHdrComposition";
}

// ----------------------------------------------------------------------------

IsoString GradientsHdrCompositionProcess::Category() const
{
   return "GradientDomain";
}

// ----------------------------------------------------------------------------

uint32 GradientsHdrCompositionProcess::Version() const
{
   return 0x101;
}

// ----------------------------------------------------------------------------

String GradientsHdrCompositionProcess::Description() const
{
   return "<html>"
          "<p>Combines a set of aligned images with different exposures into an HDR type image. "
          "All images need to have the same geometry and the same number of channels. The need to be linear with black close to 0. "
          "Values of exactly 0 will be considered background.</p>"
          "</html>";
}

// ----------------------------------------------------------------------------

String GradientsHdrCompositionProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/GradientHDRComposition.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* GradientsHdrCompositionProcess::DefaultInterface() const
{
   return TheGradientsHdrCompositionInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* GradientsHdrCompositionProcess::Create() const
{
   return new GradientsHdrCompositionInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* GradientsHdrCompositionProcess::Clone( const ProcessImplementation& p ) const
{
   const GradientsHdrCompositionInstance* instPtr = dynamic_cast<const GradientsHdrCompositionInstance*>( &p );
   return (instPtr != nullptr) ? new GradientsHdrCompositionInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF GradientsHdrCompositionProcess.cpp - Released 2023-08-28T15:23:41Z
