//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.5.1
// ----------------------------------------------------------------------------
// ImageCalibrationInstance.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#ifndef __ImageCalibrationInstance_h
#define __ImageCalibrationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Vector.h>
#include <pcl/Matrix.h>

#include "ImageCalibrationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class FileFormatInstance;

class CalibrationThread;
struct CalibrationThreadData;

typedef IndirectArray<CalibrationThread>  thread_list;

class ImageCalibrationInstance : public ProcessImplementation
{
public:

   ImageCalibrationInstance( const MetaProcess* );
   ImageCalibrationInstance( const ImageCalibrationInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool IsHistoryUpdater( const View& v ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   struct OverscanRegions
   {
      pcl_bool enabled;    // whether to apply this overscan correction
      Rect     sourceRect; // source overscan region
      Rect     targetRect; // image region to be corrected

      OverscanRegions( const Rect& rs = Rect( 0 ), const Rect& rt = Rect( 0 ), bool enable = false )
         : enabled( enable )
         , sourceRect( rs )
         , targetRect( rt )
      {
      }

      OverscanRegions( const OverscanRegions& ) = default;

      bool IsValid() const
      {
         if ( !enabled )
            return true;
         if ( !sourceRect.IsNormal() || !targetRect.IsNormal() )
            return false;
         if ( sourceRect.x0 < 0 || sourceRect.y0 < 0 ||
              targetRect.x0 < 0 || targetRect.y0 < 0 )
            return false;
         return true;
      }
   };

   struct Overscan
   {
      pcl_bool        enabled = false; // whether overscan correction is globally enabled
      OverscanRegions overscan[ 4 ];   // four overscan source and target regions
      Rect            imageRect = 0;   // image region (i.e. the cropping rectangle)

      Overscan() = default;
      Overscan( const Overscan& ) = default;

      bool IsValid() const
      {
         if ( !enabled )
            return true;
         for ( int i = 0; i < 4; ++i )
            if ( !overscan[i].IsValid() )
               return false;
         if ( !imageRect.IsNormal() )
            return false;
         if ( imageRect.x0 < 0 || imageRect.y0 < 0 )
            return false;
         return true;
      }

      bool HasOverscanRegions() const
      {
         for ( int i = 0; i < 4; ++i )
            if ( overscan[i].enabled )
               return true;
         return false;
      }
   };

   typedef Array<OverscanRegions> overscan_list;
   typedef Array<overscan_list>   overscan_table;

private:

   struct ImageItem
   {
      pcl_bool enabled = true; // if disabled, skip (ignore) this image
      String   path;           // absolute file path

      ImageItem( const String& p = String() )
         : path( p )
      {
      }

      ImageItem( const ImageItem& ) = default;

      bool IsValid() const
      {
         return !enabled || !path.IsEmpty();
      }
   };

   typedef Array<ImageItem>  image_list;

   // The set of target frames to be calibrated
   image_list      p_targetFrames;

   // CFA parameters
   pcl_bool        p_enableCFA;
   pcl_enum        p_cfaPattern;

   // Format hints
   String          p_inputHints;    // e.g.: "raw cfa"
   String          p_outputHints;   // e.g.: "bottom-up"

   // Subtractive pedestal
   int32           p_pedestal;      // in 16-bit DN
   pcl_enum        p_pedestalMode;  // literal | default keyword | custom keyword
   String          p_pedestalKeyword;

   // Overscan source and target regions
   Overscan        p_overscan;

   // Master bias, dark and flat frames
   ImageItem       p_masterBias;
   ImageItem       p_masterDark;
   ImageItem       p_masterFlat;

   // Calibration options
   pcl_bool        p_calibrateBias; // apply master bias frame calibration (overscan)
   pcl_bool        p_calibrateDark; // apply master dark frame calibration (overscan+bias)
   pcl_bool        p_calibrateFlat; // apply master flat frame calibration (overscan+bias+dark)

   // Dark frame optimization
   pcl_bool        p_optimizeDarks;             // perform dark frame optimizations
   float           p_darkOptimizationThreshold; // ### DEPRECATED
   float           p_darkOptimizationLow;       // lower bound for dark frame optimization in sigma units
   int32           p_darkOptimizationWindow;    // size in px of the central region for dark optimization
   pcl_enum        p_darkCFADetectionMode;      // ### DEPRECATED

   // Flat frame calibration options
   pcl_bool        p_separateCFAFlatScalingFactors;
   float           p_flatScaleClippingFactor;

   // Noise estimates
   pcl_bool        p_evaluateNoise;   // perform MRS noise evaluation
   pcl_enum        p_noiseEvaluationAlgorithm;

   // Output files
   String          p_outputDirectory;
   String          p_outputExtension;
   String          p_outputPrefix;
   String          p_outputPostfix;
   pcl_enum        p_outputSampleFormat;
   int32           p_outputPedestal;  // additive in 16-bit DN to ensure positivity
   pcl_bool        p_overwriteExistingFiles;
   pcl_enum        p_onError;
   pcl_bool        p_noGUIMessages;   // ### DEPRECATED

   // Read-only output properties
   struct OutputData
   {
      String     outputFilePath;
      FVector    darkScalingFactors = FVector( 0.0F, 3 );
      FVector    noiseEstimates = FVector( 0.0F, 3 );
      FVector    noiseFractions = FVector( 0.0F, 3 );
      StringList noiseAlgorithms = StringList( size_type( 3 ) );
   };
   Array<OutputData> o_output;

   // -------------------------------------------------------------------------

   // Working geometry validation
   Rect m_geometry;             // uncalibrated, including overscan regions
   Rect m_calibratedGeometry;   // calibrated, with overscan regions trimmed
   void ValidateImageGeometry( const Image*, bool uncalibrated = true );

   // Group source overscan regions with identical target regions
   overscan_table BuildOverscanTable() const;

   // Subtract input pedestal if appropriate, depending on instance parameters
   // and file keywords.
   void SubtractPedestal( Image*, FileFormatInstance& );

   // Read a master calibration frame
   Image* LoadCalibrationFrame( const String& filePath, bool willCalibrate, IsoString* cfaPattern = nullptr );

   // Read a target frame file
   thread_list LoadTargetFrame( const String& filePath, const CalibrationThreadData& );

   // Write a calibrated image file
   void WriteCalibratedImage( const CalibrationThread* );

   // CFA pattern identification
   IsoString CFAPatternFromTarget( FileFormatInstance& ) const;

   friend class CalibrationThread;
   friend class ImageCalibrationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ImageCalibrationInstance_h

// ----------------------------------------------------------------------------
// EOF ImageCalibrationInstance.h - Released 2021-04-09T19:41:48Z
