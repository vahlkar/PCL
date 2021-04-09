//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// RejectionMapGenerationEngine.cpp - Released 2021-04-09T19:41:48Z
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
#include "RejectionMapGenerationEngine.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RejectionMapGenerationEngine::RejectionMapGenerationEngine( const ImageIntegrationInstance& instance, StatusMonitor& monitor,
                                                            Image& map, bool high, int channel )
   : IntegrationEngineBase( instance, monitor )
   , m_map( map )
   , m_high( high )
   , m_channel( channel )
{
   Array<size_type> L = Thread::OptimalThreadLoads( m_map.Height() );
   for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
      m_threads << new GenerationThread( *this, n, n + int( L[i] ) );
}

// ----------------------------------------------------------------------------

RejectionMapGenerationEngine::~RejectionMapGenerationEngine()
{
   m_threads.Destroy();
}

// ----------------------------------------------------------------------------

void RejectionMapGenerationEngine::Generate()
{
   if ( !m_threads.IsEmpty() )
   {
      m_threadData.total = size_type( m_map.Height() );
      m_threadData.count = 0;
      AbstractImage::RunThreads( m_threads, m_threadData );
      m_monitor = m_threadData.status;
   }
}

// ----------------------------------------------------------------------------

#define E   m_engine
#define I   m_engine.m_instance

// ----------------------------------------------------------------------------

void RejectionMapGenerationEngine::GenerationThread::Run()
{
   INIT_THREAD_MONITOR()

   uint8 inputMask = E.m_high ? 0x11 : 0x22;
   if ( I.p_mapRangeRejection )
      inputMask |= E.m_high ? 0x04 : 0x08;

   Rect rect( 0, m_firstStack, IntegrationFile::Width(), m_endStack );

   GenericVector<bool> v( IntegrationFile::NumberOfFiles() );
   GenericVector<UInt8Image::const_roi_sample_iterator> r( IntegrationFile::NumberOfFiles() );
   for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
   {
      // N.B. A rejection map can be empty if no pixel has been rejected.
      const UInt8Image& map = IntegrationFile::FileByIndex( i ).RejectionMap();
      if ( (v[i] = map) != false )
         r[i] = UInt8Image::const_roi_sample_iterator( map, rect, E.m_channel );
   }

   for ( Image::roi_sample_iterator m( E.m_map, rect, E.m_channel ); m; ++m )
   {
      int n = 0;
      for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
         if ( v[i] )
            if ( *r[i]++ & inputMask )
               ++n;
      *m = float( n )/IntegrationFile::NumberOfFiles();

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

#undef E
#undef I

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RejectionMapGenerationEngine.cpp - Released 2021-04-09T19:41:48Z
