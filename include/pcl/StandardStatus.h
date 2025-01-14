//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/StandardStatus.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_StandardStatus_h
#define __PCL_StandardStatus_h

/// \file pcl/StandardStatus.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Console.h>
#include <pcl/StatusMonitor.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class StandardStatus
 * \brief A status monitoring callback that sends progress information to the
 *        process console.
 *
 * %StandardStatus uses the processing console associated with the current
 * processing thread to provide feedback and progress information on a
 * process being monitored by a StatusMonitor object.
 *
 * %StandardStatus is suitable for processes where a total <em>status
 * monitoring count</em> can be computed in advance. For example, this happens
 * with processes that operate on a pixel-by-pixel basis and perform a fixed
 * number of iterations, or a number of iterations that can be predicted before
 * running the process. For cases where a total count is not available or
 * cannot be computed, the SpinStatus class is more suitable.
 *
 * %StandardStatus and %SpinStatus are used by all standard PixInsight
 * processes and file formats. Unless a specific module has an extremely good
 * reason to proceed otherwise, we recommend that all modules also use these
 * monitoring classes. This ensures that efficient and coherent feedback is
 * provided to the user on the entire PixInsight platform.
 *
 * In addition, using %StandardStatus (and also %SpinStatus) allows the
 * PixInsight core application to process mouse, keyboard and screen update
 * events. In this way the application's GUI can be responsive if the user
 * requests aborting or suspending the monitored process.
 *
 * \sa StatusCallback, StatusMonitor, Console, SpinStatus, ProgressBarStatus,
 * MuteStatus
 */
class PCL_CLASS StandardStatus : public StatusCallback
{
public:

   /*!
    * Constructs a default %StandardStatus object.
    */
   StandardStatus() = default;

   /*!
    * Copy constructor.
    */
   StandardStatus( const StandardStatus& x )
      : StatusCallback( x )
      , m_last( x.m_last )
   {
   }

   /*!
    * Move constructor.
    */
   StandardStatus( StandardStatus&& x )
      : StatusCallback( std::move( x ) )
      , m_console( std::move( x.m_console ) )
      , m_last( x.m_last )
      , m_thread( x.m_thread )
   {
      x.m_thread = nullptr;
   }

   /*!
    * Destroys a %StandardStatus instance.
    */
   ~StandardStatus() override
   {
   }

   /*!
    * Copy constructor. Returns a reference to this object.
    */
   StandardStatus& operator =( const StandardStatus& x )
   {
      (void)StatusCallback::operator =( x );
      m_last = x.m_last;
      return *this;
   }

   /*!
    * Move constructor. Returns a reference to this object.
    */
   StandardStatus& operator =( StandardStatus&& x )
   {
      if ( this != &x )
      {
         (void)StatusCallback::operator =( std::move( x ) );
         m_console = std::move( x.m_console );
         m_last = x.m_last;
         m_thread = x.m_thread;
         x.m_thread = nullptr;
      }
      return *this;
   }

   /*!
    * This function is called by a status \a monitor object when a new
    * monitored process is about to start.
    */
   int Initialized( const StatusMonitor& monitor ) const override;

   /*!
    * Function called by a status \a monitor object to signal an update of the
    * progress count for the current process.
    */
   int Updated( const StatusMonitor& monitor ) const override;

   /*!
    * Function called by a status \a monitor object to signal that the current
    * process has finished.
    */
   int Completed( const StatusMonitor& monitor ) const override;

   /*!
    * Function called by a status \a monitor object when the progress
    * information for the current process has been changed.
    */
   void InfoUpdated( const StatusMonitor& monitor ) const override;

private:

   mutable pcl::Console m_console;
   mutable int          m_last = -1;
   mutable void*        m_thread = nullptr;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_StandardStatus_h

// ----------------------------------------------------------------------------
// EOF pcl/StandardStatus.h - Released 2024-12-28T16:53:48Z
