//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.9
// ----------------------------------------------------------------------------
// Standard RAW File Format Module Version 1.6.2
// ----------------------------------------------------------------------------
// RawInstance.h - Released 2024-03-20T10:42:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard RAW PixInsight module.
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

#ifndef __RawInstance_h
#define __RawInstance_h

#include <pcl/FileFormatImplementation.h>

#include "RawPreferences.h"

class LibRaw;

#define RAW_DEFAULT_VERBOSITY    2
#define RAW_TIMEOFFSET_UNKNOWN   int32_min
#define RAW_GPS_UNKNOWN          -1000

namespace pcl
{

// ----------------------------------------------------------------------------

class RawFormat;
class RawProgress;

class RawInstance : public FileFormatImplementation
{
public:

   RawInstance( const RawFormat* );

   virtual ~RawInstance() noexcept( false );

   ImageDescriptionArray Open( const String& filePath, const IsoString& hints ) override;
   bool IsOpen() const override;
   String FilePath() const override;
   void Close() override;

   String ImageFormatInfo() const override;

   ICCProfile ReadICCProfile() override;

   // We synthesize critical FITS keywords: TIME-OBS, EXPTIME, FOCALLEN, APTDIA
   FITSKeywordArray ReadFITSKeywords() override;

   PropertyDescriptionArray ImagePropertyDescriptions() override;
   Variant ReadImageProperty( const IsoString& property ) override;

   void ReadImage( Image& ) override;
   void ReadImage( DImage& ) override;
   void ReadImage( UInt8Image& ) override;
   void ReadImage( UInt16Image& ) override;
   void ReadImage( UInt32Image& ) override;

   UInt8Image ReadThumbnail() override;

private:

   RawPreferences           m_preferences;
   int                      m_verbosity = RAW_DEFAULT_VERBOSITY;
   String                   m_filePath;
   AutoPointer<LibRaw>      m_raw;
   AutoPointer<RawProgress> m_progress;

   String                   m_description;
   String                   m_author;
   IsoString                m_cameraManufacturer;
   IsoString                m_cameraModel;
   IsoString                m_cfaPattern;
   IsoString                m_rawCFAPattern;
   IsoString                m_cfaPatternName;
   F32Matrix                m_sRGBConversionMatrix;
   TimePoint                m_timestamp;
   int                      m_timeOffset = RAW_TIMEOFFSET_UNKNOWN; // in minutes
   double                   m_gpsLongitude = RAW_GPS_UNKNOWN; // in degrees
   double                   m_gpsLatitude = RAW_GPS_UNKNOWN; // in degrees
   float                    m_gpsAltitude = RAW_GPS_UNKNOWN; // in meters
   float                    m_exposure = 0;
   int                      m_isoSpeed = 0;
   float                    m_focalLength = 0;
   float                    m_aperture = 0;

   String ExposureAsText() const;

   void CheckOpenStream( const String& ) const;

   static void CheckLibRawReturnCode( int );
   static void LibRawEXIFCallback( void*, int, int, int, unsigned, void*, int64 );

   friend class RawReadHints;
   friend class RawProgress;
   friend class RawImageReader;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RawInstance_h

// ----------------------------------------------------------------------------
// EOF RawInstance.h - Released 2024-03-20T10:42:01Z
