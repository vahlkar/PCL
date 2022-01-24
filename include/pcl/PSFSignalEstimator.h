//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.19
// ----------------------------------------------------------------------------
// pcl/PSFSignalEstimator.h - Released 2022-01-24T22:43:24Z
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
   typedef PSFEstimator::psf_function  psf_function;

   /*!
    * \struct pcl::PSFSignalEstimator::Estimates
    * \brief Structure to hold signal estimates.
    */
   struct Estimates
   {
      double mean = 0;      //!< Estimate of the mean signal.
      double power = 0;     //!< Estimate of the mean squared signal.
      double flux = 0;      //!< Total PSF flux.
      double powerFlux = 0; //!< Total squared PSF .
      int    count = 0;     //!< Number of valid PSF signal estimates.

      /*!
       * Conversion to double operator.
       */
      operator double() const
      {
         return mean;
      }

      /*!
       * Returns true iff this object contains valid signal estimates.
       */
      bool IsValid() const
      {
         return count > 0 && 1 + mean != 1 && 1 + flux != 1;
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
    * Returns the high clipping point of this signal estimator.
    *
    * Bright pixels are rejected for calculation of a robust penalty function
    * applied to compute PSF signal estimates representative of the whole
    * image, not just of the measured stars. The high clipping point parameter
    * defines an order statistic, in the [0.5,1] range, used as a rejection
    * limit to exclude bright image structures, including most outliers such as
    * cosmics, plane and satellite trails, uncorrected hot pixels, etc. For
    * example, by setting this parameter to 0.5 all pixels above the median of
    * the image would be rejected. The default high clipping point is 0.85,
    * which works correctly in most practical cases.
    */
   float HighClippingPoint() const
   {
      return m_clipHigh;
   }

   /*!
    * Sets the high clipping point for this signal estimator. The valid range
    * is [0.5,1.0], whose boundaries correspond to the median and the maximum
    * pixel sample values of the target image, respectively.
    *
    * See HighClippingPoint() for a description of this parameter.
    */
   void SetHighClippingPoint( float x )
   {
      m_clipHigh = Range( x, 0.5F, 1.0F );
   }

   /*!
    * Evaluates the mean values of the signal and the signal power for the
    * specified \a image. Returns the estimates as a new
    * PSFSignalEstimator::Estimates object.
    *
    * \note This function is thread-safe.
    */
   Estimates EstimateSignal( const ImageVariant& image ) const;

   /*!
    * Evaluates the mean value of the signal and the signal power for the
    * specified \a image. Returns the estimates as a new
    * PSFSignalEstimator::Estimates object.
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

private:

   float m_clipHigh = 0.85F;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PSFSignalEstimator_h

// ----------------------------------------------------------------------------
// EOF pcl/PSFSignalEstimator.h - Released 2022-01-24T22:43:24Z
