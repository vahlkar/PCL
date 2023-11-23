//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.3
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceModule.cpp - Released 2023-11-23T18:45:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
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

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     2
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2023
#define MODULE_RELEASE_MONTH     11
#define MODULE_RELEASE_DAY       23

#include "NetworkServiceInterface.h"
#include "NetworkServiceModule.h"
#include "NetworkServiceProcess.h"
#include "NetworkServiceTask.h"

namespace pcl
{

// ----------------------------------------------------------------------------

NetworkServiceModule::NetworkServiceModule()
{
}

// ----------------------------------------------------------------------------

const char* NetworkServiceModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString NetworkServiceModule::Name() const
{
   return "NetworkService";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::Description() const
{
   return "PixInsight NetworkService Process Module";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::Copyright() const
{
   return "Copyright (c) 2018-2023 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "NetworkService-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "NetworkService-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "NetworkService-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "NetworkService-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void NetworkServiceModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void NetworkServiceModule::OnUnload()
{
   if ( TheNetworkServiceProcess != nullptr )
      while ( TheNetworkServiceProcess->IsRunning() )
      {
         ProcessEvents();
         TheNetworkServiceProcess->Stop();
         Sleep( 150 );
      }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::NetworkServiceModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      /*
       * Instantiate the task class(es) implemented in a custom version of the
       * NetworkService module (and remove the following instantiation of the
       * default NetworkServiceTask class).
       */
      new pcl::NetworkServiceTask;

      new pcl::NetworkServiceProcess;
      new pcl::NetworkServiceInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF NetworkServiceModule.cpp - Released 2023-11-23T18:45:25Z
