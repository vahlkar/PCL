//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/APIException.h - Released 2024-12-27T18:16:01Z
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

#ifndef __PCL_API_APIException_h
#define __PCL_API_APIException_h

#include <pcl/Defs.h>

#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ### NOTE
 * The following exception classes may be thrown across different DSOs, which
 * requires a typeinfo lookup upon exception catching. For this reason these
 * classes, despite being for internal use exclusively, must be exported with
 * the PCL_CLASS macro.
 */

// ----------------------------------------------------------------------------

class PCL_CLASS APIError : public Error
{
public:

   APIError()
   {
      GetAPIErrorCode();
   }

   APIError( const pcl::APIError& ) = default;

   APIError( const String& s )
      : Error( s )
   {
      GetAPIErrorCode();
   }

   String FormatInfo() const override;

   String Caption() const override
   {
      return "PixInsight API Error";
   }

private:

   uint32 apiErrorCode = 0;

   void GetAPIErrorCode();
};

// ----------------------------------------------------------------------------

class PCL_CLASS APIFunctionError : public pcl::APIError
{
public:

   APIFunctionError( const pcl::APIFunctionError& x )
      : pcl::APIError( x )
   {
   }

   APIFunctionError( const String& funcId )
      : pcl::APIError( funcId + "(): API function error" )
   {
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_API_APIException_h

// ----------------------------------------------------------------------------
// EOF pcl/APIException.h - Released 2024-12-27T18:16:01Z
