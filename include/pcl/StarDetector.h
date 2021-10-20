//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.12
// ----------------------------------------------------------------------------
// pcl/StarDetector.h - Released 2021-10-20T18:03:58Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_StarDetector_h
#define __PCL_StarDetector_h

/// \file pcl/StarDetector.h

#include <pcl/Defs.h>

#include <pcl/ImageVariant.h>
#include <pcl/ParallelProcess.h>
#include <pcl/PSFFit.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class StarDetector
 * \brief Automatic star detection
 * \sa PSFFit, PSFSignalEstimator
 */
class PCL_CLASS StarDetector : public ParallelProcess
{
public:

   /*!
    * Represents a point spread function type.
    */
   typedef PSFFit::psf_function  psf_function;

   /*!
    * \struct pcl::StarDetector::Star
    * \brief Structure to hold the data of a detected star.
    */
   struct Star
   {
      /*!
       * The type of a coordinate used to represent star positions (barycenter
       * or PSF centroid coordinates).
       */
      typedef DPoint::component  component;

      DPoint pos = 0.0;     //!< Barycenter or centroid position in pixels, image coordinates.
      Rect   rect = 0;      //!< Detection region in pixels, image coordinates.
      float  flux = 0;      //!< Total flux minus local background.
      float  signal = 0;    //!< Estimated mean signal over the PSF fitting region.
      float  mad = 0;       //!< Average absolute deviation of measured PSF with respect to sampled image data.
      Star*  ref = nullptr; //!< Referenced star, for cross-reference applications.

      /*!
       * Default constructor.
       */
      Star() = default;

      /*!
       * Constructs a new %Star object with the specified position \a p in
       * image coordinates, total flux \a f, mean signal \a s  and PSF average
       * deviation \a m.
       */
      Star( const DPoint& p, const Rect& r, float f = 0, float s = 0, float m = 0 )
         : pos( p ), rect( r ), flux( f ), signal( s ), mad( m )
      {
      }

      /*!
       * Copy constructor.
       */
      Star( const Star& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      Star& operator =( const Star& ) = default;

      /*!
       * Equality operator. Two stars are equal iif they are located at the
       * same coordinates.
       */
      bool operator ==( const Star& s ) const
      {
         return pos == s.pos;
      }

      /*!
       * Less-than relational operator. By default stars are sorted by
       * brightness in descending order (brighter stars come first).
       */
      bool operator <( const Star& s ) const
      {
         return flux > s.flux;
      }

      /*!
       * Array subscript operator for coordinate selection, pcl::QuadTree
       * compatible.
       */
      component operator []( int i ) const
      {
         return pos[i];
      }
   };

   /*!
    * A dynamic array of detected pcl::StarDetector::Star structures.
    */
   typedef Array<Star>  star_list;

   /*!
    * Default constructor.
    */
   StarDetector() = default;

   /*!
    * Copy constructor.
    */
   StarDetector( const StarDetector& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   StarDetector& operator =( const StarDetector& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~StarDetector()
   {
   }

   /*!
    *
    */
   int StructureLayers() const
   {
      return m_structureLayers;
   }

   /*!
    *
    */
   void SetStructureLayers( int n )
   {
      PCL_PRECONDITION( n > 0 && n <= 8 )
      m_structureLayers = Range( n, 1, 8 );
   }

   /*!
    *
    */
   int NoiseLayers() const
   {
      return m_noiseLayers;
   }

   /*!
    *
    */
   void SetNoiseLayers( int n )
   {
      PCL_PRECONDITION( n >= 0 && n <= 4 )
      m_noiseLayers = Range( n, 0, 4 );
   }

   /*!
    *
    */
   int HotPixelFilterRadius() const
   {
      return m_hotPixelFilterRadius;
   }

   /*!
    *
    */
   void SetHotPixelFilterRadius( int n )
   {
      PCL_PRECONDITION( n >= 0 && n <= 3 )
      m_hotPixelFilterRadius = Range( n, 0, 3 );
   }

   /*!
    *
    */
   int NoiseReductionFilterRadius() const
   {
      return m_noiseReductionFilterRadius;
   }

   /*!
    *
    */
   void SetNoiseReductionFilterRadius( int n )
   {
      PCL_PRECONDITION( n >= 0 && n <= 64 )
      m_noiseReductionFilterRadius = Range( n, 0, 64 );
   }

   /*!
    *
    */
   int MinStructureSize() const
   {
      return m_minStructureSize;
   }

   /*!
    *
    */
   void SetMinStructureSize( int n )
   {
      PCL_PRECONDITION( n >= 0 )
      m_minStructureSize = Max( 0, n );
   }

   /*!
    *
    */
   float Sensitivity() const
   {
      return m_sensitivity;
   }

   /*!
    *
    */
   void SetSensitivity( float s )
   {
      PCL_PRECONDITION( s >= 0.001 && s <= 1000 )
      m_sensitivity = Range( s, 0.001F, 1000.0F );
   }

   /*!
    *
    */
   float PeakResponse() const
   {
      return m_peakResponse;
   }

   /*!
    *
    */
   void SetPeakResponse( float r )
   {
      PCL_PRECONDITION( r >= 0 && r <= 1 )
      m_peakResponse = Range( r, 0.0F, 1.0F );
   }

   /*!
    *
    */
   float MaxDistortion() const
   {
      return m_maxDistortion;
   }

   /*!
    *
    */
   void SetMaxDistortion( float d )
   {
      PCL_PRECONDITION( d >= 0 && d <= 1 )
      m_maxDistortion = Range( d, 0.0F, 1.0F );
   }

   /*!
    *
    */
   float UpperLimit() const
   {
      return m_upperLimit;
   }

   /*!
    *
    */
   void SetUpperLimit( float u )
   {
      PCL_PRECONDITION( u >= 0 && u <= 1 )
      m_upperLimit = Range( u, 0.0F, 1.0F );
   }

   /*!
    *
    */
   bool IsImageInversionEnabled() const
   {
      return m_invert;
   }

   /*!
    *
    */
   void EnableImageInversion( bool enabled = true )
   {
      m_invert = enabled;
   }

   /*!
    *
    */
   void DisableImageInversion( bool disable = true )
   {
      EnableImageInversion( !disable );
   }

   /*!
    *
    */
   bool IsPSFFittingEnabled() const
   {
      return m_fitPSF;
   }

   /*!
    *
    */
   void EnablePSFFitting( bool enable = true )
   {
      m_fitPSF = enable;
   }

   /*!
    *
    */
   void DisablePSFFitting( bool disable = true )
   {
      EnablePSFFitting( !disable );
   }

   /*!
    *
    */
   psf_function PSFType() const
   {
      return m_psfType;
   }

   /*!
    *
    */
   void SetPSFType( psf_function type )
   {
      m_psfType = type;
   }

   /*!
    *
    */
   bool IsEllipticPSF() const
   {
      return m_psfElliptic;
   }

   /*!
    *
    */
   void EnableEllipticPSF( bool enable = true )
   {
      m_psfElliptic = enable;
   }

   /*!
    *
    */
   void DisableEllipticPSF( bool disable = true )
   {
      EnableEllipticPSF( !disable );
   }

   /*!
    * Returns the centroid tolerance in pixels.
    *
    * The centroid tolerance is the maximum allowed distance in pixels between
    * a detected star position (or \e barycenter) and the centroid position of
    * the corresponding fitted PSF. Stars where that distance is greater than
    * the value returned by this function will be rejected and excluded for
    * signal estimation. This allows preventing PSF fits departing too much
    * from detected star coordinates, which enforces robustness and stability
    * of the PSF fitting process.
    *
    * The default centroid tolerance is 1.5 pixels.
    */
   float PSFCentroidTolerance() const
   {
      return m_psfTolerance;
   }

   /*!
    * Sets the centroid tolerance in pixels. See PSFCentroidTolerance() for a
    * description of this parameter.
    */
   void SetPSFCentroidTolerance( float t )
   {
      PCL_PRECONDITION( t >= 0 )
      m_psfTolerance = Max( 0.0F, t );
   }

   /*!
    * Returns a pointer to the current star detection mask image, or nullptr if
    * no detection mask is being used.
    *
    * If a detection mask is used, star detection will only consider source
    * pixels for which the corresponding mask pixels are nonzero. This allows
    * for the implementation of partial star detection tasks limited to
    * arbitrary regions of interest.
    */
   const UInt8Image* DetectionMask() const
   {
      return m_mask;
   }

   /*!
    * Sets the detection \a mask image. See DetectionMask() for a description
    * of this parameter.
    *
    * If the specified \a mask pointer is nullptr, no mask will be used in
    * subsequent star detection tasks performed by this object. Otherwise, the
    * specified \a mask image must have the same dimensions as the image(s)
    * that will be specified for star detection, and must remain valid while
    * this object performs star detection tasks.
    */
   void SetDetectionMask( const UInt8Image* mask )
   {
      if ( mask != nullptr )
         if ( mask->IsEmpty() )
            mask = nullptr;
      m_mask = mask;
   }

   /*!
    * Performs star detection with the current parameters. Returns a dynamic
    * array of Star structures sorted by brightness (flux) in descending order.
    */
   star_list DetectStars( const ImageVariant& image ) const;

   /*!
    * Performs star detection with the current parameters. Returns a dynamic
    * array of Star structures sorted by brightness (flux) in descending order.
    *
    * This operator is equivalent to the DetectStars( const ImageVariant& )
    * member function.
    */
   star_list operator()( const ImageVariant& image ) const
   {
      return DetectStars( image );
   }

   /*!
    * Computes a binary map of star detection structures for the specified
    * \a image and the current set of star detection parameters.
    *
    * Returns the star detection map as a new image.
    */
   Image StructureMap( const ImageVariant& image ) const;

   /*!
    * Isolates star detection structures for the specified \a image.
    *
    * Returns a new image which is the result of multiplying the specified
    * \a image by the binarized structure map generated with the current star
    * detection parameters.
    */
   Image Structures( const ImageVariant& image ) const;

protected:

         int          m_structureLayers = 5;
         int          m_noiseLayers = 1;
         int          m_hotPixelFilterRadius = 1;
         int          m_noiseReductionFilterRadius = 0;
         int          m_minStructureSize = 0;
         float        m_sensitivity = 0.1F;
         float        m_peakResponse = 0.75F;
         float        m_maxDistortion = 0.5F;
         float        m_upperLimit = 1.0F;
         bool         m_invert = false;
         bool         m_fitPSF = false;
         psf_function m_psfType = PSFunction::Gaussian;
         bool         m_psfElliptic = false;
         float        m_psfTolerance = 1.5;
   const UInt8Image*  m_mask = nullptr;

private:

   star_list DetectStars( Image& image ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_StarDetector_h

// ----------------------------------------------------------------------------
// EOF pcl/StarDetector.h - Released 2021-10-20T18:03:58Z
