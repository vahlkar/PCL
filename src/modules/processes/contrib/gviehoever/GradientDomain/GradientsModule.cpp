//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 0.6.4
// ----------------------------------------------------------------------------
// GradientsModule.cpp - Released 2024-12-28T16:54:15Z
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

#define MODULE_VERSION_MAJOR     0
#define MODULE_VERSION_MINOR     6
#define MODULE_VERSION_REVISION  4
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2024
#define MODULE_RELEASE_MONTH     12
#define MODULE_RELEASE_DAY       28

#include "GradientsHdrCompositionInterface.h"
#include "GradientsHdrCompositionProcess.h"
#include "GradientsHdrInterface.h"
#include "GradientsHdrProcess.h"
#include "GradientsMergeMosaicInterface.h"
#include "GradientsMergeMosaicProcess.h"
#include "GradientsModule.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GradientsModule::GradientsModule()
{
}

// ----------------------------------------------------------------------------

const char* GradientsModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
      MODULE_VERSION_MINOR,
      MODULE_VERSION_REVISION,
      MODULE_VERSION_BUILD,
      MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString GradientsModule::Name() const
{
   return "GradientDomain";
}

// ----------------------------------------------------------------------------

String GradientsModule::Description() const
{
   return "Gradient Domain Operations Module";
}

// ----------------------------------------------------------------------------

String GradientsModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String GradientsModule::Author() const
{
   return "Georg Viehoever";
}

// ----------------------------------------------------------------------------

String GradientsModule::Copyright() const
{
   return "Copyright (c) Georg Viehoever, 2011-2020. Licensed under LGPL 2.1";
}

// ----------------------------------------------------------------------------

String GradientsModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String GradientsModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Gradients-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Gradients-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Gradients-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Gradients-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void GradientsModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::GradientsModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::GradientsHdrProcess;
      new pcl::GradientsHdrInterface;

      new pcl::GradientsMergeMosaicProcess;
      new pcl::GradientsMergeMosaicInterface;

      new pcl::GradientsHdrCompositionProcess;
      new pcl::GradientsHdrCompositionInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF GradientsModule.cpp - Released 2024-12-28T16:54:15Z
