//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.5
// ----------------------------------------------------------------------------
// CalibrationUtility.cpp - Released 2024-12-23T11:33:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "CalibrationUtility.h"

#include <pcl/Console.h>
#include <pcl/Graphics.h>
#include <pcl/ImageVariant.h>
#include <pcl/ImageWindow.h>
#include <pcl/Random.h>
#include <pcl/StandardStatus.h>

namespace pcl
{

// ----------------------------------------------------------------------------

template <class P>
static DVector ComputeBackgroundReference( const GenericImage<P>& image, double sigmaLow, double sigmaHigh )
{
   StandardStatus status;
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   String monitorInfo = "Evaluating background reference";
   if ( !image.IsFullSelection() )
   {
      Rect r = image.SelectedRectangle();
      monitorInfo << String().Format( " (ROI: x=%d y=%d w=%d h=%d)", r.x0, r.y0, r.Width(), r.Height() );
   }
   monitor.Initialize( monitorInfo, image.NumberOfSelectedPixels() );

   DVector low( 3 ), high( 3 );
   for ( int c = 0; c < 3; ++c )
   {
      image.SelectChannel( c );
      double median = image.Median();
      double sigma = 1.4826 * image.MAD( median );
      low[c] = Range( median + sigmaLow*sigma, 0.0, 1.0 );
      high[c] = Range( median + sigmaHigh*sigma, 0.0, 1.0 );
   }
   image.ResetChannelRange();

   Array<double> B0R, B0G, B0B;
   for ( typename GenericImage<P>::const_roi_pixel_iterator i( image ); i; ++i, ++monitor )
   {
      double R; P::FromSample( R, i[0] );
      double G; P::FromSample( G, i[1] );
      double B; P::FromSample( B, i[2] );
      if ( R > low[0] && R < high[0] )
         B0R << R;
      if ( G > low[1] && G < high[1] )
         B0G << G;
      if ( B > low[2] && B < high[2] )
         B0B << B;
   }
   return DVector( pcl::Median( B0R.Begin(), B0R.End() ),
                   pcl::Median( B0G.Begin(), B0G.End() ),
                   pcl::Median( B0B.Begin(), B0B.End() ) );
}

DVector ComputeBackgroundReference( const ImageVariant& image, double sigmaLow, double sigmaHigh )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: return ComputeBackgroundReference( static_cast<const Image&>( *image ), sigmaLow, sigmaHigh );
      case 64: return ComputeBackgroundReference( static_cast<const DImage&>( *image ), sigmaLow, sigmaHigh );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: return ComputeBackgroundReference( static_cast<const UInt8Image&>( *image ), sigmaLow, sigmaHigh );
      case 16: return ComputeBackgroundReference( static_cast<const UInt16Image&>( *image ), sigmaLow, sigmaHigh );
      case 32: return ComputeBackgroundReference( static_cast<const UInt32Image&>( *image ), sigmaLow, sigmaHigh );
      }

   return 0;
}

// ----------------------------------------------------------------------------

template <class P>
static void ApplyBackgroundNeutralization( GenericImage<P>& image, const DVector& backgroundReference )
{
   StandardStatus status;
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Applying background neutralization", image.NumberOfPixels() );

   for ( typename GenericImage<P>::pixel_iterator i( image ); i; ++i, ++monitor )
      for ( int c = 0; c < 3; ++c )
      {
         double v; P::FromSample( v, i[c] );
         i[c] = P::ToSample( v - backgroundReference[c] );
      }

   image.Normalize();
}

void ApplyBackgroundNeutralization( ImageVariant& image, const DVector& backgroundReference )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: ApplyBackgroundNeutralization( static_cast<Image&>( *image ), backgroundReference ); break;
      case 64: ApplyBackgroundNeutralization( static_cast<DImage&>( *image ), backgroundReference ); break;
      }
   else
   {
      ImageVariant tmp;
      tmp.CreateFloatImage( (image.BitsPerSample() > 16) ? 64 : 32 );
      tmp.CopyImage( image );
      ApplyBackgroundNeutralization( tmp, backgroundReference );
      image.CopyImage( tmp );
   }
}

// ----------------------------------------------------------------------------

template <class P>
static void ApplyWhiteBalance( GenericImage<P>& image, const DVector& W )
{
   StandardStatus status;
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Applying white balance", image.NumberOfPixels() );

   for ( typename GenericImage<P>::pixel_iterator i( image ); i; ++i, ++monitor )
   {
      double v0; P::FromSample( v0, i[0] );
      double v1; P::FromSample( v1, i[1] );
      double v2; P::FromSample( v2, i[2] );
      i[0] = P::ToSample( v0 * W[0] );
      i[1] = P::ToSample( v1 * W[1] );
      i[2] = P::ToSample( v2 * W[2] );
   }
}

void ApplyWhiteBalance( ImageVariant& image, const DVector& W )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: ApplyWhiteBalance( static_cast<Image&>( *image ), W ); break;
      case 64: ApplyWhiteBalance( static_cast<DImage&>( *image ), W ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: ApplyWhiteBalance( static_cast<UInt8Image&>( *image ), W ); break;
      case 16: ApplyWhiteBalance( static_cast<UInt16Image&>( *image ), W ); break;
      case 32: ApplyWhiteBalance( static_cast<UInt32Image&>( *image ), W ); break;
      }
}

// ----------------------------------------------------------------------------

#define RME_LARGE_SAMPLE   10000

/*
 * Fit two vectors to a straight line with robust linear regression.
 *
 * We implement the repeated median estimator of slope (Siegel, 1982) with an
 * approximate randomized version for large sets of more than 10,000 points.
 */
LinearFit RobustLinearRegression( const Vector& x, const Vector& y, const String& what )
{
   int N = x.Length();

   StandardStatus status;
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Robust linear regression (" + what + ")", N );

   if ( N < 5 || y.Length() < 5 )
      throw Error( "Insufficient data: only " + String( x.Length() ) + " sample(s) are available; at least 5 are required." );

   GenericVector<DPoint> P( N );
   for ( int i = 0; i < N; ++i )
      P[i] = DPoint( x[i], y[i] );

   Vector M;
   if ( N <= RME_LARGE_SAMPLE )
   {
      M = Vector( N-1 );
      for ( int i = 0; i < N-1; ++i, ++monitor )
      {
         Array<double> Mi;
         for ( int j = i; ++j < N; )
         {
            double dx = P[j].x - P[i].x;
            if ( unlikely( Abs( dx ) < 2*std::numeric_limits<double>::epsilon() ) )
               continue;
            Mi << (P[j].y - P[i].y)/dx;
         }
         M[i] = Median( Mi.Begin(), Mi.End() );
      }
      ++monitor;
   }
   else
   {
      M = Vector( N );
      XoShiRo256ss R;
      for ( int i = 0; i < N; ++i, ++monitor )
      {
         Array<double> Mi;
         for ( int n = 0; n < RME_LARGE_SAMPLE; ++n )
         {
            int j = int( R.UI64N( N ) );
            double dx = P[j].x - P[i].x;
            if ( unlikely( Abs( dx ) < 2*std::numeric_limits<double>::epsilon() ) )
            {
               --n;
               continue;
            }
            Mi << (P[j].y - P[i].y)/dx;
         }
         M[i] = Median( Mi.Begin(), Mi.End() );
      }
   }

   LinearFit L;
   L.b = M.Median();
   L.adev = 1.1926*M.Sn();
   M = Vector( N );
   for ( int i = 0; i < N; ++i )
      M[i] = y[i] - L.b*x[i];
   L.a = M.Median();
   return L;

// ----------------------------------------------------------------------------
// Non-robust regression methods for tests.
// ----------------------------------------------------------------------------
//
//    /*
//     * Average absolute deviation minimization.
//     */
//    return LinearFit( x, y );
//
//    /*
//     * Simple linear regression.
//     */
//    int N = x.Length();
//    double sumX = 0, sumX2 = 0, sumY = 0, sumXY = 0;
//    for ( int i = 0; i < N; ++i )
//    {
//       sumX = sumX + x[i];
//       sumX2 = sumX2 + x[i]*x[i];
//       sumY = sumY + y[i];
//       sumXY = sumXY + x[i]*y[i];
//    }
//    LinearFit L;
//    L.b = (N*sumXY - sumX*sumY)/(N*sumX2 - sumX*sumX);
//    L.a = (sumY - L.b*sumX)/N;
//    return L;
}

// ----------------------------------------------------------------------------

static Bitmap RenderStars( int width, int height, const Array<StarPSFSample>& stars )
{
   Bitmap bmp( width, height );
   bmp.Fill( 0 ); // transparent

   VectorGraphics G( bmp );
   G.EnableAntialiasing();
   G.SetPen( 0xff000000 ); // solid black

   for ( const StarPSFSample& s : stars )
   {
      DPoint q( s.pos.x + 0.5, s.pos.y + 0.5 );
      G.DrawLine( q.x, q.y-5, q.x, q.y-1 );
      G.DrawLine( q.x, q.y+1, q.x, q.y+5 );
      G.DrawLine( q.x-5, q.y, q.x-1, q.y );
      G.DrawLine( q.x+1, q.y, q.x+5, q.y );
//       G.StrokeEllipse( s.bounds );
   }

   G.EndPaint();

   return bmp;
}

void CreateStarMapWindow( int width, int height, const IsoString& id, const Array<StarPSFSample>& stars )
{
   ImageWindow window( 1, 1, 1, 8/*bitsPerSample*/, false/*floatSample*/, false/*color*/, true/*initialProcessing*/, id );
   if ( window.IsNull() )
      throw Error( "Unable to create image window: '" + id + '\'' );
   window.MainView().Image().AllocateData( width, height ).White().Blend( RenderStars( width, height, stars ) );
   window.Show();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CalibrationUtility.cpp - Released 2024-12-23T11:33:28Z
