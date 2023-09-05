//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard APASS Process Module Version 1.1.4
// ----------------------------------------------------------------------------
// APASSInstance.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard APASS PixInsight module.
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

#ifndef __APASSInstance_h
#define __APASSInstance_h

#include <pcl/APASSDatabaseFile.h>
#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class APASSInstance : public ProcessImplementation
{
public:

   APASSInstance( const MetaProcess* );
   APASSInstance( const APASSInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   int OutputDataRelease() const;

private:

   String          p_command;
   int32           p_verbosity;   // Verbosity level: 0=silent 1=minimal 2=full
   pcl_enum        p_dataRelease; // DR9, DR10, BestAvailable
   pcl_enum        p_sortBy;
   pcl_bool        p_generateTextOutput;
   pcl_bool        p_generateBinaryOutput;
   pcl_enum        p_textFormat;
   pcl_enum        p_textHeaders;
   String          p_outputFilePath;
   APASSSearchData p_searchData;
   StringList      p_databaseFilePaths;
   pcl_bool        o_isValid = false;
   pcl_enum        o_dataRelease = -1; // DR9, DR10
   float           o_databaseMagnitudeLow = 0;
   float           o_databaseMagnitudeHigh = 0;

   void Search();
   void GenerateTextOutput() const;
   void GenerateBinaryOutput() const;
   void GetInfo();
   void Configure();

   friend class APASSInterface;
   friend class APASSPreferencesDialog;
};

// ----------------------------------------------------------------------------

struct APASSBinaryHeader
{
   uint32 version = 0;
   uint32 reserved1 = 0;
   uint32 reserved2 = 0;
   uint32 reserved3 = 0;
   uint32 dataRelease = 0;
   uint32 sourceCount = 0;
   uint32 reserved4 = 0;
   uint32 reserved5 = 0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __APASSInstance_h

// ----------------------------------------------------------------------------
// EOF APASSInstance.h - Released 2023-08-28T15:23:41Z
