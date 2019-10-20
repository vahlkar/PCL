#define MODULE_VERSION_MAJOR     00
#define MODULE_VERSION_MINOR     01
#define MODULE_VERSION_REVISION  01
#define MODULE_VERSION_BUILD     0015
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2019
#define MODULE_RELEASE_MONTH     5
#define MODULE_RELEASE_DAY       30

#include "StarNetModule.h"
#include "StarNetProcess.h"
#include "StarNetInterface.h"

namespace pcl {
    StarNetModule::StarNetModule() : MetaModule() {
    }

    const char* StarNetModule::Version() const {
        return PCL_MODULE_VERSION(MODULE_VERSION_MAJOR,
                                  MODULE_VERSION_MINOR,
                                  MODULE_VERSION_REVISION,
                                  MODULE_VERSION_BUILD,
                                  MODULE_VERSION_LANGUAGE);
    }

    IsoString StarNetModule::Name() const {
        return "StarNet";
    }

    String StarNetModule::Description() const {
        return "PixInsight StarNet Process Module"; // Replace with your own description
    }

    String StarNetModule::Company() const {
        return "none";
    }

    String StarNetModule::Author() const {
        return "Nikita Misiura";
    }

    String StarNetModule::Copyright() const {
        return "Copyright (c) 2019 Nikita Misiura";
    }

    String StarNetModule::TradeMarks() const {
        return "";
    }

    String StarNetModule::OriginalFileName() const {
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

    void StarNetModule::GetReleaseDate(int& year, int& month, int& day) const {
        year  = MODULE_RELEASE_YEAR;
        month = MODULE_RELEASE_MONTH;
        day   = MODULE_RELEASE_DAY;
    }
}

// ----------------------------------------------------------------------------

/*
 * PCL_MODULE_EXPORT int InstallPixInsightModule(int mode)
 *
 * Module installation routine.
 *
 * If this routine is defined as a public symbol in a module, the PixInsight
 * core application will call it just after loading and initializing the module
 * shared object or dynamic-link library.
 *
 * The mode argument specifies the kind of installation being performed by the
 * core application. See the pcl::InstallMode namespace for more information.
 */
PCL_MODULE_EXPORT int InstallPixInsightModule(int mode) {
    /*
     * When the PixInsight application installs this module, we just have to
     * instantiate the meta objects describing it.
     */
    new pcl::StarNetModule;

    /*
     * The mode argument tells us which kind of installation is being requested
     * by the PixInsight application. Incomplete installation requests only need
     * module descriptions.
     */
    if (mode == pcl::InstallMode::FullInstall) {
        new pcl::StarNetProcess;
        new pcl::StarNetInterface;
    }

    /*
     * Return zero to signal successful installation
     */
    return 0;
}