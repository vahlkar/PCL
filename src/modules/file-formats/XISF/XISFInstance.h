//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 1.0.12
// ----------------------------------------------------------------------------
// XISFInstance.h - Released 2021-04-09T19:41:32Z
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

#ifndef __XISFInstance_h
#define __XISFInstance_h

#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/FileFormatImplementation.h>
#include <pcl/XISF.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class XISFFormat;
class XISFStreamHints;

class XISFInstance : public FileFormatImplementation
{
public:

   XISFInstance( const XISFFormat* );

   virtual ~XISFInstance() noexcept( false );

   ImageDescriptionArray Open( const String& filePath, const IsoString& hints ) override;
   bool IsOpen() const override;
   String FilePath() const override;
   void Close() override;

   void SelectImage( int index ) override;
   int SelectedImageIndex() const override;

   void* FormatSpecificData() const override;

   ICCProfile ReadICCProfile() override;
   RGBColorSystem ReadRGBWorkingSpace() override;
   DisplayFunction ReadDisplayFunction() override;
   ColorFilterArray ReadColorFilterArray() override;
   UInt8Image ReadThumbnail() override;
   FITSKeywordArray ReadFITSKeywords() override;
   PropertyDescriptionArray PropertyDescriptions() override;
   Variant ReadProperty( const IsoString& property ) override;
   PropertyDescriptionArray ImagePropertyDescriptions() override;
   Variant ReadImageProperty( const IsoString& property ) override;

   void ReadImage( Image& ) override;
   void ReadImage( DImage& ) override;
   void ReadImage( UInt8Image& ) override;
   void ReadImage( UInt16Image& ) override;
   void ReadImage( UInt32Image& ) override;

   bool CanReadIncrementally() const override;

   void ReadSamples( Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( DImage::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel ) override;

   bool QueryOptions( Array<ImageOptions>& options, Array<void*>& formatOptions ) override;
   void Create( const String& filePath, int numberOfImages, const IsoString& hints ) override;
   void SetId( const IsoString& ) override;
   void SetOptions( const ImageOptions& ) override;
   void SetFormatSpecificData( const void* ) override;

   void WriteICCProfile( const ICCProfile& ) override;
   void WriteRGBWorkingSpace( const RGBColorSystem& rgbws ) override;
   void WriteDisplayFunction( const DisplayFunction& df ) override;
   void WriteColorFilterArray( const ColorFilterArray& cfa ) override;
   void WriteThumbnail( const UInt8Image& ) override;
   void WriteFITSKeywords( const FITSKeywordArray& ) override;
   void WriteProperty( const IsoString& property, const Variant& value ) override;
   void WriteImageProperty( const IsoString& property, const Variant& value ) override;

   void WriteImage( const Image& ) override;
   void WriteImage( const DImage& ) override;
   void WriteImage( const UInt8Image& ) override;
   void WriteImage( const UInt16Image& ) override;
   void WriteImage( const UInt32Image& ) override;

   void CreateImage( const ImageInfo& ) override;
   void CloseImage() override;

   void WriteSamples( const Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const DImage::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel ) override;

private:

   AutoPointer<XISFReader>      m_reader;
   AutoPointer<XISFWriter>      m_writer;
   AutoPointer<XISFStreamHints> m_readHints;
   AutoPointer<XISFStreamHints> m_writeHints;
   bool                         m_queriedOptions = false; // did us query options to the user?

   class LogHandler : public XISFLogHandler
   {
   public:

      LogHandler() = default;

      void Init( const String& filePath, bool writing ) override;
      void Log( const String& text, message_type type ) override;
      void Close() override;

   private:

      Console m_console;
   };
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __XISFInstance_h

// ----------------------------------------------------------------------------
// EOF XISFInstance.h - Released 2021-04-09T19:41:32Z
