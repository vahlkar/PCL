//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// pcl/PSFScaleEstimator.cpp - Released 2024-06-18T15:49:06Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Constants.h>
#include <pcl/LinearFit.h>
#include <pcl/PSFScaleEstimator.h>
#include <pcl/QuadTree.h>
#include <pcl/SurfaceSimplifier.h>

namespace pcl
{

// ----------------------------------------------------------------------------

int PSFScaleEstimator::SetReference( const ImageVariant& image )
{
   m_psfReference = FitStars( image );
   return int( m_psfReference.Length() );
}

// ----------------------------------------------------------------------------

/*
 * The iterative sigma clipping rejection with Chauvenet criterion implements
 * an algorithm based on:
 *
 * Maples, M. P., Reichart, D. E. et al., Robust Chauvenet Outlier Rejection,
 * Astrophysical Journal Supplement Series, 2018, ApJS, 238, A2, 1 - 49
 */

/*
 * The Q-function: the probability for a value from a normal distribution of
 * being more than x standard deviations from the mean.
 */
inline static double QF( double x )
{
   return 0.5*(1 - Erf( x/Const<double>::sqrt2() ));
}

/*
 * Correction factor for 68.3-percentile deviations to avoid overaggressive
 * rejection for small samples.
 */
inline static double FN( int N )
{
   return 1/(1 - 2.9442*Pow( double( N ), -1.073 ));
}

/*
 * The 68.3-percentile value from the distribution of absolute deviations.
 */
template <class sample>
inline static double SampleDeviation( const sample& R, int i, int j, double m )
{
   int N = j - i;
   Vector D( N );
   for ( int k = 0; i < j; ++i, ++k )
      D[k] = Abs( double( R[i] ) - m );
   return FN( N ) * *pcl::Select( D.Begin(), D.End(), TruncInt( 0.683*D.Length() ) );
}

/*
 * 68.3-percentile deviation by fitting a zero-intercept line to the vector of
 * absolute differences.
 */
template <class sample>
inline static double LineFitDeviation( const sample& R, int i, int j, double m )
{
   int N = j - i;
   int n = int( 0.683*N + 0.317 );
   if ( n < 8 )
      return SampleDeviation( R, i, j, m );

   Vector y( N );
   for ( int k = 0, ii = i; ii < j; ++ii, ++k )
      y[k] = Abs( double( R[ii] ) - m );
   y.Sort();
   y = Vector( y.Begin(), n );

   Vector x( n );
   for ( int i = 0; i < n; ++i )
      x[i] = Const<double>::sqrt2() * ErfInv( (i + 1 - 0.317)/N );

   double s;
   LinearFit f( x, y );
   if ( f.IsValid() )
      s = FN( N ) * f( 1.0 );
   else
      s = SampleDeviation( R, i, j, m );

   return s;
}

PSFScaleEstimator::Estimates PSFScaleEstimator::EstimateScale( const ImageVariant& image ) const
{
   if ( m_psfReference.IsEmpty() )
      throw Error( "PSFScaleEstimator::EstimateScale(): Internal error: No reference image has been defined" );

   Estimates E;

   /*
    * Detect sources and fit PSF models.
    */
   Array<PSFData> psfs = FitStars( image );

   /*
    * Star matching by proximity search.
    */
   QuadTree<PSFData> T2( psfs );
   Array<PSFData> P1, P2, Q1, Q2;
   for ( int it = 0;; ++it )
   {
      for ( const PSFData& p1 : m_psfReference )
      {
         const DPoint& c1 = (it == 0) ? p1.c0 : p1.b0;

         QuadTree<PSFData>::point_list p2 =
            T2.Search( QuadTree<PSFData>::rectangle( c1.x - m_psfSearchTolerance, c1.y - m_psfSearchTolerance,
                                                     c1.x + m_psfSearchTolerance, c1.y + m_psfSearchTolerance ) );
         if ( !p2.IsEmpty() )
         {
            size_type j = 0;
            if ( p2.Length() > 1 )
            {
               double d2 = p2[0].c0.SquaredDistanceTo( c1 );
               for ( size_type i = 1; i < p2.Length(); ++i )
               {
                  double d2i = p2[i].c0.SquaredDistanceTo( c1 );
                  if ( d2i < d2 )
                  {
                     j = i;
                     d2 = d2i;
                  }
               }
            }

            if ( it == 0 )
            {
               P1 << p1;
               P2 << p2[j];
            }
            else
            {
               Q1 << p1;
               Q2 << p2[j];
            }
         }
      }

      if ( it == 0 )
      {
         if ( P1.Length() > 0.8*psfs.Length() )
            break;

         for ( PSFData& p2 : psfs )
            p2.c0 = p2.b0;
         T2.Build( psfs );
      }
      else
      {
         if ( Q1.Length() > P1.Length() )
         {
            P1 = Q1;
            P2 = Q2;
         }

         break;
      }
   }

   if ( !P1.IsEmpty() )
   {
      /*
       * Gather relative scale samples.
       */
      sample_vector R( int( P1.Length() ) );
      for ( int i = 0; i < R.Length(); ++i )
      {
         R[i].x = P1[i].c0.x;
         R[i].y = P1[i].c0.y;
         R[i].z = P1[i].signal/P2[i].signal;
      }

      /*
       * Robust Chauvenet rejection.
       */
      R.Sort();
      int i = 0;
      int j = R.Length();
      double m, s;

      for ( int phase = 0; phase < 3; ++phase )
         for ( ;; )
         {
            switch ( phase )
            {
            case 0: // + robustness / - precision
               m = Median( R.At( i ), R.At( j ) );
               s = LineFitDeviation( R, i, j, m );
               break;
            case 1: // = robustness / = precision
               m = Median( R.At( i ), R.At( j ) );
               s = SampleDeviation( R, i, j, m );
               break;
            case 2: // - robustness / + precision
               m = Mean( R.At( i ), R.At( j ) );
               s = StdDev( R.At( i ), R.At( j ), m );
               break;
            }

            if ( 1 + s == 1 )
               goto __rcr_end;

            int n = j - i;
            if ( n < 3 )
               goto __rcr_end;

            double d0 = n*QF( (m - double( R[i] ))/s );
            double d1 = n*QF( (double( R[j-1] ) - m)/s );
            if ( d0 >= m_rejectionLimit )
               if ( d1 >= m_rejectionLimit )
                  break;
            if ( d1 < d0 )
               --j;
            else
               ++i;
         }

__rcr_end:
      E.scale = m;
      E.sigma = s;
      E.total = int( psfs.Length() );
      E.count = j - i;

      if ( m_enableLocalModel )
      {
         Array<double> x, y, z;
         for ( int k = i, n = 0; k < j; ++k, ++n )
         {
            x << R[i+n].x;
            y << R[i+n].y;
            z << R[i+n].z - E.scale;
         }

         double zs = StdDev( z.Begin(), z.End() );

         SurfaceSimplifier SS;
         SS.EnableRejection();
         SS.SetRejectFraction( 0.1 );
         SS.EnableCentroidInclusion();

         Array<double> sx, sy, sz;
         SS.SetTolerance( 3*zs );
         SS.Simplify( sx, sy, sz, x, y, z );

         if ( sx.Length() > 3 )
            if ( sx.Length() <= 2100 )
            {
               E.local.SetSmoothing( 5*zs );
               E.local.Initialize( sx.Begin(), sy.Begin(), sz.Begin(), int( sx.Length() ) );
            }
      }
   }

   return E;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/PSFScaleEstimator.cpp - Released 2024-06-18T15:49:06Z
