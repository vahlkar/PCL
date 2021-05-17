//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.8.1
// ----------------------------------------------------------------------------
// Symbol.cpp - Released 2021-05-05T15:38:07Z
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

#include "Data.h"
#include "Expression.h"
#include "Symbol.h"

#include <pcl/Exception.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static int s_numberOfProcessors = 0;

enum
{
   SF_KEYWORD_VALUE,
   SF_KEYWORD_DEFINED,
   SF_PROPERTY_VALUE,
   SF_PROPERTY_DEFINED,
   SF_ENVVAR_VALUE,
   SF_ENVVAR_DEFINED,
   SF_WIDTH,
   SF_HEIGHT,
   SF_AREA,
   SF_INVAREA,
   SF_ISCOLOR,
   SF_MAXIMUM,
   SF_MINIMUM,
   SF_MEDIAN,
   SF_MEAN,
   SF_MDEV,
   SF_ADEV,
   SF_SDEV,
   SF_MODULUS,
   SF_SSQR,
   SF_ASQR,
   SF_PIXEL
};

static const char* s_functionName[] =
{
   "keyword_value",
   "keyword_defined",
   "property_value",
   "property_defined",
   "envvar_value",
   "envvar_defined",
   "width",
   "height",
   "area",
   "invarea",
   "iscolor",
   "maximum",
   "minimum",
   "median",
   "mean",
   "mdev",
   "adev",
   "sdev",
   "modulus",
   "ssqr",
   "asqr",
   "pixel",
   nullptr
};

// ----------------------------------------------------------------------------

Symbol::symbol_set Symbol::m_symbols;

// ----------------------------------------------------------------------------

Symbol::OutputData::OutputData( const GlobalVariable& g )
   : id( g.Id() )
{
   Pixel r = g.FinalValue();
   value = r.IsRGB() ? DVector( r[0], r[1], r[2] ) : DVector( r[0], 3 );
}

// ----------------------------------------------------------------------------

Symbol::Symbol( const String& id )
   : m_id( id )
{
   m_symbols << this;
}

// ----------------------------------------------------------------------------

static
String StripValueTokenQuotes( const String& s )
{
   if ( s.StartsWith( '\'' ) )
   {
      if ( !s.EndsWith( '\'' ) )
         throw ParseError( "Missing right single quote: " + s );
   }
   else if ( s.StartsWith( '\"' ) )
   {
      if ( !s.EndsWith( '\"' ) )
         throw ParseError( "Missing right double quote: " + s );
   }
   else
      return s.Trimmed();
   return s.Substring( 1, s.Length()-2 ).Trimmed();
}

static
String ParseValueImageId( const String& s )
{
   if ( !s.IsEmpty() )
   {
      if ( s == "$T" || s == "$target" )
         return String();
      if ( !s.IsValidIdentifier() )
         throw ParseError( "In symbol function specification: Invalid image identifier '" + s + "'" );
   }
   return s;
}

static
void NewConstantForImageStatistics( const String& id, int functionId, const StringList& valueTokens, const String& symDef )
{
   String imageId, channel;
   switch ( valueTokens.Length() )
   {
   case 2:
      channel = StripValueTokenQuotes( valueTokens[1] );
   case 1:
      imageId = ParseValueImageId( valueTokens[0] );
      break;
   case 0:
      break;
   default:
      throw ParseError( "The " + String( s_functionName[functionId] ) + " symbol function takes zero, one or two argument(s): " + symDef );
   }

   new Constant( id, functionId, imageId, channel );
}

void Symbol::Create( const String& defList )
{
   DestroyAll();

   String s = defList.Trimmed();
   StringList symDefs;
   {
      int p = 0;
      size_type i0 = 0;
      for ( size_type i = 0, n = s.Length(); i < n; ++i )
         switch ( s[i] )
         {
         case '(':
            ++p;
            break;
         case ')':
            --p;
            break;
         case ',':
         case ';':
            if ( p <= 0 )
            {
               String d = s.Substring( i0, i-i0 ).Trimmed();
               if ( d.IsEmpty() )
                  throw ParseError( "Missing symbol definition" );
               symDefs.Append( d );
               i0 = i+1;
            }
            break;
         default:
            break;
         }
      String d = s.Substring( i0 ).Trimmed();
      if ( !d.IsEmpty() )
         symDefs << d;
   }

   for ( const String& def : symDefs )
   {
      size_type eq = def.Find( '=' );

      String id = def.Left( eq ).Trimmed();
      if ( id.IsEmpty() )
         throw ParseError( "Missing symbol identifier: " + def );
      if ( !id.IsValidIdentifier() )
         throw ParseError( "Invalid symbol identifier: " + def );
      if ( IsDefined( id ) )
         throw ParseError( "Duplicate symbol identifier '" + id + "': " + def );

      if ( eq == String::notFound )
      {
         /*
          * Variable declaration
          */
         new Variable( id );
      }
      else
      {
         /*
          * Symbol definition
          */
         String value = def.Substring( eq+1 ).Trimmed();
         if ( value.IsEmpty() )
            throw ParseError( "Missing symbol value: " + def );

         if ( CharTraits::IsStartingSymbolDigit( *value ) )
         {
            /*
             * Symbol function
             */
            size_type lp = value.Find( '(' );
            if ( lp == String::notFound )
               throw ParseError( "In symbol function specification: Missing left parenthesis: " + def );
            size_type rp = value.Find( ')', lp+1 );
            if ( rp == String::notFound )
               throw ParseError( "In symbol function specification: Missing right parenthesis: " + def );
            if ( rp != value.Length()-1 )
               throw ParseError( "In symbol function specification: Extra token(s) after right parenthesis: " + def );

            String functionId = value.Substring( 0, lp ).Trimmed();
            if ( functionId.IsEmpty() )
               throw ParseError( "Missing symbol function name: " + def );

            StringList valueTokens;
            value.Substring( lp+1, rp-lp-1 ).Break( valueTokens, ',', true/*trim*/ );

            if ( functionId == "keyword_value" || functionId == "kwd_value" )
            {
               String imageId, keywordName;
               switch ( valueTokens.Length() )
               {
               case 1:
                  keywordName = StripValueTokenQuotes( valueTokens[0] );
                  break;
               case 2:
                  imageId = ParseValueImageId( valueTokens[0] );
                  keywordName = StripValueTokenQuotes( valueTokens[1] );
                  break;
               case 0:
                  throw ParseError( "In keyword_value() symbol function specification: Missing symbol function arguments: " + def );
               default:
                  throw ParseError( "The keyword_value() symbol function only takes one or two arguments: " + def );
               }
               if ( keywordName.IsEmpty() )
                  throw ParseError( "In keyword_value() symbol function specification: Missing keyword name: " + def );

               new Constant( id, SF_KEYWORD_VALUE, imageId, keywordName );
            }
            else if ( functionId == "keyword_defined" || functionId == "kwd_defined" )
            {
               String imageId, keywordName;
               switch ( valueTokens.Length() )
               {
               case 1:
                  keywordName = StripValueTokenQuotes( valueTokens[0] );
                  break;
               case 2:
                  imageId = ParseValueImageId( valueTokens[0] );
                  keywordName = StripValueTokenQuotes( valueTokens[1] );
                  break;
               case 0:
                  throw ParseError( "In keyword_defined() symbol function specification: Missing symbol function arguments: " + def );
               default:
                  throw ParseError( "The keyword_defined() symbol function only takes one or two arguments: " + def );
               }
               if ( keywordName.IsEmpty() )
                  throw ParseError( "In keyword_defined() symbol function specification: Missing keyword name: " + def );

               new Constant( id, SF_KEYWORD_DEFINED, imageId, keywordName );
            }
            else if ( functionId == "property_value" || functionId == "pty_value" )
            {
               String imageId, propertyName;
               switch ( valueTokens.Length() )
               {
               case 1:
                  propertyName = StripValueTokenQuotes( valueTokens[0] );
                  break;
               case 2:
                  imageId = ParseValueImageId( valueTokens[0] );
                  propertyName = StripValueTokenQuotes( valueTokens[1] );
                  break;
               case 0:
                  throw ParseError( "In property_value() symbol function specification: Missing symbol function arguments: " + def );
               default:
                  throw ParseError( "The property_value() symbol function only takes one or two arguments: " + def );
               }
               if ( propertyName.IsEmpty() )
                  throw ParseError( "In property_value() symbol function specification: Missing property name: " + def );

               new Constant( id, SF_PROPERTY_VALUE, imageId, propertyName );
            }
            else if ( functionId == "property_defined" || functionId == "pty_defined" )
            {
               String imageId, propertyName;
               switch ( valueTokens.Length() )
               {
               case 1:
                  propertyName = StripValueTokenQuotes( valueTokens[0] );
                  break;
               case 2:
                  imageId = ParseValueImageId( valueTokens[0] );
                  propertyName = StripValueTokenQuotes( valueTokens[1] );
                  break;
               case 0:
                  throw ParseError( "In property_defined() symbol function specification: Missing symbol function arguments: " + def );
               default:
                  throw ParseError( "The property_defined() symbol function only takes one or two arguments: " + def );
               }
               if ( propertyName.IsEmpty() )
                  throw ParseError( "In property_defined() symbol function specification: Missing property name: " + def );

               new Constant( id, SF_PROPERTY_DEFINED, imageId, propertyName );
            }
            else if ( functionId == "envvar_value" )
            {
               String varName;
               switch ( valueTokens.Length() )
               {
               case 1:
                  varName = StripValueTokenQuotes( valueTokens[0] );
                  break;
               case 0:
                  throw ParseError( "In envvar_value() symbol function specification: Missing symbol function arguments: " + def );
               default:
                  throw ParseError( "The envvar_value() symbol function takes a single argument: " + def );
               }
               if ( varName.IsEmpty() )
                  throw ParseError( "In envvar_value() symbol function specification: Missing environment variable name: " + def );

               new Constant( id, SF_ENVVAR_VALUE, String()/*imageId*/, varName );
            }
            else if ( functionId == "envvar_defined" )
            {
               String varName;
               switch ( valueTokens.Length() )
               {
               case 1:
                  varName = StripValueTokenQuotes( valueTokens[0] );
                  break;
               case 0:
                  throw ParseError( "In envvar_defined() symbol function specification: Missing symbol function arguments: " + def );
               default:
                  throw ParseError( "The envvar_defined() symbol function takes a single argument: " + def );
               }
               if ( varName.IsEmpty() )
                  throw ParseError( "In envvar_defined() symbol function specification: Missing environment variable name: " + def );

               new Constant( id, SF_ENVVAR_DEFINED, String()/*imageId*/, varName );
            }
            else if ( functionId == "width" )
            {
               String imageId;
               switch ( valueTokens.Length() )
               {
               case 1:
                  imageId = ParseValueImageId( valueTokens[0] );
                  break;
               case 0:
                  break;
               default:
                  throw ParseError( "The width() symbol function only takes zero or one argument(s): " + def );
               }

               new Constant( id, SF_WIDTH, imageId );
            }
            else if ( functionId == "height" )
            {
               String imageId;
               switch ( valueTokens.Length() )
               {
               case 1:
                  imageId = ParseValueImageId( valueTokens[0] );
                  break;
               case 0:
                  break;
               default:
                  throw ParseError( "The height() symbol function only takes zero or one argument(s): " + def );
               }

               new Constant( id, SF_HEIGHT, imageId );
            }
            else if ( functionId == "area" )
            {
               String imageId;
               switch ( valueTokens.Length() )
               {
               case 1:
                  imageId = ParseValueImageId( valueTokens[0] );
                  break;
               case 0:
                  break;
               default:
                  throw ParseError( "The area() symbol function only takes zero or one argument(s): " + def );
               }

               new Constant( id, SF_AREA, imageId );
            }
            else if ( functionId == "invarea" )
            {
               String imageId;
               switch ( valueTokens.Length() )
               {
               case 1:
                  imageId = ParseValueImageId( valueTokens[0] );
                  break;
               case 0:
                  break;
               default:
                  throw ParseError( "The invarea() symbol function only takes zero or one argument(s): " + def );
               }

               new Constant( id, SF_INVAREA, imageId );
            }
            else if ( functionId == "iscolor" )
            {
               String imageId;
               switch ( valueTokens.Length() )
               {
               case 1:
                  imageId = ParseValueImageId( valueTokens[0] );
                  break;
               case 0:
                  break;
               default:
                  throw ParseError( "The iscolor() symbol function only takes zero or one argument(s): " + def );
               }

               new Constant( id, SF_ISCOLOR, imageId );
            }
            else if ( functionId == "maximum" || functionId == "max" )
            {
               NewConstantForImageStatistics( id, SF_MAXIMUM, valueTokens, def );
            }
            else if ( functionId == "minimum" || functionId == "min" )
            {
               NewConstantForImageStatistics( id, SF_MINIMUM, valueTokens, def );
            }
            else if ( functionId == "median" || functionId == "med" )
            {
               NewConstantForImageStatistics( id, SF_MEDIAN, valueTokens, def );
            }
            else if ( functionId == "mdev" )
            {
               NewConstantForImageStatistics( id, SF_MDEV, valueTokens, def );
            }
            else if ( functionId == "adev" )
            {
               NewConstantForImageStatistics( id, SF_ADEV, valueTokens, def );
            }
            else if ( functionId == "mean" )
            {
               NewConstantForImageStatistics( id, SF_MEAN, valueTokens, def );
            }
            else if ( functionId == "sdev" )
            {
               NewConstantForImageStatistics( id, SF_SDEV, valueTokens, def );
            }
            else if ( functionId == "modulus" || functionId == "mod" )
            {
               NewConstantForImageStatistics( id, SF_MODULUS, valueTokens, def );
            }
            else if ( functionId == "ssqr" )
            {
               NewConstantForImageStatistics( id, SF_SSQR, valueTokens, def );
            }
            else if ( functionId == "asqr" )
            {
               NewConstantForImageStatistics( id, SF_ASQR, valueTokens, def );
            }
            else if ( functionId == "pixel" )
            {
               String imageId, xpos, ypos, channel;
               switch ( valueTokens.Length() )
               {
               case 4:
                  channel = StripValueTokenQuotes( valueTokens[3] );
               case 3:
                  imageId = ParseValueImageId( valueTokens[0] );
                  xpos = StripValueTokenQuotes( valueTokens[1] );
                  ypos = StripValueTokenQuotes( valueTokens[2] );
                  break;
               case 2:
                  xpos = StripValueTokenQuotes( valueTokens[0] );
                  ypos = StripValueTokenQuotes( valueTokens[1] );
                  break;
               case 0:
                  throw ParseError( "In pixel() symbol function specification: Missing function arguments: " + def );
               default:
                  throw ParseError( "The pixel() symbol function takes two, three or four arguments: " + def );
               }

               StringList arguments;
               arguments.Append( xpos );
               arguments.Append( ypos );
               arguments.Append( channel );
               new Constant( id, SF_PIXEL, imageId, arguments );
            }
            else if ( functionId == "init" )
            {
               double value = 0;
               switch ( valueTokens.Length() )
               {
               case 1:
                  if ( !valueTokens[0].TryToDouble( value ) )
                     throw ParseError( "In variable initialization: Invalid numeric value '" + valueTokens[0] + "'" );
                  break;
               case 0:
                  break;
               default:
                  throw ParseError( "The variable initialization function can only take a single immediate numeric argument: " + def );
               }

               new Variable( id, value );
            }
            else if ( functionId == "global" )
            {
               bool valueSet = false;
               double value = 0;
               GlobalVariable::global_op op;
               switch ( valueTokens.Length() )
               {
               case 2:
                  if ( !(valueSet = valueTokens[1].TryToDouble( value )) )
                     throw ParseError( "In global variable initialization: Invalid numeric value '" + valueTokens[1] + "'" );
               case 1:
                  if ( valueTokens[0] == '+' )
                  {
                     op = GlobalVariable::Add;
                     if ( !valueSet )
                        value = 0;
                  }
                  else if ( valueTokens[0] == '*' )
                  {
                     op = GlobalVariable::Mul;
                     if ( !valueSet )
                        value = 1;
                  }
                  else
                     throw ParseError( "In global variable initialization: Unknown/unsupported global operator '" + valueTokens[0] + "'" );
                  break;
               case 0:
                  throw ParseError( "In global variable initialization: Missing global operator: " + def );
               default:
                  throw ParseError( "The global variable initialization function can only take one or two argument(s): " + def );
               }

               new GlobalVariable( id, op, value );
            }
            else
            {
               throw ParseError( "Unknown or unsupported symbol function '" + functionId + "': " + def );
            }
         }
         else
         {
            /*
             * Immediate constant definition
             */
            StringList components;
            value.Break( components, ':', true ); // trim tokens
            components.Remove( String() );

            if ( components.Length() != 1 )
               if ( components.Length() < 3 )
                  throw ParseError( "Missing immediate symbol value component(s): " + def );
            if ( components.Length() > 3 )
               throw ParseError( "Too many immediate symbol value components: " + def );

            try
            {
               Pixel valuePixel;
               if ( components.Length() == 1 )
                  valuePixel.SetSamples( components[0].ToDouble() );
               else
                  valuePixel.SetSamples( components[0].ToDouble(), components[1].ToDouble(), components[2].ToDouble() );

               new Constant( id, valuePixel );
            }
            catch ( ParseError& )
            {
               throw ParseError( "Invalid immediate symbol component value - expected a numeric literal: " + def );
            }
         }
      }
   }
}

// ----------------------------------------------------------------------------

String Symbol::GlobalVariablesReport()
{
   String text;
   for ( const Symbol* symbol : m_symbols )
      if ( symbol->IsGlobalVariable() )
      {
         const GlobalVariable* g = static_cast<const GlobalVariable*>( symbol );
         text << g->QualifiedId()
              << " = "
              << g->FinalValue().ToString()
              << '\n';
      }
   return text;
}

// ----------------------------------------------------------------------------

Array<Symbol::OutputData> Symbol::GlobalVariablesData()
{
   Array<Symbol::OutputData> data;
   for ( symbol_set::const_iterator i = m_symbols.Begin(); i != m_symbols.End(); ++i )
      if ( (*i)->IsGlobalVariable() )
         data << *static_cast<const GlobalVariable*>( *i );
   return data;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Variable::Variable( const String& id, double value )
   : Symbol( id )
{
   // Initialize thread local storage (TLS)
   if ( s_numberOfProcessors == 0 )
      s_numberOfProcessors = PixInsightSettings::GlobalInteger( "System/NumberOfProcessors" );
   m_tls = GenericVector<Pixel>( s_numberOfProcessors );
   for ( int i = 0; i < s_numberOfProcessors; ++i )
      m_tls[i].SetSamples( value, value, value );
   SetSamples( value, value, value );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

String Constant::ToString() const
{
   if ( !IsValueFunction() )
      return Pixel::ToString();

   String s = String( s_functionName[m_functionId] ) + '(';
   if ( !m_imageId.IsEmpty() )
      s += m_imageId;
   if ( !m_arguments.IsEmpty() )
   {
      if ( !m_imageId.IsEmpty() )
         s += ", ";
      for ( StringList::const_iterator i = m_arguments.Begin(); i != m_arguments.End(); ++i )
      {
         s += '\'' + *i + '\'';
         if ( i != m_arguments.ReverseBegin() )
            s += ", ";
      }
   }
   s += ")";
   return s;
}

// ----------------------------------------------------------------------------

Expression* Constant::FunctionValue( int tokenPos, const ImageWindow& targetWindow ) const
{
   IsoString imageId( m_imageId );
   ImageWindow window;
   if ( imageId.IsEmpty() )
   {
      window = targetWindow;
      if ( window.IsNull() )
         throw ParseError( "In symbol '" + Id() + "' definition: The " + String( s_functionName[m_functionId] ) + " function needs an explicit image identifier" );
      imageId = window.MainView().Id();
   }
   else
   {
      window = ImageWindow::WindowById( imageId );
      if ( window.IsNull() )
         throw ParseError( "In symbol '" + Id() + "' definition: No such image: '" + imageId + "'" );
   }

   switch ( m_functionId )
   {
   case SF_KEYWORD_VALUE:
      {
         IsoString keywordName( m_arguments[0] );
         double value;
         bool found = false;
         FITSKeywordArray keywords;
         if ( window.GetKeywords( keywords ) )
            for ( FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
               if ( !i->name.CompareIC( keywordName ) )
               {
                  IsoString s = i->StripValueDelimiters();
                  if ( s.IsNumeral() )
                  {
                     if ( !s.TryToDouble( value ) )
                        throw ParseError( "In symbol '" + Id() + "' definition: Invalid numeric keyword value: '" + keywordName + "'" );
                     found = true;
                  }
                  else if ( s == 'F' || s == 'f' )
                  {
                     value = 0;
                     found = true;
                  }
                  else if ( s == 'T' || s == 't' )
                  {
                     value = 1;
                     found = true;
                  }
                  else
                  {
                     throw ParseError( "In symbol '" + Id() + "' definition: Invalid keyword type: '" + keywordName + "'" );
                  }

                  break;
               }

         if ( !found )
            throw ParseError( "In symbol '" + Id() + "' definition: No keyword '" + keywordName + "' defined for image '" + imageId + "'" );

         return new Sample( value, tokenPos );
      }
   case SF_KEYWORD_DEFINED:
      {
         IsoString keywordName( m_arguments[0] );
         bool found = false;
         FITSKeywordArray keywords;
         if ( window.GetKeywords( keywords ) )
            for ( FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
               if ( !i->name.CompareIC( keywordName ) )
               {
                  found = true;
                  break;
               }

         return new Sample( found ? 1.0 : 0.0, tokenPos );
      }
   case SF_PROPERTY_VALUE:
      {
         IsoString propertyName( m_arguments[0] );
         View view = window.MainView();
         Variant value = view.ComputeOrFetchProperty( propertyName, false/*notify*/ );
         if ( !value.IsValid() )
            throw ParseError( "In symbol '" + Id() + "' definition: Property '" + propertyName + "' cannot be computed or retrieved for image '" + imageId + "'" );
         try
         {
            DVector v = value.ToDVector();
            if ( v.Length() < 3 )
               return new Sample( v[0], tokenPos );
            return new PixelData( Pixel( v[0], v[1], v[2] ), tokenPos );
         }
         catch ( Exception& )
         {
            throw ParseError( "In symbol '" + Id() + "' definition: Property '" + propertyName + "' cannot be converted to a vector or scalar type" );
         }
      }
   case SF_PROPERTY_DEFINED:
      {
         View view = window.MainView();
         return new Sample( view.HasProperty( m_arguments[0] ) ? 1.0 : 0.0, tokenPos );
      }
   case SF_ENVVAR_VALUE:
      {
         IsoString varName( m_arguments[0] );
         IsoString varValue( ::getenv( varName.c_str() ) );
         if ( varValue.IsEmpty() )
            throw ParseError( "In symbol '" + Id() + "' definition: No '" + varName + "' environment variable has been defined for the running process" );
         double value;
         if ( !varValue.TryToDouble( value ) )
            throw ParseError( "In symbol '" + Id() + "' definition: The value of the '" + varName + "' environment variable cannot be converted to a scalar type" );

         return new Sample( value, tokenPos );
      }
   case SF_ENVVAR_DEFINED:
      {
         IsoString varName( m_arguments[0] );
         return new Sample( IsoString( ::getenv( varName.c_str() ) ).Trimmed().IsEmpty() ? 0.0 : 1.0, tokenPos );
      }
   case SF_WIDTH:
      return new Sample( window.MainView().Image().Width(), tokenPos );
   case SF_HEIGHT:
      return new Sample( window.MainView().Image().Height(), tokenPos );
   case SF_AREA:
      return new Sample( window.MainView().Image().NumberOfPixels(), tokenPos );
   case SF_INVAREA:
      return new Sample( 1.0/window.MainView().Image().NumberOfPixels(), tokenPos );
   case SF_ISCOLOR:
      return new Sample( window.MainView().Image().IsColor() ? 1.0 : 0.0, tokenPos );
   case SF_MAXIMUM:
      {
         View view = window.MainView();
         DVector maximum = view.ComputeOrFetchProperty( "Maximum", false/*notify*/ ).ToDVector();
         return FunctionValue( maximum, tokenPos );
      }
   case SF_MINIMUM:
      {
         View view = window.MainView();
         DVector minimum = view.ComputeOrFetchProperty( "Minimum", false/*notify*/ ).ToDVector();
         return FunctionValue( minimum, tokenPos );
      }
   case SF_MEDIAN:
      {
         View view = window.MainView();
         DVector median = view.ComputeOrFetchProperty( "Median", false/*notify*/ ).ToDVector();
         return FunctionValue( median, tokenPos );
      }
   case SF_MEAN:
      {
         View view = window.MainView();
         DVector mean = view.ComputeOrFetchProperty( "Mean", false/*notify*/ ).ToDVector();
         return FunctionValue( mean, tokenPos );
      }
   case SF_MDEV:
      {
         View view = window.MainView();
         DVector mdev = view.ComputeOrFetchProperty( "MAD", false/*notify*/ ).ToDVector() * 1.4826;
         return FunctionValue( mdev, tokenPos );
      }
   case SF_ADEV:
      {
         View view = window.MainView();
         DVector adev = view.ComputeOrFetchProperty( "AvgDev", false/*notify*/ ).ToDVector() * 1.2533;
         return FunctionValue( adev, tokenPos );
      }
   case SF_SDEV:
      {
         View view = window.MainView();
         DVector sdev = view.ComputeOrFetchProperty( "StdDev", false/*notify*/ ).ToDVector();
         return FunctionValue( sdev, tokenPos );
      }
   case SF_MODULUS:
      {
         View view = window.MainView();
         DVector modulus = view.ComputeOrFetchProperty( "Modulus", false/*notify*/ ).ToDVector();
         return FunctionValue( modulus, tokenPos );
      }
   case SF_SSQR:
      {
         View view = window.MainView();
         DVector ssqr = view.ComputeOrFetchProperty( "SumOfSquares", false/*notify*/ ).ToDVector();
         return FunctionValue( ssqr, tokenPos );
      }
   case SF_ASQR:
      {
         View view = window.MainView();
         DVector msqr = view.ComputeOrFetchProperty( "MeanOfSquares", false/*notify*/ ).ToDVector();
         return FunctionValue( msqr, tokenPos );
      }
   case SF_PIXEL:
      {
         int xpos, ypos, channel = -1;
         if ( !m_arguments[0].TryToInt( xpos ) ||
            !m_arguments[1].TryToInt( ypos ) )
            throw ParseError( "In symbol '" + Id() + "' definition: Invalid pixel coordinates '" + m_arguments[0] + "," + m_arguments[1] + "'" );
         if ( m_arguments.Length() > 2 )
            if ( !m_arguments[2].IsEmpty() )
               if ( !m_arguments[2].TryToInt( channel ) || channel < 0 )
                  throw ParseError( "In symbol '" + Id() + "' definition: Invalid channel index '[" + m_arguments[2] + "]'" );

         const ImageVariant image = window.MainView().Image();

         if ( channel >= 0 ) // i has been incremented two times
            if ( channel >= image->NumberOfChannels() )
               throw ParseError( "In symbol '" + Id() + "' definition: Channel index '[" + m_arguments[2] + "]' out of range" );

         if ( image->Includes( xpos, ypos ) )
         {
            if ( channel < 0 && !image->IsColor() )
               channel = 0;
            if ( channel < 0 )
               return new PixelData( Pixel( image( xpos, ypos, 0 ), image( xpos, ypos, 1 ), image( xpos, ypos, 2 ) ), tokenPos );

            return new Sample( image( xpos, ypos, channel ), tokenPos );
         }

         return new Sample( 0, tokenPos );
      }
   default:
      throw ParseError( "Constant::FunctionValue(): Internal error" );
   }
}

// ----------------------------------------------------------------------------

Expression* Constant::FunctionValue( const DVector& data, int tokenPos ) const
{
   int channel = -1;
   if ( m_arguments.Length() > 0 )
      if ( !m_arguments[0].IsEmpty() )
         if ( !m_arguments[0].TryToInt( channel ) )
            throw ParseError( "In symbol '" + Id() + "' definition: Invalid channel index '[" + m_arguments[0] + "]'" );
   if ( channel >= 0 )
   {
      if ( channel >= int( data.Length() ) )
         throw ParseError( "In symbol '" + Id() + "' definition: Channel index '[" + m_arguments[0] + "]' out of range" );
      return new Sample( data[channel], tokenPos );
   }
   if ( data.Length() < 3 )
      return new Sample( data[0], tokenPos );
   return new PixelData( Pixel( data[0], data[1], data[2] ), tokenPos );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Symbol.cpp - Released 2021-05-05T15:38:07Z
