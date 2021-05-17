//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Function.h - Released 2021-05-05T15:38:07Z
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
      , m_numberOfArguments( n )
   {
   }

   Function( const Function& ) = default;

   virtual int NumberOfArguments() const
   {
      return arguments.IsEmpty() ? m_numberOfArguments : int( arguments.Length() );
   }

   virtual bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const
   {
      return true;
   }

   virtual Expression* Clone() const = 0;
   virtual Expression* Generate( int ) const = 0;

   virtual String Meta() const = 0;
   virtual String Id() const = 0;
   virtual String Token() const = 0;

   virtual bool IsImageGenerator() const
   {
      return false;
   }

   // Returns internal image key
   virtual IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const
   {
      return IsoString();
   }

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

   int m_numberOfArguments;
};

// ----------------------------------------------------------------------------

class AbsFunction : public Function
{
public:

   AbsFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new AbsFunction( *this ); }
   Expression* Generate( int p ) const override { return new AbsFunction( p ); }

   String Meta() const override            { return "abs( x )"; }
   String Id() const override              { return "<p>Absolute value of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Arc cosine of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Hyperbolic arc cosine of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Arc sine of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Hyperbolic arc sine of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Arc tangent of <i>y</i>/<i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Hyperbolic arc tangent of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
                                                    "mean( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Arithmetic mean function:</p>"
                                                    "<ol>"
                                                    "<li>Arithmetic mean of a set of two or more arguments.</li>"
                                                    "<li>Arithmetic mean of the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the arithmetic mean will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "arithmetic mean will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "mean"; }
   String Aliases() const override         { return "avg,Avg"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "adev( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Average absolute deviation from the median (ADEV):</p>"
                                                    "<ol>"
                                                    "<li>ADEV of a set of two or more arguments.</li>"
                                                    "<li>ADEV of the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the ADEV will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "ADEV will be computed for the entire image.</p>"
                                                    "<p>The returned value is the ADEV multiplied by 1.2533 in order to make the scale "
                                                    "estimate consistent with the standard deviation of a normal distribution.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "adev"; }
   String Aliases() const override         { return "avgDev,AvgDev"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "mdev( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Median absolute deviation from the median (MAD):</p>"
                                                    "<ol>"
                                                    "<li>MAD of a set of two or more arguments.</li>"
                                                    "<li>MAD of the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the MAD will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "MAD will be computed for the entire image.</p>"
                                                    "<p>The returned value is the MAD multiplied by 1.4826 in order to make the scale "
                                                    "estimate consistent with the standard deviation of a normal distribution.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "mdev"; }
   String Aliases() const override         { return "MAD,medDev,MedDev"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "bwmv( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Biweight midvariance (BWMV):</p>"
                                                    "<ol>"
                                                    "<li>BWMV of a set of two or more arguments.</li>"
                                                    "<li>BWMV of the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the BWMV will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "BWMV will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "bwmv"; }
   String Aliases() const override         { return "BWMV,biweightMidvariance"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "pbmv( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Percentage bend midvariance (PBMV):</p>"
                                                    "<ol>"
                                                    "<li>PBMV of a set of two or more arguments.</li>"
                                                    "<li>PBMV of the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the PBMV will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "PBMV will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "pbmv"; }
   String Aliases() const override         { return "PBMV,bendMidvariance"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "Sn( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>S<sub>n</sub> scale estimate of Rousseeuw and Croux:</p>"
                                                    "<ol>"
                                                    "<li>S<sub>n</sub> scale estimate for a set of two or more arguments.</li>"
                                                    "<li>S<sub>n</sub> scale estimate for the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the S<sub>n</sub> estimate will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "S<sub>n</sub> estimate will be computed for the entire image.</p>"
                                                    "<p>The returned value is S<sub>n</sub> multiplied by 1.1926 in order to make the scale "
                                                    "estimate consistent with the standard deviation of a normal distribution.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "Sn"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "Qn( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Q<sub>n</sub> scale estimate of Rousseeuw and Croux:</p>"
                                                    "<ol>"
                                                    "<li>Q<sub>n</sub> scale estimate for a set of two or more arguments.</li>"
                                                    "<li>Q<sub>n</sub> scale estimate for the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the Q<sub>n</sub> estimate will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "Q<sub>n</sub> estimate will be computed for the entire image.</p>"
                                                    "<p>The returned value is Q<sub>n</sub> multiplied by 2.2191 in order to make the scale "
                                                    "estimate consistent with the standard deviation of a normal distribution.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "Qn"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "mod( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Modulus, or the sum of absolute values:</p>"
                                                    "<ol>"
                                                    "<li>Modulus of a set of two or more arguments.</li>"
                                                    "<li>Modulus of the pixel samples in the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the modulus will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "modulus will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "mod"; }
   String Aliases() const override         { return "modulus,Mod,Modulus"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "norm( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Norm, or the sum of values:</p>"
                                                    "<ol>"
                                                    "<li>Norm of a set of two or more arguments.</li>"
                                                    "<li>Norm of the pixel samples in the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the norm will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "norm will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "norm"; }
   String Aliases() const override         { return "Norm"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "ssqr( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Sum of squares:</p>"
                                                    "<ol>"
                                                    "<li>Sum of the squares of a set of two or more arguments.</li>"
                                                    "<li>Sum of the squared pixel sample values in the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the sum of squares will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "sum of squares will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "ssqr"; }
   String Aliases() const override         { return "sumOfSquares,SumOfSquares"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "asqr( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Mean of squares:</p>"
                                                    "<ol>"
                                                    "<li>Mean of the squares of a set of two or more arguments.</li>"
                                                    "<li>Mean of the squared pixel sample values in the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the mean of squares will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "mean of squares will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "asqr"; }
   String Aliases() const override         { return "meanOfSquares,MeanOfSquares"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Normalized CIE L* component of the current pixel in the specified image, "
                                                    "in the range [0,1).</p>"; }
   String Token() const override           { return "CIEL"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEaFunction : public Function
{
public:

   CIEaFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEaFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEaFunction( p ); }

   String Meta() const override            { return "CIEa( image )"; }
   String Id() const override              { return "<p>Normalized CIE a* component of the current pixel in the specified image, "
                                                    "in the range [0,1).</p>"; }
   String Token() const override           { return "CIEa"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEbFunction : public Function
{
public:

   CIEbFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEbFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEbFunction( p ); }

   String Meta() const override            { return "CIEb( image )"; }
   String Id() const override              { return "<p>Normalized CIE b* component of the current pixel in the specified image, "
                                                    "in the range [0,1).</p>"; }
   String Token() const override           { return "CIEb"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEcFunction : public Function
{
public:

   CIEcFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEcFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEcFunction( p ); }

   String Meta() const override            { return "CIEc( image )"; }
   String Id() const override              { return "<p>Normalized CIE c* component of the current pixel in the specified image, "
                                                    "in the range [0,1).</p>"; }
   String Token() const override           { return "CIEc"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEhFunction : public Function
{
public:

   CIEhFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEhFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEhFunction( p ); }

   String Meta() const override            { return "CIEh( image )"; }
   String Id() const override              { return "<p>Normalized CIE h* component of the current pixel in the specified image, "
                                                    "in the range [0,1).</p>"; }
   String Token() const override           { return "CIEh"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEhrFunction : public Function
{
public:

   CIEhrFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEhrFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEhrFunction( p ); }

   String Meta() const override            { return "CIEhr( image )"; }
   String Id() const override              { return "<p>CIE h* component of the current pixel in the specified image, expressed "
                                                    "in radians in the range [0,2&pi;).</p>"; }
   String Token() const override           { return "CIEhr"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEhdFunction : public Function
{
public:

   CIEhdFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEhdFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEhdFunction( p ); }

   String Meta() const override            { return "CIEhd( image )"; }
   String Id() const override              { return "<p>CIE h* component of the current pixel in the specified image, expressed "
                                                    "in degrees in the range [0&deg;,360&deg;).</p>"; }
   String Token() const override           { return "CIEhd"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEXFunction : public Function
{
public:

   CIEXFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEXFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEXFunction( p ); }

   String Meta() const override            { return "CIEX( image )"; }
   String Id() const override              { return "<p>CIE X component of the current pixel in the specified image.</p>"; }
   String Token() const override           { return "CIEX"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEYFunction : public Function
{
public:

   CIEYFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEYFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEYFunction( p ); }

   String Meta() const override            { return "CIEY( image )"; }
   String Id() const override              { return "<p>CIE Y component of the current pixel in the specified image.</p>"; }
   String Token() const override           { return "CIEY"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class CIEZFunction : public Function
{
public:

   CIEZFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CIEZFunction( *this ); }
   Expression* Generate( int p ) const override { return new CIEZFunction( p ); }

   String Meta() const override            { return "CIEZ( image )"; }
   String Id() const override              { return "<p>CIE Z component of the current pixel in the specified image.</p>"; }
   String Token() const override           { return "CIEZ"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Lowest integer greater than or equal to <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Cosine of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Hyperbolic cosine of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Exponential function <i>e</i><sup><i>x</i></sup>, where <i>e</i>=2.71828...</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Highest integer less than or equal to <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Fractional part of <i>x</i> in the range (&minus;1,+1).</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Normalized hue angle of the current pixel in the specified image, "
                                                    "in the range [0,1).</p>"; }
   String Token() const override           { return "H"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class HrFunction : public Function
{
public:

   HrFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new HrFunction( *this ); }
   Expression* Generate( int p ) const override { return new HrFunction( p ); }

   String Meta() const override            { return "Hr( image )"; }
   String Id() const override              { return "<p>Hue angle of the current pixel in the specified image, expressed in "
                                                    "radians in the range [0,2&pi;).</p>"; }
   String Token() const override           { return "Hr"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

class HdFunction : public Function
{
public:

   HdFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new HdFunction( *this ); }
   Expression* Generate( int p ) const override { return new HdFunction( p ); }

   String Meta() const override            { return "Hd( image )"; }
   String Id() const override              { return "<p>Hue angle of the current pixel in the specified image, expressed in "
                                                    "degrees in the range [0&deg;,360&deg;).</p>"; }
   String Token() const override           { return "Hd"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>HSV saturation of the current pixel in the specified image.</p>"; }
   String Token() const override           { return "Sv"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>HSI saturation of the current pixel in the specified image.</p>"; }
   String Token() const override           { return "Si"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>HSV value of the current pixel in the specified image.</p>"; }
   String Token() const override           { return "V"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>HSI intensity of the current pixel in the specified image.</p>"; }
   String Token() const override           { return "I"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Natural logarithm of <i>x</i>, or base-<i>e</i> logarithm, "
                                                    "where <i>e</i>=2.71828...</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Base-10 logarithm of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Base-2 logarithm of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
                                                    "max( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Maximum function:</p>"
                                                    "<ol>"
                                                    "<li>Maximum value of a set of two or more arguments.</li>"
                                                    "<li>Maximum pixel sample value in the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the maximum pixel sample value will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "maximum pixel sample value will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "max"; }
   String Aliases() const override         { return "Max,maximum"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Maximum absolute deviation of a set of three or more arguments.</p>"; }
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
   String Id() const override              { return "<p>The maximum pixel sample value in the current pixel of the specified image.</p>"; }
   String Token() const override           { return "maxsample"; }
   String Aliases() const override         { return "maxSample,MaxSample"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "med( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Median function:</p>"
                                                    "<ol>"
                                                    "<li>Median value of a set of three or more scalar arguments.</li>"
                                                    "<li>Median pixel sample value in the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the median pixel sample value will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "median pixel sample value will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "med"; }
   String Aliases() const override         { return "Med,median"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
                                                    "min( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Minimum function:</p>"
                                                    "<ol>"
                                                    "<li>Minimum value of a set of two or more arguments.</li>"
                                                    "<li>Minimum pixel sample value in the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the minimum pixel sample value will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "minimum pixel sample value will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "min"; }
   String Aliases() const override         { return "Min,minimum"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>The minimum pixel sample value in the current pixel of the specified image.</p>"; }
   String Token() const override           { return "minsample"; }
   String Aliases() const override         { return "minSample,MinSample"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Midtones Transfer Function (MTF) of <i>x</i> for a midtones "
                                                    "balance parameter <i>m</i>, 0 &le; <i>m</i> &le; 1.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>The constant &pi; = 3.141592...</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
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
   String Id() const override              { return "<p>Pseudo-random uniform deviate in the range [0,1).</p>"
                                                    "<p>The current implementation uses the xoshiro256** algorithm by "
                                                    "David Blackman and Sebastiano Vigna.</p>"; }
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
   String Id() const override              { return "<p>Normal (Gaussian) random deviate with zero mean and unit standard "
                                                    "deviation.</p>"
                                                    "<p>A typical application of this function to add normal distributed noise "
                                                    "to the target image is:</p>"
                                                    "<p>$T + k*mdev( $T )*gauss()</p>"
                                                    "<p>where the median absolute deviation from the median (MAD), multiplied "
                                                    "by an optional amplitude parameter <i>k</i>, defines the standard "
                                                    "deviation of additive noise adaptively.</p>"; }
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
   String Id() const override              { return "<p>Random Poisson deviate for the specified pixel sample value <i>x</i>, which "
                                                    "is expected to be in DN units.</p>"
                                                    "<p>A typical application of this function is:</p>"
                                                    "<p>poisson( 65535*$T )/65535</p>"
                                                    "<p>which will generate Poisson noise for the current pixel value in the "
                                                    "[0,1] range, simulating a 16-bit detector with unit gain.</p>"; }
   String Token() const override           { return "poisson"; }
   String Aliases() const override         { return "Poisson"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Random deviate from a Gamma distribution with the specified shape and "
                                                    "scale parameters, <i>k</i> and <i>t</i> respectively.</p>"
                                                    "<p>If not specified, the default scale is <i>t</i> = 1.0.</p>"; }
   String Token() const override           { return "gamma"; }
   String Aliases() const override         { return "Gamma"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 2; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Random deviate from a chi squared distribution with the specified "
                                                    "degrees of freedom <i>k</i> &gt; 0.</p>"; }
   String Token() const override           { return "chisq"; }
   String Aliases() const override         { return "ChiSquare"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Random deviate from a Student's t distribution with the specified "
                                                    "degrees of freedom <i>n</i> &gt; 0.</p>"; }
   String Token() const override           { return "studentt"; }
   String Aliases() const override         { return "StudentT"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Returns a randomly selected value among the set of arguments "
                                                    "{<i>a</i>, <i>b</i>, ...}.</p>"; }
   String Token() const override           { return "rndselect"; }
   String Aliases() const override         { return "RandomSelect"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Result of <i>x</i> rounded to <i>n</i> decimal digits, <i>n</i> &ge; 0.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
   String Token() const override           { return "round"; }
   String Aliases() const override         { return "Round"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 2; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Sign of <i>x</i>:</p>"
                                                    "<ul>"
                                                    "<li>+1 if <i>x</i> &gt; 0</li>"
                                                    "<li>&minus;1 if <i>x</i> &lt; 0</li>"
                                                    "<li>&nbsp;0 if <i>x</i> = 0.</li>"
                                                    "</ul>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Sine of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Hyperbolic sine of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Square root of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
                                                    "sdev( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Standard deviation:</p>"
                                                    "<ol>"
                                                    "<li>Standard deviation of a set of two or more arguments.</li>"
                                                    "<li>Standard deviation of the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the standard deviation will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "standard deviation will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "sdev"; }
   String Aliases() const override         { return "stdDev,StdDev"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Summation of a set of two or more arguments.</p>"; }
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
   String Id() const override              { return "<p>Tangent of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Hyperbolic tangent of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Truncated integer part of <i>x</i>.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the argument <i>x</i> is either an immediate numeric value "
                                                    "or an invariant subexpression.</p>"; }
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

   String Meta() const override            { return "var( a, b[, ...] )\n"
                                                    "var( image[, x0, y0, w, h] )"; }
   String Id() const override              { return "<p>Variance:</p>"
                                                    "<ol>"
                                                    "<li>Variance of a set of two or more arguments.</li>"
                                                    "<li>Variance of the specified image.</li>"
                                                    "</ol>"
                                                    "<p>In the second form, the optional <i>x0</i>, <i>y0</i>, <i>w</i>, and <i>h</i> "
                                                    "parameters define a rectangular region of interest (ROI). These parameters are, "
                                                    "respectively, the left coordinate, the top coordinate, the width and the height "
                                                    "of the ROI, all of them expressed in integer pixel units. If a ROI is specified, "
                                                    "the variance will be computed exclusively for pixel samples within the "
                                                    "intersection between the ROI and the image bounds. If no ROI is specified, the "
                                                    "variance will be computed for the entire image.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When the first argument is an image reference with optional scalar ROI coordinates "
                                                    "(second form).</p>"; }
   String Token() const override           { return "var"; }
   String Aliases() const override         { return "variance,Var"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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

   String Meta() const override            { return "width( [image=$T] )\n"
                                                    "w( [image=$T] )"; }
   String Id() const override              { return "<p>Width in pixels of the specified image.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "width"; }
   String Aliases() const override         { return "w,Width"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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

   String Meta() const override            { return "height( [image=$T] )\n"
                                                    "h( [image=$T] )"; }
   String Id() const override              { return "<p>Height in pixels of the specified image.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "height"; }
   String Aliases() const override         { return "h,Height"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Area of the specified image in square pixels.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "area"; }
   String Aliases() const override         { return "Area"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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

   String Meta() const override            { return "channels( [image=$T] )\n"
                                                    "n( [image=$T] )"; }
   String Id() const override              { return "<p>Number of channels in the specified image, including nominal and alpha "
                                                    "channels.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "channels"; }
   String Aliases() const override         { return "n,numberOfChannels,NumberOfChannels"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Returns one if the specified image is in the RGB color space. "
                                                    "Returns zero for a grayscale image.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "iscolor"; }
   String Aliases() const override         { return "isColor,IsColor"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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

   String Meta() const override            { return "x()\n"
                                                    "xpos()"; }
   String Id() const override              { return "<p>Current horizontal (X-axis) coordinate in integer pixels.</p>"; }
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
   String Id() const override              { return "<p>Current normalized horizontal (X-axis) coordinate in the [0,1] range, "
                                                    "where 1 corresponds to w() &minus; 1.</p>"; }
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

   String Meta() const override            { return "y()\n"
                                                    "ypos()"; }
   String Id() const override              { return "<p>Current vertical (Y-axis) coordinate in integer pixels.</p>"; }
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
   String Id() const override              { return "<p>Current normalized vertical (Y-axis) coordinate in the [0,1] range, "
                                                    "where 1 corresponds to h() &minus; 1.</p>"; }
   String Token() const override           { return "Y"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class RAFunction : public Function
{
public:

   RAFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new RAFunction( *this ); }
   Expression* Generate( int p ) const override { return new RAFunction( p ); }

   String Meta() const override            { return "ra( image, x, y )"; }
   String Id() const override              { return "<p>Right ascension equatorial coordinate calculated for the specified image "
                                                    "at (<i>x</i>,<i>y</i>) image coordinates. The returned value is expressed "
                                                    "in degrees in the range [0,360).</p>"
                                                    "<p>This function requires a valid astrometric solution available in the "
                                                    "specified image.</p>"
                                                    "<p><b>Note:</b> Use of this function can lead to poor performance when "
                                                    "executed on large images with spline-based astrometric solutions (for "
                                                    "distortion correction) and slow machines with few processor cores.</p>"; }
   String Token() const override           { return "ra"; }
   String Aliases() const override         { return "RA"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return 3; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class RawRAFunction : public Function
{
public:

   RawRAFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new RawRAFunction( *this ); }
   Expression* Generate( int p ) const override { return new RawRAFunction( p ); }

   String Meta() const override            { return "rra( image, x, y )"; }
   String Id() const override              { return "<p>Right ascension equatorial coordinate calculated for the specified image "
                                                    "at (<i>x</i>,<i>y</i>) image coordinates. The returned value is expressed "
                                                    "in degrees and <u><i>not constrained</i></u> to a valid range of right ascensions.</p>"
                                                    "<p>This function is useful for interpolation schemes where discontinuities "
                                                    "caused by zero crossings in right ascension, i.e. abrupt changes from 360 to 0 "
                                                    "degrees, are not admissible numerically. Right ascensions returned by this "
                                                    "function can be larger than 360 degrees or less than zero, ensuring smooth "
                                                    "transitions.</p>"
                                                    "<p>This function requires a valid astrometric solution available in the "
                                                    "specified image.</p>"
                                                    "<p><b>Note:</b> Use of this function can lead to poor performance when "
                                                    "executed on large images with spline-based astrometric solutions (for "
                                                    "distortion correction) and slow machines with few processor cores.</p>"; }
   String Token() const override           { return "rra"; }
   String Aliases() const override         { return "RawRA"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return 3; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class DecFunction : public Function
{
public:

   DecFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new DecFunction( *this ); }
   Expression* Generate( int p ) const override { return new DecFunction( p ); }

   String Meta() const override            { return "dec( image, x, y )"; }
   String Id() const override              { return "<p>Declination equatorial coordinate calculated for the specified image at "
                                                    "(<i>x</i>,<i>y</i>) image coordinates. The returned value is expressed "
                                                    "in degrees in the range [-90,+90].</p>"
                                                    "<p>This function requires a valid astrometric solution available in the "
                                                    "specified image.</p>"
                                                    "<p><b>Note:</b> Use of this function can lead to poor performance when "
                                                    "executed on large images with spline-based astrometric solutions (for "
                                                    "distortion correction) and slow machines with few processor cores.</p>"; }
   String Token() const override           { return "dec"; }
   String Aliases() const override         { return "Dec"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return 3; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
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
   String Id() const override              { return "<p>Returns the distance from the current position to the line defined "
                                                    "by two points (<i>x1</i>,<i>y1</i>) and (<i>x2</i>,<i>y2</i>).</p>"; }
   String Token() const override           { return "d2line"; }
   String Aliases() const override         { return "DistToLine"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Returns the distance from the current position to the line segment "
                                                    "defined by two points (<i>x1</i>,<i>y1</i>) and (<i>x2</i>,<i>y2</i>).</p>"; }
   String Token() const override           { return "d2seg"; }
   String Aliases() const override         { return "DistToSegment"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Returns the X-coordinate of the intersection of the line through the current "
                                                    "position perpendicular to the line defined by two points (<i>x1</i>,<i>y1</i>) "
                                                    "and (<i>x2</i>,<i>y2</i>).</p>"; }
   String Token() const override           { return "xperp2line"; }
   String Aliases() const override         { return "XPerpToLine"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Returns the Y-coordinate of the intersection of the line through the current "
                                                    "position perpendicular to the line defined by two points (<i>x1</i>,<i>y1</i>) "
                                                    "and (<i>x2</i>,<i>y2</i>).</p>"; }
   String Token() const override           { return "yperp2line"; }
   String Aliases() const override         { return "YPerpToLine"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Returns one if the current coordinates are inside the specified rectangular "
                                                    "region given by its top-left corner coordinates (<i>x0</i>,<i>y0</i>), "
                                                    "<i>width</i> and <i>height</i> in pixels. Returns zero otherwise.</p>"; }
   String Token() const override           { return "inrect"; }
   String Aliases() const override         { return "inRect,InRect"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Returns the minimum distance in pixels from the current coordinates "
                                                    "to the specified rectangle given by its top-left corner coordinates "
                                                    "(<i>x0</i>,<i>y0</i>), <i>width</i> and <i>height</i> in pixels, when the "
                                                    "current position is interior to the rectangular region. Returns &minus;1 if the "
                                                    "current position is exterior to the rectangular region.</p>"; }
   String Token() const override           { return "mind2rect"; }
   String Aliases() const override         { return "MinDistToRect"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Returns the maximum distance in pixels from the current coordinates "
                                                    "to the specified rectangle given by its top-left corner coordinates "
                                                    "(<i>x0</i>,<i>y0</i>), <i>width</i> and <i>height</i> in pixels, when the "
                                                    "current position is interior to the rectangular region. Returns &minus;1 if the "
                                                    "current position is exterior to the rectangular region.</p>"; }
   String Token() const override           { return "maxd2rect"; }
   String Aliases() const override         { return "MaxDistToRect"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p>Returns one if the current coordinates are included in the specified "
                                                    "ellipse given by its center coordinates (<i>xc</i>,<i>yc</i>), horizontal "
                                                    "and vertical radii <i>rx</i> and <i>ry</i>, respectively. Returns zero otherwise.</p>"; }
   String Token() const override           { return "inellipse"; }
   String Aliases() const override         { return "inEllipse,InEllipse"; }
   int RequiredArguments() const override  { return 4; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class RDistFunction : public Function
{
public:

   RDistFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new RDistFunction( *this ); }
   Expression* Generate( int p ) const override { return new RDistFunction( p ); }

   String Meta() const override            { return "rdist( [xc, yc] )\n"
                                                    "r( [xc, yc] )"; }
   String Id() const override              { return "<p>Current radial distance in pixels with respect to an arbitrary "
                                                    "center point (<i>xc</i>,<i>yc</i>).</p>"
                                                    "<p>If not specified, the default center is the geometric center "
                                                    "of the target image.</p>"; }
   String Token() const override           { return "rdist"; }
   String Aliases() const override         { return "r,RDist"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 2; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class PAngleFunction : public Function
{
public:

   PAngleFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new PAngleFunction( *this ); }
   Expression* Generate( int p ) const override { return new PAngleFunction( p ); }

   String Meta() const override            { return "pangle( [xc, yc] )\n"
                                                    "a( [xc, yc] )"; }
   String Id() const override              { return "<p>Current polar angle in radians, with respect to an arbitrary "
                                                    "center point (<i>xc</i>,<i>yc</i>).</p>"
                                                    "<p>If not specified, the default center is the geometric center "
                                                    "of the target image.</p>"; }
   String Token() const override           { return "pangle"; }
   String Aliases() const override         { return "a,PAngle"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 2; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class PixelFunction : public Function
{
public:

   PixelFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new PixelFunction( *this ); }
   Expression* Generate( int p ) const override { return new PixelFunction( p ); }

   String Meta() const override            { return "pixel( image, x, y[, c] )\n"
                                                    "p( image, x, y[, c] )"; }
   String Id() const override              { return "<p>Pixel sample value of the specified image at (<i>x</i>,<i>y</i>) "
                                                    "coordinates of channel <i>c</i>.</p>"
                                                    "<p>If not specified, the default channel is the current channel "
                                                    "corresponding to the running expression.</p>"; }
   String Token() const override           { return "pixel"; }
   String Aliases() const override         { return "p,Pixel"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

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
   String Id() const override              { return "<p><i>x</i> constrained to the [<i>a</i>,<i>b</i>] range:</p>"
                                                    "<ul>"
                                                    "<li>Returns <i>x</i> if <i>x</i> &ge; <i>a</i> and <i>x</i> &le; <i>b</i></li>"
                                                    "<li>Returns <i>a</i> if <i>x</i> &lt; <i>a</i></li>"
                                                    "<li>Returns <i>b</i> if <i>x</i> &gt; <i>b</i></li>"
                                                    "</ul>"
                                                    "<p>Invariant subexpression: "
                                                    "When each of the three arguments is either an immediate "
                                                    "numeric value or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p><i>x</i> rescaled linearly from the [<i>a</i>,<i>b</i>] range to [0,1]:</p>"
                                                    "<ul>"
                                                    "<li>Returns 0 if <i>x</i> &lt; <i>a</i> or <i>b</i> &minus; <i>a</i> is zero or insignificant.</li>"
                                                    "<li>Returns 1 if <i>x</i> &gt; <i>b</i></li>"
                                                    "<li>Returns (<i>x</i> &minus; <i>a</i>)/(<i>b</i> &minus; <i>a</i>) if <i>x</i> &ge; <i>a</i> and <i>x</i> &le; <i>b</i></li>"
                                                    "</ul>"
                                                    "<p>If <i>b</i> &lt; <i>a</i>, <i>a</i> and <i>b</i> are exchanged prior to evaluation.</p>"
                                                    "<p>Invariant subexpression: "
                                                    "When each of the three arguments is either an immediate "
                                                    "numeric value or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Conditional function (or <i>inline if</i> function):</p>"
                                                    "<ul>"
                                                    "<li>Returns <i>expr_true</i> if <i>cond</i> evaluates to nonzero.</li>"
                                                    "<li>Returns <i>expr_false</i> if <i>cond</i> evaluates to zero.</li>"
                                                    "</ul>"
                                                    "<p>Invariant subexpression: "
                                                    "When each of the three arguments is either an immediate "
                                                    "numeric value or an invariant subexpression.</p>"; }
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
   String Id() const override              { return "<p>Multiple conditional function (or <i>inline switch</i> function):</p>"
                                                    "<ul>"
                                                    "<li>Returns <i>expr_1</i> if <i>cond_1</i> evaluates to nonzero;</li>"
                                                    "<li>otherwise, returns <i>expr_2</i> if <i>cond_2</i> evaluates to nonzero;<br/>"
                                                    "...</li>"
                                                    "<li>otherwise, returns <i>expr_n</i> if <i>cond_n</i> evaluates to nonzero;</li>"
                                                    "<li>otherwise, if all <i>cond_i</i> evaluate to zero "
                                                    "(1 &le; <i>i</i> &le; n), returns <i>expr_default</i>.</li>"
                                                    "</ul>"
                                                    "<p>Invariant subexpression: "
                                                    "When all specified arguments are either immediate numeric values or "
                                                    "invariant subexpressions.</p>"; }
   String Token() const override           { return "iswitch"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool CanOptimize() const override       { return true; }
   component_list Optimized() const override;
};

// ----------------------------------------------------------------------------

class GaussianConvolutionFunction : public Function
{
public:

   GaussianConvolutionFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new GaussianConvolutionFunction( *this ); }
   Expression* Generate( int p ) const override { return new GaussianConvolutionFunction( p ); }

   String Meta() const override            { return "gconv( image[, sigma=2[, rho=1[, theta=0[, eps=0.01]]]] )"; }
   String Id() const override              { return "<p>Convolution of the specified image with an elliptical Gaussian filter:</p>"
                                                    "<ul>"
                                                    "<li><i>sigma</i>: Standard deviation of the filter distribution on its major axis "
                                                      "(<i>sigma</i> &gt; 0).<br/></li>"
                                                    "<li><i>rho</i>: The ratio between the minor and major axes of the generated filter "
                                                      "distribution (0 &le; <i>rho</i> &le; 1).<br/></li>"
                                                    "<li><i>theta</i>: Rotation angle of the major filter axis in degrees "
                                                      "(&minus;180&deg; &le; <i>theta</i> &le; 180&deg;). Ignored when <i>rho</i> = 1.<br/></li>"
                                                    "<li><i>eps</i>: Maximum truncation error of computed filter coefficients "
                                                      "(<i>eps</i> &gt; 0).</li>"
                                                    "</ul>"; }
   String Token() const override           { return "gconv"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 5; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class BoxConvolutionFunction : public Function
{
public:

   BoxConvolutionFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new BoxConvolutionFunction( *this ); }
   Expression* Generate( int p ) const override { return new BoxConvolutionFunction( p ); }

   String Meta() const override            { return "bconv( image[, n=3] )"; }
   String Id() const override              { return "<p>Convolution of the specified image with a box average filter of odd size <i>n</i> &ge; 3.</p>"; }
   String Token() const override           { return "bconv"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 2; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class KernelConvolutionFunction : public Function
{
public:

   KernelConvolutionFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new KernelConvolutionFunction( *this ); }
   Expression* Generate( int p ) const override { return new KernelConvolutionFunction( p ); }

   String Meta() const override            { return "kconv( image, k11, k12, k13, k21, k22, k23, k31, k32, k33[, ...] )"; }
   String Id() const override              { return "<p>Convolution of the specified image with a custom kernel filter.</p>"
                                                    "<p>The arguments <i>k<sub>ij</sub></i> are filter kernel elements specified in row-major order. "
                                                    "The minimum kernel size is 3&times;3, hence at least 9 elements must be defined. "
                                                    "For the smallest 3&times;3 kernel we have the following filter configuration:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   k11 k12 k13<br/>"
                                                    "   k21 k22 k23<br/>"
                                                    "   k31 k32 k33</p>"
                                                    "<p>Filter kernels must be square matrices with odd dimensions, so valid kernel sizes are "
                                                    "3&times;3, 5&times;5, 7&times;7, etc.</p>"; }
   String Token() const override           { return "kconv"; }
   int RequiredArguments() const override  { return 10; }
   int MaximumArguments() const override   { return int_max; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MedianFilterFunction : public Function
{
public:

   MedianFilterFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MedianFilterFunction( *this ); }
   Expression* Generate( int p ) const override { return new MedianFilterFunction( p ); }

   String Meta() const override            { return "medfilt( image[, n=3[, str=str_square()]] )"; }
   String Id() const override              { return "<p>Median filter with odd filter size <i>n</i> &ge; 3 and structuring element <i>str</i>, "
                                                    "applied to the specified image.</p>"; }
   String Token() const override           { return "medfilt"; }
   String Aliases() const override         { return "medfilter"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 3; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class ErosionFilterFunction : public Function
{
public:

   ErosionFilterFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new ErosionFilterFunction( *this ); }
   Expression* Generate( int p ) const override { return new ErosionFilterFunction( p ); }

   String Meta() const override            { return "erosion( image[, n=3[, str=str_square()]] )\n"
                                                    "minfilt( image[, n=3[, str=str_square()]] )"; }
   String Id() const override              { return "<p>Erosion filter with odd filter size <i>n</i> &ge; 3 and structuring element <i>str</i>, "
                                                    "applied to the specified image.</p>"; }
   String Token() const override           { return "erosion"; }
   String Aliases() const override         { return "minfilt,minfilter"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 3; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class DilationFilterFunction : public Function
{
public:

   DilationFilterFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new DilationFilterFunction( *this ); }
   Expression* Generate( int p ) const override { return new DilationFilterFunction( p ); }

   String Meta() const override            { return "dilation( image[, n=3[, str=str_square()]] )\n"
                                                    "maxfilt( image[, n=3[, str=str_square()]] )"; }
   String Id() const override              { return "<p>Dilation filter with odd filter size <i>n</i> &ge; 3 and structuring element <i>str</i>, "
                                                    "applied to the specified image.</p>"; }
   String Token() const override           { return "dilation"; }
   String Aliases() const override         { return "maxfilt,maxfilter"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 3; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class StrSquareFunction : public Function
{
public:

   StrSquareFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new StrSquareFunction( *this ); }
   Expression* Generate( int p ) const override { return new StrSquareFunction( p ); }

   String Meta() const override            { return "str_square()"; }
   String Id() const override              { return "<p>Represents a square structuring element in a morphological transformation, "
                                                    "such as the medfilt(), erosion() or dilation() generators.</p>"
                                                    "<p>A square structure defines all of its elements. For example, a square structure "
                                                    "of size 5 is:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   x x x x x<br/>"
                                                    "   x x x x x<br/>"
                                                    "   x x x x x<br/>"
                                                    "   x x x x x<br/>"
                                                    "   x x x x x"
                                                    "</p>"
                                                    "<p>where existing elements are marked as 'x'.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "str_square"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class StrCircularFunction : public Function
{
public:

   StrCircularFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new StrCircularFunction( *this ); }
   Expression* Generate( int p ) const override { return new StrCircularFunction( p ); }

   String Meta() const override            { return "str_circular()"; }
   String Id() const override              { return "<p>Represents a circular structuring element in a morphological transformation, "
                                                    "such as the medfilt(), erosion() or dilation() generators.</p>"
                                                    "<p>For example, the standard circular structure of size 7 is defined as follows:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   . . x x x . .<br/>"
                                                    "   . x x x x x .<br/>"
                                                    "   x x x x x x x<br/>"
                                                    "   x x x x x x x<br/>"
                                                    "   x x x x x x x<br/>"
                                                    "   . x x x x x .<br/>"
                                                    "   . . x x x . ."
                                                    "</p>"
                                                    "<p>where existing elements are marked as 'x' and nonexisting elements are denoted as '.'.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "str_circular"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class StrOrthogonalFunction : public Function
{
public:

   StrOrthogonalFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new StrOrthogonalFunction( *this ); }
   Expression* Generate( int p ) const override { return new StrOrthogonalFunction( p ); }

   String Meta() const override            { return "str_orthogonal()"; }
   String Id() const override              { return "<p>Represents a orthogonal structuring element in a morphological transformation, "
                                                    "such as the medfilt(), erosion() or dilation() generators.</p>"
                                                    "<p>For example, an orthogonal structure of size 5 is defined as follows:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   . . x . .<br/>"
                                                    "   . . x . .<br/>"
                                                    "   x x x x x<br/>"
                                                    "   . . x . .<br/>"
                                                    "   . . x . ."
                                                    "</p>"
                                                    "<p>where existing elements are marked as 'x' and nonexisting elements are denoted as '.'.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "str_orthogonal"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class StrDiagonalFunction : public Function
{
public:

   StrDiagonalFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new StrDiagonalFunction( *this ); }
   Expression* Generate( int p ) const override { return new StrDiagonalFunction( p ); }

   String Meta() const override            { return "str_diagonal()"; }
   String Id() const override              { return "<p>Represents a diagonal structuring element in a morphological transformation, "
                                                    "such as the medfilt(), erosion() or dilation() generators.</p>"
                                                    "<p>For example, a diagonal structure of size 5 is defined as follows:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   x . . . x<br/>"
                                                    "   . x . x .<br/>"
                                                    "   . . x . .<br/>"
                                                    "   . x . x .<br/>"
                                                    "   x . . . x"
                                                    "</p>"
                                                    "<p>where existing elements are marked as 'x' and nonexisting elements are denoted as '.'.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "str_diagonal"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class StrStarFunction : public Function
{
public:

   StrStarFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new StrStarFunction( *this ); }
   Expression* Generate( int p ) const override { return new StrStarFunction( p ); }

   String Meta() const override            { return "str_star()"; }
   String Id() const override              { return "<p>Represents a star structuring element in a morphological transformation, "
                                                    "such as the medfilt(), erosion() or dilation() generators.</p>"
                                                    "<p>For example, a star structure of size 7 is defined as follows:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   . . . x . . .<br/>"
                                                    "   . x . x . x .<br/>"
                                                    "   . . x x x . .<br/>"
                                                    "   x x x x x x x<br/>"
                                                    "   . . x x x . .<br/>"
                                                    "   . x . x . x .<br/>"
                                                    "   . . . x . . ."
                                                    "</p>"
                                                    "<p>where existing elements are marked as 'x' and nonexisting elements are denoted as '.'.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "str_star"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class StrThreeWayFunction : public Function
{
public:

   StrThreeWayFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new StrThreeWayFunction( *this ); }
   Expression* Generate( int p ) const override { return new StrThreeWayFunction( p ); }

   String Meta() const override            { return "str_threeway()"; }
   String Id() const override              { return "<p>Represents a standard three-way structuring element in a morphological transformation, "
                                                    "such as the medfilt(), erosion() or dilation() generators.</p>"
                                                    "<p>The standard three-way structure of size 5 is defined as follows:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "     Way #0       Way #1       Way #2<br/>"
                                                    "   =========    =========    =========<br/>"
                                                    "   . . x . .    x . . . x    . . . . .<br/>"
                                                    "   . . x . .    . x . x .    . . . . .<br/>"
                                                    "   x x . x x    . . . . .    . . x . .<br/>"
                                                    "   . . x . .    . x . x .    . . . . .<br/>"
                                                    "   . . x . .    x . . . x    . . . . ."
                                                    "</p>"
                                                    "<p>where existing elements are marked as 'x' and nonexisting elements are denoted as '.'.</p>"
                                                    "<p>Three-way structures lead to ranking operations where data from "
                                                    "different spatial directions are ranked separately. When used to apply "
                                                    "morphological operators, standard three-way structures can preserve edges "
                                                    "better than box structures, especially when applied recursively.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "str_threeway"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class TranslationFunction : public Function
{
public:

   TranslationFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new TranslationFunction( *this ); }
   Expression* Generate( int p ) const override { return new TranslationFunction( p ); }

   String Meta() const override            { return "translate( image, dx, dy )"; }
   String Id() const override              { return "<p>Translation of an image by the specified increments <i>dx</i> and <i>dy</i>, "
                                                    "respectively in the horizontal (X-axis) and vertical (Y-axis) directions.</p>"; }
   String Token() const override           { return "translate"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return 3; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class RotationFunction : public Function
{
public:

   RotationFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new RotationFunction( *this ); }
   Expression* Generate( int p ) const override { return new RotationFunction( p ); }

   String Meta() const override            { return "rotate( image, angle[, dx=0, dy=0] )"; }
   String Id() const override              { return "<p>Rotation of an image by the specified <i>angle</i> in degrees, "
                                                    "with center of rotation located at (<i>dx</i>,<i>dy</i>) coordinates "
                                                    "measured in pixels from the geometric center of the image.</p>"
                                                    "<p>If no center coordinates are specified, the default center of rotation is (0,0), "
                                                    "which corresponds to the geometric center of the image.</p>"; }
   String Token() const override           { return "rotate"; }
   int RequiredArguments() const override  { return 2; }
   int MaximumArguments() const override   { return 4; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MirrorHorzFunction : public Function
{
public:

   MirrorHorzFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MirrorHorzFunction( *this ); }
   Expression* Generate( int p ) const override { return new MirrorHorzFunction( p ); }

   String Meta() const override            { return "hmirror( image )"; }
   String Id() const override              { return "<p>Horizontal mirror (or horizontal reflection) of the specified image.</p>"; }
   String Token() const override           { return "hmirror"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class MirrorVertFunction : public Function
{
public:

   MirrorVertFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new MirrorVertFunction( *this ); }
   Expression* Generate( int p ) const override { return new MirrorVertFunction( p ); }

   String Meta() const override            { return "vmirror( image )"; }
   String Id() const override              { return "<p>Vertical mirror (or vertical reflection) of the specified image.</p>"; }
   String Token() const override           { return "vmirror"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 1; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class NormalizationFunction : public Function
{
public:

   NormalizationFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new NormalizationFunction( *this ); }
   Expression* Generate( int p ) const override { return new NormalizationFunction( p ); }

   String Meta() const override            { return "normalize( image[, a=0, b=1] )"; }
   String Id() const override              { return "<p>The specified image with its pixel sample values scaled linearly "
                                                    "to the range [<i>a</i>,<i>b</i>].</p>"
                                                    "<p>For each pixel sample <i>v</i>, the corresponding normalized value <i>v</i>' is given by:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   <i>v</i>' = <i>a</i> + (<i>b</i> - <i>a</i>)&times;(<i>v</i> - <i>m</i>)/(<i>M</i> - <i>m</i>)</p>"
                                                    "<p>where <i>m</i> and <i>M</i> are, respectively, the minimum and maximum pixel sample values in the image before normalization.</p>"
                                                    ; }
   String Token() const override           { return "normalize"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 3; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class TruncationFunction : public Function
{
public:

   TruncationFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new TruncationFunction( *this ); }
   Expression* Generate( int p ) const override { return new TruncationFunction( p ); }

   String Meta() const override            { return "truncate( image[, a=0, b=1] )"; }
   String Id() const override              { return "<p>The specified image with its pixel sample values truncated "
                                                    "to the range [<i>a</i>,<i>b</i>].</p>"
                                                    "<p>For each pixel sample <i>v</i>, the corresponding truncated sample value <i>v'</i> "
                                                    "is given by:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   <i>v'</i> = <i>a</i> if <i>v</i> &lt; <i>a</i><br/>"
                                                    "   <i>v'</i> = <i>b</i> if <i>v</i> &gt; <i>b</i><br/>"
                                                    "   <i>v'</i> = <i>v</i> if <i>a</i> &le; <i>v</i> &le; <i>b</i></p>"; }
   String Token() const override           { return "truncate"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 3; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class BinarizationFunction : public Function
{
public:

   BinarizationFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new BinarizationFunction( *this ); }
   Expression* Generate( int p ) const override { return new BinarizationFunction( p ); }

   String Meta() const override            { return "binarize( image[, t=0.5] )"; }
   String Id() const override              { return "<p>The specified image with its pixel sample values binarized "
                                                    "with respect to the threshold value <i>t</i>."
                                                    "<p>For each pixel sample <i>v</i>, the corresponding binarized sample value <i>v'</i> "
                                                    "is given by:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   <i>v'</i> = 0 if <i>v</i> &lt; <i>t</i><br/>"
                                                    "   <i>v'</i> = 1 if <i>v</i> &ge; <i>t</i></p>"; }
   String Token() const override           { return "binarize"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 2; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class LocalVarianceFunction : public Function
{
public:

   LocalVarianceFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new LocalVarianceFunction( *this ); }
   Expression* Generate( int p ) const override { return new LocalVarianceFunction( p ); }

   String Meta() const override            { return "lvar( image[, d=3[, k=krn_flat()]] )"; }
   String Id() const override              { return "<p>Local variance map of the specified image with odd window size <i>d</i> &ge; 3 pixels "
                                                    "and kernel function <i>k</i>.</p>"
                                                    "<p>A local variance map is a sensitive analysis tool to explore the distribution of "
                                                    "pixel intensity variations at small scales. This allows for the implementation of "
                                                    "subtle adaptive procedures with PixelMath. For example, the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   iif( lvar( $T, 7, krn_gauss() ) &lt; t<br/>"
                                                    "        , medfilt( $T, 5, str_circular() )<br/>"
                                                    "        , $T )</p>"
                                                    "<p>applies a median filter selectively to pixels where the local variance is below a prescribed "
                                                    "threshold <i>t</i>, which depends on the target image and must be found experimentally. "
                                                    "For example, <i>t</i>=1e-8 can be a good starting value for a linear deep-sky image. The "
                                                    "expression above implements a powerful noise reduction procedure. More sophisticated, adaptive "
                                                    "noise filtering processes based on the same technique can be designed with iswitch() constructs.</p>"
                                                    "<p>Local variance maps are always generated as 64-bit floating point images. This ensures that "
                                                    "no truncation or roundoff errors will degrade their ability to represent subtle intensity "
                                                    "variations, which is especially important for linear images.</p>"; }
   String Token() const override           { return "lvar"; }
   int RequiredArguments() const override  { return 1; }
   int MaximumArguments() const override   { return 3; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class KrnFlatFunction : public Function
{
public:

   KrnFlatFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new KrnFlatFunction( *this ); }
   Expression* Generate( int p ) const override { return new KrnFlatFunction( p ); }

   String Meta() const override            { return "krn_flat()"; }
   String Id() const override              { return "<p>Represents a flat kernel function for local variance calculation with the lvar() generator.</p>"
                                                    "<p>A flat function has identical kernel elements. For example, the flat kernel of size 5 is:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   1/25 1/25 1/25 1/25 1/25<br/>"
                                                    "   1/25 1/25 1/25 1/25 1/25<br/>"
                                                    "   1/25 1/25 1/25 1/25 1/25<br/>"
                                                    "   1/25 1/25 1/25 1/25 1/25<br/>"
                                                    "   1/25 1/25 1/25 1/25 1/25"
                                                    "</p>"
                                                    "<p>When applied by convolution, a flat kernel function of size <i>n</i> computes the "
                                                    "arithmetic mean of a neighborhood of n&times;n pixels.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "krn_flat"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class KrnGaussianFunction : public Function
{
public:

   KrnGaussianFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new KrnGaussianFunction( *this ); }
   Expression* Generate( int p ) const override { return new KrnGaussianFunction( p ); }

   String Meta() const override            { return "krn_gauss()"; }
   String Id() const override              { return "<p>Represents a Gaussian kernel function for local variance calculation with the lvar() generator.</p>"
                                                    "<p>A Gaussian kernel is a discrete representation of a normal distribution in two dimensions. "
                                                    "For example, the Gaussian kernel of size 5 is:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   0.000100  0.003162  0.010000  0.003162  0.000100<br/>"
                                                    "   0.003162  0.100000  0.316228  0.100000  0.003162<br/>"
                                                    "   0.010000  0.316228  1.000000  0.316228  0.010000<br/>"
                                                    "   0.003162  0.100000  0.316228  0.100000  0.003162<br/>"
                                                    "   0.000100  0.003162  0.010000  0.003162  0.000100"
                                                    "</p>"
                                                    "<p>For local variance calculation, a Gaussian kernel helps to isolate pixel variations at "
                                                    "smaller scales, compared with a flat kernel, since the normal distribution concentrates "
                                                    "more energy around the center of the 2-D distribution. It also tends to generate more isotropic "
                                                    "variance maps, since the flat kernel tends to reproduce a square structure while the Gaussian "
                                                    "kernel approximates to a more circular distribution.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "krn_gauss"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class CombinationFunction : public Function
{
public:

   CombinationFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new CombinationFunction( *this ); }
   Expression* Generate( int p ) const override { return new CombinationFunction( p ); }

   String Meta() const override            { return "combine( image1, image2, op )"; }
   String Id() const override              { return "<p>Combination of two source operand images <i>image1</i> and <i>image2</i> "
                                                    "with the specified binary operation <i>op</i>. Returns the result of "
                                                    "<i>image1</i> <i>op</i> <i>image2</i>, which is a dynamically generated image.</p>"
                                                    "<p>See the family of op_xxx() symbolic functions for the set of available binary "
                                                    "image operators.</p>"; }
   String Token() const override           { return "combine"; }
   int RequiredArguments() const override  { return 3; }
   int MaximumArguments() const override   { return 3; }

   bool ValidateArguments( String&, Expression*&, component_list::const_iterator, component_list::const_iterator ) const override;

   bool IsImageGenerator() const override
   {
      return true;
   }

   IsoString GenerateImage( component_list::const_iterator, component_list::const_iterator ) const override;

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpMovFunction : public Function
{
public:

   OpMovFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpMovFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpMovFunction( p ); }

   String Meta() const override            { return "op_mov()"; }
   String Id() const override              { return "<p>Represents a pixel wise assignment operation for the combine() generator.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_mov"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpAddFunction : public Function
{
public:

   OpAddFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpAddFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpAddFunction( p ); }

   String Meta() const override            { return "op_add()"; }
   String Id() const override              { return "<p>Represents a pixel wise arithmetic addition operation for the combine() generator.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_add"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpSubFunction : public Function
{
public:

   OpSubFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpSubFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpSubFunction( p ); }

   String Meta() const override            { return "op_sub()"; }
   String Id() const override              { return "<p>Represents a pixel wise arithmetic subtraction operation for the combine() generator.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_sub"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpMulFunction : public Function
{
public:

   OpMulFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpMulFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpMulFunction( p ); }

   String Meta() const override            { return "op_mul()"; }
   String Id() const override              { return "<p>Represents a pixel wise arithmetic multiplication operation for the combine() generator.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_mul"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpDivFunction : public Function
{
public:

   OpDivFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpDivFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpDivFunction( p ); }

   String Meta() const override            { return "op_div()"; }
   String Id() const override              { return "<p>Represents a pixel wise arithmetic division operation for the combine() generator.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_div"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpPowFunction : public Function
{
public:

   OpPowFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpPowFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpPowFunction( p ); }

   String Meta() const override            { return "op_pow()"; }
   String Id() const override              { return "<p>Represents a pixel wise arithmetic exponentiation operation for the combine() generator.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_pow"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpDifFunction : public Function
{
public:

   OpDifFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpDifFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpDifFunction( p ); }

   String Meta() const override            { return "op_dif()"; }
   String Id() const override              { return "<p>Represents a pixel wise absolute difference operation for the combine() generator.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_dif"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpMinFunction : public Function
{
public:

   OpMinFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpMinFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpMinFunction( p ); }

   String Meta() const override            { return "op_min()"; }
   String Id() const override              { return "<p>Represents a pixel wise minimum operation for the combine() generator.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_min"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpMaxFunction : public Function
{
public:

   OpMaxFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpMaxFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpMaxFunction( p ); }

   String Meta() const override            { return "op_max()"; }
   String Id() const override              { return "<p>Represents a pixel wise maximum operation for the combine() generator.</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_max"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpColorBurnFunction : public Function
{
public:

   OpColorBurnFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpColorBurnFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpColorBurnFunction( p ); }

   String Meta() const override            { return "op_color_burn()"; }
   String Id() const override              { return "<p>Represents a pixel wise <i>color burn</i> operation for the combine() generator.</p>"
                                                    "<p>Given two images <i>a</i> and <i>b</i> with pixel sample values in the [0,1] range, "
                                                    "the color burn operation is defined by the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   1 - min( (1 - a)/b, 1 )"
                                                    "</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_color_burn"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpLinearBurnFunction : public Function
{
public:

   OpLinearBurnFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpLinearBurnFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpLinearBurnFunction( p ); }

   String Meta() const override            { return "op_linear_burn()"; }
   String Id() const override              { return "<p>Represents a pixel wise <i>linear burn</i> operation for the combine() generator.</p>"
                                                    "<p>Given two images <i>a</i> and <i>b</i> with pixel sample values in the [0,1] range, "
                                                    "the linear burn operation is defined by the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   a + b - 1"
                                                    "</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_linear_burn"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpScreenFunction : public Function
{
public:

   OpScreenFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpScreenFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpScreenFunction( p ); }

   String Meta() const override            { return "op_screen()"; }
   String Id() const override              { return "<p>Represents a pixel wise <i>screen</i> operation for the combine() generator.</p>"
                                                    "<p>Given two images <i>a</i> and <i>b</i> with pixel sample values in the [0,1] range, "
                                                    "the screen operation is defined by the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   1 - (1 - a)*(1 - b)"
                                                    "</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_screen"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpColorDodgeFunction : public Function
{
public:

   OpColorDodgeFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpColorDodgeFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpColorDodgeFunction( p ); }

   String Meta() const override            { return "op_color_dodge()"; }
   String Id() const override              { return "<p>Represents a pixel wise <i>color dodge</i> operation for the combine() generator.</p>"
                                                    "<p>Given two images <i>a</i> and <i>b</i> with pixel sample values in the [0,1] range, "
                                                    "the color dodge operation is defined by the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   1 - min( (1 - a)/b, 1 )"
                                                    "</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_color_dodge"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpOverlayFunction : public Function
{
public:

   OpOverlayFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpOverlayFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpOverlayFunction( p ); }

   String Meta() const override            { return "op_overlay()"; }
   String Id() const override              { return "<p>Represents a pixel wise <i>overlay</i> operation for the combine() generator.</p>"
                                                    "<p>Given two images <i>a</i> and <i>b</i> with pixel sample values in the [0,1] range, "
                                                    "the overlay operation is defined by the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   1 - (1 - 2*(a - 0.5)) * (1 - b)   if a &gt; 0.5<br/>"
                                                    "   2*a*b                             otherwise"
                                                    "</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_overlay"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpSoftLightFunction : public Function
{
public:

   OpSoftLightFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpSoftLightFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpSoftLightFunction( p ); }

   String Meta() const override            { return "op_soft_light()"; }
   String Id() const override              { return "<p>Represents a pixel wise <i>soft light</i> operation for the combine() generator.</p>"
                                                    "<p>Given two images <i>a</i> and <i>b</i> with pixel sample values in the [0,1] range, "
                                                    "the soft light operation is defined by the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   1 - (1 - a)*(1 - b - 0.5)   if b &gt; 0.5<br/>"
                                                    "   a*(b + 0.5)                 otherwise"
                                                    "</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_soft_light"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpHardLightFunction : public Function
{
public:

   OpHardLightFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpHardLightFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpHardLightFunction( p ); }

   String Meta() const override            { return "op_hard_light()"; }
   String Id() const override              { return "<p>Represents a pixel wise <i>hard light</i> operation for the combine() generator.</p>"
                                                    "<p>Given two images <i>a</i> and <i>b</i> with pixel sample values in the [0,1] range, "
                                                    "the hard light operation is defined by the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   1 - (1 - a)*(1 - 2*(b - 0.5))   if b &gt; 0.5<br/>"
                                                    "   2*a*b                           otherwise"
                                                    "</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_hard_light"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpVividLightFunction : public Function
{
public:

   OpVividLightFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpVividLightFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpVividLightFunction( p ); }

   String Meta() const override            { return "op_vivid_light()"; }
   String Id() const override              { return "<p>Represents a pixel wise <i>vivid light</i> operation for the combine() generator.</p>"
                                                    "<p>Given two images <i>a</i> and <i>b</i> with pixel sample values in the [0,1] range, "
                                                    "the vivid light operation is defined by the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   1 - max( (1 - a)/(b - 0.5)/2, 1 )   if b &gt; 0.5<br/>"
                                                    "   min( a/(1 - 2*b ), 1 )              otherwise"
                                                    "</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_vivid_light"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpLinearLightFunction : public Function
{
public:

   OpLinearLightFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpLinearLightFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpLinearLightFunction( p ); }

   String Meta() const override            { return "op_linear_light()"; }
   String Id() const override              { return "<p>Represents a pixel wise <i>linear light</i> operation for the combine() generator.</p>"
                                                    "<p>Given two images <i>a</i> and <i>b</i> with pixel sample values in the [0,1] range, "
                                                    "the linear light operation is defined by the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   max( a + 2*(b - 0.5), 1 )   if b &gt; 0.5<br/>"
                                                    "   max( a + 2*b - 1, 1 )       otherwise"
                                                    "</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_linear_light"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpPinLightFunction : public Function
{
public:

   OpPinLightFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpPinLightFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpPinLightFunction( p ); }

   String Meta() const override            { return "op_pin_light()"; }
   String Id() const override              { return "<p>Represents a pixel wise <i>pin light</i> operation for the combine() generator.</p>"
                                                    "<p>Given two images <i>a</i> and <i>b</i> with pixel sample values in the [0,1] range, "
                                                    "the pin light operation is defined by the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   max( a, 2*(b - 0.5) )   if b &gt; 0.5<br/>"
                                                    "   min( a, 2*b )           otherwise"
                                                    "</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_pin_light"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

class OpExclusionFunction : public Function
{
public:

   OpExclusionFunction( int p = 0, int n = 0 ) : Function( p, n ) {}

   Expression* Clone() const override { return new OpExclusionFunction( *this ); }
   Expression* Generate( int p ) const override { return new OpExclusionFunction( p ); }

   String Meta() const override            { return "op_exclusion()"; }
   String Id() const override              { return "<p>Represents a pixel wise <i>exclusion</i> operation for the combine() generator.</p>"
                                                    "<p>Given two images <i>a</i> and <i>b</i> with pixel sample values in the [0,1] range, "
                                                    "the exclusion operation is defined by the following expression:</p>"
                                                    "<p style=\"white-space: pre;\">"
                                                    "   0.5 - 2*(a - 0.5)*(b - 0.5)"
                                                    "</p>"
                                                    "<p>Invariant subexpression: always.</p>"; }
   String Token() const override           { return "op_exclusion"; }
   int RequiredArguments() const override  { return 0; }
   int MaximumArguments() const override   { return 0; }

   void operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const override;

   bool IsInvariant( component_list::const_iterator, component_list::const_iterator ) const override;
   void operator()( Pixel&, component_list::const_iterator, component_list::const_iterator ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Function_h

// ----------------------------------------------------------------------------
// EOF Function.h - Released 2021-05-05T15:38:07Z
