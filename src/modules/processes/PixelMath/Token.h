//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// Token.h - Released 2024-06-18T15:49:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
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

#ifndef __Token_h
#define __Token_h

#include <pcl/IndirectArray.h>
#include <pcl/String.h>
#include <pcl/StringList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * A token in a raw PixelMath expression analysis. Tokens are generated and
 * consumed by expression parser routines.
 *
 * See Expression::Parse() and the Tokenize() function in Token.cpp.
 */
class Token
{
public:

   enum { Symbol, MetaSymbol, Numeric, Separator };

   Token( const String& str, int pos, bool meta = false )
      : m_type( meta ? MetaSymbol : Symbol )
      , m_pos( pos )
      , m_stringValue( str )
   {
   }

   Token( double num, int pos )
      : m_type( Numeric )
      , m_pos( pos )
      , m_numericValue( num )
   {
   }

   Token( char16_type sep, int pos )
      : m_type( Separator )
      , m_pos( pos )
      , m_separatorValue( sep )
   {
   }

   int Type() const
   {
      return m_type;
   }

   bool IsSymbol() const
   {
      return m_type == Symbol;
   }

   bool IsMetaSymbol() const
   {
      return m_type == MetaSymbol;
   }

   bool IsNumeric() const
   {
      return m_type == Numeric;
   }

   bool IsSeparator() const
   {
      return m_type == Separator;
   }

   int Position() const
   {
      return m_pos;
   }

   String ToString() const
   {
      if ( IsSymbol() || IsMetaSymbol() )
         return m_stringValue;
      if ( IsNumeric() )
         return String( m_numericValue );
      if ( IsSeparator() )
         return String( m_separatorValue, 1 );
      return String();
   }

   const String& AsString() const
   {
      return m_stringValue;
   }

   double AsNumeric() const
   {
      return m_numericValue;
   }

   char16_type AsSeparator() const
   {
      return m_separatorValue;
   }

private:

   int         m_type;
   int         m_pos;
   String      m_stringValue;
   double      m_numericValue   = 0;
   char16_type m_separatorValue = 0;
};

// ----------------------------------------------------------------------------

class Directive
{
public:

   Directive( const String& name, const StringList& arguments, int pos, const Array<int>& argPos )
      : m_name( name )
      , m_arguments( arguments )
      , m_pos( pos )
      , m_argPos( argPos )
   {
   }

   const String& Name() const
   {
      return m_name;
   }

   const StringList& Arguments() const
   {
      return m_arguments;
   }

   int Position() const
   {
      return m_pos;
   }

   int ArgumentPosition( int i ) const
   {
      return m_argPos[Range( i, 0, int( m_argPos.Length() ) )];
   }

private:

   String     m_name;
   StringList m_arguments;
   int        m_pos;
   Array<int> m_argPos;

};

// ----------------------------------------------------------------------------

typedef IndirectArray<Token>     TokenList;
typedef Array<TokenList>         TokenSet;

typedef IndirectArray<Directive> DirectiveList;

void Tokenize( TokenSet& tokens, DirectiveList& directives, const String& text );

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Token_h

// ----------------------------------------------------------------------------
// EOF Token.h - Released 2024-06-18T15:49:25Z
