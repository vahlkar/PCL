//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Action.cpp - Released 2024-12-28T16:53:56Z
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

#include <pcl/Action.h>
#include <pcl/AutoLock.h>
#include <pcl/ErrorHandler.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static void MandatoryError( const char* funcName )
{
   throw Error( String( "Action::" ) + funcName + "() must be reimplemented in descendant class." );
}

// ----------------------------------------------------------------------------

class ActionDispatcher
{
public:

   static void api_func Execute( action_handle hSender )
   {
      try
      {
         reinterpret_cast<Action*>( hSender )->Execute();
      }
      ERROR_HANDLER
   }

   static api_bool api_func QueryState( const_action_handle hSender, const void* info )
   {
      try
      {
         return api_bool( reinterpret_cast<const Action*>( hSender )->IsEnabled( *reinterpret_cast<const ActionInfo*>( info ) ) );
      }
      ERROR_HANDLER
      return api_false;
   }
};

// ----------------------------------------------------------------------------

Action::Action( const String& menuItem, const String& iconSVGFile, const String& toolBar )
   : UIObject( (*API->Action->CreateActionSVGFile)( ModuleHandle(), this, menuItem.c_str(), toolBar.c_str(), iconSVGFile.c_str(), 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateActionSVGFile" );

   (*API->Action->SetActionExecutionRoutine)( handle, ActionDispatcher::Execute );
   (*API->Action->SetActionStateQueryRoutine)( handle, ActionDispatcher::QueryState );
}

// ----------------------------------------------------------------------------

Action::Action( const String& menuItem, const IsoString& iconSVGSource, int, const String& toolBar )
   : UIObject( (*API->Action->CreateActionSVG)( ModuleHandle(), this, menuItem.c_str(), toolBar.c_str(), iconSVGSource.c_str(), 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateActionSVG" );

   (*API->Action->SetActionExecutionRoutine)( handle, ActionDispatcher::Execute );
   (*API->Action->SetActionStateQueryRoutine)( handle, ActionDispatcher::QueryState );
}

// ----------------------------------------------------------------------------

#ifdef __PCL_ACTION_DEPRECATED_CTOR
// ### DEPRECATED
Action::Action( const String& menuItem, const Bitmap& icon, const String& toolBar )
   : UIObject( (*API->Action->CreateAction)( ModuleHandle(), this, menuItem.c_str(), toolBar.c_str(), icon.handle, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateAction" );

   (*API->Action->SetActionExecutionRoutine)( handle, ActionDispatcher::Execute );
   (*API->Action->SetActionStateQueryRoutine)( handle, ActionDispatcher::QueryState );
}
#endif

// ----------------------------------------------------------------------------

Action& Action::Null()
{
   static Action* nullAction = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullAction == nullptr )
      nullAction = new Action( reinterpret_cast<void*>( 0 ) );
   return *nullAction;
}

// ----------------------------------------------------------------------------

String Action::MenuItem() const
{
   size_type len = 0;
   (*API->Action->GetActionMenuItem)( handle, 0, &len );

   String menuItem;
   if ( len > 0 )
   {
      menuItem.SetLength( len );
      if ( (*API->Action->GetActionMenuItem)( handle, menuItem.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetActionMenuItem" );
      menuItem.ResizeToNullTerminated();
   }
   return menuItem;
}

// ----------------------------------------------------------------------------

String Action::MenuText() const
{
   size_type len = 0;
   (*API->Action->GetActionMenuText)( handle, 0, &len );

   String menuText;
   if ( len > 0 )
   {
      menuText.SetLength( len );
      if ( (*API->Action->GetActionMenuText)( handle, menuText.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetActionMenuText" );
      menuText.ResizeToNullTerminated();
   }
   return menuText;
}

// ----------------------------------------------------------------------------

void Action::SetMenuText( const String& item )
{
   (*API->Action->SetActionMenuText)( handle, item.c_str() );
}

// ----------------------------------------------------------------------------

String Action::ToolBar() const
{
   size_type len = 0;
   (*API->Action->GetActionToolBar)( handle, 0, &len );

   String toolBarName;
   if ( len > 0 )
   {
      toolBarName.SetLength( len );
      if ( (*API->Action->GetActionToolBar)( handle, toolBarName.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetActionToolBar" );
      toolBarName.ResizeToNullTerminated();
   }
   return toolBarName;
}

// ----------------------------------------------------------------------------

void Action::GetAccelerator( int& keyModifiers, int& keyCode ) const
{
   (*API->Action->GetActionAccelerator)( handle, &keyModifiers, &keyCode );
}

// ----------------------------------------------------------------------------

void Action::SetAccelerator( int keyModifiers, int keyCode )
{
   (*API->Action->SetActionAccelerator)( handle, keyModifiers, keyCode );
}

// ----------------------------------------------------------------------------

String Action::ToolTip() const
{
   size_type len = 0;
   (*API->Action->GetActionToolTip)( handle, 0, &len );

   String toolTip;
   if ( len > 0 )
   {
      toolTip.SetLength( len );
      if ( (*API->Action->GetActionToolTip)( handle, toolTip.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetActionToolTip" );
      toolTip.ResizeToNullTerminated();
   }
   return toolTip;
}

// ----------------------------------------------------------------------------

void Action::SetToolTip( const String& tip )
{
   (*API->Action->SetActionToolTip)( handle, tip.c_str() );
}

// ----------------------------------------------------------------------------

Bitmap Action::Icon() const
{
   return Bitmap( (*API->Action->GetActionIcon)( handle ) );
}

// ----------------------------------------------------------------------------

void Action::SetIconSVGFile( const String& filePath )
{
   (*API->Action->SetActionIconSVGFile)( handle, filePath.c_str() );
}

// ----------------------------------------------------------------------------

void Action::SetIconSVG( const IsoString& svgSource )
{
   (*API->Action->SetActionIconSVG)( handle, svgSource.c_str() );
}

// ----------------------------------------------------------------------------

// ### DEPRECATED
void Action::SetIcon( const Bitmap& bmp )
{
   (*API->Action->SetActionIcon)( handle, bmp.handle );
}

// ----------------------------------------------------------------------------

void Action::Execute()
{
   MandatoryError( "Execute" );
}

// ----------------------------------------------------------------------------

void* Action::CloneHandle() const
{
   throw Error( "Cannot clone an Action handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Action.cpp - Released 2024-12-28T16:53:56Z
