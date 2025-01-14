//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIClientModule.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2024 Klaus Kretzschmar
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

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     2
#define MODULE_VERSION_REVISION  3
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2024
#define MODULE_RELEASE_MONTH     12
#define MODULE_RELEASE_DAY       28

#include "INDICCDFrameInterface.h"
#include "INDICCDFrameProcess.h"
#include "INDIClientModule.h"
#include "INDIDeviceControllerInterface.h"
#include "INDIDeviceControllerProcess.h"
#include "INDIMountInterface.h"
#include "INDIMountProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

INDIClientModule::INDIClientModule()
{
}

// ----------------------------------------------------------------------------

const char* INDIClientModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString INDIClientModule::Name() const
{
   return "INDIClient";
}

// ----------------------------------------------------------------------------

String INDIClientModule::Description() const
{
   return "PixInsight INDIClient Process Module";
}

// ----------------------------------------------------------------------------

String INDIClientModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String INDIClientModule::Author() const
{
   return "Klaus Kretzschmar (PTeam), Juan Conejero (PTeam)";
}

// ----------------------------------------------------------------------------

String INDIClientModule::Copyright() const
{
   return "Copyright (c) 2014-2024 Klaus Kretzschmar";
}

// ----------------------------------------------------------------------------

String INDIClientModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String INDIClientModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "INDIClient-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "INDIClient-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "INDIClient-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "INDIClient-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void INDIClientModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::INDIClientModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::INDIDeviceControllerProcess;
      new pcl::INDIDeviceControllerInterface;
      new pcl::INDICCDFrameProcess;
      new pcl::INDICCDFrameInterface;
      new pcl::INDIMountProcess;
      new pcl::INDIMountInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF INDIClientModule.cpp - Released 2024-12-28T16:54:15Z
