//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/ElapsedTime.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_ElapsedTime_h
#define __PCL_ElapsedTime_h

/// \file pcl/ElapsedTime.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/String.h>

#if !defined( __PCL_LINUX ) && !defined( __PCL_FREEBSD )
#  include <pcl/Atomic.h>
#  include <pcl/AutoLock.h>
#  define PCL_CLOCK_ID 0
#endif

#ifdef __PCL_LINUX
#  include <time.h>
#  ifdef CLOCK_MONOTONIC_RAW // since kernel 2.6.28
#    define PCL_CLOCK_ID CLOCK_MONOTONIC_RAW
#  else
#    define PCL_CLOCK_ID CLOCK_MONOTONIC
#  endif
#endif

#ifdef __PCL_FREEBSD
#  include <sys/timespec.h>
#  ifdef CLOCK_MONOTONIC_PRECISE // since FreeBSD 10.1
#    define PCL_CLOCK_ID CLOCK_MONOTONIC_PRECISE
#  else
#    define PCL_CLOCK_ID CLOCK_MONOTONIC
#  endif
#endif

#ifdef __PCL_MACOSX
#  include <mach/mach.h>
#  include <mach/mach_time.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ElapsedTime
 * \brief High-resolution time stamp.
 *
 * %ElapsedTime is a platform-independent, monotonically nondecreasing
 * high-resolution time stamp based on hardware time resources that are
 * independent on any external time reference. The underlying implementation
 * uses monotonic clock services on Linux and FreeBSD (via clock_gettime()
 * system calls), performance counters on Windows, and Mach absolute time
 * system services on macOS.
 *
 * Example of use:
 *
 * \code
 * ElapsedTime T;
 * for ( int i = 0; i < 100000; ++i )
 *    foo();
 * double t1 = T();
 * T.Reset();
 * for ( int i = 0; i < 100000; ++i )
 *    bar();
 * double t2 = T();
 * Console out;
 * out.WriteLn( "Execution times (100K function calls): );
 * out.WriteLn( "foo(): " + ElapsedTime::ToString( t1 ) );
 * out.WriteLn( "bar(): " + ElapsedTime::ToString( t2 ) );
 * \endcode
 *
 * Timing resolution should be better than one microsecond on all platforms,
 * but the actual resolution achieved is hardware and system dependent.
 *
 * This class is thread-safe: it can be instantiated and its member functions
 * can be safely invoked from multiple running threads.
 */
class PCL_CLASS ElapsedTime
{
public:

   /*!
    * Constructs a new %ElapsedTime object and initializes it with the current
    * system time, which will be the starting point of subsequent time interval
    * evaluations.
    */
   ElapsedTime()
   {
      Reset();
   }

   /*!
    * Copy constructor.
    */
   ElapsedTime( const ElapsedTime& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   ElapsedTime& operator =( const ElapsedTime& ) = default;

   /*!
    * Returns the time interval elapsed since the last object initialization,
    * expressed in seconds. An %ElapsedTime object is initialized either when
    * it is constructed, or through a subsequent call to the Reset() member
    * function.
    */
   double operator ()() const
   {
      return TimeStamp() - m_start;
   }

   /*!
    * Initializes this %ElapsedTime object with a new time stamp, which will be
    * the starting point of subsequent time interval evaluations.
    */
   void Reset()
   {
      m_start = TimeStamp();
   }

   /*!
    * Returns an 8-bit string representation of the time interval elapsed since
    * the last object initialization. See operator()() and
    * ToIsoString( double ) for detailed information.
    */
   IsoString ToIsoString() const
   {
      return ToIsoString( operator()() );
   }

   /*!
    * Returns a Unicode string representation of the time interval elapsed
    * since the last object initialization. See operator()() and
    * ToIsoString( double ) for detailed information.
    */
   String ToString() const
   {
      return ToString( operator()() );
   }

   /*!
    * IsoString conversion operator. Equivalent to ToIsoString().
    */
   operator IsoString() const
   {
      return ToIsoString();
   }

   /*!
    * String conversion operator. Equivalent to ToString().
    */
   operator String() const
   {
      return ToString();
   }

   /*!
    * Returns a 64-bit, monotonically nondecreasing time stamp.
    *
    * The returned value is the time elapsed in seconds since an arbitrary,
    * platform-dependent starting time that will remain invariant during the
    * current execution of the calling module.
    *
    * With the current implementation, accuracy of timestamps should be better
    * than one microsecond on all supported platforms.
    */
   static double TimeStamp()
   {
#if defined( __PCL_LINUX ) || defined( __PCL_FREEBSD )

      /*
       * Linux/FreeBSD implementation based on clock_gettime().
       */
      timespec ts;
      (void)clock_gettime( PCL_CLOCK_ID, &ts );
      return ts.tv_sec + ts.tv_nsec*1.0e-09;

#endif   // __PCL_LINUX || __PCL_FREEBSD

#ifdef __PCL_MACOSX

      /*
       * macOS implementation based on Mach absolute time system services.
       *
       * https://developer.apple.com/library/mac/qa/qa1398/_index.html
       * http://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x
       */
      static Mutex     mutex;
      static AtomicInt initialized;
      static double    absoluteToSeconds;
      static uint64    offset;

      if ( !initialized )
      {
         volatile AutoLock lock( mutex );
         if ( initialized.Load() == 0 )
         {
            mach_timebase_info_data_t timeBase = { 0 };
            mach_timebase_info( &timeBase );
            absoluteToSeconds = 1.0e-09*(double( timeBase.numer )/timeBase.denom);
            offset = mach_absolute_time();
            initialized.Store( 1 );
         }
      }

      uint64 t = mach_absolute_time();
      return (t - offset)*absoluteToSeconds;

#endif   // __PCL_MACOSX

#ifdef __PCL_WINDOWS

      /*
       * Windows implementation based on Win32 performance counters.
       *
       * https://msdn.microsoft.com/en-us/library/windows/desktop/ms644905%28v=vs.85%29.aspx
       * https://msdn.microsoft.com/en-us/library/windows/desktop/ms644904%28v=vs.85%29.aspx
       * http://stackoverflow.com/questions/5404277/porting-clock-gettime-to-windows
       * http://stackoverflow.com/questions/1676036/what-should-i-use-to-replace-gettimeofday-on-windows
       */
      static Mutex     mutex;
      static AtomicInt initialized;
      static double    countsToSeconds;
      static uint64    offset;

      if ( !initialized )
      {
         volatile AutoLock lock( mutex );
         if ( initialized.Load() == 0 )
         {
            uint64 performanceFrequency;
            QueryPerformanceFrequency( (LARGE_INTEGER*)&performanceFrequency );
            countsToSeconds = 1.0/performanceFrequency;
            QueryPerformanceCounter( (LARGE_INTEGER*)&offset );
            initialized.Store( 1 );
         }
      }

      uint64 t;
      QueryPerformanceCounter( (LARGE_INTEGER*)&t );
      return (t - offset)*countsToSeconds;

#endif   // __PCL_WINDOWS
   }

   /*!
    * Returns an 8-bit string representation of the specified time interval
    * in seconds.
    *
    * The returned string has one of the following formats:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>(1) u.uuu us</td>   <td>u.uuu is the time t in microseconds if t &le; 0.001 s</td></tr>
    * <tr><td>(2) m.mmm ms</td>   <td>m.mmm is the time t in milliseconds if t &le; 1.0 s</td></tr>
    * <tr><td>(3) s.sss s</td>    <td>s.sss is the time t in seconds if t &le; 60.0 s</td></tr>
    * <tr><td>(4) mm:ss.ss</td>   <td>Zero-padded minutes and seconds if t &le; 3600 s</td></tr>
    * <tr><td>(5) hh:mm:ss.s</td> <td>Same as (4) with zero-padded hours if t &le; 86400 s</td></tr>
    * <tr><td>(6) d:hh:mm:ss</td> <td>Same as (5) with elapsed days if t &ge; 86400</td></tr>
    * </table>
    *
    * If \a width > 0 and t &le; 60, the integer part of the represented
    * microseconds, milliseconds or seconds will be right-padded in a field of
    * \a width space characters.
    *
    * The \a precision argument specifies the maximum number of decimal digits
    * represented, and must be in the [0,3] range. Depending on the magnitude
    * of the specified \a seconds argument, the precision will be reduced by
    * one or two units automatically, with a minimum of zero digits. The
    * default precision is 3 digits.
    *
    * The output format is chosen automatically to generate the most
    * significant representation. If the specified interval is negative (toward
    * the past), a minus sign is prepended to the returned string.
    */
   static IsoString ToIsoString( double seconds, int width = 0, int precision = 3 )
   {
      return ToString( seconds, width, precision, (IsoString*)0 );
   }

   /*!
    * Returns a Unicode string representation of the specified time interval
    * \a s in seconds. See ToIsoString( double ) for information on the
    * representation format and parameters.
    */
   static String ToString( double s, int width = 0, int precision = 3 )
   {
      return ToString( s, width, precision, (String*)0 );
   }

   /*!
    * Returns the time interval in seconds elapsed between the starting points
    * of two %ElapsedTime objects \a t1 and \a t2. The result is positive if
    * \a t2 precedes \a t1, negative if \a t1 precedes \a t2, zero if and only
    * if either \a t1 and \a t2 are references to the same instance, or if one
    * of the objects has been constructed as a copy of the other.
    */
   friend double operator -( const ElapsedTime& t1, const ElapsedTime& t2 )
   {
      return t1.m_start - t2.m_start;
   }

private:

   double m_start; // timestamp in seconds

   template <class S>
   static S ToString( double s, int w, int p, S* )
   {
      w = Max( 0, w );
      p = Range( p, 0, 3 );

      if ( s < 60 )
      {
         if ( w > 0 )
            if ( p > 0 )
               w = Max( p+2, w+p+1 );

         const char* u;
         if ( s < 0.001 )
         {
            s *= 1000000;
            u = "us";
         }
         else if ( s < 1 )
         {
            s *= 1000;
            u = "ms";
         }
         else
            u = "s";

         if ( w > 0 )
            return S().Format( "%*.*f %s", w, p, s, u );
         return S().Format( "%.*f %s", p, s, u );
      }

      int sign = (s < 0) ? -1 : +1;
      s = Abs( s );

      if ( s < 3600 )
      {
         int m = TruncInt( s/60 );
         s -= m*60;
         p = Max( 0, p-1 );
         return S().Format( "%02d:%0*.*f", m*sign, (p > 0) ? 3+p : 2, p, s );
      }

      if ( s < 86400 )
      {
         int h = TruncInt( s/3600 );
         s -= h*3600;
         int m = TruncInt( s/60 );
         s -= m*60;
         p = Max( 0, p-2 );
         return S().Format( "%02d:%02d:%0*.*f", h*sign, m, (p > 0) ? 3+p : 2, p, s );
      }

      int d = TruncInt( s/86400 );
      s -= d*86400;
      int h = TruncInt( s/3600 );
      s -= h*3600;
      int m = TruncInt( s/60 );
      s -= m*60;
      return S().Format( "%d:%02d:%02d:%02d", d*sign, h, m, RoundInt( s ) );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ElapsedTime_h

// ----------------------------------------------------------------------------
// EOF pcl/ElapsedTime.h - Released 2024-12-28T16:53:48Z
