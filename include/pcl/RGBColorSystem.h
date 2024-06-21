//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// pcl/RGBColorSystem.h - Released 2024-06-18T15:48:54Z
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

#ifndef __PCL_RGBColorSystem_h
#define __PCL_RGBColorSystem_h

/// \file pcl/RGBColorSystem.h

#include <pcl/Defs.h>

#include <pcl/Math.h>
#include <pcl/ReferenceCounter.h>
#include <pcl/Vector.h>

#define _1_3            3.333333333333333e-01 // 1/3
#define _1_6            1.666666666666667e-01 // 1/6
#define _2_3            6.666666666666667e-01 // 2/3
#define _16_116         1.379310344827586e-01 // 16/116
#define CIEEpsilon      8.856451679035631e-03 // 216/24389
#define CIEKappa        9.032962962962963e+02 // 24389/27
#define CIEKappa116     7.787037037037037e+00 // CIEKappa/116
#define CIED501931XR    0.96422 // D50 (1931) mX reference white (ASTM E308-01)
#define CIED501931ZR    0.82521 // D50 (1931) mZ reference white (ASTM E308-01)
#define CIED501964XR    0.96720 // D50 (1964) mX reference white (ASTM E308-01)
#define CIED501964ZR    0.81427 // D50 (1964) mZ reference white (ASTM E308-01)

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class RGBColorSystem
 * \brief Colorimetrically defined RGB working color space
 *
 * %RGBColorSystem is a rigorous and efficient RGB working space (RGBWS)
 * implementation. This class provides all conversions between the supported
 * color spaces: RGB, CIE XYZ, CIE L*a*b*, CIE L*c*h* and grayscale, plus the
 * HSV and HSI color ordering systems, which are also supported for
 * convenience, although not linked to any particular RGBWS.
 *
 * Color components and channel values are represented as real scalars using
 * 64-bit floating point variables. Normalization of components and channel
 * values to the standard real range [0,1] is ensured for all supported color
 * spaces and color ordering systems. All internal transformations to and from
 * native color space component ranges are performed transparently.
 *
 * Chrominance coordinates and luminance coefficients are always relative to
 * the D50 reference white in conformance with the ICC standard.
 *
 * \sa ImageColor, AbstractImage, GenericImage, ImageVariant
 */
class PCL_CLASS RGBColorSystem
{
public:

   /*!
    * The type used to represent components and channel values for all
    * supported color spaces and color ordering systems.
    */
   using sample = double;

   /*!
    * Constructs a %RGBColorSystem object as a new instance of the default RGB
    * working space. The default RGBWS is sRGB (adapted to D50) in current
    * versions of the PixInsight platform.
    */
   RGBColorSystem()
      : m_data( RGBColorSystem::sRGB.m_data )
   {
      if ( m_data != nullptr )
         m_data->Attach();
      else // should not happen!
         m_data = new Data( 2.2F/*gamma*/, true/*issRGB*/, sRGB_x, sRGB_y, sRGB_Y );
   }

   /*!
    * Constructs a %RGBColorSystem object as a copy of an existing instance.
    */
   RGBColorSystem( const RGBColorSystem& s )
      : m_data( s.m_data )
   {
      m_data->Attach();
   }

   /*!
    * Constructs a new %RGBColorSystem instance by its RGB working space
    * parameters.
    *
    * \param gamma   Gamma parameter of the RGB working space. Ignored if
    *                \a issRGB is true.
    *
    * \param issRGB  If true, this space uses the sRGB gamma function. If this
    *                parameter is false, the space uses a standard raise gamma
    *                function with the specified \a gamma exponent.
    *
    * \param x,y     Vectors of chromaticity coordinates relative to the D50
    *                reference white.
    *
    * \param Y       Vector of luminance coefficients relative to D50. If not
    *                specified, or if an empty vector is specified, luminance
    *                coefficients will be calculated internally from the
    *                specified color primaries and the D50 reference white
    *                components.
    */
   RGBColorSystem( float gamma, bool issRGB, const FVector& x, const FVector& y, const FVector& Y = FVector() )
   {
      m_data = new Data( gamma, issRGB, x, y, Y );
   }

   /*!
    * Constructs a new %RGBColorSystem instance by its RGB working space
    * parameters.
    *
    * \param gamma   Gamma parameter of the RGB working space. Ignored if
    *                \a issRGB is true.
    *
    * \param issRGB  If true, this space uses the sRGB gamma function. If this
    *                parameter is false, the space uses a standard raise gamma
    *                function with the specified \a gamma exponent.
    *
    * \param x,y     Arrays of chromaticity coordinates relative to the D50
    *                reference white.
    *
    * \param Y       Array of luminance coefficients relative to D50. If not
    *                specified, or if a null pointer is specified, luminance
    *                coefficients will be calculated internally from the
    *                specified color primaries and the D50 reference white
    *                components.
    */
   RGBColorSystem( float gamma, bool issRGB, const float* x, const float* y, const float* Y = nullptr )
   {
      m_data = new Data( gamma, issRGB, FVector( x, 3 ), FVector( y, 3 ), (Y != nullptr) ? FVector( Y, 3 ) : FVector() );
   }

   /*!
    * Virtual destructor.
    */
   virtual ~RGBColorSystem()
   {
      if ( m_data != nullptr )
      {
         DetachFromData();
         m_data = nullptr;
      }
   }

   /*!
    * Returns true iff this %RGBColorSystem object uniquely references its RGB
    * working space data.
    */
   bool IsUnique() const
   {
      return m_data->IsUnique();
   }

   /*!
    * Returns true iff this %RGBColorSystem object is an alias of another
    * %RGBColorSystem instance \a s.
    *
    * Two instances of %RGBColorSystem are aliases if both share the same RGB
    * working space data.
    */
   bool IsAliasOf( const RGBColorSystem& s ) const
   {
      return m_data == s.m_data;
   }

   /*!
    * Ensures that this %RGBColorSystem object uniquely references its RGB
    * working space data.
    *
    * If necessary, this member function generates a duplicate of the RGB
    * working space data, references it, and decrements the reference counter
    * of the original data.
    */
   void EnsureUnique()
   {
      if ( !IsUnique() )
      {
         Data* newData = new Data( *m_data );
         DetachFromData();
         m_data = newData;
      }
   }

   /*!
    * Returns the gamma value of this RGB working space.
    *
    * \note If this space uses a sRGB gamma function, the returned value is
    * 2.2, but the space doesn't use that value as the exponent of a standard
    * power-law gamma function. sRGB defines a special, piecewise component
    * delinearization function; see the references for detailed information.
    */
   float Gamma() const
   {
      return m_data->gamma;
   }

   /*!
    * Returns true iff this space uses a sRGB gamma function.
    */
   bool IsSRGB() const
   {
      return m_data->issRGB;
   }

   /*!
    * Returns true iff this space uses a linear gamma function.
    *
    * A linear RGB space has gamma=1 and doesn't use a sRGB gamma function.
    */
   bool IsLinear() const
   {
      return m_data->isLinear;
   }

   /*!
    * Returns a reference to a vector with the elements of the 3x3 matrix for
    * conversion from RGB to CIE XYZ color spaces.
    *
    * The RGB-to-XYZ conversion matrix is a function of the reference white
    * (always D50 in this implementation) and the chromaticity coordinates that
    * define this RGB working color space.
    *
    * The nine matrix elements are stored contiguously in row order in the
    * returned vector: M[0][0], M[0][1], ..., M[2][2].
    */
   const Vector& RGBToXYZMatrix() const
   {
      return m_data->M;
   }

   /*!
    * Returns a reference to a vector with the elements of the 3x3 inverse
    * matrix for conversion from CIE XYZ to RGB color spaces.
    *
    * The XYZ-to-RGB inverse conversion matrix is a function of the reference
    * white (always D50 in this implementation) and the chromaticity
    * coordinates that define this RGB working color space.
    *
    * The nine inverse matrix elements are stored contiguously in row order in
    * the returned vector: M_[0][0], M_[0][1], ..., M_[2][2].
    */
   const Vector& XYZToRGBMatrix() const
   {
      return m_data->M_;
   }

   /*!
    * Returns the scaling factor applied for normalization of CIE X components.
    *
    * This normalization factor is the sum of the elements in the first row of
    * the XYZ-to-RGB conversion matrix: M[0][0] + M[0][1] + M[0][2].
    *
    * Normalization applies linear transformations to ensure that the values of
    * all color space components are always represented in the [0,1] range,
    * irrespective of RGBWS parameters.
    */
   double CIEXNormalizationFactor() const
   {
      return m_data->mX;
   }

   /*!
    * Returns the scaling factor applied for normalization of CIE Z components.
    *
    * This normalization factor is the sum of the elements in the third row of
    * the XYZ-to-RGB conversion matrix: M[2][0] + M[2][1] + M[2][2].
    *
    * Normalization applies linear transformations to ensure that the values of
    * all color space components are always represented in the [0,1] range,
    * irrespective of RGBWS parameters.
    */
   double CIEZNormalizationFactor() const
   {
      return m_data->mZ;
   }

   /*!
    * Returns the additive constant applied for normalization of CIE a*
    * components.
    */
   double CIEaNormalizationOffset() const
   {
      return m_data->zA;
   }

   /*!
    * Returns the scaling factor applied for normalization of CIE a*
    * components.
    */
   double CIEaNormalizationFactor() const
   {
      return m_data->mA;
   }

   /*!
    * Returns the additive constant applied for normalization of CIE b*
    * components.
    */
   double CIEbNormalizationOffset() const
   {
      return m_data->zB;
   }

   /*!
    * Returns the scaling factor applied for normalization of CIE b*
    * components.
    */
   double CIEbNormalizationFactor() const
   {
      return m_data->mB;
   }

   /*!
    * Returns the scaling factor applied for normalization of CIE c*
    * components.
    */
   double CIEcNormalizationFactor() const
   {
      return m_data->mC;
   }

   /*!
    * Returns a reference to the vector of X chromaticity coordinates in this
    * RGB working space.
    *
    * In the current PixInsight platform, chromaticity coordinates are always
    * relative to the D50 reference white.
    */
   const FVector& ChromaticityXCoordinates() const
   {
      return m_data->x;
   }

   /*!
    * Returns a reference to the vector of Y chromaticity coordinates in this
    * RGB working space.
    *
    * In the current PixInsight platform, chromaticity coordinates are always
    * relative to the D50 reference white.
    */
   const FVector& ChromaticityYCoordinates() const
   {
      return m_data->y;
   }

   /*!
    * Returns a reference to the vector of luminance coefficients in this RGB
    * working space.
    *
    * In the current PixInsight platform, luminance coefficients are always
    * relative to the D50 reference white.
    */
   const FVector& LuminanceCoefficients() const
   {
      return m_data->Y;
   }

   /*!
    * Returns true iff two %RGBColorSystem instances define the same RGB
    * working space. This happens when either both instances are aliases, or if
    * they define exactly the same RGB space parameters.
    */
   friend bool operator ==( const RGBColorSystem& S1, const RGBColorSystem& S2 )
   {
      return S1.IsAliasOf( S2 ) || *S1.m_data == *S2.m_data;
   }

   /*!
    * Causes this %RGBColorSystem instance to reference the same RGB working
    * space as another instance.
    *
    * The reference count of the previously referenced space is decremented,
    * and the previous space is deleted if it becomes unreferenced. The
    * reference count of the new space is then incremented.
    */
   void Assign( const RGBColorSystem& rgbws )
   {
      rgbws.m_data->Attach();
      DetachFromData();
      m_data = rgbws.m_data;
   }

   /*!
    * Assignment iterator. Returns a reference to this object.
    *
    * This operator calls Assign() with the specified source space \a rgbws.
    */
   RGBColorSystem& operator =( const RGBColorSystem& rgbws )
   {
      Assign( rgbws );
      return *this;
   }

   /*!
    * Returns the lightness component in the CIE L*a*b* space corresponding to
    * a set of RGB components.
    *
    * \param R,G,B   RGB components from which lightness will be calculated.
    *
    * \note A synonym for this function is CIEL().
    */
   sample Lightness( sample R, sample G, sample B ) const
   {
      XYZLab( R = m_data->CIEY( R, G, B ) );
      return sample( (1.16 * R) - 0.16 );
   }

   // ### Deprecated - retained for compatibility -> suppress in PCL 2.x
   sample Luminance( sample R, sample G, sample B ) const
   {
      return Lightness( R, G, B );
   }

   /*!
    * A synonym for Lightness().
    */
   sample CIEL( sample R, sample G, sample B ) const
   {
      return Lightness( R, G, B );
   }

   /*!
    * Calculates the lightness component in the CIE L*a*b* space corresponding
    * to a set of RGB components and copies it to a variable.
    *
    * \param[out] K  Reference to an output variable for the calculated
    *                lightness component.
    *
    * \param R,G,B   Input RGB components.
    *
    * \sa Lightness(), CIEL()
    */
   void RGBToGray( sample& K, sample R, sample G, sample B ) const
   {
      K = Lightness( R, G, B );
   }

   /*!
    * Returns the Value channel value in the HSV color ordering system,
    * corresponding to a specified set of RGB components.
    *
    * \param R,G,B   Input RGB components.
    *
    * \note This utility function is provided for convenience; HSV is not a
    * color space, but a color ordering system not based on an RGBWS.
    */
   sample Value( sample R, sample G, sample B ) const
   {
      return pcl::Max( pcl::Max( R, G ), B );
   }

   /*!
    * Returns the Intensity channel value in the HSI color ordering system,
    * corresponding to a specified set of RGB components.
    *
    * \param R,G,B   Input RGB components.
    *
    * \note HSI is more often known as HSL. However, we reserve the L channel
    * identifier exclusively for the CIE L* component (lightness) in PCL.
    *
    * \note This utility function is provided for convenience; HSI is not a
    * color space, but a color ordering system not based on an RGBWS.
    */
   sample Intensity( sample R, sample G, sample B ) const
   {
      return sample( 0.5*(   pcl::Min( pcl::Min( R, G ), B )
                           + pcl::Max( pcl::Max( R, G ), B ) ) );
   }

   /*!
    * Returns the Hue channel value in the HSV and HSI color ordering systems,
    * corresponding to a specified set of RGB components.
    *
    * \param R,G,B   Input RGB components.
    *
    * The returned value is a <em>normalized hue</em>, which is the hue angle
    * rescaled to the normalized [0,1) range. A normalized hue value of 1
    * corresponds to a hue angle of 2*pi radians, or 360 degrees.
    *
    * \note This utility function is provided for convenience; HSV and HSI are
    * not color spaces, but color ordering systems not based on an RGBWS.
    */
   sample Hue( sample R, sample G, sample B ) const
   {
      sample max   =       pcl::Max( pcl::Max( R, G ), B );
      sample delta = max - pcl::Min( pcl::Min( R, G ), B );
      if ( delta != 0 )
      {
         sample H;
         if ( R == max )
            H = (G - B)/delta;      // between yellow & magenta
         else if ( G == max )
            H = 2 + (B - R)/delta;  // between cyan & yellow
         else
            H = 4 + (R - G)/delta;  // between magenta & cyan

         H /= 6;
         if ( H < 0 )
            H += 1;
         return H;
      }

      // Achromatic case: R = G = B
      //    Hue is undefined (H is set to 0 conventionally)
      return 0;
   }

   /*!
    * Returns the Saturation channel value in the HSV color ordering system,
    * corresponding to a specified set of RGB components.
    *
    * \param R,G,B   Input RGB components.
    *
    * \note This utility function is provided for convenience; HSV is not a
    * color space but a color ordering system not based on an RGBWS.
    */
   sample HSVSaturation( sample R, sample G, sample B ) const
   {
      sample max   =       pcl::Max( pcl::Max( R, G ), B );
      sample delta = max - pcl::Min( pcl::Min( R, G ), B );
      return sample( (1.0 + max != 1.0) ? delta/max : 0.0 );
   }

   /*!
    * Returns the Saturation channel value in the HSI color ordering system,
    * corresponding to a specified set of RGB components.
    *
    * \param R,G,B   Input RGB components.
    *
    * \note HSI is more often known as HSL. However, we reserve the L channel
    * identifier exclusively for the CIE L* component (lightness) in PCL.
    *
    * \note This utility function is provided for convenience; HSI is not a
    * color space, but a color ordering system not based on an RGBWS.
    */
   sample HSISaturation( sample R, sample G, sample B ) const
   {
      sample min   = pcl::Min( pcl::Min( R, G ), B );
      sample max   = pcl::Max( pcl::Max( R, G ), B );
      sample delta = max - min;
      if ( delta != 0 )
      {
         sample sum = min + max;
         return delta/((sum <= 1) ? sum : 2-sum);
      }
      return sample( 0 );
   }

   /*!
    * Returns the Saturation channel value in the HSV color ordering system,
    * corresponding to a specified set of RGB components.
    *
    * \param R,G,B   Input RGB components.
    *
    * This function is a synonym for HSVSaturation().
    */
   sample Saturation( sample R, sample G, sample B ) const
   {
      return HSVSaturation( R, G, B );
   }

   /*!
    * Converts a set of RGB components to the corresponding channel values in
    * the HSV (Hue, Saturation, Value) color ordering system.
    *
    * \param[out] H,S,V    References to the variables where output HSV channel
    *                      values will be stored.
    *
    * \param R,G,B         Input RGB components.
    *
    * The output \a H value is a <em>normalized hue</em>, which is the hue
    * angle rescaled to the normalized [0,1) range. A normalized hue value of
    * one corresponds to a hue angle of 2*pi radians, or 360 degrees.
    *
    * \note This utility function is provided for convenience; HSV is not a
    * color space, but a color ordering system not based on an RGBWS.
    */
   static void RGBToHSV( sample& H, sample& S, sample& V, sample R, sample G, sample B )
   {
      V = pcl::Max( pcl::Max( R, G ), B ); // V = Value( R, G, B );

      sample delta = V - pcl::Min( pcl::Min( R, G ), B );
      if ( delta != 0 )
      {
         S = delta/V;

         if ( R == V )
            H = (G - B)/delta;      // between yellow & magenta
         else if ( G == V )
            H = 2 + (B - R)/delta;  // between cyan & yellow
         else
            H = 4 + (R - G)/delta;  // between magenta & cyan

         H /= 6;
         if ( H < 0 )
            H += 1;
      }
      else
      {
         // Achromatic case: R = G = B
         //    S = 0, and H is undefined (H is set to 0 conventionally)
         S = H = 0;
      }
   }

   /*!
    * Converts a set of RGB components to the corresponding channel values in
    * the HSI (Hue, Saturation, Intensity) color ordering system.
    *
    * \param[out] H,S,I    References to the variables where output HSI channel
    *                      values will be stored.
    *
    * \param R,G,B         Input RGB components.
    *
    * The output \a H value is a <em>normalized hue</em>, which is the hue
    * angle rescaled to the normalized [0,1) range. A normalized hue value of
    * one corresponds to a hue angle of 2*pi radians, or 360 degrees.
    *
    * \note HSI is more often known as HSL. However, we reserve the L
    * identifier exclusively for the CIE L* component (lightness) in PCL.
    *
    * \note This utility function is provided for convenience; HSV is not a
    * color space, but a color ordering system not based on an RGBWS.
    */
   static void RGBToHSI( sample& H, sample& S, sample& I, sample R, sample G, sample B )
   {
      sample min   = pcl::Min( pcl::Min( R, G ), B );
      sample max   = pcl::Max( pcl::Max( R, G ), B );
      sample delta = max - min;
      sample sum   = min + max;

      I = 0.5*sum;

      if ( delta != 0 )
      {
         S = delta/((sum <= 1) ? sum : 2-sum);

         if ( R == max )
            H = (G - B)/delta;      // between yellow & magenta
         else if ( G == max )
            H = 2 + (B - R)/delta;  // between cyan & yellow
         else
            H = 4 + (R - G)/delta;  // between magenta & cyan

         H /= 6;
         if ( H < 0 )
            H += 1;
      }
      else
      {
         // Achromatic case: R = G = B
         //    S = 0, and H is undefined (H is set to 0 conventionally)
         S = H = 0;
      }
   }

   /*!
    * Converts a set of RGB components to the corresponding channel values in
    * the HSV (Hue, Saturation, Value) color ordering system, plus the L*
    * component (lightness) in the CIE L*a*b* space.
    *
    * \param[out] H,S,V    References to the variables where output HSV channel
    *                      values will be stored.
    *
    * \param[out] L        Reference to a variable where the output CIE L*
    *                      component will be stored.
    *
    * \param R,G,B   Input RGB components.
    *
    * The output \a H value is a <em>normalized hue</em>, which is the hue
    * angle rescaled to the normalized [0,1) range. A normalized hue value of
    * one corresponds to a hue angle of 2*pi radians, or 360 degrees.
    */
   void RGBToHSVL( sample& H, sample& S, sample& V, sample& L, sample R, sample G, sample B ) const
   {
      RGBToHSV( H, S, V, R, G, B );
      L = Lightness( R, G, B );
   }

   /*!
    * Converts a set of RGB components to the corresponding channel values in
    * the HSI (Hue, Saturation, Intensity) color ordering system, plus the L*
    * component (lightness) in the CIE L*a*b* space.
    *
    * \param[out] H,S,I    References to the variables where output HSI channel
    *                      values will be stored.
    *
    * \param[out] L        Reference to a variable where the output CIE L*
    *                      component will be stored.
    *
    * \param R,G,B   Input RGB components.
    *
    * The output \a H value is a <em>normalized hue</em>, which is the hue
    * angle rescaled to the normalized [0,1) range. A normalized hue value of
    * one corresponds to a hue angle of 2*pi radians, or 360 degrees.
    *
    * \note HSI is more often known as HSL. However, we reserve the L
    * identifier exclusively for the CIE L* component (lightness) in PCL.
    */
   void RGBToHSIL( sample& H, sample& S, sample& I, sample& L, sample R, sample G, sample B ) const
   {
      RGBToHSI( H, S, I, R, G, B );
      L = Lightness( R, G, B );
   }

   /*!
    * Conversion from the RGB color space to the CIE XYZ color space.
    *
    * \param[out] X,Y,Z    References to the variables where output CIE XYZ
    *                      components will be stored.
    *
    * \param R,G,B         Input RGB components.
    */
   void RGBToCIEXYZ( sample& X, sample& Y, sample& Z, sample R, sample G, sample B ) const
   {
      m_data->RGBToCIEXYZ( X, Y, Z, R, G, B );
   }

   /*!
    * Calculates the chrominance X, Z components in the CIE XYZ color space
    * corresponding to a specified set of RGB components.
    *
    * \param[out] X,Z   References to the variables where output CIE X and Z
    *                   components will be stored.
    *
    * \param R,G,B      Input RGB components.
    *
    * This function avoids the calculation of the CIE Y component, which saves
    * time when only the chrominance components are required.
    */
   void RGBToCIEXZ( sample& X, sample& Z, sample R, sample G, sample B ) const
   {
      m_data->RGBToCIEXZ( X, Z, R, G, B );
   }

   /*!
    * CIE X component from RGB components.
    *
    * \param R,G,B   Input RGB components.
    */
   sample CIEX( sample R, sample G, sample B ) const
   {
      return m_data->CIEX( R, G, B );
   }

   /*!
    * CIE Y component from RGB components.
    *
    * \param R,G,B   Input RGB components.
    */
   sample CIEY( sample R, sample G, sample B ) const
   {
      return m_data->CIEY( R, G, B );
   }

   /*!
    * CIE Z component from RGB components.
    *
    * \param R,G,B   Input RGB components.
    */
   sample CIEZ( sample R, sample G, sample B ) const
   {
      return m_data->CIEZ( R, G, B );
   }

   /*!
    * CIE L* component from CIE Y.
    *
    * \param Y    Input CIE Y component.
    */
   sample CIEYToCIEL( sample Y ) const
   {
      XYZLab( Y );
      return 1.16*Y - 0.16;
   }

   /*!
    * CIE Y component from CIE L*.
    *
    * \param L    Input CIE L* component.
    */
   sample CIELToCIEY( sample L ) const
   {
      L = (L + 0.16)/1.16;
      LabXYZ( L );
      return L;
   }

   /*!
    * Conversion from RGB to the CIE L*a*b* color space.
    *
    * \param[out] L,a,b    References to the variables where output normalized
    *                      CIE L*a*b* components will be stored.
    *
    * \param R,G,B         Input RGB components.
    */
   void RGBToCIELab( sample& L, sample& a, sample& b, sample R, sample G, sample B ) const
   {
      sample X, Y, Z;
      m_data->RGBToCIEXYZ( X, Y, Z, R, G, B );
      XYZLab( X ); XYZLab( Y ); XYZLab( Z );
      L = m_data->Range( sample( (1.16 * Y) - 0.16 ) );
      a = m_data->Range( (5*(X - Y) + m_data->zA)/m_data->mA );
      b = m_data->Range( (2*(Y - Z) + m_data->zB)/m_data->mB );
   }

   /*!
    * Calculates the normalized chrominance components of the CIE L*a*b* color
    * space corresponding to a specified set of RGB components.
    *
    * \param[out] a,b   References to the variables where output normalized CIE
    *                   a* and CIE b* components will be stored.
    *
    * \param R,G,B      Input RGB components.
    */
   void RGBToCIEab( sample& a, sample& b, sample R, sample G, sample B ) const
   {
      sample X, Y, Z;
      m_data->RGBToCIEXYZ( X, Y, Z, R, G, B );
      XYZLab( X ); XYZLab( Y ); XYZLab( Z );
      a = m_data->Range( (5*(X - Y) + m_data->zA)/m_data->mA );
      b = m_data->Range( (2*(Y - Z) + m_data->zB)/m_data->mB );
   }

   /*!
    * Returns the normalized a* chrominance component of the CIE L*a*b* color
    * space corresponding to a specified set of RGB components.
    *
    * \param R,G,B   Input RGB components.
    */
   sample CIEa( sample R, sample G, sample B ) const
   {
      sample X, Y;
      m_data->RGBToCIEXY( X, Y, R, G, B );
      XYZLab( X ); XYZLab( Y );
      return m_data->Range( (5*(X - Y) + m_data->zA)/m_data->mA );
   }

   /*!
    * Returns the normalized b* chrominance component of the CIE L*a*b* color
    * space corresponding to a specified set of RGB components.
    *
    * \param R,G,B   Input RGB components.
    *
    * This function avoids the calculation of the L* and a* components, which
    * saves time when only the b* chrominance component is required.
    */
   sample CIEb( sample R, sample G, sample B ) const
   {
      sample Y, Z;
      m_data->RGBToCIEYZ( Y, Z, R, G, B );
      XYZLab( Y ); XYZLab( Z );
      return m_data->Range( (2*(Y - Z) + m_data->zB)/m_data->mB );
   }

   /*!
    * Returns the normalized c* chrominance component of the CIE L*c*h* color
    * space corresponding to a specified set of RGB components.
    *
    * \param R,G,B   Input RGB components.
    *
    * This function avoids the calculation of the L* and h* components, which
    * saves time when only the c* chrominance component is required.
    */
   sample CIEc( sample R, sample G, sample B ) const
   {
      sample X, Y, Z;
      m_data->RGBToCIEXYZ( X, Y, Z, R, G, B );
      XYZLab( X ); XYZLab( Y ); XYZLab( Z );
      sample a = 5*(X - Y);
      sample b = 2*(Y - Z);
      return m_data->Range( pcl::Sqrt( a*a + b*b )/m_data->mC );
   }

   /*!
    * Returns the normalized h* chrominance component of the CIE L*c*h* color
    * space corresponding to a specified set of RGB components.
    *
    * \param R,G,B   Input RGB components.
    *
    * This function avoids the calculation of the L* and c* components, which
    * saves time when only the h* chrominance component is required.
    *
    * The returned value is the hue angle normalized to the range [0,1), where
    * 0 corresponds to a hue angle of zero degrees, and 1 to an angle of 360
    * degrees (2*pi radians).
    */
   sample CIEh( sample R, sample G, sample B ) const
   {
      return CIEhr( R, G, B )/Const<sample>::_2pi();
   }

   /*!
    * Returns the normalized h* chrominance component of the CIE L*c*h* color
    * space, expressed in radians, corresponding to a specified set of RGB
    * components.
    *
    * \param R,G,B   Input RGB components.
    *
    * This function avoids the calculation of the L* and c* channels, which
    * saves time when only the h* chrominance component is required.
    *
    * The returned value is the hue angle in radians, in the range [0,2pi).
    */
   sample CIEhr( sample R, sample G, sample B ) const
   {
      sample X, Y, Z;
      m_data->RGBToCIEXYZ( X, Y, Z, R, G, B );
      XYZLab( X ); XYZLab( Y ); XYZLab( Z );
      sample h = ArcTan( 2*(Y - Z), 5*(X - Y) );
      if ( h < 0 )
         h += Const<sample>::_2pi();
      return h;
   }

   /*!
    * Conversion from the RGB color space to the CIE L*c*h* color space.
    *
    * \param[out] L,c,h    References to the variables where output normalized
    *                      CIE L*c*h* components will be stored.
    *
    * \param R,G,B         Input RGB components.
    */
   void RGBToCIELch( sample& L, sample& c, sample& h, sample R, sample G, sample B ) const
   {
      sample X, Y, Z;
      m_data->RGBToCIEXYZ( X, Y, Z, R, G, B );
      XYZLab( X ); XYZLab( Y ); XYZLab( Z );
      L = m_data->Range( sample( (1.16 * Y) - 0.16 ) );
      sample a = 5*(X - Y);
      sample b = 2*(Y - Z);
      c = m_data->Range( pcl::Sqrt( a*a + b*b )/m_data->mC );
      h = ArcTan( b, a );
      if ( h < 0 )
         h += Const<sample>::_2pi();
      h /= Const<sample>::_2pi();
   }

   /*!
    * Conversion from the RGB color space to the CIE L*a*b* color space with
    * optimized, on-the-fly calculation of the CIE c* component.
    *
    * \param[out] L,a,b    References to the variables where output normalized
    *                      CIE L*a*b* components will be stored.
    *
    * \param[out] c        Reference to a variable where the output normalized
    *                      CIE c* component will be stored.
    *
    * \param R,G,B         Input RGB components.
    *
    * If only the CIE c* component is required besides CIE L*a*b* components,
    * this function is much faster than performing two separate conversions to
    * the CIE L*a*b* and L*c*h* color spaces.
    */
   void RGBToCIELabc( sample& L, sample& a, sample& b, sample& c, sample R, sample G, sample B ) const
   {
      sample X, Y, Z;
      m_data->RGBToCIEXYZ( X, Y, Z, R, G, B );
      XYZLab( X ); XYZLab( Y ); XYZLab( Z );
      L = m_data->Range( sample( (1.16 * Y) - 0.16 ) );
      a = m_data->Range( ((X = 5*(X - Y)) + m_data->zA)/m_data->mA );
      b = m_data->Range( ((Z = 2*(Y - Z)) + m_data->zB)/m_data->mB );
      c = m_data->Range( pcl::Sqrt( X*X + Z*Z )/m_data->mC );
   }

   /*!
    * Conversion from the HSV color ordering system to the RGB color space.
    *
    * \param[out] R,G,B    References to the variables where output normalized
    *                      RGB components will be stored.
    *
    * \param H,S,V         Input HSV channel values.
    *
    * The input \a H value must be a <em>normalized hue</em>, which is the hue
    * angle rescaled to the normalized [0,1) range. A normalized hue value of
    * one corresponds to a hue angle of 2*pi radians, or 360 degrees.
    *
    * \note This utility function is provided for convenience; HSV is not a
    * color space, but a color ordering system not based on an RGBWS.
    */
   static void HSVToRGB( sample& R, sample& G, sample& B, sample H, sample S, sample V )
   {
      if ( S != 0 )
      {
         H *= 6;  // degrees -> quadrant index

         int i = TruncInt( Floor( H ) ); // i = sector 0 to 5
         sample f = H - i;               // f = fractional part of H
         sample p = V*(1 - S);
         sample q = V*(1 - S*f);
         sample t = V*(1 - S*(1 - f));

         switch( i )
         {
         case 0: R = V; G = t; B = p; break;
         case 1: R = q; G = V; B = p; break;
         case 2: R = p; G = V; B = t; break;
         case 3: R = p; G = q; B = V; break;
         case 4: R = t; G = p; B = V; break;
         case 5: R = V; G = p; B = q; break;
         default: R = G = B = V; break; // out-of-range H argument
         }
      }
      else
         R = G = B = V; // achromatic
   }

   /*!
    * Conversion from separate HSV chrominance and CIE L* components to the RGB
    * color space.
    *
    * \param[out] R,G,B    References to the variables where output normalized
    *                      RGB components will be stored.
    *
    * \param H,S,V         Input HSV channel values for chrominance.
    *
    * \param L             Input normalized CIE L* (lightness) component.
    *
    * The lightness implicitly defined by the input HSV values is discarded and
    * replaced with the specified CIE L* component.
    *
    * Strictly speaking, the resulting chrominance is not colorimetrically
    * defined, since it derives from HSV channel values and hence is not
    * defined in an RGB working space. However, the chrominance is supposed to
    * be expressed in the RGB working space referenced by this %RGBColorSystem
    * object.
    *
    * The input \a H value must be a <em>normalized hue</em>, which is the hue
    * angle rescaled to the normalized [0,1) range. A normalized hue value of
    * one corresponds to a hue angle of 2*pi radians, or 360 degrees.
    *
    * The primary usefulness of this function is implementing accurate
    * hue/saturation image transformations in the HSV system with complete
    * preservation of the colorimetrically defined lightness component.
    */
   void HSVLToRGB( sample& R, sample& G, sample& B, sample H, sample S, sample V, sample L ) const
   {
      HSVToRGB( R, G, B, H, S, V );
#define a   H
#define b   S
      RGBToCIEab( a, b, R, G, B );
      CIELabToRGB( R, G, B, L, a, b );
#undef a
#undef b
   }

   /*!
    * Conversion from the HSI color ordering system to the RGB color space.
    *
    * \param[out] R,G,B    References to the variables where output normalized
    *                      RGB components will be stored.
    *
    * \param H,S,I         Input HSI channel values.
    *
    * The input \a H value must be a <em>normalized hue</em>, which is the hue
    * angle rescaled to the normalized [0,1) range. A normalized hue value of
    * one corresponds to a hue angle of 2*pi radians, or 360 degrees.
    *
    * \note HSI is more often known as HSL. However, we reserve the L
    * identifier exclusively for the CIE L* component (lightness) in PCL.
    *
    * \note This utility function is provided for convenience; HSI is not a
    * color space, but a color ordering system not based on an RGBWS.
    */
   static void HSIToRGB( sample& R, sample& G, sample& B, sample H, sample S, sample I )
   {
      if ( S != 0 )
      {
         sample v2 = (I < 0.5) ? I*(1 + S) : I + S - S*I;
         sample v1 = I+I - v2;
         R = HSIH2RGB( v1, v2, H+_1_3 );
         G = HSIH2RGB( v1, v2, H      );
         B = HSIH2RGB( v1, v2, H-_1_3 );
      }
      else
         R = G = B = I; // achromatic
   }

   /*!
    * Transformation from separate HSI chrominance and normalized CIE L*
    * components to the RGB color space.
    *
    * \param[out] R,G,B    References to the variables where output normalized
    *                      RGB components will be stored.
    *
    * \param H,S,I         Input HSI channel values for chrominance.
    *
    * \param L             Input normalized CIE L* component (lightness).
    *
    * The lightness implicitly defined by the input HSI values is discarded
    * and replaced with the specified CIE L* component.
    *
    * Strictly speaking, the resulting chrominance is not colorimetrically
    * defined, since it derives from HSI channel values and hence is not
    * defined in an RGB working space. However, the chrominance is supposed to
    * be expressed in the RGB working space referenced by this %RGBColorSystem
    * object.
    *
    * The input \a H value must be a <em>normalized hue</em>, which is the hue
    * angle rescaled to the normalized [0,1) range. A normalized hue value of
    * one corresponds to a hue angle of 2*pi radians, or 360 degrees.
    *
    * The primary usefulness of this function is implementing accurate
    * hue/saturation image transformations in the HSI system with complete
    * preservation of the colorimetrically defined lightness component.
    *
    * \note HSI is more often known as HSL. However, we reserve the L
    * identifier exclusively for the CIE L* component (lightness) in PCL.
    */
   void HSILToRGB( sample& R, sample& G, sample& B, sample H, sample S, sample I, sample L ) const
   {
      HSIToRGB( R, G, B, H, S, I );
#define a   H
#define b   S
      RGBToCIEab( a, b, R, G, B );
      CIELabToRGB( R, G, B, L, a, b );
#undef a
#undef b
   }

   /*!
    * Conversion from CIE XYZ to RGB.
    *
    * \param[out] R,G,B    References to the variables where output normalized
    *                      RGB components will be stored.
    *
    * \param X,Y,Z         Input normalized CIE XYZ components.
    */
   void CIEXYZToRGB( sample& R, sample& G, sample& B, sample X, sample Y, sample Z ) const
   {
      m_data->CIEXYZToRGB( R, G, B, X, Y, Z );
   }

   /*!
    * Conversion from CIE XYZ to CIE L*a*b*.
    *
    * \param[out] L,a,b    References to the variables where output normalized
    *                      CIE L*a*b* components will be stored.
    *
    * \param X,Y,Z         Input normalized CIE XYZ components.
    */
   void CIEXYZToCIELab( sample& L, sample& a, sample& b, sample X, sample Y, sample Z ) const
   {
      XYZLab( X ); XYZLab( Y ); XYZLab( Z );
      L = m_data->Range( sample( (1.16 * Y) - 0.16 ) );
      a = m_data->Range( (5*(X - Y) + m_data->zA)/m_data->mA );
      b = m_data->Range( (2*(Y - Z) + m_data->zB)/m_data->mB );
   }

   /*!
    * Conversion from CIE L*a*b* to RGB.
    *
    * \param[out] R,G,B    References to the variables where output normalized
    *                      RGB components will be stored.
    *
    * \param L,a,b         Input normalized CIE L*a*b* components.
    */
   void CIELabToRGB( sample& R, sample& G, sample& B, sample L, sample a, sample b ) const
   {
      sample Y = sample( (L + 0.16)/1.16 );
      sample X = (m_data->mA*a - m_data->zA)/5 + Y;
      sample Z = Y - (m_data->mB*b - m_data->zB)/2;
      LabXYZ( X ); LabXYZ( Y ); LabXYZ( Z );
      m_data->CIEXYZToRGB( R, G, B, X, Y, Z );
   }

   /*!
    * Conversion from CIE L*a*b* to CIE XYZ.
    *
    * \param[out] X,Y,Z    References to the variables where output normalized
    *                      CIE XYZ components will be stored.
    *
    * \param L,a,b         Input normalized CIE L*a*b* components.
    */
   void CIELabToCIEXYZ( sample& X, sample& Y, sample& Z, sample L, sample a, sample b ) const
   {
      Y = sample( (L + 0.16)/1.16 );
      X = (m_data->mA*a - m_data->zA)/5 + Y;
      Z = Y - (m_data->mB*b - m_data->zB)/2;
      LabXYZ( X ); LabXYZ( Y ); LabXYZ( Z );
      X = m_data->Range( X );
      Y = m_data->Range( Y );
      Z = m_data->Range( Z );
   }

   /*!
    * Conversion from CIE L*a*b* to CIE L*c*h*.
    *
    * \param[out] L,c,h    References to the variables where output normalized
    *                      CIE L*c*h* components will be stored.
    *
    * \param L0,a,b        Input normalized CIE L*a*b* components.
    *
    * \note The output \a L component is always identical to the input \a L0
    * component. The \a L parameter of this function has been included for the
    * sake of coherence in function signatures.
    */
   void CIELabToCIELch( sample& L, sample& c, sample& h, sample L0, sample a, sample b ) const
   {
      L = m_data->Range( L0 );
      a = m_data->mA*a - m_data->zA;
      b = m_data->mB*b - m_data->zB;
      c = m_data->Range( Sqrt( a*a + b*b )/m_data->mC );
      h = ArcTan( b, a )/Const<sample>::pi();
      if ( h < 0 )
         h += 1;
   }

   /*!
    * Conversion from CIE L*c*h* to RGB.
    *
    * \param[out] R,G,B    References to the variables where output normalized
    *                      RGB components will be stored.
    *
    * \param L,c,h         Input normalized CIE L*c*h* components.
    */
   void CIELchToRGB( sample& R, sample& G, sample& B, sample L, sample c, sample h ) const
   {
      sample a, b;
      SinCos( h*Const<sample>::_2pi(), b, a );
      c *= m_data->mC;
      a *= c;
      b *= c;
      sample Y = sample( (L + 0.16)/1.16 );
      sample X = a/5 + Y;
      sample Z = Y - b/2;
      LabXYZ( X ); LabXYZ( Y ); LabXYZ( Z );
      m_data->CIEXYZToRGB( R, G, B, X, Y, Z );
   }

   /*!
    * Conversion from CIE L*c*h* to CIE L*a*b*.
    *
    * \param[out] L,a,b    References to the variables where output normalized
    *                      CIE L*a*b* components will be stored.
    *
    * \param L0,c,h        Input normalized CIE L*c*h* components.
    *
    * \note The output \a L component is always identical to the input \a L0
    * component. The \a L parameter of this function has been included for the
    * sake of coherence in function signatures.
    */
   void CIELchToCIELab( sample& L, sample& a, sample& b, sample L0, sample c, sample h ) const
   {
      L = m_data->Range( L0 );
      SinCos( h*Const<sample>::_2pi(), b, a );
      c *= m_data->mC;
      a = m_data->Range( (c*a + m_data->zA)/m_data->mA );
      b = m_data->Range( (c*b + m_data->zB)/m_data->mB );
   }

   /*!
    * The sRGB gamma function. Returns a linear color sample value for the
    * specified sample value \a x in the sRGB color space. \a x must be in the
    * normalized [0,1] range.
    */
   static double SRGBToLinear( double x )
   {
      PCL_PRECONDITION( x >= 0 && x <= 1 )
      return (x > 0.04045) ? Pow( (x + 0.055)/1.055, 2.4 ) : x/12.92;
   }

   /*!
    * The inverse sRGB gamma function. Returns a color sample value in the sRGB
    * color space for the specified linear sample value \a x. \a x must be in
    * the normalized [0,1] range.
    */
   static double LinearToSRGB( double x )
   {
      PCL_PRECONDITION( x >= 0 && x <= 1 )
      return (x > 0.0031308) ? 1.055*Pow( x, sample( 1/2.4 ) ) - 0.055 : 12.92*x;
   }

protected:

   struct Data : public ReferenceCounter
   {
      float gamma, gammaInv;  // gamma, 1/gamma; ignored if issRGB=true
      bool issRGB;   // true if sRGB gamma function is being used
      bool isLinear; // true if gamma=1.0 and issRGB=false, for optimization

      /*
       * Chromaticity coordinates w.r.t. the D50 reference white.
       */
      FVector x, y;

      /*
       * Luminance coefficients w.r.t. the D50 reference white.
       */
      FVector Y;

      /*
       * RGB <-> CIE XYZ transformation matrices.
       */
      Vector M;  // RGB -> CIE XYZ
      Vector M_; // CIE XYZ -> RGB

      /*
       * Normalization of CIE X and Z components.
       */
      double mX, mZ; // scale factors

      /*
       * Normalization of CIE L*, a*, b* and c* components.
       */
      double mA, mB, mC; // scale factors
      double zA, zB;     // zero offsets

      Data( float, bool, const FVector&, const FVector&, const FVector& );
      Data( float, bool, const float*, const float*, const float* );
      Data( const Data& ) = default;

      void Initialize();

      bool operator ==( const RGBColorSystem::Data& data ) const
      {
         return gamma == data.gamma &&
                issRGB == data.issRGB &&
                Y == data.Y &&
                x == data.x && y == data.y;
      }

      /*
       * Primary gamma functions.
       */

      void LinearRGB( sample& x ) const
      {
         x = sample( issRGB ? SRGBToLinear( double( x ) ) : Pow( double( x ), double( gamma ) ) );
      }

      void GammaRGB( sample& x ) const
      {
         x = sample( issRGB ? LinearToSRGB( double( x ) ) : Pow( double( x ), double( gammaInv ) ) );
      }

      /*
       * Primary linear transformations.
       */

      void RGBToCIEXYZ( sample& X, sample& Y, sample& Z, sample R, sample G, sample B ) const
      {
         if ( !isLinear )
         {
            LinearRGB( R );
            LinearRGB( G );
            LinearRGB( B );
         }

         X = Range( (R*M[0] + G*M[1] + B*M[2])/mX );
         Y = Range(  R*M[3] + G*M[4] + B*M[5]     );
         Z = Range( (R*M[6] + G*M[7] + B*M[8])/mZ );
      }

      void CIEXYZToRGB( sample& R, sample& G, sample& B, sample X, sample Y, sample Z ) const
      {
         X *= mX;
         Z *= mZ;
         R = Range( X*M_[0] + Y*M_[1] + Z*M_[2] );
         G = Range( X*M_[3] + Y*M_[4] + Z*M_[5] );
         B = Range( X*M_[6] + Y*M_[7] + Z*M_[8] );

         if ( !isLinear )
         {
            GammaRGB( R );
            GammaRGB( G );
            GammaRGB( B );
         }
      }

      /*
       * Partial transformations for fast luminance/chrominance calculations.
       */

      sample CIEX( sample R, sample G, sample B ) const
      {
         if ( !isLinear )
         {
            LinearRGB( R );
            LinearRGB( G );
            LinearRGB( B );
         }

         return Range( (R*M[0] + G*M[1] + B*M[2])/mX );
      }

      sample CIEY( sample R, sample G, sample B ) const
      {
         if ( !isLinear )
         {
            LinearRGB( R );
            LinearRGB( G );
            LinearRGB( B );
         }

         return Range( R*M[3] + G*M[4] + B*M[5] );
      }

      sample CIEZ( sample R, sample G, sample B ) const
      {
         if ( !isLinear )
         {
            LinearRGB( R );
            LinearRGB( G );
            LinearRGB( B );
         }

         return Range( (R*M[6] + G*M[7] + B*M[8])/mZ );
      }

      void RGBToCIEXY( sample& X, sample& Y, sample R, sample G, sample B ) const
      {
         if ( !isLinear )
         {
            LinearRGB( R );
            LinearRGB( G );
            LinearRGB( B );
         }

         X = Range( (R*M[0] + G*M[1] + B*M[2])/mX );
         Y = Range(  R*M[3] + G*M[4] + B*M[5] );
      }

      void RGBToCIEYZ( sample& Y, sample& Z, sample R, sample G, sample B ) const
      {
         if ( !isLinear )
         {
            LinearRGB( R );
            LinearRGB( G );
            LinearRGB( B );
         }

         Y = Range(  R*M[3] + G*M[4] + B*M[5] );
         Z = Range( (R*M[6] + G*M[7] + B*M[8])/mZ );
      }

      void RGBToCIEXZ( sample& X, sample& Z, sample R, sample G, sample B ) const
      {
         if ( !isLinear )
         {
            LinearRGB( R );
            LinearRGB( G );
            LinearRGB( B );
         }

         X = Range( (R*M[0] + G*M[1] + B*M[2])/mX );
         Z = Range( (R*M[6] + G*M[7] + B*M[8])/mZ );
      }

      static bool ValidateParameters( const FVector& x, const FVector& y, const FVector& Y );

      // Ensure output values stay within the nominal [0,1] range.
      static sample Range( const sample& x )
      {
         return pcl::Range( x, sample( 0 ), sample( 1 ) );
      }
   };

   Data* m_data = nullptr;

   void DetachFromData()
   {
      if ( !m_data->Detach() )
         delete m_data;
   }

   /*
    * CIE XYZ -> Lab component transformation function.
    */
   static void XYZLab( sample& x )
   {
      x = (x > CIEEpsilon) ? Pow( x, sample( _1_3 ) ) : sample( CIEKappa116*x + _16_116 );
   }

   /*
    * CIE Lab -> XYZ component transformation function.
    */
   static void LabXYZ( sample& x )
   {
      sample x3 = x*x*x;
      x = (x3 > CIEEpsilon) ? x3 : sample( (x - _16_116)/CIEKappa116 );
   }

   /*
    * HSI hue angle to RGB component conversion function.
    */
   static sample HSIH2RGB( sample v1, sample v2, sample H )
   {
      if ( H < 0 )
         H += 1;
      else if ( H > 1 )
         H -= 1;

      if ( H < _1_6 )
         return v1 + 6*H*(v2 - v1);

      if ( H < 0.5 )
         return v2;

      if ( H < _2_3 )
         return v1 + 6*(_2_3 - H)*(v2 - v1);

      return v1;
   }

   friend struct Data;

public:

   /*!
    * sRGB x chromaticity coordinates (D50).
    */
   static const float sRGB_x[ 3 ];

   /*!
    * sRGB y chromaticity coordinates (D50).
    */
   static const float sRGB_y[ 3 ];

   /*!
    * sRGB luminance coefficients (D50).
    */
   static const float sRGB_Y[ 3 ];

   /*!
    * The sRGB (D50) working space.
    */
   static const RGBColorSystem sRGB;

#ifdef __PCL_WITH_STANDARD_RGB_WORKING_SPACES

   /*
    * Adobe RGB 1998
    */
   static const float AdobeRGB1998_x[ 3 ];
   static const float AdobeRGB1998_y[ 3 ];
   static const float AdobeRGB1998_Y[ 3 ];

   /*
    * Apple RGB
    */
   static const float AppleRGB_x[ 3 ];
   static const float AppleRGB_y[ 3 ];
   static const float AppleRGB_Y[ 3 ];

   /*
    * Best RGB
    */
   static const float BestRGB_x[ 3 ];
   static const float BestRGB_y[ 3 ];
   static const float BestRGB_Y[ 3 ];

   /*
    * Beta RGB (by Bruce Lindbloom)
    */
   static const float BetaRGB_x[ 3 ];
   static const float BetaRGB_y[ 3 ];
   static const float BetaRGB_Y[ 3 ];

   /*
    * Bruce RGB
    */
   static const float BruceRGB_x[ 3 ];
   static const float BruceRGB_y[ 3 ];
   static const float BruceRGB_Y[ 3 ];

   /*
    * CIE RGB
    */
   static const float CIERGB_x[ 3 ];
   static const float CIERGB_y[ 3 ];
   static const float CIERGB_Y[ 3 ];

   /*
    * Color Match RGB
    */
   static const float ColorMatchRGB_x[ 3 ];
   static const float ColorMatchRGB_y[ 3 ];
   static const float ColorMatchRGB_Y[ 3 ];

   /*
    * NTSC RGB
    */
   static const float NTSCRGB_x[ 3 ];
   static const float NTSCRGB_y[ 3 ];
   static const float NTSCRGB_Y[ 3 ];

   /*
    * PAL/SECAM RGB
    */
   static const float PALSECAMRGB_x[ 3 ];
   static const float PALSECAMRGB_y[ 3 ];
   static const float PALSECAMRGB_Y[ 3 ];

   /*
    * ProPhoto RGB
    */
   static const float ProPhotoRGB_x[ 3 ];
   static const float ProPhotoRGB_y[ 3 ];
   static const float ProPhotoRGB_Y[ 3 ];

   /*
    * SMPTE-C RGB
    */
   static const float SMPTECRGB_x[ 3 ];
   static const float SMPTECRGB_y[ 3 ];
   static const float SMPTECRGB_Y[ 3 ];

   /*
    * Wide Gamut RGB
    */
   static const float WideGamutRGB_x[ 3 ];
   static const float WideGamutRGB_y[ 3 ];
   static const float WideGamutRGB_Y[ 3 ];

#endif   // __PCL_WITH_STANDARD_RGB_WORKING_SPACES
};

// ----------------------------------------------------------------------------

} // pcl

#undef _1_3
#undef _1_6
#undef _2_3
#undef _16_116
#undef CIEEpsilon
#undef CIEKappa
#undef CIEKappa116

#endif   // __PCL_RGBColorSystem_h

// ----------------------------------------------------------------------------
// EOF pcl/RGBColorSystem.h - Released 2024-06-18T15:48:54Z
