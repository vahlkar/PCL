//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// pcl/Algebra.cpp - Released 2023-11-25T17:26:56Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/Algebra.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void PCL_FUNC InPlaceGaussJordan( Matrix& A, Matrix& B )
{
   A.EnsureUnique();
   B.EnsureUnique();
   if ( (*API->Numerical->GaussJordanInPlaceD)( A.DataPtr(), B.DataPtr(), A.Rows(), B.Columns() ) == api_false )
      throw APIFunctionError( "GaussJordanInPlaceD" );
}

void PCL_FUNC InPlaceGaussJordan( FMatrix& A, FMatrix& B )
{
   A.EnsureUnique();
   B.EnsureUnique();
   if ( (*API->Numerical->GaussJordanInPlaceF)( A.DataPtr(), B.DataPtr(), A.Rows(), B.Columns() ) == api_false )
      throw APIFunctionError( "GaussJordanInPlaceF" );
}

// ----------------------------------------------------------------------------

void PCL_FUNC InPlaceSVDImplementation( Matrix& A, Vector& W, Matrix& V )
{
   A.EnsureUnique();
   int m = A.Rows();
   int n = A.Columns();
   W = Vector( n );
   V = Matrix( n, n );
   if ( (*API->Numerical->SVDInPlaceD)( A.DataPtr(), W.DataPtr(), V.DataPtr(), m, n ) == api_false )
      throw APIFunctionError( "SVDInPlaceD" );
}

void PCL_FUNC InPlaceSVDImplementation( FMatrix& A, FVector& W, FMatrix& V )
{
   A.EnsureUnique();
   int m = A.Rows();
   int n = A.Columns();
   W = FVector( n );
   V = FMatrix( n, n );
   if ( (*API->Numerical->SVDInPlaceF)( A.DataPtr(), W.DataPtr(), V.DataPtr(), m, n ) == api_false )
      throw APIFunctionError( "SVDInPlaceF" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Algebra.cpp - Released 2023-11-25T17:26:56Z
