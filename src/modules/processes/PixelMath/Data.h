//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.5
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.5.0
// ----------------------------------------------------------------------------
// Data.h - Released 2020-12-12T20:51:40Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L. All Rights Reserved.
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

namespace pcl
{

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
      , objectId( id )
   {
   }

   ObjectReference( const ObjectReference& ) = default;

   String Id() const
   {
      return objectId;
   }

   bool IsChannelIndex() const
   {
      return channelIndex >= 0;
   }

   int ChannelIndex() const
   {
      return channelIndex;
   }

   void SetChannelIndex( int c )
   {
      channelIndex = c;
   }

   virtual Expression* Clone() const = 0;

   String ToString() const override
   {
      String id = objectId;
      if ( channelIndex >= 0 )
         id.AppendFormat( "[%d]", channelIndex );
      return id;
   }

private:

   String objectId;
   int    channelIndex = -1;
};

// ----------------------------------------------------------------------------

/*
 * Reference to an image.
 */
class ImageReference : public ObjectReference
{
public:

   ImageReference( const String& id, int p );
   ImageReference( const ImageReference& );

   virtual ~ImageReference();

   const ImageVariant* Image() const
   {
      return image;
   }

   bool FindImage();

   bool ByReference() const
   {
      return byReference;
   }

   void SetByReference() const
   {
      byReference = true; // mutable byReference
   }

   bool IsInterpolated() const
   {
      return *interpolators != nullptr;
   }

   void** Interpolators() const
   {
      return interpolators;
   }

   void InitInterpolators( PixelInterpolation* );

   Expression* Clone() const override
   {
      return new ImageReference( *this );
   }

private:

           ImageVariant* image = nullptr;
   mutable void*         interpolators[ 3 ] = {};
   mutable bool          byReference = false; // true -> will be passed by reference instead of by value
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
      , variable( v )
   {
   }

   VariableReference( const VariableReference& ) = default;

   Variable& Reference() const
   {
      return *variable;
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

   Variable* variable = nullptr;
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
      , constant( c )
   {
   }

   ConstantReference( const ConstantReference& ) = default;

   Constant& Reference() const
   {
      return *constant;
   }

   Expression* Clone() const override
   {
      return new ConstantReference( *this );
   }

private:

   Constant* constant = nullptr;
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
      , pixel( px )
   {
   }

   PixelData( const PixelData& ) = default;

   const Pixel& PixelValue() const
   {
      return pixel;
   }

   Expression* Clone() const override
   {
      return new PixelData( *this );
   }

   String ToString() const override
   {
      return pixel.ToString();
   }

private:

   Pixel pixel;
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
      , value( v )
   {
   }

   Sample( const Sample& ) = default;

   double Value() const
   {
      return value;
   }

   Expression* Clone() const override
   {
      return new Sample( *this );
   }

   String ToString() const override
   {
      return String( value );
   }

private:

   double value;
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

} // pcl

#endif   // __Data_h

// ----------------------------------------------------------------------------
// EOF Data.h - Released 2020-12-12T20:51:40Z
