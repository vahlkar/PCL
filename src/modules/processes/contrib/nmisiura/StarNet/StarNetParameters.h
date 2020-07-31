#ifndef __StarNetParameters_h
#define __StarNetParameters_h

#include <pcl/MetaParameter.h>

namespace pcl {
    PCL_BEGIN_LOCAL
    class StarNetStride : public MetaEnumeration {
    public:
        enum { itemOne,
               itemTwo,
               itemThree,
               itemFour,
               itemFive,
               NumberOfItems,
               Default = itemOne };

        StarNetStride(MetaProcess*);

        IsoString Id() const override;
        size_type NumberOfElements() const override;
        IsoString ElementId( size_type ) const override;
        int ElementValue( size_type ) const override;
        size_type DefaultValueIndex() const override;
    };

    extern StarNetStride* TheStrideParameter;

    class StarNetMask : public MetaBoolean {
    public:

        StarNetMask(MetaProcess*);
        IsoString Id() const override;
        bool DefaultValue() const override;
    };

    extern StarNetMask* TheMaskParameter;
    PCL_END_LOCAL
}
#endif
