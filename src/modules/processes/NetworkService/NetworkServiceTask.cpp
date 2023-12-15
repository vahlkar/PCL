//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceTask.cpp - Released 2023-12-01T19:16:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
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

#include "NetworkServiceDownloader.h"
#include "NetworkServiceLogger.h"
#include "NetworkServiceProcess.h"
#include "NetworkServiceTask.h"

#include <pcl/Exception.h>
#include <pcl/NetworkTransfer.h>
#include <pcl/ReferenceSortedArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static ReferenceSortedArray<NetworkServiceTask> s_tasks;

// ----------------------------------------------------------------------------

class NetworkServiceTaskByType : public NetworkServiceTask
{
public:

   NetworkServiceTaskByType( const String& name )
      : m_name( name )
   {
   }

   String TaskType() const override
   {
      return m_name;
   }

   String TaskVersion() const override
   {
      return String();
   }

   void Perform( const TaskData&, NetworkServiceLogger& ) override
   {
   }

private:

   String m_name;
};

// ----------------------------------------------------------------------------

String NetworkServiceTask::TaskType() const
{
   return "DEFAULT_TASK";
}

// ----------------------------------------------------------------------------

String NetworkServiceTask::TaskVersion() const
{
   return "1.0";
}

// ----------------------------------------------------------------------------

void NetworkServiceTask::Perform( const TaskData& data, NetworkServiceLogger& log )
{
   /*
    * This is the default task, which simply sends back a notification to
    * identify the task for testing purposes.
    *
    * You must reimplement this member function in derived classes to implement
    * actual network services.
    */

   StringList parameters;
   data.parameters.Break( parameters, '\n', true/*trim*/ );
   parameters.Remove( String() );

   log.Line( "Task parameters :" );
   log.Line( String().ToNewLineSeparated( parameters ) );

   log.Section( "Uploading result: " + data.callbackURL );
   {
      String postVars;
      postVars << "serialNumber=" << data.serialNumber
               << "&taskType=" << TaskType()
               << "&workerName=" << (data.workerName.IsEmpty() ? "anonymous" : data.workerName)
               << "&status=OK"
               << "&taskVersion=" << TaskVersion();

      NetworkTransfer transfer;
      transfer.SetURL( data.callbackURL );
      transfer.SetSSL( true/*enableSSL*/, false/*forceSSL*/ );

      NetworkServiceDownloader download;
      download.Attach( transfer );

      if ( !transfer.POST( postVars ) )
         if ( !TheNetworkServiceProcess->StopRequested() )
            throw Error( "POST operation failed: " + transfer.ErrorInformation() );

      if ( !TheNetworkServiceProcess->StopRequested() )
         log.Section( "POST operation successful. Callback result:\n" + IsoString::ToURLDecoded( download.data ).UTF8ToUTF16() );
   }
}

// ----------------------------------------------------------------------------

NetworkServiceTask* NetworkServiceTask::TaskByType( const String& taskName )
{
   ReferenceSortedArray<NetworkServiceTask>::const_iterator i = s_tasks.Search( NetworkServiceTaskByType( taskName ) );
   return (i == s_tasks.End()) ? nullptr : const_cast<NetworkServiceTask*>( (const NetworkServiceTask*)i );
}

// ----------------------------------------------------------------------------

void NetworkServiceTask::RegisterTask( NetworkServiceTask* task )
{
   if ( task != nullptr )
      if ( !s_tasks.Contains( *task ) )
         s_tasks.Add( task );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NetworkServiceTask.cpp - Released 2023-12-01T19:16:18Z
