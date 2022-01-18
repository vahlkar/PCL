//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.18
// ----------------------------------------------------------------------------
// pcl/StarDetector.h - Released 2022-01-18T11:02:40Z
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

      DPoint pos = 0.0;     /*!< Star position in image coordinates. Corresponds to the centroid of
                                 the fitted PSF when PSF fitting is enabled; to the barycenter
                                 calculated by the star detection algorithm otherwise. */
      Rect   rect = 0;      //!< Detection region in pixels, image coordinates.
      float  flux = 0;      //!< Total flux minus local background.
      float  signal = 0;    /*!< Estimated mean signal over the local background within the PSF
                                 fitting region. Nonzero only when PSF fitting is enabled. */
      float  mad = 0;       /*!< Average absolute deviation of the fitted PSF with respect to
                                 sampled image data. */
      Star*  ref = nullptr; //!< Referenced star, useful for cross-referencing applications.

      /*!
       * Default constructor.
       */
      Star() = default;

      /*!
       * Constructs a new %Star object with the specified position \a p in
       * image coordinates, total flux \a f, mean signal \a s and PSF average
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
       * Equality operator. Two stars are equal iff they are located at the
       * same coordinates, i.e at the same barycenter or centroid position.
       */
      bool operator ==( const Star& s ) const
      {
         return pos == s.pos;
      }

      /*!
       * Less-than relational operator. By default stars are sorted by
       * brightness in descending order. Stars with larger flux values come
       * first in a sorted list.
       */
      bool operator <( const Star& s ) const
      {
         return flux > s.flux;
      }

      /*!
       * Array subscript operator for coordinate selection, compatible with
       * pcl::QuadTree.
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
    * Number of (dyadic) wavelet layers used for structure detection.
    *
    * This parameter determines the size in pixels of the largest image
    * structures that can be detected. With more layers, larger stars (and
    * perhaps also some nonstellar objects) will be detected. The default value
    * is 5, which corresponds to structures in the scale of 32 pixels.
    *
    * \note Although the current star detection algorithm does not use a
    * wavelet transform for structure detection, this parameter is still
    * expressed as a number of dyadic wavelet layers for convenience and
    * compatibility with existing implementations.
    */
   int StructureLayers() const
   {
      return m_structureLayers;
   }

   /*!
    * Sets the number of wavelet layers used for structure detection. See
    * StructureLayers() for a description of this parameter.
    */
   void SetStructureLayers( int n )
   {
      PCL_PRECONDITION( n > 0 && n <= 8 )
      m_structureLayers = Range( n, 1, 8 );
   }

   /*!
    * Number of (dyadic) wavelet layers used for noise reduction.
    *
    * Noise reduction prevents detection of bright noise structures as false
    * stars, including hot pixels and cosmic rays. This parameter can also be
    * used to control the sizes of the smallest detected stars (increase it to
    * exclude more stars), although the <em>minimum structure size</em>
    * parameter can be more efficient for this purpose. The default value is 0,
    * which effectively disables this noise reduction feature.
    *
    * \note Although the current star detection algorithm does not use a
    * wavelet transform for structure detection, this parameter is still
    * expressed as a number of dyadic wavelet layers for convenience and
    * compatibility with existing implementations.
    */
   int NoiseLayers() const
   {
      return m_noiseLayers;
   }

   /*!
    * Sets the number of wavelet layers used for noise reduction. See
    * NoiseLayers() for a description of this parameter.
    */
   void SetNoiseLayers( int n )
   {
      PCL_PRECONDITION( n >= 0 && n <= 4 )
      m_noiseLayers = Range( n, 0, 4 );
   }

   /*!
    * Size of the hot pixel removal filter.
    *
    * This is the radius in pixels of a median filter applied by the star
    * detector before the structure detection phase. A median filter is very
    * efficient to remove hot pixels. To disable hot pixel removal, set this
    * parameter to zero. The default value is 1 pixel.
    */
   int HotPixelFilterRadius() const
   {
      return m_hotPixelFilterRadius;
   }

   /*!
    * Sets the size of the hot pixel removal filter. See HotPixelFilterRadius()
    * for a description of this parameter.
    */
   void SetHotPixelFilterRadius( int n )
   {
      PCL_PRECONDITION( n >= 0 && n <= 3 )
      m_hotPixelFilterRadius = Range( n, 0, 3 );
   }

   /*!
    * Size of the noise reduction filter.
    *
    * This is the radius in pixels of a Gaussian convolution filter applied to
    * the working image used for calculation of star positions during the star
    * detection phase. Use it only for very low SNR images, where the star
    * detector cannot find reliable stars with default parameters.
    *
    * Be aware that noise reduction will modify star profiles and hence the way
    * star positions are calculated, resulting in a less accurate centroid
    * determination. Under extreme low-SNR conditions, however, this is
    * probably better than working with the actual data anyway.
    *
    * The default value is zero, which effectively disables this feature.
    */
   int NoiseReductionFilterRadius() const
   {
      return m_noiseReductionFilterRadius;
   }

   /*!
    * Sets the size in pixels of the noise reduction filter. See
    * NoiseReductionFilterRadius() for a description of this parameter.
    */
   void SetNoiseReductionFilterRadius( int n )
   {
      PCL_PRECONDITION( n >= 0 && n <= 64 )
      m_noiseReductionFilterRadius = Range( n, 0, 64 );
   }

   /*!
    * Minimum size of a detectable star structure in square pixels.
    *
    * This parameter can be used to prevent detection of small and bright image
    * artifacts as stars. This can be useful to work with uncalibrated or
    * wrongly calibrated data, especially demosaiced CFA frames where hot
    * pixels have generated large bright artifacts that cannot be removed with
    * a median filter, poorly focused images, and images with poor tracking.
    *
    * The default value is zero, which effectively disables this feature.
    */
   int MinStructureSize() const
   {
      return m_minStructureSize;
   }

   /*!
    * Sets the minimum size of a detectable star structure in square pixels.
    * See MinStructureSize() for a description of this parameter.
    */
   void SetMinStructureSize( int n )
   {
      PCL_PRECONDITION( n >= 0 )
      m_minStructureSize = Max( 0, n );
   }

   /*!
    * Star detection sensitivity.
    *
    * The sensitivity of the star detection algorithm is measured with respect
    * to the local background of each detected star. Given a star with
    * estimated brightness \e s and local background \e b, sensitivity is the
    * minimum value of (\e s - \e b)/\e b necessary to trigger star detection.
    *
    * Decrease this value to favor detection of fainter stars. Increase it to
    * restrict detection to brighter stars. The default value is 0.1.
    */
   float Sensitivity() const
   {
      return m_sensitivity;
   }

   /*!
    * Sets the star detection sensitivity. See Sensitivity() for a description
    * of this parameter.
    */
   void SetSensitivity( float s )
   {
      PCL_PRECONDITION( s >= 0.001 && s <= 1000 )
      m_sensitivity = Range( s, 0.001F, 1000.0F );
   }

   /*!
    * Star peak response.
    *
    * If you decrease this value, stars will need to have stronger (or more
    * prominent) peaks to be detected by the star detection algorithm. This is
    * useful to prevent detection of saturated stars, as well as small
    * nonstellar features. By increasing this parameter, the star detection
    * algorithm will be more sensitive to \e peakedness, and hence more
    * tolerant with relatively flat image features. The default value is 0.8.
    */
   float PeakResponse() const
   {
      return m_peakResponse;
   }

   /*!
    * Sets the star peak response. See PeakResponse() for a description of this
    * parameter.
    */
   void SetPeakResponse( float r )
   {
      PCL_PRECONDITION( r >= 0 && r <= 1 )
      m_peakResponse = Range( r, 0.0F, 1.0F );
   }

   /*!
    * Maximum star distortion.
    *
    * Star distortion is measured with respect to a perfect square, whose
    * distortion is 1. Lower values mean more distortion. The distortion of a
    * perfectly circular star is about 0.8 (actually, &pi;/4). Use this
    * parameter, if necessary, to control inclusion of elongated stars,
    * multiple stars, and nonstellar image features. The default value is 0.5.
    */
   float MaxDistortion() const
   {
      return m_maxDistortion;
   }

   /*!
    * Sets the maximum star distortion. See MaxDistortion() for a description
    * of this parameter.
    */
   void SetMaxDistortion( float d )
   {
      PCL_PRECONDITION( d >= 0 && d <= 1 )
      m_maxDistortion = Range( d, 0.0F, 1.0F );
   }

   /*!
    * Upper star detection limit in the normalized [0,1] range.
    *
    * Stars with peak values larger than this value won't be detected. This can
    * be useful to reject all stars brighter than a fixed level systematically.
    * The default value is 1.0, which effectively disables this feature.
    */
   float UpperLimit() const
   {
      return m_upperLimit;
   }

   /*!
    * Sets the upper star detection limit. See UpperLimit() for a description
    * of this parameter.
    */
   void SetUpperLimit( float u )
   {
      PCL_PRECONDITION( u >= 0 && u <= 1 )
      m_upperLimit = Range( u, 0.0F, 1.0F );
   }

   /*!
    * Returns true iff image inversion is enabled for this star detector.
    *
    * When inversion is enabled, the star detector algorithm will look for dark
    * stars over a bright background. This feature is disabled by default.
    */
   bool IsImageInversionEnabled() const
   {
      return m_invert;
   }

   /*!
    * Enables image inversion. See IsImageInversionEnabled() for a description
    * of this parameter.
    */
   void EnableImageInversion( bool enabled = true )
   {
      m_invert = enabled;
   }

   /*!
    * Disables image inversion. See IsImageInversionEnabled() for a description
    * of this parameter.
    */
   void DisableImageInversion( bool disable = true )
   {
      EnableImageInversion( !disable );
   }

   /*!
    * Returns true iff PSF fitting is enabled for this star detector.
    *
    * When PSF fitting is enabled, a point spread function of the type
    * specified by PSFType() will be fitted numerically for each detected star.
    * The Levenberg-Marquardt non-linear least squares algorithm will be used
    * to minimize the difference between the fitted function and the pixel
    * sample values in the detection region. See the PSFFit class for complete
    * information on our PSF fitting implementation.
    *
    * When PSF fitting is enabled, the \c pos data member of the
    * pcl::StarDetector::Star corresponds to the centroid of the fitted PSF
    * instead of the barycenter position calculated by the star detection
    * algorithm, and the \c signal member is a robust estimate of the mean
    * signal evaluated over the local background, also from the fitted PSF.
    *
    * PSF fitting is disabled by default.
    */
   bool IsPSFFittingEnabled() const
   {
      return m_fitPSF;
   }

   /*!
    * Enables PSF fitting for this star detector. See IsPSFFittingEnabled() for
    * a description of this parameter.
    */
   void EnablePSFFitting( bool enable = true )
   {
      m_fitPSF = enable;
   }

   /*!
    * Disables PSF fitting for this star detector. See IsPSFFittingEnabled()
    * for a description of this parameter.
    */
   void DisablePSFFitting( bool disable = true )
   {
      EnablePSFFitting( !disable );
   }

   /*!
    * Returns the type of point spread function (PSF) fitted numerically when
    * PSF fitting is enabled. See IsPSFFittingEnabled() and the pcl::PSFunction
    * namespace.
    */
   psf_function PSFType() const
   {
      return m_psfType;
   }

   /*!
    * Sets the type of point spread function (PSF) fitted numerically when PSF
    * fitting is enabled. See PSFType().
    */
   void SetPSFType( psf_function type )
   {
      m_psfType = type;
   }

   /*!
    * Returns true if elliptical point spread functions (PSFs) will be fitted
    * when PSF fitting is enabled. Returns false if circular functions will be
    * fitted. Elliptical PSFs are disabled by default.
    */
   bool IsEllipticPSF() const
   {
      return m_psfElliptic;
   }

   /*!
    * Enables elliptical PSF fits. See IsEllipticPSF().
    */
   void EnableEllipticPSF( bool enable = true )
   {
      m_psfElliptic = enable;
   }

   /*!
    * Disables elliptical PSF fits. See IsEllipticPSF().
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
      return m_psfCentroidTolerance;
   }

   /*!
    * Sets the centroid tolerance in pixels. See PSFCentroidTolerance() for a
    * description of this parameter.
    */
   void SetPSFCentroidTolerance( float t )
   {
      PCL_PRECONDITION( t >= 0 )
      m_psfCentroidTolerance = Max( 0.0F, t );
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
    *
    * \note This function is thread-safe.
    */
   star_list DetectStars( const ImageVariant& image ) const;

   /*!
    * Performs star detection with the current parameters. Returns a dynamic
    * array of Star structures sorted by brightness (flux) in descending order.
    *
    * This operator is equivalent to the DetectStars( const ImageVariant& )
    * member function.
    *
    * \note This function is thread-safe.
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
         int          m_noiseLayers = 0;
         int          m_hotPixelFilterRadius = 1;
         int          m_noiseReductionFilterRadius = 0;
         int          m_minStructureSize = 0;
         float        m_sensitivity = 0.1F;
         float        m_peakResponse = 0.8F;
         float        m_maxDistortion = 0.5F;
         float        m_upperLimit = 1.0F;
         bool         m_invert = false;
         bool         m_fitPSF = false;
         psf_function m_psfType = PSFunction::Gaussian;
         bool         m_psfElliptic = false;
         float        m_psfCentroidTolerance = 1.5F;
   const UInt8Image*  m_mask = nullptr;

private:

   star_list DetectStars( Image& image ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_StarDetector_h

// ----------------------------------------------------------------------------
// EOF pcl/StarDetector.h - Released 2022-01-18T11:02:40Z
