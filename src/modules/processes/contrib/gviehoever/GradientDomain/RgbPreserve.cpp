//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 0.6.4
// ----------------------------------------------------------------------------
// RgbPreserve.cpp - Released 2023-08-28T15:23:41Z
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

#include "RgbPreserve.h"
#include "Assert.h"
//#include <cassert>

namespace pcl
{

// ----------------------------------------------------------------------------

RgbPreserve::RgbPreserve()
{
}

// ----------------------------------------------------------------------------

RgbPreserve::~RgbPreserve()
{
}

// ----------------------------------------------------------------------------

void RgbPreserve::rgbTransformation( imageType_t const& rOrigLuminance_p,
   imageType_t const& rNewLuminance_p,
   imageType_t& rImage_p ) const
{
#ifdef DEBUG
   int width = rImage_p.Width();
   int height = rImage_p.Height();
   Assert( width == rOrigLuminance_p.Width() );
   Assert( height == rOrigLuminance_p.Height() );
   Assert( width == rNewLuminance_p.Width() );
   Assert( height == rNewLuminance_p.Height() );
   Assert( rOrigLuminance_p.NumberOfNominalChannels() == 1 );
   Assert( rNewLuminance_p.NumberOfNominalChannels() == 1 );
#endif
   std::size_t channels = rImage_p.NumberOfNominalChannels();

   // this is the multiplication factor
   imageType_t propImage( rNewLuminance_p );
   propImage.ResetSelections();
   propImage.Div( rOrigLuminance_p );

   rImage_p.ResetSelections();
   // note: we need to do this on a float image. Otherwise, we
   // most likely will suffer from overflows
   for ( std::size_t chan = 0; chan < channels; chan++ )
   {
      rImage_p.SelectChannel( chan );
      rImage_p.Mul( propImage );
   }
   realType_t dMin, dMax;
   rNewLuminance_p.GetExtremePixelValues( dMin, dMax );
   rImage_p.ResetSelections();
   rImage_p.Rescale( dMin, dMax );
}

// ----------------------------------------------------------------------------

}; // namespace pcl

// ----------------------------------------------------------------------------
// EOF RgbPreserve.cpp - Released 2023-08-28T15:23:41Z
