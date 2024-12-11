//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// pcl/AstrometricReprojection.h - Released 2024-12-11T17:42:29Z
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

#ifndef __PCL_AstrometricReprojection_h
#define __PCL_AstrometricReprojection_h

/// \file pcl/AstrometricReprojection.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AstrometricMetadata.h>
#include <pcl/GeometricTransformation.h>
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class AstrometricReprojection
 * \brief Astrometric image reprojection.
 *
 * ### TODO: Write a detailed description for %AstrometricReprojection
 */
class PCL_CLASS AstrometricReprojection : public InterpolatingGeometricTransformation,
                                          public ParallelProcess
{
public:

   /*!
    * Constructs a new %AstrometricReprojection object.
    *
    * \param interpolation    Reference to a pixel interpolation for
    *                         reprojection of source pixels.
    *
    * \param targetSolution   The astrometric solution of the target image.
    *                         This object provides image-to-celestial
    *                         coordinate transformations for reverse projection
    *                         of interpolated source pixels on a target image.
    *
    * \param sourceSolution   The astrometric solution of the source image.
    *                         This object provides celestial-to-image
    *                         coordinate transformations for interpolation of
    *                         source image pixels.
    *
    * \param sourceImage      The source image that will be reprojected.
    *
    * All objects specified in a call to this constructor must remain valid
    * while this instance exists.
    */
   AstrometricReprojection( PixelInterpolation& interpolation,
                            const AstrometricMetadata& targetSolution,
                            const AstrometricMetadata& sourceSolution,
                            const ImageVariant& sourceImage )
      : InterpolatingGeometricTransformation( interpolation )
      , m_targetSolution( targetSolution )
      , m_sourceSolution( sourceSolution )
      , m_sourceImage( sourceImage )
   {
   }

   /*!
    * Copy constructor.
    */
   AstrometricReprojection( const AstrometricReprojection& ) = default;

   /*!
    * Returns a reference to the astrometric solution of the target image. The
    * returned object provides image-to-celestial coordinate transformations
    * for reverse projection of interpolated source pixels on a target image.
    */
   const AstrometricMetadata& TargetSolution() const
   {
      return m_targetSolution;
   }

   /*!
    * Returns a reference to the astrometric solution of the source image. The
    * returned object provides celestial-to-image coordinate transformations
    * for interpolation of source image pixels.
    */
   const AstrometricMetadata& SourceSolution() const
   {
      return m_sourceSolution;
   }

   /*!
    * Returns a reference to the source image that will be reprojected.
    */
   const ImageVariant& SourceImage() const
   {
      return m_sourceImage;
   }

   /*!
    * Returns the rectangle in target image coordinates that encloses the
    * boundaries of the reprojected source image.
    */
   DRect TargetRect() const
   {
      return TargetRect( m_targetSolution, m_sourceSolution, m_sourceSolution.Bounds() );
   }

   /*!
    * Returns a reprojected rectangle in target image coordinates.
    *
    * \param targetSolution   The target astrometric solution.
    *
    * \param sourceSolution   The source astrometric solution.
    *
    * \param sourceRect       The source rectangular region.
    *
    * This function throws an Error exception in the event of invalid
    * coordinate transformations.
    */
   static DRect TargetRect( const AstrometricMetadata& targetSolution,
                            const AstrometricMetadata& sourceSolution,
                            const Rect& sourceRect );

   /*!
    * Returns true iff the specified point in target image coordinates lies
    * inside the specified source image coordinates after astrometric
    * reprojection.
    *
    * \param targetSolution   The target astrometric solution.
    *
    * \param sourceSolution   The source astrometric solution.
    *
    * \param targetPt         The target point in target image coordinates.
    *
    * \param sourceRect       The source rectangular region in source image
    *                         coordinates.
    */
   static bool TargetPointInsideSourceRect( const AstrometricMetadata& targetSolution,
                                            const AstrometricMetadata& sourceSolution,
                                            const DPoint& targetPt,
                                            const DRect& sourceRect );

   /*!
    * Returns true iff the specified rectangular region in target image
    * coordinates lies inside the specified source image coordinates after
    * astrometric reprojection.
    *
    * \param targetSolution   The target astrometric solution.
    *
    * \param sourceSolution   The source astrometric solution.
    *
    * \param targetRect       The target rectangular region in target image
    *                         coordinates.
    *
    * \param sourceRect       The source rectangular region in source image
    *                         coordinates.
    */
   static bool TargetRectInsideSourceRect( const AstrometricMetadata& targetSolution,
                                           const AstrometricMetadata& sourceSolution,
                                           const DRect& targetRect,
                                           const DRect& sourceRect );

   /*!
    */
   void GetNewSizes( int& width, int& height ) const override
   {
   }

   /*!
    * Returns the total number of black (zero) source pixels reprojected in the
    * last process execution. This is useful to detect reprojection of empty
    * source image regions.
    *
    * Returns zero if this instante has not yet been executed.
    */
   size_type ZeroCount() const
   {
      return m_zeroCount;
   }

protected:

   const   AstrometricMetadata& m_targetSolution;
   const   AstrometricMetadata& m_sourceSolution;
   const   ImageVariant&        m_sourceImage;
   mutable size_type            m_zeroCount = 0;

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;

   friend class PCL_AstrometricReprojectionEngine;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_AstrometricReprojection_h

// ----------------------------------------------------------------------------
// EOF pcl/AstrometricReprojection.h - Released 2024-12-11T17:42:29Z
