//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.6
// ----------------------------------------------------------------------------
// pcl/FileInfo.h - Released 2023-07-06T16:53:21Z
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

#ifndef __PCL_FileInfo_h
#define __PCL_FileInfo_h

/// \file pcl/FileInfo.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Flags.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::FileAttribute
 * \brief File attributes
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td colspan="2"><b>File type</b></td></tr>
 * <tr><td>FileAttribute::Block</td>           <td>Block special</td></tr>
 * <tr><td>FileAttribute::Character</td>       <td>Character special</td></tr>
 * <tr><td>FileAttribute::FIFO</td>            <td>FIFO special</td></tr>
 * <tr><td>FileAttribute::Regular</td>         <td>Regular file</td></tr>
 * <tr><td>FileAttribute::Directory</td>       <td>Directory</td></tr>
 * <tr><td>FileAttribute::SymbolicLink</td>    <td>Symbolic link</td></tr>
 * <tr><td>FileAttribute::Socket</td>          <td>Socket</td></tr>
 * <tr><td>FileAttribute::FileType</td>        <td>Mask to isolate file type flags</td></tr>
 * <tr><td colspan="2"><b>File attributes</b>\n
 *                    <em>These are Windows-exclusive, except ReadOnly and Hidden, which we emulate on UNIX and Linux platforms.</em></td></tr>
 * <tr><td>FileAttribute::Archive</td>         <td>File is archived</td></tr>
 * <tr><td>FileAttribute::Compressed</td>      <td>File is compressed</td></tr>
 * <tr><td>FileAttribute::Encrypted</td>       <td>File is encrypted</td></tr>
 * <tr><td>FileAttribute::Hidden</td>          <td>File is hidden</td></tr>
 * <tr><td>FileAttribute::ReadOnly</td>        <td>File is read-only</td></tr>
 * <tr><td>FileAttribute::System</td>          <td>File is a system file</td></tr>
 * <tr><td>FileAttribute::Temporary</td>       <td>File is a temporary file</td></tr>
 * <tr><td>FileAttribute::FileAttributes</td>  <td>Mask to isolate file attributes flags</td></tr>
 * <tr><td colspan="2"><b>File permissions</b></td></tr>
 * <tr><td>FileAttribute::Read</td>            <td>Owner can read</td></tr>
 * <tr><td>FileAttribute::Write</td>           <td>Owner can write</td></tr>
 * <tr><td>FileAttribute::Execute</td>         <td>Owner can execute/search</td></tr>
 * <tr><td>FileAttribute::ReadGroup</td>       <td>Group can read</td></tr>
 * <tr><td>FileAttribute::WriteGroup</td>      <td>Group can write</td></tr>
 * <tr><td>FileAttribute::ExecuteGroup</td>    <td>Group can execute/search</td></tr>
 * <tr><td>FileAttribute::ReadOthers</td>      <td>Others can read</td></tr>
 * <tr><td>FileAttribute::WriteOthers</td>     <td>Others can write</td></tr>
 * <tr><td>FileAttribute::ExecuteOthers</td>   <td>Others can execute/search</td></tr>
 * <tr><td>FileAttribute::FilePermissions</td> <td>Mask to isolate file permission flags</td></tr>
 * </table>
 *
 * \ingroup file_utilities
 */
namespace FileAttribute
{
   enum mask_type
   {
      /*
       * File type.
       */
      Block          = 0x00000001,  // Block special
      Character      = 0x00000002,  // Character special
      FIFO           = 0x00000004,  // FIFO special
      Regular        = 0x00000008,  // Regular file
      Directory      = 0x00000010,  // Directory
      SymbolicLink   = 0x00000020,  // Symbolic link
      Socket         = 0x00000040,  // Socket
      FileType       = 0x000000FF,

      /*
       * File attributes.
       * These are Windows-exclusive, except ReadOnly and Hidden, which we
       * emulate on UNIX and Linux.
       */
      Archive        = 0x00001000,  // File is archived
      Compressed     = 0x00002000,  // File is compressed
      Encrypted      = 0x00004000,  // File is encrypted
      Hidden         = 0x00008000,  // File is hidden
      ReadOnly       = 0x00010000,  // File is read-only
      System         = 0x00020000,  // File is a system file
      Temporary      = 0x00040000,  // File is a temporary file
      FileAttributes = 0x000FF000,

      /*
       * File permissions.
       */
      Read           = 0x00100000,  // Owner can read
      Write          = 0x00200000,  // Owner can write
      Execute        = 0x00400000,  // Owner can execute/search
      ReadGroup      = 0x01000000,  // Group can read
      WriteGroup     = 0x02000000,  // Group can write
      ExecuteGroup   = 0x04000000,  // Group can execute/search
      ReadOthers     = 0x10000000,  // Others can read
      WriteOthers    = 0x20000000,  // Others can write
      ExecuteOthers  = 0x40000000,  // Others can execute/search
      FilePermissions= 0xFFF00000
   };
}

/*!
 * \class pcl::FileAttributes
 * \brief A combination of file type, attribute and access mode flags
 * \ingroup file_utilities
 */
using FileAttributes = Flags<FileAttribute::mask_type>;

// ----------------------------------------------------------------------------

/*!
 * \struct FileTime
 * \brief File date and time
 *
 * %FileTime is used to represent the creation, last access and last
 * modification times of a file or directory. On most file systems, this object
 * represents a time point in the Coordinated Universal Time (UTC) time scale.
 * \ingroup file_utilities
 */
struct FileTime
{
   uint16   year         : 16; //!< Year
   uint8    month        :  4; //!< Month [1,12]
   uint8    day          :  5; //!< Day [1,31]
   uint8    hour         :  5; //!< Hour [0,23]
   int                   :  2;
   uint8    minute       :  6; //!< Minute [0,59]
   uint8    second       :  6; //!< Second [0,59]
   int                   :  4;
   uint16   milliseconds : 10; //!< Milliseconds [0,999]
   int                   :  6;

   /*!
    * Returns true iff this file time is equal to another object \a t.
    */
   bool operator ==( const FileTime& t ) const
   {
      return second == t.second && minute == t.minute && hour == t.hour &&
             day == t.day && month == t.month && year == t.year &&
             milliseconds == t.milliseconds;
   }

   /*!
    * Returns true iff this file time precedes another object \a t.
    */
   bool operator <( const FileTime& t ) const
   {
      if ( year != t.year )
         return year < t.year;
      if ( month != t.month )
         return month < t.month;
      if ( day != t.day )
         return day < t.day;
      if ( hour != t.hour )
         return hour < t.hour;
      if ( minute != t.minute )
         return minute < t.minute;
      if ( second != t.second )
         return second < t.second;
      if ( milliseconds != t.milliseconds )
         return milliseconds < t.milliseconds;
      return false;
   }

   /*!
    * Returns this file date and time as a Julian day number. The returned
    * value represents a time point in the UTC time scale on most file systems.
    */
   double ToJD() const;

   /*!
    * Returns the elapsed time in days since the standard UNIX epoch (1970
    * January 1.0 UTC).
    */
   double DaysSinceEpoch() const
   {
      return ToJD() - 2440587.5;
   }

   /*!
    * Returns the elapsed time in seconds since the standard UNIX epoch (1970
    * January 1.0 UTC).
    */
   double SecondsSinceEpoch() const
   {
      return 86400*DaysSinceEpoch();
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class FileInfo
 * \brief Platform-independent information on file and directory items.
 *
 * ### TODO: Write a detailed description for %FileInfo.
 * \ingroup file_utilities
 */
class PCL_CLASS FileInfo
{
public:

   /*!
    * Default constructor. Constructs an empty %FileInfo object without any
    * information on existing file or directory items.
    */
   FileInfo()
   {
      Clear();
   }

   /*!
    * Constructs a %FileInfo object with information retrieved for the
    * specified file or directory \a path.
    */
   FileInfo( const String& path )
   {
      Refresh( path );
   }

   /*!
    * Copy constructor.
    */
   FileInfo( const FileInfo& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~FileInfo()
   {
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   FileInfo& operator =( const FileInfo& ) = default;

   /*!
    * Returns the absolute path of this file or directory.
    */
   const String& Path() const
   {
      return m_path;
   }

   /*!
    * Returns the Windows drive component of the path in this object, or an
    * empty string if the path does not include a Windows drive specification.
    *
    * This function only makes sense on Windows. On UNIX and Linux operating
    * systems, this function always returns an empty string.
    */
   String Drive() const;

   /*!
    * Returns the parent directory component of the path in this object, or an
    * empty string if the path does not include a parent directory.
    *
    * On Windows, the drive component of the path is \e not included.
    *
    * Examples:
    *
    * \li In '/foo.tar.gz' the directory is '/'.
    * \li In '/foo/bar.tar.gz' the directory is '/foo'.
    * \li In 'foo/bar.tar.gz' the directory is 'foo'.
    * \li In 'C:/Foo/Bar.txt' the directory is '/Foo'.
    */
   String Directory() const;

   /*!
    * Returns the file name component of the path in this object, or an empty
    * string if the path does not include a name.
    *
    * The file extension or suffix is \e not included.
    *
    * Examples:
    *
    * \li In '/foo/bar.tar.gz' the name is 'bar'.
    * \li In 'C:/Foo/Bar.txt' the name is 'Bar'.
    */
   String Name() const;

   /*!
    * Returns the file extension component of the path in this object, or an
    * empty string if the path does not include an extension.
    *
    * The returned extension \e includes the initial dot separator.
    *
    * Examples:
    *
    * \li In '/foo/bar.tar.gz' the extension is '.gz'.
    * \li In 'C:/Foo/Bar.txt' the extension is '.txt'.
    */
   String Extension() const;

   /*!
    * A synonym for Extension().
    */
   String Suffix() const
   {
      return Extension();
   }

   /*!
    * Returns the complete suffix of the path in this object, or an empty
    * string if the path does not include a suffix.
    *
    * The returned string \e includes the initial dot separator.
    *
    * The complete suffix is the rightmost substring of the path, starting with
    * the first occurrence of a dot character. For example, in 'foo.tar.gz' the
    * complete suffix is '.tar.gz'.
    */
   String CompleteSuffix() const;

   /*!
    * Returns the name and extension of the path in this object. Calling this
    * member function is functionally equivalent to: Name() + Extension().
    */
   String NameAndExtension() const;

   /*!
    * A synonym for NameAndExtension().
    */
   String NameAndSuffix() const
   {
      return NameAndExtension();
   }

   /*!
    * Returns true iff this object represents an existing file or directory.
    */
   bool Exists() const
   {
      return m_exists;
   }

   /*!
    * Returns a constant reference to the FileAttributes flags for this file or
    * directory.
    */
   const FileAttributes& Attributes() const
   {
      return m_attributes;
   }

   /*!
    * Returns true iff this object represents an existing directory.
    */
   bool IsDirectory() const
   {
      return m_attributes.IsFlagSet( FileAttribute::Directory );
   }

   /*!
    * Returns true iff this object represents an existing file.
    */
   bool IsFile() const
   {
      return m_attributes.IsFlagSet( FileAttribute::Regular );
   }

   /*!
    * Returns true iff this object represents a symbolic link.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns false.
    */
   bool IsSymbolicLink() const
   {
      return m_attributes.IsFlagSet( FileAttribute::SymbolicLink );
   }

   /*!
    * Returns the target path of this symbolic link, or an empty string if this
    * object does not represent a symbolic link.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns an empty string.
    */
   String SymbolicLinkTarget() const;

   /*!
    * Returns true iff this object represents a hidden file or directory.
    *
    * On Windows, this member function returns true if the file or directory
    * represented by this object has the hidden attribute set. On UNIX and
    * Linux operating systems, this function returns true when the file or
    * directory name begins with a dot character.
    */
   bool IsHidden() const
   {
      return m_attributes.IsFlagSet( FileAttribute::Hidden );
   }

   /*!
    * Returns true iff the user id and group id of the caller process have read
    * access permission for the item represented by this object.
    */
   bool IsReadable() const
   {
      return m_readable;
   }

   /*!
    * Returns true iff the user id and group id of the caller process have write
    * access permission for the item represented by this object.
    */
   bool IsWritable() const
   {
      return m_writable;
   }

   /*!
    * Returns true iff the user id and group id of the caller process have
    * execution access permission for the item represented by this object.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns false.
    *
    * On POSIX systems, this function returns true only if the item represented
    * has the S_IXUSR file mode bit set. This means that this function will
    * normally return true for directories, since S_IXUSR/S_IXGRP/S_IXOTH
    * represent standard search permissions for a directory.
    */
   bool IsExecutable() const
   {
      return m_executable;
   }

   /*!
    * Returns the size in bytes of the file or directory item represented by
    * this object.
    *
    * If this object represents a directory, a fixed size (usually 4096 bytes)
    * is always returned on UNIX and Linux systems. On Windows, zero is always
    * returned for directories.
    */
   fsize_type Size() const
   {
      return m_size;
   }

   /*!
    * Returns the number of existing hard links to the file or directory item
    * represented by this object.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns zero.
    */
   int NumberOfHardLinks() const
   {
      return m_numberOfHardLinks;
   }

   /*!
    * Returns the user id of the owner of the file or directory item
    * represented by this object.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns zero.
    */
   int UserId() const
   {
      return m_userId;
   }

   /*!
    * Returns the group id of the owner of the file or directory item
    * represented by this object.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns zero.
    */
   int GroupId() const
   {
      return m_groupId;
   }

   /*!
    * Returns a constant reference to a FileTime structure corresponding to the
    * creation time of this file or directory.
    */
   const FileTime& TimeCreated() const
   {
      return m_created;
   }

   /*!
    * Returns a constant reference to a FileTime structure corresponding to the
    * last access time of this file or directory.
    */
   const FileTime& LastAccessed() const
   {
      return m_lastAccessed;
   }

   /*!
    * Returns a constant reference to a FileTime structure corresponding to the
    * last modification time of this file or directory.
    */
   const FileTime& LastModified() const
   {
      return m_lastModified;
   }

   /*!
    * Retrieves up-to-date information for the item represented by this object.
    */
   void Refresh();

   /*!
    * Retrieves information for the specified \a path.
    */
   void Refresh( const String& path );

   /*!
    * Clears all the information in this object, leaving it in an invalid state
    * corresponding to a nonexistent item.
    */
   void Clear();

private:

   String         m_path;              // full file or directory path
   FileAttributes m_attributes;        // item attributes
   fsize_type     m_size;              // file size in bytes
   int            m_numberOfHardLinks; // number of existing hard links to this file
   int            m_userId;            // user id of the file owner
   int            m_groupId;           // group id of the file owner
   FileTime       m_created;           // creation time
   FileTime       m_lastAccessed;      // time of last access
   FileTime       m_lastModified;      // time of last change
   bool           m_exists;            // item exists
   bool           m_readable;          // caller has read permission
   bool           m_writable;          // caller has write permission
   bool           m_executable;        // caller has execute permission

   // Clears everything but *preserves* m_path
   void ClearInfo();
};

/*!
 * \class pcl::FileInfoList
 * \brief A dynamic array of FileInfo structures
 * \ingroup file_utilities
 */
using FileInfoList = Array<FileInfo>;

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_FileInfo_h

// ----------------------------------------------------------------------------
// EOF pcl/FileInfo.h - Released 2023-07-06T16:53:21Z
