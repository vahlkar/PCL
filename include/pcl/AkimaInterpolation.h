//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/AkimaInterpolation.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_AkimaInterpolation_h
#define __PCL_AkimaInterpolation_h

/// \file pcl/AkimaInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/UnidimensionalInterpolation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define m_x this->m_x
#define m_y this->m_y

/*!
 * \class AkimaInterpolation
 * \brief Akima subspline interpolation algorithm
 *
 * <b>References</b>
 *
 * Hiroshi Akima, <em>A new method of interpolation and smooth curve fitting
 * based on local procedures</em>, Journal of the ACM, Vol. 17, No. 4, October
 * 1970, pages 589-602.
 *
 * <b>Implementation</b>
 *
 * Our implementation is based on the book <em>Numerical Algorithms with
 * C</em>, by G. Engeln-Mullges and F. Uhlig (Springer, 1996), section 13.1.
 *
 * We properly represent corners when a data point lies between two adjacent
 * straight lines with different slopes. This means that our implementation
 * does not impose continuous differentiability, which deviates from the
 * original work by Akima. Supporting the accurate representation of corners
 * has several practical advantages in our opinion; one of them is the enhanced
 * flexibility for the application of Akima interpolation to graphical
 * representations of curves given by a set of prescribed x,y data points.
 *
 * \sa CubicSplineInterpolation, LinearInterpolation
 */
template <typename T = double>
class PCL_CLASS AkimaInterpolation : public UnidimensionalInterpolation<T>
{
public:

   /*!
    * Represents a vector of independent and dependent variable values.
    */
   using vector_type = typename UnidimensionalInterpolation<T>::vector_type;

   /*!
    * Represents a vector of interpolation coefficients.
    */
   using coefficient_vector = vector_type;

   /*!
    * Constructs an %AkimaInterpolation object.
    */
   AkimaInterpolation() = default;

   /*!
    * Copy constructor.
    */
   AkimaInterpolation( const AkimaInterpolation& ) = default;

   /*!
    * Move constructor.
    */
   AkimaInterpolation( AkimaInterpolation&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   AkimaInterpolation& operator =( const AkimaInterpolation& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   AkimaInterpolation& operator =( AkimaInterpolation&& ) = default;

   /*!
    * Destroys an %AkimaInterpolation object.
    */
   ~AkimaInterpolation() override
   {
   }

   /*!
    * Initializes a new interpolation.
    *
    * \param x          %Vector of x-values:\n
    *                   \n
    *    \li If x is not empty: Must be a vector of monotonically increasing,
    *    distinct values: x[0] < x[1] < ... < x[n-1].\n
    *    \li If x is empty: The interpolation will use implicit x[i] = i for
    *    i = {0,1,...,n-1}.\n
    *
    * \param y          %Vector of function values for i = {0,1,...,n-1}.
    *
    * The length of the \a y vector (and also the length of a nonempty \a x
    * vector) must be \e n >= 5. This is because Akima subspline interpolation
    * requires at least 4 subintervals.
    */
   void Initialize( const vector_type& x, const vector_type& y ) override
   {
      if ( y.Length() < 5 )
         throw Error( "AkimaInterpolation::Initialize(): Less than five data points specified." );

      try
      {
         Clear();
         UnidimensionalInterpolation<T>::Initialize( x, y );

         int n = m_y.Length();
         int N = n-1; // Number of subintervals

         m_b = coefficient_vector( N );
         m_c = coefficient_vector( N );
         m_d = coefficient_vector( N );

         // Chordal slopes
         coefficient_vector m0( N+4 ); // room for 4 additional prescribed slopes
         T* m = m0.At( 2 );            // allow negative subscripts m[-1] and m[-2]

         // Akima left-hand slopes to support corners
         coefficient_vector tL( n );

         // Calculate chordal slopes for each subinterval
         if ( m_x )
            for ( int i = 0; i < N; ++i )
            {
               T h = m_x[i+1] - m_x[i];
               if ( 1 + h*h == 1 )
                  throw Error( "AkimaInterpolation::Initialize(): Empty interpolation subinterval(s)." );
               m[i] = (m_y[i+1] - m_y[i])/h;
            }
         else
            for ( int i = 0; i < N; ++i )
               m[i] = m_y[i+1] - m_y[i];

         // Prescribed slopes at ending locations
         m[-2 ] = 3*m[  0] - 2*m[  1];
         m[-1 ] = 2*m[  0] -   m[  1];
         m[ N ] = 2*m[N-1] -   m[N-2];
         m[N+1] = 3*m[N-1] - 2*m[N-2];

         /*
          * Akima left-hand and right-hand slopes.
          * Right-hand slopes are just the interpolation coefficients bi.
          */
         for ( int i = 0; i < n; ++i )
         {
            T f = Abs( m[i-1] - m[i-2] );
            T e = Abs( m[i+1] - m[i] ) + f;
            if ( 1 + e != 1 )
            {
               tL[i] = m[i-1] + f*(m[i] - m[i-1])/e;
               if ( i < N )
                  m_b[i] = tL[i];
            }
            else
            {
               tL[i] = m[i-1];
               if ( i < N )
                  m_b[i] = m[i];
            }
         }

         /*
          * Interpolation coefficients m_b[i], m_c[i], m_d[i]. 'ai'
          * coefficients are the m_y[i] ordinate values.
          */
         for ( int i = 0; i < N; ++i )
         {
            m_c[i] = 3*m[i] - 2*m_b[i] - tL[i+1];
            m_d[i] = m_b[i] + tL[i+1] - 2*m[i];

            if ( m_x )
            {
               T h = m_x[i+1] - m_x[i];
               m_c[i] /= h;
               m_d[i] /= h*h;
            }
         }
      }
      catch ( ... )
      {
         Clear();
         throw;
      }
   }

   /*!
    * Returns an interpolated function value at \a x location.
    *
    * \note Akima subsplines cannot be used for extrapolation. If the specified
    * interpolation point \a x lies outside the range of interpolation defined
    * upon initialization, this function returns zero conventionally.
    */
   PCL_HOT_FUNCTION
   double operator()( double x ) const override
   {
      PCL_PRECONDITION( IsValid() )

      /*
       * Find the subinterval i0 such that m_x[i0] <= x < m_x[i0+1].
       * Find the distance dx = x - m_x[i], or dx = x - i for implicit x = {0,1,...n-1}.
       */
      int i0;
      double dx;
      if ( m_x )
      {
         i0 = 0;
         int i1 = m_x.Length() - 1;
         // if trying to extrapolate, return zero.
         if ( unlikely( x < m_x[0] || x > m_x[i1] ) )
            return 0;
         while ( i1-i0 > 1 )
         {
            int im = (i0 + i1) >> 1;
            if ( x < m_x[im] )
               i1 = im;
            else
               i0 = im;
         }
         dx = x - double( m_x[i0] );
      }
      else
      {
         if ( x <= 0 )
            return m_y[0];
         if ( x >= m_y.Length()-1 )
            return m_y[m_y.Length()-1];
         i0 = TruncInt( x );
         dx = x - i0;
      }

      /*
       * Use a Horner scheme to calculate b[i]*dx + c[i]*dx^2 + d[i]*dx^3.
       */
      return m_y[i0] + dx*(m_b[i0] + dx*(m_c[i0] + dx*m_d[i0]));
   }

   /*!
    * Frees internal data structures in this AkimaInterpolation object.
    */
   void Clear() override
   {
      m_b.Clear();
      m_c.Clear();
      m_d.Clear();
      UnidimensionalInterpolation<T>::Clear();
   }

   /*!
    * Returns true iff this interpolation is valid, i.e. if it has been
    * correctly initialized and is ready to interpolate function values.
    */
   bool IsValid() const override
   {
      return m_b && m_c && m_d;
   }

protected:

   /*
    * Interpolating coefficients for each subinterval.
    * The coefficients for dx^0 are the input ordinate values in the m_y vector.
    */
   coefficient_vector m_b; // coefficients for dx^1
   coefficient_vector m_c; // coefficients for dx^2
   coefficient_vector m_d; // coefficients for dx^3
};

#undef m_x
#undef m_y

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_AkimaInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/AkimaInterpolation.h - Released 2024-12-28T16:53:48Z
