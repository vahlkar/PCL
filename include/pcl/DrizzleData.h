//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/DrizzleData.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_DrizzleData_h
#define __PCL_DrizzleData_h

/// \file pcl/DrizzleData.h

#include <pcl/Defs.h>

#include <pcl/File.h>
#include <pcl/GridInterpolation.h>
#include <pcl/MultiVector.h>
#include <pcl/SurfaceSpline.h>
#include <pcl/TimePoint.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::DrizzleParserOption
 * \brief     %Drizzle data parsing options
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>DrizzleParserOption::IgnoreIntegrationData</td>
 *     <td>All drizzle data relative to the image integration task will be
 * ignored. This includes statistical location and scale estimates for image
 * normalization, image weights, and pixel rejection data.</td></tr>
 * <tr><td>DrizzleParserOption::RequireIntegrationData</td>
 *     <td>Fail if a .xdrz file does not include integration data.</td></tr>
 * </table>
 */
namespace DrizzleParserOption
{
   enum mask_type
   {
      IgnoreIntegrationData  = 0x00000001,
      RequireIntegrationData = 0x00000002,
   };
}

/*!
 * \class pcl::DrizzleParserOptions
 * \brief A collection of %Drizzle data parsing options
 */
using DrizzleParserOptions = Flags<DrizzleParserOption::mask_type>;

// ----------------------------------------------------------------------------

class PCL_CLASS XMLDocument;
class PCL_CLASS XMLElement;

/*!
 * \class DrizzleData
 * \brief Drizzle integration data parser and generator
 *
 * %DrizzleData implements support for the %XML drizzle data format (XDRZ
 * format, .xdrz file suffix).
 *
 * A drizzle image integration process has been formalized as the following
 * chain of subtasks on the PixInsight/PCL platform:
 *
 * \li 1. Demosaicing. Only required if the input data set has been mosaiced
 * with a color filter array (CFA), such as a Bayer or X-Trans filter.
 *
 * \li 2. Image registration. Generates image alignment information in the form
 * of alignment matrices (projective transformations) and/or two-dimensional
 * surface splines for arbitrary distortion correction.
 *
 * \li 3. Image integration. Generates statistical data for each channel of the
 * integrated image. This includes estimates of location and scale, statistical
 * weights and pixel rejection data.
 *
 * \li 4. Drizzle integration. The input data for the drizzle algorithm is the
 * input image for subtasks 1 (CFA drizzle) or 2 (normal drizzle). The
 * information generated by subtasks 2 and 3 is used to perform a drizzle image
 * integration process by inverse coordinate projection with image weighting,
 * image normalization, and pixel rejection.
 *
 * An XDRZ file stores all the data generated by the above drizzle integration
 * subtasks as an %XML document. The %DrizzleData class is a high-level
 * interface for parsing and generation of these special files.
 *
 * This class also implements an internal read-only compatibility layer with
 * the old plain text drizzle data format (.drz file suffix) used by versions
 * of PixInsight before the 1.8.5 standard distribution. By using the class
 * constructor or the Parse() member function to load and parse an existing
 * file, the correct file format is detected automatically.
 *
 * \sa LocalNormalizationData, FluxCalibrationData
 */
class PCL_CLASS DrizzleData
{
public:

   /*!
    * Represents a vector surface spline interpolation/approximation in two
    * dimensions. This class is used to implement image alignment with
    * arbitrary distortion correction.
    */
   using vector_spline = PointSurfaceSpline;

   /*!
    * Represents a coordinate interpolating/approximating surface spline used
    * for image registration with arbitrary distortion correction.
    */
   using spline = vector_spline::spline;

   /*!
    * An ordered list of image coordinates. Used to store reference image
    * coordinates or image displacements in distortion models. Also used to
    * store interpolation node coordinates for adaptive normalization.
    */
   using point_list = Array<DPoint>;

   /*!
    * An ordered list of statistical weights corresponding to points in a
    * distortion model.
    */
   using weight_vector = Array<float>;

   /*!
    * Default constructor. Constructs an empty %DrizzleData object.
    */
   DrizzleData() = default;

   /*!
    * Copy constructor.
    */
   DrizzleData( const DrizzleData& ) = default;

   /*!
    * Move constructor.
    */
   DrizzleData( DrizzleData&& ) = default;

   /*!
    * Constructs a new %DrizzleData instance by loading and parsing a file.
    *
    * \param filePath   Path to an existing file that will be parsed. The file
    *                   contents can be in %XML drizzle data format (normally,
    *                   a file with the .xdrz suffix), or in old plain text
    *                   format (typically with the .drz suffix). This
    *                   constructor will detect the format in use from the
    *                   first bytes of the file, and will decode it
    *                   correspondingly.
    *
    * \param options    Optional set of flags controlling the way drizzle data
    *                   are parsed and verified. See the DrizzleParserOption
    *                   namespace for more information.
    *
    * This constructor validates the data retrieved from the specified file. It
    * throws the appropriate Error exceptions in the event of parsing errors or
    * invalid data.
    */
   DrizzleData( const String& filePath, DrizzleParserOptions options = DrizzleParserOptions() )
   {
      Parse( filePath, options );
   }

   /*!
    * Constructs a new %DrizzleData instance by parsing a well-formed %XML
    * document.
    *
    * \param xml        Reference to the source %XML document. This constructor
    *                   expects an %XML document in valid %XML drizzle data
    *                   format (.xdrz).
    *
    * \param options    Optional set of flags controlling the way drizzle data
    *                   are parsed and verified. See the DrizzleParserOption
    *                   namespace for more information.
    *
    * This constructor validates the data retrieved from the specified %XML
    * document. It throws an Error exception in the event of an invalid
    * document or invalid data.
    */
   DrizzleData( const XMLDocument& xml, DrizzleParserOptions options = DrizzleParserOptions() )
   {
      Parse( xml, options );
   }

   /*!
    * Constructs a new %DrizzleData instance by parsing an %XML element.
    *
    * \param element    Reference to the source %XML element. This constructor
    *                   expects an %XML document tree in valid %XML drizzle
    *                   data format (.xdrz) rooted at this element.
    *
    * \param options    Optional set of flags controlling the way drizzle data
    *                   are parsed and verified. See the DrizzleParserOption
    *                   namespace for more information.
    *
    * This constructor validates the data retrieved from the specified %XML
    * element. It throws an Error exception in the event of invalid data.
    */
   DrizzleData( const XMLElement& element, DrizzleParserOptions options = DrizzleParserOptions() )
   {
      Parse( element, options );
   }

   /*!
    * Virtual destructor.
    */
   virtual ~DrizzleData()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   DrizzleData& operator =( const DrizzleData& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   DrizzleData& operator =( DrizzleData&& ) = default;

   /*!
    * Returns the full path to the unregistered image file corresponding to the
    * drizzle source image represented by this instance.
    *
    * The file identified by this function stores the input image of the image
    * registration task. This file can be used as input for a drizzle
    * integration task.
    *
    * \sa SetSourceFilePath()
    */
   const String& SourceFilePath() const
   {
      return m_sourceFilePath;
   }

   /*!
    * Sets the path to the unregistered image file corresponding to the drizzle
    * image represented by this instance.
    *
    * \sa SourceFilePath()
    */
   void SetSourceFilePath( const String& filePath )
   {
      m_sourceFilePath = File::FullPath( filePath );
   }

   /*!
    * Returns the full path to the mosaiced/unregistered image file
    * corresponding to the drizzle source image represented by this instance.
    *
    * The file identified by this function stores the input image of the image
    * demosaicing task. This file can be used as input for a CFA drizzle
    * integration task.
    *
    * \sa SetCFASourceFilePath(), CFASourcePattern(), CFASourceChannel()
    */
   const String& CFASourceFilePath() const
   {
      return m_cfaSourceFilePath;
   }

   /*!
    * Sets the path to the mosaiced/unregistered image file corresponding t
    * the drizzle source image represented by this instance.
    *
    * \sa CFASourceFilePath(), SetCFASourcePattern(), SetCFASourceChannel()
    */
   void SetCFASourceFilePath( const String& filePath )
   {
      m_cfaSourceFilePath = File::FullPath( filePath );
   }

   /*!
    * Returns a string representation of the color filter array (CFA) used by
    * the mosaiced/unregistered image file corresponding to the drizzle source
    * image represented by this instance.
    *
    * The CFA corresponds to the file represented by CFASourceFilePath(). For
    * the standard Bayer filters, this function returns "RGGB", "BGGR", "GBRG",
    * and "GRBG".
    *
    * \sa SetCFASourcePattern(), CFASourceChannel(), CFASourceFilePath()
    */
   const String& CFASourcePattern() const
   {
      return m_cfaSourcePattern;
   }

   /*!
    * Defines the color filter array (CFA) used by the mosaiced/unregistered
    * image file corresponding to the drizzle source image represented by this
    * instance.
    *
    * See CFASourcePattern() for more information on CFA representations.
    *
    * \sa CFASourcePattern(), SetCFASourceChannel(), SetCFASourceFilePath()
    */
   void SetCFASourcePattern( const String& cfaPattern )
   {
      m_cfaSourcePattern = cfaPattern;
   }

   /*!
    * Returns the channel index selected for the CFA mosaiced/unregistered
    * image file corresponding to the drizzle source image represented by this
    * instance.
    *
    * The returned value is the zero-based index of an image channel in a color
    * filter array (CFA) source frame, where typically 0 corresponds to red, 1
    * to green and 2 to blue. This member function returns a value &lt; 0 if no
    * channel index has been defined.
    *
    * The CFA channel index can be defined for drizzle integration of separate
    * color components working with color filter array (CFA) images. This is
    * useful for correction of non-isotropic channel misalignments, such as
    * those caused by chromatic aberration or atmospheric dispersion, by
    * computing image registration transformations with distortion corrections
    * among color components for each source frame. When a channel index has
    * been defined, the final CFA drizzle integration generates a monochrome
    * single-channel image instead of an RGB image.
    *
    * \sa SetCFASourceChannel(), CFASourceFilePath(), CFASourcePattern()
    */
   int CFASourceChannel() const
   {
      return m_cfaSourceChannel;
   }

   /*!
    * Defines the channel index selected for the CFA mosaiced/unregistered
    * image file corresponding to the drizzle source image represented by this
    * instance.
    *
    * The specified \a channel can be &lt; 0 to disable the CFA channel index
    * feature. See CFASourceChannel() for more information on CFA channel
    * indexes and their role for CFA drizzle integrations.
    *
    * \sa CFASourceChannel(), SetCFASourceFilePath(), SetCFASourcePattern()
    */
   void SetCFASourceChannel( int channel )
   {
      m_cfaSourceChannel = Range( channel, -1, int( int32_max ) );
   }

   /*!
    * Returns the full path to the registered image file corresponding to the
    * drizzle source image represented by this instance, or an empty string if
    * that file path is not available.
    *
    * The file identified by this function stores the output image of the image
    * registration task. This file can be used as input for a regular
    * (non-drizzle) integration task.
    *
    * \note This file <em>should not</em> be used as input for drizzle
    * integration, since it has already been registered and interpolated. Use
    * SourceFilePath() or CFASourceFilePath() as input for drizzle or CFA
    * drizzle, respectively.
    *
    * \sa SetAlignmentTargetFilePath()
    */
   const String& AlignmentTargetFilePath() const
   {
      return m_alignTargetFilePath;
   }

   /*!
    * Sets the path to the registered image file corresponding to the drizzle
    * image represented by this instance.
    *
    * \sa AlignmentTargetFilePath()
    */
   void SetAlignmentTargetFilePath( const String& filePath )
   {
      m_alignTargetFilePath = File::FullPath( filePath );
   }

   /*!
    * Returns the width in pixels of the registration reference image.
    *
    * \sa ReferenceHeight(), SetReferenceDimensions()
    */
   int ReferenceWidth() const
   {
      return m_referenceWidth;
   }

   /*!
    * Returns the height in pixels of the registration reference image.
    *
    * \sa ReferenceWidth(), SetReferenceDimensions()
    */
   int ReferenceHeight() const
   {
      return m_referenceHeight;
   }

   /*!
    * Sets new registration reference image dimensions in pixels, \a width and
    * \a height, respectively.
    *
    * \sa ReferenceWidth(), ReferenceHeight()
    */
   void SetReferenceDimensions( int width, int height )
   {
      m_referenceWidth = width;
      m_referenceHeight = height;
   }

   /*!
    * Returns the number of channels or pixel sample planes in the image
    * represented by this instance.
    */
   int NumberOfChannels() const
   {
      return m_location.Length();
   }

   /*!
    * Returns the origin of image alignment coordinates. The returned point
    * corresponds to image coordinates {0,0} in the coordinate system used for
    * image registration.
    *
    * \sa SetAlignmentOrigin()
    */
   const DPoint& AlignmentOrigin() const
   {
      return m_alignmentOrigin;
   }

   /*!
    * Defines the origin of image alignment coordinates.  This function is
    * reserved for image registration tasks involved in drizzle integration
    * processes.
    *
    * \note The default alignment origin coordinates are {0.5,0.5}, that is,
    * the center of the top-left pixel in the image coordinate system. These
    * coordinates ensure compatibility with old versions of drizzle data
    * serializations and standard image registration tools on the PixInsight
    * platform.
    */
   void SetAlignmentOrigin( const DPoint& p0 )
   {
      m_alignmentOrigin = p0;
   }

   /*!
    * Returns the 3x3 alignment matrix for the drizzle source image represented
    * by this instance. The returned matrix defines a projective geometric
    * transformation, also known as \e homography.
    *
    * \sa SetAlignmentMatrix()
    */
   const Matrix& AlignmentMatrix() const
   {
      return m_H;
   }

   /*!
    * Defines a new 3x3 alignment matrix. This function is reserved for image
    * registration tasks involved in drizzle integration processes.
    *
    * \sa AlignmentMatrix()
    */
   void SetAlignmentMatrix( const Matrix& H )
   {
      m_H = H;
   }

   /*!
    * Returns true iff this instance defines a 3x3 alignment matrix for the
    * projective image registration transformation.
    *
    * \sa AlignmentMatrix(), HasAlignmentSplines()
    */
   bool HasAlignmentMatrix() const
   {
      return !m_H.IsEmpty();
   }

   /*!
    * Returns the vector surface spline interpolation/approximation device in
    * two dimensions for the drizzle source image represented by this instance.
    * These splines define an image registration transformation with
    * corrections for arbitrary distortions.
    *
    * \sa SetAlignmentSplines(), SetAlignmentInverseSplines()
    */
   const vector_spline& AlignmentSplines() const
   {
      return m_S;
   }

   /*!
    * Sets a new vector surface spline interpolation/approximation device. This
    * member function is reserved for image registration tasks involved in
    * drizzle integration processes.
    *
    * \sa AlignmentSplines(), AlignmentInverseSplines()
    */
   void SetAlignmentSplines( const vector_spline& S )
   {
      m_S = S;
   }

   /*!
    * Returns true iff this instance defines a vector surface spline
    * interpolation/approximation device.
    *
    * \sa AlignmentSplines(), HasAlignmentInverseSplines(),
    * HasAlignmentMatrix()
    */
   bool HasAlignmentSplines() const
   {
      return m_S.IsValid();
   }

   /*!
    * Returns the inverse vector surface spline interpolation/approximation
    * device in two dimensions for the drizzle source image represented by this
    * instance. These splines define an inverse image registration
    * transformation with corrections for arbitrary distortions.
    *
    * \sa SetAlignmentInverseSplines(), SetAlignmentSplines()
    */
   const vector_spline& AlignmentInverseSplines() const
   {
      return m_Sinv;
   }

   /*!
    * Sets a new inverse vector surface spline interpolation/approximation
    * device. This member function is reserved for image registration tasks
    * involved in drizzle integration processes.
    *
    * \sa AlignmentInverseSplines(), AlignmentSplines()
    */
   void SetAlignmentInverseSplines( const vector_spline& Sinv )
   {
      m_Sinv = Sinv;
   }

   /*!
    * Returns true iff this instance defines an inverse vector surface spline
    * interpolation/approximation device.
    *
    * \sa AlignmentInverseSplines(), HasAlignmentSplines(),
    * HasAlignmentMatrix()
    */
   bool HasAlignmentInverseSplines() const
   {
      return m_Sinv.IsValid();
   }

   /*!
    * Stores a copy of the local distortion model in the specified arrays.
    *
    * \param[out] P1    Reference to an array of points where the function will
    *                   store a copy of the reference image coordinates of the
    *                   local distortion model.
    *
    * \param[out] D2    Reference to an array of points where the function will
    *                   store a copy of the set of pixels displacements of the
    *                   local distortion model.
    *
    * \param[out] P2    Reference to an array of points where the function will
    *                   store a copy of the reference image coordinates of the
    *                   inverse local distortion model.
    *
    * \param[out] D1    Reference to an array of points where the function will
    *                   store a copy of the set of pixels displacements of the
    *                   inverse local distortion model.
    *
    * \param[out] W     Reference to an array of scalars where the function
    *                   will store a copy of the set of statistical weights
    *                   associated with the local distortion model. This array
    *                   can be empty if the local distortion model is
    *                   unweighted.
    *
    * \param order      Reference to a variable that will be assigned the
    *                   derivability order for surface interpolation
    *                   generation. Normally this is a surface spline
    *                   derivative order.
    *
    * \param regularization   Reference to a variable that will be assigned the
    *                   recommended regularization factor for surface
    *                   interpolation generation. Normally this is a thin plate
    *                   spline smoothness factor.
    *
    * \param extrapolate   Reference to a variable that will be assigned true
    *                   if the local distortion model has to be extrapolated
    *                   over the entire reference image; false if the model can
    *                   only be interpolated within the containing rectangle of
    *                   the set \a P.
    *
    * If this instance does not transport a valid local distortion model, the
    * specified arrays \a P1, \a D2, \a P2 and \a D1 will be empty after
    * calling this function.
    *
    * \deprecated This function is deprecated. It is kept only to support
    * existing drizzle data files and dependent applications. This function
    * should not be used in newly produced code unless strictly necessary for
    * backwards compatibility.
    *
    * \sa SetLocalDistortionModel(), HasLocalDistortionModel()
    */
   void GetLocalDistortionModel( point_list& P1, point_list& D2,
                                 point_list& P2, point_list& D1,
                                 weight_vector& W,
                                 int& order, float& regularization, bool& extrapolate )
   {
      P1 = m_LP1;
      D2 = m_LD2;
      P2 = m_LP2;
      D1 = m_LD1;
      W = m_LW;
      order = m_localDistortionOrder;
      regularization = m_localDistortionRegularization;
      extrapolate = m_localDistortionExtrapolation;
   }

   /*!
    * Returns true iff this instance defines a local distortion model
    * consisting of a list of reference points and their corresponding pixel
    * displacements in image coordinates.
    *
    * \deprecated This function is deprecated. It is kept only to support
    * existing drizzle data files and dependent applications. This function
    * should not be used in newly produced code unless strictly necessary for
    * backwards compatibility.
    */
   bool HasLocalDistortionModel() const
   {
      return !m_LP1.IsEmpty();
   }

   /*!
    * Returns a string serialization of metadata items corresponding to the
    * drizzle source image represented by this instance. Metadata items are
    * normally generated and serialized by a regular integration task.
    *
    * \sa SetMetadata()
    */
   const String& Metadata() const
   {
      return m_metadata;
   }

   /*!
    * Defines a string serialization of metadata items for the drizzle source
    * image represented by this instance.
    *
    * \sa Metadata()
    */
   void SetMetadata( const String& metadata )
   {
      m_metadata = metadata.Trimmed();
   }

   /*!
    * Returns the vector of per-channel robust estimates of location for the
    * drizzle source image represented by this instance.
    *
    * See Scale() for a description of the output normalization procedure.
    *
    * Typically, the components of the vector returned by this function are
    * median values, or similar robust estimates of location, computed for each
    * channel of the image identified by AlignmentTargetFilePath(). These
    * estimates are normally generated by a regular integration task.
    *
    * \sa SetLocation(), ReferenceLocation(), Scale(), AdaptiveNormalizationLocation()
    */
   const DVector& Location() const
   {
      return m_location;
   }

   /*!
    * Sets a new vector of per-channel robust estimates of location for the
    * drizzle source image represented by this instance. This function is
    * reserved for image integration tasks involved in drizzle integration
    * processes.
    *
    * \sa Location(), ReferenceLocation(), SetScale(), SetAdaptiveNormalizationLocation()
    */
   void SetLocation( const DVector& v )
   {
      m_location = v;
   }

   /*!
    * Returns the vector of per-channel robust estimates of location for the
    * reference integration image associated with the drizzle source image
    * represented by this instance.
    *
    * See Scale() for a description of the output normalization procedure.
    *
    * Typically, the components of the vector returned by this function are
    * median values, or similar robust estimates of location, computed for each
    * channel of an image used as reference by a regular integration task.
    *
    * \sa SetLocation(), ReferenceLocation(), Scale(),
    * ReferenceAdaptiveNormalizationLocation()
    */
   const DVector& ReferenceLocation() const
   {
      return m_referenceLocation;
   }

   /*!
    * Sets a new vector of per-channel robust estimates of location for the
    * reference integration image. This function is reserved for image
    * integration tasks involved in drizzle integration processes.
    *
    * \sa ReferenceLocation(), Location(), SetScale(),
    * SetReferenceAdaptiveNormalizationLocation()
    */
   void SetReferenceLocation( const DVector& v )
   {
      m_referenceLocation = v;
   }

   /*!
    * Returns the vector of per-channel scaling factors for the drizzle source
    * image represented by this instance.
    *
    * For normalization of output pixel samples in a channel c of the drizzle
    * image, the following equation should be implemented:
    *
    * <tt>vc' = (vc - Lc)*Sc + L0c</tt>
    *
    * where vc is the source pixel sample, Lc is the location estimate given by
    * Location()[c], Sc is the scaling factor given by Scale()[c], L0c is the
    * reference location estimate given by ReferenceLocation()[c], and vc' is
    * the normalized pixel sample value.
    *
    * Typically, the components of the vector returned by this function are the
    * result of dividing reference scale estimates by robust estimates of
    * dispersion, such as MAD or the square root of the biweight midvariance,
    * among many others, computed for each channel of the image identified by
    * AlignmentTargetFilePath(). Scaling factors are normally generated by a
    * regular integration task.
    *
    * \sa SetScale(), Location(), ReferenceLocation(),
    * GetAdaptiveNormalizationScaleVectors()
    */
   const DVector& Scale() const
   {
      if ( !m_scale.IsEmpty() )
         return m_scale;
      if ( m_unitScale.IsEmpty() )
         m_unitScale = DVector( 1.0, NumberOfChannels() );
      return m_unitScale;
   }

   /*!
    * Sets a new vector of per-channel scaling factors for the drizzle source
    * image represented by this instance. This function is reserved for image
    * integration tasks involved in drizzle integration processes.
    *
    * \sa Scale(), SetLocation(), SetAdaptiveNormalizationScaleVectors()
    */
   void SetScale( const DVector& v )
   {
      m_scale = v;
   }

   /*!
    * Returns the vector of adaptive normalization node coordinates.
    *
    * Adaptive normalization is based on surface interpolation of location and
    * scale estimates computed for regions of interest distributed over source
    * and reference images. This function returns the list of 2-D interpolation
    * node coordinates. See GetAdaptiveNormalizationScaleVectors() for a
    * complete description of the adaptive output normalization procedure.
    *
    * \sa SetAdaptiveNormalizationCoordinates()
    */
   const point_list& AdaptiveNormalizationCoordinates() const
   {
      return m_adaptiveCoordinates;
   }

   /*!
    * Sets the vector of adaptive normalization node coordinates. This function
    * is reserved for image integration tasks involved in drizzle integration
    * processes.
    *
    * \sa AdaptiveNormalizationCoordinates()
    */
   void SetAdaptiveNormalizationCoordinates( const point_list& points )
   {
      m_adaptiveCoordinates = points;
   }

   /*!
    * Returns the vectors of per-channel adaptive estimates of location for the
    * drizzle source image represented by this instance.
    *
    * See GetAdaptiveNormalizationScaleVectors() for a complete description of
    * the adaptive output normalization procedure.
    *
    * Typically, the components of the multivector structure returned by this
    * function are median values, or similar robust estimates of location,
    * computed on a regular grid for each channel of the image identified by
    * AlignmentTargetFilePath(). These estimates are normally generated by a
    * regular integration task.
    *
    * \sa SetAdaptiveNormalizationLocationVectors()
    */
   const MultiVector& AdaptiveNormalizationLocationVectors() const
   {
      return m_adaptiveLocation;
   }

   /*!
    * Sets new vectors of per-channel adaptive estimates of location for the
    * drizzle source image represented by this instance. This function is
    * reserved for image integration tasks involved in drizzle integration
    * processes.
    *
    * \sa AdaptiveNormalizationLocationVectors()
    */
   void SetAdaptiveNormalizationLocationVectors( const MultiVector& m )
   {
      m_adaptiveLocation = m;
   }

   /*!
    * Provides the vectors of per-channel adaptive scale factors for the
    * drizzle source image represented by this instance.
    *
    * \param[out] sLow     Reference to a multivector where a copy of the
    *                      current set of low adaptive scale factor vectors
    *                      will be stored.
    *
    * \param[out] sHigh    Reference to a multivector where a copy of the
    *                      current set of high adaptive scale factor vectors
    *                      will be stored.
    *
    * For a channel index c and position vector i on a given input drizzle
    * integration image, the adaptive normalization function is given by:
    *
    * <pre>
    * v'(c,i) = v(c,i) * (a*U(S0(c,i)) + (1-a)*U(S1(c,i)))
    *                  + (a*U(Z0(c,i)) + (1-a)*U(Z1(c,i)))
    * </pre>
    *
    * where:
    *
    * S0 is a matrix of low adaptive scale factors,\n
    * S1 is a matrix of high adaptive scale factors,\n
    * Z0 is a matrix of low adaptive zero offset coefficients,\n
    * Z1 is a matrix of high adaptive zero offset coefficients,\n
    * U is a scale-dependent interpolation functional,\n
    * v is the source pixel value,\n
    * v' is the resulting normalized pixel value,\n
    * a is an indicator function given by:
    *
    * <pre>
    * a = 1   if v(c,i) &le; U(L(c,i))
    * a = 0   if v(c,i) > U(L(c,i))
    * </pre>
    *
    * and L is a matrix of adaptive location estimates.
    *
    * Typically, the components of the multivector structures provided by this
    * function are the result of dividing reference scale estimates by robust
    * estimates of dispersion, such as MAD or the square root of the biweight
    * midvariance, among many others, computed for each channel of the image
    * identified by AlignmentTargetFilePath(). Scale factors are normally
    * generated by a regular integration task.
    *
    * \sa SetAdaptiveNormalizationScaleVectors()
    */
   void GetAdaptiveNormalizationScaleVectors( MultiVector& sLow, MultiVector& sHigh ) const
   {
      sLow = m_adaptiveScaleLow;
      sHigh = m_adaptiveScaleHigh;
   }

   /*!
    * Sets new vectors of per-channel adaptive scaling factors for the drizzle
    * source image represented by this instance. This function is reserved for
    * image integration tasks involved in drizzle integration processes.
    *
    * \sa GetAdaptiveNormalizationScaleVectors()
    */
   void SetAdaptiveNormalizationScaleVectors( const MultiVector& sLow, const MultiVector& sHigh )
   {
      m_adaptiveScaleLow = sLow;
      m_adaptiveScaleHigh = sHigh;
   }

   /*!
    * Provides the vectors of per-channel adaptive zero offset coefficients for
    * the drizzle source image represented by this instance.
    *
    * \param[out] mLow     Reference to a multivector where a copy of the
    *                      current set of low adaptive zero offset coefficient
    *                      vectors will be stored.
    *
    * \param[out] mHigh    Reference to a multivector where a copy of the
    *                      current set of high adaptive zero offset coefficient
    *                      vectors will be stored.
    *
    * See GetAdaptiveNormalizationScaleVectors() for a complete description of
    * the output adaptive normalization procedure.
    *
    * \sa SetAdaptiveNormalizationZeroOffsetVectors()
    */
   void GetAdaptiveNormalizationZeroOffsetVectors( MultiVector& mLow, MultiVector& mHigh ) const
   {
      mLow = m_adaptiveZeroOffsetLow;
      mHigh = m_adaptiveZeroOffsetHigh;
   }

   /*!
    * Sets new vectors of per-channel adaptive zero offsets for the drizzle
    * source image represented by this instance. This function is reserved for
    * image integration tasks involved in drizzle integration processes.
    *
    * \sa GetAdaptiveNormalizationZeroOffsetVectors()
    */
   void SetAdaptiveNormalizationZeroOffsetVectors( const MultiVector& mLow, const MultiVector& mHigh )
   {
      m_adaptiveZeroOffsetLow = mLow;
      m_adaptiveZeroOffsetHigh = mHigh;
   }

   /*!
    * Returns true iff this instance transports adaptive normalization data,
    * i.e. vectors of adaptive normalization coordinates and multivectors of
    * adaptive normalization location estimates, scale factors and zero offset
    * coefficients.
    */
   bool HasAdaptiveNormalizationData() const
   {
      return !m_adaptiveCoordinates.IsEmpty() &&
             !m_adaptiveLocation.IsEmpty() &&
             !m_adaptiveScaleLow.IsEmpty() &&
             !m_adaptiveScaleHigh.IsEmpty() &&
             !m_adaptiveZeroOffsetLow.IsEmpty() &&
             !m_adaptiveZeroOffsetHigh.IsEmpty();
   }

   /*!
    * Returns a positive value in the range [0,1] to be subtracted from the
    * image represented by this instance.
    */
   double Pedestal() const
   {
      return m_pedestal;
   }

   /*!
    * Defines a positive value in the range [0,1] to be subtracted from the
    * image represented by this instance. This function is reserved for image
    * integration tasks involved in drizzle integration processes.
    */
   void SetPedestal( double p )
   {
      m_pedestal = Range( p, 0.0, 1.0 );
   }

   /*!
    * Returns the vector of per-channel statistical weights for the drizzle
    * image represented by this instance.
    *
    * Typically, the components of the returned vector are efficient
    * statistical weights computed for each channel of the image identified by
    * AlignmentTargetFilePath(). These weights are normally generated by a
    * regular integration task.
    *
    * \sa SetWeight(), Location(), ReferenceLocation(), Scale()
    */
   const DVector& Weight() const
   {
      if ( !m_weight.IsEmpty() )
         return m_weight;
      if ( m_unitWeight.IsEmpty() )
         m_unitWeight = DVector( 1.0, NumberOfChannels() );
      return m_unitWeight;
   }

   /*!
    * Sets a new vector of per-channel statistical weights for the drizzle
    * image represented by this instance. This function is reserved for image
    * integration tasks involved in drizzle integration processes.
    *
    * \sa Weight(), SetLocation(), SetReferenceLocation(), SetScale()
    */
   void SetWeight( const DVector& v )
   {
      m_weight = v;
   }

   /*!
    * Returns true iff this %DrizzleData object transports image integration
    * data for normalization and image combination. This includes at least
    * location and reference location estimates, and optionally (but usually
    * present) scaling factors and statistical image weights.
    */
   bool HasIntegrationData() const
   {
      return !m_location.IsEmpty();
   }

   /*!
    * Returns true iff this %DrizzleData object transports per-channel
    * statistical image weights. If no weight information is available, the
    * Weight() member function will return a reference to an internal vector
    * with all components set to one.
    */
   bool HasImageWeightsData() const
   {
      return !m_weight.IsEmpty();
   }

   /*!
    * Returns the vector of per-channel low pixel rejection counts for the
    * drizzle source image represented by this instance.
    *
    * The components of the returned vector are the total amounts of rejected
    * low pixels for each channel of the image identified by
    * AlignmentTargetFilePath(). Pixel rejection counts are normally generated
    * by a regular image integration task.
    *
    * \sa RejectionHighCount(), RejectionMap()
    */
   const UI64Vector& RejectionLowCount() const
   {
      return m_rejectionLowCount;
   }

   /*!
    * Returns the vector of per-channel high pixel rejection counts for the
    * drizzle source image represented by this instance.
    *
    * The components of the returned vector are the total amounts of rejected
    * high pixels for each channel of the image identified by
    * AlignmentTargetFilePath(). Pixel rejection counts are normally generated
    * by a regular image integration task.
    *
    * \sa RejectionLowCount(), RejectionMap()
    */
   const UI64Vector& RejectionHighCount() const
   {
      return m_rejectionHighCount;
   }

   /*!
    * Returns a reference to a pixel rejection map generated for the drizzle
    * image represented by this instance.
    *
    * This member function returns a reference to an 8-bit unsigned integer
    * image known as <em>drizzle rejection map</em>. Each channel of the
    * rejection map corresponds to the same channel of the image represented by
    * this object. For a channel index c, a pixel sample of a rejection map can
    * have the following values:
    *
    * \li 0 (no bit set): The corresponding pixel has not been rejected for
    * image channel c.
    *
    * \li Bit 0 set: High statistical rejection: The pixel has been rejected
    * above the estimated central value of its pixel integration stack using a
    * statistical rejection algorithm (such as sigma clipping).
    *
    * \li Bit 1 set: Low statistical rejection: The pixel has been rejected
    * below the estimated central value of its pixel integration stack using a
    * statistical rejection algorithm (such as sigma clipping).
    *
    * \li Bit 2 set: High range rejection: The pixel has been rejected because
    * its value is greater than or equal to a prescribed upper limit.
    *
    * \li Bit 3 set: Low range rejection: The pixel has been rejected because
    * its value is smaller than or equal to a prescribed lower limit.
    *
    * \li Bit 4 set: High large-scale rejection: The pixel belongs to a bright
    * large-scale image structure that has been rejected using multiscale
    * analysis techniques.
    *
    * \li Bit 5 set: Low large-scale rejection: The pixel belongs to a dark
    * large-scale image structure that has been rejected using multiscale
    * analysis techniques.
    *
    * Bits 6 and 7 are reserved for future extensions and should be zero in the
    * current implementation.
    *
    * Rejection maps are automatically generated from pixel rejection data
    * stored in .xdrz files. Pixel rejection data are normally generated by a
    * regular image integration task.
    *
    * If no pixel rejection information is available in this instance, this
    * function returns a reference to an empty image.
    *
    * \sa SetRejectionMap(), HasRejectionData(), IsRejected()
    */
   const UInt8Image& RejectionMap() const
   {
      return m_rejectionMap;
   }

   /*!
    * Returns true iff the pixel at the specified pixel coordinates has been
    * rejected for the drizzle source image represented by this instance.
    *
    * \sa RejectionMap(), HasRejectionData()
    */
   bool IsRejected( const Point& position, int channel = 0 ) const
   {
      return  m_rejectionMap.Includes( position )
          &&  m_rejectionMap.IsValidChannelIndex( channel )
          && (m_rejectionMap( position, channel ) & 0x3F) != 0;
   }

   /*!
    * Defines per-channel pixel rejection data for the drizzle source image
    * represented by this instance. The specified \a map image is a drizzle
    * rejection map as described in RejectionMap().
    *
    * \sa RejectionMap(), HasRejectionData()
    */
   void SetRejectionMap( const UInt8Image& map )
   {
      m_rejectionMap = map;
   }

   /*!
    * Returns true iff pixel rejection information is available for the drizzle
    * image represented by this instance.
    */
   bool HasRejectionData() const
   {
      return !m_rejectionMap.IsEmpty();
   }

   /*!
    * Returns the UTC time this drizzle data was created, or an invalid
    * TimePoint instance if creation time information is not available.
    */
   TimePoint CreationTime() const
   {
      return m_creationTime;
   }

   /*!
    * Clears all drizzle integration data transported by this instance,
    * yielding an empty %DrizzleData object.
    */
   void Clear();

   /*!
    * Clears the image integration  data transported by this instance. This
    * includes statistical location and scale estimates for image
    * normalization, image weights, pixel rejection data, and adaptive
    * normalization data.
    */
   void ClearIntegrationData();

   /*!
    * Loads and parses a drizzle data file.
    *
    * \param filePath   Path to an existing file that will be parsed. The file
    *                   contents can be in %XML drizzle data format (normally,
    *                   a file with the .xdrz suffix), or in old plain text
    *                   format (typically with the .drz suffix). This function
    *                   will detect the format in use from the first bytes read
    *                   from the file, and will decode it correspondingly.
    *
    * \param options    Optional set of flags controlling the way drizzle data
    *                   are parsed and verified. See the DrizzleParserOption
    *                   namespace for more information.
    *
    * All previous data transported by this instance will be replaced with new
    * data acquired from the specified file.
    *
    * This function validates the data retrieved from the specified file. It
    * throws an Error exception in the event of parsing errors or invalid data.
    */
   void Parse( const String& filePath, DrizzleParserOptions options = DrizzleParserOptions() );

   /*!
    * Parses a well-formed %XML document.
    *
    * \param xml        Reference to the source %XML document. This member
    *                   function expects an %XML document in valid %XML
    *                   drizzle data format (.xdrz).
    *
    * \param options    Optional set of flags controlling the way drizzle data
    *                   are parsed and verified. See the DrizzleParserOption
    *                   namespace for more information.
    *
    * All previous data transported by this instance will be replaced with new
    * data acquired from the specified %XML contents.
    *
    * This function validates the data retrieved from the specified %XML
    * document. It throws an Error exception in the event of an invalid
    * document or invalid data.
    */
   void Parse( const XMLDocument& xml, DrizzleParserOptions options = DrizzleParserOptions() );

   /*!
    * Parses an %XML element.
    *
    * \param element    Reference to the source %XML element. This member
    *                   function expects an %XML document tree in valid %XML
    *                   drizzle data format (.xdrz) rooted at this element.
    *
    * \param options    Optional set of flags controlling the way drizzle data
    *                   are parsed and verified. See the DrizzleParserOption
    *                   namespace for more information.
    *
    * All previous data transported by this instance will be replaced with new
    * data acquired from the specified %XML contents.
    *
    * This function validates the data retrieved from the specified %XML
    * element. It throws an Error exception in the event of invalid data.
    */
   void Parse( const XMLElement& element, DrizzleParserOptions options = DrizzleParserOptions() );

   /*!
    * Returns true if drizzle data compression is enabled for serializations
    * performed with this object.
    *
    * Drizzle data compression is enabled by default for newly constructed
    * %DrizzleData objects. Currently the LZ4 compression codec is applied for
    * serialization of pixel rejection data.
    *
    * \sa EnableCompression(), DisableCompression()
    */
   bool IsCompressionEnabled() const
   {
      return m_compressionEnabled;
   }

   /*!
    * Enables compression of serialized drizzle data.
    *
    * \sa IsCompressionEnabled(), DisableCompression()
    */
   void EnableCompression( bool enable = true )
   {
      m_compressionEnabled = enable;
   }

   /*!
    * Disables compression of serialized drizzle data.
    *
    * \sa IsCompressionEnabled(), EnableCompression()
    */
   void DisableCompression( bool disable = true )
   {
      EnableCompression( !disable );
   }

   /*!
    * Serializes the drizzle integration data transported by this object as a
    * new %XML document in .xdrz format.
    *
    * The returned XMLDocument object has been allocated dynamically. The
    * caller is responsible for destroying and deallocating it as appropriate.
    */
   XMLDocument* Serialize() const;

   /*!
    * Serializes the drizzle integration data transported by this object as a
    * new %XML document file in .xdrz format. The file will be newly created at
    * the specified file \a path.
    *
    * \warning If a file already exists at the specified \a path, its previous
    * contents will be lost after calling this function.
    */
   void SerializeToFile( const String& path ) const;

private:

           String         m_sourceFilePath;
           String         m_cfaSourceFilePath;
           String         m_cfaSourcePattern;
           int            m_cfaSourceChannel = -1;
           String         m_alignTargetFilePath;
           int            m_referenceWidth  = -1;
           int            m_referenceHeight = -1;
           DPoint         m_alignmentOrigin = 0.5;
           Matrix         m_H;
           vector_spline  m_S;
           vector_spline  m_Sinv;
           point_list     m_LP1;                                   // D* = deprecated
           point_list     m_LD2;                                   // D*
           point_list     m_LP2;                                   // D*
           point_list     m_LD1;                                   // D*
           weight_vector  m_LW;                                    // D*
           int            m_localDistortionOrder = 2;              // D*
           float          m_localDistortionRegularization = 0.01F; // D*
           bool           m_localDistortionExtrapolation = true;   // D*
           String         m_metadata;
           double         m_pedestal = 0.0;
           Vector         m_location;
           Vector         m_referenceLocation;
           Vector         m_scale;
   mutable Vector         m_unitScale;
           Vector         m_weight;
   mutable Vector         m_unitWeight;
           point_list     m_adaptiveCoordinates;
           MultiVector    m_adaptiveLocation;
           MultiVector    m_adaptiveScaleLow;
           MultiVector    m_adaptiveScaleHigh;
           MultiVector    m_adaptiveZeroOffsetLow;
           MultiVector    m_adaptiveZeroOffsetHigh;
           UI64Vector     m_rejectionLowCount;
           UI64Vector     m_rejectionHighCount;
           UInt8Image     m_rejectionMap;
           TimePoint      m_creationTime;
           bool           m_compressionEnabled = true;

   void ParseRejectionMap( const XMLElement& );
   void SerializeRejectionMap( XMLElement* ) const;

   static void ParseSpline( spline&, const XMLElement& );
   void SerializeSpline( XMLElement*, const spline& ) const;

   static point_list ParsePoints( const XMLElement& );
   void SerializePoints( XMLElement*, const point_list& ) const;

   static weight_vector ParseDistortionWeights( const XMLElement& );
   void SerializeDistortionWeights( XMLElement*, const weight_vector& ) const;

   static ByteArray ParseMaybeCompressedData( const XMLElement& );
   void SerializeMaybeCompressedData( XMLElement* root,
                        const void* data, size_type size, size_type itemSize = 1 ) const;

   template <typename T>
   static GenericVector<T> ParseMaybeCompressedVector( const XMLElement&, size_type = 0, size_type = ~size_type( 0 ) );

   template <typename T>
   void SerializeMaybeCompressedVector( XMLElement* root, const GenericVector<T> v ) const
   {
      SerializeMaybeCompressedData( root, v.Begin(), v.Size(), sizeof( typename GenericVector<T>::item_type ) );
   }

   /*!
    * \internal
    * \class DrizzleData::PlainTextDecoder
    * \brief Read-only compatibility with old plain text .drz files
    */
   class PlainTextDecoder
   {
   public:

      /*!
       * A list of coordinates corresponding to pixels rejected by an image
       * integration task.
       */
      using rejection_coordinates = Array<Point>;

      /*!
       * A set of per-channel pixel rejection coordinates.
       */
      using rejection_data = Array<rejection_coordinates>;

      PlainTextDecoder( DrizzleData* data, bool ignoreIntegrationData )
         : m_data( data )
         , m_ignoreIntegrationData( ignoreIntegrationData )
      {
      }

      virtual ~PlainTextDecoder()
      {
      }

      void Decode( IsoString&, size_type start = 0, size_type end = 0 );

   protected:

      PlainTextDecoder() = default;

   private:

      DrizzleData* m_data = nullptr;
      bool         m_ignoreIntegrationData = false;

      virtual void ProcessBlock( IsoString&, const IsoString&, size_type, size_type );

      static rejection_coordinates ParseRejectionCoordinates( IsoString&, size_type, size_type );
      static rejection_data ParseRejectionData( IsoString&, size_type, size_type );
      static spline ParseSurfaceSpline( IsoString&, size_type, size_type );
   };

   class PlainTextSplineDecoder : public PlainTextDecoder
   {
   public:

      PlainTextSplineDecoder( spline& S )
         : m_S( S )
      {
      }

   private:

      spline& m_S;

      void ProcessBlock( IsoString&, const IsoString&, size_type, size_type ) override;
   };

   friend class DrizzleDataPrivate;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_DrizzleData_h

// ----------------------------------------------------------------------------
// EOF pcl/DrizzleData.h - Released 2024-12-28T16:53:48Z
