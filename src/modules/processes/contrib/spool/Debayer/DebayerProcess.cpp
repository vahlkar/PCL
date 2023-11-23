//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.3
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 1.11.2
// ----------------------------------------------------------------------------
// DebayerProcess.cpp - Released 2023-11-23T18:45:26Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "DebayerProcess.h"
#include "DebayerParameters.h"
#include "DebayerInstance.h"
#include "DebayerInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

DebayerProcess* TheDebayerProcess = nullptr;

// ----------------------------------------------------------------------------

DebayerProcess::DebayerProcess()
{
   TheDebayerProcess = this;

   new DebayerBayerPatternParameter( this );
   new DebayerMethodParameter( this );
   new DebayerFBDDNoiseReduction( this );
   new DebayerShowImages( this );
   new DebayerCFASourceFilePath( this );
   new DebayerTargetItems( this );
   new DebayerTargetEnabled( TheDebayerTargetItemsParameter );
   new DebayerTargetImage( TheDebayerTargetItemsParameter );
   new DebayerNoGUIMessages( this );
   new DebayerEvaluateNoise( this );
   new DebayerNoiseEvaluationAlgorithm( this );
   new DebayerEvaluateSignal( this );
   new DebayerStructureLayers( this );
   new DebayerSaturationThreshold( this );
   new DebayerSaturationRelative( this );
   new DebayerNoiseLayers( this );
   new DebayerHotPixelFilterRadius( this );
   new DebayerNoiseReductionFilterRadius( this );
   new DebayerMinStructureSize( this );
   new DebayerPSFType( this );
   new DebayerPSFGrowth( this );
   new DebayerMaxStars( this );
   new DebayerInputHints( this );
   new DebayerOutputHints( this );
   new DebayerOutputRGBImages( this );
   new DebayerOutputSeparateChannels( this );
   new DebayerOutputDirectory( this );
   new DebayerOutputExtension( this );
   new DebayerOutputPrefix( this );
   new DebayerOutputPostfix( this );
   new DebayerOverwriteExistingFiles( this );
   new DebayerOnError( this );
   new DebayerUseFileThreads( this );
   new DebayerFileThreadOverload( this );
   new DebayerMaxFileReadThreads( this );
   new DebayerMaxFileWriteThreads( this );
   new DebayerMemoryLoadControl( this );
   new DebayerMemoryLoadLimit( this );

   new DebayerOutputImage( this );
   new DebayerOutputChannelImageR( this );
   new DebayerOutputChannelImageG( this );
   new DebayerOutputChannelImageB( this );
   new DebayerPSFTotalFluxEstimateR( this );
   new DebayerPSFTotalFluxEstimateG( this );
   new DebayerPSFTotalFluxEstimateB( this );
   new DebayerPSFTotalPowerFluxEstimateR( this );
   new DebayerPSFTotalPowerFluxEstimateG( this );
   new DebayerPSFTotalPowerFluxEstimateB( this );
   new DebayerPSFTotalMeanFluxEstimateR( this );
   new DebayerPSFTotalMeanFluxEstimateG( this );
   new DebayerPSFTotalMeanFluxEstimateB( this );
   new DebayerPSFTotalMeanPowerFluxEstimateR( this );
   new DebayerPSFTotalMeanPowerFluxEstimateG( this );
   new DebayerPSFTotalMeanPowerFluxEstimateB( this );
   new DebayerPSFMStarEstimateR( this );
   new DebayerPSFMStarEstimateG( this );
   new DebayerPSFMStarEstimateB( this );
   new DebayerPSFNStarEstimateR( this );
   new DebayerPSFNStarEstimateG( this );
   new DebayerPSFNStarEstimateB( this );
   new DebayerPSFCountR( this );
   new DebayerPSFCountG( this );
   new DebayerPSFCountB( this );
   new DebayerNoiseEstimateR( this );
   new DebayerNoiseEstimateG( this );
   new DebayerNoiseEstimateB( this );
   new DebayerNoiseFractionR( this );
   new DebayerNoiseFractionG( this );
   new DebayerNoiseFractionB( this );
   new DebayerNoiseScaleLowR( this );
   new DebayerNoiseScaleLowG( this );
   new DebayerNoiseScaleLowB( this );
   new DebayerNoiseScaleHighR( this );
   new DebayerNoiseScaleHighG( this );
   new DebayerNoiseScaleHighB( this );
   new DebayerNoiseAlgorithmR( this );
   new DebayerNoiseAlgorithmG( this );
   new DebayerNoiseAlgorithmB( this );

   new DebayerOutputFileData( this );
   new DebayerOutputFilePath( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalFluxEstimateR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalFluxEstimateG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalFluxEstimateB( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalPowerFluxEstimateR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalPowerFluxEstimateG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalPowerFluxEstimateB( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalMeanFluxEstimateR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalMeanFluxEstimateG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalMeanFluxEstimateB( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalMeanPowerFluxEstimateR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalMeanPowerFluxEstimateG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFTotalMeanPowerFluxEstimateB( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFMStarEstimateR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFMStarEstimateG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFMStarEstimateB( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFNStarEstimateR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFNStarEstimateG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFNStarEstimateB( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFCountR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFCountG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFilePSFCountB( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseEstimateR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseEstimateG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseEstimateB( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseFractionR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseFractionG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseFractionB( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseScaleLowR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseScaleLowG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseScaleLowB( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseScaleHighR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseScaleHighG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseScaleHighB( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseAlgorithmR( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseAlgorithmG( TheDebayerOutputFileDataParameter );
   new DebayerOutputFileNoiseAlgorithmB( TheDebayerOutputFileDataParameter );
   new DebayerOutputChannelFilePathR( TheDebayerOutputFileDataParameter );
   new DebayerOutputChannelFilePathG( TheDebayerOutputFileDataParameter );
   new DebayerOutputChannelFilePathB( TheDebayerOutputFileDataParameter );
}

// ----------------------------------------------------------------------------

IsoString DebayerProcess::Id() const
{
   return "Debayer";
}

// ----------------------------------------------------------------------------

IsoString DebayerProcess::Category() const
{
   return "Preprocessing,ColorSpaces";
}

// ----------------------------------------------------------------------------

uint32 DebayerProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String DebayerProcess::Description() const
{
   return
   "<html>"
   "<p>Demosaics a raw digital camera image.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String DebayerProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Debayer.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* DebayerProcess::DefaultInterface() const
{
   return TheDebayerInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* DebayerProcess::Create() const
{
   return new DebayerInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* DebayerProcess::Clone( const ProcessImplementation& p ) const
{
   const DebayerInstance* instPtr = dynamic_cast<const DebayerInstance*>( &p );
   return (instPtr != nullptr) ? new DebayerInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool DebayerProcess::CanProcessCommandLines() const
{
   // ### TODO update the command line processing bit
   return false;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: Debayer [<arg_list>] [<view_list>]"
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

int DebayerProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   DebayerInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

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
// EOF DebayerProcess.cpp - Released 2023-11-23T18:45:26Z
