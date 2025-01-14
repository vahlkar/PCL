//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Interpolation.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_Interpolation_h
#define __PCL_Interpolation_h

/// \file pcl/Interpolation.h

#include <pcl/Defs.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::InterpolationAlgorithm
 * \brief Standard pixel interpolation algorithms.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>InterpolationAlgorithm::NearestNeighbor</td>         <td>Nearest neighbor interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::Bilinear</td>                <td>Bilinear interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::BicubicSpline</td>           <td>Bicubic spline interpolation (usually the default algorithm)</td></tr>
 * <tr><td>InterpolationAlgorithm::Bicubic</td>                 <td>A synonym for bicubic spline interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::BicubicBSpline</td>          <td>Bicubic B-spline interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::Lanczos3</td>                <td>Lanczos-3 interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::Lanczos4</td>                <td>Lanczos-4 interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::Lanczos5</td>                <td>Lanczos-5 interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::MitchellNetravaliFilter</td> <td>Mitchell-Netravali cubic filter interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::CatmullRomSplineFilter</td>  <td>Catmull-Rom spline filter interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::CubicBSplineFilter</td>      <td>Cubic B-spline filter interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::Auto</td>                    <td>Used to indicate an automatic algorithm selection (often the default option)</td></tr>
 * <tr><td>InterpolationAlgorithm::Default</td>                 <td>Used to indicate a default interpolation algorithm (currently bicubic spline)</td></tr>
 * </table>
 */
namespace InterpolationAlgorithm
{
   enum value_type
   {
      Auto = -1,
      NearestNeighbor = 0,
      Bilinear,
      BicubicSpline,
      Bicubic = BicubicSpline,
      BicubicBSpline,
      Lanczos3,
      Lanczos4,
      Lanczos5,
      MitchellNetravaliFilter,
      CatmullRomSplineFilter,
      CubicBSplineFilter,
      NumberOfInterpolationAlgorithms,
      Default = BicubicSpline
   };
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Interpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/Interpolation.h - Released 2024-12-28T16:53:48Z
