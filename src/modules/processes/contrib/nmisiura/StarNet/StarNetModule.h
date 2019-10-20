#ifndef __StarNetModule_h
#define __StarNetModule_h

#include <pcl/MetaModule.h>

namespace pcl {
    class StarNetModule : public MetaModule {
    public:
        StarNetModule();
        virtual const char* Version() const;
        virtual IsoString Name() const;
        virtual String Description() const;
        virtual String Company() const;
        virtual String Author() const;
        virtual String Copyright() const;
        virtual String TradeMarks() const;
        virtual String OriginalFileName() const;
        virtual void GetReleaseDate(int& year, int& month, int& day) const;
    };
}
#endif