//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/Convolution.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_Convolution_h
#define __PCL_Convolution_h

/// \file pcl/Convolution.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AutoPointer.h>
#include <pcl/InterlacedTransformation.h>
#include <pcl/KernelFilter.h>
#include <pcl/ParallelProcess.h>
#include <pcl/ThresholdedTransformation.h>

#define __PCL_CONVOLUTION_TINY_WEIGHT 1.0e-20

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Convolution
 * \brief Discrete two-dimensional nonseparable convolution in the spatial
 * domain
 *
 * %Convolution implements a fully multithreaded, two-dimensional discrete
 * nonseparable convolution algorithm. It performs automatic fixing of
 * border artifacts by applying Neumann boundary conditions (mirroring).
 *
 * \note ImageTransformation is a virtual base class of %Convolution.
 *
 * \sa SeparableConvolution, FFTConvolution, KernelFilter
 */
class PCL_CLASS Convolution : public InterlacedTransformation,
                              public ThresholdedTransformation,
                              public ParallelProcess
{                          // NB: ImageTransformation is a virtual base class
public:

   /*!
    * Default constructor.
    *
    * \note This constructor yields an uninitialized instance that cannot be
    * used before explicit association with a KernelFilter instance.
    */
   Convolution() = default;

   /*!
    * Constructs a %Convolution instance with the specified filter.
    *
    * \param filter  Response function, or <em>convolution filter</em>. The
    *                specified object does not have to remain valid while this
    *                instance is actively used, since %Convolution owns a
    *                private copy of the filter (note that KernelFilter is a
    *                reference-counted class).
    */
   Convolution( const KernelFilter& filter )
   {
      SetFilter( filter );
   }

   /*!
    * Copy constructor.
    */
   Convolution( const Convolution& x )
      : InterlacedTransformation( x )
      , ThresholdedTransformation( x )
      , ParallelProcess( x )
      , m_weight( x.m_weight )
      , m_highPass( x.m_highPass )
      , m_rawHighPass( x.m_rawHighPass )
      , m_rescaleHighPass( x.m_rescaleHighPass )
   {
      if ( !x.m_filter.IsNull() )
         m_filter = x.m_filter->Clone();
   }

   /*!
    * Move constructor.
    */
   Convolution( Convolution&& ) = default;

   /*!
    * Destroys this %Convolution object.
    */
   ~Convolution() override
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   Convolution& operator =( const Convolution& x )
   {
      if ( &x != this )
      {
         (void)InterlacedTransformation::operator =( x );
         (void)ThresholdedTransformation::operator =( x );
         (void)ParallelProcess::operator =( x );
         if ( x.m_filter.IsNull() )
            m_filter.Destroy();
         else
            m_filter = x.m_filter->Clone();
         m_weight = x.m_weight;
         m_highPass = x.m_highPass;
         m_rawHighPass = x.m_rawHighPass;
         m_rescaleHighPass = x.m_rescaleHighPass;
      }
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   Convolution& operator =( Convolution&& ) = default;

   /*!
    * Returns a reference to the kernel filter currently associated with this
    * %Convolution object.
    *
    * If this object has not been initialized, this member function returns an
    * empty kernel filter.
    */
   const KernelFilter& Filter() const
   {
      PCL_PRECONDITION( !m_filter.IsNull() )
      return *m_filter;
   }

   /*!
    * Sets a new kernel \a filter to be applied by this %Convolution object.
    */
   void SetFilter( const KernelFilter& filter )
   {
      m_filter = filter.Clone();
      CacheFilterProperties();
   }

   /*!
    * Returns the current filter weight. The filter weight is computed each
    * time a kernel filter is associated with this object. It is only
    * applied for low-pass filters as a normalization factor. For more
    * information, see the documentation for KernelFilter::Weight().
    *
    * The filter weight and other filter properties are cached in private data
    * members for quick reference.
    */
   double FilterWeight() const
   {
      return m_weight;
   }

   /*!
    * Returns true if the kernel filter currently associated with this
    * %Convolution object is a high-pass filter; false if it is a low-pass
    * filter. For more information, see the documentation for
    * kernelFilter::IsHighPassFilter().
    *
    * Each time a kernel filter is associated with this object, its high-pass
    * nature is checked and stored, along with other filter properties, in
    * private data members. This allows for quick lookup of critical filter
    * characteristics without degrading performance.
    */
   bool IsHighPassFilter() const
   {
      return m_highPass;
   }

   /*!
    * Returns true iff out-of-range values will be rescaled for normalization of
    * images after convolution with a high-pass filter.
    *
    * A high-pass filter has negative coefficients. As a result, some pixels in
    * the convolved image may have negative values. Saturated pixels (values
    * above one) can also result, depending on the filter coefficients. The
    * standard behavior is to truncate out-of-range pixel values to the [0,1]
    * range, which preserves the dynamics of the convolved image, so high-pass
    * rescaling is disabled by default. When high-pass rescaling is enabled,
    * the resulting image is \e normalized (that is, rescaled to [0,1] only if
    * there are out-of-range values) and hence all the data after convolution
    * are preserved at the cost of reducing the overall contrast of the image.
    * Finally, if <em>raw high-pass convolution</em> has been enabled,
    * out-of-range values are neither truncated nor rescaled irrespective of
    * the value returned by this function. See the documentation for
    * IsRawHighPassEnabled() for more information.
    */
   bool IsHighPassRescalingEnabled() const
   {
      return m_rescaleHighPass;
   }

   /*!
    * Enables (or disables) high-pass rescaling of out-of-range convolved pixel
    * values. See the documentation for IsHighPassRescalingEnabled() for more
    * information.
    */
   void EnableHighPassRescaling( bool enable = true )
   {
      m_rescaleHighPass = enable;
   }

   /*!
    * Disables (or enables) high-pass rescaling of out-of-range convolved pixel
    * values. See the documentation for IsHighPassRescalingEnabled() for more
    * information.
    */
   void DisableHighPassRescaling( bool disable = true )
   {
      EnableHighPassRescaling( !disable );
   }

   /*!
    * Returns true iff <em>raw high-pass convolution</em> is enabled. When raw
    * high-pass convolution is enabled, out-of-range values after convolution
    * with a high-pass filter are neither truncated nor normalized. Note that
    * this is only relevant to convolution of floating point data.
    *
    * Raw high-pass convolution is disabled by default. For more information on
    * out-of-range convolution results, refer to the documentation for
    * IsHighPassRescalingEnabled().
    */
   bool IsRawHighPassEnabled() const
   {
      return m_rawHighPass;
   }

   /*!
    * Enables (or disables) raw high-pass convolution. See the documentation
    * for IsRawHighPassEnabled() for more information.
    */
   void EnableRawHighPass( bool enable = true )
   {
      m_rawHighPass = enable;
   }

   /*!
    * Disables (or enables) raw high-pass convolution. See the documentation
    * for IsRawHighPassEnabled() for more information.
    */
   void DisableRawHighPass( bool disable = true )
   {
      EnableRawHighPass( !disable );
   }

   /*!
    * Returns the length in pixels of the overlapping regions between adjacent
    * areas processed by parallel execution threads. The overlapping distance
    * is a function of the filter size and the interlacing distance.
    */
   int OverlappingDistance() const
   {
      PCL_PRECONDITION( !m_filter.IsNull() )
      return m_filter->Size() + (m_filter->Size() - 1)*(InterlacingDistance() - 1);
   }

protected:

   /*
    * The response function for convolution is defined as a kernel filter.
    */
   AutoPointer<KernelFilter> m_filter;

   /*
    * Cached filter properties.
    */
   double m_weight = 0;       // filter weight for low-pass normalization
   bool   m_highPass = false; // true if this is a high-pass filter

   /*
    * User-selectable options
    */
   bool   m_rawHighPass = false;     // neither truncate nor normalize out-of-range values
   bool   m_rescaleHighPass = false; // truncate out-of-range values instead of normalize

   /*
    * In-place 2-D nonseparable convolution algorithm in the spatial domain.
    */
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;

private:

   void CacheFilterProperties()
   {
      PCL_PRECONDITION( !m_filter.IsNull() )
      PCL_PRECONDITION( !m_filter->IsEmpty() )
      ValidateFilter();
      m_highPass = m_filter->IsHighPassFilter();
      m_weight = m_filter->Weight();
      if ( pcl::Abs( m_weight ) < __PCL_CONVOLUTION_TINY_WEIGHT )
         m_weight = 1;
   }

   void ValidateFilter() const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Convolution_h

// ----------------------------------------------------------------------------
// EOF pcl/Convolution.h - Released 2024-12-28T16:53:48Z
