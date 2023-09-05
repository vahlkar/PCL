//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.0
// ----------------------------------------------------------------------------
// FITSInstance.cpp - Released 2023-08-28T15:23:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

#include "FITSInstance.h"
#include "FITSFormat.h"
#include "FITSOptionsDialog.h"
#include "FITSRangeOptionsDialog.h"

#include <pcl/ErrorHandler.h>
#include <pcl/StandardStatus.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FITSReadHints
{
public:

   double                            lowerRange;
   double                            upperRange;
   FITSFormat::out_of_range_fix_mode outOfRangeFixMode;
   bool                              bottomUp;
   bool                              useRowOrderKeywords;
   bool                              signedIntegersArePhysical;
   bool                              fixNonFinite;
   int                               verbosity;

   FITSReadHints( const IsoString& hints )
   {
      FITSFormat::OutOfRangePolicyOptions options = FITSFormat::DefaultOutOfRangePolicyOptions();
      lowerRange = options.lowerRange;
      upperRange = options.upperRange;
      outOfRangeFixMode = options.outOfRangeFixMode;

      FITSImageOptions fitsOptions = FITSFormat::DefaultOptions();
      bottomUp = fitsOptions.bottomUp;
      useRowOrderKeywords = fitsOptions.useRowOrderKeywords;
      signedIntegersArePhysical = fitsOptions.signedIntegersArePhysical;
      fixNonFinite = true;
      verbosity = 1;

      IsoStringList theHints;
      hints.Break( theHints, ' ', true/*trim*/ );
      theHints.Remove( IsoString() );
      for ( IsoStringList::const_iterator i = theHints.Begin(); i < theHints.End(); ++i )
      {
         if ( *i == "lower-range" )
         {
            if ( ++i == theHints.End() )
               break;
            i->TryToDouble( lowerRange );
         }
         else if ( *i == "upper-range" )
         {
            if ( ++i == theHints.End() )
               break;
            i->TryToDouble( upperRange );
         }
         else if ( *i == "rescale" || *i == "rescale-out-of-range" )
            outOfRangeFixMode = FITSFormat::OutOfRangeFix_Rescale;
         else if ( *i == "truncate" || *i == "truncate-out-of-range" )
            outOfRangeFixMode = FITSFormat::OutOfRangeFix_Truncate;
         else if ( *i == "ignore-out-of-range" )
            outOfRangeFixMode = FITSFormat::OutOfRangeFix_Ignore;
         else if ( *i == "bottom-up" )
            bottomUp = true;
         else if ( *i == "up-bottom" || *i == "top-down" )
            bottomUp = false;
         else if ( *i == "use-roworder-keywords" )
            useRowOrderKeywords = true;
         else if ( *i == "ignore-roworder-keywords" )
            useRowOrderKeywords = false;
         else if ( *i == "signed-is-physical" )
            signedIntegersArePhysical = true;
         else if ( *i == "signed-is-logical" )
            signedIntegersArePhysical = false;
         else if ( *i == "fix-non-finite" )
            fixNonFinite = true;
         else if ( *i == "ignore-non-finite" )
            fixNonFinite = false;
         else if ( *i == "verbosity" )
         {
            if ( ++i == theHints.End() )
               break;
            int n;
            if ( i->TryToInt( n ) )
               verbosity = Range( n, 0, 3 );
         }
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FITSInstance::FITSInstance( const FITSFormat* f )
   : FileFormatImplementation( f )
{
}

// ----------------------------------------------------------------------------

FITSInstance::~FITSInstance() noexcept( false )
{
   Close();
}

// ----------------------------------------------------------------------------

template <class S>
static void CheckOpenStream( const S& stream, const String& memberFunc )
{
   if ( !stream || !stream->IsOpen() )
      throw Error( "FITSInstance::" + memberFunc + "(): Illegal request on a closed stream." );
}

// ----------------------------------------------------------------------------

ImageDescriptionArray FITSInstance::Open( const String& filePath, const IsoString& hints )
{
   Close();

   try
   {
      Exception::EnableConsoleOutput();

      m_reader = new FITSReader;
      m_reader->Open( filePath );

      FITSImageOptions options = FITSFormat::DefaultOptions();

      if ( !hints.IsEmpty() )
      {
         m_readHints = new FITSReadHints( hints );
         options.bottomUp = m_readHints->bottomUp;
         options.useRowOrderKeywords = m_readHints->useRowOrderKeywords;
         options.signedIntegersArePhysical = m_readHints->signedIntegersArePhysical;
         options.verbosity = m_readHints->verbosity;
      }

      ImageDescriptionArray a;

      for ( int i = 0; i < int( m_reader->NumberOfImages() ); ++i )
      {
         m_reader->SetIndex( i );
         m_reader->SetFITSOptions( options );

         // Some alien FITS files use invalid image identifiers - fix them here
         IsoString id = m_reader->Id();
         if ( !id.IsEmpty() )
         {
            for ( IsoString::iterator i = id.Begin(); i != id.End(); ++i )
               if ( (*i < 'A' || *i > 'Z') && (*i < 'a' || *i > 'z') && (*i < '0' || *i > '9') && *i != '_' )
                  *i = '_';
            if ( *id >= '0' && *id <= '9' )
               id.Prepend( '_' );
         }

         a.Add( ImageDescription( m_reader->Info(), m_reader->Options(), id ) );
      }

      m_reader->SetIndex( 0 );

      return a;
   }
   catch ( ... )
   {
      Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FITSInstance::IsOpen() const
{
   return m_reader != nullptr && m_reader->IsOpen() || m_writer != nullptr && m_writer->IsOpen();
}

// ----------------------------------------------------------------------------

String FITSInstance::FilePath() const
{
   if ( m_reader )
      return m_reader->Path();
   if ( m_writer )
      return m_writer->Path();
   return String();
}

// ----------------------------------------------------------------------------

void FITSInstance::Close()
{
   m_reader.Destroy();
   m_writer.Destroy();
   m_readHints.Destroy();
   m_queriedOptions = m_shownExtractedKeywordsInfo = false;
}

// ----------------------------------------------------------------------------

void FITSInstance::SelectImage( int index )
{
   CheckOpenStream( m_reader, "SelectImage" );
   if ( index < 0 || index >= int( m_reader->NumberOfImages() ) )
      throw Error( String( "FITSInstance::SelectImage(): " ) + "Attempt to select a nonexistent image." );
   m_reader->SetIndex( index );
}

int FITSInstance::SelectedImageIndex() const
{
   CheckOpenStream( m_reader, "SelectedImageIndex" );
   return m_reader->Index();
}

// ----------------------------------------------------------------------------

void* FITSInstance::FormatSpecificData() const
{
   if ( !IsOpen() )
      return nullptr;

   FITSFormat::FormatOptions* data = new FITSFormat::FormatOptions;
   if ( m_reader != nullptr )
      data->options = m_reader->FITSOptions();
   else if ( m_writer != nullptr )
      data->options = m_writer->FITSOptions();
   return data;
}

// ----------------------------------------------------------------------------

FITSKeywordArray FITSInstance::ReadFITSKeywords()
{
   /*!
    * N.B.: The PixInsight core application has an option to reload the list of
    * keywords actually embedded after writing a new image. Hence the
    * 'peculiar' implementation below.
    */
   FITSKeywordArray keywords;
   bool verbose = false;
   if ( m_reader )
   {
      if ( m_reader->IsOpen() )
      {
         keywords = m_reader->ReadFITSKeywords();
         verbose = m_reader->FITSOptions().verbosity > 0;
      }
      else
         CheckOpenStream( m_reader, "ReadFITSKeywords" );
   }
   if ( m_writer )
   {
      if ( m_writer->IsOpen() )
      {
         keywords = m_writer->FITSKeywords();
         verbose = m_writer->FITSOptions().verbosity > 0;
      }
      else
         CheckOpenStream( m_writer, "ReadFITSKeywords" );
   }
   if ( !keywords.IsEmpty() ) // ?!
      if ( verbose )
         if ( !m_shownExtractedKeywordsInfo ) // prevent repeated console messages, some clients read kwds. multiple times.
         {
            Console().WriteLn( "<end><cbr>" + String( keywords.Length() ) + " FITS keywords extracted" );
            m_shownExtractedKeywordsInfo = true;
         }
   return keywords;
}

// ----------------------------------------------------------------------------

template <class P>
static bool ApplyOutOfRangePolicy( GenericImage<P>& image, const FITSReadHints* readHints )
{
   image.ResetSelections();

   /*
    * Replace NaNs, infinities and negative zeros with positive zeros.
    */
   if ( readHints == nullptr || readHints->fixNonFinite )
      for ( int c = 0; c < image.NumberOfChannels(); ++c )
         for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
            if ( !IsFinite( *i ) || IsNegativeZero( *i ) )
               *i = 0;

   /*
    * Get extreme pixel sample values.
    */
   typename P::sample mn, mx;
   {
      StatusMonitor status = image.Status();
      image.SetStatusCallback( nullptr );
      image.GetExtremePixelValues( mn, mx );
      image.Status() = status;
   }

   /*
    * Apply out-of-range policy.
    */
   FITSFormat::OutOfRangePolicyOptions options = FITSFormat::DefaultOutOfRangePolicyOptions();
   if ( readHints != nullptr )
   {
      options.lowerRange = readHints->lowerRange;
      options.upperRange = readHints->upperRange;
      options.outOfRangeFixMode = readHints->outOfRangeFixMode;
   }
   else // if there are hints, never ask user.
   {
      switch ( options.outOfRangePolicy )
      {
      case FITSFormat::OutOfRangePolicy_SetRange:
         break;
      case FITSFormat::OutOfRangePolicy_AskIfOut:
         if ( mn >= options.lowerRange && mx <= options.upperRange )
            break;
         // else pass-through
      case FITSFormat::OutOfRangePolicy_AskAlways:
         {
            FITSRangeOptionsDialog dlg( options, image.BitsPerSample(), mn, mx );

            for ( ;; )
            {
               if ( dlg.Execute() != StdDialogCode::Ok )
                  return false;

               options = dlg.outOfRange;

               if ( options.upperRange < options.lowerRange )
                  Swap( options.lowerRange, options.upperRange );

               double delta = options.upperRange - options.lowerRange;
               if ( 1 + delta == 1 )
               {
                  try
                  {
                     throw Error( "Empty input range!" );
                  }
                  ERROR_HANDLER
                  continue;
               }

               break;
            }
         }
         break;
      }
   }

   bool verbose = readHints == nullptr || readHints->verbosity > 0;

   if ( options.outOfRangeFixMode != FITSFormat::OutOfRangeFix_Ignore )
   {
      double delta = options.upperRange - options.lowerRange;

      StatusMonitor monitor;
      StandardStatus status;
      if ( verbose )
         monitor.SetCallback( &status );

      if ( mn < options.lowerRange || mx > options.upperRange )
      {
         if ( verbose )
            monitor.Initialize( String().Format( "%s sample values: [%.15g,%.15g] -> [%.15g,%.15g]",
                  (options.outOfRangeFixMode == FITSFormat::OutOfRangeFix_Truncate) ? "Truncating" : "Rescaling",
                  mn, mx,
                  options.lowerRange, options.upperRange ), image.NumberOfSamples() );

         double idelta = mx - mn;
         for ( int c = 0; c < image.NumberOfChannels(); ++c )
            switch ( options.outOfRangeFixMode )
            {
            default: // ?!
            case FITSFormat::OutOfRangeFix_Truncate:
               for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
               {
                  if ( *i < options.lowerRange )
                     *i = options.lowerRange;
                  else if ( *i > options.upperRange )
                     *i = options.upperRange;
                  if ( verbose )
                     ++monitor;
               }
               break;
            case FITSFormat::OutOfRangeFix_Rescale:
               if ( delta != 1 || options.lowerRange != 0 )
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
                  {
                     *i = ((*i - mn)/idelta)*delta + options.lowerRange;
                     if ( verbose )
                        ++monitor;
                  }
               else
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
                  {
                     *i = Range( (*i - mn)/idelta, 0.0, 1.0 );
                     if ( verbose )
                        ++monitor;
                  }
               break;
            }
      }

      if ( options.lowerRange != 0 || options.upperRange != 1 )
      {
         if ( verbose )
            monitor.Initialize( "Normalizing sample values", image.NumberOfSamples() );

         for ( int c = 0; c < image.NumberOfChannels(); ++c )
            if ( delta != 1 )
            {
               if ( options.lowerRange != 0 )
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
                  {
                     *i = Range( (*i - options.lowerRange)/delta, 0.0, 1.0 );
                     if ( verbose )
                        ++monitor;
                  }
               else
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
                  {
                     *i = Range( *i/delta, 0.0, 1.0 );
                     if ( verbose )
                        ++monitor;
                  }
            }
            else
            {
               for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
               {
                  *i = Range( *i - options.lowerRange, 0.0, 1.0 );
                  if ( verbose )
                     ++monitor;
               }
            }
      }
   }
   else
   {
      if ( mn < options.lowerRange || mx > options.upperRange )
         if ( verbose )
            Console().WarningLn( String().Format( "<end><cbr>** Warning: FITS: Out-of-range floating point pixel sample values "
                                                  "were not fixed because of permissive policy. Data range is [%.15g,%.15g]", mn, mx ) );
   }

   return true;
}

// ----------------------------------------------------------------------------

static void ReadFITSImage3( Image& image, FITSReader* reader, const FITSReadHints* readHints )
{
   reader->ReadImage( image );
   if ( reader->Options().ieeefpSampleFormat )
      if ( !ApplyOutOfRangePolicy( image, readHints ) )
         throw ProcessAborted();
}

static void ReadFITSImage3( DImage& image, FITSReader* reader, const FITSReadHints* readHints )
{
   reader->ReadImage( image );
   if ( reader->Options().ieeefpSampleFormat )
      if ( !ApplyOutOfRangePolicy( image, readHints ) )
         throw ProcessAborted();
}

template <class P>
static void ReadFITSImage2( GenericImage<P>& image, FITSReader* reader, const FITSReadHints* readHints )
{
   if ( reader->Options().ieeefpSampleFormat )
   {
      switch ( reader->Options().bitsPerSample )
      {
      case 32:
         {
            Image tmp;
            ReadFITSImage3( tmp, reader, readHints );
            image.Assign( tmp );
         }
         break;
      default: // !?
      case 64:
         {
            DImage tmp;
            ReadFITSImage3( tmp, reader, readHints );
            image.Assign( tmp );
         }
         break;
      }
   }
   else
      reader->ReadImage( image );
}

static void ReadFITSImage2( Image& image, FITSReader* reader, const FITSReadHints* readHints )
{
   ReadFITSImage3( image, reader, readHints );
}

static void ReadFITSImage2( DImage& image, FITSReader* reader, const FITSReadHints* readHints )
{
   ReadFITSImage3( image, reader, readHints );
}

template <class P>
static void ReadFITSImage1( GenericImage<P>& image, FITSReader* reader, const FITSReadHints* readHints )
{
   CheckOpenStream( reader, "ReadImage" );
   // Disable automatic normalization of floating point FITS images
   ImageOptions options = reader->Options();
   options.readNormalized = !options.ieeefpSampleFormat;
   reader->SetOptions( options );
   ReadFITSImage2( image, reader, readHints );
}

// ----------------------------------------------------------------------------

void FITSInstance::ReadImage( Image& image )
{
   ReadFITSImage1( image, m_reader, m_readHints );
}

void FITSInstance::ReadImage( DImage& image )
{
   ReadFITSImage1( image, m_reader, m_readHints );
}

void FITSInstance::ReadImage( UInt8Image& image )
{
   ReadFITSImage1( image, m_reader, m_readHints );
}

void FITSInstance::ReadImage( UInt16Image& image )
{
   ReadFITSImage1( image, m_reader, m_readHints );
}

void FITSInstance::ReadImage( UInt32Image& image )
{
   ReadFITSImage1( image, m_reader, m_readHints );
}

// ----------------------------------------------------------------------------

template <class T>
void ReadFITSSamples( T* buffer, int startRow, int rowCount, int channel, FITSReader* reader )
{
   CheckOpenStream( reader, "ReadSamples" );
   reader->ReadSamples( buffer, startRow, rowCount, channel );
}

void FITSInstance::ReadSamples( pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadFITSSamples( buffer, startRow, rowCount, channel, m_reader );
}

void FITSInstance::ReadSamples( pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadFITSSamples( buffer, startRow, rowCount, channel, m_reader );
}

void FITSInstance::ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadFITSSamples( buffer, startRow, rowCount, channel, m_reader );
}

void FITSInstance::ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadFITSSamples( buffer, startRow, rowCount, channel, m_reader );
}

void FITSInstance::ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadFITSSamples( buffer, startRow, rowCount, channel, m_reader );
}

// ----------------------------------------------------------------------------

bool FITSInstance::QueryOptions( Array<ImageOptions>& imageOptions, Array<void*>& formatOptions )
{
   m_queriedOptions = true;

   // Format-independent options
   ImageOptions options;
   if ( !imageOptions.IsEmpty() )
      options = *imageOptions;

   // Format-specific options
   FITSFormat::FormatOptions* fits = nullptr;
   if ( !formatOptions.IsEmpty() )
   {
      FITSFormat::FormatOptions* o = FITSFormat::FormatOptions::FromGenericDataBlock( *formatOptions );
      if ( o != nullptr )
         fits = o;
   }

   bool reusedFormatOptions = fits != nullptr;
   if ( !reusedFormatOptions )
      fits = new FITSFormat::FormatOptions;

   FITSOptionsDialog dlg( options, fits->options );
   if ( dlg.Execute() == StdDialogCode::Ok )
   {
      fits->options = dlg.fitsOptions;

      if ( imageOptions.IsEmpty() )
         imageOptions << dlg.options;
      else
         *imageOptions = dlg.options;

      if ( formatOptions.IsEmpty() )
         formatOptions << (void*)fits;
      else
         *formatOptions = (void*)fits;

      return true;
   }

   if ( !reusedFormatOptions )
      delete fits;

   return false;
}

// ----------------------------------------------------------------------------

void FITSInstance::Create( const String& filePath, int numberOfImages, const IsoString& hints )
{
   Close();

   Exception::EnableConsoleOutput();

   m_writer = new FITSWriter;
   m_writer->Create( filePath, numberOfImages );

   ImageOptions options = m_writer->Options();
   FITSImageOptions fitsOptions = FITSFormat::DefaultOptions();

   if ( !hints.IsEmpty() )
   {
      IsoStringList theHints;
      hints.Break( theHints, ' ', true/*trim*/ );
      theHints.Remove( IsoString() );
      for ( IsoStringList::const_iterator i = theHints.Begin(); i < theHints.End(); ++i )
      {
         if ( *i == "unsigned" )
            fitsOptions.unsignedIntegers = true;
         else if ( *i == "signed" )
            fitsOptions.unsignedIntegers = false;
         else if ( *i == "bottom-up" )
            fitsOptions.bottomUp = true;
         else if ( *i == "up-bottom" || *i == "top-down" )
            fitsOptions.bottomUp = false;
         else if ( *i == "cleanup-headers" )
            fitsOptions.cleanupHeaders = true;
         else if ( *i == "no-cleanup-headers" )
            fitsOptions.cleanupHeaders = false;
         else if ( *i == "verbosity" )
         {
            if ( ++i == theHints.End() )
               break;
            int n;
            if ( i->TryToInt( n ) )
               fitsOptions.verbosity = Range( n, 0, 3 );
         }
      }
   }

   m_writer->SetOptions( options );
   m_writer->SetFITSOptions( fitsOptions );
}

// ----------------------------------------------------------------------------

void FITSInstance::SetId( const IsoString& id )
{
   CheckOpenStream( m_writer, "SetId" );
   m_writer->SetId( id );
}

// ----------------------------------------------------------------------------

void FITSInstance::SetOptions( const ImageOptions& newOptions )
{
   CheckOpenStream( m_writer, "SetOptions" );
   m_writer->SetOptions( newOptions );
}

// ----------------------------------------------------------------------------

void FITSInstance::SetFormatSpecificData( const void* data )
{
   CheckOpenStream( m_writer, "SetFormatSpecificData" );
   const FITSFormat::FormatOptions* o = FITSFormat::FormatOptions::FromGenericDataBlock( data );
   if ( o != nullptr )
      m_writer->SetFITSOptions( o->options );
}

// ----------------------------------------------------------------------------

void FITSInstance::WriteFITSKeywords( const FITSKeywordArray& keywords )
{
   CheckOpenStream( m_writer, "WriteFITSKeywords" );
   m_writer->WriteFITSKeywords( keywords );
   if ( m_writer->FITSOptions().verbosity > 0 )
      Console().WriteLn( "<end><cbr>" + String( keywords.Length() ) + " FITS keywords embedded" );
}

// ----------------------------------------------------------------------------

template <class P>
static void WriteFITSImage( const GenericImage<P>& image, FITSWriter* writer )
{
   CheckOpenStream( writer, "WriteImage" );
   writer->WriteImage( image );
}

void FITSInstance::WriteImage( const Image& image )
{
   WriteFITSImage( image, m_writer );
}

void FITSInstance::WriteImage( const DImage& image )
{
   WriteFITSImage( image, m_writer );
}

void FITSInstance::WriteImage( const UInt8Image& image )
{
   WriteFITSImage( image, m_writer );
}

void FITSInstance::WriteImage( const UInt16Image& image )
{
   WriteFITSImage( image, m_writer );
}

void FITSInstance::WriteImage( const UInt32Image& image )
{
   WriteFITSImage( image, m_writer );
}

// ----------------------------------------------------------------------------

void FITSInstance::CreateImage( const ImageInfo& info )
{
   CheckOpenStream( m_writer, "CreateImage" );
   m_writer->CreateImage( info );
}

// ----------------------------------------------------------------------------

template <class T>
void WriteFITSSamples( const T* buffer, int startRow, int rowCount, int channel, FITSWriter* writer )
{
   CheckOpenStream( writer, "WriteSamples" );
   writer->WriteSamples( buffer, startRow, rowCount, channel );
}

void FITSInstance::WriteSamples( const pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteFITSSamples( buffer, startRow, rowCount, channel, m_writer );
}

void FITSInstance::WriteSamples( const pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteFITSSamples( buffer, startRow, rowCount, channel, m_writer );
}

void FITSInstance::WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteFITSSamples( buffer, startRow, rowCount, channel, m_writer );
}

void FITSInstance::WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteFITSSamples( buffer, startRow, rowCount, channel, m_writer );
}

void FITSInstance::WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteFITSSamples( buffer, startRow, rowCount, channel, m_writer );
}

// ----------------------------------------------------------------------------

void FITSInstance::CloseImage()
{
   CheckOpenStream( m_writer, "CloseImage" );
   m_writer->CloseImage();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSInstance.cpp - Released 2023-08-28T15:23:33Z
