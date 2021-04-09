//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// IntegrationRejectionEngine.cpp - Released 2021-04-09T19:41:48Z
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

#include "IntegrationFile.h"
#include "IntegrationRejectionEngine.h"

#include <pcl/LinearFit.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IntegrationRejectionEngine::IntegrationRejectionEngine( const ImageIntegrationInstance& instance, StatusMonitor& monitor,
                                                        RejectionStacks& stacks, RejectionCounts& counts, RejectionSlopes& slopes,
                                                        const DVector& location, const scale_estimates& scale, const DVector& flux,
                                                        int startRow, int channel )
   : IntegrationEngineBase( instance, monitor )
   , m_R( stacks ), m_N( counts ), m_M( slopes )
   , m_m( location ), m_s( scale ), m_q( flux )
   , m_x0( instance.p_useROI ? instance.p_roi.x0 : 0 )
   , m_y0( startRow + (instance.p_useROI ? instance.p_roi.y0 : 0) )
   , m_channel( channel )
{
   Array<size_type> L = Thread::OptimalThreadLoads( m_R.Length() );

   for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
   {
      int n1 = n + int( L[i] );

      if ( m_instance.p_rejection != IIRejection::NoRejection )
         normalizeThreads << new NormalizationThread( *this, n, n1 );

      if ( m_instance.p_rangeClipLow || m_instance.p_rangeClipHigh )
         rangeThreads << new RangeRejectionThread( *this, n, n1 );

      switch ( m_instance.p_rejection )
      {
      case IIRejection::MinMax:
         rejectThreads << new MinMaxRejectionThread( *this, n, n1 );
         break;
      case IIRejection::PercentileClip:
         rejectThreads << new PercentileClipRejectionThread( *this, n, n1 );
         break;
      case IIRejection::SigmaClip:
         rejectThreads << new SigmaClipRejectionThread( *this, n, n1 );
         break;
      case IIRejection::WinsorizedSigmaClip:
         rejectThreads << new WinsorizedSigmaClipRejectionThread( *this, n, n1 );
         break;
      case IIRejection::AveragedSigmaClip:
         rejectThreads << new AveragedSigmaClipRejectionThread( *this, n, n1 );
         break;
      case IIRejection::LinearFit:
         rejectThreads << new LinearFitRejectionThread( *this, n, n1 );
         break;
      case IIRejection::ESD:
         rejectThreads << new ESDRejectionThread( *this, n, n1 );
         break;
      case IIRejection::CCDClip:
         rejectThreads << new CCDClipRejectionThread( *this, n, n1 );
         break;
      default:
         break;
      }
   }
}

// ----------------------------------------------------------------------------

IntegrationRejectionEngine::~IntegrationRejectionEngine()
{
   normalizeThreads.Destroy();
   rangeThreads.Destroy();
   rejectThreads.Destroy();
   threadPrivate.Destroy();
}

// ----------------------------------------------------------------------------

void IntegrationRejectionEngine::Normalize()
{
   if ( !normalizeThreads.IsEmpty() )
   {
      m_threadData.total = m_R.Length();
      m_threadData.count = 0;
      AbstractImage::RunThreads( normalizeThreads, m_threadData );
      m_monitor = m_threadData.status;
   }
}

// ----------------------------------------------------------------------------

void IntegrationRejectionEngine::RejectRange()
{
   if ( !rangeThreads.IsEmpty() )
   {
      m_threadData.total = m_R.Length();
      m_threadData.count = 0;
      AbstractImage::RunThreads( rangeThreads, m_threadData );
      m_monitor = m_threadData.status;
   }
}

// ----------------------------------------------------------------------------

void IntegrationRejectionEngine::Reject()
{
   if ( !rejectThreads.IsEmpty() )
   {
      for ( RejectionThread& thread : rejectThreads )
         thread.PreRun();

      m_threadData.total = m_R.Length();
      m_threadData.count = 0;
      AbstractImage::RunThreads( rejectThreads, m_threadData );
      m_monitor = m_threadData.status;

      for ( RejectionThread& thread : rejectThreads )
         thread.PostRun();
   }
}

// ----------------------------------------------------------------------------

#define E   m_engine
#define I   m_engine.m_instance
#define P   static_cast<RejectionData*>( m_engine.threadPrivate.Ptr() )

// ----------------------------------------------------------------------------

void IntegrationRejectionEngine::RangeRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );
   IVector* N = E.m_N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 1 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];

         /*
          * ### N.B.: Do not change the order in the stack of pixels here (e.g.
          * do not sort them). Pixels are required in file order for rejection
          * normalization, which takes place AFTER range rejection.
          *
          * Note that the sets of low and high out-of-range pixels are
          * disjoint. For this reason we can accumulate a single rejection
          * counter in the nr variable.
          */
         int nr = 0;
         for ( int j = 0; j < n; ++j )
            if ( I.p_rangeClipLow && r[j].raw <= I.p_rangeLow )
               r[j].rejectRangeLow = true, ++nr;
            else if ( I.p_rangeClipHigh && r[j].raw >= I.p_rangeHigh )
               r[j].rejectRangeHigh = true, ++nr;
         N->DataPtr()[i] -= nr;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

void IntegrationRejectionEngine::NormalizationThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );

   /*
    * ### N.B.: All rejection normalizations except EqualizeFluxes can yield
    * negative pixel values. If this happens, we must raise all pixels by
    * adding a constant pedestal, since all rejection routines expect positive
    * data. Note that statistical scale is invariant to this transformation.
    */

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R )
   {
      switch ( I.p_rejectionNormalization )
      {
      case IIRejectionNormalization::Scale:
         {
            /*
             * Scale + zero offset normalization.
             */
            const DVector& m = E.m_m;
            const scale_estimates& s = E.m_s;
            float rmin = 0;
            for ( int j = 0; j < R->Rows(); ++j )
            {
               RejectionDataItem* r = R->RowPtr( j ) + 1;
               for ( int i = 1; i < R->Columns(); ++i, ++r )
                  if ( !r->IsRejected() )
                  {
                     r->value = (r->value - m[i])*((r->value <= m[i]) ? s[i].low : s[i].high) + m[0];
                     if ( r->value < rmin )
                        rmin = r->value;
                  }
            }

            if ( rmin < 0 )
               for ( int j = 0; j < R->Rows(); ++j )
               {
                  RejectionDataItem* r = R->RowPtr( j );
                  for ( int i = 0; i < R->Columns(); ++i, ++r )
                     if ( !r->IsRejected() )
                        r->value -= rmin;
               }
         }
         break;

      case IIRejectionNormalization::EqualizeFluxes:
         {
            /*
             * Flux equalization normalization.
             */
            const DVector& q = E.m_q;
            for ( int j = 0; j < R->Rows(); ++j )
            {
               RejectionDataItem* r = R->RowPtr( j ) + 1;
               for ( int i = 1; i < R->Columns(); ++i, ++r )
                  if ( !r->IsRejected() )
                     r->value *= q[i];
            }
         }
         break;

      case IIRejectionNormalization::LocalRejectionNormalization:
         {
            /*
             * Local normalization via XNML data.
             */
            float rmin = 0;
            for ( int j = 0, x = E.m_x0, y = E.m_y0+k; j < R->Rows(); ++j, ++x )
            {
               RejectionDataItem* r = R->RowPtr( j );
               for ( int i = 0; i < R->Columns(); ++i, ++r )
                  if ( !r->IsRejected() )
                  {
                     r->value = IntegrationFile::FileByIndex( i ).Normalize( r->value, x, y, E.m_channel );
                     if ( r->value < rmin )
                        rmin = r->value;
                  }
            }

            if ( rmin < 0 )
               for ( int j = 0; j < R->Rows(); ++j )
               {
                  RejectionDataItem* r = R->RowPtr( j );
                  for ( int i = 0; i < R->Columns(); ++i, ++r )
                     if ( !r->IsRejected() )
                        r->value -= rmin;
               }
         }
         break;

      case IIRejectionNormalization::AdaptiveRejectionNormalization:
         {
            /*
             * Adaptive scale + zero offset normalization.
             */
            const AdaptiveNormalizationData& a0 = IntegrationFile::FileByIndex( 0 ).AdaptiveNormalization();
            float rmin = 0;
            for ( int j = 0, x = E.m_x0, y = E.m_y0+k; j < R->Rows(); ++j, ++x )
            {
               double m0 = a0.Location( x, y, E.m_channel );
               if ( I.p_adaptiveNoScale )
               {
                  const scale_estimates& s = E.m_s;
                  RejectionDataItem* r = R->RowPtr( j );
                  for ( int i = 0; i < R->Columns(); ++i, ++r )
                     if ( !r->IsRejected() )
                     {
                        const AdaptiveNormalizationData& a = IntegrationFile::FileByIndex( i ).AdaptiveNormalization();
                        double m = a.Location( x, y, E.m_channel );
                        r->value = (r->value - m)*((r->value <= m) ? s[i].low : s[i].high) + m0;
                        if ( r->value < rmin )
                           rmin = r->value;
                     }
               }
               else
               {
                  double s00 = a0.ScaleLow( x, y, E.m_channel );
                  double s10 = a0.ScaleHigh( x, y, E.m_channel );
                  RejectionDataItem* r = R->RowPtr( j );
                  for ( int i = 0; i < R->Columns(); ++i, ++r )
                     if ( !r->IsRejected() )
                     {
                        const AdaptiveNormalizationData& a = IntegrationFile::FileByIndex( i ).AdaptiveNormalization();
                        double m = a.Location( x, y, E.m_channel );
                        r->value = (r->value - m)
                           * ((r->value <= m) ? s00/a.ScaleLow( x, y, E.m_channel ) : s10/a.ScaleHigh( x, y, E.m_channel ))
                           + m0;
                        if ( r->value < rmin )
                           rmin = r->value;
                     }
               }
            }

            if ( rmin < 0 )
               for ( int j = 0; j < R->Rows(); ++j )
               {
                  RejectionDataItem* r = R->RowPtr( j );
                  for ( int i = 0; i < R->Columns(); ++i, ++r )
                     if ( !r->IsRejected() )
                        r->value -= rmin;
               }
         }
         break;

      default: // ?!
         break;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

void IntegrationRejectionEngine::MinMaxRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );
   IVector* N = E.m_N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 1 )
            continue;

         int nl = I.p_clipLow ? Min( I.p_minMaxLow, n ) : 0;
         int nh = I.p_clipHigh ? Min( I.p_minMaxHigh, n ) : 0;

         if ( nl > 0 || nh > 0 )
         {
            RejectionDataItem* r = R->DataPtr()[i];
            Sort( r, r + n );

            for ( int j = 0; j < nl; ++j )
               r[j].rejectLow = true;
            for ( int j = n, nj = nh; nj > 0; --nj )
               r[--j].rejectHigh = true;

            Sort( r, r + n );
            N->DataPtr()[i] -= Min( n, nl+nh );
         }
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

void IntegrationRejectionEngine::PercentileClipRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );
   IVector* N = E.m_N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 2 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         double median = E.RejectionMedian( r, n );
         if ( 1 + median == 1 )
            continue;

         int nc = 0;

         if ( I.p_clipLow )
            for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
            {
               if ( (median - d->value)/median <= I.p_pcClipLow )
                  break;
               d->rejectLow = true, ++nc;
            }

         if ( I.p_clipHigh )
            for ( RejectionDataItem* d = r + n; --d >= r; )
            {
               if ( (d->value - median)/median <= I.p_pcClipHigh )
                  break;
               d->rejectHigh = true, ++nc;
            }

         if ( nc > 0 )
         {
            Sort( r, r + n );
            N->DataPtr()[i] -= nc;
         }
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

void IntegrationRejectionEngine::SigmaClipRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );
   IVector* N = E.m_N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 3 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         for ( ;; )
         {
            double sigma = E.RejectionSigma( r, n );
            if ( 1 + sigma == 1 )
               break;

            double median = E.RejectionMedian( r, n );

            int nc = 0;

            if ( I.p_clipLow )
               for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
               {
                  if ( (median - d->value)/sigma <= I.p_sigmaLow )
                     break;
                  d->rejectLow = true, ++nc;
               }

            if ( I.p_clipHigh )
               for ( RejectionDataItem* d = r + n; --d >= r; )
               {
                  if ( (d->value - median)/sigma <= I.p_sigmaHigh )
                     break;
                  d->rejectHigh = true, ++nc;
               }

            if ( nc == 0 )
               break;

            Sort( r, r + n );

            n -= nc;
            if ( n < 3 )
               break;
         }

         N->DataPtr()[i] = n;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

void IntegrationRejectionEngine::WinsorizedSigmaClipRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );
   IVector* N = E.m_N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 3 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         for ( int it = 0; ; ++it )
         {
            double mean, sigma;
            RejectionWinsorization( mean, sigma, r, n, (it > 0) ? .0F : I.p_winsorizationCutoff );
            if ( 1 + sigma == 1 )
               break;

            // N.B. Winsorization cut off may alter sample order.
            if ( it == 0 )
               if ( I.p_winsorizationCutoff > 1.5 )
                  Sort( r, r + n );

            int nc = 0;

            if ( I.p_clipLow )
               for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
               {
                  if ( (mean - d->value)/sigma <= I.p_sigmaLow )
                     break;
                  d->rejectLow = true, ++nc;
               }

            if ( I.p_clipHigh )
               for ( RejectionDataItem* d = r + n; --d >= r; )
               {
                  if ( (d->value - mean)/sigma <= I.p_sigmaHigh )
                     break;
                  d->rejectHigh = true, ++nc;
               }

            if ( nc == 0 )
               break;

            Sort( r, r + n );

            n -= nc;
            if ( n < 3 )
               break;
         }

         N->DataPtr()[i] = n;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

IntegrationRejectionEngine::AveragedSigmaClipRejectionThread::RejectionData::RejectionData( RejectionStacks& R_, RejectionCounts& N_ )
   : IntegrationRejectionEngine::RejectionThreadPrivate()
   , m( R_.Length() )
   , s( R_.Length() )
{
   RejectionMatrix* R = R_.DataPtr();
   IVector* N = N_.DataPtr();

   for ( int k = 0; k < R_.Length(); ++k, ++R, ++N )
   {
      m[k] = DVector( R->Rows() );
      s[k] = DVector( R->Rows() );

      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 3 )
         {
            m[k][i] = s[k][i] = .0F;
            continue;
         }

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         double median = m[k][i] = IntegrationRejectionEngine::RejectionMedian( r, n );
         if ( 1 + median != 1 )
         {
            double acc = 0;
            for ( int j = 0; j < n; ++j )
            {
               double d = r[j].value - median;
               acc += d*d/median;
            }

            s[k][i] = Sqrt( acc/(n - 1) );
         }
         else
         {
            s[k][i] = m[k][i] = 0;
            N->DataPtr()[i] = 0;
         }
      }
   }
}

void IntegrationRejectionEngine::AveragedSigmaClipRejectionThread::PreRun()
{
   if ( E.threadPrivate.IsNull() )
      E.threadPrivate = new RejectionData( E.m_R, E.m_N );
}

void IntegrationRejectionEngine::AveragedSigmaClipRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );
   IVector* N = E.m_N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      const DVector& m = P->m[k];
      const DVector& s = P->s[k];

      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 3 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];

         double median = m[i];

         for ( ;; )
         {
            double sigma = s[i] * Sqrt( median );
            if ( 1 + sigma == 1 )
               break;

            int nc = 0;

            if ( I.p_clipLow )
               for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
               {
                  if ( (median - d->value)/sigma <= I.p_sigmaLow )
                     break;
                  d->rejectLow = true, ++nc;
               }

            if ( I.p_clipHigh )
               for ( RejectionDataItem* d = r + n; --d >= r; )
               {
                  if ( (d->value - median)/sigma <= I.p_sigmaHigh )
                     break;
                  d->rejectHigh = true, ++nc;
               }

            if ( nc == 0 )
               break;

            Sort( r, r + n );

            n -= nc;
            if ( n < 3 )
               break;

            median = E.RejectionMedian( r, n );
         }

         N->DataPtr()[i] = n;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

void IntegrationRejectionEngine::AveragedSigmaClipRejectionThread::PostRun()
{
   E.threadPrivate.Destroy();
}

// ----------------------------------------------------------------------------

void IntegrationRejectionEngine::LinearFitRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );
   IVector* N = E.m_N.ComponentPtr( m_firstStack );
   FVector* M = E.m_M.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N, ++M )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 5 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         for ( ;; )
         {
            FVector X( n ), Y( n );
            for ( int j = 0; j < n; ++j )
               X[j] = float( j ), Y[j] = r[j].value;

            LinearFit L( X, Y );
            if ( !L.IsValid() )
            {
               for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
                  d->rejectLow = d->rejectHigh = true;
               N->DataPtr()[i] = 0;
               M->DataPtr()[i] = 0;
               break;
            }

            double s = 2 * L.adev * Sqrt( 1 + L.b*L.b ); // 2 * to compatibilize with sigma clipping
            if ( 1 + s == 1 )
               break;

            int nc = 0;
            for ( int j = 0; j < n; ++j )
            {
               double y = L( j );

               if ( r[j].value < y )
               {
                  if ( I.p_clipLow )
                     if ( (y - r[j].value)/s >= I.p_linearFitLow )
                        r[j].rejectLow = true, ++nc;
               }
               else
               {
                  if ( I.p_clipHigh )
                     if ( (r[j].value - y)/s >= I.p_linearFitHigh )
                        r[j].rejectHigh = true, ++nc;
               }
            }

            if ( nc == 0 )
            {
               /*
                * N.B.: The fitted slope could be an insignificant negative
                * number because of roundoff - do not propagate it. See bug
                * report: http://pixinsight.com/forum/index.php?topic=8704.0
                */
               M->DataPtr()[i] = (L.b > 0) ? ((L.b < 1e7) ? ArcTan( L.b )/Const<double>::pi4() : 1.0) : 0.0;
               break;
            }

            Sort( r, r + n );

            n -= nc;
            if ( n < 3 )
               break;
         }

         N->DataPtr()[i] = n;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

/*
 * ESD critical value cache.
 */
struct ESDLambdaCache
{
public:

   ESDLambdaCache( double alpha )
      : m_alpha( alpha )
   {
   }

   PCL_HOT_FUNCTION float operator()( int n, int c )
   {
      uint32 key = Key( n, c );
      if ( m_cache.Length() > 8 )
      {
         cache_impl::const_iterator i = BinarySearch( m_cache.Begin(), m_cache.End(), key );
         if ( i != m_cache.End() )
            return i->lambda;
      }
      else
      {
         for ( const Item& item : m_cache )
            if ( item.key == key )
               return item.lambda;
      }
      float l = float( IntegrationRejectionEngine::ESDLambda( n, c, m_alpha ) );
      m_cache << Item{ key, l };
      return l;
   }

private:

   struct Item
   {
      uint32 key;
      float  lambda;

      friend bool operator <( const Item& i, const Item& j )
      {
         return i.key < j.key;
      }

      friend bool operator <( const Item& i, uint32 k )
      {
         return i.key < k;
      }

      friend bool operator <( uint32 k, const Item& i )
      {
         return k < i.key;
      }
   };

   static uint32 Key( int n, int c )
   {
      return (uint32( n ) << 16) | uint32( c );
   }

   typedef SortedArray<Item>  cache_impl;

   double     m_alpha; // significance level
   cache_impl m_cache;
};

/*
 * ESD test statistic data.
 */
struct ESDT
{
   double x;
   int    i;

   operator double() const
   {
      return x;
   }
};

void IntegrationRejectionEngine::ESDRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   ESDLambdaCache lambdaCache( I.p_esdAlpha );

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );
   IVector* N = E.m_N.ComponentPtr( m_firstStack );

   for ( int l = m_firstStack; l < m_endStack; ++l, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 3 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         Array<ESDT> X = Array<ESDT>( size_type( n ) );
         for ( int i = 0; i < n; ++i )
         {
            X[i].x = r[i].value;
            X[i].i = i;
         }

         int k = Range( TruncInt( I.p_esdOutliersFraction * n ), 1, n-2 );

         Array<ESDT> T = Array<ESDT>( size_type( k ) );
         for ( int i = 0; ; )
         {
            int n = int( X.Length() );
//             double m = Mean( X.Begin(), X.End() );
            int th = Max( 1, TruncInt( I.p_esdOutliersFraction * n ) - i );
            int tl = Max( 1, TruncInt( I.p_esdOutliersFraction/I.p_esdLowRelaxation * n ) - i );
            double m = (th+tl < n-2) ? TrimmedMeanDestructive( X.Begin(), X.End(), tl, th ) :
                                       MedianDestructive( X.Begin(), X.End() );
            double sh = StdDev( X.Begin(), X.End(), m );
            double sl = I.p_esdLowRelaxation * sh;
            Vector r( n );
            for ( int i = 0; i < n; ++i )
            {
//                r[i] = Abs( X[i].x - m )/s;
               double d = X[i].x - m;
               r[i] = (d >= 0) ? d/sh : -d/sl;
            }

            int imax = 0;
            for ( int i = 1; i < n; ++i )
               if ( r[i] > r[imax] )
                  imax = i;
            T[i].x = r[imax];
            T[i].i = X[imax].i;

            if ( ++i == k )
               break;

            Array<ESDT> Y = Array<ESDT>( size_type( n-1 ) );
            for ( int i = 0, j = 0; i < n; ++i )
               if ( i != imax )
                  Y[j++] = X[i];
            X = Y;
         }

         int nc;
         for ( nc = 0; nc < k; ++nc )
            if ( T[nc].x < lambdaCache( n, nc ) /*ESDLambda( n, nc, I.p_esdAlpha )*/ )
               break;

         if ( nc > 0 )
         {
            double median = E.RejectionMedian( r, n );
            for ( int i = 0; i < nc; ++i )
            {
               int j = T[i].i;
               if ( r[j].value > median )
               {
                  if ( I.p_clipHigh )
                     r[j].rejectHigh = true;
               }
               else
               {
                  if ( I.p_clipLow )
                     r[j].rejectLow = true;
               }
            }

            Sort( r, r + n );
            N->DataPtr()[i] -= nc;
         }
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

IntegrationRejectionEngine::CCDClipRejectionThread::RejectionData::RejectionData( float ccdGain, float ccdReadNoise, float ccdScaleNoise, int bits )
{
   /*
   r2g2 = double( ccdReadNoise )/ccdGain; r2g2 *= r2g2/n;
   gk = n/double( ccdGain )*n;
   sn2 = double( ccdScaleNoise )*ccdScaleNoise*n;
   */
   double n = Pow2I<double>()( bits ) - 1;
   r2g2 = double( ccdReadNoise )/ccdGain; r2g2 *= r2g2/n;
   gk = 1.0/ccdGain/n;
   sn2 = double( ccdScaleNoise )*ccdScaleNoise/n;
   isScaleNoise = sn2 != 0;
}

void IntegrationRejectionEngine::CCDClipRejectionThread::PreRun()
{
   if ( E.threadPrivate.IsNull() )
      E.threadPrivate = new RejectionData( I.p_ccdGain, I.p_ccdReadNoise, I.p_ccdScaleNoise, 16 );
}

void IntegrationRejectionEngine::CCDClipRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );
   IVector* N = E.m_N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 2 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         for ( ;; )
         {
            double median = E.RejectionMedian( r, n );
            double sigma = P->r2g2 + P->gk*median;
            if ( P->isScaleNoise )
               sigma += P->sn2*median*median;
            sigma = Sqrt( sigma );
            /*
            double DN = double( median )*65535;
            double sigma = Sqrt( ((ccdReadNoise*ccdReadNoise)/(ccdGain*ccdGain)
                                   + DN/ccdGain
                                   + ccdScaleNoise*ccdScaleNoise*DN*DN)/65535 );
            */

            int nc = 0;

            if ( I.p_clipLow )
               for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
               {
                  if ( (median - d->value)/sigma <= I.p_sigmaLow )
                     break;
                  d->rejectLow = true, ++nc;
               }

            if ( I.p_clipHigh )
               for ( RejectionDataItem* d = r + n; --d >= r; )
               {
                  if ( (d->value - median)/sigma <= I.p_sigmaHigh )
                     break;
                  d->rejectHigh = true, ++nc;
               }

            if ( nc == 0 )
               break;

            Sort( r, r + n );

            n -= nc;
            if ( n < 2 )
               break;
         }

         N->DataPtr()[i] = n;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

void IntegrationRejectionEngine::CCDClipRejectionThread::PostRun()
{
   E.threadPrivate.Destroy();
}

// ----------------------------------------------------------------------------

#undef E
#undef I
#undef P

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegrationRejectionEngine.cpp - Released 2021-04-09T19:41:48Z
