//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// EphemerisGeneratorProcess.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
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

#include "EphemerisGeneratorInstance.h"
#include "EphemerisGeneratorInterface.h"
#include "EphemerisGeneratorParameters.h"
#include "EphemerisGeneratorProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/ErrorHandler.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

EphemerisGeneratorProcess* TheEphemerisGeneratorProcess = nullptr;

// ----------------------------------------------------------------------------

EphemerisGeneratorProcess::EphemerisGeneratorProcess()
{
   TheEphemerisGeneratorProcess = this;

   new EGWorkingMode( this );
   new EGPosX( this );
   new EGPosY( this );
   new EGPosZ( this );
   new EGVelX( this );
   new EGVelY( this );
   new EGVelZ( this );
   new EGElemA( this );
   new EGElemQ( this );
   new EGElemE( this );
   new EGElemM( this );
   new EGElemT( this );
   new EGElemI( this );
   new EGElemO( this );
   new EGElemW( this );
   new EGEpochJD( this );
   new EGObjectId( this );
   new EGObjectName( this );
   new EGH( this );
   new EGG( this );
   new EGB_VDefined( this );
   new EGB_V( this );
   new EGDDefined( this );
   new EGD( this );
   new EGDatabaseFilePath( this );
   new EGDatabaseFormatName( this );
   new EGObjects( this );
   new EGUseRegularExpressions( this );
   new EGExcludeCoreAsteroids( this );
   new EGFundamentalFilePath( this );
   new EGAsteroidsFilePath( this );
   new EGKBOsFilePath( this );
   new EGStartTimeJD( this );
   new EGEndTimeJD( this );
   new EGUseAsteroidPerturbers( this );
   new EGUseKBOPerturbers( this );
   new EGVelocityExpansions( this );
   new EGSeparateEarthMoonPerturbers( this );
   new EGRelativisticPerturbations( this );
   new EGFigureEffects( this );
   new EGOutputXEPHFile( this );
   new EGOutputXEPHFilePath( this );
   new EGOverwriteExistingFiles( this );
   new EGDenseOutputToleranceFactor( this );
   new EGEphemerisToleranceFactor( this );
}

// ----------------------------------------------------------------------------

IsoString EphemerisGeneratorProcess::Id() const
{
   return "EphemerisGenerator";
}

// ----------------------------------------------------------------------------

IsoString EphemerisGeneratorProcess::Category() const
{
   return "Ephemerides";
}

// ----------------------------------------------------------------------------

uint32 EphemerisGeneratorProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String EphemerisGeneratorProcess::Description() const
{
   return
   "<html>"
   "</html>";
}

// ----------------------------------------------------------------------------

String EphemerisGeneratorProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/EphemerisGenerator.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* EphemerisGeneratorProcess::DefaultInterface() const
{
   return TheEphemerisGeneratorInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* EphemerisGeneratorProcess::Create() const
{
   return new EphemerisGeneratorInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* EphemerisGeneratorProcess::Clone( const ProcessImplementation& p ) const
{
   const EphemerisGeneratorInstance* instance = dynamic_cast<const EphemerisGeneratorInstance*>( &p );
   return (instance != nullptr) ? new EphemerisGeneratorInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static Array<TextDatabase::FormatDescription> s_formats;
static bool                                   s_initialized = false;
static bool                                   s_valid = false;

void EphemerisGeneratorProcess::InitializeDatabaseFormats()
{
   if ( !s_initialized )
   {
      s_initialized = true;

      String odfFilePath = PixInsightSettings::GlobalString( "Application/LibraryDirectory" ) + "/default.xodf";

      try
      {
         Console console;
         console.NoteLn( "<end><cbr><br>* EphemerisGenerator: Parsing object database format descriptions: <raw>" + odfFilePath + "</raw>" );

         if ( !File::Exists( odfFilePath ) )
            throw Error( "EphemerisGenerator: The database format description file does not exist: " + odfFilePath );

         s_formats = TextDatabase::FormatDescription::Parse( odfFilePath );
         s_valid = true;
         console.WriteLn( "<end><cbr>" + String( s_formats.Length() ) + " object database format(s) acquired.<br>" );
      }
      ERROR_HANDLER
   }
}

bool EphemerisGeneratorProcess::HasValidDatabaseFormats()
{
   return s_valid;
}

const Array<TextDatabase::FormatDescription>& EphemerisGeneratorProcess::DatabaseFormats()
{
   return s_formats;
}

const TextDatabase::FormatDescription& EphemerisGeneratorProcess::DefaultDatabaseFormat()
{
   return s_formats[0];
}

int EphemerisGeneratorProcess::FindDatabaseFormatByName( const String& name )
{
   for ( size_type i = 0; i < s_formats.Length(); ++i )
      if ( s_formats[i].name.CompareIC( name ) == 0 )
         return int( i );
   return -1;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF EphemerisGeneratorProcess.cpp - Released 2021-04-09T19:41:48Z
