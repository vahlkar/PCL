//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.0
// ----------------------------------------------------------------------------
// pcl/WinsorizedSigmaClippingRejection.h - Released 2023-09-15T14:49:04Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_WinsorizedSigmaClippingRejection_h
#define __PCL_WinsorizedSigmaClippingRejection_h

/// \file pcl/WinsorizedSigmaClippingRejection.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Array.h>
#include <pcl/Math.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class WinsorizedSigmaClippingRejection
 * \brief Winsorized sigma clipping outlier rejection algorithm
 *
 * This class implements a robust iterative sigma clipping outlier rejection
 * algorithm based on Huber's method of robust estimation of parameters through
 * Winsorization. This algorithm can yield superior rejection of outliers and
 * better preservation of significant data for relatively large data sets, with
 * a small performance penalty compared to a simple sigma clipping scheme.
 *
 * \sa RobustChauvenetRejection
 */
class PCL_CLASS WinsorizedSigmaClippingRejection
{
public:

   /*!
    * Default constructor.
    */
   WinsorizedSigmaClippingRejection() = default;

   /*!
    * Copy constructor.
    */
   WinsorizedSigmaClippingRejection( const WinsorizedSigmaClippingRejection& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~WinsorizedSigmaClippingRejection()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   WinsorizedSigmaClippingRejection& operator =( const WinsorizedSigmaClippingRejection& ) = default;

   /*!
    * Returns true iff rejection of low sample values is enabled. Low sample
    * values are values smaller than the (robust) mean of the sample being
    * analyzed.
    */
   bool ClipLow() const
   {
      return m_clipLow;
   }

   /*!
    * Returns true iff rejection of high sample values is enabled. High sample
    * values are values greater than the (robust) mean of the sample being
    * analyzed.
    */
   bool ClipHigh() const
   {
      return m_clipHigh;
   }

   /*!
    * Enables rejection of low values. See ClipLow() for more information on
    * this parameter.
    */
   void EnableLowClipping( bool enable = true )
   {
      m_clipLow = enable;
   }

   /*!
    * Disables rejection of low values. See ClipLow() for more information on
    * this parameter.
    */
   void DisableLowClipping( bool disable = true )
   {
      EnableLowClipping( !disable );
   }

   /*!
    * Enables rejection of high values. See ClipHigh() for more information on
    * this parameter.
    */
   void EnableHighClipping( bool enable = true )
   {
      m_clipHigh = enable;
   }

   /*!
    * Disables rejection of high values. See ClipHigh() for more information on
    * this parameter.
    */
   void DisableHighClipping( bool disable = true )
   {
      EnableHighClipping( !disable );
   }

   /*!
    * Returns the <em>low sigma</em> parameter of the rejection algorithm.
    *
    * Values smaller than the mean and located at distances from the mean
    * greater than this parameter in sigma units will be rejected in successive
    * iterations. The lower this parameter, the more data will be rejected at
    * the lower tail of the sample.
    *
    * The default low sigma parameter value is 3 sigma.
    */
   float LowSigma() const
   {
      return m_sigmaLow;
   }

   /*!
    * Returns the <em>high sigma</em> parameter of the rejection algorithm.
    *
    * Values greater than the mean and located at distances from the mean
    * greater than this parameter in sigma units will be rejected in successive
    * iterations. The lower this parameter, the more data will be rejected at
    * the upper tail of the sample.
    *
    * The default high sigma parameter value is 3 sigma.
    */
   float HighSigma() const
   {
      return m_sigmaHigh;
   }

   /*!
    * Sets the value of the <em>low sigma</em> parameter. See LowSigma() for
    * more information on this parameter.
    */
   void SetLowSigma( float sigma )
   {
      PCL_PRECONDITION( sigma >= 0 )
      m_sigmaLow = Max( 0.0F, sigma );
   }

   /*!
    * Sets the value of the <em>high sigma</em> parameter. See HighSigma() for
    * more information on this parameter.
    */
   void SetHighSigma( float sigma )
   {
      PCL_PRECONDITION( sigma >= 0 )
      m_sigmaHigh = Max( 0.0F, sigma );
   }

   /*!
    * Returns the <em>Winsorization cutoff</em> parameter of the rejection
    * algorithm in sigma units.
    *
    * All values with absolute differences from the mean larger than this
    * parameter will be set equal to the mean of the sample in the first
    * rejection iteration. This replaces extreme outliers with plausible values
    * instead of their nearest neighbors, which avoids contamination with
    * extreme spurious values.
    *
    * The default Winsorization cutoff is 5 sigma.
    */
   float WinsorizationCutoff() const
   {
      return m_cutoff;
   }

   /*!
    * Sets the value of the <em>Winsorization cutoff</em> parameter of the
    * rejection algorithm in sigma units. See WinsorizationCutoff() for more
    * information on this parameter.
    */
   void SetWinsorizationCutoff( float cutoff )
   {
      PCL_PRECONDITION( cutoff >= 1 )
      m_cutoff = Max( 1.0F, cutoff );
   }

   /*!
    * Performs a Winsorized sigma clipping rejection procedure for the
    * specified data.
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
    * This function will perform a robust, iterative sigma clipping outlier
    * rejection procedure using Winsorization to compute sample location and
    * scale parameters at each iteration. On output, the robust location and
    * scale parameters of the non-rejected subset will be returned in the
    * specified \a mean and \a sigma variables, respectively.
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
      for ( int it = 0; ; ++it )
      {
         GetWinsorizedParameters( mean, sigma, i, j, data, (it > 0) ? .0F : m_cutoff );
         if ( 1 + sigma == 1 )
            return;
         if ( I( j - i ) < I( 3 ) )
            return;

         int c = 0;
         if ( m_clipLow )
            for ( ; i < j; ++i, ++c )
               if ( (mean - data[i])/sigma <= m_sigmaLow )
                  break;
         if ( m_clipHigh )
            for ( ; i < j; --j, ++c )
               if ( (data[j-1] - mean)/sigma <= m_sigmaHigh )
                  break;
         if ( c == 0 )
            return;
      }
   }

   /*!
    * Performs a Winsorized sigma clipping rejection procedure.
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

   bool  m_clipLow = true;  // perform rejection of low values
   bool  m_clipHigh = true; // perform rejection of high values
   float m_sigmaLow = 3;    // rejection sigma, low values
   float m_sigmaHigh = 3;   // rejection sigma, high values
   float m_cutoff = 5;      // Winsorization cutoff point in sigma units

   /*!
    * \internal
    * Winsorized parameter estimation routine.
    *
    * \param[out] mean     On output, the Winsorized mean of the sample.
    *
    * \param[out] sigma    On output, the Winsorized standard deviation of the
    *                      sample.
    *
    * \param      i        The zero-based index of the first non-rejected
    *                      element in the sorted \a data array.
    *
    * \param      j        The zero-based index of the end of the non-rejected
    *                      sequence in the sorted \a data array.
    *
    * \param      data     The sample being analyzed.
    *
    * \param      cutoff   Winsorization cutoff. Should be > 0 only for the
    *                      first rejection iteration.
    */
   template <class C, typename I>
   static void GetWinsorizedParameters( double& mean, double& sigma, const I i, const I j, const C& data, float cutoff )
   {
      size_type n = size_type( j - i );
      if ( n < 2 )
      {
         mean = sigma = 0;
         return;
      }

      Array<double> v( n );
      {
         size_type l = 0;
         for ( I k = i; k < j; ++k, ++l )
            v[l] = double( data[k] );
      }

      mean = MedianDestructive( v.Begin(), v.End() );
      sigma = 1.1926*Sn( v.Begin(), v.End() );
      v.Sort();

      for ( int it = 0;; )
      {
         if ( 1 + sigma == 1 )
            break;

         double t0 = mean - 1.5*sigma;
         double t1 = mean + 1.5*sigma;

         if ( cutoff > 0 )
         {
            double c0 = mean - cutoff*sigma;
            double c1 = mean + cutoff*sigma;
            for ( size_type i = 0; i < n; ++i )
               if ( v[i] < t0 )
                  v[i] = (v[i] > c0) ? t0 : mean;
               else if ( v[i] > t1 )
                  v[i] = (v[i] < c1) ? t1 : mean;
         }
         else
         {
            for ( size_type i = 0; i < n; ++i )
               if ( v[i] < t0 )
                  v[i] = t0;
               else if ( v[i] > t1 )
                  v[i] = t1;
         }

         double s0 = sigma;
         sigma = 1.134*StdDev( v.Begin(), v.End() );
         mean = Mean( v.Begin(), v.End() );
         if ( ++it > 1 )
            if ( Abs( s0 - sigma )/s0 < 0.0005 )
               break;
      }
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_WinsorizedSigmaClippingRejection_h

// ----------------------------------------------------------------------------
// EOF pcl/WinsorizedSigmaClippingRejection.h - Released 2023-09-15T14:49:04Z
