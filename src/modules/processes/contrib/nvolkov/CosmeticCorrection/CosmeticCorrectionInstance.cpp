//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 1.4.0
// ----------------------------------------------------------------------------
// CosmeticCorrectionInstance.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
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

#include "CosmeticCorrectionInstance.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileFormat.h>
#include <pcl/ICCProfile.h>
#include <pcl/MetaModule.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/MuteStatus.h>
#include <pcl/StandardStatus.h>
#include <pcl/Version.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CosmeticCorrectionInstance::CosmeticCorrectionInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_outputDir( TheOutputDir->DefaultValue() )
   , p_outputExtension( TheOutputExtension->DefaultValue() ) // ### DEPRECATED
   , p_overwrite( TheOverwrite->DefaultValue() )
   , p_prefix( ThePrefix->DefaultValue() )
   , p_postfix( ThePostfix->DefaultValue() )
   , p_cfa( TheCFA->DefaultValue() )
   , p_amount( TheAmount->DefaultValue() )
   , p_useMasterDark( TheUseMasterDark->DefaultValue() )
   , p_masterDark( TheMasterPath->DefaultValue() )
   , p_hotDarkCheck( TheHotDarkCheck->DefaultValue() )
   , p_hotDarkLevel( TheHotLevel->DefaultValue() )
   , p_coldDarkCheck( TheColdAutoCheck->DefaultValue() )
   , p_coldDarkLevel( TheColdLevel->DefaultValue() )
   , p_useAutoDetect( TheUseAutoDetect->DefaultValue() )
   , p_hotAutoCheck( TheHotAutoCheck->DefaultValue() )
   , p_hotAutoValue( TheHotAutoValue->DefaultValue() )
   , p_coldAutoCheck( TheColdAutoCheck->DefaultValue() )
   , p_coldAutoValue( TheColdAutoValue->DefaultValue() )
   , p_useDefectList( TheUseDefectList->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

CosmeticCorrectionInstance::CosmeticCorrectionInstance( const CosmeticCorrectionInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void CosmeticCorrectionInstance::Assign( const ProcessImplementation& p )
{
   const CosmeticCorrectionInstance* x = dynamic_cast<const CosmeticCorrectionInstance*>( &p );
   if ( x != nullptr )
   {
      p_targetFrames = x->p_targetFrames;
      p_masterDark = x->p_masterDark;
      p_outputDir = x->p_outputDir;
      p_outputExtension = x->p_outputExtension;
      p_overwrite = x->p_overwrite;
      p_prefix = x->p_prefix;
      p_postfix = x->p_postfix;
      p_cfa = x->p_cfa;
      p_amount = x->p_amount;
      p_useMasterDark = x->p_useMasterDark;
      p_hotDarkCheck = x->p_hotDarkCheck;
      p_hotDarkLevel = x->p_hotDarkLevel;
      p_coldDarkCheck = x->p_coldDarkCheck;
      p_coldDarkLevel = x->p_coldDarkLevel;
      p_useAutoDetect = x->p_useAutoDetect;
      p_hotAutoCheck = x->p_hotAutoCheck;
      p_hotAutoValue = x->p_hotAutoValue;
      p_coldAutoCheck = x->p_coldAutoCheck;
      p_coldAutoValue = x->p_coldAutoValue;
      p_useDefectList = x->p_useDefectList;
      p_defects = x->p_defects;
   }
}

// ----------------------------------------------------------------------------

bool CosmeticCorrectionInstance::CanExecute( String& whyNot ) const
{
   if ( p_useMasterDark && !p_masterDark.IsEmpty() && ( p_hotDarkCheck || p_coldDarkCheck ) && !File::Exists( p_masterDark ) )
      whyNot = "MasterDark not found: " + p_masterDark;
   else if ( !(
                ( p_useMasterDark && !p_masterDark.IsEmpty() && ( p_hotDarkCheck || p_coldDarkCheck ) )
                || ( p_useAutoDetect && ( p_hotAutoCheck || p_coldAutoCheck ) )
                || ( p_useDefectList && !p_defects.IsEmpty() ) ) )
      whyNot = "Nothing to do. Choose appropriate method for locating defective pixels.";
   else
      return true;

   return false;
}

// ----------------------------------------------------------------------------

bool CosmeticCorrectionInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "CosmeticCorrection can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool CosmeticCorrectionInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool CosmeticCorrectionInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_targetFrames.IsEmpty() )
   {
      whyNot = "No target frames have been specified.";
      return false;
   }
   else
   {
      for ( image_list::const_iterator i = p_targetFrames.Begin(); i != p_targetFrames.End(); ++i )
         if ( i->enabled && !File::Exists( i->path ) )
         {
            whyNot = "File not found: " + i->path;
            return false;
         }
   }

   if ( !p_outputDir.IsEmpty() && !File::DirectoryExists( p_outputDir ) )
   {
      whyNot = "The specified output directory does not exist: " + p_outputDir;
      return false;
   }
   else
      return CanExecute( whyNot );
}

// ----------------------------------------------------------------------------

MorphologicalTransformation* CosmeticCorrectionInstance::BkgMT() const
{
   static MorphologicalTransformation MT;
   if ( p_cfa )
   {
      const char* Box[] = // define bacground CFA area arround star
         {
            "x-x-x-x-x-x-x"
            "-------------"
            "x-x-x-x-x-x-x"
            "-------------"
            "x-x-------x-x"
            "-------------"
            "x-x-------x-x"
            "-------------"
            "x-x-------x-x"
            "-------------"
            "x-x-x-x-x-x-x"
            "-------------"
            "x-x-x-x-x-x-x"
         };

      BitmapStructure s( Box, 13 );
      MT.SetStructure( s );
   }
   else
   {
      const char* Box[] = // define bacground area arround star
         {
            "xxxxxxx"
            "xxxxxxx"
            "xx---xx"
            "xx---xx"
            "xx---xx"
            "xxxxxxx"
            "xxxxxxx"
         };

      BitmapStructure s( Box, 7 );
      MT.SetStructure( s );
   }

   MT.SetOperator( MedianFilter() );
   return &MT;
}

// ----------------------------------------------------------------------------

MorphologicalTransformation* CosmeticCorrectionInstance::MedMT() const
{
   static MorphologicalTransformation MT;
   if ( p_cfa )
   {
      const char* Box[] = {
         "x-x-x"
         "-----"
         "x---x"
         "-----"
         "x-x-x"
      };

      BitmapStructure s( Box, 5 );
      MT.SetStructure( s );
   }
   else
   {
      const char* Box[] = // define area arround pixel
         {
            "xxx"
            "x-x"
            "xxx"
         };

      BitmapStructure s( Box, 3 );
      MT.SetStructure( s );
   }
   MT.SetOperator( MedianFilter() );
   return &MT;
}

// ----------------------------------------------------------------------------

MorphologicalTransformation* CosmeticCorrectionInstance::AvrMT() const
{
   static MorphologicalTransformation MT;
   if ( p_cfa )
   {
      const char* Box[] = {
         "x-x-x"
         "-----"
         "x---x"
         "-----"
         "x-x-x"
      };

      BitmapStructure s( Box, 5 );
      MT.SetStructure( s );
   }
   else
   {
      const char* Box[] = // define area arround pixel
         {
            "xxx"
            "x-x"
            "xxx"
         };

      BitmapStructure s( Box, 3 );
      MT.SetStructure( s );
   }
   MT.SetOperator( AlphaTrimmedMeanFilter( 0 ) );
   return &MT;
}

// ----------------------------------------------------------------------------

struct FileData
{
   FileFormat* format = nullptr; // the file format of retrieved data
   const void* fsData = nullptr; // format-specific data
   ImageOptions options;         // currently used for resolution only
   PropertyArray properties;     // image properties
   FITSKeywordArray keywords;    // FITS keywords
   ICCProfile profile;           // ICC profile

   FileData() = default;

   FileData( FileFormatInstance& file, const ImageOptions& o )
      : options( o )
   {
      format = new FileFormat( file.Format() );

      if ( format->UsesFormatSpecificData() )
         fsData = file.FormatSpecificData();

      if ( format->CanStoreImageProperties() )
         properties = file.ReadImageProperties();

      if ( format->CanStoreKeywords() )
         file.ReadFITSKeywords( keywords );

      if ( format->CanStoreICCProfiles() )
         file.ReadICCProfile( profile );
   }

   FileData( const FileData& ) = delete;

   FileData( FileData&& x )
      : format( x.format )
      , fsData( x.fsData )
      , options( std::move( x.options ) )
      , properties( std::move( x.properties ) )
      , keywords( std::move( x.keywords ) )
      , profile( std::move( x.profile ) )
   {
      x.format = nullptr;
      x.fsData = nullptr;
   }

   ~FileData()
   {
      if ( format != nullptr )
      {
         if ( fsData != nullptr )
            format->DisposeFormatSpecificData( const_cast<void*>( fsData ) ), fsData = nullptr;
         delete format, format = nullptr;
      }
   }

   FileData& operator=( const FileData& ) = delete;

   FileData& operator=( FileData&& x )
   {
      format = x.format;
      x.format = nullptr;
      fsData = x.fsData;
      x.fsData = nullptr;
      options = std::move( x.options );
      properties = std::move( x.properties );
      keywords = std::move( x.keywords );
      profile = std::move( x.profile );
      return *this;
   }
};

// ----------------------------------------------------------------------------

Image* LoadImageFile( FileFormatInstance& file, int index = 0 )
{
   if ( !file.SelectImage( index ) )
      throw CaughtException();
   Image* image = new Image( (void*)0, 0, 0 );
   if ( !file.ReadImage( *image ) )
      throw CaughtException();
   return image;
}

// ----------------------------------------------------------------------------

String UniqueFilePath( const String& filePath )
{
   for ( unsigned u = 1;; ++u )
   {
      String tryFilePath = File::AppendToName( filePath, '_' + String( u ) );
      if ( !File::Exists( tryFilePath ) )
         return tryFilePath;
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// CosmeticCorrection Thread
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct CCThreadData
{
   MorphologicalTransformation* avrMT = nullptr;
   MorphologicalTransformation* medMT = nullptr;
   MorphologicalTransformation* bkgMT = nullptr;
};

class CCThread : public Thread
{
public:

   CCThread( Image* t, FileData* fd, const String& tp, int i,
             const CCThreadData& d, const CosmeticCorrectionInstance* _instance )
      : target( t )
      , fileData( fd )
      , targetPath( tp )
      , subimageIndex( i )
      , success( false )
      , data( d )
   {
      instance = _instance;
      count = 0;
   }

   virtual ~CCThread()
   {
      if ( target != nullptr )
         delete target, target = nullptr;
      if ( fileData != nullptr )
         delete fileData, fileData = nullptr;
   }

   void Run() override
   {
      try
      {
         success = false;

         /*
          * Prepare filtered images according to selected methods (a, m, b => average, median, background )
          * AutoHot  : a m b
          * AutoCold :   m b
          * DarkHot  : a
          * DarkCold :   m
          */

         MuteStatus status;
         target->SetStatusCallback( &status );
         target->Status().DisableInitialization();

         Image avr, med, bkg;
         bool needAvr = false, needMed = false, needBkg = false;

         if ( instance->p_useAutoDetect )
         {
            if ( instance->p_hotAutoCheck )
               needAvr = needMed = needBkg = true;
            if ( instance->p_coldAutoCheck )
               needMed = needBkg = true;
         }

         if ( needAvr )
            avr.Assign( *target ), (*data.avrMT) >> avr; // prepare surrounding neighbors Mean
         if ( needMed )
            med.Assign( *target ), (*data.medMT) >> med; // prepare surrounding neighbors Median
         if ( needBkg )
            bkg.Assign( *target ), (*data.bkgMT) >> bkg; // prepare background Median

         const float f0 = instance->p_amount;
         const float f1 = 1 - f0;

         const int width = target->Width();
         const int height = target->Height();

         for ( int c = 0; c < target->NumberOfChannels(); ++c )
         {
            if ( instance->m_mapDarkHot )
            {
               const MapImg::sample* map = instance->m_mapDarkHot->PixelData( Min( c, instance->m_mapDarkHot->NumberOfChannels() - 1 ) );
               for ( int y = 0; y < height; ++y )
               {
                  if ( TryIsAborted() )
                     return;

                  for ( int x = 0; x < width; ++x, ++map )
                     if ( *map != 0 )
                     {
                        count++;
                        const Image::sample v = GetAverage3x3( target, x, y, c, width, height );
                        target->Pixel( x, y, c ) = v * f0 + target->Pixel( x, y, c ) * f1;
                     }
               }
            }

            if ( instance->m_mapDarkCold )
            {
               const MapImg::sample* map = instance->m_mapDarkCold->PixelData( Min( c, instance->m_mapDarkCold->NumberOfChannels() - 1 ) );
               for ( int y = 0; y < height; ++y )
               {
                  if ( TryIsAborted() )
                     return;

                  for ( int x = 0; x < width; ++x, ++map )
                     if ( *map != 0 )
                     {
                        count++;
                        const Image::sample v = GetMedian5x5( target, x, y, c, width, height );
                        target->Pixel( x, y, c ) = v * f0 + target->Pixel( x, y, c ) * f1;
                     }
               }
            }

            if ( instance->p_useAutoDetect && (instance->p_coldAutoCheck || instance->p_hotAutoCheck) )
            {
               target->SelectChannel( c );

               double min, max;
               target->GetExtremeSampleValues( min, max );

               /*
                * Reject saturated areas for median calculation.
                */
               target->SetRangeClipping( min + 1.0/65535, max - 1.0/65535 );
               double median = target->Median();

               /*
                * Robust estimate of scale: trimmed mean deviation from the median.
                */
               target->SetRangeClipping( median - 0.9*(median - min),
                                         median + 0.9*(max - median) );
               double avgDev = target->AvgDev( median );

               target->ResetSelections();

               if ( instance->p_hotAutoCheck )
               {
                  if ( TryIsAborted() )
                     return;

                  Image::sample* t = target->PixelData( c );
                  const Image::sample* end = t + target->NumberOfPixels();
                  const Image::sample* m = med.PixelData( c );
                  const Image::sample* b = bkg.PixelData( c );
                  const Image::sample* a = avr.PixelData( c );

                  const double k1 = avgDev;
                  const double k2 = k1 / 2;                        // avgDev / 2
                  const double k3 = instance->p_hotAutoValue * k1; // avgDev * k

                  while ( t < end )
                  {
                     if (  ( *a < *b + k2 ) // ignore pixel surrounded by other bright pixels at avgDev/2
                        && ( *t > *b + k1 ) // ignore pixel with brightnes less then (background + avgDev)
                        && ( *t > *m + k3 ) // ignore pixel with brightnes less then avg of neighbor pixels * k * avgDev
                        )
                     {
                        ++count;
                        *t = *a * f0 + *t * f1;
                     }
                     ++t, ++m, ++b, ++a;
                  }
               }

               if ( instance->p_coldAutoCheck )
               {
                  if ( TryIsAborted() )
                     return;

                  Image::sample* t = target->PixelData( c );
                  const Image::sample* end = t + target->NumberOfPixels();
                  const Image::sample* m = med.PixelData( c );
                  const Image::sample* b = bkg.PixelData( c );

                  const double k = avgDev * instance->p_coldAutoValue; // avgDev * how much pixel must be less
                  while ( t < end )
                  {
                     const double T = *t + k;
                     if ( T < *b && T < *m )
                     {
                        ++count;
                        *t = *b * f0 + *t * f1;
                     }
                     ++t, ++m, ++b;
                  }
               }
            }

            if ( instance->p_useDefectList && !instance->p_defects.IsEmpty() )
            {
               for ( size_t i = 0; i < instance->p_defects.Length(); ++i )
               {
                  if ( TryIsAborted() )
                     return;

                  const CosmeticCorrectionInstance::DefectItem& item = instance->p_defects[i];
                  if ( !item.enabled )
                     continue; // skip distable defects

                  int h = height - 1, w = width - 1;
                  if ( item.isRow )
                     Swap( w, h );

                  int x = item.address; // address in mainView coordinate
                  if ( x > w )
                     continue; // skip because the defect out of image view

                  int y0 = 0, y1 = h; // first and last possible pixel
                  if ( item.isRange )
                  {
                     y0 = Min( item.begin, item.end ); // chouse minimum value for begining coordinate
                     if ( y0 > h )
                        continue;                      // skip out of view defects
                     y1 = Max( item.begin, item.end ); // chouse maximum value for ending coordinate
                     y1 = Min( y1, h );                // cut out of view defective pixels
                  }

                  for ( int y = y0; y <= y1; ++y )
                     if ( item.isRow )
                     {
                        Image::sample v = GetMedian5x5( target, y, x, c, height, width );
                        target->Pixel( y, x, c ) = v * f0 + target->Pixel( y, x, c ) * f1;
                        ++count;
                     }
                     else
                     {
                        Image::sample v = GetMedian5x5( target, x, y, c, width, height );
                        target->Pixel( x, y, c ) = v * f0 + target->Pixel( x, y, c ) * f1;
                        ++count;
                     }
               }
            }
         } // for each channel

         success = true;

      } // try
      catch ( ... )
      {
         ClearConsoleOutputText();
         try
         {
            throw;
         }
         ERROR_HANDLER
      }
   }

   const Image* TargetImage() const
   {
      return target;
   }

   String TargetPath() const
   {
      return targetPath;
   }

   const FileData& GetFileData() const
   {
      return *fileData;
   }

   int SubimageIndex() const
   {
      return subimageIndex;
   }

   size_t Count() const
   {
      return count;
   }

   bool Success() const
   {
      return success;
   }

private:

   const CosmeticCorrectionInstance* instance = nullptr;

   Image*    target = nullptr;   // The image being CosmeticCorrected. It belongs to this thread.
   FileData* fileData = nullptr; // Target image parameters and embedded data. It belongs to this thread.
   String    targetPath;         // File path of this target image
   int       subimageIndex = 0;  // > 0 in case of a multiple image; = 0 otherwise
   size_t    count = 0;          // count of corrected pixels in the image
   bool      success = false;    // The thread completed execution successfully

   const CCThreadData& data;

   Image::sample GetMedian5x5( const Image* t, const int x, const int y, const int chanel, const int width, const int height ) const
   {
      int step, radius;
      if ( instance->p_cfa )
         step = 2, radius = 4;
      else
         step = 1, radius = 2;

      int n = 0;
      GenericVector<Image::sample> pixels( 24 ); // 24 = (2*2+1)*(2*2+1) - 1

      for ( int i = x - radius; i <= x + radius; i += step )
      {
         for ( int j = y - radius; j <= y + radius; j += step )
         {
            if ( i >= 0 && i < width )                            // ignore out of image pixel
               if ( j >= 0 && j < height )                        // ignore out of image pixel
                  if ( x != i && y != j )                         // for ignore curent processed pixel
                     pixels[n++] = target->Pixel( i, j, chanel ); // store pixel value to array
         }
      }

      return pcl::Median( pixels.Begin(), pixels.At( n ) );
   }

   Image::sample GetAverage3x3( const Image* t, const int x, const int y, const int chanel, const int width, const int height ) const
   {
      int step, radius;
      if ( instance->p_cfa )
         step = 2, radius = 2;
      else
         step = 1, radius = 1;

      int n = 0;
      Image::sample v = 0;
      for ( int i = x - radius; i <= x + radius; i += step )
      {
         for ( int j = y - radius; j <= y + radius; j += step )
            if ( i >= 0 && i < width )
               if ( j >= 0 && j < height )
                  if ( x != i && y != j )
                     v += target->Pixel( i, j, chanel ), n++;
      }
      return v / n;
   }
};

// ----------------------------------------------------------------------------

DarkImg CosmeticCorrectionInstance::GetDark( const String& filePath )
{
   Console().WriteLn( "<end><cbr>Loading MasterDark image:" );
   Console().WriteLn( filePath );
   Module->ProcessEvents();

   FileFormat format( File::ExtractExtension( filePath ), true, false );
   FileFormatInstance file( format );
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath ) )
      throw CaughtException();
   if ( images.IsEmpty() )
      throw Error( filePath + ": Empty MasterDark image." );
   //if ( images.Length() > 1 ) throw Error( filePath + ": Multiple images cannot be used as MasterDark." );
   if ( !file.SelectImage( 0 ) ) // which one of Multiple images is masterDark ? I think is #0. I am right?
      throw CaughtException();
   DarkImg img;
   if ( !file.ReadImage( img ) || !file.Close() )
      throw CaughtException();
   m_geometry = img.Bounds(); //set geometry from MasterDark.
   return img;
}

// ----------------------------------------------------------------------------

void CosmeticCorrectionInstance::PrepareMasterDarkMaps()
{
   if ( !( p_useMasterDark && !p_masterDark.IsEmpty() && ( p_hotDarkCheck || p_coldDarkCheck ) ) )
      return;

   DarkImg mdImage( GetDark( p_masterDark ) );
   const bool cfaMode = p_cfa && mdImage.IsColor(); // true == if CFA is RGB space, false == non CFA or mono CFA

   if ( p_hotDarkCheck )
   {
      const uint16 hotBad = Histogram().HistogramLevel( p_hotDarkLevel );
      Console().WriteLn( String().Format( "MasterDark hot map threshold: %f", p_hotDarkLevel ) );
      m_mapDarkHot = new MapImg( mdImage.Width(), mdImage.Height(), mdImage.IsColor() ? ColorSpace::RGB : ColorSpace::Gray );
      m_mapDarkHot->Zero();
      for ( int c = 0; c < mdImage.NumberOfNominalChannels(); ++c )
      {
         MapImg::sample* m = m_mapDarkHot->PixelData( c );
         const MapImg::sample* end = m + m_mapDarkHot->NumberOfPixels();
         const DarkImg::sample* d = mdImage.PixelData( c );
         while ( m < end )
         {
            if ( ( *d > 0 || !cfaMode ) && ( *d >= hotBad ) )
               *m = -1;
            m++, d++;
         }
      }
   }

   if ( p_coldDarkCheck )
   {
      const uint16 coldBad = Histogram().HistogramLevel( p_coldDarkLevel );
      Console().WriteLn( String().Format( "MasterDark cold map threshold: %f", p_coldDarkLevel ) );
      m_mapDarkCold = new MapImg( mdImage.Width(), mdImage.Height(), mdImage.IsColor() ? ColorSpace::RGB : ColorSpace::Gray );
      m_mapDarkCold->Zero();
      for ( int c = 0; c < mdImage.NumberOfNominalChannels(); ++c )
      {
         MapImg::sample* m = m_mapDarkCold->PixelData( c );
         const MapImg::sample* end = m + m_mapDarkCold->NumberOfPixels();
         const DarkImg::sample* d = mdImage.PixelData( c );
         while ( m < end )
         {
            if ( ( *d > 0 || !cfaMode ) && ( *d <= coldBad ) )
               *m = -1;
            m++, d++;
         }
      }
   }
}

// ----------------------------------------------------------------------------

thread_list CosmeticCorrectionInstance::LoadTargetFrame( const String& filePath, const CCThreadData& threadData )
{
   Console console;
   console.WriteLn( "Open " + filePath );

   Image* target = nullptr;
   thread_list threads;

   FileFormat format( File::ExtractExtension( filePath ), true, false );
   FileFormatInstance file( format );
   ImageDescriptionArray images;

   try
   {
      if ( !file.Open( images, filePath ) )
         throw CaughtException();
      if ( images.IsEmpty() )
         throw Error( filePath + ": Empty image file." );

      for ( unsigned int index = 0; index < images.Length(); ++index )
      {
         if ( images.Length() > 1 )
            console.WriteLn( String().Format( "* Subimage %u of %u", index + 1, images.Length() ) );
         target = LoadImageFile( file, index );
         Module->ProcessEvents();
         if ( m_geometry.IsRect() && ( target->Bounds() != m_geometry ) )
         {
            throw Error( "Image and MasterDark geometries are not equal." );
         }
         FileData* inputData = new FileData( file, images[index].options );
         threads.Add( new CCThread( target, inputData, filePath, index, threadData, this ) );
         target = nullptr;
      }
      console.WriteLn( "Close " + filePath );
      if ( !file.Close() )
         throw CaughtException();
   }
   catch ( ... )
   {
      if ( target != nullptr )
         delete target;
      if ( file.IsOpen() )
         file.Close();
      try
      {
         throw;
      }
      ERROR_HANDLER;
   }
   return threads;
}

// ----------------------------------------------------------------------------

String CosmeticCorrectionInstance::OutputFilePath( const String& filePath, const size_t index )
{
   String dir = p_outputDir;
   dir.Trim();
   if ( dir.IsEmpty() )
      dir = File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath );
   if ( dir.IsEmpty() )
      throw Error( dir + ": Unable to determine an output directory." );
   if ( !dir.EndsWith( '/' ) )
      dir.Append( '/' );

   const String ext = ".xisf";

   String fileName = File::ExtractName( filePath );
   fileName.Trim();
   if ( !p_prefix.IsEmpty() )
      fileName.Prepend( p_prefix );
   if ( index > 0 )
      fileName.Append( String().Format( "_%02d", index + 1 ) );
   if ( !p_postfix.IsEmpty() )
      fileName.Append( p_postfix );
   if ( fileName.IsEmpty() )
      throw Error( filePath + ": Unable to determine an output file name." );

   String outputFilePath = dir + fileName + ext;
   Console().WriteLn( "<end><cbr><br>Writing output file: " + outputFilePath );

   if ( File::Exists( outputFilePath ) )
      if ( p_overwrite )
         Console().WarningLn( "** Warning: Overwriting already existing file." );
      else
      {
         outputFilePath = UniqueFilePath( outputFilePath );
         Console().NoteLn( "* File already exists, writing to: " + outputFilePath );
      }

   return outputFilePath;
}

// ----------------------------------------------------------------------------

void CosmeticCorrectionInstance::SaveImage( const CCThread* t )
{
   Console console;
   String outputFilePath = OutputFilePath( t->TargetPath(), t->SubimageIndex() );
   console.WriteLn( "<end><cbr>Create file: " + outputFilePath );

   FileFormat outputFormat( File::ExtractExtension( outputFilePath ), false /*read*/, true /*write*/ );
   FileFormatInstance outputFile( outputFormat );
   if ( !outputFile.Create( outputFilePath ) )
      throw CaughtException();

   const FileData& inputData = t->GetFileData();

   outputFile.SetOptions( inputData.options );

   if ( inputData.fsData != nullptr )
      if ( outputFormat.UsesFormatSpecificData() )
         if ( outputFormat.ValidateFormatSpecificData( inputData.fsData ) )
            outputFile.SetFormatSpecificData( inputData.fsData );

   if ( !inputData.properties.IsEmpty() )
      if ( outputFormat.CanStoreImageProperties() )
         outputFile.WriteImageProperties( inputData.properties );
      else
         console.WarningLn( "** Warning: The output format cannot store image properties - existing properties not embedded" );

   if ( outputFormat.CanStoreKeywords() )
   {
      FITSKeywordArray keywords = inputData.keywords;
      keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "CosmeticCorrection with " + PixInsightVersion::AsString() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), Module->ReadableVersion() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "CosmeticCorrection: Total corrected pixels = " + IsoString( t->Count() ) );
      outputFile.WriteFITSKeywords( keywords );
   }
   else if ( !inputData.keywords.IsEmpty() )
      console.WarningLn( "** Warning: The output format cannot store FITS keywords - existing keywords not embedded" );

   if ( inputData.profile.IsProfile() )
      if ( outputFormat.CanStoreICCProfiles() )
         outputFile.WriteICCProfile( inputData.profile );
      else
         console.WarningLn( "** Warning: The output format cannot store ICC profiles - existing profile not embedded" );

   if ( !outputFile.WriteImage( *t->TargetImage() ) || !outputFile.Close() )
      throw CaughtException();
}

// ----------------------------------------------------------------------------

bool CosmeticCorrectionInstance::ExecuteGlobal()
{
   Console console;
   console.Show();

   m_mapDarkHot = m_mapDarkCold = nullptr;
   m_geometry = 0;

   try
   {
      console.EnableAbort();

      PrepareMasterDarkMaps();

      ImageItem item;

      size_t succeeded = 0;
      size_t skipped = 0;
      int numberOfThreads = Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 );
      thread_list runningThreads( Min( int( p_targetFrames.Length() ), numberOfThreads ) );
      thread_list waitingThreads;
      image_list targets( p_targetFrames );

      CCThreadData threadData;
      threadData.avrMT = AvrMT();
      threadData.medMT = MedMT();
      threadData.bkgMT = BkgMT();

      console.WriteLn( String().Format( "<br>CosmeticCorrection of %u target frames:", p_targetFrames.Length() ) );
      console.WriteLn( String().Format( "* Using %u worker threads", runningThreads.Length() ) );

      try
      {
         int running = 0;
         do
         {
            // Keep the GUI responsive
            Module->ProcessEvents();
            if ( console.AbortRequested() )
               throw ProcessAborted();

            /*
             * Open File
             */
            if ( !targets.IsEmpty() && waitingThreads.IsEmpty() )
            {
               item = *targets;                   // take one from the top of targets list
               targets.Remove( targets.Begin() ); // remove top file from the list
               console.WriteLn( String().Format( "<br>File %u of %u",
                  p_targetFrames.Length() - targets.Length(), p_targetFrames.Length() ) );
               if ( item.enabled )
               {
                  waitingThreads = LoadTargetFrame( item.path, threadData ); // put all sub-images from file to waitingThreads
                  if ( waitingThreads.IsEmpty() )                            // sothing wrong with target image. Maybe incompatible geometry or etc.
                  {
                     ++skipped;
                     console.NoteLn( "* Skipping target on error" );
                  }
               }
               else
               {
                  ++skipped;
                  console.NoteLn( "* Skipping disabled target" );
               }
            }

            /*
             * Find idle or free CPU
             */
            thread_list::iterator i = nullptr;
            for ( thread_list::iterator j = runningThreads.Begin(); j != runningThreads.End(); ++j )
            {
               if ( *j == nullptr ) // the CPU is free and empty.
               {
                  if ( !waitingThreads.IsEmpty() ) // there are not processed images
                  {
                     i = j;
                     break; // i pointed to CPU which is free now.
                  }
               }                               // *j != nullptr -> the CPU is not free and maybe idle or active
               else if ( !(*j)->IsActive() ) // the CPU is idle = the CPU has finished processing
               {
                  i = j;
                  break; // i pointed to CPU thread which ready to save.
               }
            }

            if ( i == nullptr ) // all CPUs active or no new images
            {
               pcl::Sleep( 100 );
               continue;
            }

            /*
             * Write File
             */
            if ( *i != nullptr ) // the CPU is idle
            {
               --running;
               try
               {
                  if ( !(*i)->Success() )
                     throw Error( ( *i )->ConsoleOutputText() );

                  console.WriteLn( String().Format( "<br>CPU #%u done. %u pixels corrected.", i - runningThreads.Begin(), ( *i )->Count() ) );
                  SaveImage( *i );
                  runningThreads.Delete( i ); // prepare thread for next image. now (*i == nullptr) the CPU is free
               }
               catch ( ... )
               {
                  runningThreads.Delete( i );
                  throw;
               }
               ++succeeded;
            }

            // Keep the GUI responsive
            Module->ProcessEvents();
            if ( console.AbortRequested() )
               throw ProcessAborted();

            /*
             * Put image to empty runningThreads slot and Run()
             */
            if ( !waitingThreads.IsEmpty() )
            {
               *i = *waitingThreads;                            //put one sub-image to runningThreads. so, now (*i != 0)
               waitingThreads.Remove( waitingThreads.Begin() ); //remove one sub-image from waitingThreads
               console.WriteLn( String().Format( "<br>CPU #%u processing file ", i - runningThreads.Begin() ) + item.path );
               (*i)->Start( ThreadPriority::DefaultMax, i - runningThreads.Begin() );
               running++;
            }
         } while ( running > 0 || !targets.IsEmpty() );
      }
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER;

         console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate ..." );
         for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
            if ( *i != nullptr )
               (*i)->Abort();
         for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
            if ( *i != nullptr )
               (*i)->Wait();
         runningThreads.Destroy();
         waitingThreads.Destroy();
         throw;
      }

      if ( m_mapDarkHot != nullptr )
         delete m_mapDarkHot, m_mapDarkHot = nullptr;
      if ( m_mapDarkCold != nullptr )
         delete m_mapDarkCold, m_mapDarkCold = nullptr;

      console.NoteLn( String().Format(
         "<br>===== CosmeticCorrection: %u succeeded, %u skipped, %u canceled =====",
         succeeded, skipped, p_targetFrames.Length() - succeeded ) );

      return true;
   }
   catch ( ... )
   {
      if ( m_mapDarkHot != nullptr )
         delete m_mapDarkHot, m_mapDarkHot = nullptr;
      if ( m_mapDarkCold != nullptr )
         delete m_mapDarkCold, m_mapDarkCold = nullptr;

      console.NoteLn( "<end><cbr><br>* CosmeticCorrection terminated." );
      throw;
   }
}

// ----------------------------------------------------------------------------

void* CosmeticCorrectionInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheTargetFrameEnabled )
      return &p_targetFrames[tableRow].enabled;
   if ( p == TheTargetFramePath )
      return p_targetFrames[tableRow].path.Begin();

   if ( p == TheOutputDir )
      return p_outputDir.Begin();
   if ( p == TheOutputExtension )
      return p_outputExtension.Begin();
   if ( p == ThePrefix )
      return p_prefix.Begin();
   if ( p == ThePostfix )
      return p_postfix.Begin();
   if ( p == TheOverwrite )
      return &p_overwrite;

   if ( p == TheAmount )
      return &p_amount;
   if ( p == TheCFA )
      return &p_cfa;

   if ( p == TheUseMasterDark )
      return &p_useMasterDark;
   if ( p == TheMasterPath )
      return p_masterDark.Begin();
   if ( p == TheHotDarkCheck )
      return &p_hotDarkCheck;
   if ( p == TheHotLevel )
      return &p_hotDarkLevel;
   if ( p == TheColdDarkCheck )
      return &p_coldDarkCheck;
   if ( p == TheColdLevel )
      return &p_coldDarkLevel;

   if ( p == TheUseAutoDetect )
      return &p_useAutoDetect;
   if ( p == TheHotAutoCheck )
      return &p_hotAutoCheck;
   if ( p == TheHotAutoValue )
      return &p_hotAutoValue;
   if ( p == TheColdAutoCheck )
      return &p_coldAutoCheck;
   if ( p == TheColdAutoValue )
      return &p_coldAutoValue;

   if ( p == TheUseDefectList )
      return &p_useDefectList;
   if ( p == TheDefectEnabled )
      return &p_defects[tableRow].enabled;
   if ( p == TheDefectIsRow )
      return &p_defects[tableRow].isRow;
   if ( p == TheDefectAddress )
      return &p_defects[tableRow].address;
   if ( p == TheDefectIsRange )
      return &p_defects[tableRow].isRange;
   if ( p == TheDefectBegin )
      return &p_defects[tableRow].begin;
   if ( p == TheDefectEnd )
      return &p_defects[tableRow].end;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool CosmeticCorrectionInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheTargetFrames )
   {
      p_targetFrames.Clear();
      if ( sizeOrLength > 0 )
         p_targetFrames.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheTargetFramePath )
   {
      p_targetFrames[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_targetFrames[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheMasterPath )
   {
      p_masterDark.Clear();
      if ( sizeOrLength > 0 )
         p_masterDark.SetLength( sizeOrLength );
   }
   else if ( p == TheOutputDir )
   {
      p_outputDir.Clear();
      if ( sizeOrLength > 0 )
         p_outputDir.SetLength( sizeOrLength );
   }
   else if ( p == TheOutputExtension )
   {
      p_outputExtension.Clear();
      if ( sizeOrLength > 0 )
         p_outputExtension.SetLength( sizeOrLength );
   }
   else if ( p == ThePrefix )
   {
      p_prefix.Clear();
      if ( sizeOrLength > 0 )
         p_prefix.SetLength( sizeOrLength );
   }
   else if ( p == ThePostfix )
   {
      p_postfix.Clear();
      if ( sizeOrLength > 0 )
         p_postfix.SetLength( sizeOrLength );
   }
   else if ( p == TheDefects )
   {
      p_defects.Clear();
      if ( sizeOrLength > 0 )
         p_defects.Add( DefectItem(), sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type CosmeticCorrectionInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheTargetFrames )
      return p_targetFrames.Length();
   if ( p == TheTargetFramePath )
      return p_targetFrames[tableRow].path.Length();
   if ( p == TheMasterPath )
      return p_masterDark.Length();
   if ( p == TheOutputDir )
      return p_outputDir.Length();
   if ( p == TheOutputExtension )
      return p_outputExtension.Length();
   if ( p == ThePrefix )
      return p_prefix.Length();
   if ( p == ThePostfix )
      return p_postfix.Length();
   if ( p == TheDefects )
      return p_defects.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionInstance.cpp - Released 2024-12-28T16:54:15Z
