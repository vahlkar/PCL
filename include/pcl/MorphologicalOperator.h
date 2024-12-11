//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// pcl/MorphologicalOperator.h - Released 2024-12-11T17:42:29Z
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

#ifndef __PCL_MorphologicalOperator_h
#define __PCL_MorphologicalOperator_h

/// \file pcl/MorphologicalOperator.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Exception.h>
#include <pcl/Math.h>
#include <pcl/PixelTraits.h>
#include <pcl/Selection.h>
#include <pcl/Sort.h>
#include <pcl/String.h>
#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MorphologicalOperator
 * \brief Abstract base class of all PCL morphological operators.
 *
 * \sa ErosionFilter, DilationFilter, MedianFilter, SelectionFilter,
 * MidpointFilter, AlphaTrimmedMeanFilter
 */
class PCL_CLASS MorphologicalOperator
{
public:

   /*!
    * Constructs a %MorphologicalOperator object.
    */
   MorphologicalOperator() = default;

   /*!
    * Copy constructor.
    */
   MorphologicalOperator( const MorphologicalOperator& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~MorphologicalOperator()
   {
   }

   /*!
    * Returns a pointer to a dynamically allocated duplicate of this
    * morphological operator.
    */
   virtual MorphologicalOperator* Clone() const = 0;

   /*!
    * Returns a human-readable description of this morphological operator.
    */
   virtual String Description() const
   {
      return String();
   }

   /*!
    * \internal
    * Returns true iff this object performs a dilation morphological operation.
    * This member function is used internally to decide whether to reflect the
    * structuring element during morphological transformations.
    */
   virtual bool IsDilation() const
   {
      return false;
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * floating point pixel samples.
    */
   virtual FloatPixelTraits::sample operator ()( FloatPixelTraits::sample* f, size_type n ) const
   {
      throw NotImplemented( *this, "Apply to 32-bit floating-point images" );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 64-bit
    * floating point pixel samples.
    */
   virtual DoublePixelTraits::sample operator ()( DoublePixelTraits::sample* f, size_type n ) const
   {
      throw NotImplemented( *this, "Apply to 64-bit floating-point images" );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * floating point complex pixel samples.
    */
   virtual ComplexPixelTraits::sample operator ()( ComplexPixelTraits::sample* f, size_type n ) const
   {
      throw NotImplemented( *this, "Apply to 32-bit complex images" );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 64-bit
    * floating point complex pixel samples.
    */
   virtual DComplexPixelTraits::sample operator ()( DComplexPixelTraits::sample* f, size_type n ) const
   {
      throw NotImplemented( *this, "Apply to 64-bit complex images" );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 8-bit
    * unsigned integer pixel samples.
    */
   virtual UInt8PixelTraits::sample operator ()( UInt8PixelTraits::sample* f, size_type n ) const
   {
      throw NotImplemented( *this, "Apply to 8-bit integer images" );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 16-bit
    * unsigned integer pixel samples.
    */
   virtual UInt16PixelTraits::sample operator ()( UInt16PixelTraits::sample* f, size_type n ) const
   {
      throw NotImplemented( *this, "Apply to 16-bit integer images" );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * unsigned integer pixel samples.
    */
   virtual UInt32PixelTraits::sample operator ()( UInt32PixelTraits::sample* f, size_type n ) const
   {
      throw NotImplemented( *this, "Apply to 32-bit integer images" );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class ErosionFilter
 * \brief Erosion morphological operator.
 *
 * The \e erosion morphological operator computes the minimum value in a pixel
 * neighborhood.
 */
class PCL_CLASS ErosionFilter : public MorphologicalOperator
{
public:

   /*!
    */
   MorphologicalOperator* Clone() const override
   {
      return new ErosionFilter( *this );
   }

   /*!
    */
   String Description() const override
   {
      return "Erosion";
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * floating point pixel samples.
    */
   FloatPixelTraits::sample operator ()( FloatPixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 64-bit
    * floating point pixel samples.
    */
   DoublePixelTraits::sample operator ()( DoublePixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 8-bit
    * unsigned integer pixel samples.
    */
   UInt8PixelTraits::sample operator ()( UInt8PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 16-bit
    * unsigned integer pixel samples.
    */
   UInt16PixelTraits::sample operator ()( UInt16PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * unsigned integer pixel samples.
    */
   UInt32PixelTraits::sample operator ()( UInt32PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

private:

   template <typename T>
   static T Operate( T* __restrict__ f, size_type n )
   {
      T x = *f++;
      for ( ; --n > 0; ++f )
         if ( *f < x )
            x = *f;
      return x;
   }

#ifdef __PCL_AVX2

   static float Operate( float* __restrict__ f, size_type n )
   {
      if ( unlikely( n < 8 ) )
      {
         float min = f[0];
         for ( size_type i = 1; i < n; ++i )
            if ( f[i] < min )
               min = f[i];
         return min;
      }

      __m256 vmin;
      const size_type n8 = n >> 3;
      if ( unlikely( ((ptrdiff_t)f) & 31 ) )
      {
         vmin = _mm256_loadu_ps( f );
         for ( size_type i = 1; i < n8; ++i )
            vmin = _mm256_min_ps( vmin, _mm256_loadu_ps( (const float* __restrict__)(f + i*8) ) );
      }
      else
      {
         vmin = _mm256_load_ps( f );
         for ( size_type i = 1; i < n8; ++i )
            vmin = _mm256_min_ps( vmin, ((const __m256* __restrict__)f)[i] );
      }
      float min = ((const float* __restrict__)&vmin)[0];
      for ( int i = 1; i < 8; ++i )
      {
         float vn = ((const float* __restrict__)&vmin)[i];
         if ( vn < min )
            min = vn;
      }
      for ( size_type i = n8 << 3; i < n; ++i )
         if ( f[i] < min )
            min = f[i];
      return min;
   }

   static double Operate( double* __restrict__ f, size_type n )
   {
      if ( unlikely( n < 4 ) )
      {
         double min = f[0];
         for ( size_type i = 1; i < n; ++i )
            if ( f[i] < min )
               min = f[i];
         return min;
      }

      __m256d vmin;
      const size_type n4 = n >> 2;
      if ( unlikely( ((ptrdiff_t)f) & 31 ) )
      {
         vmin = _mm256_loadu_pd( f );
         for ( size_type i = 1; i < n4; ++i )
            vmin = _mm256_min_pd( vmin, _mm256_loadu_pd( (const double* __restrict__)(f + i*4) ) );
      }
      else
      {
         vmin = _mm256_load_pd( f );
         for ( size_type i = 1; i < n4; ++i )
            vmin = _mm256_min_pd( vmin, ((const __m256d* __restrict__)f)[i] );
      }
      double min = ((const double* __restrict__)&vmin)[0];
      for ( int i = 1; i < 4; ++i )
      {
         double vn = ((const double* __restrict__)&vmin)[i];
         if ( vn < min )
            min = vn;
      }
      for ( size_type i = n4 << 2; i < n; ++i )
         if ( f[i] < min )
            min = f[i];
      return min;
   }

#endif
};

// ----------------------------------------------------------------------------

/*!
 * \class DilationFilter
 * \brief Dilation morphological operator.
 *
 * The \e dilation morphological operator computes the maximum value in a pixel
 * neighborhood.
 */
class PCL_CLASS DilationFilter : public MorphologicalOperator
{
public:

   /*!
    */
   MorphologicalOperator* Clone() const override
   {
      return new DilationFilter( *this );
   }

   /*!
    */
   String Description() const override
   {
      return "Dilation";
   }

   /*!
    */
   bool IsDilation() const override
   {
      return true;
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * floating point pixel samples.
    */
   FloatPixelTraits::sample operator ()( FloatPixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 64-bit
    * floating point pixel samples.
    */
   DoublePixelTraits::sample operator ()( DoublePixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 8-bit
    * unsigned integer pixel samples.
    */
   UInt8PixelTraits::sample operator ()( UInt8PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 16-bit
    * unsigned integer pixel samples.
    */
   UInt16PixelTraits::sample operator ()( UInt16PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * unsigned integer pixel samples.
    */
   UInt32PixelTraits::sample operator ()( UInt32PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

private:

   template <typename T>
   static T Operate( T* __restrict__ f, size_type n )
   {
      T x = *f++;
      for ( ; --n > 0; ++f )
         if ( x < *f )
            x = *f;
      return x;
   }

#ifdef __PCL_AVX2

   static float Operate( float* __restrict__ f, size_type n )
   {
      if ( unlikely( n < 8 ) )
      {
         float max = f[0];
         for ( size_type i = 1; i < n; ++i )
            if ( f[i] > max )
               max = f[i];
         return max;
      }

      __m256 vmax;
      const size_type n8 = n >> 3;
      if ( unlikely( ((ptrdiff_t)f) & 31 ) )
      {
         vmax = _mm256_loadu_ps( f );
         for ( size_type i = 1; i < n8; ++i )
            vmax = _mm256_max_ps( vmax, _mm256_loadu_ps( (const float* __restrict__)(f + i*8) ) );
      }
      else
      {
         vmax = _mm256_load_ps( f );
         for ( size_type i = 1; i < n8; ++i )
            vmax = _mm256_max_ps( vmax, ((const __m256* __restrict__)f)[i] );
      }
      float max = ((const float* __restrict__)&vmax)[0];
      for ( int i = 1; i < 8; ++i )
      {
         float vn = ((const float* __restrict__)&vmax)[i];
         if ( vn > max )
            max = vn;
      }
      for ( size_type i = n8 << 3; i < n; ++i )
         if ( f[i] > max )
            max = f[i];
      return max;
   }

   static double Operate( double* __restrict__ f, size_type n )
   {
      if ( unlikely( n < 4 ) )
      {
         double max = f[0];
         for ( size_type i = 1; i < n; ++i )
            if ( f[i] > max )
               max = f[i];
         return max;
      }

      __m256d vmax;
      const size_type n4 = n >> 2;
      if ( unlikely( ((ptrdiff_t)f) & 31 ) )
      {
         vmax = _mm256_loadu_pd( f );
         for ( size_type i = 1; i < n4; ++i )
            vmax = _mm256_max_pd( vmax, _mm256_loadu_pd( (const double* __restrict__)(f + i*4) ) );
      }
      else
      {
         vmax = _mm256_load_pd( f );
         for ( size_type i = 1; i < n4; ++i )
            vmax = _mm256_max_pd( vmax, ((const __m256d* __restrict__)f)[i] );
      }
      double max = ((const double* __restrict__)&vmax)[0];
      for ( int i = 1; i < 4; ++i )
      {
         double vn = ((const double* __restrict__)&vmax)[i];
         if ( vn > max )
            max = vn;
      }
      for ( size_type i = n4 << 2; i < n; ++i )
         if ( f[i] > max )
            max = f[i];
      return max;
   }

#endif
};

// ----------------------------------------------------------------------------

/*!
 * \class MedianFilter
 * \brief Morphological median operator.
 *
 * The \e median morphological operator (or median order-statistic filter)
 * computes the median of the values in a pixel neighborhood.
 */
class PCL_CLASS MedianFilter : public MorphologicalOperator
{
public:

   /*!
    */
   MorphologicalOperator* Clone() const override
   {
      return new MedianFilter( *this );
   }

   /*!
    */
   String Description() const override
   {
      return "Median";
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * floating point pixel samples.
    */
   FloatPixelTraits::sample operator ()( FloatPixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (FloatPixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 64-bit
    * floating point pixel samples.
    */
   DoublePixelTraits::sample operator ()( DoublePixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (DoublePixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 8-bit
    * unsigned integer pixel samples.
    */
   UInt8PixelTraits::sample operator ()( UInt8PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (UInt8PixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 16-bit
    * unsigned integer pixel samples.
    */
   UInt16PixelTraits::sample operator ()( UInt16PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (UInt16PixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * unsigned integer pixel samples.
    */
   UInt32PixelTraits::sample operator ()( UInt32PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (UInt32PixelTraits*)0 );
   }

private:

   template <typename T, class P>
   static T Operate( T* __restrict__ f, size_type n, P* )
   {
      if ( n > __PCL_SMALL_MEDIAN_MAX_LENGTH )
      {
         distance_type n2 = distance_type( n >> 1 );
         if ( n & 1 )
            return *pcl::Select( f, f+n, n2 );
         return P::FloatToSample( (double( *pcl::Select( f, f+n, n2   ) )
                                 + double( *pcl::Select( f, f+n, n2-1 ) ))/2 );
      }

      return P::FloatToSample( pcl::SmallMedianDestructive( f, f+n ) );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class SelectionFilter
 * \brief Morphological selection operator.
 *
 * The \e selection morphological operator (or selection order-statistic
 * filter) computes the value corresponding to a prescribed percentile in the
 * ranked set of values from a pixel neighborhood.
 *
 * The <em>selection point</em> \a k of the selection operator defines the
 * working percentile: for k=0, we have an erosion (or minimum) operator; for
 * k=1 we have a dilation (or maximum) operator; k=0.5 defines the median
 * operator. Other values work as a balance between erosion and dilation.
 */
class PCL_CLASS SelectionFilter : public MorphologicalOperator
{
public:

   /*!
    * Default constructor. The default selection point is 0.5, equivalent to a
    * median operator.
    */
   SelectionFilter() = default;

   /*!
    * Constructs a %SelectionFilter with selection point \a k.
    */
   SelectionFilter( double k )
      : m_k( pcl::Range( k, 0.0, 1.0 ) )
   {
      PCL_PRECONDITION( 0 <= k && k <= 1 )
      PCL_CHECK( 0 <= m_k && m_k <= 1 )
   }

   /*!
    * Copy constructor.
    */
   SelectionFilter( const SelectionFilter& ) = default;

   /*!
    */
   MorphologicalOperator* Clone() const override
   {
      return new SelectionFilter( *this );
   }

   /*!
    * Returns the selection point of this morphological selection operator.
    */
   double SelectionPoint() const
   {
      return m_k;
   }

   /*!
    * Sets the selection point \a k of this morphological selection operator.
    */
   void SetSelectionPoint( double k )
   {
      PCL_PRECONDITION( 0 <= k && k <= 1 )
      m_k = pcl::Range( k, 0.0, 1.0 );
   }

   /*!
    */
   String Description() const override
   {
      return String().Format( "Selection, k=%.5f", m_k );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * floating point pixel samples.
    */
   FloatPixelTraits::sample operator ()( FloatPixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 64-bit
    * floating point pixel samples.
    */
   DoublePixelTraits::sample operator ()( DoublePixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 8-bit
    * unsigned integer pixel samples.
    */
   UInt8PixelTraits::sample operator ()( UInt8PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 16-bit
    * unsigned integer pixel samples.
    */
   UInt16PixelTraits::sample operator ()( UInt16PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * unsigned integer pixel samples.
    */
   UInt32PixelTraits::sample operator ()( UInt32PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n );
   }

private:

   double m_k = 0.5;

   template <typename T>
   T Operate( T* __restrict__ f, size_type n ) const
   {
      return *pcl::Select( f, f+n, distance_type( pcl::RoundInt( m_k*(n - 1) ) ) );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class MidpointFilter
 * \brief Midpoint operator.
 *
 * The \e midpoint operator computes the midpoint value in a pixel
 * neighborhood, namely: (minimum + maximum)/2.
 */
class PCL_CLASS MidpointFilter : public MorphologicalOperator
{
public:

   /*!
    */
   MorphologicalOperator* Clone() const override
   {
      return new MidpointFilter( *this );
   }

   /*!
    */
   String Description() const override
   {
      return "Midpoint";
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * floating point pixel samples.
    */
   FloatPixelTraits::sample operator ()( FloatPixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (FloatPixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 64-bit
    * floating point pixel samples.
    */
   DoublePixelTraits::sample operator ()( DoublePixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (DoublePixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 8-bit
    * unsigned integer pixel samples.
    */
   UInt8PixelTraits::sample operator ()( UInt8PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (UInt8PixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 16-bit
    * unsigned integer pixel samples.
    */
   UInt16PixelTraits::sample operator ()( UInt16PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (UInt16PixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * unsigned integer pixel samples.
    */
   UInt32PixelTraits::sample operator ()( UInt32PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (UInt32PixelTraits*)0 );
   }

private:

   template <typename T, class P>
   static T Operate( T* __restrict__ f, size_type n, P* )
   {
      T* min, * max;
      pcl::FindExtremeItems( min, max, f, f+n );
      return P::FloatToSample( (double( *min ) + double( *max ))/2 );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class AlphaTrimmedMeanFilter
 * \brief Alpha-trimmed mean operator
 *
 * The <em>alpha-trimmed mean</em> operator computes the mean of the remaining
 * values in a pixel neighborhood, after suppressing the d/2 lowest and the d/2
 * highest values. The point \a d defines the <em>trimming factor</em> of the
 * operator. When d=0, we have an arithmetic mean filter. If d=1, the filter is
 * the median operator.
 *
 * The alpha-trimmed operator is not a morphological operator in the strict
 * sense, although it allows for the implementation of robust filters that
 * share some important properties with morphological filters in practical
 * applications.
 */
class PCL_CLASS AlphaTrimmedMeanFilter : public MorphologicalOperator
{
public:

   /*!
    * Default constructor. The default trimming factor is 0.2, which leads to
    * the suppression of a 10% of the lowest and highest neighbor pixels. The
    * resulting object is a robust mean filter resilient to outliers in most
    * practical cases.
    */
   AlphaTrimmedMeanFilter() = default;

   /*!
    * Constructs a %AlphaTrimmedMeanFilter object with the specified trimming
    * factor \a d in the [0,1] range.
    */
   AlphaTrimmedMeanFilter( double d )
      : m_d( pcl::Range( d, 0.0, 1.0 ) )
   {
      PCL_PRECONDITION( 0 <= d && d <= 1 )
      PCL_CHECK( 0 <= m_d && m_d <= 1 )
   }

   /*!
    * Copy constructor.
    */
   AlphaTrimmedMeanFilter( const AlphaTrimmedMeanFilter& ) = default;

   /*!
    */
   MorphologicalOperator* Clone() const override
   {
      return new AlphaTrimmedMeanFilter( *this );
   }

   /*!
    * Returns the trimming factor of this alpha-trimmed mean operator.
    */
   double TrimmingFactor() const
   {
      return m_d;
   }

   /*!
    * Sets the trimming factor \a d of this alpha-trimmed mean operator.
    */
   void SetTrimmingFactor( double d )
   {
      PCL_PRECONDITION( 0 <= d && d <= 1 )
      m_d = pcl::Range( d, 0.0, 1.0 );
   }

   /*!
    */
   String Description() const override
   {
      return String().Format( "Alpha-trimmed mean, d=%.5f", m_d );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * floating point pixel samples.
    */
   FloatPixelTraits::sample operator ()( FloatPixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (FloatPixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 64-bit
    * floating point pixel samples.
    */
   DoublePixelTraits::sample operator ()( DoublePixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (DoublePixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 8-bit
    * unsigned integer pixel samples.
    */
   UInt8PixelTraits::sample operator ()( UInt8PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (UInt8PixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 16-bit
    * unsigned integer pixel samples.
    */
   UInt16PixelTraits::sample operator ()( UInt16PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (UInt16PixelTraits*)0 );
   }

   /*!
    * Applies this morphological operator to a vector \a f of \a n 32-bit
    * unsigned integer pixel samples.
    */
   UInt32PixelTraits::sample operator ()( UInt32PixelTraits::sample* f, size_type n ) const override
   {
      return Operate( f, n, (UInt32PixelTraits*)0 );
   }

private:

   double m_d = 0.2;

   template <typename T, class P>
   T Operate( T* __restrict__ f, size_type n, P* ) const
   {
      pcl::Sort( f, f+n );
      double s = 0;
      size_type i1 = RoundInt( m_d*((n - 1) >> 1) );
      size_type i2 = n-i1;
      for ( size_type i = i1; i < i2; ++i )
         s += f[i];
      return P::FloatToSample( s/(i2 - i1) );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MorphologicalOperator_h

// ----------------------------------------------------------------------------
// EOF pcl/MorphologicalOperator.h - Released 2024-12-11T17:42:29Z
