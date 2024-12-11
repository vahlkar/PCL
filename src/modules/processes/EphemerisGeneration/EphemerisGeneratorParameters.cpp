//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// EphemerisGeneratorParameters.cpp - Released 2024-12-11T17:43:17Z
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

#include "EphemerisGeneratorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

EGWorkingMode*                   TheEGWorkingModeParameter = nullptr;
EGPosX*                          TheEGPosXParameter = nullptr;
EGPosY*                          TheEGPosYParameter = nullptr;
EGPosZ*                          TheEGPosZParameter = nullptr;
EGVelX*                          TheEGVelXParameter = nullptr;
EGVelY*                          TheEGVelYParameter = nullptr;
EGVelZ*                          TheEGVelZParameter = nullptr;
EGElemA*                         TheEGElemAParameter = nullptr;
EGElemQ*                         TheEGElemQParameter = nullptr;
EGElemE*                         TheEGElemEParameter = nullptr;
EGElemM*                         TheEGElemMParameter = nullptr;
EGElemT*                         TheEGElemTParameter = nullptr;
EGElemI*                         TheEGElemIParameter = nullptr;
EGElemO*                         TheEGElemOParameter = nullptr;
EGElemW*                         TheEGElemWParameter = nullptr;
EGEpochJD*                       TheEGEpochJDParameter = nullptr;
EGObjectId*                      TheEGObjectIdParameter = nullptr;
EGObjectName*                    TheEGObjectNameParameter = nullptr;
EGHDefined*                      TheEGHDefinedParameter = nullptr;
EGH*                             TheEGHParameter = nullptr;
EGGDefined*                      TheEGGDefinedParameter = nullptr;
EGG*                             TheEGGParameter = nullptr;
EGM1Defined*                     TheEGM1DefinedParameter = nullptr;
EGM1*                            TheEGM1Parameter = nullptr;
EGK1Defined*                     TheEGK1DefinedParameter = nullptr;
EGK1*                            TheEGK1Parameter = nullptr;
EGM2Defined*                     TheEGM2DefinedParameter = nullptr;
EGM2*                            TheEGM2Parameter = nullptr;
EGK2Defined*                     TheEGK2DefinedParameter = nullptr;
EGK2*                            TheEGK2Parameter = nullptr;
EGPCDefined*                     TheEGPCDefinedParameter = nullptr;
EGPC*                            TheEGPCParameter = nullptr;
EGB_VDefined*                    TheEGB_VDefinedParameter = nullptr;
EGB_V*                           TheEGB_VParameter = nullptr;
EGU_BDefined*                    TheEGU_BDefinedParameter = nullptr;
EGU_B*                           TheEGU_BParameter = nullptr;
EGI_RDefined*                    TheEGI_RDefinedParameter = nullptr;
EGI_R*                           TheEGI_RParameter = nullptr;
EGDDefined*                      TheEGDDefinedParameter = nullptr;
EGD*                             TheEGDParameter = nullptr;
EGA1Defined*                     TheEGA1DefinedParameter = nullptr;
EGA1*                            TheEGA1Parameter = nullptr;
EGA2Defined*                     TheEGA2DefinedParameter = nullptr;
EGA2*                            TheEGA2Parameter = nullptr;
EGA3Defined*                     TheEGA3DefinedParameter = nullptr;
EGA3*                            TheEGA3Parameter = nullptr;
EGDTDefined*                     TheEGDTDefinedParameter = nullptr;
EGDT*                            TheEGDTParameter = nullptr;
EGDatabaseFilePath*              TheEGDatabaseFilePathParameter = nullptr;
EGDatabaseFormatName*            TheEGDatabaseFormatNameParameter = nullptr;
EGObjects*                       TheEGObjectsParameter = nullptr;
EGUseRegularExpressions*         TheEGUseRegularExpressionsParameter = nullptr;
EGExcludeCoreAsteroids*          TheEGExcludeCoreAsteroidsParameter = nullptr;
EGFundamentalFilePath*           TheEGFundamentalFilePathParameter = nullptr;
EGAsteroidsFilePath*             TheEGAsteroidsFilePathParameter = nullptr;
EGKBOsFilePath*                  TheEGKBOsFilePathParameter = nullptr;
EGStartTimeJD*                   TheEGStartTimeJDParameter = nullptr;
EGEndTimeJD*                     TheEGEndTimeJDParameter = nullptr;
EGUseAsteroidPerturbers*         TheEGUseAsteroidPerturbersParameter = nullptr;
EGUseKBOPerturbers*              TheEGUseKBOPerturbersParameter = nullptr;
EGVelocityExpansions*            TheEGVelocityExpansionsParameter = nullptr;
EGSeparateEarthMoonPerturbers*   TheEGSeparateEarthMoonPerturbersParameter = nullptr;
EGRelativisticPerturbations*     TheEGRelativisticPerturbationsParameter = nullptr;
EGFigureEffects*                 TheEGFigureEffectsParameter = nullptr;
EGNonGravitationalPerturbations* TheEGNonGravitationalPerturbationsParameter = nullptr;
EGOutputXEPHFile*                TheEGOutputXEPHFileParameter = nullptr;
EGOutputXEPHFilePath*            TheEGOutputXEPHFilePathParameter = nullptr;
EGOutputLogFile*                 TheEGOutputLogFileParameter = nullptr;
EGOverwriteExistingFiles*        TheEGOverwriteExistingFilesParameter = nullptr;
EGDenseOutputToleranceFactor*    TheEGDenseOutputToleranceFactorParameter = nullptr;
EGEphemerisToleranceFactor*      TheEGEphemerisToleranceFactorParameter = nullptr;
EGEphemerisMaxExpansionLength*   TheEGEphemerisMaxExpansionLengthParameter = nullptr;
EGEphemerisMaxTruncationError*   TheEGEphemerisMaxTruncationErrorParameter = nullptr;

// ----------------------------------------------------------------------------

EGWorkingMode::EGWorkingMode( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheEGWorkingModeParameter = this;
}

IsoString EGWorkingMode::Id() const
{
   return "workingMode";
}

size_type EGWorkingMode::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString EGWorkingMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case StateVectors:    return "WorkingMode_StateVectors";
   case OrbitalElements: return "WorkingMode_OrbitalElements";
   case DatabaseObjects: return "WorkingMode_DatabaseObjects";
   }
}

int EGWorkingMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type EGWorkingMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

EGPosX::EGPosX( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGPosXParameter = this;
}

IsoString EGPosX::Id() const
{
   return "x";
}

int EGPosX::Precision() const
{
   return 16;
}

double EGPosX::DefaultValue() const
{
   return 1;
}

double EGPosX::MinimumValue() const
{
   return -1000;
}

double EGPosX::MaximumValue() const
{
   return +1000;
}

// ----------------------------------------------------------------------------

EGPosY::EGPosY( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGPosYParameter = this;
}

IsoString EGPosY::Id() const
{
   return "y";
}

int EGPosY::Precision() const
{
   return 16;
}

double EGPosY::DefaultValue() const
{
   return 1;
}

double EGPosY::MinimumValue() const
{
   return -1000;
}

double EGPosY::MaximumValue() const
{
   return +1000;
}

// ----------------------------------------------------------------------------

EGPosZ::EGPosZ( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGPosZParameter = this;
}

IsoString EGPosZ::Id() const
{
   return "z";
}

int EGPosZ::Precision() const
{
   return 16;
}

double EGPosZ::DefaultValue() const
{
   return 1;
}

double EGPosZ::MinimumValue() const
{
   return -1000;
}

double EGPosZ::MaximumValue() const
{
   return +1000;
}

// ----------------------------------------------------------------------------

EGVelX::EGVelX( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGVelXParameter = this;
}

IsoString EGVelX::Id() const
{
   return "vx";
}

int EGVelX::Precision() const
{
   return 16;
}

double EGVelX::DefaultValue() const
{
   return 0.01;
}

double EGVelX::MinimumValue() const
{
   return -1;
}

double EGVelX::MaximumValue() const
{
   return +1;
}

// ----------------------------------------------------------------------------

EGVelY::EGVelY( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGVelYParameter = this;
}

IsoString EGVelY::Id() const
{
   return "vy";
}

int EGVelY::Precision() const
{
   return 16;
}

double EGVelY::DefaultValue() const
{
   return 0.01;
}

double EGVelY::MinimumValue() const
{
   return -1;
}

double EGVelY::MaximumValue() const
{
   return +1;
}

// ----------------------------------------------------------------------------

EGVelZ::EGVelZ( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGVelZParameter = this;
}

IsoString EGVelZ::Id() const
{
   return "vz";
}

int EGVelZ::Precision() const
{
   return 16;
}

double EGVelZ::DefaultValue() const
{
   return 0.01;
}

double EGVelZ::MinimumValue() const
{
   return -1;
}

double EGVelZ::MaximumValue() const
{
   return +1;
}

// ----------------------------------------------------------------------------

EGElemA::EGElemA( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGElemAParameter = this;
}

IsoString EGElemA::Id() const
{
   return "a";
}

int EGElemA::Precision() const
{
   return 16;
}

double EGElemA::DefaultValue() const
{
   return 2;
}

double EGElemA::MinimumValue() const
{
   return -1.0e6;
}

double EGElemA::MaximumValue() const
{
   return +1.0e6;
}

// ----------------------------------------------------------------------------

EGElemQ::EGElemQ( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGElemQParameter = this;
}

IsoString EGElemQ::Id() const
{
   return "q";
}

int EGElemQ::Precision() const
{
   return 16;
}

double EGElemQ::DefaultValue() const
{
   return 1;
}

double EGElemQ::MinimumValue() const
{
   return 0;
}

double EGElemQ::MaximumValue() const
{
   return 10000;
}

// ----------------------------------------------------------------------------

EGElemE::EGElemE( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGElemEParameter = this;
}

IsoString EGElemE::Id() const
{
   return "e";
}

int EGElemE::Precision() const
{
   return 16;
}

double EGElemE::DefaultValue() const
{
   return 0.5;
}

double EGElemE::MinimumValue() const
{
   return 0;
}

double EGElemE::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

EGElemM::EGElemM( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGElemMParameter = this;
}

IsoString EGElemM::Id() const
{
   return "M";
}

int EGElemM::Precision() const
{
   return 16;
}

double EGElemM::DefaultValue() const
{
   return 0;
}

double EGElemM::MinimumValue() const
{
   return -360;
}

double EGElemM::MaximumValue() const
{
   return +360;
}

// ----------------------------------------------------------------------------

EGElemT::EGElemT( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGElemTParameter = this;
}

IsoString EGElemT::Id() const
{
   return "T";
}

int EGElemT::Precision() const
{
   return 10; // 1 ms = 2.7...e-07 day
}

double EGElemT::DefaultValue() const
{
   return 2451545.0;
}

// ----------------------------------------------------------------------------

EGElemI::EGElemI( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGElemIParameter = this;
}

IsoString EGElemI::Id() const
{
   return "i";
}

int EGElemI::Precision() const
{
   return 16;
}

double EGElemI::DefaultValue() const
{
   return 45.0;
}

double EGElemI::MinimumValue() const
{
   return 0;
}

double EGElemI::MaximumValue() const
{
   return 180;
}

// ----------------------------------------------------------------------------

EGElemO::EGElemO( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGElemOParameter = this;
}

IsoString EGElemO::Id() const
{
   return "O";
}

int EGElemO::Precision() const
{
   return 16;
}

double EGElemO::DefaultValue() const
{
   return 251.8822812;
}

double EGElemO::MinimumValue() const
{
   return 0;
}

double EGElemO::MaximumValue() const
{
   return 360;
}

// ----------------------------------------------------------------------------

EGElemW::EGElemW( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGElemWParameter = this;
}

IsoString EGElemW::Id() const
{
   return "w";
}

int EGElemW::Precision() const
{
   return 16;
}

double EGElemW::DefaultValue() const
{
   return 12.8450708;
}

double EGElemW::MinimumValue() const
{
   return 0;
}

double EGElemW::MaximumValue() const
{
   return 360;
}

// ----------------------------------------------------------------------------

EGEpochJD::EGEpochJD( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGEpochJDParameter = this;
}

IsoString EGEpochJD::Id() const
{
   return "epochJD";
}

int EGEpochJD::Precision() const
{
   return 10; // 1 ms = 2.7...e-07 day
}

double EGEpochJD::DefaultValue() const
{
   return 2451545.0;
}

// ----------------------------------------------------------------------------

EGObjectId::EGObjectId( MetaProcess* P )
   : MetaString( P )
{
   TheEGObjectIdParameter = this;
}

IsoString EGObjectId::Id() const
{
   return "objectId";
}

// ----------------------------------------------------------------------------

EGObjectName::EGObjectName( MetaProcess* P )
   : MetaString( P )
{
   TheEGObjectNameParameter = this;
}

IsoString EGObjectName::Id() const
{
   return "objectName";
}

// ----------------------------------------------------------------------------

EGHDefined::EGHDefined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGHDefinedParameter = this;
}

IsoString EGHDefined::Id() const
{
   return "H_defined";
}

bool EGHDefined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGH::EGH( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGHParameter = this;
}

IsoString EGH::Id() const
{
   return "H";
}

int EGH::Precision() const
{
   return 2;
}

double EGH::DefaultValue() const
{
   return 14.0;
}

// ----------------------------------------------------------------------------

EGGDefined::EGGDefined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGGDefinedParameter = this;
}

IsoString EGGDefined::Id() const
{
   return "G_defined";
}

bool EGGDefined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGG::EGG( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGGParameter = this;
}

IsoString EGG::Id() const
{
   return "G";
}

int EGG::Precision() const
{
   return 2;
}

double EGG::DefaultValue() const
{
   return 0.15;
}

// ----------------------------------------------------------------------------

EGM1Defined::EGM1Defined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGM1DefinedParameter = this;
}

IsoString EGM1Defined::Id() const
{
   return "M1_defined";
}

bool EGM1Defined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGM1::EGM1( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGM1Parameter = this;
}

IsoString EGM1::Id() const
{
   return "M1";
}

int EGM1::Precision() const
{
   return 2;
}

double EGM1::DefaultValue() const
{
   return 15.0;
}

// ----------------------------------------------------------------------------

EGK1Defined::EGK1Defined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGK1DefinedParameter = this;
}

IsoString EGK1Defined::Id() const
{
   return "K1_defined";
}

bool EGK1Defined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGK1::EGK1( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGK1Parameter = this;
}

IsoString EGK1::Id() const
{
   return "K1";
}

int EGK1::Precision() const
{
   return 2;
}

double EGK1::DefaultValue() const
{
   return 5.0;
}

// ----------------------------------------------------------------------------

EGM2Defined::EGM2Defined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGM2DefinedParameter = this;
}

IsoString EGM2Defined::Id() const
{
   return "M2_defined";
}

bool EGM2Defined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGM2::EGM2( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGM2Parameter = this;
}

IsoString EGM2::Id() const
{
   return "M2";
}

int EGM2::Precision() const
{
   return 2;
}

double EGM2::DefaultValue() const
{
   return 15.0;
}

// ----------------------------------------------------------------------------

EGK2Defined::EGK2Defined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGK2DefinedParameter = this;
}

IsoString EGK2Defined::Id() const
{
   return "K2_defined";
}

bool EGK2Defined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGK2::EGK2( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGK2Parameter = this;
}

IsoString EGK2::Id() const
{
   return "K2";
}

int EGK2::Precision() const
{
   return 2;
}

double EGK2::DefaultValue() const
{
   return 5.0;
}

// ----------------------------------------------------------------------------

EGPCDefined::EGPCDefined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGPCDefinedParameter = this;
}

IsoString EGPCDefined::Id() const
{
   return "PC_defined";
}

bool EGPCDefined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGPC::EGPC( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGPCParameter = this;
}

IsoString EGPC::Id() const
{
   return "PC";
}

int EGPC::Precision() const
{
   return 2;
}

double EGPC::DefaultValue() const
{
   return 0.03;
}

// ----------------------------------------------------------------------------

EGB_VDefined::EGB_VDefined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGB_VDefinedParameter = this;
}

IsoString EGB_VDefined::Id() const
{
   return "B_V_defined";
}

bool EGB_VDefined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGB_V::EGB_V( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGB_VParameter = this;
}

IsoString EGB_V::Id() const
{
   return "B_V";
}

int EGB_V::Precision() const
{
   return 2;
}

double EGB_V::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

EGU_BDefined::EGU_BDefined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGU_BDefinedParameter = this;
}

IsoString EGU_BDefined::Id() const
{
   return "U_B_defined";
}

bool EGU_BDefined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGU_B::EGU_B( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGU_BParameter = this;
}

IsoString EGU_B::Id() const
{
   return "U_B";
}

int EGU_B::Precision() const
{
   return 2;
}

double EGU_B::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

EGI_RDefined::EGI_RDefined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGI_RDefinedParameter = this;
}

IsoString EGI_RDefined::Id() const
{
   return "I_R_defined";
}

bool EGI_RDefined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGI_R::EGI_R( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGI_RParameter = this;
}

IsoString EGI_R::Id() const
{
   return "I_R";
}

int EGI_R::Precision() const
{
   return 2;
}

double EGI_R::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

EGDDefined::EGDDefined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGDDefinedParameter = this;
}

IsoString EGDDefined::Id() const
{
   return "D_defined";
}

bool EGDDefined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGD::EGD( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGDParameter = this;
}

IsoString EGD::Id() const
{
   return "D";
}

int EGD::Precision() const
{
   return 2;
}

double EGD::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

EGA1Defined::EGA1Defined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGA1DefinedParameter = this;
}

IsoString EGA1Defined::Id() const
{
   return "A1_defined";
}

bool EGA1Defined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGA1::EGA1( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGA1Parameter = this;
}

IsoString EGA1::Id() const
{
   return "A1";
}

int EGA1::Precision() const
{
   return 5;
}

double EGA1::DefaultValue() const
{
   return 0;
}

bool EGA1::ScientificNotation() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGA2Defined::EGA2Defined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGA2DefinedParameter = this;
}

IsoString EGA2Defined::Id() const
{
   return "A2_defined";
}

bool EGA2Defined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGA2::EGA2( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGA2Parameter = this;
}

IsoString EGA2::Id() const
{
   return "A2";
}

int EGA2::Precision() const
{
   return 5;
}

double EGA2::DefaultValue() const
{
   return 0;
}

bool EGA2::ScientificNotation() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGA3Defined::EGA3Defined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGA3DefinedParameter = this;
}

IsoString EGA3Defined::Id() const
{
   return "A3_defined";
}

bool EGA3Defined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGA3::EGA3( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGA3Parameter = this;
}

IsoString EGA3::Id() const
{
   return "A3";
}

int EGA3::Precision() const
{
   return 5;
}

double EGA3::DefaultValue() const
{
   return 0;
}

bool EGA3::ScientificNotation() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGDTDefined::EGDTDefined( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGDTDefinedParameter = this;
}

IsoString EGDTDefined::Id() const
{
   return "DT_defined";
}

bool EGDTDefined::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGDT::EGDT( MetaProcess* P )
   : MetaFloat( P )
{
   TheEGDTParameter = this;
}

IsoString EGDT::Id() const
{
   return "DT";
}

int EGDT::Precision() const
{
   return 5;
}

double EGDT::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

EGDatabaseFilePath::EGDatabaseFilePath( MetaProcess* P )
   : MetaString( P )
{
   TheEGDatabaseFilePathParameter = this;
}

IsoString EGDatabaseFilePath::Id() const
{
   return "databaseFilePath";
}

// ----------------------------------------------------------------------------

EGDatabaseFormatName::EGDatabaseFormatName( MetaProcess* P )
   : MetaString( P )
{
   TheEGDatabaseFormatNameParameter = this;
}

IsoString EGDatabaseFormatName::Id() const
{
   return "databaseFormatName";
}

String EGDatabaseFormatName::DefaultValue() const
{
   return "Lowell astorbDB";
}

// ----------------------------------------------------------------------------

EGObjects::EGObjects( MetaProcess* P )
   : MetaString( P )
{
   TheEGObjectsParameter = this;
}

IsoString EGObjects::Id() const
{
   return "objects";
}

// ----------------------------------------------------------------------------

EGUseRegularExpressions::EGUseRegularExpressions( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGUseRegularExpressionsParameter = this;
}

IsoString EGUseRegularExpressions::Id() const
{
   return "useRegularExpressions";
}

bool EGUseRegularExpressions::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGExcludeCoreAsteroids::EGExcludeCoreAsteroids( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGExcludeCoreAsteroidsParameter = this;
}

IsoString EGExcludeCoreAsteroids::Id() const
{
   return "excludeCoreAsteroids";
}

bool EGExcludeCoreAsteroids::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGFundamentalFilePath::EGFundamentalFilePath( MetaProcess* P )
   : MetaString( P )
{
   TheEGFundamentalFilePathParameter = this;
}

IsoString EGFundamentalFilePath::Id() const
{
   return "fundamentalFilePath";
}

// ----------------------------------------------------------------------------

EGAsteroidsFilePath::EGAsteroidsFilePath( MetaProcess* P )
   : MetaString( P )
{
   TheEGAsteroidsFilePathParameter = this;
}

IsoString EGAsteroidsFilePath::Id() const
{
   return "asteroidsFilePath";
}

// ----------------------------------------------------------------------------

EGKBOsFilePath::EGKBOsFilePath( MetaProcess* P )
   : MetaString( P )
{
   TheEGKBOsFilePathParameter = this;
}

IsoString EGKBOsFilePath::Id() const
{
   return "KBOsFilePath";
}

// ----------------------------------------------------------------------------

EGStartTimeJD::EGStartTimeJD( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGStartTimeJDParameter = this;
}

IsoString EGStartTimeJD::Id() const
{
   return "startTimeJD";
}

int EGStartTimeJD::Precision() const
{
   return 7; // 1 ms = 2.7...e-07 day
}

double EGStartTimeJD::DefaultValue() const
{
   return 2451545.0; // J2000.0
}

// ----------------------------------------------------------------------------

EGEndTimeJD::EGEndTimeJD( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGEndTimeJDParameter = this;
}

IsoString EGEndTimeJD::Id() const
{
   return "endTimeJD";
}

int EGEndTimeJD::Precision() const
{
   return 7; // 1 ms = 2.7...e-07 day
}

double EGEndTimeJD::DefaultValue() const
{
   return 2451545.0; // J2000.0
}

// ----------------------------------------------------------------------------

EGUseAsteroidPerturbers::EGUseAsteroidPerturbers( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGUseAsteroidPerturbersParameter = this;
}

IsoString EGUseAsteroidPerturbers::Id() const
{
   return "useAsteroidPerturbers";
}

bool EGUseAsteroidPerturbers::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGUseKBOPerturbers::EGUseKBOPerturbers( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGUseKBOPerturbersParameter = this;
}

IsoString EGUseKBOPerturbers::Id() const
{
   return "useKBOPerturbers";
}

bool EGUseKBOPerturbers::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGVelocityExpansions::EGVelocityExpansions( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGVelocityExpansionsParameter = this;
}

IsoString EGVelocityExpansions::Id() const
{
   return "velocityExpansions";
}

bool EGVelocityExpansions::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGSeparateEarthMoonPerturbers::EGSeparateEarthMoonPerturbers( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGSeparateEarthMoonPerturbersParameter = this;
}

IsoString EGSeparateEarthMoonPerturbers::Id() const
{
   return "separateEarthMoonPerturbers";
}

bool EGSeparateEarthMoonPerturbers::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGRelativisticPerturbations::EGRelativisticPerturbations( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGRelativisticPerturbationsParameter = this;
}

IsoString EGRelativisticPerturbations::Id() const
{
   return "relativisticPerturbations";
}

bool EGRelativisticPerturbations::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGFigureEffects::EGFigureEffects( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGFigureEffectsParameter = this;
}

IsoString EGFigureEffects::Id() const
{
   return "figureEffects";
}

bool EGFigureEffects::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGNonGravitationalPerturbations::EGNonGravitationalPerturbations( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGNonGravitationalPerturbationsParameter = this;
}

IsoString EGNonGravitationalPerturbations::Id() const
{
   return "nonGravitationalPerturbations";
}

bool EGNonGravitationalPerturbations::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGOutputXEPHFile::EGOutputXEPHFile( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGOutputXEPHFileParameter = this;
}

IsoString EGOutputXEPHFile::Id() const
{
   return "outputXEPHFile";
}

bool EGOutputXEPHFile::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGOutputXEPHFilePath::EGOutputXEPHFilePath( MetaProcess* P )
   : MetaString( P )
{
   TheEGOutputXEPHFilePathParameter = this;
}

IsoString EGOutputXEPHFilePath::Id() const
{
   return "outputXEPHFilePath";
}

// ----------------------------------------------------------------------------

EGOutputLogFile::EGOutputLogFile( MetaProcess* P )
   : MetaBoolean( P )
{
   TheEGOutputLogFileParameter = this;
}

IsoString EGOutputLogFile::Id() const
{
   return "outputLogFile";
}

bool EGOutputLogFile::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EGOverwriteExistingFiles::EGOverwriteExistingFiles( MetaProcess* P ) : MetaBoolean( P )
{
   TheEGOverwriteExistingFilesParameter = this;
}

IsoString EGOverwriteExistingFiles::Id() const
{
   return "overwriteExistingFiles";
}

bool EGOverwriteExistingFiles::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

EGDenseOutputToleranceFactor::EGDenseOutputToleranceFactor( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGDenseOutputToleranceFactorParameter = this;
}

IsoString EGDenseOutputToleranceFactor::Id() const
{
   return "denseOutputToleranceFactor";
}

int EGDenseOutputToleranceFactor::Precision() const
{
   return 2;
}

double EGDenseOutputToleranceFactor::DefaultValue() const
{
   return 1500;
}

double EGDenseOutputToleranceFactor::MinimumValue() const
{
   return 100;
}

double EGDenseOutputToleranceFactor::MaximumValue() const
{
   return 10000;
}

// ----------------------------------------------------------------------------

EGEphemerisToleranceFactor::EGEphemerisToleranceFactor( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGEphemerisToleranceFactorParameter = this;
}

IsoString EGEphemerisToleranceFactor::Id() const
{
   return "ephemerisToleranceFactor";
}

int EGEphemerisToleranceFactor::Precision() const
{
   return 2;
}

double EGEphemerisToleranceFactor::DefaultValue() const
{
   return 25;
}

double EGEphemerisToleranceFactor::MinimumValue() const
{
   return 1;
}

double EGEphemerisToleranceFactor::MaximumValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

EGEphemerisMaxExpansionLength::EGEphemerisMaxExpansionLength( MetaProcess* P )
   : MetaInt32( P )
{
   TheEGEphemerisMaxExpansionLengthParameter = this;
}

IsoString EGEphemerisMaxExpansionLength::Id() const
{
   return "ephemerisMaxExpansionLength";
}

double EGEphemerisMaxExpansionLength::DefaultValue() const
{
   return 100;
}

double EGEphemerisMaxExpansionLength::MinimumValue() const
{
   return 10;
}

double EGEphemerisMaxExpansionLength::MaximumValue() const
{
   // N.B. XEPH format: Expansion index nodes store expansion lengths as 8-bit
   // unsigned integers.
   return 256;
}

// ----------------------------------------------------------------------------

EGEphemerisMaxTruncationError::EGEphemerisMaxTruncationError( MetaProcess* P )
   : MetaDouble( P )
{
   TheEGEphemerisMaxTruncationErrorParameter = this;
}

IsoString EGEphemerisMaxTruncationError::Id() const
{
   return "ephemerisMaxTruncationError";
}

int EGEphemerisMaxTruncationError::Precision() const
{
   return 2;
}

bool EGEphemerisMaxTruncationError::ScientificNotation() const
{
   return true;
}

double EGEphemerisMaxTruncationError::DefaultValue() const
{
   return 1.0e-07; // 15 km
}

double EGEphemerisMaxTruncationError::MinimumValue() const
{
   return 1.0e-12; // 15 cm
}

double EGEphemerisMaxTruncationError::MaximumValue() const
{
   return 1.0e-05; // 1500 km
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF EphemerisGeneratorParameters.cpp - Released 2024-12-11T17:43:17Z
