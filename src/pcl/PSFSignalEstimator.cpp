//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.18
// ----------------------------------------------------------------------------
// pcl/PSFSignalEstimator.cpp - Released 2022-01-18T11:02:48Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2022 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/PSFSignalEstimator.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PSFSignalEstimator::Estimates PSFSignalEstimator::EstimateSignal( const ImageVariant& image ) const
{
   Estimates E;

   Array<PSFData> psfs = FitStars( image );

   if ( !psfs.IsEmpty() )
   {
      /*
       * Gather PSF fluxes and accumulate mean flux.
       */
      int t = int( psfs.Length() );
      Vector psfFlux( t );
      Vector psfFlux2( t );
      Vector psfMean( t );
      for ( int i = 0; i < t; ++i )
      {
         double s = psfs[i].signal;
         psfFlux[i] = s;
         psfFlux2[i] = s*s;
         psfMean[i] = s/psfs[i].signalCount;
      }

      /*
       * Accumulate total fluxes using a stable summation algorithm to minimize
       * roundoff errors. Note that we can have tens of thousands of PSF signal
       * estimates here.
       */
      E.flux = psfFlux.StableSum();
      E.powerFlux = psfFlux2.StableSum();

      /*
       * PSF signal estimates. The penalty function is the ratio of the sum of
       * average PSF intensities to a low norm of the image.
       */
      image.PushSelections();
      image.SetRangeClipping( 0, 0.9 );
      image.SetRangeClipping( 0, image.OrderStatistic( m_clipHigh ) );
      double P = psfMean.StableSum()/image.Norm();
      image.PopSelections();
      E.mean = E.flux * P;
      E.power = E.powerFlux * P;
      E.count = t;
   }

   return E;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/PSFSignalEstimator.cpp - Released 2022-01-18T11:02:48Z
