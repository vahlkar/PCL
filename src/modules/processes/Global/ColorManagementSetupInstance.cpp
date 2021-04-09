//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// ColorManagementSetupInstance.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#include "ColorManagementSetupInstance.h"
#include "ColorManagementSetupParameters.h"

#include <pcl/GlobalSettings.h>
#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ColorManagementSetupInstance::ColorManagementSetupInstance( const MetaProcess* p )
   : ProcessImplementation( p )
   , p_enabled( TheCMSEnabledParameter->DefaultValue() )
   , p_detectMonitorProfile( TheCMSDetectMonitorProfileParameter->DefaultValue() )
   , p_defaultRenderingIntent( CMSRenderingIntent::DefaultForScreen )
   , p_onProfileMismatch( CMSOnProfileMismatch::Default )
   , p_onMissingProfile( CMSOnMissingProfile::Default )
   , p_defaultEmbedProfilesInRGBImages( TheCMSDefaultEmbedProfilesInRGBImagesParameter->DefaultValue() )
   , p_defaultEmbedProfilesInGrayscaleImages( TheCMSDefaultEmbedProfilesInGrayscaleImagesParameter->DefaultValue() )
   , p_useLowResolutionCLUTs( TheCMSUseLowResolutionCLUTsParameter->DefaultValue() )
   , p_proofingIntent( CMSRenderingIntent::DefaultForProofing )
   , p_useProofingBPC( TheCMSUseProofingBPCParameter->DefaultValue() )
   , p_defaultProofingEnabled( TheCMSDefaultProofingEnabledParameter->DefaultValue() )
   , p_defaultGamutCheckEnabled( TheCMSDefaultGamutCheckEnabledParameter->DefaultValue() )
   , p_gamutWarningColor( TheCMSGamutWarningColorParameter->DefaultValue() )
{
   /*
    * The default sRGB profile is system/platform dependent. It is detected
    * automatically upon application startup.
    *
    * N.B.: We cannot call PixInsightSettings::GlobalString() if the module has
    * not been installed, because it requires communication with the core
    * application. The interface class will have to initialize its instance
    * member in its reimplementation of Initialize().
    */
   if ( Module->IsInstalled() )
   {
      String sRGBProfilePath = PixInsightSettings::GlobalString( "ColorManagement/SRGBProfilePath" );
      if ( !sRGBProfilePath.IsEmpty() )
      {
         ICCProfile icc( sRGBProfilePath );
         if ( icc.IsProfile() )
            p_defaultRGBProfile = p_defaultGrayscaleProfile = p_proofingProfile = icc.Description();
      }
   }
}

// ----------------------------------------------------------------------------

ColorManagementSetupInstance::ColorManagementSetupInstance( const ColorManagementSetupInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ColorManagementSetupInstance::Assign( const ProcessImplementation& p )
{
   const ColorManagementSetupInstance* x = dynamic_cast<const ColorManagementSetupInstance*>( &p );
   if ( x != nullptr )
   {
      p_enabled                               = x->p_enabled;
      p_detectMonitorProfile                  = x->p_detectMonitorProfile;
      p_updateMonitorProfile                  = x->p_updateMonitorProfile;
      p_defaultRGBProfile                     = x->p_defaultRGBProfile;
      p_defaultGrayscaleProfile               = x->p_defaultGrayscaleProfile;
      p_defaultRenderingIntent                = x->p_defaultRenderingIntent;
      p_onProfileMismatch                     = x->p_onProfileMismatch;
      p_onMissingProfile                      = x->p_onMissingProfile;
      p_defaultEmbedProfilesInRGBImages       = x->p_defaultEmbedProfilesInRGBImages;
      p_defaultEmbedProfilesInGrayscaleImages = x->p_defaultEmbedProfilesInGrayscaleImages;
      p_useLowResolutionCLUTs                 = x->p_useLowResolutionCLUTs;
      p_proofingProfile                       = x->p_proofingProfile;
      p_proofingIntent                        = x->p_proofingIntent;
      p_useProofingBPC                        = x->p_useProofingBPC;
      p_defaultProofingEnabled                = x->p_defaultProofingEnabled;
      p_defaultGamutCheckEnabled              = x->p_defaultGamutCheckEnabled;
      p_gamutWarningColor                     = x->p_gamutWarningColor;
   }
}

// ----------------------------------------------------------------------------

bool ColorManagementSetupInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "ColorManagementSetup can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool ColorManagementSetupInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ColorManagementSetupInstance::ExecuteGlobal()
{
   /*
    * Find all installed ICC profiles
    */
   StringList all = ICCProfile::FindProfiles();

   /*
    * Find the default RGB profile
    */
   StringList descriptions;
   StringList paths;
   ICCProfile::ExtractProfileList( descriptions,
                                   paths,
                                   all,
                                   ICCColorSpace::RGB );

   StringList::const_iterator i = descriptions.Search( p_defaultRGBProfile );
   if ( i == descriptions.End() )
      throw Error( "Couldn't find the '" + p_defaultRGBProfile + "' profile.\n"
                   "Either it has not been installed, it is not a valid RGB profile,\n"
                   "or the corresponding disk file has been removed." );

   String rgbPath = paths[i - descriptions.Begin()];

   /*
    * Find the default grayscale profile
    */
   descriptions.Clear();
   paths.Clear();
   ICCProfile::ExtractProfileList( descriptions,
                                   paths,
                                   all,
                                   ICCColorSpace::RGB|ICCColorSpace::Gray );

   i = descriptions.Search( p_defaultGrayscaleProfile );
   if ( i == descriptions.End() )
      throw Error( "Couldn't find the '" + p_defaultGrayscaleProfile + "' profile.\n"
                   "Either it has not been installed, or the corresponding disk file has been removed." );

   String grayPath = paths[i - descriptions.Begin()];

   /*
    * Find the proofing profile
    */
   descriptions.Clear();
   paths.Clear();
   ICCProfile::ExtractProfileList( descriptions,
                                   paths,
                                   all ); // all color spaces are valid for proofing

   i = descriptions.Search( p_proofingProfile );
   if ( i == descriptions.End() )
      throw Error( "Couldn't find the '" + p_proofingProfile + "' profile.\n"
                   "Either it has not been installed, or the corresponding disk file has been removed." );

   String proofingPath = paths[i - descriptions.Begin()];

   /*
    * Perform global settings update
    */
   PixInsightSettings::BeginUpdate();

   try
   {
      PixInsightSettings::SetGlobalFlag( "ColorManagement/IsEnabled", p_enabled );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/DetectMonitorProfile", p_detectMonitorProfile );
      if ( !p_updateMonitorProfile.IsEmpty() )
         PixInsightSettings::SetGlobalString( "ColorManagement/UpdateMonitorProfile", p_updateMonitorProfile );
      PixInsightSettings::SetGlobalString( "ColorManagement/DefaultRGBProfilePath", rgbPath );
      PixInsightSettings::SetGlobalString( "ColorManagement/DefaultGrayscaleProfilePath", grayPath );
      PixInsightSettings::SetGlobalInteger( "ColorManagement/DefaultRenderingIntent", p_defaultRenderingIntent );
      PixInsightSettings::SetGlobalInteger( "ColorManagement/OnProfileMismatch", p_onProfileMismatch );
      PixInsightSettings::SetGlobalInteger( "ColorManagement/OnMissingProfile", p_onMissingProfile );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/DefaultEmbedProfilesInRGBImages", p_defaultEmbedProfilesInRGBImages );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/DefaultEmbedProfilesInGrayscaleImages", p_defaultEmbedProfilesInGrayscaleImages );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/UseLowResolutionCLUTs", p_useLowResolutionCLUTs );
      PixInsightSettings::SetGlobalString( "ColorManagement/ProofingProfilePath", proofingPath );
      PixInsightSettings::SetGlobalInteger( "ColorManagement/ProofingIntent", p_proofingIntent );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/UseProofingBPC", p_useProofingBPC );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/DefaultProofingEnabled", p_defaultProofingEnabled );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/DefaultGamutCheckEnabled", p_defaultGamutCheckEnabled );
      PixInsightSettings::SetGlobalColor( "ColorManagement/GamutWarningColor", p_gamutWarningColor );

      PixInsightSettings::EndUpdate();
      return true;
   }
   catch ( ... )
   {
      // ### Warning: Don't forget to do this, or the core will bite you!
      PixInsightSettings::CancelUpdate();
      throw;
   }
}

// ----------------------------------------------------------------------------

void* ColorManagementSetupInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheCMSEnabledParameter )
      return &p_enabled;
   if ( p == TheCMSDetectMonitorProfileParameter )
      return &p_detectMonitorProfile;
   if ( p == TheCMSUpdateMonitorProfileParameter )
      return p_updateMonitorProfile.Begin();
   if ( p == TheCMSDefaultRGBProfileParameter )
      return p_defaultRGBProfile.Begin();
   if ( p == TheCMSDefaultGrayProfileParameter )
      return p_defaultGrayscaleProfile.Begin();
   if ( p == TheCMSDefaultRenderingIntentParameter )
      return &p_defaultRenderingIntent;
   if ( p == TheCMSOnProfileMismatchParameter )
      return &p_onProfileMismatch;
   if ( p == TheCMSOnMissingProfileParameter )
      return &p_onMissingProfile;
   if ( p == TheCMSDefaultEmbedProfilesInRGBImagesParameter )
      return &p_defaultEmbedProfilesInRGBImages;
   if ( p == TheCMSDefaultEmbedProfilesInGrayscaleImagesParameter )
      return &p_defaultEmbedProfilesInGrayscaleImages;
   if ( p == TheCMSUseLowResolutionCLUTsParameter )
      return &p_useLowResolutionCLUTs;
   if ( p == TheCMSProofingProfileParameter )
      return p_proofingProfile.Begin();
   if ( p == TheCMSProofingIntentParameter )
      return &p_proofingIntent;
   if ( p == TheCMSUseProofingBPCParameter )
      return &p_useProofingBPC;
   if ( p == TheCMSDefaultProofingEnabledParameter )
      return &p_defaultProofingEnabled;
   if ( p == TheCMSDefaultGamutCheckEnabledParameter )
      return &p_defaultGamutCheckEnabled;
   if ( p == TheCMSGamutWarningColorParameter )
      return &p_gamutWarningColor;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool ColorManagementSetupInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheCMSDefaultRGBProfileParameter )
   {
      p_defaultRGBProfile.Clear();
      if ( sizeOrLength != 0 )
         p_defaultRGBProfile.SetLength( sizeOrLength );
   }
   else if ( p == TheCMSDefaultGrayProfileParameter )
   {
      p_defaultGrayscaleProfile.Clear();
      if ( sizeOrLength != 0 )
         p_defaultGrayscaleProfile.SetLength( sizeOrLength );
   }
   else if ( p == TheCMSProofingProfileParameter )
   {
      p_proofingProfile.Clear();
      if ( sizeOrLength != 0 )
         p_proofingProfile.SetLength( sizeOrLength );
   }
   else if ( p == TheCMSUpdateMonitorProfileParameter )
   {
      p_updateMonitorProfile.Clear();
      if ( sizeOrLength != 0 )
         p_updateMonitorProfile.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type ColorManagementSetupInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheCMSDefaultRGBProfileParameter )
      return p_defaultRGBProfile.Length();
   if ( p == TheCMSDefaultGrayProfileParameter )
      return p_defaultGrayscaleProfile.Length();
   if ( p == TheCMSProofingProfileParameter )
      return p_proofingProfile.Length();
   if ( p == TheCMSUpdateMonitorProfileParameter )
      return p_updateMonitorProfile.Length();

   return 0;
}

// ----------------------------------------------------------------------------

void ColorManagementSetupInstance::LoadCurrentSettings()
{
   p_enabled                  = PixInsightSettings::GlobalFlag( "ColorManagement/IsEnabled" );
   p_detectMonitorProfile     = PixInsightSettings::GlobalFlag( "ColorManagement/DetectMonitorProfile" );
   p_defaultRGBProfile        = ICCProfile( PixInsightSettings::GlobalString( "ColorManagement/DefaultRGBProfilePath" ) ).Description();
   p_defaultGrayscaleProfile  = ICCProfile( PixInsightSettings::GlobalString( "ColorManagement/DefaultGrayscaleProfilePath" ) ).Description();
   p_defaultRenderingIntent   = PixInsightSettings::GlobalInteger( "ColorManagement/DefaultRenderingIntent" );
   p_onProfileMismatch        = PixInsightSettings::GlobalInteger( "ColorManagement/OnProfileMismatch" );
   p_onMissingProfile         = PixInsightSettings::GlobalInteger( "ColorManagement/OnMissingProfile" );
   p_defaultEmbedProfilesInRGBImages = PixInsightSettings::GlobalFlag( "ColorManagement/DefaultEmbedProfilesInRGBImages" );
   p_defaultEmbedProfilesInGrayscaleImages = PixInsightSettings::GlobalFlag( "ColorManagement/DefaultEmbedProfilesInGrayscaleImages" );
   p_useLowResolutionCLUTs    = PixInsightSettings::GlobalFlag( "ColorManagement/UseLowResolutionCLUTs" );
   p_proofingProfile          = ICCProfile( PixInsightSettings::GlobalString( "ColorManagement/ProofingProfilePath" ) ).Description();
   p_proofingIntent           = PixInsightSettings::GlobalInteger( "ColorManagement/ProofingIntent" );
   p_useProofingBPC           = PixInsightSettings::GlobalFlag( "ColorManagement/UseProofingBPC" );
   p_defaultProofingEnabled   = PixInsightSettings::GlobalFlag( "ColorManagement/DefaultProofingEnabled" );
   p_defaultGamutCheckEnabled = PixInsightSettings::GlobalFlag( "ColorManagement/DefaultGamutCheckEnabled" );
   p_gamutWarningColor        = PixInsightSettings::GlobalColor( "ColorManagement/GamutWarningColor" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupInstance.cpp - Released 2021-04-09T19:41:48Z
