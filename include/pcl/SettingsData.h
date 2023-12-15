//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// pcl/SettingsData.h - Released 2023-12-01T19:15:45Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_SettingsData_h
#define __PCL_SettingsData_h

/// \file pcl/SettingsData.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AutoPointer.h>
#include <pcl/Variant.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct SettingsTree;

/*!
 * \class SettingsData
 * \brief Serializable, thread-safe settings data.
 *
 * This class implements a specialized dictionary for fast indexed access to
 * key/value pairs with persistent storage in special per-instance XML files.
 *
 * The main access functions provided by this class to add, get and find
 * settings values, as well as to clear, load and save the settings and query
 * object properties, are implemented as thread-safe routines. This supports
 * applications performing parallel settings access and disk I/O operations, as
 * well as multiple application instances running concurrently.
 *
 * Settings files are generated and managed transparently on platform-dependent
 * configuration directories.
 */
class PCL_CLASS SettingsData
{
public:

   /*!
    * Constructs a new settings data structure.
    *
    * \param identifier A valid identifier (see IsoString::IsValidIdentifier())
    *                   that will be used to construct the unique name of a
    *                   special settings file where all key/value pairs in this
    *                   object will be stored persistently for the running core
    *                   application instance.
    */
   SettingsData( const IsoString& identifier );

   /*!
    * Virtual destructor.
    *
    * Destroys and deallocates all settings data items and internal structures
    * associated with this object. Note that this refers to data currently
    * stored in memory, not to persistent storage in a settings file. To
    * destroy data stored persistently, the Purge() member function must be
    * called explicitly.
    */
   virtual ~SettingsData();

   /*!
    * Returns true iff this settings data object is empty, i.e. if there are no
    * key/value pairs stored in this object.
    *
    * The returned value corresponds to the items currently stored in internal
    * memory data structures. This does not necessarily equals the total number
    * of items currently stored in persistent settings files.
    *
    * \note This function is thread-safe.
    */
   bool IsEmpty() const;

   /*!
    * Returns the current size in bytes of the settings file generated by this
    * object. Returns zero if no settings file has been generated yet.
    */
   fsize_type FileSize() const;

   /*!
    * Destroys and removes all key/value pairs currently stored in this object.
    *
    * Only items stored in internal memory data structures are removed by this
    * function. Persistent storage is not altered.
    *
    * \note This function is thread-safe.
    */
   void Clear();

   /*!
    * Adds the specified \a key with the specified \a value to this object.
    *
    * If an item with the specified \a key already exists in this object, it
    * will be updated with the specified \a value only if it is different from
    * the current one. Otherwise, a new key/value pair will be created.
    *
    * Returns true iff the specified \a key was added, or if an existing value
    * was modified.
    *
    * The key/value pair will be stored in internal memory data structures,
    * \e not in persistent settings files. To store settings persistently, the
    * Save() member function must be called for this object.
    *
    * \note This function is thread-safe.
    */
   bool Add( const IsoString& key, const Variant& value );

   /*!
    * Removes a key/value pair or a subtree of key/value pairs.
    *
    * This function can be used to remove an existing key/value pair or a
    * subtree rooted at the specified \a key. No action will be taken if \a key
    * is not found.
    *
    * Returns true iff the specified \a key was found and removed.
    *
    * Calling this function with \a key equal to the root specifier "/" will
    * cause no effect and false will be returned. Clear() must be used to
    * remove the entire set of key/value pairs stored in this object in a
    * single operation.
    *
    * The \a key will be removed from internal memory data structures, \e not
    * from persistent settings files. To store settings persistently, the
    * Save() member function must be called for this object.
    *
    * \note This function is thread-safe.
    */
   bool Remove( const IsoString& key );

   /*!
    * Returns the current value associated with the specified \a key.
    *
    * If a key/value pair with the specified \a key is currently stored in this
    * object, the corresponding value will be returned. If \a key is not found,
    * or if it corresponds to a structural key (i.e. the root of a subtree of
    * key/value pairs) this function returns an invalid Variant object.
    *
    * \note This function is thread-safe.
    */
   Variant Search( const IsoString& key ) const;

   /*!
    * Returns a list with all existing keys rooted at the specified \a key.
    *
    * \note This function is thread-safe.
    *
    * \sa AllKeys()
    */
   IsoStringList Keys( const IsoString& key ) const;

   /*!
    * Returns a list with all keys stored in this object.
    *
    * \note This function is thread-safe.
    *
    * \sa Keys()
    */
   IsoStringList AllKeys() const;

   /*!
    * Returns true iff XML source code minification is currently enabled.
    *
    * When minification is enabled, XML source code will be generated without
    * superfluous whitespace and newline characters, making it smaller and
    * marginally more efficient for encoding/decoding operations. With
    * minification disabled, XML tags will be separated with newline characters
    * and indented with three spaces, making the source code readable and
    * easily editable.
    *
    * XML minification is enabled by default.
    *
    * \sa EnableMinifiedXML(), DisableMinifiedXML()
    */
   bool IsMinifiedXMLEnabled() const
   {
      return m_minifyXML;
   }

   /*!
    * Enables XML source code minification.
    *
    * \sa IsMinifiedXMLEnabled()
    */
   void EnableMinifiedXML( bool enable = true )
   {
      m_minifyXML = enable;
   }

   /*!
    * Disables XML source code minification.
    *
    * \sa IsMinifiedXMLEnabled()
    */
   void DisableMinifiedXML( bool disable = true )
   {
      EnableMinifiedXML( !disable );
   }

   /*!
    * Returns true iff block compression is currently enabled.
    *
    * When block compression is enabled, String and ByteArray property values
    * are serialized with compression in XML files when their sizes are greater
    * than or equal to the current compression threshold (see
    * CompressionThreshold()). The Zstandard compression codec is used in
    * current versions of PCL.
    *
    * Block compression is enabled by default.
    *
    * \sa EnableBlockCompression(), DisableBlockCompression(),
    * CompressionThreshold()
    */
   bool IsBlockCompressionEnabled() const
   {
      return m_compressBlocks;
   }

   /*!
    * Enables block compression.
    *
    * \sa IsBlockCompressionEnabled()
    */
   void EnableBlockCompression( bool enable = true )
   {
      m_compressBlocks = enable;
   }

   /*!
    * Disables block compression.
    *
    * \sa IsBlockCompressionEnabled()
    */
   void DisableBlockCompression( bool disable = true )
   {
      EnableBlockCompression( !disable );
   }

   /*!
    * Returns the current compression threshold in bytes.
    *
    * This value is the minimum length of a String or ByteArray property value
    * that triggers block compression for serialization as XML files.
    *
    * The default compression threshold is 4 KiB (4096 bytes).
    *
    * \sa IsBlockCompressionEnabled()
    */
   unsigned CompressionThreshold() const
   {
      return m_compressionThreshold;
   }

   /*!
    * Sets the compression \a threshold in bytes.
    *
    * The valid range is from 256 to 65536 bytes.
    *
    * \sa CompressionThreshold(), IsBlockCompressionEnabled()
    */
   void SetCompressionThreshold( unsigned threshold )
   {
      m_compressionThreshold = Range( threshold, 256u, 65536u );
   }

   /*!
    * Returns the number of modifications performed since the last file load
    * operation.
    *
    * Modifications happen each time a new key/value pair is added or the value
    * associated with an existing key is modified. The modification count is
    * reset to zero when the settings are cleared, when a settings file is
    * loaded, and when the settings data are saved to a disk file.
    *
    * \sa IsModified()
    */
   int ModifiedCount() const
   {
      return m_modifiedCount;
   }

   /*!
    * Returns true iff this settings object has been modified.
    *
    * \sa ModifiedCount()
    */
   bool IsModified() const
   {
      return m_modifiedCount > 0;
   }

   /*!
    * Loads existing valid settings data from persistent storage.
    *
    * All previously existing settings items stored in internal memory
    * structures will be destroyed and deallocated before loading new data.
    *
    * \note This function is thread-safe.
    *
    * \note Overriding implementations of this function must call the base
    * class implementation.
    */
   virtual void Load();

   /*!
    * Writes all settings key/value pairs stored in this object to persistent
    * file storage.
    *
    * \note This function is thread-safe.
    *
    * \note Overriding implementations of this function must call the base
    * class implementation.
    */
   virtual void Save() const;

   /*!
    * Destroys and deallocates all existing settings items, including all items
    * currently in internal memory data structures as well as all items stored
    * in persistent storage. This yields an empty settings file associated with
    * this object.
    *
    * \note This function is thread-safe.
    *
    * \note Overriding implementations of this function must call the base
    * class implementation.
    */
   virtual void Purge();

protected:

           AutoPointer<SettingsTree> m_root;
           IsoString                 m_identifier;
           String                    m_filePath;
           bool                      m_minifyXML = true;
           bool                      m_compressBlocks = true;
           unsigned                  m_compressionThreshold = 4096;
   mutable Mutex                     m_mutex;
   mutable int                       m_modifiedCount = 0;

   static IsoStringList TokenizedKey( const IsoString& );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SettingsData_h

// ----------------------------------------------------------------------------
// EOF pcl/SettingsData.h - Released 2023-12-01T19:15:45Z
