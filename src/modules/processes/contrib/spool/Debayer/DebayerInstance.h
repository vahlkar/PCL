//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 1.8.2
// ----------------------------------------------------------------------------
// DebayerInstance.h - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

#ifndef __DebayerInstance_h
#define __DebayerInstance_h

#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS FileFormatInstance;

class DebayerInstance : public ProcessImplementation
{
public:

   DebayerInstance( const MetaProcess* );
   DebayerInstance( const DebayerInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsHistoryUpdater( const View& ) const override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   struct Item
   {
      pcl_bool enabled = true;
      String   path;

      Item() = default;
      Item( const Item& ) = default;

      Item( const String& path_ ) : path( path_ )
      {
      }

      bool IsDefined() const
      {
         return !path.IsEmpty();
      }
   };

   typedef Array<Item>  item_list;

   // Process parameters
   pcl_enum   p_bayerPattern;
   pcl_enum   p_debayerMethod;
   int32      p_fbddNoiseReduction;
   pcl_bool   p_evaluateNoise;
   pcl_enum   p_noiseEvaluationAlgorithm;
   pcl_bool   p_showImages;        // optional for view execution only
   String     p_cfaSourceFilePath; // ...
   item_list  p_targets;
   pcl_bool   p_autoMemoryLimit;
   pcl_bool   p_noGUIMessages; // ### DEPRECATED

   // Format hints
   String      p_inputHints;
   String      p_outputHints;

   // Output images
   String      p_outputDirectory;
   String      p_outputExtension;
   String      p_outputPrefix;
   String      p_outputPostfix;
   pcl_bool    p_overwriteExistingFiles;
   pcl_enum    p_onError;

   // High-level parallelism
   pcl_bool    p_useFileThreads;
   float       p_fileThreadOverload;
   int32       p_maxFileReadThreads;
   int32       p_maxFileWriteThreads;
   int         m_maxFileReadThreads = 1;
   int         m_maxFileWriteThreads = 1;

   // Read-only output properties, view execution.
   String     o_imageId;
   FVector    o_noiseEstimates;
   FVector    o_noiseFractions;
   StringList o_noiseAlgorithms;

   // Read-only output properties, batch execution.
   struct OutputFileData
   {
      String     filePath;
      FVector    noiseEstimates = FVector( 0.0F, 3 );
      FVector    noiseFractions = FVector( 0.0F, 3 );
      StringList noiseAlgorithms = StringList( size_type( 3 ) );
   };
   Array<OutputFileData> o_outputFileData;

   pcl_enum BayerPatternFromTarget( const View& ) const;
   pcl_enum BayerPatternFromTarget( FileFormatInstance& ) const;
   static pcl_enum BayerPatternFromTargetProperty( const Variant& );
   static pcl_enum BayerPatternFromTargetPropertyValue( const IsoString&, int dx = 0, int dy = 0 );

   IsoString CFAPatternIdFromTarget( const View&, bool xtrans ) const;
   IsoString CFAPatternIdFromTarget( FileFormatInstance&, bool xtrans ) const;
   static IsoString CFAPatternIdFromTargetProperty( const Variant&, int dx = 0, int dy = 0 );

   static bool IsXTransCFAFromTarget( const View& );
   static bool IsXTransCFAFromTarget( FileFormatInstance& );
   static bool IsXTransCFAFromTargetProperty( const Variant& );

   static IMatrix XTransPatternFiltersFromTarget( const View& );
   static IMatrix XTransPatternFiltersFromTarget( FileFormatInstance& );
   static IMatrix XTransPatternFiltersFromTargetProperty( const Variant& );

   static FMatrix sRGBConversionMatrixFromTarget( const View& );
   static FMatrix sRGBConversionMatrixFromTarget( FileFormatInstance& );
   static FMatrix sRGBConversionMatrixFromTargetProperty( const Variant& );

   void EvaluateNoise( FVector& noiseEstimates, FVector& noiseFractions, StringList& noiseAlgorithms,
                       const ImageVariant&, const IsoString& cfaPattern ) const;

   void ApplyErrorPolicy();

   friend class DebayerProcess;
   friend class DebayerInterface;
   friend class DebayerEngine;
   friend class DebayerFileThread;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __DebayerInstance_h

// ----------------------------------------------------------------------------
// EOF DebayerInstance.h - Released 2021-04-09T19:41:49Z
