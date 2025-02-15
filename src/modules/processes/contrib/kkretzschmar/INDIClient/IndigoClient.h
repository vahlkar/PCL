//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// IndigoClient.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2024 Klaus Kretzschmar
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

#ifndef INDIGOCLIENT_H
#define INDIGOCLIENT_H

#include "IndigoDefs.h"

#include <indigo/indigo_client.h>
#include <indigo/indigo_names.h>

#include <functional>
#include <iostream>
#include <set>
#include <string>

class IndigoClient
{
public:
  enum class ReturnCode
  {
    INDIGO_OK,
    INDIGO_DUPLICATED,
    INDIGO_ERROR
  };

   explicit IndigoClient( const char* clientName );
   IndigoClient( const char* clientName, const char* host, int32_t port );
   IndigoClient( const IndigoClient& ) = delete;
   IndigoClient( IndigoClient& ) = delete;

   ~IndigoClient(); // do not make it virtual, no vtable allowed

   ReturnCode connectServer( std::ostream& errMessage, uint32_t interfaceIndex );
   bool disconnectServer(uint32_t interfaceIndex);
   bool serverIsConnected( std::ostream& errMessage, uint32_t interfaceIndex ) const;
   void attach();
   void detach();

   bool connectDevice( const std::string& deviceName );
   bool disconnectDevice( const std::string& deviceName );
   bool sendNewProperty( indigo_property* property );
   bool sendNewNumberProperty( char* deviceName, const char* propertyName, size_t numOfItems, const char** items, const double* values );
   bool sendNewSwitchProperty( char* deviceName, const char* propertyName, size_t numOfItems, const char** items, const bool* values );
   bool sendNewTextProperty( char* deviceName, const char* propertyName, size_t numOfItems, const char** items, const char** values );
   bool loadDeviceDriver( const std::string& driver );

   std::function<void( const std::string& )> newDevice = []( const std::string& ) {};
   std::function<void( const std::string& )> removeDevice = []( const std::string& ) {};
   std::function<void( indigo_property* )> newProperty = []( indigo_property* ) {};
   std::function<void( indigo_property* )> removeProperty = []( indigo_property* ) {};
   std::function<void( indigo_property* )> newSwitch = []( indigo_property* ) {};
   std::function<void( indigo_property* )> newNumber = []( indigo_property* ) {};
   std::function<void( indigo_property* )> newText = []( indigo_property* ) {};
   std::function<void( indigo_property* )> newLight = []( indigo_property* ) {};
   std::function<void( indigo_property* )> newBlob = []( indigo_property* ) {};
   std::function<void( const char*, int )> newMessage = []( const char*, int severity ) {};

private:

   typedef indigo_result ( *clientAttachCallBack_t )( indigo_client* );
   typedef indigo_result ( *definePropertyCallBack_t )( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   typedef indigo_result ( *deletePropertyCallBack_t )( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   typedef indigo_result ( *updatePropertyCallBack_t )( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   typedef indigo_result ( *getMessageCallback_t )( indigo_client* client, indigo_device* device, const char* message );

   struct indigo_client m_indigoClient = {
      "PixInsight", false, nullptr, INDIGO_OK, INDIGO_VERSION_LEGACY, nullptr,
      reinterpret_cast<clientAttachCallBack_t>( &IndigoClient::clientAttach ),
      reinterpret_cast<definePropertyCallBack_t>( &IndigoClient::defineProperty ),
      reinterpret_cast<updatePropertyCallBack_t>( &IndigoClient::updateProperty ),
      reinterpret_cast<deletePropertyCallBack_t>( &IndigoClient::deleteProperty ),
      reinterpret_cast<getMessageCallback_t>( &IndigoClient::getMessage ),
      nullptr
   };

   std::string                    m_serverHost;
   uint32_t                       m_port;
   indigo_server_entry*           m_serverEntry = nullptr;
#ifndef __PCL_WINDOWS
   std::set<indigo_driver_entry*> m_devices;
#endif

   static indigo_result clientAttach( indigo_client* client );
   static indigo_result defineProperty( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   static indigo_result deleteProperty( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   static indigo_result updateProperty( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   static indigo_result getMessage( indigo_client* client, indigo_device* device, const char* message );
};

#endif // INDIGOCLIENT_H

// ----------------------------------------------------------------------------
// EOF IndigoClient.h - Released 2024-12-28T16:54:15Z
