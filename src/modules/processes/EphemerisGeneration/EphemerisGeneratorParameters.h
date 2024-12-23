//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// EphemerisGeneratorParameters.h - Released 2024-12-23T11:33:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
//
// Copyright (c) 2003-2024 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __EphemerisGeneratorParameters_h
#define __EphemerisGeneratorParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class EGWorkingMode : public MetaEnumeration
{
public:

   enum { StateVectors,
          OrbitalElements,
          DatabaseObjects,
          NumberOfItems,
          Default = OrbitalElements };

   EGWorkingMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern EGWorkingMode* TheEGWorkingModeParameter;

// ----------------------------------------------------------------------------

class EGPosX : public MetaDouble
{
public:

   EGPosX( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGPosX* TheEGPosXParameter;

// ----------------------------------------------------------------------------

class EGPosY : public MetaDouble
{
public:

   EGPosY( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGPosY* TheEGPosYParameter;

// ----------------------------------------------------------------------------

class EGPosZ : public MetaDouble
{
public:

   EGPosZ( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGPosZ* TheEGPosZParameter;

// ----------------------------------------------------------------------------

class EGVelX : public MetaDouble
{
public:

   EGVelX( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGVelX* TheEGVelXParameter;

// ----------------------------------------------------------------------------

class EGVelY : public MetaDouble
{
public:

   EGVelY( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGVelY* TheEGVelYParameter;

// ----------------------------------------------------------------------------

class EGVelZ : public MetaDouble
{
public:

   EGVelZ( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGVelZ* TheEGVelZParameter;

// ----------------------------------------------------------------------------

class EGElemA : public MetaDouble
{
public:

   EGElemA( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGElemA* TheEGElemAParameter;

// ----------------------------------------------------------------------------

class EGElemQ : public MetaDouble
{
public:

   EGElemQ( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGElemQ* TheEGElemQParameter;

// ----------------------------------------------------------------------------

class EGElemE : public MetaDouble
{
public:

   EGElemE( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGElemE* TheEGElemEParameter;

// ----------------------------------------------------------------------------

class EGElemM : public MetaDouble
{
public:

   EGElemM( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGElemM* TheEGElemMParameter;

// ----------------------------------------------------------------------------

class EGElemT : public MetaDouble
{
public:

   EGElemT( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGElemT* TheEGElemTParameter;

// ----------------------------------------------------------------------------

class EGElemI : public MetaDouble
{
public:

   EGElemI( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGElemI* TheEGElemIParameter;

// ----------------------------------------------------------------------------

class EGElemO : public MetaDouble
{
public:

   EGElemO( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGElemO* TheEGElemOParameter;

// ----------------------------------------------------------------------------

class EGElemW : public MetaDouble
{
public:

   EGElemW( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGElemW* TheEGElemWParameter;

// ----------------------------------------------------------------------------

class EGEpochJD : public MetaDouble
{
public:

   EGEpochJD( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGEpochJD* TheEGEpochJDParameter;

// ----------------------------------------------------------------------------

class EGObjectId : public MetaString
{
public:

   EGObjectId( MetaProcess* );

   IsoString Id() const override;
};

extern EGObjectId* TheEGObjectIdParameter;

// ----------------------------------------------------------------------------

class EGObjectName : public MetaString
{
public:

   EGObjectName( MetaProcess* );

   IsoString Id() const override;
};

extern EGObjectName* TheEGObjectNameParameter;

// ----------------------------------------------------------------------------

class EGHDefined : public MetaBoolean
{
public:

   EGHDefined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGHDefined* TheEGHDefinedParameter;

// ----------------------------------------------------------------------------

class EGH : public MetaFloat
{
public:

   EGH( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGH* TheEGHParameter;


// ----------------------------------------------------------------------------

class EGGDefined : public MetaBoolean
{
public:

   EGGDefined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGGDefined* TheEGGDefinedParameter;

// ----------------------------------------------------------------------------

class EGG : public MetaFloat
{
public:

   EGG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGG* TheEGGParameter;

// ----------------------------------------------------------------------------

class EGM1Defined : public MetaBoolean
{
public:

   EGM1Defined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGM1Defined* TheEGM1DefinedParameter;

// ----------------------------------------------------------------------------

class EGM1 : public MetaFloat
{
public:

   EGM1( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGM1* TheEGM1Parameter;

// ----------------------------------------------------------------------------

class EGK1Defined : public MetaBoolean
{
public:

   EGK1Defined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGK1Defined* TheEGK1DefinedParameter;

// ----------------------------------------------------------------------------

class EGK1 : public MetaFloat
{
public:

   EGK1( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGK1* TheEGK1Parameter;

// ----------------------------------------------------------------------------

class EGM2Defined : public MetaBoolean
{
public:

   EGM2Defined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGM2Defined* TheEGM2DefinedParameter;

// ----------------------------------------------------------------------------

class EGM2 : public MetaFloat
{
public:

   EGM2( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGM2* TheEGM2Parameter;

// ----------------------------------------------------------------------------

class EGK2Defined : public MetaBoolean
{
public:

   EGK2Defined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGK2Defined* TheEGK2DefinedParameter;

// ----------------------------------------------------------------------------

class EGK2 : public MetaFloat
{
public:

   EGK2( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGK2* TheEGK2Parameter;

// ----------------------------------------------------------------------------

class EGPCDefined : public MetaBoolean
{
public:

   EGPCDefined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGPCDefined* TheEGPCDefinedParameter;

// ----------------------------------------------------------------------------

class EGPC : public MetaFloat
{
public:

   EGPC( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGPC* TheEGPCParameter;

// ----------------------------------------------------------------------------

class EGB_VDefined : public MetaBoolean
{
public:

   EGB_VDefined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGB_VDefined* TheEGB_VDefinedParameter;

// ----------------------------------------------------------------------------

class EGB_V : public MetaFloat
{
public:

   EGB_V( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGB_V* TheEGB_VParameter;

// ----------------------------------------------------------------------------

class EGU_BDefined : public MetaBoolean
{
public:

   EGU_BDefined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGU_BDefined* TheEGU_BDefinedParameter;

// ----------------------------------------------------------------------------

class EGU_B : public MetaFloat
{
public:

   EGU_B( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGU_B* TheEGU_BParameter;

// ----------------------------------------------------------------------------

class EGI_RDefined : public MetaBoolean
{
public:

   EGI_RDefined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGI_RDefined* TheEGI_RDefinedParameter;

// ----------------------------------------------------------------------------

class EGI_R : public MetaFloat
{
public:

   EGI_R( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGI_R* TheEGI_RParameter;

// ----------------------------------------------------------------------------

class EGDDefined : public MetaBoolean
{
public:

   EGDDefined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGDDefined* TheEGDDefinedParameter;

// ----------------------------------------------------------------------------

class EGD : public MetaFloat
{
public:

   EGD( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGD* TheEGDParameter;

// ----------------------------------------------------------------------------

class EGA1Defined : public MetaBoolean
{
public:

   EGA1Defined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGA1Defined* TheEGA1DefinedParameter;

// ----------------------------------------------------------------------------

class EGA1 : public MetaFloat
{
public:

   EGA1( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   bool ScientificNotation() const override;
};

extern EGA1* TheEGA1Parameter;

// ----------------------------------------------------------------------------

class EGA2Defined : public MetaBoolean
{
public:

   EGA2Defined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGA2Defined* TheEGA2DefinedParameter;

// ----------------------------------------------------------------------------

class EGA2 : public MetaFloat
{
public:

   EGA2( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   bool ScientificNotation() const override;
};

extern EGA2* TheEGA2Parameter;

// ----------------------------------------------------------------------------

class EGA3Defined : public MetaBoolean
{
public:

   EGA3Defined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGA3Defined* TheEGA3DefinedParameter;

// ----------------------------------------------------------------------------

class EGA3 : public MetaFloat
{
public:

   EGA3( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   bool ScientificNotation() const override;
};

extern EGA3* TheEGA3Parameter;

// ----------------------------------------------------------------------------

class EGDTDefined : public MetaBoolean
{
public:

   EGDTDefined( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGDTDefined* TheEGDTDefinedParameter;

// ----------------------------------------------------------------------------

class EGDT : public MetaFloat
{
public:

   EGDT( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGDT* TheEGDTParameter;

// ----------------------------------------------------------------------------

class EGDatabaseFilePath : public MetaString
{
public:

   EGDatabaseFilePath( MetaProcess* );

   IsoString Id() const override;
};

extern EGDatabaseFilePath* TheEGDatabaseFilePathParameter;

// ----------------------------------------------------------------------------

class EGDatabaseFormatName : public MetaString
{
public:

   EGDatabaseFormatName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern EGDatabaseFormatName* TheEGDatabaseFormatNameParameter;

// ----------------------------------------------------------------------------

class EGObjects : public MetaString
{
public:

   EGObjects( MetaProcess* );

   IsoString Id() const override;
};

extern EGObjects* TheEGObjectsParameter;

// ----------------------------------------------------------------------------

class EGUseRegularExpressions : public MetaBoolean
{
public:

   EGUseRegularExpressions( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGUseRegularExpressions* TheEGUseRegularExpressionsParameter;

// ----------------------------------------------------------------------------

class EGExcludeCoreAsteroids : public MetaBoolean
{
public:

   EGExcludeCoreAsteroids( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGExcludeCoreAsteroids* TheEGExcludeCoreAsteroidsParameter;

// ----------------------------------------------------------------------------

class EGFundamentalFilePath : public MetaString
{
public:

   EGFundamentalFilePath( MetaProcess* );

   IsoString Id() const override;
};

extern EGFundamentalFilePath* TheEGFundamentalFilePathParameter;

// ----------------------------------------------------------------------------

class EGAsteroidsFilePath : public MetaString
{
public:

   EGAsteroidsFilePath( MetaProcess* );

   IsoString Id() const override;
};

extern EGAsteroidsFilePath* TheEGAsteroidsFilePathParameter;

// ----------------------------------------------------------------------------

class EGKBOsFilePath : public MetaString
{
public:

   EGKBOsFilePath( MetaProcess* );

   IsoString Id() const override;
};

extern EGKBOsFilePath* TheEGKBOsFilePathParameter;

// ----------------------------------------------------------------------------

class EGStartTimeJD : public MetaDouble
{
public:

   EGStartTimeJD( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGStartTimeJD* TheEGStartTimeJDParameter;

// ----------------------------------------------------------------------------

class EGEndTimeJD : public MetaDouble
{
public:

   EGEndTimeJD( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern EGEndTimeJD* TheEGEndTimeJDParameter;

// ----------------------------------------------------------------------------

class EGUseAsteroidPerturbers : public MetaBoolean
{
public:

   EGUseAsteroidPerturbers( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGUseAsteroidPerturbers* TheEGUseAsteroidPerturbersParameter;

// ----------------------------------------------------------------------------

class EGUseKBOPerturbers : public MetaBoolean
{
public:

   EGUseKBOPerturbers( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGUseKBOPerturbers* TheEGUseKBOPerturbersParameter;

// ----------------------------------------------------------------------------

class EGVelocityExpansions : public MetaBoolean
{
public:

   EGVelocityExpansions( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGVelocityExpansions* TheEGVelocityExpansionsParameter;

// ----------------------------------------------------------------------------

class EGSeparateEarthMoonPerturbers : public MetaBoolean
{
public:

   EGSeparateEarthMoonPerturbers( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGSeparateEarthMoonPerturbers* TheEGSeparateEarthMoonPerturbersParameter;

// ----------------------------------------------------------------------------

class EGRelativisticPerturbations : public MetaBoolean
{
public:

   EGRelativisticPerturbations( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGRelativisticPerturbations* TheEGRelativisticPerturbationsParameter;

// ----------------------------------------------------------------------------

class EGFigureEffects : public MetaBoolean
{
public:

   EGFigureEffects( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGFigureEffects* TheEGFigureEffectsParameter;

// ----------------------------------------------------------------------------

class EGNonGravitationalPerturbations : public MetaBoolean
{
public:

   EGNonGravitationalPerturbations( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGNonGravitationalPerturbations* TheEGNonGravitationalPerturbationsParameter;

// ----------------------------------------------------------------------------

class EGOutputXEPHFile : public MetaBoolean
{
public:

   EGOutputXEPHFile( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGOutputXEPHFile* TheEGOutputXEPHFileParameter;

// ----------------------------------------------------------------------------

class EGOutputXEPHFilePath : public MetaString
{
public:

   EGOutputXEPHFilePath( MetaProcess* );

   IsoString Id() const override;
};

extern EGOutputXEPHFilePath* TheEGOutputXEPHFilePathParameter;

// ----------------------------------------------------------------------------

class EGOutputLogFile : public MetaBoolean
{
public:

   EGOutputLogFile( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGOutputLogFile* TheEGOutputLogFileParameter;

// ----------------------------------------------------------------------------

class EGOverwriteExistingFiles : public MetaBoolean
{
public:

   EGOverwriteExistingFiles( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EGOverwriteExistingFiles* TheEGOverwriteExistingFilesParameter;

// ----------------------------------------------------------------------------

class EGDenseOutputToleranceFactor : public MetaDouble
{
public:

   EGDenseOutputToleranceFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGDenseOutputToleranceFactor* TheEGDenseOutputToleranceFactorParameter;

// ----------------------------------------------------------------------------

class EGEphemerisToleranceFactor : public MetaDouble
{
public:

   EGEphemerisToleranceFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGEphemerisToleranceFactor* TheEGEphemerisToleranceFactorParameter;

// ----------------------------------------------------------------------------

class EGEphemerisMaxExpansionLength : public MetaInt32
{
public:

   EGEphemerisMaxExpansionLength( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGEphemerisMaxExpansionLength* TheEGEphemerisMaxExpansionLengthParameter;

// ----------------------------------------------------------------------------

class EGEphemerisMaxTruncationError : public MetaDouble
{
public:

   EGEphemerisMaxTruncationError( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern EGEphemerisMaxTruncationError* TheEGEphemerisMaxTruncationErrorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __EphemerisGeneratorParameters_h

// ----------------------------------------------------------------------------
// EOF EphemerisGeneratorParameters.h - Released 2024-12-23T11:33:28Z
