//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Pixel.h - Released 2021-05-05T15:38:07Z
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

#ifndef __Pixel_h
#define __Pixel_h

#include <pcl/Array.h>
#include <pcl/Point.h>
#include <pcl/Random.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Thread-local storage data associated with a Pixel component.
 */
struct PixelTLS
{
   /*
    * For now we use TLS to support lock-free pseudo-random number generation.
    * In future versions we'll implement additional thread-private data items.
    */
   XoShiRo256ss random;
//    XoRoShiRo1024ss random;
};

// ----------------------------------------------------------------------------

/*
 * A pixel component in a PixelMath expression.
 *
 * The pixel abstraction represented by this class constitutes the fundamental
 * data item of a PixelMath expression. A pixel component can represent an
 * actual image pixel or a pixel sample of any image, or a reference to a
 * constant or variable.
 */
class Pixel
{
public:

   /*
    * Constructor from coordinates, default zero sample values.
    */
   Pixel( int x = 0, int y = 0 )
      : pos( x, y )
   {
      samples[0] = samples[1] = samples[2] = 0;
   }

   /*
    * Construction with thread-local storage data.
    */
   Pixel( PixelTLS* tls, int x, int y )
      : pos( x, y )
      , m_tls( tls )
   {
      samples[0] = samples[1] = samples[2] = 0;
   }

   /*
    * Construction with a monochrome sample value.
    */
   Pixel( double v, int x = 0, int y = 0 )
      : length( 1 )
      , pos( x, y )
   {
      samples[0] = samples[1] = samples[2] = v;
   }

   /*
    * Construction with RGB sample values.
    */
   Pixel( double v0, double v1, double v2, int x = 0, int y = 0 )
      : pos( x, y )
   {
      samples[0] = v0;
      samples[1] = v1;
      samples[2] = v2;
   }

   /*
    * Copy constructor.
    */
   Pixel( const Pixel& x )
   {
      Assign( x );
   }

   /*
    * Assignment operator. Returns a reference to this object.
    */
   Pixel& operator =( const Pixel& x )
   {
      Assign( x );
      return *this;
   }

   /*
    * Copy assignment of pixel sample data and object references.
    *
    * Thread-local storage data are not assigned, as they are uniquely
    * associated with a particular instance.
    */
   void Assign( const Pixel& x )
   {
      pos        = x.pos;
      reference  = x.reference;
      refChannel = x.refChannel;
      AssignSamples( x );
   }

   /*
    * Copy assignment of pixel sample values.
    */
   void AssignSamples( const Pixel& x )
   {
      if ( (length = x.length) == 1 )
         samples[0] = samples[1] = samples[2] = x.samples[0];
      else
      {
         samples[0] = x.samples[0];
         samples[1] = x.samples[1];
         samples[2] = x.samples[2];
      }
   }

   /*
    * Returns true iff this pixel component represents a reference to another
    * object, such as an image or a variable.
    */
   bool HasReference() const
   {
      return reference != nullptr;
   }

   /*
    * Returns the object referenced by this pixel component, or nullptr if this
    * instance does not transport an object reference.
    */
   void* Reference() const
   {
      return reference;
   }

   /*
    * Returns true iff this pixel component has an associated channel index.
    */
   bool HasReferenceChannel() const
   {
      return refChannel >= 0;
   }

   /*
    * Returns the channel index (in the [0,2] range) associated with this pixel
    * component, or -1 if this instance does not reference an specific channel.
    */
   int ReferenceChannel() const
   {
      return refChannel;
   }

   /*
    * Causes this pixel component to reference the specified object and channel
    * index.
    */
   template <class T>
   void SetReference( T& object, int channel ) const
   {
      reference  = &object;
      refChannel = channel;
   }

   /*
    * Causes this pixel component to reference the specified object, with no
    * specific channel index reference.
    */
   template <class T>
   void SetReference( T& object ) const
   {
      reference = &object;
   }

   /*
    * Returns true iff this pixel component represents an RGB color pixel, with
    * three separate channel pixel samples.
    */
   bool IsRGB() const
   {
      return length > 1;
   }

   /*
    * Returns the number of color channels represented by this pixel component,
    * which should be 1 for a monochrome pixel, or 3 for an RGB color pixel.
    */
   int Length() const
   {
      return length;
   }

   /*
    * Returns the pixel sample at the specified channel index.
    */
   double operator []( int i ) const
   {
      return samples[i];
   }

   /*
    * Returns a mutable reference to the pixel sample at the specified channel
    * index.
    */
   double& operator []( int i )
   {
      return samples[i];
   }

   /*
    * Returns true iff this pixel component is zero for the specified channel
    * index i. If this pixel is monochrome, the channel index is ignored and
    * the function returns true iff the sample at channel index zero is zero.
    */
   bool IsLogicalZero( int i ) const
   {
      return samples[IsRGB() ? i : 0] == 0;
   }

   /*
    * Returns true iff this pixel component is nonzero for the specified
    * channel index i. If this pixel is monochrome, the channel index is
    * ignored and the function returns true iff the sample at channel index
    * zero is nonzero.
    */
   bool IsLogicalOne( int i ) const
   {
      return samples[IsRGB() ? i : 0] != 0;
   }

   /*
    * Causes all pixel samples to be equal to the specified value, and forces
    * this pixel component to represent a monochrome pixel.
    */
   void SetSamples( double v )
   {
      samples[0] = samples[1] = samples[2] = v;
      length = 1;
   }

   /*
    * Sets the values of the red, green and blue pixel samples, and forces this
    * pixel component to represent an RGB color pixel.
    */
   void SetSamples( double v0, double v1, double v2 )
   {
      samples[0] = v0; samples[1] = v1; samples[2] = v2;
      length = 3;
   }

   /*
    * Causes all pixel samples to be equal to the specified value, but does not
    * change the length (or color space) of this pixel component.
    */
   void CopySamples( double v )
   {
      samples[0] = samples[1] = samples[2] = v;
   }

   /*
    * Assigns pixel sample values from another instance, but does not change
    * the length (or color space) of this pixel component.
    */
   void CopySamples( const Pixel& x )
   {
      samples[0] = x.samples[0];
      samples[1] = x.samples[1];
      samples[2] = x.samples[2];
   }

   /*
    * Sets all pixel samples equal to the first channel sample, and forces this
    * pixel component to represent an RGB color pixel.
    */
   void ConvertToRGB()
   {
      samples[2] = samples[1] = samples[0];
      length = 3;
   }

   /*
    * Forces this pixel component to represent a monochrome pixel, without
    * altering pixel sample values.
    */
   void SetGrayscale()
   {
      length = 1;
   }

   /*
    * Forces this pixel component to represent an RGB color pixel, without
    * altering pixel sample values.
    */
   void SetRGB()
   {
      length = 3;
   }

   /*
    * Forces this pixel component to represent a pixel in the same color space
    * as the specified pixel, without altering pixel sample values.
    */
   void SetAs( const Pixel& p )
   {
      length = p.length;
   }

   /*
    * Returns the X-coordinate associated with this pixel.
    */
   int X() const
   {
      return pos.x;
   }

   /*
    * Returns the Y-coordinate associated with this pixel.
    */
   int Y() const
   {
      return pos.y;
   }

   /*
    * Returns a pointer to the thread-local storage data structure associated
    * with this pixel.
    */
   PixelTLS* TLS() const
   {
      return m_tls;
   }

   /*
    * Returns a readable string representation of this pixel component.
    */
   virtual String ToString() const
   {
      if ( length == 1 )
         return String().Format( "{%.15g}", *samples );
      return String().Format( "{%.15g, %.15g, %.15g}", samples[0], samples[1], samples[2] );
   }

private:

   int    length = 3;   // number of samples defined
   double samples[ 3 ]; // 0=R/K, 1=G, 2=B
   Point  pos;          // pixel coordinates

   /*
    * Variable references
    */
   mutable void* reference  = nullptr; // if nonzero, reference to an object
   mutable int   refChannel = -1;      // if >= 0, channel index for the referenced object

   PixelTLS*     m_tls = nullptr;
};

// ----------------------------------------------------------------------------

typedef Array<Pixel> pixel_set;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Pixel_h

// ----------------------------------------------------------------------------
// EOF Pixel.h - Released 2021-05-05T15:38:07Z
