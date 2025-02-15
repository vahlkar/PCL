//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Exception.cpp - Released 2024-12-28T16:53:56Z
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

#include <pcl/api/APIInterface.h>

#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/MessageBox.h>
#include <pcl/Thread.h>

#include <iostream>

namespace pcl
{

// ----------------------------------------------------------------------------

static bool s_useConsole = true;
static bool s_useGUI = false;

// ----------------------------------------------------------------------------

static bool HaveConsole()
{
   return API != nullptr && (*API->Global->GetConsole)() != nullptr;
}

// ----------------------------------------------------------------------------

static String TranslateHTMLParagraphTags( const String& s )
{
   String r;
   for ( size_type p0 = 0; ; )
   {
      size_type p = s.FindIC( "<p", p0 );
      if ( p == String::notFound )
      {
         r << s.Substring( p0 );
         break;
      }

      if ( p != p0 )
         r << s.Substring( p0, p-p0 ).Trimmed();

      r << '\n';

      size_type p1 = s.Find( '>', p+1 );
      if ( p1 == String::notFound )
         break;

      ++p1;

      size_type p2 = s.FindIC( "</p>", p1 );
      if ( p2 == String::notFound )
      {
         r << s.Substring( p1 );
         break;
      }

      if ( p2 != p1 )
         r << s.Substring( p1, p2-p1 ).Trimmed();

      r << '\n';

      p0 = p2 + 4;
   }

   return r;
}

// ----------------------------------------------------------------------------

static String TranslateHTMLBreakTags( const String& s )
{
   String r;
   for ( size_type p0 = 0; ; )
   {
      size_type p = s.FindIC( "<br", p0 );
      if ( p == String::notFound )
      {
         r << s.Substring( p0 );
         break;
      }

      if ( p != p0 )
         r << s.Substring( p0, p-p0 );

      r << '\n';

      size_type p1 = s.Find( '>', p+1 );
      if ( p1 == String::notFound )
         break;
      p0 = p1 + 1;
   }

   return r;
}

// ----------------------------------------------------------------------------

static String RemoveHTMLTags( const String& s )
{
   String r;
   for ( size_type p0 = 0, n = s.Length(); ; )
   {
      size_type p = s.Find( '<', p0 );
      if ( p == String::notFound )
      {
         r << s.Substring( p0 );
         return r;
      }

      size_type p1 = p;
      for ( ;; )
      {
         if ( ++p1 == n )
            return r << s.Substring( p0 );

         if ( s[p1] == '>' )
         {
            r << s.Substring( p0, (p1-p > 1) ? p-p0 : p1-p0+1 );
            break;
         }

         if ( (s[p1] < 'a' || s[p1] > 'z') && (s[p1] < 'A' || s[p1] > 'Z') && s[p1] != '/' )
         {
            r << s.Substring( p0, p1-p0+1 );
            break;
         }
      }

      p0 = p1 + 1;
   }

   return r;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

String Exception::FormatInfo() const
{
   String info = ExceptionClass();
   String msg = Message();

   if ( !msg.IsEmpty() )
   {
      if ( !info.IsEmpty() )
         info << ": ";
      info << msg;
   }

   return info;
}

// ----------------------------------------------------------------------------

String Exception::Caption() const
{
   return "PCL Exception";
}

// ----------------------------------------------------------------------------

void Exception::Show() const
{
   String caption = Caption();
   String info = FormatInfo();

   bool showOnGUI = s_useGUI && Thread::IsRootThread();

   if ( s_useConsole || !showOnGUI )
   {
      String text = caption + ": " + info;
      if ( HaveConsole() )
      {
         Console console;
         console.Show();
         console.Critical( "<end><cbr>*** " + text );
         console.Write( "\x1b[39m<end><cbr>" );
      }
      else
         std::cerr << "\n*** " << RemoveHTMLTags( TranslateHTMLParagraphTags( TranslateHTMLBreakTags( text ) ) ) << '\n' << std::flush;
   }

   if ( showOnGUI )
      pcl::MessageBox( info, caption, StdIcon::Error ).Execute();
}

// ----------------------------------------------------------------------------

bool Exception::IsConsoleOutputEnabled()
{
   return s_useConsole;
}

// ----------------------------------------------------------------------------

void Exception::EnableConsoleOutput( bool enable )
{
   s_useConsole = enable;
}

// ----------------------------------------------------------------------------

bool Exception::IsGUIOutputEnabled()
{
   return s_useGUI;
}

// ----------------------------------------------------------------------------

void Exception::EnableGUIOutput( bool enable )
{
   s_useGUI = enable;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

String ParseError::Message() const
{
   String info = m_message;

   if ( !m_beingParsed.IsEmpty() )
      if ( m_errorPosition >= 0 && size_type( m_errorPosition ) < m_beingParsed.Length() )
      {
         if ( !info.IsEmpty() )
            info << ":\n";

         int p0 = 0, p1 = 0;
         String::const_iterator i0 = m_beingParsed.Begin();
         for ( String::const_iterator i1 = m_beingParsed.Begin(); i1 < m_beingParsed.End(); ++i1, ++p1 )
         {
            if ( p1 == m_errorPosition )
            {
               for ( ; i1 < m_beingParsed.End(); ++i1 )
                  if ( *i1 == '\n' )
                     break;
               info << String( i0, i1 ) << '\n'
                    << String( '~', p1-p0  ) << '^';
               break;
            }
            if ( *i1 == '\n' )
            {
               info << String( i0, i1 ) << '\n';
               i0 = i1+1;
               p0 = p1+1;
            }
         }
      }
      else
      {
         if ( !info.IsEmpty() )
            info << ": ";
         info << m_beingParsed;
      }

   return info;
}

// ----------------------------------------------------------------------------

void ParseError::Show() const
{
   bool showOnGUI = s_useGUI && Thread::IsRootThread();

   if ( s_useConsole || !showOnGUI )
      if ( HaveConsole() )
      {
         Console console;
         console.Show();
         console.Critical( "<end><cbr>*** " + Caption() + ": <raw>" + Message() + "</raw>" );
         console.Write( "\x1b[39m<end><cbr>" );
      }
      else
         std::cerr << "\n*** " << Caption() << ": " << Message() << '\n';

   if ( showOnGUI )
   {
      String info = "<div style=\"white-space:pre; font-family:Hack;\">" + Message() + "</div>";
      String caption = Caption();
      pcl::MessageBox( info, caption, StdIcon::Error ).Execute();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

String SourceCodeError::Message() const
{
   String s = m_message;

   if ( m_lineNumber >= 0 || m_columnNumber >= 0 )
   {
      s << " (";

      if ( m_lineNumber >= 0 )
      {
         s.AppendFormat( "%d", m_lineNumber );
         if ( m_columnNumber >= 0 )
            s << ':';
      }

      if ( m_columnNumber >= 0 )
         s.AppendFormat( "%d", m_columnNumber );

      s << ")";
   }

   return s;
}

// ----------------------------------------------------------------------------

void SourceCodeError::Show() const
{
   bool showOnGUI = s_useGUI && Thread::IsRootThread();

   if ( s_useConsole || !showOnGUI )
      if ( HaveConsole() )
      {
         Console console;
         console.Show();
         console.Critical( "<end><cbr>*** " + Caption() + ": <raw>" + Message() + "</raw>" );
         console.Write( "\x1b[39m<end><cbr>" );
      }
      else
         std::cerr << "\n*** " << Caption() << ": " << Message() << '\n';

   if ( showOnGUI )
   {
      String info = "<div style=\"white-space:pre; font-family:Hack;\">" + Message() + "</div>";
      String caption = Caption();
      pcl::MessageBox( info, caption, StdIcon::Error ).Execute();
   }
}

// ----------------------------------------------------------------------------

}  // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Exception.cpp - Released 2024-12-28T16:53:56Z
