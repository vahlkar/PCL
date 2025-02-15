//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// ImageModule.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     3
#define MODULE_VERSION_REVISION  5
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2024
#define MODULE_RELEASE_MONTH     12
#define MODULE_RELEASE_DAY       28

#include "CreateAlphaChannelsInterface.h"
#include "CreateAlphaChannelsProcess.h"
#include "DynamicPSFInterface.h"
#include "DynamicPSFProcess.h"
#include "ExtractAlphaChannelsInterface.h"
#include "ExtractAlphaChannelsProcess.h"
#include "FITSHeaderAction.h"
#include "FITSHeaderInterface.h"
#include "FITSHeaderProcess.h"
#include "ImageIdentifierInterface.h"
#include "ImageIdentifierProcess.h"
#include "ImageModule.h"
#include "NewImageAction.h"
#include "NewImageInterface.h"
#include "NewImageProcess.h"
#include "SampleFormatConversionInterface.h"
#include "SampleFormatConversionProcess.h"
#include "StatisticsInterface.h"
#include "StatisticsProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

const char* ImageModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString ImageModule::Name() const
{
   return "Image";
}

// ----------------------------------------------------------------------------

String ImageModule::Description() const
{
   return "PixInsight Standard Image Process Module";
}

// ----------------------------------------------------------------------------

String ImageModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String ImageModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String ImageModule::Copyright() const
{
   return "Copyright (c) 2005-2024 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String ImageModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String ImageModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Image-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Image-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Image-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Image-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void ImageModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void ImageModule::OnLoad()
{
   new NewImageAction;
   new FITSHeaderAction;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::ImageModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::NewImageProcess;
      new pcl::NewImageInterface;
      new pcl::ImageIdentifierProcess;
      new pcl::ImageIdentifierInterface;
      new pcl::SampleFormatConversionProcess;
      new pcl::SampleFormatConversionInterface;
      new pcl::CreateAlphaChannelsProcess;
      new pcl::CreateAlphaChannelsInterface;
      new pcl::ExtractAlphaChannelsProcess;
      new pcl::ExtractAlphaChannelsInterface;
      new pcl::FITSHeaderProcess;
      new pcl::FITSHeaderInterface;
      new pcl::StatisticsProcess;
      new pcl::StatisticsInterface;
      new pcl::DynamicPSFProcess;
      new pcl::DynamicPSFInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF ImageModule.cpp - Released 2024-12-28T16:54:15Z
