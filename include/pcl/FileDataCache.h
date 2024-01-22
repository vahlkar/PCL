//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.6
// ----------------------------------------------------------------------------
// pcl/FileDataCache.h - Released 2024-01-19T15:23:14Z
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

#ifndef __PCL_FileDataCache_h
#define __PCL_FileDataCache_h

/// \file pcl/FileDataCache.h

#include <pcl/File.h>
#include <pcl/Matrix.h>
#include <pcl/MultiVector.h>
#include <pcl/Mutex.h>
#include <pcl/ReferenceSortedArray.h>
#include <pcl/StringList.h>
#include <pcl/TimePoint.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class FileDataCacheItem
 * \brief Element of a file data cache
 *
 * This class represents a file in a FileDataCache object. This is a basic
 * cache item structure to transport a file's full path, known time of last
 * modification, and the time of last cache access.
 *
 * Reimplementing this class is normally necessary to store additional data
 * associated with cached files.
 *
 * \sa FileDataCache
 */
class PCL_CLASS FileDataCacheItem
{
public:

   String    path;     //!< Full path to the file represented by this item.
   IsoString key;      //!< Cache key. Allows different cache entries for the same file.
   TimePoint time;     //!< Cached file time.
   TimePoint lastUsed; //!< Time this cache item was last used.

   /*!
    * Virtual destructor.
    */
   virtual ~FileDataCacheItem()
   {
   }

   /*!
    * Assigns data from another cache \a item.
    */
   void Assign( const FileDataCacheItem& item )
   {
      path     = item.path;
      key      = item.key;
      time     = item.time;
      lastUsed = item.lastUsed;
   }

   /*!
    * Returs true iff this object represents the same file as another cache
    * \a item.
    */
   bool operator ==( const FileDataCacheItem& item ) const
   {
      return path == item.path;
   }

   /*!
    * Returns true iff this object precedes another cache \a item. File cache
    * items are sorted by full file paths in ascending order.
    */
   bool operator <( const FileDataCacheItem& item ) const
   {
      if ( path != item.path )
         return path < item.path;
      return key < item.key;
   }

   /*!
    * Returns true iff the file represented by this cache item was last
    * modified before the specified file time \a t.
    *
    * \note This member function ignores the milliseconds component of the
    * specified FileTime instance, by setting it to zero. This is done to
    * prevent wrong cache invalidations caused by unreliable file time
    * milliseconds on Windows.
    */
   bool ModifiedSince( FileTime t ) const
   {
      t.milliseconds = 0;
      return time < TimePoint( t );
   }

   /*!
    * Returns the amount of days elapsed since the time this cache item was
    * last used.
    */
   double DaysSinceLastUsed() const
   {
      return TimePoint::Now() - lastUsed;
   }

protected:

   /*!
    * Assigns additional data stored in another file cache \a item.
    *
    * The default implementation does nothing. This virtual member function
    * should be reimplemented by derived classes, when necessary, to ensure
    * persistence of reimplementation-specific data.
    */
   virtual void AssignData( const FileDataCacheItem& item )
   {
   }

   /*!
    * Returns an ordered list of UTF-8 strings representing the additional data
    * stored in this cache item.
    *
    * The default implementation returns an empty string list. This virtual
    * member function should be reimplemented by derived classes when necessary
    * to allow this root base class to access reimplementation-specific data.
    */
   virtual IsoStringList SerializedData() const
   {
      return IsoStringList();
   }

   /*!
    * Retrieves additional data from an ordered list of UTF-8 string \a tokens.
    * Returns true iff the data were successfully retrieved.
    *
    * The default implementation returns true. This virtual member function
    * should be reimplemented by derived classes when necessary for retrieval
    * of reimplementation-specific data.
    */
   virtual bool DeserializeData( const IsoStringList& tokens )
   {
      return true;
   }

   /*!
    * Returns true iff the additional data stored in this cache item is valid.
    *
    * The default implementation returns true. This virtual member function
    * should be reimplemented by derived classes, when necessary, for the
    * validation of reimplementation-specific data.
    */
   virtual bool ValidateData() const
   {
      return true;
   }

   /*!
    * Returns a string list serialization of a floating-point \a vector.
    */
   static IsoStringList SerializedVector( const DVector& vector );

   /*!
    * Deserializes a floating-point \a vector from the specified ordered list
    * of \a tokens, parsing the necessary tokens starting from the specified
    * \a start iterator.
    */
   static bool DeserializeVector( DVector& vector, IsoStringList::const_iterator& start, const IsoStringList& tokens );

   /*!
    * Returns a string list serialization of a floating-point \a matrix.
    */
   static IsoStringList SerializedMatrix( const DMatrix& matrix );

   /*!
    * Deserializes a floating-point \a matrix from the specified ordered list
    * of \a tokens, parsing the necessary tokens starting from the specified
    * \a start iterator.
    */
   static bool DeserializeMatrix( DMatrix& matrix, IsoStringList::const_iterator& start, const IsoStringList& tokens );

   /*!
    * Returns a string list serialization of a floating-point \a multivector.
    */
   static IsoStringList SerializedMultiVector( const DMultiVector& multivector );

   /*!
    * Deserializes a floating-point \a multivector from the specified ordered
    * list of \a tokens, parsing the necessary tokens starting from the
    * specified \a start iterator.
    */
   static bool DeserializeMultiVector( DMultiVector& multivector, IsoStringList::const_iterator& start, const IsoStringList& tokens );

   /*!
    * Returns a string list serialization of an array of floating-point \a matrices.
    */
   static IsoStringList SerializedMatrices( const Array<DMatrix>& matrices );

   /*!
    * Deserializes an array of floating-point \a matrices from the specified
    * ordered list of \a tokens, parsing the necessary tokens starting from the
    * specified \a start iterator.
    */
   static bool DeserializeMatrices( Array<DMatrix>& matrices, IsoStringList::const_iterator& start, const IsoStringList& tokens );

   /*!
    * Copy constructor.
    */
   FileDataCacheItem( const FileDataCacheItem& ) = default;

   /*!
    * \internal
    * Special constructor used for cache search operations.
    */
   FileDataCacheItem( const String& p = String(), const IsoString& k = IsoString() )
      : path( p )
      , key( k )
   {
   }

   friend class FileDataCache;
};

// ----------------------------------------------------------------------------

/*!
 * \class FileDataCache
 * \brief Abstract base class of file data cache implementations.
 *
 * This class provides the necessary infrastructure to implement a specialized
 * file cache with persistent storage in special cache files.
 *
 * The main cache access functions provided by this class to add, get and find
 * cache items, as well as to clear the cache and query cache properties, are
 * implemented as thread-safe routines. This supports applications performing
 * parallel disk I/O operations and multiple application instances running
 * concurrently, with separate per-instance cache data files.
 *
 * Cache files are generated and managed transparently on platform-dependent
 * configuration directories.
 *
 * You'll find examples of use for this class in standard PixInsight modules
 * such as ImageIntegration and SubframeSelector.
 *
 * \sa FileDataCacheItem
 */
class PCL_CLASS FileDataCache
{
public:

   /*!
    * Constructs a new file data cache structure.
    *
    * \param identifier A valid identifier (see IsoString::IsValidIdentifier())
    *                   that will be used to construct the unique name of a
    *                   special cache data file where all cache data structures
    *                   associated with this object will be stored persistently
    *                   for the running core application instance.
    *
    * \param maxItemDuration  The maximum duration in days for valid cache
    *                   items. The supported range is from zero (meaning that
    *                   cache items never expire) to 120 days. The default
    *                   value is 30 days.
    *
    * \warning If the specified value of \a maxItemDuration is &le; 0, existing
    * cache items will never expire. This is <em>not recommended</em> and can
    * cause problems by increasing the size of cache files indiscriminately. In
    * general, the default maximum duration of 30 days is quite appropriate for
    * most applications.
    */
   FileDataCache( const IsoString& identifier, int maxItemDuration = 30 );

   /*!
    * Virtual destructor.
    *
    * Destroys and deallocates all file data cache items and internal
    * structures associated with this object. Note that this refers to data
    * currently stored in memory, not to persistent storage in a cache file.
    * To destroy data stored persistently, the Purge() member function must be
    * called explicitly.
    */
   virtual ~FileDataCache()
   {
      Clear();
   }

   /*!
    * Returns an identifying name for this cache object. The default
    * implementation returns "File Cache". Derived classes should reimplement
    * this function to return more specific names.
    */
   virtual String CacheName() const
   {
      return "File Cache";
   }

   /*!
    * Returns the current cache version. The default implementation returns 1.
    *
    * \sa MinSupportedVersion()
    */
   virtual int Version() const
   {
      return 1;
   }

   /*!
    * Returns the minimum supported cache version. The default implementation
    * returns 1.
    *
    * Items with a version that is either less than the value returned by this
    * function or greater than the current cache version will not be loaded
    * from existing cache files. This allows for a basic version control system
    * with a range of valid cache versions.
    *
    * \sa Version()
    */
   virtual int MinSupportedVersion() const
   {
      return 1;
   }

   /*!
    * Returns the maximum duration in days of a valid cache item.
    *
    * Existing cache items that have not been accessed during a period longer
    * than the value returned by this function will not be loaded from
    * persistent data.
    *
    * \sa SetMaxItemDuration(), ItemsNeverExpire()
    */
   int MaxItemDuration() const
   {
      return m_maxItemDuration;
   }

   /*!
    * Sets the maximum duration in days for valid cache items. The supported
    * range is from zero (meaning that cache items never expire) to 120 days.
    *
    * \warning If the specified number of \a days is &le; 0, existing cache
    * items will never expire. This is <em>not recommended</em> and can cause
    * problems by increasing the size of cache files indiscriminately. In
    * general, the default maximum duration of 30 days is quite appropriate for
    * most applications.
    *
    * \sa MaxItemDuration(), ItemsNeverExpire()
    */
   virtual void SetMaxItemDuration( int days )
   {
      m_maxItemDuration = Range( days, 0, 120 );
   }

   /*!
    * Returns true iff existing cache items associated with this object will
    * never expire. This is achieved when the maximum item duration has been
    * set to a value &le; 0.
    *
    * \sa MaxItemDuration(), SetMaxItemDuration()
    */
   bool ItemsNeverExpire() const
   {
      return m_maxItemDuration <= 0;
   }

   /*!
    * Returns the token separator used by this cache file.
    *
    * Reimplementation-specific cache item data is serialized as ordered lists
    * of string tokens in XML cache files. This member function returns an
    * UTF-8 string (usually consisting of a single character) used to separate
    * adjacent tokens in these data serializations.
    *
    * The default token separator is a newline character.
    *
    * \sa SetTokenSeparator()
    */
   IsoString TokenSeparator() const
   {
      return m_separator;
   }

   /*!
    * Sets the token separator for this cache file.
    *
    * The specified \a separator UTF-8 string will be used to separate items in
    * cache item data serialized as ordered lists of string tokens. If an empty
    * string is specified, the default token separator (a single newline
    * character) will be used.
    *
    * \sa TokenSeparator()
    */
   void SetTokenSeparator( const IsoString& separator )
   {
      m_separator = separator.IsEmpty() ? IsoString( '\n' ) : separator;
   }

   /*!
    * Returns true if data compression is enabled for serializations of cache
    * item data performed with this object.
    *
    * Data compression is enabled by default for newly constructed
    * %FileDataCache objects. Currently the Zstandard compression codec is
    * applied for serialization of cache item data.
    *
    * \sa EnableCompression(), DisableCompression()
    */
   bool IsCompressionEnabled() const
   {
      return m_compressionEnabled;
   }

   /*!
    * Enables compression of serialized cache item data.
    *
    * \sa IsCompressionEnabled(), DisableCompression()
    */
   void EnableCompression( bool enable = true )
   {
      m_compressionEnabled = enable;
   }

   /*!
    * Disables compression of serialized cache item data.
    *
    * \sa IsCompressionEnabled(), EnableCompression()
    */
   void DisableCompression( bool disable = true )
   {
      EnableCompression( !disable );
   }

   /*!
    * Returns the total number of cache items associated with this object.
    *
    * The returned value corresponds to the number of cache items currently
    * stored in internal data structures. This includes cache items loaded from
    * existing cache files as well as items newly created and possibly still
    * not copied to persistent storage.
    *
    * \note This function is thread-safe.
    */
   size_type NumberOfItems() const;

   /*!
    * Returns true iff this cache is empty, i.e. if there are no cache items
    * associated with this object.
    *
    * The returned value corresponds to the items currently stored in internal
    * memory data structures. This does not necessarily equals the total number
    * of items currently stored in persistent cache files.
    *
    * \note This function is thread-safe.
    */
   bool IsEmpty() const;

   /*!
    * Returns the current size in bytes of the cache file generated by this
    * object. Returns zero if no cache file has been generated yet.
    */
   fsize_type FileSize() const;

   /*!
    * Returns the address of a file cache item corresponding to the specified
    * file \a path and \a key, or nullptr if no such cache item could befound.
    *
    * This function does not modify the cache or the time of last access of the
    * item found.
    *
    * \note This function is thread-safe.
    *
    * \sa Get()
    */
   const FileDataCacheItem* Find( const String& path, const IsoString& key = IsoString() ) const;

   /*!
    * Destroys and removes all cache items currently associated with this
    * object.
    *
    * Only items stored in internal memory data structures are removed by this
    * function. Persistent storage is not altered.
    *
    * \note This function is thread-safe.
    */
   void Clear();

   /*!
    * Adds the specified \a item to this cache.
    *
    * The item will be stored in internal memory data structures, \e not in
    * persistent cache files. To store cache items persistently, the Save()
    * member function must be called for this object.
    *
    * \note This function is thread-safe.
    */
   void Add( const FileDataCacheItem& item );

   /*!
    * Retrieves a copy of the existing cache data corresponding to the
    * specified file \a path and \a key in the specified \a item.
    *
    * Returns true iff a cache item for the specified \a path was found with
    * the specified \a key in internal memory data structures, and its data
    * were copied. If false is returned, the specified \a item will not be
    * modified in any way.
    *
    * \note This function is thread-safe.
    *
    * \sa Find()
    */
   bool Get( FileDataCacheItem& item, const String& path, const IsoString& key = IsoString() );

   /*!
    * Returns the number of cache modifications performed since the last cache
    * file load operation.
    *
    * Cache modifications happen each time a new cache item is added or an
    * existing cache item is accessed. The modification count is reset to zero
    * when the cache is cleared, when a cache file is loaded, and when the
    * cache is saved to a disk file.
    *
    * \sa IsModified()
    */
   int ModifiedCount() const
   {
      return m_modifiedCount;
   }

   /*!
    * Returns true iff this cache object has been modified.
    *
    * \sa ModifiedCount()
    */
   bool IsModified() const
   {
      return m_modifiedCount > 0;
   }

   /*!
    * Loads existing valid cache items from persistent storage.
    *
    * All previously existing cache items stored in internal memory structures
    * will be destroyed and deallocated before loading new data.
    *
    * \note Overriding implementations of this function must call the base
    * class implementation.
    */
   virtual void Load();

   /*!
    * Writes all cache items associated with this object to persistent cache
    * file storage.
    *
    * \note Overriding implementations of this function must call the base
    * class implementation.
    */
   virtual void Save() const;

   /*!
    * Destroys and deallocates all existing cache items, including all items
    * currently in internal memory data structures as well as all items stored
    * in persistent storage. This yields an empty cache file associated with
    * this object.
    *
    * \note Overriding implementations of this function must call the base
    * class implementation.
    */
   virtual void Purge() const;

protected:

   /*!
    * Allocates and constructs a new cache item.
    *
    * Returns a pointer to the newly created cache item. The new item will be
    * owned by this object, which will destroy and deallocate it automatically
    * when appropriate.
    *
    * This is a pure virtual member function that must be reimplemented by all
    * derived classes. This is because the data transported by a cache item is
    * application-specific and cannot be known in advance by this base class.
    */
   virtual FileDataCacheItem* NewItem() const = 0;

private:

   using cache_index = ReferenceSortedArray<FileDataCacheItem>;

   mutable Mutex       m_mutex;
           cache_index m_cache;
           IsoString   m_identifier;
           String      m_filePath;
           IsoString   m_separator = '\n';
           int         m_maxItemDuration = 30; // in days
           bool        m_compressionEnabled = true;
   mutable int         m_modifiedCount = 0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FileDataCache_h

// ----------------------------------------------------------------------------
// EOF pcl/FileDataCache.h - Released 2024-01-19T15:23:14Z
