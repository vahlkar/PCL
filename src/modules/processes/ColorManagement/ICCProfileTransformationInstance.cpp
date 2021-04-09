//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// ICCProfileTransformationInstance.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
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

#include "ICCProfileTransformationInstance.h"
#include "ICCProfileTransformationParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ICCProfileTransformation.h>
#include <pcl/ImageWindow.h>
#include <pcl/StandardStatus.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ICCProfileTransformationInstance::ICCProfileTransformationInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_toDefaultProfile( TheICCTToDefaultProfileParameter->DefaultValue() )
   , p_renderingIntent( ICCTRenderingIntent::Default )
   , p_useBlackPointCompensation( TheICCTUseBlackPointCompensationParameter->DefaultValue() )
   , p_useFloatingPointTransformation( TheICCTUseFloatingPointTransformationParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

ICCProfileTransformationInstance::ICCProfileTransformationInstance( const ICCProfileTransformationInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// -------------------------------------------------------------------------

void ICCProfileTransformationInstance::Assign( const ProcessImplementation& p )
{
   const ICCProfileTransformationInstance* x = dynamic_cast<const ICCProfileTransformationInstance*>( &p );
   if ( x != nullptr )
   {
      p_targetProfile = x->p_targetProfile;
      p_toDefaultProfile = x->p_toDefaultProfile;
      p_renderingIntent = x->p_renderingIntent;
      p_useBlackPointCompensation = x->p_useBlackPointCompensation;
      p_useFloatingPointTransformation = x->p_useFloatingPointTransformation;
   }
}

// ----------------------------------------------------------------------------

bool ICCProfileTransformationInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags ICCProfileTransformationInstance::UndoMode( const View& v  ) const
{
   return UndoFlag::PixelData | UndoFlag::ICCProfile;
}

// ----------------------------------------------------------------------------

bool ICCProfileTransformationInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "ICCProfileTransformation cannot be executed on previews.";
      return false;
   }

   if ( !p_toDefaultProfile )
      if ( p_targetProfile.IsEmpty() )
      {
         whyNot = "This instance of ICCProfileTransformation cannot be executed because no target profile has been specified.";
         return false;
      }

   return true;
}

// ----------------------------------------------------------------------------

bool ICCProfileTransformationInstance::ExecuteOn( View& view )
{
   static ICCProfile::profile_list profiles; // ### Warning: Not reentrant -> thread unsafe.

   if ( !view.IsMainView() )
      return false;

   AutoViewLock lock( view );

   Console().EnableAbort();

   ImageWindow w = view.Window();

   ICCProfile sourceICC;
   w.GetICCProfile( sourceICC );

   if ( !sourceICC.IsProfile() )
   {
      String defaultProfilePath = PixInsightSettings::GlobalString( view.IsColor() ?
         "ColorManagement/DefaultRGBProfilePath" : "ColorManagement/DefaultGrayscaleProfilePath" );

      try
      {
         sourceICC.Load( defaultProfilePath );
      }
      ERROR_HANDLER

      if ( !sourceICC.IsProfile() )
         throw Error( "Unable to load the default ICC profile: " + defaultProfilePath );
   }

   ICCProfile targetICC;

   if ( p_toDefaultProfile )
   {
      String defaultProfilePath = PixInsightSettings::GlobalString( view.IsColor() ?
         "ColorManagement/DefaultRGBProfilePath" : "ColorManagement/DefaultGrayscaleProfilePath" );

      try
      {
         targetICC.Load( defaultProfilePath );
      }
      ERROR_HANDLER

      if ( !targetICC.IsProfile() )
         throw Error( "Unable to load the default ICC profile: " + defaultProfilePath );
   }
   else
   {
      if ( p_targetProfile.IsEmpty() )
         return false;

      bool hadProfiles = !profiles.IsEmpty();
      if ( !hadProfiles )
         profiles = ICCProfile::FindProfilesByColorSpace( ICCColorSpace::RGB|ICCColorSpace::Gray );

      ICCProfile::profile_list::const_iterator i = profiles.Search( ICCProfile::Info( p_targetProfile ) );

      if ( i == profiles.End() || !File::Exists( i->path ) )
      {
         if ( hadProfiles )
         {
            profiles = ICCProfile::FindProfilesByColorSpace( ICCColorSpace::RGB|ICCColorSpace::Gray );
            i = profiles.Search( ICCProfile::Info( p_targetProfile ) );
         }
         else
            i = profiles.End();

         if ( i == profiles.End() )
            throw Error( "Couldn't find the '" + p_targetProfile + "' profile. "
                         "Either it has not been installed, or the corresponding disk file has been deleted." );
      }

      try
      {
         targetICC.Load( i->path );
      }
      ERROR_HANDLER

      if ( !targetICC.IsProfile() )
         throw Error( "Unable to load the '" + p_targetProfile + "' profile. "
                      "The corresponding disk file could be corrupted, or it is not a valid ICC profile." );

      if ( w.MainView().IsColor() && !targetICC.IsRGB() )
         throw Error( '\'' + p_targetProfile + "' is a grayscale profile. "
                      "This profile cannot be used to transform a RGB color image." );
   }

   Console().WriteLn( "<end><cbr>ICC profile transform: <raw>" + sourceICC.Description() + " -> " + targetICC.Description() + "</raw>" );

   ICCProfileTransformation T;
   T.Add( sourceICC );
   T.Add( targetICC );
   T.SetRenderingIntent( ICCProfileTransformation::rendering_intent( p_renderingIntent ) );
   T.EnableBlackPointCompensation( p_useBlackPointCompensation );
   T.ForceFloatingPointTransformation( p_useFloatingPointTransformation );
   T.EnableHighResolutionCLUT();

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );

   T >> image;
   w.SetICCProfile( targetICC );

   Console().WriteLn( "<end><cbr>Profile assigned: <raw>" + targetICC.Description() + "</raw>" );
   Console().WriteLn( "Profile path: <raw>" + targetICC.FilePath() + "</raw>" );

   return true;
}

// ----------------------------------------------------------------------------

void* ICCProfileTransformationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheICCTTargetProfileParameter )
      return p_targetProfile.Begin();
   if ( p == TheICCTToDefaultProfileParameter )
      return &p_toDefaultProfile;
   if ( p == TheICCTRenderingIntentParameter )
      return &p_renderingIntent;
   if ( p == TheICCTUseBlackPointCompensationParameter )
      return &p_useBlackPointCompensation;
   if ( p == TheICCTUseFloatingPointTransformationParameter )
      return &p_useFloatingPointTransformation;

   return nullptr;
}

bool ICCProfileTransformationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheICCTTargetProfileParameter )
   {
      p_targetProfile.Clear();
      if ( sizeOrLength > 0 )
         p_targetProfile.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type ICCProfileTransformationInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheICCTTargetProfileParameter )
      return p_targetProfile.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ICCProfileTransformationInstance.cpp - Released 2021-04-09T19:41:48Z
