//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Translation.cpp - Released 2024-12-28T16:53:56Z
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

#include <pcl/AutoPointer.h>
#include <pcl/Translation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_TranslationEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const Translation& T )
   {
      if ( T.Delta() == 0.0 )
         return;

      int width = image.Width();
      int height = image.Height();
      if ( width == 0 || height == 0 )
         return;

      image.EnsureUnique();

      typename P::sample* f = nullptr;
      typename P::sample** f0 = nullptr;

      int numberOfChannels = image.NumberOfChannels();
      typename GenericImage<P>::color_space colorSpace = image.ColorSpace();

      StatusMonitor status = image.Status();

      Array<size_type> L = Thread::OptimalThreadLoads( height,
                                                       1/*overheadLimit*/,
                                                       T.IsParallelProcessingEnabled() ? T.MaxProcessors() : 1 );
      try
      {
         size_type N = size_type( width ) * size_type( height );
         f0 = image.ReleaseData();

         if ( T.IsGammaCorrectionEnabled() )
         {
            if ( status.IsInitializationEnabled() )
               status.Initialize( "Gamma correction ("
                           + (T.RGBWorkingSpace().IsSRGB() ? String( "sRGB" ) :
                              String().Format( "gamma=%.2f", T.RGBWorkingSpace().Gamma() )) + ')', size_type( numberOfChannels )*N );
            AbstractImage::ThreadData data( status, N );
            for ( int c = 0; c < numberOfChannels; ++c )
               T.ApplyGammaCorrection<P>( f0[c], N, data, T.MaxProcessors() );
         }

         if ( status.IsInitializationEnabled() )
            status.Initialize( String().Format( "Translate dx=%.3f, dy=%.3f, ",
                        T.Delta().x, T.Delta().y ) + T.Interpolation().Description(),
                        size_type( numberOfChannels )*N );

         for ( int c = 0; c < numberOfChannels; ++c )
         {
            TranslationThreadData<P> data( T.Delta(), width, height, status, N );

            data.f = f = image.Allocator().AllocatePixels( size_type( width )*size_type( height ) );
            data.fillValue = (c < T.FillValues().Length()) ? P::ToSample( T.FillValues()[c] ) : P::MinSampleValue();

            ReferenceArray<TranslationThread<P>> threads;
            for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
               threads.Add( new TranslationThread<P>( data,
                                           T.Interpolation().NewInterpolator<P>( f0[c], width, height, T.UsingUnclippedInterpolation() ),
                                           n,
                                           n + int( L[i] ) ) );

            AbstractImage::RunThreads( threads, data );
            threads.Destroy();

            image.Allocator().Deallocate( f0[c] );
            f0[c] = f;
            f = nullptr;

            status = data.status;
         }

         if ( T.IsGammaCorrectionEnabled() )
         {
            if ( status.IsInitializationEnabled() )
               status.Initialize( "Inverse gamma correction", size_type( numberOfChannels )*N );
            AbstractImage::ThreadData data( status, N );
            for ( int c = 0; c < numberOfChannels; ++c )
               T.ApplyInverseGammaCorrection<P>( f0[c], N, data, T.MaxProcessors() );
         }

         image.ImportData( f0, width, height, numberOfChannels, colorSpace ).Status() = status;
      }
      catch ( ... )
      {
         if ( f != nullptr )
            image.Allocator().Deallocate( f );
         if ( f0 != nullptr )
         {
            for ( int c = 0; c < numberOfChannels; ++c )
               if ( f0[c] != nullptr )
                  image.Allocator().Deallocate( f0[c] );
            image.Allocator().Deallocate( f0 );
         }
         image.FreeData();
         throw;
      }
   }

private:

   template <class P>
   struct TranslationThreadData : public AbstractImage::ThreadData
   {
      TranslationThreadData( const DPoint& a_delta, int a_width, int a_height, const StatusMonitor& a_status, size_type a_count )
         : AbstractImage::ThreadData( a_status, a_count )
         , delta( a_delta )
         , width( a_width )
         , height( a_height )
      {
      }

      typename P::sample* f = nullptr; // target data
      typename P::sample  fillValue = P::MinSampleValue(); // unmapped pixel value
      DPoint              delta;
      int                 width;
      int                 height;
   };

   template <class P>
   class TranslationThread : public Thread
   {
   public:

      using interpolator_type = PixelInterpolation::Interpolator<P>;

      TranslationThread( TranslationThreadData<P>& data, interpolator_type* interpolator, int firstRow, int endRow )
         : m_data( data )
         , m_firstRow( firstRow )
         , m_endRow( endRow )
         , m_interpolator( interpolator )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         typename P::sample* __restrict__ f = m_data.f + m_firstRow*m_data.width;

         for ( int i = m_firstRow; i < m_endRow; ++i )
         {
            double dy = i - m_data.delta.y;

            for ( int j = 0; j < m_data.width; ++j, ++f )
            {
               DPoint p( j - m_data.delta.x, dy );
               *f = (p.x >= 0
                  && p.x < m_data.width
                  && p.y >= 0
                  && p.y < m_data.height) ? (*m_interpolator)( p ) : m_data.fillValue;

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

      TranslationThreadData<P>&      m_data;
      int                            m_firstRow;
      int                            m_endRow;
      AutoPointer<interpolator_type> m_interpolator;
   };
};

// ----------------------------------------------------------------------------

void Translation::Apply( Image& img ) const
{
   PCL_TranslationEngine::Apply( img, *this );
}

void Translation::Apply( DImage& img ) const
{
   PCL_TranslationEngine::Apply( img, *this );
}

void Translation::Apply( UInt8Image& img ) const
{
   PCL_TranslationEngine::Apply( img, *this );
}

void Translation::Apply( UInt16Image& img ) const
{
   PCL_TranslationEngine::Apply( img, *this );
}

void Translation::Apply( UInt32Image& img ) const
{
   PCL_TranslationEngine::Apply( img, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Translation.cpp - Released 2024-12-28T16:53:56Z
