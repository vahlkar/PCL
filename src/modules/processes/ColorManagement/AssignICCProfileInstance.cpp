//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// AssignICCProfileInstance.cpp - Released 2021-04-09T19:41:48Z
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

#include "AssignICCProfileInstance.h"
#include "AssignICCProfileParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ICCProfile.h>
#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AssignICCProfileInstance::AssignICCProfileInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_mode( ICCAMode::Default )
{
}

// ----------------------------------------------------------------------------

AssignICCProfileInstance::AssignICCProfileInstance( const AssignICCProfileInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// -------------------------------------------------------------------------

void AssignICCProfileInstance::Assign( const ProcessImplementation& p )
{
   const AssignICCProfileInstance* x = dynamic_cast<const AssignICCProfileInstance*>( &p );
   if ( x != nullptr )
   {
      p_targetProfile = x->p_targetProfile;
      p_mode = x->p_mode;
   }
}

// ----------------------------------------------------------------------------

bool AssignICCProfileInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags AssignICCProfileInstance::UndoMode( const View& ) const
{
   return UndoFlag::ICCProfile;
}

// ----------------------------------------------------------------------------

bool AssignICCProfileInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "AssignICCProfile cannot be executed on previews.";
      return false;
   }

   if ( p_mode == ICCAMode::AssignNewProfile )
      if ( p_targetProfile.IsEmpty() )
      {
         whyNot = "This instance of AssignICCProfile cannot be executed because no target profile has been specified.";
         return false;
      }

   return true;
}

// ----------------------------------------------------------------------------

bool AssignICCProfileInstance::ExecuteOn( View& view )
{
   static ICCProfile::profile_list profiles; // ### N.B.: Not reentrant - not thread-safe.

   if ( !view.IsMainView() )
      return false;

   AutoViewLock lock( view );

   Console console;

   ImageWindow w = view.Window();

   if ( p_mode == ICCAMode::LeaveUntagged || p_mode == ICCAMode::AssignDefaultProfile )
   {
      ICCProfile icc;
      w.GetICCProfile( icc );

      console.Write( "<end><cbr>Previous ICC profile: " );

      if ( icc.IsProfile() )
      {
         w.DeleteICCProfile();
         console.WriteLn( "<raw>" + icc.Description() + "</raw>" );
      }
      else
         console.WriteLn( "&lt;* none *&gt;" );
   }

   if ( p_mode != ICCAMode::LeaveUntagged )
   {
      String profilePath;

      if ( p_mode == ICCAMode::AssignDefaultProfile )
         profilePath = PixInsightSettings::GlobalString( view.IsColor() ?
               "ColorManagement/DefaultRGBProfilePath" : "ColorManagement/DefaultGrayscaleProfilePath" );
      else // ICCAMode::AssignNewProfile
      {
         if ( p_targetProfile.IsEmpty() )
            throw Error( "No profile specified!" );

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

         profilePath = i->path;
      }

      ICCProfile targetICC;

      try
      {
         targetICC.Load( profilePath );
      }
      ERROR_HANDLER

      if ( !targetICC.IsProfile() )
         throw Error( "Unable to load ICC profile: " + profilePath +
                      "The disk file could be corrupted, or it is not a valid ICC profile." );

      if ( w.MainView().IsColor() && !targetICC.IsRGB() )
         throw Error( '\'' + p_targetProfile + "' is a grayscale profile. "
                      "This profile cannot be assigned to a RGB color image." );

      w.SetICCProfile( targetICC );

      console.WriteLn( "<end><cbr>Profile assigned: <raw>" + targetICC.Description() + "</raw>" );
      console.WriteLn( "Profile path: <raw>" + targetICC.FilePath() + "</raw>" );
   }

   return true;
}

// ----------------------------------------------------------------------------

void* AssignICCProfileInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheICCATargetProfileParameter )
      return p_targetProfile.Begin();
   if ( p == TheICCAModeParameter )
      return &p_mode;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool AssignICCProfileInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheICCATargetProfileParameter )
   {
      p_targetProfile.Clear();
      if ( sizeOrLength > 0 )
         p_targetProfile.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type AssignICCProfileInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheICCATargetProfileParameter )
      return p_targetProfile.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AssignICCProfileInstance.cpp - Released 2021-04-09T19:41:48Z
