//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// StarDetector.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "StarDetector.h"

#include <pcl/Matrix.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static
void Threshold( FMatrix& V, float threshold )
{
   V.Truncate( Range( FMatrix( V ).Median() + threshold*V.StdDev(), 0.0, 1.0 ), 1.0 );
   V.Rescale();
}

static
StarDetector::Status Detect( const Image& image, int channel,
                             DPoint& pos, int& radius, float threshold )
{
   /*
    * Iteratively find the barycenter of a star.
    */
   for ( int it = 0; it < 10; ++it )
   {
      // Central pixel in the current search box
      Point p0 = pos;

      // Search box
      Rect r0( p0-radius, p0+radius+1 );
      if ( !image.Intersects( r0 ) )
         return StarDetector::OutsideImage;

      // Extract the search subimage
      image.Clip( r0 ); // in case the search box is clipped
      FMatrix V = FMatrix::FromImage( image, r0, channel );

      // Threshold background pixels
      Threshold( V, threshold );

      // Begin searching from the brightest pixel
      if ( V.MaxElement( p0 ) == V.MinElement() )
         return StarDetector::NoSignificantData;

      // Coordinate and intensity accumulators.
      double sx = 0, sy = 0, si = 0;

      // Star bounding rectangle.
      Rect r( p0, p0 );

      for ( int down = 0; down < 2; ++down )
      {
         if ( down ? (p0.y == V.Rows()-1) : (p0.y == 0) )
            continue;

         for ( int y = down ? p0.y+1 : p0.y; ; )
         {
            double yc = y + 0.5;
            int xa, xb;

            /*
             * Explore the left segment of this row.
             */
            for ( xa = p0.x+1; xa > 0; )
            {
               float f = V[y][xa-1];
               if ( f == 0 )
                  break;
               --xa;
               sx += f*(xa + 0.5);
               sy += f*yc;
               si += f;
            }

            /*
             * Explore the right segment of this row.
             */
            for ( xb = p0.x; xb < V.Cols()-1; )
            {
               float f = V[y][xb+1];
               if ( f == 0 )
                  break;
               ++xb;
               sx += f*(xb + 0.5);
               sy += f*yc;
               si += f;
            }

            /*
             * Update horizontal boundaries.
             */
            if ( xa < r.x0 )  // left boundary
               r.x0 = xa;
            if ( xb >= r.x1 ) // right boundary
               r.x1 = xb+1;

            /*
             * Update y to explore the next row.
             */
            if ( down )
            {
               ++y;
               if ( y == V.Rows() )
                  break;
            }
            else
            {
               if ( y == 0 )
                  break;
               --y;
            }

            /*
             * Decide whether we are done with this star, or if there is at
             * least one more row that has to be explored. This is true if
             * there is at least one significant pixel touching the current row
             * in the next row.
             */
            bool nextRow = false;
            for ( int x = xa; x <= xb; ++x )
               if ( V[y][x] != 0 )
               {
                  nextRow = true;
                  break;
               }
            if ( !nextRow )
               break;

            /*
             * Update vertical boundaries.
             */
            if ( down )
               r.y1 = y+1; // bottom boundary
            else
               r.y0 = y;   // top boundary
         }
      }

      /*
       * Check if we have gathered some data.
       */
      if ( 1 + si == 1 )
         return StarDetector::NoSignificantData;

      /*
       * Update barycenter coordinates.
       */
      DPoint lastPos = pos;
      pos.x = r0.x0 + sx/si;
      pos.y = r0.y0 + sy/si;

      /*
       * Update search radius.
       */
      int r1 = Range( Max( r.Width(), r.Height() ), 5, 127 );
      if ( r1 != radius )
         radius = r1;
      else
      {
         /*
          * If the search radius has stabilized, check if we have reached
          * convergence. We converge to within +/- 0.01 px.
          */
         if ( Abs( pos.x - lastPos.x ) < 0.005 )
            if ( Abs( pos.y - lastPos.y ) < 0.005 )
               return (r.x0 > 0 && r.y0 > 0 && r.x1 < V.Cols() && r.y1 < V.Rows()) ?
                           StarDetector::DetectedOk : StarDetector::CrossingEdges;
      }
   }

   return StarDetector::NoConvergence;
}

// ----------------------------------------------------------------------------

StarDetector::StarDetector( const Image& image, int channel,
                            const DPoint& pos, int radius, float threshold, bool autoAperture )
{
   star.status = NotDetected;
   star.channel = channel;
   star.rect = pos;
   star.pos = pos;
   star.status = Detect( image, channel, star.pos, radius, threshold );
   star.rect = star.drawRect = DRect( star.pos - double( radius ), star.pos + double( radius ) );

   if ( star )
      if ( autoAperture )
      {
         Rect r = star.rect.RoundedToInt();
         double m0 = image.Median( r, channel, channel );
         for ( int i = 0;; )
         {
            Rect r1 = r.InflatedBy( 1, 1 );
            double m = image.Median( r1, channel, channel );
            if ( m > m0 || (m0 - m)/m0 < 0.01 )
               break;
            if ( ++i == 200 )
            {
               // Guard us against rare ill-posed conditions.
               star.status = StarDetector::NoConvergence;
               break;
            }
            m0 = m;
            r = r1;
         }
         star.rect = star.drawRect = r;
      }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF StarDetector.cpp - Released 2024-12-28T16:54:15Z
