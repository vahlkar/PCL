//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.23
// ----------------------------------------------------------------------------
// pcl/PSFFit.h - Released 2022-03-12T18:59:29Z
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

#ifndef __PCL_PSFFit_h
#define __PCL_PSFFit_h

/// \file pcl/PSFFit.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ImageVariant.h>
#include <pcl/Matrix.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::PSFunction
 * \brief     Point spread function types.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>PSFunction::Invalid</td>      <td>Represents an invalid or unsupported PSF type.</td></tr>
 * <tr><td>PSFunction::Gaussian</td>      <td>Gaussian PSF.</td></tr>
 * <tr><td>PSFunction::Moffat</td>        <td>Moffat PSF with a fitted beta parameter.</td></tr>
 * <tr><td>PSFunction::MoffatA</td>       <td>Moffat PSF with fixed beta=10 parameter.</td></tr>
 * <tr><td>PSFunction::Moffat8</td>       <td>Moffat PSF with fixed beta=8 parameter.</td></tr>
 * <tr><td>PSFunction::Moffat6</td>       <td>Moffat PSF with fixed beta=6 parameter.</td></tr>
 * <tr><td>PSFunction::Moffat4</td>       <td>Moffat PSF with fixed beta=4 parameter.</td></tr>
 * <tr><td>PSFunction::Moffat25</td>      <td>Moffat PSF with fixed beta=2.5 parameter.</td></tr>
 * <tr><td>PSFunction::Moffat15</td>      <td>Moffat PSF with fixed beta=1.5 parameter.</td></tr>
 * <tr><td>PSFunction::Lorentzian</td>    <td>Lorentzian PSF, equivalent to a Moffat PSF with fixed beta=1 parameter.</td></tr>
 * <tr><td>PSFunction::VariableShape</td> <td>Variable shape PSF</td></tr>
 * </table>
 */
namespace PSFunction
{
   enum value_type
   {
      Invalid  = -1, // Represents an invalid or unsupported PSF type
      Gaussian = 0,  // Gaussian PSF
      Moffat,        // Moffat PSF with a fitted beta parameter
      MoffatA,       // Moffat PSF with fixed beta=10
      Moffat8,       // Moffat PSF with fixed beta=8
      Moffat6,       // Moffat PSF with fixed beta=6
      Moffat4,       // Moffat PSF with fixed beta=4
      Moffat25,      // Moffat PSF with fixed beta=2.5
      Moffat15,      // Moffat PSF with fixed beta=1.5
      Lorentzian,    // Lorentzian PSF, equivalent to a Moffat PSF with fixed beta=1
      VariableShape, // Variable shape PSF
      NumberOfFunctions,
      Auto = 999,    // Automatic selection for optimization, process-dependent.
      Default = Gaussian
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::PSFFitStatus
 * \brief     PSF fit process results.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>PSFFitStatus::Invalid</td>            <td>Represents an invalid or unsupported PSF fit status</td></tr>
 * <tr><td>PSFFitStatus::NotFitted</td>          <td>The PSF has not been fitted because the process has not been executed.</td></tr>
 * <tr><td>PSFFitStatus::FittedOk</td>           <td>PSF fitted correctly. The relative error of the solution is at most equal to the specified tolerance.</td></tr>
 * <tr><td>PSFFitStatus::BadParameters</td>      <td>The PSF fitting process failed because of improper input parameters.</td></tr>
 * <tr><td>PSFFitStatus::NoSolution</td>         <td>There is no solution with the specified parameters and source data.</td></tr>
 * <tr><td>PSFFitStatus::NoConvergence</td>      <td>The Levenberg-Marquardt algorithm did not find a valid solution after a prescribed maximum number of iterations.</td></tr>
 * <tr><td>PSFFitStatus::InaccurateSolution</td> <td>A PSF has been fitted, but the Levenberg-Marquardt algorithm couldn't find a valid solution to the specified tolerance.</td></tr>
 * <tr><td>PSFFitStatus::UnknownError</td>       <td>The PSF fitting process failed for an unspecified reason.</td></tr>
 * </table>
 */
namespace PSFFitStatus
{
   enum value_type
   {
      Invalid   = -1,
      NotFitted = 0,
      FittedOk,
      BadParameters,
      NoSolution,
      NoConvergence,
      InaccurateSolution,
      UnknownError
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class PSFData
 * \brief PSF fit parameters.
 */
struct PSFData
{
   /*!
    * Represents a point spread function type.
    */
   typedef PSFunction::value_type      psf_function;

   /*!
    * Represents a PSF fitting process status.
    */
   typedef PSFFitStatus::value_type    psf_fit_status;

   /*!
    * Represents a component of a centroid position vector (required for
    * QuadTree compatibility).
    */
   typedef DPoint::component           component;

   psf_function   function = PSFunction::Invalid;   //!< Point spread function type (PSFunction namespace).
   bool           circular = false;                 //!< Circular or elliptical PSF.
   psf_fit_status status = PSFFitStatus::NotFitted; //!< Status code (PSFFitStatus namespace).
   bool           celestial = false;   //!< True iff equatorial coordinates are available.
   double         B = 0;      //!< Local background estimate in pixel value units.
   double         A = 0;      //!< Function amplitude (or estimated maximum) in pixel value units.
   DPoint         b0 = 0.0;   //!< Barycenter position (initial star detection position) in image coordinates.
   DPoint         c0 = 0.0;   //!< Fitted centroid position in image coordinates.
   DPoint         q0 = 0.0;   //!< Centroid equatorial coordinates, when celestial=true.
   double         sx = 0;     //!< Function width in pixels on the X axis, sx >= sy.
   double         sy = 0;     //!< Function width in pixels on the Y axis, sx >= sy.
   double         theta = 0;  //!< Rotation angle of the sx axis in degrees, in the [0,180) range.
   double         beta = 0;   //!< Moffat beta or shape parameter (dimensionless).
   double         flux = 0;   //!< Total flux above the local background, measured over the rectangular fitting region.
   double         signal = 0; //!< Total flux above the local background, measured over the elliptical PSF region.
   unsigned       signalCount = 0; //!< Number of pixels used for signal evaluation.
   double         mad = 0;    /*!< Goodness of fit estimate. A robust, mean absolute difference between the
                                   fitted PSF and the sample of source image pixels over the fitting region, scaled
                                   by the total signal estimate. */
   /*!
    * Default constructor.
    */
   PSFData() = default;

   /*!
    * Copy constructor.
    */
   PSFData( const PSFData& ) = default;

   /*!
    * Move constructor.
    */
   PSFData( PSFData&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PSFData& operator =( const PSFData& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   PSFData& operator =( PSFData&& ) = default;

   /*!
    * Returns true iff this object contained valid PSF fitting parameters. This
    * happens (or \e should happen, under normal conditions) when the status
    * data member is equal to PSFFitStatus::FittedOk.
    */
   operator bool() const
   {
      return status == PSFFitStatus::FittedOk;
   }

   /*!
    * Conversion to double operator. Returns the PSF signal estimate if
    * nonzero, or the total flux otherwise. Useful for sorting and statistics
    * calculations.
    */
   operator double() const
   {
      return (signal != 0) ? signal : flux;
   }

   /*!
    * Returns a centroid coordinate. Returns the X centroid coordinate if the
    * specified index \a i is zero, or the Y coordinate otherwise.
    *
    * \note This operator provides compatibility with the QuadTree class.
    */
   double operator []( int i ) const noexcept
   {
      return (i == 0) ? c0.x : c0.y;
   }

   /*!
    * Returns the name of the fitted PSF function. See the PSFunction namespace
    * for supported functions.
    */
   String FunctionName() const;

   /*!
    * Returns a string with a brief description of the current fitting status.
    */
   String StatusText() const;

   /*!
    * Returns the full width at half maximum (FWHM) on the X axis in pixels.
    *
    * For an elliptic PSF, the X axis corresponds to the orientation of the
    * major function axis as projected on the image.
    *
    * For a circular PSF, FWHMx() and FWHMy() are equivalent.
    */
   double FWHMx() const
   {
      return FWHM( function, sx, beta );
   }

   /*!
    * Returns the full width at half maximum (FWHM) on the Y axis in pixels.
    *
    * For an elliptic PSF, the Y axis corresponds to the orientation of the
    * minor function axis as projected on the image.
    *
    * For a circular PSF, FWHMx() and FWHMy() are equivalent.
    */
   double FWHMy() const
   {
      return FWHM( function, sy, beta );
   }

   /*!
    * Returns the full width at tenth maximum (FWTM) on the X axis in pixels.
    *
    * For an elliptic PSF, the X axis corresponds to the orientation of the
    * major function axis as projected on the image.
    *
    * For a circular PSF, FWTMx() and FWTMy() are equivalent.
    */
   double FWTMx() const
   {
      return FWTM( function, sx, beta );
   }

   /*!
    * Returns the full width at tenth maximum (FWTM) on the Y axis in pixels.
    *
    * For an elliptic PSF, the Y axis corresponds to the orientation of the
    * minor function axis as projected on the image.
    *
    * For a circular PSF, FWTMx() and FWTMy() are equivalent.
    */
   double FWTMy() const
   {
      return FWTM( function, sy, beta );
   }

   /*!
    * Returns the double integral of the PSF, or the estimated volume over the
    * XY plane for z > B.
    */
   double Volume() const
   {
      return A*Volume( function, sx, sy, beta );
   }

   /*!
    * Returns the PSF bounding rectangle. The coordinates of the bounding
    * rectangle \a r are calculated as follows:
    *
    * r.x0 = c0.x - d \n
    * r.y0 = c0.y - d \n
    * r.x1 = c0.x + d \n
    * r.y1 = c0.y + d \n
    *
    * where d is equal to:
    *
    * FWHMx()/2 for a circular PSF, \n
    * Max( FWHMx(), FWHMy() )/2 for an elliptic PSF.
    */
   DRect Bounds() const
   {
      double d = (circular ? FWHMx() : Max( FWHMx(), FWHMy() ))/2;
      return DRect( c0.x-d, c0.y-d, c0.x+d, c0.y+d );
   }

   /*!
    * Returns true iff this object contains valid equatorial coordinates
    * computed by an astrometric solution of the image for the centroid
    * coordinates.
    */
   bool HasCelestialCoordinates() const
   {
      return celestial;
   }

   /*!
    * Returns an image representation (in 32-bit floating point format) of the
    * fitted point spread function.
    */
   void ToImage( Image& ) const;

   /*!
    * Returns the full width at half maximum (FWHM) corresponding to a
    * supported function with the specified parameters.
    *
    * \param function   The type of point spread function. See the PSFunction
    *                   namespace for supported functions.
    *
    * \param sigma      Estimated function width.
    *
    * \param beta       Moffat beta or VariableShape shape parameter.
    *                   Must be > 0.
    *
    * The returned value is the FWHM in sigma units, or zero if an invalid or
    * unsupported function type has been specified.
    */
   static double FWHM( psf_function function, double sigma, double beta = 2 )
   {
      PCL_PRECONDITION( beta > 0 )
      switch ( function )
      {
      case PSFunction::Gaussian:      return 2.3548200450309493 * sigma;
      case PSFunction::Moffat:        return 2 * sigma * Sqrt( Pow2( 1/beta ) - 1 );
      case PSFunction::MoffatA:       return 0.5358113941912513 * sigma;
      case PSFunction::Moffat8:       return 0.6016900619596693 * sigma;
      case PSFunction::Moffat6:       return 0.6998915581984769 * sigma;
      case PSFunction::Moffat4:       return 0.8699588840921645 * sigma;
      case PSFunction::Moffat25:      return 1.1305006161394060 * sigma;
      case PSFunction::Moffat15:      return 1.5328418730817597 * sigma;
      case PSFunction::Lorentzian:    return 2 * sigma;
      case PSFunction::VariableShape: return 2 * sigma * Pow( beta*0.6931471805599453, 1/beta );
      default:                        return 0; // ?!
      }
   }

   /*!
    * Returns the full width at tenth maximum (FWTM) corresponding to a
    * supported function with the specified parameters.
    *
    * \param function   The type of point spread function. See the PSFunction
    *                   namespace for supported functions.
    *
    * \param sigma      Estimated function width.
    *
    * \param beta       Moffat beta or VariableShape shape parameter.
    *                   Must be > 0.
    *
    * The returned value is the FWTM in sigma units, or zero if an invalid or
    * unsupported function type has been specified.
    */
   static double FWTM( psf_function function, double sigma, double beta = 2 )
   {
      PCL_PRECONDITION( beta > 0 )
      switch ( function )
      {
      case PSFunction::Gaussian:      return 4.291932052578694 * sigma;
      case PSFunction::Moffat:        return 2 * sigma * Sqrt( Pow( 10.0, 1/beta ) - 1 );
      case PSFunction::MoffatA:       return 1.017694279819175 * sigma;
      case PSFunction::Moffat8:       return 1.155026289161115 * sigma;
      case PSFunction::Moffat6:       return 1.367917055412454 * sigma;
      case PSFunction::Moffat4:       return 1.764402913213331 * sigma;
      case PSFunction::Moffat25:      return 2.459175822514185 * sigma;
      case PSFunction::Moffat15:      return 3.816589489904712 * sigma;
      case PSFunction::Lorentzian:    return 6 * sigma;
      case PSFunction::VariableShape: return 2 * sigma * Pow( beta*2.302585092994045, 1/beta );
      default:                        return 0; // ?!
      }
   }

   /*!
    * Returns the double integral of a supported elliptical function with the
    * specified parameters.
    *
    * \param function   The type of point spread function. See the PSFunction
    *                   namespace for supported functions.
    *
    * \param sigma_x    Estimated function width on the X axis.
    *
    * \param sigma_y    Estimated function width on the Y axis.
    *
    * \param beta       Moffat beta or VariableShape shape parameter.
    *                   Must be > 0. Must be > 1 for Moffat and Lorentzian
    *                   functions.
    *
    * The returned value is the volume of the PSF over the XY plane, or zero if
    * an invalid or unsupported function type has been specified, or if the
    * \a beta parameter is invalid for the type of PSF specified.
    */
   static double Volume( psf_function function, double sigma_x, double sigma_y, double beta = 2 )
   {
      PCL_PRECONDITION( beta > 0 )
      PCL_PRECONDITION( function != PSFunction::Lorentzian && (function != PSFunction::Moffat || beta > 1) )
      switch ( function )
      {
      case PSFunction::Gaussian: return 6.2831853071795862 * sigma_x*sigma_y;
      case PSFunction::Moffat:   return 3.1415926535897931 * sigma_x*sigma_y/(beta - 1);
      case PSFunction::MoffatA:  return 0.3490658503988659 * sigma_x*sigma_y;
      case PSFunction::Moffat8:  return 0.4487989505128276 * sigma_x*sigma_y;
      case PSFunction::Moffat6:  return 0.6283185307179586 * sigma_x*sigma_y;
      case PSFunction::Moffat4:  return 1.0471975511965976 * sigma_x*sigma_y;
      case PSFunction::Moffat25: return 2.0943951023931953 * sigma_x*sigma_y;
      case PSFunction::Moffat15: return 6.2831853071795862 * sigma_x*sigma_y;
      default:                   return 0; // ?!
      }
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class PSFFit
 * \brief Numerical Point Spread Function (PSF) fit to a source in an image.
 * \sa StarDetector, PSFSignalEstimator, PSFScaleEstimator
 */
class PSFFit
{
public:

   /*!
    * Represents a point spread function type.
    */
   typedef PSFData::psf_function    psf_function;

   /*!
    * Represents a PSF fitting process status.
    */
   typedef PSFData::psf_fit_status  psf_fit_status;

   /*!
    * Fitted PSF parameters.
    */
   PSFData psf;

   /*!
    * Fits a point spread function to a source in the specified \a image.
    *
    * \param image            The source image.
    *
    * \param center           The intial search point in image coordinates. For
    *                         consistent results, these coordinates must be the
    *                         result of a robust object detection process.
    *
    * \param rect             The PSF sampling region in image coordinates.
    *                         This is the rectangular region of the image where
    *                         the function fitting process will take place. PSF
    *                         parameters will be evaluated from source pixel
    *                         values acquired exclusively from this region.
    *
    * \param function         The point spread function type to be fitted. A
    *                         Gaussian PSF is fitted by default.
    *
    * \param circular         Whether to fit a circular or an elliptical PSF.
    *                         Elliptical functions are fitted by default.
    *
    * \param betaMin,betaMax  The range of shape parameter values when
    *                         \a function is PSFunction::VariableShape; ignored
    *       for other point spread functions. When the values of these
    *       parameters are such that \a betaMin &lt; \a betaMax, an optimal
    *       value of the beta (shape) PSF parameter will be searched for
    *       iteratively within the specified range. The shape parameter will be
    *       optimized for minimization of the absolute difference between the
    *       estimated PSF and the sample of source image pixels. When
    *       \a betaMin &ge; \a betaMax, the shape parameter will stay constant
    *       and equal to the specified \a betaMin value during the entire PSF
    *       fitting process, and the rest of PSF parameters will be estimated
    *       accordingly. The valid range of beta parameter values is [1.0,6.0].
    *       Values outside this range may lead to numerically unstable PSF
    *       fitting processes.
    *
    * \param tolerance        Tolerance of the Levenberg-Marquardt algorithm.
    *                         The default value is 1.0e-08. The valid range is
    *                         from 1.0e-12 to 0.001.
    *
    * \param bkgMaxVar        Maximum relative difference allowed between the
    *                         fitted local background (\e B PSF parameter) and
    *       the initial local background estimated as the median pixel sample
    *       value of the sampling region. The default value is 0.1, meaning
    *       that a maximum relative difference of a 10% will be allowed. This
    *       parameter is useful to enforce stability of the PSF fitting
    *       process: assuming that the median pixel sample value is a robust
    *       estimate of the local background, by constraining the range of
    *       variation of the \e B parameter the PSF fitting process can be more
    *       accurate and robust.
    *
    * \param growthForFlux    Growing factor in units of the Full Width at
    *                         Tenth Maximum (FWTM) for extension/contraction of
    *                         the PSF flux measurement region. The default
    *                         value is 1.0.
    *
    * The implementation of the Levenberg-Marquardt algorithm used internally
    * by this function is extremely sensitive to the specified \a center and
    * \a rect parameters. These starting parameters should always be
    * calculated using robust procedures to achieve consistent results.
    *
    * In the most frequent use case, where a star detection procedure is
    * typically used to obtain these starting parameters, robustness to poorly
    * sampled data and resilience to outlier pixels and noise are particularly
    * important to this task.
    */
   PSFFit( const ImageVariant& image,
           const DPoint& center, const DRect& rect,
           psf_function function = PSFunction::Gaussian, bool circular = false,
           float betaMin = 1.0F, float betaMax = 4.0F,
           double tolerance = 1.0e-08, float bkgMaxVar = 0.1F, float growthForFlux = 1.0F );

   /*!
    * Copy constructor.
    */
   PSFFit( const PSFFit& ) = default;

   /*!
    * Move constructor.
    */
   PSFFit( PSFFit&& ) = default;

   /*!
    * Copy assignment opèrator. Returns a reference to this object.
    */
   PSFFit& operator =( const PSFFit& x ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   PSFFit& operator =( PSFFit&& x ) = default;

   /*!
    * Returns true iff this object contains a valid PSF fit, i.e. valid PSF
    * fitted parameters.
    */
   operator bool() const
   {
      return psf;
   }

private:

   Matrix S; // matrix of sampled pixel data
   Vector P; // vector of function parameters

   // The initial local background measured on the sampling region and the
   // maximum allowed relative difference, for stabilization of local
   // background PSF parameters.
   double m_bkg;
   float  m_bkgMaxVar;

   // Growing factor in units of the Full Width at Tenth Maximum (FWTM) for
   // extension/contraction of the PSF flux measurement region.
   float  m_growthForFlux = 1.0F;

   // Keep track of successive beta values in L-M iterations for stabilization
   // of shape parameters. This guarantees convergence for Moffat functions.
   mutable float m_beta;

   Vector GoodnessOfFit( psf_function, bool circular, bool test = false ) const;

   friend class PSFFitEngine;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PSFFit_h

// ----------------------------------------------------------------------------
// EOF pcl/PSFFit.h - Released 2022-03-12T18:59:29Z
