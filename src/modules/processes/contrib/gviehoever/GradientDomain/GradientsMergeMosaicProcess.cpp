//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 0.6.4
// ----------------------------------------------------------------------------
// GradientsMergeMosaicProcess.cpp - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2020. Licensed under LGPL 2.1
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L.
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

#include "GradientsMergeMosaicInstance.h"
#include "GradientsMergeMosaicInterface.h"
#include "GradientsMergeMosaicParameters.h"
#include "GradientsMergeMosaicProcess.h"

namespace pcl
{
// ----------------------------------------------------------------------------

GradientsMergeMosaicProcess* TheGradientsMergeMosaicProcess = nullptr;

// ----------------------------------------------------------------------------

GradientsMergeMosaicProcess::GradientsMergeMosaicProcess()
{
   TheGradientsMergeMosaicProcess = this;

   new GradientsMergeMosaicTargetFrames( this );
   new GradientsMergeMosaicTargetFrameEnabled( TheGradientsMergeMosaicTargetFramesParameter );
   new GradientsMergeMosaicTargetFramePath( TheGradientsMergeMosaicTargetFramesParameter );
   new GradientsMergeMosaicType( this );
   new GradientsMergeMosaicShrinkCount( this );
   new GradientsMergeMosaicFeatherRadius( this );
   new GradientsMergeMosaicBlackPoint( this );
   new GradientsMergeMosaicGenerateMask( this );
}

// ----------------------------------------------------------------------------

IsoString GradientsMergeMosaicProcess::Id() const
{
   return "GradientMergeMosaic";
}

// ----------------------------------------------------------------------------

IsoString GradientsMergeMosaicProcess::Aliases() const
{
   return "GradientsMergeMosaic";
}

// ----------------------------------------------------------------------------

IsoString GradientsMergeMosaicProcess::Category() const
{
   return "GradientDomain,ImageIntegration";
}

// ----------------------------------------------------------------------------

uint32 GradientsMergeMosaicProcess::Version() const
{
   return 0x101;
}

// ----------------------------------------------------------------------------

String GradientsMergeMosaicProcess::Description() const
{
   return "<html>"
          "<p>Combines a set of images using Gradient Domain Image Compositing. "
          "All images need to have the same geometry and the same number of channels. "
          "Black (see BlackPoint parameter) will be considered transparent background.</p>"
          "</html>";
}

// ----------------------------------------------------------------------------

String GradientsMergeMosaicProcess::IconImageSVGFile() const
{
   return String(); //"@module_icons_dir/GradientsMergeMosaic.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* GradientsMergeMosaicProcess::DefaultInterface() const
{
   return TheGradientsMergeMosaicInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* GradientsMergeMosaicProcess::Create() const
{
   return new GradientsMergeMosaicInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* GradientsMergeMosaicProcess::Clone( const ProcessImplementation& p ) const
{
   const GradientsMergeMosaicInstance* instPtr = dynamic_cast<const GradientsMergeMosaicInstance*>( &p );
   return (instPtr != nullptr) ? new GradientsMergeMosaicInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF GradientsMergeMosaicProcess.cpp - Released 2021-04-09T19:41:49Z
