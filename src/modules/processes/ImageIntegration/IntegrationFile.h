//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// IntegrationFile.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#ifndef __IntegrationFile_h
#define __IntegrationFile_h

#include <pcl/AutoPointer.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/Image.h>
#include <pcl/IntegrationMetadata.h>
#include <pcl/LocalNormalizationData.h>
#include <pcl/Thread.h>

#include "AdaptiveNormalizationData.h"
#include "ImageIntegrationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS FileFormatInstance;
class PCL_CLASS DrizzleData;

/*
 * Metadata, descriptive statistics and pixel data of an image being
 * integrated.
 */
class IntegrationFile
{
public:

   typedef IndirectArray<IntegrationFile>             file_list;

   typedef ImageIntegrationInstance::scale_estimates  scale_estimates;

   static void OpenFiles( const ImageIntegrationInstance& );

   String Path() const
   {
      return m_file.IsValid() ? m_file->FilePath() : String();
   }

   String LocalNormalizationDataPath() const
   {
      return m_nmlPath;
   }

   String DrizzleDataPath() const
   {
      return m_drzPath;
   }

   const float* operator []( int row ) const
   {
      if ( s_incremental )
         return m_buffer[row] + s_roi.x0;
      return m_image->ScanLine( row, m_currentChannel );
   }

   bool HasLocationEstimates() const
   {
      return !m_locationEstimates.IsEmpty();
   }

   double LocationEstimate( int c ) const
   {
      return m_locationEstimates[c];
   }

   bool HasScaleEstimates() const
   {
      return !m_scaleEstimates.IsEmpty();
   }

   const TwoSidedEstimate& ScaleEstimates( int c ) const
   {
      return m_scaleEstimates[c];
   }

   bool HasNoiseEstimates() const
   {
      return !m_noiseEstimates.IsEmpty();
   }

   double NoiseEstimate( int c ) const
   {
      return m_noiseEstimates[c];
   }

   bool HasScaleFactors() const
   {
      return HasScaleEstimates();
   }

   TwoSidedEstimate ScaleFactor( int c ) const
   {
      return s_files[0]->m_scaleEstimates[c] / m_scaleEstimates[c];
   }

   DVector ScalarScaleFactors() const
   {
      DVector s( s_numberOfChannels );
      for ( int c = 0; c < s_numberOfChannels; ++c )
         s[c] = double( s_files[0]->m_scaleEstimates[c] / m_scaleEstimates[c] );
      return s;
   }

   bool HasZeroOffsets() const
   {
      return HasLocationEstimates();
   }

   double ZeroOffset( int c ) const
   {
      return s_files[0]->m_locationEstimates[c] - m_locationEstimates[c];
   }

   bool HasWeights() const
   {
      return !m_weights.IsEmpty();
   }

   double Weight( int c ) const
   {
      return m_weights[c];
   }

   bool HasImageWeights() const
   {
      return HasWeights();
   }

   double ImageWeight( int c ) const
   {
      return m_weights[c] / s_files[0]->m_weights[c];
   }

   double Mean( int c ) const
   {
      return m_mean[c];
   }

   double Median( int c ) const
   {
      return m_median[c];
   }

   const TwoSidedEstimate& AvgDev( int c ) const
   {
      return m_avgDev[c];
   }

   const TwoSidedEstimate& MAD( int c ) const
   {
      return m_mad[c];
   }

   const TwoSidedEstimate& BWMV( int c ) const
   {
      return m_bwmv[c];
   }

   double Normalize( double z, int x, int y, int c ) const
   {
      return m_hasLocalNormalization ? m_localNormalization( z, x, y, c ) : z;
   }

   const AdaptiveNormalizationData& AdaptiveNormalization() const
   {
      return m_adaptiveNormalization;
   }

   const IntegrationMetadata& Metadata() const
   {
      return m_metadata;
   }

   static IntegrationMetadata SummaryMetadata();

   const UInt8Image& RejectionMap() const
   {
      return m_rejectionMap;
   }

   UInt8Image& RejectionMap()
   {
      return m_rejectionMap;
   }

   void UpdateRejectionMap( int x, int y, int channel, uint8 mask )
   {
      if ( m_rejectionMap.IsEmpty() )
         InitializeRejectionMap();
      m_rejectionMap( x, y, channel ) |= mask;
   }

   void ToDrizzleData( DrizzleData& ) const;

   static bool Incremental()
   {
      return s_incremental;
   }

   static int NumberOfFiles()
   {
      return int( s_files.Length() );
   }

   static IntegrationFile& FileByIndex( int i )
   {
      return *s_files[i];
   }

   static int Width()
   {
      return s_roi.Width();
   }

   static int Height()
   {
      return s_roi.Height();
   }

   static int NumberOfChannels()
   {
      return s_numberOfChannels;
   }

   static size_type NumberOfPixels()
   {
      return size_type( s_roi.Width() ) * size_type( s_roi.Height() );
   }

   static bool IsColor()
   {
      return s_isColor;
   }

   static int BufferRows()
   {
      return s_bufferRows;
   }

   static size_type AvailableMemory()
   {
      return s_availableMemory;
   }

   static size_type TotalBufferSize()
   {
      return s_files.Length()
           * size_type( s_bufferRows )
           * size_type( s_width )
           * sizeof( float );
   }

   static void UpdateBuffers( int startRow, int channel, int maxThreads );

   static void CloseAll()
   {
      s_files.Destroy();
   }

private:

   AutoPointer<FileFormatInstance> m_file;
   String                          m_nmlPath;
   String                          m_drzPath;
   AutoPointer<Image>              m_image;  // non-incremental file reading
   int                             m_currentChannel;
   FMatrix                         m_buffer; // incremental file reading
   UInt8Image                      m_rejectionMap;
   DVector                         m_mean;
   DVector                         m_median;
   scale_estimates                 m_avgDev;
   scale_estimates                 m_mad;
   scale_estimates                 m_bwmv;
   DVector                         m_locationEstimates;
   scale_estimates                 m_scaleEstimates;
   DVector                         m_noiseEstimates;
   DVector                         m_weights;
   LocalNormalizationData          m_localNormalization;
   bool                            m_hasLocalNormalization = false;
   AdaptiveNormalizationData       m_adaptiveNormalization;
   IntegrationMetadata             m_metadata;
   const ImageIntegrationInstance* m_instance = nullptr;

   static file_list                s_files;
   static Rect                     s_roi;
   static int                      s_width;
   static int                      s_height;
   static int                      s_numberOfChannels;
   static bool                     s_isColor;
   static bool                     s_incremental;
   static size_type                s_availableMemory;
   static int                      s_bufferRows;

   struct ThreadIndex
   {
      size_type itemIndex, fileIndex;

      ThreadIndex( size_type i, size_type f )
         : itemIndex( i )
         , fileIndex( f )
      {
      }
   };

   class OpenFileThread : public Thread
   {
   public:

      OpenFileThread( const ThreadIndex& index, const ImageIntegrationInstance& instance, bool isReference = false )
         : m_file( s_files[index.fileIndex] )
         , m_item( instance.p_images[index.itemIndex] )
         , m_instance( instance )
         , m_isReference( isReference )
      {
      }

      void Run() override;

      String FilePath() const
      {
         return m_item.path;
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

            IntegrationFile*                    m_file = nullptr;
      const ImageIntegrationInstance::ImageItem m_item;
      const ImageIntegrationInstance&           m_instance;
            bool                                m_isReference = false;
            bool                                m_success = false;
            String                              m_errorInfo;
   };

   class UpdateBuffersThread : public Thread
   {
   public:

      UpdateBuffersThread( int firstFile, int endFile, int startRow, int channel )
         : m_firstFile( firstFile )
         , m_endFile( endFile )
         , m_startRow( startRow )
         , m_channel( channel )
      {
      }

      void Run() override
      {
         for ( int i = m_firstFile; i < m_endFile; ++i )
            s_files[i]->Read( m_startRow, m_channel );
      }

   private:

      int m_firstFile;
      int m_endFile;
      int m_startRow;
      int m_channel;
   };

   typedef IndirectArray<OpenFileThread> file_thread_list;

   IntegrationFile() = default;

   void Open( const String&, const String&, const String&, const ImageIntegrationInstance&, bool isReference );

   void Read( int startRow, int channel );

   double KeywordValue( const IsoString& keyName );

   template <class S>
   double KeywordValue( const S& keyName )
   {
      return KeywordValue( IsoString( keyName ) );
   }

   void InitializeRejectionMap()
   {
      m_rejectionMap.AllocateData( Width(), Height(), s_numberOfChannels ).Zero();
   }

   void ResetCacheableData();
   void AddToCache( const String& path ) const;
   int GetFromCache( const String& path );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __IntegrationFile_h

// ----------------------------------------------------------------------------
// EOF IntegrationFile.h - Released 2021-04-09T19:41:48Z
