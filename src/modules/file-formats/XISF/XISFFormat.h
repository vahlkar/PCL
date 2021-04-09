//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 1.0.12
// ----------------------------------------------------------------------------
// XISFFormat.h - Released 2021-04-09T19:41:32Z
// ----------------------------------------------------------------------------
// This file is part of the standard XISF PixInsight module.
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

#ifndef __XISFFormat_h
#define __XISFFormat_h

#include <pcl/MetaFileFormat.h>
#include <pcl/XISF.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class XISFFormat : public MetaFileFormat
{
public:

   XISFFormat();

   IsoString Name() const override;

   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;

   uint32 Version() const override;

   String Description() const override;
   String Implementation() const override;

   String IconImageSVGFile() const override;

   bool CanReadIncrementally() const override;
   bool CanWriteIncrementally() const override;
   bool CanStore32Bit() const override;
   bool CanStore64Bit() const override;
   bool CanStoreFloat() const override;
   bool CanStoreDouble() const override;
   bool CanStoreComplex() const override;
   bool CanStoreDComplex() const override;
   bool CanStoreResolution() const override;
   bool CanStoreKeywords() const override;
   bool CanStoreICCProfiles() const override;
   bool CanStoreThumbnails() const override;
   bool CanStoreProperties() const override;
   bool CanStoreImageProperties() const override;
   bool CanStoreRGBWS() const override;
   bool CanStoreDisplayFunctions() const override;
   bool CanStoreColorFilterArrays() const override;
   bool SupportsCompression() const override;
   bool SupportsMultipleImages() const override;
   bool SupportsViewProperties() const override;
   bool CanEditPreferences() const override;
   bool UsesFormatSpecificData() const override;

   FileFormatImplementation* Create() const override;

   bool EditPreferences() const override;

   bool ValidateFormatSpecificData( const void* data ) const override;
   void DisposeFormatSpecificData( void* data ) const override;

   struct FormatOptions
   {
      XISFOptions options;

      FormatOptions();
      FormatOptions( const FormatOptions& ) = default;

      static FormatOptions* FromGenericDataBlock( const void* );

   private:

      uint32 signature;
   };

   static XISFOptions DefaultOptions();

   struct EmbeddingOverrides
   {
      bool overridePropertyEmbedding        = false;
      bool embedProperties                  = true;
      bool overrideICCProfileEmbedding      = false;
      bool embedICCProfiles                 = false;
      bool overrideDisplayFunctionEmbedding = false;
      bool embedDisplayFunctions            = false;
      bool overrideRGBWorkingSpaceEmbedding = false;
      bool embedRGBWorkingSpaces            = false;
      bool overrideThumbnailEmbedding       = false;
      bool embedThumbnails                  = false;
      bool overridePreviewRectsEmbedding    = true;
      bool embedPreviewRects                = false;
   };

   static EmbeddingOverrides DefaultEmbeddingOverrides();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __XISFFormat_h

// ----------------------------------------------------------------------------
// EOF XISFFormat.h - Released 2021-04-09T19:41:32Z
