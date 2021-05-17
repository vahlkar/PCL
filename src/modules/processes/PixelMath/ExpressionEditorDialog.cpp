//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// ExpressionEditorDialog.cpp - Released 2021-05-05T15:38:07Z
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

#include "Data.h"
#include "ExpressionEditorDialog.h"
#include "Function.h"
#include "Operator.h"
#include "PixelMathInstance.h"

#include <pcl/ErrorHandler.h>

#define NO_ITEM_TEXT "<i>Select an image, a symbol, or a PixelMath function or operator.</i>"

#define MIN_WIDTH_LEFT_PANEL      600
#define MIN_WIDTH_RIGHT_PANEL     250
#define MIN_HEIGHT_SYNTAX_TEXTBOX 150

namespace pcl
{

// ----------------------------------------------------------------------------

struct SyntaxItem
{
   String                     meta;
   String                     token;
   String                     info;
   const Function::IndexNode* fn = nullptr;
   const Operator::IndexNode* op = nullptr;

   SyntaxItem( const String& m, const Function::IndexNode& f )
      : meta( m )
      , token( f.token )
      , info( f.function->Id() )
      , fn( &f )
   {
   }

   SyntaxItem( const String& m, const Operator::IndexNode& o )
      : meta( m )
      , token( o.token )
      , info( o.op->Id() )
      , op( &o )
   {
   }

   SyntaxItem( const String& m, const String& t, const String& s )
      : meta( m )
      , token( t )
      , info( s )
   {
   }

   SyntaxItem( const SyntaxItem& ) = default;

   bool operator ==( const SyntaxItem& x ) const
   {
      return token.CompareIC( x.token ) == 0;
   }

   bool operator <( const SyntaxItem& x ) const
   {
      return token.CompareIC( x.token ) < 0;
   }
};

static Function::function_set    s_functions;
static Function::function_index  s_functionIndex;

static Operator::operator_set    s_operators;
static Operator::operator_index  s_operatorIndex;

typedef SortedArray<SyntaxItem>  syntax_item_set;
static syntax_item_set           s_syntaxItems;

static void InitializeSyntaxItems()
{
   if ( !s_syntaxItems.IsEmpty() )
      return;

   if ( s_functions.IsEmpty() )
      Function::InitializeList( s_functions, s_functionIndex );

   if ( s_operators.IsEmpty() )
      Operator::InitializeList( s_operators, s_operatorIndex );

   /*
    * Functions
    */
   for ( const Function::IndexNode& node : s_functionIndex )
   {
      if ( node.function->Token() != node.token ) // do not include alias function names
         continue;
      s_syntaxItems << SyntaxItem( node.function->Meta(), node );
   }

   /*
    * Operators
    */
   for ( const Operator::IndexNode& node : s_operatorIndex )
      s_syntaxItems << SyntaxItem( node.op->Meta(), node );

   /*
    * Punctuators
    */
   s_syntaxItems << SyntaxItem( "(expr)", "()",
                        "<p>Parentheses.</p>"
                        "<p>The expression <i>expr</i> is evaluated with the highest precedence.</p>" )

                 << SyntaxItem( "func( [expr1[, expr2[, ...]]] )", "()",
                        "<p>Function Call.</p>"
                        "<p>The optional expressions <i>expr1</i>[, <i>expr2</i>[, ...]] are evaluated in "
                        "sequence from left to right and passed as arguments in a call to function <i>func</i>. "
                        "The resulting expression evaluates to the return value of <i>func</i>.</p>" )

                 << SyntaxItem( "image[c]", "[]",
                        "<p>Channel Selector.</p>"
                        "<p>A channel of the specified image, whose index is <i>c</i>, is referenced. The "
                        "channel index <i>c</i> must be an integer literal &ge; 0.</p>"
                        "<p>If the specified channel does not exist in the specified image, a runtime error occurs.</p>" )

                 << SyntaxItem( "expr, expr", ",",
                        "<p>Comma Operator.</p>"
                        "<p>Used mainly to separate function arguments.</p>"
                        "<p>Also used in C-style <i>comma expressions</i>, where the result is the last evaluated "
                        "expression (i.e. the rightmost expression).</p>" )

                 << SyntaxItem( "expr; expr", ";",
                        "<p>Expression Separator.</p>"
                        "<p>Used to separate multiple PixelMath expressions.</p>" )

                 << SyntaxItem( "/* <comment> */", "/**/",
                        "<p>Block comment.</p>"
                        "<p>Comments are ignored by the PixelMath compiler. They can be used to document PixelMath "
                        "source code, providing human-readable information on the purpose of a PixelMath expression "
                        "and its implementation.</p>"
                        "<p>A block comment consists of the pair of tokens '/*' and '*/' and any text in between. "
                        "Unlike line comments, block comments can span across multiple text lines.</p>" )

                 << SyntaxItem( "// <comment>", "//",
                        "<p>Line comment.</p>"
                        "<p>Comments are ignored by the PixelMath compiler. They can be used to document PixelMath "
                        "source code, providing human-readable information on the purpose of a PixelMath expression "
                        "and its implementation.</p>"
                        "<p>A line comment consists of the token '//' and all the subsequent text up to either "
                        "the end of the current line, or to the end of the source code, whichever happens first.</p>" )

   /*
    * Symbol definition functions
    */
                 << SyntaxItem( "symbol = keyword_value( [image=$T,] keyword )", "keyword_value",
                        "<p>The value of a numeric or Boolean FITS header keyword in the specified image. "
                        "Boolean keywords generate 0 (false) or 1 (true) symbol values.</p>"
                        "<p>FITS keyword names are case-insensitive.</p>" )

                 << SyntaxItem( "symbol = keyword_defined( [image=$T,] keyword )", "keyword_defined",
                        "<p>The symbol value will be either one, if the specified FITS header keyword is defined "
                        "in the image, or zero if the keyword is not defined.</p>"
                        "<p>FITS keyword names are case-insensitive.</p>" )

                 << SyntaxItem( "symbol = property_value( [image=$T,] property )", "property_value",
                        "<p>The value of a vector or scalar XISF image property in the specified image.</p>"
                        "<p>XISF property names are case-sensitive.</p>" )

                 << SyntaxItem( "symbol = property_defined( [image=$T,] property )", "property_defined",
                        "<p>The symbol value will be either one, if the specified XISF property is defined in the image, "
                        "or zero if the property is not defined.</p>"
                        "<p>XISF property names are case-sensitive.</p>" )

                 << SyntaxItem( "symbol = envvar_value( var )", "envvar_value",
                        "<p>The value of the specified environment variable with name <i>var</i> converted to a scalar.</p>"
                        "<p>Environment variable names are case-sensitive on Linux/UNIX platforms, case-insensitive on Windows.</p>" )

                 << SyntaxItem( "symbol = envvar_defined( var )", "envvar_defined",
                        "<p>The symbol value will be either one, if the specified environment variable is currently defined "
                        "for the running process with a non-empty value, or zero otherwise.</p>"
                        "<p>Environment variable names are case-sensitive on Linux/UNIX platforms, case-insensitive on Windows.</p>" )

                 << SyntaxItem( "symbol = width( [image=$T] )", "width",
                        "<p>The width in pixels of the specified image.</p>" )

                 << SyntaxItem( "symbol = height( [image=$T] )", "height",
                        "<p>The width in pixels of the specified image.</p>" )

                 << SyntaxItem( "symbol = area( [image=$T] )", "area",
                        "<p>The area in square pixels of the specified image.</p>" )

                 << SyntaxItem( "symbol = invarea( [image=$T] )", "invarea",
                        "<p>The reciprocal of the area in square pixels of the specified image.</p>" )

                 << SyntaxItem( "symbol = iscolor( [image=$T] )", "iscolor",
                        "<p>The result is one if the specified image is in the RGB color space; zero if it is "
                        "a grayscale monochrome image.</p>" )

                 << SyntaxItem( "symbol = maximum( [image=$T[, channel]] )", "maximum",
                        "<p>The maximum pixel value in the specified image.</p>" )

                 << SyntaxItem( "symbol = minimum( [image=$T[, channel]] )", "minimum",
                        "<p>The minimum pixel value in the specified image.</p>" )

                 << SyntaxItem( "symbol = median( [image=$T[, channel]] )", "median",
                        "<p>The median pixel value in the specified image.</p>" )

                 << SyntaxItem( "symbol = mdev( [image=$T[, channel]] )", "mdev",
                        "<p>The median absolute deviation from the median (MAD) of the specified image.</p>" )

                 << SyntaxItem( "symbol = adev( [image=$T[, channel]] )", "adev",
                        "<p>The average absolute deviation from the median of the specified image.</p>" )

                 << SyntaxItem( "symbol = sdev( [image=$T[, channel]] )", "sdev",
                        "<p>The standard deviation from the mean of the specified image.</p>" )

                 << SyntaxItem( "symbol = mean( [image=$T[, channel]] )", "mean",
                        "<p>The arithmetic mean of pixel values in the specified image.</p>" )

                 << SyntaxItem( "symbol = modulus( [image=$T[, channel]] )", "modulus",
                        "<p>The modulus (sum of absolute values) of the specified image.</p>" )

                 << SyntaxItem( "symbol = ssqr( [image=$T[, channel]] )", "ssqr",
                        "<p>The sum of square pixel values of the specified image.</p>" )

                 << SyntaxItem( "symbol = asqr( [image=$T[, channel]] )", "asqr",
                        "<p>The mean of square pixel values of the specified image.</p>" )

                 << SyntaxItem( "symbol = pixel( [image=$T,] x, y )\n"
                                "symbol = pixel( image, x, y, channel )", "pixel",
                        "<p>The pixel or pixel sample of an image at the specified coordinates. Out-of-range image "
                        "coordinates generate zero symbol values.</p>" )

                 << SyntaxItem( "symbol = init( [value=0] )", "init",
                        "<p>Variable initialization.</p>"
                        "<p>The symbol will be declared as a thread-local variable with the specified initial value.</p>"
                        "<p>If no explicit initial value is specified, thread-local variables are initialized to zero by default.</p>" )

                 << SyntaxItem( "symbol = global( op[, value] )", "global",
                        "<p>Global variable initialization.</p>"
                        "<p>The symbol will be declared as a global variable with the specified global operator and initial value.</p>"
                        "<p>The 'op' global operator specification can be one of '+' and '*'. For example, 'n=global(+)' will declare a "
                        "global additive variable <i>n</i> with a default zero initial value.</p>"
                        "<p>Global variables can only play lvalue roles in expressions. In practice this means that they can only occur at "
                        "the left hand of an assignment operator. For example, if a global variable <i>n</i> has been declared, then an "
                        "expression such as 'x = $T + n' is illegal. Furthermore, global variables are always specialized for additive or "
                        "multiplicative operations. For example, if <i>n</i> has been declared as additive with 'n=global(+)', then "
                        "'n *= $T' is illegal, but 'n -= $T &lt; 0.01' is valid.</p>"
                        "<p>If no explicit initial value is specified, additive global variables are initialized to zero and "
                        "multiplicative global variables are initialized to one. The final values of all global variables are reported on "
                        "the process console after PixelMath execution.</p>" )

         /* Metasymbols */

                 << SyntaxItem( "$T\n"
                                "$target", "$T",
                        "<p>Target image.</p>"
                        "<p>The $T (or $target) metasymbol represents the target image when PixelMath is being executed in a view context.</p>"
                        "<p>$T is illegal when PixelMath is executed as a global process, i.e. when a new image is being generated instead of "
                        "replacing an existing one, since in these cases no target image exists.</p>" )

                 << SyntaxItem( "$M\n"
                                "$mask", "$M",
                        "<p>Target mask image.</p>"
                        "<p>The $M (or $mask) metasymbol represents the mask image currently selected for the target image, when PixelMath "
                        "is being executed in a view context.</p>"
                        "<p>$M is illegal when PixelMath is being executed as a global process, for the same reason $T is also illegal in "
                        "this case: no target image exists for execution in the global context.</p>" )

                 << SyntaxItem( "$A\n"
                                "$active", "$A",
                        "<p>Active image.</p>"
                        "<p>The $A (or $active) metasymbol represents the active image in the current workspace, that is, the image in the "
                        "main view that has the keyboard focus when the instance of PixelMath is executed.</p>"
                        "<p>$A can always be used, in both the view and global execution contexts. If there is no active image in the current "
                        "workspace, a runtime error will occur if $A is used.</p>" )
                 ;
}

// ----------------------------------------------------------------------------

class SyntaxNode : public TreeBox::Node
{
public:

   SyntaxNode( Node& parent, int index, const SyntaxItem* item )
      : TreeBox::Node( parent, index )
      , m_item( item )
   {
      SetText( 0, m_item->token );
   }

   const SyntaxItem* Item() const
   {
      return m_item;
   }

private:

   const SyntaxItem* m_item;
};

// ----------------------------------------------------------------------------

class SymbolNode : public TreeBox::Node
{
public:

   SymbolNode( TreeBox& parent, const Symbol* symbol )
      : TreeBox::Node( parent )
   {
      SetText( 0, symbol->Id() );
      m_info = symbol->Id() + ": " + symbol->Information();
   }

   String SymbolInformation() const
   {
      return m_info;
   }

private:

   String m_info;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ExpressionEditorDialog::ExpressionEditorDialog()
{
   InitializeSyntaxItems();

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

   Editors_TabBox.SetScaledMinHeight( 250 );
   Editors_TabBox.AddPage( RK_Control, "RGB/K" );
   Editors_TabBox.AddPage( G_Control, "G" );
   Editors_TabBox.AddPage( B_Control, "B" );
   Editors_TabBox.AddPage( A_Control, "A" );
   Editors_TabBox.AddPage( Symbols_Control, "Symbols" );

   Editors_TabBox.SetPageIcon( 0, ScaledResource( ":/toolbar/image-display-red.png" ) );
   Editors_TabBox.SetPageIcon( 1, ScaledResource( ":/toolbar/image-display-green.png" ) );
   Editors_TabBox.SetPageIcon( 2, ScaledResource( ":/toolbar/image-display-blue.png" ) );
   Editors_TabBox.SetPageIcon( 3, ScaledResource( ":/toolbar/image-display-alpha.png" ) );
   Editors_TabBox.SetPageIcon( 4, ScaledResource( ":/icons/table.png" ) );

   VertSplitter_Control.SetScaledFixedHeight( 4 );
   VertSplitter_Control.EnableExpansion( true/*horzEnable*/, false/*vertEnable*/ );
   VertSplitter_Control.SetCursor( StdCursor::VerticalSplit );
   VertSplitter_Control.EnableMouseTracking();
   VertSplitter_Control.OnMousePress( (Control::mouse_button_event_handler)&ExpressionEditorDialog::e_MousePress, *this );
   VertSplitter_Control.OnMouseRelease( (Control::mouse_button_event_handler)&ExpressionEditorDialog::e_MouseRelease, *this );
   VertSplitter_Control.SetStyleSheet( ScaledStyleSheet( "QWidget { border: 1px solid darkgray; }" ) );
   VertSplitter_Control.BringToFront();

   String codeStyleSheet = ScaledStyleSheet(
         "QWidget {"
            "font-family: Hack, DejaVu Sans Mono, Monospace;"
            "font-size: 9pt;"
         "}"
      );

   Syntax_TextBox.SetReadOnly();
   Syntax_TextBox.SetScaledMinHeight( MIN_HEIGHT_SYNTAX_TEXTBOX );
   Syntax_TextBox.SetStyleSheet( codeStyleSheet );
   Syntax_TextBox.SetText( NO_ITEM_TEXT );

   Parser_TextBox.SetReadOnly();
   Parser_TextBox.SetScaledFixedHeight( 150 );
   Parser_TextBox.SetStyleSheet( codeStyleSheet );

   TopLeft_Sizer.Add( Editors_TabBox, 100 );
   TopLeft_Sizer.AddSpacing( 4 );
   TopLeft_Sizer.Add( VertSplitter_Control );
   TopLeft_Sizer.AddSpacing( 4 );
   TopLeft_Sizer.Add( Syntax_TextBox );
   TopLeft_Sizer.AddSpacing( 4 );
   TopLeft_Sizer.Add( Parser_TextBox );

   TopLeft_Control.SetSizer( TopLeft_Sizer );
   TopLeft_Control.SetScaledMinWidth( MIN_WIDTH_LEFT_PANEL );

   HorzSplitter_Control.SetScaledFixedWidth( 4 );
   HorzSplitter_Control.EnableExpansion( false/*horzEnable*/, true/*vertEnable*/ );
   HorzSplitter_Control.SetCursor( StdCursor::HorizontalSplit );
   HorzSplitter_Control.EnableMouseTracking();
   HorzSplitter_Control.OnMousePress( (Control::mouse_button_event_handler)&ExpressionEditorDialog::e_MousePress, *this );
   HorzSplitter_Control.OnMouseRelease( (Control::mouse_button_event_handler)&ExpressionEditorDialog::e_MouseRelease, *this );
   HorzSplitter_Control.SetStyleSheet( ScaledStyleSheet( "QWidget { border: 1px solid darkgray; }" ) );
   HorzSplitter_Control.BringToFront();

   Images_Label.SetText( "Images" );

   Images_TreeBox.HideHeader();
   Images_TreeBox.SetNumberOfColumns( 1 );
   Images_TreeBox.EnableRootDecoration();
   Images_TreeBox.EnableAlternateRowColor();
   Images_TreeBox.SetStyleSheet( codeStyleSheet );
   Images_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&ExpressionEditorDialog::e_CurrentNodeUpdated, *this );
   Images_TreeBox.OnNodeClicked( (TreeBox::node_event_handler)&ExpressionEditorDialog::e_CurrentNodeUpdated, *this );
   Images_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&ExpressionEditorDialog::e_NodeActivated, *this );

   Symbols_Label.SetText( "Symbols" );

   Symbols_TreeBox.HideHeader();
   Symbols_TreeBox.SetNumberOfColumns( 1 );
   Symbols_TreeBox.EnableRootDecoration();
   Symbols_TreeBox.EnableAlternateRowColor();
   Symbols_TreeBox.SetStyleSheet( codeStyleSheet );
   Symbols_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&ExpressionEditorDialog::e_CurrentNodeUpdated, *this );
   Symbols_TreeBox.OnNodeClicked( (TreeBox::node_event_handler)&ExpressionEditorDialog::e_CurrentNodeUpdated, *this );
   Symbols_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&ExpressionEditorDialog::e_NodeActivated, *this );

   Syntax_Label.SetText( "Syntax" );

   Syntax_TreeBox.HideHeader();
   Syntax_TreeBox.SetNumberOfColumns( 1 );
   Syntax_TreeBox.EnableRootDecoration();
   Syntax_TreeBox.EnableAlternateRowColor();
   Syntax_TreeBox.SetStyleSheet( codeStyleSheet );
   Syntax_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&ExpressionEditorDialog::e_CurrentNodeUpdated, *this );
   Syntax_TreeBox.OnNodeClicked( (TreeBox::node_event_handler)&ExpressionEditorDialog::e_CurrentNodeUpdated, *this );
   Syntax_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&ExpressionEditorDialog::e_NodeActivated, *this );

   TopRight_Sizer.Add( Images_Label );
   TopRight_Sizer.AddSpacing( 2 );
   TopRight_Sizer.Add( Images_TreeBox, 30 );
   TopRight_Sizer.AddSpacing( 4 );
   TopRight_Sizer.Add( Symbols_Label );
   TopRight_Sizer.AddSpacing( 2 );
   TopRight_Sizer.Add( Symbols_TreeBox, 20 );
   TopRight_Sizer.AddSpacing( 4 );
   TopRight_Sizer.Add( Syntax_Label );
   TopRight_Sizer.AddSpacing( 2 );
   TopRight_Sizer.Add( Syntax_TreeBox, 50 );

   TopRight_Control.SetSizer( TopRight_Sizer );
   TopRight_Control.SetScaledMinWidth( MIN_WIDTH_RIGHT_PANEL );

   Top_Sizer.Add( TopLeft_Control, 100 );
   Top_Sizer.AddSpacing( 4 );
   Top_Sizer.Add( HorzSplitter_Control );
   Top_Sizer.AddSpacing( 4 );
   Top_Sizer.Add( TopRight_Control );

   Parse_PushButton.SetText( "Parse" );
   Parse_PushButton.OnClick( (Button::click_event_handler)&ExpressionEditorDialog::e_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&ExpressionEditorDialog::e_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&ExpressionEditorDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.Add( Parse_PushButton );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Top_Sizer, 100 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();

   // Let the splitters be operational by default
   Resize( Width() + LogicalPixelsToPhysical( 40 ), Height() + LogicalPixelsToPhysical( 40 ) );

   // Splitter actions
   EnableMouseTracking();
   OnMouseMove( (Control::mouse_event_handler)&ExpressionEditorDialog::e_MouseMove, *this );

   // Manage Enter/Return keys pressed in code editors
   OnKeyPress( (Control::keyboard_event_handler)&ExpressionEditorDialog::e_KeyPress, *this );

   SetWindowTitle( "PixelMath Expression Editor" );
}

// ----------------------------------------------------------------------------

int ExpressionEditorDialog::Execute( PixelMathInstance& instance )
{
   RK_CodeEditor.SetText( instance.p_expression[0] );
   RK_CodeEditor.SetCursorPosition( 0, 0 );

   G_CodeEditor.SetText( instance.p_expression[1] );
   G_CodeEditor.SetCursorPosition( 0, 0 );

   B_CodeEditor.SetText( instance.p_expression[2] );
   B_CodeEditor.SetCursorPosition( 0, 0 );

   A_CodeEditor.SetText( instance.p_expression[3] );
   A_CodeEditor.SetCursorPosition( 0, 0 );

   Symbols_CodeEditor.SetText( instance.p_symbols );
   Symbols_CodeEditor.SetCursorPosition( 0, 0 );

   Parser_TextBox.Clear();

   RebuildImagesTree();
   RebuildSymbolsTree();

   if ( Syntax_TreeBox.NumberOfChildren() == 0 )
      RebuildSyntaxTree();

   if ( instance.p_useSingleExpression )
   {
      if ( Editors_TabBox.CurrentPageIndex() == 1 || Editors_TabBox.CurrentPageIndex() == 2 )
         Editors_TabBox.SetCurrentPageIndex( 0 );
      Editors_TabBox.SetPageLabel( 0, "RGB/K" );
      Editors_TabBox.SetPageIcon( 0, ScaledResource( ":/toolbar/image-display-rgb.png" ) );
      Editors_TabBox.DisablePage( 1 );
      Editors_TabBox.DisablePage( 2 );
   }
   else
   {
      Editors_TabBox.SetPageLabel( 0, "R/K" );
      Editors_TabBox.SetPageIcon( 0, ScaledResource( ":/toolbar/image-display-red.png" ) );
      Editors_TabBox.EnablePage( 1 );
      Editors_TabBox.EnablePage( 2 );
   }

   if ( Dialog::Execute() == StdDialogCode::Ok )
   {
      instance.p_expression[0] = RK_CodeEditor.Text().Trimmed();
      instance.p_expression[1] = G_CodeEditor.Text().Trimmed();
      instance.p_expression[2] = B_CodeEditor.Text().Trimmed();
      instance.p_expression[3] = A_CodeEditor.Text().Trimmed();
      instance.p_symbols       = Symbols_CodeEditor.Text().Trimmed();
      return StdDialogCode::Ok;
   }

   return StdDialogCode::Cancel;
}

// ----------------------------------------------------------------------------

void ExpressionEditorDialog::RebuildImagesTree()
{
   Images_TreeBox.DisableUpdates();
   Images_TreeBox.Clear();

   Array<ImageWindow> windows = ImageWindow::AllWindows();
   SortedStringList imageIds;
   for ( const ImageWindow& window : windows )
      imageIds << window.MainView().Id();
   for ( const String& id : imageIds )
      (new TreeBox::Node( Images_TreeBox ))->SetText( 0, id );

   Images_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void ExpressionEditorDialog::RebuildSymbolsTree()
{
   Symbols_TreeBox.DisableUpdates();
   Symbols_TreeBox.Clear();

   try
   {
      Symbol::DestroyAll();
      Symbol::Create( Symbols_CodeEditor.Text().Trimmed() );

      for ( const Symbol* symbol : Symbol::Symbols() )
         new SymbolNode( Symbols_TreeBox, symbol );

      Symbol::DestroyAll();
   }
   catch ( ... )
   {
      Symbol::DestroyAll();

      try
      {
         throw;
      }
      catch ( ParseError& x )
      {
         Parser_TextBox.SetText( "<nowrap>" + x.FormatInfo() + "</nowrap><beg>" );
      }
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER
      }
   }

   Symbols_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void ExpressionEditorDialog::RebuildSyntaxTree()
{
   Syntax_TreeBox.DisableUpdates();
   Syntax_TreeBox.Clear();

   TreeBox::Node* functionsNode = new TreeBox::Node( Syntax_TreeBox );
   functionsNode->SetText( 0, "Functions" );

   TreeBox::Node* generatorsNode = new TreeBox::Node( Syntax_TreeBox );
   generatorsNode->SetText( 0, "Generators" );

   TreeBox::Node* operatorsNode = new TreeBox::Node( Syntax_TreeBox );
   operatorsNode->SetText( 0, "Operators" );

   TreeBox::Node* punctuatorsNode = new TreeBox::Node( Syntax_TreeBox );
   punctuatorsNode->SetText( 0, "Punctuators" );

   TreeBox::Node* symDefFunctionsNode = new TreeBox::Node( Syntax_TreeBox );
   symDefFunctionsNode->SetText( 0, "Symbol Definition" );

   TreeBox::Node* metasymbolsNode = new TreeBox::Node( Syntax_TreeBox );
   metasymbolsNode->SetText( 0, "Metasymbols" );

   for ( const SyntaxItem& item : s_syntaxItems )
   {
      TreeBox::Node* node;
      if ( item.op != nullptr )
         node = operatorsNode;
      else if ( item.fn != nullptr )
         node = item.fn->function->IsImageGenerator() ? generatorsNode : functionsNode;
      else if ( item.meta.StartsWith( "symbol =" ) )
         node = symDefFunctionsNode;
      else if ( item.meta.StartsWith( "$" ) )
         node = metasymbolsNode;
      else
         node = punctuatorsNode;

      new SyntaxNode( *node, node->NumberOfChildren(), &item );
   }

   Syntax_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void ExpressionEditorDialog::ParseSymbols()
{
   String text;

   try
   {
      Symbol::DestroyAll();
      Symbol::Create( Symbols_CodeEditor.Text().Trimmed() );

      Symbols_TreeBox.DisableUpdates();
      Symbols_TreeBox.Clear();
      for ( const Symbol* symbol : Symbol::Symbols() )
         new SymbolNode( Symbols_TreeBox, symbol );
      Symbols_TreeBox.EnableUpdates();

      if ( Symbol::Symbols().IsEmpty() )
         text = "No symbols have been defined.";
      else
         for ( const Symbol* symbol : Symbol::Symbols() )
            text << symbol->Id() << ":\n   " << symbol->Information() << '\n';

      Symbol::DestroyAll();
   }
   catch ( ParseError& x )
   {
      Symbol::DestroyAll();

      text += "<end><cbr>";
      text += x.FormatInfo();
   }
   catch ( ... )
   {
      Symbol::DestroyAll();

      try
      {
         throw;
      }
      ERROR_HANDLER
   }

   Parser_TextBox.SetText( "<nowrap>" + text + "</nowrap><beg>" );
}

// ----------------------------------------------------------------------------

void ExpressionEditorDialog::ParseExpression()
{
   String text;

   try
   {
      const CodeEditor* editor;
      switch ( Editors_TabBox.CurrentPageIndex() )
      {
      case 0: editor = &RK_CodeEditor; break;
      case 1: editor = &G_CodeEditor; break;
      case 2: editor = &B_CodeEditor; break;
      case 3: editor = &A_CodeEditor; break;
      default: // ?!
         return;
      }

      Symbol::DestroyAll();
      Symbol::Create( Symbols_CodeEditor.Text().Trimmed() );

      Symbols_TreeBox.DisableUpdates();
      Symbols_TreeBox.Clear();
      for ( const Symbol* symbol : Symbol::Symbols() )
         new SymbolNode( Symbols_TreeBox, symbol );
      Symbols_TreeBox.EnableUpdates();

      Expression::component_list L;
      Expression::Parse( L, editor->Text().Trimmed() );

      Symbol::DestroyAll();

      if ( L.IsEmpty() )
         text = "Empty PixelMath expression.";
      else
      {
         text = "Parsed " + String( L.Length() ) + " PixelMath expression(s).\n";

         SortedStringList imageIds;

         for ( const Expression* i : L )
         {
            Expression::component_list RPN;

            const Functional* F = dynamic_cast<const Functional*>( i );
            if ( F != nullptr )
               F->PostOrder( RPN );
            else
               RPN.Add( i->Clone() );

            for ( const Expression* j : RPN )
            {
               const ImageReference* I = dynamic_cast<const ImageReference*>( j );
               if ( I != nullptr )
                  if ( dynamic_cast<const MetaSymbol*>( j ) == nullptr )
                     if ( !imageIds.Contains( I->Id() ) )
                        imageIds << I->Id();
            }

            RPN.Destroy();
         }

         if ( !imageIds.IsEmpty() )
         {
            text << "\n<b>" << String( imageIds.Length() ) << " referenced image(s):</b>\n";
            for ( const String& id : imageIds )
            {
               text << id << ": ";
               ImageWindow w = ImageWindow::WindowById( id );
               if ( w.IsNull() )
                  text << TextBox::PlainText( String( " <* not found *>" ) );
               else
               {
                  View v = w.MainView();
                  text.AppendFormat( "%dx%d, %s", v.Width(), v.Height(), v.IsColor() ? "RGB" : "Gray" );
               }
               text << '\n';
            }
         }

         int n = 0;
         for ( const Expression* x : L )
         {
            text << "\n<b>Expression #" << String( ++n ) << ":</b>\n"
                 << "<b>Infix:</b> "
                 << TextBox::PlainText( x->ToString() );
            const Functional* F = dynamic_cast<const Functional*>( x );
            if ( F != nullptr )
               text << "\n<b>Postfix:</b> "
                    << TextBox::PlainText( F->PostOrder() );
            text << '\n';
         }

         L.Destroy();
      }
   }
   catch ( ParseError& x )
   {
      Symbol::DestroyAll();

      text += "<end><cbr>";
      text += x.FormatInfo();
   }
   catch ( ... )
   {
      Symbol::DestroyAll();

      try
      {
         throw;
      }
      ERROR_HANDLER
   }

   Parser_TextBox.SetText( "<nowrap>" + text + "</nowrap><beg>" );
}

// ----------------------------------------------------------------------------

CodeEditor* ExpressionEditorDialog::CurrentEditor()
{
   switch ( Editors_TabBox.CurrentPageIndex() )
   {
   case 0:  return &RK_CodeEditor;
   case 1:  return &G_CodeEditor;
   case 2:  return &B_CodeEditor;
   case 3:  return &A_CodeEditor;
   case 4:  return &Symbols_CodeEditor;
   default: return nullptr; // ?!
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ExpressionEditorDialog::e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   String text = NO_ITEM_TEXT;

   if ( sender == Images_TreeBox )
   {
      text = "Image: " + current.Text( 0 );
      ImageWindow window = ImageWindow::WindowById( current.Text( 0 ) );
      if ( !window.IsNull() )
      {
         ImageVariant image = window.MainView().Image();
         text.AppendFormat( "\nw:%d h:%d n:%d %s %d-bit %s ",
                            image.Width(), image.Height(), image.NumberOfChannels(),
                            image.IsColor() ? "color" : "grayscale",
                            image.BitsPerSample(), image.IsFloatSample() ? "floating-point" : "integer" );
         text.Append( File::SizeAsString( image.ImageSize() ) );
      }
   }
   else if ( sender == Symbols_TreeBox )
   {
      const SymbolNode* node = dynamic_cast<const SymbolNode*>( &current );
      if ( node != nullptr )
         text = node->SymbolInformation();
   }
   else if ( sender == Syntax_TreeBox )
   {
      SyntaxNode* node = dynamic_cast<SyntaxNode*>( &current );
      if ( node != nullptr )
      {
         const SyntaxItem* item = node->Item();
         text = "<b><noentities><notags>" + item->meta + "</notags></noentities></b>\n\n<html>" + item->info + "</html>";
//          if ( item->fn != nullptr )
//          {
//             String aliases = item->fn->function->Aliases();
//             if ( !aliases.IsEmpty() )
//                text += "\n\nAliases: " + aliases;
//          }
//          else if ( item->op != nullptr )
//          {
//          }
      }
   }

   Syntax_TextBox.SetText( "<clr>" + text + "<beg>" );
}

// ----------------------------------------------------------------------------

void ExpressionEditorDialog::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   CodeEditor* editor = CurrentEditor();
   if ( editor != nullptr ) // ?!
   {
      String token;

      if ( sender == Images_TreeBox )
      {
         token = node.Text( 0 ); // image identifier
      }
      else if ( sender == Symbols_TreeBox )
      {
         token = node.Text( 0 ); // symbol identifier
      }
      else if ( sender == Syntax_TreeBox )
      {
         SyntaxNode* syntaxNode = dynamic_cast<SyntaxNode*>( &node );
         if ( syntaxNode != nullptr )
         {
            const SyntaxItem* item = syntaxNode->Item();
            if ( item->fn == nullptr && item->op == nullptr && (item->token == "()" || item->token == "[]") )
            {
               token = editor->SelectedText();
               if ( token.IsEmpty() )
                  token = item->token;
               else
                  token = item->token[0] + token + item->token[1];
            }
            else
               token = item->token;
         }
      }

      if ( !token.IsEmpty() )
      {
         editor->InsertText( token );
         editor->Focus();
      }
   }
}

// ----------------------------------------------------------------------------

void ExpressionEditorDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
   else if ( sender == Parse_PushButton )
   {
      if ( Editors_TabBox.CurrentPageIndex() == 4 )
         ParseSymbols();
      else
         ParseExpression();
   }
}

// ----------------------------------------------------------------------------

void ExpressionEditorDialog::e_KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey )
{
   wantsKey = key == KeyCode::Enter;
}

// ----------------------------------------------------------------------------

void ExpressionEditorDialog::e_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( sender == VertSplitter_Control )
   {
      m_splitterPressed = 1;
      m_splitterPos = sender.LocalToControl( *this, pos ).y;
   }
   else if ( sender == HorzSplitter_Control )
   {
      m_splitterPressed = 2;
      m_splitterPos = sender.LocalToControl( *this, pos ).x;
   }
}

// ----------------------------------------------------------------------------

void ExpressionEditorDialog::e_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   m_splitterPressed = 0;
}

// ----------------------------------------------------------------------------

void ExpressionEditorDialog::e_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers )
{
   switch ( m_splitterPressed )
   {
   case 0:
      break;
   case 1: // vertical
      {
         int dy = pos.y - m_splitterPos;
         if ( dy < 0 )
            if ( Editors_TabBox.Height() <= Editors_TabBox.MinHeight() )
               return;
         int newHeight = Max( LogicalPixelsToPhysical( MIN_HEIGHT_SYNTAX_TEXTBOX ), Syntax_TextBox.Height() - dy );
         if ( newHeight != Syntax_TextBox.Height() )
         {
            Syntax_TextBox.SetMinHeight( newHeight );
            EnsureLayoutUpdated();
            m_splitterPos = pos.y;
         }
      }
      break;
   case 2: // horizontal
      {
         int dx = pos.x - m_splitterPos;
         if ( dx < 0 )
            if ( TopLeft_Control.Width() <= TopLeft_Control.MinWidth() )
               return;
         int newWidth = Max( LogicalPixelsToPhysical( MIN_WIDTH_RIGHT_PANEL ), TopRight_Control.Width() - dx );
         if ( newWidth != TopRight_Control.Width() )
         {
            TopRight_Control.SetMinWidth( newWidth );
            EnsureLayoutUpdated();
            m_splitterPos = pos.x;
         }
      }
      break;
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExpressionEditorDialog.cpp - Released 2021-05-05T15:38:07Z
