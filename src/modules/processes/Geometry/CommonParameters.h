//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// CommonParameters.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#ifndef __CommonParameters_h
#define __CommonParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/PixelInterpolation.h>

/*
 * 'Auto' interpolation mode:
 *
 * - Uncomment this declaration to use Lanczos interpolation (best quality).
 * - Leave it commented to use bicubic spline interpolation (fastest).
 *
#define __GEOMETRY_QUALITY_INTERPOLATIONS
 */

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS ImageVariant;

// ----------------------------------------------------------------------------

class InterpolationAlgorithm : public MetaEnumeration
{
public:

   enum { NearestNeighbor,
          Bilinear,
          BicubicSpline,
          BicubicBSpline,
          Lanczos3,
          Lanczos4,
          MitchellNetravaliFilter,
          CatmullRomSplineFilter,
          CubicBSplineFilter,
          Auto,
          NumberOfInterpolationAlgorithms,
          Default = Auto };

   InterpolationAlgorithm( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static bool IsClampedInterpolation( int interp )
   {
      return interp == int( BicubicSpline ) ||
             interp == int( Lanczos3 ) ||
             interp == int( Lanczos4 ) ||
             interp == int( Auto );
   }

   static bool IsCubicFilterInterpolation( int interp )
   {
      return interp == int( MitchellNetravaliFilter ) ||
             interp == int( CatmullRomSplineFilter ) ||
             interp == int( CubicBSplineFilter ) ||
             interp == int( Auto );
   }
};

template <typename T>
inline bool UseLanczosLUT( T* )
{
   return false;
}

inline bool UseLanczosLUT( int8* )
{
   return true;
}

inline bool UseLanczosLUT( uint8* )
{
   return true;
}

inline bool UseLanczosLUT( int16* )
{
   return true;
}

inline bool UseLanczosLUT( uint16* )
{
   return true;
}

template <class P>
PixelInterpolation* NewInterpolation( int interpolation, int w, int h, int w0, int h0,
                                      bool rotate, float clampingThreshold, float smoothness, P* )
{
   switch ( interpolation )
   {
   case InterpolationAlgorithm::NearestNeighbor:
      return new NearestNeighborPixelInterpolation();
   case InterpolationAlgorithm::Bilinear:
      return new BilinearPixelInterpolation();
   case InterpolationAlgorithm::BicubicSpline:
      return new BicubicSplinePixelInterpolation( clampingThreshold );
   case InterpolationAlgorithm::BicubicBSpline:
      return new BicubicBSplinePixelInterpolation();
   case InterpolationAlgorithm::Lanczos3:
      if ( UseLanczosLUT( static_cast<typename P::sample*>( 0 ) ) )
         return new Lanczos3LUTPixelInterpolation( clampingThreshold );
      return new LanczosPixelInterpolation( 3, clampingThreshold );
   case InterpolationAlgorithm::Lanczos4:
      if ( UseLanczosLUT( static_cast<typename P::sample*>( 0 ) ) )
         return new Lanczos4LUTPixelInterpolation( clampingThreshold );
      return new LanczosPixelInterpolation( 4, clampingThreshold );

   default:
      {
         double fx = double( w0 )/w;
         double fy = double( h0 )/h;

         if ( interpolation == InterpolationAlgorithm::Auto )
         {
            if ( w < w0 || h < h0 )
            {
               /*
                * Downsampling, with or without rotation:
                *    - Mitchell-Netravali for filter sizes >= 7x7
                *
                * Otherwise:
                *    Quality optimization:
                *       - Lanczos-3
                *    Speed optimization:
                *       - Bicubic spline
                */
               int rx = TruncI( smoothness*fx );
               int ry = TruncI( smoothness*fy );
               if ( rx > 2 || ry > 2 )
                  return new BicubicFilterPixelInterpolation( rx, ry, MitchellNetravaliCubicFilter() );
#ifdef __GEOMETRY_QUALITY_INTERPOLATIONS
               if ( UseLanczosLUT( static_cast<typename P::sample*>( 0 ) ) )
                  return new Lanczos3LUTPixelInterpolation( clampingThreshold );
               return new LanczosPixelInterpolation( 3, clampingThreshold );
#else
               return new BicubicSplinePixelInterpolation( clampingThreshold );
#endif
            }

            /*
             * Upsampling and/or rotation:
             *    Quality optimization:
             *       - Lanczos-3
             *    Speed optimization:
             *       - Bicubic spline
             *
             * No resampling, no rotation:
             *    - Bicubic spline
             */
#ifdef __GEOMETRY_QUALITY_INTERPOLATIONS
            if ( rotate || w > w0 || h > h0 )
            {
               if ( UseLanczosLUT( static_cast<typename P::sample*>( 0 ) ) )
                  return new Lanczos3LUTPixelInterpolation( clampingThreshold );
               return new LanczosPixelInterpolation( 3, clampingThreshold );
            }
#endif
            return new BicubicSplinePixelInterpolation( clampingThreshold );
         }

         int rx = Max( 2, TruncI( smoothness*fx ) );
         int ry = Max( 2, TruncI( smoothness*fy ) );

         switch ( interpolation )
         {
         case InterpolationAlgorithm::MitchellNetravaliFilter:
            return new BicubicFilterPixelInterpolation( rx, ry, MitchellNetravaliCubicFilter() );
         case InterpolationAlgorithm::CatmullRomSplineFilter:
            return new BicubicFilterPixelInterpolation( rx, ry, CatmullRomSplineFilter() );
         case InterpolationAlgorithm::CubicBSplineFilter:
            return new BicubicFilterPixelInterpolation( rx, ry, CubicBSplineFilter() );
         default: // ???
            return new BicubicSplinePixelInterpolation( clampingThreshold );
         }
      }
   }
}

PixelInterpolation* NewInterpolation( int interpolation, int w, int h, int w0, int h0,
                                      bool rotate, float clampingThreshold, float smoothness,
                                      ImageVariant& image );

// ----------------------------------------------------------------------------

class RotationAngle : public MetaDouble
{
public:

   RotationAngle( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

class OptimizeFastRotations : public MetaBoolean
{
public:

   OptimizeFastRotations( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class ClampingThreshold : public MetaFloat
{
public:

   ClampingThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

class Smoothness : public MetaFloat
{
public:

   Smoothness( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

class XResolution : public MetaDouble
{
public:

   XResolution( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

class YResolution : public MetaDouble
{
public:

   YResolution( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

class MetricResolution : public MetaBoolean
{
public:

   MetricResolution( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class ForceResolution : public MetaBoolean
{
public:

   ForceResolution( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class FillRed : public MetaDouble
{
public:

   FillRed( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

class FillGreen : public MetaDouble
{
public:

   FillGreen( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

class FillBlue : public MetaDouble
{
public:

   FillBlue( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

class FillAlpha : public MetaDouble
{
public:

   FillAlpha( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// ----------------------------------------------------------------------------

class NoGUIMessages : public MetaBoolean
{
public:

   NoGUIMessages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CommonParameters_h

// ----------------------------------------------------------------------------
// EOF CommonParameters.h - Released 2021-04-09T19:41:48Z
