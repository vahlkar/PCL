//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// LargeScaleRejectionMapGenerationEngine.cpp - Released 2021-04-09T19:41:48Z
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

#include "LargeScaleRejectionMapGenerationEngine.h"

#include <pcl/MetaModule.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/MultiscaleMedianTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

LargeScaleRejectionMapGenerationEngine::LargeScaleRejectionMapGenerationEngine( const ImageIntegrationInstance& instance, StatusMonitor& monitor,
                                                                                GenericVector<size_type>& N, bool high, int channel )
   : IntegrationEngineBase( instance, monitor )
   , m_high( high )
   , m_channel( channel )
   , m_N( N )
{
   size_type availableMemory = size_type( double( m_instance.p_autoMemoryLimit ) * Module->AvailablePhysicalMemory() );
   size_type layersPerThread = Max( 1, m_high ? m_instance.p_largeScaleClipHighProtectedLayers :
                                                m_instance.p_largeScaleClipLowProtectedLayers );
   size_type bytesPerThread = IntegrationFile::NumberOfPixels() * (1 + sizeof( float )*(1 + layersPerThread));
   int maxThreads = availableMemory / bytesPerThread;
   Array<size_type> L = Thread::OptimalThreadLoads( IntegrationFile::NumberOfFiles(), 1u/*overheadLimit*/, maxThreads );
   for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
      m_threads << new GenerationThread( *this, n, n + int( L[i] ) );
}

// ----------------------------------------------------------------------------

LargeScaleRejectionMapGenerationEngine::~LargeScaleRejectionMapGenerationEngine()
{
   m_threads.Destroy();
}

// ----------------------------------------------------------------------------

void LargeScaleRejectionMapGenerationEngine::Generate()
{
   if ( !m_threads.IsEmpty() )
   {
      m_threadData.total = size_type( 4*IntegrationFile::NumberOfFiles() );
      m_threadData.count = 0;
      AbstractImage::RunThreads( m_threads, m_threadData );
      m_monitor = m_threadData.status;
      for ( const GenerationThread& thread : m_threads )
         m_N += thread.N;
   }
}

// ----------------------------------------------------------------------------

#define E   m_engine
#define I   m_engine.m_instance

// ----------------------------------------------------------------------------

void LargeScaleRejectionMapGenerationEngine::GenerationThread::Run()
{
   INIT_THREAD_MONITOR()

   int protectedLayers = E.m_high ? I.p_largeScaleClipHighProtectedLayers : I.p_largeScaleClipLowProtectedLayers;
   int growth = E.m_high ? I.p_largeScaleClipHighGrowth : I.p_largeScaleClipLowGrowth;
   uint8 inputMask = E.m_high ? 0x01 : 0x02;
   uint8 outputMask = E.m_high ? 0x10 : 0x20;

   for ( int i = m_firstStack; i < m_endStack; ++i )
   {
      // N.B. A rejection map can be empty if no pixel has been rejected.
      UInt8Image& rmap = IntegrationFile::FileByIndex( i ).RejectionMap();
      if ( rmap )
      {
         UInt8Image map;
         map.AllocateData( IntegrationFile::Width(), IntegrationFile::Height() ).Zero();
         {
            UInt8Image::sample_iterator m( map );
            for ( UInt8Image::const_sample_iterator r( rmap, E.m_channel ); r; ++r, ++m )
               if ( *r & inputMask )
                  *m = 0xff;
         }

         UPDATE_THREAD_MONITOR( 1 )

         {
            MultiscaleMedianTransform T( Max( 1, protectedLayers ) );
            for ( int j = 0; j < T.NumberOfLayers(); ++j )
               T.DisableLayer( j );
            T << map;
            T >> map;
         }

         UPDATE_THREAD_MONITOR( 1 )

         {
            DilationFilter D;
            CircularStructure C( (growth << 1) + 1 );
            MorphologicalTransformation M( D, C );
            M >> map;
         }

         UPDATE_THREAD_MONITOR( 1 )

         {
            UInt8Image::const_sample_iterator m( map );
            for ( UInt8Image::sample_iterator r( rmap, E.m_channel ); r; ++r, ++m )
               if ( *m )
                  if ( (*r |= outputMask) & 0x03 == 0 )
                     ++N[i];
         }

         UPDATE_THREAD_MONITOR( 1 )
      }
   }
}

// ----------------------------------------------------------------------------

#undef E
#undef I
#undef P

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LargeScaleRejectionMapGenerationEngine.cpp - Released 2021-04-09T19:41:48Z
