//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// IntegrationDataLoaderEngine.cpp - Released 2021-04-09T19:41:48Z
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

#include "IntegrationDataLoaderEngine.h"
#include "IntegrationFile.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IntegrationDataLoaderEngine::IntegrationDataLoaderEngine( const ImageIntegrationInstance& instance_, StatusMonitor& monitor_,
                                                          int startingRow,
                                                          RejectionStacks& stacks, RejectionCounts& counts , RejectionSlopes& slopes )
   : IntegrationEngineBase( instance_, monitor_ )
   , m_r( startingRow )
   , m_R( stacks )
   , m_N( counts )
   , m_M( slopes )
{
   Array<size_type> L = Thread::OptimalThreadLoads( m_R.Length() );
   for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
      m_threads << new DataLoaderThread( *this, n, n + int( L[i] ) );
}

// ----------------------------------------------------------------------------

IntegrationDataLoaderEngine::~IntegrationDataLoaderEngine()
{
   m_threads.Destroy();
}

// ----------------------------------------------------------------------------

void IntegrationDataLoaderEngine::LoadData()
{
   if ( !m_threads.IsEmpty() )
   {
      m_threadData.total = m_R.Length();
      m_threadData.count = 0;
      AbstractImage::RunThreads( m_threads, m_threadData );
      m_monitor = m_threadData.status;
   }
}

// ----------------------------------------------------------------------------

#define E   m_engine
#define I   m_engine.m_instance

// ----------------------------------------------------------------------------

void IntegrationDataLoaderEngine::DataLoaderThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.m_R.ComponentPtr( m_firstStack );
   IVector* N = E.m_N.ComponentPtr( m_firstStack );
   FVector* M = (I.p_rejection == IIRejection::LinearFit) ? E.m_M.ComponentPtr( m_firstStack ) : nullptr;

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      // Source pixels
      *R = RejectionMatrix( IntegrationFile::Width(), IntegrationFile::NumberOfFiles() );
      for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
      {
         const float* b = IntegrationFile::FileByIndex( i )[E.m_r+k];
         for ( int x = 0; x < IntegrationFile::Width(); ++x )
            R->RowPtr( x )[i].Set( *b++, i );
      }

      // Pixel counters
      *N = IVector( IntegrationFile::NumberOfFiles(), IntegrationFile::Width() );

      // Rejection slopes for the linear fit clipping algorithm
      if ( I.p_rejection == IIRejection::LinearFit )
         *M++ = FVector( .0F, IntegrationFile::Width() );

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

#undef E
#undef I

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegrationDataLoaderEngine.cpp - Released 2021-04-09T19:41:48Z
