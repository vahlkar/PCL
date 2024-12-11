//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// Standard APASS Process Module Version 1.1.5
// ----------------------------------------------------------------------------
// APASSProcess.h - Released 2024-12-11T17:43:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard APASS PixInsight module.
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

#ifndef __APASSProcess_h
#define __APASSProcess_h

#include <pcl/APASSDatabaseFile.h>
#include <pcl/MetaProcess.h>
#include <pcl/Mutex.h>
#include <pcl/ReferenceArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class APASSProcess : public MetaProcess
{
public:

   APASSProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool PrefersGlobalExecution() const override;
   bool CanProcessCommandLines() const override;
   int ProcessCommandLine( const StringList& ) const override;

   bool CanProcessIPCMessages() const override;
   void IPCStart( int instance, const IsoString& messageUID, const String& parameters ) const override;
   void IPCStop( int instance, const IsoString& messageUID ) const override;
   void IPCSetParameters( int instance, const IsoString& messageUID, const String& parameters ) const override;
   int IPCStatus( int instance, const IsoString& messageUID ) const override;

   void SetDatabaseFilePaths( int dr, const StringList& );
   const StringList& DatabaseFilePaths( int dr ) const;

   bool IsValid( int dr ) const;
   bool HasDatabaseFiles( int dr ) const;

   void EnsureDatabasesInitialized( int dr, int verbosity = 0 ); // thread-safe
   void CloseDatabases( int dr );
   const ReferenceArray<APASSDatabaseFile>& Databases( int dr ) const;
   float MagnitudeLow( int dr ) const;
   float MagnitudeHigh( int dr ) const;

   bool PreferencesLoaded( int dr ) const;
   void LoadPreferences( int dr );
   void SavePreferences( int dr );

private:

   class IPCWorker
   {
   public:

      IPCWorker() = default;
      IPCWorker( const IPCWorker& ) = delete;

      void Start( const String& parameters );
      void Stop();
      int Status() const;
      bool IsRunning() const;
      bool IsError() const;

   private:

      String        m_task;
      pcl_enum      m_dataRelease;
      StringList    m_xpsdFilePaths;
      int           m_verbosity; // 0=silent 1=minimal 2=full
      bool          m_running = false;
      bool          m_error = false;
      mutable Mutex m_mutex;

      void GetParameters( const String& );
      void SetRunning();
      void SetNotRunning();
      void SetError();
      void ClearData();
      void ClearStatus();
   };

   mutable IPCWorker m_ipcWorker;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern APASSProcess* TheAPASSProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __APASSProcess_h

// ----------------------------------------------------------------------------
// EOF APASSProcess.h - Released 2024-12-11T17:43:17Z
