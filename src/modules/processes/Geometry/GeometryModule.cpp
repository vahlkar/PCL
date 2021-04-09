//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// GeometryModule.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     2
#define MODULE_VERSION_REVISION  3
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2021
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       9

#include <pcl/AstrometricMetadata.h>
#include <pcl/Console.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/View.h>

#include "ChannelMatchInterface.h"
#include "ChannelMatchProcess.h"
#include "CropInterface.h"
#include "CropProcess.h"
#include "DynamicCropInterface.h"
#include "DynamicCropProcess.h"
#include "FastRotationActions.h"
#include "FastRotationInterface.h"
#include "FastRotationProcess.h"
#include "GeometryModule.h"
#include "IntegerResampleInterface.h"
#include "IntegerResampleProcess.h"
#include "ResampleInterface.h"
#include "ResampleProcess.h"
#include "RotationInterface.h"
#include "RotationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GeometryModule::GeometryModule()
{
}

// ----------------------------------------------------------------------------

const char* GeometryModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString GeometryModule::Name() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

String GeometryModule::Description() const
{
   return "PixInsight Standard Geometry Process Module";
}

// ----------------------------------------------------------------------------

String GeometryModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String GeometryModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String GeometryModule::Copyright() const
{
   return "Copyright (c) 2005-2020 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String GeometryModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String GeometryModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Geometry-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Geometry-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Geometry-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Geometry-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void GeometryModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void GeometryModule::OnLoad()
{
   new Rotate180Action;
   new Rotate90CWAction;
   new Rotate90CCWAction;
   new HorizontalMirrorAction;
   new VerticalMirrorAction;
}

// ----------------------------------------------------------------------------

bool WarnOnAstrometryMetadataOrPreviewsOrMask( const ImageWindow& window, const IsoString& processId, bool noGUIMessages )
{
   bool hasAstrometry = window.HasAstrometricSolution();
   bool hasPreviews = window.HasPreviews();
   bool hasMaskReferences = window.HasMaskReferences();
   bool hasMask = !window.Mask().IsNull();

   if ( hasAstrometry || hasPreviews || hasMaskReferences || hasMask )
   {
      if ( !noGUIMessages )
         if ( MessageBox( "<p>" + window.MainView().Id() + "</p>"
                          "<p>The following items will be deleted as a result of the geometric transformation:</p>"
                          "<ul>"
                          + (hasAstrometry ?     "<li>Astrometric solution</li>" : "")
                          + (hasPreviews ?       "<li>Previews</li>" : "")
                          + (hasMaskReferences ? "<li>Mask references</li>" : "")
                          + (hasMask ?           "<li>Mask</li>" : "")
                          + "</ul>"
                          "<p><b>Some of these side effects could be irreversible. Proceed?</b></p>",
                          processId,
                          StdIcon::Warning,
                          StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
         {
            return false;
         }

      Console console;
      if ( hasAstrometry )
         console.WarningLn( "<end><cbr><br>** Warning: " + processId + ": Existing astrometric solution will be deleted." );
      if ( hasPreviews )
         console.WarningLn( "** Warning: " + processId + ": Existing previews will be deleted." );
      if ( hasMaskReferences )
         console.WarningLn( "** Warning: " + processId + ": Existing mask references will be deleted." );
      if ( hasMaskReferences )
         console.WarningLn( "** Warning: " + processId + ": Existing mask will be removed." );
   }

   return true;
}

// ----------------------------------------------------------------------------

void DeleteAstrometryMetadataAndPreviewsAndMask( ImageWindow& window,
                                                 bool deleteCenterMetadata, bool deleteScaleMetadata, double pixelSizeScalingFactor )
{
   DeleteAstrometryMetadataAndPreviews( window, deleteCenterMetadata, deleteScaleMetadata, pixelSizeScalingFactor );
   window.RemoveMaskReferences();
   window.RemoveMask();
}

// ----------------------------------------------------------------------------

void DeleteAstrometryMetadataAndPreviews( ImageWindow& window,
                                          bool deleteCenterMetadata, bool deleteScaleMetadata, double pixelSizeScalingFactor )
{
   window.ClearAstrometricSolution();

   FITSKeywordArray keywords = window.Keywords();
   AstrometricMetadata::RemoveKeywords( keywords, deleteCenterMetadata, deleteScaleMetadata );
   if ( !deleteScaleMetadata )
      if ( pixelSizeScalingFactor != 1 )
         AstrometricMetadata::RescalePixelSizeKeywords( keywords, pixelSizeScalingFactor );
   window.SetKeywords( keywords );

   AstrometricMetadata::RemoveProperties( window, deleteCenterMetadata, deleteScaleMetadata );
   if ( !deleteScaleMetadata )
      if ( pixelSizeScalingFactor != 1 )
         AstrometricMetadata::RescalePixelSizeProperties( window, pixelSizeScalingFactor );

   window.DeletePreviews();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::GeometryModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::DynamicCropProcess;
      new pcl::DynamicCropInterface;
      new pcl::FastRotationProcess;
      new pcl::FastRotationInterface;
      new pcl::ChannelMatchProcess;
      new pcl::ChannelMatchInterface;
      new pcl::ResampleProcess;
      new pcl::ResampleInterface;
      new pcl::IntegerResampleProcess;
      new pcl::IntegerResampleInterface;
      new pcl::RotationProcess;
      new pcl::RotationInterface;
      new pcl::CropProcess;
      new pcl::CropInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF GeometryModule.cpp - Released 2021-04-09T19:41:48Z
