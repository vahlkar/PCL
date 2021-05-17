//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// ImageCache.h - Released 2021-05-05T15:38:07Z
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

#ifndef __ImageCache_h
#define __ImageCache_h

#include <pcl/ImageVariant.h>
#include <pcl/SortedArray.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ImageCache
{
public:

   ImageCache();
   virtual ~ImageCache();

   bool HasImage( const IsoString& key ) const
   {
      return m_cache.Contains( key );
   }

   ImageVariant Image( const IsoString& key ) const
   {
      SortedArray<CacheItem>::const_iterator i = m_cache.Search( key );
      if ( i != m_cache.End() )
         return i->image;
      return ImageVariant();
   }

   void AddImage( const IsoString& key, const ImageVariant& image )
   {
      m_cache << CacheItem( key, image );
   }

   void ClearImages()
   {
      m_cache.Clear();
   }

   bool ClearImages( size_type& count, size_type& size )
   {
      count = NumberOfImages();
      size = TotalImageSize();
      ClearImages();
      return count > 0;
   }

   bool IsEmpty() const
   {
      return m_cache.IsEmpty();
   }

   size_type NumberOfImages() const
   {
      return m_cache.Length();
   }

   size_type TotalImageSize() const
   {
      size_type sz = 0;
      for ( const CacheItem& item : m_cache )
         sz += item.image.ImageSize();
      return sz;
   }

   template <class S1, class S2>
   static IsoString MakeKey( const S1& imageId, const S2& keyData )
   {
      return IsoString( imageId ) + '#' + IsoString( keyData );
   }

   void InvalidateImage( const IsoString& imageId );

private:

   struct CacheItem
   {
      IsoString    key;
      ImageVariant image;

      CacheItem( const IsoString& k, const ImageVariant& i = ImageVariant() )
         : key( k )
         , image( i )
      {
      }

      CacheItem( const CacheItem& ) = default;

      bool operator ==( const CacheItem& x ) const
      {
         return key == x.key;
      }

      bool operator <( const CacheItem& x ) const
      {
         return key < x.key;
      }
   };

   SortedArray<CacheItem> m_cache;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ImageCache* TheImageCache;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ImageCache_h

// ----------------------------------------------------------------------------
// EOF ImageCache.h - Released 2021-05-05T15:38:07Z
