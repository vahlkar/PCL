//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.4
// ----------------------------------------------------------------------------
// pcl/AutoStatusCallbackRestorer.h - Released 2023-06-12T18:01:05Z
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

#ifndef __PCL_AutoStatusCallbackRestorer_h
#define __PCL_AutoStatusCallbackRestorer_h

/// \file pcl/AutoStatusCallbackRestorer.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/StatusMonitor.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class AutoStatusCallbackRestorer
 * \brief Automatic recovery of status monitoring callbacks.
 *
 * %AutoStatusCallbackRestorer simplifies working with different status
 * monitoring callback objects, including objects allocated as automatic
 * variables (i.e., inside functions without the static qualifier), ensuring
 * that the initial monitoring callback will be recovered when the instance
 * gets out of scope.
 *
 * An instance of %AutoStatusCallbackRestorer stores a pointer to the
 * StatusCallback object being used by a StatusMonitor instance upon
 * construction, and restores it upon destruction. This ensures that the
 * %StatusMonitor object will always be left in a valid state, even in critical
 * situations involving multiple function return points and exceptions.
 *
 * Consider the following example:
 *
 * \code
 * void Foo( Image& image )
 * {
 *    StatusCallback* oldStatus = image.StatusCallback();
 *    StandardStatus status;
 *    image.SetStatusCallback( &status );
 *    image.Status().Initialize( "Performing Foo()", image.NumberOfPixels() );
 *    image.Status().DisableInitialization();
 *
 *    try
 *    {
 *       DoFoo( image );
 *       image.SetStatusCallback( oldStatus );
 *    }
 *    catch ( ... )
 *    {
 *       image.SetStatusCallback( oldStatus );
 *       throw;
 *    }
 * }
 * \endcode
 *
 * Since \c status is an automatic variable, we cannot return from the Foo()
 * function without removing it from image.Status(), because otherwise the
 * StatusMonitor object would be referencing a dangling pointer. For this
 * reason we have to be careful to restore the \c oldStatus pointer before
 * returning from Foo(), including any possible returning points and all
 * exceptions thrown.
 *
 * With %AutoStatusCallbackRestorer the above code can be simplified to:
 *
 * \code
 * void Foo( Image& image )
 * {
 *    AutoStatusCallbackRestorer saveStatus( image.Status() );
 *    StandardStatus status;
 *    image.SetStatusCallback( &status );
 *    image.Status().Initialize( "Performing Foo()", image.NumberOfPixels() );
 *    image.Status().DisableInitialization();
 *    DoFoo( image );
 * }
 * \endcode
 *
 * Note that the try-catch blocks are now unnecessary. As soon as the
 * \c saveStatus variable gets out of scope, the \c image.Status() object will
 * use its previous status callback again, that is, the same status callback
 * that it was using before calling Foo(). This will happen automatically,
 * including both normal function returns and uncaught exceptions within the
 * Foo() function.
 *
 * \sa StatusMonitor, StatusCallback
 */
class PCL_CLASS AutoStatusCallbackRestorer
{
public:

   /*!
    * Constructs an %AutoStatusCallbackRestorer object for the specified
    * client status \a monitor.
    *
    * \param monitor    Reference to the client StatusMonitor instance. This
    *                   object will store a pointer to the StatusCallback
    *                   object currently being used by the client \a monitor.
    *
    * \warning The specified \a monitor object must remain valid while this
    * object exists. Otherwise undefined behavior will be invoked when this
    * object is destroyed.
    */
   explicit AutoStatusCallbackRestorer( StatusMonitor& monitor )
      : m_monitor( monitor )
   {
      Store();
   }

   /*!
    * Destroys this %AutoStatusCallbackRestorer object.
    *
    * This destructor restores the status callback being used by the client
    * status monitor when this object was constructed, or upon the last call to
    * Store().
    */
   ~AutoStatusCallbackRestorer()
   {
      Restore();
   }

   /*!
    * Restores the status callback that was being used by the client status
    * monitor when this object was constructed, or upon the last call to
    * Store().
    */
   void Restore()
   {
      m_monitor.m_callback = m_callback;
   }

   /*!
    * Stores the status callback currently used by the client status monitor.
    */
   void Store()
   {
      m_callback = m_monitor.m_callback;
   }

   /*!
    * Returns a reference to the client status monitor object. The client
    * status monitor has been specified upon construction of this instance and
    * cannot be changed.
    */
   StatusMonitor& Monitor() const
   {
      return m_monitor;
   }

   /*!
    * Returns the address of the status callback object currently stored by
    * this instance. This is a pointer to a StatusCallback object that was
    * acquired when this object was constructed, or in the last call to the
    * Store() member function.
    */
   StatusCallback* Callback() const
   {
      return m_callback;
   }

   /*!
    * Copy constructor. This constructor is disabled.
    */
   AutoStatusCallbackRestorer( const AutoStatusCallbackRestorer& ) = delete;

   /*!
    * Copy assignment. This constructor is disabled.
    */
   AutoStatusCallbackRestorer& operator =( const AutoStatusCallbackRestorer& ) = delete;

private:

   StatusMonitor&  m_monitor;
   StatusCallback* m_callback = nullptr;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_AutoStatusCallbackRestorer_h

// ----------------------------------------------------------------------------
// EOF pcl/AutoStatusCallbackRestorer.h - Released 2023-06-12T18:01:05Z
