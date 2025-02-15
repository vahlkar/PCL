//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// AdaptiveStretchInstance.h - Released 2024-12-28T16:54:15Z
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

#ifndef __AdaptiveStretchInstance_h
#define __AdaptiveStretchInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Vector.h>

#include "AdaptiveStretchParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Base class for the adaptive stretch curve.
 */
class StretchCurve : public FVector
{
public:

   StretchCurve() = default;
   StretchCurve( const StretchCurve& ) = default;

   StretchCurve( int n ) : FVector( n )
   {
   }

   /*
    * Returns a transformed value f in [0,1]
    */
   double operator ()( double f ) const
   {
      int n1 = Length() - 1;
      f *= n1;
      int a = TruncI( f );
      if ( a < n1 )
      {
         double fa = (*this)[a];
         return fa + (f - a)*((*this)[a+1] - fa);
      }
      return 1;
   }
};

// ----------------------------------------------------------------------------

class AdaptiveStretchInstance : public ProcessImplementation
{
public:

   AdaptiveStretchInstance( const MetaProcess* );
   AdaptiveStretchInstance( const AdaptiveStretchInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

   // Reimplemented to launch the auxiliary graph interface.
   ProcessInterface* SelectInterface() const override;

   StretchCurve Preview( UInt16Image&, const View& ) const;

private:

   double   p_noiseThreshold;
   double   p_protection;
   pcl_bool p_useProtection;
   int32    p_maxCurvePoints;
   pcl_bool p_useROI;
   Rect     p_roi = 0;

   friend class AdaptiveStretchCurve;
   friend class AdaptiveStretchEngine;
   friend class AdaptiveStretchProcess;
   friend class AdaptiveStretchInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AdaptiveStretchInstance_h

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchInstance.h - Released 2024-12-28T16:54:15Z
