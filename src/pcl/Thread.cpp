//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Thread.cpp - Released 2024-12-28T16:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Atomic.h>
#include <pcl/AutoLock.h>
#include <pcl/Console.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Math.h>
#include <pcl/Thread.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

#ifdef __PCL_UNIX
# include <time.h> // for nanosleep()
#endif

#ifdef __PCL_LINUX
# include <sched.h>
# include <errno.h>
#endif

#ifdef __PCL_WINDOWS
# define SLEEP( ms )                      \
   ::Sleep( DWORD( ms ) );
#else
# define SLEEP( ms )                      \
   {                                      \
      struct timespec ts;                 \
      if ( ms < 1000 )                    \
      {                                   \
         ts.tv_sec = 0;                   \
         ts.tv_nsec = ms*1000000;         \
      }                                   \
      else                                \
      {                                   \
         ts.tv_sec = ms/1000;             \
         ts.tv_nsec = (ms%1000)*1000000;  \
      }                                   \
      ::nanosleep( &ts, 0 );              \
   }
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

static bool      s_enableAffinity = false;
static AtomicInt s_featureDataInitialized;
static AtomicInt s_numberOfRunningThreads;

// ----------------------------------------------------------------------------

#define T   reinterpret_cast<Thread*>( hThread )

class ThreadDispatcher
{
private:

   struct AutoCounter
   {
      AutoCounter()
      {
         s_numberOfRunningThreads.Increment();
      }

      ~AutoCounter()
      {
         s_numberOfRunningThreads.Decrement();
      }
   };

public:

   static void api_func RunThread( thread_handle hThread )
   {
      try
      {
         if ( !T->IsStealth() )
         {
            volatile AutoCounter counter;

            if ( s_enableAffinity )
               if ( T->m_processorIndex >= 0 )
                  T->SetAffinity( T->m_processorIndex );

            T->Run();
         }
         else
            T->Run();
      }
      catch ( ... )
      {
         /*
          * ### Do _not_ propagate exceptions from a running thread. Never.
          */
      }
   }
}; // ThreadDispatcher

#undef T

// ----------------------------------------------------------------------------

/*
 * ### TODO: Implement pcl::Thread without core application support, in case
 * we are running as an independent application.
 *
 * For now, we simply construct a null object when API = nullptr. The
 * reimplemented Thread::Run() can be invoked single-threaded.
 */

Thread::Thread()
   : UIObject( (API != nullptr) ? (*API->Thread->CreateThread)( ModuleHandle(), this, 0/*flags*/ ) : nullptr )
{
   if ( API != nullptr )
   {
      if ( IsNull() )
         throw APIFunctionError( "CreateThread" );

      (*API->Thread->SetThreadExecRoutine)( handle, ThreadDispatcher::RunThread );

      if ( s_featureDataInitialized.Load() == 0 )
      {
         /*
          * ### TODO: Add a new StartThreadEx() API function to allow
          * specifying a logical processor index and other flags. In this way
          * this wouldn't be necessary and thread scheduling and affinity would
          * be controlled completely by the core application.
          */
         static Mutex mutex;
         volatile AutoLock lock( mutex );
         if ( s_featureDataInitialized.Load() == 0 )
         {
            s_enableAffinity = PixInsightSettings::GlobalFlag( "Process/EnableThreadCPUAffinity" );
            s_featureDataInitialized.Store( 1 );
         }
      }
   }
}

// ----------------------------------------------------------------------------

Thread& Thread::Null()
{
   static Thread* nullThread = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullThread == nullptr )
      nullThread = new Thread( nullptr );
   return *nullThread;
}

// ----------------------------------------------------------------------------

void Thread::Start( Thread::priority p, int processor )
{
   m_processorIndex = -1;
   if ( processor >= 0 )
      if ( processor <= PCL_MAX_PROCESSORS )
         if ( Thread::IsRootThread() )
            m_processorIndex = processor;
   (*API->Thread->StartThread)( handle, p );
}

// ----------------------------------------------------------------------------

Array<int> Thread::Affinity() const
{
   Array<int> processors;
#ifdef __PCL_LINUX
   cpu_set_t set;
   if ( sched_getaffinity( 0, sizeof( cpu_set_t ), &set ) >= 0 )
      for ( int i = 0; i < CPU_SETSIZE; ++i )
         if ( CPU_ISSET( i, &set ) )
            processors.Add( i );
#endif
#ifdef __PCL_FREEBSD
   // ### TODO
#endif
#ifdef __PCL_MACOSX
   // ### TODO
#endif
#ifdef __PCL_WINDOWS
   // ### TODO - How can we know the affinity mask of a thread on Windows ???
   //            There is no GetThreadAffinityMask() !
#endif
   return processors;
}

// ----------------------------------------------------------------------------

bool Thread::SetAffinity( const Array<int>& processors )
{
   if ( (*API->Thread->IsThreadActive)( handle ) == api_false )
      return false;
#ifdef __PCL_LINUX
   cpu_set_t set;
   CPU_ZERO( &set );
   for ( int p : processors )
   {
      if ( p < 0 || p >= CPU_SETSIZE )
         return false;
      CPU_SET( p, &set );
   }
   return sched_setaffinity( 0, sizeof( cpu_set_t ), &set ) >= 0;
#endif
#ifdef __PCL_FREEBSD
   // ### TODO
#endif
#ifdef __PCL_MACOSX
   // ### TODO
#endif
#ifdef __PCL_WINDOWS
   DWORD_PTR mask = 0;
   for ( int p : processors )
   {
      if ( p < 0 || p >= sizeof( DWORD_PTR )<<3 )
         return false;
      mask |= DWORD_PTR( 1u ) << p;
   }
   return SetThreadAffinityMask( GetCurrentThread(), mask ) != 0;
#endif
   return false;
}

// ----------------------------------------------------------------------------

bool Thread::SetAffinity( int processor )
{
   if ( (*API->Thread->IsThreadActive)( handle ) == api_false )
      return false;
#ifdef __PCL_LINUX
   if ( processor < 0 || processor >= CPU_SETSIZE )
      return false;
   cpu_set_t set;
   CPU_ZERO( &set );
   CPU_SET( processor, &set );
   return sched_setaffinity( 0, sizeof( cpu_set_t ), &set ) >= 0;
#endif
#ifdef __PCL_FREEBSD
   // ### TODO
#endif
#ifdef __PCL_MACOSX
   // ### TODO
#endif
#ifdef __PCL_WINDOWS
   if ( processor < 0 || processor >= sizeof( DWORD_PTR )<<3 )
      return false;
   DWORD_PTR mask = uint64( 1u ) << processor;
   return SetThreadAffinityMask( GetCurrentThread(), mask ) != 0;
#endif
   return false;
}

// ----------------------------------------------------------------------------

void Thread::Kill()
{
   (*API->Thread->KillThread)( handle );
}

// ----------------------------------------------------------------------------

bool Thread::IsActive() const
{
   return (*API->Thread->IsThreadActive)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

Thread::priority Thread::Priority() const
{
   return priority( (*API->Thread->GetThreadPriority)( handle ) );
}

// ----------------------------------------------------------------------------

void Thread::SetPriority( Thread::priority p )
{
   (*API->Thread->SetThreadPriority)( handle, p );
}

// ----------------------------------------------------------------------------

void Thread::Wait()
{
   (void)(*API->Thread->WaitThread)( handle, uint32_max );
}

// ----------------------------------------------------------------------------

bool Thread::Wait( unsigned ms )
{
   return (*API->Thread->WaitThread)( handle, ms ) != api_false;
}

// ----------------------------------------------------------------------------

void Thread::Sleep( unsigned ms )
{
   //(*API->Thread->SleepThread)( handle, ms );
   SLEEP( ms )
}

// ----------------------------------------------------------------------------

/*
Thread& Thread::CurrentThread()
{
   thread_handle handle = (*API->Thread->GetCurrentThread)();
   if ( handle != 0 )
      if ( (*API->UI->GetUIObjectModule)( handle ) == ModuleHandle() )
      {
         UIObject& object = UIObject::ObjectByHandle( handle );
         if ( !object.IsNull() )
         {
            Thread* thread = dynamic_cast<Thread*>( &object );
            if ( thread != 0 )
               return *thread;
         }
      }
   return Null();
}
*/

// ----------------------------------------------------------------------------

bool Thread::IsRootThread()
{
   return (*API->Thread->GetCurrentThread)() == 0;
}

// ----------------------------------------------------------------------------

int Thread::NumberOfRunningThreads()
{
   return s_numberOfRunningThreads.Load();
}

// ----------------------------------------------------------------------------

uint32 Thread::Status() const
{
   return (*API->Thread->GetThreadStatus)( handle );
}

// ----------------------------------------------------------------------------

bool Thread::TryGetStatus( uint32& status ) const
{
   return (*API->Thread->GetThreadStatusEx)( handle, &status, 0x00000001 ) != api_false;
}

// ----------------------------------------------------------------------------

void Thread::SetStatus( uint32 status )
{
   (*API->Thread->SetThreadStatus)( handle, status );
}

// ----------------------------------------------------------------------------

String Thread::ConsoleOutputText() const
{
   size_type len = 0;
   (*API->Thread->GetThreadConsoleOutputText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->Thread->GetThreadConsoleOutputText)( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetThreadConsoleOutputText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void Thread::ClearConsoleOutputText()
{
   (*API->Thread->ClearThreadConsoleOutputText)( handle );
}

// ----------------------------------------------------------------------------

void Thread::FlushConsoleOutputText()
{
   if ( IsRootThread() )
   {
      Console().Write( "<end><cbr>" + ConsoleOutputText() );
      ClearConsoleOutputText();
   }
}

// ----------------------------------------------------------------------------

void* Thread::CloneHandle() const
{
   throw Error( "Cannot clone a Thread handle" );
}

// ----------------------------------------------------------------------------

int Thread::NumberOfThreads( size_type N, size_type overheadLimit )
{
   if ( N > overheadLimit )
      if ( API != nullptr )
      {
         static AtomicInt numberOfProcessors;
         int processorsAvailable = numberOfProcessors.Load();
         if ( processorsAvailable == 0 )
         {
            static Mutex mutex;
            volatile AutoLock lock( mutex );
            processorsAvailable = numberOfProcessors.Load();
            if ( processorsAvailable == 0 )
            {
               processorsAvailable = Max( 1, PixInsightSettings::GlobalInteger( "System/NumberOfProcessors" ) );
               numberOfProcessors.Store( processorsAvailable );
            }
         }

         processorsAvailable -= NumberOfRunningThreads();
         if ( processorsAvailable > 1 )
         {
            int threadsAvailable = Min( processorsAvailable, (*API->Global->MaxProcessorsAllowedForModule)( ModuleHandle(), 0u/*flags*/ ) );
            if ( overheadLimit < 2 || N/threadsAvailable >= overheadLimit )
               return threadsAvailable;
            return Max( 1, int( N/overheadLimit ) );
         }

         /*
          * Equivalent, but slower, code based on platform settings.
          */
//          if ( processorsAvailable > 1 )
//             if ( PixInsightSettings::GlobalFlag( "Process/EnableParallelModuleProcessing" ) )
//                if ( PixInsightSettings::GlobalFlag( "Process/EnableParallelProcessing" ) )
//                {
//                   int threadsAvailable = Min( processorsAvailable, PixInsightSettings::GlobalInteger( "Process/MaxProcessors" ) );
//                   if ( overheadLimit < 2 || N/threadsAvailable >= overheadLimit )
//                      return threadsAvailable;
//                   return Max( 1, int( N/overheadLimit ) );
//                }
      }

   return 1;
}

// ----------------------------------------------------------------------------

Array<size_type> Thread::OptimalThreadLoads( size_type N, size_type overheadLimit, int maxThreads )
{
   size_type numberOfThreads = Min( NumberOfThreads( N, overheadLimit ), Max( 1, maxThreads ) );
   size_type itemsPerThread = N/numberOfThreads;
   size_type remainderItems = N - itemsPerThread*numberOfThreads;
   Array<size_type> L( numberOfThreads, itemsPerThread );
   for ( size_type i = 0; remainderItems > 0; ++i, --remainderItems )
      ++L[i];
   return L;
}

// ----------------------------------------------------------------------------

Array<size_type> Thread::OptimalThreadLoadsAligned( size_type N, int align, size_type overheadLimit, int maxThreads )
{
   Array<size_type> L = OptimalThreadLoads( N, Max( size_type( align ), overheadLimit ), maxThreads );
   for ( size_type i = 0, j = 1; j < L.Length(); ++i, ++j )
      while ( L[i] % align )
      {
         if ( L[j] == 0 )
            if ( ++j == L.Length() )
               break;
         --L[j];
         ++L[i];
      }

   Array<size_type> A;
   for ( size_type n : L )
      if ( n > 0 )
         A << n;
   return A;
}

// ----------------------------------------------------------------------------

void PCL_FUNC Sleep( unsigned ms )
{
   //(*API->Thread->SleepThread)( 0, ms );
   SLEEP( ms )
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Thread.cpp - Released 2024-12-28T16:53:56Z
