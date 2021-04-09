//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 0.6.4
// ----------------------------------------------------------------------------
// GradientsMergeMosaicParameters.h - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2020. Licensed under LGPL 2.1
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#ifndef __GradientsMergeMosaicParameters_h
#define __GradientsMergeMosaicParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class GradientsMergeMosaicTargetFrames : public MetaTable
{
public:

   GradientsMergeMosaicTargetFrames( MetaProcess* );

   IsoString Id() const override;
};

extern GradientsMergeMosaicTargetFrames* TheGradientsMergeMosaicTargetFramesParameter;

// ----------------------------------------------------------------------------

class GradientsMergeMosaicTargetFrameEnabled : public MetaBoolean
{
public:

   GradientsMergeMosaicTargetFrameEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern GradientsMergeMosaicTargetFrameEnabled* TheGradientsMergeMosaicTargetFrameEnabledParameter;

// ----------------------------------------------------------------------------

class GradientsMergeMosaicTargetFramePath : public MetaString
{
public:

   GradientsMergeMosaicTargetFramePath( MetaTable* );

   IsoString Id() const override;
};

extern GradientsMergeMosaicTargetFramePath* TheGradientsMergeMosaicTargetFramePathParameter;

// ----------------------------------------------------------------------------

class GradientsMergeMosaicType : public MetaEnumeration
{
public:

   typedef enum
   {
      Overlay,
      Average,
      NumberOfItems,
      Default = Average
   } eType_t;

   GradientsMergeMosaicType( MetaProcess* );

   IsoString Id() const override;

   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern GradientsMergeMosaicType* TheGradientsMergeMosaicTypeParameter;

// ----------------------------------------------------------------------------

/// this is the radius of an erosion mask that reduces the size of the actual image region
class GradientsMergeMosaicShrinkCount : public MetaInt32
{
public:

   GradientsMergeMosaicShrinkCount( MetaProcess* );

   IsoString Id() const override;

   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern GradientsMergeMosaicShrinkCount* TheGradientsMergeMosaicShrinkCountParameter;

// ----------------------------------------------------------------------------

/// this is the radius of convolution that feathers the borders of an image
class GradientsMergeMosaicFeatherRadius : public MetaInt32
{
public:

   GradientsMergeMosaicFeatherRadius( MetaProcess* );

   IsoString Id() const override;

   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern GradientsMergeMosaicFeatherRadius* TheGradientsMergeMosaicFeatherRadiusParameter;

// ----------------------------------------------------------------------------

class GradientsMergeMosaicBlackPoint : public MetaDouble
{
public:

   GradientsMergeMosaicBlackPoint( MetaProcess* );

   IsoString Id() const override;

   int Precision() const override;

   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern GradientsMergeMosaicBlackPoint* TheGradientsMergeMosaicBlackPointParameter;

// ----------------------------------------------------------------------------

class GradientsMergeMosaicGenerateMask : public MetaBoolean
{
public:

   GradientsMergeMosaicGenerateMask( MetaProcess* );

   IsoString Id() const override;

   bool DefaultValue() const override;
};

extern GradientsMergeMosaicGenerateMask* TheGradientsMergeMosaicGenerateMaskParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif

// ----------------------------------------------------------------------------
// EOF GradientsMergeMosaicParameters.h - Released 2021-04-09T19:41:49Z
