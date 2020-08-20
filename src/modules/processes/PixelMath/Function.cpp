//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.0
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.5.0
// ----------------------------------------------------------------------------
// Function.cpp - Released 2020-08-18T19:14:14Z
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
#include "PixelMathInstance.h"

#include <pcl/HistogramTransformation.h>
#include <pcl/ImageWindow.h>
#include <pcl/Math.h>
#include <pcl/Selection.h>

#define S   (static_cast<Sample*>( *i ))
#define P   (static_cast<PixelData*>( *i ))
#define I   (static_cast<ImageReference*>( *i ))

namespace pcl
{

// ----------------------------------------------------------------------------

String Function::ToString() const
{
   String s = Token() + '(';
   if ( !arguments.IsEmpty() )
      for ( argument_list::const_iterator i = arguments.Begin(); ; )
      {
         s << (*i)->ToString();
         if ( ++i == arguments.End() )
            break;
         s << ',';
      }
   s << ')';
   return s;
}

// ----------------------------------------------------------------------------

void Function::InitializeList( function_set& functions, function_index& index )
{
   functions.Destroy();

   functions << new AbsFunction
             << new ArcCosFunction
             << new ArcCoshFunction
             << new ArcSinFunction
             << new ArcSinhFunction
             << new ArcTanFunction
             << new ArcTanhFunction
             << new AvgDevFunction
             << new BWMVFunction
             << new ChiSquareFunction
             << new CIELFunction
             << new CIEXFunction
             << new CIEYFunction
             << new CIEZFunction
             << new CIEaFunction
             << new CIEbFunction
             << new CIEcFunction
             << new CIEhFunction
             << new CIEhdFunction
             << new CIEhrFunction
             << new CeilFunction
             << new CosFunction
             << new CoshFunction
             << new DistToLineFunction
             << new DistToSegmentFunction
             << new ExpFunction
             << new FloorFunction
             << new FracFunction
             << new GammaFunction
             << new GaussFunction
             << new HFunction
             << new HdFunction
             << new HeightFunction
             << new HrFunction
             << new IFunction
             << new InEllipseFunction
             << new InRectFunction
             << new InlineIfFunction
             << new InlineSwitchFunction
             << new IsColorFunction
             << new LnFunction
             << new Log2Function
             << new LogFunction
             << new MADFunction
             << new MTFFunction
             << new MaxDevFunction
             << new MaxDistRectFunction
             << new MaxFunction
             << new MaxSampleFunction
             << new MeanFunction
             << new MeanOfSquaresFunction
             << new MedFunction
             << new MinDistRectFunction
             << new MinFunction
             << new MinSampleFunction
             << new ModFunction
             << new NormFunction
             << new NumberOfChannelsFunction
             << new PAngleFunction
             << new PBMVFunction
             << new PiFunction
             << new PixelFunction
             << new PoissonFunction
             << new QnFunction
             << new RDistFunction
             << new RandomFunction
             << new RangeFunction
             << new RescaleFunction
             << new RndSelectFunction
             << new RoundFunction
             << new SiFunction
             << new SignFunction
             << new SinFunction
             << new SinhFunction
             << new SnFunction
             << new SqrtFunction
             << new StdDevFunction
             << new StudentTFunction
             << new SumFunction
             << new SumOfSquaresFunction
             << new SvFunction
             << new TanFunction
             << new TanhFunction
             << new TruncFunction
             << new VFunction
             << new VarFunction
             << new WidthFunction
             << new XFunction
             << new XPerpToLineFunction
             << new XPosFunction
             << new YFunction
             << new YPerpToLineFunction
             << new YPosFunction;

   index.Clear();
   for ( const Function* fn : functions )
   {
      index << IndexNode( fn->Token(), fn );
      StringList aliases;
      fn->Aliases().Break( aliases, ',', true/*trim*/ );
      aliases.Remove( String() );
      for ( const String& alias : aliases )
         index << IndexNode( alias, fn );
   }
}

// ----------------------------------------------------------------------------

void AbsFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Abs( (*i)[c] );
}

bool AbsFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void AbsFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Abs( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Abs( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void ArcCosFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ArcCos( (*i)[c] );
}

bool ArcCosFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void ArcCosFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( ArcCos( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = ArcCos( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void ArcCoshFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ArcCosh( (*i)[c] );
}

bool ArcCoshFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void ArcCoshFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( ArcCosh( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = ArcCosh( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void ArcSinFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ArcSin( (*i)[c] );
}

bool ArcSinFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void ArcSinFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( ArcSin( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = ArcSin( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void ArcSinhFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ArcSinh( (*i)[c] );
}

bool ArcSinhFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void ArcSinhFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( ArcSinh( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = ArcSinh( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void ArcTanFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 )
   {
      j = i; ++j;
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = ArcTan( (*i)[c], (*j)[c] );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = ArcTan( (*i)[c] );
   }
}

bool ArcTanFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( !(*i)->IsSample() && !(*i)->IsPixel() )
      return false;
   if ( Distance( i, j ) == 1 )
      return true;
   ++i;
   return (*i)->IsSample() || (*i)->IsPixel();
}

void ArcTanFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   Pixel y;

   if ( (*i)->IsSample() )
      y.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      y.SetAs( P->PixelValue() );
      y.CopySamples( P->PixelValue() );
   }

   if ( Distance( i, j ) > 1 )
   {
      Pixel x;

      ++i;

      if ( (*i)->IsSample() )
         x.SetSamples( S->Value() );
      else if ( (*i)->IsPixel() )
      {
         x.SetAs( P->PixelValue() );
         x.CopySamples( P->PixelValue() );
      }

      if ( y.IsRGB() != x.IsRGB() )
         if ( !y.IsRGB() )
            y.ConvertToRGB();
         else if ( !x.IsRGB() )
            x.ConvertToRGB();

      r.SetAs( y );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = ArcTan( y[c], x[c] );
   }
   else
   {
      r.SetAs( y );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = ArcTan( y[c] );
   }
}

// ----------------------------------------------------------------------------

void ArcTanhFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ArcTanh( (*i)[c] );
}

bool ArcTanhFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void ArcTanhFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( ArcTanh( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = ArcTanh( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

bool MeanFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || (*i)->IsImageReference() )
      return true;
   info = "mean() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void MeanFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.Mean();
   }
}

bool MeanFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void MeanFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector mean = view.ComputeOrFetchProperty( "Mean", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( mean.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( mean[c] );
   }
   else
   {
      if ( mean.Length() < 3 )
         r.SetSamples( mean[0] );
      else
         r.SetSamples( mean[0], mean[1], mean[2] );
   }
}

// ----------------------------------------------------------------------------

bool AvgDevFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "adev() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void AvgDevFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.AvgDev()*1.2533;
   }
}

bool AvgDevFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void AvgDevFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector avgDev = view.ComputeOrFetchProperty( "AvgDev", false/*notify*/ ).ToDVector() * 1.2533;

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( avgDev.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( avgDev[c] );
   }
   else
   {
      if ( avgDev.Length() < 3 )
         r.SetSamples( avgDev[0] );
      else
         r.SetSamples( avgDev[0], avgDev[1], avgDev[2] );
   }
}

// ----------------------------------------------------------------------------

bool MADFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "mdev() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void MADFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.MAD()*1.4826;
   }
}

bool MADFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void MADFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector MAD = view.ComputeOrFetchProperty( "MAD", false/*notify*/ ).ToDVector() * 1.4826;

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( MAD.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( MAD[c] );
   }
   else
   {
      if ( MAD.Length() < 3 )
         r.SetSamples( MAD[0] );
      else
         r.SetSamples( MAD[0], MAD[1], MAD[2] );
   }
}

// ----------------------------------------------------------------------------

bool BWMVFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "BWMV() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void BWMVFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.BiweightMidvariance();
   }
}

bool BWMVFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void BWMVFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector BWMV = view.ComputeOrFetchProperty( "BWMV", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( BWMV.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( BWMV[c] );
   }
   else
   {
      if ( BWMV.Length() < 3 )
         r.SetSamples( BWMV[0] );
      else
         r.SetSamples( BWMV[0], BWMV[1], BWMV[2] );
   }
}

// ----------------------------------------------------------------------------

bool PBMVFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "PBMV() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void PBMVFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.BendMidvariance();
   }
}

bool PBMVFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void PBMVFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector PBMV = view.ComputeOrFetchProperty( "PBMV", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( PBMV.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( PBMV[c] );
   }
   else
   {
      if ( PBMV.Length() < 3 )
         r.SetSamples( PBMV[0] );
      else
         r.SetSamples( PBMV[0], PBMV[1], PBMV[2] );
   }
}

// ----------------------------------------------------------------------------

bool SnFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "Sn() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void SnFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.Sn()*1.1926;
   }
}

bool SnFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void SnFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector Sn = view.ComputeOrFetchProperty( "Sn", false/*notify*/ ).ToDVector() * 1.1926;

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( Sn.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( Sn[c] );
   }
   else
   {
      if ( Sn.Length() < 3 )
         r.SetSamples( Sn[0] );
      else
         r.SetSamples( Sn[0], Sn[1], Sn[2] );
   }
}

// ----------------------------------------------------------------------------

bool QnFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "Qn() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void QnFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.Qn()*2.2191;
   }
}

bool QnFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void QnFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector Qn = view.ComputeOrFetchProperty( "Qn", false/*notify*/ ).ToDVector() * 2.2191;

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( Qn.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( Qn[c] );
   }
   else
   {
      if ( Qn.Length() < 3 )
         r.SetSamples( Qn[0] );
      else
         r.SetSamples( Qn[0], Qn[1], Qn[2] );
   }
}

// ----------------------------------------------------------------------------

bool ModFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "mod() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void ModFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.Modulus();
   }
}

bool ModFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void ModFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector mod = view.ComputeOrFetchProperty( "Modulus", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( mod.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( mod[c] );
   }
   else
   {
      if ( mod.Length() < 3 )
         r.SetSamples( mod[0] );
      else
         r.SetSamples( mod[0], mod[1], mod[2] );
   }
}

// ----------------------------------------------------------------------------

bool NormFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "norm() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void NormFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.Sum();
   }
}

bool NormFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void NormFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector norm = view.ComputeOrFetchProperty( "Norm", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( norm.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( norm[c] );
   }
   else
   {
      if ( norm.Length() < 3 )
         r.SetSamples( norm[0] );
      else
         r.SetSamples( norm[0], norm[1], norm[2] );
   }
}

// ----------------------------------------------------------------------------

bool SumOfSquaresFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "ssqr() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void SumOfSquaresFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.SumOfSquares();
   }
}

bool SumOfSquaresFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void SumOfSquaresFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector s2 = view.ComputeOrFetchProperty( "SumOfSquares", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( s2.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( s2[c] );
   }
   else
   {
      if ( s2.Length() < 3 )
         r.SetSamples( s2[0] );
      else
         r.SetSamples( s2[0], s2[1], s2[2] );
   }
}

// ----------------------------------------------------------------------------

bool MeanOfSquaresFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "asqr() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void MeanOfSquaresFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.SumOfSquares()/v.Length();
   }
}

bool MeanOfSquaresFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void MeanOfSquaresFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector m2 = view.ComputeOrFetchProperty( "MeanOfSquares", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( m2.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( m2[c] );
   }
   else
   {
      if ( m2.Length() < 3 )
         r.SetSamples( m2[0] );
      else
         r.SetSamples( m2[0], m2[1], m2[2] );
   }
}

// ----------------------------------------------------------------------------

bool CIELFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "CIEL() takes a single image or pixel reference argument";
   return false;
}

void CIELFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->CIEL( (*i)[0], (*i)[1], (*i)[2] ) :
                              PixelMathInstance::s_targetRGBWS->CIEL( (*i)[0], (*i)[0], (*i)[0] ) );
}

// ----------------------------------------------------------------------------

bool CIEaFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "CIEa() takes a single image or pixel reference argument";
   return false;
}

void CIEaFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->CIEa( (*i)[0], (*i)[1], (*i)[2] ) :
                              PixelMathInstance::s_targetRGBWS->CIEa( (*i)[0], (*i)[0], (*i)[0] ) );
}

// ----------------------------------------------------------------------------

bool CIEbFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "CIEb() takes a single image or pixel reference argument";
   return false;
}

void CIEbFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->CIEb( (*i)[0], (*i)[1], (*i)[2] ) :
                              PixelMathInstance::s_targetRGBWS->CIEb( (*i)[0], (*i)[0], (*i)[0] ) );
}

// ----------------------------------------------------------------------------

bool CIEcFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "CIEc() takes a single image or pixel reference argument";
   return false;
}

void CIEcFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->CIEc( (*i)[0], (*i)[1], (*i)[2] ) :
                              PixelMathInstance::s_targetRGBWS->CIEc( (*i)[0], (*i)[0], (*i)[0] ) );
}

// ----------------------------------------------------------------------------

bool CIEhFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "CIEh() takes a single image or pixel reference argument";
   return false;
}

void CIEhFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->CIEh( (*i)[0], (*i)[1], (*i)[2] ) : 0.0 );
}

// ----------------------------------------------------------------------------

bool CIEhrFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "CIEhr() takes a single image or pixel reference argument";
   return false;
}

void CIEhrFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->CIEhr( (*i)[0], (*i)[1], (*i)[2] ) : 0.0 );
}

// ----------------------------------------------------------------------------

bool CIEhdFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "CIEhd() takes a single image or pixel reference argument";
   return false;
}

void CIEhdFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? Deg( PixelMathInstance::s_targetRGBWS->CIEhr( (*i)[0], (*i)[1], (*i)[2] ) ) : 0.0 );
}

// ----------------------------------------------------------------------------

bool CIEXFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "CIEX() takes a single image or pixel reference argument";
   return false;
}

void CIEXFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->CIEX( (*i)[0], (*i)[1], (*i)[2] ) :
                              PixelMathInstance::s_targetRGBWS->CIEX( (*i)[0], (*i)[0], (*i)[0] ) );
}

// ----------------------------------------------------------------------------

bool CIEYFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "CIEY() takes a single image or pixel reference argument";
   return false;
}

void CIEYFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->CIEY( (*i)[0], (*i)[1], (*i)[2] ) :
                              PixelMathInstance::s_targetRGBWS->CIEY( (*i)[0], (*i)[0], (*i)[0] ) );
}

// ----------------------------------------------------------------------------

bool CIEZFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "CIEZ() takes a single image or pixel reference argument";
   return false;
}

void CIEZFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->CIEZ( (*i)[0], (*i)[1], (*i)[2] ) :
                              PixelMathInstance::s_targetRGBWS->CIEZ( (*i)[0], (*i)[0], (*i)[0] ) );
}

// ----------------------------------------------------------------------------

void CeilFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Ceil( (*i)[c] );
}

bool CeilFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void CeilFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Ceil( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Ceil( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void CosFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Cos( (*i)[c] );
}

bool CosFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void CosFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Cos( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Cos( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void CoshFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Cosh( (*i)[c] );
}

bool CoshFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void CoshFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Cosh( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Cosh( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void ExpFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Exp( (*i)[c] );
}

bool ExpFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void ExpFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Exp( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Exp( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void FloorFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Floor( (*i)[c] );
}

bool FloorFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void FloorFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Floor( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Floor( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void FracFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Frac( (*i)[c] );
}

bool FracFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void FracFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Frac( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Frac( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

bool HFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "H() takes a single image or pixel reference argument";
   return false;
}

void HFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->Hue( (*i)[0], (*i)[1], (*i)[2] ) : 0.0 );
}

// ----------------------------------------------------------------------------

bool HrFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "Hr() takes a single image or pixel reference argument";
   return false;
}

void HrFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? Const<double>::_2pi()*PixelMathInstance::s_targetRGBWS->Hue( (*i)[0], (*i)[1], (*i)[2] ) : 0.0 );
}

// ----------------------------------------------------------------------------

bool HdFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "Hd() takes a single image or pixel reference argument";
   return false;
}

void HdFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? 360*PixelMathInstance::s_targetRGBWS->Hue( (*i)[0], (*i)[1], (*i)[2] ) : 0.0 );
}

// ----------------------------------------------------------------------------

bool SvFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "Sv() takes a single image or pixel reference argument";
   return false;
}

void SvFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->HSVSaturation( (*i)[0], (*i)[1], (*i)[2] ) : 0.0 );
}

// ----------------------------------------------------------------------------

bool SiFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "Si() takes a single image or pixel reference argument";
   return false;
}

void SiFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->HSISaturation( (*i)[0], (*i)[1], (*i)[2] ) : 0.0 );
}

// ----------------------------------------------------------------------------

bool VFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "I() takes a single image or pixel reference argument";
   return false;
}

void VFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->Value( (*i)[0], (*i)[1], (*i)[2] ) : (*i)[0] );
}

// ----------------------------------------------------------------------------

bool IFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "I() takes a single image or pixel reference argument";
   return false;
}

void IFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( i->IsRGB() ? PixelMathInstance::s_targetRGBWS->Intensity( (*i)[0], (*i)[1], (*i)[2] ) :
                              PixelMathInstance::s_targetRGBWS->Intensity( (*i)[0], (*i)[0], (*i)[0] ) );
}

// ----------------------------------------------------------------------------

void LnFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Ln( (*i)[c] );
}

bool LnFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void LnFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Ln( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Ln( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void LogFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Log( (*i)[c] );
}

bool LogFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void LogFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Log( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Log( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void Log2Function::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Log2( (*i)[c] );
}

bool Log2Function::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void Log2Function::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Log2( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Log2( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

bool MaxFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || (*i)->IsImageReference() )
      return true;
   info = "max() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void MaxFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   for ( int c = 0; c < r.Length(); ++c )
   {
      double m = (*i)[c];
      for ( pixel_set::const_iterator k = i; ++k < j; )
      {
         double t = (*k)[c];
         if ( m < t )
            m = t;
      }
      r[c] = m;
   }
}

bool MaxFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void MaxFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector maximum = view.ComputeOrFetchProperty( "Maximum", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( maximum.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( maximum[c] );
   }
   else
   {
      if ( maximum.Length() < 3 )
         r.SetSamples( maximum[0] );
      else
         r.SetSamples( maximum[0], maximum[1], maximum[2] );
   }
}

// ----------------------------------------------------------------------------

void MaxDevFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   distance_type n = Distance( i, j );
   distance_type n2 = n >> 1;
   DVector v( n );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      v.Sort();
      double m = (n & 1) ? v[n2] : (v[n2] + v[n2-1])/2;
      r[c] = Max( m - v[0], v[n-1] - m );
   }
}

// ----------------------------------------------------------------------------

bool MaxSampleFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "maxsample() takes a single image or pixel reference argument";
   return false;
}

void MaxSampleFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( (i->Length() == 1) ? (*i)[0] : Max( Max( (*i)[0], (*i)[1] ), (*i)[2] ) );
}

// ----------------------------------------------------------------------------

bool MedFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 2 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "med() takes either a single image reference argument or a set of three or more arguments";
   return false;
}

void MedFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.Median();
   }
}

bool MedFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void MedFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector median = view.ComputeOrFetchProperty( "Median", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( median.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( median[c] );
   }
   else
   {
      if ( median.Length() < 3 )
         r.SetSamples( median[0] );
      else
         r.SetSamples( median[0], median[1], median[2] );
   }
}

// ----------------------------------------------------------------------------

bool MinFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || (*i)->IsImageReference() )
      return true;
   info = "min() takes either a single image reference argument or a set of two or more arguments";
   return false;
}

void MinFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   for ( int c = 0; c < r.Length(); ++c )
   {
      double m = (*i)[c];
      for ( pixel_set::const_iterator k = i; ++k < j; )
      {
         double t = (*k)[c];
         if ( t < m )
            m = t;
      }
      r[c] = m;
   }
}

bool MinFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void MinFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector minimum = view.ComputeOrFetchProperty( "Minimum", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( minimum.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( minimum[c] );
   }
   else
   {
      if ( minimum.Length() < 3 )
         r.SetSamples( minimum[0] );
      else
         r.SetSamples( minimum[0], minimum[1], minimum[2] );
   }
}

// ----------------------------------------------------------------------------

bool MinSampleFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "minsample() takes a single image or pixel reference argument";
   return false;
}

void MinSampleFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   r.SetSamples( (i->Length() == 1) ? (*i)[0] : Min( Min( (*i)[0], (*i)[1] ), (*i)[2] ) );
}

// ----------------------------------------------------------------------------

void MTFFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   j = i; ++j;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = HistogramTransformation::MTF( (*i)[c], (*j)[c] );
}

bool MTFFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   j = i; ++j;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel());
}

void MTFFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   Pixel m, x;

   if ( (*i)->IsSample() )
      m.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      m.SetAs( P->PixelValue() );
      m.CopySamples( P->PixelValue() );
   }

   ++i;

   if ( (*i)->IsSample() )
      x.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      x.SetAs( P->PixelValue() );
      x.CopySamples( P->PixelValue() );
   }

   if ( m.IsRGB() != x.IsRGB() )
      if ( !m.IsRGB() )
         m.ConvertToRGB();
      else if ( !x.IsRGB() )
         x.ConvertToRGB();

   r.SetAs( x );
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = HistogramTransformation::MTF( m[c], x[c] );
}

// ----------------------------------------------------------------------------

void PiFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "pi(): Internal parser error" );
}

bool PiFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void PiFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( Const<double>::pi() );
}

// ----------------------------------------------------------------------------

void RandomFunction::operator()( Pixel& r, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = r.TLS()->random();
}

// ----------------------------------------------------------------------------

static double Normal( const Pixel& r )
{
   // Use the Box-Muller algorithm
   return Sqrt( -2*Ln( r.TLS()->random() ) ) * Sin( 2*Const<double>::pi()*r.TLS()->random() );
}

void GaussFunction::operator()( Pixel& r, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Normal( r );
}

// ----------------------------------------------------------------------------

/*
 * Returns a random Poisson deviate for a given pixel value.
 */
static int PoissonRandomDeviate( const Pixel& r, double value )
{
   if ( value < 30 )
   {
      /*
       * Implementation of the algorithm by Donald E. Knuth, 1969.
       *
       * Slow (unusable) for large values.
       */
      double p = 1, L = Exp( -value );
      int k = 0;
      do
      {
         ++k;
         p *= r.TLS()->random();
      }
      while ( p > L );
      return k-1;
   }
   else
   {
      /*
       * Code adapted from 'Random number generation in C++', by John D. Cook:
       *    https://www.johndcook.com/blog/cpp_random_number_generation/
       *
       * The algorithm is from "The Computer Generation of Poisson Random
       * Variables" by A. C. Atkinson, Journal of the Royal Statistical Society
       * Series C (Applied Statistics) Vol. 28, No. 1. (1979)
       *
       * Slow (unusable) for small values.
       */
      double c = 0.767 - 3.36/value;
      double beta = Const<double>::pi()/Sqrt( 3*value );
      double alpha = beta*value;
      double k = Ln( c ) - value - Ln( beta );
      for ( ;; )
      {
         double u = r.TLS()->random();
         double x = (alpha - Ln( (1 - u)/u ))/beta;
         int n = int( Floor( x + 0.5 ) );
         if ( n < 0 )
            continue;
         double v = r.TLS()->random();
         double y = alpha - beta*x;
         double temp = 1.0 + Exp( y );
         double lhs = y + Ln( v/temp/temp );
         double rhs = k + n*Ln( value ) - LnFactorial( n );
         if ( lhs <= rhs )
            return n;
      }
   }
}

bool PoissonFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) == 1 )
      return true;
   info = "poisson() takes a single argument";
   return false;
}

void PoissonFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = PoissonRandomDeviate( r, (*i)[c] );
}

// ----------------------------------------------------------------------------

static double GammaRandomDeviate( const Pixel& r, double shape, double scale )
{
   /*
    * Code adapted from 'Random number generation in C++', by John D. Cook:
    *    https://www.johndcook.com/blog/cpp_random_number_generation/
    *
    * Implementation based on "A Simple Method for Generating Gamma Variables"
    * by George Marsaglia and Wai Wan Tsang.  ACM Transactions on Mathematical
    * Software Vol 26, No 3, September 2000, pages 363-372.
    */
   if ( shape <= 0 )
      throw ParseError( "gamma() argument #1: the function shape parameter must be > 0; got " + String( shape ) + '.' );
   if ( scale <= 0 )
      throw ParseError( "gamma() argument #1: the scale parameter must be > 0; got " + String( scale ) + '.' );
   if ( shape >= 1 )
   {
      double d = shape - 1.0/3.0;
      double c = 1/Sqrt( 9*d );
      for ( ;; )
      {
         double x, v;
         do
         {
            x = Normal( r );
            v = 1 + c*x;
         }
         while ( v <= 0 );
         v = v*v*v;
         double u = r.TLS()->random();
         double xsquared = x*x;
         if ( u < 1 - 0.0331*xsquared*xsquared || Ln( u ) < 0.5*xsquared + d*(1 - v + Ln( v )) )
            return scale*d*v;
      }
   }
   else
   {
      double g = GammaRandomDeviate( r, shape+1, 1.0 );
      double w = r.TLS()->random();
      return scale*g*Pow( w, 1/shape );
   }
}

bool GammaFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) < 1 || Distance( i, j ) > 2 )
   {
      info = "gamma() takes one or two arguments";
      return false;
   }

   if ( (*i)->IsSample() && S->Value() <= 0 )
   {
      info = "gamma() argument #1: the function shape parameter must be > 0";
      return false;
   }

   if ( Distance( i, j ) > 1 )
   {
      ++i;
      if ( (*i)->IsSample() && S->Value() <= 0 )
      {
         info = "gamma() argument #2: the scale parameter must be > 0";
         return false;
      }
   }

   return true;
}

void GammaFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   double scale = (Distance( i, j ) > 1) ? (*(i+1))[0] : 1.0;
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = GammaRandomDeviate( r, (*i)[0], scale );
}

// ----------------------------------------------------------------------------

bool ChiSquareFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) != 1 )
   {
      info = "chisq() takes a single argument";
      return false;
   }

   if ( (*i)->IsSample() && S->Value() <= 0 )
   {
      info = "chisq(): the degrees of freedom parameter must be > 0";
      return false;
   }

   return true;
}

void ChiSquareFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   double hnu = (*i)[0]/2;
   if ( hnu <= 0 )
      throw ParseError( "chisq(): the degrees of freedom parameter must be > 0; got " + String( hnu*2 ) + '.' );
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = GammaRandomDeviate( r, hnu, 2 );
}

// ----------------------------------------------------------------------------

bool StudentTFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) != 1 )
   {
      info = "studentt() takes a single argument";
      return false;
   }

   if ( (*i)->IsSample() && S->Value() <= 0 )
   {
      info = "studentt(): the degrees of freedom parameter must be > 0";
      return false;
   }

   return true;
}

void StudentTFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   double nu = (*i)[0];
   if ( nu <= 0 )
      throw ParseError( "studentt(): the degrees of freedom parameter must be > 0; got " + String( nu ) + '.' );
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Normal( r )/Sqrt( GammaRandomDeviate( r, nu/2, 2 )/nu );
}

// ----------------------------------------------------------------------------

bool RndSelectFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 )
      return true;
   info = "rndselect() takes a set of two or more arguments";
   return false;
}

void RndSelectFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   unsigned n = Distance( i, j );
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = i[r.TLS()->random.UIN( n )][c];
}

// ----------------------------------------------------------------------------

bool RoundFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 )
   {
      ++i;
      if ( !(*i)->IsSample() || Frac( S->Value() ) != 0 )
      {
         info = "round() argument #2: the number of decimal digits must be a literal of integral type";
         return false;
      }
   }
   return true;
}

void RoundFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 )
   {
      j = i; ++j;
      int n = TruncInt( (*j)[0] );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Round( (*i)[c], n );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Round( (*i)[c] );
   }
}

bool RoundFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void RoundFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   Pixel x;

   if ( (*i)->IsSample() )
      x.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      x.SetAs( P->PixelValue() );
      x.CopySamples( P->PixelValue() );
   }

   r.SetAs( x );

   if ( Distance( i, j ) > 1 )
   {
      int n = 0;

      ++i;

      if ( (*i)->IsSample() )
         n = TruncInt( S->Value() );

      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Round( x[c], n );
   }
   else
   {
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Round( x[c] );
   }
}

// ----------------------------------------------------------------------------

void SignFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Sign( (*i)[c] );
}

bool SignFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void SignFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Sign( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Sign( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void SinFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Sin( (*i)[c] );
}

bool SinFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void SinFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Sin( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Sin( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void SinhFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Sinh( (*i)[c] );
}

bool SinhFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void SinhFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Sinh( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Sinh( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void SqrtFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Sqrt( (*i)[c] );
}

bool SqrtFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void SqrtFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Sqrt( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Sqrt( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

bool StdDevFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 1 || Distance( i, j ) == 1 && (*i)->IsImageReference() )
      return true;
   info = "sdev() takes either a single image reference argument or a set of three or more arguments";
   return false;
}

void StdDevFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   DVector v( Distance( i, j ) );
   for ( int c = 0; c < r.Length(); ++c )
   {
      DVector::iterator a = v.Begin();
      for ( pixel_set::const_iterator k = i; k < j; ++k, ++a )
         *a = (*k)[c];
      r[c] = v.StdDev();
   }
}

bool StdDevFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return Distance( i, j ) == 1;
}

void StdDevFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector stdDev = view.ComputeOrFetchProperty( "StdDev", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( stdDev.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( stdDev[c] );
   }
   else
   {
      if ( stdDev.Length() < 3 )
         r.SetSamples( stdDev[0] );
      else
         r.SetSamples( stdDev[0], stdDev[1], stdDev[2] );
   }
}

// ----------------------------------------------------------------------------

void SumFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   for ( int c = 0; c < r.Length(); ++c )
   {
      r[c] = (*i)[c];
      for ( pixel_set::const_iterator k = i; ++k < j; )
         r[c] += (*k)[c];
   }
}

// ----------------------------------------------------------------------------

void TanFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Tan( (*i)[c] );
}

bool TanFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void TanFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Tan( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Tan( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void TanhFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Tanh( (*i)[c] );
}

bool TanhFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void TanhFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Tanh( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Tanh( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

void TruncFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Trunc( (*i)[c] );
}

bool TruncFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   return (*i)->IsSample() || (*i)->IsPixel();
}

void TruncFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   if ( (*i)->IsSample() )
      r.SetSamples( Trunc( S->Value() ) );
   else if ( (*i)->IsPixel() )
   {
      r.SetAs( P->PixelValue() );
      for ( int c = 0; c < r.Length(); ++c )
         r[c] = Trunc( P->PixelValue()[c] );
   }
}

// ----------------------------------------------------------------------------

bool VarFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "var() takes a single image reference argument";
   return false;
}

void VarFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "var(): Internal parser error" );
}

bool VarFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   return true;
}

void VarFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   ImageWindow window = ImageWindow::WindowById( I->Id() );
   if ( window.IsNull() )
      return;

   View view = window.MainView();
   DVector variance = view.ComputeOrFetchProperty( "Variance", false/*notify*/ ).ToDVector();

   int c = I->ChannelIndex();
   if ( c >= 0 )
   {
      if ( c >= int( variance.Length() ) )
         throw ParseError( ("Channel index out of range: " + I->Id()).AppendFormat( "[%d]", c ) );
      r.SetSamples( variance[c] );
   }
   else
   {
      if ( variance.Length() < 3 )
         r.SetSamples( variance[0] );
      else
         r.SetSamples( variance[0], variance[1], variance[2] );
   }
}

// ----------------------------------------------------------------------------

bool WidthFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) == 0 || (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "width() takes either no argument or a single image reference argument";
   return false;
}

void WidthFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "width(): Internal parser error" );
}

bool WidthFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void WidthFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) == 0 )
      r.SetSamples( PixelMathInstance::s_targetWidth );
   else
      r.SetSamples( I->Image()->Width() );
}

// ----------------------------------------------------------------------------

bool HeightFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) == 0 || (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "height() takes either no argument or a single image reference argument";
   return false;
}

void HeightFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "height(): Internal parser error" );
}

bool HeightFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void HeightFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) == 0 )
      r.SetSamples( PixelMathInstance::s_targetHeight );
   else
      r.SetSamples( I->Image()->Height() );
}

// ----------------------------------------------------------------------------

bool AreaFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) == 0 || (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "area() takes either no argument or a single image reference argument";
   return false;
}

void AreaFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "area(): Internal parser error" );
}

bool AreaFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void AreaFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) == 0 )
      r.SetSamples( double( uint64( PixelMathInstance::s_targetWidth ) * uint64( PixelMathInstance::s_targetHeight ) ) );
   else
      r.SetSamples( double( I->Image()->NumberOfPixels() ) );
}

// ----------------------------------------------------------------------------

bool NumberOfChannelsFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) == 0 || (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "n() takes either no argument or a single image reference argument";
   return false;
}

void NumberOfChannelsFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "n(): Internal parser error" );
}

bool NumberOfChannelsFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void NumberOfChannelsFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) == 0 )
      r.SetSamples( PixelMathInstance::s_targetNumberOfChannels );
   else
      r.SetSamples( I->Image()->NumberOfChannels() );
}

// ----------------------------------------------------------------------------

bool IsColorFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) == 0 || (*i)->IsImageReference() && !I->IsChannelIndex() )
      return true;
   info = "iscolor() takes either no argument or a single image reference argument";
   return false;
}

void IsColorFunction::operator()( Pixel&, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   throw Error( "iscolor(): Internal parser error" );
}

bool IsColorFunction::IsInvariant( component_list::const_iterator, component_list::const_iterator ) const
{
   return true;
}

void IsColorFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) == 0 )
      r.SetSamples( PixelMathInstance::s_targetIsColor ? 1 : 0 );
   else
      r.SetSamples( I->Image()->IsColor() ? 1 : 0 );
}

// ----------------------------------------------------------------------------

void XPosFunction::operator()( Pixel& r, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = r.X();
}

void XPosFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( r.X() );
}

// ----------------------------------------------------------------------------

void XFunction::operator()( Pixel& r, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   double x = double( r.X() )/(PixelMathInstance::s_targetWidth - 1);
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = x;
}

void XFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( double( r.X() )/(PixelMathInstance::s_targetWidth - 1) );
}

// ----------------------------------------------------------------------------

void YPosFunction::operator()( Pixel& r, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = r.Y();
}

void YPosFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( r.Y() );
}

// ----------------------------------------------------------------------------

void YFunction::operator()( Pixel& r, pixel_set::const_iterator, pixel_set::const_iterator ) const
{
   double x = double( r.Y() )/(PixelMathInstance::s_targetHeight - 1);
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = x;
}

void YFunction::operator()( Pixel& r, component_list::const_iterator, component_list::const_iterator ) const
{
   r.SetSamples( double( r.Y() )/(PixelMathInstance::s_targetHeight - 1) );
}

// ----------------------------------------------------------------------------

bool DistToLineFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) != 4 )
   {
      info = "d2line() takes four variable or immediate numeric arguments";
      return false;
   }

   for ( int n = 0; n < 4; ++n, ++i )
      if ( (*i)->IsImageReference() )
      {
         info = "d2line(): Point coordinates must be variables or literal numeric values";
         return false;
      }

   return true;
}

void DistToLineFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   pixel_set::const_iterator i1 = i ; i1++;
   pixel_set::const_iterator i2 = i1; i2++;
   pixel_set::const_iterator i3 = i2; i3++;
   DPoint P0( (*i )[0], (*i1)[0] );
   DPoint P1( (*i2)[0], (*i3)[0] );
   DPoint p( r.X(), r.Y() );
   double d;
   double dx = P1.x - P0.x;
   double dy = P1.y - P0.y;
   if ( 1 + dy == 1 )
      d = pcl::Abs( p.y - P0.y ); // distance to horizontal line
   else if ( 1 + dx == 1 )
      d = pcl::Abs( p.x - P0.x ); // distance to vertical line
   else
   {
      // Distance from p to the intersection of the line through p
      // perpendicular to the line y = mx + b.
      double m = dy/dx;
      double b = P0.y - m*P0.x;
      double a = (p.x + m*p.y - m*b)/(m*m + 1);
      double x1 = a - p.x;
      double x2 = m*a + b - p.y;
      d = Sqrt( x1*x1 + x2*x2 );
   }

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = d;
}

// ----------------------------------------------------------------------------

bool XPerpToLineFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) != 4 )
   {
      info = "xperp2line() takes four variable or immediate numeric arguments";
      return false;
   }

   for ( int n = 0; n < 4; ++n, ++i )
      if ( (*i)->IsImageReference() )
      {
         info = "xperp2line(): Point coordinates must be variables or literal numeric values";
         return false;
      }

   return true;
}

void XPerpToLineFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   pixel_set::const_iterator i1 = i ; i1++;
   pixel_set::const_iterator i2 = i1; i2++;
   pixel_set::const_iterator i3 = i2; i3++;
   DPoint P0( (*i )[0], (*i1)[0] );
   DPoint P1( (*i2)[0], (*i3)[0] );
   DPoint p( r.X(), r.Y() );
   double x;
   double dx = P1.x - P0.x;
   double dy = P1.y - P0.y;
   if ( 1 + dy == 1 )
      x = p.x; // horizontal line
   else if ( 1 + dx == 1 )
      x = P0.x; // vertical line
   else
   {
      // Intersection of the line through p perpendicular to y = mx + b.
      double m = dy/dx;
      double b = P0.y - m*P0.x;
      x = (p.x + m*p.y - m*b)/(m*m + 1);
   }

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = x;
}

// ----------------------------------------------------------------------------

bool YPerpToLineFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) != 4 )
   {
      info = "yperp2line() takes four variable or immediate numeric arguments";
      return false;
   }

   for ( int n = 0; n < 4; ++n, ++i )
      if ( (*i)->IsImageReference() )
      {
         info = "yperp2line(): Point coordinates must be variables or literal numeric values";
         return false;
      }

   return true;
}

void YPerpToLineFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   pixel_set::const_iterator i1 = i ; i1++;
   pixel_set::const_iterator i2 = i1; i2++;
   pixel_set::const_iterator i3 = i2; i3++;
   DPoint P0( (*i )[0], (*i1)[0] );
   DPoint P1( (*i2)[0], (*i3)[0] );
   DPoint p( r.X(), r.Y() );
   double y;
   double dx = P1.x - P0.x;
   double dy = P1.y - P0.y;
   if ( 1 + dy == 1 )
      y = P0.y; // horizontal line
   else if ( 1 + dx == 1 )
      y = p.y; // vertical line
   else
   {
      // Intersection of the line through p perpendicular to y = mx + b.
      double m = dy/dx;
      double b = P0.y - m*P0.x;
      y = m*(p.x + m*p.y - m*b)/(m*m + 1) + b;
   }

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = y;
}

// ----------------------------------------------------------------------------

bool DistToSegmentFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) != 4 )
   {
      info = "d2seg() takes four variable or immediate numeric arguments";
      return false;
   }

   for ( int n = 0; n < 4; ++n, ++i )
      if ( (*i)->IsImageReference() )
      {
         info = "d2seg(): Point coordinates must be variables or literal numeric values";
         return false;
      }

   return true;
}

void DistToSegmentFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   /*
    * Geometric Tools for Computer Graphics, pp. 192,193.
    */
   pixel_set::const_iterator i1 = i ; i1++;
   pixel_set::const_iterator i2 = i1; i2++;
   pixel_set::const_iterator i3 = i2; i3++;
   DPoint P0( (*i )[0], (*i1)[0] );
   DPoint P1( (*i2)[0], (*i3)[0] );
   DPoint Y( r.X(), r.Y() );
   DPoint D = P1 - P0;
   DPoint YmP0 = Y - P0;
   double t = D.Dot( YmP0 );
   double d;
   if ( t <= 0 )
      d = YmP0.Dot( YmP0 );
   else
   {
      double d2 = D.Dot( D );
      if ( t >= d2 )
      {
         Point YmP1 = Y - P1;
         d = YmP1.Dot( YmP1 );
      }
      else
         d = YmP0.Dot( YmP0 ) - t*t/d2;
   }

   d = Sqrt( d );
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = d;
}

// ----------------------------------------------------------------------------

bool InRectFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) != 4 )
   {
      info = "inrect() takes four variable or immediate numeric arguments";
      return false;
   }

   for ( int n = 0; n < 4; ++n, ++i )
   {
      if ( (*i)->IsImageReference() )
      {
         info = "inrect(): Rectangle coordinates must be variables or literal numeric values";
         return false;
      }

      if ( (*i)->IsSample() )
         if ( Frac( S->Value() ) != 0 )
         {
            info = "inrect(): Literal rectangle coordinates must be of integral type";
            return false;
         }
   }

   return true;
}

void InRectFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   int x0 = TruncInt( (*i++)[0] );
   int x  = r.X();
   if ( x >= x0 )
   {
      int y0 = TruncInt( (*i++)[0] );
      int y  = r.Y();
      if ( y >= y0 )
      {
         int x1 = x0 + Abs( TruncInt( (*i++)[0] ) );
         if ( x < x1 )
         {
            int y1 = y0 + Abs( TruncInt( (*i)[0] ) );
            if ( y < y1 )
            {
               for ( int c = 0; c < r.Length(); ++c )
                  r[c] = 1;
               return;
            }
         }
      }
   }
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = 0;
}

// ----------------------------------------------------------------------------

bool MinDistRectFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) != 4 )
   {
      info = "mind2rect() takes four immediate or variable numeric arguments";
      return false;
   }

   for ( int n = 0; n < 4; ++n, ++i )
      if ( (*i)->IsImageReference() )
      {
         info = "mind2rect(): Rectangle coordinates must be variables or literal numeric values";
         return false;
      }

   return true;
}

void MinDistRectFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   int x0 = TruncInt( (*i++)[0] );
   int x  = r.X();
   if ( x >= x0 )
   {
      int y0 = TruncInt( (*i++)[0] );
      int y  = r.Y();
      if ( y >= y0 )
      {
         int x1 = x0 + Abs( TruncInt( (*i++)[0] ) );
         if ( x < x1 )
         {
            int y1 = y0 + Abs( TruncInt( (*i)[0] ) );
            if ( y < y1 )
            {
               double d  = Min( Min( x-x0, y-y0 ), Min( x1-x-1, y1-y-1 ) );
               for ( int c = 0; c < r.Length(); ++c )
                  r[c] = d;
               return;
            }
         }
      }
   }
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = -1;
}

// ----------------------------------------------------------------------------

bool MaxDistRectFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) != 4 )
   {
      info = "maxd2rect() takes four immediate or variable numeric arguments";
      return false;
   }

   for ( int n = 0; n < 4; ++n, ++i )
      if ( (*i)->IsImageReference() )
      {
         info = "maxd2rect(): Rectangle coordinates must be variables or literal numeric values";
         return false;
      }

   return true;
}

void MaxDistRectFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   int x0 = TruncInt( (*i++)[0] );
   int x  = r.X();
   if ( x >= x0 )
   {
      int y0 = TruncInt( (*i++)[0] );
      int y  = r.Y();
      if ( y >= y0 )
      {
         int x1 = x0 + Abs( TruncInt( (*i++)[0] ) );
         if ( x < x1 )
         {
            int y1 = y0 + Abs( TruncInt( (*i)[0] ) );
            if ( y < y1 )
            {
               double d  = Max( Max( x-x0, y-y0 ), Max( x1-x, y1-y ) );
               for ( int c = 0; c < r.Length(); ++c )
                  r[c] = d;
               return;
            }
         }
      }
   }
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = -1;
}

// ----------------------------------------------------------------------------

bool InEllipseFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) != 4 )
   {
      info = "inellipse() takes four immediate or variable numeric arguments";
      return false;
   }

   for ( int n = 0; n < 4; ++n, ++i )
      if ( (*i)->IsImageReference() )
      {
         info = "inellipse(): Ellipse parameters must be variables or literal numeric values";
         return false;
      }

   return true;
}

void InEllipseFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   double xc  = (*i++)[0];
   double yc  = (*i++)[0];
   double rx  = (*i++)[0];
   double rx2 = rx*rx;
   double dx  = r.X() - xc;
   double dx2 = dx*dx;
   if ( dx2 <= rx2 )
   {
      double ry  = (*i  )[0];
      double ry2 = ry*ry;
      double dy  = r.Y() - yc;
      double dy2 = dy*dy;
      if ( dy2 <= ry2 )
         if ( dx2/rx2 + dy2/ry2 <= 1 )
         {
            for ( int c = 0; c < r.Length(); ++c )
               r[c] = 1;
            return;
         }
   }
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = 0;
}

// ----------------------------------------------------------------------------

bool RDistFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 0 )
   {
      if ( Distance( i, j ) != 2 )
      {
         info = "rdist() takes either no argument or two center coordinates";
         return false;
      }

      j = i; ++j;
      if ( (*i)->IsImageReference() || (*j)->IsImageReference() )
      {
         info = "rdist(): The coordinates of the center point must be variables or literal numeric values";
         return false;
      }
   }
   return true;
}

void RDistFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   double xc, yc;

   if ( Distance( i, j ) > 0 )
   {
      xc = (*i)[0];
      yc = (*++i)[0];
   }
   else
   {
      xc = 0.5*PixelMathInstance::s_targetWidth;
      yc = 0.5*PixelMathInstance::s_targetHeight;
   }

   double dx = r.X() - xc;
   double dy = r.Y() - yc;
   double rd = Sqrt( dx*dx + dy*dy );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = rd;
}

// ----------------------------------------------------------------------------

bool PAngleFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) > 0 )
   {
      if ( Distance( i, j ) != 2 )
      {
         info = "pangle() takes either no argument or two center coordinates";
         return false;
      }

      j = i; ++j;
      if ( (*i)->IsImageReference() || (*j)->IsImageReference() )
      {
         info = "pangle(): The coordinates of the center point must be variables or literal numeric values";
         return false;
      }
   }
   return true;
}

void PAngleFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   double xc, yc;

   if ( Distance( i, j ) > 0 )
   {
      xc = (*i)[0];
      yc = (*++i)[0];
   }
   else
   {
      xc = 0.5*PixelMathInstance::s_targetWidth;
      yc = 0.5*PixelMathInstance::s_targetHeight;
   }

   double theta = ArcTan( r.Y() - yc, r.X() - xc );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = theta;
}

// ----------------------------------------------------------------------------

bool PixelFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( !(*i)->IsImageReference() )
   {
      info = "pixel() argument #1: Must be an image reference";
      return false;
   }

   if ( I->IsChannelIndex() )
   {
      info = "pixel() argument #1: Invalid image channel index";
      return false;
   }

   I->SetByReference();

   component_list::const_iterator x = i; ++x;
   component_list::const_iterator y = x; ++y;

   if ( (*x)->IsImageReference() || (*y)->IsImageReference() )
   {
      info = "pixel() arguments #2,3: Pixel coordinates must be either immediate integrals, variable references, or expressions";
      return false;
   }

   if ( (*x)->IsSample() )
   {
      i = x;
      if ( Frac( S->Value() ) != 0 )
      {
         info = "pixel() argument #2: Immediate X-coordinates must be of integral type";
         return false;
      }
   }

   if ( (*y)->IsSample() )
   {
      i = y;
      if ( Frac( S->Value() ) != 0 )
      {
         info = "pixel() argument #3: Immediate Y-coordinates must be of integral type";
         return false;
      }
   }

   if ( Distance( i, x ) > 3 )
   {
      component_list::const_iterator c = y; ++c;

      if ( (*c)->IsImageReference() )
      {
         info = "pixel() argument #4: Channel indexes must be either immediate integrals, variable references, or expressions";
         return false;
      }

      if ( (*c)->IsSample() )
      {
         i = c;
         if ( Frac( S->Value() ) != 0 )
         {
            info = "pixel() argument #4: Immediate channel indexes must be of integral type";
            return false;
         }

         if ( S->Value() < 0 )
         {
            info = "pixel() argument #4: Immediate channel indexes must be integers greater than zero";
            return false;
         }
      }
   }

   return true;
}

void PixelFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   const ImageVariant& image = *static_cast<ImageReference*>( i->Reference() )->Image();

   int x = TruncInt( (*++i)[0] );
   int y = TruncInt( (*++i)[0] );

   int c;
   if ( Distance( i, j ) > 1 ) // i has been incremented two times
   {
      c = TruncInt( (*++i)[0] );
      if ( c < 0 || c >= image->NumberOfChannels() )
         throw ParseError( "pixel(): Channel index out of range: " + String( c ) );
   }
   else
      c = -1;

   if ( image->Includes( x, y ) )
   {
      if ( c < 0 && !image->IsColor() )
         c = 0;
      if ( c < 0 )
         r.SetSamples( image( x, y, 0 ), image( x, y, 1 ), image( x, y, 2 ) );
      else
         r.SetSamples( image( x, y, c ) );
   }
   else
      r.SetSamples( 0 );
}

bool PixelFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   component_list::const_iterator x = i; ++x;
   component_list::const_iterator y = x; ++y;

   if ( !(*x)->IsSample() || !(*y)->IsSample() )
      return false;

   if ( Distance( i, j ) == 3 )
      return true;

   component_list::const_iterator c = y; ++c;
   return (*c)->IsSample();
}

void PixelFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   const ImageVariant& image = *I->Image();

   ++i;
   int x = TruncInt( S->Value() );
   ++i;
   int y = TruncInt( S->Value() );

   int c;
   if ( Distance( i, j ) > 1 ) // i has been incremented two times
   {
      ++i;
      c = TruncInt( S->Value() );
      if ( c < 0 || c >= image->NumberOfChannels() )
         throw ParseError( "pixel(): Channel index out of range: " + String( c ) );
   }
   else
      c = -1;

   if ( image->Includes( x, y ) )
   {
      if ( c < 0 && !image->IsColor() )
         c = 0;
      if ( c < 0 )
         r.SetSamples( image( x, y, 0 ), image( x, y, 1 ), image( x, y, 2 ) );
      else
         r.SetSamples( image( x, y, c ) );
   }
   else
      r.SetSamples( 0 );
}

// ----------------------------------------------------------------------------

void RangeFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   pixel_set::const_iterator j = i; ++j;
   pixel_set::const_iterator k = j; ++k;

   if ( !r.IsRGB() )
      if ( i->IsRGB() )
         r.SetAs( *i );
      else if ( j->IsRGB() )
         r.SetAs( *j );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Range( (*i)[c], (*j)[c], (*k)[c] );
}

bool RangeFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   component_list::const_iterator j = i; ++j;
   component_list::const_iterator k = j; ++k;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel()) && ((*k)->IsSample() || (*k)->IsPixel());
}

void RangeFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   Pixel x, a, b;

   if ( (*i)->IsSample() )
      x.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      x.SetAs( P->PixelValue() );
      x.CopySamples( P->PixelValue() );
   }

   ++i;

   if ( (*i)->IsSample() )
      a.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      a.SetAs( P->PixelValue() );
      a.CopySamples( P->PixelValue() );
   }

   ++i;

   if ( (*i)->IsSample() )
      b.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      b.SetAs( P->PixelValue() );
      b.CopySamples( P->PixelValue() );
   }

   if ( x.IsRGB() || a.IsRGB() || b.IsRGB() )
   {
      if ( !x.IsRGB() )
         x.ConvertToRGB();
      if ( !a.IsRGB() )
         a.ConvertToRGB();
      if ( !b.IsRGB() )
         b.ConvertToRGB();
   }

   r.SetAs( x );
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = Range( x[c], a[c], b[c] );
}

// ----------------------------------------------------------------------------

void RescaleFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   pixel_set::const_iterator j = i; ++j;
   pixel_set::const_iterator k = j; ++k;

   if ( !r.IsRGB() )
      if ( i->IsRGB() )
         r.SetAs( *i );
      else if ( j->IsRGB() )
         r.SetAs( *j );

   for ( int c = 0; c < r.Length(); ++c )
   {
      double x = (*i)[c];
      double a = (*j)[c];
      double b = (*k)[c];
      if ( b < a )
         Swap( a, b );
      double d = b - a;
      r[c] = (x < a || d <= std::numeric_limits<double>::epsilon()) ? 0.0 : ((x > b) ? 1.0 : (x - a)/d);
   }
}

bool RescaleFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   component_list::const_iterator j = i; ++j;
   component_list::const_iterator k = j; ++k;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel()) && ((*k)->IsSample() || (*k)->IsPixel());
}

void RescaleFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   Pixel x, a, b;

   if ( (*i)->IsSample() )
      x.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      x.SetAs( P->PixelValue() );
      x.CopySamples( P->PixelValue() );
   }

   ++i;

   if ( (*i)->IsSample() )
      a.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      a.SetAs( P->PixelValue() );
      a.CopySamples( P->PixelValue() );
   }

   ++i;

   if ( (*i)->IsSample() )
      b.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      b.SetAs( P->PixelValue() );
      b.CopySamples( P->PixelValue() );
   }

   if ( x.IsRGB() || a.IsRGB() || b.IsRGB() )
   {
      if ( !x.IsRGB() )
         x.ConvertToRGB();
      if ( !a.IsRGB() )
         a.ConvertToRGB();
      if ( !b.IsRGB() )
         b.ConvertToRGB();
   }

   r.SetAs( x );
   for ( int c = 0; c < r.Length(); ++c )
   {
      double xc = x[c];
      double ac = a[c];
      double bc = b[c];
      if ( bc < ac )
         Swap( ac, bc );
      double d = bc - ac;
      r[c] = (xc < ac || d <= std::numeric_limits<double>::epsilon()) ? 0.0 : ((xc > bc) ? 1.0 : (xc - ac)/d);
   }
}

// ----------------------------------------------------------------------------

void InlineIfFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator ) const
{
   pixel_set::const_iterator j = i; ++j;
   pixel_set::const_iterator k = j; ++k;

   if ( !r.IsRGB() )
      if ( i->IsRGB() )
         r.SetAs( *i );
      else if ( j->IsRGB() )
         r.SetAs( *j );

   for ( int c = 0; c < r.Length(); ++c )
      r[c] = ((*i)[c] != 0) ? (*j)[c] : (*k)[c];
}

bool InlineIfFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator ) const
{
   component_list::const_iterator j = i; ++j;
   component_list::const_iterator k = j; ++k;
   return ((*i)->IsSample() || (*i)->IsPixel()) && ((*j)->IsSample() || (*j)->IsPixel()) && ((*k)->IsSample() || (*k)->IsPixel());
}

void InlineIfFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator ) const
{
   Pixel x, a, b;

   if ( (*i)->IsSample() )
      x.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      x.SetAs( P->PixelValue() );
      x.CopySamples( P->PixelValue() );
   }

   ++i;

   if ( (*i)->IsSample() )
      a.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      a.SetAs( P->PixelValue() );
      a.CopySamples( P->PixelValue() );
   }

   ++i;

   if ( (*i)->IsSample() )
      b.SetSamples( S->Value() );
   else if ( (*i)->IsPixel() )
   {
      b.SetAs( P->PixelValue() );
      b.CopySamples( P->PixelValue() );
   }

   if ( x.IsRGB() || a.IsRGB() || b.IsRGB() )
   {
      if ( !x.IsRGB() )
         x.ConvertToRGB();
      if ( !a.IsRGB() )
         a.ConvertToRGB();
      if ( !b.IsRGB() )
         b.ConvertToRGB();
   }

   r.SetAs( x );
   for ( int c = 0; c < r.Length(); ++c )
      r[c] = (x[c] != 0) ? a[c] : b[c];
}

Expression::component_list InlineIfFunction::Optimized() const
{
   unsigned a = Pointer::NextId();
   unsigned b = Pointer::NextId();
   return component_list()
            << arguments[0]->Clone()
            << new BranchOnZeroAndPop( a, TokenPosition() )
            << arguments[1]->Clone()
            << new UnconditionalBranch( b, TokenPosition() )
            << new Pointer( a )
            << arguments[2]->Clone()
            << new Pointer( b );
}

// ----------------------------------------------------------------------------

bool InlineSwitchFunction::ValidateArguments( String& info, component_list::const_iterator i, component_list::const_iterator j ) const
{
   if ( Distance( i, j ) < 3 || (Distance( i, j ) & 1) == 0 )
   {
      info = "iswitch() takes an odd number n >= 3 of arguments";
      return false;
   }
   return true;
}

void InlineSwitchFunction::operator()( Pixel& r, pixel_set::const_iterator i, pixel_set::const_iterator j ) const
{
   if ( !r.IsRGB() )
      for ( pixel_set::const_iterator k = i; k < j; ++k )
         if ( k->IsRGB() )
         {
            r.SetAs( *k );
            break;
         }

   for ( int c = 0; c < r.Length(); ++c )
      for ( ;; i += 2 )
      {
         pixel_set::const_iterator k = i;
         if ( ++k == j )
         {
            r[c] = (*i)[c];
            break;
         }
         if ( (*i)[c] != 0 )
         {
            r[c] = (*k)[c];
            break;
         }
      }
}

bool InlineSwitchFunction::IsInvariant( component_list::const_iterator i, component_list::const_iterator j ) const
{
   for ( ; i < j; ++i )
      if ( !(*i)->IsSample() )
         if ( !(*i)->IsPixel() )
            return false;
   return true;
}

void InlineSwitchFunction::operator()( Pixel& r, component_list::const_iterator i, component_list::const_iterator j ) const
{
   pixel_set X( Distance( i, j ) );
   for ( pixel_set::iterator x = X.Begin(); i < j; ++i, ++x )
      if ( (*i)->IsSample() )
         x->SetSamples( S->Value() );
      else if ( (*i)->IsPixel() )
      {
         x->SetAs( P->PixelValue() );
         x->CopySamples( P->PixelValue() );
      }

   for ( const Pixel& x : X )
      if ( x.IsRGB() )
      {
         for ( Pixel& x : X )
            if ( !x.IsRGB() )
               x.ConvertToRGB();
         break;
      }

   r.SetAs( X[0] );
   for ( int c = 0; c < r.Length(); ++c )
      for ( pixel_set::const_iterator x = X.Begin();; x += 2 )
      {
         pixel_set::const_iterator y = x;
         if ( ++y == X.End() )
         {
            r[c] = (*x)[c];
            break;
         }
         if ( (*x)[c] != 0 )
         {
            r[c] = (*y)[c];
            break;
         }
      }
}

Expression::component_list InlineSwitchFunction::Optimized() const
{
   Array<unsigned> switchTargets;
   size_type n2 = arguments.Length() >> 1;
   for ( size_type i = 0; i < n2; ++i )
      switchTargets << Pointer::NextId();
   unsigned lastTarget = Pointer::NextId();

   component_list opt;
   for ( size_type i = 0, j = 0; i < n2; ++i, j += 2 )
      opt << arguments[j]->Clone()
          << new BranchOnZeroAndPop( switchTargets[i], TokenPosition() )
          << arguments[j+1]->Clone()
          << new UnconditionalBranch( lastTarget, TokenPosition() )
          << new Pointer( switchTargets[i] );
   return opt << (*arguments.ReverseBegin())->Clone()
              << new Pointer( lastTarget );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Function.cpp - Released 2020-08-18T19:14:14Z
