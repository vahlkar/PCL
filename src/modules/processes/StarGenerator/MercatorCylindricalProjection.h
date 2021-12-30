//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.17
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 1.1.0
// ----------------------------------------------------------------------------
// MercatorCylindricalProjection.h - Released 2021-12-29T20:37:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
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

#ifndef __MercatorCylindricalProjection_h
#define __MercatorCylindricalProjection_h

#include "ProjectionSystem.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class MercatorCylindricalProjection : public ProjectionSystem
{
public:

   MercatorCylindricalProjection( double lon )
      : ProjectionSystem( lon, 0 )
   {
   }

   bool IsCylindricalProjection() const override
   {
      return true;
   }

   bool IsConformalProjection() const override
   {
      return true;
   }

   String Name() const override
   {
      return "Mercator Cylindrical Projection";
   }

   void SphericalToRectangular( double& x, double& y, double lon, double lat ) const override
   {
      x = InPiRange( lon - lon0 );
      y = ArcSinh( Tan( lat ) );
   }

   void RectangularToSpherical( double& lon, double& lat, double x, double y ) const override
   {
      lon = In2PiRange( lon0 + x );
      lat = ArcTan( Sinh( y ) );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __MercatorCylindricalProjection_h

// ----------------------------------------------------------------------------
// EOF MercatorCylindricalProjection.h - Released 2021-12-29T20:37:28Z
