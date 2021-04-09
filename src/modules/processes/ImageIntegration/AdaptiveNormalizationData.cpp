//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// AdaptiveNormalizationData.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#include "AdaptiveNormalizationData.h"
#include "ImageIntegrationParameters.h"

#include <pcl/SurfaceSpline.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AdaptiveNormalizationData::AdaptiveNormalizationData( const Image& image, int scaleEstimator, int gridSize )
{
   m_width = image.Width();
   m_height = image.Height();

   gridSize = Range( gridSize, 2, 50 );
   int delta = Max( m_width, m_height )/gridSize;
   int nx = m_width/delta + (m_width%delta > 0);
   int ny = m_height/delta + (m_height%delta > 0);
   int dx = m_width/nx;
   int dy = m_height/ny;

   int offsetX = (m_width - nx*dx) >> 1;
   int offsetY = (m_height - ny*dy) >> 1;

   int n = nx * ny;

   m_x = DVector( n );
   m_y = DVector( n );

   image.ResetSelections();
   image.SetRangeClipping( 1.0/65535, 1 - 1.0/65535 );

   for ( int c = 0; c < image.NumberOfChannels(); ++c )
   {
      image.SelectChannel( c );

      DVector m( n ), s0( n ), s1( n );
      for ( int i = 0, k = 0; i < ny; ++i )
      {
         int y0 = i*dy - offsetY;
         int y1 = y0 + dy;
         for ( int j = 0; j < nx; ++j, ++k )
         {
            int x0 = j*dx - offsetX;
            int x1 = x0 + dx;
            image.SelectRectangle( x0, y0, x1, y1 );
            const Rect& r = image.SelectedRectangle();
            if ( c == 0 )
            {
               m_x[k] = 0.5*(r.x0 + r.x1);
               m_y[k] = 0.5*(r.y0 + r.y1);
            }
            m[k] = image.Median();
            TwoSidedEstimate s2;
            switch ( scaleEstimator )
            {
            case IIWeightScale::AvgDev:
               s2 = image.TwoSidedAvgDev( m[k] );
               break;
            case IIWeightScale::MAD:
               s2 = image.TwoSidedMAD( m[k] );
               if ( 1 + s2.low == 1 || 1 + s2.high == 1 )
                  s2.low = s2.high = image.MAD( m[k] );
               break;
            default:
            case IIWeightScale::BWMV:
               {
                  s2 = image.TwoSidedMAD( m[k] );
                  if ( 1 + s2.low == 1 || 1 + s2.high == 1 )
                     s2.low = s2.high = image.MAD( m[k] );
                  s2 = Sqrt( image.TwoSidedBiweightMidvariance( m[k], s2 ) );
               }
               break;
            }

            if ( !s2.IsValid() )
               throw Error( String().Format( "AdaptiveNormalizationData: Zero or insignificant signal detected "
                                             "(x0=%d y0=%d x1=%d y1=%d c=%d)", r.x0, r.y0, r.x1, r.y1, c ) );
            s0[k] = s2.low;
            s1[k] = s2.high;
         }
      }

      m_m << m;
      m_s0 << s0;
      m_s1 << s1;
   }

   image.ResetSelections();

   InitInterpolations();
}

// ----------------------------------------------------------------------------

AdaptiveNormalizationData::AdaptiveNormalizationData( int width, int height,
                                                      const DVector& x, const DVector& y,
                                                      const DMultiVector& m, const DMultiVector& s0, const DMultiVector& s1 )
   : m_width( width )
   , m_height( height )
   , m_x( x )
   , m_y( y )
   , m_m( m )
   , m_s0( s0 )
   , m_s1( s1 )
{
   if ( m_width <= 0 || m_height <= 0 )
      throw Error( "AdaptiveNormalizationData: Invalid image dimensions." );
   if ( m_x.Length() != m_y.Length() || m_x.Length() < 4 )
      throw Error( "AdaptiveNormalizationData: Invalid coordinate vectors." );
   if ( m_m.Length() != m_s0.Length() || m_m.Length() != m_s1.Length() || m_m.IsEmpty() )
      throw Error( "AdaptiveNormalizationData: Invalid sample vectors." );
   for ( size_type i = 0; i < m_m.Length(); ++i )
      if ( m_m[i].Length() != m_x.Length() || m_s0[i].Length() != m_x.Length() || m_s1[i].Length() != m_x.Length() )
         throw Error( "AdaptiveNormalizationData: Invalid spline vectors." );

   InitInterpolations();
}

// ----------------------------------------------------------------------------

bool AdaptiveNormalizationData::IsValid() const
{
   if ( m_width > 0 )
      if ( m_height > 0 )
         if ( m_x.Length() >= 4 )
            if ( m_x.Length() == m_y.Length() )
               if ( !m_m.IsEmpty() )
                  if ( m_m.Length() == m_s0.Length() )
                     if ( m_m.Length() == m_s1.Length() )
                     {
                        for ( size_type i = 0; i < m_m.Length(); ++i )
                           if ( m_m[i].Length() != m_x.Length() || m_s0[i].Length() != m_x.Length() || m_s1[i].Length() != m_x.Length() )
                              return false;
                        return true;
                     }
   return false;
}

// ----------------------------------------------------------------------------

void AdaptiveNormalizationData::Clear()
{
   m_width = m_height = -1;
   m_x.Clear();
   m_y.Clear();
   m_m.Clear();
   m_s0.Clear();
   m_s1.Clear();
   m_location.Clear();
   m_scaleLow.Clear();
   m_scaleHigh.Clear();
}

// ----------------------------------------------------------------------------

void AdaptiveNormalizationData::InitInterpolations()
{
   m_location.Clear();
   m_scaleLow.Clear();
   m_scaleHigh.Clear();

   for ( size_type i = 0; i < m_m.Length(); ++i )
   {
      SurfaceSpline<double> M, S0, S1;
      M.Initialize( m_x.Begin(), m_y.Begin(), m_m[i].Begin(), m_x.Length() );
      S0.Initialize( m_x.Begin(), m_y.Begin(), m_s0[i].Begin(), m_x.Length() );
      S1.Initialize( m_x.Begin(), m_y.Begin(), m_s1[i].Begin(), m_x.Length() );

      interpolation GM, GS0, GS1;
      GM.Initialize( Rect( m_width, m_height ), 64, M, false/*verbose*/ );
      GS0.Initialize( Rect( m_width, m_height ), 64, S0, false/*verbose*/ );
      GS1.Initialize( Rect( m_width, m_height ), 64, S1, false/*verbose*/ );

      m_location << GM;
      m_scaleLow << GS0;
      m_scaleHigh << GS1;
   }
}

// ----------------------------------------------------------------------------

int AdaptiveNormalizationData::NumberOfGridElements( int width, int height, int gridSize )
{
   /*
    * ### N.B. This must be coherent with the class constructor.
    */
   gridSize = Range( gridSize, 2, 50 );
   int delta = Max( width, height )/gridSize;
   int nx = width/delta + (width%delta > 0);
   int ny = height/delta + (height%delta > 0);
   return nx * ny;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AdaptiveNormalizationData.cpp - Released 2021-04-09T19:41:48Z
