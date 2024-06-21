//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// pcl/AstrometricReprojection.cpp - Released 2024-06-18T15:49:06Z
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

#include <pcl/AstrometricReprojection.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Rect AstrometricReprojection::TargetRect( const AstrometricMetadata& targetSolution,
                                          const AstrometricMetadata& sourceSolution,
                                          const Rect& sourceRect )
{
   Rect targetRect( int_max, int_max, int_min, int_min );
   for ( int x = 0; x < sourceRect.Width(); ++x )
      for ( int p = 0; p < 2; ++p )
      {
         DPoint pRD, pI;
         if ( !sourceSolution.ImageToCelestial( pRD, DPoint( double( x ), p*(sourceRect.Height() - 1) ) ) )
            throw Error( "Invalid source coordinate transformation" );
         if ( !targetSolution.CelestialToImage( pI, pRD ) )
            throw Error( "Invalid target coordinate transformation" );

         if ( pI.x < targetRect.x0 )
            targetRect.x0 = TruncInt( pI.x );
         if ( pI.x > targetRect.x1 )
            targetRect.x1 = int( Ceil( pI.x ) );
         if ( pI.y < targetRect.y0 )
            targetRect.y0 = TruncInt( pI.y );
         if ( pI.y > targetRect.y1 )
            targetRect.y1 = int( Ceil( pI.y ) );
      }
   for ( int y = 0; y < sourceRect.Height(); ++y )
      for ( int p = 0; p < 2; ++p )
      {
         DPoint pRD, pI;
         if ( !sourceSolution.ImageToCelestial( pRD, DPoint( p*(sourceRect.Width() - 1), double( y ) ) ) )
            throw Error( "Invalid source coordinate transformation" );
         if ( !targetSolution.CelestialToImage( pI, pRD ) )
            throw Error( "Invalid target coordinate transformation" );

         if ( pI.x < targetRect.x0 )
            targetRect.x0 = TruncInt( pI.x );
         if ( pI.x > targetRect.x1 )
            targetRect.x1 = int( Ceil( pI.x ) );
         if ( pI.y < targetRect.y0 )
            targetRect.y0 = TruncInt( pI.y );
         if ( pI.y > targetRect.y1 )
            targetRect.y1 = int( Ceil( pI.y ) );
      }

   return targetRect;
}

// ----------------------------------------------------------------------------

class PCL_AstrometricReprojectionEngine
{
public:

   template <class P2> static
   void Apply( GenericImage<P2>& targetImage, const AstrometricReprojection& R )
   {
      if ( R.SourceImage() )
         if ( !R.SourceImage()->IsEmpty() )
            if ( !targetImage.IsEmpty() )
               if ( R.TargetRect().IsRect() )
                  if ( R.SourceImage().IsFloatSample() )
                     switch ( R.SourceImage().BitsPerSample() )
                     {
                     case 32: Apply( targetImage, static_cast<const Image&>( *R.SourceImage() ), R ); break;
                     case 64: Apply( targetImage, static_cast<const DImage&>( *R.SourceImage() ), R ); break;
                     }
                  else
                     switch ( R.SourceImage().BitsPerSample() )
                     {
                     case  8: Apply( targetImage, static_cast<const UInt8Image&>( *R.SourceImage() ), R ); break;
                     case 16: Apply( targetImage, static_cast<const UInt16Image&>( *R.SourceImage() ), R ); break;
                     case 32: Apply( targetImage, static_cast<const UInt32Image&>( *R.SourceImage() ), R ); break;
                     }
   }

private:

   template <class P1, class P2> static
   void Apply( GenericImage<P2>& targetImage, const GenericImage<P1>& sourceImage, const AstrometricReprojection& R )
   {
      StatusMonitor& status = targetImage.Status();
      Array<size_type> L = Thread::OptimalThreadLoads( R.TargetRect().Height(),
                                                       1/*overheadLimit*/,
                                                       R.IsParallelProcessingEnabled() ? R.MaxProcessors() : 1 );

      size_type N = size_type( R.TargetRect().Width() ) * size_type( R.TargetRect().Height() );
      if ( status.IsInitializationEnabled() )
         status.Initialize( "Astrometric reprojection, " + R.Interpolation().Description(), N );

      ReprojectionThreadData<P1, P2> data( targetImage, sourceImage, R, status, N );

      ReferenceArray<ReprojectionThread<P1, P2>> threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads << new ReprojectionThread<P1, P2>( data, n, n + int( L[i] ) );

      AbstractImage::RunThreads( threads, data );
      threads.Destroy();
   }

   template <class P1, class P2>
   struct ReprojectionThreadData : public AbstractImage::ThreadData
   {
      ReprojectionThreadData( GenericImage<P2>& a_targetImage, const GenericImage<P1>& a_sourceImage,
                              const AstrometricReprojection& a_R, const StatusMonitor& a_status, size_type a_count )
         : AbstractImage::ThreadData( a_status, a_count )
         , targetImage( a_targetImage )
         , sourceImage( a_sourceImage )
         , R( a_R )
      {
      }

            GenericImage<P2>&        targetImage;
      const GenericImage<P1>&        sourceImage;
      const AstrometricReprojection& R;
   };

   template <class P1, class P2>
   class ReprojectionThread : public Thread
   {
   public:

      using interpolator_type = PixelInterpolation::Interpolator<P1>;

      ReprojectionThread( ReprojectionThreadData<P1, P2>& data, int firstRow, int endRow )
         : m_data( data )
         , m_firstRow( firstRow )
         , m_endRow( endRow )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         Rect rect( m_data.R.TargetRect().x0,
                    m_data.R.TargetRect().y0 + m_firstRow,
                    m_data.R.TargetRect().x1,
                    m_data.R.TargetRect().y0 + m_endRow );

         if ( m_data.targetImage.Clip( rect ) )
         {
            ReferenceArray<interpolator_type> interpolators;
            for ( int c = 0; c < m_data.sourceImage.NumberOfChannels(); ++c )
               interpolators << m_data.R.Interpolation().template NewInterpolator<P1>(
                                                                        m_data.sourceImage[c],
                                                                        m_data.sourceImage.Width(),
                                                                        m_data.sourceImage.Height(),
                                                                        m_data.R.UsingUnclippedInterpolation() );

            typename GenericImage<P2>::roi_pixel_iterator it( m_data.targetImage, rect );

            for ( int y = rect.y0; y < rect.y1; ++y )
               for ( int x = rect.x0; x < rect.x1; ++x, ++it )
               {
                  DPoint pRD, pI;
                  if ( m_data.R.TargetSolution().ImageToCelestial( pRD, DPoint( x, y ) ) )
                     if ( m_data.R.SourceSolution().CelestialToImage( pI, pRD ) )
                        if (  pI.x >= 0
                           && pI.x < m_data.sourceImage.Width()
                           && pI.y >= 0
                           && pI.y < m_data.sourceImage.Height() )
                        {
                           for ( int c = 0; c < m_data.sourceImage.NumberOfChannels(); ++c )
                              it[c] = P2::ToSample( interpolators[c]( pI ) );
                        }

                  UPDATE_THREAD_MONITOR( 65536 )
               }

            interpolators.Destroy();
         }
      }

   private:

      ReprojectionThreadData<P1, P2>& m_data;
      int                             m_firstRow;
      int                             m_endRow;
   };
};

// ----------------------------------------------------------------------------

void AstrometricReprojection::Apply( pcl::Image& image ) const
{
   PCL_AstrometricReprojectionEngine::Apply( image, *this );
}

void AstrometricReprojection::Apply( pcl::DImage& image ) const
{
   PCL_AstrometricReprojectionEngine::Apply( image, *this );
}

void AstrometricReprojection::Apply( pcl::UInt8Image& image ) const
{
   PCL_AstrometricReprojectionEngine::Apply( image, *this );
}

void AstrometricReprojection::Apply( pcl::UInt16Image& image ) const
{
   PCL_AstrometricReprojectionEngine::Apply( image, *this );
}

void AstrometricReprojection::Apply( pcl::UInt32Image& image ) const
{
   PCL_AstrometricReprojectionEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/AstrometricReprojection.cpp - Released 2024-06-18T15:49:06Z
