//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// EphemerisGeneratorInstance.cpp - Released 2021-04-09T19:41:48Z
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

#include "Ephemerides.h"
#include "EphemerisGeneratorInstance.h"
#include "EphemerisGeneratorParameters.h"
#include "EphemerisGeneratorProcess.h"
#include "Integration.h"
#include "XEPHGenerator.h"

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
   , p_H( TheEGHParameter->DefaultValue() )
   , p_G( TheEGGParameter->DefaultValue() )
   , p_B_V_defined( TheEGB_VDefinedParameter->DefaultValue() )
   , p_B_V( TheEGB_VParameter->DefaultValue() )
   , p_D_defined( TheEGDDefinedParameter->DefaultValue() )
   , p_D( TheEGDParameter->DefaultValue() )
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
   , p_outputXEPHFile( TheEGOutputXEPHFileParameter->DefaultValue() )
   , p_outputXEPHFilePath( TheEGOutputXEPHFilePathParameter->DefaultValue() )
   , p_overwriteExistingFiles( TheEGOverwriteExistingFilesParameter->DefaultValue() )
   , p_denseOutputToleranceFactor( TheEGDenseOutputToleranceFactorParameter->DefaultValue() )
   , p_ephemerisToleranceFactor( TheEGEphemerisToleranceFactorParameter->DefaultValue() )
{
   el_a = TheEGElemAParameter->DefaultValue();
   el_q = TheEGElemQParameter->DefaultValue();
   el_e = TheEGElemEParameter->DefaultValue();
   el_M = TheEGElemMParameter->DefaultValue();
   el_T = TheEGElemTParameter->DefaultValue();
   el_i = TheEGElemIParameter->DefaultValue();
   el_O = TheEGElemOParameter->DefaultValue();
   el_w = TheEGElemWParameter->DefaultValue();

   StateVectorsFromOrbitalElements( p_position, p_velocity, p_elements, p_epochJD );
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
      p_H = x->p_H;
      p_G = x->p_G;
      p_B_V_defined = x->p_B_V_defined;
      p_B_V = x->p_B_V;
      p_D_defined = x->p_D_defined;
      p_D = x->p_D;
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
      p_outputXEPHFile = x->p_outputXEPHFile;
      p_outputXEPHFilePath = x->p_outputXEPHFilePath;
      p_overwriteExistingFiles = x->p_overwriteExistingFiles;
      p_denseOutputToleranceFactor = x->p_denseOutputToleranceFactor;
      p_ephemerisToleranceFactor = x->p_ephemerisToleranceFactor;
   }
}

// ----------------------------------------------------------------------------

bool EphemerisGeneratorInstance::CanExecuteOn( const View&, String& whyNot ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool EphemerisGeneratorInstance::CanExecuteGlobal( String& whyNot ) const
{
   whyNot = "EphemerisGenerator can only be executed in the global context.";
   return true;
}

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
   Optional<double> B_V;
   Optional<double> D;
};

class IntegrationThread : public Thread
{
public:

   IntegrationThread( const EphemerisGeneratorInstance& instance
                    , const Ephemerides& eph
                    , const IntegrationObjectData& object
                    , XEPHGenerator* xeph )
      : m_instance( instance )
      , m_eph( eph )
      , m_object( object )
      , m_xeph( xeph )
   {
   }

   void Run() override
   {
      try
      {
         m_success = false;

         Console console;
         console.WriteLn(    "<end><cbr><br>Integrating orbital motion for " + m_object.objectId + ' ' + m_object.objectName + "<br>" );
         console.WriteLn(                  "Initial conditions:" );
         console.WriteLn( String().Format( "Epoch (TDB) ......... %.6f", m_object.epoch.JD() ) + " = " + m_object.epoch.ToString() );
         console.WriteLn( String().Format( "Position (au) ....... %+.16e %+.16e %+.16e<br>"
                                           "Velocity (au/day) ... %+.16e %+.16e %+.16e"
                                           , m_object.initialPosition[0], m_object.initialPosition[1], m_object.initialPosition[2]
                                           , m_object.initialVelocity[0], m_object.initialVelocity[1], m_object.initialVelocity[2] ) );

         TimePoint startTime( Min( m_instance.p_startTimeJD, m_instance.p_endTimeJD ) );
         TimePoint endTime( Max( m_instance.p_startTimeJD, m_instance.p_endTimeJD ) );
         if ( startTime > m_object.epoch )
            startTime = m_object.epoch;
         if ( endTime < m_object.epoch )
            endTime = m_object.epoch;

         StandardStatus status;
         StatusMonitor monitor;
         monitor.SetCallback( &status );
         monitor.Initialize( String().Format( "Performing numerical integration: %.6f -> %.6f", startTime.JD(), endTime.JD() )
                           , size_type( endTime - startTime ) );

         Integration integrator( m_eph, m_instance, m_object.objectId/*excludeId*/, m_object.objectName/*excludeName*/ );

         IntegrationDenseOutputData data;
         Vector r0, v0, r1, v1;
         bool rootThread = IsRootThread();
         double hacc = 0;

         for ( int forward = 0; forward < 2; ++forward )
         {
            TimePoint t1 = forward ? endTime : startTime;
            if ( t1 != m_object.epoch )
            {
               integrator( m_object.initialPosition, m_object.initialVelocity, m_object.epoch, t1,
                           [&]( size_type step, double h )
                           {
                              hacc += h;
                              if ( hacc > 1 )
                              {
                                 size_type d = size_type( hacc );
                                 hacc -= d;
                                 monitor += d;
                              }
                              Module->ProcessEvents();
                              if ( rootThread )
                                 if ( console.AbortRequested() )
                                    throw ProcessAborted();
                           } );

               if ( m_xeph != nullptr )
                  data << integrator.OutputData();

               Integration<>::State s = integrator.FinalState();
               Vector r( s[0], s[1], s[2] );
               Vector v( s[3], s[4], s[5] );
               if ( forward )
               {
                  r1 = r;
                  v1 = v;
               }
               else
               {
                  r0 = r;
                  v0 = v;
               }
            }
            else
            {
               if ( forward )
               {
                  r1 = m_object.initialPosition;
                  v1 = m_object.initialVelocity;
               }
               else
               {
                  r0 = m_object.initialPosition;
                  v0 = m_object.initialVelocity;
               }
            }
         }

         monitor.Complete();

         console.WriteLn(    "<end><cbr><br>Integration results:<br>" );
         console.WriteLn( String().Format( "Start time (TDB) .... %.6f", startTime.JD() ) + " = " + startTime.ToString() );
         console.WriteLn( String().Format( "Position (au) ....... %+.10e %+.10e %+.10e<br>"
                                           "Velocity (au/day) ... %+.10e %+.10e %+.10e<br>"
                                           , r0[0], r0[1], r0[2], v0[0], v0[1], v0[2] ) );
         console.WriteLn( String().Format( "End time (TDB) ...... %.6f", endTime.JD() ) + " = " + endTime.ToString() );
         console.WriteLn( String().Format( "Position (au) ....... %+.10e %+.10e %+.10e<br>"
                                           "Velocity (au/day) ... %+.10e %+.10e %+.10e"
                                           , r1[0], r1[1], r1[2], v1[0], v1[1], v1[2] ) );

         if ( m_xeph != nullptr )
         {
            data.Sort();
            m_xeph->AddObject( data, startTime, endTime
                             , m_instance.p_ephemerisToleranceFactor
                             , m_object.objectId, m_object.objectName
                             , m_object.H, m_object.G, m_object.B_V, m_object.D );
         }

         m_success = true;
         return;
      }
      catch ( ... )
      {
         if ( IsRootThread() )
            throw;

         String text = ConsoleOutputText();
         ClearConsoleOutputText();
         try
         {
            throw;
         }
         ERROR_HANDLER
         m_errorInfo = ConsoleOutputText();
         ClearConsoleOutputText();
         Console().Write( text );
      }
   }

   bool Succeeded() const
   {
      return m_success;
   }

   const IsoString& TextOutput() const
   {
      return m_text;
   }

   const String& ErrorInfo() const
   {
      return m_errorInfo;
   }

   IsoString ObjectIdAndName() const
   {
      if ( m_object.objectId.IsEmpty() )
         return m_object.objectName;
      return m_object.objectId + ' ' + m_object.objectName;
   }

private:

   const EphemerisGeneratorInstance& m_instance;
   const Ephemerides&                m_eph;
         IntegrationObjectData       m_object;
         XEPHGenerator*              m_xeph;
         IsoString                   m_text;
         String                      m_errorInfo;
         bool                        m_success = false;
};

typedef IndirectArray<IntegrationThread> thread_list;

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
   TimePoint startTime( p_startTimeJD );
   TimePoint endTime( p_endTimeJD );
   if ( endTime < startTime )
      Swap( startTime, endTime );
   if ( endTime - startTime < 5 )
      throw Error( IsoString().Format( "Too short integration time span: %.2f. At least 5 days are required.", endTime - startTime ) );

   Ephemerides eph( *this );

   AutoPointer<XEPHGenerator> xeph;
   if ( p_outputXEPHFile )
   {
      if ( p_outputXEPHFilePath.IsEmpty() )
         throw Error( "No output ephemerides file has been specified." );
      if ( File::ExtractSuffix( p_outputXEPHFilePath ) != ".xeph" )
         throw Error( "The output ephemerides file name must have the '.xeph' extension." );
      xeph = new XEPHGenerator;
   }

   Array<IntegrationObjectData> objects;

   switch ( p_workingMode )
   {
   default:
   case EGWorkingMode::StateVectors:
      {
         objects << IntegrationObjectData{ p_position, p_velocity
                                          , p_epochJD
                                          , p_objectId.ToIsoString(), p_objectName.ToIsoString()
                                          , p_H, p_G
                                          , p_B_V_defined ? Optional<double>( p_B_V ) : Optional<double>()
                                          , p_D_defined ? Optional<double>( p_D ) : Optional<double>() };
      }
      break;

   case EGWorkingMode::OrbitalElements:
      {
         Vector initialPosition, initialVelocity;
         StateVectorsFromOrbitalElements( initialPosition, initialVelocity, p_elements, p_epochJD, eph.GMS() );

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
                                          , p_objectId.ToIsoString(), p_objectName.ToIsoString()
                                          , p_H, p_G
                                          , p_B_V_defined ? Optional<double>( p_B_V ) : Optional<double>()
                                          , p_D_defined ? Optional<double>( p_D ) : Optional<double>() };
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
         Array<TextDatabase::ObjectData> objectData = database.Search( *this, 1000/*maxCount*/ );
         if ( objectData.IsEmpty() )
            throw Error( "EphemerisGenerator: No objects found." );

         for ( const TextDatabase::ObjectData& object : objectData )
         {
            Vector initialPosition, initialVelocity;
            if ( format.contents == TextDatabase::Contents_OrbitalElements )
            {
               StateVectorsFromOrbitalElements( initialPosition, initialVelocity, object.state, object.epochJD, eph.GMS() );

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
                                             , object.id, object.name
                                             , object.H, object.G
                                             , object.B_V, object.D };
         }
      }
      break;
   }

   Console console;
   console.EnableAbort();
   console.WriteLn( String().Format( "<end><cbr><br>Performing numerical integration: %u object%s."
                                    , objects.Length(), (objects.Length() > 1) ? "s" : "" ) );

   Array<size_type> pendingItems;
   for ( size_type i = 0; i < objects.Length(); ++i )
      pendingItems << i;

   int succeeded = 0;
   int failed = 0;

   int numberOfThreadsAvailable = Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 );

   IsoStringList textOutput;

   if ( pendingItems.Length() > 1 && numberOfThreadsAvailable > 1 )
   {
      int numberOfThreads = Min( numberOfThreadsAvailable, int( pendingItems.Length() ) );
      thread_list runningThreads( numberOfThreads ); // N.B.: all pointers are set to nullptr by IndirectArray's ctor.

      console.NoteLn( String().Format( "* Using %d worker threads.", numberOfThreads ) );

      try
      {
         /*
          * Thread watching loop.
          */
         for ( ;; )
         {
            try
            {
               int running = 0;
               for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
               {
                  Module->ProcessEvents();
                  if ( console.AbortRequested() )
                     throw ProcessAborted();

                  if ( *i != nullptr )
                  {
                     if ( !(*i)->Wait( 150 ) )
                     {
                        ++running;
                        continue;
                     }

                     /*
                      * A thread has just finished.
                      */
                     (*i)->FlushConsoleOutputText();
                     console.WriteLn();
                     String errorInfo;
                     if ( !(*i)->Succeeded() )
                        errorInfo = (*i)->ErrorInfo();

                     /*
                      * N.B.: IndirectArray<>::Delete() sets to zero the
                      * pointer pointed to by the iterator, but does not
                      * remove the array element.
                      */
                     runningThreads.Delete( i );

                     if ( !errorInfo.IsEmpty() )
                        throw Error( errorInfo );

                     ++succeeded;
                  }

                  /*
                   * If there are pending items, create a new thread and
                   * fire the next one.
                   */
                  if ( !pendingItems.IsEmpty() )
                  {
                     *i = new IntegrationThread( *this, eph, objects[*pendingItems], xeph.Ptr() );
                     pendingItems.Remove( pendingItems.Begin() );
                     size_type threadIndex = i - runningThreads.Begin();
                     console.NoteLn( String().Format( "<end><cbr>[%03u] ", threadIndex ) + (*i)->ObjectIdAndName() );
                     (*i)->Start( ThreadPriority::DefaultMax, threadIndex );
                     ++running;
                     if ( pendingItems.IsEmpty() )
                        console.NoteLn( "<br>* Waiting for running tasks to terminate...<br>" );
                     else if ( succeeded+failed > 0 )
                        console.WriteLn();
                  }
               }

               if ( !running )
                  break;
            }
            catch ( ProcessAborted& )
            {
               throw;
            }
            catch ( ... )
            {
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               ++failed;
               try
               {
                  throw;
               }
               ERROR_HANDLER
            }
         } // for
      }
      catch ( ... )
      {
         console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate..." );
         for ( IntegrationThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Abort();
         for ( IntegrationThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Wait();
         runningThreads.Destroy();
         throw;
      }
   }
   else // pendingItems.Length() == 1 || numberOfThreadsAvailable == 1
   {
      for ( size_type itemIndex : pendingItems )
      {
         try
         {
            console.WriteLn( "<end><cbr><br>" );
            IntegrationThread thread( *this, eph, objects[itemIndex], xeph.Ptr() );
            thread.Run();
            ++succeeded;
         }
         catch ( ProcessAborted& )
         {
            throw;
         }
         catch ( ... )
         {
            if ( console.AbortRequested() )
               throw ProcessAborted();

            ++failed;
            try
            {
               throw;
            }
            ERROR_HANDLER
         }
      }
   }

   Module->ProcessEvents();

   if ( succeeded == 0 )
      throw Error( "No object could be successfully integrated." );

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
      metadata.copyright = String().Format( "Copyright (C) %d, Pleiades Astrophoto S.L.", TimePoint::Now().Year() );

      xeph->Serialize( outputFilePath, metadata );
   }

   console.NoteLn( String().Format( "<end><cbr><br>===== EphemerisGenerator: %d succeeded, %d failed =====", succeeded, failed ) );
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
   if ( p == TheEGHParameter )
      return &p_H;
   if ( p == TheEGGParameter )
      return &p_G;
   if ( p == TheEGB_VDefinedParameter )
      return &p_B_V_defined;
   if ( p == TheEGB_VParameter )
      return &p_B_V;
   if ( p == TheEGDDefinedParameter )
      return &p_D_defined;
   if ( p == TheEGDParameter )
      return &p_D;
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
   if ( p == TheEGOutputXEPHFileParameter )
      return &p_outputXEPHFile;
   if ( p == TheEGOutputXEPHFilePathParameter )
      return p_outputXEPHFilePath.Begin();
   if ( p == TheEGOverwriteExistingFilesParameter )
      return &p_overwriteExistingFiles;
   if ( p == TheEGDenseOutputToleranceFactorParameter )
      return &p_denseOutputToleranceFactor;
   if ( p == TheEGEphemerisToleranceFactorParameter )
      return &p_ephemerisToleranceFactor;

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
// EOF EphemerisGeneratorInstance.cpp - Released 2021-04-09T19:41:48Z
