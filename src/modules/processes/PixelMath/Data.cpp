//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Data.cpp - Released 2021-05-05T15:38:07Z
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
#include "ImageCache.h"
#include "PixelMathInstance.h"

#include <pcl/AstrometricMetadata.h>
#include <pcl/Image.h>
#include <pcl/ImageWindow.h>
#include <pcl/PixelInterpolation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ReferencedWindow
{
public:

   ReferencedWindow( const String& imageId )
      : m_id( imageId )
   {
   }

   ReferencedWindow( const ReferencedWindow& ) = default;

   void Attach() const
   {
      ++m_refCount;
   }

   void Detach() const
   {
      if ( --m_refCount == 0 )
         if ( m_locked )
         {
            View view = Window().MainView();
            if ( !view.CanWrite() )
               view.UnlockForWrite();
            m_locked = false;
         }
   }

   bool IsGarbage() const
   {
      return m_refCount <= 0;
   }

   ImageWindow Window() const
   {
      return ImageWindow::WindowById( m_id );
   }

   ImageVariant* NewImage() const
   {
      ImageWindow window = Window();
      if ( window.IsNull() ) // ?!
         return nullptr;

      View view = window.MainView();
      if ( !m_locked )
         if ( view.CanWrite() )
         {
            view.LockForWrite();
            m_locked = true;
         }

      Attach();

      return new ImageVariant( view.Image() );
   }

   bool operator ==( const ReferencedWindow& x ) const
   {
      return m_id == x.m_id;
   }

   bool operator <( const ReferencedWindow& x ) const
   {
      return m_id < x.m_id;
   }

private:

   String       m_id;
   mutable int  m_refCount = 0;
   mutable bool m_locked = false;
};

static SortedArray<ReferencedWindow> s_referencedWindows;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ImageReference::ImageReference( const String& id, int p, bool isWindow )
   : ObjectReference( XPR_IMAGEREF, p, id )
   , m_isWindow( isWindow )
{
}

// ----------------------------------------------------------------------------

ImageReference::ImageReference( const ImageReference& x )
   : ObjectReference( x )
   , m_isWindow( x.m_isWindow )
   , m_byReference( x.m_byReference )
   , m_astrometryRequired( x.m_astrometryRequired )
   , m_image( (x.m_image != nullptr) ? new ImageVariant( *x.m_image ) : nullptr )
{
   if ( m_isWindow )
   {
      if ( x.m_window )
         m_window = new ImageWindow( *x.m_window.Ptr() );

      if ( m_image != nullptr )
      {
         SortedArray<ReferencedWindow>::const_iterator r = s_referencedWindows.Search( Id() );
         if ( r != s_referencedWindows.End() )
            r->Attach();
      }
   }
}

// ----------------------------------------------------------------------------

template <class P> static
void DestroyInterpolators( void** interpolators, P* )
{
   for ( int i = 0; i < 3; ++i )
      if ( interpolators[i] != nullptr )
         delete reinterpret_cast<PixelInterpolation::Interpolator<P>*>( interpolators[i] ), interpolators[i] = nullptr;
}

static
void DestroyInterpolators( void** interpolators, const ImageVariant& v )
{
   if ( v.IsFloatSample() )
      switch ( v.BitsPerSample() )
      {
      case 32: DestroyInterpolators( interpolators, (FloatPixelTraits*)0 ); break;
      case 64: DestroyInterpolators( interpolators, (DoublePixelTraits*)0 ); break;
      }
   else
      switch ( v.BitsPerSample() )
      {
      case  8: DestroyInterpolators( interpolators, (UInt8PixelTraits*)0 ); break;
      case 16: DestroyInterpolators( interpolators, (UInt16PixelTraits*)0 ); break;
      case 32: DestroyInterpolators( interpolators, (UInt32PixelTraits*)0 ); break;
      }
}

ImageReference::~ImageReference()
{
   if ( m_image != nullptr )
   {
      if ( m_isWindow )
      {
         SortedArray<ReferencedWindow>::const_iterator r = s_referencedWindows.Search( Id() );
         if ( r != s_referencedWindows.End() )
         {
            r->Detach();
            if ( r->IsGarbage() )
               s_referencedWindows.Remove( s_referencedWindows.MutableIterator( r ) );
         }
      }

      DestroyInterpolators( m_interpolators, *m_image );

      delete m_image, m_image = nullptr;
   }
}

// ----------------------------------------------------------------------------

bool ImageReference::FindImage()
{
   delete m_image, m_image = nullptr;

   if ( m_isWindow )
   {
      SortedArray<ReferencedWindow>::const_iterator r = s_referencedWindows.Search( Id() );
      if ( r == s_referencedWindows.End() )
      {
         s_referencedWindows << Id();
         r = s_referencedWindows.Search( Id() );
      }
      m_window = new ImageWindow( r->Window() );
      m_image = r->NewImage();
   }

   return m_image != nullptr;
}

// ----------------------------------------------------------------------------

bool ImageReference::HasAstrometricSolution() const
{
   if ( m_window )
      return m_window->HasAstrometricSolution();
   return false; // ?!
}

// ----------------------------------------------------------------------------

template <class P> static
void InitImageInterpolators( void** interpolators, PixelInterpolation* interpolation, const GenericImage<P>& image )
{
   for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      interpolators[c] = (void*)interpolation->NewInterpolator<P>( image.PixelData( c ), image.Width(), image.Height() );
}

static
void InitImageInterpolators( void** interpolators, PixelInterpolation* interpolation, const ImageVariant* image )
{
   if ( image->IsFloatSample() )
      switch ( image->BitsPerSample() )
      {
      case 32: InitImageInterpolators( interpolators, interpolation, static_cast<const pcl::Image&>( **image ) ); break;
      case 64: InitImageInterpolators( interpolators, interpolation, static_cast<const pcl::DImage&>( **image ) ); break;
      }
   else
      switch ( image->BitsPerSample() )
      {
      case  8: InitImageInterpolators( interpolators, interpolation, static_cast<const pcl::UInt8Image&>( **image ) ); break;
      case 16: InitImageInterpolators( interpolators, interpolation, static_cast<const pcl::UInt16Image&>( **image ) ); break;
      case 32: InitImageInterpolators( interpolators, interpolation, static_cast<const pcl::UInt32Image&>( **image ) ); break;
      }
}

void ImageReference::InitInterpolators( PixelInterpolation* interpolation )
{
   InitImageInterpolators( m_interpolators, interpolation, m_image );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool InternalImageReference::FindImage()
{
   if ( m_image != nullptr )
      delete m_image, m_image = nullptr;

   ImageVariant image = TheImageCache->Image( IsoString( Id() ) );
   if ( image )
   {
      m_image = new ImageVariant( image );
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Data.cpp - Released 2021-05-05T15:38:07Z
