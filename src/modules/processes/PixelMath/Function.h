//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.0
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.5.0
// ----------------------------------------------------------------------------
// Function.h - Released 2020-07-31T19:33:39Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L. All Rights Reserved.
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
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
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

#ifndef __Function_h
#define __Function_h

#include "Functional.h"

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Representation of a function in a PixelMath expression.
 */
class Function : public Functional
{
public:

   typedef IndirectArray<Function>  function_set;

   struct IndexNode
   {
      String          token;
      const Function* function;

      IndexNode( const String& t, const Function* f = nullptr )
         : token( t )
         , function( f )
      {
      }

      IndexNode( const IndexNode& ) = default;

      bool operator ==( const IndexNode& x ) const
      {
         return token == x.token;
      }

      bool operator <( const IndexNode& x ) const
      {
         return token < x.token;
      }
   };

   typedef SortedArray<IndexNode>   function_index;

   Function( int p, int n )
      : Functional( XPR_FUNCTION, p )
      , numberOfArguments( n )
   {
   }

   Function( const Function& ) = default;

   virtual int NumberOfArguments() const
   {
      return arguments.IsEmpty() ? numberOfArguments : int( arguments.Length() );
   }

   virtual bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const
   {
      return true;
   }

   virtual Expression* Clone() const = 0;
   virtual Expression* Generate( int ) const = 0;

   virtual String Meta() const = 0;
   virtual String Id() const = 0;
   virtual String Token() const = 0;

   /*
    * NB. Aliases don't include canonical identifiers. So if a function has no
    * aliases, the result of this member function is an empty string.
    */
   virtual String Aliases() const
   {
      return String();
   }

   virtual int RequiredArguments() const
   {
      return 1;
   }

   virtual int MaximumArguments() const
   {
      return 1;
   }

   String ToString() const override;

   static void InitializeList( function_set&, function_index& );

private:

   int numberOfArguments;
};

// ----------------------------------------------------------------------------

class AbsFunction : public Function
{
public:

   AbsFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new AbsFunction( *this ); }
   Expression* Generate( int p ) const override { return new AbsFunction( p ); }

   String Meta() const override            { return "abs( x )"; }
   String Id() const override              { return "Absolute value of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "abs"; }
   String Aliases() const override         { return "Abs"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class ArcCosFunction : public Function
{
public:

   ArcCosFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new ArcCosFunction( *this ); }
   Expression* Generate( int p ) const override { return new ArcCosFunction( p ); }

   String Meta() const override            { return "acos( x )"; }
   String Id() const override              { return "Arc cosine of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "acos"; }
   String Aliases() const override         { return "ArcCos"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class ArcCoshFunction : public Function
{
public:

   ArcCoshFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new ArcCoshFunction( *this ); }
   Expression* Generate( int p ) const override { return new ArcCoshFunction( p ); }

   String Meta() const override            { return "acosh( x )"; }
   String Id() const override              { return "Hyperbolic arc cosine of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "acosh"; }
   String Aliases() const override         { return "ArcCosh"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class ArcSinFunction : public Function
{
public:

   ArcSinFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new ArcSinFunction( *this ); }
   Expression* Generate( int p ) const override { return new ArcSinFunction( p ); }

   String Meta() const override            { return "asin( x )"; }
   String Id() const override              { return "Arc sine of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "asin"; }
   String Aliases() const override         { return "ArcSin"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class ArcSinhFunction : public Function
{
public:

   ArcSinhFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new ArcSinhFunction( *this ); }
   Expression* Generate( int p ) const override { return new ArcSinhFunction( p ); }

   String Meta() const override            { return "asinh( x )"; }
   String Id() const override              { return "Hyperbolic arc sine of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "asinh"; }
   String Aliases() const override         { return "ArcSinh"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class ArcTanFunction : public Function
{
public:

   ArcTanFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new ArcTanFunction( *this ); }
   Expression* Generate( int p ) const override { return new ArcTanFunction( p ); }

   String Meta() const override            { return "atan( y[, x=1] )"; }
   String Id() const override              { return "Arc tangent of <i>y</i>/<i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "atan"; }
   String Aliases() const override         { return "ArcTan"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 2; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class ArcTanhFunction : public Function
{
public:

   ArcTanhFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new ArcTanhFunction( *this ); }
   Expression* Generate( int p ) const override { return new ArcTanhFunction( p ); }

   String Meta() const override            { return "atanh( x )"; }
   String Id() const override              { return "Hyperbolic arc tangent of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "atanh"; }
   String Aliases() const override         { return "ArcTanh"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MeanFunction : public Function
{
public:

   MeanFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MeanFunction( *this ); }
   Expression* Generate( int p ) const override { return new MeanFunction( p ); }

   String Meta() const override            { return "mean( a, b[, ...] )\n"
                                                    "mean( image )"; }
   String Id() const override              { return "(1) Arithmetic mean of a set of two or more arguments.\n"
                                                    "(2) Arithmetic mean of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "mean"; }
   String Aliases() const override         { return "avg,Avg"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class AvgDevFunction : public Function
{
public:

   AvgDevFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new AvgDevFunction( *this ); }
   Expression* Generate( int p ) const override { return new AvgDevFunction( p ); }

   String Meta() const override            { return "adev( a, b[, ...] )\n"
                                                    "adev( image )"; }
   String Id() const override              { return "(1) Average absolute deviation from the median of a set of two or more arguments.\n"
                                                    "(2) Average absolute deviation from the median of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "adev"; }
   String Aliases() const override         { return "avgDev,AvgDev"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MADFunction : public Function
{
public:

   MADFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MADFunction( *this ); }
   Expression* Generate( int p ) const override { return new MADFunction( p ); }

   String Meta() const override            { return "mdev( a, b[, ...] )\n"
                                                    "mdev( image )"; }
   String Id() const override              { return "(1) Median absolute deviation (MAD) from the median of a set of two or more arguments.\n"
                                                    "(2) Median absolute deviation (MAD) from the median of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "mdev"; }
   String Aliases() const override         { return "MAD,medDev,MedDev"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class BWMVFunction : public Function
{
public:

   BWMVFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new BWMVFunction( *this ); }
   Expression* Generate( int p ) const override { return new BWMVFunction( p ); }

   String Meta() const override            { return "bwmv( a, b[, ...] )\n"
                                                    "bwmv( image )"; }
   String Id() const override              { return "(1) Biweight midvariance of a set of two or more arguments.\n"
                                                    "(2) Biweight midvariance of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "bwmv"; }
   String Aliases() const override         { return "BWMV,biweightMidvariance"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class PBMVFunction : public Function
{
public:

   PBMVFunction( int p = 0, int n = 0 ) : Function( p, n ) {}
   Expression* Clone() const override { return new PBMVFunction( *this ); }

   Expression* Generate( int p ) const override { return new PBMVFunction( p ); }

   String Meta() const override            { return "pbmv( a, b[, ...] )\n"
                                                    "pbmv( image )"; }
   String Id() const override              { return "(1) Percentage bend midvariance of a set of two or more arguments.\n"
                                                    "(2) Percentage bend midvariance of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "pbmv"; }
   String Aliases() const override         { return "PBMV,bendMidvariance"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class SnFunction : public Function
{
public:

   SnFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new SnFunction( *this ); }
   Expression* Generate( int p ) const override { return new SnFunction( p ); }

   String Meta() const override            { return "Sn( a, b[, ...] )\n"
                                                    "Sn( image )"; }
   String Id() const override              { return "(1) Sn scale estimate of Rousseeuw and Croux for a set of two or more arguments.\n"
                                                    "(2) Sn scale estimate of Rousseeuw and Croux of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "Sn"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class QnFunction : public Function
{
public:

   QnFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new QnFunction( *this ); }
   Expression* Generate( int p ) const override { return new QnFunction( p ); }

   String Meta() const override            { return "Qn( a, b[, ...] )\n"
                                                    "Qn( image )"; }
   String Id() const override              { return "(1) Qn scale estimate of Rousseeuw and Croux for a set of two or more arguments.\n"
                                                    "(2) Qn scale estimate of Rousseeuw and Croux of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "Qn"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class ModFunction : public Function
{
public:

   ModFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new ModFunction( *this ); }
   Expression* Generate( int p ) const override { return new ModFunction( p ); }

   String Meta() const override            { return "mod( a, b[, ...] )\n"
                                                    "mod( image )"; }
   String Id() const override              { return "(1) Modulus (sum of absolute values) of a set of two or more arguments.\n"
                                                    "(2) Modulus (sum of absolute sample values) of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "mod"; }
   String Aliases() const override         { return "modulus,Mod,Modulus"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class NormFunction : public Function
{
public:

   NormFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new NormFunction( *this ); }
   Expression* Generate( int p ) const override { return new NormFunction( p ); }

   String Meta() const override            { return "norm( a, b[, ...] )\n"
                                                    "norm( image )"; }
   String Id() const override              { return "(1) Norm (sum) of a set of two or more arguments.\n"
                                                    "(2) Norm (sum of samples) of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "norm"; }
   String Aliases() const override         { return "Norm"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class SumOfSquaresFunction : public Function
{
public:

   SumOfSquaresFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new SumOfSquaresFunction( *this ); }
   Expression* Generate( int p ) const override { return new SumOfSquaresFunction( p ); }

   String Meta() const override            { return "ssqr( a, b[, ...] )\n"
                                                    "ssqr( image )"; }
   String Id() const override              { return "(1) Sum of the squares of a set of two or more arguments.\n"
                                                    "(2) Sum of the square sample values of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "ssqr"; }
   String Aliases() const override         { return "sumOfSquares,SumOfSquares"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MeanOfSquaresFunction : public Function
{
public:

   MeanOfSquaresFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MeanOfSquaresFunction( *this ); }
   Expression* Generate( int p ) const override { return new MeanOfSquaresFunction( p ); }

   String Meta() const override            { return "asqr( a, b[, ...] )\n"
                                                    "asqr( image )"; }
   String Id() const override              { return "(1) Mean of the squares of a set of two or more arguments.\n"
                                                    "(2) Mean of the square sample values of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "asqr"; }
   String Aliases() const override         { return "meanOfSquares,MeanOfSquares"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class CIELFunction : public Function
{
public:

   CIELFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIELFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIELFunction( p ); }

   String Meta() const override            { return "CIEL( image )"; }
   String Id() const override              { return "CIE L* component of the current pixel in an image."; }
   String Token() const override           { return "CIEL"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEaFunction : public Function
{
public:

   CIEaFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEaFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEaFunction( p ); }

   String Meta() const override            { return "CIEa( image )"; }
   String Id() const override              { return "CIE a* component of the current pixel in an image."; }
   String Token() const override           { return "CIEa"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEbFunction : public Function
{
public:

   CIEbFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEbFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEbFunction( p ); }

   String Meta() const override            { return "CIEb( image )"; }
   String Id() const override              { return "CIE b* component of the current pixel in an image."; }
   String Token() const override           { return "CIEb"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEcFunction : public Function
{
public:

   CIEcFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEcFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEcFunction( p ); }

   String Meta() const override            { return "CIEc( image )"; }
   String Id() const override              { return "CIE c* component of the current pixel in an image."; }
   String Token() const override           { return "CIEc"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEhFunction : public Function
{
public:

   CIEhFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEhFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEhFunction( p ); }

   String Meta() const override            { return "CIEh( image )"; }
   String Id() const override              { return "CIE h* component of the current pixel in an image, normalized "
                                                    "to the range [0,1)."; }
   String Token() const override           { return "CIEh"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEhrFunction : public Function
{
public:

   CIEhrFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEhrFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEhrFunction( p ); }

   String Meta() const override            { return "CIEhr( image )"; }
   String Id() const override              { return "CIE h* component of the current pixel in an image, expressed "
                                                    "in radians in the range [0,2&pi;)."; }
   String Token() const override           { return "CIEhr"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEhdFunction : public Function
{
public:

   CIEhdFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEhdFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEhdFunction( p ); }

   String Meta() const override            { return "CIEhd( image )"; }
   String Id() const override              { return "CIE h* component of the current pixel in an image, expressed "
                                                    "in degrees in the range [0&deg;,360&deg;)."; }
   String Token() const override           { return "CIEhd"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEXFunction : public Function
{
public:

   CIEXFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEXFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEXFunction( p ); }

   String Meta() const override            { return "CIEX( image )"; }
   String Id() const override              { return "CIE X component of the current pixel in an image."; }
   String Token() const override           { return "CIEX"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEYFunction : public Function
{
public:

   CIEYFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEYFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEYFunction( p ); }

   String Meta() const override            { return "CIEY( image )"; }
   String Id() const override              { return "CIE Y component of the current pixel in an image."; }
   String Token() const override           { return "CIEY"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEZFunction : public Function
{
public:

   CIEZFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEZFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEZFunction( p ); }

   String Meta() const override            { return "CIEZ( image )"; }
   String Id() const override              { return "CIE Z component of the current pixel in an image."; }
   String Token() const override           { return "CIEZ"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class CeilFunction : public Function
{
public:

   CeilFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CeilFunction( *this ); }
   Expression* Generate( int p ) const override { return new CeilFunction( p ); }

   String Meta() const override            { return "ceil( x )"; }
   String Id() const override              { return "Lowest integer greater than or equal to <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "ceil"; }
   String Aliases() const override         { return "Ceil"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class CosFunction : public Function
{
public:

   CosFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CosFunction( *this ); }
   Expression* Generate( int p ) const override { return new CosFunction( p ); }

   String Meta() const override            { return "cos( x )"; }
   String Id() const override              { return "Cosine of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "cos"; }
   String Aliases() const override         { return "Cos"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class CoshFunction : public Function
{
public:

   CoshFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CoshFunction( *this ); }
   Expression* Generate( int p ) const override { return new CoshFunction( p ); }

   String Meta() const override            { return "cosh( x )"; }
   String Id() const override              { return "Hyperbolic cosine of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "cosh"; }
   String Aliases() const override         { return "Cosh"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class ExpFunction : public Function
{
public:

   ExpFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new ExpFunction( *this ); }
   Expression* Generate( int p ) const override { return new ExpFunction( p ); }

   String Meta() const override            { return "exp( x )"; }
   String Id() const override              { return "Exponential function: <i>e</i><sup><i>x</i></sup>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "exp"; }
   String Aliases() const override         { return "Exp"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class FloorFunction : public Function
{
public:

   FloorFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new FloorFunction( *this ); }
   Expression* Generate( int p ) const override { return new FloorFunction( p ); }

   String Meta() const override            { return "floor( x )"; }
   String Id() const override              { return "Highest integer less than or equal to <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "floor"; }
   String Aliases() const override         { return "Floor"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class FracFunction : public Function
{
public:

   FracFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new FracFunction( *this ); }
   Expression* Generate( int p ) const override { return new FracFunction( p ); }

   String Meta() const override            { return "frac( x )"; }
   String Id() const override              { return "Fractional part of <i>x</i> in the range (-1,+1).\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "frac"; }
   String Aliases() const override         { return "Frac"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class HFunction : public Function
{
public:

   HFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new HFunction( *this ); }
   Expression* Generate( int p ) const override { return new HFunction( p ); }

   String Meta() const override            { return "H( image )"; }
   String Id() const override              { return "Hue angle of the current pixel in an image, "
                                                    "normalized to the range [0,1)."; }
   String Token() const override           { return "H"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class HrFunction : public Function
{
public:

   HrFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new HrFunction( *this ); }
   Expression* Generate( int p ) const override { return new HrFunction( p ); }

   String Meta() const override            { return "Hr( image )"; }
   String Id() const override              { return "Hue angle of the current pixel in an image, expressed in "
                                                    "radians in the range [0,2&pi;)."; }
   String Token() const override           { return "Hr"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class HdFunction : public Function
{
public:

   HdFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new HdFunction( *this ); }
   Expression* Generate( int p ) const override { return new HdFunction( p ); }

   String Meta() const override            { return "Hd( image )"; }
   String Id() const override              { return "Hue angle of the current pixel in an image, expressed in "
                                                    "degrees in the range [0&deg;,360&deg;)."; }
   String Token() const override           { return "Hd"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class SvFunction : public Function
{
public:

   SvFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new SvFunction( *this ); }
   Expression* Generate( int p ) const override { return new SvFunction( p ); }

   String Meta() const override            { return "Sv( image )"; }
   String Id() const override              { return "HSV saturation of the current pixel in an image."; }
   String Token() const override           { return "Sv"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class SiFunction : public Function
{
public:

   SiFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new SiFunction( *this ); }
   Expression* Generate( int p ) const override { return new SiFunction( p ); }

   String Meta() const override            { return "Si( image )"; }
   String Id() const override              { return "HSI saturation of the current pixel in an image."; }
   String Token() const override           { return "Si"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class VFunction : public Function
{
public:

   VFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new VFunction( *this ); }
   Expression* Generate( int p ) const override { return new VFunction( p ); }

   String Meta() const override            { return "V( image )"; }
   String Id() const override              { return "HSV value of the current pixel in an image."; }
   String Token() const override           { return "V"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class IFunction : public Function
{
public:

   IFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new IFunction( *this ); }
   Expression* Generate( int p ) const override { return new IFunction( p ); }

   String Meta() const override            { return "I( image )"; }
   String Id() const override              { return "HSI intensity of the current pixel in an image."; }
   String Token() const override           { return "I"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class LnFunction : public Function
{
public:

   LnFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new LnFunction( *this ); }
   Expression* Generate( int p ) const override { return new LnFunction( p ); }

   String Meta() const override            { return "ln( x )"; }
   String Id() const override              { return "Natural logarithm of <i>x</i>, or base-<i>e</i> logarithm, "
                                                    "where <i>e</i>=2.71828...\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "ln"; }
   String Aliases() const override         { return "Ln"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class LogFunction : public Function
{
public:

   LogFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new LogFunction( *this ); }
   Expression* Generate( int p ) const override { return new LogFunction( p ); }

   String Meta() const override            { return "log( x )"; }
   String Id() const override              { return "Base-10 logarithm of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "log"; }
   String Aliases() const override         { return "log10,Log10,Log"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class Log2Function : public Function
{
public:

   Log2Function( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new Log2Function( *this ); }
   Expression* Generate( int p ) const override { return new Log2Function( p ); }

   String Meta() const override            { return "log2( x )"; }
   String Id() const override              { return "Base-2 logarithm of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "log2"; }
   String Aliases() const override         { return "Log2"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MaxFunction : public Function
{
public:

   MaxFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MaxFunction( *this ); }
   Expression* Generate( int p ) const override { return new MaxFunction( p ); }

   String Meta() const override            { return "max( a, b[, ...] )\n"
                                                    "max( image )"; }
   String Id() const override              { return "(1) Maximum value of a set of two or more arguments.\n"
                                                    "(2) Maximum pixel sample value of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "max"; }
   String Aliases() const override         { return "Max,maximum"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MaxDevFunction : public Function
{
public:

   MaxDevFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MaxDevFunction( *this ); }
   Expression* Generate( int p ) const override { return new MaxDevFunction( p ); }

   String Meta() const override            { return "maxdev( a, b, c[, ...] )"; }
   String Id() const override              { return "Maximum absolute deviation of a set of three or more arguments."; }
   String Token() const override           { return "maxdev"; }
   String Aliases() const override         { return "maxDev,MaxDev"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return int_max; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MaxSampleFunction : public Function
{
public:

   MaxSampleFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MaxSampleFunction( *this ); }
   Expression* Generate( int p ) const override { return new MaxSampleFunction( p ); }

   String Meta() const override            { return "maxsample( image )"; }
   String Id() const override              { return "The maximum pixel sample value in the current pixel."; }
   String Token() const override           { return "maxsample"; }
   String Aliases() const override         { return "maxSample,MaxSample"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MedFunction : public Function
{
public:

   MedFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MedFunction( *this ); }
   Expression* Generate( int p ) const override { return new MedFunction( p ); }

   String Meta() const override            { return "med( a, b, c[, ...] )\n"
                                                    "med( image )"; }
   String Id() const override              { return "(1) Median value of a set of three or more arguments.\n"
                                                    "(2) Median pixel sample value of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "med"; }
   String Aliases() const override         { return "Med,median"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MinFunction : public Function
{
public:

   MinFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MinFunction( *this ); }
   Expression* Generate( int p ) const override { return new MinFunction( p ); }

   String Meta() const override            { return "min( a, b[, ...] )\n"
                                                    "min( image )"; }
   String Id() const override              { return "(1) Minimum value of a set of two or more arguments\n"
                                                    "(2) Minimum pixel sample value of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "min"; }
   String Aliases() const override         { return "Min,minimum"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MinSampleFunction : public Function
{
public:

   MinSampleFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MinSampleFunction( *this ); }
   Expression* Generate( int p ) const override { return new MinSampleFunction( p ); }

   String Meta() const override            { return "minsample( image )"; }
   String Id() const override              { return "The minimum pixel sample value in the current pixel."; }
   String Token() const override           { return "minsample"; }
   String Aliases() const override         { return "minSample,MinSample"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MTFFunction : public Function
{
public:

   MTFFunction( int p = 0 ) : Function( p, 2 ) {}

   Expression* Clone() const override { return new MTFFunction( *this ); }
   Expression* Generate( int p ) const override { return new MTFFunction( p ); }

   String Meta() const override            { return "mtf( m, x )"; }
   String Id() const override              { return "Midtones Transfer Function (MTF) of <i>x</i> for a midtones "
                                                    "balance <i>m</i>, 0 &lt;= <i>m</i> &lt;= 1.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "mtf"; }
   String Aliases() const override         { return "MTF"; }
   int RequiredArguments() const override  { return 2; }
   int MaximumArguments() const override   { return 2; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class PiFunction : public Function
{
public:

   PiFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new PiFunction( *this ); }
   Expression* Generate( int p ) const override { return new PiFunction( p ); }

   String Meta() const override            { return "pi()"; }
   String Id() const override              { return "The constant &pi;=3.141592...\n\n"
                                                    "Invariant subexpression: Always."; }
   String Token() const override           { return "pi"; }
   String Aliases() const override         { return "Pi,PI"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class RandomFunction : public Function
{
public:

   RandomFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new RandomFunction( *this ); }
   Expression* Generate( int p ) const override { return new RandomFunction( p ); }

   String Meta() const override            { return "random()"; }
   String Id() const override              { return "Pseudo-random uniform deviate in the range [0,1).\n"
                                                    "The current implementation uses the xoshiro256** algorithm by "
                                                    "David Blackman and Sebastiano Vigna."; }
   String Token() const override           { return "random"; }
   String Aliases() const override         { return "Random,rnd"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class GaussFunction : public Function
{
public:

   GaussFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new GaussFunction( *this ); }
   Expression* Generate( int p ) const override { return new GaussFunction( p ); }

   String Meta() const override            { return "gauss()"; }
   String Id() const override              { return "Normal (Gaussian) random deviate with zero mean and unit standard "
                                                    "deviation. A typical application of this function to add normal "
                                                    "distributed noise to the target image is:\n\n"
                                                    "$T + k*mdev( $T )*gauss()\n\n"
                                                    "where the median absolute deviation from the median (MAD), multiplied "
                                                    "by an optional amplitude parameter <i>k</i>, defines the standard "
                                                    "deviation of additive noise adaptively."; }
   String Token() const override           { return "gauss"; }
   String Aliases() const override         { return "Gauss"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class PoissonFunction : public Function
{
public:

   PoissonFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new PoissonFunction( *this ); }
   Expression* Generate( int p ) const override { return new PoissonFunction( p ); }

   String Meta() const override            { return "poisson( x )"; }
   String Id() const override              { return "Random Poisson deviate for the specified pixel sample value <i>x</i>, which "
                                                    "is expected to be in DN units. A typical application of this function is:\n\n"
                                                    "poisson( 65535*$T )/65535\n\n"
                                                    "which will generate Poisson noise for the current pixel value in the "
                                                    "[0,1] range, simulating a 16-bit detector with unit gain."; }
   String Token() const override           { return "poisson"; }
   String Aliases() const override         { return "Poisson"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class GammaFunction : public Function
{
public:

   GammaFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new GammaFunction( *this ); }
   Expression* Generate( int p ) const override { return new GammaFunction( p ); }

   String Meta() const override            { return "gamma( k[, t=1] )"; }
   String Id() const override              { return "Random deviate from a Gamma distribution with the specified shape and "
                                                    "scale parameters, <i>k</i> and <i>t</i> respectively. If not specified, "
                                                    "the default scale is <i>t</i> = 1.0."; }
   String Token() const override           { return "gamma"; }
   String Aliases() const override         { return "Gamma"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 2; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class ChiSquareFunction : public Function
{
public:

   ChiSquareFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new ChiSquareFunction( *this ); }
   Expression* Generate( int p ) const override { return new ChiSquareFunction( p ); }

   String Meta() const override            { return "chisq( k )"; }
   String Id() const override              { return "Random deviate from a chi squared distribution with the specified "
                                                    "degrees of freedom <i>k</i> &gt; 0."; }
   String Token() const override           { return "chisq"; }
   String Aliases() const override         { return "ChiSquare"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class StudentTFunction : public Function
{
public:

   StudentTFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new StudentTFunction( *this ); }
   Expression* Generate( int p ) const override { return new StudentTFunction( p ); }

   String Meta() const override            { return "studentt( n )"; }
   String Id() const override              { return "Random deviate from a Student's t distribution with the specified "
                                                    "degrees of freedom <i>n</i> &gt; 0."; }
   String Token() const override           { return "studentt"; }
   String Aliases() const override         { return "StudentT"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class RndSelectFunction : public Function
{
public:

   RndSelectFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new RndSelectFunction( *this ); }
   Expression* Generate( int p ) const override { return new RndSelectFunction( p ); }

   String Meta() const override            { return "rndselect( a, b[, ...] )"; }
   String Id() const override              { return "Returns a randomly selected value among the set of arguments {<i>a</i>, <i>b</i>, ...}."; }
   String Token() const override           { return "rndselect"; }
   String Aliases() const override         { return "RandomSelect"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class RoundFunction : public Function
{
public:

   RoundFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new RoundFunction( *this ); }
   Expression* Generate( int p ) const override { return new RoundFunction( p ); }

   String Meta() const override            { return "round( x[, n=0] )"; }
   String Id() const override              { return "Result of <i>x</i> rounded to <i>n</i> decimal digits, <i>n</i> &ge; 0.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "round"; }
   String Aliases() const override         { return "Round"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 2; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class SignFunction : public Function
{
public:

   SignFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new SignFunction( *this ); }
   Expression* Generate( int p ) const override { return new SignFunction( p ); }

   String Meta() const override            { return "sign( x )"; }
   String Id() const override              { return "The sign of <i>x</i>:\n\n"
                                                    "+1 if <i>x</i> &gt; 0\n"
                                                    "-1 if <i>x</i> &lt; 0\n"
                                                    " 0 if <i>x</i> = 0.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "sign"; }
   String Aliases() const override         { return "Sign"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class SinFunction : public Function
{
public:

   SinFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new SinFunction( *this ); }
   Expression* Generate( int p ) const override { return new SinFunction( p ); }

   String Meta() const override            { return "sin( x )"; }
   String Id() const override              { return "Sine of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "sin"; }
   String Aliases() const override         { return "Sin"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class SinhFunction : public Function
{
public:

   SinhFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new SinhFunction( *this ); }
   Expression* Generate( int p ) const override { return new SinhFunction( p ); }

   String Meta() const override            { return "sinh( x )"; }
   String Id() const override              { return "Hyperbolic sine of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "sinh"; }
   String Aliases() const override         { return "Sinh"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class SqrtFunction : public Function
{
public:

   SqrtFunction( int p = 0 ) : Function( p, 1 ) {}

   Expression* Clone() const override { return new SqrtFunction( *this ); }
   Expression* Generate( int p ) const override { return new SqrtFunction( p ); }

   String Meta() const override            { return "sqrt( x )"; }
   String Id() const override              { return "Square root of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "sqrt"; }
   String Aliases() const override         { return "Sqrt"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class StdDevFunction : public Function
{
public:

   StdDevFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new StdDevFunction( *this ); }
   Expression* Generate( int p ) const override { return new StdDevFunction( p ); }

   String Meta() const override            { return "sdev( a, b[, ...] )\n"
                                                    "sdev( image )"; }
   String Id() const override              { return "(1) Standard deviation of a set of two or more arguments\n"
                                                    "(2) Standard deviation of an image.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument is an image reference."; }
   String Token() const override           { return "sdev"; }
   String Aliases() const override         { return "stdDev,StdDev"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class SumFunction : public Function
{
public:

   SumFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new SumFunction( *this ); }
   Expression* Generate( int p ) const override { return new SumFunction( p ); }

   String Meta() const override            { return "sum( a, b[, ...] )"; }
   String Id() const override              { return "Summatory of a set of two or more arguments."; }
   String Token() const override           { return "sum"; }
   String Aliases() const override         { return "Sum"; }
   int RequiredArguments() const override  { return 2; }
   int MaximumArguments() const override   { return int_max; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class TanFunction : public Function
{
public:

   TanFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new TanFunction( *this ); }
   Expression* Generate( int p ) const override { return new TanFunction( p ); }

   String Meta() const override            { return "tan( x )"; }
   String Id() const override              { return "Tangent of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "tan"; }
   String Aliases() const override         { return "Tan"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class TanhFunction : public Function
{
public:

   TanhFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new TanhFunction( *this ); }
   Expression* Generate( int p ) const override { return new TanhFunction( p ); }

   String Meta() const override            { return "tanh( x )"; }
   String Id() const override              { return "Hyperbolic tangent of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "tanh"; }
   String Aliases() const override         { return "Tanh"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class TruncFunction : public Function
{
public:

   TruncFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new TruncFunction( *this ); }
   Expression* Generate( int p ) const override { return new TruncFunction( p ); }

   String Meta() const override            { return "trunc( x )"; }
   String Id() const override              { return "Truncated integer part of <i>x</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression."; }
   String Token() const override           { return "trunc"; }
   String Aliases() const override         { return "Trunc"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class VarFunction : public Function
{
public:

   VarFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new VarFunction( *this ); }
   Expression* Generate( int p ) const override { return new VarFunction( p ); }

   String Meta() const override            { return "var( image )"; }
   String Id() const override              { return "Variance of an image.\n\n"
                                                    "Invariant subexpression: Always."; }
   String Token() const override           { return "var"; }
   String Aliases() const override         { return "variance,Var"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class WidthFunction : public Function
{
public:

   WidthFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new WidthFunction( *this ); }
   Expression* Generate( int p ) const override { return new WidthFunction( p ); }

   String Meta() const override            { return "w( [image=$T] )"; }
   String Id() const override              { return "Width in pixels of an image.\n\n"
                                                    "Invariant subexpression: Always."; }
   String Token() const override           { return "w"; }
   String Aliases() const override         { return "width,Width"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class HeightFunction : public Function
{
public:

   HeightFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new HeightFunction( *this ); }
   Expression* Generate( int p ) const override { return new HeightFunction( p ); }

   String Meta() const override            { return "h( [image=$T] )"; }
   String Id() const override              { return "Height in pixels of an image.\n\n"
                                                    "Invariant subexpression: Always."; }
   String Token() const override           { return "h"; }
   String Aliases() const override         { return "height,Height"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class AreaFunction : public Function
{
public:

   AreaFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new AreaFunction( *this ); }
   Expression* Generate( int p ) const override { return new AreaFunction( p ); }

   String Meta() const override            { return "area( [image=$T] )"; }
   String Id() const override              { return "Area in square pixels of an image.\n\n"
                                                    "Invariant subexpression: Always."; }
   String Token() const override           { return "area"; }
   String Aliases() const override         { return "Area"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class NumberOfChannelsFunction : public Function
{
public:

   NumberOfChannelsFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new NumberOfChannelsFunction( *this ); }
   Expression* Generate( int p ) const override { return new NumberOfChannelsFunction( p ); }

   String Meta() const override            { return "n( [image=$T] )"; }
   String Id() const override              { return "Number of channels in an image, including nominal and alpha "
                                                    "channels.\n\n"
                                                    "Invariant subexpression: Always."; }
   String Token() const override           { return "n"; }
   String Aliases() const override         { return "numberOfChannels,NumberOfChannels"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class IsColorFunction : public Function
{
public:

   IsColorFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new IsColorFunction( *this ); }
   Expression* Generate( int p ) const override { return new IsColorFunction( p ); }

   String Meta() const override            { return "iscolor( [image=$T] )"; }
   String Id() const override              { return "Returns one if the specified image is in the RGB color space.\n"
                                                    "Returns zero for a grayscale image.\n\n"
                                                    "Invariant subexpression: Always."; }
   String Token() const override           { return "iscolor"; }
   String Aliases() const override         { return "isColor,IsColor"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class XPosFunction : public Function
{
public:

   XPosFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new XPosFunction( *this ); }
   Expression* Generate( int p ) const override { return new XPosFunction( p ); }

   String Meta() const override            { return "x()"; }
   String Id() const override              { return "Current horizontal (X-axis) coordinate in integer pixels."; }
   String Token() const override           { return "x"; }
   String Aliases() const override         { return "xpos,xPos,XPos"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class XFunction : public Function
{
public:

   XFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new XFunction( *this ); }
   Expression* Generate( int p ) const override { return new XFunction( p ); }

   String Meta() const override            { return "X()"; }
   String Id() const override              { return "Current normalized horizontal (X-axis) coordinate, in the [0,1] range."; }
   String Token() const override           { return "X"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class YPosFunction : public Function
{
public:

   YPosFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new YPosFunction( *this ); }
   Expression* Generate( int p ) const override { return new YPosFunction( p ); }

   String Meta() const override            { return "y()"; }
   String Id() const override              { return "Current vertical (Y-axis) coordinate in integer pixels."; }
   String Token() const override           { return "y"; }
   String Aliases() const override         { return "ypos,yPos,YPos"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class YFunction : public Function
{
public:

   YFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new YFunction( *this ); }
   Expression* Generate( int p ) const override { return new YFunction( p ); }

   String Meta() const override            { return "Y()"; }
   String Id() const override              { return "Current normalized vertical (Y-axis) coordinate, in the [0,1] range."; }
   String Token() const override           { return "Y"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class DistToLineFunction : public Function
{
public:

   DistToLineFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new DistToLineFunction( *this ); }
   Expression* Generate( int p ) const override { return new DistToLineFunction( p ); }

   String Meta() const override            { return "d2line( x1, y1, x2, y2 )"; }
   String Id() const override              { return "Returns the distance from the current position to the line defined "
                                                    "by two points (x1,y1) and (x2,y2)."; }
   String Token() const override           { return "d2line"; }
   String Aliases() const override         { return "DistToLine"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class DistToSegmentFunction : public Function
{
public:

   DistToSegmentFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new DistToSegmentFunction( *this ); }
   Expression* Generate( int p ) const override { return new DistToSegmentFunction( p ); }

   String Meta() const override            { return "d2seg( x1, y1, x2, y2 )"; }
   String Id() const override              { return "Returns the distance from the current position to the line segment "
                                                    "defined by two points (x1,y1) and (x2,y2)."; }
   String Token() const override           { return "d2seg"; }
   String Aliases() const override         { return "DistToSegment"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class XPerpToLineFunction : public Function
{
public:

   XPerpToLineFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new XPerpToLineFunction( *this ); }
   Expression* Generate( int p ) const override { return new XPerpToLineFunction( p ); }

   String Meta() const override            { return "xperp2line( x1, y1, x2, y2 )"; }
   String Id() const override              { return "Returns the X-coordinate of the intersection of the line through the current "
                                                    "position perpendicular to the line defined by two points (x1,y1) and (x2,y2)."; }
   String Token() const override           { return "xperp2line"; }
   String Aliases() const override         { return "XPerpToLine"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class YPerpToLineFunction : public Function
{
public:

   YPerpToLineFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new YPerpToLineFunction( *this ); }
   Expression* Generate( int p ) const override { return new YPerpToLineFunction( p ); }

   String Meta() const override            { return "yperp2line( x1, y1, x2, y2 )"; }
   String Id() const override              { return "Returns the Y-coordinate of the intersection of the line through the current "
                                                    "position perpendicular to the line defined by two points (x1,y1) and (x2,y2)."; }
   String Token() const override           { return "yperp2line"; }
   String Aliases() const override         { return "YPerpToLine"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class InRectFunction : public Function
{
public:

   InRectFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new InRectFunction( *this ); }
   Expression* Generate( int p ) const override { return new InRectFunction( p ); }

   String Meta() const override            { return "inrect( x0, y0, width, height )"; }
   String Id() const override              { return "Returns one if the current coordinates are inside the specified "
                                                    "rectangular region; returns zero otherwise."; }
   String Token() const override           { return "inrect"; }
   String Aliases() const override         { return "inRect,InRect"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MinDistRectFunction : public Function
{
public:

   MinDistRectFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MinDistRectFunction( *this ); }
   Expression* Generate( int p ) const override { return new MinDistRectFunction( p ); }

   String Meta() const override            { return "mind2rect( x0, y0, width, height )"; }
   String Id() const override              { return "Returns the minimum distance in pixels from the current coordinates "
                                                    "to the specified rectangle, when the current position is interior to "
                                                    "the rectangular region. Returns -1 if the current position is exterior "
                                                    "to the rectangular region."; }
   String Token() const override           { return "mind2rect"; }
   String Aliases() const override         { return "MinDistToRect"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MaxDistRectFunction : public Function
{
public:

   MaxDistRectFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MaxDistRectFunction( *this ); }
   Expression* Generate( int p ) const override { return new MaxDistRectFunction( p ); }

   String Meta() const override            { return "maxd2rect( x0, y0, width, height )"; }
   String Id() const override              { return "Returns the maximum distance in pixels from the current coordinates "
                                                    "to the specified rectangle, when the current position is interior to "
                                                    "the rectangular region. Returns -1 if the current position is exterior "
                                                    "to the rectangular region."; }
   String Token() const override           { return "maxd2rect"; }
   String Aliases() const override         { return "MaxDistToRect"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class InEllipseFunction : public Function
{
public:

   InEllipseFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new InEllipseFunction( *this ); }
   Expression* Generate( int p ) const override { return new InEllipseFunction( p ); }

   String Meta() const override            { return "inellipse( xc, yc, rx, ry )"; }
   String Id() const override              { return "Returns one if the current coordinates are included in the specified "
                                                    "ellipse; returns zero otherwise."; }
   String Token() const override           { return "inellipse"; }
   String Aliases() const override         { return "inEllipse,InEllipse"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class RDistFunction : public Function
{
public:

   RDistFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new RDistFunction( *this ); }
   Expression* Generate( int p ) const override { return new RDistFunction( p ); }

   String Meta() const override            { return "rdist( [xc, yc] )"; }
   String Id() const override              { return "Current radial distance in pixels, with respect to an arbitrary "
                                                    "center point {<i>xc</i>,<i>yc</i>}. If not specified, the default "
                                                    "center is the center of the central pixel of the target image"; }
   String Token() const override           { return "rdist"; }
   String Aliases() const override         { return "r,RDist"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 2; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class PAngleFunction : public Function
{
public:

   PAngleFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new PAngleFunction( *this ); }
   Expression* Generate( int p ) const override { return new PAngleFunction( p ); }

   String Meta() const override            { return "pangle( [xc, yc] )"; }
   String Id() const override              { return "Current polar angle in radians, with respect to an arbitrary "
                                                    "center point {<i>xc</i>,<i>yc</i>}. If not specified, the default "
                                                    "center is the geometric center of the target image"; }
   String Token() const override           { return "pangle"; }
   String Aliases() const override         { return "a,PAngle"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 2; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class PixelFunction : public Function
{
public:

   PixelFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new PixelFunction( *this ); }
   Expression* Generate( int p ) const override { return new PixelFunction( p ); }

   String Meta() const override            { return "pixel( image, x, y[, c] )"; }
   String Id() const override              { return "Pixel sample value of an image at <i>x</i>,<i>y</i> coordinates "
                                                    "of channel <i>c</i>. If not specified, the default channel is the "
                                                    "current channel corresponding to the running expression."; }
   String Token() const override           { return "pixel"; }
   String Aliases() const override         { return "Pixel"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class RangeFunction : public Function
{
public:

   RangeFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new RangeFunction( *this ); }
   Expression* Generate( int p ) const override { return new RangeFunction( p ); }

   String Meta() const override            { return "range( x, a, b )"; }
   String Id() const override              { return "<i>x</i> constrained to the [<i>a</i>,<i>b</i>] range:\n"
                                                    "Returns <i>x</i> if <i>x</i> &ge; <i>a</i> and <i>x</i> &le; <i>b</i>\n"
                                                    "Returns <i>a</i> if <i>x</i> &lt; <i>a</i>\n"
                                                    "Returns <i>b</i> if <i>x</i> &gt; <i>b</i>\n\n"
                                                    "Invariant subexpression: "
                                                    "When each of the three arguments is either an immediate "
                                                    "numeric value or an invariant subexpression."; }
   String Token() const override           { return "range"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return 3; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class RescaleFunction : public Function
{
public:

   RescaleFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new RescaleFunction( *this ); }
   Expression* Generate( int p ) const override { return new RescaleFunction( p ); }

   String Meta() const override            { return "rescale( x, a, b )"; }
   String Id() const override              { return "<i>x</i> rescaled linearly from the [<i>a</i>,<i>b</i>] range to [0,1]:\n"
                                                    "Returns 0 if <i>x</i> &lt; <i>a</i> or <i>b</i> - <i>a</i> is zero or insignificant.\n"
                                                    "Returns 1 if <i>x</i> &gt; <i>b</i>\n"
                                                    "Returns (<i>x</i> - <i>a</i>)/(<i>b</i> - <i>a</i>) if <i>x</i> &ge; <i>a</i> and <i>x</i> &le; <i>b</i>\n\n"
                                                    "If <i>b</i> &le; <i>a</i>, <i>a</i> and <i>b</i> are exchanged prior to evaluation.\n\n"
                                                    "Invariant subexpression: "
                                                    "When each of the three arguments is either an immediate "
                                                    "numeric value or an invariant subexpression."; }
   String Token() const override           { return "rescale"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return 3; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class InlineIfFunction : public Function
{
public:

   InlineIfFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new InlineIfFunction( *this ); }
   Expression* Generate( int p ) const override { return new InlineIfFunction( p ); }

   String Meta() const override            { return "iif( cond, expr_true, expr_false )"; }
   String Id() const override              { return "Conditional function (or <i>inline if</i> function):\n\n"
                                                    "Returns <i>expr_true</i> if <i>cond</i> evaluates to nonzero.\n"
                                                    "Returns <i>expr_false</i> if <i>cond</i> evaluates to zero.\n\n"
                                                    "Invariant subexpression: "
                                                    "When each of the three arguments is either an immediate "
                                                    "numeric value or an invariant subexpression."; }
   String Token() const override           { return "iif"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return 3; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool CanOptimize() const override       { return true; }
   component_list Optimized() const override;
};

// ----------------------------------------------------------------------------

class InlineSwitchFunction : public Function
{
public:

   InlineSwitchFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new InlineSwitchFunction( *this ); }
   Expression* Generate( int p ) const override { return new InlineSwitchFunction( p ); }

   String Meta() const override            { return "iswitch( cond_1, expr_1[, cond_2, expr_2[, ..., cond_n, expr_n]], expr_default )"; }
   String Id() const override              { return "Multiple conditional function (or <i>inline switch</i> function):\n\n"
                                                    "Returns <i>expr_1</i> if <i>cond_1</i> evaluates to nonzero;\n"
                                                    "otherwise, returns <i>expr_2</i> if <i>cond_2</i> evaluates to nonzero;\n"
                                                    "...\n"
                                                    "otherwise, returns <i>expr_n</i> if <i>cond_n</i> evaluates to nonzero;\n"
                                                    "otherwise, if all <i>cond_i</i> evaluate to zero "
                                                    "(1 &le; <i>i</i> &le; n), returns <i>expr_default</i>.\n\n"
                                                    "Invariant subexpression: "
                                                    "When all specified arguments are either immediate numeric values or "
                                                    "invariant subexpressions."; }
   String Token() const override           { return "iswitch"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool CanOptimize() const override       { return true; }
   component_list Optimized() const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Function_h

// ----------------------------------------------------------------------------
// EOF Function.h - Released 2020-07-31T19:33:39Z
