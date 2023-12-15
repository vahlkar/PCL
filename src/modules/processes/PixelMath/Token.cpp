//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.4
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// Token.cpp - Released 2023-12-01T19:16:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "Token.h"

#include <pcl/Exception.h>

#include <stdlib.h>

namespace pcl
{

// ----------------------------------------------------------------------------

inline static bool IsSeparator( char c )
{
   static const char* s = "+-*/%^~!|&<>=#()[]{},.:;";
   return ::strchr( s, c ) != nullptr;
}

inline static bool IsExpressionSeparator( char c )
{
   return c == ';';
}

inline static bool IsMetaSpecifier( char c )
{
   return c == '$';
}

inline static bool IsDirectiveSpecifier( char c )
{
   return c == '.';
}

static String::const_iterator SkipComment( const String& s, String::const_iterator i )
{
   if ( i < s.End() )
      if ( unlikely( *i == '/' ) )
         if ( i+1 < s.End() )
         {
            if ( i[1] == '*' )
            {
               /*
                * Block comment
                */
               size_type c = s.Find( "*/", (i-s.Begin())+1 );
               if ( c == String::notFound )
                  throw ParseError( "Unmatched block comment delimiter", s, i-s.Begin() );
               return s.At( c+2 );
            }

            if ( i[1] == '/' )
            {
               /*
                * Line comment
                */
               size_type e = s.Find( '\n', (i-s.Begin())+1 );
               return (e == String::notFound) ? s.End() : s.At( e+1 );
            }
         }

   return i;
}

struct BlockIndex
{
   distance_type codeStart;
   distance_type sourceStart;

   template <typename D1, typename D2>
   BlockIndex( D1 cs, D2 ss )
      : codeStart( distance_type( cs ) )
      , sourceStart( distance_type( ss ) )
   {
   }
};

static size_type SourcePosition( String::const_iterator i, const String& code, const Array<BlockIndex>& index )
{
   distance_type d = i - code.Begin();
   for ( const BlockIndex& b : ReverseIterable( index ) )
      if ( d >= b.codeStart )
         return d - b.codeStart + b.sourceStart;
   return 0; // ?!
}

// ----------------------------------------------------------------------------

#define PARSE_ERROR( msg, itr )  throw ParseError( msg, s, SourcePosition( itr, code, index ) )

void Tokenize( TokenSet& t, DirectiveList& d, const String& s )
{
   for ( TokenList& l : t )
      l.Destroy();
   t.Clear();
   d.Destroy();

   TokenList tokens;

   try
   {
      String code;
      Array<BlockIndex> index;

      /*
       * Extract comment-free code blocks and block index.
       */
      for ( String::const_iterator i = SkipComment( s, s.Begin() ), j = i; j != s.End(); )
      {
         String::const_iterator k = SkipComment( s, j );
         if ( j == k )
         {
            if ( ++j == s.End() )
            {
               index << BlockIndex( code.Length(), i-s.Begin() );
               code << String( i, j );
               break;
            }
         }
         else
         {
            index << BlockIndex( code.Length(), i-s.Begin() );
            code << String( i, j );
            i = j = k;
         }
      }

      /*
       * Process code blocks
       */
      for ( String::const_iterator i = code.Begin(); i != code.End(); )
      {
         if ( IsDirectiveSpecifier( *i ) )
         {
            if ( i+1 == code.End() )
               PARSE_ERROR( "Expected digit(s) or directive specification after '.'", i );

            if ( IsoCharTraits::IsStartingSymbolDigit( i[1] ) )
            {
               /*
                * Directive
                */
               if ( !tokens.IsEmpty() )
                  PARSE_ERROR( "Illegal directive specification inside expression", i );
               if ( !t.IsEmpty() )
                  PARSE_ERROR( "Illegal directive specification between expressions", i );

               // Look for mandatory terminating expression separator
               String::const_iterator j = ++i;
               while ( ++j != code.End() && !IsExpressionSeparator( *j ) ) {}
               if ( j == code.End() )
                  PARSE_ERROR( "Expected expression separator after directive specification", i );

               // Name separator: space or terminating separator
               String::const_iterator k = i;
               while ( ++k != j && !IsoCharTraits::IsSpace( *k ) ) {}

               // Directive name and starting position
               int pos = SourcePosition( i, code, index );
               String name( i, k );

               // Skip whitespace
               while ( k != j && IsoCharTraits::IsSpace( *++k ) ) {}

               // Comma-separated list of directive arguments
               StringList args;
               Array<int> argPos;
               int p = 0;
               for ( i = k; k != j; )
               {
                  bool newArg = false;
                  switch ( *k )
                  {
                  case '(':
                     ++p;
                     break;
                  case ')':
                     --p;
                     break;
                  case ',':
                     if ( p <= 0 )
                        newArg = true;
                     break;
                  default:
                     break;
                  }

                  if ( newArg || ++k == j )
                  {
                     // Skip initial whitespace
                     for ( ; i != k && IsoCharTraits::IsSpace( *i ); ++i ) {}
                     if ( i == k )
                        PARSE_ERROR( "Expected directive argument", i );
                     // New argument and position
                     String arg = String( i, k ).Trimmed();
                     args << arg;
                     argPos << SourcePosition( i, code, index );
                     if ( newArg )
                     {
                        if ( ++k == j )
                           PARSE_ERROR( "Expected directive argument", j );
                        i = k;
                     }
                  }
               }

               d << new Directive( name, args, pos, argPos );

               i = j;
               if ( i != code.End() )
                  ++i;
               continue;
            }
         }

         // Next token start
         String::const_iterator j = i;

         if ( IsoCharTraits::IsStartingSymbolDigit( *i ) )
         {
            /*
             * Symbol
             */
            for ( ; ++j != code.End() && IsoCharTraits::IsSymbolDigit( *j ); ) {}
            tokens << new Token( String( i, j ), SourcePosition( i, code, index ) );
         }
         else if ( IsMetaSpecifier( *i ) )
         {
            /*
             * Metasymbol
             */
            if ( ++j == code.End() )
               PARSE_ERROR( "Expected metasymbol identifier", i );
            if ( !IsoCharTraits::IsStartingSymbolDigit( *j ) )
               PARSE_ERROR( "Illegal metasymbol identifier", j );
            for ( ; ++j != code.End() && IsoCharTraits::IsSymbolDigit( *j ); ) {}
            tokens << new Token( String( i+1, j ), SourcePosition( i, code, index ), true );
         }
         else if ( IsoCharTraits::IsDigit( *i )
                || IsoCharTraits::IsDecimalSeparator( *i ) )
         {
            /*
             * Numeric literal
             */
            int digCount = 0, decPoint = 0, exponent = 0;
            String::const_iterator exponentPos = nullptr;

            for ( ;; )
            {
               if ( IsoCharTraits::IsDigit( *j ) )
               {
                  ++digCount;
               }
               else if ( IsoCharTraits::IsDecimalSeparator( *j ) )
               {
                  if ( exponent )
                     PARSE_ERROR( "Misplaced decimal point", j );
                  if ( decPoint )
                     PARSE_ERROR( "Too many decimal points", j );
                  ++decPoint;
               }
               else if ( IsoCharTraits::IsSign( *j ) )
               {
                  if ( exponent )
                  {
                     if ( j != exponentPos+1 )
                        PARSE_ERROR( "Misplaced exponent sign", j );
                  }
                  else if ( j != i )
                     break;
               }
               else if ( IsoCharTraits::IsExponentDelimiter( *j ) )
               {
                  if ( digCount == 0 )
                     PARSE_ERROR( "Misplaced exponent separator", j );
                  if ( exponent )
                     PARSE_ERROR( "Too many exponent separators", j );
                  exponentPos = j;
                  ++exponent;
                  digCount = 0;
               }
               else
               {
                  if ( !(IsSeparator( *j ) || IsoCharTraits::IsSpace( *j )) )
                     PARSE_ERROR( String().Format( "Invalid character '#%04X' in numeric literal", unsigned( *j ) ), j );
                  break;
               }

               if ( ++j == code.End() )
                  break;
            }

            if ( digCount == 0 )
               PARSE_ERROR( "Digit(s) expected", j );

            String numStr( i, j );
            String::iterator errorPtr = nullptr;
#ifdef __PCL_WINDOWS
            double x = ::wcstod( reinterpret_cast<wchar_t*>( numStr.Begin() ),
                                 &reinterpret_cast<wchar_t*&>( errorPtr ) );
#else
            IsoString iStr( numStr );
            IsoString::iterator iPtr = nullptr;
            double x = ::strtod( iStr.c_str(), &iPtr );
            if ( iPtr != nullptr && *iPtr != '\0' )
               errorPtr = numStr.Begin() + (iPtr - iStr.Begin());
#endif
            if ( errorPtr != nullptr )
               if ( *errorPtr != 0 )
                  PARSE_ERROR( "Invalid numeral", i + (errorPtr - numStr.c_str()) );

            tokens << new Token( x, SourcePosition( i, code, index ) );
         }
         else if ( IsSeparator( *i ) )
         {
            /*
             * Separator
             */
            if ( IsExpressionSeparator( *i ) )
            {
               t << tokens;
               tokens.Clear();
            }
            else
               tokens << new Token( *i, SourcePosition( i, code, index ) );

            j = i+1;
         }
         else if ( IsoCharTraits::IsSpace( *i ) )
         {
            /*
             * Whitespace
             */
            while ( ++j != code.End() && IsoCharTraits::IsSpace( *j ) ) {}
         }
         else
         {
            /*
             * Illegal character
             */
            PARSE_ERROR( String().Format( "Invalid character '#%04X' in expression", unsigned( *i ) ), i );
         }

         i = j;
      }

      t << tokens;
   }
   catch ( ... )
   {
      tokens.Destroy();
      for ( TokenList& l : t )
         l.Destroy();
      t.Clear();
      d.Destroy();
      throw;
   }
}

#undef PARSE_ERROR

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Token.cpp - Released 2023-12-01T19:16:18Z
