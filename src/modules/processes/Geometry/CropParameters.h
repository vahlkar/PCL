//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// CropParameters.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __CropParameters_h
#define __CropParameters_h

#include "CommonParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CRLeftMargin : public MetaDouble
{
public:

   CRLeftMargin( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
};

extern CRLeftMargin* TheCRLeftMarginParameter;

// ----------------------------------------------------------------------------

class CRTopMargin : public MetaDouble
{
public:

   CRTopMargin( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
};

extern CRTopMargin* TheCRTopMarginParameter;

// ----------------------------------------------------------------------------

class CRRightMargin : public MetaDouble
{
public:

   CRRightMargin( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
};

extern CRRightMargin* TheCRRightMarginParameter;

// ----------------------------------------------------------------------------

class CRBottomMargin : public MetaDouble
{
public:

   CRBottomMargin( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
};

extern CRBottomMargin* TheCRBottomMarginParameter;

// ----------------------------------------------------------------------------

class CRMode : public MetaEnumeration
{
public:

   enum { RelativeMargins,       // margins relative to current image dimensions
          AbsolutePixels,        // absolute margins in pixels
          AbsoluteCentimeters,   // absolute margins in centimeters
          AbsoluteInches,        // absolute margins in inches
          NumberOfCRModes,
          Default = AbsolutePixels };

   CRMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern CRMode* TheCRModeParameter;

// ----------------------------------------------------------------------------

extern XResolution* TheCRXResolutionParameter;
extern YResolution* TheCRYResolutionParameter;
extern MetricResolution* TheCRMetricResolutionParameter;
extern ForceResolution* TheCRForceResolutionParameter;

extern FillRed* TheCRFillRedParameter;
extern FillGreen* TheCRFillGreenParameter;
extern FillBlue* TheCRFillBlueParameter;
extern FillAlpha* TheCRFillAlphaParameter;

extern NoGUIMessages* TheCRNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __CropParameters_h

// ----------------------------------------------------------------------------
// EOF CropParameters.h - Released 2021-04-09T19:41:48Z
