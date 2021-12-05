//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.15
// ----------------------------------------------------------------------------
// pcl/PSFSignalEstimator.cpp - Released 2021-11-25T11:44:55Z
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

class PCL_SE_PSFFitThread : public Thread
{
public:

   typedef PSFSignalEstimator::psf_function  psf_function;

   Array<PSFData> psfs;

   PCL_SE_PSFFitThread( const AbstractImage::ThreadData& data,
                        const ImageVariant& image, psf_function psfType, float tolerance,
                        const StarDetector::star_list& stars, int start, int end )
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

         /*
          * Adaptive PSF sampling region.
          */
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

         /*
          * PSF fitting and validation. Always fit elliptical functions instead
          * of circular ones for more adaptive and accurate signal evaluation.
          */
         PSFFit fit( m_image, star.pos + 0.5, rect, m_psfType, false/*circular*/,
                     1.0F/*betaMin*/, 4.0F/*betaMax*/,
                     1.0e-06/*tolerance*/, 0.1F/*bkgMaxVar*/ );
         if ( fit )
            if ( fit.psf.meanSignal > 0 )
               if ( fit.psf.meanSignalSqr > 0 )
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
   const StarDetector::star_list&   m_stars;
         int                        m_start, m_end;
};

// ----------------------------------------------------------------------------

PSFSignalEstimator::Estimates PSFSignalEstimator::EstimateSignal( const ImageVariant& image ) const
{
   bool initializeStatus = image.Status().IsInitializationEnabled();

   /*
    * Perform star detection
    */
   m_starDetector.DisablePSFFitting();
   m_starDetector.EnableParallelProcessing( IsParallelProcessingEnabled() );
   m_starDetector.SetMaxProcessors( MaxProcessors() );
   StarDetector::star_list stars = m_starDetector.DetectStars( image );

   Estimates E;

   if ( !stars.IsEmpty() )
   {
      /*
       * Optional limit on the number of measured stars. StarDetector returns
       * a list of stars sorted by flux in descending order, so we are always
       * restricting measurements to a subset of the brightest stars.
       */
      size_type numberOfStars = (m_maxStars > 0) ? Min( size_type( m_maxStars ), stars.Length() ) : stars.Length();

      if ( initializeStatus )
      {
         image.Status().EnableInitialization();
         image.Status().Initialize( String().Format( "Fitting %u stars", numberOfStars ), numberOfStars );
         image.Status().DisableInitialization();
      }

      /*
       * Perform PSF fitting
       */
      Array<size_type> L = Thread::OptimalThreadLoads( numberOfStars,
                                          1/*overheadLimit*/,
                                          IsParallelProcessingEnabled() ? MaxProcessors() : 1 );
      ReferenceArray<PCL_SE_PSFFitThread> threads;
      AbstractImage::ThreadData data( *image, numberOfStars );
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new PCL_SE_PSFFitThread( data, image, m_psfType, m_psfCentroidTolerance, stars, n, n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );

      Array<PSFData> psfs;
      for ( const PCL_SE_PSFFitThread& thread : threads )
         psfs << thread.psfs;

      threads.Destroy();

      if ( !psfs.IsEmpty() )
      {
         /*
          * Use a robust sigma-clipping scheme for outlier rejection with
          * bilateral scale estimates. Here we want to reject signal estimates
          * that are:
          *
          * - Too dim, since they can be unreliable because of poor SNR.
          *
          * - Too bright, since they can be unreliable because of clipping,
          *   relative saturation or nonlinearity.
          *
          * The resulting set of inliers should be an accurate and robust
          * sample of the true mean signal gathered in the image.
          */
         psfs.Sort( []( const PSFData& p, const PSFData& q ){ return p.meanSignal < q.meanSignal; } );
         int N = int( psfs.Length() );
         int t1 = 0, t2 = N;
         for ( TwoSidedEstimate s0 = 0;; )
         {
            if ( t2 - t1 <= 3 )
               break;
            double m = Median( psfs.At( t1 ), psfs.At( t2 ) );
            TwoSidedEstimate s = TwoSidedAvgDev( psfs.At( t1 ), psfs.At( t2 ), m );
            double m1 = m - m_rejectionLimit*1.2533*s.low;
            double m2 = m + m_rejectionLimit*1.2533*s.high;
            for ( ; psfs[t1].meanSignal < m1; ++t1 ) {}
            for ( ; psfs[t2-1].meanSignal > m2; --t2 ) {}
            if ( Abs( double( s ) - double( s0 ) ) < 1.0e-6 )
               break;
            s0 = s;
         }

         /*
          * Compute efficient average signal and squared signal estimates as
          * trimmed means after sigma clipping.
          */
         if ( (E.count = t2 - t1) > 0 )
         {
            /*
             * PSF flux estimates can optionally be weighted by inverse mean
             * absolute deviations of fitted PSFs with respect to sampled data.
             * In such case the minimum PSF mean absolute deviation determines
             * the maximum sample weight.
             */
            double mmin = 0;
            if ( m_weighted )
            {
               mmin = psfs[t1].mad;
               for ( int i = t1; ++i <= t2; )
                  if ( psfs[i].mad < mmin )
                     mmin = psfs[i].mad;
            }

            /*
             * Accumulate PSF fluxes and intensities.
             */
            double W = 0;
            double psfNorm = 0;
            for ( int i = t2; --i >= t1; )
            {
               if ( unlikely( m_weighted ) )
               {
                  double w = mmin/psfs[i].mad;
                  E.meanFlux += psfs[i].meanSignal * w;
                  E.powerFlux += psfs[i].meanSignalSqr * w;
                  W += w;
               }
               else
               {
                  E.meanFlux += psfs[i].meanSignal;
                  E.powerFlux += psfs[i].meanSignalSqr;
               }

               psfNorm += psfs[i].A + psfs[i].B;
            }

            /*
             * Mean PSF flux.
             */
            if ( m_weighted )
            {
               E.meanFlux /= W;
               E.powerFlux /= W;
            }
            else
            {
               E.meanFlux /= E.count;
               E.powerFlux /= E.count;
            }

            /*
             * Mean PSF signal estimates. The penalty function is the ratio of
             * the total measured PSF intensity to the norm of the image.
             */
            image.PushSelections();
            image.SetRangeClipping( 2.0/65535, 1 - 2.0/65535 );
            double P = psfNorm/image.Norm();
            image.PopSelections();
            E.mean = E.meanFlux * P;
            E.power = E.powerFlux * P;
         }
      }
   }

   if ( initializeStatus )
      image.Status().EnableInitialization();

   return E;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/PSFSignalEstimator.cpp - Released 2021-11-25T11:44:55Z
