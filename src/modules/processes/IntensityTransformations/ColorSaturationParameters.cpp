//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0345
// ----------------------------------------------------------------------------
// ColorSaturationParameters.cpp - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
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

#include "ColorSaturationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

#define DEFINE_PARAMETER( classId )    classId* The##classId##Parameter = 0

DEFINE_PARAMETER( HueSaturationCurve );
DEFINE_PARAMETER( XHS );
DEFINE_PARAMETER( YHS );
DEFINE_PARAMETER( HueSaturationCurveType );

DEFINE_PARAMETER( HueShift );

// ----------------------------------------------------------------------------

HSCurvePointValue::HSCurvePointValue( MetaTable* t ) : CurvePointValue( t )
{
}

double HSCurvePointValue::MinimumValue() const
{
   return -10.0;
}

double HSCurvePointValue::MaximumValue() const
{
   return 10.0;
}

// ----------------------------------------------------------------------------

HueShift::HueShift( MetaProcess* P ) : MetaFloat( P )
{
   TheHueShiftParameter = this;
}

IsoString HueShift::Id() const
{
   return "hueShift";
}

int HueShift::Precision() const
{
   return 3;
}

double HueShift::MinimumValue() const
{
   return 0;
}

double HueShift::MaximumValue() const
{
   return 1;
}

double HueShift::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorSaturationParameters.cpp - Released 2015/11/26 16:00:13 UTC
