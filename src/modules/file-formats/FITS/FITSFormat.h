//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.1
// ----------------------------------------------------------------------------
// FITSFormat.h - Released 2024-12-28T16:54:05Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

#ifndef __FITSFormat_h
#define __FITSFormat_h

#include "FITS.h"

#include <pcl/MetaFileFormat.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FITSFormat : public MetaFileFormat
{
public:

   FITSFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String Status() const override;
   String IconImageSVGFile() const override;
   bool CanReadIncrementally() const override;
   bool CanWriteIncrementally() const override;
   bool CanStore32Bit() const override;
   bool CanStore64Bit() const override;
   bool CanStoreFloat() const override;
   bool CanStoreDouble() const override;
   bool CanStoreResolution() const override;
   bool CanStoreKeywords() const override;
   bool CanStoreICCProfiles() const override;
   bool CanStoreThumbnails() const override;
   bool CanStoreImageProperties() const override;
   bool SupportsMultipleImages() const override;
   bool CanEditPreferences() const override;
   bool UsesFormatSpecificData() const override;
   bool IsDeprecated() const override;
   bool ValidateFormatSpecificData( const void* data ) const override;
   void DisposeFormatSpecificData( void* data ) const override;
   FileFormatImplementation* Create() const override;
   bool EditPreferences() const override;

   // FITS format-specific data

   struct FormatOptions
   {
      uint32            signature;
      FITSImageOptions  options;

      FormatOptions();
      FormatOptions( const FormatOptions& );

      static FormatOptions* FromGenericDataBlock( const void* );
   };

   // Default FITS format options.
   static FITSImageOptions DefaultOptions();

   // Defines how to proceed when there are out of range values in a just-read
   // floating point FITS image.
   enum out_of_range_policy
   {
      OutOfRangePolicy_SetRange,    // use LowerRange() & UpperRange() values
      OutOfRangePolicy_AskIfOut,    // if there are out of range values, ask range and action
      OutOfRangePolicy_AskAlways,   // always ask for range and action (truncate, rescale)
      OutOfRangePolicy_Default = OutOfRangePolicy_SetRange
   };

   // Defines how to fix out of range pixel values in a just-read floating
   // point FITS image.
   enum out_of_range_fix_mode
   {
      OutOfRangeFix_Rescale,        // rescale existing values to [lowerRange,upperRange]
      OutOfRangeFix_Truncate,       // truncate existing values to [lowerRange,upperRange]
      OutOfRangeFix_Ignore,         // allow (do not fix) out-of-range values
      OutOfRangeFix_Default = OutOfRangeFix_Rescale
   };

   struct OutOfRangePolicyOptions
   {
      double                lowerRange        = 0;
      double                upperRange        = 1;
      out_of_range_policy   outOfRangePolicy  = OutOfRangePolicy_Default;
      out_of_range_fix_mode outOfRangeFixMode = OutOfRangeFix_Default;
   };

   static OutOfRangePolicyOptions DefaultOutOfRangePolicyOptions();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FITSFormat_h

// ----------------------------------------------------------------------------
// EOF FITSFormat.h - Released 2024-12-28T16:54:05Z
