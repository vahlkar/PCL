//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard Sandbox Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SandboxModule.cpp - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Sandbox PixInsight module.
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
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  2
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2023
#define MODULE_RELEASE_MONTH     8
#define MODULE_RELEASE_DAY       28

#include "SandboxModule.h"
#include "SandboxProcess.h"
#include "SandboxInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SandboxModule::SandboxModule()
{
}

// ----------------------------------------------------------------------------

const char* SandboxModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString SandboxModule::Name() const
{
   /*
    * Replace with the actual name of this module. Must be unique.
    */
   return "Sandbox";
}

// ----------------------------------------------------------------------------

String SandboxModule::Description() const
{
   /*
    * Replace with a short (one-line) description of this module.
    */
   return "PixInsight Sandbox Process Module";
}

// ----------------------------------------------------------------------------

String SandboxModule::Company() const
{
   /*
    * Replace with the name of the company that has developed this module, or
    * return an empty String() if not applicable.
    */
   return "Pleiades Astrophoto S.L.";
}

// ----------------------------------------------------------------------------

String SandboxModule::Author() const
{
   /*
    * Comma-separated list of module authors.
    * Replace with the name(s) of the actual author(s) of this module.
    */
   return "PTeam";
}

// ----------------------------------------------------------------------------

String SandboxModule::Copyright() const
{
   /*
    * Replace with the actual copyright information.
    */
   return "Copyright (c) 2005-2023 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String SandboxModule::TradeMarks() const
{
   /*
    * Comma-separated list of trade marks.
    * PixInsight should always be included in the list.
    */
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String SandboxModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Sandbox-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Sandbox-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Sandbox-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Sandbox-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void SandboxModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

/*
 * Module installation routine.
 *
 * If this routine is defined as a public symbol in a module, the PixInsight
 * core application will call it just after loading and initializing the module
 * shared object or dynamic-link library.
 *
 * The mode argument specifies the kind of installation being performed by the
 * core application. See the pcl::InstallMode namespace for more information.
 */
PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   /*
    * When the PixInsight application installs this module, we just have to
    * instantiate the meta objects describing it.
    */
   new pcl::SandboxModule;

   /*
    * The mode argument tells us which kind of installation is being requested
    * by the PixInsight application. Incomplete installation requests only need
    * module descriptions.
    */
   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::SandboxProcess;
      new pcl::SandboxInterface;
   }

   /*
    * Return zero to signal successful installation
    */
   return 0;
}

// ----------------------------------------------------------------------------
// EOF SandboxModule.cpp - Released 2023-08-28T15:23:41Z
