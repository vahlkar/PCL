//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// PixelMathInterface.cpp - Released 2021-05-05T15:38:07Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
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

#include "ExpressionEditorDialog.h"
#include "ImageCache.h"
#include "PixelMathInterface.h"
#include "PixelMathProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/MessageBox.h>

#define AUTO_ID            "<Auto>"
#define EDITOR_MIN_WIDTH   600
#define EDITOR_MIN_HEIGHT  150

namespace pcl
{

// ----------------------------------------------------------------------------

PixelMathInterface* ThePixelMathInterface = nullptr;

// ----------------------------------------------------------------------------

PixelMathInterface::PixelMathInterface()
   : m_instance( ThePixelMathProcess )
{
   ThePixelMathInterface = this;

   // The auto save geometry feature is of no good to interfaces that include
   // both auto-expanding controls (e.g. CodeEditor) and collapsible sections
   // (e.g. SectionBar).
   DisableAutoSaveGeometry();
}

// ----------------------------------------------------------------------------

PixelMathInterface::~PixelMathInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString PixelMathInterface::Id() const
{
   return "PixelMath";
}

// ----------------------------------------------------------------------------

MetaProcess* PixelMathInterface::Process() const
{
   return ThePixelMathProcess;
}

// ----------------------------------------------------------------------------

String PixelMathInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/PixelMath.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures PixelMathInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::ApplyGlobalButton;
}

// ----------------------------------------------------------------------------

void PixelMathInterface::ApplyInstance() const
{
   const_cast<PixelMathInterface*>( this )->ExportSourceCode();
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void PixelMathInterface::ApplyInstanceGlobal() const
{
   const_cast<PixelMathInterface*>( this )->ExportSourceCode();
   m_instance.LaunchGlobal();
}

// ----------------------------------------------------------------------------

void PixelMathInterface::ResetInstance()
{
   PixelMathInstance defaultInstance( ThePixelMathProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool PixelMathInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "PixelMath" );
      ImportSourceCode();
      UpdateControls();
   }

   dynamic = false;
   return &P == ThePixelMathProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* PixelMathInterface::NewProcess() const
{
   const_cast<PixelMathInterface*>( this )->ExportSourceCode();
   return new PixelMathInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool PixelMathInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const PixelMathInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a PixelMath instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool PixelMathInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool PixelMathInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   ImportSourceCode();
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

/*
 * We use image notifications to keep the image cache consistent when images
 * are modified, renamed or deleted.
 */

bool PixelMathInterface::WantsImageNotifications() const
{
   return true;
}

void PixelMathInterface::ImageUpdated( const View& view )
{
   if ( !TheImageCache->IsEmpty() )
      if ( view.IsMainView() )
         TheImageCache->InvalidateImage( view.Id() );
}

void PixelMathInterface::ImageRenamed( const View& view )
{
/*
 * ### TODO - CRITICAL
 *
 * We should have a notification where we can know the old and new image
 * identifiers in the same function call. We cannot rely on View references in
 * this case because we can have nested generators, hence cached images
 * generated from generated images ... and so on.
 *
 * We have a serious limitation here because we cannot invalidate cache items
 * for renamed images. For example, suppose that I execute:
 *
 * generator( A )
 *
 * Then I rename A -> B. The cached images for A (now B) cannot be invalidated
 * because they cannot be tracked by this function. Now I have a different
 * image C, which I rename C -> A. The cached images for the old A (now B) will
 * be used as if they had been generated for C (now A) - not nice, although
 * very unlikely to happen, fortunately.
 */

//    if ( !TheImageCache->IsEmpty() )
//       if ( view.IsMainView() )
//          TheImageCache->InvalidateImage( view.Id() );
}

void PixelMathInterface::ImageDeleted( const View& view )
{
   if ( !TheImageCache->IsEmpty() )
      if ( view.IsMainView() )
         TheImageCache->InvalidateImage( view.Id() );
}

// ----------------------------------------------------------------------------

void PixelMathInterface::UpdateControls()
{
   if ( m_instance.p_useSingleExpression )
   {
      if ( GUI->Editors_TabBox.CurrentPageIndex() == 1 || GUI->Editors_TabBox.CurrentPageIndex() == 2 )
         GUI->Editors_TabBox.SetCurrentPageIndex( 0 );
      GUI->Editors_TabBox.SetPageLabel( 0, "RGB/K" );
      GUI->Editors_TabBox.SetPageIcon( 0, ScaledResource( ":/toolbar/image-display-rgb.png" ) );
      GUI->Editors_TabBox.DisablePage( 1 );
      GUI->Editors_TabBox.DisablePage( 2 );
   }
   else
   {
      GUI->Editors_TabBox.SetPageLabel( 0, "R/K" );
      GUI->Editors_TabBox.SetPageIcon( 0, ScaledResource( ":/toolbar/image-display-red.png" ) );
      GUI->Editors_TabBox.EnablePage( 1 );
      GUI->Editors_TabBox.EnablePage( 2 );
   }

   GUI->UseSingleExpression_CheckBox.SetChecked( m_instance.p_useSingleExpression );

   GUI->GenerateOutput_CheckBox.SetChecked( m_instance.p_generateOutput );

   GUI->CacheGeneratedImages_CheckBox.SetChecked( m_instance.p_cacheGeneratedImages );

   GUI->SingleThreaded_CheckBox.SetChecked( m_instance.p_singleThreaded );

   GUI->Optimization_CheckBox.SetChecked( m_instance.p_optimization );

   GUI->Use64BitWorkingImage_CheckBox.Enable( m_instance.p_generateOutput );
   GUI->Use64BitWorkingImage_CheckBox.SetChecked( m_instance.p_use64BitWorkingImage );

   GUI->Rescale_CheckBox.Enable( m_instance.p_generateOutput );
   GUI->Rescale_CheckBox.SetChecked( m_instance.p_rescaleResult );

   GUI->RescaleLower_NumericEdit.Enable( m_instance.p_generateOutput && m_instance.p_rescaleResult );
   GUI->RescaleLower_NumericEdit.SetValue( m_instance.p_rescaleLower );

   GUI->RescaleUpper_NumericEdit.Enable( m_instance.p_generateOutput && m_instance.p_rescaleResult );
   GUI->RescaleUpper_NumericEdit.SetValue( m_instance.p_rescaleUpper );

   GUI->ReplaceTarget_RadioButton.Enable( m_instance.p_generateOutput );
   GUI->ReplaceTarget_RadioButton.SetChecked( !m_instance.p_createNewImage );

   GUI->CreateNewImage_RadioButton.Enable( m_instance.p_generateOutput );
   GUI->CreateNewImage_RadioButton.SetChecked( m_instance.p_createNewImage );

   GUI->ImageId_Label.Enable( m_instance.p_generateOutput && m_instance.p_createNewImage );

   GUI->ImageId_Edit.Enable( m_instance.p_generateOutput && m_instance.p_createNewImage );
   GUI->ImageId_Edit.SetText( m_instance.p_newImageId.IsEmpty() ? AUTO_ID : m_instance.p_newImageId );

   GUI->ImageId_Button.Enable( m_instance.p_generateOutput && m_instance.p_createNewImage );

   GUI->Width_Label.Enable( m_instance.p_createNewImage );

   GUI->Width_SpinBox.Enable( m_instance.p_createNewImage );
   GUI->Width_SpinBox.SetValue( m_instance.p_newImageWidth );

   GUI->Height_Label.Enable( m_instance.p_createNewImage );

   GUI->Height_SpinBox.Enable( m_instance.p_createNewImage );
   GUI->Height_SpinBox.SetValue( m_instance.p_newImageHeight );

   GUI->ColorSpace_Label.Enable( m_instance.p_createNewImage );

   GUI->ColorSpace_ComboBox.Enable( m_instance.p_createNewImage );
   GUI->ColorSpace_ComboBox.SetCurrentItem( m_instance.p_newImageColorSpace );

   GUI->CreateAlpha_CheckBox.Enable( m_instance.p_createNewImage );
   GUI->CreateAlpha_CheckBox.SetChecked( m_instance.p_newImageAlpha );

   GUI->SampleFormat_Label.Enable( m_instance.p_generateOutput && m_instance.p_createNewImage );

   GUI->SampleFormat_ComboBox.Enable( m_instance.p_generateOutput && m_instance.p_createNewImage );
   GUI->SampleFormat_ComboBox.SetCurrentItem( m_instance.p_newImageSampleFormat );
}

// ----------------------------------------------------------------------------

void PixelMathInterface::ImportSourceCode()
{
   String x0 = m_instance.p_expression[0].Trimmed();
   String x1 = m_instance.p_expression[1].Trimmed();
   String x2 = m_instance.p_expression[2].Trimmed();
   String x3 = m_instance.p_expression[3].Trimmed();
   String x4 = m_instance.p_symbols.Trimmed();

   if ( !x0.IsEmpty() )
      x0 << '\n';
   if ( !x1.IsEmpty() )
      x1 << '\n';
   if ( !x2.IsEmpty() )
      x2 << '\n';
   if ( !x3.IsEmpty() )
      x3 << '\n';
   if ( !x4.IsEmpty() )
      x4 << '\n';

   GUI->RK_CodeEditor.SetText( x0 );
   GUI->G_CodeEditor.SetText( x1 );
   GUI->B_CodeEditor.SetText( x2 );
   GUI->A_CodeEditor.SetText( x3 );
   GUI->Symbols_CodeEditor.SetText( x4 );

   MakeSummary();

   GUI->RK_CodeEditor.SetCursorPosition( 0, 0 );
   GUI->G_CodeEditor.SetCursorPosition( 0, 0 );
   GUI->B_CodeEditor.SetCursorPosition( 0, 0 );
   GUI->A_CodeEditor.SetCursorPosition( 0, 0 );
   GUI->Symbols_CodeEditor.SetCursorPosition( 0, 0 );
   GUI->Summary_CodeEditor.SetCursorPosition( 0, 0 );
}

// ----------------------------------------------------------------------------

void PixelMathInterface::ExportSourceCode()
{
   m_instance.p_expression[0] = GUI->RK_CodeEditor.Text().Trimmed();
   m_instance.p_expression[1] = GUI->G_CodeEditor.Text().Trimmed();
   m_instance.p_expression[2] = GUI->B_CodeEditor.Text().Trimmed();
   m_instance.p_expression[3] = GUI->A_CodeEditor.Text().Trimmed();
   m_instance.p_symbols = GUI->Symbols_CodeEditor.Text().Trimmed();
}

// ----------------------------------------------------------------------------

void PixelMathInterface::MakeSummary()
{
   String x0 = GUI->RK_CodeEditor.Text().Trimmed();
   String x1 = GUI->G_CodeEditor.Text().Trimmed();
   String x2 = GUI->B_CodeEditor.Text().Trimmed();
   String x3 = GUI->A_CodeEditor.Text().Trimmed();
   String x4 = GUI->Symbols_CodeEditor.Text().Trimmed();

   StringList textBlocks;
   if ( !x0.IsEmpty() )
      textBlocks << (String( m_instance.p_useSingleExpression ? "// RGB/K:" : "// R:" ) << '\n' << x0);

   if ( !m_instance.p_useSingleExpression )
   {
      if ( !x1.IsEmpty() )
         textBlocks << (String( "// G:\n" ) << x1);
      if ( !x2.IsEmpty() )
         textBlocks << (String( "// B:\n" ) << x2);
      if ( !x3.IsEmpty() )
         textBlocks << (String( "// A:\n" ) << x3);
   }

   if ( !x4.IsEmpty() )
      textBlocks << (String( "// Symbols:\n" ) << x4);

   GUI->Summary_CodeEditor.SetText( String().ToSeparated( textBlocks, "\n\n" ) << '\n' );
}

// ----------------------------------------------------------------------------

void PixelMathInterface::e_PageSelected( TabBox& sender, int pageIndex )
{
   if ( pageIndex == 5 )
      MakeSummary();
}

// ----------------------------------------------------------------------------

void PixelMathInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->Editors_TabBox.SetFixedHeight();
   else
   {
      GUI->Editors_TabBox.SetScaledMinHeight( EDITOR_MIN_HEIGHT );
      GUI->Editors_TabBox.SetMaxHeight( int_max );

      if ( GUI->Editors_TabBox.IsVisible() )
         SetVariableHeight();
      else
         SetFixedHeight();
   }
}

// ----------------------------------------------------------------------------

void PixelMathInterface::e_EditCompleted( Edit& sender )
{
   if ( sender == GUI->ImageId_Edit )
   {
      try
      {
         String text = sender.Text().Trimmed();
         if ( !text.IsEmpty() && text != AUTO_ID && !text.IsValidIdentifier() )
            throw Error( "Invalid identifier: " + text );
         m_instance.p_newImageId = (text != AUTO_ID) ? text : String();
         text = m_instance.p_newImageId.IsEmpty() ? AUTO_ID : m_instance.p_newImageId;
         sender.SetText( text );
      }
      ERROR_CLEANUP(
         sender.SetText( m_instance.p_newImageId );
         sender.SelectAll();
         sender.Focus();
         return;
      )
   }
}

// ----------------------------------------------------------------------------

void PixelMathInterface::e_EditGetFocus( Control& sender )
{
   if ( sender == GUI->ImageId_Edit )
      if ( GUI->ImageId_Edit.Text() == AUTO_ID )
         GUI->ImageId_Edit.Clear();
}

// ----------------------------------------------------------------------------

void PixelMathInterface::e_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->ExpressionEditor_Button )
   {
      static ExpressionEditorDialog* dialog = nullptr;
      if ( dialog == nullptr )
         dialog = new ExpressionEditorDialog;
      ExportSourceCode();
      if ( dialog->Execute( m_instance ) )
      {
         ImportSourceCode();
         UpdateControls();
      }
   }
   else if ( sender == GUI->UseSingleExpression_CheckBox )
   {
      m_instance.p_useSingleExpression = checked;
      MakeSummary();
      UpdateControls();
   }
   else if ( sender == GUI->Use64BitWorkingImage_CheckBox )
   {
      m_instance.p_use64BitWorkingImage = checked;
   }
   else if ( sender == GUI->GenerateOutput_CheckBox )
   {
      m_instance.p_generateOutput = checked;
      UpdateControls();
   }
   else if ( sender == GUI->CacheGeneratedImages_CheckBox )
   {
      m_instance.p_cacheGeneratedImages = checked;
   }
   else if ( sender == GUI->SingleThreaded_CheckBox )
   {
      m_instance.p_singleThreaded = checked;
   }
   else if ( sender == GUI->Optimization_CheckBox )
   {
      m_instance.p_optimization = checked;
   }
   else if ( sender == GUI->Rescale_CheckBox )
   {
      m_instance.p_rescaleResult = checked;
      UpdateControls();
   }
   else if ( sender == GUI->ReplaceTarget_RadioButton )
   {
      m_instance.p_createNewImage = false;
      UpdateControls();
   }
   else if ( sender == GUI->CreateNewImage_RadioButton )
   {
      m_instance.p_createNewImage = true;
      UpdateControls();
   }
   else if ( sender == GUI->CreateAlpha_CheckBox )
   {
      m_instance.p_newImageAlpha = checked;
   }
   else if ( sender == GUI->ImageId_Button )
   {
      m_instance.p_newImageId.Clear();
      GUI->ImageId_Edit.SetText( AUTO_ID );
   }
   else if ( sender == GUI->ClearGeneratedImages_Button )
   {
      size_type count, size;
      String text;
      if ( TheImageCache->ClearImages( count, size ) )
         text << String( count ) << " cached image(s) removed, "
              << File::SizeAsString( size ) << " freed up.";
      else
         text << "The image cache is empty.";

      MessageBox( "<p style=\"white-space: pre;\">" + text + "</p>", "PixelMath",
                  StdIcon::Information,
                  StdButton::Ok ).Execute();
   }
}

// ----------------------------------------------------------------------------

void PixelMathInterface::e_NumericEditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->RescaleLower_NumericEdit )
      m_instance.p_rescaleLower = value;
   else if ( sender == GUI->RescaleUpper_NumericEdit )
      m_instance.p_rescaleUpper = value;
}

// ----------------------------------------------------------------------------

void PixelMathInterface::e_SpinBoxValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->Width_SpinBox )
      m_instance.p_newImageWidth = value;
   else if ( sender == GUI->Height_SpinBox )
      m_instance.p_newImageHeight = value;

   UpdateControls();
}

// ----------------------------------------------------------------------------

void PixelMathInterface::e_ComboBoxItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->SampleFormat_ComboBox )
      m_instance.p_newImageSampleFormat = itemIndex;
   else if ( sender == GUI->ColorSpace_ComboBox )
      m_instance.p_newImageColorSpace = itemIndex;
}

// ----------------------------------------------------------------------------

PixelMathInterface::GUIData::GUIData( PixelMathInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Sample format:" ) + 'T' );
   int comboWidth1 = fnt.Width( String( 'M', 25 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   Expression_SectionBar.SetTitle( "Expressions" );
   Expression_SectionBar.SetSection( Expression_Control );
   Expression_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&PixelMathInterface::e_ToggleSection, w );

   //

   RK_CodeEditor.SetFilePath( "rk.pmath" ); // enable syntax highlighting, PixelMath language
   RK_CodeEditor.LineNumbersControl().SetVisible( false );
   RK_Sizer.Add( RK_CodeEditor.LineNumbersControl() );
   RK_Sizer.Add( RK_CodeEditor );
   RK_Control.SetSizer( RK_Sizer );

   G_CodeEditor.SetFilePath( "g.pmath" );
   G_CodeEditor.LineNumbersControl().SetVisible( false );
   G_Sizer.Add( G_CodeEditor.LineNumbersControl() );
   G_Sizer.Add( G_CodeEditor );
   G_Control.SetSizer( G_Sizer );

   B_CodeEditor.SetFilePath( "b.pmath" );
   B_CodeEditor.LineNumbersControl().SetVisible( false );
   B_Sizer.Add( B_CodeEditor.LineNumbersControl() );
   B_Sizer.Add( B_CodeEditor );
   B_Control.SetSizer( B_Sizer );

   A_CodeEditor.SetFilePath( "a.pmath" );
   A_CodeEditor.LineNumbersControl().SetVisible( false );
   A_Sizer.Add( A_CodeEditor.LineNumbersControl() );
   A_Sizer.Add( A_CodeEditor );
   A_Control.SetSizer( A_Sizer );

   Symbols_CodeEditor.SetFilePath( "symbols.pmath" );
   Symbols_CodeEditor.LineNumbersControl().SetVisible( false );
   Symbols_Sizer.Add( Symbols_CodeEditor.LineNumbersControl() );
   Symbols_Sizer.Add( Symbols_CodeEditor );
   Symbols_Control.SetSizer( Symbols_Sizer );

   Summary_CodeEditor.SetFilePath( "summary.pmath" );
   Summary_CodeEditor.SetReadOnly();
   Summary_CodeEditor.LineNumbersControl().SetVisible( false );
   Summary_Sizer.Add( Summary_CodeEditor.LineNumbersControl() );
   Summary_Sizer.Add( Summary_CodeEditor );
   Summary_Control.SetSizer( Summary_Sizer );

   Editors_TabBox.SetScaledMinSize( EDITOR_MIN_WIDTH, EDITOR_MIN_HEIGHT );
   Editors_TabBox.AddPage( RK_Control, "RGB/K" );
   Editors_TabBox.AddPage( G_Control, "G" );
   Editors_TabBox.AddPage( B_Control, "B" );
   Editors_TabBox.AddPage( A_Control, "A" );
   Editors_TabBox.AddPage( Symbols_Control, "Symbols" );
   Editors_TabBox.AddPage( Summary_Control, "Summary" );

   Editors_TabBox.SetPageIcon( 0, w.ScaledResource( ":/toolbar/image-display-red.png" ) );
   Editors_TabBox.SetPageIcon( 1, w.ScaledResource( ":/toolbar/image-display-green.png" ) );
   Editors_TabBox.SetPageIcon( 2, w.ScaledResource( ":/toolbar/image-display-blue.png" ) );
   Editors_TabBox.SetPageIcon( 3, w.ScaledResource( ":/toolbar/image-display-alpha.png" ) );
   Editors_TabBox.SetPageIcon( 4, w.ScaledResource( ":/icons/table.png" ) );
   Editors_TabBox.SetPageIcon( 5, w.ScaledResource( ":/icons/document-text.png" ) );

   Editors_TabBox.OnPageSelected( (TabBox::page_event_handler)&PixelMathInterface::e_PageSelected, w );

   //

   UseSingleExpression_CheckBox.SetText( "Use a single RGB/K expression" );
   UseSingleExpression_CheckBox.SetToolTip(
      "<p>Apply a single expression to the red, green and blue channels of RGB color images. "
      "The same expression will also be applied to the gray channel of grayscale images.</p>" );
   UseSingleExpression_CheckBox.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   UseSingleExpression_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   UseSingleExpression_Sizer.Add( UseSingleExpression_CheckBox );
   UseSingleExpression_Sizer.AddStretch();

   //

   ExpressionEditor_Button.SetText( "Expression Editor" );
   ExpressionEditor_Button.SetToolTip( "<p>Open the PixelMath Expression Editor dialog.</p>" );
   ExpressionEditor_Button.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   ExpressionEditor_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ExpressionEditor_Sizer.Add( ExpressionEditor_Button );
   ExpressionEditor_Sizer.AddStretch();

   //

   Expression_Sizer.SetSpacing( 4 );
   Expression_Sizer.Add( Editors_TabBox );
   Expression_Sizer.Add( UseSingleExpression_Sizer );
   Expression_Sizer.Add( ExpressionEditor_Sizer );

   Expression_Control.SetSizer( Expression_Sizer );

   //

   Destination_SectionBar.SetTitle( "Destination" );
   Destination_SectionBar.SetSection( Destination_Control );
   Destination_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&PixelMathInterface::e_ToggleSection, w );

   //

   GenerateOutput_CheckBox.SetText( "Generate output" );
   GenerateOutput_CheckBox.SetToolTip( "<p>When this option is selected, PixelMath expressions will be evaluated and their "
      "results will generate output pixels, either in a newly created image (if the <i>create new image</i> option is selected), "
      "or to replace the target image (if <i>replace target image</i> is selected). This is the normal PixelMath working mode.</p>"
      "<p>If this option is disabled, PixelMath expressions will be evaluated but their results will be discarded. No output will "
      "be generated, and hence PixelMath will behave as a no-op process: the target image won't be modified when the process "
      "is executed in a view context, and no new image will be created as a result of global execution. This working mode is "
      "useful when PixelMath is being used exclusively for evaluation of global variables as a side-effect of expression "
      "evaluation.</p>" );
   GenerateOutput_CheckBox.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   GenerateOutput_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateOutput_Sizer.Add( GenerateOutput_CheckBox );
   GenerateOutput_Sizer.AddStretch();

   //

   CacheGeneratedImages_CheckBox.SetText( "Cache generated images" );
   CacheGeneratedImages_CheckBox.SetToolTip(
      "<p>Preserve generated images before and after PixelMath execution. This option is useful to speed up "
      "calculations when PixelMath expressions using generators are being executed repeatedly (for example, "
      "on a preview to fine tune expressions by trial/error work). Note that this uses memory resources, which "
      "may become significant, especially after many executions of PixelMath expressions with varying "
      "generator function parameters.</p>"
      "<p>If this option is disabled, existing generated images will be destroyed before and after PixelMath "
      "execution, so they'll have to be recalculated in successive executions of the same expressions.</p>" );
   CacheGeneratedImages_CheckBox.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   CacheGeneratedImages_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CacheGeneratedImages_Sizer.Add( CacheGeneratedImages_CheckBox );
   CacheGeneratedImages_Sizer.AddStretch();

   //

   SingleThreaded_CheckBox.SetText( "Single threaded" );
   SingleThreaded_CheckBox.SetToolTip( "<p>Enable this option to force PixelMath to use a single execution thread, irrespective of "
      "current global parallel execution settings. Single-threaded execution is necessary for some applications that are "
      "incompatible with multithreaded implementations. A good example is calculation of integral images. In an integral image, "
      "each pixel at coordinates {x,y} is the sum of all pixels at coordinates {i &lt;= x, j &lt;= y} in the source image. "
      "This task cannot be parallelized, and hence requires enabling this option to be implemented with PixelMath.</p>" );
   SingleThreaded_CheckBox.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   SingleThreaded_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SingleThreaded_Sizer.Add( SingleThreaded_CheckBox );
   SingleThreaded_Sizer.AddStretch();

   //

   Optimization_CheckBox.SetText( "Code optimization" );
   Optimization_CheckBox.SetToolTip( "<p>Enable code optimization to implement faster conditional functions and logical operators. "
      "Code optimization works by minimizing execution of unnecessary subexpressions, which can lead to a significant increase in "
      "the execution speed of iif() and iswitch() inline conditional functions, as well as logical operators (through short-circuit "
      "evaluation), among other critical PixelMath language constructs. This option is enabled by default, and should only be "
      "disabled by developers for verification and debugging purposes.</p>" );
   Optimization_CheckBox.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   Optimization_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Optimization_Sizer.Add( Optimization_CheckBox );
   Optimization_Sizer.AddStretch();

   //

   Use64BitWorkingImage_CheckBox.SetText( "Use 64-bit working images" );
   Use64BitWorkingImage_CheckBox.SetToolTip(
      "<p>If this option is selected, PixelMath will use 64-bit floating point working images to "
      "accumulate and store all intermediate results.</p>"
      "<p>When this option is disabled, PixelMath uses 32-bit floating point working images.</p>" );
   Use64BitWorkingImage_CheckBox.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   Use64BitWorkingImage_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Use64BitWorkingImage_Sizer.Add( Use64BitWorkingImage_CheckBox );
   Use64BitWorkingImage_Sizer.AddStretch();

   //

   Rescale_CheckBox.SetText( "Rescale result" );
   Rescale_CheckBox.SetToolTip( "<p>When this option is selected, PixelMath will rescale the final resulting "
      "image to the specified output range (see the <i>lower range</i> and <i>upper range</i> parameters below).</p>"
      "<p>If this option is disabled (default state), the final image will be truncated, if necessary, to fit "
      "in the normalized [0,1] range.</p>"
      "<p>You can disable both rescaling and truncation by creating a PixelMath process icon and editing its "
      "instance source code. If you set both 'rescale' and 'truncate' instance properties to false, the resulting "
      "image will be left intact upon PixelMath execution termination.</p>"
      "<p><b>Warning: Platform stability is not guaranteed if out-of-range pixel sample values propagate after "
      "process execution.</b></p>" );
   Rescale_CheckBox.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   Rescale_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Rescale_Sizer.Add( Rescale_CheckBox );
   Rescale_Sizer.AddStretch();

   //

   RescaleLower_NumericEdit.label.SetText( "Lower bound:" );
   RescaleLower_NumericEdit.label.SetFixedWidth( labelWidth1 );
   RescaleLower_NumericEdit.sizer.AddStretch();
   RescaleLower_NumericEdit.SetReal();
   RescaleLower_NumericEdit.SetRange( 0, 1 );
   RescaleLower_NumericEdit.SetPrecision( Abs( ThePMRescaleLowerBoundParameter->Precision() ) );
   RescaleLower_NumericEdit.SetToolTip( "<p>When the <i>rescale result</i> option is enabled, this is the lower bound "
      "of the output rescaling range.</p>" );
   RescaleLower_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&PixelMathInterface::e_NumericEditValueUpdated, w );

   //

   RescaleUpper_NumericEdit.label.SetText( "Upper bound:" );
   RescaleUpper_NumericEdit.label.SetFixedWidth( labelWidth1 );
   RescaleUpper_NumericEdit.sizer.AddStretch();
   RescaleUpper_NumericEdit.SetReal();
   RescaleUpper_NumericEdit.SetRange( 0, 1 );
   RescaleUpper_NumericEdit.SetPrecision( Abs( ThePMRescaleUpperBoundParameter->Precision() ) );
   RescaleUpper_NumericEdit.SetToolTip( "<p>When the <i>rescale result</i> option is enabled, this is the upper bound "
      "of the output rescaling range.</p>" );
   RescaleUpper_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&PixelMathInterface::e_NumericEditValueUpdated, w );

   //

   ReplaceTarget_RadioButton.SetText( "Replace target image" );
   ReplaceTarget_RadioButton.SetToolTip( "<p>Replace the target image with the result of PixelMath execution.</p>" );
   ReplaceTarget_RadioButton.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   ReplaceTarget_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ReplaceTarget_Sizer.Add( ReplaceTarget_RadioButton );
   ReplaceTarget_Sizer.AddStretch();

   //

   CreateNewImage_RadioButton.SetText( "Create new image" );
   CreateNewImage_RadioButton.SetToolTip( "<p>Send the PixelMath result to a newly created image window.</p>"
      "<p>If you select this option, the geometry, color space and data format of the output image can either be specified "
      "by this PixelMath instance (see the <i>width</i>, <i>height</i>, <i>color space</i> and <i>sample format</i> "
      "parameters below), or taken from the target image this PixelMath instance is executed on. In the former case, "
      "PixelMath can be executed in the global context because none of its parameters depend on an existing image. In the "
      "latter case, PixelMath can only be applied to an image.</p>"
      "<p>When PixelMath is executed globally, the working image has all of its pixel samples initially set to zero.</p>" );
   CreateNewImage_RadioButton.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   CreateNewImage_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CreateNewImage_Sizer.Add( CreateNewImage_RadioButton );
   CreateNewImage_Sizer.AddStretch();

   //

   ImageId_Label.SetText( "Image Id:" );
   ImageId_Label.SetMinWidth( labelWidth1 );
   ImageId_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ImageId_Edit.SetToolTip( "<p>When the <i>create new image</i> option is enabled, this is the identifier of the newly "
      "created output image. If it is left blank (or with its default <Auto> value), the platform will assign an automatic "
      "image identifier.</p>" );
   ImageId_Edit.OnGetFocus( (Control::event_handler)&PixelMathInterface::e_EditGetFocus, w );
   ImageId_Edit.OnEditCompleted( (Edit::edit_event_handler)&PixelMathInterface::e_EditCompleted, w );

   ImageId_Button.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   ImageId_Button.SetScaledFixedSize( 20, 20 );
   ImageId_Button.SetToolTip( "<p>Clear the image identifier</p>" );
   ImageId_Button.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   ImageId_Sizer.SetSpacing( 4 );
   ImageId_Sizer.Add( ImageId_Label );
   ImageId_Sizer.Add( ImageId_Edit, 100 );
   ImageId_Sizer.Add( ImageId_Button );

   //

   const char* widthToolTip = "<p>Width in pixels of the output image, when the <i>create new image</i> option is selected.</p>";

   Width_Label.SetText( "Image width:" );
   Width_Label.SetMinWidth( labelWidth1 );
   Width_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Width_Label.SetToolTip( widthToolTip );

   Width_SpinBox.SetRange( 0, int_max );
   Width_SpinBox.SetMinimumValueText( "<As target>" );
   Width_SpinBox.SetToolTip( widthToolTip );
   Width_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PixelMathInterface::e_SpinBoxValueUpdated, w );

   Width_Sizer.SetSpacing( 4 );
   Width_Sizer.Add( Width_Label );
   Width_Sizer.Add( Width_SpinBox );
   Width_Sizer.AddStretch();

   //

   const char* heightToolTip = "<p>Height in pixels of the output image, when the <i>create new image</i> option is selected.</p>";

   Height_Label.SetText( "Image height:" );
   Height_Label.SetMinWidth( labelWidth1 );
   Height_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Height_Label.SetToolTip( heightToolTip );

   Height_SpinBox.SetRange( 0, int_max );
   Height_SpinBox.SetMinimumValueText( "<As target>" );
   Height_SpinBox.SetToolTip( heightToolTip );
   Height_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PixelMathInterface::e_SpinBoxValueUpdated, w );

   Height_Sizer.SetSpacing( 4 );
   Height_Sizer.Add( Height_Label );
   Height_Sizer.Add( Height_SpinBox );
   Height_Sizer.AddStretch();

   //

   const char* colorSpaceToolTip = "<p>Color space of the output image, when the <i>create new image</i> option is selected.</p>";

   ColorSpace_Label.SetText( "Color space:" );
   ColorSpace_Label.SetMinWidth( labelWidth1 );
   ColorSpace_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ColorSpace_Label.SetToolTip( colorSpaceToolTip );

   ColorSpace_ComboBox.AddItem( "<Same as target>" );
   ColorSpace_ComboBox.AddItem( "RGB Color" );
   ColorSpace_ComboBox.AddItem( "Grayscale" );
   ColorSpace_ComboBox.SetMinWidth( comboWidth1 );
   ColorSpace_ComboBox.SetToolTip( colorSpaceToolTip );
   ColorSpace_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PixelMathInterface::e_ComboBoxItemSelected, w );

   ColorSpace_Sizer.SetSpacing( 4 );
   ColorSpace_Sizer.Add( ColorSpace_Label );
   ColorSpace_Sizer.Add( ColorSpace_ComboBox );
   ColorSpace_Sizer.AddStretch();

   //

   CreateAlpha_CheckBox.SetText( "Alpha channel" );
   CreateAlpha_CheckBox.SetToolTip( "<p>Create an alpha channel in the output image, when the <i>create new image</i> option is "
      "selected. This option should be enabled when an alpha channel PixelMath expression has been defined and the process is "
      "being executed on the global context. Otherwise the alpha channel expression will be ignored.</p>" );
   CreateAlpha_CheckBox.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   CreateAlpha_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CreateAlpha_Sizer.Add( CreateAlpha_CheckBox );
   CreateAlpha_Sizer.AddStretch();

   //

   const char* sampleFormatToolTip = "<p>Pixel sample format of the output image, when the <i>create new image</i> option is selected.</p>";

   SampleFormat_Label.SetText( "Sample format:" );
   SampleFormat_Label.SetMinWidth( labelWidth1 );
   SampleFormat_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SampleFormat_Label.SetToolTip( sampleFormatToolTip );

   SampleFormat_ComboBox.AddItem( "<Same as target>" );
   SampleFormat_ComboBox.AddItem( "8-bit unsigned integer" );
   SampleFormat_ComboBox.AddItem( "16-bit unsigned integer" );
   SampleFormat_ComboBox.AddItem( "32-bit unsigned integer" );
   SampleFormat_ComboBox.AddItem( "32-bit IEEE 754 floating point" );
   SampleFormat_ComboBox.AddItem( "64-bit IEEE 754 floating point" );
   SampleFormat_ComboBox.SetMinWidth( comboWidth1 );
   SampleFormat_ComboBox.SetToolTip( sampleFormatToolTip );
   SampleFormat_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PixelMathInterface::e_ComboBoxItemSelected, w );

   SampleFormat_Sizer.SetSpacing( 4 );
   SampleFormat_Sizer.Add( SampleFormat_Label );
   SampleFormat_Sizer.Add( SampleFormat_ComboBox );
   SampleFormat_Sizer.AddStretch();

   //

   DestinationLeft_Sizer.SetSpacing( 4 );
   DestinationLeft_Sizer.Add( GenerateOutput_Sizer );
   DestinationLeft_Sizer.Add( CacheGeneratedImages_Sizer );
   DestinationLeft_Sizer.Add( SingleThreaded_Sizer );
   DestinationLeft_Sizer.Add( Optimization_Sizer );
   DestinationLeft_Sizer.Add( Use64BitWorkingImage_Sizer );
   DestinationLeft_Sizer.Add( Rescale_Sizer );
   DestinationLeft_Sizer.Add( RescaleLower_NumericEdit );
   DestinationLeft_Sizer.Add( RescaleUpper_NumericEdit );
   DestinationLeft_Sizer.Add( ReplaceTarget_Sizer );
   DestinationLeft_Sizer.Add( CreateNewImage_Sizer );
   DestinationLeft_Sizer.Add( ImageId_Sizer );
   DestinationLeft_Sizer.Add( Width_Sizer );
   DestinationLeft_Sizer.Add( Height_Sizer );
   DestinationLeft_Sizer.Add( ColorSpace_Sizer );
   DestinationLeft_Sizer.Add( CreateAlpha_Sizer );
   DestinationLeft_Sizer.Add( SampleFormat_Sizer );

   //

   ClearGeneratedImages_Button.SetText( "Clear Image Cache" );
   ClearGeneratedImages_Button.SetToolTip( "<p>Destroy all cached images that have been created by generators "
      "in previously executed PixelMath expressions.</p>" );
   ClearGeneratedImages_Button.OnClick( (Button::click_event_handler)&PixelMathInterface::e_ButtonClick, w );

   //

   DestinationRight_Sizer.SetSpacing( 4 );
   DestinationRight_Sizer.Add( ClearGeneratedImages_Button );
   DestinationRight_Sizer.AddStretch();

   //

   Destination_Sizer.SetSpacing( 16 );
   Destination_Sizer.Add( DestinationLeft_Sizer, 100 );
   Destination_Sizer.Add( DestinationRight_Sizer );

   Destination_Control.SetSizer( Destination_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( Expression_SectionBar );
   Global_Sizer.Add( Expression_Control, 100 );
   Global_Sizer.Add( Destination_SectionBar );
   Global_Sizer.Add( Destination_Control );

   w.SetSizer( Global_Sizer );

   Destination_Control.Hide();

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetMinSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PixelMathInterface.cpp - Released 2021-05-05T15:38:07Z
