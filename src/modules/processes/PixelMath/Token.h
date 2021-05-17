//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Token.h - Released 2021-05-05T15:38:07Z
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

#ifndef __Token_h
#define __Token_h

#include <pcl/IndirectArray.h>
#include <pcl/String.h>

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

   Token( const String& s, int p, bool meta = false )
      : type( meta ? MetaSymbol : Symbol )
      , pos( p )
      , stringValue( s )
   {
   }

   Token( double v, int p )
      : type( Numeric )
      , pos( p )
      , numericValue( v )
   {
   }

   Token( char16_type c, int p )
      : type( Separator )
      , pos( p )
      , separatorValue( c )
   {
   }

   int Type() const
   {
      return type;
   }

   bool IsSymbol() const
   {
      return type == Symbol;
   }

   bool IsMetaSymbol() const
   {
      return type == MetaSymbol;
   }

   bool IsNumeric() const
   {
      return type == Numeric;
   }

   bool IsSeparator() const
   {
      return type == Separator;
   }

   int Position() const
   {
      return pos;
   }

   String ToString() const
   {
      if ( IsSymbol() || IsMetaSymbol() )
         return stringValue;
      if ( IsNumeric() )
         return String( numericValue );
      if ( IsSeparator() )
         return String( separatorValue, 1 );
      return String();
   }

   double AsNumeric() const
   {
      return numericValue;
   }

   char16_type AsSeparator() const
   {
      return separatorValue;
   }

private:

   int type;
   int pos;
   String stringValue;
   double numericValue        = 0;
   char16_type separatorValue = 0;
};

// ----------------------------------------------------------------------------

typedef IndirectArray<Token>  token_list;
typedef Array<token_list>     token_set;

void Tokenize( token_set&, const String& );

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Token_h

// ----------------------------------------------------------------------------
// EOF Token.h - Released 2021-05-05T15:38:07Z
