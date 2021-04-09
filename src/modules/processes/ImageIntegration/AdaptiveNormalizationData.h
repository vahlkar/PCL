//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// AdaptiveNormalizationData.h - Released 2021-04-09T19:41:48Z
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

#ifndef __AdaptiveNormalizationData_h
#define __AdaptiveNormalizationData_h

#include <pcl/GridInterpolation.h>
#include <pcl/Image.h>
#include <pcl/MultiVector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class AdaptiveNormalizationData
{
public:

   AdaptiveNormalizationData() = default;
   AdaptiveNormalizationData( const AdaptiveNormalizationData& ) = default;

   AdaptiveNormalizationData( const Image& image, int scaleEstimator, int gridSize );

   AdaptiveNormalizationData( int width, int height, const DVector& x, const DVector& y,
                              const DMultiVector& m, const DMultiVector& s0, const DMultiVector& s1 );

   bool IsValid() const;

   double Location( double x, double y, int c = 0 ) const
   {
      return m_location[c]( x, y );
   }

   double ScaleLow( double x, double y, int c = 0 ) const
   {
      return m_scaleLow[c]( x, y );
   }

   double ScaleHigh( double x, double y, int c = 0 ) const
   {
      return m_scaleHigh[c]( x, y );
   }

   void Clear();

private:

   typedef GridInterpolation     interpolation;
   typedef Array<interpolation>  interpolators;

   int           m_width = -1;  // reference width
   int           m_height = -1; // reference height
   DVector       m_x;           // node X coordinates
   DVector       m_y;           // node Y coordinates
   DMultiVector  m_m;           // location estimates
   DMultiVector  m_s0;          // low scale estimates
   DMultiVector  m_s1;          // high scale estimates
   interpolators m_location;    // location interpolation
   interpolators m_scaleLow;    // low scale interpolation
   interpolators m_scaleHigh;   // high scale interpolation

   void InitInterpolations();

   // For cache validation
   static int NumberOfGridElements( int width, int height, int gridSize );

   friend class IntegrationFile;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AdaptiveNormalizationData_h

// ----------------------------------------------------------------------------
// EOF AdaptiveNormalizationData.h - Released 2021-04-09T19:41:48Z
