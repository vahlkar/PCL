//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// XEPHGenerator.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
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

#include "XEPHGenerator.h"

#include <pcl/AutoLock.h>
#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void XEPHGenerator::AddObject( const IntegrationDenseOutputData& data
                             , const TimePoint& startTime, const TimePoint& endTime
                             , double epsFactor
                             , const IsoString& id, const String& name
                             , Optional<double> H, Optional<double> G, Optional<double> B_V, Optional<double> D )
{
   int startJDI = startTime.JDI();
   int endJDI = endTime.JDI();
   if ( endTime.JDF() < 0.5 )
      --endJDI;

   IsoString autoId = id;
   {
      volatile AutoLock lock( m_mutex );
      if ( m_objects.IsEmpty() )
      {
         m_startJDI = startJDI;
         m_endJDI = endJDI;
      }
      else
      {
         if ( startJDI != m_startJDI || endJDI != m_endJDI )
            throw Error( "XEPHGenerator: Internal error: inconsistent ephemeris time spans." );
      }

      if ( data[0].t > m_startJDI + 0.5 || data[data.Length()-1].t + data[data.Length()-1].h < m_endJDI + 0.5 )
         throw Error( "XEPHGenerator: Internal error: invalid dense output time spans." );

      /*
       * Unique object identifiers are mandatory in XEPH files, so we have to
       * generate one (a dumb one, really) automatically if none is available.
       */
      ++m_autoId;
      if ( autoId.IsEmpty() )
         autoId.Format( "X%04d", m_autoId );
   }

   SerializableEphemerisObjectData objectData = MakeObjectData( data, epsFactor, autoId, name, H, G, B_V, D );

   {
      volatile AutoLock lock( m_mutex );
      m_objects << objectData;
   }
}

// ----------------------------------------------------------------------------

void XEPHGenerator::Serialize( const String& filePath, const EphemerisMetadata& metadata )
{
   EphemerisFile::Serialize( filePath,
                             TimePoint( m_startJDI, 0.5 ), TimePoint( m_endJDI, 0.5 ),
                             m_objects, metadata );
}

// ----------------------------------------------------------------------------

static int InitialExpansionSpan( double d )
{
   if ( d < 0.5 )
      return 30;
   if ( d < 0.7 )
      return 140;
   if ( d < 1.2 )
      return 100;
   if ( d < 1.5 )
      return 250;
   if ( d < 3 )
      return 400;
   if ( d < 5 )
      return 700;
   if ( d < 8 )
      return 1000;
   if ( d < 15 )
      return 3400;
   if ( d < 30 )
      return 4500;
   return 5400;
}

class IntegrationDenseOutputEvaluation
{
public:

   IntegrationDenseOutputEvaluation( const IntegrationDenseOutputData& data, int order )
      : m_data( data )
      , m_order( order )
   {
   }

   Vector operator()( TimePoint t ) const
   {
      Update( t.JD() );
      return m_itemExpansion( t - m_itemStartTime );
   }

   double Tolerance( TimePoint t ) const
   {
      Update( t.JD() );
      return m_data[m_itemIndex].eps;
   }

private:

     const IntegrationDenseOutputData& m_data;
           int                         m_order;
   mutable int                         m_itemIndex = -1;
   mutable TimePoint                   m_itemStartTime;
   mutable ChebyshevFit                m_itemExpansion;

   void Update( double t ) const
   {
      for ( int N = int( m_data.Length() ), l = 0, r = N-1; ; )
      {
         int m = (l + r) >> 1;
         const IntegrationDenseOutputItem& item = m_data[m];
         if ( t < item.t )
            r = m;
         else
         {
            if ( m == N-1 || t <= item.t + item.h )
            {
               if ( m != m_itemIndex )
               {
                  m_itemIndex = m;
                  m_itemStartTime = item.t;
                  m_itemExpansion = ChebyshevFit(
                     ChebyshevFit::coefficient_series( m_order ? item.c1[0] : item.c0[0]
                                                     , m_order ? item.c1[1] : item.c0[1]
                                                     , m_order ? item.c1[2] : item.c0[2] ), 0, item.h );
               }
               break;
            }

            l = m + 1;
         }
      }
   }
};

SerializableEphemerisObjectData XEPHGenerator::MakeObjectData( const IntegrationDenseOutputData& data
                                    , double epsFactor
                                    , const IsoString& id, const String& name
                                    , Optional<double> H, Optional<double> G, Optional<double> B_V, Optional<double> D ) const
{
   if ( data.IsEmpty() )
      throw Error( "XEPHGenerator: Internal error: Empty integration data." );

   Console console;
   console.WriteLn( "<end><cbr><br>* Generating ephemeris data for " + id + ' ' + name + "<br>" );
   bool rootThread = Thread::IsRootThread();

   // Maximum allowed expansion length
   const int maxLength = 250;

   SerializableEphemerisObjectData object( id, "SSB", name );
   object.H = H;
   object.G = G;
   object.B_V = B_V;
   object.D = D;

   bool haveVelocity = !data[0].c1[0].IsEmpty();

   for ( int order = 0, startDelta = 0; order <= 1; ++order )
   {
      IntegrationDenseOutputEvaluation eval( data, order );
      if ( order == 0 )
         startDelta = InitialExpansionSpan( eval( m_startJDI + 0.5 ).L2Norm() );

      int delta = startDelta;
      int minDelta = int32_max;
      int maxDelta = 0;
      double maxError[ 3 ] = {};
      int totalCoefficients = 0;

      for ( int jdi1 = m_startJDI, count = 0; jdi1 < m_endJDI; ++count )
      {
         int jdi2;
         int length = 25;
         double epsilon = eval.Tolerance( jdi1 + 0.5 ) * epsFactor;
         ChebyshevFit T;

         for ( bool reduce = false, truncated = false;; )
         {
            Module->ProcessEvents();
            if ( rootThread )
               if ( console.AbortRequested() )
                  throw ProcessAborted();

            jdi2 = jdi1 + delta;
            if ( jdi2 > m_endJDI )
            {
               jdi2 = m_endJDI;
               delta = jdi2 - jdi1;
            }

            T = ChebyshevFit( [=]( double dt )
                              {
                                 return eval( jdi1 + 0.5 + dt );
                              },
                              0, delta, 3, 2*length );
            T.Truncate( epsilon );
            if ( T.TruncatedLength() > length )
            {
               --delta;
               reduce = true;
            }
            else if ( !reduce && jdi2 < m_endJDI )
               ++delta;
            else
            {
               if ( truncated )
                  console.WarningLn( String().Format( "** Warning: Increased truncation error to %.3e", epsilon ) );
               break;
            }

            if ( delta == 0 )
            {
               /*
                * Fast movement: try with longer coefficient series.
                * This may happen during close encounters.
                */
               length += 10;
               if ( length > maxLength )
               {
                  /*
                   * Extremely fast movement: try a larger truncation error.
                   * This should only happen during near-collision encounters.
                   */
                  length = 50;
                  epsilon *= 1.1;
                  truncated = true;
               }

               delta = 12;
               reduce = false;
            }
         }

         if ( T.TruncationError() > epsilon )
            throw Error( String().Format( "XEPHGenerator: Internal error: %d -> %d (%d) : nx=%d ny=%d nz=%d e=%.3e",
                           jdi1, jdi2, delta,
                           T.TruncatedLength( 0 ), T.TruncatedLength( 1 ), T.TruncatedLength( 2 ), T.TruncationError() ) );

         object.data[order] << SerializableEphemerisData( TimePoint( jdi1, 0.5 ), T );

         console.WriteLn( String().Format( "%5d : %+10.1f -> %+10.1f (%5d) : %3d %3d %3d %.3e %.3e %.3e"
                                 , count, jdi1+0.5, jdi2+0.5, delta
                                 , T.TruncatedLength( 0 ), T.TruncatedLength( 1 ), T.TruncatedLength( 2 )
                                 , T.TruncationError( 0 ), T.TruncationError( 1 ), T.TruncationError( 2 ) ) );
         Module->ProcessEvents();

         for ( int i = 0; i < 3; ++i )
            if ( T.TruncationError( i ) > maxError[i] )
               maxError[i] = T.TruncationError( i );

         totalCoefficients += T.NumberOfTruncatedCoefficients();

         if ( delta < minDelta )
            if ( jdi2 < m_endJDI || count == 0 )
               minDelta = delta;
         if ( delta > maxDelta )
            maxDelta = delta;

         jdi1 = jdi2;
      }

      console.WriteLn( "\nObject ....................... " + id + ' ' + name
                     + "\nDerivative order ............. " + String( order )
                     + "\nTotal time span .............. " + String().Format( "%+10.1f -> %+10.1f (%d days)"
                                                               , m_startJDI+0.5, m_endJDI+0.5, Abs( m_endJDI - m_startJDI ) )
                     + "\nTotal Chebyshev expansions ... " + String( object.data[order].Length() )
                     + "\nSmallest time span ........... " + String( minDelta ) + " (days)"
                     + "\nLargest time span ............ " + String( maxDelta ) + " (days)"
                     + "\nLargest truncation errors .... " + String().Format( "%.3e  %.3e  %.3e  (%s)"
                                                               , maxError[0], maxError[1], maxError[2], order ? "au/day" : "au" )
                     + "\nTotal coefficients ........... " + String( totalCoefficients ) + '\n' );
      Module->ProcessEvents();

      if ( !haveVelocity )
         break;
   }

   return object;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF XEPHGenerator.cpp - Released 2021-04-09T19:41:48Z
