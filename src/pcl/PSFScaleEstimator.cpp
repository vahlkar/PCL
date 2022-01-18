//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.18
// ----------------------------------------------------------------------------
// pcl/PSFScaleEstimator.cpp - Released 2022-01-18T11:02:48Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2022 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/PSFScaleEstimator.h>
#include <pcl/QuadTree.h>

namespace pcl
{

// ----------------------------------------------------------------------------

int PSFScaleEstimator::SetReference( const ImageVariant& image )
{
   m_psfReference = FitStars( image );
   return int( m_psfReference.Length() );
}

// ----------------------------------------------------------------------------

PSFScaleEstimator::Estimate PSFScaleEstimator::EstimateScale( const ImageVariant& image ) const
{
   if ( m_psfReference.IsEmpty() )
      throw Error( "PSFScaleEstimator::EstimateScale(): Internal error: No reference image has been defined" );

   Estimate E;

   Array<PSFData> psfs = FitStars( image );

   /*
    * Star matching by proximity search.
    */
   QuadTree<PSFData> T2( psfs );
   Array<PSFData> P1, P2;
   for ( const PSFData& p1 : m_psfReference )
   {
      QuadTree<PSFData>::point_list p2 =
         T2.Search( QuadTree<PSFData>::rectangle( p1.c0.x - m_psfSearchTolerance, p1.c0.y - m_psfSearchTolerance,
                                                  p1.c0.x + m_psfSearchTolerance, p1.c0.y + m_psfSearchTolerance ) );
      if ( !p2.IsEmpty() )
      {
         size_type j = 0;
         if ( p2.Length() > 1 )
         {
            double d2 = p2[0].c0.SquaredDistanceTo( p1.c0 );
            for ( size_type i = 1; i < p2.Length(); ++i )
            {
               double di = p2[i].c0.SquaredDistanceTo( p1.c0 );
               if ( di < d2 )
               {
                  j = i;
                  d2 = di;
               }
            }
         }

         P1 << p1;
         P2 << p2[j];
      }
   }

   if ( !P1.IsEmpty() )
   {
      DVector D( P1.Length() );
      for ( size_type i = 0; i < P1.Length(); ++i )
         D[i] = P1[i].signal/P2[i].signal;

      E.scale = D.Median();
      E.count = D.Length();
   }

   return E;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/PSFScaleEstimator.cpp - Released 2022-01-18T11:02:48Z
