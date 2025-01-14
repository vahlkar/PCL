//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/PSFSignalEstimator.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_PSFSignalEstimator_h
#define __PCL_PSFSignalEstimator_h

/// \file pcl/PSFSignalEstimator.h

#include <pcl/Defs.h>

#include <pcl/PSFEstimator.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PSFSignalEstimator
 * \brief Signal estimation based on PSF photometry
 * \sa PSFEstimator, PSFScaleEstimator, StarDetector, PSFFit
 */
class PCL_CLASS PSFSignalEstimator : public PSFEstimator
{
public:

   /*!
    * Represents a point spread function type.
    */
   using psf_function = PSFEstimator::psf_function;

   /*!
    * \struct pcl::PSFSignalEstimator::Estimates
    * \brief Structure to hold PSF flux and robust mean background estimates.
    */
   struct Estimates
   {
      double totalFlux = 0;          //!< The sum of PSF flux estimates.
      double totalPowerFlux = 0;     //!< The sum of squared PSF flux estimates.
      double totalMeanFlux = 0;      //!< The sum of mean PSF flux estimates.
      double totalMeanPowerFlux = 0; //!< The sum of mean squared PSF flux estimates.
      double MStar = 0;              //!< The M* robust mean background estimate.
      double NStar = 0;              /*!< The N* robust estimate of the standard deviation of the noise.
                                          As generated by the EstimateSignal() member function, this
                                          estimate is already multiplied by the normalization constant
                                          2.05435, which makes it consistent with the standard deviation
                                          of a normal distribution. */
      int    count = 0;              //!< Number of valid PSF flux estimates.

      /*!
       * Conversion to double operator.
       */
      operator double() const
      {
         return totalFlux;
      }

      /*!
       * Returns true iff this object contains valid PSF flux estimates.
       */
      bool IsValid() const
      {
         return count > 0 && 1 + totalFlux != 1 && 1 + MStar != 1;
      }
   };

   /*!
    * Default constructor.
    */
   PSFSignalEstimator()
   {
      // Override default PSF estimator parameters with appropriate values for
      // signal evaluation.
      m_saturationThreshold = 1.0F;
      m_saturationRelative = false;
   }

   /*!
    * Copy constructor.
    */
   PSFSignalEstimator( const PSFSignalEstimator& ) = default;

   /*!
    * Destroys a %PSFSignalEstimator object.
    */
   ~PSFSignalEstimator() override
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PSFSignalEstimator& operator =( const PSFSignalEstimator& ) = default;

   /*!
    * Returns the scale for local background model generation in pixels.
    *
    * The implemented algorithm uses nonlinear multiscale transforms to
    * generate a model of the large-scale background component of the image.
    * This is necessary to calculate robust estimates of the mean background
    * level and the standard deviation of the noise, M* and N* respectively.
    *
    * This parameter defines the dimensional scale in pixels for isolation of
    * large-scale image structures. The current implementation supports the
    * following scale parameter values in pixels: 1024, 768, 512, 384, 256,
    * 192, 128, 64 and 32. The default scale is 256 pixels.
    */
   int BackgroundModelScale() const
   {
      return m_scale;
   }

   /*!
    * Sets the scale for local background model generation in pixels.
    *
    * See the BackgroundModelScale() member function for a description of this
    * parameter.
    */
   void SetBackgroundModelScale( int scale )
   {
      PCL_PRECONDITION( scale >= 32 && scale <= 1024 )
      m_scale = Range( scale, 32, 1024 );
   }

   /*!
    * Evaluates the total and mean PSF flux and square flux, along with the M*
    * and N* estimates of mean background level and noise standard deviation,
    * for the selected channel of the specified \a image. Returns the estimates
    * as a new PSFSignalEstimator::Estimates object.
    *
    * \note This function is thread-safe.
    */
   Estimates EstimateSignal( const ImageVariant& image ) const;

   /*!
    * Evaluates the total and mean PSF flux and square flux, along with the M*
    * and N* estimates of mean background level and noise standard deviation,
    * for the selected channel of the specified \a image. Returns the estimates
    * as a new PSFSignalEstimator::Estimates object.
    *
    * This operator is equivalent to the EstimateSignal( const ImageVariant& )
    * member function.
    *
    * \note This function is thread-safe.
    */
   Estimates operator()( const ImageVariant& image ) const
   {
      return EstimateSignal( image );
   }

   /*!
    * Extracts the set of residual pixels from a large-scale local background
    * model.
    *
    * \param image   The target image, from which the set of residual pixels
    *                will be evaluated and extracted. The task will work
    *                exclusively on the selected channel of the image, ignoring
    *                region selections.
    *
    * \param scale   The dimensional \a scale in pixels. This is the scale of
    *                the generated local background model used for detection of
    *                non-significant and residual pixels. The current
    *                implementation supports the following scales for
    *                large-scale local background modeling: 512, 384, 256, 192,
    *                128, 64 and 32 pixels. The default scale is 256 pixels.
    *
    * \param maxThreads    If specified, the task will use at most this number
    *                of parallel execution threads. Otherwise, or if a value
    *                &le; 0 is specified, the number of threads will be set
    *                according to current platform settings.
    *
    * The mean (or median) and standard deviation of the returned residual are
    * the M* and N* robust estimators, respectively. M* estimates the mean
    * background level, and N* the standard deviation of the noise.
    */
   static Array<float> LocalBackgroundResidual( const ImageVariant& image, int scale = 256, int maxThreads = 0 );

   /*!
    * Returns the normalized N* robust estimator of the standard deviation of
    * the noise for the specified local background residual \a R. This function
    * computes the normalized N* variant with the MAD scale estimator:
    *
    * N* = 2.48308 * MAD( R )
    *
    * The type \a T must be a direct container with PCL random access container
    * semantics, such as Vector or Array. You can use the output of
    * LocalBackgroundResidual() directly to call this function.
    */
   template <class T>
   static double NStar_MAD( const T& R )
   {
      return 2.48308 * pcl::MAD( R.Begin(), R.End() );
   }

   /*!
    * Returns the normalized N* robust estimator of the standard deviation of
    * the noise for the specified local background residual \a R. This function
    * computes the normalized N* variant with the Sn scale estimator of
    * Rousseeuw and Croux:
    *
    * N* = 2.03636 * Sn( R )
    *
    * The type \a T must be a direct container with PCL random access container
    * semantics, such as Vector or Array. You can use the output of
    * LocalBackgroundResidual() directly to call this function.
    *
    * \note This is a \e destructive algorithm: it may alter the initial order
    * of items in the specified \a R container.
    */
   template <class T>
   static double NStar_Sn( T& R )
   {
      return 2.03636 * pcl::Sn( R.Begin(), R.End() );
   }

   /*!
    * Returns the normalized N* robust estimator of the standard deviation of
    * the noise for the specified local background residual \a R. This function
    * computes the normalized N* variant with the Sn scale estimator of
    * Rousseeuw and Croux. It is equivalent to NStar_Sn( R ).
    *
    * \note This is a \e destructive algorithm: it may alter the initial order
    * of items in the specified \a R container.
    */
   template <class T>
   static double NStar( T& R )
   {
      return NStar_Sn( R );
   }

   /*!
    * The PSF Signal Weight (PSFSW) image quality estimator.
    *
    * \param E       PSF flux and robust mean background and noise estimates.
    *                The estimates held by this object should be the result of
    *                a previous call to the EstimateSignal() member function,
    *                or be assigned with values retrieved from the appropriate
    *                image metadata items.
    *
    * \param sigmaN  Estimate of the standard deviation of the noise. If no
    *                value is specified for this parameter, or if a value
    *                &le; 0 is specified, the N* robust noise estimate held by
    *                the specified \a E structure will be used.
    */
   static double PSFSignalWeight( const Estimates& E, double sigmaN = 0 )
   {
      if ( sigmaN <= 0 )
         sigmaN = E.NStar;
      return (5.326e-6 * E.totalFlux * E.totalMeanFlux) / (9.0e+6 * sigmaN * E.MStar);
   }

   /*!
    * The PSF Signal to Noise Ratio (PSFSNR) estimator.
    *
    * \param E       PSF flux and robust mean background and noise estimates.
    *                The estimates held by this object should be the result of
    *                a previous call to the EstimateSignal() member function,
    *                or be assigned with values retrieved from the appropriate
    *                image metadata items.
    *
    * \param sigmaN  Estimate of the standard deviation of the noise. If no
    *                value is specified for this parameter, or if a value
    *                &le; 0 is specified, the N* robust noise estimate held by
    *                the specified \a E structure will be used.
    */
   static double PSFSNR( const Estimates& E, double sigmaN = 0 )
   {
      if ( sigmaN <= 0 )
         sigmaN = E.NStar;
      return (1.316e-7 * E.totalFlux * E.totalFlux) / (4.987e+6 * sigmaN * sigmaN);
   }

private:

   int m_scale = 256;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PSFSignalEstimator_h

// ----------------------------------------------------------------------------
// EOF pcl/PSFSignalEstimator.h - Released 2024-12-28T16:53:48Z
