//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.0
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.25.0
// ----------------------------------------------------------------------------
// AdaptiveNormalizationData.cpp - Released 2020-07-31T19:33:39Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
//
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L. All Rights Reserved.
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
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
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

namespace pcl
{

// ----------------------------------------------------------------------------

AdaptiveNormalizationData::AdaptiveNormalizationData( const Image& image, int nx, int ny )
{
   nx = Range( nx, 2, 16 );
   ny = Range( ny, 2, 16 );

   int n = nx * ny;
   int dx = image.Width() / nx;
   int dy = image.Height() / ny;

   image.ResetSelections();
   image.SetRangeClipping( 1.0/65535, 1 - 1.0/65535 );

   m_x = DVector( n );
   m_y = DVector( n );

   for ( int c = 0; c < image.NumberOfChannels(); ++c )
   {
      image.SelectChannel( c );

      DVector m( n ), s0( n ), s1( n );
      for ( int i = 0, k = 0; i < ny; ++i )
      {
         int y0 = i*dy;
         int y1 = y0 + dy;
         for ( int j = 0; j < nx; ++j, ++k )
         {
            int x0 = j*dx;
            int x1 = x0 + dx;
            image.SelectRectangle( x0, y0, x1, y1 );
            const Rect& r = image.SelectedRectangle();
            if ( c == 0 )
            {
               m_x[k] = 0.5*(r.x0 + r.x1);
               m_y[k] = 0.5*(r.y0 + r.y1);
            }
            m[k] = image.Median();
            TwoSidedEstimate s2 = image.TwoSidedMAD( m[k] );
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

      local_interpolation M, S0, S1;
      M.Initialize( m_x.Begin(), m_y.Begin(), m.Begin(), n );
      S0.Initialize( m_x.Begin(), m_y.Begin(), s0.Begin(), n );
      S1.Initialize( m_x.Begin(), m_y.Begin(), s1.Begin(), n );

      m_location << M;
      m_scaleLow << S0;
      m_scaleHigh << S1;
   }

   image.ResetSelections();
}

// ----------------------------------------------------------------------------

AdaptiveNormalizationData::AdaptiveNormalizationData( const DVector& x, const DVector& y,
                                                      const DMultiVector& m, const DMultiVector& s0, const DMultiVector& s1 )
   : m_x( x )
   , m_y( y )
   , m_m( m )
   , m_s0( s0 )
   , m_s1( s1 )
{
   if ( m_x.Length() != m_y.Length() || m_x.Length() < 4 )
      throw Error( "AdaptiveNormalizationData: Invalid coordinate vectors." );
   if ( m_m.Length() != m_s0.Length() || m_m.Length() != m_s1.Length() || m_m.IsEmpty() )
      throw Error( "AdaptiveNormalizationData: Invalid sample vectors." );

   for ( size_type i = 0; i < m_m.Length(); ++i )
   {
      if ( m_m[i].Length() != m_x.Length() || m_s0[i].Length() != m_x.Length() || m_s1[i].Length() != m_x.Length() )
         throw Error( "AdaptiveNormalizationData: Invalid spline vectors." );

      local_interpolation M, S0, S1;
      M.Initialize( m_x.Begin(), m_y.Begin(), m_m[i].Begin(), m_x.Length() );
      S0.Initialize( m_x.Begin(), m_y.Begin(), m_s0[i].Begin(), m_x.Length() );
      S1.Initialize( m_x.Begin(), m_y.Begin(), m_s1[i].Begin(), m_x.Length() );

      m_location << M;
      m_scaleLow << S0;
      m_scaleHigh << S1;
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AdaptiveNormalizationData.cpp - Released 2020-07-31T19:33:39Z
