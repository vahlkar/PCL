//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.6
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceProcess.h - Released 2024-01-19T15:23:40Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
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

#ifndef __NetworkServiceProcess_h
#define __NetworkServiceProcess_h

#include <pcl/MetaProcess.h>
#include <pcl/Mutex.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct NetworkServiceLogger;
class PCL_CLASS NetworkTransfer;

class NetworkServiceProcess : public MetaProcess
{
public:

   NetworkServiceProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool IsAssignable() const override;

   bool CanProcessIPCMessages() const override;
   void IPCStart( int instance, const IsoString& messageUID, const String& parameters ) const override;
   void IPCStop( int instance, const IsoString& messageUID ) const override;
   void IPCSetParameters( int instance, const IsoString& messageUID, const String& parameters ) const override;
   int IPCStatus( int instance, const IsoString& messageUID ) const override;

   void Start( const String& parameters, NetworkServiceLogger* = nullptr ) const;
   void Stop() const;

   bool IsRunning() const
   {
      return m_worker.IsRunning();
   }

   bool StopRequested() const
   {
      return m_worker.StopRequested();
   }

private:

   class Worker
   {
   public:

      Worker() = default;
      Worker( const Worker& ) = delete;

      void Start( const String& parameters, NetworkServiceLogger* = nullptr );
      void Stop();
      int Status() const;
      bool IsRunning() const;
      bool IsError() const;
      bool StopRequested() const;

   private:

      NetworkServiceLogger* m_log = nullptr;
      String                m_taskProviderURL;
      String                m_workerName;
      String                m_taskType;
      String                m_userName;
      String                m_userPassword;
      String                m_logFilePath;
      ByteArray             m_downloadData;
      unsigned              m_waitTimeSecs;
      bool                  m_running = false;
      bool                  m_error = false;
      bool                  m_stop = false;
      bool                  m_debug = false;
      mutable Mutex         m_mutex;

      void GetParameters( const String& );
      void ValidateParameters();

      void QueryTask();
      void PerformTask();
      void Wait();

      void SetRunning();
      void SetNotRunning();
      void SetError();
      void ClearStatus();
   };

   mutable Worker m_worker;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern NetworkServiceProcess* TheNetworkServiceProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NetworkServiceProcess_h

// ----------------------------------------------------------------------------
// EOF NetworkServiceProcess.h - Released 2024-01-19T15:23:40Z
