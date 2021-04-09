//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 0.6.4
// ----------------------------------------------------------------------------
// GradientsHdrInstance.h - Released 2021-04-09T19:41:49Z
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

#ifndef __GradientsHdrInstance_h
#define __GradientsHdrInstance_h

#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class GradientsHdrInstance : public ProcessImplementation
{
public:

   GradientsHdrInstance( const MetaProcess* );
   GradientsHdrInstance( const GradientsHdrInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   ///  use pow(10,expGradient) to map gradient values
   double expGradient;
   /// relative log10(maximum) gradient value that is clampled
   double logMaxGradient;
   /// relative log10(minimum) gradient value that is clamped
   double logMinGradient;
   /// rescale to range [0,1]? Otherwise rescale to original image range
   bool bRescale01;
   /// preserve relative proportions of RGB, otherwise just set luminance
   bool bPreserveColor;

   /// for realtime preview. Zoom is the zoomlevel used
   void ApplyClip16( UInt16Image& img, int zoom );

   friend class GradientsHdrEngine;
   friend class GradientsHdrProcess;
   friend class GradientsHdrInterface;
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __GradientsHdrInstance_h

// ----------------------------------------------------------------------------
// EOF GradientsHdrInstance.h - Released 2021-04-09T19:41:49Z
