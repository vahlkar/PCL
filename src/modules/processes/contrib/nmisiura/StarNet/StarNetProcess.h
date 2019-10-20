#ifndef __StarNetProcess_h
#define __StarNetProcess_h

#include <pcl/MetaProcess.h>

namespace pcl {
    class StarNetProcess : public MetaProcess {
    public:
       StarNetProcess();

       virtual IsoString Id() const;
       virtual IsoString Category() const;

       virtual uint32 Version() const;

       virtual String Description() const;

       virtual const char** IconImageXPM() const;

       virtual ProcessInterface* DefaultInterface() const;

       virtual ProcessImplementation* Create() const;
       virtual ProcessImplementation* Clone(const ProcessImplementation&) const;

       virtual bool CanProcessCommandLines() const;
       virtual int ProcessCommandLine(const StringList&) const;
    };

    PCL_BEGIN_LOCAL
    extern StarNetProcess* TheStarNetProcess;
    PCL_END_LOCAL
}
#endif