//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// ExpressionEditorDialog.h - Released 2021-05-05T15:38:07Z
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

#ifndef __ExpressionEditorDialog_h
#define __ExpressionEditorDialog_h

#include <pcl/CodeEditor.h>
#include <pcl/Dialog.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/TabBox.h>
#include <pcl/TextBox.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PixelMathInstance;

class ExpressionEditorDialog : public Dialog
{
public:

   ExpressionEditorDialog();

   int Execute( PixelMathInstance& );

private:

   int m_splitterPressed = 0;
   int m_splitterPos = 0;

   VerticalSizer     Global_Sizer;
      HorizontalSizer   Top_Sizer;
         Control           TopLeft_Control;
            VerticalSizer     TopLeft_Sizer;
               TabBox            Editors_TabBox;
                  Control           RK_Control;
                  HorizontalSizer   RK_Sizer;
                     CodeEditor        RK_CodeEditor;
                  Control           G_Control;
                  HorizontalSizer   G_Sizer;
                     CodeEditor        G_CodeEditor;
                  Control           B_Control;
                  HorizontalSizer   B_Sizer;
                     CodeEditor        B_CodeEditor;
                  Control           A_Control;
                  HorizontalSizer   A_Sizer;
                     CodeEditor        A_CodeEditor;
                  Control           Symbols_Control;
                  HorizontalSizer   Symbols_Sizer;
                     CodeEditor        Symbols_CodeEditor;
               Control           VertSplitter_Control;
               TextBox           Syntax_TextBox;
               TextBox           Parser_TextBox;
         Control           HorzSplitter_Control;
         Control           TopRight_Control;
            VerticalSizer     TopRight_Sizer;
               Label             Images_Label;
               TreeBox           Images_TreeBox;
               Label             Symbols_Label;
               TreeBox           Symbols_TreeBox;
               Label             Syntax_Label;
               TreeBox           Syntax_TreeBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        Parse_PushButton;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void RebuildImagesTree();
   void RebuildSymbolsTree();
   void RebuildSyntaxTree();

   void ParseExpression();
   void ParseSymbols();

   CodeEditor* CurrentEditor();

   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_Click( Button& sender, bool checked );
   void e_KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   void e_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ExpressionEditorDialog_h

// ----------------------------------------------------------------------------
// EOF ExpressionEditorDialog.h - Released 2021-05-05T15:38:07Z
