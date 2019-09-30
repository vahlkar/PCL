#ifndef __StarNetInstance_h
#define __StarNetInstance_h

#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl {
    class StarNetInstance : public ProcessImplementation {
    public:
       StarNetInstance(const MetaProcess*);
       StarNetInstance(const StarNetInstance&);

       virtual void Assign(const ProcessImplementation&);

       virtual bool CanExecuteOn(const View&, pcl::String& whyNot) const;
       virtual bool ExecuteOn(View&);

       virtual void* LockParameter(const MetaParameter*, size_type tableRow);
       virtual bool AllocateParameter(size_type sizeOrLength, const MetaParameter* p, size_type tableRow);
       virtual size_type ParameterLength(const MetaParameter* p, size_type tableRow) const;

    private:
       //float    p_one;       // Real parameters can be either float or double
       pcl_enum   p_stride;    // ONLY use intxx or uintxx - NEVER int, long, etc.
       pcl_bool   p_mask;      // ONLY pcl_bool for Boolean parameter - NEVER use bool
       //pcl_enum p_four;      // ONLY pcl_enum for enumerated parameters - NEVER use enum, int, etc.
       //String   p_five;      // String parameters must be UTF-16 strings

       friend class StarNetEngine;
       friend class StarNetProcess;
       friend class StarNetInterface;
    };
}
#endif