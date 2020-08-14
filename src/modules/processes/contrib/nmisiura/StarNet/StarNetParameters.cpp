#include "StarNetParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SNCreateMask* TheSNCreateMaskParameter = nullptr;
SNStride*     TheSNStrideParameter = nullptr;

// ----------------------------------------------------------------------------

SNCreateMask::SNCreateMask( MetaProcess* P )
   : MetaBoolean( P )
{
   TheSNCreateMaskParameter = this;
}

IsoString SNCreateMask::Id() const
{
   return "mask";
}

bool SNCreateMask::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SNStride::SNStride( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheSNStrideParameter = this;
}

IsoString SNStride::Id() const
{
   return "stride";
}

size_type SNStride::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SNStride::ElementId( size_type i ) const
{
   switch ( i )
   {
   default: // ?!
   case itemOne:
      return "itemOne";
   case itemTwo:
      return "itemTwo";
   case itemThree:
      return "itemThree";
   case itemFour:
      return "itemFour";
   case itemFive:
      return "itemFive";
   }
}

int SNStride::ElementValue( size_type i ) const
{
   return int( i );
}

size_type SNStride::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl
