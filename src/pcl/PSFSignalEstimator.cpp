//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.12
// ----------------------------------------------------------------------------
// pcl/PSFSignalEstimator.cpp - Released 2021-10-20T18:04:06Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/PSFSignalEstimator.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PSFFitThread : public Thread
{
public:

   typedef PSFSignalEstimator::psf_function  psf_function;

   Array<PSFData> psfs;

   PSFFitThread( const AbstractImage::ThreadData& data,
                 const ImageVariant& image, psf_function psfType, float tolerance,
                 const Array<StarDetector::Star>& stars, int start, int end )
      : m_data( data )
      , m_image( image )
      , m_psfType( psfType )
      , m_tolerance( tolerance )
      , m_stars( stars )
      , m_start( start )
      , m_end( end )
   {
   }

   PCL_HOT_FUNCTION void Run() override
   {
      INIT_THREAD_MONITOR()

      for ( int i = m_start; i < m_end; ++i )
      {
         const StarDetector::Star& star = m_stars[i];

         // Adaptive PSF sampling region
         int size = Max( star.rect.Width(), star.rect.Height() );
         Rect rect = DRect( star.pos + (0.5-size), star.pos + (0.5+size) ).TruncatedToInt();
         for ( double m0 = 1;; )
         {
            double m = FMatrix::FromImage( m_image, rect ).Median();
            if ( m0 <= m || (m0 - m)/m0 < 0.01 )
               break;
            m0 = m;
            rect.InflateBy( 1, 1 );
         }

         PSFFit fit( m_image, star.pos + 0.5, rect, m_psfType, false/*circular*/ );
         if ( fit )
            if ( DRect( rect ).DeflatedBy( rect.Width()*0.15 ).Includes( fit.psf.c0 ) )
               if ( fit.psf.c0.DistanceTo( star.pos + 0.5 ) < m_tolerance )
                  psfs << fit.psf;

         UPDATE_THREAD_MONITOR( 16 )
      }
   }

private:

   const AbstractImage::ThreadData& m_data;
   const ImageVariant&              m_image;
         psf_function               m_psfType;
         float                      m_tolerance;
   const Array<StarDetector::Star>& m_stars;
         int                        m_start, m_end;
};

// ----------------------------------------------------------------------------

PSFSignalEstimator::Estimates PSFSignalEstimator::EstimateSignal( const ImageVariant& image ) const
{
   bool initializeStatus = image.Status().IsInitializationEnabled();

   StarDetector S;
   S.DisablePSFFitting();
   S.EnableParallelProcessing( IsParallelProcessingEnabled() );
   S.SetMaxProcessors( MaxProcessors() );
   StarDetector::star_list stars = S.DetectStars( image );

   if ( !stars.IsEmpty() )
   {
      if ( initializeStatus )
      {
         image.Status().EnableInitialization();
         image.Status().Initialize( String().Format( "Fitting %u stars", stars.Length() ), stars.Length() );
         image.Status().DisableInitialization();
      }

      Array<size_type> L = Thread::OptimalThreadLoads( stars.Length(),
                                          1/*overheadLimit*/,
                                          IsParallelProcessingEnabled() ? MaxProcessors() : 1 );
      ReferenceArray<PSFFitThread> threads;
      AbstractImage::ThreadData data( *image, stars.Length() );
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new PSFFitThread( data, image, m_psfType, m_psfTolerance, stars, n, n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );

      Array<PSFData> psfs;
      for ( const PSFFitThread& thread : threads )
         psfs << thread.psfs;

      threads.Destroy();

      if ( !psfs.IsEmpty() )
      {
         psfs.Sort( []( const PSFData& p, const PSFData& q ){ return p.meanSignal > q.meanSignal; } );
         int N = int( psfs.Length() );
         int t = TruncInt( 0.15*N );
         Estimates E;
         for ( int i = t; i < N-t; ++i )
            if ( psfs[i].meanSignal > 0 )
               if ( psfs[i].meanSignalSqr > 0 )
               {
                  E.mean += psfs[i].meanSignal;
                  E.power += psfs[i].meanSignalSqr;
                  ++E.count;
               }
         E.mean /= E.count;
         E.power /= E.count;
         return E;
      }
   }

   if ( initializeStatus )
      image.Status().EnableInitialization();

   return Estimates();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/PSFSignalEstimator.cpp - Released 2021-10-20T18:04:06Z
