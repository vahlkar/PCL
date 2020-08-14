#ifndef __StarNetParameters_h
#define __StarNetParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SNStride : public MetaEnumeration
{
public:

   enum
   {
      itemOne,
      itemTwo,
      itemThree,
      itemFour,
      itemFive,
      NumberOfItems,
      Default = itemOne
   };

   SNStride( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern SNStride* TheSNStrideParameter;

// ----------------------------------------------------------------------------

class SNCreateMask : public MetaBoolean
{
public:

   SNCreateMask( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SNCreateMask* TheSNCreateMaskParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __StarNetParameters_h
