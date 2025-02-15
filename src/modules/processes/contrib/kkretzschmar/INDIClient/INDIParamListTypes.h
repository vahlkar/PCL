//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIParamListTypes.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2024 Klaus Kretzschmar
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (https://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#ifndef __INDIParamListTypes_h
#define __INDIParamListTypes_h

#include <pcl/Array.h>
#include <pcl/String.h>

#include "IndigoDefs.h"

#include <indigo/indigo_bus.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct INDIDeviceListItem
{
   String DeviceName;
   String DeviceLabel;

   bool operator==( const INDIDeviceListItem& rhs ) const
   {
      return DeviceName == rhs.DeviceName;
   }

   bool operator<( const INDIDeviceListItem& rhs ) const
   {
      return DeviceName < rhs.DeviceName;
   }
};

struct INDIPropertyListItem
{
   String Device;
   String Property;
   indigo_property_type PropertyType;
   String PropertyTypeStr;
   String Element;
   indigo_property_state PropertyState;
   String PropertyNumberFormat;
   String PropertyLabel;
   String ElementLabel;
   String PropertyKey;
   String PropertyValue;
   String PropertyTarget;
   String NewPropertyValue;

   bool operator==( const INDIPropertyListItem& rhs ) const
   {
      return PropertyKey == rhs.PropertyKey;
   }

   bool operator<( const INDIPropertyListItem& rhs ) const
   {
      return PropertyKey < rhs.PropertyKey;
   }
};

struct BelongsToSameDevice
{
   bool operator()( const INDIPropertyListItem& left, const INDIPropertyListItem& right )
   {
      return left.Device == right.Device;
   }
};

struct ElementValue
{
   String Element;
   String Value;

   template <typename T>
   ElementValue( const String& element, const T& value )
      : Element( element )
      , Value( String( value ) )
   {
   }

   bool operator==( const ElementValue& rhs ) const
   {
      return Element == rhs.Element && Value == rhs.Value;
   }

   bool operator<( const ElementValue& rhs ) const
   {
      return (Element != rhs.Element) ? Element < rhs.Element : Value < rhs.Value;
   }
};

struct INDINewPropertyItem
{
   typedef Array<ElementValue> element_value_list;

   String Device;
   String Property;
   String PropertyKey;
   String PropertyType;
   element_value_list ElementValues;

   INDINewPropertyItem() = default;

   INDINewPropertyItem( const String& device, const String& property, const String& type )
      : Device( device )
      , Property( property )
      , PropertyType( type )
   {
   }

   template <typename T>
   INDINewPropertyItem( const String& device, const String& property, const String& type,
                        const String& element, const T& value )
      : Device( device )
      , Property( property )
      , PropertyType( type )
   {
      ElementValues << ElementValue( element, value );
   }

   template <typename T1, typename T2>
   INDINewPropertyItem( const String& device, const String& property, const String& type,
                        const String& element1, const T1& value1,
                        const String& element2, const T2& value2 )
      : Device( device )
      , Property( property )
      , PropertyType( type )
   {
      ElementValues << ElementValue( element1, value1 )
                    << ElementValue( element2, value2 );
   }

   bool operator==( const INDINewPropertyItem& rhs ) const
   {
      return Device == rhs.Device
          && Property == rhs.Property
          && PropertyKey == rhs.PropertyKey
          && PropertyType == rhs.PropertyType
          && ElementValues == rhs.ElementValues;
   }
};

struct INDINewPropertyListItem
{
   String PropertyKey;
   String PropertyType;
   String NewPropertyValue;

   bool operator==( const INDINewPropertyListItem& rhs ) const
   {
      return PropertyKey == rhs.PropertyKey
          && PropertyType == rhs.PropertyType
          && NewPropertyValue == rhs.NewPropertyValue;
   }
};

typedef Array<INDIDeviceListItem> INDIDeviceListItemArray;
typedef Array<INDIPropertyListItem> INDIPropertyListItemArray;
typedef Array<INDINewPropertyListItem> INDINewPropertyListItemArray;

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __INDIParamListTypes_h

// ----------------------------------------------------------------------------
// EOF INDIParamListTypes.h - Released 2024-12-28T16:54:15Z
