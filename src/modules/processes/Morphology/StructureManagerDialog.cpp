//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// StructureManagerDialog.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
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

#include "StructureManagerDialog.h"

#include <pcl/FileDialog.h>
#include <pcl/MessageBox.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

StructureManagerDialog::StructureManagerDialog( MorphologicalTransformationInterface& i )
   : m_interface( i )
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( 'M', 50 ) );
   int buttonWidth = fnt.Width( String( "Delete All" ) + String( 'M', 3 ) );

   //

   ModulePath_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   ModulePath_Label.SetStyle( FrameStyle::Sunken );
   ModulePath_Label.SetMinWidth( labelWidth );

   LoadModule_PushButton.SetText( "Load..." );
   LoadModule_PushButton.SetMinWidth( buttonWidth );
   LoadModule_PushButton.SetToolTip( "Load structure set module" );
   LoadModule_PushButton.OnClick( (PushButton::click_event_handler)&StructureManagerDialog::ModuleClick, *this );

   SaveModule_PushButton.SetText( "Save..." );
   SaveModule_PushButton.SetMinWidth( buttonWidth );
   SaveModule_PushButton.SetToolTip( "Save structure set module" );
   SaveModule_PushButton.OnClick( (PushButton::click_event_handler)&StructureManagerDialog::ModuleClick, *this );

   ModuleButtons_Sizer.SetSpacing( 4 );
   ModuleButtons_Sizer.Add( LoadModule_PushButton );
   ModuleButtons_Sizer.Add( SaveModule_PushButton );
   ModuleButtons_Sizer.AddStretch();

   Module_Sizer.SetMargin( 6 );
   Module_Sizer.SetSpacing( 6 );
   Module_Sizer.Add( ModulePath_Label );
   Module_Sizer.Add( ModuleButtons_Sizer );

   Module_GroupBox.SetTitle( "Structure Set Module" );
   Module_GroupBox.SetSizer( Module_Sizer );

   //

   StructureList_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&StructureManagerDialog::ItemSelected, *this );
   StructureList_ComboBox.SetMinWidth( labelWidth );

   PickStructure_PushButton.SetText( "Pick" );
   PickStructure_PushButton.SetMinWidth( buttonWidth );
   PickStructure_PushButton.SetToolTip( "Pick selected structure" );
   PickStructure_PushButton.OnClick( (PushButton::click_event_handler)&StructureManagerDialog::StructureListClick, *this );

   DuplicateStructure_PushButton.SetText( "Duplicate" );
   DuplicateStructure_PushButton.SetMinWidth( buttonWidth );
   DuplicateStructure_PushButton.SetToolTip( "Duplicate selected structure" );
   DuplicateStructure_PushButton.OnClick( (PushButton::click_event_handler)&StructureManagerDialog::StructureListClick, *this );

   DeleteStructure_PushButton.SetText( "Delete" );
   DeleteStructure_PushButton.SetMinWidth( buttonWidth );
   DeleteStructure_PushButton.SetToolTip( "Delete selected structure" );
   DeleteStructure_PushButton.OnClick( (PushButton::click_event_handler)&StructureManagerDialog::StructureListClick, *this );

   DeleteAll_PushButton.SetText( "Delete All" );
   DeleteAll_PushButton.SetMinWidth( buttonWidth );
   DeleteAll_PushButton.SetToolTip( "Delete all structures" );
   DeleteAll_PushButton.OnClick( (PushButton::click_event_handler)&StructureManagerDialog::StructureListClick, *this );

   StructureListButtons_Sizer.SetSpacing( 4 );
   StructureListButtons_Sizer.Add( PickStructure_PushButton );
   StructureListButtons_Sizer.Add( DuplicateStructure_PushButton );
   StructureListButtons_Sizer.Add( DeleteStructure_PushButton );
   StructureListButtons_Sizer.Add( DeleteAll_PushButton );
   StructureListButtons_Sizer.AddStretch();

   StructureList_Sizer.SetMargin( 6 );
   StructureList_Sizer.SetSpacing( 6 );
   StructureList_Sizer.Add( StructureList_ComboBox );
   StructureList_Sizer.Add( StructureListButtons_Sizer );

   StructureList_GroupBox.SetTitle( "Available Structures" );
   StructureList_GroupBox.SetSizer( StructureList_Sizer );

   //

   CurrentStructure_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   CurrentStructure_Label.SetStyle( FrameStyle::Sunken );
   CurrentStructure_Label.SetMinWidth( labelWidth );

   SaveStructure_PushButton.SetText( "Save" );
   SaveStructure_PushButton.SetMinWidth( buttonWidth );
   SaveStructure_PushButton.SetToolTip( "Save current structure" );
   SaveStructure_PushButton.OnClick( (PushButton::click_event_handler)&StructureManagerDialog::SaveStructureClick, *this );

   CurrentStructureButtons_Sizer.SetSpacing( 4 );
   CurrentStructureButtons_Sizer.Add( SaveStructure_PushButton );
   CurrentStructureButtons_Sizer.AddStretch();

   CurrentStructure_Sizer.SetMargin( 6 );
   CurrentStructure_Sizer.SetSpacing( 6 );
   CurrentStructure_Sizer.Add( CurrentStructure_Label );
   CurrentStructure_Sizer.Add( CurrentStructureButtons_Sizer );

   CurrentStructure_GroupBox.SetTitle( "Current Structure" );
   CurrentStructure_GroupBox.SetSizer( CurrentStructure_Sizer );

   //

   Done_PushButton.SetText( "Done" );
   Done_PushButton.SetMinWidth( buttonWidth );
   Done_PushButton.OnClick( (PushButton::click_event_handler)&StructureManagerDialog::DoneClick, *this );

   Buttons_Sizer.SetSpacing( 4 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( Done_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Module_GroupBox );
   Global_Sizer.Add( StructureList_GroupBox );
   Global_Sizer.Add( CurrentStructure_GroupBox );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   SetWindowTitle( "Structure Manager" );

   OnShow( (Control::event_handler)&StructureManagerDialog::ControlShow, *this );

   Update();
}

// ----------------------------------------------------------------------------

void StructureManagerDialog::Update()
{
   ModulePath_Label.SetText( m_interface.collection.Path() );

   SaveModule_PushButton.Enable( !m_interface.collection.IsEmpty() );

   StructureList_ComboBox.Clear();
   if ( m_interface.collection.IsEmpty() )
      StructureList_ComboBox.AddItem( "<No structures available>" );
   else
   {
      for ( size_type i = 0; i < m_interface.collection.Length(); ++i )
         StructureList_ComboBox.AddItem( m_interface.collection[i].Name() );

      if ( !m_interface.instance.GetStructure().Name().IsEmpty() )
      {
         size_type idx = m_interface.collection.FindStructure( m_interface.instance.GetStructure().Name() );
         if ( idx != StructureCollection::notFound )
            StructureList_ComboBox.SetCurrentItem( idx );
      }
   }

   PickStructure_PushButton.Enable( !m_interface.collection.IsEmpty() );
   DeleteStructure_PushButton.Enable( !m_interface.collection.IsEmpty() );
   DeleteAll_PushButton.Enable( !m_interface.collection.IsEmpty() );

   CurrentStructure_Label.SetText( m_interface.instance.GetStructure().Name() );

   SaveStructure_PushButton.Enable( !m_interface.instance.GetStructure().Name().IsEmpty() );
}

// ----------------------------------------------------------------------------

void StructureManagerDialog::ModuleClick( Button& sender, bool /*checked*/ )
{
   if ( sender == LoadModule_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "Load Structure Set Module" );
      d.SetFilter( FileFilter( "Structure Set Modules", ".ssm" ) );
      d.DisableMultipleSelections();

      if ( !m_interface.collection.Path().IsEmpty() )
      {
         String initialPath = File::ExtractDrive( m_interface.collection.Path() ) +
                              File::ExtractDirectory( m_interface.collection.Path() );
         if ( !initialPath.EndsWith( '/' ) )
            initialPath += '/';
         d.SetInitialPath( initialPath );
      }

      if ( d.Execute() )
      {
         try
         {
            String fileName = d.FileName();
            if ( File::ExtractExtension( fileName ).IsEmpty() )
               fileName.Append( ".ssm" );

            m_interface.collection.Read( fileName );
         }

         ERROR_HANDLER

         Update();
      }
   }
   else if ( sender == SaveModule_PushButton )
   {
      SaveFileDialog d;
      d.SetCaption( "Save Structure Set Module" );
      d.SetFilter( FileFilter( "Structure Set Modules", ".ssm" ) );
      d.SetInitialPath( m_interface.collection.Path() );
      d.EnableOverwritePrompt();

      if ( d.Execute() )
      {
         try
         {
            String fileName = d.FileName();
            if ( File::ExtractExtension( fileName ).IsEmpty() )
               fileName.Append( ".ssm" );

            m_interface.collection.Write( fileName );
         }

         ERROR_HANDLER

         Update();
      }
   }
}

// ----------------------------------------------------------------------------

void StructureManagerDialog::ItemSelected( ComboBox& /*sender*/, int /*itemIndex*/ )
{
   // Placeholder
}

// ----------------------------------------------------------------------------

void StructureManagerDialog::StructureListClick( Button& sender, bool /*checked*/ )
{
   if ( m_interface.collection.IsEmpty() )
      return;

   int idx = StructureList_ComboBox.CurrentItem();

   if ( sender == PickStructure_PushButton )
   {
      if ( idx < 0 )
         return;

      if ( m_interface.CanUndo() &&
           pcl::MessageBox( "<p>The current structure has been modified and is unsaved. Ok to throw it away?</p>",
                            m_interface.WindowTitle(),
                            StdIcon::Warning,
                            StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes ) return;

      m_interface.instance.GetStructure() = m_interface.collection[idx];
      m_interface.currentWayIndex = 0;
      m_interface.ResetUndo();
      m_interface.initialStructureName = m_interface.instance.GetStructure().Name();
      Ok();
   }
   else if ( sender == DuplicateStructure_PushButton )
   {
      String name = StructureList_ComboBox.ItemText( idx );
      String newName;

      for ( unsigned i = 1; ; )
      {
         newName = name + String().Format( " (%u)", i );
         if ( !m_interface.collection.HasStructure( newName ) )
            break;
      }

      Structure s( m_interface.collection[idx] );
      s.Rename( newName );
      m_interface.collection.AddStructure( s );

      Update();
      StructureList_ComboBox.FindItem( newName );
   }
   else if ( sender == DeleteStructure_PushButton )
   {
      if ( idx < 0 )
         return;

      if ( pcl::MessageBox( "<p>The requested operation cannot be undone</p>"
                            "<p><b>Delete \'" + StructureList_ComboBox.ItemText( idx ) + "\' structure?</b></p>",
                            m_interface.WindowTitle(),
                            StdIcon::Warning,
                            StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes ) return;

      m_interface.collection.DeleteStructure( idx );
      Update();
   }
   else if ( sender == DeleteAll_PushButton )
   {
      if ( pcl::MessageBox( "<p>The requested operation cannot be undone</p>"
                            "<p><b>Delete all structures in the current structure set?</b<</p>",
                            m_interface.WindowTitle(),
                            StdIcon::Warning,
                            StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes ) return;

      m_interface.collection.Destroy();
      Update();
   }
}

// ----------------------------------------------------------------------------

void StructureManagerDialog::SaveStructureClick( Button& sender, bool checked )
{
   String name = m_interface.instance.GetStructure().Name();

   if ( !name.IsEmpty() )
   {
      size_type idx = m_interface.collection.FindStructure( name );

      if ( idx != StructureCollection::notFound )
      {
         if ( pcl::MessageBox( "<p>Replace existing \'" + name + "\' structure?</p>",
                              m_interface.WindowTitle(),
                              StdIcon::Warning,
                              StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes ) return;

         m_interface.collection[idx] = m_interface.instance.GetStructure();
         m_interface.collection.Reindex();
      }
      else
      {
         m_interface.collection.AddStructure( m_interface.instance.GetStructure() );
      }

      Update();
      StructureList_ComboBox.SetCurrentItem( StructureList_ComboBox.FindItem( name ) );

      m_interface.ResetUndo();
   }
}

// ----------------------------------------------------------------------------

void StructureManagerDialog::DoneClick( Button& sender, bool checked )
{
   Ok();
}

// ----------------------------------------------------------------------------

void StructureManagerDialog::ControlShow( Control& sender )
{
   AdjustToContents();
   SetMinWidth();
   SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF StructureManagerDialog.cpp - Released 2024-12-28T16:54:15Z
