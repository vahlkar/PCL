//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// PixelMathInstance.cpp - Released 2021-05-05T15:38:07Z
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
#include "FlowControl.h"
#include "Function.h"
#include "ImageCache.h"
#include "PixelMathInstance.h"

#include <pcl/AutoLock.h>
#include <pcl/AutoViewLock.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/StandardStatus.h>

// #include <iostream>

namespace pcl
{

// ----------------------------------------------------------------------------

int PixelMathInstance::s_targetWidth = 0;
int PixelMathInstance::s_targetHeight = 0;
int PixelMathInstance::s_targetNumberOfChannels = 0;
bool PixelMathInstance::s_targetIsColor = false;
RGBColorSystem* PixelMathInstance::s_targetRGBWS = nullptr;

// ----------------------------------------------------------------------------

PixelMathInstance::PixelMathInstance( const MetaProcess* P )
   : ProcessImplementation( P )
   , p_useSingleExpression( ThePMUseSingleExpressionParameter->DefaultValue() )
   , p_clearImageCacheAndExit( ThePMClearImageCacheAndExitParameter->DefaultValue() )
   , p_cacheGeneratedImages( ThePMCacheGeneratedImagesParameter->DefaultValue() )
   , p_generateOutput( ThePMGenerateOutputParameter->DefaultValue() )
   , p_singleThreaded( ThePMSingleThreadedParameter->DefaultValue() )
   , p_optimization( ThePMOptimizationParameter->DefaultValue() )
   , p_use64BitWorkingImage( ThePMUse64BitWorkingImageParameter->DefaultValue() )
   , p_rescaleResult( ThePMRescaleResultParameter->DefaultValue() )
   , p_rescaleLower( ThePMRescaleLowerBoundParameter->DefaultValue() )
   , p_rescaleUpper( ThePMRescaleUpperBoundParameter->DefaultValue() )
   , p_truncateResult( ThePMTruncateResultParameter->DefaultValue() )
   , p_truncateLower( ThePMTruncateLowerBoundParameter->DefaultValue() )
   , p_truncateUpper( ThePMTruncateUpperBoundParameter->DefaultValue() )
   , p_createNewImage( ThePMCreateNewImageParameter->DefaultValue() )
   , p_showNewImage( ThePMShowNewImageParameter->DefaultValue() )
   , p_newImageId( ThePMNewImageIdentifierParameter->DefaultValue() )
   , p_newImageWidth( int( ThePMNewImageWidthParameter->DefaultValue() ) )
   , p_newImageHeight( int( ThePMNewImageHeightParameter->DefaultValue() ) )
   , p_newImageAlpha( ThePMNewImageAlphaParameter->DefaultValue() )
   , p_newImageColorSpace( PMNewImageColorSpace::Default )
   , p_newImageSampleFormat( PMNewImageSampleFormat::Default )
{
}

// ----------------------------------------------------------------------------

PixelMathInstance::PixelMathInstance( const PixelMathInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void PixelMathInstance::Assign( const ProcessImplementation& p )
{
   const PixelMathInstance* x = dynamic_cast<const PixelMathInstance*>( &p );
   if ( x != nullptr )
   {
      p_expression[0]          = x->p_expression[0];
      p_expression[1]          = x->p_expression[1];
      p_expression[2]          = x->p_expression[2];
      p_expression[3]          = x->p_expression[3];
      p_useSingleExpression    = x->p_useSingleExpression;
      p_symbols                = x->p_symbols;
      p_clearImageCacheAndExit = x->p_clearImageCacheAndExit;
      p_cacheGeneratedImages   = x->p_cacheGeneratedImages;
      p_generateOutput         = x->p_generateOutput;
      p_singleThreaded         = x->p_singleThreaded;
      p_optimization           = x->p_optimization;
      p_use64BitWorkingImage   = x->p_use64BitWorkingImage;
      p_rescaleResult          = x->p_rescaleResult;
      p_rescaleLower           = x->p_rescaleLower;
      p_rescaleUpper           = x->p_rescaleUpper;
      p_truncateResult         = x->p_truncateResult;
      p_truncateLower          = x->p_truncateLower;
      p_truncateUpper          = x->p_truncateUpper;
      p_createNewImage         = x->p_createNewImage;
      p_showNewImage           = x->p_showNewImage;
      p_newImageId             = x->p_newImageId;
      p_newImageWidth          = x->p_newImageWidth;
      p_newImageHeight         = x->p_newImageHeight;
      p_newImageAlpha          = x->p_newImageAlpha;
      p_newImageColorSpace     = x->p_newImageColorSpace;
      p_newImageSampleFormat   = x->p_newImageSampleFormat;
      o_outputData             = x->o_outputData;
   }
}

// ----------------------------------------------------------------------------

bool PixelMathInstance::IsHistoryUpdater( const View& view ) const
{
   return !p_clearImageCacheAndExit
       && p_generateOutput
       && (!p_createNewImage || view.IsPreview());
}

// ----------------------------------------------------------------------------

UndoFlags PixelMathInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool PixelMathInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "This implementation of PixelMath cannot be executed on complex images.";
      return false;
   }

   if ( p_createNewImage && view.IsPreview() )
   {
      if ( p_newImageWidth > 0 || p_newImageHeight > 0 /*|| p_newImageColorSpace != PMNewImageColorSpace::SameAsTarget*/ )
      {
         whyNot = "This instance of PixelMath cannot be executed on previews because some of its "
                  "parameters have been set to generate a new image with specific dimensions.";
         return false;
      }
   }

   return true;
}

// ----------------------------------------------------------------------------

bool PixelMathInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_generateOutput && !p_createNewImage ||
        p_newImageWidth <= 0 || p_newImageHeight <= 0 ||
        p_newImageColorSpace == PMNewImageColorSpace::SameAsTarget ||
        p_generateOutput && p_newImageSampleFormat == PMNewImageSampleFormat::SameAsTarget )
   {
      whyNot = "This instance of PixelMath cannot be executed in the global context because some of its "
               "parameters depend on a particular target image.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Pixel reading functions, no interpolation.
 */

template <class P>
inline static void GetPixel( Pixel& p, const GenericImage<P>& image, int x, int y )
{
   double v0, v1, v2;
   P::FromSample( v0, image( x, y, 0 ) );
   P::FromSample( v1, image( x, y, 1 ) );
   P::FromSample( v2, image( x, y, 2 ) );
   p.SetSamples( v0, v1, v2 );
}

inline static void GetPixel( Pixel& p, const ImageVariant& image, int x, int y )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: GetPixel( p, static_cast<const Image&>( *image ), x, y ); break;
      case 64: GetPixel( p, static_cast<const DImage&>( *image ), x, y ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: GetPixel( p, static_cast<const UInt8Image&>( *image ), x, y ); break;
      case 16: GetPixel( p, static_cast<const UInt16Image&>( *image ), x, y ); break;
      case 32: GetPixel( p, static_cast<const UInt32Image&>( *image ), x, y ); break;
      }
}

template <class P>
inline static void GetPixel( Pixel& p, const GenericImage<P>& image, int x, int y, int c )
{
   double v;
   P::FromSample( v, image( x, y, c ) );
   p.SetSamples( v );
}

inline static void GetPixel( Pixel& p, const ImageVariant& image, int x, int y, int c )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: GetPixel( p, static_cast<const Image&>( *image ), x, y, c ); break;
      case 64: GetPixel( p, static_cast<const DImage&>( *image ), x, y, c ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: GetPixel( p, static_cast<const UInt8Image&>( *image ), x, y, c ); break;
      case 16: GetPixel( p, static_cast<const UInt16Image&>( *image ), x, y, c ); break;
      case 32: GetPixel( p, static_cast<const UInt32Image&>( *image ), x, y, c ); break;
      }
}

inline static void GetPixel( Pixel& p, const ImageReference& r, int x, int y, int c )
{
   if ( r.ChannelIndex() >= 0 )
   {
      c = r.ChannelIndex();
      if ( c >= r.Image()->NumberOfChannels() )
         throw ParseError( ("Channel index out of range: " + r.Id()).AppendFormat( "[%d]", c ) );
      GetPixel( p, *r.Image(), x, y, c );
   }
   else if ( !r.Image()->IsColor() )
      GetPixel( p, *r.Image(), x, y, 0 );
   else
      GetPixel( p, *r.Image(), x, y );
}

/*
 * Pixel reading functions, interpolated sample values.
 */

template <class P>
inline static void GetPixel( Pixel& p, const GenericImage<P>& image, double x, double y, void** i )
{
   double v0, v1, v2;
   P::FromSample( v0, (*reinterpret_cast<PixelInterpolation::Interpolator<P>*>( i[0] ))( x, y ) );
   P::FromSample( v1, (*reinterpret_cast<PixelInterpolation::Interpolator<P>*>( i[1] ))( x, y ) );
   P::FromSample( v2, (*reinterpret_cast<PixelInterpolation::Interpolator<P>*>( i[2] ))( x, y ) );
   p.SetSamples( v0, v1, v2 );
}

inline static void GetPixel( Pixel& p, const ImageVariant& image, double x, double y, void** i )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: GetPixel( p, static_cast<const Image&>( *image ), x, y, i ); break;
      case 64: GetPixel( p, static_cast<const DImage&>( *image ), x, y, i ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: GetPixel( p, static_cast<const UInt8Image&>( *image ), x, y, i ); break;
      case 16: GetPixel( p, static_cast<const UInt16Image&>( *image ), x, y, i ); break;
      case 32: GetPixel( p, static_cast<const UInt32Image&>( *image ), x, y, i ); break;
      }
}

template <class P>
inline static void GetPixel( Pixel& p, const GenericImage<P>& image, double x, double y, int c, void** i )
{
   double v;
   P::FromSample( v, (*reinterpret_cast<PixelInterpolation::Interpolator<P>*>( i[c] ))( x, y ) );
   p.SetSamples( v );
}

inline static void GetPixel( Pixel& p, const ImageVariant& image, double x, double y, int c, void** i )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: GetPixel( p, static_cast<const Image&>( *image ), x, y, c, i ); break;
      case 64: GetPixel( p, static_cast<const DImage&>( *image ), x, y, c, i ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: GetPixel( p, static_cast<const UInt8Image&>( *image ), x, y, c, i ); break;
      case 16: GetPixel( p, static_cast<const UInt16Image&>( *image ), x, y, c, i ); break;
      case 32: GetPixel( p, static_cast<const UInt32Image&>( *image ), x, y, c, i ); break;
      }
}

inline static void GetPixel( Pixel& p, const ImageReference& r, double x, double y, int c )
{
   if ( r.ChannelIndex() >= 0 )
   {
      c = r.ChannelIndex();
      if ( c >= r.Image()->NumberOfChannels() )
         throw ParseError( ("Channel index out of range: " + r.Id()).AppendFormat( "[%d]", c ) );
      GetPixel( p, *r.Image(), x, y, c, r.Interpolators() );
   }
   else if ( !r.Image()->IsColor() )
      GetPixel( p, *r.Image(), x, y, 0, r.Interpolators() );
   else
      GetPixel( p, *r.Image(), x, y, r.Interpolators() );
}

// ----------------------------------------------------------------------------

#define F   (static_cast<const Functional*>( *i ))
#define I   (static_cast<const ImageReference*>( *i ))
#define V   (static_cast<const VariableReference*>( *i ))
#define S   (static_cast<const Sample*>( *i ))
#define X   (static_cast<const PixelData*>( *i ))
#define B   (static_cast<const Branch*>( *i ))

class PixelMathEngine
{
public:

   static void Apply( ImageVariant& image, bool singleThreaded,
                      int posX, int posY, int fullWidth, int fullHeight,
                      const PixelMathInstance::rpn_set& rpn, int ch )
   {
      if ( image.IsFloatSample() ) // the working image is always a floating point image
         switch ( image.BitsPerSample() )
         {
         case 32: Apply( static_cast<Image&>( *image ), singleThreaded, posX, posY, fullWidth, fullHeight, rpn, ch ); break;
         case 64: Apply( static_cast<DImage&>( *image ), singleThreaded, posX, posY, fullWidth, fullHeight, rpn, ch ); break;
         }
   }

   template <class P>
   static void Apply( GenericImage<P>& image, bool singleThreaded,
                      int posX, int posY, int fullWidth, int fullHeight, const PixelMathInstance::rpn_set& rpn, int ch )
   {
      Array<size_type> L = Thread::OptimalThreadLoads( image.Height(), 1/*overheadLimit*/, singleThreaded ? 1 : PCL_MAX_PROCESSORS );
      ReferenceArray<PixelMathThread<P> > threads;
      ThreadData data( image, posX, posY, fullWidth, fullHeight, rpn, ch );
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads << new PixelMathThread<P>( i, data, image, n, n + int( L[i] ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      if ( data.error )
         throw Error( data.errorMessage );

      image.Status() = data.status;
   }

   static void Apply( StatusMonitor& status, bool singleThreaded,
                      int posX, int posY, int width, int height, int fullWidth, int fullHeight,
                      const PixelMathInstance::rpn_set& rpn, int ch )
   {
      Array<size_type> L = Thread::OptimalThreadLoads( height, 1/*overheadLimit*/, singleThreaded ? 1 : PCL_MAX_PROCESSORS );
      ThreadData data( status, posX, posY, width, height, fullWidth, fullHeight, rpn, ch );
      Image empty;
      ReferenceArray<PixelMathThread<FloatPixelTraits> > threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads << new PixelMathThread<FloatPixelTraits>( i, data, empty, n, n + int( L[i] ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      if ( data.error )
         throw Error( data.errorMessage );

      status = data.status;
   }

private:

   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( const AbstractImage& image,
                  int posX, int posY, int fullWidth, int fullHeight,
                  const PixelMathInstance::rpn_set& rpn, int ch )
         : AbstractImage::ThreadData( image, image.NumberOfPixels() )
         , x0( posX ), y0( posY )
         , w0( fullWidth ), h0( fullHeight )
         , wt( image.Width() ), ht( image.Height() )
         , channel( ch )
         , RPN( rpn )
      {
      }

      ThreadData( StatusMonitor& status,
                  int posX, int posY, int width, int height, int fullWidth, int fullHeight,
                  const PixelMathInstance::rpn_set& rpn, int ch )
         : AbstractImage::ThreadData( status, size_type( width )*size_type( height ) )
         , x0( posX ), y0( posY )
         , w0( fullWidth ), h0( fullHeight )
         , wt( width ), ht( height )
         , channel( ch )
         , RPN( rpn )
      {
      }

      int x0, y0;
      int w0, h0;
      int wt, ht;
      int channel;
      const PixelMathInstance::rpn_set& RPN;

      mutable bool error = false;
      mutable String errorMessage;
   };

   template <class P>
   class PixelMathThread : public Thread
   {
   public:

      PixelMathThread( int id, const ThreadData& data, GenericImage<P>& image, int firstRow, int endRow )
         : m_id( id )
         , m_data( data )
         , m_image( image )
         , m_firstRow( firstRow ), m_endRow( endRow )
      {
      }

      const String& ErrorMessage() const
      {
         return m_errorMessage;
      }

      void Run() override
      {
         INIT_THREAD_MONITOR()

         try
         {
            pixel_set stack;

            size_type stackLen = 0;
            for ( const Expression::component_list& l : m_data.RPN )
               if ( l.Length() > stackLen )
                  stackLen = l.Length();
            stack.Add( Pixel(), stackLen );

            pixel_set::iterator tos = stack.Begin();

            bool generate = !m_image.IsEmpty();

            typename P::sample* f0 = nullptr, * f1 = nullptr, * f2 = nullptr;
            if ( generate )
               if ( m_data.channel >= 0 )
                  f0 = m_image.ScanLine( m_firstRow, m_data.channel );
               else // single expression
               {
                  f0 = m_image.ScanLine( m_firstRow, 0 );
                  if ( m_image.IsColor() )
                  {
                     f1 = m_image.ScanLine( m_firstRow, 1 );
                     f2 = m_image.ScanLine( m_firstRow, 2 );
                  }
               }

            /*
             * Initialize thread-local data
             */
            PixelTLS tls;

            /*
             * For each pixel row
             */
            for ( int y = m_firstRow, yPos = m_data.y0 + m_firstRow, ch = Max( 0, m_data.channel ); y < m_endRow; ++y, ++yPos )
            {
               /*
                * For each pixel
                */
               for ( int x = 0, xPos = m_data.x0; x < m_data.wt; ++x, ++xPos )
               {
                  /*
                   * For each expression
                   */
                  for ( const Expression::component_list& rpn : m_data.RPN )
                  {
                     // Current stack pointer
                     pixel_set::iterator sp = tos;

                     int rgbCount = 0;
                     bool stackPushed = false;

                     /*
                      * For each expression component
                      */
                     for ( Expression::component_list::const_iterator i = rpn.Begin(); i != rpn.End(); ++i )
                     {
                        if ( (*i)->IsData() )
                        {
                           /*
                            * Data component
                            */
                           switch ( (*i)->Type() )
                           {
                           case XPR_IMAGEREF:
                              if ( I->ByReference() )
                                 sp->SetReference( *const_cast<ImageReference*>( I ) );
                              else
                              {
                                 if ( I->IsInterpolated() )
                                 {
                                    double fx = double( xPos )*I->Image()->Width()/m_data.w0;
                                    double fy = double( yPos )*I->Image()->Height()/m_data.h0;
                                    GetPixel( *sp, *I, fx, fy, m_data.channel );
                                 }
                                 else
                                    GetPixel( *sp, *I, xPos, yPos, m_data.channel );
                              }
                              break;

                           case XPR_VARREF:
                              {
                                 Pixel& v = V->Reference().ThreadLocalData( m_id );
                                 if ( V->HasLValueRole() )
                                    sp->SetReference( v, V->ChannelIndex() );
                                 else
                                 {
                                    if ( V->IsChannelIndex() )
                                       sp->SetSamples( v[V->ChannelIndex()] );
                                    else
                                       sp->AssignSamples( v );
                                 }
                              }
                              break;

                           case XPR_SAMPLE:
                              sp->SetSamples( S->Value() );
                              break;

                           case XPR_PIXEL:
                              *sp = X->PixelValue();
                              break;
                           }

                           if ( sp->IsRGB() )
                              ++rgbCount;

                           ++sp;
                           stackPushed = true;
                        }
                        else if ( (*i)->IsFunctional() )
                        {
                           /*
                            * Functional component
                            */
                           pixel_set::iterator s0 = sp - F->NumberOfArguments();

                           Pixel pixel( &tls, xPos, yPos );
                           if ( rgbCount != 0 )
                           {
                              for ( pixel_set::iterator s = s0; s != sp; ++s )
                                 if ( !s->IsRGB() )
                                    s->ConvertToRGB();
                           }
                           else
                              pixel.SetGrayscale();

                           (*F)( pixel, s0, sp );

                           rgbCount = pixel.IsRGB() ? 1 : 0;

                           *s0 = pixel;
                           sp = s0 + 1;
                           stackPushed = true;
                        }
                        else if ( (*i)->IsBranch() )
                        {
                           /*
                            * Flow control primitive
                            */
                           bool pop = false;
                           switch ( B->OpCode() )
                           {
                           case Branch::JMP:          // Unconditional branch
                              i += B->TargetOffset();
                              break;
                           case Branch::JZ:           // Branch if zero
                              if ( (sp-1)->IsLogicalZero( ch ) )
                                 i += B->TargetOffset();
                              else
                                 pop = true;
                              break;
                           case Branch::JZP:          // Branch if zero and pop
                              if ( (sp-1)->IsLogicalZero( ch ) )
                                 i += B->TargetOffset();
                              pop = true;
                              break;
                           case Branch::JNZ:          // Branch if nonzero
                              if ( (sp-1)->IsLogicalOne( ch ) )
                                 i += B->TargetOffset();
                              else
                                 pop = true;
                              break;
                           case Branch::JNZP:         // Branch if nonzero and pop
                              if ( (sp-1)->IsLogicalOne( ch ) )
                                 i += B->TargetOffset();
                              pop = true;
                              break;
                           default: // ?!
                              break;
                           }

                           if ( pop )
                              if ( stackPushed )
                              {
                                 --sp;
                                 stackPushed = false;
                              }
                        }
                     }
                  }

                  /*
                   * Result = TOS
                   */
                  if ( generate )
                  {
                     const Pixel& px = *tos;

                     if ( m_image.IsColor() )
                     {
                        if ( px.IsRGB() )
                        {
                           if ( m_data.channel < 0 ) // p_useSingleExpression?
                           {
                              *f0++ = P::ToSample( px[0] );
                              *f1++ = P::ToSample( px[1] );
                              *f2++ = P::ToSample( px[2] );
                           }
                           else
                              *f0++ = P::ToSample( px[m_data.channel] );
                        }
                        else
                        {
                           if ( m_data.channel < 0 ) // p_useSingleExpression?
                              *f0++ = *f1++ = *f2++ = P::ToSample( px[0] );
                           else
                              *f0++ = P::ToSample( px[0] );
                        }
                     }
                     else
                        *f0++ = P::ToSample( px[0] );
                  }

                  UPDATE_THREAD_MONITOR( 65536 )
               }
            }
         }
         catch ( ... )
         {
            volatile AutoLock lock( m_data.mutex );
            m_data.error = true;
            try
            {
               throw;
            }
            catch ( Exception& x )
            {
               m_data.errorMessage = x.Message();
            }
            catch ( std::bad_alloc& )
            {
               m_data.errorMessage = "Out of memory";
            }
            catch ( ... )
            {
               m_data.errorMessage = "Unknown error";
            }
         }
      }

   private:

            int              m_id;
      const ThreadData&      m_data;
            GenericImage<P>& m_image;
            int              m_firstRow;
            int              m_endRow;
            String           m_errorMessage;
   };
};

#undef F
#undef I
#undef V
#undef S
#undef X
#undef B
#undef T

// ----------------------------------------------------------------------------

static void Rescale( ImageVariant& image, double r0, double r1 )
{
   image->Status().Initialize( String().Format( "Rescaling to [%.15g,%.15g]", r0, r1 ), image->NumberOfSamples() );
   image->Status().DisableInitialization();
   image->ResetSelections();
   image.Rescale( r0, r1 );
}

// ----------------------------------------------------------------------------

static void Truncate( ImageVariant& image, double r0, double r1 )
{
   image->Status().Initialize( String().Format( "Truncating to [%.15g,%.15g]", r0, r1 ), image->NumberOfSamples() );
   image->Status().DisableInitialization();
   image->ResetSelections();
   image.Truncate( r0, r1 );
}

// ----------------------------------------------------------------------------

void PixelMathInstance::SolveInvariants( rpn_set RPN[], const ImageWindow& targetWindow )
{
   int invTotal = 0;

   for ( int c = 0; c < 4; ++c )
      for ( Expression::component_list& rpn : RPN[c] )
         for ( ;; )
         {
            int invFound = 0;

            for ( Expression::component_list::iterator i = rpn.Begin(); i != rpn.End(); ++i )
            {
               Functional* F = dynamic_cast<Functional*>( *i );
               if ( F != nullptr )
               {
                  int n = F->NumberOfArguments();

                  if ( F->IsInvariant( i-n, i ) )
                  {
                     ++invFound;

                     int pos = F->TokenPosition();

                     Pixel pixel( 0.0 );

                     for ( Expression::component_list::const_iterator j = i-n; j != i; ++j )
                     {
                        const PixelData* P = dynamic_cast<const PixelData*>( *j );
                        if ( P != nullptr )
                           if ( P->PixelValue().Length() != 1 )
                           {
                              pixel.SetAs( P->PixelValue() );
                              break;
                           }
                     }

                     (*F)( pixel, i-n, i );

                     rpn.Delete( i );
                     if ( pixel.Length() == 1 )
                        *i = new Sample( pixel[0], pos );
                     else
                        *i = new PixelData( pixel, pos );

                     rpn.Destroy( i-n, i );
                     break;
                  }
               }
               else
               {
                  ConstantReference* C = dynamic_cast<ConstantReference*>( *i );
                  if ( C != nullptr )
                  {
                     ++invFound;

                     int pos = C->TokenPosition();

                     Expression* x;

                     if ( C->Reference().IsValueFunction() )
                     {
                        x = C->Reference().FunctionValue( pos, targetWindow );
                     }
                     else
                     {
                        Pixel pixel = C->Reference();

                        if ( C->IsChannelIndex() )
                        {
                           if ( C->ChannelIndex() >= pixel.Length() )
                              throw ParseError( ("In constant definition: Channel index out of range: " + C->Id()).AppendFormat( "[%d]", C->ChannelIndex() ) );

                           x = new Sample( pixel[C->ChannelIndex()], pos );
                        }
                        else
                        {
                           if ( pixel.Length() == 1 )
                              x = new Sample( pixel[0], pos );
                           else
                              x = new PixelData( pixel, pos );
                        }
                     }

                     rpn.Delete( i );
                     *i = x;
                     break;
                  }
               }
            }

            if ( invFound == 0 )
               break;

            invTotal += invFound;
         }

   if ( invTotal > 0 )
      Console().WriteLn( "<end><cbr>Solved " + String( invTotal ) + " invariant subexpression(s)." );
}

// ----------------------------------------------------------------------------

struct PointerLocation
{
   unsigned      id;
   distance_type pos;

   bool operator <( const PointerLocation& p ) const
   {
      return id < p.id;
   }
};

void PixelMathInstance::SolveBranchTargets( rpn_set RPN[] )
{
   Array<PointerLocation> pointers;
   for ( int c = 0; c < 4; ++c )
      for ( const component_list& rpn : RPN[c] )
         for ( component_list::const_iterator i = rpn.Begin(); i < rpn.End(); ++i )
            if ( (*i)->IsPointer() )
               pointers << PointerLocation{ static_cast<const Pointer*>( *i )->Id(), i - rpn.Begin() };

   pointers.Sort();

   for ( int c = 0; c < 4; ++c )
      for ( component_list& rpn : RPN[c] )
         for ( component_list::iterator i = rpn.Begin(); i < rpn.End(); ++i )
            if ( (*i)->IsBranch() )
            {
               unsigned id = static_cast<const Branch*>( *i )->TargetId();
               Array<PointerLocation>::const_iterator p = BinarySearch( pointers.Begin(), pointers.End(), PointerLocation{ id, 0 } );
               if ( p == pointers.End() )
                  throw Error( "PixelMathInstance::SolveBranchTargets(): Internal error." );
               static_cast<Branch*>( *i )->SetTargetOffset( p->pos - (i - rpn.Begin()) );
            }
}

// ----------------------------------------------------------------------------

static
void RunImageGenerators( PixelMathInstance::rpn_set RPN[], AutoPointer<BicubicPixelInterpolation>& interpolation )
{
   int genTotal = 0;

   for ( int c = 0; c < 4; ++c )
      for ( Expression::component_list& rpn : RPN[c] )
         for ( ;; )
         {
            int genFound = 0;

            for ( Expression::component_list::iterator i = rpn.Begin(); i != rpn.End(); ++i )
               if ( (*i)->IsFunction() )
               {
                  const Function* F = dynamic_cast<Function*>( *i );
                  if ( F != nullptr )
                     if ( F->IsImageGenerator() )
                     {
                        ++genFound;

                        int n = F->NumberOfArguments();
                        int pos = F->TokenPosition();

                        IsoString key = F->GenerateImage( i-n, i );
                        InternalImageReference* ref = new InternalImageReference( key, pos );

                        rpn.Delete( i );
                        *i = ref;
                        rpn.Destroy( i-n, i );

                        if ( !ref->FindImage() )
                           throw ParseError( "Internal parser error: Invalid internal image reference." );

                        if ( ref->Image()->Width() != PixelMathInstance::TargetWidth() || ref->Image()->Height() != PixelMathInstance::TargetHeight() )
                        {
                           if ( interpolation.IsNull() )
                              interpolation = new BicubicPixelInterpolation;
                           ref->InitInterpolators( interpolation );
                        }

                        break;
                     }
               }

            if ( genFound == 0 )
               break;

            genTotal += genFound;
         }

   if ( genTotal > 0 )
      Console().WriteLn( "<end><cbr>Executed " + String( genTotal ) + " image generator(s), "
                        + File::SizeAsString( TheImageCache->TotalImageSize() ) );
}

// ----------------------------------------------------------------------------

static
void ReportGlobalVariables()
{
   String globals = Symbol::GlobalVariablesReport();
   if ( !globals.IsEmpty() )
      Console().WriteLn( "<end><cbr><br>* Global variables:<br>" + globals );
}

static
void ReportCachedImages()
{
   if ( TheImageCache->NumberOfImages() > 0 )
      Console().NoteLn( "<end><cbr>* " + String( TheImageCache->NumberOfImages() ) + " generated image(s)"
                        ", cache size: " + File::SizeAsString( TheImageCache->TotalImageSize() ) );
}

static
void ClearImageCache()
{
   size_type count, size;
   if ( TheImageCache->ClearImages( count, size ) )
      Console().NoteLn( "<end><cbr>* " + String( count ) + " cached image(s) removed"
                        ", " + File::SizeAsString( size ) + " freed up." );
}

// ----------------------------------------------------------------------------

void PixelMathInstance::Execute( ImageVariant& image, int x0, int y0, int w0, int h0, const rpn_set RPN[] )
{
   for ( int i = 0; i < 4; ++i )
   {
      int c = i;

      switch ( c )
      {
      case 0:
         if ( p_useSingleExpression && image->IsColor() )
            c = -1;
         break;
      case 1:
      case 2:
         if ( p_useSingleExpression || !image->IsColor() )
            continue;
         break;
      case 3:
         if ( !image->HasAlphaChannels() )
            continue;
         c = image->NumberOfNominalChannels();
         break;
      }

      if ( !RPN[i].IsEmpty() )
      {
         String statusStr = "Executing PixelMath expression:";
         statusStr << "<br><raw>" << p_expression[i] << "</raw><br>";
         if ( c < 0 )
            statusStr << "combined RGB/K channels";
         else
            statusStr.AppendFormat( "channel #%d", c );

         image->Status().Initialize( statusStr, image->NumberOfPixels() );

         if ( p_generateOutput )
         {
            // The working image is always a floating point image.
            switch ( image.BitsPerSample() )
            {
            case 32: PixelMathEngine::Apply( static_cast<Image&>( *image ), p_singleThreaded, x0, y0, w0, h0, RPN[i], c ); break;
            case 64: PixelMathEngine::Apply( static_cast<DImage&>( *image ), p_singleThreaded, x0, y0, w0, h0, RPN[i], c ); break;
            }
         }
         else
         {
            PixelMathEngine::Apply( image->Status(), p_singleThreaded, x0, y0, image->Width(), image->Height(), w0, h0, RPN[i], c );
         }
      }
   }

   ReportGlobalVariables();

   o_outputData = Symbol::GlobalVariablesData();
}

// ----------------------------------------------------------------------------

void PixelMathInstance::Execute( int width, int height, int numberOfChannels, const rpn_set RPN[] )
{
   for ( int i = 0; i < 4; ++i )
   {
      int c = i;

      switch ( c )
      {
      case 0:
         if ( p_useSingleExpression && numberOfChannels >= 3 )
            c = -1;
         break;
      case 1:
      case 2:
         if ( p_useSingleExpression || numberOfChannels < 3 )
            continue;
         break;
      case 3:
         switch ( numberOfChannels )
         {
         case 2:
            c = 1;
            break;
         case 4:
            c = 3;
            break;
         default:
            continue;
         }
         break;
      }

      if ( !RPN[i].IsEmpty() )
      {
         String statusStr = "Executing PixelMath expression:";
         statusStr << "<br><raw>" << p_expression[i] << "</raw><br>";
         if ( c < 0 )
            statusStr << "combined RGB/K channels";
         else
            statusStr.AppendFormat( "channel #%d", c );

         StandardStatus status;
         StatusMonitor monitor;
         monitor.SetCallback( &status );
         monitor.Initialize( statusStr, size_type( width )*size_type( height ) );

         PixelMathEngine::Apply( monitor, p_singleThreaded, 0, 0, width, height, width, height, RPN[i], c );
      }
   }

   ReportGlobalVariables();

   o_outputData = Symbol::GlobalVariablesData();
}

// ----------------------------------------------------------------------------

bool PixelMathInstance::ExecuteOn( View& view )
{
   o_outputData.Clear();

   if ( !p_cacheGeneratedImages || p_clearImageCacheAndExit )
   {
      ClearImageCache();
      if ( p_clearImageCacheAndExit )
         return true;
   }

   Expression::component_list L[ 4 ];
   rpn_set RPN[ 4 ];

   AutoPointer<BicubicPixelInterpolation> interpolation;
   ImageVariant workingImage;
   ImageWindow destinationWindow;

   try
   {
      /*
       * Create symbols and global variables.
       */
      Symbol::Create( p_symbols );

      /*
       * Parse expressions.
       */
      Expression::Parse( L[0], p_expression[0] );
      if ( !p_useSingleExpression )
      {
         Expression::Parse( L[1], p_expression[1] );
         Expression::Parse( L[2], p_expression[2] );
      }
      Expression::Parse( L[3], p_expression[3] );

      if ( L[0].IsEmpty() && L[1].IsEmpty() && L[2].IsEmpty() && L[3].IsEmpty() )
         throw ParseError( "No applicable PixelMath expression has been defined." );

      /*
       * For now, lock the target view just for write operations.
       */
      AutoViewWriteLock lock( view );

      /*
       * Check and gather target image properties.
       */
      int x0, y0, w0, h0;
      String targetId;
      bool partialPreview;

      if ( view.IsPreview() )
      {
         if ( p_createNewImage )
            if ( p_newImageWidth > 0 || p_newImageHeight > 0 /*|| p_newImageColorSpace != PMNewImageColorSpace::SameAsTarget*/ )
               throw Error( "This PixelMath instance cannot be executed on previews." );

         ImageWindow window = view.Window();
         ImageVariant image = window.MainView().Image();

         Rect r = window.PreviewRect( view.Id() );
         x0 = r.x0;
         y0 = r.y0;
         w0 = image.Width();
         h0 = image.Height();
         targetId = window.MainView().Id();
         partialPreview = x0 != 0 || y0 != 0 || Rect( w0, h0 ) != view.Bounds();

         s_targetNumberOfChannels = image.NumberOfChannels();
         s_targetIsColor = image.IsColor();
      }
      else
      {
         x0 = y0 = 0;
         w0 = (p_createNewImage && p_newImageWidth > 0) ? p_newImageWidth : view.Width();
         h0 = (p_createNewImage && p_newImageHeight > 0) ? p_newImageHeight : view.Height();
         targetId = view.Id();
         partialPreview = false;

         s_targetNumberOfChannels = view.Image().NumberOfChannels();
         s_targetIsColor = view.IsColor();
      }

      s_targetWidth = w0;
      s_targetHeight = h0;

      if ( s_targetRGBWS == nullptr )
         s_targetRGBWS = new RGBColorSystem;

      view.Window().GetRGBWS( *s_targetRGBWS );

      /*
       * Currently, code optimizations are incompatible with execution of
       * combined RGB PixelMath expressions.
       */
      bool optimize = p_optimization;
      if ( optimize )
         if ( p_useSingleExpression )
            if ( p_generateOutput )
            {
               if ( p_createNewImage )
                  switch ( p_newImageColorSpace )
                  {
                  default:
                  case PMNewImageColorSpace::SameAsTarget: optimize = !view.Window().MainView().IsColor(); break;
                  case PMNewImageColorSpace::RGB:          optimize = false; break;
                  }
               else
                  optimize = !view.Window().MainView().IsColor();
            }
            else
               optimize = !view.Image().IsColor();

      bool isTargetReferenced = false;

      for ( int c = 0; c < 4; ++c )
         for ( const Expression* X : L[c] )
         {
            Expression::component_list rpn;

            try
            {
               /*
                * Generate a linear representation in Reverse Polish Notation
                * format, with optional code optimization (which can be
                * forcibly disabled by circumstances; see above).
                */
               const Functional* F = dynamic_cast<const Functional*>( X );
               if ( F != nullptr )
                  F->PostOrder( rpn, optimize );
               else
                  rpn << X->Clone();

//                std::cout << "\n----------------\n";
//                for ( const Expression* x : rpn )
//                   std::cout << IsoString( x->ToString() ) << '\n';
//                std::cout << "\n----------------\n" << std::flush;

               /*
                * Resolve metasymbols and image references.
                */
               for ( Expression::component_list::iterator i = rpn.Begin(); i != rpn.End(); ++i )
               {
                  ImageReference* I = nullptr;

                  const MetaSymbol* M = dynamic_cast<MetaSymbol*>( *i );
                  if ( M != nullptr )
                  {
                     if ( M->Id().StartsWith( 'T' ) )
                     {
                        String id = targetId + M->Id().Substring( 1 );
                        ImageWindow w = ImageWindow::WindowById( id );
                        if ( w.IsNull() )
                           throw ParseError( '\'' + id + "': No such image available for $T metasymbol", p_expression[c], M->TokenPosition() );
                        I = new ImageReference( id, M->TokenPosition() );
                     }
                     else if ( M->Id() == "target" )
                     {
                        I = new ImageReference( targetId, M->TokenPosition() );
                     }
                     else if ( M->Id() == "M" || M->Id() == "mask" )
                     {
                        const ImageWindow m = view.Window().Mask();
                        if ( m.IsNull() )
                           throw ParseError( "No image available for $M metasymbol", p_expression[c], M->TokenPosition() );
                        I = new ImageReference( m.MainView().Id(), M->TokenPosition() );
                     }
                     else if ( M->Id() == "A" || M->Id() == "active" )
                     {
                        const ImageWindow a = ImageWindow::ActiveWindow();
                        if ( a.IsNull() )
                           throw ParseError( "No image available for $A metasymbol", p_expression[c], M->TokenPosition() );
                        I = new ImageReference( a.MainView().Id(), M->TokenPosition() );
                     }
                     else
                     {
                        // ### TODO: More metasymbols (e.g. $query, user-defined variables, etc)
                        throw ParseError( "Unknown metasymbol identifier", p_expression[c], M->TokenPosition() );
                     }

                     I->SetChannelIndex( M->ChannelIndex() );
                     if ( M->ByReference() )
                        I->SetByReference();
                     if ( M->RequiresAstrometricSolution() )
                        I->RequireAstrometricSolution();
                     rpn.Delete( i );
                     *i = I;
                  }
                  else
                  {
                     I = dynamic_cast<ImageReference*>( *i );
                  }

                  if ( I != nullptr )
                  {
                     if ( !I->FindImage() )
                        throw ParseError( "No such image", p_expression[c], I->TokenPosition() );

                     if ( I->Id() == targetId )
                        isTargetReferenced = true;

                     if ( I->Image()->Width() != w0 || I->Image()->Height() != h0 )
                     {
                        if ( interpolation.IsNull() )
                           interpolation = new BicubicPixelInterpolation;
                        I->InitInterpolators( interpolation );
                     }

                     if ( I->RequiresAstrometricSolution() )
                        if ( !I->HasAstrometricSolution() )
                           throw ParseError( "The image has no valid astrometric solution", p_expression[c], I->TokenPosition() );
                  }
               }

               RPN[c] << rpn;
               rpn.Clear();
            }
            catch ( ... )
            {
               rpn.Destroy();
               throw;
            }
         }

      /*
       * Solve invariant subexpressions.
       */
      SolveInvariants( RPN, view.Window() );

      /*
       * Execute image generators. Each generator replaces its entire
       * expression (function and arguments) with an internal image reference.
       */
      RunImageGenerators( RPN, interpolation );

      /*
       * If we are optimizing, compute branch target offsets.
       */
      if ( optimize )
         SolveBranchTargets( RPN );

      /*
       * Generate an output image or use the target view, as appropriate
       * depending on instance parameters.
       *
       * If createNewImage is enabled and we are executing on a preview, we use
       * the preview for output instead of creating a new image window. This is
       * a very useful although somewhat counterintuitive feature, which some
       * users interpret as a bug - alas, this is PixInsight, so complexity is
       * always welcome.
       *
       * If use64BitWorkingImage is enabled but the target image provides an
       * inferior sample format, we cannot use it so we'll have to use a
       * temporary working image, which we'll create after this block.
       *
       * If we are not generating output pixels, always use the target view
       * directly, even if it is a preview.
       */
      if ( p_generateOutput )
      {
         if ( p_createNewImage && !view.IsPreview() )
         {
            int bitsPerSample;
            bool floatSample;
            switch ( p_newImageSampleFormat )
            {
            default:
            case PMNewImageSampleFormat::SameAsTarget: view.Window().GetSampleFormat( bitsPerSample, floatSample ); break;
            case PMNewImageSampleFormat::I8:  bitsPerSample =  8; floatSample = false; break;
            case PMNewImageSampleFormat::I16: bitsPerSample = 16; floatSample = false; break;
            case PMNewImageSampleFormat::I32: bitsPerSample = 32; floatSample = false; break;
            case PMNewImageSampleFormat::F32: bitsPerSample = 32; floatSample = true; break;
            case PMNewImageSampleFormat::F64: bitsPerSample = 64; floatSample = true; break;
            }

            bool color;
            switch ( p_newImageColorSpace )
            {
            default:
            case PMNewImageColorSpace::SameAsTarget: color = view.Window().MainView().IsColor(); break;
            case PMNewImageColorSpace::RGB:          color = true; break;
            case PMNewImageColorSpace::Gray:         color = false; break;
            }

            destinationWindow = ImageWindow( w0, h0, (color ? 3 : 1) + (p_newImageAlpha ? 1 : 0),
                                             bitsPerSample, floatSample, color, true, p_newImageId );
            if ( destinationWindow.IsNull() )
               throw Error( "PixelMath: Unable to create destination image window." );

            destinationWindow.MainView().Lock();

            RGBColorSystem rgbws;
            view.Window().GetRGBWS( rgbws );
            destinationWindow.SetRGBWS( rgbws );

            double xRes, yRes;
            bool metric;
            view.Window().GetResolution( xRes, yRes, metric );
            destinationWindow.SetResolution( xRes, yRes, metric );

            if ( floatSample )
               if ( bitsPerSample == 64 || !p_use64BitWorkingImage )
                  workingImage = destinationWindow.MainView().Image();
         }
         else
         {
            if ( !isTargetReferenced )
            {
               int bitsPerSample;
               bool floatSample;
               view.Window().GetSampleFormat( bitsPerSample, floatSample );

               if ( floatSample )
                  if ( bitsPerSample == 64 || !p_use64BitWorkingImage )
                     if ( !p_createNewImage || bool( p_newImageAlpha ) == view.Image().HasAlphaChannels() )
                     {
                        lock.Lock();
                        workingImage = view.Image();
                     }
            }
         }
      }
      else
      {
         workingImage = view.Image();
      }

      /*
       * Generate a temporary local working image if we cannot work on an
       * existing view (either the target view or a newly created one), for the
       * reasons explained above.
       */
      bool isLocalWorkingImage = !workingImage;
      if ( isLocalWorkingImage )
      {
         workingImage.CreateFloatImage( p_use64BitWorkingImage ? 64 : 32 );

         int w, h;
         if ( view.IsPreview() )
         {
            w = view.Width();
            h = view.Height();
         }
         else
         {
            w = w0;
            h = h0;
         }

         bool color;
         if ( p_createNewImage )
            switch ( p_newImageColorSpace )
            {
            default:
            case PMNewImageColorSpace::SameAsTarget: color = view.Window().MainView().IsColor(); break;
            case PMNewImageColorSpace::RGB:          color = true; break;
            case PMNewImageColorSpace::Gray:         color = false; break;
            }
         else
            color = view.Window().MainView().IsColor();

         int n = color ? 3 : 1;
         if ( p_createNewImage )
         {
            if ( p_newImageAlpha )
               ++n;
         }
         else
         {
            if ( view.Window().MainView().Image().HasAlphaChannels() )
               ++n;
         }

         workingImage.AllocateImage( w, h, n, color ? ColorSpace::RGB : ColorSpace::Gray );
         workingImage.Zero();
      }

      /*
       * Execute PixelMath expressions.
       */
      Console().EnableAbort();
      StandardStatus status;
      workingImage.SetStatusCallback( &status );

      Execute( workingImage, x0, y0, w0, h0, RPN );

      /*
       * Rescale or truncate the result.
       *
       * Note that we allow unscaled/unbounded results (by disabling both
       * rescaleResult and truncateResult), but this is not exposed to the user
       * on PixelMath's interface because platform stability is not guaranteed
       * if out-of-range images are propagated to the core GUI. This feature is
       * only intended for execution of PixelMath instances from scripts.
       */
      if ( p_generateOutput )
         if ( p_rescaleResult )
         {
            if ( partialPreview )
               if ( destinationWindow.IsNull() )
                  Console().WarningLn( "<end><cbr>** Warning: rescaling a partial preview can yield inaccurate results." );
            Rescale( workingImage, p_rescaleLower, p_rescaleUpper );
         }
         else if ( p_truncateResult )
            Truncate( workingImage, p_truncateLower, p_truncateUpper );

      /*
       * Clean up
       */
      for ( int c = 0; c < 4; ++c )
      {
         for ( Expression::component_list& l : RPN[c] )
            l.Destroy();
         L[c].Destroy();
      }

      Symbol::DestroyAll();

      if ( p_cacheGeneratedImages )
         ReportCachedImages();
      else
         TheImageCache->ClearImages();

      /*
       * If we have used a temporary working image, copy the result to the
       * target view.
       */
      if ( isLocalWorkingImage )
         if ( destinationWindow.IsNull() )
         {
            lock.Lock();
            view.Image().CopyImage( workingImage );
         }
         else
            destinationWindow.MainView().Image().CopyImage( workingImage );

      /*
       * Unlock and show (if requested) a newly created destination image.
       */
      if ( !destinationWindow.IsNull() )
      {
         destinationWindow.MainView().Unlock();
         if ( p_showNewImage )
            destinationWindow.Show();
      }

      return true;
   }
   catch ( ... )
   {
      for ( int c = 0; c < 4; ++c )
      {
         for ( Expression::component_list& l : RPN[c] )
            l.Destroy();
         L[c].Destroy();
      }

      Symbol::DestroyAll();

      if ( !p_cacheGeneratedImages )
         TheImageCache->ClearImages();

      workingImage.Free();

      if ( !destinationWindow.IsNull() )
      {
         destinationWindow.MainView().Unlock();
         destinationWindow.ForceClose();
      }

      try
      {
         throw;
      }
      catch ( ParseError& x )
      {
         x.Show();
         return false;
      }
      catch ( ... )
      {
         throw;
      }
   }
}

// ----------------------------------------------------------------------------

bool PixelMathInstance::ExecuteGlobal()
{
   o_outputData.Clear();

   if ( !p_cacheGeneratedImages || p_clearImageCacheAndExit )
   {
      ClearImageCache();
      if ( p_clearImageCacheAndExit )
         return true;
   }

   Expression::component_list L[ 4 ];
   rpn_set RPN[ 4 ];

   AutoPointer<BicubicPixelInterpolation> interpolation;

   ImageVariant workingImage;
   bool isLocalWorkingImage = false;

   ImageWindow destinationWindow;

   try
   {
      /*
       * Create symbols and global variables.
       */
      Symbol::Create( p_symbols );

      /*
       * Parse expressions.
       */
      Expression::Parse( L[0], p_expression[0] );
      if ( !p_useSingleExpression )
      {
         Expression::Parse( L[1], p_expression[1] );
         Expression::Parse( L[2], p_expression[2] );
      }
      Expression::Parse( L[3], p_expression[3] );

      if ( L[0].IsEmpty() && L[1].IsEmpty() && L[2].IsEmpty() && L[3].IsEmpty() )
         throw ParseError( "No applicable PixelMath expression has been defined." );

      /*
       * Check and gather image properties.
       */
      if ( p_generateOutput && !p_createNewImage ||
           p_newImageWidth <= 0 || p_newImageHeight <= 0 ||
           p_newImageColorSpace == PMNewImageColorSpace::SameAsTarget ||
           p_generateOutput && p_newImageSampleFormat == PMNewImageSampleFormat::SameAsTarget )
      {
         throw Error( "This PixelMath instance cannot be executed in the global context." );
      }

      s_targetWidth = p_newImageWidth;
      s_targetHeight = p_newImageHeight;
      s_targetIsColor = p_newImageColorSpace == PMNewImageColorSpace::RGB;
      s_targetNumberOfChannels = (s_targetIsColor ? 3 : 1) + (p_newImageAlpha ? 1 : 0);

      if ( s_targetRGBWS == nullptr )
         s_targetRGBWS = new RGBColorSystem;

      ImageWindow::GetGlobalRGBWS( *s_targetRGBWS );

      /*
       * Currently, code optimizations are incompatible with execution of
       * combined RGB PixelMath expressions.
       */
      bool optimize = p_optimization && !(s_targetIsColor && p_useSingleExpression);

      for ( int c = 0; c < 4; ++c )
         for ( Expression* X : L[c] )
         {
            Expression::component_list rpn;

            try
            {
               /*
                * Generate a linear representation in Reverse Polish Notation
                * format, with optional code optimization (which can be
                * forcibly disabled by circumstances; see above).
                */
               const Functional* F = dynamic_cast<const Functional*>( X );
               if ( F != nullptr )
                  F->PostOrder( rpn, optimize );
               else
                  rpn << X->Clone();

               /*
                * Resolve metasymbols and image references.
                */
               for ( Expression::component_list::iterator i = rpn.Begin(); i != rpn.End(); ++i )
               {
                  ImageReference* I = nullptr;

                  MetaSymbol* M = dynamic_cast<MetaSymbol*>( *i );
                  if ( M != nullptr )
                  {
                     if ( M->Id().StartsWith( 'T' ) || M->Id() == "target" )
                        throw ParseError( "Illegal $T metasymbol for global PixelMath execution", p_expression[c], M->TokenPosition() );
                     else if ( M->Id() == "M" || M->Id() == "mask" )
                        throw ParseError( "Illegal $M metasymbol for global PixelMath execution", p_expression[c], M->TokenPosition() );
                     else if ( M->Id() == "A" || M->Id() == "active" )
                     {
                        const ImageWindow a = ImageWindow::ActiveWindow();
                        if ( a.IsNull() )
                           throw ParseError( "No image available for $A metasymbol", p_expression[c], M->TokenPosition() );
                        I = new ImageReference( a.MainView().Id(), M->TokenPosition() );
                     }
                     else
                     {
                        // ### TODO: More metasymbols (e.g. $query, user-defined variables, etc)
                        throw ParseError( "Unknown metasymbol identifier", p_expression[c], M->TokenPosition() );
                     }

                     I->SetChannelIndex( M->ChannelIndex() );
                     if ( M->ByReference() )
                        I->SetByReference();
                     if ( M->RequiresAstrometricSolution() )
                        I->RequireAstrometricSolution();
                     rpn.Delete( i );
                     *i = I;
                  }
                  else
                  {
                     I = dynamic_cast<ImageReference*>( *i );
                  }

                  if ( I != nullptr )
                  {
                     if ( !I->FindImage() )
                        throw ParseError( "No such image", p_expression[c], I->TokenPosition() );

                     if ( I->Image()->Width() != p_newImageWidth || I->Image()->Height() != p_newImageHeight )
                     {
                        if ( interpolation.IsNull() )
                           interpolation = new BicubicPixelInterpolation;
                        I->InitInterpolators( interpolation );
                     }

                     if ( I->RequiresAstrometricSolution() )
                        if ( !I->HasAstrometricSolution() )
                           throw ParseError( "The image has no valid astrometric solution", p_expression[c], I->TokenPosition() );
                  }
               }

               RPN[c] << rpn;
               rpn.Clear();
            }
            catch ( ... )
            {
               rpn.Destroy();
               throw;
            }
         }

      /*
       * Solve invariant subexpressions.
       */
      SolveInvariants( RPN );

      /*
       * Execute image generators. Each generator replaces its entire
       * expression (function and arguments) with an internal image reference.
       */
      RunImageGenerators( RPN, interpolation );

      /*
       * If we are optimizing, compute branch target offsets.
       */
      if ( optimize )
         SolveBranchTargets( RPN );

      /*
       * Generate an output image.
       *
       * If use64BitWorkingImage is enabled but the output image uses an
       * inferior sample format, we cannot use it directly so we'll have to use
       * a temporary working image.
       *
       * If we are not generating output pixels, we don't need any image for
       * global execution.
       */
      if ( p_generateOutput )
      {
         int bitsPerSample;
         bool floatSample;
         switch ( p_newImageSampleFormat )
         {
         case PMNewImageSampleFormat::I8 : bitsPerSample =  8; floatSample = false; break;
         case PMNewImageSampleFormat::I16: bitsPerSample = 16; floatSample = false; break;
         case PMNewImageSampleFormat::I32: bitsPerSample = 32; floatSample = false; break;
         default:
         case PMNewImageSampleFormat::F32: bitsPerSample = 32; floatSample = true; break;
         case PMNewImageSampleFormat::F64: bitsPerSample = 64; floatSample = true; break;
         }

         bool color;
         switch ( p_newImageColorSpace )
         {
         default:
         case PMNewImageColorSpace::RGB : color = true; break;
         case PMNewImageColorSpace::Gray: color = false; break;
         }

         int numberOfChannels = (color ? 3 : 1) + (p_newImageAlpha ? 1 : 0);

         destinationWindow = ImageWindow( p_newImageWidth, p_newImageHeight, numberOfChannels,
                                          bitsPerSample, floatSample, color, true, p_newImageId );
         if ( destinationWindow.IsNull() )
            throw Error( "PixelMath: Unable to create destination image window." );

         destinationWindow.MainView().Lock();

         if ( floatSample )
            if ( bitsPerSample == 64 || !p_use64BitWorkingImage )
               workingImage = destinationWindow.MainView().Image();

         isLocalWorkingImage = !workingImage;
         if ( isLocalWorkingImage )
         {
            workingImage.CreateFloatImage( p_use64BitWorkingImage ? 64 : 32 );
            workingImage.AllocateImage( p_newImageWidth, p_newImageHeight, numberOfChannels, color ? ColorSpace::RGB : ColorSpace::Gray );
            workingImage.Zero();
         }

         Console().EnableAbort();

         StandardStatus status;
         workingImage.SetStatusCallback( &status );

         /*
          * Execute PixelMath expressions on the working image.
          */
         Execute( workingImage, 0, 0, p_newImageWidth, p_newImageHeight, RPN );

         if ( p_rescaleResult )
            Rescale( workingImage, p_rescaleLower, p_rescaleUpper );
         else if ( p_truncateResult )
            Truncate( workingImage, p_truncateLower, p_truncateUpper );
      }
      else
      {
         /*
          * Execute PixelMath expressions without an output image.
          */
         Execute( p_newImageWidth, p_newImageHeight, s_targetNumberOfChannels, RPN );
      }

      /*
       * Clean up
       */
      for ( int c = 0; c < 4; ++c )
      {
         for ( Expression::component_list& l : RPN[c] )
            l.Destroy();
         L[c].Destroy();
      }

      Symbol::DestroyAll();

      if ( p_cacheGeneratedImages )
         ReportCachedImages();
      else
         TheImageCache->ClearImages();

      /*
       * If we have used a temporary working image, copy the result to the
       * target view.
       */
      if ( p_generateOutput )
      {
         if ( isLocalWorkingImage )
            destinationWindow.MainView().Image().CopyImage( workingImage );

         /*
          * Unlock and show (if requested) the output image.
          */
         destinationWindow.MainView().Unlock();
         if ( p_showNewImage )
            destinationWindow.Show();
      }

      return true;
   }
   catch ( ... )
   {
      for ( int c = 0; c < 4; ++c )
      {
         for ( Expression::component_list& l : RPN[c] )
            l.Destroy();
         L[c].Destroy();
      }

      Symbol::DestroyAll();

      if ( !p_cacheGeneratedImages )
         TheImageCache->ClearImages();

      workingImage.Free();

      if ( !destinationWindow.IsNull() )
      {
         destinationWindow.MainView().Unlock();
         destinationWindow.ForceClose();
      }

      try
      {
         throw;
      }
      catch ( ParseError& x )
      {
         x.Show();
         return false;
      }
      catch ( ... )
      {
         throw;
      }
   }
}

// ----------------------------------------------------------------------------

void* PixelMathInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == ThePMExpression0Parameter )
      return p_expression[0].Begin();
   if ( p == ThePMExpression1Parameter )
      return p_expression[1].Begin();
   if ( p == ThePMExpression2Parameter )
      return p_expression[2].Begin();
   if ( p == ThePMExpression3Parameter )
      return p_expression[3].Begin();
   if ( p == ThePMUseSingleExpressionParameter )
      return &p_useSingleExpression;
   if ( p == ThePMSymbolsParameter )
      return p_symbols.Begin();
   if ( p == ThePMClearImageCacheAndExitParameter )
      return &p_clearImageCacheAndExit;
   if ( p == ThePMCacheGeneratedImagesParameter )
      return &p_cacheGeneratedImages;
   if ( p == ThePMGenerateOutputParameter )
      return &p_generateOutput;
   if ( p == ThePMSingleThreadedParameter )
      return &p_singleThreaded;
   if ( p == ThePMOptimizationParameter )
      return &p_optimization;
   if ( p == ThePMUse64BitWorkingImageParameter )
      return &p_use64BitWorkingImage;
   if ( p == ThePMRescaleResultParameter )
      return &p_rescaleResult;
   if ( p == ThePMRescaleLowerBoundParameter )
      return &p_rescaleLower;
   if ( p == ThePMRescaleUpperBoundParameter )
      return &p_rescaleUpper;
   if ( p == ThePMTruncateResultParameter )
      return &p_truncateResult;
   if ( p == ThePMTruncateLowerBoundParameter )
      return &p_truncateLower;
   if ( p == ThePMTruncateUpperBoundParameter )
      return &p_truncateUpper;
   if ( p == ThePMCreateNewImageParameter )
      return &p_createNewImage;
   if ( p == ThePMShowNewImageParameter )
      return &p_showNewImage;
   if ( p == ThePMNewImageIdentifierParameter )
      return p_newImageId.Begin();
   if ( p == ThePMNewImageWidthParameter )
      return &p_newImageWidth;
   if ( p == ThePMNewImageHeightParameter )
      return &p_newImageHeight;
   if ( p == ThePMNewImageAlphaParameter )
      return &p_newImageAlpha;
   if ( p == ThePMNewImageColorSpaceParameter )
      return &p_newImageColorSpace;
   if ( p == ThePMNewImageSampleFormatParameter )
      return &p_newImageSampleFormat;
   if ( p == ThePMOutputGlobalVariableIdParameter )
      return o_outputData[tableRow].id.Begin();
   if ( p == ThePMOutputGlobalVariableRKParameter )
      return o_outputData[tableRow].value.At( 0 );
   if ( p == ThePMOutputGlobalVariableGParameter )
      return o_outputData[tableRow].value.At( 1 );
   if ( p == ThePMOutputGlobalVariableBParameter )
      return o_outputData[tableRow].value.At( 2 );

   return nullptr;
}

// ----------------------------------------------------------------------------

bool PixelMathInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == ThePMExpression0Parameter )
   {
      p_expression[0].Clear();
      if ( sizeOrLength > 0 )
         p_expression[0].SetLength( sizeOrLength );
   }
   else if ( p == ThePMExpression1Parameter )
   {
      p_expression[1].Clear();
      if ( sizeOrLength > 0 )
         p_expression[1].SetLength( sizeOrLength );
   }
   else if ( p == ThePMExpression2Parameter )
   {
      p_expression[2].Clear();
      if ( sizeOrLength > 0 )
         p_expression[2].SetLength( sizeOrLength );
   }
   else if ( p == ThePMExpression3Parameter )
   {
      p_expression[3].Clear();
      if ( sizeOrLength > 0 )
         p_expression[3].SetLength( sizeOrLength );
   }
   else if ( p == ThePMSymbolsParameter )
   {
      p_symbols.Clear();
      if ( sizeOrLength > 0 )
         p_symbols.SetLength( sizeOrLength );
   }
   else if ( p == ThePMNewImageIdentifierParameter )
   {
      p_newImageId.Clear();
      if ( sizeOrLength > 0 )
         p_newImageId.SetLength( sizeOrLength );
   }
   else if ( p == ThePMOutputDataParameter )
   {
      o_outputData.Clear();
      if ( sizeOrLength > 0 )
         o_outputData.Append( Symbol::OutputData(), sizeOrLength );
   }
   else if ( p == ThePMOutputGlobalVariableIdParameter )
   {
      o_outputData[tableRow].id.Clear();
      if ( sizeOrLength > 0 )
         o_outputData[tableRow].id.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type PixelMathInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == ThePMExpression0Parameter )
      return p_expression[0].Length();
   if ( p == ThePMExpression1Parameter )
      return p_expression[1].Length();
   if ( p == ThePMExpression2Parameter )
      return p_expression[2].Length();
   if ( p == ThePMExpression3Parameter )
      return p_expression[3].Length();
   if ( p == ThePMSymbolsParameter )
      return p_symbols.Length();
   if ( p == ThePMNewImageIdentifierParameter )
      return p_newImageId.Length();
   if ( p == ThePMOutputDataParameter )
      return o_outputData.Length();
   if ( p == ThePMOutputGlobalVariableIdParameter )
      return o_outputData[tableRow].id.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PixelMathInstance.cpp - Released 2021-05-05T15:38:07Z
