//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// pcl/PSFEstimator.cpp - Released 2023-12-01T19:15:53Z
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

#include <pcl/AutoStatusCallbackRestorer.h>
#include <pcl/MuteStatus.h>
#include <pcl/PSFEstimator.h>
#include <pcl/QuadTree.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_PSFE_PSFFitThread : public Thread
{
public:

   using psf_function = PSFEstimator::psf_function;

   Array<PSFData> psfs;

   PCL_PSFE_PSFFitThread( const AbstractImage::ThreadData& data,
                          const ImageVariant& image, psf_function psfType,
                          float tolerance, float growth,
                          const StarDetector::star_list& stars, int start, int end )
      : m_data( data )
      , m_image( image )
      , m_psfType( psfType )
      , m_tolerance( tolerance )
      , m_growth( growth )
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
         PSFData psf;
         for ( int it = 0; it < 4; ++it )
         {
            psf_function psfType;
            switch ( it )
            {
            default: // ?!
            case 0: psfType = (m_psfType == PSFunction::Auto) ? PSFunction::Moffat25 : m_psfType; break;
            case 1: psfType = PSFunction::Moffat4; break;
            case 2: psfType = PSFunction::Moffat6; break;
            case 3: psfType = PSFunction::MoffatA; break;
            }

            PSFFit fit( m_image, star.pos + 0.5, rect, psfType, false/*circular*/,
                        1.0F/*betaMin*/, 4.0F/*betaMax*/,
                        1.0e-06/*tolerance*/, 0.1F/*bkgMaxVar*/, m_growth );
            if ( fit )
               if ( it == 0 || fit.psf.mad < psf.mad )
                  if ( DRect( rect ).DeflatedBy( rect.Width()*0.15 ).Includes( fit.psf.c0 ) )
                     if ( fit.psf.c0.DistanceTo( star.pos + 0.5 ) < m_tolerance )
                        psf = fit.psf;

            if ( m_psfType != PSFunction::Auto )
               break;
         }

         if ( psf )
            psfs << psf;

         UPDATE_THREAD_MONITOR( 16 )
      }
   }

private:

   const AbstractImage::ThreadData& m_data;
   const ImageVariant&              m_image;
         psf_function               m_psfType;
         float                      m_tolerance;
         float                      m_growth;
   const StarDetector::star_list&   m_stars;
         int                        m_start, m_end;
};

// ----------------------------------------------------------------------------

Array<PSFData> PSFEstimator::FitStars( const ImageVariant& image ) const
{
   bool initializeStatus = image.Status().IsInitializationEnabled();

   Array<PSFData> psfs;

   /*
    * Perform star detection
    */
   StarDetector::star_list stars;
   {
      volatile AutoStatusCallbackRestorer saveStatus( image.Status() );

      MuteStatus status;
      image.SetStatusCallback( &status );
      image.Status().DisableInitialization();

      m_starDetector.DisablePSFFitting();
      m_starDetector.SetUpperLimit( m_saturationRelative ? m_saturationThreshold*image.MaximumSampleValue()
                                                         : double( m_saturationThreshold ) );
      m_starDetector.EnableParallelProcessing( IsParallelProcessingEnabled() );
      m_starDetector.SetMaxProcessors( MaxProcessors() );
      stars = m_starDetector.DetectStars( image );

      if ( initializeStatus )
         image.Status().EnableInitialization();
   }

   if ( !stars.IsEmpty() )
   {
      /*
       * Optional limit on the number of measured stars. StarDetector returns a
       * list of stars sorted by flux in descending order, so we are always
       * restricting measurements to a subset of the brightest stars.
       */
      size_type numberOfStars = (m_maxStars > 0) ? Min( size_type( m_maxStars ), stars.Length() ) : stars.Length();

      if ( initializeStatus )
      {
         image.Status().Initialize( String().Format( "Fitting %u stars", numberOfStars ), numberOfStars );
         image.Status().DisableInitialization();
      }

      /*
       * Perform PSF fitting
       */
      Array<size_type> L = Thread::OptimalThreadLoads( numberOfStars,
                                          1/*overheadLimit*/,
                                          IsParallelProcessingEnabled() ? MaxProcessors() : 1 );
      ReferenceArray<PCL_PSFE_PSFFitThread> threads;
      AbstractImage::ThreadData data( *image, numberOfStars );
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new PCL_PSFE_PSFFitThread( data, image,
                                                 m_psfType, m_psfCentroidTolerance, m_growthForFlux,
                                                 stars, n, n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );

      for ( const PCL_PSFE_PSFFitThread& thread : threads )
         psfs << thread.psfs;

      threads.Destroy();

      if ( !psfs.IsEmpty() )
      {
         /*
          * Remove highly uncertain sources
          */
         QuadTree<PSFData> T( psfs );
         Array<PSFData> P;
         for ( const PSFData& p : psfs )
            if ( T.Search( QuadTree<PSFData>::rectangle( p[0] - 1, p[1] - 1,
                                                         p[0] + 1, p[1] + 1 ) ).Length() == 1 )
               P << p;
         psfs = P;
      }

      if ( !psfs.IsEmpty() )
      {
         /*
          * PSF flux estimates can optionally be weighted by inverse mean
          * absolute deviations of fitted PSFs with respect to sampled data. In
          * such a case the minimum PSF mean absolute deviation determines the
          * maximum sample weight.
          */
         if ( m_weighted )
         {
            double w1 = psfs[0].mad;
            for ( const PSFData& psf : psfs )
               if ( psf.mad < w1 )
                  w1 = psf.mad;
            for ( PSFData& psf : psfs )
               psf.signal *= w1/psf.mad;
         }

         /*
          * Optional rejection of a prescribed fraction of the brightest signal
          * estimates. This is normally not necessary (hence m_rejectionLimit
          * has a default value of 1.0) if the saturation threshold parameter
          * has been set to a reasonable value. Validity of the dimmest and
          * brightest measurements in the psfs list should already be ensured
          * by robust star detection.
          */
         psfs.Sort( []( const PSFData& p, const PSFData& q ){ return double( p ) < double( q ); } );
         psfs.Resize( Min( RoundInt( m_rejectionLimit*psfs.Length() ), int( psfs.Length() ) ) );
      }
   }

   if ( initializeStatus )
      image.Status().EnableInitialization();

   return psfs;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/PSFEstimator.cpp - Released 2023-12-01T19:15:53Z
