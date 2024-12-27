//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/AstrometricReprojection.cpp - Released 2024-12-27T18:16:14Z
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

DRect AstrometricReprojection::TargetRect( const AstrometricMetadata& targetSolution,
                                           const AstrometricMetadata& sourceSolution,
                                           const Rect& sourceRect )
{
   DRect rect( std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
               std::numeric_limits<double>::min(), std::numeric_limits<double>::min() );
   for ( int p = 0; p < 2; ++p )
   {
      for ( int x = 0, y = p*(sourceRect.Height() - 1); x < sourceRect.Width(); ++x )
      {
         DPoint pRD, pI;
         if ( !sourceSolution.ImageToCelestial( pRD, DPoint( x, y ) ) )
            throw Error( "AstrometricReprojection::TargetRect(): Invalid source coordinate transformation (step 1)." );
         if ( !targetSolution.CelestialToImage( pI, pRD ) )
            throw Error( "AstrometricReprojection::TargetRect(): Invalid target coordinate transformation (step 1)." );

         if ( pI.x < rect.x0 )
            rect.x0 = pI.x;
         if ( pI.x > rect.x1 )
            rect.x1 = pI.x;
         if ( pI.y < rect.y0 )
            rect.y0 = pI.y;
         if ( pI.y > rect.y1 )
            rect.y1 = pI.y;
      }
      for ( int x = p*(sourceRect.Width() - 1), y = 0; y < sourceRect.Height(); ++y )
      {
         DPoint pRD, pI;
         if ( !sourceSolution.ImageToCelestial( pRD, DPoint( x, y ) ) )
            throw Error( "AstrometricReprojection::TargetRect(): Invalid source coordinate transformation (step 2)." );
         if ( !targetSolution.CelestialToImage( pI, pRD ) )
            throw Error( "AstrometricReprojection::TargetRect(): Invalid target coordinate transformation (step 2)." );

         if ( pI.x < rect.x0 )
            rect.x0 = pI.x;
         if ( pI.x > rect.x1 )
            rect.x1 = pI.x;
         if ( pI.y < rect.y0 )
            rect.y0 = pI.y;
         if ( pI.y > rect.y1 )
            rect.y1 = pI.y;
      }
   }

   return rect;
}

// ----------------------------------------------------------------------------

bool AstrometricReprojection::TargetPointInsideSourceRect( const AstrometricMetadata& targetSolution,
                                                           const AstrometricMetadata& sourceSolution,
                                                           const DPoint& targetPt,
                                                           const DRect& sourceRect )
{
   DPoint pRD, pIR;
   if ( targetSolution.ImageToCelestial( pRD, targetPt ) )
      if ( sourceSolution.CelestialToImage( pIR, pRD ) )
         return sourceRect.Includes( pIR );
   return false;
}

bool AstrometricReprojection::TargetRectInsideSourceRect( const AstrometricMetadata& targetSolution,
                                                          const AstrometricMetadata& sourceSolution,
                                                          const DRect& targetRect,
                                                          const DRect& sourceRect )
{
   return TargetPointInsideSourceRect( targetSolution, sourceSolution, DPoint( 0, 0 ),                                    sourceRect )
       && TargetPointInsideSourceRect( targetSolution, sourceSolution, DPoint( targetRect.Width(), 0 ),                   sourceRect )
       && TargetPointInsideSourceRect( targetSolution, sourceSolution, DPoint( 0, targetRect.Height() ),                  sourceRect )
       && TargetPointInsideSourceRect( targetSolution, sourceSolution, DPoint( targetRect.Width(), targetRect.Height() ), sourceRect );
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
      /*
       * Limiting bounds of the reprojected target image in target image
       * coordinates.
       */
      Rect rect = R.TargetRect().TruncatedToInt().InflatedBy( 1 );

      if ( targetImage.Width() != R.TargetSolution().Width()
        || targetImage.Height() != R.TargetSolution().Height() )
      {
         double sx = double( R.TargetSolution().Width() )/targetImage.Width();
         double sy = double( R.TargetSolution().Height() )/targetImage.Height();
         rect.x0 = TruncInt( rect.x0 * sx );
         rect.y0 = TruncInt( rect.y0 * sy );
         rect.x1 = TruncInt( rect.x1 * sx ) + 1;
         rect.y1 = TruncInt( rect.y1 * sy ) + 1;
      }

      if ( !targetImage.Clip( rect ) )
         return;

      Array<size_type> L = Thread::OptimalThreadLoads( rect.Height(),
                                                       1/*overheadLimit*/,
                                                       R.IsParallelProcessingEnabled() ? R.MaxProcessors() : 1 );
      if ( targetImage.Status().IsInitializationEnabled() )
      {
         size_type N = size_type( rect.Width() ) * size_type( rect.Height() );
         targetImage.Status().Initialize( "Astrometric reprojection, " + R.Interpolation().Description(), N );
      }

      ReprojectionThreadData<P1,P2> data( targetImage, sourceImage, rect, R );

      ReferenceArray<ReprojectionThread<P1,P2>> threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads << new ReprojectionThread<P1,P2>( data, n, n + int( L[i] ) );

      AbstractImage::RunThreads( threads, data );

      R.m_zeroCount = 0; // mutable m_zeroCount
      for ( const auto& thread : threads )
         R.m_zeroCount += thread.ZeroCount();

      threads.Destroy();
   }

   template <class P1, class P2>
   struct ReprojectionThreadData : public AbstractImage::ThreadData
   {
      ReprojectionThreadData( GenericImage<P2>& a_targetImage, const GenericImage<P1>& a_sourceImage,
                              const Rect& a_rect,
                              const AstrometricReprojection& a_R )
         : AbstractImage::ThreadData( a_targetImage, a_targetImage.NumberOfPixels() )
         , targetImage( a_targetImage )
         , sourceImage( a_sourceImage )
         , rect( a_rect )
         , R( a_R )
      {
      }

            GenericImage<P2>&        targetImage;
      const GenericImage<P1>&        sourceImage;
            Rect                     rect; // in target image coordinates
      const AstrometricReprojection& R;
   };

   template <class P1, class P2>
   class ReprojectionThread : public Thread
   {
   public:

      using interpolator_type = PixelInterpolation::Interpolator<P1>;

      ReprojectionThread( ReprojectionThreadData<P1,P2>& data, int firstRow, int endRow )
         : m_data( data )
         , m_firstRow( firstRow )
         , m_endRow( endRow )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         m_zeroCount = 0;

         Rect rect( m_data.rect.x0,
                    m_data.rect.y0 + m_firstRow,
                    m_data.rect.x1,
                    m_data.rect.y0 + m_endRow );

         ReferenceArray<interpolator_type> interpolators;
         for ( int c = 0; c < m_data.sourceImage.NumberOfChannels(); ++c )
            interpolators << m_data.R.Interpolation().template NewInterpolator<P1>(
                                                                     m_data.sourceImage[c],
                                                                     m_data.sourceImage.Width(),
                                                                     m_data.sourceImage.Height(),
                                                                     m_data.R.UsingUnclippedInterpolation() );

         typename GenericImage<P2>::roi_pixel_iterator it( m_data.targetImage, rect );

         double sourceScaleX = 0, sourceScaleY = 0;
         bool sourceResampled = m_data.sourceImage.Width() != m_data.R.SourceSolution().Width()
                             || m_data.sourceImage.Height() != m_data.R.SourceSolution().Height();
         if ( sourceResampled )
         {
            sourceScaleX = double( m_data.sourceImage.Width() )/m_data.R.SourceSolution().Width();
            sourceScaleY = double( m_data.sourceImage.Height() )/m_data.R.SourceSolution().Height();
         }

         double targetScaleX = 0, targetScaleY = 0;
         bool targetResampled = m_data.targetImage.Width() != m_data.R.TargetSolution().Width()
                             || m_data.targetImage.Height() != m_data.R.TargetSolution().Height();
         if ( targetResampled )
         {
            targetScaleX = double( m_data.R.TargetSolution().Width() )/m_data.targetImage.Width();
            targetScaleY = double( m_data.R.TargetSolution().Height() )/m_data.targetImage.Height();
         }

         for ( int y = rect.y0; y < rect.y1; ++y )
            for ( int x = rect.x0; x < rect.x1; ++x, ++it )
            {
               DPoint pRD, pI( x, y );
               if ( targetResampled )
               {
                  pI.x *= targetScaleX;
                  pI.y *= targetScaleY;
               }
               if ( m_data.R.TargetSolution().ImageToCelestial( pRD, pI ) )
                  if ( m_data.R.SourceSolution().CelestialToImage( pI, pRD ) )
                  {
                     if ( sourceResampled )
                     {
                        pI.x *= sourceScaleX;
                        pI.y *= sourceScaleY;
                     }
                     if (  pI.x >= 0
                        && pI.x < m_data.sourceImage.Width()
                        && pI.y >= 0
                        && pI.y < m_data.sourceImage.Height() )
                     {
                        for ( int c = 0; c < m_data.sourceImage.NumberOfChannels(); ++c )
                        {
                           it[c] = P2::ToSample( interpolators[c]( pI ) );
                           if ( m_data.sourceImage( pI.TruncatedToInt(), c ) == P1::MinSampleValue() )
                              ++m_zeroCount;
                        }
                     }
                  }

               UPDATE_THREAD_MONITOR( 16 )
            }

         interpolators.Destroy();
      }

      size_type ZeroCount() const
      {
         return m_zeroCount;
      }

   private:

      ReprojectionThreadData<P1,P2>& m_data;
      size_type                      m_zeroCount = 0;
      int                            m_firstRow;
      int                            m_endRow;
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
// EOF pcl/AstrometricReprojection.cpp - Released 2024-12-27T18:16:14Z
