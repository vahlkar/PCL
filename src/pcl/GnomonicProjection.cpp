//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/GnomonicProjection.cpp - Released 2024-12-28T16:53:56Z
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

#include <pcl/GnomonicProjection.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

bool GnomonicProjection::Direct( DPoint& pW, const DPoint& pRD ) const noexcept
{
   double sinRA, cosRA;
   SinCos( Rad( pRD.x ) - m_ra0, sinRA, cosRA );
   double sinDec, cosDec;
   SinCos( Rad( pRD.y ), sinDec, cosDec );
   double A = cosDec * cosRA;
   double cosD = m_sinDec0*sinDec + m_cosDec0*A;
   if ( cosD < 1.0e-5 ) // distance > 89.999 deg ?
      return false;
   double F = Const<double>::deg()/cosD;
   pW.x = F * cosDec * sinRA;
   pW.y = F * (m_cosDec0*sinDec - m_sinDec0*A);
   return true;
}

// ----------------------------------------------------------------------------

bool GnomonicProjection::Inverse( DPoint& pRD, const DPoint& pW ) const noexcept
{
   double X = Rad( pW.x );
   double Y = Rad( pW.y );
   double sinD, cosD;
   SinCos( ArcTan( Sqrt( X*X + Y*Y ) ), sinD, cosD );
   double sinB, cosB;
   SinCos( ArcTan( X, -Y ), sinB, cosB );
   pRD.x = Deg( m_ra0 + ArcTan( sinD*sinB, m_sinDec0*sinD*cosB + m_cosDec0*cosD ) );
   pRD.y = Deg( ArcSin( m_sinDec0*cosD - m_cosDec0*sinD*cosB ) );
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/GnomonicProjection.cpp - Released 2024-12-28T16:53:56Z
