#ifndef __StarNetParameters_h
#define __StarNetParameters_h

#include <pcl/MetaParameter.h>

namespace pcl {
    PCL_BEGIN_LOCAL
    class StarNetStride : public MetaEnumeration {
    public:
        enum {itemOne,
              itemTwo,
              itemThree,
              itemFour,
              itemFive,
              NumberOfItems,
              Default = itemOne};

        StarNetStride(MetaProcess*);
        
        virtual IsoString Id() const;
        
        virtual size_type NumberOfElements() const;
        virtual IsoString ElementId(size_type) const;
        virtual int ElementValue(size_type) const;
        virtual size_type DefaultValueIndex() const;
    };
    
    extern StarNetStride* TheStrideParameter;
    
    class StarNetMask : public MetaBoolean {
    public:

        StarNetMask(MetaProcess*);
        virtual IsoString Id() const;
        virtual bool DefaultValue() const;
    };
    
    extern StarNetMask* TheMaskParameter;
    PCL_END_LOCAL
}
#endif