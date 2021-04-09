//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard StarNet Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// StarNetProcess.cpp - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarNet PixInsight module.
//
// Copyright (c) 2018-2020 Nikita Misiura
//
// This software is available under Attribution-NonCommercial-ShareAlike 4.0
// International Creative Commons license (CC BY-NC-SA 4.0):
// 
// https://creativecommons.org/licenses/by-nc-sa/4.0/
// 
// In short: You are free to use and redistribute the code in any medium or
// format, but only under the same license terms. You can transform and build
// your projects upon it. You can NOT use the code for commercial purposes. You
// must give appropriate credit for usage of the code.
// 
// This product is based on software from the PixInsight project, developed by
// Pleiades Astrophoto and its contributors:
// 
// https://pixinsight.com/
// ----------------------------------------------------------------------------

#include "StarNetProcess.h"
#include "StarNetInstance.h"
#include "StarNetInterface.h"
#include "StarNetParameters.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/Settings.h>
#include <pcl/View.h>

// Settings keys
#define KEY_RGB_WEIGHTS_FILE        "RGBWeightsFile"
#define KEY_GRAYSCALE_WEIGHTS_FILE  "GrayscaleWeightsFile"

namespace pcl
{

// ----------------------------------------------------------------------------

StarNetProcess* TheStarNetProcess = nullptr;

static String s_rgbWeightsFilePath;
static String s_grayscaleWeightsFilePath;
static bool   s_preferencesLoaded = false;

// ----------------------------------------------------------------------------

StarNetProcess::StarNetProcess()
{
   TheStarNetProcess = this;

   new SNStride( this );
   new SNCreateMask( this );
}

// ----------------------------------------------------------------------------

IsoString StarNetProcess::Id() const
{
   return "StarNet";
}

// ----------------------------------------------------------------------------

IsoString StarNetProcess::Category() const
{
   return "MaskGeneration,ObjectRecognition";
}

// ----------------------------------------------------------------------------

uint32 StarNetProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String StarNetProcess::Description() const
{
   return "<html>"
          "<p>A neural network removing stars from stretched images.</p>"
          "</html>";
}

// ----------------------------------------------------------------------------

String StarNetProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/StarNet.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* StarNetProcess::DefaultInterface() const
{
   return TheStarNetInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* StarNetProcess::Create() const
{
   return new StarNetInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* StarNetProcess::Clone( const ProcessImplementation& p ) const
{
   const StarNetInstance* instance = dynamic_cast<const StarNetInstance*>( &p );
   return (instance != nullptr) ? new StarNetInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------

void StarNetProcess::SetRGBWeightsFilePath( const String& filePath )
{
   if ( !filePath.IsEmpty() )
      if ( !File::Exists( filePath ) )
         throw Error( "The specified weights database file (RGB) does not exist: <raw>" + filePath + "</raw>" );
   s_rgbWeightsFilePath = filePath;
}

const String& StarNetProcess::RGBWeightsFilePath() const
{
   return s_rgbWeightsFilePath;
}

bool StarNetProcess::IsValidRGBWeightsFilePath() const
{
   return !s_rgbWeightsFilePath.IsEmpty() && File::Exists( s_rgbWeightsFilePath );
}

// ----------------------------------------------------------------------------

void StarNetProcess::SetGrayscaleWeightsFilePath( const String& filePath )
{
   if ( !filePath.IsEmpty() )
      if ( !File::Exists( filePath ) )
         throw Error( "The specified weights database file (grayscale) does not exist: <raw>" + filePath + "</raw>" );
   s_grayscaleWeightsFilePath = filePath;
}

const String& StarNetProcess::GrayscaleWeightsFilePath() const
{
   return s_grayscaleWeightsFilePath;
}

bool StarNetProcess::IsValidGrayscaleWeightsFilePath() const
{
   return !s_grayscaleWeightsFilePath.IsEmpty() && File::Exists( s_grayscaleWeightsFilePath );
}

// ----------------------------------------------------------------------------

bool StarNetProcess::PreferencesLoaded() const
{
   return s_preferencesLoaded;
}

void StarNetProcess::LoadPreferences()
{
   Settings::Read( KEY_RGB_WEIGHTS_FILE,       s_rgbWeightsFilePath );
   Settings::Read( KEY_GRAYSCALE_WEIGHTS_FILE, s_grayscaleWeightsFilePath );
   s_preferencesLoaded = true;
}

void StarNetProcess::SavePreferences()
{
   Settings::Write( KEY_RGB_WEIGHTS_FILE,       s_rgbWeightsFilePath );
   Settings::Write( KEY_GRAYSCALE_WEIGHTS_FILE, s_grayscaleWeightsFilePath );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF StarNetProcess.cpp - Released 2021-04-09T19:41:49Z
