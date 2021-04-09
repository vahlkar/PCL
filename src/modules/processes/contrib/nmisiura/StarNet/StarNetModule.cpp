//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard StarNet Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// StarNetModule.cpp - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarNet PixInsight module.
//
// Copyright (c) 2018-2020 Nikita Misiura
//
// This software is available under Attribution-NonCommercial-ShareAlike 4.0
// International Creative Commons license (CC BY-NC-SA 4.0):
// 
// https://creativecommons.org/licenses/by-nc-sa/4.0/
// 
// In short: You are free to use and redistribute the code in any medium or
// format, but only under the same license terms. You can transform and build
// your projects upon it. You can NOT use the code for commercial purposes. You
// must give appropriate credit for usage of the code.
// 
// This product is based on software from the PixInsight project, developed by
// Pleiades Astrophoto and its contributors:
// 
// https://pixinsight.com/
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  1
#define MODULE_VERSION_BUILD     24
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2021
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       9

#include "StarNetModule.h"
#include "StarNetInterface.h"
#include "StarNetProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

StarNetModule::StarNetModule()
{
}

// ----------------------------------------------------------------------------

const char* StarNetModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString StarNetModule::Name() const
{
   return "StarNet";
}

// ----------------------------------------------------------------------------

String StarNetModule::Description() const
{
   return "PixInsight StarNet Process Module";
}

// ----------------------------------------------------------------------------

String StarNetModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String StarNetModule::Author() const
{
   return "Nikita Misiura";
}

// ----------------------------------------------------------------------------

String StarNetModule::Copyright() const
{
   return "Copyright (c) 2018-2020 Nikita Misiura";
}

// ----------------------------------------------------------------------------

String StarNetModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String StarNetModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "StarNet-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "StarNet-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "StarNet-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "StarNet-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void StarNetModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void StarNetModule::OnUnload()
{
   if ( TheStarNetProcess != nullptr ) // cannot be false
      if ( TheStarNetProcess->PreferencesLoaded() )
         TheStarNetProcess->SavePreferences();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::StarNetModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::StarNetProcess;
      new pcl::StarNetInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF StarNetModule.cpp - Released 2021-04-09T19:41:49Z
