//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.0
// ----------------------------------------------------------------------------
// Standard StarNet Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// StarNetInstance.cpp - Released 2020-08-17T12:19:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarNet PixInsight module.
//
// Copyright (c) 2018-2020 Nikita Misiura
//
// This software is available under Attribution-NonCommercial-ShareAlike 4.0
// International Creative Commons license (CC BY-NC-SA 4.0):
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//
// In short: You are free to use and redistribute the code in any medium or
// format, but only under the same license terms. You can transform and build
// your projects upon it. You can NOT use the code for commercial purposes. You
// must give appropriate credit for usage of the code.
//
// This product is based on software from the PixInsight project, developed by
// Pleiades Astrophoto and its contributors:
//
// https://pixinsight.com/
// ----------------------------------------------------------------------------

#include "StarNetInstance.h"
#include "StarNetParameters.h"
#include "StarNetProcess.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

#include <tensorflow/c/c_api.h>

namespace pcl
{

// ----------------------------------------------------------------------------

StarNetInstance::StarNetInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_stride( SNStride::Default )
   , p_mask( TheSNCreateMaskParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

StarNetInstance::StarNetInstance( const StarNetInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void StarNetInstance::Assign( const ProcessImplementation& p )
{
   const StarNetInstance* x = dynamic_cast<const StarNetInstance*>( &p );
   if ( x != nullptr )
   {
      p_stride = x->p_stride;
      p_mask = x->p_mask;
   }
}

// ----------------------------------------------------------------------------

bool StarNetInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   const ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
   {
      whyNot = "StarNet cannot be executed on complex images.";
      return false;
   }

   if ( image.Width() < 256 || image.Height() < 256 )
   {
      whyNot = "StarNet requires minimum image dimensions of 256 x 256 pixels.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class StarNetEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, View& view, const StarNetInstance& instance )
   {
      const int window = 256; // input tile size

      if ( !TheStarNetProcess->PreferencesLoaded() )
         TheStarNetProcess->LoadPreferences();

      String weightsFilePath = image.IsColor() ? TheStarNetProcess->RGBWeightsFilePath()
                                               : TheStarNetProcess->GrayscaleWeightsFilePath();
            //"rgb_starnet_weights.pb" : "mono_starnet_weights.pb" );
      if ( !File::Exists( weightsFilePath ) )
         throw Error( "Checkpoint file not found: " + weightsFilePath );

      // Getting some info about our image
      int imgNumChannels = image.NumberOfNominalChannels();
      int imgHeight = image.Height();
      int imgWidth = image.Width();
      int strideSize = Pow2I<int>()( 7 - instance.p_stride );

      /*
       * Starting augmentation of the image.
       *
       * Because we process image tile by tile of size 256 by 256 and then only
       * use central part of the final output of the size stride by stride,
       * there is a restriction on the size of the image we can process.
       *
       * Size of the image should be a multiple of stride plus some additional
       * margin of the size (256 - stride)/2 to allow to process the very edges
       * of an image. This proper size is called finalHeight by finalWidth and
       * is calculated below.
       */
      int border = (window - strideSize)/2;
      int finalWidth  = ((imgWidth % strideSize != 0)  ? (imgWidth/strideSize + 1)*strideSize : imgWidth) + 2*border;
      int finalHeight = ((imgHeight % strideSize != 0) ? (imgHeight/strideSize + 1)*strideSize : imgHeight) + 2*border;

      // Original image with added edges
      GenericImage<P> transformed( finalWidth, finalHeight, image.ColorSpace() );
      // Final image after star removal
      GenericImage<P> starless( finalWidth, finalHeight, image.ColorSpace() );

      /*
       * The whole bunch of loops below calculates pixel values for the added
       * edges we fill them with mirrored edges of the original image.
       */
      int left = border;
      int top = border;
      int right = finalWidth - imgWidth - left;
      int bottom = finalHeight - imgHeight - border;
      for ( int c = 0; c < imgNumChannels; ++c )
      {
         for ( int i = 0; i < imgWidth; ++i )
            for ( int j = 0; j < imgHeight; ++j )
               transformed( i + left, j + top, c ) = image( i, j, c );

         for ( int i = 0; i < left; ++i )
            for ( int j = 0; j < imgHeight; ++j )
               transformed( left - i - 1, j + top, c ) = transformed( left + i, j + top, c );

         for ( int i = 0; i < right; ++i )
            for ( int j = 0; j < imgHeight; ++j )
               transformed( finalWidth - right + i, j + top, c ) = transformed( finalWidth - right - i - 1, j + top, c );

         for ( int i = 0; i < finalWidth; ++i )
            for ( int j = 0; j < top; ++j )
               transformed( i, top - j - 1, c ) = transformed( i, top + j, c );

         for ( int i = 0; i < finalWidth; ++i )
            for ( int j = 0; j < bottom; ++j )
               transformed( i, finalHeight - bottom + j, c ) = transformed( i, finalHeight - bottom - j - 1, c );
      }

      /*
       * Load the appropriate model for the color space of the image.
       */
      Console().WriteLn( "<end><cbr>Restoring neural network checkpoint: <raw>" + weightsFilePath + "</raw>" );
      TF_Graph* graph = LoadGraphDef( weightsFilePath );

      /*
       * Tensorflow stuff.
       */
      TF_Output inputOp = { TF_GraphOperationByName( graph, "X" ), 0 };
      // Input dimensions.
      const int64_t inputDims[ 4 ] = { 1, window, window, imgNumChannels };

      // Creating all necessary tensors and session for TF.
      TF_SessionOptions* options = TF_NewSessionOptions();
      TF_Status* status = TF_NewStatus();
      TF_Output outOp = { TF_GraphOperationByName( graph, "generator/g_deconv7/Sub" ), 0 };
      TF_Session* sess = TF_NewSession( graph, options, status );

#define CLEANUP()    TF_CloseSession( sess, status );    \
                     TF_DeleteSession( sess, status );   \
                     TF_DeleteSessionOptions( options ); \
                     TF_DeleteGraph( graph );            \
                     TF_DeleteStatus( status );

      size_type numberOfTiles = size_type( (finalHeight - window)/strideSize + 1 )
                              * size_type( (finalWidth - window)/strideSize + 1 );
      Console().EnableAbort();
      StandardStatus callback;
      StatusMonitor monitor;
      monitor.SetCallback( &callback );
      monitor.Initialize( String().Format( "Processing %u image tiles", numberOfTiles ), numberOfTiles );

      /*
       * This is the main loop - it goes through the whole image tile by tile
       * and processes it.
       */
      try
      {
         for ( int i = 0; i <= finalWidth - window; i += strideSize )
            for ( int j = 0; j <= finalHeight - window; j += strideSize, ++monitor )
            {
               /*
                * Filling the input vector for TF.
                *
                * Rescaling the values - at the moment RGB model works with
                * samples in the range [-1, 1] and grayscale model works with
                * samples with zero mean and standard deviation of one. This
                * will be changed in the future; I am plannning to make both
                * models works with samples in [-1, 1] range.
                */
               Array<float> inputVals;
               for ( int w = i; w < i + window; ++w )
                  for ( int h = j; h < j + window; ++h )
                     for ( int c = 0; c < imgNumChannels; ++c )
                     {
                        float f; P::FromSample( f, transformed( w, h, c ) );
                        inputVals << (image.IsColor() ? f+f - 1 : f);
                     }

               /*
                * Calculation of standard deviation and mean for grayscale
                * model. I am plannning to make Grayscale model to work with
                * samples rescaled to [-1, 1] instead, so this is likely to be
                * removed in the future.
                */
               double mean = Mean( inputVals.Begin(), inputVals.End() );
               double stdev = StdDev( inputVals.Begin(), inputVals.End(), mean );

               if ( 1 + stdev == 1 )
                  stdev = 1;
               if ( !image.IsColor() )
                  for ( float& x : inputVals )
                     x = (x - mean)/stdev;

               // load values into tensor
               TF_Tensor* inputTensor = CreateTensor( TF_FLOAT, inputDims, 4, inputVals.Begin(), inputVals.Size() );
               TF_Tensor* outputTensor = nullptr;

               // Run TF to get output
               TF_SessionRun( sess, nullptr,                 // run options
                              &inputOp, &inputTensor, 1,     // input tensors, input tensor values, number of inputs
                              &outOp, &outputTensor, 1,      // output tensors, output tensor values, number of outputs
                              nullptr, 0, nullptr, status ); // target operations, number of targets, run metadata, output status

               // Output data
               float* data = reinterpret_cast<float*>( TF_TensorData( outputTensor ) );

               /*
                * Basically, just a primitive loop to load vector of values
                * into pixels and rescale them back.
                */
               for ( int w = border; w < window - border; ++w )
                  for ( int h = border; h < window - border; ++h )
                     for ( int c = 0; c < imgNumChannels; ++c )
                     {
                        float v = data[c + imgNumChannels*(w*window + h)];
                        v = image.IsColor() ? (v + 1)/2 : v*stdev + mean;
                        starless( i + w, j + h, c ) = P::ToSample( Range( v, .0F, 1.F ) );
                     }

               TF_DeleteTensor( inputTensor );
               TF_DeleteTensor( outputTensor );
            }

         /*
          * Crop the image back to its original size.
          * If we want a mask, return the difference between the initial and
          * output images.
          */
         if ( instance.p_mask )
         {
            GenericImage<P> mask( finalWidth, finalHeight, image.ColorSpace() );
            mask = transformed - starless;
            image = mask.CropBy( -left, -top, -right, -bottom );
         }
         else
            image = starless.CropBy( -left, -top, -right, -bottom );

         /*
          * Some cleaning.
          */
         CLEANUP()
      }
      catch ( ... )
      {
         CLEANUP()
         throw;
      }

#undef CLEANUP
   }

private:

   static TF_Tensor* CreateTensor( TF_DataType dataType, const int64_t* dims, int num_dims, const void* data, size_type size )
   {
      TF_Tensor* tensor = TF_AllocateTensor( dataType, dims, num_dims, size );
      if ( tensor == nullptr )
         throw Error( "StarNetEngine::CreateTensor(): Failure to allocate tensor." );
      void* tensorData = TF_TensorData( tensor );
      if ( tensorData == nullptr )
      {
         TF_DeleteTensor( tensor );
         throw Error( "StarNetEngine::CreateTensor(): No tensor data available." );
      }
      ::memcpy( tensorData, data, Min( size, TF_TensorByteSize( tensor ) ) );
      return tensor;
   }

   static TF_Graph* LoadGraphDef( const String& filePath )
   {
      TF_Buffer* buffer = ReadBufferFromFile( filePath );
      TF_Graph* graph = TF_NewGraph();
      TF_Status* status = TF_NewStatus();
      TF_ImportGraphDefOptions* options = TF_NewImportGraphDefOptions();
      TF_GraphImportGraphDef( graph, buffer, options, status );
      TF_DeleteImportGraphDefOptions( options );
      TF_DeleteBuffer( buffer );

      if ( TF_GetCode( status ) != TF_OK )
      {
         TF_DeleteGraph( graph );
         throw Error( "StarNetEngine::LoadGraphDef(): Unable to load graph definition." );
      }

      TF_DeleteStatus( status );

      return graph;
   }

   static TF_Buffer* ReadBufferFromFile( const String& filePath )
   {
      ByteArray data = File::ReadFile( filePath );
      TF_Buffer* buffer = TF_NewBuffer();
      buffer->length = data.Size();
      buffer->data = data.Release();
      buffer->data_deallocator = DeallocateBuffer;
      return buffer;
   }

   static void DeallocateBuffer( void* data, size_t )
   {
      StandardAllocator().DeallocateBlock( data );
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool StarNetInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();
   if ( image.IsComplexSample() )
      return false;

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32:
         StarNetEngine::Apply( static_cast<Image&>( *image ), view, *this );
         break;
      case 64:
         StarNetEngine::Apply( static_cast<DImage&>( *image ), view, *this );
         break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case 8:
         StarNetEngine::Apply( static_cast<UInt8Image&>( *image ), view, *this );
         break;
      case 16:
         StarNetEngine::Apply( static_cast<UInt16Image&>( *image ), view, *this );
         break;
      case 32:
         StarNetEngine::Apply( static_cast<UInt32Image&>( *image ), view, *this );
         break;
      }

   return true;
}

// ----------------------------------------------------------------------------

void* StarNetInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheSNStrideParameter )
      return &p_stride;
   if ( p == TheSNCreateMaskParameter )
      return &p_mask;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool StarNetInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   return true;
}

// ----------------------------------------------------------------------------

size_type StarNetInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF StarNetInstance.cpp - Released 2020-08-17T12:19:56Z
