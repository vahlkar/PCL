//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// IntegrationRejectionEngine.h - Released 2021-04-09T19:41:48Z
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

#ifndef __IntegrationRejectionEngine_h
#define __IntegrationRejectionEngine_h

#include <pcl/AutoPointer.h>

#include "ImageIntegrationInstance.h"
#include "IntegrationEngineBase.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class IntegrationRejectionEngine : public IntegrationEngineBase
{
public:

   typedef ImageIntegrationInstance::RejectionDataItem   RejectionDataItem;
   typedef ImageIntegrationInstance::RejectionMatrix     RejectionMatrix;
   typedef ImageIntegrationInstance::RejectionStacks     RejectionStacks;
   typedef ImageIntegrationInstance::RejectionCounts     RejectionCounts;
   typedef ImageIntegrationInstance::RejectionSlopes     RejectionSlopes;
   typedef ImageIntegrationInstance::scale_estimates     scale_estimates;

   IntegrationRejectionEngine( const ImageIntegrationInstance&, StatusMonitor&,
                               RejectionStacks&, RejectionCounts&, RejectionSlopes&,
                               const DVector& location, const scale_estimates& scale, const DVector& flux,
                               int startRow, int channel );

   virtual ~IntegrationRejectionEngine();

   void Normalize();
   void RejectRange();
   void Reject();

   /*
    * Arithmetic mean of a pixel sample.
    *
    * r  The sample of rejection items.
    * n  Number of unrejected items (sample length).
    */
   static double RejectionMean( const RejectionDataItem* r, int n )
   {
      if ( n < 2 )
         return 0;
      double s = 0;
      for ( int i = 0; i < n; ++i )
         s += r[i].value;
      return s/n;
   }

   /*
    * Standard deviation of a pixel sample.
    *
    * r  The sample of rejection items.
    * n  Number of unrejected items (sample length).
    */
   static double RejectionSigma( const RejectionDataItem* r, int n )
   {
      if ( n < 2 )
         return 0;
      double mean = 0;
      for ( int i = 0; i < n; ++i )
         mean += r[i].value;
      mean /= n;
      double var = 0, eps = 0;
      for ( int i = 0; i < n; ++i )
      {
         double d = r[i].value - mean;
         var += d*d;
         eps += d;
      }
      return Sqrt( (var - (eps*eps)/n)/(n - 1) );
   }

   /*
    * Median of a pixel sample.
    *
    * r  The sample of rejection items.
    * n  Number of unrejected items (sample length).
    */
   static double RejectionMedian( const RejectionDataItem* r, int n )
   {
      // Assume that {r0...rn} is already sorted by value.
      if ( n < 2 )
         return 0;
      int n2 = n >> 1;
      return (n & 1) ? r[n2].value : (r[n2].value + r[n2-1].value)/2;
   }

   /*
    * Mean absolute deviation from the median of a pixel sample.
    *
    * r        The sample of rejection items.
    * n        Number of unrejected items (sample length).
    * median   The median of the (unrejected) sample.
    */
   static double RejectionADev( const RejectionDataItem* r, int n, double median )
   {
      if ( n < 2 )
         return 0;
      double sd = 0;
      for ( int i = 0; i < n; ++i )
         sd += Abs( r[i].value - median );
      return sd/n;
   }

   /*
    * Median absolute deviation from the median of a pixel sample.
    *
    * r        The sample of rejection items.
    * n        Number of unrejected items (sample length).
    * median   The median of the (unrejected) sample.
    */
   static double RejectionMAD( const RejectionDataItem* r, int n, double median )
   {
      if ( n < 2 )
         return 0;
      DVector d( n );
      for ( int i = 0; i < n; ++i )
         d[i] = Abs( r[i].value - median );
      return pcl::Median( d.Begin(), d.End() );
   }

   /*
    * Sn scale estimator of Rousseeuw and Croux for a pixel sample.
    *
    * r  The sample of rejection items.
    * n  Number of unrejected items (sample length).
    */
   static double RejectionSn( const RejectionDataItem* r, int n )
   {
      DVector d( n );
      for ( int i = 0; i < n; ++i )
         d[i] = r[i].value;
      return pcl::Sn( d.Begin(), d.End() );
   }

   /*
    * Winsorization of a pixel sample.
    *
    * mean     On output, the Winsorized mean of the sample.
    *
    * sigma    On output, the Winsorized standard deviation of the sample.
    *
    * r        The sample of rejection items.
    *
    * n        Number of unrejected items (sample length).
    *
    * cutoffPoint Winsorization cutoff point in sigma units. If
    *          cutoffPoint > 0, all sample values at distances larger than
    *          cutoffPoint will be set equal to the sample median.
    */
   static void RejectionWinsorization( double& mean, double& sigma, const RejectionDataItem* r, int n, float cutoffPoint )
   {
      if ( n < 2 )
      {
         mean = sigma = 0;
         return;
      }

      mean = RejectionMedian( r, n );
      sigma = 1.1926*RejectionSn( r, n );

      DVector v( n );
      for ( int i = 0; i < n; ++i )
         v[i] = r[i].value;

      for ( int it = 0;; )
      {
         if ( 1 + sigma == 1 )
            break;

         double t0 = mean - 1.5*sigma;
         double t1 = mean + 1.5*sigma;

         if ( cutoffPoint > 0 )
         {
            double c0 = mean - cutoffPoint*sigma;
            double c1 = mean + cutoffPoint*sigma;
            for ( int i = 0; i < n; ++i )
               if ( v[i] < t0 )
                  v[i] = (v[i] > c0) ? t0 : mean;
               else if ( v[i] > t1 )
                  v[i] = (v[i] < c1) ? t1 : mean;
         }
         else
         {
            for ( int i = 0; i < n; ++i )
               if ( v[i] < t0 )
                  v[i] = t0;
               else if ( v[i] > t1 )
                  v[i] = t1;
         }

         double s0 = sigma;
         sigma = 1.134*v.StdDev();
         mean = v.Mean();
         if ( ++it > 1 )
            if ( Abs( s0 - sigma )/s0 < 0.0005 )
               break;
      }
   }

   /*
    * The cumulative distribution function (CDF) of Student's t distribution.
    *
    * nu    Degrees of freedom of the distribution.
    * t     Evaluation point.
    */
   static double t_CDF( double nu, double t )
   {
      double p = IncompleteBeta( nu/2, 0.5, nu/(nu + t*t) )/2;
      return (t < 0) ? p : 1 - p;
   }

   /*
    * The upper percentile of Student's t distribution.
    *
    * nu    Degrees of freedom of the distribution.
    * x     Percentile in the range [0,0.5].
    * eps   Relative accuracy of the returned result.
    *
    * We implement an adaptive binary search algorithm based on the cumulative
    * distribution function.
    *
    * Returns t(nu,x) such that P(t(nu) > t(nu,x)) = x.
    */
   static double t_p( double nu, double x, double eps = 1.0e-9 )
   {
      x = Abs( x );
      if ( x >= 0.5 )
         return 0;
      for ( double l = 0, h = 50; ; )
      {
         double p = (l + h)/2;
         double c = 1 - t_CDF( nu, p );
         if ( Abs( c - x ) < eps )
            return p;
         if ( c < x )
         {
            h = p;
            // Check if we are approaching zero asymptotically.
            if ( h - l < 2*std::numeric_limits<double>::epsilon() )
               return 0;
         }
         else
         {
            l = p;
            // Check if we are exhausting the search space; double it at the
            // upper side if necessary.
            if ( h - l < 1.0e-8 )
               h *= 2;
         }
      }
   }

   /*
    * Generalized ESD Test for Outliers - Critical values for two-tailed
    * outlier detection.
    *
    * n        Sample length.
    *
    * i        Test index in the range [0,k-1], where k is the prescribed
    *          maximum number of outliers.
    *
    * alpha    The Type I error, or significance level.
   */
   static double ESDLambda( int n, int i, double alpha )
   {
      double p = alpha/2/(n - i);
      double t = t_p( n - i - 2, p );
      return t*(n - i - 1)/Sqrt( (n - i - 2 + t*t)*(n - i) );
   }

private:

   class RejectionThreadPrivate
   {
   public:

      RejectionThreadPrivate() = default;

      virtual ~RejectionThreadPrivate()
      {
      }
   };

   class RejectionThread : public EngineThreadBase
   {
   public:

      RejectionThread( IntegrationRejectionEngine& engine, int firstStack, int endStack )
         : EngineThreadBase( engine, firstStack, endStack )
         , m_engine( engine )
      {
      }

      virtual ~RejectionThread()
      {
      }

      virtual void PreRun()
      {
      }

      virtual void Run() = 0;

      virtual void PostRun()
      {
      }

   protected:

      IntegrationRejectionEngine& m_engine;
   };

   typedef ReferenceArray<RejectionThread> thread_list;

   class NormalizationThread : public RejectionThread
   {
   public:

      NormalizationThread( IntegrationRejectionEngine& engine, int firstStack, int endStack )
         : RejectionThread( engine, firstStack, endStack )
      {
      }

      void Run() override;
   };

   class RangeRejectionThread : public RejectionThread
   {
   public:

      RangeRejectionThread( IntegrationRejectionEngine& engine, int firstStack, int endStack )
         : RejectionThread( engine, firstStack, endStack )
      {
      }

      void Run() override;
   };

   class MinMaxRejectionThread : public RejectionThread
   {
   public:

      MinMaxRejectionThread( IntegrationRejectionEngine& engine, int firstStack, int endStack )
         : RejectionThread( engine, firstStack, endStack )
      {
      }

      void Run() override;
   };

   class PercentileClipRejectionThread : public RejectionThread
   {
   public:

      PercentileClipRejectionThread( IntegrationRejectionEngine& engine, int firstStack, int endStack )
         : RejectionThread( engine, firstStack, endStack )
      {
      }

      void Run() override;
   };

   class SigmaClipRejectionThread : public RejectionThread
   {
   public:

      SigmaClipRejectionThread( IntegrationRejectionEngine& engine, int firstStack, int endStack )
         : RejectionThread( engine, firstStack, endStack )
      {
      }

      void Run() override;
   };

   class WinsorizedSigmaClipRejectionThread : public RejectionThread
   {
   public:

      WinsorizedSigmaClipRejectionThread( IntegrationRejectionEngine& engine, int firstStack, int endStack )
         : RejectionThread( engine, firstStack, endStack )
      {
      }

      void Run() override;
   };

   class AveragedSigmaClipRejectionThread : public RejectionThread
   {
   public:

      AveragedSigmaClipRejectionThread( IntegrationRejectionEngine& engine, int firstStack, int endStack )
         : RejectionThread( engine, firstStack, endStack )
      {
      }

      void PreRun() override;
      void Run() override;
      void PostRun() override;

   private:

      class RejectionData : public RejectionThreadPrivate
      {
      public:

         Array<DVector> m;
         Array<DVector> s;

         RejectionData( RejectionStacks&, RejectionCounts& );
      };
   };

   class LinearFitRejectionThread : public RejectionThread
   {
   public:

      LinearFitRejectionThread( IntegrationRejectionEngine& engine, int firstStack, int endStack )
         : RejectionThread( engine, firstStack, endStack )
      {
      }

      void Run() override;
   };

   class ESDRejectionThread : public RejectionThread
   {
   public:

      ESDRejectionThread( IntegrationRejectionEngine& engine, int firstStack, int endStack )
         : RejectionThread( engine, firstStack, endStack )
      {
      }

      void Run() override;
   };

   class CCDClipRejectionThread : public RejectionThread
   {
   public:

      CCDClipRejectionThread( IntegrationRejectionEngine& engine, int firstStack, int endStack )
         : RejectionThread( engine, firstStack, endStack )
      {
      }

      void PreRun() override;
      void Run() override;
      void PostRun() override;

   private:

      class RejectionData : public RejectionThreadPrivate
      {
      public:

         double r2g2, gk, sn2;
         bool isScaleNoise;

         RejectionData( float ccdGain, float ccdReadNoise, float ccdScaleNoise, int bits );
      };
   };

         RejectionStacks& m_R;       // set of pixel stacks
         RejectionCounts& m_N;       // set of counts
         RejectionSlopes& m_M;       // set of slopes, for linear fit clipping only
   const DVector&         m_m;       // rejection normalization: location
   const scale_estimates& m_s;       //                        : scale
   const DVector&         m_q;       //                        : flux
         int              m_x0;      // local normalization: starting horizontal coordinate (ROI)
         int              m_y0;      //                    : starting vertical coordinate
         int              m_channel; //                    : current component

         thread_list      rangeThreads, normalizeThreads, rejectThreads;
         AutoPointer<RejectionThreadPrivate> threadPrivate;

   friend class NormalizationThread;
   friend class RangeRejectionThread;
   friend class MinMaxRejectionThread;
   friend class PercentileClipRejectionThread;
   friend class SigmaClipRejectionThread;
   friend class WinsorizedSigmaClipRejectionThread;
   friend class AveragedSigmaClipRejectionThread;
   friend class LinearFitRejectionThread;
   friend class ESDRejectionThread;
   friend class CCDClipRejectionThread;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __IntegrationRejectionEngine_h

// ----------------------------------------------------------------------------
// EOF IntegrationRejectionEngine.h - Released 2021-04-09T19:41:48Z
