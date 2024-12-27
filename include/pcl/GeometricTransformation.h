//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/GeometricTransformation.h - Released 2024-12-27T18:16:01Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_GeometricTransformation_h
#define __PCL_GeometricTransformation_h

/// \file pcl/GeometricTransformation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ImageTransformation.h>
#include <pcl/PixelInterpolation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class GeometricTransformation
 * \brief Abstract base class of all PCL geometric image transformations.
 *
 * %GeometricTransformation represents a geometrical transformation that can be
 * applied to any image type defined in PCL.
 *
 * \sa InterpolatingGeometricTransformation, ImageTransformation
 */
class PCL_CLASS GeometricTransformation : public virtual ImageTransformation
{
public:

   /*!
    * Constructs a %GeometricTransformation object.
    */
   GeometricTransformation() = default;

   /*!
    * Copy constructor.
    */
   GeometricTransformation( const GeometricTransformation& ) = default;

   /*!
    * Move constructor
    */
   GeometricTransformation( GeometricTransformation&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   GeometricTransformation& operator =( const GeometricTransformation& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   GeometricTransformation& operator =( GeometricTransformation&& ) = default;

   /*!
    * Destroys a %GeometricTransformation object.
    */
   ~GeometricTransformation() override
   {
   }

   /*!
    * Predicts transformed image dimensions.
    *
    * \param[in,out] width  Reference to a variable whose value is a horizontal
    *             dimension in pixels (width). On output, it will receive the
    *             predicted horizontal dimension after the transformation.
    *
    * \param[in,out] height Reference to a variable whose value is a vertical
    *             dimension in pixels (height). On output, it will receive the
    *             predicted vertical dimension after the transformation.
    *
    * \note This is a pure virtual member function that must be reimplemented
    * in every derived class.
    */
   virtual void GetNewSizes( int& width, int& height ) const = 0;

   /*!
    *
    */
   virtual bool SupportsGammaCorrection() const
   {
      return false;
   }

   /*!
    *
    */
   bool IsGammaCorrectionEnabled() const
   {
      return m_gammaCorrected;
   }

   /*!
    *
    */
   void EnableGammaCorrection( bool enable = true )
   {
      m_gammaCorrected = enable;
   }

   /*!
    *
    */
   void DisableGammaCorrection( bool disable = true )
   {
      EnableGammaCorrection( !disable );
   }

   /*!
    *
    */
   const RGBColorSystem& RGBWorkingSpace() const
   {
      return m_rgbws.IsNull() ? RGBColorSystem::sRGB : *m_rgbws;
   }

   /*!
    *
    */
   void SetRGBWorkingSpace( const RGBColorSystem& rgbws )
   {
      m_rgbws = new RGBColorSystem( rgbws );
   }

   /*!
    *
    */
   void ClearRGBWorkingSpace()
   {
      m_rgbws.Destroy();
   }

   /*!
    *
    */
   template <class P>
   void ApplyGammaCorrection( typename P::sample* samples, size_type length,
                              AbstractImage::ThreadData& threadData, int maxProcessors = PCL_MAX_PROCESSORS ) const
   {
      if ( m_rgbws.IsNull() || m_rgbws->IsSRGB() )
         ApplySRGBGammaCorrection<P>( samples, length, threadData, maxProcessors );
      else
         ApplyGammaExponentCorrection<P>( samples, length, m_rgbws->Gamma(), threadData, maxProcessors );
   }

   /*!
    *
    */
   template <class P>
   void ApplyInverseGammaCorrection( typename P::sample* samples, size_type length,
                              AbstractImage::ThreadData& threadData, int maxProcessors = PCL_MAX_PROCESSORS ) const
   {
      if ( m_rgbws.IsNull() || m_rgbws->IsSRGB() )
         ApplyInverseSRGBGammaCorrection<P>( samples, length, threadData, maxProcessors );
      else
         ApplyInverseGammaExponentCorrection<P>( samples, length, m_rgbws->Gamma(), threadData, maxProcessors );
   }

   /*!
    * Applies a gamma correction function to a block of pixel sample values.
    *
    * \param[in,out] samples  Pointer to the start of a contiguous block of
    *                   pixel sample values. On input, these values must be
    *                   expressed in a nonlinear color space with the specified
    *                   \a gamma exponent. On output, these values will be
    *                   expressed in a linear color space.
    *
    * \param length           Number of pixel sample values to process.
    *
    * \param gamma            The gamma exponent of the nonlinear color space.
    *                   A value \a gamma > 0 must be specified.
    *
    * \param threadData       Reference to an AbstractImage::ThreadData object
    *                   for thread control and monitoring.
    *
    * \param maxProcessors    Maximum number of parallel threads to use. The
    *                   default value does not impose a practical limit.
    */
   template <class P>
   static void ApplyGammaExponentCorrection( typename P::sample* samples, size_type length, double gamma,
                                    AbstractImage::ThreadData& threadData, int maxProcessors = PCL_MAX_PROCESSORS )
   {
      PCL_PRECONDITION( gamma > 0 )
      if ( samples != nullptr )
         if ( length > 0 )
            if ( gamma != 1 )
            {
               Array<size_type> L = pcl::Thread::OptimalThreadLoads( length, 1024/*overheadLimit*/, maxProcessors );
               ReferenceArray<GammaThread<P>> threads;
               for ( size_type i = 0, n = 0; i < L.Length(); n += L[i++] )
                  threads.Add( new GammaThread<P>( threadData, gamma, samples, n, n + L[i] ) );
               AbstractImage::RunThreads( threads, threadData );
               threads.Destroy();
            }
   }

   /*!
    * Applies an inverse gamma correction function to a block of pixel sample
    * values.
    *
    * \param[in,out] samples  Pointer to the start of a contiguous block of
    *                   pixel sample values. On input, these values must be
    *                   expressed in a linear color space. On output, these
    *                   values will be expressed in a nonlinear color space
    *                   with the specified \a gamma exponent.
    *
    * \param length           Number of pixel sample values to process.
    *
    * \param gamma            The gamma exponent of the nonlinear color space.
    *                   A value \a gamma > 0 must be specified.
    *
    * \param threadData       Reference to an AbstractImage::ThreadData object
    *                   for thread control and monitoring.
    *
    * \param maxProcessors    Maximum number of parallel threads to use. The
    *                   default value does not impose a practical limit.
    */
   template <class P>
   static void ApplyInverseGammaExponentCorrection( typename P::sample* samples, size_type length, double gamma,
                                     AbstractImage::ThreadData& threadData, int maxProcessors = PCL_MAX_PROCESSORS )
   {
      PCL_PRECONDITION( gamma > 0 )
      if ( samples != nullptr )
         if ( length > 0 )
            if ( gamma != 1 )
            {
               Array<size_type> L = pcl::Thread::OptimalThreadLoads( length, 1024/*overheadLimit*/, maxProcessors );
               ReferenceArray<GammaInvThread<P>> threads;
               for ( size_type i = 0, n = 0; i < L.Length(); n += L[i++] )
                  threads.Add( new GammaInvThread<P>( threadData, gamma, samples, n, n + L[i] ) );
               AbstractImage::RunThreads( threads, threadData );
               threads.Destroy();
            }
   }

   /*!
    * Applies an sRGB gamma correction function to a block of pixel sample
    * values.
    *
    * \param[in,out] samples  Pointer to the start of a contiguous block of
    *                   pixel sample values. On input, these values must be
    *                   expressed in the sRGB color space. On output, these
    *                   values will be expressed in a linear color space.
    *
    * \param length           Number of pixel sample values to process.
    *
    * \param threadData       Reference to an AbstractImage::ThreadData object
    *                   for thread control and monitoring.
    *
    * \param maxProcessors    Maximum number of parallel threads to use. The
    *                   default value does not impose a practical limit.
    */
   template <class P>
   static void ApplySRGBGammaCorrection( typename P::sample* samples, size_type length,
                                     AbstractImage::ThreadData& threadData, int maxProcessors = PCL_MAX_PROCESSORS )
   {
      if ( samples != nullptr )
         if ( length > 0 )
         {
            Array<size_type> L = pcl::Thread::OptimalThreadLoads( length, 1024/*overheadLimit*/, maxProcessors );
            ReferenceArray<SRGBGammaThread<P>> threads;
            for ( size_type i = 0, n = 0; i < L.Length(); n += L[i++] )
               threads.Add( new SRGBGammaThread<P>( threadData, samples, n, n + L[i] ) );
            AbstractImage::RunThreads( threads, threadData );
            threads.Destroy();
         }
   }

   /*!
    * Applies an inverse sRGB gamma correction function to a block of pixel
    * sample values.
    *
    * \param[in,out] samples  Pointer to the start of a contiguous block of
    *                   pixel sample values. On input, these values must be
    *                   expressed in a linear color space. On output, these
    *                   values will be expressed in the sRGB color space.
    *
    * \param length           Number of pixel sample values to process.
    *
    * \param threadData       Reference to an AbstractImage::ThreadData object
    *                   for thread control and monitoring.
    *
    * \param maxProcessors    Maximum number of parallel threads to use. The
    *                   default value does not impose a practical limit.
    */
   template <class P>
   static void ApplyInverseSRGBGammaCorrection( typename P::sample* samples, size_type length,
                                     AbstractImage::ThreadData& threadData, int maxProcessors = PCL_MAX_PROCESSORS )
   {
      if ( samples != nullptr )
         if ( length > 0 )
         {
            Array<size_type> L = pcl::Thread::OptimalThreadLoads( length, 1024/*overheadLimit*/, maxProcessors );
            ReferenceArray<SRGBGammaInvThread<P>> threads;
            for ( size_type i = 0, n = 0; i < L.Length(); n += L[i++] )
               threads.Add( new SRGBGammaInvThread<P>( threadData, samples, n, n + L[i] ) );
            AbstractImage::RunThreads( threads, threadData );
            threads.Destroy();
         }
   }

protected:

   bool                              m_gammaCorrected = false;
   AutoPointerCloner<RGBColorSystem> m_rgbws;

   template <class P>
   class GammaThread : public pcl::Thread
   {
   public:

      using sample_type = typename P::sample;

      GammaThread( AbstractImage::ThreadData& data, double gamma, sample_type* samples, size_type start, size_type end )
         : m_data( data )
         , m_gamma( gamma )
         , m_samples( samples )
         , m_start( start )
         , m_end( end )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         sample_type* __restrict__ f = m_samples + m_start;
         for ( size_type i = m_start; i < m_end; ++i, ++f )
         {
            double x; P::FromSample( x, *f );
            *f = P::ToSample( pcl::Pow( x, m_gamma ) );

            UPDATE_THREAD_MONITOR( 256 )
         }
      }

   private:

      AbstractImage::ThreadData& m_data;
      double                     m_gamma;
      sample_type*               m_samples;
      size_type                  m_start;
      size_type                  m_end;
   };

   template <class P>
   class GammaInvThread : public pcl::Thread
   {
   public:

      using sample_type = typename P::sample;

      GammaInvThread( AbstractImage::ThreadData& data, double gamma, sample_type* samples, size_type start, size_type end )
         : m_data( data )
         , m_gammaInv( 1/gamma )
         , m_samples( samples )
         , m_start( start )
         , m_end( end )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         sample_type* __restrict__ f = m_samples + m_start;
         for ( size_type i = m_start; i < m_end; ++i, ++f )
         {
            double x; P::FromSample( x, *f );
            *f = P::ToSample( pcl::Pow( x, m_gammaInv ) );

            UPDATE_THREAD_MONITOR( 256 )
         }
      }

   private:

      AbstractImage::ThreadData& m_data;
      double                     m_gammaInv;
      sample_type*               m_samples;
      size_type                  m_start;
      size_type                  m_end;
   };

   template <class P>
   class SRGBGammaThread : public pcl::Thread
   {
   public:

      using sample_type = typename P::sample;

      SRGBGammaThread( AbstractImage::ThreadData& data, sample_type* samples, size_type start, size_type end )
         : m_data( data )
         , m_samples( samples )
         , m_start( start )
         , m_end( end )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         sample_type* __restrict__ f = m_samples + m_start;
         for ( size_type i = m_start; i < m_end; ++i, ++f )
         {
            double x; P::FromSample( x, *f );
            *f = P::ToSample( RGBColorSystem::SRGBToLinear( x ) );

            UPDATE_THREAD_MONITOR( 256 )
         }
      }

   private:

      AbstractImage::ThreadData& m_data;
      sample_type*               m_samples;
      size_type                  m_start;
      size_type                  m_end;
   };

   template <class P>
   class SRGBGammaInvThread : public pcl::Thread
   {
   public:

      using sample_type = typename P::sample;

      SRGBGammaInvThread( AbstractImage::ThreadData& data, sample_type* samples, size_type start, size_type end )
         : m_data( data )
         , m_samples( samples )
         , m_start( start )
         , m_end( end )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         sample_type* __restrict__ f = m_samples + m_start;
         for ( size_type i = m_start; i < m_end; ++i, ++f )
         {
            double x; P::FromSample( x, *f );
            *f = P::ToSample( RGBColorSystem::LinearToSRGB( x ) );

            UPDATE_THREAD_MONITOR( 256 )
         }
      }

   private:

      AbstractImage::ThreadData& m_data;
      sample_type*               m_samples;
      size_type                  m_start;
      size_type                  m_end;
   };
};

// ----------------------------------------------------------------------------

/*!
 * \class InterpolatingGeometricTransformation
 * \brief Abstract base class of all PCL interpolating geometric image
 *        transformations.
 *
 * %InterpolatingGeometricTransformation represents a geometrical
 * transformation that uses a pixel interpolating algorithm to yield
 * transformed pixel sample values. Transformations implemented as derived
 * classes of %InterpolatingGeometricTransformation can be applied to all
 * standard image types defined in PCL.
 *
 * \sa GeometricTransformation, PixelInterpolation, ImageTransformation
 */
class PCL_CLASS InterpolatingGeometricTransformation : public GeometricTransformation
{
public:

   /*!
    * Constructs a %InterpolatingGeometricTransformation object that will use
    * the specified pixel interpolation \a p.
    *
    * The specified PixelInterpolation object must remain valid and accessible
    * as long as this object exists, or until this object is associated with a
    * different pixel interpolation.
    */
   InterpolatingGeometricTransformation( PixelInterpolation& p )
      : m_interpolation( &p )
   {
      PCL_CHECK( m_interpolation != nullptr )
   }

   /*!
    * Copy constructor. The constructed object will use the same pixel
    * interpolation as the specified source object.
    *
    * The PixelInterpolation object used by both this object and the source
    * object must remain valid and accessible as long as at least one of both
    * objects is associated with it.
    */
   InterpolatingGeometricTransformation( const InterpolatingGeometricTransformation& ) = default;

   /*!
    * Move constructor.
    */
   InterpolatingGeometricTransformation( InterpolatingGeometricTransformation&& x )
      : GeometricTransformation( std::move( x ) )
      , m_interpolation( x.m_interpolation )
      , m_unclipped( x.m_unclipped )
   {
      x.m_interpolation = nullptr;
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    *
    * After assignment, this object will use the same pixel interpolation as
    * the specified source object. The previously used pixel interpolation is
    * simply forgotten by this object, which will no longer depend on it.
    *
    * The PixelInterpolation object used by both this object and the source
    * object must remain valid and accessible as long as at least one of both
    * objects is associated with it.
    */
   InterpolatingGeometricTransformation& operator =( const InterpolatingGeometricTransformation& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   InterpolatingGeometricTransformation& operator =( InterpolatingGeometricTransformation&& x )
   {
      GeometricTransformation::operator =( std::move( x ) );
      m_interpolation = x.m_interpolation;
      x.m_interpolation = nullptr;
      m_unclipped = x.m_unclipped;
      return *this;
   }

   /*!
    * Returns a reference to the unmodifiable PixelInterpolation object that
    * this transformation is currently using.
    */
   const PixelInterpolation& Interpolation() const
   {
      PCL_CHECK( m_interpolation != nullptr )
      return *m_interpolation;
   }

   /*!
    * Returns a reference to the PixelInterpolation object that this
    * transformation is currently using.
    */
   PixelInterpolation& Interpolation()
   {
      PCL_CHECK( m_interpolation != nullptr )
      return *m_interpolation;
   }

   /*!
    * Causes this transformation to use the specified pixel interpolation \a p.
    *
    * The new pixel interpolation will be used to generate transformed pixel
    * values after calling this function. The previously used pixel
    * interpolation is simply forgotten by this object, which will no longer
    * depend on it.
    *
    * The specified PixelInterpolation object must remain valid and accessible
    * as long as this object exists, or until this object is associated with a
    * different pixel interpolation.
    */
   void SetInterpolation( PixelInterpolation& p )
   {
      m_interpolation = &p;
      PCL_CHECK( m_interpolation != nullptr )
   }

   /*!
    * Returns true if this image transformation applies unclipped pixel
    * interpolations. Unclipped interpolations don't constrain interpolated
    * pixel sample values to the native range of the pixel data type of the
    * transformed image. This allows to apply interpolating transformations to
    * images that exceed their representable ranges; for example, when images
    * are used to store and manipulate arbitrary matrices for convenience.
    *
    * Unclipped transformations are always disabled by default.
    */
   bool UsingUnclippedInterpolation() const
   {
      return m_unclipped;
   }

   /*!
    * Enables the use of unclipped pixel interpolations. See
    * UsingUnclippedInterpolation() for more information.
    */
   void EnableUnclippedInterpolation( bool enable = true )
   {
      m_unclipped = enable;
   }

   /*!
    * Disables the use of unclipped pixel interpolations. See
    * UsingUnclippedInterpolation() for more information.
    */
   void DisableUnclippedInterpolation( bool disable = true )
   {
      EnableUnclippedInterpolation( !disable );
   }

protected:

   PixelInterpolation* m_interpolation = nullptr;
   bool                m_unclipped = false;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_GeometricTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/GeometricTransformation.h - Released 2024-12-27T18:16:01Z
