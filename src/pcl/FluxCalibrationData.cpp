//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// pcl/FluxCalibrationData.cpp - Released 2024-12-23T11:33:03Z
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

#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/FluxCalibrationData.h>
#include <pcl/XML.h>

#include <pcl/api/APIInterface.h>

#include <errno.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void FluxCalibrationData::Clear()
{
   m_targetFilePath.Clear();
   m_fluxScaleFactors = m_fluxScaleSigmas = Vector();
   m_fwhmx = m_fwhmy = FVector();
   m_counts = IVector();
   m_normalizationFactor = __PCL_SPFC_NORMALIZATION_FACTOR;
   m_version = 0;
   m_creationTime = TimePoint();
}

// ----------------------------------------------------------------------------

XMLDocument* FluxCalibrationData::Serialize() const
{
   // Validate image registration data
   if ( m_targetFilePath.IsEmpty() )
      throw Error( "No target file path has been specified." );

   if ( m_fluxScaleFactors.IsEmpty() || m_fluxScaleSigmas.IsEmpty() )
      throw Error( "Insufficient flux calibration data." );

   if ( m_fluxScaleFactors.Length() != m_fluxScaleSigmas.Length() ||
        !m_fwhmx.IsEmpty() && m_fwhmx.Length() != m_fluxScaleFactors.Length() ||
        !m_fwhmy.IsEmpty() && m_fwhmy.Length() != m_fluxScaleFactors.Length() ||
        !m_counts.IsEmpty() && m_counts.Length() != m_fluxScaleFactors.Length() )
      throw Error( "Incongruent vector lengths." );

   if ( 1 + m_normalizationFactor == 1 )
      throw Error( "Zero or insignificant scale normalization factor." );

   AutoPointer<XMLDocument> xml = new XMLDocument;
   xml->SetXML( "1.0", "UTF-8" );
   *xml << new XMLComment( "\nPixInsight XML Flux Calibration Data Format - XFCL version 1.0"
                           "\nCreated with PixInsight software - http://pixinsight.com/"
                           "\n" );

   XMLElement* root = new XMLElement( "xfcl", XMLAttributeList()
      << XMLAttribute( "version", "1.0" )
      << XMLAttribute( "xmlns", "http://www.pixinsight.com/xfcl" )
      << XMLAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" )
      << XMLAttribute( "xsi:schemaLocation", "http://www.pixinsight.com/xfcl http://pixinsight.com/xfcl/xfcl-1.0.xsd" ) );

   xml->SetRootElement( root );

   *(new XMLElement( *root, "CreationTime" )) << new XMLText( TimePoint::Now().ToString() );
   if ( m_version != 0 )
      *(new XMLElement( *root, "Version" )) << new XMLText( String( m_version ) );

   *(new XMLElement( *root, "TargetImage" )) << new XMLText( m_targetFilePath );

   *(new XMLElement( *root, "FluxScaleFactors" )) << new XMLText( String().ToCommaSeparated( m_fluxScaleFactors ) );
   *(new XMLElement( *root, "FluxScaleSigmas" )) << new XMLText( String().ToCommaSeparated( m_fluxScaleSigmas ) );
   *(new XMLElement( *root, "NormalizationFactor" )) << new XMLText( String( m_normalizationFactor ) );

   if ( !m_fwhmx.IsEmpty() )
      *(new XMLElement( *root, "FWHMx" )) << new XMLText( String().ToCommaSeparated( m_fwhmx ) );
   if ( !m_fwhmy.IsEmpty() )
      *(new XMLElement( *root, "FWHMy" )) << new XMLText( String().ToCommaSeparated( m_fwhmy ) );

   if ( !m_counts.IsEmpty() )
      *(new XMLElement( *root, "StarCounts" )) << new XMLText( String().ToCommaSeparated( m_counts ) );

   return xml.Release();
}

// ----------------------------------------------------------------------------

void FluxCalibrationData::SerializeToFile( const String& path ) const
{
   AutoPointer<XMLDocument> xml = Serialize();
   xml->EnableAutoFormatting();
   xml->SetIndentSize( 3 );
   xml->SerializeToFile( path );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static void WarnOnUnexpectedChildNode( const XMLNode& node, const String& parsingWhatElement )
{
   if ( !node.IsComment() )
   {
      XMLParseError e( node,
            "Parsing " + parsingWhatElement + " element",
            "Ignoring unexpected XML child node of " + XMLNodeType::AsString( node.NodeType() ) + " type." );
      Console().WarningLn( "<end><cbr>** Warning: " + e.Message() );
   }
}

static void WarnOnUnknownChildElement( const XMLElement& element, const String& parsingWhatElement )
{
   XMLParseError e( element,
         "Parsing " + parsingWhatElement + " element",
         "Skipping unknown \'" + element.Name() + "\' child element." );
   Console().WarningLn( "<end><cbr>** Warning: " + e.Message() );
}

// ----------------------------------------------------------------------------

static bool TryToDouble( double& value, IsoString::const_iterator p )
{
   IsoString::iterator endptr = nullptr;
   errno = 0;
   double val = ::strtod( p, &endptr );
   if ( errno == 0 && (endptr == nullptr || *endptr == '\0') )
   {
      value = val;
      return true;
   }
   return false;
}

template <class V>
static void ParseListOfRealValues( V& vector, IsoString& text, size_type start, size_type end, size_type maxCount = 256 )
{
   Array<typename V::component> v;
   for ( size_type i = start, j; i < end; ++i )
   {
      for ( j = i; j < end; ++j )
         if ( text[j] == ',' )
            break;
      text[j] = '\0';
      double x;
      if ( !TryToDouble( x, text.At( i ) ) )
         throw Error( "Parsing real numeric list: Invalid floating point numeric literal \'" + IsoString( text.At( i ) ) + "\' at offset " + IsoString( start ) );
      if ( v.Length() == maxCount )
         throw Error( "Parsing real numeric list: Too many items." );
      v << typename V::component( x );
      i = j;
   }
   vector = V( v.Begin(), int( v.Length() ) );
}

template <class V>
static void ParseListOfRealValues( V& vector, const XMLElement& element )
{
   IsoString text = IsoString( element.Text().Trimmed() );
   ParseListOfRealValues( vector, text, 0, text.Length() );
}

static bool TryToInt( int& value, IsoString::const_iterator p )
{
   IsoString::iterator endptr = nullptr;
   errno = 0;
   long val = ::strtol( p, &endptr, 0 );
   if ( errno == 0 && (endptr == nullptr || *endptr == '\0') )
   {
      value = int( val );
      return true;
   }
   return false;
}

static void ParseListOfIntegerValues( IVector& vector, IsoString& text, size_type start, size_type end, size_type maxCount = 256 )
{
   Array<int> v;
   for ( size_type i = start, j; i < end; ++i )
   {
      for ( j = i; j < end; ++j )
         if ( text[j] == ',' )
            break;
      text[j] = '\0';
      int x;
      if ( !TryToInt( x, text.At( i ) ) )
         throw Error( "Parsing integer numeric list: Invalid integer numeric literal \'" + IsoString( text.At( i ) ) + "\' at offset " + IsoString( start ) );
      if ( v.Length() == maxCount )
         throw Error( "Parsing integer numeric list: Too many items." );
      v << x;
      i = j;
   }
   vector = IVector( v.Begin(), int( v.Length() ) );
}

static void ParseListOfIntegerValues( IVector& vector, const XMLElement& element )
{
   IsoString text = IsoString( element.Text().Trimmed() );
   ParseListOfIntegerValues( vector, text, 0, text.Length() );
}

// ----------------------------------------------------------------------------

void FluxCalibrationData::Parse( const String& filePath )
{
   IsoString text = File::ReadTextFile( filePath );
   XMLDocument xml;
   xml.SetParserOption( XMLParserOption::IgnoreComments );
   xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
   xml.Parse( text.UTF8ToUTF16() );
   Parse( xml );
}

// ----------------------------------------------------------------------------

void FluxCalibrationData::Parse( const XMLDocument& xml )
{
   if ( xml.RootElement() == nullptr )
      throw Error( "The XML document has no root element." );
   if ( xml.RootElement()->Name() != "xfcl" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
      throw Error( "Not an XFCL version 1.0 document." );
   Parse( *xml.RootElement() );
}

// ----------------------------------------------------------------------------

void FluxCalibrationData::Parse( const XMLElement& root )
{
   Clear();

   for ( const XMLNode& node : root )
   {
      if ( !node.IsElement() )
      {
         WarnOnUnexpectedChildNode( node, "xfcl root" );
         continue;
      }

      const XMLElement& element = static_cast<const XMLElement&>( node );

      try
      {
         if ( element.Name() == "TargetImage" )
            m_targetFilePath = element.Text().Trimmed();
         else if ( element.Name() == "FluxScaleFactors" )
            ParseListOfRealValues( m_fluxScaleFactors, element );
         else if ( element.Name() == "FluxScaleSigmas" )
            ParseListOfRealValues( m_fluxScaleSigmas, element );
         else if ( element.Name() == "NormalizationFactor" )
            m_normalizationFactor = element.Text().Trimmed().ToDouble();
         else if ( element.Name() == "FWHMx" )
            ParseListOfRealValues( m_fwhmx, element );
         else if ( element.Name() == "FWHMy" )
            ParseListOfRealValues( m_fwhmy, element );
         else if ( element.Name() == "StarCounts" )
            ParseListOfIntegerValues( m_counts, element );
         else if ( element.Name() == "Version" )
            m_version = element.Text().Trimmed().ToUInt();
         else if ( element.Name() == "CreationTime" )
            m_creationTime = TimePoint( element.Text().Trimmed() );
         else
            WarnOnUnknownChildElement( element, "xfcl root" );
      }
      catch ( Exception& x )
      {
         try
         {
            throw XMLParseError( element, "Parsing " + element.Name() + " element", x.Message() );
         }
         catch ( Exception& x )
         {
            x.Show();
         }
      }
      catch ( ... )
      {
         throw;
      }
   }

   if ( m_targetFilePath.IsEmpty() )
      throw Error( "Missing or invalid TargetImage element." );

   if ( m_fluxScaleFactors.IsEmpty() )
      throw Error( "Missing or invalid FluxScaleFactors element." );

   if ( m_fluxScaleSigmas.IsEmpty() )
      throw Error( "Missing or invalid FluxScaleSigmas element." );

   if ( m_fluxScaleFactors.Length() != m_fluxScaleSigmas.Length() )
      throw Error( "Incongruent scale vector lengths." );

   if ( !m_fwhmx.IsEmpty() )
      if ( m_fwhmx.Length() != m_fluxScaleFactors.Length() )
         throw Error( "Incongruent FWHMx vector length." );

   if ( !m_fwhmy.IsEmpty() )
      if ( m_fwhmy.Length() != m_fluxScaleFactors.Length() )
         throw Error( "Incongruent FWHMy vector length." );

   if ( !m_counts.IsEmpty() )
      if ( m_counts.Length() != m_fluxScaleFactors.Length() )
         throw Error( "Incongruent StarCounts vector length." );

   if ( 1 + m_normalizationFactor == 1 )
      throw Error( "Zero or insignificant scale normalization factor." );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FluxCalibrationData.cpp - Released 2024-12-23T11:33:03Z
