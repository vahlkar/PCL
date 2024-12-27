//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/RGBColorSystem.cpp - Released 2024-12-27T18:16:14Z
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

#include <pcl/RGBColorSystem.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Standard RGB working spaces.
 *
 * All primaries are relative to the D50 reference white. For spaces originally
 * defined relative to other references (usually D65), the Bradford chromatic
 * adaptation algorithm has been used to perform the conversions to D50.
 *
 * Reference: Bruce Lindbloom - http://www.brucelindbloom.com/
 */

/*
 * sRGB (D50)
 */
const float RGBColorSystem::sRGB_x[ 3 ] = { 0.648431F, 0.321152F, 0.155886F };
const float RGBColorSystem::sRGB_y[ 3 ] = { 0.330856F, 0.597871F, 0.066044F };
const float RGBColorSystem::sRGB_Y[ 3 ] = { 0.222491F, 0.716888F, 0.060621F };

/*
 * The default sRGB (D50) working space.
 */
const RGBColorSystem RGBColorSystem::sRGB = RGBColorSystem( 2.2F, true/*sRGB*/, sRGB_x, sRGB_y, sRGB_Y );

#ifdef __PCL_WITH_STANDARD_RGB_WORKING_SPACES

/*
 * Adobe RGB 1998
 */
const float RGBColorSystem::AdobeRGB1998_x[ 3 ] = { 0.648431F, 0.230154F, 0.155886F };
const float RGBColorSystem::AdobeRGB1998_y[ 3 ] = { 0.330856F, 0.701572F, 0.066044F };
const float RGBColorSystem::AdobeRGB1998_Y[ 3 ] = { 0.311114F, 0.625662F, 0.063224F };

/*
 * Apple RGB
 */
const float RGBColorSystem::AppleRGB_x[ 3 ] = { 0.634756F, 0.301775F, 0.162897F };
const float RGBColorSystem::AppleRGB_y[ 3 ] = { 0.340596F, 0.597511F, 0.079001F };
const float RGBColorSystem::AppleRGB_Y[ 3 ] = { 0.255166F, 0.672578F, 0.072256F };

/*
 * Best RGB
 */
const float RGBColorSystem::BestRGB_x[ 3 ] = { 0.734700F, 0.215000F, 0.130000F };
const float RGBColorSystem::BestRGB_y[ 3 ] = { 0.265300F, 0.775000F, 0.035000F };
const float RGBColorSystem::BestRGB_Y[ 3 ] = { 0.228457F, 0.737352F, 0.034191F };

/*
 * Beta RGB (by Bruce Lindbloom)
 */
const float RGBColorSystem::BetaRGB_x[ 3 ] = { 0.688800F, 0.198600F, 0.126500F };
const float RGBColorSystem::BetaRGB_y[ 3 ] = { 0.311200F, 0.755100F, 0.035200F };
const float RGBColorSystem::BetaRGB_Y[ 3 ] = { 0.303273F, 0.663786F, 0.032941F };

/*
 * Bruce RGB
 */
const float RGBColorSystem::BruceRGB_x[ 3 ] = { 0.648431F, 0.300115F, 0.155886F };
const float RGBColorSystem::BruceRGB_y[ 3 ] = { 0.330856F, 0.640960F, 0.066044F };
const float RGBColorSystem::BruceRGB_Y[ 3 ] = { 0.252141F, 0.684495F, 0.063364F };

/*
 * CIE RGB
 */
const float RGBColorSystem::CIERGB_x[ 3 ] = { 0.737385F, 0.266802F, 0.174329F };
const float RGBColorSystem::CIERGB_y[ 3 ] = { 0.264518F, 0.718404F, 0.000599F };
const float RGBColorSystem::CIERGB_Y[ 3 ] = { 0.174658F, 0.824754F, 0.000588F };

/*
 * Color Match RGB
 */
const float RGBColorSystem::ColorMatchRGB_x[ 3 ] = { 0.630000F, 0.295000F, 0.150000F };
const float RGBColorSystem::ColorMatchRGB_y[ 3 ] = { 0.340000F, 0.605000F, 0.075000F };
const float RGBColorSystem::ColorMatchRGB_Y[ 3 ] = { 0.274884F, 0.658132F, 0.066985F };

/*
 * NTSC RGB
 */
const float RGBColorSystem::NTSCRGB_x[ 3 ] = { 0.671910F, 0.222591F, 0.142783F };
const float RGBColorSystem::NTSCRGB_y[ 3 ] = { 0.329340F, 0.710647F, 0.096145F };
const float RGBColorSystem::NTSCRGB_Y[ 3 ] = { 0.310889F, 0.591737F, 0.097374F };

/*
 * PAL/SECAM RGB
 */
const float RGBColorSystem::PALSECAMRGB_x[ 3 ] = { 0.648431F, 0.311424F, 0.155886F };
const float RGBColorSystem::PALSECAMRGB_y[ 3 ] = { 0.330856F, 0.599693F, 0.066044F };
const float RGBColorSystem::PALSECAMRGB_Y[ 3 ] = { 0.232289F, 0.707805F, 0.059906F };

/*
 * ProPhoto RGB
 */
const float RGBColorSystem::ProPhotoRGB_x[ 3 ] = { 0.734700F, 0.159600F, 0.036600F };
const float RGBColorSystem::ProPhotoRGB_y[ 3 ] = { 0.265300F, 0.840400F, 0.000100F };
const float RGBColorSystem::ProPhotoRGB_Y[ 3 ] = { 0.288040F, 0.711874F, 0.000086F };

/*
 * SMPTE-C RGB
 */
const float RGBColorSystem::SMPTECRGB_x[ 3 ] = { 0.638852F, 0.331007F, 0.162897F };
const float RGBColorSystem::SMPTECRGB_y[ 3 ] = { 0.340194F, 0.592082F, 0.079001F };
const float RGBColorSystem::SMPTECRGB_Y[ 3 ] = { 0.221685F, 0.703264F, 0.075052F };

/*
 * Wide Gamut RGB
 */
const float RGBColorSystem::WideGamutRGB_x[ 3 ] = { 0.735000F, 0.115000F, 0.157000F };
const float RGBColorSystem::WideGamutRGB_y[ 3 ] = { 0.265000F, 0.826000F, 0.018000F };
const float RGBColorSystem::WideGamutRGB_Y[ 3 ] = { 0.258187F, 0.724938F, 0.016875F };

#endif   // __PCL_WITH_STANDARD_RGB_WORKING_SPACES

// ----------------------------------------------------------------------------

#define M11    M[0]
#define M12    M[1]
#define M13    M[2]
#define M21    M[3]
#define M22    M[4]
#define M23    M[5]
#define M31    M[6]
#define M32    M[7]
#define M33    M[8]

#define M11_   M_[0]
#define M12_   M_[1]
#define M13_   M_[2]
#define M21_   M_[3]
#define M22_   M_[4]
#define M23_   M_[5]
#define M31_   M_[6]
#define M32_   M_[7]
#define M33_   M_[8]

static Vector InverseMatrix( const Vector& M )
{
   // Determinant
   double d1 = M22*M33 - M23*M32;
   double d2 = M23*M31 - M21*M33;
   double d3 = M21*M32 - M22*M31;
   double d  = M11*d1 + M12*d2 + M13*d3;
   if ( 1 + d == 1 )
      throw Error( "Singular matrix in RGB working color space initialization." );

   Vector M_( 9 );

   M11_ = d1/d;
   M12_ = (M32*M13 - M33*M12)/d;
   M13_ = (M12*M23 - M13*M22)/d;
   M21_ = d2/d;
   M22_ = (M33*M11 - M31*M13)/d;
   M23_ = (M13*M21 - M11*M23)/d;
   M31_ = d3/d;
   M32_ = (M31*M12 - M32*M11)/d;
   M33_ = (M11*M22 - M12*M21)/d;

   return M_;
}

static Vector SetupMatrix( const FVector& x, const FVector& y, const FVector& Y )
{
   if ( x.Length() != 3 || y.Length() != 3 || Y.Length() != 3 )
      throw Error( "Invalid vector lengths in RGB working color space initialization." );

   if ( 1 + x[0] == 1 || 1 + x[1] == 1 || 1 + x[2] == 1 ||
        1 + y[0] == 1 || 1 + y[1] == 1 || 1 + y[2] == 1 )
      throw Error( "Invalid chromaticity coordinates in RGB working color space initialization." );

   if ( 1 + Y.Sum() == 1 )
      throw Error( "Invalid luminance coefficients in RGB working color space initialization." );

   Vector M( 9 );

   M11 = Y[0]*x[0]/y[0];
   M12 = Y[1]*x[1]/y[1];
   M13 = Y[2]*x[2]/y[2];
   M21 = Y[0];
   M22 = Y[1];
   M23 = Y[2];
   M31 = Y[0]*(1 - x[0] - y[0])/y[0];
   M32 = Y[1]*(1 - x[1] - y[1])/y[1];
   M33 = Y[2]*(1 - x[2] - y[2])/y[2];

   return M;
}

static Vector SetupMatrix( const FVector& x, const FVector& y )
{
   if ( x.Length() != 3 || y.Length() != 3 )
      throw Error( "Invalid vector lengths in RGB working color space initialization." );

   if ( 1 + x[0] == 1 || 1 + x[1] == 1 || 1 + x[2] == 1 ||
        1 + y[0] == 1 || 1 + y[1] == 1 || 1 + y[2] == 1 )
      throw Error( "Invalid chromaticity coordinates in RGB working color space initialization." );

   Vector M( 9 );

   M11 = x[0]/y[0];
   M12 = x[1]/y[1];
   M13 = x[2]/y[2];
   M21 = 1;
   M22 = 1;
   M23 = 1;
   M31 = (1 - x[0] - y[0])/y[0];
   M32 = (1 - x[1] - y[1])/y[1];
   M33 = (1 - x[2] - y[2])/y[2];

   Vector M_ = InverseMatrix( M );
   double Sr = CIED501931XR*M11_ + M12_ + CIED501931ZR*M13_;
   double Sg = CIED501931XR*M21_ + M22_ + CIED501931ZR*M23_;
   double Sb = CIED501931XR*M31_ + M32_ + CIED501931ZR*M33_;

   double S = Sr + Sg + Sb;
   if ( 1 + S == 1 )
      throw Error( "Invalid luminance coefficients in RGB working color space initialization." );

   Sr /= S;
   Sg /= S;
   Sb /= S;

   M11 *= Sr;
   M12 *= Sg;
   M13 *= Sb;
   M21  = Sr;
   M22  = Sg;
   M23  = Sb;
   M31 *= Sr;
   M32 *= Sg;
   M33 *= Sb;

   return M;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

RGBColorSystem::Data::Data( float a_gamma, bool a_issRGB,
                            const FVector& a_x, const FVector& a_y, const FVector& a_Y )
   : gamma( a_gamma )
   , issRGB( a_issRGB )
   , isLinear( !issRGB && gamma == 1 )
   , x( a_x )
   , y( a_y )
   , Y( a_Y )
{
   Initialize();
}

RGBColorSystem::Data::Data( float a_gamma, bool a_issRGB,
                            const float* a_x, const float* a_y, const float* a_Y )
   : Data( a_gamma, a_issRGB, FVector( a_x, 3 ), FVector( a_y, 3 ), FVector( a_Y, 3 ) )
{
}

// ----------------------------------------------------------------------------

bool RGBColorSystem::Data::ValidateParameters( const FVector& x, const FVector& y, const FVector& Y )
{
   try
   {
      volatile Vector M_ = InverseMatrix( SetupMatrix( x, y, Y ) );
      return true;
   }
   catch ( ... )
   {
      return false;
   }
}

// ----------------------------------------------------------------------------

void RGBColorSystem::Data::Initialize()
{
   /*
    * RGB -> XYZ transformation matrix.
    */
   if ( Y.IsEmpty() )
   {
      M = SetupMatrix( x, y );
      Y = FVector( M21, M22, M23 );
   }
   else
   {
      // Normalize luminance coefficients.
      double s = Y.Sum();
      if ( 1 + s == 1 )
         throw Error( "Invalid luminance coefficients in RGB working color space initialization." );
      Y /= s;
      M = SetupMatrix( x, y, Y );
   }

   /*
    * XYZ -> RGB inverse transformation matrix.
    */
   M_ = InverseMatrix( M );

   /*
    * Inverse gamma exponent. We require a valid one even for sRGB.
    */
   if ( 1 + gamma == 1 || gamma < 0 )
      throw Error( "Invalid gamma value in RGB working color space initialization." );
   gammaInv = 1/gamma;

   /*
    * CIE X, Z normalization coefficients.
    */
   mX = M11 + M12 + M13;
   mZ = M31 + M32 + M33;

   /*
    * Normalization of CIE a*, b*, c* components.
    */
   double X, Y, Z;
   X = 0;
   Y = 1;
   RGBColorSystem::XYZLab( X );
   RGBColorSystem::XYZLab( Y );
   double a_min = 5*(X - Y);
   X = 1;
   Y = 0;
   RGBColorSystem::XYZLab( X );
   RGBColorSystem::XYZLab( Y );
   double a_max = 5*(X - Y);
   Y = 0;
   Z = 1;
   RGBColorSystem::XYZLab( Y );
   RGBColorSystem::XYZLab( Z );
   double b_min = 2*(Y - Z);
   Y = 1;
   Z = 0;
   RGBColorSystem::XYZLab( Y );
   RGBColorSystem::XYZLab( Z );
   double b_max = 2*(Y - Z);

   zA = -a_min;
   zB = -b_min;
   mA = a_max - a_min;
   mB = b_max - b_min;

   a_max = Max( a_max, Abs( a_min ) );
   b_max = Max( b_max, Abs( b_min ) );
   mC = Sqrt( a_max*a_max + b_max*b_max );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/RGBColorSystem.cpp - Released 2024-12-27T18:16:14Z
