//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.5
// ----------------------------------------------------------------------------
// pcl/PSFEstimator.h - Released 2024-01-13T15:47:58Z
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

#ifndef __PCL_PSFEstimator_h
#define __PCL_PSFEstimator_h

/// \file pcl/PSFEstimator.h

#include <pcl/Defs.h>

#include <pcl/StarDetector.h>
#include <pcl/PSFFit.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PSFEstimator
 * \brief Base class of estimators based on PSF photometry
 * \sa PSFSignalEstimator, PSFScaleEstimator, StarDetector, PSFFit
 */
class PCL_CLASS PSFEstimator : public ParallelProcess
{
public:

   /*!
    * Represents a point spread function type.
    */
   using psf_function = StarDetector::psf_function;

   /*!
    * Default constructor.
    */
   PSFEstimator() = default;

   /*!
    * Copy constructor.
    */
   PSFEstimator( const PSFEstimator& ) = default;

   /*!
    * Destroys a %PSFEstimator object.
    */
   ~PSFEstimator() override
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PSFEstimator& operator =( const PSFEstimator& ) = default;

   /*!
    * Returns a reference to the (immutable) internal star detector used by
    * this PSF estimator.
    *
    * PSF fitting will always be performed as a separate step after star
    * detection, so the PSF fitting parameters of the returned object are
    * meaningless for the estimation tasks implemented by derived classes.
    */
   const StarDetector& Detector() const
   {
      return const_cast<const StarDetector&>( m_starDetector );
   }

   /*!
    * Returns a reference to the (mutable) internal star detector used by this
    * PSF estimator.
    *
    * PSF fitting will always be performed as a separate step after star
    * detection, so changing the PSF fitting parameters of the returned object
    * will have no effect on the estimation tasks implemented by derived
    * classes.
    */
   StarDetector& Detector()
   {
      return m_starDetector;
   }

   /*!
    * Returns the type of point spread function (PSF) used by this estimator.
    * Elliptical PSFs are always used for PSF-based evaluation.
    *
    * This implementation supports the special PSFunction::Auto PSF type. When
    * this type is selected, a series of different PSFs will be fitted for each
    * source, and the fit that leads to the least absolute difference among
    * function values and sampled pixel values will be used for estimation.
    * Currently the following functions are tested in this special mode:
    * Gaussian, Lorentzian, and Moffat functions with beta shape parameters of
    * 1.5, 4 and 10.
    *
    * The default PSF type is PSFunction::Moffat4.
    */
   psf_function PSFType() const
   {
      return m_psfType;
   }

   /*!
    * Sets the type of point spread function used by this estimator. See
    * PSFType() for more information.
    */
   void SetPSFType( psf_function type )
   {
      m_psfType = type;
   }

   /*!
    * Returns the centroid tolerance in pixels.
    *
    * The centroid tolerance is the maximum allowed distance in pixels between
    * a detected star position (or \e barycenter) and the centroid position of
    * the corresponding fitted PSF. Stars where that distance is greater than
    * the value returned by this function will be rejected and excluded for
    * estimation. This allows preventing PSF fits departing too much from
    * detected star coordinates, which enforces robustness and stability of the
    * PSF fitting process.
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
    * Returns the saturation threshold for PSF flux measurements.
    *
    * Detected stars with one or more pixels with values above this threshold
    * will be excluded to perform the PSF estimation task.
    *
    * The returned value is expressed in the [0,1] range. It can applied either
    * as an absolute pixel sample value in the normalized [0,1] range, or as a
    * value relative to the maximum pixel sample value of the measured image.
    * See IsRelativeSaturationEnabled() for more information.
    *
    * The default saturation threshold is 0.75.
    */
   float SaturationThreshold() const
   {
      return m_saturationThreshold;
   }

   /*!
    * Sets the saturation threshold in the [0,1] range. The minimum acceptable
    * value \a t is 0.1. See SaturationThreshold() for a description of this
    * parameter.
    */
   void SetSaturationThreshold( float t )
   {
      PCL_PRECONDITION( t >= 0.1 && t <= 1.0 )
      m_saturationThreshold = Range( t, 0.1F, 1.0F );
   }

   /*!
    * The saturation threshold parameter can be applied either as an absolute
    * pixel sample value in the normalized [0,1] range, or as a value relative
    * to the maximum pixel sample value of the measured image.
    *
    * The relative saturation threshold option is enabled by default.
    */
   bool IsRelativeSaturationEnabled() const
   {
      return m_saturationRelative;
   }

   /*!
    * Enables the relative saturation threshold option. See
    * SaturationThreshold() and IsRelativeSaturationEnabled() for complete
    * information on these parameters.
    */
   void EnableRelativeSaturation( bool enable = true )
   {
      m_saturationRelative = enable;
   }

   /*!
    * Disables the relative saturation threshold option. See
    * SaturationThreshold() and IsRelativeSaturationEnabled() for complete
    * information on these parameters.
    */
   void DisableRelativeSaturation( bool disable = true )
   {
      EnableRelativeSaturation( !disable );
   }

   /*!
    * Returns the rejection limit parameter of this estimator.
    *
    * The rejection limit parameter defines an order statistic, in the [0.5,1]
    * range, used to exclude a fraction of the brightest PSF signal samples
    * during the estimation process.
    *
    * The brightest signal samples usually tend to be unreliable because of
    * relative saturation and nonlinearity. However, reliability of PSF flux
    * measurements is normally ensured by robust star detection, where the
    * source detection algorithm excludes too dim stars, and the saturation
    * threshold parameter should impose a reliable upper limit in most
    * practical cases. So this parameter should not be necessary under normal
    * working conditions.
    *
    * The default value of this parameter is 1.0, meaning that the sample of
    * PSF flux measurements is not clipped by default.
    */
   float RejectionLimit() const
   {
      return m_rejectionLimit;
   }

   /*!
    * Sets a new value of the rejection limit parameter in the [0.5,1] range.
    * See RejectionLimit() for a complete description of this parameter.
    */
   void SetRejectionLimit( float r )
   {
      PCL_PRECONDITION( r >= 0.5 && r <= 1 )
      m_rejectionLimit = Range( r, 0.5F, 1.0F );
   }

   /*!
    * Returns the growing factor for expansion/contraction of the PSF flux
    * measurement region, in units of the Full Width at Tenth Maximum (FWTM),
    * assuming a normalized PSF of unit height.
    *
    * The default value of this parameter is 1.0, meaning that flux is measured
    * exclusively for pixels within the elliptical region defined at one tenth
    * of the fitted PSF maximum.
    */
   float GrowthFactorForFluxMeasurement() const
   {
      return m_growthForFlux;
   }

   /*!
    * Sets a new value of the growing factor for the PSF flux measurement
    * region. See GrowthFactorForFluxMeasurement() for a description of this
    * parameter. The valid range for the specified factor \a k is [0.5,2.0].
    */
   void SetGrowthFactorForFluxMeasurement( float k )
   {
      PCL_PRECONDITION( k >= 0.5 && k <= 2.0 )
      m_growthForFlux = Range( k, 0.5F, 2.0F );
   }

   /*!
    * Returns the maximum number of stars that will be measured. Returns zero
    * if no limit has been set on the maximum number of PSF measurements.
    *
    * When a limit \a n greater than zero is specified, PSF-based estimation
    * will be performed for no more than the \a n brightest stars detected in
    * the target image.
    *
    * The default value is zero, meaning that no specific limit is set.
    */
   int MaxStars() const
   {
      return m_maxStars;
   }

   /*!
    * Sets the maximum number of stars that will be measured. See MaxStars()
    * for more information on this parameter.
    *
    * When a limit \a n greater than zero is specified, PSF-based estimation
    * will be performed for no more than the \a n brightest stars detected in
    * the target image. When a value &le; 0 is specified, no specific limit
    * will be set on the number of measured stars.
    */
   void SetMaxStars( int n )
   {
      PCL_PRECONDITION( n >= 0 )
      m_maxStars = Max( 0, n );
   }

   /*!
    * Returns true iff PSF weighting is enabled. When PSF weighting is enabled,
    * each photometric PSF sample is multiplied by the inverse of the mean
    * absolute deviation of the fitted PSF measured with respect to sampled
    * image data.
    *
    * PSF weighting is disabled by default.
    */
   bool IsPSFWeightingEnabled() const
   {
      return m_weighted;
   }

   /*!
    * Enables PSF weighting for this signal estimator. See
    * IsPSFWeightingEnabled() for more information.
    */
   void EnablePSFWeighting( bool enable = true )
   {
      m_weighted = enable;
   }

   /*!
    * Disables PSF weighting for this signal estimator. See
    * IsPSFWeightingEnabled() for more information.
    */
   void DisablePSFWeighting( bool disable = true )
   {
      EnablePSFWeighting( !disable );
   }

   /*!
    * Performs star detection, PSF fitting and signal evaluation for the
    * specified \a image. Returns a list of PSFData structures for all valid
    * PSF fits performed.
    *
    * Star detection and PSF fitting will be performed for the current
    * selection in the specified \a image, including its current rectangular
    * selection and currently selected channel.
    *
    * \note This function is thread-safe. It can be invoked from multiple
    * threads running concurrently.
    */
   Array<PSFData> FitStars( const ImageVariant& image ) const;

protected:

   mutable pcl::StarDetector m_starDetector;
           psf_function      m_psfType = PSFunction::Moffat4;
           float             m_psfCentroidTolerance = 1.5F;
           float             m_saturationThreshold = 0.75F;
           bool              m_saturationRelative = true;
           float             m_rejectionLimit = 1.0F;
           float             m_growthForFlux = 1.0F;
           int               m_maxStars = 0;
           bool              m_weighted = false;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PSFEstimator_h

// ----------------------------------------------------------------------------
// EOF pcl/PSFEstimator.h - Released 2024-01-13T15:47:58Z
