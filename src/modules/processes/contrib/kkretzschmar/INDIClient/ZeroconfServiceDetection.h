//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.15
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ZeroconfServiceDetection.h - Released 2021-10-28T16:39:26Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2021 Klaus Kretzschmar
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

#ifndef ZEROCONFSERVICEDETECTION_H
#define ZEROCONFSERVICEDETECTION_H

#ifdef __PCL_LINUX
  #define WITH_ZEROCONF
#endif

#include <memory>
#include <functional>

#ifdef WITH_ZEROCONF
#include <dns_sd.h>
#include <sys/socket.h>
#endif

#include <pcl/Thread.h>
#include <pcl/SortedArray.h>

#include "ExclusiveAccess.h"

namespace pcl {

struct ZeroConfServiceItem
{
   IsoString     ServiceName;
   IsoString     ServiceType;
   IsoString     Domain;
   IsoString     Host;
   uint32_t      Port;
   uint32_t      InterfaceIndex = 0; // Representing the network interface of the client host
   IsoString     InterfaceName;
   bool          IsWireless = false;

   ZeroConfServiceItem(){}

   ZeroConfServiceItem(const IsoString& serviceName, const IsoString& serviceType, const IsoString& domain, const IsoString& host, uint32_t port, uint32_t interfaceIndex, const IsoString& interfaceName, bool isWireless):
                       ServiceName(serviceName), ServiceType(serviceType), Domain(domain), Host(host), Port(port), InterfaceIndex(interfaceIndex), InterfaceName(interfaceName), IsWireless(isWireless)
   {
   }
   ZeroConfServiceItem(const IsoString& serviceName, const IsoString& serviceType, const IsoString& domain, IsoString host, uint32_t port, uint32_t interfaceIndex):
                       ServiceName(serviceName), ServiceType(serviceType), Domain(domain), Host(host), Port(port), InterfaceIndex(interfaceIndex)
   {
   }
   ZeroConfServiceItem(const IsoString& serviceName, const IsoString& serviceType, const IsoString& domain, uint32_t interfaceIndex):
                       ServiceName(serviceName), ServiceType(serviceType), Domain(domain), InterfaceIndex(interfaceIndex)
   {
   }
   ZeroConfServiceItem(const ZeroConfServiceItem& other):ServiceName(other.ServiceName), ServiceType(other.ServiceType), Domain(other.Domain), Host(other.Host), Port(other.Port), InterfaceIndex(other.InterfaceIndex), InterfaceName(other.InterfaceName), IsWireless(other.IsWireless)
   {
   }
   bool operator ==( const ZeroConfServiceItem& rhs ) const
   {
      return (ServiceName == rhs.ServiceName) &&  (ServiceType == rhs.ServiceType) && (Domain == rhs.Domain) && (InterfaceIndex == rhs.InterfaceIndex) ;
   }

   bool operator <( const ZeroConfServiceItem& rhs ) const
   {
     if (ServiceName != rhs.ServiceName)
        return ServiceName < rhs.ServiceName;

     if (ServiceType != rhs.ServiceType)
       return ServiceType < rhs.ServiceType;

     if (Domain != rhs.Domain)
       return Domain < rhs.Domain;

     if (InterfaceIndex != rhs.InterfaceIndex)
       return InterfaceIndex < rhs.InterfaceIndex;

     return false;
   }
};

typedef SortedArray<ZeroConfServiceItem>   ZeroConfServicesArray;

class ExclZeroConfServicesList : public ExclusiveAccess<ZeroConfServicesArray>
{
public:

   ExclZeroConfServicesList( Mutex& mutex, ZeroConfServicesArray& list ) :
      ExclusiveAccess( mutex, list )
   {
   }
};

class ExclConstZeroConfServicesList : public ExclusiveConstAccess<ZeroConfServicesArray>
{
public:

   ExclConstZeroConfServicesList( Mutex& mutex, const ZeroConfServicesArray& list ) :
      ExclusiveConstAccess( mutex, list )
   {
   }
};

class ZeroConfClient
{
  mutable Mutex         m_serviceMutex;
  ZeroConfServicesArray m_services;
public:
  ZeroConfClient(){}
  ~ZeroConfClient() = default;

  void clear()
  {
    m_services.Clear();
  }


  ExclZeroConfServicesList ZeroConfServicesList()
  {
    return ExclZeroConfServicesList(m_serviceMutex, m_services);
  }

  ExclConstZeroConfServicesList ConstZeroConfServicesList() const
  {
    return ExclConstZeroConfServicesList(m_serviceMutex, m_services);
  }

  ExclConstZeroConfServicesList ZeroConfServicesList() const
  {
    return ConstZeroConfServicesList();
  }

  std::function<void(const ZeroConfServiceItem&)> onServiceAdded = [] (const ZeroConfServiceItem&) {};
  std::function<void(const ZeroConfServiceItem&)> onServiceRemoved = [] (const ZeroConfServiceItem&) {};

};

#ifdef WITH_ZEROCONF
class ZeroConfServiceHandler : public Thread
{
  volatile bool m_stop = false;
protected:
  DNSServiceRef m_serviceRef;

  static bool getAddrInfoFromInterfaceIndex(uint32_t interfaceIndex, char* host);
  static bool isWireless(const IsoString& interfaceName);

public:

  ZeroConfServiceHandler()  {}
  virtual ~ZeroConfServiceHandler(){
    DNSServiceRefDeallocate(m_serviceRef);
  }

  virtual void Run();
  void Stop()
  {
    int socket_fd = DNSServiceRefSockFD(m_serviceRef);
    shutdown(socket_fd, 2);
    m_stop = true;
  }

};

class ZeroConfServiceBrowser : public ZeroConfServiceHandler
{
protected:
  std::unique_ptr<ZeroConfClient> m_client;
public:
  ZeroConfServiceBrowser(){}
  virtual ~ZeroConfServiceBrowser(){

  }

  void setClient(std::unique_ptr<ZeroConfClient> client)
  {
    m_client = std::move(client);
  }

  void Start();

  const ZeroConfClient& getClient() const
  {
      return *m_client.get();
  }

  ZeroConfClient& getClient()
  {
      return *m_client.get();
  }


private:

  static void onBrowseEvent(DNSServiceRef service,
                            DNSServiceFlags flags,
                            uint32_t interfaceIndex,
                            DNSServiceErrorType errorCode,
                            const char* name,
                            const char* type,
                            const char* domain,
                            void* context);

};

class ZeroConfServiceResolver;
struct ResolveContext
{
  ZeroConfServiceResolver*  m_zeroConfServiceResolver   = nullptr;
  ZeroConfClient&           m_zeroConfClient;

  ResolveContext(ZeroConfServiceResolver* zeroConfServiceResolver, ZeroConfClient& zeroConfClient): m_zeroConfServiceResolver(zeroConfServiceResolver), m_zeroConfClient(zeroConfClient)
  {
  }
};

class ZeroConfServiceResolver : public ZeroConfServiceHandler {
  ResolveContext m_resolveContext;
  const IsoString m_serviceName;
  const IsoString m_serviceType;
  const IsoString m_serviceDomain;
  uint32_t m_InterfaceIndex = 0;

public:
  ZeroConfServiceResolver(ZeroConfClient& zeroConfClient,  const IsoString& serviceName, const IsoString& serviceType, const IsoString& serviceDomain, uint32_t interfaceIndex):m_resolveContext(this, zeroConfClient), m_serviceName(serviceName), m_serviceType(serviceType), m_serviceDomain(serviceDomain), m_InterfaceIndex(interfaceIndex)  {}
  virtual ~ZeroConfServiceResolver(){
  }

  void Start();


private:

  static void onResolveEvent(DNSServiceRef serviceRef,
                              DNSServiceFlags flags,
                              uint32_t interface,
                              DNSServiceErrorType errorCode,
                              const char *fullname,
                              const char *hosttarget,
                              uint16_t port,
                              uint16_t txtLen,
                              const unsigned char *txtRecord,
                              void *context);


};
#endif // WITH_ZEROCONF
}
#endif // ZEROCONFSERVICEDETECTION_H

// ----------------------------------------------------------------------------
// EOF ZeroconfServiceDetection.h - Released 2021-10-28T16:39:26Z
