//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ExponentialTransformationInstance.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2024 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "ExponentialTransformationInstance.h"

#include <pcl/AutoPointer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/FFTConvolution.h>
#include <pcl/GaussianFilter.h>
#include <pcl/ReferenceArray.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/StandardStatus.h>
#include <pcl/Thread.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ExponentialTransformationInstance::ExponentialTransformationInstance( const MetaProcess* P )
   : ProcessImplementation( P )
   , p_type( TheExponentialFunctionTypeParameter->ElementValue( TheExponentialFunctionTypeParameter->DefaultValueIndex() ) )
   , p_order( TheExponentialFunctionOrderParameter->DefaultValue() )
   , p_sigma( TheExponentialFunctionSmoothingParameter->DefaultValue() )
   , p_useLightnessMask( TheExponentialFunctionMaskParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

ExponentialTransformationInstance::ExponentialTransformationInstance( const ExponentialTransformationInstance& p )
   : ProcessImplementation( p )
{
   Assign( p );
}

// ----------------------------------------------------------------------------

void ExponentialTransformationInstance::Assign( const ProcessImplementation& p )
{
   const ExponentialTransformationInstance* x = dynamic_cast<const ExponentialTransformationInstance*>( &p );
   if ( x != nullptr )
   {
      p_type = x->p_type;
      p_order = x->p_order;
      p_sigma = x->p_sigma;
      p_useLightnessMask = x->p_useLightnessMask;
   }
}

// ----------------------------------------------------------------------------

UndoFlags ExponentialTransformationInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool ExponentialTransformationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "ExponentialTransformation cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class ExponentialTransformationEngine
{
public:

   static void Apply( ImageVariant& image, const ExponentialTransformationInstance& instance )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: Apply( static_cast<Image&>( *image ), instance ); break;
         case 64: Apply( static_cast<DImage&>( *image ), instance ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: Apply( static_cast<UInt8Image&>( *image ), instance ); break;
         case 16: Apply( static_cast<UInt16Image&>( *image ), instance ); break;
         case 32: Apply( static_cast<UInt32Image&>( *image ), instance ); break;
         }
   }

   template <class P>
   static void Apply( GenericImage<P>& image, const ExponentialTransformationInstance& instance )
   {
      AutoPointer<Image> mask;

      if ( instance.p_sigma > 0 )
      {
         image.ResetSelections();
         image.SelectNominalChannels();
         mask = new Image( image );
         mask->Status().Initialize( "Generating smoothing mask", mask->NumberOfSamples() );
         mask->Status().DisableInitialization();
         mask->SelectNominalChannels();
         GaussianFilter G( instance.p_sigma );
         if ( G.Size() < FFTConvolution::FasterThanSeparableFilterSize( Thread::NumberOfThreads( PCL_MAX_PROCESSORS ) ) )
            SeparableConvolution( G.AsSeparableFilter() ) >> *mask;
         else
            FFTConvolution( G ) >> *mask;
      }

      size_type N = image.NumberOfPixels();

      image.Status().Initialize( instance.TypeAsString() + " transformation", image.NumberOfNominalSamples() );

      ThreadData data( image, image.NumberOfNominalSamples() );
      if ( instance.p_order == 1 )
         data.iorder = 1;
      else if ( instance.p_order == 2 )
         data.iorder = 2;
      else if ( instance.p_order == 3 )
         data.iorder = 3;
      else if ( instance.p_order == 4 )
         data.iorder = 4;
      else if ( instance.p_order == 5 )
         data.iorder = 5;
      else if ( instance.p_order == 6 )
         data.iorder = 6;
      else if ( instance.p_order == 0.5 )
         data.iorder = -1;
      else
         data.iorder = 0;

      Array<size_type> L = pcl::Thread::OptimalThreadLoads( N, 16/*overheadLimit*/ );
      ReferenceArray<ExponentialThread<P>> threads;
      for ( size_type i = 0, n = 0; i < L.Length(); n += L[i++] )
         threads.Add( new ExponentialThread<P>( instance, data, image, mask, n, n + L[i] ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      image.Status() = data.status;
   }

private:

   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( const AbstractImage& image, size_type count )
         : AbstractImage::ThreadData( image, count )
      {
      }

      int iorder;
   };

   template <class P>
   class ExponentialThread : public Thread
   {
   public:

      ExponentialThread( const ExponentialTransformationInstance& instance,
                         const ThreadData& data,
                         GenericImage<P>& image,
                         const Image* mask,
                         size_type start, size_type end )
         : m_instance( instance )
         , m_data( data )
         , m_image( image )
         , m_mask( mask )
         , m_start( start )
         , m_end( end )
      {
      }

      void Run() override
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         int nc = m_image.NumberOfNominalChannels();
         typename P::sample* f[ 3 ];
         const float*        m[ 3 ];
         for ( int c = 0; c < nc; ++c )
         {
            f[c] = m_image[c] + m_start;
            m[c] = (m_mask != nullptr) ? (*m_mask)[c] + m_start : nullptr;
         }
         typename P::sample* fN = m_image[0] + m_end;

         for ( ; f[0] < fN; )
         {
            double L = 0, L1 = 0;
            if ( m_instance.p_useLightnessMask )
            {
               if ( nc == 1 )
                  P::FromSample( L, *f[0] );
               else
               {
                  RGBColorSystem::sample R, G, B;
                  P::FromSample( R, *f[0] );
                  P::FromSample( G, *f[1] );
                  P::FromSample( B, *f[2] );
                  L = rgbws.Lightness( R, G, B );
               }
               L1 = 1 - L;
            }

            for ( int c = 0; c < nc; ++c )
            {
               double f0;
               P::FromSample( f0, *f[c] );

               double fm;
               if ( m_mask != nullptr )
                  fm = *(m[c])++;
               else
                  fm = f0;
               fm = 1 - fm;
               switch ( m_data.iorder )
               {
               case 6:
                  fm = fm*fm*fm;
                  fm *= fm;
                  break;
               case 5:
                  {
                     double fm1 = fm;
                     fm = fm*fm;
                     fm *= fm;
                     fm *= fm1;
                  }
                  break;
               case 4:
                  fm = fm*fm;
                  fm *= fm;
                  break;
               case 3:
                  fm = fm*fm*fm;
                  break;
               case 2:
                  fm *= fm;
                  break;
               case 1:
                  break;
               case -1:
                  fm = Sqrt( fm );
                  break;
               default:
                  fm = Pow( fm, double( m_instance.p_order ) );
                  break;
               }

               double f1;
               switch ( m_instance.p_type )
               {
               default:
               case ExponentialFunctionType::PIP: f1 = Pow( f0, fm ); break;
               case ExponentialFunctionType::SMI: f1 = 1 - (1 - f0)*fm; break;
               }

               if ( m_instance.p_useLightnessMask )
                  f1 = L1*f1 + L*f0;

               *(f[c])++ = P::ToSample( f1 );

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

      const ExponentialTransformationInstance& m_instance;
      const ThreadData&                        m_data;
      GenericImage<P>&                         m_image;
      const Image*                             m_mask;
      size_type                                m_start;
      size_type                                m_end;
   };
};

// ----------------------------------------------------------------------------

void ExponentialTransformationInstance::Preview( UInt16Image& image ) const
{
   ExponentialTransformationEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

bool ExponentialTransformationInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   ExponentialTransformationEngine::Apply( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

void* ExponentialTransformationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheExponentialFunctionTypeParameter )
      return &p_type;
   if ( p == TheExponentialFunctionOrderParameter )
      return &p_order;
   if ( p == TheExponentialFunctionSmoothingParameter )
      return &p_sigma;
   if ( p == TheExponentialFunctionMaskParameter )
      return &p_useLightnessMask;

   return nullptr;
}

// ----------------------------------------------------------------------------

String ExponentialTransformationInstance::TypeAsString() const
{
   switch ( p_type )
   {
   case ExponentialFunctionType::PIP: return "PIP";
   case ExponentialFunctionType::SMI: return "SMI";
   }

   return String();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExponentialTransformationInstance.cpp - Released 2024-12-28T16:54:15Z
