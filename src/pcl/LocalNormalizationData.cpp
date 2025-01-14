//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/LocalNormalizationData.cpp - Released 2024-12-28T16:53:56Z
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
#include <pcl/Compression.h>
#include <pcl/Console.h>
#include <pcl/LocalNormalizationData.h>
#include <pcl/XML.h>

#define MIN_NORMALIZATION_SCALE  16

namespace pcl
{

// ----------------------------------------------------------------------------

void LocalNormalizationData::InitInterpolations()
{
   if ( m_A.IsEmpty() ||
        m_B.IsEmpty() ||
        m_A.Bounds() != m_B.Bounds() ||
        m_A.NumberOfChannels() != m_B.NumberOfChannels() ||
        !m_C.IsEmpty() && m_C.Length() != m_B.NumberOfChannels() ||
        m_referenceWidth < m_A.Width() ||
        m_referenceHeight < m_A.Height() )
      throw Error( "LocalNormalizationData::InitInterpolations(): Uninitialized or invalid local normalization data." );

   m_sx = double( m_A.Width() )/m_referenceWidth;
   m_sy = double( m_A.Height() )/m_referenceHeight;
   m_UA.Clear();
   m_UB.Clear();
   for ( int c = 0; c < m_A.NumberOfChannels(); ++c )
   {
      matrix_interpolation A;
      A.Initialize( m_A[c], m_A.Width(), m_A.Height() );
      m_UA << A;
      matrix_interpolation B;
      B.Initialize( m_B[c], m_B.Width(), m_B.Height() );
      m_UB << B;
   }
   if ( m_C.IsEmpty() )
      m_C = Vector( 0.0, m_A.NumberOfChannels() );
}

// ----------------------------------------------------------------------------

void LocalNormalizationData::Clear()
{
   m_referenceFilePath.Clear();
   m_targetFilePath.Clear();
   m_scale = m_referenceWidth = m_referenceHeight = -1;
   m_A.FreeData();
   m_B.FreeData();
   m_C.Clear();
   m_UA.Clear();
   m_UB.Clear();
   m_Rc.Clear();
   m_Tc.Clear();
   m_S.Clear();
   m_relScale.Clear();
   m_sx = m_sy = 0;
   m_creationTime = TimePoint();
   m_version = 0;
   m_invalid = false;
}

// ----------------------------------------------------------------------------

void LocalNormalizationData::Parse( const String& filePath, bool ignoreNormalizationData )
{
   IsoString text = File::ReadTextFile( filePath );
   if ( text.IsEmpty() )
      throw Error( "Empty normalization data file." );

   XMLDocument xml;
   xml.SetParserOption( XMLParserOption::IgnoreComments );
   xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
   xml.Parse( text.UTF8ToUTF16() );
   Parse( xml, ignoreNormalizationData );
}

// ----------------------------------------------------------------------------

void LocalNormalizationData::Parse( const XMLDocument& xml, bool ignoreNormalizationData )
{
   if ( xml.RootElement() == nullptr )
      throw Error( "The XML document has no root element." );
   if ( xml.RootElement()->Name() != "xnml" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
      throw Error( "Not an XNML version 1.0 document." );
   Parse( *xml.RootElement(), ignoreNormalizationData );
}

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

static Vector ParseVector( const XMLElement& element, size_type maxCount = 256 )
{
   IsoString text = IsoString( element.Text().Trimmed() );
   Array<double> v;
   for ( size_type i = 0, j, end = text.Length(); i < end; ++i )
   {
      for ( j = i; j < end; ++j )
         if ( text[j] == ',' )
            break;
      text[j] = '\0';
      double x;
      if ( !TryToDouble( x, text.At( i ) ) )
         throw Error( "Parsing vector components: Invalid floating point numeric literal \'" + IsoString( text.At( i ) ) + "\'" );
      if ( v.Length() == maxCount )
         throw Error( "Parsing vector components: Too many items." );
      v << x;
      i = j;
   }
   if ( v.Length() < 1 )
      throw Error( "Parsing vector: Empty vector." );
   return Vector( v.Begin(), int( v.Length() ) );
}

void LocalNormalizationData::Parse( const XMLElement& root, bool ignoreNormalizationData )
{
   Clear();

   {
      String valid = root.AttributeValue( "valid" );
      if ( !valid.IsEmpty() )
         m_invalid = !valid.ToBool();
      if ( m_invalid )
         ignoreNormalizationData = true;
   }

   for ( const XMLNode& node : root )
   {
      if ( !node.IsElement() )
      {
         WarnOnUnexpectedChildNode( node, "xnml root" );
         continue;
      }

      const XMLElement& element = static_cast<const XMLElement&>( node );

      try
      {
         if ( element.Name() == "ReferenceImage" )
         {
            // optional
            m_referenceFilePath = element.Text().Trimmed();
         }
         else if ( element.Name() == "TargetImage" )
         {
            // optional
            m_targetFilePath = element.Text().Trimmed();
         }
         else if ( element.Name() == "ReferenceGeometry" )
         {
            String width = element.AttributeValue( "width" );
            String height = element.AttributeValue( "height" );
            if ( width.IsEmpty() || height.IsEmpty() )
               throw Error( "Missing reference dimension attribute(s)." );
            m_referenceWidth = width.ToInt();
            m_referenceHeight = height.ToInt();
            if ( m_referenceWidth < 1 || m_referenceHeight < 1 )
               throw Error( "Invalid reference dimension(s)." );
         }
         else if ( element.Name() == "LocalNormalization" )
         {
            // The normalization scale and data version are always reported.
            String scale = element.AttributeValue( "scale" );
            if ( scale.IsEmpty() )
               throw Error( "Missing local normalization scale attribute." );
            m_scale = scale.ToInt();
            if ( m_scale < MIN_NORMALIZATION_SCALE )
               throw Error( "Invalid local normalization scale attribute value '" + scale + '\'' );
            String version = element.AttributeValue( "version" );
            if ( !version.IsEmpty() )
               m_version = version.ToInt();

            if ( !ignoreNormalizationData )
            {
               for ( const XMLNode& node : element )
               {
                  if ( !node.IsElement() )
                  {
                     WarnOnUnexpectedChildNode( node, "LocalNormalization" );
                     continue;
                  }

                  const XMLElement& element = static_cast<const XMLElement&>( node );

                  if ( element.Name() == "Scale" )
                     ParseNormalizationMatrices( m_A, element );
                  else if ( element.Name() == "ZeroOffset" )
                     ParseNormalizationMatrices( m_B, element );
                  else if ( element.Name() == "Bias" )
                     m_C = ParseVector( element );
                  else
                     WarnOnUnknownChildElement( element, "LocalNormalization" );
               }

               if ( m_A.IsEmpty() )
                  throw Error( "Missing local normalization scale matrices." );
               if ( m_B.IsEmpty() )
                  throw Error( "Missing local normalization zero offset matrices." );
               if ( !m_C.IsEmpty() )
                  if ( m_C.Length() != m_A.NumberOfChannels() )
                     throw Error( "Wrong number of bias vector components." );
            }
         }
         else if ( element.Name() == "GlobalNormalization" )
         {
            if ( !ignoreNormalizationData )
            {
               for ( const XMLNode& node : element )
               {
                  if ( !node.IsElement() )
                  {
                     WarnOnUnexpectedChildNode( node, "GlobalNormalization" );
                     continue;
                  }

                  const XMLElement& element = static_cast<const XMLElement&>( node );

                  if ( element.Name() == "Scale" )
                     m_S = ParseVector( element );
                  else if ( element.Name() == "ReferenceLocation" )
                     m_Rc = ParseVector( element );
                  else if ( element.Name() == "TargetLocation" )
                     m_Tc = ParseVector( element );
                  else
                     WarnOnUnknownChildElement( element, "GlobalNormalization" );
               }

               if ( m_S.IsEmpty() )
                  throw Error( "Missing global normalization scale vector." );
               if ( m_Rc.IsEmpty() )
                  throw Error( "Missing global normalization reference location vector." );
               if ( m_Tc.IsEmpty() )
                  throw Error( "Missing global normalization target location vector." );
            }
         }
         else if ( element.Name() == "RelativeScaleFactors" )
         {
            m_relScale = ParseVector( element );
         }
         else if ( element.Name() == "CreationTime" )
         {
            m_creationTime = TimePoint( element.Text().Trimmed() );
         }
         else
         {
            WarnOnUnknownChildElement( element, "xnml root" );
         }
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

   if ( m_referenceWidth < 0 || m_referenceHeight < 0 )
      throw Error( "Missing required ReferenceGeometry element." );

   if ( !ignoreNormalizationData )
   {
      if ( m_A.IsEmpty() || m_B.IsEmpty() )
         throw Error( "Missing required LocalNormalization element." );

      if ( m_A.Bounds() != m_B.Bounds() || m_A.NumberOfChannels() != m_B.NumberOfChannels() )
         throw Error( "Incongruent local normalization matrices." );

      if ( m_referenceWidth < m_scale || m_referenceHeight < m_scale ||
         m_referenceWidth < m_A.Width() || m_referenceHeight < m_A.Height() )
         throw Error( "Invalid reference dimensions." );

      if ( !m_S.IsEmpty() )
         if ( m_S.Length() != m_B.NumberOfChannels() ||
              m_Rc.Length() != m_B.NumberOfChannels() ||
              m_Tc.Length() != m_B.NumberOfChannels() )
            throw Error( "Incongruent global normalization vectors." );

      if ( !m_relScale.IsEmpty() )
         if ( m_relScale.Length() != m_B.NumberOfChannels() )
            throw Error( "Incongruent relative scale vector." );

      InitInterpolations();
   }
}

// ----------------------------------------------------------------------------

XMLDocument* LocalNormalizationData::Serialize() const
{
   // Validate data
   if ( m_scale < MIN_NORMALIZATION_SCALE ||
        m_referenceWidth < m_scale ||
        m_referenceHeight < m_scale ||
        !m_invalid &&
         (
            m_A.IsEmpty() ||
            m_B.IsEmpty() ||
            m_A.Bounds() != m_B.Bounds() ||
            m_A.NumberOfChannels() != m_B.NumberOfChannels() ||
            !m_C.IsEmpty() && m_C.Length() != m_B.NumberOfChannels() ||
            m_referenceWidth < m_A.Width() ||
            m_referenceHeight < m_A.Height()
         ) )
      throw Error( "LocalNormalizationData::Serialize(): Uninitialized or invalid local normalization data." );

   AutoPointer<XMLDocument> xml = new XMLDocument;
   xml->SetXML( "1.0", "UTF-8" );
   *xml << new XMLComment( "\nPixInsight XML Local Normalization Data Format - XNML version 1.0"
                           "\nCreated with PixInsight software - http://pixinsight.com/"
                           "\n" );

   XMLElement* root = new XMLElement( "xnml", XMLAttributeList()
      << XMLAttribute( "version", "1.0" )
      << (m_invalid ? XMLAttribute( "valid", "false" ) : XMLAttribute())
      << XMLAttribute( "xmlns", "http://www.pixinsight.com/xnml" )
      << XMLAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" )
      << XMLAttribute( "xsi:schemaLocation", "http://www.pixinsight.com/xnml http://pixinsight.com/xnml/xnml-1.0.xsd" ) );

   xml->SetRootElement( root );

   *(new XMLElement( *root, "CreationTime" )) << new XMLText( TimePoint::Now().ToString() );

   *(new XMLElement( *root, "ReferenceImage" )) << new XMLText( m_referenceFilePath );

   *(new XMLElement( *root, "TargetImage" )) << new XMLText( m_targetFilePath );

   new XMLElement( *root, "ReferenceGeometry", XMLAttributeList()
      << XMLAttribute( "width", String( m_referenceWidth ) )
      << XMLAttribute( "height", String( m_referenceHeight ) )
      << XMLAttribute( "numberOfChannels", String( m_A.NumberOfChannels() ) ) );

   XMLElement* ln = new XMLElement( *root, "LocalNormalization", XMLAttributeList()
      << XMLAttribute( "scale", String( m_scale ) )
      << XMLAttribute( "version", String( Max( 1, m_version ) ) ) );

   if ( !m_invalid )
   {
      SerializeNormalizationMatrices( new XMLElement( *ln, "Scale" ), m_A );
      SerializeNormalizationMatrices( new XMLElement( *ln, "ZeroOffset" ), m_B );
      if ( !m_C.IsEmpty() )
         *(new XMLElement( *ln, "Bias" )) << new XMLText( String().ToCommaSeparated( m_C ) );

      if ( !m_S.IsEmpty() && !m_Rc.IsEmpty() && !m_Tc.IsEmpty() )
      {
         XMLElement* gn = new XMLElement( *root, "GlobalNormalization" );

         *(new XMLElement( *gn, "Scale" )) << new XMLText( String().ToCommaSeparated( m_S ) );
         *(new XMLElement( *gn, "ReferenceLocation" )) << new XMLText( String().ToCommaSeparated( m_Rc ) );
         *(new XMLElement( *gn, "TargetLocation" )) << new XMLText( String().ToCommaSeparated( m_Tc ) );
      }

      if ( !m_relScale.IsEmpty() )
         *(new XMLElement( *root, "RelativeScaleFactors" )) << new XMLText( String().ToCommaSeparated( m_relScale ) );
   }

   return xml.Release();
}

// ----------------------------------------------------------------------------

void LocalNormalizationData::SerializeToFile( const String& path ) const
{
   AutoPointer<XMLDocument> xml = Serialize();
   xml->EnableAutoFormatting();
   xml->SetIndentSize( 3 );
   xml->SerializeToFile( path );
}

// ----------------------------------------------------------------------------

void LocalNormalizationData::ParseNormalizationMatrices( normalization_matrices& M, const XMLElement& root ) const
{
   String s = root.AttributeValue( "width" );
   if ( s.IsEmpty() )
      throw Error( "Missing " + root.Name() + " width attribute." );
   int width = s.ToInt();
   if ( width < 1 )
      throw Error( "Invalid " + root.Name() + " width attribute value '" + s + '\'' );

   s = root.AttributeValue( "height" );
   if ( s.IsEmpty() )
      throw Error( "Missing " + root.Name() + " height attribute." );
   int height = s.ToInt();
   if ( height < 1 )
      throw Error( "Invalid " + root.Name() + " height attribute value '" + s + '\'' );

   s = root.AttributeValue( "numberOfChannels" );
   if ( s.IsEmpty() )
      throw Error( "Missing " + root.Name() + " numberOfChannels attribute." );
   int numberOfChannels = s.ToInt();
   if ( numberOfChannels < 1 )
      throw Error( "Invalid " + root.Name() + " numberOfChannels attribute value '" + s + '\'' );

   bool fp64 = false;
   s = root.AttributeValue( "sampleFormat" );
   if ( !s.IsEmpty() )
      if ( s == "Float64" )
         fp64 = true;
      else if ( s != "Float32" )
         throw Error( "Invalid or unsupported " + root.Name() + " sampleFormat attribute value '" + s + '\'' );

   M.AllocateData( width, height, numberOfChannels );

   int channel = 0;

   for ( const XMLNode& node : root )
   {
      if ( !node.IsElement() )
      {
         WarnOnUnexpectedChildNode( node, root.Name() );
         continue;
      }

      const XMLElement& element = static_cast<const XMLElement&>( node );

      if ( element.Name() == "ChannelData" )
      {
         if ( channel == numberOfChannels )
            throw Error( "Unexpected ChannelData child element - all normalization function channels are already defined." );

         ByteArray channelData;

         String compressionName = element.AttributeValue( "compression" ).CaseFolded();
         if ( !compressionName.IsEmpty() )
         {
            AutoPointer<Compression> compression;
            if ( compressionName == "lz4" || compressionName == "lz4+sh" )
               compression = new LZ4Compression;
            else if ( compressionName == "lz4hc" || compressionName == "lz4hc+sh" )
               compression = new LZ4HCCompression;
            else if ( compressionName == "zlib" || compressionName == "zlib+sh" )
               compression = new ZLibCompression;
            else if ( compressionName == "zstd" || compressionName == "zstd+sh" )
               compression = new ZstdCompression;
            else
               throw Error( "Unknown or unsupported compression codec '" + compressionName + '\'' );

            if ( compressionName.EndsWith( "+sh" ) )
            {
               compression->EnableByteShuffling();
               compression->SetItemSize( M.BytesPerSample() );
            }

            Compression::subblock_list subblocks;

            for ( const XMLNode& node : element )
            {
               if ( !node.IsElement() )
               {
                  WarnOnUnexpectedChildNode( node, "ChannelData" );
                  continue;
               }

               const XMLElement& subElement = static_cast<const XMLElement&>( node );

               if ( subElement.Name() == "Subblock" )
               {
                  Compression::Subblock subblock;
                  s = subElement.AttributeValue( "uncompressedSize" );
                  if ( s.IsEmpty() )
                     throw Error( "Missing subblock uncompressedSize attribute." );
                  subblock.uncompressedSize = s.ToUInt64();
                  subblock.compressedData = IsoString( subElement.Text().Trimmed() ).FromBase64();
                  subblocks << subblock;
               }
               else
               {
                  WarnOnUnknownChildElement( element, "ChannelData" );
               }
            }

            if ( subblocks.IsEmpty() )
               throw Error( "Parsing xnml " + root.Name() + " ChannelData element: Missing Subblock child element(s)." );

            channelData = compression->Uncompress( subblocks );
         }
         else
         {
            channelData = IsoString( element.Text().Trimmed() ).FromBase64();
         }

         if ( channelData.Size() == M.ChannelSize() )
         {
            ::memcpy( M[channel], channelData.Begin(), channelData.Size() );
         }
         else if ( fp64 && channelData.Size() == sizeof( double )/normalization_matrices::BytesPerSample()*M.ChannelSize() )
         {
            normalization_matrices::sample_iterator j( M, channel );
            for ( ByteArray::const_iterator i = channelData.Begin(); i != channelData.End(); i += sizeof( double ), ++j )
               *j = normalization_matrices::sample( *reinterpret_cast<const double*>( i ) );
         }
         else
         {
            throw Error( "Parsing xnml " + root.Name() + " ChannelData element: Invalid channel data size: "
               "Expected " + String( M.ChannelSize() * (fp64 ? 2 : 1) ) + " bytes, "
               "got " + String( channelData.Size() ) + " bytes." );
         }

         ++channel;
      }
      else
      {
         WarnOnUnknownChildElement( element, root.Name() );
      }
   }

   if ( channel < numberOfChannels )
      throw Error( "Missing " + root.Name() + " channel data." );
}

// ----------------------------------------------------------------------------

void LocalNormalizationData::SerializeNormalizationMatrices( XMLElement* root, const normalization_matrices& M ) const
{
   root->SetAttribute( "width", String( M.Width() ) );
   root->SetAttribute( "height", String( M.Height() ) );
   root->SetAttribute( "numberOfChannels", String( M.NumberOfChannels() ) );
   root->SetAttribute( "sampleFormat", "Float32" );

   for ( int c = 0; c < M.NumberOfChannels(); ++c )
   {
      XMLElement* element = new XMLElement( *root, "ChannelData" );
      if ( m_compressionEnabled )
      {
         LZ4Compression compression;
         compression.EnableByteShuffling();
         compression.SetItemSize( M.BytesPerSample() );
         Compression::subblock_list subblocks = compression.Compress( M[c], M.ChannelSize() );
         if ( !subblocks.IsEmpty() )
         {
            element->SetAttribute( "compression", compression.AlgorithmName() + "+sh" );
            for ( const Compression::Subblock& subblock : subblocks )
            {
               XMLElement* subblockElement = new XMLElement( *element, "Subblock" );
               subblockElement->SetAttribute( "uncompressedSize", String( subblock.uncompressedSize ) );
               *subblockElement << new XMLText( IsoString::ToBase64( subblock.compressedData ) );
            }
            continue;
         }
      }

      *element << new XMLText( IsoString::ToBase64( M[c], M.ChannelSize() ) );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/LocalNormalizationData.cpp - Released 2024-12-28T16:53:56Z
