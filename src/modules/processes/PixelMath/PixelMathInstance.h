//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// PixelMathInstance.h - Released 2021-05-05T15:38:07Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
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

#ifndef __PixelMathInstance_h
#define __PixelMathInstance_h

#include <pcl/Image.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/Thread.h>

#include "Expression.h"
#include "PixelMathParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PixelMathInstance : public ProcessImplementation
{
public:

   typedef Expression::component_list  component_list;
   typedef Array<component_list>       rpn_set;

   PixelMathInstance( const MetaProcess* );
   PixelMathInstance( const PixelMathInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsHistoryUpdater( const View& v ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   /*
    * Properties of the current target image.
    */

   static int TargetWidth()
   {
      return s_targetWidth;
   }

   static int TargetHeight()
   {
      return s_targetHeight;
   }

   static int TargetNumberOfChannels()
   {
      return s_targetNumberOfChannels;
   }

   static bool TargetIsColor()
   {
      return s_targetIsColor;
   }

   static const RGBColorSystem& TargetRGBWS()
   {
      return *s_targetRGBWS;
   }

private:

   String    p_expression[ 4 ];        // R/K/RGB, G, B, Alpha
   pcl_bool  p_useSingleExpression;    // Use expression[0] for all channels
   String    p_symbols;                // comma-delimited list of symbol definitions
   pcl_bool  p_clearImageCacheAndExit; // Special execution mode to clear the image cache
   pcl_bool  p_cacheGeneratedImages;   // Preserve generated images before/after execution
   pcl_bool  p_generateOutput;         // Generate an output image
   pcl_bool  p_singleThreaded;         // Use a single thread
   pcl_bool  p_optimization;           // Use code optimization
   pcl_bool  p_use64BitWorkingImage;   // Use 64-bit floating point data for intermediate calculation
   pcl_bool  p_rescaleResult;          // Rescale result after execution
   double    p_rescaleLower;           // Rescaling range, lower bound
   double    p_rescaleUpper;           // Rescaling range, upper bound
   pcl_bool  p_truncateResult;         // Truncate result after execution, if not rescaled
   double    p_truncateLower;          // Truncation range, lower bound
   double    p_truncateUpper;          // Truncation range, upper bound
   pcl_bool  p_createNewImage;         // Destination: new image or replace target
   pcl_bool  p_showNewImage;           // (Scripting only) Show newly created image windows
   String    p_newImageId;             // Image identifier (empty = auto)
   int32     p_newImageWidth;          // Width in pixels (< 0 = same as target)
   int32     p_newImageHeight;         // Height in pixels (< 0 = same as target)
   pcl_bool  p_newImageAlpha;          // Create an alpha channel
   pcl_enum  p_newImageColorSpace;     // RGB or grayscale (can be same as target)
   pcl_enum  p_newImageSampleFormat;   // Integer/real, bit count (can be same as target)

   Array<Symbol::OutputData> o_outputData;

   static int s_targetWidth, s_targetHeight, s_targetNumberOfChannels;
   static bool s_targetIsColor;
   static RGBColorSystem* s_targetRGBWS;

   void SolveInvariants( rpn_set [], const ImageWindow& targetWindow = ImageWindow::Null() );
   void SolveBranchTargets( rpn_set [] );

   void Execute( ImageVariant&, int x0, int y0, int w0, int h0, const rpn_set RPN[] );
   void Execute( int width, int height, int numberOfChannels, const rpn_set RPN[] );

   friend class PixelMathProcess;
   friend class PixelMathInterface;
   friend class ExpressionEditorDialog;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PixelMathInstance_h

// ----------------------------------------------------------------------------
// EOF PixelMathInstance.h - Released 2021-05-05T15:38:07Z
