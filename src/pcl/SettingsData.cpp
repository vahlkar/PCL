//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// pcl/SettingsData.cpp - Released 2024-12-23T11:33:03Z
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

#include <pcl/AutoLock.h>
#include <pcl/FileInfo.h>
#include <pcl/ReferenceSortedArray.h>
#include <pcl/SettingsData.h>
#include <pcl/XML.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct SettingsTree
{
   struct Node
   {
      IsoString key;

      Node() = default;

      Node( const IsoString& a_key )
         : key( a_key )
      {
      }

      virtual ~Node()
      {
      }

      virtual bool IsLeaf() const
      {
         return false;
      }

      virtual Variant Value() const
      {
         return Variant();
      }

      bool operator ==( const Node& other ) const
      {
         return key == other.key;
      }

      bool operator <( const Node& other ) const
      {
         return key < other.key;
      }
   };

   struct IndexNode : public Node
   {
      AutoPointer<SettingsTree> tree;

      IndexNode( const IsoString& a_key )
         : Node( a_key )
         , tree( new SettingsTree )
      {
      }
   };

   struct LeafNode : public Node
   {
      Variant value;

      LeafNode( const IsoString& a_key, const Variant& a_value )
         : Node( a_key )
         , value( a_value )
      {
      }

      bool IsLeaf() const override
      {
         return true;
      }

      Variant Value() const override
      {
         return value;
      }
   };

   SettingsTree() = default;

   ~SettingsTree()
   {
      Clear();
   }

   bool IsEmpty() const
   {
      return m_nodes.IsEmpty();
   }

   void Clear()
   {
      m_nodes.Destroy();
   }

   const LeafNode* Search( const IsoStringList& keys, int index = 0 ) const
   {
      if ( size_type( index ) < keys.Length() )
      {
         ReferenceSortedArray<Node>::const_iterator i = m_nodes.Search( keys[index] );
         if ( i != m_nodes.End() )
            return i->IsLeaf() ? static_cast<const LeafNode*>( i.Pointer() )
                               : static_cast<const IndexNode*>( i.Pointer() )->tree->Search( keys, index+1 );
      }
      return nullptr;
   }

   void GetKeys( IsoStringList& found, IsoString& currentKey, const IsoStringList& keys, int index = 0 ) const
   {
      if ( size_type( index ) < keys.Length() )
      {
         ReferenceSortedArray<Node>::const_iterator i = m_nodes.Search( keys[index] );
         if ( i != m_nodes.End() )
         {
            currentKey << i->key;
            if ( i->IsLeaf() )
               found << currentKey;
            else
            {
               currentKey << '/';
               if ( size_type( index ) == keys.UpperBound() )
                  static_cast<const IndexNode*>( i.Pointer() )->tree->GetAllKeys( found, currentKey );
               else
                  static_cast<const IndexNode*>( i.Pointer() )->tree->GetKeys( found, currentKey, keys, index+1 );
            }
         }
      }
   }

   void GetAllKeys( IsoStringList& found, IsoString& currentKey ) const
   {
      for ( const Node& node : m_nodes )
         if ( node.IsLeaf() )
            found << currentKey + node.key;
         else
         {
            IsoString newKey = currentKey + node.key + '/';
            static_cast<const IndexNode&>( node ).tree->GetAllKeys( found, newKey );
         }
   }

   /*
    * Returns true iff the settings have been modified.
    */
   bool Set( const IsoStringList& keys, const Variant& value, int index = 0 )
   {
      if ( size_type( index ) < keys.Length() )
      {
         ReferenceSortedArray<Node>::const_iterator i = m_nodes.Search( keys[index] );
         if ( i == m_nodes.End() )
         {
            if ( size_type( index ) == keys.UpperBound() )
            {
               m_nodes << new LeafNode( keys[index], value );
            }
            else
            {
               IndexNode* node = new IndexNode( keys[index] );
               node->tree->Set( keys, value, index+1 );
               m_nodes << node;
            }
         }
         else
         {
            if ( i->IsLeaf() )
            {
               if ( size_type( index ) == keys.UpperBound() )
               {
                  Variant& nodeValue = static_cast<LeafNode*>( m_nodes.MutableIterator( i ).Pointer() )->value;
                  if ( value == nodeValue )
                     return false;
                  nodeValue = value;
               }
               else
               {
                  // Mutate a leaf node into an index node.
                  m_nodes.Destroy( m_nodes.MutableIterator( i ) );
                  IndexNode* node = new IndexNode( keys[index] );
                  node->tree->Set( keys, value, index+1 );
                  m_nodes << node;
               }
            }
            else
            {
               if ( size_type( index ) == keys.UpperBound() )
               {
                  // Mutate an index node into a leaf node.
                  m_nodes.Destroy( m_nodes.MutableIterator( i ) );
                  m_nodes << new LeafNode( keys[index], value );
               }
               else
               {
                  return static_cast<IndexNode*>( m_nodes.MutableIterator( i ).Pointer() )->tree->Set( keys, value, index+1 );
               }
            }
         }

         return true;
      }

      return false;
   }

   bool Remove( const IsoStringList& keys, int index = 0 )
   {
      if ( size_type( index ) < keys.Length() )
      {
         ReferenceSortedArray<Node>::const_iterator i = m_nodes.Search( keys[index] );
         if ( i != m_nodes.End() )
         {
            if ( size_type( index ) == keys.UpperBound() || i->IsLeaf() )
            {
               m_nodes.Destroy( m_nodes.MutableIterator( i ) );
               return true;
            }
            return static_cast<IndexNode*>( m_nodes.MutableIterator( i ).Pointer() )->tree->Remove( keys, index+1 );
         }
      }
      return false;
   }

   void Serialize( XMLElement& root, unsigned compressionThreshold ) const
   {
      for ( const Node& node : m_nodes )
         if ( node.IsLeaf() )
         {
            Variant value = node.Value();
            if ( !value.IsValid() )
               continue;

            XMLElement* element = new XMLElement( root, "v",
                                                  XMLAttributeList() << XMLAttribute( "k", node.key ) );
            switch ( value.Type() )
            {
            case VariantType::Bool:
               element->SetAttribute( "t", "b" );
               element->SetAttribute( "v", value.ToString() );
               break;
            case VariantType::Int8:
            case VariantType::Int16:
            case VariantType::Int32:
               element->SetAttribute( "t", "i" );
               element->SetAttribute( "v", value.ToString() );
               break;
            case VariantType::UInt8:
            case VariantType::UInt16:
            case VariantType::UInt32:
               element->SetAttribute( "t", "u" );
               element->SetAttribute( "v", value.ToString() );
               break;
            case VariantType::Int64:
               element->SetAttribute( "t", "I" );
               element->SetAttribute( "v", value.ToString() );
               break;
            case VariantType::UInt64:
               element->SetAttribute( "t", "U" );
               element->SetAttribute( "v", value.ToString() );
               break;
            case VariantType::Float32:
               element->SetAttribute( "t", "f" );
               element->SetAttribute( "v", value.ToString() );
               break;
            case VariantType::Float64:
               element->SetAttribute( "t", "F" );
               element->SetAttribute( "v", value.ToString() );
               break;
            default:
            case VariantType::String:
            case VariantType::IsoString:
               element->SetAttribute( "t", "s" );
               {
                  String text = value.ToString();
                  if ( compressionThreshold == 0
                     || text.Length() < compressionThreshold
                     || !SerializeCompressedBlock( *element, text ) )
                  {
                     *element << new XMLText( text );
                  }
               }
               break;
            case VariantType::ByteArray:
               element->SetAttribute( "t", "B" );
               {
                  ByteArray data = value.ToByteArray();
                  if ( compressionThreshold == 0
                     || data.Length() < compressionThreshold
                     || !SerializeCompressedBlock( *element, data ) )
                  {
                     *element << new XMLText( IsoString::ToBase64( value.ToByteArray() ) );
                  }
               }
               break;
            case VariantType::TimePoint:
               element->SetAttribute( "t", "t" );
               element->SetAttribute( "v", value.ToTimePoint().ToString() );
               break;
            }
         }
         else
         {
            static_cast<const IndexNode&>( node ).tree->Serialize(
                  *(new XMLElement( root, "i",
                                    XMLAttributeList() << XMLAttribute( "k", node.key ) )), compressionThreshold );
         }
   }

   void Deserialize( const XMLElement& root )
   {
      for ( const XMLNode& node : root )
      {
         if ( !node.IsElement() )
            continue;

         const XMLElement& element = static_cast<const XMLElement&>( node );

         try
         {
            IsoString key = IsoString( element.AttributeValue( "k" ) );
            if ( key.IsEmpty() )
               continue;

            if ( element.Name() == "i" ) // index node
            {
               IndexNode* node = new IndexNode( key );
               node->tree->Deserialize( element );
               m_nodes << node;
            }
            else if ( element.Name() == "v" ) // value (leaf) node
            {
               IsoString valueType = IsoString( element.AttributeValue( "t" ) );
               String valueText = element.AttributeValue( "v" );
               Variant value;
               if ( valueType.Length() == 1 )
                  switch ( *valueType )
                  {
                  case 'b': // Boolean
                     value = valueText.ToBool();
                     break;
                  case 'i': // 32-bit integer
                     value = int32( valueText.ToInt( 10 ) );
                     break;
                  case 'I': // 64-bit integer
                     value = int64( valueText.ToInt64( 10 ) );
                     break;
                  case 'u': // 32-bit unsigned integer
                     value = uint32( valueText.ToUInt( 10 ) );
                     break;
                  case 'U': // 64-bit unsigned integer
                     value = uint64( valueText.ToUInt64( 10 ) );
                     break;
                  case 'f': // 32-bit floating point (float)
                     value = valueText.ToFloat();
                     break;
                  case 'F': // 64-bit floating point (double)
                     value = valueText.ToDouble();
                     break;
                  case 's': // UTF-16 string
                     value = valueText.IsEmpty() ? DeserializeBlock( element, true/*isText*/ ).ToString() : valueText;
                     break;
                  case 'B': // blob (ByteArray)
                     value = DeserializeBlock( element, false/*isText*/ ).ToByteArray();
                     break;
                  case 't': // time point
                     value = valueText.IsEmpty() ? TimePoint() : TimePoint( valueText );
                     break;
                  default:
                     break;
                  }
               if ( value.IsValid() )
                  m_nodes << new LeafNode( key, value );
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
   }

private:

   ReferenceSortedArray<Node> m_nodes;

   template <class T>
   static bool SerializeCompressedBlock( XMLElement& element, const T& data )
   {
      ZstdCompression compression;
      if ( sizeof( typename T::item_type ) > 1 )
      {
         compression.EnableByteShuffling();
         compression.SetItemSize( sizeof( typename T::item_type ) );
      }
      Compression::subblock_list subblocks = compression.Compress( data );
      if ( !subblocks.IsEmpty() )
      {
         element.SetAttribute( "c", compression.AlgorithmName() + "+sh" );
         for ( const Compression::Subblock& subblock : subblocks )
         {
            XMLElement* subblockElement = new XMLElement( element, "s" ); // Subblock
            subblockElement->SetAttribute( "u", String( subblock.uncompressedSize ) ); // uncompressedSize
            *subblockElement << new XMLText( IsoString::ToBase64( subblock.compressedData ) );
         }
         return true;
      }
      return false;
   }

   static Variant DeserializeBlock( const XMLElement& element, bool isText )
   {
      String compressionName = element.AttributeValue( "c" ).CaseFolded();
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
            compression->SetItemSize( isText ? sizeof( char16_type ) : sizeof( uint8 ) );
         }

         Compression::subblock_list subblocks;

         for ( const XMLNode& node : element )
         {
            if ( !node.IsElement() )
               continue;

            const XMLElement& subElement = static_cast<const XMLElement&>( node );

            if ( subElement.Name() == "s" ) // Subblock
            {
               Compression::Subblock subblock;
               String s = subElement.AttributeValue( "u" ); // uncompressedSize
               if ( s.IsEmpty() )
                  throw Error( "Missing subblock uncompressedSize attribute." );
               subblock.uncompressedSize = s.ToUInt64();
               subblock.compressedData = IsoString( subElement.Text().Trimmed() ).FromBase64();
               subblocks << subblock;
            }
         }

         if ( subblocks.IsEmpty() )
            throw Error( "Missing subblock child element(s)." );

         ByteArray data = compression->Uncompress( subblocks );
         if ( isText )
            return String( reinterpret_cast<const char16_type*>( data.Begin() ),
                           reinterpret_cast<const char16_type*>( data.End() ) );

         return data;
      }
      else
      {
         if ( isText )
            return element.Text();

         return IsoString( element.Text().Trimmed() ).FromBase64();
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * ### WARNING
 *
 * Since core version 1.8.9-2 build 1581 (September 2023), this class is used
 * as the settings back end of the core application. We must make sure that we
 * make no API calls that depend on the core pi::Application object, since
 * preferences settings are loaded before creating it upon application
 * execution. In particular, the pcl::GlobalSettings class must not be used.
 */

SettingsData::SettingsData( const IsoString& identifier )
   : m_root( new SettingsTree() )
   , m_identifier( identifier.Trimmed() )
{
   PCL_PRECONDITION( !m_identifier.IsEmpty() )
   if ( m_identifier.IsEmpty() )
      throw Error( "SettingsData: Empty file identifier" );
   if ( !m_identifier.IsValidIdentifier() )
      throw Error( "SettingsData: Invalid file identifier" );

   int instance = (*API->Global->ApplicationInstanceSlot)( ModuleHandle() );
   if ( instance <= 0 )
      throw Error( "SettingsData: Invalid application instance" );

   size_type len = 0;
   (*API->Global->GetApplicationConfigurationDirectory)( 0, &len );
   if ( len == 0 )
      throw Error( "SettingsData: Internal error: Unable to get the application configuration directory" );

   String confDirPath;
   confDirPath.SetLength( len );
   if ( (*API->Global->GetApplicationConfigurationDirectory)( confDirPath.Begin(), &len ) == api_false )
      throw APIFunctionError( "GetApplicationConfigurationDirectory" );
   confDirPath.ResizeToNullTerminated();

   if ( confDirPath.IsEmpty() || !File::DirectoryExists( confDirPath ) )
      throw Error( "SettingsData: Internal error: Invalid application configuration directory" );
   if ( !confDirPath.EndsWith( '/' ) )
      confDirPath << '/';

   m_filePath = confDirPath + m_identifier + String().Format( "-%03d-pxi.settings", instance );
}

// ----------------------------------------------------------------------------

SettingsData::~SettingsData()
{
   // ### N.B. This destructor must be declared offline because SettingsTree is
   // not defined in the class declaration.
}

// ----------------------------------------------------------------------------

bool SettingsData::IsEmpty() const
{
   volatile AutoLock lock( m_mutex );
   return m_root->IsEmpty();
}

// ----------------------------------------------------------------------------

fsize_type SettingsData::FileSize() const
{
   FileInfo info( m_filePath );
   if ( !info.Exists() || !info.IsFile() ) // just in case
      return 0;
   return info.Size();
}

// ----------------------------------------------------------------------------

void SettingsData::Clear()
{
   volatile AutoLock lock( m_mutex );
   m_root->Clear();
   m_modifiedCount = 0;
}

// ----------------------------------------------------------------------------

bool SettingsData::Add( const IsoString& key, const Variant& value )
{
   if ( !value.IsValid() )
      throw Error( "SettingsData::Add(): Invalid value specified" );

   IsoStringList keys = TokenizedKey( key );

   {
      volatile AutoLock lock( m_mutex );

      if ( m_root->Set( keys, value ) )
      {
         ++m_modifiedCount;
         return true;
      }
   }

   return false;
}

// ----------------------------------------------------------------------------

bool SettingsData::Remove( const IsoString& key )
{
   IsoStringList keys = TokenizedKey( key );

   {
      volatile AutoLock lock( m_mutex );

      if ( m_root->Remove( keys ) )
      {
         ++m_modifiedCount;
         return true;
      }
   }

   return false;
}

// ----------------------------------------------------------------------------

Variant SettingsData::Search( const IsoString& key ) const
{
   IsoStringList keys = TokenizedKey( key );

   {
      volatile AutoLock lock( m_mutex );

      const SettingsTree::LeafNode* leaf = m_root->Search( keys );
      if ( leaf != nullptr )
         return leaf->value;
   }

   return Variant();
}

// ----------------------------------------------------------------------------

IsoStringList SettingsData::Keys( const IsoString& key ) const
{
   IsoStringList keys = TokenizedKey( key );

   {
      volatile AutoLock lock( m_mutex );

      IsoStringList found;
      IsoString work;
      m_root->GetKeys( found, work, keys );
      return found;
   }
}

// ----------------------------------------------------------------------------

IsoStringList SettingsData::AllKeys() const
{
   volatile AutoLock lock( m_mutex );

   IsoStringList found;
   IsoString work;
   m_root->GetAllKeys( found, work );
   return found;
}

// ----------------------------------------------------------------------------

void SettingsData::Load()
{
   volatile AutoLock lock( m_mutex );

   m_root->Clear();
   m_modifiedCount = 0;

   if ( !File::Exists( m_filePath ) )
      return;

   IsoString text = File::ReadTextFile( m_filePath );
   if ( text.IsEmpty() )
      return;

   XMLDocument xml;
   xml.SetParserOption( XMLParserOption::IgnoreComments );
   xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
   xml.Parse( text.UTF8ToUTF16() );

   if ( xml.RootElement() == nullptr )
      throw Error( "SettingsData::Load(): The XML document has no root element." );
   if ( xml.RootElement()->Name() != "xsdt" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
      throw Error( "SettingsData::Load(): Not an XSDT version 1.0 document." );

   try
   {
      m_root->Deserialize( *xml.RootElement() );
   }
   catch ( Exception& x )
   {
      m_root->Clear();
      throw Error( "SettingsData::Load(): Corrupted settings data: " + x.Message() );
   }
   catch ( ... )
   {
      m_root->Clear();
      throw Error( "SettingsData::Load(): Corrupted settings data." );
   }
}

// ----------------------------------------------------------------------------

void SettingsData::Save() const
{
   AutoPointer<XMLDocument> xml = new XMLDocument;
   xml->SetXML( "1.0", "UTF-8" );
   *xml << new XMLComment( "\nPixInsight XML Settings Dictionary Format - XSDT version 1.0"
                           "\nCreated with PixInsight software - http://pixinsight.com/"
                           "\n" );

   XMLElement* root = new XMLElement( "xsdt", XMLAttributeList()
      << XMLAttribute( "version", "1.0" )
      << XMLAttribute( "xmlns", "http://www.pixinsight.com/xsdt" )
      << XMLAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" )
      << XMLAttribute( "xsi:schemaLocation", "http://www.pixinsight.com/xsdt http://pixinsight.com/xsdt/xsdt-1.0.xsd" ) );

   xml->SetRootElement( root );

   {
      volatile AutoLock lock( m_mutex );

      m_root->Serialize( *root, m_compressBlocks ? m_compressionThreshold : 0u );

      if ( !m_minifyXML )
      {
         xml->EnableAutoFormatting();
         xml->SetIndentSize( 3 );
      }

      xml->SerializeToFile( m_filePath );

      m_modifiedCount = 0;
   }
}

// ----------------------------------------------------------------------------

void SettingsData::Purge()
{
   volatile AutoLock lock( m_mutex );

   m_root->Clear();
   m_modifiedCount = 0;

   if ( File::Exists( m_filePath ) )
      File::Remove( m_filePath );
}

// ----------------------------------------------------------------------------

IsoStringList SettingsData::TokenizedKey( const IsoString& key )
{
   IsoStringList keys;
   IsoString currentKey;
   for ( char c : key )
   {
      if ( c == '/' )
      {
         if ( !currentKey.IsEmpty() )
         {
            keys << currentKey;
            currentKey.Clear();
         }
      }
      else if ( !IsoCharTraits::IsSpace( c ) )
         currentKey << c;
   }
   if ( !currentKey.IsEmpty() )
      keys << currentKey;

   if ( keys.IsEmpty() )
      throw Error( "SettingsData: Empty or invalid key specified" );

   return keys;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SettingsData.cpp - Released 2024-12-23T11:33:03Z
