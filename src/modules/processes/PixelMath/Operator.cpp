//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// Operator.cpp - Released 2024-03-20T10:42:12Z
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

#include "Data.h"
#include "FlowControl.h"
#include "Operator.h"

#include <pcl/Math.h>

#ifdef _MSC_VER
# ifdef min
#  undef min
# endif
# ifdef max
#  undef max
# endif
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

String Operator::ToString() const
{
   String s = '(';
   int i = 0;
   if ( IsLeftAssociative() )
      s << arguments[0]->ToString(), i = 1;
   s << Token();
   if ( IsRightAssociative() )
      s << arguments[i]->ToString();
   s << ')';
   return s;
}

// ----------------------------------------------------------------------------

void Operator::InitializeList( operator_list& operators, operator_index& index )
{
   operators.Destroy();

   operators << new InvOperator
             << new ChsOperator
             << new PlusOperator
             << new LogicalNotOperator
             << new PowOperator
             << new MulOperator
             << new DivOperator
             << new ModOperator
             << new AddOperator
             << new SubOperator
             << new DifOperator
             << new LessThanOperator
             << new LessThanOrEqualOperator
             << new GreaterThanOperator
             << new GreaterThanOrEqualOperator
             << new EqualToOperator
             << new NotEqualToOperator
             << new AndOperator
             << new NandOperator
             << new XorOperator
             << new XnorOperator
             << new OrOperator
             << new NorOperator
             << new LogicalAndOperator
             << new LogicalOrOperator
             << new AssignmentOperator
             << new AssignmentAddOperator
             << new AssignmentSubOperator
             << new AssignmentMulOperator
             << new AssignmentDivOperator
             << new AssignmentPowOperator
             << new AssignmentModOperator
             << new AssignmentDifOperator
             << new AssignmentAndOperator
             << new AssignmentNandOperator
             << new AssignmentOrOperator
             << new AssignmentNorOperator
             << new AssignmentXorOperator
             << new AssignmentXnorOperator
             << new AssignmentLogicalAndOperator
             << new AssignmentLogicalOrOperator;

   index.Clear();
   for ( const Operator* op : operators )
      index << IndexNode( op->Token(), op );
}

// ----------------------------------------------------------------------------

void InvOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = 1 - (*i)[c];
}

bool InvOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void InvOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   if ( (*i)->IsSample() )
   {
      double v = static_cast<const Sample*>( *i )->Value();
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = 1 - v;
   }
   else
   {
      const Pixel& p = static_cast<const PixelData*>( *i )->PixelValue();
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = 1 - p[c];
   }
}

// ----------------------------------------------------------------------------

void ChsOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = -(*i)[c];
}

bool ChsOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void ChsOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   if ( (*i)->IsSample() )
   {
      double v = static_cast<const Sample*>( *i )->Value();
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = -v;
   }
   else
   {
      const Pixel& p = static_cast<const PixelData*>( *i )->PixelValue();
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = -p[c];
   }
}

// ----------------------------------------------------------------------------

void PlusOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (*i)[c];
}

bool PlusOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void PlusOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   if ( (*i)->IsSample() )
   {
      double v = static_cast<const Sample*>( *i )->Value();
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = v;
   }
   else
   {
      const Pixel& p = static_cast<const PixelData*>( *i )->PixelValue();
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = p[c];
   }
}

// ----------------------------------------------------------------------------

void LogicalNotOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ((*i)[c] == 0) ? 1 : 0;
}

bool LogicalNotOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void LogicalNotOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   if ( (*i)->IsSample() )
   {
      int v = (static_cast<const Sample*>( *i )->Value() == 0) ? 1 : 0;
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = v;
   }
   else
   {
      const Pixel& p = static_cast<const PixelData*>( *i )->PixelValue();
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = (p[c] == 0) ? 1 : 0;
   }
}

// ----------------------------------------------------------------------------

void PowOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = pcl::Pow( (*i)[c], (*j)[c] );
}

bool PowOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void PowOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = pcl::Pow( a[c], b[c] );
}

// ----------------------------------------------------------------------------

void MulOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (*i)[c] * (*j)[c];
}

bool MulOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void MulOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = a[c] * b[c];
}

// ----------------------------------------------------------------------------

void DivOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (Abs( (*j)[c] ) > std::numeric_limits<double>::epsilon()) ? ((*i)[c] / (*j)[c]) : std::numeric_limits<double>::max();
}

bool DivOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void DivOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (Abs( b[c] ) > std::numeric_limits<double>::epsilon()) ? a[c]/b[c] : std::numeric_limits<double>::max();
}

// ----------------------------------------------------------------------------

void ModOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (Abs( (*j)[c] ) > std::numeric_limits<double>::epsilon()) ? Mod( (*i)[c], (*j)[c] ) : 0.0;
}

bool ModOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void ModOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (Abs( b[c] ) > std::numeric_limits<double>::epsilon()) ? Mod( a[c], b[c] ) : 0.0;
}

// ----------------------------------------------------------------------------

void AddOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (*i)[c] + (*j)[c];
}

bool AddOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void AddOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = a[c] + b[c];
}

// ----------------------------------------------------------------------------

void SubOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (*i)[c] - (*j)[c];
}

bool SubOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void SubOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = a[c] - b[c];
}

// ----------------------------------------------------------------------------

void DifOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Abs( (*i)[c] - (*j)[c] );
}

bool DifOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void DifOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Abs( a[c] - b[c] );
}

// ----------------------------------------------------------------------------

void LessThanOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ((*i)[c] < (*j)[c]) ? 1 : 0;
}

bool LessThanOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void LessThanOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (a[c] < b[c]) ? 1 : 0;
}

// ----------------------------------------------------------------------------

void LessThanOrEqualOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ((*i)[c] <= (*j)[c]) ? 1 : 0;
}

bool LessThanOrEqualOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void LessThanOrEqualOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (a[c] <= b[c]) ? 1 : 0;
}

// ----------------------------------------------------------------------------

void GreaterThanOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ((*i)[c] > (*j)[c]) ? 1 : 0;
}

bool GreaterThanOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void GreaterThanOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (a[c] > b[c]) ? 1 : 0;
}

// ----------------------------------------------------------------------------

void GreaterThanOrEqualOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ((*i)[c] >= (*j)[c]) ? 1 : 0;
}

bool GreaterThanOrEqualOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void GreaterThanOrEqualOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (a[c] >= b[c]) ? 1 : 0;
}

// ----------------------------------------------------------------------------

void EqualToOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ((*i)[c] == (*j)[c]) ? 1 : 0;
}

bool EqualToOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void EqualToOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (a[c] == b[c]) ? 1 : 0;
}

// ----------------------------------------------------------------------------

void NotEqualToOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ((*i)[c] != (*j)[c]) ? 1 : 0;
}

bool NotEqualToOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void NotEqualToOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (a[c] != b[c]) ? 1 : 0;
}

// ----------------------------------------------------------------------------

void AndOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = TruncInt( (*i)[c] ) & TruncInt( (*j)[c] );
}

bool AndOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void AndOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = TruncInt( a[c] ) & TruncInt( b[c] );
}

// ----------------------------------------------------------------------------

void NandOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ~(TruncInt( (*i)[c] ) & TruncInt( (*j)[c] ));
}

bool NandOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void NandOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ~(TruncInt( a[c] ) & TruncInt( b[c] ));
}

// ----------------------------------------------------------------------------

void XorOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = TruncInt( (*i)[c] ) ^ TruncInt( (*j)[c] );
}

bool XorOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void XorOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = TruncInt( a[c] ) ^ TruncInt( b[c] );
}

// ----------------------------------------------------------------------------

void XnorOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ~(TruncInt( (*i)[c] ) ^ TruncInt( (*j)[c] ));
}

bool XnorOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void XnorOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ~(TruncInt( a[c] ) ^ TruncInt( b[c] ));
}

// ----------------------------------------------------------------------------

void OrOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = TruncInt( (*i)[c] ) | TruncInt( (*j)[c] );
}

bool OrOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void OrOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = TruncInt( a[c] ) | TruncInt( b[c] );
}

// ----------------------------------------------------------------------------

void NorOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ~(TruncInt( (*i)[c] ) | TruncInt( (*j)[c] ));
}

bool NorOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void NorOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ~(TruncInt( a[c] ) | TruncInt( b[c] ));
}

// ----------------------------------------------------------------------------

void LogicalAndOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ((*i)[c] != 0 && (*j)[c] != 0) ? 1 : 0;
}

bool LogicalAndOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void LogicalAndOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (a[c] != 0 && b[c] != 0) ? 1 : 0;
}

ExpressionList LogicalAndOperator::Optimized() const
{
   unsigned a = Pointer::NextId();
   return ExpressionList()
            << arguments[0]->Clone()
            << new BranchOnZero( a, TokenPosition() )
            << arguments[1]->Clone()
            << new BranchOnZero( a, TokenPosition() )
            << new Sample( 1, TokenPosition() )
            << new Pointer( a );
}

// ----------------------------------------------------------------------------

void LogicalOrOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ((*i)[c] != 0 || (*j)[c] != 0) ? 1 : 0;
}

bool LogicalOrOperator::IsInvariant( ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void LogicalOrOperator::operator()( Pixel& r, ExpressionList::const_iterator i, ExpressionList::const_iterator j ) const
{
   j = i; ++j;

   Pixel a, b;
   a.SetAs( r );
   b.SetAs( r );

   if ( (*i)->IsSample() )
      a.CopySamples( static_cast<const Sample*>( *i )->Value() );
   else
      a.CopySamples( static_cast<const PixelData*>( *i )->PixelValue() );

   if ( (*j)->IsSample() )
      b.CopySamples( static_cast<const Sample*>( *j )->Value() );
   else
      b.CopySamples( static_cast<const PixelData*>( *j )->PixelValue() );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (a[c] != 0 || b[c] != 0) ? 1 : 0;
}

ExpressionList LogicalOrOperator::Optimized() const
{
   unsigned a = Pointer::NextId();
   return ExpressionList()
            << arguments[0]->Clone()
            << new BranchOnNonZero( a, TokenPosition() )
            << arguments[1]->Clone()
            << new BranchOnNonZero( a, TokenPosition() )
            << new Sample( 0, TokenPosition() )
            << new Pointer( a );
}

// ----------------------------------------------------------------------------

#define REF_ASSIGN()                                  \
   Pixel* p = static_cast<Pixel*>( i->Reference() );  \
   if ( p != 0 )                                      \
   {                                                  \
      int c = i->ReferenceChannel();                  \
      if ( c < 0 )                                    \
         p->AssignSamples( r );                       \
      else                                            \
         (*p)[c] = r[c];                              \
   }

// ----------------------------------------------------------------------------

void AssignmentOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;

   if ( i->HasReference() )
   {
      Pixel* p = static_cast<Pixel*>( i->Reference() );
      int c = p->ReferenceChannel();
      if ( c < 0 )
         for ( int c = 0; c < p->Length(); ++c )
            r[c] = (*p)[c] = (*j)[c];
      else
         r.SetSamples( (*p)[c] = (*j)[c] );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = (*j)[c];
   }
}

// ----------------------------------------------------------------------------

void AssignmentAddOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;

   if ( i->HasReference() )
   {
      Pixel* p = static_cast<Pixel*>( i->Reference() );
      int c = i->ReferenceChannel();
      if ( c < 0 )
         for ( int c = 0; c < p->Length(); ++c )
            r[c] = (*p)[c] += (*j)[c];
      else
         r.SetSamples( (*p)[c] += (*j)[c] );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = (*i)[c] + (*j)[c];
   }
}

// ----------------------------------------------------------------------------

void AssignmentSubOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;

   if ( i->HasReference() )
   {
      Pixel* p = static_cast<Pixel*>( i->Reference() );
      int c = i->ReferenceChannel();
      if ( c < 0 )
         for ( int c = 0; c < p->Length(); ++c )
            r[c] = (*p)[c] -= (*j)[c];
      else
         r.SetSamples( (*p)[c] -= (*j)[c] );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = (*i)[c] - (*j)[c];
   }
}

// ----------------------------------------------------------------------------

void AssignmentMulOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;

   if ( i->HasReference() )
   {
      Pixel* p = static_cast<Pixel*>( i->Reference() );
      int c = i->ReferenceChannel();
      if ( c < 0 )
         for ( int c = 0; c < p->Length(); ++c )
            r[c] = (*p)[c] *= (*j)[c];
      else
         r.SetSamples( (*p)[c] *= (*j)[c] );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = (*i)[c] * (*j)[c];
   }
}

// ----------------------------------------------------------------------------

void AssignmentDivOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;

   if ( i->HasReference() )
   {
      Pixel* p = static_cast<Pixel*>( i->Reference() );
      int c = i->ReferenceChannel();
      if ( c < 0 )
         for ( int c = 0; c < p->Length(); ++c )
            if ( Abs( (*j)[c] ) > std::numeric_limits<double>::epsilon() )
               r[c] = (*p)[c] /= (*j)[c];
            else
               r[c] = (*p)[c] = std::numeric_limits<double>::max();
         else
            if ( Abs( (*j)[c] ) > std::numeric_limits<double>::epsilon() )
               r.SetSamples( (*p)[c] /= (*j)[c] );
            else
               r.SetSamples( std::numeric_limits<double>::max() );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         if ( Abs( (*j)[c] ) > std::numeric_limits<double>::epsilon() )
            r[c] = (*i)[c] / (*j)[c];
         else
            r[c] = std::numeric_limits<double>::max();
   }
}

// ----------------------------------------------------------------------------

void AssignmentPowOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;

   if ( i->HasReference() )
   {
      Pixel* p = static_cast<Pixel*>( i->Reference() );
      int c = i->ReferenceChannel();
      if ( c < 0 )
         for ( int c = 0; c < p->Length(); ++c )
            r[c] = (*p)[c] = Pow( (*p)[c], (*j)[c] );
      else
         r.SetSamples( (*p)[c] = Pow( (*p)[c], (*j)[c] ) );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Pow( (*i)[c], (*j)[c] );
   }
}

// ----------------------------------------------------------------------------

void AssignmentModOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;

   if ( i->HasReference() )
   {
      Pixel* p = static_cast<Pixel*>( i->Reference() );
      int c = i->ReferenceChannel();
      if ( c < 0 )
         for ( int c = 0; c < p->Length(); ++c )
            if ( Abs( (*j)[c] ) > std::numeric_limits<double>::epsilon() )
               r[c] = (*p)[c] = Mod( (*p)[c], (*j)[c] );
            else
               r[c] = (*p)[c] = 0;
      else
         if ( Abs( (*j)[c] ) > std::numeric_limits<double>::epsilon() )
            r.SetSamples( (*p)[c] = Mod( (*p)[c], (*j)[c] ) );
         else
            r.SetSamples( (*p)[c] = 0 );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         if ( Abs( (*j)[c] ) > std::numeric_limits<double>::epsilon() )
            r[c] = Mod( (*i)[c], (*j)[c] );
         else
            r[c] = 0;
   }
}

// ----------------------------------------------------------------------------

void AssignmentDifOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;

   if ( i->HasReference() )
   {
      Pixel* p = static_cast<Pixel*>( i->Reference() );
      int c = i->ReferenceChannel();
      if ( c < 0 )
         for ( int c = 0; c < p->Length(); ++c )
            r[c] = (*p)[c] = Abs( (*p)[c] - (*j)[c] );
      else
         r.SetSamples( (*p)[c] = Abs( (*p)[c] - (*j)[c] ) );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Abs( (*i)[c] - (*j)[c] );
   }
}

// ----------------------------------------------------------------------------

void AssignmentAndOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = TruncInt( (*i)[c] ) & TruncInt( (*j)[c] );
   REF_ASSIGN()
}

// ----------------------------------------------------------------------------

void AssignmentNandOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ~(TruncInt( (*i)[c] ) & TruncInt( (*j)[c] ));
   REF_ASSIGN()
}

// ----------------------------------------------------------------------------

void AssignmentOrOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = TruncInt( (*i)[c] ) | TruncInt( (*j)[c] );
   REF_ASSIGN()
}

// ----------------------------------------------------------------------------

void AssignmentNorOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ~(TruncInt( (*i)[c] ) | TruncInt( (*j)[c] ));
   REF_ASSIGN()
}

// ----------------------------------------------------------------------------

void AssignmentXorOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = TruncInt( (*i)[c] ) ^ TruncInt( (*j)[c] );
   REF_ASSIGN()
}

// ----------------------------------------------------------------------------

void AssignmentXnorOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ~(TruncInt( (*i)[c] ) ^ TruncInt( (*j)[c] ));
   REF_ASSIGN()
}

// ----------------------------------------------------------------------------

void AssignmentLogicalAndOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;

   if ( i->HasReference() )
   {
      Pixel* p = static_cast<Pixel*>( i->Reference() );
      int c = i->ReferenceChannel();
      if ( c < 0 )
         for ( int c = 0; c < p->Length(); ++c )
            r[c] = (*p)[c] = ((*p)[c] != 0 && (*j)[c] != 0) ? 1 : 0;
      else
         r.SetSamples( (*p)[c] = ((*p)[c] != 0 && (*j)[c] != 0) ? 1 : 0 );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = ((*i)[c] != 0 && (*j)[c] != 0) ? 1 : 0;
   }
}

// ----------------------------------------------------------------------------

void AssignmentLogicalOrOperator::operator()( Pixel& r, PixelList::const_iterator i, PixelList::const_iterator j ) const
{
   j = i; ++j;

   if ( i->HasReference() )
   {
      Pixel* p = static_cast<Pixel*>( i->Reference() );
      int c = i->ReferenceChannel();
      if ( c < 0 )
         for ( int c = 0; c < p->Length(); ++c )
            r[c] = (*p)[c] = ((*p)[c] != 0 || (*j)[c] != 0) ? 1 : 0;
      else
         r.SetSamples( (*p)[c] = ((*p)[c] != 0 || (*j)[c] != 0) ? 1 : 0 );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = ((*i)[c] != 0 || (*j)[c] != 0) ? 1 : 0;
   }
}

// ----------------------------------------------------------------------------

#undef REF_ASSIGN

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Operator.cpp - Released 2024-03-20T10:42:12Z
