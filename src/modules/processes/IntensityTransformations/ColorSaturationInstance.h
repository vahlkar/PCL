//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ColorSaturationInstance.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2024 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ColorSaturationInstance_h
#define __ColorSaturationInstance_h

#include <pcl/ProcessImplementation.h>

#include "CurveBase.h"
#include "ColorSaturationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class HSCurve : public CurveBase
{
public:

   typedef CurveBase::input_list    input_list;
   typedef CurveBase::output_list   output_list;
   typedef CurveBase::interpolator  interpolator;

   HSCurve()
   {
      Initialize();
   }

   HSCurve( const HSCurve& ) = default;
   HSCurve& operator =( const HSCurve& ) = default;

   void Reverse() override
   {
      for ( size_type i = 0; i < x.Length(); ++i )
         y[i] = -y[i];
   }

   bool IsIdentity() const override
   {
      for ( size_type i = 0; i < x.Length(); ++i )
         if ( y[i] + 1 != 1 )
            return false;
      return true;
   }

   static double Interpolate( interpolator* i, double x )
   {
      return Range( (*i)( x ), -10.0, 10.0 );
   }

private:

   void Initialize() override
   {
      x << 0.0 << 1.0;
      y << 0.0 << 0.0;
   }
};

// ----------------------------------------------------------------------------

class ColorSaturationInstance : public ProcessImplementation
{
public:

   ColorSaturationInstance( const MetaProcess* );
   ColorSaturationInstance( const ColorSaturationInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   const HSCurve& Curve() const
   {
      return C;
   }

   template <typename T> T ShiftHueValue( T f ) const
   {
      if ( f > hueShift )
         f -= hueShift;
      else
         f += 1 - hueShift;
      return f;
   }

   template <typename T> T UnshiftHueValue( T f ) const
   {
      if ( f > 1 - hueShift )
         f -= 1-hueShift;
      else
         f += hueShift;
      return f;
   }

   void Preview( UInt16Image& ) const;

private:

   HSCurve C;
   float   hueShift = 0;

   friend class ColorSaturationInterface;
   friend class ColorSaturationProcess;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ColorSaturationInstance_h

// ----------------------------------------------------------------------------
// EOF ColorSaturationInstance.h - Released 2024-12-28T16:54:15Z
