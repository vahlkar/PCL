//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Data.h - Released 2021-05-05T15:38:07Z
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

#ifndef __Data_h
#define __Data_h

#include "Expression.h"

#include <pcl/AutoPointer.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS ImageWindow;

// ----------------------------------------------------------------------------

/*
 * Abstract base class of all PixelMath data expression components.
 */
class Data : public Expression
{
public:

   Data( unsigned t, int p )
      : Expression( t, p )
   {
   }

   Data( const Data& ) = default;

   virtual Expression* Clone() const = 0;
   virtual String ToString() const = 0;
};

// ----------------------------------------------------------------------------

/*
 * Generic reference to a data object (such as an image or a variable).
 */
class ObjectReference : public Data
{
public:

   ObjectReference( unsigned t, int p, const String& id )
      : Data( t, p )
      , m_objectId( id )
   {
   }

   ObjectReference( const ObjectReference& ) = default;

   String Id() const
   {
      return m_objectId;
   }

   bool IsChannelIndex() const
   {
      return m_channelIndex >= 0;
   }

   int ChannelIndex() const
   {
      return m_channelIndex;
   }

   void SetChannelIndex( int c )
   {
      m_channelIndex = c;
   }

   virtual Expression* Clone() const = 0;

   String ToString() const override
   {
      String id = m_objectId;
      if ( m_channelIndex >= 0 )
         id.AppendFormat( "[%d]", m_channelIndex );
      return id;
   }

private:

   String m_objectId;
   int    m_channelIndex = -1;
};

// ----------------------------------------------------------------------------

/*
 * Reference to an image.
 */
class ImageReference : public ObjectReference
{
public:

   ImageReference( const String& id, int p, bool isWindow = true );
   ImageReference( const ImageReference& );

   virtual ~ImageReference();

   const ImageVariant* Image() const
   {
      return m_image;
   }

   virtual bool FindImage();

   bool RequiresAstrometricSolution() const
   {
      return m_astrometryRequired;
   }

   void RequireAstrometricSolution() const
   {
      m_astrometryRequired = true;
   }

   bool HasAstrometricSolution() const;

   bool IsWindow() const
   {
      return m_isWindow;
   }

   const ImageWindow* Window() const
   {
      return m_window.Ptr();
   }

   bool ByReference() const
   {
      return m_byReference;
   }

   void SetByReference() const
   {
      m_byReference = true;
   }

   bool IsInterpolated() const
   {
      return *m_interpolators != nullptr;
   }

   void** Interpolators() const
   {
      return m_interpolators;
   }

   void InitInterpolators( PixelInterpolation* );

   Expression* Clone() const override
   {
      return new ImageReference( *this );
   }

private:

           bool                     m_isWindow = true;     // true -> this represents an existing image window
           AutoPointer<ImageWindow> m_window;              // the existing image window if m_isWindow == true
   mutable void*                    m_interpolators[ 3 ] = {};
   mutable bool                     m_byReference = false; // true -> will be passed by reference instead of by value
   mutable bool                     m_astrometryRequired = false; // true -> astrometric solution is required for this image

protected:

   ImageVariant* m_image = nullptr;
};

// ----------------------------------------------------------------------------

/*
 * Reference to a variable.
 */
class VariableReference : public ObjectReference
{
public:

   VariableReference( Variable* v, int p )
      : ObjectReference( XPR_VARREF, p, v->Id() )
      , m_variable( v )
   {
   }

   VariableReference( const VariableReference& ) = default;

   Variable& Reference() const
   {
      return *m_variable;
   }

   virtual bool IsLValue() const
   {
      return true;
   }

   Expression* Clone() const override
   {
      return new VariableReference( *this );
   }

private:

   Variable* m_variable = nullptr;
};

// ----------------------------------------------------------------------------

/*
 * Reference to a constant.
 */
class ConstantReference : public ObjectReference
{
public:

   ConstantReference( Constant* c, int p )
      : ObjectReference( XPR_CONSTREF, p, c->Id() )
      , m_constant( c )
   {
   }

   ConstantReference( const ConstantReference& ) = default;

   Constant& Reference() const
   {
      return *m_constant;
   }

   Expression* Clone() const override
   {
      return new ConstantReference( *this );
   }

private:

   Constant* m_constant = nullptr;
};

// ----------------------------------------------------------------------------

/*
 * Representation of a pixel component in a PixelMath expression.
 */
class PixelData : public Data
{
public:

   PixelData( const Pixel& px, int p )
      : Data( XPR_PIXEL, p )
      , m_pixel( px )
   {
   }

   PixelData( const PixelData& ) = default;

   const Pixel& PixelValue() const
   {
      return m_pixel;
   }

   Expression* Clone() const override
   {
      return new PixelData( *this );
   }

   String ToString() const override
   {
      return m_pixel.ToString();
   }

private:

   Pixel m_pixel;
};

// ----------------------------------------------------------------------------

/*
 * Representation of a pixel sample in a PixelMath expression.
 */
class Sample : public Data
{
public:

   Sample( double v, int p )
      : Data( XPR_SAMPLE, p )
      , m_value( v )
   {
   }

   Sample( const Sample& ) = default;

   double Value() const
   {
      return m_value;
   }

   Expression* Clone() const override
   {
      return new Sample( *this );
   }

   String ToString() const override
   {
      return String( m_value );
   }

private:

   double m_value;
};

// ----------------------------------------------------------------------------

/*
 * Reference to an image represented as a metasymbol in a PixelMath expression.
 */
class MetaSymbol : public ImageReference
{
public:

   MetaSymbol( const String& id, int p )
      : ImageReference( id, p )
   {
   }

   MetaSymbol( const MetaSymbol& ) = default;

   Expression* Clone() const override
   {
      return new MetaSymbol( *this );
   }

   String ToString() const override
   {
      String s = '$' + Id();
      if ( ChannelIndex() >= 0 )
         s.AppendFormat( "[%d]", ChannelIndex() );
      return s;
   }
};

// ----------------------------------------------------------------------------

/*
 * Reference to an internal image.
 */
class InternalImageReference : public ImageReference
{
public:

   InternalImageReference( const String& id, int p )
      : ImageReference( id, p, false/*isWindow*/ )
   {
   }

   InternalImageReference( const InternalImageReference& ) = default;

   bool FindImage() override;

   Expression* Clone() const override
   {
      return new InternalImageReference( *this );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Data_h

// ----------------------------------------------------------------------------
// EOF Data.h - Released 2021-05-05T15:38:07Z
