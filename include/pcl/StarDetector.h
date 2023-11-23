//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.3
// ----------------------------------------------------------------------------
// pcl/StarDetector.h - Released 2023-11-23T18:44:57Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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
   using psf_function = PSFFit::psf_function;

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
      using component = DPoint::component;

      DPoint pos = 0.0;     /*!< Star position in image coordinates. Corresponds to the centroid of
                                 the fitted PSF when PSF fitting is enabled; to the barycenter
                                 calculated by the star detection algorithm otherwise. */
      Rect   rect = 0;      //!< Bounding rectangle of the detection region in pixels, image coordinates.
      Rect   srect = 0;     //!< Sampling region in pixels, image coordinates.
      float  area = 0;      //!< Area of the detected source structure in square pixels.
      float  flux = 0;      //!< Total flux above local background.
      float  signal = 0;    /*!< Estimated mean signal over the local background within the PSF
                                 fitting region. Nonzero only when PSF fitting is enabled. */
      float  mad = 0;       /*!< Robust average absolute deviation of the fitted PSF with respect
                                 to sampled image data. */
      Star*  ref = nullptr; //!< Referenced star, useful for cross-referencing applications.

      /*!
       * Default constructor.
       */
      Star() = default;

      /*!
       * Constructs a new %Star object with the specified position \a p,
       * detection region \a r, sampling region \a sr, area \a a, total flux
       * \a f, mean signal \a s, and PSF average deviation \a m.
       */
      Star( const DPoint& p, const Rect& r, const Rect& sr = 0, float a = 0, float f = 0, float s = 0, float m = 0 )
         : pos( p ), rect( r ), srect( sr ), area( a ), flux( f ), signal( s ), mad( m )
      {
      }

      /*!
       * Constructs a new %Star object with the specified position \a p in
       * image coordinates and total flux \a f.
       */
      Star( const DPoint& p, float f = 0 )
         : pos( p ), flux( f )
      {
      }

      /*!
       * Constructs a new %Star object with the specified image coordinates
       * \a x, \a y and total flux \a f.
       */
      Star( double x, double y, float f = 0 )
         : pos( x, y ), flux( f )
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
    * A dynamic array of pcl::StarDetector::Star structures.
    */
   using star_list = Array<Star>;

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
    * Destroys a %StarDetector object.
    */
   ~StarDetector() override
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
    * This parameter can be used to prevent the detection of small and bright
    * image artifacts wrongly as stars. This can be useful to work with
    * uncalibrated or poorly calibrated data, especially demosaiced CFA frames
    * where uncorrected hot pixels have generated large bright artifacts that
    * cannot be removed with a median filter, or for rejection of cosmic rays.
    *
    * This parameter can be used in three ways:
    *
    * \li <b>Automatic mode.</b> A zero value enables an adaptive algorithm
    * to find an optimal minimum structure size using statistical analysis
    * techniques. This is the default option.
    *
    * \li <b>Disabled.</b> A value of one turns off minimum structure size
    * rejection since no detectable star can be represented by less than one
    * pixel.
    *
    * \li <b>Literal value.</b> A value &gt; 1 forces using the specified
    * minimum structure size in square pixels.
    */
   int MinStructureSize() const
   {
      return m_minStructureSize;
   }

   /*!
    * Sets the minimum size \a n &ge; 0 of a detectable star structure in
    * square pixels. See MinStructureSize() for a detailed description of this
    * parameter and its possible values.
    */
   void SetMinStructureSize( int n )
   {
      PCL_PRECONDITION( n >= 0 )
      m_minStructureSize = Max( 0, n );
   }

   /*!
    * Returns the automatically calculated minimum star size in square pixels.
    *
    * The value returned by this function is only valid after a successful
    * execution of the DetectStars() function (or, equivalently, operator()())
    * with a zero minimum structure size parameter value. See the
    * MinStructureSize() member function for more information.
    */
   int MinStarSize() const
   {
      return m_minStarSize;
   }

   /*!
    * Sensitivity of the star detection device.
    *
    * Internally, the sensitivity of the star detection algorithm is expressed
    * in signal-to-noise ratio units with respect to the evaluated dispersion
    * of local background pixels for each detected structure. Given a source
    * with estimated brightness \e s, local background \e b and local
    * background dispersion \e n, sensitivity is the minimum value of
    * (\e s - \e b)/\e n necessary to trigger star detection.
    *
    * To isolate the public interface of this class from its internal
    * implementation, this parameter is normalized to the [0,1] range, where 0
    * and 1 represent minimum and maximum sensitivity, respectively. This
    * abstraction allows us to change the star detection engine without
    * breaking dependent tools and processes.
    *
    * Increase this value to favor detection of fainter stars. Decrease it to
    * restrict detection to brighter stars. The default value is 0.5.
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
      PCL_PRECONDITION( s >= 0 && s <= 1 )
      m_sensitivity = Range( s, 0.0F, 1.0F );
   }

   /*!
    * Peak sensitivity of the star detection device.
    *
    * Internally, the peak response property of the star detection algorithm is
    * expressed in kurtosis units. For each detected structure, kurtosis is
    * evaluated from all significant pixels with values greater than the
    * estimated mean local background. Peak response is the minimum value of
    * kurtosis necessary to trigger star detection.
    *
    * To isolate the public interface of this class from its internal
    * implementation, this parameter is normalized to the [0,1] range, where 0
    * and 1 represent minimum and maximum peak response, respectively. This
    * abstraction allows us to change the star detection engine without
    * breaking dependent tools and processes.
    *
    * If you decrease this parameter, stars will need to have stronger (or more
    * prominent) peaks to be detected. This is useful to prevent detection of
    * saturated stars, as well as small nonstellar features. By increasing this
    * parameter, the star detection algorithm will be more sensitive to
    * \e peakedness, and hence more tolerant with relatively flat image
    * features. The default value is 0.5.
    */
   float PeakResponse() const
   {
      return m_peakResponse;
   }

   /*!
    * Sets the peak sensitivity. See PeakResponse() for a description of this
    * parameter.
    */
   void SetPeakResponse( float r )
   {
      PCL_PRECONDITION( r >= 0 && r <= 1 )
      m_peakResponse = Range( r, 0.0F, 1.0F );
   }

   /*!
    * Minimum signal-to-noise ratio of a detectable star.
    *
    * Given a source with estimated brightness \e s, local background \e b and
    * local background dispersion \e n, SNR is evaluated as (\e s - \e b)/\e n.
    * Stars with measured SNR below this parameter won't be detected.
    *
    * The value of this parameter can be increased to limit star detection to a
    * subset of the brightest sources in the image adaptively, instead of
    * imposing an arbitrary limit on the number of detected stars. The default
    * value is zero.
    */
   float MinSNR() const
   {
      return m_minSNR;
   }

   /*!
    * Sets the minimum signal-to-noise detection level. See MinSNR() for a
    * description of this parameter.
    */
   void SetMinSNR( float snr )
   {
      PCL_PRECONDITION( snr >= 0 )
      m_minSNR = Max( 0.0F, snr );
   }

   /*!
    * Bright stars threshold.
    *
    * Stars with measured SNR above this parameter in units of the minimum
    * detection level (as defined by the sensitivity parameter) will always be
    * detected, even if their profiles are too flat for the current peak
    * response. This allows us to force inclusion of bright stars. The default
    * value is 3.0.
    */
   float BrightThreshold() const
   {
      return m_brightThreshold;
   }

   /*!
    * Sets the bright stars threshold. See BrightThreshold() for a description
    * of this parameter.
    */
   void SetBrightThreshold( float r )
   {
      PCL_PRECONDITION( r >= 1 && r <= 100 )
      m_brightThreshold = Range( r, 1.0F, 100.0F );
   }

   /*!
    * Maximum star distortion.
    *
    * Internally, star distortion is evaluated in units of coverage of a square
    * region circumscribed to each detected structure. The coverage of a
    * perfectly circular star is &pi;/4 (about 0.8). Lower values denote
    * elongated or irregular sources.
    *
    * To isolate the public interface of this class from its internal
    * implementation, this parameter is normalized to the [0,1] range, where 0
    * and 1 represent minimum and maximum distortion, respectively. This
    * abstraction allows us to change the star detection engine without
    * breaking dependent tools and processes.
    *
    * Use this parameter, if necessary, to control inclusion of elongated
    * stars, complex clusters of stars, and nonstellar image features. The
    * default value is 0.6.
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
    * Returns true iff detection of clustered sources is allowed.
    *
    * If this parameter is false, a local maxima map will be generated to
    * identify and prevent detection of multiple sources that are too close to
    * be separated as individual structures, such as double and multiple stars.
    * In general, barycenter positions cannot be accurately determined for
    * sources with several local maxima. If this parameter is true,
    * non-separable multiple sources will be detectable as single objects. The
    * default value is false.
    */
   bool IsClusteredSourcesEnabled() const
   {
      return m_allowClusteredSources;
   }

   /*!
    * Enables detection of clustered sources. See IsClusteredSourcesEnabled()
    * for a description of this parameter.
    */
   void EnableClusteredSources( bool enable = true )
   {
      m_allowClusteredSources = enable;
   }

   /*!
    * Disables detection of clustered sources. See IsClusteredSourcesEnabled()
    * for a description of this parameter.
    */
   void DisableClusteredSources( bool disable = true )
   {
      EnableClusteredSources( !disable );
   }

   /*!
    * Size of the local maxima detection filter.
    *
    * This is the radius in pixels of a structuring element used to generate
    * a local maxima map, when detection of clustered sources is disabled. The
    * default value is 2, which applies a 5x5 filter.
    */
   int LocalDetectionFilterRadius() const
   {
      return m_localDetectionFilterRadius;
   }

   /*!
    * Sets the radius in pixels of the local maxima detection filter. See
    * LocalDetectionFilterRadius() for a description of this parameter.
    */
   void SetLocalDetectionFilterRadius( int n )
   {
      PCL_PRECONDITION( n >= 1 && n <= 5 )
      m_localDetectionFilterRadius = Range( n, 1, 5 );
   }

   /*!
    * Returns the local maxima detection limit.
    *
    * This parameter is a normalized pixel value in the [0,1] range. Structures
    * with pixels above this value will be excluded for local maxima detection.
    * The default value is 0.75.
    */
   float LocalMaximaDetectionLimit() const
   {
      return m_localMaximaDetectionLimit;
   }

   /*!
    * Sets the local maxima detection limit. See LocalMaximaDetectionLimit()
    * for a description of this parameter.
    */
   void SetLocalMaximaDetectionLimit( float v )
   {
      PCL_PRECONDITION( v >= 0 && v <= 1 )
      m_localMaximaDetectionLimit = Range( v, 0.1F, 1.0F );
   }

   /*!
    * Returns true iff detection of local maxima is enabled.
    *
    * When this parameter is enabled, each reported Star object will include
    * the number of local maxima detected in the corresponding image structure,
    * irrespective of the current state of the <em>detection of clustered
    * sources</em> parameter (see IsClusteredSourcesEnabled()).
    *
    * When disabled, the number of local maxima will be zero for all reported
    * Star objects, and the <em>detection of clustered sources</em> parameter
    * will be implicitly disabled.
    *
    * The default value is true.
    */
   bool IsLocalMaximaDetectionEnabled() const
   {
      return !m_noLocalMaximaDetection;
   }

   /*!
    * Enables local maxima detection. See IsLocalMaximaDetectionEnabled() for
    * a description of this parameter.
    */
   void EnableLocalMaximaDetection( bool enable = true )
   {
      m_noLocalMaximaDetection = !enable;
   }

   /*!
    * Disables local maxima detection. See IsLocalMaximaDetectionEnabled() for
    * a description of this parameter.
    */
   void DisableLocalMaximaDetection( bool disable = true )
   {
      EnableLocalMaximaDetection( !disable );
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
         int          m_minStructureSize = 0; // automatic mode by default
 mutable int          m_minStarSize = 0; // when minStructureSize=0
         float        m_sensitivity = 0.5F;
         float        m_peakResponse = 0.5F;
         float        m_minSNR = 0.0F;
         float        m_brightThreshold = 3.0F;
         float        m_maxDistortion = 0.6F;
         bool         m_allowClusteredSources = false;
         int          m_localDetectionFilterRadius = 2;
         float        m_localMaximaDetectionLimit = 0.75F;
         bool         m_noLocalMaximaDetection = false;
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
// EOF pcl/StarDetector.h - Released 2023-11-23T18:44:57Z
