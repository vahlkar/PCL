//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.1
// ----------------------------------------------------------------------------
// ChannelCombinationInstance.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "ChannelCombinationInstance.h"
#include "ChannelCombinationParameters.h"
#include "ChannelCombinationProcess.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/IntegrationMetadata.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelCombinationInstance::ChannelCombinationInstance( const MetaProcess* P )
   : ProcessImplementation( P )
   , p_colorSpace( TheColorSpaceIdCombinationParameter->ElementValue( TheColorSpaceIdCombinationParameter->Default ) )
   , p_inheritAstrometricSolution( TheCCInheritAstrometricSolutionParameter )
{
   for ( int i = 0; i < 3; ++i )
   {
      p_channelEnabled[i] = true;
      p_channelId[i] = String( TheChannelIdCombinationParameter->DefaultValue() );
   }
}

// ----------------------------------------------------------------------------

ChannelCombinationInstance::ChannelCombinationInstance( const ChannelCombinationInstance& x )
   : ProcessImplementation( x )
   , p_colorSpace( x.p_colorSpace )
   , p_inheritAstrometricSolution( x.p_inheritAstrometricSolution )
{
   for ( int i = 0; i < 3; ++i )
   {
      p_channelEnabled[i] = x.p_channelEnabled[i];
      p_channelId[i] = x.p_channelId[i];
   }
}

// ----------------------------------------------------------------------------

bool ChannelCombinationInstance::Validate( pcl::String& info )
{
   int n = 0;
   for ( int i = 0; i < 3; ++i )
   {
      if ( !p_channelId[i].IsEmpty() && !p_channelId[i].IsValidIdentifier() )
      {
         info = "Invalid image identifier: " + p_channelId[i];
         return false;
      }

      if ( p_channelEnabled[i] )
         ++n;
   }

   if ( n == 0 )
   {
      info = "No channel selected for combination";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

void ChannelCombinationInstance::Assign( const ProcessImplementation& p )
{
   const ChannelCombinationInstance* x = dynamic_cast<const ChannelCombinationInstance*>( &p );
   if ( x != nullptr )
   {
      p_colorSpace = x->p_colorSpace;

      for ( int i = 0; i < 3; ++i )
      {
         p_channelEnabled[i] = x->p_channelEnabled[i];
         p_channelId[i] = x->p_channelId[i];
      }

      p_inheritAstrometricSolution = x->p_inheritAstrometricSolution;
   }
}

// ----------------------------------------------------------------------------

UndoFlags ChannelCombinationInstance::UndoMode( const View& view ) const
{
   UndoFlags flags = UndoFlag::PixelData;
   if ( !view.IsPreview() )
   {
      flags |= UndoFlag::Keywords;
      if ( p_inheritAstrometricSolution )
         flags |= UndoFlag::AstrometricSolution;
   }
   return flags;
}

// ----------------------------------------------------------------------------

bool ChannelCombinationInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.Image().IsComplexSample() )
   {
      whyNot = "ChannelCombination cannot be executed on complex images.";
      return false;
   }

   if ( v.Image()->ColorSpace() != ColorSpace::RGB )
   {
      whyNot = "ChannelCombination can only be executed on RGB color images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

#ifdef __GNUC__
__attribute__((noinline))
#endif
static void FromRGB( int colorSpace,
                     const RGBColorSystem& rgbws,
                     RGBColorSystem::sample& ch0, RGBColorSystem::sample& ch1, RGBColorSystem::sample& ch2,
                     RGBColorSystem::sample r, RGBColorSystem::sample g, RGBColorSystem::sample b )
{
   switch ( colorSpace )
   {
   case ColorSpaceId::CIEXYZ:
      rgbws.RGBToCIEXYZ( ch0, ch1, ch2, r, g, b );
      break;
   case ColorSpaceId::CIELab:
      rgbws.RGBToCIELab( ch0, ch1, ch2, r, g, b );
      break;
   case ColorSpaceId::CIELch:
      rgbws.RGBToCIELch( ch0, ch1, ch2, r, g, b );
      break;
   case ColorSpaceId::HSV:
      rgbws.RGBToHSV( ch0, ch1, ch2, r, g, b );
      break;
   case ColorSpaceId::HSI:
      rgbws.RGBToHSI( ch0, ch1, ch2, r, g, b );
      break;
   }
}

// ----------------------------------------------------------------------------

#ifdef __GNUC__
__attribute__((noinline))
#endif
static void ToRGB( int colorSpace,
                   const RGBColorSystem& rgbws,
                   RGBColorSystem::sample& r, RGBColorSystem::sample& g, RGBColorSystem::sample& b,
                   RGBColorSystem::sample ch0, RGBColorSystem::sample ch1, RGBColorSystem::sample ch2 )
{
   switch ( colorSpace )
   {
   case ColorSpaceId::CIEXYZ:
      rgbws.CIEXYZToRGB( r, g, b, ch0, ch1, ch2 );
      break;
   case ColorSpaceId::CIELab:
      rgbws.CIELabToRGB( r, g, b, ch0, ch1, ch2 );
      break;
   case ColorSpaceId::CIELch:
      rgbws.CIELchToRGB( r, g, b, ch0, ch1, ch2 );
      break;
   case ColorSpaceId::HSV:
      rgbws.HSVToRGB( r, g, b, ch0, ch1, ch2 );
      break;
   case ColorSpaceId::HSI:
      rgbws.HSIToRGB( r, g, b, ch0, ch1, ch2 );
      break;
   }
}

// ----------------------------------------------------------------------------

template <class P, class P0, class P1, class P2>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineChannels( GenericImage<P>& img, int colorSpace, const String& baseId,
                             const Rect& r,
                             const GenericImage<P0>* src0, const GenericImage<P1>* src1, const GenericImage<P2>* src2 )
{
   bool allChannels = src0 != 0 && src1 != 0 && src2 != 0;

   typename P::sample* R = img.PixelData( 0 );
   typename P::sample* G = img.PixelData( 1 );
   typename P::sample* B = img.PixelData( 2 );

   const RGBColorSystem& rgbws = img.RGBWorkingSpace();

   for ( int y = r.y0; y < r.y1; ++y )
   {
      const typename P0::sample* data0 = (src0 != 0) ? src0->PixelAddress( r.x0, y ) : 0;
      const typename P1::sample* data1 = (src1 != 0) ? src1->PixelAddress( r.x0, y ) : 0;
      const typename P2::sample* data2 = (src2 != 0) ? src2->PixelAddress( r.x0, y ) : 0;

      for ( int x = r.x0; x < r.x1; ++x, ++img.Status() )
      {
         if ( colorSpace == ColorSpaceId::RGB )
         {
            if ( data0 != 0 )
               P0::FromSample( *R++, *data0++ );
            if ( data1 != 0 )
               P1::FromSample( *G++, *data1++ );
            if ( data2 != 0 )
               P2::FromSample( *B++, *data2++ );
         }
         else
         {
            RGBColorSystem::sample ch0, ch1, ch2;
            RGBColorSystem::sample r, g, b;

            if ( !allChannels )
            {
               P::FromSample( r, *R );
               P::FromSample( g, *G );
               P::FromSample( b, *B );

               FromRGB( colorSpace, rgbws, ch0, ch1, ch2, r, g, b );
            }

            if ( data0 != 0 )
               P0::FromSample( ch0, *data0++ );
            if ( data1 != 0 )
               P1::FromSample( ch1, *data1++ );
            if ( data2 != 0 )
               P2::FromSample( ch2, *data2++ );

            ToRGB( colorSpace, rgbws, r, g, b, ch0, ch1, ch2 );

            *R++ = P::ToSample( r );
            *G++ = P::ToSample( g );
            *B++ = P::ToSample( b );
         }
      }
   }
}

template <class P, class P0, class P1>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineChannels( GenericImage<P>& img, int colorSpace, const String& baseId,
                             const Rect& r,
                             const GenericImage<P0>* src0, const GenericImage<P1>* src1, ImageVariant& src2 )
{
   if ( src2 )
   {
      if ( src2.IsFloatSample() )
         switch ( src2.BitsPerSample() )
         {
         case 32:
            CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<Image*>( src2.ImagePtr() ) );
            break;
         case 64:
            CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<DImage*>( src2.ImagePtr() ) );
            break;
         }
      else
         switch ( src2.BitsPerSample() )
         {
         case  8:
            CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<UInt8Image*>( src2.ImagePtr() ) );
            break;
         case 16:
            CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<UInt16Image*>( src2.ImagePtr() ) );
            break;
         case 32:
            CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<UInt32Image*>( src2.ImagePtr() ) );
            break;
         }
   }
   else
      CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<UInt8Image*>( nullptr ) );
}

template <class P, class P0>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineChannels( GenericImage<P>& img, int colorSpace, const String& baseId,
                             const Rect& r,
                             const GenericImage<P0>* src0, ImageVariant& src1, ImageVariant& src2 )
{
   if ( src1 )
   {
      if ( src1.IsFloatSample() )
         switch ( src1.BitsPerSample() )
         {
         case 32:
            CombineChannels( img, colorSpace, baseId, r, src0, static_cast<Image*>( src1.ImagePtr() ), src2 );
            break;
         case 64:
            CombineChannels( img, colorSpace, baseId, r, src0, static_cast<DImage*>( src1.ImagePtr() ), src2 );
            break;
         }
      else
         switch ( src1.BitsPerSample() )
         {
         case  8:
            CombineChannels( img, colorSpace, baseId, r, src0, static_cast<UInt8Image*>( src1.ImagePtr() ), src2 );
            break;
         case 16:
            CombineChannels( img, colorSpace, baseId, r, src0, static_cast<UInt16Image*>( src1.ImagePtr() ), src2 );
            break;
         case 32:
            CombineChannels( img, colorSpace, baseId, r, src0, static_cast<UInt32Image*>( src1.ImagePtr() ), src2 );
            break;
         }
   }
   else
      CombineChannels( img, colorSpace, baseId, r, src0, static_cast<UInt8Image*>( nullptr ), src2 );
}

template <class P>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineChannels( GenericImage<P>& img, int colorSpace, const String& baseId,
                             const Rect& r,
                             ImageVariant& src0, ImageVariant& src1, ImageVariant& src2 )
{
   if ( src0 )
   {
      if ( src0.IsFloatSample() )
         switch ( src0.BitsPerSample() )
         {
         case 32:
            CombineChannels( img, colorSpace, baseId, r, static_cast<Image*>( src0.ImagePtr() ), src1, src2 );
            break;
         case 64:
            CombineChannels( img, colorSpace, baseId, r, static_cast<DImage*>( src0.ImagePtr() ), src1, src2 );
            break;
         }
      else
         switch ( src0.BitsPerSample() )
         {
         case  8:
            CombineChannels( img, colorSpace, baseId, r, static_cast<UInt8Image*>( src0.ImagePtr() ), src1, src2 );
            break;
         case 16:
            CombineChannels( img, colorSpace, baseId, r, static_cast<UInt16Image*>( src0.ImagePtr() ), src1, src2 );
            break;
         case 32:
            CombineChannels( img, colorSpace, baseId, r, static_cast<UInt32Image*>( src0.ImagePtr() ), src1, src2 );
            break;
         }
   }
   else
      CombineChannels( img, colorSpace, baseId, r, static_cast<UInt8Image*>( nullptr ), src1, src2 );
}

// ----------------------------------------------------------------------------

bool ChannelCombinationInstance::ExecuteOn( View& view )
{
   ImageWindow sourceWindow[ 3 ];
   ImageVariant sourceImage[ 3 ];
   Array<IntegrationMetadata> sourceMetadata;

   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      throw Error( "ChannelCombination cannot be executed on complex images." );

   if ( image->ColorSpace() != ColorSpace::RGB )
      throw Error( "ChannelCombination requires a RGB color image." );

   Console().EnableAbort();

   StandardStatus status;
   image->SetStatusCallback( &status );

   ImageWindow window = view.Window();

   String baseId;
   Rect rect;
   int w0, h0;

   if ( view.IsPreview() )
   {
      View mainView = window.MainView();
      baseId = mainView.Id();
      rect = window.PreviewRect( view.Id() );
      mainView.GetSize( w0, h0 );
   }
   else
   {
      baseId = view.Id();
      rect = image->Bounds();
      w0 = rect.Width();
      h0 = rect.Height();
   }

   int numberOfSources = 0;

   for ( int i = 0; i < 3; ++i )
      if ( p_channelEnabled[i] )
      {
         String id = p_channelId[i];
         if ( id.IsEmpty() )
            id = baseId + '_' + ColorSpaceId::ChannelId( p_colorSpace, i );

         sourceWindow[i] = ImageWindow::WindowById( id );

         if ( sourceWindow[i].IsNull() )
            throw Error( "ChannelCombination: Source image not found: " + id );

         sourceImage[i] = sourceWindow[i].MainView().Image();

         if ( !sourceImage[i] )
            throw Error( "ChannelCombination: Invalid source image: " + id );

         if ( sourceImage[i]->IsColor() )
            throw Error( "ChannelCombination: Invalid source color space: " + id );

         if ( sourceImage[i]->Width() != w0 || sourceImage[i]->Height() != h0 )
            throw Error( "ChannelCombination: Incompatible source image dimensions: " + id );

         if ( !view.IsPreview() )
            sourceMetadata << IntegrationMetadata( sourceWindow[i].MainView().StorableProperties(),
                                                   sourceWindow[i].Keywords() );
         ++numberOfSources;
      }

   if ( numberOfSources == 0 )
      return false;

   const char* what = "";
   int asChannel = 0;
   switch ( p_colorSpace )
   {
   default: // ?!
   case ColorSpaceId::RGB:
      what = "RGB channels";
      asChannel = 1; // green
      break;
   case ColorSpaceId::CIEXYZ:
      what = "normalized CIE XYZ components";
      asChannel = 1; // Y
      break;
   case ColorSpaceId::CIELab:
      what = "normalized CIE L*a*b* components";
      asChannel = 0; // L*
      break;
   case ColorSpaceId::CIELch:
      what = "normalized CIE L*c*h* components";
      asChannel = 0; // L*
      break;
   case ColorSpaceId::HSV:
      what = "normalized HSV components";
      asChannel = 2; // V
      break;
   case ColorSpaceId::HSI:
      what = "normalized HSI components";
      asChannel = 2; // I
      break;
   }
   image->Status().Initialize( String( "Combining " ) + what, image->NumberOfPixels() );

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32:
         CombineChannels( static_cast<Image&>( *image ), p_colorSpace, baseId, rect,
                          sourceImage[0], sourceImage[1], sourceImage[2] );
         break;
      case 64:
         CombineChannels( static_cast<DImage&>( *image ), p_colorSpace, baseId, rect,
                          sourceImage[0], sourceImage[1], sourceImage[2] );
         break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8:
         CombineChannels( static_cast<UInt8Image&>( *image ), p_colorSpace, baseId, rect,
                          sourceImage[0], sourceImage[1], sourceImage[2] );
         break;
      case 16:
         CombineChannels( static_cast<UInt16Image&>( *image ), p_colorSpace, baseId, rect,
                          sourceImage[0], sourceImage[1], sourceImage[2] );
         break;
      case 32:
         CombineChannels( static_cast<UInt32Image&>( *image ), p_colorSpace, baseId, rect,
                          sourceImage[0], sourceImage[1], sourceImage[2] );
         break;
      }

   if ( !view.IsPreview() )
   {
      PropertyArray properties;
      FITSKeywordArray keywords = window.Keywords();

      IntegrationMetadata metadata = IntegrationMetadata::Summary( sourceMetadata );
      metadata.UpdatePropertiesAndKeywords( properties, keywords );

      view.SetStorablePermanentProperties( properties );
      window.SetKeywords( keywords );

      if ( p_inheritAstrometricSolution )
      {
         ImageWindow asSourceWindow = sourceWindow[asChannel];
         bool asFound = !asSourceWindow.IsNull() && asSourceWindow.HasAstrometricSolution();
         if ( !asFound )
            for ( int i = 0; i < 3; ++i )
               if ( !sourceWindow[i].IsNull() && sourceWindow[i].HasAstrometricSolution() )
               {
                  asSourceWindow = sourceWindow[i];
                  asFound = true;
                  break;
               }

         if ( asFound )
            if ( window.CopyAstrometricSolution( asSourceWindow ) )
            {
               window.MainView().SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:Information",
                                                                    "source=inherited,process=ChannelCombination" );
               Console().NoteLn( "<end><cbr>* Astrometric solution inherited: "
                                 + asSourceWindow.MainView().Id() + " => " + window.MainView().Id() );
            }
            else
            {
               Console().WarningLn( "<end><cbr>** Invalid astrometric solution ignored: "
                                    + asSourceWindow.MainView().Id() );
            }
      }
   }

   return true;
}

// ----------------------------------------------------------------------------

bool ChannelCombinationInstance::CanExecuteGlobal( String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ChannelCombinationInstance::ExecuteGlobal()
{
   ImageWindow sourceWindow[ 3 ];
   ImageVariant sourceImage[ 3 ];
   Array<IntegrationMetadata> sourceMetadata;

   int numberOfSources = 0;
   int width = 0, height = 0;
   bool floatSample = false;
   int bitsPerSample = 0;

   for ( int i = 0; i < 3; ++i )
      if ( p_channelEnabled[i] && !p_channelId[i].IsEmpty() )
      {
         sourceWindow[i] = ImageWindow::WindowById( p_channelId[i] );
         if ( sourceWindow[i].IsNull() )
            throw Error( "ChannelCombination: Source image not found: " + p_channelId[i] );

         sourceImage[i] = sourceWindow[i].MainView().Image();
         if ( !sourceImage[i] )
            throw Error( "ChannelCombination: Invalid source image: " + p_channelId[i] );

         if ( sourceImage[i]->IsColor() )
            throw Error( "ChannelCombination: Invalid source color space: " + p_channelId[i] );

         if ( sourceImage[i].IsFloatSample() )
            floatSample = true;

         if ( sourceImage[i].BitsPerSample() > bitsPerSample )
            bitsPerSample = sourceImage[i].BitsPerSample();

         if ( width == 0 )
         {
            width = sourceImage[i]->Width();
            height = sourceImage[i]->Height();
         }
         else
         {
            if ( sourceImage[i]->Width() != width || sourceImage[i]->Height() != height )
               throw Error( "ChannelCombination: Incompatible source image dimensions: " + p_channelId[i] );
         }

         sourceMetadata << IntegrationMetadata( sourceWindow[i].MainView().StorableProperties(),
                                                sourceWindow[i].Keywords() );
         ++numberOfSources;
      }

   if ( numberOfSources == 0 )
      throw Error( "ChannelCombination: No source image(s)." );

   ImageWindow outputWindow( width, height, 3, bitsPerSample, floatSample, true, true );
   if ( outputWindow.IsNull() )
      throw Error( "ChannelCombination: Unable to create output image." );

   View outputView = outputWindow.MainView();

   try
   {
      ImageVariant outputImage = outputView.Image();

      Console console;
      console.EnableAbort();

      StandardStatus status;
      outputImage->SetStatusCallback( &status );

      const char* what = "";
      int asChannel = 0;
      switch ( p_colorSpace )
      {
      default: // ?!
      case ColorSpaceId::RGB:
         what = "RGB channels";
         asChannel = 1; // green
         break;
      case ColorSpaceId::CIEXYZ:
         what = "normalized CIE XYZ components";
         asChannel = 1; // Y
         break;
      case ColorSpaceId::CIELab:
         what = "normalized CIE L*a*b* components";
         asChannel = 0; // L*
         break;
      case ColorSpaceId::CIELch:
         what = "normalized CIE L*c*h* components";
         asChannel = 0; // L*
         break;
      case ColorSpaceId::HSV:
         what = "normalized HSV components";
         asChannel = 2; // V
         break;
      case ColorSpaceId::HSI:
         what = "normalized HSI components";
         asChannel = 2; // I
         break;
      }
      outputImage->Status().Initialize( String( "Combining " ) + what, outputImage->NumberOfPixels() );

      String baseId = outputView.Id();
      Rect r = outputImage->Bounds();

      if ( outputImage.IsFloatSample() )
         switch ( outputImage.BitsPerSample() )
         {
         case 32:
            CombineChannels( static_cast<Image&>( *outputImage ), p_colorSpace, baseId, r,
                             sourceImage[0], sourceImage[1], sourceImage[2] );
            break;
         case 64:
            CombineChannels( static_cast<DImage&>( *outputImage ), p_colorSpace, baseId, r,
                             sourceImage[0], sourceImage[1], sourceImage[2] );
            break;
         }
      else
         switch ( outputImage.BitsPerSample() )
         {
         case  8:
            CombineChannels( static_cast<UInt8Image&>( *outputImage ), p_colorSpace, baseId, r,
                             sourceImage[0], sourceImage[1], sourceImage[2] );
            break;
         case 16:
            CombineChannels( static_cast<UInt16Image&>( *outputImage ), p_colorSpace, baseId, r,
                             sourceImage[0], sourceImage[1], sourceImage[2] );
            break;
         case 32:
            CombineChannels( static_cast<UInt32Image&>( *outputImage ), p_colorSpace, baseId, r,
                             sourceImage[0], sourceImage[1], sourceImage[2] );
            break;
         }

      PropertyArray properties;
      FITSKeywordArray keywords = outputWindow.Keywords();

      IntegrationMetadata metadata = IntegrationMetadata::Summary( sourceMetadata );
      metadata.UpdatePropertiesAndKeywords( properties, keywords );

      outputView.SetStorablePermanentProperties( properties );
      outputWindow.SetKeywords( keywords );

      if ( p_inheritAstrometricSolution )
      {
         ImageWindow asSourceWindow = sourceWindow[asChannel];
         bool asFound = !asSourceWindow.IsNull() && asSourceWindow.HasAstrometricSolution();
         if ( !asFound )
            for ( int i = 0; i < 3; ++i )
               if ( !sourceWindow[i].IsNull() && sourceWindow[i].HasAstrometricSolution() )
               {
                  asSourceWindow = sourceWindow[i];
                  asFound = true;
                  break;
               }

         if ( asFound )
            if ( outputWindow.CopyAstrometricSolution( asSourceWindow ) )
            {
               outputView.SetStorablePermanentPropertyValue( "PCL:AstrometricSolution:Information",
                                                             "source=inherited,process=ChannelCombination" );
               console.NoteLn( "<end><cbr>* Astrometric solution inherited: "
                              + asSourceWindow.MainView().Id() + " => " + baseId );
            }
            else
            {
               console.WarningLn( "<end><cbr>** Invalid astrometric solution ignored: "
                                 + asSourceWindow.MainView().Id() );
            }
      }

      outputWindow.Show();

      return true;
   }
   catch ( ... )
   {
      outputWindow.ForceClose();
      throw;
   }
}

// ----------------------------------------------------------------------------

void* ChannelCombinationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheColorSpaceIdCombinationParameter )
      return &p_colorSpace;
   if ( p == TheChannelEnabledCombinationParameter )
      return p_channelEnabled+tableRow;
   if ( p == TheChannelIdCombinationParameter )
      return p_channelId[tableRow].Begin();
   if ( p == TheCCInheritAstrometricSolutionParameter )
      return &p_inheritAstrometricSolution;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool ChannelCombinationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheChannelIdCombinationParameter )
   {
      p_channelId[tableRow].Clear();
      if ( sizeOrLength > 0 )
         p_channelId[tableRow].SetLength( sizeOrLength );
      return true;
   }

   if ( p == TheChannelTableCombinationParameter )
      return true;

   return false;
}

// ----------------------------------------------------------------------------

size_type ChannelCombinationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheChannelIdCombinationParameter )
      return p_channelId[tableRow].Length();
   if ( p == TheChannelTableCombinationParameter )
      return 3;

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelCombinationInstance.cpp - Released 2024-12-28T16:54:15Z
