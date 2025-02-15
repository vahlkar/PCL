//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// StarDetector.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#ifndef __StarDetector_h
#define __StarDetector_h

#include <pcl/Image.h>
#include <pcl/MetaParameter.h>   // pcl_enum, pcl_bool

namespace pcl
{

// ----------------------------------------------------------------------------

struct StarData
{
   pcl_enum status;  // error code (StarDetector::Status)
   int32    channel; // channel index (0=R/K, 1=G, 2=B)
   DRect    rect;    // selection rectangle
   DPoint   pos;     // barycenter position

   mutable DRect drawRect; // drawing region

   operator bool() const;

   String StatusText() const;
};

// ----------------------------------------------------------------------------

class StarDetector
{
public:

   enum Status
   {
      NotDetected,
      DetectedOk,
      NoSignificantData,
      CrossingEdges,
      OutsideImage,
      NoConvergence,
      UnknownError
   };

   StarData star;

   StarDetector( const Image& img, int channel,
                 const DPoint& pos, int searchRadius = 8, float bkgThreshold = 1.0F,
                 bool autoAperture = true );

   operator bool() const
   {
      return star;
   }
};

// ----------------------------------------------------------------------------

inline StarData::operator bool() const
{
   return status == StarDetector::DetectedOk;
}

// ----------------------------------------------------------------------------

inline String StarData::StatusText() const
{
   switch ( status )
   {
   case StarDetector::NotDetected:       return "Not detected";
   case StarDetector::DetectedOk:        return "Detected Ok";
   case StarDetector::NoSignificantData: return "No significant data";
   case StarDetector::CrossingEdges:     return "Crossing edges";
   case StarDetector::OutsideImage:      return "Outside image";
   case StarDetector::NoConvergence:     return "No convergence";
   default:
   case StarDetector::UnknownError:      return "Unknown error";
   }
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarDetector_h

// ----------------------------------------------------------------------------
// EOF StarDetector.h - Released 2024-12-28T16:54:15Z
