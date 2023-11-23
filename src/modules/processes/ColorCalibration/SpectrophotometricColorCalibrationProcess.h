//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.3
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// SpectrophotometricColorCalibrationProcess.h - Released 2023-11-23T18:45:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __SpectrophotometricColorCalibrationProcess_h
#define __SpectrophotometricColorCalibrationProcess_h

#include <pcl/MetaProcess.h>

#include "SampledSpectrumData.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SpectrophotometricColorCalibrationProcess : public MetaProcess
{
public:

   SpectrophotometricColorCalibrationProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;

   static SampledSpectrumDataList LoadWhiteReferences( const String& filePath );
   static SampledSpectrumDataList LoadFilters( const String& filePath );

   static void ExportWhiteReferencesDatabase( const String& filePath, const SampledSpectrumDataList& );
   static void ExportFiltersDatabase( const String& filePath, const SampledSpectrumDataList& );

   static SampledSpectrumData DecodeWhiteReferenceCSV( const IsoStringList& );
   static SampledSpectrumData DecodeFilterCSV( const IsoStringList& );

   static IsoStringList EncodeWhiteReferenceCSV( const SampledSpectrumData& );
   static IsoStringList EncodeFilterCSV( const SampledSpectrumData& );

   static bool ImportWhiteReferences( const String& filePath, bool merge = true );
   static void ImportWhiteReferences( const SampledSpectrumDataList& filters, bool merge = true );

   static bool ImportFilters( const String& filePath, bool merge = true );
   static void ImportFilters( const SampledSpectrumDataList& filters, bool merge = true );

   static bool InitializeSpectrumData( const String& whiteReferencesDatabaseFilePath = String(),
                                       const String& filtersDatabaseFilePath = String() );
   static bool HasValidSpectrumData();
   static const SampledSpectrumDataList& WhiteReferences();
   static const SampledSpectrumData& DefaultWhiteReference();
   static const SampledSpectrumDataList& Filters();
   static const SampledSpectrumData& DefaultRedFilter();
   static const SampledSpectrumData& DefaultGreenFilter();
   static const SampledSpectrumData& DefaultBlueFilter();
   static const SampledSpectrumData& DefaultDeviceQE();

   static String WhiteReferencesDatabaseFilePath();
   static String FiltersDatabaseFilePath();
   static String DefaultWhiteReferencesDatabaseFilePath();
   static String DefaultFiltersDatabaseFilePath();
   static bool UsingDefaultWhiteReferencesDatabase();
   static bool UsingDefaultFiltersDatabase();

   static int FindWhiteReferenceByName( const String& name )
   {
      return WhiteReferences().Find( name );
   }

   static int FindFilterByName( const String& name )
   {
      return Filters().Find( name );
   }

   static void LoadPreferences();
   static void SavePreferences();
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SpectrophotometricColorCalibrationProcess* TheSpectrophotometricColorCalibrationProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SpectrophotometricColorCalibrationProcess_h

// ----------------------------------------------------------------------------
// EOF SpectrophotometricColorCalibrationProcess.h - Released 2023-11-23T18:45:25Z
