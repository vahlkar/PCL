//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// pcl/Rotation.cpp - Released 2023-12-01T19:15:53Z
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

#include <pcl/AutoPointer.h>
#include <pcl/Rotation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static void GetUnclippedBounds( int& width, int& height, DPoint& origin, const Rotation& R )
{
   DPoint p0( 0.5*width, 0.5*height );
   DPoint p1(       0.5,        0.5 );
   DPoint p2( width-0.5,        0.5 );
   DPoint p3( width-0.5, height-0.5 );
   DPoint p4(       0.5, height-0.5 );

   double sa, ca;
   pcl::SinCos( -R.Angle(), sa, ca );
   p1.Rotate( sa, ca, p0 );
   p2.Rotate( sa, ca, p0 );
   p3.Rotate( sa, ca, p0 );
   p4.Rotate( sa, ca, p0 );

   origin.x = pcl::Min( p1.x, pcl::Min( p2.x, pcl::Min( p3.x, p4.x ) ) );
   origin.y = pcl::Min( p1.y, pcl::Min( p2.y, pcl::Min( p3.y, p4.y ) ) );

   width = 1 + RoundInt( pcl::Max( p1.x, pcl::Max( p2.x, pcl::Max( p3.x, p4.x ) ) ) - origin.x );
   height = 1 + RoundInt( pcl::Max( p1.y, pcl::Max( p2.y, pcl::Max( p3.y, p4.y ) ) ) - origin.y );
}

// ----------------------------------------------------------------------------

void Rotation::GetNewSizes( int& width, int& height ) const
{
   if ( IsUnclipped() )
   {
      DPoint dum;
      GetUnclippedBounds( width, height, dum, *this );
   }
}

// ----------------------------------------------------------------------------

class PCL_RotationEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const Rotation& R )
   {
      if ( 1 + R.Angle() == 1 )
         return;

      int width = image.Width();
      int height = image.Height();
      if ( width == 0 || height == 0 )
         return;

      int srcWidth = width;
      int srcHeight = height;
      DPoint origin = 0.0;
      if ( R.IsUnclipped() )
         GetUnclippedBounds( width, height, origin, R );

      image.EnsureUnique();

      typename P::sample* f = nullptr;
      typename P::sample** f0 = nullptr;

      int numberOfChannels = image.NumberOfChannels();
      typename GenericImage<P>::color_space cs0 = image.ColorSpace();

      double sa, ca;
      pcl::SinCos( R.Angle(), sa, ca );

      DPoint center;
      if ( R.IsUnclipped() )
         center = DPoint( 0.5*width, 0.5*height ) + origin;
      else
         center = R.Center();

      StatusMonitor status = image.Status();

      Array<size_type> L = Thread::OptimalThreadLoads( height,
                                                       1/*overheadLimit*/,
                                                       R.IsParallelProcessingEnabled() ? R.MaxProcessors() : 1 );
      try
      {
         size_type N = size_type( width ) * size_type( height );
         f0 = image.ReleaseData();

         if ( R.IsGammaCorrectionEnabled() )
         {
            size_type Nsrc = size_type( srcWidth ) * size_type( srcHeight );
            if ( status.IsInitializationEnabled() )
               status.Initialize( "Gamma correction ("
                           + (R.RGBWorkingSpace().IsSRGB() ? String( "sRGB" ) :
                              String().Format( "gamma=%.2f", R.RGBWorkingSpace().Gamma() )) + ')', size_type( numberOfChannels )*Nsrc );
            AbstractImage::ThreadData data( status, Nsrc );
            for ( int c = 0; c < numberOfChannels; ++c )
               R.ApplyGammaCorrection<P>( f0[c], Nsrc, data, R.MaxProcessors() );
         }

         if ( status.IsInitializationEnabled() )
            status.Initialize( String().Format( "Rotate %.3f deg, ",
                                                pcl::Deg( R.Angle() ) ) + R.Interpolation().Description(), size_type( numberOfChannels )*N );
         for ( int c = 0; c < numberOfChannels; ++c )
         {
            RotationThreadData<P> data( sa, ca, center, origin, srcWidth, srcHeight, width, status, N );

            data.f = f = image.Allocator().AllocatePixels( N );
            data.fillValue = (c < R.FillValues().Length()) ? P::ToSample( R.FillValues()[c] ) : P::MinSampleValue();

            ReferenceArray<RotationThread<P>> threads;
            for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
               threads.Add( new RotationThread<P>( data,
                                           R.Interpolation().NewInterpolator<P>( f0[c], srcWidth, srcHeight, R.UsingUnclippedInterpolation() ),
                                           n,
                                           n + int( L[i] ) ) );

            AbstractImage::RunThreads( threads, data );
            threads.Destroy();

            image.Allocator().Deallocate( f0[c] );
            f0[c] = f;
            f = nullptr;

            status = data.status;
         }

         if ( R.IsGammaCorrectionEnabled() )
         {
            if ( status.IsInitializationEnabled() )
               status.Initialize( "Inverse gamma correction", size_type( numberOfChannels )*N );
            AbstractImage::ThreadData data( status, N );
            for ( int c = 0; c < numberOfChannels; ++c )
               R.ApplyInverseGammaCorrection<P>( f0[c], N, data, R.MaxProcessors() );
         }

         image.ImportData( f0, width, height, numberOfChannels, cs0 ).Status() = status;
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
   struct RotationThreadData : public AbstractImage::ThreadData
   {
      RotationThreadData( double a_sa, double a_ca, const DPoint& a_center, const DPoint& a_origin,
                          int a_srcWidth, int a_srcHeight, int a_width, const StatusMonitor& a_status, size_type a_count )
         : AbstractImage::ThreadData( a_status, a_count )
         , sa( a_sa )
         , ca( a_ca )
         , center( a_center )
         , origin( a_origin )
         , srcWidth( a_srcWidth )
         , srcHeight( a_srcHeight )
         , width( a_width )
      {
      }

      typename P::sample* f = nullptr; // target data
      typename P::sample  fillValue = P::MinSampleValue(); // unmapped pixel value
      double              sa, ca;      // sine and cosine of rotation angle
      DPoint              center;      // center of rotation
      DPoint              origin;      // origin of unclipped bounds
      int                 srcWidth;    // unrotated width
      int                 srcHeight;   // unrotated height
      int                 width;       // rotated width
   };

   template <class P>
   class RotationThread : public Thread
   {
   public:

      using interpolator_type = PixelInterpolation::Interpolator<P>;

      RotationThread( RotationThreadData<P>& data, interpolator_type* interpolator, int firstRow, int endRow )
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
            double oy = m_data.origin.y + i;

            for ( int j = 0; j < m_data.width; ++j, ++f )
            {
               DPoint p( m_data.origin.x + j, oy );
               p.Rotate( m_data.sa, m_data.ca, m_data.center );
               *f = (p.x >= 0
                  && p.x < m_data.srcWidth
                  && p.y >= 0
                  && p.y < m_data.srcHeight) ? (*m_interpolator)( p ) : m_data.fillValue;

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

      RotationThreadData<P>&         m_data;
      int                            m_firstRow;
      int                            m_endRow;
      AutoPointer<interpolator_type> m_interpolator;
   };
};

// ----------------------------------------------------------------------------

void Rotation::Apply( pcl::Image& image ) const
{
   PCL_RotationEngine::Apply( image, *this );
}

void Rotation::Apply( pcl::DImage& image ) const
{
   PCL_RotationEngine::Apply( image, *this );
}

void Rotation::Apply( pcl::UInt8Image& image ) const
{
   PCL_RotationEngine::Apply( image, *this );
}

void Rotation::Apply( pcl::UInt16Image& image ) const
{
   PCL_RotationEngine::Apply( image, *this );
}

void Rotation::Apply( pcl::UInt32Image& image ) const
{
   PCL_RotationEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Rotation.cpp - Released 2023-12-01T19:15:53Z
