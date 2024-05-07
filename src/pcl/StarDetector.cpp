//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.11
// ----------------------------------------------------------------------------
// pcl/StarDetector.cpp - Released 2024-05-07T15:27:40Z
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

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/AutoStatusCallbackRestorer.h>
#include <pcl/Console.h>
#include <pcl/Convolution.h>
#include <pcl/GaussianFilter.h>
#include <pcl/MetaModule.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/MuteStatus.h>
#include <pcl/QuadTree.h>
#include <pcl/Selection.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/StarDetector.h>

#include <float.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * The local background is evaluated for each star on an inflated rectangular
 * region around the star detection structure. bkgDelta is the inflation
 * distance in pixels.
 */
#define BKG_DELTA    3

/*
 * Stretch factor for the barycenter search algorithm, in sigma units.
 * Increase it to make the algorithm more robust to nearby structures, such as
 * multiple/crowded stars and small nebular features. However, too large of a
 * stretch factor will make the algorithm less accurate.
 */
#define XY_STRETCH   1.5

/*
 * Computational cost of the structure detection algorithm.
 */
#define SD_COST      12

/*
 * Computational cost of the local maxima detection algorithm.
 */
#define MD_COST      3

// ----------------------------------------------------------------------------

static void HotPixelFilter( Image& image, int hotPixelFilterRadius )
{
   if ( hotPixelFilterRadius > 0 ) // N
   {
      if ( hotPixelFilterRadius > 1 )
         MorphologicalTransformation( MedianFilter(), CircularStructure( 2*hotPixelFilterRadius + 1 ) ) >> image;
      else
         MorphologicalTransformation( MedianFilter(), BoxStructure( 3 ) ) >> image;
   }
   else
      image.Status() += image.NumberOfPixels();
}

// ----------------------------------------------------------------------------

static void NoiseLayersFilter( Image& image, int noiseLayers )
{
   if ( noiseLayers > 0 ) // N
   {
      GaussianFilter G( 1 + (1 << noiseLayers) );
      if ( G.Size() < SeparableConvolution::FasterThanNonseparableFilterSize( Thread::NumberOfThreads( PCL_MAX_PROCESSORS ) ) )
         Convolution( G ) >> image;
      else
         SeparableConvolution( G.AsSeparableFilter() ) >> image;
   }
   else
      image.Status() += image.NumberOfPixels();
}

// ----------------------------------------------------------------------------

static void GetStructureMap( Image& map, int structureLayers, int noiseLayers, int hotPixelFilterRadius,
                             const UInt8Image* mask = nullptr )
{
   bool initializeStatus = map.Status().IsInitializationEnabled();
   if ( initializeStatus )
   {
      map.Status().Initialize( "Structure map", map.NumberOfPixels()*SD_COST );
      map.Status().DisableInitialization();
   }

   /*
    * Optional hot pixel removal with a median filter.
    */
   HotPixelFilter( map, hotPixelFilterRadius ); // N

   /*
    * Optional noise reduction with a low-pass filter.
    */
   NoiseLayersFilter( map, noiseLayers ); // N

   /*
    * Flatten the image with a high-pass filter.
    */
   {
      Image s( map );
      GaussianFilter G( 1 + (1 << structureLayers) );
      if ( G.Size() < SeparableConvolution::FasterThanNonseparableFilterSize( Thread::NumberOfThreads( PCL_MAX_PROCESSORS ) ) )
         Convolution( G ) >> s;  // N
      else
         SeparableConvolution( G.AsSeparableFilter() ) >> s;
      map.Status() = s.Status();
      map.Sub( s );     // N
      map.Truncate();   // N
      map.Rescale();    // N
   }

   /*
    * Strength the smallest structures with a dilation filter.
    */
   MorphologicalTransformation( DilationFilter(), BoxStructure( 3 ) ) >> map; // N

   /*
    * Adaptive binarization.
    */
   double median = map.Median();
   if ( 1 + median == 1 )
   {
      // Black background - probably a noiseless synthetic star field
      map.SetRangeClipping( 0, 1 );
      double median = map.Median();
      map.Binarize( median + map.MAD( median ) );  // N
      map.DisableRangeClipping();
   }
   else
   {
      // A "natural" image - binarize at 3*noise_stdDev
      static const float B3[] =
      {
         0.003906F, 0.015625F, 0.023438F, 0.015625F, 0.003906F,
         0.015625F, 0.062500F, 0.093750F, 0.062500F, 0.015625F,
         0.023438F, 0.093750F, 0.140625F, 0.093750F, 0.023438F,
         0.015625F, 0.062500F, 0.093750F, 0.062500F, 0.015625F,
         0.003906F, 0.015625F, 0.023438F, 0.015625F, 0.003906F
      };
      static const float B3v[] = { 0.0625F, 0.25F, 0.375F, 0.25F, 0.0625F };
      static const float B3k[] = { 0.8907F, 0.2007F, 0.0856F };

      ATrousWaveletTransform::WaveletScalingFunction H;
      if ( SeparableConvolution::FasterThanNonseparableFilterSize( Thread::NumberOfThreads( PCL_MAX_PROCESSORS ) ) > 5 )
         H.Set( KernelFilter( B3, 5 ) );
      else
         H.Set( SeparableFilter( B3v, B3v, 5 ) );
      ATrousWaveletTransform W( H, 2 );
      W << map; // 2*N
      float noise = W.NoiseKSigma( 1/*j*/, 3/*k*/, 0.01F/*eps*/, 10/*nit*/ )/B3k[1];
      map.Binarize( median + 3*noise ); // N
   }

   /*
    * Remove noise residuals with an erosion filter.
    */
   MorphologicalTransformation( ErosionFilter(), BoxStructure( 3 ) ) >> map; // N

   /*
    * Optional detection mask.
    */
   if ( mask != nullptr )
      map.Mul( *mask );          // N
   else
      map.Status() += map.NumberOfPixels();

   if ( initializeStatus )
      map.Status().EnableInitialization();
}

// ----------------------------------------------------------------------------

static void GetStructures( Image& image, int structureLayers, int noiseLayers, int hotPixelFilterRadius, const UInt8Image* mask )
{
   bool initializeStatus = image.Status().IsInitializationEnabled();
   if ( initializeStatus )
   {
      image.Status().Initialize( "Extracting structures", image.NumberOfPixels()*(SD_COST + 1) );
      image.Status().DisableInitialization();
   }

   Image map( image );
   GetStructureMap( map, structureLayers, noiseLayers, hotPixelFilterRadius, mask );
   image.Status() = map.Status();
   image.Mul( map );               // N

   if ( initializeStatus )
      image.Status().EnableInitialization();
}

// ----------------------------------------------------------------------------

static void GetLocalMaximaMap( Image& map, int filterRadius, float detectionLimit,
                               const UInt8Image* mask = nullptr )
{
   bool initializeStatus = map.Status().IsInitializationEnabled();
   if ( initializeStatus )
   {
      map.Status().Initialize( "Local maxima map", map.NumberOfPixels()*MD_COST );
      map.Status().DisableInitialization();
   }

   /*
    * We apply a dilation filter with a flat structuring element without its
    * central element. Local maxima are those pixels in the input image with
    * values greater than the dilated image.
    *
    * The detectionLimit parameter allows us to prevent detection of false
    * multiple maxima on saturated or close to saturated structures.
    */
   int filterSize = (filterRadius << 1)|1;
   IsoString B( 'x',  filterSize*filterSize );
   B[B.Length() >> 1] = '-';
   const char* Bs = B.c_str();
   Image s( map );
   MorphologicalTransformation( DilationFilter(), BitmapStructure( &Bs, filterSize ) ) >> s; // N
   map.Status() = s.Status();
   Image::const_sample_iterator d( s );
   for ( Image::sample_iterator m( map ); m; ++m, ++d, ++map.Status() ) // N
      *m = (*m > *d && *m < detectionLimit) ? 1.0F : 0.0F;

   /*
    * Optional detection mask.
    */
   if ( mask != nullptr )
      map.Mul( *mask );          // N
   else
      map.Status() += map.NumberOfPixels();

   if ( initializeStatus )
      map.Status().EnableInitialization();
}

// ----------------------------------------------------------------------------

struct StarParameters
{
   DPoint pos = 0;   // barycenter image coordinates
   Rect   rect = 0;  // detection region
   Rect   srect = 0; // sampling region
   float  bkg = 0;   // local background
   float  sigma = 0; // local background dispersion
   float  flux = 0;  // total flux
   float  max = 0;   // maximum pixel value
   int    nmax = 0;  // number of local maxima in structure
   float  peak = 0;  // significant peak value
   float  kurt = 0;  // kurtosis
   int    count = 0; // sample length
};

static bool
GetStarParameters( StarParameters& params, const Image& image, const Rect& rect, const Array<Point>& sp,
                   const Image& localMaximaMap, bool noLocalMaximaDetection, bool allowClusteredSources )
{
   // Mean local background and local background dispersion.
   double m0 = 1;
   for ( int delta = 4, it = 0;; ++delta, ++it )
   {
      Rect r = rect.InflatedBy( delta );
      Array<float> b;
      for ( Image::const_roi_sample_iterator i( image, Rect(    r.x0,    r.y0,    r.x1, rect.y0 ) ); i; ++i )
         b << *i;
      for ( Image::const_roi_sample_iterator i( image, Rect(    r.x0, rect.y0, rect.x0, rect.y1 ) ); i; ++i )
         b << *i;
      for ( Image::const_roi_sample_iterator i( image, Rect(    r.x0, rect.y1,    r.x1,    r.y1 ) ); i; ++i )
         b << *i;
      for ( Image::const_roi_sample_iterator i( image, Rect( rect.x1, rect.y0,    r.x1, rect.y1 ) ); i; ++i )
         b << *i;
      double m = Median( b.Begin(), b.End() );
      if ( m > m0 || (m0 - m)/m0 < 0.01 )
      {
         // Mean local background
         params.bkg = m;
         // Local background dispersion
         params.sigma = Max( float( 1.4826*MAD( b.Begin(), b.End(), m ) ), std::numeric_limits<float>::epsilon() );
         // Sampling rectangle
         params.srect = r.Intersection( image.Bounds() );
         break;
      }
      // Guard us against rare ill-posed conditions.
      if ( it == 200 )
         return false;
      m0 = m;
   }

   // Detection rectangle. Expand it by two pixels to ensure that the entire
   // structure is included in the detection region.
   params.rect = rect.InflatedBy( 2 ).Intersection( image.Bounds() );

   // Significant pixel subset
   Array<float> v;
   for ( const Point& p : sp )
   {
      float f = image( p );
      if ( f > params.bkg )
      {
         // Local maxima
         if ( !noLocalMaximaDetection )
            if ( localMaximaMap( p ) != 0 )
               ++params.nmax;
         v << f;
         // Total flux above local background
         params.flux += f;
      }
   }

   // Fail if no significant data
   if ( v.IsEmpty() )
      return false;

   // Fail if we have multiple maxima and those are not allowed
   if ( params.nmax > 1 )
      if ( !allowClusteredSources )
         return false;

   // Barycenter coordinates
   {
      FMatrix Z = FMatrix::FromImage( image, rect );
      for ( float& z : Z )
         if ( z < 1.0e-07 ) // zero out insignificant values
            z = 0;
      Z.Truncate( Range( FMatrix( Z ).Median() + XY_STRETCH*Z.StdDev(), 0.0, 1.0 ), 1.0 );
      Z.Rescale();
      const float* z = Z.Begin();
      double sx = 0, sy = 0, sz = 0;
      for ( int y = rect.y0; y < rect.y1; ++y )
         for ( int x = rect.x0; x < rect.x1; ++x, ++z )
            if ( *z > 0 )
            {
               sx += *z * x;
               sy += *z * y;
               sz += *z;
            }
      if ( 1 + sz == 1 )
         return false;
      params.pos.x = sx/sz;
      params.pos.y = sy/sz;
   }

   // Sort significant pixels in decreasing flux order
   v.Sort( []( float a, float b ) { return b < a; } );
   // Maximum pixel value
   params.max = v[0];
   // Find subset of significant high pixel values
   int mn = 0;
   for ( size_type i = 0; i < v.Length() && (mn < 5 || v[i] == v[i-1]); ++i, ++mn ) {}
   for ( int i = 0; i < mn; ++i )
      params.peak += v[i];
   // Significant peak value
   params.peak /= mn;
   // Significant sample length
   params.count = int( v.Length() );

   // Kurtosis
   double s = StdDev( v.Begin(), v.End() );
   if ( 1 + s != 1 )
   {
      double m = double( params.flux )/params.count;
      double k = 0;
      for ( float f : v )
      {
         double d = (f - m)/s;
         d *= d;
         k += d*d;
      }
      params.kurt = k/params.count;
   }

   return true;
};

// ----------------------------------------------------------------------------

struct PSFFitData
{
   DPoint pos;
   Rect   rect;
};

class PCL_SD_PSFFitThread : public Thread
{
public:

   using psf_function = StarDetector::psf_function;

   StarDetector::star_list stars;

   PCL_SD_PSFFitThread( const AbstractImage::ThreadData& data,
                        Image& image, psf_function psfType, bool circular, float tolerance,
                        const Array<PSFFitData>& psfData, int start, int end )
      : m_data( data )
      , m_image( image )
      , m_psfType( psfType )
      , m_circular( circular )
      , m_tolerance( tolerance )
      , m_psfData( psfData )
      , m_start( start )
      , m_end( end )
   {
   }

   PCL_HOT_FUNCTION void Run() override
   {
      INIT_THREAD_MONITOR()

      for ( int i = m_start; i < m_end; ++i )
      {
         const PSFFitData& d = m_psfData[i];

         // Adaptive PSF sampling region
         int size = Max( d.rect.Width(), d.rect.Height() );
         Rect rect = DRect( d.pos + (0.5-size), d.pos + (0.5+size) ).TruncatedToInt();
         for ( double m0 = 1;; )
         {
            double m = FMatrix::FromImage( m_image, rect ).Median();
            if ( m0 <= m || (m0 - m)/m0 < 0.01 )
               break;
            m0 = m;
            rect.InflateBy( 1, 1 );
         }

         PSFFit fit( ImageVariant( &m_image ), d.pos + 0.5, rect, m_psfType, m_circular );
         if ( fit )
            if ( DRect( rect ).DeflatedBy( rect.Width()*0.15 ).Includes( fit.psf.c0 ) )
               if ( fit.psf.c0.DistanceTo( d.pos + 0.5 ) < m_tolerance )
                  stars << StarDetector::Star( fit.psf.c0 - 0.5, d.rect/*detection*/, rect/*sampling*/,
                                               0.7854*fit.psf.FWTMx()*fit.psf.FWTMy()/*area*/,
                                               fit.psf.flux, fit.psf.signal, fit.psf.mad );

         UPDATE_THREAD_MONITOR( 16 )
      }
   }

private:

   const AbstractImage::ThreadData& m_data;
         Image&                     m_image;
         psf_function               m_psfType;
         bool                       m_circular;
         float                      m_tolerance;
   const Array<PSFFitData>&         m_psfData;
         int                        m_start, m_end;
};

StarDetector::star_list StarDetector::DetectStars( Image& image ) const
{
   if ( image.IsEmpty() )
      return star_list();

   /*
    * If the invert flag is set, then we are looking for dark structures on a
    * bright background.
    */
   if ( m_invert )
      image.Invert();

   bool initializeStatus = image.Status().IsInitializationEnabled();

   /*
    * Optional noise reduction
    */
   if ( m_noiseReductionFilterRadius > 0 )
   {
      if ( initializeStatus )
      {
         image.Status().Initialize( "Noise reduction", 2*image.NumberOfPixels() );
         image.Status().DisableInitialization();
      }

      // If noise reduction is enabled, remove hot pixels first - Otherwise
      // they would be "promoted to stars".
      HotPixelFilter( image, m_hotPixelFilterRadius ); // N

      GaussianFilter G( (m_noiseReductionFilterRadius << 1)|1 );
      if ( G.Size() < SeparableConvolution::FasterThanNonseparableFilterSize( Thread::NumberOfThreads( PCL_MAX_PROCESSORS ) ) )
         Convolution( G ) >> image;  // N
      else
         SeparableConvolution( G.AsSeparableFilter() ) >> image;

      if ( initializeStatus )
         image.Status().EnableInitialization();
   }

   /*
    * Structure map
    */
   Image map( image );
   GetStructureMap( map, m_structureLayers, m_noiseLayers,
                    (m_noiseReductionFilterRadius <= 0) ? m_hotPixelFilterRadius : 0 );

   /*
    * Local maxima map
    */
   Image lmMap;
   if ( !m_noLocalMaximaDetection )
   {
      lmMap = image;
      GetLocalMaximaMap( lmMap, m_localDetectionFilterRadius, m_localMaximaDetectionLimit );
   }

   if ( initializeStatus )
   {
      image.Status().Initialize( "Detecting stars", size_type( image.Width()-1 )*size_type( image.Height()-1 ) );
      image.Status().DisableInitialization();
   }

   /*
    * Internal detection parameters
    */
   // Signal detection threshold in SNR units.
   float snrThreshold = 0.1 + 4.8*(1 - Range( m_sensitivity, 0.0F, 1.0F ));
   // Peak detection threshold in kurtosis units.
   float peakThreshold = 0.1 + 9.8*(1 - Range( m_peakResponse, 0.0F, 1.0F ));
   // Maximum distortion in coverage units.
   float minCoverage = Const<float>::pi4()*(1 - Range( m_maxDistortion, 0.0F, 1.0F ));

   /*
    * Star detection
    */
   star_list S;
   Array<PSFFitData> psfData;

   for ( int y0 = 0, x1 = image.Width()-1, y1 = image.Height()-1; y0 < y1; ++y0 )
      for ( int x0 = 0; x0 < x1; ++x0, ++image.Status() )
      {
         // Exclude background pixels and already visited pixels
         if ( map( x0, y0 ) == 0 )
            continue;

         // Star bounding rectangle
         Rect r( x0, y0, x0+1, y0+1 );

         // Current star points
         Array<Point> sp;

         // Grow star region downward
         for ( int y = y0, x = x0, xa, xb; ; )
         {
            // Add this pixel to the current star
            sp << Point( x, y );

            // Explore left segment of this row
            for ( xa = x; xa > 0; )
            {
               if ( map( xa-1, y ) == 0 )
                  break;
               --xa;
               sp << Point( xa, y );
            }

            // Explore right segment of this row
            for ( xb = x; xb < x1; )
            {
               if ( map( xb+1, y ) == 0 )
                  break;
               ++xb;
               sp << Point( xb, y );
            }

            // xa and xb are now the left and right boundary limits,
            // respectively, of this row in the current star.

            if ( xa < r.x0 )  // update left boundary
               r.x0 = xa;

            if ( xb >= r.x1 ) // update right boundary
               r.x1 = xb + 1; // bottom-right corner excluded (PCL-specific)

            // Prepare for next row
            ++y;

            // Decide whether we are done with this star now, or if
            // there is at least one more row that must be explored.

            bool nextRow = false;

            // We explore the next row from left to right. We'll continue
            // gathering pixels if we find at least one nonzero pixel.
            for ( x = xa; x <= xb; ++x )
               if ( map( x, y ) != 0 )
               {
                  nextRow = true;
                  break;
               }

            if ( !nextRow )
               break;

            // Update bottom boundary
            r.y1 = y + 1;  // Rectangle<int> *excludes* the bottom-right corner

            // Terminate if we reach the last row of the image
            if ( y == y1 )
               break;
         }

         /*
          * If this is a reliable star, compute its parameters and add it to
          * the star list.
          *
          * Rejection criteria:
          *
          * * Stars whose peak values are greater than the upperLimit parameter
          *   are rejected.
          *
          * * If this structure is touching a border of the image, reject it.
          *   We cannot compute an accurate position for a clipped star.
          *
          * * Too small structures are rejected. This mainly prevents inclusion
          *   of hot (or cold) pixels. This condition is enforced by the hot
          *   pixel removal and noise reduction steps performed during the
          *   structure detection phase, and optionally by increasing the
          *   minStructureSize parameter.
          *
          * * Too large structures are rejected. This prevents inclusion of
          *   extended nonstellar objects and saturated bright stars. This is
          *   also part of the structure detection algorithm.
          *
          * * Too elongated stars are rejected. The minCoverage parameter
          *   determines the maximum distortion allowed. A perfect square has
          *   coverage = 1. The coverage of a perfect circle is pi/4.
          *
          * * Too sparse sources are rejected. This prevents detection of
          *   multiple stars where centroids cannot be well determined.
          *
          * * Too dim structures are rejected. The sensitivity parameter
          *   defines the sensitivity of the star detection algorithm in local
          *   sigma units. The minSNR parameter can be used to limit star
          *   detection to a subset of the brightest stars adaptively.
          *
          * * Too flat structures are rejected. The peakThreshold parameter
          *   defines the peak sensitivity of the star detection algorithm in
          *   kurtosis units.
          */
         if ( r.Width() > 1 && r.Height() > 1 )
            if ( r.y0 > 0 && r.y1 <= y1 && r.x0 > 0 && r.x1 <= x1 )
               if ( int( sp.Length() ) >= m_minStructureSize )
               {
                  StarParameters params;
                  if ( GetStarParameters( params, image, r, sp, lmMap, m_noLocalMaximaDetection, m_allowClusteredSources ) )
                     if ( params.max <= m_upperLimit )
                     {
                        int d = Max( r.Width(), r.Height() );
                        if ( float( params.count )/d/d >= minCoverage )
                        {
                           Point p = (params.pos + 0.5).TruncatedToInt();
                           if ( m_mask == nullptr || (*m_mask)( p ) != 0 )
                           {
                              float snr = (params.peak - params.bkg)/params.sigma;
                              if ( snr >= m_minSNR )
                              {
                                 float s1 = snr/snrThreshold;
                                 if ( s1 >= 1 )
                                    if ( s1 >= m_brightThreshold || params.kurt == 0 || params.kurt/peakThreshold >= 1 )
                                       if ( m_fitPSF )
                                          psfData << PSFFitData{ params.pos, params.rect };
                                       else
                                          S << Star{ params.pos, params.rect, params.srect, float( sp.Length() ), params.flux };
                              }
                           }
                        }
                     }
               }

         // Erase this structure
         for ( const Point& p : sp )
            map( p ) = 0;
      }

   /*
    * PSF fitting
    */
   if ( !psfData.IsEmpty() )
   {
      if ( initializeStatus )
      {
         image.Status().EnableInitialization();
         image.Status().Initialize( String().Format( "Fitting %u stars", psfData.Length() ), psfData.Length() );
         image.Status().DisableInitialization();
      }

      Array<size_type> L = Thread::OptimalThreadLoads( psfData.Length(),
                                          1/*overheadLimit*/,
                                          IsParallelProcessingEnabled() ? MaxProcessors() : 1 );
      ReferenceArray<PCL_SD_PSFFitThread> threads;
      AbstractImage::ThreadData data( image, psfData.Length() );
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads.Add( new PCL_SD_PSFFitThread( data, image, m_psfType, !m_psfElliptic, m_psfCentroidTolerance,
                                               psfData, n, n + int( L[i] ) ) );
      AbstractImage::RunThreads( threads, data );

      for ( const PCL_SD_PSFFitThread& thread : threads )
         S << thread.stars;

      threads.Destroy();
   }

   /*
    * Minimum star size rejection.
    */
   m_minStarSize = 0;
   if ( m_minStructureSize == 0 ) // if automatic MSS mode
   {
      Array<int> sizes;
      for ( const Star& s : S )
         sizes << RoundInt( s.area );
      sizes.Sort();

      Array<int> deltas;
      for ( int i = 0, lastDelta = 0; ++i < int( sizes.Length() ); )
      {
         int delta = sizes[i] - sizes[i-1];
         if ( delta > lastDelta )
         {
            deltas << delta;
            lastDelta = delta;
         }
      }
      if ( !deltas.IsEmpty() )
      {
         int bandwidth = RoundInt( Median( deltas.Begin(), deltas.End() ) );
//          File::WriteTextFile( "/tmp/deltas.txt", IsoString().ToSeparated( deltas, ",\n" ) );

         Array<Array<int>> clusters;
         Array<int> currentCluster;
         currentCluster << sizes[0];
         for ( int i = 0; ++i < int( sizes.Length() ); )
         {
            if ( sizes[i] >= sizes[i-1] + bandwidth )
            {
               clusters << currentCluster;
               currentCluster = Array<int>();
            }
            currentCluster << sizes[i];
         }
         clusters << currentCluster;

//          IsoString text;
//          text << IsoString().Format( "B = %d\n", bandwidth );
//          for ( const Array<int>& cluster : clusters )
//             text << "{\n   "
//                << IsoString().ToSeparated( cluster,  ",\n   " )
//                << "\n}\n";
//          File::WriteTextFile( "/tmp/clusters.txt", text );

         if ( clusters.Length() < 2 || clusters[0].Length() >= clusters[1].Length() )
            m_minStarSize = Max( 1, clusters[0][0] );
         else
            m_minStarSize = clusters[1][0];
      }
   }

   /*
    * Remove too small and potentially conflicting sources.
    */
   QuadTree<Star> T( S );
   Array<Star> S1;
   for ( const Star& s : S )
      if ( s.area >= m_minStarSize )
         if ( T.Search( QuadTree<Star>::rectangle( s.pos.x - 1, s.pos.y - 1,
                                                   s.pos.x + 1, s.pos.y + 1 ) ).Length() == 1 )
            S1 << s;
   S = S1;

   if ( initializeStatus )
      image.Status().EnableInitialization();

   S.Sort();
   return S;
}

// ----------------------------------------------------------------------------

StarDetector::star_list StarDetector::DetectStars( const ImageVariant& image ) const
{
   Image I;
   {
      volatile AutoStatusCallbackRestorer saveStatus( image.Status() );
      bool initializeStatus = image.Status().IsInitializationEnabled();
      MuteStatus status;
      image.SetStatusCallback( &status );
      image.Status().DisableInitialization();

      ImageVariant V( &I );
      if ( image.NumberOfSelectedChannels() == image.NumberOfNominalChannels() )
         image.GetIntensity( V );
      else
      {
         image.PushSelections();
         image.SelectChannel( image.SelectedChannel() );
         V.AssignImage( image );
         image.PopSelections();
      }

      if ( initializeStatus )
         image.Status().EnableInitialization();
   }

   I.Status() = image.Status();
   return DetectStars( I );
}

// ----------------------------------------------------------------------------

Image StarDetector::StructureMap( const ImageVariant& image ) const
{
   Image I;
   {
      volatile AutoStatusCallbackRestorer saveStatus( image.Status() );
      bool initializeStatus = image.Status().IsInitializationEnabled();
      MuteStatus status;
      image.SetStatusCallback( &status );
//       image.Status().DisableInitialization();

      ImageVariant V( &I );
      if ( image.NumberOfSelectedChannels() == image.NumberOfNominalChannels() )
         image.GetIntensity( V );
      else
      {
         image.PushSelections();
         image.SelectChannel( image.SelectedChannel() );
         V.AssignImage( image );
         image.PopSelections();
      }

      if ( initializeStatus )
         image.Status().EnableInitialization();
   }

   I.Status() = image.Status();
   if ( m_invert )
      I.Invert();
   GetStructureMap( I, m_structureLayers, m_noiseLayers, m_hotPixelFilterRadius, m_mask );
   return I;
}

// ----------------------------------------------------------------------------

Image StarDetector::Structures( const ImageVariant& image ) const
{
   Image I;
   {
      volatile AutoStatusCallbackRestorer saveStatus( image.Status() );
      bool initializeStatus = image.Status().IsInitializationEnabled();
      MuteStatus status;
      image.SetStatusCallback( &status );
//       image.Status().DisableInitialization();

      ImageVariant V( &I );
      if ( image.NumberOfSelectedChannels() == image.NumberOfNominalChannels() )
         image.GetIntensity( V );
      else
      {
         image.PushSelections();
         image.SelectChannel( image.SelectedChannel() );
         V.AssignImage( image );
         image.PopSelections();
      }

      if ( initializeStatus )
         image.Status().EnableInitialization();
   }

   I.Status() = image.Status();
   if ( m_invert )
      I.Invert();
   GetStructures( I, m_structureLayers, m_noiseLayers, m_hotPixelFilterRadius, m_mask );
   return I;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/StarDetector.cpp - Released 2024-05-07T15:27:40Z
