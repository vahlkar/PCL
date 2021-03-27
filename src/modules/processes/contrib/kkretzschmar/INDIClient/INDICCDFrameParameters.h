//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.7
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.1
// ----------------------------------------------------------------------------
// INDICCDFrameParameters.h - Released 2020-12-17T15:46:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2020 Klaus Kretzschmar
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

#ifndef __INDICCDFrameParameters_h
#define __INDICCDFrameParameters_h

#include <pcl/MetaParameter.h>

#include "ModuleParameterRegistration.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ICFDeviceName : public MetaString
{
public:

   ICFDeviceName( MetaProcess* );

   IsoString Id() const override;
};

extern ICFDeviceName* TheICFDeviceNameParameter;

// ----------------------------------------------------------------------------

class ICFUploadMode : public MetaEnumeration
{
public:

   enum
   {
      UploadClient,
      UploadServer,
      UploadServerAndClient,
      NumberOfUploadModes,
      Default = UploadClient
   };

   ICFUploadMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICFUploadMode* TheICFUploadModeParameter;

// ----------------------------------------------------------------------------

class ICFServerUploadDirectory : public MetaString
{
public:

   ICFServerUploadDirectory( MetaProcess* );

   IsoString Id() const override;
};

extern ICFServerUploadDirectory* TheICFServerUploadDirectoryParameter;

// ----------------------------------------------------------------------------

class ICFServerFileNameTemplate : public MetaString
{
public:

   ICFServerFileNameTemplate( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFServerFileNameTemplate* TheICFServerFileNameTemplateParameter;

// ----------------------------------------------------------------------------

class ICFFrameType : public MetaEnumeration
{
public:

   enum
   {
      LightFrame,
      BiasFrame,
      DarkFrame,
      FlatFrame,
      NumberOfFrameTypes,
      Default = LightFrame
   };

   ICFFrameType( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICFFrameType* TheICFFrameTypeParameter;

// ----------------------------------------------------------------------------

class ICFBinningX : public MetaInt32
{
public:

   ICFBinningX( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFBinningX* TheICFBinningXParameter;

// ----------------------------------------------------------------------------

class ICFBinningY : public MetaInt32
{
public:

   ICFBinningY( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFBinningY* TheICFBinningYParameter;

// ----------------------------------------------------------------------------

class ICFFilterSlot : public MetaInt32
{
public:

   ICFFilterSlot( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFFilterSlot* TheICFFilterSlotParameter;

// ----------------------------------------------------------------------------

class ICFExposureTime : public MetaDouble
{
public:

   ICFExposureTime( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFExposureTime* TheICFExposureTimeParameter;

// ----------------------------------------------------------------------------

class ICFExposureDelay : public MetaDouble
{
public:

   ICFExposureDelay( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFExposureDelay* TheICFExposureDelayParameter;

// ----------------------------------------------------------------------------

class ICFExposureCount : public MetaInt32
{
public:

   ICFExposureCount( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFExposureCount* TheICFExposureCountParameter;

// ----------------------------------------------------------------------------

class ICFOpenClientImages : public MetaBoolean
{
public:

   ICFOpenClientImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFOpenClientImages* TheICFOpenClientImagesParameter;

// ----------------------------------------------------------------------------

class ICFNewImageIdTemplate : public MetaString
{
public:

   ICFNewImageIdTemplate( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFNewImageIdTemplate* TheICFNewImageIdTemplateParameter;

// ----------------------------------------------------------------------------

class ICFReuseImageWindow : public MetaBoolean
{
public:

   ICFReuseImageWindow( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFReuseImageWindow* TheICFReuseImageWindowParameter;

// ----------------------------------------------------------------------------

class ICFAutoStretch : public MetaBoolean
{
public:

   ICFAutoStretch( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFAutoStretch* TheICFAutoStretchParameter;

// ----------------------------------------------------------------------------

class ICFLinkedAutoStretch : public MetaBoolean
{
public:

   ICFLinkedAutoStretch( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFLinkedAutoStretch* TheICFLinkedAutoStretchParameter;

// ----------------------------------------------------------------------------

class ICFSaveClientImages : public MetaBoolean
{
public:

   ICFSaveClientImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFSaveClientImages* TheICFSaveClientImagesParameter;

// ----------------------------------------------------------------------------

class ICFOverwriteClientImages : public MetaBoolean
{
public:

   ICFOverwriteClientImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFOverwriteClientImages* TheICFOverwriteClientImagesParameter;

// ----------------------------------------------------------------------------

class ICFClientDownloadDirectory : public MetaString
{
public:

   ICFClientDownloadDirectory( MetaProcess* );

   IsoString Id() const override;
};

extern ICFClientDownloadDirectory* TheICFClientDownloadDirectoryParameter;

// ----------------------------------------------------------------------------

class ICFClientFileNameTemplate : public MetaString
{
public:

   ICFClientFileNameTemplate( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFClientFileNameTemplate* TheICFClientFileNameTemplateParameter;

// ----------------------------------------------------------------------------

class ICFClientOutputFormatHints : public MetaString
{
public:

   ICFClientOutputFormatHints( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFClientOutputFormatHints* TheICFClientOutputFormatHintsParameter;

// ----------------------------------------------------------------------------

class ICFObjectName : public MetaString
{
public:

   ICFObjectName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFObjectName* TheICFObjectNameParameter;

// ----------------------------------------------------------------------------

class ICFTelescopeSelection : public MetaEnumeration
{
public:

   enum
   {
      NoTelescope,
      ActiveTelescope,
      MountControllerTelescope,
      MountControllerOrActiveTelescope,
      TelescopeDeviceName,
      NumberOfTelescopeSelections,
      Default = MountControllerOrActiveTelescope
   };

   ICFTelescopeSelection( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICFTelescopeSelection* TheICFTelescopeSelectionParameter;

// ----------------------------------------------------------------------------

class ICFRequireSelectedTelescope : public MetaBoolean
{
public:

   ICFRequireSelectedTelescope( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFRequireSelectedTelescope* TheICFRequireSelectedTelescopeParameter;

// ----------------------------------------------------------------------------

class ICFTelescopeDeviceName : public MetaString
{
public:

   ICFTelescopeDeviceName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFTelescopeDeviceName* TheICFTelescopeDeviceNameParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class ICFClientFrames : public MetaTable
{
public:

   ICFClientFrames( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICFClientFrames* TheICFClientFramesParameter;

// ----------------------------------------------------------------------------

class ICFClientViewId : public MetaString
{
public:

   ICFClientViewId( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICFClientViewId* TheICFClientViewIdParameter;

// ----------------------------------------------------------------------------

class ICFClientFilePath : public MetaString
{
public:

   ICFClientFilePath( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICFClientFilePath* TheICFClientFilePathParameter;

// ----------------------------------------------------------------------------

class ICFServerFrames : public MetaTable
{
public:

   ICFServerFrames( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICFServerFrames* TheICFServerFramesParameter;

// ----------------------------------------------------------------------------

class ICFServerFrame : public MetaString
{
public:

   ICFServerFrame( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICFServerFrame* TheICFServerFrameParameter;

// ----------------------------------------------------------------------------

DECLARE_STRING_PARAMETER_CLASS( ICFExternalFilterWheelDeviceName );

// ----------------------------------------------------------------------------

DECLARE_BOOLEAN_PARAMETER_CLASS( ICFEnableAlignmentCorrection );
DECLARE_STRING_PARAMETER_CLASS( ICFAlignmentFile );

// ----------------------------------------------------------------------------

DECLARE_INT32_PARAMETER_CLASS( ICFTelescopeFocalLength );

// ----------------------------------------------------------------------------

DECLARE_BOOLEAN_PARAMETER_CLASS( ICFApplyPlateSolver );
DECLARE_BOOLEAN_PARAMETER_CLASS( ICFCenterTarget );
DECLARE_STRING_PARAMETER_WITH_DEFAULT_CLASS( ICFServerURL );
DECLARE_STRING_PARAMETER_WITH_DEFAULT_CLASS( ICFSolverCatalogName );
DECLARE_BOOLEAN_PARAMETER_CLASS( ICFSolverAutoCatalog );
DECLARE_INT32_PARAMETER_CLASS( ICFSolverLimitMagnitude );
DECLARE_BOOLEAN_PARAMETER_CLASS( ICFSolverAutoLimitMagnitude );
DECLARE_FLOAT_PARAMETER_CLASS(ICFSolverStarSensitivity); 
DECLARE_INT32_PARAMETER_CLASS(ICFSolverNoiseLayers); 
DECLARE_ENUM_PARAMETER(ICFSolverAlignmentDevice, ENUM_ITEM(TriangleSimilarity) ENUM_ITEM(PolygonMatching), TriangleSimilarity);
DECLARE_BOOLEAN_PARAMETER_CLASS( ICFSolverDistortionCorrection );
DECLARE_FLOAT_PARAMETER_CLASS(ICFSolverSplineSmoothing); 
DECLARE_ENUM_PARAMETER(ICFSolverProjection, ENUM_ITEM(Gnomonic) ENUM_ITEM(Stereographic) ENUM_ITEM(PlateCarree) ENUM_ITEM(Mercator) ENUM_ITEM(HammerAitoff) ENUM_ITEM(ZenithalEqualArea) ENUM_ITEM(Orthographic), Gnomonic);

PCL_END_LOCAL

} // namespace pcl

#endif // __INDICCDFrameParameters_h

// ----------------------------------------------------------------------------
// EOF INDICCDFrameParameters.h - Released 2020-12-17T15:46:56Z
