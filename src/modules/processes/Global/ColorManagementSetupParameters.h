//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// ColorManagementSetupParameters.h - Released 2021-04-09T19:41:48Z
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

#ifndef __ColorManagementSetupParameters_h
#define __ColorManagementSetupParameters_h

#include <pcl/ICCProfileTransformation.h>
#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CMSRenderingIntent : public MetaEnumeration
{
public:

   enum { Perceptual = ICCRenderingIntent::Perceptual,
          Saturation = ICCRenderingIntent::Saturation,
          RelativeColorimetric = ICCRenderingIntent::RelativeColorimetric,
          AbsoluteColorimetric = ICCRenderingIntent::AbsoluteColorimetric,
          NumberOfRenderingIntents,
          DefaultForScreen = Perceptual,
          DefaultForProofing = RelativeColorimetric };

   CMSRenderingIntent( MetaProcess* );

   virtual IsoString Id() const = 0;

   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
};

// ----------------------------------------------------------------------------

class CMSEnabled : public MetaBoolean
{
public:

   CMSEnabled( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSEnabled* TheCMSEnabledParameter;

// ----------------------------------------------------------------------------

class CMSDetectMonitorProfile : public MetaBoolean
{
public:

   CMSDetectMonitorProfile( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSDetectMonitorProfile* TheCMSDetectMonitorProfileParameter;

// ----------------------------------------------------------------------------

class CMSUpdateMonitorProfile : public MetaString
{
public:

   CMSUpdateMonitorProfile( MetaProcess* );

   IsoString Id() const override;
};

extern CMSUpdateMonitorProfile* TheCMSUpdateMonitorProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultRGBProfile : public MetaString
{
public:

   CMSDefaultRGBProfile( MetaProcess* );

   IsoString Id() const override;
};

extern CMSDefaultRGBProfile* TheCMSDefaultRGBProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultGrayProfile : public MetaString
{
public:

   CMSDefaultGrayProfile( MetaProcess* );

   IsoString Id() const override;
};

extern CMSDefaultGrayProfile* TheCMSDefaultGrayProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultRenderingIntent : public CMSRenderingIntent
{
public:

   CMSDefaultRenderingIntent( MetaProcess* );

   IsoString Id() const override;
   size_type DefaultValueIndex() const override;
};

extern CMSDefaultRenderingIntent* TheCMSDefaultRenderingIntentParameter;

// ----------------------------------------------------------------------------

class CMSOnProfileMismatch : public MetaEnumeration
{
public:

   enum { AskUser,
          KeepEmbedded,
          ConvertToDefault,
          DiscardEmbedded,
          DisableCM,
          NumberOfOnProfileMismatchPolicies,
          Default = KeepEmbedded };

   CMSOnProfileMismatch( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern CMSOnProfileMismatch* TheCMSOnProfileMismatchParameter;

// ----------------------------------------------------------------------------

class CMSOnMissingProfile : public MetaEnumeration
{
public:

   enum { AskUser,
          AssignDefault,
          LeaveUntagged,
          DisableCM,
          NumberOfOnMissingProfilePolicies,
          Default = LeaveUntagged };

   CMSOnMissingProfile( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern CMSOnMissingProfile* TheCMSOnMissingProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultEmbedProfilesInRGBImages : public MetaBoolean
{
public:

   CMSDefaultEmbedProfilesInRGBImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSDefaultEmbedProfilesInRGBImages* TheCMSDefaultEmbedProfilesInRGBImagesParameter;

// ----------------------------------------------------------------------------

class CMSDefaultEmbedProfilesInGrayscaleImages : public MetaBoolean
{
public:

   CMSDefaultEmbedProfilesInGrayscaleImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSDefaultEmbedProfilesInGrayscaleImages* TheCMSDefaultEmbedProfilesInGrayscaleImagesParameter;

// ----------------------------------------------------------------------------

class CMSUseLowResolutionCLUTs : public MetaBoolean
{
public:

   CMSUseLowResolutionCLUTs( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSUseLowResolutionCLUTs* TheCMSUseLowResolutionCLUTsParameter;

// ----------------------------------------------------------------------------

class CMSProofingProfile : public MetaString
{
public:

   CMSProofingProfile( MetaProcess* );

   IsoString Id() const override;
};

extern CMSProofingProfile* TheCMSProofingProfileParameter;

// ----------------------------------------------------------------------------

class CMSProofingIntent : public CMSRenderingIntent
{
public:

   CMSProofingIntent( MetaProcess* );

   IsoString Id() const override;
   size_type DefaultValueIndex() const override;
};

extern CMSProofingIntent* TheCMSProofingIntentParameter;

// ----------------------------------------------------------------------------

class CMSUseProofingBPC : public MetaBoolean
{
public:

   CMSUseProofingBPC( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSUseProofingBPC* TheCMSUseProofingBPCParameter;

// ----------------------------------------------------------------------------

class CMSDefaultProofingEnabled : public MetaBoolean
{
public:

   CMSDefaultProofingEnabled( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSDefaultProofingEnabled* TheCMSDefaultProofingEnabledParameter;

// ----------------------------------------------------------------------------

class CMSDefaultGamutCheckEnabled : public MetaBoolean
{
public:

   CMSDefaultGamutCheckEnabled( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSDefaultGamutCheckEnabled* TheCMSDefaultGamutCheckEnabledParameter;

// ----------------------------------------------------------------------------

class CMSGamutWarningColor : public MetaUInt32
{
public:

   CMSGamutWarningColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern CMSGamutWarningColor* TheCMSGamutWarningColorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ColorManagementSetupParameters_h

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupParameters.h - Released 2021-04-09T19:41:48Z
