//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIDeviceControllerParameters.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2023 Klaus Kretzschmar
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

#ifndef __INDIDeviceControllerParameters_h
#define __INDIDeviceControllerParameters_h

#include <pcl/MetaParameter.h>

#include "INDIParamListTypes.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class IDCServerHostName : public MetaString
{
public:

   IDCServerHostName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern IDCServerHostName* TheIDCServerHostNameParameter;

// ----------------------------------------------------------------------------

class IDCServerPort : public MetaUInt32
{
public:

   IDCServerPort( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IDCServerPort* TheIDCServerPortParameter;

// ----------------------------------------------------------------------------

class IDCServerConnect : public MetaBoolean
{
public:

   IDCServerConnect( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IDCServerConnect* TheIDCServerConnectParameter;

// ----------------------------------------------------------------------------

class IDCServerCommand : public MetaString
{
public:

   IDCServerCommand( MetaProcess* );

   IsoString Id() const override;
};

extern IDCServerCommand* TheIDCServerCommandParameter;

// ----------------------------------------------------------------------------

class IDCAbort : public MetaBoolean
{
public:

   IDCAbort( MetaProcess* );

   IsoString Id() const override;
};

extern IDCAbort* TheIDCAbortParameter;

// ----------------------------------------------------------------------------

class IDCVerbosity : public MetaInt32
{
public:

   IDCVerbosity( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern IDCVerbosity* TheIDCVerbosityParameter;

// ----------------------------------------------------------------------------

class IDCGetCommandParameters : public MetaString
{
public:

   IDCGetCommandParameters( MetaProcess* );

   IsoString Id() const override;
};

extern IDCGetCommandParameters* TheIDCGetCommandParametersParameter;

// ----------------------------------------------------------------------------

class IDCGetCommandResult : public MetaString
{
public:

   IDCGetCommandResult( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCGetCommandResult* TheIDCGetCommandResultParameter;

// ----------------------------------------------------------------------------

class IDCDevices : public MetaTable
{
public:

   IDCDevices( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCDevices* TheIDCDevicesParameter;

// ----------------------------------------------------------------------------

class IDCDeviceName : public MetaString
{
public:

   IDCDeviceName( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCDeviceName* TheIDCDeviceNameParameter;

// ----------------------------------------------------------------------------

class IDCDeviceLabel : public MetaString
{
public:

   IDCDeviceLabel( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCDeviceLabel* TheIDCDeviceLabelParameter;

// ----------------------------------------------------------------------------

class IDCProperties : public MetaTable
{
public:

   IDCProperties( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCProperties* TheIDCPropertiesParameter;

// ----------------------------------------------------------------------------

class IDCPropertyName : public MetaString
{
public:

   IDCPropertyName( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCPropertyName* TheIDCPropertyNameParameter;

// ----------------------------------------------------------------------------

class IDCPropertyValue : public MetaString
{
public:

   IDCPropertyValue( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCPropertyValue* TheIDCPropertyValueParameter;

// ----------------------------------------------------------------------------

class IDCPropertyType : public MetaString
{
public:

   IDCPropertyType( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCPropertyType* TheIDCPropertyTypeParameter;

// ----------------------------------------------------------------------------

class IDCPropertyState : public MetaUInt32
{
public:

   IDCPropertyState( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern IDCPropertyState* TheIDCPropertyStateParameter;

// ----------------------------------------------------------------------------

class IDCNewProperties : public MetaTable
{
public:

   IDCNewProperties( MetaProcess* );

   IsoString Id() const override;
};

extern IDCNewProperties* TheIDCNewPropertiesParameter;

// ----------------------------------------------------------------------------

class IDCNewPropertyKey : public MetaString
{
public:

   IDCNewPropertyKey( MetaTable* );

   IsoString Id() const override;
};

extern IDCNewPropertyKey* TheIDCNewPropertyKeyParameter;

// ----------------------------------------------------------------------------

class IDCNewPropertyType : public MetaString
{
public:

   IDCNewPropertyType( MetaTable* );

   IsoString Id() const override;
};

extern IDCNewPropertyType* TheIDCNewPropertyTypeParameter;

// ----------------------------------------------------------------------------

class IDCNewPropertyValue : public MetaString
{
public:

   IDCNewPropertyValue( MetaTable* );

   IsoString Id() const override;
};

extern IDCNewPropertyValue* TheIDCNewPropertyValueParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif // INDIDeviceControllerParameters_h

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerParameters.h - Released 2023-08-28T15:23:41Z
