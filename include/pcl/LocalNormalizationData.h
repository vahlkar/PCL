//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/LocalNormalizationData.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_LocalNormalizationData_h
#define __PCL_LocalNormalizationData_h

/// \file pcl/LocalNormalizationData.h

#include <pcl/Defs.h>

#include <pcl/BicubicInterpolation.h>
#include <pcl/File.h>
#include <pcl/Image.h>
#include <pcl/TimePoint.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS XMLDocument;
class PCL_CLASS XMLElement;

/*!
 * \class LocalNormalizationData
 * \brief Normalization data parser and generator
 *
 * %LocalNormalizationData implements support for the %XML normalization data
 * format (XNML format, .xnml file suffix).
 *
 * An XNML file stores local normalization function data suitable for pixel
 * rejection and image integration tasks, including drizzle integration. The
 * %LocalNormalizationData class is a high-level interface for parsing and
 * generating these special files.
 *
 * Local normalization functions are serialized as floating point real images,
 * where each channel stores scaling and zero offset coefficients, sampled at a
 * prescribed normalization scale for a channel of a specific image.
 *
 * For a channel index c and position vector i on a given input image, the
 * local normalization function is:
 *
 * v'(c,i) = U(A(c,i))*(v(c,i) - C(c)) + U(B(c,i))
 *
 * where A(c) is the matrix of scaling coefficients, B(c) is the matrix of zero
 * offset coefficients, C(c) is the bias level (C = 0 by default), U() is a
 * scale-dependent interpolation functional, v is an input pixel value, and v'
 * is the normalized pixel value.
 *
 * This implementation can also apply an optional global normalization function
 * after the local normalization. If the corresponding parameters are defined,
 * the global normalization function is:
 *
 * v''(c,i) = S(c)*(v'(c,i) - T(c)) + R(c)
 *
 * where S is a vector of global scaling factors, T is a vector of target
 * locations, and R is a vector of reference locations. v' is the locally
 * normalized pixel value, as defined above, and v'' is the output value after
 * local and global normalization.
 *
 * Local normalization matrices are conveniently stored as multichannel,
 * two-dimensional images, facilitating two-dimensional interpolation with
 * smooth pixel interpolation algorithms. Normalization matrices usually are
 * much smaller than the reference image dimensions. This happens because we
 * define local normalization functions at a specific dimensional scale,
 * typically corresponding to a large dyadic scale from 256 to 1024 pixels. For
 * this reason, normalization matrices require a smooth interpolation to
 * compute normalization coefficients at reference image coordinates.
 *
 * \sa DrizzleData, FluxCalibrationData
 */
class PCL_CLASS LocalNormalizationData
{
public:

   /*!
    * Represents a per-channel set of local image normalization matrices.
    */
   using normalization_matrices = Image;

   /*!
    * Represents a local normalization function coefficient.
    */
   using normalization_coefficient = normalization_matrices::sample;

   /*!
    * Represents a smooth interpolation used for calculation of local
    * normalization function values at image pixel coordinates.
    */
   using matrix_interpolation = BicubicBSplineInterpolation<normalization_coefficient>;

   /*!
    * Represents an array of local normalization interpolators.
    */
   using matrix_interpolations = Array<matrix_interpolation>;

   /*!
    * Default constructor. Constructs an empty %LocalNormalizationData object.
    */
   LocalNormalizationData() = default;

   /*!
    * Copy constructor.
    */
   LocalNormalizationData( const LocalNormalizationData& ) = default;

   /*!
    * Move constructor.
    */
   LocalNormalizationData( LocalNormalizationData&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   LocalNormalizationData& operator =( const LocalNormalizationData& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   LocalNormalizationData& operator =( LocalNormalizationData&& ) = default;

   /*!
    * Constructs a new %LocalNormalizationData instance by loading and parsing
    * a file in XNML format.
    *
    * \param filePath   Path to an existing file that will be parsed. The file
    *                   contents must be in %XML normalization data format
    *                   (usually, a file with the .xnml suffix).
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * This constructor validates the data retrieved from the specified file. It
    * throws the appropriate Error exceptions in the event of parsing errors or
    * invalid data.
    */
   LocalNormalizationData( const String& filePath, bool ignoreNormalizationData = false )
   {
      Parse( filePath, ignoreNormalizationData );
   }

   /*!
    * Constructs a new %LocalNormalizationData instance by parsing a
    * well-formed %XML document in XNML format.
    *
    * \param xml        Reference to the source %XML document. This constructor
    *                   expects an %XML document in valid %XML normalization
    *                   data format (.xnml).
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * This constructor validates the data retrieved from the specified %XML
    * document. It throws an Error exception in the event of an invalid
    * document or invalid data.
    */
   LocalNormalizationData( const XMLDocument& xml, bool ignoreNormalizationData = false )
   {
      Parse( xml, ignoreNormalizationData );
   }

   /*!
    * Constructs a new %LocalNormalizationData instance by parsing an %XML
    * element in XNML format.
    *
    * \param element    Reference to the source %XML element. This constructor
    *                   expects an %XML document tree in valid %XML
    *                   normalization data format (.xnml) rooted at the
    *                   specified element.
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * This constructor validates the data retrieved from the specified %XML
    * element. It throws an Error exception in the event of invalid data.
    */
   LocalNormalizationData( const XMLElement& element, bool ignoreNormalizationData = false )
   {
      Parse( element, ignoreNormalizationData );
   }

   /*!
    * Virtual destructor.
    */
   virtual ~LocalNormalizationData()
   {
   }

   /*!
    * Returns the full path to the reference image file used for calculation of
    * the normalization data represented by this instance, or an empty string
    * if that file path is not available.
    *
    * \sa SetReferenceFilePath()
    */
   const String& ReferenceFilePath() const
   {
      return m_referenceFilePath;
   }

   /*!
    * Sets the path to the reference image file used for calculation of the
    * normalization data stored in this instance.
    *
    * \sa ReferenceFilePath()
    */
   void SetReferenceFilePath( const String& filePath )
   {
      m_referenceFilePath = File::FullPath( filePath );
   }

   /*!
    * Returns the full path to the image file for which the normalization data
    * represented by this instance was calculated, or an empty string if such
    * file path is not available.
    *
    * The file identified by this function stores the input image of the image
    * normalization task that created this object. This file can be used as
    * input for a regular (non-drizzle) integration task, and the data
    * transported by this object can then be used for normalization of pixel
    * values in pixel rejection and/or integration output generation, including
    * drizzle integration.
    *
    * \sa SetTargetFilePath()
    */
   const String& TargetFilePath() const
   {
      return m_targetFilePath;
   }

   /*!
    * Sets the path to the image file for which the normalization data stored
    * in this instance was calculated.
    *
    * \sa TargetFilePath()
    */
   void SetTargetFilePath( const String& filePath )
   {
      m_targetFilePath = File::FullPath( filePath );
   }

   /*!
    * Returns the scale in pixels at which the local normalization functions
    * were sampled and calculated. For valid normalization data, the returned
    * value must be larger than zero, and should normally be larger than or
    * equal to 32 pixels.
    *
    * \sa SetNormalizationScale()
    */
   int NormalizationScale() const
   {
      return m_scale;
   }

   /*!
    * Sets the scale in pixels at which the local normalization functions have
    * been sampled and calculated.
    *
    * \sa NormalizationScale()
    */
   void SetNormalizationScale( int scale )
   {
      m_scale = scale;
   }

   /*!
    * Returns the width in pixels of the normalization reference and target
    * images, or -1 if this object has not been initialized with valid data.
    *
    * \sa ReferenceHeight(), SetReferenceDimensions()
    */
   int ReferenceWidth() const
   {
      return m_referenceWidth;
   }

   /*!
    * Returns the height in pixels of the normalization reference and target
    * images, or -1 if this object has not been initialized with valid data.
    *
    * \sa ReferenceWidth(), SetReferenceDimensions()
    */
   int ReferenceHeight() const
   {
      return m_referenceHeight;
   }

   /*!
    * Sets new normalization image dimensions in pixels, \a width and
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
    * represented by this instance, or zero if this object has not been
    * initialized with valid data.
    */
   int NumberOfChannels() const
   {
      return m_A.NumberOfChannels();
   }

   /*!
    * Returns a reference to the set of matrices of zero offset coefficients in
    * the local normalization function.
    *
    * See the class description for detailed information on local normalization
    * functions.
    *
    * \sa Scale(), SetLocalNormalizationParameters(), NormalizationScale()
    */
   const normalization_matrices& ZeroOffset() const
   {
      return m_B;
   }

   /*!
    * Returns a reference to the set of matrices of scaling coefficients in the
    * local normalization function.
    *
    * See the class description for detailed information on local normalization
    * functions.
    *
    * \sa ZeroOffset(), SetLocalNormalizationParameters(), NormalizationScale()
    */
   const normalization_matrices& Scale() const
   {
      return m_A;
   }

   /*!
    * Returns a reference to the vector of per-channel bias levels. Each bias
    * is a constant scalar subtracted from input pixel sample values before
    * applying the local normalization function. This allows to remove any
    * systematic offset or additive pedestal that could be present in the data
    * to be normalized. The possibility to remove a constant value also adds
    * flexibility to the local normalization function.
    *
    * If not specified, the target bias is always zero by default.
    */
   const Vector& Bias() const
   {
      return m_C;
   }

   /*!
    * Sets new local normalization function parameters.
    *
    * \param A          Reference to an image storing the matrices of scaling
    *                   normalization coefficients.
    *
    * \param B          Reference to an image storing the matrices of zero
    *                   offset normalization coefficients.
    *
    * \param C          Reference to an optional vector of per-channel biases.
    *                   If not specified, the bias will be set to zero for each
    *                   channel by default.
    *
    * See the class description for detailed information on local normalization
    * functions.
    *
    * This function throws an Error exception if some of the specified local
    * normalization parameters are invalid or incongruent.
    *
    * \sa ZeroOffset(), Scale(), Bias(), SetGlobalNormalizationParameters()
    */
   void SetLocalNormalizationParameters( const normalization_matrices& A, const normalization_matrices& B, const Vector& C = Vector() )
   {
      if ( A.Bounds() != B.Bounds() )
         throw Error( "LocalNormalizationData::SetLocalNormalizationParameters(): Incompatible matrix dimensions." );
      if ( A.NumberOfChannels() != B.NumberOfChannels() )
         throw Error( "LocalNormalizationData::SetLocalNormalizationParameters(): Incompatible number of normalization matrices." );
      m_A = A;
      m_B = B;
      if ( !C.IsEmpty() )
      {
         if ( C.Length() != A.NumberOfChannels() )
            throw Error( "LocalNormalizationData::SetLocalNormalizationParameters(): Incompatible bias vector length." );
         m_C = C;
      }
   }

   /*!
    * Returns a reference to the vector of per-channel global reference
    * locations (or zero-offsets).
    *
    * \sa GlobalTargetLocation(), GlobalScalingFactors(),
    * SetGlobalNormalizationParameters()
    */
   const Vector& GlobalReferenceLocation() const
   {
      return m_Rc;
   }

   /*!
    * Returns a reference to the vector of per-channel global target locations
    * (or zero-offsets).
    *
    * \sa GlobalReferenceLocation(), GlobalScalingFactors(),
    * SetGlobalNormalizationParameters()
    */
   const Vector& GlobalTargetLocation() const
   {
      return m_Rc;
   }

   /*!
    * Returns a reference to the vector of per-channel global scaling factors.
    *
    * \sa GlobalReferenceLocation(), GlobalTargetLocation(),
    * SetGlobalNormalizationParameters()
    */
   const Vector& GlobalScalingFactors() const
   {
      return m_S;
   }

   /*!
    * Sets new global normalization vectors.
    *
    * \param Rc         Reference to a vector of per-channel reference global
    *                   locations.
    *
    * \param Tc         Reference to a vector of per-channel target global
    *                   locations.
    *
    * \param S          Reference to a vector of per-channel global scaling
    *                   factors.
    *
    * See the class description for detailed information on local normalization
    * functions.
    *
    * This function throws an Error exception if some of the specified global
    * normalization parameters are invalid or incongruent.
    *
    * \sa GlobalReferenceLocation(), GlobalTargetLocation(),
    * GlobalScalingFactors(), SetLocalNormalizationParameters()
    */
   void SetGlobalNormalizationParameters( const Vector& Rc, const Vector& Tc, const Vector& S )
   {
      if ( m_B.IsEmpty() )
         throw Error( "LocalNormalizationData::SetGlobalNormalizationParameters(): No zero offset coefficient matrix has been defined." );
      if ( Rc.Length() != m_B.NumberOfChannels() || Tc.Length() != m_B.NumberOfChannels() || S.Length() != m_B.NumberOfChannels() )
         throw Error( "LocalNormalizationData::SetGlobalNormalizationParameters(): Incompatible vector lengths." );
      m_Rc = Rc;
      m_Tc = Tc;
      m_S = S;
   }

   /*!
    * Returns a reference to the vector of per-channel relative scale factors.
    */
   const Vector& RelativeScaleFactors() const
   {
      return m_relScale;
   }

   /*!
    * Sets a new relative scale factors vector.
    *
    * \param Rc   Reference to a vector of per-channel relative scale factors.
    *
    * See the class description for detailed information on local normalization
    * functions.
    *
    * This function throws an Error exception if the specified vector is
    * invalid or incongruent with normalization data.
    */
   void SetRelativeScaleFactors( const Vector& s )
   {
      if ( m_B.IsEmpty() )
         throw Error( "LocalNormalizationData::SetRelativeScaleFactors(): No zero offset coefficient matrix has been defined." );
      if ( s.Length() != m_B.NumberOfChannels() )
         throw Error( "LocalNormalizationData::SetRelativeScaleFactors(): Incompatible vector lengths." );
      m_relScale = s;
   }

   /*!
    * Initializes the internal local normalization function interpolators.
    * After this initialization, operator()() can be called repeatedly to apply
    * the local normalization function to pixel sample values in reference
    * image coordinates.
    *
    * Calling this member function is not necessary when valid local
    * normalization data have been loaded from an XNML file or %XML document,
    * since in this case the internal interpolators are always initialized
    * automatically. This function must be called, however, for newly defined
    * normalization data set by calling SetReferenceDimensions() and
    * SetLocalNormalizationParameters().
    *
    * If this object has not been initialized with valid local normalization
    * data, this member function throws an Error exception.
    *
    * \sa operator()()
    */
   void InitInterpolations();

   /*!
    * Returns true iff valid local normalization function data have been loaded
    * and the internal interpolators have been initialized.
    */
   bool HasInterpolations() const
   {
      return !m_UA.IsEmpty() && !m_UB.IsEmpty();
   }

   /*!
    * Returns a locally normalized pixel sample value.
    *
    * \param z          Input pixel sample value (unnormalized).
    * \param x          Horizontal image pixel coordinate.
    * \param y          Vertical image pixel coordinate.
    * \param c          Channel index.
    *
    * This operator interpolates local normalization functions and applies the
    * corresponding linear transformation to the input value \a z, which gives
    * the return value. Note that the normalized value might be out of the
    * input pixel sample range. In such case the normalized value should be
    * truncated or rescaled as appropriate.
    *
    * For performance reasons, this operator does not check that the required
    * normalization and interpolation data have been initialized; if they are
    * not, calling this operator will most likely lead to a crash.
    * Interpolation data are automatically initialized when a valid XNML file
    * or %XML element is loaded and parsed. For newly constructed objects, make
    * sure you define the required reference dimensions and normalization
    * matrices. Then call InitInterpolations() before starting to call this
    * operator.
    */
   double operator()( double z, int x, int y, int c = 0 ) const
   {
      double sx = m_sx*x;
      double sy = m_sy*y;
      if ( likely( m_S.IsEmpty() ) )
         return m_UA[c]( sx, sy )*(z - m_C[c]) + m_UB[c]( sx, sy );
      return (m_UA[c]( sx, sy )*(z - m_C[c]) + m_UB[c]( sx, sy ) - m_Tc[c])*m_S[c] + m_Rc[c];
   }

   /*!
    * Returns the UTC time this normalization data was created, or an invalid
    * TimePoint instance if creation time information is not available.
    */
   TimePoint CreationTime() const
   {
      return m_creationTime;
   }

   /*!
    * Clears all image normalization data transported by this instance,
    * yielding an empty %LocalNormalizationData object.
    */
   void Clear();

   /*!
    * Loads and parses a normalization data file in XNML format.
    *
    * \param filePath   Path to an existing file that will be parsed. The file
    *                   contents must be in %XML normalization data format
    *                   (usually, a file with the .xnml suffix).
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * All previous data transported by this instance will be replaced with new
    * data acquired from the specified file.
    *
    * This function validates the data retrieved from the specified file. It
    * throws an Error exception in the event of parsing errors or invalid data.
    */
   void Parse( const String& filePath, bool ignoreNormalizationData = false );

   /*!
    * Parses a well-formed %XML document in XNML format.
    *
    * \param xml        Reference to the source %XML document. This member
    *                   function expects an %XML document in valid %XML
    *                   normalization data format (.xnml).
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * All previous data transported by this instance will be replaced with new
    * data acquired from the specified %XML contents.
    *
    * This function validates the data retrieved from the specified %XML
    * document. It throws an Error exception in the event of an invalid
    * document or invalid data.
    */
   void Parse( const XMLDocument& xml, bool ignoreNormalizationData = false );

   /*!
    * Parses an %XML element in XNML format.
    *
    * \param element    Reference to the source %XML element. This member
    *                   function expects an %XML document tree in valid %XML
    *                   normalization data format (.xnml) rooted at the
    *                   specified element.
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * All previous data transported by this instance will be replaced with new
    * data acquired from the specified %XML contents.
    *
    * This function validates the data retrieved from the specified %XML
    * element. It throws an Error exception in the event of invalid data.
    */
   void Parse( const XMLElement& element, bool ignoreNormalizationData = false );

   /*!
    * Returns true if data compression is enabled for serializations performed
    * with this object.
    *
    * Data compression is enabled by default for newly constructed
    * %LocalNormalizationData objects. Currently the LZ4 compression codec is
    * applied for serialization of local normalization matrices.
    *
    * \sa EnableCompression(), DisableCompression()
    */
   bool IsCompressionEnabled() const
   {
      return m_compressionEnabled;
   }

   /*!
    * Enables compression of serialized local normalization data.
    *
    * \sa IsCompressionEnabled(), DisableCompression()
    */
   void EnableCompression( bool enable = true )
   {
      m_compressionEnabled = enable;
   }

   /*!
    * Disables compression of serialized local normalization data.
    *
    * \sa IsCompressionEnabled(), EnableCompression()
    */
   void DisableCompression( bool disable = true )
   {
      EnableCompression( !disable );
   }

   /*!
    * Returns a version number corresponding to the normalization data
    * transported by this object.
    *
    * Version numbers can be used to control the use and propagation of
    * normalization data generated by different versions of a normalization
    * tool or process, such as the standard LocalNormalization process in
    * PixInsight. The version number is zero by default, meaning that the
    * version is unspecified. An appropriate version number &gt; 0 can be set
    * by calling SetVersion(), which is recommended before serializing
    * normalization data to a new XNML file. When an XNML file is loaded and it
    * contains a valid version number tag, it is loaded and will be returned by
    * this function. Otherwise the returned value will be zero.
    */
   int Version() const
   {
      return m_version;
   }

   /*!
    * Sets the normalization data version. See Version() for information on
    * normalization data versions.
    */
   void SetVersion( int version )
   {
      m_version = version;
   }

   /*!
    * Returns true iff this object has parsed XNML source code, either from an
    * existing file or from an %XML document or element, that has been tagged
    * as invalid local normalization data.
    */
   bool IsTaggedAsInvalid() const
   {
      return m_invalid;
   }

   /*!
    * Tags this object as invalid local normalization data.
    */
   void TagAsInvalid()
   {
      m_invalid = true;
   }

   /*!
    * Serializes the normalization data transported by this object as a new
    * %XML document in XNML format.
    *
    * The returned XMLDocument object has been allocated dynamically. The
    * caller is responsible for destroying and deallocating it as appropriate.
    */
   XMLDocument* Serialize() const;

   /*!
    * Serializes the normalization data transported by this object as a new
    * %XML document file in XNML format (.xnml). The file will be newly created
    * at the specified file \a path.
    *
    * \warning If a file already exists at the specified \a path, its previous
    * contents will be lost after calling this function.
    */
   void SerializeToFile( const String& path ) const;

private:

   String                 m_referenceFilePath;    // path to the normalization reference image
   String                 m_targetFilePath;       // path to the normalization target image
   int                    m_scale = -1;           // normalization scale in px
   int                    m_referenceWidth = -1;  // reference image width in px
   int                    m_referenceHeight = -1; // reference image height in px
   normalization_matrices m_A;                    // scaling coefficients
   normalization_matrices m_B;                    // zero offset coefficients
   Vector                 m_C;                    // input bias
   matrix_interpolations  m_UA;                   // interpolators for m_A
   matrix_interpolations  m_UB;                   // interpolators for m_B
   Vector                 m_Rc;                   // global normalization, reference center
   Vector                 m_Tc;                   // global normalization, target center
   Vector                 m_S;                    // global normalization, scale factor
   Vector                 m_relScale;             // relative scale factor
   double                 m_sx;                   // coordinate scaling factor, X axis
   double                 m_sy;                   // coordinate scaling factor, Y axis
   TimePoint              m_creationTime;
   bool                   m_compressionEnabled = true;
   int                    m_version = 0;
   bool                   m_invalid = false;

   void ParseNormalizationMatrices( normalization_matrices&, const XMLElement& ) const;
   void SerializeNormalizationMatrices( XMLElement*, const normalization_matrices& ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_LocalNormalizationData_h

// ----------------------------------------------------------------------------
// EOF pcl/LocalNormalizationData.h - Released 2024-12-28T16:53:48Z
