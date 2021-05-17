//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// PixelMathProcess.cpp - Released 2021-05-05T15:38:07Z
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

#include "ImageCache.h"
#include "PixelMathInstance.h"
#include "PixelMathInterface.h"
#include "PixelMathParameters.h"
#include "PixelMathProcess.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PixelMathProcess* ThePixelMathProcess = nullptr;

// ----------------------------------------------------------------------------

PixelMathProcess::PixelMathProcess()
{
   ThePixelMathProcess = this;

   new PMExpression0( this );
   new PMExpression1( this );
   new PMExpression2( this );
   new PMExpression3( this );
   new PMUseSingleExpression( this );
   new PMSymbols( this );
   new PMClearImageCacheAndExit( this );
   new PMCacheGeneratedImages( this );
   new PMGenerateOutput( this );
   new PMSingleThreaded( this );
   new PMOptimization( this );
   new PMUse64BitWorkingImage( this );
   new PMRescaleResult( this );
   new PMRescaleLowerBound( this );
   new PMRescaleUpperBound( this );
   new PMTruncateResult( this );
   new PMTruncateLowerBound( this );
   new PMTruncateUpperBound( this );
   new PMCreateNewImage( this );
   new PMShowNewImage( this );
   new PMNewImageIdentifier( this );
   new PMNewImageWidth( this );
   new PMNewImageHeight( this );
   new PMNewImageAlpha( this );
   new PMNewImageColorSpace( this );
   new PMNewImageSampleFormat( this );
   new PMOutputData( this );
   new PMOutputGlobalVariableId( ThePMOutputDataParameter );
   new PMOutputGlobalVariableRK( ThePMOutputDataParameter );
   new PMOutputGlobalVariableG( ThePMOutputDataParameter );
   new PMOutputGlobalVariableB( ThePMOutputDataParameter );

   new pcl::ImageCache;
}

// ----------------------------------------------------------------------------

IsoString PixelMathProcess::Id() const
{
   return "PixelMath";
}

// ----------------------------------------------------------------------------

IsoString PixelMathProcess::Category() const
{
   return "PixelMath";
}

// ----------------------------------------------------------------------------

uint32 PixelMathProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String PixelMathProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String PixelMathProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/PixelMath.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* PixelMathProcess::DefaultInterface() const
{
   return ThePixelMathInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* PixelMathProcess::Create() const
{
   return new PixelMathInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* PixelMathProcess::Clone( const ProcessImplementation& p ) const
{
   const PixelMathInstance* instPtr = dynamic_cast<const PixelMathInstance*>( &p );
   return (instPtr != nullptr) ? new PixelMathInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool PixelMathProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: PixelMath [<arg_list>] [<view_list>]"
"\n"
"\n-x[<ch>]=<expr> | -expression[<ch>]=<expr>"
"\n"
"\n      <expr> is a PixelMath expression that will be applied to the target"
"\n      image(s). The optional <ch> suffix specifies a target channel:"
"\n      0=red/gray, 1=green, 2=blue, 3=alpha."
"\n"
"\n      When no suffix is specified, the -g+ argument is implicitly assumed."
"\n      This means that <expr> will be applied to all nominal channels of the"
"\n      target image, unless the -g- argument is explicitly used, in which"
"\n      case <expr> will be applied to the first nominal channel exclusively."
"\n"
"\n      If <expr> contains one or more whitespace characters, the entire"
"\n      expression must be enclosed by double quotes."
"\n"
"\n-s=<sym-list> | -symbols=<sym-list>"
"\n"
"\n      <sym-list> is a comma-delimited list of symbol definitions."
"\n"
"\n-g[+|-] | -single-expression[+|-]"
"\n"
"\n      Apply a single expression to all nominal channels of the target image"
"\n      (when enabled), or apply an independent expression for each channel,"
"\n      if available (when disabled). (default=enabled)"
"\n"
"\n-64[+|-] | -64-bit-working-image"
"\n"
"\n      Use a 64-bit IEEE 754 floating point working image to store the"
"\n      intermediate result (when enabled), or use a 32-bit IEEE 754 floating"
"\n      point working image (when disabled). (default=disabled)"
"\n"
"\n-r[+|-] | -rescale[+|-]"
"\n"
"\n      Enable/disable rescaling of the resulting image. (default=disabled)"
"\n"
"\n-r0=<n> | -rescale-lower=<n>"
"\n-r1=<n> | -rescale-upper=<n>"
"\n"
"\n      <n> is the lower or upper bound to rescale the result, when automatic"
"\n      rescaling is enabled (-r+). The default rescaling range is [0,1]."
"\n"
"\n-t[+|-] | -truncate[+|-]"
"\n"
"\n      Enable/disable truncation of the resulting image. Truncation is never"
"\n      applied when rescaling is enabled (see -r). (default=enabled)"
"\n"
"\n-t0=<n> | -truncate-lower=<n>"
"\n-t1=<n> | -truncate-upper=<n>"
"\n"
"\n      <n> is the lower or upper bound to truncate the result, when automatic"
"\n      truncation is enabled. The default truncation range is [0,1]."
"\n"
"\n-n[+|-] | create-new-image[+|-]"
"\n"
"\n      Enables/disables creation of a new image window to store the result"
"\n      after PixelMath execution. When this option is disabled, PixelMath"
"\n      replaces the current contents of the target image. (default=disabled)"
"\n"
"\n      Unless the -w, -h, -rgb, -gray, -i or -f arguments are used, newly"
"\n      created images will have the same dimensions, color space and sample"
"\n      data type as the target image."
"\n"
"\n-id=<image-id> | image-id=<image-id>"
"\n"
"\n      <image-id> is the identifier that will be assigned to the newly created"
"\n      image, when that option is enabled (see -n). When no identifier is"
"\n      specified, the platform assigns one automatically. Uniqueness of image"
"\n      identifiers is always ensured by appending the necessary suffixes."
"\n"
"\n-w=<n> | -width=<n>"
"\n-h=<n> | -height=<n>"
"\n"
"\n      <n> is the width or height in pixels of the newly created image, when"
"\n      that option is enabled (see -n). By default, or if negative <n> values"
"\n      are specified, these dimensions are equal to that of the target image."
"\n"
"\n-rgb | -rgbcolor"
"\n"
"\n      Create a RGB color image to store the result. This option implies -n+."
"\n"
"\n-gray | -grayscale"
"\n"
"\n      Create a grayscale image to store the result. This option implies -n+."
"\n"
"\n-i<n>"
"\n"
"\n      Create an integer image with <n> bits per sample, where <n> is one of"
"\n      8, 16 or 32. Specifying this argument implies -n+."
"\n"
"\n-f<n>"
"\n"
"\n      Create an IEEE 754 floating point real image with <n> bits per sample,"
"\n      where <n> is one of 32 or 64. Specifying this argument implies -n+."
"\n"
"\n--global"
"\n"
"\n      Execute PixelMath in the global context. This does not exclude"
"\n      execution on views when one or more target views are specified."
"\n"
"\n--interface"
"\n"
"\n      Launches the interface of this process."
"\n"
"\n--help"
"\n"
"\n      Displays this help and exits."
"</raw>" );
}

#define CHECK_OUT_OF_RANGE( param ) \
   if ( arg.NumericValue() < param->MinimumValue() || arg.NumericValue() > param->MaximumValue() ) \
      throw pcl::Error( "Numeric argument out of range: " + arg.Token() )

#define CHECK_INTEGER() \
   if ( Frac( arg.NumericValue() ) != 0 ) \
      throw pcl::Error( "Expected an integer argument value: " + arg.Token() )

int PixelMathProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
      ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   PixelMathInstance instance( this );

   bool launchInterface = false;
   bool executeGlobal = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "r0" || arg.Id() == "rescale-lower" )
         {
            CHECK_OUT_OF_RANGE( ThePMRescaleLowerBoundParameter );
            instance.p_rescaleLower = arg.NumericValue();
         }
         else if ( arg.Id() == "r1" || arg.Id() == "rescale-upper" )
         {
            CHECK_OUT_OF_RANGE( ThePMRescaleUpperBoundParameter );
            instance.p_rescaleUpper = arg.NumericValue();
         }
         else if ( arg.Id() == "t0" || arg.Id() == "truncate-lower" )
         {
            CHECK_OUT_OF_RANGE( ThePMTruncateLowerBoundParameter );
            instance.p_truncateLower = arg.NumericValue();
         }
         else if ( arg.Id() == "t1" || arg.Id() == "truncate-upper" )
         {
            CHECK_OUT_OF_RANGE( ThePMTruncateUpperBoundParameter );
            instance.p_truncateUpper = arg.NumericValue();
         }
         else if ( arg.Id() == "w" || arg.Id() == "width" )
         {
            CHECK_INTEGER();
            CHECK_OUT_OF_RANGE( ThePMNewImageWidthParameter );
            instance.p_newImageWidth = int32( arg.NumericValue() );
         }
         else if ( arg.Id() == "h" || arg.Id() == "height" )
         {
            CHECK_INTEGER();
            CHECK_OUT_OF_RANGE( ThePMNewImageHeightParameter );
            instance.p_newImageHeight = int32( arg.NumericValue() );
         }
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         String s = arg.StringValue();
         s.Trim();

         if ( arg.Id() == "x" || arg.Id() == "expression" || arg.Id() == "x0" || arg.Id() == "expression0" )
            instance.p_expression[0] = s;
         else if ( arg.Id() == "x1" || arg.Id() == "expression1" )
            instance.p_expression[1] = s;
         else if ( arg.Id() == "x2" || arg.Id() == "expression2" )
            instance.p_expression[2] = s;
         else if ( arg.Id() == "s" || arg.Id() == "symbols" )
            instance.p_symbols = s;
         else if ( arg.Id() == "id" || arg.Id() == "image-id" )
         {
            if ( !s.IsEmpty() && !s.IsValidIdentifier() )
               throw Error( "Invalid image identifier: " + arg.Token() );
            instance.p_newImageId = s;
         }
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         if ( arg.Id() == "g" || arg.Id() == "single-expression" )
            instance.p_useSingleExpression = arg.SwitchState();
         else if ( arg.Id() == "64" || arg.Id() == "64-bit-working-image" )
            instance.p_use64BitWorkingImage = arg.SwitchState();
         else if ( arg.Id() == "r" || arg.Id() == "rescale" )
            instance.p_rescaleResult = arg.SwitchState();
         else if ( arg.Id() == "t" || arg.Id() == "truncate" )
            instance.p_truncateResult = arg.SwitchState();
         else if ( arg.Id() == "n" || arg.Id() == "create-new-image" )
            instance.p_createNewImage = arg.SwitchState();
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "rgb" || arg.Id() == "rgbcolor" )
            instance.p_newImageColorSpace = PMNewImageColorSpace::RGB;
         else if ( arg.Id() == "gray" || arg.Id() == "grayscale" )
            instance.p_newImageColorSpace = PMNewImageColorSpace::Gray;
         else if ( arg.Id() == "i8" )
            instance.p_newImageSampleFormat = PMNewImageSampleFormat::I8;
         else if ( arg.Id() == "i16" )
            instance.p_newImageSampleFormat = PMNewImageSampleFormat::I16;
         else if ( arg.Id() == "i32" )
            instance.p_newImageSampleFormat = PMNewImageSampleFormat::I32;
         else if ( arg.Id() == "f32" )
            instance.p_newImageSampleFormat = PMNewImageSampleFormat::F32;
         else if ( arg.Id() == "f64" )
            instance.p_newImageSampleFormat = PMNewImageSampleFormat::F64;
         else if ( arg.Id() == "g" || arg.Id() == "single-expression" )
            instance.p_useSingleExpression = true;
         else if ( arg.Id() == "64" || arg.Id() == "64-bit-working-image" )
            instance.p_use64BitWorkingImage = true;
         else if ( arg.Id() == "r" || arg.Id() == "rescale" )
            instance.p_rescaleResult = true;
         else if ( arg.Id() == "t" || arg.Id() == "truncate" )
            instance.p_truncateResult = true;
         else if ( arg.Id() == "n" || arg.Id() == "create-new-image" )
            instance.p_createNewImage = true;
         else if ( arg.Id() == "-global" )
            executeGlobal = true;
         else if ( arg.Id() == "-interface" )
            launchInterface = true;
         else if ( arg.Id() == "-help" )
         {
            ShowHelp();
            return 0;
         }
         else
            throw Error( "Unknown argument: " + arg.Token() );
      }
      else if ( arg.IsItemList() )
      {
         ++count;

         if ( arg.Items().IsEmpty() )
         {
            Console().WriteLn( "No view(s) found: " + arg.Token() );
            throw;
         }

         for ( StringList::const_iterator j = arg.Items().Begin(); j != arg.Items().End(); ++j )
         {
            View v = View::ViewById( *j );
            if ( v.IsNull() )
               throw Error( "No such view: " + *j );
            instance.LaunchOn( v );
         }
      }
   }

   if ( launchInterface )
      instance.LaunchInterface();

   if ( executeGlobal )
      instance.LaunchGlobal();
   else if ( count == 0 )
   {
      if ( ImageWindow::ActiveWindow().IsNull() )
         throw Error( "There is no active image window." );
      instance.LaunchOnCurrentView();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PixelMathProcess.cpp - Released 2021-05-05T15:38:07Z
