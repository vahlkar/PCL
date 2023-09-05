//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.8.8
// ----------------------------------------------------------------------------
// GraphWebView.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2021 Cameron Leger
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

#ifndef __GRAPHWEBVIEW_h
#define __GRAPHWEBVIEW_h

#include <pcl/WebView.h>
#include <pcl/Timer.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct DataPoint
{
   double x;
   double data;
   double auxData;
   bool   approved;
   bool   locked;
};

typedef GenericVector<DataPoint> DataPointVector;
typedef Array<DataPoint> DataPointArray;

// ----------------------------------------------------------------------------

class GraphWebView : public WebView
{
public:

   GraphWebView( Control& parent = Control::Null() );

   void SetDataset( const String& dataName, const String& auxDataName, const DataPointVector& dataset );

   typedef void (Control::*approve_event_handler)( GraphWebView& sender, int index );
   typedef void (Control::*unlock_event_handler)( GraphWebView& sender, int index );

   void OnApprove( approve_event_handler handler, Control& receiver );
   void OnUnlock( unlock_event_handler handler, Control& receiver );

   void Cleanup();

private:

   String Header() const;
   String Footer() const;

   void e_MouseEnter( Control& sender );
   void e_MouseLeave( Control& sender );
   void e_Timer( Timer& sender );
   void e_JSResult( WebView& sender, const Variant& result );
   void e_LoadFinished( WebView& sender, bool state );

   bool   m_loaded = false;
   Timer  m_eventCheckTimer;
   bool   m_keepChecking = true;
   String m_htmlFilePath;

   struct EventHandlers
   {
      approve_event_handler   onApprove = nullptr;
      Control*                onApproveReceiver = nullptr;
      unlock_event_handler    onUnlock = nullptr;
      Control*                onUnlockReceiver = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_eventHandlers;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GRAPHWEBVIEW_h

// ----------------------------------------------------------------------------
// EOF GraphWebView.h - Released 2023-08-28T15:23:41Z
