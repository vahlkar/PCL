//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 1.2.5
// ----------------------------------------------------------------------------
// CosmeticCorrectionParameters.h - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2020 Nikolay Volkov
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L.
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

#ifndef __CosmeticCorrectionParameters_h
#define __CosmeticCorrectionParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CCTargetFrames : public MetaTable
{
public:

   CCTargetFrames( MetaProcess* );
   IsoString Id() const override;
};

class CCTargetFrameEnabled : public MetaBoolean
{
public:

   CCTargetFrameEnabled( MetaTable* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCTargetFramePath : public MetaString
{
public:

   CCTargetFramePath( MetaTable* );
   IsoString Id() const override;
};

class CCOutputDir : public MetaString
{
public:

   CCOutputDir( MetaProcess* );
   IsoString Id() const override;
};

class CCOutputExtension : public MetaString
{
public:

   CCOutputExtension( MetaProcess* );
   IsoString Id() const override;
   String DefaultValue() const override;
};

class CCPrefix : public MetaString
{
public:

   CCPrefix( MetaProcess* );
   IsoString Id() const override;
   String DefaultValue() const override;
};

class CCPostfix : public MetaString
{
public:

   CCPostfix( MetaProcess* );
   IsoString Id() const override;
   String DefaultValue() const override;
};

class CCOverwrite : public MetaBoolean
{
public:

   CCOverwrite( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};
extern CCOverwrite* TheOverwrite;

class CCCFA : public MetaBoolean
{
public:

   CCCFA( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};
class CCAmount : public MetaFloat
{
public:

   CCAmount( MetaProcess* );
   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// Via Master Dark -----------------------------
class CCUseMasterDark : public MetaBoolean
{
public:

   CCUseMasterDark( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCMasterDarkPath : public MetaString
{
public:

   CCMasterDarkPath( MetaProcess* );
   IsoString Id() const override;
};

class CCHotDarkCheck : public MetaBoolean
{
public:

   CCHotDarkCheck( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCHotLevel : public MetaFloat
{
public:

   CCHotLevel( MetaProcess* );
   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

class CCColdDarkCheck : public MetaBoolean
{
public:

   CCColdDarkCheck( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCColdLevel : public MetaFloat
{
public:

   CCColdLevel( MetaProcess* );
   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// Via Auto Detect -----------------------------
class CCUseAutoDetect : public MetaBoolean
{
public:

   CCUseAutoDetect( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCHotAutoValue : public MetaFloat
{
public:

   CCHotAutoValue( MetaProcess* );
   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

class CCHotAutoCheck : public MetaBoolean
{
public:

   CCHotAutoCheck( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCColdAutoValue : public MetaFloat
{
public:

   CCColdAutoValue( MetaProcess* );
   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

class CCColdAutoCheck : public MetaBoolean
{
public:

   CCColdAutoCheck( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

// Via Defect List -----------------------------
class CCUseDefectList : public MetaBoolean
{
public:

   CCUseDefectList( MetaProcess* );
   IsoString Id() const override;
};

class CCDefects : public MetaTable
{
public:

   CCDefects( MetaProcess* );
   IsoString Id() const override;
};

class CCDefectEnabled : public MetaBoolean
{
public:

   CCDefectEnabled( MetaTable* );
   IsoString Id() const override;
};

class CCDefectIsRow : public MetaBoolean
{
public:

   CCDefectIsRow( MetaTable* );
   IsoString Id() const override;
};

class CCDefectAddress : public MetaUInt16
{
public:

   CCDefectAddress( MetaTable* );
   IsoString Id() const override;
};

class CCDefectIsRange : public MetaBoolean
{
public:

   CCDefectIsRange( MetaTable* );
   IsoString Id() const override;
};

class CCDefectBegin : public MetaUInt16
{
public:

   CCDefectBegin( MetaTable* );
   IsoString Id() const override;
};

class CCDefectEnd : public MetaUInt16
{
public:

   CCDefectEnd( MetaTable* );
   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

extern CCTargetFrames* TheTargetFrames;
extern CCTargetFrameEnabled* TheTargetFrameEnabled;
extern CCTargetFramePath* TheTargetFramePath;

extern CCOutputDir* TheOutputDir;
extern CCOutputExtension* TheOutputExtension;
extern CCPrefix* ThePrefix;
extern CCPostfix* ThePostfix;

extern CCCFA* TheCFA;
extern CCAmount* TheAmount;

extern CCUseMasterDark* TheUseMasterDark;
extern CCMasterDarkPath* TheMasterPath;
extern CCHotDarkCheck* TheHotDarkCheck;
extern CCHotLevel* TheHotLevel;
extern CCColdDarkCheck* TheColdDarkCheck;
extern CCColdLevel* TheColdLevel;

extern CCUseAutoDetect* TheUseAutoDetect;
extern CCHotAutoCheck* TheHotAutoCheck;
extern CCHotAutoValue* TheHotAutoValue;
extern CCColdAutoCheck* TheColdAutoCheck;
extern CCColdAutoValue* TheColdAutoValue;

extern CCUseDefectList* TheUseDefectList;
extern CCDefects* TheDefects;
extern CCDefectEnabled* TheDefectEnabled;
extern CCDefectIsRow* TheDefectIsRow;
extern CCDefectAddress* TheDefectAddress;
extern CCDefectIsRange* TheDefectIsRange;
extern CCDefectBegin* TheDefectBegin;
extern CCDefectEnd* TheDefectEnd;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif // __CosmeticCorrectionParameters_h

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionParameters.h - Released 2021-04-09T19:41:49Z
