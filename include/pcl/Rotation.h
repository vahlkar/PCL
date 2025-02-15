//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Rotation.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_Rotation_h
#define __PCL_Rotation_h

/// \file pcl/Rotation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/GeometricTransformation.h>
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Rotation
 * \brief %Image rotation algorithm.
 *
 * ### TODO: Write a detailed description for %Rotation
 */
class PCL_CLASS Rotation : public InterpolatingGeometricTransformation,
                           public ParallelProcess
{
public:

   /*!
    * Constructs a new %Rotation object.
    *
    * \param p       Reference to a pixel interpolation, which must remain
    *                valid while this object exists.
    *
    * \param angle   Rotation angle in radians
    *
    * \param cx      Horizontal coordinate of the center of rotation, measured
    *                in pixels in the image coordinate system.
    *
    * \param cy      Vertical coordinate of the center of rotation, measured
    *                in pixels in the image coordinate system.
    */
   Rotation( PixelInterpolation& p, double angle = 0, double cx = 0, double cy = 0 )
      : InterpolatingGeometricTransformation( p )
      , m_angle( angle )
      , m_center( cx, cy )
   {
   }

   /*!
    * Constructs a new %Rotation object.
    *
    * \param p       Reference to a pixel interpolation, which must remain
    *                valid while this object exists.
    *
    * \param angle   Rotation angle in radians
    *
    * \param center  Coordinates of the center of rotation, measured in pixels
    *                in the image coordinate system.
    */
   Rotation( PixelInterpolation& p, double angle, const DPoint& center )
      : InterpolatingGeometricTransformation( p )
      , m_angle( angle )
      , m_center( center )
   {
   }

   /*!
    * Copy constructor.
    */
   Rotation( const Rotation& ) = default;

   /*!
    * Returns the rotation angle in radians.
    */
   double Angle() const
   {
      return m_angle;
   }

   /*!
    * Sets the rotation angle in radians.
    */
   void SetAngle( double rads )
   {
      m_angle = rads;
   }

   /*!
    * Returns the center of rotation. The returned point coordinates are
    * measured in pixels in the image coordinate system.
    */
   DPoint Center() const
   {
      return m_center;
   }

   /*!
    * Returns the horizontal coordinate of the center of rotation, measured in
    * pixels in the image coordinate system.
    */
   double CenterX() const
   {
      return m_center.x;
   }

   /*!
    * Returns the vertical coordinate of the center of rotation, measured in
    * pixels in the image coordinate system.
    */
   double CenterY() const
   {
      return m_center.y;
   }

   /*!
    * Sets the center of rotation. The specified \a center point coordinates
    * must be expressed in pixels in the image coordinate system.
    */
   void SetCenter( const DPoint& center )
   {
      m_center = center;
   }

   /*!
    * Sets the center of rotation to the specified coordinates \a xc and \a yc,
    * which must be expressed in pixels in the image coordinate system.
    */
   void SetCenter( double xc, double yc )
   {
      m_center.x = xc; m_center.y = yc;
   }

   /*!
    * Returns true iff this transformation will preserve the entire target
    * image without clipping any pixels after rotating it.
    *
    * An unclipped rotation ignores the specified rotation center coordinates
    * (see CenterX() and CenterY(), as well as the class constructor) and
    * rotates images with respect to their geometric centers. To accommodate
    * all rotated pixels in the result, the unclipped rotated image will always
    * be bigger than the original, except for the trivial cases of rotations by
    * 0 and 180 degrees.
    */
   bool IsUnclipped() const
   {
      return m_unclipped;
   }

   /*!
    * Enables unclipped rotations for this object. See IsUnclipped() for
    * information on unclipped rotations.
    */
   void EnableUnclippedRotation( bool enable = true )
   {
      m_unclipped = enable;
   }

   /*!
    * Disables unclipped rotations for this object. See IsUnclipped() for
    * information on unclipped rotations.
    */
   void DisableUnclippedRotation( bool disable = true )
   {
      EnableUnclippedRotation( !disable );
   }

   /*!
    * Returns the current vector of per-channel filling values for uncovered
    * image regions.
    *
    * See the documentation for SetFillValues() for more information.
    */
   const DVector& FillValues() const
   {
      return m_fillValues;
   }

   /*!
    * Sets a vector of per-channel filling values for uncovered image regions.
    *
    * Uncovered regions result when an image is rotated by non-orthogonal
    * angles.
    *
    * By default, there are no filling values defined (and hence the returned
    * vector is empty by default). When the %Rotation instance is executed and
    * a filling value is not defined for a channel of the target image,
    * uncovered regions are filled with the minimum sample value in the native
    * range of the image (usually zero).
    */
   void SetFillValues( const DVector& fillValues )
   {
      m_fillValues = fillValues;
   }

   /*!
    */
   void GetNewSizes( int& width, int& height ) const override;

   /*!
    */
   bool SupportsGammaCorrection() const override
   {
      return true;
   }

protected:

   double   m_angle = 0; // radians
   bool     m_unclipped = false;
   DPoint   m_center = DPoint( 0 );
   DVector  m_fillValues;

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Rotation_h

// ----------------------------------------------------------------------------
// EOF pcl/Rotation.h - Released 2024-12-28T16:53:48Z
