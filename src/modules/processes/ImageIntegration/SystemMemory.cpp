//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.1.16
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.18.0
// ----------------------------------------------------------------------------
// SystemMemory.cpp - Released 2019-09-29T12:27:57Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "SystemMemory.h"

#ifdef __PCL_WINDOWS
# include <windows.h>
#else
# include <unistd.h>
#endif

#if defined( __PCL_MACOSX ) || defined( __PCL_FREEBSD )
# include <sys/types.h>
# include <sys/param.h>
# include <sys/sysctl.h>
#endif

#ifdef __PCL_MACOSX
# include <mach/mach_host.h>
# include <mach/mach_port.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

size_type AvailablePhysicalMemory()
{
#ifdef __PCL_FREEBSD

   int mib[ 2 ] = { CTL_HW, HW_USERMEM };
   unsigned long long userMemBytes;
   size_t size = sizeof( userMemBytes );
   if ( sysctl( mib, 2, &userMemBytes, &size, 0, 0 ) == 0 )
      return userMemBytes;
   return 0;

#endif

#ifdef __PCL_LINUX

   return size_type( sysconf( _SC_AVPHYS_PAGES ) ) * size_type( sysconf( _SC_PAGESIZE ) );

#endif

#ifdef __PCL_MACOSX

   mach_port_t host = mach_host_self();
   kern_return_t kret;
# ifdef HOST_VM_INFO64
   struct vm_statistics64 vm_stat;
   natural_t count = HOST_VM_INFO64_COUNT;
   kret = host_statistics64( host, HOST_VM_INFO64, (host_info64_t)&vm_stat, &count );
# else
   struct vm_statistics	vm_stat;
   natural_t count = HOST_VM_INFO_COUNT;
   kret = host_statistics( host, HOST_VM_INFO, (host_info_t)&vm_stat, &count );
# endif
   size_type totalFreeMemBytes = 0;
   if ( kret == KERN_SUCCESS )
      totalFreeMemBytes = size_type( vm_stat.free_count + vm_stat.active_count + vm_stat.inactive_count )
                        * size_type( sysconf( _SC_PAGESIZE ) );
   mach_port_deallocate( mach_task_self(), host );
   return totalFreeMemBytes;

#endif

#ifdef __PCL_WINDOWS

   MEMORYSTATUSEX m;
   m.dwLength = sizeof( m );
   GlobalMemoryStatusEx( &m );
   return size_type( m.ullAvailPhys );

#endif
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SystemMemory.cpp - Released 2019-09-29T12:27:57Z
