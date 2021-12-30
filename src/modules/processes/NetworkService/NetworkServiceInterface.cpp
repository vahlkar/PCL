//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.17
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceInterface.cpp - Released 2021-12-29T20:37:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
//
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "NetworkServiceInterface.h"
#include "NetworkServiceLogger.h"
#include "NetworkServiceProcess.h"
#include "NetworkServiceTask.h"

#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static const char* DEFAULT_TASK_PROVIDER_URL = "";
static const char* DEFAULT_TASK_TYPE = "";

// ----------------------------------------------------------------------------

NetworkServiceInterface* TheNetworkServiceInterface = nullptr;

// ----------------------------------------------------------------------------

NetworkServiceInterface::NetworkServiceInterface()
   : m_taskProviderURL( DEFAULT_TASK_PROVIDER_URL )
   , m_taskType( DEFAULT_TASK_TYPE )
{
   TheNetworkServiceInterface = this;
}

// ----------------------------------------------------------------------------

NetworkServiceInterface::~NetworkServiceInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString NetworkServiceInterface::Id() const
{
   return "NetworkService";
}

// ----------------------------------------------------------------------------

MetaProcess* NetworkServiceInterface::Process() const
{
   return TheNetworkServiceProcess;
}

// ----------------------------------------------------------------------------

String NetworkServiceInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/NetworkService.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures NetworkServiceInterface::Features() const
{
   return InterfaceFeature::None;
}

// ----------------------------------------------------------------------------

bool NetworkServiceInterface::IsInstanceGenerator() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool NetworkServiceInterface::CanImportInstances() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool NetworkServiceInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "NetworkService" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheNetworkServiceProcess;
}

// ----------------------------------------------------------------------------

void NetworkServiceInterface::UpdateControls()
{
   GUI->TaskProvider_Edit.SetText( m_taskProviderURL );
   GUI->TaskType_Edit.SetText( m_taskType );

   bool running = TheNetworkServiceProcess->IsRunning();
   GUI->TaskProvider_Edit.Enable( !running );
   GUI->TaskType_Edit.Enable( !running );
   GUI->Start_PushButton.Enable( !running );
   GUI->Stop_PushButton.Enable( running );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class UILogger : public NetworkServiceLogger
{
public:

   UILogger( NetworkServiceInterface& ui )
      : m_ui( ui )
   {
   }

   UILogger( const UILogger& ) = delete;

   virtual ~UILogger() noexcept( false )
   {
      End();
   }

   void Begin() override
   {
      m_ui.UpdateControls();
      m_ui.GUI->Log_TextBox.Clear();
      Module->ProcessEvents();
   }

   void End() override
   {
      m_ui.UpdateControls();
      SetStatus( "Not running" );
   }

   void Section( const String& text ) override
   {
      m_ui.GUI->Log_TextBox.Insert( "<end><cbr><br><raw>" + text + "</raw><br>" );
      Module->ProcessEvents();
   }

   void Line( const String& text ) override
   {
      m_ui.GUI->Log_TextBox.Insert( "<end><cbr><raw>" + text + "</raw><br>" );
      Module->ProcessEvents();
   }

   void Warning( const String& text ) override
   {
      m_ui.GUI->Log_TextBox.Insert( "<end><cbr>** Warning: <raw>" + text + "</raw><br>" );
      Module->ProcessEvents();
   }

   void Error( const String& text ) override
   {
      m_ui.GUI->Log_TextBox.Insert( "<end><cbr>*** Error: <raw>" + text + "</raw><br>" );
      Module->ProcessEvents();
   }

   void SetStatus( const String& text ) override
   {
      m_ui.GUI->Status_Label.SetText( text );
      Module->ProcessEvents();
   }

private:

   NetworkServiceInterface& m_ui;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void NetworkServiceInterface::e_EditGetFocus( Control& sender )
{
   // Placeholder
}

// ----------------------------------------------------------------------------

void NetworkServiceInterface::e_EditCompleted( Edit& sender )
{
   String text = sender.Text().Trimmed();
   if ( sender == GUI->TaskProvider_Edit )
      m_taskProviderURL = text;
   else if ( sender == GUI->TaskType_Edit )
      m_taskType = text;
   else if ( sender == GUI->UserName_Edit )
      m_userName = text;
   else if ( sender == GUI->WorkerName_Edit )
      m_workerName = text;
   else if ( sender == GUI->UserPassword_Edit )
      m_userPassword = text;

   sender.SetText( text );
}

// ----------------------------------------------------------------------------

void NetworkServiceInterface::e_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->Start_PushButton )
   {
      if ( TheNetworkServiceProcess->IsRunning() )
         return;

      String parameters = String()
         << "taskProviderURL=" << m_taskProviderURL << ','
         << "workerName=" << m_workerName << ','
         << "taskType=" << m_taskType << ','
         << "userName=" << m_userName << ','
         << "userPassword=" << m_userPassword;

      UILogger logger( *this );
      TheNetworkServiceProcess->Start( parameters, &logger );

      UpdateControls();
   }
   else if ( sender == GUI->Stop_PushButton )
   {
      if ( TheNetworkServiceProcess->IsRunning() )
         TheNetworkServiceProcess->Stop();

      UpdateControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

NetworkServiceInterface::GUIData::GUIData( NetworkServiceInterface& w )
{
   pcl::Font font = w.Font();
   int labelWidth1 = font.Width( String( "Task provider:" ) + 'M' );
   int editWidth1 = 60 * font.Width( 'M' );
   int editWidth2 = 40 * font.Width( 'M' );

   //

   const char* taskProviderToolTip = "<p>URL of the task provider for this service.</p>";

   TaskProvider_Label.SetText( "Task provider:" );
   TaskProvider_Label.SetMinWidth( labelWidth1 );
   TaskProvider_Label.SetToolTip( taskProviderToolTip );
   TaskProvider_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   TaskProvider_Edit.SetMinWidth( editWidth1 );
   TaskProvider_Edit.SetToolTip( taskProviderToolTip );
   TaskProvider_Edit.OnGetFocus( (Control::event_handler)&NetworkServiceInterface::e_EditGetFocus, w );
   TaskProvider_Edit.OnEditCompleted( (Edit::edit_event_handler)&NetworkServiceInterface::e_EditCompleted, w );

   TaskProvider_Sizer.SetSpacing( 4 );
   TaskProvider_Sizer.Add( TaskProvider_Label );
   TaskProvider_Sizer.Add( TaskProvider_Edit, 100 );

   //

   const char* taskTypeToolTip = "<p>The task to be performed by this service.</p>";

   TaskType_Label.SetText( "Task type:" );
   TaskType_Label.SetMinWidth( labelWidth1 );
   TaskType_Label.SetToolTip( taskTypeToolTip );
   TaskType_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   TaskType_Edit.SetMinWidth( editWidth2 );
   TaskType_Edit.SetToolTip( taskTypeToolTip );
   TaskType_Edit.OnGetFocus( (Control::event_handler)&NetworkServiceInterface::e_EditGetFocus, w );
   TaskType_Edit.OnEditCompleted( (Edit::edit_event_handler)&NetworkServiceInterface::e_EditCompleted, w );

   TaskType_Sizer.SetSpacing( 4 );
   TaskType_Sizer.Add( TaskType_Label );
   TaskType_Sizer.Add( TaskType_Edit );
   TaskType_Sizer.AddStretch();

   //

   WorkerName_Label.SetText( "Worker name:" );
   WorkerName_Label.SetMinWidth( labelWidth1 );
   WorkerName_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   WorkerName_Edit.SetMinWidth( editWidth2 );
   WorkerName_Edit.OnGetFocus( (Control::event_handler)&NetworkServiceInterface::e_EditGetFocus, w );
   WorkerName_Edit.OnEditCompleted( (Edit::edit_event_handler)&NetworkServiceInterface::e_EditCompleted, w );

   WorkerName_Sizer.SetSpacing( 4 );
   WorkerName_Sizer.Add( WorkerName_Label );
   WorkerName_Sizer.Add( WorkerName_Edit );
   WorkerName_Sizer.AddStretch();

   //

   UserName_Label.SetText( "User name:" );
   UserName_Label.SetMinWidth( labelWidth1 );
   UserName_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   UserName_Edit.SetMinWidth( editWidth2 );
   UserName_Edit.OnGetFocus( (Control::event_handler)&NetworkServiceInterface::e_EditGetFocus, w );
   UserName_Edit.OnEditCompleted( (Edit::edit_event_handler)&NetworkServiceInterface::e_EditCompleted, w );

   UserName_Sizer.SetSpacing( 4 );
   UserName_Sizer.Add( UserName_Label );
   UserName_Sizer.Add( UserName_Edit );
   UserName_Sizer.AddStretch();

   //

   UserPassword_Label.SetText( "Password:" );
   UserPassword_Label.SetMinWidth( labelWidth1 );
   UserPassword_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   UserPassword_Edit.SetMinWidth( editWidth2 );
   UserPassword_Edit.EnablePasswordMode();
   UserPassword_Edit.OnGetFocus( (Control::event_handler)&NetworkServiceInterface::e_EditGetFocus, w );
   UserPassword_Edit.OnEditCompleted( (Edit::edit_event_handler)&NetworkServiceInterface::e_EditCompleted, w );

   UserPassword_Sizer.SetSpacing( 4 );
   UserPassword_Sizer.Add( UserPassword_Label );
   UserPassword_Sizer.Add( UserPassword_Edit );
   UserPassword_Sizer.AddStretch();

   //

   Log_TextBox.SetScaledMinSize( 640, 400 );
   Log_TextBox.SetReadOnly();
   Log_TextBox.SetObjectId( "CWConsole" ); // inherit font and colors from PixInsight's terminal

   //

   Start_PushButton.SetText( "Start" );
   Start_PushButton.SetDefault();
   Start_PushButton.OnClick( (Button::click_event_handler)&NetworkServiceInterface::e_ButtonClick, w );

   Stop_PushButton.SetText( "Stop" );
   Stop_PushButton.OnClick( (Button::click_event_handler)&NetworkServiceInterface::e_ButtonClick, w );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.Add( Start_PushButton );
   Buttons_Sizer.Add( Stop_PushButton );
   Buttons_Sizer.AddStretch();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( TaskProvider_Sizer );
   Global_Sizer.Add( TaskType_Sizer );
   Global_Sizer.Add( WorkerName_Sizer );
   Global_Sizer.Add( UserName_Sizer );
   Global_Sizer.Add( UserPassword_Sizer );
   Global_Sizer.Add( Log_TextBox, 100 );
   Global_Sizer.Add( Status_Label );
   Global_Sizer.Add( Buttons_Sizer );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetMinSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NetworkServiceInterface.cpp - Released 2021-12-29T20:37:28Z
