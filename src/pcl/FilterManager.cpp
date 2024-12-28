//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/FilterManager.cpp - Released 2024-12-28T16:53:56Z
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
#include <pcl/FilterManager.h>
#include <pcl/GlobalSettings.h>
#include <pcl/TimePoint.h>
#include <pcl/XML.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static String ConfigurationFilePath( const String& id )
{
   int instance = PixInsightSettings::GlobalInteger( "Application/Instance" );
   if ( instance <= 0 )
      throw Error( "FilterManager: Internal error: Invalid application instance" );

   String confDirPath = PixInsightSettings::GlobalString( "Application/ConfigDirectory" );
   if ( confDirPath.IsEmpty() || !File::DirectoryExists( confDirPath ) )
      throw Error( "FilterManager: Internal error: Invalid application configuration directory" );
   if ( !confDirPath.EndsWith( '/' ) )
      confDirPath << '/';

   return confDirPath + id + String().Format( "-%03d-pxi.xspd", instance );
}

// ----------------------------------------------------------------------------

bool FilterManager::Initialize( const String& a_filtersDatabaseFilePath, const String& a_whiteReferencesDatabaseFilePath )
{
   String filtersDatabaseFilePath = a_filtersDatabaseFilePath;
   if ( filtersDatabaseFilePath.IsEmpty() )
   {
      filtersDatabaseFilePath = ConfigurationFiltersDatabaseFilePath();
      if ( !File::Exists( filtersDatabaseFilePath ) )
      {
         filtersDatabaseFilePath = DefaultFiltersDatabaseFilePath();
         if ( filtersDatabaseFilePath.IsEmpty() )
            filtersDatabaseFilePath = FactoryDefaultFiltersDatabaseFilePath();
      }
   }
   if ( filtersDatabaseFilePath.IsEmpty() || !File::Exists( filtersDatabaseFilePath ) )
      throw Error( "FilterManager: Internal error: Unable to acquire a valid filter database file path" );

   String whiteReferencesDatabaseFilePath = a_whiteReferencesDatabaseFilePath;
   if ( whiteReferencesDatabaseFilePath.IsEmpty() )
   {
      whiteReferencesDatabaseFilePath = ConfigurationWhiteReferencesDatabaseFilePath();
      if ( !File::Exists( whiteReferencesDatabaseFilePath ) )
      {
         whiteReferencesDatabaseFilePath = DefaultWhiteReferencesDatabaseFilePath();
         if ( whiteReferencesDatabaseFilePath.IsEmpty() )
            whiteReferencesDatabaseFilePath = FactoryDefaultWhiteReferencesDatabaseFilePath();
      }
   }
   if ( whiteReferencesDatabaseFilePath.IsEmpty() || !File::Exists( whiteReferencesDatabaseFilePath ) )
      throw Error( "FilterManager: Internal error: Unable to acquire a valid white reference database file path" );

   m_filters.Clear();
   m_whiteReferences.Clear();
   m_defaultRedFilter = m_defaultGreenFilter = m_defaultBlueFilter = m_defaultDeviceQE = m_defaultWhiteReference = SampledSpectrumData();
   m_filtersDatabaseFilePath = filtersDatabaseFilePath;
   m_whiteReferencesDatabaseFilePath = whiteReferencesDatabaseFilePath;
   m_initialized = true;
   m_valid = false;
   m_modified = 0;

   return m_valid = ImportFilters( filtersDatabaseFilePath ) &&
                    ImportWhiteReferences( whiteReferencesDatabaseFilePath );
}

// ----------------------------------------------------------------------------

bool FilterManager::ImportFilters( const String& filePath, bool merge )
{
   SampledSpectrumDataList filters = LoadFilters( filePath );
   if ( !filters.IsEmpty() )
   {
      ImportFilters( filters, merge );
      return true;
   }
   return false;
}

void FilterManager::ImportFilters( const SampledSpectrumDataList& filters, bool merge )
{
   if ( filters.IsEmpty() )
      return;

   for ( const SampledSpectrumData& filter : filters )
      if ( !filter.IsValid() )
         throw Error( "Invalid filter definition: '" + filter.name + "'" );

   SampledSpectrumData defaultLuminanceFilter;
   for ( const SampledSpectrumData& filter : filters )
      if ( filter.defaultItem == "L" )
      {
         defaultLuminanceFilter = filter;
         break;
      }
   if ( !defaultLuminanceFilter.IsValid() )
      if ( !m_defaultLuminanceFilter.IsValid() )
         throw Error( "Unable to determine a default luminance filter" );

   SampledSpectrumData defaultRedFilter;
   for ( const SampledSpectrumData& filter : filters )
      if ( filter.defaultItem == "R" )
      {
         defaultRedFilter = filter;
         break;
      }
   if ( !defaultRedFilter.IsValid() )
      if ( !m_defaultRedFilter.IsValid() )
         throw Error( "Unable to determine a default red filter" );

   SampledSpectrumData defaultGreenFilter;
   for ( const SampledSpectrumData& filter : filters )
      if ( filter.defaultItem == "G" )
      {
         defaultGreenFilter = filter;
         break;
      }
   if ( !defaultGreenFilter.IsValid() )
      if ( !m_defaultGreenFilter.IsValid() )
         throw Error( "Unable to determine a default green filter" );

   SampledSpectrumData defaultBlueFilter;
   for ( const SampledSpectrumData& filter : filters )
      if ( filter.defaultItem == "B" )
      {
         defaultBlueFilter = filter;
         break;
      }
   if ( !defaultBlueFilter.IsValid() )
      if ( !m_defaultBlueFilter.IsValid() )
         throw Error( "Unable to determine a default blue filter" );

   SampledSpectrumData defaultDeviceQE;
   for ( const SampledSpectrumData& filter : filters )
      if ( filter.defaultItem == "Q" )
      {
         defaultDeviceQE = filter;
         break;
      }
   if ( !defaultDeviceQE.IsValid() )
      if ( !m_defaultDeviceQE.IsValid() )
         throw Error( "Unable to determine a default device QE curve" );

   if ( merge )
      for ( const SampledSpectrumData& filter : filters )
         m_filters << filter;
   else
      m_filters = filters;

   if ( defaultLuminanceFilter.IsValid() )
      m_defaultLuminanceFilter = defaultLuminanceFilter;
   else if ( FindFilterByName( m_defaultLuminanceFilter.name ) < 0 )
      m_filters << m_defaultLuminanceFilter;

   if ( defaultRedFilter.IsValid() )
      m_defaultRedFilter = defaultRedFilter;
   else if ( FindFilterByName( m_defaultRedFilter.name ) < 0 )
      m_filters << m_defaultRedFilter;

   if ( defaultGreenFilter.IsValid() )
      m_defaultGreenFilter = defaultGreenFilter;
   else if ( FindFilterByName( m_defaultGreenFilter.name ) < 0 )
      m_filters << m_defaultGreenFilter;

   if ( defaultBlueFilter.IsValid() )
      m_defaultBlueFilter = defaultBlueFilter;
   else if ( FindFilterByName( m_defaultBlueFilter.name ) < 0 )
      m_filters << m_defaultBlueFilter;

   if ( defaultDeviceQE.IsValid() )
      m_defaultDeviceQE = defaultDeviceQE;
   else if ( FindFilterByName( m_defaultDeviceQE.name ) < 0 )
      m_filters << m_defaultDeviceQE;

   // Ensure we have only one filter marked as default for each L,R,G,B,Q.
   for ( SampledSpectrumData& filter : m_filters )
      if ( !filter.defaultItem.IsEmpty() )
         if ( filter.defaultItem == "L" && filter.name != m_defaultLuminanceFilter.name
           || filter.defaultItem == "R" && filter.name != m_defaultRedFilter.name
           || filter.defaultItem == "G" && filter.name != m_defaultGreenFilter.name
           || filter.defaultItem == "B" && filter.name != m_defaultBlueFilter.name
           || filter.defaultItem == "Q" && filter.name != m_defaultDeviceQE.name )
            filter.defaultItem.Clear();

   m_filters.Sort();
   m_modified |= 1;
}

// ----------------------------------------------------------------------------

bool FilterManager::ImportWhiteReferences( const String& filePath, bool merge )
{
   SampledSpectrumDataList whiteReferences = LoadWhiteReferences( filePath );
   if ( !whiteReferences.IsEmpty() )
   {
      ImportWhiteReferences( whiteReferences, merge );
      return true;
   }
   return false;
}

void FilterManager::ImportWhiteReferences( const SampledSpectrumDataList& whiteReferences, bool merge )
{
   if ( whiteReferences.IsEmpty() )
      return;

   for ( const SampledSpectrumData& whiteReference : whiteReferences )
      if ( !whiteReference.IsValid() )
         throw Error( "Invalid white reference definition: '" + whiteReference.name + "'" );

   SampledSpectrumData defaultWhiteReference;
   for ( const SampledSpectrumData& whiteReference : whiteReferences )
      if ( whiteReference.defaultItem == "W" )
      {
         defaultWhiteReference = whiteReference;
         break;
      }

   if ( !defaultWhiteReference.IsValid() )
      if ( !m_defaultWhiteReference.IsValid() )
         throw Error( "Unable to determine a default white reference" );

   if ( merge )
      for ( const SampledSpectrumData& whiteReference : whiteReferences )
         m_whiteReferences << whiteReference;
   else
      m_whiteReferences = whiteReferences;

   if ( defaultWhiteReference.IsValid() )
      m_defaultWhiteReference = defaultWhiteReference;
   else if ( FindFilterByName( m_defaultWhiteReference.name ) < 0 )
      m_whiteReferences << m_defaultWhiteReference;

   // Ensure we have only one white reference marked as default.
   for ( SampledSpectrumData& whiteReference : m_whiteReferences )
      if ( !whiteReference.defaultItem.IsEmpty() )
         if ( whiteReference.name != m_defaultWhiteReference.name )
            whiteReference.defaultItem.Clear();

   m_whiteReferences.Sort();
   m_modified |= 2;
}

// ----------------------------------------------------------------------------

void FilterManager::ReloadFilters()
{
   ImportFilters( m_filtersDatabaseFilePath, false/*merge*/ );
   m_modified &= ~1;
}

void FilterManager::ReloadWhiteReferences()
{
   ImportWhiteReferences( m_whiteReferencesDatabaseFilePath, false/*merge*/ );
   m_modified &= ~2;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

String FilterManager::ConfigurationFiltersDatabaseFilePath()
{
   return ConfigurationFilePath( "filters" );
}

String FilterManager::ConfigurationWhiteReferencesDatabaseFilePath()
{
   return ConfigurationFilePath( "white-references" );
}

// ----------------------------------------------------------------------------

String FilterManager::DefaultFiltersDatabaseFilePath()
{
   return PixInsightSettings::GlobalString( "Application/FiltersDatabaseFilePath" );
}

String FilterManager::DefaultWhiteReferencesDatabaseFilePath()
{
   return PixInsightSettings::GlobalString( "Application/WhiteReferencesDatabaseFilePath" );
}

// ----------------------------------------------------------------------------

String FilterManager::FactoryDefaultFiltersDatabaseFilePath()
{
   return PixInsightSettings::GlobalString( "Application/LibraryDirectory" ) + "/filters.xspd";
}

String FilterManager::FactoryDefaultWhiteReferencesDatabaseFilePath()
{
   return PixInsightSettings::GlobalString( "Application/LibraryDirectory" ) + "/white-references.xspd";
}

// ----------------------------------------------------------------------------

void FilterManager::SaveToConfigurationFiltersDatabaseFile()
{
   m_filtersDatabaseFilePath = ConfigurationFiltersDatabaseFilePath();
   ExportFiltersDatabase( m_filtersDatabaseFilePath, m_filters );

   m_modified &= ~1;

   (*API->Global->BroadcastGlobalFiltersUpdated)( nullptr /*reserved*/ );
}

void FilterManager::SaveToConfigurationWhiteReferencesDatabaseFile()
{
   m_whiteReferencesDatabaseFilePath = ConfigurationWhiteReferencesDatabaseFilePath();
   ExportWhiteReferencesDatabase( m_whiteReferencesDatabaseFilePath, m_whiteReferences );

   m_modified &= ~2;

   (*API->Global->BroadcastGlobalFiltersUpdated)( nullptr /*reserved*/ );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

SampledSpectrumDataList FilterManager::LoadFilters( const String& filePath )
{
   SampledSpectrumDataList filters;

   try
   {
      if ( !File::Exists( filePath ) )
         throw Error( "No such file" );

      XMLDocument xml;
      xml.SetParserOption( XMLParserOption::IgnoreComments );
      xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
      xml.Parse( File::ReadTextFile( filePath ).UTF8ToUTF16() );

      if ( xml.RootElement() == nullptr )
         throw Error( "The XML document has no root element" );
      if ( xml.RootElement()->Name() != "xspd" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
         throw Error( "Not an XSPD version 1.0 document" );

      XMLElement::child_element_list elements = xml.RootElement()->ChildElements();
      for ( const XMLElement& element : elements )
         if ( element.Name() == "Filter" )
         {
            String name = element.AttributeValue( "name" );
            String data = element.AttributeValue( "data" );
            if ( !name.IsEmpty() )
               if ( !data.IsEmpty() )
               {
                  String channel = element.AttributeValue( "channel" );
                  String filterDefault = element.AttributeValue( "default" );
                  String reference = element.AttributeValue( "reference" );
                  filters << SampledSpectrumData{ name, channel, filterDefault, reference, data };
               }
         }

      filters.Sort();

      return filters;
   }
   catch ( const Exception& x )
   {
      throw Error( "Decoding filters spectrum database file: " + x.Message() + ": " + filePath );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

SampledSpectrumDataList FilterManager::LoadWhiteReferences( const String& filePath )
{
   SampledSpectrumDataList whiteReferences;

   try
   {
      if ( !File::Exists( filePath ) )
         throw Error( "No such file" );

      XMLDocument xml;
      xml.SetParserOption( XMLParserOption::IgnoreComments );
      xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
      xml.Parse( File::ReadTextFile( filePath ).UTF8ToUTF16() );

      if ( xml.RootElement() == nullptr )
         throw Error( "The XML document has no root element" );
      if ( xml.RootElement()->Name() != "xspd" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
         throw Error( "Not an XSPD version 1.0 document" );

      XMLElement::child_element_list elements = xml.RootElement()->ChildElements();
      for ( const XMLElement& element : elements )
         if ( element.Name() == "WhiteRef" )
         {
            String name = element.AttributeValue( "name" );
            String data = element.AttributeValue( "data" );
            if ( !name.IsEmpty() )
               if ( !data.IsEmpty() )
               {
                  bool defaultOne = element.AttributeValue( "default" ) == "W";
                  String reference = element.AttributeValue( "reference" );
                  whiteReferences << SampledSpectrumData{ name, String()/*channel*/, defaultOne ? "W" : "", reference, data };
               }
         }

      whiteReferences.Sort();

      return whiteReferences;
   }
   catch ( const Exception& x )
   {
      throw Error( "Decoding white references spectrum database file: " + x.Message() + ": " + filePath );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

void FilterManager::ExportFiltersDatabase( const String& filePath, const SampledSpectrumDataList& filters )
{
   AutoPointer<XMLDocument> xml = new XMLDocument;
   xml->SetXML( "1.0", "UTF-8" );
   *xml << new XMLComment( "\nPixInsight Spectrum Database Format - XSPD version 1.0"
                           "\nFilters Database"
                           "\nCreated with PixInsight software - www.pixinsight.com"
                           "\n" );

   XMLElement* root = new XMLElement( "xspd", XMLAttributeList()
      << XMLAttribute( "version", "1.0" )
      << XMLAttribute( "xmlns", "http://www.pixinsight.com/xspd" )
      << XMLAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" )
      << XMLAttribute( "xsi:schemaLocation", "http://www.pixinsight.com/xspd http://pixinsight.com/xspd/xspd-1.0.xsd" ) );

   xml->SetRootElement( root );

   *(new XMLElement( *root, "CreationTime" )) << new XMLText( TimePoint::Now().ToString() );

   for ( const SampledSpectrumData& f : filters )
      new XMLElement( *root, "Filter", XMLAttributeList()
               << XMLAttribute( "name", f.name )
               << (f.channel.IsEmpty() ? XMLAttribute() : XMLAttribute( "channel", f.channel ))
               << (f.defaultItem.IsEmpty() ? XMLAttribute() : XMLAttribute( "default", f.defaultItem ))
               << (f.reference.IsEmpty() ? XMLAttribute() : XMLAttribute( "reference", f.reference ))
               << XMLAttribute( "data", f.data ) );

   xml->EnableAutoFormatting();
   xml->SetIndentSize( 3 );
   xml->SerializeToFile( filePath );
}

// ----------------------------------------------------------------------------

void FilterManager::ExportWhiteReferencesDatabase( const String& filePath, const SampledSpectrumDataList& whiteReferences )
{
   AutoPointer<XMLDocument> xml = new XMLDocument;
   xml->SetXML( "1.0", "UTF-8" );
   *xml << new XMLComment( "\nPixInsight Spectrum Database Format - XSPD version 1.0"
                           "\nWhite References Database"
                           "\nCreated with PixInsight software - www.pixinsight.com"
                           "\n" );

   XMLElement* root = new XMLElement( "xspd", XMLAttributeList()
      << XMLAttribute( "version", "1.0" )
      << XMLAttribute( "xmlns", "http://www.pixinsight.com/xspd" )
      << XMLAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" )
      << XMLAttribute( "xsi:schemaLocation", "http://www.pixinsight.com/xspd http://pixinsight.com/xspd/xspd-1.0.xsd" ) );

   xml->SetRootElement( root );

   *(new XMLElement( *root, "CreationTime" )) << new XMLText( TimePoint::Now().ToString() );

   for ( const SampledSpectrumData& w : whiteReferences )
      new XMLElement( *root, "WhiteRef", XMLAttributeList()
               << XMLAttribute( "name", w.name )
               << ((w.defaultItem == "W") ? XMLAttribute( "default", "W" ) : XMLAttribute())
               << (w.reference.IsEmpty() ? XMLAttribute() : XMLAttribute( "reference", w.reference ))
               << XMLAttribute( "data", w.data ) );

   xml->EnableAutoFormatting();
   xml->SetIndentSize( 3 );
   xml->SerializeToFile( filePath );
}

// ----------------------------------------------------------------------------

SampledSpectrumData FilterManager::DecodeFilterCSV( const IsoStringList& lines )
{
   SampledSpectrumData filter;
   int lineNo = 1;
   IsoString wavelength, transmission;
   for ( const IsoString& line : lines )
   {
      size_type p = line.FindFirst( ',' );
      if ( p == IsoString::notFound )
         throw Error( "Expected comma separator (line #" + String( lineNo ) + ")" );

      IsoString itemName = line.Left( p ).Trimmed();
      if ( itemName.IsEmpty() )
         throw Error( "Expected item identifier before ',' (line #" + String( lineNo ) + ")" );

      IsoString itemValue = line.Substring( p+1 ).Trimmed().Unquoted().Trimmed();
      if ( itemValue.IsEmpty() )
         throw Error( "Expected item value after ',' (line #" + String( lineNo ) + ")" );

      if ( itemName.CompareIC( "type" ) == 0 )
      {
         if ( itemValue.CompareIC( "filter" ) != 0 )
            throw Error( "Expected type=filter, got '" + itemValue + "'" );
      }
      else if ( itemName.CompareIC( "name" ) == 0 )
         filter.name = itemValue.UTF8ToUTF16();
      else if ( itemName.CompareIC( "channel" ) == 0 )
         filter.channel = itemValue;
      else if ( itemName.CompareIC( "default" ) == 0 )
         filter.defaultItem = itemValue;
      else if ( itemName.CompareIC( "reference" ) == 0 )
         filter.reference = itemValue.UTF8ToUTF16();
      else if ( itemName.CompareIC( "wavelength" ) == 0 )
         wavelength = itemValue;
      else if ( itemName.CompareIC( "transmission" ) == 0 )
         transmission = itemValue;
   }

   if ( filter.name.IsEmpty() )
      throw Error( "No filter name has been specified" );
   if ( wavelength.IsEmpty() )
      throw Error( "Invalid filter data (" + filter.name + "): No filter wavelength data have been specified" );
   if ( transmission.IsEmpty() )
      throw Error( "Invalid filter data (" + filter.name + "): No filter transmission data have been specified" );

   Array<float> wavelengthValues = wavelength.ParseListOfFloat();
   Array<float> transmissionValues = transmission.ParseListOfFloat();

   if ( wavelengthValues.Length() != transmissionValues.Length() )
      throw Error( "Invalid filter data (" + filter.name + "): Wavelength and transmission lists of different lengths" );

   if ( wavelengthValues.Length() < 5 )
      throw Error( "Invalid filter data (" + filter.name + "): At least 5 filter transmission values are required, only "
                     + String( wavelengthValues.Length() ) + " are available" );

   for ( size_type i = 1; i < wavelengthValues.Length(); ++i )
      if ( wavelengthValues[i] <= wavelengthValues[i-1] )
         throw Error( "Invalid filter data (" + filter.name + "): Wavelength values must be different and sorted in ascending order" );

   /*
    * ### NB. Be permissive with transmission ranges, since in some occasions
    * we have values slightly above 1.0 due to instrumental dispersion. We'll
    * truncate out-of-range values when reserializing filter data.
    */
//    for ( size_type i = 0; i < transmissionValues.Length(); ++i )
//       if ( transmissionValues[i] < 0 || transmissionValues[i] > 1 )
//          throw Error( "Invalid filter data (" + filter.name + "): Filter transmission values must be in the [0,1] range" );

   for ( size_type i = 0;; )
   {
      filter.data << String().Format( "%g", wavelengthValues[i] )
                  << ','
                  << String().Format( "%g", Range( transmissionValues[i], 0.0F, 1.0F ) );
      if ( ++i == wavelengthValues.Length() )
         break;
      filter.data << ',';
   }

   return filter;
}

// ----------------------------------------------------------------------------

SampledSpectrumData FilterManager::DecodeWhiteReferenceCSV( const IsoStringList& lines )
{
   SampledSpectrumData whiteReference;
   int lineNo = 1;
   IsoString wavelength, flux;
   for ( const IsoString& line : lines )
   {
      size_type p = line.FindFirst( ',' );
      if ( p == IsoString::notFound )
         throw Error( "Expected comma separator (line #" + String( lineNo ) + ")" );

      IsoString itemName = line.Left( p ).Trimmed();
      if ( itemName.IsEmpty() )
         throw Error( "Expected item identifier before ',' (line #" + String( lineNo ) + ")" );

      IsoString itemValue = line.Substring( p+1 ).Trimmed().Unquoted().Trimmed();
      if ( itemValue.IsEmpty() )
         throw Error( "Expected item value after ',' (line #" + String( lineNo ) + ")" );

      if ( itemName.CompareIC( "type" ) == 0 )
      {
         if ( itemValue.CompareIC( "white-reference" ) != 0 )
            if ( itemValue.CompareIC( "white reference" ) != 0 )
               throw Error( "Expected type=white-reference, got '" + itemValue + "'" );
      }
      else if ( itemName.CompareIC( "name" ) == 0 )
         whiteReference.name = itemValue.UTF8ToUTF16();
      else if ( itemName.CompareIC( "default" ) == 0 && itemValue == "W" )
         whiteReference.defaultItem = itemValue;
      else if ( itemName.CompareIC( "reference" ) == 0 )
         whiteReference.reference = itemValue.UTF8ToUTF16();
      else if ( itemName.CompareIC( "wavelength" ) == 0 )
         wavelength = itemValue;
      else if ( itemName.CompareIC( "flux" ) == 0 )
         flux = itemValue;
   }

   if ( whiteReference.name.IsEmpty() )
      throw Error( "No white reference name has been specified" );
   if ( wavelength.IsEmpty() )
      throw Error( "Invalid white reference data (" + whiteReference.name + "): No wavelength data have been specified" );
   if ( flux.IsEmpty() )
      throw Error( "Invalid white reference data (" + whiteReference.name + "): No flux data have been specified" );

   Array<float> wavelengthValues = wavelength.ParseListOfFloat();
   Array<float> fluxValues = flux.ParseListOfFloat();

   if ( wavelengthValues.Length() != fluxValues.Length() )
      throw Error( "Invalid white reference data (" + whiteReference.name + "): Wavelength and flux lists of different lengths" );

   if ( wavelengthValues.Length() < 5 )
      throw Error( "Invalid white reference data (" + whiteReference.name + "): At least 5 flux values are required, only "
                     + String( wavelengthValues.Length() ) + " are available" );

   for ( size_type i = 1; i < wavelengthValues.Length(); ++i )
      if ( wavelengthValues[i] <= wavelengthValues[i-1] )
         throw Error( "Invalid white reference data (" + whiteReference.name + "): Wavelength values must be different and sorted in ascending order" );

   for ( size_type i = 0; i < fluxValues.Length(); ++i )
      if ( fluxValues[i] < 0 )
         throw Error( "Invalid white reference data (" + whiteReference.name + "): Flux values must be >= 0" );

   for ( size_type i = 0;; )
   {
      whiteReference.data << String().Format( "%g", wavelengthValues[i] )
                          << ','
                          << String().Format( "%g", fluxValues[i] );
      if ( ++i == fluxValues.Length() )
         break;
      whiteReference.data << ',';
   }

   return whiteReference;
}

// ----------------------------------------------------------------------------

IsoStringList FilterManager::EncodeFilterCSV( const SampledSpectrumData& filter )
{
   StringList tokens;
   filter.data.Break( tokens, ',', true/*trim*/ );
   IsoStringList wavelength, transmission;
   for ( size_type i = 0, j = 0, n = tokens.Length() >> 1; i < n; ++i )
   {
      wavelength << tokens[j++].ToIsoString();
      transmission << tokens[j++].ToIsoString();
   }

   IsoStringList csv;
   csv << "type,\"filter\"";
   csv << "name," + filter.name.ToUTF8().DoubleQuoted();
   if ( !filter.channel.IsEmpty() )
      csv << "channel," + filter.channel.ToIsoString().DoubleQuoted();
   if ( !filter.defaultItem.IsEmpty() )
      csv << "default," + filter.defaultItem.ToIsoString().DoubleQuoted();
   if ( !filter.reference.IsEmpty() )
      csv << "reference," + filter.reference.ToUTF8().DoubleQuoted();
   csv << "wavelength," + IsoString().ToCommaSeparated( wavelength ).DoubleQuoted();
   csv << "transmission," + IsoString().ToCommaSeparated( transmission ).DoubleQuoted();
   return csv;
}

// ----------------------------------------------------------------------------

IsoStringList FilterManager::EncodeWhiteReferenceCSV( const SampledSpectrumData& whiteReference )
{
   StringList tokens;
   whiteReference.data.Break( tokens, ',', true/*trim*/ );
   IsoStringList wavelength, flux;
   for ( size_type i = 0, j = 0, n = tokens.Length() >> 1; i < n; ++i )
   {
      wavelength << tokens[j++].ToIsoString();
      flux << tokens[j++].ToIsoString();
   }

   IsoStringList csv;
   csv << "type,\"spectrum\"";
   csv << "name," + whiteReference.name.ToUTF8().DoubleQuoted();
   if ( whiteReference.defaultItem == "W" )
      csv << "default," + whiteReference.defaultItem.ToIsoString().DoubleQuoted();
   if ( !whiteReference.reference.IsEmpty() )
      csv << "reference," + whiteReference.reference.ToUTF8().DoubleQuoted();
   csv << "wavelength," + IsoString().ToCommaSeparated( wavelength ).DoubleQuoted();
   csv << "flux," + IsoString().ToCommaSeparated( flux ).DoubleQuoted();
   return csv;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FilterManager.cpp - Released 2024-12-28T16:53:56Z
