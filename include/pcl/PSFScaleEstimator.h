//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.3
// ----------------------------------------------------------------------------
// pcl/PSFScaleEstimator.h - Released 2023-11-23T18:44:57Z
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

#ifndef __PCL_PSFScaleEstimator_h
#define __PCL_PSFScaleEstimator_h

/// \file pcl/PSFScaleEstimator.h

#include <pcl/Defs.h>

#include <pcl/PSFEstimator.h>
#include <pcl/SurfaceSpline.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PSFScaleEstimator
 * \brief Estimation of relative image scale based on PSF photometry
 * \sa PSFEstimator, PSFSignalEstimator, StarDetector, PSFFit
 */
class PCL_CLASS PSFScaleEstimator : public PSFEstimator
{
public:

   /*!
    * Represents a point spread function type.
    */
   using psf_function = PSFEstimator::psf_function;

   using local_model = SurfaceSpline<double>;

   /*!
    * \struct pcl::PSFScaleEstimator::Estimates
    * \brief Structure to hold a PSF relative scale estimate.
    */
   struct Estimates
   {
      double      scale = 1; //!< Estimate of the mean relative scale with respect to the reference image.
      double      sigma = 0; //!< Standard deviation of the sample of scale measurements used for evaluation.
      int         total = 0; //!< Number of valid PSF fits.
      int         count = 0; //!< Number of valid PSF flux measurements used for scale evaluation.
      local_model local;     //!< 2-D model of local scale variations.

      /*!
       * Conversion to double operator.
       */
      operator double() const
      {
         return scale;
      }

      /*!
       * Returns true iff this is a valid scale estimate.
       */
      bool IsValid() const
      {
         return count > 0 && 1 + scale != 1;
      }
   };

   /*!
    * Default constructor.
    */
   PSFScaleEstimator() = default;

   /*!
    * Copy constructor.
    */
   PSFScaleEstimator( const PSFScaleEstimator& ) = default;

   /*!
    * Destroys a %PSFScaleEstimator object.
    */
   ~PSFScaleEstimator() override
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PSFScaleEstimator& operator =( const PSFScaleEstimator& ) = default;

   /*!
    * Returns the search tolerance in pixels.
    *
    * For evaluation of relative scale finding a set of matched pairs of PSF
    * measurements, i.e. matched stars, is necessary to ensure robustness of
    * the computed scale estimate. The search tolerance is the half side in
    * pixels of a square region around each fitted PSF used to find matched
    * pairs of stars by proximity search.
    *
    * In theory a very small search tolerance (about 0.01 - 0.05 pixels,
    * depending on the scale of the image) should be applicable, since the
    * reference and target images are assumed to be registered and our standard
    * image registration processes, such as StarAlignment for example, sport
    * centipixel accuracy. However, in practice we may have to deal with larger
    * registration errors, especially for wide field images if the user has not
    * defined the necessary parameters to apply distortion corrections.
    *
    * The default search tolerance is 4 pixels.
    */
   float PSFSearchTolerance() const
   {
      return m_psfSearchTolerance;
   }

   /*!
    * Sets the search tolerance in pixels. See PSFSearchTolerance() for a
    * description of this parameter.
    */
   void SetPSFSearchTolerance( float t )
   {
      PCL_PRECONDITION( t >= 0 )
      m_psfSearchTolerance = Max( 0.0F, t );
   }

   /*!
    * Returns the limit for the altered Chauvenet rejection criterion.
    *
    * A Robust Chauvenet Rejection (RCR) routine is used internally by this
    * implementation for rejection of outlier relative scale samples. The
    * larger the value of this parameter, the more samples will be rejected
    * by the RCR algorithm. The original Chauvenet rejection criterion is
    * N*P(x > |z|) &lt; 0.5, where N is the number of measurements and P()
    * represents the probability of x being more than z standard deviations
    * from the mean. This parameter alters the rejection criterion by
    * replacing 0.5 with an arbitrary limit in the [0,1] range, in order to
    * make the algorithm controllable. The default rejection limit is 0.3.
    */
   float RejectionLimit() const
   {
      return m_rejectionLimit;
   }

   /*!
    * Sets the limit for the altered Chauvenet rejection criterion. See
    * RejectionLimit() for a description of this parameter.
    */
   void SetRejectionLimit( float r )
   {
      PCL_PRECONDITION( r > 0 && r < 1 )
      m_rejectionLimit = Range( r, 0.0F, 1.0F );
   }

   /*!
    * Sets a new reference image for relative scale estimation.
    *
    * This function performs the star detection and PSF fitting tasks for the
    * specified image. The resulting set of PSF signal measurements will be
    * stored in this object for relative scale evaluation by subsequent calls
    * to EstimateScale().
    *
    * Returns the number of valid PSF signal measurements gathered for the
    * specified reference \a image.
    */
   int SetReference( const ImageVariant& image );

   /*!
    * Returns true iff generation of models of local relative scale variations
    * is enabled for this object.
    *
    * When this option is enabled, local scale variations are modeled with an
    * approximating thin plate spline during relative scale estimation. This
    * feature is disabled by default.
    */
   bool IsLocalModelEnabled() const
   {
      return m_enableLocalModel;
   }

   /*!
    * Enables generation of local scale variation models.
    */
   void EnableLocalModel( bool enable = true )
   {
      m_enableLocalModel = enable;
   }

   /*!
    * Disables generation of local scale variation models.
    */
   void DisableLocalModel( bool disable = true )
   {
      EnableLocalModel( !disable );
   }

   /*!
    * Evaluates the mean relative scaling factor of the currently selected
    * reference image with respect to the specified target \a image.
    *
    * A successful call to SetReference() is required to select a reference
    * image \e before calling this function; otherwise an Error exception will
    * be thrown. The specified target \a image must be accurately registered
    * with respect to the reference image.
    *
    * The returned object contains the estimated mean ratio of the reference
    * signal to the target signal, as well as the number of PSF measurements
    * used for scale evaluation. All PSF signal measurements exclude local
    * background estimates.
    *
    * \note This function is thread-safe.
    */
   Estimates EstimateScale( const ImageVariant& image ) const;

   /*!
    * Evaluates the mean relative scaling factor of the currently selected
    * reference image with respect to the specified target \a image.
    *
    * This operator is equivalent to the
    * EstimateScale( const ImageVariant& ) const member function.
    *
    * \note This function is thread-safe.
    */
   Estimates operator()( const ImageVariant& image ) const
   {
      return EstimateScale( image );
   }

private:

   Array<PSFData> m_psfReference;
   float          m_psfSearchTolerance = 4.0F; // px
   float          m_rejectionLimit = 0.3F;
   bool           m_enableLocalModel = false;

   /*
    * Auxiliary structure for generation of first-order local scale models.
    */
   struct Sample
   {
      double x, y, z;

      operator double() const
      {
         return z;
      }

      bool operator <( const Sample& s ) const
      {
         return z < s.z;
      }
   };

   using sample_vector = GenericVector<Sample>;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PSFScaleEstimator_h

// ----------------------------------------------------------------------------
// EOF pcl/PSFScaleEstimator.h - Released 2023-11-23T18:44:57Z
