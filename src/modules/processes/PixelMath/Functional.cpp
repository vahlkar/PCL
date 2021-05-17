//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Functional.cpp - Released 2021-05-05T15:38:07Z
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
#include "Functional.h"

namespace pcl
{

// ----------------------------------------------------------------------------

String Functional::PostOrder() const
{
   String s;
   if ( !arguments.IsEmpty() )
   {
      for ( argument_list::const_iterator i = arguments.Begin(); ; )
      {
         Functional* f = dynamic_cast<Functional*>( *i );
         if ( f != nullptr )
            s << f->PostOrder();
         else
            s << (*i)->ToString();
         if ( ++i == arguments.End() )
            break;
         s << ',';
      }

      s << ',';
   }
   return s << Token() << '(' + String( arguments.Length() ) << ')';
}

// ----------------------------------------------------------------------------

void Functional::PostOrder( component_list& components, bool optimize ) const
{
   components.Destroy();
   PostOrderRecursive( components, optimize );
}

void Functional::PostOrderRecursive( component_list& components, bool optimize ) const
{
   if ( optimize && CanOptimize() )
   {
      component_list optimized = Optimized();
      for ( const Expression* xpr : optimized )
      {
         const Functional* f = dynamic_cast<const Functional*>( xpr );
         if ( f != nullptr )
            f->PostOrderRecursive( components, optimize );
         else
            components << xpr->Clone();
      }
      optimized.Destroy();
   }
   else
   {
      for ( const Expression* arg : arguments )
      {
         const Functional* f = dynamic_cast<const Functional*>( arg );
         if ( f != nullptr )
            f->PostOrderRecursive( components, optimize );
         else
            components << arg->Clone();
      }
      const Functional* f = dynamic_cast<const Functional*>( this );
      if ( f != nullptr )
         components << f->Clone();
      else
         components << Clone();
   }
}

// ----------------------------------------------------------------------------

void Functional::CheckParsedGlobalVariables( const String& beingParsed ) const
{
   for ( const Expression* arg : arguments )
   {
      const Functional* f = dynamic_cast<const Functional*>( arg );
      if ( f != nullptr )
         f->CheckParsedGlobalVariables( beingParsed );
      else if ( arg->IsVariableReference() )
      {
         const Variable& v = static_cast<const VariableReference*>( arg )->Reference();
         if ( v.IsGlobalVariable() )
            if ( !arg->HasLValueRole() )
               throw ParseError( "Illegal rvalue role of global variable '"
                                 + static_cast<const GlobalVariable&>( v ).QualifiedId()
                                 + "'"
                                 , beingParsed
                                 , arg->TokenPosition() );
      }
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Functional.cpp - Released 2021-05-05T15:38:07Z
