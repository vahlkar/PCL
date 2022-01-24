//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.19
// ----------------------------------------------------------------------------
// pcl/PSFScaleEstimator.cpp - Released 2022-01-24T22:43:35Z
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

#include <pcl/LinearFit.h>
#include <pcl/PSFScaleEstimator.h>
#include <pcl/QuadTree.h>

#define SQRT2  1.414213562373095

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
   return 0.5*(1 - Erf( x/SQRT2 ));
}

/*
 * The 68.3-percentile value from the distribution of absolute deviations.
 */
inline static double SampleDeviation( const Vector& R, int i, int j, double m )
{
   int N = j - i;
   Vector D( N );
   for ( int k = 0; i < j; ++i, ++k )
      D[k] = Abs( R[i] - m );
   return 1/(1 - 2.9442*Pow( double( N ), -1.073 ))
        * *pcl::Select( D.Begin(), D.End(), TruncInt( 0.683*D.Length() ) );
}

/*
 * 68.3-percentile deviation by fitting a zero-intercept line to the vector of
 * absolute differences.
 */
inline static double LineFitDeviation( const Vector& R, int i, int j, double m )
{
   int N = j - i;
   int n = int( 0.683*N + 0.317 );
   if ( n < 8 )
      return SampleDeviation( R, i, j, m );

   Vector y( N );
   for ( int k = 0, ii = i; ii < j; ++ii, ++k )
      y[k] = Abs( R[ii] - m );
   y.Sort();
   y = Vector( y.Begin(), n );

   Vector x( n );
   for ( int i = 0; i < n; ++i )
      x[i] = SQRT2 * ErfInv( (i + 1 - 0.317)/N );

   double s;
   LinearFit f( x, y );
   if ( f.IsValid() )
      s = 1/(1 - 2.9442*Pow( double( N ), -1.073 )) * f( 1.0 );
   else
      s = SampleDeviation( R, i, j, m );

   return s;
}

// inline static double BrokenLineFitDeviation( const Vector& R, int i, int j, double m )
// {
// std::cout << IsoString().Format( "s0 = %.8f\n", SampleDeviation( R, i, j, m ) );
//
//    int N = j - i;
//    if ( N < 50 )
//       return SampleDeviation( R, i, j, m );
//
//    Vector y( N );
//    for ( int k = 0; i < j; ++i, ++k )
//       y[k] = Abs( R[i] - m );
//    y.Sort();
//    int n = int( 0.683*N + 0.317 );
//    y = Vector( y.Begin(), n );
//
//    Vector x( n );
//    for ( int i = 0; i < n; ++i )
//       x[i] = SQRT2 * ErfInv( (i + 1 - 0.317)/N );
//
//    double dmin = DBL_MAX;
//    double s = 0;
//    for ( int i = 1; i < 8; ++i )
//    {
//       double p = i * 1.0/8;
//       Array<double> xl, yl, xh, yh;
//       for ( int j = 0; j < n; ++j )
//          if ( x[j] <= p )
//          {
//             xl << x[j];
//             yl << y[j];
//          }
//          else
//          {
//             xh << x[j];
//             yh << y[j];
//          }
//
//       if ( xl.Length() > 1 && xh.Length() > 1 )
//       {
//          LinearFit fl( xl, yl );
//          LinearFit fh( xh, yh );
//          if ( fl.IsValid() && fh.IsValid() )
//          {
//             double d = 0;
//             for ( int j = 0; j < n; ++j )
//                d += Abs( ((x[j] <= p) ? fl( x[j] ) : fh( x[j] )) - y[j] );
//             if ( d < dmin )
//             {
//                dmin = d;
//                s = fl( p );
//             }
//          }
//       }
//    }
//
// std::cout << IsoString().Format( "s1 = %.8f\n", s );
//
//    return s;
// }

PSFScaleEstimator::Estimates PSFScaleEstimator::EstimateScale( const ImageVariant& image ) const
{
   if ( m_psfReference.IsEmpty() )
      throw Error( "PSFScaleEstimator::EstimateScale(): Internal error: No reference image has been defined" );

   Estimates E;

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
      switch ( m_psfFittingMethod )
      {
      default:
      case ChauvenetSigmaClipping:
         {
            /*
             * Robust iterative sigma clipping with Chauvenet rejection
             * criterion. The rejection threshold parameter is not
             * necessary/used when this option is selected.
             */
            Vector R( P1.Length() );
            for ( size_type i = 0; i < P1.Length(); ++i )
               R[i] = P1[i].signal/P2[i].signal;
            int i = 0;
            int j = R.Length();
            double m = R.Median();
            double s = LineFitDeviation( R, i, j, m );
            R.Sort();
            for ( ;; )
            {
               if ( 1 + s == 1 )
                  goto __bad_sigma;
               int n = j - i;
               int c = 0;
               if ( n*QF( (m - R[i])/s ) < 0.5 )
                  ++i, ++c;
               if ( n*QF( (R[j-1] - m)/s ) < 0.5 )
                  --j, ++c;
               if ( c == 0 )
                  break;
               m = Median( R.At( i ), R.At( j ) );
               if ( j - i < 3 )
                  goto __bad_sigma;
               s = LineFitDeviation( R, i, j, m );
            }

            s = SampleDeviation( R, i, j, m );
            for ( ;; )
            {
               if ( 1 + s == 1 )
                  goto __bad_sigma;
               int n = j - i;
               int c = 0;
               if ( n*QF( (m - R[i])/s ) < 0.5 )
                  ++i, ++c;
               if ( n*QF( (R[j-1] - m)/s ) < 0.5 )
                  --j, ++c;
               if ( c == 0 )
                  break;
               m = Median( R.At( i ), R.At( j ) );
               if ( j - i < 3 )
                  goto __bad_sigma;
               s = SampleDeviation( R, i, j, m );
            }

            m = Mean( R.At( i ), R.At( j ) );
            s = StdDev( R.At( i ), R.At( j ), m );
            for ( ;; )
            {
               if ( 1 + s == 1 )
                  goto __bad_sigma;
               int n = j - i;
               int c = 0;
               if ( n*QF( (m - R[i])/s ) < 0.5 )
                  ++i, ++c;
               if ( n*QF( (R[j-1] - m)/s ) < 0.5 )
                  --j, ++c;
               if ( c == 0 )
                  break;
               m = Mean( R.At( i ), R.At( j ) );
               if ( j - i < 3 )
                  goto __bad_sigma;
               s = StdDev( R.At( i ), R.At( j ), m );
            }
__bad_sigma:
            E.scale = m;
            E.total = int( psfs.Length() );
            E.count = j - i;
         }
         break;

      case SigmaClipping:
         {
            /*
             * Iterative sigma clipping with fixed rejection threshold.
             */
            Vector R( P1.Length() );
            for ( size_type i = 0; i < P1.Length(); ++i )
               R[i] = P1[i].signal/P2[i].signal;
            int i = 0;
            int j = R.Length();
            double m = R.Median();
            double s = m_psfRejectionThreshold*SampleDeviation( R, i, j, m );
            R.Sort();
            for ( ;; )
            {
               int c = 0;
               for ( ; m - R[i] > s; ++i ) { ++c; };
               for ( ; R[j-1] - m > s; --j ) { ++c; };
               if ( c == 0 )
                  break;
               m = Median( R.At( i ), R.At( j ) );
               if ( j - i < 3 )
                  break;
               s = m_psfRejectionThreshold*SampleDeviation( R, i, j, m );
            }

            E.scale = m;
            E.total = int( psfs.Length() );
            E.count = j - i;
         }
         break;

      case OneStepRejection:
         {
            /*
             * One-step rejection with fixed rejection threshold.
             */
            Vector R( P1.Length() );
            for ( size_type i = 0; i < P1.Length(); ++i )
               R[i] = P1[i].signal/P2[i].signal;
            double m = R.Median();
            double s = m_psfRejectionThreshold*2.2219*R.Qn();
            Array<double> x;
            for ( double r : R )
               if ( Abs( r - m ) < s )
                  x << r;
            E.scale = Median( x.Begin(), x.End() );
            E.total = int( psfs.Length() );
            E.count = x.Length();
         }
         break;

      case LineFit:
         {
            /*
             * Robust line fitting by minimization of absolute deviation from
             * the median.
             */
            Vector D1( P1.Length() );
            Vector D2( P2.Length() );
            for ( size_type i = 0; i < P1.Length(); ++i )
            {
               D1[i] = P1[i].signal;
               D2[i] = P2[i].signal;
            }
            double mx = D2.Median();
            double my = D1.Median();
            double sx = m_psfRejectionThreshold*2.2219*D2.Qn();
            double sy = m_psfRejectionThreshold*2.2219*D1.Qn();
            Array<double> x1, y1;
            for ( int i = 0; i < D2.Length(); ++i )
               if ( Abs( D2[i] - mx ) < sx )
                  if ( Abs( D1[i] - my ) < sy )
                  {
                     x1 << D2[i];
                     y1 << D1[i];
                  }
            if ( x1.Length() < 5 )
               E.scale = 0;
            else
            {
               LinearFit L( x1, y1 );
               E.scale = (IsFinite( L.b ) && 1 + L.b != 1) ? L.b : 0.0;
            }

            E.total = int( psfs.Length() );
            E.count = x1.Length();
         }
         break;
      }

   return E;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/PSFScaleEstimator.cpp - Released 2022-01-24T22:43:35Z
