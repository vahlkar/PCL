//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// EphemerisGeneratorInstance.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
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

#include "Ephemerides.h"
#include "EphemerisGeneratorInstance.h"
#include "EphemerisGeneratorParameters.h"
#include "EphemerisGeneratorProcess.h"
#include "Integration.h"

#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/Position.h>
#include <pcl/Thread.h>
#include <pcl/StandardStatus.h>
#include <pcl/Version.h>

// #define HSCALAR_TYPE float128
// #define HSCALAR_TYPE cpp_bin_float_quad

namespace pcl
{

// ----------------------------------------------------------------------------

EphemerisGeneratorInstance::EphemerisGeneratorInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_workingMode( EGWorkingMode::Default )
   , p_position( 0, 0, 0 )
   , p_velocity( 0, 0, 0 )
   , p_elements( {0, 0, 0, 0, 0, 0, 0, 0} )
   , p_epochJD( TheEGEpochJDParameter->DefaultValue() )
   , p_objectId( TheEGObjectIdParameter->DefaultValue() )
   , p_objectName( TheEGObjectNameParameter->DefaultValue() )
   , p_H_defined( TheEGHDefinedParameter->DefaultValue() )
   , p_H( TheEGHParameter->DefaultValue() )
   , p_G_defined( TheEGGDefinedParameter->DefaultValue() )
   , p_G( TheEGGParameter->DefaultValue() )
   , p_M1_defined( TheEGM1DefinedParameter->DefaultValue() )
   , p_M1( TheEGM1Parameter->DefaultValue() )
   , p_K1_defined( TheEGK1DefinedParameter->DefaultValue() )
   , p_K1( TheEGK1Parameter->DefaultValue() )
   , p_M2_defined( TheEGM2DefinedParameter->DefaultValue() )
   , p_M2( TheEGM2Parameter->DefaultValue() )
   , p_K2_defined( TheEGK2DefinedParameter->DefaultValue() )
   , p_K2( TheEGK2Parameter->DefaultValue() )
   , p_PC_defined( TheEGPCDefinedParameter->DefaultValue() )
   , p_PC( TheEGPCParameter->DefaultValue() )
   , p_B_V_defined( TheEGB_VDefinedParameter->DefaultValue() )
   , p_B_V( TheEGB_VParameter->DefaultValue() )
   , p_U_B_defined( TheEGU_BDefinedParameter->DefaultValue() )
   , p_U_B( TheEGU_BParameter->DefaultValue() )
   , p_I_R_defined( TheEGI_RDefinedParameter->DefaultValue() )
   , p_I_R( TheEGI_RParameter->DefaultValue() )
   , p_D_defined( TheEGDDefinedParameter->DefaultValue() )
   , p_D( TheEGDParameter->DefaultValue() )
   , p_A1_defined( TheEGA1DefinedParameter->DefaultValue() )
   , p_A1( TheEGA1Parameter->DefaultValue() )
   , p_A2_defined( TheEGA2DefinedParameter->DefaultValue() )
   , p_A2( TheEGA2Parameter->DefaultValue() )
   , p_A3_defined( TheEGA3DefinedParameter->DefaultValue() )
   , p_A3( TheEGA3Parameter->DefaultValue() )
   , p_DT_defined( TheEGDTDefinedParameter->DefaultValue() )
   , p_DT( TheEGDTParameter->DefaultValue() )
   , p_databaseFilePath( TheEGDatabaseFilePathParameter->DefaultValue() )
   , p_databaseFormatName( TheEGDatabaseFormatNameParameter->DefaultValue() )
   , p_objects( TheEGObjectsParameter->DefaultValue() )
   , p_useRegularExpressions( TheEGUseRegularExpressionsParameter->DefaultValue() )
   , p_excludeCoreAsteroids( TheEGExcludeCoreAsteroidsParameter->DefaultValue() )
   , p_fundamentalFilePath( TheEGFundamentalFilePathParameter->DefaultValue() )
   , p_asteroidsFilePath( TheEGAsteroidsFilePathParameter->DefaultValue() )
   , p_KBOsFilePath( TheEGKBOsFilePathParameter->DefaultValue() )
   , p_startTimeJD( TheEGStartTimeJDParameter->DefaultValue() )
   , p_endTimeJD( TheEGEndTimeJDParameter->DefaultValue() )
   , p_useAsteroidPerturbers( TheEGUseAsteroidPerturbersParameter->DefaultValue() )
   , p_useKBOPerturbers( TheEGUseKBOPerturbersParameter->DefaultValue() )
   , p_velocityExpansions( TheEGVelocityExpansionsParameter->DefaultValue() )
   , p_separateEarthMoonPerturbers( TheEGSeparateEarthMoonPerturbersParameter->DefaultValue() )
   , p_relativisticPerturbations( TheEGRelativisticPerturbationsParameter->DefaultValue() )
   , p_figureEffects( TheEGFigureEffectsParameter->DefaultValue() )
   , p_nonGravitationalPerturbations( TheEGNonGravitationalPerturbationsParameter->DefaultValue() )
   , p_outputXEPHFile( TheEGOutputXEPHFileParameter->DefaultValue() )
   , p_outputXEPHFilePath( TheEGOutputXEPHFilePathParameter->DefaultValue() )
   , p_outputLogFile( TheEGOutputLogFileParameter->DefaultValue() )
   , p_overwriteExistingFiles( TheEGOverwriteExistingFilesParameter->DefaultValue() )
   , p_denseOutputToleranceFactor( TheEGDenseOutputToleranceFactorParameter->DefaultValue() )
   , p_ephemerisToleranceFactor( TheEGEphemerisToleranceFactorParameter->DefaultValue() )
   , p_ephemerisMaxExpansionLength( int32( TheEGEphemerisMaxExpansionLengthParameter->DefaultValue() ) )
   , p_ephemerisMaxTruncationError( TheEGEphemerisMaxTruncationErrorParameter->DefaultValue() )
{
   el_a = TheEGElemAParameter->DefaultValue();
   el_q = TheEGElemQParameter->DefaultValue();
   el_e = TheEGElemEParameter->DefaultValue();
   el_M = TheEGElemMParameter->DefaultValue();
   el_T = TheEGElemTParameter->DefaultValue();
   el_i = TheEGElemIParameter->DefaultValue();
   el_O = TheEGElemOParameter->DefaultValue();
   el_w = TheEGElemWParameter->DefaultValue();

   ToOsculatingElements().ToStateVectors( p_position, p_velocity, p_epochJD ); //, Ephemerides( *this, true/*fundamentalOnly*/ ).GMS() );
}

// ----------------------------------------------------------------------------

EphemerisGeneratorInstance::EphemerisGeneratorInstance( const EphemerisGeneratorInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInstance::Assign( const ProcessImplementation& p )
{
   const EphemerisGeneratorInstance* x = dynamic_cast<const EphemerisGeneratorInstance*>( &p );
   if ( x != nullptr )
   {
      p_workingMode = x->p_workingMode;
      p_position = x->p_position;
      p_velocity = x->p_velocity;
      p_elements = x->p_elements;
      p_epochJD = x->p_epochJD;
      p_objectId = x->p_objectId;
      p_objectName = x->p_objectName;
      p_H_defined = x->p_H_defined;
      p_H = x->p_H;
      p_G_defined = x->p_G_defined;
      p_G = x->p_G;
      p_M1_defined = x->p_M1_defined;
      p_M1 = x->p_M1;
      p_K1_defined = x->p_K1_defined;
      p_K1 = x->p_K1;
      p_M2_defined = x->p_M2_defined;
      p_M2 = x->p_M2;
      p_K2_defined = x->p_K2_defined;
      p_K2 = x->p_K2;
      p_PC_defined = x->p_PC_defined;
      p_PC = x->p_PC;
      p_B_V_defined = x->p_B_V_defined;
      p_B_V = x->p_B_V;
      p_U_B_defined = x->p_U_B_defined;
      p_U_B = x->p_U_B;
      p_I_R_defined = x->p_I_R_defined;
      p_I_R = x->p_I_R;
      p_D_defined = x->p_D_defined;
      p_D = x->p_D;
      p_A1_defined = x->p_A1_defined;
      p_A1 = x->p_A1;
      p_A2_defined = x->p_A2_defined;
      p_A2 = x->p_A2;
      p_A3_defined = x->p_A3_defined;
      p_A3 = x->p_A3;
      p_DT_defined = x->p_DT_defined;
      p_DT = x->p_DT;
      p_databaseFilePath = x->p_databaseFilePath;
      p_databaseFormatName = x->p_databaseFormatName;
      p_objects = x->p_objects;
      p_useRegularExpressions = x->p_useRegularExpressions;
      p_excludeCoreAsteroids = x->p_excludeCoreAsteroids;
      p_fundamentalFilePath = x->p_fundamentalFilePath;
      p_asteroidsFilePath = x->p_asteroidsFilePath;
      p_KBOsFilePath = x->p_KBOsFilePath;
      p_startTimeJD = x->p_startTimeJD;
      p_endTimeJD = x->p_endTimeJD;
      p_useAsteroidPerturbers = x->p_useAsteroidPerturbers;
      p_useKBOPerturbers = x->p_useKBOPerturbers;
      p_velocityExpansions = x->p_velocityExpansions;
      p_separateEarthMoonPerturbers = x->p_separateEarthMoonPerturbers;
      p_relativisticPerturbations = x->p_relativisticPerturbations;
      p_figureEffects = x->p_figureEffects;
      p_nonGravitationalPerturbations = x->p_nonGravitationalPerturbations;
      p_outputXEPHFile = x->p_outputXEPHFile;
      p_outputXEPHFilePath = x->p_outputXEPHFilePath;
      p_outputLogFile = x->p_outputLogFile;
      p_overwriteExistingFiles = x->p_overwriteExistingFiles;
      p_denseOutputToleranceFactor = x->p_denseOutputToleranceFactor;
      p_ephemerisToleranceFactor = x->p_ephemerisToleranceFactor;
      p_ephemerisMaxExpansionLength = x->p_ephemerisMaxExpansionLength;
      p_ephemerisMaxTruncationError = x->p_ephemerisMaxTruncationError;
   }
}

// ----------------------------------------------------------------------------

bool EphemerisGeneratorInstance::CanExecuteOn( const View&, String& whyNot ) const
{
//    long double a0, d0, W;
//    PerturberJupiter Ju( EphemerisFile::FundamentalEphemerides() );
//    Ju.GetRotation( a0, d0, W, TimePoint( IsoString( "2022-12-26T00:00:00" ) ) );
//
//    double sa = Sin( a0 );
//    double ca = Cos( a0 );
//    double sd = Sin( d0 );
//    double cd = Cos( d0 );
//    double sW = Sin( W );
//    double cW = Cos( W );
//    Matrix M(
//          -cW*sa - ca*sd*sW,  ca*cW - sa*sd*sW, cd*sW,
//          -ca*cW*sd + sa*sW, -cW*sa*sd - ca*sW, cd*cW,
//           ca*cd,             cd*sa,            sd );
//
//    std::cout << IsoString().Format( "%+.16f %+.16f %+.16f\n"
//                                     "%+.16f %+.16f %+.16f\n"
//                                     "%+.16f %+.16f %+.16f\n\n",
//                                     M[0][0], M[0][1], M[0][2],
//                                     M[1][0], M[1][1], M[1][2],
//                                     M[2][0], M[2][1], M[2][2] );

   return false;
}

// ----------------------------------------------------------------------------

bool EphemerisGeneratorInstance::CanExecuteGlobal( String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class EphemerisGenerationEngine
{
public:

   static SerializableEphemerisObjectData MakeEphemeris( const EphemerisGeneratorInstance& instance
                           , const IntegrationDenseOutputData& data
                           , int startJDI, int endJDI
                           , const IsoString& id, const String& name
                           , Optional<double> H, Optional<double> G
                           , Optional<double> M1, Optional<double> K1, Optional<double> M2, Optional<double> K2, Optional<double> PC
                           , Optional<double> B_V, Optional<double> U_B, Optional<double> I_R
                           , Optional<double> D
                           , Optional<double> A1, Optional<double> A2, Optional<double> A3, Optional<double> DT )
   {
      if ( data.IsEmpty() )
         throw Error( "MakeEphemeris(): Internal error: Empty integration data." );

      if ( data[0].t > startJDI + 0.5 || data[data.Length()-1].t + data[data.Length()-1].h < endJDI + 0.5 )
         throw Error( "MakeEphemeris(): Internal error: invalid dense output time spans." );

      IsoString objectId = id.Trimmed();
      if ( objectId.IsEmpty() )
         throw Error( "MakeEphemeris(): Internal error: empty object identifier." );

      Console console;
      console.WriteLn( "<end><cbr><br>* Generating ephemeris data for " + id + ' ' + name + "<br>" );
      bool rootThread = Thread::IsRootThread();

      SerializableEphemerisObjectData object( id, "SSB", name );
      object.H = H;
      object.G = G;
      object.M1 = M1;
      object.K1 = K1;
      object.M2 = M2;
      object.K2 = K2;
      object.PC = PC;
      object.B_V = B_V;
      object.U_B = U_B;
      object.I_R = I_R;
      object.D = D;
      object.A1 = A1;
      object.A2 = A2;
      object.A3 = A3;
      object.DT = DT;

      bool haveVelocity = !data[0].c1[0].IsEmpty();

      for ( int order = 0, startDelta = 0; order <= 1; ++order )
      {
         IntegrationDenseOutputEvaluation dense( data, order );
         if ( order == 0 )
            startDelta = InitialExpansionSpan( dense( startJDI + 0.5 ).L2Norm() );

         int delta = startDelta;
         int minDelta = int32_max;
         int maxDelta = 0;
         double maxError[ 3 ] = {};
         int totalCoefficients = 0;

         for ( int jdi1 = startJDI, count = 0; jdi1 < endJDI; ++count )
         {
            int jdi2;
            int length = 25;
            Vector epsilon( dense.Tolerance( jdi1 + 0.5 ) * instance.p_ephemerisToleranceFactor, 3 );
            ChebyshevFit T;

            for ( bool reduce = false, truncated = false;; )
            {
               Module->ProcessEvents();
               if ( rootThread )
                  if ( console.AbortRequested() )
                     throw ProcessAborted();

               jdi2 = jdi1 + delta;
               if ( jdi2 > endJDI )
               {
                  jdi2 = endJDI;
                  delta = jdi2 - jdi1;
               }

               T = ChebyshevFit( [=]( double dt )
                                 {
                                    return dense( jdi1 + 0.5 + dt );
                                 },
                                 0, delta, 3, 2*length );

               T.Truncate( epsilon );
               if ( T.TruncatedLength() > length )
               {
                  --delta;
                  reduce = true;
               }
               else if ( !reduce && jdi2 < endJDI )
               {
                  ++delta;
               }
               else
               {
                  if ( truncated )
                     console.WarningLn( String().Format( "** Warning: Increased truncation error to %.3e", epsilon.MaxComponent() ) );
                  break;
               }

               if ( delta == 0 )
               {
                  if ( !truncated )
                     truncated = length >= instance.p_ephemerisMaxExpansionLength;

                  if ( !truncated )
                  {
                     /*
                      * Fast movement: try with longer coefficient series.
                      * This may happen during close encounters.
                      */
                     length += 5;
                  }
                  else
                  {
                     /*
                      * Extremely fast movement or too low tolerance factor:
                      * Increase truncation error.
                      */
                     for ( int i = 0; i < 3; ++i )
                        if ( T.TruncatedLength( i ) > length )
                        {
                           epsilon[i] *= 1.1;
                           if ( epsilon[i] > instance.p_ephemerisMaxTruncationError )
                              throw Error( String().Format( "Maximum allowed truncation error exceeded (%.3e) for object ", epsilon[i] ) + id + ' ' + name );
                        }
                  }

                  delta = 16;
                  reduce = false;
               }
            }

            if ( T.TruncationError() > epsilon.MaxComponent() )
               throw Error( String().Format( "MakeEphemeris(): Internal error: %d -> %d (%d) : nx=%d ny=%d nz=%d e=%.3e",
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
               if ( jdi2 < endJDI || count == 0 )
                  minDelta = delta;
            if ( delta > maxDelta )
               maxDelta = delta;

            jdi1 = jdi2;
         }

         console.WriteLn( "\nObject ....................... " + id + ' ' + name
                        + "\nDerivative order ............. " + String( order )
                        + "\nTotal time span .............. " + String().Format( "%+10.1f -> %+10.1f (%d days)"
                                                                  , startJDI+0.5, endJDI+0.5, Abs( endJDI - startJDI ) )
                        + "\nTotal Chebyshev expansions ... " + String( object.data[order].Length() )
                        + "\nSmallest time span ........... " + String( minDelta ) + " (days)"
                        + "\nLargest time span ............ " + String( maxDelta ) + " (days)"
                        + "\nLargest truncation errors .... " + String().Format( "%.3e  %.3e  %.3e  (%s)"
                                                                  , maxError[0], maxError[1], maxError[2], order ? "au/day" : "au" )
                        + "\nTotal coefficients ........... " + String( totalCoefficients ) );

         Module->ProcessEvents();

         if ( !haveVelocity )
            break;
      }

      return object;
   }

private:

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
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct IntegrationObjectData
{
   Vector           initialPosition;
   Vector           initialVelocity;
   TimePoint        epoch;
   IsoString        objectId;
   IsoString        objectName;
   Optional<double> H;
   Optional<double> G;
   Optional<double> M1;
   Optional<double> K1;
   Optional<double> M2;
   Optional<double> K2;
   Optional<double> PC;
   Optional<double> B_V;
   Optional<double> U_B;
   Optional<double> I_R;
   Optional<double> D;
   Optional<double> A1;
   Optional<double> A2;
   Optional<double> A3;
   Optional<double> DT;

   bool HaveNonGravitationalParameters() const
   {
      return A1.IsDefined() && A1() != 0
          || A2.IsDefined() && A2() != 0
          || A3.IsDefined() && A3() != 0;
   }
};

using IntegrationObjectDataList = Array<IntegrationObjectData>;

class IntegrationThread : public Thread
{
public:

   IntegrationThread( AbstractImage::ThreadData& threadData
                    , const EphemerisGeneratorInstance& instance
                    , const Ephemerides& eph
                    , const IntegrationObjectDataList& objects
                    , size_type start, size_type end )
      : m_data( threadData )
      , m_instance( instance )
      , m_eph( eph )
      , m_objects( objects )
      , m_start( start )
      , m_end( end )
   {
   }

   void Run() override
   {
      INIT_THREAD_MONITOR()

      Console console;
      bool rootThread = IsRootThread();

      for ( size_type index = m_start; index < m_end; ++index )
      {
         const IntegrationObjectData& object = m_objects[index];

         try
         {
            console.WriteLn(    "<end><cbr><br>Integrating orbital motion for " + object.objectId + ' ' + object.objectName + "<br>" );
            console.WriteLn(                  "Initial conditions:" );
            console.WriteLn( String().Format( "Epoch (TDB) ......... %.8f", object.epoch.JD() ) + " = " + TimeToString( object.epoch ) );
            console.WriteLn( String().Format( "Position (au) ....... %+.16e %+.16e %+.16e<br>"
                                              "Velocity (au/day) ... %+.16e %+.16e %+.16e"
                                             , object.initialPosition[0], object.initialPosition[1], object.initialPosition[2]
                                             , object.initialVelocity[0], object.initialVelocity[1], object.initialVelocity[2] ) );

            TimePoint ephemerisStartTime( Min( m_instance.p_startTimeJD, m_instance.p_endTimeJD ) );
            TimePoint ephemerisEndTime( Max( m_instance.p_startTimeJD, m_instance.p_endTimeJD ) );

            TimePoint startTime = ephemerisStartTime;
            TimePoint endTime = ephemerisEndTime;
            if ( startTime > object.epoch )
               startTime = object.epoch;
            if ( endTime < object.epoch )
               endTime = object.epoch;

            StandardStatus status;
            StatusMonitor monitor;
            if ( rootThread )
            {
               monitor.SetCallback( &status );
               monitor.Initialize( String().Format( "Performing numerical integration: %.6f -> %.6f", startTime.JD(), endTime.JD() )
                                 , size_type( endTime - startTime ) );
            }

            Integration integrator( m_eph, m_instance, object.objectId/*excludeId*/, object.objectName/*excludeName*/ );

            if ( m_instance.p_nonGravitationalPerturbations )
               if ( object.HaveNonGravitationalParameters() )
                  integrator.SetNonGravitationalParameters( object.A1, object.A2, object.A3, object.DT );

            IntegrationDenseOutputData integrationData;
            TimePoint t0, t1;
            Vector r0, v0, r1, v1;
            double hacc = 0;

            for ( int forward = 0; forward < 2; ++forward )
            {
               Vector r = object.initialPosition;
               Vector v = object.initialVelocity;
               TimePoint t = forward ? endTime : startTime;
               if ( t != object.epoch )
               {
                  integrator( r, v, object.epoch, t,
                              [&]( size_type step, double h, bool stop )
                              {
                                 if ( unlikely( stop ) )
                                 {
                                    monitor.Clear();
                                    return;
                                 }

                                 hacc += h;
                                 if ( hacc > 1 )
                                 {
                                    size_type d = size_type( hacc );
                                    hacc -= d;
                                    if ( rootThread )
                                       monitor += d;
                                 }
                                 Module->ProcessEvents();
                                 if ( rootThread )
                                    if ( console.AbortRequested() )
                                       throw ProcessAborted();
                              } );

                  if ( m_instance.p_outputXEPHFile )
                     integrationData << integrator.OutputData();

                  t = integrator.FinalTime();
                  Integration<>::State s = integrator.FinalState();
                  r = Vector( s[0], s[1], s[2] );
                  v = Vector( s[3], s[4], s[5] );
               }

               if ( forward )
               {
                  t1 = t;
                  r1 = r;
                  v1 = v;
               }
               else
               {
                  t0 = t;
                  r0 = r;
                  v0 = v;
               }
            }

            if ( rootThread )
               monitor.Complete();

            console.WriteLn(    "<end><cbr><br>Integration results:<br>" );
            console.WriteLn( String().Format( "Start time (TDB) .... %.8f", t0.JD() ) + " = " + TimeToString( t0 ) );
            console.WriteLn( String().Format( "Position (au) ....... %+.10e %+.10e %+.10e<br>"
                                              "Velocity (au/day) ... %+.10e %+.10e %+.10e<br>"
                                             , r0[0], r0[1], r0[2], v0[0], v0[1], v0[2] ) );
            console.WriteLn( String().Format( "End time (TDB) ...... %.8f", t1.JD() ) + " = " + TimeToString( t1 ) );
            console.WriteLn( String().Format( "Position (au) ....... %+.10e %+.10e %+.10e<br>"
                                              "Velocity (au/day) ... %+.10e %+.10e %+.10e"
                                             , r1[0], r1[1], r1[2], v1[0], v1[1], v1[2] ) );

            if ( m_instance.p_outputXEPHFile )
            {
               integrationData.Sort();
               int startJDI = ephemerisStartTime.JDI();
               int endJDI = ephemerisEndTime.JDI();
               if ( ephemerisEndTime.JDF() < 0.5 )
                  --endJDI;
               m_ephemerides << EphemerisGenerationEngine::MakeEphemeris( m_instance
                                                      , integrationData
                                                      , startJDI, endJDI
                                                      , object.objectId, object.objectName
                                                      , object.H
                                                      , object.G
                                                      , object.M1
                                                      , object.K1
                                                      , object.M2
                                                      , object.K2
                                                      , object.PC
                                                      , object.B_V
                                                      , object.U_B
                                                      , object.I_R
                                                      , object.D
                                                      , object.A1
                                                      , object.A2
                                                      , object.A3
                                                      , object.DT );
            }
         }
         catch ( ... )
         {
            m_failed << object.objectId.ToString() + ' ' + object.objectName;

            try
            {
               throw;
            }
            ERROR_HANDLER
         }

         UPDATE_THREAD_MONITOR( 1 )
      }
   }

   const SerializableEphemerisObjectDataList& EphemeridesData() const
   {
      return m_ephemerides;
   }

   bool Failed() const
   {
      return !m_failed.IsEmpty();
   }

   const StringList& FailedObjects() const
   {
      return m_failed;
   }

private:

   const AbstractImage::ThreadData&          m_data;
   const EphemerisGeneratorInstance&         m_instance;
   const Ephemerides&                        m_eph;
   const IntegrationObjectDataList&          m_objects;
         size_type                           m_start, m_end;
         SerializableEphemerisObjectDataList m_ephemerides;
         StringList                          m_failed;

   static String TimeToString( const TimePoint& t )
   {
      return t.ToString( ISO8601ConversionOptionsNoTimeZone() );
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static String UniqueFilePath( const String& filePath )
{
   for ( unsigned u = 1; ; ++u )
   {
      String tryFilePath = File::AppendToName( filePath, '_' + String( u ) );
      if ( !File::Exists( tryFilePath ) )
         return tryFilePath;
   }
}

bool EphemerisGeneratorInstance::ExecuteGlobal()
{
   Ephemerides eph( *this );

   TimePoint startTime( p_startTimeJD );
   TimePoint endTime( p_endTimeJD );
   if ( endTime < startTime )
      Swap( startTime, endTime );

   if ( startTime < eph.StartTime() || endTime > eph.EndTime() )
      throw Error( "Integration time span out of range. The supported range is: "
                  + eph.StartTime().ToString() + " -> " + eph.EndTime().ToString() );
   if ( endTime - startTime < 5 )
      throw Error( IsoString().Format( "Too short integration time span: %.2f. At least 5 days are required.", endTime - startTime ) );

   if ( p_outputXEPHFile )
   {
      if ( p_outputXEPHFilePath.IsEmpty() )
         throw Error( "No output ephemerides file has been specified." );
      if ( File::ExtractSuffix( p_outputXEPHFilePath ) != ".xeph" )
         throw Error( "The output ephemerides file name must have the '.xeph' extension." );
   }

   Console console;
   console.EnableAbort();

   /*
    * N.B. Unique object identifiers are mandatory in XEPH files, so we have to
    * generate one (a dumb one, really) automatically when none is available.
    * Automatically generated object identifiers follow the pattern X<n>, where
    * <n> is a zero-padded running number from 1 to N and N is the total number
    * of objects in the XEPH file.
    */

   IntegrationObjectDataList objects;

   switch ( p_workingMode )
   {
   default:
   case EGWorkingMode::StateVectors:
      {
         if ( p_epochJD < eph.StartTime() || p_epochJD > eph.EndTime() )
            throw Error( "Epoch of state vectors out of range. The supported range is: "
                        + eph.StartTime().ToString() + " -> " + eph.EndTime().ToString() );

         objects << IntegrationObjectData{ p_position, p_velocity
                                          , p_epochJD
                                          , p_objectId.IsEmpty() ? IsoString( "X0001" ) : p_objectId.ToIsoString()
                                          , p_objectName.ToIsoString()
                                          , p_H_defined ? Optional<double>( p_H ) : Optional<double>()
                                          , p_G_defined ? Optional<double>( p_G ) : Optional<double>()
                                          , p_M1_defined ? Optional<double>( p_M1 ) : Optional<double>()
                                          , p_K1_defined ? Optional<double>( p_K1 ) : Optional<double>()
                                          , p_M2_defined ? Optional<double>( p_M2 ) : Optional<double>()
                                          , p_K2_defined ? Optional<double>( p_K2 ) : Optional<double>()
                                          , p_PC_defined ? Optional<double>( p_PC ) : Optional<double>()
                                          , p_B_V_defined ? Optional<double>( p_B_V ) : Optional<double>()
                                          , p_U_B_defined ? Optional<double>( p_U_B ) : Optional<double>()
                                          , p_I_R_defined ? Optional<double>( p_I_R ) : Optional<double>()
                                          , p_D_defined ? Optional<double>( p_D ) : Optional<double>()
                                          , p_A1_defined ? Optional<double>( p_A1 ) : Optional<double>()
                                          , p_A2_defined ? Optional<double>( p_A2 ) : Optional<double>()
                                          , p_A3_defined ? Optional<double>( p_A3 ) : Optional<double>()
                                          , p_DT_defined ? Optional<double>( p_DT ) : Optional<double>() };
      }
      break;

   case EGWorkingMode::OrbitalElements:
      {
         if ( p_epochJD < eph.StartTime() || p_epochJD > eph.EndTime() )
            throw Error( "Epoch of osculating orbital elements out of range. The supported range is: "
                        + eph.StartTime().ToString() + " -> " + eph.EndTime().ToString() );

         Vector initialPosition, initialVelocity;
         ToOsculatingElements().ToStateVectors( initialPosition, initialVelocity, p_epochJD, eph.GMS() );

         Position P( TimePoint::J2000(), "TDB" );
         initialPosition = Position::EclipticToEquatorial( initialPosition, P.EpsA() );
         initialVelocity = Position::EclipticToEquatorial( initialVelocity, P.EpsA() );

         EphemerisFile::Handle sun( eph.Fundamental(), "Sn", "SSB" );
         Vector r0, v0;
         sun.ComputeState( r0, v0, p_epochJD );
         initialPosition += r0;
         initialVelocity += v0;

         objects << IntegrationObjectData{ initialPosition, initialVelocity
                                          , p_epochJD
                                          , p_objectId.IsEmpty() ? IsoString( "X0001" ) : p_objectId.ToIsoString()
                                          , p_objectName.ToIsoString()
                                          , p_H_defined ? Optional<double>( p_H ) : Optional<double>()
                                          , p_G_defined ? Optional<double>( p_G ) : Optional<double>()
                                          , p_M1_defined ? Optional<double>( p_M1 ) : Optional<double>()
                                          , p_K1_defined ? Optional<double>( p_K1 ) : Optional<double>()
                                          , p_M2_defined ? Optional<double>( p_M2 ) : Optional<double>()
                                          , p_K2_defined ? Optional<double>( p_K2 ) : Optional<double>()
                                          , p_PC_defined ? Optional<double>( p_PC ) : Optional<double>()
                                          , p_B_V_defined ? Optional<double>( p_B_V ) : Optional<double>()
                                          , p_U_B_defined ? Optional<double>( p_U_B ) : Optional<double>()
                                          , p_I_R_defined ? Optional<double>( p_I_R ) : Optional<double>()
                                          , p_D_defined ? Optional<double>( p_D ) : Optional<double>()
                                          , p_A1_defined ? Optional<double>( p_A1 ) : Optional<double>()
                                          , p_A2_defined ? Optional<double>( p_A2 ) : Optional<double>()
                                          , p_A3_defined ? Optional<double>( p_A3 ) : Optional<double>()
                                          , p_DT_defined ? Optional<double>( p_DT ) : Optional<double>() };
      }
      break;

   case EGWorkingMode::DatabaseObjects:
      {
         EphemerisGeneratorProcess::InitializeDatabaseFormats();

         if ( p_databaseFilePath.IsEmpty() )
            throw Error( "EphemerisGenerator: No database file has been specified." );

         int formatIndex = EphemerisGeneratorProcess::FindDatabaseFormatByName( p_databaseFormatName );
         if ( formatIndex < 0 )
            throw Error( "EphemerisGenerator: Database format unknown or not available: '" + p_databaseFormatName + '\'' );

         TextDatabase::FormatDescription format = EphemerisGeneratorProcess::DatabaseFormats()[formatIndex];
         TextDatabase database( format );
         Array<TextDatabase::ObjectData> objectData = database.Search( *this/*, 100000*//*maxCount*/ );
         if ( objectData.IsEmpty() )
            throw Error( "EphemerisGenerator: No objects found." );

         for ( const TextDatabase::ObjectData& object : objectData )
         {
            try
            {
               if ( object.epochJD < eph.StartTime() || object.epochJD > eph.EndTime() )
               {
                  console.WarningLn( "** Skipping object with initial conditions epoch out of range: "
                                     "id = '" + object.id + "', name = '" + object.name + "'" );
                  continue;
               }

               Module->ProcessEvents();

               Vector initialPosition, initialVelocity;
               if ( format.contents == TextDatabase::Contents_OrbitalElements )
               {
                  OsculatingElements el;
                  el.a = object.state[a_idx];
                  el.q = object.state[q_idx];
                  el.e = object.state[e_idx];
                  el.M = Rad( object.state[M_idx] );
                  el.T = TimePoint( object.state[T_idx] );
                  el.i = Rad( object.state[i_idx] );
                  el.O = Rad( object.state[O_idx] );
                  el.w = Rad( object.state[w_idx] );
                  el.ToStateVectors( initialPosition, initialVelocity, object.epochJD, eph.GMS() );

                  Position P( TimePoint::J2000(), "TDB" );
                  initialPosition = Position::EclipticToEquatorial( initialPosition, P.EpsA() );
                  initialVelocity = Position::EclipticToEquatorial( initialVelocity, P.EpsA() );

                  EphemerisFile::Handle sun( eph.Fundamental(), "Sn", "SSB" );
                  Vector r0, v0;
                  sun.ComputeState( r0, v0, object.epochJD );
                  initialPosition += r0;
                  initialVelocity += v0;
               }
               else
               {
                  initialPosition = Vector( object.state[0], object.state[1], object.state[2] );
                  initialVelocity = Vector( object.state[3], object.state[4], object.state[5] );
               }

               objects << IntegrationObjectData{ initialPosition, initialVelocity
                                                , object.epochJD
                                                , object.id.IsEmpty() ? IsoString().Format( "X%04u", objects.Length()+1 ) : object.id
                                                , object.name
                                                , object.H
                                                , object.G
                                                , object.M1
                                                , object.K1
                                                , object.M2
                                                , object.K2
                                                , object.PC
                                                , object.B_V
                                                , object.U_B
                                                , object.I_R
                                                , object.D
                                                , object.A1
                                                , object.A2
                                                , object.A3
                                                , object.DT };
            }
            catch ( const Exception& ex )
            {
               console.CriticalLn( "*** Error: " + ex.Message() + "\nobject id = '" + object.id + "', name = '" + object.name + "'" );
            }
            catch ( ... )
            {
               throw;
            }
         }

         if ( objects.IsEmpty() )
            throw Error( "EphemerisGenerator: No objects could be selected." );
      }
      break;
   }

   StatusMonitor monitor;

   SerializableEphemerisObjectDataList ephemerides;
   StringList failed;

   if ( objects.Length() > 1 )
   {
      Array<size_type> L = Thread::OptimalThreadLoads( objects.Length() );
      if ( L.Length() > 1 )
         console.NoteLn( String().Format( "<end><cbr><br>* EphemerisGenerator: Using %u worker threads.", L.Length() ) );

      StandardStatus status;
      monitor.SetCallback( &status );
      monitor.Initialize( "Performing numerical integration", objects.Length() );
      Module->ProcessEvents();

      AbstractImage::ThreadData threadData( monitor, objects.Length() );

      ReferenceArray<IntegrationThread> threads;
      for ( size_type i = 0, n = 0; i < L.Length(); n += L[i++] )
         if ( L[i] > 0 ) // ?!
            threads.Add( new IntegrationThread( threadData, *this, eph, objects, n, n + L[i] ) );
      AbstractImage::RunThreads( threads, threadData );

      for ( const IntegrationThread& thread : threads )
         failed << thread.FailedObjects();

      if ( p_outputXEPHFile )
      {
         for ( const IntegrationThread& thread : threads )
            ephemerides << thread.EphemeridesData();

         if ( p_outputLogFile )
         {
            IsoString logText;
            logText << IsoString( '*', 79 ) << '\n'
                  << TimePoint::Now().ToIsoString() << '\n'
                  << IsoString( '*', 79 ) << '\n';
            for ( const IntegrationThread& thread : threads )
               logText << thread.ConsoleOutputText().ToUTF8();
            logText.DeleteString( "<end>" );
            logText.DeleteString( "<cbr>" );
            logText.DeleteString( "\x1b[31m" );
            logText.DeleteString( "\x1b[32m" );
            logText.DeleteString( "\x1b[35m" );
            logText.DeleteString( "\x1b[39m" );
            logText.ReplaceString( "<br>", "\n" );

            String logFilePath = File::ChangeExtension( p_outputXEPHFilePath, ".log" );
            console.WriteLn( "<end><cbr><br>Generating output log file: <raw>" + logFilePath + "</raw>" );
            if ( File::Exists( logFilePath ) )
               if ( p_overwriteExistingFiles )
                  console.WarningLn( "** Warning: Overwriting existing file" );
               else
               {
                  logFilePath = UniqueFilePath( logFilePath );
                  console.NoteLn( "* File already exists, writing to: " + logFilePath );
               }
            File::WriteTextFile( logFilePath, logText );
         }
      }

      threads.Destroy();
   }
   else
   {
      console.WriteLn( String().Format( "<end><cbr><br>Performing numerical integration: %u object%s."
                                       , objects.Length(), (objects.Length() > 1) ? "s" : "" ) );
      Module->ProcessEvents();

      AbstractImage::ThreadData threadData;
      IntegrationThread thread( threadData, *this, eph, objects, 0, objects.Length() );
      thread.Run();

      failed << thread.FailedObjects();

      if ( p_outputXEPHFile )
         ephemerides = thread.EphemeridesData();
   }

   Module->ProcessEvents();

   if ( failed.Length() == objects.Length() )
      throw Error( "No object could be successfully integrated." );

   if ( !failed.IsEmpty() )
      console.CriticalLn( "<end><cbr><br>*** Error: The following objects couldn't be successfully integrated:<br>" + String().ToNewLineSeparated( failed ) );

   if ( p_outputXEPHFile )
   {
      String outputFilePath = p_outputXEPHFilePath;
      console.WriteLn( "<end><cbr><br>Generating output ephemerides file: <raw>" + p_outputXEPHFilePath + "</raw>" );
      if ( File::Exists( p_outputXEPHFilePath ) )
         if ( p_overwriteExistingFiles )
            console.WarningLn( "** Warning: Overwriting existing file" );
         else
         {
            outputFilePath = UniqueFilePath( p_outputXEPHFilePath );
            console.NoteLn( "* File already exists, writing to: " + outputFilePath );
         }
      Module->ProcessEvents();

      EphemerisMetadata metadata;
      metadata.creatorApplication = PixInsightVersion::AsString();
      // ### TODO: Allow users define the following metadata items.
//    metadata.title = ;
//    metadata.briefDescription = ;
//    metadata.description = ;
      metadata.organizationName = "Pleiades Astrophoto S.L.";
      metadata.authors = "PTeam";
      metadata.copyright = String().Format( "Copyright (c) %d, Pleiades Astrophoto S.L.", TimePoint::Now().Year() );

      int startJDI = startTime.JDI();
      int endJDI = endTime.JDI();
      if ( endTime.JDF() < 0.5 )
         --endJDI;

      EphemerisFile::Serialize( outputFilePath,
                                TimePoint( startJDI, 0.5 ), TimePoint( endJDI, 0.5 ),
                                ephemerides, metadata );
   }

   return true;
}

// ----------------------------------------------------------------------------

void* EphemerisGeneratorInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheEGWorkingModeParameter )
      return &p_workingMode;
   if ( p == TheEGPosXParameter )
      return p_position.At( 0 );
   if ( p == TheEGPosYParameter )
      return p_position.At( 1 );
   if ( p == TheEGPosZParameter )
      return p_position.At( 2 );
   if ( p == TheEGVelXParameter )
      return p_velocity.At( 0 );
   if ( p == TheEGVelYParameter )
      return p_velocity.At( 1 );
   if ( p == TheEGVelZParameter )
      return p_velocity.At( 2 );
   if ( p == TheEGElemAParameter )
      return &el_a;
   if ( p == TheEGElemQParameter )
      return &el_q;
   if ( p == TheEGElemEParameter )
      return &el_e;
   if ( p == TheEGElemMParameter )
      return &el_M;
   if ( p == TheEGElemTParameter )
      return &el_T;
   if ( p == TheEGElemIParameter )
      return &el_i;
   if ( p == TheEGElemOParameter )
      return &el_O;
   if ( p == TheEGElemWParameter )
      return &el_w;
   if ( p == TheEGEpochJDParameter )
      return &p_epochJD;
   if ( p == TheEGObjectIdParameter )
      return p_objectId.Begin();
   if ( p == TheEGObjectNameParameter )
      return p_objectName.Begin();
   if ( p == TheEGHDefinedParameter )
      return &p_H_defined;
   if ( p == TheEGHParameter )
      return &p_H;
   if ( p == TheEGGDefinedParameter )
      return &p_G_defined;
   if ( p == TheEGGParameter )
      return &p_G;
   if ( p == TheEGM1DefinedParameter )
      return &p_M1_defined;
   if ( p == TheEGM1Parameter )
      return &p_M1;
   if ( p == TheEGK1DefinedParameter )
      return &p_K1_defined;
   if ( p == TheEGK1Parameter )
      return &p_K1;
   if ( p == TheEGM2DefinedParameter )
      return &p_M2_defined;
   if ( p == TheEGM2Parameter )
      return &p_M2;
   if ( p == TheEGK2DefinedParameter )
      return &p_K2_defined;
   if ( p == TheEGK2Parameter )
      return &p_K2;
   if ( p == TheEGPCDefinedParameter )
      return &p_PC_defined;
   if ( p == TheEGPCParameter )
      return &p_PC;
   if ( p == TheEGB_VDefinedParameter )
      return &p_B_V_defined;
   if ( p == TheEGB_VParameter )
      return &p_B_V;
   if ( p == TheEGU_BDefinedParameter )
      return &p_U_B_defined;
   if ( p == TheEGU_BParameter )
      return &p_U_B;
   if ( p == TheEGI_RDefinedParameter )
      return &p_I_R_defined;
   if ( p == TheEGI_RParameter )
      return &p_I_R;
   if ( p == TheEGDDefinedParameter )
      return &p_D_defined;
   if ( p == TheEGDParameter )
      return &p_D;
   if ( p == TheEGA1DefinedParameter )
      return &p_A1_defined;
   if ( p == TheEGA1Parameter )
      return &p_A1;
   if ( p == TheEGA2DefinedParameter )
      return &p_A2_defined;
   if ( p == TheEGA2Parameter )
      return &p_A2;
   if ( p == TheEGA3DefinedParameter )
      return &p_A3_defined;
   if ( p == TheEGA3Parameter )
      return &p_A3;
   if ( p == TheEGDTDefinedParameter )
      return &p_DT_defined;
   if ( p == TheEGDTParameter )
      return &p_DT;
   if ( p == TheEGDatabaseFilePathParameter )
      return p_databaseFilePath.Begin();
   if ( p == TheEGDatabaseFormatNameParameter )
      return p_databaseFormatName.Begin();
   if ( p == TheEGObjectsParameter )
      return p_objects.Begin();
   if ( p == TheEGUseRegularExpressionsParameter )
      return &p_useRegularExpressions;
   if ( p == TheEGExcludeCoreAsteroidsParameter )
      return &p_excludeCoreAsteroids;
   if ( p == TheEGFundamentalFilePathParameter )
      return p_fundamentalFilePath.Begin();
   if ( p == TheEGAsteroidsFilePathParameter )
      return p_asteroidsFilePath.Begin();
   if ( p == TheEGKBOsFilePathParameter )
      return p_KBOsFilePath.Begin();
   if ( p == TheEGStartTimeJDParameter )
      return &p_startTimeJD;
   if ( p == TheEGEndTimeJDParameter )
      return &p_endTimeJD;
   if ( p == TheEGUseAsteroidPerturbersParameter )
      return &p_useAsteroidPerturbers;
   if ( p == TheEGUseKBOPerturbersParameter )
      return &p_useKBOPerturbers;
   if ( p == TheEGVelocityExpansionsParameter )
      return &p_velocityExpansions;
   if ( p == TheEGSeparateEarthMoonPerturbersParameter )
      return &p_separateEarthMoonPerturbers;
   if ( p == TheEGRelativisticPerturbationsParameter )
      return &p_relativisticPerturbations;
   if ( p == TheEGFigureEffectsParameter )
      return &p_figureEffects;
   if ( p == TheEGNonGravitationalPerturbationsParameter )
      return &p_nonGravitationalPerturbations;
   if ( p == TheEGOutputXEPHFileParameter )
      return &p_outputXEPHFile;
   if ( p == TheEGOutputXEPHFilePathParameter )
      return p_outputXEPHFilePath.Begin();
   if ( p == TheEGOutputLogFileParameter )
      return &p_outputLogFile;
   if ( p == TheEGOverwriteExistingFilesParameter )
      return &p_overwriteExistingFiles;
   if ( p == TheEGDenseOutputToleranceFactorParameter )
      return &p_denseOutputToleranceFactor;
   if ( p == TheEGEphemerisToleranceFactorParameter )
      return &p_ephemerisToleranceFactor;
   if ( p == TheEGEphemerisMaxExpansionLengthParameter )
      return &p_ephemerisMaxExpansionLength;
   if ( p == TheEGEphemerisMaxTruncationErrorParameter )
      return &p_ephemerisMaxTruncationError;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool EphemerisGeneratorInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheEGObjectIdParameter )
   {
      p_objectId.Clear();
      if ( sizeOrLength > 0 )
         p_objectId.SetLength( sizeOrLength );
   }
   else if ( p == TheEGObjectNameParameter )
   {
      p_objectName.Clear();
      if ( sizeOrLength > 0 )
         p_objectName.SetLength( sizeOrLength );
   }
   else if ( p == TheEGDatabaseFilePathParameter )
   {
      p_databaseFilePath.Clear();
      if ( sizeOrLength > 0 )
         p_databaseFilePath.SetLength( sizeOrLength );
   }
   else if ( p == TheEGDatabaseFormatNameParameter )
   {
      p_databaseFormatName.Clear();
      if ( sizeOrLength > 0 )
         p_databaseFormatName.SetLength( sizeOrLength );
   }
   else if ( p == TheEGObjectsParameter )
   {
      p_objects.Clear();
      if ( sizeOrLength > 0 )
         p_objects.SetLength( sizeOrLength );
   }
   else if ( p == TheEGFundamentalFilePathParameter )
   {
      p_fundamentalFilePath.Clear();
      if ( sizeOrLength > 0 )
         p_fundamentalFilePath.SetLength( sizeOrLength );
   }
   else if ( p == TheEGAsteroidsFilePathParameter )
   {
      p_asteroidsFilePath.Clear();
      if ( sizeOrLength > 0 )
         p_asteroidsFilePath.SetLength( sizeOrLength );
   }
   else if ( p == TheEGKBOsFilePathParameter )
   {
      p_KBOsFilePath.Clear();
      if ( sizeOrLength > 0 )
         p_KBOsFilePath.SetLength( sizeOrLength );
   }
   else if ( p == TheEGOutputXEPHFilePathParameter )
   {
      p_outputXEPHFilePath.Clear();
      if ( sizeOrLength > 0 )
         p_outputXEPHFilePath.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type EphemerisGeneratorInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheEGObjectIdParameter )
      return p_objectId.Length();
   if ( p == TheEGObjectNameParameter )
      return p_objectName.Length();
   if ( p == TheEGDatabaseFilePathParameter )
      return p_databaseFilePath.Length();
   if ( p == TheEGDatabaseFormatNameParameter )
      return p_databaseFormatName.Length();
   if ( p == TheEGObjectsParameter )
      return p_objects.Length();
   if ( p == TheEGFundamentalFilePathParameter )
      return p_fundamentalFilePath.Length();
   if ( p == TheEGAsteroidsFilePathParameter )
      return p_asteroidsFilePath.Length();
   if ( p == TheEGKBOsFilePathParameter )
      return p_KBOsFilePath.Length();
   if ( p == TheEGOutputXEPHFilePathParameter )
      return p_outputXEPHFilePath.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF EphemerisGeneratorInstance.cpp - Released 2024-12-28T16:54:15Z
