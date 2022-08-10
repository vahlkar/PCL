//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.30
// ----------------------------------------------------------------------------
// pcl/RobustChauvenetRejection.h - Released 2022-08-10T16:36:28Z
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

#ifndef __PCL_RobustChauvenetRejection_h
#define __PCL_RobustChauvenetRejection_h

/// \file pcl/RobustChauvenetRejection.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Constants.h>
#include <pcl/LinearFit.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class RobustChauvenetRejection
 * \brief Robust Chauvenet outlier rejection algorithm
 *
 * This class implements a robust iterative sigma clipping outlier rejection
 * algorithm with Chauvenet rejection criterion. The design and implementation
 * are based on:
 *
 * Maples, M. P., Reichart, D. E. et al., <em>Robust Chauvenet Outlier
 * Rejection</em>, Astrophysical Journal Supplement Series, 2018, ApJS, 238,
 * A2, 1-49.
 */
class PCL_CLASS RobustChauvenetRejection
{
public:

   /*!
    * Default constructor.
    */
   RobustChauvenetRejection() = default;

   /*!
    * Copy constructor.
    */
   RobustChauvenetRejection( const RobustChauvenetRejection& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~RobustChauvenetRejection()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   RobustChauvenetRejection& operator =( const RobustChauvenetRejection& ) = default;

   /*!
    * Returns the limit for the altered Chauvenet rejection criterion.
    *
    * The larger the value of this parameter, the more pixels will be rejected
    * by the Robust Chauvenet Rejection algorithm. The original Chauvenet
    * rejection criterion is N*P(x > |z|) &lt; 0.5, where N is the number of
    * measurements and P() represents the probability of x being more than z
    * standard deviations from the mean. This parameter alters the rejection
    * criterion by replacing 0.5 with an arbitrary limit in the [0,1] range, in
    * order to make the algorithm controllable.
    *
    * The default rejection limit is 0.5, that is, the original Chauvenet
    * rejection criterion is applied by default.
    */
   float RejectionLimit() const
   {
      return m_rejectionLimit;
   }

   /*!
    * Sets the limit for the altered Chauvenet rejection criterion. See
    * RejectionLimit() for a description of this parameter.
    */
   void SetRejectionLimit( float r )
   {
      PCL_PRECONDITION( r > 0 && r < 1 )
      m_rejectionLimit = Range( r, 0.0F, 1.0F );
   }

   /*!
    * Returns the minimum length to enable bulk rejection.
    *
    * For data sets smaller than the value of this parameter the robust
    * Chauvenet rejection routine rejects a single value at each iteration.
    * This improves stability and accuracy of the rejection process.
    *
    * For larger data sets, a bulk rejection procedure is enabled for
    * performance reasons. The larger the length of the input array with
    * respect to this parameter, the more values will be rejected at each
    * iteration, which accelerates the process and allows this implementation
    * to handle even very large data sets of hundreds of thousands of items.
    *
    * The default large sample size is 20,000.
    */
   int LargeSampleSize() const
   {
      return m_largeSampleSize;
   }

   /*!
    * Sets the minimum length to enable bulk rejection. See LargeSampleSize()
    * for a description of this parameter.
    */
   void SetLargeSampleSize( int n )
   {
      m_largeSampleSize = Max( 1, n );
   }

   /*!
    * Performs a Robust Chauvenet Rejection procedure for the specified data.
    *
    * \param[out] i        Reference to a variable where the function will
    *                      store the zero-based index of the first non-rejected
    *                      element in the sorted \a data array.
    *
    * \param[out] j        Reference to a variable where the function will
    *                      store the zero-based index of the end of the
    *                      non-rejected sequence in the sorted \a data array.
    *
    * \param[out] mean     Reference to a variable where the function will
    *                      store the mean of the subset of non-rejected
    *                      elements in the \a data array.
    *
    * \param[out] sigma    Reference to a variable where the function will
    *                      store the standard deviation of the subset of
    *                      non-rejected elements in the \a data array.
    *
    * \param[in,out] data  Random-access container of objects to be analyzed
    *                      and rejected. On output, the elements in this
    *                      container will be sorted in ascending order.
    *
    * On output, the subset of non-rejected array elements is defined by the
    * range [i,j) of zero-based array indices relative to the beginning of the
    * \a data container. The elements in the \a data container will be sorted
    * in ascending order.
    *
    * This function will perform a three-stage iterative rejection procedure,
    * applying a Chauvenet rejection criterion to reject one or more array
    * elements at each iteration (depending on the length of the input
    * array and the value of the LargeSampleSize() parameter), in succesive
    * stages of decreasing robustness/increasing precision. On output, the
    * robust location and scale parameters of the non-rejected subset will be
    * returned in the specified \a mean and \a sigma variables, respectively.
    *
    * The template parameter C represents a vector or array-like container with
    * indexed random access. If T is the type of an element in a container of
    * type C, the following operators must be implicitly or explicitly defined:
    *
    * \code
    * T::operator double() const;
    * bool T::operator <( const T& ) const;
    * \endcode
    *
    * The type represented by the I template parameter should correspond to an
    * integral scalar type such as int, unsigned, etc.
    *
    * \note This function is thread-safe.
    */
   template <class C, typename I>
   void PerformRejection( I& i, I& j, double& mean, double& sigma, C& data ) const
   {
      data.Sort();
      i = I( 0 );
      j = I( data.Length() );

      for ( int phase = 0; phase < 3; ++phase )
         for ( ;; )
         {
            switch ( phase )
            {
            case  0: // + robustness / - precision
               mean = Median( data.At( i ), data.At( j ) );
               sigma = LineFitDeviation( data, i, j, mean );
               break;
            case  1: // = robustness / = precision
               mean = Median( data.At( i ), data.At( j ) );
               sigma = SampleDeviation( data, i, j, mean );
               break;
            case  2: // - robustness / + precision
               mean = Mean( data.At( i ), data.At( j ) );
               sigma = StdDev( data.At( i ), data.At( j ), mean );
               break;
            }

            if ( 1 + sigma == 1 )
               return;

            I n = j - i;
            if ( n < I( 3 ) )
               return;

            if ( n <= I( m_largeSampleSize ) + I( m_largeSampleSize >> 1 ) )
            {
               /*
                * Optimal single-pixel rejection iteration for 'small' samples.
                */
               double d0 = n*QF( (mean - data[i])/sigma );
               double d1 = n*QF( (data[j-1] - mean)/sigma );
               if ( d0 >= m_rejectionLimit )
                  if ( d1 >= m_rejectionLimit )
                     break;
               if ( d1 < d0 )
                  --j;
               else
                  ++i;
            }
            else
            {
               /*
                * Accelerated bulk rejection for large samples.
                */
               int nc = RoundInt( double( n )/m_largeSampleSize );
               int c = 0;
               for ( int it = 0; it < nc && n*QF( (mean - data[i])/sigma ) < m_rejectionLimit; ++i, ++c, ++it ) {}
               for ( int it = 0; it < nc && n*QF( (data[j-1] - mean)/sigma ) < m_rejectionLimit; --j, ++c, ++it ) {}
               if ( c == 0 )
                  break;
            }
         }
   }

   /*!
    * Performs a Robust Chauvenet Rejection procedure.
    *
    * This operator is equivalent to the PerformRejection() const member
    * function.
    *
    * \note This function is thread-safe.
    */
   template <class C, typename I>
   void operator()( I& i, I& j, double& mean, double& sigma, C& data ) const
   {
      return PerformRejection( i, j, mean, sigma, data );
   }

private:

   float m_rejectionLimit = 0.5; // 0.5 = Chauvenet rejection criterion
   int   m_largeSampleSize = 20000;

   /*
    * The Q-function: the probability for a value from a normal distribution of
    * being more than x standard deviations from the mean.
    */
   static double QF( double x )
   {
      return 0.5*(1 - Erf( x/Const<double>::sqrt2() ));
   }

   /*
    * Correction factor for 68.3-percentile deviations to avoid overaggressive
    * rejection for small samples.
    */
   template <typename I>
   static double FN( I N )
   {
      return 1/(1 - 2.9442*Pow( double( N ), -1.073 ));
   }

   /*
    * The 68.3-percentile value from the distribution of absolute deviations.
    */
   template <class C, typename I>
   static double SampleDeviation( const C& R, I i, I j, double m )
   {
      int N = int( j - i );
      Vector D( N );
      for ( int k = 0; i < j; ++i, ++k )
         D[k] = Abs( double( R[i] ) - m );
      return FN( N ) * *pcl::Select( D.Begin(), D.End(), TruncInt( 0.683*D.Length() ) );
   }

   /*
    * 68.3-percentile deviation by fitting a zero-intercept line to the vector
    * of absolute differences.
    */
   template <class C, typename I>
   static double LineFitDeviation( const C& R, I i, I j, double m )
   {
      int N = int( j - i );
      int n = int( 0.683*N + 0.317 );
      if ( n < 8 )
         return SampleDeviation( R, i, j, m );

      Vector y( N );
      int k = 0;
      for ( I ii = i; ii < j; ++ii, ++k )
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
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_RobustChauvenetRejection_h

// ----------------------------------------------------------------------------
// EOF pcl/RobustChauvenetRejection.h - Released 2022-08-10T16:36:28Z
