//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.15
// ----------------------------------------------------------------------------
// pcl/PSFSignalEstimator.h - Released 2021-11-25T11:44:47Z
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

#ifndef __PCL_PSFSignalEstimator_h
#define __PCL_PSFSignalEstimator_h

/// \file pcl/PSFSignalEstimator.h

#include <pcl/Defs.h>

#include <pcl/StarDetector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PSFSignalEstimator
 * \brief Estimation of mean signal based on PSF photometry
 * \sa StarDetector, PSFFit
 */
class PCL_CLASS PSFSignalEstimator : public ParallelProcess
{
public:

   /*!
    * Represents a point spread function type.
    */
   typedef StarDetector::psf_function  psf_function;

   /*!
    * \struct pcl::PSFSignalEstimator::Estimates
    * \brief Structure to hold mean signal estimates.
    */
   struct Estimates
   {
      double mean = 0;      //!< Estimate of the mean signal.
      double power = 0;     //!< Estimate of the mean squared signal.
      double meanFlux = 0;  //!< Estimate of the mean flux.
      double powerFlux = 0; //!< Estimate of the mean squared flux.
      int    count = 0;     //!< Number of valid PSF signal estimates.

      /*!
       * Conversion to double operator.
       */
      operator double() const
      {
         return mean;
      }
   };

   /*!
    * Default constructor.
    */
   PSFSignalEstimator() = default;

   /*!
    * Copy constructor.
    */
   PSFSignalEstimator( const PSFSignalEstimator& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~PSFSignalEstimator()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PSFSignalEstimator& operator =( const PSFSignalEstimator& ) = default;

   /*!
    * Returns a reference to the (immutable) internal star detector used by
    * this PSF signal estimator.
    *
    * PSF fitting will always be performed as a separate step after star
    * detection, so the PSF fitting parameters of the returned object are
    * meaningless for the signal estimation task.
    */
   const StarDetector& Detector() const
   {
      return const_cast<const StarDetector&>( m_starDetector );
   }

   /*!
    * Returns a reference to the (mutable) internal star detector used by this
    * PSF signal estimator.
    *
    * PSF fitting will always be performed as a separate step after star
    * detection, so changing the PSF fitting parameters of the returned object
    * will have no effect on the signal estimation task.
    */
   StarDetector& Detector()
   {
      return m_starDetector;
   }

   /*!
    * Returns the type of point spread function (PSF) used by this signal
    * estimator. Elliptical PSFs are always used for signal evaluation.
    *
    * The default PSF type is PSFunction::Moffat4.
    */
   psf_function PSFType() const
   {
      return m_psfType;
   }

   /*!
    * Sets the type of point spread function used by this signal estimator.
    * See PSFType() for more information.
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
    * Returns the rejection limit in sigma units.
    *
    * The rejection limit controls an iterative sigma-clipping algorithm used
    * for robust rejection of outliers during the signal estimation process.
    * Larger values favor the inclusion of more photometric PSF measurements,
    * which can improve accuracy, but at the risk of including outliers that
    * can degrade the result.
    *
    * The default rejection limit is 5 sigmas.
    */
   float RejectionLimit() const
   {
      return m_rejectionLimit;
   }

   /*!
    * Sets the rejection limit in sigma units. See RejectionLimit() for a
    * description of this parameter.
    */
   void SetRejectionLimit( float r )
   {
      PCL_PRECONDITION( r >= 0 )
      m_rejectionLimit = Max( 0.0F, r );
   }

   /*!
    * Returns the maximum number of stars that will be measured to compute mean
    * signal estimates. Returns zero if no limit has been set on the maximum
    * number of star measurements.
    *
    * When a limit \a n greater than zero is specified, PSF photometry will be
    * performed for no more than the \a n brightest stars detected in the
    * target image.
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
    * When a limit \a n greater than zero is specified, PSF photometry will be
    * performed for no more than the \a n brightest stars detected in the
    * target image. When a value &le; 0 is specified, no specific limit will be
    * set on the number of measured stars.
    */
   void SetMaxStars( int n )
   {
      PCL_PRECONDITION( n >= 0 )
      m_maxStars = Max( 0, n );
   }

   /*!
    * Returns true iff PSF weighting is enabled for signal estimation. When PSF
    * weighting is enabled, each photometric PSF sample is multiplied by the
    * inverse of the mean absolute deviation of the fitted PSF measured with
    * respect to sampled image data.
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
    * Evaluates the mean values of the signal and the signal power for the
    * specified \a image. Returns the estimates as a new
    * PSFSignalEstimator::Estimates object.
    */
   Estimates EstimateSignal( const ImageVariant& image ) const;

   /*!
    * Evaluates the mean value of the signal and the signal power for the
    * specified \a image. Returns the estimates as a new
    * PSFSignalEstimator::Estimates object.
    *
    * This operator is equivalent to the EstimateSignal( const ImageVariant& )
    * member function.
    */
   Estimates operator()( const ImageVariant& image ) const
   {
      return EstimateSignal( image );
   }

private:

   mutable pcl::StarDetector m_starDetector;
           psf_function      m_psfType = PSFunction::Moffat4;
           float             m_psfCentroidTolerance = 1.5F;
           float             m_rejectionLimit = 5.0F;
           int               m_maxStars = 0;
           bool              m_weighted = false;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PSFSignalEstimator_h

// ----------------------------------------------------------------------------
// EOF pcl/PSFSignalEstimator.h - Released 2021-11-25T11:44:47Z
