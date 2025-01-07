//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Convolution.cpp - Released 2024-12-27T18:16:14Z
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

#include <pcl/Convolution.h>
#include <pcl/MultiVector.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_ConvolutionEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const Convolution& convolution )
   {
      if ( convolution.IsHighPassFilter() )
      {
         if ( P::BitsPerSample() < 32 )
            HighPassIntegerImage( image, convolution, reinterpret_cast<Image*>( 0 ) );
         else
            HighPassIntegerImage( image, convolution, reinterpret_cast<DImage*>( 0 ) );
      }
      else
         DoApply( image, convolution );
   }

   static
   void Apply( Image& image, const Convolution& convolution )
   {
      DoApply( image, convolution );
      if ( convolution.IsHighPassFilter() )
         HighPassRescaleFloatImage( image, convolution );
   }

   static
   void Apply( DImage& image, const Convolution& convolution )
   {
      DoApply( image, convolution );
      if ( convolution.IsHighPassFilter() )
         HighPassRescaleFloatImage( image, convolution );
   }

private:

   template <class P> static
   void DoApply( GenericImage<P>& image, const Convolution& convolution )
   {
      if ( image.IsEmptySelection() )
         return;

      if ( convolution.Filter().IsEmpty() )
         throw Error( "Attempt to perform a convolution with an empty kernel filter." );

      image.EnsureUnique();

      int n = convolution.OverlappingDistance();
      if ( n > image.Height() || n > image.Width() )
      {
         image.Zero();
         return;
      }

      /*
       * We implement a correlation algorithm: ensure the convolution filter is
       * rotated by 180 degrees. We'll unrotate it once the task has finished.
       */
      bool didFlip = false;
      if ( !convolution.Filter().IsFlipped() )
      {
         const_cast<KernelFilter&>( convolution.Filter() ).Flip();
         didFlip = true;
      }

      Array<size_type> L = pcl::Thread::OptimalThreadLoads( image.SelectedRectangle().Height(),
                                       n/*overheadLimit*/,
                                       convolution.IsParallelProcessingEnabled() ? convolution.MaxProcessors() : 1 );
      int numberOfThreads = int( L.Length() );

      size_type N = image.NumberOfSelectedSamples();
      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Convolution", N );

      ThreadData<P> data( image, convolution, N );

      ReferenceArray<Thread<P> > threads;
      for ( int i = 0, n = 0, y0 = image.SelectedRectangle().y0; i < numberOfThreads; n += int( L[i++] ) )
         threads.Add( new Thread<P>( data,
                                     y0 + n,
                                     y0 + n + int( L[i] ),
                                     i > 0,
                                     i < numberOfThreads-1 ) );
      try
      {
         AbstractImage::RunThreads( threads, data );
         if ( didFlip )
            const_cast<KernelFilter&>( convolution.Filter() ).Flip();
      }
      catch ( ... )
      {
         if ( didFlip )
            const_cast<KernelFilter&>( convolution.Filter() ).Flip();
         throw;
      }

      image.SetStatusCallback( nullptr );

      int c0 = image.SelectedChannel();
      Point p0 = image.SelectedRectangle().LeftTop();

      for ( int i = 0, n = 0; i < numberOfThreads; n += int( L[i++] ) )
      {
         if ( i > 0 )
            image.Mov( threads[i].UpperOverlappingRegion(),
                       Point( p0.x, p0.y + n ), c0 );
         if ( i < numberOfThreads-1 )
            image.Mov( threads[i].LowerOverlappingRegion(),
                       Point( p0.x, p0.y + n + int( L[i] ) - threads[i].LowerOverlappingRegion().Height() ), c0 );
      }

      image.Status() = data.status;

      threads.Destroy();
   }

   template <class P, class P1> static
   void HighPassIntegerImage( GenericImage<P>& image, const Convolution& convolution, GenericImage<P1>* )
   {
      GenericImage<P1> tmp( image );
      Apply( tmp, convolution );

      StatusMonitor monitor = tmp.Status();
      image.SetStatusCallback( nullptr );

      image.Mov( tmp, image.SelectedRectangle().LeftTop() );

      image.Status() = monitor;
   }

   template <class P> static
   void HighPassRescaleFloatImage( GenericImage<P>& image, const Convolution& convolution )
   {
      if ( !convolution.IsRawHighPassEnabled() )
         if ( convolution.IsHighPassFilter() )
         {
            StatusMonitor monitor = image.Status();
            image.SetStatusCallback( nullptr );

            if ( convolution.IsHighPassRescalingEnabled() )
               image.Normalize();
            else
               image.Truncate();

            image.Status() = monitor;
         }
   }

   template <class P>
   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( GenericImage<P>& a_image, const Convolution& a_convolution, size_type a_count )
         : AbstractImage::ThreadData( a_image, a_count )
         , image( a_image )
         , convolution( a_convolution )
      {
      }

            GenericImage<P>& image;
      const Convolution&     convolution;
   };

   template <class P>
   class Thread : public pcl::Thread
   {
   public:

      using region   = GenericImage<P>;
      using raw_data = GenericMultiVector<typename P::sample>;

      Thread( ThreadData<P>& data, int startRow, int endRow, bool upperOvRgn, bool lowerOvRgn )
         : m_data( data )
         , m_firstRow( startRow )
         , m_endRow( endRow )
         , m_haveUpperOvRgn( upperOvRgn )
         , m_haveLowerOvRgn( lowerOvRgn )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         const Rect rect = m_data.image.SelectedRectangle();
         const int width = rect.Width();
         const int hdelta = m_data.image.Width() - width;

         const int filterSize = m_data.convolution.Filter().Size();
         const int bufRows = m_data.convolution.OverlappingDistance();
         const int idist = m_data.convolution.InterlacingDistance();
         const int n2 = bufRows >> 1;
         const int bufCols = width + (n2 << 1);

         int o0 = m_firstRow;
         if ( m_haveUpperOvRgn )
         {
            m_upperOvRgn.AllocateData( width, n2, m_data.image.NumberOfSelectedChannels() );
            o0 += n2;
         }

         int o1 = m_endRow;
         if ( m_haveLowerOvRgn )
         {
            m_lowerOvRgn.AllocateData( width, n2, m_data.image.NumberOfSelectedChannels() );
            o1 -= n2;
         }

         const typename P::sample lowThreshold = P::ToSample( m_data.convolution.LowThreshold() );
         const typename P::sample highThreshold = P::ToSample( m_data.convolution.HighThreshold() );

         const bool haveLowThreshold = 1 + lowThreshold != 1;
         const bool haveHighThreshold = 1 + highThreshold != 1;
         const bool noThreshold = !(haveLowThreshold || haveHighThreshold);

         const bool unitWeight = m_data.convolution.FilterWeight() == 1;

         double (*innerLoop)( typename raw_data::const_iterator, typename raw_data::const_iterator,
                              const KernelFilter::coefficient* __restrict__, int, int, int ) noexcept;
         innerLoop = m_data.convolution.IsInterlaced() ? InnerLoop_Interlaced : InnerLoop_Compact;

         raw_data buffer( P::MinSampleValue(), bufRows, bufCols );

         const KernelFilter::coefficient* kernel = m_data.convolution.Filter().Begin();

         for ( int c = m_data.image.FirstSelectedChannel(), cn = 0; c <= m_data.image.LastSelectedChannel(); ++c, ++cn )
         {
            typename P::sample* f = m_data.image.PixelAddress( rect.x0, m_firstRow, c );
            typename P::sample* g = m_haveUpperOvRgn ? m_upperOvRgn[cn] : nullptr;

            for ( int i = 0, i0 = m_firstRow-n2, i1 = m_firstRow+n2-1; i < n2; ++i, ++i0, --i1 )
               ::memcpy( buffer[i].At( n2 ), m_data.image.PixelAddress( rect.x0, (i0 < 0) ? i1 : i0, c ), width*P::BytesPerSample() );

            for ( int i = n2, i1 = m_firstRow; i < bufRows; ++i, ++i1 )
               ::memcpy( buffer[i].At( n2 ), m_data.image.PixelAddress( rect.x0, i1, c ), width*P::BytesPerSample() );

            for ( int i = 0; i < bufRows; ++i )
            {
               typename raw_data::vector_iterator f0i = *buffer[i];
               typename raw_data::vector_iterator f1i = f0i + n2+n2;
               PCL_UNROLL( 8 )
               do
                  *f0i++ = *f1i--;
               while ( f0i < f1i );

               f0i = buffer[i].At( width-1 );
               f1i = f0i + n2+n2;
               PCL_UNROLL( 8 )
               do
                  *f1i-- = *f0i++;
               while ( f0i < f1i );
            }

            for ( int y = m_firstRow;; )
            {
               if ( likely( noThreshold ) )
               {
                  for ( int x = 0; x < width; ++x, ++f )
                  {
                     double r = (*innerLoop)( buffer.Begin(), buffer.End(), kernel, x, filterSize, idist );
                     if ( !unitWeight )
                        r /= m_data.convolution.FilterWeight();

                     if ( g == nullptr )
                        *f = P::FloatToSample( r );
                     else
                        *g++ = P::FloatToSample( r );
                  }
               }
               else
               {
                  for ( int x = 0; x < width; ++x, ++f )
                  {
                     double r = (*innerLoop)( buffer.Begin(), buffer.End(), kernel, x, filterSize, idist );
                     if ( !unitWeight )
                        r /= m_data.convolution.FilterWeight();

                     if ( r < *f )
                     {
                        if ( haveLowThreshold )
                        {
                           double k = *f - r;
                           if ( k < lowThreshold )
                           {
                              k /= lowThreshold;
                              r = k*r + (1 - k)*(*f);
                           }
                        }
                     }
                     else
                     {
                        if ( haveHighThreshold )
                        {
                           double k = r - *f;
                           if ( k < highThreshold )
                           {
                              k /= highThreshold;
                              r = k*r + (1 - k)*(*f);
                           }
                        }
                     }

                     if ( g == nullptr )
                        *f = P::FloatToSample( r );
                     else
                        *g++ = P::FloatToSample( r );
                  }
               }

               if ( ++y == m_endRow )
                  break;

               f += hdelta;

               if ( g == nullptr )
               {
                  if ( m_haveLowerOvRgn )
                     if ( y == o1 )
                        g = m_lowerOvRgn[cn];
               }
               else
               {
                  if ( y == o0 )
                     g = nullptr;
               }

               for ( int i = 1; i < bufRows; ++i )
                  pcl::Swap( buffer[i-1], buffer[i] );

               if ( y+n2 < m_data.image.Height() )
               {
                  ::memcpy( buffer[bufRows-1].At( n2 ), m_data.image.PixelAddress( rect.x0, y+n2, c ), width*P::BytesPerSample() );

                  typename raw_data::vector_iterator f0n = *buffer[bufRows-1];
                  typename raw_data::vector_iterator f1n = f0n + n2+n2;
                  PCL_UNROLL( 8 )
                  do
                     *f0n++ = *f1n--;
                  while ( f0n < f1n );

                  f0n = buffer[bufRows-1].At( width-1 );
                  f1n = f0n + n2+n2;
                  PCL_UNROLL( 8 )
                  do
                     *f1n-- = *f0n++;
                  while ( f0n < f1n );
               }
               else
               {
                  ::memcpy( *buffer[bufRows-1], *buffer[bufRows-2], bufCols*P::BytesPerSample() );
                  /*
                   * ### N.B.: Cannot use an assignment operator here because
                   * all of the buffer vectors must be unique.
                   */
                  //buffer[bufRows-1] = buffer[bufRows-2];
               }
            }

            UPDATE_THREAD_MONITOR( 1024 )
         }
      }

      const region& UpperOverlappingRegion() const
      {
         return m_upperOvRgn;
      }

      const region& LowerOverlappingRegion() const
      {
         return m_lowerOvRgn;
      }

   private:

      ThreadData<P>& m_data;
      int            m_firstRow;
      int            m_endRow;
      region         m_upperOvRgn;
      region         m_lowerOvRgn;
      bool           m_haveUpperOvRgn;
      bool           m_haveLowerOvRgn;

      /*
       * Compact correlation.
       */
      static double InnerLoop_Compact( typename raw_data::const_iterator i,
                                       typename raw_data::const_iterator j,
                                       const KernelFilter::coefficient* __restrict__ h,
                                       int x, int n, int/*d*/ ) noexcept
      {
         double r = 0;
         for ( ; i < j; ++i )
         {
            typename raw_data::const_vector_iterator __restrict__ fi = i->At( x );
            for ( int k = 0; k < n; ++k )
               r += *h++ * *fi++;
         }
         return r;
      }

      /*
       * Interlaced correlation (e.g., the à trous algorithm).
       */
      static double InnerLoop_Interlaced( typename raw_data::const_iterator i,
                                          typename raw_data::const_iterator j,
                                          const KernelFilter::coefficient* __restrict__ h,
                                          int x, int n, int d ) noexcept
      {
         double r = 0;
         for ( ; i < j; i += d )
         {
            typename raw_data::const_vector_iterator __restrict__ fi = i->At( x );
            for ( int k = 0, l = 0; k < n; ++k, l += d )
               r += *h++ * fi[l];
         }
         return r;
      }
   };
};

// ----------------------------------------------------------------------------

void Convolution::Apply( Image& image ) const
{
   PCL_PRECONDITION( !m_filter.IsNull() )
   ValidateFilter();
   PCL_ConvolutionEngine::Apply( image, *this );
}

void Convolution::Apply( DImage& image ) const
{
   PCL_PRECONDITION( !m_filter.IsNull() )
   ValidateFilter();
   PCL_ConvolutionEngine::Apply( image, *this );
}

void Convolution::Apply( UInt8Image& image ) const
{
   PCL_PRECONDITION( !m_filter.IsNull() )
   ValidateFilter();
   PCL_ConvolutionEngine::Apply( image, *this );
}

void Convolution::Apply( UInt16Image& image ) const
{
   PCL_PRECONDITION( !m_filter.IsNull() )
   ValidateFilter();
   PCL_ConvolutionEngine::Apply( image, *this );
}

void Convolution::Apply( UInt32Image& image ) const
{
   PCL_PRECONDITION( !m_filter.IsNull() )
   ValidateFilter();
   PCL_ConvolutionEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

void Convolution::ValidateFilter() const
{
   if ( m_filter.IsNull() )
      throw Error( "Invalid access to uninitialized convolution" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Convolution.cpp - Released 2024-12-27T18:16:14Z
