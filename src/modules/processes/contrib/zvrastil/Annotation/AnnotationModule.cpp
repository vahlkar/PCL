//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AnnotationModule.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard Annotation PixInsight module.
//
// Copyright (c) 2010-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
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
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE eng

#define MODULE_RELEASE_YEAR      2024
#define MODULE_RELEASE_MONTH     12
#define MODULE_RELEASE_DAY       28

#include "AnnotationInterface.h"
#include "AnnotationModule.h"
#include "AnnotationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

AnnotationModule::AnnotationModule()
{
}

// ----------------------------------------------------------------------------

const char* AnnotationModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
      MODULE_VERSION_MINOR,
      MODULE_VERSION_REVISION,
      MODULE_VERSION_BUILD,
      MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString AnnotationModule::Name() const
{
   return "Annotation";
}

// ----------------------------------------------------------------------------

String AnnotationModule::Description() const
{
   return "Annotation Process Module"; // Replace with your own description
}

// ----------------------------------------------------------------------------

String AnnotationModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String AnnotationModule::Author() const
{
   return "Zbynek Vrastil (PTeam)";
}

// ----------------------------------------------------------------------------

String AnnotationModule::Copyright() const
{
   return "Copyright (c) 2010-2024 Zbynek Vrastil";
}

// ----------------------------------------------------------------------------

String AnnotationModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String AnnotationModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Annotation-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Annotation-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Annotation-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Annotation-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void AnnotationModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::AnnotationModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::AnnotationProcess;
      new pcl::AnnotationInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF AnnotationModule.cpp - Released 2024-12-28T16:54:15Z
