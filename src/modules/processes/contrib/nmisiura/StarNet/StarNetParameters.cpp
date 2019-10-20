#include "StarNetParameters.h"

namespace pcl {
    StarNetMask* TheMaskParameter = nullptr;
    StarNetStride*  TheStrideParameter = nullptr;
    
    StarNetMask::StarNetMask(MetaProcess* P) : MetaBoolean(P) {
        TheMaskParameter = this;
    }

    IsoString StarNetMask::Id() const {
        return "mask";
    }

    bool StarNetMask::DefaultValue() const {
        return false;
    }
    
    StarNetStride::StarNetStride(MetaProcess* P) : MetaEnumeration(P) {
        TheStrideParameter = this;
    }

    IsoString StarNetStride::Id() const {
        return "stride";
    }

    size_type StarNetStride::NumberOfElements() const {
        return NumberOfItems;
    }

    IsoString StarNetStride::ElementId(size_type i) const {
        switch (i) {
            default:
            case itemOne:    return "itemOne";
            case itemTwo:    return "itemTwo";
            case itemThree:  return "itemThree";
            case itemFour:   return "itemFour";
            case itemFive:   return "itemFive";
        }
    }

    int StarNetStride::ElementValue(size_type i) const {
       return int(i);
    }

    size_type StarNetStride::DefaultValueIndex() const {
       return Default;
    }
}