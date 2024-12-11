//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// pcl/SeparableConvolution.cpp - Released 2024-12-11T17:42:39Z
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

#include <pcl/SeparableConvolution.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

template <typename T> inline static
double __pcl_dp_3( const T* __restrict__ x, const T* __restrict__ y )
{
   return double( x[0] ) * double( y[0] )
        + double( x[1] ) * double( y[1] )
        + double( x[2] ) * double( y[2] );
}

template <typename T> inline static
double __pcl_dp_3_i( const T* __restrict__ x, const T* __restrict__ y, const int* __restrict__ index )
{
   return double( x[index[0]] ) * double( y[0] )
        + double( x[index[1]] ) * double( y[1] )
        + double( x[index[2]] ) * double( y[2] );
}

template <typename T> inline static
double __pcl_dp_5( const T* __restrict__ x, const T* __restrict__ y )
{
   return double( x[0] ) * double( y[0] )
        + double( x[1] ) * double( y[1] )
        + double( x[2] ) * double( y[2] )
        + double( x[3] ) * double( y[3] )
        + double( x[4] ) * double( y[4] );
}

template <typename T> inline static
double __pcl_dp_5_i( const T* __restrict__ x, const T* __restrict__ y, const int* __restrict__ index )
{
   return double( x[index[0]] ) * double( y[0] )
        + double( x[index[1]] ) * double( y[1] )
        + double( x[index[2]] ) * double( y[2] )
        + double( x[index[3]] ) * double( y[3] )
        + double( x[index[4]] ) * double( y[4] );
}

template <typename T> inline static
double __pcl_dp_7( const T* __restrict__ x, const T* __restrict__ y )
{
   return double( x[0] ) * double( y[0] )
        + double( x[1] ) * double( y[1] )
        + double( x[2] ) * double( y[2] )
        + double( x[3] ) * double( y[3] )
        + double( x[4] ) * double( y[4] )
        + double( x[5] ) * double( y[5] )
        + double( x[6] ) * double( y[6] );
}

template <typename T> inline static
double __pcl_dp_7_i( const T* __restrict__ x, const T* __restrict__ y, const int* __restrict__ index )
{
   return double( x[index[0]] ) * double( y[0] )
        + double( x[index[1]] ) * double( y[1] )
        + double( x[index[2]] ) * double( y[2] )
        + double( x[index[3]] ) * double( y[3] )
        + double( x[index[4]] ) * double( y[4] )
        + double( x[index[5]] ) * double( y[5] )
        + double( x[index[6]] ) * double( y[6] );
}

template <typename T> inline static
double __pcl_dp( const T* __restrict__ x, const T* __restrict__ y, int n )
{
   double r = 0;
   PCL_IVDEP
   for ( int i = 0; i < n; ++i )
      r += double( x[i] ) * double( y[i] );
   return r;
}

template <typename T> inline static
double __pcl_dp_i( const T* __restrict__ x, const T* __restrict__ y, const int* __restrict__ index, int n )
{
   double r = 0;
   PCL_IVDEP
   for ( int i = 0; i < n; ++i )
      r += double( x[index[i]] ) * double( y[i] );
   return r;
}

#ifdef __PCL_AVX2

inline static
double __pcl_dp( const float* __restrict__ x, const float* __restrict__ y, int n )
{
   const int n8 = n >> 3;
   __m256 d = _mm256_setzero_ps();
   for ( int i = 0; i < n8; ++i )
      d = _mm256_fmadd_ps( _mm256_loadu_ps( x + i*8 ), _mm256_loadu_ps( y + i*8 ), d );
   float r = __pcl_hsum256_ps( d );
   for ( int i = n8 << 3; i < n; ++i )
      r += x[i] * y[i];
   return r;
}

/*
inline static
double __pcl_dp( const float* __restrict__ x, const float* __restrict__ y, int n )
{
   const int n8 = n >> 3;
   __m256d d = _mm256_setzero_pd();
   for ( int i = 0; i < n8; ++i )
   {
      // Accumulate all sums in double precision to avoid roundoff errors.
      __m256 p = _mm256_mul_ps( ((const __m256* __restrict__)x)[i],
                                ((const __m256* __restrict__)y)[i] );
      d = _mm256_add_pd( d, _mm256_add_pd( _mm256_cvtps_pd( ((const __m128* __restrict__)&p)[0] ),
                                           _mm256_cvtps_pd( ((const __m128* __restrict__)&p)[1] ) ) );
   }
   double r = __pcl_hsum256_pd( d );
   for ( int i = n8 << 3; i < n; ++i )
      r += x[i] * y[i];
   return r;
}
*/

inline static
double __pcl_dp_i( const float* __restrict__ x, const float* __restrict__ y, const int* __restrict__ index, int n )
{
   const int n8 = n >> 3;
   __m256 d = _mm256_setzero_ps();
   for ( int i = 0; i < n8; ++i )
      d = _mm256_fmadd_ps( _mm256_i32gather_ps( x, ((const __m256i* __restrict__)index)[i], 4 ),
                           _mm256_loadu_ps( y + i*8 ),
                           d );
   float r = __pcl_hsum256_ps( d );
   for ( int i = n8 << 3; i < n; ++i )
      r += x[index[i]] * y[i];
   return r;
}

// inline static
// double __pcl_dp_i( const float* __restrict__ x, const float* __restrict__ y, const int* __restrict__ index, int n )
// {
//    const int n8 = n >> 3;
//    __m256d d = _mm256_setzero_pd();
//    for ( int i = 0; i < n8; ++i )
//    {
//       // Accumulate all sums in double precision to avoid roundoff errors.
//       __m256 p = _mm256_mul_ps( _mm256_i32gather_ps( x, ((const __m256i* __restrict__)index)[i], 4 ),
//                                 ((const __m256* __restrict__)y)[i] );
//       d = _mm256_add_pd( d, _mm256_add_pd( _mm256_cvtps_pd( ((const __m128* __restrict__)&p)[0] ),
//                                            _mm256_cvtps_pd( ((const __m128* __restrict__)&p)[1] ) ) );
//    }
//    double r = __pcl_hsum256_pd( d );
//    for ( int i = n8 << 3; i < n; ++i )
//       r += x[index[i]] * y[i];
//    return r;
// }

inline static
double __pcl_dp( const double* __restrict__ x, const double* __restrict__ y, int n )
{
   const int n4 = n >> 2;
   __m256d d = _mm256_setzero_pd();
   for ( int i = 0; i < n4; ++i )
      d = _mm256_fmadd_pd( _mm256_loadu_pd( x + i*4 ), _mm256_loadu_pd( y + i*4 ), d );
   double r = __pcl_hsum256_pd( d );
   for ( int i = n4 << 2; i < n; ++i )
      r += x[i] * y[i];
   return r;
}

inline static
double __pcl_dp_i( const double* __restrict__ x, const double* __restrict__ y, const int* __restrict__ index, int n )
{
   const int n4 = n >> 2;
   __m256d d = _mm256_setzero_pd();
   for ( int i = 0; i < n4; ++i )
      d = _mm256_fmadd_pd( _mm256_i32gather_pd( x, ((const __m128i* __restrict__)index)[i], 8 ),
                           _mm256_loadu_pd( y + i*4 ),
                           d );
   double r = __pcl_hsum256_pd( d );
   for ( int i = n4 << 2; i < n; ++i )
      r += x[index[i]] * y[i];
   return r;
}

#endif

// ----------------------------------------------------------------------------

class PCL_SeparableConvolutionEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const SeparableConvolution& convolution )
   {
      if ( P::BitsPerSample() < 32 )
         ConvolveIntegerImage( image, convolution, reinterpret_cast<Image*>( 0 ) );
      else
         ConvolveIntegerImage( image, convolution, reinterpret_cast<DImage*>( 0 ) );
   }

   static void Apply( Image& image, const SeparableConvolution& convolution )
   {
      DoApply( image, convolution );
   }

   static void Apply( DImage& image, const SeparableConvolution& convolution )
   {
      DoApply( image, convolution );
   }

private:

   template <class P> static
   void DoApply( GenericImage<P>& image, const SeparableConvolution& convolution )
   {
      if ( image.IsEmptySelection() )
         return;

      if ( convolution.Filter().IsEmpty() )
         throw Error( "Attempt to perform a separable convolution with an empty filter." );

      image.EnsureUnique();

      int n = convolution.OverlappingDistance();
      if ( n > image.Height() || n > image.Width() )
      {
         image.Zero();
         return;
      }

      /*
       * In order to make SeparableConvolution a perfect replacement for
       * Convolution and FFTConvolution, we force it to perform the same status
       * monitoring increments, i.e., image.NumberOfSelectedSamples().
       */
      size_type N = image.NumberOfSelectedSamples();
      size_type N1 = N >> 1;
      size_type N2 = N - N1;
      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Convolution (separable)", N );

      if ( convolution.IsRowConvolutionEnabled() )
      {
         ThreadData<P> data( image, convolution, N1 );
         int numberOfRows = image.SelectedRectangle().Height();
         Array<size_type> L = Thread::OptimalThreadLoads( numberOfRows,
                                          1/*overheadLimit*/,
                                          convolution.IsParallelProcessingEnabled() ? convolution.MaxProcessors() : 1 );
         ReferenceArray<RowThread<P> > threads;
         for ( int i = 0, n = 0, y0 = image.SelectedRectangle().y0; i < int( L.Length() ); n += int( L[i++] ) )
            threads.Add( new RowThread<P>( data, y0 + n, y0 + n + int( L[i] ) ) );
         AbstractImage::RunThreads( threads, data );
         threads.Destroy();

         image.Status() = data.status;
      }

      if ( convolution.IsColumnConvolutionEnabled() )
      {
         ThreadData<P> data( image, convolution, N2 );
         int numberOfCols = image.SelectedRectangle().Width();
         Array<size_type> L = Thread::OptimalThreadLoads( numberOfCols,
                                          1/*overheadLimit*/,
                                          convolution.IsParallelProcessingEnabled() ? convolution.MaxProcessors() : 1 );
         ReferenceArray<ColThread<P> > threads;
         for ( int i = 0, n = 0, x0 = image.SelectedRectangle().x0; i < int( L.Length() ); n += int( L[i++] ) )
            threads.Add( new ColThread<P>( data, x0 + n, x0 + n + int( L[i] ) ) );
         AbstractImage::RunThreads( threads, data );
         threads.Destroy();

         image.Status() = data.status;
      }

      if ( convolution.IsRowConvolutionEnabled() && convolution.IsColumnConvolutionEnabled() )
      {
         StatusMonitor monitor = image.Status();
         image.SetStatusCallback( nullptr );

         double k = convolution.FilterWeight();
         if ( k != 1 )
            image /= k;

         if ( !convolution.IsRawHighPassEnabled() )
            if ( convolution.IsHighPassFilter() )
               if ( convolution.IsHighPassRescalingEnabled() )
                  image.Normalize();
               else
                  image.Truncate();

         image.Status() = monitor;
      }
   }

   template <class P, class P1> static
   void ConvolveIntegerImage( GenericImage<P>& image, const SeparableConvolution& convolution, GenericImage<P1>* )
   {
      GenericImage<P1> tmp( image );
      DoApply( tmp, convolution );

      StatusMonitor monitor = tmp.Status();
      image.SetStatusCallback( nullptr );

      image.Mov( tmp, image.SelectedRectangle().LeftTop() );

      image.Status() = monitor;
   }

   template <class P>
   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( GenericImage<P>& a_image, const SeparableConvolution& a_convolution, size_type a_count )
         : AbstractImage::ThreadData( a_image, a_count )
         , image( a_image )
         , convolution( a_convolution )
      {
         if ( convolution.IsInterlaced() )
         {
            int n = convolution.Filter().Size();
            int d = convolution.InterlacingDistance();
            index = IVector( n );
            for ( int i = 0, l = 0; i < n; ++i, l += d )
               index[i] = l;
         }
      }

            GenericImage<P>&      image;
      const SeparableConvolution& convolution;
            IVector               index;
   };

   template <class P>
   struct OneDimensionalConvolution
   {
      static PCL_HOT_FUNCTION
      void Convolve1D( typename P::sample* __restrict__ f, typename P::sample* __restrict__ t, int N, int d, int dn2,
                       const typename P::sample* __restrict__ h, const int* __restrict__ index, int n ) noexcept
      {
         // dn2 = (N + (N - 1)*(d - 1)) >> 1;

         for ( int i = 0, j = dn2; j > 0; )
            t[i++] = f[--j];
         ::memcpy( t+dn2, f, N*sizeof( *f ) );
         for ( int i = N+dn2+dn2, j = N-dn2; j < N; )
            t[--i] = f[j++];

         if ( d == 1 )
         {
            /*
             * Compact separable convolution
             */
            switch ( n )
            {
            case 3:
               for ( int i = 0; i < N; ++i, ++f, ++t )
                  *f = P::FloatToSample( __pcl_dp_3( t, h ) );
               break;
            case 5:
               for ( int i = 0; i < N; ++i, ++f, ++t )
                  *f = P::FloatToSample( __pcl_dp_5( t, h ) );
               break;
            case 7:
               for ( int i = 0; i < N; ++i, ++f, ++t )
                  *f = P::FloatToSample( __pcl_dp_7( t, h ) );
               break;
            default:
               for ( int i = 0; i < N; ++i, ++f, ++t )
                  *f = P::FloatToSample( __pcl_dp( t, h, n ) );
               break;
            }
         }
         else
         {
            /*
             * Interlaced separable convolution (e.g., the à trous algorithm)
             */
            switch ( n )
            {
            case 3:
               for ( int i = 0; i < N; ++i, ++f, ++t )
                  *f = P::FloatToSample( __pcl_dp_3_i( t, h, index ) );
               break;
            case 5:
               for ( int i = 0; i < N; ++i, ++f, ++t )
                  *f = P::FloatToSample( __pcl_dp_5_i( t, h, index ) );
               break;
            case 7:
               for ( int i = 0; i < N; ++i, ++f, ++t )
                  *f = P::FloatToSample( __pcl_dp_7_i( t, h, index ) );
               break;
            default:
               for ( int i = 0; i < N; ++i, ++f, ++t )
                  *f = P::FloatToSample( __pcl_dp_i( t, h, index, n ) );
               break;
            }
         }
      }
   };

   template <class P>
   class RowThread : public Thread, public OneDimensionalConvolution<P>
   {
   public:

      RowThread( ThreadData<P>& data, int firstRow, int endRow )
         : m_data( data )
         , m_firstRow( firstRow )
         , m_endRow( endRow )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         const Rect rect = m_data.image.SelectedRectangle();
         const int width = rect.Width();

         const int d = m_data.convolution.InterlacingDistance();
         const int dn = m_data.convolution.OverlappingDistance();
         const int dn2 = dn >> 1;

         GenericVector<typename P::sample> tv( width + dn2+dn2 );
         GenericVector<typename P::sample> hv = m_data.convolution.FilterAs( 0, (typename P::sample*)0 );
         typename P::sample* t = tv.DataPtr();
         const typename P::sample* h = hv.DataPtr();
         const int n = hv.Length();

         for ( int c = m_data.image.FirstSelectedChannel(); c <= m_data.image.LastSelectedChannel(); ++c )
         {
            typename P::sample* f = m_data.image.PixelAddress( rect.x0, m_firstRow, c );
            for ( int i = m_firstRow; i < m_endRow; ++i, f += m_data.image.Width() )
            {
               this->Convolve1D( f, t, width, d, dn2, h, m_data.index.Begin(), n );
               UPDATE_THREAD_MONITOR_CHUNK( 65536, width )
            }
         }
      }

   private:

      ThreadData<P>& m_data;
      int            m_firstRow;
      int            m_endRow;
   };

   template <class P>
   class ColThread : public Thread, public OneDimensionalConvolution<P>
   {
   public:

      ColThread( ThreadData<P>& data, int firstCol, int endCol )
         : m_data( data )
         , m_firstCol( firstCol )
         , m_endCol( endCol )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         const Rect rect = m_data.image.SelectedRectangle();
         const int width = rect.Width();
         const int height = rect.Height();

         const int d = m_data.convolution.InterlacingDistance();
         const int dn = m_data.convolution.OverlappingDistance();
         const int dn2 = dn >> 1;

         GenericVector<typename P::sample> gv( height );
         GenericVector<typename P::sample> tv( height + dn2+dn2 );
         GenericVector<typename P::sample> hv = m_data.convolution.FilterAs( 1, (typename P::sample*)0 );
         typename P::sample* __restrict__ g = gv.DataPtr();
         typename P::sample* t = tv.DataPtr();
         const typename P::sample* h = hv.DataPtr();
         const int n = hv.Length();

         for ( int c = m_data.image.FirstSelectedChannel(); c <= m_data.image.LastSelectedChannel(); ++c )
         {
            typename P::sample* __restrict__ f = m_data.image.PixelAddress( m_firstCol, rect.y0, c );
            for ( int i = m_firstCol; i < m_endCol; ++i, ++f )
            {
               for ( int i = 0, j = 0; i < height; ++i, j += width )
                  g[i] = f[j];

               this->Convolve1D( g, t, height, d, dn2, h, m_data.index.Begin(), n );

               for ( int i = 0, j = 0; i < height; ++i, j += width )
                  f[j] = g[i];

               UPDATE_THREAD_MONITOR_CHUNK( 65536, height )
            }
         }
      }

   private:

      ThreadData<P>& m_data;
      int            m_firstCol;
      int            m_endCol;
   };
};

// ----------------------------------------------------------------------------

void SeparableConvolution::Apply( pcl::Image& image ) const
{
   PCL_PRECONDITION( !m_filter.IsNull() )
   ValidateFilter();
   PCL_SeparableConvolutionEngine::Apply( image, *this );
}

void SeparableConvolution::Apply( pcl::DImage& image ) const
{
   PCL_PRECONDITION( !m_filter.IsNull() )
   ValidateFilter();
   PCL_SeparableConvolutionEngine::Apply( image, *this );
}

void SeparableConvolution::Apply( pcl::UInt8Image& image ) const
{
   PCL_PRECONDITION( !m_filter.IsNull() )
   ValidateFilter();
   PCL_SeparableConvolutionEngine::Apply( image, *this );
}

void SeparableConvolution::Apply( pcl::UInt16Image& image ) const
{
   PCL_PRECONDITION( !m_filter.IsNull() )
   ValidateFilter();
   PCL_SeparableConvolutionEngine::Apply( image, *this );
}

void SeparableConvolution::Apply( pcl::UInt32Image& image ) const
{
   PCL_PRECONDITION( !m_filter.IsNull() )
   ValidateFilter();
   PCL_SeparableConvolutionEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

void SeparableConvolution::ValidateFilter() const
{
   if ( m_filter.IsNull() )
      throw Error( "Invalid access to uninitialized separable convolution" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SeparableConvolution.cpp - Released 2024-12-11T17:42:39Z
