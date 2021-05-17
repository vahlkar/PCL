//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Generators.cpp - Released 2021-05-05T15:38:07Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
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

#include "Data.h"
#include "Function.h"
#include "ImageCache.h"
#include "PixelMathInstance.h"

#include <pcl/Convolution.h>
#include <pcl/Cryptography.h>
#include <pcl/FastRotation.h>
#include <pcl/FFTConvolution.h>
#include <pcl/GaussianFilter.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/Rotation.h>
#include <pcl/Selection.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/Translation.h>

#define S   (static_cast<Sample*>( *i ))
#define P   (static_cast<PixelData*>( *i ))
#define I   (static_cast<ImageReference*>( *i ))

namespace pcl
{

// ----------------------------------------------------------------------------

static ImageVariant NewGeneratorResult( const ImageReference* ref, int bitsPerSample = 32 )
{
   const ImageVariant* image = ref->Image();
   image->PushSelections();
   if ( ref->IsChannelIndex() )
   {
      if ( ref->ChannelIndex() >= image->NumberOfChannels() )
         throw ParseError( ("Channel index out of range: " + ref->Id()).AppendFormat( "[%d]", ref->ChannelIndex() ) );
      image->SelectChannel( ref->ChannelIndex() );
   }
   ImageVariant result;
   result.CreateFloatImage( bitsPerSample );
   result.CopyImage( *image );
   image->PopSelections();
   return result;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool GaussianConvolutionFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // gconv( image[, sigma=2[, rho=1[, theta=0[, eps=0.01]]]] )

   distance_type n = Distance( i, j );
   if ( n < 1 || n > 5 )
   {
      info = "gconv() takes from 1 to 5 arguments";
      return false;
   }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "gconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   if ( ++i < j )
   {
      if ( (*i)->IsImageReference() )
      {
         info = "gconv() argument #2: The filter standard deviation must be an invariant scalar subexpression";
         arg = *i;
         return false;
      }

      if ( (*i)->IsSample() )
         if ( S->Value() < 0 || S->Value() + 1 == 1 )
         {
            info = "gconv() argument #2: The filter standard deviation must be > 0";
            arg = *i;
            return false;
         }

      if ( ++i < j )
      {
         if ( (*i)->IsImageReference() )
         {
            info = "gconv() argument #3: The filter aspect ratio must be an invariant scalar subexpression";
            arg = *i;
            return false;
         }

         if ( (*i)->IsSample() )
            if ( S->Value() < 0 || S->Value() + 1 == 1 || S->Value() > 1 )
            {
               info = "gconv() argument #3: The filter aspect ratio must be in the (0,1] range";
               arg = *i;
               return false;
            }

         if ( ++i < j )
         {
            if ( (*i)->IsImageReference() )
            {
               info = "gconv() argument #4: The filter rotation angle must be an invariant scalar subexpression";
               arg = *i;
               return false;
            }

            if ( (*i)->IsSample() )
               if ( S->Value() < -180 || S->Value() > 180 )
               {
                  info = "gconv() argument #4: The filter rotation angle must be in the range [-180,+180] degrees";
                  arg = *i;
                  return false;
               }

            if ( ++i < j )
            {
               if ( (*i)->IsImageReference() )
               {
                  info = "gconv() argument #5: The filter truncation error must be an invariant scalar subexpression";
                  arg = *i;
                  return false;
               }

               if ( (*i)->IsSample() )
                  if ( S->Value() < 0 || S->Value() + 1 == 1 )
                  {
                     info = "gconv() argument #5: The filter truncation error must be > 0";
                     arg = *i;
                     return false;
                  }
            }
         }
      }
   }

   return true;
}

IsoString GaussianConvolutionFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // gconv( image[, sigma=2[, rho=1[, theta=0[, eps=0.01]]]] )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "gconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );
   float sigma = 2;
   float rho = 1;
   float theta = 0;
   float eps = 0.01;

   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         sigma = S->Value();
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         sigma = P->PixelValue()[0];
      else
         throw ParseError( "gconv() argument #2: The filter standard deviation must be an invariant scalar subexpression" );
      if ( sigma < 0 || sigma + 1 == 1 )
         throw ParseError( "gconv() argument #2: The filter standard deviation must be > 0; got " + String( sigma ) );

      if ( ++i < j )
      {
         if ( (*i)->IsSample() )
            rho = S->Value();
         else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
            rho = P->PixelValue()[0];
         else
            throw ParseError( "gconv() argument #3: The filter aspect ratio must be an invariant scalar subexpression" );
         if ( rho < 0 || rho + 1 == 1 || rho > 1 )
            throw ParseError( "gconv() argument #3: The filter aspect ratio must be in the (0,1] range; got " + String( rho ) );

         if ( ++i < j )
         {
            if ( (*i)->IsSample() )
               theta = S->Value();
            else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
               theta = P->PixelValue()[0];
            else
               throw ParseError( "gconv() argument #4: The filter rotation angle must be an invariant scalar subexpression" );
            if ( theta < -180 || theta > 180 )
               throw ParseError( "gconv() argument #4: The filter rotation angle must be in the range [-180,+180] degrees; got " + String( theta ) );

            if ( ++i < j )
            {
               if ( (*i)->IsSample() )
                  eps = S->Value();
               else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
                  eps = P->PixelValue()[0];
               else
                  throw ParseError( "gconv() argument #5: The filter truncation error must be an invariant scalar subexpression" );
               if ( eps < 0 || eps + 1 == 1 )
                  throw ParseError( "gconv() argument #5: The filter truncation error must be > 0; got " + String( eps ) );
            }
         }
      }
   }

   IsoString key = TheImageCache->MakeKey( ref->Id(), IsoString().Format( "_gconv_%.4f_%.4f_%.4f_%.4f", sigma, rho, theta, eps ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      GaussianFilter H( sigma, eps, rho, Rad( theta ) );
      int nofThreads = Thread::NumberOfThreads( PCL_MAX_PROCESSORS );
      if (  H.Size() >= FFTConvolution::FasterThanSeparableFilterSize( nofThreads )
         || !H.IsSeparable() && H.Size() >= FFTConvolution::FasterThanNonseparableFilterSize( nofThreads )
         || H.Size() > result->Width()
         || H.Size() > result->Height() )
      {
         FFTConvolution( H ) >> result;
      }
      else if ( H.IsSeparable() && H.Size() >= SeparableConvolution::FasterThanNonseparableFilterSize( nofThreads ) )
      {
         SeparableConvolution( H.AsSeparableFilter() ) >> result;
      }
      else
      {
         Convolution( H ) >> result;
      }

      TheImageCache->AddImage( key, result );
   }

   return key;
}

void GaussianConvolutionFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "gconv(): Internal execution error" );
}

// ----------------------------------------------------------------------------

bool BoxConvolutionFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // bconv( image[, n=3] )

   distance_type n = Distance( i, j );
   if ( n != 1 )
      if ( n != 2 )
      {
         info = "bconv() can only take 1 or 2 arguments";
         return false;
      }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "bconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   if ( ++i < j )
   {
      if ( (*i)->IsImageReference() )
      {
         info = "bconv() argument #2: The filter size must be an invariant scalar subexpression";
         arg = *i;
         return false;
      }

      if ( (*i)->IsSample() )
      {
         if ( Frac( S->Value() ) != 0 )
         {
            info = "bconv() argument #2: The filter size must be an integer value";
            arg = *i;
            return false;
         }
         if ( S->Value() < 3 )
         {
            info = "bconv() argument #2: The filter size must be >= 3";
            arg = *i;
            return false;
         }
         if ( (int( S->Value() ) & 1) == 0 )
         {
            info = "bconv() argument #2: The filter size must be an odd integer";
            arg = *i;
            return false;
         }
      }
   }

   return true;
}

IsoString BoxConvolutionFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // bconv( image[, n=3] )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "bconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );
   int n = 3;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         n = int( S->Value() );
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         n = int( P->PixelValue()[0] );
      else
         throw ParseError( "bconv() argument #2: The filter size must be an invariant scalar subexpression" );

      if ( n < 3 )
         throw ParseError( "bconv() argument #2: The filter size must be >= 3" );
      if ( (n & 1) == 0 )
         throw ParseError( "bconv() argument #2: The filter size must be an odd integer" );
   }

   IsoString key = TheImageCache->MakeKey( ref->Id(), IsoString().Format( "_bconv_%d", n ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      int nofThreads = Thread::NumberOfThreads( PCL_MAX_PROCESSORS );
      if (  n >= FFTConvolution::FasterThanSeparableFilterSize( nofThreads )
         || n > result->Width()
         || n > result->Height() )
      {
         KernelFilter H( n, 1.0 );
         FFTConvolution( H ) >> result;
      }
      else if ( n >= SeparableConvolution::FasterThanNonseparableFilterSize( nofThreads ) )
      {
         SeparableFilter H( n, 1.0 );
         SeparableConvolution( H ) >> result;
      }
      else
      {
         KernelFilter H( n, 1.0 );
         Convolution( H ) >> result;
      }

      TheImageCache->AddImage( key, result );
   }

   return key;
}

void BoxConvolutionFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "bconv(): Internal execution error" );
}

// ----------------------------------------------------------------------------

bool KernelConvolutionFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // kconv( image, k11, k12, k13, k21, k22, k23, k31, k32, k33[, ...] )

   distance_type n = Distance( i, j );
   if ( n < 10 )
   {
      info = "kconv() takes at least 10 arguments";
      return false;
   }

   --n;

   if ( (n & 1) == 0 )
   {
      info = "kconv() requires an odd number of kernel filter element arguments";
      arg = *(i+n);
      return false;
   }

   int sz = int( Sqrt( n ) );
   if ( sz*sz != n )
   {
      info = "kconv() requires a square kernel filter. The number of specified elements (" + String( n ) + ") does not have an integer square root";
      arg = *(i+n);
      return false;
   }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "kconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   for ( int k = 2; ++i < j; ++k )
      if ( (*i)->IsImageReference() )
      {
         info = "kconv() argument #" + String( k ) + ": Kernel filter elements must be invariant scalar subexpressions";
         arg = *i;
         return false;
      }

   return true;
}

IsoString KernelConvolutionFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // kconv( image, k11, k12, k13, k21, k22, k23, k31, k32, k33[, ...] )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "kconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );
   int n = int( Sqrt( Distance( i, j ) - 1 ) );
   DVector K( n*n );
   for ( int k = 0; ++i < j; ++k )
   {
      if ( (*i)->IsSample() )
         K[k] = S->Value();
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         K[k] = P->PixelValue()[0];
      else
         throw ParseError( "kconv() argument #" + String( 2+k ) + ": Kernel filter elements must be invariant scalar subexpressions" );
   }

   IsoString key = TheImageCache->MakeKey( ref->Id(), "_kconv_" + IsoString::ToHex( MD5().Hash( K ) ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      KernelFilter H( K.Begin(), n );
      int nofThreads = Thread::NumberOfThreads( PCL_MAX_PROCESSORS );

      if (  H.Size() < FFTConvolution::FasterThanNonseparableFilterSize( nofThreads )
         || H.IsHighPassFilter() )
      {
         Convolution C( H );
         C.EnableHighPassRescaling( false/*rescaleHighPass*/ );
         C >> result;
      }
      else
      {
         FFTConvolution( H ) >> result;
      }

      TheImageCache->AddImage( key, result );
   }

   return key;
}

void KernelConvolutionFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "kconv(): Internal execution error" );
}

// ----------------------------------------------------------------------------

enum
{
   StrSquare, StrCircular, StrOrthogonal, StrDiagonal, StrStar, StrThreeWay, StrCount
};

static bool ValidateMorphologicalTransformationGeneratorParameters( const String& functionName, String& info, Expression*& arg,
                                       Expression::component_list::const_iterator i, Expression::component_list::const_iterator j )
{
   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = functionName + "() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   if ( ++i < j )
   {
      if ( (*i)->IsImageReference() )
      {
         info = functionName + "() argument #2: The filter size must be an invariant scalar subexpression";
         arg = *i;
         return false;
      }

      if ( (*i)->IsSample() )
      {
         if ( Frac( S->Value() ) != 0 )
         {
            info = functionName + "() argument #2: The filter size must be an integer value";
            arg = *i;
            return false;
         }
         if ( S->Value() < 3 )
         {
            info = functionName + "() argument #2: The filter size must be >= 3";
            arg = *i;
            return false;
         }
         if ( (int( S->Value() ) & 1) == 0 )
         {
            info = functionName + "() argument #2: The filter size must be an odd integer";
            arg = *i;
            return false;
         }
      }

      if ( ++i < j )
      {
         if ( (*i)->IsImageReference() )
         {
            info = functionName + "() argument #3: The structuring element selector must be an invariant scalar subexpression";
            arg = *i;
            return false;
         }

         if ( (*i)->IsSample() )
         {
            if ( Frac( S->Value() ) != 0 )
            {
               info = functionName + "() argument #3: The structuring element selector must be an integer value";
               arg = *i;
               return false;
            }
            if ( S->Value() < 0 || S->Value() >= StrCount )
            {
               info = functionName + "() argument #3: Unknown or unsupported structuring element selector '" + String( int( S->Value() ) ) + "'";
               arg = *i;
               return false;
            }
         }
      }
   }

   return true;
}

static void GetMorphologicalTransformationGeneratorParameters( const String& functionName, const ImageReference*& ref, int& n, int& s,
                                       Expression::component_list::const_iterator i, Expression::component_list::const_iterator j )
{
   if ( !(*i)->IsImageReference() )
      throw ParseError( functionName + "() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   ref = dynamic_cast<ImageReference*>( *i );
   n = 3;
   s = StrSquare;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         n = int( S->Value() );
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         n = int( P->PixelValue()[0] );
      else
         throw ParseError( functionName + "() argument #2: The filter size must be an invariant scalar subexpression" );

      if ( n < 3 )
         throw ParseError( functionName + "() argument #2: The filter size must be >= 3" );
      if ( (n & 1) == 0 )
         throw ParseError( functionName + "() argument #2: The filter size must be an odd integer" );

      if ( ++i < j )
      {
         if ( (*i)->IsSample() )
            s = int( S->Value() );
         else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
            s = int( P->PixelValue()[0] );
         else
            throw ParseError( functionName + "() argument #3: The structuring element selector must be an invariant scalar subexpression" );

         if ( s < 0 || s >= StrCount )
            throw ParseError( functionName + "() argument #3: Unknown or unsupported structuring element selector '" + String( s ) + '\'' );
      }
   }
}

static StructuringElement* NewStructuringElement( int n, int s )
{
   switch ( s )
   {
   default: // ?!
   case StrSquare:     return new BoxStructure( n );
   case StrCircular:   return new CircularStructure( n );
   case StrOrthogonal: return new OrthogonalStructure( n );
   case StrDiagonal:   return new DiagonalStructure( n );
   case StrStar:       return new StarStructure( n );
   case StrThreeWay:   return new ThreeWayStructure( n );
   }
}

// ----------------------------------------------------------------------------

bool MedianFilterFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // medfilt( image[, n=3[, str=str_square()]] )

   distance_type n = Distance( i, j );
   if ( n < 1 || n > 3 )
   {
      info = "medfilt() takes from 1 to 3 arguments";
      return false;
   }

   return ValidateMorphologicalTransformationGeneratorParameters( "medfilt", info, arg, i, j );
}

IsoString MedianFilterFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // medfilt( image[, n=3[, str=str_square()]] )

   const ImageReference* ref;
   int n, s;
   GetMorphologicalTransformationGeneratorParameters( "medfilt", ref, n, s, i, j );

   IsoString key = TheImageCache->MakeKey( ref->Id(), IsoString().Format( "_medfilt_%d_%d", n, s ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      AutoPointer<StructuringElement> str = NewStructuringElement( n, s );
      MorphologicalTransformation( MedianFilter(), *str ) >> result;

      TheImageCache->AddImage( key, result );
   }

   return key;
}

void MedianFilterFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "medfilt(): Internal execution error" );
}

// ----------------------------------------------------------------------------

bool ErosionFilterFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // erosion( image[, n=3[, str=str_square()]] )

   distance_type n = Distance( i, j );
   if ( n < 1 || n > 3 )
   {
      info = "erosion() takes from 1 to 3 arguments";
      return false;
   }

   return ValidateMorphologicalTransformationGeneratorParameters( "erosion", info, arg, i, j );
}

IsoString ErosionFilterFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // erosion( image[, n=3[, str=str_square()]] )

   const ImageReference* ref;
   int n, s;
   GetMorphologicalTransformationGeneratorParameters( "erosion", ref, n, s, i, j );

   IsoString key = TheImageCache->MakeKey( ref->Id(), IsoString().Format( "_erosion_%d_%d", n, s ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      AutoPointer<StructuringElement> str = NewStructuringElement( n, s );
      MorphologicalTransformation( ErosionFilter(), *str ) >> result;

      TheImageCache->AddImage( key, result );
   }

   return key;
}

void ErosionFilterFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "erosion(): Internal execution error" );
}

// ----------------------------------------------------------------------------

bool DilationFilterFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // dilation( image[, n=3[, str=str_square()]] )

   distance_type n = Distance( i, j );
   if ( n < 1 || n > 3 )
   {
      info = "dilation() takes from 1 to 3 arguments";
      return false;
   }

   return ValidateMorphologicalTransformationGeneratorParameters( "dilation", info, arg, i, j );
}

IsoString DilationFilterFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // dilation( image[, n=3[, str=str_square()]] )

   const ImageReference* ref;
   int n, s;
   GetMorphologicalTransformationGeneratorParameters( "dilation", ref, n, s, i, j );

   IsoString key = TheImageCache->MakeKey( ref->Id(), IsoString().Format( "_dilation_%d_%d", n, s ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      AutoPointer<StructuringElement> str = NewStructuringElement( n, s );
      MorphologicalTransformation( DilationFilter(), *str ) >> result;

      TheImageCache->AddImage( key, result );
   }

   return key;
}

void DilationFilterFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "dilation(): Internal execution error" );
}

// ----------------------------------------------------------------------------

void StrSquareFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "str_square(): Internal parser error" );
}

bool StrSquareFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void StrSquareFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( StrSquare );
}

// ----------------------------------------------------------------------------

void StrCircularFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "str_circular(): Internal parser error" );
}

bool StrCircularFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void StrCircularFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( StrCircular );
}

// ----------------------------------------------------------------------------

void StrOrthogonalFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "str_orthogonal(): Internal parser error" );
}

bool StrOrthogonalFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void StrOrthogonalFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( StrOrthogonal );
}

// ----------------------------------------------------------------------------

void StrDiagonalFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "str_diagonal(): Internal parser error" );
}

bool StrDiagonalFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void StrDiagonalFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( StrDiagonal );
}

// ----------------------------------------------------------------------------

void StrStarFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "str_star(): Internal parser error" );
}

bool StrStarFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void StrStarFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( StrStar );
}

// ----------------------------------------------------------------------------

void StrThreeWayFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "str_threeway(): Internal parser error" );
}

bool StrThreeWayFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void StrThreeWayFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( StrThreeWay );
}

// ----------------------------------------------------------------------------

bool TranslationFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // translate( image, dx, dy )

   if ( Distance( i, j ) != 3 )
   {
      info = "translate() takes 3 arguments";
      return false;
   }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "translate() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   for ( int k = 2; ++i < j; ++k )
      if ( (*i)->IsImageReference() )
      {
         info = "translate() argument #" + String( k ) + ": translation increments must be invariant scalar subexpressions";
         arg = *i;
         return false;
      }

   return true;
}

IsoString TranslationFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // translate( image, dx, dy )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "translate() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );

   double dx;
   if ( (*++i)->IsSample() )
      dx = S->Value();
   else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
      dx = P->PixelValue()[0];
   else
      throw ParseError( "translate() argument #2: The X-axis increment must be an invariant scalar subexpression" );

   double dy;
   if ( (*++i)->IsSample() )
      dy = S->Value();
   else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
      dy = P->PixelValue()[0];
   else
      throw ParseError( "translate() argument #3: The Y-axis increment must be an invariant scalar subexpression" );

   IsoString key = TheImageCache->MakeKey( ref->Id(), IsoString().Format( "_translate_%.4f_%.4f", dx, dy ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      BicubicPixelInterpolation interpolation;
      Translation( interpolation, dx, dy ) >> result;

      TheImageCache->AddImage( key, result );
   }

   return key;
}

void TranslationFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "translate(): Internal execution error" );
}

// ----------------------------------------------------------------------------

bool RotationFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // rotate( image, angle[, cx, cy] )

   distance_type n = Distance( i, j );
   if ( n != 2 )
      if ( n != 4 )
      {
         info = "rotate() can only take 2 or 4 arguments";
         return false;
      }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "rotate() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   for ( int k = 2; ++i < j; ++k )
      if ( (*i)->IsImageReference() )
      {
         info = "rotate() argument #" + String( k ) + ": rotation angles and center coordinates must be invariant scalar subexpressions";
         arg = *i;
         return false;
      }

   return true;
}

IsoString RotationFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // rotate( image, angle[, cx, cy] )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "rotate() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );

   double angle;
   if ( (*++i)->IsSample() )
      angle = S->Value();
   else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
      angle = P->PixelValue()[0];
   else
      throw ParseError( "rotate() argument #2: The rotation angle must be an invariant scalar subexpression" );

   double cx = 0, cy = 0;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         cx = S->Value();
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         cx = P->PixelValue()[0];
      else
         throw ParseError( "rotate() argument #3: The center x coordinate must be an invariant scalar subexpression" );

      if ( ++i < j )
      {
         if ( (*i)->IsSample() )
            cy = S->Value();
         else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
            cy = P->PixelValue()[0];
         else
            throw ParseError( "rotate() argument #4: The center y coordinate must be an invariant scalar subexpression" );
      }
   }

   IsoString key = TheImageCache->MakeKey( ref->Id(), IsoString().Format( "_rotate_%.6f_%.4f_%.4f", angle, cx, cy ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      BicubicPixelInterpolation interpolation;
      Rotation( interpolation, Rad( angle ), cx, cy ) >> result;
      int dx0 = (result->Width() - ref->Image()->Width()) >> 1;
      int dy0 = (result->Height() - ref->Image()->Height()) >> 1;
      int dx1 = dx0 + (((result->Width() - ref->Image()->Width()) & 1) ? 1 : 0);
      int dy1 = dy0 + (((result->Height() - ref->Image()->Height()) & 1) ? 1 : 0);
      result.CropBy( -dx0, -dy0, -dx1, -dy1 );

      TheImageCache->AddImage( key, result );
   }

   return key;
}

void RotationFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "rotate(): Internal execution error" );
}

// ----------------------------------------------------------------------------

bool MirrorHorzFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // hmirror( image )

   if ( Distance( i, j ) != 1 )
   {
      info = "hmirror() takes a single argument";
      return false;
   }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "hmirror() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   return true;
}

IsoString MirrorHorzFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // hmirror( image )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "hmirror() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );

   IsoString key = TheImageCache->MakeKey( ref->Id(), "_hmirror" );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );
      HorizontalMirror() >> result;
      TheImageCache->AddImage( key, result );
   }

   return key;
}

void MirrorHorzFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "hmirror(): Internal execution error" );
}

// ----------------------------------------------------------------------------

bool MirrorVertFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // vmirror( image )

   if ( Distance( i, j ) != 1 )
   {
      info = "vmirror() takes a single argument";
      return false;
   }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "vmirror() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   return true;
}

IsoString MirrorVertFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // vmirror( image )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "vmirror() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );

   IsoString key = TheImageCache->MakeKey( ref->Id(), "_vmirror" );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );
      VerticalMirror() >> result;
      TheImageCache->AddImage( key, result );
   }

   return key;
}

void MirrorVertFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "vmirror(): Internal execution error" );
}

// ----------------------------------------------------------------------------

bool NormalizationFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // normalize( image[, a=0, b=1] )

   distance_type n = Distance( i, j );
   if ( n != 1 )
      if ( n != 3 )
      {
         info = "normalize() can only take 1 or 3 arguments";
         return false;
      }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "normalize() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   if ( ++i < j )
   {
      if ( (*i)->IsImageReference() )
      {
         info = "normalize() argument #2: The normalization lower bound must be an invariant scalar subexpression";
         arg = *i;
         return false;
      }

      if ( ++i < j )
      {
         if ( (*i)->IsImageReference() )
         {
            info = "normalize() argument #3: The normalization upper bound must be an invariant scalar subexpression";
            arg = *i;
            return false;
         }
      }
   }

   return true;
}

IsoString NormalizationFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // normalize( image[, a=0, b=1] )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "normalize() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );
   double a = 0, b = 1;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         a = S->Value();
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         a = P->PixelValue()[0];
      else
         throw ParseError( "normalize() argument #2: The normalization lower bound must be an invariant scalar subexpression" );

      if ( ++i < j )
      {
         if ( (*i)->IsSample() )
            b = S->Value();
         else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
            b = P->PixelValue()[0];
         else
            throw ParseError( "normalize() argument #3: The normalization upper bound must be an invariant scalar subexpression" );
      }

      if ( b < a )
         Swap( a, b );
   }

   IsoString key = TheImageCache->MakeKey( ref->Id(), IsoString().Format( "_normalize_%.16e_%.16e", a, b ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      int bps = Max( 32, ref->Image()->BitsPerSample() );
      if ( bps == 32 )
         if ( !ref->Image()->IsFloatSample() )
            if ( !ref->Image()->IsComplexSample() )
               bps = 64;
      ImageVariant result = NewGeneratorResult( ref, bps );
      result.Rescale( a, b );
      TheImageCache->AddImage( key, result );
   }

   return key;
}

void NormalizationFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "normalize(): Internal execution error" );
}

// ----------------------------------------------------------------------------

bool TruncationFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // truncate( image[, a=0, b=1] )

   distance_type n = Distance( i, j );
   if ( n != 1 )
      if ( n != 3 )
      {
         info = "truncate() can only take 1 or 3 arguments";
         return false;
      }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "truncate() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   if ( ++i < j )
   {
      if ( (*i)->IsImageReference() )
      {
         info = "truncate() argument #2: The truncation lower bound must be an invariant scalar subexpression";
         arg = *i;
         return false;
      }

      if ( ++i < j )
      {
         if ( (*i)->IsImageReference() )
         {
            info = "truncate() argument #3: The truncation upper bound must be an invariant scalar subexpression";
            arg = *i;
            return false;
         }
      }
   }

   return true;
}

IsoString TruncationFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // truncate( image[, a=0, b=1] )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "truncate() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );
   double a = 0, b = 1;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         a = S->Value();
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         a = P->PixelValue()[0];
      else
         throw ParseError( "truncate() argument #2: The truncation lower bound must be an invariant scalar subexpression" );

      if ( ++i < j )
      {
         if ( (*i)->IsSample() )
            b = S->Value();
         else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
            b = P->PixelValue()[0];
         else
            throw ParseError( "truncate() argument #3: The truncation upper bound must be an invariant scalar subexpression" );
      }

      if ( b < a )
         Swap( a, b );
   }

   IsoString key = TheImageCache->MakeKey( ref->Id(), IsoString().Format( "_truncate_%.16e_%.16e", a, b ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      int bps = Max( 32, ref->Image()->BitsPerSample() );
      if ( bps == 32 )
         if ( !ref->Image()->IsFloatSample() )
            if ( !ref->Image()->IsComplexSample() )
               bps = 64;
      ImageVariant result = NewGeneratorResult( ref, bps );
      result.Truncate( a, b );
      TheImageCache->AddImage( key, result );
   }

   return key;
}

void TruncationFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "truncate(): Internal execution error" );
}

// ----------------------------------------------------------------------------

bool BinarizationFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // binarize( image[, t=0.5] )

   distance_type n = Distance( i, j );
   if ( n != 1 )
      if ( n != 2 )
      {
         info = "binarize() can only take 1 or 2 arguments";
         return false;
      }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "binarize() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   if ( ++i < j )
      if ( (*i)->IsImageReference() )
      {
         info = "binarize() argument #2: The binarization threshold must be an invariant scalar subexpression";
         arg = *i;
         return false;
      }

   return true;
}

IsoString BinarizationFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // binarize( image[, t=0.5] )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "binarize() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );
   double t;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         t = S->Value();
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         t = P->PixelValue()[0];
      else
         throw ParseError( "binarize() argument #2: The binarization threshold must be an invariant scalar subexpression" );
   }
   else
   {
      double m, M;
      ref->Image()->GetExtremeSampleValues( m, M );
      t = (m + M)/2;
   }

   IsoString key = TheImageCache->MakeKey( ref->Id(), IsoString().Format( "_binarize_%.16e", t ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      int bps = Max( 32, ref->Image()->BitsPerSample() );
      if ( bps == 32 )
         if ( !ref->Image()->IsFloatSample() )
            if ( !ref->Image()->IsComplexSample() )
               bps = 64;
      ImageVariant result = NewGeneratorResult( ref, bps );
      result.Binarize( t );
      TheImageCache->AddImage( key, result );
   }

   return key;
}

void BinarizationFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "binarize(): Internal execution error" );
}

// ----------------------------------------------------------------------------

enum
{
   KrnFlat, KrnGaussian, KrnCount
};

KernelFilter LVarKernelFunction( int d, int k )
{
   switch( k )
   {
   default:
   case KrnFlat:
      return KernelFilter( d, 1.0 );
   case KrnGaussian:
      return GaussianFilter( d );
   }
}

SeparableFilter LVarKernelFunctionSeparable( int d, int k )
{
   switch( k )
   {
   default:
   case KrnFlat:
      return SeparableFilter( d, 1.0 );
   case KrnGaussian:
      return GaussianFilter( d ).AsSeparableFilter();
   }
}

bool LocalVarianceFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // lvar( image[, d=3[, k=krn_flat()]] )

   distance_type n = Distance( i, j );
   if ( n < 1 || n > 3 )
   {
      info = "lvar() takes from 1 to 3 arguments";
      return false;
   }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "lvar() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   if ( ++i < j )
   {
      if ( (*i)->IsImageReference() )
      {
         info = "lvar() argument #2: The local variance window size must be an invariant scalar subexpression";
         arg = *i;
         return false;
      }

      if ( (*i)->IsSample() )
      {
         if ( Frac( S->Value() ) != 0 )
         {
            info = "lvar() argument #2: The local variance window size must be an integer value";
            arg = *i;
            return false;
         }
         if ( S->Value() < 3 )
         {
            info = "lvar() argument #2: The local variance window size must be >= 3";
            arg = *i;
            return false;
         }
         if ( (int( S->Value() ) & 1) == 0 )
         {
            info = "lvar() argument #2: The local variance window size must be an odd integer";
            arg = *i;
            return false;
         }
      }

      if ( ++i < j )
      {
         if ( (*i)->IsImageReference() )
         {
            info = "lvar() argument #3: The local variance kernel function must be an invariant scalar subexpression";
            arg = *i;
            return false;
         }

         if ( (*i)->IsSample() )
         {
            if ( Frac( S->Value() ) != 0 )
            {
               info = "lvar() argument #3: The local variance kernel function must be an integer value";
               arg = *i;
               return false;
            }
            if ( S->Value() < 0 || S->Value() >= KrnCount )
            {
               info = "lvar() argument #3: Unknown or unsupported kernel function '" + String( int( S->Value() ) ) + "'";
               arg = *i;
               return false;
            }
         }
      }
   }

   return true;
}

IsoString LocalVarianceFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // lvar( image[, d=3[, k=krn_flat()]] )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "lvar() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );
   int d = 3;
   int k = KrnFlat;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         d = int( S->Value() );
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         d = int( P->PixelValue()[0] );
      else
         throw ParseError( "lvar() argument #2: The local variance window size must be an invariant scalar subexpression" );

      if ( d < 3 )
         throw ParseError( "lvar() argument #2: The local variance window size must be >= 3" );
      if ( (d & 1) == 0 )
         throw ParseError( "lvar() argument #2: The local variance window size must be an odd integer" );

      if ( ++i < j )
      {
         if ( (*i)->IsSample() )
            k = int( S->Value() );
         else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
            k = int( P->PixelValue()[0] );
         else
            throw ParseError( "lvar() argument #3: The local variance kernel function must be an integer value" );

         if ( k < 0 || k >= KrnCount )
            throw ParseError( "lvar() argument #3: Unknown or unsupported kernel function '" + String( k ) + '\'' );
      }
   }

   IsoString key = TheImageCache->MakeKey( ref->Id(), IsoString().Format( "_lvar_%d_%d", d, k ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref, 64 );

      ImageVariant X; X.CopyImage( result );
      result.Raise( 2.0 );

      int nofThreads = Thread::NumberOfThreads( PCL_MAX_PROCESSORS );
      if (  d >= FFTConvolution::FasterThanSeparableFilterSize( nofThreads )
         || d > ref->Image()->Width()
         || d > ref->Image()->Height() )
      {
         KernelFilter H = LVarKernelFunction( d, k );
         FFTConvolution Z( H );
         Z >> result;
         Z >> X;
      }
      else if ( d >= SeparableConvolution::FasterThanNonseparableFilterSize( nofThreads ) )
      {
         SeparableFilter H = LVarKernelFunctionSeparable( d, k );
         SeparableConvolution C( H );
         C >> result;
         C >> X;
      }
      else
      {
         KernelFilter H = LVarKernelFunction( d, k );
         Convolution C( H );
         C >> result;
         C >> X;
      }

      X.Raise( 2.0 );
      result.Subtract( X );

      TheImageCache->AddImage( key, result );
   }

   return key;
}

void LocalVarianceFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "lvar(): Internal execution error" );
}

// ----------------------------------------------------------------------------

void KrnFlatFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "krn_flat(): Internal parser error" );
}

bool KrnFlatFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void KrnFlatFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( KrnFlat );
}

// ----------------------------------------------------------------------------

void KrnGaussianFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "krn_gauss(): Internal parser error" );
}

bool KrnGaussianFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void KrnGaussianFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( KrnGaussian );
}

// ----------------------------------------------------------------------------

enum
{
   OpMov, OpAdd, OpSub, OpMul, OpDiv, OpPow, OpDif, OpMin, OpMax,
   OpColorBurn, OpLinearBurn, OpScreen, OpColorDodge, OpOverlay,
   OpSoftLight, OpHardLight, OpVividLight, OpLinearLight, OpPinLight, OpExclusion,
   OpCount
};

// ----------------------------------------------------------------------------

bool CombinationFunction::ValidateArguments( String& info, Expression*& arg, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // combine( image1, image2, op )

   distance_type n = Distance( i, j );
   if ( n != 3 )
   {
      info = "combine() takes 3 arguments";
      return false;
   }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "combine() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   ++i;

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "combine() argument #2: Must be an image reference or a functional subexpression evaluating to an image";
         arg = *i;
         return false;
      }

   ++i;

   if ( (*i)->IsImageReference() )
   {
      info = "combine() argument #3: The binary operation selector must be an invariant scalar subexpression";
      arg = *i;
      return false;
   }

   if ( (*i)->IsSample() )
   {
      if ( Frac( S->Value() ) != 0 )
      {
         info = "combine() argument #3: The binary operation selector must be an integer value";
         arg = *i;
         return false;
      }
      if ( S->Value() < 0 || S->Value() >= OpCount )
      {
         info = "combine() argument #3: Unknown or unsupported binary operation '" + String( int( S->Value() ) ) + "'";
         arg = *i;
         return false;
      }
   }

   return true;
}

IsoString CombinationFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // combine( image1, image2, op )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "combine() argument #1: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref1 = dynamic_cast<ImageReference*>( *i );

   ++i;

   if ( !(*i)->IsImageReference() )
      throw ParseError( "combine() argument #2: Must be an image reference or a functional subexpression evaluating to an image" );

   const ImageReference* ref2 = dynamic_cast<ImageReference*>( *i );

   ++i;

   int op;
   if ( (*i)->IsSample() )
      op = int( S->Value() );
   else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
      op = int( P->PixelValue()[0] );
   else
      throw ParseError( "combine() argument #3: The binary operation selector must be an invariant scalar subexpression" );

   if ( op < 0 || op >= OpCount )
      throw ParseError( "combine() argument #3: Unknown or unsupported binary operation '" + String( op ) + "'" );

   IsoString key = TheImageCache->MakeKey( ref1->Id() + '_' + ref2->Id(), IsoString().Format( "_combine_%d", op ) );

   if ( !TheImageCache->HasImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref1 );

      ImageVariant::image_op iop;
      switch ( op )
      {
      default: // ?!
      case OpMov:
         iop = ImageOp::Mov;
         break;
      case OpAdd:
         iop = ImageOp::Add;
         break;
      case OpSub:
         iop = ImageOp::Sub;
         break;
      case OpMul:
         iop = ImageOp::Mul;
         break;
      case OpDiv:
         iop = ImageOp::Div;
         break;
      case OpPow:
         iop = ImageOp::Pow;
         break;
      case OpDif:
         iop = ImageOp::Dif;
         break;
      case OpMin:
         iop = ImageOp::Min;
         break;
      case OpMax:
         iop = ImageOp::Max;
         break;
      case OpColorBurn:
         iop = ImageOp::ColorBurn;
         break;
      case OpLinearBurn:
         iop = ImageOp::LinearBurn;
         break;
      case OpScreen:
         iop = ImageOp::Screen;
         break;
      case OpColorDodge:
         iop = ImageOp::ColorDodge;
         break;
      case OpOverlay:
         iop = ImageOp::Overlay;
         break;
      case OpSoftLight:
         iop = ImageOp::SoftLight;
         break;
      case OpHardLight:
         iop = ImageOp::HardLight;
         break;
      case OpVividLight:
         iop = ImageOp::VividLight;
         break;
      case OpLinearLight:
         iop = ImageOp::LinearLight;
         break;
      case OpPinLight:
         iop = ImageOp::PinLight;
         break;
      case OpExclusion:
         iop = ImageOp::Exclusion;
         break;
      }

      result.Apply( *ref2->Image(), iop );

      TheImageCache->AddImage( key, result );
   }

   return key;
}

void CombinationFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "combine(): Internal execution error" );
}

// ----------------------------------------------------------------------------

void OpMovFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_mov(): Internal parser error" );
}

bool OpMovFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpMovFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpMov );
}

// ----------------------------------------------------------------------------

void OpAddFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_add(): Internal parser error" );
}

bool OpAddFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpAddFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpAdd );
}

// ----------------------------------------------------------------------------

void OpSubFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_sub(): Internal parser error" );
}

bool OpSubFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpSubFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpSub );
}

// ----------------------------------------------------------------------------

void OpMulFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_mul(): Internal parser error" );
}

bool OpMulFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpMulFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpMul );
}

// ----------------------------------------------------------------------------

void OpDivFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_div(): Internal parser error" );
}

bool OpDivFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpDivFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpDiv );
}

// ----------------------------------------------------------------------------

void OpPowFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_pow(): Internal parser error" );
}

bool OpPowFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpPowFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpPow );
}

// ----------------------------------------------------------------------------

void OpDifFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_dif(): Internal parser error" );
}

bool OpDifFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpDifFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpDif );
}

// ----------------------------------------------------------------------------

void OpMinFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_min(): Internal parser error" );
}

bool OpMinFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpMinFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpMin );
}

// ----------------------------------------------------------------------------

void OpMaxFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_max(): Internal parser error" );
}

bool OpMaxFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpMaxFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpMax );
}

// ----------------------------------------------------------------------------

void OpColorBurnFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_color_burn(): Internal parser error" );
}

bool OpColorBurnFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpColorBurnFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpColorBurn );
}

// ----------------------------------------------------------------------------

void OpLinearBurnFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_linear_burn(): Internal parser error" );
}

bool OpLinearBurnFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpLinearBurnFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpLinearBurn );
}

// ----------------------------------------------------------------------------

void OpScreenFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_screen(): Internal parser error" );
}

bool OpScreenFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpScreenFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpScreen );
}

// ----------------------------------------------------------------------------

void OpColorDodgeFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_color_dodge(): Internal parser error" );
}

bool OpColorDodgeFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpColorDodgeFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpColorDodge );
}

// ----------------------------------------------------------------------------

void OpOverlayFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_overlay(): Internal parser error" );
}

bool OpOverlayFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpOverlayFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpOverlay );
}

// ----------------------------------------------------------------------------

void OpSoftLightFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_soft_light(): Internal parser error" );
}

bool OpSoftLightFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpSoftLightFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpSoftLight );
}

// ----------------------------------------------------------------------------

void OpHardLightFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_hard_light(): Internal parser error" );
}

bool OpHardLightFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpHardLightFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpHardLight );
}

// ----------------------------------------------------------------------------

void OpVividLightFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_vivid_light(): Internal parser error" );
}

bool OpVividLightFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpVividLightFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpVividLight );
}

// ----------------------------------------------------------------------------

void OpLinearLightFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_linear_light(): Internal parser error" );
}

bool OpLinearLightFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpLinearLightFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpLinearLight );
}

// ----------------------------------------------------------------------------

void OpPinLightFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_pin_light(): Internal parser error" );
}

bool OpPinLightFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpPinLightFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpPinLight );
}

// ----------------------------------------------------------------------------

void OpExclusionFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "op_exclusion(): Internal parser error" );
}

bool OpExclusionFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void OpExclusionFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( OpExclusion );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Generators.cpp - Released 2021-05-05T15:38:07Z
