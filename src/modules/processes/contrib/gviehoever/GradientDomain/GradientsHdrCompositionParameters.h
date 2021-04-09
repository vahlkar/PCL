//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 0.6.4
// ----------------------------------------------------------------------------
// GradientsHdrCompositionParameters.h - Released 2021-04-09T19:41:49Z
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

#ifndef __GradientsHdrCompositionParameters_h
#define __GradientsHdrCompositionParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class GradientsHdrCompositionTargetFrames : public MetaTable
{
public:

   GradientsHdrCompositionTargetFrames( MetaProcess* );

   IsoString Id() const override;
};

extern GradientsHdrCompositionTargetFrames* TheGradientsHdrCompositionTargetFramesParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionTargetFrameEnabled : public MetaBoolean
{
public:

   GradientsHdrCompositionTargetFrameEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern GradientsHdrCompositionTargetFrameEnabled* TheGradientsHdrCompositionTargetFrameEnabledParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionTargetFramePath : public MetaString
{
public:

   GradientsHdrCompositionTargetFramePath( MetaTable* );

   IsoString Id() const override;
};

extern GradientsHdrCompositionTargetFramePath* TheGradientsHdrCompositionTargetFramePathParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionLogBias : public MetaDouble
{
public:

   GradientsHdrCompositionLogBias( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern GradientsHdrCompositionLogBias* TheGradientsHdrCompositionLogBiasParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionKeepLog : public MetaBoolean
{
public:

   GradientsHdrCompositionKeepLog( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern GradientsHdrCompositionKeepLog* TheGradientsHdrCompositionKeepLogParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionNegativeBias : public MetaBoolean
{
public:

   GradientsHdrCompositionNegativeBias( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern GradientsHdrCompositionNegativeBias* TheGradientsHdrCompositionNegativeBiasParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionGenerateMask : public MetaBoolean
{
public:

   GradientsHdrCompositionGenerateMask( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern GradientsHdrCompositionGenerateMask* TheGradientsHdrCompositionGenerateMaskParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif

// ----------------------------------------------------------------------------
// EOF GradientsHdrCompositionParameters.h - Released 2021-04-09T19:41:49Z
