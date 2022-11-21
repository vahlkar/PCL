//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.35
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 1.9.7
// ----------------------------------------------------------------------------
// LocalNormalizationInstance.cpp - Released 2022-11-21T14:47:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
//
// Copyright (c) 2003-2022 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "LocalNormalizationInstance.h"
#include "LocalNormalizationParameters.h"
#include "OutputFileData.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/Constants.h>
#include <pcl/Convolution.h>
#include <pcl/ErrorHandler.h>
#include <pcl/ExternalProcess.h>
#include <pcl/FFTConvolution.h>
#include <pcl/GaussianFilter.h>
#include <pcl/GlobalSettings.h>
#include <pcl/GridInterpolation.h>
#include <pcl/ImageWindow.h>
#include <pcl/LinearFit.h>
#include <pcl/LocalNormalizationData.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/MultiscaleMedianTransform.h>
#include <pcl/PSFScaleEstimator.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/Resample.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/ShepardInterpolation.h>
#include <pcl/SpinStatus.h>
#include <pcl/StandardStatus.h>
#include <pcl/SurfaceSpline.h>
#include <pcl/Version.h>
#include <pcl/View.h>

#define RCR_LARGE_SAMPLE   20000

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * B3-Spline wavelet scaling function.
 */
static const float s_B3[] =
{
   0.003906F, 0.015625F, 0.023438F, 0.015625F, 0.003906F,
   0.015625F, 0.062500F, 0.093750F, 0.062500F, 0.015625F,
   0.023438F, 0.093750F, 0.140625F, 0.093750F, 0.023438F,
   0.015625F, 0.062500F, 0.093750F, 0.062500F, 0.015625F,
   0.003906F, 0.015625F, 0.023438F, 0.015625F, 0.003906F
};
static const float s_B3v[] = { 0.0625F, 0.25F, 0.375F, 0.25F, 0.0625F };
static const float s_B3k[] = { 0.8907F, 0.2007F, 0.0856F };

// ----------------------------------------------------------------------------

LocalNormalizationInstance::LocalNormalizationInstance( const MetaProcess* P )
   : ProcessImplementation( P )
   , p_scale( TheLNScaleParameter->DefaultValue() )
   , p_noScale( TheLNNoScaleParameter->DefaultValue() )
   , p_globalLocationNormalization( TheLNGlobalLocationNormalizationParameter->DefaultValue() )
   , p_truncate( TheLNTruncateParameter->DefaultValue() )
   , p_backgroundSamplingDelta( TheLNBackgroundSamplingDeltaParameter->DefaultValue() )
   , p_rejection( TheLNRejectionParameter->DefaultValue() )
   , p_referenceRejection( TheLNReferenceRejectionParameter->DefaultValue() )
   , p_lowClippingLevel( TheLNLowClippingLevelParameter->DefaultValue() )
   , p_highClippingLevel( TheLNHighClippingLevelParameter->DefaultValue() )
   , p_referenceRejectionThreshold( TheLNReferenceRejectionThresholdParameter->DefaultValue() )
   , p_targetRejectionThreshold( TheLNTargetRejectionThresholdParameter->DefaultValue() )
   , p_hotPixelFilterRadius( TheLNHotPixelFilterRadiusParameter->DefaultValue() )
   , p_noiseReductionFilterRadius( TheLNNoiseReductionFilterRadiusParameter->DefaultValue() )
   , p_modelScalingFactor( TheLNModelScalingFactorParameter->DefaultValue() )
   , p_scaleEvaluationMethod( LNScaleEvaluationMethod::Default )
   , p_localScaleCorrections( TheLNLocalScaleCorrectionsParameter->DefaultValue() )
   , p_structureLayers( TheLNStructureLayersParameter->DefaultValue() )
   , p_saturationThreshold( TheLNSaturationThresholdParameter->DefaultValue() )
   , p_saturationRelative( TheLNSaturationRelativeParameter->DefaultValue() )
   , p_rejectionLimit( TheLNRejectionLimitParameter->DefaultValue() )
   , p_psfNoiseLayers( TheLNPSFNoiseLayersParameter->DefaultValue() )
   , p_psfHotPixelFilterRadius( TheLNPSFHotPixelFilterRadiusParameter->DefaultValue() )
   , p_psfNoiseReductionFilterRadius( TheLNPSFNoiseReductionFilterRadiusParameter->DefaultValue() )
   , p_psfMinStructureSize( TheLNPSFMinStructureSizeParameter->DefaultValue() )
   , p_psfMinSNR( TheLNPSFMinSNRParameter->DefaultValue() )
   , p_psfAllowClusteredSources( TheLNPSFAllowClusteredSourcesParameter->DefaultValue() )
   , p_psfType( LNPSFType::Default )
   , p_psfGrowth( TheLNPSFGrowthParameter->DefaultValue() )
   , p_psfMaxStars( TheLNPSFMaxStarsParameter->DefaultValue() )
   , p_referencePathOrViewId( TheLNReferencePathOrViewIdParameter->DefaultValue() )
   , p_referenceIsView( TheLNReferenceIsViewParameter->DefaultValue() )
   , p_inputHints( TheLNInputHintsParameter->DefaultValue() )
   , p_outputHints( TheLNOutputHintsParameter->DefaultValue() )
   , p_generateNormalizedImages( LNGenerateNormalizedImages::Default )
   , p_generateNormalizationData( TheLNGenerateNormalizationDataParameter->DefaultValue() )
   , p_generateInvalidData( TheLNGenerateInvalidDataParameter->DefaultValue() )
   , p_showBackgroundModels( TheLNShowBackgroundModelsParameter->DefaultValue() )
   , p_showLocalScaleModels( TheLNShowLocalScaleModelsParameter->DefaultValue() )
   , p_showRejectionMaps( TheLNShowRejectionMapsParameter->DefaultValue() )
   , p_showStructureMaps( TheLNShowStructureMapsParameter->DefaultValue() )
   , p_plotNormalizationFunctions( LNPlotNormalizationFunctions::Default )
   , p_noGUIMessages( TheLNNoGUIMessagesParameter->DefaultValue() ) // ### DEPRECATED
   , p_autoMemoryLimit( TheLNAutoMemoryLimitParameter->DefaultValue() )
   , p_outputDirectory( TheLNOutputDirectoryParameter->DefaultValue() )
   , p_outputExtension( TheLNOutputExtensionParameter->DefaultValue() )
   , p_outputPrefix( TheLNOutputPrefixParameter->DefaultValue() )
   , p_outputPostfix( TheLNOutputPostfixParameter->DefaultValue() )
   , p_overwriteExistingFiles( TheLNOverwriteExistingFilesParameter->DefaultValue() )
   , p_onError( LNOnError::Default )
   , p_useFileThreads( TheLNUseFileThreadsParameter->DefaultValue() )
   , p_fileThreadOverload( TheLNFileThreadOverloadParameter->DefaultValue() )
   , p_maxFileReadThreads( TheLNMaxFileReadThreadsParameter->DefaultValue() )
   , p_maxFileWriteThreads( TheLNMaxFileWriteThreadsParameter->DefaultValue() )
   , p_graphSize( TheLNGraphSizeParameter->DefaultValue() )
   , p_graphTextSize( TheLNGraphTextSizeParameter->DefaultValue() )
   , p_graphTitleSize( TheLNGraphTitleSizeParameter->DefaultValue() )
   , p_graphTransparent( TheLNGraphTransparentParameter->DefaultValue() )
   , p_graphOutputDirectory( TheLNGraphOutputDirectoryParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

LocalNormalizationInstance::LocalNormalizationInstance( const LocalNormalizationInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInstance::Assign( const ProcessImplementation& p )
{
   const LocalNormalizationInstance* x = dynamic_cast<const LocalNormalizationInstance*>( &p );
   if ( x != nullptr )
   {
      p_scale                         = x->p_scale;
      p_noScale                       = x->p_noScale;
      p_globalLocationNormalization   = x->p_globalLocationNormalization;
      p_truncate                      = x->p_truncate;
      p_backgroundSamplingDelta       = x->p_backgroundSamplingDelta;
      p_rejection                     = x->p_rejection;
      p_referenceRejection            = x->p_referenceRejection;
      p_lowClippingLevel              = x->p_lowClippingLevel;
      p_highClippingLevel             = x->p_highClippingLevel;

      /*
       * N.B. Some outlier rejection parameters have changed their meaning and
       * valid ranges after a redesign of the outlier rejection algorithms.
       * Replace these parameters with default values for existing instances
       * created by older versions.
       */
      if ( p.Version() < 0x101 )
      {
         p_referenceRejectionThreshold = TheLNReferenceRejectionThresholdParameter->DefaultValue();
         p_targetRejectionThreshold    = TheLNTargetRejectionThresholdParameter->DefaultValue();
      }
      else
      {
         p_referenceRejectionThreshold = x->p_referenceRejectionThreshold;
         p_targetRejectionThreshold    = x->p_targetRejectionThreshold;
      }

      p_hotPixelFilterRadius          = x->p_hotPixelFilterRadius;
      p_noiseReductionFilterRadius    = x->p_noiseReductionFilterRadius;
      p_modelScalingFactor            = x->p_modelScalingFactor;
      p_scaleEvaluationMethod         = x->p_scaleEvaluationMethod;
      p_localScaleCorrections         = x->p_localScaleCorrections;
      p_structureLayers               = x->p_structureLayers;
      p_saturationThreshold           = x->p_saturationThreshold;
      p_saturationRelative            = x->p_saturationRelative;
      p_rejectionLimit                = x->p_rejectionLimit;
      p_psfNoiseLayers                = x->p_psfNoiseLayers;
      p_psfHotPixelFilterRadius       = x->p_psfHotPixelFilterRadius;
      p_psfNoiseReductionFilterRadius = x->p_psfNoiseReductionFilterRadius;
      p_psfMinStructureSize           = x->p_psfMinStructureSize;
      p_psfMinSNR                     = x->p_psfMinSNR;
      p_psfAllowClusteredSources      = x->p_psfAllowClusteredSources;
      p_psfType                       = x->p_psfType;
      p_psfGrowth                     = x->p_psfGrowth;
      p_psfMaxStars                   = x->p_psfMaxStars;
      p_referencePathOrViewId         = x->p_referencePathOrViewId;
      p_referenceIsView               = x->p_referenceIsView;
      p_targets                       = x->p_targets;
      p_inputHints                    = x->p_inputHints;
      p_outputHints                   = x->p_outputHints;
      p_generateNormalizedImages      = x->p_generateNormalizedImages;
      p_generateNormalizationData     = x->p_generateNormalizationData;
      p_generateInvalidData           = x->p_generateInvalidData;
      p_showBackgroundModels          = x->p_showBackgroundModels;
      p_showLocalScaleModels          = x->p_showLocalScaleModels;
      p_showRejectionMaps             = x->p_showRejectionMaps;
      p_showStructureMaps             = x->p_showStructureMaps;
      p_plotNormalizationFunctions    = x->p_plotNormalizationFunctions;
      p_noGUIMessages                 = x->p_noGUIMessages;
      p_autoMemoryLimit               = x->p_autoMemoryLimit;
      p_outputDirectory               = x->p_outputDirectory;
      p_outputExtension               = x->p_outputExtension;
      p_outputPrefix                  = x->p_outputPrefix;
      p_outputPostfix                 = x->p_outputPostfix;
      p_overwriteExistingFiles        = x->p_overwriteExistingFiles;
      p_onError                       = x->p_onError;
      p_useFileThreads                = x->p_useFileThreads;
      p_fileThreadOverload            = x->p_fileThreadOverload;
      p_maxFileReadThreads            = x->p_maxFileReadThreads;
      p_maxFileWriteThreads           = x->p_maxFileWriteThreads;
      p_graphSize                     = x->p_graphSize;
      p_graphTextSize                 = x->p_graphTextSize;
      p_graphTitleSize                = x->p_graphTitleSize;
      p_graphTransparent              = x->p_graphTransparent;
      p_graphOutputDirectory          = x->p_graphOutputDirectory;

      o_output                        = x->o_output;
   }
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::IsHistoryUpdater( const View& view ) const
{
   return p_generateNormalizedImages == LNGenerateNormalizedImages::ViewExecutionOnly ||
          p_generateNormalizedImages == LNGenerateNormalizedImages::Always;
}

// ----------------------------------------------------------------------------

UndoFlags LocalNormalizationInstance::UndoMode( const View& view ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Location estimates
 */
template <class I>
static Vector Location( const I& image )
{
   Vector m( image.NumberOfChannels() );
   image.SetRangeClipping( 0, 1 );
   for ( int c = 0; c < image.NumberOfChannels(); ++c )
   {
      image.SelectChannel( c );
      m[c] = image.Median();
   }
   image.ResetSelections();
   return m;
}

/*
 * Winsorized standard deviation
 */
// template <class I>
// static Vector WinsorizedSD( const I& image, double gl, double gh )
// {
//    Vector wsd( image.NumberOfChannels() );
//    image.SetRangeClipping( 0, 1 );
//    for ( int c = 0; c < image.NumberOfChannels(); ++c )
//    {
//       image.SelectChannel( c );
//       float l = image.OrderStatistic( gl );
//       float h = image.OrderStatistic( 1 - gh );
//       ImageVariant W;
//       W.CreateFloatImage().CopyImage( image );
//       for ( Image::sample_iterator w( static_cast<Image&>( *W ) ); w; ++w )
//          if ( *w != 0 )
//             *w = Range( *w, l, h );
//       W.SetRangeClipping( 0, 1 );
//       W.SetMaxProcessors( image.MaxProcessors() );
//       wsd[c] = W.StdDev();
//    }
//    image.ResetSelections();
//    return wsd;
// }

// ----------------------------------------------------------------------------

static void RunGnuplot( const String& gnuFilePath )
{
   Console console;

   String gnuplotPath = PixInsightSettings::GlobalString( "Application/BinDirectory" ) + "/gnuplot";
#ifdef __PCL_WINDOWS
   gnuplotPath << ".exe";
#endif
   if ( !File::Exists( gnuplotPath ) )
      throw Error( "The gnuplot executable does not exist: " + gnuplotPath );

   ExternalProcess P;
   P.Start( gnuplotPath, StringList() << gnuFilePath );
   if ( !P.WaitForStarted() )
      throw Error( "Unable to launch gnuplot executable: " + gnuplotPath );

   Module->ProcessEvents();

   console.Write( "<end><cbr>Running gnuplot:  " );
   for ( int n = 0; P.IsRunning() && !P.WaitForFinished( 250 ); ++n )
   {
      console.Write( String( "<end>\b" ) + "-/|\\"[n%4] );
      Module->ProcessEvents();
      if ( console.AbortRequested() )
      {
         console.NoteLn( "<end><cbr>* Abort requested - killing process." );
         P.Kill();
         throw ProcessAborted();
      }
   }
   console.WriteLn( "<end>\bdone" );

   if ( P.HasCrashed() )
      throw Error( "The gnuplot executable has crashed: " + gnuplotPath );
   if ( P.ExitCode() != 0 )
      throw Error( "Gnuplot rendering failed. Exit code = " + String( P.ExitCode() ) );
}

// ----------------------------------------------------------------------------

class LocalNormalizationThread : public Thread
{
public:

   typedef ShepardInterpolation<double>   background_interpolation;

   typedef GridInterpolation              background_model;

   typedef Array<background_model>        background_models;

   LocalNormalizationThread( const LocalNormalizationInstance& instance,
                             const Image& referenceImage,
                             const Image& referenceBackground,
                             const UInt8Image& referenceStructures,
                             const UInt8Image& referenceSignificant,
                             const Vector& referenceLocation,
                             const Array<PSFScaleEstimator>& scaleEstimators )
      : m_instance( instance )
      , m_R( referenceImage )
      , m_RB( referenceBackground )
      , m_RM( referenceStructures )
      , m_RS( referenceSignificant )
      , m_Rc( referenceLocation )
      , m_scaleEstimators( scaleEstimators )
   {
   }

   LocalNormalizationThread( const LocalNormalizationInstance& instance,
                             size_type index,
                             const Image& referenceImage,
                             const Image& referenceBackground,
                             const UInt8Image& referenceStructures,
                             const UInt8Image& referenceSignificant,
                             const Vector& referenceLocation,
                             const Array<PSFScaleEstimator>& scaleEstimators,
                             int maxThreads = PCL_MAX_PROCESSORS )
      : m_instance( instance )
      , m_index( index )
      , m_R( referenceImage )
      , m_RB( referenceBackground )
      , m_RM( referenceStructures )
      , m_RS( referenceSignificant )
      , m_Rc( referenceLocation )
      , m_scaleEstimators( scaleEstimators )
      , m_targetFilePath( m_instance.p_targets[m_index].path )
      , m_maxThreads( maxThreads )
   {
      if ( m_instance.p_referenceIsView )
         m_referenceFilePath = View::ViewById( m_instance.p_referencePathOrViewId ).Window().FilePath();
      else
         m_referenceFilePath = m_instance.p_referencePathOrViewId;
   }

   LocalNormalizationThread( const LocalNormalizationInstance& instance,
                             ImageVariant& targetImage,
                             const Image& referenceImage,
                             const Image& referenceBackground,
                             const UInt8Image& referenceStructures,
                             const UInt8Image& referenceSignificant,
                             const Vector& referenceLocation,
                             const Array<PSFScaleEstimator>& scaleEstimators,
                             int maxThreads = PCL_MAX_PROCESSORS )
      : m_instance( instance )
      , m_R( referenceImage )
      , m_RB( referenceBackground )
      , m_RM( referenceStructures )
      , m_RS( referenceSignificant )
      , m_Rc( referenceLocation )
      , m_scaleEstimators( scaleEstimators )
      , m_targetImage( targetImage )
      , m_maxThreads( maxThreads )
   {
      m_targetImage.SetOwnership( false );
   }

   void Run() override
   {
      Console console;

      try
      {
         Module->ProcessEvents();

         if ( !ReadInputData() )
            return;

         Build();

         if ( GeneratesNormalizedImages() )
            Apply();

         WriteOutputData();

         m_success = true;
      }
      catch ( ... )
      {
         if ( IsRootThread() )
         {
            if ( !m_invalid )
               throw;

            try
            {
               throw;
            }
            ERROR_HANDLER
            WriteOutputData();
            m_success = true;
            return;
         }

         if ( m_invalid )
         {
            try
            {
               throw;
            }
            ERROR_HANDLER
            WriteOutputData();
            m_success = true;
         }
         else
         {
            String text = ConsoleOutputText();
            ClearConsoleOutputText();
            try
            {
               throw;
            }
            ERROR_HANDLER
            m_errorInfo = ConsoleOutputText();
            ClearConsoleOutputText();
            console.Write( text );
         }
      }
   }

   template <class P>
   void ProcessReferenceImage( Image& R, Image& RB, UInt8Image& RM, UInt8Image& RS, Vector& Rc, Array<PSFScaleEstimator>& S,
                               const GenericImage<P>& referenceImage, ImageVariant& rejectionImage,
                               bool showImages )
   {
      Console console;
      SpinStatus spin;

      console.NoteLn( "<end><cbr>* Processing reference image" );

      referenceImage.SelectNominalChannels();
      R = referenceImage;
      referenceImage.ResetSelections();

      if ( m_instance.p_scaleEvaluationMethod == LNScaleEvaluationMethod::PSFSignal )
      {
         console.WriteLn( "<end><cbr>* Initializing PSF scale estimators" );
         StandardStatus status;
         R.SetStatusCallback( &status );

         S = Array<PSFScaleEstimator>( R.NumberOfNominalChannels() );

         for ( int c = 0; c < R.NumberOfNominalChannels(); ++c )
         {
            S[c].Detector().SetStructureLayers( m_instance.p_structureLayers );
            S[c].Detector().SetNoiseLayers( m_instance.p_psfNoiseLayers );
            S[c].Detector().SetHotPixelFilterRadius( m_instance.p_psfHotPixelFilterRadius );
            S[c].Detector().SetNoiseReductionFilterRadius( m_instance.p_psfNoiseReductionFilterRadius );
            S[c].Detector().SetMinStructureSize( m_instance.p_psfMinStructureSize );
            S[c].Detector().SetMinSNR( m_instance.p_psfMinSNR );
            S[c].Detector().EnableClusteredSources( m_instance.p_psfAllowClusteredSources );
            S[c].SetPSFType( LNPSFType::ToPSFFunction( m_instance.p_psfType ) );
            S[c].SetSaturationThreshold( m_instance.p_saturationThreshold );
            S[c].EnableRelativeSaturation( m_instance.p_saturationRelative );
            S[c].SetRejectionLimit( m_instance.p_rejectionLimit );
            S[c].SetGrowthFactorForFluxMeasurement( m_instance.p_psfGrowth );
            S[c].SetMaxStars( m_instance.p_psfMaxStars );
            S[c].EnableLocalModel( m_instance.p_localScaleCorrections );

            R.SelectChannel( c );
            S[c].SetReference( ImageVariant( &R ) );
         }

         R.ResetSelections();
         R.SetStatusCallback( nullptr );
      }

      Rc = Location( R );

      console.WriteLn( "* Generating local background model" );

      m_monitor.SetCallback( &spin );
      m_monitor.Initialize( "Pixel rejection" );
      m_monitor.DisableInitialization();

      UInt8Image Rmap;
      bool showRmap = showImages && m_instance.p_showRejectionMaps;
      if ( showRmap )
         Rmap.AllocateData( R.Width(), R.Height(), R.NumberOfChannels(), R.ColorSpace() ).Fill( uint8( 0 ) );

      /*
       * Rejection of insignificant and saturated pixel samples.
       */
      for ( int c = 0; c < R.NumberOfChannels(); ++c )
      {
         UInt8Image::sample_iterator rm;
         if ( showRmap )
            rm = UInt8Image::sample_iterator( Rmap, c );
         for ( Image::sample_iterator r( R, c ); r; ++r )
         {
            if ( *r <= m_instance.p_lowClippingLevel || *r >= m_instance.p_highClippingLevel )
               *r = 0;
            if ( showRmap )
            {
               if ( *r == 0 )
                  *rm = uint8( 0xff );
               ++rm;
            }
         }
         ++m_monitor;
      }

      Vector Rn;

      if ( m_instance.p_rejection )
      {
         /*
          * Reference noise estimates
          */
         Rn = Vector( R.NumberOfChannels() );
         for ( int c = 0; c < R.NumberOfChannels(); ++c )
            Rn[c] = Noise( R, c );

         /*
          * Outlier rejection for the reference image. If this option is
          * enabled then we have received a reference to the selected target
          * image for comparison as the rejectionImage function argument.
          */
         if ( m_instance.p_referenceRejection )
         {
            Image& T = static_cast<Image&>( *rejectionImage );
            for ( int c = 0; c < T.NumberOfChannels(); ++c )
            {
               for ( Image::sample_iterator t( T, c ); t; ++t )
                  if ( *t <= m_instance.p_lowClippingLevel || *t >= m_instance.p_highClippingLevel )
                     *t = 0;
               ++m_monitor;
            }

            background_models Rz = FixZero( R );
            background_models Tz = FixZero( T );

            for ( int c = 0; c < R.NumberOfChannels(); ++c )
            {
               float rs = m_instance.p_targetRejectionThreshold * Rn[c];
               float ts = m_instance.p_referenceRejectionThreshold * Noise( T, c );
               UInt8Image::sample_iterator rm;
               if ( showRmap )
                  rm = UInt8Image::sample_iterator( Rmap, c );
               Image::sample_iterator r( R, c );
               Image::const_sample_iterator t( T, c );
               for ( int y = 0; y < R.Height(); ++y )
                  for ( int x = 0; x < R.Width(); ++x, ++r, ++t )
                  {
                     if ( *t < Tz[c]( x, y ) + ts )
                        if ( *r > Rz[c]( x, y ) + rs )
                        {
                           *r = 0;
                           if ( showRmap )
                              *rm = uint8( 0xff );
                        }
                     if ( showRmap )
                        ++rm;
                  }

               ++m_monitor;
            }
         }

         if ( showRmap )
            CreateImageWindow( Rmap, "LN_rmap_r" );
      }

      Image R0 = R;

      FixZero( R );

      m_monitor.Complete();

      m_monitor.EnableInitialization();
      m_monitor.Initialize( "Local background" );
      m_monitor.DisableInitialization();

      RB = LS( R );

      m_monitor.Complete();

      if ( showImages )
         if ( m_instance.p_showBackgroundModels )
         {
            Image RB0 = RB;
            for ( int c = 0; c < RB0.NumberOfChannels(); ++c )
            {
               Image::sample_iterator rb0( RB0, c );
               for ( Image::const_sample_iterator r0( R0, c ); r0; ++r0, ++rb0 )
                  if ( *r0 == 0 )
                     *rb0 = 0;
            }
            CreateImageWindow( RB0, "LN_background_r" );
         }

      if ( !m_instance.p_noScale )
         if ( m_instance.p_scaleEvaluationMethod == LNScaleEvaluationMethod::MultiscaleAnalysis )
         {
            console.WriteLn( "<end><cbr>* Extracting significant structures" );

            m_monitor.EnableInitialization();
            m_monitor.Initialize( "Multiscale analysis" );
            m_monitor.DisableInitialization();

            RM = StructureMap( R, RB );
            for ( int c = 0; c < RM.NumberOfChannels(); ++c )
            {
               UInt8Image::sample_iterator rm( RM, c );
               for ( Image::const_sample_iterator r0( R0, c ); r0; ++r0, ++rm )
                  if ( *r0 == 0 )
                     *rm = 0;
            }

            m_monitor.Complete();

            if ( showImages )
               if ( m_instance.p_showStructureMaps )
                  CreateImageWindow( RM, "LN_structures_r" );
         }

      R = R0;

      if ( m_instance.p_rejection )
      {
         RS.AllocateData( R.Width(), R.Height(), R.NumberOfChannels() );
         for ( int c = 0; c < R.NumberOfChannels(); ++c )
         {
            float s = m_instance.p_referenceRejectionThreshold * Rn[c];
            Image::const_sample_iterator r( R, c );
            Image::const_sample_iterator rb( RB, c );
            for ( UInt8Image::sample_iterator rs( RS, c ); rs; ++rs, ++r, ++rb )
               *rs = (*r != 0 && *r < *rb + s) ? uint8( 0 ) : uint8( 0xff );
         }
      }
   }

   void ProcessReferenceImage( Image& R, Image& RB, UInt8Image& RM, UInt8Image& RS, Vector& Rc, Array<PSFScaleEstimator>& S,
                               const ImageVariant& referenceImage, ImageVariant& rejectionImage,
                               bool showImages )
   {
      if ( referenceImage.IsFloatSample() )
         switch ( referenceImage.BitsPerSample() )
         {
         case 32: ProcessReferenceImage( R, RB, RM, RS, Rc, S, static_cast<const Image&>( *referenceImage ), rejectionImage, showImages ); break;
         case 64: ProcessReferenceImage( R, RB, RM, RS, Rc, S, static_cast<const DImage&>( *referenceImage ), rejectionImage, showImages ); break;
         }
      else
         switch ( referenceImage.BitsPerSample() )
         {
         case  8: ProcessReferenceImage( R, RB, RM, RS, Rc, S, static_cast<const UInt8Image&>( *referenceImage ), rejectionImage, showImages ); break;
         case 16: ProcessReferenceImage( R, RB, RM, RS, Rc, S, static_cast<const UInt16Image&>( *referenceImage ), rejectionImage, showImages ); break;
         case 32: ProcessReferenceImage( R, RB, RM, RS, Rc, S, static_cast<const UInt32Image&>( *referenceImage ), rejectionImage, showImages ); break;
         }
   }

   bool ExecutedOnView() const
   {
      return m_targetFilePath.IsEmpty();
   }

   bool GeneratesNormalizedImages() const
   {
      return !m_invalid &&
             (m_instance.p_generateNormalizedImages == LNGenerateNormalizedImages::Always ||
              m_instance.p_generateNormalizedImages == (ExecutedOnView() ?
                                    LNGenerateNormalizedImages::ViewExecutionOnly :
                                    LNGenerateNormalizedImages::GlobalExecutionOnly));
   }

   size_type Index() const
   {
      return m_index;
   }

   void GetOutputData( LocalNormalizationInstance::OutputData& o ) const
   {
      o.outputFilePathXNML = m_outputFilePathXNML;
      o.outputFilePath = m_outputFilePath;
      o.scaleFactors = m_scaleFactors;
      o.valid = !m_invalid;
   }

   String TargetFilePath() const
   {
      return m_targetFilePath;
   }

   bool Succeeded() const
   {
      return m_success;
   }

   bool Invalid() const
   {
      return m_invalid;
   }

   String ErrorInfo() const
   {
      return m_errorInfo;
   }

private:

   const LocalNormalizationInstance& m_instance;
         size_type                   m_index = 0; // position of this thread in the instance's target list
   const Image&                      m_R;  // reference image
   const Image&                      m_RB; // reference local background
   const UInt8Image&                 m_RM; // reference structure map
   const UInt8Image&                 m_RS; // reference significance map
   const Vector&                     m_Rc; // reference global location estimates
   const Array<PSFScaleEstimator>&   m_scaleEstimators;
         String                      m_referenceFilePath;
         String                      m_targetFilePath;
         OutputFileData              m_fileData;
         ImageVariant                m_targetImage;
         int                         m_maxThreads = PCL_MAX_PROCESSORS;
         Image                       m_A0;
         Image                       m_A1;
         Vector                      m_Tc;
         GenericVector<PSFScaleEstimator::Estimates> m_s;
         String                      m_outputFilePathXNML;
         String                      m_outputFilePath;
         FVector                     m_scaleFactors = Vector( 1.0F, 3 );
         String                      m_errorInfo;
         StatusMonitor               m_monitor;
         bool                        m_invalid = false;
         bool                        m_success = false;

   // -------------------------------------------------------------------------

   template <class P>
   static void CreateImageWindow( const GenericImage<P>& image, const IsoString& id, bool rescale = false )
   {
      ImageWindow window( 1, 1, 1, P::BitsPerSample(), P::IsFloatSample(), false/*color*/, true/*processing*/, id );
      window.MainView().Image().CopyImage( image );
      if ( rescale )
         window.MainView().Image().Rescale();
      window.Show();
   }

   // -------------------------------------------------------------------------

   class FixZeroThread : public Thread
   {
   public:

      FixZeroThread( Image& image, const background_model& G, int channel, int startRow, int endRow )
         : m_image( image )
         , m_G( G )
         , m_channel( channel ) , m_startRow( startRow ) , m_endRow( endRow )
      {
      }

      void Run() override
      {
         Image::sample_iterator r( m_image, m_channel );
         r.MoveBy( 0, m_startRow );
         for ( int y = m_startRow; y < m_endRow; ++y )
            for ( int x = 0; x < m_image.Width(); ++x, ++r )
               if ( *r == 0 )
                  *r = m_G( x, y );
      }

   private:

      Image&                  m_image;
      const background_model& m_G;
      int                     m_channel;
      int                     m_startRow;
      int                     m_endRow;
   };

   background_models FixZero( Image& image )
   {
      const int w   = image.Width();
      const int h   = image.Height();
      const int d   = m_instance.p_backgroundSamplingDelta;
      const int d2  = d >> 1;
      const int dx0 = (w - w/d * d) >> 1;
      const int dy0 = (h - h/d * d) >> 1;

      image.SetRangeClipping( 0, 1 );

      background_models B;

      for ( int c = 0; c < image.NumberOfChannels(); ++c )
      {
         Array<double> X, Y, Z;
         for ( int y = dy0; y < h; y += d )
            for ( int x = dx0; x < w; x += d )
               if ( image( x, y, c ) != 0 )
               {
                  double z = image.Median( Rect( x-d2, y-d2, x+d2+1, y+d2+1 ), c, c, 1 );
                  if ( 1 + z != 1 )
                  {
                     X << x;
                     Y << y;
                     Z << z;
                  }
               }

         ++m_monitor;

         if ( X.Length() < 16 )
            throw Error( "LocalNormalizationThread::FixZero(): Insufficient data to sample background image pixels, channel " + String( c ) );

         background_interpolation S;
         S.SetRadius( 0.1 );
         S.Initialize( X.Begin(), Y.Begin(), Z.Begin(), X.Length() );

         ++m_monitor;

         background_model G;
         G.Initialize( image.Bounds(), 16/*delta*/, S, false/*verbose*/ );

         ++m_monitor;

         Array<size_type> L = Thread::OptimalThreadLoads( image.Height(), 1/*overheadLimit*/, m_maxThreads );
         ReferenceArray<FixZeroThread> threads;
         for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
            threads << new FixZeroThread( image, G, c, n, n + int( L[i] ) );
         if ( threads.Length() > 1 )
         {
            for ( int i = 0; i < int( threads.Length() ); ++i )
               threads[i].Start( ThreadPriority::DefaultMax );
            for ( int i = 0; i < int( threads.Length() ); ++i )
               threads[i].Wait();
         }
         else
            threads[0].Run();

         threads.Destroy();

         B << G;

         ++m_monitor;
      }

      return B;
   }

   // -------------------------------------------------------------------------

   float Noise( const Image& image, int c )
   {
      ATrousWaveletTransform::WaveletScalingFunction H;
      if ( SeparableConvolution::FasterThanNonseparableFilterSize( Thread::NumberOfThreads( PCL_MAX_PROCESSORS ) ) > 5 )
         H.Set( KernelFilter( s_B3, 5 ) );
      else
         H.Set( SeparableFilter( s_B3v, s_B3v, 5 ) );
      ATrousWaveletTransform W( H, 2 );
      image.PushSelections();
      image.SelectChannel( c );
      W << image; // 2*N
      image.PopSelections();
      return W.NoiseKSigma( 1/*j*/, 3/*k*/, 0.01F/*eps*/, 10/*nit*/ )/s_B3k[1];
      ++m_monitor;
   }

   // -------------------------------------------------------------------------

   void Reject( Image& T, UInt8Image& Tmap )
   {
      /*
       * Setup rejection map.
       */
      Tmap.AllocateData( T.Width(), T.Height(), T.NumberOfNominalChannels(), T.ColorSpace() ).Fill( uint8( 0 ) );

      /*
       * Initial rejection of insignificant and saturated pixel samples.
       */
      for ( int c = 0; c < T.NumberOfChannels(); ++c )
      {
         UInt8Image::sample_iterator tm( Tmap, c );
         for ( Image::sample_iterator t( T, c ); t; ++t, ++tm )
         {
            if ( *t <= m_instance.p_lowClippingLevel || *t >= m_instance.p_highClippingLevel )
               *t = 0;
            if ( *t == 0 )
               *tm = uint8( 0xff );
         }
      }

      ++m_monitor;

      /*
       * Initial approximate background model and replacement of black pixels.
       */
      background_models Tz = FixZero( T );

      /*
       * Optional hot/cold pixel removal.
       */
      if ( m_instance.p_hotPixelFilterRadius > 0 )
      {
         MorphologicalTransformation M;
         M.SetMaxProcessors( m_maxThreads );
         M.SetOperator( MedianFilter() );
         if ( m_instance.p_hotPixelFilterRadius > 1 )
            M.SetStructure( CircularStructure( 2*m_instance.p_hotPixelFilterRadius + 1 ) );
         else
            M.SetStructure( BoxStructure( 3 ) );
         M >> T;
      }

      ++m_monitor;

      /*
       * Optional noise reduction.
       */
      if ( m_instance.p_noiseReductionFilterRadius > 0 )
      {
         SeparableConvolution C( GaussianFilter( 2*m_instance.p_noiseReductionFilterRadius + 1 ).AsSeparableFilter() );
         C.SetMaxProcessors( m_maxThreads );
         C >> T;
      }

      ++m_monitor;

      /*
       * Outlier rejection.
       */
      if ( m_instance.p_rejection )
      {
         for ( int c = 0; c < T.NumberOfChannels(); ++c )
         {
            float tn = m_instance.p_targetRejectionThreshold * Noise( T, c );
            Image::sample_iterator t( T, c );
            UInt8Image::const_sample_iterator rs( m_RS, c );
            UInt8Image::sample_iterator tm( Tmap, c );
            for ( int y = 0; y < T.Height(); ++y )
               for ( int x = 0; x < T.Width(); ++x, ++t, ++rs, ++tm )
                  if ( *rs == 0 ) // if reference is not significant
                     if ( *t > Tz[c]( x, y ) + tn ) // if target is significant
                     {
                        *t = 0;
                        *tm = uint8( 0xff );
                     }

            ++m_monitor;
         }

         FixZero( T );
      }
   }

   // -------------------------------------------------------------------------

   /*
    * Large-scale component extraction with the MMT.
    */
   Image LS( const Image& image )
   {
      /*
       * Accelerated Multiscale Median Transform.
       *
       * The following values for the number of layers (n) and inverse sampling
       * ratio (r) approximate the rigorous transforms with negligible errors
       * using dyadic scaling sequences and decimation.
       *
       * We apply nearest neighbor interpolation for decimation/undecimation in
       * order to avoid contamination of the nonlinear multiscale transform
       * with linear components.
       */
      int n, r;
      switch ( m_instance.p_scale )
      {
      case 8192: n = 9; r = 16; break;
      case 6144: n = 9; r = 12; break;
      case 4096: n = 9; r =  8; break;
      case 3072: n = 8; r = 12; break;
      case 2048: n = 8; r =  8; break;
      case 1536: n = 8; r =  6; break;
      default: // ?!
      case 1024: n = 7; r =  8; break;
      case  768: n = 7; r =  6; break;
      case  512: n = 6; r =  8; break;
      case  384: n = 6; r =  6; break;
      case  256: n = 6; r =  4; break;
      case  192: n = 6; r =  3; break;
      case  128: n = 6; r =  2; break;
      case   64: n = 5; r =  2; break;
      case   32: n = 4; r =  2; break;
      }

      /*
       * MMT
       */
      MultiscaleMedianTransform M( n, 0/*dyadic*/ );
      M.SetMaxProcessors( m_maxThreads );
      for ( int i = 0; i < M.NumberOfLayers(); ++i )
         M.DisableLayer( i );

      {
         /*
          * Decimate
          */
         Image I( image );
         I.Status() = m_monitor;
         NearestNeighborPixelInterpolation N;
         Resample R( N, 1.0/r );
         R.SetMaxProcessors( m_maxThreads );
         R >> I;

         /*
          * Perform MMT
          */
         M << I;

         m_monitor = I.Status();
         M[M.NumberOfLayers()].Status() = m_monitor;

         /*
          * Undecimate
          */
         R.SetMode( ResizeMode::AbsolutePixels );
         R.SetSizes( image.Width(), image.Height() );
         R >> M[M.NumberOfLayers()];
      }

      /*
       * Apply a convolution with a Gaussian filter to smooth out
       * discontinuities.
       */
      GaussianFilter H( (m_instance.p_scale >> 1)|1 );
      if (     H.Size() >= FFTConvolution::FasterThanSeparableFilterSize( Thread::NumberOfThreads( m_maxThreads ) )
            || H.Size() >= image.Width()
            || H.Size() >= image.Height() )
      {
         FFTConvolution F( H );
         F.SetMaxProcessors( m_maxThreads );
         F >> M[M.NumberOfLayers()];
      }
      else if ( H.Size() >= SeparableConvolution::FasterThanNonseparableFilterSize( Thread::NumberOfThreads( m_maxThreads ) ) )
      {
         SeparableConvolution S( H.AsSeparableFilter() );
         S.SetMaxProcessors( m_maxThreads );
         S >> M[M.NumberOfLayers()];
      }
      else
      {
         Convolution C( H );
         C.SetMaxProcessors( m_maxThreads );
         C >> M[M.NumberOfLayers()];
      }

      m_monitor = M[M.NumberOfLayers()].Status();
      M[M.NumberOfLayers()].Status() = StatusMonitor();

      return M[M.NumberOfLayers()];
   }

   UInt8Image StructureMap( const Image& image, const Image& background ) // 2*N
   {
      ATrousWaveletTransform::WaveletScalingFunction H;
      if ( SeparableConvolution::FasterThanNonseparableFilterSize( m_maxThreads ) > 5 )
         H.Set( KernelFilter( s_B3, 5 ) );
      else
         H.Set( SeparableFilter( s_B3v, s_B3v, 5 ) );

      UInt8Image map( image.Width(), image.Height(), image.ColorSpace() );

      for ( int c = 0; c < image.NumberOfChannels(); ++c )
      {
         image.SelectChannel( c );
         background.SelectChannel( c );
         Image str = image;
         str.SetMaxProcessors( m_maxThreads );
         str.Status() = m_monitor;

         str -= background;

         {
            ATrousWaveletTransform W( H, m_instance.p_structureLayers );
            W.SetMaxProcessors( m_maxThreads );
            for ( int i = 0; i < W.NumberOfLayers(); ++i )
               W.DisableLayer( i );
            for ( int i = 0; i < 3; ++i )
            {
               W << str;
               str -= W[m_instance.p_structureLayers];
               str.Truncate();
               str.Rescale();
            }
         }

         {
            MultiscaleMedianTransform M( 3 );
            M.SetMaxProcessors( m_maxThreads );
            for ( int i = 0; i < M.NumberOfLayers(); ++i )
               M.DisableLayer( i );
            for ( int i = 0; i < 2; ++i )
            {
               M << str;
               str = M[3];
               str.Truncate();
               str.Rescale();
            }
         }

         float saturation = m_instance.p_saturationThreshold;
         if ( m_instance.p_saturationRelative )
            saturation *= image.MaximumSampleValue();
         UInt8Image::sample_iterator p( map, c );
         for ( Image::const_sample_iterator s( str ); s; ++s, ++p )
            *p = (*s > 1e-6 && *s <= saturation) ? 0xff : 0;

         MorphologicalTransformation M;
         M.SetMaxProcessors( m_maxThreads );
         M.SetStructure( CircularStructure( 5 ) );
         M.SetOperator( ErosionFilter() );
         M >> map;
         M.SetOperator( DilationFilter() );
         M >> map;
      }

      image.ResetSelections();

      return map;
   }

   // -------------------------------------------------------------------------

   static int TicsDelta( int size )
   {
      int x = size/8;
      return x - x%TruncInt( Pow10I<double>()( TruncInt( Log( double( x ) ) ) ) );
   }

   void Plot( int component )
   {
      const Image& image = (component == 0) ? m_A0 : m_A1;
      const String fileSuffix = (component == 0) ? "_a0" : "_a1";
      const IsoString windowSuffix = (component == 0) ? "_offset" : "_scale";
      const IsoString titleComponent = (component == 0) ? "Offset" : "Scale";
      const String tmpDir = File::SystemTempDirectory();
      const double sx = double( image.Width() )/m_R.Width();
      const double sy = double( image.Height() )/m_R.Height();
      const double gridStep = Max( m_R.Width(), m_R.Height() )/64;
      const int xGridSize = RoundInt( m_R.Width()/gridStep );
      const int yGridSize = RoundInt( m_R.Height()/gridStep );

      for ( int c = 0; c < image.NumberOfChannels(); ++c )
      {
         Module->ProcessEvents();

         BicubicBSplineInterpolation<float> A;
         A.Initialize( image[c], image.Width(), image.Height() );
         IsoString text;
         for ( int x = 0, n = 0; x < xGridSize; ++x )
            for ( int y = 0; y < yGridSize; ++y, ++n )
            {
               double nx = x*gridStep;
               double ny = y*gridStep;
               text << IsoString( nx ) << ' ' << IsoString( ny ) << ' ' << IsoString( A( sx*nx, sy*ny ) ) << '\n';
            }
         String datFilePath = tmpDir + "/ln" + fileSuffix + ".dat";
         File::WriteTextFile( datFilePath, text );

         String outputDir = m_instance.p_graphOutputDirectory.Trimmed();
         if ( outputDir.IsEmpty() )
            outputDir = tmpDir;
         else
         {
            if ( !File::DirectoryExists( outputDir ) )
            {
               Console console;
               console.WarningLn( "** Warning: The specified graph output directory does not exist: <raw>" + outputDir + "</raw>" );
               console.WarningLn( "Will use the system temp directory instead: <raw>" + tmpDir + "</raw>" );
            }
         }
         if ( !outputDir.EndsWith( '/' ) )
            outputDir << '/';

         String gnuFilePath = tmpDir + '/' + "ln" + fileSuffix + ".gnu";
         String svgFilePath = outputDir + "ln" + fileSuffix + ".svg";

         text.Clear();
         text << "set terminal svg size ";
         if ( m_instance.p_plotNormalizationFunctions == LNPlotNormalizationFunctions::Map3D )
         {
            if ( m_R.Width() >= m_R.Height() )
               text << IsoString( m_instance.p_graphSize ) << ','
                    << IsoString( RoundInt( m_R.Height()*double( m_instance.p_graphSize )/m_R.Width() ) );
            else
               text << IsoString( RoundInt( m_R.Width()*double( m_instance.p_graphSize )/m_R.Height() ) ) << ','
                    << IsoString( m_instance.p_graphSize );
         }
         else
            text << IsoString( m_instance.p_graphSize ) << ',' << IsoString( m_instance.p_graphSize );
         text << " font \"helvetica," << IsoString( m_instance.p_graphTextSize ) << "\" enhanced";
         if ( !m_instance.p_graphTransparent )
            text << " background \"#FFFFFF\"";
         text << '\n';

         text << "set samples " << IsoString( xGridSize ) << "," << IsoString( yGridSize ) << '\n'
              << "set isosamples " << IsoString( xGridSize ) << "," << IsoString( yGridSize ) << '\n'
              << "set dgrid3d " << IsoString( xGridSize ) << "," << IsoString( yGridSize ) << '\n'
              << "set xyplane 0\n"
              << "set hidden3d\n"
              << "set yrange [] reverse\n"
              << "set xtics 0," << IsoString( TicsDelta( m_R.Width() ) ) << "," << IsoString( m_R.Width()-1 ) << '\n'
              << "set ytics 0," << IsoString( TicsDelta( m_R.Height() ) ) << "," << IsoString( m_R.Height()-1 ) << '\n';

         if ( m_instance.p_plotNormalizationFunctions != LNPlotNormalizationFunctions::Map3D )
            text << "set xtics offset character 0,-0.25\n"
                 << "set ytics offset character 1.75\n"
                 << "set border 895\n";

         if ( m_instance.p_plotNormalizationFunctions != LNPlotNormalizationFunctions::Line3D )
            text << "set pm3d interpolate 0,0\n";

         if ( m_instance.p_plotNormalizationFunctions == LNPlotNormalizationFunctions::Map3D )
            text << "set view map scale 1\n"
                 << "set lmargin at screen 0.10\n"
                 << "set rmargin at screen 0.85\n"
                 << "set tmargin at screen 0.10\n"
                 << "set bmargin at screen 0.85\n";
         else
            text << "set margins 0,0,0,0\n";

         text << "set grid xtics ytics ztics\n"
              << "set cbtics format \"%.4e\"\n"
              << "set contour base\n"
              << "set title \"Local Normalization " << titleComponent << " Component, Channel " << IsoString( c ) << '\"'
                  << " font \"helvetica," << IsoString( m_instance.p_graphTitleSize ) << "\" offset character 0,0.5\n"
              << "unset key\n"
              << "set output \"" << svgFilePath.ToUTF8() << "\"\n"
              << "splot \"" << datFilePath.ToUTF8() << "\" using 1:2:3 with ";

         if ( m_instance.p_plotNormalizationFunctions == LNPlotNormalizationFunctions::Line3D )
            text << "lines lc rgbcolor \"#E00000\"\n";
         else
            text << "pm3d\n";

         File::WriteTextFile( gnuFilePath, text );

         RunGnuplot( gnuFilePath );

         {
            Bitmap bmp( svgFilePath );
            UInt8Image image;
            image.AllocateData( bmp.Width(), bmp.Height(), m_instance.p_graphTransparent ? 4 : 3, ColorSpace::RGB );
            image.Blend( bmp );
            CreateImageWindow( image, "LN" + windowSuffix );
         }
      }
   }

   // -------------------------------------------------------------------------

   /*
    * The iterative sigma clipping rejection with Chauvenet criterion
    * implements an algorithm based on:
    *
    * Maples, M. P., Reichart, D. E. et al.,
    * Robust Chauvenet Outlier Rejection, Astrophysical Journal Supplement
    * Series, 2018, ApJS, 238, A2, 1 - 49
    */

   /*
    * The Q-function: the probability for a value from a normal distribution of
    * being more than x standard deviations from the mean.
    */
   static double QF( double x )
   {
      return 0.5*(1 - Erf( x/Const<double>::sqrt2() ));
   }

   /*
    * Correction factor for 68.3-percentile deviations to avoid overaggressive
    * rejection for small samples.
    */
   static double FN( int N )
   {
      return 1/(1 - 2.9442*Pow( double( N ), -1.073 ));
   }

   /*
    * The 68.3-percentile value from the distribution of absolute deviations.
    */
   static double SampleDeviation( const Array<double>& R, int i, int j, double m )
   {
      int N = j - i;
      Vector D( N );
      for ( int k = 0; i < j; ++i, ++k )
         D[k] = Abs( R[i] - m );
      return FN( N ) * *pcl::Select( D.Begin(), D.End(), TruncInt( 0.683*D.Length() ) );
   }

   /*
    * 68.3-percentile deviation by fitting a zero-intercept line to the vector
    * of absolute differences.
    */
   static double LineFitDeviation( const Array<double>& R, int i, int j, double m )
   {
      int N = j - i;
      int n = int( 0.683*N + 0.317 );
      if ( n < 8 )
         return SampleDeviation( R, i, j, m );

      Vector y( N );
      for ( int k = 0, ii = i; ii < j; ++ii, ++k )
         y[k] = Abs( R[ii] - m );
      y.Sort();
      y = Vector( y.Begin(), n );

      Vector x( n );
      for ( int i = 0; i < n; ++i )
         x[i] = Const<double>::sqrt2() * ErfInv( (i + 1 - 0.317)/N );

      double s;
      LinearFit f( x, y );
      if ( f.IsValid() )
         s = FN( N ) * f( 1.0 );
      else
         s = SampleDeviation( R, i, j, m );

      return s;
   }

   template <class P>
   void Build( const GenericImage<P>& target )
   {
      Console console;
      SpinStatus spin;

      if ( ExecutedOnView() )
         console.NoteLn( "<end><cbr>* Processing target image" );

      if ( target.Bounds() != m_R.Bounds() || target.NumberOfNominalChannels() != m_R.NumberOfChannels() )
         throw Error( "LocalNormalizationThread::Build(): Internal error: Incompatible image geometries." );

      target.SelectNominalChannels();

      Image T = target;

      target.ResetSelections();

      console.WriteLn( "* Generating local background model" );

      m_monitor.SetCallback( &spin );
      m_monitor.Initialize( "Pixel rejection" );
      m_monitor.DisableInitialization();

      UInt8Image Tmap;
      Reject( T, Tmap );

      m_monitor.Complete();

      if ( m_instance.p_showRejectionMaps )
         if ( m_instance.p_rejection )
            if ( ExecutedOnView() )
               CreateImageWindow( Tmap, "LN_rmap_t" );

      m_monitor.EnableInitialization();
      m_monitor.Initialize( "Local background" );
      m_monitor.DisableInitialization();

      Image TB = LS( T );

      m_monitor.Complete();

      T = target;

      if ( m_instance.p_showBackgroundModels )
         if ( ExecutedOnView() )
         {
            Image TB0 = TB;
            for ( int c = 0; c < T.NumberOfChannels(); ++c )
            {
               Image::sample_iterator tb0( TB0, c );
               for ( Image::const_sample_iterator t( T, c ); t; ++t, ++tb0 )
                  if ( *t == 0 )
                     *tb0 = 0;
            }
            CreateImageWindow( TB0, "LN_background_t" );
         }

      m_s = GenericVector<PSFScaleEstimator::Estimates>( T.NumberOfChannels() );

      if ( !m_instance.p_noScale )
      {
         switch ( m_instance.p_scaleEvaluationMethod )
         {
         default:
         case LNScaleEvaluationMethod::PSFSignal:
            {
               console.WriteLn( "<end><cbr>* Estimating global scale factors" );

               StandardStatus status;
               T.SetStatusCallback( &status );

               for ( int c = 0; c < T.NumberOfChannels(); ++c )
               {
                  T.SelectChannel( c );
                  PSFScaleEstimator::Estimates se = m_scaleEstimators[c].EstimateScale( ImageVariant( &T ) );
                  if ( !se.IsValid() )
                  {
                     m_invalid = m_instance.p_generateInvalidData;
                     throw Error( String().Format( "Unable to compute a valid scale estimate (channel %d)", c ) );
                  }

                  m_s[c] = se;
                  console.WriteLn( String().Format( "<end><cbr>ch %d : scale = %.4f, sigma = %.4e, %d valid PSF fits, %d PSF signal estimates",
                                                    c, m_s[c].scale, m_s[c].sigma, m_s[c].total, m_s[c].count ) );
                  m_scaleFactors[c] = m_s[c].scale;
               }

               T.ResetSelections();
               T.SetStatusCallback( nullptr );
            }
            break;

         case LNScaleEvaluationMethod::MultiscaleAnalysis:
            {
               console.WriteLn( "<end><cbr>* Extracting significant structures" );

               for ( int c = 0; c < T.NumberOfChannels(); ++c )
               {
                  UInt8Image::const_sample_iterator tm( Tmap, c );
                  for ( Image::sample_iterator t( T, c ); t; ++t, ++tm )
                     if ( *tm )
                        *t = 0;
               }

               m_monitor.EnableInitialization();
               m_monitor.Initialize( "Multiscale analysis" );
               m_monitor.DisableInitialization();

               FixZero( T );
               UInt8Image TM = StructureMap( T, TB );
               T = target;
               for ( int c = 0; c < T.NumberOfChannels(); ++c )
               {
                  UInt8Image::sample_iterator tm( TM, c );
                  for ( Image::const_sample_iterator t( T, c ); t; ++t, ++tm )
                     if ( *t == 0 )
                        *tm = 0;
               }

               m_monitor.Complete();

               if ( m_instance.p_showStructureMaps )
                  if ( ExecutedOnView() )
                     CreateImageWindow( TM, "LN_structures_t" );

               console.WriteLn( "<end><cbr>* Estimating global scale factors" );

               for ( int c = 0; c < T.NumberOfChannels(); ++c )
               {
                  m_monitor.EnableInitialization();
                  m_monitor.Initialize( String().Format( "Robust estimation (channel %d)",c ) );
                  m_monitor.DisableInitialization();

                  Array<double> S;
                  UInt8Image::const_sample_iterator rm( m_RM, c );
                  UInt8Image::const_sample_iterator tm( TM, c );
                  for ( Image::const_sample_iterator r( m_R, c ), t( T, c ), rb( m_RB, c ), tb( TB, c ); r; ++r, ++t, ++rb, ++tb, ++rm, ++tm )
                     if ( *rm && *tm )
                        if ( *r > *rb && *t > *tb )
                           if ( 1 + (*t - *tb) != 1 )
                              S << double( *r - *rb )/(*t - *tb);
                  if ( S.Length() < 64 )
                  {
                     m_invalid = m_instance.p_generateInvalidData;
                     throw Error( String().Format( "Unable to compute a valid scale estimate (channel %d)", c ) );
                  }

                  /*
                   * Robust Chauvenet rejection.
                   */
                  S.Sort();
                  int i = 0;
                  int j = S.Length();
                  double m, d;

                  for ( int phase = 0; phase < 3; ++phase )
                     for ( ;; )
                     {
                        switch ( phase )
                        {
                        case  0: // + robustness / - precision
                           m = Median( S.At( i ), S.At( j ) );
                           d = LineFitDeviation( S, i, j, m );
                           break;
                        case  1: // = robustness / = precision
                           m = Median( S.At( i ), S.At( j ) );
                           d = SampleDeviation( S, i, j, m );
                           break;
                        case  2: // - robustness / + precision
                           m = Mean( S.At( i ), S.At( j ) );
                           d = StdDev( S.At( i ), S.At( j ), m );
                           break;
                        }

                        if ( 1 + d == 1 )
                           goto __rcr_end;

                        int n = j - i;
                        if ( n < 3 )
                           goto __rcr_end;

                        if ( n <= RCR_LARGE_SAMPLE + (RCR_LARGE_SAMPLE >> 1) )
                        {
                           /*
                            * Optimal single pixel rejection for small and
                            * moderately large samples.
                            */
                           double d0 = n*QF( (m - S[i])/d );
                           double d1 = n*QF( (S[j-1] - m)/d );
                           if ( d0 >= m_instance.p_rejectionLimit && d1 >= m_instance.p_rejectionLimit )
                              break;
                           if ( d1 < d0 )
                              --j;
                           else
                              ++i;
                        }
                        else
                        {
                           /*
                            * Accelerated bulk rejection for large samples.
                            */
                           int nc = RoundInt( double( n )/RCR_LARGE_SAMPLE );
                           int c = 0;
                           for ( int it = 0; it < nc && n*QF( (m - S[i])/d ) < m_instance.p_rejectionLimit; ++i, ++c, ++it ) {}
                           for ( int it = 0; it < nc && n*QF( (S[j-1] - m)/d ) < m_instance.p_rejectionLimit; --j, ++c, ++it ) {}
                           if ( c == 0 )
                              break;
                        }

                        ++m_monitor;
                     }
__rcr_end:
                  m_s[c].scale = m;
                  m_s[c].sigma = d;
                  m_s[c].total = int( S.Length() );
                  m_s[c].count = j - i;

                  m_monitor.Complete();

                  console.WriteLn( String().Format( "<end><cbr>ch %d : scale = %.4f, sigma = %.4e, %d measured pixels, %d significant pixels",
                                                    c, m_s[c].scale, m_s[c].sigma, m_s[c].total, m_s[c].count ) );
                  m_scaleFactors[c] = m_s[c].scale;
               }
            }
            break;
         }
      } // !m_instance.p_noScale

      m_A0.AllocateData( T.Width(), T.Height(), T.NumberOfChannels() );
      m_A1.AllocateData( T.Width(), T.Height(), T.NumberOfChannels() );

      m_monitor.EnableInitialization();
      m_monitor.Initialize( "* Computing local normalization functions" );
      m_monitor.DisableInitialization();

      Vector smin( T.NumberOfChannels() );
      Vector smax( T.NumberOfChannels() );

      if ( !m_instance.p_noScale )
      {
         for ( int c = 0; c < T.NumberOfChannels(); ++c )
         {
            T.SelectChannel( c );

            if ( m_s[c].local.IsValid() )
            {
               Image dA1;
               dA1.AllocateData( T.Width(), T.Height() );
               GridInterpolation G;
               G.Initialize( T.Bounds(), 16/*delta*/, m_s[c].local, false/*verbose*/ );
               Image::sample_iterator da1( dA1 );
               for ( int y = 0; y < dA1.Height(); ++y )
                  for ( int x = 0; x < dA1.Width(); ++x, ++da1 )
                     *da1 = G( x, y );
               dA1.GetExtremeSampleValues( smin[c], smax[c] );

               if ( m_instance.p_showLocalScaleModels )
                  if ( ExecutedOnView() )
                     CreateImageWindow( dA1, IsoString().Format( "LN_scale_corrections_%d", c ), true/*rescale*/ );

               Image::sample_iterator a0( m_A0, c );
               Image::sample_iterator a1( m_A1, c );
               for ( Image::const_sample_iterator rb( m_RB, c ), tb( TB, c ), da1( dA1 ); rb; ++rb, ++tb, ++a0, ++a1, ++da1 )
               {
                  *a1 = m_s[c].scale + *da1;
                  *a0 = *rb - *a1 * *tb;
               }
            }
            else
            {
               Image::sample_iterator a0( m_A0, c );
               Image::sample_iterator a1( m_A1, c );
               for ( Image::const_sample_iterator rb( m_RB, c ), tb( TB, c ); rb; ++rb, ++tb, ++a0, ++a1 )
               {
                  *a1 = m_s[c].scale;
                  *a0 = *rb - *a1 * *tb;
               }
            }
         }
      }
      else
      {
         for ( int c = 0; c < T.NumberOfChannels(); ++c )
         {
            Image::sample_iterator a0( m_A0, c );
            Image::sample_iterator a1( m_A1, c );
            for ( Image::const_sample_iterator rb( m_RB, c ), tb( TB, c ); rb; ++rb, ++tb, ++a0, ++a1 )
            {
               *a0 = *rb - *tb;
               *a1 = 1.0;
            }
         }
      } // !m_instance.p_noScale

      m_monitor.Complete();

      if ( !m_instance.p_noScale )
         if ( m_instance.p_localScaleCorrections )
         {
            console.WriteLn( "<end><cbr>* Local scale corrections:" );
            for ( int c = 0; c < T.NumberOfChannels(); ++c )
               if ( m_s[c].local.IsValid() )
                  console.WriteLn( String().Format( "ch %d : delta_min = %+.4f, delta_max = %+.4f", c, smin[c], smax[c] ) );
               else
                  console.WriteLn( String().Format( "ch %d : &lt;* failed *&gt;", c ) );
         }

      {
         BicubicFilterPixelInterpolation BF( m_instance.p_scale, m_instance.p_scale, CubicBSplineFilter() );
         Resample S( BF, m_instance.p_modelScalingFactor/m_instance.p_scale );
         S.SetMaxProcessors( m_maxThreads );
         S.EnableUnclippedInterpolation();
         S >> m_A0;
         S >> m_A1;
      }

      if ( m_instance.p_globalLocationNormalization )
      {
         target.SelectNominalChannels();
         T = target;
         Apply( T, false/*truncate*/, false/*verbose*/ );
         T.SetMaxProcessors( m_maxThreads );
         m_Tc = Location( T );
      }

      if ( m_instance.p_plotNormalizationFunctions != LNPlotNormalizationFunctions::DontPlot )
         if ( ExecutedOnView() )
         {
            Plot( 0 );
            if ( !m_instance.p_noScale )
               if ( m_instance.p_localScaleCorrections )
                  Plot( 1 );
         }
   }

   void Build()
   {
      if ( m_targetImage.IsFloatSample() )
         switch ( m_targetImage.BitsPerSample() )
         {
         case 32: Build( static_cast<const Image&>( *m_targetImage ) ); break;
         case 64: Build( static_cast<const DImage&>( *m_targetImage ) ); break;
         }
      else
         switch ( m_targetImage.BitsPerSample() )
         {
         case  8: Build( static_cast<const UInt8Image&>( *m_targetImage ) ); break;
         case 16: Build( static_cast<const UInt16Image&>( *m_targetImage ) ); break;
         case 32: Build( static_cast<const UInt32Image&>( *m_targetImage ) ); break;
         }
   }

   // -------------------------------------------------------------------------

   template <class P>
   class ApplyThread : public Thread
   {
   public:

      ApplyThread( AbstractImage::ThreadData& data,
                   GenericImage<P>& target, const Image& A0, const Image& A1,
                   const Vector& Rc, const Vector& Tc, bool truncate,
                   int startRow, int endRow )
         : m_data( data )
         , m_target( target )
         , m_A0( A0 ), m_A1( A1 )
         , m_Rc( Rc ), m_Tc( Tc ), m_truncate( truncate )
         , m_startRow( startRow ), m_endRow( endRow )
      {
      }

      void Run() override
      {
         INIT_THREAD_MONITOR()

         double sx = double( m_A1.Width() )/m_target.Width();
         double sy = double( m_A1.Height() )/m_target.Height();

         for ( int c = 0; c < m_target.NumberOfNominalChannels(); ++c )
         {
            m_truncated << size_type( 0 );
            m_outputLow << DBL_MAX;
            m_outputHigh << -DBL_MAX;

            BicubicBSplineInterpolation<float> A0;
            A0.Initialize( m_A0[c], m_A0.Width(), m_A0.Height() );
            BicubicBSplineInterpolation<float> A1;
            A1.Initialize( m_A1[c], m_A1.Width(), m_A1.Height() );

            typename GenericImage<P>::sample_iterator t( m_target, c );
            t.MoveBy( 0, m_startRow );
            for ( int y = m_startRow; y < m_endRow; ++y )
            {
               double ys = sy*y;
               for ( int x = 0; x < m_target.Width(); ++x, ++t )
                  if ( *t != P::MinSampleValue() )
                  {
                     double f; P::FromSample( f, *t );
                     double xs = sx*x;
                     f = A0( xs, ys ) + A1( xs, ys )*f;
                     if ( !m_Tc.IsEmpty() )
                        f = f - m_Tc[c] + m_Rc[c];

                     if ( f < m_outputLow[c] )
                        m_outputLow[c] = f;
                     if ( f > m_outputHigh[c] )
                        m_outputHigh[c] = f;

                     if ( m_truncate )
                        if ( f < 0.0 )
                        {
                           f = 0.0;
                           ++m_truncated[c];
                        }
                        else if ( f > 1.0 )
                        {
                           f = 1.0;
                           ++m_truncated[c];
                        }

                     *t = P::ToSample( f );

                     UPDATE_THREAD_MONITOR( 65536 )
                  }
            }
         }
      }

      size_type Truncated( int c ) const
      {
         return m_truncated[c];
      }

      double OutputLow( int c ) const
      {
         return m_outputLow[c];
      }

      double OutputHigh( int c ) const
      {
         return m_outputHigh[c];
      }

   private:

            AbstractImage::ThreadData& m_data;
            GenericImage<P>&           m_target;
      const Image&                     m_A0;
      const Image&                     m_A1;
      const Vector&                    m_Rc;
      const Vector&                    m_Tc;
            bool                       m_truncate;
            Array<size_type>           m_truncated;
            Array<double>              m_outputLow;
            Array<double>              m_outputHigh;
            int                        m_startRow;
            int                        m_endRow;
   };

   template <class P>
   void Apply( GenericImage<P>& target, bool truncate = true, bool verbose = true )
   {
      StandardStatus status;
      StatusMonitor monitor;
      if ( verbose )
      {
         monitor.SetCallback( &status );
         monitor.Initialize( "* Applying local normalization", target.NumberOfSamples() );
      }

      truncate &= bool( m_instance.p_truncate );

      Array<size_type> L = Thread::OptimalThreadLoads( target.Height(), 1/*overheadLimit*/, m_maxThreads );
      AbstractImage::ThreadData data( monitor, target.NumberOfSamples() );
      ReferenceArray<ApplyThread<P> > threads;
      for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
         threads << new ApplyThread<P>( data, target, m_A0, m_A1, m_Rc, m_Tc, truncate, n, n + int( L[i] ) );
      AbstractImage::RunThreads( threads, data, false/*useAffinity*/ );

      if ( verbose )
      {
         Console console;
         console.WriteLn( "<end><cbr>* Output ranges: " );

         for ( int c = 0; c < target.NumberOfNominalChannels(); ++c )
         {
            size_type truncated = 0;
            double outputLow = DBL_MAX;
            double outputHigh = -DBL_MAX;
            for ( const auto& thread : threads )
            {
               truncated += thread.Truncated( c );
               if ( thread.OutputLow( c ) < outputLow )
                  outputLow = thread.OutputLow( c );
               if ( thread.OutputHigh( c ) > outputHigh )
                  outputHigh = thread.OutputHigh( c );
            }

            if ( truncate )
               console.WriteLn( String().Format( "ch %d : low = %.8f, high = %.8f, %u truncated pixels (%.3f%%)",
                                                 c, outputLow, outputHigh, truncated, 100.0*truncated/target.NumberOfPixels() ) );
            else
               console.WriteLn( String().Format( "ch %d : low = %.8f, high = %.8f",
                                                 c, outputLow, outputHigh ) );
         }
      }

      threads.Destroy();
   }

   void Apply()
   {
      if ( m_targetImage.IsFloatSample() )
         switch ( m_targetImage.BitsPerSample() )
         {
         case 32: Apply( static_cast<Image&>( *m_targetImage ) ); break;
         case 64: Apply( static_cast<DImage&>( *m_targetImage ) ); break;
         }
      else
         switch ( m_targetImage.BitsPerSample() )
         {
         case  8: Apply( static_cast<UInt8Image&>( *m_targetImage ) ); break;
         case 16: Apply( static_cast<UInt16Image&>( *m_targetImage ) ); break;
         case 32: Apply( static_cast<UInt32Image&>( *m_targetImage ) ); break;
         }
   }

   // -------------------------------------------------------------------------

   bool ReadInputData()
   {
      if ( m_targetImage )
         return true;

      static Mutex mutex;
      static AtomicInt count;

      Console console;
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
         return false;
      }

      if ( images.Length() > 1 )
         console.NoteLn( String().Format( "* Ignoring %u additional image(s) in target file.", images.Length()-1 ) );

      m_targetImage.CreateSharedImage( images[0].options.ieeefpSampleFormat,
                                       false/*isComplex*/,
                                       images[0].options.bitsPerSample );
      {
         volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileReadThreads );
         if ( !file.ReadImage( m_targetImage ) )
            throw CaughtException();
         m_fileData = OutputFileData( file, images[0].options );
         if ( !file.Close() )
            throw CaughtException();
      }

      return true;
   }

   // -------------------------------------------------------------------------

   void WriteOutputData()
   {
      static Mutex mutex;
      static AtomicInt count;

      if ( m_targetFilePath.IsEmpty() )
         return;

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

      Console console;

      if ( m_instance.p_generateNormalizationData )
      {
         Module->ProcessEvents();

         m_outputFilePathXNML = fileDir + fileName + ".xnml";
         console.WriteLn( "<end><cbr>Writing output file: <raw>" + m_outputFilePathXNML + "</raw>" );

         UniqueFileChecks checks = File::EnsureNewUniqueFile( m_outputFilePathXNML, m_instance.p_overwriteExistingFiles );
         if ( checks.overwrite )
            console.WarningLn( "** Warning: Overwriting existing file." );
         else if ( checks.exists )
            console.NoteLn( "* File already exists, writing to: <raw>" + m_outputFilePathXNML + "</raw>" );

         LocalNormalizationData data;
         data.SetReferenceFilePath( m_referenceFilePath );
         data.SetTargetFilePath( m_targetFilePath );
         data.SetNormalizationScale( m_instance.p_scale );
         data.SetReferenceDimensions( m_R.Width(), m_R.Height() );

         if ( m_invalid )
         {
            console.WarningLn( "** Warning: Writing a local normalization data file tagged as invalid." );
            data.TagAsInvalid();
         }
         else
         {
            data.SetLocalNormalizationParameters( m_A1, m_A0 );
            if ( !m_instance.p_noScale )
            {
               Vector rs( m_s.Length() );
               for ( int c = 0; c < m_s.Length(); ++c )
                  rs[c] = m_s[c].scale;
               data.SetRelativeScaleFactors( rs );
            }
            if ( m_instance.p_globalLocationNormalization )
               data.SetGlobalNormalizationParameters( m_Rc, m_Tc, Vector( 1.0, m_Tc.Length() ) );
         }

         {
            volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileWriteThreads );
            data.SerializeToFile( m_outputFilePathXNML );
         }
      }

      if ( GeneratesNormalizedImages() )
      {
         String fileExtension = m_instance.p_outputExtension.Trimmed();
         if ( fileExtension.IsEmpty() )
            fileExtension = File::ExtractExtension( m_targetFilePath ).Trimmed();
         if ( fileExtension.IsEmpty() )
            throw Error( "Unable to determine an output file extension: " + m_targetFilePath );
         if ( !fileExtension.StartsWith( '.' ) )
            fileExtension.Prepend( '.' );

         m_outputFilePath = fileDir + m_instance.p_outputPrefix + fileName + m_instance.p_outputPostfix + fileExtension;

         console.WriteLn( "<end><cbr>Writing output file: <raw>" + m_outputFilePath + "</raw>" );

         UniqueFileChecks checks = File::EnsureNewUniqueFile( m_outputFilePath, m_instance.p_overwriteExistingFiles );
         if ( checks.overwrite )
            console.WarningLn( "** Warning: Overwriting existing file." );
         else if ( checks.exists )
            console.NoteLn( "* File already exists, writing to: <raw>" + m_outputFilePath + "</raw>" );

         FileFormat outputFormat( fileExtension, false/*read*/, true/*write*/ );
         FileFormatInstance outputFile( outputFormat );

         if ( !outputFile.Create( m_outputFilePath, m_instance.p_outputHints ) )
            throw CaughtException();

         ImageOptions options = m_fileData.options; // get resolution, etc.
         options.bitsPerSample = m_targetImage.BitsPerSample();
         options.ieeefpSampleFormat = m_targetImage.IsFloatSample();
         outputFile.SetOptions( options );

         bool canStore = true;
         if ( m_targetImage.IsFloatSample() )
            switch ( m_targetImage.BitsPerSample() )
            {
            case 32: canStore = outputFormat.CanStoreFloat(); break;
            case 64: canStore = outputFormat.CanStoreDouble(); break;
            }
         else
            switch ( m_targetImage.BitsPerSample() )
            {
            case  8: canStore = outputFormat.CanStore8Bit(); break;
            case 16: canStore = outputFormat.CanStore16Bit(); break;
            case 32: canStore = outputFormat.CanStore32Bit(); break;
            case 64: canStore = outputFormat.CanStore64Bit(); break;
            }

         if ( !canStore )
            console.WarningLn( "** Warning: The output format does not support the required sample data format." );

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
            keywords << FITSHeaderKeyword( "COMMENT",  IsoString(), "Normalization with " + PixInsightVersion::AsString() )
                     << FITSHeaderKeyword( "HISTORY",  IsoString(), "Normalization with " + Module->ReadableVersion() )
                     << FITSHeaderKeyword( "HISTORY",  IsoString(), "Normalization with LocalNormalization process" );
            outputFile.WriteFITSKeywords( keywords );
         }
         else
            console.WarningLn( "** Warning: The output format cannot store FITS keywords - keywords not embedded" );

         if ( m_fileData.profile.IsProfile() )
            if ( outputFormat.CanStoreICCProfiles() )
               outputFile.WriteICCProfile( m_fileData.profile );
            else
               console.WarningLn( "** Warning: The output format cannot store ICC profiles - existing profile not embedded" );

         Module->ProcessEvents();

         {
            volatile AutoLockCounter lock( mutex, count, m_instance.m_maxFileWriteThreads );
            if ( !outputFile.WriteImage( m_targetImage ) || !outputFile.Close() )
               throw CaughtException();
         }

         m_targetImage.Free();
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( p_referencePathOrViewId.IsEmpty() )
   {
      whyNot = "No reference image has been defined.";
      return false;
   }

   if ( view.IsPreview() && view.Bounds() != view.Window().MainView().Bounds() )
   {
      whyNot = "LocalNormalization cannot be executed on partial previews.";
      return false;
   }

   if ( view.Image().IsComplexSample() )
   {
      whyNot = "LocalNormalization cannot be executed on complex images.";
      return false;
   }

   if ( p_generateNormalizedImages != LNGenerateNormalizedImages::Always )
      if ( p_generateNormalizedImages != LNGenerateNormalizedImages::ViewExecutionOnly )
         if ( !p_showBackgroundModels )
            if ( !p_localScaleCorrections || !p_showLocalScaleModels )
               if ( !p_showRejectionMaps )
                  if ( !p_showStructureMaps )
                     if ( p_plotNormalizationFunctions == LNPlotNormalizationFunctions::DontPlot )
                     {
                        whyNot = "No process operation has been defined applicable to view execution.";
                        return false;
                     }

   return true;
}

// ----------------------------------------------------------------------------

static void LoadImage( ImageVariant& image, const String& path, const String& inputHints, const String& whatImage )
{
   Console console;

   console.WriteLn( "<end><cbr>Loading " + whatImage + " image: <raw>" + path + "</raw>" );

   FileFormat format( File::ExtractExtension( path ), true/*read*/, false/*write*/ );
   FileFormatInstance file( format );

   ImageDescriptionArray images;

   if ( !file.Open( images, path, inputHints ) )
      throw CaughtException();

   if ( images.IsEmpty() )
      throw Error( "Empty image: " + path );
   if ( images.Length() > 1 )
      console.NoteLn( String().Format( "<end><cbr>* Ignoring %u additional image(s) in file.", images.Length()-1 ) );

   image.CreateSharedImage( images[0].options.ieeefpSampleFormat,
                            false/*isComplex*/,
                            images[0].options.bitsPerSample );

   if ( !file.ReadImage( image ) || !file.Close() )
      throw CaughtException();
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::ExecuteOn( View& view )
{
   o_output.Clear();

   {
      String why;
      if ( !CanExecuteOn( view, why ) )
         throw Error( why );
   }

   m_maxFileReadThreads = p_maxFileReadThreads;
   if ( m_maxFileReadThreads < 1 )
      m_maxFileReadThreads = Max( 1, PixInsightSettings::GlobalInteger( "Process/MaxFileReadThreads" ) );

   m_maxFileWriteThreads = p_maxFileWriteThreads;
   if ( m_maxFileWriteThreads < 1 )
      m_maxFileWriteThreads = Max( 1, PixInsightSettings::GlobalInteger( "Process/MaxFileWriteThreads" ) );

   Console console;

   View referenceView = View::Null();
   if ( p_referenceIsView )
   {
      referenceView = View::ViewById( p_referencePathOrViewId );
      if ( referenceView.IsNull() )
         throw Error( "No such reference view: " + p_referencePathOrViewId );
      if ( referenceView.IsPreview() && referenceView.Bounds() != referenceView.Window().MainView().Bounds() )
         throw Error( "Cannot use a partial preview as normalization reference: " + p_referencePathOrViewId );
      if ( referenceView == view )
         throw Error( "The normalization reference and target images must be different" );
   }

   AutoViewLock referenceLock( referenceView, false/*lock*/ );
   if ( !referenceView.IsNull() )
      referenceLock.LockForWrite();

   console.EnableAbort();

   ImageVariant referenceImage;
   if ( p_referenceIsView )
   {
      console.WriteLn( "<end><cbr><br>Reference view: " + p_referencePathOrViewId );
      referenceImage = referenceView.Image();
   }
   else
      LoadImage( referenceImage, p_referencePathOrViewId, p_inputHints, "reference" );

   if ( Min( referenceImage.Width(), referenceImage.Height() ) < 2*p_scale )
      throw Error( "Insufficient image dimensions for the selected normalization scale: " + p_referencePathOrViewId );

   if ( Min( referenceImage.Width(), referenceImage.Height() ) < 256 )
      throw Error( "Image too small for local normalization; at least 256 pixels are required." );

   AutoViewLock targetLock( view, false/*lock*/ );
   if ( !view.IsPreview() || referenceView.IsNull() || view.Window() != referenceView.Window() )
      if ( IsHistoryUpdater( view ) )
         targetLock.Lock();
      else
         targetLock.LockForWrite();

   ImageVariant targetImage = view.Image();

   if ( targetImage.Bounds() != referenceImage.Bounds() || targetImage.IsColor() != referenceImage.IsColor() )
      throw Error( "Incompatible image geometry: " + view.FullId() );

   Image R, RB;
   UInt8Image RM, RS;
   Vector Rc;
   Array<PSFScaleEstimator> S;
   ImageVariant rejectionImage;
   if ( p_rejection && p_referenceRejection )
      rejectionImage.CreateFloatImage().CopyImage( targetImage );
   LocalNormalizationThread( *this, R, RB, RM, RS, Rc, S ).ProcessReferenceImage( R, RB, RM, RS, Rc, S, referenceImage, rejectionImage, true/*showImages*/ );
   if ( p_rejection && p_referenceRejection )
      rejectionImage.FreeImage();

   LocalNormalizationThread( *this, targetImage, R, RB, RM, RS, Rc, S ).Run();

   return true;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_referencePathOrViewId.IsEmpty() )
   {
      whyNot = "No reference image has been defined.";
      return false;
   }

   if ( p_targets.IsEmpty() )
   {
      whyNot = "No target images have been defined.";
      return false;
   }

   if ( p_generateNormalizedImages != LNGenerateNormalizedImages::Always )
      if ( p_generateNormalizedImages != LNGenerateNormalizedImages::GlobalExecutionOnly )
         if ( !p_generateNormalizationData )
         {
            whyNot = "No process operation has been defined applicable to global execution.";
            return false;
         }

   return true;
}

// ----------------------------------------------------------------------------

typedef IndirectArray<LocalNormalizationThread> thread_list;

bool LocalNormalizationInstance::ExecuteGlobal()
{
   Console console;

   o_output = Array<OutputData>( p_targets.Length() );

   {
      String why;
      if ( !CanExecuteGlobal( why ) )
         throw Error( why );

      if ( !p_outputDirectory.IsEmpty() )
         if ( !File::DirectoryExists( p_outputDirectory ) )
            throw Error( "The specified output directory does not exist: " + p_outputDirectory );

      StringList fileNames;
      for ( const auto& target : p_targets )
         if ( target.enabled )
         {
            if ( !File::Exists( target.path ) )
               throw Error( "No such file exists on the local filesystem: " + target.path );
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

   View referenceView = View::Null();
   if ( p_referenceIsView )
   {
      referenceView = View::ViewById( p_referencePathOrViewId );
      if ( referenceView.IsNull() )
         throw Error( "No such reference view: " + p_referencePathOrViewId );
      if ( referenceView.IsPreview() && referenceView.Bounds() != referenceView.Window().MainView().Bounds() )
         throw Error( "Cannot use a partial preview as normalization reference: " + p_referencePathOrViewId );
   }

   AutoViewLock referenceLock( referenceView, false/*lock*/ );
   if ( !referenceView.IsNull() )
      referenceLock.LockForWrite();

   ImageVariant referenceImage;
   int succeeded = 0;
   int failed = 0;
   int skipped = 0;

   try
   {
      console.EnableAbort();

      if ( p_referenceIsView )
      {
         console.WriteLn( "<end><cbr><br>Reference view: " + p_referencePathOrViewId );
         referenceImage = referenceView.Image();
      }
      else
         LoadImage( referenceImage, p_referencePathOrViewId, p_inputHints, "reference" );

      if ( Min( referenceImage.Width(), referenceImage.Height() ) < 2*p_scale )
         throw Error( "Insufficient image dimensions for the selected normalization scale: " + p_referencePathOrViewId );

      if ( Min( referenceImage.Width(), referenceImage.Height() ) < 256 )
         throw Error( "Image too small for local normalization; at least 256 pixels are required." );

      Image R, RB;
      UInt8Image RM, RS;
      Vector Rc;
      Array<PSFScaleEstimator> S;
      ImageVariant rejectionImage;
      if ( p_rejection && p_referenceRejection )
      {
         String path;
         for ( size_type i = 0; i < p_targets.Length(); ++i )
            if ( p_targets[i].enabled )
               if ( p_referenceIsView || !File::SameFile( p_targets[i].path, p_referencePathOrViewId ) )
               {
                  path = p_targets[i].path;
                  break;
               }
         if ( path.IsEmpty() )
            throw Error( "No file available for reference outlier rejection" );
         LoadImage( rejectionImage, path, p_inputHints, "reference outlier rejection" );
      }
      LocalNormalizationThread( *this, R, RB, RM, RS, Rc, S ).ProcessReferenceImage( R, RB, RM, RS, Rc, S, referenceImage, rejectionImage, false/*showImages*/ );
      if ( p_rejection && p_referenceRejection )
         rejectionImage.FreeImage();

      console.WriteLn( String().Format( "<end><cbr><br>Normalization of %u target files.", p_targets.Length() ) );

      Array<size_type> pendingItems;
      for ( size_type i = 0; i < p_targets.Length(); ++i )
         if ( p_targets[i].enabled )
            pendingItems << i;
         else
         {
            console.NoteLn( "* Skipping disabled target: " + p_targets[i].path );
            ++skipped;
         }

      size_type availableMemory = Module->AvailablePhysicalMemory();
      if ( availableMemory == 0 )
         console.WarningLn( "<end><cbr><br>** Warning: Unable to estimate the available physical memory." );
      else
         console.NoteLn( String().Format( "<end><cbr><br>* Available physical memory: %.3f GiB", availableMemory/1024/1024/1024.0 ) );

      if ( p_useFileThreads && pendingItems.Length() > 1 )
      {
         size_type worstCaseBytesPerThread = sizeof( float ) * 6 * referenceImage.NumberOfNominalSamples();
         console.NoteLn( String().Format( "<end><cbr>* Estimated per-thread memory allocation: %.3f GiB", worstCaseBytesPerThread/1024/1024/1024.0 ) );

         int maxThreads = Max( 1, TruncInt( double( p_autoMemoryLimit )*availableMemory / worstCaseBytesPerThread ) );
         int numberOfThreadsAvailable = RoundInt( Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 ) * p_fileThreadOverload );
         int numberOfThreadsShouldUse = Min( numberOfThreadsAvailable, int( pendingItems.Length() ) );
         int numberOfThreads = Min( maxThreads, numberOfThreadsShouldUse );
         thread_list runningThreads( numberOfThreads ); // N.B.: all pointers are set to nullptr by IndirectArray's ctor.
         console.NoteLn( String().Format( "<end><br>* Using %d worker threads%s.",
                                          numberOfThreads,
                                          (numberOfThreads < numberOfThreadsShouldUse) ? " (memory-limited)" : "" ) );
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
                  for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
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
                        if ( (*i)->Succeeded() )
                           (*i)->GetOutputData( o_output[(*i)->Index()] );

                        String errorInfo;
                        if ( !(*i)->Succeeded() )
                           errorInfo = (*i)->ErrorInfo();

                        /*
                         * Optional generation of invalid normalization data.
                         */
                        if ( (*i)->Invalid() )
                        {
                           ++failed;
                           --succeeded; // compensate ++succeeded below
                        }

                        /*
                         * N.B.: IndirectArray<>::Delete() sets to zero the
                         * pointer pointed to by the iterator, but does not
                         * remove the array element.
                         */
                        runningThreads.Delete( i );

                        if ( !errorInfo.IsEmpty() )
                           throw Error( errorInfo );

                        ++succeeded;
                     }

                     /*
                      * If there are items pending, create a new thread and
                      * fire the next one.
                      */
                     if ( !pendingItems.IsEmpty() )
                     {
                        *i = new LocalNormalizationThread( *this, *pendingItems, R, RB, RM, RS, Rc, S,
                                                           Max( 1, numberOfThreadsAvailable/numberOfThreads ) );
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
            for ( LocalNormalizationThread* thread : runningThreads )
               if ( thread != nullptr )
                  thread->Abort();
            for ( LocalNormalizationThread* thread : runningThreads )
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
               LocalNormalizationThread thread( *this, itemIndex, R, RB, RM, RS, Rc, S );
               thread.Run();
               thread.GetOutputData( o_output[itemIndex] );
               if ( thread.Invalid() )
                  ++failed;
               else
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
            throw Error( "No images were normalized: Empty target frames list? No enabled target frames?" );
         throw Error( "No image could be normalized." );
      }

      if ( !p_referenceIsView )
         referenceImage.Free();

      console.NoteLn( String().Format( "<end><cbr><br>===== LocalNormalization: %u succeeded, %u failed, %u skipped =====",
                                       succeeded, failed, skipped ) );
      return true;
   }
   catch ( ... )
   {
      if ( !p_referenceIsView )
         referenceImage.Free();
      throw;
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInstance::ApplyErrorPolicy()
{
   Console console;
   console.ResetStatus();
   console.EnableAbort();

   console.Note( "<end><cbr><br>* Applying error policy: " );

   switch ( p_onError )
   {
   default: // ?
   case LNOnError::Continue:
      console.NoteLn( "Continue on error." );
      break;

   case LNOnError::Abort:
      console.NoteLn( "Abort on error." );
      throw ProcessAborted();

   case LNOnError::AskUser:
      {
         console.NoteLn( "Ask on error..." );

         int r = MessageBox( "<p style=\"white-space:pre;\">"
                             "An error occurred during LocalNormalization execution. What do you want to do?</p>",
                             "LocalNormalization",
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

void* LocalNormalizationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheLNScaleParameter )
      return &p_scale;
   if ( p == TheLNNoScaleParameter )
      return &p_noScale;
   if ( p == TheLNGlobalLocationNormalizationParameter )
      return &p_globalLocationNormalization;
   if ( p == TheLNTruncateParameter )
      return &p_truncate;
   if ( p == TheLNBackgroundSamplingDeltaParameter )
      return &p_backgroundSamplingDelta;
   if ( p == TheLNRejectionParameter )
      return &p_rejection;
   if ( p == TheLNReferenceRejectionParameter )
      return &p_referenceRejection;
   if ( p == TheLNLowClippingLevelParameter )
      return &p_lowClippingLevel;
   if ( p == TheLNHighClippingLevelParameter )
      return &p_highClippingLevel;
   if ( p == TheLNReferenceRejectionThresholdParameter )
      return &p_referenceRejectionThreshold;
   if ( p == TheLNTargetRejectionThresholdParameter )
      return &p_targetRejectionThreshold;
   if ( p == TheLNHotPixelFilterRadiusParameter )
      return &p_hotPixelFilterRadius;
   if ( p == TheLNNoiseReductionFilterRadiusParameter )
      return &p_noiseReductionFilterRadius;
   if ( p == TheLNModelScalingFactorParameter )
      return &p_modelScalingFactor;
   if ( p == TheLNScaleEvaluationMethodParameter )
      return &p_scaleEvaluationMethod;
   if ( p == TheLNLocalScaleCorrectionsParameter )
      return &p_localScaleCorrections;
   if ( p == TheLNStructureLayersParameter )
      return &p_structureLayers;
   if ( p == TheLNSaturationThresholdParameter )
      return &p_saturationThreshold;
   if ( p == TheLNSaturationRelativeParameter )
      return &p_saturationRelative;
   if ( p == TheLNRejectionLimitParameter )
      return &p_rejectionLimit;
   if ( p == TheLNPSFNoiseLayersParameter )
      return &p_psfNoiseLayers;
   if ( p == TheLNPSFHotPixelFilterRadiusParameter )
      return &p_psfHotPixelFilterRadius;
   if ( p == TheLNPSFNoiseReductionFilterRadiusParameter )
      return &p_psfNoiseReductionFilterRadius;
   if ( p == TheLNPSFMinStructureSizeParameter )
      return &p_psfMinStructureSize;
   if ( p == TheLNPSFMinSNRParameter )
      return &p_psfMinSNR;
   if ( p == TheLNPSFAllowClusteredSourcesParameter )
      return &p_psfAllowClusteredSources;
   if ( p == TheLNPSFTypeParameter )
      return &p_psfType;
   if ( p == TheLNPSFGrowthParameter )
      return &p_psfGrowth;
   if ( p == TheLNPSFMaxStarsParameter )
      return &p_psfMaxStars;
   if ( p == TheLNReferencePathOrViewIdParameter )
      return p_referencePathOrViewId.Begin();
   if ( p == TheLNReferenceIsViewParameter )
      return &p_referenceIsView;
   if ( p == TheLNTargetEnabledParameter )
      return &p_targets[tableRow].enabled;
   if ( p == TheLNTargetImageParameter )
      return p_targets[tableRow].path.Begin();
   if ( p == TheLNInputHintsParameter )
      return p_inputHints.Begin();
   if ( p == TheLNOutputHintsParameter )
      return p_outputHints.Begin();
   if ( p == TheLNGenerateNormalizedImagesParameter )
      return &p_generateNormalizedImages;
   if ( p == TheLNGenerateNormalizationDataParameter )
      return &p_generateNormalizationData;
   if ( p == TheLNGenerateInvalidDataParameter )
      return &p_generateInvalidData;
   if ( p == TheLNShowBackgroundModelsParameter )
      return &p_showBackgroundModels;
   if ( p == TheLNShowLocalScaleModelsParameter )
      return &p_showLocalScaleModels;
   if ( p == TheLNShowRejectionMapsParameter )
      return &p_showRejectionMaps;
   if ( p == TheLNShowStructureMapsParameter )
      return &p_showStructureMaps;
   if ( p == TheLNPlotNormalizationFunctionsParameter )
      return &p_plotNormalizationFunctions;
   if ( p == TheLNNoGUIMessagesParameter )
      return &p_noGUIMessages;
   if ( p == TheLNAutoMemoryLimitParameter )
      return &p_autoMemoryLimit;
   if ( p == TheLNOutputDirectoryParameter )
      return p_outputDirectory.Begin();
   if ( p == TheLNOutputExtensionParameter )
      return p_outputExtension.Begin();
   if ( p == TheLNOutputPrefixParameter )
      return p_outputPrefix.Begin();
   if ( p == TheLNOutputPostfixParameter )
      return p_outputPostfix.begin();
   if ( p == TheLNOverwriteExistingFilesParameter )
      return &p_overwriteExistingFiles;
   if ( p == TheLNOnErrorParameter )
      return &p_onError;
   if ( p == TheLNUseFileThreadsParameter )
      return &p_useFileThreads;
   if ( p == TheLNFileThreadOverloadParameter )
      return &p_fileThreadOverload;
   if ( p == TheLNMaxFileReadThreadsParameter )
      return &p_maxFileReadThreads;
   if ( p == TheLNMaxFileWriteThreadsParameter )
      return &p_maxFileWriteThreads;
   if ( p == TheLNGraphSizeParameter )
      return &p_graphSize;
   if ( p == TheLNGraphTextSizeParameter )
      return &p_graphTextSize;
   if ( p == TheLNGraphTitleSizeParameter )
      return &p_graphTitleSize;
   if ( p == TheLNGraphTransparentParameter )
      return &p_graphTransparent;
   if ( p == TheLNGraphOutputDirectoryParameter )
      return p_graphOutputDirectory.Begin();

   if ( p == TheLNOutputFilePathXNMLParameter )
      return o_output[tableRow].outputFilePathXNML.Begin();
   if ( p == TheLNOutputFilePathParameter )
      return o_output[tableRow].outputFilePath.Begin();
   if ( p == TheLNScaleFactorRKParameter )
      return o_output[tableRow].scaleFactors.At( 0 );
   if ( p == TheLNScaleFactorGParameter )
      return o_output[tableRow].scaleFactors.At( 1 );
   if ( p == TheLNScaleFactorBParameter )
      return o_output[tableRow].scaleFactors.At( 2 );
   if ( p == TheLNValidParameter )
      return &o_output[tableRow].valid;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheLNReferencePathOrViewIdParameter )
   {
      p_referencePathOrViewId.Clear();
      if ( sizeOrLength > 0 )
         p_referencePathOrViewId.SetLength( sizeOrLength );
   }
   else if ( p == TheLNTargetItemsParameter )
   {
      p_targets.Clear();
      if ( sizeOrLength > 0 )
         p_targets.Add( Item(), sizeOrLength );
   }
   else if ( p == TheLNTargetImageParameter )
   {
      p_targets[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_targets[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheLNInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputHintsParameter )
   {
      p_outputHints.Clear();
      if ( sizeOrLength > 0 )
         p_outputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputDirectoryParameter )
   {
      p_outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_outputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputExtensionParameter )
   {
      p_outputExtension.Clear();
      if ( sizeOrLength > 0 )
         p_outputExtension.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputPrefixParameter )
   {
      p_outputPrefix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPrefix.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputPostfixParameter )
   {
      p_outputPostfix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPostfix.SetLength( sizeOrLength );
   }
   else if ( p == TheLNGraphOutputDirectoryParameter )
   {
      p_graphOutputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_graphOutputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputDataParameter )
   {
      o_output.Clear();
      if ( sizeOrLength > 0 )
         o_output.Add( OutputData(), sizeOrLength );
   }
   else if ( p == TheLNOutputFilePathXNMLParameter )
   {
      o_output[tableRow].outputFilePathXNML.Clear();
      if ( sizeOrLength > 0 )
         o_output[tableRow].outputFilePathXNML.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputFilePathParameter )
   {
      o_output[tableRow].outputFilePath.Clear();
      if ( sizeOrLength > 0 )
         o_output[tableRow].outputFilePath.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type LocalNormalizationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheLNReferencePathOrViewIdParameter )
      return p_referencePathOrViewId.Length();
   if ( p == TheLNTargetItemsParameter )
      return p_targets.Length();
   if ( p == TheLNTargetImageParameter )
      return p_targets[tableRow].path.Length();
   if ( p == TheLNInputHintsParameter )
      return p_inputHints.Length();
   if ( p == TheLNOutputHintsParameter )
      return p_outputHints.Length();
   if ( p == TheLNOutputDirectoryParameter )
      return p_outputDirectory.Length();
   if ( p == TheLNOutputExtensionParameter )
      return p_outputExtension.Length();
   if ( p == TheLNOutputPrefixParameter )
      return p_outputPrefix.Length();
   if ( p == TheLNOutputPostfixParameter )
      return p_outputPostfix.Length();
   if ( p == TheLNGraphOutputDirectoryParameter )
      return p_graphOutputDirectory.Length();
   if ( p == TheLNOutputDataParameter )
      return o_output.Length();
   if ( p == TheLNOutputFilePathXNMLParameter )
      return o_output[tableRow].outputFilePathXNML.Length();
   if ( p == TheLNOutputFilePathParameter )
      return o_output[tableRow].outputFilePath.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalNormalizationInstance.cpp - Released 2022-11-21T14:47:17Z
