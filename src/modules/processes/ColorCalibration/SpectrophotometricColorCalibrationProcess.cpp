//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.0
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// SpectrophotometricColorCalibrationProcess.cpp - Released 2023-09-14T17:02:22Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "SpectrophotometricColorCalibrationProcess.h"
#include "SpectrophotometricColorCalibrationParameters.h"
#include "SpectrophotometricColorCalibrationInstance.h"
#include "SpectrophotometricColorCalibrationInterface.h"

#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Settings.h>
#include <pcl/XML.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SpectrophotometricColorCalibrationProcess* TheSpectrophotometricColorCalibrationProcess = nullptr;

// ----------------------------------------------------------------------------

SpectrophotometricColorCalibrationProcess::SpectrophotometricColorCalibrationProcess()
{
   TheSpectrophotometricColorCalibrationProcess = this;

   new SPCCApplyCalibration( this );
   new SPCCNarrowbandMode( this );
   new SPCCNarrowbandOptimizeStars( this );
   new SPCCWhiteReferenceSpectrum( this );
   new SPCCWhiteReferenceName( this );
   new SPCCRedFilterTrCurve( this );
   new SPCCRedFilterName( this );
   new SPCCGreenFilterTrCurve( this );
   new SPCCGreenFilterName( this );
   new SPCCBlueFilterTrCurve( this );
   new SPCCBlueFilterName( this );
   new SPCCRedFilterWavelength( this );
   new SPCCRedFilterBandwidth( this );
   new SPCCGreenFilterWavelength( this );
   new SPCCGreenFilterBandwidth( this );
   new SPCCBlueFilterWavelength( this );
   new SPCCBlueFilterBandwidth( this );
   new SPCCDeviceQECurve( this );
   new SPCCDeviceQECurveName( this );
   new SPCCBroadbandIntegrationStepSize( this );
   new SPCCNarrowbandIntegrationSteps( this );
   new SPCCCatalogId( this );
   new SPCCLimitMagnitude( this );
   new SPCCAutoLimitMagnitude( this );
   new SPCCTargetSourceCount( this );
   new SPCCStructureLayers( this );
   new SPCCSaturationThreshold( this );
   new SPCCSaturationRelative( this );
   new SPCCSaturationShrinkFactor( this );
   new SPCCPSFNoiseLayers( this );
   new SPCCPSFHotPixelFilterRadius( this );
   new SPCCPSFNoiseReductionFilterRadius( this );
   new SPCCPSFMinStructureSize( this );
   new SPCCPSFMinSNR( this );
   new SPCCPSFAllowClusteredSources( this );
   new SPCCPSFType( this );
   new SPCCPSFGrowth( this );
   new SPCCPSFMaxStars( this );
   new SPCCPSFSearchTolerance( this );
   new SPCCPSFChannelSearchTolerance( this );
   new SPCCNeutralizeBackground( this );
   new SPCCBackgroundReferenceViewId( this );
   new SPCCBackgroundLow( this );
   new SPCCBackgroundHigh( this );
   new SPCCBackgroundUseROI( this );
   new SPCCBackgroundROIX0( this );
   new SPCCBackgroundROIY0( this );
   new SPCCBackgroundROIX1( this );
   new SPCCBackgroundROIY1( this );
   new SPCCGenerateGraphs( this );
   new SPCCGenerateStarMaps( this );
   new SPCCGenerateTextFiles( this );
   new SPCCOutputDirectory( this );
}

// ----------------------------------------------------------------------------

IsoString SpectrophotometricColorCalibrationProcess::Id() const
{
   return "SpectrophotometricColorCalibration";
}

// ----------------------------------------------------------------------------

IsoString SpectrophotometricColorCalibrationProcess::Category() const
{
   return "ColorCalibration,Photometry,Spectrophotometry";
}

// ----------------------------------------------------------------------------

uint32 SpectrophotometricColorCalibrationProcess::Version() const
{
   return 1; // November 2022
}

// ----------------------------------------------------------------------------

String SpectrophotometricColorCalibrationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String SpectrophotometricColorCalibrationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/SpectrophotometricColorCalibration.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* SpectrophotometricColorCalibrationProcess::DefaultInterface() const
{
   return TheSpectrophotometricColorCalibrationInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* SpectrophotometricColorCalibrationProcess::Create() const
{
   return new SpectrophotometricColorCalibrationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SpectrophotometricColorCalibrationProcess::Clone( const ProcessImplementation& p ) const
{
   const SpectrophotometricColorCalibrationInstance* instPtr = dynamic_cast<const SpectrophotometricColorCalibrationInstance*>( &p );
   return (instPtr != nullptr) ? new SpectrophotometricColorCalibrationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

SampledSpectrumDataList SpectrophotometricColorCalibrationProcess::LoadWhiteReferences( const String& filePath )
{
   SampledSpectrumDataList whiteReferences;

   try
   {
      if ( !File::Exists( filePath ) )
         throw String( "No such file" );

      XMLDocument xml;
      xml.SetParserOption( XMLParserOption::IgnoreComments );
      xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
      xml.Parse( File::ReadTextFile( filePath ).UTF8ToUTF16() );

      if ( xml.RootElement() == nullptr )
         throw String( "The XML document has no root element" );
      if ( xml.RootElement()->Name() != "xspd" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
         throw String( "Not an XSPD version 1.0 document" );

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

      if ( whiteReferences.IsEmpty() )
         throw String( "No valid white references have been defined" );

      whiteReferences.Sort();

      return whiteReferences;
   }
   catch ( const Exception& x )
   {
      try
      {
         throw x;
      }
      ERROR_HANDLER
   }
   catch ( const String& what )
   {
      Console console;
      console.Show();
      console.CriticalLn( "<end><cbr><br>*** Error: SpectrophotometricColorCalibration: "
                  "Decoding white references spectrum database file: " + what + ": " + filePath );
   }
   catch ( ... )
   {
      try
      {
         throw;
      }
      ERROR_HANDLER
   }

   return SampledSpectrumDataList();
}

// ----------------------------------------------------------------------------

SampledSpectrumDataList SpectrophotometricColorCalibrationProcess::LoadFilters( const String& filePath )
{
   SampledSpectrumDataList filters;

   try
   {
      if ( !File::Exists( filePath ) )
         throw String( "No such file" );

      XMLDocument xml;
      xml.SetParserOption( XMLParserOption::IgnoreComments );
      xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
      xml.Parse( File::ReadTextFile( filePath ).UTF8ToUTF16() );

      if ( xml.RootElement() == nullptr )
         throw String( "The XML document has no root element" );
      if ( xml.RootElement()->Name() != "xspd" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
         throw String( "Not an XSPD version 1.0 document" );

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

      if ( filters.IsEmpty() )
         throw String( "No valid filters have been defined" );

      filters.Sort();

      return filters;
   }
   catch ( const Exception& x )
   {
      try
      {
         throw x;
      }
      ERROR_HANDLER
   }
   catch ( const String& what )
   {
      Console console;
      console.Show();
      console.CriticalLn( "<end><cbr><br>*** Error: SpectrophotometricColorCalibration: "
                  "Decoding filters spectrum database file: " + what + ": " + filePath );
   }
   catch ( ... )
   {
      try
      {
         throw;
      }
      ERROR_HANDLER
   }

   return SampledSpectrumDataList();
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationProcess::ExportWhiteReferencesDatabase( const String& filePath, const SampledSpectrumDataList& whiteReferences )
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

void SpectrophotometricColorCalibrationProcess::ExportFiltersDatabase( const String& filePath, const SampledSpectrumDataList& filters )
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

SampledSpectrumData SpectrophotometricColorCalibrationProcess::DecodeWhiteReferenceCSV( const IsoStringList& lines )
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

SampledSpectrumData SpectrophotometricColorCalibrationProcess::DecodeFilterCSV( const IsoStringList& lines )
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

IsoStringList SpectrophotometricColorCalibrationProcess::EncodeWhiteReferenceCSV( const SampledSpectrumData& whiteReference )
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

IsoStringList SpectrophotometricColorCalibrationProcess::EncodeFilterCSV( const SampledSpectrumData& filter )
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
// ----------------------------------------------------------------------------

static String                  s_whiteReferencesDatabaseFilePath;
static String                  s_filtersDatabaseFilePath;
static SampledSpectrumDataList s_whiteReferences;
static SampledSpectrumData     s_defaultWhiteReference;
static SampledSpectrumDataList s_filters;
static SampledSpectrumData     s_defaultRedFilter;
static SampledSpectrumData     s_defaultGreenFilter;
static SampledSpectrumData     s_defaultBlueFilter;
static SampledSpectrumData     s_defaultDeviceQE;
static bool                    s_initialized = false;
static bool                    s_valid = false;

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationProcess::LoadPreferences()
{
   Settings::Read( "WhiteReferencesDatabaseFilePath", s_whiteReferencesDatabaseFilePath );
   Settings::Read( "FiltersDatabaseFilePath", s_filtersDatabaseFilePath );
}

// ----------------------------------------------------------------------------

void SpectrophotometricColorCalibrationProcess::SavePreferences()
{
   if ( s_whiteReferencesDatabaseFilePath.IsEmpty() )
      Settings::Remove( "WhiteReferencesDatabaseFilePath" );
   else
      Settings::Write( "WhiteReferencesDatabaseFilePath", s_whiteReferencesDatabaseFilePath );

   if ( s_filtersDatabaseFilePath.IsEmpty() )
      Settings::Remove( "FiltersDatabaseFilePath" );
   else
      Settings::Write( "FiltersDatabaseFilePath", s_filtersDatabaseFilePath );
}

// ----------------------------------------------------------------------------

bool SpectrophotometricColorCalibrationProcess::ImportWhiteReferences( const String& filePath, bool merge )
{
   SampledSpectrumDataList whiteReferences = LoadWhiteReferences( filePath );
   if ( !whiteReferences.IsEmpty() )
   {
      ImportWhiteReferences( whiteReferences, merge );
      return true;
   }
   return false;
}

void SpectrophotometricColorCalibrationProcess::ImportWhiteReferences( const SampledSpectrumDataList& whiteReferences, bool merge )
{
   if ( whiteReferences.IsEmpty() )
      throw Error( "No white reference definitions available" );

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
      if ( !s_defaultWhiteReference.IsValid() )
         throw Error( "Unable to determine a default white reference" );

   if ( merge )
      for ( const SampledSpectrumData& whiteReference : whiteReferences )
         s_whiteReferences << whiteReference;
   else
      s_whiteReferences = whiteReferences;

   if ( defaultWhiteReference.IsValid() )
      s_defaultWhiteReference = defaultWhiteReference;
   else if ( FindFilterByName( s_defaultWhiteReference.name ) < 0 )
      s_whiteReferences << s_defaultWhiteReference;

   // Ensure that we have only a white reference marked as default.
   for ( SampledSpectrumData& whiteReference : s_whiteReferences )
      if ( !whiteReference.defaultItem.IsEmpty() )
         if ( whiteReference.name != s_defaultWhiteReference.name )
            whiteReference.defaultItem.Clear();

   s_whiteReferences.Sort();
}

// ----------------------------------------------------------------------------

bool SpectrophotometricColorCalibrationProcess::ImportFilters( const String& filePath, bool merge )
{
   SampledSpectrumDataList filters = LoadFilters( filePath );
   if ( !filters.IsEmpty() )
   {
      ImportFilters( filters, merge );
      return true;
   }
   return false;
}

void SpectrophotometricColorCalibrationProcess::ImportFilters( const SampledSpectrumDataList& filters, bool merge )
{
   if ( filters.IsEmpty() )
      throw Error( "No filter definitions available" );

   for ( const SampledSpectrumData& filter : filters )
      if ( !filter.IsValid() )
         throw Error( "Invalid filter definition: '" + filter.name + "'" );

   SampledSpectrumData defaultRedFilter;
   for ( const SampledSpectrumData& filter : filters )
      if ( filter.defaultItem == "R" )
      {
         defaultRedFilter = filter;
         break;
      }
   if ( !defaultRedFilter.IsValid() )
      if ( !s_defaultRedFilter.IsValid() )
         throw Error( "Unable to determine a default red filter" );

   SampledSpectrumData defaultGreenFilter;
   for ( const SampledSpectrumData& filter : filters )
      if ( filter.defaultItem == "G" )
      {
         defaultGreenFilter = filter;
         break;
      }
   if ( !defaultGreenFilter.IsValid() )
      if ( !s_defaultGreenFilter.IsValid() )
         throw Error( "Unable to determine a default green filter" );

   SampledSpectrumData defaultBlueFilter;
   for ( const SampledSpectrumData& filter : filters )
      if ( filter.defaultItem == "B" )
      {
         defaultBlueFilter = filter;
         break;
      }
   if ( !defaultBlueFilter.IsValid() )
      if ( !s_defaultBlueFilter.IsValid() )
         throw Error( "Unable to determine a default blue filter" );

   SampledSpectrumData defaultDeviceQE;
   for ( const SampledSpectrumData& filter : filters )
      if ( filter.defaultItem == "Q" )
      {
         defaultDeviceQE = filter;
         break;
      }
   if ( !defaultDeviceQE.IsValid() )
      if ( !s_defaultDeviceQE.IsValid() )
         throw Error( "Unable to determine a default device QE curve" );

   if ( merge )
      for ( const SampledSpectrumData& filter : filters )
         s_filters << filter;
   else
      s_filters = filters;

   if ( defaultRedFilter.IsValid() )
      s_defaultRedFilter = defaultRedFilter;
   else if ( FindFilterByName( s_defaultRedFilter.name ) < 0 )
      s_filters << s_defaultRedFilter;

   if ( defaultGreenFilter.IsValid() )
      s_defaultGreenFilter = defaultGreenFilter;
   else if ( FindFilterByName( s_defaultGreenFilter.name ) < 0 )
      s_filters << s_defaultGreenFilter;

   if ( defaultBlueFilter.IsValid() )
      s_defaultBlueFilter = defaultBlueFilter;
   else if ( FindFilterByName( s_defaultBlueFilter.name ) < 0 )
      s_filters << s_defaultBlueFilter;

   if ( defaultDeviceQE.IsValid() )
      s_defaultDeviceQE = defaultDeviceQE;
   else if ( FindFilterByName( s_defaultDeviceQE.name ) < 0 )
      s_filters << s_defaultDeviceQE;

   // Ensure that we have only a filter marked as default for each R,G,B,Q.
   for ( SampledSpectrumData& filter : s_filters )
      if ( !filter.defaultItem.IsEmpty() )
         if ( filter.defaultItem == "R" && filter.name != s_defaultRedFilter.name
           || filter.defaultItem == "G" && filter.name != s_defaultGreenFilter.name
           || filter.defaultItem == "B" && filter.name != s_defaultBlueFilter.name
           || filter.defaultItem == "Q" && filter.name != s_defaultDeviceQE.name )
            filter.defaultItem.Clear();

   s_filters.Sort();
}

// ----------------------------------------------------------------------------

bool SpectrophotometricColorCalibrationProcess::InitializeSpectrumData( const String& a_whiteReferencesDatabaseFilePath,
                                                                        const String& a_filtersDatabaseFilePath )
{
   s_whiteReferences.Clear();
   s_filters.Clear();
   s_defaultWhiteReference = s_defaultRedFilter = s_defaultGreenFilter = s_defaultBlueFilter = s_defaultDeviceQE = SampledSpectrumData();
   s_initialized = true;

   LoadPreferences();

   String whiteReferencesDatabaseFilePath = a_whiteReferencesDatabaseFilePath;
   if ( whiteReferencesDatabaseFilePath.IsEmpty() )
   {
      whiteReferencesDatabaseFilePath = s_whiteReferencesDatabaseFilePath;
      if ( whiteReferencesDatabaseFilePath.IsEmpty() )
         whiteReferencesDatabaseFilePath = DefaultWhiteReferencesDatabaseFilePath();
   }

   String filtersDatabaseFilePath = a_filtersDatabaseFilePath;
   if ( filtersDatabaseFilePath.IsEmpty() )
   {
      filtersDatabaseFilePath = s_filtersDatabaseFilePath;
      if ( filtersDatabaseFilePath.IsEmpty() )
         filtersDatabaseFilePath = DefaultFiltersDatabaseFilePath();
   }

   try
   {
      if ( ImportWhiteReferences( whiteReferencesDatabaseFilePath ) )
         if ( ImportFilters( filtersDatabaseFilePath ) )
         {
            s_whiteReferencesDatabaseFilePath = whiteReferencesDatabaseFilePath;
            s_filtersDatabaseFilePath = filtersDatabaseFilePath;
            return s_valid = true;
         }
   }
   ERROR_HANDLER

   // In case of error, reset everything to the last known valid state.
   if ( s_valid )
   {
      s_valid = false; // prevent potential infinite recursion
      InitializeSpectrumData();
   }
   return false;
}

bool SpectrophotometricColorCalibrationProcess::HasValidSpectrumData()
{
   return s_valid;
}

const SampledSpectrumDataList& SpectrophotometricColorCalibrationProcess::WhiteReferences()
{
   return s_whiteReferences;
}

const SampledSpectrumData& SpectrophotometricColorCalibrationProcess::DefaultWhiteReference()
{
   return s_defaultWhiteReference;
}

const SampledSpectrumDataList& SpectrophotometricColorCalibrationProcess::Filters()
{
   return s_filters;
}

const SampledSpectrumData& SpectrophotometricColorCalibrationProcess::DefaultRedFilter()
{
   return s_defaultRedFilter;
}

const SampledSpectrumData& SpectrophotometricColorCalibrationProcess::DefaultGreenFilter()
{
   return s_defaultGreenFilter;
}

const SampledSpectrumData& SpectrophotometricColorCalibrationProcess::DefaultBlueFilter()
{
   return s_defaultBlueFilter;
}

const SampledSpectrumData& SpectrophotometricColorCalibrationProcess::DefaultDeviceQE()
{
   return s_defaultDeviceQE;
}

// ----------------------------------------------------------------------------

String SpectrophotometricColorCalibrationProcess::WhiteReferencesDatabaseFilePath()
{
   return s_whiteReferencesDatabaseFilePath.IsEmpty() ? DefaultWhiteReferencesDatabaseFilePath() : s_whiteReferencesDatabaseFilePath;
}

String SpectrophotometricColorCalibrationProcess::FiltersDatabaseFilePath()
{
   return s_filtersDatabaseFilePath.IsEmpty() ? DefaultFiltersDatabaseFilePath() : s_filtersDatabaseFilePath;
}

String SpectrophotometricColorCalibrationProcess::DefaultWhiteReferencesDatabaseFilePath()
{
   return PixInsightSettings::GlobalString( "Application/LibraryDirectory" ) + "/white-references.xspd";
}

String SpectrophotometricColorCalibrationProcess::DefaultFiltersDatabaseFilePath()
{
   return PixInsightSettings::GlobalString( "Application/LibraryDirectory" ) + "/filters.xspd";
}

bool SpectrophotometricColorCalibrationProcess::UsingDefaultWhiteReferencesDatabase()
{
   return s_whiteReferencesDatabaseFilePath.IsEmpty() || s_whiteReferencesDatabaseFilePath == DefaultWhiteReferencesDatabaseFilePath();
}

bool SpectrophotometricColorCalibrationProcess::UsingDefaultFiltersDatabase()
{
   return s_filtersDatabaseFilePath.IsEmpty() || s_filtersDatabaseFilePath == DefaultFiltersDatabaseFilePath();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SpectrophotometricColorCalibrationProcess.cpp - Released 2023-09-14T17:02:22Z
