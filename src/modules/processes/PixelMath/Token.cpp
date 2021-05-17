//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Token.cpp - Released 2021-05-05T15:38:07Z
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

// ----------------------------------------------------------------------------

#define PARSE_ERROR( msg )  throw ParseError( msg, s, j-s.Begin() )

void Tokenize( token_set& t, const String& s )
{
   for ( token_list& l : t )
      l.Destroy();
   t.Clear();

   token_list tokens;

   try
   {
      for ( String::const_iterator i = s.Begin(); i != s.End(); )
      {
         /*
          * Comments
          */
         if ( *i == '/' )
            if ( i+1 < s.End() )
               if ( i[1] == '*' )
               {
                  /*
                   * Block comment
                   */
                  size_type c = s.Find( "*/", (i-s.Begin())+1 );
                  if ( c == String::notFound )
                     throw ParseError( "Unmatched block comment delimiter", s, i-s.Begin() );
                  if ( (i = s.At( c+2 )) == s.End() )
                     break;
               }
               else if ( i[1] == '/' )
               {
                  /*
                   * Line comment
                   */
                  size_type e = s.Find( '\n', (i-s.Begin())+1 );
                  if ( e == String::notFound || (i = s.At( e+1 )) == s.End() )
                     break;
                  continue;
               }

         // Next token start
         String::const_iterator j = i;

         if ( IsoCharTraits::IsStartingSymbolDigit( *i ) )
         {
            /*
             * Symbol
             */
            for ( ; ++j != s.End() && IsoCharTraits::IsSymbolDigit( *j ); ) {}
            tokens << new Token( String( i, j ), i-s.Begin() );
         }
         else if ( IsMetaSpecifier( *i ) )
         {
            /*
             * Metasymbol
             */
            if ( ++j == s.End() )
               throw ParseError( "Expected a metasymbol identifier", s, i-s.Begin() );
            if ( !IsoCharTraits::IsStartingSymbolDigit( *j ) )
               throw ParseError( "Illegal metasymbol identifier", s, j-s.Begin() );
            for ( ; ++j != s.End() && IsoCharTraits::IsSymbolDigit( *j ); ) {}
            tokens << new Token( String( i+1, j ), i-s.Begin(), true );
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
                     PARSE_ERROR( "Misplaced decimal point" );
                  if ( decPoint )
                     PARSE_ERROR( "Too many decimal points" );
                  ++decPoint;
               }
               else if ( IsoCharTraits::IsSign( *j ) )
               {
                  if ( exponent )
                  {
                     if ( j != exponentPos+1 )
                        PARSE_ERROR( "Misplaced exponent sign" );
                  }
                  else if ( j != i )
                     break;
               }
               else if ( IsoCharTraits::IsExponentDelimiter( *j ) )
               {
                  if ( digCount == 0 )
                     PARSE_ERROR( "Misplaced exponent separator" );
                  if ( exponent )
                     PARSE_ERROR( "Too many exponent separators" );
                  exponentPos = j;
                  ++exponent;
                  digCount = 0;
               }
               else
               {
                  if ( !(IsSeparator( *j ) || IsoCharTraits::IsSpace( *j )) )
                     PARSE_ERROR( "Invalid character in numeric literal" );
                  break;
               }

               if ( ++j == s.End() )
                  break;
            }

            if ( digCount == 0 )
               PARSE_ERROR( "Digit(s) expected" );

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
            if ( errorPtr != nullptr && *errorPtr != 0 )
               throw ParseError( "Invalid numeral", s, i-s.Begin() + errorPtr-numStr.c_str() );

            tokens << new Token( x, i-s.Begin() );
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
               tokens << new Token( *i, i-s.Begin() );

            j = i+1;
         }
         else if ( IsoCharTraits::IsSpace( *i ) )
         {
            /*
             * Whitespace
             */
            while ( ++j != s.End() && IsoCharTraits::IsSpace( *j ) ) {}
         }
         else
         {
            /*
             * Illegal token
             */
            throw ParseError( "Invalid character in expression", s, i-s.Begin() );
         }

         i = j;
      }

      t << tokens;
   }
   catch ( ... )
   {
      tokens.Destroy();
      for ( token_list& l : t )
         l.Destroy();
      t.Clear();
      throw;
   }
}

#undef PARSE_ERROR

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Token.cpp - Released 2021-05-05T15:38:07Z
