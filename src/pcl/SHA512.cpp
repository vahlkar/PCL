//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/SHA512.cpp - Released 2024-12-28T16:53:56Z
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

#include <pcl/Cryptography.h>
#include <pcl/Exception.h>

extern "C" {
#include <RFC6234/sha.h>
}

namespace pcl
{

// ----------------------------------------------------------------------------

#define CTX reinterpret_cast<SHA512Context*>( m_context )

SHA512::~SHA512()
{
   if ( m_context != nullptr )
      delete CTX, m_context = nullptr;
}

void SHA512::Initialize()
{
   if ( m_context == nullptr )
      m_context = new SHA512Context;
   SHA512Reset( CTX );
}

void SHA512::Update( const void* data, size_type size )
{
   if ( data != nullptr )
      if ( size > 0 )
      {
         if ( m_context == nullptr )
         {
            m_context = new SHA512Context;
            SHA512Reset( CTX );
         }
         const uint8* bytes = reinterpret_cast<const uint8*>( data );
         do
         {
            size_type blockSize = Min( size_type( 0xfffffff0 ), size );
            SHA512Input( CTX, bytes, unsigned( blockSize ) );
            size -= blockSize;
            bytes += blockSize;
         }
         while ( size > 0 );
      }
}

void SHA512::DoFinalize( void* hash )
{
   if ( m_context == nullptr )
      throw Error( "SHA512::Finalize(): Invalid call on uninitialized object." );
   SHA512Result( CTX, static_cast<uint8_t*>( hash ) );
   SHA512Reset( CTX );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SHA512.cpp - Released 2024-12-28T16:53:56Z
