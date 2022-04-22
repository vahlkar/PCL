//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.28
// ----------------------------------------------------------------------------
// pcl/StarDetector.cpp - Released 2022-04-22T19:28:42Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2022 Pleiades Astrophoto S.L. All Rights Reserved.
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
#include <pcl/Console.h>
#include <pcl/Convolution.h>
#include <pcl/GaussianFilter.h>
#include <pcl/MetaModule.h>
#include <pcl/MorphologicalTransformation.h>
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
#define SD_COST      11

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
    * Adaptive binarization based on noise evaluation.
    */
   double median = map.Median();
   if ( 1 + median == 1 )
   {
      // An empty image, or a noise-free synthetic star field.
      map.SetRangeClipping( 0, 1 );
      median = map.Median();
      map.Binarize( median + map.MAD( median ) );  // N
      map.DisableRangeClipping();
   }
   else
   {
      // A "natural image" or something similar.
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

struct StarData
{
   DPoint pos;  // barycenter image coordinates
   Rect   rect;
   float  bkg;  // local background
   float  flux; // total flux
   float  peak; // peak value
   int    size; // structure size in square pixels
};

static void
GetStarParameters( StarData& data, const Image& image, const Rect& rect, const Array<Point>& sp )
{
   /*
    * Calculate a mean local background as the median of four selected regions
    * of background pixels.
    */
   {
      Rect r = rect.InflatedBy( BKG_DELTA );
      FMatrix bt = FMatrix::FromImage( image, Rect(    r.x0,    r.y0,    r.x1, rect.y0 ) );
      FMatrix bl = FMatrix::FromImage( image, Rect(    r.x0, rect.y0, rect.x0, rect.y1 ) );
      FMatrix bb = FMatrix::FromImage( image, Rect(    r.x0, rect.y1,    r.x1,    r.y1 ) );
      FMatrix br = FMatrix::FromImage( image, Rect( rect.x1, rect.y0,    r.x1, rect.y1 ) );
      Array<float> b;
      b.Add( bt.Begin(), bt.End() );
      b.Add( bl.Begin(), bl.End() );
      b.Add( bb.Begin(), bb.End() );
      b.Add( br.Begin(), br.End() );
      data.rect = r;
      data.bkg = Median( b.Begin(), b.End() );
   }

   /*
    * Compute barycenter coordinates.
    */
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
      {
         data.flux = 0;
         return;
      }
      data.pos.x = sx/sz;
      data.pos.y = sy/sz;
   }

   /*
    * Total flux, peak value, and structure size.
    */
   data.flux = data.peak = 0;
   for ( Array<Point>::const_iterator i = sp.Begin(); i != sp.End(); ++i )
   {
      float f = image( *i );
      data.flux += f;
      if ( f > data.peak )
         data.peak = f;
   }
   data.size = int( sp.Length() );
};

// ----------------------------------------------------------------------------

struct PSFFitData
{
   DPoint pos;
   Rect   rect;
   float  flux;
};

class PCL_SD_PSFFitThread : public Thread
{
public:

   typedef StarDetector::psf_function  psf_function;

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
                  stars << StarDetector::Star( fit.psf.c0 - 0.5, rect, fit.psf.flux, fit.psf.signal, fit.psf.mad );

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
                    (m_noiseReductionFilterRadius <= 0) ? m_hotPixelFilterRadius : 0, nullptr/*m_mask*/ );

   if ( initializeStatus )
   {
      image.Status().Initialize( "Detecting stars", size_type( image.Width()-1 )*size_type( image.Height()-1 ) );
      image.Status().DisableInitialization();
   }

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
          * If this is a reliable star, compute its barycenter coordinates
          * and add it to the star list.
          *
          * Rejection criteria:
          *
          * * If this structure is touching a border of the image, reject
          *   it. We cannot compute an accurate position for a clipped star.
          *
          * * Too small structures are rejected. This mainly prevents
          *   inclusion of hot (or cold) pixels. This condition is enforced
          *   by the hot pixel removal and noise reduction steps performed
          *   during the structure detection phase.
          *
          * * Too large structures are rejected. This prevents inclusion of
          *   extended nonstellar objects and saturated bright stars. This
          *   is also part of the structure detection algorithm.
          *
          * * Too elongated stars are rejected. The m_maxDistortion parameter
          *   determines the maximum distortion allowed. A perfect square
          *   has distortion = 1. The distortion of a perfect circle is
          *   pi/4, or about 0.8.
          *
          * * We don't trust stars whose centroids are too misplaced with
          *   respect to their peak positions. This prevents detection of
          *   multiple stars, where an accurate position cannot be computed.
          *
          * * Too flat structures are rejected. The m_peakResponse parameter
          *   defines a peakedness threshold necessary for a structure to be
          *   idenfified as a valid star.
          */
         if ( r.Width() > 1 && r.Height() > 1 )
            if ( r.y0 > 0 && r.y1 <= y1 && r.x0 > 0 && r.x1 <= x1 )
               if ( sp.Length() >= size_type( m_minStructureSize ) )
               {
                  int d = Max( r.Width(), r.Height() );
                  if ( float( sp.Length() )/d/d > m_maxDistortion )
                  {
                     StarData data;
                     GetStarParameters( data, image, r, sp );
                     if ( data.flux > 0 )
                        if ( data.peak <= m_upperLimit )
                        {
                           Point p = (data.pos + 0.5).TruncatedToInt();
                           if ( m_mask == nullptr || (*m_mask)( p ) != 0 )
                           {
                              // Detection level corrected for peak response
                              float f = data.peak - (1 - m_peakResponse)*data.flux/data.size;
                              if ( data.bkg == 0 || (f - data.bkg)/data.bkg > m_sensitivity )
                                 if ( image( p ) > 0.85*data.peak )
                                    if ( image.Median( r ) < m_peakResponse*data.peak )
                                       if ( m_fitPSF )
                                          psfData << PSFFitData{ data.pos, data.rect, data.flux };
                                       else
                                          S << Star( data.pos, data.rect, data.flux );
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

   if ( initializeStatus )
      image.Status().EnableInitialization();

   S.Sort();
   return S;
}

// ----------------------------------------------------------------------------

StarDetector::star_list StarDetector::DetectStars( const ImageVariant& image ) const
{
   Image I;
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

   I.Status() = image.Status();
   return DetectStars( I );
}

// ----------------------------------------------------------------------------

Image StarDetector::StructureMap( const ImageVariant& image ) const
{
   Image I;
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

   I.Status() = image.Status();
   if ( m_invert )
      I.Invert();
   GetStructures( I, m_structureLayers, m_noiseLayers, m_hotPixelFilterRadius, m_mask );
   return I;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/StarDetector.cpp - Released 2022-04-22T19:28:42Z
