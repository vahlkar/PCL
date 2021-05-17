//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Expression.cpp - Released 2021-05-05T15:38:07Z
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
#include "FlowControl.h"
#include "Function.h"
#include "Operator.h"

#include <pcl/Exception.h>
#include <pcl/Math.h>

// #include <iostream>

namespace pcl
{

// ----------------------------------------------------------------------------

static Operator::operator_set   s_operators;
static Operator::operator_index s_operatorIndex;

static Function::function_set   s_functions;
static Function::function_index s_functionIndex;

// ----------------------------------------------------------------------------

static String s_beingParsed;

// ----------------------------------------------------------------------------

/*
 * Intermediate argument list.
 */
class Arguments : public Data
{
public:

   Arguments( int p ) : Data( XPR_ARGLIST, p )
   {
   }

   Expression* Clone() const override
   {
      return nullptr;
   }

   String ToString() const override
   {
      return String();
   }

   component_list arguments;
};

// ----------------------------------------------------------------------------

/*
 * Temporary list to hold still ambiguous operators.
 */
class OperatorSet : public Expression
{
public:

   OperatorSet( int p ) : Expression( 0, p )
   {
   }

   virtual ~OperatorSet()
   {
      operators.Destroy();
   }

   Expression* Clone() const override
   {
      return nullptr;
   }

   String ToString() const override
   {
      return String();
   }

   IndirectArray<Operator> operators;
};

// ----------------------------------------------------------------------------

/*
 * Temporary search-only operator.
 */
class SearchOperator : public Operator
{
public:

   SearchOperator( const String& t ) : Operator( 0 ), token( t ) {}

   Expression* Clone() const override { return nullptr; }
   Expression* Generate( int p ) const override { return nullptr; }

   String Meta() const override       { return String(); }
   String Id() const override         { return String(); }
   String Token() const override      { return token; }
   int    Precedence() const override { return -1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override
   {
   }

   String token;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Expression::Parse( component_list& x, const String& s )
{
   if ( s_operators.IsEmpty() )
      Operator::InitializeList( s_operators, s_operatorIndex );
   if ( s_functions.IsEmpty() )
      Function::InitializeList( s_functions, s_functionIndex );

   x.Destroy();

   token_set tokens;

   try
   {
      Tokenize( tokens, s );

      s_beingParsed = s;

      for ( token_list& l : tokens )
      {
         Expression* xpr = Parse( l );
         if ( xpr != nullptr )
         {
            if ( dynamic_cast<Arguments*>( xpr ) != nullptr )
               throw ParseError( "Invalid comma expression", s_beingParsed, xpr->TokenPosition() );
            x << xpr;
         }
      }
   }
   catch ( ... )
   {
      x.Destroy();
      for ( token_list& l : tokens )
         l.Destroy();
      tokens.Clear();
      throw;
   }
}

// ----------------------------------------------------------------------------

inline static
char RightBracketForLeftBracket( char ld )
{
   switch ( ld )
   {
   case '(': return ')';
   case '[': return ']';
   case '{': return '}';
   }
   return '\0';
}

inline static
char LeftBracketForRightBracket( char rd )
{
   switch ( rd )
   {
   case ')': return '(';
   case ']': return '[';
   case '}': return '{';
   }
   return '\0';
}

inline static
const char* NameOfBracket( char d )
{
   switch ( d )
   {
   case '(': case ')': return "parenthesis";
   case '[': case ']': return "square bracket";
   case '{': case '}': return "brace";
   }
   return nullptr;
}

// ----------------------------------------------------------------------------

/*
 * Parser First Pass: Syntactical analysis and expression tree generation.
 */
Expression* Expression::Parse( token_list& tokens )
{
   if ( tokens.IsEmpty() )
      return nullptr;

   /*
   for ( size_type i = 0; ; )
   {
      std::cout << tokens[i]->ToString();
      if ( ++i == tokens.Length() )
         break;
      std::cout << ',';
   }
   std::cout << '\n';
   */

   component_list components;

   int argumentCount = 0;

   while ( !tokens.IsEmpty() )
   {
      Token* t = *tokens;

      int pos = t->Position();

      if ( t->IsSymbol() )
      {
         Symbol* s = Symbol::Find( t->ToString() );
         if ( s == nullptr )
            components << new ImageReference( t->ToString(), pos );
         else if ( s->IsVariable() )
            components << new VariableReference( static_cast<Variable*>( s ), pos );
         else if ( s->IsConstant() )
            components << new ConstantReference( static_cast<Constant*>( s ), pos );
         else
            throw ParseError( "Expression::Parse(): Internal parser error: Invalid expression component type", s_beingParsed, pos );

         tokens.Destroy( tokens.Begin() );
      }
      else if ( t->IsMetaSymbol() )
      {
         components << new MetaSymbol( t->ToString(), pos );
         tokens.Destroy( tokens.Begin() );
      }
      else if ( t->IsNumeric() )
      {
         components << new Sample( t->AsNumeric(), pos );
         tokens.Destroy( tokens.Begin() );
      }
      else if ( t->IsSeparator() )
      {
         char rb = RightBracketForLeftBracket( t->AsSeparator() );
         if ( rb != '\0' )
         {
            token_list::iterator j = tokens.Begin();
            for ( int n = 1; ; )
            {
               if ( ++j == tokens.End() )
                  throw ParseError( String( "Mismatched left " ) + NameOfBracket( rb ), s_beingParsed, pos );

               if ( (*j)->IsSeparator() )
               {
                  if ( (*j)->AsSeparator() == rb )
                  {
                     if ( --n == 0 )
                        break;
                  }
                  else if ( (*j)->AsSeparator() == t->AsSeparator() )
                     ++n;
               }
            }

            if ( rb == ']' )
            {
               if ( components.IsEmpty() )
                  throw ParseError( "Invalid channel index specification", s_beingParsed, pos );

               ObjectReference* r = dynamic_cast<ObjectReference*>( components.Last() );
               if ( r == nullptr )
                  throw ParseError( "A channel index specification must follow an image or variable identifier", s_beingParsed, pos );

               if ( r->ChannelIndex() >= 0 )
                  throw ParseError( "A channel index specification has already been defined here. Missing object identifier?", s_beingParsed, pos );
            }

            token_list subList( tokens.Begin()+1, j );

            tokens.Delete( tokens.Begin() );
            tokens.Delete( j );
            tokens.Remove( tokens.Begin(), j+1 );

            Expression* x;
            try
            {
               x = Parse( subList );
            }
            catch ( ... )
            {
               subList.Destroy();
               throw;
            }

            if ( rb == ']' )
            {
               if ( x == nullptr )
                  throw ParseError( "Missing channel index", s_beingParsed, pos );

               Sample* s = dynamic_cast<Sample*>( x );
               if ( s == nullptr )
                  throw ParseError( "Channel indexes must be immediate positive integrals", s_beingParsed, pos );

               if ( Frac( s->Value() ) != 0 || s->Value() < 0 )
                  throw ParseError( "Channel indexes must be positive integers", s_beingParsed, pos );

               dynamic_cast<ObjectReference*>( components.Last() )->SetChannelIndex( int( s->Value() ) );

               delete x, x = nullptr;
            }
            else if ( rb == ')' )
            {
               ObjectReference* r = (argumentCount >= int( components.Length() )) ? 0 : dynamic_cast<ObjectReference*>( components.Last() );
               if ( r != nullptr )
               {
                  if ( r->ChannelIndex() >= 0 )
                     throw ParseError( "Invalid subexpression", s_beingParsed, pos );

                  String funcId = r->Id();

                  Function::function_index::const_iterator fx = s_functionIndex.Search( Function::IndexNode( funcId ) );
                  if ( fx == s_functionIndex.End() )
                     throw ParseError( "Unknown function '" + funcId + "'", s_beingParsed, r->TokenPosition() );

                  const Function* f = fx->function;
                  int n = 0;
                  Arguments* a = nullptr;

                  if ( x != nullptr )
                  {
                     a = dynamic_cast<Arguments*>( x );
                     n = (a != nullptr) ? int( a->arguments.Length() ) : 1;
                  }

                  if ( n < f->RequiredArguments() )
                     throw ParseError( "The " + fx->token + " function requires at least " + String( f->RequiredArguments() ) + " argument(s)", s_beingParsed, pos );

                  if ( n > f->MaximumArguments() )
                  {
                     if ( f->MaximumArguments() == 0 )
                        throw ParseError( "The " + fx->token + " function does not take arguments", s_beingParsed, pos );
                     throw ParseError( "The " + fx->token + " function does not take more than " + String( f->MaximumArguments() ) + " argument(s)", s_beingParsed, pos );
                  }

                  if ( x != nullptr )
                  {
                     String info;
                     Expression* arg = nullptr;
                     if ( !f->ValidateArguments( info, arg, (a != nullptr) ? a->arguments.Begin() : &x,
                                                            (a != nullptr) ? a->arguments.End()   : (&x)+1 ) )
                        throw ParseError( info, s_beingParsed, (arg != nullptr) ? arg->TokenPosition() : x->TokenPosition() );
                  }

                  Function* fn = reinterpret_cast<Function*>( f->Generate( r->TokenPosition() ) );
                  if ( x != nullptr )
                     if ( a != nullptr )
                     {
                        for ( Expression* arg : a->arguments )
                           fn->AddArgument( arg );
                        delete x, x = nullptr;
                     }
                     else
                        fn->AddArgument( x );

                  components.Delete( components.ReverseBegin() );
                  *components.ReverseBegin() = fn;
               }
               else
               {
                  if ( x == nullptr )
                     throw ParseError( "Empty subexpression", s_beingParsed, pos );

                  if ( x->IsArgumentList() )
                     throw ParseError( "Unexpected argument list. Missing function identifier?", s_beingParsed, pos );

                  components << x;
               }
            }
            else //if ( rb == '}' )
            {
               if ( x == nullptr )
                  throw ParseError( "Empty subexpression", s_beingParsed, pos );

               if ( x->IsArgumentList() )
                  throw ParseError( "Invalid comma expression", s_beingParsed, pos );

               components << x;
            }
         }
         else
         {
            char lb = LeftBracketForRightBracket( t->AsSeparator() );
            if ( lb != '\0' )
               throw ParseError( String( "Mismatched right " ) + NameOfBracket( lb ), s_beingParsed, pos );

            if ( t->AsSeparator() == ',' )
            {
               if ( argumentCount >= int( components.Length() ) )
                  throw ParseError( "Missing comma-delimited subexpression", s_beingParsed, pos );

               component_list subList( components.At( argumentCount ), components.End() );
               components.Replace( components.At( argumentCount ), components.End(), Parse( subList ) );

               tokens.Destroy( tokens.Begin() );

               ++argumentCount;
            }
            else
            {
               String opToken = t->AsSeparator();
               token_list::iterator j = tokens.Begin();
               for ( ; ++j != tokens.End(); )
                  if ( (*j)->IsSeparator() )
                     opToken += (*j)->AsSeparator();
                  else
                     break;

               Operator::operator_index::const_iterator ox = s_operatorIndex.Search( Operator::IndexNode( opToken ) );
               if ( ox == s_operatorIndex.End() )
               {
                  while ( opToken.Length() > 1 )
                  {
                     opToken.DeleteRight( opToken.Length()-1 );
                     --j;
                     ox = s_operatorIndex.Search( Operator::IndexNode( opToken ) );
                     if ( ox != s_operatorIndex.End() )
                        break;
                  }

                  if ( ox == s_operatorIndex.End() )
                     throw ParseError( "Unknown operator '" + opToken + "'", s_beingParsed, pos );
               }

               Operator::operator_index::const_iterator o1 = ox;
               while ( o1 != s_operatorIndex.Begin() )
                  if ( (--o1)->token != opToken )
                  {
                     ++o1;
                     break;
                  }

               Operator::operator_index::const_iterator o2 = ox;
               while ( ++o2 != s_operatorIndex.End() )
                  if ( o2->token != opToken )
                     break;

               if ( Distance( o1, o2 ) > 1 )
               {
                  OperatorSet* s = new OperatorSet( t->Position() );
                  for ( ;; )
                  {
                     s->operators << reinterpret_cast<Operator*>( o1->op->Generate( t->Position() ) );
                     if ( ++o1 == o2 )
                        break;
                  }
                  components << s;
               }
               else
                  components << ox->op->Generate( t->Position() );

               tokens.Destroy( tokens.Begin(), j );
            }
         }
      }
   }

   if ( argumentCount != 0 )
   {
      if ( argumentCount >= int( components.Length() ) )
         throw ParseError( "Missing comma-delimited subexpression", s_beingParsed, components.Last()->TokenPosition() );

      component_list subList( components.At( argumentCount ), components.End() );
      components.Replace( components.At( argumentCount ), components.End(), Parse( subList ) );

      Arguments* a = new Arguments( (*components)->TokenPosition() );
      a->arguments = components;
      return a;
   }

   return Parse( components );
}

// ----------------------------------------------------------------------------

/*
 * Parser Second Pass: Operator disambiguation and precedence assignment.
 */
Expression* Expression::Parse( component_list& components )
{
   if ( components.IsEmpty() )
      return nullptr;

   for ( component_list::iterator i = components.Begin(); i != components.End(); ++i )
   {
      OperatorSet* s = dynamic_cast<OperatorSet*>( *i );
      if ( s != nullptr )
      {
         bool haveLeftOperand = false;
         bool haveRightOperand = false;

         if ( i != components.Begin() )
         {
            Operator* o = dynamic_cast<Operator*>( *--i );
            ++i;
            if ( o == nullptr || !o->IsRightAssociative() || o->IsParsed() )
               haveLeftOperand = true;
         }

         if ( ++i != components.End() )
         {
            Operator* o = dynamic_cast<Operator*>( *i );
            if ( o == nullptr || !o->IsLeftAssociative() || o->IsParsed() )
               haveRightOperand = true;
         }
         --i;

         for ( int j = 0;; )
         {
            if ( s->operators[j]->IsLeftAssociative() == haveLeftOperand )
               if ( s->operators[j]->IsRightAssociative() == haveRightOperand )
               {
                  *i = s->operators[j];
                  s->operators[j] = nullptr;
                  delete s;
                  break;
               }

            if ( ++j == int( s->operators.Length() ) )
               throw ParseError( "Unresolved operator", s_beingParsed, s->TokenPosition() );
         }
      }
   }

   for ( int p = 0, n = 0; p <= MAX_PRECEDENCE && (p == 0 || n != 0); ++p )
      for ( component_list::iterator i = components.Begin(); i != components.End(); ++i )
      {
         Operator* o = dynamic_cast<Operator*>( *i );
         if ( o != nullptr )
            if ( !o->IsParsed() )
               if ( o->Precedence() == p )
               {
                  component_list::iterator j = i; ++j;

                  if ( o->IsLeftAssociative() )
                  {
                     if ( i == components.Begin() )
                        throw ParseError( "Expected left operand", s_beingParsed, o->TokenPosition() );

                     --i;

                     if ( o->IsAssignment() )
                     {
                        if ( !(*i)->IsLValue() )
                           throw ParseError( "Expected lvalue operand", s_beingParsed, (*i)->TokenPosition() );

                        if ( (*i)->IsVariableReference() )
                        {
                           const Variable& v = static_cast<VariableReference*>( *i )->Reference();
                           if ( v.IsGlobalVariable() )
                           {
                              const GlobalVariable& g = static_cast<const GlobalVariable&>( v );
                              if ( g.IsAdditive() )
                              {
                                 if ( !o->IsAdditive() )
                                    throw ParseError( "Global variable '" + g.QualifiedId() + "' requires an additive assignment operator", s_beingParsed, o->TokenPosition() );
                              }
                              else if ( g.IsMultiplicative() )
                              {
                                 if ( !o->IsMultiplicative() )
                                    throw ParseError( "Global variable '" + g.QualifiedId() + "' requires a multiplicative assignment operator", s_beingParsed, o->TokenPosition() );
                              }
                           }
                        }

                        (*i)->SetLValueRole();
                     }

                     o->AddArgument( *i );
                  }

                  if ( o->IsRightAssociative() )
                  {
                     if ( j == components.End() )
                        throw ParseError( "Expected right operand", s_beingParsed, o->TokenPosition() );

                     o->AddArgument( *j );
                     ++j;
                  }

                  components.Replace( i, j, o );
               }
               else
                  ++n;
      }

   if ( components.Length() != 1 )
      throw ParseError( "Disassociated subexpressions", s_beingParsed, components[1]->TokenPosition() );

   Expression* x = *components;

   components.Clear();

   if ( x->IsFunctional() )
      static_cast<const Functional*>( x )->CheckParsedGlobalVariables( s_beingParsed );

   return x;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Expression.cpp - Released 2021-05-05T15:38:07Z
