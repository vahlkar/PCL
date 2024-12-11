//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.3.8
// ----------------------------------------------------------------------------
// CometAlignmentInstance.cpp - Released 2024-12-11T17:43:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2023 Nikolay Volkov
// Copyright (c) 2019-2023 Juan Conejero (PTeam)
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L.
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

#include "CentroidDetector.h"
#include "CometAlignmentInstance.h"
#include "CometAlignmentInterface.h"

#include <pcl/DrizzleData.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileFormat.h>
#include <pcl/Graphics.h>
#include <pcl/Homography.h>
#include <pcl/ICCProfile.h>
#include <pcl/LinearFit.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/Version.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CometAlignmentInstance::CometAlignmentInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_referenceIndex( TheCAReferenceIndexParameter->DefaultValue() )
   , p_fitPSF( TheCAFitPSFParameter->DefaultValue() )
   , p_operandSubtractAligned( TheCAOperandSubtractAlignedParameter->DefaultValue() )
   , p_operandLinearFit( TheCAOperandLinearFitParameter->DefaultValue() )
   , p_operandLinearFitLow( TheCAOperandLinearFitLowParameter->DefaultValue() )
   , p_operandLinearFitHigh( TheCAOperandLinearFitHighParameter->DefaultValue() )
   , p_operandNormalize( TheCAOperandNormalizeParameter->DefaultValue() )
   , p_drizzleWriteStarAlignedImage( TheCADrizzleWriteStarAlignedImageParameter->DefaultValue() )
   , p_drizzleWriteCometAlignedImage( TheCADrizzleWriteCometAlignedImageParameter->DefaultValue() )
   , p_pixelInterpolation( CAPixelInterpolation::Default )
   , p_linearClampingThreshold( TheCALinearClampingThresholdParameter->DefaultValue() )
   , p_inputHints( TheCAInputHintsParameter->DefaultValue() )
   , p_outputHints( TheCAOutputHintsParameter->DefaultValue() )
   , p_outputDirectory( TheCAOutputDirectoryParameter->DefaultValue() )
   , p_outputExtension( TheCAOutputExtensionParameter->DefaultValue() ) // ### DEPRECATED
   , p_outputPrefix( TheCAOutputPrefixParameter->DefaultValue() )
   , p_outputPostfix( TheCAOutputPostfixParameter->DefaultValue() )
   , p_overwriteExistingFiles( TheCAOverwriteExistingFilesParameter->DefaultValue() )
   , p_generateCometPathMap( TheCAGenerateCometPathMapParameter->DefaultValue() )
   , p_onError( CAOnError::Default )
   , p_useFileThreads( TheCAUseFileThreadsParameter->DefaultValue() )
   , p_fileThreadOverload( TheCAFileThreadOverloadParameter->DefaultValue() )
   , p_maxFileReadThreads( TheCAMaxFileReadThreadsParameter->DefaultValue() )
   , p_maxFileWriteThreads( TheCAMaxFileWriteThreadsParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

CometAlignmentInstance::CometAlignmentInstance( const CometAlignmentInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void CometAlignmentInstance::Assign( const ProcessImplementation& p )
{
   const CometAlignmentInstance* x = dynamic_cast<const CometAlignmentInstance*>( &p );
   if ( x != nullptr )
   {
      p_targets                        = x->p_targets;
      p_referenceIndex                 = x->p_referenceIndex;
      p_fitPSF                         = x->p_fitPSF;
      p_operandImageFilePath           = x->p_operandImageFilePath;
      p_operandSubtractAligned         = x->p_operandSubtractAligned;
      p_operandLinearFit               = x->p_operandLinearFit;
      p_operandLinearFitLow            = x->p_operandLinearFitLow;
      p_operandLinearFitHigh           = x->p_operandLinearFitHigh;
      p_operandNormalize               = x->p_operandNormalize;
      p_drizzleWriteStarAlignedImage   = x->p_drizzleWriteStarAlignedImage;
      p_drizzleWriteCometAlignedImage  = x->p_drizzleWriteCometAlignedImage;
      p_pixelInterpolation             = x->p_pixelInterpolation;
      p_linearClampingThreshold        = x->p_linearClampingThreshold;
      p_inputHints                     = x->p_inputHints;
      p_outputHints                    = x->p_outputHints;
      p_outputDirectory                = x->p_outputDirectory;
      p_outputExtension                = x->p_outputExtension;
      p_outputPrefix                   = x->p_outputPrefix;
      p_outputPostfix                  = x->p_outputPostfix;
      p_overwriteExistingFiles         = x->p_overwriteExistingFiles;
      p_generateCometPathMap           = x->p_generateCometPathMap;
      p_onError                        = x->p_onError;
      p_useFileThreads                 = x->p_useFileThreads;
      p_fileThreadOverload             = x->p_fileThreadOverload;
      p_maxFileReadThreads             = x->p_maxFileReadThreads;
      p_maxFileWriteThreads            = x->p_maxFileWriteThreads;
   }
}

// ----------------------------------------------------------------------------

bool CometAlignmentInstance::CanExecuteOn( const View&, String& whyNot ) const
{
   whyNot = "CometAlignment can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool CometAlignmentInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool CometAlignmentInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_targets.IsEmpty() )
   {
      whyNot = "No target frames have been specified.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct FileData
{
   AutoPointer<FileFormat> format;           // the file format of retrieved data
   const void*             fsData = nullptr; // format-specific data
   ImageOptions            options;          // currently used for resolution only
   PropertyArray           properties;       // image properties
   FITSKeywordArray        keywords;         // FITS keywords
   ICCProfile              profile;          // ICC profile

   FileData() = default;

   FileData( const FileData& ) = delete;

   FileData( FileData&& x )
      : format( std::move( x.format ) )
      , fsData( x.fsData )
      , options( x.options )
      , properties( std::move( x.properties ) )
      , keywords( std::move( x.keywords ) )
      , profile( std::move( x.profile ) )
   {
      x.fsData = nullptr;
   }

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

   ~FileData()
   {
      if ( format )
         if ( fsData != nullptr )
         {
            format->DisposeFormatSpecificData( const_cast<void*>( fsData ) );
            fsData = nullptr;
         }
   }

   FileData& operator =( const FileData& ) = delete;

   FileData& operator =( FileData&& x )
   {
      format = std::move( x.format );
      fsData = x.fsData; x.fsData = nullptr;
      options = x.options;
      properties = std::move( x.properties );
      keywords = std::move( x.keywords );
      profile = std::move( x.profile );
      return *this;
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CometAlignmentThread : public Thread
{
public:

   CometAlignmentThread( const CometAlignmentInstance& instance,
                         size_type index,
                         const Image& operandImage )
      : m_instance( instance )
      , m_index( index )
      , m_targetFilePath( m_instance.p_targets[m_index].path )
      , m_operandImage( operandImage )
   {
   }

   enum OutputStage
   {
      OutputTarget,
      OutputStarAligned,
      OutputCometAligned
   };

   void Run() override
   {
      try
      {
         Module->ProcessEvents();

         Image targetImage = ReadInputData();
         if ( !targetImage )
            return;

         if ( !m_drizzle && !m_operandImage )
         {
            /*
             * Create a comet-aligned image from a star-aligned frame.
             */
            ApplyHomography( targetImage, Matrix::Translation( m_delta ) );
         }
         else if ( m_drizzle && !m_operandImage )
         {
            /*
             * Create a comet-aligned image and drizzle data file from an
             * unaligned source drizzle frame.
             */
            Matrix M = m_drizzleAlignmentMatrix.Translated( m_delta ); // combine star alignment and comet movement matrices
            M /= M[2][2];
            ApplyHomography( targetImage, M );
         }
         else if ( !m_drizzle && m_operandImage )
         {
            /*
             * Create a comet-only or stars-only aligned image from a
             * star-aligned frame.
             */
            Image operandImage( m_operandImage );

            if ( m_instance.p_operandSubtractAligned )
            {
               /*
                * Move operand and subtract to create stars-only aligned image.
                *
                * N.B.: The operand is expected to be an integration of
                * comet-aligned frames.
                */
               ApplyHomography( operandImage, Matrix::Translation( m_delta ).Inverse() ); // invert delta to align operand to comet position

               if ( m_instance.p_operandLinearFit )
                  ApplyLinearFit( operandImage, targetImage );

               if ( m_instance.p_operandNormalize )
                  Normalize( operandImage );

               targetImage -= operandImage; // subtract Operand(CometIntegration) from StarAligned and create PureStarAligned
            }
            else
            {
               /*
                * Subtract operand and move to comet position to create a
                * comet-only aligned image.
                *
                * N.B.: The operand is expected to be an integration of
                * star-aligned frames.
                */
               if ( m_instance.p_operandLinearFit )
                  ApplyLinearFit( operandImage, targetImage );

               if ( m_instance.p_operandNormalize )
                  Normalize( operandImage );

               targetImage -= operandImage;
               ApplyHomography( targetImage, Matrix::Translation( m_delta ) ); // align result to comet position
            }

            targetImage.Truncate();
         }
         else if ( m_drizzle && m_operandImage )
         {
            /*
             * Create a new comet-only or stars-only unaligned frame from an
             * unaligned source drizzle frame.
             *
             * Optionally create a comet-only or stars-only aligned image plus
             * the corresponding drizzle data files.
             */
            Image operandImage( m_operandImage );

            Matrix M( m_drizzleAlignmentMatrix );
            if ( m_instance.p_operandSubtractAligned ) // the operand is a comet-aligned image
            {
               M.Translate( m_delta ); // combine star alignment and comet movement matrices
               M /= M[2][2];
            }

            ApplyHomography( operandImage, M.Inverse() );

            if ( m_instance.p_operandLinearFit )
               ApplyLinearFit( operandImage, targetImage );

            if ( m_instance.p_operandNormalize )
               Normalize( operandImage );

            targetImage -= operandImage;
            targetImage.Truncate();

            if ( m_instance.p_drizzleWriteStarAlignedImage )
            {
               Image starAlignedImage = targetImage;
               ApplyHomography( starAlignedImage, m_drizzleAlignmentMatrix ); // use star alignment matrix
               WriteOutputData( starAlignedImage, OutputStarAligned );
            }

            if ( m_instance.p_drizzleWriteCometAlignedImage )
            {
               Image cometAlignedImage = targetImage;
               Matrix M = m_drizzleAlignmentMatrix.Translated( m_delta ); // integrate star alignment matrix and comet movement matrix
               M /= M[2][2];
               ApplyHomography( cometAlignedImage, M );
               WriteOutputData( cometAlignedImage, OutputCometAligned );
            }
         }

         WriteOutputData( targetImage, OutputTarget );

         m_success = true;
      }
      catch ( ... )
      {
         if ( IsRootThread() )
            throw;

         String text = ConsoleOutputText();
         ClearConsoleOutputText();
         try
         {
            throw;
         }
         ERROR_HANDLER
         m_errorInfo = ConsoleOutputText();
         ClearConsoleOutputText();
         Console().Write( text );
      }
   }

   size_type Index() const
   {
      return m_index;
   }

   String TargetFilePath() const
   {
      return m_targetFilePath;
   }

   const DPoint& Position() const
   {
      return m_position;
   }

   bool Succeeded() const
   {
      return m_success;
   }

   String ErrorInfo() const
   {
      return m_errorInfo;
   }

private:

   const CometAlignmentInstance& m_instance;
         size_type               m_index = 0;      // position of this thread in the instance's target list
         String                  m_targetFilePath; // path to source star-aligned image, or in drizzle mode path to original drizzle source image
   const Image&                  m_operandImage;   // image to be subtracted from the target image
         FileData                m_fileData;       // target image metadata.
         DPoint                  m_position;       // comet position in image coordinates
         DPoint                  m_delta;          // comet movement vector
         bool                    m_drizzle;        // true -> drizzle mode
         Matrix                  m_drizzleAlignmentMatrix;   // drizzle alignment matrix
         String                  m_drizzleCFASourceFilePath; // drizzle source CFA image
         String                  m_drizzleCFASourcePattern;  // drizzle source CFA pattern
         String                  m_errorInfo;
         bool                    m_success = false;

   template <class P>
   void ApplyHomography( GenericImage<P>& image, const Matrix& M )
   {
      int width = image.Width();
      int height = image.Height();
      int n = image.NumberOfChannels();

      GenericImage<P> transformed;
      transformed.AllocateData( width, height, n, image.ColorSpace() ).Black();

      IndirectArray<PixelInterpolation::Interpolator<P>> interpolators;
      for ( int c = 0; c < n; ++c )
         interpolators << m_instance.m_pixelInterpolation->NewInterpolator<P>( image.PixelData( c ), width, height );

      Homography H( M );
      for ( int y = 0; y < height; ++y )
         for ( int x = 0; x < width; ++x )
         {
            DPoint p = H( x, y ); // source coordinates
            if ( p.x >= 0 && p.x < width && p.y >= 0 && p.y < height ) // ignore out-of-bounds locations
               for ( int c = 0; c < n; ++c )
                  transformed.Pixel( x, y, c ) = (*interpolators[c])( p );
         }

      interpolators.Destroy();
      image.Assign( transformed );
   }

   void ApplyLinearFit( Image& image, const Image& reference )
   {
      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         Array<float> F1, F2;
         F1.Reserve( image.NumberOfPixels() );
         F2.Reserve( image.NumberOfPixels() );
         for ( Image::const_sample_iterator i1( image, c ), i2( reference, c ); i1 && i2; ++i1, ++i2 )
            if ( *i1 > m_instance.p_operandLinearFitLow && *i1 < m_instance.p_operandLinearFitHigh )
               if ( *i2 > m_instance.p_operandLinearFitLow && *i2 < m_instance.p_operandLinearFitHigh )
               {
                  F1 << *i1;
                  F2 << *i2;
               }

         if ( F1.Length() < 3 )
            throw Error( "Linear fit: Insufficient data (channel " + String( c ) + ')' );

         LinearFit L( F1, F2 );
         if ( !L.IsValid() )
            throw Error( "Linear fit: Invalid linear function (channel " + String( c ) + ')' );

         for ( Image::sample_iterator i( image, c ); i; ++i )
            if ( *i > 0 ) // ignore black pixels
               *i = L( *i );
      }
   }

   template <class P>
   void Normalize( GenericImage<P>& image )
   {
      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         typename P::sample m = image.Median( 0, c, c );
         for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
            if ( *i > 0 ) // ignore black pixels
               *i -= m;
      }
   }

   Image ReadInputData()
   {
      static Mutex mutex;
      static AtomicInt count;

      Console console;

      const CometAlignmentInstance::ImageItem& item = m_instance.p_targets[m_index];
      const CometAlignmentInstance::ImageItem& referenceItem = m_instance.p_targets[m_instance.p_referenceIndex];

      m_drizzle = !item.drzPath.IsEmpty();
      if ( m_drizzle )
      {
         console.WriteLn( "<end><cbr>Decoding drizzle data file: <raw>" + item.drzPath + "</raw>" );

         DrizzleData decoder;
         decoder.Parse( item.drzPath, DrizzleParserOption::IgnoreIntegrationData );
         if ( !decoder.HasAlignmentMatrix() )
            throw Error( "The drizzle file does not define an alignment matrix: " + item.drzPath );
         m_targetFilePath = decoder.SourceFilePath();
         m_drizzleAlignmentMatrix = decoder.AlignmentMatrix();
         m_drizzleCFASourceFilePath = decoder.CFASourceFilePath();
         m_drizzleCFASourcePattern = decoder.CFASourcePattern();
      }
      else
         m_targetFilePath = item.path;

      console.WriteLn( "<end><cbr>Loading target file: <raw>" + m_targetFilePath + "</raw>" );

      FileFormat format( File::ExtractExtension( m_targetFilePath ), true/*read*/, false/*write*/ );
      FileFormatInstance file( format );

      ImageDescriptionArray images;
      if ( !file.Open( images, m_targetFilePath, m_instance.p_inputHints ) )
         throw CaughtException();

      if ( images.IsEmpty() )
      {
         console.NoteLn( "* Skipping empty image file." );
         if ( !file.Close() )
            throw CaughtException();
         return Image();
      }

      if ( images.Length() > 1 )
         console.NoteLn( String().Format( "* Ignoring %u additional image(s) in target file.", images.Length()-1 ) );

      Image image( (void*)0, 0, 0 ); // shared image
      {
         volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileReadThreads );
         if ( !file.ReadImage( image ) )
            throw CaughtException();
         m_fileData = FileData( file, images[0].options );
         if ( !file.Close() )
            throw CaughtException();

         if ( m_instance.m_geometryAvailable.Load() == 0 )
         {
            static Mutex mutex;
            volatile AutoLock lock( mutex );
            if ( m_instance.m_geometryAvailable.Load() == 0 )
            {
               m_instance.m_geometry = image.Bounds();
               m_instance.m_geometryAvailable.Store( 1 );
            }
         }
         if ( image.Bounds() != m_instance.m_geometry )
            throw Error( "Incompatible image geometry: " + m_targetFilePath );
      }

      m_position = item.position;

      if ( m_instance.p_fitPSF )
      {
         PSFData psf;
         String status;

         Image I;
         ImageVariant V;
         if ( image.IsColor() )
         {
            image.GetIntensity( I );
            if ( m_drizzle )
               ApplyHomography( I, m_drizzleAlignmentMatrix );
            V = ImageVariant( &I );
         }
         else
         {
            if ( m_drizzle )
            {
               I = image;
               ApplyHomography( I, m_drizzleAlignmentMatrix );
               V = ImageVariant( &I );
            }
            else
               V = ImageVariant( &image );
         }

         CentroidDetector D( V, 0, m_position );
         if ( D )
            psf = D.psf;
         else
            status = D.StatusText();

         if ( psf )
         {
            m_position = psf.c0;
            console.WriteLn( "<end><cbr>PSF: "
                     + psf.FunctionName()
                     + String().Format( ", B = %.6f, A = %.6f, FWHMx = %.2f, FWHMy = %.2f,<br>     r = %.3f, theta = %.2f, beta = %.2f",
                                       psf.B, psf.A, psf.FWHMx(), psf.FWHMy(), psf.sy/psf.sx, psf.theta, psf.beta ) );
            console.WriteLn( String().Format( "<end><cbr>ex = %+.2f, ey = %+.2f",
                                              m_position.x - item.position.x, m_position.y - item.position.y ) );
         }
         else
            console.WarningLn( "<end><cbr>** Warning: PSF fitting error: " + status + ". Using interpolated centroid coordinates." );
      }

      m_delta = m_position - referenceItem.position;

      console.WriteLn( String().Format( "<end><cbr>cx = %.2f, cy = %.2f", m_position.x, m_position.y ) );
      console.WriteLn( String().Format( "<end><cbr>dx = %+.2f, dy = %+.2f", m_delta.x, m_delta.y ) );

      return image;
   }

   void WriteOutputData( const Image& image, OutputStage stage )
   {
      static Mutex mutex;
      static AtomicInt count;

      Console console;

      String postfix( m_instance.p_outputPostfix );
      if ( m_drizzle )
         if ( m_operandImage )
            switch ( stage )
            {
            case OutputTarget: // write unregistered target image (= drizzle source)
               postfix.Clear();
               break;
            case OutputStarAligned: // write stars-only registered image
               postfix = "_r";
               break;
            case OutputCometAligned: // write comet-only registered image
               postfix = "_r" + m_instance.p_outputPostfix;
               break;
            }

      String fileDir = m_instance.p_outputDirectory.Trimmed();
      if ( fileDir.IsEmpty() )
         fileDir = File::ExtractDrive( m_targetFilePath ) + File::ExtractDirectory( m_targetFilePath );
      if ( fileDir.IsEmpty() )
         throw Error( "Unable to determine an output directory: " + m_targetFilePath );
      if ( !fileDir.EndsWith( '/' ) )
         fileDir.Append( '/' );

      String fileName = File::ExtractName( m_targetFilePath ).Trimmed();
      if ( fileName.IsEmpty() )
         throw Error( "Unable to determine an output file name: " + m_targetFilePath );
      if ( !m_instance.p_outputPrefix.IsEmpty() )
         fileName.Prepend( m_instance.p_outputPrefix );
      if ( !postfix.IsEmpty() )
         fileName.Append( postfix );

      const String outputFileExtension = ".xisf";
      String outputFilePath = fileDir + fileName + outputFileExtension;

      console.WriteLn( "<end><cbr>Writing output file: <raw>" + outputFilePath + "</raw>" );

      UniqueFileChecks checks = File::EnsureNewUniqueFile( outputFilePath, m_instance.p_overwriteExistingFiles );
      if ( checks.overwrite )
         console.WarningLn( "** Warning: Overwriting existing file." );
      else if ( checks.exists )
         console.NoteLn( "* File already exists, writing to: <raw>" + outputFilePath + "</raw>" );

      FileFormat outputFormat( outputFileExtension, false/*read*/, true/*write*/ );

      FileFormatInstance outputFile( outputFormat );
      if ( !outputFile.Create( outputFilePath, m_instance.p_outputHints ) )
         throw CaughtException();

      ImageOptions options = m_fileData.options; // get resolution, etc.
      options.bitsPerSample = 32;
      options.ieeefpSampleFormat = true;
      outputFile.SetOptions( options );

      if ( !outputFormat.CanStoreFloat() )
         console.WarningLn( "** Warning: The output format does not support the 32-bit floating point pixel sample format." );

      if ( m_fileData.fsData != nullptr )
         if ( outputFormat.UsesFormatSpecificData() )
            if ( outputFormat.ValidateFormatSpecificData( m_fileData.fsData ) )
               outputFile.SetFormatSpecificData( m_fileData.fsData );

      if ( !m_fileData.properties.IsEmpty() )
         if ( outputFormat.CanStoreImageProperties() )
            outputFile.WriteImageProperties( m_fileData.properties );
         else
            console.WarningLn( "** Warning: The output format cannot store image properties - existing properties not embedded" );

      if ( outputFormat.CanStoreKeywords() )
      {
         FITSKeywordArray keywords = m_fileData.keywords;
         keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Registration with " + PixInsightVersion::AsString() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Registration with " + Module->ReadableVersion() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Registration with CometAlignment process" )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "CometAlignment.delta.x: %+.2f", m_delta.x ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "CometAlignment.delta.y: %+.2f", m_delta.y ) );

         outputFile.WriteFITSKeywords( keywords );
      }
      else if ( !m_fileData.keywords.IsEmpty() )
         console.WarningLn( "** Warning: The output format cannot store FITS keywords - original keywords not embedded" );

      if ( m_fileData.profile.IsProfile() )
         if ( outputFormat.CanStoreICCProfiles() )
            outputFile.WriteICCProfile( m_fileData.profile );
         else
            console.WarningLn( "** Warning: The output format cannot store ICC profiles - existing profile not embedded" );

      Module->ProcessEvents();

      {
         volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileWriteThreads );
         if ( !outputFile.WriteImage( image ) || !outputFile.Close() )
            throw CaughtException();
      }

      if ( m_drizzle )
         if ( m_operandImage && stage == OutputTarget ) // new not registred drizzle integrable image writen
            m_targetFilePath = outputFilePath;
         else
         {
            String outputDrizzleFilePath( File::ChangeExtension( outputFilePath, ".xdrz" ) );
            console.WriteLn( "<end><cbr>Writing output file: <raw>" + outputDrizzleFilePath + "</raw>" );

            UniqueFileChecks checks = File::EnsureNewUniqueFile( outputDrizzleFilePath, m_instance.p_overwriteExistingFiles );
            if ( checks.overwrite )
               console.WarningLn( "** Warning: Overwriting existing file." );
            else if ( checks.exists )
               console.NoteLn( "* File already exists, writing to: <raw>" + outputDrizzleFilePath + "</raw>" );

            Matrix H = m_drizzleAlignmentMatrix;
            if ( !m_operandImage || stage == OutputCometAligned )
            {
               H.Translate( m_delta ); // add comet movement delta
               H /= H[2][2];
            }

            DrizzleData drz;
            drz.SetSourceFilePath( m_targetFilePath );
            drz.SetAlignmentTargetFilePath( outputFilePath );
            drz.SetReferenceDimensions( image.Width(), image.Height() );
            drz.SetAlignmentMatrix( H );

            if ( !m_drizzleCFASourceFilePath.IsEmpty() )
            {
               drz.SetCFASourceFilePath( m_drizzleCFASourceFilePath );
               drz.SetCFASourcePattern( m_drizzleCFASourcePattern );
            }

            {
               volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileWriteThreads );
               drz.SerializeToFile( outputDrizzleFilePath );
            }
         }
   }
};

using CAThreadList = IndirectArray<CometAlignmentThread>;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PixelInterpolation* CometAlignmentInstance::NewPixelInterpolation()
{
   switch ( p_pixelInterpolation )
   {
   case CAPixelInterpolation::NearestNeighbor:
      return new NearestNeighborPixelInterpolation;
   case CAPixelInterpolation::Bilinear:
      return new BilinearPixelInterpolation;
   default:
   case CAPixelInterpolation::BicubicSpline:
      return new BicubicSplinePixelInterpolation( p_linearClampingThreshold );
   case CAPixelInterpolation::BicubicBSpline:
      return new BicubicBSplinePixelInterpolation;
   case CAPixelInterpolation::Lanczos3:
   case CAPixelInterpolation::Lanczos4:
   case CAPixelInterpolation::Lanczos5:
   {
      // ### FIXME: Use LUT Lanczos interpolations for 8-bit and 16-bit images

      // Disable clamping when clampingThreshold == 1
      float clamp = ( p_linearClampingThreshold < 1 ) ? p_linearClampingThreshold : -1;
      switch ( p_pixelInterpolation )
      {
      default:
      case CAPixelInterpolation::Lanczos3:
         return new LanczosPixelInterpolation( 3, clamp );
      case CAPixelInterpolation::Lanczos4:
         return new LanczosPixelInterpolation( 4, clamp );
      case CAPixelInterpolation::Lanczos5:
         return new LanczosPixelInterpolation( 5, clamp );
      }
   }
   break;
   case CAPixelInterpolation::MitchellNetravaliFilter:
      return new BicubicFilterPixelInterpolation( 2, 2, MitchellNetravaliCubicFilter() );
      break;
   case CAPixelInterpolation::CatmullRomSplineFilter:
      return new BicubicFilterPixelInterpolation( 2, 2, CatmullRomSplineFilter() );
      break;
   case CAPixelInterpolation::CubicBSplineFilter:
      return new BicubicFilterPixelInterpolation( 2, 2, CubicBSplineFilter() );
      break;
   }
}

// ----------------------------------------------------------------------------

static Bitmap CometPathMap( int width, int height, const Array<DPoint>& P )
{
   Bitmap bmp( width, height );
   bmp.Fill( 0 ); // transparent

   VectorGraphics G( bmp );
   G.EnableAntialiasing();
   G.SetPen( 0xff000000 ); // solid black

   for ( size_type i = 0; i < P.Length(); ++i )
   {
      const DPoint& p = P[i];
      if ( p != 0.0 )
      {
         DPoint q( p.x + 0.5, p.y + 0.5 );
         G.DrawLine( q.x, q.y-7, q.x, q.y-1 );
         G.DrawLine( q.x, q.y+1, q.x, q.y+7 );
         G.DrawLine( q.x-7, q.y, q.x-1, q.y );
         G.DrawLine( q.x+1, q.y, q.x+7, q.y );

         if ( i > 0 )
            for ( size_type j = i; --j >= 0; )
            {
               const DPoint& p0 = P[j];
               if ( p0 != 0.0 )
               {
                  DPoint q0( p0.x + 0.5, p0.y + 0.5 );
                  G.DrawLine( q0.x, q0.y, q.x, q.y );
                  break;
               }
            }
      }
   }

   G.EndPaint();

   return bmp;
}

// ----------------------------------------------------------------------------

bool CometAlignmentInstance::ExecuteGlobal()
{
   Console console;

   {
      String why;
      if ( !CanExecuteGlobal( why ) )
         throw Error( why );

      if ( !p_outputDirectory.IsEmpty() )
         if ( !File::DirectoryExists( p_outputDirectory ) )
            throw Error( "The specified output directory does not exist: " + p_outputDirectory );

      if ( p_referenceIndex >= p_targets.Length() )
         throw Error( "Reference index out of range (" + String( p_referenceIndex ) + ", expected < " + String( p_targets.Length() ) + ")" );

      StringList fileNames;
      for ( const auto& target : p_targets )
         if ( target.enabled )
         {
            if ( !File::Exists( target.path ) )
               throw Error( "No such file exists on the local filesystem: " + target.path );

            if ( !target.drzPath.IsEmpty() )
               if ( !File::Exists( target.drzPath ) )
                  throw Error( "No such file exists on the local filesystem: " + target.drzPath );

            if ( target.position.x <= 0 || target.position.y <= 0 )
               throw Error( "Invalid object coordinates: " + target.path );

            if ( target.jd <= 0 )
               throw Error( "Invalid frame time: " + target.path );

            fileNames << File::ExtractNameAndSuffix( target.path );
         }
      fileNames.Sort();
      for ( size_type i = 1; i < fileNames.Length(); ++i )
         if ( fileNames[i].CompareIC( fileNames[i-1] ) == 0 )
         {
            if ( p_overwriteExistingFiles )
               throw Error( "The target images list contains duplicate file names (case-insensitive). "
                            "This is not allowed when the 'Overwrite existing files' option is enabled." );
            console.WarningLn( "<end><cbr><br>** Warning: The target images list contains duplicate file names (case-insensitive)." );
            break;
         }
   }

   m_maxFileReadThreads = p_maxFileReadThreads;
   if ( m_maxFileReadThreads < 1 )
      m_maxFileReadThreads = Max( 1, PixInsightSettings::GlobalInteger( "Process/MaxFileReadThreads" ) );

   m_maxFileWriteThreads = p_maxFileWriteThreads;
   if ( m_maxFileWriteThreads < 1 )
      m_maxFileWriteThreads = Max( 1, PixInsightSettings::GlobalInteger( "Process/MaxFileWriteThreads" ) );

   m_geometry = 0;
   m_geometryAvailable.Store( 0 );

   Image operandImage;

   if ( p_operandImageFilePath.IsEmpty() )
   {
      console.WriteLn( "<end><cbr><br>Working mode: Only align target images." );
   }
   else
   {
      console.WriteLn( "<end><cbr><br>Loading operand image:<br><raw>" + p_operandImageFilePath + "</raw>" );

      FileFormat format( File::ExtractExtension( p_operandImageFilePath ), true/*read*/, false/*write*/ );
      FileFormatInstance file( format );

      ImageDescriptionArray images;
      if ( !file.Open( images, p_operandImageFilePath, p_inputHints ) )
         throw CaughtException();

      if ( images.IsEmpty() )
         throw Error( "Empty image file: " + p_operandImageFilePath );

      if ( !file.ReadImage( operandImage ) )
         throw CaughtException();

      Module->ProcessEvents();

      m_geometry = operandImage.Bounds();
      m_geometryAvailable.Store( 1 );

      if ( p_operandSubtractAligned )
         console.WriteLn( "<end><cbr><br>Working mode: Align operand image, then subtract operand from targets." );
      else
         console.WriteLn( "<end><cbr><br>Working mode: Subtract operand image from targets, then align targets." );

      console.WriteLn( String( "Linear fit " ) + (p_operandLinearFit ? "enabled" : "disabled") );
      if ( p_operandLinearFit )
         console.WriteLn( String().Format( "Linear fit parameters: rejection_low = %.6f, rejection_high = %.6f", p_operandLinearFitLow, p_operandLinearFitHigh ) );
      console.WriteLn( String( "Normalization " ) + (p_operandNormalize ? "enabled" : "disabled") );
   }

   AutoPointer pixelInterpolation( NewPixelInterpolation() );
   m_pixelInterpolation = pixelInterpolation.Ptr();

   console.WriteLn( m_pixelInterpolation->Description() );

   console.WriteLn();
   console.EnableAbort();

   int succeeded = 0;
   int failed = 0;
   int skipped = 0;
   console.WriteLn( String().Format( "Processing %u target frames.", p_targets.Length() ) );

   Array<size_type> pendingItems;
   for ( size_type i = 0; i < p_targets.Length(); ++i )
      if ( p_targets[i].enabled )
         pendingItems << i;
      else
      {
         console.NoteLn( "* Skipping disabled target: " + p_targets[i].path );
         ++skipped;
      }

   Array<DPoint> positions( p_targets.Length(), DPoint( 0 ) );

   if ( p_useFileThreads && pendingItems.Length() > 1 )
   {
      int numberOfThreadsAvailable = RoundInt( Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 ) * p_fileThreadOverload );
      int numberOfThreads = Min( numberOfThreadsAvailable, int( pendingItems.Length() ) );
      CAThreadList runningThreads( numberOfThreads ); // N.B.: all pointers are set to nullptr by IndirectArray's ctor.
      console.NoteLn( String().Format( "<end><br>* Using %d worker threads.", numberOfThreads ) );

      try
      {
         /*
          * Thread execution loop.
          */
         for ( ;; )
         {
            try
            {
               int running = 0;
               for ( CAThreadList::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
               {
                  Module->ProcessEvents();
                  if ( console.AbortRequested() )
                     throw ProcessAborted();

                  if ( *i != nullptr )
                  {
                     if ( !(*i)->Wait( 150 ) )
                     {
                        ++running;
                        continue;
                     }

                     /*
                      * A thread has just finished.
                      */
                     if ( succeeded + failed == 0 )
                        console.WriteLn();
                     (*i)->FlushConsoleOutputText();
                     console.WriteLn();
//                      if ( (*i)->Succeeded() )
//                         (*i)->GetOutputData( o_output[(*i)->Index()] );

                     String errorInfo;
                     if ( !(*i)->Succeeded() )
                        errorInfo = (*i)->ErrorInfo();

                     positions[(*i)->Index()] = (*i)->Position();

                     /*
                      * N.B.: IndirectArray<>::Delete() sets to zero the
                      * pointer pointed to by the iterator, but does not remove
                      * the array element.
                      */
                     runningThreads.Delete( i );

                     if ( !errorInfo.IsEmpty() )
                        throw Error( errorInfo );

                     ++succeeded;
                  }

                  /*
                   * If there are items pending, create a new thread and fire
                   * the next one.
                   */
                  if ( !pendingItems.IsEmpty() )
                  {
                     *i = new CometAlignmentThread( *this, *pendingItems, operandImage );
                     pendingItems.Remove( pendingItems.Begin() );
                     size_type threadIndex = i - runningThreads.Begin();
                     console.NoteLn( String().Format( "<end><cbr>[%03u] ", threadIndex ) + (*i)->TargetFilePath() );
                     (*i)->Start( ThreadPriority::DefaultMax, threadIndex );
                     ++running;
                     if ( pendingItems.IsEmpty() )
                        console.NoteLn( "<br>* Waiting for running tasks to terminate...<br>" );
                     else if ( succeeded + failed > 0 )
                        console.WriteLn();
                  }
               }

               if ( !running )
                  break;
            }
            catch ( ProcessAborted& )
            {
               throw;
            }
            catch ( ... )
            {
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               ++failed;
               try
               {
                  throw;
               }
               ERROR_HANDLER

               ApplyErrorPolicy();
            }
         }
      }
      catch ( ... )
      {
         console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate..." );
         for ( CometAlignmentThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Abort();
         for ( CometAlignmentThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Wait();
         runningThreads.Destroy();
         throw;
      }
   }
   else // !p_useFileThreads || pendingItems.Length() < 2
   {
      for ( size_type itemIndex : pendingItems )
      {
         try
         {
            console.WriteLn( "<end><cbr><br>" );
            CometAlignmentThread thread( *this, itemIndex, operandImage );
            thread.Run();
//             thread.GetOutputData( o_output[itemIndex] );
            positions[itemIndex] = thread.Position();
            ++succeeded;
         }
         catch ( ProcessAborted& )
         {
            throw;
         }
         catch ( ... )
         {
            if ( console.AbortRequested() )
               throw ProcessAborted();

            ++failed;
            try
            {
               throw;
            }
            ERROR_HANDLER

            ApplyErrorPolicy();
         }
      }
   }

   Module->ProcessEvents();

   if ( succeeded == 0 )
   {
      if ( failed == 0 )
         throw Error( "No images were aligned: Empty target frames list? No enabled target frames?" );
      throw Error( "No image could be aligned." );
   }
   else
   {
      if ( p_generateCometPathMap )
      {
         ImageWindow window( m_geometry.Width(), m_geometry.Height(), 1/*numberOfChannels*/,
                             8/*bitsPerSample*/, false/*floatSample*/, false/*color*/, true/*processing*/, "CA_CometPath" );
         window.MainView().Image().White().Blend( CometPathMap( m_geometry.Width(), m_geometry.Height(), positions ) );
         window.Show();
      }
   }

   console.NoteLn( String().Format( "<end><cbr><br>===== CometAlignment: %u succeeded, %u failed, %u skipped =====",
                                    succeeded, failed, skipped ) );
   return true;
}

// ----------------------------------------------------------------------------

void CometAlignmentInstance::ApplyErrorPolicy()
{
   Console console;
   console.ResetStatus();
   console.EnableAbort();

   console.Note( "<end><cbr><br>* Applying error policy: " );

   switch ( p_onError )
   {
   default: // ?
   case CAOnError::Continue:
      console.NoteLn( "Continue on error." );
      break;

   case CAOnError::Abort:
      console.NoteLn( "Abort on error." );
      throw ProcessAborted();

   case CAOnError::AskUser:
      {
         console.NoteLn( "Ask on error..." );

         int r = MessageBox( "<p style=\"white-space:pre;\">"
                             "An error occurred during CometAlignment execution. What do you want to do?</p>",
                             "CometAlignment",
                             StdIcon::Error,
                             StdButton::Ignore, StdButton::Abort ).Execute();

         if ( r == StdButton::Abort )
         {
            console.NoteLn( "* Aborting as per user request." );
            throw ProcessAborted();
         }

         console.NoteLn( "* Error ignored as per user request." );
      }
      break;
   }
}

// ----------------------------------------------------------------------------

void* CometAlignmentInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheCATargetFramePathParameter )
      return p_targets[tableRow].path.Begin();
   if ( p == TheCATargetFrameEnabledParameter )
      return &p_targets[tableRow].enabled;
   if ( p == TheCATargetFrameDateParameter ) // ### DEPRECATED
      return p_targets[tableRow].date.Begin();
   if ( p == TheCATargetFrameJDateParameter )
      return &p_targets[tableRow].jd;
   if ( p == TheCATargetFrameXParameter )
      return &p_targets[tableRow].position.x;
   if ( p == TheCATargetFrameYParameter )
      return &p_targets[tableRow].position.y;
   if ( p == TheCATargetFrameFixedParameter )
      return &p_targets[tableRow].fixed;
   if ( p == TheCATargetFrameDrizzlePathParameter )
      return p_targets[tableRow].drzPath.Begin();

   if ( p == TheCAReferenceIndexParameter )
      return &p_referenceIndex;
   if ( p == TheCAFitPSFParameter )
      return &p_fitPSF;
   if ( p == TheCAOperandImageFilePathParameter )
      return p_operandImageFilePath.Begin();
   if ( p == TheCAOperandSubtractAlignedParameter )
      return &p_operandSubtractAligned;
   if ( p == TheCAOperandLinearFitParameter )
      return &p_operandLinearFit;
   if ( p == TheCAOperandLinearFitLowParameter )
      return &p_operandLinearFitLow;
   if ( p == TheCAOperandLinearFitHighParameter )
      return &p_operandLinearFitHigh;
   if ( p == TheCAOperandNormalizeParameter )
      return &p_operandNormalize;
   if ( p == TheCADrizzleWriteStarAlignedImageParameter )
      return &p_drizzleWriteStarAlignedImage;
   if ( p == TheCADrizzleWriteCometAlignedImageParameter )
      return &p_drizzleWriteCometAlignedImage;
   if ( p == TheCAPixelInterpolationParameter )
      return &p_pixelInterpolation;
   if ( p == TheCALinearClampingThresholdParameter )
      return &p_linearClampingThreshold;

   if ( p == TheCAInputHintsParameter )
      return p_inputHints.Begin();
   if ( p == TheCAOutputHintsParameter )
      return p_outputHints.Begin();

   if ( p == TheCAOutputDirectoryParameter )
      return p_outputDirectory.Begin();
   if ( p == TheCAOutputExtensionParameter )
      return p_outputExtension.Begin();
   if ( p == TheCAOutputPrefixParameter )
      return p_outputPrefix.Begin();
   if ( p == TheCAOutputPostfixParameter )
      return p_outputPostfix.Begin();
   if ( p == TheCAOverwriteExistingFilesParameter )
      return &p_overwriteExistingFiles;
   if ( p == TheCAGenerateCometPathMapParameter )
      return &p_generateCometPathMap;
   if ( p == TheCAOnErrorParameter )
      return &p_onError;

   if ( p == TheCAUseFileThreadsParameter )
      return &p_useFileThreads;
   if ( p == TheCAFileThreadOverloadParameter )
      return &p_fileThreadOverload;
   if ( p == TheCAMaxFileReadThreadsParameter )
      return &p_maxFileReadThreads;
   if ( p == TheCAMaxFileWriteThreadsParameter )
      return &p_maxFileWriteThreads;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool CometAlignmentInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )

{
   if ( p == TheCATargetFramesParameter )
   {
      p_targets.Clear();
      if ( sizeOrLength > 0 )
         p_targets.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheCATargetFramePathParameter )
   {
      p_targets[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_targets[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheCATargetFrameDateParameter ) // ### DEPRECATED
   {
      p_targets[tableRow].date.Clear();
      if ( sizeOrLength > 0 )
         p_targets[tableRow].date.SetLength( sizeOrLength );
   }
   else if ( p == TheCATargetFrameDrizzlePathParameter )
   {
      p_targets[tableRow].drzPath.Clear();
      if ( sizeOrLength > 0 )
         p_targets[tableRow].drzPath.SetLength( sizeOrLength );
   }
   else if ( p == TheCAOperandImageFilePathParameter )
   {
      p_operandImageFilePath.Clear();
      if ( sizeOrLength > 0 )
         p_operandImageFilePath.SetLength( sizeOrLength );
   }
   else if ( p == TheCAInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheCAOutputHintsParameter )
   {
      p_outputHints.Clear();
      if ( sizeOrLength > 0 )
         p_outputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheCAOutputDirectoryParameter )
   {
      p_outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_outputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheCAOutputExtensionParameter )
   {
      p_outputExtension.Clear();
      if ( sizeOrLength > 0 )
         p_outputExtension.SetLength( sizeOrLength );
   }
   else if ( p == TheCAOutputPrefixParameter )
   {
      p_outputPrefix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPrefix.SetLength( sizeOrLength );
   }
   else if ( p == TheCAOutputPostfixParameter )
   {
      p_outputPostfix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPostfix.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type CometAlignmentInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheCATargetFramesParameter )
      return p_targets.Length();
   if ( p == TheCATargetFramePathParameter )
      return p_targets[tableRow].path.Length();
   if ( p == TheCATargetFrameDateParameter ) // ### DEPRECATED
      return p_targets[tableRow].date.Length();
   if ( p == TheCATargetFrameDrizzlePathParameter )
      return p_targets[tableRow].drzPath.Length();
   if ( p == TheCAOperandImageFilePathParameter )
      return p_operandImageFilePath.Length();
   if ( p == TheCAInputHintsParameter )
      return p_inputHints.Length();
   if ( p == TheCAOutputHintsParameter )
      return p_outputHints.Length();
   if ( p == TheCAOutputDirectoryParameter )
      return p_outputDirectory.Length();
   if ( p == TheCAOutputExtensionParameter )
      return p_outputExtension.Length();
   if ( p == TheCAOutputPrefixParameter )
      return p_outputPrefix.Length();
   if ( p == TheCAOutputPostfixParameter )
      return p_outputPostfix.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF CometAlignmentInstance.cpp - Released 2024-12-11T17:43:17Z
