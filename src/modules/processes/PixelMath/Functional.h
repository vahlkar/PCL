//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Functional.h - Released 2021-05-05T15:38:07Z
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

#ifndef __Functional_h
#define __Functional_h

#include "Expression.h"

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Abstract base class of all PixelMath functional expression components.
 */
class Functional : public Expression
{
public:

   typedef component_list  argument_list;

   Functional( unsigned t, int p )
      : Expression( t, p )
   {
   }

   Functional( const Functional& x )
      : Expression( x )
   {
      for ( const Expression* arg : x.arguments )
         arguments << arg->Clone();
   }

   virtual ~Functional()
   {
      DestroyArguments();
   }

   virtual Expression* Clone() const = 0;
   virtual Expression* Generate( int ) const = 0;

   virtual String Meta() const = 0;
   virtual String Id() const = 0;
   virtual String Token() const = 0;

   virtual String ToString() const = 0;

   virtual void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const = 0;

   virtual bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
   {
      return false;
   }

   // Invariant function call
   virtual void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const
   {
   }

   virtual int NumberOfArguments() const
   {
      return int( arguments.Length() );
   }

   virtual bool CanOptimize() const
   {
      return false;
   }

   virtual component_list Optimized() const
   {
      return component_list();
   }

   bool IsParsed() const
   {
      return !arguments.IsEmpty();
   }

   const Expression& operator []( int i ) const
   {
      return *arguments[i];
   }

   void AddArgument( Expression* x )
   {
      arguments.Add( x );
   }

   void DestroyArguments()
   {
      arguments.Destroy();
   }

   bool operator ==( const Functional& x ) const
   {
      return Token() == x.Token();
   }

   bool operator <( const Functional& x ) const
   {
      return Token() < x.Token();
   }

   String PostOrder() const;

   void PostOrder( component_list&, bool optimize = true ) const;

   void CheckParsedGlobalVariables( const String& beingParsed ) const;

protected:

   argument_list arguments;

private:

   void PostOrderRecursive( component_list&, bool optimize ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Functional_h

// ----------------------------------------------------------------------------
// EOF Functional.h - Released 2021-05-05T15:38:07Z
