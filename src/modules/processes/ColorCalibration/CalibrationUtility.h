//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// CalibrationUtility.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#ifndef __CalibrationUtility_h
#define __CalibrationUtility_h

#include <pcl/LinearFit.h>
#include <pcl/Point.h>
#include <pcl/Rectangle.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS ImageVariant;
class PCL_CLASS String;

// ----------------------------------------------------------------------------

/*
 * PSF photometry sample.
 */
struct StarPSFSample
{
   DPoint pos;
   DRect  bounds;
   float  signal[ 3 ];
};

/*
 * Color calibration sample.
 */
struct ColorSample
{
   DPoint pos;
   double catalog[ 3 ];
   double measured[ 3 ];
};

// ----------------------------------------------------------------------------

/*
 * Compute background reference values.
 */
DVector ComputeBackgroundReference( const ImageVariant& image, double sigmaLow, double sigmaHigh );

/*
 * Apply background neutralization.
 */
void ApplyBackgroundNeutralization( ImageVariant& image, const DVector& backgroundReference );

/*
 * Apply a white balance correction.
 */
void ApplyWhiteBalance( ImageVariant& image, const DVector& whiteBalanceFactors );

/*
 * Robust linear regression.
 */
LinearFit RobustLinearRegression( const Vector& x, const Vector& y, const String& what );

/*
 * Create an 8-bit grayscale image with star positions rendered as cross marks.
 * The new image can be used directly as a mask on the target image.
 */
void CreateStarMapWindow( int width, int height, const IsoString& id, const Array<StarPSFSample>& stars );

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CalibrationUtility_h

// ----------------------------------------------------------------------------
// EOF CalibrationUtility.h - Released 2023-08-28T15:23:41Z
