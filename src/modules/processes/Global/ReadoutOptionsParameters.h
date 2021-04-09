//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// ReadoutOptionsParameters.h - Released 2021-04-09T19:41:48Z
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

#ifndef __ReadoutOptionsParameters_h
#define __ReadoutOptionsParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ReadoutOptionsData : public MetaEnumeration
{
public:

   // This enumeration corresponds to pcl::ReadoutData (see pcl/ReadoutOptions.h)

   ReadoutOptionsData( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ReadoutOptionsData* TheReadoutOptionsDataParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsMode : public MetaEnumeration
{
public:

   // This enumeration corresponds to pcl::ReadoutMode (see pcl/ReadoutOptions.h)

   ReadoutOptionsMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ReadoutOptionsMode* TheReadoutOptionsModeParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsProbeSize : public MetaInt32
{
public:

   ReadoutOptionsProbeSize( MetaProcess* );

   IsoString Id() const override;
   bool NeedsValidation() const override; // to ensure an odd value
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutOptionsProbeSize* TheReadoutOptionsProbeSizeParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsPreviewSize : public MetaInt32
{
public:

   ReadoutOptionsPreviewSize( MetaProcess* );

   IsoString Id() const override;
   bool NeedsValidation() const override; // to ensure an odd value
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutOptionsPreviewSize* TheReadoutOptionsPreviewSizeParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsPreviewZoomFactor : public MetaInt32
{
public:

   ReadoutOptionsPreviewZoomFactor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutOptionsPreviewZoomFactor* TheReadoutOptionsPreviewZoomFactorParameter;

// ----------------------------------------------------------------------------

class ReadoutRealPrecision : public MetaInt32
{
public:

   ReadoutRealPrecision( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutRealPrecision* TheReadoutRealPrecisionParameter;

// ----------------------------------------------------------------------------

class ReadoutIntegerRange : public MetaUInt32
{
public:

   ReadoutIntegerRange( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutIntegerRange* TheReadoutIntegerRangeParameter;

// ----------------------------------------------------------------------------

class ReadoutAlpha : public MetaBoolean
{
public:

   ReadoutAlpha( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutAlpha* TheReadoutAlphaParameter;

// ----------------------------------------------------------------------------

class ReadoutMask : public MetaBoolean
{
public:

   ReadoutMask( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutMask* TheReadoutMaskParameter;

// ----------------------------------------------------------------------------

class ReadoutPreview : public MetaBoolean
{
public:

   ReadoutPreview( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutPreview* TheReadoutPreviewParameter;

// ----------------------------------------------------------------------------

class ReadoutPreviewCenter : public MetaBoolean
{
public:

   ReadoutPreviewCenter( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutPreviewCenter* TheReadoutPreviewCenterParameter;

// ----------------------------------------------------------------------------

class ReadoutShowEquatorial : public MetaBoolean
{
public:

   ReadoutShowEquatorial( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutShowEquatorial* TheReadoutShowEquatorialParameter;

// ----------------------------------------------------------------------------

class ReadoutShowEcliptic : public MetaBoolean
{
public:

   ReadoutShowEcliptic( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutShowEcliptic* TheReadoutShowEclipticParameter;

// ----------------------------------------------------------------------------

class ReadoutShowGalactic : public MetaBoolean
{
public:

   ReadoutShowGalactic( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutShowGalactic* TheReadoutShowGalacticParameter;

// ----------------------------------------------------------------------------

class ReadoutCoordinateItems : public MetaInt32
{
public:

   ReadoutCoordinateItems( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutCoordinateItems* TheReadoutCoordinateItemsParameter;

// ----------------------------------------------------------------------------

class ReadoutCoordinatePrecision : public MetaInt32
{
public:

   ReadoutCoordinatePrecision( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutCoordinatePrecision* TheReadoutCoordinatePrecisionParameter;

// ----------------------------------------------------------------------------

class ReadoutBroadcast : public MetaBoolean
{
public:

   ReadoutBroadcast( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutBroadcast* TheReadoutBroadcastParameter;

// ----------------------------------------------------------------------------

class ReadoutReal : public MetaBoolean
{
public:

   ReadoutReal( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutReal* TheReadoutRealParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ReadoutOptionsParameters_h

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsParameters.h - Released 2021-04-09T19:41:48Z
