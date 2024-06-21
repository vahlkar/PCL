//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.6.0
// ----------------------------------------------------------------------------
// IntegrationCache.cpp - Released 2024-06-18T15:49:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#include "IntegrationCache.h"

#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IntegrationCache* TheIntegrationCache = nullptr;

// ----------------------------------------------------------------------------

static IsoStringList SerializedTwoSidedEstimates( const IntegrationFile::scale_estimates& v )
{
   IsoStringList tokens;
   tokens << IsoString( v.Length() );
   for ( int i = 0; i < v.Length(); ++i )
      tokens << IsoString().Format( "%.8e,%.8e", v[i].low, v[i].high );
   return tokens;
}

static bool DeserializeTwoSidedEstimates( IntegrationFile::scale_estimates& v,
                                          IsoStringList::const_iterator& i, const IsoStringList& s )
{
   if ( i == s.End() )
      return false;
   int n = i->ToInt();
   if ( n < 0 || s.End() - i <= n )
      return false;
   ++i;
   v = IntegrationFile::scale_estimates( n );
   for ( int j = 0; j < n; ++j, ++i )
   {
      IsoStringList tokens;
      i->Break( tokens, ',' );
      if ( tokens.Length() != 2 )
         throw ParseError( "Parsing two-sided scale estimate: wrong number of components" );
      v[j].low = tokens[0].ToDouble();
      v[j].high = tokens[1].ToDouble();
   }
   return true;
}

// ----------------------------------------------------------------------------

static IsoStringList SerializedSignalEstimates( const IntegrationFile::signal_estimates& v )
{
   IsoStringList tokens;
   tokens << IsoString( v.Length() );
   for ( int i = 0; i < v.Length(); ++i )
      tokens << IsoString().Format( "%.8e,%.8e,%.8e,%.8e,%.8e,%.8e,%d",
                                    v[i].totalFlux, v[i].totalPowerFlux,
                                    v[i].totalMeanFlux, v[i].totalMeanPowerFlux,
                                    v[i].MStar, v[i].NStar, v[i].count );
   return tokens;
}

static bool DeserializeSignalEstimates( IntegrationFile::signal_estimates& v,
                                        IsoStringList::const_iterator& i, const IsoStringList& s )
{
   if ( i == s.End() )
      return false;
   int n = i->ToInt();
   if ( n < 0 || s.End() - i <= n )
      return false;
   ++i;
   v = IntegrationFile::signal_estimates( n );
   for ( int j = 0; j < n; ++j, ++i )
   {
      IsoStringList tokens;
      i->Break( tokens, ',' );
      if ( tokens.Length() != 7 )
         throw ParseError( "Parsing signal estimate: wrong number of components" );
      v[j].totalFlux = tokens[0].ToDouble();
      v[j].totalPowerFlux = tokens[1].ToDouble();
      v[j].totalMeanFlux = tokens[2].ToDouble();
      v[j].totalMeanPowerFlux = tokens[3].ToDouble();
      v[j].MStar = tokens[4].ToDouble();
      v[j].NStar = tokens[5].ToDouble();
      v[j].count = Max( 0, int( tokens[6].ToInt() ) );
   }
   return true;
}

// ----------------------------------------------------------------------------

void IntegrationCacheItem::AssignData( const FileDataCacheItem& item )
{
#define src static_cast<const IntegrationCacheItem&>( item )
   mean       = src.mean;
   median     = src.median;
   avgDev     = src.avgDev;
   mad        = src.mad;
   bwmv       = src.bwmv;
   psfSignal  = src.psfSignal;
   noise      = src.noise;
   noiseScale = src.noiseScale;
   ax         = src.ax;
   ay         = src.ay;
   am         = src.am;
   as0_avgDev = src.as0_avgDev;
   as1_avgDev = src.as1_avgDev;
   as0_mad    = src.as0_mad;
   as1_mad    = src.as1_mad;
   as0_bwmv   = src.as0_bwmv;
   as1_bwmv   = src.as1_bwmv;
   metadata   = src.metadata;
#undef src
}

// ----------------------------------------------------------------------------

IsoStringList IntegrationCacheItem::SerializedData() const
{
   IsoStringList tokens;
   if ( !mean.IsEmpty() )
      tokens << "mean" << SerializedVector( mean );
   if ( !median.IsEmpty() )
      tokens << "median" << SerializedVector( median );
   if ( !avgDev.IsEmpty() )
      tokens << "avgDev" << SerializedTwoSidedEstimates( avgDev );
   if ( !mad.IsEmpty() )
      tokens << "mad" << SerializedTwoSidedEstimates( mad );
   if ( !bwmv.IsEmpty() )
      tokens << "bwmv" << SerializedTwoSidedEstimates( bwmv );
   if ( !psfSignal.IsEmpty() )
      tokens << "psfSignal" << SerializedSignalEstimates( psfSignal );
   if ( !noise.IsEmpty() )
      tokens << "noise" << SerializedVector( noise );
   if ( !noiseScale.IsEmpty() )
      tokens << "noiseScale" << SerializedTwoSidedEstimates( noiseScale );
   if ( !ax.IsEmpty() )
      tokens << "ax" << SerializedVector( ax );
   if ( !ay.IsEmpty() )
      tokens << "ay" << SerializedVector( ay );
   if ( !am.IsEmpty() )
      tokens << "am" << SerializedMultiVector( am );
   if ( !as0_avgDev.IsEmpty() )
      tokens << "as0_avgDev" << SerializedMultiVector( as0_avgDev );
   if ( !as1_avgDev.IsEmpty() )
      tokens << "as1_avgDev" << SerializedMultiVector( as1_avgDev );
   if ( !as0_mad.IsEmpty() )
      tokens << "as0_mad" << SerializedMultiVector( as0_mad );
   if ( !as1_mad.IsEmpty() )
      tokens << "as1_mad" << SerializedMultiVector( as1_mad );
   if ( !as0_bwmv.IsEmpty() )
      tokens << "as0_bwmv" << SerializedMultiVector( as0_bwmv );
   if ( !as1_bwmv.IsEmpty() )
      tokens << "as1_bwmv" << SerializedMultiVector( as1_bwmv );
   if ( !metadata.IsEmpty() )
      tokens << "metadata" << metadata.ToUTF8();
   return tokens;
}

// ----------------------------------------------------------------------------

bool IntegrationCacheItem::DeserializeData( const IsoStringList& tokens )
{
   AssignData( IntegrationCacheItem() );

   for ( IsoStringList::const_iterator i = tokens.Begin(); i != tokens.End(); )
      if ( *i == "mean" )
      {
         if ( !DeserializeVector( mean, ++i, tokens ) )
            return false;
      }
      else if ( *i == "median" )
      {
         if ( !DeserializeVector( median, ++i, tokens ) )
            return false;
      }
      else if ( *i == "avgDev" )
      {
         if ( !DeserializeTwoSidedEstimates( avgDev, ++i, tokens ) )
            return false;
      }
      else if ( *i == "mad" )
      {
         if ( !DeserializeTwoSidedEstimates( mad, ++i, tokens ) )
            return false;
      }
      else if ( *i == "bwmv" )
      {
         if ( !DeserializeTwoSidedEstimates( bwmv, ++i, tokens ) )
            return false;
      }
      else if ( *i == "psfSignal" )
      {
         if ( !DeserializeSignalEstimates( psfSignal, ++i, tokens ) )
            return false;
      }
      else if ( *i == "noise" )
      {
         if ( !DeserializeVector( noise, ++i, tokens ) )
            return false;
      }
      else if ( *i == "noiseScale" )
      {
         if ( !DeserializeTwoSidedEstimates( noiseScale, ++i, tokens ) )
            return false;
      }
      else if ( *i == "ax" )
      {
         if ( !DeserializeVector( ax, ++i, tokens ) )
            return false;
      }
      else if ( *i == "ay" )
      {
         if ( !DeserializeVector( ay, ++i, tokens ) )
            return false;
      }
      else if ( *i == "am" )
      {
         if ( !DeserializeMultiVector( am, ++i, tokens ) )
            return false;
      }
      else if ( *i == "as0_avgDev" )
      {
         if ( !DeserializeMultiVector( as0_avgDev, ++i, tokens ) )
            return false;
      }
      else if ( *i == "as1_avgDev" )
      {
         if ( !DeserializeMultiVector( as1_avgDev, ++i, tokens ) )
            return false;
      }
      else if ( *i == "as0_mad" )
      {
         if ( !DeserializeMultiVector( as0_mad, ++i, tokens ) )
            return false;
      }
      else if ( *i == "as1_mad" )
      {
         if ( !DeserializeMultiVector( as1_mad, ++i, tokens ) )
            return false;
      }
      else if ( *i == "as0_bwmv" )
      {
         if ( !DeserializeMultiVector( as0_bwmv, ++i, tokens ) )
            return false;
      }
      else if ( *i == "as1_bwmv" )
      {
         if ( !DeserializeMultiVector( as1_bwmv, ++i, tokens ) )
            return false;
      }
      else if ( *i == "metadata" )
      {
         if ( ++i == tokens.End() )
            return false;
         metadata = i->UTF8ToUTF16();
      }
      else
      {
         ++i;
      }

   return true;
}

// ----------------------------------------------------------------------------

IntegrationCache::IntegrationCache()
   : FileDataCache( "ImageIntegration" )
{
   if ( TheIntegrationCache == nullptr )
      TheIntegrationCache = this;

   int maxItemDuration = 30;
   Settings::Read( "/Cache/MaxItemDuration", maxItemDuration );
   SetMaxItemDuration( maxItemDuration );
   Settings::Read( "/Cache/Persistent", m_persistent );
}

// ----------------------------------------------------------------------------

IntegrationCache::~IntegrationCache()
{
   if ( TheIntegrationCache == this )
      TheIntegrationCache = nullptr;
}

// ----------------------------------------------------------------------------

void IntegrationCache::SetPersistent( bool persistent )
{
   if ( persistent != m_persistent )
   {
      if ( persistent )
         if ( IsEmpty() )
            Load();

      m_persistent = persistent;
      Settings::Write( "/Cache/Persistent", m_persistent );
   }
}

// ----------------------------------------------------------------------------

void IntegrationCache::SetMaxItemDuration( int days )
{
   if ( days != MaxItemDuration() )
   {
      FileDataCache::SetMaxItemDuration( days );
      Settings::Write( "/Cache/MaxItemDuration", MaxItemDuration() );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegrationCache.cpp - Released 2024-06-18T15:49:25Z
