//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// pcl/FileDataCache.cpp - Released 2024-06-18T15:49:06Z
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

#include <pcl/AutoLock.h>
#include <pcl/AutoPointer.h>
#include <pcl/Compression.h>
#include <pcl/FileDataCache.h>
#include <pcl/FileInfo.h>
#include <pcl/GlobalSettings.h>
#include <pcl/XML.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FileDataCache::FileDataCache( const IsoString& identifier, int maxItemDuration )
   : m_identifier( identifier.Trimmed() )
   , m_maxItemDuration( Range( maxItemDuration, 0, 120 ) )
{
   PCL_PRECONDITION( !m_identifier.IsEmpty() )
   if ( m_identifier.IsEmpty() )
      throw Error( "FileDataCache: Empty file identifier" );
   if ( !m_identifier.IsValidIdentifier() )
      throw Error( "FileDataCache: Invalid file identifier" );

   int instance = PixInsightSettings::GlobalInteger( "Application/Instance" );
   if ( instance <= 0 )
      throw Error( "FileDataCache: Invalid application instance" );

   String confDirPath = PixInsightSettings::GlobalString( "Application/ConfigDirectory" );
   if ( confDirPath.IsEmpty() || !File::DirectoryExists( confDirPath ) )
      throw Error( "FileDataCache: Internal error: Invalid application configuration directory" );
   if ( !confDirPath.EndsWith( '/' ) )
      confDirPath << '/';

   m_filePath = confDirPath + m_identifier + String().Format( "-%03d-pxi.cache", instance );
}

// ----------------------------------------------------------------------------

size_type FileDataCache::NumberOfItems() const
{
   volatile AutoLock lock( m_mutex );
   return m_cache.Length();
}

// ----------------------------------------------------------------------------

bool FileDataCache::IsEmpty() const
{
   volatile AutoLock lock( m_mutex );
   return m_cache.IsEmpty();
}

// ----------------------------------------------------------------------------

fsize_type FileDataCache::FileSize() const
{
   FileInfo info( m_filePath );
   if ( !info.Exists() || !info.IsFile() ) // just in case
      return 0;
   return info.Size();
}

// ----------------------------------------------------------------------------

const FileDataCacheItem* FileDataCache::Find( const String& path, const IsoString& key ) const
{
   volatile AutoLock lock( m_mutex );
   cache_index::const_iterator i = m_cache.Search( FileDataCacheItem( path, key ) );
   return (i == m_cache.End()) ? nullptr : i;
}

// ----------------------------------------------------------------------------

void FileDataCache::Clear()
{
   volatile AutoLock lock( m_mutex );
   m_cache.Destroy();
   m_modifiedCount = 0;
}

// ----------------------------------------------------------------------------

void FileDataCache::Add( const FileDataCacheItem& item )
{
   FileInfo info( item.path );
   if ( !info.Exists() || !info.IsFile() )
      throw Error( "FileDataCache::Add(): No such file: " + item.path );

   {
      volatile AutoLock lock( m_mutex );

      cache_index::const_iterator i = m_cache.Search( item );
      if ( i == m_cache.End() )
      {
         FileDataCacheItem* newItem = NewItem();
         newItem->path = item.path;
         newItem->key = item.key;
         m_cache << newItem;
         i = m_cache.Search( *newItem );
         if ( i == m_cache.End() ) // ?!
            throw Error( "FileDataCache::Add(): Internal error adding cache item: " + item.path );
      }
      FileDataCacheItem& newItem = *m_cache.MutableIterator( i );
      FileTime t = info.LastModified();
      t.milliseconds = 0; // prevent wrong cache invalidations on Windows
      newItem.time = t;
      newItem.lastUsed = TimePoint::Now();
      newItem.AssignData( item );

      ++m_modifiedCount;
   }
}

// ----------------------------------------------------------------------------

bool FileDataCache::Get( FileDataCacheItem& item, const String& path, const IsoString& key )
{
   FileInfo info( path );
   bool badItem = !info.Exists() || !info.IsFile();

   {
      volatile AutoLock lock( m_mutex );

      cache_index::const_iterator i = m_cache.Search( FileDataCacheItem( path, key ) );
      if ( i != m_cache.End() )
      {
         ++m_modifiedCount;

         if ( !badItem )
         {
            item.Assign( *i );
            item.AssignData( *i );
            if ( !item.ModifiedSince( info.LastModified() ) )
            {
               m_cache.MutableIterator( i )->lastUsed = TimePoint::Now();
               return true;
            }
         }

         m_cache.Destroy( m_cache.MutableIterator( i ) );
      }
   }

   if ( badItem )
      throw Error( "FileDataCache::Get(): No such file: " + path );

   return false;
}

// ----------------------------------------------------------------------------

void FileDataCache::Load()
{
   if ( !File::Exists( m_filePath ) )
      return;

   m_cache.Destroy();
   m_separator = '\n';
   m_modifiedCount = 0;

   IsoString text = File::ReadTextFile( m_filePath );
   if ( text.IsEmpty() )
      return;

   XMLDocument xml;
   xml.SetParserOption( XMLParserOption::IgnoreComments );
   xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
   xml.Parse( text.UTF8ToUTF16() );

   if ( xml.RootElement() == nullptr )
      throw Error( "FileDataCache::Load(): The XML document has no root element." );
   if ( xml.RootElement()->Name() != "xfch" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
      throw Error( "FileDataCache::Load(): Not an XFCH version 1.0 document." );

   try
   {
      TimePoint currentTime = TimePoint::Now();

      for ( const XMLNode& node : *xml.RootElement() )
      {
         if ( !node.IsElement() )
            continue;

         const XMLElement& element = static_cast<const XMLElement&>( node );

         if ( element.Name() == "Cache" )
         {
            if ( !m_cache.IsEmpty() )
               throw Error( "Duplicate Cache child element." );

            String s = element.AttributeValue( "version" );
            if ( !s.IsEmpty() )
            {
               int version = s.ToInt();
               if ( version < MinSupportedVersion() || version > Version() )
                  return;
            }

            s = element.AttributeValue( "separator" );
            if ( !s.IsEmpty() )
               m_separator = IsoString( s ).URLDecoded();

            for ( const XMLNode& node : element )
            {
               if ( !node.IsElement() )
                  continue;

               const XMLElement& element1 = static_cast<const XMLElement&>( node );

               if ( element1.Name() == "Item" )
               {
                  s = element1.AttributeValue( "time" );
                  if ( s.IsEmpty() )
                     throw Error( "Parsing Cache Item child element: missing time attribute." );
                  TimePoint itemTime = TimePoint( s );
                  if ( !itemTime.IsValid() )
                     throw Error( "Parsing Cache Item child element: invalid time attribute value." );

                  IsoString itemKey;
                  s = element1.AttributeValue( "key" );
                     itemKey = IsoString( s );

                  s = element1.AttributeValue( "lastUsed" );
                  if ( s.IsEmpty() )
                     throw Error( "Parsing Cache Item child element: missing lastUsed attribute." );
                  TimePoint itemLastUsed = TimePoint( s );
                  if ( !itemLastUsed.IsValid() || itemLastUsed < itemTime )
                     throw Error( "Parsing Cache Item child element: invalid lastUsed attribute value." );

                  if ( m_maxItemDuration > 0 )
                     if ( currentTime - itemLastUsed > m_maxItemDuration )
                        continue;

                  String itemPath;
                  IsoString itemData;

                  for ( const XMLNode& node : element1 )
                  {
                     if ( !node.IsElement() )
                        continue;

                     const XMLElement& element2 = static_cast<const XMLElement&>( node );

                     if ( element2.Name() == "Path" )
                     {
                        itemPath = element2.Text().Trimmed();
                        if ( itemPath.IsEmpty() )
                           throw Error( "Parsing Cache Item Path child element: empty element value." );
                     }
                     else if ( element2.Name() == "Data" )
                     {
                        String compressionName = element2.AttributeValue( "compression" ).CaseFolded();
                        if ( !compressionName.IsEmpty() )
                        {
                           AutoPointer<Compression> compression;
                           if ( compressionName == "lz4" || compressionName == "lz4+sh" )
                              compression = new LZ4Compression;
                           else if ( compressionName == "lz4hc" || compressionName == "lz4hc+sh" )
                              compression = new LZ4HCCompression;
                           else if ( compressionName == "zlib" || compressionName == "zlib+sh" )
                              compression = new ZLibCompression;
                           else if ( compressionName == "zstd" || compressionName == "zstd+sh" )
                              compression = new ZstdCompression;
                           else
                              throw Error( "Parsing Cache Item Data child element: unknown or unsupported compression codec '" + compressionName + '\'' );

                           if ( compressionName.EndsWith( "+sh" ) )
                           {
                              s = element2.AttributeValue( "itemSize" );
                              if ( s.IsEmpty() )
                                 throw Error( "Parsing Cache Item Data child element: missing itemSize attribute, required for compression with byte shuffling." );
                              compression->EnableByteShuffling();
                              compression->SetItemSize( s.ToInt() );
                           }

                           Compression::subblock_list subblocks;

                           for ( const XMLNode& node : element2 )
                           {
                              if ( !node.IsElement() )
                                 continue;

                              const XMLElement& element3 = static_cast<const XMLElement&>( node );

                              if ( element3.Name() == "Subblock" )
                              {
                                 Compression::Subblock subblock;
                                 s = element3.AttributeValue( "uncompressedSize" );
                                 if ( s.IsEmpty() )
                                    throw Error( "Parsing Cache Item Data Subblock child element: missing uncompressedSize attribute." );
                                 subblock.uncompressedSize = s.ToUInt64();
                                 subblock.compressedData = IsoString( element3.Text().Trimmed() ).FromBase64();
                                 subblocks << subblock;
                              }
                           }

                           if ( subblocks.IsEmpty() )
                              throw Error( "Parsing Cache Item Data child element: missing Subblock child element(s)." );

                           itemData = IsoString( compression->Uncompress( subblocks ) );
                        }
                        else
                        {
                           itemData = IsoString( IsoString( element2.Text().Trimmed() ).FromBase64() );
                        }
                     }
                  }

                  if ( itemPath.IsEmpty() )
                     throw Error( "Parsing Cache Item child element: missing Path child element." );

                  IsoStringList tokens;
                  if ( !itemData.IsEmpty() )
                     itemData.Break( tokens, m_separator );

                  AutoPointer<FileDataCacheItem> item = NewItem();
                  item->path = itemPath;
                  item->key = itemKey;
                  item->time = itemTime;
                  item->lastUsed = itemLastUsed;

                  if ( item->DeserializeData( tokens ) )
                     if ( item->ValidateData() )
                        m_cache << item.Release();
               }
            }
         }
      }
   }
   catch ( Exception& x )
   {
      m_cache.Destroy();
      throw Error( "FileDataCache::Load(): Corrupted cache data: " + x.Message() );
   }
   catch ( ... )
   {
      m_cache.Destroy();
      throw Error( "FileDataCache::Load(): Corrupted cache data." );
   }
}

// ----------------------------------------------------------------------------

void FileDataCache::Save() const
{
   AutoPointer<XMLDocument> xml = new XMLDocument;
   xml->SetXML( "1.0", "UTF-8" );
   *xml << new XMLComment( "\nPixInsight XML File Cache Data Format - XFCH version 1.0"
                           "\nCreated with PixInsight software - http://pixinsight.com/"
                           "\n" );

   XMLElement* root = new XMLElement( "xfch", XMLAttributeList()
      << XMLAttribute( "version", "1.0" )
      << XMLAttribute( "xmlns", "http://www.pixinsight.com/xfch" )
      << XMLAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" )
      << XMLAttribute( "xsi:schemaLocation", "http://www.pixinsight.com/xfch http://pixinsight.com/xfch/xfch-1.0.xsd" ) );

   xml->SetRootElement( root );

   XMLElement* cacheChild = new XMLElement( *root, "Cache", XMLAttributeList()
      << XMLAttribute( "version", String( Version() ) )
      << XMLAttribute( "separator", m_separator.URLEncoded() )
      << XMLAttribute( "creationTime", TimePoint::Now().ToString() ) );

   for ( const FileDataCacheItem& item : m_cache )
   {
      XMLElement* itemChild = new XMLElement( *cacheChild, "Item", XMLAttributeList()
         << XMLAttribute( "key", item.key )
         << XMLAttribute( "time", item.time.ToString() )
         << XMLAttribute( "lastUsed", item.lastUsed.ToString() ) );

      *(new XMLElement( *itemChild, "Path" )) << new XMLText( item.path );

      IsoStringList itemData = item.SerializedData();
      if ( !itemData.IsEmpty() )
      {
         XMLElement* dataChild = new XMLElement( *itemChild, "Data" );

         IsoString text = IsoString().ToSeparated( itemData, m_separator );

         if ( m_compressionEnabled )
         {
            ZstdCompression compression;
            Compression::subblock_list subblocks = compression.Compress( text );
            if ( !subblocks.IsEmpty() )
            {
               dataChild->SetAttribute( "compression", compression.AlgorithmName() );
               for ( const Compression::Subblock& subblock : subblocks )
               {
                  XMLElement* subblockChild = new XMLElement( *dataChild, "Subblock" );
                  subblockChild->SetAttribute( "uncompressedSize", String( subblock.uncompressedSize ) );
                  *subblockChild << new XMLText( IsoString::ToBase64( subblock.compressedData ) );
               }
               continue;
            }
         }

         *dataChild << new XMLText( IsoString::ToBase64( text ) );
      }
   }

   xml->EnableAutoFormatting();
   xml->SetIndentSize( 3 );
   xml->SerializeToFile( m_filePath );

   m_modifiedCount = 0;
}

// ----------------------------------------------------------------------------

void FileDataCache::Purge() const
{
   if ( File::Exists( m_filePath ) )
      File::Remove( m_filePath );
}

// ----------------------------------------------------------------------------

IsoStringList FileDataCacheItem::SerializedVector( const DVector& v )
{
   IsoStringList tokens;
   tokens << IsoString( v.Length() );
   for ( double x : v )
      tokens << IsoString().Format( "%.16e", x );
   return tokens;
}

bool FileDataCacheItem::DeserializeVector( DVector& v, IsoStringList::const_iterator& i, const IsoStringList& s )
{
   if ( i == s.End() )
      return false;
   int n = i->ToInt();
   ++i;
   if ( n < 0 || s.End() - i < n )
      return false;
   v = DVector( n );
   for ( double& x : v )
   {
      x = i->ToDouble();
      ++i;
   }
   return true;
}

// ----------------------------------------------------------------------------

IsoStringList FileDataCacheItem::SerializedMatrix( const DMatrix& M )
{
   IsoStringList tokens;
   tokens << IsoString( M.Rows() )
          << IsoString( M.Cols() );
   for ( double x : M )
      tokens << IsoString().Format( "%.16e", x );
   return tokens;
}

bool FileDataCacheItem::DeserializeMatrix( DMatrix& M, IsoStringList::const_iterator& i, const IsoStringList& s )
{
   if ( i == s.End() )
      return false;
   int rows = i->ToInt();
   if ( rows < 0 )
      return false;
   ++i;
   if ( i == s.End() )
      return false;
   int cols = i->ToInt();
   if ( cols < 0 )
      return false;
   ++i;
   if ( s.End() - i < rows*cols )
      return false;
   M = DMatrix( rows, cols );
   for ( double& x : M )
   {
      x = i->ToDouble();
      ++i;
   }
   return true;
}

// ----------------------------------------------------------------------------

IsoStringList FileDataCacheItem::SerializedMultiVector( const DMultiVector& m )
{
   IsoStringList tokens;
   tokens << IsoString( m.Length() );
   for ( const DVector& v : m )
      tokens << SerializedVector( v );
   return tokens;
}

bool FileDataCacheItem::DeserializeMultiVector( DMultiVector& m, IsoStringList::const_iterator& i, const IsoStringList& s )
{
   if ( i == s.End() )
      return false;
   int n = i->ToInt();
   ++i;
   if ( n < 0 || s.End() - i < n )
      return false;
   for ( int j = 0; j < n; ++j )
   {
      DVector v;
      if ( !DeserializeVector( v, i, s ) )
         return false;
      m << v;
   }
   return true;
}

// ----------------------------------------------------------------------------

IsoStringList FileDataCacheItem::SerializedMatrices( const Array<DMatrix>& A )
{
   IsoStringList tokens;
   tokens << IsoString( A.Length() );
   for ( const DMatrix& M : A )
      tokens << SerializedMatrix( M );
   return tokens;
}

bool FileDataCacheItem::DeserializeMatrices( Array<DMatrix>& A, IsoStringList::const_iterator& i, const IsoStringList& s )
{
   if ( i == s.End() )
      return false;
   int n = i->ToInt();
   ++i;
   if ( n < 0 || s.End() - i < n )
      return false;
   for ( int j = 0; j < n; ++j )
   {
      DMatrix M;
      if ( !DeserializeMatrix( M, i, s ) )
         return false;
      A << M;
   }
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FileDataCache.cpp - Released 2024-06-18T15:49:06Z
