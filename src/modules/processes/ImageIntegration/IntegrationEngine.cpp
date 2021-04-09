//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// IntegrationEngine.cpp - Released 2021-04-09T19:41:48Z
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

#include "IntegrationEngine.h"
#include "IntegrationFile.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IntegrationEngine::IntegrationEngine( const ImageIntegrationInstance& instance, StatusMonitor& monitor,
                                      RejectionStacks& stacks, const RejectionCounts& counts,
                                      const DVector& zeroOffset, DVector& location, const scale_estimates& scale,
                                      int startRow, int channel,
                                      float* result32, double* result64 )
   : IntegrationEngineBase( instance, monitor )
   , m_R( stacks ), m_N( counts )
   , m_d( zeroOffset ), m_m( location ), m_s( scale )
   , m_x0( instance.p_useROI ? instance.p_roi.x0 : 0 )
   , m_y0( startRow + (instance.p_useROI ? instance.p_roi.y0 : 0) )
   , m_channel( channel )
   , m_result32( result32 ), m_result64( result64 )
{
   Array<size_type> L = Thread::OptimalThreadLoads( m_R.Length() );
   for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
      m_threads << new IntegrationThread( *this, n, n + int( L[i] ) );
}

// ----------------------------------------------------------------------------

IntegrationEngine::~IntegrationEngine()
{
   m_threads.Destroy();
}

// ----------------------------------------------------------------------------

void IntegrationEngine::Integrate()
{
   if ( !m_threads.IsEmpty() )
   {
      m_threadData.total = m_R.Length();
      m_threadData.count = 0;
      AbstractImage::RunThreads( m_threads, m_threadData );
      m_monitor = m_threadData.status;
   }
}

// ----------------------------------------------------------------------------

#define E   m_engine
#define I   m_engine.m_instance

// ----------------------------------------------------------------------------

void IntegrationEngine::IntegrationThread::Run()
{
   INIT_THREAD_MONITOR()

   float* result32 = nullptr;
   double* result64 = nullptr;
   if ( I.p_generate64BitResult )
      result64 = E.m_result64 + size_type( m_firstStack )*size_type( IntegrationFile::Width() );
   else
      result32 = E.m_result32 + size_type( m_firstStack )*size_type( IntegrationFile::Width() );

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R )
   {
      const IVector& N = E.m_N[k];

      for ( int j = 0; j < R->Rows(); ++j )
      {
         RejectionDataItem* r = R->DataPtr()[j];
         int n = N[j];
         double f;

         pcl_enum thisCombination = I.p_combination;
         if ( n == 0 )
         {
            /*
             * If all pixels have been rejected, take the median as the final
             * pixel value for this stack.
             */
            n = IntegrationFile::NumberOfFiles();
            thisCombination = IICombination::Median;
         }

         const DVector& d = E.m_d;
         const DVector& m = E.m_m;
         const scale_estimates& s = E.m_s;

         /*
          * Normalize/scale output
          */
         switch ( I.p_normalization )
         {
         case IINormalization::NoNormalization:
            break;
         default:
         case IINormalization::Additive:
            for ( int i = 0; i < n; ++i )
               r[i].raw += d[r[i].index];
            break;
         case IINormalization::Multiplicative:
            for ( int i = 0; i < n; ++i )
               r[i].raw *= d[r[i].index];
            break;
         case IINormalization::AdditiveWithScaling:
            for ( int i = 0; i < n; ++i )
               r[i].raw = (double( r[i].raw ) - m[r[i].index])*double( s[r[i].index] ) + m[0];
            break;
         case IINormalization::MultiplicativeWithScaling:
            for ( int i = 0; i < n; ++i )
               r[i].raw = (double( r[i].raw ) / m[r[i].index])*double( s[r[i].index] ) * m[0];
            break;
         case IINormalization::LocalNormalization:
            for ( int i = 0, x = E.m_x0+j, y = E.m_y0+k; i < n; ++i )
               r[i].raw = IntegrationFile::FileByIndex( r[i].index ).Normalize( r[i].raw, x, y, E.m_channel );
            break;
         case IINormalization::AdaptiveNormalization:
            {
               int x = E.m_x0 + j;
               int y = E.m_y0 + k;
               const AdaptiveNormalizationData& a0 = IntegrationFile::FileByIndex( 0 ).AdaptiveNormalization();
               double m0 = a0.Location( x, y, E.m_channel );
               if ( I.p_adaptiveNoScale )
               {
                  for ( int i = 0; i < n; ++i )
                  {
                     const AdaptiveNormalizationData& a = IntegrationFile::FileByIndex( r[i].index ).AdaptiveNormalization();
                     double m = a.Location( x, y, E.m_channel );
                     r[i].raw = (r[i].raw - m)*((r[i].raw <= m) ? s[r[i].index].low : s[r[i].index].high) + m0;
                  }
               }
               else
               {
                  double s00 = a0.ScaleLow( x, y, E.m_channel );
                  double s10 = a0.ScaleHigh( x, y, E.m_channel );
                  for ( int i = 0; i < n; ++i )
                  {
                     const AdaptiveNormalizationData& a = IntegrationFile::FileByIndex( r[i].index ).AdaptiveNormalization();
                     double m = a.Location( x, y, E.m_channel );
                     r[i].raw = (r[i].raw - m)
                           * ((r[i].raw <= m) ? s00/a.ScaleLow( x, y, E.m_channel ) : s10/a.ScaleHigh( x, y, E.m_channel ))
                           + m0;
                  }
               }
            }
            break;
         }

         /*
          * Integrate
          */
         switch ( thisCombination )
         {
         default:
         case IICombination::Average:
            {
               f = 0;
               if ( I.p_weightMode == IIWeightMode::DontCare )
               {
                  for ( int i = 0; i < n; ++i, ++r )
                     f += r->raw;
                  f /= n;
               }
               else
               {
                  double ws = 0;
                  for ( int i = 0; i < n; ++i, ++r )
                  {
                     double w = IntegrationFile::FileByIndex( r->index ).Weight( E.m_channel );
                     f += w*r->raw;
                     ws += w;
                  }
                  f /= ws;
               }
            }
            break;

         case IICombination::Median:
         case IICombination::Minimum:
         case IICombination::Maximum:
            Sort( r, r+n, RejectionDataItem::CompareRaw );
            switch ( thisCombination )
            {
            default:
            case IICombination::Median:
               {
                  int n2 = n >> 1;
                  if ( n & 1 )
                     f = r[n2].raw;
                  else
                     f = 0.5*(r[n2].raw + r[n2-1].raw);
               }
               break;
            case IICombination::Minimum:
               f = r[0].raw;
               break;
            case IICombination::Maximum:
               f = r[n-1].raw;
               break;
            }
            break;
         }

         if ( result32 != nullptr )
            *result32++ = float( f );
         else
            *result64++ = f;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

#undef E
#undef I

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegrationEngine.cpp - Released 2021-04-09T19:41:48Z
