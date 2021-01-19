
#include "Data.h"
#include "Function.h"
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

bool GaussianConvolutionFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // gconv( image[, sigma=2[, rho=1[, theta=0[, eps=0.01]]]] )

   if ( Distance( i, j ) == 0 )
   {
      info = "gconv() takes from 1 to 5 arguments";
      return false;
   }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "gconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         return false;
      }

   if ( Distance( i, j ) > 1 )
   {
      ++i;

      if ( (*i)->IsImageReference() )
      {
         info = "gconv() argument #2: The filter standard deviation must be an invariant scalar subexpression";
         return false;
      }

      if ( (*i)->IsSample() )
         if ( S->Value() < 0 || S->Value() + 1 == 1 )
         {
            info = "gconv() argument #2: The filter standard deviation must be > 0 px";
            return false;
         }

      if ( Distance( i, j ) > 2 )
      {
         ++i;

         if ( (*i)->IsImageReference() )
         {
            info = "gconv() argument #3: The filter aspect ratio must be an invariant scalar subexpression";
            return false;
         }

         if ( (*i)->IsSample() )
            if ( S->Value() < 0 || S->Value() + 1 == 1 || S->Value() > 1 )
            {
               info = "gconv() argument #3: The filter aspect ratio must be in the (0,1] range";
               return false;
            }

         if ( Distance( i, j ) > 3 )
         {
            ++i;

            if ( (*i)->IsImageReference() )
            {
               info = "gconv() argument #4: The filter rotation angle must be an invariant scalar subexpression";
               return false;
            }

            if ( (*i)->IsSample() )
               if ( S->Value() < -180 || S->Value() > 180 )
               {
                  info = "gconv() argument #4: The filter rotation angle must be in the range [-180,+180] degrees";
                  return false;
               }

            if ( Distance( i, j ) > 4 )
            {
               ++i;

               if ( (*i)->IsImageReference() )
               {
                  info = "gconv() argument #5: The filter truncation error must be an invariant scalar subexpression";
                  return false;
               }

               if ( (*i)->IsSample() )
                  if ( S->Value() < 0 || S->Value() + 1 == 1 )
                  {
                     info = "gconv() argument #5: The filter truncation error must be > 0";
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
      throw ParseError( "gconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image." );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );
   float sigma = 2;
   float rho = 1;
   float theta = 0;
   float eps = 0.01;

   if ( Distance( ++i, j ) > 0 )
   {
      if ( (*i)->IsSample() )
         sigma = S->Value();
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         sigma = P->PixelValue()[0];
      else
         throw ParseError( "gconv() argument #2: The filter standard deviation must be an invariant scalar subexpression." );
      if ( sigma < 0 || sigma + 1 == 1 )
         throw ParseError( "gconv() argument #2: The filter standard deviation must be > 0 px; got " + String( sigma ) + '.' );

      if ( Distance( ++i, j ) > 0 )
      {
         if ( (*i)->IsSample() )
            rho = S->Value();
         else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
            rho = P->PixelValue()[0];
         else
            throw ParseError( "gconv() argument #3: The filter aspect ratio must be an invariant scalar subexpression." );
         if ( rho < 0 || rho + 1 == 1 || rho > 1 )
            throw ParseError( "gconv() argument #3: The filter aspect ratio must be in the (0,1] range; got " + String( rho ) + '.' );

         if ( Distance( ++i, j ) > 0 )
         {
            if ( (*i)->IsSample() )
               theta = S->Value();
            else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
               theta = P->PixelValue()[0];
            else
               throw ParseError( "gconv() argument #4: The filter rotation angle must be an invariant scalar subexpression." );
            if ( theta < -180 || theta > 180 )
               throw ParseError( "gconv() argument #4: The filter rotation angle must be in the range [-180,+180] degrees; got " +  + '.' );

            if ( Distance( ++i, j ) > 0 )
            {
               if ( (*i)->IsSample() )
                  eps = S->Value();
               else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
                  eps = P->PixelValue()[0];
               else
                  throw ParseError( "gconv() argument #5: The filter truncation error must be an invariant scalar subexpression." );
               if ( eps < 0 || eps + 1 == 1 )
                  throw ParseError( "gconv() argument #5: The filter truncation error must be > 0; got " + String( eps ) + '.' );
            }
         }
      }
   }

   IsoString key = IsoString( ref->Id() ) + IsoString().Format( "_gconv_%.4f_%.4f_%.4f_%.4f", sigma, rho, theta, eps );

   if ( !PixelMathInstance::HasInternalImage( key ) )
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

      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void GaussianConvolutionFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "gconv(): Internal execution error." );
}

// ----------------------------------------------------------------------------

bool BoxConvolutionFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // bconv( image[, n=3] )

   if ( Distance( i, j ) != 1 )
      if ( Distance( i, j ) != 2 )
      {
         info = "bconv() takes 1 or 2 arguments";
         return false;
      }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "bconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         return false;
      }

   if ( ++i < j )
   {
      if ( (*i)->IsImageReference() )
      {
         info = "bconv() argument #2: The filter size must be an invariant scalar subexpression";
         return false;
      }

      if ( (*i)->IsSample() )
      {
         if ( Frac( S->Value() ) != 0 )
         {
            info = "bconv() argument #2: The filter size must be an integer value";
            return false;
         }
         if ( S->Value() < 3 )
         {
            info = "bconv() argument #2: The smallest valid filter size is 3 px";
            return false;
         }
         if ( (int( S->Value() ) & 1) == 0 )
         {
            info = "bconv() argument #2: The filter size must be an odd integer";
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
      throw ParseError( "bconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image." );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );
   int n = 3;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         n = int( S->Value() );
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         n = int( P->PixelValue()[0] );
      else
         throw ParseError( "bconv() argument #2: The filter size must be an invariant scalar subexpression." );

      if ( n < 3 )
         throw ParseError( "bconv() argument #2: The smallest valid filter size is 3 px." );
      if ( (n & 1) == 0 )
         throw ParseError( "bconv() argument #2: The filter size must be an odd integer." );
   }

   IsoString key = IsoString( ref->Id() ) + IsoString().Format( "_bconv_%d", n );

   if ( !PixelMathInstance::HasInternalImage( key ) )
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

      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void BoxConvolutionFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "bconv(): Internal execution error." );
}

// ----------------------------------------------------------------------------

bool KernelConvolutionFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // kconv( image, k11, k12, k13, k21, k22, k23, k31, k32, k33[, ...] )

   if ( Distance( i, j ) < 10 )
   {
      info = "kconv() takes at least 10 arguments";
      return false;
   }

   size_type n = Distance( i, j ) - 1;
   if ( (n & 1) == 0 )
   {
      info = "kconv() requires an odd number of kernel filter element arguments";
      return false;
   }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "kconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         return false;
      }

   for ( int k = 2; ++i < j; ++k )
      if ( (*i)->IsImageReference() )
      {
         info = "kconv() argument #" + String( k ) + ": Kernel filter elements must be invariant scalar subexpressions";
         return false;
      }

   return true;
}

IsoString KernelConvolutionFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // kconv( image, k11, k12, k13, k21, k22, k23, k31, k32, k33[, ...] )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "kconv() argument #1: Must be an image reference or a functional subexpression evaluating to an image." );

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
         throw ParseError( "kconv() argument #" + String( 2+k ) + ": Kernel filter elements must be invariant scalar subexpressions." );
   }

   IsoString key = IsoString( ref->Id() ) + "_kconv_" + IsoString::ToHex( MD5().Hash( K ) );

   if ( !PixelMathInstance::HasInternalImage( key ) )
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

      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void KernelConvolutionFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "kconv(): Internal execution error." );
}

// ----------------------------------------------------------------------------

enum PMStructuringElement
{
   Square, Circular, Orthogonal, Diagonal, Star, ThreeWay, count
};

static bool ValidateMorphologicalTransformationGeneratorParameters( const String& functionName, String& info,
                                       Expression::component_list::const_iterator i, Expression::component_list::const_iterator j )
{
   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = functionName + "() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         return false;
      }

   if ( ++i < j )
   {
      if ( (*i)->IsImageReference() )
      {
         info = functionName + "() argument #2: The filter size must be an invariant scalar subexpression";
         return false;
      }

      if ( (*i)->IsSample() )
      {
         if ( Frac( S->Value() ) != 0 )
         {
            info = functionName + "() argument #2: The filter size must be an integer value";
            return false;
         }
         if ( S->Value() < 3 )
         {
            info = functionName + "() argument #2: The smallest valid filter size is 3 pixels";
            return false;
         }
         if ( (int( S->Value() ) & 1) == 0 )
         {
            info = functionName + "() argument #2: The filter size must be an odd integer";
            return false;
         }
      }

      if ( ++i < j )
      {
         if ( (*i)->IsImageReference() )
         {
            info = functionName + "() argument #3: The structuring element selector must be an invariant scalar subexpression";
            return false;
         }

         if ( (*i)->IsSample() )
         {
            if ( Frac( S->Value() ) != 0 )
            {
               info = functionName + "() argument #3: The structuring element selector must be an integer value";
               return false;
            }
            if ( S->Value() < 0 || S->Value() >= PMStructuringElement::count )
            {
               info = functionName + "() argument #3: Unknown or unsupported structuring element selector '" + String( int( S->Value() ) ) + "'";
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
      throw ParseError( functionName + "() argument #1: Must be an image reference or a functional subexpression evaluating to an image." );

   ref = dynamic_cast<ImageReference*>( *i );
   n = 3;
   s = PMStructuringElement::Square;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         n = int( S->Value() );
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         n = int( P->PixelValue()[0] );
      else
         throw ParseError( functionName + "() argument #2: The filter size must be an invariant scalar subexpression." );

      if ( n < 3 )
         throw ParseError( functionName + "() argument #2: The smallest valid filter size is 3 pixels." );
      if ( (n & 1) == 0 )
         throw ParseError( functionName + "() argument #2: The filter size must be an odd integer." );

      if ( ++i < j )
      {
         if ( (*i)->IsSample() )
            s = int( S->Value() );
         else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
            s = int( P->PixelValue()[0] );
         else
            throw ParseError( functionName + "() argument #3: The structuring element selector must be an invariant scalar subexpression." );

         if ( s < 0 || s >= PMStructuringElement::count )
            throw ParseError( functionName + "() argument #3: Unknown or unsupported structuring element selector '" + String( s ) + "'." );
      }
   }
}

static StructuringElement* NewPMStructuringElement( int n, int s )
{
   switch ( s )
   {
   default: // ?!
   case PMStructuringElement::Square:     return new BoxStructure( n );
   case PMStructuringElement::Circular:   return new CircularStructure( n );
   case PMStructuringElement::Orthogonal: return new OrthogonalStructure( n );
   case PMStructuringElement::Diagonal:   return new DiagonalStructure( n );
   case PMStructuringElement::Star:       return new StarStructure( n );
   case PMStructuringElement::ThreeWay:   return new ThreeWayStructure( n );
   }
}

// ----------------------------------------------------------------------------

bool MedianFilterFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // medfilt( image[, n=3[, str=str_square()]] )

   if ( Distance( i, j ) < 1 || Distance( i, j ) > 3 )
   {
      info = "medfilt() takes from 1 to 3 arguments";
      return false;
   }

   return ValidateMorphologicalTransformationGeneratorParameters( "medfilt", info, i, j );
}

IsoString MedianFilterFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // medfilt( image[, n=3[, str=str_square()]] )

   const ImageReference* ref;
   int n, s;
   GetMorphologicalTransformationGeneratorParameters( "medfilt", ref, n, s, i, j );

   IsoString key = IsoString( ref->Id() ) + IsoString().Format( "_medfilt_%d_%d", n, s );

   if ( !PixelMathInstance::HasInternalImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      AutoPointer<StructuringElement> str = NewPMStructuringElement( n, s );
      MorphologicalTransformation( MedianFilter(), *str ) >> result;

      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void MedianFilterFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "medfilt(): Internal execution error." );
}

// ----------------------------------------------------------------------------

bool ErosionFilterFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // erosion( image[, n=3[, str=str_square()]] )

   if ( Distance( i, j ) < 1 || Distance( i, j ) > 3 )
   {
      info = "erosion() takes from 1 to 3 arguments";
      return false;
   }

   return ValidateMorphologicalTransformationGeneratorParameters( "erosion", info, i, j );
}

IsoString ErosionFilterFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // erosion( image[, n=3[, str=str_square()]] )

   const ImageReference* ref;
   int n, s;
   GetMorphologicalTransformationGeneratorParameters( "erosion", ref, n, s, i, j );

   IsoString key = IsoString( ref->Id() ) + IsoString().Format( "_erosion_%d_%d", n, s );

   if ( !PixelMathInstance::HasInternalImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      AutoPointer<StructuringElement> str = NewPMStructuringElement( n, s );
      MorphologicalTransformation( ErosionFilter(), *str ) >> result;

      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void ErosionFilterFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "erosion(): Internal execution error." );
}

// ----------------------------------------------------------------------------

bool DilationFilterFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // dilation( image[, n=3[, str=str_square()]] )

   if ( Distance( i, j ) < 1 || Distance( i, j ) > 3 )
   {
      info = "dilation() takes from 1 to 3 arguments";
      return false;
   }

   return ValidateMorphologicalTransformationGeneratorParameters( "dilation", info, i, j );
}

IsoString DilationFilterFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // dilation( image[, n=3[, str=str_square()]] )

   const ImageReference* ref;
   int n, s;
   GetMorphologicalTransformationGeneratorParameters( "dilation", ref, n, s, i, j );

   IsoString key = IsoString( ref->Id() ) + IsoString().Format( "_dilation_%d_%d", n, s );

   if ( !PixelMathInstance::HasInternalImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      AutoPointer<StructuringElement> str = NewPMStructuringElement( n, s );
      MorphologicalTransformation( DilationFilter(), *str ) >> result;

      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void DilationFilterFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "dilation(): Internal execution error." );
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
   r.SetSamples( PMStructuringElement::Square );
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
   r.SetSamples( PMStructuringElement::Circular );
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
   r.SetSamples( PMStructuringElement::Orthogonal );
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
   r.SetSamples( PMStructuringElement::Diagonal );
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
   r.SetSamples( PMStructuringElement::Star );
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
   r.SetSamples( PMStructuringElement::ThreeWay );
}

// ----------------------------------------------------------------------------

bool TranslationFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
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
         return false;
      }

   for ( int k = 2; ++i < j; ++k )
      if ( (*i)->IsImageReference() )
      {
         info = "translate() argument #" + String( k ) + ": translation increments must be invariant scalar subexpressions";
         return false;
      }

   return true;
}

IsoString TranslationFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // translate( image, dx, dy )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "translate() argument #1: Must be an image reference or a functional subexpression evaluating to an image." );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );

   double dx;
   if ( (*++i)->IsSample() )
      dx = S->Value();
   else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
      dx = P->PixelValue()[0];
   else
      throw ParseError( "translate() argument #2: The X-axis increment must be an invariant scalar subexpression." );

   double dy;
   if ( (*++i)->IsSample() )
      dy = S->Value();
   else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
      dy = P->PixelValue()[0];
   else
      throw ParseError( "translate() argument #3: The Y-axis increment must be an invariant scalar subexpression." );

   IsoString key = IsoString( ref->Id() ) + IsoString().Format( "_translate_%.4f_%.4f", dx, dy );

   if ( !PixelMathInstance::HasInternalImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      BicubicPixelInterpolation interpolation;
      Translation( interpolation, dx, dy ) >> result;

      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void TranslationFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "translate(): Internal execution error." );
}

// ----------------------------------------------------------------------------

bool RotationFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // rotate( image, angle[, cx, cy] )

   if ( Distance( i, j ) != 2 )
      if ( Distance( i, j ) != 4 )
      {
         info = "rotate() takes 2 or 4 arguments";
         return false;
      }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "rotate() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         return false;
      }

   for ( int k = 2; ++i < j; ++k )
      if ( (*i)->IsImageReference() )
      {
         info = "rotate() argument #" + String( k ) + ": rotation angles and center coordinates must be invariant scalar subexpressions";
         return false;
      }

   return true;
}

IsoString RotationFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // rotate( image, angle[, cx, cy] )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "rotate() argument #1: Must be an image reference or a functional subexpression evaluating to an image." );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );

   double angle;
   if ( (*++i)->IsSample() )
      angle = S->Value();
   else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
      angle = P->PixelValue()[0];
   else
      throw ParseError( "rotate() argument #2: The rotation angle must be an invariant scalar subexpression." );

   double cx = 0, cy = 0;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         cx = S->Value();
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         cx = P->PixelValue()[0];
      else
         throw ParseError( "rotate() argument #3: The center x coordinate must be an invariant scalar subexpression." );

      if ( ++i < j )
      {
         if ( (*i)->IsSample() )
            cy = S->Value();
         else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
            cy = P->PixelValue()[0];
         else
            throw ParseError( "rotate() argument #4: The center y coordinate must be an invariant scalar subexpression." );
      }
   }

   IsoString key = IsoString( ref->Id() ) + IsoString().Format( "_rotate_%.6f_%.4f_%.4f", angle, cx, cy );

   if ( !PixelMathInstance::HasInternalImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );

      BicubicPixelInterpolation interpolation;
      Rotation( interpolation, Rad( angle ), cx, cy ) >> result;
      int dx0 = (result->Width() - ref->Image()->Width()) >> 1;
      int dy0 = (result->Height() - ref->Image()->Height()) >> 1;
      int dx1 = dx0 + (((result->Width() - ref->Image()->Width()) & 1) ? 1 : 0);
      int dy1 = dy0 + (((result->Height() - ref->Image()->Height()) & 1) ? 1 : 0);
      result.CropBy( -dx0, -dy0, -dx1, -dy1 );

      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void RotationFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "rotate(): Internal execution error." );
}

// ----------------------------------------------------------------------------

bool MirrorHorzFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // hmirror( image )

   if ( Distance( i, j ) != 1 )
   {
      info = "hmirror() takes 1 argument";
      return false;
   }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "hmirror() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         return false;
      }

   return true;
}

IsoString MirrorHorzFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // hmirror( image )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "hmirror() argument #1: Must be an image reference or a functional subexpression evaluating to an image." );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );

   IsoString key = IsoString( ref->Id() ) + "_hmirror";

   if ( !PixelMathInstance::HasInternalImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );
      HorizontalMirror() >> result;
      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void MirrorHorzFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "hmirror(): Internal execution error." );
}

// ----------------------------------------------------------------------------

bool MirrorVertFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // vmirror( image )

   if ( Distance( i, j ) != 1 )
   {
      info = "vmirror() takes 1 argument";
      return false;
   }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "vmirror() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         return false;
      }

   return true;
}

IsoString MirrorVertFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // vmirror( image )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "vmirror() argument #1: Must be an image reference or a functional subexpression evaluating to an image." );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );

   IsoString key = IsoString( ref->Id() ) + "_vmirror";

   if ( !PixelMathInstance::HasInternalImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref );
      VerticalMirror() >> result;
      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void MirrorVertFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "vmirror(): Internal execution error." );
}

// ----------------------------------------------------------------------------

bool NormalizationFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // normalize( image[, a=0, b=1] )

   if ( Distance( i, j ) != 1 )
      if ( Distance( i, j ) != 3 )
      {
         info = "normalize() takes 1 or 3 arguments";
         return false;
      }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "normalize() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         return false;
      }

   if ( ++i < j )
   {
      if ( (*i)->IsImageReference() )
      {
         info = "normalize() argument #2: The normalization lower bound must be an invariant scalar subexpression";
         return false;
      }

      if ( ++i < j )
      {
         if ( (*i)->IsImageReference() )
         {
            info = "normalize() argument #3: The normalization upper bound must be an invariant scalar subexpression";
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
      throw ParseError( "normalize() argument #1: Must be an image reference or a functional subexpression evaluating to an image." );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );
   double a = 0, b = 1;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         a = S->Value();
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         a = P->PixelValue()[0];
      else
         throw ParseError( "normalize() argument #2: The normalization lower bound must be an invariant scalar subexpression." );

      if ( ++i < j )
      {
         if ( (*i)->IsSample() )
            b = S->Value();
         else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
            b = P->PixelValue()[0];
         else
            throw ParseError( "normalize() argument #3: The normalization upper bound must be an invariant scalar subexpression." );
      }

      if ( b < a )
         Swap( a, b );
   }

   IsoString key = IsoString( ref->Id() ) + IsoString().Format( "_normalize_%.15e_%.15e", a, b );

   if ( !PixelMathInstance::HasInternalImage( key ) )
   {
      int bps = Max( 32, ref->Image()->BitsPerSample() );
      if ( bps == 32 )
         if ( !ref->Image()->IsFloatSample() )
            if ( !ref->Image()->IsComplexSample() )
               bps = 64;
      ImageVariant result = NewGeneratorResult( ref, bps );
      result.Rescale( a, b );
      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void NormalizationFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "normalize(): Internal execution error." );
}

// ----------------------------------------------------------------------------

bool LocalVarianceFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   // lvar( image[, d=3] )

   if ( Distance( i, j ) != 1 )
      if ( Distance( i, j ) != 2 )
      {
         info = "lvar() takes 1 or 2 arguments";
         return false;
      }

   if ( !(*i)->IsImageReference() )
      if ( !(*i)->IsFunctional() )
      {
         info = "lvar() argument #1: Must be an image reference or a functional subexpression evaluating to an image";
         return false;
      }

   if ( ++i < j )
   {
      if ( (*i)->IsImageReference() )
      {
         info = "lvar() argument #2: The local variance window size must be an invariant scalar subexpression";
         return false;
      }

      if ( (*i)->IsSample() )
      {
         if ( Frac( S->Value() ) != 0 )
         {
            info = "lvar() argument #2: The local variance window size must be an integer value";
            return false;
         }
         if ( S->Value() < 3 )
         {
            info = "lvar() argument #2: The smallest valid local variance window size is 3 pixels";
            return false;
         }
         if ( (int( S->Value() ) & 1) == 0 )
         {
            info = "lvar() argument #2: The local variance window size must be an odd integer";
            return false;
         }
      }
   }

   return true;
}

IsoString LocalVarianceFunction::GenerateImage( component_list::const_iterator i, component_list::const_iterator j ) const
{
   // lvar( image[, d=3] )

   if ( !(*i)->IsImageReference() )
      throw ParseError( "lvar() argument #1: Must be an image reference or a functional subexpression evaluating to an image." );

   const ImageReference* ref = dynamic_cast<ImageReference*>( *i );
   int d = 3;
   if ( ++i < j )
   {
      if ( (*i)->IsSample() )
         d = int( S->Value() );
      else if ( (*i)->IsPixel() && P->PixelValue().Length() == 1 )
         d = int( P->PixelValue()[0] );
      else
         throw ParseError( "lvar() argument #2: The local variance window size must be an invariant scalar subexpression." );

      if ( d < 3 )
         throw ParseError( "lvar() argument #2: The smallest valid local variance window size is 3 pixels." );
      if ( (d & 1) == 0 )
         throw ParseError( "lvar() argument #2: The local variance window size must be an odd integer." );
   }

   IsoString key = IsoString( ref->Id() ) + IsoString().Format( "_lvar_%d", d );

   if ( !PixelMathInstance::HasInternalImage( key ) )
   {
      ImageVariant result = NewGeneratorResult( ref, 64 );

      ImageVariant X; X.CopyImage( result );
      result.Raise( 2.0 );

      int nofThreads = Thread::NumberOfThreads( PCL_MAX_PROCESSORS );
      if (  d >= FFTConvolution::FasterThanSeparableFilterSize( nofThreads )
         || d > ref->Image()->Width()
         || d > ref->Image()->Height() )
      {
         KernelFilter H( d, 1.0 );
         FFTConvolution Z( H );
         Z >> result;
         Z >> X;
      }
      else if ( d >= SeparableConvolution::FasterThanNonseparableFilterSize( nofThreads ) )
      {
         SeparableFilter H( d, 1.0 );
         SeparableConvolution C( H );
         C >> result;
         C >> X;
      }
      else
      {
         KernelFilter H( d, 1.0 );
         Convolution C( H );
         C >> result;
         C >> X;
      }

      X.Raise( 2.0 );
      result.Subtract( X );

      PixelMathInstance::AddInternalImage( key, result );
   }

   return key;
}

void LocalVarianceFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw ParseError( "lvar(): Internal execution error." );
}

// ----------------------------------------------------------------------------

} // pcl
