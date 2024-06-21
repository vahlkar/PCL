//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// Standard FindingChart Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// FindingChartProcess.cpp - Released 2024-06-18T15:49:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard FindingChart PixInsight module.
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

#include "FindingChartInstance.h"
#include "FindingChartInterface.h"
#include "FindingChartParameters.h"
#include "FindingChartProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FindingChartProcess* TheFindingChartProcess = nullptr;

// ----------------------------------------------------------------------------

FindingChartProcess::FindingChartProcess()
{
   TheFindingChartProcess = this;

   new FCChartSize( this );
   new FCAutoChartResolution( this );
   new FCChartResolution( this );
   new FCAutoChartMaxMagnitude( this );
   new FCChartMaxMagnitude( this );
   new FCDrawGrid( this );
   new FCDrawEcliptic( this );
   new FCDrawGalacticEquator( this );
   new FCDrawConstellationBorders( this );
   new FCDrawConstellationLines( this );
   new FCDrawConstellationNames( this );
   new FCDrawStars( this );
   new FCDrawStarNames( this );
   new FCDrawMessierObjects( this );
   new FCDrawNGCObjects( this );
   new FCGenerateBitmapFile( this );
   new FCOutputDirectory( this );
   new FCOutputSuffix( this );
   new FCCloseOutputWindow( this );
   new FCImageRegionColor( this );
   new FCImageRegionBorderColor( this );
   new FCGridColor( this );
   new FCGridTextColor( this );
   new FCEclipticColor( this );
   new FCEclipticTextColor( this );
   new FCGalacticEquatorColor( this );
   new FCGalacticEquatorTextColor( this );
   new FCConstellationBorderColor( this );
   new FCConstellationLineColor( this );
   new FCConstellationTextColor( this );
   new FCStarTextColor( this );
   new FCMessierTextColor( this );
   new FCNGCTextColor( this );
}

// ----------------------------------------------------------------------------

IsoString FindingChartProcess::Id() const
{
   return "FindingChart";
}

// ----------------------------------------------------------------------------

IsoString FindingChartProcess::Category() const
{
   return "Astrometry,Render";
}

// ----------------------------------------------------------------------------

uint32 FindingChartProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String FindingChartProcess::Description() const
{
   return
   "<html>"
   "</html>";
}

// ----------------------------------------------------------------------------

String FindingChartProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/FindingChart.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* FindingChartProcess::DefaultInterface() const
{
   return TheFindingChartInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* FindingChartProcess::Create() const
{
   return new FindingChartInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* FindingChartProcess::Clone( const ProcessImplementation& p ) const
{
   const FindingChartInstance* instance = dynamic_cast<const FindingChartInstance*>( &p );
   return (instance != nullptr) ? new FindingChartInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------

bool FindingChartProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: FindingChart [<arg_list>] [<view_list>]"
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

int FindingChartProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   FindingChartInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( const Argument& arg : arguments )
   {
      if ( arg.IsNumeric() )
      {
         throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         // These are standard parameters that all processes should provide.
         if ( arg.Id() == "-interface" )
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
            throw Error( "No view(s) found: " + arg.Token() );

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
// EOF FindingChartProcess.cpp - Released 2024-06-18T15:49:25Z
