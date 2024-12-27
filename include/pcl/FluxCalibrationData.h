//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/FluxCalibrationData.h - Released 2024-12-27T18:16:01Z
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

#ifndef __PCL_FluxCalibrationData_h
#define __PCL_FluxCalibrationData_h

/// \file pcl/FluxCalibrationData.h

#include <pcl/Defs.h>

#include <pcl/File.h>
#include <pcl/TimePoint.h>
#include <pcl/Vector.h>

/*
 * Normalization factor applied to all SPFC flux samples. This allows us to
 * generate scale factors within manageable ranges for typical deep-sky images,
 * i.e., in [100,0.001] instead of [1e-7,1e-12], for example.
 */
#define __PCL_SPFC_NORMALIZATION_FACTOR   1.0e+08

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS XMLDocument;
class PCL_CLASS XMLElement;

/*!
 * \class FluxCalibrationData
 * \brief Flux calibration data parser and generator
 *
 * %FluxCalibrationData implements support for the %XML flux calibration data
 * format (XFCL format, .xfcl file suffix).
 *
 * Flux calibration is essential for critical tasks requiring accurate and
 * robust estimates of statistical scale computed for deep-sky astronomical
 * images. On the PixInsight platform, we have implemented absolute,
 * universally comparable, spectrophotometry-based scale estimation based on
 * stellar photometry and Gaia mean spectra provided by the Gaia DR3 catalog.
 * The primary tool to generate these estimates and XFCL files is
 * SpectrophotometricFluxCalibration (SPFC).
 *
 * Absolute spectrophotometry-based scale estimates are currently required by
 * the MultiscaleGradientCorrection process and are critically important for
 * generating Multiscale All-Sky Reference Survey (MARS) databases.
 *
 * \sa LocalNormalizationData, DrizzleData
 */
class PCL_CLASS FluxCalibrationData
{
public:

   /*!
    * Default constructor. Constructs an empty %FluxCalibrationData object.
    */
   FluxCalibrationData() = default;

   /*!
    * Copy constructor.
    */
   FluxCalibrationData( const FluxCalibrationData& ) = default;

   /*!
    * Move constructor.
    */
   FluxCalibrationData( FluxCalibrationData&& ) = default;

   /*!
    * Constructs a new %FluxCalibrationData instance by loading and parsing an
    * existing file.
    *
    * \param filePath   Path to an existing file that will be parsed. The file
    *                   contents must be in %XML flux calibration data format
    *                   (normally, a file with the .xfcl suffix).
    *
    * This constructor validates the data retrieved from the specified file. It
    * throws the appropriate Error exceptions in the event of parsing errors or
    * invalid data.
    */
   FluxCalibrationData( const String& filePath )
   {
      Parse( filePath );
   }

   /*!
    * Constructs a new %FluxCalibrationData instance by parsing a well-formed
    * %XML document.
    *
    * \param xml        Reference to the source %XML document. This constructor
    *                   expects an %XML document in valid %XFCL flux
    *                   calibration data format (.xfcl).
    *
    * This constructor validates the data retrieved from the specified %XML
    * document. It throws an Error exception in the event of an invalid
    * document or invalid data.
    */
   FluxCalibrationData( const XMLDocument& xml )
   {
      Parse( xml );
   }

   /*!
    * Constructs a new %FluxCalibrationData instance by parsing an %XML
    * element.
    *
    * \param element    Reference to the source %XML element. This constructor
    *                   expects an %XML document tree in valid %XFCL flux
    *                   calibration data format (.xfcl) rooted at this element.
    *
    * This constructor validates the data retrieved from the specified %XML
    * element. It throws an Error exception in the event of invalid data.
    */
   FluxCalibrationData( const XMLElement& element )
   {
      Parse( element );
   }

   /*!
    * Virtual destructor.
    */
   virtual ~FluxCalibrationData()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   FluxCalibrationData& operator =( const FluxCalibrationData& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   FluxCalibrationData& operator =( FluxCalibrationData&& ) = default;

   /*!
    * Returns the full path to the flux calibration target image represented by
    * this instance.
    *
    * The file identified by this function stores the input image of the flux
    * calibration task, e.g. as processed by the
    * SpectrophotometricFluxCalibration (SPFC) tool.
    *
    * \sa SetSourceFilePath()
    */
   const String& TargetFilePath() const
   {
      return m_targetFilePath;
   }

   /*!
    * Sets the path to the flux calibration target image represented by this
    * instance.
    *
    * \sa TargetFilePath()
    */
   void SetTargetFilePath( const String& filePath )
   {
      m_targetFilePath = File::FullPath( filePath );
   }

   /*!
    * Returns the vector of per-channel flux scale factors for the calibration
    * target image represented by this instance.
    *
    * Returns a reference to an empty vector if this object has not been
    * initialized with valid flux calibration data.
    *
    * \sa SetFluxScaleFactors()
    */
   const DVector& FluxScaleFactors() const
   {
      return m_fluxScaleFactors;
   }

   /*!
    * Sets a new vector of per-channel flux scale factors for the calibration
    * target image represented by this instance.
    *
    * \sa FluxScaleFactors()
    */
   void SetFluxScaleFactors( const DVector& v )
   {
      m_fluxScaleFactors = v;
   }

   /*!
    * Returns the vector of per-channel flux calibration dispersion estimates
    * for the calibration target image represented by this instance.
    *
    * Returns a reference to an empty vector if this object has not been
    * initialized with valid flux calibration data.
    *
    * \sa SetFluxScaleSigmas()
    */
   const DVector& FluxScaleSigmas() const
   {
      return m_fluxScaleSigmas;
   }

   /*!
    * Sets a new vector of per-channel flux calibration dispersion estimates
    * for the calibration target image represented by this instance.
    *
    * \sa FluxScaleSigmas()
    */
   void SetFluxScaleSigmas( const DVector& v )
   {
      m_fluxScaleSigmas = v;
   }

   /*!
    * Returns the vector of per-channel X-axis, mean PSF full width at half
    * maximum (FWHM) estimates for the calibration target image represented by
    * this instance.
    *
    * For each measured star, the X-axis corresponds to the major axis of the
    * fitted elliptical point spread function. For mean flux calibration data
    * results, a Gaussian PSF is assumed.
    *
    * Returns a reference to an empty vector if this object has not been
    * initialized with valid flux calibration data, or if PSF FWHM data is not
    * available in this instance.
    *
    * \sa SetMeanFWHMx()
    */
   const FVector& MeanFWHMx() const
   {
      return m_fwhmx;
   }

   /*!
    * Sets a new vector of per-channel X-axis, mean PSF full width at half
    * maximum (FWHM) estimates for the calibration target image represented by
    * this instance.
    *
    * \sa MeanFWHMx()
    */
   void SetMeanFWHMx( const FVector& v )
   {
      m_fwhmx = v;
   }

   /*!
    * Returns the vector of per-channel Y-axis, mean PSF full width at half
    * maximum (FWHM) estimates for the calibration target image represented by
    * this instance.
    *
    * For each measured star, the Y-axis corresponds to the minor axis of the
    * fitted elliptical point spread function. For mean flux calibration data
    * results, a Gaussian PSF is assumed.
    *
    * Returns a reference to an empty vector if this object has not been
    * initialized with valid flux calibration data, or if PSF FWHM data is not
    * available in this instance.
    *
    * \sa SetMeanFWHMy()
    */
   const FVector& MeanFWHMy() const
   {
      return m_fwhmy;
   }

   /*!
    * Sets a new vector of per-channel Y-axis, mean PSF full width at half
    * maximum (FWHM) estimates for the calibration target image represented by
    * this instance.
    *
    * \sa MeanFWHMy()
    */
   void SetMeanFWHMy( const FVector& v )
   {
      m_fwhmy = v;
   }

   /*!
    * Returns the vector of per-channel measured star counts for the
    * calibration target image represented by this instance.
    *
    * Star counts refer to the sets of stars used for the calculation of robust
    * flux calibration factors. Note that this typically includes outlier
    * rejection procedures, so the actual numbers of measured stars are usually
    * larger than the values returned by this function.
    *
    * Returns a reference to an empty vector if this object has not been
    * initialized with valid flux calibration data, or if star count data is
    * not available in this instance.
    *
    * \sa SetStarCounts()
    */
   const IVector& StarCounts() const
   {
      return m_counts;
   }

   /*!
    * Sets a new vector of per-channel measured star counts for the
    * calibration target image represented by this instance.
    *
    * \sa StarCounts()
    */
   void SetStarCounts( const IVector& v )
   {
      m_counts = v;
   }

   /*!
    * Returns the normalization factor applied to raw flux calibration scale
    * factors.
    *
    * Normalization allows us to generate flux scale factors within manageable
    * ranges for typical deep-sky images, i.e., in [100,0.001] instead of
    * [1e-7,1e-12], for example. In current versions of the
    * SpectrophotometricFluxCalibration process, the normalization factor is
    * 1.0e+08. Although it is unlikely to happen, the normalization factor
    * might change in a future version. By including it in flux calibration
    * data, we can ensure compatibility with future calibration data.
    *
    * \sa SetScaleNormalizationFactor
    */
   double ScaleNormalizationFactor() const
   {
      return m_normalizationFactor;
   }

   /*!
    * Defines the normalization factor applied to raw flux calibration scale
    * factors.
    *
    * Calling this member function is not necessary for generation of new flux
    * calibration data if the default normalization factor is used (see the
    * __PCL_SPFC_NORMALIZATION_FACTOR macro definition).
    *
    * \sa ScaleNormalizationFactor()
    */
   void SetScaleNormalizationFactor( double k )
   {
      PCL_PRECONDITION( k > 0 )
      m_normalizationFactor = k;
   }

   /*!
    * Returns a version number for the flux calibration data in this instance.
    *
    * The value returned by this function normally corresponds to the version
    * of the SpectrophotometricFluxCalibration process used to generate the
    * flux calibration data. This version number can be useful to ensure
    * backward compatibility in future implementations.
    *
    * \sa SetVersion()
    */
   uint32 Version() const
   {
      return m_version;
   }

   /*!
    * Sets a version number for the flux calibration data in this instance.
    *
    * \sa Version()
    */
   void SetVersion( uint32 v )
   {
      m_version = v;
   }

   /*!
    * Returns the UTC time this flux calibration data was created, or an
    * invalid TimePoint instance if creation time information is not available.
    */
   TimePoint CreationTime() const
   {
      return m_creationTime;
   }

   /*!
    * Clears all the flux calibration data transported by this instance,
    * yielding an empty %FluxCalibrationData object.
    */
   void Clear();

   /*!
    * Loads and parses a flux calibration data file.
    *
    * \param filePath   Path to an existing file that will be parsed. The file
    *                   contents must be in %XML flux calibration data format
    *                   (normally, a file with the .xfcl suffix).
    *
    * All previous data transported by this instance will be replaced with new
    * data acquired from the specified file.
    *
    * This function validates the data retrieved from the specified file. It
    * throws an Error exception in the event of parsing errors or invalid data.
    */
   void Parse( const String& filePath );

   /*!
    * Parses a well-formed %XML document.
    *
    * \param xml        Reference to the source %XML document. This member
    *                   function expects an %XML document in valid %XFCL flux
    *                   calibration data format (.xfcl).
    *
    * All previous data transported by this instance will be replaced with new
    * data acquired from the specified %XML contents.
    *
    * This function validates the data retrieved from the specified %XML
    * document. It throws an Error exception in the event of an invalid
    * document or invalid data.
    */
   void Parse( const XMLDocument& xml );

   /*!
    * Parses an %XML element.
    *
    * \param element    Reference to the source %XML element. This function
    *                   expects an %XML document tree in valid %XFCL flux
    *                   calibration data format (.xfcl) rooted at this element.
    *
    * All previous data transported by this instance will be replaced with new
    * data acquired from the specified %XML contents.
    *
    * This function validates the data retrieved from the specified %XML
    * element. It throws an Error exception in the event of invalid data.
    */
   void Parse( const XMLElement& element );

   /*!
    * Serializes the flux calibration data transported by this object as a new
    * %XML document in .xfcl format.
    *
    * The returned XMLDocument object has been allocated dynamically. The
    * caller is responsible for destroying and deallocating it as appropriate.
    */
   XMLDocument* Serialize() const;

   /*!
    * Serializes the flux calibration data transported by this object as a new
    * %XML document file in .xfcl format. The file will be newly created at the
    * specified file \a path.
    *
    * \warning If a file already exists at the specified \a path, its previous
    * contents will be lost after calling this function.
    */
   void SerializeToFile( const String& path ) const;

private:

   String    m_targetFilePath;
   Vector    m_fluxScaleFactors;
   Vector    m_fluxScaleSigmas;
   FVector   m_fwhmx;
   FVector   m_fwhmy;
   IVector   m_counts;
   double    m_normalizationFactor = __PCL_SPFC_NORMALIZATION_FACTOR;
   uint32    m_version;
   TimePoint m_creationTime;

   friend class FluxCalibrationDataPrivate;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FluxCalibrationData_h

// ----------------------------------------------------------------------------
// EOF pcl/FluxCalibrationData.h - Released 2024-12-27T18:16:01Z
