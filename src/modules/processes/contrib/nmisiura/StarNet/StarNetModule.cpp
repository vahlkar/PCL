#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     23
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2020
#define MODULE_RELEASE_MONTH     8
#define MODULE_RELEASE_DAY       14

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
   return "Copyright (c) 2019-2020 Nikita Misiura";
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
