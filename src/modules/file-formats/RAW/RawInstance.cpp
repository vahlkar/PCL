//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard RAW File Format Module Version 1.6.3
// ----------------------------------------------------------------------------
// RawInstance.cpp - Released 2024-12-28T16:54:05Z
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

#include <libraw/libraw.h>

#include "RawInstance.h"
#include "RawFormat.h"
#include "RawModule.h"

#include <pcl/FastRotation.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/StandardStatus.h>
#include <pcl/TimePoint.h>
#include <pcl/Version.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class RawReadHints
{
public:

   RawPreferences preferences;
   int            verbosity = RAW_DEFAULT_VERBOSITY;

   RawReadHints( const IsoString& hints )
   {
      preferences.LoadFromSettings();

      IsoStringList theHints;
      hints.Break( theHints, ' ', true/*trim*/ );
      theHints.Remove( IsoString() );
      bool preview = false;
      for ( IsoStringList::const_iterator token = theHints.Begin(); token != theHints.End(); ++token )
      {
         if ( *token == "raw" )
         {
            preferences.interpolation = RawPreferences::Bilinear;
            preferences.interpolateAs4Colors = false;
            preferences.useAutoWhiteBalance = false;
            preferences.useCameraWhiteBalance = false;
            preferences.noWhiteBalance = true;
            preferences.createSuperPixels = false;
            preferences.outputRawRGB = true;
            preferences.outputCFA = false;
            preferences.noBlackPointCorrection = true;
            preferences.noAutoFlip = true;
            preferences.noAutoCrop = false;
            preferences.noClipHighlights = true;
            preferences.noiseThreshold = 0;
            preview = false;
         }
         else if ( *token == "preview" )
         {
            preferences.interpolation = RawPreferences::HalfSize;
            preferences.interpolateAs4Colors = false;
            preferences.useAutoWhiteBalance = false;
            preferences.useCameraWhiteBalance = true;
            preferences.noWhiteBalance = false;
            preferences.createSuperPixels = false;
            preferences.outputRawRGB = false;
            preferences.outputCFA = false;
            preferences.noBlackPointCorrection = false;
            preferences.noAutoFlip = false;
            preferences.noAutoCrop = false;
            preferences.noClipHighlights = false;
            preferences.noiseThreshold = 0;
            preview = true;
         }
         else if ( *token == "rgb" )
         {
            preferences.interpolation = RawPreferences::VNG;
            preferences.interpolateAs4Colors = false;
            preferences.useAutoWhiteBalance = false;
            preferences.useCameraWhiteBalance = true;
            preferences.noWhiteBalance = false;
            preferences.createSuperPixels = false;
            preferences.outputRawRGB = false;
            preferences.outputCFA = false;
            preferences.noBlackPointCorrection = false;
            preferences.noAutoFlip = false;
            preferences.noAutoCrop = false;
            preferences.noClipHighlights = false;
            preferences.noiseThreshold = 0;
            preview = false;
         }

         // 'fast' must be interpreted differently if the 'preview' hint has
         // already been specified --- otherwise we wouldn't be 'fast' at all!
         else if ( *token == "fast" )
            preferences.interpolation = preview ? RawPreferences::HalfSize : RawPreferences::Bilinear;

         else if ( *token == "bilinear" )
            preferences.interpolation = RawPreferences::Bilinear;
         else if ( *token == "vng" )
            preferences.interpolation = RawPreferences::VNG;
         else if ( *token == "ppg" )
            preferences.interpolation = RawPreferences::PPG;
         else if ( *token == "ahd" )
            preferences.interpolation = RawPreferences::AHD;
         else if ( *token == "aahd" )
            preferences.interpolation = RawPreferences::AAHD;
         else if ( *token == "dht" )
            preferences.interpolation = RawPreferences::DHT;
         else if ( *token == "dcb" )
            preferences.interpolation = RawPreferences::DCB;

         else if ( *token == "dcb-iterations" )
         {
            if ( ++token == theHints.End() )
               break;
            int n;
            if ( token->TryToInt( n ) )
               preferences.dcbIterations = Range( n, 1, 5 );
         }

         else if ( *token == "dcb-refinement" )
            preferences.dcbRefinement = true;
         else if ( *token == "no-dcb-refinement" )
            preferences.dcbRefinement = false;

         else if ( *token == "half-size" )
            preferences.interpolation = RawPreferences::HalfSize;

         else if ( *token == "interpolate-as-4-colors" )
            preferences.interpolateAs4Colors = true;
         else if ( *token == "no-interpolate-as-4-colors" )
            preferences.interpolateAs4Colors = false;

         else if ( *token == "auto-white-balance" )
         {
            preferences.useAutoWhiteBalance = true;
            preferences.noWhiteBalance = false;
         }
         else if ( *token == "no-auto-white-balance" )
            preferences.useAutoWhiteBalance = false;

         else if ( *token == "camera-white-balance" )
         {
            preferences.useCameraWhiteBalance = true;
            preferences.noWhiteBalance = false;
         }
         else if ( *token == "no-camera-white-balance" )
            preferences.useCameraWhiteBalance = false;

         else if ( *token == "no-white-balance" )
            preferences.noWhiteBalance = true;
         else if ( *token == "daylight-white-balance" )
         {
            preferences.useAutoWhiteBalance = false;
            preferences.useCameraWhiteBalance = false;
            preferences.noWhiteBalance = false;
         }

         else if ( *token == "super-pixels" )
         {
            preferences.createSuperPixels = true;
            preferences.outputRawRGB = false;
            preferences.outputCFA = false;
         }
         else if ( *token == "no-super-pixels" )
            preferences.createSuperPixels = false;

         else if ( *token == "bayer-drizzle" )
         {
            preferences.outputRawRGB = true;
            preferences.outputCFA = false;
            preferences.createSuperPixels = false;
         }
         else if ( *token == "no-bayer-drizzle" )
            preferences.outputRawRGB = false;

         else if ( *token == "cfa" )
         {
            preferences.outputCFA = true;
            preferences.outputRawRGB = false;
            preferences.createSuperPixels = false;
         }
         else if ( *token == "no-cfa" )
            preferences.outputCFA = false;

         else if ( *token == "auto-flip" )
            preferences.noAutoFlip = false;
         else if ( *token == "no-auto-flip" )
            preferences.noAutoFlip = true;

         else if ( *token == "auto-crop" )
            preferences.noAutoCrop = false;
         else if ( *token == "no-auto-crop" )
            preferences.noAutoCrop = true;

         else if ( *token == "black-point-correction" )
            preferences.noBlackPointCorrection = false;
         else if ( *token == "no-black-point-correction" )
            preferences.noBlackPointCorrection = true;

         else if ( *token == "clip-highlights" )
            preferences.noClipHighlights = false;
         else if ( *token == "no-clip-highlights" )
            preferences.noClipHighlights = true;

         else if ( *token == "force-focal-length" )
            preferences.forceFocalLength = true;
         else if ( *token == "no-force-focal-length" )
            preferences.forceFocalLength = false;
         else if ( *token == "focal-length" )
         {
            if ( ++token == theHints.End() )
               break;
            double f;
            if ( token->TryToDouble( f ) )
               if ( f >= 0 )
               {
                  preferences.forceFocalLength = true;
                  preferences.focalLength = f;
               }
         }

         else if ( *token == "force-aperture" )
            preferences.forceAperture = true;
         else if ( *token == "no-force-aperture" )
            preferences.forceAperture = false;
         else if ( *token == "aperture" )
         {
            if ( ++token == theHints.End() )
               break;
            double d;
            if ( token->TryToDouble( d ) )
               if ( d >= 0 )
               {
                  preferences.forceAperture = true;
                  preferences.aperture = d;
               }
         }

         else if ( *token == "wavelet-noise-threshold" )
         {
            if ( ++token == theHints.End() )
               break;
            int t;
            if ( token->TryToInt( t ) )
               if ( t >= 0 )
                  preferences.noiseThreshold = t;
         }
         else if ( *token == "no-wavelet-noise-reduction" )
            preferences.noiseThreshold = 0;

         else if ( *token == "fbdd-noise-reduction" )
         {
            if ( ++token == theHints.End() )
               break;
            int n;
            if ( token->TryToInt( n ) )
               preferences.fbddNoiseReduction = Range( n, 0, 2 );
         }
         else if ( *token == "no-fbdd-noise-reduction" )
            preferences.fbddNoiseReduction = 0;

         else if ( *token == "verbosity" )
         {
            if ( ++token == theHints.End() )
               break;
            int n;
            if ( token->TryToInt( n ) )
               verbosity = Range( n, 0, 3 );
         }
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class RawProgress
{
public:

   RawProgress( RawInstance& instance )
      : m_instance( instance )
   {
      m_instance.m_raw->set_progress_handler( Callback, this );
   }

   ~RawProgress()
   {
      Complete();
   }

   void Complete()
   {
      if ( m_monitor.IsInitialized() )
         m_monitor.Complete();
   }

private:

   RawInstance&    m_instance;
   StatusMonitor   m_monitor;
   StandardStatus  m_status;
   LibRaw_progress m_lastStage = LIBRAW_PROGRESS_START;
   int             m_lastIteration = 0;

   static int Callback( void* data, LibRaw_progress stage, int iteration, int expected )
   {
#define P   reinterpret_cast<RawProgress*>( data )

      try
      {
         if ( stage != LIBRAW_PROGRESS_START )
         {
            if ( stage != P->m_lastStage )
            {
               P->m_lastStage = stage;

               if ( P->m_monitor.IsInitialized() )
               {
                  P->m_monitor.Complete();
                  P->m_monitor.Clear();
               }

               if ( iteration < expected )
               {
                  P->m_lastIteration = iteration;
                  P->m_monitor.SetCallback( &P->m_status );
                  P->m_monitor.Initialize( LibRaw::strprogress( stage ), expected );
                  if ( iteration > 0 )
                     P->m_monitor += iteration;
               }
               else
                  Console().WriteLn( "<end><cbr>" + String( LibRaw::strprogress( stage ) ) );
            }
            else
            {
               if ( iteration > P->m_lastIteration )
               {
                  if ( P->m_monitor.IsInitialized() )
                     P->m_monitor += iteration - P->m_lastIteration;
                  P->m_lastIteration = iteration;
               }
            }
         }

         return 0;
      }
      catch ( const ProcessAborted& )
      {
         return 1;
      }
      catch ( ... )
      {
         throw;
      }

#undef P
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

RawInstance::RawInstance( const RawFormat* F )
   : FileFormatImplementation( F )
{
}

// ----------------------------------------------------------------------------

RawInstance::~RawInstance() noexcept( false )
{
   Close();
}

// ----------------------------------------------------------------------------

String RawInstance::ExposureAsText() const
{
   String expText;
   if ( m_exposure >= 1 )
      return String().Format( "%.0fs", m_exposure );
   if ( m_exposure > 0 )
      return String().Format( "1/%.0fs", 1.0/m_exposure );
   return "<unknown>";
}

// ----------------------------------------------------------------------------

void RawInstance::CheckOpenStream( const String& memberFunc ) const
{
   if ( !IsOpen() )
      throw Error( "RawInstance::" + memberFunc + "(): Illegal request on a closed stream." );
}

// ----------------------------------------------------------------------------

void RawInstance::CheckLibRawReturnCode( int errorCode )
{
   if ( errorCode != 0 )
   {
      if ( errorCode > 0 )
         throw Error( ::strerror( errorCode ) );
      if ( errorCode == LIBRAW_CANCELLED_BY_CALLBACK )
         throw ProcessAborted();
      if ( LIBRAW_FATAL_ERROR( errorCode ) )
         throw Error( String().Format( "LibRaw (%d): ", errorCode ) + libraw_strerror( errorCode ) );
   }
}

// ----------------------------------------------------------------------------

#define EXIF_TAG_OffsetTime            36880
#define EXIF_TAG_OffsetTimeOriginal    36881
#define EXIF_TAG_OffsetTimeDigitized   36882

void RawInstance::LibRawEXIFCallback( void* context, int tag, int type, int len, unsigned int ord, void* ifp, int64 base )
{
   RawInstance* instance = reinterpret_cast<RawInstance*>( context );

   if ( tag == EXIF_TAG_OffsetTime
     || tag == EXIF_TAG_OffsetTimeOriginal
     || tag == EXIF_TAG_OffsetTimeDigitized && instance->m_timeOffset == RAW_TIMEOFFSET_UNKNOWN )
   {
      LibRaw_abstract_datastream* data = reinterpret_cast<LibRaw_abstract_datastream*>( ifp );
      IsoString value( '\0', size_type( len ) );
      data->read( value.Begin(), len, 1 );
      if ( value[3] == ':' && (value[0] == '-' || value[0] == '+') )
      {
         int hours;
         if ( value.Substring( 1, 2 ).TryToInt( hours, 10 ) )
            if ( hours >= 0 && hours <= 24 )
            {
               int minutes;
               if ( value.Substring( 4, 2 ).TryToInt( minutes, 10 ) )
                  if ( minutes >= 0 && minutes <= 60 )
                  {
                     instance->m_timeOffset = hours*60 + minutes;
                     if ( value[0] == '-' )
                        instance->m_timeOffset = -instance->m_timeOffset;
                  }
            }
      }
   }
}

// ----------------------------------------------------------------------------

static String EnabledOrDisabled( bool x )
{
   return x ? "enabled" : "disabled";
}

ImageDescriptionArray RawInstance::Open( const String& filePath, const IsoString& hints )
{
#define sizes     m_raw->imgdata.sizes
#define idata     m_raw->imgdata.idata
#define color     m_raw->imgdata.color
#define thumbnail m_raw->imgdata.thumbnail
#define other     m_raw->imgdata.other

   Close();

   Exception::EnableConsoleOutput();

   try
   {
      /*
       * Decode read hints.
       */
      {
         RawReadHints readHints( hints );
         m_preferences = readHints.preferences;
         m_verbosity = readHints.verbosity;
      }

      /*
       * Open the file with LibRaw.
       */
      m_filePath = filePath;

      m_raw = new LibRaw;

      if ( m_verbosity > 1 )
         m_progress = new RawProgress( *this );

      m_raw->set_exifparser_handler( LibRawEXIFCallback, this );

      {
#ifdef __PCL_WINDOWS
         // NB. wchar_t is the same as char16_t on Windows.
         String filePathWin = File::UnixPathToWindows( filePath );
         CheckLibRawReturnCode( m_raw->open_file( reinterpret_cast<const wchar_t*>( filePathWin.c_str() ) ) );
#else
         IsoString filePath8 = filePath.ToUTF8();
         CheckLibRawReturnCode( m_raw->open_file( filePath8.c_str() ) );
#endif
      }

      /*
      ByteArray data = File::ReadFile( filePath );
      CheckLibRawReturnCode( m_raw->open_buffer( data.Begin(), data.Length() ) );
      //CheckLibRawReturnCode( m_raw->adjust_sizes_info_only() );
      */

      bool xtrans = idata.filters == 9;
      bool fullColor = idata.filters == 0;
      bool foveon = idata.is_foveon;
      bool mosaiced = !(fullColor || foveon);
      bool raw = mosaiced && (m_preferences.outputCFA || m_preferences.outputRawRGB || m_preferences.createSuperPixels && !xtrans);
      bool noAutoCrop = raw && m_preferences.noAutoCrop
                            && m_preferences.noWhiteBalance
                            && m_preferences.noBlackPointCorrection
                            && m_preferences.noClipHighlights
                            && m_preferences.noiseThreshold == 0;

      /*
       * Descriptive metadata.
       */
      m_description = IsoString( other.desc ).Trimmed().UTF8ToUTF16();
      m_author = IsoString( other.artist ).Trimmed().UTF8ToUTF16();

      /*
       * Camera identification.
       */
      m_cameraManufacturer = IsoString( idata.make ).Trimmed();
      m_cameraModel = IsoString( idata.model ).Trimmed();

      if ( mosaiced )
      {
         /*
          * Get the CFA pattern of the raw image.
          */
         if ( xtrans )
         {
            const char* x = reinterpret_cast<const char*>( noAutoCrop ? idata.xtrans_abs : idata.xtrans );
            for ( int i = 0; i < 36; ++i )
               m_rawCFAPattern << idata.cdesc[int( x[i] )];

            m_cfaPatternName = "X-Trans";
         }
         else
         {
            m_rawCFAPattern = IsoString() << idata.cdesc[m_raw->COLOR( 0, 0 )]
                                          << idata.cdesc[m_raw->COLOR( 0, 1 )]
                                          << idata.cdesc[m_raw->COLOR( 1, 0 )]
                                          << idata.cdesc[m_raw->COLOR( 1, 1 )];

            m_cfaPatternName = "Bayer";
         }

         /*
          * CFA patterns refer to the top-left corner of the *visible* image
          * region. If we are not cropping invisible areas, rotate CFA patterns
          * horizontally and/or vertically, as necessary.
          */
         if ( noAutoCrop )
            if ( xtrans )
            {
               // 6x6 X-Trans pattern
               int dy = sizes.top_margin % 6;
               int dx = sizes.left_margin % 6;
               char* p = m_rawCFAPattern.Begin();

               if ( dy )
               {
                  char* t = new char[ 6*dy ];
                  memcpy( t, p, 6*dy );
                  memcpy( p, p + 6*dy, 36 - 6*dy );
                  memcpy( p + 36 - 6*dy, t, 6*dy );
                  delete [] t;
               }
               if ( dx )
               {
                  char* t = new char[ dx ];
                  for ( int i = 0; i < 6; ++i, p += 6 )
                  {
                     memcpy( t, p, dx );
                     memcpy( p, p + dx, 6-dx );
                     memcpy( p + 6-dx, t, dx );
                  }
                  delete [] t;
               }
            }
            else
            {
               // 2x2 Bayer pattern
               if ( sizes.top_margin & 1 )
               {
                  Swap( m_rawCFAPattern[0], m_rawCFAPattern[2] );
                  Swap( m_rawCFAPattern[1], m_rawCFAPattern[3] );
               }
               if ( sizes.left_margin & 1 )
               {
                  Swap( m_rawCFAPattern[0], m_rawCFAPattern[1] );
                  Swap( m_rawCFAPattern[2], m_rawCFAPattern[3] );
               }
            }

         /*
          * Get the CFA pattern of the output image. Rotate it if we are
          * loading with camera flipping enabled and the original frame has
          * been rotated by 180, 90 CCW or 90 CW degrees.
          */
         m_cfaPattern = m_rawCFAPattern;
         if ( !m_preferences.noAutoFlip )
            if ( raw )
               if ( xtrans )
                  switch ( sizes.flip )
                  {
                  case 0:
                     break;
                  case 3:  // 180 degrees
                     for ( int y0 = 0, y1 = 5; y0 < y1; ++y0, --y1 )
                        for ( int x0 = 0, x1 = 5; x0 < x1; ++x0, --x1 )
                        {
                           Swap( m_cfaPattern[6*y0 + x0], m_cfaPattern[6*y1 + x1] );
                           Swap( m_cfaPattern[6*y0 + x1], m_cfaPattern[6*y1 + x0] );
                        }
                     break;
                  case 5:  // 90 degrees counter-clockwise
                     {
                        IsoString tmp = m_cfaPattern;
                        for ( int y = 0, t = 0; y < 6; ++y )
                           for ( int x = 0; x < 6; ++x, ++t )
                              m_cfaPattern[(5 - x)*6 + y] = tmp[t];
                     }
                     break;
                  case 6:  // 90 degrees clockwise
                     {
                        IsoString tmp = m_cfaPattern;
                        for ( int y = 0, t = 0; y < 6; ++y )
                           for ( int x = 0; x < 6; ++x, ++t )
                              m_cfaPattern[x*6 + 5 - y] = tmp[t];
                     }
                     break;
                  }
               else
                  switch ( sizes.flip )
                  {
                  case 0:
                     break;
                  case 3:  // 180 degrees
                     Swap( m_cfaPattern[0], m_cfaPattern[3] );
                     Swap( m_cfaPattern[1], m_cfaPattern[2] );
                     break;
                  case 5:  // 90 degrees counter-clockwise
                     Swap( m_cfaPattern[0], m_cfaPattern[2] );
                     Swap( m_cfaPattern[1], m_cfaPattern[3] );
                     Swap( m_cfaPattern[0], m_cfaPattern[3] );
                     break;
                  case 6:  // 90 degrees clockwise
                     Swap( m_cfaPattern[0], m_cfaPattern[1] );
                     Swap( m_cfaPattern[2], m_cfaPattern[3] );
                     Swap( m_cfaPattern[0], m_cfaPattern[3] );
                     break;
                  }
      }

      /*
       * Camera RGB to sRGB conversion matrix.
       */
      m_sRGBConversionMatrix = F32Matrix( 3, idata.colors );
      for ( int i = 0; i < 3; ++i )
         for ( int j = 0; j < idata.colors; ++j )
            m_sRGBConversionMatrix[i][j] = color.rgb_cam[i][j];

      /*
       * Camera timestamp.
       */
      if ( other.timestamp != 0 )
         m_timestamp = TimePoint( other.timestamp );
      if ( m_timeOffset != RAW_TIMEOFFSET_UNKNOWN )
         m_timestamp -= m_timeOffset/1440.0;

      /*
       * GPS geodetic coordinates.
       */
      if ( other.parsed_gps.gpsparsed )
         if ( other.parsed_gps.longitude[0] != 0 || other.parsed_gps.longitude[1] != 0 || other.parsed_gps.longitude[2] != 0
           || other.parsed_gps.latitude[0] != 0 || other.parsed_gps.latitude[1] != 0 || other.parsed_gps.latitude[2] != 0
           || other.parsed_gps.altitude != 0 )
         {
            m_gpsLongitude = other.parsed_gps.longitude[0]
                           + other.parsed_gps.longitude[1]/60.0
                           + other.parsed_gps.longitude[2]/3600.0;
            if ( other.parsed_gps.longref == 'W' )
               m_gpsLongitude = -m_gpsLongitude;

            m_gpsLatitude  = other.parsed_gps.latitude[0]
                           + other.parsed_gps.latitude[1]/60.0
                           + other.parsed_gps.latitude[2]/3600.0;
            if ( other.parsed_gps.latref == 'S' )
               m_gpsLatitude = -m_gpsLatitude;

            m_gpsAltitude = other.parsed_gps.altitude;
            if ( other.parsed_gps.altref == 1 )
               m_gpsAltitude = -m_gpsAltitude;
         }

      /*
       * Exposure time in seconds, ISO speed, focal length in mm, and f/d
       * aperture.
       */
      m_exposure = other.shutter;
      m_isoSpeed = other.iso_speed;
      m_focalLength = other.focal_len;
      m_aperture = other.aperture;

      /*
       * Check for nonexistent or invalid metadata items. These have caused
       * problems in the old dcraw times, so checking them here doesn't hurt.
       */
      if ( !IsFinite( m_exposure ) )
         m_exposure = 0;
      if ( !IsFinite( m_focalLength ) )
         m_focalLength = 0;
      if ( !IsFinite( m_aperture ) )
         m_aperture = 0;

      /*
       * Forced focal length and aperture via preferences / read hints
       */
      if ( m_preferences.forceFocalLength )
         m_focalLength = m_preferences.focalLength;
      if ( m_preferences.forceAperture )
         if ( m_focalLength > 0 && m_preferences.aperture > 0 )
            m_aperture = m_focalLength/m_preferences.aperture;
         else
            m_aperture = 0;

      /*
       * Geometry and color space image properties.
       */
      ImageInfo i;
      i.supported        = idata.filters >= 1000 || xtrans || fullColor || foveon;
      i.colorSpace       = m_preferences.outputCFA ? ColorSpace::Gray : ColorSpace::RGB;
      i.numberOfChannels = m_preferences.outputCFA ? 1 : 3;

      if ( noAutoCrop )
      {
         i.width  = sizes.raw_width;
         i.height = sizes.raw_height;
      }
      else
      {
         i.width  = sizes.width;
         i.height = sizes.height;
         if ( !raw )
            if ( m_preferences.interpolation == RawPreferences::HalfSize )
            {
               i.width  >>= 1;
               i.height >>= 1;
            }
      }

      if ( !m_preferences.noAutoFlip )
         if ( sizes.flip == 5 || sizes.flip == 6 ) // rotated by +/-90 deg?
            Swap( i.width, i.height );

      /*
       * Format-independent image properties.
       */
      ImageOptions o;
      o.bitsPerSample      = 16;
      o.complexSample      = false;
      o.embedICCProfile    = color.profile != nullptr &&
                                 reinterpret_cast<const ICCProfile*>( color.profile )->IsProfile();
      o.embedThumbnail     = thumbnail.tlength > 0 && (thumbnail.tformat == LIBRAW_THUMBNAIL_JPEG || thumbnail.tformat == LIBRAW_THUMBNAIL_BITMAP);
      o.ieeefpSampleFormat = false;
      o.xResolution        = 100;
      o.yResolution        = 100;
      o.lowerRange         = 0;
      o.upperRange         = 65535;
      o.isoSpeed           = m_isoSpeed;
      o.exposure           = m_exposure;
      o.focalLength        = m_focalLength;
      o.aperture           = m_aperture;

      /*
       * Be verbose as requested.
       */
      if ( m_verbosity > 0 )
      {
         m_progress->Complete();

         Console console;
         console.WriteLn( "<end><cbr>" );
         if ( !m_cameraModel.IsEmpty() )
            console.WriteLn(                     "Camera ............ <raw>" + m_cameraManufacturer + ' ' + m_cameraModel + "</raw>" );
         if ( m_timestamp.IsValid() )
            console.WriteLn(                     "Timestamp ......... " + m_timestamp.ToString( 3/*timeItems*/, 0/*precision*/ ) );
         if ( m_timeOffset != RAW_TIMEOFFSET_UNKNOWN )
            console.WriteLn(                     "Time offset ....... " + String().Format( "%c%02d:%02d",
                                                 (m_timeOffset < 0) ? '-' : '+', Abs( m_timeOffset )/60, Abs( m_timeOffset )%60 ) );
         if ( m_gpsLongitude != RAW_GPS_UNKNOWN )
            console.WriteLn(                     "GPS coordinates ... " + String().ToSexagesimal( Abs( m_gpsLongitude ),
                                                                           SexagesimalConversionOptions( 3/*items*/,
                                                                                                         2/*precision*/,
                                                                                                         false/*sign*/,
                                                                                                         0/*width*/,
                                                                                                         ' '/*separator*/ ) )
                                                 + ' ' + ((m_gpsLongitude < 0) ? 'W' : 'E')
                                                 + "  " + String().ToSexagesimal( Abs( m_gpsLatitude ),
                                                                           SexagesimalConversionOptions( 3/*items*/,
                                                                                                         2/*precision*/,
                                                                                                         false/*sign*/,
                                                                                                         0/*width*/,
                                                                                                         ' '/*separator*/ ) )
                                                 + ' ' + ((m_gpsLatitude < 0) ? 'S' : 'N')
                                                 + String().Format( "  %.2f m", m_gpsAltitude ) );
         if ( m_exposure > 0 )
            console.WriteLn(                     "Exposure .......... " + ExposureAsText() );
         if ( m_isoSpeed > 0 )
            console.WriteLn( String().Format(    "ISO speed ......... %d", m_isoSpeed ) );
         if ( m_focalLength > 0 )
         {
            console.WriteLn( String().Format(    "Focal length ...... %.2f mm%s",
                                                 m_focalLength,
                                                 m_preferences.forceFocalLength ? " (forced)" : "" ) );
            if ( m_aperture > 0 )
               console.WriteLn( String().Format( "Aperture .......... f/%.2f = %.2f mm%s",
                                                 m_aperture,
                                                 m_focalLength/m_aperture,
                                                 m_preferences.forceAperture ? " (forced)" : "" ) );
         }
         if ( o.embedICCProfile )
            console.WriteLn(                     "ICC profile ....... <raw>"
                                                 + reinterpret_cast<const ICCProfile*>( color.profile )->Description() + "</raw>" );
         if ( !m_author.IsEmpty() )
            console.WriteLn(                     "Author ............ <raw>" + m_author + "</raw>" );
         if ( !m_rawCFAPattern.IsEmpty() )
            console.WriteLn(                     "CFA pattern ....... "
                                                 + m_cfaPatternName + ' ' + m_rawCFAPattern
                                                 + ((m_cfaPattern != m_rawCFAPattern) ? " (" + m_cfaPattern + ")" : IsoString()) );
         console.WriteLn( String().Format(       "Raw dimensions .... w=%d h=%d", sizes.raw_width, sizes.raw_height ) );
         console.WriteLn( String().Format(       "Image geometry .... x=%d y=%d w=%d h=%d",
                                                 sizes.left_margin, sizes.top_margin, sizes.width, sizes.height ) );
         if ( sizes.flip > 0 )
            console.WriteLn( String().Format(    "Image rotation .... %d deg", (sizes.flip == 5) ? 90 : ((sizes.flip == 6) ? 270 : 180) ) );
         console.WriteLn( "<end><cbr>" );

         if ( m_verbosity > 1 )
         {
            console.WriteLn( "<end><cbr>Raw decoding parameters:" );
            console.WriteLn(                     "Output mode ............... " + m_preferences.OutputModeAsString() );
            if ( raw )
            {
               console.WriteLn(                  "Auto crop ................. " + EnabledOrDisabled( !noAutoCrop ) );
            }
            else
            {
               if ( mosaiced )
                  console.Write(                 "Interpolation ............. " );
               else
                  console.Write(                 "Full-color frame .......... " );

               if ( xtrans )
                  console.WriteLn( "X-Trans" );
               else if ( fullColor )
                  console.WriteLn( "RGB" );
               else if ( foveon )
                  console.WriteLn( "Foveon" );
               else
               {
                  console.WriteLn( m_preferences.InterpolationAsString() );
                  if ( m_preferences.interpolation == RawPreferences::DCB )
                  {
                     console.WriteLn(            "DCB iterations ............ " + String( m_preferences.dcbIterations ) );
                     console.WriteLn(            "DCB refinement ............ " + EnabledOrDisabled( m_preferences.dcbRefinement ) );
                  }
                  console.WriteLn(               "Interpolate as 4 colors ... " + EnabledOrDisabled( m_preferences.interpolateAs4Colors ) );
               }

               if ( mosaiced && !xtrans )
                  console.WriteLn(               "FBDD noise reduction ...... " + String( m_preferences.fbddNoiseReduction ) );
            }
            console.WriteLn(                     "Wavelet noise threshold ... " + String( m_preferences.noiseThreshold ) );
            console.WriteLn(                     "White balancing ........... " + m_preferences.WhiteBalancingAsString() );
            console.WriteLn(                     "Black point correction .... " + EnabledOrDisabled( !m_preferences.noBlackPointCorrection  ) );
            console.WriteLn(                     "Highlights clipping ....... " + EnabledOrDisabled( !m_preferences.noClipHighlights ) );
            console.WriteLn(                     "Auto rotate ............... " + EnabledOrDisabled( !m_preferences.noAutoFlip ) );
            console.WriteLn( String().Format(    "Output image .............. w=%d h=%d n=%d %s", i.width, i.height, i.numberOfChannels,
                                                                              (i.colorSpace == ColorSpace::RGB) ? "RGB" : "Gray" ) );
            console.WriteLn( "<end><cbr>" );
         }
      }

      return ImageDescriptionArray() << ImageDescription( i, o );
   }
   catch ( ... )
   {
      Close();
      throw;
   }

#undef sizes
#undef idata
#undef color
#undef thumbnail
#undef other
}

// ----------------------------------------------------------------------------

bool RawInstance::IsOpen() const
{
   return !m_raw.IsNull();
}

// ----------------------------------------------------------------------------

String RawInstance::FilePath() const
{
   return m_filePath;
}

// ----------------------------------------------------------------------------

void RawInstance::Close()
{
   m_raw.Reset();
   m_progress.Reset();
   m_description = m_author = String();
   m_cameraManufacturer = m_cameraModel = m_cfaPattern = m_rawCFAPattern = m_cfaPatternName = IsoString();
   m_sRGBConversionMatrix = F32Matrix();
   m_timestamp = TimePoint();
   m_timeOffset = RAW_TIMEOFFSET_UNKNOWN;
   m_gpsLongitude = m_gpsLatitude = m_gpsAltitude = RAW_GPS_UNKNOWN;
   m_exposure = m_isoSpeed = m_focalLength = m_aperture = 0;
}

// ----------------------------------------------------------------------------

String RawInstance::ImageFormatInfo() const
{
   if ( !IsOpen() )
      return String();

   return String( "camera=" ) + '\'' + m_cameraManufacturer + ' ' + m_cameraModel + '\'' + ' ' +
          String( "ISO=" ) + String( m_isoSpeed ) + ' ' +
          String( "exposure=" ) + ExposureAsText();
}

// ----------------------------------------------------------------------------

ICCProfile RawInstance::ReadICCProfile()
{
   CheckOpenStream( "ReadICCProfile" );

   if ( m_raw->imgdata.color.profile != nullptr )
   {
      const ICCProfile* icc = reinterpret_cast<const ICCProfile*>( m_raw->imgdata.color.profile );
      if ( icc->IsProfile() )
         return *icc;
   }
   return ICCProfile();
}

// ----------------------------------------------------------------------------

FITSKeywordArray RawInstance::ReadFITSKeywords()
{
   if ( !IsOpen() )
      return FITSKeywordArray();

   FITSKeywordArray keywords;

   keywords << FITSHeaderKeyword( "COMMENT",
                                  IsoString(),
                                  "Decoded with " + PixInsightVersion::AsString().ToIsoString() )
            << FITSHeaderKeyword( "COMMENT",
                                  IsoString(),
                                  "Decoded with " + Module->ReadableVersion() )
            << FITSHeaderKeyword( "COMMENT",
                                  IsoString(),
                                  IsoString( "Decoded with LibRaw version " ) + LibRaw::version() );

   if ( !m_cameraModel.IsEmpty() )
      keywords << FITSHeaderKeyword( "INSTRUME", '\'' + m_cameraManufacturer + ' ' + m_cameraModel + '\'', "Camera model" );

   if ( m_timestamp.IsValid() )
      keywords << FITSHeaderKeyword( "DATE-OBS", '\'' + m_timestamp.ToIsoString(
                              3/*timeItems*/, 0/*precision*/, 0/*tz*/, false/*timeZone*/ ) + '\'', "Camera timestamp (UTC)" );

   if ( m_gpsLongitude != RAW_GPS_UNKNOWN && m_gpsLatitude != RAW_GPS_UNKNOWN && m_gpsAltitude != RAW_GPS_UNKNOWN &&
        (m_gpsLongitude != 0 || m_gpsLatitude != 0 || m_gpsAltitude != 0) )
   {
      keywords.Add( FITSHeaderKeyword( "OBSGEO-L", IsoString().Format( "%.10g", m_gpsLongitude ), "GPS longitude (deg)" ) );
      keywords.Add( FITSHeaderKeyword( "OBSGEO-B", IsoString().Format( "%.10g", m_gpsLatitude ), "GPS latitude (deg)" ) );
      keywords.Add( FITSHeaderKeyword( "OBSGEO-H", IsoString().Format( "%.0f", m_gpsAltitude ), "GPS altitude (m)" ) );
   }

   if ( m_exposure > 0 )
      keywords.Add( FITSHeaderKeyword( "EXPTIME", IsoString().Format( "%.6f", m_exposure ), "Exposure time in seconds" ) );

   if ( m_isoSpeed > 0 )
      keywords.Add( FITSHeaderKeyword( "ISOSPEED", IsoString().Format( "%d", m_isoSpeed ), "ISO speed as specified in ISO 12232" ) );

   if ( m_focalLength > 0 )
   {
      keywords.Add( FITSHeaderKeyword( "FOCALLEN", IsoString().Format( "%.2f", m_focalLength ), "Focal length in mm" ) );
      if ( m_aperture > 0 )
         keywords.Add( FITSHeaderKeyword( "APTDIA", IsoString().Format( "%.2f", m_focalLength/m_aperture ), "Aperture diameter in mm" ) );
   }

   return keywords;
}

// ----------------------------------------------------------------------------

PropertyDescriptionArray RawInstance::ImagePropertyDescriptions()
{
   CheckOpenStream( "ImagePropertyDescriptions" );

   PropertyDescriptionArray descriptions;
   if ( !m_cfaPattern.IsEmpty() )
   {
      descriptions << PropertyDescription( "PCL:CFASourcePattern", VariantType::IsoString );
      if ( m_rawCFAPattern != m_cfaPattern )
         descriptions << PropertyDescription( "PCL:RawCFASourcePattern", VariantType::IsoString );
      descriptions << PropertyDescription( "PCL:CFASourcePatternName", VariantType::IsoString )
                   << PropertyDescription( "PCL:sRGBConversionMatrix", VariantType::F32Matrix );
   }
   if ( !m_cameraModel.IsEmpty() )
      descriptions << PropertyDescription( "Instrument:Camera:Name", VariantType::String );
   if ( m_isoSpeed > 0 )
      descriptions << PropertyDescription( "Instrument:Camera:ISOSpeed", VariantType::Int32 );
   if ( m_exposure > 0 )
      descriptions << PropertyDescription( "Instrument:ExposureTime", VariantType::Float32 );
   if ( m_focalLength > 0 )
   {
      descriptions << PropertyDescription( "Instrument:Telescope:FocalLength", VariantType::Float32 );
      if ( m_aperture > 0 )
         descriptions << PropertyDescription( "Instrument:Telescope:Aperture", VariantType::Float32 );
   }
   if ( !m_description.IsEmpty() )
      descriptions << PropertyDescription( "Observation:Description", VariantType::String );
   if ( m_timestamp.IsValid() )
      descriptions << PropertyDescription( "Observation:Time:Start", VariantType::TimePoint );
   if ( m_gpsLongitude != RAW_GPS_UNKNOWN && m_gpsLatitude != RAW_GPS_UNKNOWN && m_gpsAltitude != RAW_GPS_UNKNOWN &&
        (m_gpsLongitude != 0 || m_gpsLatitude != 0 || m_gpsAltitude != 0) )
   {
      descriptions << PropertyDescription( "Observation:Location:Longitude", VariantType::Float64 )
                   << PropertyDescription( "Observation:Location:Latitude", VariantType::Float64 )
                   << PropertyDescription( "Observation:Location:Elevation", VariantType::Float32 );
   }
   if ( !m_author.IsEmpty() )
      descriptions << PropertyDescription( "Observer:Name", VariantType::String );

   return descriptions;
}

// ----------------------------------------------------------------------------

Variant RawInstance::ReadImageProperty( const IsoString& property )
{
   CheckOpenStream( "ReadImageProperty" );

   if ( property == "PCL:CFASourcePattern" )
      return m_cfaPattern;
   if ( property == "PCL:RawCFASourcePattern" )
      return m_rawCFAPattern;
   if ( property == "PCL:CFASourcePatternName" )
      return m_cfaPatternName;
   if ( property == "PCL:sRGBConversionMatrix" )
      return m_sRGBConversionMatrix;
   if ( property == "Instrument:Camera:Name" )
      return String( m_cameraManufacturer + ' ' + m_cameraModel );
   if ( property == "Instrument:Camera:ISOSpeed" )
      return int32( m_isoSpeed );
   if ( property == "Instrument:ExposureTime" )
      return m_exposure;
   if ( property == "Instrument:Telescope:Aperture" )
      return (m_aperture > 0) ? m_focalLength/m_aperture/1000 : .0F;
   if ( property == "Instrument:Telescope:FocalLength" )
      return m_focalLength/1000;
   if ( property == "Observation:Description" )
      return m_description;
   if ( property == "Observation:Time:Start" )
      return m_timestamp;
   if ( property == "Observation:Location:Longitude" )
      return m_gpsLongitude;
   if ( property == "Observation:Location:Latitude" )
      return m_gpsLatitude;
   if ( property == "Observation:Location:Elevation" )
      return m_gpsAltitude;
   if ( property == "Observer:Name" )
      return m_author;

   return Variant();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Fast color filter array (CFA) pixel sample selection.
 * Code borrowed from the DrizzleIntegration process.
 */
class CFAIndex
{
public:

   typedef GenericMatrix<bool>   cfa_channel_index;

   CFAIndex() = default;
   CFAIndex( const CFAIndex& ) = default;

   CFAIndex( const IsoString& pattern )
   {
      if ( pattern.IsEmpty() )
         throw Error( "Empty CFA pattern." );
      m_size = int( Sqrt( pattern.Length() ) );
      if ( m_size < 2 )
         throw Error( "Invalid CFA pattern '" + pattern + '\'' );
      if ( m_size*m_size != pattern.Length() )
         throw Error( "Non-square CFA patterns are not supported: '" + pattern + '\'' );
      for ( int c = 0; c < 3; ++c )
      {
         m_index[c] = cfa_channel_index( m_size, m_size );
         IsoString::const_iterator p = pattern.Begin();
         for ( int i = 0; i < m_size; ++i )
            for ( int j = 0; j < m_size; ++j )
               m_index[c][i][j] = *p++ == "RGB"[c];
      }
   }

   operator bool() const
   {
      return m_size > 0;
   }

   bool operator ()( int chn, int row, int col ) const
   {
      return m_index[chn][row % m_size][col % m_size];
   }

private:

   int               m_size = 0;
   cfa_channel_index m_index[ 3 ]; // RGB
};

// ----------------------------------------------------------------------------

class RawImageReader
{
public:

#define preferences  instance.m_preferences
#define RAW          instance.m_raw
#define idata        instance.m_raw->imgdata.idata
#define params       instance.m_raw->imgdata.params
#define sizes        instance.m_raw->imgdata.sizes
#define color        instance.m_raw->imgdata.color
#define rawdata      instance.m_raw->imgdata.rawdata

   template <class P>
   static void Read( GenericImage<P>& image, RawInstance& instance )
   {
      instance.CheckOpenStream( "ReadImage" );

      instance.CheckLibRawReturnCode( RAW->unpack() );

      bool xtrans = idata.filters == 9;
      bool fullColor = idata.filters == 0;
      bool foveon = idata.is_foveon;
      bool mosaiced = !(fullColor || foveon);
      bool raw = mosaiced && (preferences.outputCFA || preferences.outputRawRGB || preferences.createSuperPixels && !xtrans);

      /*
       * Set up dcraw processing parameters.
       */

      // Linear 16-bit output
      params.output_bps = 16;

      // Output color space: raw
      params.output_color = 0;

      // Disable LibRaw's default histogram transformation
      params.no_auto_bright = 1;

      // Disable LibRaw's default gamma curve transformation
      params.gamm[0] = params.gamm[1] = 1.0;

      if ( preferences.noWhiteBalance )
      {
         // Disable white balancing
         for ( size_type i = 0; i < ItemsInArray( params.user_mul ); ++i )
            params.user_mul[i] = 1;
         params.use_auto_wb = params.use_camera_wb = 0;
         params.use_camera_matrix = 0;
      }
      else
      {
         // Automatic white balance
         if ( preferences.useAutoWhiteBalance )
            params.use_auto_wb = 1;
         // Camera white balance
         if ( preferences.useCameraWhiteBalance )
            params.use_camera_wb = 1;
      }

      // Interpolation and related parameters
      if ( raw )
      {
         params.no_interpolation = 1;
      }
      else if ( !xtrans )
      {
         if ( preferences.interpolation == RawPreferences::HalfSize )
            params.half_size = 1;
         else
         {
            int q = 0;
            switch ( preferences.interpolation )
            {
            case RawPreferences::Bilinear: q =  0; break;
            default:
            case RawPreferences::VNG:      q =  1; break;
            case RawPreferences::PPG:      q =  2; break;
            case RawPreferences::AHD:      q =  3; break;
            case RawPreferences::DCB:      q =  4; break;
            case RawPreferences::DHT:      q = 11; break;
            case RawPreferences::AAHD:     q = 12; break;
            }
            params.user_qual = q;

            if ( preferences.interpolation == RawPreferences::DCB )
            {
               params.dcb_iterations = preferences.dcbIterations;
               params.dcb_enhance_fl = preferences.dcbRefinement;
            }
         }

         if ( preferences.interpolateAs4Colors )
            params.four_color_rgb = 1;

         params.fbdd_noiserd = preferences.fbddNoiseReduction;
      }

      // No automatic camera flip
      if ( raw || preferences.noAutoFlip )
         params.user_flip = 0;

      // No black point adjustment
      if ( preferences.noBlackPointCorrection )
         params.user_black = 0;

      // Do not clip highlights
      if ( preferences.noClipHighlights )
         params.highlight = 1;

      // Wavelet noise reduction
      if ( preferences.noiseThreshold > 0 )
         params.threshold = preferences.noiseThreshold;

      /*
       * Generate the output image.
       */
      if ( raw )
      {
         if ( preferences.noWhiteBalance
           && preferences.noBlackPointCorrection
           && preferences.noClipHighlights
           && preferences.noiseThreshold == 0 )
         {
            /*
             * Output pure raw data
             */
            const uint16* u = rawdata.raw_image;
            if ( u == nullptr )
               throw Error( "LibRaw: Null raw image data" );

            int width, height;
            if ( preferences.noAutoCrop )
            {
               width = sizes.raw_width;
               height = sizes.raw_height;
            }
            else
            {
               width = sizes.width;
               height = sizes.height;
               u += sizes.top_margin*sizes.raw_width + sizes.left_margin;
            }

            if ( preferences.outputCFA )
            {
               /*
                * Output monochrome CFA pure raw image.
                */
               image.AllocateData( width, height, 1, ColorSpace::Gray );
               typename GenericImage<P>::sample_iterator i( image );
               for ( int y = 0; y < height; ++y )
               {
                  const uint16* U = u + y*sizes.raw_width;
                  for ( int x = 0; x < width; ++x, ++i, ++U )
                     *i = P::ToSample( *U );
               }
            }
            else if ( preferences.outputRawRGB )
            {
               /*
                * Output RGB pure raw image.
                */
               image.AllocateData( width, height, 3, ColorSpace::RGB );
               image.Zero();
               CFAIndex index( instance.m_rawCFAPattern );
               typename GenericImage<P>::pixel_iterator i( image );
               for ( int y = 0; y < height; ++y )
               {
                  const uint16* U = u + y*sizes.raw_width;
                  for ( int x = 0; x < width; ++x, ++i, ++U )
                  {
                     typename P::sample v = P::ToSample( *U );
                     if ( index( 1, y, x ) )
                        i[1] = v;
                     else if ( index( 0, y, x ) )
                        i[0] = v;
                     else if ( index( 2, y, x ) )
                        i[2] = v;
                  }
               }
            }
            else if ( preferences.createSuperPixels )
            {
               /*
                * Output RGB pure superpixels image.
                */
               CFAIndex index( instance.m_rawCFAPattern );
               int Rr = 0, Rc = 0, G1r = -1, G1c = 0, G2r = 0, G2c = 0, Br = 0, Bc = 0;
               for ( int i = 0; i < 2; ++i )
                  for ( int j = 0; j < 2; ++j )
                     if ( index( 1, i, j ) )
                     {
                        if ( G1r < 0 )
                           G1r = i, G1c = j;
                        else
                           G2r = i, G2c = j;
                     }
                     else if ( index( 0, i, j ) )
                        Rr = i, Rc = j;
                     else if ( index( 2, i, j ) )
                        Br = i, Bc = j;

               image.AllocateData( width >> 1, height >> 1, 3, ColorSpace::RGB );
               typename GenericImage<P>::pixel_iterator i( image );
               for ( int y = 0; y < height-1; y += 2 )
               {
                  const uint16* U[2];
                  U[0] = u + y*sizes.raw_width;
                  U[1] = U[0] + sizes.raw_width;
                  for ( int x = 0; x < width-1; x += 2, ++i, U[0] += 2, U[1] += 2 )
                  {
                     i[0] = P::ToSample( U[Rr][Rc] );
                     i[1] = P::ToSample( double( int( U[G1r][G1c] ) + int( U[G2r][G2c] ) )/2/uint16_max );
                     i[2] = P::ToSample( U[Br][Bc] );
                  }
               }
            }
            else
               throw Error( "RawImageReader::Read(): Internal error: invalid raw output mode" );
         }
         else
         {
            /*
             * Output preprocessed raw data with one or more of white
             * balancing, black level subtraction and highlights clipping
             * applied.
             */
            instance.CheckLibRawReturnCode( RAW->dcraw_process() );
            const uint16 (*u)[4] = RAW->imgdata.image;
            if ( u == nullptr )
               throw Error( "LibRaw: Null preprocessed raw image data" );

            if ( preferences.outputCFA )
            {
               /*
                * Output monochrome CFA preprocessed raw image.
                */
               image.AllocateData( sizes.width, sizes.height, 1, ColorSpace::Gray );
               typename GenericImage<P>::sample_iterator i( image );
               for ( int y = 0; y < sizes.height; ++y )
               {
                  const uint16 (*U)[4] = u + y*sizes.width;
                  for ( int x = 0; x < sizes.width; ++x, ++i )
                     *i = P::ToSample( U[x][RAW->COLOR( y, x )] );
               }
            }
            else if ( preferences.outputRawRGB )
            {
               /*
                * Output RGB preprocessed raw image.
                */
               image.AllocateData( sizes.width, sizes.height, 3, ColorSpace::RGB );
               typename GenericImage<P>::pixel_iterator i( image );
               for ( int y = 0; y < sizes.height; ++y )
               {
                  const uint16 (*U)[4] = u + y*sizes.width;
                  for ( int x = 0; x < sizes.width; ++x, ++i )
                     for ( int c = 0; c < 3; ++c )
                        i[c] = P::ToSample( U[x][c] );
               }
            }
            else if ( preferences.createSuperPixels )
            {
               /*
                * Output RGB preprocessed superpixels image.
                */
               image.AllocateData( sizes.width >> 1, sizes.height >> 1, 3, ColorSpace::RGB );
               typename GenericImage<P>::pixel_iterator i( image );
               for ( int y = 0; y < sizes.height-1; y += 2 )
               {
                  const uint16 (*U0)[4] = u + y*sizes.width;
                  const uint16 (*U1)[4] = U0 + sizes.width;
                  for ( int x = 0; x < sizes.width-1; x += 2, ++i )
                  {
                     i[0] = P::ToSample( double( U0[x][0] + U0[x+1][0] + U1[x][0] + U1[x+1][0] )/uint16_max );
                     i[1] = P::ToSample( double( int( U0[x][1] ) + int( U0[x+1][1] )
                                               + int( U1[x][1] ) + int( U1[x+1][1] ) )/2/uint16_max );
                     i[2] = P::ToSample( double( U0[x][2] + U0[x+1][2] + U1[x][2] + U1[x+1][2] )/uint16_max );
                  }
               }
            }
            else
               throw Error( "RawImageReader::Read(): Internal error: invalid raw output mode" );
         }

         /*
          * Apply camera rotation.
          */
         if ( !preferences.noAutoFlip )
            switch ( sizes.flip )
            {
            case 0:
               break;
            case 3:
               Rotate180() >> image;
               break;
            case 5:
               Rotate90CCW() >> image;
               break;
            case 6:
               Rotate90CW() >> image;
               break;
            }
      }
      else
      {
         /*
          * Output demosaiced / postprocessed RGB image.
          */
         instance.CheckLibRawReturnCode( RAW->dcraw_process() );
         int ret;
         libraw_processed_image_t* p = RAW->dcraw_make_mem_image( &ret );
         if ( p != nullptr )
         {
            bool valid = p->type == LIBRAW_IMAGE_BITMAP
                     && (p->colors == 3 || p->colors == 1)
                     && (p->bits == 16 || p->bits == 8);
            if ( valid )
            {
               image.AllocateData( p->width, p->height, p->colors, (p->colors == 3) ? ColorSpace::RGB : ColorSpace::Gray );
               if ( p->bits == 16 )
               {
                  const uint16* u = reinterpret_cast<const uint16*>( p->data );
                  for ( typename GenericImage<P>::pixel_iterator i( image ); i; ++i )
                     for ( int j = 0; j < p->colors; ++j, ++u )
                        i[j] = P::ToSample( *u );
               }
               else // p->bits == 8
               {
                  const uint8* u = reinterpret_cast<const uint8*>( p->data );
                  for ( typename GenericImage<P>::pixel_iterator i( image ); i; ++i )
                     for ( int j = 0; j < p->colors; ++j, ++u )
                        i[j] = P::ToSample( *u );
               }
            }

            LibRaw::dcraw_clear_mem( p );

            if ( !valid )
               throw Error( "LibRaw: Invalid postprocessed image format" );
         }
         else
            instance.CheckLibRawReturnCode( ret );
      }
   }

#undef preferences
#undef RAW
#undef idata
#undef params
#undef sizes
#undef color
#undef rawdata
};

// ----------------------------------------------------------------------------

void RawInstance::ReadImage( Image& image )
{
   RawImageReader::Read( image, *this );
}

// ----------------------------------------------------------------------------

void RawInstance::ReadImage( DImage& image )
{
   RawImageReader::Read( image, *this );
}

// ----------------------------------------------------------------------------

void RawInstance::ReadImage( UInt8Image& image )
{
   RawImageReader::Read( image, *this );
}

// ----------------------------------------------------------------------------

void RawInstance::ReadImage( UInt16Image& image )
{
   RawImageReader::Read( image, *this );
}

// ----------------------------------------------------------------------------

void RawInstance::ReadImage( UInt32Image& image )
{
   RawImageReader::Read( image, *this );
}

// ----------------------------------------------------------------------------

UInt8Image RawInstance::ReadThumbnail()
{
#define thumbnail m_raw->imgdata.thumbnail

   CheckOpenStream( "ReadImage" );

   UInt8Image T;

   if ( thumbnail.tlength > 0 )
   {
      if ( thumbnail.tformat == LIBRAW_THUMBNAIL_JPEG )
      {
         CheckLibRawReturnCode( m_raw->unpack_thumb() );
         Bitmap bmp( thumbnail.thumb, thumbnail.tlength, "JPG" );
         T.AllocateData( bmp.Width(), bmp.Height(), 3, ColorSpace::RGB );
         T.Blend( bmp );
      }
      else if ( thumbnail.tformat == LIBRAW_THUMBNAIL_BITMAP )
      {
         CheckLibRawReturnCode( m_raw->unpack_thumb() );
         T.AllocateData( thumbnail.twidth, thumbnail.theight, 3, ColorSpace::RGB );
         const char* t = thumbnail.thumb;
         for ( UInt8Image::pixel_iterator i( T ); i; ++i )
         {
            i[0] = uint8( *t++ );
            i[1] = uint8( *t++ );
            i[2] = uint8( *t++ );
         }
      }
   }

   return T;

#undef thumbnail
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RawInstance.cpp - Released 2024-12-28T16:54:05Z
