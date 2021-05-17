//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Symbol.h - Released 2021-05-05T15:38:07Z
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

#ifndef __Symbol_h
#define __Symbol_h

#include "Pixel.h"

#include <pcl/IndirectSortedArray.h>
#include <pcl/Mutex.h>
#include <pcl/StringList.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS ImageWindow;

class Expression;
class GlobalVariable;

// ----------------------------------------------------------------------------

/*
 * Representation of a symbolic object reference in a PixelMath expression.
 */
class Symbol : public Pixel
{
public:

   typedef IndirectSortedArray<Symbol> symbol_set;

   Symbol( const String& id );

   virtual ~Symbol()
   {
   }

   String Id() const
   {
      return m_id;
   }

   bool operator ==( const Symbol& x ) const
   {
      return m_id == x.m_id;
   }

   bool operator <( const Symbol& x ) const
   {
      return m_id < x.m_id;
   }

   virtual bool IsVariable() const
   {
      return false;
   }

   virtual bool IsGlobalVariable() const
   {
      return false;
   }

   virtual bool IsConstant() const
   {
      return false;
   }

   virtual String Information() const
   {
      return "symbol";
   }

   static bool IsDefined( const String& id )
   {
      return m_symbols.Contains( Symbol( id, 0 ) );
   }

   static Symbol* Find( const String& id )
   {
      symbol_set::const_iterator i = m_symbols.Search( Symbol( id, 0 ) );
      return (i == m_symbols.End()) ? nullptr : *i;
   }

   static void Create( const String& defList );

   static void DestroyAll()
   {
      m_symbols.Destroy();
   }

   static String GlobalVariablesReport();

   struct OutputData
   {
      String  id;
      DVector value = DVector( 0.0, 3 );

      OutputData() = default;
      OutputData( const OutputData& ) = default;
      OutputData( const GlobalVariable& );
   };

   static Array<OutputData> GlobalVariablesData();

   static const symbol_set& Symbols()
   {
      return m_symbols;
   }

private:

   String m_id;

   // Create a temporary object
   Symbol( const String& id, int ) : m_id( id )
   {
   }

   static symbol_set m_symbols;
};

// ----------------------------------------------------------------------------

class Variable : public Symbol
{
public:

   Variable( const String& id, double initialValue = 0 );

   virtual ~Variable()
   {
   }

   bool IsVariable() const override
   {
      return true;
   }

   String Information() const override
   {
      return "variable = " + ToString();
   }

   Pixel& ThreadLocalData( int threadId )
   {
      return m_tls[threadId];
   }

protected:

   GenericVector<Pixel> m_tls; // thread-local storage
};

// ----------------------------------------------------------------------------

class GlobalVariable : public Variable
{
public:

   enum global_op { Add, Mul/*, Or, And*/ };

   GlobalVariable( const String& id, global_op op, double initialValue = 0 )
      : Variable( id, initialValue )
      , m_op( op )
   {
   }

   virtual ~GlobalVariable()
   {
   }

   bool IsGlobalVariable() const override
   {
      return true;
   }

   String Information() const override
   {
      return "global" + Qualifier() + " = " + ToString();
   }

   String Qualifier() const
   {
      String q = '(';
      switch ( m_op )
      {
      case Add: q += '+'; break;
      case Mul: q += '*'; break;
      }
      return q + ')';
   }

   String QualifiedId() const
   {
      return Id() + Qualifier();
   }

   bool IsAdditive() const
   {
      return m_op == Add;
   }

   bool IsMultiplicative() const
   {
      return m_op == Mul;
   }

   Pixel FinalValue() const
   {
      Pixel r( m_tls[0] );
      for ( int i = 1; i < m_tls.Length(); ++i )
         for ( int j = 0; j < r.Length(); ++j )
            switch ( m_op )
            {
            case Add: r[j] += m_tls[i][j]; break;
            case Mul: r[j] *= m_tls[i][j]; break;
            }
      return r;
   }

private:

   global_op m_op;
};

// ----------------------------------------------------------------------------

class Constant : public Symbol
{
public:

   Constant( const String& id, const Pixel& val )
      : Symbol( id )
   {
      SetAs( val );
      CopySamples( val );
   }

   Constant( const String& id, int functionId, const String& imageId, const String& argument = String() )
      : Symbol( id )
      , m_functionId( functionId )
      , m_imageId( imageId )
   {
      if ( !argument.IsEmpty() )
         m_arguments << argument;
   }

   Constant( const String& id, int functionId, const String& imageId, const StringList& arguments )
      : Symbol( id )
      , m_functionId( functionId )
      , m_imageId( imageId )
      , m_arguments( arguments )
   {
   }

   virtual ~Constant()
   {
   }

   bool IsConstant() const override
   {
      return true;
   }

   String ToString() const override;

   String Information() const override
   {
      return "constant = " + ToString();
   }

   bool IsValueFunction() const
   {
      return m_functionId >= 0;
   }

   Expression* FunctionValue( int tokenPos, const ImageWindow& targetWindow ) const;

private:

   int        m_functionId = -1;
   String     m_imageId;
   StringList m_arguments;

   Expression* FunctionValue( const DVector&, int tokenPos ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Symbol_h

// ----------------------------------------------------------------------------
// EOF Symbol.h - Released 2021-05-05T15:38:07Z
