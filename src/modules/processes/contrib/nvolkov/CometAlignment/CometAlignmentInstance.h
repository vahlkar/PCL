//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.3
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.3.7
// ----------------------------------------------------------------------------
// CometAlignmentInstance.h - Released 2023-05-17T17:06:42Z
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

#ifndef __CometAlignmentInstance_h
#define __CometAlignmentInstance_h

#include <pcl/FileFormatInstance.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/TimePoint.h>

#include "CometAlignmentParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CometAlignmentInstance : public ProcessImplementation
{
public:

   CometAlignmentInstance( const MetaProcess* );
   CometAlignmentInstance( const CometAlignmentInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool IsHistoryUpdater( const View& ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter*, size_type tableRow ) const override;

private:

   struct ImageItem
   {
      String   path;           // absolute file path
      pcl_bool enabled = true; // if disabled, skip (ignore) this image
      String   date;           // ### DEPRECATED
      double   jd = 0;         // observation time, Julian date
      DPoint   position = 0;   // comet coordinates
      pcl_bool fixed = false;  // true if position is user-defined, for dynamic GUI interaction
      String   drzPath;        // drizzle data file

      ImageItem( const String& p, const String& d )
         : path( p )
         , jd( TimePoint( d ).JD() )
      {
      }

      ImageItem() = default;
      ImageItem( const ImageItem& ) = default;

      bool operator <( const ImageItem& item ) const
      {
         if ( jd != item.jd )
            return jd < item.jd;
         return path < item.path;
      }

      bool operator ==( const ImageItem& item ) const
      {
         return jd == item.jd && path == item.path;
      }
   };

   typedef Array<ImageItem> image_list;

   // Working data and parameters
   image_list p_targets;
   uint32     p_referenceIndex;
   pcl_bool   p_fitPSF;
   String     p_operandImageFilePath;
   pcl_bool   p_operandSubtractAligned; // true -> align operand and subtract from target, false -> subtract operand from target and align
   pcl_bool   p_operandLinearFit;
   float      p_operandLinearFitLow;
   float      p_operandLinearFitHigh;
   pcl_bool   p_operandNormalize;
   pcl_bool   p_drizzleWriteStarAlignedImage;
   pcl_bool   p_drizzleWriteCometAlignedImage;
   pcl_enum   p_pixelInterpolation;
   float      p_linearClampingThreshold;

   // Format hints
   String     p_inputHints;
   String     p_outputHints;

   // Output images
   String     p_outputDirectory;
   String     p_outputExtension;
   String     p_outputPrefix;
   String     p_outputPostfix;
   pcl_bool   p_overwriteExistingFiles;
   pcl_bool   p_generateCometPathMap;
   pcl_enum   p_onError;

   // High-level parallelism
   pcl_bool   p_useFileThreads;
   float      p_fileThreadOverload;
   int32      p_maxFileReadThreads;
   int32      p_maxFileWriteThreads;

   mutable Rect        m_geometry = 0;
   mutable AtomicInt   m_geometryAvailable;
   PixelInterpolation* m_pixelInterpolation = nullptr;
   int                 m_maxFileReadThreads = 1;
   int                 m_maxFileWriteThreads = 1;

   PixelInterpolation* NewPixelInterpolation();

   void ApplyErrorPolicy();

   friend class CometAlignmentThread;
   friend class CometAlignmentInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif // __CometAlignmentInstance_h

// ----------------------------------------------------------------------------
// EOF CometAlignmentInstance.h - Released 2023-05-17T17:06:42Z
