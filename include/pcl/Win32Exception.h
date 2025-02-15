//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Win32Exception.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_Win32Exception_h
#define __PCL_Win32Exception_h

/// \file pcl/Win32Exception.h

#if !defined( __PCL_WINDOWS ) || defined( __PCL_LINUX ) || defined( __PCL_FREEBSD ) || defined( __PCL_MACOSX )
#  error Win32Exception can only be used on MS Windows platforms.
#endif

#include <pcl/Defs.h>

#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Win32Exception
 * \brief A Win32 structured exception handler that throws C++ exceptions.
 *
 * This class can only be used on Microsoft Windows platforms. On the rest of
 * supported platforms, including this header file from compilable code raises
 * a compiler error.
 *
 * For this handler to work properly, all code that may raise asynchronous
 * (structured) exceptions must be compiled with the '-EHa' Visual C/C++
 * compiler option. Otherwise no asynchronous exception thrown in a try{} block
 * will be caught by the corresponding catch{} block, and the program will
 * terminate.
 */
class PCL_CLASS Win32Exception : public pcl::Exception
{
public:

   /*!
    * Represents a memory address associated with an exception. For example,
    * the address that has been accessed to cause an access violation error.
    */
   using exception_address = const void*;

   /*!
    * Represents a pointer to a data block that describes an exception. This is
    * an opaque pointer to an EXCEPTION_RECORD structure.
    */
   using exception_data_pointer = const void*;

   /*!
    * The type of an exception error code.
    */
   using exception_code = unsigned;

   /*!
    * Constructs a new %Win32Exception object with the specified exception
    * \a code and \a data, plus optional backtrace information \a details.
    */
   Win32Exception( exception_code code, exception_data_pointer data,
                   const IsoString& details = IsoString() )
      : m_code( code )
      , m_data( data )
      , m_details( details )
   {
   }

   /*!
    * Copy constructor.
    */
   Win32Exception( const Win32Exception& ) = default;

   /*!
    * Returns the memory address associated with this exception.
    */
   exception_address ExceptionAddress() const;

   /*!
    * Returns the error code of this exception.
    */
   exception_code ExceptionCode() const
   {
      return m_code;
   }

   /*!
    * Returns the backtrace information associated with this exception.
    *
    * The returned string will be empty if no backtrace data were available at
    * the time this exception was generated.
    */
   const IsoString& Details() const
   {
      return m_details;
   }

   /*!
    * Returns an error or warning message corresponding to this exception.
    * For example, some typical messages are "access violation", "stack
    * overflow" and "illegal instruction", returned by specific derived
    * classes.
    */
   String Message() const override
   {
      return "Undefined system exception";
   }

   /*!
    * Returns a formatted error message with information on this exception.
    */
   String FormatInfo() const override
   {
      String info = String().Format( "At address %p with exception code %X :\n",
                                     ExceptionAddress(), ExceptionCode() ) + Message();
      if ( !m_details.IsEmpty() )
      {
         info.Append( '\n' );
         info.Append( m_details );
      }
      return info;
   }

   /*!
    * Returns the type of this exception, intended to be used as a caption for
    * a message box. As reimplemented in this class, this member function
    * returns the string "PCL Win32 System Exception".
    */
   String Caption() const override
   {
      return "PCL Win32 System Exception";
   }

   /*!
    * Writes a formatted representation of this exception on the platform
    * console. A plain text version of the same textual representation will
    * also be written on stdout; however, if there is no console currently
    * attached to the process (which is the typical situation for GUI Windows
    * applications), no console stream output will be generated.
    *
    * \note Asynchronous exceptions are never reported on interactive graphical
    * interfaces, such as message boxes, irrespective of global platform
    * settings or local settings defined through calls to EnableGUIOutput() and
    * similar functions.
    */
   void Show() const override;

   /*!
    * Initializes the structured exception handler. This static member function
    * must be called before the calling process can raise any system exception.
    *
    * \note A module should never call this member function. It is invoked when
    * appropriate by the PixInsight core application and internal PCL routines.
    */
   static void Initialize();

protected:

   exception_code         m_code;
   exception_data_pointer m_data;    // points to an EXCEPTION_RECORD structure
   IsoString              m_details; // backtrace information
};

// ----------------------------------------------------------------------------

class PCL_CLASS Win32AccessViolationException : public Win32Exception
{
public:

   Win32AccessViolationException( exception_code code, exception_data_pointer data,
                                  const IsoString& details = IsoString() )
      : Win32Exception( code, data, details )
   {
   }

   Win32AccessViolationException( const Win32AccessViolationException& ) = default;

   String Message() const override;
};

// ----------------------------------------------------------------------------

#define DECLARE_WIN32_EXCEPTION( className, message )                         \
   class PCL_CLASS className : public pcl::Win32Exception                     \
   {                                                                          \
   public:                                                                    \
      className( exception_code code, exception_data_pointer data,            \
                 const IsoString& details = IsoString() ) :                   \
         pcl::Win32Exception( code, data, details )                           \
      {                                                                       \
      }                                                                       \
      className( const className& ) = default;                                \
      String Message() const override                                         \
      {                                                                       \
         return message;                                                      \
      }                                                                       \
   }

// ----------------------------------------------------------------------------

DECLARE_WIN32_EXCEPTION( EWin32ArrayBoundsExceeded,
   "Array bounds exceeded" );

DECLARE_WIN32_EXCEPTION( EWin32Breakpoint,
   "A breakpoint was encountered" );

DECLARE_WIN32_EXCEPTION( EWin32DataMisalignment,
   "Invalid read/write operation on misaligned data" );

DECLARE_WIN32_EXCEPTION( EWin32FloatingPointDenormalOperand,
   "Denormal operand in floating-point operation" );

DECLARE_WIN32_EXCEPTION( EWin32FloatingPointDivideByZero,
   "Division by zero in floating-point operation" );

DECLARE_WIN32_EXCEPTION( EWin32FloatingPointInexactResult,
   "Inexact result in floating-point operation" );

DECLARE_WIN32_EXCEPTION( EWin32FloatingPointInvalidOperation,
   "Invalid floating-point operation" );

DECLARE_WIN32_EXCEPTION( EWin32FloatingPointOverflow,
   "Overflow in floating-point operation" );

DECLARE_WIN32_EXCEPTION( EWin32FloatingPointStackCheck,
   "Stack limits exceeded in floating-point operation" );

DECLARE_WIN32_EXCEPTION( EWin32FloatingPointUnderflow,
   "Underflow in floating-point operation" );

DECLARE_WIN32_EXCEPTION( EWin32IllegalInstruction,
   "Illegal processor instruction" );

DECLARE_WIN32_EXCEPTION( EWin32PageError,
   "Memory page not found" );

DECLARE_WIN32_EXCEPTION( EWin32DivideByZero,
   "Integer division by zero" );

DECLARE_WIN32_EXCEPTION( EWin32Overflow,
   "Integer overflow" );

DECLARE_WIN32_EXCEPTION( EWin32InvalidDisposition,
   "Invalid exception disposition" );

DECLARE_WIN32_EXCEPTION( EWin32NonContinuableException,
   "Noncontinuable exception " );

DECLARE_WIN32_EXCEPTION( EWin32PrivilegedInstruction,
   "Privileged processor instruction" );

DECLARE_WIN32_EXCEPTION( EWin32SingleStep,
   "Single-instruction step" );

DECLARE_WIN32_EXCEPTION( EWin32StackOverflow,
   "Stack overflow" );

// ----------------------------------------------------------------------------

#undef DECLARE_WIN32_EXCEPTION

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Win32Exception_h

// ----------------------------------------------------------------------------
// EOF pcl/Win32Exception.h - Released 2024-12-28T16:53:48Z
