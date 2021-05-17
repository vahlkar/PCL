//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Expression.h - Released 2021-05-05T15:38:07Z
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

#ifndef __Expression_h
#define __Expression_h

#include "Pixel.h"
#include "Symbol.h"
#include "Token.h"

#include <pcl/Point.h>
#include <pcl/Relational.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS ImageVariant;
class PCL_CLASS PixelInterpolation;

// ----------------------------------------------------------------------------

#define  XPR_OPERATOR      0x00000001
#define  XPR_FUNCTION      0x00000002
#define  XPR_FUNCTIONAL    0x0000000f

#define  XPR_IMAGEREF      0x00000010
#define  XPR_VARREF        0x00000020
#define  XPR_CONSTREF      0x00000040
#define  XPR_PIXEL         0x00000100
#define  XPR_SAMPLE        0x00000200
#define  XPR_ARGLIST       0x00000800
#define  XPR_DATA          0x00000ff0

#define  XPR_BRANCH        0x00100000
#define  XPR_POINTER       0x00200000

// ----------------------------------------------------------------------------

/*
 * Representation of a generic PixelMath expression.
 */
class Expression
{
public:

   typedef IndirectArray<Expression>   component_list;

   Expression( unsigned t, int p )
      : m_type( t )
      , m_pos( p )
   {
   }

   Expression( const Expression& ) = default;

   virtual ~Expression()
   {
   }

   int TokenPosition() const
   {
      return m_pos;
   }

   unsigned Type() const
   {
      return m_type;
   }

   bool IsFunctional() const
   {
      return (m_type & XPR_FUNCTIONAL) != 0;
   }

   bool IsOperator() const
   {
      return (m_type & XPR_OPERATOR) != 0;
   }

   bool IsFunction() const
   {
      return (m_type & XPR_FUNCTION) != 0;
   }

   bool IsData() const
   {
      return (m_type & XPR_DATA) != 0;
   }

   bool IsImageReference() const
   {
      return (m_type & XPR_IMAGEREF) != 0;
   }

   bool IsVariableReference() const
   {
      return (m_type & XPR_VARREF) != 0;
   }

   bool IsConstantReference() const
   {
      return (m_type & XPR_CONSTREF) != 0;
   }

   bool IsPixel() const
   {
      return (m_type & XPR_PIXEL) != 0;
   }

   bool IsSample() const
   {
      return (m_type & XPR_SAMPLE) != 0;
   }

   bool IsArgumentList() const
   {
      return (m_type & XPR_ARGLIST) != 0;
   }

   bool IsBranch() const
   {
      return (m_type & XPR_BRANCH) != 0;
   }

   bool IsPointer() const
   {
      return (m_type & XPR_POINTER) != 0;
   }

   bool HasLValueRole() const
   {
      return m_isLvalue;
   }

   void SetLValueRole( bool set = true )
   {
      m_isLvalue = set;
   }

   virtual bool IsLValue() const
   {
      return false;
   }

   virtual Expression* Clone() const = 0;

   virtual String ToString() const = 0;

   static void Parse( component_list&, const String& );

private:

   unsigned m_type;             // component type
   int      m_pos;              // token position
   bool     m_isLvalue = false; // lvalue role set by the parser

   static Expression* Parse( token_list& );     // parser stage 1
   static Expression* Parse( component_list& ); // parser stage 2
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Expression_h

// ----------------------------------------------------------------------------
// EOF Expression.h - Released 2021-05-05T15:38:07Z
