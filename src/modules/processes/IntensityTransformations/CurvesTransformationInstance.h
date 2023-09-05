//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.1
// ----------------------------------------------------------------------------
// CurvesTransformationInstance.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __CurvesTransformationInstance_h
#define __CurvesTransformationInstance_h

#include <pcl/ProcessImplementation.h>

#include "CurveBase.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class Curve : public CurveBase
{
public:

   typedef CurveBase::input_list    input_list;
   typedef CurveBase::output_list   output_list;
   typedef CurveBase::interpolator  interpolator;

   Curve()
   {
      Initialize();
   }

   Curve( const Curve& ) = default;
   Curve& operator =( const Curve& ) = default;

   void Reverse() override
   {
      for ( size_type i = 0; i < x.Length(); ++i )
         y[i] = 1 - y[i];
   }

   bool IsIdentity() const override
   {
      for ( size_type i = 0; i < x.Length(); ++i )
         if ( x[i] != y[i] )
            return false;
      return true;
   }

private:

   void Initialize() override
   {
      x << 0.0 << 1.0;
      y << 0.0 << 1.0;
   }
};

// ----------------------------------------------------------------------------

class CurvesTransformationInstance : public ProcessImplementation
{
public:

   typedef Curve::interpolator   interpolator;

   CurvesTransformationInstance( const MetaProcess* );
   CurvesTransformationInstance( const CurvesTransformationInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool Validate( String& info ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   const Curve& operator[]( int curveIdx ) const
   {
      return C[curveIdx];
   }

   Curve& operator[]( int curveIdx )
   {
      return C[curveIdx];
   }

   void Preview( UInt16Image& ) const;

private:

   Curve C[ CurveIndex::NumberOfCurves ];
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CurvesTransformationInstance_h

// ----------------------------------------------------------------------------
// EOF CurvesTransformationInstance.h - Released 2023-08-28T15:23:41Z
