//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard StarNet Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// StarNetParameters.cpp - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarNet PixInsight module.
//
// Copyright (c) 2018-2020 Nikita Misiura
//
// This software is available under Attribution-NonCommercial-ShareAlike 4.0
// International Creative Commons license (CC BY-NC-SA 4.0):
// 
// https://creativecommons.org/licenses/by-nc-sa/4.0/
// 
// In short: You are free to use and redistribute the code in any medium or
// format, but only under the same license terms. You can transform and build
// your projects upon it. You can NOT use the code for commercial purposes. You
// must give appropriate credit for usage of the code.
// 
// This product is based on software from the PixInsight project, developed by
// Pleiades Astrophoto and its contributors:
// 
// https://pixinsight.com/
// ----------------------------------------------------------------------------

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
   case Stride_128:
      return "Stride_128";
   case Stride_64:
      return "Stride_64";
   case Stride_32:
      return "Stride_32";
   case Stride_16:
      return "Stride_16";
   case Stride_8:
      return "Stride_8";
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

// ----------------------------------------------------------------------------
// EOF StarNetParameters.cpp - Released 2021-04-09T19:41:49Z
