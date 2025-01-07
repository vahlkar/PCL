//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/WorldTransformation.cpp - Released 2024-12-28T16:53:56Z
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

#include <pcl/WorldTransformation.h>

#include <pcl/api/APIInterface.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

#ifndef __PCL_NO_WARNING_MUTE_PRAGMAS
#  ifdef _MSC_VER
#    pragma warning( disable: 4804 ) // unsafe use of type 'bool' in operation
#  endif
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

static SplineWorldTransformation::rbf_type RBFTypeFromPropertyValue( const Property& property )
{
   // When feasible, favor the use of DDM RBF implementations.
   String value = property.Value().ToString();
   if ( value == "DDMThinPlateSpline" )
      return RadialBasisFunction::DDMThinPlateSpline;
   if ( value == "DDMVariableOrder" )
      return RadialBasisFunction::DDMVariableOrder;
   if ( value == "ThinPlateSpline" )
      return RadialBasisFunction::ThinPlateSpline;
   if ( value == "VariableOrder" )
      return RadialBasisFunction::VariableOrder;
   if ( value == "DDMMultiquadric" )
      return RadialBasisFunction::DDMMultiquadric;

   throw Error( "Unknown or unsupported radial basis function type '" + value + "'" );
}

static String RBFTypeToPropertyValue( SplineWorldTransformation::rbf_type rbf )
{
   switch ( rbf )
   {
   default: // ?!
   case RadialBasisFunction::DDMThinPlateSpline:
      return "DDMThinPlateSpline";
   case RadialBasisFunction::DDMVariableOrder:
      return "DDMVariableOrder";
   case RadialBasisFunction::ThinPlateSpline:
      return "ThinPlateSpline";
   case RadialBasisFunction::VariableOrder:
      return "VariableOrder";
   case RadialBasisFunction::DDMMultiquadric:
      return "DDMMultiquadric";
   }
}

SplineWorldTransformation::SplineWorldTransformation( const PropertyArray& properties, const LinearTransformation& linearIW, bool regenerate )
{
   try
   {
      /*
       * Compatibility with core <= 1.8.9-2
       */
      for ( const Property& property : properties )
         if ( property.Id() == "PCL:AstrometricSolution:SplineWorldTransformation" // core <= 1.8.9-2
           || property.Id() == "Transformation_ImageToProjection" )                // core < 1.8.9-2
         {
            Deserialize( property.Value().ToByteArray() );
            m_rbf = (m_order > 2) ? RadialBasisFunction::VariableOrder : RadialBasisFunction::ThinPlateSpline;
            m_maxSplinePoints = __PCL_WCS_DEFAULT_SPLINE_MAX_DENSE_POINTS;
            InitializeSplines();
            if ( linearIW.IsSingularMatrix() )
               CalculateLinearApproximation();
            else
               m_linearIW = linearIW;

            return;
         }

      /*
       * Serializations as XISF properties.
       */
      const IsoString prefix = PropertyPrefix();
      const IsoString splinePrefixWI = "PointSurfaceSpline:NativeToImage:";
      const IsoString splinePrefixIW = "PointSurfaceSpline:ImageToNative:";
      const IsoString gridPrefixWI = "PointGridInterpolation:NativeToImage:";
      const IsoString gridPrefixIW = "PointGridInterpolation:ImageToNative:";

      PropertyArray splineWIProperties, splineIWProperties, gridWIProperties, gridIWProperties;
      m_linearIW = linearIW;

      for ( const Property& property : properties )
      {
         IsoString id;
         if ( property.Id().StartsWith( prefix ) )
            id = property.Id().Substring( prefix.Length() );
         else
            continue;

         if ( id == "Version" )
         {
            m_version = property.Value().ToIsoString();
            if ( m_version != __PCL_WCS_SPLINE_VERSION_CURRENT )
               if ( m_version != __PCL_WCS_SPLINE_VERSION_OLD )
                  throw Error( "Unsupported/unknown serialization version '" + m_version + "'" );
         }
         else if ( id == "RBFType" )
         {
            m_rbf = RBFTypeFromPropertyValue( property );
         }
         else if ( id == "SplineOrder" )
         {
            m_order = property.Value().ToInt();
            if ( m_order < 2 )
               throw Error( "Invalid surface spline order parameter" );
         }
         else if ( id == "SplineSmoothness" )
         {
            m_smoothness = property.Value().ToFloat();
            if ( m_smoothness < 0 )
               throw Error( "Invalid surface spline smoothness parameter" );
         }
         else if ( id == "MaxSplinePoints" )
         {
            m_maxSplinePoints = property.Value().ToInt();
            if ( m_maxSplinePoints < 3 )
               throw Error( "Invalid surface spline maximum control points parameter" );
         }
         else if ( id == "UseSimplifiers" )
         {
            m_useSimplifiers = property.Value().ToBoolean();
         }
         else if ( id == "SimplifierRejectFraction" )
         {
            m_simplifierRejectFraction = property.Value().ToFloat();
            if ( m_simplifierRejectFraction < 0 || m_simplifierRejectFraction >= 1 )
               throw Error( "Invalid simplifier reject fraction parameter" );
         }
         else if ( id == "Truncated" )
         {
            m_truncated = property.Value().ToBoolean();
         }
         else if ( id == "ControlPoints:World" )
         {
            Vector coordinates = property.Value().ToVector();
            if ( (coordinates.Length() & 1) == 0 )
               for ( int i = 0; i < coordinates.Length(); i += 2 )
                  m_controlPointsW << DPoint( coordinates[i], coordinates[i+1] );
         }
         else if ( id == "ControlPoints:Image" )
         {
            Vector coordinates = property.Value().ToVector();
            if ( (coordinates.Length() & 1) == 0 )
               for ( int i = 0; i < coordinates.Length(); i += 2 )
                  m_controlPointsI << DPoint( coordinates[i], coordinates[i+1] );
         }
         else if ( id == "Weights" )
         {
            m_weights = property.Value().ToFVector();
         }
         else if ( id == "Projective:NativeToImage" )
         {
            Matrix H = property.Value().ToMatrix();
            if ( !H.IsEmpty() )
            {
               if ( H.Rows() != 3 || H.Cols() != 3 )
                  throw Error( "Invalid Projective:NativeToImage matrix dimensions" );
               m_projectiveWI = Homography( H );
            }
         }
         else if ( id == "Projective:ImageToNative" )
         {
            Matrix H = property.Value().ToMatrix();
            if ( !H.IsEmpty() )
            {
               if ( H.Rows() != 3 || H.Cols() != 3 )
                  throw Error( "Invalid Projective:ImageToNative matrix dimensions" );
               m_projectiveIW = H;
            }
         }
         else if ( id.StartsWith( splinePrefixWI ) )
         {
            splineWIProperties << Property( id.Substring( splinePrefixWI.Length() ), property.Value() );
         }
         else if ( id.StartsWith( splinePrefixIW ) )
         {
            splineIWProperties << Property( id.Substring( splinePrefixIW.Length() ), property.Value() );
         }
         else if ( id.StartsWith( gridPrefixWI ) )
         {
            gridWIProperties << Property( id.Substring( gridPrefixWI.Length() ), property.Value() );
         }
         else if ( id.StartsWith( gridPrefixIW ) )
         {
            gridIWProperties << Property( id.Substring( gridPrefixIW.Length() ), property.Value() );
         }
         else if ( id == "LinearApproximation" )
         {
            // Ignore the LinearApproximation property if a linear
            // transformation has been prescribed.
            if ( m_linearIW.IsSingularMatrix() )
            {
               Matrix H = property.Value().ToMatrix();
               if ( !H.IsEmpty() )
               {
                  if ( H.Rows() != 2 || H.Cols() != 3 )
                     throw Error( "Invalid LinearApproximation matrix dimensions" );
                  m_linearIW = LinearTransformation( H[0][0], H[0][1], H[0][2], H[1][0], H[1][1], H[1][2] );
               }
            }
         }
      } // for each property

      if ( m_controlPointsW.IsEmpty() || m_controlPointsI.IsEmpty() )
         throw Error( "Missing control point vector(s)" );
      if ( m_controlPointsW.Length() < 3 )
         throw Error( "Insufficient control points" );
      if ( m_controlPointsI.Length() != m_controlPointsW.Length() )
         throw Error( "Mismatched control point vector lengths" );
      if ( !m_weights.IsEmpty() && size_type( m_weights.Length() ) != m_controlPointsW.Length() )
         throw Error( "Mismatched weights vector length" );

      EnsureValidRBFParameters();

      if ( !splineWIProperties.IsEmpty() )
      {
         if ( splineIWProperties.IsEmpty() )
            throw Error( "Missing image-to-native surface spline definition properties" );

         m_splineWI = PointSurfaceSplineFromProperties( splineWIProperties, m_version );
         m_splineIW = PointSurfaceSplineFromProperties( splineIWProperties, m_version );

         if ( m_version == __PCL_WCS_SPLINE_VERSION_OLD )
         {
            /*
             * Be compatible with previous versions, where spline generation
             * parameters were not defined as global properties of the WCS
             * transformation, but in the surface spline serializations.
             */
            m_rbf = m_splineWI.m_Sx.m_rbf;
            m_order = m_splineWI.m_Sx.m_order;
            m_smoothness = m_splineWI.m_Sx.m_smoothing;
            m_weights = m_splineWI.m_Sx.m_weights;
         }
         else
         {
            /*
             * For current versions we no longer store redundant metadata.
             */
            m_splineWI.m_Sx.m_rbf = m_splineWI.m_Sy.m_rbf =
            m_splineIW.m_Sx.m_rbf = m_splineIW.m_Sy.m_rbf = m_rbf;

            m_splineWI.m_Sx.m_order = m_splineWI.m_Sy.m_order =
            m_splineIW.m_Sx.m_order = m_splineIW.m_Sy.m_order = m_order;

            m_splineWI.m_Sx.m_smoothing = m_splineWI.m_Sy.m_smoothing =
            m_splineIW.m_Sx.m_smoothing = m_splineIW.m_Sy.m_smoothing = m_smoothness;

            /*
             * Delayed deserialization of core surface splines.
             */
            if ( RadialBasisFunction::HasCoreImplementation( m_rbf ) )
            {
               if ( API != nullptr )
                  if ( !m_splineWI.m_Sx.m_serialization.IsEmpty() && !m_splineWI.m_Sy.m_serialization.IsEmpty() )
                     if ( !m_splineIW.m_Sx.m_serialization.IsEmpty() && !m_splineIW.m_Sy.m_serialization.IsEmpty() )
                     {
                        m_splineWI.m_Sx.m_handle = SurfaceSplineBase::DeserializeHandle( m_splineWI.m_Sx.m_serialization );
                        m_splineWI.m_Sx.m_serialization.Clear();
                        m_splineWI.m_Sy.m_handle = SurfaceSplineBase::DeserializeHandle( m_splineWI.m_Sy.m_serialization );
                        m_splineWI.m_Sy.m_serialization.Clear();

                        m_splineIW.m_Sx.m_handle = SurfaceSplineBase::DeserializeHandle( m_splineIW.m_Sx.m_serialization );
                        m_splineIW.m_Sx.m_serialization.Clear();
                        m_splineIW.m_Sy.m_handle = SurfaceSplineBase::DeserializeHandle( m_splineIW.m_Sy.m_serialization );
                        m_splineIW.m_Sy.m_serialization.Clear();
                     }
            }
            else
            {
               int n = ((m_order*(m_order + 1)) >> 1);
               if (  m_splineWI.m_Sx.m_spline.Length() != m_splineWI.m_Sx.m_x.Length() + n
                  || m_splineWI.m_Sy.m_spline.Length() != m_splineWI.m_Sy.m_x.Length() + n
                  || m_splineIW.m_Sx.m_spline.Length() != m_splineIW.m_Sx.m_x.Length() + n
                  || m_splineIW.m_Sy.m_spline.Length() != m_splineIW.m_Sy.m_x.Length() + n )
               {
                  throw Error( "Invalid length(s) of surface spline coefficient vector(s)" );
               }
            }
         }

         /*
          * Grid interpolations.
          */
         if ( !gridWIProperties.IsEmpty() )
         {
            if ( gridIWProperties.IsEmpty() )
               throw Error( "Missing image-to-native grid interpolation definition properties" );

            if ( !regenerate )
            {
               m_gridWI = PointGridInterpolationFromProperties( gridWIProperties, m_version );
               m_gridIW = PointGridInterpolationFromProperties( gridIWProperties, m_version );
            }
         }
         else
         {
            if ( !gridIWProperties.IsEmpty() )
               throw Error( "Missing native-to-image grid interpolation definition properties" );
         }
      }
      else
      {
         if ( !splineIWProperties.IsEmpty() )
            throw Error( "Missing native-to-image surface spline definition properties" );
      }

      /*
       * Generate surface splines.
       *
       * Do not force a regeneration of surface splines if we already have
       * valid coefficients or have performed a core deserialization.
       */
      if ( regenerate || !m_splineWI.IsValid() || !m_splineIW.IsValid() )
         InitializeSplines();

      /*
       * Calculate the approximate linear transformation if not already
       * available.
       */
      if ( m_linearIW.IsSingularMatrix() )
         CalculateLinearApproximation();
   }
   catch ( const Exception& x )
   {
      throw Error( "SplineWorldTransformation: " + x.Message() );
   }
   catch ( std::bad_alloc& )
   {
      throw Error( "SplineWorldTransformation: " + String( "Out of memory" ) );
   }
   catch ( ... )
   {
      throw;
   }
}

PointSurfaceSpline SplineWorldTransformation::PointSurfaceSplineFromProperties( const PropertyArray& properties, const IsoString& version )
{
   const IsoString splineXPrefix = "SurfaceSplineX:";
   const IsoString splineYPrefix = "SurfaceSplineY:";

   PointSurfaceSpline S;
   PropertyArray splineXProperties, splineYProperties;
   for ( const Property& property : properties )
      if ( property.Id() == "SimplificationErrorX" )
         S.m_epsX = property.Value().ToDouble();
      else if ( property.Id() == "SimplificationErrorY" )
         S.m_epsY = property.Value().ToDouble();
      else if ( property.Id() == "TruncatedX" )
         S.m_truncatedX = property.Value().ToBoolean();
      else if ( property.Id() == "TruncatedY" )
         S.m_truncatedY = property.Value().ToBoolean();
      else if ( property.Id() == "LinearFunction" )
      {
         Matrix H = property.Value().ToMatrix();
         if ( !H.IsEmpty() )
         {
            if ( H.Rows() != 3 || H.Cols() != 3 )
               throw Error( "Invalid linear matrix dimensions in point surface spline definition" );
            S.m_H = Homography( H );
            S.m_incremental = true;
         }
      }
      else if ( property.Id().StartsWith( splineXPrefix ) )
         splineXProperties << Property( property.Id().Substring( splineXPrefix.Length() ), property.Value() );
      else if ( property.Id().StartsWith( splineYPrefix ) )
         splineYProperties << Property( property.Id().Substring( splineYPrefix.Length() ), property.Value() );

   if ( splineXProperties.IsEmpty() || splineYProperties.IsEmpty() )
      throw Error( "Missing surface spline definition properties" );

   S.m_Sx = SurfaceSplineFromProperties( splineXProperties, version );
   S.m_Sy = SurfaceSplineFromProperties( splineYProperties, version );

   return S;
}

SurfaceSpline<double> SplineWorldTransformation::SurfaceSplineFromProperties( const PropertyArray& properties, const IsoString& version )
{
   SurfaceSpline<double> S;
   for ( const Property& property : properties )
      if ( property.Id() == "Order" ) // version 1.3
      {
         S.m_order = property.Value().ToInt();
         if ( S.m_order < 2 )
            throw Error( "Invalid surface spline order parameter" );
      }
      else if ( property.Id() == "Smoothness" ) // version 1.3
      {
         S.m_smoothing = property.Value().ToDouble();
         if ( S.m_smoothing < 0 )
            throw Error( "Invalid surface spline smoothness parameter" );
      }
      else if ( property.Id() == "ScalingFactor" )
      {
         S.m_r0 = property.Value().ToDouble();
         if ( S.m_r0 < 0 || 1 + S.m_r0 == 1 )
            throw Error( "Invalid surface spline scaling factor parameter" );
      }
      else if ( property.Id() == "ZeroOffsetX" )
         S.m_x0 = property.Value().ToDouble();
      else if ( property.Id() == "ZeroOffsetY" )
         S.m_y0 = property.Value().ToDouble();
      else if ( property.Id() == "NodeXCoordinates" )
         S.m_x = property.Value().ToVector();
      else if ( property.Id() == "NodeYCoordinates" )
         S.m_y = property.Value().ToVector();
      else if ( property.Id() == "Coefficients" )
         S.m_spline = property.Value().ToVector();
      else if ( property.Id() == "Serialization" )
         S.m_serialization = property.Value().ToIsoString().Trimmed();
      else if ( property.Id() == "NodeWeights" ) // version 1.3
         S.m_weights = property.Value().ToFVector();

   if ( S.m_x.IsEmpty() || S.m_y.IsEmpty() )
      throw Error( "Missing surface spline node coordinates" );
   if ( S.m_x.Length() < 3 )
      throw Error( "Insufficient surface spline node coordinates" );
   if ( S.m_y.Length() != S.m_x.Length() )
      throw Error( "Mismatched surface spline node vector lengths" );

   if ( version == __PCL_WCS_SPLINE_VERSION_OLD )
   {
      if ( S.m_spline.IsEmpty() )
         throw Error( "Missing surface spline coefficient vector" );
      if ( S.m_spline.Length() != S.m_x.Length() + ((S.m_order*(S.m_order + 1)) >> 1) )
         throw Error( "Invalid surface spline coefficient vector length" );
   }

   if ( !S.m_weights.IsEmpty() )
      if ( S.m_weights.Length() != S.m_x.Length() )
         throw Error( "Mismatched surface spline weights vector length" );

   S.m_havePolynomial = true;

   // Old versions don't serialize the RBF type, but we know it for sure.
   if ( version == __PCL_WCS_SPLINE_VERSION_OLD )
      S.m_rbf = (S.m_order > 2) ? RadialBasisFunction::VariableOrder : RadialBasisFunction::ThinPlateSpline;

   return S;
}

PointGridInterpolation SplineWorldTransformation::PointGridInterpolationFromProperties( const PropertyArray& properties, const IsoString& /*version*/ )
{
   DRect rect = 0;
   double delta = 0;
   DMatrix Gx, Gy;
   for ( const Property& property : properties )
      if ( property.Id() == "Rect" )
      {
         Vector v = property.Value().ToVector();
         if ( v.Length() != 4 )
            throw Error( "Invalid point grid interpolation: Invalid interpolation region coordinates" );
         rect = DRect( v[0], v[1], v[2], v[3] ).Ordered();
      }
      else if ( property.Id() == "Delta" )
         delta = property.Value().ToDouble();
      else if ( property.Id() == "GridX" )
         Gx = property.Value().ToMatrix();
      else if ( property.Id() == "GridY" )
         Gy = property.Value().ToMatrix();

   if ( rect.IsPoint() && rect.x0 == 0 )
      throw Error( "Invalid point grid interpolation: Missing interpolation region parameter" );
   if ( !rect.IsRect() || 1 + rect.Width() == 1 || 1 + rect.Height() == 1 )
      throw Error( "Invalid point grid interpolation: Empty interpolation region" );

   if ( delta == 0 )
      throw Error( "Invalid point grid interpolation: Missing grid distance parameter" );
   if ( delta < 0 || 1 + delta == 1 )
      throw Error( "Invalid point grid interpolation: Invalid grid distance" );

   if ( Gx.IsEmpty() || Gy.IsEmpty() )
      throw Error( "Invalid point grid interpolation: Missing interpolation matrices" );
   int rows = 1 + int( Ceil( rect.Height()/delta ) );
   int cols = 1 + int( Ceil( rect.Width()/delta ) );
   if ( Gx.Rows() != rows || Gx.Cols() != cols || Gy.Rows() != rows || Gy.Cols() != cols )
      throw Error( "Invalid point grid interpolation: Invalid matrix dimensions" );

   PointGridInterpolation G;
   G.m_rect = rect;
   G.m_delta = delta;
   G.m_Gx = Gx;
   G.m_Gy = Gy;
   G.m_Ix.Initialize( G.m_Gx.Begin(), cols, rows );
   G.m_Iy.Initialize( G.m_Gy.Begin(), cols, rows );
   return G;
}

// ----------------------------------------------------------------------------

static Vector PointsToCoordinateVector( const Array<DPoint>& points )
{
   Vector coordinates( int( points.Length() << 1 ) );
   int i = 0;
   for ( const DPoint& point : points )
   {
      coordinates[i++] = point.x;
      coordinates[i++] = point.y;
   }
   return coordinates;
}

PropertyArray SplineWorldTransformation::ToProperties() const
{
   const IsoString prefix = PropertyPrefix();

   PropertyArray properties = PropertyArray()
      << Property( prefix + "Version", __PCL_WCS_SPLINE_VERSION_CURRENT )
      << Property( prefix + "RBFType", RBFTypeToPropertyValue( m_rbf ) )
      << Property( prefix + "SplineOrder", m_order )
      << Property( prefix + "SplineSmoothness", m_smoothness )
      << Property( prefix + "MaxSplinePoints", m_maxSplinePoints )
      << Property( prefix + "UseSimplifiers", m_useSimplifiers )
      << Property( prefix + "SimplifierRejectFraction", m_simplifierRejectFraction )
      << Property( prefix + "Truncated", m_truncated )
      << Property( prefix + "ControlPoints:World", PointsToCoordinateVector( m_controlPointsW ) )
      << Property( prefix + "ControlPoints:Image", PointsToCoordinateVector( m_controlPointsI ) )
      << Property( prefix + "LinearApproximation", m_linearIW.To2x3Matrix() );

   if ( !m_weights.IsEmpty() )
      properties << Property( prefix + "Weights", m_weights );

   if ( m_projectiveWI.IsValid() )
      properties << Property( prefix + "Projective:NativeToImage", Matrix( m_projectiveWI ) );
   if ( m_projectiveIW.IsValid() )
      properties << Property( prefix + "Projective:ImageToNative", Matrix( m_projectiveIW ) );

   if ( m_splineWI.IsValid() && m_splineIW.IsValid() )
   {
      properties << PointSurfaceSplineToProperties( m_splineWI, prefix + "PointSurfaceSpline:NativeToImage:" );
      properties << PointSurfaceSplineToProperties( m_splineIW, prefix + "PointSurfaceSpline:ImageToNative:" );

      if ( m_gridWI.IsValid() && m_gridIW.IsValid() )
      {
         properties << PointGridInterpolationToProperties( m_gridWI, prefix + "PointGridInterpolation:NativeToImage:" );
         properties << PointGridInterpolationToProperties( m_gridIW, prefix + "PointGridInterpolation:ImageToNative:" );
      }
   }

   return properties;
}

PropertyArray SplineWorldTransformation::PointSurfaceSplineToProperties( const PointSurfaceSpline& S, const IsoString& prefix )
{
   PropertyArray properties = PropertyArray()
      << Property( prefix + "SimplificationErrorX", S.m_epsX )
      << Property( prefix + "SimplificationErrorY", S.m_epsY )
      << Property( prefix + "TruncatedX", S.m_truncatedX )
      << Property( prefix + "TruncatedY", S.m_truncatedY );

   if ( S.m_incremental )
      properties << Property( prefix + "LinearFunction", Matrix( S.m_H ) );

   properties << SurfaceSplineToProperties( S.m_Sx, prefix + "SurfaceSplineX:" )
              << SurfaceSplineToProperties( S.m_Sy, prefix + "SurfaceSplineY:" );

   return properties;
}

PropertyArray SplineWorldTransformation::SurfaceSplineToProperties( const SurfaceSpline<double>& S, const IsoString& prefix )
{
   PropertyArray properties = PropertyArray()
      << Property( prefix + "ScalingFactor", S.m_r0 )
      << Property( prefix + "ZeroOffsetX", S.m_x0 )
      << Property( prefix + "ZeroOffsetY", S.m_y0 )
      << Property( prefix + "NodeXCoordinates", S.m_x )
      << Property( prefix + "NodeYCoordinates", S.m_y );

   if ( S.m_handle != 0 )
   {
      IsoString data;
      SurfaceSplineBase::SerializeHandle( data, S.m_handle );
      properties << Property( prefix + "Serialization", data );
   }
   else
      properties << Property( prefix + "Coefficients", S.m_spline );

   return properties;
}

PropertyArray SplineWorldTransformation::PointGridInterpolationToProperties( const PointGridInterpolation& G, const IsoString& prefix )
{
   return PropertyArray()
      << Property( prefix + "Rect", Vector( { G.m_rect.x0, G.m_rect.y0, G.m_rect.x1, G.m_rect.y1 } ) )
      << Property( prefix + "Delta", G.m_delta )
      << Property( prefix + "GridX", G.m_Gx )
      << Property( prefix + "GridY", G.m_Gy );
}

// ----------------------------------------------------------------------------

IsoString SplineWorldTransformation::RBFTypeName() const
{
   IsoString name;
   switch ( m_rbf )
   {
   default: // ?!
   case RadialBasisFunction::ThinPlateSpline:
      name = "Thin plate spline";
      break;
   case RadialBasisFunction::DDMThinPlateSpline:
      name = "DDM thin plate spline";
      break;
   case RadialBasisFunction::DDMMultiquadric:
      name = "DDM multiquadric";
      break;
   case RadialBasisFunction::DDMVariableOrder:
      name = "DDM "; // fall through
   case RadialBasisFunction::VariableOrder:
      switch ( m_order )
      {
      case 2:
         name << "2nd order surface spline";
         break;
      case 3:
         name << "3rd order surface spline";
         break;
      default:
         name.AppendFormat( "%dth order surface spline", m_order );
         break;
      }
      break;
   }
   return name;
}

// ----------------------------------------------------------------------------

/*
 * ### DEPRECATED
 */
void SplineWorldTransformation::Serialize( ByteArray& data ) const
{
   IsoString text;
   text << "VERSION:1.2" << '\n'
        << "TYPE:SurfaceSpline" << '\n'
        << "ORDER:" << IsoString( m_order ) << '\n'
        << "SMOOTHING:" << IsoString().Format( "%.4f", m_smoothness ) << '\n'
        << "SIMPLIFIER:" << (m_useSimplifiers ? '1' : '0') << '\n'
        << "REJECTFRACTION:" << IsoString().Format( "%.2f", m_simplifierRejectFraction ) << '\n'
        << "CONTROLPOINTS:[" << '\n';

   if ( m_weights.IsEmpty() )
      for ( size_type i = 0; i < m_controlPointsW.Length(); ++i )
         text << IsoString().Format( "%.16e;%.16e;%.16e;%.16e",
                                     m_controlPointsI[i].x, m_controlPointsI[i].y,
                                     m_controlPointsW[i].x, m_controlPointsW[i].y ) << '\n';
   else
      for ( size_type i = 0; i < m_controlPointsW.Length(); ++i )
         text << IsoString().Format( "%.16e;%.16e;%.16e;%.16e;%.6e",
                                     m_controlPointsI[i].x, m_controlPointsI[i].y,
                                     m_controlPointsW[i].x, m_controlPointsW[i].y, m_weights[int( i )] ) << '\n';
   text << "]";

   data = text.ToByteArray();
}

// ----------------------------------------------------------------------------

/*
 * ### DEPRECATED
 */
void SplineWorldTransformation::Deserialize( const ByteArray& data )
{
   try
   {
      IsoStringList lines;
      IsoString( reinterpret_cast<const char*>( data.Begin() ),
                 reinterpret_cast<const char*>( data.End() ) ).Break( lines, '\n' );
      if ( lines.IsEmpty() )
         throw Error( "Invalid spline raw serialization." );

      {
         IsoStringList tokens;
         lines[0].Break( tokens, ':' );
         if ( tokens.Length() != 2 || tokens[0] != "VERSION" )
            throw Error( "Invalid spline raw serialization version data." );
         if ( tokens[1] != "1" && tokens[1] != "1.1" && tokens[1] != "1.2" )
            throw Error( "Unsupported spline raw serialization version '" + tokens[1] + "'." );
      }

      Array<float> weights;

      for ( IsoStringList::const_iterator i = lines.Begin(); ++i != lines.End(); )
      {
         IsoStringList tokens;
         i->Break( tokens, ':' );
         if ( tokens.Length() < 2 )
            continue;
         if ( tokens[0] == "ORDER" )
            m_order = Range( tokens[1].ToInt(), 2L, 6L );
         else if ( tokens[0] == "SMOOTHING" )
            m_smoothness = tokens[1].ToFloat();
         else if ( tokens[0] == "SIMPLIFIER" )
            m_useSimplifiers = tokens[1].ToInt() != 0;
         else if ( tokens[0] == "REJECTFRACTION" )
            m_simplifierRejectFraction = tokens[1].ToFloat();
         else if ( tokens[0] == "CONTROLPOINTS" )
         {
            for ( ;; )
            {
               if ( ++i == lines.End() )
                  throw Error( "Parsing spline control points: Unexpected end of data." );
               if ( i->StartsWith( ']' ) )
                  break;

               tokens.Clear();
               i->Break( tokens, ';' );
               if ( tokens.Length() != 4 && tokens.Length() != 5 )
                  throw Error( "Parsing spline control points: Invalid raw control point serialization." );

               double pIx = tokens[0].ToDouble();
               double pIy = tokens[1].ToDouble();
               double pWx = tokens[2].ToDouble();
               double pWy = tokens[3].ToDouble();
               m_controlPointsW << DPoint( pWx, pWy );
               m_controlPointsI << DPoint( pIx, pIy );

               if ( tokens.Length() == 5 )
                  weights << tokens[4].ToFloat();
            }
         }
      }

      if ( !weights.IsEmpty() )
         m_weights = FVector( weights.Begin(), int( weights.Length() ) );
   }
   catch ( const Exception& x )
   {
      throw Error( "SplineWorldTransformation::Deserialize(): " + x.Message() );
   }
   catch ( std::bad_alloc& )
   {
      throw Error( "SplineWorldTransformation::Deserialize(): " + String( "Out of memory" ) );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

void SplineWorldTransformation::EnsureValidRBFParameters()
{
   /*
    * Constrain function order to a reasonable maximum.
    */
   m_order = Range( m_order, 2, 6 );

   /*
    * If running as a standalone unit, use the 'traditional' PCL implementation
    * based on dense linear systems with O(n^3) time complexity.
    */
   if ( RadialBasisFunction::HasCoreImplementation( m_rbf ) )
      if ( API == nullptr )
         m_rbf = ((m_rbf == RadialBasisFunction::DDMVariableOrder)) ?
               RadialBasisFunction::VariableOrder : RadialBasisFunction::ThinPlateSpline;

   /*
    * With DDM surface splines we have a pretty high practical limit on the
    * number of control points, since the spline generation process uses 'only'
    * O(N^2) time. With the traditional PCL implementation based on the
    * solution of dense linear systems spline generation requires O(N^3) time,
    * which imposes a much smaller maximum with current hardware.
    */
   m_maxSplinePoints = Range( m_maxSplinePoints,
                              __PCL_WCS_MIN_SPLINE_MAX_POINTS,
                              RadialBasisFunction::HasDDMImplementation( m_rbf ) ?
                                    __PCL_WCS_MAX_SPLINE_MAX_DDM_POINTS :
                                    __PCL_WCS_MAX_SPLINE_MAX_DENSE_POINTS );
}

// ----------------------------------------------------------------------------

namespace pcl_SWT
{
   class SplineGenerationThread : public Thread
   {
   public:

      SplineGenerationThread( PointSurfaceSpline& S,
                              const Array<DPoint>& P1, const Array<DPoint>& P2, float smoothness, int order,
                              const FVector& W,
                              SplineWorldTransformation::rbf_type rbf )
         : m_S( S )
         , m_P1( P1 )
         , m_P2( P2 )
         , m_smoothness( smoothness )
         , m_order( order )
         , m_W( W )
         , m_rbf( rbf )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         try
         {
            m_S.Initialize( m_P1, m_P2, m_smoothness, m_W, m_order, m_rbf );
            return;
         }
         catch ( const Exception& x )
         {
            m_exception = x;
         }
         catch ( ... )
         {
            m_exception = Error( "Unknown exception" );
         }
         m_S.Clear();
      }

      void ValidateOrThrow() const
      {
         if ( !m_S.IsValid() )
            throw m_exception;
      }

   private:

      PointSurfaceSpline&                 m_S;
      const Array<DPoint>&                m_P1;
      const Array<DPoint>&                m_P2;
      float                               m_smoothness;
      int                                 m_order;
      FVector                             m_W; // ### N.B. do not use a reference: the W ctor. argument can be a temporary object.
      SplineWorldTransformation::rbf_type m_rbf;
      Exception                           m_exception;
   };
}

void SplineWorldTransformation::InitializeSplines()
{
   try
   {
      int n = int( m_controlPointsW.Length() );
      if ( n < 5 )
         throw Error( "At least five control points are required." );
      if ( n != int( m_controlPointsI.Length() ) )
         throw Error( "The specified control point arrays have different lengths." );
      if ( m_smoothness > 0 )
         if ( !m_weights.IsEmpty() )
            if ( m_weights.Length() != n )
               throw Error( "Invalid length of point weights vector." );

      m_projectiveWI = Homography( m_controlPointsW, m_controlPointsI );
      m_splineWI.SetLinearFunction( m_projectiveWI );
      m_splineWI.EnableIncrementalFunction();

      m_projectiveIW = Homography( m_controlPointsI, m_controlPointsW );
      m_splineIW.SetLinearFunction( m_projectiveIW );
      m_splineIW.EnableIncrementalFunction();

      /*
       * If running as a standalone executable, we can only use our standard
       * surface splines (with O(n^3) time complexity).
       */
      if ( API == nullptr )
         m_rbf = (m_order > 2) ? RadialBasisFunction::VariableOrder : RadialBasisFunction::ThinPlateSpline;

      /*
       * Ensure that the maximum number of control points is within the
       * acceptable limits.
       */
      m_maxSplinePoints = Range( m_maxSplinePoints,
                                 __PCL_WCS_MIN_SPLINE_MAX_POINTS,
                                 RadialBasisFunction::HasDDMImplementation( m_rbf ) ?
                                       __PCL_WCS_MAX_SPLINE_MAX_DDM_POINTS :
                                       __PCL_WCS_MAX_SPLINE_MAX_DENSE_POINTS );

      if ( m_useSimplifiers )
      {
         /*
          * With surface simplification enabled, build approximating surface
          * splines from simplified control point lists. The point weights
          * array will be ignored. Spline smoothness should be low in this
          * case, just to filter out residual noise, since surface
          * simplification applies robust outlier rejection techniques.
          */
         m_splineWI.EnableSimplifiers();
         m_splineWI.SetSimplifierRejectFraction( m_simplifierRejectFraction );
         m_splineWI.SetMaxSplinePoints( m_maxSplinePoints );

         m_splineIW.EnableSimplifiers();
         m_splineIW.SetSimplifierRejectFraction( m_simplifierRejectFraction );
         m_splineIW.SetMaxSplinePoints( m_maxSplinePoints );

         pcl_SWT::SplineGenerationThread TWI( m_splineWI, m_controlPointsW, m_controlPointsI, m_smoothness, m_order, FVector(), m_rbf );
         pcl_SWT::SplineGenerationThread TIW( m_splineIW, m_controlPointsI, m_controlPointsW, m_smoothness, m_order, FVector(), m_rbf );
         TWI.Start( ThreadPriority::DefaultMax );
         TIW.Start( ThreadPriority::DefaultMax );
         TWI.Wait();
         TIW.Wait();
         TWI.ValidateOrThrow();
         TIW.ValidateOrThrow();

         m_truncated = m_splineWI.Truncated() || m_splineIW.Truncated();
      }
      else
      {
         /*
          * When no surface simplification is used, build surface splines with
          * the specified metadata: The (possibly truncated) sets of control
          * points and weights, spline smoothness, order, and RBF.
          */
         if ( n > m_maxSplinePoints )
         {
            m_truncated = true;

            Array<DPoint> PW( m_controlPointsW.Begin(), m_controlPointsW.At( m_maxSplinePoints ) );
            Array<DPoint> PI( m_controlPointsI.Begin(), m_controlPointsI.At( m_maxSplinePoints ) );
            FVector W = m_weights.IsEmpty() ? FVector() : FVector( m_weights.Begin(), m_maxSplinePoints );

            pcl_SWT::SplineGenerationThread TWI( m_splineWI, PW, PI, m_smoothness, m_order, W, m_rbf );
            pcl_SWT::SplineGenerationThread TIW( m_splineIW, PI, PW, m_smoothness, m_order, W, m_rbf );
            TWI.Start( ThreadPriority::DefaultMax );
            TIW.Start( ThreadPriority::DefaultMax );
            TWI.Wait();
            TIW.Wait();
            TWI.ValidateOrThrow();
            TIW.ValidateOrThrow();
         }
         else
         {
            pcl_SWT::SplineGenerationThread TWI( m_splineWI, m_controlPointsW, m_controlPointsI, m_smoothness, m_order, m_weights, m_rbf );
            pcl_SWT::SplineGenerationThread TIW( m_splineIW, m_controlPointsI, m_controlPointsW, m_smoothness, m_order, m_weights, m_rbf );
            TWI.Start( ThreadPriority::DefaultMax );
            TIW.Start( ThreadPriority::DefaultMax );
            TWI.Wait();
            TIW.Wait();
            TWI.ValidateOrThrow();
            TIW.ValidateOrThrow();
         }
      }

      if ( !m_splineWI.IsValid() || !m_splineIW.IsValid() )
         throw Error( "Invalid surface spline initialization (unknown reason)." );
   }
   catch ( const Exception& x )
   {
      throw Error( "SplineWorldTransformation::InitializeSplines(): " + x.Message() );
   }
   catch ( std::bad_alloc& )
   {
      throw Error( "SplineWorldTransformation::InitializeSplines(): " + String( "Out of memory" ) );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

void SplineWorldTransformation::CalculateLinearApproximation()
{
   /*
    * Use independent multiple linear regression for x and y. The model is:
    *
    *   Y = X * B + error
    *
    * - The regressand Y contains the x (or y) of the predicted coordinates
    *   (m_controlPointsW).
    *
    * - The regressor X contains the vectors (x, y, 1) with the source
    *   coordinates (m_controlPointsI).
    *
    * - The parameters vector B contains the factors of the expression:
    *
    *   xc = xi*B0 + yi*B1 + B2
    */
   int N = int( m_controlPointsI.Length() );
   DMatrix Y1( N, 1 );
   DMatrix Y2( N, 1 );
   DMatrix X( N, 3 );
   for ( int i = 0; i < N; ++i )
   {
      Y1[i][0] = m_controlPointsW[i].x;
      Y2[i][0] = m_controlPointsW[i].y;
      X[i][0]  = m_controlPointsI[i].x;
      X[i][1]  = m_controlPointsI[i].y;
      X[i][2]  = 1;
   }

   DMatrix XT = X.Transpose();
   DMatrix XT_X_inv_XT = (XT * X).Inverse() * XT;
   DMatrix B1 = XT_X_inv_XT * Y1;
   DMatrix B2 = XT_X_inv_XT * Y2;
   m_linearIW = LinearTransformation( B1[0][0], B1[1][0], B1[2][0],
                                      B2[0][0], B2[1][0], B2[2][0] );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/WorldTransformation.cpp - Released 2024-12-28T16:53:56Z
