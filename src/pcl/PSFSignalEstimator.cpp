//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.23
// ----------------------------------------------------------------------------
// pcl/PSFSignalEstimator.cpp - Released 2022-03-12T18:59:35Z
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

#include <pcl/GridInterpolation.h>
#include <pcl/MultiscaleMedianTransform.h>
#include <pcl/PSFSignalEstimator.h>
#include <pcl/RobustChauvenetRejection.h>
#include <pcl/ShepardInterpolation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Simplistic but foolproof detection of potential black regions at image
 * borders, e.g. those caused by alignment displacements.
 */
template <class P>
static bool HasZeroBorders( const GenericImage<P>& image )
{
   for ( int x = 0, n = 0; x < image.Width(); ++x )
      if ( image( x, 0 ) == 0 )
         if ( ++n > 3 )
            return true;
   for ( int x = 0, y = image.Height()-1, n = 0; x < image.Width(); ++x )
      if ( image( x, y ) == 0 )
         if ( ++n > 3 )
            return true;
   for ( int y = 0, n = 0; y < image.Height(); ++y )
      if ( image( 0, y ) == 0 )
         if ( ++n > 3 )
            return true;
   for ( int y = 0, x = image.Width()-1, n = 0; y < image.Height(); ++y )
      if ( image( x, y ) == 0 )
         if ( ++n > 3 )
            return true;

   return false;
}

/*
 * Replace black pixels with plausible values.
 */
static void FixZero( Image& image )
{
   const int w   = image.Width();
   const int h   = image.Height();
   const int d   = 32; // sampling delta
   const int d2  = d >> 1;
   const int dx0 = (w - w/d * d) >> 1;
   const int dy0 = (h - h/d * d) >> 1;

   image.PushSelections();
   image.SetRangeClipping( 0, 1 );

   for ( int c = 0; c < image.NumberOfChannels(); ++c )
   {
      Array<double> X, Y, Z;
      for ( int y = dy0; y < h; y += d )
         for ( int x = dx0; x < w; x += d )
            if ( image( x, y, c ) != 0 )
            {
               double z = image.Median( Rect( x-d2, y-d2, x+d2+1, y+d2+1 ), c, c, 1 );
               if ( 1 + z != 1 )
               {
                  X << x;
                  Y << y;
                  Z << z;
               }
            }

      if ( X.Length() < 16 )
         return; // !?

      ShepardInterpolation<double> S;
      S.SetRadius( 0.1 );
      S.Initialize( X.Begin(), Y.Begin(), Z.Begin(), int( X.Length() ) );

      GridInterpolation G;
      G.Initialize( image.Bounds(), 16/*delta*/, S, false/*verbose*/ );

      Image::sample_iterator r( image, c );
      for ( int y = 0; y < h; ++y )
         for ( int x = 0; x < w; ++x, ++r )
            if ( *r == 0 )
               *r = G( x, y );

      image.PopSelections();
   }
}

/*
 * The large-scale local background residual estimator.
 */
template <class P>
static Array<float> LocalBackgroundResidual_Imp( const GenericImage<P>& image, int scale, int maxThreads )
{
   /*
    * Accelerated Multiscale Median Transform with linear scaling sequences.
    *
    * The following values for the number of layers (n) and scaling increments
    * (d) have been found experimentally to be accurate to within a 0.1% of the
    * rigorous transforms with dyadic scaling sequences.
    *
    * N.B.: These optimizations are valid for the current PCL implementation of
    * the MMT algorithm. If the implementations change, these optimized
    * parameters will have to be re-evaluated through new tests.
    */
   int n, d;
   switch ( scale )
   {
   case 1024: n = 5; d = 80; break;
   case  768: n = 5; d = 60; break;
   case  512: n = 4; d = 55; break;
   case  384: n = 4; d = 40; break;
   default: // ?!
   case  256: n = 3; d = 40; break;
   case  192: n = 3; d = 30; break;
   case  128: n = 3; d = 20; break;
   case   64: n = 3; d = 10; break;
   case   32: n = 5; d =  0; break;
   }
   MultiscaleMedianTransform M( n, d );
   M.SetMaxProcessors( maxThreads );
   for ( int i = 0; i < M.NumberOfLayers(); ++i )
      M.DisableLayer( i );

   image.PushSelections();
   image.SelectChannel( image.SelectedChannel() );

   /*
    * If potential black borders are detected, replace them with statistically
    * plausible values to prevent border artifacts.
    */
   if ( HasZeroBorders( image ) )
   {
      Image fz( image );
      FixZero( fz );
      M << fz;
   }
   else
      M << image;

   /*
    * Gather the set of non-significant background pixels.
    */
   Array<float> R;
   Image::const_sample_iterator m( M[M.NumberOfLayers()] );
   for ( typename GenericImage<P>::const_sample_iterator i( image ); i; ++i, ++m )
   {
      float v; P::FromSample( v, *i );
      if ( v != 0 )
         if ( v < *m )
            R << *m - v;
   }

   image.PopSelections();

   return R;
}

Array<float> PSFSignalEstimator::LocalBackgroundResidual( const ImageVariant& image, int scale, int maxThreads )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: return LocalBackgroundResidual_Imp( static_cast<const Image&>( *image ), scale, maxThreads );
      case 64: return LocalBackgroundResidual_Imp( static_cast<const DImage&>( *image ), scale, maxThreads );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: return LocalBackgroundResidual_Imp( static_cast<const UInt8Image&>( *image ), scale, maxThreads );
      case 16: return LocalBackgroundResidual_Imp( static_cast<const UInt16Image&>( *image ), scale, maxThreads );
      case 32: return LocalBackgroundResidual_Imp( static_cast<const UInt32Image&>( *image ), scale, maxThreads );
      }
   return Array<float>(); // !?
}

// ----------------------------------------------------------------------------

PSFSignalEstimator::Estimates PSFSignalEstimator::EstimateSignal( const ImageVariant& image ) const
{
   Estimates E;

   /*
    * Detect sources and fit PSF models.
    */
   Array<PSFData> psfs = FitStars( image );

   if ( !psfs.IsEmpty() )
   {
      /*
       * Gather PSF fluxes and accumulate mean flux.
       */
      int n = int( psfs.Length() );
      Vector psfFlux( n );
      Vector psfFlux2( n );
      Vector psfMean( n );
      for ( int i = 0; i < n; ++i )
      {
         double s = psfs[i].signal;
         psfFlux[i] = s;
         psfFlux2[i] = s*s;
         // N.B. The fitted PSF area is more accurate and 'fine grained' than
         // the number of measured source pixels, especially for low FWHM.
         psfMean[i] = s/psfs[i].sx/psfs[i].sy; // instead of psfs[i].signalCount
      }

      /*
       * The vector of mean flux estimates requires outlier rejection. In rare
       * cases a few PSF fits can provide abnormally small sampling regions,
       * which can lead to wrongly high mean flux estimates. We Winsorize the
       * sample after robust Chauvenet rejection.
       */
      {
         double mean, sigma;
         int i, j;
         RobustChauvenetRejection()( i, j, mean, sigma, psfMean );
         for ( int k = 0; k < i; ++k )
            psfMean[k] = psfMean[i];
         for ( int k = --j; ++k < n; )
            psfMean[k] = psfMean[j];
      }

      /*
       * Vector of squared mean flux estimates.
       */
      Vector psfMean2 = psfMean;
      for ( double& x : psfMean2 )
         x *= x;

      /*
       * Accumulate total fluxes and mean fluxes using a stable summation
       * algorithm to minimize roundoff errors. Note that we can have tens of
       * thousands of PSF flux estimates here, and the range of values can be
       * considerably large.
       */
      E.totalFlux = psfFlux.StableSum();
      E.totalPowerFlux = psfFlux2.StableSum();
      E.totalMeanFlux = psfMean.StableSum();
      E.totalMeanPowerFlux = psfMean2.StableSum();
      E.count = n;

      /*
       * M* robust mean background estimate.
       * N* robust noise estimate.
       */
      Array<float> R = LocalBackgroundResidual( image, m_scale, MaxProcessors() );
      E.MStar = Median( R.Begin(), R.End() );
      E.NStar = 2.05434 * Sn( R.Begin(), R.End() );
//       E.NStar = 2.50573 * MAD( R.Begin(), R.End(), E.MStar );
   }

   return E;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/PSFSignalEstimator.cpp - Released 2022-03-12T18:59:35Z
