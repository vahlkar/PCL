//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard StarNet Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// StarNetInstance.cpp - Released 2021-04-09T19:41:49Z
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
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

#include <tensorflow/c/c_api.h>

#define INPUT_TILE_SIZE 256

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
      if ( !TheStarNetProcess->PreferencesLoaded() )
         TheStarNetProcess->LoadPreferences();

      String weightsFilePath = image.IsColor() ? TheStarNetProcess->RGBWeightsFilePath()
                                               : TheStarNetProcess->GrayscaleWeightsFilePath();
      if ( !File::Exists( weightsFilePath ) )
         throw Error( "Checkpoint file not found: " + weightsFilePath );

      // Getting some info about our image
      int imgNumChannels = image.NumberOfNominalChannels();
      int imgHeight = image.Height();
      int imgWidth = image.Width();
      int strideSize = Pow2I<int>()( 7 - instance.p_stride );

      AutoViewLock lock( view, false/*lock*/ );
      lock.LockForWrite();

      Console console;

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
      int border = (INPUT_TILE_SIZE - strideSize)/2;
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
      console.WriteLn( "<end><cbr>Restoring neural network checkpoint: <raw>" + weightsFilePath + "</raw>" );
      TF_Graph* graph = LoadGraphDef( weightsFilePath );

      int stridesPerRow = (finalWidth - INPUT_TILE_SIZE)/strideSize + 1;
      int numberOfTiles = ((finalHeight - INPUT_TILE_SIZE)/strideSize + 1) * stridesPerRow;
      console.EnableAbort();
      StandardStatus callback;
      StatusMonitor monitor;
      monitor.SetCallback( &callback );
      monitor.Initialize( String().Format( "Processing %d image tiles", numberOfTiles ), numberOfTiles );

      /*
       * ### N.B.
       *
       * The commented code below (and the corresponding thread code after this
       * function) is an attempt to parallelize tile processing.
       *
       * This code is kept here for reference but not used, since TensorFlow
       * already runs parallelized and we cannot improve its execution times
       * for now.
       */
//       Array<size_type> L = Thread::OptimalThreadLoads( numberOfTiles, 1/*overheadLimit*/ );
//       AbstractImage::ThreadData data( monitor, numberOfTiles );
//       ReferenceArray<WorkerThread<P>> threads;
//       for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
//          threads << new WorkerThread<P>( data, transformed, starless, graph, strideSize, n, n + int( L[i] ) - 1 );
//       AbstractImage::RunThreads( threads, data );
//       threads.Destroy();
//
//       TF_DeleteGraph( graph );
//
//       /*
//        * Crop the image back to its original size.
//        * If we want a mask, return the difference between the initial and
//        * output images.
//        */
//       if ( instance.p_mask )
//       {
//          GenericImage<P> mask( finalWidth, finalHeight, image.ColorSpace() );
//          mask = transformed - starless;
//          image = mask.CropBy( -left, -top, -right, -bottom );
//       }
//       else
//          image = starless.CropBy( -left, -top, -right, -bottom );

      /*
       * Tensorflow stuff.
       */
      // Operations
      TF_Output inputOp = { TF_GraphOperationByName( graph, "X" ), 0 };
      TF_Output outOp = { TF_GraphOperationByName( graph, "generator/g_deconv7/Sub" ), 0 };
      // Input dimensions
      const int64_t inputDims[ 4 ] = { 1, INPUT_TILE_SIZE, INPUT_TILE_SIZE, imgNumChannels };
      // Creating all necessary tensors and session for TF.
      TF_SessionOptions* options = TF_NewSessionOptions();
      TF_Status* status = TF_NewStatus();
      TF_Session* sess = TF_NewSession( graph, options, status );

#define CLEANUP()    TF_CloseSession( sess, status );    \
                     TF_DeleteSession( sess, status );   \
                     TF_DeleteSessionOptions( options ); \
                     TF_DeleteGraph( graph );            \
                     TF_DeleteStatus( status );
      try
      {
         console.EnableAbort();

         /*
          * This is the main loop - it goes through the whole image tile by
          * tile and processes it.
          */
         for ( int tile = 0; tile < numberOfTiles; ++tile, ++monitor )
         {
            /*
             * Image coordinates of the top left corner of the current tile.
             */
            int i = (tile % stridesPerRow) * strideSize;
            int j = (tile / stridesPerRow) * strideSize;

            /*
             * Filling the input vector for TF.
             *
             * Rescaling the values - at the moment RGB model works with
             * samples in the range [-1, 1] and grayscale model works with
             * samples with zero mean and standard deviation of one.
             *
             * This will be changed in the future; I am plannning to make both
             * models works with samples in [-1, 1] range.
             */
            Array<float> inputVals;
            for ( int w = i; w < i + INPUT_TILE_SIZE; ++w )
               for ( int h = j; h < j + INPUT_TILE_SIZE; ++h )
                  for ( int c = 0; c < imgNumChannels; ++c )
                  {
                     float f; P::FromSample( f, transformed( w, h, c ) );
                     inputVals << (image.IsColor() ? f+f - 1 : f);
                  }

            /*
             * Calculation of standard deviation and mean for grayscale model.
             * I am plannning to make Grayscale model to work with samples
             * rescaled to [-1, 1] instead, so this is likely to be removed in
             * the future.
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
             * Load vector of values into pixels and rescale them back.
             */
            for ( int w = border; w < INPUT_TILE_SIZE - border; ++w )
               for ( int h = border; h < INPUT_TILE_SIZE - border; ++h )
                  for ( int c = 0; c < imgNumChannels; ++c )
                  {
                     float v = data[c + imgNumChannels*(w*INPUT_TILE_SIZE + h)];
                     v = image.IsColor() ? (v + 1)/2 : v*stdev + mean;
                     starless( i + w, j + h, c ) = P::ToSample( Range( v, .0F, 1.F ) );
                  }

            TF_DeleteTensor( inputTensor );
            TF_DeleteTensor( outputTensor );
         } // for each tile

         /*
          * Crop the image back to its original size.
          */
         starless.CropBy( -left, -top, -right, -bottom );

         /*
          * If we want a mask, return the difference between the initial and
          * output images.
          */
         if ( instance.p_mask )
         {
            ImageWindow maskWindow( imgWidth, imgHeight, imgNumChannels,
                                    P::BitsPerSample(),
                                    P::IsFloatSample(),
                                    image.IsColor(),
                                    true/*initialProcessing*/,
                                    "star_mask" );
            if ( maskWindow.IsNull() )
               throw Error( "Unable to create image window" );

            View view = maskWindow.MainView();
            AutoViewLock lock( view );
            ImageVariant v = view.Image();
            GenericImage<P>& mask = static_cast<GenericImage<P>&>( *v );
            for ( int c = 0; c < imgNumChannels; ++c )
            {
               typename GenericImage<P>::const_sample_iterator i( image, c );
               typename GenericImage<P>::const_sample_iterator s( starless, c );
               for ( typename GenericImage<P>::sample_iterator m( mask, c ); m; ++m, ++i, ++s )
                  *m = P::FloatToSample( Max( 0.0, double( *i ) - double( *s ) ) );
            }
            lock.Unlock();
            maskWindow.Show();
         }

         /*
          * Replace the target image with the starless image.
          */
         lock.Lock();
         image = starless;

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

//    template <class P>
//    class WorkerThread : public Thread
//    {
//    public:
//
//       WorkerThread( const AbstractImage::ThreadData& data,
//                     const GenericImage<P>& image, GenericImage<P>& starless,
//                     TF_Graph* graph, int strideSize, int firstTile, int lastTile )
//          : m_data( data )
//          , m_image( image )
//          , m_starless( starless )
//          , m_graph( graph )
//          , m_strideSize( strideSize )
//          , m_firstTile( firstTile )
//          , m_lastTile( lastTile )
//       {
//       }
//
//       void Run() override
//       {
//          INIT_THREAD_MONITOR()
//
//          // Input dimensions.
//          int imgNumChannels = m_image.NumberOfNominalChannels();
//          const int64_t inputDims[ 4 ] = { 1, INPUT_TILE_SIZE, INPUT_TILE_SIZE, imgNumChannels };
//
//          // Creating all necessary tensors and session for TF.
//          TF_Output inputOp = { TF_GraphOperationByName( m_graph, "X" ), 0 };
//          TF_Output outOp = { TF_GraphOperationByName( m_graph, "generator/g_deconv7/Sub" ), 0 };
//          TF_SessionOptions* options = TF_NewSessionOptions();
//          TF_Status* status = TF_NewStatus();
//          TF_Session* sess = TF_NewSession( m_graph, options, status );
//
/* #define CLEANUP()    TF_CloseSession( sess, status );    \
//                      TF_DeleteSession( sess, status );   \
//                      TF_DeleteSessionOptions( options ); \
//                      TF_DeleteStatus( status ); */
//          try
//          {
//             int stridesPerRow = (m_image.Width() - INPUT_TILE_SIZE)/m_strideSize + 1;
//
//             for ( int tile = m_firstTile; tile <= m_lastTile; ++tile )
//             {
//                int i = (tile % stridesPerRow) * m_strideSize;
//                int j = (tile / stridesPerRow) * m_strideSize;
//
//                /*
//                 * Filling the input vector for TF.
//                 *
//                 * Rescaling the values - at the moment RGB model works with
//                 * samples in the range [-1, 1] and grayscale model works with
//                 * samples with zero mean and standard deviation of one.
//                 *
//                 * This will be changed in the future; I am plannning to make
//                 * both models works with samples in [-1, 1] range.
//                 */
//                Array<float> inputVals;
//                for ( int w = i; w < i + INPUT_TILE_SIZE; ++w )
//                   for ( int h = j; h < j + INPUT_TILE_SIZE; ++h )
//                      for ( int c = 0; c < imgNumChannels; ++c )
//                      {
//                         float f; P::FromSample( f, m_image( w, h, c ) );
//                         inputVals << (m_image.IsColor() ? f+f - 1 : f);
//                      }
//
//                /*
//                 * Calculation of standard deviation and mean for grayscale
//                 * model. I am plannning to make grayscale model to work with
//                 * samples rescaled to [-1, 1] instead, so this is likely to be
//                 * removed in the future.
//                 */
//                double mean = Mean( inputVals.Begin(), inputVals.End() );
//                double stdev = StdDev( inputVals.Begin(), inputVals.End(), mean );
//
//                if ( 1 + stdev == 1 )
//                   stdev = 1;
//                if ( !m_image.IsColor() )
//                   for ( float& x : inputVals )
//                      x = (x - mean)/stdev;
//
//                // load values into tensor
//                TF_Tensor* inputTensor = CreateTensor( TF_FLOAT, inputDims, 4, inputVals.Begin(), inputVals.Size() );
//                TF_Tensor* outputTensor = nullptr;
//
//                // Run TF to get output
//                TF_SessionRun( sess, nullptr,                 // run options
//                               &inputOp, &inputTensor, 1,     // input tensors, input tensor values, number of inputs
//                               &outOp, &outputTensor, 1,      // output tensors, output tensor values, number of outputs
//                               nullptr, 0, nullptr, status ); // target operations, number of targets, run metadata, output status
//
//                // Output data
//                float* data = reinterpret_cast<float*>( TF_TensorData( outputTensor ) );
//
//                /*
//                 * Load vector of values into pixels and rescale them back.
//                 */
//                for ( int border = (INPUT_TILE_SIZE - m_strideSize)/2, w = border; w < INPUT_TILE_SIZE - border; ++w )
//                   for ( int h = border; h < INPUT_TILE_SIZE - border; ++h )
//                      for ( int c = 0; c < imgNumChannels; ++c )
//                      {
//                         float v = data[c + imgNumChannels*(w*INPUT_TILE_SIZE + h)];
//                         v = m_image.IsColor() ? (v + 1)/2 : v*stdev + mean;
//                         m_starless( i + w, j + h, c ) = P::ToSample( Range( v, .0F, 1.F ) );
//                      }
//
//                TF_DeleteTensor( inputTensor );
//                TF_DeleteTensor( outputTensor );
//
//                UPDATE_THREAD_MONITOR( 1 )
//             }
//
//             CLEANUP()
//             m_success = true;
//          }
//          catch ( ... )
//          {
//             CLEANUP()
//          }
//       }
//
//    private:
//
//       const AbstractImage::ThreadData& m_data;
//       const GenericImage<P>&           m_image;
//             GenericImage<P>&           m_starless;
//             TF_Graph*                  m_graph;
//             int                        m_strideSize;
//             int                        m_firstTile;
//             int                        m_lastTile;
//             bool                       m_success = false;
//    };

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
   ImageVariant image = view.Image();
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
// EOF StarNetInstance.cpp - Released 2021-04-09T19:41:49Z
